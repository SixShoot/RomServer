#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "NetWaker/GSrvNetWaker.h"
#include <Windows.h>
#include "BaseItemObject/BaseItemObject.h"
#include "GlobalDef.h"
#include "GSrvIni/GSrvIniFile.h"
#include "GMCommand.h"
#include "GlobalBase.h"
#include "PG/NetWakerPGEnum.h"

#include "RoleData/ObjectDataClass.h"
#include "..\ServerDll.h"
#include "Combo\SkillComboClass.h"

#include "./Rune Engine/rune/core/utility/RuUtility.h"
#include "ControllerClient/ControllerClient.h"
#include "LuaLineBlockManage.h"

#ifndef _ENABLE_LOG_CENTER
#define USE_NEW_LOG_CENTER_SERVER 0
#else
#define USE_NEW_LOG_CENTER_SERVER 1
#endif

class Global : public GlobalBase
{
protected:
	//-----------------------------------------------------------------------------------------------------------------------
	struct GlobalStaticStruct
	{
		RecycleBin< PlayerRoleData >		    OnLoginPlayerRecycle;	//角色資源回收物件
		map< string, PlayerRoleData* >		    OnLoginPlayer;		    //登入中角色與帳號對應資料
		vector< MyMapAreaManagement* >          Partition;		        //資料管理分割區
		GSrvIniFileClass                        Ini;
		map< SpaceStruct, RoleData* >	        PosList;			    //物件所在位置的Map(10秒處理一次)		
		GmCommandClass                          GM;
		bool                                    IsDelay;                //是否Server 在Delay狀態
		bool                                    IsDestroy;              //是否要結束程式    
		LocalServerBaseDBInfoStruct		        LocalDBInfo;            //區域資訊        

		map< int, float >			            ObjectAccumulate;       //所有有產量限制的物件的產量
		vector< BaseItemObject* >				SearchPlayerResult;
		bool                                    IsNPCMove;              //是否NPC可移動
		IGameSrvPluginNotify* FromNotify;				//通知from事件,用來處理Form顯像資訊

		bool									IsDisableNPCReviveTime;	//使立刻重生

		vector< PrivateRoomInfoStruct >			PrivateRoomInfoList;

		SkillComboClass							ComboProc;				//處理連攜的物件

		map< int, ServerItemLogStruct >		SrvItemManage;

		vector< CampReviveStruct >				CampReviveList;			//如果有設則依陣營重生
//		CountryTypeENUM							CountryType;			//
		bool									IsDisableLogin;			//不允許登入							
		int										PingLogCount;			//測試Ping的次數
		int										ProxyPingLogCount;		//測試Ping的次數
		int										ServerNetProcLogCount;	//記錄Server  處理時間的Log

		bool									WaitServerClose;
		int										WaitServerCloseCountDown;
		int										WaitServerCloseChangeParalledID;

		bool									IsLoadAllNpcComplete;	//是否Np全部載完
		bool									IsUseBillingServer;		//是否使用Billing server


		map< int, LineBlockManageClass >		LuaLineBlock;			//企劃組檔

		GlobalStaticStruct()
		{
			ProxyPingLogCount = 0;
			ServerNetProcLogCount = 0;
			PingLogCount = 0;
			IsDisableLogin = false;
			IsDelay = false;
			IsDestroy = false;
			IsNPCMove = true;
			IsDisableNPCReviveTime = false;
			FromNotify = NULL;
			WaitServerClose = false;
			IsLoadAllNpcComplete = false;
			IsUseBillingServer = false;
		};
		~GlobalStaticStruct()
		{
			//   Schedular->Del( );
		};
	};
	//-----------------------------------------------------------------------------------------------------------------------
	static GlobalStaticStruct* _St;


	static void     _OnCreateObj(BaseItemObject*, RoleDataEx*);
	static void     _OnDestroyObj(BaseItemObject*, RoleDataEx*);

	//定時處理
	static void     _OnTimeProc();

	static void     _Sys_OnTimeProc();
	//單一物件路徑定時處理
	static void     _OnTimePathProcess(BaseItemObject*, RoleDataEx*);

