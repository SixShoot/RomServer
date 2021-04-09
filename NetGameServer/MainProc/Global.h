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
		RecycleBin< PlayerRoleData >		    OnLoginPlayerRecycle;	//����귽�^������
		map< string, PlayerRoleData* >		    OnLoginPlayer;		    //�n�J������P�b���������
		vector< MyMapAreaManagement* >          Partition;		        //��ƺ޲z���ΰ�
		GSrvIniFileClass                        Ini;
		map< SpaceStruct, RoleData* >	        PosList;			    //����Ҧb��m��Map(10��B�z�@��)		
		GmCommandClass                          GM;
		bool                                    IsDelay;                //�O�_Server �bDelay���A
		bool                                    IsDestroy;              //�O�_�n�����{��    
		LocalServerBaseDBInfoStruct		        LocalDBInfo;            //�ϰ��T        

		map< int, float >			            ObjectAccumulate;       //�Ҧ������q������󪺲��q
		vector< BaseItemObject* >				SearchPlayerResult;
		bool                                    IsNPCMove;              //�O�_NPC�i����
		IGameSrvPluginNotify* FromNotify;				//�q��from�ƥ�,�ΨӳB�zForm�㹳��T

		bool									IsDisableNPCReviveTime;	//�ϥߨ譫��

		vector< PrivateRoomInfoStruct >			PrivateRoomInfoList;

		SkillComboClass							ComboProc;				//�B�z�s�⪺����

		map< int, ServerItemLogStruct >		SrvItemManage;

		vector< CampReviveStruct >				CampReviveList;			//�p�G���]�h�̰}�筫��
//		CountryTypeENUM							CountryType;			//
		bool									IsDisableLogin;			//�����\�n�J							
		int										PingLogCount;			//����Ping������
		int										ProxyPingLogCount;		//����Ping������
		int										ServerNetProcLogCount;	//�O��Server  �B�z�ɶ���Log

		bool									WaitServerClose;
		int										WaitServerCloseCountDown;
		int										WaitServerCloseChangeParalledID;

		bool									IsLoadAllNpcComplete;	//�O�_Np��������
		bool									IsUseBillingServer;		//�O�_�ϥ�Billing server


		map< int, LineBlockManageClass >		LuaLineBlock;			//��������

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

	//�w�ɳB�z
	static void     _OnTimeProc();

	static void     _Sys_OnTimeProc();
	//��@������|�w�ɳB�z
	static void     _OnTimePathProcess(BaseItemObject*, RoleDataEx*);

	static int      _CheckChangeZoneNPC(SchedularInfo* Obj, void* InputClass);    //�ˬd�O�_���`�n�J
	static int      _ChangeZoneDelay(SchedularInfo* Obj, void* InputClass);    //���ᴫ��
	static int      _ChangeWorldDelay(SchedularInfo* Obj, void* InputClass);    //���ᴫ��

	static void		_TpExpCalculate(RoleDataEx* Owner, float TpExp, int Exp);

	//�k�_�B�z
	static void		_MagicToolsProc(RoleDataEx* Owner);

	static void		_ProxyPingLog(int ProxyID, int Time);
