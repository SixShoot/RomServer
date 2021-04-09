#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_DepartmentStore.h"

class NetSrv_DepartmentStore : public Global
{
protected:
    //-------------------------------------------------------------------    
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------

	static void _PG_DepartmentStore_CtoL_ShopFunctionRequest		( int NetID , int Size , void* Data );
	static void _PG_DepartmentStore_CtoL_SellTypeRequest			( int NetID , int Size , void* Data );
	static void _PG_DepartmentStore_CtoL_ShopInfoRequest			( int NetID , int Size , void* Data );
	static void _PG_DepartmentStore_CtoL_BuyItem					( int NetID , int Size , void* Data );
	static void _PG_DepartmentStore_DtoL_BuyItemResult				( int NetID , int Size , void* Data );

	static void _PG_DepartmentStore_CtoL_ImportAccountMoney			( int NetID , int Size , void* Data );
	static void _PG_DepartmentStore_DtoL_ImportAccountMoneyResult	( int NetID , int Size , void* Data );

	static void _PG_DepartmentStore_CtoL_ExchangeItemRequest		( int NetID , int Size , void* Data );
	static void _PG_DepartmentStore_DtoL_ExchangeItemResult			( int NetID , int Size , void* Data );
	static void _PG_DepartmentStore_CtoL_RunningMessageRequest		( int NetID , int Size , void* Data );

	static void _PG_DepartmentStore_CtoL_MailGift					( int NetID , int Size , void* Data );
	static void _PG_DepartmentStore_DtoL_CheckMailGiftResult		( int NetID , int Size , void* Data );

	static void _PG_DepartmentStore_CtoL_WebBagRequest				( int NetID , int Size , void* Data );
	static void _PG_DepartmentStore_DtoL_WebBagResult				( int NetID , int Size , void* Data );

	static void _PG_DepartmentStore_DtoL_SlotMachineInfo			( int NetID , int size , void* data );
	static void _PG_DepartmentStore_CtoL_SlotMachineInfoRequest		( int NetID , int size , void* data );
	static void _PG_DepartmentStore_CtoL_SlotMachinePlay			( int NetID , int size , void* data );
	static void _PG_DepartmentStore_CtoL_GetSlotMachineItem			( int NetID , int size , void* data );
//	static void _PG_CliConnect_LtoBL_BuyItemResult					( int FromWorldId , int FromNetID , int Size , void* Data );
//	static void _PG_DepartmentStore_BLtoL_BuyItemResult				( int FromWorldId , int FromNetID , int Size , void* Data );
public:    
	static NetSrv_DepartmentStore* This;

	static void SC_ShopFunctionResult( int SendToID , int FunctionID , bool Result );
	static void SC_BuyItemResult	( int SendToID , DepartmentStoreBuyItemResultENUM Result );
	static void SD_BuyItem			( int ItemGUID , int PlayerDBID , int PlayerAcountMoney , int Count , int Pos );
	static void SD_ShopInfoRequest	( int CliSockID , int CliProxyID , int SellType );
	static void SD_SellTypeRequest	( int CliSockID , int CliProxyID );
	static void SD_BuyItemResult	( int PlayerDBID , int ItemGUID , bool Result , int Count );

	static void SD_ImportAccountMoney		( int PlayerDBID , const char* Account );
	static void SD_ImportAccountMoneyFailed	( int PlayerDBID , const char* Account , int Money_Account , int MoneyBonus , int ImportGUID );

	static void SC_ImportAccountMoneyResult	( int SendID  , int Money_Account , int Money_Bonus );

	static void SD_ExchangeItemRequest	( int PlayerDBID , const char* Account , const char* ItemSerial , const char* Password, int MapKey );
	static void SD_ExchangeItemResultOK	( int PlayerDBID , int ExchangeItemDBID , bool Result , bool IsDataError );
	static void SC_ExchangeItemResult	( int PlayerDBID , ItemFieldStruct& Item , int Money , int MoneyAccount , ItemExchangeResultENUM Result );


