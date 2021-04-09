
#pragma once
#include "PG/PG_Houses.h"
#include "../../MainProc/Global.h"

class CNetDC_Houses : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetDC_Houses*	This;

	static bool				_Init();
	static bool				_Release();

	static void _PG_Houses_LtoD_BuildHouseRequest			( int ServerID , int Size , void* Data );	
	static void _PG_Houses_LtoD_FindHouseRequest			( int ServerID , int Size , void* Data );	
	static void _PG_Houses_LtoD_HouseInfoLoading			( int ServerID , int Size , void* Data );	
//	static void _PG_Houses_LtoD_SaveSwapBodyHouseItem		( int ServerID , int Size , void* Data );	
	static void _PG_Houses_LtoD_SaveHouseBase				( int ServerID , int Size , void* Data );	
	static void _PG_Houses_LtoD_SaveItem					( int ServerID , int Size , void* Data );
	static void _PG_Houses_LtoD_FindHouseRequest_ByName		( int ServerID , int Size , void* Data );

public:
	static void SL_BuildHouseResult			( int ServerID , int PlayerDBID , int HouseDBID , bool Result );
	static void SL_FindHouseResult			( int ServerID , int PlayerDBID , int HouseDBID , FindHouseResultENUM Result );
	static void SL_HouseBaseInfo			( int ServerID , HouseBaseDBStruct& HouseBaseInfo );
	static void SL_ItemInfo					( int ServerID , int HouseDBID , int ItemDBID , int ItemCount , HouseItemDBStruct Item[] );
	static void SL_HouseInfoLoadOK			( int ServerID , int HouseDBID  );
	static void SL_FriendDBID				( int ServerID , int HouseDBID , int FriendDBID );

	virtual void RL_BuildHouseRequest		( int ServerID , int PlayerDBID , int HouseType , const char* Account_ID ) = 0;
	virtual void RL_FindHouseRequest		( int ServerID , int PlayerDBID , int HouseDBID , char* Password , int ManageLv ) = 0;
	virtual void RL_FindHouseRequest_ByName ( int ServerID , int PlayerDBID , const char* RoleName , const char* Password , int ManageLv ) = 0;
	virtual void RL_HouseInfoLoading		( int ServerID , int HouseDBID ) = 0;
//	virtual void RL_SaveSwapBodyHouseItem	( int ServerID , int PlayerDBID , int BodyPos , ItemFieldStruct& BodyItem ,	HouseItemDBStruct& HouseItem ) = 0;
	virtual void RL_SaveHouseBase			( int ServerID , int PlayerDBID , HouseBaseDBStruct& HouseBaseInfo ) = 0;
	virtual void RL_SaveItem				( int ServerID , int PlayerDBID , HouseItemDBStruct& Item) = 0;

	
};
