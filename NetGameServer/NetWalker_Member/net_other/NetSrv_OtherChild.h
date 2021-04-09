#pragma once
#include "NetSrv_Other.h"

class NetSrv_OtherChild : public NetSrv_Other
{
	static int		ManageKey;

	static int		MoneyKeyValueCount;
	static int		MoneyKeyValueSize;
	static char		MoneyKeyValueZip[0x10000];

	static int		DBStringCount;
	static int		DBStringSize;
	static char		DBStringZip[0x10000];
	
	static vector< map< int , MapMarkInfoStruct > > AllMarkInfo;
	static vector< map< int , int > >	AllRoomValueInfo;
	static map< string , int >			SysKeyValue;			//後台設定的SysKeyValue
	static vector<WeekZoneStruct>		WeekInst;
public:
	NetSrv_OtherChild() { ManageKey = rand(); };
    static bool Init();
    static bool Release();

	virtual void R_RequestPlayerPos	( int ZoneID , int DBID , char* Name );
	virtual void R_PlayerPos		( int ZoneID , int DBID , int RoomID , RolePosStruct& Pos );
	virtual void R_CallPlayer		( int ZoneID , RolePosStruct& Pos , char* Name , int RoomID );

	virtual void R_DialogSelectID			( BaseItemObject* Sender  , int SelectID );
	virtual void R_BeginCastingRequest		( BaseItemObject* Sender , CastingTypeENUM Type , int ItemID , int TargetGUID );
	virtual void R_InterruptCastingRequest	(  BaseItemObject* Sender  );
	virtual void R_RangeDataTransferRequest	( BaseItemObject* Sender , int Range , int Size , const char* Data );
	virtual void R_ColoringShopRequest		( BaseItemObject* Sender , ColoringStruct& ColorInfo , const char* Password );
	virtual void R_PlayerInfoRequest		( BaseItemObject* Sender , int PlayerGItemID );
	virtual void R_SysKeyFunctionRequest	( BaseItemObject* Sender , int KeyItemID , int TargetGUID );
	virtual void R_AllObjectPosRequest		( BaseItemObject* Sender );
	virtual void R_RunGlobalPlotRequest		( BaseItemObject* Obj , int PlotID , int Value );
	virtual void R_HateListRequest			( BaseItemObject* Obj , int NpcGUID );
	virtual void R_ManagePasswordKey		( BaseItemObject* Obj );
	virtual void R_ManagePassword			( BaseItemObject* Obj , const char* Md5Pwd );
	virtual void R_GiveItemPlot				( BaseItemObject* Obj , bool IsSort , int ItemPos[4] );
	virtual void R_CheckPassword			( BaseItemObject* Obj , char* Password );
	virtual void RC_Find_DBID_RoleName		( BaseItemObject* Obj , int DBID , const char* RoleName );
	//virtual void RC_SendPerformance		(  BaseItemObject* Obj, const char* cMac, float MaxCPUUsage, float MaxMemUsage, float MaxFPS, float AverageCPUUsage, float AverageMemUsage, float AverageFPS, float MinCPUUsage, float MinMemUsage, float MinFPS, float Ping, float PosX, float PosY, float PosZ );
	virtual void R_DialogInput				( BaseItemObject* Sender  , int SelectID, const char* pszInput, int iCheckResult );

	virtual	void RD_ExchangeMoneyTable		( int Count , int DataSize , char* Data );
	virtual	void RD_DBStringTable			( int Count , int DataSize , char* Data );	
	virtual void RC_GmNotification			( BaseItemObject* Obj , const char* Subject, const char* Content, const char* Classification );

	static void SendRange_GoodEvilValue		( RoleDataEx *Owner );
	static void SendRangeCampID				( RoleDataEx *Owner , CampID_ENUM CampID );

	static void SendClient_DBTable			( int SendID );
	
	virtual	void OnEvent_ClientDisconnect	( BaseItemObject* pObj );

	virtual void RC_ColoringHorse			( BaseItemObject* Obj , int Color[4] , char* Password , int ItemPos );
	virtual void RC_RoleNameDuplicateErr	( BaseItemObject* Obj , const char* RoleName , int PlayerDBID[2] );
	virtual void RC_LookTarget				( BaseItemObject* Obj , int TargetGUID );
	virtual void RC_SetCycleMagic			( BaseItemObject* Obj , int MagicID , int MagicPos  );
	virtual void RC_CloseMaster				( BaseItemObject* Obj , const char* Password );
	virtual void RC_SetSendPacketMode		( BaseItemObject* Obj , SendPacketModeStruct& Mode );
	//重置副本
	virtual void RC_ResetInstanceRequest	( BaseItemObject* Obj , int ZoneID );
	virtual void RL_ResetInstance			( int PlayerDBID , int PartyID );

	virtual void RC_GmResetRolet			( BaseItemObject* Obj , int RoleBaseSize , int PlayerRoleBaseSize , BaseRoleData& BaseData , PlayerBaseRoleData& PlayerBaseData  );
	virtual void RC_PartyDice				( BaseItemObject* Obj , int Type );