public:

	static int      _CheckLoginRoleData(SchedularInfo* Obj, void* InputClass);   //�ˬd�O�_���`�n�J	
	static int		_CheckDailyQuestReset(SchedularInfo* Obj, void* InputClass);   //�ˬd�C����ȭ��m�ɶ�
	static int		_CheckZoneShutdown(SchedularInfo* Obj, void* InputClass);   //�����ƥ�
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

	static	BaseItemObject* GetObj(int ID);               //���o�Y��ID��������
	static 	BaseItemObject* GetObjBySockID(int SockID);   //���o�Y��SockID��������
	static 	BaseItemObject* GetObjByName(char* Name);		//���o�Y�Ӫ���
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
	//  ����
	//----------------------------------------------------------------------------------------
	static  int ViewBlockSize();
	//----------------------------------------------------------------------------------------
	//	���~�޲z
	//----------------------------------------------------------------------------------------
	//���~�Ǹ�����
	static int	GenerateItemVersion();

	//�ˬd�Y����i�H�Ͳ��h�֭�
	static int	GetItemMaxGenerate(int OrgObjID);

	//���ͬY���� �h�֭�( ����󲣥Ͷq )
	static bool ItemGenerate(int OrgObjID, int Count);

	//��Ϥ�Item ���ͼƶq
	static void InitGenerate();

	//�X�l�Ʒs���ͥX�Ӫ����~(�ɶ� �Ǹ�)  
	static bool	NewItemInit(ItemFieldStruct& Item, int OrgObjID, int NPCObjID);

	//----------------------------------------------------------------------------------------
	//	�j�M���a�g��
	//----------------------------------------------------------------------------------------
	static void				ResetRange(RoleDataEx* Owner, int Range);
	static RoleDataEx* GetRangeRole();
	static BaseItemObject* GetRangeObj();


	static RoleDataEx* GetRangePlayer();
	static BaseItemObject* GetRangePlayerObj();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//  ���ΰϳB�z(�u�B�z���e�ʥ])
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
	//�����d��
	//----------------------------------------------------------------------------------------
	//PetType = 0 �@�� 1 �i��
	static int		CreatePet(int OwnerID, int OrgObjID, char* Name, int LV, SummonPetTypeENUM PetType, int MagicBaseID = -1);
	static int		CreateGuard(int OwnerID, int OrgObjID, char* Name, int LV, bool CanFight, int GroupID, int MagicBaseID = -1);
	//----------------------------------------------------------------------------------------
	//�e�갣����T�����a
	static void     SendLuaErrorInfo(int OwnerID, int TargetID, const char* FunName, const char* Msg);
	//----------------------------------------------------------------------------------------
	//���J����ҪO���
	static bool     LoadObjDB();
	//������禡��Ʒj�M
	static BaseItemObject* Search(ComObjFunction  Func, void* Data);
	static BaseItemObject* SearchNpcDBID(int NpcDBID);
	//----------------------------------------------------------------------------------------
	//���|�B�z
	//----------------------------------------------------------------------------------------
	static void	    SearchAttackNode(RoleDataEx* Owner, RoleDataEx* Target, float& NX, float& NY, float& NZ);
	static void	    SearchAttackNode(RoleDataEx* Owner, float Tx, float Ty, float Tz, float TBoundRadius, float& NX, float& NY, float& NZ);
	//�}�h��m
	static void	    SearchStrikeBackNode(RoleDataEx* Owner, float Tx, float Ty, float Tz, float Range, float& NX, float& NZ);

	static void     SearchNode(RoleDataEx* Owner, float& NX, float& NZ);
	//----------------------------------------------------------------------------------------
	//���~
	//----------------------------------------------------------------------------------------
	static bool     UseItem(int OwnerID, int TargetID, float TargetX, float TargetY, float TargetZ, int Pos, int Type, ItemFieldStruct& Item);
	static bool		AutoUseItem(RoleDataEx* Owner, GameObjDbStructEx* ItemDB);
	//----------------------------------------------------------------------------------------
	//�԰�
	//----------------------------------------------------------------------------------------

	static RoleDataEx* SearchEnemy(BaseItemObject* OwnerObj);
	//�M��P��O�_������b�԰�
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

	//�j�M�d�����I�k�ؼ�
