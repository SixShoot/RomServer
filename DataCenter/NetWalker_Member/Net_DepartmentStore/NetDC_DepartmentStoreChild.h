#pragma once

#include "NetDC_DepartmentStore.h"
//////////////////////////////////////////////////////////////////////////
//實做注意事項
//1 初始讀取所有商城資料
//2 定時回存並重新讀取( 回存時注意是否有角色正在購買 )
//////////////////////////////////////////////////////////////////////////

// 匯入帳號幣專用狀態值列舉.
enum e_ImportAccountMoney_ProcessStatus // do not over 0xFF (127) (tinyint)
{
    ImportAccountMoneyProcess_Null              = 0x00, // trap, invalid insert will get default value. (also for database update checking)

    ImportAccountMoneyProcess_FirstInsert       = 0x01, // 新資料.
    ImportAccountMoneyProcess_CheckFailed       = 0x02, // 檢查失敗.
    ImportAccountMoneyProcess_WithdrawFailed    = 0x03, // 提領失敗.
    ImportAccountMoneyProcess_Unlocked          = 0x04, // 解鎖後.

    ImportAccountMoneyProcess_Checking          = 0x20, // 正在檢查.
    ImportAccountMoneyProcess_Checked           = 0x21, // 已經檢查.

    ImportAccountMoneyProcess_Withdraw          = 0x30, // 正在提領.

    ImportAccountMoneyProcess_Locked            = 0x40, // 已經鎖定.

    ImportAccountMoneyProcess_Max               = 0xFF, // 最大值.
};

//--------------------------------------------------------------------------------------------
class CNetDC_DepartmentStoreChild : public CNetDC_DepartmentStore
{
	static bool														_IsEnterBroadcastingProc;
	static vector< string >											_BroadCastingMsg;
	static int														_BroadCastingCount;
	static int														_BroadCastingZipSize;
	static char														_BroadCastingZip[0x10000];
	
	static map< int , DepartmentStoreBaseInfoStruct >				_SellItemList;				//販賣物品列表
	static map< int , DepartmentStoreBaseInfoStruct >				_SellItemList_DB;			//販賣物品列表
	static map< int , vector< DepartmentStoreBaseInfoStruct* > >	_SellItemList_SellType;				
	static map< int , MemoryAllocStruct >							_SellItemList_SellType_Zip;

    static bool m_bImportAccountMoney_CheckAndLog;  // 是否開啟帳號匯入金錢檢查.

	static vector<int>	_SellType;
	static bool _IsSaveShopInfo;

	static bool _SQLCmdOnTimeProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdOnTimeProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

    // 匯入帳號幣之事前檢查(呼叫遠端 WebService)
	static bool _SQLCmdImportAccountMoneyCheck(vector<int>& Vec, SqlBaseClass* sqlBase, void* UserObj, ArgTransferStruct& Arg);
    static void _SQLCmdImportAccountMoneyCheckResult(vector<int>& Vec, void* UserObj, ArgTransferStruct& Arg, bool ResultOK);

	//提領點數幣
	static bool _SQLCmdImportAccountMoneyProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdImportAccountMoneyResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//回存點數幣
	static bool _SQLCmdImportAccountMoneyErrProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdImportAccountMoneyErrResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//兌換物品
	static bool _SQLCmdItemExchangeProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdItemExchangeResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//兌換物品失敗回復
	static bool _SQLCmdItemExchangeErrProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdItemExchangeErrResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLOnTimeBroadCastingProcEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLOnTimeBroadCastingProcResultEvent( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdCheckSendMailEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdCheckSendMailResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//取得網頁商城的東西
	static bool _SQLCmdImportWebShopItemProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdImportWebShopItemResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//定時處理
	static int _OnTimeProc_( SchedularInfo* Obj , void* InputClass );
	static int _OnTimeProc_SellItemListZip( SchedularInfo* Obj , void* InputClass );
	//
	static void _SellItemListZipProc();

	//////////////////////////////////////////////////////////////////////////
	static SlotMachineBase					_SlotMachineBaseInfo[_MAX_SLOT_MACHINE_PRIZES_COUNT_];
	static vector< SlotMachineTypeBase >	_SlotMachineTypeInfo;

	static int  _OnTimeProc_SlotMachine( SchedularInfo* Obj , void* InputClass );
	static bool _SQLCmdLoadSlotMachineEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadSlotMachineResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//////////////////////////////////////////////////////////////////////////
public:
	//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

	virtual void RL_SellTypeRequest	( int SrvSockID , int CliSockID , int CliProxyID );
	virtual void RL_ShopInfoRequest	( int SrvSockID , int CliSockID , int CliProxyID , int SellType );
	virtual void RL_BuyItem			( int SrvSockID , int ItemGUID , int PlayerDBID , int PlayerAccountMoney , int Pos , int Count  );
	virtual void RL_BuyItemResult	( int SrvSockID , int ItemGUID , int PlayerDBID , bool Result , int Count );

	virtual void RL_ImportAccountMoney			( int ServerID , int PlayerDBID , const char* Account );
	virtual void RL_ImportAccountMoneyFailed	( int PlayerDBID , const char* Account , int Money_Account , int Money_Bonus , int ImportGUID );

	virtual void RL_ExchangeItemRequest( int ServerID , int PlayerDBID , const char* Account , char* ItemVersion , char* Password, int MapKey );
	virtual void RL_ExchangeItemResultOK( int ServerID , int PlayerDBID , int ExchangeItemDBID , bool Result , bool IsDataError );

	virtual void RL_RunningMessageRequest( int PlayerDBID );

	virtual void RL_CheckMailGift		( int SrvNetID , int PlayerDBID , int ItemGUID , int Count , const char* TargetName , int MapKey  );

	virtual void RL_WebBagRequest( int ServerID , int PlayerDBID, const char* Account ); 
	virtual void OnLocalSrvConnect( int ZoneID );
};
