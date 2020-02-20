#include "Bean.h"
#include <windows.h>
#include <sql.h>              // This is the the main include for ODBC Core functions.
#include <sqlext.h>         // This is the include for applications using the Microsoft SQL Extensions
#include <sqltypes.h>   // This file defines the types used in ODBC
#include <sqlucode.h>   // This is the the unicode include for ODBC Core functions
#include <odbcss.h>      // This is the application include file for the SQL Server driver specific defines.
//#pragma coment(lib, "odbc32.lib")

//数据源及登陆信息
RETCODE retcode;
SQLHENV  henv = NULL;        //环境句柄
SQLHDBC  hdbc = NULL;        //连接句柄
SQLHSTMT hstmt = NULL;       //语句句柄
SQLHDESC hdesc = NULL;       //描述句柄
SQLRETURN rs = 0;            //使用指针的初始化
SQLSMALLINT errmsglen;
SQLINTEGER errnative;
UCHAR errmsg[255];     //错误信息
UCHAR errstate[5];      //错误代码

//数据库连接信息
UCHAR szDSN[SQL_MAX_DSN_LENGTH + 1] = "HotelAirconditioner";     //数据源名
UCHAR szUID[MAXNAME] = "sa";  //登录名
UCHAR szPASSWORD[MAXNAME] = "123";             //密码


Bean::Bean()
{

}


Bean::~Bean()
{

}

bool Bean::applyHenv()
{
	rs = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		printf("环境句柄申请失败");
		system("pause");
		return false;
	}
	return true;
}

bool Bean::applyHdbc()
{
	rs = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	rs = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		printf("连接句柄申请失败");
		system("pause");
		return false;
	}
	return true;
}

bool Bean::applyHstmt()
{
	rs = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		printf("申请语句句柄失败！\n");
		system("pause");
		return false;
	}
	return true;
}

bool Bean::connectDatabase()
{
	rs = SQLConnect(hdbc, szDSN, SQL_NTS, szUID, SQL_NTS, szPASSWORD, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		printf("连接数据源失败！\n");
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		system("pause");
		exit(3);
		system("pause");
		return false;
	}
	return true;

}

void Bean::freeHandleandConnect()
{
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄
	SQLDisconnect(hdbc);//断开与数据源的连接
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄

}

bool Bean::updatePara(float feeRate_L, float feeRate_M, float feeRate_H, int mode, float default_target_temp, float temp_LowLimit, float temp_HighLimit)//修改Para表的参数
{
	//创建句柄并链接数据库
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//两条需要执行的语句
	std::string executeClearSentence = "truncate table para";
	std::string executeInsertSentence = "insert into para values(" + std::to_string(mode) + "," + std::to_string(temp_HighLimit) + "," + std::to_string(temp_LowLimit) + ","
		+ std::to_string(default_target_temp) + "," + std::to_string(feeRate_H) + "," + std::to_string(feeRate_M) + "," + std::to_string(feeRate_L) + ");";
	const char* executeClearSentence_char = executeClearSentence.c_str();
	const char* executeInsertSentence_char = executeInsertSentence.c_str();

	//执行清空para表的语句
	rs = SQLExecDirect(hstmt, (SQLCHAR*)executeClearSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << executeClearSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}

	//执行插入语句
	rs = SQLExecDirect(hstmt, (SQLCHAR*)executeInsertSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << executeInsertSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}
	this->freeHandleandConnect();
	return true;

}

bool Bean::selectCurFee_Regist(std::string room_id, float * returnFee)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();


	//执行语句初始化
	std::string exeSentence = "select fee from regist where room_id = '" + room_id + "'and date_out = '" + default_dateOut + "';";
	const char* exeSentence_char = exeSentence.c_str();

	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}
	SQLBindCol(hstmt, 1, SQL_C_FLOAT, returnFee, sizeof(returnFee), 0);
	while ((retcode = SQLFetch(hstmt)) != SQL_NO_DATA)
	{

	}
	//释放句柄、连接
	this->freeHandleandConnect();
	return true;

}

