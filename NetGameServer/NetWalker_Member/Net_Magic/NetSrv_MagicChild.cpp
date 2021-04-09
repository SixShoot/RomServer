#include "../NetWakerGSrvInc.h"
#include "NetSrv_MagicChild.h"
#include "../../MainProc/MagicProc/MagicProcess.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_MagicChild::Init()
{
    NetSrv_Magic::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_MagicChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_MagicChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_Magic::_Release();

    delete This;
    This = NULL;

    return true;
    
}

void NetSrv_MagicChild::R_CancelAssistMagic  ( BaseItemObject* OwnerObj , int MagicID )
{
	RoleDataEx* Owner = OwnerObj->Role();

	GameObjDbStructEx	*MagicOrgDB = Global::GetObjDB( MagicID );
	if( MagicOrgDB == NULL )
		return;

	if( MagicOrgDB->MagicBase.Setting.CancelBuff )
	{
		Owner->ClearBuff( MagicID , -1 );
	}
}


void NetSrv_MagicChild::R_SpellRequest       ( BaseItemObject* OwnerObj , int OwnerID , int TargetID , int MagicID , int MagicPos )
{
	RoleDataEx* Owner = OwnerObj->Role();
    //目前只能控制自己的角色
    if( Owner->GUID() != OwnerID )
        return;

	int MagicLv = 0;

	if( MagicPos == -1 )
	{
		GameObjDbStructEx* TitleDB = Global::GetObjDB( Owner->BaseData.TitleID );
		//檢查是否是頭銜技能
		if(		TitleDB->IsTitle() 
			&&	TitleDB->Title.SkillID == MagicID )
		{
			MagicLv = TitleDB->Title.SkillLv;
		}
		//檢查是否為魔靈技能
		else if( MagicID == Owner->PlayerBaseData->Phantom.SkillID )
		{
			MagicLv = Owner->PlayerBaseData->Phantom.SkillLv;
		}
		else 
		{
			return;
		}
		
	}
	else
	{	//檢查是否有此法術
		if( Owner->TempData.Attr.Effect.DisableJobSkill != false )
			return;

		if( Owner->CheckRoleMagic( MagicID , MagicPos ) == false )
			return;

		MagicLv = Owner->GetMagicLv( MagicPos );
	}

	if( Owner->TempData.Attr.Effect.DisableJobSkill != false )
		return;

	RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );
	if( Target == NULL )
		Target = Owner;

	GameObjDbStructEx* SpellMagicCol = Global::GetObjDB( MagicID );
	if( SpellMagicCol == NULL || SpellMagicCol->MagicCol.PetSkillType != -1 )
	{
		return;
	}

	if( Owner->IsSpell() )
	{
		if( Owner->TempData.Magic.MagicCollectID == MagicID )
		{
			return;
		}
		//取得正在施展的法術資料
		GameObjDbStructEx* MagicCol = Global::GetObjDB( Owner->TempData.Magic.MagicCollectID );

		//取得正在施展的法術資料		
		if( MagicCol != NULL && MagicCol->MagicCol.Flag.Interrupt_SpellOtherMagic )
		{
			if( Owner->TempData.Magic.State != EM_MAGIC_PROC_STATE_NORMAL )
				Owner->TempData.Magic.State = EM_MAGIC_PROC_STATE_INTERRUPT;

			MagicProcessClass::Process( Owner , &(Owner->TempData.Magic) );
		}
		else return;

	}
	
	BaseItemObject* WagonObj = Global::GetObj( Owner->TempData.AttachObjID );
	if( WagonObj != NULL && SpellMagicCol != NULL )
	{
		if( SpellMagicCol->MagicCol.IsOnWagonEnabled == false )
			return;
	}

	//check target sneak
	if(		Target->TempData.Attr.Effect.Sneak  )
	{
		//check magic target type
		if(		SpellMagicCol->MagicCol.TargetType != EM_Releation_Self 
			&&  SpellMagicCol->MagicCol.TargetType != EM_Releation_Locatoin )
		{
			int level = Owner->Level() - Target->Level();
			float baseDistance = 30.0f;

			float distance = 0.0f;
			if ( Ini()->IsBattleWorld && Target->TempData.AI.CampID == Owner->TempData.AI.CampID ) 
			{
				distance = 10000.0f;
			}
			else
			{
				if ( Owner->TempData.Attr.Effect.DetectSneak )
					baseDistance += (float)(min(max(g_ObjectData->GetSysKeyValue("Listening"), 0), 500));
				distance = baseDistance + ((level > 0) ? level * 5.0f : level * 3.0f);
			}		
			if( Owner->Length( Target ) > distance )
				return;
		}
		
	}


	MagicProcessClass::SpellMagic( OwnerID , TargetID , Target->X() , Target->Y() , Target->Z() , MagicID , MagicLv/*Owner->GetMagicLv( MagicPos )*/ );	
}

