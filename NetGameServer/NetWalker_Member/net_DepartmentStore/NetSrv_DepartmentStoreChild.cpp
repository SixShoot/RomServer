#include "../NetWakerGSrvInc.h"
#include "NetSrv_DepartmentStoreChild.h"
#include "../../mainproc/magicproc/MagicProcess.h"
#include "MD5/Mymd5.h"
//-----------------------------------------------------------------
int											NetSrv_DepartmentStoreChild::_MailGiftKey = 0;
map< int , MailGiftTempStruct >				NetSrv_DepartmentStoreChild::_MailGiftTempInfo;
int											NetSrv_DepartmentStoreChild::_ExchangeItemRequestKey = 0;
map< int , std::string >					NetSrv_DepartmentStoreChild::_ExchangeItemRequestTempInfo;
int											NetSrv_DepartmentStoreChild::_BuyItemResultKey = 0;
map< int , BuyItemResultTempStruct>			NetSrv_DepartmentStoreChild::_BuyItemResultTempInfo;
int											NetSrv_DepartmentStoreChild::_CheckMailGiftResultKey = 0;
map< int , CheckMailGiftResultTempStruct>	NetSrv_DepartmentStoreChild::_CheckMailGiftResultTempInfo;
SlotMachineBase								NetSrv_DepartmentStoreChild::_SlotMachineBaseInfo[_MAX_SLOT_MACHINE_PRIZES_COUNT_];
map< int , vector< SlotMachineTypeBase > >	NetSrv_DepartmentStoreChild::_SlotMachineTypeInfo;
map< int , BuyItemTaskQueueStruct >			NetSrv_DepartmentStoreChild::_BuyItemTaskQueue;
//-----------------------------------------------------------------
bool    NetSrv_DepartmentStoreChild::Init()
{
    NetSrv_DepartmentStore::_Init();

    if( This != NULL)
        return false;	

    This = NEW( NetSrv_DepartmentStoreChild );

	if (Global::St()->IsUseBillingServer == true)
	{
		Global::Schedular()->Push(_OnTimeProc_BuyItem, 1000, This, NULL, NULL);
	}

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_DepartmentStoreChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_DepartmentStore::_Release();

    delete This;
	This = NULL;

    return true;
    
}

void NetSrv_DepartmentStoreChild::RC_ShopFunctionReques	( BaseItemObject* Obj , int FunctionID )
{
	RoleDataEx* Owner = Obj->Role();

	vector<RentFunctionTableStruct>&	Rent = g_ObjectData->RentFunction();
	if( Rent.size() <= (unsigned)FunctionID )
	{
		//找不要資訊
		SC_ShopFunctionResult( Owner->GUID() , FunctionID , false );
		return;
	}

	int MemberType = Rent[FunctionID].MemberType;
	if( (unsigned) MemberType >= _MAX_RENT_MENBER_RANK_ )
	{
		SC_ShopFunctionResult( Owner->GUID() , FunctionID , false );
		return;
	}
	

	//檢查是否已有
	if( Owner->PlayerBaseData->RentFun.Member[ MemberType ] >= TimeStr::Now_Value() )
	{
		SC_ShopFunctionResult( Owner->GUID() , FunctionID , false );
		return;
	}


	//檢查金錢是否足夠
	if( Owner->PlayerBaseData->Body.Money_Account < Rent[ FunctionID ].Cost )
	{
		SC_ShopFunctionResult( Owner->GUID() , FunctionID , false );
		return;
	}

	//設定資料
	int TimeLimit = TimeStr::Now_Value() + int( Rent[ FunctionID ].RentDay * 24*60*60 );
	Owner->PlayerBaseData->RentFun.Member[ MemberType ] = TimeLimit;
	for( int i = 0 ; i < 20 ; i++ )
	{
		if( Rent[FunctionID].RentFunc[i] )
			Owner->PlayerBaseData->RentFun.Function[i] = TimeLimit;
	}

	SC_ShopFunctionResult( Owner->GUID() , FunctionID , true );

}
void NetSrv_DepartmentStoreChild::RC_SellTypeRequest		( BaseItemObject* Obj )
{
	//通知Datacenter
	SD_SellTypeRequest( Obj->Role()->TempData.Sys.SockID , Obj->Role()->TempData.Sys.ProxyID );
}
void NetSrv_DepartmentStoreChild::RC_ShopInfoRequest( BaseItemObject* Obj , int SellType )
{
	SD_ShopInfoRequest( Obj->Role()->TempData.Sys.SockID , Obj->Role()->TempData.Sys.ProxyID , SellType );
}
void NetSrv_DepartmentStoreChild::RC_BuyItem		( BaseItemObject* Obj , int ItemGUID , int Count , int Pos , char* Password )
{
	RoleDataEx* Owner = Obj->Role();
	//檢查二次密碼
	MyMD5Class Md5;
	Md5.ComputeStringHash( Password );
	if( stricmp( Md5.GetMd5Str() , Owner->PlayerBaseData->Password.Begin() ) != 0 )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_SecondPasswordError );
		SC_BuyItemResult( Owner->GUID() , EM_DepartmentStoreBuyItemResult_PasswordErr );
		return;
	}

	if (Global::St()->IsUseBillingServer == true)
	{
		if (Global::AccountMoneyOperable() == true)
		{
			BuyItemTempStruct Temp;
			Temp.ItemGUID = ItemGUID;
			Temp.Count	  = Count;
			Temp.Pos	  = Pos;

			_BuyItemTaskQueue[Owner->DBID()].Queue.push_back(Temp);

			Print(LV2, "RC_BuyItem", "Push task into queue, role=%s, dbid=%d, itemguid=%d, count=%d, pos=%d", Owner->RoleName(), Owner->DBID(), ItemGUID, Count, Pos);
		}
		else
		{
			SC_BuyItemResult( Owner->GUID() , EM_DepartmentStoreBuyItemResult_Err );

			Print(LV2, "RC_BuyItem", "Billing server unavailable, role=%s, dbid=%d, itemguid=%d, count=%d, pos=%d", Owner->RoleName(), Owner->DBID(), ItemGUID, Count, Pos);
		}
	}
	else
	{
		Print(LV2, "RC_BuyItem", "Normal mode");
		SD_BuyItem( ItemGUID , Owner->DBID() , Owner->PlayerBaseData->Body.Money_Account , Count , Pos );
	}
}

