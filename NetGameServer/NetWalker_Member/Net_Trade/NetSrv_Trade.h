#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_Trade.h"

class NetSrv_Trade : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Trade* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------

    static void _PG_Trade_CtoL_RequestTrade     	( int sockid , int size , void* data );    
    static void _PG_Trade_CtoL_AgreeTrade       	( int sockid , int size , void* data );    
    static void _PG_Trade_CtoL_PutItem          	( int sockid , int size , void* data );    
    static void _PG_Trade_CtoL_PutMoney         	( int sockid , int size , void* data );    
    static void _PG_Trade_CtoL_ItemOK           	( int sockid , int size , void* data );    
    static void _PG_Trade_CtoL_FinalOK          	( int sockid , int size , void* data );    
    static void _PG_Trade_CtoL_StopTrade        	( int sockid , int size , void* data );    
	static void _PG_Trade_CtoL_CancelTradeRequest	( int sockid , int size , void* data );
public:    

    static void	S_ClientRequestTrade	( int SendID , int TargetID );
    static void	S_OpenTrade			    ( int SendID , int TargetID , const char* Name );
    static void	S_TargetPutItem		    ( int SendID , int Pos , ItemFieldStruct* Item );
    static void	S_TargetPutMoney		( int SendID , int Money , int Money_Account );
    static void	S_OwnerPutMoney		    ( int SendID , int Money , int Money_Account );
    static void	S_TargetItemOK		    ( int SendID );
    static void	S_CancelItemOK		    ( int SendID );
    static void	S_FinalOKEnable		    ( int SendID );
    static void	S_TargetFinalOK		    ( int SendID );
    static void	S_ClearTradeItem		( int SendID );
    static void	S_TradeItemOK			( int SendID );
    static void	S_TradeItemFalse		( int SendID );
    static void	S_PutItemFalse  		( int SendID );
    static void	S_PutItemOK     		( int SendID );

	static void	S_RequestTradeError		( int SendID );
	static void	S_DisagreeTrade			( int SendID , int ClientResult );
	static void	S_StopTrade				( int SendID , int TargetID );
	static void	S_TradeState			( int SendID , bool TradeOK , bool FinalOK , bool TargetTradeOK , bool TargetFinalOK );
	static void S_CancelTradeRequest	( int SendID , int TargetID );
	static void S_FinalOkFailed			( int SendID , FinalOKFailedENUM Result );
    //---------------------------------------------------------------------------
    // Client 端要求的事件
    //---------------------------------------------------------------------------
    virtual void R_RequestTrade			( BaseItemObject*	Obj , int TargetID ) = 0;
    virtual void R_AgreeTrade			( BaseItemObject*	Obj , int TargetID , bool Result , int ClientResult ) = 0;
    virtual void R_PutItem				( BaseItemObject*	Obj , int Pos , int ItemPos , ItemFieldStruct *Item ) = 0;
    virtual void R_PutMoney				( BaseItemObject*	Obj , int Money , int Money_Account ) = 0;
    virtual void R_ItemOK				( BaseItemObject*	Obj ) = 0;
    virtual void R_FinalOK				( BaseItemObject*	Obj , const char* Password ) = 0;
    virtual void R_StopTrade			( BaseItemObject*	Obj ) = 0;
	virtual void R_CancelTradeRequest	( BaseItemObject*	Obj , int TargetID ) = 0;
};