void NetSrv_MagicChild::R_SpellRequest_Pos   ( BaseItemObject* OwnerObj , int OwnerID , float X , float Y , float Z , int MagicID , int MagicPos )
{
	RoleDataEx* Owner = OwnerObj->Role();
	//目前只能控制自己的角色
	if( Owner->GUID() != OwnerID )
		return;

	int MagicLv = 0;

	if( MagicPos == -1 )
	{

		GameObjDbStructEx* TitleDB = Global::GetObjDB( Owner->BaseData.TitleID );
		//檢查是否是頭銜技能
		if(		TitleDB->IsTitle() != false 
			&&	TitleDB->Title.SkillID == MagicID )
		{
			MagicLv = TitleDB->Title.SkillLv;
		}

	}
	else
	{	//檢查是否有此法術
		if( Owner->TempData.Attr.Effect.DisableJobSkill != false )
			return;

		if( Owner->CheckRoleMagic( MagicID , MagicPos ) == false )
			return;

		MagicLv = Owner->GetMagicLv( MagicPos );
	}
	/*
	if( Owner->TempData.Attr.Effect.DisableJobSkill != false )
		return;

	//檢查是否有此法術
	if( Owner->CheckRoleMagic( MagicID , MagicPos ) == false )
		return;
*/
	if( Owner->IsSpell() )
	{
		if( Owner->TempData.Magic.MagicCollectID == MagicID )
		{
			return;
		}
		//取得正在施展的法術資料
		GameObjDbStructEx* MagicCol = Global::GetObjDB( Owner->TempData.Magic.MagicCollectID );
		if( MagicCol != NULL && MagicCol->MagicCol.Flag.Interrupt_SpellOtherMagic )
		{
			if( Owner->TempData.Magic.State != EM_MAGIC_PROC_STATE_NORMAL )
				Owner->TempData.Magic.State = EM_MAGIC_PROC_STATE_INTERRUPT;

			MagicProcessClass::Process( Owner , &(Owner->TempData.Magic) );
		}
		else return;

	}

	GameObjDbStructEx* SpellMagicCol = Global::GetObjDB( MagicID );
	BaseItemObject* WagonObj = Global::GetObj( Owner->TempData.AttachObjID );
	if( WagonObj != NULL && SpellMagicCol != NULL )
	{
		if( SpellMagicCol->MagicCol.IsOnWagonEnabled == false )
			return;
	}

	MagicProcessClass::SpellMagic( OwnerID , NULL , X , Y , Z , MagicID , MagicLv );	
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//通知週圍的 A 用法術攻擊打(or治療) B
void NetSrv_MagicChild::MagicAttackRange	(  int OwnerID ,  int MagicBaseID , int SerialID , MagicAttackTypeENUM	AtkType
                                             , vector< MagicAtkEffectInfoStruct > & List  )
{
    RoleDataEx *Owner = Global::GetRoleDataByGUID( OwnerID );
    RoleDataEx *Other; 

    if( Owner == NULL )
        return;

    Global::ResetRange( Owner , _Def_View_Block_Range_ );

    PG_Magic_LtoC_MagicAttack& Send = G_MagicAttack( MagicBaseID , SerialID , AtkType ,  List );

	if( Owner->TempData.AroundPlayer > 20 )
	{
		while( (Other = Global::GetRangePlayer()) != NULL )
		{
			if( Other->Length( Owner ) > 300 )
				continue;
			Global::SendToCli_ByGUID( Other->GUID() , Send.Size() , &Send );
		}
	}
	else
	{
		while( (Other = Global::GetRangePlayer()) != NULL )
		{
			Global::SendToCli_ByGUID( Other->GUID() , Send.Size() , &Send );
		}
	}
}


//通知週圍的 A 用輔助法術攻擊(or 幫助) B
void NetSrv_MagicChild::MagicAssistRange	(  int OwnerID , int MagicBaseID , int SerialID
                                             , vector< MagicAssistEffectInfoStruct >& List)
{
    RoleDataEx *Owner = Global::GetRoleDataByGUID( OwnerID );
    RoleDataEx *Other; 

    if( Owner == NULL )
        return;

    Global::ResetRange( Owner , _Def_View_Block_RangeAttack_);

    PG_Magic_LtoC_MagicAssistResult& Send = G_MagicAssist ( MagicBaseID  , SerialID ,  List );

    while( (Other = Global::GetRangePlayer()) != NULL )
    {
       Global::SendToCli_ByGUID( Other->GUID() , Send.Size() , &Send );
    }
}

//通知週圍某角色開始施法
void NetSrv_MagicChild::BeginSpellRange( int	OwnerID , int TargetID , float TargetX , float TargetY , float TargetZ , int MagicID , int SerialID , int SpellTime )
{
    RoleDataEx *Owner  = Global::GetRoleDataByGUID( OwnerID );
    RoleDataEx *Target = Global::GetRoleDataByGUID( TargetID );
    RoleDataEx *Other; 

    if( Owner == NULL || Target == NULL )
        return;

    //設定面朝目標
    if( !Owner->IsPlayer() && Target != Owner )
    {
        Owner->BaseData.Pos.Dir = Owner->CalDir( Target );
    }
    Global::ResetRange( Owner , _Def_View_Block_Range_ );


    while( (Other = Global::GetRangePlayer()) != NULL )
    {
        S_BeginSpell( Other->GUID() , OwnerID , TargetID , TargetX , TargetY , TargetZ , MagicID , SerialID , SpellTime );
    }
}

//通知週圍某角色 系統出手
void NetSrv_MagicChild::SysBeginShootRange	( int	OwnerID , int TargetID , int MagicColID , int SerialID , float TX , float TY , float TZ  )
{
	RoleDataEx *Owner = Global::GetRoleDataByGUID( OwnerID );
	RoleDataEx *Other; 

	if( Owner == NULL )
		return;

	Global::ResetRange( Owner , _Def_View_Block_Range_ );


	while( (Other = Global::GetRangePlayer()) != NULL )
	{
		S_SysBeginShoot( Other->GUID() , OwnerID , TargetID , MagicColID , SerialID , TX , TY , TZ );
	}
}
//通知週圍某角色 系統結束
void NetSrv_MagicChild::SysEndShootRange	( int	OwnerID , int SerialID )
{
	RoleDataEx *Owner = Global::GetRoleDataByGUID( OwnerID );
	RoleDataEx *Other; 

	if( Owner == NULL )
		return;

	Global::ResetRange( Owner , _Def_View_Block_Range_ );


	while( (Other = Global::GetRangePlayer()) != NULL )
	{
		S_SysEndShoot( Other->GUID() , SerialID );
	}
}


//通知週圍某角色停止開始施法
void NetSrv_MagicChild::EndSpellRange	( int	OwnerID , int SerialID  )
{
    RoleDataEx *Owner = Global::GetRoleDataByGUID( OwnerID );
    RoleDataEx *Other; 

    if( Owner == NULL )
        return;

    Global::ResetRange( Owner , _Def_View_Block_Range_ );


    while( (Other = Global::GetRangePlayer()) != NULL )
    {
        S_EndSpell( Other->GUID() , SerialID );
    }
}

//通知週圍某角色出手
void NetSrv_MagicChild::BeginShootRange	( int	OwnerID , int SerialID , vector<int>& TargetGUIDList )
{
	RoleDataEx *Owner = Global::GetRoleDataByGUID( OwnerID );
	RoleDataEx *Other; 

	if( Owner == NULL )
		return;

	Global::ResetRange( Owner , _Def_View_Block_Range_ );


	while( (Other = Global::GetRangePlayer()) != NULL )
	{
		S_BeginShoot( Other->GUID() , SerialID , TargetGUIDList );
	}
}

void NetSrv_MagicChild::EndShootRange	( int	OwnerID , int SerialID  )
{
	RoleDataEx *Owner = Global::GetRoleDataByGUID( OwnerID );
	RoleDataEx *Other; 

	if( Owner == NULL )
		return;

	Global::ResetRange( Owner , _Def_View_Block_Range_ );


	while( (Other = Global::GetRangePlayer()) != NULL )
	{
		S_EndShoot( Other->GUID() , SerialID );
	}
}

//取消某人施法
void NetSrv_MagicChild::CancleSpellMagicRange( int	OwnerID , int SerialID )
{
    RoleDataEx *Owner = Global::GetRoleDataByGUID( OwnerID );
    RoleDataEx *Other; 

    if( Owner == NULL )
        return;

    Global::ResetRange( Owner , _Def_View_Block_Range_ );


    while( (Other = Global::GetRangePlayer()) != NULL )
    {
        S_CancelSpellMagic( Other->GUID() , OwnerID , SerialID  );
    }
}

void NetSrv_MagicChild::R_SetMagicPoint      ( BaseItemObject* OwnerObj , int MagicID , int MagicPos )
{

    RoleDataEx *Owner = OwnerObj->Role();

	if(  (unsigned)MagicPos >= _MAX_SPSkill_COUNT_+_MAX_NormalSkill_COUNT_ )
	{
		S_SetMagicPointResult( Owner->GUID() , MagicID , MagicPos , EM_MagicPointResult_MagicPosTooLarge );
		return;
	}

	float	&MagicLv = Owner->TempData.Attr.Ability->AllSkillLv[ MagicPos ];
	int		&TpExp   = Owner->TempData.Attr.Ability->TpExp;

	if( MagicID != Owner->PlayerTempData->Skill.AllSkill[ MagicPos ] )
	{
		S_SetMagicPointResult( Owner->GUID() , MagicID , MagicPos , EM_MagicPointResult_IDandPosError );
		return;
	}
	
	//取得法術資料
	GameObjDbStructEx* MagicObjDB = GetObjDB( MagicID );

	if( MagicObjDB == NULL || TpExp == 0 )
	{
		S_SetMagicPointResult( Owner->GUID() , MagicID , MagicPos , EM_MagicPointResult_OtherError );
		return;
	}

	if( MagicObjDB->MagicCol.MaxSkillLv <= int( MagicLv ) )
	{
		S_SetMagicPointResult( Owner->GUID() , MagicID , MagicPos , EM_MagicPointResult_MaxLvError );
		return;
	}

	//取得該等級升級所需經驗值

	int ExpMagicLv = RangeValue( int( MagicLv ) + int(MagicObjDB->MagicCol.ExpTableRate) - 1 , int( RoleDataEx::JobSkillExpTable.size() ) -1 , 0 );

	int BaseNeedExp = int( RoleDataEx::JobSkillExpTable[ ExpMagicLv ]  );

	int NeedExp = int( BaseNeedExp  * ( 1 + int( MagicLv ) - MagicLv  ) + 0.9 );

	if( TpExp >= NeedExp )
	{
		TpExp -= NeedExp;
		MagicLv = float( int( MagicLv ) + 1 );
	}
	else
	{
		MagicLv = MagicLv + float( TpExp ) / BaseNeedExp ;
		TpExp = 0;
	}
	
	Owner->Net_FixRoleValue( (RoleValueName_ENUM)(EM_RoleValue_NormalMagic + MagicPos) , MagicLv );
	Owner->Net_FixRoleValue( EM_RoleValue_TpExp , float( TpExp ) );

    S_SetMagicPointResult( Owner->GUID() , MagicID , MagicPos , EM_MagicPointResult_OK );
    
	//if( MagicObjDB->MagicCol.EffectType == EM_Magic_Eq )
	Owner->TempData.UpdateInfo.ReSetTalbe = true;
}


void NetSrv_MagicChild::R_SetMagicPointEx      ( BaseItemObject* OwnerObj , int MagicID , int MagicPos , int Times )
{
/*
	RoleDataEx *Owner = OwnerObj->Role();

	if(  (unsigned)MagicPos >= _MAX_SPSkill_COUNT_+_MAX_NormalSkill_COUNT_ )
	{
		S_SetMagicPointResult( Owner->GUID() , MagicID , MagicPos , EM_MagicPointResult_MagicPosTooLarge );
		return;
	}

	float	&MagicLv = Owner->TempData.Attr.Ability->AllSkillLv[ MagicPos ];
	int		&TpExp   = Owner->TempData.Attr.Ability->TpExp;

	if( MagicID != Owner->PlayerTempData->Skill.AllSkill[ MagicPos ] )
	{
		S_SetMagicPointResult( Owner->GUID() , MagicID , MagicPos , EM_MagicPointResult_IDandPosError );
		return;
	}

	//取得法術資料
	GameObjDbStructEx* MagicObjDB = GetObjDB( MagicID );

	if( MagicObjDB == NULL || TpExp == 0 )
	{
		S_SetMagicPointResult( Owner->GUID() , MagicID , MagicPos , EM_MagicPointResult_OtherError );
		return;
	}

	if( MagicObjDB->MagicCol.MaxSkillLv <= int( MagicLv ) )
	{
		S_SetMagicPointResult( Owner->GUID() , MagicID , MagicPos , EM_MagicPointResult_MaxLvError );
		return;
	}

	//取得該等級升級所需經驗值

	int ExpMagicLv = RangeValue( int( MagicLv ) + int(MagicObjDB->MagicCol.ExpTableRate) - 1 , int( RoleDataEx::JobSkillExpTable.size() ) -1 , 0 );

	int BaseNeedExp = int( RoleDataEx::JobSkillExpTable[ ExpMagicLv ]  );

	int NeedExp = int( BaseNeedExp  * ( 1 + int( MagicLv ) - MagicLv  ) + 0.9 );

	if( TpExp >= NeedExp )
	{
		TpExp -= NeedExp;
		MagicLv = float( int( MagicLv ) + 1 );
	}
	else
	{
		MagicLv = MagicLv + float( TpExp ) / BaseNeedExp ;
		TpExp = 0;
	}

	Owner->Net_FixRoleValue( (RoleValueName_ENUM)(EM_RoleValue_NormalMagic + MagicPos) , MagicLv );
	Owner->Net_FixRoleValue( EM_RoleValue_TpExp , float( TpExp ) );

	S_SetMagicPointResult( Owner->GUID() , MagicID , MagicPos , EM_MagicPointResult_OK );

	if( MagicObjDB->MagicCol.EffectType == EM_Magic_Eq )
		Owner->TempData.UpdateInfo.ReSetTalbe = true;
		*/
}

//清除某輔助法術(範圍通知)
void NetSrv_MagicChild::S_ClearAssistMagic_Range( int OwnerID , int MagicID , int BuffOwnerID )
{
//	S_ClearAssistMagic( OwnerID , MagicID , BuffOwnerID  );
	RoleDataEx *Owner = Global::GetRoleDataByGUID( OwnerID );
	RoleDataEx *Other; 

	if( Owner == NULL )
		return;

	Global::ResetRange( Owner , _Def_View_Block_Range_ );

	while( (Other = Global::GetRangePlayer()) != NULL )
	{
		if( Other->GUID() != OwnerID )
			S_DelBuffInfo( Other->GUID() , OwnerID , MagicID , BuffOwnerID );
	}
	S_DelBuffInfo( OwnerID , OwnerID , MagicID , BuffOwnerID );
}

//通知他人自己有的Buf
void NetSrv_MagicChild::S_AddBuffInfo_List( int SendID , RoleDataEx* Role )
{
	S_DelBuffInfo( SendID , Role->GUID() , -1 , -1 );
	for( int i = 0 ; i < Role->BaseData.Buff.Size() ; i++ )
	{
		S_AddBuffInfo( SendID , Role->GUID() , Role->BaseData.Buff[i].OwnerID , Role->BaseData.Buff[i].BuffID , Role->BaseData.Buff[i].Power , Role->BaseData.Buff[i].Time );
	}
	
}

//通知週圍某角色 魔法盾資訊
void NetSrv_MagicChild::MagicShieldInfoRange( int AttackGUID , int UnAttackGUID , int AttackMagicID , int ShieldMagicID , int AbsobDamage )
{

	RoleDataEx *Owner = Global::GetRoleDataByGUID( UnAttackGUID );
	RoleDataEx *Other; 

	if( Owner == NULL )
		return;

	Global::ResetRange( Owner , _Def_View_Block_Range_ );


	while( (Other = Global::GetRangePlayer()) != NULL )
	{
		S_MagicShieldInfo( Other->GUID() , AttackGUID , UnAttackGUID , AttackMagicID , ShieldMagicID , AbsobDamage );
	}
}


void NetSrv_MagicChild::SendRange_AddBuffInfo( int GItemID , int TargetGItemID , int MagicID , int MagicLv , int EffectTime )
{
	RoleDataEx *Owner = Global::GetRoleDataByGUID( GItemID );
	RoleDataEx *Other; 

	if( Owner == NULL )
		return;

	Global::ResetRange( Owner , _Def_View_Block_Range_ );

	while( (Other = Global::GetRangePlayer()) != NULL )
	{
		S_AddBuffInfo( Other->GUID() , GItemID , TargetGItemID , MagicID , MagicLv , EffectTime );
	}
}

void NetSrv_MagicChild::SendRange_ModifyBuffInfo( int GItemID , int TargetGItemID , int MagicID , int MagicLv , int EffectTime )
{
	RoleDataEx *Owner = Global::GetRoleDataByGUID( GItemID );
	RoleDataEx *Other; 

	if( Owner == NULL )
		return;

	Global::ResetRange( Owner , _Def_View_Block_Range_ );

	while( (Other = Global::GetRangePlayer()) != NULL )
	{
		S_ModifyBuffInfo( Other->GUID() , GItemID , TargetGItemID , MagicID , MagicLv , EffectTime );
	}
}


void NetSrv_MagicChild::R_PetCommand			( BaseItemObject* OwnerObj , PET_ACTION_ENUM Action , int TargetID , int MagicID , int PetGUID )
{

	RoleDataEx* Owner = OwnerObj->Role();
//	PetStruct& PetInfo = ;//Owner->TempData.SummonPet;
	BaseItemObject* PetObj = Global::GetObj( PetGUID );
	RoleDataEx* Pet = NULL;// = Global::GetRoleDataByGUID( PetInfo.PetID );
	RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );
	if( PetObj != NULL )
		Pet = PetObj->Role();

	if( Pet == NULL || Pet->IsPet( Owner ) == false  )
	{
		//Owner->Msg( "找不到寵物" );
//		Owner->Net_GameMsgEvent( EM_GameMessageEvent_MagicError_PetNotFound );
		return;
	}

	PetStruct& PetInfo = *(Owner->GetPetInfo( Pet ));
	if( Target == NULL )
		Target = Pet;

	if( Action == EM_PET_SPELLMAGIC )
	{
		//檢查是否有此法術
		for( int i = 0 ; i < PetInfo.Skill.Size() ; i++ )
		{
			if( PetInfo.Skill[i].MagicColID == MagicID )
			{

				GameObjDbStructEx* MagicCol = Global::GetObjDB( MagicID );
				if( MagicCol == NULL )
					return;
				
				if( Target != Pet )
					Pet->BaseData.Pos.Dir = Pet->CalDir( Target );
				
				if( MagicCol->MagicCol.IsAutoSpell != false )
				{
					MagicProcInfo  MagicInfo;
					MagicInfo.Init();
					MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
					MagicInfo.TargetID			= TargetID;				//目標
					MagicInfo.MagicCollectID	= MagicID;			//法術
					MagicInfo.MagicLv			= PetInfo.Skill[i].MagicLv;
					bool Ret = MagicProcessClass::SysSpellMagic( Pet , &MagicInfo );
				}
				else
				{
					Pet->StopMove();
					MagicProcessClass::SpellMagic( PetInfo.PetID , TargetID , Pet->Pos()->X , Pet->Pos()->Y , Pet->Pos()->Z , MagicID , PetInfo.Skill[i].MagicLv );
				}
				break;
			}
		}	
	}
	else
	{
		switch( Action )
		{
		case EM_PET_NONE:   // 目前沒命令
			break;
		case EM_PET_STOP:
			break;
		case EM_PET_FOLLOW:
//			Pet->SetAtkFolOff();
			Pet->StopMove();
			break;
		case EM_PET_ATTACK:
			break;
		case EM_PET_ATTACKOFF:
			if( Pet->IsAttack() == false )
				return;
			break;
		case EM_PET_STRICKBACK:
			if( Pet->BaseData.Mode.Strikback == false )
			{
				Pet->BaseData.Mode.Strikback = true;
				NetSrv_RoleValue::S_ObjMode( Owner->GUID() , Pet->GUID() , Pet->BaseData.Mode );
				return;
			}
			return;
		case EM_PET_STRICKBACK_OFF:
			if( Pet->BaseData.Mode.Strikback != false )
			{
				Pet->BaseData.Mode.Strikback = false;
				NetSrv_RoleValue::S_ObjMode( Owner->GUID() , Pet->GUID() , Pet->BaseData.Mode );
				return;
			}
			return;
		case EM_PET_GUARD:
			break;		
		}
		PetInfo.Action   =  Action;
		PetInfo.TargetID = TargetID;
		PetProc( PetObj );
	}

}

