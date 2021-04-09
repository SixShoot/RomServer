#pragma once

#include ".\NetSrv_BattleGround.h"


#define DF_BG_TIME_SENT_QUEUE_STATUS			10000
#define DF_BG_TIME_CHECK_NO_RESPOND_QUEUEOBJ	20000
#define DF_BG_TIME_DELETE_NO_RESPOND_QUEUEOBJ	150000
#define	DF_BG_WAIT_ENTER_TIME					60000
#define	DF_BG_DICONNECT_TO_KICK					150000
#define	DF_BG_CHECK_OBJ_EXIST_TIME				150000
#define DF_BG_UPDATE_PLAYER_POS_TIME			3000
#define DF_ARENA_TYPE							3




enum EM_BG_ROLEINFO_STATUS
{
	EM_BG_ROLEINFO_STATUS_IN						= 0,	// 正在戰場中
	EM_BG_ROLEINFO_STATUS_NOTIFIED_WAIT_TO_ENTER_BG	= 1,	// 已通知, 等待進入 ( 過久會被移除戰場 )
	EM_BG_ROLEINFO_STATUS_DISCONNECT_INBG			= 2,	// 斷線了, 等待上線 ( 過久會被移除戰場 )
	EM_BG_ROLEINFO_STATUS_QUEUE						= 3,	// 排隊中
	EM_BG_ROLEINFO_STATUS_DISCONNECT_QUEUE			= 4,	// 排隊中

	EM_BG_ROLEINFO_STATUS_NONE						= 255,	// 初始狀態
};



enum EM_BG_STATUS
{
	EM_BG_STATUS_NONE					= 0,
	EM_BG_STATUS_CLOSING				= 1,	// 戰場結束中
	EM_BG_STATUS_DESTORING				= 2,	// 戰場解構
	EM_BG_STATUS_RUNNING				= 3,	// 戰場正常運作中
};


struct StructBattleGroundListInfo
{
	int			iZoneID;
	int			iWorldID;
};

struct StructBattleGroundRoleInfo
{
	StructBattleGroundRoleInfo()
	{
		RoleName	= "unknow";

		ZeroMemory( &Info, sizeof(Info) );
		ZeroMemory( &iRoleVar, sizeof(iRoleVar) );

		iRoleWorldID		= 0;

		fRoleX				= 0;
		fRoleY				= 0;
		fRoleZ				= 0;
		fRoleDir			= 0;	
		iRoleRoomID			= 0;

		bMoveToBattleGround	= false;
	}

	int						iClienDBID;				// 人物 GUID
	
	int						iRoleWorldID;
	int						iRoleWorldSrvID;

	int						iRoleRoomID;	
	int						iRoleZoneID;							// 人物進入戰場前, 排隊點的區域
	float					fRoleX, fRoleY, fRoleZ, fRoleDir;		// 人物進入戰場前, 排隊點的位置
	float					fRoleHP, fRoleMP;						// 人物進入戰場前, 屬性資料

	EM_BG_ROLEINFO_STATUS	emStatus;				// 戰場狀態
	int						iTeamID;				// 所屬隊伍
					
	int						iBeginWaitTime;			// 開始等待時間
	int						iBeginDisconnectTime;	// 開始斷線時間
	int						iLastCheckTime;		

	Voc_ENUM				Voc, Voc_Sub;
	int						LV, LV_Sub;
	StaticString< _MAX_NAMERECORD_SIZE_ >	RoleName;
	BattleGroundInfoStruct					Info;

	int						iRoleVar[DF_MAX_BG_ROLEVAR];
	bool					bMoveToBattleGround;

};

struct StructBattleGroundEnterPoint
{
	int		iTeamID;
	float	X,Y,Z,Dir;
};


struct StructBattleGround
{
	StaticString< _MAX_OBJ_NAME_STR_SIZE_ >		szName;

	int											iRoomID;
	//vector<StructBattleGroundRoleInfo>			vecTeam1;
	//vector<StructBattleGroundRoleInfo>			vecTeam2;
	vector<StructBattleGroundRoleInfo>			vecTeam[ DF_BG_MAXTEAM ];

	EM_BG_STATUS								emBattleGroundStatus;
	int											iCloseTime;

	StructBattleGround()
	{
		emBattleGroundStatus	= EM_BG_STATUS_NONE;
	}
};

