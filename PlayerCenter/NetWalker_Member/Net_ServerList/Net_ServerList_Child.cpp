#include ".\Net_ServerList_Child.h"	
#include "../../mainproc/Global.h"
#include "RemotetableInput/RemotableInput.h"
#pragma warning (disable:4949)
#pragma unmanaged
/*
//vector< ServerData* >	CNet_ServerList_Child::m_vecServerData;
vector< ServerData* >			CNet_ServerList_Child::m_vecServerData[ EM_SERVER_TYPE_END ];
vector< ServerEvent* >			CNet_ServerList_Child::m_vecEventServerLogin;
vector< ServerEvent* >			CNet_ServerList_Child::m_vecEventServerLogout;
pfEventServerRegFailedCB		CNet_ServerList_Child::m_pfEventServerRegFailedCB = NULL;
*/
char* SERVER_TYPE_STRING [] =
{
	{"MASTER"},
	{"LOCAL"},
	{"CHAT"},
	{"DATACENTER"},
	{"PARTITION"},
	{"GMTOOLS"},
	{"PLAYERCENTER"},
	{"UNKNOWTYPE 1"},
	{"UNKNOWTYPE 2"},
	{"UNKNOWTYPE 3"},
	{"UNKNOWTYPE 4"}
};

//-------------------------------------------------------------------
CNet_ServerList_Child::CNet_ServerList_Child( )
{
    m_pfEventServerRegFailedCB = NULL;
}
CNet_ServerList_Child::~CNet_ServerList_Child(void)	
{

    // 釋放掉 Server 資料
    ClEraserverList();

    {
        vector< ServerEvent* >::iterator it = m_vecEventServerLogin.begin(); 
        while( it != m_vecEventServerLogin.end() )
        {
            delete *it;
            it = m_vecEventServerLogin.erase( it );
        }
    }

    {
        vector< ServerEvent* >::iterator it = m_vecEventServerLogout.begin(); 
        while( it != m_vecEventServerLogout.end() )
        {
            delete *it;
            it = m_vecEventServerLogout.erase( it );
        }
    }

};
//-------------------------------------------------------------------
/*
bool    CNet_ServerList_Child::Init( GSrvNetWaker* pNet, EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	if( m_pThis == NULL)
	{
		Print( Def_PrintLV1, "CNet_ServerList_Child:Init(): \n		ServerList Working!! Waiting register from master. [ %s ][ %d ]", SERVER_TYPE_STRING[ ServerType ], dwServerLocalID );
		CNet_ServerList::_Init( pNet, ServerType, dwServerLocalID );

		m_pThis = NEW( CNet_ServerList_Child );
		return true;
	}
	else
	{
		return false;
	}
}
//-------------------------------------------------------------------
bool    CNet_ServerList_Child::Release()
{
	if( m_pThis == NULL )
		return false;

	// 釋放掉 Server 資料
	ClEraserverList();

	{
		vector< ServerEvent* >::iterator it = m_vecEventServerLogin.begin(); 
		while( it != m_vecEventServerLogin.end() )
		{
			delete *it;
			it = m_vecEventServerLogin.erase( it );
		}
	}

	{
		vector< ServerEvent* >::iterator it = m_vecEventServerLogout.begin(); 
		while( it != m_vecEventServerLogout.end() )
		{
			delete *it;
			it = m_vecEventServerLogout.erase( it );
		}
	}

	delete m_pThis;
	m_pThis = NULL;

	CNet_ServerList::_Release();
	return true;
}
*/
void CNet_ServerList_Child::OnServer_TransmitServerData ( int iServerID, EM_SERVER_TYPE emServerType, DWORD dwServerLocalID )
{
	bool IsExist = false;
	Print( Def_PrintLV1, "SL:OnServer_TransmitServerData" , "Receive other Server[ %s ][ %d ] data" ,SERVER_TYPE_STRING[emServerType], dwServerLocalID );


	OnServerEvent_ServerLogin( emServerType, dwServerLocalID );

	ServerData* pServerData			= NEW( ServerData);
	pServerData->m_dwServerID		= iServerID;
	pServerData->m_ServerType		= emServerType;
	pServerData->m_dwServerLocalID	= dwServerLocalID;

	// 如果加入的 Server 沒有 LocalID , 則加入 0 號位置, 否則的話照 LocalID 分配位置
	if( dwServerLocalID == DF_NO_LOCALID )
	{
		//if( m_vecServerData[ emServerType ].empty() != false )	
		for( vector< ServerData* >::iterator	it = m_vecServerData[ emServerType ].begin(); it != m_vecServerData[ emServerType ].end(); it++ )
		{
			if( (*it)->m_dwServerID == iServerID )
			{
				IsExist = true;
			}
		}
		
		if( IsExist == false )
			m_vecServerData[ emServerType ].push_back( pServerData );
	}
	else
	{
		DWORD dwServerSize = (DWORD)m_vecServerData[ emServerType ].size();
		if( dwServerSize <= dwServerLocalID )
		{
			DWORD dwOffset = dwServerLocalID - dwServerSize;
			while( dwOffset >= 1 )
			{
				m_vecServerData[ emServerType ].push_back( NULL );
				dwOffset--;
			}

			m_vecServerData[ emServerType ].push_back( pServerData );
		}
		else
		{
			//if( m_vecServerData[ emServerType ][ dwServerLocalID ] != NULL )
			//{
			//	Print( Def_PrintLV2, "CNet_ServerList_Child:OnServer_TransmitServerData" , "收到其它?Server資料, 但 m_vecServerData 中卻已有相同 Type(%d) LocID(%d) 的 Server(%d) 存在. 將不會通知其它 ServerList. 並通知那部 Server ID 重覆.", SERVER_TYPE_STRING[ emServerType ], dwServerLocalID );
			//}

			//delete m_vecServerData[ emServerType ][ dwServerLocalID ];
			//m_vecServerData[ emServerType ][ dwServerLocalID ] = pServerData;
			if( m_vecServerData[ emServerType ][ dwServerLocalID ] != NULL  ) 
			{
				IsExist = true;
				if( m_vecServerData[ emServerType ][ dwServerLocalID ]->m_dwServerID != pServerData->m_dwServerID  )
				{
					Print( Def_PrintLV2, "CNet_ServerList_Child:OnServer_TransmitServerData" , "收到其它?Server資料, 但 m_vecServerData 中卻已有相同 Type(%d) LocID(%d) 的 Server(%d) 存在. 將不會通知其它 ServerList. Server ID 不一置.", SERVER_TYPE_STRING[ emServerType ], dwServerLocalID );
				}
			}

			if( IsExist == false ) 
				m_vecServerData[ emServerType ][ dwServerLocalID ] = pServerData;

			// 12/20 決定不刪除舊的資料, 
		}
	}

	if( m_vecEventServerLogin.empty() == false && IsExist == false )
	{
		ServerEvent* pServerEvent = NULL;
		for( vector< ServerEvent* >::iterator it = m_vecEventServerLogin.begin(); it != m_vecEventServerLogin.end(); it++ )
		{	
			pServerEvent = *it;
			if( pServerEvent->m_ServerType == emServerType && ( pServerEvent->m_dwServerLocalID	== DF_NO_LOCALID || pServerEvent->m_dwServerLocalID == dwServerLocalID ) )
			{
				if( pServerData != NULL )
				{
					pServerEvent->m_pfEventServerListChangeCB( pServerData->m_ServerType, pServerData->m_dwServerLocalID );					
				}
			}		
		}

		
	}



}
//-------------------------------------------------------------------
void CNet_ServerList_Child::OnServer_ResiterServerData ( int iServerID, EM_SERVER_TYPE emServerType, DWORD dwServerLocalID, bool bReg )
{
	if( emServerType >= EM_SERVER_TYPE_END )
		Print( LV2, "SL:OnServer_ResiterServerData()" ,"Receive [ UNKNOW ][ %d ] ServerID[ %d ] Register!!", dwServerLocalID, iServerID );
	else
		Print( LV2, "SL:OnServer_ResiterServerData()" , "Receive [ %s ][ %d ] ServerID[ %d ] Register!!", SERVER_TYPE_STRING[ emServerType ], dwServerLocalID, iServerID );

	OnServerEvent_ServerLogin( emServerType, dwServerLocalID );

	ServerData* pServerData			= NEW( ServerData );
	pServerData->m_dwServerID		= iServerID;
	pServerData->m_ServerType		= emServerType;
	pServerData->m_dwServerLocalID	= dwServerLocalID;

	//m_vecServerData.push_back( pServerData );


	// 如果加入的 Server 沒有 LocalID , 則加入 0 號位置, 否則的話照 LocalID 分配位置
	if( dwServerLocalID == DF_NO_LOCALID )
	{
		//if( m_vecServerData[ emServerType ].empty() != false )
			m_vecServerData[ emServerType ].push_back( pServerData );
		//else
		//	RegSrvDataFailed( iServerID, EM_REG_SRV_FAILED_LOCALIDINUSE );
	}
	else
	{
		if( dwServerLocalID == DF_REQUEST_LOCALID )
		{
			DWORD dwID = (DWORD)m_vecServerData[ emServerType ].size();
			m_vecServerData[ emServerType ].push_back( pServerData );

			pServerData->m_dwServerLocalID	= dwID;

			// 將 LocalID 發送回要求的 Server
				SendLocalID( iServerID, dwID );
		}
		else
		{
			DWORD dwServerSize = (DWORD)m_vecServerData[ emServerType ].size();
			if( dwServerSize <= dwServerLocalID )
			{
				DWORD dwOffset = dwServerLocalID - dwServerSize;
				while( dwOffset >= 1 )
				{
					m_vecServerData[ emServerType ].push_back( NULL );
					dwOffset--;
				}

				m_vecServerData[ emServerType ].push_back( pServerData );
			}
			else
			{
				if( m_vecServerData[ emServerType ][ dwServerLocalID ] != NULL )
				{
					Print( Def_PrintLV5, "SL:OnServer_ResiterServerData" , "有 Server( %d ) 加入, 但 m_vecServerData 中卻已有相同 Type[ %s ][ %d ] 的 Server 存在", iServerID, SERVER_TYPE_STRING[ emServerType ], dwServerLocalID );
					RegSrvDataFailed( iServerID, EM_REG_SRV_FAILED_LOCALIDINUSE );
					return;
				}
				
				m_vecServerData[ emServerType ][ dwServerLocalID ] = pServerData;
			}
		}
	}

	ServerData* pServerTransmit = NULL;
	vector< ServerData* >::iterator	it;

	if( m_ServerType == EM_SERVER_TYPE_MASTER )
	{
		// 發送新註冊的 Server 資料給已在 List 的 Server
		for( int i = 0; i < EM_SERVER_TYPE_END; i++ )
		{
			for( it = m_vecServerData[i].begin(); it != m_vecServerData[i].end(); it++ )
			{
				pServerTransmit = *it;
				if( pServerTransmit != NULL )
				{
					// 通知在 ServerList 中所有 Server, 新加入的 Server 資料, 除了 Master 以外
					if( pServerTransmit->m_ServerType != EM_SERVER_TYPE_MASTER )
					{
						TransmitServerData( pServerTransmit->m_dwServerID, iServerID, pServerData->m_ServerType, pServerData->m_dwServerLocalID );
					}
				}
			}
		}

		// 將所有 ServerList 資料發送給新加入的 Server
		for( int i = 0; i < EM_SERVER_TYPE_END; i++ )
		{
			for( it = m_vecServerData[i].begin(); it != m_vecServerData[i].end(); it++ )
			{
				pServerTransmit = *it;
				if( pServerTransmit != NULL )
				{
					// 告知新加入的 Server 其它 Server 的資料
					if( iServerID != pServerTransmit->m_dwServerID &&
						emServerType != EM_SERVER_TYPE_MASTER )
					{
						TransmitServerData( iServerID, pServerTransmit->m_dwServerID, pServerTransmit->m_ServerType, pServerTransmit->m_dwServerLocalID );

						if( pServerTransmit->m_bServerRdy != false )
						{
							ServerRdy( iServerID, pServerTransmit->m_ServerType, pServerTransmit->m_dwServerLocalID );

							//
							//g_remotableInput.OutputStartupMessage();
						}
					}
				}
			}
		}

	}


	if( m_vecEventServerLogin.empty() == false )
	{
		ServerEvent* pServerEvent = NULL;
		for( vector< ServerEvent* >::iterator it = m_vecEventServerLogin.begin(); it != m_vecEventServerLogin.end(); it++ )
		{	
			pServerEvent = *it;
			if( pServerEvent->m_ServerType == emServerType && ( pServerEvent->m_dwServerLocalID	== DF_NO_LOCALID || pServerEvent->m_dwServerLocalID == dwServerLocalID ) )
			{
				if( pServerData != NULL )
					pServerEvent->m_pfEventServerListChangeCB( pServerData->m_ServerType, pServerData->m_dwServerLocalID );
			}
		}
	}

	

	//hash_multimap< EM_SERVER_TYPE , hash_map< DWORD , ServerData* > >	m_mhmapServerData;
}
//-------------------------------------------------------------------
void	CNet_ServerList_Child::OnServer_Disconnect		(  int iServerID )
{
	// 檢查現有資料庫資料是否存在
	vector< ServerData* >::iterator	it;
	ServerData*						pServerData = NULL;

	bool	bFound = false;

	for( int i = 0; i < EM_SERVER_TYPE_END; i++ )
	{
		for( it = m_vecServerData[i].begin(); it != m_vecServerData[i].end(); it++ )
		{
			pServerData = *it;
			if( pServerData != NULL && pServerData->m_dwServerID == iServerID )
			{

				OnServerEvent_ServerLogOut( pServerData->m_ServerType, pServerData->m_dwServerLocalID );

				// 檢查該 Server 有無註冊 Logout Message
				if( m_vecEventServerLogout.empty() == false )
				{
					ServerEvent* pServerEvent = NULL;
					for( vector< ServerEvent* >::iterator it = m_vecEventServerLogout.begin(); it != m_vecEventServerLogout.end(); it++ )
					{	
						pServerEvent = *it;
						if( pServerEvent->m_ServerType == pServerData->m_ServerType && ( pServerEvent->m_dwServerLocalID	== DF_NO_LOCALID || pServerEvent->m_dwServerLocalID == pServerData->m_dwServerLocalID ) )
						{
							if( pServerData != NULL )
							{
								pServerEvent->m_pfEventServerListChangeCB( pServerData->m_ServerType, pServerData->m_dwServerLocalID );
								
							}
						}
					}
				}

				if( pServerData->m_ServerType == EM_SERVER_TYPE_MASTER )
				{
					//Print( Def_PrintLV1, "SL:OnServer_Disconnect()" ," MASTER SERVER DISCONNECT!! CLEAR SERVER LIST!!");
					Print( Def_PrintLV1, "SL:OnServer_Disconnect()" ," MASTER SERVER DISCONNECT!!");
					//ClEraserverList();
					//return;
				}
				else
				{
					Print( Def_PrintLV2, "SL:OnServer_Disconnect()" , "Server[ %s ][ %d ] disconnect!!" ,SERVER_TYPE_STRING[pServerData->m_ServerType], pServerData->m_dwServerLocalID );
				}

				bFound = true;				
				
				// 找到了, 清除該筆資料
				if( pServerData->m_dwServerLocalID == DF_NO_LOCALID )
				{
					m_vecServerData[i].erase( it );					
				}
				else
				{
					*it = NULL;
				}

				delete pServerData;

				while( m_vecServerData[i].size() != 0 )
				{
					if( m_vecServerData[i].back() == NULL )
						m_vecServerData[i].pop_back();
					else
						break;
				}

				return;
			}

		}
	}



	if( bFound == false )
	{
		// 有 Server 離開, 但資料中卻找不到該 Server , 表系統環節中出錯
		Print( Def_PrintLV1, "CNet_ServerList_Child:OnServer_Disconnect" , "Server disconnect, but it cant be found in m_vecServerData. ServerID(%d)!",iServerID );
	}

/*	
	for( it = m_vecServerData.begin(); it != m_vecServerData.end(); it++ )
	{
		pServerData = *it;
		if( pServerData->m_dwServerID == dwServerID )
		{

			// 檢查該 Server 有無註冊 Logout Message
			if( m_vecEventServerLogout.empty() == false )
			{
				ServerEvent* pServerEvent = NULL;
				for( vector< ServerEvent* >::iterator it = m_vecEventServerLogout.begin(); it != m_vecEventServerLogout.end(); it++ )
				{	
					pServerEvent = *it;
					if( pServerEvent->m_ServerType == pServerData->m_ServerType && ( pServerEvent->m_dwServerLocalID	== 0 || pServerEvent->m_dwServerLocalID == pServerData->m_dwServerLocalID ) )
					{
						pServerEvent->m_pfEventServerListChangeCB( pServerData->m_ServerType, pServerData->m_dwServerLocalID );
					}
				}
			}

			// 找到了, 清除該筆資料
			delete pServerData;
			m_vecServerData.erase( it );
			return;
		}
	}

	if( it == m_vecServerData.end() )
	{
		// 有 Server 離開, 但資料中卻找不到該 Server , 表系統環節中出錯
		Print( Def_PrintLV5, "CNet_ServerList_Child:OnServer_Del(): 有 Server 離開, 但 m_vecServerData 中卻找不到該 Server(%d) , 系統環節中出錯",dwServerID );
	}
*/
}
//-------------------------------------------------------------------
void CNet_ServerList_Child::SendToServerByID ( DWORD dwServerID, DWORD dwSize, PVOID pData )
{
	m_pGlobal->Net()->SendToSrv( dwServerID, dwSize, pData );
}
//-------------------------------------------------------------------
bool CNet_ServerList_Child::SendToServer	( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID, DWORD dwSize, PVOID pData )
{
	// 找出所指定的 Server, 並將資料送出
	ServerData*						pServerData = NULL;
	vector< ServerData* >::iterator	it;

	bool bSend		= false;

	if( dwServerLocalID != -1 )
	{
		if( dwServerLocalID >= m_vecServerData[ ServerType ].size() )
		{
			Print( Def_PrintLV1, "CNet_ServerList_Child:SendToServer" , "Cant find server [ %s ][ %d ]",SERVER_TYPE_STRING[ ServerType ], dwServerLocalID );
			return false;
		}

		pServerData = m_vecServerData[ ServerType ][ dwServerLocalID ];
		if( pServerData != NULL )
		{
            m_pGlobal->Net()->SendToSrv( pServerData->m_dwServerID, dwSize, pData );
			bSend	= true;
		}
		else
		{
			Print( Def_PrintLV1, "CNet_ServerList_Child:SendToServer" , "Cant find server [ %s ][ %d ]",SERVER_TYPE_STRING[ ServerType ], dwServerLocalID );
			return false;
		}
	}
	else
	{
        
		for( it = m_vecServerData[ ServerType ].begin(); it != m_vecServerData[ ServerType ].end(); it++ )
		{
			pServerData = *it;
			if( pServerData != NULL )
			{
				m_pGlobal->Net()->SendToSrv( pServerData->m_dwServerID, dwSize, pData );
				bSend = true;
			}
		}
        
	}

	if( bSend == false )
	{
		Print( Def_PrintLV1, "CNet_ServerList_Child:SendToServer" , "Cant find server [ %s ][ %d ]",SERVER_TYPE_STRING[ ServerType ], dwServerLocalID );
	}



	/*
	for( it = m_vecServerData.begin(); it != m_vecServerData.end(); it++ )
	{
		pServerData = *it;
		if( pServerData->m_ServerType == ServerType )
		{
			if( dwServerLocalID == DF_NO_LOCALID || ( pServerData->m_dwServerLocalID == dwServerLocalID ) )
			{
				m_pGlobal->Net()->SendToSrv( pServerData->m_dwServerID, dwSize, pData );
				bReturn = true;
			}
		}
	}
	*/

	return bSend;
}


