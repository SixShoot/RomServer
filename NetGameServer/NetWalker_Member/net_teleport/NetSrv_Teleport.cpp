#include "../NetWakerGSrvInc.h"
#include "NetSrv_Teleport.h"
//-------------------------------------------------------------------
NetSrv_Teleport*    NetSrv_Teleport::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_Teleport::_Init()
{
	Srv_NetCliReg( PG_Teleport_CtoL_AddTeleportRequest		);
	Srv_NetCliReg( PG_Teleport_CtoL_DelTeleportRequest		);
	Srv_NetCliReg( PG_Teleport_CtoL_SwapTeleportRequest		);
	Srv_NetCliReg( PG_Teleport_CtoL_ModifyTeleportRequest	);
	Srv_NetCliReg( PG_Teleport_CtoL_UseTeleportRequest		);

	_Net->RegOnSrvPacketFunction( EM_PG_Teleport_LtoL_CreateTeleport , _PG_Teleport_LtoL_CreateTeleport		);
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Teleport::_Release()
{
    return true;
}
void NetSrv_Teleport::_PG_Teleport_LtoL_CreateTeleport			( int Sockid , int Size , void* Data )
{
	PG_Teleport_LtoL_CreateTeleport* pg =(PG_Teleport_LtoL_CreateTeleport*)Data;

	This->RL_CreateTeleport( pg->FromZone , pg->FromX , pg->FromY , pg->FromZ , pg->ToZone , pg->ToX , pg->ToY , pg->ToZ , pg->Name.Begin() );
}

void NetSrv_Teleport::_PG_Teleport_CtoL_AddTeleportRequest		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Teleport_CtoL_AddTeleportRequest* pg =(PG_Teleport_CtoL_AddTeleportRequest*)data;

	This->R_AddTeleportRequest( Obj , pg->ItemPos , pg->TeleportPos , pg->Info );
}
void NetSrv_Teleport::_PG_Teleport_CtoL_DelTeleportRequest		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Teleport_CtoL_DelTeleportRequest* pg =(PG_Teleport_CtoL_DelTeleportRequest*)data;

	This->R_DelTeleportRequest( Obj , pg->TeleportPos );
}
void NetSrv_Teleport::_PG_Teleport_CtoL_SwapTeleportRequest		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Teleport_CtoL_SwapTeleportRequest* pg =(PG_Teleport_CtoL_SwapTeleportRequest*)data;

	This->R_SwapTeleportRequest( Obj , pg->TeleportPos );
}
void NetSrv_Teleport::_PG_Teleport_CtoL_ModifyTeleportRequest		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Teleport_CtoL_ModifyTeleportRequest* pg =(PG_Teleport_CtoL_ModifyTeleportRequest*)data;

	This->R_ModifyTeleportRequest( Obj , pg->TeleportPos , pg->Info );
}
void NetSrv_Teleport::_PG_Teleport_CtoL_UseTeleportRequest		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Teleport_CtoL_UseTeleportRequest* pg =(PG_Teleport_CtoL_UseTeleportRequest*)data;

	This->R_UseTeleportRequest( Obj , pg->Type , pg->ItemPos , pg->TeleportPos );
}

//////////////////////////////////////////////////////////////////////////
void NetSrv_Teleport::SC_AddTeleportResult		( int SendID , int TeleportPos , LocationSaveStruct& Info , bool Result )
{
	PG_Teleport_LtoC_AddTeleportResult Send;
	Send.TeleportPos = TeleportPos;		//儲存位置
	Send.Info = Info;	
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Teleport::SC_DelTeleportResult		( int SendID , int TeleportPos , bool Result )
{
	PG_Teleport_LtoC_DelTeleportResult Send;
	Send.TeleportPos = TeleportPos;		//儲存位置
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Teleport::SC_SwapTeleportResult		( int SendID , int TeleportPos[2] , bool Result )
{
	PG_Teleport_LtoC_SwapTeleportResult Send;
	Send.TeleportPos[0] = TeleportPos[0];		//儲存位置
	Send.TeleportPos[1] = TeleportPos[1];		//儲存位置
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Teleport::SC_ModifyTeleportResult	( int SendID , int TeleportPos , LocationSaveStruct& Info , bool Result )
{
	PG_Teleport_LtoC_ModifyTeleportResult Send;
	Send.TeleportPos = TeleportPos;		//儲存位置
	Send.Info = Info;	
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Teleport::SC_UseTeleportResult		( int SendID , UseTeleportTypeENUM Type , int ItemPos , int TeleportPos , bool Result )
{
	PG_Teleport_LtoC_UseTeleportResult Send;
	Send.ItemPos = ItemPos;
	Send.TeleportPos = TeleportPos;		//儲存位置
	Send.Type = Type;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Teleport::SL_CreateTeleport			( int FromZone , float FromX , float FromY , float FromZ , int ToZone , float ToX , float ToY , float ToZ , const char* Name )
{
	PG_Teleport_LtoL_CreateTeleport Send;
	
	Send.FromZone = FromZone;
	Send.FromX = FromX;
	Send.FromY = FromY;
	Send.FromZ = FromZ;

	Send.ToZone = ToZone;
	Send.ToX = ToX;
	Send.ToY = ToY;
	Send.ToZ = ToZ;

	Send.Name = Name;

	Global::SendToLocal( ToZone , sizeof(Send) , &Send );

}
