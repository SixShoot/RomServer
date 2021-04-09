#include "NetSrv_BG_GuildWar.h"
#include "../net_serverlist/Net_ServerList_Child.h"
#include "BG_GuildWarManage/BG_GuildWarManage.h"
//////////////////////////////////////////////////////////////////////////
NetSrv_BG_GuildWar*	NetSrv_BG_GuildWar::This	=NULL;
//////////////////////////////////////////////////////////////////////////
bool NetSrv_BG_GuildWar::_Init()
{
	Srv_NetCliReg           ( PG_BG_GuildWar_CtoBL_GuildNameRequest		);
	Server_Reg_World_Packet ( PG_BG_GuildWar_BLtoL_GuildNameRequest		);
	Server_Reg_World_Packet ( PG_BG_GuildWar_LtoBL_GuildNameResult		);
	Srv_NetCliReg           ( PG_BG_GuildWar_CtoL_OpenMenuRequest		);
	Server_Reg_World_Packet ( PG_BG_GuildWar_LtoBL_OpenMenuRequest		);
	Server_Reg_World_Packet ( PG_BG_GuildWar_BLtoL_OpenMenuResult		);
	Srv_NetCliReg           ( PG_BG_GuildWar_CtoL_RegisterRequest		);
	Server_Reg_World_Packet ( PG_BG_GuildWar_LtoBL_RegisterRequest		);
	Server_Reg_World_Packet ( PG_BG_GuildWar_BLtoL_RegisterResult		);
	Server_Reg_World_Packet ( PG_BG_GuildWar_BLtoL_Status				);
	Server_Reg_World_Packet ( PG_BG_GuildWar_BLtoL_HouseLoadRequest		);
	Server_Reg_World_Packet ( PG_BG_GuildWar_LtoBL_HouseBaseInfo		);
	Server_Reg_World_Packet ( PG_BG_GuildWar_LtoBL_HouseBuildingInfo	);
	Server_Reg_World_Packet ( PG_BG_GuildWar_LtoBL_HouseFurnitureInfo	);
	Server_Reg_World_Packet ( PG_BG_GuildWar_LtoBL_HouseLoadOK			);
	Srv_NetCliReg           ( PG_BG_GuildWar_CtoL_EnterRequest			);
	Server_Reg_World_Packet ( PG_BG_GuildWar_LtoBL_EnterRequest			);
	Server_Reg_World_Packet ( PG_BG_GuildWar_BLtoL_EnterResult			);
	Srv_NetCliReg           ( PG_BG_GuildWar_CtoBL_LeaveRequest			);
	Server_Reg_World_Packet ( PG_BG_GuildWar_LtoBL_DebugTime			);
	Server_Reg_World_Packet ( PG_BG_GuildWar_BLtoL_EndWarResult			);

	Srv_NetCliReg           ( PG_BG_GuildWar_CtoBL_PlayerInfoRequest	);

	_Net->RegOnSrvPacketFunction	( EM_PG_BG_GuildWar_DtoBL_OrderInfo	, _PG_BG_GuildWar_DtoBL_OrderInfo	);

	Global::Net_ServerList->RegServerLoginEvent( EM_SERVER_TYPE_DATACENTER , _OnDataCenterConnectEvent );


	Server_Reg_World_Packet ( PG_BG_GuildWar_DtoBL_RegGuildWarRankInfo				);
	Srv_NetCliReg			( PG_BG_GuildWar_CtoL_GuildWarRankInfoRequest			);
	Server_Reg_World_Packet ( PG_BG_GuildWar_LtoBL_GuildWarRankInfoRequest			);
	Server_Reg_World_Packet ( PG_BG_GuildWar_BLtoL_GuildWarHisotry					);
	Server_Reg_World_Packet ( PG_BG_GuildWar_BLtoL_GuildWarRankInfo					);

	return true;
}
bool NetSrv_BG_GuildWar::_Release()
{
	return true;
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_DtoBL_RegGuildWarRankInfo		( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_DtoBL_RegGuildWarRankInfo* PG = (PG_BG_GuildWar_DtoBL_RegGuildWarRankInfo*)Data;
	This->RD_RegGuildWarRankInfo( PG->Info , PG->Hisotry );
}
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_CtoL_GuildWarRankInfoRequest	( int Sockid , int Size , void* Data )
{
	PG_BG_GuildWar_CtoL_GuildWarRankInfoRequest* PG = (PG_BG_GuildWar_CtoL_GuildWarRankInfoRequest*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return;
	This->RC_GuildWarRankInfoRequest( Obj , PG->Type , PG->WorldGuildID );
}
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_LtoBL_GuildWarRankInfoRequest	( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_LtoBL_GuildWarRankInfoRequest* PG = (PG_BG_GuildWar_LtoBL_GuildWarRankInfoRequest*)Data;
	This->RL_GuildWarRankInfoRequest( FromWorldId , FromNetID , PG->PlayerDBID , PG->Type , PG->WorldGuildID );
}
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_BLtoL_GuildWarHisotry			( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_BLtoL_GuildWarHisotry* PG = (PG_BG_GuildWar_BLtoL_GuildWarHisotry*)Data;
	This->RBL_GuildWarHisotry( PG->PlayerDBID , PG->Hisotry );
}
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_BLtoL_GuildWarRankInfo			( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_BLtoL_GuildWarRankInfo* PG = (PG_BG_GuildWar_BLtoL_GuildWarRankInfo*)Data;
	This->RBL_GuildWarRankInfo( PG->Info , PG->PlayerDBID );
}

void NetSrv_BG_GuildWar::_OnDataCenterConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	This->OnDataCenterConnect( );
}

void NetSrv_BG_GuildWar::_PG_BG_GuildWar_DtoBL_OrderInfo( int Sockid , int Size , void* Data )	
{
	PG_BG_GuildWar_DtoBL_OrderInfo* PG = (PG_BG_GuildWar_DtoBL_OrderInfo*)Data;
	This->RD_OrderInfo( PG->Count , PG->List );

}

void NetSrv_BG_GuildWar::_PG_BG_GuildWar_BLtoL_EndWarResult( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_BLtoL_EndWarResult* PG = (PG_BG_GuildWar_BLtoL_EndWarResult*)Data;
	This->RBL_EndWarResult( PG->GuildID , PG->GameResult , PG->GuildScore , PG->TargetGuild );
}

//要求進入公會戰場
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_CtoBL_PlayerInfoRequest( int Sockid , int Size , void* Data )
{
	PG_BG_GuildWar_CtoBL_PlayerInfoRequest* PG = (PG_BG_GuildWar_CtoBL_PlayerInfoRequest*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return;
	This->RC_PlayerInfoRequest( Obj );
}
///////////////////////////////////////gg///////////////////////////////////
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_LtoBL_DebugTime				( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_LtoBL_DebugTime* PG = (PG_BG_GuildWar_LtoBL_DebugTime*)Data;

	This->RL_DebugTime( FromWorldId , FromNetID , PG->Time_Day , PG->Time_Hour );
}
//////////////////////////////////////////////////////////////////////////
//要求跨伺服器公會名稱要求
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_CtoBL_GuildNameRequest( int Sockid , int Size , void* Data )
{
	PG_BG_GuildWar_CtoBL_GuildNameRequest* PG = (PG_BG_GuildWar_CtoBL_GuildNameRequest*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return;
	This->RC_GuildNameRequest( Obj  , PG->WorldGuildID );
}
//////////////////////////////////////////////////////////////////////////
//要求跨伺服器公會名稱要求
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_BLtoL_GuildNameRequest( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_BLtoL_GuildNameRequest* PG = (PG_BG_GuildWar_BLtoL_GuildNameRequest*)Data;
	This->RBL_GuildNameRequest( FromWorldId , FromNetID , PG->PlayerDBID , PG->WorldGuildID );
}
//////////////////////////////////////////////////////////////////////////
//要求跨伺服器公會名稱結果
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_LtoBL_GuildNameResult( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_LtoBL_GuildNameResult* PG = (PG_BG_GuildWar_LtoBL_GuildNameResult*)Data;
	This->RL_GuildNameResult( FromWorldId , FromNetID , PG->PlayerDBID , PG->WorldGuildID , PG->GuildName.Begin() );
}
//////////////////////////////////////////////////////////////////////////
//開啟註冊公會戰介面
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_CtoL_OpenMenuRequest( int Sockid , int Size , void* Data )
{
	PG_BG_GuildWar_CtoL_OpenMenuRequest* PG = (PG_BG_GuildWar_CtoL_OpenMenuRequest*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return;
	This->RC_OpenMenuRequest( Obj  );
}
//////////////////////////////////////////////////////////////////////////
//要求公會戰資訊
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_LtoBL_OpenMenuRequest( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_LtoBL_OpenMenuRequest* PG = (PG_BG_GuildWar_LtoBL_OpenMenuRequest*)Data;
	This->RL_OpenMenuRequest( FromWorldId , FromNetID , PG->PlayerDBID );
}
//////////////////////////////////////////////////////////////////////////
//公會戰開啟狀況
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_BLtoL_OpenMenuResult( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_BLtoL_OpenMenuResult* PG = (PG_BG_GuildWar_BLtoL_OpenMenuResult*)Data;
	This->RBL_OpenMenuResult( FromWorldId , FromNetID , PG->PlayerDBID , PG->ZoneGroupID , PG->NextTime , PG->State , PG->Count , PG->List );
}
//////////////////////////////////////////////////////////////////////////
//註冊公會戰( or 取消 )
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_CtoL_RegisterRequest( int Sockid , int Size , void* Data )
{
	PG_BG_GuildWar_CtoL_RegisterRequest* PG = (PG_BG_GuildWar_CtoL_RegisterRequest*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return;
	This->RC_RegisterRequest( Obj , PG->ZoneGroupID , PG->Type , PG->GuildCount , PG->IsAcceptAssignment , PG->TargetGuild );
}
//////////////////////////////////////////////////////////////////////////
//註冊公會戰( or 取消 )
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_LtoBL_RegisterRequest( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_LtoBL_RegisterRequest* PG = (PG_BG_GuildWar_LtoBL_RegisterRequest*)Data;
	This->RL_RegisterRequest( FromWorldId , FromNetID , PG->Info );
}
//////////////////////////////////////////////////////////////////////////
//註冊公會戰( or 取消 )結果
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_BLtoL_RegisterResult( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_BLtoL_RegisterResult* PG = (PG_BG_GuildWar_BLtoL_RegisterResult*)Data;
	This->RBL_RegisterResult( FromWorldId , FromNetID , PG->GuildID , PG->Type , PG->Info , PG->Result , PG->PlayerDBID );
}
//////////////////////////////////////////////////////////////////////////
//公會戰狀況( 開始 , 準備 , 結束 )
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_BLtoL_Status( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_BLtoL_Status* PG = (PG_BG_GuildWar_BLtoL_Status*)Data;
	This->RBL_Status( FromWorldId , FromNetID , PG->Status , PG->NextTime );
}
//////////////////////////////////////////////////////////////////////////
//要求公會屋資料(取zone401的資料)
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_BLtoL_HouseLoadRequest( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_BLtoL_HouseLoadRequest* PG = (PG_BG_GuildWar_BLtoL_HouseLoadRequest*)Data;
	This->RBL_HouseLoadRequest( FromWorldId , FromNetID , PG->GuildID );
}
//////////////////////////////////////////////////////////////////////////
//公會屋基本資料
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_LtoBL_HouseBaseInfo( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_LtoBL_HouseBaseInfo* PG = (PG_BG_GuildWar_LtoBL_HouseBaseInfo*)Data;
	This->RL_HouseBaseInfo( FromWorldId , FromNetID , PG->HouseBaseInfo );
}
//////////////////////////////////////////////////////////////////////////
//公會屋建築資料
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_LtoBL_HouseBuildingInfo( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_LtoBL_HouseBuildingInfo* PG = (PG_BG_GuildWar_LtoBL_HouseBuildingInfo*)Data;
	This->RL_HouseBuildingInfo( FromWorldId , FromNetID , PG->Building );
}
//////////////////////////////////////////////////////////////////////////
//公會屋家具資料
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_LtoBL_HouseFurnitureInfo( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_LtoBL_HouseFurnitureInfo* PG = (PG_BG_GuildWar_LtoBL_HouseFurnitureInfo*)Data;
	This->RL_HouseFurnitureInfo( FromWorldId , FromNetID , PG->Item );
}
//////////////////////////////////////////////////////////////////////////
//公會屋資料讀取完畢
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_LtoBL_HouseLoadOK( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_LtoBL_HouseLoadOK* PG = (PG_BG_GuildWar_LtoBL_HouseLoadOK*)Data;
	This->RL_HouseLoadOK( FromWorldId , FromNetID , PG->GuildID );
}
//////////////////////////////////////////////////////////////////////////
//要求進入公會戰場
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_CtoL_EnterRequest( int Sockid , int Size , void* Data )
{
	PG_BG_GuildWar_CtoL_EnterRequest* PG = (PG_BG_GuildWar_CtoL_EnterRequest*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return;
	This->RC_EnterRequest( Obj , PG->ZoneGroupID );
}
//////////////////////////////////////////////////////////////////////////
//要求進入公會戰場
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_LtoBL_EnterRequest( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_LtoBL_EnterRequest* PG = (PG_BG_GuildWar_LtoBL_EnterRequest*)Data;
	This->RL_EnterRequest( FromWorldId , FromNetID , PG->PlayerDBID , PG->GuildID );
}
//////////////////////////////////////////////////////////////////////////
//要求進入結果
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_BLtoL_EnterResult( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_BG_GuildWar_BLtoL_EnterResult* PG = (PG_BG_GuildWar_BLtoL_EnterResult*)Data;
	This->RBL_EnterResult( FromWorldId , FromNetID , PG->ZoneGroupID , PG->PlayerDBID , PG->GuildID , PG->Result );
}
//////////////////////////////////////////////////////////////////////////
//要求離開公會戰場
void NetSrv_BG_GuildWar::_PG_BG_GuildWar_CtoBL_LeaveRequest( int Sockid , int Size , void* Data )
{
	PG_BG_GuildWar_CtoBL_LeaveRequest* PG = (PG_BG_GuildWar_CtoBL_LeaveRequest*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return;
	This->RC_LeaveRequest( Obj  );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SL_GuildNameRequest( int ToWorldID , int ToGSrvID , int PlayerDBID , int WorldGuildID )
{
	PG_BG_GuildWar_BLtoL_GuildNameRequest Send;
	Send.PlayerDBID = PlayerDBID;
	Send.WorldGuildID = WorldGuildID;
	Global::SendToOtherWorld_GSrvID( ToWorldID , ToGSrvID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SBL_GuildNameResult( int ToWorldID , int ToNetID , int PlayerDBID , int WorldGuildID , const char* GuildName )
{
	PG_BG_GuildWar_LtoBL_GuildNameResult Send;
	Send.PlayerDBID = PlayerDBID;
	Send.WorldGuildID = WorldGuildID;
	Send.GuildName = GuildName;
	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
//要求跨伺服器公會名稱結果
void NetSrv_BG_GuildWar::SC_GuildNameResult( int SendtoID , int WorldGuildID , const char* GuildName )
{
	PG_BG_GuildWar_BLtoC_GuildNameResult Send;
	Send.WorldGuildID = WorldGuildID;
	Send.GuildName = GuildName;
	SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SBL_OpenMenuRequest( int ToWorldID , int ToGSrvID , int PlayerDBID )
{
	PG_BG_GuildWar_LtoBL_OpenMenuRequest Send;
	Send.PlayerDBID = PlayerDBID;
	Global::SendToOtherWorld_GSrvID( ToWorldID , ToGSrvID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SL_OpenMenuResult( int ToWorldID , int ToNetID , int PlayerDBID , GuildHouseWarStateENUM State , int NextTime , vector<GuildHouseWarInfoStruct>& List )
{
	PG_BG_GuildWar_BLtoL_OpenMenuResult Send;
	Send.ZoneGroupID = RoleDataEx::G_ZoneID / 1000;
	Send.PlayerDBID = PlayerDBID;
	Send.State = State;
	Send.Count = (int)List.size();
	Send.NextTime = NextTime;
	for( int i = 0 ; i < List.size() ; i++ )
	{
		Send.List[i] = List[i];
	}
	//Send.List = List;
	//memcpy( Send.List , List , sizeof( GuildHouseWarInfoStruct) * Size );
	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , Send.PGSize() , &Send );
}
//////////////////////////////////////////////////////////////////////////
//開啟註冊公會戰介面
void NetSrv_BG_GuildWar::SC_OpenMenuResult( int SendtoID , int PlayerDBID , int ZoneGroupID , GuildHouseWarStateENUM State , int NextTime , int Count , GuildHouseWarInfoStruct List[1000] )
{
	PG_BG_GuildWar_LtoC_OpenMenuResult Send;
	Send.ZoneGroupID = ZoneGroupID;
	Send.PlayerDBID = PlayerDBID;
	Send.State = State;
	Send.Count = Count;
	Send.NextTime = NextTime;
	//Send.List[1000] = List[1000];
	memcpy( Send.List , List , sizeof( GuildHouseWarInfoStruct) * Count );
	SendToCli_ByGUID( SendtoID , Send.PGSize() , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SBL_RegisterRequest( int ToWorldID , int ToGSrvID , int GuildID , int GuildLv , const char* GuildName , int Score , GuildWarRegisterTypeENUM Type , int GuildCount , int PlayerDBID , bool IsAcceptAssignment , int TargetGuild )
{
	PG_BG_GuildWar_LtoBL_RegisterRequest Send;
	Send.Info.GuildID = GuildID;
	Send.Info.Type = Type;
	Send.Info.GuildName = GuildName;
	Send.Info.Score = Score;
	Send.Info.GuildCount = GuildCount;
	Send.Info.PlayerDBID = PlayerDBID;
	Send.Info.IsAcceptAssignment = IsAcceptAssignment;
	Send.Info.TargetGuild = TargetGuild;
	Send.Info.GuildLv = GuildLv;
	Global::SendToOtherWorld_GSrvID( ToWorldID , ToGSrvID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SL_RegisterResult( int ToWorldID , int ToNetID , int GuildID , GuildWarRegisterTypeENUM Type , GuildHouseWarInfoStruct& Info , GuildWarRegisterResultENUM Result , int PlayerDBID )
{
	PG_BG_GuildWar_BLtoL_RegisterResult Send;
	Send.GuildID = GuildID;
	Send.Type = Type;
	Send.Result = Result;
	Send.Info = Info;
	Send.PlayerDBID = PlayerDBID;
	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
//註冊公會戰( or 取消 )結果
void NetSrv_BG_GuildWar::SC_RegisterResult( int SendtoID , GuildWarRegisterTypeENUM Type , GuildHouseWarInfoStruct& Info , GuildWarRegisterResultENUM Result )
{
	PG_BG_GuildWar_LtoC_RegisterResult Send;
	Send.Type = Type;
	Send.Result = Result;
	Send.Info = Info;
	SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SL_Status( int ToWorldID , int ToGSrvID , GuildWarStatusTypeENUM Status , int NextTime )
{
	PG_BG_GuildWar_BLtoL_Status Send;
	Send.Status = Status;
	Send.NextTime = NextTime;
	Global::SendToOtherWorld_GSrvID( ToWorldID , ToGSrvID , sizeof( Send ) , &Send );
}

void NetSrv_BG_GuildWar::SL_AllWorld_Status( int ToGSrvID , GuildWarStatusTypeENUM Status , int NextTime )
{
//	if( RoleDataEx::G_ZoneID / 1000 != 0 )
//		return;
	PG_BG_GuildWar_BLtoL_Status Send;
	Send.Status = Status;
	Send.NextTime = NextTime;
	Global::SendToOtherWorld_GSrvID( -1 , ToGSrvID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
//公會戰狀況( 開始 , 準備 , 結束 )
void NetSrv_BG_GuildWar::SC_Status( int SendtoDBID , GuildWarStatusTypeENUM Status , int NextTime )
{
	PG_BG_GuildWar_LtoC_Status Send;
	Send.Status = Status;
	Send.NextTime = NextTime;
	SendToCli_ByDBID( SendtoDBID , sizeof( Send ) , &Send );
}
void NetSrv_BG_GuildWar::SC_AllPlayer_Status( GuildWarStatusTypeENUM Status , int NextTime )
{
	PG_BG_GuildWar_LtoC_Status Send;
	Send.Status = Status;
	Send.NextTime = NextTime;
	SendToAllPlayer( sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SL_HouseLoadRequest( int ToWorldID , int ToGSrvID , int GuildID )
{
	PG_BG_GuildWar_BLtoL_HouseLoadRequest Send;
	Send.GuildID = GuildID;
	Global::SendToOtherWorld_GSrvID( ToWorldID , ToGSrvID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SBL_HouseBaseInfo( int ToWorldID , int ToNetID , GuildHousesInfoStruct& HouseBaseInfo )
{
	PG_BG_GuildWar_LtoBL_HouseBaseInfo Send;
	Send.HouseBaseInfo = HouseBaseInfo;
	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SBL_HouseBuildingInfo( int ToWorldID , int ToNetID , GuildHouseBuildingInfoStruct Building )
{
	PG_BG_GuildWar_LtoBL_HouseBuildingInfo Send;
	Send.Building = Building;
	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SBL_HouseFurnitureInfo( int ToWorldID , int ToNetID , GuildHouseFurnitureItemStruct Item )
{
	PG_BG_GuildWar_LtoBL_HouseFurnitureInfo Send;
	Send.Item = Item;
	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SBL_HouseLoadOK( int ToWorldID , int ToNetID , int GuildID )
{
	PG_BG_GuildWar_LtoBL_HouseLoadOK Send;
	Send.GuildID = GuildID;
	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SBL_EnterRequest( int ToWorldID , int ToGSrvID , int PlayerDBID , int GuildID )
{
	PG_BG_GuildWar_LtoBL_EnterRequest Send;
	Send.PlayerDBID = PlayerDBID;
	Send.GuildID = GuildID;
	Global::SendToOtherWorld_GSrvID( ToWorldID , ToGSrvID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SL_EnterResult( int ToWorldID , int ToNetID , int PlayerDBID , int GuildID , bool Result )
{
	PG_BG_GuildWar_BLtoL_EnterResult Send;
	Send.ZoneGroupID = RoleDataEx::G_ZoneID / 1000;
	Send.PlayerDBID = PlayerDBID;
	Send.GuildID = GuildID;
	Send.Result = Result;
	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
//要求進入結果
void NetSrv_BG_GuildWar::SC_EnterResult( int SendtoDBID , bool Result )
{
	PG_BG_GuildWar_LtoC_EnterResult Send;
	Send.Result = Result;
	SendToCli_ByDBID( SendtoDBID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
//戰場分數
void NetSrv_BG_GuildWar::SC_GuildScore( int SendtoID , GuildHouseWarFightStruct& Info )
{
	PG_BG_GuildWar_BLtoC_GuildScore Send;
	Send.Info = Info;
	SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SC_AllRoom_GuildScore( int RoomID , GuildHouseWarFightStruct& Info )
{
	PG_BG_GuildWar_BLtoC_GuildScore	Send;
	Send.Info = Info;
	Global::SendToCli_ByRoomID( RoomID , sizeof(Send) , &Send );
}
//////////////////////////////////////////////////////////////////////////
//公會戰開始
void NetSrv_BG_GuildWar::SC_BeginWar( int SendtoID )
{
	PG_BG_GuildWar_BLtoC_BeginWar Send;
	SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SC_AllRoom_BeginWar( int RoomID  )
{
	PG_BG_GuildWar_BLtoC_BeginWar	Send;
	Global::SendToCli_ByRoomID( RoomID , sizeof(Send) , &Send );
}
//////////////////////////////////////////////////////////////////////////
//公會戰結束
void NetSrv_BG_GuildWar::SC_EndWar( int SendtoID )
{
	PG_BG_GuildWar_BLtoC_EndWar Send;
	SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SC_AllRoom_EndWar( int RoomID )
{
	PG_BG_GuildWar_BLtoC_EndWar Send;
	Global::SendToCli_ByRoomID( RoomID , sizeof(Send) , &Send );
	/*
	BaseItemObject* PlayerObj;
	for( PlayerObj = BaseItemObject::GetByOrder_Player(true) ; PlayerObj != NULL ; PlayerObj = BaseItemObject::GetByOrder_Player() )
	{
		if( PlayerObj->Role()->RoomID() != RoomID )
			continue;
		Global::SendToCli_ByGUID( PlayerObj->Role()->GUID() , sizeof(Send) , &Send );
	}
	*/
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SC_AllPlayer_EndWar( )
{
	PG_BG_GuildWar_BLtoC_EndWar Send;
	SendToAllPlayer( sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_BG_GuildWar::SBL_DebugTime				( int Time_Day , int Time_Hour )
{
	PG_BG_GuildWar_LtoBL_DebugTime Send;
	Send.Time_Hour = Time_Hour % 24;
	Send.Time_Day  = Time_Day % 7;
	for( int i = 0 ; i < 10 ; i++ )
		Global::SendToOtherWorld_GSrvID( _Def_BattleGround_WorldID_ , _Def_BattleGround_GuildWar_ZoneID_ + i * 1000 , sizeof( Send ) , &Send );
}
void NetSrv_BG_GuildWar::SC_PlayerInfoResult( int SendID , int TotalCount , int ID , GuildWarPlayerInfoStruct& Info )
{
	PG_BG_GuildWar_BLtoC_PlayerInfoResult Send;
	Send.ID = ID;
	Send.TotalCount = TotalCount;
	Send.Info = Info;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_BG_GuildWar::SC_PlayerFinalInfo( int RoomID , int PlayerDBID , int PrizeType , int Score , int OrderID 
											, Voc_ENUM Voc , Voc_ENUM Voc_Sub , int Level , int Level_Sub )
{
	PG_BG_GuildWar_BLtoC_PlayerFinalInfo Send;
	Send.PlayerDBID = PlayerDBID;
	Send.PrizeType = PrizeType;
	Send.Score = Score;
	Send.OrderID = OrderID;

	Send.Voc = Voc;								//職業
	Send.Voc_Sub = Voc_Sub;							//副職業
	Send.Level = Level;
	Send.Level_Sub = Level_Sub;	

	Global::SendToCli_ByRoomID( RoomID , sizeof(Send) , &Send );
}

void NetSrv_BG_GuildWar::SL_EndWarResult( int ToWorldID , int ToGSrvID , int GuildID , GameResultENUM GameResult , int GuildScore , GuildHouseWarInfoStruct* TargetGuild )
{
	PG_BG_GuildWar_BLtoL_EndWarResult Send;
	Send.GuildID = GuildID;
	Send.GameResult = GameResult;
	Send.GuildScore = GuildScore;
	Send.TargetGuild.Guild = TargetGuild->GuildID;
	Send.TargetGuild.GuildName = TargetGuild->GuildName;
	Send.TargetGuild.Score = TargetGuild->Score;
	Send.TargetGuild.IsAssignment = TargetGuild->IsAcceptAssignment;

	Global::SendToOtherWorld_GSrvID( ToWorldID , ToGSrvID , sizeof( Send ) , &Send );
}

void NetSrv_BG_GuildWar::SD_LoadInfo( int ZoneGroupID )
{
	PG_BG_GuildWar_BLtoD_LoadInfo Send;
	Send.ZoneGroupID = ZoneGroupID;
	Global::SendToDBCenter( sizeof(Send) , &Send );
}

void NetSrv_BG_GuildWar::SC_AllRoom_PlayerListInfo_Zip( int RoomID , vector<GuildWarPlayerInfoStruct*>& PlayerList )
{
	PlayerListInfoBase Temp;
	Temp.Count = (int)PlayerList.size();
	for( int i = 0 ; i < Temp.Count ; i++ )
	{
		Temp.Info[i] = *PlayerList[i];
	}

	PG_BG_GuildWar_BLtoC_PlayerListInfo_Zip Send;

	MyLzoClass myZip;
	Send.DataSize = myZip.Encode( (char*)(&Temp) , sizeof( Temp ) , (char*)&Send.Data );

	Global::SendToCli_ByRoomID( RoomID , Send.PGSize() , &Send );	
}

void NetSrv_BG_GuildWar::SBL_GuildWarRankInfoRequest	( int PlayerDBID , int WorldGuildID , GuildWarRankInfoTypeENUM Type )
{
	PG_BG_GuildWar_LtoBL_GuildWarRankInfoRequest Send;
	Send.PlayerDBID = PlayerDBID;
	Send.WorldGuildID = WorldGuildID;
	Send.Type = Type;

	Global::SendToOtherWorld_GSrvID( _Def_BattleGround_WorldID_ , _Def_BattleGround_GuildWar_ZoneID_ , sizeof( Send ) , &Send );
}
void NetSrv_BG_GuildWar::SL_GuildWarHisotry( int ToWorldID , int ToNetID , int	PlayerDBID , GuildHouseWarHistoryStruct	Hisotry[50] )
{
	PG_BG_GuildWar_BLtoL_GuildWarHisotry Send;
	Send.PlayerDBID = PlayerDBID;
	memcpy( Send.Hisotry , Hisotry , sizeof(Send.Hisotry) );

	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , sizeof( Send ) , &Send );
}
void NetSrv_BG_GuildWar::SC_GuildWarHisotry	( int PlayerDBID , GuildHouseWarHistoryStruct Hisotry[50] )
{
	PG_BG_GuildWar_LtoC_GuildWarHisotry Send;
	memcpy( Send.Hisotry , Hisotry , sizeof(Send.Hisotry) );
	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}
void NetSrv_BG_GuildWar::SL_GuildWarRankInfo			( int ToWorldID , int ToNetID , int PlayerDBID , GuildWarRankInfoStruct& Info )
{
	PG_BG_GuildWar_BLtoL_GuildWarRankInfo Send;
	Send.PlayerDBID = PlayerDBID;
	Send.Info = Info;
	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , sizeof( Send ) , &Send );
}
void NetSrv_BG_GuildWar::SC_GuildWarRankInfo			( int PlayerDBID , GuildWarRankInfoStruct& Info )
{
	PG_BG_GuildWar_LtoC_GuildWarRankInfo Send;
	Send.Info = Info;
	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}