bool Bean::update_Cur_State(std::string room_id, int on_off, int hang, int serve, float cur_temp, float target_temp, int cur_fan, int target_fan) //修改cur_state表
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	std::string exeSentence = "update cur_state set on_off = " + std::to_string(on_off) + ", hang = " + std::to_string(hang) + ", serve = " + std::to_string(serve) +
		", cur_temp = " + std::to_string(cur_temp) + ", target_temp = " + std::to_string(target_temp) + ", cur_fan = " + std::to_string(cur_fan) + "," + "target_fan = " + std::to_string(target_fan) + " where room_id = '" + room_id + "';";
	const char* exeSentence_char = exeSentence.c_str();

	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}
	this->freeHandleandConnect();
	return true;
}

bool Bean::update_report_powerOn_Off(std::string room_id, std::string curDate)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	std::string exeSentence = "update report set on_off_amount = on_off_amount + 1 where room_id = '" + room_id + "' and date = '" + curDate + "';";
	const char* exeSentence_char = exeSentence.c_str();
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}
	this->freeHandleandConnect();
	return true;

}

bool Bean::update_report_changeFan(std::string room_id, std::string curDate)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	std::string exeSentence = "update report set changeFan_amount = changeFan_amount + 1 where room_id = '" + room_id + "' and date = '" + curDate + "';";
	const char* exeSentence_char = exeSentence.c_str();
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}
	this->freeHandleandConnect();
	return true;
}

bool Bean::update_report_changeTemp(std::string room_id, std::string curDate)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	std::string exeSentence = "update report set changeTemp_amount = changeTemp_amount + 1 where room_id = '" + room_id + "' and date = '" + curDate + "';";
	const char* exeSentence_char = exeSentence.c_str();
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}
	this->freeHandleandConnect();
	return true;
}

bool Bean::update_report_dispatch_amount(std::string room_id, std::string curDate, int change)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	std::string exeSentence = "update report set dispatch_amount = dispatch_amount + " + std::to_string(change) + " where room_id = '" + room_id + "' and date = '" + curDate + "';";
	const char* exeSentence_char = exeSentence.c_str();

	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}
	this->freeHandleandConnect();
	return true;
}

bool Bean::update_report_detail_amount(std::string room_id, std::string curDate)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	std::string exeSentence = "update report set detail_amount = detail_amount + 1 where room_id = '" + room_id + "' and date = '" + curDate + "';";
	const char* exeSentence_char = exeSentence.c_str();
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}
	this->freeHandleandConnect();
	return true;
}


bool Bean::update_report_total_fee(std::string room_id, std::string curDate, float fee)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	std::string exeSentence = "update report set total_fee = total_fee + " + std::to_string(fee) + "-(select sum(total_fee)from report where room_id = '" + room_id + "' and report.date between (select convert(date,date_in,111) from regist where room_id = '" + room_id + "' and date_out = '" + default_dateOut + "') and '" + curDate + "') "
		" where room_id = '" + room_id + "' and date = '" + curDate + "';";
	const char* exeSentence_char = exeSentence.c_str();
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}
	this->freeHandleandConnect();
	return true;
}

bool Bean::update_report_serve_time(std::string room_id, std::string curDate, int time)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	std::string exeSentence = "update report set serve_time = serve_time + " + std::to_string(time) + "-(select sum(serve_time)from report where room_id = '" + room_id + "' and report.date between (select convert(date,date_in,111) from regist where room_id = '" + room_id + "' and date_out = '" + default_dateOut + "') and '" + curDate + "') "
		" where room_id = '" + room_id + "' and date = '" + curDate + "';";
	const char* exeSentence_char = exeSentence.c_str();
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}
	this->freeHandleandConnect();
	return true;
}