void CNet_ServerList_Child::RegServerLoginEvent		( EM_SERVER_TYPE ServerType, pfEventServerStatusCB pfun, DWORD dwServerLocalID  )
{
	ServerEvent* pServerEvent					= NEW( ServerEvent );

	pServerEvent->m_ServerType					= ServerType;
	pServerEvent->m_dwServerLocalID				= dwServerLocalID;
	pServerEvent->m_pfEventServerListChangeCB	= pfun;
	
	m_vecEventServerLogin.push_back( pServerEvent );
}

void CNet_ServerList_Child::RegServerLogoutEvent	( EM_SERVER_TYPE ServerType, pfEventServerStatusCB pfun, DWORD dwServerLocalID  )
{
	ServerEvent* pServerEvent					= NEW( ServerEvent );

	pServerEvent->m_ServerType					= ServerType;
	pServerEvent->m_dwServerLocalID				= dwServerLocalID;
	pServerEvent->m_pfEventServerListChangeCB	= pfun;

	m_vecEventServerLogout.push_back( pServerEvent );
}

void CNet_ServerList_Child::RegServerReadyEvent	( EM_SERVER_TYPE ServerType, pfEventServerStatusCB pfun, DWORD dwServerLocalID  )
{
	ServerEvent* pServerEvent					= NEW( ServerEvent );

	pServerEvent->m_ServerType					= ServerType;
	pServerEvent->m_dwServerLocalID				= dwServerLocalID;
	pServerEvent->m_pfEventServerListChangeCB	= pfun;

	m_vecEventServerReady.push_back( pServerEvent );
}