//	static RoleDataEx*		SearchPetMagicTarget( RoleDataEx* Owner , RoleDataEx* Pet , PetSkillStruct* Skill );

	static int				RoomPlayerCount(int RoomID);

	static int				GetPCenterID(const char* account);

	//*****************************************************************************************
	//��J SimpleSchedular �Ƶ{���禡
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
		//�Ҧ����a�A���~�A�㹳���󳣷|�w��Ĳ�o
	static	void	_Obj_OnTimeProc(BaseItemObject*, RoleDataEx*);
	//�u�����a�w�ɷ|Ĳ�o
	static	void	_Player_OnTimeProc(BaseItemObject*, RoleDataEx*);
	//�u��NPC�w�ɷ|Ĳ�o
	static	void	_NPC_OnTimeProc(BaseItemObject*, RoleDataEx*);
	//----------------------------------------------------------------------------------------
	//�X�l�B�z
	static  bool    CreateFlag(int FlagOrgObjID, int FlagID, float x, float y, float z, float dir, int DBID = -1, bool IsSave = false);
	static  bool    CreateFlag_ShowFlag(int FlagOrgObjID, int FlagID);
	static  void    ShowFlag(int FlagID);
	static  void    HideFlag(int FlagID);
	//----------------------------------------------------------------------------------------
	//���a�t�Φ۰ʴ_��
	static void	    PlayerResuretion(BaseItemObject* OwnerObj, int ZoneID, float X, float Y, float Z, float ExpDecRate, bool IsNoBuff = false);
	static void     ChangeZone(int GItemID, int ZoneID, int RoomID = 0, float X = 0, float Y = 0, float Z = 0, float Dir = 0);
	static void		SafeChangeZone(int GItemID, int ZoneID, int RoomID, float X, float Y, float Z, float Dir);
	static void     ChangeZoneDirect(int GItemID, int ZoneID, int RoomID, float X, float Y, float Z, float Dir);
	static bool		ChangeParalledID(int GItemID, int ParalledID);

	static void     ChangeWorld(int GItemID, RoleDataEx* pRole, int iReturnZoneID, RolePosStruct Pos, int WorldID, int ZoneID, int RoomID = 0, float X = 0, float Y = 0, float Z = 0, float Dir = 0);
	static void     ChangeWorldDirect(int GItemID, int WorldID, int ZoneID, int RoomID, float X, float Y, float Z, float Dir);

	static void		ChangeParallel_CloseZone(int ParalledID);
	//----------------------------------------------------------------------------------------
	//�䥦
	static void     PartyExpCal(RoleDataEx* Owner, int DeadLv, float Exp, float TpExp);
	//�t�ΰT����X�B�z
	static void		SysPrintOutput(int LV, const char* Title, const char* Context);
	//���m����
	static void		ResetObj(int iObjGUID);
	//�e�T����GM Tools
	static void		SendMsgToGMTools(int LV, const char* Title, const char* Context);
	//----------------------------------------------------------------------------------------
	//log�x�s
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

	//���~�[�Ť�
	static bool		AddRune(ItemFieldStruct* UseItem, GameObjDbStructEx* UseItemOrgDB, ItemFieldStruct* TargetItem, GameObjDbStructEx* TargetItemOrgDB);
	//-------------------------------------------------------------------------------------------
	//�W�߰ϰ�B�z
	//-------------------------------------------------------------------------------------------
	//������0�өж����Ǫ�
	static bool		CopyRoomMonster(int FromRoomID, int ToRoomID);
	//�R���Y�ж����Ǫ�( ��0�өж����i�R��)
	static bool		DelRoomMonster(int RoomID);
	//����Ҩ�
	static int		CreateRoomObj(int RoomID, RoleDataEx* Owner);
	//�w���ˬd�W�߰ϰ�O�_�n�Ѻc
	static void		CheckPrivateZone();
	//-------------------------------------------------------------------------------------------
	//�]�w�зǸ˳�
	static void		SetStandardWearEq(RoleDataEx* Owner, int Level, int Rank);
	//-------------------------------------------------------------------------------------------
	//��L
	//-------------------------------------------------------------------------------------------
	//�M��������|
	static void		ClearPath(int GItemID);
	static void		SetPos(int GItemID);

	static const char* GetRoleName(RoleDataEx* Owner);
	static const string	GetRoleName_ASCII(RoleDataEx* Owner);

	//�ˬd�òM���ƻs
	static void 	CheckAndClearCopyItem(RoleDataEx* Owner);

	//���Ůt
	static float	DLvRate(int DLv, bool IsPlayer);

	//���c��
	static void		GoodEvilProc(RoleDataEx* Owner, float Value);
	//���a���` PK ���c�� �p�� 
	static void		PlayerDead_CalGoodEvil(RoleDataEx* Deader, RoleDataEx* Killer);
	//-------------------------------------------------------------------------------------------
	//���Y�B�z
	static void		PartyBuffProc(RoleDataEx* Owner);
	//-------------------------------------------------------------------------------------------
	static SysKeyValueStruct* SysKeyValue();


	static void		TimeFlagProc(RoleDataEx* Owner);
	//-------------------------------------------------------------------------------------------
	//�{��Crash�ɪ��B�z
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
	//	���]�Ƶ{������
	//-------------------------------------------------------------------------------------------
	static void ResetAttackSchedular(RoleDataEx* Owner);

	//-------------------------------------------------------------------------------------------
	//	�ӸO�B�z
	//-------------------------------------------------------------------------------------------
	//�d�M�ӸO�p�G�����h����
	static void	TombProcess(RoleDataEx* Owner);
	//���͹ӸO(�^�� ItemGUID)
	static int CreateTomb(RoleDataEx* Owner, float X, float Y, float Z);
	//////////////////////////////////////////////////////////////////////////
	static void GoodEvilAtkInfoProc(RoleDataEx* AtkRole, RoleDataEx* DefRole, int DHP);
	//////////////////////////////////////////////////////////////////////////
	//�d���w�ɳB�z
	static void ProcPetEvent(RoleDataEx* Owner);
	//////////////////////////////////////////////////////////////////////////
	//�e�d��ʥ]
	//////////////////////////////////////////////////////////////////////////
	static void SendToCli_ByRoomID(int RoomID, int Size, void* Data);
	static void SendToCli_Range(RoleDataEx* Owner, int BlockSize, int Size, void* Data);
	//////////////////////////////////////////////////////////////////////////
	//�a�μv�T�B�z
	static void ZoneDamageProc(RoleDataEx* Owner);
	//////////////////////////////////////////////////////////////////////////
	static bool CheckLuaScript(int OwnerID, int TargetID, const char* LuaScript);


	//Ū��Lua���}�C
	static void	ReadLuaArray(int OwnerID, const char* LuaScript, int Size, int RetArray[]);
	//Pcall CheckFunction
	static bool LuaCheckFunction(int OwnerID, int TargetID, const char* LuaScript);

	//���o�Ū��ж�
//	static PrivateRoomInfoStruct*	GetEmptyRoomID();
	//////////////////////////////////////////////////////////////////////////
	//�W�[����(�Ҧ�����)
	static bool	AddMoneyBase(RoleDataEx* Owner, ActionTypeENUM ActionType, PriceTypeENUM Type, int Money);
	//�ˬd����(�Ҧ�����)
	static bool	CheckMoneyBase(RoleDataEx* Owner, PriceTypeENUM Type, int Money);

	//���U�Ʀ�]���
//	static void	BillboardRegister( int PlayerDBID , int SortType , int SortValue );
	//�O�����ˬd
	static void RecycleBinMemCheck();
	//////////////////////////////////////////////////////////////////////////
	static void FixAllAttackMode();

	//////////////////////////////////////////////////////////////////////////
	//�w�ɦL�XLua�O����ϥζq
	static int  PrintLuaMemoryUsage(SchedularInfo* Obj, void* InputClass);

	//�b�����O�_�i�H�i��[��ާ@
	static bool AccountMoneyOperable();
	//����n�J��ƪ�l��
	static void LoginInitProc(RoleDataEx* Owner);


};
#endif //__GLOBAL_H__