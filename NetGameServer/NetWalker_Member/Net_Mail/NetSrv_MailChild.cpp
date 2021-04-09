#include "../NetWakerGSrvInc.h"
#include "NetSrv_MailChild.h"
#include <string.h>
#include "MD5/Mymd5.h"
//-----------------------------------------------------------------
vector< SendMailTempStruct >  NetSrv_MailChild::_SendMailWaitVector;
vector< int >				  NetSrv_MailChild::_SemMailUnUseID;
//-----------------------------------------------------------------
bool    NetSrv_MailChild::Init()
{
    NetSrv_Mail::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_MailChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_MailChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_Mail::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
void NetSrv_MailChild::RC_SendMail	( BaseItemObject* OwnerObj , MailBaseInfoStruct& MailBaseInfo , int ItemPos[10] , char* Content , char* Password )
{

	RoleDataEx* Owner = OwnerObj->Role();

	if( Ini()->IsBattleWorld == true )
	{
		SC_CloseMail( Owner->GUID() );
		return;
	}

	if( MailBaseInfo.Money > 10000000 )
	{
		Print( LV3, "MailMoney", "Role: %s Money: %d", Global::GetRoleName_ASCII( Owner ).c_str(), Owner->PlayerBaseData->Body.Money );
		
		for( int i=0; i<sizeof( MailBaseInfoStruct ); i++ )
		{
			Print( LV3,"MailMoney", "M[%d] = %d", i, (byte*)&MailBaseInfo );
		}

		for( int i=0; i < 10; i++ )
		{
			Print( LV3,"MailMoney", "I[%d] = %d", i, ItemPos[i] );
		}

		for( int i=0; i < strlen( Content ); i++ )
		{
			Print( LV3,"MailMoney", "C[%d] = %c", i, Content[i] );
		}

		for( int i=0; i < strlen( Password ); i++ )
		{
			Print( LV3,"MailMoney", "P[%d] = %c", i, Password[i] );
		}
	}

	if( Global::Ini()->IsDisableMailMoney == 1 && MailBaseInfo.Money != 0 )
	{
		SC_CloseMail( Owner->GUID() );
		return;
	}

	if( Global::Ini()->IsDisableMailAccountMoney == 1 && MailBaseInfo.Money_Account != 0 )
	{
		SC_CloseMail( Owner->GUID() );
		return;
	}


	if(		Owner->TempData.Sys.OnChangeZone 
		||	Owner->TempData.Sys.OnChangeWorld )
	{
		return;
	}

	if(		Owner->CheckOpenMenu( EM_RoleOpenMenuType_Mail) == false 	 )
	{
		SC_CloseMail( Owner->GUID() );
		return;
	}

	if( Owner->BaseData.SysFlag.DisableTrade != false )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
		SC_CloseMail( Owner->GUID() );
		return;
	}

	if( Owner->MaxJobLv() < Global::Ini()->MailLv )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Mail_RoleLvError );
		return;
	}


	if( Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_Mail ] > 30 )
	{
		SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_SendTooOften , MailBaseInfo.OrgTargetName.Begin() );
		return;
	}

	//只準一種金錢交易
//	if( MailBaseInfo.Money != 0 && MailBaseInfo.Money_Account != 0 )
//	{
//		SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_MoneyError );
//		return;
//	}

	if( Owner->PlayerBaseData->TimeFlag[EM_TimeFlagEffect_MailPaper] == -1 || MailBaseInfo.PaperType == EM_MailBackGroundType_System )
		MailBaseInfo.PaperType = EM_MailBackGroundType_Normal;

	MailBaseInfo.OrgSendName = OwnerObj->Role()->RoleName();
	
	if( stricmp( MailBaseInfo.OrgSendName.Begin() , MailBaseInfo.OrgTargetName.Begin() ) == 0 )
	{//不可以送給自己
		SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_TargetNotFind , MailBaseInfo.OrgTargetName.Begin() );
		return;
	}

	for( int i = 0 ; i < 10 ; i++ )
	{
		ItemFieldStruct* BodyItem = Owner->GetBodyItem( ItemPos[i] );
		if( BodyItem == NULL )
		{
			MailBaseInfo.Item[i].Init();
		}
		else if( *BodyItem != MailBaseInfo.Item[i] || BodyItem->Mode.Trade == false )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_ItemError , MailBaseInfo.OrgTargetName.Begin() );
			return;
		}
		else if( BodyItem->Mode.ItemLock != false )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_ItemLock , MailBaseInfo.OrgTargetName.Begin() );
			return;
		}
		else
		{

			MailBaseInfo.Item[i] = *BodyItem;
		}

	}

