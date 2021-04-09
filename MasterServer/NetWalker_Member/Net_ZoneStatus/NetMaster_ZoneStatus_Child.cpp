#include "../Net_DCBase/Net_DCBaseChild.h"
#include "../Net_ServerList/Net_ServerList_Child.h"
#include "NetMaster_ZoneStatus_Child.h"

#include "../Net_login/NetSrv_Master_Login_Child.h"
#include "../net_channelbase/Net_OnlinePlayerChild.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"

#include "../../mainproc/ClientInfo/ClientInfo.h"

CNetMaster_ZoneStatus_Child*	CNetMaster_ZoneStatus_Child::m_pThis;
bool							CNetMaster_ZoneStatus_Child::m_bInitDone = false;

//-------------------------------------------------------------------
CNetMaster_ZoneStatus_Child::CNetMaster_ZoneStatus_Child(void)
{
	m_iShutdownMin = 10000;
}
//-------------------------------------------------------------------
CNetMaster_ZoneStatus_Child::~CNetMaster_ZoneStatus_Child(void)
{

}

//-------------------------------------------------------------------
bool    CNetMaster_ZoneStatus_Child::Init()
{
	CNetMaster_ZoneStatus_Child::_Init();

	if( m_pThis != NULL)
		return false;

	CNetMaster_ZoneStatus::m_pThis = m_pThis = NEW( CNetMaster_ZoneStatus_Child );

	Global::Net_ServerList->RegServerLoginEvent		( EM_SERVER_TYPE_LOCAL,		OnEvent_ZoneSrvConnected,	DF_NO_LOCALID ); 
	Global::Net_ServerList->RegServerLoginEvent		( EM_SERVER_TYPE_NONAGE,	OnEvent_NonageSrvConnected,	DF_NO_LOCALID );

	Global::Net_ServerList->RegServerLogoutEvent	( EM_SERVER_TYPE_LOCAL,		OnEvent_ZoneSrvDisconnect,	DF_NO_LOCALID ); 

	Schedular()->Push( OnEvent_CheckRegStatus, DF_TIME_ZONESRV_REGTIMEOUT, NULL, NULL );

	Print( Def_PrintLV1, "CNetMaster_ZoneStatus_Child:Init" , "等待( %0.2f )秒鐘. 讓 LocalServer 發送註冊訊息!!", ((float)DF_TIME_ZONESRV_REGTIMEOUT / 1000 ) );

	return true;
}

//-------------------------------------------------------------------
bool    CNetMaster_ZoneStatus_Child::Release()
{
	if( m_pThis == NULL )
		return false;

	delete m_pThis;
	m_pThis = NULL;
	
	CNetMaster_ZoneStatus_Child::_Release();
	return true;
}
//-------------------------------------------------------------------
int CNetMaster_ZoneStatus_Child::GetFreeZoneItemID()
{
	int iLastID = -1;
	set< int >::iterator it = m_setUsedZoneItemID.begin();
	while( it != m_setUsedZoneItemID.end() )
	{
		if( iLastID != -1 && (iLastID+1) != *it )
		{
			// 將中間跳號的部份發出去
			//return (iLastID+1);
			break;
		}
		iLastID = *it;
		it++;
	}

	int iZoneItemID =  (iLastID+1);
	
	if( iZoneItemID > 2400 )
	{
		Print( Def_PrintLV4, "CNetMaster_ZoneStatus_Child:GetFreeZoneItemID" ,"ZoneItemID 2400 配發完了!!" );
		return -1;
	}
	else
	{
		m_setUsedZoneItemID.insert( iZoneItemID );
		return iZoneItemID;
	}
}