void NetSrv_DepartmentStoreChild::RD_BuyItemResult( BaseItemObject* Obj , DepartmentStoreBuyItemResultENUM Result , DepartmentStoreBaseInfoStruct& Item , int Count , int Pos )
{
	RoleDataEx* Owner = Obj->Role();

	if (Global::St()->IsUseBillingServer == true)
	{
		if (Global::AccountMoneyOperable() == true)
		{
			_BuyItemResultKey++;
			BuyItemResultTempStruct& Temp = _BuyItemResultTempInfo[_BuyItemResultKey];
			Temp.PlayerDBID = Owner->DBID();
			Temp.Result		= Result;
			Temp.Item		= Item;
			Temp.Count		= Count;
			Temp.Pos		= Pos;

			Net_BillingChild::SB_BalanceCheck(Owner->Account_ID(), Owner->DBID(), _BuyItemResultKey, &NetSrv_DepartmentStoreChild::BuyItemBalanceCheckResult);

			Print(LV2, "RD_BuyItemResult", "SB_BalanceCheck(account=%s, dbid=%d, customid=%d)", Owner->Account_ID(), Owner->DBID(), _BuyItemResultKey);
		}
		else
		{
			SC_BuyItemResult( Owner->GUID() , EM_DepartmentStoreBuyItemResult_Err );
			SD_BuyItemResult( Owner->DBID() , Item.GUID , false , 0 );

			Print(LV2, "RD_BuyItemResult", "Billing server unavailable, role=%s, dbid=%d, itemguid=%d, count=%d, pos=%d", Owner->RoleName(), Owner->DBID(), Item.GUID, Count, Pos);
		}
	}
	else
	{
		Print(LV2, "_RD_BuyItemResult", "Normal mode");
		_RD_BuyItemResult(Obj, Result, Item, Count, Pos);
	}
}

void NetSrv_DepartmentStoreChild::_RD_BuyItemResult( BaseItemObject* Obj , DepartmentStoreBuyItemResultENUM Result , DepartmentStoreBaseInfoStruct& Item , int Count , int Pos )
{
	int OrgCount = Count;
	RoleDataEx* Owner = Obj->Role();

	if( Result != EM_DepartmentStoreBuyItemResult_OK )
	{
		SC_BuyItemResult( Owner->GUID() , Result );
		return;
	}

	int AccountMoneyCost = Item.Sell.Cost * Count / Item.Item.Count;
	int BonusMoneyCost = ( Item.Sell.Cost_Bonus - Item.Sell.Get_Bonus ) * Count / Item.Item.Count;
	int	FreeMoneyCost = Item.Sell.Cost_Free * Count / Item.Item.Count;

	//檢查金錢
	if(		Owner->PlayerBaseData->Body.Money_Account < AccountMoneyCost 
		||	Owner->PlayerBaseData->Money_Bonus  < BonusMoneyCost 
		|| Owner->PlayerBaseData->Money_Bonus - BonusMoneyCost < 0	)
	{
		SC_BuyItemResult( Owner->GUID() , EM_DepartmentStoreBuyItemResult_MoneyErr );
		SD_BuyItemResult( Owner->DBID() , Item.GUID , false , 0 );
		return;
	}

	int FreeMoneyID = 0;
	if( FreeMoneyCost > 0 )
	{
		//取出金錢類型
		FreeMoneyID = g_ObjectData->GetSysKeyValue( "SYS_PRICETYPE_ICONID_03" ); 

		if( Owner->CalBodyItemCount(FreeMoneyID) < FreeMoneyCost  )
		{
			SC_BuyItemResult( Owner->GUID() , EM_DepartmentStoreBuyItemResult_MoneyErr );
			SD_BuyItemResult( Owner->DBID() , Item.GUID , false , 0 );
			return;
		}
	}

	ItemFieldStruct* BodyItem = Owner->GetBodyItem( Pos );

	if( BodyItem == NULL )
	{
		SC_BuyItemResult( Owner->GUID() , EM_DepartmentStoreBuyItemResult_PacketErr );
		SD_BuyItemResult( Owner->DBID() , Item.GUID , false , 0 );
		return;
	}

	if( BodyItem->IsEmpty() == false )
	{
		if( BodyItem->OrgObjID != Item.Item.OrgObjID )
		{
			SC_BuyItemResult( Owner->GUID() , EM_DepartmentStoreBuyItemResult_PacketErr );
			SD_BuyItemResult( Owner->DBID() , Item.GUID , false , 0 );
			return;
		}
		else
		{
			Count += BodyItem->Count;
		}

	}

	GameObjDbStructEx* OrgDB = g_ObjectData->GetObj( Item.Item.OrgObjID );
	if( OrgDB == NULL || OrgDB->MaxHeap < Count )
	{
		SC_BuyItemResult( Owner->GUID() , EM_DepartmentStoreBuyItemResult_PacketErr );
		SD_BuyItemResult( Owner->DBID() , Item.GUID , false , 0 );
		return;
	}

	*BodyItem = Item.Item;
	BodyItem->Count = Count;
	//BodyItem->Mode.DepartmentStore = true;

	BodyItem->CreateTime = RoleDataEx::G_Now;
	BodyItem->Serial	 = GenerateItemVersion( );

	//如果初始耐久值需為0
	if( OrgDB->Mode.DurableStart0 != false )
	{
		BodyItem->Durable = 0;
	}


	if( FreeMoneyCost > 0  )
	{
		Owner->DelBodyItem( FreeMoneyID , FreeMoneyCost , EM_ActionType_Buy_Account );
	}

	/////////////////////////////////////////////////////////////////////////
	if( Item.Gamble.Count > 0  )
	{
		int GetCount = 0;
		for( int i = 0 ; i < 8 ; i++ )
		{	
			int	ItemID = BodyItem->IntAbility[i]/0x100;
			int Count  = BodyItem->IntAbility[i]%0x100;

			if( Count == 0 )
				continue;

			if( Random( 10000 ) < Item.Gamble.Rate[i] )
			{
				GetCount++;
				if( GetCount >= Item.Gamble.Count )
				{
					for( int j = i+1 ; j < 8 ; j++ )
						BodyItem->IntAbility[j] &= 0xffffff00;
					break;
				}
			}
			else
			{
				BodyItem->IntAbility[i] &= 0xffffff00;
			}
		}
		if( GetCount == 0 )
		{
			BodyItem->IntAbility[0] = Item.Item.IntAbility[0];
		}
	}
	/////////////////////////////////////////////////////////////////////////

	Owner->AddBodyMoney_Account( AccountMoneyCost * -1 , NULL , EM_ActionType_Buy_Account , true , true );
	Owner->AddMoney_Bonus( BonusMoneyCost * -1 , NULL , EM_ActionType_Buy_Account );

	//寫商城Log
	Log_DepartmentStore( *BodyItem , EM_ActionType_Buy_Account , OrgCount , AccountMoneyCost , BonusMoneyCost , FreeMoneyCost , Owner->DBID() );

	Owner->Net_FixItemInfo_Body( Pos );
	if( AccountMoneyCost != 0 )
		Owner->Log_ItemTrade( NULL, EM_ActionType_Buy_Account , *BodyItem , "");
	else if( BonusMoneyCost != 0 )
		Owner->Log_ItemTrade( NULL, EM_ActionType_Buy_Account_Bonus , *BodyItem , "" );
	else
		Owner->Log_ItemTrade( NULL, EM_ActionType_Buy_Account_Free , *BodyItem , "" );

	SC_BuyItemResult( Owner->GUID() , EM_DepartmentStoreBuyItemResult_OK );
	SD_BuyItemResult( Owner->DBID() , Item.GUID , true , OrgCount );

	if (Global::St()->IsUseBillingServer == true)
	{
		BuyItemTaskQueueStruct& TaskQueue = _BuyItemTaskQueue[Owner->DBID()];
		if (TaskQueue.Queue.empty() == false)
		{			
			TaskQueue.Queue.pop_front();
			TaskQueue.Status = EM_BuyItemTaskQueueStatus_StandBy;
			Print(LV2, "_RD_BuyItemResult", "dbid=%d, task queue pop front, size=%d", Owner->DBID(), TaskQueue.Queue.size());
		}
	}
}

