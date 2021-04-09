#include "../NetWakerGSrvInc.h"
#include "NetSrv_Attack.h"
//-------------------------------------------------------------------
NetSrv_Attack*      NetSrv_Attack::This         = NULL;
//-------------------------------------------------------------------
bool NetSrv_Attack::_Init()
{
    Srv_NetCliReg( PG_Attack_CtoL_AttackSignal        );
    Srv_NetCliReg( PG_Attack_CtoL_EndAttackSignal     );
    Srv_NetCliReg( PG_Attack_CtoL_RequestResurrection );
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Attack::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_Attack::_PG_Attack_CtoL_AttackSignal         ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;

    PG_Attack_CtoL_AttackSignal* PG = (PG_Attack_CtoL_AttackSignal*)Data;
    This->R_AttackSignal( Obj , PG->TargetID );
}

//-------------------------------------------------------------------
void NetSrv_Attack::_PG_Attack_CtoL_EndAttackSignal         ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;

    PG_Attack_CtoL_EndAttackSignal* PG = (PG_Attack_CtoL_EndAttackSignal*)Data;
    This->R_EndAttackSignal( Obj );
}

void NetSrv_Attack::_PG_Attack_CtoL_RequestResurrection  ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;

    PG_Attack_CtoL_RequestResurrection* PG = (PG_Attack_CtoL_RequestResurrection*)Data;
    This->R_RequestResurrection( Obj , PG->Type );
}
//-------------------------------------------------------------------
void NetSrv_Attack::S_AttackResult  ( int SendToID , int AttackID , int UnderAttackID , int DamageHP , int OrgDamageHP , ATTACK_DMGTYPE_ENUM	Type , AttackHandTypeENUM	HandType )
{
    PG_Attack_LtoC_AttackResult Send;
    Send.AttackID       = AttackID;		
    Send.UnderAttackID  = UnderAttackID;
    Send.DamageHP       = DamageHP;		
	Send.OrgDamageHP	= OrgDamageHP;
    Send.Type           = Type;
	Send.HandType		= HandType;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Attack::S_All_Dead      ( RoleDataEx* role , int KillerID , int KillerOwnerID , int Time , RolePosStruct& Pos , int ReviveTime , int Mode )
{
	PG_Attack_LtoC_Dead Send;
	Send.DeadID = role->GUID();
	Send.KillerID = KillerID;
	Send.Time = Time;
	Send.Pos = Pos;
	Send.KillerOwnerID = KillerOwnerID;
	Send.ReviveTime = ReviveTime;
	Send.Mode.Mode = Mode;
	Global::SendToCli_ByRoomID( role->RoomID() , sizeof(Send) , &Send );
}
void NetSrv_Attack::S_Dead          ( int SendToID , int DeadID , int KillerID , int KillerOwnerID , int Time , RolePosStruct& Pos , int ReviveTime , int Mode ) 
{
    PG_Attack_LtoC_Dead Send;
    Send.DeadID = DeadID;
    Send.KillerID = KillerID;
    Send.Time = Time;
	Send.Pos = Pos;
	Send.KillerOwnerID = KillerOwnerID;
	Send.ReviveTime = ReviveTime;
	Send.Mode.Mode = Mode;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );

}
void NetSrv_Attack::S_Resurrection  ( int SendToID , int GItemID )
{
    PG_Attack_LtoC_Resurrection Send;
    Send.ItemID = GItemID;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Attack::S_All_HPMPSP      ( RoleDataEx* Owner )
{
	PG_Attack_LtoC_HPMP Send;
	Send.HP = int(Owner->BaseData.HP);
	Send.MP = int(Owner->BaseData.MP);
	Send.SP = int(Owner->BaseData.SP);
	Send.SP_Sub = int(Owner->BaseData.SP_Sub);
	Send.ItemID = Owner->TempData.Sys.GUID;
	Global::SendToCli_ByRoomID( Owner->RoomID() , sizeof(Send) , &Send );
}
void NetSrv_Attack::S_HPMPSP          ( int SendToID , int ItemID , int HP , int MP , int SP , int SP_Sub )
{
    PG_Attack_LtoC_HPMP Send;
    Send.HP = HP;
    Send.MP = MP;
    Send.SP = SP;
	Send.SP_Sub = SP_Sub;
    Send.ItemID = ItemID;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );

}
void NetSrv_Attack::S_All_MaxHPMaxMPMaxSP    ( RoleDataEx* role )
{
	PG_Attack_LtoC_MaxHPMaxMP Send;
	Send.MaxHP  = (int)role->TempData.Attr.Fin.MaxHP;
	Send.MaxMP  = (int)role->TempData.Attr.Fin.MaxMP;
	Send.MaxSP  = (int)role->TempData.Attr.Fin.MaxSP;
	Send.MaxSP_Sub  = (int)role->TempData.Attr.Fin.MaxSP_Sub;
	Send.ItemID = role->TempData.Sys.GUID;
	Global::SendToCli_ByRoomID( role->RoomID() , sizeof(Send) , &Send );
}

