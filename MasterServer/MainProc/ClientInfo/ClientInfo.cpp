#include "ClientInfo.h"
#include "SmallObj/Smallobj.h"

CClientManger*				g_pClientManger = NULL;
// ----------------------------------------------------------------------------
ClientInfo*			CClientManger::GetClientInfoByID ( int iClientID		)
{
	map< int	, ClientInfo* >::iterator it;

	if( ( it = m_mapClientInfo.find( iClientID ) ) == m_mapClientInfo.end() )
	{
		//Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::GetClientInfo() 找不到 ClientInfo by iClientID( %d ).", iClientID );
		return NULL;
	}
	else
	{
		return it->second;
	}
}
// ----------------------------------------------------------------------------
ClientInfo* CClientManger::GetClientInfoByDBID		( int iDBID	)
{
	map< int,		ClientInfo* >::iterator it;

	if( ( it = m_mapDBID.find( iDBID ) ) == m_mapDBID.end() )
	{
		//Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::GetClientInfo() 找不到 ClientInfo by iClientID( %d ).", iClientID );
		return NULL;
	}
	else
	{
		return it->second;
	}
	
}
// ----------------------------------------------------------------------------
ClientInfo* CClientManger::GetClientInfoByAccount ( string sAccount	)
{
	map< string, ClientInfo* >::iterator it;

	if( ( it = m_mapAccount.find( sAccount ) ) == m_mapAccount.end() )
	{
		//Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::GetClientInfo() 找不到 ClientInfo by iClientID( %d ).", iClientID );
		return NULL;
	}
	else
	{
		return it->second;
	}
}
// ----------------------------------------------------------------------------
void CClientManger::SetClientInfoByID			( int iClientID,	ClientInfo* pClientInfo )
{
	m_mapClientInfo[ iClientID ]	= pClientInfo;
}
// ----------------------------------------------------------------------------
void CClientManger::SetClientInfoByDBID			( int iDBID,		ClientInfo* pClientInfo )
{
	m_mapDBID[ iDBID ]	= pClientInfo;
}
// ----------------------------------------------------------------------------
void CClientManger::SetClientInfoByAccount		( string sAccount,	ClientInfo* pClientInfo )
{
	m_mapAccount[ sAccount ]		= pClientInfo;
}
// ----------------------------------------------------------------------------
ClientInfo*	 CClientManger::CreateClientInfo ()
{
	ClientInfo* pClientInfo = m_rClientInfo.NewObj();

	//pClientInfo->m_sAccountName		= sAccount;
	//pClientInfo->m_iClientID		= iClientID;

	pClientInfo->Init();

	pClientInfo->m_iRoleCount			= -1;
	//pClientInfo->m_iZoneID			= -1;
	//pClientInfo->m_pSelectedRole		= NULL;
	pClientInfo->m_bEnterWorld			= false;
	pClientInfo->m_emClientStatus		= EM_ClientStatus_Init;
	pClientInfo->m_iLoadCount			= 0;
	pClientInfo->m_sAccountName			= "";
	pClientInfo->m_iIDCheckZonePlayer	= -1;
	pClientInfo->m_bPlayControl			= false;
	pClientInfo->m_iActive				= 0;

	
	pClientInfo->m_setZone.clear();


	//m_mapClientInfo[ iClientID ]	= pClientInfo;
	//m_mapAccount[ sAccount ]		= iClientID;

	return pClientInfo;
}
// ----------------------------------------------------------------------------
void CClientManger::DeleteClientInfo ( ClientInfo* pClientInfo )
{

	// 檢查資料是否完全刪除
	if( pClientInfo->m_iClientID != -1 )
	{
		EraseClientInfoByID( pClientInfo->m_iClientID );
		pClientInfo->m_iClientID = -1;
	}

	if( pClientInfo->m_sAccountName.size() != 0 )
	{
		EraseClientInfoByAccount( pClientInfo->m_sAccountName );
		pClientInfo->m_sAccountName = "";
	}

	if( pClientInfo->m_iSelectedRoleDBID != 0 ) //if( pClientInfo->m_pSelectedRole != NULL )
	{
		EraseClientInfoByDBID( pClientInfo->m_iSelectedRoleDBID );	// 將其從 DBID MAP 中刪除
		pClientInfo->m_iSelectedRoleDBID = 0; //pClientInfo->m_pSelectedRole = NULL;
	}


	map< int , PlayerRoleData* >::iterator it = pClientInfo->m_mapRoleData.begin();
	for( it = pClientInfo->m_mapRoleData.begin(); it != pClientInfo->m_mapRoleData.end(); it++ )
	{
		//delete it->second;
		m_rRole.DeleteObj( it->second );
	}
	
	pClientInfo->m_iIDCheckZonePlayer	= -1;
	
	pClientInfo->m_mapRoleData.clear();

	m_rClientInfo.DeleteObj( pClientInfo );
}
// ----------------------------------------------------------------------------
void CClientManger::ClearRoleFromClientInfo	( ClientInfo* pClientInfo )
{
	map< int , PlayerRoleData* >::iterator it = pClientInfo->m_mapRoleData.begin();
	for( it = pClientInfo->m_mapRoleData.begin(); it != pClientInfo->m_mapRoleData.end(); it++ )
	{
		//delete it->second;
		m_rRole.DeleteObj( it->second );
	}

	pClientInfo->m_mapRoleData.clear();
}

