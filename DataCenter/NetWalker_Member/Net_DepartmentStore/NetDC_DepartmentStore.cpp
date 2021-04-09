#include "NetDC_DepartmentStore.h"
#include "../net_serverlist/Net_ServerList_Child.h"
//-------------------------------------------------------------------
CNetDC_DepartmentStore*	CNetDC_DepartmentStore::This = NULL;

//-------------------------------------------------------------------
bool CNetDC_DepartmentStore::_Release()
{
	return true;
}
//-------------------------------------------------------------------
bool CNetDC_DepartmentStore::_Init()
{
	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_LtoD_SellTypeRequest			, _PG_DepartmentStore_LtoD_SellTypeRequest			);
	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_LtoD_ShopInfoRequest			, _PG_DepartmentStore_LtoD_ShopInfoRequest			);
	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_LtoD_BuyItem					, _PG_DepartmentStore_LtoD_BuyItem					);
	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_LtoD_BuyItemResult				, _PG_DepartmentStore_LtoD_BuyItemResult			);

	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_LtoD_ImportAccountMoney			, _PG_DepartmentStore_LtoD_ImportAccountMoney		);
	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_LtoD_ImportAccountMoneyFailed	, _PG_DepartmentStore_LtoD_ImportAccountMoneyFailed	);

	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_LtoD_ExchangeItemRequest		, _PG_DepartmentStore_LtoD_ExchangeItemRequest		);
	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_LtoD_ExchangeItemResultOK		, _PG_DepartmentStore_LtoD_ExchangeItemResultOK		);

	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_LtoD_RunningMessageRequest		, _PG_DepartmentStore_LtoD_RunningMessageRequest	);

	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_LtoD_CheckMailGift				, _PG_DepartmentStore_LtoD_CheckMailGift			);
	_Net->RegOnSrvPacketFunction( EM_PG_DepartmentStore_LtoD_WebBagRequest				, _PG_DepartmentStore_LtoD_WebBagRequest			);
	Net_ServerList->RegServerReadyEvent( EM_SERVER_TYPE_LOCAL , _OnLocalSrvConnectEvent );
	return true;
}

void CNetDC_DepartmentStore::_PG_DepartmentStore_LtoD_WebBagRequest				( int ServerID , int Size , void* Data )
{
	PG_DepartmentStore_LtoD_WebBagRequest* PG = ( PG_DepartmentStore_LtoD_WebBagRequest* )Data;
	This->RL_WebBagRequest( ServerID , PG->PlayerDBID, PG->Account.Begin() ); 
}

void CNetDC_DepartmentStore::_PG_DepartmentStore_LtoD_CheckMailGift				( int ServerID , int Size , void* Data )
{
	PG_DepartmentStore_LtoD_CheckMailGift* PG = ( PG_DepartmentStore_LtoD_CheckMailGift* )Data;
	This->RL_CheckMailGift( ServerID , PG->PlayerDBID , PG->ItemGUID , PG->Count , PG->TargetName.Begin() , PG->MapKey ); 
}
/*
void CNetDC_DepartmentStore::_PG_DepartmentStore_LtoD_MailGift					( int ServerID , int Size , void* Data )
{
	PG_DepartmentStore_LtoD_MailGift* PG = ( PG_DepartmentStore_LtoD_MailGift* )Data;
	This->RL_RunningMessageRequest( PG->PlayerDBID );
}
*/

void CNetDC_DepartmentStore::_PG_DepartmentStore_LtoD_RunningMessageRequest( int ServerID , int Size , void* Data )
{
	PG_DepartmentStore_LtoD_RunningMessageRequest* PG = ( PG_DepartmentStore_LtoD_RunningMessageRequest* )Data;
	This->RL_RunningMessageRequest( PG->PlayerDBID );
}

void CNetDC_DepartmentStore::_PG_DepartmentStore_LtoD_ExchangeItemRequest		( int ServerID , int Size , void* Data )
{
	PG_DepartmentStore_LtoD_ExchangeItemRequest* PG = ( PG_DepartmentStore_LtoD_ExchangeItemRequest* )Data;
	This->RL_ExchangeItemRequest( ServerID , PG->PlayerDBID , PG->Account.Begin() , PG->ItemSerial , PG->Password , PG->MapKey );
}
void CNetDC_DepartmentStore::_PG_DepartmentStore_LtoD_ExchangeItemResultOK		( int ServerID , int Size , void* Data )
{
	PG_DepartmentStore_LtoD_ExchangeItemResultOK* PG = ( PG_DepartmentStore_LtoD_ExchangeItemResultOK* )Data;
	This->RL_ExchangeItemResultOK( ServerID , PG->PlayerDBID , PG->ExchangeItemDBID , PG->Result , PG->IsDataError );
}

