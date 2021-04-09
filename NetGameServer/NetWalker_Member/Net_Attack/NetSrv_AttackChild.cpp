#include "../NetWakerGSrvInc.h"
#include "NetSrv_AttackChild.h"
#include "../../mainproc/PartyInfoList/PartyInfoList.h"
#include "../../mainproc/magicproc/MagicProcess.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_AttackChild::Init()
{
    NetSrv_Attack::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_AttackChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_AttackChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_Attack::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
void NetSrv_AttackChild::R_AttackSignal             ( BaseItemObject* OwnerObj , int TargetID )
{
    BaseItemObject* TargetObj = BaseItemObject::GetObj( TargetID );
    if( TargetObj == NULL )
    {
        S_AttackSignalResult( OwnerObj->GUID() , TargetID , EM_AttackSignalResult_TargetNULL );
        return;
    }
    RoleDataEx* Target = TargetObj->Role();
    RoleDataEx* Owner  = OwnerObj->Role();
    if( Owner->SetAttack( Target ) != false )
    {
        S_AttackSignalResult( OwnerObj->GUID() , TargetID , EM_AttackSignalResult_OK );
    }
    else
    {
        S_AttackSignalResult( OwnerObj->GUID() , TargetID , EM_AttackSignalResult_TargetDisAttackAbled );
    }

}
void NetSrv_AttackChild::R_EndAttackSignal          ( BaseItemObject* OwnerObj )
{
    RoleDataEx* Owner  = OwnerObj->Role();
 //   Owner->SetAtkFolOff( );
	Owner->TempData.Attr.Action.Attack = false;
	Owner->TempData.AI.AttackCount = 0;
	//Owner->ClearHate( );

}

void NetSrv_AttackChild::R_RequestResurrection      ( BaseItemObject* Obj , RequestResurrection_ENUM Type )
{
	if( Obj->Role()->IsDead() == false )
		return;

	switch( Type )
	{
	case EM_RequestResurrection_Normal:	//一般復活
		Obj->Role()->PlayerBaseData->DeadCountDown = 0;
		break;
	case EM_RequestResurrection_RaiseMagic:	//重生法術
		{
			if( Obj->Role()->PlayerBaseData->RaiseInfo.Mode.MagicRaiseEnabled != false)
			{
				if( Obj->Role()->PlayerBaseData->RaiseInfo.ExpRate > 100 )
					Obj->Role()->PlayerBaseData->RaiseInfo.ExpRate = 100;
				else if ( Obj->Role()->PlayerBaseData->RaiseInfo.ExpRate < 30 ) 
					Obj->Role()->PlayerBaseData->RaiseInfo.ExpRate = 30;
				
				float DecRate = ( 100 - Obj->Role()->PlayerBaseData->RaiseInfo.ExpRate ) / 100.0f;

				if( Global::St()->Ini.ReviveScript.size() != 0 )
				{
					Obj->PlotVM()->PCall( Global::St()->Ini.ReviveScript , Obj->Role()->GUID() , Obj->Role()->GUID() );
				}
				Global::PlayerResuretion( Obj , RoleDataEx::G_ZoneID , Obj->Role()->PlayerBaseData->RaiseInfo.RevX , Obj->Role()->PlayerBaseData->RaiseInfo.RevY , Obj->Role()->PlayerBaseData->RaiseInfo.RevZ , DecRate , true );
			}
			Obj->Role()->PlayerBaseData->RaiseInfo.Mode.MagicRaiseEnabled = false;
		}	
		break;
	}

	Obj->Role()->PlayerBaseData->DeadCountDown = 0;	
}
//-----------------------------------------------------------------
//通知周圍的人損血的資訊
void NetSrv_AttackChild::SendRangeAttackInfo( RoleDataEx* Owner , int TargetID ,  ATTACK_DMGTYPE_ENUM	Type , AttackHandTypeENUM	HandType ,  int	DamageHP , int OrgDamageHP )
{
	if( Owner->TempData.AroundPlayer > 80 )
	{
		S_AttackResult(  Owner->GUID() , Owner->GUID() , TargetID , DamageHP , OrgDamageHP , Type , HandType );
		S_AttackResult(  TargetID	   , Owner->GUID() , TargetID , DamageHP , OrgDamageHP , Type , HandType );
		return;
	}
	else if( Owner->TempData.AroundPlayer > 20 )
	{
		RoleDataEx*	Other;
		Global::ResetRange( Owner , _Def_View_Block_RangeAttack_ );
		while( 1 ) 
		{
			Other = Global::GetRangePlayer();
			if( Other == NULL  )
				break;
			if( Other->Length( Owner ) > 300 )
				continue;

			S_AttackResult(  Other->TempData.Sys.GUID , Owner->TempData.Sys.GUID , TargetID , DamageHP , OrgDamageHP , Type , HandType );
		}
	}
	else
	{
		RoleDataEx*	Other;
		Global::ResetRange( Owner , _Def_View_Block_RangeAttack_ );
		while( 1 ) 
		{
			Other = Global::GetRangePlayer();
			if( Other == NULL  )
				break;

			S_AttackResult(  Other->TempData.Sys.GUID , Owner->TempData.Sys.GUID , TargetID , DamageHP , OrgDamageHP , Type , HandType );
		}
	}
}
//通知周圍目前角色的血上限
void NetSrv_AttackChild::SendRangeMaxHPMPInfo( RoleDataEx* Owner )
{
	Owner->TempData.BackInfo.MaxHP = (int)Owner->TempData.Attr.Fin.MaxHP;
	Owner->TempData.BackInfo.MaxMP = (int)Owner->TempData.Attr.Fin.MaxMP;
	Owner->TempData.BackInfo.MaxSP = (int)Owner->TempData.Attr.Fin.MaxSP;
	Owner->TempData.BackInfo.MaxSP_Sub = (int)Owner->TempData.Attr.Fin.MaxSP_Sub;

	if( Owner->BaseData.Mode.IsAllZoneVisible )
	{
		S_All_MaxHPMaxMPMaxSP( Owner );
	}
	else
	{
		RoleDataEx*	Other;
		Global::ResetRange( Owner , _Def_View_Block_RangeAttack_ );
		while( 1 ) 
		{
			Other = Global::GetRangePlayer();
			if( Other == NULL  )
				break;

			S_MaxHPMaxMPMaxSP(  Other->TempData.Sys.GUID , Owner->TempData.Sys.GUID 
								, (int)Owner->TempData.Attr.Fin.MaxHP 
								, (int)Owner->TempData.Attr.Fin.MaxMP 
								, (int) Owner->TempData.Attr.Fin.MaxSP 
								, (int) Owner->TempData.Attr.Fin.MaxSP_Sub );
		}
	}
}
//通知周圍目前血量
void NetSrv_AttackChild::SendRangeHPInfo( RoleDataEx* Owner )
{
	Owner->TempData.BackInfo.HP = int( Owner->BaseData.HP );
	Owner->TempData.BackInfo.MP = int( Owner->BaseData.MP );
	Owner->TempData.BackInfo.SP = int( Owner->BaseData.SP );
	Owner->TempData.BackInfo.SP_Sub = (int)Owner->BaseData.SP_Sub;
	if( Owner->BaseData.Mode.IsAllZoneVisible )
	{
		S_All_HPMPSP( Owner );
	}
	else
	{
		RoleDataEx*	Other;
		Global::ResetRange( Owner , _Def_View_Block_RangeAttack_ );
		while( 1 ) 
		{
			Other = Global::GetRangePlayer();
			if( Other == NULL  )
				break;

			S_HPMPSP(  Other->TempData.Sys.GUID , Owner->TempData.Sys.GUID , int(Owner->BaseData.HP) , int(Owner->BaseData.MP) , int(Owner->BaseData.SP) , int(Owner->BaseData.SP_Sub) );
		}
	}
}
//通知周圍目前角色死亡
void NetSrv_AttackChild::SendRangeDead( RoleDataEx* Owner , int KillerID , int KillerOwnerID , int ReviveTime , int Mode )
{
	if( Owner->BaseData.Mode.IsAllZoneVisible )
	{
		S_All_Dead( Owner , KillerID , KillerOwnerID , 1000 , *(Owner->Pos()) ,  ReviveTime , Mode );
	}
	else
	{
		RoleDataEx*	Other;
		Global::ResetRange( Owner , _Def_View_Block_RangeAttack_ );
		while( 1 ) 
		{
			Other = Global::GetRangePlayer();
			if( Other == NULL )
				break;
			if( MagicProcessClass::ProcMagicID() != -1 )
				S_Dead(  Other->TempData.Sys.GUID , Owner->TempData.Sys.GUID , KillerID , KillerOwnerID , 1000 , *(Owner->Pos()) ,  ReviveTime , Mode );
			else
				S_Dead(  Other->TempData.Sys.GUID , Owner->TempData.Sys.GUID , KillerID , KillerOwnerID , 0 , *(Owner->Pos()) , ReviveTime , Mode );
		}
	}
}
//通知周圍目前角色復活
void NetSrv_AttackChild::SendRangeResurrection( RoleDataEx* Owner )
{
    RoleDataEx*	Other;
    Global::ResetRange( Owner , _Def_View_Block_Range_ );
    while( 1 ) 
    {
        Other = Global::GetRangePlayer();
        if( Other == NULL  )
            break;

        S_Resurrection(  Other->GUID() , Owner->GUID() );
    }
}
//當HP or MP 修改時通知Party 內的所有人
void NetSrv_AttackChild::SendPartyHPMP( RoleDataEx* Owner )
{
	if( Owner->IsPlayer() == false ) 
		return;

	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( Owner->BaseData.PartyID );
	if( Party == NULL )
		return;

	for( int i = 0 ; i < (int)Party->Member.size() ; i++ )
	{
		if( Owner->DBID() == Party->Member[i].DBID )
			continue;

		SC_PartyMemberHPMPSP( Party->Member[i].DBID , Owner );
	}
}
//當HP MP上限值修改時通知Party內的所有人
void NetSrv_AttackChild::SendPartyMaxHPMaxMP(  RoleDataEx* Owner )
{
	if( Owner->IsPlayer() == false )
		return;

	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( Owner->BaseData.PartyID );
	if( Party == NULL )
		return;

	for( int i = 0 ; i < (int)Party->Member.size() ; i++ )
	{
		if( Owner->DBID() == Party->Member[i].DBID )
			continue;

		SC_PartyMemberMaxHPMPSP( Party->Member[i].DBID , Owner );
	}
}


void NetSrv_AttackChild::SendRangeAttackMode( RoleDataEx* Owner , bool Mode )
{
    RoleDataEx*	Other;
    Global::ResetRange( Owner , _Def_View_Block_RangeAttack_ );
    while( 1 ) 
    {
        Other = Global::GetRangePlayer();
        if( Other == NULL || !Other->IsPlayer()  )
            break;

        S_AttackMode(  Other->TempData.Sys.GUID , Owner->TempData.Sys.GUID , Mode );
    }
}

//通知週圍 npc 要攻擊的目標
void NetSrv_AttackChild::SendRangeNpcAttackTarget( RoleDataEx* NPC )
{
	if( NPC->IsNPC() == false )
		return;

	Global::ResetRange( NPC , _Def_View_Block_RangeAttack_ );
	while( 1 ) 
	{
		RoleDataEx* Other = Global::GetRangePlayer();
		if( Other == NULL || !Other->IsPlayer() )
			break;

		S_NPCAttackTarget( Other->GUID() , NPC->GUID() , NPC->TargetID() );
	}

}

