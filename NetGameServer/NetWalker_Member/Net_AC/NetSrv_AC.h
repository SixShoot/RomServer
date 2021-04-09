#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_AC.h"

class NetSrv_AC : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_AC* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------

	static void _PG_AC_CtoL_SearchItem			( int sockid , int size , void* data );
	static void _PG_AC_CtoL_GetNextSearchItem	( int sockid , int size , void* data );
	static void _PG_AC_CtoL_GetMyACItemInfo		( int sockid , int size , void* data );
	static void _PG_AC_CtoL_BuyItem				( int sockid , int size , void* data );
	static void _PG_AC_CtoL_SellItem			( int sockid , int size , void* data );
	static void _PG_AC_CtoL_CancelSellItem		( int sockid , int size , void* data );
	static void _PG_AC_CtoL_CloseAC				( int sockid , int size , void* data );
	static void _PG_AC_CtoL_ItemHistoryRequest	( int sockid , int size , void* data );
	static void _PG_AC_CtoL_SellMoney			( int sockid , int size , void* data );

	static void _PG_AC_DCtoL_BuyItemResult		( int sockid , int size , void* data );
	static void _PG_AC_DCtoL_SellItemResult		( int sockid , int size , void* data );
public:    
	//---------------------------------------------------------------------------------------------
	virtual void RC_SearchItem			( BaseItemObject* Obj , ACSearchConditionStruct& Data ) = 0;
	virtual void RC_GetNextSearchItem	( BaseItemObject* Obj ) = 0;
	virtual void RC_GetMyACItemInfo		( BaseItemObject* Obj ) = 0;
	virtual void RC_BuyItem				( BaseItemObject* Obj , int ACItemDBID , int Prices , PriceTypeENUM PricesType , const char* Password ) = 0;
	virtual void RC_SellItem			( BaseItemObject* Obj , int BodyPos , ItemFieldStruct& Item , int BuyOutPrices , int Prices , PriceTypeENUM PricesType , int LiveTime ) = 0;
	virtual void RC_CancelSellItem		( BaseItemObject* Obj , int ACItemDBID ) = 0;
	virtual void RC_CloseAC				( BaseItemObject* Obj ) = 0;
	virtual void RC_ItemHistoryRequest	( BaseItemObject* Obj , int OrgObjID , int Inherent ) = 0;

	virtual void RDC_BuyItemResult		( bool Result , int BuyerDBID , int ACItemDBID , int Prices , PriceTypeENUM PricesType , ACItemStateENUM State ) = 0;
	virtual void RDC_SellItemResult		( ACSellItemResultENUM Result , int SellerDBID , ACItemInfoStruct& Item , int TaxMoney ) = 0;

	virtual void RC_SellMoney		(BaseItemObject* Obj , PriceTypeENUM SellType , int SellMoney, int BuyOutPrices , int Prices , PriceTypeENUM Type , int LiveTime , char* Password ) = 0;
	//---------------------------------------------------------------------------------------------

	static void SDC_SearchItem			( int DBID , ACSearchConditionStruct&	Data );
	static void SDC_GetNextSearchItem	( int DBID );
	static void SDC_GetMyACItemInfo		( int DBID );
	static void SDC_BuyItem				( int DBID , int ACItemDBID , int Prices , PriceTypeENUM	PricesType );
	static void SDC_SellItem			( int SellerDBID , ItemFieldStruct& Item , int Prices , int BuyOutPrices , PriceTypeENUM PricesType , int LiveTime , int TaxMoney );	
	static void SDC_CancelSellItem		( int DBID , int ACItemDBID );
	static void SDC_CloseAC				( int DBID );
	static void SDC_OpenAC				( int DBID , int NpcGUID );
	static void SDC_ItemHistoryRequest	( int DBID , int OrgObjID , int Inherent );

//	static void SC_OpenAC				( int GUID );
	static void SC_BuyItemResult		( int GUID , int ACItemDBID , int Prices , PriceTypeENUM PricesType , ACItemStateENUM State , bool Result );	
	static void SC_SellItemResult		( int GUID , ACItemInfoStruct& Item , ACSellItemResultENUM Result );

	static void SC_AcAlreadyOpen		( int GUID , int TargetGUID );

	static void SC_SellMoneyResult		( int GUID , SellMoneyResultENUM Result , ACItemInfoStruct* Item = NULL );
	//---------------------------------------------------------------------------------------------

};

