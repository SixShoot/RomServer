#include "SwitchClass.h"

#include <time.h>
#include <Psapi.h>
/*
//---------------------------------------------------------------------------------
void LSAPI LProxy::OnConnected( int NetID )
{
    printf("\n(Proxy:%d)_OnProxyConnected" , NetID );
    _Parent->_OnProxyConnected(NetID);
}
//---------------------------------------------------------------------------------
void LSAPI LProxy::OnDisconnected( int NetID )
{
    printf("\n(Proxy:%d)_OnProxyDisconnected" , NetID );
    _Parent->_OnProxyDisconnected(NetID);
}
//---------------------------------------------------------------------------------
void LSAPI LProxy::OnPeerShutdown( int NetID )
{
    printf("\n(Proxy:%d)_OnProxyDisconnected" , NetID );
    _Parent->_OnProxyDisconnected(NetID);
}
//---------------------------------------------------------------------------------
bool LSAPI LProxy::OnPacket( int NetID, HBUFFER hIncomingData )
{
    _Parent->_OnProxyPacket(NetID,hIncomingData);
    return true;
}
//---------------------------------------------------------------------------------
void LSAPI LProxy::OnConnectFailed( int NetID )
{
    printf("\n(Proxy:%d)_OnProxyConnectFailed" , NetID );
    _Parent->_OnProxyConnectFailed(NetID);
}
*/

int			SwitchClass::m_WorldID	= 0;
bool		SwitchClass::m_NeedPassword = true;

bool		SwitchClass::m_bCheck_Client_SendBuffLimit	= true;
bool		SwitchClass::m_bCheck_Client_NoRespond		= true;

bool		SwitchClass::m_bCheck_Server_SendBuffLimit	= true;
bool		SwitchClass::m_bCheck_Server_NoRespond		= true;


bool        LBProxy::OnRecv			( DWORD NetID, DWORD PackSize, PVOID PacketData )
{
    _Parent->_OnProxyPacket(NetID , PackSize , PacketData );
    return true;
}
void		LBProxy::OnConnectFailed( DWORD dwNetID, DWORD dwFailedCode )
{
    Print( LV2 , "OnConnectFailed" , "(Proxy)_OnProxyConnectFailed Error=%d" , dwFailedCode );
    _Parent->_OnProxyConnectFailed(dwFailedCode);
}
void		LBProxy::OnConnect		( DWORD NetID )
{
    Print( LV2 , "OnConnect" , "(Proxy:%d)_OnProxyConnected" , NetID );
    _Parent->_OnProxyConnected(NetID);
}
void		LBProxy::OnDisconnect	( DWORD NetID )
{
    Print( LV2 , "OnDisconnect" , "(Proxy:%d)_OnProxyDisconnected" , NetID );
    _Parent->_OnProxyDisconnected(NetID);
}
CEventObj*	LBProxy::OnAccept		( DWORD NetID )
{
    Print( LV2 , "OnAccept" , "(Proxy:%d)OnProxyAccept" , NetID );
    //_Parent->_OnProxyConnected(NetID);
    return this;
}
//---------------------------------------------------------------------------------
/*
void LSAPI LGSrv::OnConnected( int NetID )
{
    printf("\n(LGSrv:%d)_OnGSrvConnected" , NetID );
    _Parent->_OnGSrvConnected(NetID);
}
//---------------------------------------------------------------------------------
void LSAPI LGSrv::OnDisconnected( int NetID )
{
    printf("\n(LGSrv:%d)_OnGSrvDisconnected" , NetID );
    _Parent->_OnGSrvDisconnected(NetID);
}
//---------------------------------------------------------------------------------
void LSAPI LGSrv::OnPeerShutdown( int NetID )
{
    printf("\n(LGSrv:%d)_OnGSrvDisconnected" , NetID );
    _Parent->_OnGSrvDisconnected(NetID);
}
//---------------------------------------------------------------------------------
bool LSAPI LGSrv::OnPacket( int NetID, HBUFFER hIncomingData )
{
    _Parent->_OnGSrvPacket(NetID,hIncomingData);
    return true;
}
//---------------------------------------------------------------------------------
void LSAPI LGSrv::OnConnectFailed( int NetID )
{
    printf("\n(LGSrv:%d)_OnGSrvConnectFailed" , NetID );
    _Parent->_OnGSrvConnectFailed(NetID);
}
*/

