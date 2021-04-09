#define _WIN32_WINNT 0x0500

#include "./Netsrv_Master_Login_Child.h"
#include "../net_zonestatus/NetMaster_ZoneStatus_Child.h"
#include "../Net_ServerList/Net_ServerList_Child.h"
#include "../Net_DCBase/Net_DCBaseChild.h"
#include "roledata/RoleDataEx.h"
#include "../../mainproc/ClientInfo/ClientInfo.h"
#include "md5/Mymd5.h"
#include <Windows.h>
//#include "../../BTI/RABTI.h"

//#define DF_MAXNUM_ACCOUNT	4000
#define DF_TIME_CHECK_BTI 4000000

int	g_iNumLoadAccountLimit	= 100;
int g_iNumEnterWorldLimit	= 4000;

map< string, DWORD >			CNetSrv_MasterServer_Child::m_mapLoadQueue;
deque< string >					CNetSrv_MasterServer_Child::m_vecWaitQueue;
int								CNetSrv_MasterServer_Child::m_iObjCount;

#define DF_TIME_CHECK_ACCOUNTCACHE		60000
#define DF_TIME_DELETE_ACCOUNTCACHE		180000


#define DF_TIME_CHECKWAITLIST			15000
#define DF_TIME_SET_SERVERLIST_INFO		10000
#define DF_TIME_REFRESH_CAPTCHA			1000
#define DF_TIME_CHECK_KEEPACCOUNTLIVE	180000
#define DF_TIME_CHECK_LOADACCOUNT_TIME	300000

#define DF_TIME_KEEP_WAITQUEUEPRIORITY	300000

#define DF_TIME_CHECK_ACCOUNT_LIVE		180000	// 5 �����S���ʧ@����, �t�αN�|�۰ʽ�H
#define DF_TIME_CHECK_ACCOUNT_DBLIVE	600000	// 5 �����S���ʧ@����, �t�αN�|�۰ʽ�H
#define DF_TIME_KICK_DEAD_ACCOUNT		900000	// 15 �����S���ʧ@����, �t�αN�|�۰ʽ�H
//-------------------------------------------------------------------
int	CNetSrv_MasterServer_Child::_OnTimeProc_10Sec( SchedularInfo* Obj , void* InputClass )
{
	static int TimesCount = 0;
	TimesCount++;

	if( CNetMaster_ZoneStatus_Child::m_pThis->m_iShutdownMin > 5 )
	{
		RecalculateOnlinePlayer( );
	}

	//if( TimesCount >= 60 )
	{
	//	TimesCount = 0;
		Log_ServerStatus( );
	}
	
	for( int i = 0 ; i < Global::_PlayerCenterCount ; i++ )
		GetObjCount( i );

	Global::Schedular()->Push( _OnTimeProc_10Sec , DF_TIME_SET_SERVERLIST_INFO , NULL , NULL );
	
	return 0;
}

//-------------------------------------------------------------------
int	CNetSrv_MasterServer_Child::_OnTimeProc_RefreshCaptcha( SchedularInfo* Obj , void* InputClass )
{
	if (m_CaptchaGenerator)
	{
		m_CaptchaGenerator->Refresh();
	}

	Global::Schedular()->Push( _OnTimeProc_RefreshCaptcha , DF_TIME_REFRESH_CAPTCHA , NULL , NULL );

	return 0;
}
//-------------------------------------------------------------------

CNetSrv_MasterServer_Child::CNetSrv_MasterServer_Child()
{
	//m_iNumLoading = 0;
}
//-------------------------------------------------------------------
bool    CNetSrv_MasterServer_Child::Init()
{
	CNetSrv_MasterServer::_Init();

	if( m_pThis != NULL)
		return false;

	m_pThis = NEW( CNetSrv_MasterServer_Child );


	CNetSrv_MasterServer_Child* pChild = (CNetSrv_MasterServer_Child*)m_pThis;

	Schedular()->Push( OnEvent_WaitLoadAccount,		DF_TIME_CHECKWAITLIST,		m_pThis,	NULL );
	Schedular()->Push( OnEvent_CheckAlive,			DF_TIME_CHECK_ACCOUNT_LIVE, NULL,		NULL );

	Schedular()->Push( OnEvent_CheckAccountCache,	DF_TIME_CHECK_ACCOUNTCACHE, m_pThis,	NULL );
	
	Schedular()->Push( OnEvent_VivoxUpdate,			100, m_pThis,	NULL );
	

	Global::Net_ServerList->RegServerLogoutEvent	( EM_SERVER_TYPE_DATACENTER, OnEvent_DataSrvDisconnect,	DF_NO_LOCALID ); 

	Global::Schedular()->Push( _OnTimeProc_10Sec , 10*1000 , NULL , NULL );
	
	Global::Schedular()->Push( _OnTimeProc_RefreshCaptcha , DF_TIME_REFRESH_CAPTCHA , NULL , NULL );

	// APEX
//		pChild->m_APEX.Init( SC_APEXTOCLIENT, ApexKickPlayer );
//		pChild->m_APEX.StartApex();

	InitializeCriticalSection( &(((CNetSrv_MasterServer_Child*)m_pThis)->m_csBTI) );

	m_iObjCount = 0;

	for( int i=0; i<5; i++ )
	{
		((CNetSrv_MasterServer_Child*)m_pThis)->m_iAgency[i]=0;
	}

	//((CNetSrv_MasterServer_Child*)m_pThis)->m_vivox.Init();

	return true;
}