int	CNet_ServerList_Child::LocalID_To_SrvID( int iLocalID )
{
    // 找出所指定的 Server, 並將資料送出
    ServerData*						pServerData = NULL;

    bool bSend		= false;

    if( ( unsigned )iLocalID > m_vecServerData[ EM_SERVER_TYPE_LOCAL ].size() )
        return -1;

    pServerData = m_vecServerData[ EM_SERVER_TYPE_LOCAL ][ iLocalID ];
    if( pServerData != NULL )
    {
        return pServerData->m_dwServerID;
    }

	return -1;
}

int CNet_ServerList_Child::SrvTypeToSrvID( EM_SERVER_TYPE ServerType )
{
    ServerData*	pServerData = NULL;
    if( EM_SERVER_TYPE_END <= ServerType )
        return -1;
    if( m_vecServerData[ ServerType ].empty() )
        return -1;

    pServerData = *(m_vecServerData[ ServerType ].begin());
    return pServerData->m_dwServerID;
}

int	CNet_ServerList_Child::GetLocalServerSize()
{
	int iSize = 0;

	vector< ServerData* >::iterator	it;
	for( it = m_vecServerData[ EM_SERVER_TYPE_LOCAL ].begin(); it != m_vecServerData[ EM_SERVER_TYPE_LOCAL ].end(); it++ )
	{
		if( *it != NULL )
		{
			iSize++;
		}
	}
	return iSize;
}

