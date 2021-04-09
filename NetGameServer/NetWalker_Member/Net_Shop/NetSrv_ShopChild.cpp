#include "../NetWakerGSrvInc.h"
#include "NetSrv_ShopChild.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_ShopChild::Init()
{
    NetSrv_Shop::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_ShopChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_ShopChild::Release()
{
    if( This == NULL )
        return false;

	NetSrv_Shop::_Release();

    delete This;
    This = NULL;

    return true;
    
}
void NetSrv_ShopChild::R_ShopCloseNotify      ( BaseItemObject* Obj )
{
    RoleDataEx* Role = Obj->Role();
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_Shop) == false )
		return;

	Role->ClsOpenMenu();
    //Role->TempData.ShopInfo.TargetID = -1;
	//Role->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_None;
}
void NetSrv_ShopChild::R_BuyItemRequest       ( BaseItemObject* Obj , int OrgObjID , int Count , int Pos )
{
    RoleDataEx* Role = Obj->Role();
    RoleDataEx* Shop = Global::GetRoleDataByGUID( Role->TempData.ShopInfo.TargetID );

	if( Role->TempData.Sys.OnChangeZone != false )
		return;


	if( Shop == NULL || Count <= 0 || unsigned(Pos) >= 30 )
    {
        S_BuyItemResult( Role->GUID() , Role->TempData.ShopInfo.TargetID , OrgObjID , Count , false );
        S_ShopClose( Role->GUID() );
        return;
    }

	if( Role->BaseData.SysFlag.DisableTrade )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
		S_BuyItemResult( Role->GUID() , Role->TempData.ShopInfo.TargetID , OrgObjID , Count , false );
		S_ShopClose( Role->GUID() );
		return ;
	}
	
	//距離
	if(	Role->CheckOpenMenu( EM_RoleOpenMenuType_Shop) == false )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_ShopNoOpen );
		S_BuyItemResult( Role->GUID() , Role->TempData.ShopInfo.TargetID , OrgObjID , Count , false );
		S_ShopClose( Role->GUID() );

		return;
	}


    ShopStateStruct &ShopInfo = Shop->TempData.ShopInfo;
    GameObjDbStructEx*  ShopOrgDB	= Global::GetObjDB( ShopInfo.ShopObjID );

    if( ShopOrgDB == NULL || !ShopOrgDB->IsShop() )
    {
        S_BuyItemResult( Role->GUID() , Shop->GUID() , OrgObjID , Count , false );
        S_ShopClose( Role->GUID()  );

		Role->TempData.ShopInfo.TargetID = -1;
		Role->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_Shop;
        return;
    }

    int BuyPos = Pos;

	/*
    for( int i = 0 ; i < 30 ; i++ )
    {
        if( ShopOrgDB->Shop.SellItemID[i] == OrgObjID )
        {
            BuyPos = i;
            break;
        }
    }
	*/
    if( ShopOrgDB->Shop.SellItemID[BuyPos] != OrgObjID  )
    {
        //Role->Msg( "沒賣此物品" );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_ItemNotSell );
        S_BuyItemResult( Role->GUID() , Shop->GUID() , OrgObjID , Count , false );
        return;
    }

    if( ShopInfo.ItemCount[ BuyPos ] < Count )
    {
        //Role->Msg( "購買過多" );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_CountTooMany );
        S_BuyItemResult( Role->GUID() , Shop->GUID() , OrgObjID , Count , false );
        return;
    }

    GameObjDbStructEx*  BuyItemOrgDB = Global::GetObjDB( OrgObjID );
    if( BuyItemOrgDB == NULL )
    {
        //Role->Msg( "????此物品不存在" );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_ItemNotExist );
        S_BuyItemResult( Role->GUID() , Shop->GUID() , OrgObjID , Count , false );
        return;
    }
	
	if( Count > BuyItemOrgDB->MaxHeap )
	{
		//Role->Msg( "一次最多只可以買一堆的量" );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_CountError );
		S_BuyItemResult( Role->GUID() , Shop->GUID() , OrgObjID , Count , false );
		return;
	}

	//計算空欄位數量
	int EmptyCount = Role->EmptyPacketCount();

	if( Role->PlayerBaseData->ItemTemp.Size() >= EmptyCount )
	{
		//Role->Msg( "東西過多無法買入" );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_BagFull );
		S_BuyItemResult( Role->GUID() , Shop->GUID() , OrgObjID , Count , false );
		return;
	}


	//////////////////////////////////////////////////////////////////////////
	//檢查是否有特別設定買價
	//BuyPos
	int SellCost[2];// = ShopOrgDB->Shop.SellCost[ BuyPos ];
	PriceTypeENUM CostType[2];// = ShopOrgDB->Shop.CostType[ BuyPos ];
	SellCost[0] = ShopOrgDB->Shop.SellCost[ BuyPos ][0] * Count;
	SellCost[1] = ShopOrgDB->Shop.SellCost[ BuyPos ][1] * Count;
	CostType[0] = ShopOrgDB->Shop.CostType[ BuyPos ][0];
	CostType[1] = ShopOrgDB->Shop.CostType[ BuyPos ][1];

	if( SellCost[0] != 0 || SellCost[1] != 0 )
	{
		if( CostType[0] == CostType[1] )		
		{
			SellCost[0] += SellCost[1];
			SellCost[1] = 0;
		}

		if( SellCost[0] < 0 || SellCost[1] < 0 )
		{
			Print( LV5 , "NetSrv_ShopChild::R_BuyItemRequest" , "SellCost[0] < 0 || SellCost[1] < 0  ShopObjID = %d " , Shop->BaseData.ItemInfo.OrgObjID );
			S_BuyItemResult( Role->GUID() , Shop->GUID() , OrgObjID , Count , false );
			return;
		}

		//檢查某種錢幣是否足夠
		if(		CheckMoneyBase( Role , CostType[0] , SellCost[0] ) == false 
			||	CheckMoneyBase( Role , CostType[1] , SellCost[1] ) == false )
		{
			Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_MomeyNotEnought );
			S_BuyItemResult( Role->GUID() , Shop->GUID() , OrgObjID , Count , false );
			return;
		}

		AddMoneyBase( Role , EM_ActionType_Buy , CostType[0] , SellCost[0]*-1 );
		AddMoneyBase( Role , EM_ActionType_Buy , CostType[1] , SellCost[1]*-1 );
		S_BuyItemResult( Role->GUID() , Shop->GUID() , OrgObjID , Count , true , CostType[0] , CostType[1] , SellCost[0] , SellCost[1] );
	}
	else
	{
		int Money;
		if( BuyItemOrgDB->PricesType == EM_ACPriceType_GameMoney )
			Money = int( float( BuyItemOrgDB->Cost ) * ShopOrgDB->Shop.BuyRate  / 100 + 0.5 ) * Count;
		else
			Money = int( float( BuyItemOrgDB->SpecialMoneyCost ) * ShopOrgDB->Shop.BuyRate  / 100 + 0.5 ) * Count;

		if( Money < 0 )
		{
			Print( LV5 , "NetSrv_ShopChild::R_BuyItemRequest" , "Buy ItemObjID = %d Money=%d" , OrgObjID , Money );
			return;
		}

		if( CheckMoneyBase( Role , BuyItemOrgDB->PricesType , Money ) == false )
		{
			Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_MomeyNotEnought );
			S_BuyItemResult( Role->GUID() , Shop->GUID() , OrgObjID , Count , false );
			return;
		}
		AddMoneyBase( Role , EM_ActionType_Buy , BuyItemOrgDB->PricesType , Money*-1 );

		S_BuyItemResult( Role->GUID() , Shop->GUID() , OrgObjID , Count , true , BuyItemOrgDB->PricesType , EM_ACPriceType_None , Money , 0 );
	}
	//////////////////////////////////////////////////////////////////////////