	static int      _CheckChangeZoneNPC(SchedularInfo* Obj, void* InputClass);    //檢查是否正常登入
	static int      _ChangeZoneDelay(SchedularInfo* Obj, void* InputClass);    //延後換區
	static int      _ChangeWorldDelay(SchedularInfo* Obj, void* InputClass);    //延後換區

	static void		_TpExpCalculate(RoleDataEx* Owner, float TpExp, int Exp);

	//法寶處理
	static void		_MagicToolsProc(RoleDataEx* Owner);

	static void		_ProxyPingLog(int ProxyID, int Time);
public:

	static int      _CheckLoginRoleData(SchedularInfo* Obj, void* InputClass);   //檢查是否正常登入	
	static int		_CheckDailyQuestReset(SchedularInfo* Obj, void* InputClass);   //檢查每日任務重置時間
	static int		_CheckZoneShutdown(SchedularInfo* Obj, void* InputClass);   //停機事件
	static int		_CheckTreasureLootRight(SchedularInfo* Obj, void* InputClass);

	static int		_ChangeParallel_CloseZone_Proc(SchedularInfo* Obj, void* InputClass);

	static GlobalStaticStruct* St() { return _St; }
	static GSrvIniFileClass* Ini() { return &_St->Ini; }
	static bool     Init(char* IniFile);
	static bool     Release();
	static bool     Process();
	static void     Destory() { _St->IsDestroy = true; };
	static void     InitRoleDataEx();
	static void     InitNet();
	static void     InitBattleGround();
	static void     ReleaseBattleGround();
	static void     InitBaseItemObject();
	static bool     InitLUA();
	static bool InitLUA_LoadFile();
	static bool InitLUA_Init();
	static vector<BaseSortStruct>* SearchRangeObject(RoleDataEx* Owner, float X, float Y, float Z, SearchRangeTypeENUM Type, int Range);
	static vector<RoleDataEx*>* SearchRangeChildObject(RoleDataEx* Owner, int GroupID);
	//----------------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------------
	static void     CliLogout(int SockID, int SrvType, int SrvID, const char* szAccount, const char* szReason) { _Net->CliLogout(SockID, SrvType, SrvID, szAccount, szReason); };
	static void		CliDisconnect(int SockID) { _Net->CliDisconnect(SockID); };
	//----------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------------
	static bool         AddOnLoginPlayer(PlayerRoleData* PlayerData);
	static bool         DelOnLoginPlayer(string AccountName);
	static PlayerRoleData* GetOnLoginPlayer(string AccountName);
	//----------------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------------

	static	BaseItemObject* GetObj(int ID);               //取得某個ID的物件資料
	static 	BaseItemObject* GetObjBySockID(int SockID);   //取得某個SockID的物件資料
	static 	BaseItemObject* GetObjByName(char* Name);		//取得某個物件
	static 	RoleDataEx* GetRoleDataByGUID(int ID);
	static 	RoleDataEx* GetRoleDataByDBID(int DBID);
	static  BaseItemObject* GetNPCObj_ByDBID(int NPCDBID);
	static  BaseItemObject* GetPlayerObj_ByDBID(int PlayerDBID);
	static	int				GetOrgObjID(int ID);

	static  bool SendToCli_ByDBID(int DBID, int Size, void* Data);
	static  bool SendToCli_ByGUID(int GUID, int Size, void* Data);
	static  bool SendToCli_ByRoleName(const char* RoleName, int Size, void* Data);

	static  void SendToOtherWorld_GSrvID(int WorldID, int GSrvID, int Size, void* Data);
	static  void SendToOtherWorld_NetID(int WorldID, int NetID, int Size, void* Data);
	static  void SendToOtherWorld_ServerType(int WorldID, EM_SERVER_TYPE iServerType, int Size, void* Data);

	static	void SendToOtherWorldSrvClient_ByDBID(int iWorldID, int iSrvID, int iClientDBID, int iSize, void* pData);
	static	void SendToOtherWorldZoneClient_ByDBID(int iWorldID, int iZoneID, int iClientDBID, int iSize, void* pData);
	//----------------------------------------------------------------------------------------
	//  取值
	//----------------------------------------------------------------------------------------
	static  int ViewBlockSize();
	//----------------------------------------------------------------------------------------
	//	物品管理
	//----------------------------------------------------------------------------------------
	//物品序號產生
	static int	GenerateItemVersion();

