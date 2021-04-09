#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_Houses.h"

struct HouseItemStruct;
class NetSrv_Houses : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Houses* This;
    static bool _Init();
    static bool _Release();

	static void _PG_Houses_DtoL_BuildHouseResult			( int sockid , int size , void* data );	
	static void _PG_Houses_DtoL_FindHouseResult				( int sockid , int size , void* data );
	static void _PG_Houses_DtoL_HouseBaseInfo				( int sockid , int size , void* data );	
	static void _PG_Houses_DtoL_ItemInfo					( int sockid , int size , void* data );	
	static void _PG_Houses_DtoL_HouseInfoLoadOK				( int sockid , int size , void* data );
															
	static void _PG_Houses_CtoL_VisitHouseRequest			( int sockid , int size , void* data );	
	static void _PG_Houses_CtoL_CloseVisitHouse				( int sockid , int size , void* data );
	static void _PG_Houses_CtoL_SwapBodyHouseItem			( int sockid , int size , void* data );	
	static void _PG_Houses_CtoL_SwapHouseItemRequest		( int sockid , int size , void* data );	
	static void _PG_Houses_CtoL_HouseItemLayoutRequest		( int sockid , int size , void* data );
	static void _PG_Houses_CtoL_ChangeHouseRequest			( int sockid , int size , void* data );
	static void _PG_Houses_CtoL_BuyFunctionRequest			( int sockid , int size , void* data );
	static void _PG_Houses_CtoL_FunctionSettingRequest		( int sockid , int size , void* data );	
	static void _PG_Houses_CtoL_BuyEnergyRequest			( int sockid , int size , void* data );	
	static void _PG_Houses_CtoL_LeaveHouse					( int sockid , int size , void* data );	
	static void _PG_Houses_CtoL_BoxInfoRequset				( int sockid , int size , void* data );	

	static void _PG_Houses_CtoL_ExchangeEq					( int sockid , int size , void* data );	
	static void _PG_Houses_CtoL_SetPassword					( int sockid , int size , void* data );	
	static void _PG_Houses_CtoL_DestroyItemRequest			( int sockid , int size , void* data );	
	static void _PG_Houses_CtoL_SetHouseName				( int sockid , int size , void* data );	
	static void _PG_Houses_CtoL_SetClientData				( int sockid , int size , void* data );

	static void _PG_Houses_CtoL_PlantClearRequest			( int sockid , int size , void* data );	
	static void _PG_Houses_CtoL_PlantItemRequest			( int sockid , int size , void* data );	
	static void _PG_Houses_CtoL_PlantLockRequest			( int sockid , int size , void* data );	
	static void _PG_Houses_CtoL_PlantGetProductRequest		( int sockid , int size , void* data );
	
	static void _PG_Houses_DtoL_FriendDBID					( int sockid , int size , void* data );
	static void _PG_Houses_CtoL_ModifyFriendDBID			( int sockid , int size , void* data );
	static void _PG_Houses_CtoL_VisitHouseRequest_ByName	( int sockid , int size , void* data );
	static void _PG_Houses_CtoL_ClearItemLogRequest			( int sockid , int size , void* data );

	static void _PG_Houses_CtoL_ServantHireListRequest		( int Sockid , int Size , void* Data );
	static void _PG_Houses_CtoL_ServantHireRequest			( int Sockid , int Size , void* Data );
	static void _PG_Houses_CtoL_ServantDelRequest			( int Sockid , int Size , void* Data );
	static void _PG_Houses_CtoL_ServantEventRequest			( int Sockid , int Size , void* Data );
	static void _PG_Houses_CtoL_ServantLayout				( int Sockid , int Size , void* Data );
	static void _PG_Houses_CtoL_ServantRenameRequest		( int Sockid , int Size , void* Data );
public:    

	static void SD_BuildHouseRequest		( int PlayerDBID , int HouseType , char* Account_ID );
	static void SD_FindHouseRequest			( int PlayerDBID , int HouseDBID , char* Password , int ManageLv );
	static void SD_FindHouseRequest_ByName	( int PlayerDBID , char* RoleName , char* Password , int ManageLv );
	static void SD_HouseInfoLoading			( int HouseDBID );	