//	S_BuyItemResult( Role->GUID() , Shop->GUID() , OrgObjID , Count , true );

	if( BuyItemOrgDB->IsTreasure() )
	{
		Role->GiveTreasure_Gamble( OrgObjID , Shop );
	}
	else
	{
		Role->GiveItem( OrgObjID , Count , EM_ActionType_Buy , Shop , "" );
	}

	//更新物品數量
	ShopInfo.ItemCount[ BuyPos ] -= Count;

	vector<BaseSortStruct>*	SearchList = Global::SearchRangeObject( Role , Role->Pos()->X , Role->Pos()->Y , Role->Pos()->Z , EM_SearchRange_Player , 200	);

	for( int i = 0 ; i < (int)SearchList->size() ; i++ )
	{
		RoleDataEx*	Other = (RoleDataEx*)(*SearchList)[i].Data ;
		if( Role->TempData.ShopInfo.TargetID != Other->TempData.ShopInfo.TargetID )
			continue;

		S_FixShopItemCount( Other->GUID() , Other->TempData.ShopInfo.TargetID ,  ShopInfo.ShopObjID , BuyPos , ShopInfo.ItemCount[ BuyPos ] );
	}

	return;

}
void NetSrv_ShopChild::R_SelectSellItemRequest( BaseItemObject* Obj , int FieldID , ItemFieldStruct& Item )
{
    RoleDataEx* Role = Obj->Role();

	if( Role->TempData.Sys.OnChangeZone != false )
		return;


    RoleDataEx* Shop = Global::GetRoleDataByGUID( Role->TempData.ShopInfo.TargetID );
	ItemFieldStruct* BodyItem =  Role->GetBodyItem( FieldID );

	if( BodyItem == NULL )
	{
		//Role->Msg( "物品資料不符" );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_SellDataError );
		NetSrv_Item::FixItemInfo_Body( Role->GUID() , *BodyItem , FieldID );
		S_SelectSellItemResult( Obj->GUID() , Shop->GUID() , FieldID , false );
		return;
	}

	if( Role->BaseData.SysFlag.DisableTrade )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
		NetSrv_Item::FixItemInfo_Body( Role->GUID() , *BodyItem , FieldID );
		S_SelectSellItemResult( Obj->GUID() , Role->TempData.ShopInfo.TargetID , FieldID , false );
		S_ShopClose( Role->GUID() );
		return ;
	}
	
    if( Shop == NULL )
    {
		NetSrv_Item::FixItemInfo_Body( Role->GUID() , *BodyItem , FieldID );
        S_SelectSellItemResult( Obj->GUID() , Role->TempData.ShopInfo.TargetID , FieldID , false );
        S_ShopClose( Role->GUID() );
        return;
    }

	//距離
	if(	Role->CheckOpenMenu( EM_RoleOpenMenuType_Shop) == false )
	{
		//Role->Msg( "商店開啟狀況有問題" );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_ShopNoOpen );
		NetSrv_Item::FixItemInfo_Body( Role->GUID() , *BodyItem , FieldID );
		S_SelectSellItemResult( Obj->GUID() , Role->TempData.ShopInfo.TargetID , FieldID , false );
		S_ShopClose( Role->GUID() );
		return;
	}

	GameObjDbStructEx*  ShopOrgDB = Global::GetObjDB( Shop->TempData.ShopInfo.ShopObjID );

    if(  *BodyItem != Item )
    {
        //Role->Msg( "物品資料不符" );
		NetSrv_Item::FixItemInfo_Body( Role->GUID() , *BodyItem , FieldID );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_SellDataError );
        S_SelectSellItemResult( Obj->GUID() , Shop->GUID() , FieldID , false );
        return;
    }

	if( BodyItem->Mode.ItemLock != false )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_ItemLock );
		NetSrv_Item::FixItemInfo_Body( Role->GUID() , *BodyItem , FieldID );
		S_SelectSellItemResult( Obj->GUID() , Shop->GUID() , FieldID , false );
		return;

	}

    if( BodyItem->Pos != EM_ItemState_NONE )
    {
        //Role->Msg( "此物品在裝備中" );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_SellItemOnUse );
		NetSrv_Item::FixItemInfo_Body( Role->GUID() , *BodyItem , FieldID );
		S_FixSellItemBackup( Role );
        S_SelectSellItemResult( Obj->GUID() , Shop->GUID() , FieldID , false );
        return;
    }
    GameObjDbStructEx*  ItemOrgDB = Global::GetObjDB( Item.OrgObjID );

	// TODO
	if( ItemOrgDB == NULL )
	{
		//Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_SellItemOnUse );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_SellDataError );
		NetSrv_Item::FixItemInfo_Body( Role->GUID() , *BodyItem , FieldID );
		S_SelectSellItemResult( Obj->GUID() , Shop->GUID() , FieldID , false );
		return;
	}


    if( ItemOrgDB->Mode.Sell == false )
    {
		//Role->Msg( "此物品不能賣" );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_CanNotSell );
		NetSrv_Item::FixItemInfo_Body( Role->GUID() , *BodyItem , FieldID );
        S_SelectSellItemResult( Obj->GUID() , Shop->GUID() , FieldID , false );
        return;
	}

	if( BodyItem->Count > ItemOrgDB->MaxHeap )
	{
		BodyItem->Count = ItemOrgDB->MaxHeap;
		NetSrv_Item::FixItemInfo_Body( Role->GUID() , *BodyItem , FieldID );
		S_SelectSellItemResult( Obj->GUID() , Shop->GUID() , FieldID , false );
	}

	SellItemFieldStruct	Info;
	Info.Money = int( float(ShopOrgDB->Shop.SellRate * ItemOrgDB->Cost) / 100 + 0.5 ) * Item.Count;
	if( Info.Money <= 0 )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_CanNotSell );
		NetSrv_Item::FixItemInfo_Body( Role->GUID() , *BodyItem , FieldID );
		S_SelectSellItemResult( Obj->GUID() , Shop->GUID() , FieldID , false );

		return;
	//	Info.Money = 0;
	}

	Info.Item = *BodyItem;

	if( Role->PlayerTempData->SellItemBackup.Size() >= _MAX_SELL_ITEM_SAVE_COUNT_ )
	{
		S_DelSellItemRecord( Role->GUID() , 0 );
		Role->PlayerTempData->SellItemBackup.Pop_Front();
	}
	Role->PlayerTempData->SellItemBackup.Push_Back( Info );
	S_AddSellItemRecord( Role->GUID() , Info );


	Log_ItemDestroy( Role , EM_ActionType_Sell , *BodyItem , BodyItem->Count , Shop->DBID() , "");
	BodyItem->Init();
	NetSrv_Item::FixItemInfo_Body( Role->GUID() , *BodyItem , FieldID );
	Role->AddBodyMoney( Info.Money , Shop , EM_ActionType_Sell , true );

    S_SelectSellItemResult( Obj->GUID() , Shop->GUID() , FieldID , true );