/*
	if( MailBaseInfo.Money != 0 )
	{
		SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_MoneyError , MailBaseInfo.OrgTargetName.Begin() );
		SC_CloseMail( Owner->GUID() );
		return;
	}

	if( MailBaseInfo.Money_Account != 0 )
	{
		SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_AccountMoneyError , MailBaseInfo.OrgTargetName.Begin() );
		SC_CloseMail( Owner->GUID() );
		return;
	}

	if( MailBaseInfo.Money_Bonus < 0 )
	{
		SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_BonusMoneyError , MailBaseInfo.OrgTargetName.Begin() );
		return;
	}
*/
	if( MailBaseInfo.Money_Account > Owner->OkAccountMoney() )
	{
		SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_AccountMoneyError , MailBaseInfo.OrgTargetName.Begin() );
		NetSrv_Other::SC_AccountMoneyTranError( Owner->GUID() );		
		return;
	}


	if( MailBaseInfo.IsSendMoney != false )
	{
		if( Owner->PlayerBaseData->Body.Money < MailBaseInfo.Money )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_MoneyError , MailBaseInfo.OrgTargetName.Begin() );
			return;
		}
		if( Owner->PlayerBaseData->Body.Money_Account < MailBaseInfo.Money_Account )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_AccountMoneyError , MailBaseInfo.OrgTargetName.Begin() );
			return;
		}
		if( Owner->PlayerBaseData->Money_Bonus < MailBaseInfo.Money_Bonus )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_BonusMoneyError , MailBaseInfo.OrgTargetName.Begin() );
			return;
		}

		if( MailBaseInfo.Money_Account > 0 )
		{
			//檢查二次密碼
			MyMD5Class Md5;
			Md5.ComputeStringHash( Password );
			if( stricmp( Md5.GetMd5Str() , Owner->PlayerBaseData->Password.Begin() ) != 0 )
			{
				Owner->Net_GameMsgEvent( EM_GameMessageEvent_SecondPasswordError );
				SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_PasswordError , MailBaseInfo.OrgTargetName.Begin() );
				return;
			}
		}
	}
	else //if( MailBaseInfo.IsSendMoney == false  )
	{
		if( MailBaseInfo.Money == 0 && MailBaseInfo.Money_Account == 0 && MailBaseInfo.Money_Bonus == 0 )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_MoneyError , MailBaseInfo.OrgTargetName.Begin() );
			return;
		}

		if(  MailBaseInfo.Money < 0 )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_MoneyError , MailBaseInfo.OrgTargetName.Begin() );
			return;
		}

		if(  MailBaseInfo.Money_Account < 0 )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_AccountMoneyError , MailBaseInfo.OrgTargetName.Begin() );
			return;
		}

		if(  MailBaseInfo.Money_Bonus < 0 )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_BonusMoneyError , MailBaseInfo.OrgTargetName.Begin() );
			return;
		}


		bool IsFindItem = false;
		for( int i = 0 ; i < 10 ; i++ )
		{
			if( MailBaseInfo.Item[i].IsEmpty() == false )
			{
				IsFindItem = true;
				break;
			}
		}

		if( IsFindItem == false )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_ItemError , MailBaseInfo.OrgTargetName.Begin() );
			return;
		}		
	}

	MailBaseInfo.IsRead = false;
	MailBaseInfo.IsSendBack = false;
	MailBaseInfo.IsSystem = false;
	MailBaseInfo.IsGift = false;

	//
	int UnUseID;
	if( _SemMailUnUseID.size() == 0 )
	{
		UnUseID = int( _SendMailWaitVector.size() );
		SendMailTempStruct Temp;
		_SendMailWaitVector.push_back( Temp );
	}
	else
	{
		UnUseID = _SemMailUnUseID.back();
		_SemMailUnUseID.pop_back();
	}
	SendMailTempStruct& TempMailInfo = _SendMailWaitVector[ UnUseID ];

	TempMailInfo.Content = Content;
	memcpy( TempMailInfo.ItemPos , ItemPos , sizeof(TempMailInfo.ItemPos) );
	TempMailInfo.MailBaseInfo = MailBaseInfo;
	TempMailInfo.SenderGUID = OwnerObj->GUID();
	TempMailInfo.IsDestroy = false;

	SD_CheckSendMail( OwnerObj->Role()->DBID() , (char*)MailBaseInfo.OrgTargetName.Begin() , UnUseID );

}
void NetSrv_MailChild::RC_MailList	( BaseItemObject* OwnerObj )
{
	RoleDataEx* Owner = OwnerObj->Role();
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_Mail) == false )
	{
		SC_CloseMail( Owner->GUID() );
		return;
	}
	SD_MailList( Owner->DBID() , Owner->SockID() , Owner->ProxyID() );
}
void NetSrv_MailChild::RC_SendBack	( BaseItemObject* OwnerObj , int MailDBID )
{
	RoleDataEx* Owner = OwnerObj->Role();
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_Mail) == false )
	{
		SC_CloseMail( Owner->GUID() );
		return;
	}
	SD_SendBack( Owner->DBID() , MailDBID );
}
void NetSrv_MailChild::RC_DelMail	( BaseItemObject* OwnerObj , int MailDBID )
{
	RoleDataEx* Owner = OwnerObj->Role();
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_Mail) == false )
	{
		SC_CloseMail( Owner->GUID() );
		return;
	}
	SD_DelMail( Owner->DBID() , MailDBID );
}
void NetSrv_MailChild::RC_ReadMail	( BaseItemObject* OwnerObj , int MailDBID )
{
	RoleDataEx* Owner = OwnerObj->Role();
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_Mail) == false )
	{
		SC_CloseMail( Owner->GUID() );
		return;
	}
	SD_ReadMail( Owner->DBID() , MailDBID );
}
void NetSrv_MailChild::RC_GetItem	( BaseItemObject* OwnerObj , int MailDBID , int Money , int Money_Account , int Money_Bonus , const char* Password )
{

	RoleDataEx* Owner = OwnerObj->Role();

	if(		Ini()->IsBattleWorld == true
		||	Owner->TempData.Sys.OnChangeWorld == true	
		||	Owner->TempData.Sys.OnChangeZone == true	)
	{
		SC_CloseMail( Owner->GUID() );
		return;
	}

	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_Mail) == false )
	{
		SC_CloseMail( Owner->GUID() );
		return;
	}

	
	if( Money < 0 && Money <  Owner->PlayerBaseData->Body.Money * -1 )
	{
		//金錢不足
		SC_GetItemResult( Owner->GUID() , MailDBID , false );
		return;
	}
	if( Money_Account < 0 && Money_Account < Owner->PlayerBaseData->Body.Money_Account * -1 )
	{
		//金錢不足
		SC_GetItemResult( Owner->GUID() , MailDBID , false );
		return;
	}
	if( Money_Bonus < 0 && Money_Bonus < Owner->PlayerBaseData->Money_Bonus * -1 )
	{
		//金錢不足
		SC_GetItemResult( Owner->GUID() , MailDBID , false );
		return;
	}

	if( Money_Account < 0 )
	{
		//檢查二次密碼
		MyMD5Class Md5;
		Md5.ComputeStringHash( Password );
		if( stricmp( Md5.GetMd5Str() , Owner->PlayerBaseData->Password.Begin() ) != 0 )
		{
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_SecondPasswordError );
			SC_GetItemResult( Owner->GUID() , MailDBID , false );
			return;
		}
	}

	if(		Money >0 && Owner->PlayerBaseData->Body.Money + Money > _MAX_MONEY_COUNT_  )		
	{
		SC_GetItemResult( Owner->GUID() , MailDBID , false );
		return;
	}

	if(		Money_Account >0 && Owner->PlayerBaseData->Body.Money_Account + Money_Account > _MAX_MONEY_COUNT_ )
	{
		SC_GetItemResult( Owner->GUID() , MailDBID , false );
		return;
	}		

	if(		Money_Bonus >0 && Owner->PlayerBaseData->Money_Bonus + Money_Bonus > _MAX_BONUSMONEY_COUNT_ )
	{
		SC_GetItemResult( Owner->GUID() , MailDBID , false );
		return;
	}		

	Owner->TempData.IsDisabledChangeZone = true;
	SD_CheckGetItemRequest( Owner->DBID() , MailDBID , Money , Money_Account , Money_Bonus );
