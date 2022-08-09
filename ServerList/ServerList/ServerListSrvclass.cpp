#include "ServerListSrvClass.h"
#include <time.h>
#include "md5\Mymd5.h"
#include "readerclass\DbSqlExecClass.h"
#include "filebase\FileBase.h"


NetPGCallBack   ServerListClass::_SrvPGCallBack[ EM_ServerListNet_Packet_Count ];
NetPGCallBack   ServerListClass::_CliPGCallBack[ EM_ServerListNet_Packet_Count ];
//----------------------------------------------------------------------------------------------

bool		LBServers::OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData )
{
    return _Parent->_OnSrvPacket( dwNetID , dwPackSize , pPacketData );
}
void		LBServers::OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode )
{
    _Parent->_OnSrvConnectFailed( dwFailedCode );
}
void		LBServers::OnConnect		( DWORD dwNetID )
{
   _Parent->_OnSrvConnected( dwNetID );
}
void		LBServers::OnDisconnect	    ( DWORD dwNetID )
{
    _Parent->_OnSrvDisconnected( dwNetID );
}
CEventObj*	LBServers::OnAccept		    ( DWORD dwNetID )
{
    return this;
}

bool		LBClients::OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData )
{
	return _Parent->_OnCliPacket( dwNetID , dwPackSize , pPacketData );
}
void		LBClients::OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode )
{
	_Parent->_OnCliConnectFailed( dwFailedCode );
}
void		LBClients::OnConnect		( DWORD dwNetID )
{
	_Parent->_OnCliConnected( dwNetID );
}
void		LBClients::OnDisconnect	    ( DWORD dwNetID )
{
	_Parent->_OnCliDisconnected( dwNetID );
}
CEventObj*	LBClients::OnAccept		    ( DWORD dwNetID )
{
	return this;
}

bool		LProxyClient::OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData )
{					
	return _Parent->_OnProxyPacket( dwNetID , dwPackSize , pPacketData );
}
void		LProxyClient::OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode )
{
	_Parent->_OnProxyConnectFailed( dwFailedCode );
}
void		LProxyClient::OnConnect			( DWORD dwNetID )
{
	_Parent->_OnProxyConnected( dwNetID );
}
void		LProxyClient::OnDisconnect	    ( DWORD dwNetID )
{
	_Parent->_OnProxyDisconnected( dwNetID );
}
CEventObj*	LProxyClient::OnAccept		    ( DWORD dwNetID )
{
	return this;
}
//----------------------------------------------------------------------------------------------
void ServerListClass::_PGxUnKnoew( int NetID , ServerListNet_SysNull* PG , int Size )
{
	Print( LV5 , "_PGxUnKnoew" , "�ʥ]���`�U ID=%d Size=%d" , NetID , Size );
}
//-----------------------------------------------------------------------------------
// ��l�� Client �P Server �� CallBack�禡
//-----------------------------------------------------------------------------------
void	ServerListClass::InitPGCallBack( )
{
    int		i;
    //callback �禡�}�C�X�l��
    for( i = 0 ; i <EM_ServerListNet_Packet_Count ; i++ )
    {
        _SrvPGCallBack[i] = &ServerListClass::_PGxUnKnoew;
		_CliPGCallBack[i] = &ServerListClass::_PGxUnKnoew;
    }

	//Client�ݫʥ]
	_CliPGCallBack[ EM_ServerListNet_CtoS_ServerListRequest ]	= &ServerListClass::_ServerListNet_CtoS_ServerListRequest;	
	_CliPGCallBack[ EM_ServerListNet_CtoS_ForwardTo ]			= &ServerListClass::_ServerListNet_CtoS_ForwardTo;	
	_CliPGCallBack[ EM_ServerListNet_CtoS_SetVivoxLicenseTime ]	= &ServerListClass::_ServerListNet_CtoS_SetVivoxLicenseTime;
	_CliPGCallBack[ EM_ServerListNet_CtoS_ResetPasswordResult ]	= &ServerListClass::_ServerListNet_CtoS_ResetPasswordResult;	
	_CliPGCallBack[ EM_ServerListNet_CtoS_ClientSystemInfo ]	= &ServerListClass::_ServerListNet_CtoS_ClientSystemInfo;	

	//Server�ݫʥ]
	_SrvPGCallBack[ EM_ServerListNet_SCtoS_RegServerInfo	 ] = &ServerListClass::_ServerListNet_SCtoS_RegServerInfo;			
}	

