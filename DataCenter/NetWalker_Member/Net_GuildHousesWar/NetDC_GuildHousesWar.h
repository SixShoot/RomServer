/*
#pragma once
#include "PG/PG_GuildHousesWar.h"
#include "../../MainProc/Global.h"

class CNetDC_GuildHousesWar : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetDC_GuildHousesWar*	This;

	static bool				_Init();
	static bool				_Release();

	static void _PG_GuildHousesWar_LtoD_OpenMenu		( int ServerID , int Size , void* Data );
	static void _PG_GuildHousesWar_LtoD_WarRegister		( int ServerID , int Size , void* Data );
	static void _PG_GuildHousesWar_LtoD_LoadHouseBaseInfo( int ServerID , int Size , void* Data );
	static void _PG_GuildHousesWar_LtoD_WarEndOK		( int ServerID , int Size , void* Data );
	static void _PG_GuildHousesWar_LtoD_DebugTime		( int ServerID , int Size , void* Data );
	static void _PG_GuildHousesWar_LtoD_HistoryRequest	( int ServerID , int Size , void* Data );
public:
	//////////////////////////////////////////////////////////////////////////
	//約戰 與 進入
	static void SC_OpenMenu				( int PlayerDBID , GuildHouseWarStateENUM State , vector<GuildHouseWarInfoStruct> List );
	static void SC_WarRegisterResult	( int PlayerDBID , GuildHouseWarRegisterTypeENUM Type , bool Result );
	static void SL_WarBegin				( int GuildID1 , int GuildID2 );
	static void SL_WarEnd				( );

	static void SL_HouseBaseInfo		( int ServerID , GuildHousesInfoStruct&	HouseBaseInfo );
	static void SL_BuildingInfo			( int ServerID , GuildHouseBuildingInfoStruct& Building );
	static void SL_FurnitureItemInfo	( int ServerID , GuildHouseFurnitureItemStruct& Item );
	static void SL_HouseInfoLoadOK		( int ServerID , int GuildID );

	virtual void RL_OpenMenu			( int PlayerDBID ) = 0;
	virtual void RL_WarRegister			( int PlayerDBID , GuildHouseWarRegisterTypeENUM Type ) = 0;
	virtual void RL_LoadHouseBaseInfo	( int ServerID ) = 0;
	virtual void RL_WarEndOK			( int ServerID ) = 0;

	virtual void RL_DebugTime			( int ServerID , int Time_Day , int Time_Hour ) = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual void RL_HistoryRequest		( int PlayerDBID , int DayBefore ) = 0;
	static void SC_HistoryResult		( int PlayerDBID , int DayBefore , vector<GuildHouseWarHistoryStruct> List );
};
*/