#pragma  once

#include "NetSrv_AC.h"

class NetSrv_ACChild : public NetSrv_AC
{

public:
    static bool Init();
    static bool Release();

	virtual void RC_SearchItem			( BaseItemObject* Obj , ACSearchConditionStruct& Data );
	virtual void RC_GetNextSearchItem	( BaseItemObject* Obj );
	virtual void RC_GetMyACItemInfo		( BaseItemObject* Obj );
	virtual void RC_BuyItem				( BaseItemObject* Obj , int ACItemDBID , int Prices , PriceTypeENUM PricesType , const char* Password );
	virtual void RC_SellItem			( BaseItemObject* Obj , int BodyPos , ItemFieldStruct& Item , int BuyOutPrices , int Prices , PriceTypeENUM PricesType , int LiveTime );
	virtual void RC_CancelSellItem		( BaseItemObject* Obj , int ACItemDBID );
	virtual void RC_CloseAC				( BaseItemObject* Obj );
	virtual void RC_ItemHistoryRequest	( BaseItemObject* Obj , int OrgObjID , int Inherent );
	virtual void RC_SellMoney		(BaseItemObject* Obj , PriceTypeENUM SellType , int SellMoney, int BuyOutPrices , int Prices , PriceTypeENUM Type , int LiveTime , char* Password );

	virtual void RDC_BuyItemResult		( bool Result , int BuyerDBID , int ACItemDBID , int Prices , PriceTypeENUM PricesType , ACItemStateENUM State );
	virtual void RDC_SellItemResult		( ACSellItemResultENUM Result , int SellerDBID , ACItemInfoStruct& Item , int TaxMoney );

};

