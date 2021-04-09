#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_CultivatePet.h"

class NetSrv_CultivatePet : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_CultivatePet* This;
    static bool _Init();
    static bool _Release();

	static void _PG_CultivatePet_CtoL_PushPet			( int NetID , int Size , void* Data );
	static void _PG_CultivatePet_CtoL_PopPet			( int NetID , int Size , void* Data );
	static void _PG_CultivatePet_CtoL_PetName			( int NetID , int Size , void* Data );
	static void _PG_CultivatePet_CtoL_PetEvent			( int NetID , int Size , void* Data );
	static void _PG_CultivatePet_CtoL_LearnSkill		( int NetID , int Size , void* Data );
	static void _PG_CultivatePet_CtoL_PetLifeSkill		( int NetID , int Size , void* Data );
	static void _PG_CultivatePet_CtoL_SwapItem			( int NetID , int Size , void* Data );
	static void _PG_CultivatePet_CtoL_GetProductRequest	( int NetID , int Size , void* Data );
	static void _PG_CultivatePet_CtoL_PetMarge			( int NetID , int Size , void* Data );
	static void _PG_CultivatePet_CtoL_RegPetCard		( int NetID , int Size , void* Data );
public:    

	virtual void RC_PushPet				( BaseItemObject* Obj , int PetPos , int ItemPos ) = 0;
	virtual void RC_PopPet				( BaseItemObject* Obj , int PetPos , int ItemPos ) = 0;
	virtual void RC_PetName				( BaseItemObject* Obj , int PetPos , const char* Name ) = 0;
	virtual void RC_PetEvent			( BaseItemObject* Obj , int PetPos , int ItemPos , CultivatePetCommandTypeENUM Event ) = 0;
	virtual void RC_LearnSkill			( BaseItemObject* Obj , int PetPos , int MagicID , int MagicLv ) = 0;
	virtual void RC_PetLifeSkill		( BaseItemObject* Obj , int PetPos , CultivatePetLifeSkillTypeENUM SkillType , int Lv , int TablePos ) = 0;
	virtual void RC_SwapItem			( BaseItemObject* Obj , int PetPos , CultivatePetEQTypeENUM PetItemPos , int BodyPos , bool IsFromBody ) = 0;
	virtual void RC_GetProductRequest	( BaseItemObject* Obj , int PetPos ) = 0;


	static void SC_PushPetResultOK		( int SendToID , int PetPos , CultivatePetStruct& PetInfo );
	static void SC_PushPetResultFailed	( int SendToID  );
	static void	SC_PopPetResult			( int SendToID , bool Result , int ItemPos );
	static void	SC_PetNameResult		( int SendToID , bool Result , const char* Name );
	static void	SC_PetEventResult		( int SendToID , int PetPos , CultivatePetCommandTypeENUM OrgEvent , CultivatePetCommandTypeENUM Event , int EventTime , bool Result );
	static void	SC_LearnSkillResult		( int SendToID , int PetPos , int MagicID , int MagicLv , bool Result );
	static void	SC_PetLifeResult		( int SendToID , CultivatePetLifeSkillTypeENUM	SkillType , int PetPos , int EventTime , bool Result );
	static void	SC_SwapItemResult		( int SendToID , bool Result );
	static void	SC_FixItem				( int SendToID , int PetPos , int PetItemPos , ItemFieldStruct& Item );
	static void	SC_FixPetInfo			( int SendToID , int PetPos , CultivatePetStruct&	PetBase );
	static void SC_GetProductResult		( int SendToID , bool Result );
	static void SC_PetEventProcess		( int SendToID , int PetPos , CultivatePetCommandTypeENUM Event );

	//////////////////////////////////////////////////////////////////////////
	//寵物融合
	virtual void RC_PetMarge			( BaseItemObject* Obj , int PetCount , int PetPos[3] ) = 0;
	static void SC_PetMargeResultOK		( int SendToID , int PetCount , int PetPos[3] , CultivatePetStruct& PetBase );
	static void SC_PetMargeResultFailed	( int SendToID , PetMargeResultENUM Result );
	//////////////////////////////////////////////////////////////////////////
	//登入寵物卡片
	virtual void RC_RegPetCard			( BaseItemObject* obj , int petPos ) = 0;
	//登入寵物卡片
	void SC_RegPetCardResult			( int SendToID , bool result );

};

