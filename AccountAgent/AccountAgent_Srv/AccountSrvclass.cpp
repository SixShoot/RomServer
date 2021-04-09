#include "AccountSrvClass.h"
#include <time.h>
#include "readerclass\DbSqlExecClass.h"
#include "md5\Mymd5.h"
#include "../AccountManage/AccountManage.h"
//#include "../GamaAccountManage/GamaAccountManage.h"
//#include "../K2AccountManage/K2AccountManage.h"
//#include "../KRAccountManage/KRAccountManage.h"
//#include "../AeriaAccountManager/AeriaAccountManager.h"
//#include "../GameWakerAccountManage/GameWakerAccountManage.h"
//#include "../GFAccountManage/GFAccountManage.h"

#define		_Max_PlayTimeQuota_	(60*60*5)	//五小時為上限Quota
NetPGCallBack   AccountClass::_PGCallBack[ EM_AccountNet_Packet_Count ];
//----------------------------------------------------------------------------------------------
bool		LBServers::OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData )
{
    return _Parent->_OnPacket( dwNetID , dwPackSize , pPacketData );
}
void		LBServers::OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode )
{
    _Parent->_OnConnectFailed( dwFailedCode );
}
void		LBServers::OnConnect		( DWORD dwNetID )
{
   _Parent->_OnConnected( dwNetID );
}
void		LBServers::OnDisconnect	    ( DWORD dwNetID )
{
    _Parent->_OnDisconnected( dwNetID );
}
CEventObj*	LBServers::OnAccept		    ( DWORD dwNetID )
{
    return this;
}
//-----------------------------------------------------------------------------------
// 初始化 Client 與 Server 的 CallBack函式
//-----------------------------------------------------------------------------------
void	AccountClass::InitPGCallBack( )
{
    int		i;
    //callback 函式陣列出始化
    for( i = 0 ; i <EM_AccountNet_Packet_Count ; i++ )
    {
        _PGCallBack[i] = &AccountClass::_PGxUnKnoew;
    }

    _PGCallBack[ EM_AccountNet_CtoS_GSrvLogin           	] = &AccountClass::_AccountNet_CtoS_GSrvLogin;
    _PGCallBack[ EM_AccountNet_CtoS_PlayerLogin         	] = &AccountClass::_AccountNet_CtoS_PlayerLogin;
    _PGCallBack[ EM_AccountNet_CtoS_PlayerLogout        	] = &AccountClass::_AccountNet_CtoS_PlayerLogout;
    _PGCallBack[ EM_AccountNet_CtoS_CheckAccountResult  	] = &AccountClass::_AccountNet_CtoS_CheckAccountResult;
	_PGCallBack[ EM_AccountNet_CtoS_FreezeAccount			] = &AccountClass::_AccountNet_CtoS_FreezeAccount;
}
//------------------------------------------------------------------------------------------------------------------
//用來都執行序用的
int		AccountClass::_AccountValue( char* Account )
{
	_strlwr( Account );

	int		Value = 0;
	for( int i = 0 ; ; i++ )
	{
		if( Account[i] == 0 )
			return Value;
		Value = Account[i];
	}
	return 0;
}

