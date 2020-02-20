#pragma once
#include<iostream>
#include<string>
#include<vector>
#include <time.h>
#include"Bean.h"
//using namespace std;

//std::string dateToday = "1998/02/27";
//�굥���ݽṹ


/*//�˵����ݽṹ
typedef struct invoice
{
	std::string room_id;
	vector<Detail>Invoice;
	float total_fee;
}Invoice;*/

//��������ı���


class DBFacade
{
public://1998/02/29

	DBFacade();
	~DBFacade();

	void newRoom(std::string room_id, std::string date, float cur_temp, float target_temp);      //���뷿��ŵ�½

	//��Para�Ĳ���
	int setPara(float feeRate_L, float feeRate_M, float feeRate_H, int mode, float default_target_temp, float temp_LowLimit, float temp_HighLimit);//�������ò���,�ɹ�����1��ʧ�ܷ���0

	//��cur_state�Ĳ���
	void setRoomCurState(std::string room_id, int on_off, int hang, int serve, float cur_temp, float target_temp, int cur_fan, int target_fan); //�ڱ�cur_state�޸ĵ�ǰ�����״̬

	//��record�Ĳ���
	void setRecord_On_Off(std::string room_id, std::string dateToday);  //�û������Դ˿յ�����¼��record��

	void setRecord_ChangeFan(std::string room_id, std::string dateToday);//�û��������٣���¼��record��

	void setRecord_ChangeTemp(std::string room_id, std::string dateToday);//�û������¶ȣ���¼��record��

	void setRecord_dispatch_amount(std::string room_id, std::string dateToday, int change);//�յ����������б�����һ�Σ����ȴ�����1����¼��record��

	void setRecord_detail_amount(std::string room_id, std::string dateToday);  //����һ���µ��굥����¼��record�У�������1

	void setRecord_total_fee(std::string room_id, std::string dateToday, float fee);//�����µķ��ã���¼��record��,fee��Ҫ���ӵĽ����

	void setRecord_serve_time(std::string room_id, std::string dateToday, int time);//���·����ѷ����ʱ��


	Room_record * getRoomRecord(std::string room_id, int reportType, std::string end_time);//���ص���������start_time��end_timeʱ����ڵı���


	//��Detail�Ĳ���
	void newDetail(std::string room_id, std::string requestTime, int fan, float feeRate);     //�����µ��굥

	void endDetail(std::string room_id, std::string endTime, float detailFee);     //һ���굥����

	Detail * getRoomDetail(std::string room_id);   //���ص���������start_time��end_timeʱ����ڵ������굥�������ڵ�ȡ�˵����굥

	//��regist�Ĳ���
	void signIn(std::string room_id, std::string signInTime);     //�Ǽ���ס�������µ�regist��¼

	void setRegist_total_fee(std::string room_id, float fee);//�����µķ��ã���¼��regist��,fee��Ҫ���ӵĽ����.ǰ�ᣬ�÷����������ס״̬�����޸ġ�

	void setRegist_signOut(std::string room_id, std::string signOutTime);

	Room_Invoice* getInvoice(std::string room_id);  //�˷��������ܽ���˵��������ȵ���sign_out,�ٵ���getRoomDetail

	float getCurRoomFee(std::string room_id);        //���ص�ǰ����Ŀյ�����

	void initDB();
	//�·������Ϣ�Ĳ���

	int checkRoom_id(std::string room_id);//1�����˷�  0��δ������  -1���Ҵ���

private:
	Bean B;


};

extern DBFacade dbfacade;

extern std::string getDate(bool flag);					// ��ȡ��ǰϵͳʱ�䣺flagΪ�淵��"��/��/�� ʱ:��:��"��Ϊ�ٷ���"��/��/��"
extern std::string getRoomIdString(const int roomId);	// ��int�ͷ����ת����string�ͣ�r_**������Ų����� 