//	static void SD_SaveSwapBodyHouseItem	( int PlayerDBID , int BodyPos , ItemFieldStruct& BodyItem , HouseItemDBStruct& HouseItem );		
	static void SD_SaveHouseBase			( int PlayerDBID , HouseBaseDBStruct& HouseBaseInfo );		
	static void SD_SaveItem					( int PlayerDBID , HouseItemDBStruct& Item );	

	static void SC_BuildHouseResult			( int SendToID , int HouseDBID , bool Result );	
	static void SC_OpenVisitHouse			( int SendToID , int TargetNpcID , bool IsVisitMyHouse );	
	static void SC_CloseVisitHouse			( int SendToID );	
	static void SC_FindHouseResult			( int SendToID , FindHouseResultENUM Result );	

	static void SC_HouseInfoLoading			( int SendToID );	
	static void SC_HouseBaseInfo			( int SendToID , HouseBaseDBStruct& HouseBaseInfo );		
	static void SC_ItemInfo					( int SendToID , int ItemDBID , vector<HouseItemDBStruct>& ItemList );	
	static void SC_ItemInfo					( int SendToID , int ItemDBID , vector<HouseItemStruct*>& ItemList );	
	static void SC_ItemInfo					( int SendToID , int ItemDBID , int ItemCount , HouseItemDBStruct Item[_DEF_MAX_HOUSE_FURNITURE] );
	static void SC_HouseInfoLoadOK			( int SendToID );	
	static void SC_SwapBodyHouseItemResult	( int SendToID , int BodyPos , int HousePos , int HouseParentItemDBID , bool Result );	
	static void SC_FixHouseItem				( int SendToID , HouseItemDBStruct& HouseItem );	
	static void SC_SwapHouseItemResult		( int SendToID , int ParentItemDBID1 , int HousePos1 , int ParentItemDBID2 , int HousePos2 , bool Result );	
	static void SC_HouseItemLayoutResult	( int SendToID , int HouseItemDBID , bool Result );	
	static void SC_ChangeHouseResult		( int SendToID , int HouseType , bool Result );		
	static void SC_BuyFunctionResult		( int SendToID , int FunctionID , bool Result );		
	static void SC_FunctionSettingResult	( int SendToID , int FunctionID , int Mode , bool Result );		
	static void SC_BuyEnergyResult			( int SendToID , int Money_Account , int Energy , bool Result );	
	static void SC_BoxInfoResult			( int SendToID , bool Result );	

	static void SC_ExchangeEqResult			( int SendToID , bool Result );
	static void SC_SetPasswordResult		( int SendToID , bool Result );
	static void SC_DestroyItemResult		( int SendToID , bool Result );
	static void SC_SetHouseNameResult		( int SendToID , SetHouseNameResultENUM Result );
	static void SC_ModifyFriendDBID			( int SendToID	, bool IsDelete , int FriendDBID ); 
	static void SC_FriendDBID				( int SendToID , int HouseDBID , int FriendDBID );

	virtual void RD_BuildHouseResult		( int PlayerDBID , int HouseDBID , bool Result ) = 0;
	virtual void RD_FindHouseResult			( int PlayerDBID , int HouseDBID , FindHouseResultENUM Result ) = 0;	
	virtual void RD_HouseBaseInfo			( HouseBaseDBStruct& HouseBaseInfo ) = 0;	
	virtual void RD_ItemInfo				( int HouseDBID , int ItemDBID , int ItemCount , HouseItemDBStruct Item[] ) = 0;
	virtual void RD_HouseInfoLoadOK			( int HouseDBID ) = 0;
							
	virtual void RC_VisitHouseRequest		( BaseItemObject* Obj , int HouseDBID , char* Password ) = 0; 
	virtual void RC_VisitHouseRequest_ByName( BaseItemObject* Obj , char* RoleName , char* Password ) = 0;
	virtual void RC_CloseVisitHouse			( BaseItemObject* Obj ) = 0;
	virtual void RC_SwapBodyHouseItem		( BaseItemObject* Obj , int HouseParentItemDBID , int HousePos , int BodyPos , bool IsFromBody ) = 0;
	virtual void RC_SwapHouseItemRequest	( BaseItemObject* Obj , int HouseParentItemDBID[2] , int HousePos[2] ) = 0;
	virtual void RC_HouseItemLayoutRequest	( BaseItemObject* Obj , int ItemDBID , House3DLayoutStruct& Layout ) = 0;
	virtual void RC_ChangeHouseRequest		( BaseItemObject* Obj , int HouseType ) = 0;
	virtual void RC_BuyFunctionRequest		( BaseItemObject* Obj , int FunctionID ) = 0;
	virtual void RC_FunctionSettingRequest	( BaseItemObject* Obj , int FunctionID , int Mode ) = 0;
	virtual void RC_BuyEnergyRequest		( BaseItemObject* Obj , int Money_Account , const char* Password ) = 0;

	virtual void RC_LeaveHouse				( BaseItemObject* Obj , int PlayerDBID ) = 0;
	virtual void RC_BoxInfoRequset			( BaseItemObject* Obj , int BoxItemDBID ) = 0;

	virtual void RC_ExchangeEq				( BaseItemObject* Obj , int BoxItemDBID , int BoxPos , EQWearPosENUM EqPos ) = 0;
	virtual void RC_SetPassword				( BaseItemObject* Obj , const char* Password ) = 0; 
	virtual void RC_DestroyItemRequest		( BaseItemObject* Obj , int ItemDBID ) = 0;
	virtual void RC_SetHouseName			( BaseItemObject* Obj , const char* HouseName ) = 0; 
	virtual void RC_SetClientData			( BaseItemObject* Obj , const char* ClientData ) = 0; 

	virtual void RD_FriendDBID				( int HouseDBID , int FriendDBID ) = 0;
	virtual void RC_ModifyFriendDBID		( BaseItemObject* Obj , bool IsDelete , int FriendDBID ) = 0;
	//////////////////////////////////////////////////////////////////////////
	//贺从
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_PlantClearRequest		( BaseItemObject* Obj , int HousePos ) = 0;	
	virtual void RC_PlantItemRequest		( BaseItemObject* Obj , PlantItemTypeENUM Type , int PotHousePos , int ItemBodyPos) = 0;	
	virtual void RC_PlantLockRequest		( BaseItemObject* Obj , int PotHousePos ) = 0;	
	virtual void RC_PlantGetProductRequest	( BaseItemObject* Obj , int PotHousePos ) = 0;

	static void SC_PlantClearResult			( int SendToID , int HousePos , PlantClearResultTypeENUM Result );
	static void SC_PlantItemResult			( int SendToID , PlantItemTypeENUM Type , int PotHousePos , int ItemBodyPos , PlantItemResultTypeENUM Result );
	static void SC_PlantLockResult			( int SendToID , int PotHousePos , PlantLockResultTypeENUM Result );
	static void SC_PlantGetProductResult	( int SendToID , int PotHousePos , PlantGetProductResultTypeENUM Result);
	//////////////////////////////////////////////////////////////////////////
	static void SC_FriendItemLog			( int SendToID , HouseFriendItemLogStruct& Log );
	static void SC_ClearItemLogResult		( int SendToID , bool Result );
	virtual void RC_ClearItemLogRequest		( BaseItemObject* Obj ) = 0;








	//////////////////////////////////////////////////////////////////////////
	//共
	//////////////////////////////////////////////////////////////////////////

	//璶―┷兑
	virtual	void RC_ServantHireListRequest( BaseItemObject* OwnerObj ) = 0;
		//┷兑莱
		static	void SC_ServantHireList( int SendtoID , int ID , bool End , HouseServantStruct Servant );
	//┷兑共
	virtual	void RC_ServantHireRequest( BaseItemObject* OwnerObj , int ID , int Pos ) = 0;
		//┷兑共挡狦
		static	void SC_ServantHireResultOK( int SendtoID , int ID , int Pos , HouseServantStruct Servant );
		static	void SC_ServantHireResultFailed( int SendtoID , int ID );
	//璶―埃共
	virtual	void RC_ServantDelRequest( BaseItemObject* OwnerObj , int ServantID ) = 0;
		//共埃挡狦
		static	void SC_ServantDelResult( int SendtoID , int ServantID , bool Result );
	//共が笆ㄆン
	virtual	void RC_ServantEventRequest( BaseItemObject* OwnerObj , int ServantID , int EventType1 , int EventType2 ) = 0;
		//共が笆ㄆン硄
		static	void SC_ServantEvent( int SendtoID , int ServantID , int EventID );
	//共耚砞
	virtual	void RC_ServantLayout( BaseItemObject* OwnerObj , int ServantID , House3DLayoutStruct& Layout ) = 0;
	//共㏑
	virtual	void RC_ServantRenameRequest( BaseItemObject* OwnerObj , int ServantID , const char* Name ) = 0;
		//共㏑挡狦
		static	void SC_ServantRenameResult( int SendtoID , const char* Name , bool Result );

	//タ共计
	static	void SC_FixServantValue( int RoomID , int	ServantID , HouseServantValueTypeENUM Type , int Value );
};											

											
