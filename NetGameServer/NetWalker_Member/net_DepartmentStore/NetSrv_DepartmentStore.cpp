#include "../NetWakerGSrvInc.h"
#include "NetSrv_DepartmentStore.h"
//-------------------------------------------------------------------
NetSrv_DepartmentStore*    NetSrv_DepartmentStore::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_DepartmentStore::_Init()
{
	Srv_NetCliReg( PG_DepartmentStore_CtoL_ShopFunctionRequest	);
	Srv_NetCliReg( PG_DepartmentStore_CtoL_SellTypeRequest		);
	Srv_NetCliReg( PG_DepartmentStore_CtoL_ShopInfoRequest		);
	Srv_NetCliReg( PG_DepartmentStore_CtoL_BuyItem				);
	Srv_NetCliReg( PG_DepartmentStore_CtoL_ImportAccountMoney	);
	Srv_NetCliReg( PG_DepartmentStore_CtoL_ExchangeItemRequest	);
	Srv_NetCliReg( PG_DepartmentStore_CtoL_RunningMessageRequest);

	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_DtoL_BuyItemResult			 , _PG_DepartmentStore_DtoL_BuyItemResult       		   );
	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_DtoL_ImportAccountMoneyResult, _PG_DepartmentStore_DtoL_ImportAccountMoneyResult       );
	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_DtoL_ExchangeItemResult		 , _PG_DepartmentStore_DtoL_ExchangeItemResult		       );


	Srv_NetCliReg( PG_DepartmentStore_CtoL_MailGift	);
	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_DtoL_CheckMailGiftResult	 , _PG_DepartmentStore_DtoL_CheckMailGiftResult		   );

	Srv_NetCliReg( PG_DepartmentStore_CtoL_WebBagRequest );
	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_DtoL_WebBagResult			 , _PG_DepartmentStore_DtoL_WebBagResult		   );


	Srv_NetCliReg( PG_DepartmentStore_CtoL_SlotMachineInfoRequest				);
	Srv_NetCliReg( PG_DepartmentStore_CtoL_SlotMachinePlay	);
	Srv_NetCliReg( PG_DepartmentStore_CtoL_GetSlotMachineItem	);
	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_DtoL_SlotMachineInfo		 , _PG_DepartmentStore_DtoL_SlotMachineInfo		   );
	return true;
}
//-------------------------------------------------------------------
bool NetSrv_DepartmentStore::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_DepartmentStore::_PG_DepartmentStore_CtoL_GetSlotMachineItem( int netID , int size , void* data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( netID );
	if( Obj == NULL )
		return ;

	PG_DepartmentStore_CtoL_GetSlotMachineItem* pg = (PG_DepartmentStore_CtoL_GetSlotMachineItem*)data;
	This->RC_GetSlotMachineItem( Obj );
}
void NetSrv_DepartmentStore::_PG_DepartmentStore_DtoL_SlotMachineInfo( int netID , int size , void* data )
{
	PG_DepartmentStore_DtoL_SlotMachineInfo* pg = (PG_DepartmentStore_DtoL_SlotMachineInfo*)data;
	This->RD_SlotMachineInfo( pg->Base , pg->TypeInfoCount , pg->TypeInfo );
}
void NetSrv_DepartmentStore::_PG_DepartmentStore_CtoL_SlotMachineInfoRequest( int netID , int size , void* data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( netID );
	if( Obj == NULL )
		return ;
	PG_DepartmentStore_CtoL_SlotMachineInfoRequest* pg = (PG_DepartmentStore_CtoL_SlotMachineInfoRequest*)data;
	This->RC_SlotMachineInfoRequest( Obj );
}
void NetSrv_DepartmentStore::_PG_DepartmentStore_CtoL_SlotMachinePlay( int netID , int size , void* data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( netID );
	if( Obj == NULL )
		return ;
	PG_DepartmentStore_CtoL_SlotMachinePlay* pg = (PG_DepartmentStore_CtoL_SlotMachinePlay*)data;
	This->RC_SlotMachinePlay( Obj );
}
/*
void NetSrv_DepartmentStore::_PG_CliConnect_LtoBL_BuyItemResult				( int FromWorldId , int FromNetID , int Size , void* Data );
{
	PG_CliConnect_LtoBL_BuyItemResult*   PG = (PG_CliConnect_LtoBL_BuyItemResult*)Data;

	This->RL_BuyItemResult( FromWorldId , FromNetID , PG->PlayerDBID , PG->Result , PG->Item , PG->Pos );
}
void NetSrv_DepartmentStore::_PG_DepartmentStore_BLtoL_BuyItemResult		( int FromWorldId , int FromNetID , int Size , void* Data );
{
	PG_DepartmentStore_BLtoL_BuyItemResult*   PG = (PG_DepartmentStore_BLtoL_BuyItemResult*)Data;


	This->RBL_BuyItemResult( FromWorldId , FromNetID , PG->PlayerDBID , PG->ItemGUID , PG->Result , PG->Count  );
}
*/

