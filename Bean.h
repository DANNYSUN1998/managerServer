#pragma once
#include <stdio.h>     
#include <string>   
#include<iostream>



//detail表新detail的初始endTime
#define default_endTime "2200/01/1 00:00:00" //
#define default_dateOut "2200/01/1 00:00:00"





class Detail
{
public:

	std::string room_id;      //房间号
	char start_time[30];   //开始时间
	short int duration = 0;        //时长
	short int fan = -1;             //风速
	float fee = -1;           //费用
	float feeRate = -1;
	Detail* next = NULL;            //用于指向下一节点的指针
};

class Room_record
{
public:
	std::string room_id;
	char date[21] = "/0";
	int amount_on_off = 0;//开关次数
	int using_time = 0;//空调使用时间
	float total_fee = 0;  //总费用
	int amount_dispatch = 0;//调度次数
	int amonut_detail = 0;  //详单个数
	int amount_changeTemp = 0;//调温次数
	int amount_changeFan = 0;//调风次数
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

	bool applyHenv();              //申请环境句柄

	bool applyHdbc();              //申请连接句柄

	bool connectDatabase();        //连接数据源

	bool applyHstmt();             //申请语句句柄

	void freeHandleandConnect();   //释放句柄和连接

	bool updatePara(float feeRate_L, float feeRate_M, float feeRate_H, int mode, float default_target_temp, float temp_LowLimit, float temp_HighLimit);             //修改Para表的参数

	bool selectCurFee_Regist(std::string room_id, float * returnFee);  //get当前房间的费用

	bool update_Cur_State(std::string room_id, int on_off, int hang, int serve, float cur_temp, float target_temp, int cur_fan, int target_fan); //修改cur_state表

	bool update_report_powerOn_Off(std::string room_id, std::string curDate);

	bool update_report_changeFan(std::string room_id, std::string curDate);

	bool update_report_changeTemp(std::string room_id, std::string curDate);

	bool update_report_dispatch_amount(std::string room_id, std::string curDate, int change);

	bool update_report_detail_amount(std::string room_id, std::string curDate);

	bool update_report_total_fee(std::string room_id, std::string curDate, float fee);

	bool update_report_serve_time(std::string room_id, std::string curDate, int time);

	bool insert_detail_newDetail(std::string room_id, std::string requestTime, int fan, float feeRate);//产生新的详单，endTime设置为初始值default_endtime

	bool update_Detail_endDetail(std::string room_id, std::string endTime, float detailFee);             //当前的详单结束

	bool insert_Regist_signIn(std::string room_id, std::string signInTime);        //登记入住

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