void CClientManger::AddRoleToClientInfo			( ClientInfo* pClientInfo, int iDBID, PlayerRoleData* pRole )
{
	map< int , PlayerRoleData* >::iterator Iter = pClientInfo->m_mapRoleData.find( iDBID );
	if( Iter != pClientInfo->m_mapRoleData.end() )
	{
		if( Iter->second != NULL )
		{
			Iter->second->Copy( pRole );
			return;
		}
	}

	PlayerRoleData* t_pRole = m_rRole.NewObj();		// 分派空間
	if( t_pRole == NULL )
	{
		Print( LV5 , "AddRoleToClientInfo" , "t_pRole == NULL" );
		return;
	}


//	*t_pRole = *pRole;
//	t_pRole->SetLink();
//	t_pRole->DynamicData.TreasureBox = NULL;
	t_pRole->Copy( pRole );


	pClientInfo->m_mapRoleData[ iDBID ] = t_pRole;

}
// ----------------------------------------------------------------------------
bool CClientManger::DelRoleToClientInfo			( ClientInfo* pClientInfo, int iDBID )
{
	// 實際的將角色刪除
	map< int , PlayerRoleData* >::iterator it;
	if( ( it = pClientInfo->m_mapRoleData.find( iDBID ) ) == pClientInfo->m_mapRoleData.end() )
	{
		// 找不到要刪除的資料
		//Print( Def_PrintLV2, "CNetSrv_MasterServer_Child::On_DeleteRoleResult" , "找不到該 DBID( %d ) 上的角色資料. ClientInfo by Account( %s ).", iDBID, pClientInfo->m_sAccountName.c_str() );
		return false;
	}
	else
	{

		
		// 找到要刪除的資料
		PlayerRoleData* pRole = it->second;
		pRole->PlayerBaseData.DestroyTime = TimeStr::Now_Value() + _DEL_ROLE_RESERVE_TIME_;
		/*
		//delete pRole;
		m_rRole.DeleteObj( pRole );
		pClientInfo->m_mapRoleData.erase( it );

		pClientInfo->m_iRoleCount--;
*/
		// 告訴 Client 刪除了角色資料
		//DeleteRoleResult(  pClientInfo->m_iClientID, iDBID, EM_DeleteRole_OK );
		return true;
	}
}
// ----------------------------------------------------------------------------
PlayerRoleData* CClientManger::CreateRoleData()
{
	return m_rRole.NewObj();
}
// ---------------------------------------------------------------------------
void CClientManger::DeleteRoleData( PlayerRoleData* pObj )
{
	m_rRole.DeleteObj( pObj );
}
// ---------------------------------------------------------------------------
void CClientManger::Init()
{
	if( g_pClientManger == NULL )
	{
		g_pClientManger = new CClientManger;
	}
}
// ----------------------------------------------------------------------------
void CClientManger::Release()
{
	if( g_pClientManger != NULL )
	{
		delete g_pClientManger;
		g_pClientManger = NULL;
	}
}
// ----------------------------------------------------------------------------
/*
bool CClientManger::RoleEnterWorld ( int iDBID, ClientInfo* pClientInfo )
{
	RoleData* pRole = NULL;
	map< int , RoleData* >::iterator it;
	if( ( it = pClientInfo->m_mapRoleData.find( iDBID ) ) !=  pClientInfo->m_mapRoleData.end() )
	{
		pRole = it->second;

		m_mapDBID[ iDBID ]				= pClientInfo;
		pClientInfo->m_pSelectedRole	= pRole;

		return true;
	}
	else
	{
		return false;
	}
}
*/
// ----------------------------------------------------------------------------
/*
bool CClientManger::RoleLeaveWorld ( int iDBID, ClientInfo* pClientInfo )
{
	map< int,		ClientInfo* >::iterator it;

	if( ( it = m_mapDBID.find( iDBID ) ) != m_mapDBID.end() )
	{
		m_mapDBID.erase( iDBID );
		pClientInfo->m_pSelectedRole	= NULL;
		return true;
	}
	else
	{
		return false;
	}
}
*/