/*
	if( Money < 0 )
		Owner->AddBodyMoney( Money , NULL , EM_ActionType_FromMail);

	if( Money_Account < 0 )
	{
		Owner->AddBodyMoney_Account( Money_Account , NULL , EM_ActionType_FromMail);
		//檢查二次密碼
		MyMD5Class Md5;
		Md5.ComputeStringHash( Password );
		if( stricmp( Md5.GetMd5Str() , Owner->PlayerBaseData->Password.Begin() ) != 0 )
		{
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_SecondPasswordError );
			SC_GetItemResult( Owner->GUID() , MailDBID , false );
			return;
		}

	}

	SD_GetItem( Owner->DBID() , MailDBID , Money , Money_Account );
*/
}

void NetSrv_MailChild::RD_SendMailResult	( BaseItemObject* OwnerObj , bool IsSendMoney , bool IsSystem 
						 , int Money , int Money_Account , int Money_Bonus , SendMailResultEnum Result , const char* TargetName  )
{
	RoleDataEx* Owner = OwnerObj->Role();

	if( Result != EM_SendMailResult_OK )
	{	//退還金錢與物品
		if( IsSendMoney != false )
		{
			Owner->AddBodyMoney			( Money , NULL , EM_ActionType_ToMailError , false );
			Owner->AddBodyMoney_Account	( Money_Account , NULL , EM_ActionType_ToMailError , false );
			Owner->AddMoney_Bonus		( Money_Bonus , NULL , EM_ActionType_ToMailError );
		}
	}
	else
	{
		Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_Mail ] += (1.0f/2.0f);
	}
	if( IsSystem != true )
		SC_SendMailResult( Owner->GUID() , Result , TargetName );
}
void NetSrv_MailChild::RD_GetItemResult	( BaseItemObject* OwnerObj , int DBID , int OrgSenderDBID , int MailDBID , int Money , int Money_Account , int Money_Bonus , ItemFieldStruct Item[10] , bool Result  )
{
	RoleDataEx* Owner = OwnerObj->Role();

	Owner->TempData.IsDisabledChangeZone = false;

	if( Result != false )
	{
		if(		Money >0 
			&& Owner->AddBodyMoney( Money , OrgSenderDBID , -1 , EM_ActionType_FromMail , false ) == false )
		{
			Print(LV5 , "RD_GetItemResult" ,"Result != false AddBodyMoney Failed Money=%d OwnerDBID=%d" , Money , OwnerObj->Role()->DBID() );
			Owner->PlayerBaseData->Body.Money = _MAX_MONEY_COUNT_;
		}

		if(		Money_Account >0 
			&& Owner->AddBodyMoney_Account( Money_Account , OrgSenderDBID , -1 , EM_ActionType_FromMail , false ) == false )
		{
			Print(LV5 , "RD_GetItemResult" ,"Result != false AddBodyMoney_Account Failed Money_Account=%d OwnerDBID=%d" , Money_Account , OwnerObj->Role()->DBID() );
//			Owner->PlayerBaseData->Body.Money_Account = _MAX_MONEY_COUNT_;
		}		

		if(		Money_Bonus >0 
			&& Owner->AddMoney_Bonus( Money_Bonus , Owner , EM_ActionType_FromMail ) == false )
		{
			Print(LV5 , "RD_GetItemResult" ,"Result != false AddMoney_Bonus Failed Money_Account=%d OwnerDBID=%d" , Money_Bonus , OwnerObj->Role()->DBID() );
//			Owner->PlayerBaseData->Money_Bonus = _MAX_BONUSMONEY_COUNT_;
		}		

		//Owner->GiveItem( Item , EM_ActionType_FromMail , NULL );
		for( int i = 0 ; i < 10 ; i++ )
		{
			if( Item[i].IsEmpty() != false )
				continue;

			GameObjDbStructEx* ItemObjDB = g_ObjectData->GetObj( Item[i].OrgObjID );
			if( ItemObjDB == NULL )
				continue;
			if( Item[i].Count > ItemObjDB->MaxHeap )
				Item[i].Count = ItemObjDB->MaxHeap;

			Owner->GiveItem( Item[i] , EM_ActionType_FromMail , OrgSenderDBID , -1 , "" );
		}
		
	}
	else
	{
		if( Money < 0 )
		{
			//如果錯誤 則把之前扣除的金錢退還
			if( Owner->AddBodyMoney( Money * -1 , NULL , EM_ActionType_ToMailError , false ) == false )
			{
				Print(LV5 , "RD_GetItemResult" ,"Result == false AddBodyMoney Failed Money=%d OwnerDBID=%d" , Money , OwnerObj->Role()->DBID() );
//				Owner->PlayerBaseData->Body.Money = _MAX_MONEY_COUNT_;
			};
		}
		if( Money_Account < 0 )
		{
			//如果錯誤 則把之前扣除的金錢退還
			if( Owner->AddBodyMoney_Account( Money_Account * -1 , NULL , EM_ActionType_ToMailError , false ) == false )
			{
				Print(LV5 , "RD_GetItemResult" ,"Result == false AddBodyMoney Failed Money=%d OwnerDBID=%d" , Money , OwnerObj->Role()->DBID() );
//				Owner->PlayerBaseData->Body.Money_Account = _MAX_MONEY_COUNT_;
			};
		}
		if( Money_Bonus < 0 )
		{
			//如果錯誤 則把之前扣除的金錢退還
			if( Owner->AddMoney_Bonus( Money_Bonus * -1 , NULL , EM_ActionType_ToMailError ) == false )
			{
				Print(LV5 , "RD_GetItemResult" ,"Result == false AddBodyMoney Failed Money=%d OwnerDBID=%d" , Money , OwnerObj->Role()->DBID() );
				Owner->PlayerBaseData->Money_Bonus = _MAX_BONUSMONEY_COUNT_;
			};
		}
	}
	SC_GetItemResult( Owner->GUID() , MailDBID , Result );

}