bool Bean::insert_detail_newDetail(std::string room_id, std::string requestTime, int fan, float feeRate)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	std::string exeSentence = "insert into detail values('" + room_id + "','" + requestTime + "','" + default_endTime + "'," + std::to_string(fan) + ",0," + std::to_string(feeRate) + ");";
	const char* exeSentence_char = exeSentence.c_str();
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}
	this->freeHandleandConnect();
	return true;
}

bool Bean::update_Detail_endDetail(std::string room_id, std::string endTime, float detailFee)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//先更新endTime值
	std::string exeSentence = "update detail set endTime = '" + endTime + "',fee =  " + std::to_string(detailFee) + " where room_id = '" + room_id + "' and endTime = '" + default_endTime + "';";
	const char* exeSentence_char = exeSentence.c_str();
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}


	this->freeHandleandConnect();
	return true;
}

bool Bean::insert_Regist_signIn(std::string room_id, std::string signInTime)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	std::string exeSentence = "insert into regist values( '" + room_id + "', '" + signInTime + "','" + default_dateOut + "',0);";
	const char * exeSentence_char = exeSentence.c_str();
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}
	this->freeHandleandConnect();
	return true;
}

bool Bean::setRoom_signOut(std::string room_id, std::string signOutTime)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	std::string exeSentence = "update regist set date_out = '" + signOutTime + "'where room_id = '" + room_id + "'and date_out = '" + default_dateOut + "';";
	const char* exeSentence_char = exeSentence.c_str();
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		//getchar();
		return false;
	}
	//std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
	return true;
}

Room_Invoice* Bean::updateAndselect_Regist_signOut(std::string room_id)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//改
	std::string exeSentence = "select top(1)  convert(char(20),date_in,120),convert(char(20),date_out,120),fee from regist where room_id = '" + room_id + "' order by date_out desc";
	const char* exeSentence_char = exeSentence.c_str();
	std::cout << exeSentence;
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		getchar();
		return NULL;
	}
	Room_Invoice * dataBack = new Room_Invoice;
	dataBack->room_id = room_id;
	float * fee = new float;
	SQLBindCol(hstmt, 1, SQL_C_CHAR, dataBack->date_in, sizeof(dataBack->date_in), 0);
	SQLBindCol(hstmt, 2, SQL_C_CHAR, dataBack->date_out, sizeof(dataBack->date_out), 0);
	SQLBindCol(hstmt, 3, SQL_C_FLOAT, fee, sizeof(fee), 0);


	while ((retcode = SQLFetch(hstmt)) != SQL_NO_DATA)
	{

	}
	dataBack->total_fee = *fee;
	this->freeHandleandConnect();
	return dataBack;
}


bool Bean::update_Curfee_Regist(std::string room_id, float fee)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	std::string exeSentence = "update regist set fee = " + std::to_string(fee) + " where room_id = '" + room_id + "' and date_out = '" + default_dateOut + "';";
	const char* exeSentence_char = exeSentence.c_str();
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}
	this->freeHandleandConnect();
	return true;
}

