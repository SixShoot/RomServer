#include "./NetSrv_BattleGround.h"
#include "../Net_ServerList/Net_ServerList_Child.h"


//#define Register_LuaFunc_NetSrv_Script
//	LuaRegisterFunc( "Lua_Quest_CheckRoleItem"		, bool( int, int, int )		, CNetSrv_Script::Lua_Quest_CheckRoleItem	);\
//	LuaRegisterFunc( "Lua_Quest_DeleteItem"			, bool( int, int, int )     , CNetSrv_Script::Lua_Quest_DeleteItem		);
	// LuaRegisterFunc( "DelFromPartition"	, bool( int )                           , DelFromPartition		);


// ÀRºA¦¨­û«Å§i
//-------------------------------------------------------------------
CNetDC_BattleGround*		CNetDC_BattleGround::m_pThis = NULL;

//-------------------------------------------------------------------
bool CNetDC_BattleGround::Init()
{
	//RegServerPacket( PG_BattleGround_LtoD_GetArenaData	);

	return true;
}
//-------------------------------------------------------------------
void CNetDC_BattleGround::_PG_BattleGround_LtoD_GetArenaData ( int iSrvID , int iSize , PVOID pData )
{
	//M_PACKET( PG_BattleGround_LtoD_GetBattleGround );
	//m_pThis->OnRZ_GetBattleGround( pPacket->iClientID, pPacket->iObjID, pPacket->iBattleGroundID );
}
//-------------------------------------------------------------------
/*
void CNetDC_BattleGround::_PG_BattleGround_LtoD_JoinBattleGround ( int iSrvID, int iSize , PVOID pData )
{
	//M_PACKET( PG_BattleGround_LtoD_GetBattleGround );
	//m_pThis->OnRZ_JoinBattleGround( pPacket->iClientID, pPacket->iObjID, pPacket->iBattleGroundID, pPacket->iBattleGroundID );
}
*/
//-------------------------------------------------------------------
void CNetDC_BattleGround::SL_UpdateArenaData( int iSrvID, int iArenaType, int iTeamGUID, StructArenaInfo ArenaInfo )
{
	PG_BattleGround_DtoL_UpdateArenaData	Packet;

	Packet.iArenaType	= iArenaType;
	Packet.iTeamGUID	= iTeamGUID;
	Packet.ArenaInfo	= ArenaInfo;

	Global::SendToSrvBySockID( iSrvID, sizeof( Packet ), &Packet );
}