bool		LBGSrv::OnRecv			( DWORD NetID, DWORD PackSize, PVOID PacketData )
{
    _Parent->_OnGSrvPacket( NetID , PackSize , PacketData );
    return true;
}
void		LBGSrv::OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode )
{
    Print( LV2 , "OnConnectFailed" , "(LGSrv:%d)_OnGSrvConnectFailed" , dwFailedCode );
    _Parent->_OnGSrvConnectFailed(dwFailedCode);
}
void		LBGSrv::OnConnect		( DWORD NetID )
{
    Print( LV2 , "OnConnect" , "(LGSrv:%d)_OnGSrvConnected" , NetID );
    _Parent->_OnGSrvConnected(NetID);
}
void		LBGSrv::OnDisconnect	    ( DWORD NetID )
{
    Print( LV2 , "OnDisconnect" ,"(LGSrv:%d)_OnGSrvDisconnected" , NetID );
    _Parent->_OnGSrvDisconnected(NetID);
}
CEventObj*	LBGSrv::OnAccept		    ( DWORD NetID )
{
    Print( LV2 , "OnAccept" , "(LGSrv:%d)OnAccept" , NetID );
    //_Parent->_OnGSrvConnected(NetID);
    return this;
}
//---------------------------------------------------------------------------------
//處理與 Client 間通訊傳輸的Class
/*
void LSAPI LCli::OnConnected      ( int NetID )
{
    printf("\n(LCli:%d)_OnCliConnected" , NetID );
    _Parent->_OnCliConnected(NetID);
}
void LSAPI LCli::OnDisconnected   ( int NetID )
{
    printf("\n(LCli:%d)_OnCliDisconnected" , NetID );
    _Parent->_OnCliDisconnected(NetID);
}
void LSAPI LCli::OnPeerShutdown   ( int NetID )
{
    printf("\n(LCli:%d)_OnCliDisconnected" , NetID );
    _Parent->_OnCliDisconnected(NetID);
}
bool LSAPI LCli::OnPacket         ( int NetID, HBUFFER hIncomingData )
{
    _Parent->_OnCliPacket(NetID,hIncomingData);
    return true;
}
void LSAPI LCli::OnConnectFailed  ( int NetID )
{
    printf("\n(LCli:%d)_OnCliConnectFailed" , NetID );
    _Parent->_OnCliConnectFailed(NetID);
}
*/
bool		LBCli::OnRecv			( DWORD NetID, DWORD PackSize, PVOID PacketData )
{
    _Parent->_OnCliPacket( NetID, PackSize , PacketData );
    return true;
}
void		LBCli::OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode )
{
    Print( LV2 , "OnConnectFailed" , "(LCli)_OnCliConnected Error=%d" , dwFailedCode );
    _Parent->_OnCliConnectFailed(dwFailedCode);
}
void		LBCli::OnConnect		( DWORD NetID )
{
    Print( LV2 , "OnConnect" , "(LCli:%d)_OnCliConnected" , NetID );
    _Parent->_OnCliConnected(NetID);
}
void		LBCli::OnDisconnect	    ( DWORD NetID )
{
    Print( LV2 , "OnDisconnect" , "(LCli:%d)_OnCliDisconnected" , NetID );
    _Parent->_OnCliDisconnected(NetID);
}
CEventObj*	LBCli::OnAccept		    ( DWORD NetID )
{
//    printf("\n(LCli:%d)_OnCliConnected" , NetID );
    //_Parent->_OnCliConnected(NetID);
    return this;
}
//-----------------------------------------------------------------------------------

bool		LBGateway::OnRecv			( DWORD NetID, DWORD PackSize, PVOID PacketData )
{
	_Parent->_OnGatewayPacket( NetID, PackSize , PacketData );
	return true;
}
void		LBGateway::OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode )
{
	Print( LV2 , "OnConnectFailed" , "(LCli)_OnatewayConnected Error=%d" , dwFailedCode );
	_Parent->_OnGatewayConnectFailed(dwFailedCode);
}
void		LBGateway::OnConnect		( DWORD NetID )
{
	Print( LV2 , "OnConnect" , "(LCli:%d)_OnatewayConnected" , NetID );
	_Parent->_OnGatewayConnected(NetID);
}
void		LBGateway::OnDisconnect	    ( DWORD NetID )
{
	Print( LV2 , "OnDisconnect" , "(LCli:%d)_OnatewayDisconnected" , NetID );
	_Parent->_OnGatewayDisconnected(NetID);
}
CEventObj*	LBGateway::OnAccept		    ( DWORD NetID )
{
	return this;
}

