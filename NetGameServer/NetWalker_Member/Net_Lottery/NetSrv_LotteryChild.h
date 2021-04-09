#pragma once

#include "NetSrv_Lottery.h"

class NetSrv_LotteryChild : public NetSrv_Lottery
{
public:
    static bool Init();
    static bool Release();

	virtual void RC_ExchangePrize		( BaseItemObject* Obj , ItemFieldStruct& Item , int Pos );
	virtual void RC_CloseExchangePrize	( BaseItemObject* Obj );
	virtual void RC_BuyLottery			( BaseItemObject* Obj , int Money , int Count , int Number[20] );
	virtual void RC_CloseBuyLottery		( BaseItemObject* Obj );
	virtual void RC_LotteryInfoRequest	( BaseItemObject* Obj );
	virtual void RC_LotteryHistoryInfoRequest	( BaseItemObject* Obj );

	virtual void RD_ExchangePrizeResult( ItemFieldStruct& Item , int Pos , int PlayerDBID , int PrizeCount[3] , int TotalMoney , bool Result );
	virtual void RD_BuyLotteryResult( int Version , int Money , int PlayerDBID , int Count, int Number[20] , bool Result );
};