void NetSrv_DepartmentStoreChild::RC_ImportAccountMoney	( BaseItemObject* Obj )
{
	RoleDataEx* Owner = Obj->Role();

	if( Owner->TempData.Sys.OnChangeZone != false )
		return;

	SD_ImportAccountMoney( Owner->DBID() , Owner->Account_ID() );
	Owner->TempData.IsDisabledChangeZone = true;
}

void NetSrv_DepartmentStoreChild::RD_ImportAccountMoneyResult( int PlayerDBID , const char* Account_ID , int Money_Account , int Money_Bonus , int LockDay , int ImportGUID )
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObjByDBID( PlayerDBID );

	if(		OwnerObj == NULL 
		||	OwnerObj->Role()->IsDestroy() 
		||	OwnerObj->Role()->TempData.Sys.OnChangeZone != false )
	{
		Print( LV3 , "RD_ImportAccountMoneyResult" , "Failed : PlayerDBID =%d Account=%s Money_Account=%d ImportGUID=%d" ,  PlayerDBID , Account_ID , Money_Account , ImportGUID );
		SD_ImportAccountMoneyFailed( PlayerDBID , Account_ID , Money_Account , Money_Bonus , ImportGUID );
		return;
	}

	Print( LV3 , "RD_ImportAccountMoneyResult" , "Success : PlayerDBID =%d Account=%s Money_Account=%d ImportGUID=%d" ,  PlayerDBID , Account_ID , Money_Account , ImportGUID );

	RoleDataEx* Owner = OwnerObj->Role();

	Owner->TempData.IsDisabledChangeZone = false;
	Owner->AddBodyMoney_Account( Money_Account , NULL , EM_ActionType_ImportAccountMoney , true );
	Owner->AddMoney_Bonus( Money_Bonus , NULL , EM_ActionType_ImportAccountMoney );

	if (LockDay > 0)
	{
		Owner->AddLockAccountMoney(Money_Account, LockDay);
	}

//	LockDay -= 1;
//	if( LockDay >= 0 )
//	{
//		if( LockDay >= Def_AccountMoneyLockTime_Max_Count_ )
//		{
////			LockDay = Def_AccountMoneyLockTime_Max_Count_ -1 ;
//			Owner->PlayerBaseData->LockAccountMoney_Forever += Money_Account;
//			Owner->TempData.UpdateInfo.AccountMoneyLockInfoChange = true;
//			Owner->ResetLockAccountMoney();
//		}
//		else
//		{
//			Owner->PlayerBaseData->LockAccountMoney[ LockDay ] += Money_Account;
//			Owner->TempData.UpdateInfo.AccountMoneyLockInfoChange = true;
//			Owner->ResetLockAccountMoney();
//		}
//	}

	SC_ImportAccountMoneyResult	( Owner->GUID()  , Money_Account , Money_Bonus );
}

void NetSrv_DepartmentStoreChild::RC_ExchangeItemRequest	( BaseItemObject* Obj , const char* ItemSerial , const char* Password )
{
	if (Obj == NULL)
		return;

	RoleDataEx* Owner = Obj->Role();

	if (Owner == NULL)
		return;

	_ExchangeItemRequestKey++;
	_ExchangeItemRequestTempInfo[_ExchangeItemRequestKey] = ItemSerial;

	SD_ExchangeItemRequest( Owner->DBID() , Owner->Account_ID(), ItemSerial , Password, _ExchangeItemRequestKey);
}

void NetSrv_DepartmentStoreChild::RD_ExchangeItemResult	( int PlayerDBID , ItemFieldStruct& Item , int Money , int MoneyAccount , int ExchangeItemDBID , ItemExchangeResultENUM Result, int LockDay, int MapKey )
{
	string ItemSerial = _ExchangeItemRequestTempInfo[MapKey];
	_ExchangeItemRequestTempInfo.erase(MapKey);

	BaseItemObject*	Obj =	BaseItemObject::GetObjByDBID( PlayerDBID );

	if( Obj == NULL || Obj->Role()->PlayerBaseData->ItemTemp.Size() >= 5  )
	{		
		SC_ExchangeItemResult( PlayerDBID , Item , Money , MoneyAccount , EM_ItemExchangeResult_ItemBufErr );
		if( Result == EM_ItemExchangeResult_OK)
			SD_ExchangeItemResultOK( PlayerDBID , ExchangeItemDBID , false , false ) ;
		return;
	}

	RoleDataEx* Role = Obj->Role();

	if( Result == EM_ItemExchangeResult_OK )
	{
		if( Item.OrgObjID != 0 )
		{
			int Count	= Item.Count;
			int OrgID	= Item.OrgObjID;
			GameObjDbStructEx* OrgDB = GetObjDB( Item.OrgObjID );
			if( OrgDB == NULL || Count > OrgDB->MaxHeap || Count < 1 )
			{
				SC_ExchangeItemResult( PlayerDBID , Item , Money , MoneyAccount , EM_ItemExchangeResult_DataError );
				SD_ExchangeItemResultOK( PlayerDBID , ExchangeItemDBID , false , true ) ;
				return;
			}
			if( Global::NewItemInit( Item , OrgID , 0 ) == false )
			{
				SC_ExchangeItemResult( PlayerDBID , Item , Money , MoneyAccount , EM_ItemExchangeResult_DataError );
				SD_ExchangeItemResultOK( PlayerDBID , ExchangeItemDBID , false , true ) ;
				return;
			}
			Item.Count	  = Count;
			Item.OrgObjID = OrgID;
		}

		if (MoneyAccount > 0)
		{
			if (Global::AccountMoneyOperable() == false)
			{
				SC_ExchangeItemResult( PlayerDBID , Item , Money , MoneyAccount , EM_ItemExchangeResult_ItemBufErr );
				SD_ExchangeItemResultOK( PlayerDBID , ExchangeItemDBID , false , false ) ;
				return;
			}
		}

		/*
		GameObjDbStructEx* OrgDB = GetObjDB( Item.OrgObjID );
		if( OrgDB == NULL )
			return;

		Item.CreateTime = RoleDataEx::G_Now;
		Item.Serial	 = GenerateItemVersion( );
		Item.OrgQuality = 100;
		Item.Quality = 100;
		Item.PowerQuality = 10;

		//如果初始耐久值需為0
		if( OrgDB->Mode.DurableStart0 != false )
			Item.Durable = 0;
		else
			Item.Durable = OrgDB->Item.Durable * Item.Quality;
*/
		if( Money > 0 )
			Role->AddBodyMoney( Money , NULL , EM_ActionType_ItemExchange , true );

		if( MoneyAccount > 0 )
		{
			Role->AddBodyMoney_Account( MoneyAccount , NULL , EM_ActionType_ItemExchange , true );

			if (LockDay > 0)
			{
				Role->AddLockAccountMoney(MoneyAccount, LockDay);
			}
		}

		Role->GiveItem( Item , EM_ActionType_ItemExchange , NULL , "" );

		SD_ExchangeItemResultOK( PlayerDBID , ExchangeItemDBID , true , false );

		Log_ExchangeItem(ItemSerial.c_str(), Role, Item.OrgObjID, Item.Count, Money, MoneyAccount);
	}


	SC_ExchangeItemResult( PlayerDBID , Item , Money , MoneyAccount , Result );
}