//-----------------------------------------------------------------------------------
void LAC::OnLogin( )
{
    _Parent->_OnACLogin( );
}
//---------------------------------------------------------------------------------
void LAC::OnLoginFailed( GSrvLoginResultEnum Result )
{
    _Parent->_OnACLoginFailed( Result );
}
//---------------------------------------------------------------------------------
void LAC::OnLogout( )
{
    _Parent->_OnACLogout( );
}
//---------------------------------------------------------------------------------
void LAC::OnPlayerReg( int UserID , char* Account , ChargTypeEnum Type , int Point ,  char* LastLoginIP , char* LastLoginTime , int PlayTimeQuota )
{
    _Parent->_OnACPlayerReg(UserID , Account , Type ,Point , LastLoginIP , LastLoginTime , PlayTimeQuota );
}
//---------------------------------------------------------------------------------
void LAC::OnPlayerRegFailed(  int UserID , char* Account , LoginResultEnum Result )
{
    _Parent->_OnACPlayerRegFailed( UserID , Account , Result );
}
//---------------------------------------------------------------------------------
void LAC::OnPlayerRegFailedData(  int UserID , char* Account , LoginResultEnum Result, void* Data, unsigned long DataSize )
{
	_Parent->_OnACPlayerRegFailedData( UserID , Account , Result, Data, DataSize);
}
//---------------------------------------------------------------------------------
void LAC::OnPlayerLogout( int UserID , char* Account )
{
    _Parent->_OnACPlayerLogout( UserID , Account );
}
//---------------------------------------------------------------------------------
void LAC::OnPlayerLogoutFaild( int UserID , char* Account , LogoutResultEnum Result )
{
    _Parent->_OnACPlayerLogoutFaild( UserID , Account , Result );
}
//---------------------------------------------------------------------------------
bool LAC::CheckAccount( char* Account  )
{
    return _Parent->_CheckAccount( Account );
}
/*
void LAC::ItemExchangeResult ( int GSrvNetID , char* UserAccount , int RoleGUID , CheckExchangeItemTypeENUIM Type , GameItemFieldStruct& Item , int Money , int Money_Account )
{
    _Parent->_ItemExchangeResult( GSrvNetID , UserAccount , RoleGUID , Type , Item , Money , Money_Account );
}
*/
//---------------------------------------------------------------------------------
//LSrvList
//---------------------------------------------------------------------------------
void LSrvList::ClientRelogin( char* Account )
{
	_Parent->_ClientRelogin( Account );
}
//---------------------------------------------------------------------------------
// 初始化 Client 與 Server 的 CallBack函式
//-----------------------------------------------------------------------------------
void	SwitchClass::InitPGCallBack()
{

    int		i;
    PGBaseInfo	PGBase;
    //callback 函式陣列出始化
    for( i = 0 ; i <EM_SysNet_Packet_Count ; i++ )
    {
        _PGCallBack[i] = _PGxUnKnow;
        _PGInfo.push_back(PGBase);
    }

    //(Proxy)
    _PGInfo[ EM_SysNet_Proxy_Switch_Connect ].Type.Proxy        			= true;
    _PGCallBack[ EM_SysNet_Proxy_Switch_Connect ]               			= _PGxProxyConnect;

    _PGInfo[ EM_SysNet_Proxy_Switch_GSrvConnectOK ].Type.Proxy  			= true;
    _PGCallBack[ EM_SysNet_Proxy_Switch_GSrvConnectOK ]         			= _PGxGSrvConnectToProxy;

    _PGInfo[ EM_SysNet_Proxy_Switch_GSrvDisconnect ].Type.Proxy 			= true;
    _PGCallBack[ EM_SysNet_Proxy_Switch_GSrvDisconnect ]        			= _PGxProxy_GSrvProxyDisconnect;

    _PGInfo[ EM_SysNet_Proxy_Switch_CliLoginOK ].Type.Proxy     			= true;
    _PGCallBack[ EM_SysNet_Proxy_Switch_CliLoginOK ]            			= _PGxCliConnectToProxy;

    _PGInfo[ EM_SysNet_Proxy_Switch_CliLogout ].Type.Proxy      			= true;
    _PGCallBack[ EM_SysNet_Proxy_Switch_CliLogout ]             			= _PGxProxy_CliProxyDisconnect;

    _PGInfo[ EM_SysNet_Proxy_Switch_NotifyCliIDLive ].Type.Proxy			= true;
	_PGCallBack[ EM_SysNet_Proxy_Switch_NotifyCliIDLive ]       			= _SysNet_Proxy_Switch_NotifyCliIDLive;

	_PGInfo[ EM_SysNet_Proxy_Switch_GSrvConnectPrepareOK ].Type.Proxy		= true;
	_PGCallBack[ EM_SysNet_Proxy_Switch_GSrvConnectPrepareOK ]       		= _SysNet_Proxy_Switch_GSrvConnectPrepareOK;
    
	_PGInfo[ EM_SysNet_Proxy_Switch_CliConnectPrepareOK ].Type.Proxy		= true;
	_PGCallBack[ EM_SysNet_Proxy_Switch_CliConnectPrepareOK ]       		= _SysNet_Proxy_Switch_CliConnectPrepareOK;


    //(GSrv)
    _PGInfo[ EM_SysNet_GSrv_Switch_Connect ].Type.GSrv          			= true;
    _PGCallBack[ EM_SysNet_GSrv_Switch_Connect ]                			= _PGxGSrvConnect;

    _PGInfo[ EM_SysNet_GSrv_Switch_ProxyDisconnect ].Type.GSrv  			= true;
    _PGCallBack[ EM_SysNet_GSrv_Switch_ProxyDisconnect ]        			= _PGxGSrv_GSrvProxyDisconnect;

    _PGInfo[ EM_SysNet_GSrv_Switch_SetActive ].Type.GSrv        			= true;
    _PGCallBack[ EM_SysNet_GSrv_Switch_SetActive ]              			= _PGxSetGSrvActiveType;

    _PGInfo[ EM_SysNet_GSrv_Switch_DataGSrvToGSrv ].Type.GSrv   			= true;
	_PGCallBack[ EM_SysNet_GSrv_Switch_DataGSrvToGSrv ]         			= _PGxDataGSrvToGSrv;

	_PGInfo[ EM_SysNet_GSrv_Switch_DataGSrvToGSrv_Zip ].Type.GSrv  			= true;
	_PGCallBack[ EM_SysNet_GSrv_Switch_DataGSrvToGSrv_Zip ]        			= _PGxDataGSrvToGSrv_Zip;

	_PGInfo[ EM_SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv ].Type.GSrv      = true;
	_PGCallBack[ EM_SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv ]            = _PGxConnectCliAndGSrv;

	_PGInfo[ EM_SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv ].Type.GSrv   = true;
	_PGCallBack[ EM_SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv ]         = _PGxDisconnectCliAndGSrv;

	_PGInfo[ EM_SysNet_GSrv_Switch_ServerListInfo ].Type.GSrv   			= true;
	_PGCallBack[ EM_SysNet_GSrv_Switch_ServerListInfo ]         			= _PGxServerListInfo;

	_PGInfo[ EM_SysNet_X_DataTransmitToWorldGSrv ].Type.GSrv   				= true;
	_PGInfo[ EM_SysNet_X_DataTransmitToWorldGSrv ].Type.Gateway  				= true;
	_PGCallBack[ EM_SysNet_X_DataTransmitToWorldGSrv ]						= _PGxDataTransmitToWorldGSrv;

	_PGInfo[ EM_SysNet_X_DataTransmitToWorldGSrv_GSrvID ].Type.GSrv   		= true;
	_PGInfo[ EM_SysNet_X_DataTransmitToWorldGSrv_GSrvID ].Type.Gateway  	= true;
	_PGCallBack[ EM_SysNet_X_DataTransmitToWorldGSrv_GSrvID ]				= _PGxDataTransmitToWorldGSrv_GSrvID;

	_PGInfo[ EM_SysNet_GSrv_Switch_RegisterGSrvID ].Type.GSrv   			= true;
	_PGCallBack[ EM_SysNet_GSrv_Switch_RegisterGSrvID ]						= _PGx_RegisterGSrvID;

	_PGInfo[ EM_SysNet_GSrv_Switch_FreezeAccount ].Type.GSrv   				= true;
	_PGCallBack[ EM_SysNet_GSrv_Switch_FreezeAccount ]						= _PGx_FreezeAccount;

    //( Client )
    _PGInfo[ EM_SysNet_Cli_Switch_LoginInfo ].Type.Client       			= true;
    _PGCallBack[ EM_SysNet_Cli_Switch_LoginInfo ]               			= _PGxClientLoginInfo;
	_PGInfo[ EM_SysNet_Cli_Switch_LoginInfo ].MinPGSize	= _PGInfo[ EM_SysNet_Cli_Switch_LoginInfo ].MaxPGSize = sizeof(SysNet_Cli_Switch_LoginInfo);


	//(Gateway)
	_PGInfo[ EM_SysNet_Gateway_Switch_RegisterResult ].Type.Gateway      	= true;
	_PGCallBack[ EM_SysNet_Gateway_Switch_RegisterResult ]               	= _PGxGatewayRegisterResult;
	_PGInfo[ EM_SysNet_Gateway_Switch_WorldConnect ].Type.Gateway       	= true;
	_PGCallBack[ EM_SysNet_Gateway_Switch_WorldConnect ]               		= _PGxGatewayWorldConnect;
	_PGInfo[ EM_SysNet_Gateway_Switch_WorldDisconnect ].Type.Gateway    	= true;
	_PGCallBack[ EM_SysNet_Gateway_Switch_WorldDisconnect ]             	= _PGxGatewayWorldDisconnect;

}
//---------------------------------------------------------------------------------

