#include "../NetWakerGSrvInc.h"
#include "NetSrv_Treasure.h"
//-------------------------------------------------------------------
NetSrv_Treasure*    NetSrv_Treasure::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_Treasure::_Init()
{
	Srv_NetCliReg( PG_Treasure_CtoL_GetItem		);
	Srv_NetCliReg( PG_Treasure_CtoL_Close		);
	Srv_NetCliReg( PG_Treasure_CtoL_OpenRequest	);
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Treasure::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_Treasure::_PG_Treasure_CtoL_OpenRequest	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Treasure_CtoL_OpenRequest* pg =(PG_Treasure_CtoL_OpenRequest*)data;

	This->RC_OpenRequest( Obj , pg->ItemGUID );
}
void NetSrv_Treasure::_PG_Treasure_CtoL_GetItem		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Treasure_CtoL_GetItem* pg =(PG_Treasure_CtoL_GetItem*)data;

	This->RC_GetItem( Obj , pg->ItemGUID , pg->BodyPos , pg->ItemSerial );
}
void NetSrv_Treasure::_PG_Treasure_CtoL_Close		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Treasure_CtoL_Close* pg =(PG_Treasure_CtoL_Close*)data;

	This->RC_Close( Obj , pg->ItemGUID );
}

void NetSrv_Treasure::SC_BaseInfo			( int SendID , int ItemGUID , bool IsEmpty )
{
	PG_Treasure_LtoC_BaseInfo	Send;
	Send.ItemGUID = ItemGUID;
	Send.IsEmpty = IsEmpty;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Treasure::SC_Open				( int SendID , int ItemGUID , int MaxCount , ItemFieldStruct Items[30] )
{
	PG_Treasure_LtoC_Open	Send;
	Send.ItemGUID = ItemGUID;
	Send.MaxCount = MaxCount;
	memcpy( Send.Items , Items , sizeof(Send.Items) );

	Global::SendToCli_ByGUID( SendID , Send.Size(), &Send );
}
void NetSrv_Treasure::SC_GetItemResult		( int SendID , int ItemSerial , int BodyPos , TreasureGetItemResultENUM Result )
{
	PG_Treasure_LtoC_GetItemResult	Send;
	Send.ItemSerial = ItemSerial;
	Send.BodyPos = BodyPos;
	Send.Result	= Result;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Treasure::SC_OpenFailed			( int SendID , int ItemGUID )
{
	PG_Treasure_LtoC_OpenFailed	Send;
	Send.ItemGUID = ItemGUID;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}