#pragma once

#include "../../mainproc/Global.h"
#include "PG/PG_BattleGround.h"



class CNetSrv_BattleGround : public Global
{
protected:

	static void _PG_BattleGround_CtoL_GetBattleGround						( int iCliID , int iSize , PVOID pData );    
	static void _PG_BattleGround_CtoL_JoinBattleGround						( int iCliID , int iSize , PVOID pData );
	static void _PG_BattleGround_LtoL_GetBattleGround						( int iCliID , int iSize , PVOID pData );
	static void _PG_BattleGround_LtoL_JoinBattleGround						( int iCliID , int iSize , PVOID pData );
	static void _PG_BattleGround_CtoL_EnterBattleGround						( int iCliID , int iSize , PVOID pData );
	static void _PG_BattleGround_LtoL_EnterBattleGround						( int iCliID , int iSize , PVOID pData );
	static void _PG_BattleGround_LtoL_ChangeZoneToBattleGround				( int iCliID , int iSize , PVOID pData );
	static void _PG_BattleGround_CtoL_LeaveBattleGround						( int iCliID , int iSize , PVOID pData );
	static void _PG_BattleGround_CtoL_LeaveBattleGroundRemote				( int iCliID , int iSize , PVOID pData );
	static void _PG_BattleGround_CtoL_GetBattleGroundScoreInfo				( int iCliID , int iSize , PVOID pData );
	static void _PG_BattleGround_CtoL_LeaveBattleGroundWaitQueue			( int iCliID , int iSize , PVOID pData );
	static void _PG_BattleGround_LtoL_LeaveBattleGroundWaitQueue			( int iCliID , int iSize , PVOID pData );

	static void _PG_BattleGround_DtoL_UpdateArenaData						( int iCliID , int iSize , PVOID pData );
	static void _PG_BattleGround_CtoL_BattleGroundWaitQueueStatusRespond	( int iCliID , int iSize , PVOID pData );
	static void _PG_BattleGround_LtoL_BattleGroundWaitQueueStatusRespond	( int iCliID , int iSize , PVOID pData );

	static void _PG_BattleGround_CtoL_GetBattleGroundList					( int iCliID , int iSize , PVOID pData );
	static void _PG_BattleGround_LtoL_GetBattleGroundList					( int iCliID , int iSize , PVOID pData );
	static void _PG_BattleGround_LtoL_LeaveBattleGroundRemote				( int iCliID , int iSize , PVOID pData );

	static void _PG_BattleGround_WLtoWL_EnterBattleGround					( int iWorldID, int iSrvID , int iSize , PVOID pData );
	static void _PG_BattleGround_WLtoWL_ChangeZoneToBattleGround			( int iWorldID, int iSrvID , int iSize , PVOID pData );
	static void _PG_BattleGround_WLtoWL_JoinBattleGround					( int iWorldID, int iSrvID , int iSize , PVOID pData );
	static void _PG_BattleGround_WLtoWL_GetBattleGroundList					( int iWorldID, int iSrvID , int iSize , PVOID pData );
	static void _PG_BattleGround_WLtoWL_LeaveBattleGroundWaitQueue			( int iWorldID, int iSrvID , int iSize , PVOID pData );
	
	static void _PG_BattleGround_WLtoWL_LeaveBattleGround					( int iWorldID, int iSrvID , int iSize , PVOID pData );

	static void _PG_BattleGround_WDtoWL_GetRankPersonalInfoResult			( int iWorldID, int iSrvID , int iSize , PVOID pData );
	static void _PG_BattleGround_WDtoWL_AddRankPointResult					( int iWorldID, int iSrvID , int iSize , PVOID pData );

	static void _PG_BattleGround_CtoL_GetRankPersonalInfo					( int iCliID , int iSize , PVOID pData );

public:

    CNetSrv_BattleGround(){};
    ~CNetSrv_BattleGround(){};

    static bool			_Init();
    static bool			_Release(){ return true;};

	//----------------------------------------------------------------

	virtual void		OnRC_GetBattleGround					( int iClientID, int iObjID ) = 0;
	
	virtual void		OnRC_EnterBattleGround					( BaseItemObject* pObj, int iType, int iRoomID, int iTeamID ) = 0;
	virtual void		OnRC_LeaveBattleGround					( BaseItemObject* pObj ) = 0;

	virtual	void		OnRC_LeaveBattleGroundWaitQueue			( int iClientDBID, int iBattleGroundType ) = 0;
	virtual void		OnRZ_LeaveBattleGroundWaitQueue			( int iWorldID, int iWorldSrvID, int iClientDBID, int iBattleGroundType ) = 0;

	virtual	void		OnRC_LeaveBattleGroundRemote			( int iClientDBID, int iBattleGroundType ) = 0;
	virtual void		OnRZ_LeaveBattleGroundRemote			( int iClientDBID, int iBattleGroundType ) = 0;

	virtual void		OnRZ_GetBattleGround					( int iWorldID, int iWorldSrvID, int iClientDBID )	= 0;

