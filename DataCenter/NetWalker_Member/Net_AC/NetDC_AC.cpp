#include "PG/PG_AC.h"
#include "NetDC_AC.h"
//-------------------------------------------------------------------
CNetDC_AC*	CNetDC_AC::This = NULL;
//-------------------------------------------------------------------
bool CNetDC_AC::_Release()
{

	return false;
}
//-------------------------------------------------------------------
bool CNetDC_AC::_Init()
{
	_Net->RegOnSrvPacketFunction( EM_PG_AC_LtoDC_SearchItem				, _PG_AC_LtoDC_SearchItem			);
	_Net->RegOnSrvPacketFunction( EM_PG_AC_LtoDC_GetNextSearchItem		, _PG_AC_LtoDC_GetNextSearchItem	);
	_Net->RegOnSrvPacketFunction( EM_PG_AC_LtoDC_GetMyACItemInfo		, _PG_AC_LtoDC_GetMyACItemInfo		);
	_Net->RegOnSrvPacketFunction( EM_PG_AC_LtoDC_BuyItem				, _PG_AC_LtoDC_BuyItem				);
	_Net->RegOnSrvPacketFunction( EM_PG_AC_LtoDC_SellItem				, _PG_AC_LtoDC_SellItem				);
	_Net->RegOnSrvPacketFunction( EM_PG_AC_LtoDC_CancelSellItem			, _PG_AC_LtoDC_CancelSellItem		);
	_Net->RegOnSrvPacketFunction( EM_PG_AC_LtoDC_CloseAC				, _PG_AC_LtoDC_CloseAC				);
	_Net->RegOnSrvPacketFunction( EM_PG_AC_LtoDC_OpenAC					, _PG_AC_LtoDC_OpenAC				);
	_Net->RegOnSrvPacketFunction( EM_PG_AC_LtoDC_ItemHistoryRequest		, _PG_AC_LtoDC_ItemHistoryRequest	);
	return true;
}
//-------------------------------------------------------------------
void CNetDC_AC::_PG_AC_LtoDC_ItemHistoryRequest			( int ServerID , int Size , void* Data )
{
	PG_AC_LtoDC_ItemHistoryRequest* PG = (PG_AC_LtoDC_ItemHistoryRequest*) Data;
	This->RL_ItemHistoryRequest( PG->DBID , PG->OrgObjID , PG->Inherent );
}
void CNetDC_AC::_PG_AC_LtoDC_SearchItem			( int ServerID , int Size , void* Data )
{
	PG_AC_LtoDC_SearchItem* PG = (PG_AC_LtoDC_SearchItem*) Data;
	This->RL_SearchItem( PG->DBID , PG->Data );
}
void CNetDC_AC::_PG_AC_LtoDC_GetNextSearchItem	( int ServerID , int Size , void* Data )
{
	PG_AC_LtoDC_GetNextSearchItem* PG = (PG_AC_LtoDC_GetNextSearchItem*) Data;
	This->RL_GetNextSearchItem( PG->DBID );

}
void CNetDC_AC::_PG_AC_LtoDC_GetMyACItemInfo	( int ServerID , int Size , void* Data )
{
	PG_AC_LtoDC_GetMyACItemInfo* PG = (PG_AC_LtoDC_GetMyACItemInfo*) Data;
	This->RL_GetMyACItemInfo( PG->DBID );
}
void CNetDC_AC::_PG_AC_LtoDC_BuyItem			( int ServerID , int Size , void* Data )
{
	PG_AC_LtoDC_BuyItem* PG = (PG_AC_LtoDC_BuyItem*) Data;
	This->RL_BuyItem( ServerID , PG->DBID , PG->ACItemDBID , PG->Prices , PG->Type );
}
void CNetDC_AC::_PG_AC_LtoDC_SellItem			( int ServerID , int Size , void* Data )
{
	PG_AC_LtoDC_SellItem* PG = (PG_AC_LtoDC_SellItem*) Data;
	This->RL_SellItem( ServerID , PG->DBID , PG->Item , PG->Prices , PG->BuyOutPrices , PG->Type , PG->LiveTime , PG->TaxMoney );
}
void CNetDC_AC::_PG_AC_LtoDC_CancelSellItem		( int ServerID , int Size , void* Data )
{
	PG_AC_LtoDC_CancelSellItem* PG = (PG_AC_LtoDC_CancelSellItem*) Data;
	This->RL_CancelSellItem( PG->DBID , PG->ACItemDBID );	
}
void CNetDC_AC::_PG_AC_LtoDC_CloseAC ( int ServerID , int Size , void* Data )
{
	PG_AC_LtoDC_CloseAC* PG = (PG_AC_LtoDC_CloseAC*) Data;
	This->RL_CloseAC( PG->DBID );	
}
void CNetDC_AC::_PG_AC_LtoDC_OpenAC				( int ServerID , int Size , void* Data )
{
	PG_AC_LtoDC_OpenAC* PG = (PG_AC_LtoDC_OpenAC*) Data;
	This->RL_OpenAC( PG->DBID , PG->NpcGUID );	
}
//----------------------------------------------------	----------------------------------------------------------------------------------		
//--------------------------------------------------------------------------------------------------------------------------------------		
void CNetDC_AC::SC_SearchItemCountResult	( int DBID , int BeginIndex , int MaxCount , int Count )
{
	PG_AC_DCtoC_SearchItemCountResult Send;
	Send.BeginIndex = BeginIndex;
	Send.MaxCount = MaxCount;
	Send.Count = Count;

	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}

void CNetDC_AC::SC_CloseAC					( int DBID )
{
	PG_AC_DCtoC_CloseAC Send;

	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}

