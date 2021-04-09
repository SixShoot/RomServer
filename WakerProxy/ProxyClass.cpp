


#include "ProxyClass.h"
#include <Psapi.h>
#include "controllerclient\ControllerClient.h"
//#include "CPU_Usage\CpuUsage.h"
//---------------------------------------------------------------------------------
/*
void LSAPI LSwitch::OnConnected( LNETID NetID )
{
    printf("\n(Switch:%d)_OnSwitchConnected" , NetID );
    Parent->_OnSwitchConnected(NetID);
}
void LSAPI LSwitch::OnDisconnected( LNETID NetID )
{
    printf("\n(Switch:%d)_OnSwitchDisconnected" , NetID );
    Parent->_OnSwitchDisconnected(NetID);
}
void LSAPI LSwitch::OnPeerShutdown( LNETID NetID )
{
    printf("\n(Switch:%d)_OnSwitchDisconnected" , NetID );
    Parent->_OnSwitchDisconnected(NetID);
}
bool LSAPI LSwitch::OnPacket( LNETID NetID, HBUFFER hIncomingData )
{
    //printf("\n(Switch:%d)_OnSwitchPacket" , NetID );
    Parent->_OnSwitchPacket(NetID,hIncomingData);
    return true;
}
void LSAPI LSwitch::OnConnectFailed( int NetID )
{
    printf("\n(Switch:%d)_OnSwitchConnectFailed" , NetID );
    Parent->_OnSwitchConnectFailed(NetID);
}
*/

int	ProxyClass::m_CtoS_PacketID[ DF_MAX_PACKETID ];
int	ProxyClass::m_CtoS_Bandwidth[ DF_MAX_PACKETID ];
int	ProxyClass::m_StoC_PacketID[ DF_MAX_PACKETID ];
int	ProxyClass::m_StoC_Bandwidth[ DF_MAX_PACKETID ];



int		ProxyClass::m_iProxyDisconnectCount[ EM_ClientNetCloseType_End ];

bool		LBSwitch::OnRecv			( unsigned long dwNetID, unsigned long dwPackSize, PVOID pPacketData )
{
    
    _Parent->_OnSwitchPacket( dwNetID, dwPackSize, pPacketData);
    return true;
}
void		LBSwitch::OnConnectFailed	( unsigned long dwNetID, unsigned long dwFailedCode )
{
    Print( LV3 , "OnConnectFailed " , "(Switch)_OnSwitchConnectFailed Error=%d" , dwFailedCode );
    _Parent->_OnSwitchConnectFailed(dwFailedCode);
}
void		LBSwitch::OnConnect		( unsigned long dwNetID )
{
    Print( LV3 , "OnConnect" , "(Switch:%d)_OnSwitchConnected" , dwNetID );
    _Parent->_OnSwitchConnected(dwNetID);
}
void		LBSwitch::OnDisconnect	( unsigned long dwNetID )
{
    Print( LV3 , "OnDisconnect" , "(Switch:%d)_OnSwitchDisconnected" , dwNetID );
    _Parent->_OnSwitchDisconnected(dwNetID);    
}
CEventObj*	LBSwitch::OnAccept		( unsigned long dwNetID )
{
    return this;
}
//---------------------------------------------------------------------------------
/*
void LSAPI LGSrv::OnConnected( LNETID NetID )
{
    printf("\n(GSrv:%d)_OnGSrvConnected" , NetID );
    Parent->_OnGSrvConnected(NetID);
}
void LSAPI LGSrv::OnDisconnected( LNETID NetID )
{
    printf("\n(GSrv:%d)_OnGSrvDisconnected" , NetID );
    Parent->_OnGSrvDisconnected(NetID);
}
void LSAPI LGSrv::OnPeerShutdown( LNETID NetID )
{
    printf("\n(GSrv:%d)_OnGSrvDisconnected" , NetID );
    Parent->_OnGSrvDisconnected(NetID);
}
bool LSAPI LGSrv::OnPacket( LNETID NetID, HBUFFER hIncomingData )
{
    //printf("\n(GSrv:%d)_OnGSrvPacket" , NetID );
    Parent->_OnGSrvPacket(NetID,hIncomingData);
    return true;
}
void LSAPI LGSrv::OnConnectFailed( int NetID )
{
    printf("\n(GSrv:%d)_OnGSrvConnectFailed" , NetID );
    Parent->_OnGSrvConnectFailed(NetID);
}
*/
bool		LBGSrv::OnRecv			( unsigned long dwNetID, unsigned long dwPackSize, PVOID pPacketData )
{
    _Parent->_OnGSrvPacket( dwNetID, dwPackSize, pPacketData);
    return true;
}
void		LBGSrv::OnConnectFailed	( unsigned long dwNetID, unsigned long dwFailedCode )
{
    Print( LV3 , "OnConnectFailed" , "(GSrv)_OnGSrvConnectFailed Error= %d" , dwFailedCode );
    _Parent->_OnGSrvConnectFailed(dwFailedCode);
}
void		LBGSrv::OnConnect		( unsigned long dwNetID )
{
    Print( LV3 , "OnConnect" , "(GSrv:%d)_OnGSrvConnected" , dwNetID );
    _Parent->_OnGSrvConnected(dwNetID);
}
void		LBGSrv::OnDisconnect	( unsigned long dwNetID )
{
    Print( LV3 , "OnDisconnect" , "(GSrv:%d)_OnGSrvDisconnected" , dwNetID );
    _Parent->_OnGSrvDisconnected(dwNetID);
}
CEventObj*	LBGSrv::OnAccept		( unsigned long dwNetID )
{
    return this;
}

