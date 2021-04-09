#include "NetSrv_PublicEncounterChild.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CAreaEventObjective
////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void CNetSrv_PublicEncounterChild::Init()
{
	// 系統初始化
	CNetSrv_PublicEncounter::_Init();

	if( m_pThis != NULL)
		return;

	m_pThis = NEW( CNetSrv_PublicEncounterChild );
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void CNetSrv_PublicEncounterChild::Release()
{
	CNetSrv_PublicEncounter::_Release();

	if (m_pThis)
	{
		delete m_pThis;
	}

	m_pThis = NULL;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void CNetSrv_PublicEncounterChild::CliConnect( BaseItemObject* pObj, int iSockID )
{
	PublicEncounterManager::CliConnect(pObj, iSockID);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void CNetSrv_PublicEncounterChild::CliDisconnect( int iSockID )
{
	PublicEncounterManager::CliDisconnect(iSockID);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void CNetSrv_PublicEncounterChild::OnRZ_CliRegionChanged(BaseItemObject* pObj, int iRegionID)
{
	PublicEncounterManager::CliRegionChanged(pObj, iRegionID);
}