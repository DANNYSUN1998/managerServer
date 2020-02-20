//���ȶ��󣬴��ڿ������㣬�����û����������󲢷��͸���Ӧҵ��������д���ͬʱ����һЩ���Ȳ���
#pragma once

#pragma comment(lib, "WS2_32.lib")
#include <WinSock2.h>
#pragma comment(lib, "json_vc71_libmtd.lib")	// �ǵ����� 1.���ӿ�Ŀ¼ 2.���������� 3.���п�
#include <json.h>								// �ǵ����� 1.���Ӱ���Ŀ¼

#include "DataStructure.h"
#include "Room.h"
#include "ARMServer.h"
#include <map>
#include <set>
#include <string>

const bool DEBUG = false;

enum DStateType { setMode, ready, off };	// ���ȶ���״̬������ģʽ���������ػ�
const double EPS = 1E-8;					// ���ڸ������ж����

// ���Ӳ���
const int SERVERPORT1 = 1206;				// �����˿����ӵĶ˿�
const int SERVERPORT2 = 8888;				// ��������Ա��ǰ̨���������ӵĶ˿�
const int BACKLOG = 100;					// ��������������
const int LENFIELDSIZE = 8;					// ��ʾJSON���ȵ���Ϣ�Ĵ�С
const int MAXDATALENGTH = 1500;				// ���ջ�������С

// �˿ͷ��񲿷�
enum OperType { Temp, FanSpeed};			// �˿Ͳ�������
const bool IS_SHOWQUEUE = true;				// �Ƿ�������ʾ�����������
const int SHOWQUEUE_T = 5;					// ��ʾ��������������������ڣ��룩
const int TIMEUNIT = 60;

class Dispatcher { 
public:
	Dispatcher();
	~Dispatcher();

	// ���Ӳ���
	int BindPort(const int port);											// �󶨵��˿�port�ϣ������׽���
	void BuildConnection();													// ����TCP����
	void AcceptClient(const int port, const int serverSock);				// �����˿�port
	void RecvClient(const int port, const int serverSock, const int clientSock, char *clientAddr); // ���ն˿�port��������Ϣ
	int Analyze1(char* recvBuf, const int clientSock, char*clientAddr);		// ��Ӧ�˿Ͳ���
	int Analyze2(char* recvBuf, const int clientSock);						// ��Ӧ����Ա��ǰ̨���������

	DStateType GetState()const;

	// �˿ͺ͹���Ա����
	void ShowQueue(); // ��IS_SHOWQUEUEΪ��ʱ����SHOWQUEUE_TΪ���ڣ���ʾ������С��ȴ������еķ���

	void TimeSlot();
	void ChangeTempFanSpeed(const int roomId, const OperType operType, const float targetTemp, const FanSpeedType targetFanSpeed);					// ��Ӧ�˿͵��µ���Ĳ���
	void StopServe(const int roomId, const RStateType state);				// ��Ӧ�˿͹ػ� ��ﵽԤ���¶Ⱥ���� �Ĳ���
	void CustomerRequestFee(const int clientSock, const int roomId);		// ��Ӧ�˿Ͳ鿴���õ�����

	void GetAdministratorNeed(const int clientSock);						// ��Ӧ����Ա�鿴������״̬������
	int AdministratorInit(char *recvBuf);									// ��Ӧ����Ա��ʼ����������������

	// ARM������󲿷�
	Json::Value ReturnRDR(std::string roomid);//����ĳ�����굥
	Json::Value ReturnInvoice(std::string roomid);//����ĳ�����˵�
	Json::Value ReturnReport(std::vector<std::string>ListRoomId, int ReportType);//���ر���


private:
	int MAX_CUSTOMERSERVER_NUMBER = 3;			// ���˿ͷ����������
	int TIMESLOT = 1;							// ����ʱ��Ƭ���룩
	int WAIT_TIMESLOT = 120;					// �ȴ�ʱ��Ƭ���룩
	float ARRANGE = 1.;							// �ﵽԤ���¶Ⱥ�����ܷ�Χ
	int ADJUST_TEMP_METHOD2 = 1;				// 1����ʼ����2��0-50����
	std::vector<float> RATE1 = { 0, 0.33, 0.66,0.99 };	// ���±仯���ʣ���/���ӣ�
	float RATE2 = 0.5;							// ���±仯���ʣ���/���ӣ�
	int isImmidiatelyDispatch = 2;				

	std::vector<int>T{ 0,60,120,180 };			// �¶ȸı�����
	float FeeRate = 1.;							// Ԫ/��
	
	DStateType state;							// ���ȶ���״̬
	bool isClose;								// ����Ա�Ƿ�ִ���˹ر������Ĳ���

	MODETYPE mode;								// ����ģʽ
	int tempHighLimit;							// ����¶�����
	int tempLowLimit;							// ����¶�����
	float defaultTargetTemp;					// Ĭ��Ŀ���¶�
	FanSpeedType defaultFanSpeed = FanSpeed_M;	// Ĭ��Ŀ�����

	// �˿ͷ�����󲿷�
	std::vector<Room*>servingQueue;				// �������
	std::vector<Room*>waitingQueue;				// �ȴ�����
	std::map<int, Room*>roomInfo;				// �����䣨ĳ�˿���ס���˷��ڼ䣩��Ϣ
	std::set<int>roomIds;						// �����������з����
	
	// ARM������󲿷�
	ARMServer armserver;
};

void SendJson(const Json::Value&root, const int clientSock);