void CNet_ServerList_Child::OnServer_Connect		(  int iServerID, string SrvName )
{
	PG_SL_SL2SL_RequestServerData PacketToServer;
	
	// 只有 Master 會主動詢問其它 Server 的 資料, 收到後才會通知其它 Server 出理
	if( m_ServerType == EM_SERVER_TYPE_MASTER )
	{
		m_pGlobal->Net()->SendToSrv( iServerID, sizeof( PG_SL_SL2SL_RequestServerData ), &PacketToServer );
		//Server Ready
	}
}

void CNet_ServerList_Child::OnServer_RequestServerData	( int iServerID )
{
	RegisterServerData( iServerID, m_ServerType, m_dwServerLocalID, m_bRegister );
	m_bRegister = true;
}

void CNet_ServerList_Child::OnServer_RegSrvDataFailed				( int iServerID, EM_REG_SRV_FAILED emRegSrvDataFailed )
{
	if( m_pfEventServerRegFailedCB != NULL )
		m_pfEventServerRegFailedCB( emRegSrvDataFailed );

}

void CNet_ServerList_Child::ClEraserverList()
{
	vector< ServerData* >::iterator	it;
	for( int i = 0; i < EM_SERVER_TYPE_END; i++ )
	{
		it = m_vecServerData[i].begin();
		while( it != m_vecServerData[i].end() )
		{
			if( *it != NULL )
			{
				delete *it;
				*it	= NULL;
				it	= m_vecServerData[i].erase( it );
			}
			else
				it++;
		}
	}
}
void CNet_ServerList_Child::OnServer_LocalID ( int iServerID, DWORD dwLocalID )
{
	m_dwServerLocalID	= dwLocalID;
}

