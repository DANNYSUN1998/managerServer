#include "Dispatcher.h"
#include <thread>
#include <mutex>
#include <iostream>
#include <time.h>
#include <ctime>
#include <algorithm>

std::mutex m;

DStateType Dispatcher::GetState()const
{
	return state;
}
void Dispatcher::ShowQueue()
{
	while (1) {
		clock_t startTime = clock();
		while (clock() - startTime < SHOWQUEUE_T * CLOCKS_PER_SEC) {
			;
		}

		printf("--------------------------------------------------");
		printf("\nServingQueue: \n");
		for (int i = 0; i < MAX_CUSTOMERSERVER_NUMBER; ++i) {
			if (servingQueue[i])
				printf("Room %d：%.1f, %.1f  %d, %d\n", servingQueue[i]->GetRoomId(), servingQueue[i]->GetTemp(), servingQueue[i]->GetTargetTemp(), servingQueue[i]->GetFanSpeed(), servingQueue[i]->GetTargetFanSpeed());
		}
		printf("\nWaitingQueue: \n");
		std::vector<Room*>::iterator iter = waitingQueue.begin();
		for (; iter != waitingQueue.end(); ++iter) {
			printf("Room %d：%.1f, %.1f  %d, %d\n", (*iter)->GetRoomId(), (*iter)->GetTemp(), (*iter)->GetTargetTemp(), (*iter)->GetFanSpeed(), (*iter)->GetTargetFanSpeed());
		}
		printf("\n--------------------------------------------------\n");
	}
}
//*****************************************************************************************

bool Cmp(const Room*r1, const Room*r2)
{
	return r1->GetWaitedTime() > r2->GetWaitedTime();
}

