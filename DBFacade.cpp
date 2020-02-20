#include "DBFacade.h"

DBFacade dbfacade;

DBFacade::DBFacade()
{
}


DBFacade::~DBFacade()
{
}

int DBFacade::setPara(float feeRate_L, float feeRate_M, float feeRate_H, int mode, float default_target_temp, float temp_LowLimit, float temp_HighLimit)//�������ÿյ���������
{
	if (B.updatePara(feeRate_L, feeRate_M, feeRate_H, mode, default_target_temp, temp_LowLimit, temp_HighLimit) == true)
		return 1;
	else
		return 0;
}


void DBFacade::setRoomCurState(std::string room_id, int on_off, int hang, int serve, float cur_temp, float target_temp, int cur_fan, int target_fan)
{
	if (B.update_Cur_State(room_id, on_off, hang, serve, cur_temp, target_temp, cur_fan, target_fan) == true)
	{
		std::cout << "DB:�޸ķ���" << room_id << "״̬�ɹ�" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB:�޸ķ���" << room_id << "״̬ʧ��" << std::endl;
		std::cout << "�������" << std::endl;
		exit(1);
	}
}

void DBFacade::setRecord_On_Off(std::string room_id, std::string dateToday)  //�û������Դ˿յ�����¼��record��
{
	if (B.update_report_powerOn_Off(room_id, dateToday) == true)
	{
		std::cout << "DB������" << room_id << "�����޸ĳɹ�" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB������" << room_id << "�����޸�ʧ��" << std::endl << "�����˳�" << std::endl;
		exit(1);
	}
}

void DBFacade::setRecord_ChangeFan(std::string room_id, std::string dateToday)
{
	if (B.update_report_changeFan(room_id, dateToday) == true)
	{
		std::cout << "DB������" << room_id << "�����޸ĳɹ�" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB������" << room_id << "�����޸�ʧ��" << std::endl << "�����˳�" << std::endl;
		exit(1);
	}
}

void DBFacade::setRecord_ChangeTemp(std::string room_id, std::string dateToday)
{
	if (B.update_report_changeTemp(room_id, dateToday) == true)
	{
		std::cout << "DB������" << room_id << "�����޸ĳɹ�" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB������" << room_id << "�����޸�ʧ��" << std::endl << "�����˳�" << std::endl;
		exit(1);
	}
}

void DBFacade::setRecord_dispatch_amount(std::string room_id, std::string dateToday, int change)
{
	if (B.update_report_dispatch_amount(room_id, dateToday, change) == true)
	{
		std::cout << "DB������" << room_id << "�����޸ĳɹ�" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB������" << room_id << "�����޸�ʧ��" << std::endl << "�����˳�" << std::endl;
		exit(1);
	}
}

void DBFacade::setRecord_detail_amount(std::string room_id, std::string dateToday)
{
	if (B.update_report_detail_amount(room_id, dateToday) == true)
	{
		std::cout << "DB������" << room_id << "�����޸ĳɹ�" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB������" << room_id << "�����޸�ʧ��" << std::endl << "�����˳�" << std::endl;
		exit(1);
	}
}

void DBFacade::setRecord_total_fee(std::string room_id, std::string dateToday, float fee)//���޸���
{
	if (B.update_report_total_fee(room_id, dateToday, fee) == true)
	{
		std::cout << "DB������" << room_id << "�����޸ĳɹ�" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB������" << room_id << "�����޸�ʧ��" << std::endl << "�����˳�" << std::endl;
		exit(1);
	}
}

void DBFacade::setRecord_serve_time(std::string room_id, std::string dateToday, int time)
{
	if (B.update_report_serve_time(room_id, dateToday, time) == true)
	{
		std::cout << "DB������" << room_id << "�����޸ĳɹ�" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB������" << room_id << "�����޸�ʧ��" << std::endl << "�����˳�" << std::endl;
		exit(1);
	}
}

void DBFacade::newDetail(std::string room_id, std::string requestTime, int fan, float feeRate)     //
{
	if (B.insert_detail_newDetail(room_id, requestTime, fan, feeRate) == true)
	{
		std::cout << "DB������" << room_id << "�����µ��굥�ɹ�" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB������" << room_id << "�����µ��굥ʧ��" << std::endl;

		getchar();
	}
}

void DBFacade::endDetail(std::string room_id, std::string endTime, float detailFee)
{
	if (B.update_Detail_endDetail(room_id, endTime, detailFee) == true)
	{
		std::cout << "DB������" << room_id << "�޸��굥�ɹ�" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB������" << room_id << "�޸��굥ʧ��" << std::endl;
		getchar();
	}
}

void DBFacade::signIn(std::string room_id, std::string signInTime)
{
	if (B.insert_Regist_signIn(room_id, signInTime) == true)
	{
		std::cout << "DB:����" << room_id << "�Ǽ���ס�ɹ���" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB:����" << room_id << "�Ǽ���ס�ɹ���" << std::endl;
		getchar();
		return;
	}
}

