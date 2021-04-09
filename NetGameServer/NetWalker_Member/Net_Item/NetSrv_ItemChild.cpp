#include "../NetWakerGSrvInc.h"
#include "NetSrv_ItemChild.h"
#include "../../mainproc/magicproc/MagicProcess.h"
#include "MD5/Mymd5.h"
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
bool    NetSrv_ItemChild::Init()
{
    NetSrv_Item::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_ItemChild );

    return true;
}
//--------------------------------------------------------------------------------------------------------------
bool    NetSrv_ItemChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_Item::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//--------------------------------------------------------------------------------------------------------------
int  NetSrv_ItemChild::_PowerLightProc( SchedularInfo* Obj , void* InputClass )
{
	int OwnerID = Obj->GetNumber( "OwnerID" );
	int MagicID = Obj->GetNumber( "MagicID" );
	RoleDataEx* Owner = Global::GetRoleDataByGUID( OwnerID );
	if( Owner == NULL )
		return 0;

	MagicProcInfo  MagicInfo;
	MagicInfo.Init();
	MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
	MagicInfo.TargetID			= OwnerID;				//目標
	MagicInfo.MagicCollectID	= MagicID;	//法術
	MagicInfo.MagicLv			= 0;
	MagicProcessClass::SysSpellMagic ( Owner , &MagicInfo );

	return 0;
}
void NetSrv_ItemChild::R_GetGroundItem               ( BaseItemObject* Sender , int ItemGUID , int Pos )
{
    //檢查此物品是否可檢取
    RoleDataEx *Owner = Sender->Role();
    RoleDataEx *Item  = Global::GetRoleDataByGUID( ItemGUID );
    int  SendID = Sender->GUID();
    if(		Owner == NULL || Owner->IsDestroy() 
        ||	Item  == NULL || Item->IsDestroy()		)
    {
        GetItemFailed( SendID );
        return;
    }

	if( Owner->Length( Item ) > 50 )
	{
		GetItemFailed( SendID );
		return;
	}

    //取物品原型資訊
    GameObjDbStructEx*	OrgDB;
    OrgDB = Global::GetObjDB( Item->BaseData.ItemInfo.OrgObjID );

    if( OrgDB == NULL )
    {
        GetItemFailed( SendID );
        //Owner->Msg( "物件資料有問題" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_OrgObjIDError );
        return;
    }

    if( OrgDB->Classification != EM_ObjectClass_Item )
    {
        GetItemFailed( SendID );
        //Owner->Msg( "物件資料有問題 非物品" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_NotItem );
        return;
    }

	//效果光球處理
	if( OrgDB->Item.ItemType == EM_ItemType_PowerLight )
	{
		switch( OrgDB->Item.UseType )
		{
		case EM_ItemUseType_None:
		case EM_ItemUseType_SrvScript:
		case EM_ItemUseType_CliScript:

			break;
		case EM_ItemUseType_Magic:
			{
				Schedular()->Push( _PowerLightProc , 2000 , NULL 
					,"OwnerID" , EM_ValueType_Int , Owner->GUID()
					,"MagicID" , EM_ValueType_Int , OrgDB->Item.IncMagic_Onuse
					, NULL );
				/*
				MagicProcInfo  MagicInfo;
				MagicInfo.Init();
				MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
				MagicInfo.TargetID			= Owner->GUID();				//目標
				MagicInfo.MagicCollectID	= OrgDB->Item.IncMagic_Onuse;	//法術
				MagicInfo.MagicLv			= 0;
				MagicProcessClass::SysSpellMagic ( Owner , &MagicInfo );
				*/
				SendRange_UsePowerLight( Owner->GUID() , ItemGUID );
			}
			break;
		}
		GetItemOK( SendID );
		Item->Destroy( "R_GetGroundItem-1");
		return;
		
	}

    switch( Owner->AddItemtoBody( Item->BaseData.ItemInfo , Pos ) )
    {
    case EM_RoleData_PosErr :
        {
            //Owner->Msg( "要求放置欄位有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_PosError );
            GetItemFailed( SendID );
            return;
        }
    case EM_RoleData_OK:
		Owner->Log_ItemTrade( NULL , EM_ActionType_Get , Item->BaseData.ItemInfo , "" );
        break;
    }

    Item->Destroy("R_GetGroundItem-2");
    GetItemOK( SendID , Owner->PlayerBaseData->Body.Item[ Pos ] , Pos ); 
}

void NetSrv_ItemChild::R_GetBufferItem               ( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos , int Count ) 
{
    RoleDataEx *Owner = Sender->Role();
    if( Owner == NULL )
        return;
    int SendID = Sender->GUID();

	if( Count <= 0  )
		return;


    if( Pos == -1 )
    {
        if( Owner->PlayerBaseData->ItemTemp.Front() == Item )
        {            
            DelBufferItemOK( SendID , Item );
			Owner->PlayerBaseData->ItemTemp.Erase( 0 );
        }
        else
        {
			Owner->Net_FixItemInfo_Body( Pos );
            GetBufferItemFailed( SendID );   
        }
        return;
	}


    switch( Owner->GetItemBuftoBody( Item , Pos , Count ) )
    {
    case EM_RoleData_OK:
        //Item.Count = Count;
        GetBufferItemOK( SendID , Item  , Pos , Count );
        return;
    case EM_RoleData_PosErr:
        //Owner->Msg( "(要求取Buffer內資料失敗)放置欄位有問題" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_Queue_PosError );
        break;
    case EM_RoleData_BufferEmpty:
        //Owner->Msg( "(要求取Buffer內資料失敗)Buffer內沒資料" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_Queue_BufferEmpty );
        break;
    case EM_RoleData_DataInfoErr:
        //Owner->Msg( "要求資料有問題 送來的資料與Server資料不符" );          
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_Queue_DataError );
        break;
    case EM_RoleData_CountErr:
        //Owner->Msg( "要求資料有問題 數量有問題" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_Queue_CountError );
        break;
	case EM_RoleData_IsEmpty:
		{
			ItemFieldStruct TempItem;
			TempItem.Init();
			GetBufferItemOK( SendID , TempItem  , Pos , Item.Count );
		}
		break;
    }

	Owner->Net_FixItemInfo_Body( Pos );
    GetBufferItemFailed( SendID );   

	ClsItemInBuffer( SendID );
    //更新資料
    for( int i = 0 ; i < Owner->PlayerBaseData->ItemTemp.Size() ; i++ )
    {
        NewItemInBuffer( Owner->GUID() , Owner->PlayerBaseData->ItemTemp[i] );
    }

}

void NetSrv_ItemChild::R_DeleteItem_Body             ( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos )
{
    RoleDataEx *Owner   = Sender->Role();
    int         SendID  = Sender->GUID();
    if( Owner == NULL )
        return;

	if( Owner->BaseData.SysFlag.DisableTrade != false )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
		Owner->Net_FixItemInfo_Body( Pos );
		DeleteItemFailed( SendID );
		return;
	}

	if( Item.Mode.ItemLock != false )
	{
		DeleteItemFailed( SendID );
		Owner->Net_FixItemInfo_Body( Pos );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_ItemLock );
		return;
	}
    
    switch( Owner->DelItemFromBody( Item , Pos ) )
    {
    case EM_RoleData_PosErr:
    case EM_RoleData_DataInfoErr:
        {//交易資料有問題
            DeleteItemFailed( SendID );
            //Owner->Msg( "物品資料刪除有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_DelError );
            return;
        }
    case EM_RoleData_OK:
		{		
			
		}		
        break;
    }

    Owner->TempData.UpdateInfo.Body = true;
    DeleteItemOK_Body( SendID , Item  , Pos );
    
}
void NetSrv_ItemChild::R_DeleteItem_Bank             ( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos )
{
    RoleDataEx *Owner   = Sender->Role();
    int         SendID  = Sender->GUID();
    if( Owner == NULL )
        return;


	if( Item.Mode.ItemLock != false )
	{
		Owner->Net_FixItemInfo_Bank( Pos );
		DeleteItemFailed( SendID );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_ItemLock );
		return;
	}

    switch( Owner->DelItemFromBank( Item , Pos ) )
    {
    case EM_RoleData_PosErr:
    case EM_RoleData_DataInfoErr:
        {//交易資料有問題
            DeleteItemFailed( SendID );
            //Owner->Msg( "物品資料刪除有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_DelError );
            return;
        }
    case EM_RoleData_OK:
		{
			//
		}
        break;
    }

    Owner->TempData.UpdateInfo.Body = true;
    DeleteItemOK_Bank( SendID , Item , Pos );

}

void NetSrv_ItemChild::R_DeleteItem_EQ              ( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos )
{
    RoleDataEx *Owner   = Sender->Role();
    int         SendID  = Sender->GUID();
    if( Owner == NULL )
        return;

	if( Item.Mode.ItemLock != false )
	{
		Owner->Net_FixItemInfo_EQ( Pos );
		DeleteItemFailed( SendID );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_ItemLock );
		return;
	}

    switch( Owner->DelItemFromEQ( Item , Pos ) )
    {
    case EM_RoleData_PosErr:
    case EM_RoleData_DataInfoErr:
        {//交易資料有問題
            DeleteItemFailed( SendID );
            //Owner->Msg( "物品資料刪除有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_DelError );
            return;
        }
    case EM_RoleData_OK:
		{
		
		}
        break;
    }

    Owner->TempData.UpdateInfo.Body = true;
    DeleteItemOK_EQ( SendID , Item , Pos );
}

void NetSrv_ItemChild::R_ExchangeField_Body          ( BaseItemObject* Sender , ItemFieldStruct& Item1 , int Pos1 
                                                                              , ItemFieldStruct& Item2 , int Pos2 , int ClientID )
{
	char Buf[512];
    RoleDataEx *Owner   = Sender->Role();
    int         SendID  = Sender->GUID();
    if( Owner == NULL )
        return;

    switch( Owner->ExchangeBodyItem( Item1 , Pos1 , Item2 , Pos2 ) )
    {
    case EM_RoleData_PosErr:
        {
            Print( LV5 , "R_ExchangeField_Body" , "Pos Err Account = %s" , Owner->Account_ID() );
            ExchangeFieldFailed_Body( SendID , *Owner->GetBodyItem( Pos1 ) , Pos1 , *Owner->GetBodyItem(Pos2) , Pos2 , ClientID );
			
			sprintf( Buf , "Bag(Bag->Bag)Data Exchange Err RoleName=%s Pos1=%d  Pos2 =%d " , Utf8ToChar( Owner->RoleName() ).c_str() , Pos1 , Pos2  );
			Global::SendMsgToGMTools( LV5 , Owner->Account_ID() , Buf );
			//Sender->Destroy( Buf );
            return;
        }
        break;
    case EM_RoleData_OrgObjIDErr:
    case EM_RoleData_DataInfoErr:
        {
            //Owner->Msg( "R_ExchangeField_Body 交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_ExchangeError );

            ExchangeFieldFailed_Body( SendID , *Owner->GetBodyItem( Pos1 ) , Pos1 , *Owner->GetBodyItem(Pos2) , Pos2 , ClientID );
            return;
        }
    case EM_RoleData_OK:
        ExchangeField_Body( SendID , *Owner->GetBodyItem( Pos1 ) , Pos1 , *Owner->GetBodyItem(Pos2) , Pos2 , ClientID );
        break;
    }

    return;
}
void NetSrv_ItemChild::R_ExchangeField_Bank          ( BaseItemObject* Sender , ItemFieldStruct& Item1 , int Pos1 
                                                                              , ItemFieldStruct& Item2 , int Pos2  , int ClientID )
{
	char	Buf[512];
    RoleDataEx *Owner = Sender->Role();
    int         SendID  = Sender->GUID();
    if( Owner == NULL )
        return;

	if( Owner->TempData.Attr.Action.OpenType != EM_RoleOpenMenuType_Bank )
	{
		//銀行沒開
		ExchangeFieldFailed_Bank( SendID , *Owner->GetBankItem( Pos1 ) , Pos1 , *Owner->GetBankItem(Pos2) , Pos2 , ClientID );

		sprintf( Buf , "bag(bank->bank)Bank Not Open , RoleName=%s Pos1=%d  Pos2 =%d " , Utf8ToChar( Owner->RoleName() ).c_str() , Pos1 , Pos2  );
		Global::SendMsgToGMTools( LV5 , Owner->Account_ID() , Buf );
//		Sender->Destroy( Buf );
		return;
	}

    switch( Owner->ExchangeBankItem( Item1 , Pos1 , Item2 , Pos2 ) )
    {
    case EM_RoleData_PosErr:
        {
            Print( LV5 , "R_ExchangeField_Bank" , "Pos Err Account = %s" , Owner->Account_ID() );
			sprintf( Buf , "bag(bank->bank)Pos Error, RoleName=%s Pos1=%d  Pos2 =%d " , Utf8ToChar( Owner->RoleName() ).c_str() , Pos1 , Pos2  );
			Global::SendMsgToGMTools( LV5 , Owner->Account_ID() , Buf );
//			Sender->Destroy( Buf );
            return;
        }
        break;
    case EM_RoleData_OrgObjIDErr:
    case EM_RoleData_DataInfoErr:
        {
            //Owner->Msg( "R_ExchangeField_Bank 交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_ExchangeError );
            ExchangeFieldFailed_Bank( SendID , *Owner->GetBankItem( Pos1 ) , Pos1 , *Owner->GetBankItem(Pos2) , Pos2 , ClientID );
            return;
        }
    case EM_RoleData_OK:
        ExchangeField_Bank( SendID , *Owner->GetBankItem( Pos1 ) , Pos1 , *Owner->GetBankItem(Pos2) , Pos2 , ClientID );
        break;
    }

    return;
}
void NetSrv_ItemChild::R_ExchangeField_BodyBank      ( BaseItemObject* Sender , ItemFieldStruct& BodyItem , int BodyPos 
                                                                              , ItemFieldStruct& BankItem , int BankPos , int ClientID  )
{
	char		Buf[512];
	RoleDataEx *Owner = Sender->Role();
	int         SendID  = Sender->GUID();
	if( Owner == NULL )
		return;

	//距離檢查
	RoleDataEx* Bank = Global::GetRoleDataByGUID( Owner->TempData.ShopInfo.TargetID );
	if( Bank == NULL || Owner->TempData.Attr.Action.OpenType != EM_RoleOpenMenuType_Bank )
	{
		//Owner->Msg( "銀行開啟狀況有問題" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_BankOpenError );
		ExchangeField_BodyBank( SendID , *Owner->GetBodyItem( BodyPos ) , BodyPos , *Owner->GetBankItem(BankPos) , BankPos , ClientID );
		return;
	}
	if( Bank->Length( Owner ) > _MAX_ROLE_TOUCH_MAXRANGE_ * 2 )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_BankTooFar );
		//Owner->Msg( "銀行距離過遠" );
		Owner->TempData.Attr.Action.OpenType  = EM_RoleOpenMenuType_None;
		Owner->TempData.ShopInfo.TargetID = -1;
		CloseBank( Owner->GUID() );
		ExchangeField_BodyBank( SendID , *Owner->GetBodyItem( BodyPos ) , BodyPos , *Owner->GetBankItem(BankPos) , BankPos , ClientID );
		return;
	}

    switch( Owner->ExchangeBodyBankItem( BodyItem , BodyPos , BankItem , BankPos ) )
    {
    case EM_RoleData_PosErr:
        {
            Print( LV5 , "R_ExchangeField_BodyBank" , "Pos Error Account = %s" , Owner->Account_ID() );
			sprintf( Buf , "bag(bag->bank)Pos Error, RoleName=%s BodyPos=%d  BankPos =%d " , Utf8ToChar( Owner->RoleName() ).c_str() , BodyPos , BankPos  );
			Global::SendMsgToGMTools( LV5 , Owner->Account_ID() , Buf );
            //Sender->Destroy( Buf );
            return;
        }
        break;
    case EM_RoleData_OrgObjIDErr:
    case EM_RoleData_DataInfoErr:
        {
            //Owner->Msg( "R_ExchangeField_BodyBank 交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_ExchangeError );
            ExchangeFieldFailed_BodyBank( SendID , *Owner->GetBodyItem( BodyPos ) , BodyPos , *Owner->GetBankItem(BankPos) , BankPos , ClientID );
            return;
        }
    case EM_RoleData_OK:
        ExchangeField_BodyBank( SendID , *Owner->GetBodyItem( BodyPos ) , BodyPos , *Owner->GetBankItem(BankPos) , BankPos , ClientID );
        break;
    }

    return;
}
void NetSrv_ItemChild::R_ExchangeField_BankBody      ( BaseItemObject* Sender   , ItemFieldStruct& BankItem , int BankPos
                                                                                , ItemFieldStruct& BodyItem , int BodyPos , int ClientID  )
{
	char		Buf[512];
    RoleDataEx *Owner = Sender->Role();
    int         SendID  = Sender->GUID();
    if( Owner == NULL )
        return;

	//距離檢查
	RoleDataEx* Bank = Global::GetRoleDataByGUID( Owner->TempData.ShopInfo.TargetID );
	if( Bank == NULL || Owner->TempData.Attr.Action.OpenType != EM_RoleOpenMenuType_Bank )
	{
		//Owner->Msg( "銀行開啟狀況有問題" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_BankOpenError );

		ExchangeField_BodyBank( SendID , *Owner->GetBodyItem( BodyPos ) , BodyPos , *Owner->GetBankItem(BankPos) , BankPos , ClientID );
		return;
	}
	if( Bank->Length( Owner ) > _MAX_ROLE_TOUCH_MAXRANGE_ * 2 )
	{
		//Owner->Msg( "銀行距離過遠" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_BankTooFar );
		Owner->TempData.Attr.Action.OpenType  = EM_RoleOpenMenuType_None;
		Owner->TempData.ShopInfo.TargetID = -1;
		CloseBank( Owner->GUID() );
		ExchangeField_BodyBank( SendID , *Owner->GetBodyItem( BodyPos ) , BodyPos , *Owner->GetBankItem(BankPos) , BankPos , ClientID );
		return;
	}


    switch( Owner->ExchangeBankBodyItem( BankItem , BankPos , BodyItem , BodyPos  ) )
    {
    case EM_RoleData_PosErr:
        {
            Print( LV5 , "R_ExchangeField_BankBody" , "Pos Err Account = %s" , Owner->Account_ID() );
			sprintf( Buf , "bag(bank->bag)Pos Err RoleName=%s BodyPos=%d  BankPos =%d " , Utf8ToChar( Owner->RoleName() ).c_str() , BodyPos , BankPos  );
			Global::SendMsgToGMTools( LV5 , Owner->Account_ID() , Buf );
            //Sender->Destroy( Buf );
            return;
        }
        break;
    case EM_RoleData_OrgObjIDErr:
    case EM_RoleData_DataInfoErr:
        {
            //Owner->Msg( "R_ExchangeField_BankBody 交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_ExchangeError );
            ExchangeFieldFailed_BodyBank( SendID , *Owner->GetBodyItem( BodyPos ) , BodyPos , *Owner->GetBankItem(BankPos) , BankPos , ClientID );
            return;
        }
    case EM_RoleData_OK:
        ExchangeField_BodyBank( SendID , *Owner->GetBodyItem( BodyPos ) , BodyPos , *Owner->GetBankItem(BankPos) , BankPos , ClientID );
        break;
    }

    return;
}
void NetSrv_ItemChild::R_RequestBodyBankMoneyExchange( BaseItemObject* Sender , int Count )
{
    int SendID = Sender->GUID();
    RoleDataEx *Owner = Sender->Role();
    if( Owner == NULL )
        return;

    if( Count > _MAX_MONEY_COUNT_ )
        return;

    if(     Owner->PlayerBaseData->Body.Money + Count < 0 
        ||  Owner->PlayerBaseData->Bank.Money - Count < 0 
        ||  Owner->PlayerBaseData->Body.Money + Count > _MAX_MONEY_COUNT_
        ||  Owner->PlayerBaseData->Bank.Money - Count > _MAX_MONEY_COUNT_ )
    {
        //Owner->Msg( "金錢交換,資料有問題" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_ExchangeMoneyError );
        return;
    }
    Owner->PlayerBaseData->Body.Money += Count;
    Owner->PlayerBaseData->Bank.Money  -= Count;
    FixMoney( SendID , Owner->PlayerBaseData->Body.Money   , Owner->PlayerBaseData->Bank.Money , -1 , -1 );

    return;
}