Detail* Bean::selectRoomDetail_Detail(std::string room_id)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	std::string exeSentence = "select convert(char(20),requestTime,120), DATEDIFF(SECOND,requestTime,endTime),fan,fee,feeRate from Detail where room_id = '"
		+ room_id + "' and requestTime between (select top(1).regist.date_in from regist where room_id = '" + room_id + "' order by date_out desc) and (select top(1).regist.date_out from regist where room_id = '" + room_id + "' order by date_out desc) ;";

	const char * exeSentence_char = exeSentence.c_str();
	std::cout << exeSentence << std::endl;
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return NULL;
	}
	char r_time[20];
	short int *duration_data = new short int;
	short int *fan_data = new short int;
	float *fee_data = new float;
	float *feeRate = new float;
	SQLBindCol(hstmt, 1, SQL_C_CHAR, r_time, sizeof(r_time), 0);
	SQLBindCol(hstmt, 2, SQL_C_SSHORT, duration_data, sizeof(duration_data), 0);
	SQLBindCol(hstmt, 3, SQL_C_SSHORT, fan_data, sizeof(fan_data), 0);
	SQLBindCol(hstmt, 4, SQL_C_FLOAT, fee_data, sizeof(fee_data), 0);
	SQLBindCol(hstmt, 5, SQL_C_FLOAT, feeRate, sizeof(feeRate), 0);

	Detail* dataList = new Detail;
	dataList->next = NULL;
	Detail * cur_ptr = dataList;
	while ((retcode = SQLFetch(hstmt)) != SQL_NO_DATA)
	{
		cur_ptr->room_id = room_id;
		strcpy_s(cur_ptr->start_time, strlen(r_time) + 1, r_time);
		cur_ptr->duration = *duration_data;
		cur_ptr->fan = *fan_data;
		cur_ptr->fee = *fee_data;
		cur_ptr->feeRate = *feeRate;

		Detail * new_Ptr = new Detail;
		new_Ptr->next = NULL;
		cur_ptr->next = new_Ptr;
		cur_ptr = cur_ptr->next;

	}
	Detail * str = dataList;
	if (str->next == NULL)
	{

	}
	else
	{
		while (str->next->next != NULL)
		{
			str = str->next;
		}
		str->next = NULL;
	}
	this->freeHandleandConnect();
	return dataList;

}

Room_record* Bean::selectRoomReport_Report(std::string room_id, int reportType, std::string endDate)
{
	std::string exeSentence;
	if (reportType == 1)
	{
		exeSentence = "select convert(char(20),report.date,111),on_off_amount,serve_time,total_fee,dispatch_amount,detail_amount,changeTemp_amount,changeFan_amount  from report where room_id = '" + room_id + "' and date between DATEADD(day,-1,'" + endDate + "') and '" + endDate + "';";

	}
	else if (reportType == 2)
	{
		exeSentence = "select convert(char(20),report.date,111),on_off_amount,serve_time,total_fee,dispatch_amount,detail_amount,changeTemp_amount,changeFan_amount  from report where room_id = '" + room_id + "' and date between DATEADD(day,-7,'" + endDate + "') and '" + endDate + "';";

	}
	else if (reportType == 3)
	{
		exeSentence = "select convert(char(20),report.date,111),on_off_amount,serve_time,total_fee,dispatch_amount,detail_amount,changeTemp_amount,changeFan_amount  from report where room_id = '" + room_id + "' and date between DATEADD(day,-30,'" + endDate + "') and '" + endDate + "';";

	}
	else if (reportType == 4)
	{
		exeSentence = "select convert(char(20),report.date,111),on_off_amount,serve_time,total_fee,dispatch_amount,detail_amount,changeTemp_amount,changeFan_amount  from report where room_id = '" + room_id + "' and date between DATEADD(day,-365,'" + endDate + "') and '" + endDate + "';";

	}
	else {
		std::cout << "错误的报表种类参数,退出" << std::endl;
		return NULL;
	}

	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	const char * exeSentence_char = exeSentence.c_str();
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return NULL;
	}

	char date_data[20];
	short int *on_off_amount = new short int;
	short int *serve_time = new short int;
	float * total_fee = new float;
	short int *dispatch_amount = new short int;
	short int * detail_amount = new short int;
	short int * changeTemp_amount = new short int;
	short int * changeFan_amount = new short int;

	SQLBindCol(hstmt, 1, SQL_C_CHAR, date_data, sizeof(date_data), 0);
	SQLBindCol(hstmt, 2, SQL_C_SSHORT, on_off_amount, sizeof(on_off_amount), 0);
	SQLBindCol(hstmt, 3, SQL_C_SSHORT, serve_time, sizeof(serve_time), 0);
	SQLBindCol(hstmt, 4, SQL_C_FLOAT, total_fee, sizeof(total_fee), 0);
	SQLBindCol(hstmt, 5, SQL_C_SSHORT, dispatch_amount, sizeof(dispatch_amount), 0);
	SQLBindCol(hstmt, 6, SQL_C_SSHORT, detail_amount, sizeof(detail_amount), 0);
	SQLBindCol(hstmt, 7, SQL_C_SSHORT, changeTemp_amount, sizeof(changeTemp_amount), 0);
	SQLBindCol(hstmt, 8, SQL_C_SSHORT, changeFan_amount, sizeof(changeFan_amount), 0);

	Room_record* dataList = new Room_record;
	dataList->next = NULL;
	Room_record* cur_ptr = dataList;
	while ((retcode = SQLFetch(hstmt)) != SQL_NO_DATA)
	{
		cur_ptr->room_id = room_id;
		strcpy_s(cur_ptr->date, strlen(date_data) + 1, date_data);
		cur_ptr->amount_on_off = *on_off_amount;
		cur_ptr->using_time = *serve_time;
		cur_ptr->total_fee = *total_fee;
		cur_ptr->amount_dispatch = *dispatch_amount;
		cur_ptr->amonut_detail = *detail_amount;
		cur_ptr->amount_changeTemp = *changeTemp_amount;
		cur_ptr->amount_changeFan = *changeFan_amount;

		Room_record* new_Ptr = new Room_record;
		new_Ptr->next = NULL;
		cur_ptr->next = new_Ptr;
		cur_ptr = cur_ptr->next;

	}

	Room_record * str = dataList;
	if (str->next == NULL)
	{

	}
	else
	{
		while (str->next->next != NULL)
		{
			str = str->next;
		}
		str->next = NULL;
	}
	this->freeHandleandConnect();
	return dataList;

}


