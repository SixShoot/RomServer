#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_Other.h"

struct Struct_DialogTempInfo
{
	int					iSelectID;
	int					isAllowString;		// 是否通過髒話驗證 ( Client Check )
	StaticString< 256 > pszString;

	Struct_DialogTempInfo()
	{
		iSelectID		= 0;
		isAllowString	= 0;	// 0 不合法, 1 合法
		pszString.Clear();
	}
};

class NetSrv_Other : public Global
{
protected:
    static NetSrv_Other* This;
protected:
    static bool _Init();
    static bool _Release();

protected:
	static void _PG_Other_LtoL_RequestPlayerPos				( int NetID , int Size , void* Data );
	static void _PG_Other_LtoL_PlayerPos					( int NetID , int Size , void* Data );	
	static void _PG_Other_LtoL_CallPlayer					( int NetID , int Size , void* Data );	
	static void _PG_Other_CtoL_DialogSelectID				( int NetID , int Size , void* Data );	

	static void _PG_Other_CtoL_BeginCastingRequest			( int NetID , int Size , void* Data );	
	static void _PG_Other_CtoL_InterruptCastingRequest		( int NetID , int Size , void* Data );	
	static void _PG_Other_CtoL_Performance							( int NetID , int Size , void* Data );	
	static void _PG_Other_CtoL_TracertList							( int NetID , int Size , void* Data );	