/*
void CNetDC_DepartmentStore::_PG_DepartmentStore_LtoD_BuyItemList		( int ServerID , int Size , void* Data )
{
	PG_DepartmentStore_LtoD_BuyItemList* PG = ( PG_DepartmentStore_LtoD_BuyItemList* )Data;
	This->RL_BuyItemList( ServerID , PG->PlayerDBID , PG->PlayerAccountMoney , PG->Size , PG->ItemGUID[ _MAX_DS_BUYCOUNT_ ] );
}
*/

void CNetDC_DepartmentStore::_PG_DepartmentStore_LtoD_ImportAccountMoney		( int ServerID , int Size , void* Data )
{
	PG_DepartmentStore_LtoD_ImportAccountMoney* PG = ( PG_DepartmentStore_LtoD_ImportAccountMoney* )Data;
	This->RL_ImportAccountMoney( ServerID , PG->PlayerDBID , PG->Account_ID.Begin() );
}
void CNetDC_DepartmentStore::_PG_DepartmentStore_LtoD_ImportAccountMoneyFailed	( int ServerID , int Size , void* Data )
{
	PG_DepartmentStore_LtoD_ImportAccountMoneyFailed* PG = ( PG_DepartmentStore_LtoD_ImportAccountMoneyFailed* )Data;
	This->RL_ImportAccountMoneyFailed( PG->PlayerDBID , PG->Account_ID.Begin() , PG->Money_Account , PG->Money_Bonus , PG->ImportGUID );
}


void CNetDC_DepartmentStore::_PG_DepartmentStore_LtoD_SellTypeRequest	( int ServerID , int Size , void* Data )
{
	PG_DepartmentStore_LtoD_SellTypeRequest* PG = ( PG_DepartmentStore_LtoD_SellTypeRequest* )Data;
	This->RL_SellTypeRequest( ServerID , PG->CliSockID , PG->CliProxyID );
}

void CNetDC_DepartmentStore::_PG_DepartmentStore_LtoD_ShopInfoRequest	( int ServerID , int Size , void* Data )
{
	PG_DepartmentStore_LtoD_ShopInfoRequest* PG = ( PG_DepartmentStore_LtoD_ShopInfoRequest* )Data;
	This->RL_ShopInfoRequest( ServerID , PG->CliSockID , PG->CliProxyID , PG->SellType );
}
void CNetDC_DepartmentStore::_PG_DepartmentStore_LtoD_BuyItem			( int ServerID , int Size , void* Data )
{
	PG_DepartmentStore_LtoD_BuyItem* PG = ( PG_DepartmentStore_LtoD_BuyItem* )Data;
	This->RL_BuyItem( ServerID , PG->ItemGUID , PG->PlayerDBID , PG->PlayerAccountMoney , PG->Pos , PG->Count );
}
void CNetDC_DepartmentStore::_PG_DepartmentStore_LtoD_BuyItemResult		( int ServerID , int Size , void* Data )
{
	PG_DepartmentStore_LtoD_BuyItemResult* PG = ( PG_DepartmentStore_LtoD_BuyItemResult* )Data;
	This->RL_BuyItemResult( ServerID , PG->ItemGUID , PG->PlayerDBID , PG->Result , PG->Count );
}

void CNetDC_DepartmentStore::SL_BuyItemResult	( int SrvSockID , int PlayerDBID , DepartmentStoreBuyItemResultENUM Result , DepartmentStoreBaseInfoStruct* Item , int Count , int Pos )
{
	PG_DepartmentStore_DtoL_BuyItemResult Send;

	Send.PlayerDBID = PlayerDBID;		//ª±®aDBID
	Send.Result = Result;
	if( Item != NULL)
		Send.Item = *Item;
	Send.Count = Count;
	Send.Pos = Pos;

	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );
}

void CNetDC_DepartmentStore::SC_ShopInfo		( int CliSockID , int CliProxyID , vector<DepartmentStoreBaseInfoStruct*>& ItemList )
{
	PG_DepartmentStore_DtoC_ShopInfo Send;

	while( ItemList.size() >= 1000 )
		ItemList.pop_back();

	for( unsigned i = 0 ; i < ItemList.size() ; i++ )
	{
		Send.Item[i] = *ItemList[i];
		Send.Item[i].Sell.Count += Send.Item[i].Sell.OrgCount;
	}
	
	Send.ItemCount = int( ItemList.size() );

	Global::SendToCli_ByProxyID( CliSockID , CliProxyID , Send.Size()  , &Send );
}

void CNetDC_DepartmentStore::SC_ShopInfoZip		( int CliSockID , int CliProxyID , int ItemCount , int DataSize , char* Data )
{
	if( Data == NULL )
		return;

	PG_DepartmentStore_DtoC_ShopInfo_Zip Send;
	Send.DataSize = DataSize;
	memcpy( Send.Data , Data , DataSize );
	Send.ItemCount = ItemCount;
	Global::SendToCli_ByProxyID( CliSockID , CliProxyID , Send.Size()  , &Send );
}


