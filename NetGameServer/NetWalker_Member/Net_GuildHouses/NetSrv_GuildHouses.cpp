#include "../NetWakerGSrvInc.h"
#include "NetSrv_GuildHouses.h"
#include "../../mainproc/GuildHouseManage/GuildHousesClass.h"
//-------------------------------------------------------------------
NetSrv_GuildHouses*      NetSrv_GuildHouses::This         = NULL;
//-------------------------------------------------------------------
bool NetSrv_GuildHouses::_Init()
{	
	Srv_NetCliReg( PG_GuildHouses_CtoL_VisitHouseRequest	);
	Srv_NetCliReg( PG_GuildHouses_CtoL_CloseVisitHouse		);
	Srv_NetCliReg( PG_GuildHouses_CtoL_LeaveHouse			);

	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHouses_DtoL_BuildHouseResult		, _PG_GuildHouses_DtoL_BuildHouseResult		);
	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHouses_DtoL_FindHouseResult		, _PG_GuildHouses_DtoL_FindHouseResult		);

	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHouses_DtoL_HouseBaseInfo			, _PG_GuildHouses_DtoL_HouseBaseInfo	);
	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHouses_DtoL_ItemInfo				, _PG_GuildHouses_DtoL_ItemInfo			);
	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHouses_DtoL_HouseInfoLoadOK		, _PG_GuildHouses_DtoL_HouseInfoLoadOK	);
	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHouses_DtoL_BuildingInfo			, _PG_GuildHouses_DtoL_BuildingInfo		);

//	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHouses_CtoL_CreateBuildingRequest	, _PG_GuildHouses_CtoL_CreateBuildingRequest	);
//	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHouses_CtoL_BuildingUpgradeRequest	, _PG_GuildHouses_CtoL_BuildingUpgradeRequest	);
//	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHouses_CtoL_DeleteBuildingRequest	, _PG_GuildHouses_CtoL_DeleteBuildingRequest	);
	Srv_NetCliReg( PG_GuildHouses_CtoL_CreateBuildingRequest	);
	Srv_NetCliReg( PG_GuildHouses_CtoL_BuildingUpgradeRequest	);
	Srv_NetCliReg( PG_GuildHouses_CtoL_DeleteBuildingRequest	);
	Srv_NetCliReg( PG_GuildHouses_CtoL_AllBuildingInfoRequest	);

	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHouses_DtoL_FurnitureItemInfo		, _PG_GuildHouses_DtoL_FurnitureItemInfo		);
	Srv_NetCliReg( PG_GuildHouses_CtoL_SwapBodyHouseFurniture		);
	Srv_NetCliReg( PG_GuildHouses_CtoL_SwapHouseFurnitureRequest	);
	Srv_NetCliReg( PG_GuildHouses_CtoL_HouseItemLayoutRequest		);

	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHouses_DtoL_StoreConfig		, _PG_GuildHouses_DtoL_StoreConfig		);
	Srv_NetCliReg( PG_GuildHouses_CtoL_SetStoreConfig		);

	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHouses_DtoL_ItemLog		, _PG_GuildHouses_DtoL_ItemLog		);
	Srv_NetCliReg( PG_GuildHouses_CtoL_ItemLogRequest			);
	Srv_NetCliReg( PG_GuildHouses_CtoL_ActiveBuildingRequest	);


	Srv_NetCliReg( PG_GuildHouses_CtoL_HouseItemRequest			);
	Srv_NetCliReg( PG_GuildHouses_CtoL_SwapBodyHouseItem		);
	Srv_NetCliReg( PG_GuildHouses_CtoL_SwapHouseItemRequest		);

	Srv_NetCliReg( PG_GuildHouses_CtoL_BuyItemPageRequest		);
	Srv_NetCliReg( PG_GuildHouses_CtoL_FurnitureSendBack		);
	Srv_NetCliReg( PG_GuildHouses_CtoL_BuyHouseStyle			);
