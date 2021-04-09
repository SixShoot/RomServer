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
    //�q���g�� A �Ϊk�N������(or�v��) B
    static void MagicAttackRange	( int OwnerID , int MagicBaseID , int SerialID , MagicAttackTypeENUM AtkType , vector< MagicAtkEffectInfoStruct >& List  );

    //�q���g�� A �λ��U�k�N����(or ���U) B
    static void MagicAssistRange	( int OwnerID , int MagicBaseID , int SerialID , vector< MagicAssistEffectInfoStruct >& List );

    //�q���g��Y����}�l�I�k
    static void BeginSpellRange		( int	OwnerID , int TargetID , float TargetX , float TargetY  , float TargetZ , int MagicID , int SerialID , int SpellTime );

    //�q���g��Y���ⰱ��}�l�I�k
    static void EndSpellRange		( int	OwnerID , int SerialID );

	//�q���g��Y����X��
	static void BeginShootRange		( int	OwnerID , int SerialID , vector<int>& TargetGUIDList );

	static void EndShootRange		( int	OwnerID , int SerialID );

	//�q���g��Y���� �t�ΥX��
	static void SysBeginShootRange	( int	OwnerID , int TargetID , int MagicColID , int SerialID , float TX , float TY , float TZ );

	//�q���g��Y���� �t�ε���
	static void SysEndShootRange	( int	OwnerID , int SerialID );

	//�q���g��Y���� �]�k�޸�T
	static void MagicShieldInfoRange		( int AttackGUID , int UnAttackGUID , int AttackMagicID , int ShieldMagicID , int AbsobDamage );


    //�����Y�H�I�k
    static void CancleSpellMagicRange		( int	OwnerID , int SerialID );

	//�M���Y���U�k�N	
	static void S_ClearAssistMagic_Range	( int OwnerID , int MagicID , int BuffOwnerID );
	//�q���L�H�ۤv����Buf
	static void S_AddBuffInfo_List			( int SendID , RoleDataEx* Role );
	//
	static void SendRange_AddBuffInfo		( int GItemID , int TargetGItemID , int MagicID , int MagicLv , int EffectTime );
	static void SendRange_ModifyBuffInfo	( int GItemID , int TargetGItemID , int MagicID , int MagicLv , int EffectTime );


};