//-------------------------------------------------------------------------------------
void	ServerListClass::_OnSrvConnected( DWORD NetID )
{
	if( NetID < 0 )
	{	
		Print( LV5 , "_OnSrvConnected" , " ID < 0" );
		return;
	}

	ServerInfoStruct Temp;
	while( (unsigned)NetID >= _SrvInfoList.size() )
	{
		_SrvInfoList.push_back( Temp );
	}

	_SrvInfoList[ NetID ].State = EM_ConnectState_Connect;
	_SrvInfoList[ NetID ].NetID = NetID;
}
void	ServerListClass::_OnSrvDisconnected( DWORD NetID )
{
	if( (unsigned)NetID >= _SrvInfoList.size() )
	{
		Print( LV5 , "_OnSrvDisconnected" , "(unsigned)NetID >= _SrvInfoList.size()" );
		return;
	}

	Print( LV1 , "_OnSrvDisconnected" , "ServerName=%s IP=%s  Port=%d" , _SrvInfoList[NetID].ServerName.c_str() , _SrvInfoList[NetID].ServerIP.c_str() , _SrvInfoList[NetID].CliPort );

	switch( _SrvInfoList[NetID].State )
	{
	case EM_ConnectState_NoUse:			//�|���ϥ�
		Print( LV2 , "_OnSrvDisconnected" , "�|���ϥ�???" );
		break;
	case EM_ConnectState_Connect:		//�w�s�u
		Print( LV2 , "_OnSrvDisconnected" , "�w�s�u ���L���������U" );
		break;
	case EM_ConnectState_CheckAccount:	//�b���ˬd
		Print( LV2 , "_OnSrvDisconnected" , "�b���ˬd�� ���L���������U" );
		break;
	case EM_ConnectState_Ready:			//���߳s�u
		break;
	}
	_SrvInfoList[NetID].State = EM_ConnectState_NoUse;

}
bool	ServerListClass::_OnSrvPacket( DWORD NetID , DWORD Size , void* Data )
{
	ServerListNet_SysNull* PG = ( ServerListNet_SysNull *) Data;

	//------------------------------------------------------------------------------

	if(EM_ServerListNet_Packet_Count <= (UINT32)PG->Command )
	{
		Print( LV5 , "_OnSrvPacket" , "�ʥ]cmd=%d�L�j " , NetID , PG->Command );
		return false;
	}
	//�I�sCallback �B�z
	(this->*_SrvPGCallBack[PG->Command])( NetID , PG , Size );

	//------------------------------------------------------------------------------
	return true;

}
void	ServerListClass::_OnSrvConnectFailed( int NetID )
{

}
//-------------------------------------------------------------------------------------
void	ServerListClass::_OnCliConnected( DWORD NetID )
{

	if( NetID < 0 )
	{	
		Print( LV5 , "_OnCliConnected" , " ID < 0" );
		return;
	}

	ClientInfoStruct Temp;
	while( (unsigned)NetID >= _CliInfoList.size() )
	{
		_CliInfoList.push_back( Temp );
	}

	_CliInfoList[ NetID ].State = EM_ConnectState_Connect;
}
void	ServerListClass::_OnCliDisconnected( DWORD NetID )
{
	if( (unsigned)NetID >= _CliInfoList.size() )
	{
		Print( LV5 , "_OnCliDisconnected" , "(unsigned)NetID >= _SrvInfoList.size()" );
		return;
	}
	switch( _CliInfoList[NetID].State )
	{
	case EM_ConnectState_NoUse:			//�|���ϥ�
		Print( LV2 , "_OnCliDisconnected" , "�|���ϥ�???" );
		break;
	case EM_ConnectState_Connect:		//�w�s�u
		Print( LV2 , "_OnCliDisconnected" , "�w�s�u ���L���������U" );
		break;
	case EM_ConnectState_CheckAccount:	//�b���ˬd
		Print( LV2 , "_OnCliDisconnected" , "�b���ˬd�� ���L���������U" );
		break;
	case EM_ConnectState_Ready:			//���߳s�u
		break;

	case EM_ConnectState_Proxy_ConnectTo:		//�s��Yserver
		_CliInfoList[NetID].State = EM_ConnectState_Proxy_ConnectTo_CliDisconnect;
		return;
		break;
	case EM_ConnectState_Proxy_OnConnect:		//���߳s�u
		{
			_ForwardNet->Close( _CliInfoList[NetID].ForwardNetID );
		}
		break;
	}
	_CliInfoList[NetID].State = EM_ConnectState_NoUse;
	_CliInfoList[NetID].PacketData.clear();
}
bool	ServerListClass::_OnCliPacket( DWORD NetID , DWORD Size , void* Data )
{
	ServerListNet_SysNull* PG = ( ServerListNet_SysNull *) Data;

	//////////////////////////////////////////////////////////////////////////
	//��e�ʥ]�B�z
	ClientInfoStruct& CliInfo = _CliInfoList[ NetID ];
	if( CliInfo.State == EM_ConnectState_Proxy_ConnectTo )
	{
		char*	ch = (char*)Data;
		for( int i = 0 ; i < (int)Size ; i++ )
		{
			CliInfo.PacketData.push_back( ch[i] );
		}
		return true;
	}
	else if( CliInfo.State == EM_ConnectState_Proxy_OnConnect )
	{
		_ForwardNet->Send( CliInfo.ForwardNetID , Size , Data );
		return true;
	}
	//------------------------------------------------------------------------------

	if(EM_ServerListNet_Packet_Count <= (UINT32)PG->Command )
	{
		Print( LV5 , "_OnCliPacket" , "�ʥ]cmd=%d�L�j " , NetID , PG->Command );
		return false;
	}
	//�I�sCallback �B�z
	(this->*_CliPGCallBack[PG->Command])( NetID , PG , Size );

	//------------------------------------------------------------------------------
	return true;
}
void	ServerListClass::_OnCliConnectFailed( int NetID )
{

}
//----------------------------------------------------------------------------------------------
ServerListClass::ServerListClass( IniFileClass* Ini )
{
	InitPGCallBack( );
	_CliBListener   = NEW LBClients( this );
	_SrvBListener   = NEW LBServers( this );
	_CliBNet	    = CreateNetBridge( );
	_SrvBNet	    = CreateNetBridge( );

	_ForwardNet		= CreateNetBridge( );
	_ForwardListener = NEW LProxyClient(this);

	_ServerListMode = 0;

	int		AccountType			= Ini->Int( "AccountType" );

	_Account = NEW AccountManage( this , Ini );

	_CountryFilterType = Ini->Int( "CountryFilterType" );
	_CliTempateFile = Ini->Str( "CliTempateFile" );
	ReadClientTemplateFile();

	_ApnicFileReader.LoadFile( Ini->Str( "APNIC_File" ).c_str() );
	
	_LoadIpInfo();

	_MaxPasswordErrorCount = Ini->Int( "MaxPasswordErrorCount" );
	_FreezeAccountTime	= Ini->IntDef( "FreezeAccountTime", 3600);
}
//----------------------------------------------------------------------------------------------
ServerListClass::~ServerListClass( )
{
	DeleteNetBridge( _CliBNet );
	DeleteNetBridge( _SrvBNet );
	DeleteNetBridge( _ForwardNet );
	delete _ForwardListener;
	delete _CliBListener;
	delete _SrvBListener;
	delete _Account;

}
//----------------------------------------------------------------------------------------------
//Ū��ServerList Client�� �]�w���
void	ServerListClass::ReadClientTemplateFile()
{
//	string CliTempateFile = Ini->Str( "CliTempateFile" );

	_CliTempateFileSize = 0;

	//Ū���˪O��
	MyFile myFile;
	myFile.Open( _CliTempateFile.c_str() );
	_CliTempateFileSize =  myFile.Read( _CliTempateFileData , 0x10000 );
	myFile.Close();

}
//----------------------------------------------------------------------------------------------
void	ServerListClass::Host( char* IP , char* IP_Outside , int CliPort , int SrvPort )
{
	int IPNum = ConvertIP( IP );
	_SrvBNet->Host( IPNum , SrvPort , _SrvBListener , true );
	IPNum = ConvertIP( IP_Outside );
	_CliBNet->Host( IPNum , CliPort , _CliBListener , true );
}
//----------------------------------------------------------------------------------------------
//�w�ɳB�z
void	ServerListClass::Process( )
{
	CNetBridge::WaitAll( 10 );
	Sleep(1);
	_Account->Process();
}
//----------------------------------------------------------------------------------------------
// switch �ݨӪ��ʥ]
//----------------------------------------------------------------------------------------------
void ServerListClass::_ServerListNet_SCtoS_RegServerInfo	(  int NetID , ServerListNet_SysNull* _PG , int Size )
{	
	ServerListNet_SCtoS_RegServerInfo* PG = (ServerListNet_SCtoS_RegServerInfo*)_PG;

	Print( LV1 , "_ServerListNet_SCtoS_RegServerInfo" , "ServerName=%s IP=%s  Port=%d" , PG->ServerName , PG->IP_DNS , PG->Port );

	ServerInfoStruct& SrvInfo = _SrvInfoList[ NetID ];
	SrvInfo.ServerName		= PG->ServerName;
	SrvInfo.ServerIP		= PG->IP_DNS;					//Switch IP
	SrvInfo.CliPort			= PG->Port;					//Switch �� Client �s�� Port
	SrvInfo.MaxPlayerCount	= PG->MaxPlayerCount;
	SrvInfo.UpdateTime		= TimeStr::Now_Value();
	SrvInfo.GameID			= PG->GameID;
	SrvInfo.ServerStatus	= PG->ServerStatus;
	SrvInfo.TotalPlayerRate	= PG->TotalPlayerRate;

	SrvInfo.PlayerCount.clear();
	for( int i = 0 ; i < PG->ParallelZoneCount ; i++ )
	{
		SrvInfo.PlayerCount.push_back( PG->PlayerCount[i] );
	}

	SrvInfo.State = EM_ConnectState_Ready;
}
//----------------------------------------------------------------------------------------------
// Client �ݨӪ��ʥ]
//----------------------------------------------------------------------------------------------
void ServerListClass::_ServerListNet_CtoS_ServerListRequest	(  int NetID , ServerListNet_SysNull* _PG , int Size )
{	
	ServerListNet_CtoS_ServerListRequest* PG = ( ServerListNet_CtoS_ServerListRequest* )_PG;
	ClientInfoStruct& CliInfo = _CliInfoList[ NetID ];

	Print( LV2 , "_ServerListNet_CtoS_ServerListRequest" , "Account = %s" , PG->Account );
	
	// Verify account and password is properly terminated
	if( strnlen( PG->Account, _DEF_MAX_ACCOUNTNAME_SIZE_ ) == _DEF_MAX_ACCOUNTNAME_SIZE_ || strnlen( PG->Password, _DEF_MAX_PASSWORD_SIZE_ ) == _DEF_MAX_PASSWORD_SIZE_ )
	{
		return;
	}

	//�p�G�S����J�b�� 
	if( strnlen( PG->Account, _DEF_MAX_ACCOUNTNAME_SIZE_ ) == 0 || CheckSqlStr( PG->Account ) == false )
	{
		ServerListNet_StoC_ServerListFailed SendFailed;
		SendFailed.Result = EM_ServerListFailed_Password_Error;
		_CliBNet->Send( NetID , sizeof(SendFailed) ,& SendFailed );
		return;
	}

	CliInfo.Account		= PG->Account;
	CliInfo.Password	= PG->Password;
	CliInfo.State		= EM_ConnectState_CheckAccount;
	DWORD IPNum;
	WORD Port;
	char  IPStr[256];
	_CliBNet->GetConnectInfo( NetID , &IPNum , &Port );
	ConvertIPtoString( IPNum , IPStr );
	if( PG->KickAccount == true )
		_Account->CheckAccount( NetID , PG->Account , PG->Password , IPStr, PG->HashedPassword );
	else
	{
		CheckAccountResult( NetID , -1 , PG->Account , EM_AccountResult_ReloadServerList );
	}
}
//----------------------------------------------------------------------------------------------
struct TempResetPasswordStruct
{
	ServerListClass*	This;
	int					NetID;
	int					LastLoginGameID;

