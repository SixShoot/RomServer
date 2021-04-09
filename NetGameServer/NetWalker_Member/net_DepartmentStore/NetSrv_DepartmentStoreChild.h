#pragma once
#include "NetSrv_DepartmentStore.h"

enum BuyItemTaskQueueStatusENUM
{
	EM_BuyItemTaskQueueStatus_StandBy	,
	EM_BuyItemTaskQueueStatus_Processing,
};

struct BuyItemTempStruct
{
	int			  ItemGUID;
	int			  Count;
	int			  Pos;
	unsigned long SendTick;

	BuyItemTempStruct()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct BuyItemTaskQueueStruct
{
	int Status;
	std::deque<BuyItemTempStruct> Queue;

	BuyItemTaskQueueStruct()
	{
		Status = EM_BuyItemTaskQueueStatus_StandBy;
	}
};

struct MailGiftTempStruct
{
	int PlayerDBID;
	int PlayerGUID;
	int ItemGUID;
	int Count;
	int Pos;
	MailBackGroundTypeENUM BackGroundType;
	std::string TargetName;
	std::string Title;
	std::string Content;
};

struct BuyItemResultTempStruct
{
	int PlayerDBID;
	DepartmentStoreBuyItemResultENUM Result;
	DepartmentStoreBaseInfoStruct Item;
	int Count;
	int Pos;
};

struct CheckMailGiftResultTempStruct
{
	int MapKey;
	int TargetDBID;
	DepartmentStoreBaseInfoStruct Item;
	CheckMailGiftResultTypeENUM Result;
};

class NetSrv_DepartmentStoreChild : public NetSrv_DepartmentStore
{
protected:
	static int										  _MailGiftKey;
	static map< int , MailGiftTempStruct >			  _MailGiftTempInfo;
	static int										  _ExchangeItemRequestKey;
	static map< int , std::string >					  _ExchangeItemRequestTempInfo;
	static int										  _BuyItemResultKey;
	static map< int , BuyItemResultTempStruct >		  _BuyItemResultTempInfo;
	static int										  _CheckMailGiftResultKey;
	static map< int , CheckMailGiftResultTempStruct > _CheckMailGiftResultTempInfo;
	static map< int , BuyItemTaskQueueStruct >		  _BuyItemTaskQueue;

	static void WebBagProc( BaseItemObject* Obj );

	static SlotMachineBase							   _SlotMachineBaseInfo[_MAX_SLOT_MACHINE_PRIZES_COUNT_];
	static map< int , vector< SlotMachineTypeBase > >  _SlotMachineTypeInfo;

public:
    static bool Init();
    static bool Release();

	virtual void RC_ShopFunctionReques	( BaseItemObject* Obj , int FunctionID );
	virtual void RC_SellTypeRequest		( BaseItemObject* Obj );
	virtual void RC_ShopInfoRequest		( BaseItemObject* Obj , int SellType );
	virtual void RC_BuyItem				( BaseItemObject* Obj , int ItemGUID , int Count , int Pos , char* Password );
	virtual void RD_BuyItemResult		( BaseItemObject* Obj , DepartmentStoreBuyItemResultENUM Result , DepartmentStoreBaseInfoStruct& Item , int Count , int Pos );
			void _RD_BuyItemResult		( BaseItemObject* Obj , DepartmentStoreBuyItemResultENUM Result , DepartmentStoreBaseInfoStruct& Item , int Count , int Pos );
	virtual void RC_ImportAccountMoney	( BaseItemObject* Obj );
	virtual void RD_ImportAccountMoneyResult( int PlayerDBID , const char* Account_ID , int Money_Account , int Money_Bonus , int LockDay , int ImportGUID );

	virtual void RC_ExchangeItemRequest	( BaseItemObject* Obj , const char* ItemSerial , const char*  Password );
	virtual void RD_ExchangeItemResult	( int PlayerDBID , ItemFieldStruct& Item , int Money , int MoneyAccount , int ExchangeItemDBID , ItemExchangeResultENUM Result, int LockDay, int MapKey );

	virtual void RC_RunningMessageRequest( BaseItemObject* Obj );

	virtual void RC_MailGift				( BaseItemObject* Obj , int ItemGUID , int Count , const char* Password , MailBackGroundTypeENUM BackGroundType , const char* TargetName , const char* Title , const char* Content );
	virtual void RD_CheckMailGiftResult		( int MapKey , int TargetDBID , DepartmentStoreBaseInfoStruct& Item , CheckMailGiftResultTypeENUM Result );
			void _RD_CheckMailGiftResult	( int MapKey , int TargetDBID , DepartmentStoreBaseInfoStruct& Item , CheckMailGiftResultTypeENUM Result );

	virtual void RC_WebBagRequest			( BaseItemObject* Obj );
	virtual void RD_WebBagResult			( int PlayerDBID , int ItemGUID , ItemFieldStruct& Item , bool IsEnd , char* Message, int PriceType , int Price , int AddBonusMoney );

//	virtual void RL_BuyItemResult			( int FromWorldId , int FromNetID , int PlayerDBID , DepartmentStoreBuyItemResultENUM	Result , DepartmentStoreBaseInfoStruct&	Item , int Pos );
//	virtual void RBL_BuyItemResult			( int FromWorldId , int FromNetID , int PlayerDBID , int ItemGUID , bool Result , int Count );
	//DataCenter -> Local 老虎機資料
	virtual void RD_SlotMachineInfo			( SlotMachineBase base[] , int typeInfoCount , SlotMachineTypeBase typeInfo[] );
	//Client -> Local 要求老虎機資料
	virtual void RC_SlotMachineInfoRequest	( BaseItemObject* obj  );
	//Client -> Local 投幣
	virtual void RC_SlotMachinePlay			( BaseItemObject* obj  );
	virtual void RC_GetSlotMachineItem		( BaseItemObject* obj  );
	
	static void  SlotMachineFreeCountProc	( RoleDataEx* role );

	static bool  GetSlotMachineItemInfo( RoleDataEx* role , int itemID[] , int itemCount[] );

public:
	static void BuyItemBalanceCheckResult(void* userdata);
	static void MailGiftBalanceCheckResult(void* userdata);
	static void WebBagBalanceCheckResult(void* userdata);

protected:
	static int _OnTimeProc_BuyItem(SchedularInfo* obj, void* inputclass);
};