void NetSrv_DepartmentStore::_PG_DepartmentStore_CtoL_WebBagRequest			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_DepartmentStore_CtoL_WebBagRequest* PG = (PG_DepartmentStore_CtoL_WebBagRequest*)Data;
	This->RC_WebBagRequest( Obj );
}
void NetSrv_DepartmentStore::_PG_DepartmentStore_DtoL_WebBagResult			( int NetID , int Size , void* Data )
{
	PG_DepartmentStore_DtoL_WebBagResult* PG = (PG_DepartmentStore_DtoL_WebBagResult*)Data;
	This->RD_WebBagResult( PG->PlayerDBID , PG->ItemGUID , PG->Item , PG->IsEnd , PG->Message , PG->PriceType, PG->Price, PG->AddBonusMoney );
}

void NetSrv_DepartmentStore::_PG_DepartmentStore_CtoL_MailGift				( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	
	//信任玩機制
	RoleDataEx*     Player = Global::GetRoleDataByGUID( Obj->GUID() );
	if( Player->PlayerBaseData->VipMember.UntrustFlag.DisableDepartmentStoreMailGift  )
	{
		//Player->Msg( "信任玩家機制測試_無法商城送禮" );
		Player->Net_GameMsgEvent( EM_GameMessageEvent_Role_Untrust);
		NetSrv_DepartmentStoreChild::SC_MailGiftResult( Player->GUID() , EM_CheckMailGiftResultType_DisableSendGift );
		return;
	}

	PG_DepartmentStore_CtoL_MailGift* PG = (PG_DepartmentStore_CtoL_MailGift*)Data;
	This->RC_MailGift( Obj , PG->ItemGUID , PG->Count , PG->Password.Begin() , PG->BackGroundType , PG->TargetName.Begin() , PG->Title.Begin() , PG->Content.Begin() );

}
void NetSrv_DepartmentStore::_PG_DepartmentStore_DtoL_CheckMailGiftResult	( int NetID , int Size , void* Data )
{
	PG_DepartmentStore_DtoL_CheckMailGiftResult* PG = (PG_DepartmentStore_DtoL_CheckMailGiftResult*)Data;
	This->RD_CheckMailGiftResult( PG->MapKey , PG->TargetDBID , PG->Item , PG->Result );
}


void NetSrv_DepartmentStore::_PG_DepartmentStore_CtoL_RunningMessageRequest( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_DepartmentStore_CtoL_RunningMessageRequest* PG = (PG_DepartmentStore_CtoL_RunningMessageRequest*)Data;
	This->RC_RunningMessageRequest( Obj );
}

