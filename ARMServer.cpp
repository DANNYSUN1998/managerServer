#include "ARMServer.h"

ARMServer::ARMServer()
{
}

ARMServer::~ARMServer()
{
}


list_RDR* ARMServer::CreateDetail(std::string roomid) {//ǰ̨��ȡ�÷����굥
	Detail *temp = dbfacade.getRoomDetail(roomid);
	list_RDR* temp2 = new list_RDR();
	std::vector<SingleRDR> RDR_List;
	while (temp != NULL) {
		SingleRDR *Sing = new SingleRDR();
		Sing->RoomId = temp->room_id;
		Sing->FanSpeed = temp->fan;
		Sing->Fee = temp->fee;
		Sing->FeeRate = temp->feeRate;
		Sing->Duration = temp->duration;
		Sing->RequestTime = temp->start_time;
		RDR_List.push_back(*Sing);
		temp = temp->next;
		delete Sing;
	}
	temp2->RDR_List = RDR_List;
	return temp2;
}
list_invoice* ARMServer::CreateInvoice(std::string roomid) {//ǰ̨��ȡ�÷����ͷ��β���˵�
	Room_Invoice* temp = dbfacade.getInvoice(roomid);
	list_invoice *temp2 = new list_invoice();
	temp2->date_in = temp->date_in;
	temp2->date_out = temp->date_out;
	temp2->RoomID = temp->room_id;
	temp2->total_fee = temp->total_fee;
	return temp2;
}

SingleReport* ARMServer::CreateReport(std::string RoomId, int ReportType) {//ǰ̨��ȡ�����ͷ��β�ı���
	Room_record* temp = new Room_record();
	temp = dbfacade.getRoomRecord(RoomId, ReportType, getDate(false));
	//����ţ��������ͣ���ǰ����
	SingleReport *temp2 = new SingleReport();

	temp2->UsingTime = temp->using_time;
	temp2->total_fee = temp->total_fee;
	temp2->RoomId = temp->room_id;
	temp2->OnOffNum = temp->amount_on_off;
	temp2->DispatchNum = temp->amount_dispatch;
	temp2->DetailNum = temp->amonut_detail;
	temp2->ChangeTempNum = temp->amount_changeTemp;
	temp2->ChangeFanSpeedNum = temp->amount_changeFan;
	temp2->Date = temp->date;
	
	delete temp;
	return temp2;
}
