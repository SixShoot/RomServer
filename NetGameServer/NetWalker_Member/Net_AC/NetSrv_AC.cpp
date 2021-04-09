#include "../NetWakerGSrvInc.h"
#include "NetSrv_AC.h"

//-------------------------------------------------------------------
NetSrv_AC* NetSrv_AC::This = NULL;
//-------------------------------------------------------------------
bool NetSrv_AC::_Init()
{
	Srv_NetCliReg( PG_AC_CtoL_SearchItem		 );
	Srv_NetCliReg( PG_AC_CtoL_GetNextSearchItem	 );
	Srv_NetCliReg( PG_AC_CtoL_GetMyACItemInfo	 );
	Srv_NetCliReg( PG_AC_CtoL_BuyItem			 );
	Srv_NetCliReg( PG_AC_CtoL_SellItem			 );
	Srv_NetCliReg( PG_AC_CtoL_CancelSellItem	 );
	Srv_NetCliReg( PG_AC_CtoL_CloseAC			 );
	Srv_NetCliReg( PG_AC_CtoL_ItemHistoryRequest );
	Srv_NetCliReg( PG_AC_CtoL_SellMoney			 );

	_Net->RegOnSrvPacketFunction			( EM_PG_AC_DCtoL_BuyItemResult	, _PG_AC_DCtoL_BuyItemResult	);
	_Net->RegOnSrvPacketFunction			( EM_PG_AC_DCtoL_SellItemResult	, _PG_AC_DCtoL_SellItemResult	);

    return true;
}
//-------------------------------------------------------------------
bool NetSrv_AC::_Release()
{
    return true;
}

void NetSrv_AC::_PG_AC_CtoL_ItemHistoryRequest	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_AC_CtoL_ItemHistoryRequest* pg =(PG_AC_CtoL_ItemHistoryRequest*)data;

	This->RC_ItemHistoryRequest( Obj , pg->OrgObjID , pg->Inherent );
}

void NetSrv_AC::_PG_AC_CtoL_SearchItem			( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_AC_CtoL_SearchItem* pg =(PG_AC_CtoL_SearchItem*)data;

	This->RC_SearchItem( Obj , pg->Data );
}
void NetSrv_AC::_PG_AC_CtoL_GetNextSearchItem	( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_AC_CtoL_GetNextSearchItem* pg =(PG_AC_CtoL_GetNextSearchItem*)data;

	This->RC_GetNextSearchItem( Obj );
}
void NetSrv_AC::_PG_AC_CtoL_GetMyACItemInfo		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_AC_CtoL_GetMyACItemInfo* pg =(PG_AC_CtoL_GetMyACItemInfo*)data;

	This->RC_GetMyACItemInfo( Obj );
}
void NetSrv_AC::_PG_AC_CtoL_BuyItem				( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_AC_CtoL_BuyItem* pg =(PG_AC_CtoL_BuyItem*)data;

	This->RC_BuyItem( Obj , pg->ACItemDBID , pg->Prices , pg->Type , pg->Password.Begin() );
}
void NetSrv_AC::_PG_AC_CtoL_SellItem			( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_AC_CtoL_SellItem* pg =(PG_AC_CtoL_SellItem*)data;

	This->RC_SellItem( Obj , pg->BodyPos , pg->Item , pg->BuyOutPrices , pg->Prices , pg->Type , pg->LiveTime );
}
void NetSrv_AC::_PG_AC_CtoL_SellMoney			( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_AC_CtoL_SellMoney* pg =(PG_AC_CtoL_SellMoney*)data;

	This->RC_SellMoney( Obj , pg->SellType , pg->SellMoney, pg->BuyOutPrices , pg->Prices , pg->Type , pg->LiveTime , (char*)pg->Password.Begin() );
}
void NetSrv_AC::_PG_AC_CtoL_CancelSellItem		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_AC_CtoL_CancelSellItem* pg =(PG_AC_CtoL_CancelSellItem*)data;

	This->RC_CancelSellItem( Obj , pg->ACItemDBID );
}
void NetSrv_AC::_PG_AC_CtoL_CloseAC				( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_AC_CtoL_CloseAC* pg =(PG_AC_CtoL_CloseAC*)data;

	This->RC_CloseAC( Obj );
}

