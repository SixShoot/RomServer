#include "../NetWakerGSrvInc.h"
#include "NetSrv_Trade.h"
//-------------------------------------------------------------------
NetSrv_Trade*    NetSrv_Trade::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_Trade::_Init()
{
    Srv_NetCliReg( PG_Trade_CtoL_RequestTrade 		);
    Srv_NetCliReg( PG_Trade_CtoL_AgreeTrade   		);
    Srv_NetCliReg( PG_Trade_CtoL_PutItem      		);
    Srv_NetCliReg( PG_Trade_CtoL_PutMoney     		);
    Srv_NetCliReg( PG_Trade_CtoL_ItemOK       		);
    Srv_NetCliReg( PG_Trade_CtoL_FinalOK      		);
    Srv_NetCliReg( PG_Trade_CtoL_StopTrade    		);
	Srv_NetCliReg( PG_Trade_CtoL_CancelTradeRequest );
    return false;
}
//-------------------------------------------------------------------
bool NetSrv_Trade::_Release()
{
    return false;
}
//---------------------------------------------------------------------------
//封包
//---------------------------------------------------------------------------
void NetSrv_Trade::_PG_Trade_CtoL_CancelTradeRequest	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Trade_CtoL_CancelTradeRequest* pg =(PG_Trade_CtoL_CancelTradeRequest*)data;

	This->R_CancelTradeRequest( Obj , pg->TargetID );
}

void NetSrv_Trade::_PG_Trade_CtoL_RequestTrade	( int sockid , int size , void* data )
{
    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;

    PG_Trade_CtoL_RequestTrade* pg =(PG_Trade_CtoL_RequestTrade*)data;

    This->R_RequestTrade( Obj , pg->TargetID );

}

void NetSrv_Trade::_PG_Trade_CtoL_AgreeTrade	( int sockid , int size , void* data )
{
    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;

    PG_Trade_CtoL_AgreeTrade* pg =(PG_Trade_CtoL_AgreeTrade*)data;

    This->R_AgreeTrade( Obj , pg->TargetID , pg->Result , pg->ClientResult );

}
void NetSrv_Trade::_PG_Trade_CtoL_PutItem		( int sockid , int size , void* data )
{
    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;

    PG_Trade_CtoL_PutItem* pg =(PG_Trade_CtoL_PutItem*)data;

    This->R_PutItem( Obj , pg->Pos , pg->ItemPos , &pg->Item );
}
void NetSrv_Trade::_PG_Trade_CtoL_PutMoney		( int sockid , int size , void* data )
{
    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;

    PG_Trade_CtoL_PutMoney* pg =(PG_Trade_CtoL_PutMoney*)data;

    This->R_PutMoney( Obj , pg->Money , pg->Money_Account );
}
void NetSrv_Trade::_PG_Trade_CtoL_ItemOK		( int sockid , int size , void* data )
{
    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;

    This->R_ItemOK( Obj );
}
void NetSrv_Trade::_PG_Trade_CtoL_FinalOK		( int sockid , int size , void* data )
{
    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;

	PG_Trade_CtoL_FinalOK* pg = (PG_Trade_CtoL_FinalOK*)data;

    This->R_FinalOK( Obj , pg->Password.Begin() );
}
void NetSrv_Trade::_PG_Trade_CtoL_StopTrade	( int sockid , int size , void* data )
{
    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;

    This->R_StopTrade( Obj );
}
//---------------------------------------------------------------------------
//送給Client端的資料
//---------------------------------------------------------------------------
void NetSrv_Trade::S_ClientRequestTrade		( int SendID , int TargetID )
{
    PG_Trade_LtoC_RequestTrade	Send;
    Send.TargetID = TargetID;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Trade::S_OpenTrade			( int SendID , int TargetID , const char* Name )
{
    PG_Trade_LtoC_OpenTrade	Send;
    Send.TargetID = TargetID;
    MyStrcpy( Send.Name , Name , sizeof(Send.Name) );

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Trade::S_TargetPutItem		( int SendID , int Pos , ItemFieldStruct* Item )
{
    PG_Trade_LtoC_TargetPutItem	Send;
    Send.Pos		= Pos;
    Send.Item		= *Item;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Trade::S_TargetPutMoney		( int SendID , int Money , int Money_Account )
{
    PG_Trade_LtoC_TargetPutMoney	Send;
    Send.Money	= Money;	
	Send.Money_Account = Money_Account;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Trade::S_OwnerPutMoney		( int SendID , int Money , int Money_Account )
{
    PG_Trade_LtoC_OwnerPutMoney	Send;
    Send.Money	= Money;	
	Send.Money_Account = Money_Account;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Trade::S_TargetItemOK		( int SendID )
{
    PG_Trade_LtoC_TargetItemOK	Send;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Trade::S_CancelItemOK		( int SendID )
{
    PG_Trade_LtoC_CancelItemOK	Send;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Trade::S_FinalOKEnable		( int SendID )
{
    PG_Trade_LtoC_FinalOKEnable	Send;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Trade::S_TargetFinalOK		( int SendID )
{
    PG_Trade_LtoC_TargetFinalOK	Send;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Trade::S_ClearTradeItem		( int SendID )
{
    PG_Trade_LtoC_ClearTradeItem	Send;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Trade::S_TradeItemOK		( int SendID )
{
    PG_Trade_LtoC_TradeItemResult	Send;
    Send.Result = true;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Trade::S_TradeItemFalse		( int SendID )
{
    PG_Trade_LtoC_TradeItemResult	Send;
    Send.Result = false;

    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Trade::S_PutItemFalse  		( int SendID )
{
    PG_Trade_LtoC_PutItemResult Send;
    Send.Result = false;
    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Trade::S_PutItemOK     		( int SendID )
{
    PG_Trade_LtoC_PutItemResult Send;
    Send.Result = true;
    Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );

}
void	NetSrv_Trade::S_RequestTradeError		( int SendID )
{
	PG_Trade_LtoC_RequestTradeError Send;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void	NetSrv_Trade::S_DisagreeTrade			( int SendID , int ClientResult )
{
	PG_Trade_LtoC_DisagreeTrade Send;
	Send.ClientResult = ClientResult;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void	NetSrv_Trade::S_StopTrade				( int SendID , int TargetID )
{
	PG_Trade_LtoC_StopTrade Send;
	Send.TargetID = TargetID;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void	NetSrv_Trade::S_TradeState			( int SendID , bool TradeOK , bool FinalOK , bool TargetTradeOK , bool TargetFinalOK )
{
	PG_Trade_LtoC_TradeState Send;
	Send.FinalOK = FinalOK;
	Send.TargetFinalOK = TargetFinalOK;
	Send.TradeOK = TradeOK;
	Send.TargetTradeOK = TargetTradeOK;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Trade::S_CancelTradeRequest	( int SendID , int TargetID )
{
	PG_Trade_LtoC_CancelTradeRequest Send;
	Send.TargetID = TargetID;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Trade::S_FinalOkFailed			( int SendID , FinalOKFailedENUM Result )
{
	PG_Trade_LtoC_FinalOKFailed Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}