SwitchClass::SwitchClass()
{
    InitPGCallBack();

    _NetBProxy		= CreateNetBridge( true, 3 );		//主要是用來給Server連結用的 
    _ProxyBListener = NEW LBProxy( this );	//資料列舉Class

    _NetBGSrv		= CreateNetBridge( true, 3 );	//主要是用來給Server連結用的 
    _GSrvBListener = NEW LBGSrv( this );	//資料列舉Class

    _NetBCli		= CreateNetBridge( true, 3);	//主要是用來給Server連結用的 
	_NetBCli->SetSendLimit( true, ( 2 * 1024 * 1024 ) );

    _CliBListener = NEW LBCli( this );   //資料列舉Class

	_NetGateway		= CreateNetBridge( true, 3 );	//主要是用來給Server連結用的 
	_GatewayListener = NEW LBGateway( this );   //資料列舉Class


    _AcListener = NEW LAC( this );
    _AcCli.EventListener( _AcListener );

	_SrvListListener = NEW LSrvList(this);
	_SrvList.EventListener( _SrvListListener );

    
#ifdef DF_KEEPALIVE
    Schedular()->Push( _CheckCliConnectInProxy , 10000 , this , NULL );
#endif
	_ExitFlag = false;
	_SendServerListInfo = false;
	_SwitchReady		= false;

	_NetBProxy->SetOutPutCB( _Output_Server_NetMsg );
	_NetBGSrv->SetOutPutCB( _Output_Server_NetMsg );
	_NetBCli->SetOutPutCB( _Output_Client_NetMsg );

	_GatewayNetID = -1;

	Schedular()->Push( _ReportNetInfoSchedualar , 1000 * 10  , this 	, NULL );		
}