//-------------------------------------------------------------------
bool    CNetSrv_MasterServer_Child::Release()
{
	//((CNetSrv_MasterServer_Child*)m_pThis)->m_vivox.Release();

	DeleteCriticalSection( &(((CNetSrv_MasterServer_Child*)m_pThis)->m_csBTI) );	

	CNetSrv_MasterServer::_Release();
	if( m_pThis == NULL )
		return false;

//	((CNetSrv_MasterServer_Child*)m_pThis)->m_rClientInfo.Clear();
//	((CNetSrv_MasterServer_Child*)m_pThis)->m_rRole.Clear();

//	((CNetSrv_MasterServer_Child*)m_pThis)->m_mapClientInfo.clear();
//	((CNetSrv_MasterServer_Child*)m_pThis)->m_mapAccount.clear();

	delete m_pThis;
	m_pThis = NULL;

	return true;

}
//-------------------------------------------------------------------
void CNetSrv_MasterServer_Child::On_ClientConnect		( int iClientID , string sAccount ) 
{
	if( Global::_IsStopLogin  != false )
		return;

	bool		bEraseLastAccount = false;
	set<int>	setZone;
	int			iLastDBID			= 0;

	// DEBUG
	Print( Def_PrintLV2, "Debug", "OnConnect - %s (%d)",sAccount.c_str(), iClientID );

	//�ˬd�b���O�_�����D
	//assert( sAccount.size() != 0 );
	if( sAccount.size() == 0 )
		_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), sAccount.c_str(), "On_ClientConnect() get a null account size == 0" );	// �j��N Client Logout
	// �ˬd ClientID �O�_����
	
	//ClientInfo* pClientInfo = GetClientInfoByID( iClientID );
	ClientInfo* pClientInfo = NULL;

	// �ˬd ID ���L����
		pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );
		if( pClientInfo != NULL ) 
		{ 
			Print( Def_PrintLV5, "CNetSrv_MasterServer_Child::OnClientonnect" , "ClientID( %d ) exist, last role didnt get roleleaveworld",iClientID ); 

			// We got a exist ClientID, Clear old one and report to log
			// Kick last player
				for( set< int >::iterator it = pClientInfo->m_setZone.begin(); it != pClientInfo->m_setZone.end(); it++ )
				{
					// �o���� Zone Server, ��H

					int iLocalID	= *it; 
					int iSrvID		= Global::Net_ServerList->LocalID_To_SrvID( iLocalID );

					if( iSrvID != -1 )
					{
						CNetMaster_ZoneStatus_Child::KickZonePlayer( iSrvID, iLocalID, pClientInfo->m_sAccountName.c_str() );
					}
				}
				
			// Clear last account data, and accept new client
				g_pClientManger->DeleteClientInfo( pClientInfo );
				
		}

	//�ˬd�b���O�_����
		pClientInfo = g_pClientManger->GetClientInfoByAccount( sAccount );
		if( pClientInfo != NULL )
		{
			// �{�b��Ʈw�����ۦP���b��
			//Print( Def_PrintLV3, "CNetSrv_MasterServer_Child::OnClientonnect" , "�{�b��Ʈw�����ۦP���b��( %s )", sAccount.c_str() );
			//LoginResult( iClientID, EM_LoginResult_AccountExist );
			//_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "Master On_ClientConnect() Account exist" );	// �j��N Client Logout

			
			// �q�� Client, �|������b�@��, �y��n�J
			// �N�ثe�s�b�@�ɪ������_�u, �Y�O���b���u���A������, �Y�w�����u�Ƶ{, �h���B�z, �Y�S���Ƶ{, �h���J

			/*
			if( pClientInfo->m_emClientStatus == EM_ClientStatus_WaitLeaveWorld )
			{

			}
			*/

			// �b�o�@���j���_�u, �M����J�Ƶ{, �b�ˬd�ϰ�O�_�M��
				for( set< int >::iterator it = pClientInfo->m_setZone.begin(); it != pClientInfo->m_setZone.end(); it++ )
				{
					// �o���� Zone Server, ��H

					int iLocalID	= *it; 
					int iSrvID		= Global::Net_ServerList->LocalID_To_SrvID( iLocalID );

					if( iSrvID != -1 )
					{
						CNetMaster_ZoneStatus_Child::KickZonePlayer( iSrvID, iLocalID, pClientInfo->m_sAccountName.c_str() );
					}
					else
					{
						pClientInfo->m_setZone.erase( iLocalID );
					}
				}

				//if( pClientInfo->m_pSelectedRole != NULL )
				if( pClientInfo->m_iSelectedRoleDBID != 0 )
				{
					//iLastDBID	= pClientInfo->m_pSelectedRole->Base.DBID;	
					iLastDBID = pClientInfo->m_iSelectedRoleDBID;
				}

				setZone		= pClientInfo->m_setZone;


			// �j��N�W���s�u�_�u, �çQ�Τ��e�����
				_Net->CliLogout( pClientInfo->m_iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "Master On_ClientConnect() Account exist, Kick Last Account" );

				g_pClientManger->DeleteClientInfo( pClientInfo );
				
				if( pClientInfo->m_setZone.empty() == false )
					bEraseLastAccount = true;				
		}

		__time32_t				NowTime;
		struct	tm*				gmt;

		int						iLoginTime = 0;

		_time32( &NowTime );
		gmt			= _localtime32( &NowTime );		
		iLoginTime	= (int)_mktime32( gmt );

	// �S���ۦP���b��, �إ� ClientInfo, �çi�� Client
		pClientInfo					= g_pClientManger->CreateClientInfo();
		
		pClientInfo->m_bOnline		= true;

		pClientInfo->m_iClientID	= iClientID;
		pClientInfo->m_sAccountName	= sAccount;

		pClientInfo->m_iLoginTime	= iLoginTime;
		pClientInfo->m_iLoginIP		= GlobalBase::GetClientIpNumStr( iClientID );

		pClientInfo->m_iLastRespond	= GetTickCount();

		g_pClientManger->SetClientInfoByID( iClientID, pClientInfo );
		g_pClientManger->SetClientInfoByAccount( sAccount, pClientInfo );


	// �ˬd�O������B��
		{
			for( int i=0; i<5;i++)
			{
				if( strlen( _AgencyKeyword[i] ) != 0 )
				{
					if( sAccount.find( _AgencyKeyword[i] ) != string::npos )
					{
						m_iAgency[i]++;
					}
				}				
			}
		}

		if( bEraseLastAccount != false )
		{
			pClientInfo->m_setZone = setZone;

			// �W�����w�i�J���ϰ�
			if( iLastDBID != 0 )
			{
				g_pClientManger->SetClientInfoByDBID( iLastDBID, pClientInfo );
			}
			
			// �L�����, �ˬd����Ҧb�ϰ쪺�H���O�_�Q�s�F. �n�O 5 ��ᤴ�S���s, �h��������O LOG, �ý𱼥i�����b��. �n�O
			pClientInfo->m_emClientStatus = EM_ClientStatus_WaitLastAccountLeave;
			Schedular()->Push( CNetMaster_ZoneStatus_Child::OnEvent_CheckReloginAccount, 15000, (PVOID)pClientInfo->m_iClientID, NULL );
			Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_ClientConnect" , "( %d ) �b��( %s) Connected. But wait last account to be release", iClientID, pClientInfo->m_sAccountName.c_str() );
		}
		else
		{
			LoginResult( iClientID, EM_LoginResult_OK );
			Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_ClientConnect" , "( %d ) �b��( %s) Connected", iClientID, pClientInfo->m_sAccountName.c_str() );
		}

	
		


	// �q�� BTI		
	if( Global::_MaxCreateRoleCount < 10 )
		SC_MaxRoleCount( iClientID , Global::_MaxCreateRoleCount );

	SD_ReserveRoleRequest( GetPCenterID( sAccount.c_str() ) , iClientID , sAccount.c_str() );


	// �o�e Vivox ����Ƶ� Client
	//if( m_vivox.m_url.empty() == false )
	//{

	//	SC_VivoxInfo( iClientID, m_vivox.m_clienturl.c_str(), m_vivox.m_country.c_str() );

	//	char szFullAccount[256];
	//	sprintf( szFullAccount, "%s%s", pClientInfo->m_sAccountName.c_str(), m_vivox.m_country.c_str() );

	//	string EncodePassword;
	//	GetVivoxPassword( szFullAccount, EncodePassword );
	//	SC_VivoxAccount( iClientID, szFullAccount, EncodePassword.c_str(), false );
	//}
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer_Child::On_ClientDisconnect	( int iClientID )
{	

	Print( Def_PrintLV2, "DEBUG", "OnDisconnect - %d", iClientID );
	// �o�����Ӥ��γB�z

	//ClientInfo* pClientInfo = GetClientInfoByID( iClientID );
	ClientInfo* pClientInfo	= NULL;

	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );
	
	if( pClientInfo == NULL || pClientInfo->m_iSelectedRoleDBID == 0 ) //if( pClientInfo == NULL || pClientInfo->m_pSelectedRole == NULL )
		Print( Def_PrintLV1, "CNetSrv_MasterServer_Child" , "On_ClientDisconnect( %d )", iClientID );
	else
	{
		Print( Def_PrintLV1, "CNetSrv_MasterServer_Child" , "On_ClientDisconnect( %d ) and m_pSelectedRoleDBID = %d ", iClientID, pClientInfo->m_iSelectedRoleDBID );
	}

	// DEBUG
	if( pClientInfo )
		Print( Def_PrintLV2, "DEBUG", "Debug-OnDisconnect - %s (%d)",pClientInfo->m_sAccountName.c_str(), iClientID );



	if( pClientInfo )
	{
		// 3rd Program
		//-------------------------------------

			//if( pClientInfo->m_iSelectedRoleDBID != 0 )//if( pClientInfo->m_pSelectedRole != NULL )
			//{
			//	m_APEX.ApexClientLogout( iClientID, pClientInfo->m_pSelectedRole->BaseData.RoleName.Begin(), pClientInfo->m_sAccountName.c_str() );
			//}
		//-------------------------------------


		if( pClientInfo->m_bEnterWorld != false )
		{
			pClientInfo->m_bEnterWorld = false;
			Global::Net_ServerList->AccountLeaveWorld( iClientID, pClientInfo->m_sAccountName.c_str() );

			// ��s Check Second Password �ɶ�
			SD_SetCheckSecondPasswordResult( GetPCenterID( pClientInfo->m_sAccountName.c_str() ) , iClientID , pClientInfo->m_sAccountName.c_str(), 0 );
		}
			

		pClientInfo->m_bOnline = false;	// �]�w����U�u

		if( pClientInfo->m_iActive == 1 )
		{
			//g_pClientManger->DelActive();
			g_pClientManger->m_iActiveRole--;

			if( g_pClientManger->m_iActiveRole < 0 )
				g_pClientManger->m_iActiveRole = 0;
		}

		if( pClientInfo->m_bLoadedRole != false )
		{
			g_pClientManger->m_iLoadedRole--;
			m_mapWaitQueuePriority[ pClientInfo->m_sAccountName.c_str() ] = GetTickCount() + DF_TIME_KEEP_WAITQUEUEPRIORITY;
		}

		// �ˬd�O������B��
		{
			for( int i=0; i<5;i++)
			{
				if( strlen( _AgencyKeyword[i] ) != 0 )
				{
					if( pClientInfo->m_sAccountName.find( _AgencyKeyword[i] ) != string::npos )
					{
						m_iAgency[i]--;

						if( m_iAgency[i] < 0 )
						{
							m_iAgency[i] = 0;
						}
					}
				}				
			}
		}


		// �ˬd�O�_�b�ƶ�Ū����Ʀ�C��
		m_mapLoadQueue.erase( pClientInfo->m_sAccountName.c_str() );
		//m_vecWaitQueue.erase( pClientInfo->m_sAccountName.c_str() );	// On_ClientDi������ WaitQueue?

		deque< string >::iterator itWaitQueue = m_vecWaitQueue.begin();

		while( itWaitQueue != m_vecWaitQueue.end() )
		{
			if( strcmp( (*itWaitQueue).c_str(), pClientInfo->m_sAccountName.c_str() ) == 0 )
			{
				itWaitQueue = m_vecWaitQueue.erase( itWaitQueue );
			}
			else
				itWaitQueue++;
		}
		
		if( pClientInfo->m_setZone.empty() == false )
		{

			char szAccount[ _MAX_ACCOUNT_SIZE_ ];
			MyStrcpy( szAccount, pClientInfo->m_sAccountName.c_str(), sizeof( szAccount ) );
			
			// �o�X�ʥ]�i�� ZoneSrv, �𰣸� Account
			// ���� 10 ����ˬd�ϰ�O�_�٦b
				pClientInfo->m_iIDCheckZonePlayer	=	Schedular()->Push( OnEvent_CheckZonePlayer, 15000, (PVOID)pClientInfo->m_iClientID, NULL );
				pClientInfo->m_emClientStatus		=	EM_ClientStatus_WaitLastAccountLeaveZone;


				//g_pClientManger->DeleteClientInfo( pClientInfo );
				
			//int iDBID = pClientInfo->m_pSelectedRole->Base.DBID;
			//g_pClientManger->RoleLeaveWorld( iDBID, pClientInfo );
			//CNetMaster_ZoneStatus_Child::SendLeaveWorld( pClientInfo->m_iClientID, iDBID );
		}
		else
		{
			// �]���ӱb���S���n�J�C����, �ҥH���ӥi�H�ߧJ�x�s

			// �n�D DataCenter �s��
				char szAccount[ _MAX_ACCOUNT_SIZE_ ];
				MyStrcpy( szAccount, pClientInfo->m_sAccountName.c_str(), sizeof( szAccount ) );

				if( CheckAccountCache( szAccount ) ==false )
				{
					Net_DCBaseChild::SetPlayerLiveTime_ByAccount( GetPCenterID(szAccount) , szAccount, 0 );
				}

				g_pClientManger->DeleteClientInfo( pClientInfo );


		}

	}


	unsigned int	NumActPlayer	= g_pClientManger->GetActive();
	unsigned int	WaitPlayer		= (unsigned)CNetSrv_MasterServer_Child::m_vecWaitQueue.size();

	Print( Def_PrintLV1, "CNetSrv_MasterServer_Child" , "NumActPlayer[ %d ] and WaitPlayer[ %d ]", NumActPlayer, WaitPlayer );




	/*
	// �M�����b Account �O��

		g_pClientManger->EraseClientInfo( pClientInfo );

	// �o�ʥ]�i�t DataCenter �Y����w���}�C�� ( �ݰ� )

		char szAccount[ _MAX_ACCOUNT_SIZE_ ];
		strcpy( szAccount, pClientInfo->m_sAccountName.c_str() );
		Net_DCBaseChild::SetPlayerLiveTime_ByAccount( szAccount, 0 );

	*/
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer_Child::On_LoadRoleData( int iClientID, const char* pszAccount  )
{
	ClientInfo* pClientInfo	= NULL;
	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );
	
	
	
	if( pClientInfo )
	{
		if( stricmp( pClientInfo->m_sAccountName.c_str(), pszAccount ) == 0 )
		{
//			pClientInfo->m_iPlayTimeQuota = PlayTimeQuota;
			//pClientInfo->m_iPlayTimeQuota = 60*60*24*30;
			// �������B�z, ��i�Ƶ{��

			bool bLoadDirect = false;

			if( _EnableLimit == 1 || _EnableObjWait == 1 )
			{
				// ���}�ƶ��t��
				if( g_pClientManger->GetActive() < _MaxLimitPlayerCount && m_vecWaitQueue.size() == 0 )
				{
					bLoadDirect = true;
				}

				if( bLoadDirect != false )
				{
					if( _MaxObjCount <=  m_iObjCount )
						bLoadDirect = false;
				}
			}
			else
			{
				bLoadDirect = true;
			}
			
			if( bLoadDirect != false )
			{
				//m_iNumLoading++;
				// �H�Ʃ|���F��, �αƶ��t�Υ��}��
					m_mapLoadQueue[ pClientInfo->m_sAccountName ]	= GetTickCount() + DF_TIME_CHECK_LOADACCOUNT_TIME;
					
					LoadAccount( iClientID, pClientInfo->m_sAccountName );		// �o�� DataCenter
					LoadDataEvent( iClientID, EM_LoadDataEvent_StartLoad, 0 );	// �o�� Client
			}
			else
			{
				// �Ұʱƶ��t��
					_strlwr( (char*)pszAccount );

					if( _IsEnable_DisconnectPriority == 1 )
					{
						map< string, DWORD >::iterator itP = m_mapWaitQueuePriority.find( pszAccount );

						if( itP != m_mapWaitQueuePriority.end() )
						{
							bool bFound = false;
							for( deque< string >::iterator itW = m_vecWaitQueue.begin(); itW != m_vecWaitQueue.end(); itW++ )
							{
								if( m_mapWaitQueuePriority.find( *itW ) == m_mapWaitQueuePriority.end() )
								{
									bFound = true;
									m_vecWaitQueue.insert( itW, pszAccount );
									break;
								}
							}

							if( bFound == false )
								m_vecWaitQueue.push_back( pszAccount );
						}
						else
							m_vecWaitQueue.push_back( pszAccount );
						
					}
					else
					{
						m_vecWaitQueue.push_back( pszAccount );						
					}

					

					// �o�e�ʥ]�i�� Client �䵥�ݳB�z��
					LoadDataEvent( iClientID, EM_LoadDataEvent_WaitingQueue, (int)m_vecWaitQueue.size() );				
			}
		}
		else
		{
			// �b����藍�@�P
			LoginResult( iClientID, EM_LoginResult_AccountError );
			_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "Master On_LoadRoleData() Client login and loadroledata, account doesnt match" );
		}
	}
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer_Child::On_RoleData		( const char* pszAccount, int iClientID, int iDBID, PlayerRoleData* pRole, bool bCache )
{

	// �N���o�������ƼȦsPG_Login_MtoD_ReserveRoleRequest
	//------------------------------------------------------
	if( bCache != false )
	{		
		map< string, StructAccountRoleData* >::iterator it = m_mapAccountRoleData.find( pszAccount );

		if( it != m_mapAccountRoleData.end() )
		{
			StructAccountRoleData* pAccountData = it->second;

			pAccountData->mapRoleData[ iDBID ] = *pRole;


			if( pAccountData->iTotalRole == pAccountData->mapRoleData.size() )
			{
				pAccountData->bReady = true;
			}

			if( pAccountData->iLoadAccountClientID != 0 )
			{
				ClientInfo* pClientInfo	= NULL;

				pClientInfo = g_pClientManger->GetClientInfoByID( pAccountData->iLoadAccountClientID );

				if( pClientInfo != NULL )					
				{
					if( pAccountData->iTotalRole == pAccountData->mapRoleData.size() )
					{
						Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_RoleData" , "Sending cache roledata to login account[ %s ]", pszAccount );

						On_RoleDataCount( pszAccount, pAccountData->iLoadAccountClientID, pAccountData->iTotalRole, pAccountData->AccountBaseInfo, false );

						for( map< int, PlayerRoleData >::iterator itRole = pAccountData->mapRoleData.begin(); itRole != pAccountData->mapRoleData.end(); itRole++ )
						{
							On_RoleData( pszAccount, pAccountData->iLoadAccountClientID, itRole->first, &( itRole->second ), false );
						}
					}
					return;
				}

				return;
			}				
		}
	}

	//  ��X�b���ҹ����� Client ���
	//ClientInfo* pClientInfo = GetClientInfoByID( iClientID );
	int iParallelZoneCount = Net_ServerList->GetParallelZoneCount();

	ClientInfo* pClientInfo	= NULL;

	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );

	if( pClientInfo == NULL )  {  Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_RoleData" , "���쨤����. ���b�s�u�b����Ʈw���䤣�� Client(%d)", iClientID ); return;  }


	//Print( Def_PrintLV1, "On_RoleData" , "Account: %s - RoleName:%s", pClientInfo->m_sAccountName.c_str(), Utf8ToChar( pRole->BaseData.RoleName.Begin() ).c_str() );


	if( pClientInfo->m_iRoleCount != -1 )
	{
		if( pClientInfo->m_AccountBase.PlayTimeQuota <= 18000 )	// 60 * 60 * 5
		{
			//pRole->BaseData.PlayTimeQuota = pClientInfo->m_AccountBase.PlayTimeQuota;
			pRole->PlayerBaseData.PlayTimeQuota = 60*60*24*30; //pClientInfo->m_AccountBase.PlayTimeQuota;
		}
		else
		{
			pRole->PlayerBaseData.PlayTimeQuota =  60*60*24*30;
		}

		pRole->TempData.Attr.AllZoneState.FindParty = true;

		//RoleData* t_pRole = new RoleData;
		//RoleData* t_pRole = m_rRole.NewObj();
		//*t_pRole = *pRole;

		//pClientInfo->m_mapRoleData[ pRole->Base.CharField ] = t_pRole;
		//pClientInfo->m_mapRoleData[ iDBID ] = t_pRole;

		//CClientManger::
		g_pClientManger->AddRoleToClientInfo( pClientInfo, iDBID, pRole );

		// ��ڤ��e���쪺������, �P�_�O�_���������������ƤF
		// �o�����ӭn���Ƶ{. �n�O�@�w�ɶ������쨬����������, ���ӭn�q�� Client �_�u, �ΦA�B�z
		if( pClientInfo->m_mapRoleData.size() == pClientInfo->m_iRoleCount )
		{
			/*
			if( pClientInfo->m_mapRoleData.size() == 0 || strlen( pClientInfo->m_mapRoleData.begin()->second->BaseData.Password.Begin() ) == 0 )
			{
				SC_SetPasswordRequest( iClientID );
			}
			*/

			// �N��z�n����ưe�� Client
			//RoleDataEx* pRoleDataEx = NULL;
			SampleRoleData	SampleRole;
			PlayerRoleData*	pRoleDataEx = NULL;

			for( map< int, PlayerRoleData* >::iterator it = pClientInfo->m_mapRoleData.begin(); it != pClientInfo->m_mapRoleData.end(); it++ )
			{
				pRoleDataEx = it->second;

				// DEBUG 
					//pRoleDataEx->BaseData.Skill.MainJob[0].SkillID	= 500010;
					//pRoleDataEx->BaseData.Skill.MainJob[0].Point	= 1;

				((RoleDataEx*)pRoleDataEx)->GetSampleRoleData( &SampleRole );

				//SendClientRoleData( pClientInfo->m_iClientID, pRoleDataEx->Base.CharField, &SampleRole );
				SendClientRoleData( pClientInfo->m_iClientID, it->first, &SampleRole );
			}
			

			On_RoleDataEnd( pClientInfo->m_iClientID );
			SendClientRoleDataEnd( pClientInfo->m_iClientID , iParallelZoneCount );
		}
	}
	else
	{
		 Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_RoleData" , "�b���쨤��ƶq�e, �T�����쨤����. Cli(%d)", iClientID );
	}


}