void NetSrv_DepartmentStoreChild::RC_RunningMessageRequest( BaseItemObject* Obj )
{
	SD_RunningMessageRequest( Obj->Role()->DBID() );
}


void NetSrv_DepartmentStoreChild::RC_MailGift				( BaseItemObject* Obj , int ItemGUID , int Count , const char* Password , MailBackGroundTypeENUM BackGroundType , const char* TargetName , const char* Title , const char* Content )
{

	RoleDataEx* Owner = Obj->Role();

	if( Global::Ini()->IsDisableSendGift != false )
	{
		//Owner->Net_GameMsgEvent( EM_GameMessageEvent_DisableSendGift );
		SC_MailGiftResult( Owner->GUID() , EM_CheckMailGiftResultType_DisableSendGift );
		return;
	}

	if( Owner->TempData.Sys.OnChangeZone != false )
	{
		if( Owner->TempData.Sys.OnChangeWorld != false )
			SC_MailGiftResult( Owner->GUID() , EM_CheckMailGiftResultType_DisableSendGift );
		return;
	}

	//檢查二次密碼
	MyMD5Class Md5;
	Md5.ComputeStringHash( Password );
	if( stricmp( Md5.GetMd5Str() , Owner->PlayerBaseData->Password.Begin() ) != 0 )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_SecondPasswordError );
		SC_MailGiftResult( Owner->GUID() , EM_CheckMailGiftResultType_PasswordError );
		return;
	}
	if( stricmp( Owner->RoleName() , TargetName) == 0 )
	{
		SC_MailGiftResult( Owner->GUID() , EM_CheckMailGiftResultType_NotSendToSelf );
		return;
	}

	MailGiftTempStruct	Temp;
	Temp.PlayerGUID = Owner->GUID();
	Temp.PlayerDBID = Owner->DBID();
	Temp.ItemGUID = ItemGUID;
	Temp.Count = Count;
	Temp.BackGroundType = BackGroundType;
	Temp.TargetName = TargetName;
	Temp.Title = Title;
	Temp.Content = Content;

	int MapKey = _MailGiftKey++;

	_MailGiftTempInfo[MapKey] = Temp;

	SD_CheckMailGift( Owner->DBID() , ItemGUID , Count , TargetName , MapKey );
}
void NetSrv_DepartmentStoreChild::RD_CheckMailGiftResult	( int MapKey , int TargetDBID , DepartmentStoreBaseInfoStruct& Item , CheckMailGiftResultTypeENUM Result )
{
	if (Global::St()->IsUseBillingServer == true)
	{
		map< int , MailGiftTempStruct >::iterator it = _MailGiftTempInfo.find(MapKey);

		if (it != _MailGiftTempInfo.end())
		{
			MailGiftTempStruct& Temp = it->second;

			RoleDataEx* Owner = Global::GetRoleDataByGUID(Temp.PlayerGUID);

			if (Owner == NULL)
			{
				_MailGiftTempInfo.erase(MapKey);
				return;
			}

			if( Owner->DBID() != Temp.PlayerDBID )
			{
				_MailGiftTempInfo.erase(MapKey);
				return;
			}

			if (Global::AccountMoneyOperable() == true)
			{
				_CheckMailGiftResultKey++;
				CheckMailGiftResultTempStruct& Temp = _CheckMailGiftResultTempInfo[_CheckMailGiftResultKey];
				Temp.MapKey = MapKey;
				Temp.TargetDBID = TargetDBID;
				Temp.Item = Item;
				Temp.Result = Result;

				Net_BillingChild::SB_BalanceCheck(Owner->Account_ID(), Owner->DBID(), _CheckMailGiftResultKey, &NetSrv_DepartmentStoreChild::MailGiftBalanceCheckResult);
			}
			else
			{
				_MailGiftTempInfo.erase(MapKey);
				SC_MailGiftResult( Owner->GUID() , EM_CheckMailGiftResultType_DisableSendGift );
				return;
			}
		}
	}
	else
	{
		_RD_CheckMailGiftResult(MapKey, TargetDBID, Item, Result);
	}
}