//修正所有身上未取的資料
void NetSrv_ItemChild::FixAllBufferInfo( BaseItemObject* Sender )
{
    int SendID = Sender->GUID();
    RoleDataEx *Owner = Sender->Role();
    if( Owner == NULL )
        return;

    ClsItemInBuffer( SendID );

    for( int i = 0 ; i < Owner->PlayerBaseData->ItemTemp.Size() ; i++ )
    {
        NewItemInBuffer( SendID , Owner->PlayerBaseData->ItemTemp[i] );
    }

}


void NetSrv_ItemChild::R_RequestBufferInfo           ( BaseItemObject* Sender )
{
   FixAllBufferInfo( Sender ); 
}
void NetSrv_ItemChild::R_ApartBodyItemRequest        ( BaseItemObject* Sender  , ItemFieldStruct& Item , int PosSrc , int PosDes , int CountDes )
{
    int SendID = Sender->GUID();
    RoleDataEx *Owner = Sender->Role();
    if( Owner == NULL )
        return;

    if( CountDes <= 0 )
    {
        //Owner->Msg( "數量有問題" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_CountError );
		Owner->Net_FixItemInfo_Body( PosDes );
		Owner->Net_FixItemInfo_Body( PosSrc );
		S_ApartBodyItemResult( SendID , false );
        return;
    }

    if(     unsigned(PosSrc)  > (unsigned)Owner->PlayerBaseData->Body.Count 
        ||  unsigned(PosDes)  > (unsigned)Owner->PlayerBaseData->Body.Count )
    {
        //Owner->Msg( "放置物品位置有問題" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_PosError );
		Owner->Net_FixItemInfo_Body( PosDes );
		Owner->Net_FixItemInfo_Body( PosSrc );
		S_ApartBodyItemResult( SendID , false );
        return;
    }

    ItemFieldStruct& SrcItem = Owner->PlayerBaseData->Body.Item[PosSrc];
    ItemFieldStruct& DesItem = Owner->PlayerBaseData->Body.Item[PosDes];

    if(     ( DesItem.IsEmpty() != true	&& DesItem.OrgObjID != SrcItem.OrgObjID )
        ||	PosSrc > Owner->PlayerBaseData->Body.Count 
        ||	PosDes > Owner->PlayerBaseData->Body.Count 
		||	SrcItem.Mode.HideCount != false )
    {
//        ExchangeFieldFailed_Body( SendID , SrcItem , PosSrc , DesItem , PosDes , ClientID );
		Owner->Net_FixItemInfo_Body( PosDes );
		Owner->Net_FixItemInfo_Body( PosSrc );
		S_ApartBodyItemResult( SendID , false );
        return;
    }

    if( Item != SrcItem )
    {
//        ExchangeFieldFailed_Body( SendID , SrcItem , PosSrc , DesItem , PosDes , ClientID );
		Owner->Net_FixItemInfo_Body( PosDes );
		Owner->Net_FixItemInfo_Body( PosSrc );
		S_ApartBodyItemResult( SendID , false );
        return;
    }

	GameObjDbStructEx* SrcObjDB = Global::GetObjDB( SrcItem.OrgObjID );
	if( SrcObjDB == NULL )
	{
//		ExchangeFieldFailed_Body( SendID , SrcItem , PosSrc , DesItem , PosDes , ClientID );
		Owner->Net_FixItemInfo_Body( PosDes );
		Owner->Net_FixItemInfo_Body( PosSrc );
		S_ApartBodyItemResult( SendID , false );
		return;
	}
/*
	if( 	SrcObjDB->MaxHeap < SrcItem.Count  )
	{
		SrcItem.Count = 1;
		ExchangeFieldFailed_Body( SendID , SrcItem , PosSrc , DesItem , PosDes );
		Owner->Net_FixItemInfo_Body( PosDes );
		Owner->Net_FixItemInfo_Body( PosSrc );
		S_ApartBodyItemResult( SendID , false );
		return;
	}
*/

	if(	SrcObjDB->MaxHeap < DesItem.Count + CountDes )
	{
		CountDes = SrcObjDB->MaxHeap -  DesItem.Count;
	}

    if( CountDes > SrcItem.Count || CountDes <= 0 )
    {
//        ExchangeFieldFailed_Body( SendID , SrcItem , PosSrc , DesItem , PosDes , ClientID );
		Owner->Net_FixItemInfo_Body( PosDes );
		Owner->Net_FixItemInfo_Body( PosSrc );
		S_ApartBodyItemResult( SendID , false );
        return;
    }

    //如果搬移數量為總數
    if( CountDes == SrcItem.Count )
    {
        DesItem = SrcItem;
        SrcItem.Init();
    }
    else
    {
		if( DesItem.IsEmpty() )
		{
			DesItem = SrcItem;
			DesItem.Count = CountDes;
			SrcItem.Count -= CountDes;
		}
		else
		{
			DesItem.Count += CountDes;
			SrcItem.Count -= CountDes;
		}

		DesItem.Serial		= GenerateItemVersion( );
		DesItem.CreateTime	= SrcItem.CreateTime;//RoleDataEx::G_Now;
    }

    //資料更新
    FixItemInfo_Body( SendID , DesItem , PosDes );
    FixItemInfo_Body( SendID , SrcItem , PosSrc );

	S_ApartBodyItemResult( SendID , true , PosSrc , PosDes );
}
void NetSrv_ItemChild::R_RequestFixAllItemInfo       ( BaseItemObject* Sender )
{
    RoleDataEx *Owner = Sender->Role();
    if( Owner == NULL )
        return;

    FixAllItemInfo( Sender->GUID() , Owner );
}
void NetSrv_ItemChild::R_CloseBank                   ( BaseItemObject* Sender )
{
	RoleDataEx *Owner = Sender->Role();
	if( Owner == NULL )
		return;
	
	Owner->TempData.Attr.Action.OpenType  = EM_RoleOpenMenuType_None;
	Owner->TempData.ShopInfo.TargetID = -1;
}


void NetSrv_ItemChild::R_UseItem( BaseItemObject* Sender , int Pos , int Type , int TargetID , float TargetX , float TargetY ,float TargetZ , ItemFieldStruct& Item )
{
    RoleDataEx *Owner = Sender->Role();

    ItemFieldStruct* SrvItem;

    if( TargetID == -1 )
        TargetID = Sender->GUID();

    //幾查Client 端送過來的資料有沒有問題
    SrvItem = Owner->GetItem( Pos , Type );

    if(		SrvItem == NULL 
		||	*SrvItem != Item 	)
    {
        UseItemFailed( Owner->GUID() , Pos , Type );
		return;
	}	

	//租用資訊檢查
	if( Owner->CheckItemPosTimeFlag( Type , Pos ) == false )
		return;
 	
	GameObjDbStructEx* ItemDB = Global::GetObjDB( Item.OrgObjID );	
	if( ItemDB == NULL )
		return;

	if(		ItemDB->LiveTimeType == EM_ItemLiveTimeType_RealTime 
		&&	int( RoleDataEx::G_Now - SrvItem->CreateTime ) > ItemDB->LiveTime )
	{
		Owner->Log_ItemDestroy( EM_ActionType_TimeUp , *SrvItem ,-1 , "" );
		Owner->Net_ItemTimeout( *SrvItem , EM_ItemTimeoutPos_Body );
		SrvItem->Init();
		Owner->Net_FixItemInfo( Pos , Type );
		return;
	}

	if( ItemDB->IsRecipe() )
	{
		if( Global::UseItem(  Owner->GUID()  , TargetID , TargetX , TargetY , TargetZ, Pos , Type , *SrvItem ) == false )
		{
			UseItemFailed( Owner->GUID() , Pos , Type );
		}
		else
			UseItemOK( Owner->GUID() , Pos , Type );

		return;
	}

	if( ItemDB->IsCard() )
	{
		bool Ret = false;

		Ret = Owner->AddCard( ItemDB->GUID );

		if( Ret != false )
			UseItemOK( Owner->GUID() , Pos , Type );
		else
			UseItemFailed( Owner->GUID() , Pos , Type );

		if( Ret != false )
		{
			Owner->Log_ItemDestroy(  EM_ActionType_Card_Destroy , *SrvItem , 1 , "" );

			if( SrvItem->Count <= 1 )
			{
				SrvItem->Init();
			}
			else
			{		
				SrvItem->Count --;
			}
		}
		SrvItem->Pos = EM_ItemState_NONE;

		//資料修正
		Owner->Net_FixItemInfo( Pos , Type );
		return;
	}

	if( ItemDB->IsItem() == false )
	{
		UseItemFailed( Owner->GUID() , Pos , Type );
		return;
	}

	if( ItemDB == NULL || 	Owner->CheckLimit( ItemDB->Limit ) == false )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_LimitErr );
		UseItemFailed( Owner->GUID() , Pos , Type );
        return;
	}
	if( ItemDB->Item.CastingTime > 0 )
	{
		if(		Owner->PlayerTempData->CastingType != EM_CastingType_OKRESULT 
			||	Owner->PlayerTempData->CastingItemID != ItemDB->GUID )
		{
			UseItemFailed( Owner->GUID() , Pos , Type );
			return;
		}
	}
	else
	{
		if(		Owner->PlayerTempData->CastingType != EM_CastingType_NULL 
			&&	Owner->PlayerTempData->CastingType != EM_CastingType_OKRESULT )
		{
			UseItemFailed( Owner->GUID() , Pos , Type );
			return;
		}
	}	

	Owner->PlayerTempData->CastingType = EM_CastingType_NULL;

	if( ItemDB->IsWeapon() || ItemDB->IsArmor() )
	{
		if( Type != 2 )
		{
			UseItemFailed( Owner->GUID() , Pos , Type );
		}
	}



	//如果為金錢　
	if( ItemDB->Item.ItemType == EM_ItemType_Money )
	{
		bool Ret = false;

		if( Ret != false )
			UseItemOK( Owner->GUID() , Pos , Type );
		else
			UseItemFailed( Owner->GUID() , Pos , Type );

		if( Ret != false )
		{
			Owner->Log_ItemDestroy(  EM_ActionType_UseItemDestroy , *SrvItem , 1 , "" );
			if( SrvItem->Count <= 1 )
				SrvItem->Init();
			else
				SrvItem->Count --;
		}
		SrvItem->Pos = EM_ItemState_NONE;

		//資料修正
		Owner->Net_FixItemInfo( Pos , Type );

		return;
	}

	if( Owner->TempData.Magic.UseItem != 0 )
	{
		Owner->Net_FixItemInfo( Pos , Type );
		UseItemFailed( Owner->GUID() , Pos , Type );
		return;
	}


	vector<MyVMValueStruct> RetList;
	if( strlen( ItemDB->Item.CheckUseScript ) != 0 )
	{
		bool Result = true;
		Owner->TempData.Magic.UseItem		= SrvItem->OrgObjID;
		Owner->TempData.Magic.UseItemPos	= Pos;
		Owner->TempData.Magic.UseItemType	= Type;

		if( LUA_VMClass::PCallByStrArg( ItemDB->Item.CheckUseScript , Owner->GUID() , TargetID , &RetList , 1 ) )
		{
			if( RetList.size() != 0 )
			{
				MyVMValueStruct& Temp = RetList[0];
				if( Temp.Flag != true )
				{
					Result = false;
				}
			}
	
		}
		else
		{
			Result = false;
		}

		Owner->ClsUseItem();

		if( Result == false )
		{
			UseItemFailed( Owner->GUID() , Pos , Type );
			Owner->Net_FixItemInfo( Pos , Type );
			return;
		}
	}
	if( ItemDB->Mode.UseWithoutFail )
		Owner->TempData.Magic.IsCheckUseItem = false;
	else
		Owner->TempData.Magic.IsCheckUseItem = true;

	int	UseResultType = 0;	

	const int UseResultOK = 1;
	const int UseResultFailed = 2;

	switch( ItemDB->Item.UseType )
	{
	case EM_ItemUseType_None:
		{
			UseItemFailed( Owner->GUID() , Pos , Type );
		}		
		break;
	case EM_ItemUseType_Magic:
		{
			if( Global::UseItem(  Owner->GUID()  , TargetID , TargetX , TargetY , TargetZ, Pos , Type , *SrvItem ) == false )
			{
				UseItemFailed( Owner->GUID() , Pos , Type );
			}
			else
			{
				//if( ItemDB->Item.ItemUseOKScript[0] != 0 )
				//	LUA_VMClass::CallLuaFunc( ItemDB->Item.ItemUseOKScript , TargetID );
				if( Type == 2 )
					SC_Range_UseEq( Owner , ItemDB->GUID );
				UseItemOK( Owner->GUID() , Pos , Type );
			}
		}
		return;
	case EM_ItemUseType_SrvScript:
		{
			Owner->TempData.Magic.UseItem		= SrvItem->OrgObjID;
			Owner->TempData.Magic.UseItemPos	= Pos;
			Owner->TempData.Magic.UseItemType	= Type;

			SrvItem->Pos = EM_USE_ITEM;

			if( LUA_VMClass::PCallByStrArg( ItemDB->Item.SrvScript , Owner->GUID() , TargetID , &RetList , 1 ) )
			{
				if( RetList.size() != 0 )
				{
					MyVMValueStruct& Temp = RetList[0];
					if( Temp.Flag != true )
					{
						UseResultType = UseResultFailed;
						Owner->ClsUseItem();
						break;
					}
				}
				UseResultType = UseResultOK;
			}
			else
			{
				UseResultType = UseResultFailed;

			}

			Owner->ClsUseItem();
		}
		break;
	case EM_ItemUseType_CliScript:
		{
			UseResultType = UseResultFailed;
		}		
		break;

	case EM_ItemUseType_ResetAbility:
		{
			if(		Owner->TempData.Attr.Ability->STR == 0 
				&&	Owner->TempData.Attr.Ability->STA == 0 
				&&	Owner->TempData.Attr.Ability->INT == 0 
				&&	Owner->TempData.Attr.Ability->MND == 0 
				&&	Owner->TempData.Attr.Ability->AGI == 0 	)
			{

				Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_ResetAbilityFailed );
				UseResultType = UseResultFailed;
			}
			else
			{
				Owner->TempData.Attr.Ability->ResetPoint();
				Owner->Net_FixRoleValue( EM_RoleValue_STR , 0 );
				Owner->Net_FixRoleValue( EM_RoleValue_STA , 0 );
				Owner->Net_FixRoleValue( EM_RoleValue_INT , 0 );
				Owner->Net_FixRoleValue( EM_RoleValue_MND , 0 );
				Owner->Net_FixRoleValue( EM_RoleValue_AGI , 0 );
				Owner->Net_FixRoleValue( EM_RoleValue_Point , float( Owner->TempData.Attr.Ability->Point ) );
				Owner->TempData.UpdateInfo.Recalculate_All = true;
				UseResultType = UseResultOK;
			}
			
		}
		break;
	case EM_ItemUseType_ResetSkillPoint:
		{
			if( Owner->TempData.Attr.Ability->TpExp == Owner->TempData.Attr.Ability->TotalTpExp )
			{
				Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_ResetSkillPointFailed );
				UseResultType = UseResultFailed;
				break;
			}
			Owner->TempData.Attr.Ability->TpExp = Owner->TempData.Attr.Ability->TotalTpExp;
			Owner->Net_FixRoleValue( EM_RoleValue_TpExp , float( Owner->TempData.Attr.Ability->TpExp ) );
			Owner->Net_FixRoleValue( EM_RoleValue_TotalTpExp , float( Owner->TempData.Attr.Ability->TotalTpExp ) );


			for( int i = 0 ; i < _MAX_SPSkill_COUNT_ + _MAX_NormalSkill_COUNT_ ; i++ )
			{
				Owner->SetValue( RoleValueName_ENUM( EM_RoleValue_NormalMagic + i ) , 0 , NULL );
			}
			Owner->TempData.UpdateInfo.ReSetTalbe = true;
			UseResultType = UseResultOK;
		}
		break;
	case EM_ItemUseType_UnPacket:
		{			
			if( Owner->PlayerBaseData->ItemTemp.Size() != 0  )
			{
				UseResultType = UseResultFailed;
				break;
			}
			if( Owner->OldUnPacket( SrvItem ) == false )
			{
				if( Owner->NewUnPacket( SrvItem ) == false )
				{
					UseResultType = UseResultFailed;
					break;
				}
			}

			UseResultType = UseResultOK;
			
		}
		break;
	case EM_ItemUseType_UnPacket_DropList:
		{
			if( Owner->PlayerBaseData->ItemTemp.Size() != 0  )
			{
				UseResultType = UseResultFailed;
				break;
			}

			if( Owner->UnPacket_DropList( SrvItem ) == false )
			{
				UseResultType = UseResultFailed;
				break;
			}
			UseResultType = UseResultOK;
		}
		break;
	case EM_ItemUseType_Egg:
		{
			if( Owner->PlayerBaseData->ItemTemp.Size() != 0  )
			{
				UseResultType = UseResultFailed;
				break;
			}

			if( Owner->NewUnPacket( SrvItem ) == false )
			{
				UseResultType = UseResultFailed;
				break;
			}
			UseResultType = UseResultOK;
		}
		break;
	}

	switch( UseResultType )
	{
	case UseResultOK:
		{
			Owner->PlayerTempData->ChangeZoneCountDown = 15;

			if( ItemDB->Mode.Expense != false )
			{

				Owner->Log_ItemDestroy(  EM_ActionType_UseItemDestroy , *SrvItem , 1 , "" );		
				//清除欄位資料
				if( SrvItem->Count <= 1 )
					SrvItem->Init();
				else
					SrvItem->Count --;
			}

			if( ItemDB->Item.ItemUseOKScript[0] != 0 )
				Owner->CallLuaFunc( TargetID , ItemDB->Item.ItemUseOKScript );

			UseItemOK( Owner->GUID() , Pos , Type );

			if( Type == 2 )
				SC_Range_UseEq( Owner , ItemDB->GUID );
		}		
		break;
	case UseResultFailed:
		{
			SrvItem->Pos = EM_ItemState_NONE;
			UseItemFailed( Owner->GUID() , Pos , Type );
		}		
		break;
	}

	Owner->Net_FixItemInfo( Pos , Type );
}

