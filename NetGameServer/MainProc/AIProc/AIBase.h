#pragma once

enum NPCAIStateEnum
{
    EM_AIState_None        = -1 ,       //������
    EM_AIState_Idle             ,       //���m
    EM_AIState_Moving           ,       //���ʤ�
    EM_AIState_RandMoving       ,       //�üƲ���
    EM_AIState_FlagMoving       ,       //�w�I����
    EM_AIState_Follow           ,       //���H�ؼ�
    EM_AIState_Escape           ,       //�k�]
    EM_AIState_UnderAttack      ,       //�Q����(�L�k�}�Ҿ԰�)
    EM_AIState_Attack           ,       //�������A�����m
    EM_AIState_Atk_Follow       ,       //�԰��� ���H
    EM_AIState_Atk_KeepDistance ,       //�԰��� �O���Z��
    EM_AIState_Atk_Escape       ,       //�԰��� �k�]
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

    //�]�w�óB�z�����ɩһݭn�B�z�����D
    virtual void    SetState( NPCAIStateEnum State );

     //--------------------------------------------------------------------------------------
    //��AI�B�z(�w�ɳB�z)
    virtual int AIProc( );    
	//���`��ƪ�l��
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

