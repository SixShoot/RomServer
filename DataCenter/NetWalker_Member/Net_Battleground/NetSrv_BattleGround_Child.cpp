//#include "../Net_Talk/NetDC_Talk.h"

#include "NetSrv_BattleGround_Child.h"
/*
#include "../Net_ServerList/Net_ServerList_Child.h"
#include "../../mainproc/pathmanage/NPCMoveFlagManage.h"
#include "../Net_Gather/NetDC_Gather_Child.h"
#include "../../MainProc/partyinfolist/PartyInfoList.h"
*/
//-------------------------------------------------------------------
bool    CNetDC_BattleGround_Child::Init()
{
	CNetDC_BattleGround::Init();

	if( m_pThis != NULL)
		return false;

	//m_pThis = NEW( CNetDC_BattleGround_Child );

	return true;
}

//-------------------------------------------------------------------
bool    CNetDC_BattleGround_Child::Release()
{
	CNetDC_BattleGround::Release();

	if( m_pThis == NULL )
		return false;

	delete m_pThis;
	m_pThis = NULL;

	
	return false;
}
//-------------------------------------------------------------------
void CNetDC_BattleGround_Child::OnRZ_GetArenaData( int iSrvID, int iArenaType, int iTeamGUID )
{
	// 向 Database query data
	// 在使用 SendArenaData

	//SL_UpdateArenaData					( int iSrvID, int iArenaType, int iTeamGUID, StructArenaInfo ArenaInfo );
	

}
//-------------------------------------------------------------------
/*
void CNetDC_BattleGround_Child::OnRZ_JoinBattleGround( int iClientID, int iObjID, int iBattleGroundID, int iPartyID )
{

}
*/
//-------------------------------------------------------------------