void	NetSrv_ItemChild::SendRange_UsePowerLight( int PlayerGUID , int PowerLightGUID )
{
	RoleDataEx* Owner = GetRoleDataByGUID( PlayerGUID );
	if( Owner == NULL )
		return;

	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		RoleDataEx* Other = Global::GetRangePlayer();
		if( Other == NULL || !Other->IsPlayer() )
			break;
		UsePowerLight( Other->GUID() , PlayerGUID , PowerLightGUID );
	}

}


void	NetSrv_ItemChild::R_ItemDissolutionRequest		( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos )
{
	
	RoleDataEx* Owner = Sender->Role();
	ItemFieldStruct*  BodyItem = Owner->GetBodyItem(Pos);
	bool			IsEmpty = true;

	if( Item.IsEmpty() )
		return;

	if( BodyItem->Mode.ItemLock != false )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_ItemLock );
		Owner->Net_FixItemInfo_Body( Pos );
		S_ItemDissolutionResult( Sender->GUID() , Item , EM_ItemDissolutionResult_Failed );		
		return;
	}

	if(		*BodyItem != Item 
		||	Owner->PlayerTempData->CastingType != EM_CastingType_OKRESULT
		||	RoleDataEx::CheckDissolution( NULL , &Item ) != EM_CheckDissolutionResult_OK )
	{
		Owner->Net_FixItemInfo_Body( Pos );
		 S_ItemDissolutionResult( Sender->GUID() , Item , EM_ItemDissolutionResult_Failed );
		 return;
	}

	GameObjDbStructEx* OrgObjDB = Global::GetObjDB( BodyItem->OrgObjID );

	int LimitLv = OrgObjDB->Limit.Level;
/*	int LimitLv = OrgObjDB->Limit.Level + (Item.PowerQuality - 10)*10 +  RoleDataEx::RareAddLv( OrgObjDB->Rare );
	if( LimitLv <= 0 )
		LimitLv = 1;
	else if( LimitLv >= _MAX_LEVEL_ )
		LimitLv = 200;
		*/
	//////////////////////////////////////////////////////////////////////////

	int DropPoint = 0;
//	int DropRate = 0;

	if( OrgObjDB->IsWeapon() )
	{

		if(		OrgObjDB->Item.WeaponPos == EM_WeaponPos_TwoHand 
			||	OrgObjDB->Item.WeaponPos == EM_WeaponPos_Bow  )
			//DropPoint = Global::SysKeyValue()->Dissolution.WeightWeapon_TwoHand * LimitLv / 100 ;
			//DropRate = Global::SysKeyValue()->Dissolution.WeightWeapon_TwoHand;
			LimitLv += Global::SysKeyValue()->Dissolution.DLvWeapon_TwoHand;
		else if( OrgObjDB->Item.WeaponPos != EM_WeaponPos_Ammo )
			//DropPoint = Global::SysKeyValue()->Dissolution.WeightWeapon_OneHand * LimitLv / 100 ;
			//DropRate = Global::SysKeyValue()->Dissolution.WeightWeapon_OneHand;
			LimitLv += Global::SysKeyValue()->Dissolution.DLvWeapon_OneHand;

	}
	else if( OrgObjDB->IsArmor() )
	{
		switch( OrgObjDB->Item.ArmorPos )
		{
		case EM_ArmorPos_Clothes	:   //上衣
			//DropPoint = Global::SysKeyValue()->Dissolution.WeightClothes * LimitLv / 100 ;
			//DropRate = Global::SysKeyValue()->Dissolution.WeightClothes;
			LimitLv += Global::SysKeyValue()->Dissolution.DLvClothes;
			break;
		case EM_ArmorPos_Pants	    :   //褲子
			//DropPoint = Global::SysKeyValue()->Dissolution.WeightPants * LimitLv / 100 ;
			//DropRate = Global::SysKeyValue()->Dissolution.WeightPants;
			LimitLv += Global::SysKeyValue()->Dissolution.DLvPants;
			break;
		case EM_ArmorPos_Head       :   //頭
		case EM_ArmorPos_Gloves	    :   //手套
		case EM_ArmorPos_Shoes	    :   //鞋子
		case EM_ArmorPos_Back	    :   //背部
		case EM_ArmorPos_Belt	    :   //腰帶
		case EM_ArmorPos_Shoulder   :   //肩甲
		case EM_ArmorPos_SecondHand :   //副手
		case EM_ArmorPos_Necklace   :   //項鍊 
		case EM_ArmorPos_Ring       :   //戒子
		case EM_ArmorPos_Earring    :   //耳飾
			//DropPoint = Global::SysKeyValue()->Dissolution.WeightOther * LimitLv / 100 ;
			//DropRate = Global::SysKeyValue()->Dissolution.WeightOther;
			LimitLv += Global::SysKeyValue()->Dissolution.DLvOther;
			break;
		}
	}

	/*
	if( DropPoint == 0 )
	{
		S_ItemDissolutionResult( Sender->GUID() , Item , EM_ItemDissolutionResult_Failed );
		return;
	}*/

	DropPoint = (LimitLv-1) / 10 ;
	if( DropPoint >= 20 )
		DropPoint = 19;

//	DropRate = DropRate * ( 100 - ( 19 - LimitLv%20) * 3)/100;
	GameObjDbStructEx* TreasureDB = NULL;
	//取得掉落的寶箱資料
	if( OrgObjDB->IsArmor() == false )
		TreasureDB = Global::GetObjDB( Global::SysKeyValue()->Dissolution.ItemWeapon[OrgObjDB->Rare][DropPoint] );
	else if(OrgObjDB->IsWeapon() == false ) 
		TreasureDB = Global::GetObjDB( Global::SysKeyValue()->Dissolution.ItemArmor[OrgObjDB->Rare][DropPoint] );

	if( TreasureDB == NULL )
	{
		Owner->Net_FixItemInfo_Body( Pos );
		S_ItemDissolutionResult( Sender->GUID() , Item , EM_ItemDissolutionResult_Failed );
		return;
	}
	Owner->DelBodyItem_ByPos( Pos , 1 ,EM_ActionType_Dissolution);

/*	if( rand() % 100 > DropRate )
	{
		S_ItemDissolutionResult( Sender->GUID() , Item , EM_ItemDissolutionResult_OK_Empty );
		return;
	}*/
/*
	set<int>	ItemAbilitySet;
	for( int i = 0 ; i < 6 ; i++ )
	{
		GameObjDbStructEx* AbilityDB = Global::GetObjDB( Item.Inherent(i) );
		if( AbilityDB == NULL )
			continue;
		for( int j = 0 ; j < 5 ; j++ )
		{
			if( AbilityDB->Attirbute.Ability.EqType[j] == EM_WearEqType_None )
				continue;
			ItemAbilitySet.insert( AbilityDB->Attirbute.Ability.EqType[j] );
		}
	}

	for( int i = 0 ; i < 4 ; i++ )
	{
		GameObjDbStructEx* RuneDB = Global::GetObjDB( Item.Rune(i) );
		if( RuneDB == NULL )
			continue;
		for( int j = 0 ; j < 5 ; j++ )
		{
			if( RuneDB->Item.Ability.EqType[j] == EM_WearEqType_None )
				continue;
			ItemAbilitySet.insert( RuneDB->Item.Ability.EqType[j] );
		}
	}
*/	
	int TObjID , TCount;
//	int	RuneID = 0;
//	bool IsFind = false;
	/*
	for( int i = 0 ; i < 10 && IsFind == false ; i++ )
	{
		if( RoleDataEx::GetTreasure( TreasureDB->GUID , TObjID  , TCount ) == false )
			continue;

		GameObjDbStructEx* RuneDB = Global::GetObjDB( TObjID );
		if( RuneDB->IsRune() == false )
			continue;

		RuneID = TObjID;

		if( ItemAbilitySet.size() == 0 )
			break;

		for( int j = 0 ; j < 5 ; j++ )
		{
			if( RuneDB->Item.Ability.EqType[j] == EM_WearEqType_None )
				continue;
			if( ItemAbilitySet.find( RuneDB->Item.Ability.EqType[j] ) == ItemAbilitySet.end() )
				continue;

			IsFind = true;
			break;
		}
	}
	*/
	Owner->GetTreasure( TreasureDB->GUID , TObjID  , TCount );

	bool Result = Owner->GiveItem( TObjID , TCount , EM_ActionType_Dissolution , NULL , "" );
	if( Result )
		S_ItemDissolutionResult( Sender->GUID() , Item , EM_ItemDissolutionResult_OK );
	else
		S_ItemDissolutionResult( Sender->GUID() , Item , EM_ItemDissolutionResult_OK_Empty );

}

void NetSrv_ItemChild::R_TakeOutCardRequest			( BaseItemObject* Sender , int CardID )
{
	RoleDataEx* Owner = Sender->Role();

	bool Ret = Owner->DelCard( CardID );
	if( Ret != false )
	{
		Owner->GiveItem( CardID , 1 , EM_ActionType_Card , NULL , "" );
	}

	S_TakeOutCardResult( Sender->GUID() , CardID , Ret );
}

