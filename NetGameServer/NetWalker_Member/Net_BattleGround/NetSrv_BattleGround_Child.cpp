//#include "../Net_Talk/NetSrv_Talk.h"

#include "NetSrv_BattleGround_Child.h"
#include "../../MainProc/LuaPlot/LuaPlot.h"
#include "../../MainProc/partyinfolist/PartyInfoList.h"
#include "../net_cliconnect/NetSrv_CliConnectChild.h"
#include "../net_talk/NetSrv_TalkChild.h"
#include "../net_party/NetSrv_PartyChild.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
/*
#include "../Net_ServerList/Net_ServerList_Child.h"
#include "../../mainproc/pathmanage/NPCMoveFlagManage.h"
#include "../Net_Gather/NetSrv_Gather_Child.h"
#include "../../MainProc/partyinfolist/PartyInfoList.h"
*/


//-------------------------------------------------------------------
void    CNetSrv_BattleGround_Child::Init()
{

	// 系統初始化
	CNetSrv_BattleGround::_Init();

	if( m_pThis != NULL)
		return;

	m_pThis = NEW( CNetSrv_BattleGround_Child );

	((CNetSrv_BattleGround_Child*)m_pThis)->InitBattleGround();

	if( ((CNetSrv_BattleGround_Child*)m_pThis)->m_BG_iBattleGroundType != 0 )
	{
		Global::Schedular()->Push( OnTime_CheckBattleGroundQueue , 10000, NULL, NULL );	
		Global::Schedular()->Push( OnTime_CheckBattleGroundObjExist , DF_BG_CHECK_OBJ_EXIST_TIME, NULL, NULL );	
		Global::Schedular()->Push( OnTime_UpdateAllBattleGroundPlayerPos , DF_BG_UPDATE_PLAYER_POS_TIME, NULL, NULL );
	}



}
//-------------------------------------------------------------------
void    CNetSrv_BattleGround_Child::Release()
{	
	CNetSrv_BattleGround::_Release();

	if( m_pThis == NULL )
		return;

	delete m_pThis;
	m_pThis = NULL;


	return;
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround_Child::InitBattleGround()
{

	m_BG_iBattleGroundType					= 0;
	m_BG_iNumTeamPeople						= 0;
	m_BG_iNumTeam							= 0;
	m_BG_iFillFromQueue						= 0;
	m_BG_Lua_RoleEnterBattleGround			= "";
	m_BG_Lua_MoveRoleToBattleGround			= "";
	m_BG_Lua_RoleJoinQueue					= "";
	m_BG_Lua_ClientLoaded					= "";
	m_BG_iDisableRoleReconnect				= 0;
	m_BG_iDisableDelNoRespondQueueObj		= 0;
	m_BG_iWorldBattleGround					= 0;
	m_BG_iWorldGroupID						= 0;
	m_BG_iWorldQueueRule					= 0;
	m_BG_iMinTeamPeople						= 0;
	m_BG_iKickDisconnect					= 1;

	InitBattleGroundList();

	// 變數初始化
	if( Ini()->Int( "BattleGround" ) == 1 )
	{		
		m_BG_iKickDisconnect				= Ini()->Int( "m_BG_iKickDisconnect" );
		m_BG_iBattleGroundType				= Ini()->Int( "ZoneID" );
	
		m_BG_iNumTeamPeople					= Ini()->Int( "BG_iNumTeamPeople" );
		m_BG_iNumTeam						= Ini()->Int( "BG_iNumTeam" );						//Ini()->BattleGround_NumTeam;
		m_BG_iFillFromQueue					= Ini()->Int( "BG_iFillFromQueue" );				//Ini()->BattleGround_FillFromQueue;
		m_BG_iDisableRoleReconnect			= Ini()->Int( "BG_iDisableRoleReconnect" );			//Ini()->BattleGround_Lua_RoleJoinQueue;
		m_BG_iDisableDelNoRespondQueueObj	= Ini()->Int( "BG_iDisableDelNoRespondQueueObj" );	//Ini()->BattleGround_Lua_RoleJoinQueue;

		m_BG_Lua_AssignLevelGroup			= Ini()->Str( "BG_Lua_AssignLevelGroup" );			//Ini()->BattleGround_Lua_RoleEnterBattleGround;
		m_BG_Lua_RoleEnterBattleGround		= Ini()->Str( "BG_Lua_RoleEnterBattleGround" );		//Ini()->BattleGround_Lua_RoleEnterBattleGround;
		m_BG_Lua_MoveRoleToBattleGround		= Ini()->Str( "BG_Lua_MoveRoleToBattleGround" );	//Ini()->BattleGround_Lua_MoveRoleToBattleGround;
		m_BG_Lua_RoleJoinQueue				= Ini()->Str( "BG_Lua_RoleJoinQueue" );				//Ini()->BattleGround_Lua_RoleJoinQueue;
		m_BG_Lua_ClientLoaded				= Ini()->Str( "BG_Lua_ClientLoaded" );				//Ini()->BattleGround_Lua_RoleJoinQueue;
		m_BG_iWorldQueueRule				= Ini()->Int( "BG_Lua_iWorldQueueRule" );			//Ini()->BattleGround_Lua_RoleJoinQueue;

		m_BG_iWorldGroupID					= Ini()->Int( "BG_iWorldGroupID" );
		m_BG_iMinTeamPeople					= Ini()->Int( "BG_iMinTeamPeople" );

		GameObjDbStructEx* pZoneObj	= Global::GetObjDB( ( Def_ObjectClass_Zone + ( m_BG_iBattleGroundType % 1000 ) ) );

		if( pZoneObj != NULL )
		{
			if( pZoneObj->Zone.IsWorldBattleGround != false )
			{
				m_BG_iWorldBattleGround			= 1;
				Global::Ini()->IsBattleWorld	= true;
			}

			m_IsLimitJoinLevel					= pZoneObj->Zone.IsLimitJoinLevel;
			m_iMaxJoinLevel						= pZoneObj->Zone.iMaxJoinLevel;
			m_iMinJoinLevel						= pZoneObj->Zone.iMinJoinLevel;
			m_IsForceGroup						= pZoneObj->Zone.IsForceGroup;


			// LimitJoinLevel
			//----------------------------------------
			int iIsLimitJoinLevel = -1;	

			Ini()->Int( "BG_IsLimitJoinLevel",	iIsLimitJoinLevel );

			if( iIsLimitJoinLevel != -1 )
			{
				if( iIsLimitJoinLevel == 0 )
					m_IsLimitJoinLevel = false;
				else
					m_IsLimitJoinLevel = true;
			}
			//----------------------------------------

			// IsForceGroup
			//----------------------------------------
			int iIsForceGroup = -1;	

			Ini()->Int( "BG_IsForceGroup",	iIsForceGroup );

			if( iIsForceGroup != -1 )
			{
				if( iIsForceGroup == 0 )
					m_IsForceGroup = false;
				else
					m_IsForceGroup = true;
			}
			//----------------------------------------


			Ini()->Int( "BG_iMaxJoinLevel",		m_iMaxJoinLevel );
			Ini()->Int( "BG_iMinJoinLevel",		m_iMinJoinLevel );
		}
	}

	// Load Lua Event

	// 初始化開始房間
	int iTotalRoom = Ini()->RoomCount;

	for( int i = 1; i < iTotalRoom; i++ )
	{
		setFreeID.insert( i );

	}

	// 初始化變數
	for( int i=0; i < iTotalRoom; i++ )
	{
		StructBattleGroundValue	emptyValue;

		ZeroMemory( &emptyValue, sizeof( StructBattleGroundValue ) );

		emptyValue.iRoomID	= i;

		vecValue.push_back( emptyValue );

		StructRoomLandMark	emptyLandMark;

		ZeroMemory( &emptyLandMark, sizeof( StructRoomLandMark ) );

		vecLandMark.push_back( emptyLandMark );
	}
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRC_EnterBattleGround	( BaseItemObject* pObj, int iType, int iRoomID, int iTeamID )
{
	if( pObj == NULL )
		return;


	// 取得現在 Server 是否為獨立空間
	int iZoneID = RoleDataEx::G_ZoneID % _DEF_ZONE_BASE_COUNT_;

	if( iZoneID >= 100 )
	{
		return;
	}




	RoleDataEx*								pPlayer	= pObj->Role();



	// 修正進入點資訊
	pPlayer->SelfData.ReturnZoneID	= pPlayer->BaseData.ZoneID;
	pPlayer->SelfData.ReturnPos.X	= pPlayer->BaseData.Pos.X;
	pPlayer->SelfData.ReturnPos.Y	= pPlayer->BaseData.Pos.Y;
	pPlayer->SelfData.ReturnPos.Z	= pPlayer->BaseData.Pos.Z;
	pPlayer->SelfData.ReturnPos.Dir	= pPlayer->BaseData.Pos.Dir;

	//if( iType == 410 )

	bool	bWorldBattleGround	= false;
	int		iWorldID			= 30;

	GameObjDbStructEx* pZoneObj	= Global::GetObjDB( ( Def_ObjectClass_Zone + ( iType % 1000 ) )  );

	if( pZoneObj != NULL )
	{
		bWorldBattleGround	= pZoneObj->Zone.IsWorldBattleGround;
		iWorldID			= pZoneObj->Zone.iWorldBattleGroundWorldID;
	}

	if( bWorldBattleGround != false )
	{
		PG_BattleGround_WLtoWL_EnterBattleGround	Packet;

		Packet.iClientDBID			= pPlayer->DBID();
		Packet.iType				= iType;
		Packet.iRoomID				= iRoomID;
		Packet.iTeamID				= iTeamID;

		// 取出人物座標及血量, Mana, 以便恢復狀態用
		Packet.iRoleSourceZoneID	= pPlayer->BaseData.ZoneID;
		Packet.iRoleSourceRoomID	= pPlayer->RoomID();

		Packet.fRoleSourceX			= pPlayer->BaseData.Pos.X;
		Packet.fRoleSourceY			= pPlayer->BaseData.Pos.Y;
		Packet.fRoleSourceZ			= pPlayer->BaseData.Pos.Z;
		Packet.fRoleSourceDir		= pPlayer->BaseData.Pos.Dir;

		Packet.fRoleSourceHP		= pPlayer->BaseData.HP;
		Packet.fRoleSourceMP		= pPlayer->BaseData.MP;


		Global::SendToOtherWorld_GSrvID( iWorldID, iType, sizeof( PG_BattleGround_WLtoWL_EnterBattleGround ), &Packet );
	}
	else
	{

		PG_BattleGround_LtoL_EnterBattleGround	Packet;

		Packet.iClientDBID			= pPlayer->DBID();
		Packet.iType				= iType;
		Packet.iRoomID				= iRoomID;
		Packet.iTeamID				= iTeamID;

		// 取出人物座標及血量, Mana, 以便恢復狀態用
		Packet.iRoleSourceZoneID	= pPlayer->BaseData.ZoneID;
		Packet.iRoleSourceRoomID	= pPlayer->RoomID();

		Packet.fRoleSourceX			= pPlayer->BaseData.Pos.X;
		Packet.fRoleSourceY			= pPlayer->BaseData.Pos.Y;
		Packet.fRoleSourceZ			= pPlayer->BaseData.Pos.Z;
		Packet.fRoleSourceDir		= pPlayer->BaseData.Pos.Dir;

		Packet.fRoleSourceHP		= pPlayer->BaseData.HP;
		Packet.fRoleSourceMP		= pPlayer->BaseData.MP;

		// 轉送給戰場
		SendToLocal( iType, sizeof( PG_BattleGround_LtoL_EnterBattleGround ), &Packet );
		//Global::SendToSrvBySockID( iSrvID, sizeof( PG_BattleGround_LtoL_EnterBattleGround ), &Packet );
	}
}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnEvent_ClientLoaded( BaseItemObject* pObj )
{
	StructBattleGroundRoleInfo*			pRoleInfo	= NULL;
	vector<StructBattleGroundRoleInfo>* pTeam		= NULL;

	int iClientDBID = pObj->Role()->DBID();

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		StructBattleGround* pBG = NULL;
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;

			for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
			{
				pTeam = &( pBG->vecTeam[ iTeamID ] );

				for( vector<StructBattleGroundRoleInfo>::iterator itRole = pTeam->begin(); itRole != pTeam->end(); itRole++ )
				{
					pRoleInfo = &(*itRole);
					if( pRoleInfo->iClienDBID == iClientDBID && pRoleInfo->emStatus == EM_BG_ROLEINFO_STATUS_NOTIFIED_WAIT_TO_ENTER_BG )
					{								
						pRoleInfo->emStatus = EM_BG_ROLEINFO_STATUS_IN;


						// 檢查是否有強制組隊的限制
						GameObjDbStructEx* pZoneObj	= Global::GetObjDB( ( Def_ObjectClass_Zone + ( m_BG_iBattleGroundType % 1000 ) )  );

						if( pZoneObj != NULL )
						{
							if( m_IsForceGroup != false )
							{
								// 強制組隊

								int iPartyID = (( pBG->iRoomID + 1 ) * m_BG_iNumTeam ) + iTeamID;
								NetSrv_PartyChild::Zone_Join( pObj->GUID(), iPartyID );
							}
						}


						// 傳送戰場 LandMark 資料給 Client
						if( pBG->iRoomID >= 0 && pBG->iRoomID < ((CNetSrv_BattleGround_Child*)m_pThis)->vecLandMark.size() )
						{
							StructRoomLandMark Info = ((CNetSrv_BattleGround_Child*)m_pThis)->vecLandMark[ pBG->iRoomID ];
							SC_AllLandMarkInfo( pRoleInfo->iRoleWorldID, pRoleInfo->iRoleZoneID, iClientDBID, &Info );
						}


						// 傳送戰場分數給 Client
						if( pBG->iRoomID >= 0 && pBG->iRoomID < ((CNetSrv_BattleGround_Child*)m_pThis)->vecLandMark.size() )
						{
							StructBattleGroundValue Value = ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue[ pBG->iRoomID ];
							SC_BattleGroundStatus( pRoleInfo->iRoleWorldID, pRoleInfo->iRoleZoneID, iClientDBID, &Value );
						}


						for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
						{
							vector<StructBattleGroundRoleInfo>* pTeam = NULL;

							pTeam = &( pBG->vecTeam[ iTeamID ] );

							for( vector<StructBattleGroundRoleInfo>::iterator itRole = pTeam->begin(); itRole != pTeam->end(); itRole++ )
							{
								StructBattleGroundRoleInfo* pRole = &(*itRole);

								if( pRole->emStatus == EM_BG_ROLEINFO_STATUS_IN )
								{
									SC_BattleGroundScore( pRole->iRoleWorldID, pRole->iRoleZoneID, pRole->iClienDBID, pBG->iRoomID );				
								}
							}
						}


						if( m_BG_Lua_ClientLoaded.size() != 0 )
						{
							int		iObjID	= pObj->Role()->DBID();
							char	szLuaBuffer[ 1024 ] ;						

							sprintf( szLuaBuffer, "%s( %d, %d )", m_BG_Lua_ClientLoaded.c_str(), iClientDBID, ( iTeamID + 1 ) );						
							LUA_VMClass::PCallByStrArg( szLuaBuffer, iObjID, iObjID, NULL );
						}

						return;
					}
				}
			}
		}
	}









	// 若執行至此, 表示找不到在競技場裡面找不到該角色, 列印除錯訊息
	Print( LV3, "BG", "OnEvent_ClientLoaded() Cant find battleObj data. DBID[ %d ]", iClientDBID );

}
//----------------------------------------------------------------
bool CNetSrv_BattleGround_Child::CheckRoleIsExistBattleGround( int iClientDBID )
{
	StructBattleGroundRoleInfo* pRoleInfo = NULL;

	bool bExist = false;

	// 若角色的 DBID 已經存在於戰場中
	vector<StructBattleGroundRoleInfo>* pTeam	= NULL;
	StructBattleGround*					pBG		= NULL;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{	
			pBG = *it;

			for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
			{
				pTeam = &( pBG->vecTeam[ iTeamID ] );

				for( vector<StructBattleGroundRoleInfo>::iterator it = pTeam->begin(); it != pTeam->end(); it++ )
				{
					pRoleInfo = &(*it);

					if( pRoleInfo->iClienDBID == iClientDBID && pRoleInfo->emStatus != EM_BG_ROLEINFO_STATUS_NOTIFIED_WAIT_TO_ENTER_BG )
					{
						return true;
					}
				}
			}
		}
	}

	return bExist;
}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRZ_EnterBattleGround ( int iWorldID, int iWorldSrvID, int iClientDBID, int iType, int iRoomID, int iTeamID, int iRoleZoneID, int iRoleRoomID, float fRoleX, float fRoleY, float fRoleZ, float fRoleDir, float fRoleHP, float fRoleMP )
{
	Print( LV2, "BG", "OnRZ_EnterBattleGround() WORLDID[ %d ], SRV[ %d ], DBID[ %d ], TYPE[ %d ], ROOM[ %d ], TEAM[ %d ]", iWorldID, iWorldSrvID, iClientDBID, iType, iRoomID, iTeamID );
	// 檢查是否在隊伍佇列中, 若無, 則無視該需求

	StructBattleGroundRoleInfo* pRoleInfo	= NULL;

	bool						bCheck		= false;
	bool						bExist		= false;



	if( iType != m_BG_iBattleGroundType )
		return;

	if( iWorldID != 0 )
	{
		iClientDBID = iClientDBID + ( iWorldID * _DEF_MAX_DBID_COUNT_ );
	}

	StructBattleGround* pBG = NULL;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;
			if( pBG->iRoomID == iRoomID )
			{
				vector<StructBattleGroundRoleInfo>* pTeam = NULL;

				if( iTeamID >= 0 && iTeamID < m_BG_iNumTeam )
				{		
					pTeam = &( pBG->vecTeam[ iTeamID ] );

					for( vector<StructBattleGroundRoleInfo>::iterator itRole = pTeam->begin(); itRole != pTeam->end(); itRole++ )
					{
						pRoleInfo = &(*itRole);
						if( pRoleInfo->iClienDBID == iClientDBID && pRoleInfo->emStatus == EM_BG_ROLEINFO_STATUS_NOTIFIED_WAIT_TO_ENTER_BG )
						{
							bCheck = true;
							break;
						}
					}
				}			
			}
		}

		if( bCheck != false )
			break;
	}

	bExist = CheckRoleIsExistBattleGround( iClientDBID );

	// 一切 Okay, 通知 Server 將人轉移過來
	if( bCheck != false && bExist == false )
	{
		// 將人物原本座標點記錄下來
		pRoleInfo->iRoleZoneID			= iRoleZoneID;
		pRoleInfo->iRoleRoomID			= iRoleRoomID;
		pRoleInfo->fRoleX				= fRoleX;
		pRoleInfo->fRoleY				= fRoleY;
		pRoleInfo->fRoleZ				= fRoleZ;
		pRoleInfo->fRoleDir				= fRoleDir;
		pRoleInfo->fRoleHP				= fRoleHP;
		pRoleInfo->fRoleMP				= fRoleMP;

		pRoleInfo->iRoleWorldID			= iWorldID;
		pRoleInfo->iRoleWorldSrvID		= iWorldSrvID;		

		if( pRoleInfo->bMoveToBattleGround == false && m_BG_Lua_MoveRoleToBattleGround.size() != 0 )
		{
			pRoleInfo->bMoveToBattleGround = true;

			char szLuaBuffer[ 1024 ];

			sprintf( szLuaBuffer, "%s( %d, %d, %d, %d, %d )", m_BG_Lua_MoveRoleToBattleGround.c_str(), iWorldID, pRoleInfo->iRoleZoneID, iClientDBID, ( iTeamID + 1 ), iRoomID );
			/*
			StructBattleGroundEnterPoint EnterPoint;

			if( iTeamID == 1 )
			EnterPoint = m_EnterPoint[0];
			else
			if( iTeamID == 2 )
			EnterPoint = m_EnterPoint[1];
			*/

			LUA_VMClass::PCallByStrArg( szLuaBuffer, 0, 0, NULL );
		}
	}
	else
	{
		// 戰場中仍有殘留的資料
		Print( LV2, "BG", "OnRZ_EnterBattleGround() roledata exist!! WORLDID[ %d ], SRV[ %d ], DBID[ %d ], TYPE[ %d ], ROOM[ %d ], TEAM[ %d ]", iWorldID, iWorldSrvID, iClientDBID, iType, iRoomID, iTeamID );
	}


}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRZ_ChangeZoneToBattleGround( int iWorldID, int iSrvID, int iClientDBID, int iZoneID, int iRoomID, float X, float Y, float Z, float Dir )
{
	int iDBID = iClientDBID % _DEF_MAX_DBID_COUNT_;

	BaseItemObject* pObj = BaseItemObject::GetObjByDBID( iDBID );

	if( pObj )
	{
		if( iWorldID == 0 )
		{
			Global::ChangeZone( pObj->GUID(), iZoneID, iRoomID, X, Y, Z, Dir );
		}
		else
		{
			RoleDataEx*		pPlayer = pObj->Role();

			Global::ChangeWorld( pObj->GUID(), pPlayer, pPlayer->BaseData.ZoneID, pPlayer->BaseData.Pos,  iWorldID , iZoneID , iRoomID , X , Y , Z ,Dir );
		}		
	}
}