//	Srv_NetCliReg( PG_GuildHouses_CtoL_SetHouseStyle			);
	Srv_NetCliReg( PG_GuildHouses_CtoL_HouseStyleRequest		);
	return true;
}
//-------------------------------------------------------------------
bool NetSrv_GuildHouses::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_HouseStyleRequest			( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_HouseStyleRequest* pg =(PG_GuildHouses_CtoL_HouseStyleRequest*)data;
	This->RC_HouseStyleRequest( Obj );
}
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_BuyHouseStyle				( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_BuyHouseStyle* pg =(PG_GuildHouses_CtoL_BuyHouseStyle*)data;
	This->RC_BuyHouseStyle( Obj , pg->Type );
}
/*
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_SetHouseStyle				( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_SetHouseStyle* pg =(PG_GuildHouses_CtoL_SetHouseStyle*)data;
	This->RC_SetHouseStyle( Obj , pg->Type );
}
*/
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_FurnitureSendBack	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_FurnitureSendBack* pg =(PG_GuildHouses_CtoL_FurnitureSendBack*)data;
	This->RC_FurnitureSendBack( Obj , pg->Pos );
}

void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_BuyItemPageRequest( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_BuyItemPageRequest* pg =(PG_GuildHouses_CtoL_BuyItemPageRequest*)data;
	This->RC_BuyItemPageRequest( Obj , pg->PageID );
}

void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_ActiveBuildingRequest	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_ActiveBuildingRequest* pg =(PG_GuildHouses_CtoL_ActiveBuildingRequest*)data;
	This->RC_ActiveBuildingRequest( Obj , pg->BuildDBID , pg->IsActive );
}

void NetSrv_GuildHouses::_PG_GuildHouses_DtoL_ItemLog			( int sockid , int size , void* data )
{
	PG_GuildHouses_DtoL_ItemLog* pg =(PG_GuildHouses_DtoL_ItemLog*)data;
	This->RD_ItemLog( pg->ItemLog );

}
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_ItemLogRequest	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_ItemLogRequest* pg =(PG_GuildHouses_CtoL_ItemLogRequest*)data;
	This->RC_ItemLogRequest( Obj , pg->PageID , pg->Day );
}

void NetSrv_GuildHouses::_PG_GuildHouses_DtoL_StoreConfig		( int sockid , int size , void* data )
{
	PG_GuildHouses_DtoL_StoreConfig* pg =(PG_GuildHouses_DtoL_StoreConfig*)data;
	This->RD_StoreConfig( pg->Config );
}
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_SetStoreConfig	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_SetStoreConfig* pg =(PG_GuildHouses_CtoL_SetStoreConfig*)data;
	This->RC_SetStoreConfig( Obj , pg->Config );
}

void NetSrv_GuildHouses::_PG_GuildHouses_DtoL_FurnitureItemInfo			( int sockid , int size , void* data )
{

	PG_GuildHouses_DtoL_FurnitureItemInfo* pg =(PG_GuildHouses_DtoL_FurnitureItemInfo*)data;
	This->RD_FurnitureItemInfo( pg->Item );

}
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_SwapBodyHouseFurniture	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_SwapBodyHouseFurniture* pg =(PG_GuildHouses_CtoL_SwapBodyHouseFurniture*)data;
	This->RC_SwapBodyHouseFurniture( Obj , pg->BodyPos , pg->HousePos );
}
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_SwapHouseFurnitureRequest	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_SwapHouseFurnitureRequest* pg =(PG_GuildHouses_CtoL_SwapHouseFurnitureRequest*)data;
	This->RC_SwapHouseFurnitureRequest( Obj , pg->Pos );
}
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_HouseItemLayoutRequest	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_HouseItemLayoutRequest* pg =(PG_GuildHouses_CtoL_HouseItemLayoutRequest*)data;
	This->RC_HouseItemLayoutRequest( Obj , pg->BuildingObjID , pg->Pos , pg->Layout );
}
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_AllBuildingInfoRequest( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_AllBuildingInfoRequest* pg =(PG_GuildHouses_CtoL_AllBuildingInfoRequest*)data;
	This->RC_AllBuildingInfoRequest( Obj , pg->GuildID );
}