SwitchClass::~SwitchClass()
{
	_ExitFlag = true;

    DeleteNetBridge( _NetBProxy );
    DeleteNetBridge( _NetBGSrv );
    DeleteNetBridge( _NetBCli );
	DeleteNetBridge( _NetGateway );
    delete _ProxyBListener;
    delete _GSrvBListener;
    delete _CliBListener;
	delete _AcListener;
	delete _SrvListListener;
	delete _GatewayListener;


	_ProxyBListener = NULL;
	_GSrvBListener = NULL;
	_CliBListener = NULL;
	_AcListener = NULL;
	_SrvListListener = NULL;
}
//---------------------------------------------------------------------------------
void	SwitchClass::HostProxy( char* IP,int Port )
{
    int IPNum = ConvertIP( IP );
    _NetBProxy->Host( IPNum , Port , _ProxyBListener );	
}
//---------------------------------------------------------------------------------
void	SwitchClass::HostGSrv( char* IP,int Port )
{
    int IPNum = ConvertIP( IP );
    _NetBGSrv->Host( IPNum , Port , _GSrvBListener );	
}
//---------------------------------------------------------------------------------
void	SwitchClass::HostCli( char* IP,int Port )
{
	_ClientHostIP = IP;
	_ClientPort = Port;
    int IPNum = ConvertIP( IP );
    _NetBCli->Host( IPNum , Port , _CliBListener, true, false  );	
}
void	SwitchClass::ConnectGateway( string IP,int Port )
{
	_GatewayIP = IP;
	_GatewayPort = Port;
	int IPNum = ConvertIP( IP.c_str() );
	_NetGateway->Connect( IPNum , Port , _GatewayListener );	
}
//---------------------------------------------------------------------------------
//定時處理
bool	SwitchClass::Process( )
{
	static int iLastCheckTime = GetTickCount();

	int iTime = abs( ( (int)GetTickCount() - iLastCheckTime ) );

	//check every 60 seconds
	if( iTime > 60000 )
	{
		CheckSystemStatus();
		iLastCheckTime = GetTickCount();
	}

	CNetBridge::WaitAll( 10 );
	Sleep(1);

    _AcCli.Process();
	_SrvList.Process();

    return true;
}