	string				Account;
	string				OldPassword;
	string				NewPassword;
	int					Result;
};
//----------------------------------------------------------------------------------------------
bool ServerListClass::_SQLCmdResetPasswordEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	// �q Database ���X�K�X, �M��̷� Serverlist check ���覡, ���ˬd�K�X
	TempResetPasswordStruct* TempData = (TempResetPasswordStruct*)UserObj;

	char					SqlCmd[2048];
	MyDbSqlExecClass		MyDBProc( sqlBase );
	
	bool					bCheckResult		 = false;
	
	char					szPassword[256];
	bool					IsMD5;
	int						LastLoginGameID;


	sprintf( SqlCmd , "SELECT Password,IsMd5Password,LastLoginGameID FROM PlayerAccount WHERE Account_ID = '%s'" , TempData->Account.c_str() );

	MyDBProc.SqlCmd( SqlCmd );

	MyDBProc.Bind( 1,	SQL_C_CHAR,		sizeof( szPassword ),	(LPBYTE)&szPassword );
	MyDBProc.Bind( 2,	SQL_C_BIT,		SQL_C_DEFAULT,			(LPBYTE)&IsMD5		);
	MyDBProc.Bind( 2,	SQL_C_LONG,		sizeof( LastLoginGameID ),	(LPBYTE)&LastLoginGameID );

	if( MyDBProc.Read() )
	{
		MyMD5Class myMd5Class;
		myMd5Class.ComputeStringHash( TempData->OldPassword.c_str() , NULL );

		TempData->LastLoginGameID	= LastLoginGameID;

		if( IsMD5 == true )
		{
			if( strcmp( myMd5Class.GetMd5Str() , szPassword ) == 0 )
			{
				TempData->Result = 0;
			}
			else
			{
				TempData->Result = (int)EM_ServerListFailed_Password_Error;

			}
		}
		else
		{
			if( strcmp( TempData->OldPassword.c_str(), szPassword ) == 0 )
			{
				TempData->Result = 0;
			}
			else
			{
				TempData->Result = (int)EM_ServerListFailed_Password_Error;
			}
		}
	}
	else
	{
		TempData->LastLoginGameID	= 0;
		TempData->Result			= (int)EM_ServerListFailed_Password_Error;
	}

	if( TempData->Result == 0 )
	{
		// �ϥηs�K�X���N SQL Server �W���K�X
		sprintf( SqlCmd , "UPDATE PlayerAccount SET Password=CAST( %s AS varchar(256)), IsMd5Password = 0, ResetPassword = 0 WHERE Account_ID = '%s'" ,StringToSqlX(TempData->NewPassword.c_str(), 0 , true ).c_str()  , TempData->Account.c_str() );		
		MyDBProc.SqlCmd_WriteOneLine( SqlCmd );
	}
	
	MyDBProc.Close();

	return true;
}
void ServerListClass::_SQLCmdResetPasswordEventResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempResetPasswordStruct*	TempData = (TempResetPasswordStruct*)UserObj;

	ServerListNet_StoC_ResetPasswordResult Send;

	Send.Result	= TempData->Result;

	TempData->This->_CliBNet->Send( TempData->NetID , sizeof( Send ), &Send );

	if( Send.Result == 0 )
	{
		// �o�e okay �T�� �� Client
		TempData->This->CheckAccountResult( TempData->NetID , TempData->LastLoginGameID , TempData->Account.c_str(), EM_AccountResult_Success  );
	}

	delete TempData;
}

