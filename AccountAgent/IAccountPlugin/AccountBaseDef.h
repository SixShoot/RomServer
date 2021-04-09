#pragma once
#include "../AccountAgent_Srv/AccountPackageDef.h"
#include "ReaderClass/ReaderClass.h"
using namespace NetWakerAccountPackage;
//////////////////////////////////////////////////////////////////////////
//�s��DB����b����ƪ����c
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
//�B�zDB�n�J��ƪ����c
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