void NetSrv_ItemChild::R_EQCombinRequest			( BaseItemObject* Sender , int CombinItemPos[ 2 ] , int CheckItemPos )
{
	RoleDataEx* Owner = Sender->Role();

	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_EQCombin) == false )
	{
		S_EQCombinClose( Owner->GUID() );
		return;
	}

	ItemFieldStruct* Item1		= Owner->GetBodyItem( CombinItemPos[0] );
	ItemFieldStruct* Item2		= Owner->GetBodyItem( CombinItemPos[1] );
	ItemFieldStruct* CheckItem	= Owner->GetBodyItem( CheckItemPos	  );


	if( Item1 == NULL || Item2 == NULL || CheckItem == NULL )
	{
		S_EQCombinResult( Owner->GUID() , false );
		return;
	}

	GameObjDbStructEx* Item1ObjDB = Global::GetObjDB( Item1->OrgObjID );
	GameObjDbStructEx* Item2ObjDB = Global::GetObjDB( Item2->OrgObjID );

	if( Item1ObjDB == NULL || Item2ObjDB == NULL )
		return;

	if( CheckItem->OrgObjID != Owner->TempData.EQCombin.UseItemTempID )
	{
		S_EQCombinResult( Owner->GUID() , false );
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//檢查顯像資料是否可以合
	GameObjDbStructEx* Item2ImageObjDB = Global::GetObjDB( Item2->ImageObjectID );
	if( Item2ImageObjDB->IsImage() == false )
	{
		if( Item2ImageObjDB != NULL  )
		{
			Item2ImageObjDB =  Global::GetObjDB( Item2ImageObjDB->ImageID );
		}
		if( Item2ImageObjDB->IsImage() == false )
		{
			S_EQCombinResult( Owner->GUID() , false );
			return;
		}
	}

	if( Item1ObjDB->IsArmor() )
	{
		if( Item2ImageObjDB->Image.LimitSex._Sex != 0 && Item1ObjDB->Limit.Sex._Sex != 0 )
		{
			if( (Item2ImageObjDB->Image.LimitSex._Sex & Item1ObjDB->Limit.Sex._Sex) == 0 )
			{
				S_EQCombinResult( Owner->GUID() , false );
				return;
			}
		}

		if(	Item1ObjDB->Item.ArmorPos	 != Item2ObjDB->Item.ArmorPos		)
		{
			S_EQCombinResult( Owner->GUID() , false );
			return;
		}

		if( Item2ObjDB->Mode.SpecialSuit == false && Owner->TempData.EQCombin.Type == 0 )
		{
			if( Item1ObjDB->Item.ArmorType != Item2ObjDB->Item.ArmorType )
			{
				S_EQCombinResult( Owner->GUID() , false );
				return;
			}
		}

	}
	else if( Item1ObjDB->IsWeapon() )
	{
		
		switch( Item1ObjDB->Item.WeaponPos )
		{
		case EM_WeaponPos_MainHand:		//主手
		case EM_WeaponPos_SecondHand:	//副手
		case EM_WeaponPos_OneHand:		//雙手都可以裝備
			{
				if(		Item2ObjDB->Item.WeaponPos != EM_WeaponPos_MainHand 
					&&	Item2ObjDB->Item.WeaponPos != EM_WeaponPos_SecondHand 
					&&	Item2ObjDB->Item.WeaponPos != EM_WeaponPos_OneHand 		)
				{
					S_EQCombinResult( Owner->GUID() , false );
					return;
				}
			}

			break;
		case EM_WeaponPos_TwoHand:		//兩手		
			{
				if(		Item2ObjDB->Item.WeaponPos != EM_WeaponPos_TwoHand		)
				{
					S_EQCombinResult( Owner->GUID() , false );
					return;
				}
			}
			break;
		case EM_WeaponPos_Bow:			//弓
			if( Item2ObjDB->Item.WeaponPos != EM_WeaponPos_Bow )
			{
				S_EQCombinResult( Owner->GUID() , false );
				return;
			}
			break;
		default:
			S_EQCombinResult( Owner->GUID() , false );
			return;

		}

		if( Item2ObjDB->Mode.SpecialSuit == false && Owner->TempData.EQCombin.Type == 0 )
		{
			if( Item1ObjDB->Item.WeaponType	!= Item2ObjDB->Item.WeaponType )
			{
				S_EQCombinResult( Owner->GUID() , false );
				return;
			}
		}
	}
	
	Item1->ImageObjectID = Item2->ImageObjectID;
	Item1->MainColor	= Item2->MainColor;
	Item1->OffColor		= Item2->OffColor;
//	Item1->Mode.DepartmentStore |= Item2->Mode.DepartmentStore;

	Log_ItemDestroy( Owner , EM_ActionType_EQCombineDestroy , *Item2 , -1 , -1 , "" );	
	Log_ItemDestroy( Owner , EM_ActionType_EQCombineDestroy , *CheckItem , -1 , -1 , "" );	

	Item2->Init();
	if( CheckItem->Count <= 1 )
	{
		CheckItem->Init();
	}
	else
	{
		CheckItem->Count--;
	}

	Owner->Net_FixItemInfo_Body( CombinItemPos[0] );
	Owner->Net_FixItemInfo_Body( CombinItemPos[1] );
	Owner->Net_FixItemInfo_Body( CheckItemPos	  );

	Global::Log_ItemTrade( Owner->DBID() , -1 , Owner->DBID() , (int)Owner->X() , (int)Owner->Z() , EM_ActionType_EQCombineResult , *Item1 , "" );

	S_EQCombinResult( Owner->GUID() , true );
}

void NetSrv_ItemChild::R_EQCombinClose				( BaseItemObject* Sender )
{
	RoleDataEx* Owner = Sender->Role();
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_EQCombin) != false )
	{
		Owner->ClsOpenMenu();
	}
	
}

void NetSrv_ItemChild::R_UseItemToItem_Notify	( BaseItemObject* Sender , ItemFieldStruct& SendUseItem , int UseItemPos , ItemFieldStruct& SendTargetItem , int TargetItemPos , int TargetPosType , char* Password )
{
	
	char	Buf[512];
	RoleDataEx* Owner = Sender->Role();

	if( Owner->TempData.Sys.OnChangeZone != false )
		return;

	//租用資訊檢查
	if( Owner->CheckItemPosTimeFlag( 0 , UseItemPos ) == false )
		return;
	//租用資訊檢查
	if( Owner->CheckItemPosTimeFlag( TargetPosType , TargetItemPos ) == false )
		return;

	ItemFieldStruct* UseItem =  Owner->GetBodyItem( UseItemPos );
	ItemFieldStruct* TargetItem = NULL;

	switch( TargetPosType )
	{
	case 0: //身體
		TargetItem =  Owner->GetBodyItem( TargetItemPos );
		break;
	case 1: //銀行
		TargetItem =  Owner->GetBankItem( TargetItemPos );
		break;
	case 2: //裝備
		TargetItem =  Owner->GetEqItem( TargetItemPos );
		break;
	}

	if( UseItem == NULL || TargetItem == NULL )
	{
		Print( LV5 , "R_UseItemToItem_Notify" , "UseItem == NULL || TargetItem == NULL" );
		sprintf( Buf , "Item to Item Use Pos Error,  UsePos = %d TargetPos = %d" , UseItemPos , TargetItemPos );
		Global::SendMsgToGMTools( LV5 , Owner->Account_ID() , Buf );
		Sender->Destroy( Buf );
		return;
	}
	

	if( *UseItem != SendUseItem )
	{
		Print( LV2 , "R_UseItemToItem_Notify" , " *UseItem != SendUseItem " );
		UseItemtoItemResult_Failed( Owner->GUID() , *UseItem , UseItemPos , *TargetItem , TargetItemPos , TargetPosType );
		return;
	}

	GameObjDbStructEx* UseItemOrgDB = GetObjDB( UseItem->OrgObjID );
	GameObjDbStructEx* TargetItemOrgDB = GetObjDB( TargetItem->OrgObjID );
	if( UseItemOrgDB == NULL || TargetItemOrgDB == NULL )
	{
		Print( LV2 , "R_UseItemToItem_Notify" , " UseItemOrgDB == NULL || TargetItemOrgDB == NULL " );
		UseItemtoItemResult_Failed( Owner->GUID() , *UseItem , UseItemPos , *TargetItem , TargetItemPos , TargetPosType );
		return;
	}

	if(		UseItemOrgDB->Mode.EnableLockedItem == false 
		&&	TargetItem->Mode.ItemLock != false )
	{
		if( UseItemOrgDB->Item.UseType != EM_ItemUseType_ItemUnLock )
		{
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_ItemLock );
			UseItemtoItemResult_Failed( Owner->GUID() , *UseItem , UseItemPos , *TargetItem , TargetItemPos , TargetPosType );
			return;
		}
	}
/*
	if( TargetItemOrgDB->CheckInherentAbility( UseItemOrgDB->Item.ItemToItemLimit ) == false )
	{
		UseItemtoItemResult_Failed( Owner->GUID() , *UseItem , UseItemPos , *TargetItem , TargetItemPos , TargetPosType );
		return;
	}
*/

	//檢查是否要密碼
	if( UseItemOrgDB->Item.IsUseNeedPassword )
	{
		MyMD5Class Md5;
		Md5.ComputeStringHash( Password );
		if( stricmp( Md5.GetMd5Str() , Owner->PlayerBaseData->Password.Begin() ) != 0 )
		{
			UseItemtoItemResult_Failed( Owner->GUID() , *UseItem , UseItemPos , *TargetItem , TargetItemPos , TargetPosType );
			return;
		}
	}

	if( UseItemOrgDB->Item.CastingTime > 0 )
	{
		if(		Owner->PlayerTempData->CastingType != EM_CastingType_OKRESULT 
			||	Owner->PlayerTempData->CastingItemID != UseItem->OrgObjID )
		{
			UseItemtoItemResult_Failed( Owner->GUID() , *UseItem , UseItemPos , *TargetItem , TargetItemPos , TargetPosType );
			return;
		}
	}
	else
	{
		if( Owner->PlayerTempData->CastingType != EM_CastingType_NULL && Owner->PlayerTempData->CastingType != EM_CastingType_OKRESULT )
		{
			UseItemtoItemResult_Failed( Owner->GUID() , *UseItem , UseItemPos , *TargetItem , TargetItemPos , TargetPosType );
			return;
		}
	}

	vector<MyVMValueStruct> RetList;
	if( strlen( UseItemOrgDB->Item.CheckUseScript ) != 0 )
	{
		char	luaScript[512];
		bool Result = true;
		Owner->TempData.Magic.UseItem		= UseItem->OrgObjID;
		Owner->TempData.Magic.UseItemPos	= UseItemPos;
		Owner->TempData.Magic.UseItemType	= 0;
		if( UseItemOrgDB->Item.UseType == EM_ItemUseType_ItemtoItemLua )
			sprintf( luaScript , "%s( %d,%d)" , UseItemOrgDB->Item.CheckUseScript , TargetPosType , TargetItemPos );
		else
			sprintf( luaScript , "%s" , UseItemOrgDB->Item.CheckUseScript , TargetPosType , TargetItemPos );

		if( LUA_VMClass::PCallByStrArg( luaScript , Owner->GUID() , Owner->GUID() , &RetList , 1 ) )
		{
			if( RetList.size() != 0 )
			{
				MyVMValueStruct& Temp = RetList[0];
				if( Temp.Flag != true )
				{
					Result = false;
				}
			}

		}
		else
		{
			Result = false;
		}

		Owner->ClsUseItem();

		if( Result == false )
		{
			UseItemtoItemResult_Failed( Owner->GUID() , *UseItem , UseItemPos , *TargetItem , TargetItemPos , TargetPosType );
			Owner->Net_FixItemInfo( UseItemPos , 0 );
			return;
		}
	}

	Owner->PlayerTempData->CastingType = EM_CastingType_NULL;

	int	UseResultType = 0;	
	bool	IsNeedProcItem = true;
	const int UseResultOK = 1;
	const int UseResultFailed = 2;



	if( UseItemOrgDB->IsRune() )
	{
		if(	Global::AddRune( UseItem , UseItemOrgDB , TargetItem , TargetItemOrgDB ) == false )
		{
			UseResultType = UseResultFailed;

		}
		else
		{
			IsNeedProcItem = true;

			UseResultType = UseResultOK;
		}
	}
	else
	{
		switch( UseItemOrgDB->Item.UseType )
		{
		case EM_ItemUseType_None:
		case EM_ItemUseType_Magic:
		case EM_ItemUseType_SrvScript:
		case EM_ItemUseType_CliScript:
			{
				//UseItemtoItemResult_Failed( Owner->GUID() , *UseItem , UseItemPos , *TargetItem , TargetItemPos , TargetPosType );
				UseResultType = UseResultFailed;
				break;
			}

		case EM_ItemUseType_EqRefine:	//衝等
			{

				float	EqRefineRate;
				float	EqRefineFailed_LevelDownRate;
				float	EqRefineFailed_ExplodeRate;



				if(	RoleDataEx::CheckEqRefine( UseItem , TargetItem ) != EM_CheckEqRefineResult_OK )
				{
					UseResultType = UseResultFailed;
					break;
				}

				EqRefineRate = UseItemOrgDB->Item.EqRefineRate_Base;
				EqRefineFailed_LevelDownRate = UseItemOrgDB->Item.EqRefineFailed_LevelDownRate_Base;
				EqRefineFailed_ExplodeRate = UseItemOrgDB->Item.EqRefineFailed_ExplodeRate_Base;

				for( int i = UseItemOrgDB->Item.EqRefineMinLv + 1 ; i < TargetItem->Level ; i ++ )
				{
					EqRefineRate = ( EqRefineRate + UseItemOrgDB->Item.EqRefineRate_Fix ) * UseItemOrgDB->Item.EqRefineRate_Per/100;
					EqRefineFailed_LevelDownRate = ( EqRefineFailed_LevelDownRate + UseItemOrgDB->Item.EqRefineFailed_LevelDownRate_Fix ) * UseItemOrgDB->Item.EqRefineFailed_LevelDownRate_Per/100;
					EqRefineFailed_ExplodeRate = ( EqRefineFailed_ExplodeRate + UseItemOrgDB->Item.EqRefineFailed_ExplodeRate_Fix ) * UseItemOrgDB->Item.EqRefineFailed_ExplodeRate_Per/100;
				}

				EqRefineRate *= RoleDataEx::GlobalSetting.EqRefineRate;

				bool IsItemChange = true;
				 
				if( UseItemOrgDB->Mode.DepartmentStore != false )
				{
					if( TargetItem->Level == 0 )
						EqRefineRate = 100;
					if( TargetItem->Level <= 1 )
						EqRefineFailed_LevelDownRate = 0;

				}

				if( rand() % 100 < EqRefineRate )
				{
					EqRefineResult( Owner->GUID() , *TargetItem , EM_EqRefineResult_LevelUp );
					TargetItem->Level++;
					Owner->Log_ItemTrade( NULL , EM_ActionType_RefineLvUp , *TargetItem , "" );	
				}
				else
				{
					if( rand() % 100  < EqRefineFailed_LevelDownRate )
					{
						
						if( TargetItem->Level == 0 )
						{
							EqRefineResult( Owner->GUID() , *TargetItem , EM_EqRefineResult_None );
							IsItemChange = false;
							Owner->Log_ItemTrade( NULL , EM_ActionType_RefineNone , *TargetItem , "" );	
						}
						else
						{
							EqRefineResult( Owner->GUID() , *TargetItem , EM_EqRefineResult_LevelDown );
							TargetItem->Level--;
							Owner->Log_ItemTrade( Owner , EM_ActionType_RefineLvDn , *TargetItem , "" );
						}
						
					}
					else if( rand() % 100 < EqRefineFailed_ExplodeRate )
					{

						if( TargetItem->Mode.Protect != false )
						{
							EqRefineResult( Owner->GUID() , *TargetItem , EM_EqRefineResult_LevelDown );
							TargetItem->Level = 0;
							TargetItem->Mode.Protect = false;
							Owner->Log_ItemTrade( Owner , EM_ActionType_RefineProtect , *TargetItem , "" );	
						}
						else
						{
							EqRefineResult( Owner->GUID() , *TargetItem , EM_EqRefineResult_Broken );
							Log_ItemDestroy( Owner , EM_ActionType_RefineFailedDestroy , *TargetItem , -1 , -1 , "" );	
							TargetItem->Init();						
						}
					}
					else
					{
						EqRefineResult( Owner->GUID() , *TargetItem , EM_EqRefineResult_None );
						IsItemChange = false;
						Owner->Log_ItemTrade( Owner , EM_ActionType_RefineNone , *TargetItem , "" );	
					}		

				}
				if( IsItemChange != false && TargetPosType == 2 )
				{
					Owner->TempData.UpdateInfo.Eq = true;
				}
				UseResultType = UseResultOK;
			}
			break;
		case EM_ItemUseType_FixEq:		//修裝
			{
				if(	RoleDataEx::CheckFixEq( UseItem , TargetItem ) != EM_CheckFixEqResult_OK		)
				{
					//UseItemtoItemResult_Failed( Owner->GUID() , *UseItem , UseItemPos , *TargetItem , TargetItemPos , TargetPosType );
					//return;
					UseResultType = UseResultFailed;
					break;
				}

				//int	LastMaxDurable = TargetItemOrgDB->Item.Durable * TargetItem->Quality;

				/*int	QualityDown = (( LastMaxDurable - TargetItem->Durable ) / 100) * TargetItem->Quality * ( 100 - UseItemOrgDB->Item.FixQuality ) / LastMaxDurable; 
				if( QualityDown < 0 )
					QualityDown = 0;

				TargetItem->Quality-= QualityDown;
				*/
				int LastMaxDurable = TargetItemOrgDB->Item.Durable * UseItemOrgDB->Item.FixQuality * TargetItem->Quality / 100;
				TargetItem->Durable = LastMaxDurable;
				Owner->TempData.UpdateInfo.Recalculate_All = true;

				UseResultType = UseResultOK;
			}
			break;
		case EM_ItemUseType_Dissolution:
			{

			}
			break;
		case EM_ItemUseType_Coloring:
			{
				if( RoleDataEx::CheckColoring( UseItem , TargetItem ) != EM_CheckColoringResult_OK )
				{
					UseResultType = UseResultFailed;
					break;
				}

				bool IsUse = false;

				if(		UseItemOrgDB->Item.Coloring_OffColor == 0 
					&&	UseItemOrgDB->Item.Coloring_MainColor == 0 )
				{
					if( TargetItemOrgDB->Item.ItemType == EM_ItemType_SummonHorse )
					{
						IsUse = true;
						TargetItem->HorseColor[0] = 0;
						TargetItem->HorseColor[1] = 0;
						TargetItem->HorseColor[2] = 0;
						TargetItem->HorseColor[3] = 0;
					}
					else
					{
						if(		TargetItem->OffColor != 0
							||	TargetItem->MainColor != 0 	)
						{
							IsUse = true;
							TargetItem->OffColor = 0;
							TargetItem->MainColor = 0;
						}
					}
				}
				else if( TargetItemOrgDB->Mode.Coloring != false )
				{
					if( UseItemOrgDB->Item.Coloring_OffColor != 0 )
					{
						if( TargetItem->OffColor != UseItemOrgDB->Item.Coloring_OffColor )
						{
							IsUse = true;
							TargetItem->OffColor = UseItemOrgDB->Item.Coloring_OffColor ;
						}

					}

					if( UseItemOrgDB->Item.Coloring_MainColor != 0 )
					{
						if( TargetItem->MainColor != UseItemOrgDB->Item.Coloring_MainColor )
						{
							IsUse = true;
							TargetItem->MainColor = UseItemOrgDB->Item.Coloring_MainColor ;
						}

					}

				}
				if( IsUse == false )
				{
					Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_ColoringFailed );
					UseResultType = UseResultFailed;
					break;
				}

				UseResultType = UseResultOK;
				break;
			}
		case EM_ItemUseType_ClsTrade:
			{
				if(		TargetPosType != 0 
					||	RoleDataEx::CheckClsTrade( UseItem , TargetItem ) != EM_CheckClsSoulBoundResult_OK  )
				{
					UseResultType = UseResultFailed;
					break;
				}

				bool IsUse = false;
				TargetItem->Mode.Trade = true;
				TargetItem->Mode.EQSoulBound = true;
				UseResultType = UseResultOK;
				break;
			}
			break;
		case EM_ItemUseType_ClsEqSoulbound:
			{
				if(		TargetPosType != 0 
					||	RoleDataEx::CheckClsSoulBound( UseItem , TargetItem ) != EM_CheckClsSoulBoundResult_OK  )
				{
					UseResultType = UseResultFailed;
					break;
				}

				bool IsUse = false;
				TargetItem->Mode.Trade = true;
				TargetItem->Mode.EQSoulBound = true;
				UseResultType = UseResultOK;
				break;
			}
			break;
		case EM_ItemUseType_EQDigHole:	//重設技能點數
			{
				if( RoleDataEx::CheckDigHole( UseItem , TargetItem ) != EM_CheckDigHoleResult_OK )
				{
					UseResultType = UseResultFailed;
					break;
				}
				if( rand() % 100 < UseItemOrgDB->Item.DigHole.Rate )
				{
					Owner->Msg(" 挖洞");
					TargetItem->RuneVolume++;
					S_DigHoleResult( Owner->GUID() , true );
				}
				else
				{
					Owner->Msg(" 挖洞失敗");
					S_DigHoleResult( Owner->GUID() , false );
				}
				UseResultType = UseResultOK;
			}
			break;
		case EM_ItemUseType_EQClearRune:		//清除裝備符文
			{
				if( RoleDataEx::CheckClsRune( UseItem , TargetItem ) != EM_CheckClsRuneResult_OK )
				{
					UseResultType = UseResultFailed;
					break;
				}

				for( int i = 0 ; i < 4 ; i++ )
				{
					GameObjDbStructEx*	OrgDB = Global::GetObjDB( TargetItem->Rune(i) );
					if( OrgDB->IsRune() )
					{
						TargetItem->RuneVolume += OrgDB->Item.Volume;
					}

					TargetItem->Rune(i , 0);
				}

				UseResultType = UseResultOK;
			}
			break;
		case EM_ItemUseType_ItemLock:	
			{
				if( RoleDataEx::CheckItemLock( UseItem , TargetItem ) != EM_CheckItemLockResult_OK )
				{
					UseResultType = UseResultFailed;
					break;
				}
				TargetItem->Mode.ItemLock = true;
				UseResultType = UseResultOK;
			}
			break;
		case EM_ItemUseType_ItemUnLock:				
			{
				if( RoleDataEx::CheckItemUnLock( UseItem , TargetItem ) != EM_CheckItemUnLockResult_OK )
				{
					UseResultType = UseResultFailed;
					break;
				}
				TargetItem->Mode.ItemLock = false;
				UseResultType = UseResultOK;
			}
			break;
		case EM_ItemUseType_EQProtect:	
			{
				if( RoleDataEx::CheckEQProtect( UseItem , TargetItem ) != EM_CheckEQProtectResult_OK )
				{
					UseResultType = UseResultFailed;
					break;
				}
				TargetItem->Mode.Protect = true;
				UseResultType = UseResultOK;
			}
			break;

		case EM_ItemUseType_EQLimitMax:
			{
				if( RoleDataEx::CheckEQLimitMax( UseItem , TargetItem ) != EM_CheckEQLimitMaxResult_OK )
				{
					UseResultType = UseResultFailed;
					break;
				}
				TargetItem->Quality  = TargetItem->OrgQuality;
				UseResultType = UseResultOK;
			}
			break;

		case EM_ItemUseType_IgnoreLimitLv:
			{
				if( RoleDataEx::CheckIgnoreLimitLv( UseItem , TargetItem ) != EM_CheckIgnoreLimitLvMaxResult_OK )
				{
					UseResultType = UseResultFailed;
					break;
				}
				TargetItem->Mode.IgnoreLimitLv = true;
				UseResultType = UseResultOK;
			}
			break;
		case EM_ItemUseType_PK_EQProtect:	//PK裝備保護
			{
				//查身上裝備是否太多有保護
				if( TargetPosType == 2 )
				{
					int Count = 0;
					for( int i = 0 ; i < EM_EQWearPos_MaxCount ; i++ )
					{
						if( Owner->BaseData.EQ.Item[i].Mode.PkProtect != false ) 
							Count++;
					}
					if( Count >= 8 )
					{
						Owner->Net_GameMsgEvent( EM_GameMessageEvent_EQError_PKProtectCountError );
						UseResultType = UseResultFailed;
						break;
					}
				}


				if( RoleDataEx::CheckPkEqProtect( UseItem , TargetItem ) != EM_CheckPkEqProtectResult_OK )
				{
					UseResultType = UseResultFailed;
					break;
				}
				TargetItem->Mode.PkProtect = true;
				UseResultType = UseResultOK;
			}
			break;
		case EM_ItemUseType_PK_EQProtect_Clear:	//PK裝備保護清除
			{
				if( RoleDataEx::CheckPkEqProtectClear( UseItem , TargetItem ) != EM_CheckPkEqProtectClearResult_OK )
				{
					UseResultType = UseResultFailed;
					break;
				}
				TargetItem->Mode.PkProtect = false;
				UseResultType = UseResultOK;
			}
			break;
		case EM_ItemUseType_ClsEqAbility:
			{
				if( RoleDataEx::CheckClsEqAbility( UseItem , TargetItem ) != EM_CheckClsEqAbilityResult_OK )
				{
					UseResultType = UseResultFailed;
					break;
				}
				memset( TargetItem->_Inherent , 0 , sizeof(TargetItem->_Inherent) );
				UseResultType = UseResultOK;

			}
			break;
		case EM_ItemUseType_ItemtoItemLua:
			{
/*				char	luaScript[512];
				vector< MyVMValueStruct > RetList;
				sprintf( luaScript , "%s( %d,%d)" , UseItemOrgDB->Item.ItemUseOKScript , TargetPosType , TargetItemPos );
				if(		LUA_VMClass::PCallByStrArg( luaScript , Owner->GUID() , Owner->GUID() , &RetList , 1 ) == false 
					||	( RetList.size() == 1 && RetList[0].Flag == false ) )
				{
					UseResultType = UseResultFailed;
					return;
				}
				*/
				UseResultType = UseResultOK;
			}
			break;
		}
	}

	switch( UseResultType )
	{
	case UseResultOK:
		{
			UseItemtoItemResult_OK( Owner->GUID() );
			if( IsNeedProcItem != false )
			{
				if( UseItemOrgDB->Mode.Expense != false )
				{
					Log_ItemDestroy( Owner , EM_ActionType_UseItemDestroy , *UseItem , 1 , -1 , "" );
					if( UseItem->Count <= 1 )
						UseItem->Init();
					else
						UseItem->Count --;
				}
			}
			if( UseItemOrgDB->Item.ItemUseOKScript[0] != 0 )
				Owner->CallLuaFunc( Owner->GUID() , UseItemOrgDB->Item.ItemUseOKScript );

			Global::Log_ItemTrade( Owner->DBID() , -1 , Owner->DBID() ,(int)Owner->X() , (int)Owner->Z() , EM_ActionType_ItemtoItemResult , *TargetItem , "" );
		}
		break;
	case UseResultFailed:
		{
			UseItemtoItemResult_Failed( Owner->GUID() , *UseItem , UseItemPos , *TargetItem , TargetItemPos , TargetPosType );
		}
		break;
	}

	Owner->Net_FixItemInfo_Body( UseItemPos );
	switch( TargetPosType )
	{
	case 0://身體
		Owner->Net_FixItemInfo_Body( TargetItemPos );
		break;
	case 1://銀行
		Owner->Net_FixItemInfo_Bank( TargetItemPos );
		break;
	case 2://裝備
		Owner->Net_FixItemInfo_EQ( TargetItemPos );
		Owner->TempData.UpdateInfo.Eq = true;
		break;
	}

}