ServerData*	CNet_ServerList_Child::GetSrvData ( DWORD dwSrvID )
{
	vector< ServerData* >::iterator	it;
	ServerData*	pSrvData = NULL;
	for( int i = 0; i < EM_SERVER_TYPE_END; i++ )
	{
		it = m_vecServerData[i].begin();
		while( it != m_vecServerData[i].end() )
		{
			if( *it != NULL )
			{
				pSrvData = *it;

				if( pSrvData->m_dwServerID == dwSrvID )
					return pSrvData;
                else
                    it++;
			}
			else
				it++;
		}
	}

	return NULL;
}

void CNet_ServerList_Child::ServerRdy( int iServerID, EM_SERVER_TYPE emServerType, DWORD dwServerLocalID )
{
	PG_SL_SL2SL_ServerReady Packet;

	Packet.m_ServerType			= emServerType;
	Packet.m_dwServerLocalID	= dwServerLocalID;

	if( iServerID == -1 )
	{
		SendToServer( EM_SERVER_TYPE_MASTER,		DF_NO_LOCALID, sizeof( Packet ), &Packet );
	}
	else
		m_pGlobal->Net()->SendToSrv( iServerID, sizeof( Packet ), &Packet );

	// 將自己設為 Server Okay
	ServerData*							pServerData = NULL;
	vector< ServerData* >::iterator		it;

	for( int i = 0; i < EM_SERVER_TYPE_END; i++ )
	{
		for( it = m_vecServerData[i].begin(); it != m_vecServerData[i].end(); it++ )
		{
			pServerData = *it;
			if( pServerData != NULL )
			{
				if( pServerData->m_ServerType == emServerType && pServerData->m_dwServerLocalID == dwServerLocalID )
				{
					pServerData->m_bServerRdy = true;
				}
			}
		}
	}	

}

