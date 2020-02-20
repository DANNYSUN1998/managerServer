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
	bool isTimeSlot;			// �Ƿ���Ҫ�������
	RStateType state;			// ����״̬
	int servedTime;				// �ۼƷ���ʱ��
	int waitedTime;				// �ۼƵȴ�ʱ��
	int needWaitTime;			// ʣ��ȴ�ʱ��

	int roomId;					// �����
	float naturalTemp;			// ������Ȼ״̬�µĺ��£����յ���һ�ο���ʱЯ�����¶ȣ�
	float nowTemp;				// �¶ȣ��棩
	float targetTemp;			// Ŀ���¶ȣ��棩
	FanSpeedType nowFanSpeed;	// ����
	FanSpeedType targetFanSpeed;// Ŀ�����
	float detailFee;			// ��ǰ�굥���ã�Ԫ��	
	float preFee;				// ���ã�Ԫ��
};

