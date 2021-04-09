#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_Attack.h"

class NetSrv_Attack : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Attack* This;
    static bool _Init();
    static bool _Release();
    
    //-------------------------------------------------------------------
    static void _PG_Attack_CtoL_AttackSignal         ( int NetID , int Size , void* Data );
    static void _PG_Attack_CtoL_EndAttackSignal      ( int NetID , int Size , void* Data );
    static void _PG_Attack_CtoL_RequestResurrection  ( int NetID , int Size , void* Data );
public:    
//    static int  ProcMagic;              //用來區別死亡是否因為魔法

    static void S_AttackSignalResult    ( int SendToID , int TargetID , AttackSignalResultENUM  Result );
    static void S_AttackResult          ( int SendToID , int AttackID , int UnderAttackID , int DamageHP , int OrgDamageHP , ATTACK_DMGTYPE_ENUM	Type , AttackHandTypeENUM	HandType ); 
	static void S_All_Dead              ( RoleDataEx* role , int KillerID , int KillerOwnerID , int Time , RolePosStruct& Pos , int ReviveTime , int Mode );  
    static void S_Dead                  ( int SendToID , int DeadID , int KillerID , int KillerOwnerID , int Time , RolePosStruct& Pos , int ReviveTime , int Mode );  
    static void S_Resurrection          ( int SendToID , int GItemID ); 
    static void S_HPMPSP                ( int SendToID , int GItemID , int HP , int MP , int SP , int SP_Sub ); 
	static void S_All_HPMPSP            ( RoleDataEx* role ); 
    static void S_MaxHPMaxMPMaxSP       ( int SendToID , int GItemID , int MaxHP , int MaxMP , int MaxSP , int MaxSP_Sub ); 
	static void S_All_MaxHPMaxMPMaxSP   ( RoleDataEx* role ); 
    static void S_AttackMode            ( int SendToID , int GItemID , bool Flag );
	static void S_NPCAttackTarget		( int SendToID , int NpcGUID , int TargetGUID );
	static void S_ZonePKFlag			( int SendToID , PKTypeENUM PKType );
	static void SendAll_ZonePKFlag		( PKTypeENUM PKType );


	static void	SC_PartyMemberHPMPSP	( int DBID , RoleDataEx* Owner );
	static void	SC_PartyMemberMaxHPMPSP	( int DBID , RoleDataEx* Owner );

    virtual void R_AttackSignal             ( BaseItemObject* Obj , int TargetID ) = 0;
    virtual void R_EndAttackSignal          ( BaseItemObject* Obj ) = 0;
    virtual void R_RequestResurrection      ( BaseItemObject* Obj , RequestResurrection_ENUM Type ) = 0;

};

