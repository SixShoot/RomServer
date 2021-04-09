#include "../NetWakerGSrvInc.h"
#include "NetSrv_Magic.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
#include "../../mainproc/PartyInfoList/PartyInfoList.h"
//-------------------------------------------------------------------
NetSrv_Magic*    NetSrv_Magic::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_Magic::_Init()
{
 
    Srv_NetCliReg( PG_Magic_CtoL_BeginSpell			);
    Srv_NetCliReg( PG_Magic_CtoL_BeginSpell_Pos		);
    Srv_NetCliReg( PG_Magic_CtoL_SetMagicPoint		);
	Srv_NetCliReg( PG_Magic_LtoC_CancelAssistMagic  );

	Srv_NetCliReg( PG_Magic_CtoL_PetCommand				);
	Srv_NetCliReg( PG_Magic_CtoL_PetMagicSwitchRequest  );
	Srv_NetCliReg( PG_Magic_CtoL_CancelMagicRequest		);
	Srv_NetCliReg( PG_Magic_CtoL_BuffSkillRequest		);
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Magic::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_Magic::_PG_Magic_CtoL_BuffSkillRequest		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;
	PG_Magic_CtoL_BuffSkillRequest* pg =(PG_Magic_CtoL_BuffSkillRequest*)data;
	This->R_BuffSkillRequest( Obj , pg->BuffID , pg->BuffSkillID , pg->TargetID , pg->PosX , pg->PosY , pg->PosZ );
}
void NetSrv_Magic::_PG_Magic_CtoL_CancelMagicRequest	( int sockid , int size , void* data )
{	
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->R_CancelMagicRequest( Obj );
}

void NetSrv_Magic::_PG_Magic_CtoL_PetCommand		   ( int sockid , int size , void* data )			
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;
	PG_Magic_CtoL_PetCommand* pg =(PG_Magic_CtoL_PetCommand*)data;

	This->R_PetCommand( Obj , pg->Action , pg->TargetID , pg->MagicID , pg->PetGUID );
}
void NetSrv_Magic::_PG_Magic_CtoL_PetMagicSwitchRequest( int sockid , int size , void* data )			
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;
	PG_Magic_CtoL_PetMagicSwitchRequest* pg =(PG_Magic_CtoL_PetMagicSwitchRequest*)data;

	This->R_PetMagicSwitchRequest( Obj , pg->PetGUID  , pg->ActiveMagicID , pg->Active );
}

void NetSrv_Magic::_PG_Magic_LtoC_CancelAssistMagic  ( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;
	PG_Magic_LtoC_CancelAssistMagic* pg =(PG_Magic_LtoC_CancelAssistMagic*)data;

	This->R_CancelAssistMagic( Obj , pg->MagicID );
}

void NetSrv_Magic::_PG_Magic_CtoL_SetMagicPoint      ( int sockid , int size , void* data )
{
    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;

    PG_Magic_CtoL_SetMagicPoint* pg =(PG_Magic_CtoL_SetMagicPoint*)data;

    This->R_SetMagicPoint( Obj , pg->MagicID , pg->MagicPos );
}
void NetSrv_Magic::_PG_Magic_CtoL_SetMagicPointEx      ( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Magic_CtoL_SetMagicPointEx* pg =(PG_Magic_CtoL_SetMagicPointEx*)data;

	This->R_SetMagicPointEx( Obj , pg->MagicID , pg->MagicPos , pg->Times );
}
void NetSrv_Magic::_PG_Magic_CtoL_BeginSpell         ( int sockid , int size , void* data )
{
    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;

    PG_Magic_CtoL_BeginSpell* pg =(PG_Magic_CtoL_BeginSpell*)data;

    This->R_SpellRequest( Obj , pg->OwnerID , pg->TargetID , pg->MagicID , pg->MagicPos );
}
void NetSrv_Magic::_PG_Magic_CtoL_BeginSpell_Pos         ( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Magic_CtoL_BeginSpell_Pos* pg =(PG_Magic_CtoL_BeginSpell_Pos*)data;

	This->R_SpellRequest_Pos( Obj , pg->OwnerID , pg->X , pg->Y , pg->Z , pg->MagicID , pg->MagicPos );
}