struct StructBattleGroundQueue
{

	StructBattleGroundQueue()
	{

		iDBID					= 0;
		iBattleGroundID			= 0;	// 玩家是否有指定戰場房間

		iSourceZoneID			= 0;

		iBeginDisconnectTime	= 0;	// 斷線時間
		iLastCheckQueueTime		= 0;	// 上次檢查時間
		emStatus				= EM_BG_ROLEINFO_STATUS_NONE;

		iSourceWorldID			= 0;
		iSourceWorldSrvID		= 0;

		iWorldGroupID			= 0;
		iPartyLeaderDBID		= 0;

		iLV						= 0;

	}
	//int		iClientID;			// 排隊的玩家代號
	//vector< int >	vecClientID;

	int						iLV;
	int						iDBID;
	int						iBattleGroundID;	// 玩家是否有指定戰場房間

	//float					fSourceX, fSourceY, fSourceZ, fSourceDir;
	int						iSourceZoneID;
	//int						iSourceRoomID;
	int						iBeginDisconnectTime;		// 斷線時間
	int						iLastCheckQueueTime;		// 上次檢查時間
	EM_BG_ROLEINFO_STATUS	emStatus;

	int						iSourceWorldID;
	int						iSourceWorldSrvID;

	int						iWorldGroupID;
	int						iPartyLeaderDBID;

};



class CNetSrv_BattleGround_Child : public CNetSrv_BattleGround
{
public:
	CNetSrv_BattleGround_Child(void)	{};
	~CNetSrv_BattleGround_Child(void)	{};

	static	void		Init();
	static  void		Release();

	static	int			OnTime_CheckBattleGroundQueue			( SchedularInfo* Obj, void* InputClass );
	static	int			OnTime_CheckBattleGroundObjExist		( SchedularInfo* Obj, void* InputClass );
	static  int			OnTime_UpdateAllBattleGroundPlayerPos	(SchedularInfo* Obj, void* InputClass );


	//-------------------------------------------------------

	virtual	void		InitBattleGround						();

	virtual void		InitBattleGroundList					();

	// Client 要求取得戰場狀態
	virtual void		OnRC_GetBattleGround					( int iClientID, int iObjID );
	virtual void		OnRZ_GetBattleGround					( int iWorldID, int iWorldSrvID, int iClientID );
	
	// Client 要求參加戰場等待佇列
	virtual void		OnRC_JoinBattleGround					( BaseItemObject* pObj, int iObjID, int iBattleGroundID, int iPartyID, int iSrvID, int* pTeamDBID );
	//virtual void		OnRZ_JoinBattleGround					( int iWorldID, int iWorldSrvID, int iClientID, int iBattleGroundID, int iPartyID, int iZoneID, int iRoomID, float fX, float fY, float fZ, float fDir );
	virtual void		OnRZ_JoinBattleGround					( int iWorldID, int iWorldSrvID, int iClientLV, int iClientID, int iBattleGroundID, int iPartyID, int iZoneID, int iWorldGroupID, int* pTeamDBID, int* pTeamLV );

	// Client 要求進入戰場 ( 戰場事先已經通知 Client 可以登入	 )
	virtual void		OnRC_EnterBattleGround					( BaseItemObject* pObj, int iType, int iRoomID, int iTeamID );
	virtual void		OnRZ_EnterBattleGround					( int iWorldID, int iWorldSrvID, int iClientDBID, int iType, int iRoomID, int iTeamID, int iRoleZoneID, int iRoleRoomID, float fRoleX, float fRoleY, float fRoleZ, float fRoleDir, float fRoleHP, float fRoleMP );

	// Client 要求離開戰場
	virtual void		OnRC_LeaveBattleGround					( BaseItemObject* pObj );
		
	virtual void		OnRZ_ChangeZoneToBattleGround			( int iWorldID, int iSrvID, int iClientDBID, int iZoneID, int iRoomID, float X, float Y, float Z, float Dir );

	virtual	void		OnRC_LeaveBattleGroundWaitQueue			( int iClientDBID, int iBattleGroundType );
	virtual void		OnRZ_LeaveBattleGroundWaitQueue			( int iWorldID, int iWorldSrvID, int iClientDBID, int iBattleGroundType );