void CNet_ServerList_Child::OnServer_ServerReady ( int iServerID, EM_SERVER_TYPE emServerType, DWORD dwServerLocalID )
{
	ServerData*						pServerData = NULL;
	vector< ServerData* >::iterator	it;
	

	// 記錄該 Server 準備好的訊息為真

		if( m_ServerType == EM_SERVER_TYPE_MASTER )
		{
			// 轉送給其它 Server
			// 發送新註冊的 Server 資料給已在 List 的 Server
		
			for( int i = 0; i < EM_SERVER_TYPE_END; i++ )
			{
				for( it = m_vecServerData[i].begin(); it != m_vecServerData[i].end(); it++ )
				{
					pServerData = *it;
					if( pServerData != NULL )
					{
						// 通知在 ServerList 中所有 Server, 新加入的 Server 資料, 除了 Master 以外
						if( pServerData->m_ServerType != EM_SERVER_TYPE_MASTER && iServerID != pServerData->m_dwServerID )
						{
							ServerRdy( pServerData->m_dwServerID, emServerType, dwServerLocalID );
						}
					}
				}
			}
		}

		OnServerEvent_ServerReady( emServerType, dwServerLocalID );

	// 檢查有無註冊事件
		for( int i = 0; i < EM_SERVER_TYPE_END; i++ )
		{
			for( it = m_vecServerData[i].begin(); it != m_vecServerData[i].end(); it++ )
			{
				pServerData = *it;
				if( pServerData != NULL )
				{
					if( pServerData->m_ServerType == emServerType && pServerData->m_dwServerLocalID == dwServerLocalID )
					{
						
						ServerEvent* pServerEvent = NULL;
						if( pServerData->m_bServerRdy == false )
						{
							for( vector< ServerEvent* >::iterator it = m_vecEventServerReady.begin(); it != m_vecEventServerReady.end(); it++ )
							{	
								pServerEvent = *it;
								if( pServerEvent->m_ServerType == emServerType && ( pServerEvent->m_dwServerLocalID	== DF_NO_LOCALID || pServerEvent->m_dwServerLocalID == dwServerLocalID ) )
								{
									pServerEvent->m_pfEventServerListChangeCB( emServerType, dwServerLocalID );

								}
							}
							
							
						}
						pServerData->m_bServerRdy = true;
					}
				}
			}
		}	

}

bool CNet_ServerList_Child::CheckServer( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	// 找出所指定的 Server, 並將資料送出
	ServerData*						pServerData = NULL;
	vector< ServerData* >::iterator	it;

	bool bIsExist		= false;

	if( dwServerLocalID != -1 )
	{
		if( dwServerLocalID >= m_vecServerData[ ServerType ].size() )
		{
			Print( Def_PrintLV1, "CNet_ServerList_Child:SendToServer" , "Cant find server [ %s ][ %d ]",SERVER_TYPE_STRING[ ServerType ], dwServerLocalID );
			return false;
		}

		pServerData = m_vecServerData[ ServerType ][ dwServerLocalID ];
		if( pServerData != NULL )
		{
			bIsExist	= true;
		}
		else
		{
			return false;
		}
	}
	else
	{

		for( it = m_vecServerData[ ServerType ].begin(); it != m_vecServerData[ ServerType ].end(); it++ )
		{
			pServerData = *it;
			if( pServerData != NULL )
			{
				bIsExist = true;
			}
		}

	}

	return bIsExist;
}

#pragma managed