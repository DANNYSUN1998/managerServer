#include "Room.h"

Room::Room(const int roomId, const float naturalTemp, const float nowTemp, const float targetTemp, const FanSpeedType nowFanSpeed, const FanSpeedType targetFanSpeed)
	: isTimeSlot(false), state(Off), roomId(roomId), naturalTemp(naturalTemp), nowTemp(nowTemp), targetTemp(targetTemp), nowFanSpeed(nowFanSpeed), targetFanSpeed(targetFanSpeed), detailFee(0), preFee(0), servedTime(0), waitedTime(0), needWaitTime(0)
{
	;
}

bool Room::GetWaitType()const
{
	return isTimeSlot;
}
RStateType Room::GetState()const
{
	return state;
}
int Room::GetServedTime()const
{
	return servedTime;
}
int Room::GetWaitedTime()const
{
	return waitedTime;
}
int Room::GetNeedWaitTime()const
{
	return needWaitTime;
}

int Room::GetRoomId()const
{
	return roomId;
}
float Room::GetNaturalTemp()const
{
	return naturalTemp;
}
float Room::GetTemp()const
{
	return nowTemp;
}
float Room::GetTargetTemp()const
{
	return targetTemp;
}
FanSpeedType Room::GetFanSpeed()const
{
	return nowFanSpeed;
}
FanSpeedType Room::GetTargetFanSpeed()const
{
	return targetFanSpeed;
}
float Room::GetTotalFee()const
{
	return preFee + detailFee;
}

void Room::SetWaitType(const bool isTimeSlot)
{
	this->isTimeSlot = isTimeSlot;
}
void Room::SetState(const RStateType state)
{
	this->state = state;
}
void Room::SetServedTime(const int increment)
{
	servedTime += increment;

	dbfacade.setRecord_serve_time(getRoomIdString(roomId), getDate(false), servedTime);
}
void Room::SetWaitedTime(const int increment)
{
	waitedTime += increment;
}
void Room::SetNeedWaitTime(const int increment)
{
	needWaitTime += increment;
}

void Room::SetTemp(const float temp)
{
	nowTemp = temp;

	if (temp > nowTemp && 0.99 < abs(nowTemp - (int)nowTemp) && abs(nowTemp - (int)nowTemp) < 1)
		nowTemp += 0.01;
	else if (temp < nowTemp && state == 0 < abs(nowTemp - (int)nowTemp) && abs(nowTemp - (int)nowTemp) < 0.01)
		nowTemp -= 0.01;
}
void Room::SetTargetTemp(const float targetTemp)
{
	this->targetTemp = targetTemp;

	dbfacade.setRecord_ChangeTemp(getRoomIdString(roomId), getDate(false));
}
void Room::SetFanSpeed(const FanSpeedType FanSpeed)
{
	this->nowFanSpeed = FanSpeed;
}
void Room::SetTargetFanSpeed(const FanSpeedType targetFanSpeed)
{
	this->targetFanSpeed = targetFanSpeed;

	dbfacade.setRecord_ChangeFan(getRoomIdString(roomId), getDate(false));
}
void Room::SetFee(const float increment)
{
	detailFee += increment;

	if (0.99 < abs(detailFee - (int)detailFee) && abs(detailFee - (int)detailFee) < 1)
		detailFee += 0.01;
}
void Room::EndDetail()
{
	dbfacade.endDetail(getRoomIdString(roomId), getDate(true), detailFee);
	
	preFee += detailFee;
	detailFee = 0;

	dbfacade.setRecord_total_fee(getRoomIdString(roomId), getDate(false), preFee);
	dbfacade.setRegist_total_fee(getRoomIdString(roomId), preFee);
}

void Room::Set(const RStateType state, const float nowTemp, const float targetTemp, const FanSpeedType nowFanSpeed, const FanSpeedType targetFanSpeed)
{
	this->state = state;

	this->nowTemp = nowTemp;
	this->targetTemp = targetTemp;
	this->nowFanSpeed = nowFanSpeed;
	this->targetFanSpeed = targetFanSpeed;
}

