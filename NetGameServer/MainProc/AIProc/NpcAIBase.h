#pragma once
#include "AIBase.h"
#include "../baseitemobject/BaseItemObject.h"
/*
���A���ܸ�T

    (�� �~���]�w)
    UnderAttack :    
        ��Q������ �A �� SetState() �����]�w

    Moving : 
        1 �@���]�w����(�u���bIdel�~�i�H�]�w)



    ( �t�� ���A���� )
    Idel ����    
        SpellMagic :
            1 �C�������ˬd �ۤv �P �P��O�_���ݭn���U�k�N            

        RandMove :
            1 ���]�w�üƲ��ʸ�T
            2 RandMove coldown ����

        Follow :            
            1 ���D�H �ó]�w�����H��� 

        Attack :
            1 �j����ĤH
            
            
    Moving ����:
        Idel:
            ��ؼ��I
    
    RandMove ����
        Idel ��ؼ��I

    Follow ����
        Idel ��n���H���ؼ�


    UnderAttack ����
        Attack �p�G�ؼХi����
        Escape �p�G�ؼФ��i����

    Escape ����
        Idel �T�w�k�]�@�p�q�ɶ�


    (�t�� �������A����)
    Attack ����
        Follow :
            ��P�ؼжZ���L��
        KeepDistance:            
            �p�G�����Z�����k�N�Χ����覡
        Escape:
            �p�GHP�p�� �i���� * ( MaxHP / 10 ) �h���@�w���|�k��
        SpellMagic:
            ( �u���v���� ���H)
            �p�G�����Z�����k�N�Χ����覡 �åB�ؼйL��

    Follo ����:
        Attack ��F�ئa�I

    KeppDistance: ����
        Attack: �]�w�@����m

    Escape ����:
        Attack 
            1 �k�]�@�w���ɶ�
            2 �D�ϧ�� npc ����

    SpellMagic:
        Attack
            �I�k����
*/


enum  NPCMoveFlagAITypeENUM
{
	EM_NPCMoveFlagAIType_MoveToNext =0,		// �M�w�U�Ӳ����I
	EM_NPCMoveFlagAIType_Waitting	,		// ��F�ت��I
	EM_NPCMoveFlagAIType_Moving		,		// �٦b���ʤ�
	EM_NPCMoveFlagAIType_WaitMoveEx	,		// ���ݥ~������w�I�A�ߧY�B�z�U�@�I����
	EM_NPCMoveFlagAIType_WaitMoveEx1	,		// ���ݥ~������w�I�A�ߧY�B�z�U�@�I����
};

//NPC�w�I����ai�����
struct NPCMoveFlagAIStruct 
{
	int		Index;
	float	TargetX , TargetY , TargetZ;
	float	TargetDir;
	int 	CountDown;						//����H��n���h�[
	int		WaitTime;
	bool	IsWalk;
	bool	IsUsePathFind;
	NPCMoveFlagAITypeENUM Type;				//���ʤ�?

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

//�@��NPC��AI�B�z
class NpcAIBaseClass: public IAIBaseClass
{    
protected:
    BaseItemObject* _OwnerObj;				//�ثe�B�z��������
    RoleDataEx*     _Owner;					//�ثe�B�z��������

    int             _ProcTime;				//��J���ƥ󪺮ɶ�    
    float           _X , _Y , _Z;			//�i�J�԰��Ҧb����m
	bool			_IsBeginAttack;			//��i�J�԰��� ��true ��L�ɶ����OFalse
	bool			_MoveToFlagEnabled;		//�O�_�i�H�B�z �X�l����
	bool			_ClickToFaceEnabled;	//(�ȮɵL��)
	bool			_IsMagicEscape;
	bool			_IsStopLuaOnce;

	//////////////////////////////////////////////////////////////////////////
	//�w�I���ʳB�z���
	//////////////////////////////////////////////////////////////////////////
	NPCMoveFlagAIStruct	_NPCMoveFlag;		//�X�l���ʪ����
	
	//////////////////////////////////////////////////////////////////////////


    //�ˬd�O�_�������k�N�����@���b�d��
    bool _CheckNPCAtkMagicDistance( RoleDataEx* Target , float Len );
public:
    NpcAIBaseClass( BaseItemObject* Obj );
    ~NpcAIBaseClass( );    

    //�]�w�óB�z�����ɩһݭn�B�z�����D
    virtual void    SetState( NPCAIStateEnum State );

    //--------------------------------------------------------------------------------------
	// NPC�ҳB���p��AI�B�z
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
	//�]�w�ثe�Ҧb���X�l
	bool	SetNpcFlagPos( int FlagID );

	//�k���Գ�
	//�p�G�P�򦳤H�԰��h�^�� true
	bool	GetAwayfromBattle( );

	//�j�M�I�k�ؼ�
	RoleDataEx* SearchNPCMagicTarget( RoleDataEx* Target , NPCSpellTargetENUM TargetType , int MagicID );

	//�]�wNPC�X�в��ʶ}��
	void	SetMoveToFlagEnabled( bool Value );
	void	SetClickToFaceEnable( bool Value )					{ _ClickToFaceEnabled = Value; }

	//��ƪ�l��
	void	DeadInit();

	// ���o�ثe���ʸ��
	NPCMoveFlagAIStruct* GetMoveStatus()						{ return &_NPCMoveFlag; }

	//���o�ثe�i�I�i�������k�N
	void SpellMagicProc( NPCSpellRightTimeENUM ProcTime  );
};