void NetSrv_AC::_PG_AC_DCtoL_BuyItemResult		( int sockid , int size , void* data )
{
	PG_AC_DCtoL_BuyItemResult* pg =(PG_AC_DCtoL_BuyItemResult*)data;

	This->RDC_BuyItemResult( pg->Result , pg->BuyerDBID , pg->ACItemDBID , pg->Prices , pg->Type , pg->State );
}
void NetSrv_AC::_PG_AC_DCtoL_SellItemResult		( int sockid , int size , void* data )
{
	PG_AC_DCtoL_SellItemResult* pg =(PG_AC_DCtoL_SellItemResult*)data;

	This->RDC_SellItemResult( pg->Result , pg->SellerDBID , pg->Item , pg->TaxMoney );
}
//----------------------------------------------------------------------------------------------------------------------------------
void NetSrv_AC::SDC_SearchItem			( int DBID , ACSearchConditionStruct&	Data )
{
	PG_AC_LtoDC_SearchItem Send;
	Send.DBID = DBID;
	Send.Data = Data;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_AC::SDC_GetNextSearchItem	( int DBID  ) 
{
	PG_AC_LtoDC_GetNextSearchItem Send;
	Send.DBID = DBID;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_AC::SDC_GetMyACItemInfo	( int DBID )
{
	PG_AC_LtoDC_GetMyACItemInfo Send;
	Send.DBID = DBID;
	
	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_AC::SDC_BuyItem			( int DBID , int ACItemDBID , int Prices , PriceTypeENUM	PricesType )
{
	PG_AC_LtoDC_BuyItem Send;

	Send.DBID = DBID;				//¶RªÌ
	Send.ACItemDBID = ACItemDBID;	//­n¶Rªºª««~
	Send.Prices = Prices;			//©Ò¥Xªº»ù¿ú(¦pªG µ¥©óª½±µÁÊ¶R»ù ¥Nªíª½±µÁÊ¶R)
	Send.Type = PricesType;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_AC::SDC_SellItem			( int SellerDBID , ItemFieldStruct& Item , int Prices , int BuyOutPrices , PriceTypeENUM PricesType , int LiveTime , int TaxMoney )
{
	PG_AC_LtoDC_SellItem Send;

	Send.Item = Item;					//½æªºªF¦è	
	Send.Prices = Prices;				//°ò¥»»ù
	Send.DBID = SellerDBID;
	Send.LiveTime = LiveTime;
	Send.TaxMoney = TaxMoney;
	Send.BuyOutPrices = BuyOutPrices;	//ª½±µÁÊ¶R»ù
	Send.Type = PricesType;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_AC::SDC_CancelSellItem		( int DBID , int ACItemDBID )
{
	PG_AC_LtoDC_CancelSellItem Send;
	Send.DBID = DBID;
	Send.ACItemDBID = ACItemDBID;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_AC::SDC_CloseAC			( int DBID )
{

	PG_AC_LtoDC_CloseAC Send;
	Send.DBID = DBID;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_AC::SDC_OpenAC				( int DBID , int NpcGUID )
{
	PG_AC_LtoDC_OpenAC Send;
	Send.DBID = DBID;
	Send.NpcGUID = NpcGUID;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_AC::SDC_ItemHistoryRequest	( int DBID , int OrgObjID , int Inherent )
{
	PG_AC_LtoDC_ItemHistoryRequest Send;
	Send.DBID = DBID;
	Send.OrgObjID = OrgObjID;
	Send.Inherent = Inherent;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
//----------------------------------------------------------------------------------------------------------------------------------
/*
void NetSrv_AC::SC_OpenAC				( int GUID )
{
	PG_AC_LtoC_OpenAC Send;

	Global::SendToCli_ByGUID( GUID , sizeof( Send ) , &Send );

}
*/
void NetSrv_AC::SC_BuyItemResult		( int GUID , int ACItemDBID , int Prices , PriceTypeENUM PricesType , ACItemStateENUM State , bool Result )
{
	PG_AC_LtoC_BuyItemResult Send;
	Send.ACItemDBID = ACItemDBID;
	Send.Prices = Prices;
	Send.Result = Result;
	Send.State = State;
	Send.Type = PricesType;

	Global::SendToCli_ByGUID( GUID , sizeof( Send ) , &Send );
}
void NetSrv_AC::SC_SellItemResult		( int GUID , ACItemInfoStruct& Item , ACSellItemResultENUM Result )
{
	PG_AC_LtoC_SellItemResult Send;
	Send.Item = Item;				//½æªºªF¦è	
	Send.Result = Result;			//¬O§_¦¨¥

	Global::SendToCli_ByGUID( GUID , sizeof( Send ) , &Send );
}

void NetSrv_AC::SC_AcAlreadyOpen		( int GUID , int TargetGUID )
{
	PG_AC_LtoC_AcAlreadyOpen Send;
	Send.TargtetNPC = TargetGUID;
	Global::SendToCli_ByGUID( GUID , sizeof( Send ) , &Send );
}
//----------------------------------------------------------------------------------------------------------------------------------
void NetSrv_AC::SC_SellMoneyResult		( int GUID , SellMoneyResultENUM Result , ACItemInfoStruct* Item  )
{
	PG_AC_LtoC_SellMoneyResult Send;
	Send.Result = Result;
	if( Item != NULL )
		Send.Item = *Item;
	Global::SendToCli_ByGUID( GUID , sizeof( Send ) , &Send );
}