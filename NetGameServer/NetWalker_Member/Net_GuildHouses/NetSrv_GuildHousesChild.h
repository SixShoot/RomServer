#pragma once
#include "NetSrv_GuildHouses.h"


class NetSrv_GuildHousesChild : public NetSrv_GuildHouses
{
	static bool	CheckBuildAbility( int PlayerDBID );
	static bool	CheckFunitureAbility( int PlayerDBID );
	static bool	CheckSetStyleAbility( int PlayerDBID );
public:
    static bool Init();
    static bool Release();

	virtual void RD_BuildHouseResult	( int PlayerDBID , int GuildID , bool Result );
	virtual void RD_FindHouseResult		( int PlayerDBID , int GuildID , bool Result );
	virtual void RC_VisitHouseRequest	( BaseItemObject* Obj , int GuildID);
	virtual void RC_CloseVisitHouse		( BaseItemObject* Obj );
	virtual void RC_LeaveHouse			( BaseItemObject* Obj , int PlayerDBID );
	virtual void RD_HouseBaseInfo		( GuildHousesInfoStruct& HouseBaseInfo );
	virtual void RD_ItemInfo			( int GuildID , int PageID , int ItemCount , GuildHouseItemStruct Item[ _DEF_GUIDHOUSEITEM_MAX_PAGEITEM_ ] );
	virtual void RD_HouseInfoLoadOK		( int GuildID );
	virtual void RD_BuildingInfo		( GuildHouseBuildingInfoStruct&	Building );

	//////////////////////////////////////////////////////////////////////////
	// 公會建築
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_CreateBuildingRequest	( BaseItemObject* Obj , char* Point , int BuildingID , int ParentBuildingDBID , float Dir );
	virtual void RC_BuildingUpgradeRequest	( BaseItemObject* Obj , int BuildingDBID , int UpgradeBuildingID );
	virtual void RC_DeleteBuildingRequest	( BaseItemObject* Obj , int BuildingDBID );	
	virtual void RC_ActiveBuildingRequest	( BaseItemObject* Obj , int BuildingDBID , bool IsActive );

	static void SC_AllBuildingActiveType	( int PlayerGUID );
	//////////////////////////////////////////////////////////////////////////
	// 公會倉庫
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_HouseItemRequest		( BaseItemObject* Obj , int PageID );		
	virtual void RC_SwapBodyHouseItem		( BaseItemObject* Obj , int BodyPos , int HouseItemPageID , int HouseItemPos , bool IsFromBody );	
	virtual void RC_SwapHouseItemRequest	( BaseItemObject* Obj , int HouseItemPageID[2] , int HouseItemPos[2] );
	virtual void RC_BuyItemPageRequest		( BaseItemObject* Obj , int PageID );

	static  void SC_FixHouseItemProc		( RoleDataEx* Owner , GuildHouseItemStruct* HouseItem , int PageID , int ItemPos );
	static  void SC_FixHouseFurnitureProc	( BaseItemObject* Obj , int GuildID , GuildHouseFurnitureItemStruct* HouseItem , int ItemPos );

	//////////////////////////////////////////////////////////////////////////
	// 要求建築物資料
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_AllBuildingInfoRequest( BaseItemObject* Obj , int GuildID );
	//////////////////////////////////////////////////////////////////////////
	// 傢俱
	//////////////////////////////////////////////////////////////////////////
	static  void SC_AllFurnitureItemInfo		( int SendID );
	
	virtual void RD_FurnitureItemInfo			( GuildHouseFurnitureItemStruct& Item );
	virtual void RC_SwapBodyHouseFurniture		( BaseItemObject* Obj , int BodyPos , int HousePos );
	virtual void RC_SwapHouseFurnitureRequest	( BaseItemObject* Obj , int Pos[2] );
	virtual void RC_HouseItemLayoutRequest		( BaseItemObject* Obj , int BuildingObjID , int Pos , House3DLayoutStruct&	Layout );
	virtual void RC_FurnitureSendBack			( BaseItemObject* Obj , int Pos );
	//////////////////////////////////////////////////////////////////////////
	// 公會倉庫 權限設定
	//////////////////////////////////////////////////////////////////////////
	virtual void RD_StoreConfig			( GuildStorehouseRightStruct& Config );
	virtual void RC_SetStoreConfig		( BaseItemObject* Obj , GuildStorehouseRightStruct& Config );

	static bool  SC_StoreConfigProc		( BaseItemObject* Obj );	
	//////////////////////////////////////////////////////////////////////////
	// 公會倉庫 Log
	//////////////////////////////////////////////////////////////////////////
	virtual void RD_ItemLog				( GuildItemStoreLogStruct& ItemLog );
	virtual void RC_ItemLogRequest		( BaseItemObject* Obj , int PageID , int Day );
	//////////////////////////////////////////////////////////////////////////
	//工會屋樣式
	virtual void RC_BuyHouseStyle		( BaseItemObject* Obj , int Type );
	//virtual void RC_SetHouseStyle		( BaseItemObject* Obj , int Type );
	virtual void RC_HouseStyleRequest	( BaseItemObject* Obj );
};