	//檢查某物件可以生產多少個
	static int	GetItemMaxGenerate(int OrgObjID);

	//產生某物件 多少個( 控制物件產生量 )
	static bool ItemGenerate(int OrgObjID, int Count);

	//初使化Item 產生數量
	static void InitGenerate();

	//出始化新產生出來的物品(時間 序號)  
	static bool	NewItemInit(ItemFieldStruct& Item, int OrgObjID, int NPCObjID);

	//----------------------------------------------------------------------------------------
	//	搜尋玩家週圍
	//----------------------------------------------------------------------------------------
	static void				ResetRange(RoleDataEx* Owner, int Range);
	static RoleDataEx* GetRangeRole();
	static BaseItemObject* GetRangeObj();


	static RoleDataEx* GetRangePlayer();
	static BaseItemObject* GetRangePlayerObj();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//  分割區處理(只處理不送封包)
	//----------------------------------------------------------------------------------------
	static  PlayIDInfo** PartCompSearch(RoleDataEx*, int NewSecX, int NewSecZ, int BlockCount);
	static  MyMapAreaManagement* Partition(RoleDataEx* Owner);
	static  MyMapAreaManagement* Partition(int RoomID);
	static  bool                    PartMove(RoleDataEx*, int NewSecX, int NewSecZ);
	static  bool                    PartAdd(RoleDataEx*);
	static  bool                    PartDel(RoleDataEx*);
	static  PlayIDInfo** PartSearch(RoleDataEx*, int BlockCount);
	//----------------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------------
	static int CalSecX(float X);
	static int CalSecZ(float Z);

	static bool IsMirror(int GUID);
	//----------------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------------
	static int		CreateChangeZoneNPC(RoleDataEx* Role, int CheckCount = 0);
	static int      CreateObj(int OrgObjID, float x, float y, float z, float Dir, int Count, int WorldGUID = -1);
	static int      CreateObj_Macro(RoleDataEx* Owner
		, int OrgObjID, const char* Name
		, float x, float y, float z, float dir, int count
		, const char* AutoPlot, const char* ClassName
		, int QuestID, int Mode, int PID, int DBID = -1, float vX = 0, float vY = 0, float vZ = 0, int RoomID = -1);


	static bool		CheckClientLoading_AddToPartition(int ItemID, int RoomID);
	static bool     AddToPartition(int ItemID, int RoomID);
	static bool     DelFromPartition(int ItemID);
	static bool		CheckClientLoading_SetPos(int ItemID, float X, float Y, float Z, float Dir);

	static GameObjDbStructEx* GetObjDB(int OrgID);
	static long     GenerateItemVersion_ByObjDB(GameObjDbStructEx*);
	//----------------------------------------------------------------------------------------
	//產生寵物
	//----------------------------------------------------------------------------------------
	//PetType = 0 一般 1 養殖
	static int		CreatePet(int OwnerID, int OrgObjID, char* Name, int LV, SummonPetTypeENUM PetType, int MagicBaseID = -1);
	static int		CreateGuard(int OwnerID, int OrgObjID, char* Name, int LV, bool CanFight, int GroupID, int MagicBaseID = -1);
	//----------------------------------------------------------------------------------------
	//送資除錯資訊給玩家
	static void     SendLuaErrorInfo(int OwnerID, int TargetID, const char* FunName, const char* Msg);
	//----------------------------------------------------------------------------------------
	//載入物件模板資料
	static bool     LoadObjDB();
	//給條件函式資料搜尋
	static BaseItemObject* Search(ComObjFunction  Func, void* Data);
	static BaseItemObject* SearchNpcDBID(int NpcDBID);
	//----------------------------------------------------------------------------------------
	//路徑處理
	//----------------------------------------------------------------------------------------
	static void	    SearchAttackNode(RoleDataEx* Owner, RoleDataEx* Target, float& NX, float& NY, float& NZ);
	static void	    SearchAttackNode(RoleDataEx* Owner, float Tx, float Ty, float Tz, float TBoundRadius, float& NX, float& NY, float& NZ);
	//陣退位置
	static void	    SearchStrikeBackNode(RoleDataEx* Owner, float Tx, float Ty, float Tz, float Range, float& NX, float& NZ);