void DBFacade::setRegist_total_fee(std::string room_id, float fee)
{
	if (B.update_Curfee_Regist(room_id, fee) == true)
	{
		std::cout << "DB_regist:����" << room_id << "�޸ĵ�ǰ���ɹ�" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB_regist:����" << room_id << "�޸ĵ�ǰ���ʧ��" << std::endl;
		std::cout << "�����˳�" << std::endl;
		getchar();
		exit(1);
	}
}



void DBFacade::setRegist_signOut(std::string room_id, std::string signOutTime)
{
	if (B.setRoom_signOut(room_id, signOutTime) == false)
	{
		std::cout << "DB:����" << room_id << "�˷�ʧ��" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB:����" << room_id << "�˷��ɹ�" << std::endl;
		return;
	}
}


Room_Invoice* DBFacade::getInvoice(std::string room_id)//�˷������ܽ�����ʧ�ܷ���-1
{

	Room_Invoice * dataBack = B.updateAndselect_Regist_signOut(room_id);
	if (dataBack != NULL)
	{
		std::cout << "DB:����" << room_id << "�˷��ɹ����ܽ��Ϊ" << dataBack->total_fee << std::endl;
		return dataBack;
	}
	else
	{
		std::cout << "DB:����" << room_id << "�˷�ʧ��" << std::endl;
		return NULL;
	}
}


float DBFacade::getCurRoomFee(std::string room_id)            //���ص�ǰ����Ŀյ��÷��ã��˴��ķ��������δ�˷���
{
	float returnFee;
	if (B.selectCurFee_Regist(room_id, &returnFee) == true)
		return returnFee;
	else
		std::cout << "���ҵ�ǰ����ʧ�ܣ�" << std::endl;
}

Room_record * DBFacade::getRoomRecord(std::string room_id, int reportType, std::string end_time)//���ص���������start_time��end_timeʱ����ڵı���
{
	Room_record * dataList = NULL;
	if ((dataList = B.selectRoomReport_Report(room_id, reportType, end_time)) == NULL)
	{
		std::cout << "DB:��ȡ����" << room_id << "����ʧ��" << std::endl;
		return NULL;
	}
	else
	{
		std::cout << "DB:��ȡ����" << room_id << "����ɹ�" << std::endl;
		Room_record * feedBack = new Room_record;
		feedBack->next = NULL;
		feedBack->room_id = room_id;
		Room_record *ptr = dataList;
		while (ptr != NULL)
		{
			feedBack->amonut_detail += ptr->amonut_detail;
			feedBack->amount_changeFan += ptr->amount_changeFan;
			feedBack->amount_changeTemp += ptr->amount_changeTemp;
			feedBack->amount_dispatch += ptr->amount_dispatch;
			feedBack->amount_on_off += ptr->amount_on_off;
			feedBack->total_fee += ptr->total_fee;
			feedBack->using_time += ptr->using_time;
			ptr = ptr->next;
		}
		return feedBack;
	}


}


Detail * DBFacade::getRoomDetail(std::string room_id)//���ص���������start_time��end_timeʱ����ڵ������굥�������ڵ�ȡ�˵����굥
{
	Detail * return_DetailList = B.selectRoomDetail_Detail(room_id);

	if (return_DetailList != NULL)
	{
		std::cout << "DB:��ȡ����" << room_id << "�굥�ɹ�" << std::endl;
		return return_DetailList;
	}
	else
	{
		std::cout << "DB:��ȡ����" << room_id << "�굥ʧ�ܣ�" << std::endl;
		return NULL;
	}

}

void DBFacade::newRoom(std::string room_id, std::string date, float cur_temp, float target_temp)
{
	if (B.newRoom(room_id, date, cur_temp, target_temp) == NULL)
	{

	}
}

void DBFacade::initDB()
{
	B.initDatabase();
}

//********************************************************************************************

/* ��ȡ��ǰϵͳʱ�䣺flagΪ�淵��"��/��/�� ʱ:��:��"��Ϊ�ٷ���"��/��/��" */
std::string getDate(bool flag)
{
	std::string t_s = "";
	time_t rawtime = time(0);
	std::tm *t = localtime(&rawtime);
	t_s = std::to_string(t->tm_year + 1900) + '/' + std::to_string(t->tm_mon + 1) + '/' + std::to_string(t->tm_mday);
	if (flag)
		t_s += ' ' + std::to_string(t->tm_hour) + ':' + std::to_string(t->tm_min) + ':' + std::to_string(t->tm_sec);

	return t_s;
}
/* ��int�ͷ����ת����string�ͣ�r_**������Ų����� */
std::string getRoomIdString(const int roomId)
{
	std::string roomId_s = "";

	if (roomId >= 0 && roomId <= 9)
		roomId_s = "r_0" + std::to_string(roomId);
	else
		roomId_s = "r_" + std::to_string(roomId);

	return roomId_s;
}

int DBFacade::checkRoom_id(std::string room_id)
{
	int result = B.checkRoom_id(room_id);//1�����˷�  0��δ������  -1���Ҵ���
	if (result == -1)
	{
		std::cout << "���ҷ���" << room_id << "ʱ��������" << std::endl;
		return -1;
	}

	return result;
}