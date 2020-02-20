#include "Dispatcher.h"

Json::Value Dispatcher::ReturnRDR(std::string roomid) {
	list_RDR* detailtarget = armserver.CreateDetail(roomid);

	Json::Value root;
	root["Method"] = "ReturnRDR";

	Json::Value arrayObj;
	Json::Value item;

	std::vector<SingleRDR>::iterator iter;
	for (iter = detailtarget->RDR_List.begin(); iter != detailtarget->RDR_List.end(); iter++) {
		item["RoomID"] = iter->RoomId.c_str();
		item["RequestTime"] = iter->RequestTime.c_str();
		item["Duration"] = iter->Duration;
		item["FanSpeed"] = iter->FanSpeed;
		item["FeeRate"] = iter->FeeRate;
		item["Fee"] = iter->Fee;

		arrayObj.append(item);
	}
	root["RDRList"] = arrayObj;

	delete detailtarget;
	return root;
}


Json::Value Dispatcher::ReturnInvoice(std::string roomid) {
	list_invoice* invoicetarget = armserver.CreateInvoice(roomid);
	Json::Value temp;
	//打包为JSON发送
	temp["Method"] = "ReturnInvoice";
	temp["RoomID"] = invoicetarget->RoomID;
	temp["Total_Fee"] = invoicetarget->total_fee;
	temp["date_in"] = invoicetarget->date_in;
	temp["date_out"] = invoicetarget->date_out;

	delete invoicetarget;
	return temp;
}

Json::Value Dispatcher::ReturnReport(std::vector<std::string>ListRoomId, int ReportType) 
{
	Json::Value root;
	root["Method"] = "ReturnReport";
	root["Report_type"] = ReportType;

	Json::Value arrayObj;
	Json::Value item;

	std::vector<std::string>::iterator iter;
	for (iter = ListRoomId.begin(); iter != ListRoomId.end(); iter++) {
		SingleReport* reporttarget = armserver.CreateReport(*iter, ReportType);

		item["RoomID"] = reporttarget->RoomId.c_str();
		item["OnOffNum"] = reporttarget->OnOffNum;
		item["UsingTime"] = reporttarget->UsingTime;
		item["Total_Fee"] = reporttarget->total_fee;
		item["DispatchNum"] = reporttarget->DispatchNum;
		item["DetailNum"] = reporttarget->DetailNum;
		item["ChangeTempNum"] = reporttarget->ChangeTempNum;
		item["ChangeFanSpeedNum"] = reporttarget->ChangeFanSpeedNum;

		delete reporttarget;
		
		arrayObj.append(item);
	}
	root["ReportList"] = arrayObj;
	return root;
}