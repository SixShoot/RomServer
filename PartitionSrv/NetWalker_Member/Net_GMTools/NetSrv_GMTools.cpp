//#include "../NetWakerGSrvInc.h"
#include "NetSrv_GMTools.h"
#include "../net_serverlist/Net_ServerList_Child.h"

//-------------------------------------------------------------------
NetSrv_GMTools* NetSrv_GMTools::This = NULL;
//-------------------------------------------------------------------
bool NetSrv_GMTools::_Init()
{
	_Net->RegOnSrvPacketFunction( EM_PG_GM_GMtoX_ServerStateRequest , _PG_GM_GMtoX_ServerStateRequest );
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_GMTools::_Release()
{
    return true;
}

void NetSrv_GMTools::_PG_GM_GMtoX_ServerStateRequest	( int sockid , int size , void* data )
{
	PG_GM_GMtoX_ServerStateRequest* pg =(PG_GM_GMtoX_ServerStateRequest*)data;
	This->R_ServerStateRequest( sockid , pg->RequestType );
}

void NetSrv_GMTools::S_ServerStateResult( int NetID , ServerStateRequestTypeENUM	RequestType , const char* Name , const char* Type , const char* State 
										 , const char* Content1 , const char* Content2
										 , const char* Content3 , const char* Content4
										 , const char* Content5 , const char* Content6
										 , const char* Content7 , const char* Content8 )
{
	PG_GM_XtoGM_ServerStateResult	Send;
	Send.RequestType = RequestType;
	Send.Name = Name;
	Send.Type = Type;
	Send.State = State;
	Send.Content[0] = Content1;
	Send.Content[1] = Content2;
	Send.Content[2] = Content3;
	Send.Content[3] = Content4;
	Send.Content[4] = Content5;
	Send.Content[5] = Content6;
	Send.Content[6] = Content7;
	Send.Content[7] = Content8;
	SendToSrvBySockID( NetID , sizeof( Send ) , &Send );
}