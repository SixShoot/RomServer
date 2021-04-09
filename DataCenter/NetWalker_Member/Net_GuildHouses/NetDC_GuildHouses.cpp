#include "NetDC_GuildHouses.h"
//-------------------------------------------------------------------
CNetDC_GuildHouses*	CNetDC_GuildHouses::This = NULL;

//-------------------------------------------------------------------
bool CNetDC_GuildHouses::_Init()
{
	_Net->RegOnSrvPacketFunction( EM_PG_GuildHouses_LtoD_BuildHouseRequest		, _PG_GuildHouses_LtoD_BuildHouseRequest		);
	_Net->RegOnSrvPacketFunction( EM_PG_GuildHouses_LtoD_FindHouseRequest		, _PG_GuildHouses_LtoD_FindHouseRequest			);
	_Net->RegOnSrvPacketFunction( EM_PG_GuildHouses_LtoD_HouseInfoLoading		, _PG_GuildHouses_LtoD_HouseInfoLoading			);
	return true;
}
//-------------------------------------------------------------------
bool CNetDC_GuildHouses::_Release()
{

	return true;
}

void CNetDC_GuildHouses::_PG_GuildHouses_LtoD_BuildHouseRequest		( int ServerID , int Size , void* Data )
{
	PG_GuildHouses_LtoD_BuildHouseRequest* PG = ( PG_GuildHouses_LtoD_BuildHouseRequest* )Data;
	This->RL_BuildHouseRequest( ServerID , PG->PlayerDBID , PG->GuildID );
}
void CNetDC_GuildHouses::_PG_GuildHouses_LtoD_FindHouseRequest		( int ServerID , int Size , void* Data )
{
	PG_GuildHouses_LtoD_FindHouseRequest* PG = ( PG_GuildHouses_LtoD_FindHouseRequest* )Data;
	This->RL_FindHouseRequest( ServerID , PG->PlayerDBID , PG->GuildID );
}
void CNetDC_GuildHouses::_PG_GuildHouses_LtoD_HouseInfoLoading		( int ServerID , int Size , void* Data )
{
	PG_GuildHouses_LtoD_HouseInfoLoading* PG = ( PG_GuildHouses_LtoD_HouseInfoLoading* )Data;
	This->RL_HouseInfoLoading( ServerID , PG->GuildID );
}

void CNetDC_GuildHouses::SL_BuildHouseResult	( int ServerID , int PlayerDBID , int GuildID , bool Result )
{
	PG_GuildHouses_DtoL_BuildHouseResult Send;
	Send.PlayerDBID = PlayerDBID;
	Send.GuildID = GuildID;
	Send.Result = Result;
	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}
void CNetDC_GuildHouses::SL_HouseBaseInfo		( int ServerID , GuildHousesInfoStruct& HouseBaseInfo )
{
	PG_GuildHouses_DtoL_HouseBaseInfo Send;
	Send.HouseBaseInfo = HouseBaseInfo;
	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}

void CNetDC_GuildHouses::SL_ItemInfo			( int ServerID , int GuildID , int PageID , vector< GuildHouseItemStruct > ItemList )
{
	PG_GuildHouses_DtoL_ItemInfo Send;
	Send.GuildID = GuildID;
	Send.PageID = PageID;
	Send.ItemCount = (int)ItemList.size();
	if( Send.ItemCount > _DEF_GUIDHOUSEITEM_MAX_PAGEITEM_ )
		Send.ItemCount = _DEF_GUIDHOUSEITEM_MAX_PAGEITEM_;

	for( int i = 0 ; i < Send.ItemCount ; i++ )
		Send.Item[i] = ItemList[i];
	
	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}

void CNetDC_GuildHouses::SL_HouseInfoLoadOK		( int ServerID , int GuildID )
{
	PG_GuildHouses_DtoL_HouseInfoLoadOK Send;
	Send.GuildID = GuildID;

	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}

void CNetDC_GuildHouses::SL_FindHouseResult		( int ServerID , int PlayerDBID , int GuildID ,	bool Result )
{
	PG_GuildHouses_DtoL_FindHouseResult Send;
	Send.PlayerDBID = PlayerDBID;
	Send.GuildID = GuildID;
	Send.Result = Result;

	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}
void CNetDC_GuildHouses::SL_BuildingInfo	( int ServerID , GuildHouseBuildingInfoStruct& Building)
{
	PG_GuildHouses_DtoL_BuildingInfo Send;
	Send.Building = Building;
	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}

void CNetDC_GuildHouses::SL_FurnitureItemInfo	( int ServerID , GuildHouseFurnitureItemStruct& Item )
{
	PG_GuildHouses_DtoL_FurnitureItemInfo Send;
	Send.Item = Item;
	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}

void CNetDC_GuildHouses::SL_StoreConfig			( int ServerID , GuildStorehouseRightStruct& Config )
{
	PG_GuildHouses_DtoL_StoreConfig Send;
	Send.Config = Config;
	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}
void CNetDC_GuildHouses::SL_ItemLog				( int ServerID , GuildItemStoreLogStruct& ItemLog )
{
	PG_GuildHouses_DtoL_ItemLog Send;
	Send.ItemLog = ItemLog;
	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}