/*
	NetSrv_Talk::GameMsgEventData( Obj->GUID() , EM_GameMessageEventData_ShopSell 
		, EM_GameMessageDataType_ObjID_Name , Item.Inherent
		, EM_GameMessageDataType_ObjID_Name , Item.OrgObjID 
		, EM_GameMessageDataType_Int		, Item.Count	
		, EM_GameMessageDataType_Int		, Info.Money );
		*/
	return;
}
/*
void NetSrv_ShopChild::R_SellA11ItemRequest   ( BaseItemObject* Obj )
{
    int Money = 0;
    RoleDataEx* Role = Obj->Role();
    RoleDataEx* Shop = Global::GetRoleDataByGUID( Role->TempData.ShopInfo.TargetID );
    if( Shop == NULL )
    {
        S_SellA11ItemResult( Obj->GUID() , Role->TempData.ShopInfo.TargetID , false );
        S_ShopClose( Role->GUID() , Shop->GUID() );
        return;
    }

    ShopStateStruct &ShopInfo  = Shop->TempData.ShopInfo;
    GameObjDbStructEx*  ShopOrgDB  = Global::GetObjDB( ShopInfo.ShopObjID );
    if( ShopOrgDB == NULL || !ShopOrgDB->IsShop() )
    {
        S_SellA11ItemResult( Obj->GUID() , Role->TempData.ShopInfo.TargetID , false );
        S_ShopClose( Role->GUID() , Shop->GUID() );
        return;
    }


    for( int i = 0 ; i < Role->PlayerBaseData->Body.Count ; i++ )
    {
		SellItemFieldStruct	Info;
        ItemFieldStruct& Item = Role->PlayerBaseData->Body.Item[i];
        GameObjDbStructEx*  ItemOrgDB = Global::GetObjDB( Item.OrgObjID );

        if( Item.Pos == EM_USE_SELSELL )
        {	
			Info.Money = ShopOrgDB->Shop.SellRate * ItemOrgDB->Cost / 100 * Item.Count;
			Info.Item = Item;
			if( Role->PlayerTempData->SellItemBackup.Size() >= _MAX_SELL_ITEM_SAVE_COUNT_ )
			{
				S_DelSellItemRecord( Role->GUID() , 0 );
				Role->PlayerTempData->SellItemBackup.Pop_Front();
			}
			Role->PlayerTempData->SellItemBackup.Push_Back( Info );
			S_AddSellItemRecord( Role->GUID() , Info );

            Money = Money + Info.Money;
            Item.Init();
            NetSrv_Item::FixItemInfo_Body( Role->GUID() , Item , i );
        }        
    }

    Role->AddBodyMoney( Money );
    S_SellA11ItemResult( Role->GUID() , Shop->GUID() , true );

}
void NetSrv_ShopChild::R_ClsSellA11Item       ( BaseItemObject* Obj ) 
{
    RoleDataEx* Role = Obj->Role();
    for( int i = 0 ; i < Role->PlayerBaseData->Body.Count ; i++ )
    {
        ItemFieldStruct& Item = Role->PlayerBaseData->Body.Item[i];
        if( Item.Pos == EM_USE_SELSELL )
        {
            //Role->PlayerBaseData->Body.Item[i].Pos = EM_ItemState_NONE;
            Item.Pos = EM_ItemState_NONE;
            NetSrv_Item::FixItemInfo_Body( Role->GUID() , Item , i );
        }
    }
}
*/
void NetSrv_ShopChild::R_RepairWeaponRequest  ( BaseItemObject* Obj , int Pos , bool IsWeapon )
{
    RoleDataEx* Role = Obj->Role();

	if( Role->TempData.Sys.OnChangeZone != false )
		return;

    RoleDataEx* Shop = Global::GetRoleDataByGUID( Role->TempData.ShopInfo.TargetID );
    if( Shop == NULL )
    {
        S_RepairWeaponResult( Obj->GUID() , Role->TempData.ShopInfo.TargetID , Pos , IsWeapon , false );
        S_ShopClose( Role->GUID() );
        return;
    }
	if(	Role->CheckOpenMenu( EM_RoleOpenMenuType_Shop) == false )
	{
		S_RepairWeaponResult( Obj->GUID() , Role->TempData.ShopInfo.TargetID , Pos , IsWeapon , false );
		S_ShopClose( Role->GUID() );
		return;
	}

	if( Pos == -1 && IsWeapon != false )
	{
		//備份裝備資料
		EQStruct	EQ = Role->BaseData.EQ;
	
		int Cost = 0;
		for( int i = 0 ; i < EM_EQWearPos_MaxCount ; i++  )
		{
			Cost += Role->CalRepairWeapon( &Role->BaseData.EQ.Item[i] , &EQ.Item[i] );
		}

		if( Cost == 0 || Role->AddBodyMoney( Cost , Shop , EM_ActionType_FixEQ , true ) == false )
		{			
			S_RepairWeaponResult( Obj->GUID() , Role->TempData.ShopInfo.TargetID , Pos , IsWeapon , false );
		}
		else
		{
			Role->BaseData.EQ = EQ;
			for( unsigned i = 0 ; i < EM_EQWearPos_MaxCount ; i++ )
			{
				if( Role->BaseData.EQ.Item[i].IsEmpty() )
					continue;
				Role->Net_FixItemInfo_EQ( i );
			}
			S_RepairWeaponResult( Obj->GUID() , Role->TempData.ShopInfo.TargetID , Pos , IsWeapon , true );
		}
		return;
	}



    ItemFieldStruct* Item = NULL;
    if( IsWeapon == false )
        Item =  Role->GetBodyItem( Pos );
    else
        Item = Role->GetEqItem( Pos );

    if( Item == NULL )
    {
        //Role->Msg( "物品資料不符" );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_PepairDataError );
        S_RepairWeaponResult( Obj->GUID() , Role->TempData.ShopInfo.TargetID , Pos , IsWeapon , false );
        return;
    }

	ItemFieldStruct OutItem;
	bool	FixResult = false;
	int Cost = Role->CalRepairWeapon( Item , &OutItem );
	if( Cost != 0 )
	{
		if( Role->AddBodyMoney( Cost , Shop , EM_ActionType_FixEQ , true ) != false )
		{
			*Item = OutItem;
			FixResult = true;

			if( IsWeapon == false )
				Role->Net_FixItemInfo_Body( Pos );
			else
				Role->Net_FixItemInfo_EQ( Pos );
		}
	}
	Role->TempData.UpdateInfo.Recalculate_All = true;
    S_RepairWeaponResult( Obj->GUID() , Role->TempData.ShopInfo.TargetID , Pos , IsWeapon , FixResult );
}