void NetSrv_Attack::S_MaxHPMaxMPMaxSP    ( int SendToID , int ItemID , int MaxHP , int MaxMP , int MaxSP , int MaxSP_Sub )
{
    PG_Attack_LtoC_MaxHPMaxMP Send;
    Send.MaxHP  = MaxHP;
    Send.MaxMP  = MaxMP;
    Send.MaxSP  = MaxSP;
	Send.MaxSP_Sub  = MaxSP_Sub;
    Send.ItemID = ItemID;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Attack::S_AttackMode          ( int SendToID , int GItemID , bool Flag )
{
    PG_Attack_LtoC_SetAttackMode Send;
    Send.Flag = Flag;
    Send.GItemID = GItemID;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}


void NetSrv_Attack::S_AttackSignalResult    ( int SendToID , int TargetID , AttackSignalResultENUM  Result )
{
    PG_Attack_LtoC_AttackSignalResult Send;
    Send.TargetID = TargetID;
    Send.Result = Result;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
void	NetSrv_Attack::SC_PartyMemberHPMPSP( int DBID , RoleDataEx* Owner )
{
	PG_Attack_LtoC_PartyMemberHPMPSP	Send;
	Send.DBID = Owner->DBID();
	Send.HP = (int)Owner->BaseData.HP;
	Send.MP = (int)Owner->BaseData.MP;
	Send.SP = (int)Owner->BaseData.SP;
	Send.SP_Sub = (int)Owner->BaseData.SP_Sub;
	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}
void	NetSrv_Attack::SC_PartyMemberMaxHPMPSP( int DBID , RoleDataEx* Owner )
{
	PG_Attack_LtoC_PartyMemberMaxHPMPSP	Send;
	Send.DBID = Owner->DBID();
	Send.MaxHP = int( Owner->TempData.Attr.Fin.MaxHP );
	Send.MaxMP = int( Owner->TempData.Attr.Fin.MaxMP );
	Send.MaxSP = int( Owner->TempData.Attr.Fin.MaxSP );
	Send.MaxSP_Sub = int( Owner->TempData.Attr.Fin.MaxSP_Sub );
	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}

void	NetSrv_Attack::S_NPCAttackTarget	( int SendToID , int NpcGUID , int TargetGUID )
{
	PG_Attack_LtoC_NPCAttackTarget Send;
	Send.NpcGUID	= NpcGUID;
	Send.TargetGUID = TargetGUID;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void	NetSrv_Attack::S_ZonePKFlag			( int SendToID , PKTypeENUM PKType )
{
	PG_Attack_LtoC_ZonePKFlag Send;
	Send.PKType = PKType;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void	NetSrv_Attack::SendAll_ZonePKFlag		( PKTypeENUM PKType )
{
	PG_Attack_LtoC_ZonePKFlag Send;
	Send.PKType = PKType;

	Global::SendToAllCli( sizeof( Send ) , &Send );
}