#include "Dispatcher.h"
#include <thread>
#include <iostream>

std::map<int, int>socket_roomId;

Dispatcher::Dispatcher()
	:state(off), isClose(false), tempHighLimit(50), tempLowLimit(-50), defaultTargetTemp(26)
{
	std::cout << "����������������";
	std::cin >> MAX_CUSTOMERSERVER_NUMBER;
	std::cout << "��ֱ�������иߵ������ʣ���/���ӣ���";
	std::cin >> RATE1[1] >> RATE1[2] >> RATE1[3];
	std::cout << "������ﵽԤ���¶Ⱥ�����̷�Χ���棩��";
	std::cin >> ARRANGE;
	std::cout << "������������ʣ���/���ӣ���";
	std::cin >> RATE2;
	std::cout << "�����뷿����·�ʽ��1����ʼ�¶�/2��0-50����";
	std::cin >> ADJUST_TEMP_METHOD2;
	std::cout << "���������ʱ��Ƭ��С���룩��";
	std::cin >> TIMESLOT;
	std::cout << "������ȴ�ʱ��Ƭ��С���룩��";
	std::cin >> WAIT_TIMESLOT;
	std::cout << "�ȴ������еķ�������Ƿ��������µ��ȣ���1��2��";
	std::cin >> isImmidiatelyDispatch;
	
	// ��ʼ���������
	for (int i = 0; i < MAX_CUSTOMERSERVER_NUMBER; ++i) {
		servingQueue.push_back(nullptr);
	}
	// ��ʼ���ȴ�����(�޲���)

	if (IS_SHOWQUEUE) {
		std::thread t(&Dispatcher::ShowQueue, this); // ��������ʾ�������
		t.detach();
	}
}

Dispatcher::~Dispatcher()
{
	// ɾ���������
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
	//һ������TCP����
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

	// ���ӷ���ͻ���
	int serverSock1 = BindPort(SERVERPORT1);
	// ���ӹ���Ա��ǰ̨������ͻ���
	int serverSock2 = BindPort(SERVERPORT2);

	//������������
	std::thread t1(&Dispatcher::AcceptClient, this, SERVERPORT1, serverSock1);
	t1.detach();
	std::thread t2(&Dispatcher::AcceptClient, this, SERVERPORT2, serverSock2);
	t2.join();

	// ��������
	Sleep(2000);
	WSACleanup();
}

void Dispatcher::AcceptClient(const int port, const int serverSock)
{
	int clientSock = 0;
	sockaddr_in clientAddr;
	while (1) {
		// ��������
		int sin_size = sizeof(struct sockaddr_in);
		if (-1 == (clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &sin_size))) {
			std::cerr << WSAGetLastError() << std::endl;
			closesocket(clientSock);
			perror("accept wrong");
			break;
		}
		char *clientAddr_s = inet_ntoa(clientAddr.sin_addr); // �ǵ�SDL���ĳɷ�
		printf("Get connection from %s\n", clientAddr_s);

		std::thread t(&Dispatcher::RecvClient, this, port, serverSock, clientSock, clientAddr_s);
		t.detach();
	}
}

void Dispatcher::RecvClient(const int port, const int serverSock, const int clientSock, char *clientAddr)
{
	while (1) {
		// һ��������Ϣ
		// (1)json���Ȳ���
		int numbytes = 0;
		char lenField[LENFIELDSIZE + 1] = { '\0' };
		if (-1 == (numbytes = recv(clientSock, lenField, LENFIELDSIZE, 0))) {
			std::cerr << WSAGetLastError() << std::endl;
			closesocket(clientSock);
			if (port == SERVERPORT1) { // �쳣�Ͽ�Ҳ�����˷�
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
				printf("Room %d checked out.\n", roomId);					//	�˿��˷�
				if (roomInfo[roomId]->GetState() != Off)
					StopServe(roomId, Off); // �ػ�
				roomInfo.erase(roomId);
				socket_roomId.erase(clientSock);

				dbfacade.setRegist_signOut(getRoomIdString(roomId), getDate(true));
			}
			return;
		}
		lenField[numbytes] = '\0';
		std::cout << clientAddr << ": " << lenField << std::endl;

		// (2)���ݲ���
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



		// ������Ӧ�ͻ��˲���
		if (port == SERVERPORT1) { // ��Ӧ�˿�ʹ�ÿյ�������
			if (state == ready) {
				int retcode = Analyze1(recvBuf, clientSock, clientAddr);
			}
			else if (state == setMode)
				printf("But the state is \"setMode\" now.\n");
			else
				printf("But the state is \"off\" now.\n");
		}
		else if (port == SERVERPORT2) { // ��Ӧ����Ա��ǰ̨������Ĳ���
			int retcode = Analyze2(recvBuf, clientSock);
			if (-1 == retcode) { // ����Ա������
				// closesocket(serverSock);
			}
		}
	}
}