void NetSrv_DepartmentStoreChild::_RD_CheckMailGiftResult( int MapKey , int TargetDBID , DepartmentStoreBaseInfoStruct& Item , CheckMailGiftResultTypeENUM Result )
{

	MailGiftTempStruct	Temp = _MailGiftTempInfo[MapKey];
	_MailGiftTempInfo.erase( MapKey );

	RoleDataEx* Owner = Global::GetRoleDataByGUID( Temp.PlayerGUID );
	if( Owner == NULL )
		return;
	if( Owner->DBID() != Temp.PlayerDBID )
		return;

	if( Result != EM_CheckMailGiftResultType_OK )
	{
		SC_MailGiftResult( Owner->GUID() , Result );
		return;
	}

	if( Item.Sell.Cost_Free > 0 )
	{
		SC_MailGiftResult( Owner->GUID() , EM_CheckMailGiftResultType_ItemError );
		return;
	}

	int AccountMoneyCost = Item.Sell.Cost * Temp.Count / Item.Item.Count;
	int BonusMoneyCost = ( Item.Sell.Cost_Bonus - Item.Sell.Get_Bonus ) * Temp.Count / Item.Item.Count;
	int	FreeMoneyCost = Item.Sell.Cost_Free * Temp.Count / Item.Item.Count;

	//檢查金錢
	if(		Owner->PlayerBaseData->Body.Money_Account < AccountMoneyCost 
		||	Owner->PlayerBaseData->Money_Bonus  < BonusMoneyCost 
		|| Owner->PlayerBaseData->Money_Bonus - BonusMoneyCost < 0	)
	{
		SC_MailGiftResult( Owner->GUID() , EM_CheckMailGiftResultType_MoneyError );
		SD_BuyItemResult( Owner->DBID() , Item.GUID , false , 0 );
		return;
	}

	if( AccountMoneyCost > Owner->OkAccountMoney() )
	{
		SC_MailGiftResult( Owner->GUID() , EM_CheckMailGiftResultType_MoneyError );
		SD_BuyItemResult( Owner->DBID() , Item.GUID , false , 0 );
		NetSrv_Other::SC_AccountMoneyTranError( Owner->GUID() );		
		return;
	}

	int FreeMoneyID = 0;
	if( FreeMoneyCost > 0 )
	{
		//取出金錢類型
		FreeMoneyID = g_ObjectData->GetSysKeyValue( "SYS_PRICETYPE_ICONID_03" ); 

		if( Owner->CalBodyItemCount(FreeMoneyID) < FreeMoneyCost  )
		{
			SC_MailGiftResult( Owner->GUID() , EM_CheckMailGiftResultType_MoneyError );
			SD_BuyItemResult( Owner->DBID() , Item.GUID , false , 0 );
			return;
		}
	}

	GameObjDbStructEx* OrgDB = g_ObjectData->GetObj( Item.Item.OrgObjID );
	if( OrgDB == NULL || OrgDB->MaxHeap < Temp.Count )
	{
		SC_MailGiftResult( Owner->GUID() , EM_CheckMailGiftResultType_DataError );
		SD_BuyItemResult( Owner->DBID() , Item.GUID , false , 0 );
		return;
	}

	ItemFieldStruct MailItem;
	MailItem = Item.Item;
	MailItem.Count = Temp.Count;
	//	MailItem.Mode.DepartmentStore = true;

	MailItem.CreateTime = RoleDataEx::G_Now;
	MailItem.Serial	 = GenerateItemVersion( );

	//如果初始耐久值需為0
	if( OrgDB->Mode.DurableStart0 != false )
	{
		MailItem.Durable = 0;
	}

	MailItem.Mode.DepartmentStore = OrgDB->Mode.DepartmentStore;

	if( FreeMoneyCost > 0  )
	{
		Owner->DelBodyItem( FreeMoneyID , FreeMoneyCost , EM_ActionType_Buy_Account );
	}

	if( Item.Gamble.Count > 0  )
	{
		int GetCount = 0;
		for( int i = 0 ; i < 8 ; i++ )
		{	
			int	ItemID = MailItem.IntAbility[i]/0x100;
			int Count  = MailItem.IntAbility[i]%0x100;

			if( Count == 0 )
				continue;

			if( Random( 10000 ) < Item.Gamble.Rate[i] )
			{
				GetCount++;
				if( GetCount >= Item.Gamble.Count )
				{
					for( int j = i+1 ; j < 8 ; j++ )
						MailItem.IntAbility[j] &= 0xffffff00;
					break;
				}
			}
			else
			{
				MailItem.IntAbility[i] &= 0xffffff00;
			}
		}
		if( GetCount == 0 )
		{
			MailItem.IntAbility[0] = Item.Item.IntAbility[0];
		}
	}

	Owner->AddBodyMoney_Account( AccountMoneyCost * -1 , NULL , EM_ActionType_Buy_Account_Gift , true );
	Owner->AddMoney_Bonus( BonusMoneyCost * -1 , NULL , EM_ActionType_Buy_Account_Gift );

	//寫商城Log
	Log_DepartmentStore( MailItem , EM_ActionType_Buy_Account_Gift , Temp.Count , AccountMoneyCost , BonusMoneyCost , FreeMoneyCost , Owner->DBID() );

	SC_MailGiftResult( Owner->GUID() , EM_CheckMailGiftResultType_OK );
	SD_BuyItemResult( Owner->DBID() , Item.GUID , true , Temp.Count );

	MailBaseInfoStruct MailBaseInfo;
	MailBaseInfo.Init();

	MailBaseInfo.Item[0]		= MailItem;			//資料簡查
	MailBaseInfo.IsSendMoney 	= true;				//true 寄送金錢 false 獲到付款
	MailBaseInfo.IsSystem	 	= true;				//是否為系統信件
	MailBaseInfo.IsGift			= true;
	MailBaseInfo.OrgSendName	= Owner->RoleName();
	MailBaseInfo.OrgTargetName	= Temp.TargetName.c_str();
	MailBaseInfo.Title			= Temp.Title.c_str();
	MailBaseInfo.LiveTime		= 60*24*30;			//生存時間( 單位(分鐘) )
	MailBaseInfo.PaperType		= Temp.BackGroundType;

	//把信件送出
	NetSrv_MailChild::SD_SendMail( Owner->DBID() , (char*)Temp.Content.c_str() , MailBaseInfo );
	Global::Log_ItemTrade( Owner->DBID() , -1 , TargetDBID , (int)Owner->X() , (int)Owner->Z() , EM_ActionType_Buy_Account_Gift , MailItem , "" );
}

void NetSrv_DepartmentStoreChild::RC_WebBagRequest			( BaseItemObject* Obj )
{
	RoleDataEx* Role = Obj->Role();

	if (Role == NULL)
		return;

	if (Role->TempData.Sys.WaitWebBagInfo == false)
	{
		SD_WebBagRequest( Obj->Role() );
	}	
}

