#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_Shop.h"

class NetSrv_Shop : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Shop* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------
    static void	_PG_Shop_CtoL_ShopCloseNotify        ( int sockid , int size , void* data );
    static void	_PG_Shop_CtoL_BuyItemRequest         ( int sockid , int size , void* data );
    static void	_PG_Shop_CtoL_SelectSellItemRequest  ( int sockid , int size , void* data );
//    static void	_PG_Shop_CtoL_SellA11ItemRequest     ( int sockid , int size , void* data );
//    static void	_PG_Shop_CtoL_ClsSellA11Item         ( int sockid , int size , void* data );
    static void	_PG_Shop_CtoL_RepairWeaponRequest    ( int sockid , int size , void* data );
    static void	_PG_Shop_CtoL_BuySellItemRecord		 ( int sockid , int size , void* data );

//    static void	_PG_Shop_CtoL_AccountMoneyShop		 ( int sockid , int size , void* data );

//	static void	_PG_Shop_CtoL_AccountMoneyShopBuyList( int sockid , int size , void* data );
    //-------------------------------------------------------------------
public:    

    static void	S_ShopOpen            	( int SendToID , int TraderID , int ShopObjID , int ItemCount[30] );
    static void	S_ShopClose           	( int SendToID );
    static void	S_BuyItemResult       	( int SendToID , int TraderID , int OrgObjID , int Count , bool Result , PriceTypeENUM PriceType1 = EM_ACPriceType_None , PriceTypeENUM PriceType2 = EM_ACPriceType_None , int Money1 = 0 , int Money2 = 0 );
    static void	S_SelectSellItemResult	( int SendToID , int TraderID , int FieldID , bool Result );
//    static void	S_SellA11ItemResult   	( int SendToID , int TraderID , bool Result );
    static void	S_RepairWeaponResult  	( int SendToID , int TraderID , int Pos , bool IsWeapon , bool Result );

	static void	S_AddSellItemRecord		( int SendToID , SellItemFieldStruct& Info );
	static void	S_DelSellItemRecord		( int SendToID , int EarsePos  );
	static void	S_FixShopItemCount		( int SendToID , int TraderID , int ShopObjID , int Pos , int ItemCount );
	static void S_FixSellItemBackup		( RoleDataEx* Owner );

//	static void	S_AccountMoneyShopResult( int SendToID , int OrgObjID , AccountMoneyShopResultENUM Result );
	static void	S_AccountMoneyShopBuyListResult( int SendToID , AccountMoneyShopResultENUM Result );


    virtual void R_ShopCloseNotify          ( BaseItemObject* ) = 0;
    virtual void R_BuyItemRequest           ( BaseItemObject* , int OrgObjID , int Count , int Pos ) = 0;
    virtual void R_SelectSellItemRequest    ( BaseItemObject* , int FieldID , ItemFieldStruct& Item ) = 0;
//    virtual void R_SellA11ItemRequest       ( BaseItemObject* ) = 0;
//    virtual void R_ClsSellA11Item           ( BaseItemObject* ) = 0; 
    virtual void R_RepairWeaponRequest      ( BaseItemObject* , int Pos , bool IsWeapon) = 0;
	virtual void R_BuySellItemRecord		( BaseItemObject* , SellItemFieldStruct& Info ) = 0;

//	virtual void R_AccountMoneyShop			( BaseItemObject* , int OrgObjID , int Money_Account ) = 0;
//	virtual void R_AccountMoneyShopBuyList	( BaseItemObject* Obj , int TotalCost , AccountMoneyBuyStruct BuyList[20] ) = 0;
};

