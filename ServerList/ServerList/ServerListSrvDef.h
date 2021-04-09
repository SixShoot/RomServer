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
		EM_ConnectState_NoUse				,		//�|���ϥ�
		EM_ConnectState_Connect				,		//�w�s�u
		EM_ConnectState_CheckAccount		,		//�b���ˬd
		EM_ConnectState_CheckAccountFailed	,		//�b���n�J�����D
		EM_ConnectState_Ready				,		//���߳s�u

		EM_ConnectState_Proxy_ConnectTo	= 10	,	//�s��Yserver
		EM_ConnectState_Proxy_OnConnect		,		//���߳s�u

		EM_ConnectState_Proxy_ConnectTo_CliDisconnect,

	};

	//Server �ݪ����( Switch )
	struct ServerInfoStruct
	{
		ConnectStateENUM	State;
		string  ServerName;
		string  ServerIP;					//Switch IP
		int		CliPort;					//Switch �� Client �s�� Port
		int		MaxPlayerCount;				//�̤j�i�W�u�H��
		vector	< int >		PlayerCount;	//�C�ӥ���Ŷ����H��
		int		UpdateTime;					//�̫��s�ɶ�
		int		GameID;						//�@��ID
		int		NetID;
		int		ServerStatus;				//�O���A�����A
		float	TotalPlayerRate;

		ServerInfoStruct()
		{
			State = EM_ConnectState_NoUse;
		}
	};
	//-------------------------------------------------------------------------------------
	// Client �ݪ����
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

