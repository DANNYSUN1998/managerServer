#pragma once

#include <vector>
#include <time.h>
#include "DBFacade.h"

enum FanSpeedType { FanSpeed_L = 1, FanSpeed_M, FanSpeed_H };
enum RStateType { Off, Hanging, Serving };

class Room
{
public:
	Room(const int roomId, const float naturalTemp, const float nowTemp, const float targetTemp, const FanSpeedType nowFanSpeed, const FanSpeedType targetFanSpeed);

	bool GetWaitType()const;
	RStateType GetState()const;

	int GetRoomId()const;
	float GetNaturalTemp()const;
	float GetTemp()const;
	float GetTargetTemp()const;
	FanSpeedType GetFanSpeed()const;
	FanSpeedType GetTargetFanSpeed()const;
	int GetServedTime()const;
	int GetWaitedTime()const;
	int GetNeedWaitTime()const;
	float GetTotalFee()const;

	void SetWaitType(const bool isTimeSlot);
	void SetState(const RStateType state);
	void SetServedTime(const int increment);
	void SetWaitedTime(const int increment);
	void SetNeedWaitTime(const int increment);

	void SetTemp(const float temp);
	void SetTargetTemp(const float targetTemp);
	void SetFanSpeed(const FanSpeedType FanSpeed);
	void SetTargetFanSpeed(const FanSpeedType targetFanSpeed);
	void SetFee(const float increment);
	void EndDetail(void);

	void Set(const RStateType state, const float nowTemp, const float targetTemp, const FanSpeedType nowFanSpeed, const FanSpeedType targetFanSpeed);

private:
	bool isTimeSlot;			// 是否需要申请调度
	RStateType state;			// 房间状态
	int servedTime;				// 累计服务时长
	int waitedTime;				// 累计等待时长
	int needWaitTime;			// 剩余等待时长

	int roomId;					// 房间号
	float naturalTemp;			// 房间自然状态下的恒温（即空调第一次开机时携带的温度）
	float nowTemp;				// 温度（℃）
	float targetTemp;			// 目标温度（℃）
	FanSpeedType nowFanSpeed;	// 风速
	FanSpeedType targetFanSpeed;// 目标风速
	float detailFee;			// 当前详单费用（元）	
	float preFee;				// 费用（元）
};

