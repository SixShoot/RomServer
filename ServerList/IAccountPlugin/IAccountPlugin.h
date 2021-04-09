#pragma once
#include <string>
#include "IniFile\IniFile.h"
#include "ReaderClass/CreateDBCmdClass.h"
#include "DBStruct/DBStructTransfer.h"
#include "ReaderClass/CreateDBCmdClass.h"
#include "../IAccountPlugin/AccountBaseDef.h"
using namespace std;

enum AccountResultENUM
{
	EM_AccountResult_ReloadServerList = -2,
	EM_AccountResult_UnKnow   = -1	,
	EM_AccountResult_Success  = 0	,
	EM_AccountResult_NotFind		,
	EM_AccountResult_Relogin		,
	EM_AccountResult_PasswordErr	,
	EM_AccountResult_ValidTimeErr	,
	EM_AccountResult_IPBlock		,
	EM_AccountResult_CommLock = 9	,	//通訊鎖鎖定中

	EM_AccountResult_Freeze_Self	= 100,
	EM_AccountResult_Freeze_Official,
	EM_AccountResult_Freeze_Other3	,
	EM_AccountResult_Freeze_Other4	,
	EM_AccountResult_Freeze_Other5	,
	EM_AccountResult_Freeze_Other6	,
	EM_AccountResult_Freeze_Other7	,
	EM_AccountResult_Freeze_Other8	,
	EM_AccountResult_Freeze_Other9	,
	
	EM_AccountResult_IllegalMacAddress	= 199,	// 拒絕該 MAC 登入
	EM_AccountResult_Banned			= 200,
	EM_AccountResult_BetaKeyRequest	,	//K2錯誤
	EM_AccountResult_Gama_Wrong_OTP			,	//橘子錯誤 Wrong_OTP : One time password is not correct
	EM_AccountResult_Gama_Wrong_EventOTP	,
	EM_AccountResult_VN_Wrong_UserState		,	//越南錯誤 帳號沒有認證
	EM_AccountResult_Gama_Wrong_Timeout		,	
	EM_AccountResult_Gama_AccountNotFind	,

	EM_AccountResult_KR_AgeRestriction	 = 300,		//韓國登入 - 年齡限制
};

enum IAccountPluginNotifySetDataENUM
{
	EM_SetData_None					= 0,
	EM_SetData_Age					= 1,
	EM_SetData_ValidTime			= 2,
	EM_SetData_VivoxLicenseTime		= 3,
	EM_SetData_PasswordErrorCount	= 4,
	EM_SetData_FreezeRemainingTime	= 5
};

class IAccountPluginNotify
{
public:
	IAccountPluginNotify(){};
	virtual ~IAccountPluginNotify(){};
	virtual void ResetPassword( int NetID ){};
	virtual void CheckAccountResult( int NetID , int WorldID , string Account , AccountResultENUM Result ){};
	virtual void AccountLoginResult( void* UserObj , string Account , string LastLoginIP , string LastLoginTime , AccountResultENUM Result ){};
	virtual void AccountLogoutResult( string Account , AccountResultENUM Result ){};
	virtual void SetData( int NetID , int Type , int Value ){};
	virtual void SetData( string Account , int Type , int Value ){};
};


class IAccountPlugin
{
public:
	//用來都執行序用的
	static unsigned	_AccountValue( string Account );	
	static int		TimeZone() { return _TimeZone; };
protected:
	static int		_TimeZone;
	IAccountPluginNotify *_pEventNotify;
	DBTransferClass*                                _DBTransfer;        //db傳送資料的物件
	DBStructTransferClass< PlayerAccountDBStruct >* _AcountInfoDB;      //db轉結構的物件
	DBStructTransferClass< int >*					_NormalDB;

	CreateDBCmdClass<PlayerAccountDBStruct>*		_RDPlayerAcccountBaseSql;

	//讀取個Server人數
	static bool _SQLCmdRoleCountEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdRoleCountEventResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

public:

	IAccountPlugin( IAccountPluginNotify *notify , IniFileClass	*ini );
	virtual ~IAccountPlugin( void );


	virtual	void CheckAccount( int UserID , string Account , string Password , string IPStr, bool HashedPassword ){};
	virtual	void Login( void* UserObj , string Account , string Password , string IPStr , int GameID, string MacAddress, bool HashedPassword ){};
	virtual	void Logout( string Account , string IPStr , int LoginTime, string MacAddress ){};
	virtual void Process();
	virtual void AccountServerInit(){};
	virtual void LogoutAllAccount(){};
	virtual void AccountFreeze( string UserAccount , int FreezeType ){};
	DBStructTransferClass< int >*	NormalDB() { return _NormalDB;};
};

