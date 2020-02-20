#include "DBFacade.h"

DBFacade dbfacade;

DBFacade::DBFacade()
{
}


DBFacade::~DBFacade()
{
}

int DBFacade::setPara(float feeRate_L, float feeRate_M, float feeRate_H, int mode, float default_target_temp, float temp_LowLimit, float temp_HighLimit)//调用设置空调工作参数
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
		std::cout << "DB:修改房间" << room_id << "状态成功" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB:修改房间" << room_id << "状态失败" << std::endl;
		std::cout << "程序错误！" << std::endl;
		exit(1);
	}
}

void DBFacade::setRecord_On_Off(std::string room_id, std::string dateToday)  //用户开关以此空调，记录在record中
{
	if (B.update_report_powerOn_Off(room_id, dateToday) == true)
	{
		std::cout << "DB：房间" << room_id << "报表修改成功" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB：房间" << room_id << "报表修改失败" << std::endl << "程序退出" << std::endl;
		exit(1);
	}
}

void DBFacade::setRecord_ChangeFan(std::string room_id, std::string dateToday)
{
	if (B.update_report_changeFan(room_id, dateToday) == true)
	{
		std::cout << "DB：房间" << room_id << "报表修改成功" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB：房间" << room_id << "报表修改失败" << std::endl << "程序退出" << std::endl;
		exit(1);
	}
}

void DBFacade::setRecord_ChangeTemp(std::string room_id, std::string dateToday)
{
	if (B.update_report_changeTemp(room_id, dateToday) == true)
	{
		std::cout << "DB：房间" << room_id << "报表修改成功" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB：房间" << room_id << "报表修改失败" << std::endl << "程序退出" << std::endl;
		exit(1);
	}
}

void DBFacade::setRecord_dispatch_amount(std::string room_id, std::string dateToday, int change)
{
	if (B.update_report_dispatch_amount(room_id, dateToday, change) == true)
	{
		std::cout << "DB：房间" << room_id << "报表修改成功" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB：房间" << room_id << "报表修改失败" << std::endl << "程序退出" << std::endl;
		exit(1);
	}
}

void DBFacade::setRecord_detail_amount(std::string room_id, std::string dateToday)
{
	if (B.update_report_detail_amount(room_id, dateToday) == true)
	{
		std::cout << "DB：房间" << room_id << "报表修改成功" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB：房间" << room_id << "报表修改失败" << std::endl << "程序退出" << std::endl;
		exit(1);
	}
}

void DBFacade::setRecord_total_fee(std::string room_id, std::string dateToday, float fee)//在修改了
{
	if (B.update_report_total_fee(room_id, dateToday, fee) == true)
	{
		std::cout << "DB：房间" << room_id << "报表修改成功" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB：房间" << room_id << "报表修改失败" << std::endl << "程序退出" << std::endl;
		exit(1);
	}
}

void DBFacade::setRecord_serve_time(std::string room_id, std::string dateToday, int time)
{
	if (B.update_report_serve_time(room_id, dateToday, time) == true)
	{
		std::cout << "DB：房间" << room_id << "报表修改成功" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB：房间" << room_id << "报表修改失败" << std::endl << "程序退出" << std::endl;
		exit(1);
	}
}

void DBFacade::newDetail(std::string room_id, std::string requestTime, int fan, float feeRate)     //
{
	if (B.insert_detail_newDetail(room_id, requestTime, fan, feeRate) == true)
	{
		std::cout << "DB：房间" << room_id << "插入新的详单成功" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB：房间" << room_id << "插入新的详单失败" << std::endl;

		getchar();
	}
}

void DBFacade::endDetail(std::string room_id, std::string endTime, float detailFee)
{
	if (B.update_Detail_endDetail(room_id, endTime, detailFee) == true)
	{
		std::cout << "DB：房间" << room_id << "修改详单成功" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB：房间" << room_id << "修改详单失败" << std::endl;
		getchar();
	}
}

void DBFacade::signIn(std::string room_id, std::string signInTime)
{
	if (B.insert_Regist_signIn(room_id, signInTime) == true)
	{
		std::cout << "DB:房间" << room_id << "登记入住成功！" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB:房间" << room_id << "登记入住成功！" << std::endl;
		getchar();
		return;
	}
}

void DBFacade::setRegist_total_fee(std::string room_id, float fee)
{
	if (B.update_Curfee_Regist(room_id, fee) == true)
	{
		std::cout << "DB_regist:房间" << room_id << "修改当前金额成功" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB_regist:房间" << room_id << "修改当前金额失败" << std::endl;
		std::cout << "程序退出" << std::endl;
		getchar();
		exit(1);
	}
}



void DBFacade::setRegist_signOut(std::string room_id, std::string signOutTime)
{
	if (B.setRoom_signOut(room_id, signOutTime) == false)
	{
		std::cout << "DB:房间" << room_id << "退房失败" << std::endl;
		return;
	}
	else
	{
		std::cout << "DB:房间" << room_id << "退房成功" << std::endl;
		return;
	}
}


Room_Invoice* DBFacade::getInvoice(std::string room_id)//退房返回总金额，操作失败返回-1
{

	Room_Invoice * dataBack = B.updateAndselect_Regist_signOut(room_id);
	if (dataBack != NULL)
	{
		std::cout << "DB:房间" << room_id << "退房成功，总金额为" << dataBack->total_fee << std::endl;
		return dataBack;
	}
	else
	{
		std::cout << "DB:房间" << room_id << "退房失败" << std::endl;
		return NULL;
	}
}


float DBFacade::getCurRoomFee(std::string room_id)            //返回当前房间的空调得费用，此处的房间必须是未退房的
{
	float returnFee;
	if (B.selectCurFee_Regist(room_id, &returnFee) == true)
		return returnFee;
	else
		std::cout << "查找当前费用失败！" << std::endl;
}

Room_record * DBFacade::getRoomRecord(std::string room_id, int reportType, std::string end_time)//返回单个房间在start_time到end_time时间段内的报表
{
	Room_record * dataList = NULL;
	if ((dataList = B.selectRoomReport_Report(room_id, reportType, end_time)) == NULL)
	{
		std::cout << "DB:调取房间" << room_id << "报表失败" << std::endl;
		return NULL;
	}
	else
	{
		std::cout << "DB:调取房间" << room_id << "报表成功" << std::endl;
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


Detail * DBFacade::getRoomDetail(std::string room_id)//返回单个房间在start_time到end_time时间段内的所有详单，可用于调取账单和详单
{
	Detail * return_DetailList = B.selectRoomDetail_Detail(room_id);

	if (return_DetailList != NULL)
	{
		std::cout << "DB:获取房间" << room_id << "详单成功" << std::endl;
		return return_DetailList;
	}
	else
	{
		std::cout << "DB:获取房间" << room_id << "详单失败！" << std::endl;
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

/* 获取当前系统时间：flag为真返回"年/月/日 时:分:秒"，为假返回"年/月/日" */
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
/* 将int型房间号转换成string型（r_**）房间号并返回 */
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
	int result = B.checkRoom_id(room_id);//1：已退房  0：未开过房  -1查找错误
	if (result == -1)
	{
		std::cout << "查找房间" << room_id << "时发生错误！" << std::endl;
		return -1;
	}

	return result;
}