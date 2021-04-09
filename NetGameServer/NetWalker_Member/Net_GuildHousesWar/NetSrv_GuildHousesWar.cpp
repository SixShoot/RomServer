/*
#include "../NetWakerGSrvInc.h"
#include "NetSrv_GuildHousesWar.h"
#include "../../mainproc/GuildHouseManage/GuildHousesClass.h"
//-------------------------------------------------------------------
NetSrv_GuildHousesWar*      NetSrv_GuildHousesWar::This         = NULL;
//-------------------------------------------------------------------
bool NetSrv_GuildHousesWar::_Init()
{	
	
	Srv_NetCliReg( PG_GuildHousesWar_CtoL_OpenMenu				);
	Srv_NetCliReg( PG_GuildHousesWar_CtoL_WarRegister			);
	Srv_NetCliReg( PG_GuildHousesWar_CtoL_EnterWar				);
	Srv_NetCliReg( PG_GuildHousesWar_CtoL_LeaveWar				);
//	Srv_NetCliReg( PG_GuildHousesWar_DtoL_WarEnd				);

	Srv_NetCliReg( PG_GuildHousesWar_CtoL_PricesRequst			);
	Srv_NetCliReg( PG_GuildHousesWar_CtoL_PlayerScoreRequest	);

//	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHousesWar_DtoL_WarEnd				, _PG_GuildHousesWar_DtoL_WarEnd	);

	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHousesWar_DtoL_WarEnd				, _PG_GuildHousesWar_DtoL_WarEnd	);
	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHousesWar_DtoL_WarBegin			, _PG_GuildHousesWar_DtoL_WarBegin	);


	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHousesWar_DtoL_HouseBaseInfo		, _PG_GuildHousesWar_DtoL_HouseBaseInfo			);
	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHousesWar_DtoL_BuildingInfo		, _PG_GuildHousesWar_DtoL_BuildingInfo			);
	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHousesWar_DtoL_HouseInfoLoadOK		, _PG_GuildHousesWar_DtoL_HouseInfoLoadOK		);
	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHousesWar_DtoL_FurnitureItemInfo	, _PG_GuildHousesWar_DtoL_FurnitureItemInfo		);

	_Net->RegOnSrvPacketFunction	( EM_PG_GuildHousesWar_CtoL_HistoryRequest		, _PG_GuildHousesWar_CtoL_HistoryRequest		);
	return true;
}
//-------------------------------------------------------------------
bool NetSrv_GuildHousesWar::_Release()
{
    return true;
}

void NetSrv_GuildHousesWar::_PG_GuildHousesWar_CtoL_PlayerScoreRequest	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHousesWar_CtoL_PlayerScoreRequest* pg =(PG_GuildHousesWar_CtoL_PlayerScoreRequest*)data;
	This->RC_PlayerScoreRequest( Obj );
}

void NetSrv_GuildHousesWar::_PG_GuildHousesWar_CtoL_HistoryRequest		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHousesWar_CtoL_HistoryRequest* pg =(PG_GuildHousesWar_CtoL_HistoryRequest*)data;
	This->RC_HistoryRequest( Obj , pg->DayBefore );
}

void NetSrv_GuildHousesWar::_PG_GuildHousesWar_CtoL_PricesRequst	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHousesWar_CtoL_PricesRequst* pg =(PG_GuildHousesWar_CtoL_PricesRequst*)data;
	This->RC_PricesRequst( Obj );
}

void NetSrv_GuildHousesWar::_PG_GuildHousesWar_CtoL_OpenMenu		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHousesWar_CtoL_OpenMenu* pg =(PG_GuildHousesWar_CtoL_OpenMenu*)data;
	This->RC_OpenMenu( Obj );
}

void NetSrv_GuildHousesWar::_PG_GuildHousesWar_DtoL_HouseBaseInfo		( int sockid , int size , void* data )
{
	PG_GuildHousesWar_DtoL_HouseBaseInfo* pg =(PG_GuildHousesWar_DtoL_HouseBaseInfo*)data;
	This->RD_HouseBaseInfo( pg->HouseBaseInfo );
}
void NetSrv_GuildHousesWar::_PG_GuildHousesWar_DtoL_BuildingInfo		( int sockid , int size , void* data )
{
	PG_GuildHousesWar_DtoL_BuildingInfo* pg =(PG_GuildHousesWar_DtoL_BuildingInfo*)data;
	This->RD_BuildingInfo( pg->Building );
}
void NetSrv_GuildHousesWar::_PG_GuildHousesWar_DtoL_HouseInfoLoadOK		( int sockid , int size , void* data )
{
	PG_GuildHousesWar_DtoL_HouseInfoLoadOK* pg =(PG_GuildHousesWar_DtoL_HouseInfoLoadOK*)data;
	This->RD_HouseInfoLoadOK( pg->GuildID );

}
void NetSrv_GuildHousesWar::_PG_GuildHousesWar_DtoL_FurnitureItemInfo	( int sockid , int size , void* data )
{
	PG_GuildHousesWar_DtoL_FurnitureItemInfo* pg =(PG_GuildHousesWar_DtoL_FurnitureItemInfo*)data;
	This->RD_FurnitureItemInfo( pg->Item );
}

void NetSrv_GuildHousesWar::_PG_GuildHousesWar_DtoL_WarEnd			( int sockid , int size , void* data )
{
	PG_GuildHousesWar_DtoL_WarEnd* pg =(PG_GuildHousesWar_DtoL_WarEnd*)data;
	This->RD_WarEnd( );
}

void NetSrv_GuildHousesWar::_PG_GuildHousesWar_DtoL_WarBegin		( int sockid , int size , void* data )
{
	PG_GuildHousesWar_DtoL_WarBegin* pg =(PG_GuildHousesWar_DtoL_WarBegin*)data;
	This->RD_WarBegin( pg->GuildID );
}

void NetSrv_GuildHousesWar::_PG_GuildHousesWar_CtoL_WarRegister		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHousesWar_CtoL_WarRegister* pg =(PG_GuildHousesWar_CtoL_WarRegister*)data;
	This->RC_WarRegister( Obj , pg->Type );

}
void NetSrv_GuildHousesWar::_PG_GuildHousesWar_CtoL_EnterWar		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHousesWar_CtoL_EnterWar* pg =(PG_GuildHousesWar_CtoL_EnterWar*)data;
	This->RC_EnterWar( Obj );
}
void NetSrv_GuildHousesWar::_PG_GuildHousesWar_CtoL_LeaveWar		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_GuildHousesWar_CtoL_LeaveWar* pg =(PG_GuildHousesWar_CtoL_LeaveWar*)data;
	This->RC_LeaveWar( Obj );
}


void NetSrv_GuildHousesWar::SD_OpenMenu		( int PlayerDBID )
{
	PG_GuildHousesWar_LtoD_OpenMenu Send;
	Send.PlayerDBID = PlayerDBID;
	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_GuildHousesWar::SD_WarRegister	( int PlayerDBID , GuildHouseWarRegisterTypeENUM Type )
{
	PG_GuildHousesWar_LtoD_WarRegister Send;
	Send.PlayerDBID = PlayerDBID;
	Send.Type		= Type;
	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_GuildHousesWar::SD_WarEndOK				( )
{
	PG_GuildHousesWar_LtoD_WarEndOK Send;
	Global::SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_GuildHousesWar::SC_AllPlayer_WarBegin		(  int GuildID[2] )
{
	PG_GuildHousesWar_LtoC_WarBegin Send;
	Send.GuildID[0] = GuildID[0];
	Send.GuildID[1] = GuildID[1];
	//Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
	Global::SendToAllPlayer( sizeof(Send) , &Send );
}
void NetSrv_GuildHousesWar::SC_EnterWarResult	( int SendToID , bool Result )
{
	PG_GuildHousesWar_LtoC_EnterWarResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
void NetSrv_GuildHousesWar::SC_LeaveWarResult		( int SendToID , bool Result )
{
	PG_GuildHousesWar_LtoC_LeaveWarResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
void NetSrv_GuildHousesWar::SC_AllPlayer_EndWar		( int GuildID1 , int GuildID2 , int Score1 , int Score2 )
{
	PG_GuildHousesWar_LtoC_EndWar Send;
	Send.GuildID[0] = GuildID1;
	Send.GuildID[1] = GuildID2;
	Send.Score[0] = Score1;
	Send.Score[1] = Score2;
	Global::SendToAllPlayer( sizeof(Send) , &Send );
}

void NetSrv_GuildHousesWar::SC_EndWarPlayerInfo		( int SendToID , vector< GuildWarPlayerInfoStruct* >& List1 , vector< GuildWarPlayerInfoStruct* >& List2 )
{
	PG_GuildHousesWar_LtoC_EndWarPlayerInfo Send;

	Send.Count = int( List1.size() + List2.size() );
	if( Send.Count > 1000 )
	{
		Print( LV3 , "SC_AllPlayer_EndWar" , "Send.Count > 1000" );
		return;
	}

	for( unsigned i = 0 ; i < List1.size() ; i++ )
	{
		Send.PlayerList[i] = *(List1[i]);
	}
	for( unsigned i = 0 ; i < List2.size() ; i++ )
	{
		Send.PlayerList[ i + List1.size() ] = *(List2[ i ]);
	}

	Global::SendToCli_ByGUID( SendToID ,Send.Size() , &Send );
}

void NetSrv_GuildHousesWar::SD_LoadHouseBaseInfo	( )
{
	PG_GuildHousesWar_LtoD_LoadHouseBaseInfo Send;
	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_GuildHousesWar::SD_DebugTime				( int Time_Day , int Time_Hour )
{
	PG_GuildHousesWar_LtoD_DebugTime Send;
	Send.Time_Hour = Time_Hour % 24;
	Send.Time_Day  = Time_Day % 7;
	Global::SendToDBCenter( sizeof( Send ) , &Send );

}

void NetSrv_GuildHousesWar::SC_PricesResult			( int SendToID , bool Result )
{
	PG_GuildHousesWar_LtoC_PricesResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}

void  NetSrv_GuildHousesWar::SD_HistoryRequest	( int PlayerDBID , int DayBefore )
{
	PG_GuildHousesWar_LtoD_HistoryRequest Send;
	Send.DayBefore = DayBefore;
	Send.PlayerDBID = PlayerDBID;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}

void  NetSrv_GuildHousesWar::SC_AllRoom_GuildScore	( int RoomID , GuildHouseWarFightStruct& Info )
{
	PG_GuildHousesWar_LtoC_GuildScore	Send;
	Send.Info = Info;

	Global::SendToCli_ByRoomID( RoomID , sizeof(Send) , &Send );
	//Global::SendToAllCli( sizeof(Send) , &Send );
}

void  NetSrv_GuildHousesWar::SC_PlayerScore		( int SendToID , vector< GuildWarPlayerInfoStruct* >& List )
{
	PG_GuildHousesWar_LtoC_PlayerScore Send;

	Send.Count = int( List.size() );
	if( Send.Count > 1000 )
	{
		Print( LV3 , "SC_PlayerScore" , "Send.Count > 1000" );
		return;
	}

	for( unsigned i = 0 ; i < List.size() ; i++ )
	{
		Send.PlayerList[i] = *(List[i]);
	}

	Global::SendToCli_ByGUID( SendToID , Send.Size() , &Send );
}
*/