#include "../NetWakerGSrvInc.h"
#include "NetSrv_PK.h"

//-------------------------------------------------------------------
NetSrv_PK* NetSrv_PK::This = NULL;
//-------------------------------------------------------------------
bool NetSrv_PK::_Init()
{
	Srv_NetCliReg( PG_PK_CtoL_PKInviteRequest	);
	Srv_NetCliReg( PG_PK_CtoL_PKInviteResult	);
	Srv_NetCliReg( PG_PK_CtoL_PKCancel			);
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_PK::_Release()
{
    return true;
}

void NetSrv_PK::_PG_PK_CtoL_PKInviteRequest		( int Sockid , int Size , void* Data )	
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )return;

	PG_PK_CtoL_PKInviteRequest* PG = (PG_PK_CtoL_PKInviteRequest*)Data;
	This->R_PKInviteRequest( Player , PG->TargetID );
}
void NetSrv_PK::_PG_PK_CtoL_PKInviteResult		( int Sockid , int Size , void* Data )	
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_PK_CtoL_PKInviteResult* PG = (PG_PK_CtoL_PKInviteResult*)Data;
	This->R_PKInviteResult( Player , PG->TargetID , PG->Result );
}
void NetSrv_PK::_PG_PK_CtoL_PKCancel			( int Sockid , int Size , void* Data )		
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_PK_CtoL_PKCancel* PG = (PG_PK_CtoL_PKCancel*)Data;
	This->R_PKCancel( Player  );
}


void NetSrv_PK::SC_PKInviteNotify	( int SendID , int TargetID , int PKScore )	
{
	PG_PK_LtoC_PKInviteNotify Send;
	Send.TargetID = TargetID;
	Send.TargetScore = PKScore;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_PK::SC_PKInviteResult	( int SendID , int TargetID , bool Result )	
{
	PG_PK_LtoC_PKInviteResult Send;
	Send.TargetID = TargetID;
	Send.Result	  = Result;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_PK::SC_PKBegin			( int SendID , int Time )	
{
	PG_PK_LtoC_PKBegin Send;
	Send.Time = Time;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_PK::SC_PKPrepare		( int SendID , int Player1ID , int Player2ID , int Player1Score , int Player2Score 
									 , int X , int Y , int Z , int Range , int Time )
{
	PG_PK_LtoC_PKPrepare Send;
	Send.Player1ID = Player1ID;
	Send.Player2ID = Player2ID;
	Send.Player1Score = Player1Score;
	Send.Player2Score = Player2Score;
	Send.PosX = X;
	Send.PosY = Y;
	Send.PosZ = Z;
	Send.Range = Range;
	Send.Time = Time;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_PK::SC_PKEnd			( int SendID , int WinnerID , int LoserID , bool IsEven )
{
	PG_PK_LtoC_PKEnd Send;
	Send.WinnerID = WinnerID;
	Send.LoserID = LoserID;
	Send.IsEven = IsEven;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_PK::SC_PKCancel			( int SendID )
{
	PG_PK_LtoC_PKCancel Send;

	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