//-------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRC_GetBattleGround( int iClientID, int iObjID )
{
	int				ZoneID	= 0;
	RoleDataEx*		pNPC	= Global::GetRoleDataByGUID( iObjID );
	RoleDataEx*		pPlayer	= Global::GetRoleDataByGUID( iClientID );

	if( pPlayer == NULL || pNPC == NULL && pNPC->TempData.CrystalID == 0 )
		return;

	ZoneID	= pNPC->TempData.CrystalID;

	// 將戰場代號, 發送給 DataCenter 取得戰場資訊
	PG_BattleGround_LtoL_GetBattleGround	Packet;
	Packet.iDBID		= pPlayer->DBID();

	SendToLocal( ZoneID, sizeof(Packet), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRC_JoinBattleGround( BaseItemObject*	pObj, int iObjID, int iBattleGroundID, int iPartyID, int iSrvID, int* pTeamDBID )
{
	int				ZoneID	= 0;
	RoleDataEx*		pNPC	= Global::GetRoleDataByGUID( iObjID );
	RoleDataEx*		pPlayer	= pObj->Role();

	int				iTeamMemberLV[ DF_MAX_TEAMDBID ];

	//if( pPlayer == NULL || pNPC == NULL || pNPC->TempData.CrystalID == 0 )
	//	return;

	bool	bWorldBattleGround	= false;
	int		iWorldID			= 30;

	GameObjDbStructEx* pZoneObj	= Global::GetObjDB( ( Def_ObjectClass_Zone + ( iSrvID % 1000 ) )  );

	if( pZoneObj != NULL )
	{
		bWorldBattleGround	= pZoneObj->Zone.IsWorldBattleGround;
		iWorldID			= pZoneObj->Zone.iWorldBattleGroundWorldID;
	}
	else
	{
		return;
	}

	if( pPlayer == NULL )
		return;

	// 執行加入前檢查劇情, 由劇情主動加入
	//if( iSrvID == 350 || iSrvID == 441 )
	if( strlen( pZoneObj->Zone.LuaPreJoinQueueCheck ) != 0 )
	{
		char								szLuaBuff[1024];
		sprintf( szLuaBuff, "%s( %d, %d, %d, %d )", pZoneObj->Zone.LuaPreJoinQueueCheck, iObjID, iBattleGroundID, iPartyID, iSrvID  );

		vector<MyVMValueStruct> RetList;
		LUA_VMClass::PCallByStrArg( szLuaBuff, 0, 0, &RetList, 1 );

		if( RetList.size() == 1 )
		{
			MyVMValueStruct& Value = RetList[0];
			if( Value.Number == 1 )
			{
				return;
			}
		}
	}








	ZeroMemory( iTeamMemberLV, sizeof( iTeamMemberLV ) );

	if( pTeamDBID != NULL && iPartyID > 0)
	{		
		for( int i=0; i<DF_MAX_TEAMDBID; i++ )
		{
			int						iDBID		= pTeamDBID[ i ];

			if( iDBID != 0 )
			{
				OnlinePlayerInfoStruct* pPlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( iDBID );

				if( pPlayerInfo != NULL )
				{
					iTeamMemberLV[ i ] = pPlayerInfo->LV;
				}
			}
		}
	}





	//if( bWorldBattleGround == 410 )
	if( bWorldBattleGround != false )
	{
		// 將戰場代號, 發送給 DataCenter 取得戰場資訊
		PG_BattleGround_WLtoWL_JoinBattleGround	Packet;

		Packet.iDBID			= pPlayer->DBID();
		Packet.iLV				= pPlayer->Level();
		//Packet.iBattleGroundID	= iBattleGroundID;
		Packet.iBattleGroundID	= 0;
		Packet.iParty			= iPartyID;

		Packet.iZoneID			= pPlayer->BaseData.ZoneID;

		memcpy( Packet.iTeamDBID,		pTeamDBID,		sizeof( Packet.iTeamDBID ) );
		memcpy( Packet.iTeamMemberLV,	iTeamMemberLV,	sizeof( Packet.iTeamDBID ) );

		Packet.iWorldGroupID	= m_BG_iWorldBattleGround;
		//Packet.iRoomID			= pPlayer->RoomID();

		//Packet.fX				= pPlayer->BaseData.Pos.X;
		//Packet.fY				= pPlayer->BaseData.Pos.Y;
		//Packet.fZ				= pPlayer->BaseData.Pos.Z;
		//Packet.fDir				= pPlayer->BaseData.Pos.Dir;



		Global::SendToOtherWorld_GSrvID( iWorldID, iSrvID, sizeof(Packet), &Packet );
	} 
	else
	{
		// 將戰場代號, 發送給 DataCenter 取得戰場資訊
		PG_BattleGround_LtoL_JoinBattleGround	Packet;

		Packet.iDBID			= pPlayer->DBID();
		Packet.iLV				= pPlayer->Level();
		//Packet.iBattleGroundID	= iBattleGroundID;
		Packet.iBattleGroundID	= 0;
		Packet.iParty			= iPartyID;

		Packet.iZoneID			= pPlayer->BaseData.ZoneID;

		memcpy( Packet.iTeamDBID,		pTeamDBID,		sizeof( Packet.iTeamDBID ) );
		memcpy( Packet.iTeamMemberLV,	iTeamMemberLV,	sizeof( Packet.iTeamDBID ) );

		Packet.iWorldGroupID	= m_BG_iWorldGroupID;
		//Packet.iRoomID			= pPlayer->RoomID();

		//Packet.fX				= pPlayer->BaseData.Pos.X;
		//Packet.fY				= pPlayer->BaseData.Pos.Y;
		//Packet.fZ				= pPlayer->BaseData.Pos.Z;
		//Packet.fDir				= pPlayer->BaseData.Pos.Dir;

		SendToLocal( iSrvID, sizeof(Packet), &Packet );
	}
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRZ_GetBattleGround( int iWorldID, int iWorldSrvID, int iClientDBID )
{

	if( Ini()->Int( "BattleGround" ) != 1 )
	{
		return;
	}

	/*
	int		iPacketSize		= 0;
	int		iPerBGSize		= _MAX_OBJ_NAME_STR_SIZE_ + sizeof( int ); // String 32, RoomID 4
	BYTE*	pPacket			= NULL;
	BYTE*	ptr				= NULL;
	*/


	// 將資訊整理起來發封包給 Client	
	// PacketID, Count, Data * Count

	//int		iCount			= m_vecBattleGround.size();

	/*
	int iCount = 0;
	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
	iCount += m_vecBattleGround[ iLVID ].size();
	}



	iPacketSize				= ( sizeof( DWORD ) * 3 ) + ( iPerBGSize * iCount );
	pPacket					= NEW BYTE[ iPacketSize ];
	ptr						= pPacket;

	// PacketID
	*((DWORD*)ptr)			= EM_PG_BattleGround_LtoC_GetBattleGroundResult;
	ptr						+= sizeof(DWORD);

	// Count
	*((DWORD*)ptr)			= iCount;
	ptr						+= sizeof(DWORD);

	// Type
	*((DWORD*)ptr)			= m_BG_iBattleGroundType;
	ptr						+= sizeof(DWORD);


	for( vector< StructBattleGround* >::iterator it = m_vecBattleGround.begin(); it != m_vecBattleGround.end(); it++ )
	{
	memcpy( (PVOID)ptr, &((*it)->iRoomID), sizeof( int ) );
	ptr	 += sizeof( int );

	memcpy( (PVOID)ptr, (*it)->szName.Begin(), _MAX_OBJ_NAME_STR_SIZE_ );
	ptr	 += _MAX_OBJ_NAME_STR_SIZE_;
	}

	*/

	PG_BattleGround_LtoC_GetBattleGroundResult Packet;

	Packet.iBattleGroundID		= m_BG_iBattleGroundType;

	Packet.IsLimitJoinLevel		= m_IsLimitJoinLevel;
	Packet.iMaxJoinLevel		= m_iMaxJoinLevel;
	Packet.iMinJoinLevel		= m_iMinJoinLevel;

	Packet.IsForceGroup			= m_IsForceGroup;

	Packet.iNumTeamPeople		= m_BG_iNumTeamPeople;
	Packet.iNumTeam				= m_BG_iNumTeam;


	// 發送戰場資料給 Client
	if( iWorldID == 0 )
	{
		SendToCli_ByDBID( iClientDBID, sizeof( Packet ), &Packet );
	}
	else
	{
		Global::SendToOtherWorldSrvClient_ByDBID( iWorldID, iWorldSrvID, iClientDBID, sizeof(Packet), &Packet );
	}



	// 檢查若該 Client 在本戰場, 發送一次排隊狀況給 Client 
	bool bFound = false;

	for( int iQueueID = 0; iQueueID < m_BG_iNumTeam; iQueueID++ )
	{	
		vector< StructBattleGroundQueue >* pQueue = NULL;


		for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
		{
			pQueue = &( m_vecWaitQueue[ iLVID ][ iQueueID ] );

			int iCount = 0;

			for( vector< StructBattleGroundQueue >::iterator it = pQueue->begin(); it != pQueue->end(); it++ )
			{
				if( (*it).iDBID == iClientDBID )
				{
					// 送出通知封包, 告訴 Client 等待的情況
					CNetSrv_BattleGround_Child::SC_BattleGroundWaitQueueStatus( (*it).iSourceWorldID, (*it).iSourceZoneID, iClientDBID, m_BG_iBattleGroundType, 0, iCount, RoleDataEx::G_SysTime_Base );
					iCount++;
					bFound = true;			
					break;
				}
			}

			if( bFound != false )
				break;
		}
	}
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRZ_JoinBattleGround( int iWorldID, int iWorldSrvID, int iClientLV, int iClientDBID, int iBattleGroundID, int isParty, int iZoneID, int iWorldGroupID, int *pTeamDBID, int *pTeamMemberLV )//, int iRoomID, float fX, float fY, float fZ, float fDir )
{

	if( Ini()->Int( "BattleGround" ) != 1 )
	{
		Print( LV5, "BG", "OnRZ_JoinBattleGround() but zone isnt battleground" );
		return;
	}

	Print( LV2, "BG", "OnRZ_JoinBattleGround() DBID[ %d ], BGID[ %d ], PartyID[ %d ], ZoneID [ %d ]", iClientDBID, iBattleGroundID, isParty, iZoneID );


	if( m_BG_iWorldBattleGround == 1 )
	{
		iClientDBID += iWorldID * _DEF_MAX_DBID_COUNT_; 
	}

	if( isParty > 0 )
	{
		//int iPartySize = 0;	// 要取得隊伍內正確的人數
		// 若是隊伍, 則發封包給 Chat Server, 取得隊伍資料. 並將該隊伍的資訊推入等待佇列
		//PartyInfoStruct* pParty = PartyInfoListClass::This()->GetPartyInfo( iPartyID );

		/*
		if( pParty == NULL )
		{
		SC_JoinQueueResult( iWorldID, iZoneID, iClientDBID, m_BG_iBattleGroundType, iBattleGroundID, 0, "", 0, (int)EM_BG_JOINQUEUE_RESULT_NOPARTY );
		}
		else
		{
		for( vector< PartyMemberInfoStruct >::iterator it = pParty->Member.begin(); it != pParty->Member.end(); it++ )
		{
		PartyMemberInfoStruct* pMember = &(*it);
		//JoinBattleGroundQueue( iWorldID, iZoneID, pMember->DBID, iBattleGroundID, iPartyID, iPartySize, iZoneID, iRoomID, fX, fY, fZ, fDir );
		JoinBattleGroundQueue( iWorldID, iZoneID, pMember->DBID, iBattleGroundID, iPartyID, iPartySize, iZoneID );// iRoomID, fX, fY, fZ, fDir );
		}


		}
		*/

		// 檢查

		int		iAssignQueueID	= -1;
		bool	bResult			= true;

		for( int i = 0; i < isParty; i++ )
		{
			int iDBID	= pTeamDBID[ i ];



			int iLV		= pTeamMemberLV[ i ];

			if( iDBID == 0 || iLV == 0 )
				continue;

			if( m_BG_iWorldBattleGround == 1 )
			{
				iDBID += iWorldID * _DEF_MAX_DBID_COUNT_; 
			}

			StructBattleGroundRoleInfo BGRole;

			
			int iRoomID = SearchRoleInBattleGround( iDBID, &BGRole );
			if(  iRoomID != 0 )
			{
				char szBuff[32];
				sprintf( szBuff, "%d", ( iDBID % _DEF_MAX_DBID_COUNT_ ) );


				if( BGRole.emStatus  == EM_BG_ROLEINFO_STATUS_NOTIFIED_WAIT_TO_ENTER_BG )
				{
					// 發送給隊長
					SC_JoinQueueResult( iWorldID, iZoneID, iClientDBID, m_BG_iBattleGroundType, iRoomID, 0, szBuff, 0, (int)EM_BG_JOINQUEUE_RESULT_TEAM_WAITENTERBG );

					// 發送給組員
					SC_JoinQueueResult( iWorldID, iZoneID, iDBID, m_BG_iBattleGroundType, iRoomID, 0, "", 0, (int)EM_BG_JOINQUEUE_RESULT_TEAM_MEMBER_WAITBG );

					bResult = false;

					break;
				}
				else
				{
					// 發送給隊長
					SC_JoinQueueResult( iWorldID, iZoneID, iClientDBID, m_BG_iBattleGroundType, iRoomID, 0, szBuff, 0, (int)EM_BG_JOINQUEUE_RESULT_TEAM_INBG );

					// 發送給組員
					SC_JoinQueueResult( iWorldID, iZoneID, iDBID, m_BG_iBattleGroundType, iRoomID, 0, "", 0, (int)EM_BG_JOINQUEUE_RESULT_TEAM_MEMBER_INBG );

					bResult = false;

					break;
				}
			}
		}

		if( bResult == true )
		{
			for( int i = 0; i < isParty; i++ )
			{
				int iDBID	= pTeamDBID[ i ];

				int iLV		= pTeamMemberLV[ i ];

				if( iDBID == 0 || iLV == 0 )
					continue;

				if( m_BG_iWorldBattleGround == 1 )
				{
					iDBID += iWorldID * _DEF_MAX_DBID_COUNT_; 
				}

				if( bResult != false )
				{
					JoinBattleGroundQueue( iWorldID, iWorldSrvID, iWorldGroupID, iLV, iDBID, iBattleGroundID, pTeamDBID[0], iZoneID, &iAssignQueueID ); //, iRoomID, fX, fY, fZ, fDir );
				}			
			}
			OnEvent_CheckBattleGroundQueue();
		}
	}
	else
	{
		int iAssignQueueID = -1;
		JoinBattleGroundQueue( iWorldID, iWorldSrvID, iWorldGroupID, iClientLV, iClientDBID, iBattleGroundID, 0, iZoneID, &iAssignQueueID ); //, iRoomID, fX, fY, fZ, fDir );
		OnEvent_CheckBattleGroundQueue();
	}
}
//-------------------------------------------------------------------
int CNetSrv_BattleGround_Child::SearchRoleInBattleGround( int iClientDBID, StructBattleGroundRoleInfo* pRoleInfo )
{
	// 檢查該 Client 是否在佇列中或等待進入戰場中, 若是, 則將其移除
	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			StructBattleGround*						pBG		= *it;
			bool									bFound	= false;				
			vector<StructBattleGroundRoleInfo>*		pTeam	= NULL;

			for( int i = 0; i < m_BG_iNumTeam; i++ )
			{
				int iTeamID = i;
				pTeam = &( pBG->vecTeam[ iTeamID ] );

				for( vector<StructBattleGroundRoleInfo>::iterator it = pTeam->begin(); it != pTeam->end(); it++ )
				{
					StructBattleGroundRoleInfo BGRole = *it;

					if( BGRole.iClienDBID == iClientDBID )
					{
						if( pRoleInfo != NULL )
						{
							*pRoleInfo = BGRole;
							return pBG->iRoomID;
						}
					}
				}
			}
		}
	}

	return 0;
}
//-------------------------------------------------------------------
bool CNetSrv_BattleGround_Child::SearchRoleInWaitQueue( int iClientDBID, StructBattleGroundQueue* pQueueObj, bool bEraseIfExist )
{
	// 檢查是否已在佇列中了
	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( int iQueueID = 0; iQueueID < m_BG_iNumTeam; iQueueID++ )
		{
			bool bFound = false;
			vector< StructBattleGroundQueue >* pQueue = NULL;

			pQueue = &( m_vecWaitQueue[ iLVID ][ iQueueID ] );

			for( vector< StructBattleGroundQueue >::iterator it = pQueue->begin(); it != pQueue->end(); it++ )
			{
				StructBattleGroundQueue Obj = *it;

				if( Obj.iDBID == iClientDBID )
				{
					if( pQueueObj != NULL )
					{
						*pQueueObj = Obj;
					}

					if( bEraseIfExist != false )
					{
						pQueue->erase( it );
					}

					return true;
				}
			}
		}
	}
	return false;
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround_Child::JoinBattleGroundQueue( int iWorldID, int iWorldSrvID, int iWorldGroupID, int iLV, int iClientDBID, int iBattleGroundID, int iPartyLeaderDBID,  int iZoneID, int* pAssignQueueID )
{
	int iAssignQueueID = -1;

	if( pAssignQueueID != NULL )
	{
		iAssignQueueID = *pAssignQueueID;
	}



	StructBattleGroundQueue				QueueObj;

	QueueObj.iBattleGroundID		= iBattleGroundID;
	//QueueObj.fSourceX				= fX;
	//QueueObj.fSourceY				= fY;
	//QueueObj.fSourceZ				= fZ;
	//QueueObj.fSourceDir				= fDir;
	QueueObj.iSourceZoneID			= iZoneID;
	//QueueObj.iSourceRoomID			= iRoomID;
	QueueObj.iBeginDisconnectTime	= 0;
	QueueObj.iPartyLeaderDBID		= iPartyLeaderDBID;

	QueueObj.iLV					= iLV;
	QueueObj.iDBID					= iClientDBID;

	QueueObj.iWorldGroupID			= iWorldGroupID;

	QueueObj.iSourceWorldID			= iWorldID;
	QueueObj.iSourceWorldSrvID		= iWorldSrvID;
	QueueObj.emStatus				= EM_BG_ROLEINFO_STATUS_QUEUE;

	StructBattleGroundRoleInfo BGRole;
	int iRoomID = SearchRoleInBattleGround( iClientDBID, &BGRole );
	if( iRoomID != 0 )
	{
		if( BGRole.emStatus  == EM_BG_ROLEINFO_STATUS_NOTIFIED_WAIT_TO_ENTER_BG )
		{
			SC_JoinQueueResult( iWorldID, iZoneID, iClientDBID, m_BG_iBattleGroundType, iRoomID, 0, "", 0, (int)EM_BG_JOINQUEUE_RESULT_WAITENTERBG );
			return;
		}
		else
		{
			SC_JoinQueueResult( iWorldID, iZoneID, iClientDBID, m_BG_iBattleGroundType, iRoomID, 0, "", 0, (int)EM_BG_JOINQUEUE_RESULT_INBG );
			return;
		}
	}


	StructBattleGroundQueue	Obj;
	if( SearchRoleInWaitQueue( iClientDBID, &Obj, true ) != false )
	{		
		SC_JoinQueueResult( iWorldID, iZoneID, iClientDBID, m_BG_iBattleGroundType, iBattleGroundID, 0, "", 0, (int)EM_BG_JOINQUEUE_RESULT_INQUEUE );	// 只是通知, 但仍重新加入佇列
	}



	int iLevelGroup			= 0;
	int iLevelGroupID		= 0;

	// 決定等級群組
	//---------------------------------------------------------------------
	if( m_BG_Lua_AssignLevelGroup.size() != 0 )
	{
		char								szLuaBuff[1024];
		sprintf( szLuaBuff, "%s( %d, %d, %d )", m_BG_Lua_AssignLevelGroup.c_str(), iClientDBID, iLV, iWorldGroupID );

		vector<MyVMValueStruct> RetList;
		LUA_VMClass::PCallByStrArg( szLuaBuff, 0, 0, &RetList, 1 );

		if( RetList.size() > 0 )
		{
			MyVMValueStruct& Value = RetList[0];
			if( RetList.size() == 1 )
			{
				iLevelGroup		= Value.Number;
			}

			if( iLevelGroup >= 1 && iLevelGroup <= 10 )
			{
				iLevelGroupID	= iLevelGroup - 1;
			}
			else
			{
				SC_JoinQueueResult( iWorldID, iZoneID, iClientDBID, m_BG_iBattleGroundType, iBattleGroundID, 0, "", 0, (int)EM_BG_JOINQUEUE_RESULT_LVERROR );
				return;
			}
		}
		else
		{
			SC_JoinQueueResult( iWorldID, iZoneID, iClientDBID, m_BG_iBattleGroundType, iBattleGroundID, 0, "", 0, (int)EM_BG_JOINQUEUE_RESULT_LVERROR );
			return;
		}
	}
	//---------------------------------------------------------------------


	// 決定佇列
	//---------------------------------------------------------------------
	if( m_BG_Lua_RoleJoinQueue.size() != 0 )
	{
		int									iQueueID		= 0;
		vector< StructBattleGroundQueue >*	pQueue			= NULL;
		int									iPartySize		= 0;
		int									IsParty			= 0;


		if( iAssignQueueID == -1 )
		{
			char								szLuaBuff[1024];

			sprintf( szLuaBuff, "%s( %d, %d, %d, %d )", m_BG_Lua_RoleJoinQueue.c_str(), iClientDBID, m_BG_iWorldBattleGround, iWorldGroupID, iLevelGroupID );

			vector<MyVMValueStruct> RetList;
			LUA_VMClass::PCallByStrArg( szLuaBuff, 0, 0, &RetList, 1 );

			if( RetList.size() > 0 )
			{
				MyVMValueStruct& Value = RetList[0];
				if( RetList.size() == 1 )
				{
					iQueueID		= ( Value.Number - 1 );

					if( pAssignQueueID != NULL )					
					{
						*pAssignQueueID	= iQueueID;
					}
				}
			}
			else
			{
				Print( LV2, "BattleGround", "JoinBattleGroundQueue() call m_BG_Lua_RoleJoinQueue lua [ %s ], no return value", m_BG_Lua_RoleJoinQueue.c_str() );
			}
		}
		else
		{
			iQueueID = iAssignQueueID;
		}

		if( iQueueID >= 0 && iQueueID < m_BG_iNumTeam )
		{
			pQueue = &( m_vecWaitQueue[ iLevelGroupID ][ iQueueID ] );				
			QueueObj.iLastCheckQueueTime = RoleDataEx::G_SysTime_Base;				
			pQueue->push_back( QueueObj );

			// 發送封包給 Client 給予第一次值, 方便計算等待時間
			int iQueueSize = (int)pQueue->size();
			CNetSrv_BattleGround_Child::SC_BattleGroundWaitQueueStatus( iWorldID, iZoneID, QueueObj.iDBID, m_BG_iBattleGroundType, 0, iQueueSize, RoleDataEx::G_SysTime_Base );
		}
	}
	//---------------------------------------------------------------------

	// Check queue
	//OnEvent_CheckBattleGroundQueue();
}
//-------------------------------------------------------------------
int	CNetSrv_BattleGround_Child::OnTime_CheckBattleGroundQueue( SchedularInfo* Obj, void* InputClass )
{
	((CNetSrv_BattleGround_Child*)m_pThis)->OnEvent_CheckDisconnect();

	// 檢查等待進入戰場的佇列
	((CNetSrv_BattleGround_Child*)m_pThis)->OnEvent_CheckBattleGroundQueue();


	((CNetSrv_BattleGround_Child*)m_pThis)->SC_AllPlayerInfo();


	Global::Schedular()->Push( OnTime_CheckBattleGroundQueue , 10000, NULL, NULL );	
	return 0;
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnEvent_CheckBattleGroundQueue()
{
	// 通知排隊的人, 他們目前的等待順序
	CheckWaitQueue();

	// 檢查戰場內的人物狀況, 檢查斷線及已通知但未進入的人
	CheckBattleGround();

	// 檢查佇列中等待的房間是否已經清空了. 有缺是否補人
	CheckQueue();

	// 檢查空戰場
	DestoryEmptyBG();

	// 檢查佇列中的人數夠不夠成立新的戰場
	CheckQueueToCreateBattleGround();
}
//-------------------------------------------------------------------
void CNetSrv_BattleGround_Child::CheckQueueToCreateBattleGround()
{
	// 目前規則, Queue 中有人, 且人物達到 m_BG_iNumTeamPeople, 所有隊伍 Queue 都成立, 則成立新戰場, 並將之排入戰場佇列中


	int iCount1 = 0;
	int iCount2 = 0;

	vector< StructBattleGroundQueue > vecEnterQueue;

	//bool bBreak = false;

	//while( bBreak != true )
	{
		// 事先把佇列中的人物 DBID 取出, 梢後戰場能夠成立時, 依照 DBID 取出人物
		int iLVID = 0;
		while( iLVID < DF_BG_MAXLVGROUP )
		{
			vector< int > vecTeamPlayer[ DF_BG_MAXTEAM ];

			for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
			{
				vector< StructBattleGroundQueue >::iterator itQueue = m_vecWaitQueue[ iLVID ][ iTeamID ].begin();

				while( itQueue != m_vecWaitQueue[ iLVID ][ iTeamID ].end() )
				{
					if( vecTeamPlayer[ iTeamID ].size() < m_BG_iNumTeamPeople )
					{
						if( (*itQueue).iPartyLeaderDBID != 0 )
						{
							vector< int >	vecPartyQueue;
							int				iPartyLeaderDBID = (*itQueue).iPartyLeaderDBID;

							// 團排, 檢查後面連續的人數有多少, 若全部加入後能夠符合條件, 則執行一個迴圈將人全部拉入
							for( vector< StructBattleGroundQueue >::iterator itParty = itQueue; itParty != m_vecWaitQueue[ iLVID ][ iTeamID ].end(); itParty++ )
							{
								if( (*itParty).iPartyLeaderDBID == iPartyLeaderDBID )
								{
									vecPartyQueue.push_back( (*itParty).iDBID );
								}
								else
								{
									break;
								}
							}

							bool	bMerge				= false;
							int		iNumPartyJoinQueue	= 0;

							// 檢查合理性
							int iTeamSize	= (int)vecTeamPlayer[ iTeamID ].size();
							int iPartySize	= (int)vecPartyQueue.size();

							if( iTeamSize == 0 )
							{							
								if( iPartySize >= m_BG_iNumTeamPeople )
								{								
									bMerge				= true;	// 團排人數超過戰場限制, 將足夠的人拉進佇列
									iNumPartyJoinQueue	= m_BG_iNumTeamPeople;
								}
								else
								{								
									bMerge				= true;	// 人數全部加入仍有空位, 全部加入
									iNumPartyJoinQueue	= iPartySize;
								}
							}
							else
							{
								// 前面還有人, 若隊伍人數加上已經在佇列的人超過上限, 則不將隊伍的人併入
								int iTotalSize = iTeamSize + iPartySize;
								if( iTotalSize <= m_BG_iNumTeamPeople )
								{
									bMerge				= true;
									iNumPartyJoinQueue	= iPartySize;
								}
							}

							for( int i = 0; i < iNumPartyJoinQueue; i++ )
							{
								itQueue++;
							}

							if( bMerge != false )
							{
								int iCount = 0;

								for( vector< int >::iterator it = vecPartyQueue.begin(); it != vecPartyQueue.end(); it++ )
								{
									vecTeamPlayer[ iTeamID ].push_back( (*it) );
									iCount++;

									if( iCount == iNumPartyJoinQueue )
										break;
								}
							}
							else
							{
								for( int i = 0; i < iPartySize; i++ )
								{
									itQueue++;
								}
							}
						}
						else
						{
							vecTeamPlayer[ iTeamID ].push_back( (*itQueue).iDBID );
							itQueue++;
						}
					}
					else
						break;			
				}

				/*
				for( vector< StructBattleGroundQueue >::iterator itQueue = m_vecWaitQueue[ iLVID ][ iTeamID ].begin(); itQueue != m_vecWaitQueue[ iLVID ][ iTeamID ].end(); itQueue++ )
				{
				if( (*itQueue).iBattleGroundID == 0 )
				{
				if( vecTeamPlayer[ iTeamID ].size() < m_BG_iNumTeamPeople )
				{
				vecTeamPlayer[ iTeamID ].push_back( (*itQueue).iDBID );
				}
				}
				}
				*/
			}


			// 檢查人數夠不夠開新層,
			// 2010/11/30 added 最低開局人數

			bool bEnough = true;
			for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
			{
				if( vecTeamPlayer[ iTeamID ].size() != m_BG_iNumTeamPeople )
				{
					if( m_BG_iMinTeamPeople == 0 )
					{
						bEnough = false;
						break;
					}
					else
					if( vecTeamPlayer[ iTeamID ].size() >= m_BG_iMinTeamPeople )
					{
						bEnough = true;
						break;
					}
					else
					{
						bEnough = false;
					}
				}
			}

			if( bEnough != false )
			{
				// 人數足夠開啟戰場
				// 產生新房間, 並通知玩家進入

				// 找出空間代號, 並建立戰場
				//-----------------------------------------------------------
				int iID = 0;

				set< int >::iterator itID = setFreeID.begin();

				// 若是還有 ID 表示還未達到戰場上限
				if( itID != setFreeID.end() )
				{
					iID = *itID;
					setFreeID.erase( itID );

					StructBattleGround* pBG = NEW StructBattleGround;

					pBG->iRoomID				= iID;
					pBG->emBattleGroundStatus	= EM_BG_STATUS_RUNNING;

					m_vecBattleGround[ iLVID ].push_back( pBG );


					// 清除戰場變數
					StructBattleGroundValue Data;
					ZeroMemory( &Data, sizeof( StructBattleGroundValue) );
					((CNetSrv_BattleGround_Child*)m_pThis)->vecValue[ iID ] = Data;

					StructRoomLandMark LandMarkInfo;
					ZeroMemory( &LandMarkInfo, sizeof( StructRoomLandMark) );
					((CNetSrv_BattleGround_Child*)m_pThis)->vecLandMark[ iID ] = LandMarkInfo;

					// 建立空間
					Global::CopyRoomMonster( 0, iID );
					Print( LV2, "BG", "Create BattlegroundRoom[ %d ]", iID );

					// 發知通出給 Client, 通知 Client 可以進入戰場了
					//-----------------------------------------------------------
					for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
					{
						vector< int >& vecTeamDBID = vecTeamPlayer[ iTeamID ];

						InvitePlayer( pBG, vecTeamDBID, iLVID, iTeamID );
					}
				}
				else
				{
					iLVID++;
				}
			}
			else
			{
				iLVID++;
			}
		}
	}
}
//------------------------------------------------------------------------------------------------
void CNetSrv_BattleGround_Child::InvitePlayer( StructBattleGround* pBG, vector< int >& vecDBID, int iLVID, int iTeamID )
{
	vector< StructBattleGroundQueue >* pQueue = NULL;

	pQueue = &( m_vecWaitQueue[ iLVID ][ iTeamID ] );

	for( vector< int >::iterator itDBID = vecDBID.begin(); itDBID != vecDBID.end(); itDBID++ )
	{
		vector< StructBattleGroundQueue >::iterator itQueueObj = pQueue->begin();

		while( itQueueObj != pQueue->end() )
		{
			StructBattleGroundQueue* pQueueObj = &( *itQueueObj );

			if( pQueueObj->iDBID == *itDBID )
			{
				// 將之從佇列中刪除, 並將之移入戰場隊伍中, 並設定狀態
				StructBattleGroundRoleInfo		BattleGroundObj;

				BattleGroundObj.emStatus				= EM_BG_ROLEINFO_STATUS_NOTIFIED_WAIT_TO_ENTER_BG;
				BattleGroundObj.iBeginWaitTime			= GetTickCount();
				BattleGroundObj.iClienDBID				= pQueueObj->iDBID;

				BattleGroundObj.iRoleWorldID			= pQueueObj->iSourceWorldID;
				BattleGroundObj.iRoleWorldSrvID			= pQueueObj->iSourceWorldSrvID;

				BattleGroundObj.fRoleX					= 0;	// pQueueObj->fSourceX;
				BattleGroundObj.fRoleY					= 0;	// pQueueObj->fSourceY;
				BattleGroundObj.fRoleZ					= 0;	// pQueueObj->fSourceZ;
				BattleGroundObj.fRoleDir				= 0;	// pQueueObj->fSourceDir;

				BattleGroundObj.iRoleZoneID				= pQueueObj->iSourceZoneID;
				BattleGroundObj.iRoleRoomID				= 0;	// pQueueObj->iSourceRoomID;

				BattleGroundObj.iBeginDisconnectTime	= 0;

				pBG->vecTeam[ iTeamID ].push_back( BattleGroundObj );

				// 發送資訊給 Client, 通知他可以進入了
				SC_EnterBattleGroundRight( BattleGroundObj.iRoleWorldID, BattleGroundObj.iRoleZoneID, BattleGroundObj.iClienDBID, m_BG_iBattleGroundType, pBG->iRoomID, iTeamID, pBG->szName.Begin(), DF_BG_WAIT_ENTER_TIME );

				itQueueObj = pQueue->erase( itQueueObj );
			}
			else
				itQueueObj++;
		}	
	}
}


void CNetSrv_BattleGround_Child::SendTicketToClient( StructBattleGround* pBG, vector<StructBattleGroundRoleInfo>* pTeam,  vector< StructBattleGroundQueue >* pvecQueue, int iTeamID )
{
	StructBattleGroundRoleInfo		BGRole;
	int								iCount = 0;

	while(1)
	{
		vector< StructBattleGroundQueue >::iterator itQueue = pvecQueue->begin();

		if( itQueue != pvecQueue->end() )
		{
			StructBattleGroundQueue Obj = *itQueue;

			if( Obj.iBattleGroundID == 0 )
			{
				// 找出隊友數量
				//int iSize = (*itQueue).vecClientID.size();
				int iSize = 1;


				if( ( iCount + iSize ) <= m_BG_iNumTeamPeople )
				{
					iCount += iSize;			

					BGRole.emStatus				= EM_BG_ROLEINFO_STATUS_NOTIFIED_WAIT_TO_ENTER_BG;
					BGRole.iBeginWaitTime		= GetTickCount();
					BGRole.iClienDBID			= Obj.iDBID;

					BGRole.fRoleX				= 0;	// Obj.fSourceX;
					BGRole.fRoleY				= 0;	// Obj.fSourceY;
					BGRole.fRoleZ				= 0;	// Obj.fSourceZ;
					BGRole.fRoleDir				= 0;	// Obj.fSourceDir;

					BGRole.iRoleZoneID			= Obj.iSourceZoneID;
					BGRole.iRoleRoomID			= 0;	// Obj.iSourceRoomID;

					BGRole.iBeginDisconnectTime	= 0;


					pTeam->push_back( BGRole );	

					SC_EnterBattleGroundRight( BGRole.iRoleWorldID, BGRole.iRoleZoneID, BGRole.iClienDBID, m_BG_iBattleGroundType, pBG->iRoomID, iTeamID, pBG->szName.Begin(), DF_BG_WAIT_ENTER_TIME );

					itQueue = pvecQueue->erase( itQueue );

					if( iCount == m_BG_iNumTeamPeople )
					{
						break;
					}
				}
			}
		}
		else
		{
			break;
		}
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------
//void CNetSrv_BattleGround_Child::CheckQueue( vector< StructBattleGroundQueue >* pvecQueue, int iTeamID )
void CNetSrv_BattleGround_Child::CheckQueue()
{
	// 檢查等待佇列中. 是否等帶待的戰場已經空了, 若已經空了, 則發出封包告知佇列中的人. 該戰場已經結束

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( int iQueueID = 0; iQueueID < m_BG_iNumTeam; iQueueID++ )
		{
			int									iTeamID		= iQueueID;
			vector< StructBattleGroundQueue >*	pvecQueue	= &( m_vecWaitQueue[ iLVID ][ iQueueID ] );

			vector< StructBattleGroundQueue >::iterator itQueue = pvecQueue->begin();

			while( 1 )
			{
				if( itQueue == pvecQueue->end() )
				{
					break;
				}

				int iID = (*itQueue).iBattleGroundID;
				if( setFreeID.find( iID ) != setFreeID.end() && iID != 0 )
				{
					int iClientDBID = (*itQueue).iDBID;
					SC_EmptyBattleGround( (*itQueue).iSourceWorldID, (*itQueue).iSourceZoneID, iClientDBID, m_BG_iBattleGroundType, iID );
					itQueue = pvecQueue->erase( itQueue );
				}
				else
				{
					itQueue++;
				}
			}
		}
	}

	// 檢查戰場中是否有缺人, 有的話則從 Queue 中補人 , 補缺額不從隊伍排隊中找人
	// m_BG_iFillFromQueue 表示是否遇缺補人, 因為有些特殊規則, 遇缺不補比如說賽馬場
	if( m_BG_iFillFromQueue == 1 )
	{
		for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
		{

			// 先照隊伍人數, 最大和最小相差值最大的先補位
			multimap< int , StructBattleGround* > mapBattleGround;

			for(  vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
			{
				StructBattleGround*					pBG			= *it;
				vector<StructBattleGroundRoleInfo>* pTeam		= NULL;
				int									iTeamSize	= 0;
				int									iMin		= -1;
				int									iMax		= -1;
				int									iOffset		= 0;

				for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
				{
					pTeam		= &( pBG->vecTeam[ iTeamID ] );
					iTeamSize	= (int)pTeam->size();

					if( iTeamSize >= iMax || iMax == -1 )
						iMax = iTeamSize;

					if( iMin >= iTeamSize || iMin == -1 )
						iMin = iTeamSize;					
				}

				iOffset = abs( iMax - iMin );

				

				mapBattleGround.insert( make_pair( iOffset, pBG ) );
			}

			//for(  vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
			for( multimap< int , StructBattleGround* >::iterator it = mapBattleGround.begin(); it != mapBattleGround.end(); it++ )
			{
				//StructBattleGround*					pBG			= *it;
				StructBattleGround*					pBG			= it->second;
				vector<StructBattleGroundRoleInfo>* pTeam		= NULL;
				int									iTeamSize	= 0;

				if( pBG->emBattleGroundStatus != EM_BG_STATUS_RUNNING )
				{
					continue;
				}

				// 檢查若隊伍人數不夠, 則從 Queue 中取出人數
				// 由於在玩家參加時, 就決定了隊伍. 且不同隊伍有不同的佇列, 所以不必先掃描隊伍的人數, 然後在由少的開始補人
				for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
				{
					pTeam		= &( pBG->vecTeam[ iTeamID ] );
					iTeamSize	= (int)pTeam->size();

					// 檢查若隊伍人數不夠, 則從 Queue 中取出人數
					if( iTeamSize < m_BG_iNumTeamPeople && m_vecWaitQueue[ iLVID ][ iTeamID ].size() != 0 )
					{
						StructBattleGroundQueue			Obj;
						StructBattleGroundRoleInfo		BGRole;

						// 將一個單位從 WaitQueue 取出, 檢查該單位是否有指定戰場房間, 若檢查可行, 並將之推進戰場隊伍中

						vector< StructBattleGroundQueue >*				pvecWaitQueue	= &( m_vecWaitQueue[ iLVID ][ iTeamID ] );
						vector< StructBattleGroundQueue >::iterator		it				= pvecWaitQueue->begin();

						vector< StructBattleGroundQueue >				vecFillObj;

						bool	bFill		= false;
						int		iFreeSpace	= m_BG_iNumTeamPeople - iTeamSize;




						while( it != pvecWaitQueue->end() )
						{
							StructBattleGroundQueue		*pObj = &(*it);

							if( pObj->iBattleGroundID == 0 || pObj->iBattleGroundID != pBG->iRoomID )
							{
								// 檢查排隊的人是單人還是多人

								if( pObj->iPartyLeaderDBID == 0 )
								{
									if( iFreeSpace > 0 )
									{
										vecFillObj.push_back( *pObj );

										it		= pvecWaitQueue->erase( it );
										bFill	= true;

										iFreeSpace--;
									}

									break;
								}
								else
								{
									// 多人, 比對隊伍內有多少人, 是否能夠補近戰場
									vector< StructBattleGroundQueue >::iterator		itTeam		= it;
									int												LeaderDBID	= pObj->iPartyLeaderDBID;
									int												Count		= 0;

									while( itTeam != pvecWaitQueue->end() )
									{
										if( (*itTeam).iPartyLeaderDBID == LeaderDBID )
										{
											Count++;
											itTeam++;
										}
										else
											break;
									}

									if( Count <= iFreeSpace )
									{
										bFill	= true;

										while( it != pvecWaitQueue->end() )
										{
											if( (*it).iPartyLeaderDBID == LeaderDBID )
											{
												vecFillObj.push_back( *pObj );
												iFreeSpace--;

												it		= pvecWaitQueue->erase( it );																				
											}
											else
												break;
										}

									}									
									else
									{
										while( it != pvecWaitQueue->end() )
										{
											if( (*it).iPartyLeaderDBID == LeaderDBID )
											{												
												it++;
											}
											else
												break;
										}
									}
								}
							}
							else
								it++;
						}

						if( bFill != false )
						{
							// Battle Ground Debug
							{
								string									sOutput;
								char									szBuf[1024];
								vector<StructBattleGroundRoleInfo>*		pDebugTeam;
								int										iDebugTeamSize;

								for( int iDebugTeamID = 0; iDebugTeamID < m_BG_iNumTeam; iDebugTeamID++ )
								{
									pDebugTeam			= &( pBG->vecTeam[ iDebugTeamID ] );
									iDebugTeamSize		= (int)pDebugTeam->size();

									sprintf( szBuf, "T[%d][%d] ", iDebugTeamID, iDebugTeamSize );
									sOutput += szBuf;
								}

								Print( LV2, "BG_Q", sOutput.c_str() );

								sprintf( szBuf, "assign to team[%d], size[%d]", iTeamID, vecFillObj.size() );
								Print( LV2, "BG_Q", szBuf );
							}


							StructBattleGroundQueue*			pObj;

							for( vector< StructBattleGroundQueue >::iterator it = vecFillObj.begin(); it != vecFillObj.end(); it++ )
							{
								BGRole.emStatus			= EM_BG_ROLEINFO_STATUS_NOTIFIED_WAIT_TO_ENTER_BG;
								BGRole.iBeginWaitTime	= GetTickCount();
								BGRole.iClienDBID		= (*it).iDBID;
								BGRole.iTeamID			= iTeamID;

								BGRole.iRoleZoneID		= (*it).iSourceZoneID;
								//BGRole.iRoleRoomID		= Obj.iSourceRoomID;
								//BGRole.fRoleX			= Obj.fSourceX;
								//BGRole.fRoleY			= Obj.fSourceY;
								//BGRole.fRoleZ			= Obj.fSourceZ;
								//BGRole.fRoleDir			= Obj.fSourceDir;

								BGRole.iRoleWorldID		= (*it).iSourceWorldID;
								BGRole.iRoleWorldSrvID	= (*it).iSourceWorldSrvID;

								pTeam->push_back( BGRole );	

								SC_EnterBattleGroundRight( BGRole.iRoleWorldID, BGRole.iRoleZoneID, (*it).iDBID, m_BG_iBattleGroundType, pBG->iRoomID, iTeamID, pBG->szName.Begin(), DF_BG_WAIT_ENTER_TIME );
								Print( LV2, "BG", "CheckQueue() fill people from queue, DBID[ %d ], TYPE[ %d ], ROOM[ %d ], TEAM[ %d ]", (*it).iDBID, m_BG_iBattleGroundType, pBG->iRoomID, iTeamID );
							}
						}
					}
				}
			}
		}

	}
}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::DestoryEmptyBG()
{
	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin();

		int	iTickCount = GetTickCount();

		while( 1 )
		{
			if( it == m_vecBattleGround[ iLVID ].end() )
				break;

			StructBattleGround* pBG			= *it;
			int					iNumPlayers = 0;

			for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
			{
				//iNumPlayers += pBG->vecTeam[ iTeamID ].size();
				vector<StructBattleGroundRoleInfo>* pTeam = NULL;

				pTeam = &( pBG->vecTeam[ iTeamID ] );

				for( vector<StructBattleGroundRoleInfo>::iterator it = pTeam->begin(); it != pTeam->end(); it++ )
				{
					StructBattleGroundRoleInfo BGRole = *it;

					if( BGRole.emStatus == EM_BG_ROLEINFO_STATUS_NOTIFIED_WAIT_TO_ENTER_BG || BGRole.emStatus == EM_BG_ROLEINFO_STATUS_IN )
					{
						iNumPlayers++;
					}
				}


			}


			if( iNumPlayers == 0 && pBG->emBattleGroundStatus != EM_BG_STATUS_NONE )
			{
				if( pBG->emBattleGroundStatus != EM_BG_STATUS_DESTORING )
				{
					pBG->emBattleGroundStatus	= EM_BG_STATUS_DESTORING;
					pBG->iCloseTime				= iTickCount;


					Global::DelRoomMonster( pBG->iRoomID );
				}

				if( pBG->emBattleGroundStatus == EM_BG_STATUS_DESTORING )
				{


					int iOffset = abs( iTickCount - pBG->iCloseTime );

					if( iOffset > 10000 )
					{

						if( m_IsForceGroup != false )
						{
							for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
							{
								int iPartyID = (( pBG->iRoomID + 1 ) * m_BG_iNumTeam ) + iTeamID;
								NetSrv_PartyChild::Zone_Clear( iPartyID );
							}
						}

						it = m_vecBattleGround[ iLVID ].erase( it );				
						setFreeID.insert( pBG->iRoomID );			
						Print( LV2, "BG", "Destory BattlegroundRoom[ %d ]", pBG->iRoomID );			
						delete pBG;
						continue;
					}
				}

				it++;
			}
			else
			{
				it++;
			}
		}
	}
}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::SetBattleGroundObjClickState( int iObjID, int iTeamID, int iClickState )
{
	/*
	BaseItemObject* pObj = BaseItemObject::GetObj( iObjID );

	if( pObj == NULL )
	return;

	int iRoomID = pObj->Role()->RoomID();

	if( iTeamID == 1 )
	EnterPoint = m_EnterPoint[0];
	else
	if( iTeamID == 2 )
	EnterPoint = m_EnterPoint[1];
	*/




}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::SetScore( int iRoomID, int iTeamID, int iScore )
{	
	int	iTeamIndex	= iTeamID - 1;

	if( iRoomID >= 0 && iRoomID < ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue.size() )
	{
		StructBattleGroundValue Value = ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue[ iRoomID ];

		if( iTeamIndex >= 0 && iTeamIndex < DF_BG_MAXTEAM )
		{
			Value.iTeamScore[ iTeamIndex ] = iScore;
			((CNetSrv_BattleGround_Child*)m_pThis)->vecValue[ iRoomID ] = Value;
		}
		((CNetSrv_BattleGround_Child*)m_pThis)->SendDataToClientInRoom( iRoomID );
	}
}
//----------------------------------------------------------------
int CNetSrv_BattleGround_Child::GetScore( int iRoomID, int iTeamID )
{
	int	iTeamIndex	= iTeamID - 1;

	if( iRoomID >= 0 && iRoomID < ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue.size() )
	{
		StructBattleGroundValue Value = ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue[ iRoomID ];

		if( iTeamIndex >= 0 && iTeamIndex < DF_BG_MAXTEAM )
		{
			return Value.iTeamScore[ iTeamIndex ];
		}
	}
	return 0;
}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::CloseBattleGround( int iRoomID )
{
	StructBattleGround* pBG = NULL;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;

			if( pBG->iRoomID == iRoomID )
			{
				if( pBG->emBattleGroundStatus != EM_BG_STATUS_CLOSING )
				{
					Print( LV3, "BattleGround", "Room closing but emBattleGroundStatus is not EM_BG_STATUS_CLOSING" );
				}

				// 若仍有玩家在戰場, 將之傳送出去, 並將 Room 回收
				for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
				{
					vector<StructBattleGroundRoleInfo>* pTeam = NULL;

					pTeam = &( pBG->vecTeam[ iTeamID ] );

					for( vector<StructBattleGroundRoleInfo>::iterator itRole = pTeam->begin(); itRole != pTeam->end(); itRole++ )
					{
						StructBattleGroundRoleInfo* pRoleInfo = &(*itRole);

						RoleDataEx* pRole = GetRoleDataByDBID( pRoleInfo->iClienDBID );

						if( pRole != NULL && pRoleInfo->emStatus != EM_BG_ROLEINFO_STATUS_DISCONNECT_INBG )
						{
							if( m_BG_iWorldBattleGround == 0 )
							{
								if( LuaPlotClass::ChangeZone( pRole->GUID(), pRoleInfo->iRoleZoneID, pRoleInfo->iRoleRoomID, pRoleInfo->fRoleX, pRoleInfo->fRoleY, pRoleInfo->fRoleZ, pRoleInfo->fRoleDir ) == false )
								{
									Print( LV2, "BattleGround", "ChangeZone() CloseBattleGround - Accounet[ %s ], RoleName[ %s ]", pRole->TempData.Sys.DestroyAccount.Begin(), Global::GetRoleName_ASCII( pRole ).c_str() );

									// 無法傳送至原始地點, 傳送至指定地點
									if( LuaPlotClass::ChangeZone( pRole->GUID(), pRole->SelfData.ReturnZoneID, pRole->RoomID(), pRole->SelfData.ReturnPos.X, pRole->SelfData.ReturnPos.Y, pRole->SelfData.ReturnPos.Z, pRole->SelfData.ReturnPos.Dir ) == false )
									{
										Print( LV3, "BattleGround", "CloseBattleGround() teleport error" );
									}
								}
							}
							else
							{
								//Global::DelFromPartition( pRole->GUID() );
								//NetSrv_CliConnect::SC_WorldReturnNotify( pRole->GUID() );
								NetSrv_CliConnectChild::WorldReturnNotifyProc( pRole->GUID() );
							}
						}
					}

					pTeam->clear();
				}




				/*
				// 若仍有玩家在戰場, 將之傳送出去, 並將 Room 回收
				pBG->emBattleGroundStatus = EM_BG_STATUS_CLOSING;

				// 發送給所有的人. 戰場已經結束

				for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
				{
				vector<StructBattleGroundRoleInfo>* pTeam = NULL;

				pTeam = &( pBG->vecTeam[ iTeamID ] );

				for( vector<StructBattleGroundRoleInfo>::iterator itRole = pTeam->begin(); itRole != pTeam->end(); itRole++ )
				{
				StructBattleGroundRoleInfo* pRoleInfo = &(*itRole);
				SC_BattleGroundClose( pRoleInfo->iClienDBID, iRoomID, iWinTeamID );
				}
				}
				*/
			}
		}
	}

	// Call function to check if battleground is empty 
	DestoryEmptyBG();
}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::EndBattleGround( int iRoomID, int iWinTeamIndex )
{
	Print( LV2, "BattleGround", "EndBattleGround() Room[ %d ], Winteam[ %d ]", iRoomID, iWinTeamIndex );

	StructBattleGround* pBG = NULL;

	// int iWinTeamID = iWinTeamIndex - 1;

	//if( ( iWinTeamID >= 0 && iWinTeamID < m_BG_iNumTeam ) == false )
	//	return;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;

			if( pBG->iRoomID == iRoomID && pBG->emBattleGroundStatus == EM_BG_STATUS_RUNNING )
			{
				pBG->emBattleGroundStatus = EM_BG_STATUS_CLOSING;

				// 發送給所有的人. 戰場已經結束

				for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
				{
					vector<StructBattleGroundRoleInfo>* pTeam = NULL;

					pTeam = &( pBG->vecTeam[ iTeamID ] );

					for( vector<StructBattleGroundRoleInfo>::iterator itRole = pTeam->begin(); itRole != pTeam->end(); itRole++ )
					{
						StructBattleGroundRoleInfo* pRoleInfo = &(*itRole);

						if( pRoleInfo->emStatus == EM_BG_ROLEINFO_STATUS_IN )
						{
							SC_BattleGroundScore( pRoleInfo->iRoleWorldID, pRoleInfo->iRoleZoneID, pRoleInfo->iClienDBID, iRoomID );				
							SC_BattleGroundClose( pRoleInfo->iRoleWorldID, pRoleInfo->iRoleZoneID, pRoleInfo->iClienDBID, iRoomID, ( iWinTeamIndex - 1 ) );
						}
					}
				}
			}
		}
	}
}
//----------------------------------------------------------------
int CNetSrv_BattleGround_Child::GetLandMark( int iRoomID, int iID )
{
	if( iRoomID >= 0 && iRoomID < ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue.size() )
	{
		StructBattleGroundValue Value = ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue[ iRoomID ];
		return Value.iLandMark[ iID ];
	}
}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::SetLandMarkInfo( int iRoomID, int iID, int iVal, float fX, float fY, float fZ, const char* pszName, int iIconID )
{
	iID = iID - 1;	// 以 Zero 為基底
	if( iRoomID >= 0 && iRoomID < ((CNetSrv_BattleGround_Child*)m_pThis)->vecLandMark.size() )
	{
		if( iID >= 0 && iID < 10 )
		{
			StructRoomLandMark Info = ((CNetSrv_BattleGround_Child*)m_pThis)->vecLandMark[ iRoomID ];
			//Value.iLandMark[ iID ] = iVal;
			Info.LandMark[iID].fX			= fX;
			Info.LandMark[iID].fY			= fY;
			Info.LandMark[iID].fZ			= fZ;
			Info.LandMark[iID].Name			= pszName;
			Info.LandMark[iID].iIconID		= iIconID;


			((CNetSrv_BattleGround_Child*)m_pThis)->vecLandMark[ iRoomID ] = Info;
			//SendDataToClientInRoom( iRoomID );
			SendLandInfoToClientInRoom( iRoomID, iID, &(Info.LandMark[iID]) );
			SetLandMark( iRoomID, ( iID + 1 ), iVal );
		}
	}
}

