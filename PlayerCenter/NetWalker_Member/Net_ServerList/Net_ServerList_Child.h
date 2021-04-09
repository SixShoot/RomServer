#pragma once

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <functional>

//#include "..\NetWaker\GSrvNetWaker.h"
#include "Net_ServerList.h"	
#include <unordered_map>
#include "roledata/RoleBaseDef.h"

//typedef	void	(*pfEventServerStatusCB)		( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
//typedef	void	(*pfEventServerRegFailedCB)		( EM_REG_SRV_FAILED ememRegSrvDataFailed );

#define pfEventServerStatusCB       boost::function< void( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID ) >
#define pfEventServerRegFailedCB    boost::function< void( EM_REG_SRV_FAILED ememRegSrvDataFailed ) >
#define pfEventLocalIDCB			boost::function< void( DWORD dwLocalID ) >

struct ServerEvent
{
	ServerEvent()
    {
        m_ServerType      = EM_SERVER_TYPE_END;
        m_dwServerLocalID = DF_NO_LOCALID;
    }

	EM_SERVER_TYPE			m_ServerType;				// Server類形
	DWORD					m_dwServerLocalID;			// 建立 Server 所指定的 Server 代號
	pfEventServerStatusCB	m_pfEventServerListChangeCB;
};

class CNet_ServerList_Child : public CNet_ServerList
{
public:
	//static	vector< ServerData* >	m_vecServerData;
	
	vector< ServerEvent* >			m_vecEventServerLogin;
	vector< ServerEvent* >			m_vecEventServerLogout;
	vector< ServerEvent* >			m_vecEventServerReady;


	//static	vector< vector< ServerData* >* > m_vecServerData;
	vector< ServerData* >			m_vecServerData[ EM_SERVER_TYPE_END ];

	vector< int	>					m_vecWaitingToSendServerData;	// 已經連線了. 但不確定其身份. 要等到 Master 允許 Login 再向其要求
	pfEventServerRegFailedCB		m_pfEventServerRegFailedCB;
	pfEventLocalIDCB				m_pfLocalID;
	//multimap< 

	//hash_multimap< EM_SERVER_TYPE , hash_map< DWORD , ServerData* > >	m_mhmapServerData;

	
	

	virtual	void	OnServer_Connect					( int iServerID, string SrvName );
	virtual	void	OnServer_Disconnect					( int iServerID );