	static void     SearchNode(RoleDataEx* Owner, float& NX, float& NZ);
	//----------------------------------------------------------------------------------------
	//物品
	//----------------------------------------------------------------------------------------
	static bool     UseItem(int OwnerID, int TargetID, float TargetX, float TargetY, float TargetZ, int Pos, int Type, ItemFieldStruct& Item);
	static bool		AutoUseItem(RoleDataEx* Owner, GameObjDbStructEx* ItemDB);
	//----------------------------------------------------------------------------------------
	//戰鬥
	//----------------------------------------------------------------------------------------

	static RoleDataEx* SearchEnemy(BaseItemObject* OwnerObj);
	//尋找周圍是否有物件在戰鬥
	static RoleDataEx* SearchAttackModeObj(BaseItemObject* OwnerObj, int Range);
	//    static bool             SearchHelp( BaseItemObject* OwnerObj , int Race );
	static void             AddHP(RoleDataEx* underAtk, RoleDataEx* Atk, float DHP, float HateRate = 1);
	static void             AddMP(RoleDataEx* underAtk, RoleDataEx* Atk, float DMP, float HateRate = 1);
	static void             AddSP(RoleDataEx* underAtk, RoleDataEx* Atk, float DSP, float HateRate = 1);
	static void             AddSP_Sub(RoleDataEx* underAtk, RoleDataEx* Atk, float DSP, float HateRate = 1);
	static void             AddStomachPoint(RoleDataEx* Role, int DStomachPoint);

	static bool             ProcessDead(RoleDataEx* pDeader, RoleDataEx* pKiller);
	static void             ProcessDead_Honor(RoleDataEx* pDeader, RoleDataEx* pKiller);

	static bool             PlayerDead(RoleDataEx* Dead, RoleDataEx* Kill);
	static bool             NPCDead(RoleDataEx* Dead, RoleDataEx* Kill);
	static bool             DeadDropItem(int Dead, int KillID);
	static void				DropItem(RoleDataEx* Dead, int DLv, int ItemID, bool IsCreateLightPower, ItemFieldStruct* OutItem);
	static bool				CreateTreasure(int OwnerDBID, int OwnerPartyID, RoleDataEx* Deader, vector<ItemFieldStruct>& DropItemList, vector<int>& OwnerDBIDList, vector<bool>& LockPlayer);
	static bool				CreateTeleport(float CreateX, float CreateY, float CreateZ, int ToZone, float ToX, float ToY, float ToZ, const char* Name, int Pid = 0);

	static void             FixAttackMode(RoleDataEx* Owner);
	static void             FightAddexp(RoleDataEx* Dead, RoleDataEx* Kill);
	static RoleDataEx* SearchMaxHit(RoleDataEx* Dead);
	static bool				FindNewHateTarget(RoleDataEx* Owner);
	static bool				FindNewHateTarget(RoleDataEx* Owner, float Range);

	static bool				AddRelationExp(RoleDataEx* Owner, RoleDataEx* Target);

	//搜尋寵物的施法目標
//	static RoleDataEx*		SearchPetMagicTarget( RoleDataEx* Owner , RoleDataEx* Pet , PetSkillStruct* Skill );

	static int				RoomPlayerCount(int RoomID);

	static int				GetPCenterID(const char* account);

	//*****************************************************************************************
	//放入 SimpleSchedular 排程的函式
	//*****************************************************************************************
	static void     PlayerRegProc(BaseItemObject*);
	static void     NPCRegProc(BaseItemObject*);
	static void     QuestObjRegProc(BaseItemObject*);
	static void     ItemRegProc(BaseItemObject*);
	static void		MineRegProc(BaseItemObject* OwnerClass);
	static void     RemoveAllProc(BaseItemObject*);

	//    static int      SearchEnemyProc ( BaseItemObject* );
	static INT64      MoveProc(BaseItemObject*);
	static int      MoveAIProc(BaseItemObject*);
	static INT64      AIProc(BaseItemObject*);
	static INT64      AttackProc(BaseItemObject*);
	//static int      SpellMagicProc  ( BaseItemObject* );
	static INT64      NPCMagicProc(BaseItemObject*);
	static INT64      PlayerMagicProc(BaseItemObject*);
	static INT64      PetProc(BaseItemObject*);
	static INT64      MineGenerateProc(BaseItemObject*);

