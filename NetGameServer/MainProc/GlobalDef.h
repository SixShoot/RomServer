#ifndef __GLOBALDEFINE_H__
#define __GLOBALDEFINE_H__
#include <string>

using namespace std;
//#define _Def_CheckLoginTime_    15000    //登入過時檢查時間
#define _Def_CheckLoginTime_    30000    //登入過時檢查時間
#define _Def_Map_BaseSize_		5       //地圖方塊大小(用來處理怪物不重疊的區塊大小)


#define	_Def_View_Block_Range_			    4       //視野範圍設定
#define	_Def_View_Block_RangeAttack_	    4       //攻擊資訊範圍設定(不重要的)
#define	_Def_View_Block_RangeTalk_		    1       //範圍說話設定
#define	_Def_View_Block_RangeSearchEnemy_	1       //索敵範圍

#define	_Def_LoginProtectTime_          15
#define	_Def_Max_Move_Proc_Time_		500

//--------------------------------------------------------------------------------------------------------------------
//  周圍物件搜尋列舉
//--------------------------------------------------------------------------------------------------------------------
enum	SearchRangeTypeENUM
{
    EM_SearchRange_All	= 0	    ,
    EM_SearchRange_Member		,
    EM_SearchRange_Friend		,
    EM_SearchRange_Enemy		,
    EM_SearchRange_Player		,
    EM_SearchRange_NPC		    ,
    EM_SearchRange_Item		    ,	
	EM_SearchRange_Player_Enemy ,	
	EM_SearchRange_Player_Friend ,	
};
//--------------------------------------------------------------------------------------------------------------------
//空間位置定義
struct SpaceStruct
{
    int 	X , Y , Z;
    int		RoomID;

    bool operator > ( const SpaceStruct& P ) const
    {
        if( X > P.X )
            return true;
        else if( X < P.X )
            return false;

        if( Y > P.Y )
            return true;
        else if( Y < P.Y )
            return false;

        if( Z > P.Z )
            return true;
        else if( Z < P.Z )
            return false;

        if( RoomID > P.RoomID )
            return true;	

        return false;
    };

    bool operator == ( const SpaceStruct& P ) const
    {
        if( X == P.X && Y == P.Y && Z == P.Z && RoomID == P.RoomID)
            return true;
        return false;
    };

    bool operator < ( const SpaceStruct& P ) const
    {
        if( X < P.X )
            return true;
        else if( X > P.X )
            return false;

        if( Y < P.Y )
            return true;
        else if( Y > P.Y )
            return false;

        if( Z < P.Z )
            return true;
        else if( Z > P.Z )
            return false;

        if( RoomID < P.RoomID )
            return true;	

        return false;
    };

};
//--------------------------------------------------------------------------------------------------------------------
//陣營重生位置結構
struct CampReviveStruct
{
	int		ZoneID;
	float	X , Y , Z;
	string	LuaScript;		//重生要跑的劇情
};
//--------------------------------------------------------------------------------------------------------------------
/*
enum   PKTypeENUM
{
	EM_PKType_Normal            ,
	EM_PKType_PKZone            ,
};
*/
//--------------------------------------------------------------------------------------------------------------------
/*
enum PrivateZoneTypeENUM
{
	EM_PrivateZoneType_None				,	//一般
	EM_PrivateZoneType_Private			,
	EM_PrivateZoneType_Party			,
	EM_PrivateZoneType_Raid				,
	EM_PrivateZoneType_Private_Party	,
};
*/
//區域設定資料
class	GSrvInfoClass
{

public:
	//----------------------------------------------------------------------
	string      Ini;
	string      BaseIni;

	//----------------------------------------------------------------------
	string		ZoneName;		    //區域名稱
	int			ZoneID;			    //區域ID
	int			MapID;				//地圖ID
	string		MapFileName;		//地圖檔案

	int         Switch_GServerPort;
	string      SwitchIP;

	int			Switch_CliPort;
	string		SwitchIP_Outside;

	int			PartitionWidth;	    //區塊分割寬
	int			PartitionHeight;    //區塊分割高
	int			BlockSize;			//區塊大小
	int			PartitionMaxItem;	//分割區最多物件設定

	int         ViewMinX;           //此Server管理的區塊(可見範圍)
	int         ViewMinZ;
	int         ViewMaxX;
	int         ViewMaxZ;

	int         CtrlMinX;           //此Server管理的區塊(控制範圍)
	int         CtrlMinZ;
	int         CtrlMaxX;
	int         CtrlMaxZ;

	int			CampStatuteObj;		//陣營物件

	bool        IsPrivateZone;		//是否為獨立區域
	int			PrivateZoneType;	//獨立區域類型
	int			RoomPlayerCount;	//房間最多玩家數