void SwitchClass::CheckSystemStatus()
{
	PROCESS_MEMORY_COUNTERS pmc;
	HANDLE hProcess;

	DWORD dwPID = GetCurrentProcessId();

	hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, dwPID );

	GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc));

	Print( LV2, "SYSTEM", "Memory[ %d ]", pmc.WorkingSetSize );

}
//-----------------------------------------------------------------------------------
//送資料到 Proxy
bool    SwitchClass::SendToProxy( int ProxyID , int Size , void* Data )
{
/*    if( _NetProxy->IsValidint( ProxyID ) == false )
    {
        printf( "\nSendToProxy : ProxyID(%d) is Valid NetID !! " , ProxyID );
        return false;
    }
  */  
    _NetBProxy->Send( ProxyID , Size , Data );
    return true;
}
//-----------------------------------------------------------------------------------
//送資料到 Client
bool    SwitchClass::SendToCli( int NetID , int Size , void* Data )
{
    /*if( _NetCli->IsValidint( NetID ) == false )
    {
        printf( "\nSendToCli : Cli NetID(%d) is Valid NetID !! " , NetID );
        return false;
    }*/

    _NetBCli->Send( NetID , Size , Data );
    return true;
}
//-----------------------------------------------------------------------------------
//送資料到 Game Server
bool    SwitchClass::SendToGSrv( int GSrvID , int Size , void* Data )
{
/*    if( _NetGSrv->IsValidint( GSrvID ) == false )
    {
        printf( "\nSendToGSrv : GSrvID(%d) is Valid NetID !! " , GSrvID );
        return false;
    }*/

    _NetBGSrv->Send( GSrvID , Size , Data  );
    return true;
}