void CNetSrv_MasterServer_Child::On_RoleDataCount		( const char* pszAccount, int iClientID, int iRoleCount , PlayerAccountBaseInfoStruct& AccountBase, bool bCache )
{
	//  ��X�b���ҹ����� Client ���
	//ClientInfo* pClientInfo = GetClientInfoByID( iClientID );

	Print( Def_PrintLV2, "On_RoleDataCount" , "RoleCount [%s][%d] Cache[%d]", pszAccount, iRoleCount, (DWORD)bCache);

	// �N���o�������ƼȦs
	//------------------------------------------------------
	if( bCache != false )
	{
		map< string, StructAccountRoleData* >::iterator it = m_mapAccountRoleData.find( pszAccount );

		int iLoadAccountClientID = 0;

		if( it != m_mapAccountRoleData.end() )
		{
			StructAccountRoleData* pAccountData = it->second;

			iLoadAccountClientID = pAccountData->iLoadAccountClientID;

			m_mapAccountRoleData.erase( it );		

			delete pAccountData;
		}

		// Craete Cache Data
		StructAccountRoleData* pAccountData = new StructAccountRoleData;

		pAccountData->iCreateTick			= GetTickCount();	// Delete after 5mins
		pAccountData->iTotalRole			= iRoleCount;
		pAccountData->AccountBaseInfo		= AccountBase;
		pAccountData->iLoadAccountClientID	= iLoadAccountClientID;

		m_mapAccountRoleData[ pszAccount ] = pAccountData;


		if( pAccountData->iTotalRole == 0 )
		{
			pAccountData->bReady = true;
		}

		if( pAccountData->iLoadAccountClientID != 0 && pAccountData->bReady == false )
		{
			return;
		}
	}
	//------------------------------------------------------

	ClientInfo* pClientInfo = NULL;

	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );

	if( pClientInfo == NULL )  {  Print( Def_PrintLV1, "CNetSrv_MasterServer_Child::On_RoleDataCount" , "���쨤���Ƨ���. ���b�s�u�b����Ʈw���䤣�� Client(%d)", iClientID ); return;  }

	//Print( Def_PrintLV1, "CNetSrv_MasterServer_Child::On_RoleDataCount" , "Account: %s pClientInfo->m_iRoleCount = %d", pClientInfo->m_sAccountName.c_str(), iRoleCount );


	//char szBuff[ 1024 ];
	//sprintf( szBuff, "\nDEBUG On_RoleDataCount() - %s", pClientInfo->m_sAccountName.c_str() );
	//OutputDebugString( szBuff );
	pClientInfo->m_AccountBase = AccountBase;


	if( pClientInfo->m_iRoleCount == -1 )
	{
		pClientInfo->m_iRoleCount = iRoleCount;

		
	}
	else
	{
		Print( Def_PrintLV1, "CNetSrv_MasterServer_Child::On_RoleDataCount" , "Cli(%d) pClientInfo->m_iRoleCount != -1 , ���Ц��쨤��ƶq", iClientID );
	}
	int iParallelZoneCount = Net_ServerList->GetParallelZoneCount();
	if( pClientInfo->m_iRoleCount == 0 )
	{
		On_RoleDataEnd( pClientInfo->m_iClientID );
		SendClientRoleDataEnd( pClientInfo->m_iClientID , iParallelZoneCount );
	}

	int iTimeOffset = 0;

	int	iTime	= TimeStr::Now_Value();
	iTimeOffset = iTime - pClientInfo->m_AccountBase.LastCheckTime;


	{
		if( pClientInfo->m_AccountBase.CheckFailedTimes >= 5 )
		{
			if( iTimeOffset > 60 * 60 )
			{
				pClientInfo->m_AccountBase.CheckFailedTimes = 0;
			}
			else
			{
				LoadDataEvent( pClientInfo->m_iClientID, EM_LoadDataEvent_Lock, iTimeOffset );
				pClientInfo->m_bCheckSecondPasswordOK = false;
				return;
			}
		}
	}

	
	if( strlen( AccountBase.Password ) == 0 )
	{
		SC_SetPasswordRequest( iClientID );
		pClientInfo->m_bCheckSecondPasswordOK = true;
	}
	else if( Global::_CheckSecondPassword != false )
	{
		// �קK�L�׸߰�, �b���a���}�C����, �αN�ˬd�ɶ����]�����u�ɶ�, �ñN Result �M�� 0
		if( ( iTimeOffset < 180 && pClientInfo->m_AccountBase.CheckFailedTimes == 0 ) == false )
		{
			pClientInfo->m_bCheckSecondPasswordOK = false;

			//���ݭn�B�~�ˬd���ҽX - �ϥβĤG�جy�{
			if (Global::_CheckSecondPassword_ExtraCheck_Captcha != false)
			{
				SC_SecondPasswordRequest2( iClientID );
			}
			else
			{
				//�@��y�{
				SC_SecondPasswordRequest( iClientID );
			}			
		}
		else
			pClientInfo->m_bCheckSecondPasswordOK = true;
	}
	else
	{
		pClientInfo->m_bCheckSecondPasswordOK = true;
	}
	
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer_Child::On_LoadAccountFailed		( int iClientID, EM_LoadAccountFailed emErrID )
{

	// DEBUG
		

	ClientInfo* pClientInfo = NULL;
	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );

	if( pClientInfo == NULL )
	{
		return;
	}

	Print( Def_PrintLV3, "On_LoadAccountFailed()", "LoadAccountFailed - %d, %s", iClientID, pClientInfo->m_sAccountName.c_str() );

	if( pClientInfo->m_emClientStatus == EM_ClientStatus_ReloadingData )
	{
		pClientInfo->m_iLoadCount--;

		if( pClientInfo->m_iLoadCount == 0 )
		{
			// ��Ʈw���~, �Ө���@��������
			Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_LoadAccountFailed" , "���� Logout ���sŪ�������ƥ���! ( %s )", pClientInfo->m_sAccountName.c_str() );			
			LoadDataEvent( iClientID, EM_LoadDataEvent_Error, 0 );
			m_mapLoadQueue.erase( pClientInfo->m_sAccountName.c_str() );
			CheckWaitList();
			_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "On_LoadAccountFailed() ReloadingData Retry 3 time and failed" );	// �j��N Client Logout
		}
		else		
		{
			Schedular()->Push( CNetSrv_MasterServer_Child::OnEvent_WaitAccountReady, 3000, pClientInfo, NULL );
		}
	}

