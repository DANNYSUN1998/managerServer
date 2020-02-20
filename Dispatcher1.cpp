#include "Dispatcher.h"
#include <thread>
#include <iostream>

std::map<int, int>socket_roomId;

Dispatcher::Dispatcher()
	:state(off), isClose(false), tempHighLimit(50), tempLowLimit(-50), defaultTargetTemp(26)
{
	std::cout << "请输入服务对象数：";
	std::cin >> MAX_CUSTOMERSERVER_NUMBER;
	std::cout << "请分别输入低中高调温速率（度/分钟）：";
	std::cin >> RATE1[1] >> RATE1[2] >> RATE1[3];
	std::cout << "请输入达到预设温度后的容忍范围（℃）：";
	std::cin >> ARRANGE;
	std::cout << "请输入回温速率（度/分钟）：";
	std::cin >> RATE2;
	std::cout << "请输入房间回温方式（1按初始温度/2按0-50）：";
	std::cin >> ADJUST_TEMP_METHOD2;
	std::cout << "请输入调度时间片大小（秒）：";
	std::cin >> TIMESLOT;
	std::cout << "请输入等待时间片大小（秒）：";
	std::cin >> WAIT_TIMESLOT;
	std::cout << "等待队列中的房间调风是否立即重新调度：（1是2否）";
	std::cin >> isImmidiatelyDispatch;
	
	// 初始化服务队列
	for (int i = 0; i < MAX_CUSTOMERSERVER_NUMBER; ++i) {
		servingQueue.push_back(nullptr);
	}
	// 初始化等待队列(无操作)

	if (IS_SHOWQUEUE) {
		std::thread t(&Dispatcher::ShowQueue, this); // 周期性显示队列情况
		t.detach();
	}
}

Dispatcher::~Dispatcher()
{
	// 删除房间对象
	std::map<int, Room*>::iterator iter = roomInfo.begin();
	for (; iter != roomInfo.end(); ++iter) {
		Room* tmp = iter->second;
		delete tmp;
	}
}

int Dispatcher::BindPort(const int port)
{
	int serverSock = 0;
	if (-1 == (serverSock = socket(AF_INET, SOCK_STREAM, 0))) {
		std::cerr << WSAGetLastError() << std::endl;
		perror("socket wrong");
		exit(0);
	}

	struct sockaddr_in recvAddr; // server address information
	recvAddr.sin_family = AF_INET; // host byte order
	recvAddr.sin_port = htons(port); // short, network byte order
	recvAddr.sin_addr.s_addr = INADDR_ANY; // auto-fill with server IP
	memset(&(recvAddr.sin_zero), 0, sizeof(recvAddr.sin_zero)); // zero the rest of the struct

	if (-1 == (bind(serverSock, (struct sockaddr*)&recvAddr, sizeof(struct sockaddr)))) {
		std::cerr << WSAGetLastError() << std::endl;
		perror("bind wrong");
		exit(0);
	}
	if (-1 == (listen(serverSock, BACKLOG))) {
		std::cerr << WSAGetLastError() << std::endl;
		perror("listen wrong");
		exit(0);
	}
	return serverSock;
}
void Dispatcher::BuildConnection()
{
	//一、建立TCP连接
	WORD request;
	WSADATA ws;
	request = MAKEWORD(1, 1);
	if (WSAStartup(request, &ws)) {
		std::cerr << "WSAStartup wrong!" << std::endl;
		exit(0);
	}
	if (LOBYTE(ws.wVersion) != 1 || HIBYTE(ws.wVersion) != 1) {
		std::cerr << "wVersion wrong!" << std::endl;
		WSACleanup();
		exit(0);
	}

	// 连接房间客户端
	int serverSock1 = BindPort(SERVERPORT1);
	// 连接管理员、前台、经理客户端
	int serverSock2 = BindPort(SERVERPORT2);

	//二、传输数据
	std::thread t1(&Dispatcher::AcceptClient, this, SERVERPORT1, serverSock1);
	t1.detach();
	std::thread t2(&Dispatcher::AcceptClient, this, SERVERPORT2, serverSock2);
	t2.join();

	// 三、结束
	Sleep(2000);
	WSACleanup();
}

