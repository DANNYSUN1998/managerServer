// AirConditionerSystem.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "Dispatcher.h"

int main(void)
{
	dbfacade.initDB();

	Dispatcher dispatcher;

	if (DEBUG) {
		//管理员开主机
		char powerOn[MAXDATALENGTH] = "{\"Method\":\"PowerOn\", \"Value\":true}";
		dispatcher.Analyze2(powerOn, 0);

		//管理员初始化参数
		Json::Value root;
		root["Mode"] = (int)Heating;
		root["TempH"] = 50.;
		root["TempL"] = -50.;
		root["DefaultT"] = 26.;
		root["FeeRateL"] = 0.75;
		root["FeeRateM"] = 1.;
		root["FeeRateH"] = 1.25;
		std::string sendMessage = root.toStyledString();
		char str[1600];
		for (int i = 0; i < (int)sendMessage.size(); ++i)
			str[i] = sendMessage[i];
		str[sendMessage.size()] = '\0';
		dispatcher.AdministratorInit(str);

		// 1、2、3号房间的顾客开机
		for (int i = 1; i <= 3; ++i) {
			root.clear();
			root["RoomID"] = i;
			root["RequestKind"] = PowerOn;
			root["TargetTemp"] = 26;
			root["FanSpeed"] = (int)FanSpeed_L;
			root["InitTemp"] = 25;
			std::string sendMessage = root.toStyledString();
			char str[1600];
			for (int j = 0; j < (int)sendMessage.size(); ++j)
				str[j] = sendMessage[j];
			str[sendMessage.size()] = '\0';

			char *temp = nullptr;
			dispatcher.Analyze1(str, 0, temp);
		}

		Sleep(2000);
		dispatcher.CustomerRequestFee(0, 1);
		dispatcher.CustomerRequestFee(0, 2);
		dispatcher.CustomerRequestFee(0, 3);

		// 12345号房间的顾客开机
		{
			root.clear();
			root["RoomID"] = 12345;
			root["RequestKind"] = PowerOn;
			root["TargetTemp"] = 26;
			root["FanSpeed"] = (int)FanSpeed_L;
			root["InitTemp"] = 23;
			std::string sendMessage = root.toStyledString();
			char str[1600] = { '\0' };
			for (int j = 0; j < (int)sendMessage.size(); ++j)
				str[j] = sendMessage[j];
			str[sendMessage.size()] = '\0';

			char *temp = nullptr;
			dispatcher.Analyze1(str, 0, temp);
		}

		//关机只能粗略测了
		dispatcher.StopServe(1, Off);
		dbfacade.setRegist_signOut(getRoomIdString(1), getDate(true));

		dispatcher.ChangeTempFanSpeed(2, Temp, 24, FanSpeed_L);		// 2号房间的顾客调节目标温度为24℃
		dispatcher.ChangeTempFanSpeed(3, FanSpeed, 0, FanSpeed_M);	// 3号房间的顾客调节目标风速为中速风

		Sleep(2000);
	}


	dispatcher.BuildConnection();

	return 0;
}