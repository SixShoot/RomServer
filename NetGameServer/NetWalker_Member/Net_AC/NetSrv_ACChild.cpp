#include "../NetWakerGSrvInc.h"
#include "NetSrv_ACChild.h"
#include <string.h>
#include "MD5/Mymd5.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_ACChild::Init()
{
    NetSrv_AC::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_ACChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_ACChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_AC::_Release();

    delete This;
    This = NULL;

    return true;
    
}


void NetSrv_ACChild::RC_SearchItem			( BaseItemObject* Obj , ACSearchConditionStruct& Data )
{
	RoleDataEx *Owner = Obj->Role();
	
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_AC) == false )
	{
		//Owner->Msg( "AC還沒開啟" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Shop_ACNoOpen );
		return;
	}

	if( Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_ACSerch ] > 2 )
		return;
	Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_ACSerch ] += (1.0f/10.0f);


	SDC_SearchItem( Owner->DBID() , Data );
}
void NetSrv_ACChild::RC_GetNextSearchItem	( BaseItemObject* Obj )
{
	RoleDataEx *Owner = Obj->Role();

	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_AC) == false )
	{
		//Owner->Msg( "AC還沒開啟" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Shop_ACNoOpen );
		return;
	}

	SDC_GetNextSearchItem( Obj->Role()->DBID() );
}
void NetSrv_ACChild::RC_GetMyACItemInfo		( BaseItemObject* Obj )
{

}
void NetSrv_ACChild::RC_BuyItem				( BaseItemObject* Obj , int ACItemDBID , int Prices , PriceTypeENUM PricesType , const char* Password )
{
	RoleDataEx *Owner = Obj->Role();

	if( Owner->TempData.Sys.OnChangeZone != false )
	{
		return;
	}

	if( PricesType == EM_ACPriceType_AccountMoney )
	{
		MyMD5Class Md5;
		Md5.ComputeStringHash( Password );
		if( stricmp( Md5.GetMd5Str() , Owner->PlayerBaseData->Password.Begin() ) != 0 )
		{
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_SecondPasswordError );
			SC_BuyItemResult( Owner->GUID() , ACItemDBID , Prices , PricesType , EM_ACItemState_Normal , false );
			return;
		}
	}

	if( Owner->BaseData.SysFlag.DisableTrade != false )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
		SC_BuyItemResult( Owner->GUID() , ACItemDBID , Prices , PricesType , EM_ACItemState_Normal , false );
		return;
	}

	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_AC) == false || Owner->BaseData.SysFlag.DisableTrade != false )
	{
		//Owner->Msg( "AC還沒開啟" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Shop_ACNoOpen );
		return;
	}
	switch( PricesType )
	{
	case EM_ACPriceType_GameMoney:
		{
			if( Prices < 0 || Prices > Owner->BodyMoney() )
			{
				//購買失敗
				SC_BuyItemResult( Owner->GUID() , ACItemDBID , Prices , PricesType , EM_ACItemState_Normal , false );
				return;
			}

			bool Ret = Owner->AddBodyMoney( Prices*-1 , NULL , EM_ActionType_ACBuy , false );
			if( Ret == false )
			{
				//購買失敗
				SC_BuyItemResult( Owner->GUID() , ACItemDBID , Prices , PricesType , EM_ACItemState_Normal , false );
				return;
			}
		}
		break;
	case EM_ACPriceType_AccountMoney:
		{
			if( Prices < 0 || Prices > Owner->BodyMoney_Account() )
			{
				//購買失敗
				SC_BuyItemResult( Owner->GUID() , ACItemDBID , Prices , PricesType , EM_ACItemState_Normal , false );
				return;
			}

			if( Prices > Owner->OkAccountMoney() )
			{
				SC_BuyItemResult( Owner->GUID() , ACItemDBID , Prices , PricesType , EM_ACItemState_Normal , false );
				NetSrv_Other::SC_AccountMoneyTranError( Owner->GUID() );		
				return;
			}


			bool Ret = Owner->AddBodyMoney_Account( Prices*-1 , NULL , EM_ActionType_ACBuy , false );
			if( Ret == false )
			{
				//購買失敗
				SC_BuyItemResult( Owner->GUID() , ACItemDBID , Prices , PricesType , EM_ACItemState_Normal , false );
				return;
			}

		}
		break;
	case EM_ACPriceType_BonusMoney:
		{
			if( Prices < 0 || Prices > Owner->PlayerBaseData->Money_Bonus )
			{
				//購買失敗
				SC_BuyItemResult( Owner->GUID() , ACItemDBID , Prices , PricesType , EM_ACItemState_Normal , false );
				return;
			}

			bool Ret = Owner->AddMoney_Bonus( Prices*-1 , NULL , EM_ActionType_ACBuy );
			if( Ret == false )
			{
				//購買失敗
				SC_BuyItemResult( Owner->GUID() , ACItemDBID , Prices , PricesType , EM_ACItemState_Normal , false );
				return;
			}
		}
		break;

	default:
		SC_BuyItemResult( Owner->GUID() , ACItemDBID , Prices , PricesType , EM_ACItemState_Normal , false );
		return;
		break;
	}



	//通知dc
	SDC_BuyItem( Owner->DBID() , ACItemDBID , Prices , PricesType );

}
void NetSrv_ACChild::RC_SellMoney		(BaseItemObject* Obj , PriceTypeENUM SellType , int SellMoney, int BuyOutPrices , int Prices , PriceTypeENUM PricesType , int LiveTime , char* Password )
{
	RoleDataEx *Owner = Obj->Role();

	if( Global::Ini()->AC_AccountMoneyTrade == false )
	{
		SC_SellMoneyResult( Owner->GUID() , EM_SellMoneyResult_MoneyTypeErr );
		return;
	}

	if( Owner->TempData.Sys.OnChangeZone != false )
		return;

	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_AC) == false )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Shop_ACNoOpen );
		return;
	}

	if(Owner->BaseData.SysFlag.DisableTrade != false 	)
	{
		SC_SellMoneyResult( Owner->GUID() , EM_SellMoneyResult_Err );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );

		return;
	}

	if( SellType == EM_ACPriceType_AccountMoney )
	{
		MyMD5Class Md5;
		Md5.ComputeStringHash( Password );
		if( stricmp( Md5.GetMd5Str() , Owner->PlayerBaseData->Password.Begin() ) != 0 )
		{
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_SecondPasswordError );
			SC_SellMoneyResult( Owner->GUID() , EM_SellMoneyResult_PasswordErr );
			return;
		}

	}

	
	if( SellType == PricesType )
	{
		SC_SellMoneyResult( Owner->GUID() , EM_SellMoneyResult_MoneyTypeErr );
		return;
	}
	
	ItemFieldStruct MoneyItem;
	MoneyItem.Init();
	MoneyItem.Count = 1;
	MoneyItem.Money = SellMoney;

	switch( SellType )
	{
	case EM_ACPriceType_AccountMoney:
		//MoneyItem.OrgObjID = g_ObjectData->GetSysKeyValue( "Money_AccountObjID" );
		MoneyItem.OrgObjID = g_ObjectData->SysValue().Money_AccountObjID;
		if( Owner->BodyMoney_Account() < SellMoney || SellMoney < 0  )
		{
			SC_SellMoneyResult( Owner->GUID() , EM_SellMoneyResult_MoneyErr );
			return;
		}

		if( SellMoney > Owner->OkAccountMoney() )
		{
			SC_SellMoneyResult( Owner->GUID() , EM_SellMoneyResult_MoneyErr );
			NetSrv_Other::SC_AccountMoneyTranError( Owner->GUID() );		
			return;
		}


		Owner->AddBodyMoney_Account( SellMoney * -1 , NULL , EM_ActionType_ToAC , false );
		break;

	case EM_ACPriceType_GameMoney:
		//MoneyItem.OrgObjID = g_ObjectData->GetSysKeyValue( "Money_GoldObjID" );
		MoneyItem.OrgObjID = g_ObjectData->SysValue().Money_GoldObjID;
		if( Owner->BodyMoney() < SellMoney || SellMoney < 0  )
		{
			SC_SellMoneyResult( Owner->GUID() , EM_SellMoneyResult_MoneyErr );
			return;
		}
		Owner->AddBodyMoney( SellMoney * -1 , NULL , EM_ActionType_ToAC , false );
		break;
	default:
		Owner->Msg( "AC 販賣金錢類型有問題" );
		SC_SellMoneyResult( Owner->GUID() , EM_SellMoneyResult_MoneyTypeErr );
		return;
	}

	//取得身上物品	
	if( BuyOutPrices == -1 )
		BuyOutPrices = _MAX_MONEY_COUNT_ +  1;


	//通知DC(暫時測試)
	GameObjDbStructEx*	OrgDB;
	OrgDB = Global::GetObjDB( MoneyItem.OrgObjID );

	if( OrgDB->IsItem() == false || OrgDB->Item.ItemType != EM_ItemType_Money )
	{
		SC_SellMoneyResult( Owner->GUID() , EM_SellMoneyResult_Err );
		return;
	}

	SDC_SellItem( Owner->DBID() , MoneyItem , Prices , BuyOutPrices , PricesType , LiveTime , 0 );
}