//	if( pClientInfo->m_emClientStatus == EM_ClientStatus_LoadingData )
	{
		if( emErrID == EM_LoadAccountFailed_Exist )
			LoadDataEvent( iClientID, EM_LoadDataEvent_RoleExist, 0 );
		else
			LoadDataEvent( iClientID, EM_LoadDataEvent_Error, 0 );

		// �]���|�N Client �n�X, �ҥH�N���ˬd�ƶ���C, ���� Client Logout �ɦA���

		// �ˬd�ƶ���C���L�ӱb��
		m_mapLoadQueue.erase( pClientInfo->m_sAccountName.c_str() );
		CheckWaitList();



		_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "On_LoadAccountFailed() DC Load Failed" );	// �j��N Client Logout
	}
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer_Child::On_DeleteRoleResult		( int iClientID, int iDBID, EM_DeleteRoleResult emResult )
{
	// Got result from datacenter
	//ClientInfo* pClientInfo = GetClientInfoByID( iClientID );

	ClientInfo* pClientInfo = NULL;
	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );

	if( pClientInfo == NULL ){	Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_DeleteRoleResult" , "�䤣�� ClientInfo by iClientID( %d ).", iClientID ); return; }


	Print( Def_PrintLV2, "On_DeleteRoleResult" , "DeleteRoleResult [%s][%d][%d]", pClientInfo->m_sAccountName.c_str(), iDBID , (DWORD)emResult  );

	

	switch( emResult ) 
	{
		case EM_DeleteRole_OK:
		{
			bool bResult = g_pClientManger->DelRoleToClientInfo( pClientInfo, iDBID );

			if( bResult != false )
			{
				// �i�D Client �R���F������
				DeleteRoleResult(  pClientInfo->m_iClientID, iDBID, EM_DeleteRole_OK );
			}
			else
			{
				// �䤣��n�R�������
				Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_DeleteRoleResult" , "�䤣��� DBID( %d ) �W��������. ClientInfo by Account( %s ).", iDBID, pClientInfo->m_sAccountName.c_str() );
				return;
			}

			
			
			/*
			// ��ڪ��N����R��
			map< int , RoleData* >::iterator it;
			if( ( it = pClientInfo->m_mapRoleData.find( iDBID ) ) == pClientInfo->m_mapRoleData.end() )
			{
				// �䤣��n�R�������
				Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_DeleteRoleResult" , "�䤣��� DBID( %d ) �W��������. ClientInfo by Account( %s ).", iDBID, pClientInfo->m_sAccountName.c_str() );
				return;
			}
			else
			{
				// ���n�R�������
				RoleData* pRole = it->second;
				//delete pRole;
				m_rRole.DeleteObj( pRole );
				pClientInfo->m_mapRoleData.erase( it );

				pClientInfo->m_iRoleCount--;

				// �i�D Client �R���F������
				DeleteRoleResult(  pClientInfo->m_iClientID, iDBID, EM_DeleteRole_OK );
			}
			//for( pClientInfo->m_mapRoleData.erase( iFieldIndex );		
			*/
		} break;
	}
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer_Child::On_CreateRoleResult		( int iClientID, EM_CreateRoleResult emResult, const char* pszRoleName, int iDBID , int GiftItemID[5] , int GiftItemCount[5] , int Job , int Job_Sub , int Lv , int Lv_Sub )
{
	// Got result from datacenter
	//ClientInfo* pClientInfo = GetClientInfoByID( iClientID );
	ClientInfo* pClientInfo = NULL;

	pClientInfo	= g_pClientManger->GetClientInfoByID( iClientID );

	if( pClientInfo == NULL ){	Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_CreateRoleResult" , "�䤣�� ClientInfo by ClientID( %d ).", iClientID ); return; }

	if( pClientInfo->m_iRoleCount >= Global::_MaxCreateRoleCount )
		return;

	// �N��ƥ[�J�ӱb������Ƥ�, �çi�� Client ���G
	vector< PlayerRoleData* >::iterator it;
	vector< PlayerRoleData* >& vecCreatingRole	= pClientInfo->m_vecCreatingRole;
	PlayerRoleData*	pRole		= NULL;
	
	bool		bFoundRole	= false;

	it = vecCreatingRole.begin();
	while( it != vecCreatingRole.end() )
	{
		pRole = *it;
		if( strcmp( pszRoleName, pRole->BaseData.RoleName.Begin() ) == 0 )
		{
			it = vecCreatingRole.erase( it );
			bFoundRole = true;
			break; 
		}

		it++;
	}

	ClearAccountCache( pClientInfo->m_sAccountName.c_str() );

	if( emResult == EM_CreateRoleResult_OK )
	{
		map< int , PlayerRoleData* > &mapRoleData		= pClientInfo->m_mapRoleData;

		if( bFoundRole )
		{
			pClientInfo->m_iRoleCount++;
			pRole->Base.DBID	= iDBID;
			pRole->PlayerBaseData.DestroyTime = -1;
			pRole->PlayerBaseData.TitleSys.Init();

			// �N�H����Ʃ�J�Ȧs���b������w��
			mapRoleData[ iDBID ] = pRole;
			//////////////////////////////////////////////////////////////////////////
			//�ץ�����¾�~����
			 //int Job , int Job_Sub , int Lv , int Lv_Sub
			if( unsigned(Job) < EM_Max_Vocation )
			{
				pRole->BaseData.Voc = (Voc_ENUM)Job;
				pRole->PlayerBaseData.AbilityList[ Job ].Level = Lv;
				pRole->PlayerBaseData.AbilityList[ Job ].TotalTpExp = pRole->PlayerBaseData.AbilityList[ Job ].TpExp = RoleDataEx::GetLvTotalTP( Lv );
			}

			if( unsigned(Job_Sub) < EM_Max_Vocation )
			{
				pRole->BaseData.Voc_Sub = (Voc_ENUM)Job_Sub;
				pRole->PlayerBaseData.AbilityList[ Job_Sub ].Level = Lv_Sub;
				pRole->PlayerBaseData.AbilityList[ Job_Sub ].TotalTpExp = pRole->PlayerBaseData.AbilityList[ Job_Sub ].TpExp = RoleDataEx::GetLvTotalTP( Lv );
			}

			//////////////////////////////////////////////////////////////////////////
			//��§��
			for( int GiftPos = 0 ; GiftPos < 5 ; GiftPos++ )
			{
				GameObjDbStructEx* ItemDB = g_ObjectData->GetObj( GiftItemID[GiftPos] );	
				if( ItemDB == NULL )
					continue;
				
				if( ItemDB->IsKeyItem() )
				{
					((RoleDataEx*)pRole)->AddKeyItem( ItemDB->GUID );
				}
				else
				{
					for( int i = 0 ; i < 30 ; i++ )
					{
						if( pRole->PlayerBaseData.Body.ItemBase[i].OrgObjID != 0 )
							continue;

						pRole->PlayerBaseData.Body.ItemBase[i].CreateTime = 0;
						pRole->PlayerBaseData.Body.ItemBase[i].Quality = 100;
						pRole->PlayerBaseData.Body.ItemBase[i].OrgQuality = 100;
						pRole->PlayerBaseData.Body.ItemBase[i].PowerQuality = 10;
						pRole->PlayerBaseData.Body.ItemBase[i].Durable = ItemDB->Item.Durable * 100;
						pRole->PlayerBaseData.Body.ItemBase[i].Count = GiftItemCount[GiftPos];
						pRole->PlayerBaseData.Body.ItemBase[i].OrgObjID = ItemDB->GUID;
						pRole->PlayerBaseData.Body.ItemBase[i].ImageObjectID = ItemDB->GUID;
						break;
					}
				}						
			}
			//////////////////////////////////////////////////////////////////////////
		
			// ���X SampleRole
			SampleRoleData SampleRole;
			((RoleDataEx*)pRole)->GetSampleRoleData( &SampleRole );

			// �e�X SampleRoleData �� Client
			CreateRoleResult( pClientInfo->m_iClientID, iDBID, EM_CreateRoleResult_OK, &SampleRole , GiftItemID , GiftItemCount );


		}
		else
		{
			Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_CreateRoleResult " , "Account( %s ), �䤣�����ӫإߪ�������, DBID( %d ).", pClientInfo->m_sAccountName.c_str(), iDBID );
		}

		
	}
	else
	{
		SampleRoleData SampleRole;
		CreateRoleResult( pClientInfo->m_iClientID, 0, emResult, &SampleRole  , GiftItemID , GiftItemCount );
		
		if( pRole != NULL )
			g_pClientManger->DeleteRoleData( pRole );
	} 
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer_Child::On_CreateRole				( int iClientID, const char* pszAccount, int iFieldIndex, CreateRoleData* pCreateRoleData )
{
	// �q Client �L�ӭn�D�إߨ���
	//ClientInfo* pClientInfo = GetClientInfoByID( iClientID );

	ClientInfo* pClientInfo = NULL;
	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );

	if( pClientInfo == NULL ){	Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_CreateRole" , "�䤣�� ClientInfo by iClientID( %d ).", iClientID );	return;	}

	// ���b���O�_�@�P
	if( stricmp( pClientInfo->m_sAccountName.c_str(), pszAccount ) != 0 )
	{
		Print( Def_PrintLV3, "CNetSrv_MasterServer_Child::On_CreateRole" , "�b�����@�P" );
		return;
	}

	if( _NewAccountDisableCreateRole != false && pClientInfo->m_mapRoleData.size() == 0 )
	{
		CreateRoleResult( iClientID, 0, EM_CreateRoleResult_NewHandDisableCreateRole, NULL );
		return;
	}

	if( pClientInfo->m_mapRoleData.size() >=  Global::_MaxCreateRoleCount  )
	{
		CreateRoleResult( iClientID, 0, EM_CreateRoleResult_SystemError, NULL );
		return;
	}


	if( pCreateRoleData->SampleRole.RoleName.Size() == 0 )
	{
		//Print( Def_PrintLV3, "CNetSrv_MasterServer_Child::On_CreateRole" , "�S��J�W�r" );
		CreateRoleResult( iClientID, 0, EM_CreateRoleResult_EmptyRoleName, NULL );
		return;
	}

	std::wstring outStrName;
	CharacterNameRulesENUM  Ret = g_ObjectData->CheckCharacterNameRules( pCreateRoleData->SampleRole.RoleName , Global::_CountryType , outStrName );
	if( Ret != EM_CharacterNameRules_Rightful )
	{
		switch( Ret )
		{
		case EM_CharacterNameRules_Short:				// �W�٤ӵu,���X�k
			CreateRoleResult( iClientID, 0, EM_CreateRoleResult_NameToShort, NULL );
			break;
		case EM_CharacterNameRules_Long:				// �W�٤Ӫ�,���X�k
			CreateRoleResult( iClientID, 0, EM_CreateRoleResult_NameToLong, NULL );
			break;
		case EM_CharacterNameRules_Wrongful:			// �����~���Ÿ��Φr��
			CreateRoleResult( iClientID, 0, EM_CreateRoleResult_NameIsWrongful, NULL );
			break;
		case EM_CharacterNameRules_Special:				// �S��W�٤����
			CreateRoleResult( iClientID, 0, EM_CreateRoleResult_HasSpecialString, NULL );
			break;
		case EM_CharacterNameRules_BasicLetters:		// �u��ϥΰ򥻭^��r��
			CreateRoleResult( iClientID, 0, EM_CreateRoleResult_OnlyBasicLetters, NULL );
			break;
		case EM_CharacterNameRules_EndLetter:			// �����r�����~(',")
			CreateRoleResult( iClientID, 0, EM_CreateRoleResult_EndLetterError, NULL );
			break;
		}				
		return;
	}

	pCreateRoleData->SampleRole.RoleName = WCharToUtf8( outStrName.c_str() ).c_str();	

	GameObjDbStructEx* CreateJobDB = NULL;	
	for( int i = 0 ; ; i++)
	{
		CreateJobDB = g_ObjectData->GetObj( i + Def_ObjectClass_CreateRole );
		if( CreateJobDB == NULL )
			break;
		if( CreateJobDB->CreateRole.Job == pCreateRoleData->SampleRole.Voc )
		{
			if( TestBit( CreateJobDB->CreateRole.RaceLimit._Race , pCreateRoleData->SampleRole.Race ) != false )
				break;
		}
	}

	if( CreateJobDB == NULL )
	{
		Print( Def_PrintLV3, "CNetSrv_MasterServer_Child::On_CreateRole" , "�L�k�ئ�¾�~" );
		return;
	}

//	if( iFieldIndex == pClientInfo->m_iRoleCount )
	{
		// ���T, ���U���ˬd��ƦX�z��
		if( true )
		{
			// ��ƥ��T, �V DataCenter �n�D�إߨ���
			PlayerRoleData* pNewRole = g_pClientManger->CreateRoleData();

			// <�ݰ�> �إߨ�����
			{

				pNewRole->Init();

				// �]�w�򥻸��
					pNewRole->Base.Type			= EM_ObjectClass_Player;

					pNewRole->BaseData.RoleName = pCreateRoleData->SampleRole.RoleName;
					pNewRole->Base.Account_ID	= pClientInfo->m_sAccountName.c_str();

					//pNewRole->Base.CharField	= iFieldIndex;
					
					pNewRole->BaseData.RoomID				= 0;
					/*
					pNewRole->BaseData.ZoneID				= 1;

					pNewRole->SelfData.RevPos.X		= pNewRole->BaseData.Pos.X				= -4091;
					pNewRole->SelfData.RevPos.Y		= pNewRole->BaseData.Pos.Y				= 241;
					pNewRole->SelfData.RevPos.Z		= pNewRole->BaseData.Pos.Z				= -8209;
					pNewRole->SelfData.RevPos.Dir	= pNewRole->BaseData.Pos.Dir			= 36;
					pNewRole->SelfData.RevZoneID = 1;
*/
					pNewRole->SelfData.RevPos.X		= pNewRole->BaseData.Pos.X				= float(CreateJobDB->CreateRole.BornPos.X);
					pNewRole->SelfData.RevPos.Y		= pNewRole->BaseData.Pos.Y				= float(CreateJobDB->CreateRole.BornPos.Y);
					pNewRole->SelfData.RevPos.Z		= pNewRole->BaseData.Pos.Z				= float(CreateJobDB->CreateRole.BornPos.Z);
					pNewRole->SelfData.RevPos.Dir	= pNewRole->BaseData.Pos.Dir			= float(CreateJobDB->CreateRole.BornPos.Dir);
					pNewRole->SelfData.RevZoneID	= pNewRole->BaseData.ZoneID				= CreateJobDB->CreateRole.BornPos.ZoneID;

					
					//�]�˳�
					pNewRole->PlayerBaseData.Body.Count			= _MAX_BODY_COUNT_;
					pNewRole->PlayerBaseData.Bank.Count			= _MAX_BANK_COUNT_;


					pNewRole->SelfData.LiveTime				= -1;
					pNewRole->BaseData.PartyID				=-1;
					pNewRole->PlayerBaseData.HouseDBID			= -1;
					//pNewRole->PlayerBaseData.ShowEQ._Value		= -1;



					pNewRole->BaseData.Race					= pCreateRoleData->SampleRole.Race;
					if(pCreateRoleData->SampleRole.Sex == EM_Sex_Boy )
						pNewRole->BaseData.Sex					= EM_Sex_Boy;
					else
						pNewRole->BaseData.Sex					= EM_Sex_Girl;
					
					pNewRole->BaseData.Voc					= pCreateRoleData->SampleRole.Voc;
					pNewRole->BaseData.Voc_Sub				= EM_Vocation_None;
					pNewRole->BaseData.HP					= 200;
					pNewRole->BaseData.MP					= 200;
					pNewRole->PlayerBaseData.MaxLv			= 50;
					pNewRole->PlayerBaseData.VocCount		= 2;
					pNewRole->PlayerBaseData.BoxEnergy		= 0;
					pNewRole->PlayerBaseData.Body.Money		= 100;

					pNewRole->BaseData.Look.FaceID			= pCreateRoleData->SampleRole.iFaceID;
					pNewRole->BaseData.Look.HairID			= pCreateRoleData->SampleRole.iHairID;
					pNewRole->BaseData.Look.HairColor		= pCreateRoleData->SampleRole.iHairColor;
					pNewRole->BaseData.Look.BodyColor		= pCreateRoleData->SampleRole.iBodyColor;

					pNewRole->BaseData.ItemInfo.Init();
					pNewRole->BaseData.ItemInfo.OrgObjID	= pCreateRoleData->SampleRole.OrgObjID;


					memcpy( pNewRole->BaseData.Look.BoneScale, pCreateRoleData->SampleRole.BoneScale, sizeof(pNewRole->BaseData.Look.BoneScale) );
					// ��l�ƦU¾�~�ݩ�
					for( int i = EM_Vocation_GameMaster; i < EM_Max_Vocation; i++ )
					{
						pNewRole->PlayerBaseData.AbilityList[ i ].STR = 0;
						pNewRole->PlayerBaseData.AbilityList[ i ].STA = 0;
						pNewRole->PlayerBaseData.AbilityList[ i ].MND = 0;
						pNewRole->PlayerBaseData.AbilityList[ i ].INT = 0;
						pNewRole->PlayerBaseData.AbilityList[ i ].AGI = 0;
						pNewRole->PlayerBaseData.AbilityList[ i ].EXP = 0;
						pNewRole->PlayerBaseData.AbilityList[ i ].Level = 0;
						pNewRole->PlayerBaseData.AbilityList[ i ].Point = 0;
						pNewRole->PlayerBaseData.AbilityList[ i ].PlayTime = 0;				
					}

					pNewRole->PlayerBaseData.AbilityList[ pCreateRoleData->SampleRole.Voc ].Level = 1;
			}

			pNewRole->BaseData.EQ.MainHand = GetItemStruct( 210020, 1 );

			for( int i = 0 ; i < 18 ; i++ )
			{
				pNewRole->BaseData.EQ.Item[i].Init();

				GameObjDbStructEx* ItemDB = g_ObjectData->GetObj( CreateJobDB->CreateRole.EQ.ID[i]);	
				if( ItemDB == NULL )
					continue;

				pNewRole->BaseData.EQ.Item[i].CreateTime = TimeStr::Now_Value();
				pNewRole->BaseData.EQ.Item[i].OrgQuality = 100;
				pNewRole->BaseData.EQ.Item[i].Quality = 100;
				pNewRole->BaseData.EQ.Item[i].PowerQuality = 10;
				pNewRole->BaseData.EQ.Item[i].Durable = ItemDB->Item.Durable * 100;
				pNewRole->BaseData.EQ.Item[i].Count = ItemDB->MaxHeap;
				pNewRole->BaseData.EQ.Item[i].OrgObjID = ItemDB->GUID;
				pNewRole->BaseData.EQ.Item[i].ImageObjectID = ItemDB->GUID;
			}


			for( int i = 0 , PackID = 0 ; i < 10 ; i++ )
			{
				GameObjDbStructEx* ItemDB = g_ObjectData->GetObj( CreateJobDB->CreateRole.ItemID[i] );	
				if( ItemDB == NULL )
					continue;

				if( ItemDB->IsKeyItem() )
				{
					((RoleDataEx*)pNewRole)->AddKeyItem( ItemDB->GUID );
					continue;
				}				

				pNewRole->PlayerBaseData.Body.ItemBase[PackID].CreateTime = TimeStr::Now_Value();
				pNewRole->PlayerBaseData.Body.ItemBase[PackID].Quality = 100;
				pNewRole->PlayerBaseData.Body.ItemBase[PackID].OrgQuality = 100;
				pNewRole->PlayerBaseData.Body.ItemBase[PackID].PowerQuality = 10;
				pNewRole->PlayerBaseData.Body.ItemBase[PackID].Durable = ItemDB->Item.Durable * 100;
				pNewRole->PlayerBaseData.Body.ItemBase[PackID].Count = __min( CreateJobDB->CreateRole.ItemCount[i] , ItemDB->MaxHeap );
				pNewRole->PlayerBaseData.Body.ItemBase[PackID].OrgObjID = ItemDB->GUID;
				pNewRole->PlayerBaseData.Body.ItemBase[PackID].ImageObjectID = ItemDB->GUID;
				PackID++;
			}

			pClientInfo->m_iLastRespond		= GetTickCount();


			pNewRole->PlayerBaseData.CoolSuitList[0].Init();
			pNewRole->PlayerBaseData.CoolSuitID = 0;
			//////////////////////////////////////////////////////////////////////////
			//�����@�θ��
			pNewRole->PlayerBaseData.Body.Money_Account		= pClientInfo->m_AccountBase.AccountMoney;
			pNewRole->PlayerBaseData.Money_Bonus			= pClientInfo->m_AccountBase.BonusMoney;
			pNewRole->PlayerBaseData.Password				= pClientInfo->m_AccountBase.Password;
			pNewRole->PlayerBaseData.PlayTimeQuota			= pClientInfo->m_AccountBase.PlayTimeQuota;
			pNewRole->PlayerBaseData.ManageLV				= (ManagementENUM)pClientInfo->m_AccountBase.ManageLv;
			pNewRole->PlayerBaseData.LockAccountTime		= pClientInfo->m_AccountBase.LockAccountTime;
			memcpy( pNewRole->PlayerBaseData.LockAccountMoney , pClientInfo->m_AccountBase.LockAccountMoney , sizeof(pNewRole->PlayerBaseData.LockAccountMoney) );
			pNewRole->PlayerBaseData.LockAccountMoney_Forever = pClientInfo->m_AccountBase.LockAccountMoney_Forever;
			pNewRole->Base.AccountFlag						= pClientInfo->m_AccountBase.AccountFlag;
			pNewRole->PlayerBaseData.VipMember.Type			= pClientInfo->m_AccountBase.VipMemberType;
			pNewRole->PlayerBaseData.VipMember.Term			= pClientInfo->m_AccountBase.VipMemberTerm;
			pNewRole->PlayerBaseData.VipMember._Flag		= pClientInfo->m_AccountBase.VipMemberFlag;
			pNewRole->PlayerBaseData.VipMember.UntrustFlag		= pClientInfo->m_AccountBase.UntrustFlag;

			memcpy( pNewRole->PlayerBaseData.CoolClothList , pClientInfo->m_AccountBase.CoolClothList , sizeof( pNewRole->PlayerBaseData.CoolClothList  ) );
			//////////////////////////////////////////////////////////////////////////
	
//			pNewRole->BaseData.PlayTimeQuota = pClientInfo->m_iPlayTimeQuota;
			// ���o�����ƫ�
			pClientInfo->m_vecCreatingRole.push_back( pNewRole );

			// Ask DataCenter to create role
			CreateRole( GetPCenterID( pClientInfo->m_sAccountName.c_str()  ) , iClientID, pClientInfo->m_sAccountName.c_str(), pNewRole );
		}
		else
		{
			// ��Ƥ����T
			// �N���a�_�u?
			Print( Def_PrintLV3, "CNetSrv_MasterServer_Child::On_CreateRole" , "������إ߸�Ƥ��X�k. Account( %s )", pClientInfo->m_sAccountName.c_str() );
			_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "On_CreateRole(), Check CreateData illegal" );
		}
	}
	/*
	else
	{
		Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_CreateRole" , "Field in use. iClientID( %d ) �إߨ��� .", iClientID );
		
		// �j��N Client Logout
		_Net->CliLogout( iClientID, "On_CreateRole(), Create data filed index illegal" );
		
		return;
	}
	*/

}
//-------------------------------------------------------------------
void CNetSrv_MasterServer_Child::On_SelectRole				( int iClientID, const char* pszAccount, int iDBID , int ParallelZoneID , bool EnterZone1 )
{
	//ClientInfo* pClientInfo = GetClientInfoByID( iClientID );
	ClientInfo* pClientInfo = NULL;

	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );

	if( pClientInfo == NULL )
	{	
		Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_SelectRole" , "�䤣�� ClientInfo by iClientID( %d ).", iClientID ); 
		return;	
	}
	
	if( stricmp( pClientInfo->m_sAccountName.c_str(), pszAccount ) != 0 )
	{
		Print( Def_PrintLV3, "CNetSrv_MasterServer_Child::On_SelectRole" , "�b�����@�P" );	
		_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "On_SelectRole() account doest match " );	// �j���_�u
		return;
	}

	if( pClientInfo->m_iSelectedRoleDBID != 0 )	//if( pClientInfo->m_pSelectedRole != NULL )
	{
		Print( Def_PrintLV3, "CNetSrv_MasterServer_Child::On_SelectRole" , "���⥿�b�i�J�@�ɤ�, �T�S����t�@�n�J�n�D" );	
		_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "On_SelectRole() Loading role but got another select request" );	// �j���_�u
		return;
	}


	
	map< int , PlayerRoleData* >::iterator itRole = pClientInfo->m_mapRoleData.find( iDBID );
	PlayerRoleData* pRole = NULL;

	if( itRole == pClientInfo->m_mapRoleData.end() )
	{
		char szErr[128];
		sprintf( szErr, "On_SelectRole() cant find role by DBID( %d )", iDBID );
		Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_SelectRole" , szErr );
		_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), szErr );	// �j���_�u
	}
	else
	{
		pRole = itRole->second;

		// Check if we are shutdowning server
		if( CNetMaster_ZoneStatus_Child::m_pThis->m_iShutdownMin <= 5 )
		{
			SelectRoleResult( iClientID, pRole->Base.DBID, EM_SelectRole_ServerShutdown_ );
			return;
		}


		if( pClientInfo->m_AccountBase.CheckFailedTimes >= 5 )
		{
			int	iTime	= TimeStr::Now_Value();
			int iOffset = abs( iTime - pClientInfo->m_AccountBase.LastCheckTime );
			if( iOffset > 60 * 60 )
			{
				pClientInfo->m_AccountBase.CheckFailedTimes = 0;
			}

			if( pClientInfo->m_AccountBase.CheckFailedTimes >= 5 )
			{
				// �ˬd�ɶ�
				// �e�X�]���s��{�ҥ���, �ٻݭn�h�[�~�୫�n���T��
				LoadDataEvent( pClientInfo->m_iClientID, EM_LoadDataEvent_Lock, iOffset );
				pClientInfo->m_bCheckSecondPasswordOK = false;
				return;
			}
		}

		if( pRole->PlayerBaseData.Password.Size() == 0 )
		{
			SelectRoleResult( iClientID, pRole->Base.DBID, EM_SelectRole_NoSecondPassword_ );
			return;
		}

		if( pRole->BaseData.SysFlag.MoneySerialCodeError  )
		{			
			return;
		}


		if( pRole->BaseData.SysFlag.ChangeNameEnabled )
		{
			SelectRoleResult( iClientID, pRole->Base.DBID, EM_SelectRole_NeedChangeRoleName_ );
			return;
		}

		if( pClientInfo->m_bCheckSecondPasswordOK == false)
		{
			SelectRoleResult( iClientID, pRole->Base.DBID, EM_SelectRole_SecondPasswordErr_ );
			return;
		}


		if( pRole->PlayerBaseData.DestroyTime != -1 )
		{
			char szErr[128];
			sprintf( szErr, "On_SelectRole() OnDelete Role", iDBID );
			Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_SelectRole" , szErr );
			_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), szErr );	// �j���_�u
		}
		else
		{
			pClientInfo->m_emClientStatus		= EM_ClientStatus_WaitConnectZone;
			pClientInfo->m_iSelectedRoleDBID  	= pRole->Base.DBID;				//pClientInfo->m_pSelectedRole	= pRole;
			pClientInfo->m_iLastRespond			= GetTickCount();

			 
			SetRoleToWorld( iClientID, pRole , ParallelZoneID , EnterZone1 );
		}
	}
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer_Child::On_DeleteRole				( int iClientID, const char* pszAccount, int iDBID , bool IsImmediately , char* Password )
{
	// �q Client �L�ӭn�D�R������
	//ClientInfo* pClientInfo = GetClientInfoByID( iClientID );

	ClearAccountCache( pszAccount );

	ClientInfo* pClientInfo = NULL;

	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );

	if( pClientInfo == NULL ){	Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_DeleteRole" , "�䤣�� ClientInfo by iClientID( %d ).", iClientID );	return;	}

	if( stricmp( pClientInfo->m_sAccountName.c_str(), pszAccount ) != 0 )
	{
		Print( Def_PrintLV3, "CNetSrv_MasterServer_Child::On_DeleteRole" , "�b�����@�P" );	
		_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "On_DeleteRole account mismatch" );	// �j���_�u
		return;
	}
	
	MyMD5Class Md5;
	Md5.ComputeStringHash( Password );
	if( IsImmediately == false && stricmp( Md5.GetMd5Str() , pClientInfo->m_AccountBase.Password ) != 0 )
	{
		DeleteRoleResult( iClientID , iDBID , EM_DeletePasswordErr );
		//�K�X���~
		return;
	}



	// �������θ�Ƶ��c���T
	map< int , PlayerRoleData* >::iterator it;
	if( ( it = pClientInfo->m_mapRoleData.find( iDBID ) ) != pClientInfo->m_mapRoleData.end() )
	{
		RoleData* pRole = it->second;

		pClientInfo->m_iLastRespond		= GetTickCount();

		// ��ƥ��T, �V DataCenter �n�D�R������
		DeleteRole( GetPCenterID( pClientInfo->m_sAccountName.c_str()  ) , iClientID, pClientInfo->m_sAccountName.c_str(), pRole->BaseData.RoleName.Begin() ,iDBID , IsImmediately );

		Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_DeleteRole" , "DeleteRole [%s][%s][%d][%d]", pClientInfo->m_sAccountName.c_str(), pRole->BaseData.RoleName.Begin() ,iDBID , (DWORD)IsImmediately  );
	}
	else
	{
		Print( Def_PrintLV3, "CNetSrv_MasterServer_Child::On_DeleteRole" , "iClientID( %d ) �R������, ���o�{�� DBID( %d ) ���⤣�s�b.", iClientID, iDBID );

		// �j��N Client Logout
		_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "On_DeleteRole(), Cant find role by DBID" );
		return;
	}
}
//-------------------------------------------------------------------
/*
ClientInfo* CNetSrv_MasterServer_Child::GetClientInfo( int iClientID )
{
	map< int	, ClientInfo* >::iterator it;
	if( ( it = m_mapClientInfo.find( iClientID ) ) == m_mapClientInfo.end() )
	{
		//Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::GetClientInfo() �䤣�� ClientInfo by iClientID( %d ).", iClientID );
		return NULL;
	}
	else
	{
		return it->second;
	}
}
*/
//-------------------------------------------------------------------
void CNetSrv_MasterServer_Child::On_SetRoleToWorldResult( int iServerID, int iClientID, int iDBID, EM_SelectRoleResult emResult )
{
	//ClientInfo* pClientInfo = GetClientInfoByID( iClientID );

	ClientInfo* pClientInfo = NULL;

	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );

	if( pClientInfo == NULL ){	Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_SetRoleToWorldResult" , "�䤣�� ClientInfo by iClientID( %d ).", iClientID );	return;	}

	// �i�� Client ���G
	if( emResult == EM_SelectRole_OK )
	{
		//////////////////////////////////////////////////////////////////////////
		//�⦹�b���Ҧ���L������n�X
		{
			map< int , PlayerRoleData* >::iterator Iter;
			//for( unsigned i = 0 ; i < pClientInfo->m_mapRoleData.size(); i++ )
			for( Iter = pClientInfo->m_mapRoleData.begin() ; Iter != pClientInfo->m_mapRoleData.end() ; Iter++ )
			{
				if( Iter->second->Base.DBID == iDBID )
					continue;
				Net_DCBase::SetPlayerLiveTime( GetPCenterID( pClientInfo->m_sAccountName.c_str()  ) , Iter->second->Base.DBID , 0 );
			}

		}
		//////////////////////////////////////////////////////////////////////////

		g_pClientManger->SetClientInfoByDBID( iDBID, pClientInfo );

		// ���X���e�Ȧs��������
		map< int , PlayerRoleData* >::iterator it;
		if( ( it = pClientInfo->m_mapRoleData.find( iDBID ) ) !=  pClientInfo->m_mapRoleData.end() )
		{
			pClientInfo->m_iSelectedRoleDBID = iDBID; //pClientInfo->m_pSelectedRole	= it->second;

			// Clear cache data
			g_pClientManger->ClearRoleFromClientInfo( pClientInfo );
		}
		else
		{
			Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_SetRoleToWorldResult" , "�䤣�� RoleData, DBID(%d).  iClientID(%d).", iClientID, iDBID );	
			_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "On_SetRoleToWorldResult, cant find role by dbid" );
			return;
		}

		// �i�J�@�ɤF

			SelectRoleResult( iClientID, iDBID , EM_SelectRole_OK );
			
			_Net->ConnectCliAndSrv( iServerID, iClientID );

		//PG_Login_M2C_EnterWorld
		// �q�� Client �L�i�J�@�ɤF
			SendEnterWorld( iClientID );

		char szNetID[256];

		sprintf( szNetID, "%d", iClientID );

		//	�q�� 3rd �{��, ����i�J�C��
		//-------------------------------------------------------------
			//m_APEX.ApexClientLogin( iClientID, pClientInfo->m_pSelectedRole->BaseData.RoleName.Begin(), pClientInfo->m_sAccountName.c_str() );


		// �M�� Account Cache ���
		//-------------------------------------------------------------		
			ClearAccountCache( pClientInfo->m_sAccountName.c_str() );


		// �o�e Vivox �b����Ƶ� Client
		//-------------------------------------------------------------
			//SendVivox
	}
	else
	{
		// Failed
		SelectRoleResult( iClientID, iDBID, emResult );
	}
}

