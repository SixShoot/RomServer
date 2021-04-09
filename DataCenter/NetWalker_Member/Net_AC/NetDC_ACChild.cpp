#include "NetDC_ACChild.h"	
#include "ReaderClass/CreateDBCmdClass.h"
#include "ReaderClass/DbSqlExecClass.h"
#include "RoleData/ObjectDataClass.h"
#include "../Net_Mail/NetDC_MailChild.h"
#include "smallobj/smallobj.h"
#define SELL_ACITEM_THREADID	12
//-------------------------------------------------------------------
bool									CNetDC_ACChild::_IsInitReady	= false;
map< int , DB_ACItemStructPtr >			CNetDC_ACChild::_AllItemList;			//所有拍賣的物品
map< int , PlayerSelectManageStruct >	CNetDC_ACChild::_PlayerList;			//所有有要求查尋的玩家資訊
map< ACItemHistoryTypeStruct , vector<DB_ACItemHistoryStruct> >	CNetDC_ACChild::_HistoryList;	//歷史記錄
vector< int >							CNetDC_ACChild::_WaitDelACHistoryList;	//等待刪除
vector< int >							CNetDC_ACChild::_ProcDelACHistoryList;	//正要刪除(只有定時處理程序能用)
int										CNetDC_ACChild::_AcSelectMaxCount = 500;
//-------------------------------------------------------------------
bool    CNetDC_ACChild::Init()
{
	CNetDC_AC::_Init();
	if( This != NULL)
		return false;
	This = NEW( CNetDC_ACChild );
	_RD_ACItemDB->SqlCmd( 0 , _SQLCmdInitProc , _SQLCmdInitProcResult , NULL	, NULL );
	printf( "\n");
	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 1000 );
		_RD_ACItemDB->Process( );
		switch( i % 4 )
		{
		case 0:
			printf( "\rAC Info Loading...../        " );
			break;
		case 1:
			printf( "\rAC Info Loading.....|        " );
			break;
		case 2:			
			printf( "\rAC Info Loading.....\\        " );
			break;
		case 3:
			printf( "\rAC Info Loading.....-        " );
			break;
		}
		if( _IsInitReady != false )
			break;
	}
	printf( "\n");
	if( Global::Ini()->Int( "IsDisableAC" ) == 0 )
	{
		Schedular()->Push( _OnTimeProc , 1000*60 , This , NULL );	
	}
	if( Ini()->Int( "AC_Select_Item_MaxCount", _AcSelectMaxCount ) == false )
	{
		_AcSelectMaxCount = 500;
	}
	return true;
}
//--------------------------------------------------------------------------------------
bool    CNetDC_ACChild::Release()
{
	if( This == NULL )
		return false;
	delete This;
	This = NULL;
	CNetDC_AC::_Release();
	_AllItemList.clear();	//所有拍賣的物品
	_PlayerList.clear();	//所有有要求查尋的玩家資訊
	_HistoryList.clear();	//歷史記錄
	return true;
}
//--------------------------------------------------------------------------------------
int  CNetDC_ACChild::_OnTimeProc( SchedularInfo* Obj , void* InputClass )
{
	if( Global::_IsDestory != false )
		return 0;
	static int TimeCount = 0;
	TimeCount++;
	MailBaseInfoStruct MailBaseInfo;
	//定時更新所有完家搜尋資料
	//找出所有擁有此物件的玩家
	map< int , PlayerSelectManageStruct >::iterator Iter;
	set< int >::iterator	SetIter;
	for( Iter = _PlayerList.begin() ; Iter != _PlayerList.end() ; )
	{
		PlayerSelectManageStruct&	PlayerData = Iter->second;
		if( PlayerData.LiveTime < clock() )
		{
			SC_CloseAC( PlayerData.PlayerDBID );
			Iter = _PlayerList.erase( Iter );
		}
		else
		{
			Iter++ ;
		}
	}
	if( TimeCount >= 10 && _ProcDelACHistoryList.size() == 0 )
	{
		g_CritSect()->Enter();
		_ProcDelACHistoryList = _WaitDelACHistoryList;
		_WaitDelACHistoryList.clear();
		g_CritSect()->Leave();
		vector< DB_ACItemStructPtr >*  ProcItemList = new vector< DB_ACItemStructPtr >;
		TimeCount = 0;
		//
		//所有物品生存時間下降 10
		map< int , DB_ACItemStructPtr >::iterator ItemIter;
		for( ItemIter = _AllItemList.begin() ; ItemIter != _AllItemList.end() ; )
		{
			DB_ACItemStruct& Item = *( ItemIter->second.get() );
			Item.LiveTime -= 10;
			if( Item.LiveTime < 0 )
			{
				ProcItemList->push_back( ItemIter->second );
				if( Item.BuyerDBID == -1 )
				{
					Item.State = EM_ACItemState_Cancel;
				}
				else
				{
					Item.State = EM_ACItemState_BuyOut;
				}
				OnPlayerSelectItemChange( &Item );
				//如果物品沒人買退回給賣家
				if( Item.BuyerDBID == -1 )
				{					
					if( Item.SellerDBID != -1 )
					{
						CNetDC_MailChild::SysSendItem( Item.SellerDBID , Item.SellerName.Begin() , Item.Item , "[SYS_AC_SENDBACK_ITEM]" , true );						
					}
					Global::Log_AC( &Item , EM_AcActionType_NoDeal , -1 , 0  );
				}
				else
				{
					Global::Log_AC( &Item , EM_AcActionType_Deal , Item.BuyerDBID , Item.NowPrices  );
					//送錢給賣方
					if( Item.SellerDBID != -1 )
					{
						GameObjDbStructEx* ItemDB = g_ObjectData->GetObj( Item.Item.OrgObjID );
						int Money = 0;
						if(		(ItemDB->IsItem() && ItemDB->Item.ItemType == EM_ItemType_Money ) 
							||	Item.PricesType != EM_ACPriceType_GameMoney )
						{
							Money = int( Item.NowPrices * _AC_Tax_AccountMoney );
						}
						else
						{
							Money =int( Item.NowPrices * _AC_Tax_Money );
						}
						int Tax = Item.NowPrices - Money;
						char Buf[512];
						if( Tax != 0 )
							sprintf_s(Buf, sizeof(Buf) , "[%s][$SETVAR1=[%d]][$SETVAR2=%d]", "SYS_AUCTION_RETURN_MAIL" , Item.Item.OrgObjID , Tax ); 
						else
							sprintf_s(Buf, sizeof(Buf) , "[%s][$SETVAR1=[%d]][$SETVAR2=%d]", "SYS_AUCTION_RETURN_MAIL" , Item.Item.OrgObjID , Tax ); 
						switch( Item.PricesType )
						{
						case EM_ACPriceType_GameMoney:
							Global::Log_Money( Item.SellerDBID , EM_ActionType_ACTex , Money - Item.NowPrices );
							CNetDC_MailChild::SysSendMoney( Item.BuyerDBID , Item.SellerName.Begin() , Money , 0 , 0 , "[SYS_AC_SELL_INCOME]", Buf , Item.GUID );							
							break;
						case EM_ACPriceType_AccountMoney:
//							Global::Log_Money_Account( Item.SellerDBID , EM_ActionType_ACTex , Money - Item.NowPrices );
							CNetDC_MailChild::SysSendMoney( Item.BuyerDBID , Item.SellerName.Begin() , 0 , Money , 0 , "[SYS_AC_SELL_INCOME]",Buf,Item.GUID);			
							break;
						case EM_ACPriceType_BonusMoney:
//							Global::Log_Money_Bonus( Item.SellerDBID , EM_ActionType_ACTex , Money - Item.NowPrices );	
							break;
						}
						Global::Log_AC( &Item , EM_AcActionType_SendMoneytoSeller , Item.BuyerDBID , Money  );
						SC_AcTradeOk( Item.SellerDBID , Item.BuyerName.Begin() , Item.SellerName.Begin() , Item.PricesType , Money , Tax ,Item.Item );
						SC_AcTradeOk( Item.BuyerDBID , Item.BuyerName.Begin() , Item.SellerName.Begin() , Item.PricesType , Money , Tax ,Item.Item );
					//	CNetDC_MailChild::SysSendMoney( Item.BuyerDBID , Item.SellerName.Begin() , int( Item.NowPrices * _DEF_AC_BUY_TAXRATE_ ) , 0 , 0 , g_ObjectData->GetString( "SYS_AC_SELL_INCOME" ) );
					}
					//送物品給買方					
					CNetDC_MailChild::SysSendItem( Item.SellerDBID , Item.BuyerName.Begin() , Item.Item , "[SYS_AC_BUY]" , Item.GUID );
				}
				ItemIter = _AllItemList.erase( ItemIter );
			}
			else
			{
				ItemIter++;
			}
		}
		//_RD_ACItemDB->SqlCmd_Transcation( 1 , _SQLOnTimeProc , _SQLOnTimeProcResult , ProcItemList	, NULL );
		_RD_ACItemDB->SqlCmd( 1 , _SQLOnTimeProc , _SQLOnTimeProcResult , ProcItemList	, NULL );
	}
	//////////////////////////////////////////////////////////////////////////
	//定時查看後台是否有東西要insert ac
	//////////////////////////////////////////////////////////////////////////
	_RD_NormalDB_Import->SqlCmd( 3 , _SQLImportOnTimeProc , _SQLImportOnTimeProcResult , NULL	, NULL );
	//////////////////////////////////////////////////////////////////////////
	Schedular()->Push( _OnTimeProc , 1000*60 , This , NULL );  
	return 0;
}
//--------------------------------------------------------------------------------------
struct ACSearchCoditionTempInfo
{
	int	DBID;
	ACSearchConditionStruct Data;
};
void CNetDC_ACChild::RL_SearchItem			( int DBID , ACSearchConditionStruct& SearchData )
{
	if( Global::_IsDelay != false )
	{
		SC_SearchItemCountResult( DBID , 0 , 0 , 0 );
		return;
	}
	PlayerSelectManageStruct PlayerData;
	//此玩家是否已經存在
	map< int , PlayerSelectManageStruct >::iterator Iter;
	Iter = _PlayerList.find( DBID );
	if( Iter == _PlayerList.end() )
	{
		//此玩家尚未開啟AC
		return ;
		/*
		//產生新的玩家管理物件
		PlayerData.PlayerDBID = DBID;				//玩家的資料庫ID
		PlayerData.Pos = 0;							//已傳送位置		
		PlayerData.SelectTime = 0;					//上一次搜尋玩成時間( 每次要求要相格兩秒 )
		PlayerData.OnDbSearchFlag = true;			//正在做DB Search的工作 ，此時不能處理新的要求 (如有要求表示Client發出命令太頻繁 並且有被修改的可能)
		PlayerData.LiveTime = clock() + 1000*10*60;	//生存期
		_PlayerList[ DBID ] = PlayerData;
		*/
	}
	else
	{
		//檢查是否連續查尋
		if(		Iter->second.OnDbSearchFlag != false 
			||	Iter->second.SelectTime > clock() )
		{
			return;
		}
		Iter->second.OnDbSearchFlag = true;
		Iter->second.LiveTime = clock() + 1000*10*60;
		Iter->second.ItemList.clear();
		Iter->second.ItemDBIDSet.clear();
		Iter->second.Pos = 0;
	}
	ACSearchCoditionTempInfo* TempSearch = new ACSearchCoditionTempInfo;
	TempSearch->DBID = DBID;
	TempSearch->Data = SearchData;
	_RD_ACItemDB->SqlCmd( DBID , _SQLCmdSearchProc , _SQLCmdSearchProcResult , TempSearch	, NULL );
}
void CNetDC_ACChild::RL_GetNextSearchItem	( int DBID )
{
	//此玩家是否已經存在
	map< int , PlayerSelectManageStruct >::iterator Iter;
	Iter = _PlayerList.find( DBID );
	if(		Iter == _PlayerList.end() 
		||	Iter->second.OnDbSearchFlag != false 
		||	Iter->second.SelectTime > clock()	)
	{
		SC_SearchItemCountResult( DBID , 0 , 0 , 0 );
		return;
	}
	PlayerSelectManageStruct& PlayerData = Iter->second;
	//設定角色狀態
	PlayerData.SelectTime = clock() + 2000;
	PlayerData.LiveTime = clock() + 1000*10*60;
	//送資料給Client
	// 送數量資料
	int	SendItemCount = __min( 50 , (int)PlayerData.ItemList.size() - PlayerData.Pos );
	SC_SearchItemCountResult( DBID , PlayerData.Pos , (int)PlayerData.ItemList.size() , SendItemCount );
	// 送每筆資料
	for( int i = 0 ; i < SendItemCount ; i++ )
	{
		ACItemInfoStruct Data;
		DB_ACItemStructPtr ItemPtr = PlayerData.ItemList[ i + PlayerData.Pos ];
		if( ItemPtr->State != EM_ACItemState_Normal )
			continue;
		Data.DBID = ItemPtr->GUID;
		Data.BuyOutPrices = ItemPtr->BuyOutPrices;
		Data.LiveTime = ItemPtr->LiveTime;
		Data.NowPrices = ItemPtr->NowPrices;
		Data.Item = ItemPtr->Item;
		Data.Type = ItemPtr->PricesType;
		SC_SearchItemResult( DBID , i + PlayerData.Pos , Data , (char*)ItemPtr->SellerName.Begin() , ItemPtr->BuyerDBID );
	}
	Iter->second.Pos += SendItemCount;
}
void CNetDC_ACChild::RL_GetMyACItemInfo		( int DBID )
{
}
void CNetDC_ACChild::RL_ItemHistoryRequest	( int DBID , int OrgObjID , int Inherent  )
{
	Inherent = 0;
	SC_ItemHistoryList( DBID , *GetItemHistory( OrgObjID , Inherent ) );	
}
//-----------------------------------------------------------------------------------------------------
struct ACBuyItemTempInfo
{
	int SrvSockID;
	int BuyerDBID;
	int ACItemDBID;
	int	Price;
	PriceTypeENUM PricesType;
};
void CNetDC_ACChild::RL_BuyItem				( int SrvSockID , int BuyerDBID , int ACItemDBID , int Prices , PriceTypeENUM PricesType )
{
	//此玩家是否已經存在
	map< int , PlayerSelectManageStruct >::iterator PlayerIter;
	PlayerIter = _PlayerList.find( BuyerDBID );
	if( PlayerIter == _PlayerList.end() )
		return;
	PlayerSelectManageStruct& PlayerData = PlayerIter->second;
	map< int , DB_ACItemStructPtr >::iterator Iter;
	Iter = _AllItemList.find( ACItemDBID );
	if( Iter == _AllItemList.end() )
	{
		//回應失敗
		SL_BuyItemResult( SrvSockID , BuyerDBID , ACItemDBID , Prices , PricesType , EM_ACItemState_Normal , false );
		return;
	}
	//資料確定
	DB_ACItemStructPtr ItemPtr = Iter->second;
	if( ItemPtr->PricesType != PricesType )
	{
		SL_BuyItemResult( SrvSockID , BuyerDBID , ACItemDBID , Prices , PricesType , EM_ACItemState_Normal , false );
		return;
	}
	int CheckPrices = __min( ItemPtr->BuyOutPrices , int( ItemPtr->NowPrices * AC_BID_LIMIT_PERCENTAGE ) );
	if(		ItemPtr->State	!= EM_ACItemState_Normal 
		||  CheckPrices		> Prices 
		||  ItemPtr->BuyOutPrices < Prices )
	{
		//回應失敗
		if(		ItemPtr->NowPrices == ItemPtr->BuyOutPrices 
			&& ItemPtr->NowPrices == Prices 
			&& ItemPtr->State == EM_ACItemState_Normal )
		{// Buyout == Price
		}
		else
		{
			SL_BuyItemResult( SrvSockID , BuyerDBID , ACItemDBID , Prices , PricesType , ItemPtr->State , false );
			return;
		}
	}
/*
	ACBuyItemTempInfo* TempInfo = new ACBuyItemTempInfo;
	TempInfo->SrvSockID = SrvSockID;
	TempInfo->BuyerDBID = BuyerDBID;
	TempInfo->ACItemDBID = ACItemDBID;
	TempInfo->Price = Prices;
	TempInfo->PricesType = PricesType;
*/
	//通知原本最高價者
	if( ItemPtr->BuyerDBID != -1 )
	{
		//退錢給原本出最高價者	
		switch( PricesType )
		{
		case EM_ACPriceType_GameMoney:
			CNetDC_MailChild::SysSendMoney( ItemPtr->BuyerDBID , ItemPtr->BuyerName.Begin() , ItemPtr->NowPrices , 0 , 0 , "[SYS_AC_SENDBACK_MONEY]","",ItemPtr->GUID );
			break;
		case EM_ACPriceType_AccountMoney:
			CNetDC_MailChild::SysSendMoney( ItemPtr->BuyerDBID , ItemPtr->BuyerName.Begin() , 0 , ItemPtr->NowPrices , 0 , "[SYS_AC_SENDBACK_MONEY]","",ItemPtr->GUID );
			break;
		case EM_ACPriceType_BonusMoney:
			CNetDC_MailChild::SysSendMoney( ItemPtr->BuyerDBID , ItemPtr->BuyerName.Begin() , 0 , 0 , ItemPtr->NowPrices  , "[SYS_AC_SENDBACK_MONEY]","",ItemPtr->GUID );
			break;
		}
		Global::Log_AC( ItemPtr.get() , EM_AcActionType_DrawBack , ItemPtr->BuyerDBID , ItemPtr->NowPrices  );
		//通知原本出最高價者
		SC_LostCompetitiveItem( ItemPtr->BuyerDBID , ItemPtr->GUID , ItemPtr->Item );
	}
	//通知所有有出價過的人
	DB_ACItemStruct* Item = ItemPtr.get();
	if(  Prices == ItemPtr->BuyOutPrices )
	{
		ItemPtr->State = EM_ACItemState_BuyOut;
	}
	ItemPtr->NowPrices = Prices;
	ItemPtr->BuyerDBID = BuyerDBID;
	ItemPtr->BuyerName = GetPlayerName( BuyerDBID );
	ItemPtr->PricesType = PricesType;
	OnPlayerSelectItemChange( Item );
	//檢查是否要Insert order
	bool	InsertOrder = false;
	set< int >::iterator OrderIter = PlayerData.OrderSet.find( ItemPtr->GUID) ;
	if( ItemPtr->State != EM_ACItemState_BuyOut )
	{
		if( OrderIter == PlayerData.OrderSet.end() )
		{
			InsertOrder = true;
			PlayerData.OrderSet.insert( ACItemDBID );
		}
		Global::Log_AC( ItemPtr.get() , EM_AcActionType_Bid , ItemPtr->BuyerDBID , ItemPtr->NowPrices  );
	}
	else
	{
		//搜尋看賣方是否存在
		map< int , PlayerSelectManageStruct >::iterator SellerIter;
		SellerIter = _PlayerList.find( ItemPtr->SellerDBID );
		if( SellerIter != _PlayerList.end() )
		{
			SellerIter->second.SellItemCount --;
		}
		CNetDC_MailChild::SysSendItem ( ItemPtr->SellerDBID  , ItemPtr->BuyerName.Begin()  , ItemPtr->Item , "[SYS_AC_BUY]", true );
		GameObjDbStructEx* ItemDB = g_ObjectData->GetObj( ItemPtr->Item.OrgObjID );
		Global::Log_AC( ItemPtr.get() , EM_AcActionType_BuyOut , ItemPtr->BuyerDBID , ItemPtr->NowPrices  );
		//如果不是系統信件
		if( ItemPtr->SellerDBID != -1 )
		{
			//寄錢給賣方		
			int Money = 0;
			if( (ItemDB->IsItem() && ItemDB->Item.ItemType == EM_ItemType_Money )
				|| PricesType != EM_ACPriceType_GameMoney)
			{
				Money = int(ItemPtr->NowPrices * _AC_Tax_AccountMoney);
			}
			else
			{
				Money =int( ItemPtr->NowPrices * _AC_Tax_Money );
			}
			int Tax = ItemPtr->NowPrices - Money;
			char Buf[512];
			if( Tax != 0 )
				sprintf_s(Buf, sizeof(Buf) , "[%s][$SETVAR1=[%d]][$SETVAR2=%d]", "SYS_AUCTION_RETURN_MAIL" , ItemPtr->Item.OrgObjID , Tax ); 
			else
				sprintf_s(Buf, sizeof(Buf) , "[%s][$SETVAR1=[%d]][$SETVAR2=%d]", "SYS_AUCTION_RETURN_MAIL" , ItemPtr->Item.OrgObjID , Tax ); 
			switch( PricesType )
			{
			case EM_ACPriceType_GameMoney:
				Global::Log_Money( ItemPtr->SellerDBID , EM_ActionType_ACTex , Money - ItemPtr->NowPrices );
				CNetDC_MailChild::SysSendMoney( ItemPtr->BuyerDBID , ItemPtr->SellerName.Begin() , Money , 0 , 0 , "[SYS_AC_SELL_INCOME]",Buf,ItemPtr->GUID);
				break;
			case EM_ACPriceType_AccountMoney:
//				Global::Log_Money_Account( ItemPtr->SellerDBID , EM_ActionType_ACTex , Money - ItemPtr->NowPrices );
				CNetDC_MailChild::SysSendMoney( ItemPtr->BuyerDBID , ItemPtr->SellerName.Begin() , 0 , Money , 0 , "[SYS_AC_SELL_INCOME]",Buf,ItemPtr->GUID);
				break;
			case EM_ACPriceType_BonusMoney:
//				Global::Log_Money_Bonus( ItemPtr->SellerDBID , EM_ActionType_ACTex , Money - ItemPtr->NowPrices );	
				break;
			}
			SC_AcTradeOk( ItemPtr->SellerDBID , ItemPtr->BuyerName.Begin() , ItemPtr->SellerName.Begin() , ItemPtr->PricesType , Money , Tax , ItemPtr->Item );
			Global::Log_AC( ItemPtr.get() , EM_AcActionType_SendMoneytoSeller , ItemPtr->BuyerDBID , Money  );
		}
		_AllItemList.erase( Iter );
	}
	SL_BuyItemResult( SrvSockID , BuyerDBID , ACItemDBID , Prices , PricesType , ItemPtr->State , true );
	DB_ACItemStruct* TempItem = NEW DB_ACItemStruct;
	*TempItem = *Item;
	_RD_ACItemDB->SqlCmd( ACItemDBID , _SQLCmdBuyProc , _SQLCmdBuyProcResult , TempItem	
		,"InsertOrder"		, EM_ValueType_Int , InsertOrder
		, NULL );
}
//-----------------------------------------------------------------------------------------------------
struct ACSellItemTempInfo
{
	int				SrvSockID;
	int				SellerDBID;
	StaticString<_MAX_NAMERECORD_SIZE_>	SellerName;
	ItemFieldStruct Item;
	int				Prices;
	int				BuyOutPrices;	
	PriceTypeENUM	PricesType; 
	int				LiveTime;
	int				TaxMoney;
	ACSellItemTempInfo()
	{
		memset( this , 0 , sizeof(*this) );
	}
};
void CNetDC_ACChild::RL_SellItem			( int SrvSockID , int SellerDBID , ItemFieldStruct& Item , int Prices , int BuyOutPrices , PriceTypeENUM	PricesType , int LiveTime , int TaxMoney )
{	
	//此玩家是否已經存在
	map< int , PlayerSelectManageStruct >::iterator PlayerIter;
	PlayerIter = _PlayerList.find( SellerDBID );
	if( PlayerIter == _PlayerList.end() 
		|| ( PricesType != EM_ACPriceType_GameMoney && PricesType != EM_ACPriceType_AccountMoney) )
	{
		ACItemInfoStruct ACItem;
		ACItem.BuyOutPrices = BuyOutPrices;
		ACItem.DBID = -1;
		ACItem.Item = Item;
		ACItem.LiveTime = -1;
		ACItem.NowPrices = Prices;
		ACItem.Type	= PricesType;
		SL_SellItemResult			( SrvSockID , SellerDBID , ACItem , TaxMoney , EM_ACSellItemResult_DataError );
		return;
	}
	PlayerSelectManageStruct& PlayerData = PlayerIter->second;
	if( PlayerData.SellItemCount >= _MAX_AC_SELL_ITEM_COUNT_ )
	{
		ACItemInfoStruct ACItem;
		ACItem.BuyOutPrices = BuyOutPrices;
		ACItem.DBID = -1;
		ACItem.Item = Item;
		ACItem.LiveTime = -1;
		ACItem.NowPrices = Prices;
		ACItem.Type	= PricesType;
		SL_SellItemResult			( SrvSockID , SellerDBID , ACItem , TaxMoney , EM_ACSellItemResult_MaxCountError );
		return;
	}
	ACSellItemTempInfo*	Info = new ACSellItemTempInfo;
	Info->SellerDBID = SellerDBID;
	Info->SrvSockID = SrvSockID;
	Info->Item = Item;
	Info->Prices = Prices;
	Info->BuyOutPrices = BuyOutPrices;
	Info->SellerName = Global::GetPlayerName( SellerDBID );
	Info->LiveTime = LiveTime;
	Info->TaxMoney = TaxMoney;
	Info->PricesType = PricesType;
	PlayerData.SellItemCount++;
	//_RD_ACItemDB->SqlCmd_Transcation( SellerDBID , _SQLCmdSellProc , _SQLCmdSellProcResult , Info	, NULL );
	_RD_ACItemDB->SqlCmd( SELL_ACITEM_THREADID , _SQLCmdSellProc , _SQLCmdSellProcResult , Info	, NULL );
}
void CNetDC_ACChild::RL_CancelSellItem		( int DBID , int ACItemDBID )
{
	map< int , DB_ACItemStructPtr >::iterator Iter;
	Iter = _AllItemList.find( ACItemDBID );
	if( Iter == _AllItemList.end() )
	{
		//回應失敗
		SC_CancelSellItemResult( DBID , ACItemDBID , false );
		return;
	}
	//資料確定
	DB_ACItemStructPtr ItemPtr = Iter->second;
	if( ItemPtr->SellerDBID != DBID || ItemPtr->State != EM_ACItemState_Normal )
	{
		//回應失敗
		SC_CancelSellItemResult( DBID , ACItemDBID , false );
		return;
	}
	ItemPtr->State = EM_ACItemState_Cancel;
	_RD_ACItemDB->SqlCmd( DBID	, _SQLCmdCancelSellProc , _SQLCmdCancelSellProcResult , NULL	
		,"DBID"		, EM_ValueType_Int , DBID
		,"ACItemDBID"	, EM_ValueType_Int , ACItemDBID
		, NULL );
}
void CNetDC_ACChild::RL_CloseAC				( int DBID ) 
{
	_PlayerList.erase( DBID );
}
void CNetDC_ACChild::RL_OpenAC				( int DBID , int NpcGUID  )
{
	PlayerSelectManageStruct PlayerData;
	//此玩家是否已經存在
	map< int , PlayerSelectManageStruct >::iterator Iter;
	Iter = _PlayerList.find( DBID );
	if( Iter != _PlayerList.end() )
	{
		PlayerData.LiveTime = clock() + 1000*10*60;	//生存期
		SC_OpenAC( DBID , NpcGUID , false );
		return;
	}
	//此玩家尚未開啟AC
	//產生新的玩家管理物件
	PlayerData.PlayerDBID = DBID;				//玩家的資料庫ID
	PlayerData.Pos = 0;							//已傳送位置		
	PlayerData.SelectTime = 0;					//上一次搜尋玩成時間( 每次要求要相格兩秒 )
	PlayerData.OnDbSearchFlag = true;			//正在做DB Search的工作 ，此時不能處理新的要求 (如有要求表示Client發出命令太頻繁 並且有被修改的可能)
	PlayerData.LiveTime = clock() + 1000*10*60;	//生存期
	_PlayerList[ DBID ] = PlayerData;
	_RD_ACItemDB->SqlCmd( DBID	, _SQLCmdOpenProc , _SQLCmdOpenProcResult , NULL
		,"DBID"			, EM_ValueType_Int , DBID
		,"NpcGUID"		, EM_ValueType_Int , NpcGUID
		, NULL );
}
//--------------------------------------------------------------------------------------
//取得物品歷史資訊
vector<DB_ACItemHistoryStruct>*	CNetDC_ACChild::GetItemHistory( int OrgObjID , int Inherent )
{
	static vector<DB_ACItemHistoryStruct> EmptyTemp;
	ACItemHistoryTypeStruct ItemBase;
	ItemBase.Inherent = Inherent;
	ItemBase.OrgObjID = OrgObjID;
	map< ACItemHistoryTypeStruct , vector<DB_ACItemHistoryStruct> >::iterator Iter;
	Iter = _HistoryList.find( ItemBase );
	if( Iter == _HistoryList.end() )
		return &EmptyTemp;
	return &(Iter->second);
}
//--------------------------------------------------------------------
//事件(封包 與資料庫 處理)
//--------------------------------------------------------------------
//當玩家搜尋物件被刪除
bool	CNetDC_ACChild::OnPlayerSelectItemChange( DB_ACItemStruct* Item )
{
	//找出所有擁有此物件的玩家
	map< int , PlayerSelectManageStruct >::iterator Iter;
	set< int >::iterator	SetIter;
	SC_FixSellOrderItem( Item->SellerDBID , Item->GUID , Item->NowPrices , Item->PricesType , Item->State , (char*)Item->BuyerName.Begin() );
	for( Iter = _PlayerList.begin() ; Iter != _PlayerList.end() ; Iter++ )
	{
		PlayerSelectManageStruct&	PlayerData = Iter->second;
		SetIter = PlayerData.ItemDBIDSet.find( Item->GUID );
		if( SetIter != PlayerData.ItemDBIDSet.end() )
		{
			//通知玩家
			SC_FixSearchItemResult( PlayerData.PlayerDBID , Item->GUID , Item->NowPrices , Item->PricesType , Item->State , Item->BuyerDBID );
		}
		//if( Item->State != EM_ACItemState_Normal )
		//查所有有下定單者
		SetIter = PlayerData.OrderSet.find( Item->GUID );
		if( SetIter != PlayerData.OrderSet.end() )		
		{
			SC_FixBuyOrderItem( PlayerData.PlayerDBID , Item->GUID , Item->NowPrices , Item->PricesType , Item->State , Item->BuyerDBID );
			if( Item->State != EM_ACItemState_Normal )
			{
				PlayerData.OrderSet.erase( SetIter );
			}
		}		
	}
	return true;
}
//--------------------------------------------------------------------
//資料庫處理
//--------------------------------------------------------------------
bool CNetDC_ACChild::_SQLCmdSellProc( vector<DB_ACItemStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	//MutilThread_CritSect          Thread_CritSect;
	//HSTMT dbproc = sqlBase->Gethstmt();
	ACSellItemTempInfo*	Info = ( ACSellItemTempInfo *)UserObj;
	//販賣上限檢查
	int		ItemCount = 0;
	MyDbSqlExecClass	MyDBProc( sqlBase );
	//資料庫儲存
	DB_ACItemStruct SaveACData;
	//取得物件資料
	//合成物品名稱
	GameObjDbStructEx* OrgObj = g_ObjectData->GetObj( Info->Item.OrgObjID );
	if( OrgObj == NULL )
		return false;
	SaveACData.ItemName = OrgObj->Name;
	int MaxItemDBID = 0;
//	g_CritSect_Thread()->Enter();
	{
		MyDBProc.SqlCmd( "SELECT MAX(GUID) AS Expr1 FROM AC_Base" );
		MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&MaxItemDBID );
		MyDBProc.Read();
		MyDBProc.Close();
		MaxItemDBID++;
		SaveACData.GUID = MaxItemDBID;
	}
