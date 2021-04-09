#include ".\Net_ServerList.h"	
#include "../../mainproc/Global.h"
#pragma warning (disable:4949)
#pragma unmanaged
//-------------------------------------------------------------------
/*
int						CNet_ServerList::m_iSrvID		= -1;
CNet_ServerList*		CNet_ServerList::m_pThis		= NULL;

EM_SERVER_TYPE			CNet_ServerList::m_ServerType;
DWORD					CNet_ServerList::m_dwServerLocalID;
GSrvNetWaker*			CNet_ServerList::m_pGlobal->Net();
*/

EM_SERVER_TYPE			CNet_ServerList::m_ServerType;
DWORD					CNet_ServerList::m_dwServerLocalID;
CNet_ServerList*		CNet_ServerList::m_pServerList		= NULL;

//-------------------------------------------------------------------
CNet_ServerList::CNet_ServerList( )
{/*
    m_pGlobal			= pG;
    m_ServerType		= ServerType;
    m_dwServerLocalID	= dwServerLocalID;


    OnSrvConnectFunctionBase pFConnect;
    pFConnect = boost::bind<void>( OnEvent_ServerConnect , this , _1 , _2 );
    m_pGlobal->Net()->RegOnSrvConnectFunction       ( pFConnect );

    OnSrvDisconnectFunctionBase pFDisconnect;
    pFDisconnect = boost::bind<void>( OnEvent_ServerDisconnect , this , _1 );
    m_pGlobal->Net()->RegOnSrvDisConnectFunction    ( pFDisconnect );

    OnCliPacketFunctionBase pf;
    pf = boost::bind<void>( OnPacket_RequestServerData , this , _1 , _2 , _3 );
    m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_RequestServerData,				pf			);
    pf = boost::bind<void>( OnPacket_ResiterServerData , this , _1 , _2 , _3 );
    m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_RegsiterServerData,			pf			);
    pf = boost::bind<void>( OnPacket_TransmitServerData , this , _1 , _2 , _3 );
    m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_TransmitServerData,			pf			);
    pf = boost::bind<void>( OnPacket_RegSrvDataFailed , this , _1 , _2 , _3 );
    m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_RegSrvDataFailed,				pf			);
*/
	m_bRegister			= false;
	m_pServerList		= this;
}
//-------------------------------------------------------------------
CNet_ServerList::~CNet_ServerList(void)
{

}
//-------------------------------------------------------------------
void    CNet_ServerList::Init( Global* pG , EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
    m_pGlobal			= pG;
    m_ServerType		= ServerType;
    m_dwServerLocalID	= dwServerLocalID;
	m_bRegister			= false;

    OnSrvConnectFunctionBase pFConnect;
	pFConnect = boost::bind<void>( &CNet_ServerList::OnEvent_ServerConnect , this , _1 , _2 );
    m_pGlobal->Net()->RegOnSrvConnectFunction       ( pFConnect );

    OnSrvDisconnectFunctionBase pFDisconnect;
    pFDisconnect = boost::bind<void>( &CNet_ServerList::OnEvent_ServerDisconnect , this , _1 );
    m_pGlobal->Net()->RegOnSrvDisConnectFunction    ( pFDisconnect );

    OnCliPacketFunctionBase pf;
    pf = boost::bind<void>( &CNet_ServerList::OnPacket_RequestServerData , this , _1 , _2 , _3 );
    m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_RequestServerData,				pf			);

    pf = boost::bind<void>( &CNet_ServerList::OnPacket_ResiterServerData , this , _1 , _2 , _3 );
    m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_RegsiterServerData,			pf			);

    pf = boost::bind<void>( &CNet_ServerList::OnPacket_TransmitServerData , this , _1 , _2 , _3 );
    m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_TransmitServerData,			pf			);

    pf = boost::bind<void>( &CNet_ServerList::OnPacket_RegSrvDataFailed , this , _1 , _2 , _3 );
    m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_RegSrvDataFailed,				pf			);

	pf = boost::bind<void>( &CNet_ServerList::OnPacket_LocalID , this , _1 , _2 , _3 );
	m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_LocalID,						pf			);

	pf = boost::bind<void>( &CNet_ServerList::OnPacket_ServerReady , this , _1 , _2 , _3 );
	m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_ServerReady,					pf			);

	pf = boost::bind<void>( &CNet_ServerList::OnPacket_Shutdown , this , _1 , _2 , _3 );
	m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_Shutdown,						pf			);

	pf = boost::bind<void>( &CNet_ServerList::OnPacket_RegisterAccountEvent , this , _1 , _2 , _3 );
	m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_S2M_RegisterAccountEvent,				pf			);

}

