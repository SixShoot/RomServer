#include "../NetWakerGSrvInc.h"
#include "NetSrv_Bot.h"
//-------------------------------------------------------------------
NetSrv_Bot*    NetSrv_Bot::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_Bot::_Init()
{
    Srv_NetCliReg( PG_Bot_CtoL_RequestMovePath       );
    return false;
}
//-------------------------------------------------------------------
bool NetSrv_Bot::_Release()
{
    return false;
}

void NetSrv_Bot::_PG_Bot_CtoL_RequestMovePath        ( int sockid , int size , void* data )
{
	PG_Bot_CtoL_RequestMovePath* pg =(PG_Bot_CtoL_RequestMovePath*)data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->R_RequestMovePath( Obj , pg->Range , pg->X , pg->Y , pg->Z );
}

void NetSrv_Bot::S_MovePoint( int SendID , int Count , int Index , float X , float Y , float Z )
{
	PG_Bot_CtoL_MovePoint Send;
	Send.Count = Count;
	Send.Index = Index;
	Send.X = X;
	Send.Y = Y;
	Send.Z = Z;

	SendToCli_ByGUID( SendID  , sizeof( Send ) , &Send );
}