//	g_CritSect_Thread()->Leave();
	SaveACData.SellerName = Info->SellerName;
	SaveACData.SellerDBID = Info->SellerDBID;
	SaveACData.BuyerName.Clear();
	SaveACData.BuyerDBID = -1;
	SaveACData.BuyOutPrices = Info->BuyOutPrices;
	SaveACData.NowPrices = Info->Prices;
	SaveACData.PricesType = Info->PricesType;
	SaveACData.LiveTime = Info->LiveTime;
	SaveACData.Item = Info->Item;
	SaveACData.ItemType[0] = OrgObj->ACSearchType[0];
	SaveACData.ItemType[1] = OrgObj->ACSearchType[1];
	SaveACData.ItemType[2] = OrgObj->ACSearchType[2];
	SaveACData.ItemLevel = OrgObj->Limit.Level;
	SaveACData.SexType = OrgObj->Limit.Sex;
	SaveACData.RaceType = OrgObj->Limit.Race;
	SaveACData.Rare = OrgObj->Rare + Info->Item.Rare;
	SaveACData.EQType = OrgObj->ItemEqType;
	//SaveACData.RuneVolume
	int RuneVolume = SaveACData.Item.RuneVolume;
	for( int i = 0 ; i < 4 ; i++  )
	{
		GameObjDbStructEx* AddPowerDB	= g_ObjectData->GetObj( SaveACData.Item.Rune(i) );
		if( AddPowerDB->IsRune() == false)
			continue;
		RuneVolume += AddPowerDB->Item.Volume;
	}
	SaveACData.RuneVolume = RuneVolume;
	//g_CritSect()->Enter();
	MyDBProc.SqlCmd_WriteOneLine( _RDACBaseSql->GetInsertStr( &SaveACData ).c_str() );
	//g_CritSect()->Leave();
	Data.push_back( SaveACData );
	return true;
}
void CNetDC_ACChild::_SQLCmdSellProcResult( vector<DB_ACItemStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	ACSellItemTempInfo*	Info = ( ACSellItemTempInfo *)UserObj;
	ACItemInfoStruct ACItem;
	ACItem.BuyOutPrices = Info->BuyOutPrices;
	if( Data.size() != 0 )
		ACItem.DBID = Data[0].GUID;
	else
		ACItem.DBID = -1;
	ACItem.Item = Info->Item;
	ACItem.LiveTime = Info->LiveTime;
	ACItem.NowPrices = Info->Prices;
	ACItem.Type = Info->PricesType;
	if( ResultOK == false )
	{
		if( Info->SrvSockID != -1 )
			SL_SellItemResult( Info->SrvSockID , Info->SellerDBID , ACItem , Info->TaxMoney , EM_ACSellItemResult_DataError );		
	}
	else
	{
		//加入All item
		DB_ACItemStructPtr ItemPtr( new DB_ACItemStruct );
		*( ItemPtr.get() ) = Data[0];
		ItemPtr->State = EM_ACItemState_Normal;
		_AllItemList[ ItemPtr->GUID ] = ItemPtr;
		if( Info->SrvSockID != -1 )
			SL_SellItemResult( Info->SrvSockID , Info->SellerDBID , ACItem  , Info->TaxMoney , EM_ACSellItemResult_OK );
		Global::Log_AC( ItemPtr.get() , EM_AcActionType_Sell,  -1 , 0  );		
	}
	delete Info;
}
bool CNetDC_ACChild::_SQLCmdInitProc( vector<DB_ACItemStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass	MyDBProc( sqlBase );
	//MutilThread_CritSect          Thread_CritSect;
	//1 載入所有的拍賣物件
	DB_ACItemStruct	TempACData;
	//g_CritSect()->Enter();
	MyDBProc.SqlCmd( _RDACBaseSql->GetSelectStr("WHERE DestroyTime=0").c_str() );
	//g_CritSect()->Leave();
	MyDBProc.Bind( TempACData , RoleDataEx::ReaderRD_ACBase() );
	while( MyDBProc.Read() )
	{
		RoleDataEx::ReaderRD_ACBase()->TransferWChartoUTF8( &TempACData , MyDBProc.wcTempBufList() );
		DB_ACItemStructPtr ItemPtr( new DB_ACItemStruct );
		*( ItemPtr.get() ) = TempACData;
		ItemPtr->State = EM_ACItemState_Normal;
		//		if( ItemPtr->GUID >= MaxItemDBID ) 
		//			MaxItemDBID = ItemPtr->GUID + 1;
		//**初始設定 不會跟其他的thread 搶到
		_AllItemList[ ItemPtr->GUID ] = ItemPtr;
	}
	MyDBProc.Close();
	//2 載入所有的歷史物件
	DB_ACItemHistoryStruct TempHistoryData;
	MyDBProc.SqlCmd( _RDACHistorySql->GetSelectStr("WHERE DestroyTime = 0").c_str() );
	MyDBProc.Bind( TempHistoryData , RoleDataEx::ReaderRD_ACHistory() );
	while( MyDBProc.Read() )
	{
		RoleDataEx::ReaderRD_ACHistory()->TransferWChartoUTF8( &TempHistoryData , MyDBProc.wcTempBufList() );
		ACItemHistoryTypeStruct hAcItem;
		hAcItem.OrgObjID = TempHistoryData.Item.OrgObjID;
		hAcItem.Inherent = 0;
		vector<DB_ACItemHistoryStruct>& ItemHistory = _HistoryList[ hAcItem ];
		ItemHistory.push_back( TempHistoryData );
		if( ItemHistory.size() > _MAX_HISTORY_ITEM_COUNT_ )
		{
			g_CritSect()->Enter();
			_WaitDelACHistoryList.push_back( ItemHistory[0].GUID );
			g_CritSect()->Leave();
			ItemHistory.erase( ItemHistory.begin() );
		}
	}
	MyDBProc.Close();
	return true;
}
void CNetDC_ACChild::_SQLCmdInitProcResult ( vector<DB_ACItemStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	_IsInitReady = true;
}
bool CNetDC_ACChild::_SQLCmdCancelSellProc( vector<DB_ACItemStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char	Buf[1024];
	int     DBID       = Arg.GetNumber( "DBID" );
	int		ACItemDBID = Arg.GetNumber( "ACItemDBID" );
	//資料庫刪除
	MyDbSqlExecClass	MyDBProc( sqlBase );
	//sprintf( Buf , "DELETE FROM AC_Base WHERE (GUID = %d)" , ACItemDBID );
	sprintf( Buf , "UPDATE AC_Base SET DestroyTime = getdate() WHERE (GUID = %d)" , ACItemDBID );
	MyDBProc.SqlCmd_WriteOneLine( Buf );
	return true;
}
void CNetDC_ACChild::_SQLCmdCancelSellProcResult ( vector<DB_ACItemStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	int     DBID       = Arg.GetNumber( "DBID" );
	int		ACItemDBID = Arg.GetNumber( "ACItemDBID" );
	if( ResultOK == false )
	{
		//????
	}
	else
	{
		//
		//DelItem( ACItemDBID );
		map< int , DB_ACItemStructPtr >::iterator Iter;
		Iter = _AllItemList.find( ACItemDBID );
		//找不到
		if( Iter == _AllItemList.end() )
			return ;
		DB_ACItemStruct* Item = Iter->second.get();
		OnPlayerSelectItemChange( Item );
		//把東西用mail 送回給賣方
		CNetDC_MailChild::SysSendItem( Item->SellerDBID , Item->SellerName.Begin() , Item->Item , "[SYS_AC_SENDBACK_ITEM]",Item->GUID);
		//退錢給出最高價者
		if( Item->BuyerDBID != -1 )
		{
			//退錢給原本出最高價者	
			switch( Item->PricesType )
			{
			case EM_ACPriceType_GameMoney:
				CNetDC_MailChild::SysSendMoney( Item->BuyerDBID , Item->BuyerName.Begin() , Item->NowPrices , 0 , 0 , "[SYS_AC_SENDBACK_MONEY]","",Item->GUID);
				break;
			case EM_ACPriceType_AccountMoney:
				CNetDC_MailChild::SysSendMoney( Item->BuyerDBID , Item->BuyerName.Begin() , 0 , Item->NowPrices , 0 , "[SYS_AC_SENDBACK_MONEY]","",Item->GUID);
				break;
			case EM_ACPriceType_BonusMoney:
				CNetDC_MailChild::SysSendMoney( Item->BuyerDBID , Item->BuyerName.Begin() , 0 , 0 , Item->NowPrices , "[SYS_AC_SENDBACK_MONEY]","",Item->GUID);
				break;
			}
			Global::Log_AC( Item , EM_AcActionType_DrawBack ,  Item->BuyerDBID , Item->NowPrices  );
		}
		Global::Log_AC( Item , EM_AcActionType_Cancel ,  -1 , 0  );
		_AllItemList.erase( Iter );
	}
	SC_CancelSellItemResult( DBID , ACItemDBID , ResultOK );
}
bool CNetDC_ACChild::_SQLCmdSearchProc( vector<DB_ACItemStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	ACSearchCoditionTempInfo*	Info = ( ACSearchCoditionTempInfo *)UserObj;
	//------------------------------------------------------------------------------------------------------------
	//建立搜尋字串
	wchar_t	wBuf[1024];
	wstring	wStr;
	bool	InnerJoinNameStringDB = false;
	for( int i = 0 ; i < 3 ; i++ )
	{
		if( Info->Data.Type[i] != -1 )
		{
			if( wStr.length() == 0 )
			{
				swprintf( wBuf , L"WHERE AC_Base.Type%d = %d" , i+1 , Info->Data.Type[i] );
			}
			else
			{
				swprintf( wBuf , L" and AC_Base.Type%d = %d" , i+1 , Info->Data.Type[i] );
			}
			wStr = wStr + wBuf;
		}
		else
		{
			break;
		}
	}
	if( Info->Data.LevelHi != -1 )
	{
		if( wStr.length() == 0 )
		{
			swprintf( wBuf , L"WHERE AC_Base.ItemLevel <= %d" , Info->Data.LevelHi );
		}
		else
		{
			swprintf( wBuf , L" and AC_Base.ItemLevel <= %d" , Info->Data.LevelHi );
		}
		wStr = wStr + wBuf;
	}
	if( Info->Data.LevelLow != -1 )
	{
		if( wStr.length() == 0 )
		{
			swprintf( wBuf , L"WHERE AC_Base.ItemLevel >= %d" , Info->Data.LevelLow );
		}
		else
		{
			swprintf( wBuf , L" and AC_Base.ItemLevel >= %d" , Info->Data.LevelLow );
		}
		wStr = wStr + wBuf;
	}
	if( Info->Data.SearchName.Size() != 0 && CheckSqlStr(Info->Data.SearchName.Begin() ) )
	{
		CharTransferClass	CharTransfer;
		CharTransfer.SetUtf8String( Info->Data.SearchName.Begin() );
		InnerJoinNameStringDB = true;
		if( wStr.length() == 0 )
		{
			swprintf( wBuf , L"WHERE ( ObjNameStringDB.Language%d LIKE N'%%%s%%')" , Info->Data.Language + 1 , CharTransfer.GetWCString() );
		}
		else
		{
			swprintf( wBuf , L" and ( ObjNameStringDB.Language%d LIKE N'%%%s%%')" , Info->Data.Language + 1 , CharTransfer.GetWCString() );
		}
		/*
		if( Str.length() == 0 )
		{
			sprintf( Buf , "WHERE ( ItemName LIKE '%%%s%%')" , SqlStr_AddSingleQuotationMark( CharTransfer.GetString() ).c_str() );
		}
		else
		{
			sprintf( Buf , " and ( ItemName LIKE '%%%s%%')" , SqlStr_AddSingleQuotationMark( CharTransfer.GetString() ).c_str() );
		}
		*/
		wStr = wStr + wBuf;
	}
	if( Info->Data.Race._Race != -1 )
	{
		if( wStr.length() == 0 )
		{
			swprintf( wBuf , L"WHERE ( AC_Base.RaceType & %d <> 0)" , Info->Data.Race._Race );
		}
		else
		{
			swprintf( wBuf , L" and ( AC_Base.RaceType & %d <> 0)" , Info->Data.Race._Race );
		}
		wStr = wStr + wBuf;	
	}
	if( Info->Data.RuneVolume != -1 )
	{
		if( wStr.length() == 0 )
		{
			swprintf( wBuf , L"WHERE AC_Base.RuneVolume >= %d" , Info->Data.RuneVolume );
		}
		else
		{
			swprintf( wBuf , L" and AC_Base.RuneVolume >= %d" , Info->Data.RuneVolume );
		}
		wStr = wStr + wBuf;
	}
	if( Info->Data.Sex._Sex != -1 )
	{
		if( wStr.length() == 0 )
		{
			swprintf( wBuf , L"WHERE ( AC_Base.SexType & %d <> 0)" , Info->Data.Sex._Sex );
		}
		else
		{
			swprintf( wBuf , L" and ( AC_Base.SexType & %d <> 0)" , Info->Data.Sex._Sex );
		}
		wStr = wStr + wBuf;	
	}
	if( Info->Data.Rare != -1 )
	{
		if( wStr.length() == 0 )
		{
			swprintf( wBuf , L"WHERE AC_Base.Rare >= %d" , Info->Data.Rare );
		}
		else
		{
			swprintf( wBuf , L" and AC_Base.Rare >= %d" , Info->Data.Rare );
		}
		wStr = wStr + wBuf;
	}
	if( Info->Data.JobType._Voc != -1 )
	{
		if( wStr.length() == 0 )
		{
			swprintf( wBuf , L"WHERE ( AC_Base.JobType & %d <> 0)" , Info->Data.JobType._Voc );
		}
		else
		{
			swprintf( wBuf , L" and ( AC_Base.JobType & %d <> 0)" , Info->Data.JobType._Voc );
		}
		wStr = wStr + wBuf;	
	}
	if( Info->Data.SearchType._Skill != -1 )
	{
		if( wStr.length() == 0 )
		{
			swprintf( wBuf , L"WHERE ( AC_Base.EqWearType & %d <> 0)" , Info->Data.SearchType._Skill );
		}
		else
		{
			swprintf( wBuf , L" and ( AC_Base.EqWearType & %d <> 0)" , Info->Data.SearchType._Skill );
		}
		wStr = wStr + wBuf;	
	}
	if( Info->Data.PriceType != EM_ACPriceType_None )
	{
		if( wStr.length() == 0 )
		{
			swprintf( wBuf , L"WHERE ( AC_Base.PricesType = %d )" , Info->Data.PriceType );
		}
		else
		{
			swprintf( wBuf , L" and ( AC_Base.PricesType = %d )" , Info->Data.PriceType );
		}
		wStr = wStr + wBuf;	
	}
	//------------------------------------------------------------------------------------------------------------
	if( wStr.length() == 0 )
		swprintf( wBuf , L"Select Top %d GUID From AC_Base WHERE LiveTime > 0 AND DestroyTime = 0 ORDER BY Rare DESC ,ItemLevel DESC", _AcSelectMaxCount );
	else
	{
		if( InnerJoinNameStringDB == false )
			swprintf( wBuf , L"Select Top %d GUID From AC_Base %s AND LiveTime > 0 AND DestroyTime = 0 ORDER BY Rare DESC ,ItemLevel DESC" , _AcSelectMaxCount , wStr.c_str() );
		else
			swprintf( wBuf , L"Select Top %d AC_Base.GUID From AC_Base INNER JOIN ObjNameStringDB ON AC_Base.OrgObjID = ObjNameStringDB.GUID %s AND AC_Base.LiveTime > 0 AND AC_Base.DestroyTime = 0 ORDER BY AC_Base.Rare DESC ,AC_Base.ItemLevel DESC" , _AcSelectMaxCount , wStr.c_str() );
	}
	MyDbSqlExecClass	MyDBProc( sqlBase );
	vector<int>*		ItemDBIDList = new vector<int>;
	int					ItemDBID;
	MyDBProc.SqlCmd( wBuf );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ItemDBID );
	while( MyDBProc.Read() )
	{
		ItemDBIDList->push_back( ItemDBID );
	}
	MyDBProc.Close();
	//------------------------------------------------------------------------------------------------------------
	Arg.PushValue("ItemDBIDList", UINT64(ItemDBIDList));
	return true;
}
void CNetDC_ACChild::_SQLCmdSearchProcResult ( vector<DB_ACItemStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	vector<int> *ItemDBIDList = (vector<int> *)Arg.GetUINT64("ItemDBIDList");
	if( ItemDBIDList == NULL ) 
		return;
	ACSearchCoditionTempInfo*	Info = ( ACSearchCoditionTempInfo *)UserObj;
	map< int , PlayerSelectManageStruct >::iterator Iter;
	Iter = _PlayerList.find( Info->DBID );
	if( Iter == _PlayerList.end() )
	{
		Print( LV3 , "_SQLCmdSearchProcResult" , "找不到玩家資料" );
		return;
	}
	PlayerSelectManageStruct& PlayerData = Iter->second;
	map< int , DB_ACItemStructPtr >::iterator ItemIter;
	//設定搜尋出的物品資料
	for( int i = 0 ; i < (int)ItemDBIDList->size() ; i++ )
	{
		ItemIter = _AllItemList.find( (*ItemDBIDList)[i] );
		if( ItemIter == _AllItemList.end() )
		{
			//Print( LV3 , "_SQLCmdSearchProcResult" , "搜尋出的資料與記憶體內的不符合" );
			continue;
		}
		PlayerData.ItemDBIDSet.insert( (*ItemDBIDList)[i] );
		PlayerData.ItemList.push_back( ItemIter->second );		
	}
	//設定角色狀態
	PlayerData.SelectTime = clock() + 2000;
	PlayerData.LiveTime = clock() + 1000*10*60;
	PlayerData.OnDbSearchFlag = false;
	//送資料給Client
	// 送數量資料
	int	SendItemCount = __min( 50 , (int)PlayerData.ItemList.size() );
	SC_SearchItemCountResult( Info->DBID , 0 , (int)PlayerData.ItemList.size() , SendItemCount );
	// 送每筆資料
	for( int i = 0 ; i < SendItemCount ; i++ )
	{
		ACItemInfoStruct Data;
		DB_ACItemStructPtr ItemPtr = PlayerData.ItemList[ i ];
		Data.DBID = ItemPtr->GUID;
		Data.BuyOutPrices = ItemPtr->BuyOutPrices;
		Data.LiveTime = ItemPtr->LiveTime;
		Data.NowPrices = ItemPtr->NowPrices;
		Data.Item = ItemPtr->Item;
		Data.Type = ItemPtr->PricesType;
		SC_SearchItemResult( Info->DBID , i , Data , (char*)ItemPtr->SellerName.Begin() , ItemPtr->BuyerDBID );
	}
	Iter->second.Pos = SendItemCount;
	delete Info;
	delete ItemDBIDList;
}
//----------------------------------------------------------------------------------------------------------------------------
bool CNetDC_ACChild::_SQLCmdBuyProc( vector<DB_ACItemStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	bool InsertOrder = ( Arg.GetNumber( "InsertOrder" ) != 0 ) ;
	char	Buf[1024];
	DB_ACItemStruct*		Item = (DB_ACItemStruct*)UserObj;
	//MutilThread_CritSect    Thread_CritSect;
	MyDbSqlExecClass	MyDBProc( sqlBase );	
	//清除資料庫資訊
	//如果非買斷加訂單資料 
	if( Item->State == EM_ACItemState_Normal )
	{
		//更新資料
		CharTransferClass	CharTransfer;
		CharTransfer.SetUtf8String( Item->BuyerName.Begin() );
		string BuyerNameBinStr = StringToSqlX( (char*)CharTransfer.GetWCString() , CharTransfer.WCStrLen() * 2 , false );
		sprintf( Buf , "UPDATE AC_Base SET NowPrices = %d , BuyerDBID = %d , BuyerName = CAST( %s AS nvarchar(4000) ) WHERE (GUID = %d) " 
			, Item->NowPrices , Item->BuyerDBID , BuyerNameBinStr.c_str() , Item->GUID );
		MyDBProc.SqlCmd_WriteOneLine( Buf );
		if( InsertOrder != false )
		{
			sprintf( Buf , "INSERT INTO AC_Order (ACBaseGUID, BuyerDBID) VALUES (%d, %d)" , Item->GUID , Item->BuyerDBID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
	}
	else if( Item->State == EM_ACItemState_BuyOut )
	{
		//資料刪除
		//sprintf( Buf , "DELETE FROM AC_Base	WHERE (GUID = %d)" , Item->GUID );
		sprintf( Buf , "UPDATE AC_Base SET DestroyTime=getdate() WHERE (GUID = %d)" , Item->GUID );
		MyDBProc.SqlCmd_WriteOneLine( Buf );
		//處理歷史資料
		int MaxHistroyItemDBID = 0;
		MyDBProc.SqlCmd( "SELECT MAX(GUID) AS Expr1 FROM AC_History" );
		MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&MaxHistroyItemDBID );
		MyDBProc.Read();
		MyDBProc.Close();
		DB_ACItemHistoryStruct* HistoryData = new DB_ACItemHistoryStruct;
		HistoryData->BuyerDBID = Item->BuyerDBID;
		HistoryData->BuyerName = Item->BuyerName;
		HistoryData->Date	  = TimeStr::Now_Value();
		HistoryData->GUID	  = MaxHistroyItemDBID + 1;
		HistoryData->Item = Item->Item;
		HistoryData->Prices = Item->NowPrices;
		HistoryData->PricesType = Item->PricesType;
		HistoryData->SellerDBID = Item->SellerDBID;
		if( Item->SellerDBID != -1 )
			HistoryData->SellerName = Item->SellerName;
		else
			HistoryData->SellerName = "[SYS_AC]"/*g_ObjectData->GetString( "SYS_AC" )*/;
		//g_CritSect()->Enter();
		MyDBProc.SqlCmd_WriteOneLine( _RDACHistorySql->GetInsertStr( HistoryData).c_str() );
		//g_CritSect()->Leave();
		Arg.PushValue("HistoryData", UINT64(HistoryData));
	}
	return true;
}
void CNetDC_ACChild::_SQLCmdBuyProcResult ( vector<DB_ACItemStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	DB_ACItemHistoryStruct *HistoryData = (DB_ACItemHistoryStruct *)Arg.GetUINT64("HistoryData");
	DB_ACItemStruct*		Item = (DB_ACItemStruct*)UserObj;
	if(HistoryData)
	{
		AddHistoryData( HistoryData );
		delete HistoryData;
	}
	delete Item;
}
//增加歷史計錄
bool CNetDC_ACChild::AddHistoryData( DB_ACItemHistoryStruct* HistoryData )
{
	if( HistoryData == NULL )
		return false;
	ACItemHistoryTypeStruct Type;
	Type.OrgObjID = HistoryData->Item.OrgObjID;
	//Type.Inherent = HistoryData->Item.Inherent;
	Type.Inherent = 0;
	vector< DB_ACItemHistoryStruct >& DataList = _HistoryList[ Type ];
	DataList.push_back( *HistoryData );
	while( DataList.size() > _MAX_HISTORY_ITEM_COUNT_ )
	{
		int GUID = DataList[0].GUID;
		//g_CritSect()->Enter();
		_WaitDelACHistoryList.push_back(  GUID );
		//g_CritSect()->Leave();
		DataList.erase( DataList.begin() );
	}
	return true;
}
//----------------------------------------------------------------------------------------------------------------------------
bool CNetDC_ACChild::_SQLCmdOpenProc( vector<DB_ACItemStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char	Buf[1024];
	int     DBID       = Arg.GetNumber( "DBID" );
	sprintf( Buf , "SELECT ACBaseGUID FROM AC_Order WHERE (BuyerDBID = %d)" , DBID );
	MyDbSqlExecClass	MyDBProc( sqlBase );
	vector<int>*		BuyItemDBIDList = new vector<int>;
	int					ItemDBID;
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ItemDBID );
	while( MyDBProc.Read() )
	{
		BuyItemDBIDList->push_back( ItemDBID );
	}
	MyDBProc.Close();
	vector<int>*		SellItemDBIDList = new vector<int>;
	sprintf( Buf , "SELECT GUID FROM AC_Base WHERE DestroyTime = 0 and (SellerDBID = %d)" , DBID );
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ItemDBID );
	while( MyDBProc.Read() )
	{
		SellItemDBIDList->push_back( ItemDBID );
	}
	MyDBProc.Close();
	//------------------------------------------------------------------------------------------------------------
	Arg.PushValue("BuyItemDBIDList", UINT64(BuyItemDBIDList));
	Arg.PushValue("SellItemDBIDList", UINT64(SellItemDBIDList));
	return true;
}
void CNetDC_ACChild::_SQLCmdOpenProcResult ( vector<DB_ACItemStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	vector<int> *BuyItemDBIDList = (vector<int> *)Arg.GetUINT64("BuyItemDBIDList");
	vector<int> *SellItemDBIDList = (vector<int> *)Arg.GetUINT64("SellItemDBIDList");
	int		DBID	= Arg.GetNumber( "DBID" );
	int		NpcGUID = Arg.GetNumber( "NpcGUID" );
	//通知Client
	SC_OpenAC( DBID , NpcGUID , true );
	map< int , PlayerSelectManageStruct >::iterator Iter;
	Iter = _PlayerList.find( DBID );
	if( Iter == _PlayerList.end() )
		return;
	PlayerSelectManageStruct& PlayerData = Iter->second;
	PlayerData.OnDbSearchFlag = false;
	PlayerData.SellItemCount = SellItemDBIDList ? (int)SellItemDBIDList->size() : 0;
	PlayerData.OrderSet.clear();
	if( BuyItemDBIDList )
	{
		for( int i = 0 ; i < (int)BuyItemDBIDList->size() ; i++ )
		{
			int ACItemDBID = (*BuyItemDBIDList)[i];
			PlayerData.OrderSet.insert( ACItemDBID );
			map< int , DB_ACItemStructPtr >::iterator Iter;
			Iter = _AllItemList.find( ACItemDBID );
			if( Iter == _AllItemList.end() )
				continue;
			DB_ACItemStructPtr ItemPtr = Iter->second;
			ACItemInfoStruct Data;
			Data.BuyOutPrices = ItemPtr->BuyOutPrices;
			Data.DBID = ItemPtr->GUID;
			Data.Item = ItemPtr->Item;
			Data.LiveTime = ItemPtr->LiveTime;
			Data.NowPrices = ItemPtr->NowPrices;
			Data.Type = ItemPtr->PricesType;
			CNetDC_AC::SC_SelfCompetitive_BuyItem( DBID , ItemPtr->BuyerDBID , Data , (char*)ItemPtr->SellerName.Begin() );
		}
	}
	//找出所有某玩家賣的東西
	if(SellItemDBIDList)
	{
		for( int i = 0 ; i < (int)SellItemDBIDList->size() ; i++ )
		{
			int ACItemDBID = (*SellItemDBIDList)[i];
			map< int , DB_ACItemStructPtr >::iterator Iter;
			Iter = _AllItemList.find( ACItemDBID );
			if( Iter == _AllItemList.end() )
				continue;
			DB_ACItemStructPtr ItemPtr = Iter->second;
			ACItemInfoStruct Data;
			Data.BuyOutPrices = ItemPtr->BuyOutPrices;
			Data.DBID = ItemPtr->GUID;
			Data.Item = ItemPtr->Item;
			Data.LiveTime = ItemPtr->LiveTime;
			Data.NowPrices = ItemPtr->NowPrices;
			Data.Type	= ItemPtr->PricesType;
			CNetDC_AC::SC_SelfCompetitive_SellItem( DBID , Data , (char*)ItemPtr->BuyerName.Begin() );
		}
	}
	SC_SelfCompetitive_ItemDataEnd( DBID );
	if( SellItemDBIDList )
		delete SellItemDBIDList;
	if(BuyItemDBIDList)
		delete BuyItemDBIDList;
}
//----------------------------------------------------------------------------------------------------------------------------
bool CNetDC_ACChild::_SQLOnTimeProc( vector<DB_ACItemStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	vector< DB_ACItemStructPtr >&  ProcItemList = *(( vector< DB_ACItemStructPtr >*) UserObj);
	//所有物件生存時間下降10
	char					Buf[1024];
	//MutilThread_CritSect	Thread_CritSect;
	MyDbSqlExecClass		MyDBProc( sqlBase );
//	g_CritSect_Thread()->Enter();
	{	
		//		MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );
		//------------------------------------------------------------------------------------------
		//所有信件減時間	
		sprintf( Buf , "UPDATE AC_Base SET LiveTime = LiveTime - 10 WHERE DestroyTime = 0" );
		MyDBProc.SqlCmd_WriteOneLine( Buf );
		//------------------------------------------------------------------------------------------
		//刪除所有過期的物件
		//sprintf( Buf , "DELETE AC_Base WHERE LiveTime < 0" );
		sprintf( Buf , "UPDATE AC_Base SET DestroyTime = getdate() WHERE LiveTime < 0 and DestroyTime = 0 ");
		MyDBProc.SqlCmd_WriteOneLine( Buf );
		//------------------------------------------------------------------------------------------
		//取得目前時間
		int		NowTime;
		NowTime = TimeStr::Now_Value();
		vector< DB_ACItemHistoryStruct >* ACItemHistoryList = new vector< DB_ACItemHistoryStruct >;
		for( int i = 0 ; i < (int)ProcItemList.size() ; i++ )
		{
			DB_ACItemStruct& Item = *( ProcItemList[i].get() );
			if( Item.State != EM_ACItemState_BuyOut )
				continue;
			int MaxHistroyItemDBID = 0;
			MyDBProc.SqlCmd( "SELECT MAX(GUID) AS Expr1 FROM AC_History" );
			MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&MaxHistroyItemDBID );
			MyDBProc.Read();
			MyDBProc.Close();
			DB_ACItemHistoryStruct HistoryData;
			HistoryData.BuyerDBID = Item.BuyerDBID;
			HistoryData.BuyerName = Item.BuyerName;
			HistoryData.Date	  = NowTime;
			HistoryData.GUID	  = MaxHistroyItemDBID+1;
			HistoryData.Item = Item.Item;
			HistoryData.Prices = Item.NowPrices;
			HistoryData.SellerDBID = Item.SellerDBID;
			HistoryData.SellerName = Item.SellerName;
			MyDBProc.SqlCmd_WriteOneLine( _RDACHistorySql->GetInsertStr( &HistoryData ).c_str() );
			ACItemHistoryList->push_back( HistoryData );
		}
		Arg.PushValue("ACItemHistoryList", UINT64(ACItemHistoryList));
		//------------------------------------------------------------------------------------------
		//清除所有的過期歷史記錄
		//------------------------------------------------------------------------------------------
		for( int i = 0 ; i < (int)_ProcDelACHistoryList.size() ; i++ )
		{
			//刪除所有過期的物件
			//sprintf( Buf , "DELETE AC_History WHERE GUID = %d" , _ProcDelACHistoryList[i] );
			sprintf( Buf , "UPDATE AC_History SET DestroyTime=getdate() WHERE GUID = %d" , _ProcDelACHistoryList[i] );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
		//		MyDBProc.SqlCmd_WriteOneLine( "COMMIT TRANSACTION" );
	}	
//	g_CritSectTreadOnly()->Leave();
	_ProcDelACHistoryList.clear();	
	//------------------------------------------------------------------------------------------
	return true;
}
void CNetDC_ACChild::_SQLOnTimeProcResult ( vector<DB_ACItemStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	vector<DB_ACItemHistoryStruct> *ACItemHistoryList = (vector<DB_ACItemHistoryStruct> *)Arg.GetUINT64("ACItemHistoryList");
	vector< DB_ACItemStructPtr >*  ProcItemList = (( vector< DB_ACItemStructPtr >*) UserObj);
	if(ACItemHistoryList)
	{
		for( int i = 0 ; i < (int) ACItemHistoryList->size() ; i++ )
		{
			DB_ACItemHistoryStruct& HistoryData = (*ACItemHistoryList)[i];
			AddHistoryData( &HistoryData );
		}
		delete ACItemHistoryList;
	}
	if(ProcItemList)
	{
		delete ProcItemList;
	}
}
//////////////////////////////////////////////////////////////////////////
struct SysACSellItemTempInfo
{
	int				GUID;
	ItemFieldStruct Item;
	int				Prices;
	int				BuyOutPrices;	
	PriceTypeENUM	PricesType; 
	int				LiveTime;
};
bool CNetDC_ACChild::_SQLImportOnTimeProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char	Buf[1024];
	vector< SysACSellItemTempInfo >* ResultList = new ( vector< SysACSellItemTempInfo > );
	Arg.PushValue("ResultList", UINT64(ResultList));
	SysACSellItemTempInfo TempInfo;
	MyDbSqlExecClass	MyDBProc( sqlBase );
	sprintf( Buf , "SELECT GUID, LiveTime, NowPrices, BuyOutPrices, PricesType, OrgObjID, Count, Durable, Ability, Mode, ExValue, ImageObjectID , Serial , CreateTime FROM ImportACItem WHERE (InsertTime = 0) AND (WorldID=%d)" , Global::GetWorldID()  );
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.GUID );
	MyDBProc.Bind( 2, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.LiveTime );
	MyDBProc.Bind( 3, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.Prices  );
	MyDBProc.Bind( 4, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.BuyOutPrices );
	MyDBProc.Bind( 5, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.PricesType );
	MyDBProc.Bind( 6, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.Item.OrgObjID );
	MyDBProc.Bind( 7, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.Item.Count );
	MyDBProc.Bind( 8, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.Item.Durable );
	MyDBProc.Bind( 9, SQL_C_BINARY, sizeof(TempInfo.Item.Ability) , (LPBYTE)&TempInfo.Item.Ability );
	MyDBProc.Bind( 10, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.Item.Mode._Mode );
	MyDBProc.Bind( 11, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.Item.ExValue );
	MyDBProc.Bind( 12, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.Item.ImageObjectID );
	MyDBProc.Bind( 13, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.Item.Serial );
	MyDBProc.Bind( 14, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.Item.CreateTime );
	while( MyDBProc.Read() )
	{
		ResultList->push_back( TempInfo );
	}
	MyDBProc.Close();
	//更新處理時間
	for( unsigned int i = 0 ; i < ResultList->size() ; i++ )
	{
		sprintf( Buf , "UPDATE ImportACItem SET InsertTime = getdate() WHERE GUID = %d" , (*ResultList)[i].GUID );
		MyDBProc.SqlCmd_WriteOneLine( Buf );
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
void CNetDC_ACChild::_SQLImportOnTimeProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	static int SerialID = 0;
	vector<SysACSellItemTempInfo> &ResultList = *((vector<SysACSellItemTempInfo> *)Arg.GetUINT64("ResultList"));
	for( unsigned int i = 0 ; i < ResultList.size() ; i++ )
	{
		ItemFieldStruct& Item = ResultList[i].Item;
		//取出物品資訊
		GameObjDbStructEx* ItemDB = g_ObjectData->GetObj( Item.OrgObjID );
		if( ItemDB == NULL )
			continue;
		if( Item.CreateTime == 0 && Item.Serial == 0 )
		{
			Item.CreateTime = RoleDataEx::G_Now;
			Item.Serial = SerialID++;
		}
		if( ItemDB->IsWeapon() || ItemDB->IsArmor() )
		{
			if( Item.ExValue == 0 )
			{
				Item.OrgQuality = 100;		//原始的品質
				Item.Quality = 100;		//品質
				Item.PowerQuality = 10;	//威力品質( 1 為 10%強度)
				Item.Level = 0;	//等級
				Item.RuneVolume = 0;	//符文容量			
			}
			if( ItemDB->Mode.DurableStart0 != false )
				Item.Durable = 0;
			else
				Item.Durable = ItemDB->Item.Durable * Item.Quality;
			//Item.Durable = ItemDB->Item.Durable * Item.Quality;
		}
		_SysSellItem(  ResultList[i].Item , ResultList[i].Prices , ResultList[i].BuyOutPrices , ResultList[i].PricesType , ResultList[i].LiveTime );
	}
	delete &ResultList;
}
void CNetDC_ACChild::_SysSellItem			( ItemFieldStruct& Item , int Prices , int BuyOutPrices , PriceTypeENUM	PricesType , int LiveTime )
{	
	ACSellItemTempInfo*	Info = new ACSellItemTempInfo;
	Info->SellerDBID = -1;
	Info->SrvSockID = -1;
	Info->Item = Item;
	Info->Prices = Prices;
	Info->BuyOutPrices = BuyOutPrices;
	Info->SellerName = "";
	Info->LiveTime = LiveTime;
	Info->TaxMoney = 0;
	Info->PricesType = PricesType;
	_RD_ACItemDB->SqlCmd( SELL_ACITEM_THREADID , _SQLCmdSellProc , _SQLCmdSellProcResult , Info	, NULL );
}