void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_HouseItemRequest		( int sockid , int size , void* data )	
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_HouseItemRequest* pg =(PG_GuildHouses_CtoL_HouseItemRequest*)data;
	This->RC_HouseItemRequest( Obj , pg->PageID );
}
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_SwapBodyHouseItem		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_SwapBodyHouseItem* pg =(PG_GuildHouses_CtoL_SwapBodyHouseItem*)data;
	This->RC_SwapBodyHouseItem( Obj , pg->BodyPos , pg->HouseItemPageID , pg->HouseItemPos , pg->IsFromBody );
}
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_SwapHouseItemRequest	( int sockid , int size , void* data )	
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_SwapHouseItemRequest* pg =(PG_GuildHouses_CtoL_SwapHouseItemRequest*)data;
	This->RC_SwapHouseItemRequest( Obj , pg->HouseItemPageID , pg->HouseItemPos );
}

void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_CreateBuildingRequest	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_CreateBuildingRequest* pg =(PG_GuildHouses_CtoL_CreateBuildingRequest*)data;
	This->RC_CreateBuildingRequest( Obj , pg->Point , pg->BuildingID , pg->ParentBuildingDBID , pg->Dir );
}
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_BuildingUpgradeRequest	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_BuildingUpgradeRequest* pg =(PG_GuildHouses_CtoL_BuildingUpgradeRequest*)data;
	This->RC_BuildingUpgradeRequest( Obj , pg->BuildingDBID , pg->UpgradeBuildingID );
}
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_DeleteBuildingRequest	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_DeleteBuildingRequest* pg =(PG_GuildHouses_CtoL_DeleteBuildingRequest*)data;
	This->RC_DeleteBuildingRequest( Obj , pg->BuildingDBID );
}

void NetSrv_GuildHouses::_PG_GuildHouses_DtoL_HouseBaseInfo		( int sockid , int size , void* data )
{
	PG_GuildHouses_DtoL_HouseBaseInfo* pg =(PG_GuildHouses_DtoL_HouseBaseInfo*)data;
	This->RD_HouseBaseInfo( pg->HouseBaseInfo );
}
void NetSrv_GuildHouses::_PG_GuildHouses_DtoL_ItemInfo			( int sockid , int size , void* data )
{
	PG_GuildHouses_DtoL_ItemInfo* pg =(PG_GuildHouses_DtoL_ItemInfo*)data;
	This->RD_ItemInfo( pg->GuildID , pg->PageID , pg->ItemCount , pg->Item );
}
void NetSrv_GuildHouses::_PG_GuildHouses_DtoL_HouseInfoLoadOK	( int sockid , int size , void* data )
{
	PG_GuildHouses_DtoL_HouseInfoLoadOK* pg =(PG_GuildHouses_DtoL_HouseInfoLoadOK*)data;
	This->RD_HouseInfoLoadOK( pg->GuildID );
}
void NetSrv_GuildHouses::_PG_GuildHouses_DtoL_BuildingInfo		( int sockid , int size , void* data )
{
	PG_GuildHouses_DtoL_BuildingInfo* pg =(PG_GuildHouses_DtoL_BuildingInfo*)data;
	This->RD_BuildingInfo( pg->Building );
}