void NetSrv_MailChild::RC_CloseMail	( BaseItemObject* OwnerObj )
{
	RoleDataEx* Role = OwnerObj->Role();
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_Mail) == false )
		return;

	Role->ClsOpenMenu();			
}

void NetSrv_MailChild::RD_CheckSendMailResult( int PlayerDBID , char* TargetName , int TargetDBID , int MapKey , bool Result , int ErrorCode )
{

	if( _SendMailWaitVector.size() <= (unsigned)MapKey )
	{
		Print( LV4 , "RD_CheckSendMailResult" , "_SendMailWaitVector.size() <= MapKey" );
		return;
	}

	SendMailTempStruct TempMailInfo = _SendMailWaitVector[ MapKey ];


	if( TempMailInfo.IsDestroy != false )
	{
		//資料有問題
		Print( LV4 , "RD_CheckSendMailResult" , "TempMailInfo.IsDestroy == false" );
		return;
	}

	_SemMailUnUseID.push_back( MapKey );
	_SendMailWaitVector[ MapKey ].IsDestroy = true;


	//////////////////////////////////////////////////////////////////////////
	BaseItemObject* OwnerObj = BaseItemObject::GetObjByDBID( PlayerDBID );
	if( OwnerObj == NULL )
		return;
	RoleDataEx* Owner = OwnerObj->Role();

	//角色換區中
	if(		Owner->TempData.Sys.OnChangeZone 
		||	Owner->TempData.Sys.OnChangeWorld	)
		return;

	if( Owner->SockID() == false )
		return;

	if( Result == false )
	{
		SC_SendMailResult( OwnerObj->GUID() , (SendMailResultEnum)ErrorCode , TargetName );
		return;
	}


	MailBaseInfoStruct& MailBaseInfo = TempMailInfo.MailBaseInfo;

	for( int i = 0 ; i < 10 ; i++ )
	{
		ItemFieldStruct* BodyItem = Owner->GetBodyItem( TempMailInfo.ItemPos[i] );
		if( BodyItem != NULL && *BodyItem != MailBaseInfo.Item[i] )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_ItemError , TargetName );
			Print( LV4 , "RD_CheckSendMailResult" , "*BodyItem != MailBaseInfo.Item" );
			return;
		}
	}

	if(  MailBaseInfo.Money < 0 )
	{
		SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_MoneyError , TargetName);
		return;
	}

	if(  MailBaseInfo.Money_Account < 0 )
	{
		SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_AccountMoneyError , TargetName);
		return;
	}

	if(  MailBaseInfo.Money_Bonus < 0 )
	{
		SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_BonusMoneyError , TargetName);
		return;
	}

	if( MailBaseInfo.IsSendMoney != false )
	{
		if(		Owner->PlayerBaseData->Body.Money < MailBaseInfo.Money )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_MoneyError , TargetName );
			return;
		}
		if(		Owner->PlayerBaseData->Body.Money_Account < MailBaseInfo.Money_Account )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_AccountMoneyError , TargetName);
			return;
		}
		if( Owner->PlayerBaseData->Money_Bonus < MailBaseInfo.Money_Bonus )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_BonusMoneyError , TargetName);
			return;
		}

		Owner->AddMoney_Bonus		( MailBaseInfo.Money_Bonus * -1	  , NULL , EM_ActionType_ToMail );
		Owner->AddBodyMoney			( MailBaseInfo.Money * -1		  , TargetDBID , -1 , EM_ActionType_ToMail , false );
		Owner->AddBodyMoney_Account	( MailBaseInfo.Money_Account * -1 , TargetDBID , -1 , EM_ActionType_ToMail , false );

	}
	else 
	{
		if( MailBaseInfo.Money == 0 && MailBaseInfo.Money_Account == 0 )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_MoneyError , TargetName);
			return;
		}

		/*if( MailBaseInfo.Item.IsEmpty() != false )
		{
			SC_SendMailResult( OwnerObj->GUID() , EM_SendMailResult_ItemError );
			return;
		}*/		
	}

	for( int i = 0 ; i < 10 ; i++ )
	{
		ItemFieldStruct* BodyItem = Owner->GetBodyItem( TempMailInfo.ItemPos[i] );
		if( BodyItem != NULL )
		{
			//刪除物品
			Owner->Log_ItemTrade( TargetDBID , -1 , EM_ActionType_ToMail , *BodyItem , "" );
			BodyItem->Init();
			Owner->Net_FixItemInfo_Body( TempMailInfo.ItemPos[i] );
		}

	}


	SD_SendMail( Owner->DBID() , (char*)TempMailInfo.Content.c_str() , MailBaseInfo );
	

}