void NetSrv_ItemChild::RD_AccountBagInfoResult( int PlayerDBID , int MaxCount , int ItemDBID[20] , ItemFieldStruct Item[20] )
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObjByDBID( PlayerDBID );
	if( OwnerObj == NULL )
		return;

	if( OwnerObj->Role()->CheckOpenMenu( EM_RoleOpenMenuType_AccountBag) == false 	 )
		return;

	map< int , ItemFieldStruct>&	AccountBag = *(OwnerObj->AccountBag());
	AccountBag.clear();

	for( int i = 0 ; i < 20 && i < MaxCount ; i++ )
	{
		AccountBag[ ItemDBID[i] ] = Item[i];
	}

	SC_OpenAccountBag( OwnerObj->GUID() , OwnerObj->Role()->TempData.ShopInfo.TargetID , MaxCount , ItemDBID , Item );
}
void NetSrv_ItemChild::RC_AccountBagGetItemRequest( BaseItemObject* Sender , int ItemDBID , int BodyPos )
{

	RoleDataEx* Owner = Sender->Role();
	map< int , ItemFieldStruct>&	AccountBag = *(Sender->AccountBag());

	if( Owner->TempData.Sys.OnChangeZone != false )
		return;

	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_AccountBag) == false 	 )
		return;

	ItemFieldStruct* BodyItem = Owner-> GetBodyItem( BodyPos );
	if( BodyItem == NULL )
	{
		SC_AccountBagGetItemResult( Owner->GUID() , BodyPos , ItemDBID , EM_AccountBagGetItemResult_BodyPosErr ); 
		return;
	}
	map< int , ItemFieldStruct>::iterator Iter;
	Iter = AccountBag.find( ItemDBID );

	if( Iter == AccountBag.end() )
	{
		SC_AccountBagGetItemResult( Owner->GUID() , BodyPos , ItemDBID , EM_AccountBagGetItemResult_ItemDBIDErr ); 
		return;
	}
	
	GameObjDbStructEx*	OrgDB = Global::GetObjDB( Iter->second.OrgObjID );
	if( OrgDB == NULL )
	{
		SC_AccountBagGetItemResult( Owner->GUID() , BodyPos , ItemDBID , EM_AccountBagGetItemResult_ObjDBErr ); 		
		return;
	}

	if( BodyItem->IsEmpty() )
	{
		*BodyItem = Iter->second;
		if( BodyItem->Serial == 0 )
		{
			BodyItem->CreateTime = RoleDataEx::G_Now;
			BodyItem->Serial 	= Global::GenerateItemVersion( );
		}

	}
	else
	{
		if(		BodyItem->OrgObjID != Iter->second.OrgObjID 
			||	OrgDB->MaxHeap < BodyItem->Count + Iter->second.Count 
			||	BodyItem->Mode.HideCount != false
			||	Iter->second.Mode.HideCount != false )
		{
			SC_AccountBagGetItemResult( Owner->GUID() , BodyPos , ItemDBID , EM_AccountBagGetItemResult_CountErr );
			return;
		}
		BodyItem->Count += Iter->second.Count;
	}

	char Buf[512];
	AccountBag.erase( Iter );
	sprintf( Buf , "DELETE AccountBag WHERE GUID =%d" , ItemDBID );		
	Net_DCBase::SqlCommand_Import( Owner->DBID() , Buf );

	SC_AccountBagGetItemResult( Owner->GUID() , BodyPos , ItemDBID , EM_AccountBagGetItemResult_OK );
	Owner->Net_FixItemInfo_Body( BodyPos );

}
void NetSrv_ItemChild::RC_AccountBagClose( BaseItemObject* Sender )
{
	RoleDataEx* Owner = Sender->Role();

	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_AccountBag) == false 	 )
		return;

	Owner->ClsOpenMenu();
}