	virtual void		OnRC_JoinBattleGround					( BaseItemObject* pObj, int iObjID, int iBattleGroundID, int isParty, int iSrvID, int* pTeamDBID ) = 0;
	//virtual void		OnRZ_JoinBattleGround					( int iWorldID, int iWorldSrvID, int iClientDBID, int iBattleGroundID, int iPartyID, int iZoneID, int iRoomID, float fX, float fY, float fZ, float fDir ) = 0;
	virtual void		OnRZ_JoinBattleGround					( int iWorldID, int iWorldSrvID, int iClientLV, int iClientDBID, int iBattleGroundID, int isParty, int iZoneID, int iGroupID, int* pTeamDBID, int* pTeamLV  ) = 0;

	virtual void		OnRZ_EnterBattleGround					( int iWorldID, int iSrvID, int iClientDBID, int iType, int iRoomID, int iTeamID, int iRoleZoneID, int iRoleRoomID, float fRoleX, float fRoleY, float fRoleZ, float fRoleDir, float fRoleHP, float fRoleMP ) = 0;
	virtual void		OnRZ_ChangeZoneToBattleGround			( int iWorldID, int iSrvID, int iClientDBID, int iZoneID, int iRoomID, float X, float Y, float Z, float Dir ) = 0;

	virtual	void		OnRC_GetBattleGroundScoreInfo			( int iClientDBID ) = 0;
	
	virtual void		OnRD_UpdateArenaData					( int iArenaType, int iTeamGUID, StructArenaInfo* pArenaInfo ) = 0;
	virtual void		OnRC_BattleGroundWaitQueueStatusRespond	( int iClientDBID ) = 0;
	
	virtual void		OnRC_GetBattleGroundList				( int iClientDBID )	= 0;
	virtual	void		OnRZ_GetBattleGroundList				( int iWorldID, int iWorldSrvID, int iClientDBID ) = 0;

	virtual	void		OnRD_GetRankPersonalInfoResult			( int iWorldID, int iWorldSrvID, PVOID pData ) = 0;
	virtual	void		OnRD_AddRankPointResult					( int iWorldID, int iWorldSrvID, PVOID pData ) = 0;

	virtual	void		OnRC_GetRankPersonalInfo				( int iCliID , PVOID pData ) = 0;

	//----------------------------------------------------------------
	static	void		SL_GetBGInfo							( int iZoneID, int iClientDBID );

	static	void		SC_JoinQueueResult						( int iWorldID, int iWorldZoneID, int iClientID, int iBGType, int iRoomID, int iTeamID, const char* szBGName, int iWaitTime, int iResult );

	static	void		SC_EnterBattleGroundRight				( int iWorldID, int iWorldZoneID, int iClientID, int iBGType, int iRoomID, int iTeamID, const char* szBGName, int iWaitTime );
	static	void		SC_NotifyEnterBattleGroundRight			( int iWorldID, int iWorldZoneID, int iClientID, int iBGType, int iRoomID, const char* szBGName, int iWaitTime );
	static	void		SC_LostEnterBattleGroundRight			( int iWorldID, int iWorldZoneID, int iClientID, int iBGType, int iRoomID, const char* szBGName );

	static	void		SC_EmptyBattleGround					( int iWorldID, int iWorldZoneID, int iClientID, int iBGType, int iRoomID );
	static	void		SZ_ChangeZoneToBattleGround				( int iWorldID, int iWorldZoneID, int iZoneID, int iClientDBID, int iRoomID, float X, float Y, float Z, float Dir );

	static	void		SC_BattleGroundClose					( int iWorldID, int iWorldZoneID, int iClientDBID, int iRoomID, int iWinTeamID );
	static	void		SC_BattleGroundStatus					( int iWorldID, int iWorldZoneID, int iClientDBID, StructBattleGroundValue* pData );
	static	void		SC_LandMarkInfo							( int iWorldID, int iWorldZoneID, int iClientDBID, int iID, StructLandMark* pLandMark );

	static	void		SC_AllLandMarkInfo						( int iWorldID, int iWorldZoneID, int iDBID, StructRoomLandMark* pInfo );
	virtual void		SC_AllPlayerPos							() = 0;

	static	void		SC_SetBattleGroundData					( int iClientID, int iForceFlagID, int iCampID, int iRoomID );
	virtual	void		SC_BattleGroundScore					( int iWorldID, int iWorldZoneID, int iClientDBID, int iRoomID ) = 0;

	static	void		SC_BattleGroundWaitQueueStatus			( int iWorldID, int iWorldZoneID, int iClientDBID, int iBattleGroundType, int iStatus, int iWaitID, int iSrvCheckTime );

	static	void		SZ_GetBattleGroundList					( int iClientDBID );

	// RANK SYSTEM
	//----------------------------------------------------------------
	static	void		SD_AddRankPoint							( int iClientDBID, int iRankPoint );
	static	void		SD_GetRankPersonalInfo					( int iClientDBID );
	static	void		SC_GetRankPersonalInfoResult			( int iClientDBID, int iRankPoint, int iLastRoundRank, int iRank, const char* pszNote );

	// Arena
	//----------------------------------------------------------------
	static	void		SD_GetAreanData							( int iArenaType, int iTeamGUID );
	static	void		SC_UpdateArenaData						( int iClientDBID, int iArenaType, int iTeamGUID, StructArenaInfo* pArenaInfo );
	//----------------------------------------------------------------

	static CNetSrv_BattleGround*		m_pThis;
};
