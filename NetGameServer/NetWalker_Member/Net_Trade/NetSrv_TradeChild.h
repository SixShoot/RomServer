#pragma once

#include "NetSrv_Trade.h"

class NetSrv_TradeChild : public NetSrv_Trade
{
public:
    static bool Init();
    static bool Release();


	static void S_TradeState( RoleDataEx* Owner , RoleDataEx* Target );
    //---------------------------------------------------------------------------
    // Client 端要求的事件
    //---------------------------------------------------------------------------
    //要求交易
    virtual void R_RequestTrade			( BaseItemObject*	Obj , int TargetID );
    //同意交易
    virtual void R_AgreeTrade			( BaseItemObject*	Obj , int TargetID , bool Result , int ClientResult );
    //放物品	
    virtual void R_PutItem				( BaseItemObject*	Obj , int Pos , int ItemPos , ItemFieldStruct *Item );
	//放物金錢
    virtual void R_PutMoney				( BaseItemObject*	Obj , int Money , int Money_Account );
    //確定
    virtual void R_ItemOK				( BaseItemObject*	Obj );
    //決定
    virtual void R_FinalOK				( BaseItemObject*	Obj , const char* Password );
    //停止交易
    virtual void R_StopTrade			( BaseItemObject*	Obj );
	//取消與目標交易要請
	virtual void R_CancelTradeRequest	( BaseItemObject*	Obj , int TargetID );
	
};