void AccountClass::_OnConnected( DWORD NetID )
{
	GameSrvInfo TempSrvInfo;
	while( _GSrvList.size() <= NetID )
	{
		_GSrvList.push_back( TempSrvInfo );
	}
	GameSrvInfo* SrvInfo = &( _GSrvList[ NetID ] );
	SrvInfo->Init();
	SrvInfo->SockID = NetID;
}
void AccountClass::_OnDisconnected( DWORD NetID )
{
	//把此 GameID 的資料全部清除
	GameSrvInfo TempSrvInfo;
	GameSrvInfo* SrvInfo = &( _GSrvList[ NetID ] );


	std::set< PlayerAccountInfo* >::iterator Iter;
	for( Iter = SrvInfo->PlayerInfo.begin() ; Iter != SrvInfo->PlayerInfo.end() ; Iter++ )
	{
		_AccountPlugin->Logout( (*Iter)->Account_ID , (*Iter)->LoginIP , (*Iter)->LoginTime_Value, (*Iter)->MacAddress );
		_AccountMap.erase( (*Iter)->Account_ID );
	}
	_GameIDSet.erase( SrvInfo->GameID );
}
bool AccountClass::_OnPacket( DWORD NetID , DWORD Size , void* Data )
{
	AccountNet_SysNull* PG = ( AccountNet_SysNull *) Data;

	//------------------------------------------------------------------------------
	if(EM_AccountNet_Packet_Count <= (UINT32)PG->Command )
	{
		//printf("\n_OnPacket : packet cmd=%d too big" , NetID , PG->Command );
		Print(LV2, "AccountClass", "_OnPacket : packet cmd=%d too big" , NetID , PG->Command);
		return false;
	}
	//呼叫Callback 處理
	(this->*_PGCallBack[PG->Command])( NetID , PG , Size );
	//------------------------------------------------------------------------------
	return true;
}
void AccountClass::_OnConnectFailed( int NetID )
{

}
void AccountClass::_AccountNet_CtoS_GSrvLogin(  int NetID , AccountNet_SysNull* Data , int Size )
{
	AccountNet_CtoS_GSrvLogin* PG = (AccountNet_CtoS_GSrvLogin*)Data;
	GameSrvInfo* SrvInfo = &( _GSrvList[ NetID ] );

	//printf( "\n_AccountNet_CtoS_GSrvLogin Name=%s  ID=%d" , PG->Name , PG->GameID );
	Print(LV2, "AccountClass", "_AccountNet_CtoS_GSrvLogin Name=%s  ID=%d" , PG->Name , PG->GameID );

	set< int >::iterator Iter;
	AccountNet_StoC_GSrvLoginResult Send;

	Send.GameID = PG->GameID;

	Iter = _GameIDSet.find( PG->GameID );
	if( _GameIDSet.end() != Iter )
	{     
		Send.Result = EM_GSrvLoginResult_RegTwice;
		_BNet->Send( NetID , sizeof( Send ) , &Send  );
		return;
	}
	_GameIDSet.insert( PG->GameID );

	SrvInfo->GameID = PG->GameID;
	strncpy( SrvInfo->Name , PG->Name , sizeof( SrvInfo->Name ) );

	Send.Result = EM_GSrvLoginResult_OK;
	_BNet->Send( NetID , sizeof( Send ) , &Send  );
}
void AccountClass::_AccountNet_CtoS_PlayerLogin( int NetID , AccountNet_SysNull* Data , int Size )
{
	AccountNet_CtoS_PlayerLogin* PG = (AccountNet_CtoS_PlayerLogin*)Data;
	GameSrvInfo* SrvInfo = &( _GSrvList[ NetID ] );

	Print( LV2 , "_AccountNet_CtoS_PlayerLogin" , "Account=%s UserID = %d " , PG->Account , PG->UserID );

	if( strlen( PG->MacAddress ) > 12 )
	{
		Print( LV5 , "_AccountNet_CtoS_PlayerLogin" , "mac = %s" , PG->MacAddress );
		PG->MacAddress[0] = 0;
	}
//	PG->MacAddress[0] = 0;

	AccountNet_StoC_PlayerLoginResult Send;
	Send.UserID = PG->UserID;
	memcpy( Send.Account , &PG->Account , sizeof( Send.Account ) );

	if( CheckSqlStr( PG->Account ) == false )
	{
		Send.Result = EM_LoginResul_AccountStrError;
		_BNet->Send( NetID , sizeof( Send ) , &Send  );
		return;
	}
	if( SrvInfo->GameID == -1 )
	{
		Send.Result = EM_LoginResul_SwitchError;
		_BNet->Send( NetID , sizeof( Send ) , &Send  );
		return;
	}

	map< string , PlayerAccountInfo* >::iterator Iter = _AccountMap.find( PG->Account );
	if( Iter != _AccountMap.end() )
	{
		Print( LV2 ,"_AccountNet_CtoS_PlayerLogin" ,  "Relogin Account = %s" , PG->Account );
		Send.Result = EM_LoginResul_LoginTwice;
		_BNet->Send( NetID , sizeof( Send ) , &Send  );

		//要求檢查登入狀態
		PlayerAccountInfo* AcInfo = Iter->second;
		AccountNet_StoC_CheckAccount    Send;
		strncpy( Send.Account , AcInfo->Account_ID , sizeof( Send.Account ) );
		_BNet->Send( AcInfo->GameID , sizeof( Send ) , &Send  );

		return;
	}
	//------------------------------------------------------------------------------------------
	PlayerAccountInfo* PlayInfo = _PlayerAccountInfoBin.NewObj();
	PlayInfo->Init();

	PlayInfo->GameID = SrvInfo->GameID;

	strncpy( PlayInfo->Account_ID,	PG->Account,	sizeof( PlayInfo->Account_ID ) );
	strncpy( PlayInfo->Password,	PG->Password,	sizeof( PlayInfo->Password ) );
	strncpy( PlayInfo->MacAddress,	PG->MacAddress, sizeof( PlayInfo->MacAddress ) );

	PlayInfo->AccountState = EM_AccountState_OnLogin;
	PlayInfo->TempUserID = PG->UserID;
	PlayInfo->NetID   = NetID;
	PlayInfo->LoginTime_Value = TimeStr::Now_Value( IAccountPlugin::TimeZone() );
	//in_addr IpAddr;
	//IpAddr.S_un.S_addr = PG->IPnum;

	ConvertIPtoString( PG->IPnum, PlayInfo->LoginIP );
	//strncpy( PlayInfo->LoginIP , inet_ntoa( IpAddr ) , sizeof(PlayInfo->LoginIP) );
	strncpy( PlayInfo->LoginTime , TimeStr::Now() , sizeof(PlayInfo->LoginTime) );

	SrvInfo->PlayerInfo.insert( PlayInfo );
	_AccountMap[ PG->Account ] = PlayInfo;
	//------------------------------------------------------------------------------------------
	char IPStr[32];
	//sprintf( IPStr , "%d.%d.%d.%d" , IpAddr.S_un.S_un_b.s_b1 , IpAddr.S_un.S_un_b.s_b2 , IpAddr.S_un.S_un_b.s_b3 , IpAddr.S_un.S_un_b.s_b4 );
	strncpy( IPStr, PlayInfo->LoginIP, sizeof( IPStr ) );
	_AccountPlugin->Login( PlayInfo,  PG->Account , PG->Password , IPStr , PlayInfo->GameID, PG->MacAddress, PG->HashedPassword );
	
}
void AccountClass::_AccountNet_CtoS_PlayerLogout( int NetID , AccountNet_SysNull* Data , int Size )
{
	AccountNet_CtoS_PlayerLogout* PG = (AccountNet_CtoS_PlayerLogout*)Data;

	GameSrvInfo* SrvInfo = &( _GSrvList[ NetID ] );

	//printf( "\n_AccountNet_CtoS_PlayerLogout Account=%s UserID = %d " , PG->Account , PG->UserID );
	Print(LV2, "AccountClass", "_AccountNet_CtoS_PlayerLogout Account=%s UserID = %d " , PG->Account , PG->UserID );

	AccountNet_StoC_PlayerLogoutResult Send;
	memcpy( Send.Account , &PG->Account , sizeof( Send.Account ) );
	Send.UserID = PG->UserID;

	map< string , PlayerAccountInfo* >::iterator Iter;
	Iter = _AccountMap.find( PG->Account );
	if( Iter == _AccountMap.end() )
	{
		//printf( "logout out account not exist , Account = %s" , PG->Account );
		Print(LV2, "AccountClass", "logout out account not exist , Account = %s" , PG->Account );
		Send.Result = EM_GSrvLogoutResult_NoAccount;
		_BNet->Send( NetID , sizeof( Send ) , &Send);
		return;
	}
	_AccountPlugin->Logout( PG->Account , Iter->second->LoginIP , Iter->second->LoginTime_Value, Iter->second->MacAddress );

	SrvInfo->PlayerInfo.erase( Iter->second );
	_PlayerAccountInfoBin.DeleteObj( Iter->second );
	_AccountMap.erase( Iter );
}

