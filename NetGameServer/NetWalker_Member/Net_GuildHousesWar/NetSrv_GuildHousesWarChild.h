/*
#pragma once

#include "NetSrv_GuildHousesWar.h"
//////////////////////////////////////////////////////////////////////////
struct HouseItemStruct;

class NetSrv_GuildHousesWarChild : public NetSrv_GuildHousesWar
{
	static int OnEvent_ClearGuildWar	 ( SchedularInfo* Obj, void* InputClass );
public:
    static bool Init();
    static bool Release();

	virtual void RC_OpenMenu		( BaseItemObject* Obj );
	virtual void RD_WarBegin		( int GuildID[2] );
	virtual void RD_WarEnd			( );
	virtual void RC_WarRegister		( BaseItemObject* Obj , GuildHouseWarRegisterTypeENUM Type );
	virtual void RC_EnterWar		( BaseItemObject* Obj );
	virtual void RC_LeaveWar		( BaseItemObject* Obj );
	virtual void RC_PricesRequst	( BaseItemObject* Obj );
	//////////////////////////////////////////////////////////////////////////

	virtual void RD_HouseBaseInfo		( GuildHousesInfoStruct& HouseBaseInfo );
	virtual void RD_BuildingInfo		( GuildHouseBuildingInfoStruct& Building );
	virtual void RD_FurnitureItemInfo	( GuildHouseFurnitureItemStruct& Item );
	virtual void RD_HouseInfoLoadOK		( int GuildID );
	virtual void RC_HistoryRequest		( BaseItemObject* Obj , int DayBefore );

	virtual void RC_PlayerScoreRequest( BaseItemObject* Obj );



};

*/