void Dispatcher::TimeSlot()
{
	while (state != off) {
		clock_t start = clock();
		while (clock() - start < TIMESLOT*CLOCKS_PER_SEC) { 
			;
		}

		// 每个时间片执行一次下述内容

		std::lock_guard<std::mutex> lockGuard(m);

		std::map<int, bool>isDone;
		std::vector<int>spareServerIds;
		for (int i = 0; i < MAX_CUSTOMERSERVER_NUMBER;++i) { // 对服务队列中的每一个房间
			if (servingQueue[i]) {
				isDone[servingQueue[i]->GetRoomId()] = true;

				float nowTemp = servingQueue[i]->GetTemp();
				float targetTemp = servingQueue[i]->GetTargetTemp();

				//达到预期温度，从服务队列中移除，放入等待队列
				if (abs(nowTemp - targetTemp) <= EPS ||
					(mode == Refrigeration && nowTemp < targetTemp || mode == Heating && nowTemp > targetTemp)) {
					waitingQueue.push_back(servingQueue[i]);
					servingQueue[i]->SetState(Hanging);
					servingQueue[i]->SetWaitType(false); // 该房间虽在等待队列中但无申请调度的意向
					dbfacade.setRecord_dispatch_amount(getRoomIdString(servingQueue[i]->GetRoomId()), getDate(false), 1);
					roomInfo[servingQueue[i]->GetRoomId()]->EndDetail();

					servingQueue[i] = nullptr;
					continue;
				}

				//温度向目标温度变化（调温）
				if (abs(nowTemp - targetTemp) > EPS) {
					float rate = 1.;
					rate = RATE1[servingQueue[i]->GetFanSpeed()] / (60 / TIMESLOT);
					mode == Refrigeration ? nowTemp -= rate : nowTemp += rate;
					if (mode == Refrigeration && nowTemp < targetTemp || mode == Heating && nowTemp > targetTemp)
						nowTemp = targetTemp;
					servingQueue[i]->SetTemp(nowTemp);
					//计费++
					servingQueue[i]->SetFee((FeeRate / ((int)T.size() - (int)servingQueue[i]->GetFanSpeed())) / (60. / TIMESLOT));
				}

				//服务时间++
				servingQueue[i]->SetServedTime(TIMESLOT);

				dbfacade.setRoomCurState(getRoomIdString(servingQueue[i]->GetRoomId()), 1, 0, 1, nowTemp, targetTemp, servingQueue[i]->GetFanSpeed(), servingQueue[i]->GetTargetFanSpeed());
			}
			else
				spareServerIds.push_back(i);
		}

		//bool isOneGetServe = false;
		Room *room = nullptr;
		
		std::sort(waitingQueue.begin(), waitingQueue.end(), Cmp); // 按服务时长排序

		std::vector<Room*>::iterator iter = waitingQueue.begin();
		for (; iter != waitingQueue.end();) { // 对等待队列中的每一个房间（等待队列按等待时间大小排序）
			if (!isDone[(*iter)->GetRoomId()]) { // 且该房间刚才没有在服务队列中
				//温度向初始温度变化（回温）;
				float nowTemp = (*iter)->GetTemp();
				float naturalTemp = (*iter)->GetNaturalTemp();
				if (abs(nowTemp - naturalTemp) > EPS) {
					float rate = RATE2 / (60 / TIMESLOT);
					naturalTemp < nowTemp ? nowTemp -= rate : nowTemp += rate;
					if (1 == ADJUST_TEMP_METHOD2) {
						if ((mode == Refrigeration && nowTemp < naturalTemp) || (mode == Heating && nowTemp > naturalTemp))
							nowTemp = naturalTemp;
					}
					else {
						if (mode == Refrigeration && nowTemp < 0)
							nowTemp = 0;
						else if(mode == Heating && nowTemp > 50)
							nowTemp = 50;
					}
					(*iter)->SetTemp(nowTemp);
				}

				float targetTemp = (*iter)->GetTargetTemp();
				if (((mode == Refrigeration && nowTemp > targetTemp)||(mode== Heating && nowTemp < Heating))
					&&(abs(nowTemp - targetTemp) > ARRANGE || abs(nowTemp - targetTemp - ARRANGE) <= EPS)) {
					(*iter)->SetWaitType(true); // 开始参与申请调度
				}

				(*iter)->SetWaitedTime(TIMESLOT);	// 累计等待时长++
				if ((*iter)->GetNeedWaitTime() > 0)
					(*iter)->SetNeedWaitTime(-TIMESLOT);// 剩余等待时长--

				if (/*!isOneGetServe &&*/ (*iter)->GetWaitType() && 0 == (*iter)->GetNeedWaitTime()) {
					// 如果服务对象够
					if (0 != (int)spareServerIds.size()) {
						//将房间放入服务队列，不执行下列调度
						int spareServerId = spareServerIds.back();
						spareServerIds.pop_back();

						servingQueue[spareServerId] = (*iter);
						servingQueue[spareServerId]->SetState(Serving);
						FanSpeedType fanSpeed = servingQueue[spareServerId]->GetTargetFanSpeed();
						servingQueue[spareServerId]->SetFanSpeed(fanSpeed);
						iter = waitingQueue.erase(iter);
						//isOneGetServe = true;

						dbfacade.setRoomCurState(getRoomIdString(servingQueue[spareServerId]->GetRoomId()), 1, 0, 1, nowTemp, targetTemp, fanSpeed, servingQueue[spareServerId]->GetTargetFanSpeed());
						dbfacade.setRecord_dispatch_amount(getRoomIdString(servingQueue[spareServerId]->GetRoomId()), getDate(false), 1);
						dbfacade.newDetail(getRoomIdString(servingQueue[spareServerId]->GetRoomId()), getDate(true), fanSpeed, FeeRate / (T.size() - (int)fanSpeed));

						continue;
					}

					// 如果服务对象不够，开始进行调度
					FanSpeedType targetFanSpeed = (*iter)->GetTargetFanSpeed();
					int serverId = -1;
					for (int j = 0; j < MAX_CUSTOMERSERVER_NUMBER; ++j) {
						if (servingQueue[j]->GetFanSpeed() < targetFanSpeed || servingQueue[j]->GetFanSpeed() == targetFanSpeed) {
							// 先优先级调度
							if (-1 == serverId || servingQueue[j]->GetFanSpeed() < servingQueue[serverId]->GetFanSpeed()) {
								serverId = j;
							}
							// 优先级相同时选（总）服务时长最长的
							else if (servingQueue[j]->GetFanSpeed() == servingQueue[serverId]->GetFanSpeed()
								&& servingQueue[j]->GetServedTime() > servingQueue[j]->GetServedTime()) {
								serverId = j;
							}
						}
					}
					if (-1 != serverId) {
						// 将选中房间从服务队列中删除，(加入等待队列)
						room = servingQueue[serverId];
						room->SetState(Hanging);
						room->SetNeedWaitTime(WAIT_TIMESLOT); // 等待时长
						dbfacade.setRoomCurState(getRoomIdString(room->GetRoomId()), 1, 1, 0, room->GetTemp(), room->GetTargetTemp(), room->GetFanSpeed(), room->GetTargetFanSpeed());
						dbfacade.setRecord_dispatch_amount(getRoomIdString(room->GetRoomId()), getDate(false), 1);
						room->EndDetail();

						// 将请求房间从等待队列中删除，加入服务队列
						servingQueue[serverId] = (*iter);
						servingQueue[serverId]->SetState(Serving);
						iter = waitingQueue.erase(iter);
						FanSpeedType fanSpeed = servingQueue[serverId]->GetTargetFanSpeed();
						servingQueue[serverId]->SetFanSpeed(fanSpeed);

						dbfacade.setRoomCurState(getRoomIdString(servingQueue[serverId]->GetRoomId()), 1, 0, 1, nowTemp, targetTemp, fanSpeed, servingQueue[serverId]->GetTargetFanSpeed());
						dbfacade.setRecord_dispatch_amount(getRoomIdString(servingQueue[serverId]->GetRoomId()), getDate(false), 1);
						dbfacade.newDetail(getRoomIdString(servingQueue[serverId]->GetRoomId()), getDate(true), fanSpeed, FeeRate / (T.size() - (int)fanSpeed));
					}
					else
						++iter;
				}
				else
					++iter;
			}
			else
				++iter;
		}
		if (room)
			waitingQueue.push_back(room); // 因为放在里面会使迭代器失效
	}
}

