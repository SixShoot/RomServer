#ifndef __GLOBALDEFINE_H__
#define __GLOBALDEFINE_H__
#include <string>

using namespace std;
//#define _Def_CheckLoginTime_    15000    //�n�J�L���ˬd�ɶ�
#define _Def_CheckLoginTime_    30000    //�n�J�L���ˬd�ɶ�
#define _Def_Map_BaseSize_		5       //�a�Ϥ���j�p(�ΨӳB�z�Ǫ������|���϶��j�p)


#define	_Def_View_Block_Range_			    4       //�����d��]�w
#define	_Def_View_Block_RangeAttack_	    4       //������T�d��]�w(�����n��)
#define	_Def_View_Block_RangeTalk_		    1       //�d�򻡸ܳ]�w
#define	_Def_View_Block_RangeSearchEnemy_	1       //���Ľd��

#define	_Def_LoginProtectTime_          15
#define	_Def_Max_Move_Proc_Time_		500

//--------------------------------------------------------------------------------------------------------------------
//  �P�򪫥�j�M�C�|
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
//�Ŷ���m�w�q
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
//�}�筫�ͦ�m���c
struct CampReviveStruct
{
	int		ZoneID;
	float	X , Y , Z;
	string	LuaScript;		//���ͭn�]���@��
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
	EM_PrivateZoneType_None				,	//�@��
	EM_PrivateZoneType_Private			,
	EM_PrivateZoneType_Party			,
	EM_PrivateZoneType_Raid				,
	EM_PrivateZoneType_Private_Party	,
};
*/
//�ϰ�]�w���
class	GSrvInfoClass
{

public:
	//----------------------------------------------------------------------
	string      Ini;
	string      BaseIni;

	//----------------------------------------------------------------------
	string		ZoneName;		    //�ϰ�W��
	int			ZoneID;			    //�ϰ�ID
	int			MapID;				//�a��ID
	string		MapFileName;		//�a���ɮ�

	int         Switch_GServerPort;
	string      SwitchIP;

	int			Switch_CliPort;
	string		SwitchIP_Outside;

	int			PartitionWidth;	    //�϶����μe
	int			PartitionHeight;    //�϶����ΰ�
	int			BlockSize;			//�϶��j�p
	int			PartitionMaxItem;	//���ΰϳ̦h����]�w

	int         ViewMinX;           //��Server�޲z���϶�(�i���d��)
	int         ViewMinZ;
	int         ViewMaxX;
	int         ViewMaxZ;

	int         CtrlMinX;           //��Server�޲z���϶�(����d��)
	int         CtrlMinZ;
	int         CtrlMaxX;
	int         CtrlMaxZ;

	int			CampStatuteObj;		//�}�窫��

	bool        IsPrivateZone;		//�O�_���W�߰ϰ�
	int			PrivateZoneType;	//�W�߰ϰ�����
	int			RoomPlayerCount;	//�ж��̦h���a��

	int         RoomCount;			//�`�ж��ƶq
	int			BaseRoomCount;		//�˪O�ж��ƶq
	int			PlayRoomCount;		//���a�i�i�J���ж��ƶq

	int			PKType;				//PK����


	bool		EnabledRevPoint;	//�j��ͦ�m(�Ұ�)
	int			RevZoneID;
	int			RevX;	 
	int			RevY;	 
	int			RevZ;	 
	int			PrivateZoneLiveTime;	//�W�߰ϰ�S�H�i�H�ͦs�h�[ ( ���� )
	int			ZoneLiveTime;			//�W�߰ϰ�s���ɶ�
	bool		DisabledSavePosition;	//���i�O����m
	bool		IsEnabledGMCommand;		//�O�_�i��Gm�R�O
	bool		IsEnableTryException;	//�p�G�{�������D�h��try
	bool		IsHouseZone;			//�Ы�zone
	bool		IsGuildHouseZone;		//���|��zone
	bool		IsGuildHouseWarZone;	//���|�ξ�zone
	bool		IsReturnSavePoint;		//�n�J�ɶǨ�̫�@�Ӽ@���x�s�I

