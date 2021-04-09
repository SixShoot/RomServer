#include "NetDC_Lottery.h"
//-------------------------------------------------------------------
CNetDC_Lottery*	CNetDC_Lottery::This = NULL;

//-------------------------------------------------------------------
bool CNetDC_Lottery::_Release()
{
	return true;
}
//-------------------------------------------------------------------
bool CNetDC_Lottery::_Init()
{
	_Net->RegOnSrvPacketFunction( EM_PG_Lottery_LtoD_ExchangePrize		, _PG_Lottery_LtoD_ExchangePrize			);
	_Net->RegOnSrvPacketFunction( EM_PG_Lottery_LtoD_BuyLottery			, _PG_Lottery_LtoD_BuyLottery				);
	_Net->RegOnSrvPacketFunction( EM_PG_Lottery_LtoD_LotteryInfoRequest	, _PG_Lottery_LtoD_LotteryInfoRequest		);
	_Net->RegOnSrvPacketFunction( EM_PG_Lottery_LtoD_LotteryHistoryInfoRequest	, _PG_Lottery_LtoD_LotteryHistoryInfoRequest		);

	return true;
}

void CNetDC_Lottery::_PG_Lottery_LtoD_ExchangePrize			( int ServerID , int Size , void* Data )
{
	PG_Lottery_LtoD_ExchangePrize* PG = (PG_Lottery_LtoD_ExchangePrize*)Data;
	This->RL_ExchangePrize( ServerID , PG->PlayerDBID , PG->Item , PG->Pos );
}
void CNetDC_Lottery::_PG_Lottery_LtoD_BuyLottery			( int ServerID , int Size , void* Data )
{
	PG_Lottery_LtoD_BuyLottery* PG = (PG_Lottery_LtoD_BuyLottery*)Data;

	vector< int > Number;
	for( int i = 0 ; i < 20 && i < PG->Count ; i++ )
		Number.push_back( PG->Number[i] );
	
	This->RL_BuyLottery( ServerID , PG->PlayerDBID , PG->Money , Number );
}
void CNetDC_Lottery::_PG_Lottery_LtoD_LotteryInfoRequest	( int ServerID , int Size , void* Data )
{
	PG_Lottery_LtoD_LotteryInfoRequest* PG = (PG_Lottery_LtoD_LotteryInfoRequest*)Data;
	This->RL_LotteryInfoRequest( ServerID , PG->PlayerDBID );
}
void CNetDC_Lottery::_PG_Lottery_LtoD_LotteryHistoryInfoRequest	( int ServerID , int Size , void* Data )
{
	PG_Lottery_LtoD_LotteryHistoryInfoRequest* PG = (PG_Lottery_LtoD_LotteryHistoryInfoRequest*)Data;
	This->RL_LotteryHistoryInfoRequest( ServerID , PG->PlayerDBID );
}


void CNetDC_Lottery::SL_ExchangePrizeResultOK	( int SrvSockID , ItemFieldStruct& Item , int Pos , int PlayerDBID , int PrizeCount[3] , int TotalMoney )
{
	PG_Lottery_DtoL_ExchangePrizeResult Send;

	Send.Item = Item;
	Send.Pos = Pos;
	Send.PlayerDBID = PlayerDBID;
	Send.PrizeCount[0] = PrizeCount[0];
	Send.PrizeCount[1] = PrizeCount[1];
	Send.PrizeCount[2] = PrizeCount[2];
	Send.TotalMoney	   = TotalMoney;
	Send.Result		   = true;

	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );
}

void CNetDC_Lottery::SL_ExchangePrizeResultFailed	( int SrvSockID , ItemFieldStruct& Item , int Pos , int PlayerDBID  )
{
	PG_Lottery_DtoL_ExchangePrizeResult Send;

	Send.Item = Item;
	Send.Pos = Pos;
	Send.PlayerDBID  = PlayerDBID;
	Send.Result		 = false;

	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );
}

void CNetDC_Lottery::SL_BuyLotteryResult	( int  SrvSockID , int Version , int	Money , int PlayerDBID , int Count , int Number[20] , bool Result )
{
	PG_Lottery_DtoL_BuyLotteryResult Send;

	Send.Money = Money;
	Send.PlayerDBID = PlayerDBID;
	Send.Count = Count;
	Send.Version = Version;
	memcpy( Send.Number , Number , sizeof( Send.Number ) );
	Send.Result = Result;

	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );
}
void CNetDC_Lottery::SC_LotteryInfo			( int PlayerDBID , int MaxPrizeMoney , int Version ) 
{
	PG_Lottery_DtoC_LotteryInfo Send;
	
//	memcpy( Send.LastInfo , LastInfo , sizeof( Send.LastInfo ) );
	Send.MaxPrizeMoney = MaxPrizeMoney;
	Send.Version = Version;

	Global::SendToCli_ByDBID( PlayerDBID , sizeof(Send) , &Send );
}
void CNetDC_Lottery::SC_LotteryHistoryInfo			( int PlayerDBID , LotteryInfoStruct LastInfo[5] ) 
{
	PG_Lottery_DtoC_LotteryHistoryInfo Send;

	memcpy( Send.LastInfo , LastInfo , sizeof( Send.LastInfo ) );

	Global::SendToCli_ByDBID( PlayerDBID , sizeof(Send) , &Send );
}
void CNetDC_Lottery::SC_RunLottery			( LotteryInfoStruct& Info )
{
	PG_Lottery_DtoC_RunLottery Send;
	Send.Info = Info;

	Global::SendToAllPlayer( sizeof(Send) , &Send );
}