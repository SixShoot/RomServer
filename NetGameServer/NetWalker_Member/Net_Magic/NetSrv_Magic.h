#pragma     once

#include "../../MainProc/Global.h"
#include "PG/PG_Magic.h"

class NetSrv_Magic : public Global
{
protected:
    //-------------------------------------------------------------------
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------
    static void _PG_Magic_CtoL_BeginSpell         ( int sockid , int size , void* data );    
    static void _PG_Magic_CtoL_BeginSpell_Pos     ( int sockid , int size , void* data );    
    static void _PG_Magic_CtoL_SetMagicPoint      ( int sockid , int size , void* data );    
	static void _PG_Magic_CtoL_SetMagicPointEx    ( int sockid , int size , void* data );    
	static void _PG_Magic_LtoC_CancelAssistMagic  ( int sockid , int size , void* data );			

	static void _PG_Magic_CtoL_PetCommand			( int sockid , int size , void* data );			
	static void _PG_Magic_CtoL_PetMagicSwitchRequest( int sockid , int size , void* data );			
	static void _PG_Magic_CtoL_CancelMagicRequest	( int sockid , int size , void* data );	
	static void _PG_Magic_CtoL_BuffSkillRequest		( int sockid , int size , void* data );	
public:    
    static NetSrv_Magic* This;

    static void S_BeginSpell          ( int SendID , int OwnerID , int TargetID  , float TargetX , float TargetY , float TargetZ , int MagicID , int SerialID , int SpellTime );
    static void S_EndSpell            ( int SendID , int SerialID );
    static void S_BeginSpellOK        ( int SendID , int MagicID, int SerialID , int SpellTime , int ShootAngle );
    static void S_BeginSpellFailed    ( int SendID , int MagicID , int SerialID);
	static void S_BeginShoot		  ( int SendID , int SerialID , vector<int>& TargetGUIDList );
	static void S_EndShoot			  ( int SendID , int SerialID );
    static void S_SetMagicPointResult ( int SendID , int MagicID , int MagicPos , SetMagicPointResultCodeENUM Result );
	static void S_ResetColdown		  ( int SendID , ResetColdownTypeENUM Type );
	static void S_MagicEnd			  ( int SendID , int MagicID , int Coldown_Normal , int Coldown_All );

	static void S_SysBeginShoot		  ( int SendID , int OwnerID , int TargetID , int MagicColID , int SerialID , float TX , float TY , float TZ );
	static void S_SysEndShoot		  ( int SendID , int SerialID );

	static void S_MagicShieldInfo	  ( int SendID , int AttackGUID , int UnAttackGUID , int AttackMagicID , int ShieldMagicID , int Damage );

	static void S_CreatePet			  ( int SendID , int PetGUID , int PetWorldGUID , int PetID , int MagicBaseID , SummonPetTypeENUM	PetType );
	static void S_DeletePet			  ( int SendID , int PetGUID , SummonPetTypeENUM PetType );
	static void S_PetMagicSwitchResult( int SendID , int PetGUID , SummonPetTypeENUM	PetType , int ActiveMagicID , bool Active  );
	static void S_CreatePetRangeInfo	( int PetGUID );

    static PG_Magic_LtoC_MagicAttack&       G_MagicAttack         ( int MagicBaseID , int SerialID , MagicAttackTypeENUM	AtkType , vector< MagicAtkEffectInfoStruct > & List );
    static PG_Magic_LtoC_MagicAssistResult& G_MagicAssist         ( int MagicBaseID , int SerialID , vector< MagicAssistEffectInfoStruct >& List  );


    static void S_FixAssistMagicTime  ( int SendID , int MagicID , int EffectTime , int BuffOwnerID );

	//取消法術(法術中斷)
    static void S_CancelSpellMagic  ( int SendID , int OwnerID , int SerialID );

    //清除某輔助法術	
    static void S_ClearAssistMagic	( int SendID , int MagicID , int BuffOwnerID );
	//通知顯示的Buf
	static void S_AddBuffInfo		(	int SendID , int GItemID , int TargetGUID , int MagicID , int MagicLv , int EffectTime );
	static void S_ModifyBuffInfo	(	int SendID , int GItemID , int TargetGUID , int MagicID , int MagicLv , int EffectTime );
	static void S_DelBuffInfo		(	int SendID , int GItemID , int MagicID , int BuffOwnerID );

	//通知被施展復活術
	static void S_RaiseMagicNotify		( int SendID , int SpellerID , int MagicBaseID );


    virtual void R_SpellRequest       ( BaseItemObject* OwnerObj , int OwnerID , int TargetID , int MagicID , int MagicPos ) = 0;
    virtual void R_SpellRequest_Pos   ( BaseItemObject* OwnerObj , int OwnerID , float X , float Y , float Z , int MagicID , int MagicPos ) = 0;
    virtual void R_SetMagicPoint      ( BaseItemObject* OwnerObj , int MagicID , int MagicPos )  = 0;
	virtual void R_SetMagicPointEx    ( BaseItemObject* OwnerObj , int MagicID , int MagicPos , int Times )  = 0;

	virtual void R_CancelAssistMagic  ( BaseItemObject* OwnerObj , int MagicID ) = 0;

	virtual void R_PetCommand			( BaseItemObject* OwnerObj , PET_ACTION_ENUM Action , int TargetID , int MagicID , int PetGUID ) = 0;
	virtual void R_PetMagicSwitchRequest( BaseItemObject* OwnerObj , int PetGUID , int ActiveMagicID , bool Active ) = 0;
	virtual void R_CancelMagicRequest	( BaseItemObject* OwnerObj ) = 0;


	virtual void R_BuffSkillRequest( BaseItemObject* Obj , int BuffID , int BuffSkillID ,  int TargetID , float PosX , float PosY , float PosZ  ) = 0;
	static void S_BuffSkillResult( int SendID , bool Result );

	static void S_PartyMemberBuff( int SendDBID , RoleDataEx* Role );
	static void S_AllMember_PartyMemberBuff( RoleDataEx* Role );
};

