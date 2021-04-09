#include "../NetWakerGSrvInc.h"
#include "NetSrv_LotteryChild.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"

//--------------------------------------------------------------------------------------------------------------
bool    NetSrv_LotteryChild::Init()
{
	NetSrv_Lottery::_Init();

	if( This != NULL)
		return false;

	This = NEW( NetSrv_LotteryChild );

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool    NetSrv_LotteryChild::Release()
{
	if( This == NULL )
		return false;

	NetSrv_Lottery::_Release();

	delete This;
	This = NULL;

	return true;

}
//--------------------------------------------------------------------------------------------------------------
void NetSrv_LotteryChild::RC_ExchangePrize		( BaseItemObject* Obj , ItemFieldStruct& Item , int Pos )
{
	RoleDataEx* Owner = Obj->Role();
	ItemFieldStruct* BodyItem = Owner->GetBodyItem( Pos );
	if( BodyItem == NULL || *BodyItem != Item )
	{
		SC_ExchangePrizeResultFailed( Owner->GUID() );
		return;
	}
	GameObjDbStructEx* ItemDB = Global::GetObjDB( BodyItem->OrgObjID );
	if( ItemDB->IsItem_Pure() == false || ItemDB->Item.ItemType != EM_ItemType_Lottery )
	{
		SC_ExchangePrizeResultFailed( Owner->GUID() );
		return;
	}

	SD_ExchangePrize( *BodyItem , Pos , Owner->DBID() );
}
void NetSrv_LotteryChild::RC_CloseExchangePrize	( BaseItemObject* Obj )
{
	RoleDataEx* Role = Obj->Role();
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_ExchangeLottery) == false )
		return;

	Role->ClsOpenMenu();	
}

/*
int LotteryCount( int BuyCount )
{

	if( BuyCount == 5 )
		return 1;

	if( BuyCount > 5 )
		return MathCombination( BuyCount , 5 );

	return MathCombination( 20 - BuyCount , 5-BuyCount );
}
*/

void NetSrv_LotteryChild::RC_BuyLottery			( BaseItemObject* Obj , int Money , int Count , int Number[20] )
{
	bool FindNumber[21];
	memset( FindNumber , 0 , sizeof(FindNumber) );

	RoleDataEx* Role = Obj->Role();

	RoleDataEx* Shop = Global::GetRoleDataByGUID( Role->TempData.ShopInfo.TargetID );
	if( Shop == NULL )
		return;

	if( Role->PlayerBaseData->Body.Money < Money || Count <=0 || Count > 20 )
	{
		SC_BuyLotteryResult( Role->GUID() , false );
		return; 
	}

	LotteryInfoTableStruct* LotteryInfoTable = g_ObjectData->LotteryInfoTable( Count );

	if( LotteryInfoTable == NULL || LotteryInfoTable->CombinationCount * DEF_LOTTERY_COST != Money )
	{
		SC_BuyLotteryResult( Role->GUID() , false );
		return; 
	}

	//檢查是否數字合理
	for( int i = 0 ; i < Count ; i++ )
	{
		int N = Number[ i ];
		if( N > 20 || N < 1			//號碼大小出問題
			|| FindNumber[ N ]  )	//號碼重覆
		{
			SC_BuyLotteryResult( Role->GUID() , false );
			return;
		}
		FindNumber[ N ]=true;
		
	}
	//先扣錢
	Role->AddBodyMoney( Money * -1 , Shop , EM_ActionType_Buy_Lottery , true );


	//通知
	SD_BuyLottery( Money , Role->DBID() , Count , Number );

}
void NetSrv_LotteryChild::RC_CloseBuyLottery	( BaseItemObject* Obj )
{
	RoleDataEx* Role = Obj->Role();
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_BuyLottery) == false )
		return;

	Role->ClsOpenMenu();	
}
void NetSrv_LotteryChild::RC_LotteryInfoRequest	( BaseItemObject* Obj )
{
	SD_LotteryInfoRequest( Obj->Role()->DBID() );
}
void NetSrv_LotteryChild::RC_LotteryHistoryInfoRequest	( BaseItemObject* Obj )
{
	SD_LotteryHistoryInfoRequest( Obj->Role()->DBID() );
}
void NetSrv_LotteryChild::RD_ExchangePrizeResult( ItemFieldStruct& Item , int Pos , int PlayerDBID , int PrizeCount[3] , int TotalMoney , bool Result )
{
	BaseItemObject* OwnerObj = Global::GetPlayerObj_ByDBID( PlayerDBID );
	if( OwnerObj == NULL )
		return;


	RoleDataEx* Owner = OwnerObj->Role();

	ItemFieldStruct* BodyItem = Owner->GetBodyItem( Pos );

	if( BodyItem == NULL || *BodyItem != Item || Result == false )
	{
		SC_ExchangePrizeResultFailed( Owner->GUID() );
		return;
	}

	if( Owner->AddBodyMoney( TotalMoney , NULL , EM_ActionType_Lottery_Prize , true ) == false )
	{
		Owner->Msg( "Lottery money too many" );
		SC_ExchangePrizeResultFailed( Owner->GUID() );
		return;
	}
	Log_ItemDestroy( Owner , EM_ActionType_UseItemDestroy , *BodyItem , -1 , -1 , "" );
	BodyItem->Init();
	Owner->Net_FixItemInfo_Body( Pos );

	SC_ExchangePrizeResultOK( Owner->GUID() , TotalMoney , PrizeCount[0] , PrizeCount[1] , PrizeCount[2] );
}
void NetSrv_LotteryChild::RD_BuyLotteryResult( int Version , int Money , int PlayerDBID , int Count, int Number[20] , bool Result )
{
	BaseItemObject* OwnerObj = Global::GetPlayerObj_ByDBID( PlayerDBID );
	if( OwnerObj == NULL )
	{
		Print( LV4 , "RD_BuyLotteryResult" , "OwnerObj == NULL Money = %d PlayerDBID=%d" , Money , PlayerDBID );
		return;
	}

	RoleDataEx* Owner = OwnerObj->Role();

	RoleDataEx* Shop = Global::GetRoleDataByGUID( Owner->TempData.ShopInfo.TargetID );
	if( Shop == NULL )
		return;

	if( Result == false )
	{
		//退回金錢
		Owner->AddBodyMoney( Money  , Shop , EM_ActionType_Buy_Lottery , true );
	}
	else
	{
		
		//給彩票
		ItemFieldStruct	Item;
		Global::NewItemInit( Item , _DEF_ITEMID_LOTTERY_ , 0 );
		Item.Count = 1;
		Item.Lottery.Count = Count;
		Item.Lottery.Index = Version;

		for( int i = 0 ; i < Count ; i++ )
		{
			Item.Lottery.SelectID[i] = (char)Number[i];
		}
		Owner->GiveItem( Item , EM_ActionType_Buy_Lottery , Shop , "" );

	}
}