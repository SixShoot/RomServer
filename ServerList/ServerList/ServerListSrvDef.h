#pragma once
#include "ServerListPackageDef.h"
#include "ReaderClass/ReaderClass.h"
#include <set>
namespace   NetWakerServerListPackage
{

	struct PlayerAccountInfo
	{	
		char    Account_ID      [ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
		char    Password        [ _DEF_MAX_PASSWORD_SIZE_ ];		
	};

	//-------------------------------------------------------------------------------------
	enum	ConnectStateENUM
	{
		EM_ConnectState_NoUse				,		//尚未使用
		EM_ConnectState_Connect				,		//已連線
		EM_ConnectState_CheckAccount		,		//帳號檢查
		EM_ConnectState_CheckAccountFailed	,		//帳號登入有問題
		EM_ConnectState_Ready				,		//成立連線

		EM_ConnectState_Proxy_ConnectTo	= 10	,	//連到某server
		EM_ConnectState_Proxy_OnConnect		,		//成立連線

		EM_ConnectState_Proxy_ConnectTo_CliDisconnect,

	};

	//Server 端的資料( Switch )
	struct ServerInfoStruct
	{
		ConnectStateENUM	State;
		string  ServerName;
		string  ServerIP;					//Switch IP
		int		CliPort;					//Switch 給 Client 連的 Port
		int		MaxPlayerCount;				//最大可上線人數
		vector	< int >		PlayerCount;	//每個平行空間的人數
		int		UpdateTime;					//最後更新時間
		int		GameID;						//世界ID
		int		NetID;
		int		ServerStatus;				//是伺服器狀態
		float	TotalPlayerRate;

		ServerInfoStruct()
		{
			State = EM_ConnectState_NoUse;
		}
	};
	//-------------------------------------------------------------------------------------
	// Client 端的資料
	struct ClientInfoStruct
	{
		ConnectStateENUM	State;
		string	Account;
		string	Password;
		int		Age;
		int		ValidTime;
		int		VivoxLicenseTime;
		int		PasswordErrorCount;
		int		FreezeRemaingTime;

		int					ForwardNetID;
		vector< char >		PacketData;

		ClientInfoStruct()
		{
			State = EM_ConnectState_NoUse;
			Age   = 100;
			ValidTime = -1;
			VivoxLicenseTime = 0;
			PasswordErrorCount = 0;
			FreezeRemaingTime = 0;
		}
	};
	//-------------------------------------------------------------------------------------
}