void NetSrv_GuildHouses::_PG_GuildHouses_DtoL_BuildHouseResult	( int sockid , int size , void* data )
{
	PG_GuildHouses_DtoL_BuildHouseResult* pg =(PG_GuildHouses_DtoL_BuildHouseResult*)data;
	This->RD_BuildHouseResult( pg->PlayerDBID , pg->GuildID , pg->Result );
}
void NetSrv_GuildHouses::_PG_GuildHouses_DtoL_FindHouseResult	( int sockid , int size , void* data )
{
	PG_GuildHouses_DtoL_FindHouseResult* pg =(PG_GuildHouses_DtoL_FindHouseResult*)data;
	This->RD_FindHouseResult( pg->PlayerDBID , pg->GuildID , pg->Result );
}
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_VisitHouseRequest	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_VisitHouseRequest* pg =(PG_GuildHouses_CtoL_VisitHouseRequest*)data;

	This->RC_VisitHouseRequest( Obj , pg->GuildID );
}
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_CloseVisitHouse	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_CloseVisitHouse* pg =(PG_GuildHouses_CtoL_CloseVisitHouse*)data;
	This->RC_CloseVisitHouse( Obj );
}
void NetSrv_GuildHouses::_PG_GuildHouses_CtoL_LeaveHouse		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHouses_CtoL_LeaveHouse* pg =(PG_GuildHouses_CtoL_LeaveHouse*)data;
	This->RC_LeaveHouse( Obj , pg->PlayerDBID );
}


