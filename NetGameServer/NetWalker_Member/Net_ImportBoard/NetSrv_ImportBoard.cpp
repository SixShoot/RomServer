#include "../NetWakerGSrvInc.h"
#include "NetSrv_ImportBoard.h"
//-------------------------------------------------------------------
NetSrv_ImportBoard*    NetSrv_ImportBoard::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_ImportBoard::_Init()
{
	Srv_NetCliReg( PG_ImportBoard_CtoL_ImportBoardRequest );
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_ImportBoard::_Release()
{
    return true;
}

void NetSrv_ImportBoard::_PG_ImportBoard_CtoL_ImportBoardRequest	( int sockid , int size , void* data )
{
	PG_ImportBoard_CtoL_ImportBoardRequest* pg =(PG_ImportBoard_CtoL_ImportBoardRequest*)data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->RC_ImportBoardRequest( Obj , pg->UpdateTime );
}

void NetSrv_ImportBoard::SD_ImportBoardRequest( int PlayerDBID , float UpdateTime )
{
	PG_ImportBoard_LtoD_ImportBoardRequest Send;
	Send.PlayerDBID = PlayerDBID;
	Send.UpdateTime = UpdateTime;
	Global::SendToDBCenter( sizeof(Send) , &Send );
}

void NetSrv_ImportBoard::SD_NewVersionRequest( int SockID , int ProxyID )
{
	PG_ImportBoard_LtoD_NewVersionRequest Send;
	Send.SockID = SockID;
	Send.ProxyID = ProxyID;
	Global::SendToDBCenter( sizeof(Send) , &Send );
}