	virtual void RC_ShopFunctionReques	( BaseItemObject* Obj , int FunctionID ) = 0;
	virtual void RC_SellTypeRequest		( BaseItemObject* Obj ) = 0;
	virtual void RC_ShopInfoRequest		( BaseItemObject* Obj , int SellType ) = 0;
	virtual void RC_BuyItem				( BaseItemObject* Obj , int ItemGUID , int Count , int Pos , char* Password ) = 0;
	virtual void RD_BuyItemResult		( BaseItemObject* Obj , DepartmentStoreBuyItemResultENUM Result , DepartmentStoreBaseInfoStruct& Item , int Count , int Pos ) = 0;
	virtual void RC_ImportAccountMoney	( BaseItemObject* Obj ) = 0;
	virtual void RD_ImportAccountMoneyResult( int PlayerDBID , const char* Account_ID , int Money_Account , int Money_Bonus , int LockDay , int ImportGUID ) = 0;

	virtual void RC_ExchangeItemRequest	( BaseItemObject* Obj , const char* ItemSerial , const char* Password ) = 0;
	virtual void RD_ExchangeItemResult	( int PlayerDBID , ItemFieldStruct& Item , int Money , int MoneyAccount , int ExchangeItemDBID , ItemExchangeResultENUM Result, int LockDay, int MapKey) = 0;

	//////////////////////////////////////////////////////////////////////////
	//跑馬登資訊
	virtual void RC_RunningMessageRequest( BaseItemObject* Obj ) = 0;
	static  void SD_RunningMessageRequest( int PlayerDBID );

	//////////////////////////////////////////////////////////////////////////
	//寄送商城物品
	virtual void RC_MailGift				( BaseItemObject*Obj , int ItemGUID , int Count , const char* Password , MailBackGroundTypeENUM BackGroundType , const char* TargetName , const char* Title , const char* Content ) = 0;
	virtual void RD_CheckMailGiftResult		( int MapKey , int TargetDBID , DepartmentStoreBaseInfoStruct& Item , CheckMailGiftResultTypeENUM Result ) = 0;

	static  void SD_CheckMailGift			( int PlayerDBID , int ItemGUID , int Count , const char* TargetName , int MapKey );
//	static  void SD_MailGift				( int ItemGUID , int Count , int Pos  );
	static  void SC_MailGiftResult			( int SendToID , CheckMailGiftResultTypeENUM Result );

	//////////////////////////////////////////////////////////////////////////
	static void SD_WebBagRequest			( RoleDataEx* Role );
	virtual void RC_WebBagRequest			( BaseItemObject* Obj ) = 0;
	virtual void RD_WebBagResult			( int PlayerDBID , int ItemGUID , ItemFieldStruct& Item , bool IsEnd , char* Message , int PriceType , int Price , int AddBonusMoney ) = 0;
	static void SC_WebBagResult				( int SendToID, int OrgObjID, int Count );
	//////////////////////////////////////////////////////////////////////////
	/*
	static void SBL_BuyItemResult			( int ToWorldID , int ToGSrvID , int PlayerDBID , DepartmentStoreBuyItemResultENUM	Result , DepartmentStoreBaseInfoStruct&	Item , int Pos );
	static void SL_BuyItemResult			( int ToWorldID , int ToNetID , int PlayerDBID , int ItemGUID , bool Result , int Count );
	virtual void RL_BuyItemResult			( int FromWorldId , int FromNetID , int PlayerDBID , DepartmentStoreBuyItemResultENUM	Result , DepartmentStoreBaseInfoStruct&	Item , int Pos ) = 0;
	virtual void RBL_BuyItemResult			( int FromWorldId , int FromNetID , int PlayerDBID , int ItemGUID , bool Result , int Count ) = 0;
	*/
	//////////////////////////////////////////////////////////////////////////
	//DataCenter -> Local 老虎機資料
	virtual void RD_SlotMachineInfo( SlotMachineBase base[] , int typeInfoCount , SlotMachineTypeBase typeInfo[] ) = 0;
	//Local -> Client 通知所有人 老虎機資料有改變
	static void SC_All_SlotMachineInfoChange( );
	//Client -> Local 要求老虎機資料
	virtual void RC_SlotMachineInfoRequest( BaseItemObject* obj  ) = 0;
	//Local -> Client 通知老虎機資料
	static void SC_SlotMachineInfo		( int SendToID , int itemID[] , int itemCount[] );
	//Client -> Local 投幣
	virtual void RC_SlotMachinePlay		( BaseItemObject* obj  ) = 0;
	//Local -> Client 結果
	void SC_SlotMachinePlayResult		( int SendToID , SlotMachinePlayResultENUM result , int itemID , int itemCount );
	void SC_SlotMachinePlayResultFailed	( int SendToID , SlotMachinePlayResultENUM result );
	virtual void RC_GetSlotMachineItem	( BaseItemObject* obj  ) = 0;
};	

