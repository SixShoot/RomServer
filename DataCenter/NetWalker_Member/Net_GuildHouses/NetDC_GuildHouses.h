
#pragma once
#include "PG/PG_GuildHouses.h"
#include "../../MainProc/Global.h"

class CNetDC_GuildHouses : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetDC_GuildHouses*	This;

	static bool				_Init();
	static bool				_Release();

	static void _PG_GuildHouses_LtoD_BuildHouseRequest		( int ServerID , int Size , void* Data );	
	static void _PG_GuildHouses_LtoD_FindHouseRequest		( int ServerID , int Size , void* Data );		
	static void _PG_GuildHouses_LtoD_HouseInfoLoading		( int ServerID , int Size , void* Data );		
	
public:
	static void SL_BuildHouseResult		( int ServerID , int PlayerDBID , int GuildID , bool Result );
	static void SL_FindHouseResult		( int ServerID , int PlayerDBID , int GuildID ,	bool Result );
	static void SL_HouseBaseInfo		( int ServerID , GuildHousesInfoStruct& HouseBaseInfo );
	static void SL_ItemInfo				( int ServerID , int GuildID , int PageID , vector< GuildHouseItemStruct > ItemList );
	static void SL_BuildingInfo			( int ServerID , GuildHouseBuildingInfoStruct& Building);
	static void SL_HouseInfoLoadOK		( int ServerID , int GuildID );
	static void SL_FurnitureItemInfo	( int ServerID , GuildHouseFurnitureItemStruct& Item );
	static void SL_StoreConfig			( int ServerID , GuildStorehouseRightStruct& Config );
	static void SL_ItemLog				( int ServerID , GuildItemStoreLogStruct& ItemLog );

	virtual void RL_BuildHouseRequest	( int ServerID , int PlayerDBID , int GuildID  ) = 0;	
	virtual void RL_FindHouseRequest	( int ServerID , int PlayerDBID , int GuildID ) = 0;	
	virtual void RL_HouseInfoLoading	( int ServerID , int GuildID) = 0;	



};
