#include "Global.h"
#include "../NetWalker_Member/NetWakerGSrvInc.h"
#include "MagicProc/MagicProcess.h"
#include <math.h>
//---------------------------------------------------------------------------------------------
void Global::ResetAttackSchedular(RoleDataEx* Owner)
{
	if (Owner->IsPlayer())
		return;

	BaseItemObject* OwnerClass = BaseItemObject::GetObj(Owner->GUID());

	OwnerClass->SimpleSchedular().Delete(Owner->TempData.BackInfo.HEvent_Ai, OwnerClass);
	OwnerClass->SimpleSchedular().Delete(Owner->TempData.BackInfo.HEvent_Move, OwnerClass);

	Owner->TempData.BackInfo.HEvent_Ai = OwnerClass->SimpleSchedularReg(AIProc, 0);
	Owner->TempData.BackInfo.HEvent_Move = OwnerClass->SimpleSchedularReg(MoveProc, 0);
}
void Global::PlayerRegProc(BaseItemObject* OwnerClass)
{
	OwnerClass->SimpleSchedularReg(AttackProc);
	OwnerClass->SimpleSchedularReg(PlayerMagicProc);
}
void Global::NPCRegProc(BaseItemObject* OwnerClass)
{
	//int		HEvent_Ai;
	//int		HEvent_Move;
	RoleDataEx* Owner = OwnerClass->Role();

	Owner->TempData.BackInfo.HEvent_Ai = OwnerClass->SimpleSchedularReg(AIProc);
	Owner->TempData.BackInfo.HEvent_Move = OwnerClass->SimpleSchedularReg(MoveProc);
	OwnerClass->SimpleSchedularReg(AttackProc);
	OwnerClass->SimpleSchedularReg(NPCMagicProc);
}

void Global::MineRegProc(BaseItemObject* OwnerClass)
{

	OwnerClass->SimpleSchedularReg(MineGenerateProc);
}


void Global::QuestObjRegProc(BaseItemObject* OwnerClass)
{
	RoleDataEx* Owner = OwnerClass->Role();

	Owner->TempData.BackInfo.HEvent_Ai = OwnerClass->SimpleSchedularReg(AIProc);
	Owner->TempData.BackInfo.HEvent_Move = OwnerClass->SimpleSchedularReg(MoveProc);

	//	OwnerClass->SimpleSchedularReg( MoveProc );
	//	OwnerClass->SimpleSchedularReg( AIProc );
	OwnerClass->SimpleSchedularReg(AttackProc);
	OwnerClass->SimpleSchedularReg(NPCMagicProc);
}