//----------------------------------------------------------------------------------------------
void ServerListClass::_ServerListNet_CtoS_ResetPasswordResult	(  int NetID , ServerListNet_SysNull* _PG , int Size )
{	
	ServerListNet_CtoS_ResetPasswordResult* PG		= ( ServerListNet_CtoS_ResetPasswordResult* )_PG;
	ClientInfoStruct&						CliInfo = _CliInfoList[ NetID ];

	// Verify account and password is properly terminated
	if( strnlen( PG->Account, _DEF_MAX_ACCOUNTNAME_SIZE_ ) == _DEF_MAX_ACCOUNTNAME_SIZE_ || strnlen( PG->OldPassword, _DEF_MAX_PASSWORD_SIZE_ ) == _DEF_MAX_PASSWORD_SIZE_ || strnlen( PG->NewPassword, _DEF_MAX_PASSWORD_SIZE_ ) == _DEF_MAX_PASSWORD_SIZE_ )
	{
		return;
	}

	//�p�G�S����J�b�� 
	if( strnlen( PG->Account, _DEF_MAX_ACCOUNTNAME_SIZE_ ) == 0 || CheckSqlStr( PG->Account ) == false )
	{
		return;
	}

	TempResetPasswordStruct* pData = new TempResetPasswordStruct;

	pData->Account		= PG->Account;
	pData->OldPassword	= PG->OldPassword;
	pData->NewPassword	= PG->NewPassword;
	
	pData->This			= this;
	pData->NetID		= NetID;

	_Account->NormalDB()->SqlCmd( _Account->_AccountValue( PG->Account ) ,  _SQLCmdResetPasswordEvent , _SQLCmdResetPasswordEventResult , pData , NULL );
}
//----------------------------------------------------------------------------------------------
void ServerListClass::_ServerListNet_CtoS_ClientSystemInfo	(  int NetID , ServerListNet_SysNull* _PG , int Size )
{
	ServerListNet_CtoS_ClientSystemInfo* PG = ( ServerListNet_CtoS_ClientSystemInfo* )_PG;

	char	Buf[512];
	sprintf_s( Buf , sizeof(Buf) , "INSERT INTO ClientSystemInfo(Account,MacAddress,DisplayCardVendorID,DisplayCardDeviceID,OSID) VALUES(CAST( %s AS varchar(64)), CAST( %s AS varchar(256)),%d,%d, CAST( %s AS varchar(256)))",StringToSqlX( PG->Account , 0 , true ).c_str() , StringToSqlX( PG->MacAddress , 0 , true ).c_str(), PG->DisplayCardVendorID, PG->DisplayCardDeviceID, StringToSqlX( PG->OSINFO , 0 , true ).c_str() );
	_Account->NormalDB()->SQLCMDWrite( rand() , NULL , NULL , Buf );
}
//----------------------------------------------------------------------------------------------
void ServerListClass::_ServerListNet_CtoS_SetVivoxLicenseTime			(  int NetID , ServerListNet_SysNull* _PG , int Size )
{
	ServerListNet_CtoS_SetVivoxLicenseTime* PG = ( ServerListNet_CtoS_SetVivoxLicenseTime* )_PG;
	ClientInfoStruct& CliInfo = _CliInfoList[ NetID ];

	char	Buf[512];
	sprintf_s( Buf , sizeof(Buf) , "UPDATE PlayerAccount SET vivoxLicenseTime = getdate() where Account_ID = '%s' and vivoxLicenseTime = 0", CliInfo.Account.c_str() );
	_Account->NormalDB()->SQLCMDWrite( rand() , NULL , NULL , Buf );
}
void ServerListClass::_ServerListNet_CtoS_ForwardTo			(  int NetID , ServerListNet_SysNull* _PG , int Size )
{
	ServerListNet_CtoS_ForwardTo* PG = ( ServerListNet_CtoS_ForwardTo* )_PG;
	ClientInfoStruct& CliInfo = _CliInfoList[ NetID ];
	
	CliInfo.State = EM_ConnectState_Proxy_ConnectTo;

	int IPnum = ConvertIP( PG->IPStr );
	_ForwardNet->Telnet( IPnum , PG->Port , _ForwardListener , (PVOID)NetID );

}
//----------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//IAccountPluginNotify
void ServerListClass::CheckAccountResult( int NetID , int WorldID , string Account , AccountResultENUM Result )
{
	ServerListNet_StoC_ServerListResult	SendOK;
	ServerListNet_StoC_ServerListFailed SendFailed;
	ServerListNet_StoC_ServerListFailedData SendFailedData;
	bool bSendFailedData = false;

	strcpy( SendOK.Account , Account.c_str() );

	ClientInfoStruct& CliInfo = _CliInfoList[ NetID ];

	if(		CliInfo.State != EM_ConnectState_CheckAccount 
		&&	Result != EM_AccountResult_ReloadServerList )
	{
		Print( LV5 , "_DBReadAccountDBEventCB" , "CliInfo.State != EM_ConnectState_CheckAccount" );
		return;
	}

	CliInfo.State	= EM_ConnectState_CheckAccountFailed;

	SendFailed.Result = EM_ServerListFailed_Unknow;

	switch( Result )
	{
	case EM_AccountResult_Success: 
		//////////////////////////////////////////////////////////////////////////
		//Ū���b������H��
		//////////////////////////////////////////////////////////////////////////
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_Success Account=%s" , Account.c_str()  );
		//////////////////////////////////////////////////////////////////////////
		break;
	case EM_AccountResult_Relogin:	
		{
			Print( LV2 , "CheckAccountResult" , "EM_AccountResult_Relogin Account=%s" , Account.c_str()  );
			ServerListNet_StoSC_CliReloginNotify SendRelogin;
			strncpy( SendRelogin.Account , CliInfo.Account.c_str() , sizeof(SendRelogin.Account) );

			vector< ServerInfoStruct >::iterator Iter;
			for( Iter = _SrvInfoList.begin() ; Iter != _SrvInfoList.end() ; Iter++ )
			{
				if( Iter->State != EM_ConnectState_Ready || ( Iter->GameID & 0xffff ) != WorldID )
					continue;
				_SrvBNet->Send( Iter->NetID , sizeof(SendRelogin) , &SendRelogin );
			}
		}
		break;
	case EM_AccountResult_PasswordErr:
		{
			//���]�w�K�X���\���~����
			if (_MaxPasswordErrorCount > 0)
			{
				if (CliInfo.PasswordErrorCount > 0)
				{
					SendFailedData.Result							= EM_ServerListFailed_Password_Error;
					SendFailedData.Data.PasswordError.Count			= CliInfo.PasswordErrorCount;
					SendFailedData.Data.PasswordError.Max			= _MaxPasswordErrorCount;
					SendFailedData.Data.PasswordError.FreezeTime	= _FreezeAccountTime;
					bSendFailedData = true;
				}				
			}

			Print( LV2 , "CheckAccountResult" , "Other Error (PasswordError ...) Account=%s" , Account.c_str()  );
			SendFailed.Result = EM_ServerListFailed_Password_Error;
		}
		break;
	case EM_AccountResult_IPBlock:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_IPBlock Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_IPBlock;
		break;
	case EM_AccountResult_CommLock:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_CommLock Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_CommLock;
		break;
	case EM_AccountResult_Freeze_Self:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_Freeze_Self Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_Freeze_Self;
		break;
	case EM_AccountResult_Freeze_Official:
		Print( LV2 , "CheckAccountResult" , "EM_ServerListFailed_Freeze_Official Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_Freeze_Official;
		break;
	case EM_AccountResult_Freeze_Other3:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_Freeze_Other3 Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_Freeze_Other3;
		break;
	case EM_AccountResult_Freeze_Other4:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_Freeze_Other4 Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_Freeze_Other4;
		break;
	case EM_AccountResult_Freeze_Other5:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_Freeze_Other5 Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_Freeze_Other5;
		break;
	case EM_AccountResult_Freeze_Other6:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_Freeze_Other6 Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_Freeze_Other6;
		break;
	case EM_AccountResult_Freeze_Other7:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_Freeze_Other7 Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_Freeze_Other7;
		break;
	case EM_AccountResult_Freeze_Other8:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_Freeze_Other8 Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_Freeze_Other8;
		break;
	case EM_AccountResult_Freeze_Other9:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_Freeze_Other9 Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_Freeze_Other9;
		break;
//	case EM_AccountResult_Freeze_Other:
//		Print( LV2 , "CheckAccountResult" , "EM_ServerListFailed_Freeze_Other Account=%s" , Account.c_str()  );
//		SendFailed.Result = EM_ServerListFailed_Freeze_Other;
//		break;
	case EM_AccountResult_BetaKeyRequest:
		Print( LV2 , "CheckAccountResult" , "EM_ServerListFailed_BetaKeyRequest Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_BetaKeyRequest;
		break;
	case EM_AccountResult_Gama_Wrong_OTP:	//��l���~ Wrong_OTP : One time password is not correct
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_Gama_Wrong_OTP Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_Gama_Wrong_OTP;
		break;
	case EM_AccountResult_Gama_Wrong_EventOTP:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_Gama_Wrong_EventOTP Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_Gama_Wrong_EventOTP;
		break;

	case EM_AccountResult_Gama_Wrong_Timeout:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_Gama_Wrong_Timeout Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_Gama_Wrong_Timeout;
		break;
	case EM_AccountResult_Gama_AccountNotFind:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_Gama_AccountNotFind Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_Gama_NoAccount;
		break;
	case EM_AccountResult_VN_Wrong_UserState:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_VN_Wrong_UserState Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_VN_Wrong_UserState;
		break;
	case  EM_AccountResult_ReloadServerList:
		break;
	case EM_AccountResult_ValidTimeErr:
		SendFailed.Result = EM_ServerListFailed_ValidTimeError;
		break;
	case EM_AccountResult_KR_AgeRestriction:
		Print( LV2 , "CheckAccountResult" , "EM_AccountResult_KR_AgeRestriction Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_KR_AgeRestriction;
		break;
	default:
		Print( LV2 , "CheckAccountResult" , "Other Error (PasswordError ...) Account=%s" , Account.c_str()  );
		SendFailed.Result = EM_ServerListFailed_Password_Error;
		break;
	}

	if (SendFailed.Result >= EM_AccountResult_Freeze_Self && SendFailed.Result <= EM_AccountResult_Freeze_Other9)
	{
		if (CliInfo.FreezeRemaingTime > 0)
		{
			SendFailedData.Result					 = EM_ServerListFailed_Freeze_Official;
			SendFailedData.Data.Freeze.RemainingTime = CliInfo.FreezeRemaingTime;
			bSendFailedData = true;
		}
	}

	if( SendFailed.Result != EM_ServerListFailed_Unknow	)
	{
		if (bSendFailedData)
		{
			_CliBNet->Send( NetID , sizeof(SendFailedData) ,& SendFailedData );
		}

		_CliBNet->Send( NetID , sizeof(SendFailed) ,& SendFailed );
		return;
	}

	if( _SrvInfoList.size() == 0 )
	{
		SendFailed.Result = EM_ServerListFailed_NoWorld;
		_CliBNet->Send( NetID , sizeof(SendFailed) ,& SendFailed );
		return;
	}

	CliInfo.State		= EM_ConnectState_Ready;
	
	SendOK.TotalCount = 0;
	for( unsigned i = 0 ; i < _SrvInfoList.size() ; i++ )
	{
		if( _SrvInfoList[i].State != EM_ConnectState_Ready )
			continue;

		ServerInfoStruct& SrvInfo = _SrvInfoList[i];

		if( TimeStr::Now_Value() - SrvInfo.UpdateTime  >= 300 )
			continue;

		SendOK.TotalCount++;
	}

	WORD Port;
	DWORD IPNum;
	_CliBNet->GetConnectInfo( NetID , &IPNum , &Port );
	SendOK.IPNum = IPNum;
	ApnicCountryInfoStruct& ApnicInfo = _ApnicFileReader.FindIP( SendOK.IPNum );

	if( ApnicInfo.IsAllowLogin == false )
	{

		char szIP[256];
		ConvertIPtoString( SendOK.IPNum, szIP );

		Print( LV2 , "IPBlock" , "Block IP=%d,%s, Account=%s, Country=%s", SendOK.IPNum, szIP, Account.c_str(), ApnicInfo.Country.c_str() );

		SendFailed.Result = EM_ServerListFailed_IPBlock;
		_CliBNet->Send( NetID , sizeof(SendFailed) ,& SendFailed );
		return;
	}

	strncpy( SendOK.Country , ApnicInfo.Country.c_str() , sizeof(SendOK.Country) );
	SendOK.Country[2] = 0;
	SendOK.LastLoginWorldID = WorldID;
	SendOK.ServerlistMode = _ServerListMode;
	SendOK.Age = CliInfo.Age;
	SendOK.ValidTime = CliInfo.ValidTime;
	SendOK.VivoxLicenseTime = CliInfo.VivoxLicenseTime;

	for( unsigned i = 0 ; i < _SrvInfoList.size() ; i++ )
	{
		if( _SrvInfoList[i].State != EM_ConnectState_Ready )
			continue;

		ServerInfoStruct& SrvInfo = _SrvInfoList[i];

		SendOK.Index = i;
		//strncpy(SendOK.IP_DNS, SrvInfo.ServerIP.c_str(), sizeof(SendOK.IP_DNS));
		strncpy( SendOK.IP_DNS , "24.21.184.103" , sizeof(SendOK.IP_DNS));
		strncpy( SendOK.ServerName , SrvInfo.ServerName.c_str() , sizeof(SendOK.ServerName) );
		SendOK.ParallelZoneCount = int( SrvInfo.PlayerCount.size() );
		SendOK.Port	= SrvInfo.CliPort;
		SendOK.UpdateTime = TimeStr::Now_Value() - SrvInfo.UpdateTime;
		SendOK.WorldID = SrvInfo.GameID;
		SendOK.TotalPlayerRate = SrvInfo.TotalPlayerRate;

		if( SendOK.UpdateTime >= 300 )
			continue;

		for( unsigned j = 0 ; j < SrvInfo.PlayerCount.size() ; j++ )
		{
			SendOK.PlayerRate[ j ] = float( SrvInfo.PlayerCount[j] )/SrvInfo.MaxPlayerCount;
		}

		SendOK.ServerStatus = SrvInfo.ServerStatus;
		_CliBNet->Send( NetID , sizeof(SendOK) ,& SendOK );	
	}

	Print(LV2, "SendWorldRoleCount", "Account = %s", Account.c_str());
	SendWorldRoleCount( NetID , Account.c_str() );
}

//-----------------------------------------------------------------------------------
struct TempWorldRoleCountStruct
{
	ServerListClass* This;
	string Account_ID;
	int	CliNetID;
	int RoleCount[ 30 ];
};
//�q���UServer �H��
void	ServerListClass::SendWorldRoleCount( int CliNetID , const char* Account_ID  )
{
	TempWorldRoleCountStruct* TempData = NEW TempWorldRoleCountStruct;
	TempData->This = this;
	TempData->CliNetID = CliNetID;
	TempData->Account_ID = Account_ID;
	memset( TempData->RoleCount , 0 , sizeof(TempData->RoleCount) );
	_Account->NormalDB()->SqlCmd( 0 ,  _SQLCmdRoleCountEvent , _SQLCmdRoleCountEventResult , TempData , NULL );
}

//Ū����Server�H��
bool ServerListClass::_SQLCmdRoleCountEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempWorldRoleCountStruct* TempData = (TempWorldRoleCountStruct*)UserObj;
	
	char	SqlCmd[2048];
	MyDbSqlExecClass		MyDBProc( sqlBase );
	sprintf( SqlCmd , "SELECT RoleCount1, RoleCount2, RoleCount3, RoleCount4, RoleCount5, RoleCount6, RoleCount7, RoleCount8, RoleCount9, RoleCount10, RoleCount11, RoleCount12, RoleCount13, RoleCount14, RoleCount15, RoleCount16, RoleCount17, RoleCount18, RoleCount19, RoleCount20, RoleCount21, RoleCount22, RoleCount23, RoleCount24, RoleCount25, RoleCount26, RoleCount27, RoleCount28, RoleCount29, RoleCount30 FROM Game_RoleCount WHERE Account_ID = '%s'" , TempData->Account_ID.c_str() );
	MyDBProc.SqlCmd( SqlCmd );
	for( int i = 0 ; i < 30 ; i++ )
		MyDBProc.Bind( i+1 , SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&TempData->RoleCount[i] );
	MyDBProc.Read();
	MyDBProc.Close();

	return true;
}
void ServerListClass::_SQLCmdRoleCountEventResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempWorldRoleCountStruct* TempData = (TempWorldRoleCountStruct*)UserObj;

	ServerListNet_StoC_GameRoleCount Send;
	for( int i = 0 ; i < 30 ; i++ )
	{
		Send.RoleCount[i] = (char)TempData->RoleCount[i];
	}
	Send.DataSize = TempData->This->_CliTempateFileSize;
	//Send.FileData[0] = 0;
	memcpy( Send.FileData , TempData->This->_CliTempateFileData , Send.DataSize );
	TempData->This->_CliBNet->Send( TempData->CliNetID , Send.Size() ,& Send );

	delete TempData;
}

void ServerListClass::AccountLoginResult( string Account , AccountResultENUM Result )
{

}
void ServerListClass::AccountLogoutResult( string Account , AccountResultENUM Result )
{
		
}
void ServerListClass::SetData( int NetID , int Type , int Value )
{
	ClientInfoStruct& CliInfo = _CliInfoList[ NetID ];
	switch( Type )
	{
	case EM_SetData_Age:	// Age
		CliInfo.Age = Value;
		break;
	case EM_SetData_ValidTime:	//���Įɶ�
		CliInfo.ValidTime = Value;
		break;
	case EM_SetData_VivoxLicenseTime:
		CliInfo.VivoxLicenseTime = Value;
		break;
	case EM_SetData_PasswordErrorCount:
		CliInfo.PasswordErrorCount = Value;
		break;
	case EM_SetData_FreezeRemainingTime:		
		CliInfo.FreezeRemaingTime = Value;
		break;
	default:
		break;
	}
}

void	ServerListClass::_OnProxyConnected( DWORD ForwardNetID )
{
	int CliNetID = (int)_ForwardNet->GetContext( ForwardNetID );
	ClientInfoStruct& CliInfo = _CliInfoList[ CliNetID ];
	
	if( CliInfo.State == EM_ConnectState_Proxy_ConnectTo_CliDisconnect )
	{
		_ForwardNet->Close( ForwardNetID );
		return;
	}

	if(CliInfo.State != EM_ConnectState_Proxy_ConnectTo )
	{
		Print( LV5 , "_OnProxyConnected" , "CliInfo.State != EM_ConnectState_Proxy_ConnectTo" );
		return;
	}
	CliInfo.ForwardNetID = ForwardNetID;
	CliInfo.State = EM_ConnectState_Proxy_OnConnect;
	
	if( CliInfo.PacketData.size() != 0 )
	{
		_ForwardNet->Send( ForwardNetID , (DWORD)CliInfo.PacketData.size() , &(CliInfo.PacketData[0]) );
	}
}
void	ServerListClass::_OnProxyDisconnected( DWORD ForwardNetID )
{
	int CliNetID = (int)_ForwardNet->GetContext( ForwardNetID );
	ClientInfoStruct& CliInfo = _CliInfoList[ CliNetID ];
	if(CliInfo.State != EM_ConnectState_Proxy_OnConnect )
	{
		//Print( LV5 , "_OnProxyDisconnected" , "CliInfo.State != EM_ConnectState_Proxy_OnConnect" );
		return;
	}
	_CliBNet->Close( CliNetID );
}
bool	ServerListClass::_OnProxyPacket( DWORD ForwardNetID , DWORD Size , void* Data )
{
	int CliNetID = (int)_ForwardNet->GetContext( ForwardNetID );
	ClientInfoStruct& CliInfo = _CliInfoList[ CliNetID ];
	if(CliInfo.State != EM_ConnectState_Proxy_OnConnect )
	{
		Print( LV5 , "_OnProxyPacket" , "CliInfo.State != EM_ConnectState_Proxy_OnConnect" );
		return true;
	}
	
	_CliBNet->Send( CliNetID , Size , Data );
	return true;
}
void	ServerListClass::_OnProxyConnectFailed( int ForwardNetID )
{
	int CliNetID = (int)_ForwardNet->GetContext( ForwardNetID );
	ClientInfoStruct& CliInfo = _CliInfoList[ CliNetID ];

	if(CliInfo.State != EM_ConnectState_Proxy_ConnectTo )
	{
		Print( LV5 , "_OnProxyConnectFailed" , "CliInfo.State != EM_ConnectState_Proxy_ConnectTo" );
		return;
	}	
	_CliBNet->Close( CliNetID );
}

//Ū��ip���� �P ���i�H�s�u����a
struct IpBaseInfoTempStruct
{
	char		IP_Begin[16];
	char		IP_End[16];
	char		Country[32];
};

struct IpInfoTempStruct
{
	ServerListClass*				This;
	vector< IpBaseInfoTempStruct >	IpList;
	vector< string >				CountryFilterList;
};
void	ServerListClass::_LoadIpInfo( )
{
	IpInfoTempStruct* TempData = NEW IpInfoTempStruct;
	TempData->This = this;
	_Account->NormalDB()->SqlCmd( 0 ,  _SQLCmdReadIpInfoEvent , _SQLCmdReadIpInfoEventResult , TempData , NULL );
	/*
	TempWorldRoleCountStruct* TempData = NEW TempWorldRoleCountStruct;
	TempData->This = this;
	TempData->CliNetID = CliNetID;
	TempData->Account_ID = Account_ID;
	memset( TempData->RoleCount , 0 , sizeof(TempData->RoleCount) );
	_Account->NormalDB()->SqlCmd( 0 ,  _SQLCmdRoleCountEvent , _SQLCmdRoleCountEventResult , TempData , NULL );
	*/
}

bool ServerListClass::_SQLCmdReadIpInfoEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	IpInfoTempStruct* TempData = (IpInfoTempStruct*)UserObj;
	MyDbSqlExecClass  MyDBProc( sqlBase );

	IpBaseInfoTempStruct TempIpInfo;
	MyDBProc.SqlCmd( "SELECT IP_Begin,IP_End,Country FROM Country_IP" );
	MyDBProc.Bind( 1, SQL_C_CHAR, sizeof(TempIpInfo.IP_Begin)	, (LPBYTE)&TempIpInfo.IP_Begin );
	MyDBProc.Bind( 2, SQL_C_CHAR, sizeof(TempIpInfo.IP_End)		, (LPBYTE)&TempIpInfo.IP_End );
	MyDBProc.Bind( 3, SQL_C_CHAR, sizeof(TempIpInfo.Country)	, (LPBYTE)&TempIpInfo.Country );

	//Ū���U�Ӱ�a��IP�Ϭq
	while( MyDBProc.Read() )
	{
		TempData->IpList.push_back( TempIpInfo );
	}

	MyDBProc.Close();

	char	CountryFilter[32];
	//Ū���L�o����a
	MyDBProc.SqlCmd( "SELECT Courty FROM Country_Filter" );
	MyDBProc.Bind( 1, SQL_C_CHAR, sizeof(CountryFilter)	, (LPBYTE)&CountryFilter );
	while( MyDBProc.Read() )
	{
		TempData->CountryFilterList.push_back( CountryFilter );
	}
	MyDBProc.Close();

	return true;	
}
void ServerListClass::_SQLCmdReadIpInfoEventResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	IpInfoTempStruct* TempData = (IpInfoTempStruct*)UserObj;
	ServerListClass* This = TempData->This;
	
	for( unsigned i = 0 ; i < TempData->IpList.size() ; i++ )
	{
		This->_ApnicFileReader.AddIp( TempData->IpList[i].IP_Begin , TempData->IpList[i].IP_End , TempData->IpList[i].Country	);
	}
	
	//0 �����]�������\�i�J 1 �����]�w���\�i�J
	This->_ApnicFileReader.SetAllCountryFilter( This->_CountryFilterType != 0 );

	for( unsigned i = 0 ; i < TempData->CountryFilterList.size() ; i++ )
	{
		This->_ApnicFileReader.SetCountryFilter( TempData->CountryFilterList[i].c_str() , This->_CountryFilterType == 0 );
	}

	delete TempData;
}

void ServerListClass::ResetPassword	( int NetID )
{
	ServerListNet_StoC_ResetPassword	Packet;
	_CliBNet->Send( NetID , sizeof(Packet) ,& Packet );
}