void CNetSrv_BattleGround_Child::SetLandMark( int iRoomID, int iID, int iVal )
{
	iID = iID - 1;
	if( iRoomID >= 0 && iRoomID < ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue.size() )
	{
		StructBattleGroundValue Value = ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue[ iRoomID ];
		Value.iLandMark[ iID ] = iVal;
		((CNetSrv_BattleGround_Child*)m_pThis)->vecValue[ iRoomID ] = Value;
		SendDataToClientInRoom( iRoomID );
	}
}
//----------------------------------------------------------------
int CNetSrv_BattleGround_Child::GetVar( int iRoomID, int iID )
{
	if( iRoomID >= 0 && iRoomID < ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue.size() )
	{
		StructBattleGroundValue Value = ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue[ iRoomID ];

		if( iID >= 0 && iID < DF_MAX_BG_VAR )
		{
			return Value.iVar[ iID ];
		}		
	}
}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::SetVar( int iRoomID, int iID, int iVal )
{
	if( iRoomID >= 0 && iRoomID < ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue.size() )
	{
		StructBattleGroundValue Value = ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue[ iRoomID ];

		if( iID >= 0 && iID < DF_MAX_BG_VAR )
		{
			Value.iVar[ iID ] = iVal;
			((CNetSrv_BattleGround_Child*)m_pThis)->vecValue[ iRoomID ] = Value;
			SendDataToClientInRoom( iRoomID );
		}
	}
}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::SendLandInfoToClientInRoom( int iRoomID, int iID, StructLandMark* pLandMark )
{
	StructBattleGround* pBG = NULL;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;
			if( pBG->iRoomID == iRoomID )
			{
				StructBattleGroundValue Value = ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue[ iRoomID ];

				for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
				{
					vector<StructBattleGroundRoleInfo>* pTeam = NULL;

					pTeam = &( pBG->vecTeam[ iTeamID ] );

					for( vector<StructBattleGroundRoleInfo>::iterator itRole = pTeam->begin(); itRole != pTeam->end(); itRole++ )
					{
						StructBattleGroundRoleInfo* pRoleInfo = &(*itRole);					
						SC_LandMarkInfo( pRoleInfo->iRoleWorldID, pRoleInfo->iRoleZoneID, pRoleInfo->iClienDBID, iID, pLandMark );
					}
				}

			}
		}
	}
}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::SendDataToClientInRoom( int iRoomID )
{
	StructBattleGround* pBG = NULL;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;
			if( pBG->iRoomID == iRoomID )
			{
				StructBattleGroundValue Value = ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue[ iRoomID ];

				for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
				{
					vector<StructBattleGroundRoleInfo>* pTeam = NULL;

					pTeam = &( pBG->vecTeam[ iTeamID ] );

					for( vector<StructBattleGroundRoleInfo>::iterator itRole = pTeam->begin(); itRole != pTeam->end(); itRole++ )
					{
						StructBattleGroundRoleInfo* pRoleInfo = &(*itRole);					
						SC_BattleGroundStatus( pRoleInfo->iRoleWorldID, pRoleInfo->iRoleZoneID, pRoleInfo->iClienDBID, &Value );
					}
				}
			}
		}
	}
}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRC_LeaveBattleGround( BaseItemObject* pObj )
{
	// 找尋該玩家是不是仍在戰場中

	RoleDataEx*			pRole	= pObj->Role();

	if( pRole == NULL )
		return;

	int					iDBID	= pRole->DBID();
	StructBattleGround* pBG		= NULL;
	int					iRoomID	= pRole->RoomID();

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;
			if( pBG->iRoomID == iRoomID )
			{
				StructBattleGroundValue Value = ((CNetSrv_BattleGround_Child*)m_pThis)->vecValue[ iRoomID ];

				//for( int iTeamIndex = 1; iTeamIndex <= 2; iTeamIndex++ )
				for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
				{
					vector<StructBattleGroundRoleInfo>* pTeam = NULL;

					/*
					if( iTeamIndex == 1 )
					pTeam = &( pBG->vecTeam1 );
					else
					if( iTeamIndex == 2 )
					pTeam = &( pBG->vecTeam2 );
					*/

					pTeam = &( pBG->vecTeam[ iTeamID ] );

					for( vector<StructBattleGroundRoleInfo>::iterator itRole = pTeam->begin(); itRole != pTeam->end(); itRole++ )
					{
						StructBattleGroundRoleInfo* pRoleInfo = &(*itRole);

						if( pRoleInfo->iClienDBID == iDBID )
						{
							bool bLeaveZone = false;

							if( m_BG_iWorldBattleGround == 0 )
							{
								bLeaveZone = LuaPlotClass::ChangeZone( pObj->GUID(), pRoleInfo->iRoleZoneID, pRoleInfo->iRoleRoomID, pRoleInfo->fRoleX, pRoleInfo->fRoleY, pRoleInfo->fRoleZ, pRoleInfo->fRoleDir );

								if( bLeaveZone == false )
								{
									bLeaveZone = LuaPlotClass::ChangeZone( pObj->GUID(), pRole->SelfData.ReturnZoneID, pRole->RoomID(), pRole->SelfData.ReturnPos.X, pRole->SelfData.ReturnPos.Y, pRole->SelfData.ReturnPos.Z, pRole->SelfData.ReturnPos.Dir );
								}

								Print( LV2, "BattleGround", "OnRC_LeaveBattleGround() - Accounet[ %s ], RoleName[ %s ]", pRole->TempData.Sys.DestroyAccount.Begin(), Global::GetRoleName_ASCII( pRole ).c_str() );
							}
							else
							{
								//NetSrv_CliConnect::SBL_RetNomorlWorld( pRole->PlayerTempData->ChangeWorld.WorldID , pRole->PlayerTempData->ChangeWorld.WorldNetID , pRole->DBID() , pRole );
								//Global::DelFromPartition( pObj->GUID() );
								//NetSrv_CliConnect::SC_WorldReturnNotify( pObj->GUID() );
								Print( LV2, "BattleGround", "OnRC_LeaveBattleGround() WorldReturnNotifyProc - Accounet[ %s ], RoleName[ %s ], DBID[ %d ]", pRole->TempData.Sys.DestroyAccount.Begin(), Global::GetRoleName_ASCII( pRole ).c_str(), iDBID );
								NetSrv_CliConnectChild::WorldReturnNotifyProc( pObj->GUID() );
								bLeaveZone = true;
							}


							if( m_BG_iDisableRoleReconnect == 1 )
							{
								pRoleInfo->emStatus = EM_BG_ROLEINFO_STATUS_DISCONNECT_INBG;
								break;
							}
							else
							{
								Print( LV2, "BattleGround", "OnRC_LeaveBattleGround() remove from team - Accounet[ %s ], RoleName[ %s ], DBID[ %d ]", pRole->TempData.Sys.DestroyAccount.Begin(), Global::GetRoleName_ASCII( pRole ).c_str(), iDBID );
								pTeam->erase( itRole );
								break;
							}

						}
					}
				}
			}
		}
	}

	// 檢查戰場是否所有人物均斷線或不存在
	DestoryEmptyBG();

}
//----------------------------------------------------------------
int CNetSrv_BattleGround_Child::GetPlayerFlagID ( RoleDataEx* pRole )
{
	if( pRole == NULL && pRole->IsPlayer() != false )
		return 0;

	int				iTeamID			= 0;
	int				iTeamCampID		= 0;
	bool			bFound			= false;
	int				iClientDBID		= pRole->DBID();

	// 檢查該 Client 是否在佇列中或等待進入戰場中, 若是, 則將其移除

	if( Ini()->IsGuildHouseWarZone != false )
	{
		return pRole->PlayerTempData->iForceFlagID;
	}

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			StructBattleGround*						pBG		= *it;

			vector<StructBattleGroundRoleInfo>*		pTeam	= NULL;

			for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
			{
				/*
				if( i == 1 )
				{
				iTeamID			= 1;
				iTeamCampID		= 8;
				pTeam			= &( pBG->vecTeam1 );
				}
				else
				{
				pTeam			= &( pBG->vecTeam2 );
				iTeamID			= 2;
				iTeamCampID		= 9;
				}
				*/

				pTeam = &( pBG->vecTeam[ iTeamID ] );

				for( vector<StructBattleGroundRoleInfo>::iterator it = pTeam->begin(); it != pTeam->end(); it++ )
				{
					StructBattleGroundRoleInfo *pBGRole = &(*it);

					if( pBGRole->iClienDBID == iClientDBID )
					{
						return pRole->PlayerTempData->iForceFlagID;
					}
				}
			}
		}
	}

	return 0;
}
//----------------------------------------------------------------
bool CNetSrv_BattleGround_Child::OnEvent_ClientConnect				( BaseItemObject* pObj )
{
	// 比對人物的 DBID 是否在現在戰場的隊伍中, 若不, 則將人物傳回出生點

	RoleDataEx*		pRole			= pObj->Role();
	if( pRole == NULL )
		return false;

	int							iTeamID			= 0;
	int							iTeamCampID		= 0;
	bool						bFound			= false;
	int							iClientDBID		= pObj->Role()->DBID();
	StructBattleGroundRoleInfo *pBGRole			= NULL;
	int							iRoleRoom		= 0;



	// 檢查該 Client 是否在佇列中或等待進入戰場中, 若是, 則將其移除

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			StructBattleGround*						pBG		= *it;

			if( pBG->emBattleGroundStatus != EM_BG_STATUS_RUNNING )
				continue;

			vector<StructBattleGroundRoleInfo>*		pTeam	= NULL;

			for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
			{
				pTeam = &( pBG->vecTeam[ iTeamID ] );

				for( vector<StructBattleGroundRoleInfo>::iterator it = pTeam->begin(); it != pTeam->end(); it++ )
				{
					pBGRole = &(*it);

					if( pBGRole->iClienDBID == iClientDBID )
					{
						// 執行 EnterBattleGround LuaScript, 使用外部 Script 設定細節設定
						if( pBGRole->emStatus != EM_BG_ROLEINFO_STATUS_DISCONNECT_INBG )
						{
							ZeroMemory( &(pRole->PlayerTempData->BGInfo), sizeof( pObj->Role()->PlayerTempData->BGInfo ) );
						}
						else
						{
							if( m_BG_iDisableRoleReconnect == 1 )
							{
								Print( LV1, "BG", "OnEvent_ClientConnect() DisableRoleReconnect == 1, DBID[ %d ]", iClientDBID );

								if( m_BG_iWorldBattleGround == 0 )
								{
									// Normal server
									if( LuaPlotClass::ChangeZone( pObj->GUID(), pBGRole->iRoleZoneID, pBGRole->iRoleRoomID, pBGRole->fRoleX, pBGRole->fRoleY, pBGRole->fRoleZ, pBGRole->fRoleDir ) == false )
									{
										// 無法傳送至原始地點, 傳送至指定地點
										if( LuaPlotClass::ChangeZone( pObj->GUID(), pRole->SelfData.ReturnZoneID, pRole->RoomID(), pRole->SelfData.ReturnPos.X, pRole->SelfData.ReturnPos.Y, pRole->SelfData.ReturnPos.Z, pRole->SelfData.ReturnPos.Dir ) == false )
										{
											Print( LV5, "BattleGround", "Cant teleport player to org zone" );
										}
									}

									Print( LV2, "BattleGround", "OnEvent_ClientConnect() Role enter but data exist, and m_BG_iDisableRoleReconnect == 1 - Accounet[ %s ], RoleName[ %s ]", pRole->TempData.Sys.DestroyAccount.Begin(), Global::GetRoleName_ASCII( pRole ).c_str() );
								}
								else
								{
									// Cross world server
									//NetSrv_CliConnect::SBL_RetNomorlWorld( pRole->PlayerTempData->ChangeWorld.WorldID , pRole->PlayerTempData->ChangeWorld.WorldNetID , pRole->DBID() , pRole );
									//Global::DelFromPartition( pObj->GUID() );
									//NetSrv_CliConnect::SC_WorldReturnNotify( pObj->GUID() );
									NetSrv_CliConnectChild::WorldReturnNotifyProc( pObj->GUID() );
									Print( LV2, "BattleGround", "OnEvent_ClientConnect() Role enter but data exist, and m_BG_iDisableRoleReconnect == 1 - Accounet[ %s ], RoleName[ %s ]", pRole->TempData.Sys.DestroyAccount.Begin(), Global::GetRoleName_ASCII( pRole ).c_str() );
								}

								return true;
							}
						}

						if( m_BG_Lua_RoleEnterBattleGround.size() != 0 )
						{
							int		iObjID	= pObj->Role()->DBID();
							char	szLuaBuffer[ 1024 ] ;						

							sprintf( szLuaBuffer, "%s( %d, %d )", m_BG_Lua_RoleEnterBattleGround.c_str(), iObjID, ( iTeamID + 1 ) );						
							LUA_VMClass::PCallByStrArg( szLuaBuffer, iObjID, iObjID, NULL );
						}

						// 在上面的 Lua 執行完, 應該把"不能直接設定" 人物的資料, 都進 Tempdata.BGInfo, 比方說背後的旗子, ( 需要在 AddToPartition 時處理 )
						// 但能夠直接處理的資料, 都應該在 Script 中處理


						//pBGRole->emStatus = EM_BG_ROLEINFO_STATUS_IN;

						// 設定陣營
						iTeamCampID		= pRole->PlayerTempData->BGInfo.iCampID;
						LuaPlotClass::SetRoleCampID( pObj->GUID(), iTeamCampID );

						pBGRole->iTeamID							=	iTeamID;
						pObj->Role()->TempData.BattleGroundTeamID	=	iTeamID;

						pBGRole->Voc								=	pObj->Role()->BaseData.Voc;
						pBGRole->Voc_Sub							=	pObj->Role()->BaseData.Voc_Sub;
						pBGRole->LV									=	pObj->Role()->TempData.Attr.Level;
						pBGRole->LV_Sub								=	pObj->Role()->TempData.Attr.Level_Sub;
						pBGRole->iBeginWaitTime						= GetTickCount();

						pBGRole->emStatus							= EM_BG_ROLEINFO_STATUS_NOTIFIED_WAIT_TO_ENTER_BG;	

						//SC_SetBattleGroundData( pObj->GUID(), GetFlagID( iTeamCampID ), iTeamCampID, pBG->iRoomID );

						//*it = BGRole;


						if( Global::Ini()->IsAllowHonorSystem == 1 )
						{
							int ResetHonorValue = Global::Ini()->ResetHonorValue;
							pRole->PlayerBaseData->HonorWorth = ResetHonorValue;
						}



						bFound = true;
						break;
					}
				}

				if( bFound != false )
					break;
			}
		}
	}

	if( bFound == false && m_BG_iBattleGroundType != 0 )
	{
		if( pRole->BaseData.Voc != EM_Vocation_GameMaster )
		{
			if( pRole->PlayerBaseData->ManageLV != 0 )
			{
				pRole->Msg( "Battleground doesnt allow any teleport command, excpte gm, please use class gm to edit obj" );
			}

			Print( LV1, "BG", "OnEvent_ClientConnect() cant find battleground obj, send role to sourcepoint, DBID[ %d ]", iClientDBID );

			// 該人物不在戰場隊伍中, 將之傳回人物原本的出生點

			if( m_BG_iWorldBattleGround == 0 )
			{
				LuaPlotClass::ChangeZone( pObj->GUID(), pRole->SelfData.ReturnZoneID, 0, pRole->SelfData.ReturnPos.X, pRole->SelfData.ReturnPos.Y, pRole->SelfData.ReturnPos.Z, pRole->SelfData.ReturnPos.Dir );
			}
			else
			{
				//NetSrv_CliConnect::SBL_RetNomorlWorld( pRole->PlayerTempData->ChangeWorld.WorldID , pRole->PlayerTempData->ChangeWorld.WorldNetID , pRole->DBID() , pRole );
				//Global::DelFromPartition( pObj->GUID() );
				//NetSrv_CliConnect::SC_WorldReturnNotify( pObj->GUID() );
				NetSrv_CliConnectChild::WorldReturnNotifyProc( pObj->GUID() );
			}

			return true;
		}
	}
	else
	{		
		Print( LV1, "BG", "OnEvent_ClientConnect() DBID[ %d ], ACCOUNT[ %s ], ROLE[ %s ]", iClientDBID, pObj->Role()->Account_ID(), Global::GetRoleName_ASCII( pObj->Role() ).c_str() );
	}

	return false;
}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnEvent_ClientDisconnect( BaseItemObject* pObj )
{
	RoleDataEx*		pRole			= pObj->Role();
	if( pRole == NULL )
		return;

	bool			bFound			= false;
	int				iClientDBID		= pObj->Role()->DBID();


	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		// 檢查該 Client 是否在戰場中, 若是, 則將標計斷線
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			StructBattleGround*						pBG		= *it;

			vector<StructBattleGroundRoleInfo>*		pTeam	= NULL;

			for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
			{
				pTeam = &( pBG->vecTeam[ iTeamID ] );

				for( vector<StructBattleGroundRoleInfo>::iterator it = pTeam->begin(); it != pTeam->end(); it++ )
				{
					StructBattleGroundRoleInfo BGRole = *it;

					if( BGRole.iClienDBID == iClientDBID )
					{
						if( BGRole.emStatus  == EM_BG_ROLEINFO_STATUS_IN )
						{
							Print( LV2, "BG", "OnEvent_ClientDisconnect() DBID[ %d ], ACCOUNT[ %s ], ROLE[ %s ]", iClientDBID, pObj->Role()->Account_ID(), Global::GetRoleName_ASCII( pObj->Role() ).c_str() );

							BGRole.emStatus					= EM_BG_ROLEINFO_STATUS_DISCONNECT_INBG;
							BGRole.iBeginDisconnectTime		= GetTickCount();
							*it	= BGRole;
						}

						bFound = true;
						break;
					}
				}

				if( bFound != false )
					break;
			}
		}

		// 若是該 Client 在排隊的佇列中, 則設置該 Client 的斷線時間, 超過時間就移除排隊佇列
		bFound			= false;
		//for( int i=1; i<=2; i++ )
		for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
		{
			vector< StructBattleGroundQueue >*	pWaitQueue		= NULL;

			pWaitQueue = &( m_vecWaitQueue[ iLVID ][ iTeamID ] );

			for( vector<StructBattleGroundQueue>::iterator it = pWaitQueue->begin(); it != pWaitQueue->end(); it++ )
			{
				StructBattleGroundQueue Queue = *it;

				if( Queue.iDBID == iClientDBID )
				{
					Queue.emStatus					= EM_BG_ROLEINFO_STATUS_DISCONNECT_QUEUE;
					Queue.iBeginDisconnectTime		= GetTickCount();

					bFound = true;
					break;
				}
			}

			if( bFound != false )
				break;
		}
	}
}
//---------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnEvent_CheckDisconnect()
{
	bool	bFound			= false;
	DWORD	dwTick			= GetTickCount();

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		// 檢查排隊佇列, 斷線過久的人就踢掉了

		//for( int i=1; i<=2; i++ )
		for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
		{
			vector< StructBattleGroundQueue >*	pWaitQueue		= NULL;

			pWaitQueue	= &( m_vecWaitQueue[ iLVID ][ iTeamID ] );

			for( vector<StructBattleGroundQueue>::iterator it = pWaitQueue->begin(); it != pWaitQueue->end(); it++ )
			{
				StructBattleGroundQueue Queue = *it;

				if( Queue.emStatus == EM_BG_ROLEINFO_STATUS_DISCONNECT_QUEUE  )
				{
					int offsetTick				= abs( (int)dwTick - Queue.iBeginDisconnectTime );

					if( offsetTick > 120000 )
					{
						pWaitQueue->erase( it );
					}

					bFound = true;
					break;
				}
			}

			if( bFound != false )
				break;
		}

		// 檢查目前戰場隊伍
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			StructBattleGround*						pBG		= *it;

			vector<StructBattleGroundRoleInfo>*		pTeam	= NULL;

			//for( int i=1; i<=2; i++ )
			for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
			{
				pTeam = &( pBG->vecTeam[ iTeamID ] );

				vector<StructBattleGroundRoleInfo>::iterator it = pTeam->begin();

				while( it != pTeam->end() )
				{
					StructBattleGroundRoleInfo BGRole = *it;
					if( BGRole.emStatus == EM_BG_ROLEINFO_STATUS_DISCONNECT_INBG && m_BG_iDisableRoleReconnect == 0 )
					{
						int offsetTick				= abs( (int)dwTick - BGRole.iBeginDisconnectTime );

						if( offsetTick > 120000 )
						{
							//if( m_BG_iKickDisconnect == 1 )
							{
								it = pTeam->erase( it );
								continue;
							}														
						}
					}

					it++;
				}

				/*
				for( vector<StructBattleGroundRoleInfo>::iterator it = pTeam->begin(); it != pTeam->end(); it++ )
				{
				StructBattleGroundRoleInfo BGRole = *it;

				if( BGRole.emStatus == EM_BG_ROLEINFO_STATUS_DISCONNECT_INBG && m_BG_iDisableRoleReconnect == 0 )
				{
				int offsetTick				= abs( (int)dwTick - BGRole.iBeginDisconnectTime );

				if( offsetTick > 120000 )
				{
				pTeam->erase( it );
				}

				bFound = true;
				break;
				}
				}
				*/

			}
		}
	}

}
//------------------------------------------------------------------------
int	CNetSrv_BattleGround_Child::GetFlagID( int iRoleCampID )
{
	map< int, int >::iterator it = ((CNetSrv_BattleGround_Child*)m_pThis)->m_mapTeamFlag.find( iRoleCampID );

	if( it != ((CNetSrv_BattleGround_Child*)m_pThis)->m_mapTeamFlag.end() )
	{
		int iFlagID = it->second;
		return iFlagID;
	}

	return 0;
}
//------------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRC_GetBattleGroundScoreInfo( int iClientDBID )
{
	// 收集戰場分數資訊, 發送給要求的 Client, or 定時發
	//int iClientDBID = iDBID + ( iWorldID * _DEF_MAX_DBID_COUNT_ );
	RoleDataEx*	pRole		= Global::GetRoleDataByDBID( iClientDBID );

	if( pRole == NULL )
		return;

	int iRoomID = pRole->BaseData.RoomID;

	//SC_BattleGroundScore( 0, 0, iClientDBID, iRoomID );
}
//------------------------------------------------------------------------
void CNetSrv_BattleGround_Child::SC_BattleGroundScore ( int iWorldID, int iWorldZoneID, int iClientDBID, int iRoomID )
{
	StructBattleGroundScore				Score;
	vector< StructBattleGroundScore >	vecScore;

	bool								bFound = false;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			StructBattleGround* pBG = *it;

			if( pBG->iRoomID == iRoomID )
			{			
				for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
				{
					vector<StructBattleGroundRoleInfo>* pTeam = &( pBG->vecTeam[ iTeamID ] );

					for( vector<StructBattleGroundRoleInfo>::iterator itRole = pTeam->begin(); itRole != pTeam->end(); itRole++ )
					{

						StructBattleGroundRoleInfo*	pRoleInfo	= &( *itRole );			
						RoleDataEx*					pRole		= Global::GetRoleDataByDBID( pRoleInfo->iClienDBID );

						if( pRole != NULL )
						{
							pRoleInfo->RoleName = pRole->BaseData.RoleName;
							pRoleInfo->Info		= pRole->PlayerTempData->BGInfo;
						}


						//if( pRole != NULL )
						{

							Score.iTeamID	= pRoleInfo->iTeamID;
							Score.iDBID		= pRoleInfo->iClienDBID;
							Score.iRoomID	= pRoleInfo->iRoleRoomID;
							//Score.Name		= pRole->BaseData.RoleName;
							//Score.Score		= pRole->TempData.BGInfo;
							Score.Name		= pRoleInfo->RoleName;
							Score.Score		= pRoleInfo->Info;


							Score.iVocID	= (int)pRoleInfo->Voc;
							Score.iVocSubID	= (int)pRoleInfo->Voc_Sub;
							Score.iLV		= pRoleInfo->LV;
							Score.iLVSub	= pRoleInfo->LV_Sub;
							Score.iStatus	= (int)pRoleInfo->emStatus;

							memcpy( Score.iRoleVar, pRoleInfo->iRoleVar, sizeof( pRoleInfo->iRoleVar ) );

							vecScore.push_back( Score );
						}
					}
				}

				bFound = true; 
				break;
			}
		}

		if( bFound != false )
			break;
	}

	int		iTotalRole	= (int)vecScore.size();
	int		iPacketSize	= sizeof( int ) + sizeof( int ) + ( sizeof( StructBattleGroundScore ) * iTotalRole );

	char*	pPacketData	= NEW char[ iPacketSize ];
	char*	ptr			= pPacketData;

	*((GamePGCommandEnum*)ptr)	= EM_PG_BattleGround_LtoC_BattleGroundScoreInfo;
	ptr += sizeof( GamePGCommandEnum );

	*((int*)ptr) = iTotalRole;
	ptr += sizeof( int );


	for( vector< StructBattleGroundScore >::iterator itScore = vecScore.begin(); itScore != vecScore.end(); itScore++ )
	{
		StructBattleGroundScore* pScore = &(*itScore);
		memcpy( ptr, (char*)pScore, sizeof( StructBattleGroundScore ) );
		ptr += sizeof( StructBattleGroundScore );
	}


	if( iWorldID == 0 )
	{
		Global::SendToCli_ByDBID( iClientDBID, iPacketSize, pPacketData );
	}
	else
	{
		Global::SendToOtherWorldZoneClient_ByDBID( iWorldID, iWorldZoneID, iClientDBID, iPacketSize, pPacketData );
	}

	delete [] pPacketData;
}