void CNetDC_AC::SC_OpenAC					( int DBID , int NpcGUID , bool Result )
{
	PG_AC_DCtoC_OpenAC Send;
	Send.NpcGUID = NpcGUID;
	Send.Result = Result;

	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}

void CNetDC_AC::SC_LostCompetitiveItem		( int DBID , int ACItemDBID , ItemFieldStruct& Item )
{
	PG_AC_DCtoC_LostCompetitiveItem Send;
	Send.ACItemDBID = ACItemDBID;
	Send.Item = Item;
	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}

void CNetDC_AC::SC_SearchItemResult			( int DBID , int Index , ACItemInfoStruct& Data , char* SellerName , int BuyerDBID )
{
	PG_AC_DCtoC_SearchItemResult Send;
	Send.Index = Index;
	Send.Data = Data;
	Send.SellerName = SellerName;
	Send.BuyerDBID = BuyerDBID;

	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}
void CNetDC_AC::SC_FixSearchItemResult		( int DBID , int ACItemDBID , int NewPrices , PriceTypeENUM	PricesType ,ACItemStateENUM State , int BuyerDBID )
{
	PG_AC_DCtoC_FixSearchItemResult Send;
	Send.ACItemDBID = ACItemDBID;
	Send.State = State;
	Send.NewPrices = NewPrices;
	Send.BuyerDBID = BuyerDBID;
	Send.Type = PricesType;

	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}
void CNetDC_AC::SC_FixBuyOrderItem		( int DBID , int ACItemDBID , int NewPrices , PriceTypeENUM	PricesType ,ACItemStateENUM State , int BuyerDBID )
{
	PG_AC_DCtoC_FixBuyOrderItem Send;
	Send.ACItemDBID = ACItemDBID;
	Send.State = State;
	Send.NewPrices = NewPrices;
	Send.BuyerDBID = BuyerDBID;
	Send.Type	   = PricesType;

	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}
void CNetDC_AC::SC_FixSellOrderItem		( int DBID , int ACItemDBID , int NewPrices , PriceTypeENUM	PricesType , ACItemStateENUM	State , char* BuyerName )
{
	PG_AC_DCtoC_FixSellOrderItem Send;
	Send.ACItemDBID = ACItemDBID;
	Send.State = State;
	Send.NewPrices = NewPrices;
	Send.BuyerName = BuyerName;
	Send.Type	   = PricesType;

	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}
void CNetDC_AC::SC_CancelSellItemResult		( int DBID , int ACItemDBID , bool Result )
{
	PG_AC_DCtoC_CancelSellItemResult Send;
	Send.ACItemDBID = ACItemDBID;
	Send.Result = Result;
	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}

void CNetDC_AC::SC_SelfCompetitive_BuyItem	( int DBID , int BuyerDBID , ACItemInfoStruct& Data , char* SellerName )
{
	PG_AC_DCtoC_SelfCompetitive_BuyItem Send;
	Send.BuyerDBID = BuyerDBID;
	Send.Data = Data;
	Send.SellerName = SellerName;

	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}
void CNetDC_AC::SC_SelfCompetitive_ItemDataEnd	( int DBID  )
{
	PG_AC_DCtoC_SelfCompetitive_ItemDataEnd Send;
	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}

void CNetDC_AC::SC_ItemHistoryList			( int DBID , vector<DB_ACItemHistoryStruct>& HistoryList )
{
	PG_AC_DCtoC_ItemHistoryListResult Send;
	Send.Count = __min( (int)HistoryList.size() , _MAX_HISTORY_ITEM_COUNT_ );

	for( int i = 0 ; i < Send.Count ; i++ )
	{
		Send.ItemHistory[ i ]  = HistoryList[i];
	}

	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}
void CNetDC_AC::SC_SelfCompetitive_SellItem	( int DBID , ACItemInfoStruct& Data , char* BuyerName )
{
	PG_AC_DCtoC_SelfCompetitive_SellItem Send;
	Send.BuyerName = BuyerName;
	Send.Data = Data; 

	Global::SendToCli_ByDBID( DBID , sizeof( Send ) , &Send );
}

void CNetDC_AC::SL_BuyItemResult			( int SrvSockID , int BuyerDBID , int ACItemDBID , int Prices , PriceTypeENUM PricesType , ACItemStateENUM	State  , bool Result )
{
	PG_AC_DCtoL_BuyItemResult Send;
	Send.Result = Result;
	Send.Prices = Prices;
	Send.Type = PricesType;
	Send.ACItemDBID = ACItemDBID;
	Send.BuyerDBID = BuyerDBID;
	Send.State = State;
	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );
}
void CNetDC_AC::SL_SellItemResult			( int SrvSockID , int SellerDBID , ACItemInfoStruct& Item , int TaxMoney , ACSellItemResultENUM Result )
{
	PG_AC_DCtoL_SellItemResult Send;
	Send.Item = Item;
	Send.SellerDBID = SellerDBID;
	Send.TaxMoney = TaxMoney;
	Send.Result = Result;

	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );
}
void CNetDC_AC::SC_AcTradeOk				( int SendDBID , const char* BuyerName ,  const char* SellName , PriceTypeENUM PriceType , int Money , int Tax ,ItemFieldStruct& Item )
{
	PG_AC_DtoC_AcTradeOk Send;
	Send.Item = Item;
	Send.SellerName = SellName;
	Send.BuyerName	= BuyerName;
	Send.PriceType	= PriceType;
	Send.Money		= Money;
	Send.Tax		= Tax;

	Global::SendToCli_ByDBID( SendDBID , sizeof( Send) , &Send );
}
//--------------------------------------------------------------------------------------------------------------------------------------		
