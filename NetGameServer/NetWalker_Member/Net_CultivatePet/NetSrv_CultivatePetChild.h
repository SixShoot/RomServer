#pragma once
#include "NetSrv_CultivatePet.h"

class NetSrv_CultivatePetChild : public NetSrv_CultivatePet
{

public:
    static bool Init();
    static bool Release();
	//////////////////////////////////////////////////////////////////////////
	static void	ReCalPetLv				( RoleDataEx* Owner, int PetPos );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_PushPet				( BaseItemObject* Obj , int PetPos , int ItemPos );
	virtual void RC_PopPet				( BaseItemObject* Obj , int PetPos , int ItemPos );
	virtual void RC_PetName				( BaseItemObject* Obj , int PetPos , const char* Name );
	virtual void RC_PetEvent			( BaseItemObject* Obj , int PetPos , int ItemPos , CultivatePetCommandTypeENUM Event );
	virtual void RC_LearnSkill			( BaseItemObject* Obj , int PetPos , int MagicID , int MagicLv );
	virtual void RC_PetLifeSkill		( BaseItemObject* Obj , int PetPos , CultivatePetLifeSkillTypeENUM SkillType , int LV , int TablePos );
	virtual void RC_SwapItem			( BaseItemObject* Obj , int PetPos , CultivatePetEQTypeENUM PetItemPos , int BodyPos , bool IsFromBody );
	virtual void RC_GetProductRequest	( BaseItemObject* Obj , int PetPos );	
	virtual void RC_PetMarge			( BaseItemObject* Obj , int PetCount , int PetPos[3] );

	virtual void RC_RegPetCard			( BaseItemObject* obj , int petPos );
};