void CNetDC_DepartmentStore::SC_SellType		( int CliSockID , int CliProxyID , vector<int>& SellType )
{
	PG_DepartmentStore_DtoC_SellType Send;

	while( SellType.size() >= 50 )
		SellType.pop_back();

	for( unsigned i = 0 ; i < SellType.size() ; i++ )
	{
		Send.SellType[i] = SellType[i];
	}

	Send.ItemCount = int( SellType.size() );

	Global::SendToCli_ByProxyID( CliSockID , CliProxyID , Send.Size()  , &Send );
}

void CNetDC_DepartmentStore::SL_ImportAccountMoneyResult( int SrvSockID , int PlayerDBID , const char* Account_ID , int Money_Account , int Money_Bonus , int LockDay , int ImportGUID )
{
	PG_DepartmentStore_DtoL_ImportAccountMoneyResult Send;
	Send.Money_Account = Money_Account;
	Send.Money_Bonus = Money_Bonus;
	Send.PlayerDBID = PlayerDBID;
	Send.Account_ID = Account_ID;
	Send.LockDay = LockDay;
	Send.ImportGUID = ImportGUID;

	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );
}
void CNetDC_DepartmentStore::SL_ExchangeItemResult( int SrvSockID ,	int	PlayerDBID , ItemFieldStruct &Item , int Money , int MoneyAccount , int ExchangeItemDBID , ItemExchangeResultENUM Result, int LockDay, int MapKey )
{
	PG_DepartmentStore_DtoL_ExchangeItemResult Send;
	Send.PlayerDBID = PlayerDBID;
	Send.Item = Item;
	Send.Money = Money;
	Send.MoneyAccount = MoneyAccount;
	Send.ExchangeItemDBID = ExchangeItemDBID;
	Send.Result = Result;
	Send.LockDay = LockDay;
	Send.MapKey = MapKey;
	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );
}

void CNetDC_DepartmentStore::SC_RunningMessage_Zip( int PlayerDBID , int MsgCount , int Size , const char* Data )
{
	if(Size > 0x10000 )
		return;

	PG_DepartmentStore_DtoC_RunningMessage_Zip Send;
//	memset( Send.Data , 'c' , sizeof(Send.Data) );
	memcpy( Send.Data , Data , Size );
	Send.MsgCount = MsgCount;
	Send.Size = Size;

//	Global::SendToCli_ByDBID( PlayerDBID , sizeof( Send ) , &Send );
	Global::SendToCli_ByDBID( PlayerDBID , Send.PGSize() , &Send );
}

void CNetDC_DepartmentStore::SL_CheckMailGiftResult( int SrvSockID , int MapKey , int TargetDBID , CheckMailGiftResultTypeENUM Result , DepartmentStoreBaseInfoStruct* Item )
{
	PG_DepartmentStore_DtoL_CheckMailGiftResult Send;
	Send.MapKey = MapKey;
	Send.Result = Result;
	Send.TargetDBID = TargetDBID;
	if( Item != NULL )
		Send.Item = *Item;
	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );
}
void CNetDC_DepartmentStore::SL_WebBagResult( int ServerID , int PlayerDBID , int ItemDBID , ItemFieldStruct* Item , bool IsEnd , char* Message, int PriceType , int Price , int AddBonusMoney )
{
	PG_DepartmentStore_DtoL_WebBagResult Send;
	Send.IsEnd = IsEnd;
	if( Item != NULL )
		Send.Item = *Item;
	Send.ItemGUID = ItemDBID;
	Send.PlayerDBID = PlayerDBID;
	strncpy( Send.Message , Message , sizeof(Send.Message) );
	Send.PriceType = PriceType;
	Send.Price = Price;
	Send.AddBonusMoney = AddBonusMoney;
	Global::SendToSrvBySockID( ServerID , sizeof( Send ) , &Send );
}
void CNetDC_DepartmentStore::SL_SlotMachineInfo( int ZoneID , SlotMachineBase base[_MAX_SLOT_MACHINE_PRIZES_COUNT_] , vector< SlotMachineTypeBase >& typeInfo )
{
	PG_DepartmentStore_DtoL_SlotMachineInfo send;
	send.TypeInfoCount = typeInfo.size();
	memcpy( send.Base , base , sizeof(send.Base ) );
	if( send.TypeInfoCount == 0 )
		return;
	memcpy( send.TypeInfo , &typeInfo[0] , sizeof(SlotMachineTypeBase) * send.TypeInfoCount );
	if( ZoneID == -1 )
	{
		Global::SendToAllLocal( send.PGSize() , &send );
	}
	else
	{
		Global::SendToLocal( ZoneID , send.PGSize() , &send );
	}

}
void CNetDC_DepartmentStore::_OnLocalSrvConnectEvent ( EM_SERVER_TYPE ServerType, DWORD SrvID )
{
	if( ServerType != EM_SERVER_TYPE_LOCAL )
		return;
	This->OnLocalSrvConnect( SrvID );
}