ItemFieldStruct CNetSrv_MasterServer_Child::GetItemStruct( int iObjID, int iCount )
{
	//GameObjDbStructEx*	pObjDB	= Global::GetObjDB( 210020 );

	//if( pObjDB )


	ItemFieldStruct Item;

	Item.Init();

	Item.Quality = 100;
	Item.OrgQuality = 100;
	Item.PowerQuality = 10;
	Item.OrgObjID	= iObjID;
	Item.Durable	= 5000;//pObjDB->Item.Durable;
	Item.Count		= iCount;

	return Item;
}

int CNetSrv_MasterServer_Child::OnEvent_WaitDeleteRoleData	( SchedularInfo* pInfo , PVOID pContext )
{
	ClientInfo* pClientInfo = (ClientInfo*)pContext;

	if( pClientInfo == NULL )
		return 0;

	if( pClientInfo->m_bOnline != false )
	{
		return 0;
	}


	if( pClientInfo->m_emClientStatus != EM_ClientStatus_WaitDelete )
	{
		Print( Def_PrintLV2, "Login::OnEvent_WaitRoleLeaveWorld" , "�R����ƩR�O���ҥ~���p!" );

		// �]���S������ Game Server Leave World ���ʥ], �G�n�D DataCenter �s��

		if( pClientInfo->m_sAccountName.size() != 0 )
		{
			char szAccount[64];
			MyStrcpy( szAccount, pClientInfo->m_sAccountName.c_str(), sizeof( szAccount ) );

			Net_DCBaseChild::SetPlayerLiveTime_ByAccount( GetPCenterID( szAccount ) , szAccount, 60000 );
		}
	}

	// �ˬd��ƬO�_�����R��
		if( pClientInfo->m_iClientID != -1 )
		{
			g_pClientManger->EraseClientInfoByID( pClientInfo->m_iClientID );
			pClientInfo->m_iClientID = -1;
		}

		if( pClientInfo->m_sAccountName.size() != 0 )
		{
			g_pClientManger->EraseClientInfoByAccount( pClientInfo->m_sAccountName );
			pClientInfo->m_sAccountName = "";
		}

		if( pClientInfo->m_iSelectedRoleDBID != 0 ) //if( pClientInfo->m_pSelectedRole != NULL )
		{
			g_pClientManger->EraseClientInfoByDBID( pClientInfo->m_iSelectedRoleDBID ); //g_pClientManger->EraseClientInfoByDBID( pClientInfo->m_pSelectedRole->Base.DBID );	// �N��q DBID MAP ���R��
			pClientInfo->m_iSelectedRoleDBID = NULL; //pClientInfo->m_pSelectedRole = NULL;
			for( map< int , PlayerRoleData* >::iterator it = pClientInfo->m_mapRoleData.begin(); it != pClientInfo->m_mapRoleData.end(); it++ )
			{
				g_pClientManger->m_rRole.DeleteObj( it->second );
			}
			pClientInfo->m_mapRoleData.clear();
	
		}

	// �M�����
		g_pClientManger->DeleteClientInfo( pClientInfo );

	// �ˬd���L���ݳs�u���H
		((CNetSrv_MasterServer_Child*)m_pThis)->CheckWaitList();

	return 0;
}
//-------------------------------------------------------------------------------------------------
int CNetSrv_MasterServer_Child::OnEvent_WaitAccountReady	( SchedularInfo* pInfo , PVOID pContext )
{
	ClientInfo* pClientInfo = (ClientInfo*)pContext;

	// �o�X Load Account �n�D�� DataCenter
	if( pClientInfo->m_iLoadCount != 0 && pClientInfo->m_iClientID != -1 && pClientInfo->m_sAccountName.size() != 0 )
	{
		((CNetSrv_MasterServer_Child*)m_pThis)->LoadAccount( pClientInfo->m_iClientID, pClientInfo->m_sAccountName );
		LoadDataEvent( pClientInfo->m_iClientID, EM_LoadDataEvent_StartLoad, 0 );

		pClientInfo->m_iLoadCount++;
	}
	
	return 0;
}
//-------------------------------------------------------------------------------------------------
void CNetSrv_MasterServer_Child::On_RoleDataEnd( int iClientID )
{
	// �N���X���
	ClientInfo* pClientInfo = NULL;
	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );

	if( pClientInfo != NULL )
	{
		map< string, DWORD >::iterator it = m_mapLoadQueue.find( pClientInfo->m_sAccountName.c_str() );
		
		if( it != m_mapLoadQueue.end() )
		{
			m_mapLoadQueue.erase( it );
		}

		pClientInfo->m_bLoadedRole	= true;

		g_pClientManger->m_iLoadedRole++;
	}



	//char szBuff[ 1024 ];
	//printf( szBuff, "\nDEBUG On_RoleDataEnd() - %s", pClientInfo->m_sAccountName.c_str() );
	//OutputDebugString( szBuff );


	// �ˬd���S���Ƶ{, ������, �I�s�B�z��
	CheckWaitList();
}
//-------------------------------------------------------------------------------------------------
bool CNetSrv_MasterServer_Child::CheckWaitList ()
{
	// �ˬd���S���Ƶ{, ������
	ClientInfo* pClientInfo = NULL;
	while( 1 )
	{
		int iActiveSize	= g_pClientManger->GetActive();


		bool bLoadDirect = false;

		if( _EnableLimit == 1 || _EnableObjWait == 1 )
		{
			if( iActiveSize < _MaxLimitPlayerCount )
			{
				bLoadDirect = true;
			}

			if( bLoadDirect != false )
			{
				if( _MaxObjCount <=  m_iObjCount )
					bLoadDirect = false;
			}
		}
		else
		{
			bLoadDirect = true;
		}


		if( m_vecWaitQueue.size() != 0 && bLoadDirect != false )
		{			
			string sAccount = *(m_vecWaitQueue.begin());
			m_vecWaitQueue.pop_front( );

			// �ѱb�����X���, �ä�� ID �ˬd��̬O�_�Q�ŦX

			pClientInfo = g_pClientManger->GetClientInfoByAccount( sAccount );
			//pClientInfo = g_pClientManger->GetClientInfoByID( iWaitClientID );

			if( pClientInfo != NULL && pClientInfo->m_iClientID != -1 )
			{
				ClientInfo* pClientInfoByID = g_pClientManger->GetClientInfoByID( pClientInfo->m_iClientID );

				if( pClientInfoByID != NULL && pClientInfoByID == pClientInfo )
				{
					//m_iNumLoading++;
					pClientInfo->m_emClientStatus = EM_ClientStatus_LoadingData;
					m_mapLoadQueue[ pClientInfo->m_sAccountName ] = GetTickCount() + DF_TIME_CHECK_LOADACCOUNT_TIME;

					LoadAccount( pClientInfo->m_iClientID, sAccount );
					LoadDataEvent( pClientInfo->m_iClientID, EM_LoadDataEvent_StartLoad, 0 );

					return true;
				}
			}
		}
		else
			return false;
	}


	return false;
}
//-------------------------------------------------------------------------------------------------
int CNetSrv_MasterServer_Child::OnEvent_WaitLoadAccount( SchedularInfo* pInfo , PVOID pContext )
{

	CNetSrv_MasterServer_Child* p = (CNetSrv_MasterServer_Child*)pContext;

	int			iActiveSize	= g_pClientManger->GetActive();

	ClientInfo* pClientInfo = NULL;
	DWORD		dwTimeNow	= GetTickCount();

	// �ˬd���LŪ���b���W�L�ɶ��� Account, ���ˬd Account �O�_�s��

	vector< string > vecEraseAccount;

	// �ˬd LoadQueue �����b���O�_�s��
		for( map< string, DWORD >::iterator it = p->m_mapLoadQueue.begin(); it != p->m_mapLoadQueue.end(); it++ )
		{
			pClientInfo = g_pClientManger->GetClientInfoByAccount( it->first );

			if( pClientInfo == NULL || pClientInfo->m_iClientID == -1 )
			{
				vecEraseAccount.push_back( it->first );
			}
			else
			{		
				if( dwTimeNow > it->second )
				{	
					vecEraseAccount.push_back( it->first );

					char szReason[1024];
					sprintf( szReason, "Master OnEvent_WaitLoadAccount LoadAcount donest respond. %s", pClientInfo->m_sAccountName.c_str() );

					_Net->CliLogout( pClientInfo->m_iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), szReason );
				}
			}
		}

		for( vector< string >::iterator it = vecEraseAccount.begin(); it != vecEraseAccount.end(); it++ )
		{
			p->m_mapLoadQueue.erase( *it );
		}

	// �ˬd�M�檬�p, ���ůʴN�ƶiŪ�Ϧ�C��
		while(1)
		{
			if( p->CheckWaitList() == false )
				break;
		}

	// �o�e�ʥ]����C�����C�� Client, �i���L�̥ثe�ƶ����p
		int iQueueID = 1;
		for( deque< string >::iterator it = p->m_vecWaitQueue.begin(); it != p->m_vecWaitQueue.end(); it++ )
		{
			pClientInfo = g_pClientManger->GetClientInfoByAccount( *it );
			if( pClientInfo != NULL && pClientInfo->m_iClientID != -1 )
			{
				LoadDataEvent( pClientInfo->m_iClientID , EM_LoadDataEvent_WaitingQueue, iQueueID );
				iQueueID++;
			}
		}

	Schedular()->Push( OnEvent_WaitLoadAccount, DF_TIME_CHECKWAITLIST, p, NULL );


	return 0;
}
//-------------------------------------------------------------------------------------------------
void CNetSrv_MasterServer_Child::OnEvent_DataSrvDisconnect	( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	CNetSrv_MasterServer_Child* p = (CNetSrv_MasterServer_Child*)m_pThis;

	if( ServerType == EM_SERVER_TYPE_DATACENTER )
	{
		// �N�Ƶ{�����b���Ԩ쵥�ݦ�C�����e��
		for( map< string, DWORD >::iterator it = p->m_mapLoadQueue.begin(); it != p->m_mapLoadQueue.end(); it++ )
		{
			p->m_vecWaitQueue.push_front( it->first );
		}
		
		p->m_mapLoadQueue.clear();
	}
}
//-------------------------------------------------------------------------------------------------
int	CNetSrv_MasterServer_Child::OnEvent_CheckAlive ( SchedularInfo* pInfo , PVOID pContext )
{
	int iTime = GetTickCount();

	for( map< string, ClientInfo* >::iterator it = g_pClientManger->m_mapAccount.begin(); it != g_pClientManger->m_mapAccount.end(); it++ )
	{
		ClientInfo*		pClient		= it->second;
		int				iRespond	= abs( iTime - pClient->m_iLastRespond );

		if( pClient != NULL && iRespond > DF_TIME_KICK_DEAD_ACCOUNT && pClient->m_setZone.empty() != false )
		{
			//_Net->CliLogout( pClient->m_iClientID, "OnEvent_CheckAlive() Click Selecting role over time!" );
			Net_DCBaseChild::SetPlayerLiveTime_ByAccount( GetPCenterID( pClient->m_sAccountName.c_str()  ) , (char*)pClient->m_sAccountName.c_str(), DF_TIME_CHECK_ACCOUNT_DBLIVE );
			Print( LV2 , "OnEvent_CheckAlive" , "OnEvent_CheckAlive() should kick account ( %s ), but i didnt" , pClient->m_sAccountName.c_str() );
		}
		else
		{
			if( pClient->m_setZone.empty() != false )
			{
				Net_DCBaseChild::SetPlayerLiveTime_ByAccount( GetPCenterID( pClient->m_sAccountName.c_str()  ) , (char*)pClient->m_sAccountName.c_str(), DF_TIME_CHECK_ACCOUNT_DBLIVE );
			}			
		}
	}

	Schedular()->Push( OnEvent_CheckAlive, DF_TIME_CHECK_ACCOUNT_LIVE, NULL, NULL );
	return 0;
}
//-------------------------------------------------------------------------------------------------
void CNetSrv_MasterServer_Child::On_DelRoleRecoverRequest	( int CliNetID , int ProxyID , int DBID )
{

	ClientInfo* pClientInfo = NULL;

	pClientInfo = g_pClientManger->GetClientInfoByID( CliNetID );

	if( pClientInfo == NULL )
	{	
		Print( Def_PrintLV2, "On_DelRoleRecoverRequest" , "�䤣�� ClientInfo by iClientID( %d ).", CliNetID ); 
		return;	
	}

	if( pClientInfo->m_iSelectedRoleDBID != 0 ) //if( pClientInfo->m_pSelectedRole != NULL )
	{
		return;
	}

	

	map< int , PlayerRoleData* >::iterator itRole = pClientInfo->m_mapRoleData.find( DBID );
	PlayerRoleData* pRole = NULL;

	if( itRole == pClientInfo->m_mapRoleData.end() )
	{
		Print( Def_PrintLV2, "On_DelRoleRecoverRequest" , "cant find role by DBID( %d )", DBID ); 
		_Net->CliLogout( CliNetID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "Master DBID Err" );	// �j���_�u
	}
	else
	{
		pRole = itRole->second;

		if( pRole->PlayerBaseData.DestroyTime == -1 )
		{
			Print( Def_PrintLV2, "On_DelRoleRecoverRequest" , "DelRoleRecover twice by DBID( %d )", DBID ); 
			_Net->CliLogout( CliNetID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "Master DelRoleRecover twice" );	// �j���_�u			
		}
		else
		{
			pClientInfo->m_iLastRespond		= GetTickCount();
			pRole->PlayerBaseData.DestroyTime		= -1;
			ClearAccountCache( pClientInfo->m_sAccountName.c_str() );
			DelRoleRecoverRequest( GetPCenterID(pClientInfo->m_sAccountName.c_str()) ,  CliNetID , ProxyID , DBID );
		}

	}

}
//-------------------------------------------------------------------------------------------------
void	CNetSrv_MasterServer_Child::On_SetSecondPassword		( int CliNetID , char* Password )
{
	ClientInfo* pClientInfo = g_pClientManger->GetClientInfoByID( CliNetID );

	if( pClientInfo == NULL )
		return;

	//if( pClientInfo->m_mapRoleData.size() != 0 && strlen( pClientInfo->m_mapRoleData.begin()->second->PlayerBaseData.Password.Begin() ) != 0 )
	//	return;

	if( pClientInfo->m_mapRoleData.size() != 0 && strlen( pClientInfo->m_AccountBase.Password ) != 0 )
		return;



	MyMD5Class Md5;
	Md5.ComputeStringHash( Password );
	SD_SetSecondPassword( GetPCenterID( pClientInfo->m_sAccountName.c_str()  ) , pClientInfo->m_sAccountName.c_str() , Md5.GetMd5Str() );

	ClearAccountCache( pClientInfo->m_sAccountName.c_str() );
	
	map< int , PlayerRoleData* >::iterator Iter;

	for( Iter = pClientInfo->m_mapRoleData.begin() ; Iter != pClientInfo->m_mapRoleData.end() ; Iter++ )
	{
		Iter->second->PlayerBaseData.Password = Md5.GetMd5Str();
	}

	MyStrcpy( pClientInfo->m_AccountBase.Password , Md5.GetMd5Str() , sizeof(pClientInfo->m_AccountBase.Password) );
}
//-------------------------------------------------------------------------------------------------
void	CNetSrv_MasterServer_Child::On_ZoneLoadingRequest		( int CliNetID , int ZoneID )
{
	if( ZoneID > _DEF_ZONE_BASE_COUNT_ )
		return;

	vector<int>& ZoneCount = _PlayerZoneCount[ ZoneID ];

	ZoneLoading( CliNetID , ZoneID , _MaxZonePlayerCount , ZoneCount );
}
//-------------------------------------------------------------------------------------------------
void CNetSrv_MasterServer_Child::On_ApexDataFromClient( int CliNetID, int iSize, PVOID pData )
{
//	m_APEX.ApexRecvFromClient( CliNetID, (const char*) pData, iSize  );
}
//-------------------------------------------------------------------------------------------------
int CNetSrv_MasterServer_Child::OnEvent_CheckZonePlayer( SchedularInfo* pInfo , PVOID pContext )
{
	int iClientID	=	(int)pContext;

	ClientInfo* pClientInfo	= NULL;
	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );
	
	if( pClientInfo != NULL )
	{
		pClientInfo->m_iIDCheckZonePlayer = -1;

		if( pClientInfo->m_emClientStatus == EM_ClientStatus_WaitLastAccountLeaveZone )
		{
			for( set< int >::iterator it = pClientInfo->m_setZone.begin(); it != pClientInfo->m_setZone.end(); it++ )
			{
				// �o���� Zone Server, ��H

				int iLocalID	= *it; 
				int iSrvID		= Global::Net_ServerList->LocalID_To_SrvID( iLocalID );

				if( iSrvID != -1 )
				{
					CNetMaster_ZoneStatus_Child::KickZonePlayer( iSrvID, iLocalID, pClientInfo->m_sAccountName.c_str() );
				}
			}
		}
	}
	return 0;
}