void NetSrv_MailChild::RD_CheckGetItemResult( int DBID , int TargetDBID , int MailDBID , int Money , int Money_Account , int Money_Bonus , bool Result )
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObjByDBID( DBID );
	if( OwnerObj == NULL )
		return;
	RoleDataEx* Owner = OwnerObj->Role();

	if(		Owner->TempData.Sys.OnChangeWorld 
		||	Owner->TempData.Sys.OnChangeZone	)
		return;

	if( Money < 0 && Money < Owner->PlayerBaseData->Body.Money * -1 )
	{
		//金錢不足
		SC_GetItemResult( Owner->GUID() , MailDBID , false );
		return;
	}
	if( Money_Account < 0 && Money_Account < Owner->PlayerBaseData->Body.Money_Account * -1 )
	{
		//金錢不足
		SC_GetItemResult( Owner->GUID() , MailDBID , false );
		return;
	}
	if( Money_Bonus < 0 && Money_Bonus < Owner->PlayerBaseData->Money_Bonus * -1 )
	{
		//金錢不足
		SC_GetItemResult( Owner->GUID() , MailDBID , false );
		return;
	}
	//身體欄位檢查
	//計算空欄位數量
	int EmptyCount = Owner->EmptyPacketCount();

	if( Owner->PlayerBaseData->ItemTemp.Size() > EmptyCount || Owner->PlayerBaseData->ItemTemp.Size() > 5 )
	{
		//Role->Msg( "東西過多無法買入" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Mail_BagFull );
		SC_GetItemResult( Owner->GUID() , MailDBID , false );
		return;
	}

	
	if( Money < 0 )
		Owner->AddBodyMoney( Money , TargetDBID , -1  , EM_ActionType_FromMail , false );

	if( Money_Account < 0 )
		Owner->AddBodyMoney_Account( Money_Account , TargetDBID , -1 , EM_ActionType_FromMail , false );

	if( Money_Bonus < 0 )
		Owner->AddMoney_Bonus( Money_Bonus , NULL , EM_ActionType_FromMail);

	SD_GetItem( Owner->DBID() , MailDBID , Money , Money_Account , Money_Bonus );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_MailChild::SendSysMail		( const char* SendToName , const char* Title , const char* Content )
{
	MailBaseInfoStruct MailBaseInfo;

	memset( &MailBaseInfo , 0 , sizeof(MailBaseInfo) );
	MailBaseInfo.IsSystem = true;
	MailBaseInfo.LiveTime = 60*24*30;
	MailBaseInfo.OrgTargetName = SendToName;
	MailBaseInfo.Title = Title;

	SD_SendMail( -1 , (char*)Content , MailBaseInfo );
}
void NetSrv_MailChild::RC_SendMailList( BaseItemObject* Obj , int TargetCount , StaticString <_MAX_NAMERECORD_SIZE_>	OrgTargetName[20] 
						, const char* Title , MailBackGroundTypeENUM PaperType , const char* Content )
{
	RoleDataEx* Owner = Obj->Role();

	if( Owner->TempData.Sys.OnChangeZone != false || TargetCount > 20 )
	{
		return;
	}

	if(		Owner->CheckOpenMenu( EM_RoleOpenMenuType_Mail) == false 	 )
	{
		SC_CloseMail( Owner->GUID() );
		return;
	}

	if( Owner->BaseData.SysFlag.DisableTrade != false )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
		SC_CloseMail( Owner->GUID() );
		return;
	}

	int		MailUnit = g_ObjectData->GetMoneyKeyValue( "MailUnit" , 0 );
	
	if( MailUnit < 0 )
		MailUnit = 0;

	if (Global::AccountMoneyOperable() == false)
	{
		return;
	}

	if( Owner->AddBodyMoney_Account( MailUnit*-1 , Owner , EM_ActionType_SellMailGroup , false ) == false )
	{
		return;
	}

	MailBaseInfoStruct MailBaseInfo;
	MailBaseInfo.Init();
	MailBaseInfo.Money = 0;
	MailBaseInfo.Money_Account = 0;
	MailBaseInfo.Money_Bonus = 0;

	MailBaseInfo.IsSendMoney = true;			//true 寄送金錢 false 獲到付款
	MailBaseInfo.IsSystem = false;				//是否為系統信件
	MailBaseInfo.IsRead = false;
	MailBaseInfo.IsSendBack = false;			//是否為退回信件

	MailBaseInfo.OrgSendName = Owner->RoleName();

	MailBaseInfo.Title = Title;;
	MailBaseInfo.LiveTime = 0;
	MailBaseInfo.PaperType = PaperType;

	if( Owner->PlayerBaseData->TimeFlag[EM_TimeFlagEffect_MailPaper] == -1 || MailBaseInfo.PaperType == EM_MailBackGroundType_System )
		MailBaseInfo.PaperType = EM_MailBackGroundType_Normal;

	for( int i = 0 ; i < TargetCount ; i++ )
	{
		MailBaseInfo.OrgTargetName = OrgTargetName[i];

		if( stricmp( MailBaseInfo.OrgSendName.Begin() , MailBaseInfo.OrgTargetName.Begin() ) == 0 )
		{//不可以送給自己
			SC_SendMailResult( Owner->GUID() , EM_SendMailResult_TargetNotFind , MailBaseInfo.OrgTargetName.Begin() );
			return;
		}

		//
		int UnUseID;
		if( _SemMailUnUseID.size() == 0 )
		{
			UnUseID = int( _SendMailWaitVector.size() );
			SendMailTempStruct Temp;
			_SendMailWaitVector.push_back( Temp );
		}
		else
		{
			UnUseID = _SemMailUnUseID.back();
			_SemMailUnUseID.pop_back();
		}
		SendMailTempStruct& TempMailInfo = _SendMailWaitVector[ UnUseID ];

		memset( TempMailInfo.ItemPos  , -1 , sizeof( TempMailInfo.ItemPos ) );
		TempMailInfo.Content = Content;
		TempMailInfo.MailBaseInfo = MailBaseInfo;
		TempMailInfo.SenderGUID = Owner->GUID();
		TempMailInfo.IsDestroy = false;
		TempMailInfo.MailBaseInfo.PaperType = EM_MailBackGroundType_Gift;

		SD_CheckSendMail( Owner->DBID() , (char*)MailBaseInfo.OrgTargetName.Begin() , UnUseID );
	}

	Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_Mail ] -= ( TargetCount/2.0f );

}