void NetSrv_DepartmentStore::_PG_DepartmentStore_CtoL_ExchangeItemRequest( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_DepartmentStore_CtoL_ExchangeItemRequest* PG = (PG_DepartmentStore_CtoL_ExchangeItemRequest*)Data;
	This->RC_ExchangeItemRequest( Obj , PG->ItemSerial , PG->Password );
}
void NetSrv_DepartmentStore::_PG_DepartmentStore_DtoL_ExchangeItemResult ( int NetID , int Size , void* Data )
{
//	BaseItemObject*	Obj =	BaseItemObject::GetObjByDBID( PlayerDBID );
//	if( Obj == NULL )
//		return ;

	PG_DepartmentStore_DtoL_ExchangeItemResult* PG = (PG_DepartmentStore_DtoL_ExchangeItemResult*)Data;
	This->RD_ExchangeItemResult( PG->PlayerDBID , PG->Item , PG->Money , PG->MoneyAccount , PG->ExchangeItemDBID , PG->Result, PG->LockDay, PG->MapKey );
}

void NetSrv_DepartmentStore::_PG_DepartmentStore_CtoL_ImportAccountMoney		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_DepartmentStore_CtoL_ImportAccountMoney* PG = (PG_DepartmentStore_CtoL_ImportAccountMoney*)Data;
	This->RC_ImportAccountMoney( Obj );
}
void NetSrv_DepartmentStore::_PG_DepartmentStore_DtoL_ImportAccountMoneyResult	( int NetID , int Size , void* Data )
{
	PG_DepartmentStore_DtoL_ImportAccountMoneyResult* PG = (PG_DepartmentStore_DtoL_ImportAccountMoneyResult*)Data;
	This->RD_ImportAccountMoneyResult( PG->PlayerDBID , PG->Account_ID.Begin() , PG->Money_Account , PG->Money_Bonus , PG->LockDay , PG->ImportGUID );
}

void NetSrv_DepartmentStore::_PG_DepartmentStore_CtoL_SellTypeRequest			( int NetID , int Size , void* Data )
{

	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_DepartmentStore_CtoL_SellTypeRequest* PG = (PG_DepartmentStore_CtoL_SellTypeRequest*)Data;
	This->RC_SellTypeRequest( Obj );
}

void NetSrv_DepartmentStore::_PG_DepartmentStore_CtoL_ShopFunctionRequest			( int NetID , int Size , void* Data )
{
	
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_DepartmentStore_CtoL_ShopFunctionRequest* PG = (PG_DepartmentStore_CtoL_ShopFunctionRequest*)Data;
	This->RC_ShopFunctionReques( Obj , PG->FunctionID );
	
}
void NetSrv_DepartmentStore::_PG_DepartmentStore_CtoL_ShopInfoRequest	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_DepartmentStore_CtoL_ShopInfoRequest* PG = (PG_DepartmentStore_CtoL_ShopInfoRequest*)Data;
	This->RC_ShopInfoRequest( Obj , PG->SellType );
}
void NetSrv_DepartmentStore::_PG_DepartmentStore_CtoL_BuyItem			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_DepartmentStore_CtoL_BuyItem* PG = (PG_DepartmentStore_CtoL_BuyItem*)Data;
	This->RC_BuyItem( Obj , PG->ItemGUID , PG->Count , PG->Pos , (char*)PG->Password.Begin() );
}
void NetSrv_DepartmentStore::_PG_DepartmentStore_DtoL_BuyItemResult		( int NetID , int Size , void* Data )
{
	PG_DepartmentStore_DtoL_BuyItemResult* PG = (PG_DepartmentStore_DtoL_BuyItemResult*)Data;
	BaseItemObject* Obj = BaseItemObject::GetObjByDBID( PG->PlayerDBID );
	if( Obj == NULL )
		return;

	This->RD_BuyItemResult( Obj , PG->Result , PG->Item , PG->Count , PG->Pos );
}