void Dispatcher::AcceptClient(const int port, const int serverSock)
{
	int clientSock = 0;
	sockaddr_in clientAddr;
	while (1) {
		// 侦听连接
		int sin_size = sizeof(struct sockaddr_in);
		if (-1 == (clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &sin_size))) {
			std::cerr << WSAGetLastError() << std::endl;
			closesocket(clientSock);
			perror("accept wrong");
			break;
		}
		char *clientAddr_s = inet_ntoa(clientAddr.sin_addr); // 记得SDL检查改成否
		printf("Get connection from %s\n", clientAddr_s);

		std::thread t(&Dispatcher::RecvClient, this, port, serverSock, clientSock, clientAddr_s);
		t.detach();
	}
}

void Dispatcher::RecvClient(const int port, const int serverSock, const int clientSock, char *clientAddr)
{
	while (1) {
		// 一、接收消息
		// (1)json长度部分
		int numbytes = 0;
		char lenField[LENFIELDSIZE + 1] = { '\0' };
		if (-1 == (numbytes = recv(clientSock, lenField, LENFIELDSIZE, 0))) {
			std::cerr << WSAGetLastError() << std::endl;
			closesocket(clientSock);
			if (port == SERVERPORT1) { // 异常断开也当做退房
				if (socket_roomId.end() != socket_roomId.find(clientSock)) {
					StopServe(socket_roomId[clientSock], Off);
					socket_roomId.erase(clientSock);
					roomInfo.erase(socket_roomId[clientSock]);
					dbfacade.setRegist_signOut(getRoomIdString(socket_roomId[clientSock]), getDate(true));
				}
			}

			perror("recv wrong");
			return;
		}
		else if (0 == numbytes) {
			closesocket(clientSock);
			printf("%s closed the connection.\n", clientAddr);

			if (port == SERVERPORT1 && socket_roomId.end() != socket_roomId.find(clientSock)) {
				int roomId = socket_roomId[clientSock];
				printf("Room %d checked out.\n", roomId);					//	顾客退房
				if (roomInfo[roomId]->GetState() != Off)
					StopServe(roomId, Off); // 关机
				roomInfo.erase(roomId);
				socket_roomId.erase(clientSock);

				dbfacade.setRegist_signOut(getRoomIdString(roomId), getDate(true));
			}
			return;
		}
		lenField[numbytes] = '\0';
		std::cout << clientAddr << ": " << lenField << std::endl;

		// (2)数据部分
		numbytes = 0;
		int dataLength = atoi(lenField);
		char recvBuf[MAXDATALENGTH] = { '\0' };
		while (dataLength) {
			char buf[MAXDATALENGTH] = { '\0' };
			if (-1 == (numbytes = recv(clientSock, buf, dataLength, 0))) {
				std::cerr << WSAGetLastError() << std::endl;
				closesocket(clientSock);
				perror("recv wrong");
				return;
			}
			dataLength -= numbytes;
			sprintf(recvBuf, "%s%s", recvBuf, buf);
		}
		recvBuf[atoi(lenField)] = '\0';
		std::cout << clientAddr << ": " << recvBuf << std::endl;



		// 二、响应客户端操作
		if (port == SERVERPORT1) { // 响应顾客使用空调的请求
			if (state == ready) {
				int retcode = Analyze1(recvBuf, clientSock, clientAddr);
			}
			else if (state == setMode)
				printf("But the state is \"setMode\" now.\n");
			else
				printf("But the state is \"off\" now.\n");
		}
		else if (port == SERVERPORT2) { // 响应管理员、前台、经理的操作
			int retcode = Analyze2(recvBuf, clientSock);
			if (-1 == retcode) { // 管理员关主机
				// closesocket(serverSock);
			}
		}
	}
}

