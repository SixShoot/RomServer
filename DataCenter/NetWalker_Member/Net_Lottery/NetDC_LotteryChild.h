#pragma once

#include "NetDC_Lottery.h"



//--------------------------------------------------------------------------------------------
class CNetDC_LotteryChild : public CNetDC_Lottery
{
	static vector< LotteryInfoStruct > 		_LotteryHistory;
	static LotteryInfoStruct				_Last5LotteryInfo[5];
	static int							  	_LotteryVersion;	//最新一期 尚未開獎 
	static int							  	_LotteryNumber[5];	//最新一期
	static int								_LotteryPrizeCount[3];
	static int							  	_LotteryTotalMoney;	//累積金額
	static int							  	_RunLotteryTime;		//跑樂透的時間
	static bool								_IsInitReady;

	static int _OnTimeProc_( SchedularInfo* Obj , void* InputClass );

	//初始設定
	static bool _SQLCmdInitProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdInitProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//開獎處理
	static bool _SQLCmdLotteryRun( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLotteryRunResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );


	static bool _SQLCmdLotteryExchangePrize( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLotteryExchangePrizeResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdLotteryBuy( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLotteryBuyResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	
	//定時取總獎金
	static bool _SQLCmdLotteryGetTotoalMoney( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLotteryGetTotoalMoneyResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	


	//計算中獎數量
	static int	_CalLotteryHitBall( LotteryInfoStruct& LotteryPrizeInfo , ItemFieldStruct& Item );
public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

	virtual void RL_ExchangePrize				( int SrvSockID , int PlayerDBID , ItemFieldStruct& Item , int Pos );
	virtual void RL_BuyLottery					( int SrvSockID , int PlayerDBID , int Money , vector<int>& Number );
	virtual void RL_LotteryInfoRequest			( int SrvSockID , int PlayerDBID );
	virtual void RL_LotteryHistoryInfoRequest	( int SrvSockID , int PlayerDBID );
};