bool Bean::newRoom(std::string room_id, std::string date, float cur_temp, float target_temp)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	//执行语句
	std::string exeSentence = "insert into cur_state values('" + room_id + "',0,0,0," + std::to_string(cur_temp) + "," + std::to_string(target_temp) + ",1,1); insert into report values ('" + room_id + "','" + date + "',1,0,0,0,0,0,0);";
	const char* exeSentence_char = exeSentence.c_str();
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return false;
	}
	this->freeHandleandConnect();
	return true;

}

void Bean::initDatabase()
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	std::string exeSentence = "truncate table report; truncate table regist; truncate table cur_state; truncate table Detail; truncate table para; ";
	const char * exeSentence_char = exeSentence.c_str();
	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return;
	}
	this->freeHandleandConnect();
	return;

}

int Bean::checkRoom_id(std::string checkRoom_id)
{
	//句柄、连接初始化
	this->applyHenv();
	this->applyHdbc();
	this->connectDatabase();
	this->applyHstmt();

	std::string exeSentence = "select room_id from cur_state";
	const char* exeSentence_char = exeSentence.c_str();

	rs = SQLExecDirect(hstmt, (SQLCHAR*)exeSentence_char, SQL_NTS);
	if (rs != SQL_SUCCESS && rs != SQL_SUCCESS_WITH_INFO)
	{
		std::cout << "语句" << exeSentence << "执行失败！" << std::endl;
		rs = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, errstate,
			&errnative, errmsg, sizeof(errmsg), &errmsglen);
		std::cout << "Failed   " << errstate << "  " << errnative << "  " << errmsg << std::endl;
		this->freeHandleandConnect();
		return -1;
	}

	char roomId[5] = "/0";
	SQLBindCol(hstmt, 1, SQL_C_CHAR, roomId, sizeof(roomId), 0);

	while ((retcode = SQLFetch(hstmt)) != SQL_NO_DATA)
	{
		std::string roomId_s = roomId;

		if (roomId_s == checkRoom_id)
		{
			std::cout << "房间" << checkRoom_id << "为已退房的房间" << std::endl;
			this->freeHandleandConnect();
			return 1;
		}
	}
	std::cout << "房间" << checkRoom_id << "为未开过房的房间" << std::endl;
	this->freeHandleandConnect();
	return 0;
}