void Global::ItemRegProc(BaseItemObject* OwnerClass)
{

}
void Global::RemoveAllProc(BaseItemObject* OwnerClass)
{

}
//---------------------------------------------------------------------------------------------
INT64  Global::PetProc(BaseItemObject* PetObj)
{
	RoleDataEx* Target;
	RoleDataEx* Pet = PetObj->Role();
	BaseItemObject* PetOwnerObj = BaseItemObject::GetObj(Pet->OwnerGUID());
	int         Ret = 5;
	if (PetOwnerObj == NULL || PetOwnerObj->Role()->IsDead())
	{
		PetObj->Destroy("SYS PetProc");
		return 0;
	}
	RoleDataEx* PetOwner = PetOwnerObj->Role();

	if (PetOwner->DBID() != Pet->OwnerDBID())
	{
		PetObj->Destroy("SYS PetProc");
		return 0;
	}


	PetStruct* PetInfo = PetOwner->GetPetInfo(Pet);//&PetOwner->TempData.SummonPet.Info[ Pet->TempData.SummonPet.Type ]; 
	PetStruct  TempPetInfo;

	Pet->TempData.Move.RetX = PetOwner->X();
	Pet->TempData.Move.RetZ = PetOwner->Z();

	//檢查是否為護駕
	if (PetInfo->PetID != Pet->GUID())
	{
		PetInfo = &TempPetInfo;
	}

	//如果與主人距離過遠則移動到主人身邊
	if (Pet->Length(PetOwner) >= _DEF_NPC_FIGHT_RANGE_ + 200)
	{
		Pet->SetAtkFolOff();
		Pet->SetFollow(PetOwner);
		Pet->StopMove();

		if (Pet->Length(PetOwner) >= _DEF_NPC_FIGHT_RANGE_ + 300)
			Global::ChangeZone(Pet->GUID(), RoleDataEx::G_ZoneID, Pet->RoomID(), PetOwner->X(), PetOwner->Y(), PetOwner->Z(), 0);

		return 20;
	}


	//////////////////////////////////////////////////////////////////////////
	if (PetObj->AI()->EnableAI())
	{
		//寵物法術處理
		for (int i = 0; i < PetInfo->Skill.Size(); i++)
		{
			PetSkillStruct& Skill = PetInfo->Skill[i];
			if (Skill.IsActive == false)
				continue;

			if (MagicProcessClass::SpellMagic(Pet->GUID(), Pet->AttackTargetID(), Pet->X(), Pet->Y(), Pet->Z(), Skill.MagicColID, Skill.MagicLv) != false)
				break;
		}

	}



	//////////////////////////////////////////////////////////////////////////
	int ModeBk = Pet->BaseData.Mode._Mode;
	switch (PetInfo->Action)
	{
	case EM_PET_NONE:

		break;
	case EM_PET_STOP:
		Pet->BaseData.Mode.Move = false;
		PetInfo->Action = EM_PET_NONE;
		Pet->StopMove();
		break;
	case EM_PET_FOLLOW:
	{
		Pet->BaseData.Mode.Move = true;
		if (Pet->IsAttack() != false)
			break;

		if (PetObj->AI()->EnableAI() == false)
			break;

		if (Pet->BaseData.Mode.Fight != false && Pet->BaseData.Mode.Strikback != false)
		{
			if (PetOwner->IsAttack())
			{
				Target = Global::GetRoleDataByGUID(PetOwner->TempData.AI.TargetID);
				if (Target != NULL)
				{
					if (PetOwner->Length(Target) < _DEF_NPC_FIGHT_RANGE_
						&& St()->LuaLineBlock[Target->RoomID()].CheckCollion(Pet->X(), Pet->Z(), Target->X(), Target->Z()) == false)
						Pet->SetAttack(Target);
					break;
				}

			}
			//如果主人被打
			else if (PetOwner->IsAttackMode())
			{
				Target = PetOwner->SearchHate();
				if (Target != NULL)
				{
					if (St()->LuaLineBlock[Target->RoomID()].CheckCollion(Pet->X(), Pet->Z(), Target->X(), Target->Z()) == false)
					{
						Pet->SetAttack(Target);
						break;
					}
				}
			}
		}

		//Pet->SetFollow( PetOwner );
		Pet->TargetID(-1);
		Pet->TempData.Attr.Action.Follow = true;

		if (PetOwner->IsPlayer())
		{
			if (Pet->Length(PetOwner) >= 30 || PetOwner->IsMove())
				Pet->IsWalk(false);
			else if (Pet->Length(PetOwner) <= 20)
				Pet->IsWalk(true);
		}
		else
		{
			/*
			if( Pet->Length( PetOwner ) >= 60 || PetOwner->IsMove() )
				Pet->IsWalk( false );
			else if( Pet->Length( PetOwner ) <= 20 )
				Pet->IsWalk(true );
			*/
			Pet->IsWalk(PetOwner->IsWalk());
		}

		break;

	}
	case EM_PET_ATTACK:
	{
		//Pet->BaseData.Mode.Fight    = true;
		Target = Global::GetRoleDataByGUID(PetInfo->TargetID);
		PetInfo->TargetID = -1;
		if (Target != NULL)
		{

			if (St()->LuaLineBlock[Target->RoomID()].CheckCollion(Pet->X(), Pet->Z(), Target->X(), Target->Z()) == false)
			{
				if (Pet->SetAttack(Target))
					Pet->BaseData.Mode.Move = true;
			}
		}


		if (Pet->TempData.RandomMove.Enable != false)
			PetInfo->Action = EM_PET_GUARD;
		else
			PetInfo->Action = EM_PET_FOLLOW;
		break;
	}
	case EM_PET_ATTACKOFF:
	{
		Pet->BaseData.Mode.Strikback = false;
		if (Pet->IsAttack() != false)
		{
			Pet->SetAtkFolOff();
			Pet->StopMoveNow();
		}
		if (Pet->IsSpell() != false)
			Pet->TempData.Magic.State = EM_MAGIC_PROC_STATE_INTERRUPT;

		if (Pet->TempData.RandomMove.Enable != false)
			PetInfo->Action = EM_PET_GUARD;
		else
			PetInfo->Action = EM_PET_FOLLOW;
		break;

	}
	/*	case EM_PET_STRICKBACK:
			Pet->BaseData.Mode.Strikback    = true;
			PetInfo->Action = EM_PET_FOLLOW;
			break;
		case EM_PET_STRICKBACK_OFF:
			Pet->BaseData.Mode.Strikback    = false;
			PetInfo->Action = EM_PET_FOLLOW;
			break;
	*/
	case EM_PET_GUARD:
		// 設定Random Move
		break;
	}

	if (ModeBk != Pet->BaseData.Mode._Mode)
	{
		NetSrv_RoleValue::S_ObjMode(PetOwner->GUID(), Pet->GUID(), Pet->BaseData.Mode);
	}


	return Ret;

}
INT64  Global::NPCMagicProc(BaseItemObject* OwnerObj)
{
	MagicProcessClass::Process(OwnerObj->Role(), &OwnerObj->Role()->TempData.Magic);
	return 10;
}

INT64  Global::PlayerMagicProc(BaseItemObject* OwnerObj)
{
	return MagicProcessClass::Process(OwnerObj->Role(), &OwnerObj->Role()->TempData.Magic);
	//    return 5;
}