void NetSrv_ItemChild::RC_MagicBoxRequest			( BaseItemObject* Obj )
{
	RoleDataEx* Owner = Obj->Role();
	ItemFieldStruct NewItem;
	NewItem.Init();

	if(		Owner->CheckKeyItem( g_ObjectData->SysValue().MagicBoxOpenKeyItemID ) == false 
//		||	Owner->PlayerBaseData->BoxEnergy <= 0
		||	Owner->MagicBoxSmelt( &NewItem ) == false )
	{
		SC_MagicBoxResult( Owner->GUID() , &NewItem , false );
		return;
	}

	if( Owner->PlayerBaseData->BoxEnergy < NewItem.Count )
	{
		SC_MagicBoxResult( Owner->GUID() , &NewItem , false );
		return;
	}
	Owner->PlayerBaseData->BoxEnergy -= NewItem.Count;
	Owner->Net_FixRoleValue( EM_RoleValue_BoxEnergy , Owner->PlayerBaseData->BoxEnergy );
	//清除所有寶箱物品
	for( int i = 0 ; i < _MAX_BODY_MAGIC_BOX_COUNT_ ; i++ )
	{
		ItemFieldStruct& Item = Owner->PlayerBaseData->Body.ItemMagicBox[i];
		if( Item.IsEmpty() )
			continue;
		Owner->Log_ItemDestroy( EM_ActionType_MagicBox_Destroy , Item , -1 , "" );

		Item.Init();
	}

	if( NewItem.CreateTime == 0 && NewItem.Serial == 0 )
	{
		NewItem.CreateTime	= RoleDataEx::G_Now;
		NewItem.Serial		= GenerateItemVersion( );
	}
	Owner->Log_ItemTrade	( Owner->DBID() , -1 , EM_ActionType_MagicBox , NewItem , "" );
	Owner->PlayerBaseData->Body.ItemMagicBox[0] = NewItem;
	Global::Log_ItemTrade( Owner->DBID() , -1 , Owner->DBID() , (int)Owner->X() , (int)Owner->Z() , EM_ActionType_MagicBoxResult , NewItem , "" );

	SC_MagicBoxResult( Owner->GUID() , &NewItem , true );
	
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_ItemChild::RC_GamebleItemState		( BaseItemObject* Obj , int TableID , int ItemPos , ItemFieldStruct& Item , int LockFlag )
{
	RoleDataEx* Owner = Obj->Role();
	bool IsNewTable = false;
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_EqGamble ) == false )
	{
		SC_GamebleItemStateResult( Owner->GUID() , EM_GamebleItemStateResult_DataError , TableID , 0 , 0 );
		return;
	}

	if(	Owner->CheckBodyItem( Item , ItemPos ) == false	)
	{
		SC_GamebleItemStateResult( Owner->GUID() , EM_GamebleItemStateResult_DataError , TableID , 0 , 0 );
		return;
	}

	ItemFieldStruct* BodyItem = Owner->GetBodyItem( ItemPos );
	GameObjDbStructEx*	OrgDB = Global::GetObjDB( BodyItem->OrgObjID );
	if( OrgDB->IsArmor() == false && OrgDB->IsWeapon() == false )
	{
		SC_GamebleItemStateResult( Owner->GUID() , EM_GamebleItemStateResult_DataError , TableID , 0 , 0 );
		return;
	}

	if( BodyItem->Pos != EM_USE_ITEM_GAMBLE && BodyItem->Pos != EM_ItemState_NONE )
	{
		SC_GamebleItemStateResult( Owner->GUID() , EM_GamebleItemStateResult_DataError , TableID , 0 , 0 );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	int LockCount=0;
	int		LockItemID	 = g_ObjectData->GetSysKeyValue( "GamebleItemState_LockItemID" );


	//計算鎖定個數
	for( int i = 0 ; i < 6 ; i++ )
	{
		//if( LuaPlotClass::CheckBit( LockFlag , i ) != false )
		if( TestBit( LockFlag , i ) != false )
			LockCount++;
	}

	if( LockCount > 3 )
	{
		SC_GamebleItemStateResult( Owner->GUID() , EM_GamebleItemStateResult_LockCountError , TableID , 0 , 0 );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	
	int		ItemStateCount = 0;
	int		ItemStateAvgLv = 0;
	int		TopLv1 = 0 , TopLv2 = 0;
	//計算屬性數量
	for( int i = 0 ; i < 6; i++ )
	{
		GameObjDbStructEx*	OrgStateDB = Global::GetObjDB( BodyItem->Inherent(i) );
		if( OrgStateDB->IsAttribute() == false )
			break;
		if( TopLv2 < OrgStateDB->Attirbute.StandardAbility_Lv )
		{						
			TopLv2 = OrgStateDB->Attirbute.StandardAbility_Lv;
		}
		if( TopLv1 < TopLv2 )
		{
			swap( TopLv1, TopLv2 );
		}

		ItemStateCount++;
		ItemStateAvgLv += OrgStateDB->Attirbute.StandardAbility_Lv;
	}

	if( ItemStateCount < 4 )
	{
		SC_GamebleItemStateResult( Owner->GUID() , EM_GamebleItemStateResult_AttributeCountError , TableID , 0 , 0 );
		return;
	}
	ItemStateAvgLv = ItemStateAvgLv - TopLv1 - TopLv2;
	ItemStateAvgLv = ItemStateAvgLv / 4;

	GambleItemStateStruct&	RoleGambleItemState = Owner->PlayerTempData->GambleItemState;
	if(		RoleGambleItemState.GambleTableID < 0 
		||	RoleGambleItemState.BodyItemPos		!= ItemPos
		||	RoleGambleItemState.ItemVersionID	!= BodyItem->Serial )
	{
		IsNewTable	= true;
		//找table
		vector< int > GamebleTableList;

		for( int i = 0; i < (int)g_ObjectData->_GambleItemStateTable.size(); ++i )
		{
			if( ItemStateAvgLv <= g_ObjectData->_GambleItemStateTable[i].MaxLv && 
				ItemStateAvgLv >= g_ObjectData->_GambleItemStateTable[i].MinLv )
			{
				GamebleTableList.push_back( i );
			}
		}

		if( GamebleTableList.empty() )
		{
			SC_GamebleItemStateResult( Owner->GUID() , EM_GamebleItemStateResult_TableDataError , TableID , 0 , 0 );
			return;
		}

		TableID = GamebleTableList[ rand() % GamebleTableList.size() ];
	}
	else
	{
		TableID = Owner->PlayerTempData->GambleItemState.GambleTableID;
	}

	

	//取得賭博表
	GambleItemStateTableStruct& GambleItemStateTable = g_ObjectData->_GambleItemStateTable[ TableID ];
	if(		GambleItemStateTable.MaxLv < ItemStateAvgLv
		||	GambleItemStateTable.MinLv > ItemStateAvgLv )
	{
		SC_GamebleItemStateResult( Owner->GUID() , EM_GamebleItemStateResult_TableIDError , TableID , 0 , 0 );
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//金錢檢查
	//////////////////////////////////////////////////////////////////////////

	//int Cost = ( AbilityLvArg * ItemStateAvgLv + RareArg * OrgDB->Rare + LvArg * OrgDB->Limit.Level +99 ) / 100 * 100;
	int Cost;
	int CostDiscount;
	ObjectDataClass::GetGamebleItemStateCost(  OrgDB->Rare , ItemStateAvgLv , OrgDB->Limit.Level , Cost , CostDiscount );

	if( IsNewTable != true )
	{
		Cost = CostDiscount;
	}
	/*

	if(		RoleGambleItemState.GambleTableID	== TableID 
		&&	RoleGambleItemState.BodyItemPos		== ItemPos
		&&	RoleGambleItemState.ItemVersionID	== BodyItem->Serial )
	{
		//Cost = GambleItemStateTable.ContinueCost;	
		Cost = CostDiscount;
	}
	else
	{
		//Cost = GambleItemStateTable.FirstTimeCost;	
		
		//清除之前的物品資料
		ItemFieldStruct* OldBodyItem = Owner->GetBodyItem( RoleGambleItemState.BodyItemPos );
		if( OldBodyItem != NULL && OldBodyItem->Pos == EM_USE_ITEM_GAMBLE )
		{
			OldBodyItem->Pos = EM_ItemState_NONE;
			Owner->Net_FixItemInfo_Body( RoleGambleItemState.BodyItemPos );
		}
	}
*/
	if( Cost < 0 || Cost > Owner->BodyMoney() )
	{
		SC_GamebleItemStateResult( Owner->GUID() , EM_GamebleItemStateResult_MoneyError , TableID , 0 , 0 );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	int NewItemStateID = 0;
	int NewItemStatePos = 0;
	for( int i = 0 ; i < 1000 ; i++ )
	{
		NewItemStatePos = rand() % 6;
		//if( LuaPlotClass::CheckBit( LockFlag , NewItemStatePos ) != false )
		if( TestBit( LockFlag , NewItemStatePos ) != false )
			continue;
		if( NewItemStatePos > ItemStateCount )
			NewItemStatePos = ItemStateCount+1;
		break;
	}

	for( int i = 0 ; i < 100 ; i++ )
	{
		int TObjID = 0;
		int TCount = 0;

		if( Owner->GetTreasure( GambleItemStateTable.GambleObjID , TObjID  , TCount ) == false )
		{
			SC_GamebleItemStateResult( Owner->GUID() , EM_GamebleItemStateResult_TableDataError , TableID , 0 , 0 );
			return;
		}

		if( BodyItem->CheckInherent( TObjID ) == false )
		{
			NewItemStateID = TObjID;
			break;
		}

	}

	if( NewItemStateID == 0 )
	{
		SC_GamebleItemStateResult( Owner->GUID() , EM_GamebleItemStateResult_TableDataError , TableID , 0 , 0 );
		return;
	}

	//刪除物品
	if( LockCount > 0 )
	{
		if( Owner->DelBodyItem( LockItemID , LockCount , EM_ActionType_Attribute_Gamble ) == false )
		{
			SC_GamebleItemStateResult( Owner->GUID() , EM_GamebleItemStateResult_LockItemError , TableID , 0 , 0 );
			return;
		}
	}

	RoleGambleItemState.GambleTableID	= TableID;
	RoleGambleItemState.BodyItemPos		= ItemPos;
	RoleGambleItemState.ItemVersionID	= BodyItem->Serial;
	RoleGambleItemState.StatePos		= NewItemStatePos;
	RoleGambleItemState.StateID			= NewItemStateID;
	BodyItem->Pos						= EM_USE_ITEM_GAMBLE;
	Owner->Net_FixItemInfo_Body( ItemPos );

	Owner->AddBodyMoney( Cost * -1 , NULL , EM_ActionType_Attribute_Gamble , true );
	//確定成
	SC_GamebleItemStateResult( Owner->GUID() , EM_GamebleItemStateResult_OK , TableID , NewItemStatePos , NewItemStateID );

}
void NetSrv_ItemChild::RC_GetGamebleItemState	( BaseItemObject* Obj , bool GiveUp )
{
	RoleDataEx* Owner = Obj->Role();
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_EqGamble ) == false )
	{
		SC_GetGamebleItemStateResult( Owner->GUID() , false );
		return;
	}

	GambleItemStateStruct&	RGItemState = Owner->PlayerTempData->GambleItemState;
	ItemFieldStruct* BodyItem = Owner->GetBodyItem( RGItemState.BodyItemPos );
	if(		BodyItem == NULL 
		||	BodyItem->Pos != EM_USE_ITEM_GAMBLE 
		||	BodyItem->Serial != RGItemState.ItemVersionID	)
	{
		SC_GetGamebleItemStateResult( Owner->GUID() , false );
		return;
	}

	BodyItem->Pos = EM_ItemState_NONE;
	int BodyItemPos = RGItemState.BodyItemPos;

	if( GiveUp != false )
	{
		RGItemState.Init();
	}
	else
	{
		BodyItem->Inherent( RGItemState.StatePos , RGItemState.StateID );
		//RGItemState.BodyItemPos = -1;
		//RGItemState.GambleTableID = 0;

	}
	RGItemState.Init();

	Owner->Net_FixItemInfo_Body( BodyItemPos );
	SC_GetGamebleItemStateResult( Owner->GUID() , true );
}
void NetSrv_ItemChild::RC_CloseGamebleItemState	( BaseItemObject* Obj )
{
	RoleDataEx* Owner = Obj->Role();	

	if( Owner->TempData.Attr.Action.OpenType == EM_RoleOpenMenuType_EqGamble )
	{
		GambleItemStateStruct&	RGItemState = Owner->PlayerTempData->GambleItemState;
		Owner->TempData.Attr.Action.OpenType  = EM_RoleOpenMenuType_None;
		Owner->TempData.ShopInfo.TargetID = -1;
		RGItemState.Init();
	}

}

void NetSrv_ItemChild::RC_MagicStoneExchangeResult( BaseItemObject* Obj , int AbilityID[3] , bool Result )
{
	RoleDataEx* Owner = Obj->Role();	
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_MagicStoneExchange) == false )
	{
		SC_MagicStoneExchangeResultFailed( Owner->GUID() , EM_MagicStoneExchangeResult_NotOpen );
		return;
	}
	if( Result == false )
	{
		Owner->ClsOpenMenu();
		SC_MagicStoneExchangeResultFailed( Owner->GUID() , EM_MagicStoneExchangeResult_ClientCancel );
		return;
	}

	MagicStoneExchange&	Info = Owner->PlayerTempData->MagicStoneSopeInfo;

	//////////////////////////////////////////////////////////////////////////
	//金錢檢查
	if(		CheckMoneyBase( Owner , g_ObjectData->SysValue().MagicStoneShopCostType[0] , Info.Money[0] ) == false 
		||	CheckMoneyBase( Owner , g_ObjectData->SysValue().MagicStoneShopCostType[1] , Info.Money[1] ) == false  )
	{
		SC_MagicStoneExchangeResultFailed( Owner->GUID() , EM_MagicStoneExchangeResult_MoneyErr );
		return;
	}

	ItemFieldStruct Item;
	Global::NewItemInit( Item , Info.MagicStoneID , 0 );

	Item.Count	  = 1;
	Item.OrgObjID = Info.MagicStoneID;
	Item.RuneVolume = 0;	
	memset( Item.Ability , 0 , sizeof(Item.Ability));

	//檢查三個屬性是否有問題
	vector< int >& AbilityList = g_ObjectData->SysValue().MagicStoneShopAbilityList;
	for( int j = 0 ; j < 3 ; j++ )
	{
		Item._Inherent[j] = AbilityID[j] - _DEF_ITEM_ABILITY_DELTA_;
		bool IsFind = false;
		int CheckAbility = AbilityID[j] - Info.Lv + j + 1;
		for( unsigned i = 0 ; i < AbilityList.size() ; i++ )
		{
			if( AbilityList[i] == CheckAbility )
			{
				IsFind = true;
				break;
			}
		}
		if( IsFind == false )
		{
			SC_MagicStoneExchangeResultFailed( Owner->GUID() , EM_MagicStoneExchangeResult_ClientDataErr );
			return;
		}
	}

	AddMoneyBase( Owner , EM_ActionType_MagicStoneShop , g_ObjectData->SysValue().MagicStoneShopCostType[0] , Info.Money[0]*-1 );
	AddMoneyBase( Owner , EM_ActionType_MagicStoneShop , g_ObjectData->SysValue().MagicStoneShopCostType[1] , Info.Money[1]*-1 );

	Owner->GiveItem( Item , EM_ActionType_MagicStoneShop , Owner , "" );
	Owner->ClsOpenMenu();

	SC_MagicStoneExchangeResultOK( Owner->GUID() , Item , Info.Money );
}

void NetSrv_ItemChild::RC_DrawOutRuneRequest( BaseItemObject* Obj , int BodyPos , int RuneID )
{
	RoleDataEx* Owner = Obj->Role();
	ItemFieldStruct* Item = Owner->GetBodyItem( BodyPos );

	if( Item == NULL || Item->IsEmpty() )
	{
		SC_DrawOutRuneResult( Owner->GUID() , EM_DrawOutRuneResult_DataErr );
		return;
	}
	if( Owner-> PlayerBaseData->ItemTemp.Size() > 10 )
	{
		SC_DrawOutRuneResult( Owner->GUID() , EM_DrawOutRuneResult_ItemQueueErr );
		return;
	}

	if( Item->Mode.ItemLock )
	{
		SC_DrawOutRuneResult( Owner->GUID() , EM_DrawOutRuneResult_ItemLock );
		return;
	}

	GameObjDbStructEx*	OrgDB = Global::GetObjDB( Item->OrgObjID );
	if(		OrgDB->IsWeapon() == false 
		&&	OrgDB->IsArmor() == false	)
	{
		SC_DrawOutRuneResult( Owner->GUID() , EM_DrawOutRuneResult_DataErr );
		return;
	}
	if( OrgDB->MaxHeap > 1 )
	{
		SC_DrawOutRuneResult( Owner->GUID() , EM_DrawOutRuneResult_DataErr );
		return;
	}

	GameObjDbStructEx*	RuneDB = Global::GetObjDB( RuneID );
	if( RuneDB == NULL )
	{
		SC_DrawOutRuneResult( Owner->GUID() , EM_DrawOutRuneResult_RuneErr );
		return;
	}

	int Cost = RuneDB->Item.DrawOutCost;//g_ObjectData->GetSysKeyValue( "DrawOutRuneCost" );
	if( Owner->BodyMoney() < Cost )
	{
		SC_DrawOutRuneResult( Owner->GUID() , EM_DrawOutRuneResult_MoneyErr );
		return;
	}

	int FindRunePos = -1;
	for( int i = 0 ; i < 4 ; i++ )	
	{
		if( RuneID == Item->Rune(i) )
		{
			FindRunePos = i;
			break;
		}
	}
	
	if( FindRunePos == -1 )
	{
		SC_DrawOutRuneResult( Owner->GUID() , EM_DrawOutRuneResult_RuneErr );
		return;
	}
/*
	GameObjDbStructEx*	RuneDB = Global::GetObjDB( RuneID );
	if( RuneDB->IsRune() == false )
	{
		SC_DrawOutRuneResult( Owner->GUID() , EM_DrawOutRuneResult_RuneErr );
		return;
	}
*/	
	//扣錢
	Owner->AddBodyMoney( Cost*-1 , NULL , EM_ActionType_DestroyItem_DrawOutRune , true );

	Item->RuneVolume += RuneDB->Item.Volume;
	//清符文
	Item->Rune( FindRunePos , 0 );
	for( int i = FindRunePos ; i < 3 ; i++ )
	{
		Item->_Rune[ i ] = Item->_Rune[ i + 1  ];
	}
	Item->_Rune[ 3 ] = 0;
	Owner->GiveItem(  RuneID , 1 , EM_ActionType_DestroyItem_DrawOutRune , Owner , "" );
	Owner->Net_FixItemInfo_Body( BodyPos );

	SC_DrawOutRuneResult( Owner->GUID() , EM_DrawOutRuneResult_OK );	
}

