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
	//�I�k���A�� �Ȯɤ��B�z
    int Ret = 10;
	if( _EnableAI == false )
		return 10;

	NPCAIStateEnum LState = _State;
    switch( _State )
    {
    case EM_AIState_None:                   //������
    case EM_AIState_Idle:                   //���m
        Ret = IdleProc();
        break;
    case EM_AIState_Moving:                 //���ʤ�
        Ret = MovingProc();
        break;
    case EM_AIState_RandMoving:             //�üƲ���
        Ret = RandMovingProc();
        break;

	case EM_AIState_FlagMoving:             //�üƲ���
		Ret = FlagMovingProc();
		break;

    case EM_AIState_Follow:                 //���H�ؼ�
        Ret = FollowProc();
        break;
    case EM_AIState_Attack:                 //�������A�����m
        Ret = AttackProc();
        break;
    case EM_AIState_Escape:                 //�@�몬�A�k�]
        Ret = EscapeProc();
        break;
    case EM_AIState_UnderAttack:            //�Q����(�L�k�}�Ҿ԰�)
        Ret = UnderAttackProc();
        break;
    case EM_AIState_Atk_Follow:             //�԰��� ���H
        Ret = Atk_FollowProc();
        break;
    case EM_AIState_Atk_KeepDistance:       //�԰��� �O���Z��
        Ret = Atk_KeepDistanceProc();
        break;
    case EM_AIState_Atk_Escape:             //�԰��� �k�]
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
