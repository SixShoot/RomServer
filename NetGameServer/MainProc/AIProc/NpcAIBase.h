#pragma once
#include "AIBase.h"
#include "../baseitemobject/BaseItemObject.h"
/*
狀態轉變資訊

    (純 外部設定)
    UnderAttack :    
        當被攻擊時 ， 由 SetState() 直接設定

    Moving : 
        1 劇情設定移動(只有在Idel才可以設定)



    ( 系統 狀態轉變 )
    Idel 轉變    
        SpellMagic :
            1 每五分鐘檢查 自己 與 周圍是否有需要輔助法術            

        RandMove :
            1 有設定亂數移動資訊
            2 RandMove coldown 結束

        Follow :            
            1 有主人 並設定有跟隨資料 

        Attack :
            1 搜索到敵人
            
            
    Moving 轉變:
        Idel:
            到目標點
    
    RandMove 轉變
        Idel 到目標點

    Follow 轉變
        Idel 到要跟隨的目標


    UnderAttack 轉變
        Attack 如果目標可攻擊
        Escape 如果目標不可攻擊

    Escape 轉變
        Idel 固定逃跑一小段時間


    (系統 攻擊狀態轉變)
    Attack 轉變
        Follow :
            當與目標距離過遠
        KeepDistance:            
            如果有遠距離的法術或攻擊方式
        Escape:
            如果HP小於 勇敢度 * ( MaxHP / 10 ) 則有一定機會逃走
        SpellMagic:
            ( 優先權高於 跟隨)
            如果有遠距離的法術或攻擊方式 並且目標過遠

    Follo 轉變:
        Attack 到達目地點

    KeppDistance: 轉變
        Attack: 設定一次位置

    Escape 轉變:
        Attack 
            1 逃跑一定的時間
            2 求救找到 npc 幫忙

    SpellMagic:
        Attack
            施法完後
*/


enum  NPCMoveFlagAITypeENUM
{
	EM_NPCMoveFlagAIType_MoveToNext =0,		// 決定下個移動點
	EM_NPCMoveFlagAIType_Waitting	,		// 到達目的點
	EM_NPCMoveFlagAIType_Moving		,		// 還在移動中
	EM_NPCMoveFlagAIType_WaitMoveEx	,		// 等待外部移到定點，立即處理下一點移動
	EM_NPCMoveFlagAIType_WaitMoveEx1	,		// 等待外部移到定點，立即處理下一點移動
};

//NPC定點移動ai的資料
struct NPCMoveFlagAIStruct 
{
	int		Index;
	float	TargetX , TargetY , TargetZ;
	float	TargetDir;
	int 	CountDown;						//走到以後要等多久
	int		WaitTime;
	bool	IsWalk;
	bool	IsUsePathFind;
	NPCMoveFlagAITypeENUM Type;				//移動中?

	NPCMoveFlagAIStruct()
	{
		Init();
	}
	void Init()
	{
		memset( this , 0 , sizeof(*this) );
		Index = -1;
	}
};

//一般NPC的AI處理
class NpcAIBaseClass: public IAIBaseClass
{    
protected:
    BaseItemObject* _OwnerObj;				//目前處理的角色資料
    RoleDataEx*     _Owner;					//目前處理的角色資料

    int             _ProcTime;				//近入此事件的時間    
    float           _X , _Y , _Z;			//進入戰鬥所在的位置
	bool			_IsBeginAttack;			//剛進入戰鬥時 為true 其他時間都是False
	bool			_MoveToFlagEnabled;		//是否可以處理 旗子移動
	bool			_ClickToFaceEnabled;	//(暫時無用)
	bool			_IsMagicEscape;
	bool			_IsStopLuaOnce;

	//////////////////////////////////////////////////////////////////////////
	//定點移動處理資料
	//////////////////////////////////////////////////////////////////////////
	NPCMoveFlagAIStruct	_NPCMoveFlag;		//旗子移動的資料
	
	//////////////////////////////////////////////////////////////////////////


    //檢查是否有攻擊法術攻擊劇離在範圍內
    bool _CheckNPCAtkMagicDistance( RoleDataEx* Target , float Len );
public:
    NpcAIBaseClass( BaseItemObject* Obj );
    ~NpcAIBaseClass( );    

    //設定並處理切換時所需要處理的問題
    virtual void    SetState( NPCAIStateEnum State );

    //--------------------------------------------------------------------------------------
	// NPC所處狀況的AI處理
	//--------------------------------------------------------------------------------------
    virtual int IdleProc();            
    virtual int RandMovingProc();
    virtual int FlagMovingProc();
    virtual int FollowProc();
    virtual int AttackProc();
    virtual int UnderAttackProc();
    virtual int EscapeProc();
    virtual int Atk_FollowProc();
    virtual int Atk_KeepDistanceProc();
    virtual int Atk_EscapeProc();
	//--------------------------------------------------------------------------------------
	//設定目前所在的旗子
	bool	SetNpcFlagPos( int FlagID );

	//逃離戰場
	//如果周圍有人戰鬥則回傳 true
	bool	GetAwayfromBattle( );

	//搜尋施法目標
	RoleDataEx* SearchNPCMagicTarget( RoleDataEx* Target , NPCSpellTargetENUM TargetType , int MagicID );

	//設定NPC旗標移動開關
	void	SetMoveToFlagEnabled( bool Value );
	void	SetClickToFaceEnable( bool Value )					{ _ClickToFaceEnabled = Value; }

	//資料初始化
	void	DeadInit();

	// 取得目前移動資料
	NPCMoveFlagAIStruct* GetMoveStatus()						{ return &_NPCMoveFlag; }

	//取得目前可施展的攻擊法術
	void SpellMagicProc( NPCSpellRightTimeENUM ProcTime  );
};


