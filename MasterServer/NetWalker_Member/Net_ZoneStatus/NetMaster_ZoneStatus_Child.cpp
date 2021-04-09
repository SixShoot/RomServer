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

	Print( Def_PrintLV1, "CNetMaster_ZoneStatus_Child:Init" , "����( %0.2f )����. �� LocalServer �o�e���U�T��!!", ((float)DF_TIME_ZONESRV_REGTIMEOUT / 1000 ) );

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
			// �N���������������o�X�h
			//return (iLastID+1);
			break;
		}
		iLastID = *it;
		it++;
	}

	int iZoneItemID =  (iLastID+1);
	
	if( iZoneItemID > 2400 )
	{
		Print( Def_PrintLV4, "CNetMaster_ZoneStatus_Child:GetFreeZoneItemID" ,"ZoneItemID 2400 �t�o���F!!" );
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
	// �ˬd ServerList �� Local Server �M�椤�P���U���ƶq���L�ۦP, �@���n�� ServerList �� LocalServer �ƶq�M 
	// ����n�D���U�� Local Server �ƶq�@�ˤ~�i�H�}�l�t�o ZoneItem ID

	LocalServerStatus* pSrvStatus = NULL;

	map< int , LocalServerStatus* >::iterator it;
	if( ( it = m_mapZoneSrvStatus.find( iZoneID ) ) == m_mapZoneSrvStatus.end() )
	{
		Print( LV1, "CNetMaster_ZoneStatus_Child:On_ZoneSrvReg" , "ZoneServer( %d )�䤣�� Server ���", iZoneID );
	}
	else
	{
		pSrvStatus = it->second;
	}

	
	if( iZoneItemID == DF_NO_ZONEITEMID )
		Print( LV1, "CNetMaster_ZoneStatus_Child:On_ZoneSrvReg" , "���� ZoneSrv[ %d ] ���U. ���ݰt�o ZoneItemID", pSrvStatus->iLocalID );
	else
		Print( LV1, "CNetMaster_ZoneStatus_Child:On_ZoneSrvReg" , "���� ZoneSrv[ %d ] ���U. �q�� Master ���ϥ� ZoneItemID[ %d ]��. �٦�[ %d ] ID ���ݵ��U!!", pSrvStatus->iLocalID, iZoneItemID ,iRemainCounter );

	// �ˬd ZoneServer �O�_�n���s���U�ҨϥΪ� ZoneItemID

	if( iZoneItemID != DF_NO_ZONEITEMID )
	{
		if( m_bInitDone == false )
		{
			m_setUsedZoneItemID.insert( iZoneItemID );

			if( iRemainCounter == 0 )
			{
				pSrvStatus->emStatus = EM_LSRV_STATUS_WAITINGLSRV;

				// �q�� Zone Server �w��������� ID, �A�i�H���T�u�@�F
				ZoneSrvRegResult( pSrvStatus->iServerID, EM_ZoneSrvRegResult_OK, DF_NO_ZONEITEMID );
			}
		}
		else
		{			
			// MasterServer �w�g�}�l�B�@�F. �T������ ZoneServer �o�Ӫ����U�ʥ]
			Print( LV2, "CNetMaster_ZoneStatus_Child:On_ZoneSrvReg", "MasterServer �w�g�}�l�B�@�F. �T������ ZoneServer( %d ) �o�Ӫ����U�ʥ]", iZoneID );

			// How to do? Close ZoneServer?
			// �o�ʥ]�q���ӳ� Server, �B���|�N���[�J�s�դ�
			ZoneSrvRegResult( iServerID, EM_ZoneSrvRegResult_MasterSrvInitDone, iZoneID );
		}
	}
	else
	{
		if( m_bInitDone == false )
			pSrvStatus->emStatus = EM_LSRV_STATUS_WAITINGREG; // ���� Master �t ID
		else
		{
			// MasterServer �w��l�Ƨ���!! �i�H�t�o ID
			int iZoneItemID = GetFreeZoneItemID();
			if( iZoneItemID != -1 )
			{
				// �t�e ZoneItemID , �ñN�����A�]������ OK �T�{
				Print( LV1, "CNetMaster_ZoneStatus_Child:OnEvent_CheckRegStatus" , "�t�o ZoneItemID( %d ) �� ZoneSrv( %d )!!",  iZoneItemID, pSrvStatus->iLocalID );
				ZoneSrvRegResult( pSrvStatus->iServerID, EM_ZoneSrvRegResult_OK, iZoneItemID );
				pSrvStatus->emStatus = EM_LSRV_STATUS_WAITINGLSRV;
			}
			else
			{
				Print( LV3, "CNetMaster_ZoneStatus_Child:OnEvent_CheckRegStatus" , "�t�o ZoneItemID ��, �o�{�w���t�W�L�W��!! �q�� ZoneSrv( %d )" ,pSrvStatus->iLocalID );
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
			Print( Def_PrintLV4, "CNetMaster_ZoneStatus_Child:On_ZoneSrvReg(): ZoneSrv(%d) Reg, ���T�o�{���e�w���U�L, �B iZoneItemID(%d) != -1."iServerID,iZoneItemID );
		}
	}

	if( iZoneItemID != -1 )
	{
		m_setUsedZoneItemID.insert( iZoneItemID );
	}

	if( ((int)m_mapZoneSrvStatus.size()) == iLocalServerSize )
	{
		// ��Ҧ� Local Server �w�n�D���U�ε��U�����F, �}�l��n�D���U�� Srv �t�e ZoneItemID
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

	// �N�Ҧ��u�W�H�����, �o�e�� NonageServer
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
	// �o�X�ʥ]�n�D Local Server ���U


	if( m_pThis->m_mapZoneSrvStatus.find( dwServerLocalID ) == m_pThis->m_mapZoneSrvStatus.end() )
	{
		LocalServerStatus*				pSrvStatus	= NEW( LocalServerStatus );

		pSrvStatus->emStatus						= EM_LSRV_STATUS_SENDINGREG;
		pSrvStatus->iLocalID						= (int)dwServerLocalID;
		pSrvStatus->iServerID						= Global::Net_ServerList->LocalID_To_SrvID( pSrvStatus->iLocalID );
		//pSrvStatus->iTimeEventID_ZoneSrvReg		= FunctionSchedularClass::Push( OnEvent_LocalServerRegTimeOut, DF_TIME_ZoneSrvREGTIMEOUT, (PVOID)pSrvStatus, NULL );

		m_pThis->m_mapZoneSrvStatus[ dwServerLocalID ] = pSrvStatus;

		// �n�D ZoneSrv  ���U
		RequestZoneSrvReg( pSrvStatus->iServerID );
	}
	else
	{
		Print( Def_PrintLV2, "CNetMaster_ZoneStatus_Child:OnEvent_LocalServerConnected" , "�o�{�ϥάۦP LocalID(%d) Server�n�J!!", dwServerLocalID );
	}
}
//-------------------------------------------------------------------
/*
int	CNetMaster_ZoneStatus_Child::OnEvent_LocalServerRegTimeOut	( SchedularInfo* pInfo , PVOID pContext )
{
	// �q���� LocalServer ����, �òM���b ServerList �����
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
		// �R���_�u Server ������
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

			// �Q���_�u����ӳB�z����
			_Net->CliLogout( pClientInfo->m_iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "Master Zone Server Disconnect" );	// �j��N Client Logout
		}



		pSrvStatus = it->second;
		m_pThis->m_mapZoneSrvStatus.erase( pSrvStatus->iLocalID );
		delete pSrvStatus;
	}
	else
	{
		// �b Reg Server ���䤣��
		Print( Def_PrintLV2, "CNetMaster_ZoneStatus_Child:OnEvent_LocalServerDisconnect" , "�bZoneServer�s�u�M�椤�䤣�����}�� ZoneServer(%d) ���!!", dwServerLocalID );
	}
	//m_pThis->CheckRegStatus();
}
/*
//-------------------------------------------------------------------
void	CNetMaster_ZoneStatus_Child::CheckRegStatus()
{
	// �ˬd�٦��S�� Server �٬O�B�� EM_LSRV_STATUS_INIT ���A, �٦����ܫh����t�e ZoneItemID
	bool				bCheck_LSrv_Init	= false;
	LocalServerStatus*	pSrvStatus			= NULL;

	// MasterServer �٦b��l��
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
				// �t�e ZoneItemID , �ñN�����A�]������ OK �T�{
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
	// �u�� Master ��}, �٦b��l�ƪ��ɭԤ~�|����o�ƥ�. �Ψӵ��ݩҦ� Server ���A
	Print( Def_PrintLV2, "CNetMaster_ZoneStatus_Child:OnEvent_CheckRegStatus" , "��l���ݮɶ���! �P ZoneServer ��� ZoneItemID" );
	
	LocalServerStatus*							pSrvStatus			= NULL;
	map< int , LocalServerStatus* >::iterator	it;


	// �L�X�ثe�Ҧ� Zone �����A
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
			_Net->SrvLogout( pSrvStatus->iServerID ); // �N�S���������U���� Srv �_�u
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
				// �t�e ZoneItemID , �ñN�����A�]������ OK �T�{
				Print( LV1, "CNetMaster_ZoneStatus_Child:OnEvent_CheckRegStatus" , "�t�o ZoneItemID( %d ) �� ZoneSrv( %d )!!",  iZoneItemID, pSrvStatus->iLocalID );
				ZoneSrvRegResult( pSrvStatus->iServerID, EM_ZoneSrvRegResult_OK, iZoneItemID );
				pSrvStatus->emStatus = EM_LSRV_STATUS_WAITINGLSRV;
			}
			else
			{				
				Print( LV3, "CNetMaster_ZoneStatus_Child:OnEvent_CheckRegStatus" , "�t�o ZoneItemID ��, �o�{�w���t�W�L�W��!! �q�� ZoneSrv( %d )", pSrvStatus->iLocalID );
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
	// �N ZoneSrv ���A�]�� Okay. �ä��\���a�s�J
	LocalServerStatus* pSrvStatus = NULL;
	map< int , LocalServerStatus* >::iterator it;
	if( ( it = m_pThis->m_mapZoneSrvStatus.find( iZoneID ) ) != m_pThis->m_mapZoneSrvStatus.end() )
	{
		pSrvStatus = it->second;
		pSrvStatus->emStatus = EM_LSRV_STATUS_OK;
	}
	else
	{
		Print( LV2, "CNetMaster_ZoneStatus_Child:On_ZoneSrvOkay" , "�䤣�� ZoneServer(%d) ���!!", iZoneID );
	}

	if( pSrvStatus )
		Print( LV2, "CNetMaster_ZoneStatus_Child:On_ZoneSrvOkay" , "���� ZoneSrv[ %d ] �q���ǳƧ���!!", pSrvStatus->iLocalID );


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

		// �@�� BTI ��
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
		// �|�o�o���i�঳�ܦh��]
		// 1. ZoneSrv �_�u, ������R������. ���ɨ���w�g���ϤF
		Print( Def_PrintLV2, "CNetMaster_ZoneStatus_Child" , "On_RoleEnterZone �䤣�� Client" );
	}

}

void	CNetMaster_ZoneStatus_Child::On_RoleLeaveZone			( int iSrvID, int iZoneSrvID, int iDBID )
{
	ClientInfo* pClientInfo = NULL;
	pClientInfo = g_pClientManger->GetClientInfoByDBID( iDBID );

	if( pClientInfo )
	{
		Print( LV2, "RoleLeaveZone" , "[ %s ] Zone[ %d ] SrvID[ %d ]", pClientInfo->m_sAccountName.c_str(), iZoneSrvID, iSrvID );

		//pClientInfo->m_iZoneID			= -1; // �� Zone ��
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


			// �g�J log 




			
			return;
		}

		if( pClientInfo->m_emClientStatus == EM_ClientStatus_WaitLastAccountLeave )
		{
			g_pClientManger->EraseClientInfoByDBID( iDBID );	// �N��q DBID MAP ���R��
			pClientInfo->m_emClientStatus = EM_ClientStatus_Init;
			CNetSrv_MasterServer_Child::LoginResult( pClientInfo->m_iClientID, EM_LoginResult_OK );		
			Net_DCBaseChild::SetPlayerLiveTime_ByAccount( GetPCenterID(pClientInfo->m_sAccountName.c_str()) , (char*)pClientInfo->m_sAccountName.c_str(), 10000 );
		}

		// ���ӬO CliDisconnect �ݯd�����G
		// �u��
	}
}
//-----------------------------------------------------------------------------------------------------
int CNetMaster_ZoneStatus_Child::OnEvent_StopAllServer( SchedularInfo* pInfo , PVOID pContext )
{
	// �ˬd�ɶ�, �Z�� 5 min �e, �C 5min ���i�@��, 5min ��, �C 1min ���i�@��, �̫� 30 sec, �o�ʥ]�� ZoneSrv �ۦ氱���˼�, �æb GameSrv �Ƶ{, �ǳư���. 

	//m_pThis->m_dwNextCheckStopTime = m_pThis->m_dwNextCheckStopTime - 1;


	static bool IsDestoryAllServer	= false;
	static bool IsEnd				= false;


	if( m_pThis->m_iShutdownMin <= 0 ) // ��ɶ���F, �}�l�[�d ServerList �����A, �@�� GameSrv ���_�u�F. �N�o�e Stop �R�O�� DataCenter �ΩҦ� Server
	{
		// �ˬd�Ҧ��� LocalSrv �����F, �~������ DataCenter �M�䥦 Server
			
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
				// �Ҧ� Zone �����F, �o���Ҧ� Server ���z

				//printf( "Sending shutdown command to all server...\n" ); fflush( NULL );
				Print(LV2, "CNetMaster_ZoneStatus_Child", "Sending shutdown command to all server...\n" ); fflush( NULL );
				
				PG_SL_SL2SL_Shutdown	Packet;
				Packet.iCloseTime = 0;

				SendToAllServer( sizeof( Packet ), &Packet );
				Global::SendFlush();

				IsDestoryAllServer	= true;

				Schedular()->Push( OnEvent_StopAllServer, 1000, m_pThis, NULL );	// �C��Ĳ�o�@��
				return 0;
			}

			if( IsDestoryAllServer != false && IsEnd == false )
			{
				// Wait 1 sec, then ask switch and proxy to shutdown themselves
				// SendToSwitch();
				//printf( "Wait 1 sec to close myself...\n" ); fflush( NULL );
				Print(LV2, "CNetMaster_ZoneStatus_Child", "Wait 1 sec to close myself...\n" ); fflush( NULL );

				IsEnd = true;
				Schedular()->Push( OnEvent_StopAllServer, 1000, m_pThis, NULL );	// �C��Ĳ�o�@��
				return 0;
			}

			if( IsEnd != false )
			{
				//printf( "close myself...\n" ); fflush( NULL );
				Print(LV2, "CNetMaster_ZoneStatus_Child", "close myself...\n" ); fflush( NULL );
				Global::Destory();
				return 0;
			}

			Schedular()->Push( OnEvent_StopAllServer, 200, m_pThis, NULL );	// �C��Ĳ�o�@��
	}
	else
	{
		// �o�� ZoneSrv �����s��
			//printf( "Send shutdown command to zone server...\n" ); fflush( NULL );
			Print(LV2, "CNetMaster_ZoneStatus_Child", "Send shutdown command to zone server...\n" ); fflush( NULL );
			PG_SL_SL2SL_Shutdown	Packet;
			Packet.iCloseTime = m_pThis->m_iShutdownMin;

			SendToAllLocal( sizeof( Packet ), &Packet );

			m_pThis->m_iShutdownMin--;	// �C������ 1

			Schedular()->Push( OnEvent_StopAllServer, 60000, m_pThis, NULL );	// �C 1min Ĳ�o�@��
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
	// �S���ۦP���b��, �إ� ClientInfo, �çi�� Client
	pClientInfo					= g_pClientManger->CreateClientInfo();

	pClientInfo->m_bOnline		= true;

	pClientInfo->m_iClientID	= iClientID;
	pClientInfo->m_sAccountName	= sAccount;

	pClientInfo->m_iLastRespond	= GetTickCount();

	g_pClientManger->SetClientInfoByID( iClientID, pClientInfo );
	g_pClientManger->SetClientInfoByAccount( sAccount, pClientInfo );

	LoginResult( iClientID, EM_LoginResult_OK );


	Print( Def_PrintLV1, "CNetSrv_MasterServer_Child::On_ClientConnect" , "( %d ) �b��( %s) Connected", iClientID, pClientInfo->m_sAccountName.c_str() );
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
					// ���`�b��, �����F�O�I, �ǰe�@�ӶW�j�ȵ��L
					int iSec = 2592000; // ( 24 * 30 ) * 3600;
					pClientInfo->m_AccountBase.PlayTimeQuota	= iSec;
					CNetSrv_MasterServer::SL_BTI_RolePlayHours( iSrvID, iRoleDBID, pClientInfo->m_AccountBase.PlayTimeQuota );
					Print( LV2, "BTI", "Send PlayTime ( %d ) to (%d)(%d)( %s )", pClientInfo->m_AccountBase.PlayTimeQuota, iSrvID,iZoneID, pClientInfo->m_sAccountName.c_str() );
				}
				else
				{
					// ����q���ɶ�, �ˬd�ɶ��O�_�@��
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
					// ���`�b��, �����F�O�I, �ǰe�@�ӶW�j�ȵ��L
					int iSec = 2592000; // ( 24 * 30 ) * 3600;
					pClientInfo->m_AccountBase.PlayTimeQuota	= iSec;
					CNetSrv_MasterServer::SL_BTI_RolePlayHours( iRoleSrvID, pClientInfo->m_pSelectedRole->Base.DBID, pClientInfo->m_AccountBase.PlayTimeQuota );
					Print( LV2, "BTI", "Send PlayTime ( %d ) to (%d)(%d)( %s )", pClientInfo->m_AccountBase.PlayTimeQuota, iRoleSrvID,iLocalID, pClientInfo->m_sAccountName.c_str() );
				}
				else
				{
					// ����q���ɶ�, �ˬd�ɶ��O�_�@��
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