	int         RoomCount;			//總房間數量
	int			BaseRoomCount;		//樣板房間數量
	int			PlayRoomCount;		//玩家可進入的房間數量

	int			PKType;				//PK類型


	bool		EnabledRevPoint;	//強制重生位置(啟動)
	int			RevZoneID;
	int			RevX;	 
	int			RevY;	 
	int			RevZ;	 
	int			PrivateZoneLiveTime;	//獨立區域沒人可以生存多久 ( 分鐘 )
	int			ZoneLiveTime;			//獨立區域存活時間
	bool		DisabledSavePosition;	//不可記錄位置
	bool		IsEnabledGMCommand;		//是否可用Gm命令
	bool		IsEnableTryException;	//如果程式有問題則重try
	bool		IsHouseZone;			//房屋zone
	bool		IsGuildHouseZone;		//公會屋zone
	bool		IsGuildHouseWarZone;	//公會屋戰zone
	bool		IsReturnSavePoint;		//登入時傳到最後一個劇情儲存點

	bool		IsLoadTxtLuaScript;		//載入文字的lua檔
	bool		IsLoadFdbLuaScript;		//載入fdb 的lua檔

	bool		IsSendAllPlayerPos;		//定時送所有玩家位置給client

	bool		IsSaveLog;				
	bool		IsTalkLog;
	bool		IsExpLog;
	bool		IsLevelLog;
	bool		IsItemTradeLog;
	bool		IsItemTradeLog_Detail;
	bool		IsMoneyLog;
	bool		IsAccountMoneyLog;
	bool		IsPlayerActionLog;
	bool		IsPlayerDeadLog;
	bool		IsMonsterDeadLog;
	bool		IsQuestLog;
	bool		IsClientLog;
	bool		IsEnabledRide;			//可否騎乘
	bool		DisableGoodEvil;		//是否處理善惡值
	bool		IsDisableSendGift;		//可否送禮
	bool		IsServerTotalItemLog;	
	bool		IsIgnoreCollision;		//忽略移動阻擋

	bool		IsSendNpcInfoToGMTools;
	bool		IsGlobalDrop;			//使用全域物品掉落表

	bool		IsHouseMoveItemLog;		//log記錄開關，房屋，玩家移動傢俱、銀行、背包的物品
	bool		IsHouseActionLog;		//log記錄開關，房屋，玩家動作，進入小屋、能源補充等...
	bool		IsHouseServantLog;		//log記錄開關，房屋，玩家對女僕的相關動作，開除、雇用、課程

	string      LuaPath;				//Lua 檔位置
	string      LuaPathEx;				//Lua 檔位置
	string      DataPath;				//Data檔位置
	string		ResourcePath;			//Resource 檔位置

	string      Datalanguage;			//資料語系
	string		Stringlanguage;			//字串語系
	bool		LoadAll;				//是否載入全部資料

	string		LuaFuncInitZone;		//區域開啟初始劇情
	string		LuaFunc_PVP;			//PvP 殺死玩家觸發劇情
	string		ReviveScript;			//復活觸發劇情
	string		EnterZoneScript;		//進入區域觸發劇情
	string		EnterZoneScript_Pcall;	//進入區域觸發劇情

	int			BattleGroundType;		//?戰場類型
	bool		IsCheckRoleDataSize;	//檢查角色資料size

	int			BGLimit;				//?戰場限制
	int			CountryType;			//資料語系
	bool		IsPvE;					//pvp or pve

	int			DeadExpDownRate;		//死亡經驗值懲罰率
	int			DeadDebtExpRate;		//死亡經驗值負債率
	int			DeadDebtTpRate;			//死亡TP負債率

	int			ZoneChannelBroadcastLv;	//可廣播的等級
	int			MailLv;					//可mail的等級
	bool		IsSysChannelNeedItem;	//系統頻道是否需求物品
	bool		IsBattleWorld;			//是否為戰場zone
	bool		IsZonePartyEnabled;		//可否區域隊伍
	bool		IsDisableParty;			//可否組隊


	int 		CreateTombX;			//墓碑產生位置		
	int 		CreateTombY;			//墓碑產生位置
	int 		CreateTombZ;			//墓碑產生位置
	int			CreateTombZoneID;		//墓碑產生位置
	int			BGTeamNumber;			//?