	virtual	void		OnRC_LeaveBattleGroundRemote			( int iClientDBID, int iBattleGroundType );
	virtual void		OnRZ_LeaveBattleGroundRemote			( int iClientDBID, int iBattleGroundType );

	virtual void		OnRC_GetBattleGroundList				( int iClientDBID );
	virtual	void		OnRZ_GetBattleGroundList				( int iWorldID, int iWorldSrvID, int iClientDBID );

	virtual	void		OnEvent_CheckBattleGroundQueue			();
	virtual void		OnEvent_CheckDisconnect					();

	virtual	bool		OnEvent_ClientConnect					( BaseItemObject* pObj );
	virtual	void		OnEvent_ClientDisconnect				( BaseItemObject* pObj );
	virtual	void		OnEvent_ClientLoaded					( BaseItemObject* pObj );
	

	virtual	void		OnRC_GetBattleGroundScoreInfo			( int iClientDBID );

	virtual void		OnRD_UpdateArenaData					( int iArenaType, int iTeamGUID, StructArenaInfo* pArenaInfo );
	virtual void		OnRC_BattleGroundWaitQueueStatusRespond	( int iClientDBID );

	virtual	void		OnRD_GetRankPersonalInfoResult			( int iWorldID, int iWorldSrvID, PVOID pData );
	virtual	void		OnRC_GetRankPersonalInfo				( int iCliID ,PVOID pData );
	virtual	void		OnRD_AddRankPointResult					( int iWorldID, int iWorldSrvID, PVOID pData );
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	virtual void		CheckBattleGround						();
	virtual	void		CheckWaitQueue							();
	//virtual	void		CheckEmptyBG							( vector< StructBattleGroundQueue >* pvecQueue, int iTeamID );
	//virtual	void		CheckQueue								( vector< StructBattleGroundQueue >* pvecQueue, int iTeamID );
	virtual	void		CheckQueue								();

	virtual void		DestoryEmptyBG							();
	
	virtual	void		CheckQueueToCreateBattleGround			();
	virtual	void		InvitePlayer							( StructBattleGround* pBG, vector< int >& vecDBID, int iLVID, int iTeamID );

	//virtual	void		InvitePlayerToBattleGround				( StructBattleGround* pBG, 

	virtual	bool		CheckRoleIsExistBattleGround			( int iClientDBID );
	virtual int			SearchRoleInBattleGround				( int iClientDBID, StructBattleGroundRoleInfo* pRoleInfo );
	virtual bool		SearchRoleInWaitQueue					( int iClientDBID, StructBattleGroundQueue* pQueueObj, bool bEraseIfExist );

	//-------------------------------------------------------
	virtual	void		SendTicketToClient						( StructBattleGround* pBG, vector<StructBattleGroundRoleInfo>* pTeam,  vector< StructBattleGroundQueue >* pvecQueue, int iTeamID );
	
	static	void		SetBattleGroundObjClickState			( int iObjID, int iTeamID, int iClickState );
	static	void		SetScore								( int iRoomID, int iTeamID, int iScore );
	static	int			GetScore								( int iRoomID, int iTeamID );
	virtual	void		EndBattleGround							( int iRoomID, int iTeamID );
	void				CloseBattleGround						( int iRoomID );

	virtual int 		GetLandMark								( int iRoomID, int iID );
	virtual void		SetLandMark								( int iRoomID, int iID, int iVal );
	virtual void		SetLandMarkInfo							( int iRoomID, int iID, int iVal, float fX, float fY, float fZ, const char* pszName, int iIconID );

	virtual int 		GetVar									( int iRoomID, int iID );
	virtual void		SetVar									( int iRoomID, int iID, int iVal );

	virtual	void		SendDataToClientInRoom					( int iRoomID );
	virtual void		SendLandInfoToClientInRoom				( int iRoomID, int iID, StructLandMark* pLandMark );

	virtual	int			GetPlayerFlagID							( RoleDataEx* pRole );

	virtual int			GetEnterTeamMemberInfo					( int iRoomID, int iTeamID, int iID );

	//-------------------------------------------------------
	static	int			GetFlagID								( int iRoleCampID );
	static	int			GetBattleGroundType						()	
	{ 
		if( m_pThis == NULL )
			return 0;
		else
			return ((CNetSrv_BattleGround_Child*)m_pThis)->m_BG_iBattleGroundType; 
	}
	//-------------------------------------------------------