void AccountClass::_AccountNet_CtoS_FreezeAccount(  int NetID , AccountNet_SysNull* Data , int Size )
{
	AccountNet_CtoS_FreezeAccount* PG = (AccountNet_CtoS_FreezeAccount*)Data;
	_AccountPlugin->AccountFreeze( PG->UserAccount , PG->FreezeType );
}

void AccountClass::_AccountNet_CtoS_CheckAccountResult(  int NetID , AccountNet_SysNull* Data , int Size )
{
	AccountNet_CtoS_CheckAccountResult* PG = (AccountNet_CtoS_CheckAccountResult*)Data;
	if( PG->Result == false )
	{
		//把有問題的角色踢出
		GameSrvInfo* SrvInfo = &( _GSrvList[ NetID ] );

		//printf( "\n_AccountNet_CtoS_PlayerLogout Account=%s " , PG->Account  );
		Print(LV2, "AccountClass", "_AccountNet_CtoS_PlayerLogout Account=%s " , PG->Account  );

		AccountNet_StoC_PlayerLogoutResult Send;
		memcpy( Send.Account , &PG->Account , sizeof( Send.Account ) );
		Send.UserID = 0;

		map< string , PlayerAccountInfo* >::iterator Iter;
		Iter = _AccountMap.find( PG->Account );
		if( Iter == _AccountMap.end() )
		{
			//printf( "logout out account not exist, Account = %s" , PG->Account );
			Print(LV2, "AccountClass", "logout out account not exist, Account = %s" , PG->Account );
			Send.Result = EM_GSrvLogoutResult_NoAccount;
			_BNet->Send( NetID , sizeof( Send ) , &Send );
			return;
		}
		_AccountPlugin->Logout( PG->Account , Iter->second->LoginIP , Iter->second->LoginTime_Value, Iter->second->MacAddress );

		SrvInfo->PlayerInfo.erase( Iter->second );
		_PlayerAccountInfoBin.DeleteObj( Iter->second );
		_AccountMap.erase( Iter );
	}
}
AccountClass::AccountClass( IniFileClass* Ini )
{
	InitPGCallBack( );
	_BListener   = NEW LBServers( this );
	_BNet        = CreateNetBridge( );

	_MaxPasswordErrorCount = Ini->Int( "MaxPasswordErrorCount" ); //設定密碼輸入錯誤次數上限
	_FreezeAccountTime	= Ini->IntDef( "FreezeAccountTime", 3600);

	int AccountType = Ini->Int( "AccountType" );
	_AccountPlugin = NEW AccountManage( this , Ini );

	_AccountPlugin->LogoutAllAccount( );
	_AccountPlugin->AccountServerInit( );
}
AccountClass::~AccountClass( )
{
	_AccountPlugin->LogoutAllAccount( );

	DeleteNetBridge( _BNet );
	delete _BListener;   
	delete _AccountPlugin;
}
void AccountClass::Host( char* IP,int Port )
{
	int IPNum = ConvertIP( IP );
	_BNet->Host( IPNum , Port , _BListener );
}
void AccountClass::Process( )
{
	static int Clock = 1000000;
	Clock++;
	if( Clock % 100 == 0 )
	{
		//每時分鐘檢查一次帳號資訊
		if( Clock >= 6000 )
		{
			CheckAllAccount();
			Clock = 0;
		}
	}

	CNetBridge::WaitAll( 10 );
	Sleep(1);
	_AccountPlugin->Process(); 

}
void AccountClass::CheckAllAccount()
{
	AccountNet_StoC_CheckAccount    Send;
	map< string , PlayerAccountInfo* >::iterator Iter;

	for( Iter = _AccountMap.begin() ; Iter != _AccountMap.end() ; Iter++ )
	{
		PlayerAccountInfo* AcInfo = Iter->second;

		strncpy( Send.Account , AcInfo->Account_ID , sizeof( Send.Account ) );
		_BNet->Send( AcInfo->NetID , sizeof( Send ) , &Send  );
	}
}