	static int		ResetObjProc(SchedularInfo* Obj, void* InputClass);
	//    static int      EscapeProc      ( BaseItemObject* );
		//*****************************************************************************************
		//所有玩家，物品，顯像物件都會定時觸發
	static	void	_Obj_OnTimeProc(BaseItemObject*, RoleDataEx*);
	//只有玩家定時會觸發
	static	void	_Player_OnTimeProc(BaseItemObject*, RoleDataEx*);
	//只有NPC定時會觸發
	static	void	_NPC_OnTimeProc(BaseItemObject*, RoleDataEx*);
	//----------------------------------------------------------------------------------------
	//旗子處理
	static  bool    CreateFlag(int FlagOrgObjID, int FlagID, float x, float y, float z, float dir, int DBID = -1, bool IsSave = false);
	static  bool    CreateFlag_ShowFlag(int FlagOrgObjID, int FlagID);
	static  void    ShowFlag(int FlagID);
	static  void    HideFlag(int FlagID);
	//----------------------------------------------------------------------------------------
	//玩家系統自動復活
	static void	    PlayerResuretion(BaseItemObject* OwnerObj, int ZoneID, float X, float Y, float Z, float ExpDecRate, bool IsNoBuff = false);
	static void     ChangeZone(int GItemID, int ZoneID, int RoomID = 0, float X = 0, float Y = 0, float Z = 0, float Dir = 0);
	static void		SafeChangeZone(int GItemID, int ZoneID, int RoomID, float X, float Y, float Z, float Dir);
	static void     ChangeZoneDirect(int GItemID, int ZoneID, int RoomID, float X, float Y, float Z, float Dir);
	static bool		ChangeParalledID(int GItemID, int ParalledID);

	static void     ChangeWorld(int GItemID, RoleDataEx* pRole, int iReturnZoneID, RolePosStruct Pos, int WorldID, int ZoneID, int RoomID = 0, float X = 0, float Y = 0, float Z = 0, float Dir = 0);
	static void     ChangeWorldDirect(int GItemID, int WorldID, int ZoneID, int RoomID, float X, float Y, float Z, float Dir);

	static void		ChangeParallel_CloseZone(int ParalledID);
	//----------------------------------------------------------------------------------------
	//其它
	static void     PartyExpCal(RoleDataEx* Owner, int DeadLv, float Exp, float TpExp);
	//系統訊息輸出處理
	static void		SysPrintOutput(int LV, const char* Title, const char* Context);
	//重置物件
	static void		ResetObj(int iObjGUID);
	//送訊息給GM Tools
	static void		SendMsgToGMTools(int LV, const char* Title, const char* Context);
	//----------------------------------------------------------------------------------------
	//log儲存
	static void     Log_LocalServerProcTime(int MinTime, int MaxTime, int DelayTime, int ZoneID, int WorldID);
	static void		Log_GMToolsGMCommand(const char* Account, int ManageLv, int ToRoleDBID, const char* Command);
	static void		Log_GMCommand(RoleDataEx* Owner, const char* GMCommand, const char* RoleName);
	static void		Log_ItemTrade(int FromPlayerDBID, int FromNpcDBID, int ToPlayerDBID, int X, int Z, ActionTypeENUM Type, ItemFieldStruct& Item, const char* Note);
	static void		Log_ItemDestroy(RoleDataEx* Role, ActionTypeENUM Type, ItemFieldStruct& Item, int ItemCount, int NPCDBID, const char* Note);
	static void		Log_Money(int FromPlayerDBID, int FromNpcDBID, RoleDataEx* ToRole, ActionTypeENUM Type, int Money);
	static void		Log_Money_Account(int FromPlayerDBID, int FromNpcDBID, RoleDataEx* ToRole, ActionTypeENUM Type, int Money);
	static void		Log_Money_Bonus(int FromPlayerDBID, int FromNpcDBID, RoleDataEx* ToRole, ActionTypeENUM Type, int Money);
	static void		Log_MoneyEx(int FromPlayerDBID, int FromNpcDBID, RoleDataEx* ToRole, ActionTypeENUM Type, int MoneyType, int Money, int OldMoney);