//放入 SimpleSchedular 排程的函式
INT64      Global::AttackProc(BaseItemObject* OwnerObj)
{
	//////////////////////////////////////////////////////////////////////////
	//是否在戰鬥中
	//////////////////////////////////////////////////////////////////////////
	int     DLV = 0;
	RoleDataEx* Owner = OwnerObj->Role();
	if (Owner->IsAttack() == false)
	{
		if (Owner->IsNPC())
			return 20;
		return 3;
	}
	Owner->TempData.BackInfo.LastPhyHitPoint = 0;
	//////////////////////////////////////////////////////////////////////////

	BaseItemObject* TargetObj;
	RoleDataEx* Target = NULL;
	TargetObj = BaseItemObject::GetObj(Owner->TempData.AI.TargetID);

	RoleValueData& Attr = Owner->TempData.Attr;

	if (TargetObj != NULL)
		Target = TargetObj->Role();

	//////////////////////////////////////////////////////////////////////////
	// 自己與目標狀態檢查
	//////////////////////////////////////////////////////////////////////////

	//檢查目標可否攻擊
	if (Target == NULL
		|| Target->IsDead())
	{

		NetSrv_Talk::GameMsgEvent(Owner->GUID(), EM_GameMessageEvent_AttackError_TargetDead);
		//		Owner->Msg( "目標已死亡!!" );
		if (Owner->IsNPC() &&
			(Owner->OwnerGUID() <= 0 || Owner->BaseData.Mode.Strikback != false))
		{
			//從仇恨表內找一個目標
			FindNewHateTarget(Owner);
		}
		else
		{
			Owner->SetAtkFolOff();
		}
		return 10;
	}

	if (Target->BaseData.Mode.Fight == false)
	{
		if (Owner->IsNPC())
		{
			//從仇恨表內找一個目標
			FindNewHateTarget(Owner);
		}
		else
		{
			Owner->SetAtkFolOff();
		}
		return 10;
	}


	if (Target->TempData.Attr.Effect.PlayDead && Owner->IsNPC() && Owner->TempData.Attr.Effect.DetectPlayDead == false)
	{
		Owner->SetAtkFolOff();
		return 10;
	}

	if (Target->IsSpell())
	{
		//取得施法的資料
		GameObjDbStructEx* MagicCol = Global::GetObjDB(Target->TempData.Magic.MagicCollectID);
		if (MagicCol->IsMagicCollect() && MagicCol->MagicCol.IsLockTarget)
		{
			Target->BaseData.Pos.Dir = Target->CalDir(Owner);
		}
	}

	/*
	if( Target->TempData.Attr.Effect.Stop != false && rand() % 4 == 0 )
	{
		FindNewHateTarget( Owner );
	}
	*/
	if (Owner->IsDead())
	{
		Owner->SetAtkFolOff();
		return 10;
	}

	//如果正在施法
	if (Owner->IsSpell())
	{
		return 4;
	}

	if (Target->TempData.Attr.Effect.AI_LowAttackPriority != false /*&& rand() % 4 == 0*/)
	{
		FindNewHateTarget(Owner);
	}


	//檢查是否要延長攻擊等待
	if (Owner->TempData.AI.AddAtkTime > 0)
	{
		int Temp = Owner->TempData.AI.AddAtkTime;
		Owner->TempData.AI.AddAtkTime = 0;

		if (Temp > 10)
		{
			if (Temp > 40)
				Temp = 40;
			Owner->TempData.AI.AddAtkTime = Temp - 10;
			Temp = 10;
		}
		return Temp;
	}
	if (Owner->TempData.Attr.Effect.UsePhyDisable != false)
	{
		return 10;
	}

	if (Owner->CheckAttackable(Target) == false)
	{
		NetSrv_Talk::GameMsgEvent(Owner->GUID(), EM_GameMessageEvent_AttackError_Target);
		//		Owner->Msg( "目標不可攻擊!!" );
		Global::FindNewHateTarget(Owner);
		return 10;
	}

	/*
	if( Target->TempData.Attr.Effect.PhyInvincible  != false )
	{
		NetSrv_Talk::GameMsgEvent( Owner->GUID() , EM_GameMessageEvent_AttackError_PhyDisable );
		//Owner->Msg( "[狀態]無法攻擊!!" );
		if( Owner->IsNPC() )
		{
			Owner->FindNewHateTarget( );
			return 10;
		}
	}
	*/
	//////////////////////////////////////////////////////////////////////////
	//	檢查要用主手還是副手攻擊
	//////////////////////////////////////////////////////////////////////////

	AttackHandTypeENUM  AttackHandType;   // 0 主手 1副手 2 雙手時間都還沒到

	if (Attr.MainWeaponType != EM_Weapon_None
		&& RoleDataEx::G_Clock >= Owner->TempData.AI.NextMainAtk)
	{
		if (Owner->CheckMainAttackRange(Target) == false)
		{
			if (Owner->IsPlayer())
			{
				NetSrv_Talk::GameMsgEvent(Owner->GUID(), EM_GameMessageEvent_AttackError_TargetTooFar);
				return 4;
			}
			return 1;
		}
		AttackHandType = EM_AttackHandType_Main;
		Owner->TempData.AI.NextMainAtk = RoleDataEx::G_Clock + int(Attr.Fin.Main_ATKSpeed);
	}
	else if (Attr.SecWeaponType != EM_Weapon_None
		&& RoleDataEx::G_Clock >= Owner->TempData.AI.NextSecAtk)
	{
		if (Owner->CheckSecAttackRange(Target) == false)
		{
			if (Owner->IsPlayer())
			{
				NetSrv_Talk::GameMsgEvent(Owner->GUID(), EM_GameMessageEvent_AttackError_TargetTooFar);
				return 4;
			}
			return 1;
		}
		Owner->TempData.AI.NextSecAtk = RoleDataEx::G_Clock + int(Attr.Fin.Sec_ATKSpeed);
		AttackHandType = EM_AttackHandType_Second;

	}
	else if (Attr.MainWeaponType == EM_Weapon_None && Attr.SecWeaponType == EM_Weapon_None)
	{
		return 10;
	}
	else
	{
		AttackHandType = EM_AttackHandType_None;
	}
	//////////////////////////////////////////////////////////////////////////
	//設定面朝目標
	//////////////////////////////////////////////////////////////////////////
	if (Owner->IsNPC() && Owner->BaseData.Mode.DisableRotate == false)
		Owner->BaseData.Pos.Dir = Owner->CalDir(Target);

	if (Owner->Length(Target) > 3)
	{
		//檢查攻擊面向
		if (Owner->CalAngle(Target) > 90)
		{
			if ((!Owner->IsMove() && !Target->IsMove())
				|| (Owner->Length(Target) > 20 && Target->IsAttackMode()))
			{
				NetSrv_Talk::GameMsgEvent(Owner->GUID(), EM_GameMessageEvent_AttackError_Dir);
				return 4;
			}
		}
	}

	float	AttackPower = 1.0f;
	bool	IsTargetSeeOwner = true;
	if (Target->CalAngle(Owner) > 135)
	{
		IsTargetSeeOwner = false;
		if (Target->IsAttack() == false)
			AttackPower = 1.3f;
		else
			AttackPower = 1.1f;
	}
	//////////////////////////////////////////////////////////////////////////
	//	下次攻擊時間計算
	//////////////////////////////////////////////////////////////////////////
	int MTime = Owner->TempData.AI.NextMainAtk - RoleDataEx::G_Clock;
	int LTime = Owner->TempData.AI.NextSecAtk - RoleDataEx::G_Clock;
	int RetTime;
	if (Attr.MainWeaponType != EM_Weapon_None && Attr.SecWeaponType != EM_Weapon_None)
		RetTime = __min(MTime, LTime);
	else if (Attr.MainWeaponType != EM_Weapon_None)
		RetTime = MTime;
	else
		RetTime = LTime;

	if (RetTime <= 0)
		RetTime = 5;

	if (RetTime >= 50)
		RetTime = 50;

	if (AttackHandType == EM_AttackHandType_None)
		return __max(RetTime, 1);

	//////////////////////////////////////////////////////////////////////////
	//	檢查是否命中
	//////////////////////////////////////////////////////////////////////////
	//如果目標在攻擊模式
	if (Target->IsAttack() || IsTargetSeeOwner != false)
	{
		//Owner->TempData.BackInfo.BuffClearTime.TargetOnAttack =true;
		//Owner->ClearBuffByEvent(  );
		MagicClearTimeStruct ClearEvent;
		ClearEvent._Value = 0;
		ClearEvent.TargetOnAttack = true;
		Owner->ClearBuffByEvent(ClearEvent);
	}
	//////////////////////////////////////////////////////////////////////////
	//	清除攻擊消失的Buff
	//////////////////////////////////////////////////////////////////////////
	Owner->TempData.BackInfo.BuffClearTime.Attack = true;
	Target->CalHate(Owner, 0, 1);

	ATTACK_DMGTYPE_ENUM DmgType;

	if (Target->TempData.Attr.Effect.BadPhyInvincible != false)
	{
		DmgType = EM_ATTACK_DMGTYPE_NO_EFFECT;
	}
	else
	{
		DmgType = Owner->CheckAtkDmgType(Target, AttackHandType);
	}


	if (DmgType == EM_ATTACK_DMGTYPE_MISS)
	{
		Owner->TempData.BackInfo.LastMissTime = RoleDataEx::G_SysTime;
	}
	if (DmgType == EM_ATTACK_DMGTYPE_DODGE)
	{
		Target->TempData.BackInfo.LastDodgeTime = RoleDataEx::G_SysTime;
		Owner->TempData.BackInfo.LastBeDodgeTime = RoleDataEx::G_SysTime;
	}
	if (DmgType == EM_ATTACK_DMGTYPE_HALF)
	{
		Target->TempData.BackInfo.LastParryTime = RoleDataEx::G_SysTime;
		Owner->TempData.BackInfo.LastBeParryTime = RoleDataEx::G_SysTime;
	}
	if (DmgType == EM_ATTACK_DMGTYPE_DOUBLE
		|| DmgType == EM_ATTACK_DMGTYPE_TRIPLE)
	{
		Owner->TempData.BackInfo.LastCriticalTime = RoleDataEx::G_SysTime;
		Target->TempData.BackInfo.LastBeCriticalTime = RoleDataEx::G_SysTime;
	}
	if (DmgType == EM_ATTACK_DMGTYPE_SHIELD_BLOCK)
	{
		Owner->TempData.BackInfo.LastShieldBlockTime = RoleDataEx::G_SysTime;
		Target->TempData.BackInfo.LastBeShieldBlockTime = RoleDataEx::G_SysTime;
	}
	if (DmgType != EM_ATTACK_DMGTYPE_NO_EFFECT && DmgType != EM_ATTACK_DMGTYPE_MISS)
	{
		if (!Target->IsAttack()
			&& Target->IsNPC()
			&& Target->CheckStrikback())
			Target->SetAttack(Owner);
	}

	switch (DmgType)
	{
	case EM_ATTACK_DMGTYPE_MISS:		    //MISS
	case EM_ATTACK_DMGTYPE_DODGE:		    //DODGE
	case EM_ATTACK_DMGTYPE_SHIELD_BLOCK:
	{
		NetSrv_AttackChild::SendRangeAttackInfo(Owner, Target->GUID()
			, DmgType, AttackHandType, 0, 0);

		Owner->OnAttack(Target, EM_OnAttackType_None, DmgType, 0, 0, 0);

		return RetTime;
	}

	case EM_ATTACK_DMGTYPE_NO_EFFECT:		//沒效果
	{
		NetSrv_AttackChild::SendRangeAttackInfo(Owner, Target->GUID()
			, DmgType, AttackHandType, 0, 0);

		//Owner->OnAttack( Target , -1 , DmgType );
		return __min(RetTime, 4);
	}
	}


	int     Damage;
	//    int     DamageRand;
	//設定目標
	Attr.Target = &(Target->TempData.Attr);

	//////////////////////////////////////////////////////////////////////////
	// 技能計算
	//////////////////////////////////////////////////////////////////////////
	//取主手的武器 耐久計算
	ItemFieldStruct* WeaponItem = NULL;
	int				 WeaponPos = 0;
	float			 WeaponSkill = Owner->Level() * 2.3f + 10;
	float			 TargetDefSkill = Target->GetSkillValue(EM_SkillValueType_Define) + 10;

	//計算攻方技能
//	if( Owner->TempData.Attr.Effect.Ride == false )
	{
		switch (AttackHandType)
		{
		case EM_AttackHandType_Main:
		{
			Damage = (int)Attr.MainAtkDamage(DmgType);
			//////////////////////////////////////////////////////////////////////////
			if (Owner->IsPlayer())
				WeaponSkill = Owner->GetWeaponSkillValue(Owner->TempData.Attr.MainWeaponType) + 10;
			//////////////////////////////////////////////////////////////////////////
			if (Target->IsPlayer() == false)
				Owner->AddWeaponSkillValue(Target->Level(), Attr.MainWeaponType, float(2 * Owner->TempData.Sys.KillRate));

			WeaponItem = &Owner->BaseData.EQ.MainHand;
			WeaponPos = EM_EQWearPos_MainHand;
		}

		break;
		case EM_AttackHandType_Second:
		{
			Damage = (int)Attr.SecAtkDamage(DmgType);
			//////////////////////////////////////////////////////////////////////////
			if (Owner->IsPlayer())
				WeaponSkill = Owner->GetWeaponSkillValue(Owner->TempData.Attr.SecWeaponType) + 10;
			//////////////////////////////////////////////////////////////////////////
			if (Target->IsPlayer() == false)
				Owner->AddWeaponSkillValue(Target->Level(), Attr.SecWeaponType, float(1 * Owner->TempData.Sys.KillRate));
			WeaponItem = &Owner->BaseData.EQ.SecondHand;
			WeaponPos = EM_EQWearPos_SecondHand;
		}
		break;
		}

	}
	//////////////////////////////////////////////////////////////////////////
	float DSkillRate = 1 + (WeaponSkill - TargetDefSkill) / __max(WeaponSkill, TargetDefSkill) * 0.3f;
	Damage = int(Damage * DSkillRate);
	//////////////////////////////////////////////////////////////////////////
	if (Owner->IsPlayer() != false && Target->IsPlayer() != false)
	{
		Damage = int(Damage * RoleDataEx::GlobalSetting.PK_DamageRate);
	}
	else if (Owner->IsNPC() != false && Target->IsPlayer() != false)
	{
		Damage = int(Damage * RoleDataEx::GlobalSetting.NPC_DamageRate);
	}

	{
		RoleDataEx* OTarget = Target->GetOrgOwner();
		if (OTarget->IsPlayer())
			Damage = Damage * Owner->TempData.Attr.Mid.BodyRate[EM_WearEqType_AttackPlayerPowerRate];
		else
			Damage = Damage * Owner->TempData.Attr.Mid.BodyRate[EM_WearEqType_AttackNPCPowerRate];

		RoleDataEx* OrgOwner = Owner->GetOrgOwner();
		if (OrgOwner->IsPlayer())
			Damage = Damage * Target->TempData.Attr.Mid.BodyRate[EM_WearEqType_PlayerDefRate];
		else
			Damage = Damage * Target->TempData.Attr.Mid.BodyRate[EM_WearEqType_NPCDefRate];
	}


	//耐久計算
	if (Owner->IsPlayer())
	{
		int DecDurable = Owner->TempData.Sys.KillRate * 2;
		Owner->EqDurableProcess((EQWearPosENUM)WeaponPos, DecDurable);
	}
	if (Target->IsPlayer())
	{
		int DecDurable = Target->TempData.Sys.KillRate;
		//裝備耐久計算
		for (int i = EM_EQWearPos_Head; i < EM_EQWearPos_MaxCount; i++)
		{
			Target->EqDurableProcess((EQWearPosENUM)i, DecDurable);
		}
	}
	//////////////////////////////////////////////////////////////////////////


	//計算守方技能
	if (DmgType == EM_ATTACK_DMGTYPE_HALF
		|| DmgType == EM_ATTACK_DMGTYPE_NORMAL
		|| DmgType == EM_ATTACK_DMGTYPE_DOUBLE
		|| DmgType == EM_ATTACK_DMGTYPE_TRIPLE
		|| DmgType == EM_ATTACK_DMGTYPE_SHIELD_BLOCK)
	{
		if (Owner->IsPlayer() == false)
			Target->AddSkillValue(Owner->Level(), EM_SkillValueType_Define, float(1 * Target->TempData.Sys.KillRate));
	}

	//////////////////////////////////////////////////////////////////////////
	// 等級差　與　亂數影響
	//////////////////////////////////////////////////////////////////////////
	//等級差威力影響比
	/*
	const float DLvRateNPC[11] = { 0.77f , 0.81f , 0.85f, 0.9f ,0.95f ,1.0f , 1.05f , 1.1f , 1.157f , 1.21f, 1.27f };
	const float DLvRatePlayer[11] = { 0.77f , 0.81f , 0.85f, 0.9f ,0.95f ,1.0f , 1.0f , 1.0f , 1.0f , 1.0f, 1.0f };
	//const float DLvRate[11] = { 1 , 1 , 1, 1 , 1 ,1.0f , 1.05f , 1.1f , 1.157f , 1.21f, 1.27f };
	DLV = Owner->Level() - Target->Level() + 5;
	if( DLV > 10 )
		DLV = 10;
	else if( DLV < 0 )
		DLV = 0;

	if( Owner->IsPlayer() )
	{
		Damage = int( Damage * AttackPower * DLvRatePlayer[ DLV ] ) ;
	}
	else
	{
		Damage = int( Damage * AttackPower * DLvRateNPC[ DLV ] ) ;
	}
	*/
	int PowerDLv = Owner->Level() - Target->Level();
	Damage = int(Damage * AttackPower * Global::DLvRate(PowerDLv, Owner->IsPlayer()));

	/*
	DLV = Owner->Level() - Target->Level();
	DLV *= (int)RoleValueData::FixArg.DMG_DLV;


	if( DLV > 0 )
		DamageRand = rand() % int( DLV )  ;
	else
		DamageRand = 0 ;

	Damage = int( Damage * AttackPower ) + DamageRand;
*/

	if (Damage <= 1)
		Damage = 1 + rand() % 2;

	int	OrgDamage = Damage;
	//////////////////////////////////////////////////////////////////////////
	//計算對方是否有防守狀態or 是否可以設成防守狀態
	if (Target->TempData.Attr.AssistMagicSetting.MagicShield == false)
		//||	( DmgType != 	EM_ATTACK_DMGTYPE_HALF	&&	DmgType != 	EM_ATTACK_DMGTYPE_NORMAL )	)
	{
	}
	else
	{
		Target->MagicShieldAbsorbProc(Owner, NULL, Damage);
		if (Damage == 0)
		{
			DmgType = EM_ATTACK_DMGTYPE_ABSORB;
		}
	}

	NetSrv_AttackChild::SendRangeAttackInfo(Owner, Target->GUID(), DmgType, AttackHandType, Damage, OrgDamage);
	AddHP(Target, Owner, float(Damage * -1));

	if (AttackHandType == EM_AttackHandType_Main)
		Owner->OnAttack(Target, EM_OnAttackType_MainHand, DmgType, Damage * -1, 0, 0);
	else if (AttackHandType == EM_AttackHandType_Second)
		Owner->OnAttack(Target, EM_OnAttackType_OffHand, DmgType, Damage * -1, 0, 0);

	Owner->TempData.BackInfo.LastPhyHitPoint = Damage * -1;


	//處理武器上當普攻產生的法術
	if (Damage != 0)
	{

		// Server 產生傷害事件
		if (Target->IsPlayer())
		{
			CNetSrv_Script_Child::OnEvent_RoleBeHit(Target);
		}


		switch (AttackHandType)
		{
		case EM_AttackHandType_Main:
			if (rand() % 100 < Attr.BaseEQ.Main_OnAttackMagicRate)
			{
				MagicProcInfo  MagicInfo;
				MagicInfo.Init();
				MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
				MagicInfo.TargetID = Target->GUID();					//目標
				MagicInfo.MagicCollectID = Attr.BaseEQ.Main_OnAttackMagicID;	//法術
				MagicInfo.MagicLv = 0;
				MagicProcessClass::SysSpellMagic(Owner, &MagicInfo, int(Owner->TempData.Attr.Fin.Main_ATKSpeed / 3));
			}

			if (rand() % 100 < Attr.Mid.OnAttackMagicRate)
			{
				MagicProcInfo  MagicInfo;
				MagicInfo.Init();
				MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
				MagicInfo.TargetID = Target->GUID();					//目標
				MagicInfo.MagicCollectID = Attr.Mid.OnAttackMagicID;	//法術
				MagicInfo.MagicLv = 0;
				MagicProcessClass::SysSpellMagic(Owner, &MagicInfo, int(Owner->TempData.Attr.Fin.Main_ATKSpeed / 3));
			}

			break;
		case EM_AttackHandType_Second:
			if (rand() % 100 < Attr.BaseEQ.Sec_OnAttackMagicRate)
			{
				MagicProcInfo  MagicInfo;
				MagicInfo.Init();
				MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
				MagicInfo.TargetID = Target->GUID();				//目標
				MagicInfo.MagicCollectID = Attr.BaseEQ.Sec_OnAttackMagicID;	//法術
				MagicInfo.MagicLv = 0;
				MagicProcessClass::SysSpellMagic(Owner, &MagicInfo, int(Owner->TempData.Attr.Fin.Sec_ATKSpeed / 3));
			}

			if (rand() % 100 < Attr.Mid.OnAttackMagicRate)
			{
				MagicProcInfo  MagicInfo;
				MagicInfo.Init();
				MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
				MagicInfo.TargetID = Target->GUID();					//目標
				MagicInfo.MagicCollectID = Attr.Mid.OnAttackMagicID;	//法術
				MagicInfo.MagicLv = 0;
				MagicProcessClass::SysSpellMagic(Owner, &MagicInfo, int(Owner->TempData.Attr.Fin.Sec_ATKSpeed / 3));
			}

			break;
		}


	}


	if (Target->BaseData.Sex < EM_Sex_BigMonster && rand() % 100 > 20)
	{
		if (IsTargetSeeOwner == false && Target->IsMove() && Owner->Level() >= 20 && Damage != 0)
		{

			BuffBaseStruct* RetBuf = Owner->AssistMagic(Target, RoleDataEx::BaseMagicList[EM_BaseMagic_Slow], 0, false, 5);

			if (RetBuf != NULL)
			{
				NetSrv_MagicChild::SendRange_AddBuffInfo(Target->GUID(), Owner->GUID(), RoleDataEx::BaseMagicList[EM_BaseMagic_Slow], 0, 5);
				Target->TempData.UpdateInfo.Recalculate_Buff = true;
				Target->OnTimeProc(true);
			}
		}
	}

	//看是否為連續攻擊
	if (Owner->TempData.AI.DoubleAttackCount < 1 && rand() % 100 < Owner->TempData.Attr.Mid.Body[EM_WearEqType_DoubleAttack])
	{
		Owner->TempData.AI.DoubleAttackCount++;
		switch (AttackHandType)
		{
		case EM_AttackHandType_Main:
			Owner->TempData.AI.NextMainAtk = RoleDataEx::G_Clock;
			RetTime = 1;
			break;
		case EM_AttackHandType_Second:
			Owner->TempData.AI.NextSecAtk = RoleDataEx::G_Clock;
			RetTime = 1;
			break;
		}
	}
	else
		Owner->TempData.AI.DoubleAttackCount = 0;

	//法術傷害
	switch (AttackHandType)
	{
	case EM_AttackHandType_Main:
	{
		for (int i = 0; i < EM_MagicResist_MAXCOUNT; i++)
		{
			if (Owner->TempData.Attr.Fin.Main_MagicAtk[i] != 0)
			{
				MagicProcInfo  MagicInfo;
				MagicInfo.Init();
				MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
				MagicInfo.TargetID = Target->GUID();					//目標
				MagicInfo.MagicCollectID = g_ObjectData->SysValue().ElementPowerMagicID[i];	//法術
				MagicInfo.MagicLv = int(Owner->TempData.Attr.Fin.Main_MagicAtk[i] - 1);
				MagicProcessClass::SysSpellMagic(Owner, &MagicInfo, int(Owner->TempData.Attr.Fin.Main_ATKSpeed / 3));
			}
		}
	}
	break;
	case EM_AttackHandType_Second:
	{
		for (int i = 0; i < EM_MagicResist_MAXCOUNT; i++)
		{
			if (Owner->TempData.Attr.Fin.Sec_MagicAtk[i] != 0)
			{
				MagicProcInfo  MagicInfo;
				MagicInfo.Init();
				MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
				MagicInfo.TargetID = Target->GUID();					//目標
				MagicInfo.MagicCollectID = g_ObjectData->SysValue().ElementPowerMagicID[i];	//法術
				MagicInfo.MagicLv = int(Owner->TempData.Attr.Fin.Sec_MagicAtk[i] - 1);
				MagicProcessClass::SysSpellMagic(Owner, &MagicInfo, int(Owner->TempData.Attr.Fin.Sec_ATKSpeed / 3));
			}
		}
	}
	break;
	}

	return RetTime;
}
//---------------------------------------------------------------------------------------------
INT64      Global::MoveProc(BaseItemObject* OwnerObj)
{
	const int BaseProcTime = 2;
	RoleDataEx* Owner = (RoleDataEx*)OwnerObj->Role();

	if (Owner->BaseData.Mode.Move == false)
		return 10;

	if (_St->IsNPCMove == false)
		return 10;

	//如果是attach 到物件上 不處理移動
	if (Owner->TempData.AttachObjID > 0)
	{
		OwnerObj->Path()->ClearPath();
		return 10;
	}


	if (Owner->IsDead())
		return 20;


	float   NX, NY, NZ;
	int     Ret = BaseProcTime;

	if (Owner->TempData.Move.AddeDelay > 0)
	{
		Ret = Owner->TempData.Move.AddeDelay;
		Owner->TempData.Move.AddeDelay = 0;
		return Ret;
	}

	if (Owner->TempData.Move.ProcDelay > 0)
	{
		Owner->TempData.Move.CliLastMoveTime = RoleDataEx::G_SysTime;
		Owner->TempData.BackInfo.BuffClearTime.Move = true;
		if (Owner->TempData.AroundPlayer == 0)
		{
			Ret = Owner->TempData.Move.ProcDelay - BaseProcTime;
			Owner->TempData.Move.ProcDelay = 0;
			Owner->BaseData.Pos.X = Owner->TempData.Move.Tx;
			Owner->BaseData.Pos.Z = Owner->TempData.Move.Tz;
			Owner->BaseData.Pos.Y = Owner->TempData.Move.Ty;
			Owner->TempData.Move.Dx = 0;
			Owner->TempData.Move.Dy = 0;
			Owner->TempData.Move.Dz = 0;
			return __max(1, Ret);
		}
		else if (Owner->TempData.Move.ProcDelay > BaseProcTime * 2)
		{
			Owner->TempData.Move.ProcDelay -= BaseProcTime;
			Ret = BaseProcTime;
			Owner->BaseData.Pos.X += Owner->TempData.Move.Dx * BaseProcTime;
			Owner->BaseData.Pos.Z += Owner->TempData.Move.Dz * BaseProcTime;
			Owner->BaseData.Pos.Y += Owner->TempData.Move.Dy * BaseProcTime;

			return Ret;
		}
		else if (Owner->TempData.Move.ProcDelay > BaseProcTime)
		{   //讓最後一步都留 BaseProcTime 的距離
			Ret = Owner->TempData.Move.ProcDelay - BaseProcTime;
			Owner->TempData.Move.ProcDelay = BaseProcTime;
			Owner->BaseData.Pos.X += Owner->TempData.Move.Dx * Ret;
			Owner->BaseData.Pos.Z += Owner->TempData.Move.Dz * Ret;
			Owner->BaseData.Pos.Y += Owner->TempData.Move.Dy * Ret;
			return Ret;
		}
		else
		{
			//到達目標 並搜尋下一步 (* 所以沒有 Return 要處理下去)
			Owner->TempData.Move.ProcDelay = 0;
			Owner->BaseData.Pos.X = Owner->TempData.Move.Tx;
			Owner->BaseData.Pos.Z = Owner->TempData.Move.Tz;
			Owner->BaseData.Pos.Y = Owner->TempData.Move.Ty;
			Owner->TempData.Move.Dx = 0;
			Owner->TempData.Move.Dy = 0;
			Owner->TempData.Move.Dz = 0;
		}

	}
	//移動被封著
	if (Owner->TempData.Attr.Effect.Stop)
		return 10;

	//---------------------------------------------------------------------------------------------
	//設定NPC 下一步移動的位置
	float Len = 120;
	if (Owner->IsAttack())
		Len = 30;

	if (OwnerObj->Path()->NextPoint(NX, NY, NZ, Len) == ENUM_Path_Search_End)
	{
		if (Owner->TempData.AroundPlayer == 0)
			return 50;
		else
			return 3;
	}
	if (Owner->IsSpell())
	{
		GameObjDbStructEx* MagicColDB = Global::GetObjDB(Owner->TempData.Magic.MagicCollectID);
		if (MagicColDB && MagicColDB->MagicCol.Flag.Interrupt_OnMove == true)
		{
			Owner->StopMoveNow();

			return 10;
		}
	}
	//---------------------------------------------------------------------------------------------
	//	尋找跟隨坐落點
	//---------------------------------------------------------------------------------------------
	//如果目標點有人設定
	SpaceStruct		Pos;

	Pos.X = int(Owner->TempData.Move.Tx / _Def_Map_BaseSize_);
	Pos.Z = int(Owner->TempData.Move.Tz / _Def_Map_BaseSize_);
	Pos.RoomID = Owner->BaseData.RoomID;
	Pos.Y = 0;
	//---------------------------------------------------------------------------------------------
	//	尋找坐落點
	//---------------------------------------------------------------------------------------------
	map< SpaceStruct, RoleData* >::iterator Iter;

	if (Pos.X != int(NX / _Def_Map_BaseSize_)
		|| Pos.Z != int(NZ / _Def_Map_BaseSize_))
	{

		Iter = _St->PosList.find(Pos);

		if (Iter != _St->PosList.end())
		{
			//重設 NX NZ
			//	NX -= 80; NZ -= 80;
			SearchNode(Owner, NX, NZ);
		}
	}

	//---------------------------------------------------------------------------------------------	
	if (Owner->BaseData.Mode.DisableRotate != false)
		Owner->MoveTarget(NX, NY, NZ, Owner->Dir());
	else
		Owner->MoveTarget(NX, NY, NZ, Owner->CalDir(NX - Owner->X(), NZ - Owner->Z()));

	//通知Client移動
	NetSrv_MoveChild::MoveObj(OwnerObj->Role());

	if (Owner->TempData.Move.ProcDelay > BaseProcTime)
		Ret = BaseProcTime;
	else
		Ret = __max(1, Owner->TempData.Move.ProcDelay);
	return Ret;
}
//---------------------------------------------------------------------------------------------
int      Global::MoveAIProc(BaseItemObject* OwnerObj)
{
	return 10;
}

//---------------------------------------------------------------------------------------------
INT64      Global::AIProc(BaseItemObject* OwnerObj)
{
	if (OwnerObj->Role()->IsDead())
		return 10;

	//檢查某些狀況是否不能處理AI
	if (OwnerObj->Role()->IsSpell())
		return 2;

	int Ret = OwnerObj->AI()->AIProc();
	if (Ret <= 0)
	{
		Ret = 10;
	}
	if (Ret >= 20)
		Ret = 20;
	return Ret;
}
//---------------------------------------------------------------------------------------------
INT64      Global::MineGenerateProc(BaseItemObject* OwnerObj)
{
	RoleDataEx* pRole = OwnerObj->Role();
	GameObjDbStructEx* pObj = Global::GetObjDB(pRole->BaseData.ItemInfo.OrgObjID);

	int iGenerate = int(pObj->GenerateRate * 10);

	if (pRole->TempData.iGatherNumber >= pObj->MaxWorldCount)
		return iGenerate;

	// 增加一個量
	pRole->TempData.iGatherNumber++;

	// 再次推入排程
	return iGenerate;
}
//---------------------------------------------------------------------------------------------