	static void _PG_Other_CtoL_RangeDataTransferRequest		( int NetID , int Size , void* Data );	
	static void _PG_Other_CtoL_ColoringShopRequest			( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_PlayerInfoRequest			( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_SysKeyFunctionRequest		( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_AllObjectPosRequest			( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_RunGlobalPlotRequest			( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_HateListRequest				( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_ManagePasswordKey			( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_ManagePassword				( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_GiveItemPlot					( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_CheckPassword				( int NetID , int Size , void* Data );
	static void	_PG_Other_CtoL_Find_DBID_RoleName			( int NetID , int Size , void* Data );
	static void	_PG_Other_CtoL_DialogInput					( int NetID , int Size , void* Data );
	static void	_PG_Other_DtoL_ExchangeMoneyTable			( int NetID , int Size , void* Data );
	static void	_PG_Other_DtoL_DBStringTable				( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_GmNotification				( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_ColoringHorse				( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_RoleNameDuplicateErr			( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_LookTarget					( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_SetCycleMagic				( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_CloseMaster					( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_SetSendPacketMode			( int NetID , int Size , void* Data );

	static void _PG_Other_CtoL_ResetInstanceRequest			( int NetID , int Size , void* Data );
	static void _PG_Other_LtoL_ResetInstance				( int NetID , int Size , void* Data );

	static void _PG_Other_CtoL_GmResetRole					( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_PartyDice					( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_BodyShop						( int NetID , int Size , void* Data );

	static void _PG_Other_CtoL_MoneyVendorReport			( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_BotReport					( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_UnlockBotReport				( int NetID , int Size , void* Data );

	static void _PG_Other_CtoL_SortBankRequest				( int NetID , int Size , void* Data );

	static void _PG_Other_CtoL_LoadClientData				( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_SaveClientData				( int NetID , int Size , void* Data );

	static void _PG_Other_CtoL_Suicide						( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_SpellExSkill					( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_ClientEventLog				( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_TransferData_Range			( int NetID , int Size , void* Data );

	static void _PG_Other_CtoL_SetRoleValue					( int NetID , int Size , void* Data );
	static void _PG_Other_DtoL_SysKeyValue					( int NetID , int Size , void* Data );

	static void _PG_Other_CtoL_Client_Language				( int NetID , int Size , void* Data );
	static void _PG_Other_DtoL_RoleRunPlot					( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_SetNewTitleSys				( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_NewTitleSys_UseItem			( int NetID , int Size , void* Data );
	static void _PG_Other_LtoL_PCall						( int NetID , int Size , void* Data );

	static void _PG_Other_CtoL_OpenSession					( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_CloseSession					( int NetID , int Size , void* Data );
	static void _PG_Other_CtoL_AutoReportPossibleCheater	( int NetID , int Size , void* Data );

	static void _PG_Other_DtoL_WeekInstances				( int NetID , int size , void* data );
	static void _PG_Other_CtoL_WeekInstancesRequest			( int NetID , int size , void* data );
	static void _PG_Other_CtoL_WeekInstancesTeleport		( int NetID , int size , void* data );
	static void _PG_Other_CtoL_WeekInstancesZoneReset		( int NetID , int size , void* data );
	static void _PG_Other_LtoL_WeekInstancesZoneReset		( int NetID , int size , void* data );
	static void _PG_Other_CtoL_WeekInstancesReset			( int NetID , int size , void* data );

	static void _PG_Other_CtoL_PhantomRankUp				( int NetID , int size , void* data );
	static void _PG_Other_CtoL_PhantomLevelUp				( int NetID , int size , void* data );
	static void _PG_Other_CtoL_PhantomEQ					( int NetID , int size , void* data );
				
	static void _PG_Other_DtoL_BuffSchedule					( int NetID , int size , void* data );
	static void _PG_Other_DtoL_ItemCombinInfo				( int NetID , int size , void* data );

public:        
	static void S_RequestPlayerPos	( int DBID , char* Name );
	static void S_PlayerPos			( int SendZoneID , int DBID , int RoomID , RolePosStruct& Pos );
	static void S_CallPlayer		( RolePosStruct& Pos , char* Name , int RoomID );

	static void S_OpenDialog		( int SendID , LuaDialogType_ENUM Type , const char* Content );
	static void S_DialogSelectStr	( int SendID , const char* SelectStr );	
	static void S_DialogSelectStrEnd( int SendID );
	static void S_CloseDialog		( int SendID );
	static void S_DialogSetContent	( int SendID , const char* Content );	
	static void S_DialogSetTitle	( int SendID , const char* Title );	
	

//	static void S_SystemMessageData	( int SendID , SystemMessageDataENUM Msg , int size , const char* Data );
//	static void S_ItemExchangeResult( int SendID , ItemExchangeResultTypeENUM	ResultType , ItemFieldStruct& Item , int Money , int Money_Account );

	static void S_RangeDataTransfer	( int SendID , int Range , int Size , const char* Data );
	static void S_ColoringShopResult( int SendID , ColoringShopResultENUM Result );

	static void S_BeginCastingResult( int SendID , int ItemID , CastingTypeENUM Type , int CastingTime , bool Result );
	static void S_InterruptCasting	( int SendID , CastingTypeENUM Type );
	static void S_CastingOK			( int SendID , CastingTypeENUM Type );
	static void S_PlayerInfo		( int SendID , bool Result , int GItemID , EQStruct* EQ = NULL , const char* Note = NULL );

	static void S_SysKeyFunctionResult( int SendID , bool Result , int KeyID , int TargetGUID );

	static void S_ObjectPosInfo		( int SendID , vector<ObjectPosInfoStruct>& Info );

	static void S_OpenClientMenu	( int SendID , ClientMenuType_ENUM Type );

	static void S_RunGlobalPlotResult( int SendID , bool Result );

	static void S_OpenGiveItemPlot	( int SendID , int TargetGUID , bool IsSort , int Count );

	static void S_HateListResult	( int SendID , int NpcGUID , NPCHateList& NPCHate );
	static void S_CheckPasswordResult( int SendID , char* Password , bool Result );
	static void S_GoodEvilValue		( int SendID , int PlayerGUID , float GoodEvilValue , GoodEvilTypeENUM GoodEvilType );
	static void S_CampID			( int SendID , int GItemID , CampID_ENUM CampID );

	static void	SC_ExchangeMoneyTable	( int SendID , int Count , int DataSize , const char* Data );
	static void	SC_DBStringTable		( int SendID , int Count , int DataSize , const char* Data );
	static void	SC_AllCli_DBStringTable	( int Count , int DataSize , const char* Data );
	static void	SC_PlayerEvent		( int SendID , PlayerEventTypeENUM Type );
	
	static const char*	GetDialogInputStr				( int iObjID );
	static int			GetDialogInputCheckResult		( int iObjID );

	virtual void R_RequestPlayerPos	( int ZoneID , int DBID , char* Name ) = 0;
	virtual void R_PlayerPos		( int ZoneID , int DBID , int RoomID , RolePosStruct& Pos ) = 0;
	virtual void R_CallPlayer		( int ZoneID , RolePosStruct& Pos , char* Name , int RoomID ) = 0;

	virtual void R_DialogSelectID			( BaseItemObject* Sender  , int SelectID ) = 0;
	virtual void R_DialogInput				( BaseItemObject* Sender  , int SelectID, const char* pszInput, int iCheckResult ) = 0;
	virtual void R_BeginCastingRequest		( BaseItemObject* Sender , CastingTypeENUM Type , int ItemID , int TargetGUID ) = 0;
	virtual void R_InterruptCastingRequest	( BaseItemObject* Sender  ) = 0;
	virtual void R_RangeDataTransferRequest	( BaseItemObject* Sender , int Range , int Size , const char* Data ) = 0;
	virtual void R_ColoringShopRequest		( BaseItemObject* Sender , ColoringStruct& ColorInfo , const char* Password ) = 0;

	virtual void R_PlayerInfoRequest		( BaseItemObject* Sender , int PlayerGItemID ) = 0;
	virtual void R_SysKeyFunctionRequest	( BaseItemObject* Sender , int KeyID , int TargetGUID ) = 0;
	virtual void R_AllObjectPosRequest		( BaseItemObject* Sender ) = 0;
	virtual void R_RunGlobalPlotRequest		( BaseItemObject* Obj , int PlotID , int Value ) = 0;
	virtual void R_HateListRequest			( BaseItemObject* Obj , int NpcGUID ) = 0;
	virtual void R_ManagePasswordKey		( BaseItemObject* Obj ) = 0;
	virtual void R_ManagePassword			( BaseItemObject* Obj , const char* Md5Pwd ) = 0;
	virtual void R_GiveItemPlot				( BaseItemObject* Obj , bool IsSort , int ItemPos[4] ) = 0;
	virtual void R_CheckPassword			( BaseItemObject* Obj , char* Password ) = 0;

	virtual	void RD_ExchangeMoneyTable		( int Count , int DataSize , char* Data ) = 0;	
	virtual	void RD_DBStringTable			( int Count , int DataSize , char* Data ) = 0;	
	//////////////////////////////////////////////////////////////////////////
	//dbid 與 角色名稱 查尋
	virtual void RC_Find_DBID_RoleName( BaseItemObject* Obj , int DBID , const char* RoleName ) = 0;
	//virtual void RC_SendPerformance		( BaseItemObject* Obj, const char* cMac, float MaxCPUUsage, float MaxMemUsage, float MaxFPS, float AverageCPUUsage, float AverageMemUsage, float AverageFPS, float MinCPUUsage, float MinMemUsage, float MinFPS, float Ping, float PosX, float PosY, float PosZ ) = 0;
	static  void SD_Find_DBID_RoleName( int CliDBID , int FindDBID , const char* FindRoleName );
	//static  void SD_SendPerformance		( int CliDBID , const char* cMac, float MaxCPUUsage, float MaxMemUsage, float MaxFPS, float AverageCPUUsage, float AverageMemUsage, float AverageFPS, float MinCPUUsage, float MinMemUsage, float MinFPS, float Ping, float PosX, float PosY, float PosZ );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//回報GM
	virtual void RC_GmNotification			( BaseItemObject* Obj , const char* Subject, const char* Content, const char* Classification) = 0;
	//////////////////////////////////////////////////////////////////////////

	virtual	void OnEvent_ClientDisconnect			( BaseItemObject* pObj ) = 0;
	map< int, Struct_DialogTempInfo >		m_mapDialogInfo;

	//////////////////////////////////////////////////////////////////////////
	//	座騎染色
	//////////////////////////////////////////////////////////////////////////
	virtual	void RC_ColoringHorse( BaseItemObject* Obj , int Color[4] , char* Password , int ItemPos ) = 0;
	virtual void SC_ColoringHorseResult( int SendID , ColoringShopResultENUM Result  );
	//////////////////////////////////////////////////////////////////////////
	//	名字重復
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_RoleNameDuplicateErr( BaseItemObject* Obj , const char* RoleName , int PlayerDBID[2] ) =0;
	//////////////////////////////////////////////////////////////////////////
	//死亡爆出的裝備
	//////////////////////////////////////////////////////////////////////////
	static void SC_PKDeadDropItem( int SendID , ItemFieldStruct& Item );
	//////////////////////////////////////////////////////////////////////////
	//Client 顯示時鐘
	//////////////////////////////////////////////////////////////////////////
	static void SC_ClientClock( int SendID , int BeginTime , int NowTime , int EndTime , int Type );
	//////////////////////////////////////////////////////////////////////////
	// 通知帳號幣到期時間
	//////////////////////////////////////////////////////////////////////////
	static void SC_AccountMoneyLockInfo( int SendID , int LockAccountMoney[Def_AccountMoneyLockTime_Max_Count_ ] , int LockAccount_Forever );
	//////////////////////////////////////////////////////////////////////////
	static void SC_AccountMoneyTranError( int SendID );

	virtual void RC_LookTarget			( BaseItemObject* Obj , int TargetGUID ) = 0;
	virtual void RC_SetCycleMagic		( BaseItemObject* Obj , int MagicID , int MagicPos  ) = 0;
	static void SC_SetCycleMagic		( int SendToID , int MagicID , int MagicPos ); 

	virtual void RC_CloseMaster( BaseItemObject* Obj , const char* Password ) = 0;

	//////////////////////////////////////////////////////////////////////////
	//DB Select 資料
	//////////////////////////////////////////////////////////////////////////
	static void SD_SelectDB				( int PlayerDBID , const char* DataType , const char* SelectCmd );
	//////////////////////////////////////////////////////////////////////////
	//清除某一個職業
	static void SC_DelJob				( int SendID , Voc_ENUM Job );
	//////////////////////////////////////////////////////////////////////////
	//小遊戲介面
	static void SC_SetSmallGameMenuType		( int SendID , int MenuID , SmallGameMenuTypeENUM Type );
	static void SC_SetSmallGameMenuValue	( int SendID , int MenuID , int ID , int Value );
	static void SC_SetSmallGameMenuStr		( int SendID , int MenuID , int ID , const char* Content );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_SetSendPacketMode	( BaseItemObject* Obj , SendPacketModeStruct& Mode ) = 0;
	//////////////////////////////////////////////////////////////////////////
	//重置副本
	virtual void RC_ResetInstanceRequest( BaseItemObject* Obj , int ZoneID ) = 0;
	virtual void RL_ResetInstance( int PlayerDBID , int PartyID ) = 0;

	static void SC_ResetInstanceResult	( int SendID , bool Result );
	static void SL_ResetInstance		( int ZoneID , int PartyID , int PlayerDBID );
	static void SC_ResetInstanceInfo	( int SendDBID , int ZoneID , bool IsReset );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_GmResetRolet		( BaseItemObject* Obj , int RoleBaseSize , int PlayerRoleBaseSize , BaseRoleData& BaseData , PlayerBaseRoleData& PlayerBaseData  ) = 0;
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_PartyDice			( BaseItemObject* Obj , int Type ) = 0;
	static void SC_PartyDice			( int SendDBID , int Type , int Rand , int PlayerDBID );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_BodyShop			( BaseItemObject* Obj , char BoneScale[15] ) = 0;
	static void SC_BodyShopResult		( int SendID , bool Result );

	//////////////////////////////////////////////////////////////////////////
	virtual void RC_MoneyVendorReport	( BaseItemObject* Obj , const char* Name , const char* Content ) = 0;
	virtual void RC_BotReport			( BaseItemObject* Obj , const char* Name , const char* Content ) = 0;

	static void SC_MoneyVendorReport	( int SendID , int Count );
	static void SC_BotReport			( int SendID , int Count );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_UnlockBotReport		( BaseItemObject* Obj ) = 0;
	static void SC_UnlockBotReport		( int SendID , const char* Name , UnlockBotTypeENUM	Type );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_SortBankRequest		( BaseItemObject* Obj ) = 0;
	static void SC_SortBankEnd			( int SendID );

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_LoadClientData		( BaseItemObject* Obj , int KeyID ) = 0;
	virtual void RC_SaveClientData		( BaseItemObject* Obj , int KeyID , int PageID , int Size , const char* Data ) = 0;
	static  void SD_LoadClientData		( int PlayerDBID , int KeyID );
	static  void SC_SaveClientDataResult( int SendID , int KeyID , bool Result );
	//////////////////////////////////////////////////////////////////////////
	//設定地圖Mark
	//////////////////////////////////////////////////////////////////////////
	static void SC_SetMapMark			( int SendID , int MarkID , float X , float Y , float Z , const char* TipStr , int IconID );
	static void SC_ClsMapMark			( int SendID , int MarkID );

	static void SC_AllRoom_SetMapMark	( int RoomID , int MarkID , float X , float Y , float Z , const char* TipStr , int IconID );
	static void SC_AllRoom_ClsMapMark	( int RoomID , int MarkID );

	static void SC_SetRoomValue			( int SendID , int KeyID , int Value );
	static void SC_ClsRoomValue			( int SendID , int KeyID );

	static void SC_AllRoom_SetRoomValue	( int RoomID , int KeyID , int Value );
	static void SC_AllRoom_ClsRoomValue	( int RoomID , int KeyID );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_Suicide				( BaseItemObject* Obj , int Type ) = 0;
	//////////////////////////////////////////////////////////////////////////
	//通知Client 新增移除某個法術
	static void SC_AddExSkill			( int SendID , ExSkillBaseStruct& Info );
	static void SC_DelExSkill			( int SendID , ExSkillBaseStruct& Info );

	virtual void RC_SpellExSkill		( BaseItemObject* Obj , int TargetID , float TargetX , float TargetY , float TargetZ , ExSkillBaseStruct& Info , int ShootAngle , int ExplodeTime ) = 0;
	static void SC_SpellExSkillResult	( int SendID , bool Result , RoleErrorMsgENUM	Msg );

	//////////////////////////////////////////////////////////////////////////
	virtual void RC_ClientEventLog		( BaseItemObject* Obj , int EventType ) = 0;

	//////////////////////////////////////////////////////////////////////////
	static void SC_Range_BaseObjValueChange		( int GItemID , ObjValueChangeENUM Type , const char* ValueStr , int Value  );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_TransferData_Range	( BaseItemObject* Obj , char Data[20] ) = 0;
	static void SC_TransferData_Range	( BaseItemObject* Obj , char Data[20] );
	//////////////////////////////////////////////////////////////////////////
	//Lua 通知client 觸發劇情
	static void SC_LuaClientEvent		( int SendID , int GItemID , int EventType , const char* Content ); 
	static void SC_Range_LuaClientEvent	( int GItemID , int EventType , const char* Content ); 
	//////////////////////////////////////////////////////////////////////////
	//client 端要求改變角色數值
	virtual void RC_SetRoleValue		( BaseItemObject* Obj , RoleValueName_ENUM Type , double Value ) = 0;
	static void SC_SetRoleValueResult	( int SendID , bool Result );
	//////////////////////////////////////////////////////////////////////////
	//通知Client zone 開啟
	static void SC_AllCli_ZoneOpen		( int ZoneID );
	//////////////////////////////////////////////////////////////////////////
	virtual void RD_SysKeyValue			( int TotalCount , int ID , char* KeyStr , int Value ) = 0;
	//////////////////////////////////////////////////////////////////////////
	//client使用的語言
	virtual void RC_ClientLanguage		( BaseItemObject* Obj, int iLanguage ) = 0;
	//////////////////////////////////////////////////////////////////////////
	//在角色上執行某個LuaPlot
	virtual void RD_RoleRunPlot			( const char* LuaPlot, int DBID ) = 0;
	//////////////////////////////////////////////////////////////////////////
	//新的頭銜系統
	//////////////////////////////////////////////////////////////////////////
	virtual void	RC_SetNewTitleSys	( BaseItemObject* Obj , NewTitleSysPosENUM Pos , int TitleID ) = 0;
	static void		SC_SetNewTitleSysResult( int SendID , bool Result );
	virtual void	RC_NewTitleSys_UseItem( BaseItemObject* Obj , int ItemType , int Pos ) = 0;
	static void		SC_NewTitleSys_UseItemResult( int SendID , NewTitleSysUseItemResultENUM Result );
	//////////////////////////////////////////////////////////////////////////
	//換區執行劇情
	static  bool SL_PCall( int zoneID , const char* plotStr  );
	virtual void RL_PCall( const char* plotStr ) = 0;
	//////////////////////////////////////////////////////////////////////////
	//Session
	static void SC_OpenSessionResult(int SendID, SessionTypeENUM Type, int SessionID, int CBID);
	static void SD_OpenSession	(const char* Account, int RoleDBID, SessionTypeENUM Type, int SessionID, int ValidTime);
	static void SD_CloseSession	(const char* Account, int RoleDBID, SessionTypeENUM Type, int SessionID);

	virtual void RC_OpenSession( BaseItemObject* Obj, SessionTypeENUM Type, int CBID ) = 0;
	virtual void RC_CloseSession( BaseItemObject* Obj, SessionTypeENUM Type, int SessionID ) = 0;
	//////////////////////////////////////////////////////////////////////////
	static void SC_FixAccountFlag( RoleDataEx* role );

	virtual void RC_AutoReportPossibleCheater( BaseItemObject* Obj , AutoReportBaseInfoStruct& TargetInfo , AutoReportPossibleCheaterENUM Type ) = 0;

	//////////////////////////////////////////////////////////////////////////
	//今天的複本
	virtual void RD_WeekInstances( int count , WeekZoneStruct info[] ) = 0;
	//要求今天的複本
	virtual void RC_WeekInstancesRequest( BaseItemObject* obj  ) = 0;
		//今天的複本
		static void SC_WeekInstances( int guid , int day , vector<WeekZoneStruct>& zoneGroupIDList );

	//要求傳送到某副本
	virtual void RC_WeekInstancesTeleport( BaseItemObject* obj , int zoneID ) = 0;
		//要求結果
		static void SC_WeekInstancesTeleportResult( int guid , bool result );

	//要求重置某副本
	virtual void RC_WeekInstancesZoneReset( BaseItemObject* obj , int zoneID ) = 0;
		//要求重置某副本
		static  void SL_WeekInstancesZoneReset( int zoneID , int dbid , int partyID  );
			//要求重置某副本
			virtual void RL_WeekInstancesZoneReset( int NetID , int dbid , int partyID ) = 0;

	//重置副本傳送次數
	virtual void RC_WeekInstancesReset( BaseItemObject* obj  ) = 0;
		//重置副本傳送次數結果
		static  void SC_WeekInstancesReset( int guid , bool result );

	//複本剩下的生存時間
	static void SC_WeekInstancesLiveTime( int guid , int sec );
	//開啟鏡世界副本介面
	static void SC_OpenWeekInstancesFrame( int guid );
	//////////////////////////////////////////////////////////////////////////
	//魔靈升階(或產生)
	virtual void RC_PhantomRankUp( BaseItemObject* obj , int phantomID , bool summon ) = 0;
		//魔靈升階結果
		 static void SC_PhantomRankUpResult( int guid , bool summon , bool result );

	//魔靈升級
	virtual void RC_PhantomLevelUp( BaseItemObject* obj , int phantomID ) = 0;
		//魔靈升級結果
		static void SC_PhantomLevelUpResult( int guid , bool result );

	//魔靈裝備
	virtual void RC_PhantomEQ( BaseItemObject* obj , int eQ[] ) = 0;
		//魔靈裝備結果
		static void SC_PhantomEQResult( int guid , bool result );

	//修正魔靈裝備
	static void SC_FixPhantomEQ( int guid , int eQ[] );
	//修正魔靈資料
	static void SC_FixPhantom( int guid , int phantomID , PhantomBase& info );
	//開啟幻靈介面
	static void SC_OpenPhantomFrame( int guid );

	//後台Buff設定
	virtual void RD_BuffSchedule( int buffID , int buffLv , int buffTime ) = 0;

	//後台魔幻寶盒資料
	virtual void RD_ItemCombinInfo( int groupID , ItemCombineStruct& itemCombine ) = 0;
	//後台魔幻寶盒資料
	static void SC_ItemCombinInfo( int guid , int groupID , ItemCombineStruct& itemCombine );
	static void SC_AllItemCombinInfo( int guid );

	//後台魔幻寶盒索引ID
	static void SC_ItemCombinGroupID( int guid , int groupID );
	static void SC_ALL_ItemCombinGroupID( int groupID );

};

