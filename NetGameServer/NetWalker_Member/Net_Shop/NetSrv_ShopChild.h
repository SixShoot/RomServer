#pragma once

#include "NetSrv_Shop.h"

class NetSrv_ShopChild : public NetSrv_Shop
{
public:
    static bool Init();
    static bool Release();

    virtual void R_ShopCloseNotify          ( BaseItemObject* );
    virtual void R_BuyItemRequest           ( BaseItemObject* , int OrgObjID , int Count , int Pos );
    virtual void R_SelectSellItemRequest    ( BaseItemObject* , int FieldID , ItemFieldStruct& Item );
//    virtual void R_SellA11ItemRequest       ( BaseItemObject* );
//    virtual void R_ClsSellA11Item           ( BaseItemObject* ); 
    virtual void R_RepairWeaponRequest      ( BaseItemObject* , int Pos , bool IsWeapon );
	virtual void R_BuySellItemRecord		( BaseItemObject* , SellItemFieldStruct& Info );

//	virtual void R_AccountMoneyShop			( BaseItemObject* , int OrgObjID , int Money_Account );
//	virtual void R_AccountMoneyShopBuyList	( BaseItemObject* Obj , int TotalCost , AccountMoneyBuyStruct BuyList[20] );
};