void NetSrv_ACChild::RC_SellItem			( BaseItemObject* Obj , int BodyPos , ItemFieldStruct& Item , int BuyOutPrices , int Prices , PriceTypeENUM PricesType , int LiveTime )
{
	RoleDataEx *Owner = Obj->Role();

	if( Owner->TempData.Sys.OnChangeZone != false )
		return;


	if(		Owner->BaseData.SysFlag.DisableTrade != false 	)
	{
		ACItemInfoStruct ACItem;
		ACItem.BuyOutPrices = BuyOutPrices;
		ACItem.DBID = -1;
		ACItem.Item = Item;
		ACItem.LiveTime = -1;
		ACItem.NowPrices = Prices;
		ACItem.Type	= PricesType;
		SC_SellItemResult( Owner->GUID() , ACItem , EM_ACSellItemResult_DisableTrade );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
		
		return;
	}

	if(	(  /*Global::Ini()->AC_AccountMoneyTrade == false 
		||*/ Global::Ini()->AC_Item_AccountMoneyTrade == false )

		&&	PricesType == EM_ACPriceType_AccountMoney)
	{
		ACItemInfoStruct ACItem;
		ACItem.BuyOutPrices = BuyOutPrices;
		ACItem.DBID = -1;
		ACItem.Item = Item;
		ACItem.LiveTime = -1;
		ACItem.NowPrices = Prices;
		ACItem.Type	= PricesType;
		SC_SellItemResult( Owner->GUID() , ACItem , EM_ACSellItemResult_DataError );		
		return;
	}



	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_AC) == false )
	{
		//Owner->Msg( "AC還沒開啟" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Shop_ACNoOpen );
		return;
	}
	switch( PricesType )
	{
	case EM_ACPriceType_GameMoney:
		break;
	case EM_ACPriceType_AccountMoney:
		break;
	default:
		Owner->Msg( "AC 販賣金錢類型有問題" );
		return;
		break;
	}

	//取得身上物品	
	if( BuyOutPrices == -1 )
		BuyOutPrices = _MAX_MONEY_COUNT_ +  1;

	ItemFieldStruct* BodyItem = Owner->GetBodyItem(BodyPos);
	if(		BodyItem == NULL 
		||	*BodyItem != Item 
		||	BodyItem->Pos != EM_ItemState_NONE 
		||  BuyOutPrices < Prices 
		||  BodyItem->Mode.Trade == false 
		||	BodyItem->Mode.ItemLock != false )
	{
		ACItemInfoStruct ACItem;
		ACItem.BuyOutPrices = BuyOutPrices;
		ACItem.DBID = -1;
		ACItem.Item = Item;
		ACItem.LiveTime = -1;
		ACItem.NowPrices = Prices;
		ACItem.Type	= PricesType;
		SC_SellItemResult( Owner->GUID() , ACItem , EM_ACSellItemResult_DataError );
		return;
	}

	//檢查是否下線消失或換區消失
	GameObjDbStructEx* ItemObjDB = g_ObjectData->GetObj( BodyItem->OrgObjID );
	if( ItemObjDB != NULL )
	{
		if(		ItemObjDB->Mode.Logoutdel  
			||	ItemObjDB->Mode.ChangeZoneDel  )
		{
			ACItemInfoStruct ACItem;
			ACItem.BuyOutPrices = BuyOutPrices;
			ACItem.DBID = -1;
			ACItem.Item = Item;
			ACItem.LiveTime = -1;
			ACItem.NowPrices = Prices;
			ACItem.Type	= PricesType;
			SC_SellItemResult( Owner->GUID() , ACItem , EM_ACSellItemResult_DataError );
			return;
		}
	}
	
	
	//通知DC(暫時測試)
	//int		TaxMoney = 10;
	GameObjDbStructEx*	OrgDB;
	OrgDB = Global::GetObjDB( Item.OrgObjID );

	if( OrgDB == NULL  )
	{
		ACItemInfoStruct ACItem;
		ACItem.BuyOutPrices = BuyOutPrices;
		ACItem.DBID = -1;
		ACItem.Item = Item;
		ACItem.LiveTime = -1;
		ACItem.NowPrices = Prices;
		ACItem.Type	= PricesType;
		SC_SellItemResult( Owner->GUID() , ACItem , EM_ACSellItemResult_DataError );
		return;
	}
