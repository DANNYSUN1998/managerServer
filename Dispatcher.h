//调度对象，处于控制器层，接收用户界面层的请求并发送给相应业务层对象进行处理，同时运行一些调度策略
#pragma once

#pragma comment(lib, "WS2_32.lib")
#include <WinSock2.h>
#pragma comment(lib, "json_vc71_libmtd.lib")	// 记得配置 1.附加库目录 2.附加依赖项 3.运行库
#include <json.h>								// 记得配置 1.附加包含目录

#include "DataStructure.h"
#include "Room.h"
#include "ARMServer.h"
#include <map>
#include <set>
#include <string>

const bool DEBUG = false;

enum DStateType { setMode, ready, off };	// 调度对象状态：设置模式，就绪，关机
const double EPS = 1E-8;					// 用于浮点数判断相等

// 连接部分
const int SERVERPORT1 = 1206;				// 侦听顾客连接的端口
const int SERVERPORT2 = 8888;				// 侦听管理员、前台、经理连接的端口
const int BACKLOG = 100;					// 服务器队列容量
const int LENFIELDSIZE = 8;					// 表示JSON长度的消息的大小
const int MAXDATALENGTH = 1500;				// 接收缓冲区大小

// 顾客服务部分
enum OperType { Temp, FanSpeed};			// 顾客操作类型
const bool IS_SHOWQUEUE = true;				// 是否运行显示队列情况函数
const int SHOWQUEUE_T = 5;					// 显示队列情况函数的运行周期（秒）
const int TIMEUNIT = 60;

class Dispatcher { 
public:
	Dispatcher();
	~Dispatcher();

	// 连接部分
	int BindPort(const int port);											// 绑定到端口port上，返回套接字
	void BuildConnection();													// 建立TCP连接
	void AcceptClient(const int port, const int serverSock);				// 侦听端口port
	void RecvClient(const int port, const int serverSock, const int clientSock, char *clientAddr); // 接收端口port传来的消息
	int Analyze1(char* recvBuf, const int clientSock, char*clientAddr);		// 响应顾客操作
	int Analyze2(char* recvBuf, const int clientSock);						// 响应管理员、前台、经理操作

	DStateType GetState()const;

	// 顾客和管理员部分
	void ShowQueue(); // 当IS_SHOWQUEUE为真时，以SHOWQUEUE_T为周期，显示服务队列、等待队列中的房间

	void TimeSlot();
	void ChangeTempFanSpeed(const int roomId, const OperType operType, const float targetTemp, const FanSpeedType targetFanSpeed);					// 响应顾客调温调风的操作
	void StopServe(const int roomId, const RStateType state);				// 响应顾客关机 或达到预设温度后挂起 的操作
	void CustomerRequestFee(const int clientSock, const int roomId);		// 响应顾客查看费用的请求

	void GetAdministratorNeed(const int clientSock);						// 响应管理员查看各房间状态的请求
	int AdministratorInit(char *recvBuf);									// 响应管理员初始化主机参数的请求

	// ARM服务对象部分
	Json::Value ReturnRDR(std::string roomid);//返回某房间详单
	Json::Value ReturnInvoice(std::string roomid);//返回某房间账单
	Json::Value ReturnReport(std::vector<std::string>ListRoomId, int ReportType);//返回报表


private:
	int MAX_CUSTOMERSERVER_NUMBER = 3;			// 最大顾客服务对象数量
	int TIMESLOT = 1;							// 调度时间片（秒）
	int WAIT_TIMESLOT = 120;					// 等待时间片（秒）
	float ARRANGE = 1.;							// 达到预设温度后的忍受范围
	int ADJUST_TEMP_METHOD2 = 1;				// 1按初始回温2按0-50回温
	std::vector<float> RATE1 = { 0, 0.33, 0.66,0.99 };	// 调温变化速率（℃/分钟）
	float RATE2 = 0.5;							// 回温变化速率（℃/分钟）
	int isImmidiatelyDispatch = 2;				

	std::vector<int>T{ 0,60,120,180 };			// 温度改变周期
	float FeeRate = 1.;							// 元/℃
	
	DStateType state;							// 调度对象状态
	bool isClose;								// 管理员是否执行了关闭主机的操作

	MODETYPE mode;								// 主机模式
	int tempHighLimit;							// 最低温度限制
	int tempLowLimit;							// 最高温度限制
	float defaultTargetTemp;					// 默认目标温度
	FanSpeedType defaultFanSpeed = FanSpeed_M;	// 默认目标风速

	// 顾客服务对象部分
	std::vector<Room*>servingQueue;				// 服务队列
	std::vector<Room*>waitingQueue;				// 等待队列
	std::map<int, Room*>roomInfo;				// 各房间（某顾客入住到退房期间）信息
	std::set<int>roomIds;						// 开过房的所有房间号
	
	// ARM服务对象部分
	ARMServer armserver;
};

void SendJson(const Json::Value&root, const int clientSock);