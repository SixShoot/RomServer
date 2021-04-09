/*
#include "NetDC_GuildHousesWar.h"
//-------------------------------------------------------------------
CNetDC_GuildHousesWar*	CNetDC_GuildHousesWar::This = NULL;
//-------------------------------------------------------------------
bool CNetDC_GuildHousesWar::_Init()
{
	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHousesWar_LtoD_OpenMenu			, _PG_GuildHousesWar_LtoD_OpenMenu			);
	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHousesWar_LtoD_WarRegister			, _PG_GuildHousesWar_LtoD_WarRegister		);
	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHousesWar_LtoD_LoadHouseBaseInfo	, _PG_GuildHousesWar_LtoD_LoadHouseBaseInfo	);
	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHousesWar_LtoD_WarEndOK			, _PG_GuildHousesWar_LtoD_WarEndOK			);
	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHousesWar_LtoD_DebugTime			, _PG_GuildHousesWar_LtoD_DebugTime			);
	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHousesWar_LtoD_HistoryRequest		, _PG_GuildHousesWar_LtoD_HistoryRequest	);
	return true;
}
//-------------------------------------------------------------------
bool CNetDC_GuildHousesWar::_Release()
{

	return true;
}
void CNetDC_GuildHousesWar::_PG_GuildHousesWar_LtoD_HistoryRequest	( int ServerID , int Size , void* Data )
{
	PG_GuildHousesWar_LtoD_HistoryRequest* pg =(PG_GuildHousesWar_LtoD_HistoryRequest*)Data;
	This->RL_HistoryRequest( pg->PlayerDBID , pg->DayBefore );
}
void CNetDC_GuildHousesWar::_PG_GuildHousesWar_LtoD_DebugTime		( int ServerID , int Size , void* Data )
{
	PG_GuildHousesWar_LtoD_DebugTime* pg =(PG_GuildHousesWar_LtoD_DebugTime*)Data;
	This->RL_DebugTime( ServerID , pg->Time_Day , pg->Time_Hour );
}

void CNetDC_GuildHousesWar::_PG_GuildHousesWar_LtoD_WarEndOK			( int ServerID , int Size , void* Data )
{
	PG_GuildHousesWar_LtoD_WarEndOK* pg =(PG_GuildHousesWar_LtoD_WarEndOK*)Data;
	This->RL_WarEndOK( ServerID );
}

void CNetDC_GuildHousesWar::_PG_GuildHousesWar_LtoD_LoadHouseBaseInfo( int ServerID , int Size , void* Data )
{
	PG_GuildHousesWar_LtoD_LoadHouseBaseInfo* pg =(PG_GuildHousesWar_LtoD_LoadHouseBaseInfo*)Data;
	This->RL_LoadHouseBaseInfo( ServerID );
}

void CNetDC_GuildHousesWar::_PG_GuildHousesWar_LtoD_OpenMenu		( int ServerID , int Size , void* Data )
{
	PG_GuildHousesWar_LtoD_OpenMenu* pg =(PG_GuildHousesWar_LtoD_OpenMenu*)Data;
	This->RL_OpenMenu( pg->PlayerDBID );
}

void CNetDC_GuildHousesWar::_PG_GuildHousesWar_LtoD_WarRegister		( int ServerID , int Size , void* Data )
{
	PG_GuildHousesWar_LtoD_WarRegister* pg =(PG_GuildHousesWar_LtoD_WarRegister*)Data;
	This->RL_WarRegister( pg->PlayerDBID , pg->Type );
}

void CNetDC_GuildHousesWar::SC_OpenMenu				( int PlayerDBID , GuildHouseWarStateENUM State , vector<GuildHouseWarInfoStruct> List )
{
	PG_GuildHousesWar_DtoC_OpenMenu Send;

	Send.State = State;
	Send.Size = (int)List.size();
	if( Send.Size > 1000 )
		Send.Size = 1000;

	for( int i = 0 ; i < Send.Size ; i++ )
	{
		Send.List[i] = List[i];
	}

	Global::SendToCli_ByDBID( PlayerDBID , sizeof(Send) , &Send );
}
void CNetDC_GuildHousesWar::SC_WarRegisterResult	( int PlayerDBID , GuildHouseWarRegisterTypeENUM Type , bool Result )
{
	PG_GuildHousesWar_DtoC_WarRegisterResult Send;
	Send.Type = Type;
	Send.Result = Result;
	Global::SendToCli_ByDBID( PlayerDBID , sizeof(Send) , &Send );
}
void CNetDC_GuildHousesWar::SL_WarBegin				( int GuildID1 , int GuildID2 )
{
	PG_GuildHousesWar_DtoL_WarBegin Send;
	Send.GuildID[0] = GuildID1;
	Send.GuildID[1] = GuildID2;

	Global::SendToAllLocal( sizeof( Send ) , &Send );
}

void CNetDC_GuildHousesWar::SL_WarEnd				( )
{
	PG_GuildHousesWar_DtoL_WarEnd Send;

	Global::SendToAllLocal( sizeof( Send ) , &Send );
}
void CNetDC_GuildHousesWar::SL_HouseBaseInfo		( int ServerID , GuildHousesInfoStruct&	HouseBaseInfo )
{
	PG_GuildHousesWar_DtoL_HouseBaseInfo Send;
	Send.HouseBaseInfo = HouseBaseInfo;

	Global::SendToSrvBySockID( ServerID , sizeof(Send) ,&Send );
}
void CNetDC_GuildHousesWar::SL_BuildingInfo			( int ServerID , GuildHouseBuildingInfoStruct& Building )
{
	PG_GuildHousesWar_DtoL_BuildingInfo Send;
	Send.Building = Building;

	Global::SendToSrvBySockID( ServerID , sizeof(Send) ,&Send );
}
void CNetDC_GuildHousesWar::SL_HouseInfoLoadOK		( int ServerID , int GuildID )
{
	PG_GuildHousesWar_DtoL_HouseInfoLoadOK Send;
	Send.GuildID = GuildID;
	Global::SendToSrvBySockID( ServerID , sizeof(Send) ,&Send );
}
void CNetDC_GuildHousesWar::SL_FurnitureItemInfo	( int ServerID , GuildHouseFurnitureItemStruct& Item )
{
	PG_GuildHousesWar_DtoL_FurnitureItemInfo Send;
	Send.Item = Item;

	Global::SendToSrvBySockID( ServerID , sizeof(Send) ,&Send );
}
void CNetDC_GuildHousesWar::SC_HistoryResult		( int PlayerDBID , int DayBefore , vector<GuildHouseWarHistoryStruct> List )
{
	PG_GuildHousesWar_DtoC_HistoryResult Send;
	Send.DayBefore = DayBefore;
	Send.Count = List.size();
	if( Send.Count >= 1000 )
		Send.Count = 1000;
	for( int i = 0 ; i < Send.Count ; i++ )
	{
		Send.List[i] = List[i];
	}
	Global::SendToCli_ByDBID( PlayerDBID , sizeof(Send) , &Send );
}
*/