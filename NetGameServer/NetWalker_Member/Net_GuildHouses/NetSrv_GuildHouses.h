#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_GuildHouses.h"

class NetSrv_GuildHouses : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_GuildHouses* This;
    static bool _Init();
    static bool _Release();

	//////////////////////////////////////////////////////////////////////////
	//公會屋建立 載入 登入
	//////////////////////////////////////////////////////////////////////////
	static void _PG_GuildHouses_DtoL_BuildHouseResult			( int sockid , int size , void* data );
	static void _PG_GuildHouses_DtoL_FindHouseResult			( int sockid , int size , void* data );

	static void _PG_GuildHouses_DtoL_HouseBaseInfo				( int sockid , int size , void* data );
	static void _PG_GuildHouses_DtoL_ItemInfo					( int sockid , int size , void* data );
	static void _PG_GuildHouses_DtoL_HouseInfoLoadOK			( int sockid , int size , void* data );
	static void _PG_GuildHouses_DtoL_BuildingInfo				( int sockid , int size , void* data );

	static void _PG_GuildHouses_CtoL_VisitHouseRequest			( int sockid , int size , void* data );
	static void _PG_GuildHouses_CtoL_CloseVisitHouse			( int sockid , int size , void* data );
	static void _PG_GuildHouses_CtoL_LeaveHouse					( int sockid , int size , void* data );

	static void _PG_GuildHouses_CtoL_CreateBuildingRequest		( int sockid , int size , void* data );
	static void _PG_GuildHouses_CtoL_BuildingUpgradeRequest		( int sockid , int size , void* data );
	static void _PG_GuildHouses_CtoL_DeleteBuildingRequest		( int sockid , int size , void* data );

	static void _PG_GuildHouses_CtoL_HouseItemRequest			( int sockid , int size , void* data );		
	static void _PG_GuildHouses_CtoL_SwapBodyHouseItem			( int sockid , int size , void* data );	
	static void _PG_GuildHouses_CtoL_SwapHouseItemRequest		( int sockid , int size , void* data );		
	static void _PG_GuildHouses_CtoL_AllBuildingInfoRequest		( int sockid , int size , void* data );		

	static void _PG_GuildHouses_DtoL_FurnitureItemInfo			( int sockid , int size , void* data );
	static void _PG_GuildHouses_CtoL_SwapBodyHouseFurniture		( int sockid , int size , void* data );
	static void _PG_GuildHouses_CtoL_SwapHouseFurnitureRequest	( int sockid , int size , void* data );
	static void _PG_GuildHouses_CtoL_HouseItemLayoutRequest		( int sockid , int size , void* data );

	static void _PG_GuildHouses_DtoL_StoreConfig				( int sockid , int size , void* data );
	static void _PG_GuildHouses_CtoL_SetStoreConfig				( int sockid , int size , void* data );

	static void _PG_GuildHouses_DtoL_ItemLog					( int sockid , int size , void* data );
	static void _PG_GuildHouses_CtoL_ItemLogRequest				( int sockid , int size , void* data );

	static void _PG_GuildHouses_CtoL_ActiveBuildingRequest		( int sockid , int size , void* data );
	static void _PG_GuildHouses_CtoL_BuyItemPageRequest			( int sockid , int size , void* data );
	static void _PG_GuildHouses_CtoL_FurnitureSendBack			( int sockid , int size , void* data );

	static void _PG_GuildHouses_CtoL_BuyHouseStyle				( int sockid , int size , void* data );
//	static void _PG_GuildHouses_CtoL_SetHouseStyle				( int sockid , int size , void* data );
	static void _PG_GuildHouses_CtoL_HouseStyleRequest			( int sockid , int size , void* data );