/* 响应客户调温调风的请求 */
void Dispatcher::ChangeTempFanSpeed(const int roomId, const OperType operType, const float targetTemp, const FanSpeedType targetFanSpeed)
{
	std::lock_guard<std::mutex> lockGuard(m);

	bool isServe = false;
	// 若房间在服务队列中
	for (int i = 0; i < MAX_CUSTOMERSERVER_NUMBER; ++i) {
		if (servingQueue[i] && servingQueue[i]->GetRoomId() == roomId) {
			if (operType == Temp) {
				servingQueue[i]->SetTargetTemp(targetTemp);
			}
			if (operType == FanSpeed) {
				servingQueue[i]->SetFanSpeed(targetFanSpeed);
				servingQueue[i]->SetTargetFanSpeed(targetFanSpeed);
				servingQueue[i]->EndDetail();
				dbfacade.newDetail(getRoomIdString(roomId), getDate(true), (int)targetFanSpeed, FeeRate / (T.size() - targetFanSpeed));
			}
			dbfacade.setRoomCurState(getRoomIdString(roomId), 1, 0, 1, roomInfo[roomId]->GetTemp(), roomInfo[roomId]->GetTargetTemp(), roomInfo[roomId]->GetFanSpeed(), roomInfo[roomId]->GetTargetFanSpeed());
			isServe = true;
			break;
		}
	}

	// 若房间在等待队列中
	std::vector<Room*>::iterator iter = waitingQueue.begin();
	for (; !isServe && iter != waitingQueue.end(); ++iter) {
		if ((*iter)->GetRoomId() == roomId) {
			if (operType == Temp) {
				(*iter)->SetTargetTemp(targetTemp);
			}
			if (operType == FanSpeed) {
				(*iter)->SetTargetFanSpeed(targetFanSpeed);
				if (1 == isImmidiatelyDispatch) {
					(*iter)->SetNeedWaitTime(-(*iter)->GetNeedWaitTime());
				}
			}
			dbfacade.setRoomCurState(getRoomIdString(roomId), 1, 1, 0, roomInfo[roomId]->GetTemp(), roomInfo[roomId]->GetTargetTemp(), roomInfo[roomId]->GetFanSpeed(), roomInfo[roomId]->GetTargetFanSpeed());
			break;
		}
	}
}

/* 顾客关机 */
void Dispatcher::StopServe(const int roomId, const RStateType state)
{
	std::lock_guard<std::mutex> lockGuard(m);

	roomInfo[roomId]->SetState(state);

	bool isServe = false;
	// 若房间在服务队列中
	for (int i = 0; i < MAX_CUSTOMERSERVER_NUMBER; ++i) {
		if (servingQueue[i] && servingQueue[i]->GetRoomId() == roomId) {
			servingQueue[i] = nullptr;
			isServe = true;
			roomInfo[roomId]->EndDetail();

			std::sort(waitingQueue.begin(), waitingQueue.end(), Cmp); // 按服务时长排序
			std::vector<Room*>::iterator iter = waitingQueue.begin(); // 选服务时长最大的
			if (waitingQueue.size() && (*iter)->GetWaitType() && 0 == (*iter)->GetNeedWaitTime()) {
				//将房间放入服务队列
				int spareServerId = i;

				servingQueue[spareServerId] = (*iter);
				servingQueue[spareServerId]->SetState(Serving);
				FanSpeedType fanSpeed = servingQueue[spareServerId]->GetTargetFanSpeed();
				servingQueue[spareServerId]->SetFanSpeed(fanSpeed);
				iter = waitingQueue.erase(iter);

				dbfacade.setRoomCurState(getRoomIdString(servingQueue[spareServerId]->GetRoomId()), 1, 0, 1, servingQueue[spareServerId]->GetTemp(), servingQueue[spareServerId]->GetTargetTemp(), fanSpeed, servingQueue[spareServerId]->GetTargetFanSpeed());
				dbfacade.setRecord_dispatch_amount(getRoomIdString(servingQueue[spareServerId]->GetRoomId()), getDate(false), 1);
				dbfacade.newDetail(getRoomIdString(servingQueue[spareServerId]->GetRoomId()), getDate(true), fanSpeed, FeeRate / (T.size() - (int)fanSpeed));	
			}
			break;
		}
	}

	// 若房间在等待队列中
	std::vector<Room*>::iterator iter = waitingQueue.begin();
	for (; !isServe && iter != waitingQueue.end(); ++iter) {
		if ((*iter)->GetRoomId() == roomId) {
			waitingQueue.erase(iter);
			break;
		}
	}

	dbfacade.setRoomCurState(getRoomIdString(roomId), 0, 0, 0, 0, 0, 0, 0);
	roomInfo[roomId]->Set(Off, roomInfo[roomId]->GetNaturalTemp(), defaultTargetTemp, defaultFanSpeed, defaultFanSpeed);
}