	bool		IsLoadTxtLuaScript;		//���J��r��lua��
	bool		IsLoadFdbLuaScript;		//���Jfdb ��lua��

	bool		IsSendAllPlayerPos;		//�w�ɰe�Ҧ����a��m��client

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
	bool		IsEnabledRide;			//�i�_�M��
	bool		DisableGoodEvil;		//�O�_�B�z���c��
	bool		IsDisableSendGift;		//�i�_�e§
	bool		IsServerTotalItemLog;	
	bool		IsIgnoreCollision;		//�������ʪ���

	bool		IsSendNpcInfoToGMTools;
	bool		IsGlobalDrop;			//�ϥΥ��쪫�~������

	bool		IsHouseMoveItemLog;		//log�O���}���A�ЫΡA���a���ʳíѡB�Ȧ�B�I�]�����~
	bool		IsHouseActionLog;		//log�O���}���A�ЫΡA���a�ʧ@�A�i�J�p�ΡB�෽�ɥR��...
	bool		IsHouseServantLog;		//log�O���}���A�ЫΡA���a��k���������ʧ@�A�}���B���ΡB�ҵ{

	string      LuaPath;				//Lua �ɦ�m
	string      LuaPathEx;				//Lua �ɦ�m
	string      DataPath;				//Data�ɦ�m
	string		ResourcePath;			//Resource �ɦ�m

	string      Datalanguage;			//��ƻy�t
	string		Stringlanguage;			//�r��y�t
	bool		LoadAll;				//�O�_���J�������

	string		LuaFuncInitZone;		//�ϰ�}�Ҫ�l�@��
	string		LuaFunc_PVP;			//PvP �������aĲ�o�@��
	string		ReviveScript;			//�_��Ĳ�o�@��
	string		EnterZoneScript;		//�i�J�ϰ�Ĳ�o�@��
	string		EnterZoneScript_Pcall;	//�i�J�ϰ�Ĳ�o�@��

	int			BattleGroundType;		//?�Գ�����
	bool		IsCheckRoleDataSize;	//�ˬd������size

	int			BGLimit;				//?�Գ�����
	int			CountryType;			//��ƻy�t
	bool		IsPvE;					//pvp or pve

	int			DeadExpDownRate;		//���`�g����g�@�v
	int			DeadDebtExpRate;		//���`�g��ȭt�Ųv
	int			DeadDebtTpRate;			//���`TP�t�Ųv

	int			ZoneChannelBroadcastLv;	//�i�s��������
	int			MailLv;					//�imail������
	bool		IsSysChannelNeedItem;	//�t���W�D�O�_�ݨD���~
	bool		IsBattleWorld;			//�O�_���Գ�zone
	bool		IsZonePartyEnabled;		//�i�_�ϰ춤��
	bool		IsDisableParty;			//�i�_�ն�


	int 		CreateTombX;			//�ӸO���ͦ�m		
	int 		CreateTombY;			//�ӸO���ͦ�m
	int 		CreateTombZ;			//�ӸO���ͦ�m
	int			CreateTombZoneID;		//�ӸO���ͦ�m
	int			BGTeamNumber;			//?

	int			HouseLifeTime;			//�ЫΨS�H�ɦs���ɶ�
	bool		AC_AccountMoneyTrade;		//AC�O�_�i�H�b�����R��
	bool		AC_Item_AccountMoneyTrade;	//AC���~�O�_�i�H�H�b�����c��
	bool		IsAutoRevive;				// �۰ʴ_�����g�@ EXP �M TP , EXP �l�a, = 0 �� ( �w�] ) ���۰ʴ_��, = 1 �ɦ۰�
	bool		IsDisablePVPRule;			// ���H�W��, ���c�� = 0 �� ( �w�] ) ������ PVP �W�h, = 1 ������
	bool		IsDisableTrade;				// �������
	bool		IsDisableDual;				// ����
	bool		IsNeedCheckDeadZone;		//�O�_�n�������u�ˬd
	int			IsAllowHonorSystem;			// �Q�Ĺ�}������O�_�N�a�A����
	int			ResetHonorValue;			//���]����a�A��
	int			MaxRoleLevel;				//�̤j���ⵥ��
	int			WaitUnlockPosTime;			//�d�I�Ѱ����n���ݬ��

