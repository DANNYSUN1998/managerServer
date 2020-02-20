#pragma once
#include <vector>
#include<iostream>
#include<string>
#include"DBFacade.h"
#include<WinSock2.h>
#include<time.h>
#include<windows.h>
#pragma comment( lib, "ws2_32.lib" )// 链接Winsock2.h的静态库文件 

struct SingleRDR {//单个详单
	std::string RoomId;
	std::string RequestTime;
	int Duration;
	int FanSpeed;  //风速
	float FeeRate;
	float Fee;
};

struct list_RDR {//详单
	std::vector<SingleRDR> RDR_List;
};



struct list_invoice//账单
{
	std::string RoomID;
	float total_fee;
	std::string date_in;
	std::string date_out;
};

struct SingleReport {//单个房间报表
	std::string RoomId;
	std::string Date;
	int OnOffNum;        //开关次数
	int UsingTime;        //使用空调总时间， 单位：int
	float total_fee;      //总费用
	int DispatchNum;     // 被调度的次数
	int DetailNum;      // 详单数
	int ChangeTempNum;   // 调温次数
	int ChangeFanSpeedNum; // 调风次数
};

struct Report {//报表
	int Report_Type;    //1为日表，2为周表，3为月表, 4为年表
	std::vector<SingleReport> Report_List;
};



class ARMServer
	//夹在DB和Dis之间，通过调用下层DB的select接口获取所需数据，打包成struct发送给上方Dispatcher
{
public:
	ARMServer();
	~ARMServer();
	list_RDR* CreateDetail(std::string roomid);//前台获取该房间详单
	list_invoice* CreateInvoice(std::string roomid);//前台获取该房间从头到尾的账单
	SingleReport* CreateReport(std::string RoomId, int ReportType);//经理获取报表(所有房间)
private:
};