public:    

	//////////////////////////////////////////////////////////////////////////
	//公會屋建立 載入 登入
	//////////////////////////////////////////////////////////////////////////
	static void SD_BuildHouseRequest	( int PlayerDBID , int GuildID  );
	static void SC_BuildHouseResult		( int SendToID , int GuildID , bool Result );
	static void SC_OpenVisitHouse		( int SendToID , int TargetNpcID , bool IsVisitMyGuildHouse );
	static void SD_FindHouseRequest		( int PlayerDBID , int GuildID );
	static void SC_FindHouseResult		( int SendToID , bool Result );
	static void SC_CloseVisitHouse		( int SendToID );
	static void SC_HouseInfoLoading		( int SendToID , int GuildID );
	static void SD_HouseInfoLoading		( int GuildID );
	static void SC_HouseInfoLoadOK		( int SendToID );


	virtual void RD_BuildHouseResult	( int PlayerDBID , int GuildID , bool Result ) = 0;
	virtual void RD_FindHouseResult		( int PlayerDBID , int GuildID , bool Result ) = 0;
	virtual void RC_VisitHouseRequest	( BaseItemObject* Obj , int GuildID) = 0;
	virtual void RC_CloseVisitHouse		( BaseItemObject* Obj ) = 0;
	virtual void RC_LeaveHouse			( BaseItemObject* Obj , int PlayerDBID ) = 0;

	virtual void RD_HouseBaseInfo		( GuildHousesInfoStruct& HouseBaseInfo ) = 0;
	virtual void RD_ItemInfo			( int GuildID , int PageID , int ItemCount , GuildHouseItemStruct Item[ _DEF_GUIDHOUSEITEM_MAX_PAGEITEM_ ] ) = 0;
	virtual void RD_HouseInfoLoadOK		( int GuildID ) = 0;
	virtual void RD_BuildingInfo		( GuildHouseBuildingInfoStruct&	Building ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// 公會建築
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_CreateBuildingRequest	( BaseItemObject* Obj , char* Point , int BuildingID , int ParentBuildingDBID , float Dir ) = 0;
	virtual void RC_BuildingUpgradeRequest	( BaseItemObject* Obj , int BuildingDBID , int UpgradeBuildingID ) = 0;
	virtual void RC_DeleteBuildingRequest	( BaseItemObject* Obj , int BuildingDBID ) = 0;
	virtual void RC_ActiveBuildingRequest	( BaseItemObject* Obj , int BuildingDBID , bool IsActive ) = 0;

	static void SC_CreateBuildingResult		( int SendToID , const char* Point , int BuildingID , int BuildingDBID , int ParentBuildingDBID , bool Result );
	static void SC_BuildingUpgradeResult	( int SendToID , int BuildingGItemID , int BuildingDBID , int UpgradeBuildingID , bool Result );
	static void SC_All_BuildingUpgradeResult( BaseItemObject* Obj , int BuildingGItemID , int BuildingDBID , int UpgradeBuildingID );
	static void SC_DeleteBuildingResult		( int SendToID , int BuildingDBID , bool Result );
	static void SC_ActiveBuildingResult		( int SendToID , int BuildingDBID , bool IsActive , bool Result );
	static void SC_BuildingActiveType		( int SendToID , int BuildingDBID , bool IsActive );
	static void SC_All_BuildingActiveType	( int RoomID , int BuildingDBID , bool IsActive );

	//////////////////////////////////////////////////////////////////////////
	// 公會倉庫
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_HouseItemRequest		( BaseItemObject* Obj , int PageID ) = 0;		
	virtual void RC_SwapBodyHouseItem		( BaseItemObject* Obj , int BodyPos , int HouseItemPageID , int HouseItemPos , bool IsFromBody ) = 0;	
	virtual void RC_SwapHouseItemRequest	( BaseItemObject* Obj , int HouseItemPageID[2] , int HouseItemPos[2] ) = 0;
	virtual void RC_BuyItemPageRequest		( BaseItemObject* Obj , int PageID ) = 0;

	static void SC_HouseItemInfo			( int SendToID , int MaxPage , int PageID , int GetCount , map< int , GuildHouseItemStruct >* ItemList );
	static void SC_SwapBodyHouseItemResult	( int SendToID , int BodyPos , int HouseItemPageID , int HouseItemPos , int GetCount , bool Result );
	static void SC_FixHouseItem				( int SendToID , GuildHouseItemStruct* Item );
	static void SC_SwapHouseItemResult		( int SendToID , int HouseItemPos[2] , int	HouseItemPageID[2] , bool Result );	
	static void SC_BuyItemPageResult		( int SendToID , int PageID , bool Result );
	static void SC_All_BuyItemPageResult	( BaseItemObject* Obj , int PageID , bool Result );

	//////////////////////////////////////////////////////////////////////////
	// 要求建築物資料
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_AllBuildingInfoRequest	( BaseItemObject* Obj , int GuildID ) = 0;	

	static void SC_AllBuildingInfo			( int SendToID , int GuildID );

	static void SC_AllZone_NewBuildingInfo	( BaseItemObject* Obj , int GItemID , GuildHouseBuildingInfoStruct& BuildingInfo );
	static void SC_AllZone_DelBuildingInfo	( BaseItemObject* Obj , int BuildingDBID );
	//////////////////////////////////////////////////////////////////////////
	// 傢俱倉庫
	//////////////////////////////////////////////////////////////////////////
	virtual void RD_FurnitureItemInfo			( GuildHouseFurnitureItemStruct& Item ) = 0;
	virtual void RC_SwapBodyHouseFurniture		( BaseItemObject* Obj , int BodyPos , int HousePos ) = 0;
	virtual void RC_SwapHouseFurnitureRequest	( BaseItemObject* Obj , int Pos[2] ) = 0;
	virtual void RC_HouseItemLayoutRequest		( BaseItemObject* Obj , int BuildingObjID , int Pos , House3DLayoutStruct&	Layout ) = 0;
	virtual void RC_FurnitureSendBack			( BaseItemObject* Obj , int Pos ) = 0;

	static  void SC_FurnitureItemInfo			( int SendToID , GuildHouseFurnitureItemStruct& Item );
	static  void SC_SwapBodyHouseFurnitureResult( int SendToID , int BodyPos , int HousePos , bool Result );
	static  void SC_FixHouseFurniture			( int SendToID , GuildHouseFurnitureItemStruct& Item );
	static  void SC_All_FixHouseFurniture		( BaseItemObject* Obj , GuildHouseFurnitureItemStruct& Item );
	static  void SC_SwapHouseFurnitureResult	( int SendToID , int Pos[2] , bool Result );
	static  void SC_HouseItemLayoutResult		( int SendToID , int Pos , bool Result );
	static  void SC_FurnitureSendBackResult		( int SendToID , int Pos , bool Result );
	//////////////////////////////////////////////////////////////////////////
	// 公會倉庫 權限設定
	//////////////////////////////////////////////////////////////////////////
	virtual void RD_StoreConfig					( GuildStorehouseRightStruct& Config ) = 0;
	virtual void RC_SetStoreConfig				( BaseItemObject* Obj , GuildStorehouseRightStruct& Config ) =0;

	static void SC_StoreConfig					( int SendToID , GuildStorehouseRightStruct& Config );
	static void SC_AllZone_StoreConfig			( BaseItemObject* Obj , GuildStorehouseRightStruct& Config );
	static void SC_SetStoreConfigResult			( int SendToID , bool Result );
	//////////////////////////////////////////////////////////////////////////
	// 公會倉庫 Log
	//////////////////////////////////////////////////////////////////////////
	virtual void RD_ItemLog						( GuildItemStoreLogStruct& ItemLog ) = 0;
	virtual void RC_ItemLogRequest				( BaseItemObject* Obj , int PageID , int Day ) = 0;

	static void SC_ItemLogList					( int SendToID , vector<GuildItemStoreLogStruct>*	ItemLogList );
	static void SC_ItemLog						( int SendToID , GuildItemStoreLogStruct*	ItemLog );
	static void SC_All_ItemLog					( int GuildID , int RoomID , GuildItemStoreLogStruct& ItemLog );
	//////////////////////////////////////////////////////////////////////////
	//工會屋樣式
	virtual void RC_BuyHouseStyle				( BaseItemObject* Obj , int Type ) = 0;
	static void SC_BuyHouseStyleResult			( int SendToID , int Type , bool Result );
	static void SC_Room_BuyHouseStyleResultOk	( int RoomID ,  int Type );
//	virtual void RC_SetHouseStyle				( BaseItemObject* Obj , int Type ) = 0;
//	static void SC_SetHouseStyleResult			( int SendToID , bool Result );
	virtual void RC_HouseStyleRequest			( BaseItemObject* Obj ) = 0;
	static void SC_HouseStyle					( int SendToID , int Type );
};											
											