//////////////////////////////////////////////////////////////////////////
void AccountClass::AccountLoginResult( void* UserObj , string Account , string LastLoginIP , string LastLoginTime , AccountResultENUM Result )
{
	PlayerAccountInfo* PlayerData = (PlayerAccountInfo*)UserObj;

	AccountNet_StoC_PlayerLoginResult Send;	
	Send.UserID = PlayerData->TempUserID;	
	memcpy( Send.Account , PlayerData->Account_ID , sizeof(Send.Account) );

	switch( Result )
	{
	case EM_AccountResult_Success:
		Send.Result = EM_LoginResul_OK;
		break;
	case EM_AccountResult_NotFind:		
		Send.Result = EM_LoginResul_NotFind;	
		break;
	case EM_AccountResult_Relogin:
		Send.Result = EM_LoginResul_LoginTwice;	
		break;
	case EM_AccountResult_PasswordErr:
		Send.Result = EM_LoginResul_PasswordError;
		break;
	case EM_AccountResult_CommLock:
		Send.Result = EM_LoginResul_CommLock;
		break;
	case EM_AccountResult_Freeze_Self:
		Send.Result = EM_LoginResul_FreezeType_Self;
		break;
	case EM_AccountResult_Freeze_Official:
		Send.Result = EM_LoginResul_FreezeType_Official;
		break;
	case EM_AccountResult_Freeze_Other3:
		Send.Result = EM_LoginResul_FreezeType_Other3;
		break;
	case EM_AccountResult_Freeze_Other4:
		Send.Result = EM_LoginResul_FreezeType_Other4;
		break;
	case EM_AccountResult_Freeze_Other5:
		Send.Result = EM_LoginResul_FreezeType_Other5;
		break;
	case EM_AccountResult_Freeze_Other6:
		Send.Result = EM_LoginResul_FreezeType_Other6;
		break;
	case EM_AccountResult_Freeze_Other7:
		Send.Result = EM_LoginResul_FreezeType_Other7;
		break;
	case EM_AccountResult_Freeze_Other8:
		Send.Result = EM_LoginResul_FreezeType_Other8;
		break;
	case EM_AccountResult_Freeze_Other9:
		Send.Result = EM_LoginResul_FreezeType_Other9;
		break;
	case EM_AccountResult_Banned:
		Send.Result = EM_LoginResul_Banned;
		break;
	case EM_AccountResult_IllegalMacAddress:
		Send.Result = EM_LoginResul_llegalMacAddress;
		break;
	}

	//回傳登入結果
	_BNet->Send( PlayerData->NetID , sizeof( Send ) , &Send  );

	//登入失敗的狀況回傳額外的資料
	AccountNet_StoC_PlayerLoginFailedData FailedData;
	FailedData.UserID = PlayerData->TempUserID;
	memcpy( FailedData.Account , PlayerData->Account_ID , sizeof(FailedData.Account) );
	FailedData.Result = Send.Result;

	if ((Send.Result == EM_LoginResul_PasswordError) || (Send.Result >= EM_LoginResul_FreezeType_Self && Send.Result <= EM_LoginResul_FreezeType_Other9))
	{
		//有設定密碼輸入錯誤次數上限
		if (_MaxPasswordErrorCount > 0)
		{
			//回傳密碼輸入錯誤次數及上限
			if (PlayerData->PassErrorCount > 0)
			{
				FailedData.Data.PasswordError.Count		 = PlayerData->PassErrorCount;
				FailedData.Data.PasswordError.Max		 = _MaxPasswordErrorCount;
				FailedData.Data.PasswordError.FreezeTime = _FreezeAccountTime;
			}				
		}

		//回傳帳號剩餘凍結時間
		if (PlayerData->FreezeRemainingTime > 0)
		{
			FailedData.Data.Freeze.RemainingTime = PlayerData->FreezeRemainingTime;			
		}

		_BNet->Send( PlayerData->NetID , sizeof( FailedData ) , &FailedData );
	}

	if( Result != EM_AccountResult_Success )
	{
		GameSrvInfo* SrvInfo = &( _GSrvList[ PlayerData->NetID ] );
		SrvInfo->PlayerInfo.erase( PlayerData );
		_AccountMap.erase( PlayerData->Account_ID );
		_PlayerAccountInfoBin.DeleteObj( PlayerData );
	}
}

void AccountClass::AccountLogoutResult( string Account , AccountResultENUM Result )
{

}

void AccountClass::SetData( string Account , int Type , int Value )
{
	map< string , PlayerAccountInfo* >::iterator it = _AccountMap.find(Account.c_str());

	if (it != _AccountMap.end())
	{
		PlayerAccountInfo* PlayerData = it->second;

		if (PlayerData != NULL)
		{
			switch (Type)
			{
			case EM_SetData_PasswordErrorCount:
				PlayerData->PassErrorCount = Value;
				break;
			case EM_SetData_FreezeRemainingTime:
				PlayerData->FreezeRemainingTime = Value;
				break;
			default:
				break;
			}
		}
	}

}