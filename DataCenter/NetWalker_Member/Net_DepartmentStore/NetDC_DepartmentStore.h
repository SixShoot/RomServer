
#pragma once
#include "PG/PG_DepartmentStore.h"
#include "../../MainProc/Global.h"

class CNetDC_DepartmentStore : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetDC_DepartmentStore*	This;

	static bool				_Init();
	static bool				_Release();

	static void _PG_DepartmentStore_LtoD_SellTypeRequest			( int ServerID , int Size , void* Data );
	static void _PG_DepartmentStore_LtoD_ShopInfoRequest			( int ServerID , int Size , void* Data );
	static void _PG_DepartmentStore_LtoD_BuyItem					( int ServerID , int Size , void* Data );
	static void _PG_DepartmentStore_LtoD_BuyItemResult				( int ServerID , int Size , void* Data );

	static void _PG_DepartmentStore_LtoD_ImportAccountMoney			( int ServerID , int Size , void* Data );
	static void _PG_DepartmentStore_LtoD_ImportAccountMoneyFailed	( int ServerID , int Size , void* Data );

	static void _PG_DepartmentStore_LtoD_ExchangeItemRequest		( int ServerID , int Size , void* Data );
	static void _PG_DepartmentStore_LtoD_ExchangeItemResultOK		( int ServerID , int Size , void* Data );

	static void _PG_DepartmentStore_LtoD_RunningMessageRequest		( int ServerID , int Size , void* Data );


	static void _PG_DepartmentStore_LtoD_CheckMailGift				( int ServerID , int Size , void* Data );
//	static void _PG_DepartmentStore_LtoD_MailGift					( int ServerID , int Size , void* Data );
	static void _PG_DepartmentStore_LtoD_WebBagRequest				( int ServerID , int Size , void* Data );

	static void _OnLocalSrvConnectEvent ( EM_SERVER_TYPE ServerType, DWORD SrvID );

public:
	static void SL_BuyItemResult	( int SrvSockID , int PlayerDBID , DepartmentStoreBuyItemResultENUM Result , DepartmentStoreBaseInfoStruct* Item = NULL , int Count = 0 , int Pos = 0 );
	static void SC_ShopInfo			( int CliSockID , int CliProxyID , vector<DepartmentStoreBaseInfoStruct*>& ItemList );
	static void SC_ShopInfoZip		( int CliSockID , int CliProxyID , int ItemCount , int DataSize , char* Data );
	static void SC_SellType			( int CliSockID , int CliProxyID , vector<int>& SellType );

	static void SL_ImportAccountMoneyResult( int SrvSockID , int PlayerDBID , const char* Account_ID , int Money_Account , int Money_Bonus , int LockDay , int ImportGUID );
	static void SL_ExchangeItemResult( int SrvSockID ,	int	PlayerDBID , ItemFieldStruct &Item , int Money , int MoneyAccount , int ExchangeItemDBID , ItemExchangeResultENUM Result, int LockDay, int MapKey );

	virtual void RL_ShopInfoRequest	( int SrvSockID , int CliSockID , int CliProxyID , int SellType ) = 0;
	virtual void RL_SellTypeRequest	( int SrvSockID , int CliSockID , int CliProxyID ) = 0;
	virtual void RL_BuyItem			( int SrvSockID ,int ItemGUID , int PlayerDBID , int PlayerAccountMoney , int Pos , int Count ) = 0;
	virtual void RL_BuyItemResult	( int SrvSockID ,int ItemGUID , int PlayerDBID , bool Result , int Count ) = 0;

	virtual void RL_ImportAccountMoney			( int ServerID , int PlayerDBID , const char* Account ) = 0;
	virtual void RL_ImportAccountMoneyFailed	( int PlayerDBID , const char* Account , int Money_Account , int Money_Bonus , int ImportGUID ) = 0;
	virtual void RL_ExchangeItemRequest( int ServerID , int PlayerDBID , const char* Account , char* ItemVersion , char* Password, int MapKey ) = 0;
	virtual void RL_ExchangeItemResultOK( int ServerID , int PlayerDBID , int ExchangeItemDBID , bool Result , bool IsDataError ) = 0;

	static void SC_RunningMessage_Zip( int PlayerDBID , int MsgCount , int Size , const char* Data );
	virtual void RL_RunningMessageRequest( int PlayerDBID ) = 0;

	//////////////////////////////////////////////////////////////////////////
	//送禮
	virtual void RL_CheckMailGift		( int SrvNetID , int PlayerDBID , int ItemGUID , int Count , const char* TargetName , int MapKey  ) = 0;
	static void SL_CheckMailGiftResult	( int SrvSockID , int MapKey , int TargetDBID , CheckMailGiftResultTypeENUM Result , DepartmentStoreBaseInfoStruct* Item = NULL );

	//////////////////////////////////////////////////////////////////////////
	//取得網頁商城的物品
	virtual void RL_WebBagRequest( int ServerID , int PlayerDBID, const char* Account) = 0; 
	static void SL_WebBagResult( int ServerID , int PlayerDBID , int ItemDBID , ItemFieldStruct* Item , bool IsEnd , char* Message , int PriceType , int Price , int AddBonusMoney );
	//////////////////////////////////////////////////////////////////////////
	//DataCenter -> Local 老虎機資料
	static void SL_SlotMachineInfo( int ZoneID , SlotMachineBase base[_MAX_SLOT_MACHINE_PRIZES_COUNT_] , vector< SlotMachineTypeBase >& typeInfo );
	virtual void OnLocalSrvConnect( int ZoneID ) = 0;
};
