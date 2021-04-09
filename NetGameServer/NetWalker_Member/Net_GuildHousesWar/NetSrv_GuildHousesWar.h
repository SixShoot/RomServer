/*
#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_GuildHousesWar.h"

class NetSrv_GuildHousesWar : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_GuildHousesWar* This;
    static bool _Init();
    static bool _Release();

	static void _PG_GuildHousesWar_CtoL_OpenMenu		( int sockid , int size , void* data );
	static void _PG_GuildHousesWar_CtoL_WarRegister		( int sockid , int size , void* data );
	static void _PG_GuildHousesWar_DtoL_WarBegin		( int sockid , int size , void* data );
	static void _PG_GuildHousesWar_CtoL_EnterWar		( int sockid , int size , void* data );
	static void _PG_GuildHousesWar_CtoL_LeaveWar		( int sockid , int size , void* data );
	static void _PG_GuildHousesWar_DtoL_WarEnd			( int sockid , int size , void* data );
	static void _PG_GuildHousesWar_CtoL_PricesRequst	( int sockid , int size , void* data );

	static void _PG_GuildHousesWar_DtoL_HouseBaseInfo		( int sockid , int size , void* data );
	static void _PG_GuildHousesWar_DtoL_BuildingInfo		( int sockid , int size , void* data );
	static void _PG_GuildHousesWar_DtoL_HouseInfoLoadOK		( int sockid , int size , void* data );
	static void _PG_GuildHousesWar_DtoL_FurnitureItemInfo	( int sockid , int size , void* data );
	static void _PG_GuildHousesWar_CtoL_HistoryRequest		( int sockid , int size , void* data );
	static void _PG_GuildHousesWar_CtoL_PlayerScoreRequest	( int sockid , int size , void* data );
public:    

	static void SD_OpenMenu				( int PlayerDBID );
	static void SD_WarRegister			( int PlayerDBID , GuildHouseWarRegisterTypeENUM Type );
	static void SC_AllPlayer_WarBegin	( int GuildID[2] );
	static void SC_EnterWarResult		( int SendToID , bool Result );
	static void SC_LeaveWarResult		( int SendToID , bool Result );
	static void SC_AllPlayer_EndWar		( int GuildID1 , int GuildID2 , int Score1 , int Score2 );
	static void SC_EndWarPlayerInfo		( int SendToID , vector< GuildWarPlayerInfoStruct* >& List1 , vector< GuildWarPlayerInfoStruct* >& List2 );

	static void SD_WarEndOK				( );
	static void SD_DebugTime			( int Time_Day , int Time_Hour );
	static void SC_PricesResult			( int SendToID , bool Result );

	virtual void RC_OpenMenu		( BaseItemObject* Obj ) = 0;
	virtual void RD_WarBegin		( int GuildID[2] ) = 0;
	virtual void RD_WarEnd			( ) = 0;
	virtual void RC_WarRegister		( BaseItemObject* Obj , GuildHouseWarRegisterTypeENUM Type ) = 0;
	virtual void RC_EnterWar		( BaseItemObject* Obj ) = 0;
	virtual void RC_LeaveWar		( BaseItemObject* Obj ) = 0;
	virtual void RC_PricesRequst	( BaseItemObject* Obj ) = 0;
	//////////////////////////////////////////////////////////////////////////
	//戰場積分資料
	static void SC_AllRoom_GuildScore	( int RoomID , GuildHouseWarFightStruct& Info );
	//角色積分資料
	static void SC_PlayerScore		( int SendToID , vector< GuildWarPlayerInfoStruct* >& List );
	
	virtual void RC_PlayerScoreRequest( BaseItemObject* Obj ) = 0;
	//////////////////////////////////////////////////////////////////////////
	static void SD_LoadHouseBaseInfo	( );

	virtual void RD_HouseBaseInfo		( GuildHousesInfoStruct& HouseBaseInfo ) = 0;
	virtual void RD_BuildingInfo		( GuildHouseBuildingInfoStruct& Building ) = 0;
	virtual void RD_FurnitureItemInfo	( GuildHouseFurnitureItemStruct& Item ) = 0;
	virtual void RD_HouseInfoLoadOK		( int GuildID ) = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual void RC_HistoryRequest	( BaseItemObject* Obj , int DayBefore ) = 0;
	static void  SD_HistoryRequest	( int PlayerDBID , int DayBefore );
};										


											
*/