/* 解析客户端发来的JSON包 */
int Dispatcher::Analyze1(char* recvBuf, const int clientSock, char*clientAddr)
{
	Json::Value root;
	Json::Reader reader;
	if (reader.parse(recvBuf, root)) {
		int roomId = root["RoomID"].asInt();
		REQKIND requestType = (REQKIND)root["RequestKind"].asInt();

		float initTemp = root["InitTemp"].asDouble();
		float targetTemp = root["TargetTemp"].asDouble();
		FanSpeedType targetFanSpeed = (FanSpeedType)root["FanSpeed"].asInt();

		if (requestType == PowerOn) {				// 顾客开机
			printf("Room: %d PowerOn\n", roomId);

			Json::Value root;
			if (roomInfo.end() == roomInfo.find(roomId)) { // 开房
				roomInfo[roomId] = new Room(roomId, initTemp, initTemp, defaultTargetTemp, defaultFanSpeed, defaultFanSpeed);
				dbfacade.newRoom(getRoomIdString(roomId), getDate(false), initTemp, defaultTargetTemp);
				dbfacade.signIn(getRoomIdString(roomId), getDate(true));
				root["ErrMsg"] = (int)NoError;

				socket_roomId[clientSock] = roomId;
				roomIds.insert(roomId);
			}
			else if (roomInfo[roomId]->GetState() == Off) {
				root["ErrMsg"] = (int)NoError;
				dbfacade.setRecord_On_Off(getRoomIdString(roomId), getDate(false));
			}
			else
				root["ErrMsg"] = (int)DupRoomID;

			// 给顾客回复
			root["mode"] = (int)mode;
			root["Fee"] = 0.;
			root["FeeRate"] = FeeRate / (T.size() - (int)targetFanSpeed);
			root["CurrentRoomTemp"] = initTemp;
			root["default_TargetTemp"] = defaultTargetTemp;
			root["Temp_highLimit"] = tempHighLimit;
			root["Temp_lowLimit"] = tempLowLimit;

			SendJson(root, clientSock);

			if (roomInfo[roomId]->GetState() != Off)
				printf("But the room %d has already powered on. DupRoomId.\n", roomId);
			else { // 开始服务
				roomInfo[roomId]->Set(Hanging, initTemp, defaultTargetTemp, defaultFanSpeed, defaultFanSpeed);
				waitingQueue.push_back(roomInfo[roomId]);
			}
		}
		else if (requestType == PowerOff) {			// 顾客关机
			printf("Room: %d PowerOff\n", roomId);
			if (roomInfo[roomId]->GetState() == Off)
				printf("But the room %d has already powered off. The operation is invalid.\n", roomId);
			else {
				StopServe(roomId, Off);

				dbfacade.setRecord_On_Off(getRoomIdString(roomId), getDate(false));
			}
		}
		else if (requestType == ChangeTemp) {		// 顾客调温
			printf("Room: %d ChangeTemp\n", roomId);
			ChangeTempFanSpeed(roomId, Temp, targetTemp, targetFanSpeed);
			CustomerRequestFee(clientSock, roomId);
		}
		else if (requestType == ChangeFanSpeed) {	// 顾客调风
			printf("Room: %d ChangeFanSpeed\n", roomId);
			ChangeTempFanSpeed(roomId, FanSpeed, targetTemp, targetFanSpeed);
			CustomerRequestFee(clientSock, roomId);
		}
		else if (requestType == RequestFee) {		// 顾客请求费用
			printf("Room: %d RequestFee\n", roomId);
			CustomerRequestFee(clientSock, roomId);
		}
	}
	return 0;
}
int Dispatcher::Analyze2(char* recvBuf, const int clientSock)
{
	Json::Value root;
	Json::Reader reader;
	if (reader.parse(recvBuf, root)) {
		std::string method = root["Method"].asString();

		// 管理员部分
		if (method == "PowerOn") {
			bool value = root["Value"].asBool();
			if (value) {											// 管理员开主机
				printf("Administrator: PowerOn\n");

				Json::Value root;
				root["Method"] = "ACK";
				if (state == off) {
					state = setMode;
					root["State"] = "on";
				}
				else {
					root["State"] = "error";
					root["ErrorMessage"] = "Already On";
					if (state == setMode)
						printf("But the state is \"setMode\" now.\n");
					else
						printf("But the state is \"ready\" now.\n");
				}

				SendJson(root, clientSock);
			}
			else {													// 管理员关主机
				printf("Administrator: PowerOff\n");

				//Json::Value root;
				//root["Method"] = "ACK";
				int retcode = 0;
				if (state != off) {
					state = off;
					isClose = true;

					std::map<int, int>::iterator iter1 = socket_roomId.begin();
					for (; iter1 != socket_roomId.end(); ) {
						StopServe(iter1->second, Off);	//	关闭房间空调
						closesocket(iter1->first);		// 断开连接
						socket_roomId.erase(iter1++);
						//dbfacade.setRegist_signOut(getRoomIdString(iter1->second), getDate(true)); // 强制退房
					}
					roomInfo.clear();

					//root["State"] = "off";
					retcode = -1;
				}
				else {
					//root["State"] = "error";
					//root["ErrorMessage"] = "Already Off";
					//printf("But the state is \"off\" now.\n");
				}

				//SendJson(root, clientSock);
				return retcode;
			}
		}
		else if (method == "Init") {								// 管理员初始化主机参数
			printf("Administrator: Init\n");

			Json::Value root;
			root["Method"] = "ACK";
			if (state == setMode) {
				int retcode = AdministratorInit(recvBuf);
				if (-1 == retcode) {
					root["State"] = "error";
					root["ErrorMessage"] = "tempLowLimit > tempHighLimit";
					printf("tempLowLimit > tempHighLimit\n");
				}
				else if (-2 == retcode) {
					root["State"] = "error";
					root["ErrorMessage"] = "defaultTargetTemp is out of limit";
					printf("defaultTargetTemp is out of limit\n");
				}
				else {
					root["State"] = "ready";
					std::thread t(&Dispatcher::TimeSlot, this); // 周期性执行全同步时间片函数
					t.detach();
				}
			}
			else {
				root["State"] = "error";
				root["ErrorMessage"] = "Already Init";
				if (state == ready)
					printf("But the state is \"ready\" now.\n");
				else
					printf("But the state is \"off\" now.\n");
			}

			SendJson(root, clientSock);
		}
		else if (method == "Check") {								// 管理员请求各房间信息
			printf("Administrator: Check\n");

			if (state == ready) {
				GetAdministratorNeed(clientSock);
			}
			else {
				if (state == setMode)
					printf("But the state is \"setMode\" now.\n");
				else
					printf("But the state is \"off\" now.\n");
			}
		}

		// 前台部分
		if (method == "GetRDR") {											//前台获取详单
			Json::Value temp;
			std::string roomId_s = root["RoomID"].asString();
			std::string roomId_s1 = roomId_s.substr(2, roomId_s.size() - 2); // 仅保留数字
			std::cout << roomId_s1 << std::endl;
			if (roomInfo.end() != roomInfo.find(atoi(roomId_s1.c_str()))) {	// 还没退房
				temp["Method"] = "ReturnRDR";
				temp["failure"] = "still_in";
				printf("Room %s doesn't sign out\n", roomId_s.c_str());
			}
			else if (0 == dbfacade.checkRoom_id(roomId_s)) {// 没开过房
				temp["Method"] = "ReturnRDR";
				temp["failure"] = "no_room";
				printf("Room %s doesn't exist\n", roomId_s.c_str());
			}
			else
				temp = ReturnRDR(roomId_s);
			SendJson(temp, clientSock);
		}
		else if (method == "GetInvoice") {									//前台获取账单
			Json::Value temp;
			std::string roomId_s = root["RoomID"].asString();
			std::string roomId_s1 = roomId_s.substr(2, roomId_s.size() - 2); // 仅保留数字
			if (roomInfo.end() != roomInfo.find(atoi(roomId_s1.c_str()))) {// 还没退房
				temp["Method"] = "ReturnInvoice";
				temp["failure"] = "still_in";
				printf("Room %s doesn't sign out\n", roomId_s.c_str());
			}
			else if (0 == dbfacade.checkRoom_id(roomId_s)) {// 没开过房
				temp["Method"] = "ReturnInvoice";
				temp["failure"] = "no_room";
				printf("Room %s doesn't exist\n", roomId_s.c_str());
			}
			else
				temp = ReturnInvoice(roomId_s);
			SendJson(temp, clientSock);
		}

		// 经理部分
		if (method == "GetReport") {										//前台获取报表
			std::vector<std::string>listRoomId;
			std::set<int>::iterator iter = roomIds.begin();
			for (; iter != roomIds.end(); ++iter)
				listRoomId.push_back(getRoomIdString(*iter));
			Json::Value temp = ReturnReport(listRoomId, root["Report_Type"].asInt());
			SendJson(temp, clientSock);
		}

	}
	return 0;
}

void SendJson(const Json::Value&root, const int clientSock)
{
	std::string sendMessage = root.toStyledString();
	char messageLen[LENFIELDSIZE + 1];
	sprintf(messageLen, "%08d", (int)sendMessage.size());
	std::string messageLen_s = messageLen;

	if (!DEBUG && -1 == send(clientSock, (messageLen_s + sendMessage).c_str(), (messageLen_s + sendMessage).size(), 0)) {
		std::cerr << WSAGetLastError() << std::endl;
		perror("send wrong");
		closesocket(clientSock);
		exit(0);
	}
}