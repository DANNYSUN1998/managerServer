#pragma once
#include <stdio.h>     
#include <string>   
#include<iostream>



//detail����detail�ĳ�ʼendTime
#define default_endTime "2200/01/1 00:00:00" //
#define default_dateOut "2200/01/1 00:00:00"





class Detail
{
public:

	std::string room_id;      //�����
	char start_time[30];   //��ʼʱ��
	short int duration = 0;        //ʱ��
	short int fan = -1;             //����
	float fee = -1;           //����
	float feeRate = -1;
	Detail* next = NULL;            //����ָ����һ�ڵ��ָ��
};

class Room_record
{
public:
	std::string room_id;
	char date[21] = "/0";
	int amount_on_off = 0;//���ش���
	int using_time = 0;//�յ�ʹ��ʱ��
	float total_fee = 0;  //�ܷ���
	int amount_dispatch = 0;//���ȴ���
	int amonut_detail = 0;  //�굥����
	int amount_changeTemp = 0;//���´���
	int amount_changeFan = 0;//�������
	Room_record* next = NULL;
};

class Room_Invoice
{
public:
	std::string room_id;
	float total_fee;
	char date_in[21];
	char date_out[21];
};

class Bean
{
public:
	Bean();
	~Bean();

	bool applyHenv();              //���뻷�����

	bool applyHdbc();              //�������Ӿ��

	bool connectDatabase();        //��������Դ

	bool applyHstmt();             //���������

	void freeHandleandConnect();   //�ͷž��������

	bool updatePara(float feeRate_L, float feeRate_M, float feeRate_H, int mode, float default_target_temp, float temp_LowLimit, float temp_HighLimit);             //�޸�Para��Ĳ���

	bool selectCurFee_Regist(std::string room_id, float * returnFee);  //get��ǰ����ķ���

	bool update_Cur_State(std::string room_id, int on_off, int hang, int serve, float cur_temp, float target_temp, int cur_fan, int target_fan); //�޸�cur_state��

	bool update_report_powerOn_Off(std::string room_id, std::string curDate);

	bool update_report_changeFan(std::string room_id, std::string curDate);

	bool update_report_changeTemp(std::string room_id, std::string curDate);

	bool update_report_dispatch_amount(std::string room_id, std::string curDate, int change);

	bool update_report_detail_amount(std::string room_id, std::string curDate);

	bool update_report_total_fee(std::string room_id, std::string curDate, float fee);

	bool update_report_serve_time(std::string room_id, std::string curDate, int time);

	bool insert_detail_newDetail(std::string room_id, std::string requestTime, int fan, float feeRate);//�����µ��굥��endTime����Ϊ��ʼֵdefault_endtime

	bool update_Detail_endDetail(std::string room_id, std::string endTime, float detailFee);             //��ǰ���굥����

	bool insert_Regist_signIn(std::string room_id, std::string signInTime);        //�Ǽ���ס

	Room_Invoice* updateAndselect_Regist_signOut(std::string room_id);

	bool setRoom_signOut(std::string room_id, std::string signOutTime);

	bool update_Curfee_Regist(std::string room_id, float fee);

	Detail* selectRoomDetail_Detail(std::string room_id);

	Room_record* selectRoomReport_Report(std::string room_id, int reportType, std::string endDate);

	bool newRoom(std::string room_id, std::string date, float cur_temp, float target_temp);

	void initDatabase();

	int checkRoom_id(std::string checkRoom_id);

private:

};
