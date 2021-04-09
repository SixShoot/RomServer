#pragma once
#include "../AccountAgent_Srv/AccountPackageDef.h"
#include "ReaderClass/ReaderClass.h"
using namespace NetWakerAccountPackage;
//////////////////////////////////////////////////////////////////////////
//存取DB角色帳號資料的結構
struct PlayerAccountDBStruct
{
	int						GameID;
	char					Account_ID  [ _DEF_MAX_ACCOUNTNAME_SIZE_ ];    
	char					Password    [ _DEF_MAX_PASSWORD_SIZE_ ];      
	AccountStateEnum		AccountState;  
	AccountFreezeTypeEnum	FreezeType;
	int						LastLoginGameID;
	bool					IsMd5Password;
	bool					IsAutoConvertMd5;
	int						UserState;
	float					ValidTime;
	float					Age;
	float					VivoxLicenseTime;
	bool					ResetPassword;
	float					FreezeUntil;

	static ReaderClass<PlayerAccountDBStruct>* Reader( );

};
//-------------------------------------------------------------------------------------
//處理DB登入資料的結構
struct PlayerLoginDBStruct
{
	char    Account_ID      [ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
	char    IP[32];
	char    Time[32];   

	LoginResultEnum       LoginResult;

	static ReaderClass<PlayerLoginDBStruct>* Reader( );

	void Init()
	{
		memset( this , 0 , sizeof(*this) );
	};
};

//-------------------------------------------------------------------------------------
//
struct PasswordCheckRecordStruct
{
	char Account_ID[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
	int  LastCheckTime;
	int  CheckFailedTimes;

	PasswordCheckRecordStruct()
	{
		memset( this , 0 , sizeof(*this) );
	}
};