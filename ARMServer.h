#pragma once
#include <vector>
#include<iostream>
#include<string>
#include"DBFacade.h"
#include<WinSock2.h>
#include<time.h>
#include<windows.h>
#pragma comment( lib, "ws2_32.lib" )// ����Winsock2.h�ľ�̬���ļ� 

struct SingleRDR {//�����굥
	std::string RoomId;
	std::string RequestTime;
	int Duration;
	int FanSpeed;  //����
	float FeeRate;
	float Fee;
};

struct list_RDR {//�굥
	std::vector<SingleRDR> RDR_List;
};



struct list_invoice//�˵�
{
	std::string RoomID;
	float total_fee;
	std::string date_in;
	std::string date_out;
};

struct SingleReport {//�������䱨��
	std::string RoomId;
	std::string Date;
	int OnOffNum;        //���ش���
	int UsingTime;        //ʹ�ÿյ���ʱ�䣬 ��λ��int
	float total_fee;      //�ܷ���
	int DispatchNum;     // �����ȵĴ���
	int DetailNum;      // �굥��
	int ChangeTempNum;   // ���´���
	int ChangeFanSpeedNum; // �������
};

struct Report {//����
	int Report_Type;    //1Ϊ�ձ�2Ϊ�ܱ�3Ϊ�±�, 4Ϊ���
	std::vector<SingleReport> Report_List;
};



class ARMServer
	//����DB��Dis֮�䣬ͨ�������²�DB��select�ӿڻ�ȡ�������ݣ������struct���͸��Ϸ�Dispatcher
{
public:
	ARMServer();
	~ARMServer();
	list_RDR* CreateDetail(std::string roomid);//ǰ̨��ȡ�÷����굥
	list_invoice* CreateInvoice(std::string roomid);//ǰ̨��ȡ�÷����ͷ��β���˵�
	SingleReport* CreateReport(std::string RoomId, int ReportType);//�����ȡ����(���з���)
private:
};
