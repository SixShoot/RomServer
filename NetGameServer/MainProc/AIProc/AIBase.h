#pragma once

enum NPCAIStateEnum
{
    EM_AIState_None        = -1 ,       //不改變
    EM_AIState_Idle             ,       //閒置
    EM_AIState_Moving           ,       //移動中
    EM_AIState_RandMoving       ,       //亂數移動
    EM_AIState_FlagMoving       ,       //定點移動
    EM_AIState_Follow           ,       //跟隨目標
    EM_AIState_Escape           ,       //逃跑
    EM_AIState_UnderAttack      ,       //被攻擊(無法開啟戰鬥)
    EM_AIState_Attack           ,       //攻擊狀態的閒置
    EM_AIState_Atk_Follow       ,       //戰鬥中 跟隨
    EM_AIState_Atk_KeepDistance ,       //戰鬥中 保持距離
    EM_AIState_Atk_Escape       ,       //戰鬥中 逃跑
};

class IAIBaseClass
{
protected:
    NPCAIStateEnum  _State;
	NPCAIStateEnum  _LState;
	bool			_EnableAI;
public:
    IAIBaseClass();
    virtual ~IAIBaseClass();
    //--------------------------------------------------------------------------------------
	virtual void	EnableAI( bool Value ){ _EnableAI = Value; };
	virtual bool	EnableAI( ){ return _EnableAI; };

    //設定並處理切換時所需要處理的問題
    virtual void    SetState( NPCAIStateEnum State );

     //--------------------------------------------------------------------------------------
    //基本AI處理(定時處理)
    virtual int AIProc( );    
	//死亡資料初始化
	virtual void DeadInit(){};

    virtual int IdleProc()              { return 0; };            
    virtual int MovingProc()            { return 0; };
    virtual int RandMovingProc()        { return 0; };
    virtual int FlagMovingProc()        { return 0; };
    virtual int FollowProc()            { return 0; };
    virtual int AttackProc()            { return 0; };
    virtual int EscapeProc()            { return 0; };
    virtual int UnderAttackProc()       { return 0; };
    virtual int Atk_FollowProc()        { return 0; };
    virtual int Atk_KeepDistanceProc()  { return 0; };
    virtual int Atk_EscapeProc()        { return 0; };

};
//--------------------------------------------------------------------------------------