//////////////////////////////////////////////////////////////////////////
//魔法衣櫥
//////////////////////////////////////////////////////////////////////////
void NetSrv_ItemChild::RC_PutCoolClothRequest		( BaseItemObject* Obj , int BodyPos , int CoolClothPos )
{
	RoleDataEx* Owner = Obj->Role();
	ItemFieldStruct* Item = Owner->GetBodyItem( BodyPos );
	if( Item == NULL || Item->IsEmpty() )
	{
		SC_PutCoolClothResult	( Owner->GUID() , EM_PutCoolClothResult_NotFind );
		return;
	}


	if( (unsigned)CoolClothPos >= _DEF_MAX_COOL_CLOTH_COUNT_ || Owner->PlayerBaseData->CoolClothList[CoolClothPos] != 0 )
	{
		SC_PutCoolClothResult	( Owner->GUID() , EM_PutCoolClothResult_ClothPosErr );
		return;
	}

	//檢查此欄位是否有購買
	{
		int page = CoolClothPos / 50 + 1;
		if( page > 2 )
		{
			char keyStr[256];
			sprintf( keyStr , "Wardrobe_ItemPage%d" , page );
			int	keyValue	 = g_ObjectData->GetSysKeyValue( keyStr);
			//if( Owner->CheckKeyItem( keyValue ) == false )
			if( Owner->Base.AccountFlag.GetFlag( keyValue ) == false )
			{
				SC_PutCoolClothResult	( Owner->GUID() , EM_PutCoolClothResult_KeyItemErr );
				return;
			}
		}
	}

	if( Item->ImageObjectID == 0 )
		Item->ImageObjectID = Item->OrgObjID;

	GameObjDbStructEx*	OrgDB = Global::GetObjDB( Item->ImageObjectID );

	if( OrgDB->IsArmor() == false )
	{
		SC_PutCoolClothResult	( Owner->GUID() , EM_PutCoolClothResult_TypeErr );
		return;
	}
	
	switch( OrgDB->Item.ArmorPos )
	{
	case EM_ArmorPos_Head:		//頭
	case EM_ArmorPos_Gloves:	//手套
	case EM_ArmorPos_Shoes:		//鞋子
	case EM_ArmorPos_Clothes:   //上衣
	case EM_ArmorPos_Pants:		//褲子
	case EM_ArmorPos_Back:		//背部
	case EM_ArmorPos_Belt:		//腰帶
	case EM_ArmorPos_Shoulder:  //肩甲
	case EM_ArmorPos_Ornament:	//裝飾品
		break;
	case EM_ArmorPos_SecondHand://副手
	case EM_ArmorPos_Necklace:  //項鍊 
	case EM_ArmorPos_Ring:		//戒子
	case EM_ArmorPos_Earring:	//耳飾
	case EM_ArmorPos_MagicTool:	//法寶
		{
			SC_PutCoolClothResult	( Owner->GUID() , EM_PutCoolClothResult_TypeErr );
			return;
		}
	}
	

	if(		OrgDB->IsArmor() == false 
		||	OrgDB->Mode.SpecialSuit == false )
	{
		//檢查是否有融合核心
		int	PutinItemID = g_ObjectData->GetSysKeyValue( "Wardrobe_PutInItemID" );

		if( Owner->CalBodyItemCount( PutinItemID ) == 0 )
		{
			SC_PutCoolClothResult	( Owner->GUID() , EM_PutCoolClothResult_TypeErr );
			return;
		}
		Owner->DelBodyItem( PutinItemID , 1 , EM_ActionType_PutItemToWarDrobe );
	}
/*
	if(		 OrgDB->Limit.Sex._Sex != 0 )
	{
		if(		( OrgDB->Limit.Sex.Boy  && Owner->BaseData.Sex == EM_Sex_Girl ) 
			||	( OrgDB->Limit.Sex.Girl && Owner->BaseData.Sex == EM_Sex_Boy ) )
		{
			SC_PutCoolClothResult	( Owner->GUID() , EM_PutCoolClothResult_TypeErr );
			return;
		}

	}
*/
/*	if( Owner->CheckLimit( OrgDB->Limit , true ) == false )
	{
		SC_PutCoolClothResult	( Owner->GUID() , EM_PutCoolClothResult_LimitErr );
		return;
	}
	*/

	Owner->PlayerBaseData->CoolClothList[CoolClothPos]  = Item->ImageObjectID;
	
//	if( Item->ImageObjectID == Item->OrgObjID )
	{
		Owner->Log_ItemDestroy( EM_ActionType_DestroyItem_CoolSuit , *Item , 1 , "" );
		Item->Init();
		Owner->Net_FixItemInfo_Body( BodyPos );
	}
/*	else
	{
		Item->ImageObjectID = Item->OrgObjID;
		Owner->Net_FixItemInfo_Body( BodyPos );
	}*/

	SC_PutCoolClothResult	( Owner->GUID() , EM_PutCoolClothResult_OK );
}
void NetSrv_ItemChild::RC_DelCoolClothRequest		( BaseItemObject* Obj , int Pos , int ImageObjectID )
{
	RoleDataEx* Owner = Obj->Role();
	if( (unsigned)Pos >= _DEF_MAX_COOL_CLOTH_COUNT_ )
	{
		SC_DelCoolClothResult( Owner->GUID() , false );
		return;
	}

	if( Owner->PlayerBaseData->CoolClothList[Pos] != ImageObjectID )
	{
		SC_DelCoolClothResult( Owner->GUID() , false );
		return;
	}
	
	Owner->PlayerBaseData->CoolClothList[ Pos ] = 0;
	SC_DelCoolClothResult( Owner->GUID() , true );
	return;
}

void NetSrv_ItemChild::RC_SwapCoolClothRequest	( BaseItemObject* Obj , int Pos[2] )
{
	RoleDataEx* Owner = Obj->Role();
	if(		(unsigned)Pos[0] >= _DEF_MAX_COOL_CLOTH_COUNT_ 
		||	(unsigned)Pos[1] >= _DEF_MAX_COOL_CLOTH_COUNT_ )
	{
		SC_SwapCoolClothResult( Owner->GUID() , false );
		return;
	}

	for( int i = 0 ; i < 2 ; i++ )
	{
		int page = Pos[i] / 50 + 1;
		if( page > 2 )
		{
			char keyStr[256];
			sprintf( keyStr , "Wardrobe_ItemPage%d" , page );
			int	keyValue	 = g_ObjectData->GetSysKeyValue( keyStr);
			//if( Owner->CheckKeyItem( keyValue ) == false )
			if( Owner->Base.AccountFlag.GetFlag( keyValue ) == false )
			{
				SC_SwapCoolClothResult( Owner->GUID() , false );
				return;
			}
		}
	}


	swap( Owner->PlayerBaseData->CoolClothList[ Pos[0] ] , Owner->PlayerBaseData->CoolClothList[ Pos[1] ] );

	SC_SwapCoolClothResult( Owner->GUID() , true );
}