/*
	//檢查物品是否修理OK
	if( OrgDB->MaxHeap == 1)
	{
		if( OrgDB->IsArmor() || OrgDB->IsWeapon() )
		{
			if( Item.Durable !=  OrgDB->Item.Durable * Item.Quality )
			{
				ACItemInfoStruct ACItem;
				ACItem.BuyOutPrices = BuyOutPrices;
				ACItem.DBID = -1;
				ACItem.Item = Item;
				ACItem.LiveTime = -1;
				ACItem.NowPrices = Prices;
				ACItem.Type	= PricesType;
				SC_SellItemResult( Owner->GUID() , ACItem , EM_ACSellItemResult_EqFixError );
				return;
			}
		}
	}
*/
	//擺設稅率 4小時	6% × 1/10物品價格
	int	TaxMoney = int ( float( OrgDB->Cost ) * Item.Count * float( LiveTime / 240 )*0.006  );
	if( TaxMoney <= 0 )
		TaxMoney = 0;

	//物品與金錢先扣除
	if( Owner->BodyMoney() < TaxMoney )
	{
		ACItemInfoStruct ACItem;
		ACItem.BuyOutPrices = BuyOutPrices;
		ACItem.DBID = -1;
		ACItem.Item = Item;
//		ACItem.Item.Coldown = 0;
		ACItem.LiveTime = -1;
		ACItem.NowPrices = Prices;
		ACItem.Type	= PricesType;
		SC_SellItemResult( Owner->GUID() , ACItem , EM_ACSellItemResult_MoneyError );
		return;
	}

	Owner->AddBodyMoney( TaxMoney * -1 , NULL , EM_ActionType_ACTex , true );
	SDC_SellItem( Owner->DBID() , *BodyItem , Prices , BuyOutPrices , PricesType , LiveTime , TaxMoney );

	Log_ItemDestroy( Owner , EM_ActionType_ToAC , Item , -1 , -1 , "" );
	BodyItem->Init();

	NetSrv_Item::FixItemInfo_Body( Owner->GUID() , *BodyItem , BodyPos );

}
void NetSrv_ACChild::RC_CancelSellItem		( BaseItemObject* Obj , int ACItemDBID )
{
	RoleDataEx *Owner = Obj->Role();

	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_AC) == false )
	{
		Owner->Msg( "AC還沒開啟" );
		return;
	}

	SDC_CancelSellItem( Obj->Role()->DBID() , ACItemDBID );
}
void NetSrv_ACChild::RC_CloseAC				( BaseItemObject* Obj )
{	
	RoleDataEx* Owner = Obj->Role();
	if( Owner->TempData.Attr.Action.OpenType == EM_RoleOpenMenuType_AC )
	{
		Owner->ClsOpenMenu();		
		//Owner->TempData.ShopInfo.TargetID = -1;
		//Owner->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_None;
	}
	
	SDC_CloseAC( Obj->Role()->DBID() );
}

