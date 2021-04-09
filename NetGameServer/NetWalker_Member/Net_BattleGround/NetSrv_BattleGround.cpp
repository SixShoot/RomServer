#include "./NetSrv_BattleGround.h"
#include "../Net_ServerList/Net_ServerList_Child.h"


//#define Register_LuaFunc_NetSrv_Script
//	LuaRegisterFunc( "Lua_Quest_CheckRoleItem"		, bool( int, int, int )		, CNetSrv_Script::Lua_Quest_CheckRoleItem	);\
//	LuaRegisterFunc( "Lua_Quest_DeleteItem"			, bool( int, int, int )     , CNetSrv_Script::Lua_Quest_DeleteItem		);
	// LuaRegisterFunc( "DelFromPartition"	, bool( int )                           , DelFromPartition		);

#define DF_WORLDBGID	30

// 靜態成員宣告
//-------------------------------------------------------------------
CNetSrv_BattleGround*		CNetSrv_BattleGround::m_pThis = NULL;

//-------------------------------------------------------------------
bool CNetSrv_BattleGround::_Init()
{
	Server_Reg_Client_Packet( PG_BattleGround_CtoL_GetBattleGround						);
	Server_Reg_Client_Packet( PG_BattleGround_CtoL_JoinBattleGround						);
	Server_Reg_Client_Packet( PG_BattleGround_CtoL_EnterBattleGround					);
	Server_Reg_Client_Packet( PG_BattleGround_CtoL_LeaveBattleGround					);
	Server_Reg_Client_Packet( PG_BattleGround_CtoL_GetBattleGroundScoreInfo				);
	Server_Reg_Client_Packet( PG_BattleGround_CtoL_LeaveBattleGroundWaitQueue			);
	Server_Reg_Client_Packet( PG_BattleGround_CtoL_GetBattleGroundList					);
	Server_Reg_Client_Packet( PG_BattleGround_CtoL_GetRankPersonalInfo					);

	Server_Reg_Client_Packet( PG_BattleGround_CtoL_LeaveBattleGroundRemote				);
	Server_Reg_Client_Packet( PG_BattleGround_CtoL_BattleGroundWaitQueueStatusRespond	);

	Server_Reg_Server_Packet( PG_BattleGround_LtoL_GetBattleGround						);
	Server_Reg_Server_Packet( PG_BattleGround_LtoL_JoinBattleGround						);
	Server_Reg_Server_Packet( PG_BattleGround_LtoL_EnterBattleGround					);
	Server_Reg_Server_Packet( PG_BattleGround_LtoL_ChangeZoneToBattleGround				);
	Server_Reg_Server_Packet( PG_BattleGround_LtoL_LeaveBattleGroundWaitQueue			);
	Server_Reg_Server_Packet( PG_BattleGround_LtoL_GetBattleGroundList					);

	Server_Reg_Server_Packet( PG_BattleGround_DtoL_UpdateArenaData						);

	
	Server_Reg_Server_Packet( PG_BattleGround_LtoL_BattleGroundWaitQueueStatusRespond	);
	Server_Reg_Server_Packet( PG_BattleGround_LtoL_LeaveBattleGroundRemote				);

	Server_Reg_World_Packet	( PG_BattleGround_WLtoWL_EnterBattleGround					);
	Server_Reg_World_Packet	( PG_BattleGround_WLtoWL_ChangeZoneToBattleGround			);
	Server_Reg_World_Packet	( PG_BattleGround_WLtoWL_JoinBattleGround					);
	Server_Reg_World_Packet	( PG_BattleGround_WLtoWL_GetBattleGroundList				);
	Server_Reg_World_Packet	( PG_BattleGround_WLtoWL_LeaveBattleGroundWaitQueue			);

	Server_Reg_World_Packet	( PG_BattleGround_WDtoWL_GetRankPersonalInfoResult			);
	Server_Reg_World_Packet	( PG_BattleGround_WDtoWL_AddRankPointResult					);

	

	return true;
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_LtoL_LeaveBattleGroundRemote( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_LtoL_LeaveBattleGroundRemote );
	m_pThis->OnRZ_LeaveBattleGroundRemote( pPacket->iDBID, pPacket->iBattleGroundType );

}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_CtoL_LeaveBattleGroundRemote( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_CtoL_LeaveBattleGroundRemote );
	M_CLIENT_OBJ( iCliID );

	if( pObj )
	{	
		if( pObj->Role()->DBID() == pPacket->iDBID )
		{
			m_pThis->OnRC_LeaveBattleGroundRemote( pPacket->iDBID, pPacket->iBattleGroundType );
		}
	}
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_CtoL_GetRankPersonalInfo( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_CtoL_GetRankPersonalInfo );
	m_pThis->OnRC_GetRankPersonalInfo( iCliID, pData );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_WDtoWL_AddRankPointResult( int iWorldID, int iWorldSrvID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_WDtoWL_AddRankPointResult );
	m_pThis->OnRD_AddRankPointResult( iWorldID, iWorldSrvID, pData );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_WDtoWL_GetRankPersonalInfoResult( int iWorldID, int iWorldSrvID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_WDtoWL_GetRankPersonalInfoResult );
	m_pThis->OnRD_GetRankPersonalInfoResult( iWorldID, iWorldSrvID, pData );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_WLtoWL_LeaveBattleGroundWaitQueue( int iWorldID, int iWorldSrvID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_WLtoWL_LeaveBattleGroundWaitQueue );
	m_pThis->OnRZ_LeaveBattleGroundWaitQueue( iWorldID, iWorldSrvID, pPacket->iDBID, pPacket->iBattleGroundType );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_WLtoWL_GetBattleGroundList( int iWorldID, int iWorldSrvID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_WLtoWL_GetBattleGroundList );
	m_pThis->OnRZ_GetBattleGroundList( iWorldID, iWorldSrvID, pPacket->iDBID );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_WLtoWL_JoinBattleGround	( int iWorldID, int iWorldSrvID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_WLtoWL_JoinBattleGround );
	//m_pThis->OnRZ_JoinBattleGround( iWorldID, iWorldSrvID, pPacket->iDBID, pPacket->iBattleGroundID, pPacket->iParty, p->iZoneID, p->iRoomID, p->fX, p->fY, p->fZ, p->fDir );
	m_pThis->OnRZ_JoinBattleGround( iWorldID, iWorldSrvID, pPacket->iLV, pPacket->iDBID, pPacket->iBattleGroundID, pPacket->iParty, p->iZoneID, p->iGroupID, p->iTeamDBID, p->iTeamMemberLV );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_WLtoWL_ChangeZoneToBattleGround ( int iWorldID, int iSrvID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_WLtoWL_ChangeZoneToBattleGround );
	m_pThis->OnRZ_ChangeZoneToBattleGround( pPacket->iWorldID, iSrvID, pPacket->iClientDBID, pPacket->iZoneID, pPacket->iRoomID, pPacket->X, pPacket->Y, pPacket->Z, pPacket->Dir );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_WLtoWL_EnterBattleGround ( int iWorldID, int iSrvID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_WLtoWL_EnterBattleGround );
	m_pThis->OnRZ_EnterBattleGround( 
		iWorldID, 		
		iSrvID, 
		pPacket->iClientDBID,
		pPacket->iType,
		pPacket->iRoomID,
		pPacket->iTeamID,
		pPacket->iRoleSourceZoneID,
		pPacket->iRoleSourceRoomID,
		pPacket->fRoleSourceX,
		pPacket->fRoleSourceY,
		pPacket->fRoleSourceZ,
		pPacket->fRoleSourceDir,
		pPacket->fRoleSourceHP,
		pPacket->fRoleSourceMP
		);
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_CtoL_GetBattleGroundList ( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_CtoL_GetBattleGroundList );
	m_pThis->OnRC_GetBattleGroundList( pPacket->iDBID );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_LtoL_GetBattleGroundList ( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_LtoL_GetBattleGroundList );
	m_pThis->OnRZ_GetBattleGroundList( 0, 0, pPacket->iDBID );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_LtoL_BattleGroundWaitQueueStatusRespond	( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_LtoL_BattleGroundWaitQueueStatusRespond );
	m_pThis->OnRC_BattleGroundWaitQueueStatusRespond( pPacket->iDBID );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_CtoL_BattleGroundWaitQueueStatusRespond( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_CtoL_BattleGroundWaitQueueStatusRespond );

	PG_BattleGround_LtoL_BattleGroundWaitQueueStatusRespond	PacketToBattleGround;

	PacketToBattleGround.iDBID				= pPacket->iDBID;

	SendToLocal( pPacket->iSrvID, sizeof( PG_BattleGround_LtoL_BattleGroundWaitQueueStatusRespond), &PacketToBattleGround );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_DtoL_UpdateArenaData( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_DtoL_UpdateArenaData );
	m_pThis->OnRD_UpdateArenaData( pPacket->iArenaType, pPacket->iTeamGUID, &( pPacket->ArenaInfo ) ); 
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_LtoL_LeaveBattleGroundWaitQueue( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_LtoL_LeaveBattleGroundWaitQueue );
	m_pThis->OnRZ_LeaveBattleGroundWaitQueue( 0, 0, pPacket->iDBID, pPacket->iBattleGroundType );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_CtoL_LeaveBattleGroundWaitQueue( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_CtoL_LeaveBattleGroundWaitQueue );
	M_CLIENT_OBJ( iCliID );

	if( pObj )
	{	
		if( pObj->Role()->DBID() == pPacket->iDBID )
		{

			m_pThis->OnRC_LeaveBattleGroundWaitQueue( pPacket->iDBID, pPacket->iBattleGroundType );
		}
	}
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_CtoL_GetBattleGroundScoreInfo ( int iCliID, int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_CtoL_GetBattleGroundScoreInfo );
	m_pThis->OnRC_GetBattleGroundScoreInfo( pPacket->iClientDBID );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_CtoL_LeaveBattleGround ( int iCliID, int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_CtoL_LeaveBattleGround );
	M_CLIENT_OBJ( iCliID );
	m_pThis->OnRC_LeaveBattleGround( pObj );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_LtoL_ChangeZoneToBattleGround ( int iSrvID, int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_LtoL_ChangeZoneToBattleGround );
	m_pThis->OnRZ_ChangeZoneToBattleGround( 0, iSrvID, pPacket->iClientDBID, pPacket->iZoneID, pPacket->iRoomID, pPacket->X, pPacket->Y, pPacket->Z, pPacket->Dir );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_LtoL_EnterBattleGround ( int iSrvID, int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_LtoL_EnterBattleGround );
	m_pThis->OnRZ_EnterBattleGround( 
		0,
		iSrvID, 
		pPacket->iClientDBID,
		pPacket->iType,
		pPacket->iRoomID,
		pPacket->iTeamID,
		pPacket->iRoleSourceZoneID,
		pPacket->iRoleSourceRoomID,
		pPacket->fRoleSourceX,
		pPacket->fRoleSourceY,
		pPacket->fRoleSourceZ,
		pPacket->fRoleSourceDir,
		pPacket->fRoleSourceHP,
		pPacket->fRoleSourceMP
		);
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_CtoL_EnterBattleGround ( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_CtoL_EnterBattleGround );
	M_CLIENT_OBJ( iCliID );
	m_pThis->OnRC_EnterBattleGround( pObj, pPacket->iBattleGroundType, pPacket->iRoomID, pPacket->iTeamID );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_CtoL_GetBattleGround ( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_CtoL_GetBattleGround );
	m_pThis->OnRC_GetBattleGround( iCliID, pPacket->iObjID );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_CtoL_JoinBattleGround ( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_CtoL_JoinBattleGround );
	M_CLIENT_OBJ( iCliID );
	m_pThis->OnRC_JoinBattleGround( pObj, pObj->GUID(), pPacket->iBattleGroundID, pPacket->iParty, pPacket->iSrvID, pPacket->iTeamDBID );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_LtoL_GetBattleGround			( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_LtoL_GetBattleGround );
	m_pThis->OnRZ_GetBattleGround( 0, 0, pPacket->iDBID );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::_PG_BattleGround_LtoL_JoinBattleGround			( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_BattleGround_LtoL_JoinBattleGround );
	//m_pThis->OnRZ_JoinBattleGround( 0, 0, pPacket->iDBID, pPacket->iBattleGroundID, pPacket->iParty, p->iZoneID, p->iRoomID, p->fX, p->fY, p->fZ, p->fDir );
	m_pThis->OnRZ_JoinBattleGround( 0, 0, pPacket->iLV, pPacket->iDBID, pPacket->iBattleGroundID, pPacket->iParty, p->iZoneID, pPacket->iWorldGroupID, p->iTeamDBID, p->iTeamMemberLV );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::SC_EnterBattleGroundRight ( int iWorldID, int iWorldZoneID, int iClientDBID, int iType, int iRoomID, int iTeamID, const char* szName, int iWaitTime )
{
	if( iWorldID == 0 )
	{
		PG_BattleGround_LtoC_EnterBattleGroundRight	Packet;

		Packet.iType		= iType;
		Packet.iRoomID		= iRoomID;
		Packet.iTeamID		= iTeamID;
		Packet.iWaitTime	= iWaitTime;
		Packet.sName		= szName;

		//Global::SendToCli_ByGUID( iClientID, sizeof(Packet), &Packet );	
		Global::SendToCli_ByDBID( iClientDBID, sizeof(Packet), &Packet );
	}
	else
	{
		PG_BattleGround_LtoC_EnterBattleGroundRight	Packet;

		Packet.iType		= iType;
		Packet.iRoomID		= iRoomID;
		Packet.iTeamID		= iTeamID;
		Packet.iWaitTime	= iWaitTime;
		Packet.sName		= szName;

		//Global::SendToCli_ByGUID( iClientID, sizeof(Packet), &Packet );	
		//Global::SendToCli_ByDBID( iClientDBID, sizeof(Packet), &Packet );
		Global::SendToOtherWorldZoneClient_ByDBID( iWorldID, iWorldZoneID, iClientDBID, sizeof( Packet ), &Packet );
	}
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::SC_NotifyEnterBattleGroundRight ( int iWorldID, int iWorldZoneID, int iClientID, int iType, int iRoomID, const char* szName, int iWaitTime )
{
	if( iWorldID == 0 )
	{
		PG_BattleGround_LtoC_NotifyEnterBattleGroundRight Packet;

		Packet.iType		= iType;
		Packet.iRoomID		= iRoomID;
		Packet.iWaitTime	= iWaitTime;
		Packet.sName		= szName;
		
		Global::SendToCli_ByDBID( iClientID, sizeof(Packet), &Packet );	
	}
	else
	{
		PG_BattleGround_LtoC_NotifyEnterBattleGroundRight Packet;

		Packet.iType		= iType;
		Packet.iRoomID		= iRoomID;
		Packet.iWaitTime	= iWaitTime;
		Packet.sName		= szName;

		Global::SendToOtherWorldZoneClient_ByDBID( iWorldID, iWorldZoneID, iClientID, sizeof( Packet ), &Packet );
	}
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::SC_LostEnterBattleGroundRight ( int iWorldID, int iWorldZoneID, int iClientID, int iType, int iRoomID, const char* szName )
{
	if( iWorldID == 0 )
	{

		PG_BattleGround_LtoC_LostEnterBattleGroundRight	Packet;

		Packet.iType		= iType;
		Packet.iRoomID		= iRoomID;
		Packet.sName		= szName;

		Global::SendToCli_ByDBID( iClientID, sizeof(Packet), &Packet );	
	}
	else
	{
		PG_BattleGround_LtoC_LostEnterBattleGroundRight	Packet;

		Packet.iType		= iType;
		Packet.iRoomID		= iRoomID;
		Packet.sName		= szName;

		Global::SendToOtherWorldZoneClient_ByDBID( iWorldID, iWorldZoneID, iClientID, sizeof( Packet ), &Packet );
	}
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::SC_EmptyBattleGround( int iWorldID, int iWorldZoneID, int iClientID, int iBGType, int iRoomID )
{
	if( iWorldID == 0 )
	{
		PG_BattleGround_LtoC_EmptyBattleGround Packet;

		Packet.iType		= iBGType;
		Packet.iRoomID		= iRoomID;

		Global::SendToCli_ByGUID( iClientID, sizeof(Packet), &Packet );
	}
	else
	{
		PG_BattleGround_LtoC_EmptyBattleGround Packet;

		Packet.iType		= iBGType;
		Packet.iRoomID		= iRoomID;

		Global::SendToOtherWorldZoneClient_ByDBID( iWorldID, iWorldZoneID, iClientID, sizeof( Packet ), &Packet );
	}
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::SL_GetBGInfo( int iZoneID, int iClientDBID )
{
	PG_BattleGround_LtoL_GetBattleGround	Packet;

	Packet.iDBID		= iClientDBID;

	SendToLocal( iZoneID, sizeof(Packet), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::SZ_ChangeZoneToBattleGround( int iWorldID, int iWorldZoneID, int iZoneID, int iClientDBID, int iRoomID, float X, float Y, float Z, float Dir )
{
	if( iWorldID == 0 )
	{
		PG_BattleGround_LtoL_ChangeZoneToBattleGround Packet;

		Packet.iClientDBID	= iClientDBID;
		Packet.iZoneID		= iZoneID;
		Packet.iRoomID		= iRoomID;
		Packet.X			= X;
		Packet.Y			= Y;
		Packet.Z			= Z;
		Packet.Dir			= Dir;

		//SendToSrvBySockID( iSrvID, sizeof( Packet ), &Packet );

		SendToLocal( iWorldZoneID, sizeof( Packet ), &Packet );
	}
	else
	{
		PG_BattleGround_WLtoWL_ChangeZoneToBattleGround Packet;

		Packet.iClientDBID	= iClientDBID;
		Packet.iWorldID		= Global::Net()->GetWorldID();
		Packet.iZoneID		= iZoneID;
		Packet.iRoomID		= iRoomID;
		Packet.X			= X;
		Packet.Y			= Y;
		Packet.Z			= Z;
		Packet.Dir			= Dir;

		//Global::SendToOtherWorldZoneClient_ByDBID( iWorldID, iWorldZoneID, iClientDBID, sizeof( Packet ), &Packet );
		Global::SendToOtherWorld_GSrvID( iWorldID, iWorldZoneID, sizeof( Packet ), &Packet );
	}
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::SC_JoinQueueResult ( int iWorldID, int iWorldZoneID, int iClientDBID, int iBGType, int iRoomID, int iTeamID, const char* szBGName, int iWaitTime, int iResult )
{
	if( iWorldID == 0 )
	{
		PG_BattleGround_LtoC_JoinQueueResult Packet;

		Packet.iType		= iBGType;
		Packet.iRoomID		= iRoomID;
		Packet.iTeamID		= iTeamID;
		Packet.sName		= szBGName;
		Packet.iWaitTime	= iWaitTime;
		Packet.iResult		= iResult;
			
		Global::SendToCli_ByDBID( iClientDBID, sizeof(Packet), &Packet );
	}
	else
	{
		PG_BattleGround_LtoC_JoinQueueResult Packet;

		Packet.iType		= iBGType;
		Packet.iRoomID		= iRoomID;
		Packet.iTeamID		= iTeamID;
		Packet.sName		= szBGName;
		Packet.iWaitTime	= iWaitTime;
		Packet.iResult		= iResult;

		Global::SendToOtherWorldZoneClient_ByDBID( iWorldID, iWorldZoneID, iClientDBID, sizeof(Packet), &Packet );

	}
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::SC_BattleGroundClose	( int iWorldID, int iWorldZoneID, int iClientDBID, int iRoomID, int iWinTeamID )
{
	if( iWorldID == 0 )
	{
		PG_BattleGround_LtoC_BattleGroundClose	Packet;

		Packet.iWinTeamID	= iWinTeamID;

		Global::SendToCli_ByDBID( iClientDBID, sizeof(Packet), &Packet );
	}
	else
	{
		PG_BattleGround_LtoC_BattleGroundClose	Packet;

		Packet.iWinTeamID	= iWinTeamID;

		Global::SendToOtherWorldZoneClient_ByDBID( iWorldID, iWorldZoneID, iClientDBID, sizeof(Packet), &Packet );
	}
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::SC_BattleGroundStatus ( int iWorldID, int iWorldZoneID, int iClientDBID, StructBattleGroundValue* pData )
{
	if( iWorldID == 0 )
	{
		PG_BattleGround_LtoC_BattleGroundStatus	Packet;

		Packet.Data	= *pData;

		Global::SendToCli_ByDBID( iClientDBID, sizeof(Packet), &Packet );
	}
	else
	{
		PG_BattleGround_LtoC_BattleGroundStatus	Packet;

		Packet.Data	= *pData;

		Global::SendToOtherWorldZoneClient_ByDBID( iWorldID, iWorldZoneID, iClientDBID, sizeof(Packet), &Packet );
	}
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::SC_LandMarkInfo( int iWorldID, int iWorldZoneID, int iClientDBID, int iID, StructLandMark* pLandMark )
{
	if( iWorldID == 0 )
	{
		PG_BattleGround_LtoC_BattleGroundLandMark Packet;
		
		Packet.iID		= iID;
		Packet.LandMark	= *pLandMark;

		Global::SendToCli_ByDBID( iClientDBID, sizeof(Packet), &Packet );
	}
	else
	{
		PG_BattleGround_LtoC_BattleGroundLandMark Packet;

		Packet.iID		= iID;
		Packet.LandMark	= *pLandMark;

		Global::SendToOtherWorldZoneClient_ByDBID( iWorldID, iWorldZoneID, iClientDBID, sizeof(Packet), &Packet );
	}
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::SC_SetBattleGroundData( int iClientID, int iForceFlagID, int iCampID, int iRoomID )
{
	PG_BattleGround_LtoC_SetBattleGroundData	Packet;

	Packet.iForceFlagID		= iForceFlagID;
	Packet.iCampID			= iCampID;
	Packet.iRoomID			= iRoomID;

	Global::SendToCli_ByGUID( iClientID, sizeof(Packet), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround::SC_BattleGroundWaitQueueStatus		( int iWorldID, int iWorldZoneID, int iClientDBID, int iBattleGroundType, int iStatus, int iWaitID, int iSrvCheckTime )
{
	if( iWorldID == 0 )
	{
		PG_BattleGround_LtoC_BattleGroundWaitQueueStatus	Packet;

		Packet.iBattleGroundType	= iBattleGroundType;
		Packet.iStatus				= iStatus;
		Packet.iWaitID				= iWaitID;
		Packet.iSrvCheckTime		= iSrvCheckTime;

		Global::SendToCli_ByDBID( iClientDBID, sizeof(Packet), &Packet );
	}
	else
	{
		PG_BattleGround_LtoC_BattleGroundWaitQueueStatus	Packet;

		Packet.iBattleGroundType	= iBattleGroundType;
		Packet.iStatus				= iStatus;
		Packet.iWaitID				= iWaitID;
		Packet.iSrvCheckTime		= iSrvCheckTime;

		Global::SendToOtherWorldZoneClient_ByDBID( iWorldID, iWorldZoneID, iClientDBID, sizeof(Packet), &Packet );
	}
}

//----------------------------------------------------------------
void CNetSrv_BattleGround::SC_AllLandMarkInfo( int iWorldID, int iWorldZoneID, int iClientDBID, StructRoomLandMark* pInfo )
{
	if( iWorldID == 0 )
	{
		PG_BattleGround_LtoC_AllLandMarkInfo	Packet;

		Packet.Info					= *pInfo;

		Global::SendToCli_ByDBID( iClientDBID, sizeof(Packet), &Packet );
	}
	else
	{
		PG_BattleGround_LtoC_AllLandMarkInfo	Packet;

		Packet.Info					= *pInfo;

		Global::SendToOtherWorldZoneClient_ByDBID( iWorldID, iWorldZoneID, iClientDBID, sizeof(Packet), &Packet );
	}
}
//----------------------------------------------------------------
void CNetSrv_BattleGround::SD_GetAreanData( int iArenaType, int iTeamGUID )
{
	PG_BattleGround_LtoD_GetArenaData	Packet;

	Packet.iArenaType		= iArenaType;
	Packet.iTeamGUID		= iTeamGUID;

	Global::SendToDBCenter( sizeof(Packet), &Packet );
}
//----------------------------------------------------------------
void CNetSrv_BattleGround::SC_UpdateArenaData( int iClientDBID, int iArenaType, int iTeamGUID, StructArenaInfo* pArenaInfo )
{
	PG_BattleGround_LtoC_UpdateArenaData	Packet;

	Packet.iArenaType		= iArenaType;
	Packet.iTeamGUID		= iTeamGUID;
	Packet.ArenaInfo		= *pArenaInfo;

	Global::SendToCli_ByDBID( iClientDBID, sizeof(Packet), &Packet );
}
//----------------------------------------------------------------
void CNetSrv_BattleGround::SZ_GetBattleGroundList( int iClientDBID )
{
	PG_BattleGround_LtoL_GetBattleGroundList	Packet;

	Packet.iDBID			= iClientDBID;

	Global::SendToAllLocal( sizeof(Packet), &Packet );
}
//----------------------------------------------------------------
void CNetSrv_BattleGround::SD_GetRankPersonalInfo( int iClientDBID )
{
	PG_BattleGround_WLtoWD_GetRankPersonalInfo	Packet;

	Packet.iWorldDBID		= iClientDBID;
	Packet.iWorldID			= Global::Net()->GetWorldID();

	//Global::SendToOtherWorld_GSrvID(  )
	// 送到跨 Server 的 DataCenter
	Global::SendToOtherWorld_ServerType( DF_WORLDBGID, EM_SERVER_TYPE_DATACENTER, sizeof( Packet ), &Packet );
}
//----------------------------------------------------------------
void CNetSrv_BattleGround::SC_GetRankPersonalInfoResult( int iClientDBID, int iRankPoint, int iLastRoundRank, int iRank, const char* pszNote )
{
	PG_BattleGround_LtoC_GetRankPersonalInfoResult	Packet;

	Packet.iDBID			= iClientDBID;	
	Packet.iRankPoint		= iRankPoint;
	Packet.iLastRoundRank	= iLastRoundRank;
	Packet.iRank			= iRank;
	Packet.sNote			= pszNote;

	Global::SendToCli_ByDBID( iClientDBID, sizeof( Packet ), &Packet );
}