//-------------------------------------------------------------------
/*
bool CNet_ServerList::_Init( GSrvNetWaker* pNet, EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
    //GSrvNetWaker::RegOnLoginFunction			( _OnLogin );
    //GSrvNetWaker::RegOnLogoutFunction		    ( _OnLogout );
    //GSrvNetWaker::RegOnLoginFailFunction		( _OnLogFailed ); 

	m_pGlobal->Net()				= pNet;
	m_ServerType		= ServerType;
	m_dwServerLocalID	= dwServerLocalID;


    m_pGlobal->Net()->RegOnSrvConnectFunction       ( OnEvent_ServerConnect		);
    m_pGlobal->Net()->RegOnSrvDisConnectFunction    ( OnEvent_ServerDisconnect	);

	m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_RequestServerData,				OnPacket_RequestServerData			);
	m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_RegsiterServerData,			OnPacket_ResiterServerData			);
	m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_TransmitServerData,			OnPacket_TransmitServerData			);
	m_pGlobal->Net()->RegOnSrvPacketFunction		( EM_PG_SL_SL2SL_RegSrvDataFailed,				OnPacket_RegSrvDataFailed			);


    //GSrvNetWaker::RegOnCliConnectFunction       ( _OnCliConnect );
    //GSrvNetWaker::RegOnCliDisconnectFunction    ( _OnCliDisconnect );

	//NetSrv_Test::_Init();

	return false;
}
*/
//-------------------------------------------------------------------
void CNet_ServerList::OnPacket_RegisterAccountEvent( int iServerID, int iSize, PVOID pData )
{
	m_setEventRegAccount.insert( iServerID );
}
//-------------------------------------------------------------------
void CNet_ServerList::OnPacket_Shutdown( int iServerID, int iSize, PVOID pData )
{
	M_PACKET( PG_SL_SL2SL_Shutdown );
	OnServerEvent_Shutdown( iServerID, pPacket->iCloseTime );
}
//-------------------------------------------------------------------
void CNet_ServerList::OnPacket_RequestServerData( int iServerID, int iSize, PVOID pData )
{
	// 收到 Master 要求 Server 資料
	//PG_SL_S2SL_ServerData packetServerData;
	OnServer_RequestServerData( iServerID );
	/*
	PG_SL_S2SL_RegsiterServerData Packet;

	Packet.m_ServerType			= emServerType;
	Packet.m_dwServerLocalID	= iSrvLocalID;
	Packet.m_dwServerID			= dwDataServerID;

	m_pGlobal->Net()->SendToSrv( dwServerID, sizeof( PG_SL_S2SL_ServerData ), &packetServerData );
	*/



	//SendServerData( iServerID,  m_ServerType, m_dwServerLocalID );
}
//-------------------------------------------------------------------
void CNet_ServerList::OnPacket_TransmitServerData	( int iServerID, int iSize, PVOID pData )
{
	M_PACKET( PG_SL_SL2SL_TransmitServerData );
	OnServer_TransmitServerData( pPacket->m_dwServerID, pPacket->m_ServerType, pPacket->m_dwServerLocalID );

}
//-------------------------------------------------------------------
void CNet_ServerList::OnPacket_RegSrvDataFailed	( int iServerID, int iSize, PVOID pData )
{
	M_PACKET( PG_SL_SL2SL_RegSrvDataFailed );
	OnServer_RegSrvDataFailed( iServerID, pPacket->emRegSrvDataFailed );
}
//-------------------------------------------------------------------
void CNet_ServerList::OnPacket_LocalID( int iServerID, int iSize, PVOID pData )
{
	M_PACKET( PG_SL_SL2SL_LocalID );
	OnServer_LocalID( iServerID, pPacket->m_dwLocalID );
}
//-------------------------------------------------------------------
void CNet_ServerList::RegisterServerData				( int iServerID, EM_SERVER_TYPE emServerType, int iSrvLocalID, bool bReg )
{
	PG_SL_SL2SL_RegsiterServerData Packet;

	Packet.m_ServerType			= emServerType;
	Packet.m_dwServerLocalID	= iSrvLocalID;
	Packet.m_bReg				= bReg;

	m_pGlobal->Net()->SendToSrv( iServerID, sizeof( PG_SL_SL2SL_RegsiterServerData ), &Packet );
}
//-------------------------------------------------------------------
void CNet_ServerList::TransmitServerData( int iServerID, int iTransmitServerID, EM_SERVER_TYPE emServerType, int iSrvLocalID )
{
	PG_SL_SL2SL_TransmitServerData Packet;

	Packet.m_ServerType			= emServerType;
	Packet.m_dwServerLocalID	= iSrvLocalID;
	Packet.m_dwServerID			= iTransmitServerID;

	m_pGlobal->Net()->SendToSrv( iServerID, sizeof( PG_SL_SL2SL_TransmitServerData ), &Packet );
}
//-------------------------------------------------------------------
void CNet_ServerList::OnPacket_ResiterServerData( int iServerID, int iSize, PVOID pData )
{
	//PG_SL_S2SL_ServerData*	pPacketServerData	= (PG_SL_S2SL_ServerData*)pData;
	M_PACKET( PG_SL_SL2SL_RegsiterServerData );

	OnServer_ResiterServerData( iServerID, pPacket->m_ServerType, pPacket->m_dwServerLocalID, pPacket->m_bReg );
}
//-------------------------------------------------------------------
void CNet_ServerList::OnPacket_ServerReady		( int iServerID, int iSize, PVOID pData )
{
	M_PACKET( PG_SL_SL2SL_ServerReady );

	OnServer_ServerReady( iServerID, pPacket->m_ServerType, pPacket->m_dwServerLocalID );

}
/*
bool CNet_ServerList::_Release()
{



	return false;
}
*/
//-------------------------------------------------------------------
void CNet_ServerList::OnEvent_ServerConnect( int iServerID , string SrvName )
{
	// 發出封包要求其 Server 傳回 其資料

	//virtual void OnServerConnect	( int iSrvID , string sSrvName ) = 0;
	//virtual void OnServerDisconnect	( int iSrvID ) = 0;

	OnServer_Connect( iServerID, SrvName );



	//ServerData* pServer		= NEW ServerData;
	//pServer->m_dwServerID	= ID;
	//strcpy( pServer->m_szServerNamed, SrvName.c_str() );

	//m_vecServerData.push_back( pServer );
	//m_mapServerData[ pServer->m_dwServerID ] = pServer;
}
//-------------------------------------------------------------------
void CNet_ServerList::OnEvent_ServerDisconnect( int iServerID )
{
	if( m_setEventRegAccount.find( iServerID ) != m_setEventRegAccount.end() )
	{
		m_setEventRegAccount.erase( iServerID );
	}
	
	OnServer_Disconnect( iServerID );
}
//-------------------------------------------------------------------
void CNet_ServerList::RegSrvDataFailed( int iServerID, EM_REG_SRV_FAILED emRegSrvDataFailed )
{
	PG_SL_SL2SL_RegSrvDataFailed Packet;
	
	Packet.emRegSrvDataFailed = emRegSrvDataFailed;

	m_pGlobal->Net()->SendToSrv( iServerID, sizeof( PG_SL_SL2SL_RegSrvDataFailed ), &Packet );
}
//-------------------------------------------------------------------
void CNet_ServerList::SendLocalID ( int iServerID, DWORD dwLocalID )
{
	PG_SL_SL2SL_LocalID Packet;

	Packet.m_dwLocalID = dwLocalID;

	m_pGlobal->Net()->SendToSrv( iServerID, sizeof( PG_SL_SL2SL_LocalID ), &Packet );
}
//-------------------------------------------------------------------
void CNet_ServerList::Shutdown( int iSrvID, int iCloseTime )
{
	PG_SL_SL2SL_Shutdown	Packet;

	Packet.iCloseTime	= iCloseTime;

	m_pGlobal->Net()->SendToSrv( iSrvID, sizeof( PG_SL_SL2SL_LocalID ), &Packet );
}
//-------------------------------------------------------------------
void CNet_ServerList::RegisterAccountEvent()
{
	PG_SL_S2M_RegisterAccountEvent	Packet;
	m_pGlobal->SendToMaster( sizeof( PG_SL_SL2SL_LocalID ), &Packet );
}
//-------------------------------------------------------------------
void CNet_ServerList::AccountEnterWorld( int iClientID, string sAccount )
{
	PG_SL_M2S_AccountEnterWorld Packet;

	Packet.iClientID	= iClientID;
	Packet.Account		= sAccount.c_str();

	for( set<int>::iterator it = m_pServerList->m_setEventRegAccount.begin(); it != m_pServerList->m_setEventRegAccount.end(); it++ )
	{
		m_pGlobal->Net()->SendToSrv( (*it), sizeof( PG_SL_M2S_AccountEnterWorld ), &Packet );
	}
}
//-------------------------------------------------------------------
void CNet_ServerList::AccountLeaveWorld( int iClientID, string sAccount )
{
	PG_SL_M2S_AccountLeaveWorld Packet;

	Packet.iClientID	= iClientID;
	Packet.Account		= sAccount.c_str();

	for( set<int>::iterator it = m_pServerList->m_setEventRegAccount.begin(); it != m_pServerList->m_setEventRegAccount.end(); it++ )
	{
		m_pGlobal->Net()->SendToSrv( (*it), sizeof( PG_SL_M2S_AccountLeaveWorld ), &Packet );
	}
}
//-------------------------------------------------------------------

#pragma managed