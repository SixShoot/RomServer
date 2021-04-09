
#pragma once
#include "PG/PG_Lottery.h"
#include "../../MainProc/Global.h"

class CNetDC_Lottery : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetDC_Lottery*	This;

	static bool				_Init();
	static bool				_Release();

	static void _PG_Lottery_LtoD_ExchangePrize				( int ServerID , int Size , void* Data );		
	static void _PG_Lottery_LtoD_BuyLottery					( int ServerID , int Size , void* Data );		
	static void _PG_Lottery_LtoD_LotteryInfoRequest			( int ServerID , int Size , void* Data );		
	static void _PG_Lottery_LtoD_LotteryHistoryInfoRequest	( int ServerID , int Size , void* Data );		
public:

	static void SL_ExchangePrizeResultOK		( int SrvSockID , ItemFieldStruct& Item , int Pos , int PlayerDBID , int PrizeCount[3] , int TotalMoney );
	static void SL_ExchangePrizeResultFailed	( int SrvSockID , ItemFieldStruct& Item , int Pos , int PlayerDBID );
	static void SL_BuyLotteryResult				( int SrvSockID , int Version , int Money , int PlayerDBID , int Count , int Number[20] , bool Result );
	static void SC_LotteryInfo					( int PlayerDBID , int MaxPrizeMoney , int Version );
	static void SC_LotteryHistoryInfo			( int PlayerDBID ,  LotteryInfoStruct LastInfo[5] );
	static void SC_RunLottery					( LotteryInfoStruct& Info );

	virtual void RL_ExchangePrize		( int SrvSockID , int PlayerDBID , ItemFieldStruct& Item , int Pos ) = 0;
	virtual void RL_BuyLottery			( int SrvSockID , int PlayerDBID , int Money , vector<int>& Number ) = 0;
	virtual void RL_LotteryInfoRequest	( int SrvSockID , int PlayerDBID ) = 0;
	virtual void RL_LotteryHistoryInfoRequest	( int SrvSockID , int PlayerDBID ) = 0;

};
