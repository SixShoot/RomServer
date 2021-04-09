#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_Recipe.h"

class NetSrv_Recipe : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Recipe* This;
    static bool _Init();
    static bool _Release();

	static void _PG_Recipe_CtoL_LockResourceAndRefineRequest( int Sockid , int Size , void* Data );
	static void _PG_Recipe_CtoL_LearnRecipe( int Sockid , int Size , void* Data );

public:    

	
	static void SC_RefineResultOK( int SendID , int RecipeID ,vector<ItemFieldStruct>& ItemList );
	//�X������ ItemList �����Ӫ����~
	static void SC_RefineResultFailed( int SendID , int RecipeID ,vector<ItemFieldStruct>& ItemList   );
	//�X�����󤣲ŦX
	static void SC_RefineResultError( int SendID , int RecipeID  , RefineResultENUM Result );

	//Client �ݭn�D�X�����~
	virtual void RC_LockResourceAndRefineRequest( BaseItemObject* Sender , int Recipe ) = 0;

	//Client �ݭn�D�ǲ߰t��
	virtual void RC_LearnRecipe ( BaseItemObject* Sender , int Recipe ) = 0;


	static	void	SC_RecipeList			( int iClientID, int iObjID );
	static	void	SC_LearnRecipeResult	( int iClientID, int iRecipeID, int iResult );
	static	void	SC_RefineCooldown		( int iClientID, RefineBuffStruct Cooldown );
	//----------------------------------------------------------------------
	static	void	AddRecipe				( int iObjID, int iReciptID );




	map< int, StaticFlag< DF_MAX_RECIPELIST >* >			m_mapRecipeList;		// Index NPC GUID, 

};

