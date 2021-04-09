#pragma		warning (disable:4786)
#pragma     unmanaged
#include    "AIBase.h"

IAIBaseClass::IAIBaseClass()
{
    _State = EM_AIState_Idle;
	_LState = EM_AIState_Idle;
	_EnableAI = true;
}
IAIBaseClass::~IAIBaseClass()
{

}

int IAIBaseClass::AIProc( )
{
	//施法狀態中 暫時不處理
    int Ret = 10;
	if( _EnableAI == false )
		return 10;

	NPCAIStateEnum LState = _State;
    switch( _State )
    {
    case EM_AIState_None:                   //不改變
    case EM_AIState_Idle:                   //閒置
        Ret = IdleProc();
        break;
    case EM_AIState_Moving:                 //移動中
        Ret = MovingProc();
        break;
    case EM_AIState_RandMoving:             //亂數移動
        Ret = RandMovingProc();
        break;

	case EM_AIState_FlagMoving:             //亂數移動
		Ret = FlagMovingProc();
		break;

    case EM_AIState_Follow:                 //跟隨目標
        Ret = FollowProc();
        break;
    case EM_AIState_Attack:                 //攻擊狀態的閒置
        Ret = AttackProc();
        break;
    case EM_AIState_Escape:                 //一般狀態逃跑
        Ret = EscapeProc();
        break;
    case EM_AIState_UnderAttack:            //被攻擊(無法開啟戰鬥)
        Ret = UnderAttackProc();
        break;
    case EM_AIState_Atk_Follow:             //戰鬥中 跟隨
        Ret = Atk_FollowProc();
        break;
    case EM_AIState_Atk_KeepDistance:       //戰鬥中 保持距離
        Ret = Atk_KeepDistanceProc();
        break;
    case EM_AIState_Atk_Escape:             //戰鬥中 逃跑
        Ret = Atk_EscapeProc();
        break;
    }

	_LState = LState;
    return Ret; 
}

void    IAIBaseClass::SetState( NPCAIStateEnum State )
{
    if( State == EM_AIState_None )
        return;

    _State = State;
    return;
}