void NetSrv_Magic::S_BeginSpell          ( int SendID , int OwnerID , int TargetID  , float TargetX , float TargetY , float TargetZ , int MagicID , int SerialID , int SpellTime )
{
    PG_Magic_LtoC_BeginSpell Send;
    Send.Magic      = MagicID;
    Send.OwnerID    = OwnerID;
    Send.TargetID   = TargetID;
	Send.TargetX	= TargetX;
	Send.TargetY	= TargetY;
	Send.TargetZ	= TargetZ;
	Send.SerialID	= SerialID;
	Send.SpellTime	= SpellTime;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Magic::S_EndSpell            ( int SendID , int SerialID )
{
    PG_Magic_LtoC_EndSpell Send;
	Send.SerialID	= SerialID;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Magic::S_BeginSpellOK        ( int SendID , int MagicID , int SerialID , int SpellTime , int ShootAngle )
{
    PG_Magic_LtoC_BeginSpellResult Send;
	Send.SerialID = SerialID;
	Send.MagicID = MagicID;
	Send.SpellTime = SpellTime;
    Send.Result = true;
	Send.ShootAngle = ShootAngle;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Magic::S_BeginSpellFailed    ( int SendID , int MagicID , int SerialID )
{
    PG_Magic_LtoC_BeginSpellResult Send;
    Send.MagicID = MagicID;
	Send.SerialID = SerialID;
    Send.Result = false;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
PG_Magic_LtoC_MagicAttack& NetSrv_Magic::G_MagicAttack         ( int MagicBaseID , int SerialID , MagicAttackTypeENUM	AtkType
																, vector< MagicAtkEffectInfoStruct > & List  )
{
    static PG_Magic_LtoC_MagicAttack Send;
    Send.MagicBaseID  = MagicBaseID;
	Send.AtkType  = AtkType;
	Send.SerialID = SerialID;
    Send.Count    = (int)List.size();
	if( Send.Count > 500 )
		Send.Count = 500;

    for( int i = 0 ; i < Send.Count ; i++ )
    {
        Send.Atk[ i ] = List[i];
    }   
    return Send;  
}

PG_Magic_LtoC_MagicAssistResult& NetSrv_Magic::G_MagicAssist         ( int MagicBaseID , int SerialID , vector< MagicAssistEffectInfoStruct >& List )
{
    static PG_Magic_LtoC_MagicAssistResult Send;

    Send.MagicBaseID  = MagicBaseID;
	Send.SerialID = SerialID;
    Send.Count    = (int)List.size();
	
	if( Send.Count > 500 )
		Send.Count = 500;

    for( int i = 0 ; i < Send.Count ; i++ )
    {
        Send.Atk[ i ] = List[i];
    }  
    return Send;
}

void NetSrv_Magic::S_FixAssistMagicTime  ( int SendID , int MagicID , int EffectTime , int BuffOwnerID )
{
    PG_Magic_LtoC_FixAssistMagicTime Send;
    Send.MagicID = MagicID;
    Send.EffectTime = EffectTime;
	Send.BuffOwnerID = BuffOwnerID;
    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Magic::S_CancelSpellMagic    ( int SendID , int OwnerID , int SerialID )
{
    PG_Magic_LtoC_CancelSpellMagic Send;
    Send.OwnerID = OwnerID;
	Send.SerialID = SerialID;
    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Magic::S_ClearAssistMagic	  ( int SendID , int MagicID , int BuffOwnerID )
{
    PG_Magic_LtoC_FixAssistMagicTime Send;
    Send.MagicID = MagicID;
    Send.EffectTime = 0;
	Send.BuffOwnerID = BuffOwnerID;
    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Magic::S_SetMagicPointResult ( int SendID , int MagicID , int MagicPos , SetMagicPointResultCodeENUM Result )
{
    PG_Magic_LtoC_SetMagicPointResult Send;
    Send.MagicID	= MagicID;
    Send.MagicPos	= MagicPos;
    Send.Result		= Result;
    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
//通知顯示的Buf
void NetSrv_Magic::S_AddBuffInfo(	int SendID , int GItemID , int TargetGItemID , int MagicID , int MagicLv , int EffectTime )
{
	PG_Magic_LtoC_AddBuffInfo Send;
	Send.EffectTime = EffectTime;
	Send.GItemID = GItemID;
	Send.TargetGItemID = TargetGItemID;
	Send.MagicID = MagicID;
	Send.MagicLv = MagicLv;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

//通知顯示的Buf
void NetSrv_Magic::S_ModifyBuffInfo(	int SendID , int GItemID , int TargetGItemID , int MagicID , int MagicLv , int EffectTime )
{
	PG_Magic_LtoC_ModifyBuffInfo Send;
	Send.EffectTime = EffectTime;
	Send.GItemID = GItemID;
	Send.TargetGItemID = TargetGItemID;
	Send.MagicID = MagicID;
	Send.MagicLv = MagicLv;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Magic::S_DelBuffInfo(	int SendID , int GItemID , int MagicID , int BuffOwnerID )
{
	PG_Magic_LtoC_DelBuffInfo Send;	
	Send.GItemID = GItemID;
	Send.MagicID = MagicID;
	Send.BuffOwnerID = BuffOwnerID;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Magic::S_ResetColdown		  ( int SendID , ResetColdownTypeENUM Type )
{
	PG_Magic_LtoC_ResetColdown Send;
	Send.Type = Type;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Magic::S_BeginShoot		  ( int SendID , int SerialID , vector<int>& TargetGUIDList )
{
	PG_Magic_LtoC_BeginShoot Send;
	Send.SerialID = SerialID;
	Send.TargetCount = (int)TargetGUIDList.size();
	if( Send.TargetCount >= _DEF_MAX_MAGIC_TARGET_COUNT_ )
		Send.TargetCount = _DEF_MAX_MAGIC_TARGET_COUNT_;

	for( int i = 0 ; i < Send.TargetCount ; i++ )
	{
		Send.TargetGUIDList[i] = TargetGUIDList[i];
	}

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Magic::S_EndShoot		  ( int SendID , int SerialID )
{
	PG_Magic_LtoC_EndShoot Send;
	Send.SerialID = SerialID;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Magic::S_SysBeginShoot	( int SendID , int OwnerID , int TargetID , int MagicColID , int SerialID , float TX , float TY , float TZ )
{
	PG_Magic_LtoC_SysBeginShoot	Send;
	Send.OwnerID	= OwnerID;		//施法者
	Send.TargetID	= TargetID;		//施法目標
	Send.MagicColID = MagicColID;	//所要施的法術
	Send.SerialID	= SerialID;		//同一次施法的封包會有相同的號碼
	Send.TX			= TX;
	Send.TY			= TY;
	Send.TZ			= TZ;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Magic::S_SysEndShoot	( int SendID , int SerialID )
{
	PG_Magic_LtoC_SysEndShoot Send;
	Send.SerialID = SerialID;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Magic::S_MagicShieldInfo	  ( int SendID , int AttackGUID , int UnAttackGUID , int AttackMagicID , int ShieldMagicID , int AbsobDamage )
{
	PG_Magic_LtoC_MagicShieldInfo	Send;

	Send.AttackMagicID	= AttackMagicID;			//攻擊		基本法術
	Send.ShieldMagicID	= ShieldMagicID;			//防預頓	基本法術
	Send.AttackGUID		= AttackGUID;				//攻擊者
	Send.UnAttackGUID	= UnAttackGUID;				//被攻擊者
	Send.AbsobDamage	= AbsobDamage;					//傷害值

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Magic::S_MagicEnd  ( int SendID , int MagicID , int Coldown_Normal , int Coldown_All )
{
	PG_Magic_LtoC_MagicEnd Send;
	Send.MagicID = MagicID;
	Send.Coldown_Normal = Coldown_Normal;
	Send.Coldown_All = Coldown_All;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Magic::S_CreatePet			  ( int SendID , int PetGUID , int PetWorldGUID , int PetID , int MagicBaseID , SummonPetTypeENUM PetType )
{
	PG_Magic_LtoC_CreatePet Send;
	Send.PetItemID = PetID;
	Send.PetGUID = PetGUID;
	Send.PetWorldGUID = PetWorldGUID;
	Send.MagicBaseID = MagicBaseID;
	Send.PetType = PetType;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Magic::S_PetMagicSwitchResult( int SendID , int PetGUID , SummonPetTypeENUM	PetType , int ActiveMagicID , bool Active )
{
	PG_Magic_LtoC_PetMagicSwitchResult Send;
	Send.ActiveMagicID = ActiveMagicID;
	Send.Active = Active;
	Send.PetGUID = PetGUID;
	Send.PetType = PetType;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Magic::S_DeletePet  ( int SendID , int PetGUID , SummonPetTypeENUM	PetType )
{
	PG_Magic_LtoC_DeletePet Send;
	Send.PetGUID = PetGUID;
	Send.PetType = PetType;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

//通知被施展復活術
void NetSrv_Magic::S_RaiseMagicNotify		( int SendID , int SpellerID , int MagicBaseID )
{
	PG_Magic_LtoC_RaiseMagic Send;
	Send.SpellerID = SpellerID;
	Send.MagicBaseID = MagicBaseID;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Magic::S_BuffSkillResult( int SendID , bool Result )
{
	PG_Magic_LtoC_BuffSkillResult Send;
	Send.Result = Result;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Magic::S_AllMember_PartyMemberBuff( RoleDataEx* Role )
{
	//成員檢查
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( Role->PartyID() );
	if( Party == NULL )
		return;

	int		BuffIDCheckSum = 0;
	PG_Magic_LtoC_PartyMemberBuff Send;
	Send.MemberDBID = Role->DBID();
	Send.Count = Role->BaseData.Buff.Size();
	for( int i = 0 ; i < Send.Count ; i++ )
	{
		BuffIDCheckSum += Role->BaseData.Buff[i].BuffID;
		Send.Buff[i].BuffID = Role->BaseData.Buff[i].BuffID;
		Send.Buff[i].PowerLv = Role->BaseData.Buff[i].Power;
		Send.Buff[i].Time = Role->BaseData.Buff[i].Time;
	}

	if( Role->TempData.BackInfo.ZonePlayerInfo.BuffIDCheckSum == BuffIDCheckSum )
		return;

	Role->TempData.BackInfo.ZonePlayerInfo.BuffIDCheckSum = BuffIDCheckSum;

	for( int i = 0 ; i < (int)Party->Member.size() ; i++ )
	{
		PartyMemberInfoStruct& Member = Party->Member[i];
		if( Member.GItemID == -1 )
			continue;

		if( Member.DBID == Role->DBID() )
			continue;

		OnlinePlayerInfoStruct* Info = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( Member.DBID );
		if( Info == NULL )
			continue;

		Global::SendToCli_ByProxyID( Info->SockID , Info->ProxyID , Send.PGSize() , &Send );

	}
}

void NetSrv_Magic::S_PartyMemberBuff( int SendDBID , RoleDataEx* Role )
{
	PG_Magic_LtoC_PartyMemberBuff Send;
	Send.MemberDBID = Role->DBID();
	Send.Count = Role->BaseData.Buff.Size();
	for( int i = 0 ; i < Send.Count ; i++ )
	{
		Send.Buff[i].BuffID = Role->BaseData.Buff[i].BuffID;
		Send.Buff[i].PowerLv = Role->BaseData.Buff[i].Power;
		Send.Buff[i].Time = Role->BaseData.Buff[i].Time;
	}

	Global::SendToCli_ByDBID(  SendDBID , Send.PGSize() , &Send );
}

void NetSrv_Magic::S_CreatePetRangeInfo	( int PetGUID )
{
	RoleDataEx* Role = Global::GetRoleDataByGUID( PetGUID );
	if( Role == NULL )
		return;

	PG_Magic_LtoC_CreatePet_Range Send;
	Send.PetGUID = PetGUID;
	Global::SendToCli_Range( Role , _Def_View_Block_Range_ , sizeof(Send) ,  &Send );
}