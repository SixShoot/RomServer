#include "../NetWakerGSrvInc.h"
#include "NetSrv_Recipe.h"
//-------------------------------------------------------------------
NetSrv_Recipe*      NetSrv_Recipe::This         = NULL;
//-------------------------------------------------------------------
bool NetSrv_Recipe::_Init()
{
	Srv_NetCliReg( PG_Recipe_CtoL_LockResourceAndRefineRequest                );
	Srv_NetCliReg( PG_Recipe_CtoL_LearnRecipe                );

    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Recipe::_Release()
{
	for( map< int, StaticFlag< 4096 >* >::iterator it = This->m_mapRecipeList.begin(); it != This->m_mapRecipeList.end(); it++ )
	{
		delete it->second;
	}
	This->m_mapRecipeList.clear();


    return true;
}
//-------------------------------------------------------------------
void NetSrv_Recipe::_PG_Recipe_CtoL_LearnRecipe( int Sockid , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return ;

	PG_Recipe_CtoL_LearnRecipe* PG = (PG_Recipe_CtoL_LearnRecipe*) Data;

	This->RC_LearnRecipe( Obj, PG->iRecipeID );
}
//-------------------------------------------------------------------
void NetSrv_Recipe::_PG_Recipe_CtoL_LockResourceAndRefineRequest( int Sockid , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return ;

	PG_Recipe_CtoL_LockResourceAndRefineRequest* PG = (PG_Recipe_CtoL_LockResourceAndRefineRequest*) Data;

	This->RC_LockResourceAndRefineRequest( Obj , PG->RecipeID );
}
//////////////////////////////////////////////////////////////////////////

void NetSrv_Recipe::SC_RefineResultOK( int SendID , int RecipeID ,vector<ItemFieldStruct>& ItemList )
{
	PG_Recipe_LtoC_ProduceItemResult SendItem;
	for( int i = 0 ; i < (int)ItemList.size() ; i++ )
	{
		SendItem.Item = ItemList[i];
		SendToCli_ByGUID( SendID , sizeof(SendItem ) , &SendItem );
	}

	PG_Recipe_LtoC_LockResourceAndRefineResult Send;
	Send.RecipeID = RecipeID;
	Send.Result = EM_RefineResult_OK;
	SendToCli_ByGUID( SendID , sizeof(Send ) , &Send );
}
//合成失敗 ItemList 為消耗的物品
void NetSrv_Recipe::SC_RefineResultFailed( int SendID , int RecipeID ,vector<ItemFieldStruct>& ItemList   )
{

	PG_Recipe_LtoC_ProduceItemResult SendItem;
	for( int i = 0 ; i < (int)ItemList.size() ; i++ )
	{
		SendItem.Item = ItemList[i];
		SendToCli_ByGUID( SendID , sizeof(SendItem ) , &SendItem );
	}

	PG_Recipe_LtoC_LockResourceAndRefineResult Send;
	Send.RecipeID = RecipeID;
	Send.Result = EM_RefineResult_Failed;
	SendToCli_ByGUID( SendID , sizeof(Send ) , &Send );
}
//合成條件不符合
void NetSrv_Recipe::SC_RefineResultError( int SendID , int RecipeID  , RefineResultENUM Result )
{

	PG_Recipe_LtoC_LockResourceAndRefineResult Send;
	Send.RecipeID = RecipeID;
	Send.Result = Result;
	SendToCli_ByGUID( SendID , sizeof(Send ) , &Send );
}
//-------------------------------------------------------------
void NetSrv_Recipe::AddRecipe( int iObjID, int iReciptID )
{
	if( !( iReciptID >= Def_ObjectClass_Recipe && iReciptID < ( Def_ObjectClass_Recipe + DF_MAX_RECIPELIST ) ) )
	{
		if( iReciptID != -1 )
			return;
	}

	StaticFlag< DF_MAX_RECIPELIST >* pReciptList = NULL;

	map< int, StaticFlag< DF_MAX_RECIPELIST >* >::iterator it = This->m_mapRecipeList.find( iObjID );

	if( it == This->m_mapRecipeList.end() )
	{
		pReciptList = NEW( StaticFlag< 4096 > );
		This->m_mapRecipeList[ iObjID ] = pReciptList;
	}
	else
		pReciptList = it->second;

	

	if( iReciptID == -1 )
	{
		pReciptList->ReSet();
	}
	else
	{
		int iOffset = iReciptID - Def_ObjectClass_Recipe;
		pReciptList->SetFlagOn( iOffset );
	}
}
//-------------------------------------------------------------
void NetSrv_Recipe::SC_RecipeList( int iClientID, int iObjID )
{
	map< int, StaticFlag< DF_MAX_RECIPELIST >* >::iterator it = This->m_mapRecipeList.find( iObjID );

	if( it != This->m_mapRecipeList.end() )
	{
		StaticFlag< DF_MAX_RECIPELIST >* pReciptList = it->second;
		
		PG_Recipe_LtoC_RecipeList	Packet;

		Packet.RecipeList	= *pReciptList;

		SendToCli_ByGUID( iClientID, sizeof( Packet ), &Packet );
	}
}
//-------------------------------------------------------------
void NetSrv_Recipe::SC_LearnRecipeResult	( int iClientID, int iRecipeID, int iResult )
{
	PG_Recipe_LtoC_LearnRecipeResult	Packet;

	Packet.iRecipeID	= iRecipeID;
	Packet.iResult		= iResult;

	SendToCli_ByGUID( iClientID, sizeof(Packet), &Packet );
}
//-------------------------------------------------------------
void NetSrv_Recipe::SC_RefineCooldown( int iClientID, RefineBuffStruct Cooldown )
{
	PG_Recipe_LtoC_RefineCooldown	Packet;

	Packet.Cooldown		= Cooldown;

	SendToCli_ByGUID( iClientID, sizeof(Packet), &Packet );
}