CliInfoStruct*  SwitchClass::_NewCli( )
{
	static int MaxID = 0;
    while( _UnUsedClientIDList.size() < 1000 )
    {
        _UnUsedClientIDList.push_back( MaxID++ );
    }
    int ID = _UnUsedClientIDList.front();
    _UnUsedClientIDList.pop_front();

    if( ID > _Max_Cli_Count_ )
        return NULL;

    return _CliList.New( ID );

}
//---------------------------------------------------------------------------------
void    SwitchClass::SendToAllProxy( int Size , void* Data )
{
    if( _NetBProxy != NULL )
		_NetBProxy->SendToAll( Size , Data );
}
//---------------------------------------------------------------------------------
void    SwitchClass::SendToAllGSrv( int Size , void* Data )
{
	if( _NetBGSrv != NULL )
		_NetBGSrv->SendToAll( Size , Data );
}
//---------------------------------------------------------------------------------
//定時檢查 Client 在 Proxy 內的狀況
int    SwitchClass::_CheckCliConnectInProxy( SchedularInfo*	Obj , void* InputClass )
{
    SwitchClass* Class = (SwitchClass*)InputClass;

    CliInfoStruct*  CliInfo;

    int     CheckTime = clock() - 1000 * 120 ;

    Class->_CliList.GetData( true );

    while( 1 )
    {
        CliInfo = Class->_CliList.GetData( );
        if( CliInfo == NULL )
            break;

		if( CliInfo->LastCheckTime < CheckTime )
            break;
	}
	if( CliInfo != NULL )
	{
		//對此Cli沒有定時回應 
        Print( LV5 , "_CheckCliConnectInProxy" ,  "(ERROR)Proxy -> Switch Check live , no respond ??? ProxyID = %d CliID = %d " , CliInfo->ID , CliInfo->ProxyID );

        ProxyInfoStruct* ProxyInfo    = Class->_ProxyList[ CliInfo->ProxyID ];

        if(     ProxyInfo == NULL
            ||  ProxyInfo->Stat    != EM_NetStat_Connect )
        {
			//"Proxy Info 尚未使用 or 狀態不對 ??"
            Print( LV5 , "_CheckCliConnectInProxy" ,"Proxy Info == NULL or stat error ??" );
        }

        else if( ProxyInfo->CliConnect.erase( CliInfo->ID ) == 0 )
        {
			Class->CliLogout( CliInfo->ID, "Switch: _CheckCliConnectInProxy : Proxy Info CliID = %d doesnt exist!!" );
            Print( LV5 , "_CheckCliConnectInProxy" , "Proxy Info CliID = %d does not exist" , CliInfo->ID );
        }
		else
		{
			int GSrvID;
			//把所有與此Cli有關的GSrv 清除相關資料
			set<int>::iterator Iter;

			for( Iter = CliInfo->GSrvConnect.begin() ; Iter != CliInfo->GSrvConnect.end() ; Iter++ )
			{
				GSrvID = *Iter;
				GSrvInfoStruct *GSrvInfo = Class->_GSrvList[ GSrvID ];
				if(     GSrvInfo == NULL
					||  GSrvInfo->Stat    != EM_NetStat_Connect ) 
				{
					//"GSrv and Cli 內的資料不同步????? "
					Print( LV2 , "_CheckCliConnectInProxy" , "GSrv and Cli  out of step  ????? " );
					return 0;
				}

				if( GSrvInfo->CliConnect.find( CliInfo->ID ) == GSrvInfo->CliConnect.end() )
				{
					//
					Print( LV2 , "_CheckCliConnectInProxy" , "GSrv and Cli out of step ????? ( CliID dose not in GSrvInfo )  " );
					return 0;
				}

				SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv DisconnectCliAndGSrv; 
				DisconnectCliAndGSrv.CliID  = CliInfo->ID;
				DisconnectCliAndGSrv.GSrvID = GSrvID;
				Class->SendToProxy( CliInfo->ProxyID , sizeof(DisconnectCliAndGSrv) , &DisconnectCliAndGSrv );


				GSrvInfo->CliConnect.erase( CliInfo->ID );
			}
		}

		Class->_CliList.Del( CliInfo->ID );
		Class->_UnUsedClientIDList.push_back( CliInfo->ID );
		Class->_LoginAccount.erase( CliInfo->Account );
		//把此角色退出
		Print( LV2 , "_CheckCliConnectInProxy" , "Force logout account , Account = %s ID = %d " , CliInfo->Account.c_str() , CliInfo->ID  );
		Class->_AcCli.PlayerLogout( (char*)CliInfo->Account.c_str() , CliInfo->ID );
    }

    Class->Schedular()->Push( _CheckCliConnectInProxy , 10000 , InputClass , NULL );

    return 0;
}
//---------------------------------------------------------------------------------
void    SwitchClass::ConnectAc( char* IP,int Port , int GameID , int GameGroupID , char* GameName )
{
	_WorldName = GameName;
//	_ClientHostIP = IP;
//	_ClientPort ;

    _AcCli.Connect( IP , Port ,  GameID , GameGroupID , GameName );
}
//---------------------------------------------------------------------------------
void    SwitchClass::ConnectServerList( char* IP,int Port )
{
	_SrvList.Connect( IP , Port );
}

