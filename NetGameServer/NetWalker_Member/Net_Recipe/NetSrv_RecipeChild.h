#pragma once

#include "NetSrv_Recipe.h"

struct Struct_ObjRecipeList
{
	
};

class NetSrv_RecipeChild : public NetSrv_Recipe
{
public:
    static bool Init();
    static bool Release();

	//Client �ݭn�D�X�����~
	virtual void RC_LockResourceAndRefineRequest	( BaseItemObject* Sender , int Recipe );
	virtual void RC_LearnRecipe						( BaseItemObject* Sender , int Recipe );


};