void NetSrv_DepartmentStoreChild::RD_WebBagResult			( int PlayerDBID , int ItemGUID , ItemFieldStruct& Item , bool IsEnd , char* Message , int PriceType , int Price , int AddBonusMoney )
{
	BaseItemObject* Obj = Global::GetPlayerObj_ByDBID(PlayerDBID);

	if( Obj == NULL )
		return;

	RoleDataEx* Role = Obj->Role();

	if( Role == NULL )
		return;

	if( IsEnd == true )
	{
		if (Global::St()->IsUseBillingServer == true)
		{
			if (Global::AccountMoneyOperable() == true)
			{
				Net_BillingChild::SB_BalanceCheck(Role->Account_ID(), Role->DBID(), 0, &NetSrv_DepartmentStoreChild::WebBagBalanceCheckResult);
			}
			else
			{
				WebBagProc( Obj );
				Role->TempData.Sys.WaitWebBagInfo = false;
				Role->Net_FixMoney();
			}
		}
		else
		{
			WebBagProc( Obj );
			Role->TempData.Sys.WaitWebBagInfo = false;
			Role->Net_FixMoney();
		}
	}
	else
	{
		map< int , WebShopBagItemFieldDBStruct>& WebShopBag = *( Obj->WebShopBag() );
		WebShopBagItemFieldDBStruct& TempItem = WebShopBag[ItemGUID];
		TempItem.Item = Item;
		strncpy( TempItem.Message , Message , sizeof( TempItem.Message ) );
		TempItem.PriceType = PriceType;
		TempItem.Price = Price;
		TempItem.AddBonusMoney = AddBonusMoney;
	}
}

void NetSrv_DepartmentStoreChild::WebBagProc( BaseItemObject* Obj )
{
	map< int , WebShopBagItemFieldDBStruct >& WebShopBag = *( Obj->WebShopBag() );
	map< int , WebShopBagItemFieldDBStruct >::iterator Iter;

	RoleDataEx* Role = Obj->Role();

	if( Role->TempData.Sys.OnChangeZone != false )
		return;

	int PhiriusCoinID = g_ObjectData->GetSysKeyValue( "SYS_PRICETYPE_ICONID_03" );

	std::map<int, int> ItemResult;

	for( Iter = WebShopBag.begin() ; Iter != WebShopBag.end() ; Iter++)
	{
		ItemFieldStruct& Item = Iter->second.Item;
		int	ItemDBID = Iter->first;
		int PriceType = Iter->second.PriceType;
		int Price = Iter->second.Price;
		int BonusMoney = Iter->second.AddBonusMoney;
		std::string Message = Iter->second.Message;		
		
		//-----------------------------------------
		//檢查物品資料
		GameObjDbStructEx* ItemDB = Global::GetObjDB( Item.OrgObjID );
		if( ItemDB == NULL )
		{
			//物品資料錯誤
			ItemResult[ItemDBID] = 2;
			Role->Net_GameMsgEvent( EM_GameMessageEvent_Item_Queue_DataError );
			continue;
		}

		if (ItemDB->IsItem() == false)
		{
			//物品資料錯誤
			ItemResult[ItemDBID] = 2;
			Role->Net_GameMsgEvent( EM_GameMessageEvent_Item_Queue_DataError );
			continue;
		}
		//-----------------------------------------

		//-----------------------------------------
		//檢查售價及餘額
		if (Price < 0)
			Price = 0;

		bool PriceOK = true;

		switch(PriceType)
		{
		case 1:
			{
				if (Global::St()->IsUseBillingServer == true)
				{
					if (Global::AccountMoneyOperable() == false)
					{
						PriceOK = false;
						SC_WebBagResult( Role->GUID(), -4, 0 );
						ItemResult[ItemDBID] = 4;
					}
					else
					{
						PriceOK = (Role->PlayerBaseData->Body.Money_Account >= Price);

						if (PriceOK == false)
						{
							SC_WebBagResult( Role->GUID(), -1, 0 );
							ItemResult[ItemDBID] = 3;
						}
					}
				}
				else
				{
					PriceOK = (Role->PlayerBaseData->Body.Money_Account >= Price);

					if (PriceOK == false)
					{
						SC_WebBagResult( Role->GUID(), -1, 0 );
						ItemResult[ItemDBID] = 3;
					}
				}
			}
			break;
		case 2:
			{
				PriceOK = (Role->PlayerBaseData->Money_Bonus >= Price);

				if (PriceOK == false)
				{
					SC_WebBagResult( Role->GUID(), -2, 0 );
					ItemResult[ItemDBID] = 3;
				}
			}
			break;
		case 3:
			{
				PriceOK = (Role->CalBodyItemCount(PhiriusCoinID) >= Price);

				if (PriceOK == false)
				{
					SC_WebBagResult( Role->GUID(), -3, 0 );
					ItemResult[ItemDBID] = 3;
				}
			}
			break;
		}

		if (PriceOK == false)
			continue;
		//-----------------------------------------

		//-----------------------------------------
		bool BagFull = false;

		if (Item.Count > 0)
		{
			BagFull = true;

			for( int i = 0 ; i < _MAX_BODY_EX_COUNT_ ; i++ )
			{
				ItemFieldStruct& BodyItem = Role->PlayerBaseData->Body.Item[i];

				if( BodyItem.IsEmpty() == true)
				{
					BagFull = false;

					BodyItem = Item;
					Role->FixItem( &BodyItem );
					Role->Net_FixItemInfo_Body( i );

					//通知獲得物品
					SC_WebBagResult( Role->GUID() , BodyItem.OrgObjID, BodyItem.Count );

					break;
				}
			}
		}

		if (BagFull == false)
		{
			//通知client 訊息
			if( Message.empty() == false )
			{
				NetSrv_Talk::ScriptMessage( Role->GUID(), Role->GUID(), 0,  Message.c_str() , 0 );
			}

			//扣除金額
			if (Price > 0)
			{
				switch(PriceType)
				{
				case 1:
					Role->AddBodyMoney_Account( Price * -1 , NULL , EM_ActionType_Buy_Account , true , true );
					break;
				case 2:
					Role->AddMoney_Bonus( Price * -1 , NULL , EM_ActionType_Buy_Account );
					break;
				case 3:
					Role->DelBodyItem( PhiriusCoinID , Price , EM_ActionType_Buy_Account );
					break;
				}
			}

			//給予紅利幣
			if (BonusMoney > 0)
			{
				Role->AddMoney_Bonus( BonusMoney , NULL , EM_ActionType_Buy_Account );
			}

			//領取成
			ItemResult[ItemDBID] = 1;
		}
		else
		{
			//背包已滿
			Role->Net_GameMsgEvent( EM_GameMessageEvent_DepartmentBagFull );
			break;
		}
		//-----------------------------------------
	}

	char Buf[512];
	for (std::map<int, int>::iterator it = ItemResult.begin(); it != ItemResult.end(); it++)
	{
		sprintf( Buf , "UPDATE WebShopBag SET DestroyTime = getdate(), Status = %d WHERE GUID =%d" , it->second, it->first );
		Net_DCBase::SqlCommand_Import( Role->DBID() , Buf );
		WebShopBag.erase(it->first);
	}
}
/*
void NetSrv_DepartmentStoreChild::RL_BuyItemResult			( int FromWorldId , int FromNetID , int PlayerDBID , DepartmentStoreBuyItemResultENUM	Result , DepartmentStoreBaseInfoStruct&	Item , int Pos )
{


//	Role->PlayerTempData->ChangeWorld.WorldID = FromWorldId;
//	Role->PlayerTempData->ChangeWorld.WorldNetID = FromNetID;

}
void NetSrv_DepartmentStoreChild::RBL_BuyItemResult			( int FromWorldId , int FromNetID , int PlayerDBID , int ItemGUID , bool Result , int Count )
{

}
*/
//DataCenter -> Local 老虎機資料
void NetSrv_DepartmentStoreChild::RD_SlotMachineInfo( SlotMachineBase base[] , int typeInfoCount , SlotMachineTypeBase typeInfo[] )
{
	memcpy( _SlotMachineBaseInfo , base , sizeof(_SlotMachineBaseInfo) );

	_SlotMachineTypeInfo.clear();
	for( int i = 0 ; i < typeInfoCount ; i++ )
	{
		SlotMachineTypeBase info = typeInfo[i];
		_SlotMachineTypeInfo[info.Type].push_back( info );
	}
	SC_All_SlotMachineInfoChange();
}