/* �����ͻ��˷�����JSON�� */
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

		if (requestType == PowerOn) {				// �˿Ϳ���
			printf("Room: %d PowerOn\n", roomId);

			Json::Value root;
			if (roomInfo.end() == roomInfo.find(roomId)) { // ����
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

			// ���˿ͻظ�
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
			else { // ��ʼ����
				roomInfo[roomId]->Set(Hanging, initTemp, defaultTargetTemp, defaultFanSpeed, defaultFanSpeed);
				waitingQueue.push_back(roomInfo[roomId]);
			}
		}
		else if (requestType == PowerOff) {			// �˿͹ػ�
			printf("Room: %d PowerOff\n", roomId);
			if (roomInfo[roomId]->GetState() == Off)
				printf("But the room %d has already powered off. The operation is invalid.\n", roomId);
			else {
				StopServe(roomId, Off);

				dbfacade.setRecord_On_Off(getRoomIdString(roomId), getDate(false));
			}
		}
		else if (requestType == ChangeTemp) {		// �˿͵���
			printf("Room: %d ChangeTemp\n", roomId);
			ChangeTempFanSpeed(roomId, Temp, targetTemp, targetFanSpeed);
			CustomerRequestFee(clientSock, roomId);
		}
		else if (requestType == ChangeFanSpeed) {	// �˿͵���
			printf("Room: %d ChangeFanSpeed\n", roomId);
			ChangeTempFanSpeed(roomId, FanSpeed, targetTemp, targetFanSpeed);
			CustomerRequestFee(clientSock, roomId);
		}
		else if (requestType == RequestFee) {		// �˿��������
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

		// ����Ա����
		if (method == "PowerOn") {
			bool value = root["Value"].asBool();
			if (value) {											// ����Ա������
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
			else {													// ����Ա������
				printf("Administrator: PowerOff\n");

				//Json::Value root;
				//root["Method"] = "ACK";
				int retcode = 0;
				if (state != off) {
					state = off;
					isClose = true;

					std::map<int, int>::iterator iter1 = socket_roomId.begin();
					for (; iter1 != socket_roomId.end(); ) {
						StopServe(iter1->second, Off);	//	�رշ���յ�
						closesocket(iter1->first);		// �Ͽ�����
						socket_roomId.erase(iter1++);
						//dbfacade.setRegist_signOut(getRoomIdString(iter1->second), getDate(true)); // ǿ���˷�
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
		else if (method == "Init") {								// ����Ա��ʼ����������
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
					std::thread t(&Dispatcher::TimeSlot, this); // ������ִ��ȫͬ��ʱ��Ƭ����
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
		else if (method == "Check") {								// ����Ա�����������Ϣ
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

		// ǰ̨����
		if (method == "GetRDR") {											//ǰ̨��ȡ�굥
			Json::Value temp;
			std::string roomId_s = root["RoomID"].asString();
			std::string roomId_s1 = roomId_s.substr(2, roomId_s.size() - 2); // ����������
			std::cout << roomId_s1 << std::endl;
			if (roomInfo.end() != roomInfo.find(atoi(roomId_s1.c_str()))) {	// ��û�˷�
				temp["Method"] = "ReturnRDR";
				temp["failure"] = "still_in";
				printf("Room %s doesn't sign out\n", roomId_s.c_str());
			}
			else if (0 == dbfacade.checkRoom_id(roomId_s)) {// û������
				temp["Method"] = "ReturnRDR";
				temp["failure"] = "no_room";
				printf("Room %s doesn't exist\n", roomId_s.c_str());
			}
			else
				temp = ReturnRDR(roomId_s);
			SendJson(temp, clientSock);
		}
		else if (method == "GetInvoice") {									//ǰ̨��ȡ�˵�
			Json::Value temp;
			std::string roomId_s = root["RoomID"].asString();
			std::string roomId_s1 = roomId_s.substr(2, roomId_s.size() - 2); // ����������
			if (roomInfo.end() != roomInfo.find(atoi(roomId_s1.c_str()))) {// ��û�˷�
				temp["Method"] = "ReturnInvoice";
				temp["failure"] = "still_in";
				printf("Room %s doesn't sign out\n", roomId_s.c_str());
			}
			else if (0 == dbfacade.checkRoom_id(roomId_s)) {// û������
				temp["Method"] = "ReturnInvoice";
				temp["failure"] = "no_room";
				printf("Room %s doesn't exist\n", roomId_s.c_str());
			}
			else
				temp = ReturnInvoice(roomId_s);
			SendJson(temp, clientSock);
		}

		// ������
		if (method == "GetReport") {										//ǰ̨��ȡ����
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