	int			ZoneValue_TPBonus;			//(�|�Q��x�]�w���N)�ϰ�TP�B�~���y
	int			ZoneValue_EXPBonus;			//(�|�Q��x�]�w���N)�ϰ�Exp�B�~���y
	int			ZoneValue_TreasureBouns;	//(�|�Q��x�]�w���N)�ϰ챼�_�B�~���y
	int			Max_GuildWorld_PlayerCount;	//���|�ԤW���H��

	bool		IsDisableMailMoney;			//�T��H��
	bool		IsDisableMailAccountMoney;	//�T��H�b����
	bool		IsDisableDropRateDesc;		//�������Ůt�����I��
	bool		IsNoSwimming;				//���i��a
	bool		IsCheckHeight;				//�t�Τ������ʰ����ˬd
	bool		IsDisablePKProtectBuff;		//�Ϯ�pk�O�@buff


	int			InstanceSaveID;				//�ƥ��i���x�sID
	int			PlayerCenterCount;			//���X��playercenter

	bool		IsAllowLuaDebugger;			//���\luadebug
	string		LuaDebuggerAgentIP;			
	int			LuaDebuggerAgentPort;
	int			AdjustGuildWarTime;			//�վ㤽�|�Ȯɶ�


	bool		BG_Independence_Game;			//�W�߾Գ��ƥ�(��Ƥ������^�g)
	int			BG_Independence_Game_Money;		//���� 0���a�^�h 1�Ƽg 2�X��
	int			BG_Independence_Game_Item;		//���~ 0���a�^�h 1�Ƽg 2�X��
	int			BG_Independence_Game_KeyItem;	//���n���~ 0���a�^�h 1�Ƽg 2�X��
	int			BG_Independence_Game_Title;		//�Y�� 0���a�^�h 1�Ƽg 2�X��
	int			BG_Independence_Game_Card;		//�d�� 0���a�^�h 1�Ƽg 2�X��
	string		BG_BG_LeaveScript;				//���}�W�߾Գ�Ĳ�k���@��
	string		BG_OrgWorld_LeaveScript;		//���}�W�ߦ^���@��Ĳ�o���@��

	int			DiableGuildHouseWarTime_Begin;	//���|�� �����ɶ�
	int			DiableGuildHouseWarTime_End;	//���|�� �����ɶ�	

	bool		DisableGMPassword;				//
	string		GMPassword;

	//----------------------------------------------------------------------
};
//--------------------------------------------------------------------------------------------------------------------
//�W�ߦ��A����T
struct PrivateRoomInfoStruct
{
	bool	IsActive;		//�O�_�P��
	union
	{
		int		OwnerDBID;	//�֦���DBID
		int		HouseDBID;	//�ж����X
	};
	
	int		OwnerPartyID;	
	int		OwnerRaidID;
	int		EmptyCount;		//�w���ˬd,�s��S���H������
	int		InstanceKeyID;	//-1 ��ܨS�i�� -2 ����٨S��l��
	int		RoomKey;		//�ѧO�Ы� �O�_�P�������X
	int		LiveTime;		//�ͦs�ɶ�

	PrivateRoomInfoStruct()
	{
		Init();
	}
	void Init()
	{
		static int St_RoomKeyID = 1;

		IsActive		= false;
		OwnerDBID		= -1;		//�֦���DBID
		OwnerPartyID	= -1;	
		OwnerRaidID		= -1;
		EmptyCount		= 0;		//�w���ˬd,�s��S���H������
		InstanceKeyID	= -2;		//
		RoomKey			= St_RoomKeyID++;
		LiveTime		= 0;
	}
};
//--------------------------------------------------------------------------------------------------------------------
//Server ���~���ͮ�����Log
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