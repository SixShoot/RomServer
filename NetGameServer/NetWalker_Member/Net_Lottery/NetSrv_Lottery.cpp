#include "../NetWakerGSrvInc.h"
#include "NetSrv_Lottery.h"
//-------------------------------------------------------------------
NetSrv_Lottery*      NetSrv_Lottery::This         = NULL;
//-------------------------------------------------------------------
bool NetSrv_Lottery::_Init()
{	

	Srv_NetCliReg( PG_Lottery_CtoL_ExchangePrize		);
	Srv_NetCliReg( PG_Lottery_CtoL_CloseExchangePrize	);
	Srv_NetCliReg( PG_Lottery_CtoL_BuyLottery			);
	Srv_NetCliReg( PG_Lottery_CtoL_CloseBuyLottery		);
	Srv_NetCliReg( PG_Lottery_CtoL_LotteryInfoRequest	);
	Srv_NetCliReg( PG_Lottery_CtoL_LotteryHistoryInfoRequest	);

	_Net->RegOnSrvPacketFunction( EM_PG_Lottery_DtoL_ExchangePrizeResult	, _PG_Lottery_DtoL_ExchangePrizeResult	 );
	_Net->RegOnSrvPacketFunction( EM_PG_Lottery_DtoL_BuyLotteryResult		, _PG_Lottery_DtoL_BuyLotteryResult		 );
	return true;

}
//-------------------------------------------------------------------
bool NetSrv_Lottery::_Release()
{
    return true;
}


void NetSrv_Lottery::_PG_Lottery_CtoL_ExchangePrize			( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Lottery_CtoL_ExchangePrize* pg =(PG_Lottery_CtoL_ExchangePrize*)data;

	This->RC_ExchangePrize( Obj , pg->Item , pg->Pos );
}
void NetSrv_Lottery::_PG_Lottery_CtoL_CloseExchangePrize	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Lottery_CtoL_CloseExchangePrize* pg =(PG_Lottery_CtoL_CloseExchangePrize*)data;

	This->RC_CloseExchangePrize( Obj );
}
void NetSrv_Lottery::_PG_Lottery_CtoL_BuyLottery			( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Lottery_CtoL_BuyLottery* pg =(PG_Lottery_CtoL_BuyLottery*)data;
	
	This->RC_BuyLottery( Obj , pg->Money , pg->Count , pg->Number );
}
void NetSrv_Lottery::_PG_Lottery_CtoL_CloseBuyLottery		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Lottery_CtoL_CloseBuyLottery* pg =(PG_Lottery_CtoL_CloseBuyLottery*)data;

	This->RC_CloseBuyLottery( Obj );
}
void NetSrv_Lottery::_PG_Lottery_CtoL_LotteryInfoRequest	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Lottery_CtoL_LotteryInfoRequest* pg =(PG_Lottery_CtoL_LotteryInfoRequest*)data;

	This->RC_LotteryInfoRequest( Obj );
}
void NetSrv_Lottery::_PG_Lottery_CtoL_LotteryHistoryInfoRequest	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_Lottery_CtoL_LotteryHistoryInfoRequest* pg =(PG_Lottery_CtoL_LotteryHistoryInfoRequest*)data;

	This->RC_LotteryHistoryInfoRequest( Obj );
}


void NetSrv_Lottery::_PG_Lottery_DtoL_ExchangePrizeResult	( int sockid , int size , void* data )	
{
	PG_Lottery_DtoL_ExchangePrizeResult* pg =(PG_Lottery_DtoL_ExchangePrizeResult*)data;
	This->RD_ExchangePrizeResult( pg->Item , pg->Pos , pg->PlayerDBID , pg->PrizeCount , pg->TotalMoney , pg->Result);
}
void NetSrv_Lottery::_PG_Lottery_DtoL_BuyLotteryResult		( int sockid , int size , void* data )
{
	PG_Lottery_DtoL_BuyLotteryResult* pg =(PG_Lottery_DtoL_BuyLotteryResult*)data;
	This->RD_BuyLotteryResult( pg->Version , pg->Money , pg->PlayerDBID , pg->Count, pg->Number , pg->Result );
}
//////////////////////////////////////////////////////////////////////////

void NetSrv_Lottery::SC_OpenExchangePrize	( int SendID , int TargetNPCID )
{
	PG_Lottery_LtoC_OpenExchangePrize Send;
	Send.TargetNPCID = TargetNPCID;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Lottery::SC_ExchangePrizeResultOK	( int SendID , int Money , int Prize1Count , int Prize2Count , int Prize3Count )
{
	PG_Lottery_LtoC_ExchangePrizeResult Send;

	Send.PrizeCount[0] = Prize1Count;			//中各獎項的數量
	Send.PrizeCount[1] = Prize2Count;			//中各獎項的數量
	Send.PrizeCount[2] = Prize3Count;			//中各獎項的數量
	Send.Money	= Money;						//獎金
	Send.Result = true;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void	NetSrv_Lottery::SC_ExchangePrizeResultFailed	( int SendID )
{
	PG_Lottery_LtoC_ExchangePrizeResult Send;
	Send.Result = false;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );	
}

void NetSrv_Lottery::SC_CloseExchangePrize	( int SendID )
{
	PG_Lottery_LtoC_CloseExchangePrize Send;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Lottery::SC_OpenBuyLottery		( int SendID , int TargetNPCID )
{
	PG_Lottery_LtoC_OpenBuyLottery Send;
	Send.TargetNPCID = TargetNPCID;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Lottery::SC_BuyLotteryResult	( int SendID , bool Result )
{
	PG_Lottery_LtoC_BuyLotteryResult Send;
	Send.Result = Result;

	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Lottery::SC_CloseBuyLottery		( int SendID )
{
	PG_Lottery_LtoC_CloseBuyLottery Send;
	Global::SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}


void NetSrv_Lottery::SD_ExchangePrize		( ItemFieldStruct Item , int Pos , int PlayerDBID )
{
	PG_Lottery_LtoD_ExchangePrize Send;
	Send.Item = Item;
	Send.Pos  = Pos;
	Send.PlayerDBID = PlayerDBID;

	Global::SendToDBCenter( sizeof(Send) , &Send );
}
void NetSrv_Lottery::SD_BuyLottery			( int  Money , int PlayerDBID , int Count , int Number[20] )
{
	PG_Lottery_LtoD_BuyLottery Send;
	Send.Money = Money;	
	Send.PlayerDBID = PlayerDBID; 
	Send.Count = Count;
	memcpy( Send.Number , Number , sizeof( Send.Number ) );

	Global::SendToDBCenter( sizeof(Send) , &Send );
}
void NetSrv_Lottery::SD_LotteryInfoRequest	( int PlayerDBID )
{
	PG_Lottery_LtoD_LotteryInfoRequest Send;
	Send.PlayerDBID = PlayerDBID;

	Global::SendToDBCenter( sizeof(Send) , &Send );
}
void NetSrv_Lottery::SD_LotteryHistoryInfoRequest	( int PlayerDBID )
{
	PG_Lottery_LtoD_LotteryHistoryInfoRequest Send;
	Send.PlayerDBID = PlayerDBID;

	Global::SendToDBCenter( sizeof(Send) , &Send );
}
