#pragma once

#include "NetSrv_Attack.h"

class NetSrv_AttackChild : public NetSrv_Attack
{
public:
    static bool Init();
    static bool Release();

    virtual void R_AttackSignal             ( BaseItemObject* Obj , int TargetID );
    virtual void R_RequestResurrection      ( BaseItemObject* Obj , RequestResurrection_ENUM Type );
    virtual void R_EndAttackSignal          ( BaseItemObject* Obj );

    //�q���P������Ҧ�
    static void SendRangeAttackMode( RoleDataEx* Owner , bool Mode );

    //�q���P�򪺤H�l�媺��T
    static	void SendRangeAttackInfo( RoleDataEx* Owner , int TargetID ,  ATTACK_DMGTYPE_ENUM	Type , AttackHandTypeENUM	HandType ,  int	DamageHP , int OrgDamageHP );

    //�q���P��ثe���⪺��W��
    static	void SendRangeMaxHPMPInfo( RoleDataEx* Owner );

    //�q���P��ثe��q
    static	void SendRangeHPInfo( RoleDataEx* Owner );

    //�q���P��ثe���⦺�`
    static	void SendRangeDead( RoleDataEx* Owner , int KillerID , int KillerOwnerID , int ReviveTime , int Mode );

    //�q���P��ثe����_��
    static	void SendRangeResurrection( RoleDataEx* Owner );

    //��HP or MP �ק�ɳq��Party �����Ҧ��H
    static	void SendPartyHPMP( RoleDataEx* Owner );

    //��HP MP�W���ȭק�ɳq��Party�����Ҧ��H
    static	void SendPartyMaxHPMaxMP( RoleDataEx* Owner );

	//�q���g�� npc �n�������ؼ�
	static  void SendRangeNpcAttackTarget( RoleDataEx* NPC );


};

