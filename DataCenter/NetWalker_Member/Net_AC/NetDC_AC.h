
#pragma once
#include "PG/PG_AC.h"
#include "../../MainProc/Global.h"

class CNetDC_AC : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetDC_AC*	This;

	static bool				_Init();
	static bool				_Release();
	//-------------------------------------------------------------------	
	static void _PG_AC_LtoDC_SearchItem			( int ServerID , int Size , void* Data );
	static void _PG_AC_LtoDC_GetNextSearchItem	( int ServerID , int Size , void* Data );
	static void _PG_AC_LtoDC_GetMyACItemInfo	( int ServerID , int Size , void* Data );
	static void _PG_AC_LtoDC_BuyItem			( int ServerID , int Size , void* Data );
	static void _PG_AC_LtoDC_SellItem			( int ServerID , int Size , void* Data );
	static void _PG_AC_LtoDC_CancelSellItem		( int ServerID , int Size , void* Data );
	static void _PG_AC_LtoDC_CloseAC			( int ServerID , int Size , void* Data );
	static void _PG_AC_LtoDC_OpenAC				( int ServerID , int Size , void* Data );
	static void _PG_AC_LtoDC_ItemHistoryRequest ( int ServerID , int Size , void* Data );

public:
    //-------------------------------------------------------------------	 

	virtual void RL_SearchItem			( int DBID , ACSearchConditionStruct& Data ) = 0;
	virtual void RL_GetNextSearchItem	( int DBID ) = 0;
	virtual void RL_GetMyACItemInfo		( int DBID ) = 0;
	virtual void RL_BuyItem				( int SrvSockID , int BuyerDBID , int ACItemDBID , int Prices , PriceTypeENUM PricesType ) = 0;
	virtual void RL_SellItem			( int SrvSockID , int SellerDBID , ItemFieldStruct& Item , int Prices , int BuyOutPrices , PriceTypeENUM	PricesType , int LiveTime , int TaxMoney ) = 0;
	virtual void RL_CancelSellItem		( int DBID , int ACItemDBID ) = 0;
	virtual void RL_CloseAC				( int DBID ) = 0;
	virtual void RL_OpenAC				( int DBID ,  int NpcGUID ) = 0;
	virtual void RL_ItemHistoryRequest	( int DBID , int OrgObjID , int Inherent  ) = 0;
	//-------------------------------------------------------------------	
	//¦^À³Client
	//-------------------------------------------------------------------	
	static void SC_SearchItemCountResult	( int DBID , int BeginIndex , int MaxCount , int Count );
	static void SC_SearchItemResult			( int DBID , int Index , ACItemInfoStruct& Data , char* SellerName , int BuyerDBID );
	static void SC_FixSearchItemResult		( int DBID , int ACItemDBID , int NewPrices , PriceTypeENUM	PricesType , ACItemStateENUM	State , int BuyerDBID );
	static void SC_FixBuyOrderItem			( int DBID , int ACItemDBID , int NewPrices , PriceTypeENUM	PricesType , ACItemStateENUM	State , int BuyerDBID );
	static void SC_FixSellOrderItem			( int DBID , int ACItemDBID , int NewPrices , PriceTypeENUM	PricesType , ACItemStateENUM	State , char* Buyer );
	static void SC_SelfCompetitive_BuyItem	( int DBID , int BuyerDBID , ACItemInfoStruct& Data , char* SellerName );
	static void SC_SelfCompetitive_SellItem	( int DBID , ACItemInfoStruct& Data , char* BuyerName );
	static void SC_SelfCompetitive_ItemDataEnd	( int DBID  );
	static void SC_CancelSellItemResult		( int DBID , int ACItemDBID , bool Result );
	static void SC_CloseAC					( int DBID );
	static void SC_OpenAC					( int DBID , int NpcGUID , bool Result );
	static void SC_LostCompetitiveItem		( int DBID , int ACItemDBID , ItemFieldStruct& Item );
	static void SC_ItemHistoryList			( int DBID , vector<DB_ACItemHistoryStruct>& HistoryList );
	
	static void SL_BuyItemResult			( int SrvSockID , int BuyerDBID , int ACItemDBID , int Prices , PriceTypeENUM PricesType , ACItemStateENUM	State , bool Result );
	static void SL_SellItemResult			( int SrvSockID , int SellerDBID , ACItemInfoStruct& Item , int TaxMoney , ACSellItemResultENUM Result );
	
	static void SC_AcTradeOk				( int SendDBID , const char* BuyerName ,  const char* SellName , PriceTypeENUM PriceType , int Money , int Tax ,ItemFieldStruct& Item );
	//-------------------------------------------------------------------	
};