void NetSrv_DepartmentStore::SC_ShopFunctionResult		( int SendToID , int FunctionID , bool Result )
{
	PG_DepartmentStore_LtoC_ShopFunctionResult Send;
	Send.FunctionID = FunctionID;
	Send.Result		= Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_DepartmentStore::SC_BuyItemResult	( int SendToID , DepartmentStoreBuyItemResultENUM Result  )
{
	PG_DepartmentStore_LtoC_BuyItemResult Send;
	Send.Result		= Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_DepartmentStore::SD_BuyItem			( int ItemGUID , int PlayerDBID , int PlayerAccountMoney , int Count , int Pos )
{
	PG_DepartmentStore_LtoD_BuyItem Send;
	Send.ItemGUID	= ItemGUID;		//物品索引號
	Send.PlayerDBID = PlayerDBID;	//玩家DBID
	Send.PlayerAccountMoney = PlayerAccountMoney;
	Send.Count		= Count;
	Send.Pos		= Pos;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_DepartmentStore::SD_SellTypeRequest	( int CliSockID , int CliProxyID )
{
	PG_DepartmentStore_LtoD_SellTypeRequest Send;
	Send.CliSockID = CliSockID;
	Send.CliProxyID = CliProxyID;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_DepartmentStore::SD_ShopInfoRequest	( int CliSockID , int CliProxyID , int SellType )
{
	PG_DepartmentStore_LtoD_ShopInfoRequest Send;
	Send.CliSockID = CliSockID;
	Send.CliProxyID = CliProxyID;
	Send.SellType = SellType;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_DepartmentStore::SD_BuyItemResult	( int PlayerDBID , int ItemGUID , bool Result , int Count )
{
	PG_DepartmentStore_LtoD_BuyItemResult Send;
	Send.PlayerDBID = PlayerDBID;
	Send.ItemGUID = ItemGUID;
	Send.Result = Result;
	Send.Count = Count;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_DepartmentStore::SD_ImportAccountMoney		( int PlayerDBID , const char* Account )
{
	Print( LV3 , "SD_ImportAccountMoney" , " PlayerDBID =%d Account=%s" ,  PlayerDBID , Account );

	if (Global::St()->IsUseBillingServer == true)
	{
		Print(LV3 , "SD_ImportAccountMoney" , "SB_BalanceCheck");

		Net_BillingChild::SB_BalanceCheck(Account, PlayerDBID, 0);
	}
	else
	{
		PG_DepartmentStore_LtoD_ImportAccountMoney Send;
		Send.PlayerDBID = PlayerDBID;
		Send.Account_ID = Account;

		Global::SendToDBCenter( sizeof( Send ) , &Send );
	}
}

void NetSrv_DepartmentStore::SD_ImportAccountMoneyFailed	( int PlayerDBID , const char* Account , int Money_Account , int Money_Bonus , int ImportGUID )
{
	PG_DepartmentStore_LtoD_ImportAccountMoneyFailed Send;
	Send.PlayerDBID = PlayerDBID;
	Send.Account_ID = Account;
	Send.Money_Account = Money_Account;
	Send.Money_Bonus = Money_Bonus;
	Send.ImportGUID = ImportGUID;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_DepartmentStore::SC_ImportAccountMoneyResult	( int SendID  , int Money_Account , int Money_Bonus)
{
	PG_DepartmentStore_LtoC_ImportAccountMoneyResult Send;
	Send.Money_Account = Money_Account;
	Send.Money_Bonus = Money_Bonus;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_DepartmentStore::SD_ExchangeItemRequest		( int PlayerDBID , const char* Account , const char* ItemSerial , const char* Password, int MapKey )
{
	PG_DepartmentStore_LtoD_ExchangeItemRequest Send;
	Send.Account = Account;
	Send.PlayerDBID = PlayerDBID;
	MyStrcpy( Send.ItemSerial	, ItemSerial	, sizeof(Send.ItemSerial ) );
	MyStrcpy( Send.Password		, Password		, sizeof(Send.Password ) );
	Send.MapKey = MapKey;
	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_DepartmentStore::SD_ExchangeItemResultOK	( int PlayerDBID , int ExchangeItemDBID , bool Result , bool IsDataError )
{
	PG_DepartmentStore_LtoD_ExchangeItemResultOK Send;
	Send.PlayerDBID = PlayerDBID;
	Send.ExchangeItemDBID = ExchangeItemDBID;
	Send.Result = Result;
	Send.IsDataError = IsDataError;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_DepartmentStore::SC_ExchangeItemResult		( int PlayerDBID , ItemFieldStruct& Item , int Money , int MoneyAccount , ItemExchangeResultENUM Result )
{
	PG_DepartmentStore_LtoC_ExchangeItemResult Send;
	Send.Item = Item;
	Send.Money = Money;
	Send.MoneyAccount = MoneyAccount;
	Send.Result = Result;
	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
}

void NetSrv_DepartmentStore::SD_RunningMessageRequest( int PlayerDBID )
{
	PG_DepartmentStore_LtoD_RunningMessageRequest Send;
	Send.PlayerDBID = PlayerDBID;
	Global::SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_DepartmentStore::SD_CheckMailGift		( int PlayerDBID , int ItemGUID , int Count , const char* TargetName , int MapKey )
{
	PG_DepartmentStore_LtoD_CheckMailGift Send;

	Send.ItemGUID = ItemGUID;		//物品索引號
	Send.Count = Count;			//購買數量
	Send.TargetName = TargetName;
	Send.MapKey = MapKey;
	Send.PlayerDBID = PlayerDBID;
	Global::SendToDBCenter( sizeof( Send ) , &Send );

}
/*
void NetSrv_DepartmentStore::SD_MailGift			( int ItemGUID , int Count , int Pos )
{
	PG_DepartmentStore_LtoD_MailGift Send;

	Send.ItemGUID = ItemGUID;
	Send.Count		= Count;			//購買數量
	Send.Pos		= Pos;			//放置位置

	Global::SendToDBCenter( sizeof( Send ) , &Send );

}
*/
void NetSrv_DepartmentStore::SC_MailGiftResult		( int SendToID , CheckMailGiftResultTypeENUM Result )
{
	PG_DepartmentStore_LtoC_MailGiftResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_DepartmentStore::SD_WebBagRequest			( RoleDataEx* Role )
{
	Role->TempData.Sys.WaitWebBagInfo = true;	
	PG_DepartmentStore_LtoD_WebBagRequest Send;
	Send.PlayerDBID = Role->DBID();
	Send.Account = Role->Account_ID();
	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_DepartmentStore::SC_WebBagResult( int SendToID, int OrgObjID, int Count )
{
	PG_DepartmentStore_LtoC_WebBagResult Send;
	Send.OrgObjID = OrgObjID;
	Send.Count = Count;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//Local -> Client 通知所有人 老虎機資料有改變
void NetSrv_DepartmentStore::SC_All_SlotMachineInfoChange(   )
{
	PG_DepartmentStore_LtoC_SlotMachineInfoChange send;
	Global::SendToAllCli( sizeof(send) , &send );
}
//Local -> Client 通知老虎機資料
void NetSrv_DepartmentStore::SC_SlotMachineInfo( int SendToID , int itemID[] , int itemCount[] )
{
	PG_DepartmentStore_LtoC_SlotMachineInfo Send;
	memcpy( Send.ItemID , itemID , sizeof( Send.ItemID ) );
	memcpy( Send.ItemCount , itemCount , sizeof( Send.ItemCount ) );
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//Local -> Client 結果
void NetSrv_DepartmentStore::SC_SlotMachinePlayResult( int SendToID , SlotMachinePlayResultENUM result , int itemID , int itemCount )
{
	PG_DepartmentStore_LtoC_SlotMachinePlayResult Send;
	Send.Result = result;
	Send.ItemID = itemID;
	Send.ItemCount = itemCount;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_DepartmentStore::SC_SlotMachinePlayResultFailed( int SendToID , SlotMachinePlayResultENUM result )
{
	PG_DepartmentStore_LtoC_SlotMachinePlayResult Send;
	Send.Result = result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