//-------------------------------------------------------------------
void CNetMaster_ZoneStatus_Child::On_ZoneSrvReg				(  int iServerID, int iZoneID, int iZoneItemID, int iRemainCounter )
{
	// 檢查 ServerList 中 Local Server 清單中與註冊的數量有無相同, 一直要到 ServerList 中 LocalServer 數量和 
	// 收到要求註冊的 Local Server 數量一樣才可以開始配發 ZoneItem ID

	LocalServerStatus* pSrvStatus = NULL;

	map< int , LocalServerStatus* >::iterator it;
	if( ( it = m_mapZoneSrvStatus.find( iZoneID ) ) == m_mapZoneSrvStatus.end() )
	{
		Print( LV1, "CNetMaster_ZoneStatus_Child:On_ZoneSrvReg" , "ZoneServer( %d )找不到 Server 資料", iZoneID );
	}
	else
	{
		pSrvStatus = it->second;
	}

	
	if( iZoneItemID == DF_NO_ZONEITEMID )
		Print( LV1, "CNetMaster_ZoneStatus_Child:On_ZoneSrvReg" , "收到 ZoneSrv[ %d ] 註冊. 等待配發 ZoneItemID", pSrvStatus->iLocalID );
	else
		Print( LV1, "CNetMaster_ZoneStatus_Child:On_ZoneSrvReg" , "收到 ZoneSrv[ %d ] 註冊. 通知 Master 它使用 ZoneItemID[ %d ]中. 還有[ %d ] ID 等待註冊!!", pSrvStatus->iLocalID, iZoneItemID ,iRemainCounter );

	// 檢查 ZoneServer 是否要重新註冊所使用的 ZoneItemID

	if( iZoneItemID != DF_NO_ZONEITEMID )
	{
		if( m_bInitDone == false )
		{
			m_setUsedZoneItemID.insert( iZoneItemID );

			if( iRemainCounter == 0 )
			{
				pSrvStatus->emStatus = EM_LSRV_STATUS_WAITINGLSRV;

				// 通知 Zone Server 已收到全部的 ID, 你可以正確工作了
				ZoneSrvRegResult( pSrvStatus->iServerID, EM_ZoneSrvRegResult_OK, DF_NO_ZONEITEMID );
			}
		}
		else
		{			
			// MasterServer 已經開始運作了. 確仍收到 ZoneServer 發來的註冊封包
			Print( LV2, "CNetMaster_ZoneStatus_Child:On_ZoneSrvReg", "MasterServer 已經開始運作了. 確仍收到 ZoneServer( %d ) 發來的註冊封包", iZoneID );

			// How to do? Close ZoneServer?
			// 發封包通知該部 Server, 且不會將它加入群組中
			ZoneSrvRegResult( iServerID, EM_ZoneSrvRegResult_MasterSrvInitDone, iZoneID );
		}
	}
	else
	{
		if( m_bInitDone == false )
			pSrvStatus->emStatus = EM_LSRV_STATUS_WAITINGREG; // 等待 Master 配 ID
		else
		{
			// MasterServer 已初始化完畢!! 可以配發 ID
			int iZoneItemID = GetFreeZoneItemID();
			if( iZoneItemID != -1 )
			{
				// 配送 ZoneItemID , 並將之狀態設成等待 OK 確認
				Print( LV1, "CNetMaster_ZoneStatus_Child:OnEvent_CheckRegStatus" , "配發 ZoneItemID( %d ) 給 ZoneSrv( %d )!!",  iZoneItemID, pSrvStatus->iLocalID );
				ZoneSrvRegResult( pSrvStatus->iServerID, EM_ZoneSrvRegResult_OK, iZoneItemID );
				pSrvStatus->emStatus = EM_LSRV_STATUS_WAITINGLSRV;
			}
			else
			{
				Print( LV3, "CNetMaster_ZoneStatus_Child:OnEvent_CheckRegStatus" , "配發 ZoneItemID 時, 發現已分配超過上限!! 通知 ZoneSrv( %d )" ,pSrvStatus->iLocalID );
				ZoneSrvRegResult( pSrvStatus->iServerID, EM_ZoneSrvRegResult_OUT_ZONEITEMID, iZoneItemID );
			}
		}
	}

	//CheckRegStatus();

	/*
	if( m_mapZoneSrvStatus.find( iServerID ) == m_mapZoneSrvStatus.end() )
	{
		if( iZoneItemID == -1 )
		{
			m_mapZoneSrvStatus[ iServerID ] = EM_WAITINGREG;
		}
		else
		{
			m_mapZoneSrvStatus[ iServerID ] = EM_INIT;
		}
	}
	else
	{
		if( iZoneItemID != -1 )
		{
			Print( Def_PrintLV4, "CNetMaster_ZoneStatus_Child:On_ZoneSrvReg(): ZoneSrv(%d) Reg, 但確發現之前已註冊過, 且 iZoneItemID(%d) != -1."iServerID,iZoneItemID );
		}
	}

	if( iZoneItemID != -1 )
	{
		m_setUsedZoneItemID.insert( iZoneItemID );
	}

	if( ((int)m_mapZoneSrvStatus.size()) == iLocalServerSize )
	{
		// 表所有 Local Server 已要求註冊或註冊完畢了, 開始對要求註冊的 Srv 配送 ZoneItemID
		map< int , EM_LOCALSERVER_STATUS >::iterator it;
		for( it = m_mapZoneSrvStatus.begin(); it != m_mapZoneSrvStatus.end(); it++ )
		{
			if( it->second == EM_WAITINGREG )
			{
				int iZoneItemID = GetFreeZoneItemID();
				if( iZoneItemID != -1 )
					ZoneSrvRegResult( iServerID, EM_ZoneSrvRegResult_OK, iZoneItemID );
				else
					ZoneSrvRegResult( iServerID, EM_ZoneSrvRegResult_OutofZoneItemID, iZoneItemID );
			}
		}
	}
	*/

	
}
//-------------------------------------------------------------------
void CNetMaster_ZoneStatus_Child::OnEvent_NonageSrvConnected( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{

	// 將所有線上人員資料, 發送給 NonageServer
	for( vector< ServerData* >::iterator itServer = Global::Net_ServerList->m_vecServerData[ EM_SERVER_TYPE_NONAGE ].begin(); itServer != Global::Net_ServerList->m_vecServerData[ EM_SERVER_TYPE_NONAGE ].end(); itServer++ )
	{
		ServerData* pServer = *itServer;

		if( dwServerLocalID == pServer->m_dwServerLocalID )
		{
			for( map< string,	ClientInfo*	>::iterator it = g_pClientManger->m_mapAccount.begin(); it != g_pClientManger->m_mapAccount.end(); it++ )
			{
				int		iClientID	= it->second->m_iClientID;
				string	account		= it->first;


				PG_SL_M2S_AccountEnterWorld Packet;

				Packet.iClientID	= iClientID;
				Packet.Account		= account.c_str();

				Global::Net_ServerList->SendToServerByID( pServer->m_dwServerID, sizeof(Packet), &Packet );
			}
		}
	}	
}
//-------------------------------------------------------------------
void CNetMaster_ZoneStatus_Child::OnEvent_ZoneSrvConnected( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	// 發出封包要求 Local Server 註冊


	if( m_pThis->m_mapZoneSrvStatus.find( dwServerLocalID ) == m_pThis->m_mapZoneSrvStatus.end() )
	{
		LocalServerStatus*				pSrvStatus	= NEW( LocalServerStatus );

		pSrvStatus->emStatus						= EM_LSRV_STATUS_SENDINGREG;
		pSrvStatus->iLocalID						= (int)dwServerLocalID;
		pSrvStatus->iServerID						= Global::Net_ServerList->LocalID_To_SrvID( pSrvStatus->iLocalID );
		//pSrvStatus->iTimeEventID_ZoneSrvReg		= FunctionSchedularClass::Push( OnEvent_LocalServerRegTimeOut, DF_TIME_ZoneSrvREGTIMEOUT, (PVOID)pSrvStatus, NULL );

		m_pThis->m_mapZoneSrvStatus[ dwServerLocalID ] = pSrvStatus;

		// 要求 ZoneSrv  註冊
		RequestZoneSrvReg( pSrvStatus->iServerID );
	}
	else
	{
		Print( Def_PrintLV2, "CNetMaster_ZoneStatus_Child:OnEvent_LocalServerConnected" , "發現使用相同 LocalID(%d) Server登入!!", dwServerLocalID );
	}
}
//-------------------------------------------------------------------
/*
int	CNetMaster_ZoneStatus_Child::OnEvent_LocalServerRegTimeOut	( SchedularInfo* pInfo , PVOID pContext )
{
	// 通知該 LocalServer 關掉, 並清掉在 ServerList 的資料
	LocalServerStatus* pSrvStatus	= (LocalServerStatus*)pContext;
	_Net->SrvLogout( pSrvStatus->iServerID );

	return 0;
}
*/
//-------------------------------------------------------------------
void CNetMaster_ZoneStatus_Child::OnEvent_ZoneSrvDisconnect	( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	//int iServerID = Global::NetServerList->GetLocalServerID( dwServerLocalID );
	
	LocalServerStatus* pSrvStatus = NULL;
	map< int , LocalServerStatus* >::iterator it;
	if( ( it = m_pThis->m_mapZoneSrvStatus.find( dwServerLocalID ) ) != m_pThis->m_mapZoneSrvStatus.end() )
	{
		// 刪除斷線 Server 的角色
		map< int ,		ClientInfo* >::iterator itClientInfo;
		ClientInfo* pClientInfo = NULL;
		vector< ClientInfo* > vecClientInfo;

		for( itClientInfo = g_pClientManger->m_mapClientInfo.begin(); itClientInfo != g_pClientManger->m_mapClientInfo.end(); itClientInfo++ )
		{
			pClientInfo = itClientInfo->second;

			if( pClientInfo->m_setZone.find( dwServerLocalID ) != pClientInfo->m_setZone.end() )
			{
				vecClientInfo.push_back( pClientInfo );
			}
		}

		for( vector< ClientInfo* >::iterator itErase = vecClientInfo.begin(); itErase != vecClientInfo.end(); itErase++ )
		{
			pClientInfo = *itErase;
			pClientInfo->m_setZone.erase( dwServerLocalID );

			// 利用斷線機制來處理角色
			_Net->CliLogout( pClientInfo->m_iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "Master Zone Server Disconnect" );	// 強制將 Client Logout
		}



		pSrvStatus = it->second;
		m_pThis->m_mapZoneSrvStatus.erase( pSrvStatus->iLocalID );
		delete pSrvStatus;
	}
	else
	{
		// 在 Reg Server 中找不到
		Print( Def_PrintLV2, "CNetMaster_ZoneStatus_Child:OnEvent_LocalServerDisconnect" , "在ZoneServer連線清單中找不到離開的 ZoneServer(%d) 資料!!", dwServerLocalID );
	}
	//m_pThis->CheckRegStatus();
}
/*
//-------------------------------------------------------------------
void	CNetMaster_ZoneStatus_Child::CheckRegStatus()
{
	// 檢查還有沒有 Server 還是處於 EM_LSRV_STATUS_INIT 狀態, 還有的話則不能配送 ZoneItemID
	bool				bCheck_LSrv_Init	= false;
	LocalServerStatus*	pSrvStatus			= NULL;

	// MasterServer 還在初始化
	if( m_bInit == false )
		return;

	map< int , LocalServerStatus* >::iterator it;
	for( it = m_mapZoneSrvStatus.begin(); it != m_mapZoneSrvStatus.end(); it++ )
	{
		pSrvStatus = it->second;
		if( pSrvStatus->emStatus == EM_LSRV_STATUS_SENDINGREG )
			return;
	}

	for( it = m_mapZoneSrvStatus.begin(); it != m_mapZoneSrvStatus.end(); it++ )
	{
		pSrvStatus = it->second;
		if( pSrvStatus->emStatus == EM_LSRV_STATUS_WAITINGREG )
		{
			int iZoneItemID = GetFreeZoneItemID();
			
			if( iZoneItemID != -1 )
			{
				// 配送 ZoneItemID , 並將之狀態設成等待 OK 確認
				ZoneSrvRegResult( pSrvStatus->iServerID, EM_ZoneSrvRegResult_OK, iZoneItemID );
				pSrvStatus->emStatus = EM_LSRV_STATUS_WAITINGLSRV;
			}
			else
				ZoneSrvRegResult( pSrvStatus->iServerID, EM_ZoneSrvRegResult_OUT_ZONEITEMID, iZoneItemID );
		}
	}
}
*/
//-------------------------------------------------------------------
int		CNetMaster_ZoneStatus_Child::OnEvent_CheckRegStatus			( SchedularInfo* pInfo , PVOID pContext )
{
	// 只有 Master 剛開, 還在初始化的時候才會執行這事件. 用來等待所有 Server 狀態
	Print( Def_PrintLV2, "CNetMaster_ZoneStatus_Child:OnEvent_CheckRegStatus" , "初始等待時間到! 與 ZoneServer 協調 ZoneItemID" );
	
	LocalServerStatus*							pSrvStatus			= NULL;
	map< int , LocalServerStatus* >::iterator	it;


	// 印出目前所有 Zone 的狀態
	vector< ServerData* >& LocalList = Net_ServerList->GetLocalList();

	for( vector< ServerData* >::iterator itData = LocalList.begin(); itData != LocalList.end(); itData++ )
	{
		ServerData* pData = *itData;

		//if( pData->)
		if( pData != NULL )
		{
			if( pData->m_bServerRdy != false )
				Print( LV2, "OnEvent_CheckRegStatus", "Zone %d - TRUE", pData->m_dwServerLocalID );
			else
				Print( LV2, "OnEvent_CheckRegStatus", "Zone %d - FALSE", pData->m_dwServerLocalID );
		}

	}


	
	for( it = m_pThis->m_mapZoneSrvStatus.begin(); it != m_pThis->m_mapZoneSrvStatus.end(); it++ )
	{
		pSrvStatus = it->second;
		if( pSrvStatus->emStatus == EM_LSRV_STATUS_SENDINGREG )
		{
			_Net->SrvLogout( pSrvStatus->iServerID ); // 將沒有完成註冊的的 Srv 斷線
			pSrvStatus->emStatus = EM_LSRV_STATUS_TIMEOUTCLOSING;

			//FunctionSchedularClass::Push( OnEvent_CheckRegStatus, 500, NULL, NULL );
			//return 0;
		}
	}

	for( it = m_pThis->m_mapZoneSrvStatus.begin(); it !=m_pThis-> m_mapZoneSrvStatus.end(); it++ )
	{
		pSrvStatus = it->second;
		if( pSrvStatus->emStatus == EM_LSRV_STATUS_WAITINGREG )
		{
			int iZoneItemID = m_pThis->GetFreeZoneItemID();

			if( iZoneItemID != -1 )
			{
				// 配送 ZoneItemID , 並將之狀態設成等待 OK 確認
				Print( LV1, "CNetMaster_ZoneStatus_Child:OnEvent_CheckRegStatus" , "配發 ZoneItemID( %d ) 給 ZoneSrv( %d )!!",  iZoneItemID, pSrvStatus->iLocalID );
				ZoneSrvRegResult( pSrvStatus->iServerID, EM_ZoneSrvRegResult_OK, iZoneItemID );
				pSrvStatus->emStatus = EM_LSRV_STATUS_WAITINGLSRV;
			}
			else
			{				
				Print( LV3, "CNetMaster_ZoneStatus_Child:OnEvent_CheckRegStatus" , "配發 ZoneItemID 時, 發現已分配超過上限!! 通知 ZoneSrv( %d )", pSrvStatus->iLocalID );
				ZoneSrvRegResult( pSrvStatus->iServerID, EM_ZoneSrvRegResult_OUT_ZONEITEMID, iZoneItemID );
			}
		}
	}

	m_bInitDone = true;

	return 0;
}
//-------------------------------------------------------------------
void CNetMaster_ZoneStatus_Child::On_ZoneSrvOkay ( int iServerID, int iZoneID )
{
	// 將 ZoneSrv 狀態設為 Okay. 並允許玩家連入
	LocalServerStatus* pSrvStatus = NULL;
	map< int , LocalServerStatus* >::iterator it;
	if( ( it = m_pThis->m_mapZoneSrvStatus.find( iZoneID ) ) != m_pThis->m_mapZoneSrvStatus.end() )
	{
		pSrvStatus = it->second;
		pSrvStatus->emStatus = EM_LSRV_STATUS_OK;
	}
	else
	{
		Print( LV2, "CNetMaster_ZoneStatus_Child:On_ZoneSrvOkay" , "找不到 ZoneServer(%d) 資料!!", iZoneID );
	}

	if( pSrvStatus )
		Print( LV2, "CNetMaster_ZoneStatus_Child:On_ZoneSrvOkay" , "收到 ZoneSrv[ %d ] 通知準備完成!!", pSrvStatus->iLocalID );


}

void	CNetMaster_ZoneStatus_Child::On_RoleEnterZone			( int iSrvID, int iZoneSrvID, int iDBID )
{
	//BaseItemObject::
	ClientInfo* pClientInfo = NULL;
	pClientInfo = g_pClientManger->GetClientInfoByDBID( iDBID );

	if( pClientInfo )
	{
		Print( LV2, "RoleEnterZone" , "[ %s ] Zone[ %d ] SrvID[ %d ]", pClientInfo->m_sAccountName.c_str(), iZoneSrvID, iSrvID );

		if( pClientInfo->m_emClientStatus == EM_ClientStatus_WaitConnectZone )
		{
			pClientInfo->m_emClientStatus = EM_ClientStatus_InGame;			
		}

		// 作假 BTI 用
		//if( pClientInfo->m_pSelectedRole != NULL )
		//{
		//	g_pBTI->PlayerEnterZone( pClientInfo->m_sAccountName.c_str(), iSrvID, pClientInfo->m_pSelectedRole->Base.DBID );
		//}

		pClientInfo->m_setZone.insert( iZoneSrvID );	
		//pClientInfo->m_setZone.insert( iSrvID );

		if( pClientInfo->m_bEnterWorld == false )
		{
			pClientInfo->m_bEnterWorld = true;
			Global::Net_ServerList->AccountEnterWorld( pClientInfo->m_iClientID, pClientInfo->m_sAccountName.c_str() );
		}
	}
	else
	{
		// 會發這有可能有很多原因
		// 1. ZoneSrv 斷線, 但延遲刪除未做. 此時角色已經換區了
		Print( Def_PrintLV2, "CNetMaster_ZoneStatus_Child" , "On_RoleEnterZone 找不到 Client" );
	}

}

void	CNetMaster_ZoneStatus_Child::On_RoleLeaveZone			( int iSrvID, int iZoneSrvID, int iDBID )
{
	ClientInfo* pClientInfo = NULL;
	pClientInfo = g_pClientManger->GetClientInfoByDBID( iDBID );

	if( pClientInfo )
	{
		Print( LV2, "RoleLeaveZone" , "[ %s ] Zone[ %d ] SrvID[ %d ]", pClientInfo->m_sAccountName.c_str(), iZoneSrvID, iSrvID );

		//pClientInfo->m_iZoneID			= -1; // 換 Zone 中
		//pClientInfo->m_setZone.erase( iSrvID );
		pClientInfo->m_setZone.erase( iZoneSrvID );

		//if( pClientInfo->m_emClientStatus != EM_ClientStatus_WaitLeaveWorld )
			//pClientInfo->m_emClientStatus	= EM_ClientStatus_RoleLeaveZone;

		pClientInfo->m_iLastRespond	= GetTickCount();

		//  [11/29/2007 Hsiang]
		//pClientInfo->m_bEnterWorld = false;
	}
}

void	CNetMaster_ZoneStatus_Child::On_RoleLeaveWorld			( int iServerID, int iZoneSrvID, int iDBID, EM_RoleLeaveWorld emEvent, int iExpToNextLevel )
{
	ClientInfo* pClientInfo = NULL;	
	pClientInfo = g_pClientManger->GetClientInfoByDBID( iDBID );

	if( pClientInfo != NULL && pClientInfo->m_setZone.empty() != false )
	{
		Print( LV2, "RoleLeaveWorld" , "[ %s ] Zone[%d] SrvID[ %d ]", pClientInfo->m_sAccountName.c_str(), iZoneSrvID, iServerID );

		if( Global::_IsEnablePlayHistoryLog == 1 )
		{
			__time32_t				NowTime;
			struct	tm*				gmt;

			int						iLogoutTime = 0;

			_time32( &NowTime );
			gmt			= _localtime32( &NowTime );		
			iLogoutTime	= (int)_mktime32( gmt );


			char szBuff[4096];

			sprintf( szBuff , "Insert PlayerPlayHistory( WorldID,PlayerDBID,LoginTime,LogoutTime,LogoutMapID,GainEXP, EXPToNextLevel,LoginIP) VALUES(%d,%d,CAST( %f AS float ),CAST( %f AS float ), %d ,%d, %d, '%s')"
				, Global::_Net->GetWorldID()
				, iDBID
				, TimeExchangeIntToFloat( pClientInfo->m_iLoginTime )
				, TimeExchangeIntToFloat( iLogoutTime )
				, iZoneSrvID
				, 0
				, iExpToNextLevel
				, pClientInfo->m_iLoginIP.c_str()
				);

			Net_DCBase::LogSqlCommand( szBuff );
		}

		if( pClientInfo->m_emClientStatus == EM_ClientStatus_WaitLastAccountLeaveZone )
		{
			if( pClientInfo->m_iIDCheckZonePlayer != -1 )
			{
				Schedular()->Erase( pClientInfo->m_iIDCheckZonePlayer );
				pClientInfo->m_iIDCheckZonePlayer = -1;
			}
			((CNetSrv_MasterServer_Child*)CNetSrv_MasterServer_Child::m_pThis)->ClearAccountCache( pClientInfo->m_sAccountName.c_str() );
			Net_DCBaseChild::SetPlayerLiveTime_ByAccount( GetPCenterID(pClientInfo->m_sAccountName.c_str()), (char*)pClientInfo->m_sAccountName.c_str(), 0 );
			g_pClientManger->DeleteClientInfo( pClientInfo );


			// 寫入 log 




			
			return;
		}

		if( pClientInfo->m_emClientStatus == EM_ClientStatus_WaitLastAccountLeave )
		{
			g_pClientManger->EraseClientInfoByDBID( iDBID );	// 將其從 DBID MAP 中刪除
			pClientInfo->m_emClientStatus = EM_ClientStatus_Init;
			CNetSrv_MasterServer_Child::LoginResult( pClientInfo->m_iClientID, EM_LoginResult_OK );		
			Net_DCBaseChild::SetPlayerLiveTime_ByAccount( GetPCenterID(pClientInfo->m_sAccountName.c_str()) , (char*)pClientInfo->m_sAccountName.c_str(), 10000 );
		}

		// 應該是 CliDisconnect 殘留的結果
		// 真的
	}
}
//-----------------------------------------------------------------------------------------------------
int CNetMaster_ZoneStatus_Child::OnEvent_StopAllServer( SchedularInfo* pInfo , PVOID pContext )
{
	// 檢查時間, 距離 5 min 前, 每 5min 公告一次, 5min 內, 每 1min 公告一次, 最後 30 sec, 發封包給 ZoneSrv 自行停機倒數, 並在 GameSrv 排程, 準備停機. 

	//m_pThis->m_dwNextCheckStopTime = m_pThis->m_dwNextCheckStopTime - 1;


	static bool IsDestoryAllServer	= false;
	static bool IsEnd				= false;


	if( m_pThis->m_iShutdownMin <= 0 ) // 表時間到了, 開始觀查 ServerList 的狀態, 一但 GameSrv 都斷線了. 就發送 Stop 命令給 DataCenter 及所有 Server
	{
		// 檢查所有的 LocalSrv 都關了, 才能關畢 DataCenter 和其它 Server
			
			// Because zone server would shutdown himself, if closetime smaller than 5 min, so we dont need to notice zonesrv
			bool bLocalExist = false;
			vector< ServerData* >& LocalSrvList = Global::Net_ServerList->GetLocalList();
			for( int i = 0 ; i < (int)LocalSrvList.size() ; i++ )
			{
				if( LocalSrvList[i] != NULL )
				{
					bLocalExist = true;
					break;
				}
			}

			// after all zone server closed, send command to all server and ask them to shutdown themselves
			if( bLocalExist == false && IsDestoryAllServer != true )
			{
				// 所有 Zone 都掛了, 發給所有 Server 自爆

				//printf( "Sending shutdown command to all server...\n" ); fflush( NULL );
				Print(LV2, "CNetMaster_ZoneStatus_Child", "Sending shutdown command to all server...\n" ); fflush( NULL );
				
				PG_SL_SL2SL_Shutdown	Packet;
				Packet.iCloseTime = 0;

				SendToAllServer( sizeof( Packet ), &Packet );
				Global::SendFlush();

				IsDestoryAllServer	= true;

				Schedular()->Push( OnEvent_StopAllServer, 1000, m_pThis, NULL );	// 每秒觸發一次
				return 0;
			}

			if( IsDestoryAllServer != false && IsEnd == false )
			{
				// Wait 1 sec, then ask switch and proxy to shutdown themselves
				// SendToSwitch();
				//printf( "Wait 1 sec to close myself...\n" ); fflush( NULL );
				Print(LV2, "CNetMaster_ZoneStatus_Child", "Wait 1 sec to close myself...\n" ); fflush( NULL );

				IsEnd = true;
				Schedular()->Push( OnEvent_StopAllServer, 1000, m_pThis, NULL );	// 每秒觸發一次
				return 0;
			}

			if( IsEnd != false )
			{
				//printf( "close myself...\n" ); fflush( NULL );
				Print(LV2, "CNetMaster_ZoneStatus_Child", "close myself...\n" ); fflush( NULL );
				Global::Destory();
				return 0;
			}

			Schedular()->Push( OnEvent_StopAllServer, 200, m_pThis, NULL );	// 每秒觸發一次
	}
	else
	{
		// 發給 ZoneSrv 停機廣播
			//printf( "Send shutdown command to zone server...\n" ); fflush( NULL );
			Print(LV2, "CNetMaster_ZoneStatus_Child", "Send shutdown command to zone server...\n" ); fflush( NULL );
			PG_SL_SL2SL_Shutdown	Packet;
			Packet.iCloseTime = m_pThis->m_iShutdownMin;

			SendToAllLocal( sizeof( Packet ), &Packet );

			m_pThis->m_iShutdownMin--;	// 每分鐘扣 1

			Schedular()->Push( OnEvent_StopAllServer, 60000, m_pThis, NULL );	// 每 1min 觸發一次
	}
	return 0;
}
//-----------------------------------------------------------------------------------------------------
int CNetMaster_ZoneStatus_Child::OnEvent_CheckReloginAccount( SchedularInfo* pInfo , PVOID pContext )
{
	int iClientID	=	(int)pContext;

	ClientInfo* pClientInfo	= NULL;
	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );

	if( pClientInfo )
	{
		if( pClientInfo->m_setZone.empty() != false )
		{
			if( pClientInfo->m_emClientStatus == EM_ClientStatus_WaitLastAccountLeave )
			{
				pClientInfo->m_emClientStatus = EM_ClientStatus_Init;
				CNetSrv_MasterServer_Child::LoginResult( iClientID, EM_LoginResult_OK );
			}			
		}
		else
		{
			if( pClientInfo->m_emClientStatus == EM_ClientStatus_WaitLastAccountLeave )
			{
				// After 5sec, and we still failed. so we give up and kick account. and log
				char szBuffer[1024];
				sprintf( szBuffer, "( %d )( %s ) Zone player cant release after 5sec", iClientID, pClientInfo->m_sAccountName.c_str() );
				_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), szBuffer );
				Print( Def_PrintLV5, "CNetSrv_MasterServer_Child::OnEvent_CheckReloginAccount", szBuffer );
			}
		}		
	}

	/*
	// 沒找到相同的帳號, 建立 ClientInfo, 並告知 Client
	pClientInfo					= g_pClientManger->CreateClientInfo();

	pClientInfo->m_bOnline		= true;

	pClientInfo->m_iClientID	= iClientID;
	pClientInfo->m_sAccountName	= sAccount;

	pClientInfo->m_iLastRespond	= GetTickCount();

	g_pClientManger->SetClientInfoByID( iClientID, pClientInfo );
	g_pClientManger->SetClientInfoByAccount( sAccount, pClientInfo );

	LoginResult( iClientID, EM_LoginResult_OK );


	Print( Def_PrintLV1, "CNetSrv_MasterServer_Child::On_ClientConnect" , "( %d ) 帳號( %s) Connected", iClientID, pClientInfo->m_sAccountName.c_str() );
	*/

	return 0;
}
//-------------------------------------------------------------------------------------------------
void CNetMaster_ZoneStatus_Child::On_KickZonePlayerResult( int iSrvID, int iZoneID, const char* pszAccountName, int iResult )
{
	ClientInfo* pClientInfo = NULL;
	pClientInfo = g_pClientManger->GetClientInfoByAccount( pszAccountName );

	if( pClientInfo != NULL )
	{
		pClientInfo->m_setZone.erase( iZoneID );			
	}
}
//-------------------------------------------------------------------------------------------------
void CNetMaster_ZoneStatus_Child::On_BTI_SetPlayerHours( int iSrvID, const char* _pszAccountName, int iHealthTime )
{
	map< string, ClientInfo* >::iterator it = g_pClientManger->m_mapAccount.find( _pszAccountName );

	if( it != g_pClientManger->m_mapAccount.end() )
	{
		const char* pAccountSource	= ( it->first ).c_str();
		ClientInfo* pClientInfo		= it->second;


		OnlinePlayerInfoStruct* pObjInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByAccount( (char*)_pszAccountName );

		if( pObjInfo != NULL )
		{
			int		iRoleDBID	= pObjInfo->DBID;
			int		iZoneID		= pObjInfo->ZoneID;
			int		iSrvID		= Global::Net_ServerList->LocalID_To_SrvID( iZoneID );

			if( pClientInfo != NULL )
			{
				if( iHealthTime == -1 )
				{
					// 正常帳號, 但為了保險, 傳送一個超大值給他
					int iSec = 2592000; // ( 24 * 30 ) * 3600;
					pClientInfo->m_AccountBase.PlayTimeQuota	= iSec;
					CNetSrv_MasterServer::SL_BTI_RolePlayHours( iSrvID, iRoleDBID, pClientInfo->m_AccountBase.PlayTimeQuota );
					Print( LV2, "BTI", "Send PlayTime ( %d ) to (%d)(%d)( %s )", pClientInfo->m_AccountBase.PlayTimeQuota, iSrvID,iZoneID, pClientInfo->m_sAccountName.c_str() );
				}
				else
				{
					// 收到通知時間, 檢查時間是否一樣
					//int iSec = ( 5 - iHours ) * 3600;					
					//if( pClientInfo->m_bPlayControl == false || pClientInfo->m_AccountBase.PlayTimeQuota !=  iHealthTime )
					//if( pClientInfo->m_AccountBase.PlayTimeQuota != iHealthTime )
					{
						//pClientInfo->m_bPlayControl = true;
						pClientInfo->m_AccountBase.PlayTimeQuota	= iHealthTime;
						CNetSrv_MasterServer::SL_BTI_RolePlayHours( iSrvID, iRoleDBID, iHealthTime );
						Print( LV2, "BTI", "Send PlayTime ( %d ) to (%d)(%d)( %s )", pClientInfo->m_AccountBase.PlayTimeQuota, iSrvID,iZoneID, pClientInfo->m_sAccountName.c_str() );
					}
				}			
			}

		}



		/*
		for( set<int>::iterator itz = pClientInfo->m_setZone.begin(); itz != pClientInfo->m_setZone.end(); itz++ )
		{
			int			iLocalID			= *itz;
			int			iRoleSrvID			= Global::Net_ServerList->LocalID_To_SrvID( *itz );

			if( pClientInfo != NULL && pClientInfo->m_pSelectedRole != NULL )
			{
				if( iHealthTime == -1 )
				{
					// 正常帳號, 但為了保險, 傳送一個超大值給他
					int iSec = 2592000; // ( 24 * 30 ) * 3600;
					pClientInfo->m_AccountBase.PlayTimeQuota	= iSec;
					CNetSrv_MasterServer::SL_BTI_RolePlayHours( iRoleSrvID, pClientInfo->m_pSelectedRole->Base.DBID, pClientInfo->m_AccountBase.PlayTimeQuota );
					Print( LV2, "BTI", "Send PlayTime ( %d ) to (%d)(%d)( %s )", pClientInfo->m_AccountBase.PlayTimeQuota, iRoleSrvID,iLocalID, pClientInfo->m_sAccountName.c_str() );
				}
				else
				{
					// 收到通知時間, 檢查時間是否一樣
					//int iSec = ( 5 - iHours ) * 3600;					
					if( pClientInfo->m_bPlayControl == false || pClientInfo->m_AccountBase.PlayTimeQuota !=  iHealthTime )
					{
						pClientInfo->m_bPlayControl = true;
						pClientInfo->m_AccountBase.PlayTimeQuota	= iHealthTime;
						CNetSrv_MasterServer::SL_BTI_RolePlayHours( iRoleSrvID, pClientInfo->m_pSelectedRole->Base.DBID, iHealthTime );
						Print( LV2, "BTI", "Send PlayTime ( %d ) to (%d)(%d)( %s )", pClientInfo->m_AccountBase.PlayTimeQuota, iRoleSrvID,iLocalID, pClientInfo->m_sAccountName.c_str() );
					}
				}			
			}
		}
		*/
	}
}