void CNetSrv_BattleGround_Child::SC_AllPlayerPos( )
{
	StructBattleGroundPlayerPos PlayerPos;
	vector< StructBattleGroundPlayerPos > vecPlayerPos;	

	//vector< int > vecAllPlayerDBID;
	vector< StructBattleGroundRoleInfo > vecAllPlayerDBID;

	//for each room
	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			StructBattleGround* pBG = *it;

			if( pBG != NULL )
			{
				vecPlayerPos.clear();
				vecAllPlayerDBID.clear();

				StructBattleGroundRoleInfo*	pRoleInfo	= NULL;			
				RoleDataEx*					pRole		= NULL;


				//collect position information for all team players
				for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
				{
					vector<StructBattleGroundRoleInfo>* pTeam = &( pBG->vecTeam[ iTeamID ] );
					for( vector<StructBattleGroundRoleInfo>::iterator itRole = pTeam->begin(); itRole != pTeam->end(); itRole++ )
					{
						pRoleInfo	= &( *itRole );			
						pRole		= Global::GetRoleDataByDBID( pRoleInfo->iClienDBID );
						//The Player is really in battle ground
						if( pRole != NULL && pRoleInfo->emStatus == EM_BG_ROLEINFO_STATUS_IN)
						{
							//the player is not in cover state and not in sneak state
							if( (pRole->TempData.Attr.Effect.Cover == false) && (pRole->TempData.Attr.Effect.Sneak == false))
							{
								PlayerPos.RoleName = pRole->BaseData.RoleName;
								PlayerPos.ZoneID   = GetBattleGroundType();
								PlayerPos.Pos      = pRole->BaseData.Pos;

								vecPlayerPos.push_back( PlayerPos );							
							}
							//vecAllPlayerDBID.push_back(pRole->DBID());
							vecAllPlayerDBID.push_back( *pRoleInfo );
						}
					}
				}

				//--------------------------------------------------------------------------
				//build the packet
				int		iTotalRole	= (int)vecPlayerPos.size();
				int		iPacketSize	= sizeof( int ) + sizeof( int ) + ( sizeof( StructBattleGroundPlayerPos ) * iTotalRole );

				char*	pPacketData	= NEW char[ iPacketSize ];
				char*	ptr			= pPacketData;

				*((GamePGCommandEnum*)ptr)	= EM_PG_BattleGround_LtoC_AllBattleGroundPlayerPos;
				ptr += sizeof( GamePGCommandEnum );

				*((int*)ptr) = iTotalRole;
				ptr += sizeof( int );

				for( vector< StructBattleGroundPlayerPos >::iterator itPos = vecPlayerPos.begin(); itPos != vecPlayerPos.end(); itPos++ )
				{
					StructBattleGroundPlayerPos* pPos = &(*itPos);
					memcpy( ptr, (char*)pPos, sizeof( StructBattleGroundPlayerPos ) );
					ptr += sizeof( StructBattleGroundPlayerPos );
				}
				//--------------------------------------------------------------------------

				//send packet to all players in the same room
				//for (vector< int >::iterator itDBID = vecAllPlayerDBID.begin(); itDBID != vecAllPlayerDBID.end(); itDBID++)
				//{
				//	Global::SendToCli_ByDBID( (*itDBID), iPacketSize, pPacketData );
				//}

				for (vector< StructBattleGroundRoleInfo >::iterator itDBID = vecAllPlayerDBID.begin(); itDBID != vecAllPlayerDBID.end(); itDBID++)
				{
					StructBattleGroundRoleInfo RoleInfo = *itDBID;
					//Global::SendToCli_ByDBID( (*itDBID), iPacketSize, pPacketData );
					if( RoleInfo.iRoleWorldID == 0 )
						Global::SendToCli_ByDBID( RoleInfo.iClienDBID, iPacketSize, pPacketData );
					else
						Global::SendToOtherWorldZoneClient_ByDBID( RoleInfo.iRoleWorldID, RoleInfo.iRoleZoneID, RoleInfo.iClienDBID, iPacketSize, pPacketData );

				}

				delete [] pPacketData;
			}
		}
	}
}