	static void		Log_PlayerActionLog(RoleDataEx* Role);
	static void		Log_Level(RoleDataEx* Role);
	static void		Log_Dead(RoleDataEx* Deader, RoleDataEx* Killer);
	static void		Log_Quest(RoleDataEx* pRole, int iQuestID, int iQuestStatus);
	static void		Log_ItemServerLog();
	static void		Log_EnterZone(RoleDataEx* Role);
	static void		Log_LeaveZone(RoleDataEx* Role);
	static void		Log_AddExp(RoleDataEx* Role, int OldExp);
	static void		Log_ServerStatus();
	static void		Save_ZoneInfo();
	static void		Log_DepartmentStore(ItemFieldStruct& Item, ActionTypeENUM Type, int Count, int Cost, int Cost_Bonus, int Cost_Free, int PlayerDBID);
	static void		Log_Design(int Type, char* Content, int PlayerDBID, int GuildID);
	static void		Log_SkillLv(RoleDataEx* Owner, int SkillType, int SkillValue);
	static void		Log_ClientComputerInfo(RoleDataEx* Owner, ClientComputerInfoStruct& Info);
	static void		Log_Talk(TalkLogTypeENUM Type, int ChannelID, int FromPlayerDBID, int ToPlayerDBID, const char* Content);
	static void		Log_ZoneMoneyLog();
	static void		Log_Warning_MoneyVendor(RoleDataEx* VendorRole, int PlayerDBID, const char* Content);
	static void		Log_Warning_Bot(RoleDataEx* VendorRole, int PlayerDBID, const char* Content);
	static void		Log_Warning_PossibleCheater(RoleDataEx* reportRole, AutoReportBaseInfoStruct& TargetInfo, AutoReportPossibleCheaterENUM Type);
	static void		Log_ExchangeItem(const char* Serial, RoleDataEx* Role, int ItemID, int Count, int Money, int AccountMoney);
	static void		Log_PlayerMoveErr(RoleDataEx* role, PlayerMoveErrTypeENUM type);
	//----------------------------------------------------------------------------------------
	static void		Log_OfflineMsg(const char* ToPlayerName, const char* FromName, int FromDBID, const char* Msg);
	static void		Log_Verion();
	//----------------------------------------------------------------------------------------
	//static void		Log_House			( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType);
	static void		Log_House(RoleDataEx* Player, int HouseDBID, int HouseOwnerDBID, int ActionType, const char* Msg);
	//static void		Log_House			( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number1 , int Number2 , int Number3 , int Number4 );
	//static void		Log_House			( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number1 , int Number2 );
	//static void		Log_House			( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number1 , int Number2 , float Float1 , float Float2 , float Float3 , float Float4 );
	//static void		Log_House			( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number );
	//static void		Log_House			( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , const char* Msg1 , const char* Msg2 , const char* Msg3 );
	//static void		Log_House			( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number1 , int Number2 , int Number3 , int Number4 , int Number5 , int Number6 );
	//----------------------------------------------------------------------------------------

	static int		Rand(int iMin, int iMax);

	//物品加符文
	static bool		AddRune(ItemFieldStruct* UseItem, GameObjDbStructEx* UseItemOrgDB, ItemFieldStruct* TargetItem, GameObjDbStructEx* TargetItemOrgDB);
	//-------------------------------------------------------------------------------------------
	//獨立區域處理
	//-------------------------------------------------------------------------------------------
	//拷貝第0個房間的怪物
	static bool		CopyRoomMonster(int FromRoomID, int ToRoomID);
	//刪除某房間的怪物( 第0個房間不可刪除)
	static bool		DelRoomMonster(int RoomID);
	//物件考貝
	static int		CreateRoomObj(int RoomID, RoleDataEx* Owner);
	//定時檢查獨立區域是否要解構
	static void		CheckPrivateZone();
	//-------------------------------------------------------------------------------------------
	//設定標準裝備
	static void		SetStandardWearEq(RoleDataEx* Owner, int Level, int Rank);
	//-------------------------------------------------------------------------------------------
	//其他
	//-------------------------------------------------------------------------------------------
	//清除角色路徑
	static void		ClearPath(int GItemID);
	static void		SetPos(int GItemID);

