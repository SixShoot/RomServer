#pragma once

#include "NetSrv_Trade.h"

class NetSrv_TradeChild : public NetSrv_Trade
{
public:
    static bool Init();
    static bool Release();


	static void S_TradeState( RoleDataEx* Owner , RoleDataEx* Target );
    //---------------------------------------------------------------------------
    // Client �ݭn�D���ƥ�
    //---------------------------------------------------------------------------
    //�n�D���
    virtual void R_RequestTrade			( BaseItemObject*	Obj , int TargetID );
    //�P�N���
    virtual void R_AgreeTrade			( BaseItemObject*	Obj , int TargetID , bool Result , int ClientResult );
    //�񪫫~	
    virtual void R_PutItem				( BaseItemObject*	Obj , int Pos , int ItemPos , ItemFieldStruct *Item );
	//�񪫪���
    virtual void R_PutMoney				( BaseItemObject*	Obj , int Money , int Money_Account );
    //�T�w
    virtual void R_ItemOK				( BaseItemObject*	Obj );
    //�M�w
    virtual void R_FinalOK				( BaseItemObject*	Obj , const char* Password );
    //������
    virtual void R_StopTrade			( BaseItemObject*	Obj );
	//�����P�ؼХ���n��
	virtual void R_CancelTradeRequest	( BaseItemObject*	Obj , int TargetID );
	
};

