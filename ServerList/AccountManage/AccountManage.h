#pragma once
#include "../IAccountPlugin/IAccountPlugin.h"
#include "functionschedular/functionschedular.h"
#include "ReaderClass/DbSqlExecClass.h"

using namespace std;
//////////////////////////////////////////////////////////////////////////
class AccountManage : public IAccountPlugin
{
	bool	_IsMakePasswordOK;
	int		_PlayerLoginInfoClearDay;
	int		_MaxPasswordErrorCount;
	int		_FreezeAccountTime;

	//讀取角色帳號資訊
	//Check
	static void  _DBReadCheckAccountDBEventCB  ( vector<PlayerAccountDBStruct>& Data , void* UserObj , char *WhereCmd , bool ResultOK );

	//////////////////////////////////////////////////////////////////////////
	static bool _SQLCmdCheckAccountProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdCheckAccountProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdLoginProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoginProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdLogoutProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLogoutProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//更新密碼處理
	static bool _SQLCmdMakeMd5PasswordEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdMakeMd5PasswordResultEvent ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static int  _OnTimeClear_PlayerLoginInfo( SchedularInfo* Obj , void* InputClass );

public:
	AccountManage( IAccountPluginNotify *notify , IniFileClass	*ini );
	virtual ~AccountManage( void );

public:
	virtual	void CheckAccount( int NetID , string Account , string Password , string IPStr, bool HashedPassword );
	virtual	void Login( void* UserObj , string Account , string Password , string IPStr , int GameID, string MacAddress, bool HashedPassword );
	virtual	void Logout( string Account , string IPStr , int LoginTime, string MacAddress );

public:
	virtual void AccountServerInit();
	virtual void LogoutAllAccount();
	virtual void AccountFreeze( string UserAccount , int FreezeType );

	static void PlayerLoginInfoLog( MyDbSqlExecClass& MyDBProc , const char* Account_ID , const char* IP , LoginResultEnum Result, const char* MacAddress );
};