void CNetSrv_MasterServer_Child::RC_ChangeNameRequest( int CliNetID , int DBID , char* NewRoleName )
{
	//���o������
	ClientInfo* pClientInfo	= g_pClientManger->GetClientInfoByID( CliNetID );

	

	if( pClientInfo == NULL )
	{
		SC_ChangeNameResult( CliNetID , DBID , NewRoleName , EM_ChangeNameResult_Failed );
		return;
	}

	ClearAccountCache( pClientInfo->m_sAccountName.c_str() );

	map< int , PlayerRoleData* >::iterator Iter = pClientInfo->m_mapRoleData.find( DBID );
	if( Iter == pClientInfo->m_mapRoleData.end() || Iter->second->BaseData.SysFlag.ChangeNameEnabled == false )
	{
		SC_ChangeNameResult( CliNetID , DBID , NewRoleName , EM_ChangeNameResult_Failed );
		return;
	}

	std::wstring outStrName;
	CharacterNameRulesENUM  Ret = g_ObjectData->CheckCharacterNameRules( NewRoleName , Global::_CountryType , outStrName );
	if( Ret != EM_CharacterNameRules_Rightful )
	{
		SC_ChangeNameResult( CliNetID , DBID , NewRoleName , EM_ChangeNameResult_NameErr );
		return;
	}


	SD_ChangeNameRequest( GetPCenterID( pClientInfo->m_sAccountName.c_str()  ) , CliNetID , DBID , (char*)WCharToUtf8( outStrName.c_str() ).c_str() );

	
}