void NetSrv_ShopChild::R_BuySellItemRecord		( BaseItemObject* Obj , SellItemFieldStruct& Info )
{

	int		i;
	bool	SearchObjOK = false;
    RoleDataEx* Role = Obj->Role();

	if( Role->TempData.Sys.OnChangeZone != false )
		return;


	if(	Role->CheckOpenMenu( EM_RoleOpenMenuType_Shop) == false )
	{
		S_ShopClose( Role->GUID() );
		return;
	}

	//金錢檢查
	if( Role->PlayerBaseData->Body.Money < Info.Money )
	{
		//Role->Msg( "物品買 回金額不足" );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_MomeyNotEnought );
		S_BuyItemResult( Role->GUID() , -1 , Info.Item.OrgObjID , Info.Item.Count , false );
		return;
	}

	//尋找是否有此物品
	for( i = 0 ; i < Role->PlayerTempData->SellItemBackup.Size() ; i++ )
	{
		if( Role->PlayerTempData->SellItemBackup[i] == Info )
		{
			Info = Role->PlayerTempData->SellItemBackup[i];
			Role->PlayerTempData->SellItemBackup.Erase( i );
			SearchObjOK = true;			
			S_DelSellItemRecord( Role->GUID() , i );
			break;
		}
	}

	if( SearchObjOK == false )
	{
		//Role->Msg( "物品買 找不到此物品" );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_ItemNotExist );

		S_BuyItemResult( Role->GUID() , -1 , Info.Item.OrgObjID , Info.Item.Count , false );
		return;
	}

	//加一個物品到暫存區
    if(	Role->AddItemtoBuf	( Info.Item ) != true )	
	{
		//Role->Msg( "物品買 包裹滿了" );
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Shop_BagFull );
		return;
	}
    //通知玩家
    Role->Net_ItemInBuffer( Info.Item );
	Role->Log_ItemTrade( NULL , EM_ActionType_Buy_Record , Info.Item , "" );

	Role->AddBodyMoney( Info.Money * -1 , NULL , EM_ActionType_Buy_Record , true );

	S_BuyItemResult( Role->GUID() , -1 , Info.Item.OrgObjID , Info.Item.Count , true , EM_ACPriceType_GameMoney , EM_ACPriceType_None , Info.Money , 0 );

}
/*
void NetSrv_ShopChild::R_AccountMoneyShop			( BaseItemObject* Obj , int OrgObjID , int Money_Account )
{
	
	RoleDataEx* Role = Obj->Role();
	GameObjDbStructEx* ItemObjDB = GetObjDB( OrgObjID );
	if( ItemObjDB == NULL )
	{
		S_AccountMoneyShopResult( Role->GUID() , OrgObjID , EM_AccountMoneyShopResult_Failed );
		return;
	}

	if( ItemObjDB->PricesType != EM_ACPriceType_AccountMoney )
	{
		S_AccountMoneyShopResult( Role->GUID() , OrgObjID , EM_AccountMoneyShopResult_Failed );
		return;
	}
	
	if( ItemObjDB->Cost <= 0 || ItemObjDB->Cost != Money_Account  )
	{
		S_AccountMoneyShopResult( Role->GUID() , OrgObjID , EM_AccountMoneyShopResult_Failed );
		return;
	}

	if( Role->PlayerBaseData->Body.Money_Account < Money_Account )
	{
		S_AccountMoneyShopResult( Role->GUID() , OrgObjID , EM_AccountMoneyShopResult_MoneyNotEnough );
		return;
	}

	if( Role->PlayerBaseData->ItemTemp.Size() >= 1 )
	{
		S_AccountMoneyShopResult( Role->GUID() , OrgObjID , EM_AccountMoneyShopResult_PocketFull );
		return;
	}

	//扣錢給東西
	Role->AddBodyMoney_Account( ItemObjDB->Cost * -1 , "Sys Shop" , "Buy Record" , EM_ActionType_Buy_Account );
	Role->GiveItem( OrgObjID , ItemObjDB->BuyUnit , EM_ActionType_Buy , "Sys Shop" , "Buy" );

	S_AccountMoneyShopResult( Role->GUID() , OrgObjID , EM_AccountMoneyShopResult_OK );
	
}*/
/*
void NetSrv_ShopChild::R_AccountMoneyShopBuyList	( BaseItemObject* Obj , int TotalCost , AccountMoneyBuyStruct BuyList[20] )
{
	
	vector<AccountShopInfoStruct>&	AccountShop = g_ObjectData->AccountShop();
	GameObjDbStructEx* ObjDB;

	RoleDataEx* Owner = Obj->Role();

	if( Owner->TempData.Sys.OnChangeZone != false )
		return;


	if( Owner->BaseData.SysFlag.DisableTrade != false )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
		S_AccountMoneyShopBuyListResult( Owner->GUID() , EM_AccountMoneyShopResult_Failed );
		return;
	}

	if( Owner->PlayerBaseData->Body.Money_Account < TotalCost )
	{
		S_AccountMoneyShopBuyListResult( Owner->GUID() , EM_AccountMoneyShopResult_MoneyNotEnough );
		return;
	}

	int	CheckTotalCost = 0;

	//////////////////////////////////////////////////////////////////////////
	// 檢查合理性
	//////////////////////////////////////////////////////////////////////////
	for( int i = 0 ; i < 20 ; i++)
	{
		if( BuyList[i].SellID == -1 )
			break;

		if( AccountShop.size() <= (unsigned) BuyList[i].SellID )
		{
			S_AccountMoneyShopBuyListResult( Owner->GUID() , EM_AccountMoneyShopResult_Failed );
			return;
		}
		AccountShopInfoStruct& Temp = AccountShop[ BuyList[i].SellID ];

		ObjDB = Global::GetObjDB( Temp.ItemObjID );

		//找不到物品(TABLE填錯)
		if( ObjDB == NULL )
		{
			S_AccountMoneyShopBuyListResult( Owner->GUID() , EM_AccountMoneyShopResult_Failed );
			return;
		}
		//物品數量過多(TABLE填錯)
		if( ObjDB->MaxHeap < BuyList[i].Count )	
		{
			S_AccountMoneyShopBuyListResult( Owner->GUID() , EM_AccountMoneyShopResult_Failed );
			return;
		}
		//檢查身體欄位是否為空的
		ItemFieldStruct* EmptyGrid = Owner->GetBodyItem( BuyList[i].Pos );
		if( EmptyGrid == NULL || EmptyGrid->IsEmpty() == false )
		{
			S_AccountMoneyShopBuyListResult( Owner->GUID() , EM_AccountMoneyShopResult_PocketPosErr );
			return;
		}

		CheckTotalCost += Temp.Cost * BuyList[i].Count / Temp.Count;
	}

	if( CheckTotalCost != TotalCost )
	{
		S_AccountMoneyShopBuyListResult( Owner->GUID() , EM_AccountMoneyShopResult_Failed );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	//給予物品 
	//////////////////////////////////////////////////////////////////////////
	for( int i = 0 ; i < 20 ; i++)
	{
		if( BuyList[i].SellID == -1 )
			break;

		AccountShopInfoStruct& Temp = AccountShop[ BuyList[i].SellID ];

		ObjDB = Global::GetObjDB( Temp.ItemObjID );

		ItemFieldStruct	Item;
		Global::NewItemInit( Item , ObjDB->GUID , 0 );

		Item.Count		= BuyList[i].Count;
		Item.OrgObjID	= Temp.ItemObjID;
		Item.Quality	= 100;
		Item.OrgQuality	= 100;
		Item.PowerQuality = 10;
		Item.RuneVolume = 0;
		Item.Durable	= ObjDB->Item.Durable * Item.Quality;


		ItemFieldStruct* EmptyGrid = Owner->GetBodyItem( BuyList[i].Pos );
		*EmptyGrid = Item;

		Owner->Net_FixItemInfo_Body( BuyList[i].Pos );
		Owner->Log_ItemTrade( NULL, EM_ActionType_Buy_Account , Item );
		
	}
	//扣錢給東西
	Owner->AddBodyMoney_Account( CheckTotalCost * -1 , NULL  , EM_ActionType_Buy_Account );


	S_AccountMoneyShopBuyListResult( Owner->GUID() , EM_AccountMoneyShopResult_OK );
	//////////////////////////////////////////////////////////////////////////
}
*/