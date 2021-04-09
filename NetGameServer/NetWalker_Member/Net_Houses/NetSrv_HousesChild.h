#pragma once

#include "NetSrv_Houses.h"
#include "../../mainproc/HousesManage/HousesManageClass.h"

class NetSrv_HousesChild : public NetSrv_Houses
{
public:
    static bool Init();
    static bool Release();

	static void SC_FixHouseItemProc			( RoleDataEx* Owner , HouseItemStruct*  HouseParentItem , HouseItemDBStruct& HouseItem , int RoomID = -1 );
	static void SendAllCli_FixHouseBase		( int RoomID , HouseBaseDBStruct& HouseBaseInfo  );
	static void SendAllCli_AllHouseItem		( int HouseDBID );
	static void SendAllCli_ChangeHouseResult( int HouseDBID , int HouseType , bool Result );
	static void SendAllCli_ModifyFriendDBID	( int HouseDBID , bool IsDelete , int FriendDBID );
	static void SendCli_FriendDBIDList		( int SendID ,  HousesManageClass* houseClass );

	virtual void RD_BuildHouseResult		( int PlayerDBID , int HouseDBID , bool Result );
	virtual void RD_FindHouseResult			( int PlayerDBID , int HouseDBID , FindHouseResultENUM Result );	
	virtual void RD_HouseBaseInfo			( HouseBaseDBStruct& HouseBaseInfo );	
	virtual void RD_ItemInfo				( int HouseDBID , int ItemDBID , int ItemCount , HouseItemDBStruct Item[] );
	virtual void RD_HouseInfoLoadOK			( int HouseDBID );
	virtual void RC_VisitHouseRequest		( BaseItemObject* Obj , int HouseDBID , char* Password ); 
	virtual void RC_VisitHouseRequest_ByName( BaseItemObject* Obj , char* RoleName , char* Password );
	virtual void RC_CloseVisitHouse			( BaseItemObject* Obj );
	virtual void RC_SwapBodyHouseItem		( BaseItemObject* Obj , int HouseParentItemDBID , int HousePos , int BodyPos , bool IsFromBody );
	virtual void RC_SwapHouseItemRequest	( BaseItemObject* Obj , int HouseParentItemDBID[2] , int HousePos[2] );
	virtual void RC_HouseItemLayoutRequest	( BaseItemObject* Obj , int ItemDBID , House3DLayoutStruct& Layout );
	virtual void RC_ChangeHouseRequest		( BaseItemObject* Obj , int HouseType );
	virtual void RC_BuyFunctionRequest		( BaseItemObject* Obj , int FunctionID );
	virtual void RC_FunctionSettingRequest	( BaseItemObject* Obj , int FunctionID , int Mode );
	virtual void RC_BuyEnergyRequest		( BaseItemObject* Obj , int Money_Account , const char* Password );
	virtual void RC_LeaveHouse				( BaseItemObject* Obj , int PlayerDBID );
	virtual void RC_BoxInfoRequset			( BaseItemObject* Obj , int BoxItemDBID );
	virtual void RC_ExchangeEq				( BaseItemObject* Obj , int BoxItemDBID , int BoxPos , EQWearPosENUM EqPos );
	virtual void RC_SetPassword				( BaseItemObject* Obj , const char* Password ); 
	virtual void RC_DestroyItemRequest		( BaseItemObject* Obj , int ItemDBID );
	virtual void RC_SetHouseName			( BaseItemObject* Obj , const char* HouseName ); 
	virtual void RC_SetClientData			( BaseItemObject* Obj , const char* ClientData );

	virtual void RD_FriendDBID				( int HouseDBID , int FriendDBID );
	virtual void RC_ModifyFriendDBID		( BaseItemObject* Obj , bool IsDelete , int FriendDBID );
	//////////////////////////////////////////////////////////////////////////
	//贺从
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_PlantClearRequest		( BaseItemObject* Obj , int HousePos );
	virtual void RC_PlantItemRequest		( BaseItemObject* Obj , PlantItemTypeENUM Type , int PotHousePos , int ItemBodyPos);
	virtual void RC_PlantLockRequest		( BaseItemObject* Obj , int PotHousePos );
	virtual void RC_PlantGetProductRequest	( BaseItemObject* Obj , int PotHousePos );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_ClearItemLogRequest		( BaseItemObject* Obj );



	//////////////////////////////////////////////////////////////////////////
	//共
	//////////////////////////////////////////////////////////////////////////
protected:
	static map< int , HouseServantStruct >	_ServantList;
	static int		_ServantMaxID;

	//﹚玻ネ侗ノ共
	static  int		_OnTime_UpdateServantHireInfo	(SchedularInfo* Obj, void* InputClass );

public:
	//璶―┷兑
	virtual	void RC_ServantHireListRequest( BaseItemObject* OwnerObj );
	//┷兑共
	virtual	void RC_ServantHireRequest( BaseItemObject* OwnerObj , int ID , int Pos );
	//璶―埃共
	virtual	void RC_ServantDelRequest( BaseItemObject* OwnerObj , int ServantID );
	//共が笆ㄆン
	virtual	void RC_ServantEventRequest( BaseItemObject* OwnerObj , int ServantID , int EventType1 , int EventType2 );
	//共耚砞
	virtual	void RC_ServantLayout( BaseItemObject* OwnerObj , int ServantID , House3DLayoutStruct& Layout );
	//共㏑
	virtual	void RC_ServantRenameRequest( BaseItemObject* OwnerObj , int ServantID , const char* Name );

};