void NetSrv_ACChild::RC_ItemHistoryRequest	( BaseItemObject* Obj , int OrgObjID , int Inherent )
{
	RoleDataEx *Owner = Obj->Role();

	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_AC) == false )
	{
		//Owner->Msg( "AC還沒開啟" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Shop_ACNoOpen );
		return;
	}

	SDC_ItemHistoryRequest( Obj->Role()->DBID() , OrgObjID , Inherent );
}

void NetSrv_ACChild::RDC_BuyItemResult		( bool Result , int BuyerDBID  , int ACItemDBID , int Prices , PriceTypeENUM PricesType , ACItemStateENUM State )
{
	
	BaseItemObject* Obj = BaseItemObject::GetObjByDBID( BuyerDBID );
	if( Obj == NULL )
		return;

	RoleDataEx *Owner = Obj->Role();

	if( Result == false )
	{
		switch( PricesType )
		{
		case EM_ACPriceType_GameMoney:
			Owner->AddBodyMoney( Prices , NULL , EM_ActionType_ToACError , false );
			break;
		case EM_ACPriceType_AccountMoney:
			Owner->AddBodyMoney_Account( Prices , NULL , EM_ActionType_ToACError , false );
			break;
		case EM_ACPriceType_BonusMoney:
			Owner->AddMoney_Bonus( Prices , NULL , EM_ActionType_ToACError );
			break;
		}
		
	}

	SC_BuyItemResult( Obj->GUID() , ACItemDBID , Prices , PricesType , State , Result );
}