	virtual	void		JoinBattleGroundQueue					( int iWorldID, int iWorldSrvID, int iWorldGroupID, int iLV, int iClientDBID, int iBattleGroundID, int iPartyLeaderDBID, int iZoneID, int* pAssignQueueID ); // int iRoomID, float fX, float fY, float fZ, float fDir );
	virtual	void		SC_BattleGroundScore					( int iWorldID, int iWorldZoneID, int iClientDBID, int iRoomID );
	virtual void		SC_AllPlayerPos							();
	virtual	void		SC_AllPlayerInfo						();
	//-------------------------------------------------------
	void				SetArenaScore							( int iRoomID, int iTeamID, int iArenaType, int iIndex, float iValue );
	float				GetArenaScore							( int iRoomID, int iTeamID, int iArenaType, int iIndex	);
	int					GetNumTeamMember						( int iRoomID, int iTeamID );	
	int					GetDisconnectTeamMember					( int iRoomID, int iTeamID );
	int					GetNumEnterTeamMember					( int iRoomID, int iTeamID );	
	void				AddTeamHonor							( int iRoomID, int iTeamID, int iHonor );
	void				AddTeamItem								( int iRoomID, int iTeamID, int iItemID, int iItemCount );

	void				SetMemberVar							( int iRoomID, int iTeamID, int iGUID, int iVarID, int iVar );
	int					GetMemberVar							( int iRoomID, int iTeamID, int iGUID, int iVarID );
	static int			GetBattleGroundRoomID					( int iID );
	//-------------------------------------------------------





	vector< StructBattleGround* >			m_vecBattleGround[ DF_BG_MAXLVGROUP ];			// 目前正在進行的戰場
	vector< StructBattleGroundQueue >		m_vecWaitQueue[ DF_BG_MAXLVGROUP][ DF_BG_MAXTEAM ];			// For team2

	map< int, StructBattleGroundQueue >		m_mapClientInfo;			// 用 DBID 索引的 Client 資料, 用於連線及斷線時, 快速找到 Client 資料用


	set< int >								setFreeID;
	vector< StructBattleGroundValue >		vecValue;
	vector< StructRoomLandMark >			vecLandMark;

	// BattleGround Setting
	//-------------------------------------------------------
	int										m_BG_iNumTeamPeople;			// 每個隊伍的人數限制
	int										m_BG_iMinTeamPeople;			// 隊伍內人數到達後即開局	0 的話, 表示以 m_BG_iNumTeamPeople 為設定
	int										m_BG_iKickDisconnect;			// 是否踢掉斷線的人
	int										m_BG_iBattleGroundType;
	int										m_BG_iNumTeam;				// 最多允許的隊伍人數
	int										m_BG_iFillFromQueue;		// 遇缺是否從佇列中選人補位
	int										m_BG_iDisableRoleReconnect;
	int										m_BG_iDisableDelNoRespondQueueObj;		// 是否踢掉送出進入通知, 但不進入的玩家
	int										m_BG_iWorldQueueRule;					// 世界群組排隊規則
		//	玩家斷線後是否允許重新登入, 同時也會使, BattleGround 系統不去刪除斷線的玩家,
		//	因為要在戰場結束後, 讀取其資料, 並且設定成績
		//  也就是說, 玩家斷線後, 系統仍會設定該成績

	bool									m_IsLimitJoinLevel;
	int										m_iMaxJoinLevel;
	int										m_iMinJoinLevel;

	bool									m_IsForceGroup;

	int										m_BG_iWorldBattleGround;		// 是否為跨世界戰場

	int										m_BG_iWorldGroupID;				// 世界戰場群組
	
	StructBattleGroundEnterPoint			m_EnterPoint[2];

	map< int, int >							m_mapTeamFlag;

	// Lua Event
	string									m_BG_Lua_AssignLevelGroup;
	string									m_BG_Lua_RoleJoinQueue;
	string									m_BG_Lua_MoveRoleToBattleGround;
	string									m_BG_Lua_RoleEnterBattleGround;
	string									m_BG_Lua_ClientLoaded;

	//-------------------------------------------------------
	map< int, StructArenaInfo >				m_ArenaInfo[ DF_ARENA_TYPE ];

	vector< int >							m_vectempTeamInfo;

	//-------------------------------------------------------
	vector< StructBattleGroundListInfo >	m_vecBattleGroundList;

};
