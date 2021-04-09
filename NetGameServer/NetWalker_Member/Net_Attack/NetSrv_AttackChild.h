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

    //通知周圍攻擊模式
    static void SendRangeAttackMode( RoleDataEx* Owner , bool Mode );

    //通知周圍的人損血的資訊
    static	void SendRangeAttackInfo( RoleDataEx* Owner , int TargetID ,  ATTACK_DMGTYPE_ENUM	Type , AttackHandTypeENUM	HandType ,  int	DamageHP , int OrgDamageHP );

    //通知周圍目前角色的血上限
    static	void SendRangeMaxHPMPInfo( RoleDataEx* Owner );

    //通知周圍目前血量
    static	void SendRangeHPInfo( RoleDataEx* Owner );

    //通知周圍目前角色死亡
    static	void SendRangeDead( RoleDataEx* Owner , int KillerID , int KillerOwnerID , int ReviveTime , int Mode );

    //通知周圍目前角色復活
    static	void SendRangeResurrection( RoleDataEx* Owner );

    //當HP or MP 修改時通知Party 內的所有人
    static	void SendPartyHPMP( RoleDataEx* Owner );

    //當HP MP上限值修改時通知Party內的所有人
    static	void SendPartyMaxHPMaxMP( RoleDataEx* Owner );

	//通知週圍 npc 要攻擊的目標
	static  void SendRangeNpcAttackTarget( RoleDataEx* NPC );


};