void NetSrv_ACChild::RDC_SellItemResult		( ACSellItemResultENUM Result , int SellerDBID , ACItemInfoStruct& Item , int TaxMoney )
{

	BaseItemObject* Obj = BaseItemObject::GetObjByDBID( SellerDBID );
	if( Obj == NULL )
		return;

	RoleDataEx *Owner = Obj->Role();

	GameObjDbStructEx* ItemObj = Global::GetObjDB( Item.Item.OrgObjID );
	if( ItemObj->IsItem() && ItemObj->Item.ItemType == EM_ItemType_Money )
	{
//		int	MoneyObjID = g_ObjectData->GetSysKeyValue( "Money_GoldObjID" );
//		int	AccountObjID = g_ObjectData->GetSysKeyValue( "Money_AccountObjID" );

		switch( Result )
		{
		case EM_ACSellItemResult_OK:				//成
			SC_SellMoneyResult( Owner->GUID() , EM_SellMoneyResult_OK , &Item );
			break;
		case EM_ACSellItemResult_DataError:			//資料錯誤
		case EM_ACSellItemResult_MaxCountError:		//數量到達上限
			{
				if( g_ObjectData->SysValue().Money_AccountObjID == Item.Item.OrgObjID  )
					Owner->AddBodyMoney_Account( Item.Item.Money , NULL , EM_ActionType_ToACError , false );
				else if( g_ObjectData->SysValue().Money_BonusObjID == Item.Item.OrgObjID )
					Owner->AddMoney_Bonus( Item.Item.Money , NULL , EM_ActionType_ToACError );
				else
					Owner->AddBodyMoney( Item.Item.Money , NULL , EM_ActionType_ToACError , false );

				SC_SellMoneyResult( Owner->GUID(), EM_SellMoneyResult_Err );
			}
			break;
		}
	}
	else
	{
		switch( Result )
		{
		case EM_ACSellItemResult_OK:				//成
			SC_SellItemResult( Owner->GUID() , Item , EM_ACSellItemResult_OK );
			break;
		case EM_ACSellItemResult_DataError:			//資料錯誤
		case EM_ACSellItemResult_MaxCountError:		//數量到達上限
			{
				Owner->AddBodyMoney( TaxMoney , NULL , EM_ActionType_ToACError , false );
				Owner->GiveItem( Item.Item , EM_ActionType_ToACError , NULL , "" );
				SC_SellItemResult( Owner->GUID() , Item , Result );
			}
			break;
		}
	}

}