void NetSrv_ItemChild::RC_SetCoolSuitRequest		( BaseItemObject* Obj , int SuitIndexID , CoolSuitBase& Info )
{
	RoleDataEx* Owner = Obj->Role();
	if( (unsigned)SuitIndexID > _DEF_MAX_COOL_SUIT_COUNT_ )
	{
		SC_SetCoolSuitResult( Owner->GUID() , false );
		return;
	}

	//檢查重要物品
	if( SuitIndexID >= 5 )
	{
		char keyStr[256];
		sprintf( keyStr , "Wardrobe_Set%d" , SuitIndexID+1 );
		int	keyValue	 = g_ObjectData->GetSysKeyValue( keyStr);
		//if( Owner->CheckKeyItem( keyValue ) == false )
		if( Owner->Base.AccountFlag.GetFlag( keyValue ) == false )
		{
			SC_SetCoolSuitResult( Owner->GUID() , false );
			return;
		}
	}

	int* CoolClothList = Owner->PlayerBaseData->CoolClothList;

	for( int i = 0 ; i < 9 ; i++ )
	{
		if( Info.Item[i].ImageObjectID != 0 /*&&  CoolClothList.Find( Info.Item[i].ImageObjectID ) != -1 */ )
		{
			GameObjDbStructEx*	OrgDB = Global::GetObjDB( Info.Item[i].ImageObjectID );
			if(	OrgDB != NULL && OrgDB->Limit.Sex._Sex != 0 )
			{
				if( OrgDB->Limit.Sex.Boy != OrgDB->Limit.Sex.Girl )
				{
					if(		( OrgDB->Limit.Sex.Boy  && Owner->BaseData.Sex == EM_Sex_Girl ) 
						||	( OrgDB->Limit.Sex.Girl && Owner->BaseData.Sex == EM_Sex_Boy ) )
					{
						SC_SetCoolSuitResult( Owner->GUID() , false );
						return;
					}
				}
			}

			bool IsFind = false;
			for( int j = 0 ; j < _DEF_MAX_COOL_CLOTH_COUNT_ ; j++ )
			{
				if( CoolClothList[j] ==  Info.Item[i].ImageObjectID )
				{
					IsFind = true;
					break;
				}
			}
			if( IsFind == false )
			{
				SC_SetCoolSuitResult( Owner->GUID() , false );
				return;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		GameObjDbStructEx*	OrgDB = Global::GetObjDB( Info.Head.ImageObjectID );
		if( OrgDB != NULL && OrgDB->Item.ArmorPos != EM_ArmorPos_Head )
		{
			SC_SetCoolSuitResult( Owner->GUID() , false );
			return;
		}
		OrgDB = Global::GetObjDB( Info.Gloves.ImageObjectID );
		if( OrgDB != NULL && OrgDB->Item.ArmorPos != EM_ArmorPos_Gloves )
		{
			SC_SetCoolSuitResult( Owner->GUID() , false );
			return;
		}
		OrgDB = Global::GetObjDB( Info.Shoes.ImageObjectID );
		if( OrgDB != NULL && OrgDB->Item.ArmorPos != EM_ArmorPos_Shoes )
		{
			SC_SetCoolSuitResult( Owner->GUID() , false );
			return;
		}
		OrgDB = Global::GetObjDB( Info.Clothes.ImageObjectID );
		if( OrgDB != NULL && OrgDB->Item.ArmorPos != EM_ArmorPos_Clothes )
		{
			SC_SetCoolSuitResult( Owner->GUID() , false );
			return;
		}
		OrgDB = Global::GetObjDB( Info.Pants.ImageObjectID );
		if( OrgDB != NULL && OrgDB->Item.ArmorPos != EM_ArmorPos_Pants )
		{
			SC_SetCoolSuitResult( Owner->GUID() , false );
			return;
		}		
		OrgDB = Global::GetObjDB( Info.Back.ImageObjectID );
		if( OrgDB != NULL && OrgDB->Item.ArmorPos != EM_ArmorPos_Back )
		{
			SC_SetCoolSuitResult( Owner->GUID() , false );
			return;
		}
		OrgDB = Global::GetObjDB( Info.Belt.ImageObjectID );
		if( OrgDB != NULL && OrgDB->Item.ArmorPos != EM_ArmorPos_Belt )
		{
			SC_SetCoolSuitResult( Owner->GUID() , false );
			return;
		}
		OrgDB = Global::GetObjDB( Info.Shoulder.ImageObjectID );
		if( OrgDB != NULL && OrgDB->Item.ArmorPos != EM_ArmorPos_Shoulder )
		{
			SC_SetCoolSuitResult( Owner->GUID() , false );
			return;
		}
		OrgDB = Global::GetObjDB( Info.Ornament.ImageObjectID );
		if( OrgDB != NULL && OrgDB->Item.ArmorPos != EM_ArmorPos_Ornament )
		{
			SC_SetCoolSuitResult( Owner->GUID() , false );
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Owner->PlayerBaseData->CoolSuitList[SuitIndexID] = Info;

	if( Owner->PlayerBaseData->CoolSuitID == SuitIndexID )
	{
		NetSrv_MoveChild::Send_LookEq( Owner );
	}

	SC_SetCoolSuitResult( Owner->GUID() , true );
}
void NetSrv_ItemChild::RC_ShowCoolSuitIndexRequest( BaseItemObject* Obj , int CoolSuitIndexID )
{
	RoleDataEx* Owner = Obj->Role();
	if( unsigned( CoolSuitIndexID ) >= _DEF_MAX_COOL_SUIT_COUNT_ )
	{
		SC_ShowCoolSuitIndexResult( Owner->GUID() , false );
		return;
	}

	//檢查重要物品
	if( CoolSuitIndexID >= 5 )
	{
		char keyStr[256];
		sprintf( keyStr , "Wardrobe_Set%d" , CoolSuitIndexID+1 );
		int	keyValue	 = g_ObjectData->GetSysKeyValue( keyStr);
		//if( Owner->CheckKeyItem( keyValue ) == false )
		if( Owner->Base.AccountFlag.GetFlag( keyValue ) == false )
		{
			SC_ShowCoolSuitIndexResult( Owner->GUID() , false );
			return;
		}
	}


	if( Owner->PlayerBaseData->CoolSuitID != CoolSuitIndexID )
	{
		Owner->PlayerBaseData->CoolSuitID = CoolSuitIndexID;
		NetSrv_MoveChild::Send_LookEq( Owner );
	}


	SC_ShowCoolSuitIndexResult( Owner->GUID() , true );
}

void NetSrv_ItemChild::RC_PopExBagRequest			( BaseItemObject* Obj , ExBagTypeENUM ExBagType , int ExBagPos , int BodyPos )
{
	RoleDataEx* Owner = Obj->Role();

	ItemFieldStruct* ItemInfo = Owner->GetBodyItem( BodyPos );
	if( ItemInfo == NULL || ItemInfo->IsEmpty() == false )
	{
		SC_PopExBagResult( Owner->GUID() , EM_PopExBagResult_Failed );
		return;
	}
	
	switch( ExBagType)
	{	
	case EM_ExBagType_Horse:
		{
			//檢查此坐騎是否已經存在
			StaticVector< HorseExBagBase , _DEF_MAX_HORSE_EXBAG_COUNT_ >& HorseExBag = Owner->PlayerBaseData->HorseExBag;

			if( HorseExBag.Size() <= ExBagPos )
			{
				SC_PopExBagResult( Owner->GUID() , EM_PopExBagResult_Failed );
				return;
			}
			HorseExBagBase& ExBagItem = HorseExBag[ExBagPos];
			
			ItemInfo->OrgObjID = ItemInfo->ImageObjectID = ExBagItem.ItemID;;
			ItemInfo->Count			= 1;
			ItemInfo->Serial		= GenerateItemVersion( );
			ItemInfo->CreateTime	= RoleDataEx::G_Now;			
			memcpy( ItemInfo->HorseColor , ExBagItem.HorseColor , sizeof(ItemInfo->HorseColor ) );


			HorseExBag.Erase( ExBagPos );
			Owner->Net_FixItemInfo_Body( BodyPos );
			SC_PopExBagResult( Owner->GUID() , EM_PopExBagResult_OK );

			Owner->Log_ItemTrade( 0 , 0 , EM_ActionType_HorseBag_Out , *ItemInfo  , "" );
			break;
		}
	case EM_ExBagType_Pet:
		{
			//SC_PopExBagResult( Owner->GUID() , EM_PopExBagResult_Failed );
			//檢查此寵物是否已經存在
			StaticVector< int , _DEF_MAX_PET_EXBAG_COUNT_ >& PetExBag = Owner->PlayerBaseData->PetExBag;
			if( PetExBag.Size() <= ExBagPos )
			{
				SC_PopExBagResult( Owner->GUID() , EM_PopExBagResult_Failed );
				return;
			}

			int ExBagItemID = PetExBag[ExBagPos];

			ItemInfo->OrgObjID = ItemInfo->ImageObjectID = ExBagItemID;
			ItemInfo->Count			= 1;
			ItemInfo->Serial		= GenerateItemVersion( );
			ItemInfo->CreateTime	= RoleDataEx::G_Now;			

			PetExBag.Erase( ExBagPos );
			Owner->Net_FixItemInfo_Body( BodyPos );
			SC_PopExBagResult( Owner->GUID() , EM_PopExBagResult_OK );
			Owner->Log_ItemTrade( 0 , 0 , EM_ActionType_PetBag_Out , *ItemInfo  , "" );
			break;
		}
	default:
		{
			SC_PopExBagResult( Owner->GUID() , EM_PopExBagResult_Failed );
			break;
		}
	}

}

void NetSrv_ItemChild::RC_PutInExBagRequest( BaseItemObject* Obj , ExBagTypeENUM ExBagType , int ItemID , int BodyPos )
{
	RoleDataEx* Owner = Obj->Role();
	ItemFieldStruct* ItemInfo = Owner->GetBodyItem( BodyPos );

	if( ItemInfo->IsEmpty() || ItemInfo->OrgObjID != ItemID )
	{
		SC_PutInExBagResult( Owner->GUID() , EM_PutInExBagResult_DataErr );
		return;
	}

	GameObjDbStructEx*	ItemDB = Global::GetObjDB( ItemID );
	if( ItemDB->IsItem() == false || ItemDB->Item.ExBagType != ExBagType )
	{
		SC_PutInExBagResult( Owner->GUID() , EM_PutInExBagResult_DataErr );
		return;
	}

	if( ItemDB->Mode.Expense || ItemInfo->Count != 1 )
	{
		SC_PutInExBagResult( Owner->GUID() , EM_PutInExBagResult_DataErr );
		return;
	}

	switch( ExBagType)
	{
	case EM_ExBagType_Pet:
		{
			//檢查此寵物是否已經存在
			StaticVector< int , _DEF_MAX_PET_EXBAG_COUNT_ >& PetExBag = Owner->PlayerBaseData->PetExBag;

			if( PetExBag.Size() == _DEF_MAX_PET_EXBAG_COUNT_ )
			{
				SC_PutInExBagResult( Owner->GUID() , EM_PutInExBagResult_Full );
				return;
			}

			for( int i = 0 ; i < PetExBag.Size() ; i++ )
			{
				if( PetExBag[i] == ItemID )
				{
					SC_PutInExBagResult( Owner->GUID() , EM_PutInExBagResult_Exist );
					return;
				}
			}	
			PetExBag.Push_Back( ItemID );

			Owner->Log_ItemTrade( 0 , 0 , EM_ActionType_PetBag_In , *ItemInfo  , "" );

			ItemInfo->Init();
			Owner->Net_FixItemInfo_Body( BodyPos );
			SC_PutInExBagResult( Owner->GUID() , EM_PutInExBagResult_OK );

			int cardID = g_ObjectData->GetPetCardID( ItemID );
			if( cardID != -1 )
				Owner->SetValue( (RoleValueName_ENUM)(EM_RoleValue_PetCard + cardID) , 1 );
			break;
		}
	case EM_ExBagType_Horse:
		{
			//檢查此坐騎是否已經存在
			StaticVector< HorseExBagBase , _DEF_MAX_HORSE_EXBAG_COUNT_ >& HorseExBag = Owner->PlayerBaseData->HorseExBag;

			if( HorseExBag.Size() == _DEF_MAX_HORSE_EXBAG_COUNT_ )
			{
				SC_PutInExBagResult( Owner->GUID() , EM_PutInExBagResult_Full );
				return;
			}

			for( int i = 0 ; i < HorseExBag.Size() ; i++ )
			{
				if(		HorseExBag[i].ItemID == ItemID 
					&&	memcmp( HorseExBag[i].HorseColor , ItemInfo->HorseColor , sizeof(ItemInfo->HorseColor ) ) == 0 )
				{
					SC_PutInExBagResult( Owner->GUID() , EM_PutInExBagResult_Exist );
					return;
				}
			}	

			HorseExBagBase TempItem;
			TempItem.ItemID = ItemID;
			memcpy( TempItem.HorseColor , ItemInfo->HorseColor , sizeof(ItemInfo->HorseColor ) );

			HorseExBag.Push_Back( TempItem );

			Owner->Log_ItemTrade( 0 , 0 , EM_ActionType_HorseBag_In , *ItemInfo  , "" );

			ItemInfo->Init();
			Owner->Net_FixItemInfo_Body( BodyPos );
			SC_PutInExBagResult( Owner->GUID() , EM_PutInExBagResult_OK );

			break;
		}
	default:
		{
			SC_PutInExBagResult( Owner->GUID() , EM_PutInExBagResult_DataErr );
			return;
		}
	}
}
void NetSrv_ItemChild::RC_UseExBagRequest( BaseItemObject* Obj , ExBagTypeENUM ExBagType , int BagPos )
{
	RoleDataEx *Owner = Obj->Role();

	int ItemID = 0;

	switch( ExBagType )
	{		
	case EM_ExBagType_Pet:
		{
			StaticVector< int , _DEF_MAX_PET_EXBAG_COUNT_ >& PetExBag = Owner->PlayerBaseData->PetExBag;
			if( (unsigned)BagPos >= (unsigned)PetExBag.Size() )
			{
				SC_UseExBagResult( Owner->GUID() , false );
				return;
			}
			ItemID = PetExBag[BagPos];
		}
		break;
	case EM_ExBagType_Horse:
		{
			StaticVector< HorseExBagBase , _DEF_MAX_HORSE_EXBAG_COUNT_ >& HorseExBag = Owner->PlayerBaseData->HorseExBag;
			if( (unsigned)BagPos >= (unsigned)HorseExBag.Size() )
			{
				SC_UseExBagResult( Owner->GUID() , false );
				return;
			}
			ItemID = HorseExBag[BagPos].ItemID;
			memcpy( Owner->TempData.Magic.HorseColor , HorseExBag[BagPos].HorseColor , sizeof(Owner->TempData.Magic.HorseColor));
		}
		break;
	default:
		SC_UseExBagResult( Owner->GUID() , false );
		return;		
	}

	GameObjDbStructEx* ItemDB = Global::GetObjDB( ItemID );	

	if( ItemDB == NULL || Owner->CheckLimit( ItemDB->Limit ) == false )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_LimitErr );
		SC_UseExBagResult( Owner->GUID() , false );
		return;
	}
	if( ItemDB->Item.CastingTime > 0 )
	{
		if(		Owner->PlayerTempData->CastingType != EM_CastingType_OKRESULT 
			||	Owner->PlayerTempData->CastingItemID != ItemDB->GUID )
		{
			SC_UseExBagResult( Owner->GUID() , false );
			return;
		}
	}
	else
	{
		if(		Owner->PlayerTempData->CastingType != EM_CastingType_NULL 
			&&	Owner->PlayerTempData->CastingType != EM_CastingType_OKRESULT )
		{
			SC_UseExBagResult( Owner->GUID() , false );
			return;
		}
	}	

	Owner->PlayerTempData->CastingType = EM_CastingType_NULL;


	vector<MyVMValueStruct> RetList;
	if( strlen( ItemDB->Item.CheckUseScript ) != 0 )
	{
		bool Result = true;
		if( LUA_VMClass::PCallByStrArg( ItemDB->Item.CheckUseScript , Owner->GUID() , Owner->GUID() , &RetList , 1 ) )
		{
			if( RetList.size() != 0 )
			{
				MyVMValueStruct& Temp = RetList[0];
				if( Temp.Flag != true )
				{
					Result = false;
				}
			}
		}
		else
		{
			Result = false;
		}

		if( Result == false )
		{
			SC_UseExBagResult( Owner->GUID() , false );
			return;
		}
	}


	switch( ItemDB->Item.UseType )
	{
	case EM_ItemUseType_None:
		break;
	case EM_ItemUseType_Magic:
		MagicProcessClass::SpellMagic( Owner->GUID() , Owner->GUID() , Owner->X() , Owner->Y() , Owner->Z() , ItemDB->Item.IncMagic_Onuse  , 0 );
		break;
	case EM_ItemUseType_SrvScript:
		LUA_VMClass::PCallByStrArg( ItemDB->Item.SrvScript , Owner->GUID() , Owner->GUID() , &RetList , 1 );
		break;
	}

	SC_UseExBagResult( Owner->GUID() , true );
}

void NetSrv_ItemChild::RC_Rare3EqExchangeItemRequest( BaseItemObject* Obj , int Type , int ItemPos[6] )
{
	RoleDataEx* Role = Obj->Role();
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_Rare3EqExchangeItem ) == false )
		return;

	vector< int > LvList;
	int Cost = 0;
	for( int i = 0 ; i < 6 ; i++ )
	{
		ItemFieldStruct* ItemInfo = Role->GetBodyItem( ItemPos[i] );
		if( ItemInfo == NULL )
		{
			LvList.push_back( -1 );
			continue;
		}
		
		GameObjDbStructEx* Itemobj = Global::GetObjDB( ItemInfo->OrgObjID );
		if(		Itemobj->IsArmor() == false 
			&&	Itemobj->IsWeapon() == false )
		{
			SC_Rare3EqExchangeItemResult( Role->GUID() , EM_Rare3EqExchangeItem_EQErr );
			return;
		}
		if( Itemobj->Rare != 3 )
		{
			SC_Rare3EqExchangeItemResult( Role->GUID() , EM_Rare3EqExchangeItem_EQErr );
			return;
		}
		LvList.push_back( Itemobj->Limit.Level );

		Cost += (Itemobj->Limit.Level /10 )*(Itemobj->Limit.Level /10 )*1000 + 5000;
	}

	//金錢計算
	if( Role->BodyMoney() < Cost )
	{
		SC_Rare3EqExchangeItemResult( Role->GUID() , EM_Rare3EqExchangeItem_MoneyError );
		return;
	}

	//執行lua
	char	luaScript[512];
	vector< MyVMValueStruct > RetList;
	sprintf( luaScript , "Rare3EqExchangeProc( %d,%d,%d,%d,%d,%d,%d )" , Type , LvList[0] , LvList[1] , LvList[2] , LvList[3] , LvList[4] , LvList[5] );
	if(		LUA_VMClass::PCallByStrArg( luaScript , Role->GUID() , Role->GUID() , &RetList , 1 ) == false 
		||	( RetList.size() == 1 && RetList[0].Flag == false ) )
	{
		SC_Rare3EqExchangeItemResult( Role->GUID() , EM_Rare3EqExchangeItem_LuaErr );
		return;
	}
	
	//扣錢
	Role->AddBodyMoney( Cost * -1 , NULL , EM_ActionType_Rare3EqExchangeItem , true );
	//刪除物品
	//Owner->Net_FixItemInfo_Body( Pos );
	//Owner->Log_ItemDestroy(  EM_ActionType_UseItemDestroy , *SrvItem , 1 , "" );
	for( int i = 0 ; i < 6 ; i++ )
	{
		ItemFieldStruct* ItemInfo = Role->GetBodyItem( ItemPos[i] );
		if( ItemInfo == NULL )
			continue;

		Role->Log_ItemDestroy(  EM_ActionType_Rare3EqExchangeItem , *ItemInfo , 1 , "" );
		ItemInfo->Init();
		Role->Net_FixItemInfo_Body( ItemPos[i] );
	}
	SC_Rare3EqExchangeItemResult( Role->GUID() , EM_Rare3EqExchangeItem_OK );
}
void NetSrv_ItemChild::RC_CloseRare3EqExchangeItem( BaseItemObject* Obj )
{
	RoleDataEx* Owner = Obj->Role();
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_Rare3EqExchangeItem ) == false )
	{
		return;
	}
	Owner->ClsOpenMenu();
}
void NetSrv_ItemChild::RC_ClsAttribute( BaseItemObject* Obj , int Type , int ItemPos , int AttributeID )
{
	RoleDataEx* Owner = Obj->Role();

	ItemFieldStruct* item = Owner->GetItem( ItemPos , Type );
	if( item->IsEmpty() )
	{
		SC_ClsAttributeResult( Owner->GUID() , false );
		return;
	}

	GameObjDbStructEx* ItemDB = Global::GetObjDB( item->OrgObjID );	
	if( ItemDB->IsArmor() == false && ItemDB->IsWeapon() == false )
	{
		SC_ClsAttributeResult( Owner->GUID() , false );
		return;
	}

	int AttributeClearItemID = g_ObjectData->GetSysKeyValue( "AttributeClearItemID" );

	if( Owner->CalBodyItemCount( AttributeClearItemID ) == 0 )
	{
		SC_ClsAttributeResult( Owner->GUID() , false );
		return;
	}
	Owner->DelBodyItem( AttributeClearItemID , 1 , EM_ActionType_ClsItemAttribute );

	int findPos = -1;
	for( int i = 0 ; i < 6 ; i++ )
	{
		if( item->Inherent( i ) == AttributeID )
		{
			findPos = i;
			break;
		}
	}

	if( findPos == -1 )
	{
		SC_ClsAttributeResult( Owner->GUID() , false );
		return;
	}

	for( int i = findPos ; i < 5 ; i++ )
	{
		item->_Inherent[i] = item->_Inherent[i + 1 ];
	}
	item->_Inherent[5] = 0;

	Owner->Net_FixItemInfo( ItemPos , Type );

	Global::Log_ItemTrade( Owner->DBID() , -1 , Owner->DBID() , Owner->X() , Owner->Z() , EM_ActionType_Item_ClsAttribute , *item , "" );
	SC_ClsAttributeResult( Owner->GUID() , true );
}

void NetSrv_ItemChild::RC_MagicBoxAttrDrawOff( BaseItemObject* Obj )
{
	RoleDataEx* Owner = Obj->Role();

	if(		Owner->CheckKeyItem( g_ObjectData->SysValue().MagicBoxOpenKeyItemID ) == false )
	{
		SC_MagicBoxAttrDrawOffResult( Owner->GUID() , false );
		return;
	}

	if( Owner->PlayerBaseData->BoxEnergy < 1 )
	{
		SC_MagicBoxAttrDrawOffResult( Owner->GUID() , false );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	if(		Owner->PlayerBaseData->Body.ItemMagicBox[2].OrgObjID != g_ObjectData->GetSysKeyValue("AttributeDrawOffItemID") 
		||	Owner->PlayerBaseData->Body.ItemMagicBox[2].Count != 1)
	{
		SC_MagicBoxAttrDrawOffResult( Owner->GUID() , false );
		return;
	}

	ItemFieldStruct& Item = Owner->PlayerBaseData->Body.ItemMagicBox[1];
	//檢查是否可以萃取物品
	GameObjDbStructEx* ItemDB = Global::GetObjDB( Item.OrgObjID );	
	if(		ItemDB->IsItem() == false 
		||	ItemDB->Mode.DisableMagicBox == true )
	{
		SC_MagicBoxAttrDrawOffResult( Owner->GUID() , false );
		return;
	}

	if(		ItemDB->IsWeapon() == false
		&&	ItemDB->IsArmor() == false	 )
	{
		SC_MagicBoxAttrDrawOffResult( Owner->GUID() , false );
		return;
	}
	if( ItemDB->MaxHeap > 1 )
	{
		SC_MagicBoxAttrDrawOffResult( Owner->GUID() , false );
		return;
	}

	//清除所有寶箱物品
	for( int i = 0 ; i < _MAX_BODY_MAGIC_BOX_COUNT_ ; i++ )
	{
		if( i == 1 || i == 2 )
			continue;
		ItemFieldStruct& Item = Owner->PlayerBaseData->Body.ItemMagicBox[i];
		if( Item.IsEmpty() == false )
		{
			SC_MagicBoxAttrDrawOffResult( Owner->GUID() , false );
			return;
		}
	}


	//取得屬性資訊
	vector< short > attr;
	for( int i = 0 ; i < 6 ; i++ )
	{
		if( Item._Inherent[i] != 0 )
			attr.push_back( Item._Inherent[i] );
	}

	if( attr.size() == 0 )
	{
		SC_MagicBoxAttrDrawOffResult( Owner->GUID() , false );
		return;
	}

	int MagicStoneID;
	ItemFieldStruct MagicStone;
	MagicStone.Init();
	unsigned ResultLv = (ItemDB->Limit.Level - 1 +  RoleDataEx::RareAddLv( ItemDB->Rare+Item.Rare )*3/2 ) / 20;
	if( ResultLv >= unsigned( g_ObjectData->SysValue().MagicStoneCount ) )
	{
		ResultLv = unsigned( g_ObjectData->SysValue().MagicStoneCount ) - 1; 
	}
	MagicStoneID = g_ObjectData->SysValue().MagicStoneStart + ResultLv;


	MagicStone.OrgObjID = MagicStone.ImageObjectID = MagicStoneID;
	MagicStone.Count = 1;
	MagicStone.Mode.Trade = true;
	MagicStone.CreateTime	= RoleDataEx::G_Now;
	MagicStone.Serial		= GenerateItemVersion( );

	int randValue = rand() % (int)attr.size();
	MagicStone._Inherent[0] = attr[ randValue ];

	attr.erase( attr.begin() + randValue );

	memset( Item._Inherent , 0 , sizeof(Item._Inherent) );
	for( int i = 0 ; i < attr.size() ; i++ )
		Item._Inherent[i] = attr[i];
	

	Owner->Log_ItemDestroy( EM_ActionType_MagicBox_Destroy , Owner->PlayerBaseData->Body.ItemMagicBox[2] , -1 , "" );
	Owner->PlayerBaseData->Body.ItemMagicBox[2].Init();


	//////////////////////////////////////////////////////////////////////////
	Owner->PlayerBaseData->BoxEnergy--;
	Owner->Net_FixRoleValue( EM_RoleValue_BoxEnergy , Owner->PlayerBaseData->BoxEnergy );

	Owner->Log_ItemTrade	( Owner->DBID() , -1 , EM_ActionType_MagicBox , MagicStone , "" );
	Owner->PlayerBaseData->Body.ItemMagicBox[0] = MagicStone;

	Owner->Net_FixItemInfo_Body( _MAX_BODY_EX_COUNT_ );
	Owner->Net_FixItemInfo_Body( _MAX_BODY_EX_COUNT_ + 1 );
	Owner->Net_FixItemInfo_Body( _MAX_BODY_EX_COUNT_ + 2 );

	Global::Log_ItemTrade( Owner->DBID() , -1 , Owner->DBID() , Owner->X() , Owner->Z() , EM_ActionType_Item_DrawAttribute , Item , "" );
	SC_MagicBoxAttrDrawOffResult( Owner->GUID() , true );
}