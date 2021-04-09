#include "../NetWakerGSrvInc.h"
#include "NetSrv_Shop.h"
//-------------------------------------------------------------------
NetSrv_Shop*    NetSrv_Shop::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_Shop::_Init()
{
    Srv_NetCliReg( PG_Shop_CtoL_ShopCloseNotify       		);
    Srv_NetCliReg( PG_Shop_CtoL_BuyItemRequest        		);
    Srv_NetCliReg( PG_Shop_CtoL_SelectSellItemRequest 		);
//    Srv_NetCliReg( PG_Shop_CtoL_SellA11ItemRequest    );
//    Srv_NetCliReg( PG_Shop_CtoL_ClsSellA11Item        );
    Srv_NetCliReg( PG_Shop_CtoL_RepairWeaponRequest			);
	Srv_NetCliReg( PG_Shop_CtoL_BuySellItemRecord			);

//	Srv_NetCliReg( PG_Shop_CtoL_AccountMoneyShop			);
//	Srv_NetCliReg( PG_Shop_CtoL_AccountMoneyShopBuyList		);
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Shop::_Release()
{
    return true;
}
//-------------------------------------------------------------------
/*
void	NetSrv_Shop::_PG_Shop_CtoL_AccountMoneyShopBuyList	 ( int sockid , int size , void* data )
{
	PG_Shop_CtoL_AccountMoneyShopBuyList* pg =(PG_Shop_CtoL_AccountMoneyShopBuyList*)data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->R_AccountMoneyShopBuyList( Obj , pg->TotalCost , pg->BuyList );
}
*/
/*
void	NetSrv_Shop::_PG_Shop_CtoL_AccountMoneyShop		 ( int sockid , int size , void* data )
{
	PG_Shop_CtoL_AccountMoneyShop* pg =(PG_Shop_CtoL_AccountMoneyShop*)data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->R_AccountMoneyShop( Obj , pg->OrgObjID , pg->Money_Account );
}
*/

void	NetSrv_Shop::_PG_Shop_CtoL_BuySellItemRecord( int sockid , int size , void* data )
{
	PG_Shop_CtoL_BuySellItemRecord* pg =(PG_Shop_CtoL_BuySellItemRecord*)data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	This->R_BuySellItemRecord( Obj , pg->Info );      	
}

void	NetSrv_Shop::_PG_Shop_CtoL_ShopCloseNotify        ( int sockid , int size , void* data )
{
    PG_Shop_CtoL_ShopCloseNotify* pg =(PG_Shop_CtoL_ShopCloseNotify*)data;

    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;
    
    This->R_ShopCloseNotify( Obj );       
}
void	NetSrv_Shop::_PG_Shop_CtoL_BuyItemRequest         ( int sockid , int size , void* data )
{
    PG_Shop_CtoL_BuyItemRequest* pg =(PG_Shop_CtoL_BuyItemRequest*)data;

    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;

    This->R_BuyItemRequest( Obj , pg->OrgObjID , pg->Count , pg->Pos );       
}
void	NetSrv_Shop::_PG_Shop_CtoL_SelectSellItemRequest  ( int sockid , int size , void* data )
{
    PG_Shop_CtoL_SelectSellItemRequest* pg =(PG_Shop_CtoL_SelectSellItemRequest*)data;

    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;

    This->R_SelectSellItemRequest( Obj , pg->FieldID , pg->SellItem );       
}
/*
void	NetSrv_Shop::_PG_Shop_CtoL_SellA11ItemRequest     ( int sockid , int size , void* data )
{
    PG_Shop_CtoL_SellA11ItemRequest* pg =(PG_Shop_CtoL_SellA11ItemRequest*)data;

    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;

    This->R_SellA11ItemRequest( Obj );   
}*/
/*
void	NetSrv_Shop::_PG_Shop_CtoL_ClsSellA11Item         ( int sockid , int size , void* data )
{
    PG_Shop_CtoL_ClsSellA11Item* pg =(PG_Shop_CtoL_ClsSellA11Item*)data;

    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;

    This->R_ClsSellA11Item( Obj );
}*/
void	NetSrv_Shop::_PG_Shop_CtoL_RepairWeaponRequest    ( int sockid , int size , void* data )
{
    PG_Shop_CtoL_RepairWeaponRequest* pg =(PG_Shop_CtoL_RepairWeaponRequest*)data;

    BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
    if( Obj == NULL )
        return ;

    This->R_RepairWeaponRequest( Obj , pg->Pos , pg->IsWeapon );
}

void	NetSrv_Shop::S_ShopOpen            ( int SendToID , int TraderID , int ShopObjID , int ItemCount[30] )
{
    PG_Shop_LtoC_ShopOpen   Send;
    Send.TraderID = TraderID;
    Send.ShopObjID = ShopObjID;
/*    Send.BuyRate = BuyRate;
    Send.SellRate = SellRate;
    Send.RepairWeapon = RepairWeapon;
    memcpy( Send.ItemID , ItemID , sizeof( Send.ItemID ) );*/
    memcpy( Send.ItemCount , ItemCount , sizeof( Send.ItemCount ) );

    Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
void	NetSrv_Shop::S_ShopClose           ( int SendToID )
{
    PG_Shop_LtoC_ShopClose  Send;
//    Send.TraderID = TraderID;

    Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
void	NetSrv_Shop::S_BuyItemResult       ( int SendToID , int TraderID , int OrgObjID , int Count , bool Result , PriceTypeENUM PriceType1 , PriceTypeENUM PriceType2 , int Money1 , int Money2 )
{
    PG_Shop_LtoC_BuyItemResult  Send;
    Send.TraderID = TraderID;
    Send.OrgObjID = OrgObjID;
    Send.Count    = Count;
    Send.Result   = Result;
	Send.PriceType[0] = PriceType1;
	Send.PriceType[1] = PriceType2;
	Send.Money[0] = Money1;
	Send.Money[1] = Money2;

    Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
void	NetSrv_Shop::S_SelectSellItemResult( int SendToID , int TraderID , int FieldID , bool Result )
{
    PG_Shop_LtoC_SelectSellItemResult  Send;
    Send.TraderID = TraderID;
    Send.FieldID  = FieldID;
    Send.Result   = Result;

    Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
/*
void	NetSrv_Shop::S_SellA11ItemResult   ( int SendToID , int TraderID , bool Result )
{
    PG_Shop_LtoC_SellA11ItemResult  Send;
    Send.TraderID = TraderID;
    Send.Result   = Result;

    Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}*/
void	NetSrv_Shop::S_RepairWeaponResult  ( int SendToID , int TraderID , int Pos , bool IsWeapon , bool Result )
{
    PG_Shop_LtoC_RepairWeaponResult  Send;
    Send.TraderID = TraderID;
    Send.Result   = Result;
    Send.Pos = Pos;
    Send.IsWeapon = IsWeapon;

    Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}

void	NetSrv_Shop::S_AddSellItemRecord( int SendToID , SellItemFieldStruct& Info )
{
	PG_Shop_LtoC_AddSellItemRecord Send;

	Send.Info = Info;
	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
void	NetSrv_Shop::S_DelSellItemRecord( int SendToID , int EarsePos )
{
	PG_Shop_LtoC_DelSellItemRecord Send;
	Send.EarsePos = EarsePos;
	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
void	NetSrv_Shop::S_FixShopItemCount	( int SendToID , int TraderID , int ShopObjID , int Pos , int ItemCount )
{
	PG_Shop_LtoC_FixShopItemCount Send;
	Send.ItemCount = ItemCount;
	Send.ShopObjID = ShopObjID;
	Send.TraderID  = TraderID;
	Send.Pos	   = Pos;
	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
/*
void	NetSrv_Shop::S_AccountMoneyShopResult( int SendToID , int OrgObjID , AccountMoneyShopResultENUM Result )
{
	PG_Shop_LtoC_AccountMoneyShopResult Send;
	Send.OrgObjID = OrgObjID;
	Send.Result = Result;

	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
*/
void	NetSrv_Shop::S_AccountMoneyShopBuyListResult( int SendToID , AccountMoneyShopResultENUM Result )
{
	PG_Shop_LtoC_AccountMoneyShopBuyListResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}

void	NetSrv_Shop::S_FixSellItemBackup		( RoleDataEx* Owner )
{
	PG_Shop_LtoC_FixSellItemBackup Send;
	Send.SellItemBackup = Owner->PlayerTempData->SellItemBackup;
	Global::SendToCli_ByGUID( Owner->GUID() , sizeof(Send) , &Send );
}