//---------------------------------------------------------------------------------
/*
void LSAPI LCli::OnConnected( LNETID NetID )
{
    printf("\n(LCli:%d)_OnCliConnected" , NetID );
    Parent->_OnCliConnected(NetID);
}
void LSAPI LCli::OnDisconnected( LNETID NetID )
{
    printf("\n(LCli:%d)_OnCliDisconnected" , NetID );
    Parent->_OnCliDisconnected(NetID);
}
void LSAPI LCli::OnPeerShutdown( LNETID NetID )
{
    printf("\n(LCli:%d)_OnCliDisconnected" , NetID );
    Parent->_OnCliDisconnected(NetID);
}
bool LSAPI LCli::OnPacket( LNETID NetID, HBUFFER hIncomingData )
{
    //printf("\n(LCli:%d)_OnGSrvPacket" , NetID );
    Parent->_OnCliPacket(NetID,hIncomingData);
    return true;
}
void LSAPI LCli::OnConnectFailed( int NetID )
{
    printf("\n(LCli:%d)_OnCliConnectFailed" , NetID );
    Parent->_OnCliConnectFailed(NetID);
}
*/
bool		LBCli::OnRecv			( unsigned long dwNetID, unsigned long dwPackSize, PVOID pPacketData )
{
    _Parent->_OnCliPacket( dwNetID, dwPackSize, pPacketData );
    return true;
}
void		LBCli::OnConnectFailed	( unsigned long dwNetID, unsigned long dwFailedCode )
{
    Print(LV2 , "OnConnectFailed" , "(LCli)_OnCliConnectFailed Error=%d" , dwFailedCode );
    _Parent->_OnCliConnectFailed(dwFailedCode);
}
void		LBCli::OnConnect		( unsigned long dwNetID )
{
    Print( LV1 , "OnConnect" , "(LCli:%d)_OnCliConnected" , dwNetID );
    _Parent->_OnCliConnected(dwNetID);
}
void		LBCli::OnDisconnect	( unsigned long dwNetID )
{
    Print( LV1 , "OnDisconnect" , "(LCli:%d)_OnCliDisconnected" , dwNetID );
    _Parent->_OnCliDisconnected(dwNetID);
}
CEventObj*	LBCli::OnAccept		( unsigned long dwNetID )
{
    return this;
}
//-----------------------------------------------------------------------------------
// 初始化 Client 與 Server 的 CallBack函式
//-----------------------------------------------------------------------------------
void	ProxyClass::InitPGCallBack()
{

    int		i;
    PGBaseInfo	PGBase;
    //callback 函式陣列出始化
    for( i = 0 ; i < EM_SysNet_Packet_Count ; i++ )
    {
        _PGCallBack[i] = _PGxUnKnoew;
        _PGInfo.push_back(PGBase);
    }

    _PGInfo[ EM_SysNet_Switch_Proxy_ConnectNotify ].Type.Switch         = true;
    _PGCallBack[ EM_SysNet_Switch_Proxy_ConnectNotify ]                 = _PGxConnectNotify;

    _PGInfo[ EM_SysNet_Switch_Proxy_GSrvConnect ].Type.Switch           = true;
    _PGCallBack[ EM_SysNet_Switch_Proxy_GSrvConnect ]                   = _PGxGSrvConnectInfo;

    _PGInfo[ EM_SysNet_Switch_Proxy_GSrvDisconnectNotify ].Type.Switch  = true;
    _PGCallBack[ EM_SysNet_Switch_Proxy_GSrvDisconnectNotify ]          = _PGxGSrvDisconnectNodify;

    _PGInfo[ EM_SysNet_Switch_Proxy_SetActive ].Type.Switch             = true;
    _PGCallBack[ EM_SysNet_Switch_Proxy_SetActive ]                     = _PGxSetGSrvActiveType;

    _PGInfo[ EM_SysNet_Switch_Proxy_CliConnect ].Type.Switch            = true;
    _PGCallBack[ EM_SysNet_Switch_Proxy_CliConnect ]                    = _PGxCliLoginNotify;

	_PGInfo[ EM_SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv ].Type.Switch       = true;
	_PGCallBack[ EM_SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv ]               = _PGxConnectGSrvAndCli;

	_PGInfo[ EM_SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv ].Type.Switch    = true;
	_PGCallBack[ EM_SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv ]            = _PGxDisconnectGSrvAndCli;

	_PGInfo[ EM_SysNet_Switch_Proxy_NotifyCliIDRelogin ].Type.Switch    = true;
	_PGCallBack[ EM_SysNet_Switch_Proxy_NotifyCliIDRelogin ]            = _NotifyCliIDRelogin;



    _PGInfo[ EM_SysNet_GSrv_Proxy_Connect ].Type.GSrv                   = true;
    _PGCallBack[ EM_SysNet_GSrv_Proxy_Connect ]                         = _PGxGSrvConnect;

    _PGInfo[ EM_SysNet_Proxy_CliLogoutRequest ].Type.GSrv               = true;
    _PGInfo[ EM_SysNet_Proxy_CliLogoutRequest ].Type.Switch             = true;
    _PGCallBack[ EM_SysNet_Proxy_CliLogoutRequest ]                     = _PGxCliLogout;

    _PGInfo[ EM_SysNet_GSrv_Proxy_DataGSrvToCli ].Type.GSrv             = true;
    _PGCallBack[ EM_SysNet_GSrv_Proxy_DataGSrvToCli ]                   = _PGxDataGSrvToCli;

    _PGInfo[ EM_SysNet_Switch_Proxy_SetActive ].Type.Switch             = true;
    _PGCallBack[ EM_SysNet_Switch_Proxy_SetActive ]                     = _PGxSetGSrvActiveType;


    _PGInfo[ EM_SysNet_Cli_Proxy_LoginInfo ].Type.Client                = true;
    _PGCallBack[ EM_SysNet_Cli_Proxy_LoginInfo ]                        = _PGxGCliConnect;
	_PGInfo[ EM_SysNet_Cli_Proxy_LoginInfo ].MinPGSize = _PGInfo[ EM_SysNet_Cli_Proxy_LoginInfo ].MaxPGSize = sizeof(SysNet_Cli_Proxy_LoginInfo);


    _PGInfo[ EM_SysNet_Cli_Proxy_DataCliToGSrv ].Type.Client            = true;
    _PGCallBack[ EM_SysNet_Cli_Proxy_DataCliToGSrv ]                    = _PGxDataCliToGSrv;
	_PGInfo[ EM_SysNet_Cli_Proxy_NetClose ].MinPGSize					= sizeof(SysNet_Cli_Proxy_NetClose);

	_PGInfo[ EM_SysNet_GSrv_Proxy_PingLog ].Type.GSrv           		= true;
	_PGCallBack[ EM_SysNet_GSrv_Proxy_PingLog ]                   		= _PGxOnPingLog;

	_PGInfo[ EM_SysNet_Cli_Proxy_NetClose ].Type.Client                = true;
	_PGCallBack[ EM_SysNet_Cli_Proxy_NetClose ]                        = _PGxOnNetClose;
	_PGInfo[ EM_SysNet_Cli_Proxy_NetClose ].MinPGSize = _PGInfo[ EM_SysNet_Cli_Proxy_NetClose ].MaxPGSize = sizeof(SysNet_Cli_Proxy_NetClose);

}
//-----------------------------------------------------------------------------------
ProxyClass::ProxyClass()
{
    InitPGCallBack();

    _ExitFlag       = false;

	_PacketDelay	= false;

    _NetBSwitch		= CreateNetBridge( true, 3 );		//主要是用來給Server連結用的 
//    _SwitchListener = new LSwitch;			//資料列舉Class
    _SwitchBListener = NEW LBSwitch(this);			//資料列舉Class

    _NetBGSrv		= CreateNetBridge( true, 3 );		//主要是用來給Server連結用的 
    _GSrvBListener   = NEW LBGSrv(this);			//資料列舉Class



    _NetBCli		    = CreateNetBridge( true, 3  );		//主要是用來給Server連結用的 
	_NetBCli->SetSendLimit( true, ( 5 * 1024 * 1024 ) );
	//_NetBCli		    = CreateAsio( true );		//主要是用來給Server連結用的 



    _CliBListener    = NEW LBCli(this);			    //資料列舉Class
    

    _NetSwitchID    = -1;                   //設定為尚未連線狀態
    _ProxyID        = -1;

	_Schedular = NEW FunctionSchedularClass;
    _Schedular->Push( _CheckClientConnectData , 10000 , this , NULL );


	_NetBSwitch->SetOutPutCB( _Output_Server_NetMsg );
	_NetBGSrv->SetOutPutCB( _Output_Server_NetMsg );
	_NetBCli->SetOutPutCB( _Output_Client_NetMsg );


	_NetBCli->SetCheckRespond( true ); 

	_Schedular->Push( _ReportNetInfoSchedualar,		1000 * 10  , this 	, NULL );	
	_Schedular->Push( _ReportPacketInfoSchedualar,	1000 * 180  , this 	, NULL );	


	ZeroMemory( m_iProxyDisconnectCount, sizeof( m_iProxyDisconnectCount ) );

}
//---------------------------------------------------------------------------------
ProxyClass::~ProxyClass()
{
    DeleteNetBridge( _NetBSwitch );
    DeleteNetBridge( _NetBGSrv );
    DeleteNetBridge( _NetBCli );
    delete _SwitchBListener;
    delete _GSrvBListener;
    delete _CliBListener;
	delete _Schedular;
}
//---------------------------------------------------------------------------------
int     ProxyClass::_CheckClientConnectData( SchedularInfo* Obj , void* InputClass )
{
	//DWORD dwStart = GetTickCount();


    ProxyClass* Class = (ProxyClass*)InputClass;

    CliInfoStruct*  CliInfo;
    SysNet_Proxy_Switch_NotifyCliIDLive Send;

    Class->_CliList.GetData( true );

    while( 1 )
    {
        CliInfo = Class->_CliList.GetData( );
        if( CliInfo == NULL )
            break;

        if( CliInfo->Stat != EM_NetStat_Connect )
		{
            continue;
		}


        Send.CliID = CliInfo->ID;
        Class->SendToSwitch( sizeof(Send) ,& Send );
    }

    Class->_Schedular->Push( _CheckClientConnectData , 10000 , InputClass , NULL );

	//DWORD dwTime = abs( GetTickCount() - dwStart );

	//Print( LV3, "ProxyDebug", "_CheckClientConnectData() time = %d", dwTime );

    return 0;
}
//---------------------------------------------------------------------------------
void	ProxyClass::ConnectSwitch( char* IP,int Port )
{
//    MAKE_IP_ADDRESS( (sockaddr_in*)&_SwitchAddr,IP , Port );
    //_NetBSwitch->Connect( &_SwitchAddr , _SwitchBListener );	
    int IPNum = ConvertIP( IP );
    _SwitchIPNum  = IPNum;
    _SwitchIPPort = Port;
    _NetBSwitch->Connect( IPNum , Port , _SwitchBListener );
}
//---------------------------------------------------------------------------------
void	ProxyClass::CheckSystemStatus()
{
	PROCESS_MEMORY_COUNTERS pmc;
	HANDLE hProcess;

	DWORD		dwID		= GetCurrentProcessId();
		
	//static CCpuUsage	CPU_SYSTEM;
	//static CCpuUsage	CPU_PROCESS;


	hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, dwID );

	GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc));

	//-----------------------------

	//DWORD	dwSystemCPU		= CPU_SYSTEM.GetCpuUsage();
	//DWORD	dwProcessCPU	= CPU_PROCESS.GetCpuUsage( dwID );

	Print( LV2, "SYSTEM", "Memory[ %d ]", pmc.WorkingSetSize );

}
//---------------------------------------------------------------------------------
//定時處理
bool	ProxyClass::Process( )
{
/*
	static DWORD dwNextCheckTime = GetTickCount() + 300000;

	if( GetTickCount() > dwNextCheckTime )
	{
		CheckSystemStatus();
		dwNextCheckTime = GetTickCount() + 300000;
	}
*/

	//DWORD dwStart = GetTickCount();

	static int iLastCheckTime = GetTickCount();

	int iTime = abs( ( (int)GetTickCount() - iLastCheckTime ) );

	//check every 60 seconds
	if( iTime > 60000 )
	{
		CheckSystemStatus();
		iLastCheckTime = GetTickCount();
	}

	CNetBridge::WaitAll( 0 );
	Sleep( 1 );
	//SleepEx( 1 , true );
    //timeval t1 = {0,0};
    //_NetBSwitch->Flush( );
    //_NetBGSrv->Flush( );
    //_NetBCli->Flush( );
    _Schedular->Process( );

	//DWORD dwTime = GetTickCount() - dwStart;

	//if( dwTime != 0 )
	//{
	//	Print( LV3, "ProxyDebug", "ProxyClass::Process() time == %d", dwTime );
	//}

    return !_ExitFlag;
}
//---------------------------------------------------------------------------------
//送資料到 Switch
bool    ProxyClass::SendToSwitch( int Size , void* Data )
{
    if( _NetSwitchID == -1 )
        return false;

    _NetBSwitch->Send( _NetSwitchID , Size , Data );
    return true;
}
//---------------------------------------------------------------------------------
bool    ProxyClass::SendToGSrv( int NetID , int Size , void* Data )
{
/*    if( _NetGSrv->IsValidLNETID( NetID ) == false )
    {
        printf( "\nSendToGSrv : GSrvID(%d) is Valid NetID !! " , NetID );
        return false;
    }*/

    _NetBGSrv->Send( NetID , Size , Data  );
    return true;
}
//---------------------------------------------------------------------------------
bool    ProxyClass::SendToCli( int NetID , int Size , void* Data )
{
/*    if( _NetCli->IsValidLNETID( NetID ) == false )
    {
        printf( "\nSendToCli : Cli(%d) is Valid NetID !! " , NetID );
        return false;
    }
*/
    _NetBCli->Send( NetID , Size , Data );
    return true;
}
void	ProxyClass::SendToAllCli( int Size , void* Data )
{
	_NetBCli->SendToAll( Size , Data );
}
//---------------------------------------------------------------------------------
bool    ProxyClass::SendToGSrv_GSrvID( int GSrvID , int Size , void* Data )
{
    int  NetID = _GSrvNetToIDMap.GetNetID( GSrvID );
    return SendToGSrv( NetID , Size , Data );
}
//---------------------------------------------------------------------------------
bool    ProxyClass::SendToCli_CliID( int CliID , int Size , void* Data )
{
    int  NetID = _CliNetToIDMap.GetNetID( CliID );
    return SendToCli( NetID , Size , Data );
}
//---------------------------------------------------------------------------------
void    ProxyClass::SetGSrvHost( char* IP,int Port )
{
    _GSrvHostIP   =  IP;
    _GSrvHostPort =  Port;
}
//---------------------------------------------------------------------------------
void    ProxyClass::SetCliHost( char* IP,int Port )
{
    _CliHostIP   =  IP;
    _CliHostPort =  Port;
}
//---------------------------------------------------------------------------------
void	ProxyClass::HostCli( char* IP,int Port )
{
//    sockaddr_in		addr;
//    MAKE_IP_ADDRESS( (sockaddr_in*)&addr , IP , Port );
    int IPNum = ConvertIP( IP );
    _NetBCli->Host( IPNum , Port , _CliBListener, true, false );	
}
//---------------------------------------------------------------------------------
void	ProxyClass::HostGSrv( char* IP,int Port )
{
    //sockaddr_in		addr;
    //MAKE_IP_ADDRESS( (sockaddr_in*)&addr,IP , Port );
    int IPNum = ConvertIP( IP );
    _NetBGSrv->Host( IPNum , Port , _GSrvBListener );	
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// Connect Cli and Game Server
bool    ProxyClass::Connect_GSrv_Cli( int GSrvID , int CliID )
{
    CliInfoStruct* CliInfo = _CliList[ CliID ];
    if(     CliInfo == NULL 
        ||  CliInfo->Stat != EM_NetStat_Connect )
    {
        Print( LV5 , "Connect_GSrv_Cli" , "CliInfo 資料有問題 ??(使用錯誤??) GSrvID=%d CliID=%d " , GSrvID , CliID );
        return false;
    }

    GSrvInfoStruct* GSrvInfo = _GSrvList[ GSrvID ];
    if(     GSrvInfo == NULL 
        ||  GSrvInfo->Stat != EM_NetStat_Connect )
    {
        Print( LV5 , "Connect_GSrv_Cli" , "GSrvInfo 資料有問題 ??(使用錯誤??) GSrvID=%d CliID=%d " , GSrvID , CliID );
        return false;
    }

    //檢查是否已經聯線成立
    if( GSrvInfo->CliConnect.find( CliID ) != GSrvInfo->CliConnect.end() )
    {
        Print( LV5 , "Connect_GSrv_Cli" , "GSrvInfo 內的 Cli 聯線已存在??? GSrvID=%d CliID=%d " , GSrvID , CliID  );
        return false;
    }

    if( CliInfo->GSrvConnect.find( GSrvID ) != CliInfo->GSrvConnect.end() )
    {
        Print( LV5 , "Connect_GSrv_Cli" , "CliInfo 內的 GSrvID 聯線已存在??? GSrvID=%d CliID=%d " , GSrvID , CliID  );
        return false;
    }
    
    SysNet_Proxy_Cli_GSrvOnConnect  GSrvOnConnect;
    GSrvOnConnect.GSrvID = GSrvID;
    memcpy( GSrvOnConnect.GSrvName , GSrvInfo->GSrvName , sizeof(GSrvInfo->GSrvName) );
    SendToCli_CliID( CliID , sizeof(GSrvOnConnect) , &GSrvOnConnect );

    SysNet_Proxy_GSrv_CliOnConnect  CliOnConnect;
	CliOnConnect.CliID   = CliID;
	CliOnConnect.IpNun   = CliInfo->IPnum;
    MyStrcpy( CliOnConnect.Account , CliInfo->Account.c_str() , sizeof(CliOnConnect.Account) ); 
    SendToGSrv_GSrvID( GSrvID , sizeof(CliOnConnect) , &CliOnConnect );

    CliInfo->GSrvConnect.insert( GSrvID );
    GSrvInfo->CliConnect.insert( CliID );
    return true;
}
//---------------------------------------------------------------------------------
// DisConnect Cli and Game Server
bool    ProxyClass::Disconnect_GSrv_Cli( int GSrvID , int CliID )
{
    CliInfoStruct* CliInfo = _CliList[ CliID ];
    if(     CliInfo == NULL  
        ||  CliInfo->Stat != EM_NetStat_Connect )
    {
        Print( LV2 , "Disconnect_GSrv_Cli" ,  "Disconnect_GSrv_Cli : CliInfo 資料有問題 ??(使用錯誤??) GSrvID=%d CliID=%d " , GSrvID , CliID );
        return false;
    }

    GSrvInfoStruct* GSrvInfo = _GSrvList[ GSrvID ];
    if(     GSrvInfo == NULL
        ||  GSrvInfo->Stat != EM_NetStat_Connect )
    {
        Print( LV2 , "Disconnect_GSrv_Cli" ,"Disconnect_GSrv_Cli : GSrvInfo 資料有問題 ??(使用錯誤??) GSrvID=%d CliID=%d " , GSrvID , CliID );
        return false;
    }

    //檢查是否已經聯線成立
    if( GSrvInfo->CliConnect.erase( CliID ) == 0 )
    {
        Print( LV2 , "Disconnect_GSrv_Cli" , "Disconnect_GSrv_Cli : GSrvInfo 內的 Cli 聯線不存在??? GSrvID=%d CliID=%d " , GSrvID , CliID  );
        return false;
    }

    if( CliInfo->GSrvConnect.erase( GSrvID ) == 0 )
    {
        Print( LV2 , "Disconnect_GSrv_Cli" , "Disconnect_GSrv_Cli : CliInfo 內的 GSrvID 聯線不存在??? GSrvID=%d CliID=%d " , GSrvID , CliID  );
        return false;
    }

    SysNet_Proxy_Cli_GSrvOnDisConnect  GSrvOnDisConnect;
    GSrvOnDisConnect.GSrvID = GSrvID;
    SendToCli_CliID( CliID , sizeof(GSrvOnDisConnect) , &GSrvOnDisConnect );

    SysNet_Proxy_GSrv_CliOnDisConnect  CliOnDisConnect;
    CliOnDisConnect.CliID = CliID;
    SendToGSrv_GSrvID( GSrvID , sizeof(CliOnDisConnect) , &CliOnDisConnect );

    return true;
}
//---------------------------------------------------------------------------------

int		ProxyClass::_ReportNetInfoSchedualar( SchedularInfo*	Obj , void* InputClass )
{
	ProxyClass* Class = (ProxyClass*)InputClass;
	char szOut[4096];

	Class->_NetBCli->GetStatusInfo( szOut );
	//Print( Def_PrintLV2, "NetReport" , "_NetBCli:\n%s", szOut );
	PrintNetstatus( Def_PrintLV2, "NetReport" , "_NetBCli:\n%s", szOut );


	Class->_NetBGSrv->GetStatusInfo( szOut );
	//Print( Def_PrintLV2, "NetReport" , "_NetBGSrv:\n%s", szOut );
	PrintNetstatus( Def_PrintLV2, "NetReport" , "_NetBGSrv:\n%s", szOut );


	Class->_NetBCli->GetRespondInfo( szOut );
	//Print( Def_PrintLV2, "NetReport" , "_NetBCli Ping:\n%s", szOut );
	PrintNetstatus( Def_PrintLV2, "NetReport" , "_NetBCli Ping:\n%s", szOut );



	// 記算斷線率

	// 記算不合理斷線及合理

	// 計算 Total
	int iTotalDisconnect = 0;
	for( int i = 0; i < EM_ClientNetCloseType_End; i++ )
	{
		iTotalDisconnect += m_iProxyDisconnectCount[ i ];
	}
	
	float fDisconnect	= 0;
	float fServerError	= 0;
	float fNormal		= 0;
	int	  iNormalCount	= 0;	

	if( iTotalDisconnect != 0 )
	{
		fDisconnect		= ((float)m_iProxyDisconnectCount[ EM_ClientNetCloseType_None ]) / ((float)iTotalDisconnect);
		fServerError	= ((float)m_iProxyDisconnectCount[ EM_ClientNetCloseType_ServerError ]) / ((float)iTotalDisconnect);
		fNormal			= 1 - fDisconnect - fServerError;
		iNormalCount	= iTotalDisconnect - m_iProxyDisconnectCount[ EM_ClientNetCloseType_None ] - m_iProxyDisconnectCount[ EM_ClientNetCloseType_ServerError ];
	}

	sprintf( szOut, "Disconnect [%.2f][%d], Server kick[%.2f][%d], Normal[%.2f][%d]", fDisconnect, m_iProxyDisconnectCount[ EM_ClientNetCloseType_None ], fServerError, m_iProxyDisconnectCount[ EM_ClientNetCloseType_ServerError ], fNormal, iNormalCount );
	PrintNetstatus( Def_PrintLV2, "NetReport" , "LogoutEvent:\n%s", szOut );

	ZeroMemory( m_iProxyDisconnectCount, sizeof( m_iProxyDisconnectCount ) );









	Class->_Schedular->Push( _ReportNetInfoSchedualar , 1000 * 10 , Class , NULL );

	return 0;
}
//-----------------------------------------------------------------------------------
void ProxyClass::_Output_Server_NetMsg			( int lv, const char* szOut )
{
	//Print( Def_PrintLV1, "%s", szOut );
	Print( lv , "Server NetBridge" , szOut );
}
//-----------------------------------------------------------------------------------
void ProxyClass::_Output_Client_NetMsg			( int lv, CONST char* szOut )
{
	//Print( Def_PrintLV1, "%s", szOut );
	Print( lv , "Client NetBridge" , szOut );
}

//-----------------------------------------------------------------------------------
void	ProxyClass::Set_Client_NoRespond( bool bFlag )
{
	_NetBCli->SetCheckRespond( bFlag );
}
//-----------------------------------------------------------------------------------
void	ProxyClass::Set_Server_NoRespond( bool bFlag )
{
	_NetBSwitch->SetCheckRespond( bFlag );
	_NetBGSrv->SetCheckRespond( bFlag );
}
//-----------------------------------------------------------------------------------
int ProxyClass::_ReportPacketInfoSchedualar( SchedularInfo*	Obj , void* InputClass )
{
	ProxyClass* Class = (ProxyClass*)InputClass;
	char szOut[4096];

	// 統計封包流量
	int		iCount			= 0;
	DWORD	TotalPacket		= 0;
	DWORD	TotalBandwidth	= 0;
	char	szBuff[1024];
	int		iTotalCount		= 0;

	string	sReport;

	// CtoS
	{
		multimap< DWORD, DWORD, greater<DWORD> >	mapPacketID;
		multimap< DWORD, DWORD, greater<DWORD> >	mapPacketBandwidth;

		for( int i=0; i<DF_MAX_PACKETID;i++ )
		{
			TotalPacket			+= m_CtoS_PacketID[ i ];
			TotalBandwidth		+= m_CtoS_Bandwidth[ i ];

			if( m_CtoS_PacketID[ i ] != 0 )
			{
				mapPacketID.insert( make_pair( m_CtoS_PacketID[ i ], i ) );
			}

			if( m_CtoS_Bandwidth[ i ] != 0 )
			{
				mapPacketBandwidth.insert( make_pair( m_CtoS_Bandwidth[ i ], i ) );
			}
		}

		memset( m_CtoS_PacketID, 0, sizeof( m_CtoS_PacketID ) );
		memset( m_CtoS_Bandwidth, 0, sizeof( m_CtoS_Bandwidth ) );


		sReport = "ProxyCtoS PacketID Analyze =";

		iCount	= 0;
		iTotalCount = mapPacketID.size();
		if( iTotalCount > 10 )
			iTotalCount = 10;

		for( multimap< DWORD, DWORD, greater<DWORD> >::iterator it = mapPacketID.begin(); it != mapPacketID.end(); it++ )
		{
			if( iCount < iTotalCount )
			{
				sprintf( szBuff, "P%d [ %d ]= %d(%.2f) ,", iCount, it->second, it->first, ( ((float)it->first)  * 100 / (float)TotalPacket ) );
				sReport += szBuff;
			}
			else
				break;

			iCount++;
		}

		PrintNetstatus( 2, "ProxyCtoS_PacketID", sReport.c_str() );

		sReport = "ProxyCtoS PacketBandwidth Analyze =";

		iCount	= 0;
		iTotalCount = mapPacketBandwidth.size();
		if( iTotalCount > 10 )
			iTotalCount = 10;


		for( multimap< DWORD, DWORD, greater<DWORD> >::iterator it = mapPacketBandwidth.begin(); it != mapPacketBandwidth.end(); it++ )
		{
			if( iCount < iTotalCount )
			{
				sprintf( szBuff, "B%d [ %d ]= %d(%.2f) ,", iCount, it->second, it->first, ( ((float)it->first)  * 100 / (float)TotalBandwidth ) );
				sReport += szBuff;
			}
			else
				break;

			iCount++;
		}

		PrintNetstatus( 2, "ProxyCtoS_PacketBandwidth", sReport.c_str() );


	}

	// StoC
	{

		TotalPacket		= 0;
		TotalBandwidth	= 0;

		multimap< DWORD, DWORD, greater<DWORD> >	mapPacketID;
		multimap< DWORD, DWORD, greater<DWORD> >	mapPacketBandwidth;

		for( int i=0; i<DF_MAX_PACKETID;i++ )
		{
			TotalPacket			+= m_StoC_PacketID[ i ];
			TotalBandwidth		+= m_StoC_Bandwidth[ i ];

			if( m_StoC_PacketID[ i ] != 0 )
				mapPacketID.insert( make_pair( m_StoC_PacketID[ i ], i ) );

			if( m_StoC_Bandwidth[ i ] != 0 )
				mapPacketBandwidth.insert( make_pair( m_StoC_Bandwidth[ i ], i ) );
		}

		memset( m_StoC_PacketID, 0, sizeof( m_StoC_PacketID ) );
		memset( m_StoC_Bandwidth, 0, sizeof( m_StoC_Bandwidth ) );

		iCount	= 0;
		sReport = "ProxyStoC PacketID Analyze =";

		iTotalCount = mapPacketID.size();
		if( iTotalCount > 10 )
			iTotalCount = 10;

		for( multimap< DWORD, DWORD, greater<DWORD> >::iterator it = mapPacketID.begin(); it != mapPacketID.end(); it++ )
		{
			if( iCount < iTotalCount )
			{
				sprintf( szBuff, "P%d [ %d ]= %d(%.2f) ,", iCount, it->second, it->first, ( ((float)it->first) * 100 / (float)TotalPacket ) );
				sReport += szBuff;
			}
			else
				break;

			iCount++;
		}

		PrintNetstatus( 2, "ProxyStoC_PacketID", sReport.c_str() );

		sReport = "ProxyStoC PacketBandwidth Analyze =";

		iCount	= 0;
		iTotalCount = mapPacketBandwidth.size();
		if( iTotalCount > 10 )
			iTotalCount = 10;

		for( multimap< DWORD, DWORD, greater<DWORD> >::iterator it = mapPacketBandwidth.begin(); it != mapPacketBandwidth.end(); it++ )
		{
			if( iCount < iTotalCount )
			{
				sprintf( szBuff, "B%d [ %d ]= %d(%.2f) ,", iCount, it->second, it->first, ( ((float)it->first) * 100 / (float)TotalBandwidth ) );
				sReport += szBuff;
			}
			else
				break;

			iCount++;
		}

		PrintNetstatus( 2, "ProxyStoC_PacketBandwidth", sReport.c_str() );



	}

	Class->_Schedular->Push( _ReportPacketInfoSchedualar , 1000 * 180 , Class , NULL );
	return 0;


}