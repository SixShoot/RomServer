#pragma once

#include "NetDC_Lottery.h"



//--------------------------------------------------------------------------------------------
class CNetDC_LotteryChild : public CNetDC_Lottery
{
	static vector< LotteryInfoStruct > 		_LotteryHistory;
	static LotteryInfoStruct				_Last5LotteryInfo[5];
	static int							  	_LotteryVersion;	//�̷s�@�� �|���}�� 
	static int							  	_LotteryNumber[5];	//�̷s�@��
	static int								_LotteryPrizeCount[3];
	static int							  	_LotteryTotalMoney;	//�ֿn���B
	static int							  	_RunLotteryTime;		//�]�ֳz���ɶ�
	static bool								_IsInitReady;

	static int _OnTimeProc_( SchedularInfo* Obj , void* InputClass );

	//��l�]�w
	static bool _SQLCmdInitProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdInitProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//�}���B�z
	static bool _SQLCmdLotteryRun( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLotteryRunResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );


	static bool _SQLCmdLotteryExchangePrize( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLotteryExchangePrizeResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdLotteryBuy( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLotteryBuyResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	
	//�w�ɨ��`����
	static bool _SQLCmdLotteryGetTotoalMoney( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLotteryGetTotoalMoneyResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	


	//�p�⤤���ƶq
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