void NetSrv_GuildHouses::SD_BuildHouseRequest	( int PlayerDBID , int GuildID )
{
	PG_GuildHouses_LtoD_BuildHouseRequest Send;
	Send.GuildID = GuildID;
	Send.PlayerDBID = PlayerDBID;
	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_BuildHouseResult	( int SendToID , int GuildID , bool Result )
{
	PG_GuildHouses_LtoC_BuildHouseResult Send;
	Send.GuildID = GuildID;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_OpenVisitHouse		( int SendToID , int TargetNpcID , bool IsVisitMyGuildHouse )
{
	PG_GuildHouses_LtoC_OpenVisitHouse Send;
	Send.TargetNpcID = TargetNpcID;
	Send.IsVisitMyGuildHouse = IsVisitMyGuildHouse;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}


void NetSrv_GuildHouses::SD_FindHouseRequest	(  int PlayerDBID , int GuildID )
{
	PG_GuildHouses_LtoD_FindHouseRequest Send;
	Send.GuildID = GuildID;
	Send.PlayerDBID = PlayerDBID;
	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_FindHouseResult		( int SendToID , bool Result )
{
	PG_GuildHouses_LtoC_FindHouseResult Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_CloseVisitHouse		( int SendToID )
{
	PG_GuildHouses_LtoC_CloseVisitHouse Send;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_HouseInfoLoading	( int SendToID , int GuildID )
{
	PG_GuildHouses_LtoC_HouseInfoLoading Send;
	Send.GuildID = GuildID;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SD_HouseInfoLoading	( int GuildID )
{
	PG_GuildHouses_LtoD_HouseInfoLoading Send;
	Send.GuildID = GuildID;
	SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_HouseInfoLoadOK		( int SendToID )
{
	PG_GuildHouses_LtoC_HouseInfoLoadOK Send;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_GuildHouses::SC_CreateBuildingResult	( int SendToID , const char* Point , int BuildingID , int BuildingDBID , int ParentBuildingDBID , bool Result )
{
	PG_GuildHouses_LtoC_CreateBuildingResult Send;

	MyStrcpy( Send.Point , Point , sizeof(Send.Point) );
	Send.BuildingDBID = BuildingDBID;
	Send.BuildingID = BuildingID;
	Send.ParentBuildingDBID = ParentBuildingDBID;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_BuildingUpgradeResult	( int SendToID , int BuildGItemID , int BuildingDBID , int UpgradeBuildingID , bool Result )
{
	PG_GuildHouses_LtoC_BuildingUpgradeResult Send;
	Send.GItemID = BuildGItemID;
	Send.BuildingDBID = BuildingDBID;
	Send.UpgradeBuildingID = UpgradeBuildingID;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_All_BuildingUpgradeResult( BaseItemObject* Obj , int BuildGItemID , int BuildingDBID , int UpgradeBuildingID )
{
	PG_GuildHouses_LtoC_BuildingUpgradeResult Send;
	Send.GItemID = BuildGItemID;
	Send.BuildingDBID = BuildingDBID;
	Send.UpgradeBuildingID = UpgradeBuildingID;
	Send.Result = true;

	vector< BaseItemObject* >&  ZoneObjList = *(BaseItemObject::GetZoneObjList());

	for( unsigned int i = 0 ; i < ZoneObjList.size() ; i++ )
	{
		BaseItemObject* TempObj = ZoneObjList[i];
		if( TempObj == NULL )
			continue;
		if(	TempObj->Role()->RoomID() != Obj->Role()->RoomID() 	 )
			continue;

		SendToCli_ByGUID( TempObj->GUID() , sizeof( Send ) , &Send );
	}

}

void NetSrv_GuildHouses::SC_DeleteBuildingResult	( int SendToID , int BuildingDBID , bool Result )
{
	PG_GuildHouses_LtoC_DeleteBuildingResult Send;
	Send.BuildingDBID = BuildingDBID;
	Send.Result = Result;

	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_HouseItemInfo			( int SendToID , int MaxPage, int PageID , int GetCount , map< int , GuildHouseItemStruct >* ItemList )
{

	PG_GuildHouses_LtoC_HouseItemInfo Send;
	Send.PageID = PageID;
	Send.MaxPage = MaxPage;
	Send.GetCount = GetCount;
	if( ItemList == NULL  )
	{
		Send.ItemCount = 0;
	}
	else
	{
		Send.ItemCount = int( ItemList->size() );

		int i = 0;
		for( map< int , GuildHouseItemStruct >::iterator Iter = ItemList->begin() ; Iter != ItemList->end() ; Iter++ )
		{
			Send.Item[ i++ ] = Iter->second;
		}
	}

	SendToCli_ByGUID( SendToID , Send.Size() , &Send );
}

void NetSrv_GuildHouses::SC_SwapBodyHouseItemResult	( int SendToID , int BodyPos , int HouseItemPageID , int HouseItemPos , int GetCount , bool Result )
{
	PG_GuildHouses_LtoC_SwapBodyHouseItemResult Send;
	Send.BodyPos = BodyPos;
	Send.HouseItemPageID = HouseItemPageID;
	Send.HouseItemPos = HouseItemPos;
	Send.GetCount = GetCount;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_GuildHouses::SC_FixHouseItem			( int SendToID , GuildHouseItemStruct* Item )
{
	PG_GuildHouses_LtoC_FixHouseItem Send;	
	Send.Item = *Item;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_GuildHouses::SC_SwapHouseItemResult		( int SendToID , int HouseItemPos[2] , int	HouseItemPageID[2] , bool Result )	
{
	PG_GuildHouses_LtoC_SwapHouseItemResult Send;	
	Send.HouseItemPos[0] = HouseItemPos[0];
	Send.HouseItemPos[1] = HouseItemPos[1];
	Send.HouseItemPageID[0] = HouseItemPageID[0];
	Send.HouseItemPageID[1] = HouseItemPageID[1];
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}


void NetSrv_GuildHouses::SC_AllBuildingInfo			( int SendToID , int GuildID  )
{
	PG_GuildHouses_LtoC_AllBuildingInfo Send;
	Send.GuildID = GuildID;
	Send.Count = 0;

	//vector<GuildHouseBuildingStruct>& BuildingList;
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( GuildID );
	if( houseClass == NULL )
	{
		SendToCli_ByGUID( SendToID , Send.PGSize() , &Send );
		return;
	}
	map< int , GuildHouseBuildingStruct >&	BuildingMap = houseClass->BuildingMap();	
	map< int , GuildHouseBuildingStruct >::iterator Iter;

	Send.Count = (int)BuildingMap.size();

	if( Send.Count > _MAX_GUILD_BUILDING_COUNT_ )
		Send.Count = _MAX_GUILD_BUILDING_COUNT_;

	int i = 0;
	for( Iter = BuildingMap.begin() ; Iter != BuildingMap.end() && i < _MAX_GUILD_BUILDING_COUNT_ ; Iter++ , i++ )
	{
		
		Send.Building[i].GItemID	  = Iter->second.ObjGUID;
		Send.Building[i].BuildingDBID = Iter->second.Info.BuildingDBID;
		Send.Building[i].BuildingParentDBID = Iter->second.Info.ParentDBID;
		Send.Building[i].BuildingID = Iter->second.Info.BuildingID;
		MyStrcpy( Send.Building[i].PointStr , Iter->second.Info.PointStr , sizeof(Send.Building[i].PointStr) );
		Send.Building[i].Dir = (int)Iter->second.Info.Dir;
	}

	SendToCli_ByGUID( SendToID , Send.PGSize() , &Send );
}
void NetSrv_GuildHouses::SC_AllZone_StoreConfig			( BaseItemObject* Obj , GuildStorehouseRightStruct& Config )
{
	PG_GuildHouses_LtoC_StoreConfig Send;
	Send.Config = Config;
	//SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
	vector< BaseItemObject* >&  ZoneObjList = *(BaseItemObject::GetZoneObjList());

	for( unsigned int i = 0 ; i < ZoneObjList.size() ; i++ )
	{
		BaseItemObject* TempObj = ZoneObjList[i];
		if( TempObj == NULL )
			continue;
		if(		TempObj->Role()->RoomID() != Obj->Role()->RoomID() 
			||	TempObj->Role()->GuildID() != Obj->Role()->GuildID() 	)
			continue;

		SendToCli_ByGUID( TempObj->GUID() , sizeof( Send ) , &Send );
	}
}
void NetSrv_GuildHouses::SC_AllZone_NewBuildingInfo( BaseItemObject* Obj , int GItemID , GuildHouseBuildingInfoStruct& BuildingInfo )
{
	PG_GuildHouses_LtoC_NewBuildingInfo Send;

	Send.Building.GItemID = GItemID;
	Send.Building.BuildingParentDBID  = BuildingInfo.ParentDBID;
	Send.Building.BuildingDBID  = BuildingInfo.BuildingDBID;
	Send.Building.BuildingID  = BuildingInfo.BuildingID;
	Send.Building.Dir  = (int)BuildingInfo.Dir;
	MyStrcpy( Send.Building.PointStr , BuildingInfo.PointStr , sizeof( Send.Building.PointStr ) ) ;

	vector< BaseItemObject* >&  ZoneObjList = *(BaseItemObject::GetZoneObjList());

	for( unsigned int i = 0 ; i < ZoneObjList.size() ; i++ )
	{
		BaseItemObject* TempObj = ZoneObjList[i];
		if( TempObj == NULL )
			continue;
		if( TempObj->Role()->RoomID() != Obj->Role()->RoomID() )
			continue;

		SendToCli_ByGUID( TempObj->GUID() , sizeof( Send ) , &Send );
	}

	//SendToAllCli( sizeof(Send) , &Send );
}
void NetSrv_GuildHouses::SC_AllZone_DelBuildingInfo( BaseItemObject* Obj , int BuildingDBID )
{
	PG_GuildHouses_LtoC_DelBuildingInfo Send;
	Send.BuildingDBID = BuildingDBID;

	vector< BaseItemObject* >&  ZoneObjList = *(BaseItemObject::GetZoneObjList());

	for( unsigned int i = 0 ; i < ZoneObjList.size() ; i++ )
	{
		BaseItemObject* TempObj = ZoneObjList[i];
		if( TempObj == NULL )
			continue;
		if( TempObj->Role()->RoomID() != Obj->Role()->RoomID() )
			continue;

		SendToCli_ByGUID( TempObj->GUID() , sizeof( Send ) , &Send );
	}
//	SendToAllCli( sizeof(Send) , &Send );
}

void NetSrv_GuildHouses::SC_FurnitureItemInfo			( int SendToID , GuildHouseFurnitureItemStruct& Item )
{
	PG_GuildHouses_LtoC_FurnitureItemInfo Send;
	Send.Item = Item;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_SwapBodyHouseFurnitureResult( int SendToID , int BodyPos , int HousePos , bool Result )
{
	PG_GuildHouses_LtoC_SwapBodyHouseFurnitureResult Send;	
	Send.BodyPos = BodyPos;
	Send.HousePos = HousePos;
	Send.Result = Result;

	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_FixHouseFurniture			( int SendToID , GuildHouseFurnitureItemStruct& Item )
{
	PG_GuildHouses_LtoC_FixHouseFurniture Send;	
	Send.Item = Item;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_All_FixHouseFurniture		( BaseItemObject* Obj , GuildHouseFurnitureItemStruct& Item )
{
	PG_GuildHouses_LtoC_FixHouseFurniture Send;	
	Send.Item = Item;

	vector< BaseItemObject* >&  ZoneObjList = *(BaseItemObject::GetZoneObjList());

	for( unsigned int i = 0 ; i < ZoneObjList.size() ; i++ )
	{
		BaseItemObject* TempObj = ZoneObjList[i];
		if( TempObj == NULL )
			continue;
		if(	TempObj->Role()->RoomID() != Obj->Role()->RoomID() 	 )
			continue;

		SendToCli_ByGUID( TempObj->GUID() , sizeof( Send ) , &Send );
	}

	//Global::SendToAllCli( sizeof( Send ) , &Send );
}

void NetSrv_GuildHouses::SC_SwapHouseFurnitureResult	( int SendToID , int Pos[2] , bool Result )
{
	PG_GuildHouses_LtoC_SwapHouseFurnitureResult Send;	
	Send.Pos[0] = Pos[0];
	Send.Pos[1] = Pos[1];
	Send.Result = Result;

	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
} 
void NetSrv_GuildHouses::SC_HouseItemLayoutResult		( int SendToID , int Pos , bool Result )
{
	PG_GuildHouses_LtoC_HouseItemLayoutResult Send;	
	Send.Pos = Pos;
	Send.Result = Result;

	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_StoreConfig( int SendToID , GuildStorehouseRightStruct& Config )
{
	PG_GuildHouses_LtoC_StoreConfig Send;
	Send.Config = Config;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_SetStoreConfigResult( int SendToID , bool Result )
{
	PG_GuildHouses_LtoC_SetStoreConfigResult Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_GuildHouses::SC_ItemLog	( int SendToID , GuildItemStoreLogStruct*	ItemLog )
{
	PG_GuildHouses_LtoC_ItemLog Send;
	Send.ItemLog = *ItemLog;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_GuildHouses::SC_ItemLogList	( int SendToID , vector<GuildItemStoreLogStruct>*	ItemLogList )
{
	PG_GuildHouses_LtoC_ItemLogList Send;
	if( ItemLogList == NULL )
	{
		Send.Count = 0;
	}
	else
	{
		Send.Count = int( ItemLogList->size() );
		if( Send.Count > 1000 )
			Send.Count = 1000;
	}


	for( int i = 0 ; i < Send.Count ; i++ )
	{
		Send.Log[i] = (*ItemLogList)[i];
	}

	SendToCli_ByGUID( SendToID , Send.Size() , &Send );
}

void NetSrv_GuildHouses::SC_ActiveBuildingResult( int SendToID , int BuildingDBID , bool IsActive , bool Result )
{
	PG_GuildHouses_LtoC_ActiveBuildingResult Send;
	Send.BuildDBID = BuildingDBID;
	Send.IsActive = Result;
	Send.IsActive = IsActive;
	SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}

void NetSrv_GuildHouses::SC_BuyItemPageResult		( int SendToID , int PageID , bool Result )
{
	PG_GuildHouses_LtoC_BuyItemPageResult Send;
	Send.PageID = PageID;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}

void NetSrv_GuildHouses::SC_All_BuyItemPageResult	( BaseItemObject* Obj , int PageID , bool Result )
{
	PG_GuildHouses_LtoC_BuyItemPageResult Send;
	Send.PageID = PageID;
	Send.Result = Result;

	vector< BaseItemObject* >&  ZoneObjList = *(BaseItemObject::GetZoneObjList());
	for( unsigned int i = 0 ; i < ZoneObjList.size() ; i++ )
	{
		BaseItemObject* TempObj = ZoneObjList[i];
		if( TempObj == NULL )
			continue;
		if( TempObj->Role()->RoomID() != Obj->Role()->RoomID() || TempObj->Role()->GuildID() != Obj->Role()->GuildID() )
			continue;

		SendToCli_ByGUID( TempObj->GUID() , sizeof( Send ) , &Send );
	}

}

void NetSrv_GuildHouses::SC_All_ItemLog					( int GuildID , int RoomID ,  GuildItemStoreLogStruct& ItemLog )
{
	PG_GuildHouses_LtoC_ItemLog Send;
	Send.ItemLog = ItemLog;

	vector< BaseItemObject* >&  ZoneObjList = *(BaseItemObject::GetZoneObjList());
	for( unsigned int i = 0 ; i < ZoneObjList.size() ; i++ )
	{
		BaseItemObject* TempObj = ZoneObjList[i];
		if( TempObj == NULL )
			continue;
		if( TempObj->Role()->RoomID() != RoomID || TempObj->Role()->GuildID() != GuildID )
			continue;

		SendToCli_ByGUID( TempObj->GUID() , sizeof( Send ) , &Send );
	}

}

void NetSrv_GuildHouses::SC_BuildingActiveType		( int SendToID , int BuildingDBID , bool IsActive )
{
	PG_GuildHouses_LtoC_BuildingActiveType Send;
	Send.BuildDBID = BuildingDBID;
	Send.IsActive = IsActive;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_GuildHouses::SC_All_BuildingActiveType	( int RoomID , int BuildingDBID , bool IsActive )
{
	PG_GuildHouses_LtoC_BuildingActiveType Send;
	Send.BuildDBID = BuildingDBID;
	Send.IsActive = IsActive;

	vector< BaseItemObject* >&  ZoneObjList = *(BaseItemObject::GetZoneObjList());
	for( unsigned int i = 0 ; i < ZoneObjList.size() ; i++ )
	{
		BaseItemObject* TempObj = ZoneObjList[i];
		if( TempObj == NULL )
			continue;
		if( TempObj->Role()->RoomID() != RoomID )
			continue;

		SendToCli_ByGUID( TempObj->GUID() , sizeof( Send ) , &Send );
	}

}

void NetSrv_GuildHouses::SC_FurnitureSendBackResult		( int SendToID , int Pos , bool Result )
{
	PG_GuildHouses_LtoC_FurnitureSendBackResult Send;
	Send.Pos	= Pos;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_BuyHouseStyleResult			( int SendToID , int Type , bool Result )
{
	PG_GuildHouses_LtoC_BuyHouseStyleResult Send;
	Send.Result = Result;
	Send.Type = Type;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_GuildHouses::SC_Room_BuyHouseStyleResultOk	( int RoomID ,  int Type )
{
	PG_GuildHouses_LtoC_BuyHouseStyleResult Send;
	Send.Result = true;
	Send.Type = Type;
	SendToCli_ByRoomID( RoomID , sizeof(Send) , &Send );

	//SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
/*
void NetSrv_GuildHouses::SC_SetHouseStyleResult			( int SendToID , bool Result )
{
	PG_GuildHouses_LtoC_SetHouseStyleResult Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );

}
*/
void NetSrv_GuildHouses::SC_HouseStyle			( int SendToID , int Type )
{
	PG_GuildHouses_LtoC_HouseStyle Send;
	Send.Type = Type;
	SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );

}