void NetSrv_MagicChild::R_PetMagicSwitchRequest( BaseItemObject* OwnerObj , int PetGUID , int ActiveMagicID , bool Active )
{
	RoleDataEx* Owner = OwnerObj->Role();
	RoleDataEx* Pet = Global::GetRoleDataByGUID( PetGUID );

	if( Pet == NULL || Pet->IsPet( Owner ) == false  )
	{
		//Owner->Msg( "找不到寵物" );
//		Owner->Net_GameMsgEvent( EM_GameMessageEvent_MagicError_PetNotFound );
		return;
	}
	PetStruct& PetInfo = *( Owner->GetPetInfo( Pet ) );

	//檢查是否有此法術
	for( int i = 0 ; i < PetInfo.Skill.Size() ; i++ )
	{
		PetSkillStruct& Skill = PetInfo.Skill[i];

		if( Skill.MagicColID == ActiveMagicID )
		{
			Skill.IsActive = Active;
			S_PetMagicSwitchResult( Owner->GUID() , PetGUID , Pet->TempData.SummonPet.Type , ActiveMagicID , Active );
			break;
		}
	}	

	S_PetMagicSwitchResult( Owner->GUID() , PetGUID , Pet->TempData.SummonPet.Type , ActiveMagicID , Active );
	
}