//---------------------------------------------------------------------------------
void    SwitchClass::CliLogout( int CliID, const char* pszReason )
{
    CliInfoStruct*      CliInfo   = _CliList[ CliID ]; 
    if( CliInfo == NULL  )
    {
        //TRACE0( "\n CliLogout CliID 資料錯誤" );
		Print( LV2 , "CliLogout" , "Cliount Logout , CliID data error" );
        return;
    }

    int    CliNetID = _CliNetToIDMap.GetNetID( CliID);
    if( CliNetID != -1 )
    {
        _NetBCli->Close( CliNetID );
    }

    SysNet_Proxy_CliLogoutRequest Send;
	Send.CliID = CliID;
	sprintf(Send.Reason, "CliLogout Normal");
    _NetBProxy->Send( CliInfo->ProxyID , sizeof( Send ) , &Send  );

}
//---------------------------------------------------------------------------------
//設定一小段時間後Cli斷線
int		SwitchClass::_CliLogoutSchedualar( SchedularInfo*	Obj , void* InputClass )
{
    SwitchClass* Class = (SwitchClass*)InputClass;
	int CliID = Obj->GetNumber( "CliID");
	Class->CliLogout( CliID, "Account process error" );

	return 0;
}
//---------------------------------------------------------------------------------
int		SwitchClass::SetCliLogout_2Sec( int CliID )
{
	return Schedular()->Push( _CliLogoutSchedualar , 2000 , this 
					, "CliID" , EM_ValueType_int  , CliID
					, NULL );
}
//---------------------------------------------------------------------------------

int		SwitchClass::_ReportNetInfoSchedualar( SchedularInfo*	Obj , void* InputClass )
{
	SwitchClass* Class = (SwitchClass*)InputClass;
	char szOut[4096];

	Class->_NetBCli->GetStatusInfo( szOut );
	Print( Def_PrintLV2, "NetReport" , "_NetBCli:\n%s", szOut );

	Class->_NetBGSrv->GetStatusInfo( szOut );
	Print( Def_PrintLV2, "NetReport" , "_NetBGSrv:\n%s", szOut );

	Class->_NetBProxy->GetStatusInfo( szOut );
	Print( Def_PrintLV2, "NetReport" , "_NetBProxy:\n%s", szOut );

	Class->Schedular()->Push( _ReportNetInfoSchedualar , 1000 * 10 , Class , NULL );

	return 0;
}
//-----------------------------------------------------------------------------------
void SwitchClass::_Output_Client_NetMsg( int lv, const char* szOut )
{
	Print( lv , "Client NetBridge" , szOut );
}
//-----------------------------------------------------------------------------------
void SwitchClass::_Output_Server_NetMsg( int lv, const char* szOut )
{
	Print( lv , "Server NetBridge" , szOut );
}
//-----------------------------------------------------------------------------------
void	SwitchClass::_ClientRelogin( char* Account )
{
	_strlwr( Account );
	map< string , CliInfoStruct*>::iterator Iter = _LoginAccount.find( Account);

	if( _LoginAccount.find( Account) != _LoginAccount.end() )
	{
		CliInfoStruct* LoginPlayer = Iter->second;
		//通知此帳號有人重登 = %s
		Print( LV2 , "_ClientRelogin" , "ServerList notify account=%d relogin" , Account );

		SysNet_Switch_Proxy_NotifyCliIDRelogin Send;

		Send.CliID = LoginPlayer->ID;
		Send.IPNum = LoginPlayer->IPnum;
		SendToProxy( LoginPlayer->ProxyID , sizeof(Send) , &Send );
	}
	else
	{
		//要求通知此帳號有人重登 = %s,但是查不到這個人
		Print( LV5 , "_ClientRelogin" , "ServerList notify account=%d relogin , but account not exist . notify AC logout Account" , Account );
		_AcCli.PlayerLogout( Account , -1 );
	}
}
//-----------------------------------------------------------------------------------
void	SwitchClass::Set_Client_SendBuffLimit( bool bFlag, int iSize )
{
	m_bCheck_Client_SendBuffLimit = bFlag;	
	_NetBCli->SetSendBufferLimit( bFlag, iSize );
}
//-----------------------------------------------------------------------------------
void	SwitchClass::Set_Client_NoRespond( bool bFlag )
{
	_NetBCli->SetCheckRespond( bFlag );
}
//-----------------------------------------------------------------------------------
void	SwitchClass::Set_Server_NoRespond( bool bFlag )
{
	_NetBProxy->SetCheckRespond( bFlag );
	_NetBGSrv->SetCheckRespond( bFlag );
	_NetGateway->SetCheckRespond( bFlag );
}
//-----------------------------------------------------------------------------------
void	SwitchClass::SetRedirectHost	( string Country , string Host , int Port )
{ 
	if( Port == 0 )
	{
		_RedirectHost.erase( Country );
		return;
	}

	IPStruct IpInfo;
	IpInfo.IPStr = Host;
	IpInfo.Prot = Port;
	_RedirectHost[Country] = IpInfo; 
}