/* 顾客请求查看费用 */
void Dispatcher::CustomerRequestFee(const int clientSock, const int roomId)
{
	Json::Value root;

	Room* room = roomInfo[roomId];

	root["mode"] = (int)mode;
	root["Fee"] = room->GetTotalFee();
	char feeRate[5];
	float temp = FeeRate / (T.size() - (int)room->GetFanSpeed());
	sprintf(feeRate, "%.2f", temp);
	root["FeeRate"] = atof(feeRate);
	root["ErrMsg"] = (int)NoError;
	root["CurrentRoomTemp"] = room->GetTemp(); // 顺便回送房间当前温度
	root["default_TargetTemp"] = defaultTargetTemp;
	root["Temp_highLimit"] = tempHighLimit;
	root["Temp_lowLimit"] = tempLowLimit;

	SendJson(root, clientSock);
}

/* 管理员请求各房间信息 */
void Dispatcher::GetAdministratorNeed(const int clientSock)
{
	Json::Value root;
	Json::Value arrayObj;
	Json::Value item;

	std::map<int, Room*>::iterator iter = roomInfo.begin();
	for (; iter != roomInfo.end(); ++iter) {
		Room* room = iter->second;

		item["RoomId"] = room->GetRoomId();
		item["IsPowerOn"] = (room->GetState() == Off ? 0 : 1);
		item["IsServe"] = (room->GetState() == Hanging ? 0 : 1);
		item["Temp"] = room->GetTemp();
		item["TargetTemp"] = room->GetTargetTemp();
		item["FanSpeed"] = (int)room->GetFanSpeed();
		item["FeeRate"] = FeeRate / (T.size() - (int)room->GetFanSpeed());
		item["Fee"] = room->GetTotalFee();
		item["ServingTime"] = room->GetServedTime();

		arrayObj.append(item);
	}

	root["Method"] = "RoomState";
	root["Rooms"] = arrayObj;
	SendJson(root, clientSock);
}
/* 管理员初始化主机参数 */
int Dispatcher::AdministratorInit(char *recvBuf)
{
	Json::Value root;
	Json::Reader reader;

	if (reader.parse(recvBuf, root)) {
		mode = (MODETYPE)root["Mode"].asInt();
		tempHighLimit = root["TempH"].asDouble();
		tempLowLimit = root["TempL"].asDouble();
		if (tempLowLimit > tempHighLimit)
			return -1;
		defaultTargetTemp = root["DefaultT"].asDouble();
		if (defaultTargetTemp < tempLowLimit || defaultTargetTemp > tempHighLimit)
			return -2;
		defaultFanSpeed = (FanSpeedType)root["DefaultFanSpeed"].asInt();
		FeeRate = root["FeeRate"].asDouble();
		T[FanSpeed_L] = root["FeeRateL"].asInt() * TIMEUNIT;
		T[FanSpeed_M] = root["FeeRateM"].asInt() * TIMEUNIT;
		T[FanSpeed_H] = root["FeeRateH"].asInt() * TIMEUNIT;
	}

	state = ready;
	dbfacade.setPara(FeeRate / (T.size() - FanSpeed_L), FeeRate / (T.size() - FanSpeed_M), FeeRate / (T.size() - FanSpeed_H), mode, defaultTargetTemp, tempLowLimit, tempHighLimit);

	return 0;
}