	virtual void RC_BodyShop				( BaseItemObject* Obj , char BoneScale[15] );

	virtual void RC_MoneyVendorReport		( BaseItemObject* Obj , const char* Name , const char* Content );
	virtual void RC_BotReport				( BaseItemObject* Obj , const char* Name , const char* Content );
	virtual void RC_UnlockBotReport			( BaseItemObject* Obj );

	virtual void RC_SortBankRequest			( BaseItemObject* Obj );

	virtual void RC_LoadClientData			( BaseItemObject* Obj , int KeyID );
	virtual void RC_SaveClientData			( BaseItemObject* Obj , int KeyID , int PageID , int Size , const char* Data );

	//////////////////////////////////////////////////////////////////////////
	//設定地圖 Iocn 與 Tip
	static bool ResetMapMark	( int RoomID );
	static bool SetMapMark		( int RoomID , int MarkID , float X , float Y , float Z , const char* TipStr , int IconID );
	static bool ClsMapMark		( int RoomID , int MarkID );

	static void OnEnterSendMapMark( int SendID );

	static bool ResetRoomValue	( int RoomID );
	static int  GetRoomValue	( int RoomID , int KeyID );
	static bool SetRoomValue	( int RoomID , int KeyID , int Value );
	static bool ClsRoomValue	( int RoomID , int KeyID );

	static void OnEnterSendRoomValue( int SendID );

	//////////////////////////////////////////////////////////////////////////
	virtual void RC_Suicide				( BaseItemObject* Obj , int Type );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_SpellExSkill		( BaseItemObject* Obj , int TargetID , float TargetX , float TargetY , float TargetZ , ExSkillBaseStruct& Info , int ShootAngle , int ExplodeTime );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_ClientEventLog		( BaseItemObject* Obj , int EventType );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_TransferData_Range	( BaseItemObject* Obj , char Data[20] );
	//////////////////////////////////////////////////////////////////////////
	//client 端要求改變角色數值
	virtual void RC_SetRoleValue		( BaseItemObject* Obj , RoleValueName_ENUM Type , double Value );
	//////////////////////////////////////////////////////////////////////////
	virtual void RD_SysKeyValue			( int TotalCount , int ID , char* KeyStr , int Value );
	static int GetDBKeyValue			( char* KeyStr );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_ClientLanguage		( BaseItemObject* Obj, int iLanguage );
	//////////////////////////////////////////////////////////////////////////
	virtual void RD_RoleRunPlot			( const char* LuaPlot, int DBID );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_SetNewTitleSys		( BaseItemObject* Obj , NewTitleSysPosENUM Pos , int TitleID );
	virtual void RC_NewTitleSys_UseItem	( BaseItemObject* Obj  , int ItemType  , int Pos );
	virtual void RL_PCall				( const char* plotStr );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_OpenSession( BaseItemObject* Obj, SessionTypeENUM Type, int CBID );
	virtual void RC_CloseSession( BaseItemObject* Obj, SessionTypeENUM Type, int SessionID );
	virtual void RC_AutoReportPossibleCheater( BaseItemObject* Obj , AutoReportBaseInfoStruct& TargetInfo , AutoReportPossibleCheaterENUM Type );

	//////////////////////////////////////////////////////////////////////////
	//今天的複本
	virtual void RD_WeekInstances( int count , WeekZoneStruct info[] );
	//要求今天的複本
	virtual void RC_WeekInstancesRequest( BaseItemObject* obj  );
	//要求傳送到某副本
	virtual void RC_WeekInstancesTeleport( BaseItemObject* obj , int zoneID );
	//要求重置某副本
	virtual void RC_WeekInstancesZoneReset( BaseItemObject* obj , int zoneID );
	//要求重置某副本
	virtual void RL_WeekInstancesZoneReset( int netID , int dbid , int partyID );
	//重置副本傳送次數
	virtual void RC_WeekInstancesReset( BaseItemObject* obj  );
	static bool WeekInstancesResetProc( BaseItemObject* obj  );

	static void ResetWeekInstCount( RoleDataEx* role );
	static void SendWeekInstancesLiveTime( RoleDataEx* role );
	//////////////////////////////////////////////////////////////////////////
		//魔靈升階(或產生)
	virtual void RC_PhantomRankUp( BaseItemObject* obj , int phantomID , bool summon );
	//魔靈升級
	virtual void RC_PhantomLevelUp( BaseItemObject* obj , int phantomID );
	//魔靈裝備
	virtual void RC_PhantomEQ( BaseItemObject* obj , int eQ[] );
	static void PhantomChangeProc( RoleDataEx* role );

	//後台Buff設定
	virtual void RD_BuffSchedule( int buffID , int buffLv , int buffTime );
	//後台魔幻寶盒資料
	virtual void RD_ItemCombinInfo( int groupID , ItemCombineStruct& itemCombine );

};