void CNetSrv_BattleGround_Child::CheckWaitQueue()
{
	// 通知所有在佇列的人, 他們目前等待的情況
	// 檢查是否已在佇列中了

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( int iQueueID = 0; iQueueID < m_BG_iNumTeam; iQueueID++ )
		{
			bool bFound = false;

			vector< StructBattleGroundQueue >* pQueue = NULL;


			pQueue = &( m_vecWaitQueue[ iLVID ][ iQueueID ] );

			int iCount = 0;


			vector< StructBattleGroundQueue >::iterator it = pQueue->begin();

			vector< StructBattleGroundQueue > vecNoRespondQueue;

			while( it != pQueue->end() )
			{
				StructBattleGroundQueue Obj = *it;

				int iOffset = abs( (int)(RoleDataEx::G_SysTime_Base - Obj.iLastCheckQueueTime ));

				// 超過 DF_BG_TIME_SENT_QUEUE_STATUS 的人, 發送通知
				if( iOffset > DF_BG_TIME_SENT_QUEUE_STATUS )
				{
					// 送出通知封包, 告訴 Client 等待的情況
					CNetSrv_BattleGround_Child::SC_BattleGroundWaitQueueStatus( Obj.iSourceWorldID, Obj.iSourceZoneID, Obj.iDBID, m_BG_iBattleGroundType, 0, iCount, RoleDataEx::G_SysTime_Base );
					iCount++;
				}

				// 超過 DF_BG_TIME_DELETE_NO_RESPOND_QUEUEOBJ, 則刪除
				if( iOffset > DF_BG_TIME_DELETE_NO_RESPOND_QUEUEOBJ )
				{
					it = pQueue->erase( it );
					continue;
				}
				else		
					if( iOffset > DF_BG_TIME_CHECK_NO_RESPOND_QUEUEOBJ )
					{
						// 超過 DF_BG_TIME_CHECK_NO_RESPOND_QUEUEOBJ 的人, 將佇列往後移
						vecNoRespondQueue.push_back( Obj );
						it = pQueue->erase( it );
						continue;
					}

					it++;
			}

			// 將剛剛處理完沒反應的物件, 推進佇列後面
			for( vector< StructBattleGroundQueue >::iterator it = vecNoRespondQueue.begin(); it != vecNoRespondQueue.end(); it++ )
			{
				pQueue->push_back( *it );
			}
		}
	}
}