	virtual	void	OnServer_RequestServerData			( int iServerID );
	virtual	void	OnServer_ResiterServerData			( int iServerID, EM_SERVER_TYPE ServerType, DWORD dwServerLocalID, bool bReg );
	virtual	void	OnServer_TransmitServerData			( int iServerID, EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	virtual	void	OnServer_RegSrvDataFailed			( int iServerID, EM_REG_SRV_FAILED emRegSrvDataFailed );
	virtual	void	OnServer_LocalID					( int iServerID, DWORD dwLocalID );
	virtual	void	OnServer_ServerReady				( int iServerID, EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );


	virtual	void	OnServerEvent_Shutdown				( int iServerID, int iCloseTime );
	virtual void	OnServerEvent_ServerReady			( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	virtual void	OnServerEvent_ServerLogin			( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	virtual void	OnServerEvent_ServerLogOut			( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	


    CNet_ServerList_Child( ) ;
    ~CNet_ServerList_Child(void);


//	static bool				Init( GSrvNetWaker* pNet, EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
//	static bool				Release();

	//CServerList_Child*	GetThis()		{ return m_pThis; }
	int					GetParallelZoneCount()			{ return int( m_vecServerData[EM_SERVER_TYPE_LOCAL].size() / _DEF_ZONE_BASE_COUNT_ +1 ); };

	bool				CheckServer					( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );

	bool				SendToServer				( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID, DWORD dwSize, PVOID pData );
	void				SendToServerByID			( DWORD dwServerID, DWORD dwSize, PVOID pData );
		// 傳送給特定類型的 Server, 但單指向其中一部指定的 ID

	bool				SendToServer				( EM_SERVER_TYPE ServerType, DWORD dwSize, PVOID pData ) { return SendToServer( ServerType, DF_NO_LOCALID, dwSize, pData ); }
		// 傳送給特定類型的 Server

	bool				CheckServerExist			( EM_SERVER_TYPE ServerType ) { if( m_vecServerData[ ServerType ].size() != 0 ) return true; else return false; }

	bool				SendToMaster				( DWORD dwSize, const PVOID pData ) { return SendToServer( EM_SERVER_TYPE_MASTER,		DF_NO_LOCALID, dwSize, pData ); }
	bool				SendToChat					( DWORD dwSize, const PVOID pData ) { return SendToServer( EM_SERVER_TYPE_CHAT,		DF_NO_LOCALID, dwSize, pData ); }
	bool				SendToDataCenter			( DWORD dwSize, const PVOID pData ) { return SendToServer( EM_SERVER_TYPE_DATACENTER, DF_NO_LOCALID, dwSize, pData ); }

    bool                SendToLogCenter             ( DWORD dwSize, const PVOID pData ) { return SendToServer( EM_SERVER_TYPE_LOGCENTER, DF_NO_LOCALID, dwSize, pData ); }

	//bool				SendToRoleDBCenter			( int PlayerCenterID , DWORD dwSize, const PVOID pData ) { return SendToServer( EM_SERVER_TYPE_ROLECENTER, DF_NO_LOCALID, dwSize, pData ); }
	bool				SendToRoleDBCenter			( int PlayerCenterID , DWORD dwSize, const PVOID pData ) { return SendToServer( EM_SERVER_TYPE_ROLECENTER, PlayerCenterID, dwSize, pData ); }
	bool				SendToPartition				( DWORD dwSize, const PVOID pData ) { return SendToServer( EM_SERVER_TYPE_PARTITION,	DF_NO_LOCALID, dwSize, pData ); }
	bool				SendToLocal					( DWORD dwServerLocalID, DWORD dwSize, const PVOID pData ) { return SendToServer( EM_SERVER_TYPE_LOCAL, dwServerLocalID, dwSize, (void*)pData ); }
    bool				SendToAllGMTools			( DWORD dwSize, const PVOID pData ) { return SendToServer( EM_SERVER_TYPE_GMTOOLS,		DF_NO_LOCALID, dwSize, pData ); }
	bool				SendToAllLocal				( DWORD dwSize, PVOID pData ) { return SendToServer( EM_SERVER_TYPE_LOCAL, DF_NO_LOCALID, dwSize, pData ); }

	void				ServerRdy					( int iServerID = -1, EM_SERVER_TYPE emServerType = m_ServerType, DWORD dwServerLocalID = m_dwServerLocalID );	// 藉由 Master 發給其它 Server 通知本 Server 已經準備好服務了

	void				RegServerLoginEvent			( EM_SERVER_TYPE ServerType, pfEventServerStatusCB pfun, DWORD dwServerLocalID = DF_NO_LOCALID );
	void				RegServerLogoutEvent		( EM_SERVER_TYPE ServerType, pfEventServerStatusCB pfun, DWORD dwServerLocalID = DF_NO_LOCALID );
	void				RegServerReadyEvent			( EM_SERVER_TYPE ServerType, pfEventServerStatusCB pfun, DWORD dwServerLocalID = DF_NO_LOCALID );
		// 註冊事件, 用來被通知 Server 新登入或登出


	void				RegSrvDataFailedEvent		( pfEventServerRegFailedCB pfun ) { m_pfEventServerRegFailedCB = pfun; }
	void				RegLocalID					( pfEventLocalIDCB pfun ) { m_pfLocalID = pfun; }

	ServerData*			GetSrvData					( DWORD dwSrvID );

	//----------------------------------------------------------------
	int						LocalID_To_SrvID			( int iLocalID );

    int	GMID_To_NetID( int iGMID )
    {
        // 找出所指定的 Server, 並將資料送出
        ServerData*						pServerData = NULL;

        bool bSend		= false;

        if( ( unsigned )iGMID >= m_vecServerData[ EM_SERVER_TYPE_GMTOOLS ].size() )
            return -1;

        pServerData = m_vecServerData[ EM_SERVER_TYPE_GMTOOLS ][ iGMID ];
        if( pServerData != NULL )
        {
            return pServerData->m_dwServerID;
        }

        return -1;
    }

    int                      SrvTypeToSrvID              ( EM_SERVER_TYPE ServerType );
    int						GetLocalServerSize			();
    vector< ServerData* >* 	GetServerGroupData			( EM_SERVER_TYPE ServerType ) { return &( m_vecServerData [ ServerType ] ); };

	void						ClEraserverList();

	vector< ServerData* >&	GetLocalList()				{ return m_vecServerData[ EM_SERVER_TYPE_LOCAL ]; };
};