bool NetSrv_DepartmentStoreChild::GetSlotMachineItemInfo( RoleDataEx* role , int itemID[] , int itemCount[] )
{
	SlotMachineInfo& slotMachine = role->PlayerBaseData->SlotMachine;
	for( int i = 0 ; i < _MAX_SLOT_MACHINE_PRIZES_COUNT_ ; i++ )
	{
		vector< SlotMachineTypeBase >& typeInfoList = _SlotMachineTypeInfo[ _SlotMachineBaseInfo[i].Type ];
		if( typeInfoList.size() == 0 )
		{
			Print( LV2 , "GetSlotMachineItemInfo" , "Data Setting Error!! type=%d info err" , _SlotMachineBaseInfo[i].Type );
			return false;
		}

		int randSeed = slotMachine.RandSeed[i];
		itemID[i]		= typeInfoList[0].ItemID;
		itemCount[i]	= typeInfoList[0].ItemCount;
		for( unsigned j = 0 ; j < typeInfoList.size() ; j++ )	
		{
			SlotMachineTypeBase& info = typeInfoList[j];
			if( info.Rate > randSeed )
			{
				itemID[i]		= info.ItemID;
				itemCount[i]	= info.ItemCount;
				break;
			}
			randSeed -= info.Rate;
		}
	}
	return true;
}
//Client -> Local 要求老虎機資料
void NetSrv_DepartmentStoreChild::RC_SlotMachineInfoRequest( BaseItemObject* obj  )
{
	RoleDataEx* role = obj->Role();

	SlotMachineInfo& slotMachine = role->PlayerBaseData->SlotMachine;
	if( slotMachine.RandSeed[0] == 0 )
	{
		for( int i = 0 ; i < _MAX_SLOT_MACHINE_PRIZES_COUNT_ ; i++ )
			slotMachine.RandSeed[i] = rand()%10000;
	}

	int itemID[_MAX_SLOT_MACHINE_PRIZES_COUNT_];
	int itemCount[_MAX_SLOT_MACHINE_PRIZES_COUNT_];
	if( GetSlotMachineItemInfo( role , itemID , itemCount ) == false )
		return;

	SC_SlotMachineInfo( role->GUID() , itemID , itemCount );
}
//Client -> Local 投幣
void NetSrv_DepartmentStoreChild::RC_SlotMachinePlay( BaseItemObject* obj  )
{
	RoleDataEx* role = obj->Role();
	
	//金錢檢查
	int cost = g_ObjectData->GetMoneyKeyValue( "SlotMachineCost" , 0 );
	if( cost <= 0 )
	{
		SC_SlotMachinePlayResultFailed( role->GUID() , EM_SlotMachinePlayResult_Failed );
		return;
	}

	if( role->PlayerBaseData->ItemTemp.Size() > 5 )
	{
		SC_SlotMachinePlayResultFailed( role->GUID() , EM_SlotMachinePlayResult_Failed );
		return;
	}

	SlotMachineInfo& slotMachine = role->PlayerBaseData->SlotMachine;
	if(		slotMachine.FreeCount == 0 
		&&	role->PlayerBaseData->Body.Money_Account < cost )
		//role->PlayerBaseData->Body.Money_Account < cost )
	{
		SC_SlotMachinePlayResultFailed( role->GUID() , EM_SlotMachinePlayResult_NoMoney );
		return;
	}

	if(slotMachine.FreeCount == 0)
	{
		if (Global::AccountMoneyOperable() == false)
		{
			SC_SlotMachinePlayResultFailed( role->GUID() , EM_SlotMachinePlayResult_Failed );
			return;
		}
	}

	int itemID[_MAX_SLOT_MACHINE_PRIZES_COUNT_];
	int itemCount[_MAX_SLOT_MACHINE_PRIZES_COUNT_];

	if( GetSlotMachineItemInfo( role , itemID , itemCount ) == false )
	{
		SC_SlotMachinePlayResultFailed( role->GUID() , EM_SlotMachinePlayResult_Failed );
		return;
	}
	
	//計算種哪個獎
	int randSeed = rand() % 10000;
	int prizeID = 0;

	for( int i = 0 ; i < _MAX_SLOT_MACHINE_PRIZES_COUNT_ ; i++ )
	{
		if( _SlotMachineBaseInfo[i].Rate > randSeed )
		{
			prizeID = i;
			break;
		}
		randSeed -= _SlotMachineBaseInfo[i].Rate;
	}
	
	if( slotMachine.FreeCount <= 0 )
	{
		if( role->AddBodyMoney_Account( cost * -1 , -1 , -1 , EM_ActionType_SlotMachine , true , true ) == false )
		{
			SC_SlotMachinePlayResultFailed( role->GUID() , EM_SlotMachinePlayResult_NoMoney );
			return;
		}
	}
	else
		role->AddValue( EM_RoleValue_SlotMachineFreeCount , -1 );
	
	//role->GiveItem( itemID[prizeID] , itemCount[prizeID] , EM_ActionType_SlotMachine , NULL , "" );
	slotMachine.ItemID		= itemID[prizeID];
	slotMachine.ItemCount	= itemCount[prizeID];

	for( int i = 0 ; i < _MAX_SLOT_MACHINE_PRIZES_COUNT_ ; i++ )
		slotMachine.RandSeed[i] = rand()%10000;

	SC_SlotMachinePlayResult( role->GUID() , EM_SlotMachinePlayResult_OK , itemID[prizeID] , itemCount[prizeID] );
}


