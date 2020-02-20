#pragma once
#include<iostream>
#include<string>
#include<vector>
#include <time.h>
#include"Bean.h"
//using namespace std;

//std::string dateToday = "1998/02/27";
//详单数据结构


/*//账单数据结构
typedef struct invoice
{
	std::string room_id;
	vector<Detail>Invoice;
	float total_fee;
}Invoice;*/

//单个房间的报表


class DBFacade
{
public://1998/02/29

	DBFacade();
	~DBFacade();

	void newRoom(std::string room_id, std::string date, float cur_temp, float target_temp);      //输入房间号登陆

	//表Para的操作
	int setPara(float feeRate_L, float feeRate_M, float feeRate_H, int mode, float default_target_temp, float temp_LowLimit, float temp_HighLimit);//调用设置参数,成功返回1，失败返回0

	//表cur_state的操作
	void setRoomCurState(std::string room_id, int on_off, int hang, int serve, float cur_temp, float target_temp, int cur_fan, int target_fan); //在表cur_state修改当前房间的状态

	//表record的操作
	void setRecord_On_Off(std::string room_id, std::string dateToday);  //用户开关以此空调，记录在record中

	void setRecord_ChangeFan(std::string room_id, std::string dateToday);//用户调整风速，记录在record中

	void setRecord_ChangeTemp(std::string room_id, std::string dateToday);//用户调整温度，记录在record中

	void setRecord_dispatch_amount(std::string room_id, std::string dateToday, int change);//空调在三个队列被调度一次，调度次数加1，记录在record中

	void setRecord_detail_amount(std::string room_id, std::string dateToday);  //产生一条新的详单，记录在record中，个数加1

	void setRecord_total_fee(std::string room_id, std::string dateToday, float fee);//产生新的费用，记录到record中,fee是要增加的金额数

	void setRecord_serve_time(std::string room_id, std::string dateToday, int time);//更新房间已服务的时间


	Room_record * getRoomRecord(std::string room_id, int reportType, std::string end_time);//返回单个房间在start_time到end_time时间段内的报表


	//表Detail的操作
	void newDetail(std::string room_id, std::string requestTime, int fan, float feeRate);     //产生新的详单

	void endDetail(std::string room_id, std::string endTime, float detailFee);     //一个详单结束

	Detail * getRoomDetail(std::string room_id);   //返回单个房间在start_time到end_time时间段内的所有详单，可用于调取账单和详单

	//表regist的操作
	void signIn(std::string room_id, std::string signInTime);     //登记入住，产生新的regist记录

	void setRegist_total_fee(std::string room_id, float fee);//产生新的费用，记录到regist中,fee是要增加的金额数.前提，该房间必须是入住状态才能修改。

	void setRegist_signOut(std::string room_id, std::string signOutTime);

	Room_Invoice* getInvoice(std::string room_id);  //退房，返回总金额账单，必须先调用sign_out,再调用getRoomDetail

	float getCurRoomFee(std::string room_id);        //返回当前房间的空调得用

	void initDB();
	//新房间的信息的插入

	int checkRoom_id(std::string room_id);//1：已退房  0：未开过房  -1查找错误

private:
	Bean B;


};

extern DBFacade dbfacade;

extern std::string getDate(bool flag);					// 获取当前系统时间：flag为真返回"年/月/日 时:分:秒"，为假返回"年/月/日"
extern std::string getRoomIdString(const int roomId);	// 将int型房间号转换成string型（r_**）房间号并返回 