	int			HouseLifeTime;			//房屋沒人時存活時間
	bool		AC_AccountMoneyTrade;		//AC是否可以帳號幣買賣
	bool		AC_Item_AccountMoneyTrade;	//AC物品是否可以以帳號幣販賣
	bool		IsAutoRevive;				// 自動復活不懲罰 EXP 和 TP , EXP 損壞, = 0 時 ( 預設 ) 不自動復活, = 1 時自動
	bool		IsDisablePVPRule;			// 仇人名單, 善惡值 = 0 時 ( 預設 ) 不關掉 PVP 規則, = 1 時關掉
	bool		IsDisableTrade;				// 關掉交易
	bool		IsDisableDual;				// 關掉
	bool		IsNeedCheckDeadZone;		//是否要做死海線檢查
	int			IsAllowHonorSystem;			// 被敵對陣營殺死是否將榮譽分享
	int			ResetHonorValue;			//重設角色榮譽值
	int			MaxRoleLevel;				//最大角色等級
	int			WaitUnlockPosTime;			//卡點解除須要等待秒數

	int			ZoneValue_TPBonus;			//(會被後台設定取代)區域TP額外獎勵
	int			ZoneValue_EXPBonus;			//(會被後台設定取代)區域Exp額外獎勵
	int			ZoneValue_TreasureBouns;	//(會被後台設定取代)區域掉寶額外獎勵
	int			Max_GuildWorld_PlayerCount;	//公會戰上限人數

	bool		IsDisableMailMoney;			//禁止寄錢
	bool		IsDisableMailAccountMoney;	//禁止寄帳號幣
	bool		IsDisableDropRateDesc;		//取消等級差掉落衰減
	bool		IsNoSwimming;				//不可游泳
	bool		IsCheckHeight;				//系統不做移動高度檢查
	bool		IsDisablePKProtectBuff;		//區消pk保護buff


	int			InstanceSaveID;				//複本進度儲存ID
	int			PlayerCenterCount;			//有幾個playercenter

	bool		IsAllowLuaDebugger;			//允許luadebug
	string		LuaDebuggerAgentIP;			
	int			LuaDebuggerAgentPort;
	int			AdjustGuildWarTime;			//調整公會暫時間


	bool		BG_Independence_Game;			//獨立戰場複本(資料不完全回寫)
	int			BG_Independence_Game_Money;		//金錢 0不帶回去 1複寫 2合併
	int			BG_Independence_Game_Item;		//物品 0不帶回去 1複寫 2合併
	int			BG_Independence_Game_KeyItem;	//重要物品 0不帶回去 1複寫 2合併
	int			BG_Independence_Game_Title;		//頭銜 0不帶回去 1複寫 2合併
	int			BG_Independence_Game_Card;		//卡片 0不帶回去 1複寫 2合併
	string		BG_BG_LeaveScript;				//離開獨立戰場觸法的劇情
	string		BG_OrgWorld_LeaveScript;		//離開獨立回到原世界觸發的劇情

	int			DiableGuildHouseWarTime_Begin;	//公會戰 關閉時間
	int			DiableGuildHouseWarTime_End;	//公會戰 關閉時間	

	bool		DisableGMPassword;				//
	string		GMPassword;

	//----------------------------------------------------------------------
};
//--------------------------------------------------------------------------------------------------------------------
//獨立伺服器資訊
struct PrivateRoomInfoStruct
{
	bool	IsActive;		//是否致能
	union
	{
		int		OwnerDBID;	//擁有者DBID
		int		HouseDBID;	//房間號碼
	};
	
	int		OwnerPartyID;	
	int		OwnerRaidID;
	int		EmptyCount;		//定時檢查,連續沒有人的次數
	int		InstanceKeyID;	//-1 表示沒進度 -2 表示還沒初始化
	int		RoomKey;		//識別房屋 是否同間的號碼
	int		LiveTime;		//生存時間

	PrivateRoomInfoStruct()
	{
		Init();
	}
	void Init()
	{
		static int St_RoomKeyID = 1;

		IsActive		= false;
		OwnerDBID		= -1;		//擁有者DBID
		OwnerPartyID	= -1;	
		OwnerRaidID		= -1;
		EmptyCount		= 0;		//定時檢查,連續沒有人的次數
		InstanceKeyID	= -2;		//
		RoomKey			= St_RoomKeyID++;
		LiveTime		= 0;
	}
};
//--------------------------------------------------------------------------------------------------------------------
//Server 物品產生消失的Log
struct ServerItemLogStruct
{
	int ItemID;
	int CreateCount;
	int DestroyCount;
	ServerItemLogStruct()
	{
		memset( this , 0 , sizeof(*this) );
	}
};
//--------------------------------------------------------------------------------------------------------------------
struct LineSegmentStruct
{
	int		ID;
	float	X[2];
	float	Y[2];	

	LineSegmentStruct()
	{
		memset( this , 0 , sizeof(*this) );
	}

};
//--------------------------------------------------------------------------------------------------------------------
#endif