void NetSrv_MagicChild::R_CancelMagicRequest	( BaseItemObject* OwnerObj )
{
	RoleDataEx* Owner = OwnerObj->Role();
	if( Owner->TempData.Magic.State != EM_MAGIC_PROC_STATE_NORMAL )
		Owner->TempData.Magic.State = EM_MAGIC_PROC_STATE_INTERRUPT;
}

void NetSrv_MagicChild::R_BuffSkillRequest		( BaseItemObject* Obj , int BuffID , int BuffSkillID ,  int TargetID , float PosX , float PosY , float PosZ  )
{
	//檢查是否有此Buff
	RoleDataEx* Owner = Obj->Role();

	GameObjDbStructEx* BuffDB = Global::GetObjDB( BuffID );
	GameObjDbStructEx* BuffSkillDB = Global::GetObjDB( BuffSkillID );

	if(		BuffDB->IsMagicBase() == false 
		||	BuffSkillDB->IsMagicCollect() == false 
		||	BuffDB->MagicBase.Setting.BuffSkill == false )
	{
		S_BuffSkillResult( Owner->GUID() , false );
		return;
	}
	bool IsFind = false;
	for( int i = 0 ; i < _MAX_BUFFSKILL_COUNT_ ; i++ )
	{
		if( BuffDB->MagicBase.BuffSkill[ i ] == BuffSkillID )
		{
			IsFind = true;
			break;
		}
	}
	if( IsFind == false )
	{
		S_BuffSkillResult( Owner->GUID() , false );
		return;
	}

	int BuffLv = 0;
	IsFind = false;
	//查自己是否有buff
	for( int i = 0 ; i < Owner->BaseData.Buff.Size() ; i++ )
	{
		BuffBaseStruct& Buff = Owner->BaseData.Buff[i];
		if( Buff.BuffID == BuffID )
		{
			IsFind = true;
			BuffLv = Buff.Power;
			break;
		}
	}

	if( IsFind == false )
	{
		S_BuffSkillResult( Owner->GUID() , false );
		return;
	}	

	GameObjDbStructEx* SpellMagicCol = Global::GetObjDB( BuffSkillID );
	BaseItemObject* WagonObj = Global::GetObj( Owner->TempData.AttachObjID );
	if( WagonObj != NULL && SpellMagicCol != NULL )
	{
		if( SpellMagicCol->MagicCol.IsOnWagonEnabled == false )
			return;
	}

	MagicProcessClass::SpellMagic( Owner->GUID() , TargetID , PosX , PosY , PosZ  , BuffSkillID , BuffLv );	

	S_BuffSkillResult( Owner->GUID() , true );
}