	static const char* GetRoleName(RoleDataEx* Owner);
	static const string	GetRoleName_ASCII(RoleDataEx* Owner);

	//檢查並清除複製
	static void 	CheckAndClearCopyItem(RoleDataEx* Owner);

	//等級差
	static float	DLvRate(int DLv, bool IsPlayer);

	//善惡值
	static void		GoodEvilProc(RoleDataEx* Owner, float Value);
	//玩家死亡 PK 善惡值 計算 
	static void		PlayerDead_CalGoodEvil(RoleDataEx* Deader, RoleDataEx* Killer);
	//-------------------------------------------------------------------------------------------
	//關係處理
	static void		PartyBuffProc(RoleDataEx* Owner);
	//-------------------------------------------------------------------------------------------
	static SysKeyValueStruct* SysKeyValue();


	static void		TimeFlagProc(RoleDataEx* Owner);
	//-------------------------------------------------------------------------------------------
	//程式Crash時的處理
	//-------------------------------------------------------------------------------------------
	static void SaveAllPlayer_byCrash();
	//static DWORD WINAPI CrashProcess( void* );		

	static void OnCrashProcessCheck();
	static void OnCrashProcessExit();

	static void ClientPacketCrashOutput(int id, int size, PacketBase* data);

	static	int				m_iShutdownTimes;
	static	CRuMTRandom		m_ruRand;
	static	int				m_iSoundID;

	//-------------------------------------------------------------------------------------------
	//	重設排程的順序
	//-------------------------------------------------------------------------------------------
	static void ResetAttackSchedular(RoleDataEx* Owner);

	//-------------------------------------------------------------------------------------------
	//	墓碑處理
	//-------------------------------------------------------------------------------------------
	//查尋墓碑如果不見則重建
	static void	TombProcess(RoleDataEx* Owner);
	//產生墓碑(回傳 ItemGUID)
	static int CreateTomb(RoleDataEx* Owner, float X, float Y, float Z);
	//////////////////////////////////////////////////////////////////////////
	static void GoodEvilAtkInfoProc(RoleDataEx* AtkRole, RoleDataEx* DefRole, int DHP);
	//////////////////////////////////////////////////////////////////////////
	//寵物定時處理
	static void ProcPetEvent(RoleDataEx* Owner);
	//////////////////////////////////////////////////////////////////////////
	//送範圍封包
	//////////////////////////////////////////////////////////////////////////
	static void SendToCli_ByRoomID(int RoomID, int Size, void* Data);
	static void SendToCli_Range(RoleDataEx* Owner, int BlockSize, int Size, void* Data);
	//////////////////////////////////////////////////////////////////////////
	//地形影響處理
	static void ZoneDamageProc(RoleDataEx* Owner);
	//////////////////////////////////////////////////////////////////////////
	static bool CheckLuaScript(int OwnerID, int TargetID, const char* LuaScript);


	//讀取Lua的陣列
	static void	ReadLuaArray(int OwnerID, const char* LuaScript, int Size, int RetArray[]);
	//Pcall CheckFunction
	static bool LuaCheckFunction(int OwnerID, int TargetID, const char* LuaScript);

	//取得空的房間
//	static PrivateRoomInfoStruct*	GetEmptyRoomID();
	//////////////////////////////////////////////////////////////////////////
	//增加金錢(所有類型)
	static bool	AddMoneyBase(RoleDataEx* Owner, ActionTypeENUM ActionType, PriceTypeENUM Type, int Money);
	//檢查金錢(所有類型)
	static bool	CheckMoneyBase(RoleDataEx* Owner, PriceTypeENUM Type, int Money);

	//註冊排行榜資料
//	static void	BillboardRegister( int PlayerDBID , int SortType , int SortValue );
	//記憶體檢查
	static void RecycleBinMemCheck();
	//////////////////////////////////////////////////////////////////////////
	static void FixAllAttackMode();

	//////////////////////////////////////////////////////////////////////////
	//定時印出Lua記憶體使用量
	static int  PrintLuaMemoryUsage(SchedularInfo* Obj, void* InputClass);

	//帳號幣是否可以進行加減操作
	static bool AccountMoneyOperable();
	//角色登入資料初始化
	static void LoginInitProc(RoleDataEx* Owner);


};
#endif //__GLOBAL_H__