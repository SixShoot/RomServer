#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_Lottery.h"

class NetSrv_Lottery : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Lottery* This;
    static bool _Init();
    static bool _Release();

	static void _PG_Lottery_CtoL_ExchangePrize			( int sockid , int size , void* data ); 
	static void _PG_Lottery_CtoL_CloseExchangePrize		( int sockid , int size , void* data ); 
	static void _PG_Lottery_CtoL_BuyLottery				( int sockid , int size , void* data ); 
	static void _PG_Lottery_CtoL_CloseBuyLottery		( int sockid , int size , void* data ); 
	static void _PG_Lottery_CtoL_LotteryInfoRequest		( int sockid , int size , void* data ); 

	static void _PG_Lottery_DtoL_ExchangePrizeResult	( int sockid , int size , void* data ); 	
	static void _PG_Lottery_DtoL_BuyLotteryResult		( int sockid , int size , void* data );	
	static void _PG_Lottery_CtoL_LotteryHistoryInfoRequest( int sockid , int size , void* data );	
public:    
	
	static void	SC_OpenExchangePrize			( int SendID , int TargetNPCID );		
	static void	SC_ExchangePrizeResultOK		( int SendID , int Money , int Prize1Count , int Prize2Count , int Prize3Count );	
	static void	SC_ExchangePrizeResultFailed	( int SendID );	
	static void	SC_CloseExchangePrize			( int SendID );	
	static void	SC_OpenBuyLottery				( int SendID , int TargetNPCID );	
	static void	SC_BuyLotteryResult				( int SendID , bool Result );	
	static void	SC_CloseBuyLottery				( int SendID );	


	static void	SD_ExchangePrize			( ItemFieldStruct Item , int Pos , int PlayerDBID );	
	static void	SD_BuyLottery				( int  Money , int PlayerDBID , int Count , int Number[20] );	
	static void	SD_LotteryInfoRequest		( int PlayerDBID );		
	static void	SD_LotteryHistoryInfoRequest( int PlayerDBID );
	
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_ExchangePrize				( BaseItemObject* Obj , ItemFieldStruct& Item , int Pos ) = 0;
	virtual void RC_CloseExchangePrize			( BaseItemObject* Obj ) = 0;
	virtual void RC_BuyLottery					( BaseItemObject* Obj , int Money , int Count , int Number[20] ) = 0;
	virtual void RC_CloseBuyLottery				( BaseItemObject* Obj ) = 0;
	virtual void RC_LotteryInfoRequest			( BaseItemObject* Obj ) = 0;
	virtual void RC_LotteryHistoryInfoRequest	( BaseItemObject* Obj ) = 0;

	virtual void RD_ExchangePrizeResult( ItemFieldStruct& Item , int Pos , int PlayerDBID , int PrizeCount[3] , int TotalMoney , bool Result ) = 0;
	virtual void RD_BuyLotteryResult( int Version , int Money , int PlayerDBID , int Count, int Number[20] , bool Result ) = 0;
};

