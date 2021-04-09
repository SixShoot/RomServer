#pragma once

#include "NetSrv_Magic.h"

class NetSrv_MagicChild : public NetSrv_Magic
{
public:
    static bool Init();
    static bool Release();

    virtual void R_SpellRequest       ( BaseItemObject* OwnerObj , int OwnerID , int TargetID , int MagicID , int MagicPos );
    virtual void R_SpellRequest_Pos   ( BaseItemObject* OwnerObj , int OwnerID , float X , float Y , float Z , int MagicID , int MagicPos );

    virtual void R_SetMagicPoint      ( BaseItemObject* OwnerObj , int MagicID , int MagicPos );
    virtual void R_SetMagicPointEx    ( BaseItemObject* OwnerObj , int MagicID , int MagicPos , int Times );
	virtual void R_CancelAssistMagic  ( BaseItemObject* OwnerObj , int MagicID );

	virtual void R_PetCommand			( BaseItemObject* OwnerObj , PET_ACTION_ENUM Action , int TargetID , int MagicID , int PetGUID );
	virtual void R_PetMagicSwitchRequest( BaseItemObject* OwnerObj , int PetGUID , int ActiveMagicID , bool Active );
	virtual void R_CancelMagicRequest	( BaseItemObject* OwnerObj );
	virtual void R_BuffSkillRequest		( BaseItemObject* Obj , int BuffID , int BuffSkillID ,  int TargetID , float PosX , float PosY , float PosZ );
    //---------------------------------------------------------------------------
   
    //---------------------------------------------------------------------------
    //通知週圍的 A 用法術攻擊打(or治療) B
    static void MagicAttackRange	( int OwnerID , int MagicBaseID , int SerialID , MagicAttackTypeENUM AtkType , vector< MagicAtkEffectInfoStruct >& List  );

    //通知週圍的 A 用輔助法術攻擊(or 幫助) B
    static void MagicAssistRange	( int OwnerID , int MagicBaseID , int SerialID , vector< MagicAssistEffectInfoStruct >& List );

    //通知週圍某角色開始施法
    static void BeginSpellRange		( int	OwnerID , int TargetID , float TargetX , float TargetY  , float TargetZ , int MagicID , int SerialID , int SpellTime );

    //通知週圍某角色停止開始施法
    static void EndSpellRange		( int	OwnerID , int SerialID );

	//通知週圍某角色出手
	static void BeginShootRange		( int	OwnerID , int SerialID , vector<int>& TargetGUIDList );

	static void EndShootRange		( int	OwnerID , int SerialID );

	//通知週圍某角色 系統出手
	static void SysBeginShootRange	( int	OwnerID , int TargetID , int MagicColID , int SerialID , float TX , float TY , float TZ );

	//通知週圍某角色 系統結束
	static void SysEndShootRange	( int	OwnerID , int SerialID );

	//通知週圍某角色 魔法盾資訊
	static void MagicShieldInfoRange		( int AttackGUID , int UnAttackGUID , int AttackMagicID , int ShieldMagicID , int AbsobDamage );


    //取消某人施法
    static void CancleSpellMagicRange		( int	OwnerID , int SerialID );

	//清除某輔助法術	
	static void S_ClearAssistMagic_Range	( int OwnerID , int MagicID , int BuffOwnerID );
	//通知他人自己有的Buf
	static void S_AddBuffInfo_List			( int SendID , RoleDataEx* Role );
	//
	static void SendRange_AddBuffInfo		( int GItemID , int TargetGItemID , int MagicID , int MagicLv , int EffectTime );
	static void SendRange_ModifyBuffInfo	( int GItemID , int TargetGItemID , int MagicID , int MagicLv , int EffectTime );


};