void NetSrv_DepartmentStoreChild::SlotMachineFreeCountProc	( RoleDataEx* role )
{
	SlotMachineInfo& slotMachine = role->PlayerBaseData->SlotMachine;
	if( slotMachine.FreeCount >= g_ObjectData->GetMoneyKeyValue( "SlotMachineMaxFreeCount" , 0 ) )
		return;

	role->AddValue( EM_RoleValue_SlotMachineFreeCount , 1 );

}
void NetSrv_DepartmentStoreChild::RC_GetSlotMachineItem		( BaseItemObject* obj  )
{
	RoleDataEx* role = obj->Role();
	SlotMachineInfo& slotMachine = role->PlayerBaseData->SlotMachine;
	if( slotMachine.ItemID == 0 )
		return;
	role->GiveItem( slotMachine.ItemID , slotMachine.ItemCount , EM_ActionType_SlotMachine , NULL , "" );
	slotMachine.ItemID = 0; 
	slotMachine.ItemCount = 0;
}

void NetSrv_DepartmentStoreChild::BuyItemBalanceCheckResult(void* userdata)
{
	BillingResultData* ResultData = (BillingResultData*)userdata;

	std::map<int, BuyItemResultTempStruct>::iterator it = _BuyItemResultTempInfo.find(ResultData->CustomID);

	if (it != _BuyItemResultTempInfo.end())
	{
		BuyItemResultTempStruct& Temp = it->second;

		BaseItemObject* pObj = Global::GetPlayerObj_ByDBID(Temp.PlayerDBID);

		if (pObj != NULL)
		{
			Print(LV2, "BuyItemBalanceCheckResult", "_RD_BuyItemResult(dbid=%d, result=%d, itemguid=%d, count=%d, pos=%d)", Temp.PlayerDBID, Temp.Result, Temp.Item.GUID, Temp.Count, Temp.Pos);
			((NetSrv_DepartmentStoreChild*)This)->_RD_BuyItemResult(pObj, Temp.Result, Temp.Item, Temp.Count, Temp.Pos);
		}
		else
		{
			Print(LV2, "BuyItemBalanceCheckResult", "pObj==NULL, dbid=%d", Temp.PlayerDBID);
		}

		_BuyItemResultTempInfo.erase(it);
	}
	else
	{
		Print(LV2, "BuyItemBalanceCheckResult", "CustomID=%d not found", ResultData->CustomID);
	}
}

void NetSrv_DepartmentStoreChild::MailGiftBalanceCheckResult(void* userdata)
{
	BillingResultData* ResultData = (BillingResultData*)userdata;

	std::map<int, CheckMailGiftResultTempStruct>::iterator it = _CheckMailGiftResultTempInfo.find(ResultData->CustomID);

	if (it != _CheckMailGiftResultTempInfo.end())
	{
		CheckMailGiftResultTempStruct& Temp = it->second;

		((NetSrv_DepartmentStoreChild*)This)->_RD_CheckMailGiftResult(Temp.MapKey, Temp.TargetDBID, Temp.Item, Temp.Result);

		_CheckMailGiftResultTempInfo.erase(it);
	}
}

void NetSrv_DepartmentStoreChild::WebBagBalanceCheckResult(void* userdata)
{
	BillingResultData* ResultData = (BillingResultData*)userdata;

	BaseItemObject* Obj = Global::GetPlayerObj_ByDBID(ResultData->PlayerDBID);

	if( Obj == NULL )
		return;

	RoleDataEx* Role = Obj->Role();

	if( Role == NULL )
		return;

	WebBagProc( Obj );
	Role->TempData.Sys.WaitWebBagInfo = false;
}

int NetSrv_DepartmentStoreChild::_OnTimeProc_BuyItem(SchedularInfo* obj, void* inputclass)
{
	for (std::map<int, BuyItemTaskQueueStruct>::iterator it = _BuyItemTaskQueue.begin(); it != _BuyItemTaskQueue.end(); it++)
	{
		BuyItemTaskQueueStruct& TaskQueue = it->second;

		BaseItemObject* Obj = BaseItemObject::GetObjByDBID(it->first);
		RoleDataEx* Owner = NULL;

		if (Obj != NULL)
		{
			Owner = Obj->Role();
		}

		if (Owner == NULL)
		{
			if (TaskQueue.Queue.empty() == false)
			{				
				TaskQueue.Queue.clear();
				Print(LV5, "_OnTimeProc_BuyItem", "dbid=%d, Owner == NULL and queue is not empty", it->first);
			}

			if (TaskQueue.Status != EM_BuyItemTaskQueueStatus_StandBy)
			{
				TaskQueue.Status = EM_BuyItemTaskQueueStatus_StandBy;
			}

			continue;
		}

		switch (TaskQueue.Status)
		{
		case EM_BuyItemTaskQueueStatus_StandBy:
			{
				if (TaskQueue.Queue.empty() == false)
				{
					BuyItemTempStruct& Temp = TaskQueue.Queue.front();
					
					SD_BuyItem(Temp.ItemGUID, Owner->DBID(), Owner->PlayerBaseData->Body.Money_Account, Temp.Count, Temp.Pos);
					Temp.SendTick = GetTickCount();						
					TaskQueue.Status = EM_BuyItemTaskQueueStatus_Processing;

					Print(LV2, "_OnTimeProc_BuyItem", "SD_BuyItem(itemguid=%d, dbid=%d, accountmoney=%d, count=%d, pos=%d)", Temp.ItemGUID, Owner->DBID(), Owner->PlayerBaseData->Body.Money_Account, Temp.Count, Temp.Pos);
				}
			}
			break;
		case EM_BuyItemTaskQueueStatus_Processing:
			{
				if (TaskQueue.Queue.empty() == false)
				{
					BuyItemTempStruct& Temp = TaskQueue.Queue.front();
					unsigned long NowTick = GetTickCount();

					if (NowTick - Temp.SendTick >= 30000)
					{
						Print(LV2, "_OnTimeProc_BuyItem", "Timeout, role=%s, dbid=%d, itemguid=%d, count=%d, pos=%d", Owner->RoleName(), Owner->DBID(), Temp.ItemGUID, Temp.Count, Temp.Pos);

						TaskQueue.Queue.clear();
						SC_BuyItemResult( Owner->GUID() , EM_DepartmentStoreBuyItemResult_Err );
						TaskQueue.Status = EM_BuyItemTaskQueueStatus_StandBy;						
					}
				}
				else
				{
					TaskQueue.Status = EM_BuyItemTaskQueueStatus_StandBy;
				}
			}
			break;
		default:
			{
				TaskQueue.Status = EM_BuyItemTaskQueueStatus_StandBy;
			}
		}
	}

	Global::Schedular()->Push(_OnTimeProc_BuyItem, 250, This, NULL, NULL);
	return 0;
}