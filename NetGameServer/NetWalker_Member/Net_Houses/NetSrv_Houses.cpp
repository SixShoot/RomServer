#include "../NetWakerGSrvInc.h"
#include "NetSrv_Houses.h"
#include "../../mainproc/HousesManage/HousesManageClass.h"
//-------------------------------------------------------------------
NetSrv_Houses*      NetSrv_Houses::This         = NULL;
//-------------------------------------------------------------------
bool NetSrv_Houses::_Init()
{	
	Srv_NetCliReg( PG_Houses_CtoL_VisitHouseRequest			);
	Srv_NetCliReg( PG_Houses_CtoL_CloseVisitHouse			);
	Srv_NetCliReg( PG_Houses_CtoL_SwapBodyHouseItem			);
	Srv_NetCliReg( PG_Houses_CtoL_SwapHouseItemRequest		);
	Srv_NetCliReg( PG_Houses_CtoL_HouseItemLayoutRequest	);
	Srv_NetCliReg( PG_Houses_CtoL_ChangeHouseRequest		);
	Srv_NetCliReg( PG_Houses_CtoL_BuyFunctionRequest		);
	Srv_NetCliReg( PG_Houses_CtoL_FunctionSettingRequest	);
	Srv_NetCliReg( PG_Houses_CtoL_BuyEnergyRequest			);
	Srv_NetCliReg( PG_Houses_CtoL_LeaveHouse				);
	Srv_NetCliReg( PG_Houses_CtoL_BoxInfoRequset			);
	Srv_NetCliReg( PG_Houses_CtoL_ExchangeEq				);
	Srv_NetCliReg( PG_Houses_CtoL_SetPassword				);
	Srv_NetCliReg( PG_Houses_CtoL_DestroyItemRequest		);
	Srv_NetCliReg( PG_Houses_CtoL_SetHouseName				);
	Srv_NetCliReg( PG_Houses_CtoL_SetClientData				);

	Srv_NetCliReg( PG_Houses_CtoL_VisitHouseRequest_ByName	);
	

	Srv_NetCliReg( PG_Houses_CtoL_PlantClearRequest			);
	Srv_NetCliReg( PG_Houses_CtoL_PlantItemRequest			);
	Srv_NetCliReg( PG_Houses_CtoL_PlantLockRequest			);
	Srv_NetCliReg( PG_Houses_CtoL_PlantGetProductRequest	);

	_Net->RegOnSrvPacketFunction( EM_PG_Houses_DtoL_BuildHouseResult	, _PG_Houses_DtoL_BuildHouseResult	);
	_Net->RegOnSrvPacketFunction( EM_PG_Houses_DtoL_FindHouseResult		, _PG_Houses_DtoL_FindHouseResult	);
	_Net->RegOnSrvPacketFunction( EM_PG_Houses_DtoL_HouseBaseInfo		, _PG_Houses_DtoL_HouseBaseInfo		);
	_Net->RegOnSrvPacketFunction( EM_PG_Houses_DtoL_ItemInfo			, _PG_Houses_DtoL_ItemInfo			);
	_Net->RegOnSrvPacketFunction( EM_PG_Houses_DtoL_HouseInfoLoadOK		, _PG_Houses_DtoL_HouseInfoLoadOK	);

	Srv_NetCliReg( PG_Houses_CtoL_ModifyFriendDBID	);
	_Net->RegOnSrvPacketFunction( EM_PG_Houses_DtoL_FriendDBID			, _PG_Houses_DtoL_FriendDBID	);

	Srv_NetCliReg( PG_Houses_CtoL_ClearItemLogRequest	);


	Srv_NetCliReg( PG_Houses_CtoL_ServantHireListRequest	);
	Srv_NetCliReg( PG_Houses_CtoL_ServantHireRequest		);
	Srv_NetCliReg( PG_Houses_CtoL_ServantDelRequest			);
	Srv_NetCliReg( PG_Houses_CtoL_ServantEventRequest		);
	Srv_NetCliReg( PG_Houses_CtoL_ServantLayout				);
	Srv_NetCliReg( PG_Houses_CtoL_ServantRenameRequest		);
	return true;
}
//-------------------------------------------------------------------
bool NetSrv_Houses::_Release()
{
    return true;
}
//////////////////////////////////////////////////////////////////////////
//要求招募列表
void NetSrv_Houses::_PG_Houses_CtoL_ServantHireListRequest( int Sockid , int Size , void* Data )
{
	PG_Houses_CtoL_ServantHireListRequest* PG = (PG_Houses_CtoL_ServantHireListRequest*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return;
	This->RC_ServantHireListRequest( Obj  );
}
//////////////////////////////////////////////////////////////////////////
//招募女僕
void NetSrv_Houses::_PG_Houses_CtoL_ServantHireRequest( int Sockid , int Size , void* Data )
{
	PG_Houses_CtoL_ServantHireRequest* PG = (PG_Houses_CtoL_ServantHireRequest*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return;
	This->RC_ServantHireRequest( Obj  , PG->ID , PG->Pos );
}
//////////////////////////////////////////////////////////////////////////
//要求刪除女僕
void NetSrv_Houses::_PG_Houses_CtoL_ServantDelRequest( int Sockid , int Size , void* Data )
{
	PG_Houses_CtoL_ServantDelRequest* PG = (PG_Houses_CtoL_ServantDelRequest*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return;
	This->RC_ServantDelRequest( Obj  , PG->ServantID );
}
//////////////////////////////////////////////////////////////////////////
//女僕互動事件
void NetSrv_Houses::_PG_Houses_CtoL_ServantEventRequest( int Sockid , int Size , void* Data )
{
	PG_Houses_CtoL_ServantEventRequest* PG = (PG_Houses_CtoL_ServantEventRequest*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return;
	This->RC_ServantEventRequest( Obj  , PG->ServantID , PG->EventType1 , PG->EventType2 );
}
//////////////////////////////////////////////////////////////////////////
//女僕擺設
void NetSrv_Houses::_PG_Houses_CtoL_ServantLayout( int Sockid , int Size , void* Data )
{
	PG_Houses_CtoL_ServantLayout* PG = (PG_Houses_CtoL_ServantLayout*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return;
	This->RC_ServantLayout( Obj  , PG->ServantID , PG->Layout );
}
//////////////////////////////////////////////////////////////////////////
//女僕命名
void NetSrv_Houses::_PG_Houses_CtoL_ServantRenameRequest( int Sockid , int Size , void* Data )
{
	PG_Houses_CtoL_ServantRenameRequest* PG = (PG_Houses_CtoL_ServantRenameRequest*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return;
	This->RC_ServantRenameRequest( Obj , PG->ServantID , PG->Name.Begin() );
}
void NetSrv_Houses::_PG_Houses_CtoL_ClearItemLogRequest			( int sockid , int size , void* data )
{
	PG_Houses_CtoL_ClearItemLogRequest* PG = (PG_Houses_CtoL_ClearItemLogRequest*) data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;
	This->RC_ClearItemLogRequest( Obj );
}

void NetSrv_Houses::_PG_Houses_CtoL_VisitHouseRequest_ByName					( int sockid , int size , void* data )
{
	PG_Houses_CtoL_VisitHouseRequest_ByName* PG = (PG_Houses_CtoL_VisitHouseRequest_ByName*) data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;
	This->RC_VisitHouseRequest_ByName( Obj , (char*)PG->RoleName.Begin() , (char*)PG->Password.Begin() );
}

void NetSrv_Houses::_PG_Houses_DtoL_FriendDBID					( int sockid , int size , void* data )
{
	PG_Houses_DtoL_FriendDBID* PG = (PG_Houses_DtoL_FriendDBID*) data;
	This->RD_FriendDBID(  PG->HouseDBID , PG->FriendDBID );
}
void NetSrv_Houses::_PG_Houses_CtoL_ModifyFriendDBID			( int sockid , int size , void* data )
{
	PG_Houses_CtoL_ModifyFriendDBID* PG = (PG_Houses_CtoL_ModifyFriendDBID*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_ModifyFriendDBID( Obj , PG->IsDelete , PG->FriendDBID );
}

void NetSrv_Houses::_PG_Houses_CtoL_PlantClearRequest		( int sockid , int size , void* data )
{
	PG_Houses_CtoL_PlantClearRequest* PG = (PG_Houses_CtoL_PlantClearRequest*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_PlantClearRequest( Obj , PG->HousePos );
}
void NetSrv_Houses::_PG_Houses_CtoL_PlantItemRequest		( int sockid , int size , void* data )
{
	PG_Houses_CtoL_PlantItemRequest* PG = (PG_Houses_CtoL_PlantItemRequest*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_PlantItemRequest( Obj , PG->Type , PG->PotHousePos , PG->ItemBodyPos );
}
void NetSrv_Houses::_PG_Houses_CtoL_PlantLockRequest		( int sockid , int size , void* data )
{
	PG_Houses_CtoL_PlantLockRequest* PG = (PG_Houses_CtoL_PlantLockRequest*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_PlantLockRequest( Obj , PG->PotHousePos );
}
void NetSrv_Houses::_PG_Houses_CtoL_PlantGetProductRequest	( int sockid , int size , void* data )
{
	PG_Houses_CtoL_PlantGetProductRequest* PG = (PG_Houses_CtoL_PlantGetProductRequest*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_PlantGetProductRequest( Obj , PG->PotHousePos );
}
void NetSrv_Houses::_PG_Houses_CtoL_DestroyItemRequest			( int sockid , int size , void* data )
{
	PG_Houses_CtoL_DestroyItemRequest* PG = (PG_Houses_CtoL_DestroyItemRequest*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_DestroyItemRequest( Obj , PG->ItemDBID );
}

void NetSrv_Houses::_PG_Houses_CtoL_ExchangeEq					( int sockid , int size , void* data )
{
	PG_Houses_CtoL_ExchangeEq* PG = (PG_Houses_CtoL_ExchangeEq*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_ExchangeEq( Obj , PG->BoxItemDBID , PG->BoxPos , PG->EqPos );
}
void NetSrv_Houses::_PG_Houses_CtoL_SetHouseName		( int sockid , int size , void* data )	
{
	PG_Houses_CtoL_SetHouseName* PG = (PG_Houses_CtoL_SetHouseName*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_SetHouseName( Obj , PG->HouseName.Begin() );
}

void NetSrv_Houses::_PG_Houses_CtoL_SetClientData		( int sockid , int size , void* data )	
{
	PG_Houses_CtoL_SetClientData* PG = (PG_Houses_CtoL_SetClientData*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_SetClientData( Obj , PG->ClientData );
}


void NetSrv_Houses::_PG_Houses_CtoL_SetPassword			( int sockid , int size , void* data )
{
	PG_Houses_CtoL_SetPassword* PG = (PG_Houses_CtoL_SetPassword*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_SetPassword( Obj , PG->Password.Begin() );
}

void NetSrv_Houses::_PG_Houses_CtoL_BoxInfoRequset				( int sockid , int size , void* data )
{
	PG_Houses_CtoL_BoxInfoRequset* PG = (PG_Houses_CtoL_BoxInfoRequset*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_BoxInfoRequset( Obj , PG->BoxItemDBID );
}
void NetSrv_Houses::_PG_Houses_DtoL_BuildHouseResult			( int sockid , int size , void* data )
{
	PG_Houses_DtoL_BuildHouseResult* PG = (PG_Houses_DtoL_BuildHouseResult*) data;
	This->RD_BuildHouseResult( PG->PlayerDBID , PG->HouseDBID , PG->Result );
}
void NetSrv_Houses::_PG_Houses_DtoL_FindHouseResult				( int sockid , int size , void* data )
{
	PG_Houses_DtoL_FindHouseResult* PG = (PG_Houses_DtoL_FindHouseResult*) data;
	This->RD_FindHouseResult( PG->PlayerDBID , PG->HouseDBID , PG->Result );
}
void NetSrv_Houses::_PG_Houses_DtoL_HouseBaseInfo				( int sockid , int size , void* data )	
{
	PG_Houses_DtoL_HouseBaseInfo* PG = (PG_Houses_DtoL_HouseBaseInfo*) data;
	This->RD_HouseBaseInfo( PG->HouseBaseInfo );
}
void NetSrv_Houses::_PG_Houses_DtoL_ItemInfo					( int sockid , int size , void* data )	
{
	PG_Houses_DtoL_ItemInfo* PG = (PG_Houses_DtoL_ItemInfo*) data;
	This->RD_ItemInfo( PG->HouseDBID , PG->ParentItemDBID ,  PG->ItemCount , PG->Item );
}
void NetSrv_Houses::_PG_Houses_DtoL_HouseInfoLoadOK				( int sockid , int size , void* data )
{
	PG_Houses_DtoL_HouseInfoLoadOK* PG = (PG_Houses_DtoL_HouseInfoLoadOK*) data;
	This->RD_HouseInfoLoadOK( PG->HouseDBID );
}

void NetSrv_Houses::_PG_Houses_CtoL_LeaveHouse					( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Houses_CtoL_LeaveHouse* PG = (PG_Houses_CtoL_LeaveHouse*) data;
	This->RC_LeaveHouse( Obj , PG->PlayerDBID);
}

void NetSrv_Houses::_PG_Houses_CtoL_VisitHouseRequest			( int sockid , int size , void* data )	
{
	PG_Houses_CtoL_VisitHouseRequest* PG = (PG_Houses_CtoL_VisitHouseRequest*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_VisitHouseRequest( Obj , PG->HouseDBID , (char*)PG->Password.Begin() );
}
void NetSrv_Houses::_PG_Houses_CtoL_CloseVisitHouse				( int sockid , int size , void* data )
{
	PG_Houses_CtoL_CloseVisitHouse* PG = (PG_Houses_CtoL_CloseVisitHouse*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_CloseVisitHouse( Obj );
}
void NetSrv_Houses::_PG_Houses_CtoL_SwapBodyHouseItem			( int sockid , int size , void* data )	
{
	PG_Houses_CtoL_SwapBodyHouseItem* PG = (PG_Houses_CtoL_SwapBodyHouseItem*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_SwapBodyHouseItem( Obj , PG->HouseParentItemDBID , PG->HousePos , PG->BodyPos , PG->IsFromBody );
}

void NetSrv_Houses::_PG_Houses_CtoL_SwapHouseItemRequest		( int sockid , int size , void* data )	
{
	PG_Houses_CtoL_SwapHouseItemRequest* PG = (PG_Houses_CtoL_SwapHouseItemRequest*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_SwapHouseItemRequest( Obj , PG->HouseParentItemDBID , PG->HousePos );
}

void NetSrv_Houses::_PG_Houses_CtoL_HouseItemLayoutRequest		( int sockid , int size , void* data )
{
	PG_Houses_CtoL_HouseItemLayoutRequest* PG = (PG_Houses_CtoL_HouseItemLayoutRequest*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_HouseItemLayoutRequest( Obj , PG->ItemDBID , PG->Layout );
}

void NetSrv_Houses::_PG_Houses_CtoL_ChangeHouseRequest			( int sockid , int size , void* data )
{
	PG_Houses_CtoL_ChangeHouseRequest* PG = (PG_Houses_CtoL_ChangeHouseRequest*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_ChangeHouseRequest( Obj , PG->HouseType );
}
void NetSrv_Houses::_PG_Houses_CtoL_BuyFunctionRequest			( int sockid , int size , void* data )
{
	PG_Houses_CtoL_BuyFunctionRequest* PG = (PG_Houses_CtoL_BuyFunctionRequest*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_BuyFunctionRequest( Obj , PG->FunctionID );
}
void NetSrv_Houses::_PG_Houses_CtoL_FunctionSettingRequest		( int sockid , int size , void* data )	
{
	PG_Houses_CtoL_FunctionSettingRequest* PG = (PG_Houses_CtoL_FunctionSettingRequest*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_FunctionSettingRequest( Obj , PG->FunctionID , PG->Mode );
}
void NetSrv_Houses::_PG_Houses_CtoL_BuyEnergyRequest			( int sockid , int size , void* data )	
{
	PG_Houses_CtoL_BuyEnergyRequest* PG = (PG_Houses_CtoL_BuyEnergyRequest*) data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_BuyEnergyRequest( Obj , PG->Money_Account , PG->Password.Begin() );
}


void NetSrv_Houses::SD_BuildHouseRequest		( int PlayerDBID , int HouseType , char* Account_ID )
{
	PG_Houses_LtoD_BuildHouseRequest	Send;
	Send.PlayerDBID = PlayerDBID;
	Send.HouseType = HouseType;
	Send.Account_ID = Account_ID;

	SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_Houses::SD_FindHouseRequest_ByName	( int PlayerDBID , char* RoleName , char* Password , int ManageLv )
{
	PG_Houses_LtoD_FindHouseRequest_ByName	Send;
	Send.PlayerDBID = PlayerDBID;	//要求的玩家
	Send.RoleName	= RoleName;	//參觀房間號碼
	Send.Password	= Password;
	Send.ManageLv	= ManageLv;

	SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_Houses::SD_FindHouseRequest			( int PlayerDBID , int HouseDBID , char* Password , int ManageLv )
{
	PG_Houses_LtoD_FindHouseRequest	Send;
	Send.PlayerDBID = PlayerDBID;	//要求的玩家
	Send.HouseDBID	= HouseDBID;	//參觀房間號碼
	Send.Password	= Password;
	Send.ManageLv	= ManageLv;

	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Houses::SD_HouseInfoLoading			( int HouseDBID )
{
	PG_Houses_LtoD_HouseInfoLoading	Send;
	Send.HouseDBID = HouseDBID;

	SendToDBCenter( sizeof( Send ) , &Send );
}
/*
void NetSrv_Houses::SD_SaveSwapBodyHouseItem	( int PlayerDBID , int BodyPos , ItemFieldStruct& BodyItem , HouseItemDBStruct& HouseItem )
{
	PG_Houses_LtoD_SaveSwapBodyHouseItem	Send;
	Send.PlayerDBID = PlayerDBID;
	Send.BodyPos = BodyPos;
	Send.BodyItem = BodyItem;
	Send.HouseItem = HouseItem;

	SendToDBCenter( sizeof( Send ) , &Send );
}
*/
void NetSrv_Houses::SD_SaveHouseBase			( int PlayerDBID , HouseBaseDBStruct& HouseBaseInfo )
{
	PG_Houses_LtoD_SaveHouseBase	Send;
	Send.PlayerDBID = PlayerDBID;
	Send.HouseBaseInfo = HouseBaseInfo;

	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Houses::SD_SaveItem					( int PlayerDBID , HouseItemDBStruct& Item )
{
	PG_Houses_LtoD_SaveItem		Send;
	Send.PlayerDBID = PlayerDBID;
	Send.Item = Item;

	SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_Houses::SC_BuildHouseResult			( int SendToID , int HouseDBID , bool Result )
{
	PG_Houses_LtoC_BuildHouseResult		Send;
	Send.HouseDBID = HouseDBID;
	Send.Result = Result;

	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_OpenVisitHouse			( int SendToID , int TargetNpcID , bool IsVisitMyHouse )
{
	PG_Houses_LtoC_OpenVisitHouse		Send;
	Send.TargetNpcID = TargetNpcID;
	Send.IsVisitMyHouse	= IsVisitMyHouse;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_CloseVisitHouse			( int SendToID )
{
	PG_Houses_LtoC_CloseVisitHouse		Send;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_FindHouseResult			( int SendToID , FindHouseResultENUM Result )
{
	PG_Houses_LtoC_FindHouseResult		Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_HouseInfoLoading			( int SendToID )
{
	PG_Houses_LtoC_HouseInfoLoading		Send;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_HouseBaseInfo			( int SendToID , HouseBaseDBStruct& HouseBaseInfo )
{
	PG_Houses_LtoC_HouseBaseInfo		Send;
	Send.HouseBaseInfo = HouseBaseInfo;

	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_ItemInfo					( int SendToID , int ItemDBID , vector<HouseItemDBStruct>& ItemList )
{
//	if( ItemList.size() > _DEF_MAX_HOUSE_FURNITURE )
//		return;

	PG_Houses_LtoC_ItemInfo		Send;
	Send.ItemDBID = ItemDBID;
	int Count = 0;
	for( unsigned i = 0 ; i < ItemList.size() ; i++ )	
	{
		if( ItemList[i].Item.IsEmpty() )
			continue;
		
		Send.Item[Count] = ItemList[i];
		Count++;
	}
	Send.ItemCount = Count;

	SendToCli_ByGUID( SendToID , Send.Size() , &Send );
}
void NetSrv_Houses::SC_ItemInfo					( int SendToID , int ItemDBID , vector<HouseItemStruct*>& ItemList )
{
	PG_Houses_LtoC_ItemInfo		Send;
	Send.ItemDBID = ItemDBID;
	int Count = 0;
	for( unsigned i = 0 ; i < ItemList.size() ; i++ )	
	{
		if( ItemList[i]->Info.Item.IsEmpty() )
			continue;

		Send.Item[Count] = ItemList[i]->Info;
		Count++;
	}
	Send.ItemCount = Count;

	SendToCli_ByGUID( SendToID , Send.Size() , &Send );
}

void NetSrv_Houses::SC_ItemInfo					( int SendToID , int ItemDBID , int ItemCount , HouseItemDBStruct ItemList[_DEF_MAX_HOUSE_FURNITURE] )
{
	PG_Houses_LtoC_ItemInfo		Send;
	Send.ItemDBID = ItemDBID;
	/*
	for( unsigned i = 0 ; i < ItemCount ; i++ )	
		Send.Item[i] = Item[i];
	Send.ItemCount = ItemCount;
*/
	int Count = 0;
	for( int i = 0 ; i < ItemCount ; i++ )	
	{
		if( ItemList[i].Item.IsEmpty() )
			continue;

		Send.Item[Count] = ItemList[i];
		Count++;
	}
	Send.ItemCount = Count;

	SendToCli_ByGUID( SendToID , Send.Size() , &Send );
}

void NetSrv_Houses::SC_HouseInfoLoadOK			( int SendToID )
{
	PG_Houses_LtoC_HouseInfoLoadOK		Send;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_SwapBodyHouseItemResult	( int SendToID , int BodyPos , int HousePos , int HouseParentItemDBID , bool Result )
{
	PG_Houses_LtoC_SwapBodyHouseItemResult		Send;
	Send.BodyPos			 = BodyPos;
	Send.HousePos			 = HousePos;
	Send.HouseParentItemDBID = HouseParentItemDBID;
	Send.Result				 = Result;

	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_FixHouseItem				( int SendToID , HouseItemDBStruct& HouseItem )
{
	PG_Houses_LtoC_FixHouseItem		Send;
	Send.HouseItem = HouseItem;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Houses::SC_SwapHouseItemResult		( int SendToID , int ParentItemDBID1 , int HousePos1 , int ParentItemDBID2 , int HousePos2 , bool Result )
{
	PG_Houses_LtoC_SwapHouseItemResult		Send;
	Send.HousePos[0] = HousePos1;
	Send.HousePos[1] = HousePos2;
	Send.ParentItemDBID[0] = ParentItemDBID1;
	Send.ParentItemDBID[1] = ParentItemDBID2;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Houses::SC_HouseItemLayoutResult( int SendToID , int HouseItemDBID , bool Result )
{
	PG_Houses_LtoC_HouseItemLayoutResult		Send;
	Send.HouseItemDBID = HouseItemDBID;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_ChangeHouseResult		( int SendToID , int HouseType , bool Result )
{
	PG_Houses_LtoC_ChangeHouseResult		Send;
	Send.HouseType = HouseType;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_BuyFunctionResult		( int SendToID , int FunctionID , bool Result )
{
	PG_Houses_LtoC_BuyFunctionResult		Send;
	Send.FunctionID = FunctionID;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_FunctionSettingResult	( int SendToID , int FunctionID , int Mode , bool Result )
{
	PG_Houses_LtoC_FunctionSettingResult		Send;
	Send.FunctionID = FunctionID;
	Send.Mode = Mode;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_BuyEnergyResult			( int SendToID , int Money_Account , int Energy , bool Result )
{
	PG_Houses_LtoC_BuyEnergyResult		Send;
	Send.Money_Account = Money_Account;
	Send.Energy = Energy;
	Send.Result = Result;

	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Houses::SC_BoxInfoResult			( int SendToID , bool Result )
{
	PG_Houses_LtoC_BoxInfoResult		Send;
	Send.Result = Result;

	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Houses::SC_ExchangeEqResult			( int SendToID , bool Result )
{
	PG_Houses_LtoC_ExchangeEqResult	Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Houses::SC_SetPasswordResult		( int SendToID , bool Result )
{
	PG_Houses_LtoC_SetPasswordResult	Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Houses::SC_DestroyItemResult		( int SendToID , bool Result )
{
	PG_Houses_LtoC_DestroyItemResult	Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );

}

void NetSrv_Houses::SC_SetHouseNameResult		( int SendToID , SetHouseNameResultENUM Result )
{
	PG_Houses_LtoC_SetHouseNameResult	Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Houses::SC_PlantClearResult			( int SendToID , int HousePos , PlantClearResultTypeENUM Result )
{
	PG_Houses_LtoC_PlantClearResult	Send;
	Send.HousePos = HousePos;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_PlantItemResult			( int SendToID , PlantItemTypeENUM Type , int PotHousePos , int ItemBodyPos , PlantItemResultTypeENUM Result )
{
	PG_Houses_LtoC_PlantItemResult	Send;
	Send.Type = Type;
	Send.PotHousePos = PotHousePos;		//盆子位置	
	Send.ItemBodyPos = ItemBodyPos;		//身體位置
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_PlantLockResult			( int SendToID , int PotHousePos , PlantLockResultTypeENUM Result )
{
	PG_Houses_LtoC_PlantLockResult	Send;
	Send.PotHousePos = PotHousePos;		//盆子位置	
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_PlantGetProductResult	( int SendToID , int PotHousePos , PlantGetProductResultTypeENUM Result )
{
	PG_Houses_LtoC_PlantGetProductResult	Send;
	Send.PotHousePos = PotHousePos;		//盆子位置	
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Houses::SC_ModifyFriendDBID			( int SendToID	, bool IsDelete , int FriendDBID )
{
	PG_Houses_LtoC_ModifyFriendDBID	Send;
	Send.FriendDBID = FriendDBID;		//盆子位置	
	Send.IsDelete = IsDelete;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Houses::SC_FriendDBID				( int SendToID , int HouseDBID , int FriendDBID )
{
	PG_Houses_LtoC_FriendDBID	Send;
	Send.FriendDBID = FriendDBID;		//盆子位置	
	Send.HouseDBID = HouseDBID;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_FriendItemLog			( int SendToID , HouseFriendItemLogStruct& Log )
{
	PG_Houses_LtoC_FriendItemLog	Send;
	Send.Log = Log;		//盆子位置	
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Houses::SC_ClearItemLogResult		( int SendToID , bool Result )
{
	PG_Houses_LtoC_ClearItemLogResult Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
//招募列表回應
void NetSrv_Houses::SC_ServantHireList( int SendtoID , int ID , bool End , HouseServantStruct Servant )
{
	PG_Houses_LtoC_ServantHireList Send;
	Send.ID = ID;
	Send.End = End;
	Send.Servant = Servant;
	SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
//招募女僕結果
void NetSrv_Houses::SC_ServantHireResultOK( int SendtoID , int ID , int Pos , HouseServantStruct Servant )
{
	PG_Houses_LtoC_ServantHireResult Send;
	Send.Pos = Pos;
	Send.ID = ID;
	Send.Servant = Servant;
	Send.Result = true;
	SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );
}
void NetSrv_Houses::SC_ServantHireResultFailed( int SendtoID , int ID )
{
	PG_Houses_LtoC_ServantHireResult Send;
	Send.Pos = -1;
	Send.ID = ID;
	Send.Result = false;
	SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
//女僕刪除結果
void NetSrv_Houses::SC_ServantDelResult( int SendtoID , int ServantID , bool Result )
{
	PG_Houses_LtoC_ServantDelResult Send;
	Send.ServantID = ServantID;
	Send.Result = Result;
	SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
//女僕互動事件通知
void NetSrv_Houses::SC_ServantEvent( int SendtoID , int ServantID , int EventID )
{
	PG_Houses_LtoC_ServantEvent Send;
	Send.ServantID = ServantID;
	Send.EventID = EventID;
	SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
//女僕命名結果
void NetSrv_Houses::SC_ServantRenameResult( int SendtoID , const char* Name , bool Result )
{
	PG_Houses_LtoC_ServantRenameResult Send;
	Send.Name = Name;
	Send.Result = Result;
	SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
//修正女僕數值
void NetSrv_Houses::SC_FixServantValue( int RoomID , int	ServantID , HouseServantValueTypeENUM Type , int Value )
{
	PG_Houses_LtoC_FixServantValue Send;
	Send.ServantID = ServantID;
	Send.Type = Type;
	Send.Value = Value;
	//SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );
	SendToCli_ByRoomID( RoomID , sizeof( Send ) , &Send );
}