void CNetSrv_BattleGround_Child::OnRZ_LeaveBattleGroundWaitQueue ( int iWorldID, int iWorldSrvID, int iClientDBID, int iBattleGroundType )
{
	//int iDBID = iClientDBID % _DEF_MAX_DBID_COUNT_;

	bool bFound = false;
	vector< StructBattleGroundQueue >* pQueue = NULL;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
		{
			pQueue = &( m_vecWaitQueue[ iLVID ][ iTeamID ] );

			for( vector< StructBattleGroundQueue >::iterator it = pQueue->begin(); it != pQueue->end(); it++ )
			{
				StructBattleGroundQueue Obj = *it;

				if( ( Obj.iDBID % _DEF_MAX_DBID_COUNT_ ) == ( iClientDBID % _DEF_MAX_DBID_COUNT_ ) && Obj.emStatus == EM_BG_ROLEINFO_STATUS_QUEUE )
				{
					pQueue->erase( it );	// 因為是單人, 刪除掉整個 QueueObj
					return;
				}
			}
		}
	}

/*
	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			StructBattleGround* pBG = *it;

			for( int iTeamID = 0; iTeamID < DF_BG_MAXTEAM; iTeamID++ )
			{
				if( pBG->vecTeam[ iTeamID ].size() == 0 )
					continue;

				//vector<StructBattleGroundRoleInfo>* pTeam = &( pBG->vecTeam[ iTeamID ] );

				//pTeam = &( pBG->vecTeam[ iTeamID ] );

					for( vector<StructBattleGroundRoleInfo>::iterator itRole = pBG->vecTeam[ iTeamID ].begin(); itRole != pBG->vecTeam[ iTeamID ].end(); itRole++ )
					{
						StructBattleGroundRoleInfo* pRoleInfo = &(*itRole);

						if( pRoleInfo->iClienDBID % _DEF_MAX_DBID_COUNT_ == iClientDBID % _DEF_MAX_DBID_COUNT_ )
						{				
							RoleDataEx* pRole = Global::GetRoleDataByDBID( pRoleInfo->iClienDBID );				

							if( pRole != NULL )
							{

								if( m_BG_iWorldBattleGround == 0 )
								{
									bool bLeaveZone = LuaPlotClass::ChangeZone( pRole->GUID(), pRoleInfo->iRoleZoneID, pRoleInfo->iRoleRoomID, pRoleInfo->fRoleX, pRoleInfo->fRoleY, pRoleInfo->fRoleZ, pRoleInfo->fRoleDir );

									if( bLeaveZone == false )
									{
										bLeaveZone = LuaPlotClass::ChangeZone( pRole->GUID(), pRole->SelfData.ReturnZoneID, pRole->RoomID(), pRole->SelfData.ReturnPos.X, pRole->SelfData.ReturnPos.Y, pRole->SelfData.ReturnPos.Z, pRole->SelfData.ReturnPos.Dir );
									}
								}
								else
								{
									NetSrv_CliConnectChild::WorldReturnNotifyProc( pRole->GUID() );							
								}
							}

							pBG->vecTeam[ iTeamID ].erase( itRole );
						}
					}
				
			}
		}
	}
	*/
}
//-------------------------------------------------------
void CNetSrv_BattleGround_Child::CheckBattleGround()
{

	int iTimeNow = GetTickCount();

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			StructBattleGround* pBG = *it;

			vector<StructBattleGroundRoleInfo>::iterator	itRole;
			vector<StructBattleGroundRoleInfo>*				pTeam	= NULL;

			for( int iTeamID = 0; iTeamID < DF_BG_MAXTEAM; iTeamID++ )
			{
				pTeam = &( pBG->vecTeam[ iTeamID ] );

				// 檢查目前戰場中斷線的人, 是否超過時間仍然沒有近入戰場
				itRole	= pTeam->begin();
				while( itRole != pTeam->end() )
				{
					StructBattleGroundRoleInfo*	pRoleInfo	= &( *itRole );			

					if( pRoleInfo->emStatus == EM_BG_ROLEINFO_STATUS_DISCONNECT_INBG && m_BG_iDisableRoleReconnect == 0 )
					{
						int iDisconnectTime = abs( iTimeNow - pRoleInfo->iBeginDisconnectTime );

						if( iDisconnectTime > DF_BG_DICONNECT_TO_KICK )
						{
							itRole = pTeam->erase( itRole );
							continue;
						}
					}
					itRole++;
				}

				// 檢查目歉戰場中已經通知的人, 超過時間仍沒進入戰場,  取消該資格
				// 這邊可能有問題, 若是競技場模式, 沒進來的人被移除, 將發生扣分找不到人的情況, 所以只能利用 INI 設置, 斷線不重登.
				itRole	= pTeam->begin();
				while( itRole != pTeam->end() )
				{
					StructBattleGroundRoleInfo*	pRoleInfo	= &( *itRole );			

					if( pRoleInfo->emStatus == EM_BG_ROLEINFO_STATUS_NOTIFIED_WAIT_TO_ENTER_BG )
					{
						int iNotifiedTime = abs( iTimeNow - pRoleInfo->iBeginWaitTime );

						if( m_BG_iDisableDelNoRespondQueueObj == 0 && iNotifiedTime > DF_BG_WAIT_ENTER_TIME )
						{
							SC_LostEnterBattleGroundRight( pRoleInfo->iRoleWorldID, pRoleInfo->iRoleZoneID, pRoleInfo->iClienDBID, m_BG_iBattleGroundType, pBG->iRoomID, pBG->szName.Begin() );
							itRole = pTeam->erase( itRole );
						
							continue;
						}
						else
						{
							SC_NotifyEnterBattleGroundRight( pRoleInfo->iRoleWorldID, pRoleInfo->iRoleZoneID, pRoleInfo->iClienDBID, m_BG_iBattleGroundType, pBG->iRoomID, pBG->szName.Begin(), ( DF_BG_WAIT_ENTER_TIME - iNotifiedTime ) );
						}
					}
					itRole++;
				}


				// 檢查目前戰場中的人, 人物是否真的仍在戰場, 此機制為雙重檢查, 避免有機制錯誤, 導至玩家不在區域中, 但戰場紀錄中

				static int iNextCheckObjExistTime	= GetTickCount() + DF_BG_CHECK_OBJ_EXIST_TIME;

				if( iTimeNow > iNextCheckObjExistTime )
				{
					iNextCheckObjExistTime	= GetTickCount() + DF_BG_CHECK_OBJ_EXIST_TIME;
					itRole					= pTeam->begin();

					while( itRole != pTeam->end() )
					{
						StructBattleGroundRoleInfo*	pRoleInfo	= &( *itRole );			

						if( pRoleInfo->emStatus == EM_BG_ROLEINFO_STATUS_IN )
						{
							BaseItemObject* pObj = BaseItemObject::GetObjByDBID( pRoleInfo->iClienDBID );
							if( pObj == NULL )
							{
								pRoleInfo->emStatus					= EM_BG_ROLEINFO_STATUS_DISCONNECT_INBG;
								pRoleInfo->iBeginDisconnectTime		= GetTickCount();
							}
						}
						itRole++;
					}
				}
			}
		}
	}


	// 處理發出通知但太久沒進


}
//-------------------------------------------------------
int CNetSrv_BattleGround_Child::OnTime_CheckBattleGroundObjExist	( SchedularInfo* Obj, void* InputClass )
{
	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = ((CNetSrv_BattleGround_Child*)m_pThis)->m_vecBattleGround[ iLVID ].begin(); it != ((CNetSrv_BattleGround_Child*)m_pThis)->m_vecBattleGround[ iLVID ].end(); it++ )
		{
			StructBattleGround* pBG = *it;

			vector<StructBattleGroundRoleInfo>::iterator	itRole;
			vector<StructBattleGroundRoleInfo>*				pTeam	= NULL;

			for( int iTeamID = 0; iTeamID < ((CNetSrv_BattleGround_Child*)m_pThis)->m_BG_iNumTeam; iTeamID++ )
			{
				pTeam = &( pBG->vecTeam[ iTeamID ] );

				// 檢查目前戰場中的人, 人物是否真的仍在戰場, 此機制為雙重檢查, 避免有機制錯誤, 導至玩家不在區域中, 但戰場紀錄中
				for( vector<StructBattleGroundRoleInfo>::iterator itRole = pTeam->begin(); itRole != pTeam->end(); itRole++ )
				{
					StructBattleGroundRoleInfo*	pRoleInfo	= &( *itRole );			

					if( pRoleInfo->emStatus == EM_BG_ROLEINFO_STATUS_IN )
					{
						BaseItemObject* pObj = BaseItemObject::GetObjByDBID( pRoleInfo->iClienDBID );
						if( pObj == NULL )
						{
							pRoleInfo->emStatus					= EM_BG_ROLEINFO_STATUS_DISCONNECT_INBG;
							pRoleInfo->iBeginDisconnectTime		= GetTickCount();
						}
					}
				}
			}
		}
	}

	Global::Schedular()->Push( OnTime_CheckBattleGroundObjExist , DF_BG_CHECK_OBJ_EXIST_TIME, NULL, NULL );
	return 0;
}

