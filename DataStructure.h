enum REQKIND { PowerOn = 1, PowerOff, ChangeTemp, ChangeFanSpeed, RequestFee };

typedef struct ClientRequest {
	int RoomID;
	//请求类型
	REQKIND RequestKind;
	//请求内容
	int TargetTemp;
	int FanSpeed;
	int InitTemp;
}ClientRequest;

enum ERRMESSAGE { NoError = 1, DupRoomID };
enum MODETYPE { Refrigeration = 1, Heating };

typedef struct ServerReply {
	MODETYPE mode;
	float Fee;
	float FeeRate;
	ERRMESSAGE ErrMsg;
	int CurrentRoomTemp;
	int default_TargetTemp;
	int Temp_highLimit;
	int Temp_lowLimit;
}ServerReply;