void CNetSrv_MasterServer_Child::RD_ChangeNameResult ( int CliNetID , int DBID , char* NewRoleName , ChangeNameResultENUM Result )
{
	ClientInfo* pClientInfo	= g_pClientManger->GetClientInfoByID( CliNetID );

	if( pClientInfo == NULL )
		return;

	SC_ChangeNameResult( CliNetID , DBID , NewRoleName , Result );

	map< int , PlayerRoleData* >::iterator Iter = pClientInfo->m_mapRoleData.find( DBID );
	if( Iter == pClientInfo->m_mapRoleData.end() )
		return;

	PlayerRoleData* Role = Iter->second;

	if( Result != EM_ChangeNameResult_OK ) 
	{
		Role->BaseData.SysFlag.ChangeNameEnabled = true;
	}
	else
	{
		Role->BaseData.SysFlag.ChangeNameEnabled = false;
		Role->BaseData.RoleName = NewRoleName;
	}
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer_Child::RM_CloseMaster( )
{
	Global::_IsStopLogin = true;
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer_Child::LoadAccount( int iClientID, string sAccount )
{	


	ClientInfo* pClientInfo	= NULL;
	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );

	if( pClientInfo != NULL )
	{
		pClientInfo->m_iActive = 1;
		//g_pClientManger->AddActive();
		g_pClientManger->m_iActiveRole++;
		m_iObjCount += 3;	// �첤����ӥ�����, �ϥ��C 10 ��|�P�B�@���u�����
	}

	bool bLoadData = true;

	map< string, StructAccountRoleData* >::iterator it = m_mapAccountRoleData.find( sAccount.c_str() );

	if( it != m_mapAccountRoleData.end() )
	{
		StructAccountRoleData* pAccountData = it->second;

		if( pAccountData->bReady != false )
		{
			// �w�g��������������, �����o�e�� Client
			On_RoleDataCount( sAccount.c_str(), iClientID, pAccountData->iTotalRole, pAccountData->AccountBaseInfo, false );

			for( map< int, PlayerRoleData >::iterator itRole = pAccountData->mapRoleData.begin(); itRole != pAccountData->mapRoleData.end(); itRole++ )
			{
				On_RoleData( sAccount.c_str(), iClientID, itRole->first, &( itRole->second ), false );
			}
			return;
		}
		else
		{
			// �٥��q PlayerCenter �o�쨬�������, �B�z�n�J, ����
			// �o�����ӭn���ˬd����, Ū��@�b���H�N...
			delete pAccountData;
			m_mapAccountRoleData.erase( sAccount.c_str() );
		}
	}
	
	{
		// Craete Cache Data
		StructAccountRoleData* pAccountData = new StructAccountRoleData;

		pAccountData->iCreateTick			= GetTickCount();	// Delete after 5mins
		pAccountData->iLoadAccountClientID	= iClientID;

		m_mapAccountRoleData[ sAccount.c_str() ]	= pAccountData;		
	}

	Print( Def_PrintLV2, "LoadAccount" , "Send loadaccount request to playercenter [%s]", sAccount.c_str() );

	PG_Login_M2D_LoadAccountData ptLoadAccount;

	MyStrcpy( ptLoadAccount.szAccountName, sAccount.c_str(), sizeof( ptLoadAccount.szAccountName ) );
	ptLoadAccount.iClientID	= iClientID;

	Global::SendToRoleDBCenter( GetPCenterID(sAccount.c_str()) , sizeof( PG_Login_M2D_LoadAccountData ), &ptLoadAccount );
}
//-------------------------------------------------------------------
int CNetSrv_MasterServer_Child::OnEvent_CheckAccountCache( SchedularInfo* pInfo , PVOID pContext )
{
	CNetSrv_MasterServer_Child* pThis = (CNetSrv_MasterServer_Child*)pContext;

	DWORD dwTickCount = GetTickCount();

	map< string, StructAccountRoleData* >::iterator it = pThis->m_mapAccountRoleData.begin();
	while( it != pThis->m_mapAccountRoleData.end() )
	{
		StructAccountRoleData* pAccountData = it->second;

		DWORD dwOffset = abs( (int)dwTickCount - (int)pAccountData->iCreateTick );
		if( dwOffset > DF_TIME_DELETE_ACCOUNTCACHE )
		{
			ClientInfo* pClientInfo	= NULL;
			pClientInfo = g_pClientManger->GetClientInfoByAccount( it->first );

			if( pClientInfo == NULL )
			{
				Net_DCBaseChild::SetPlayerLiveTime_ByAccount( GetPCenterID( it->first.c_str() ) , (char*)it->first.c_str(), 0 );
			}

			it = pThis->m_mapAccountRoleData.erase( it );
			delete pAccountData;
		}
		else
			it++;
	}


	map< string, DWORD >::iterator itW =	pThis->m_mapWaitQueuePriority.begin();
	while( itW != pThis->m_mapWaitQueuePriority.end() )
	{
		DWORD dwDeleteTime = itW->second;
		DWORD dwOffset = abs( (int)dwTickCount - (int)dwDeleteTime );
		
		if( dwOffset > DF_TIME_KEEP_WAITQUEUEPRIORITY )
		{
			itW = pThis->m_mapWaitQueuePriority.erase( itW );
		}
		else
			itW++;
	}


	Schedular()->Push( OnEvent_CheckAccountCache,	DF_TIME_CHECK_ACCOUNTCACHE, pThis,	NULL );

	return 0;
}
//-------------------------------------------------------------------
bool CNetSrv_MasterServer_Child::CheckAccountCache ( const char* sAccount )
{
	if( m_mapAccountRoleData.find( sAccount ) != m_mapAccountRoleData.end() )
		return true;
	else
		return false;
}

void CNetSrv_MasterServer_Child::ClearAccountCache ( const char* sAccount )
{
	// �M�� Account Cache ���
	//-------------------------------------------------------------
	map< string, StructAccountRoleData* >::iterator itAccountCache = m_mapAccountRoleData.find( sAccount );

	if( itAccountCache != m_mapAccountRoleData.end() )
	{
		Print( Def_PrintLV2, "ClearAccountCache" , "Clear cache [%s]", sAccount );
		StructAccountRoleData* pAccountCache = itAccountCache->second;
		m_mapAccountRoleData.erase( itAccountCache );
		delete pAccountCache; 
	}
}

//////////////////////////////////////////////////////////////////////////
//�n�D�G���K�X
void CNetSrv_MasterServer_Child::RC_SecondPassword( int CliNetID , const char* Password )
{
	ClientInfo* pClientInfo	= NULL;
	pClientInfo = g_pClientManger->GetClientInfoByID( CliNetID );

	if( pClientInfo == NULL )
		return;

	if( pClientInfo->m_bCheckSecondPasswordOK != false )
	{
		LoadDataEvent( CliNetID, EM_LoadDataEvent_SPWCheckFailed, 0 );
		SC_SecondPasswordResult( CliNetID , true );
		return;
	}

	//�G���K�X�ˬd
	//MyMD5Class Md5;
	//Md5.ComputeStringHash( Password );


	StructAccountRoleData* pAccountData = NULL;

	map< string, StructAccountRoleData* >::iterator it = m_mapAccountRoleData.find( pClientInfo->m_sAccountName.c_str() );
	if( it != m_mapAccountRoleData.end() )
	{
		pAccountData = it->second;
	}

	if( stricmp( pClientInfo->m_AccountBase.Password , Password ) != 0 )
	{
		pClientInfo->m_AccountBase.CheckFailedTimes++;
		pClientInfo->m_AccountBase.LastCheckTime	= TimeStr::Now_Value();

		if( pAccountData != NULL )
		{
			pAccountData->AccountBaseInfo.CheckFailedTimes++;
			pAccountData->AccountBaseInfo.LastCheckTime	= TimeStr::Now_Value();
		}

		LoadDataEvent( CliNetID, EM_LoadDataEvent_SPWCheckFailed, pClientInfo->m_AccountBase.CheckFailedTimes );

		if( pClientInfo->m_AccountBase.CheckFailedTimes >= 5 )
			LoadDataEvent( CliNetID, EM_LoadDataEvent_Lock, 1 );

		SC_SecondPasswordResult( CliNetID , false );
		SD_SetCheckSecondPasswordResult( GetPCenterID(pClientInfo->m_sAccountName.c_str()) , CliNetID, pClientInfo->m_sAccountName.c_str(), pClientInfo->m_AccountBase.CheckFailedTimes );

		/*
		if( pClientInfo->m_AccountBase.CheckFailedTimes >= 5 )
		{
			_Net->CliLogout( CliNetID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "Second password 5th times error" );
		}
		*/

		return;
	}

	pClientInfo->m_AccountBase.CheckFailedTimes = 0;
	pClientInfo->m_AccountBase.LastCheckTime	= TimeStr::Now_Value();
	
	if( pAccountData != NULL )
	{
		pAccountData->AccountBaseInfo.CheckFailedTimes			= 0;
		pAccountData->AccountBaseInfo.LastCheckTime				= TimeStr::Now_Value();
	}


	SD_SetCheckSecondPasswordResult( GetPCenterID( pClientInfo->m_sAccountName.c_str()  ) , CliNetID, pClientInfo->m_sAccountName.c_str(), pClientInfo->m_AccountBase.CheckFailedTimes );

	LoadDataEvent( CliNetID, EM_LoadDataEvent_SPWCheckFailed, pClientInfo->m_AccountBase.CheckFailedTimes );
	SC_SecondPasswordResult( CliNetID , true );
	pClientInfo->m_bCheckSecondPasswordOK = true;
	return;
}

//-------------------------------------------------------------
void CNetSrv_MasterServer_Child::RC_SecondPassword2( int CliNetID , const char* Password)
{
	ClientInfo* pClientInfo	= g_pClientManger->GetClientInfoByID( CliNetID );

	if (pClientInfo != NULL)
	{
		//--------------------------------------------------
		if( pClientInfo->m_bCheckSecondPasswordOK != false )
		{
			LoadDataEvent( CliNetID, EM_LoadDataEvent_SPWCheckFailed, 0 );
			SC_SecondPasswordResult2( CliNetID , true );
		}
		else
		{
			//--------------------------------------------------	
			//check second password
			if( stricmp( pClientInfo->m_AccountBase.Password , Password ) != 0 )
			{
				//second password check failed
				SetSecondPasswordCheckFailedTimes(CliNetID, pClientInfo->m_AccountBase.CheckFailedTimes + 1);
				SC_SecondPasswordResult2( CliNetID , false );
			}
			//second password check ok then send 
			else
			{
				//send second password check result
				SC_SecondPasswordResult2( CliNetID , true );

				//send captcha check
				pClientInfo->m_CaptchaString = m_CaptchaGenerator->ResultString;
				pClientInfo->m_LastCaptchSendTime = TimeStr::Now_Value();
				SC_CaptchaCheck(CliNetID, m_CaptchaGenerator->RawImage, m_CaptchaGenerator->RawImageSize);
			}
			//--------------------------------------------------
		}
	}
}

//-------------------------------------------------------------
void CNetSrv_MasterServer_Child::RC_CaptchaRefresh( int CliNetID)
{
	ClientInfo* pClientInfo	= g_pClientManger->GetClientInfoByID( CliNetID );

	if (pClientInfo != NULL)
	{
		//resend captcha check
		pClientInfo->m_CaptchaString = m_CaptchaGenerator->ResultString;
		pClientInfo->m_LastCaptchSendTime = TimeStr::Now_Value();
		SC_CaptchaCheck(CliNetID, m_CaptchaGenerator->RawImage, m_CaptchaGenerator->RawImageSize);
	}
}

//-------------------------------------------------------------
void CNetSrv_MasterServer_Child::RC_CaptchaReply( int CliNetID , const char* Captcha)
{
	ClientInfo* pClientInfo	= g_pClientManger->GetClientInfoByID( CliNetID );

	if (pClientInfo != NULL)
	{
		int iTimeOffset = TimeStr::Now_Value() - pClientInfo->m_LastCaptchSendTime;

		if (iTimeOffset > 60)
		{
			SC_CaptchaCheckResult(CliNetID, EM_CaptchaCheckResult_TimeExpired);

			pClientInfo->m_CaptchaString = m_CaptchaGenerator->ResultString;
			pClientInfo->m_LastCaptchSendTime = TimeStr::Now_Value();
			SC_CaptchaCheck(CliNetID, m_CaptchaGenerator->RawImage, m_CaptchaGenerator->RawImageSize);
		}
		else
		{
			//Captcha compare
			if (_stricmp(pClientInfo->m_CaptchaString.c_str(), Captcha) == 0)
			{
				//check ok
				CheckSecondPassword2_Ok(CliNetID);
			}
			else
			{
				if ((m_CaptchaGenerator != NULL) && (m_CaptchaGenerator->Refresh() != false))
				{
					//check failed, refresh captcha and send again
					SetSecondPasswordCheckFailedTimes(CliNetID, pClientInfo->m_AccountBase.CheckFailedTimes + 1);

					SC_CaptchaCheckResult(CliNetID, EM_CaptchaCheckResult_Failed);

					pClientInfo->m_CaptchaString = m_CaptchaGenerator->ResultString;
					pClientInfo->m_LastCaptchSendTime = TimeStr::Now_Value();
					SC_CaptchaCheck(CliNetID, m_CaptchaGenerator->RawImage, m_CaptchaGenerator->RawImageSize);
				}
				else
				{
					//Captcha image is invalid, ignore captcha check
					CheckSecondPassword2_Ok(CliNetID);
				}
			}
		}
	}
}

//-------------------------------------------------------------
void CNetSrv_MasterServer_Child::SetSecondPasswordCheckFailedTimes( int CliNetID, int CheckFailedTimes)
{
	ClientInfo* pClientInfo	= g_pClientManger->GetClientInfoByID( CliNetID );

	if (pClientInfo != NULL)
	{
		//--------------------------------------------------
		StructAccountRoleData* pAccountData = NULL;

		map< string, StructAccountRoleData* >::iterator it = m_mapAccountRoleData.find( pClientInfo->m_sAccountName.c_str() );
		if( it != m_mapAccountRoleData.end() )
		{
			pAccountData = it->second;
		}
		//--------------------------------------------------

		//--------------------------------------------------
		pClientInfo->m_AccountBase.CheckFailedTimes = CheckFailedTimes;
		pClientInfo->m_AccountBase.LastCheckTime	= TimeStr::Now_Value();

		if( pAccountData != NULL )
		{
			pAccountData->AccountBaseInfo.CheckFailedTimes = CheckFailedTimes;
			pAccountData->AccountBaseInfo.LastCheckTime	= TimeStr::Now_Value();
		}

		LoadDataEvent( CliNetID, EM_LoadDataEvent_SPWCheckFailed, CheckFailedTimes );

		if( CheckFailedTimes >= 5 )
			LoadDataEvent( CliNetID, EM_LoadDataEvent_Lock, 1 );

		SD_SetCheckSecondPasswordResult( GetPCenterID( pClientInfo->m_sAccountName.c_str()  ) , CliNetID, pClientInfo->m_sAccountName.c_str(), CheckFailedTimes );
		//--------------------------------------------------
	}
}

//-------------------------------------------------------------
void CNetSrv_MasterServer_Child::CheckSecondPassword2_Ok(int CliNetID)
{	
	ClientInfo* pClientInfo	= g_pClientManger->GetClientInfoByID( CliNetID );

	if (pClientInfo != NULL)
	{
		SetSecondPasswordCheckFailedTimes(CliNetID, 0);

		SC_CaptchaCheckResult(CliNetID, EM_CaptchaCheckResult_OK);
		pClientInfo->m_bCheckSecondPasswordOK = true;
		pClientInfo->m_CaptchaString = "";
	}
}

//-------------------------------------------------------------
void CNetSrv_MasterServer_Child::OnGetObjCountResult( int iSrvID, int iCount )
{
	m_iObjCount = iCount;
}
//-------------------------------------------------------------
void CNetSrv_MasterServer_Child::OnGameGuardReport( int iClientID, int iType, int iSize, const char* pszReport )
{
	char		szBuff[4096];
	ClientInfo* pClientInfo	= NULL;

	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );

	if( pClientInfo == NULL )
		return;

	int			iZoneID		= 0;

	if( pClientInfo->m_setZone.size() != 0 )
	{
		iZoneID = *( pClientInfo->m_setZone.begin() );
	}



	sprintf( szBuff , "Insert GameGuardReport( Account, ZoneID, Type,iSize,Report) VALUES( '%s', %d, %d, %d, '%s')"
		, pClientInfo->m_sAccountName.c_str()
		, iZoneID
		, iType
		, iSize
		, pszReport
		);

	Net_DCBase::LogSqlCommand( szBuff );



}
//-------------------------------------------------------------
void CNetSrv_MasterServer_Child::GetVivoxPassword( const char* pszAccount, std::string& sPassword )
{
	char szEncode[9];

	MyMD5Class Md5;
	Md5.ComputeStringHash( pszAccount, "rom" );

	int iMaxLen = strlen( Md5.GetMd5Str() );
	if( iMaxLen > 8 )
	{
		memset( szEncode, 0 , sizeof( szEncode ) );
		strncpy( szEncode, Md5.GetMd5Str(), 8 );
	}

	sPassword = szEncode;
}
//-------------------------------------------------------------
void CNetSrv_MasterServer_Child::OnCreateVivoxAccount( int iClientID, const char* pszAccount )
{
	//ClientInfo* pClientInfo	= NULL;
	//pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );

	//if( pClientInfo == NULL || m_vivox.m_url.empty() != false)
	//	return;

	//char szFullAccount[256];
	//sprintf( szFullAccount, "%s%s", pClientInfo->m_sAccountName.c_str(), m_vivox.m_country.c_str() );


	//string EncodePassword;
	//GetVivoxPassword( szFullAccount, EncodePassword );
	//m_vivox.CreateAccount( pClientInfo->m_sAccountName.c_str(), EncodePassword.c_str() );
}
//-------------------------------------------------------------
int CNetSrv_MasterServer_Child::OnEvent_VivoxUpdate	( SchedularInfo* pInfo , PVOID pContext )
{

	/*m_pThis->m_vivox.Update();

	vector< StructVivoxEvent >			vectorResult;
	
	EnterCriticalSection( &m_pThis->m_vivox.m_csVivox );

	vectorResult = m_pThis->m_vivox.m_vectorResult;
	m_pThis->m_vivox.m_vectorResult.clear();


	LeaveCriticalSection( &m_pThis->m_vivox.m_csVivox );

	for( vector< StructVivoxEvent >::iterator it = vectorResult.begin(); it != vectorResult.end(); it++ )
	{
		if( (*it).emID == emVivoxEvent_CreateResult )
		{
			ClientInfo* pClientInfo	= NULL;
			pClientInfo = g_pClientManger->GetClientInfoByAccount( (*it).sAccount.c_str() );

			if( pClientInfo != NULL )
			{
				char szFullAccount[256];
				sprintf( szFullAccount, "%s%s", (*it).sAccount.c_str(), m_pThis->m_vivox.m_country.c_str() );

				SC_VivoxAccount( pClientInfo->m_iClientID, szFullAccount, (*it).sPassword.c_str(), true );
			}	
		}
		else
		if( (*it).emID == emVivoxEvent_Message )
		{
			Print( LV2, "Vivox", (*it).sMessage.c_str() );
		}
	}

	Schedular()->Push( OnEvent_VivoxUpdate,	100, m_pThis,	NULL );*/
	return 0;
}