//-------------------------------------------------------
int CNetSrv_BattleGround_Child::OnTime_UpdateAllBattleGroundPlayerPos(SchedularInfo* Obj, void* InputClass )
{
	m_pThis->SC_AllPlayerPos();
	Global::Schedular()->Push( OnTime_UpdateAllBattleGroundPlayerPos , DF_BG_UPDATE_PLAYER_POS_TIME, NULL, NULL );
	return 0;
}
//-------------------------------------------------------
void CNetSrv_BattleGround_Child::SetArenaScore ( int iRoomID, int iTeamIndex, int iArenaType, int iIndex, float fValue )
{
	return;

	int iTeamID		= iTeamIndex - 1;
	int iAreanID	= iArenaType -1;
	StructBattleGround* pBG = NULL;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;
			if( pBG->iRoomID == iRoomID )
			{
				vector<StructBattleGroundRoleInfo>* pTeam = NULL;
				if( iTeamID >= 0 && iTeamID < m_BG_iNumTeam )
				{		
					pTeam = &( pBG->vecTeam[ iTeamID ] );
				}


				for( vector<StructBattleGroundRoleInfo>::iterator it = pTeam->begin(); it != pTeam->end(); it++ )
				{
					StructBattleGroundRoleInfo* pbgObj = &(*it);

					if( pbgObj == NULL )
						return;

					RoleDataEx* pRole = GetRoleDataByDBID( pbgObj->iClienDBID );

					if( pRole == NULL )
						return;

					if( ( iAreanID >= 0 && iAreanID < DF_ARENA_TYPE ) == false )
						return;					

					//ArenaInfo* pArena = &( pRole->TempData.Arena[ iAreanID ] );
					StructArenaInfo* pArena = NULL;

					switch( iIndex )
					{
					case 1:	{	pArena->iWeekJoinCount	= fValue;	} break;
					case 2:	{	pArena->fArenaPoint		= fValue;	} break;
					case 3:	{	pArena->iWeekWinCount	= fValue;	} break;
					case 4:	{	pArena->iWeekLoseCount	= fValue;	} break;
					case 5:	{	pArena->iMonthWinCount	= fValue;	} break;
					case 6:	{	pArena->iMonthLoseCount	= fValue;	} break;
					case 7:	{	pArena->iLifeWinCount	= fValue;	} break;
					case 8:	{	pArena->iLifeLoseCount	= fValue;	} break;
					}

					return;
				}
			}	
		}
	}
}
//-------------------------------------------------------
float	CNetSrv_BattleGround_Child::GetArenaScore ( int iRoomID, int iTeamIndex, int iArenaType, int iIndex )
{

	return 0;

	int iTeamID		= iTeamIndex - 1;
	int iAreanID	= iArenaType -1;
	StructBattleGround* pBG = NULL;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;
			if( pBG->iRoomID == iRoomID )
			{
				vector<StructBattleGroundRoleInfo>* pTeam = NULL;
				if( iTeamID >= 0 && iTeamID < m_BG_iNumTeam )
				{		
					pTeam = &( pBG->vecTeam[ iTeamID ] );
				}


				for( vector<StructBattleGroundRoleInfo>::iterator it = pTeam->begin(); it != pTeam->end(); it++ )
				{
					StructBattleGroundRoleInfo* pbgObj = &(*it);

					if( pbgObj == NULL )
						return 0 ;

					RoleDataEx* pRole = GetRoleDataByDBID( pbgObj->iClienDBID );

					if( pRole == NULL )
						return 0;

					if( ( iAreanID >= 0 && iAreanID < DF_ARENA_TYPE ) == false )
						return 0;					

					StructArenaInfo* pArena = &( pRole->PlayerTempData->Arena[ iAreanID ] );

					switch( iIndex )
					{
					case 1:	{	return pArena->iWeekJoinCount;	} break;
					case 2:	{	return pArena->fArenaPoint	;	} break;
					case 3:	{	return pArena->iWeekWinCount;	} break;
					case 4:	{	return pArena->iWeekLoseCount;	} break;
					case 5:	{	return pArena->iMonthWinCount;	} break;
					case 6:	{	return pArena->iMonthLoseCount;	} break;
					case 7:	{	return pArena->iLifeWinCount;	} break;
					case 8:	{	return pArena->iLifeLoseCount;	} break;
					}
				}
			}	
		}
	}
}
//-------------------------------------------------------
int CNetSrv_BattleGround_Child::GetNumEnterTeamMember( int iRoomID, int iTeamIndex )
{
	int iTeamID		= iTeamIndex - 1;
	int iCount		= 0;

	StructBattleGround* pBG = NULL;
	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;
			if( pBG->iRoomID == iRoomID )
			{
				vector<StructBattleGroundRoleInfo>* pTeam = NULL;
				if( iTeamID >= 0 && iTeamID < m_BG_iNumTeam )
				{		
					pTeam = &( pBG->vecTeam[ iTeamID ] );			
					for( vector<StructBattleGroundRoleInfo>::iterator itObj = pTeam->begin(); itObj != pTeam->end(); itObj++ )
					{
						StructBattleGroundRoleInfo* pObj = &(*itObj);

						if( pObj->emStatus == EM_BG_ROLEINFO_STATUS_IN )
						{
							iCount++;
						}
					}
					return iCount;
				}
			}
		}
	}

	return 0;
}
//-------------------------------------------------------
int CNetSrv_BattleGround_Child::GetBattleGroundRoomID( int iID )
{
	if( iID == -1 )
		return (int)((CNetSrv_BattleGround_Child*)m_pThis)->m_vecBattleGround->size();

	else
	{
		vector< StructBattleGround* >::iterator it = ((CNetSrv_BattleGround_Child*)m_pThis)->m_vecBattleGround->begin();

		vector< int > vecRoomID;

		for( vector< StructBattleGround* >::iterator it = ((CNetSrv_BattleGround_Child*)m_pThis)->m_vecBattleGround->begin(); it != ((CNetSrv_BattleGround_Child*)m_pThis)->m_vecBattleGround->end(); it++ )
		{
			vecRoomID.push_back( (*it)->iRoomID );
		}

		if( iID > 0 && iID <= vecRoomID.size() )
		{
			int iRoomID = vecRoomID[ ( iID - 1 ) ];
			return iRoomID;
		}		
	}


	return 0;
}
//-------------------------------------------------------
int CNetSrv_BattleGround_Child::GetNumTeamMember( int iRoomID, int iTeamIndex )
{
	int iTeamID		= iTeamIndex - 1;
	int iCount		= 0;

	StructBattleGround* pBG = NULL;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;
			if( pBG->iRoomID == iRoomID )
			{
				vector<StructBattleGroundRoleInfo>* pTeam = NULL;
				if( iTeamID >= 0 && iTeamID < m_BG_iNumTeam )
				{		
					pTeam = &( pBG->vecTeam[ iTeamID ] );
					return (int)pTeam->size();
				}
			}
		}
	}
}
//-------------------------------------------------------
int	CNetSrv_BattleGround_Child::GetDisconnectTeamMember				( int iRoomID, int iTeamIndex )
{
	int		iDisconnectCount	= 0;
	int		iTeamID				= iTeamIndex - 1;

	StructBattleGround* pBG = NULL;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;
			if( pBG->iRoomID == iRoomID )
			{
				vector<StructBattleGroundRoleInfo>* pTeam = NULL;
				if( iTeamID >= 0 && iTeamID < m_BG_iNumTeam )
				{		
					pTeam = &( pBG->vecTeam[ iTeamID ] );

					if( pTeam != NULL )
					{
						for( vector<StructBattleGroundRoleInfo>::iterator itObj = pTeam->begin(); itObj != pTeam->end(); itObj++ )
						{
							StructBattleGroundRoleInfo* pObj = &( *itObj );

							if( pObj->emStatus == EM_BG_ROLEINFO_STATUS_DISCONNECT_INBG )
							{
								iDisconnectCount++;
							}
						}

						return iDisconnectCount;
					}
				}
			}
		}
	}
}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRD_UpdateArenaData ( int iArenaType, int iTeamGUID, StructArenaInfo* pArenaInfo )
{
	if( ( iArenaType >= 0 && iArenaType < DF_ARENA_TYPE ) == false )
		return;

	if( pArenaInfo == NULL )
		return;

	m_ArenaInfo[ iArenaType ][ iTeamGUID ] = *pArenaInfo;

	// 發送更新給該隊伍的所有隊友
	for( int i = 0; i < pArenaInfo->iNumTeamMember; i++ )
	{



	}










}
//----------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRC_BattleGroundWaitQueueStatusRespond( int iClientDBID )
{
	// 發送佇列狀況給 Client 後, Client 的回應封包, 若 Client 沒有回應超過 20sec, 就將該玩家移到佇列後面, 若超過 5min 則刪除佇列

	// 通知所有在佇列的人, 他們目前等待的情況
	// 檢查是否已在佇列中了

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( int iQueueID = 0; iQueueID < m_BG_iNumTeam; iQueueID++ )
		{
			bool bFound = false;

			vector< StructBattleGroundQueue >* pQueue = NULL;

			pQueue = &( m_vecWaitQueue[ iLVID ][ iQueueID ] );

			int iCount = 0;

			for( vector< StructBattleGroundQueue >::iterator it = pQueue->begin(); it != pQueue->end(); it++ )
			{
				StructBattleGroundQueue* pObj = &( *it );			

				if( pObj->iDBID == iClientDBID )
				{
					pObj->iLastCheckQueueTime = RoleDataEx::G_SysTime_Base;
					return;
				}		
			}
		}
	}
}
//-----------------------------------------------------------------------
void CNetSrv_BattleGround_Child::AddTeamHonor( int iRoomID, int iTeamIndex, int iHonor )
{
	int iTeamID		= iTeamIndex - 1;

	StructBattleGround* pBG = NULL;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;
			if( pBG->iRoomID == iRoomID )
			{
				vector<StructBattleGroundRoleInfo>* pTeam = NULL;
				if( iTeamID >= 0 && iTeamID < m_BG_iNumTeam )
				{		
					pTeam = &( pBG->vecTeam[ iTeamID ] );			
					for( vector<StructBattleGroundRoleInfo>::iterator itObj = pTeam->begin(); itObj != pTeam->end(); itObj++ )
					{
						StructBattleGroundRoleInfo* pObj = &(*itObj);

						if( pObj->emStatus == EM_BG_ROLEINFO_STATUS_IN )
						{
							RoleDataEx* pRole = Global::GetRoleDataByDBID( pObj->iClienDBID );

							if( pRole != NULL )
							{
								pRole->AddValue( EM_RoleValue_Honor , iHonor );

								if( iHonor > 1 )
									pRole->Net_DeltaRoleValue( EM_RoleValue_Honor , iHonor );
							}					
						}
					}
				}
			}
		}
	}
}

void CNetSrv_BattleGround_Child::AddTeamItem( int iRoomID, int iTeamIndex, int iItemID, int iItemCount )
{

	int iTeamID		= iTeamIndex - 1;

	StructBattleGround* pBG = NULL;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;
			if( pBG->iRoomID == iRoomID )
			{
				vector<StructBattleGroundRoleInfo>* pTeam = NULL;
				if( iTeamID >= 0 && iTeamID < m_BG_iNumTeam )
				{		
					pTeam = &( pBG->vecTeam[ iTeamID ] );

					for( vector<StructBattleGroundRoleInfo>::iterator itObj = pTeam->begin(); itObj != pTeam->end(); itObj++ )
					{
						StructBattleGroundRoleInfo* pObj	= &(*itObj);
						GameObjDbStructEx*			OrgDB	= Global::GetObjDB( iItemID );

						if( pObj->emStatus == EM_BG_ROLEINFO_STATUS_IN && OrgDB != NULL )
						{
							RoleDataEx* pRole = Global::GetRoleDataByDBID( pObj->iClienDBID );

							if( pRole != NULL )
							{
								pRole->Sc_GiveItemToBuf(  iItemID,  iItemCount , EM_ActionType_PlotGive , NULL, "" );

							}					
						}
					}
				}
			}
		}
	}
}
//-----------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRC_GetBattleGroundList ( int iClientDBID )
{
	for( vector< StructBattleGroundListInfo >::iterator it = m_vecBattleGroundList.begin(); it != m_vecBattleGroundList.end(); it++ )
	{
		StructBattleGroundListInfo Info = *it;

		if( Info.iWorldID == 0 )
		{
			PG_BattleGround_LtoL_GetBattleGroundList	Packet;

			Packet.iDBID			= iClientDBID;

			Global::SendToLocal( Info.iZoneID, sizeof(Packet), &Packet );
		}
		else
		{
			PG_BattleGround_WLtoWL_GetBattleGroundList	Packet;

			Packet.iDBID			= iClientDBID;

			Global::SendToOtherWorld_GSrvID( Info.iWorldID, Info.iZoneID, sizeof( PG_BattleGround_WLtoWL_GetBattleGroundList ), &Packet );
		}
	}
}
//-----------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRZ_GetBattleGroundList ( int iWorldID, int iWorldSrvID, int iClientDBID )
{
	OnRZ_GetBattleGround( iWorldID, iWorldSrvID, iClientDBID );
}
//-----------------------------------------------------------------------
int	CNetSrv_BattleGround_Child::GetEnterTeamMemberInfo( int iRoomID, int iTeamIndex, int iID )
{
	int iTeamID		= iTeamIndex - 1;
	int iIndex		= iID - 1;

	if( iID == -1 )
	{
		m_vectempTeamInfo.clear();

		StructBattleGround* pBG = NULL;

		for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
		{
			for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
			{
				pBG = *it;
				if( pBG->iRoomID == iRoomID )
				{
					vector<StructBattleGroundRoleInfo>* pTeam = NULL;
					if( iTeamID >= 0 && iTeamID < m_BG_iNumTeam )
					{		
						pTeam = &( pBG->vecTeam[ iTeamID ] );

						for( vector<StructBattleGroundRoleInfo>::iterator itObj = pTeam->begin(); itObj != pTeam->end(); itObj++ )
						{
							StructBattleGroundRoleInfo* pObj	= &(*itObj);

							if( pObj->emStatus == EM_BG_ROLEINFO_STATUS_IN )
							{
								RoleDataEx* pRole = Global::GetRoleDataByDBID( pObj->iClienDBID );

								if( pRole != NULL )
								{
									m_vectempTeamInfo.push_back( pRole->GUID() );
									//m_vectempTeamInfo.push_back( pObj->iClienDBID  );
								}					
							}
						}

						return (int)m_vectempTeamInfo.size();
					}
				}
			}
		}
	}
	else
	{
		if( iIndex >= 0 && iIndex < m_vectempTeamInfo.size() )
		{
			int iGUID = m_vectempTeamInfo[ iIndex ];
			return iGUID;
		}
	}
}
//-----------------------------------------------------------------------
void CNetSrv_BattleGround_Child::SetMemberVar( int iRoomID, int iTeamIndex, int iGUID, int iVarIndex, int iVar )
{
	int iTeamID		= iTeamIndex - 1;
	int iVarID		= iVarIndex - 1;

	StructBattleGround* pBG = NULL;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;
			if( pBG->iRoomID == iRoomID )
			{
				vector<StructBattleGroundRoleInfo>* pTeam = NULL;
				if( iTeamID >= 0 && iTeamID < m_BG_iNumTeam )
				{		
					pTeam = &( pBG->vecTeam[ iTeamID ] );

					for( vector<StructBattleGroundRoleInfo>::iterator itObj = pTeam->begin(); itObj != pTeam->end(); itObj++ )
					{
						StructBattleGroundRoleInfo* pObj	= &(*itObj);

						RoleDataEx* pRole = Global::GetRoleDataByGUID( iGUID );

						if( pRole != NULL )
						{					
							if( iVarID >= 0 && iVarID < DF_MAX_BG_ROLEVAR )
							{
								pObj->iRoleVar[ iVarID ] = iVar;
							}					
						}
					}
				}
			}
		}
	}
}
//-----------------------------------------------------------------------
int	CNetSrv_BattleGround_Child::GetMemberVar( int iRoomID, int iTeamIndex, int iGUID, int iVarIndex )
{
	int iTeamID		= iTeamIndex - 1;
	int iVarID		= iVarIndex - 1;

	StructBattleGround* pBG = NULL;

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			pBG = *it;
			if( pBG->iRoomID == iRoomID )
			{
				vector<StructBattleGroundRoleInfo>* pTeam = NULL;
				if( iTeamID >= 0 && iTeamID < m_BG_iNumTeam )
				{		
					pTeam = &( pBG->vecTeam[ iTeamID ] );

					for( vector<StructBattleGroundRoleInfo>::iterator itObj = pTeam->begin(); itObj != pTeam->end(); itObj++ )
					{
						StructBattleGroundRoleInfo* pObj	= &(*itObj);

						RoleDataEx* pRole = Global::GetRoleDataByGUID( iGUID );

						if( pRole != NULL )
						{										
							if( iVarID >= 0 && iVarID < DF_MAX_BG_ROLEVAR )
							{
								return pObj->iRoleVar[ iVarID ];
							}					
						}
					}
				}
			}
		}
	}
}
//-----------------------------------------------------------------------
void CNetSrv_BattleGround_Child::InitBattleGroundList()
{
	m_vecBattleGroundList.clear();

	GameObjDbStructEx* pZoneObj	= NULL;

	int iMaxZoneID = Def_ObjectClass_Zone + 10000;

	for( int iZoneID = Def_ObjectClass_Zone; iZoneID < iMaxZoneID; iZoneID++ )
	{
		pZoneObj = Global::GetObjDB( ( iZoneID % 1000 ) + Def_ObjectClass_Zone);

		if( pZoneObj )
		{
			if( pZoneObj->Zone.IsEnableBattleGroundQueue != false )
			{
				StructBattleGroundListInfo Info;

				Info.iZoneID	= ( iZoneID - Def_ObjectClass_Zone );

				if( pZoneObj->Zone.IsWorldBattleGround != false )
				{
					Info.iWorldID	= pZoneObj->Zone.iWorldBattleGroundWorldID;
				}
				else
				{
					Info.iWorldID	= 0;
				}

				m_vecBattleGroundList.push_back( Info );
			}
		}
	}
}
//-----------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRC_LeaveBattleGroundWaitQueue ( int iClientDBID, int iBattleGroundType )
{

	GameObjDbStructEx*	pZoneObj	= NULL;
	//int					iZoneID		= Def_ObjectClass_Zone + iBattleGroundType;

	int					iWorldID	= 0;



	pZoneObj = Global::GetObjDB( Def_ObjectClass_Zone + ( iBattleGroundType % 1000 ) );

	if( pZoneObj && pZoneObj->Zone.IsWorldBattleGround != false )
	{
		iWorldID = pZoneObj->Zone.iWorldBattleGroundWorldID;
	}

	if( iWorldID == 0 )
	{
		PG_BattleGround_LtoL_LeaveBattleGroundWaitQueue	Packet;

		Packet.iBattleGroundType	= iBattleGroundType;
		Packet.iDBID				= iClientDBID;

		SendToLocal( Packet.iBattleGroundType, sizeof( PG_BattleGround_LtoL_LeaveBattleGroundWaitQueue), &Packet );
	}
	else
	{
		PG_BattleGround_WLtoWL_LeaveBattleGroundWaitQueue	Packet;

		Packet.iBattleGroundType	= iBattleGroundType;
		Packet.iDBID				= iClientDBID;

		Global::SendToOtherWorld_GSrvID( iWorldID, iBattleGroundType, sizeof(Packet), &Packet );
	}
}

void CNetSrv_BattleGround_Child::SC_AllPlayerInfo()
{
	vector< StructBattleGroundRoleInfo > vecAllPlayerDBID;

	//for each room
	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			StructBattleGround* pBG = *it;

			if( pBG != NULL )
			{
				StructBattleGroundRoleInfo*	pRoleInfo	= NULL;			
				RoleDataEx*					pRole		= NULL;


				//collect position information for all team players
				for( int iTeamID = 0; iTeamID < m_BG_iNumTeam; iTeamID++ )
				{
					vector<StructBattleGroundRoleInfo>* pTeam = &( pBG->vecTeam[ iTeamID ] );
					for( vector<StructBattleGroundRoleInfo>::iterator itRole = pTeam->begin(); itRole != pTeam->end(); itRole++ )
					{
						pRoleInfo	= &( *itRole );			
						pRole		= Global::GetRoleDataByDBID( pRoleInfo->iClienDBID );
						
						//The Player is really in battle ground
						if( pRole != NULL && pRoleInfo->emStatus == EM_BG_ROLEINFO_STATUS_IN )
						{
							SC_BattleGroundScore( pRoleInfo->iRoleWorldID, pRoleInfo->iRoleZoneID, pRoleInfo->iClienDBID, pBG->iRoomID );
						}
					}
				}
			}
		}
	}
}
//-----------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRD_GetRankPersonalInfoResult( int iWorldID, int iWorldSrvID, PVOID pData )
{
	PG_BattleGround_WDtoWL_GetRankPersonalInfoResult* pResult = (PG_BattleGround_WDtoWL_GetRankPersonalInfoResult*)pData;

	int iClientDBID = pResult->iWorldDBID % _DEF_MAX_DBID_COUNT_;



	

}
//-----------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRD_AddRankPointResult( int iWorldID, int iWorldSrvID, PVOID pData )
{
	PG_BattleGround_WDtoWL_AddRankPointResult* pResult = (PG_BattleGround_WDtoWL_AddRankPointResult*)pData;



}
//-----------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRC_GetRankPersonalInfo( int iCliID , PVOID pData )
{
	PG_BattleGround_CtoL_GetRankPersonalInfo* pPacket = (PG_BattleGround_CtoL_GetRankPersonalInfo*)pData;

	BaseItemObject*	pObj =	Global::GetObjBySockID( iCliID );
	
	if( pObj != NULL && pObj->Role()->DBID() != 0 )
	{
		int iDBID = pObj->Role()->DBID();
		if( iDBID == 0 )
			return;

		int	iWorldID	= Global::Net()->GetWorldID();
		int iWorldDBID	=  iDBID + ( iWorldID * _DEF_MAX_DBID_COUNT_ );

		SD_GetRankPersonalInfo( iWorldDBID );
	}
}
//-----------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRC_LeaveBattleGroundRemote( int iClientDBID, int iBattleGroundType )
{
	GameObjDbStructEx*	pZoneObj	= NULL;
	int					iZoneID		= Def_ObjectClass_Zone + iBattleGroundType;
	int					iWorldID	= 0;

	pZoneObj = Global::GetObjDB( iZoneID % 1000 );



		PG_BattleGround_LtoL_LeaveBattleGroundRemote	Packet;

		Packet.iBattleGroundType	= iBattleGroundType;
		Packet.iDBID				= iClientDBID;

		Global::SendToLocal( iBattleGroundType, sizeof(Packet), &Packet ); 
}
//-----------------------------------------------------------------------
void CNetSrv_BattleGround_Child::OnRZ_LeaveBattleGroundRemote( int iClientDBID, int iBattleGroundType )
{

	for( int iLVID = 0; iLVID < DF_BG_MAXLVGROUP; iLVID++ )
	{
		for( vector< StructBattleGround* >::iterator it = m_vecBattleGround[ iLVID ].begin(); it != m_vecBattleGround[ iLVID ].end(); it++ )
		{
			StructBattleGround* pBG = *it;

			for( int iTeamID = 0; iTeamID < DF_BG_MAXTEAM; iTeamID++ )
			{
				if( pBG->vecTeam[ iTeamID ].size() == 0 )
					continue;

				//vector<StructBattleGroundRoleInfo>* pTeam = &( pBG->vecTeam[ iTeamID ] );

				//pTeam = &( pBG->vecTeam[ iTeamID ] );

					for( vector<StructBattleGroundRoleInfo>::iterator itRole = pBG->vecTeam[ iTeamID ].begin(); itRole != pBG->vecTeam[ iTeamID ].end(); itRole++ )
					{
						StructBattleGroundRoleInfo* pRoleInfo = &(*itRole);

						if( pRoleInfo->iClienDBID % _DEF_MAX_DBID_COUNT_ == iClientDBID % _DEF_MAX_DBID_COUNT_ )
						{				
							RoleDataEx* pRole = Global::GetRoleDataByDBID( pRoleInfo->iClienDBID );				

							if( pRole != NULL )
							{

								if( m_BG_iWorldBattleGround == 0 )
								{
									bool bLeaveZone = LuaPlotClass::ChangeZone( pRole->GUID(), pRoleInfo->iRoleZoneID, pRoleInfo->iRoleRoomID, pRoleInfo->fRoleX, pRoleInfo->fRoleY, pRoleInfo->fRoleZ, pRoleInfo->fRoleDir );

									if( bLeaveZone == false )
									{
										bLeaveZone = LuaPlotClass::ChangeZone( pRole->GUID(), pRole->SelfData.ReturnZoneID, pRole->RoomID(), pRole->SelfData.ReturnPos.X, pRole->SelfData.ReturnPos.Y, pRole->SelfData.ReturnPos.Z, pRole->SelfData.ReturnPos.Dir );
									}
								}
								else
								{
									NetSrv_CliConnectChild::WorldReturnNotifyProc( pRole->GUID() );							
								}
							}

							pBG->vecTeam[ iTeamID ].erase( itRole );
							return;
						}
					}				
			}
		}
	}
}
//-----------------------------------------------------------------------
