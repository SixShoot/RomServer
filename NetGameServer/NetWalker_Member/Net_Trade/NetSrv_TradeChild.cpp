#include "../NetWakerGSrvInc.h"
#include "NetSrv_TradeChild.h"
#include "MD5/Mymd5.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_TradeChild::Init()
{
    NetSrv_Trade::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_TradeChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_TradeChild::Release()
{
    if( This == NULL )
        return false;

	NetSrv_Trade::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
//�n�D���
void NetSrv_TradeChild::R_RequestTrade	( BaseItemObject*	OwnerObj , int TargetID )
{
	if( Global::Ini()->IsDisableTrade != false )
	{
		S_RequestTradeError( OwnerObj->GUID() );
		return;
	}
	
    char	Buf[256];
    //�q���ؼХ��
    RoleDataEx*         Owner  = OwnerObj->Role();	
    BaseItemObject*     TargetObj = BaseItemObject::GetObj( TargetID );
    if( TargetObj == NULL )
	{
		S_RequestTradeError( OwnerObj->GUID() );
        return;
	}

    RoleDataEx *Target = TargetObj->Role();

    if( Owner->IsTrade() || Target->IsTrade() ) 
    {
        sprintf( Buf , "������� %s ���b���" , Utf8ToChar( Owner->BaseData.RoleName.Begin() ).c_str() );
        Target->Msg( Buf );
	    return;
    }

	if( Owner->BaseData.SysFlag.DisableTrade != false )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
		return;
	}

	if( Target->BaseData.SysFlag.DisableTrade != false )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Item_TargetDisableTrade );
		return;
	}
	
	//�H��������
	if( Owner->PlayerBaseData->VipMember.UntrustFlag.DisablePlayerTrade || Target->PlayerBaseData->VipMember.UntrustFlag.DisablePlayerTrade )
	{
		//Owner->Msg( "�H�����a�������_�L�k�P���a���" );
		if( Owner->PlayerBaseData->VipMember.UntrustFlag.DisablePlayerTrade )
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Role_Untrust);
		else
			S_CancelTradeRequest( Owner->GUID() , Target->GUID() );
		return;
	}

	if( Owner->TempData.Trade.TargetID != -1  )
	{
		S_CancelTradeRequest( Owner->TempData.Trade.TargetID , Owner->GUID() );
	}

	Owner->TempData.Trade.TargetID   = TargetID;
	Owner->TempData.Trade.TargetDBID = Target->DBID();

    S_ClientRequestTrade( TargetID , Owner->GUID() );


    sprintf( Buf , "%s �n�D���" , Utf8ToChar( Owner->BaseData.RoleName.Begin() ).c_str() );
    Target->Msg_Utf8( Buf );

    sprintf( Buf , "%s ����ӽ�" , Utf8ToChar( Target->BaseData.RoleName.Begin() ).c_str() );
    Owner->Msg_Utf8( Buf );

}
//---------------------------------------------------------------------------
//�P�N���
void NetSrv_TradeChild::R_AgreeTrade	( BaseItemObject*	OwnerObj , int TargetID , bool Result , int ClientResult )
{
    RoleDataEx*         Owner  = OwnerObj->Role();	
    BaseItemObject*     TargetObj = BaseItemObject::GetObj( TargetID );
    if(		TargetObj == NULL 
		||	TargetObj->Role()->TempData.Trade.TargetID != Owner->GUID()
		||	TargetObj->Role()->TempData.Trade.TargetDBID != Owner->DBID() )
	{
		if( Result != false )
			S_DisagreeTrade( OwnerObj->GUID() , ClientResult );
        return;
	}
    RoleDataEx *Target = TargetObj->Role();

	if(		Result == false )
	{
		Target->TempData.Trade.Init();
		S_DisagreeTrade( TargetObj->GUID() , ClientResult );
		return;
	}

    if(		Target->TempData.Trade.TargetID != Owner->GUID() 
		||	Target->TempData.Trade.TargetDBID != Owner->DBID() )
        return;

    Owner->TempData.Trade.TargetID = TargetID;
	Owner->TempData.Trade.TargetDBID = Target->DBID();

    Owner->TempData.Attr.Action.Trade = true;
    Target->TempData.Attr.Action.Trade = true;

    S_OpenTrade( Owner->GUID()  , Target->GUID() , Target->BaseData.RoleName.Begin() );
    S_OpenTrade( Target->GUID() , Owner->GUID()	 , Owner->BaseData.RoleName.Begin() );
}
//---------------------------------------------------------------------------
//�񪫫~		
void NetSrv_TradeChild::R_PutItem		( BaseItemObject*	OwnerObj , int Pos , int ItemOrgPos ,  ItemFieldStruct *CliItem )
{

    RoleDataEx*         Owner  = OwnerObj->Role();	
    BaseItemObject*     TargetObj = BaseItemObject::GetObj( Owner->TempData.Trade.TargetID );
    if( TargetObj == NULL )
        return;

    RoleDataEx *Target = TargetObj->Role();

	ItemFieldStruct* SrvItem = Owner->GetBodyItem( ItemOrgPos );
	if( SrvItem == NULL || *SrvItem != *CliItem )
	{
		if( SrvItem != NULL )
		{
			NetSrv_Item::FixItemInfo_Body( OwnerObj->GUID() , *SrvItem , ItemOrgPos );	
		}		
		S_PutItemFalse( Owner->GUID() );
		//Owner->Msg( "������~�����D" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_ItemError );
		return;
	}
/*
	if( Pos < _MAX_BODY_BEGIN_POS_ )
	{
		NetSrv_Item::FixItemInfo_Body( OwnerObj->GUID() , *SrvItem , ItemOrgPos );	
		S_PutItemFalse( Owner->GUID() );
		return;
	}
	*/

	//�ˬd�O�_�i�H���
	if( SrvItem->Mode.Trade == false || SrvItem->Mode.ItemLock != false )
	{
		NetSrv_Item::FixItemInfo_Body( OwnerObj->GUID() , *SrvItem , ItemOrgPos );	
		S_PutItemFalse( Owner->GUID() );
		//Owner->Msg( "�����~���i���" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_CanNotTrade );
		return;
	}


    if( !Target->IsTrade() || !Owner->IsTrade() )
    {
		NetSrv_Item::FixItemInfo_Body( OwnerObj->GUID() , *SrvItem , ItemOrgPos );	
        S_PutItemFalse( Owner->GUID() );
        return;
    }

    if( Target->TempData.Trade.TargetID != Owner->GUID() )
    {
		NetSrv_Item::FixItemInfo_Body( OwnerObj->GUID() , *SrvItem , ItemOrgPos );	
        S_PutItemFalse( Owner->GUID() );
        return;
    }

	GameObjDbStructEx*	PutItemDB = Global::GetObjDB( SrvItem->OrgObjID );
    if( PutItemDB == NULL || Owner->TempData.Trade.SelectOK != false )
    {
		NetSrv_Item::FixItemInfo_Body( OwnerObj->GUID() , *SrvItem , ItemOrgPos );	
        S_PutItemFalse( Owner->GUID() );
        //Owner->Msg("����T�w�ᤣ�i���");
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_CanNotModify );
        return;
    }

	int		TargetTradeCount = Target->CalTradeItemCount();
	int		OwnerTradeCount  = Owner->CalTradeItemCount();

	if( PutItemDB->IsItem()&& PutItemDB->Item.ItemType == EM_ItemType_Relation )
	{
		if(		TargetTradeCount != 0 
			||	OwnerTradeCount != 0 		)
		{
			NetSrv_Item::FixItemInfo_Body( OwnerObj->GUID() , *SrvItem , ItemOrgPos );	
			S_PutItemFalse( Owner->GUID() );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_RelationItemError );
			return;
		}

	}


	if( EM_ItemState_NONE != Pos )
	{
		if( Pos < EM_TRADE_ITEM1 || Pos > EM_TRADE_ITEM8 )
		{
			NetSrv_Item::FixItemInfo_Body( OwnerObj->GUID() , *SrvItem , ItemOrgPos );	
			S_PutItemFalse( Owner->GUID() );
			//Owner->Msg( "�����m��m�����D" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_PosError );
			return;
		}
		//�j�M�O�_�����Ъ���m
		for( int i = 0 ; i < Owner->PlayerBaseData->Body.Count ; i++ )
		{
			ItemFieldStruct& Item = Owner->PlayerBaseData->Body.Item[i];
			if( Item.Pos == Pos && ItemOrgPos != i )
			{
				//Item.Pos = EM_ItemState_NONE;
				Item.Pos = SrvItem->Pos;
				NetSrv_Item::FixItemInfo_Body( OwnerObj->GUID() , Item , i );	
				if( SrvItem->Pos != EM_ItemState_NONE )
					S_TargetPutItem( Target->GUID() , SrvItem->Pos , &Item );	
				break;
			}
		}
	}

	if( SrvItem->Pos != EM_ItemState_NONE  )
	{
		if( SrvItem->Pos < EM_TRADE_ITEM1 || SrvItem->Pos > EM_TRADE_ITEM8 )
		{
			NetSrv_Item::FixItemInfo_Body( OwnerObj->GUID() , *SrvItem , ItemOrgPos );	
			S_PutItemFalse( Owner->GUID() );
			//Owner->Msg( "����쥿�b��w��" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_PosOnLock );
			return;
		}
		ItemFieldStruct TempItem;
		S_TargetPutItem( Target->GUID() , SrvItem->Pos , &TempItem );			
	}
    SrvItem->Pos = (ItemState_ENUM)(Pos);

    NetSrv_Item::FixItemInfo_Body( OwnerObj->GUID() , *SrvItem , ItemOrgPos );	
    //Owner->TempData.UpdateInfo.Body = true;


	if( Pos != EM_ItemState_NONE )
		S_TargetPutItem( Target->GUID() , Pos , SrvItem );	

    //���hOK�٦��ק�h����
    if( Target->TempData.Trade.SelectOK != false )
    {
        Target->TempData.Trade.SelectOK = false;
        //�q���ؼШ����T�w
//        S_CancelItemOK( Target->GUID() );
//        S_CancelItemOK( Owner->GUID() );
		S_TradeState( Owner , Target );
    }
    S_PutItemOK( Owner->GUID() );
}
//---------------------------------------------------------------------------
//�񪫪���
void NetSrv_TradeChild::R_PutMoney		( BaseItemObject*	OwnerObj , int Money , int Money_Account )
{
    RoleDataEx*         Owner  = OwnerObj->Role();	
    BaseItemObject*     TargetObj = BaseItemObject::GetObj( Owner->TempData.Trade.TargetID );
    if( TargetObj == NULL )
        return;

	if( Money_Account )
	{
		S_OwnerPutMoney( Owner->GUID() , Owner->TempData.Trade.Money , Owner->TempData.Trade.Money_Account  );
		return;
	}

	//�u��@�ӹ��ȥ��
	if( Money_Account != 0 && Money != 0 )
	{
		//Owner->Msg("����T�w�ᤣ�i���");
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_CanNotModify );
		S_OwnerPutMoney( Owner->GUID() , Owner->TempData.Trade.Money , Owner->TempData.Trade.Money_Account  );
		return;
	}

    RoleDataEx *Target = TargetObj->Role();

    if( Owner->TempData.Trade.SelectOK != false )
    {
        //Owner->Msg("����T�w�ᤣ�i���");
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_CanNotModify );
		S_OwnerPutMoney( Owner->GUID() , Owner->TempData.Trade.Money , Owner->TempData.Trade.Money_Account  );
        return;
    }
    if( Money < 0 )
    {
        //Owner->Msg( "??Moeny ���t" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_MoneyError );
        Print( LV5 , "R_PutMoney" ,"Moeny < 0 ?? RoleName = %d Moeny= %d" , Utf8ToChar( Owner->RoleName() ).c_str() , Money );
		S_OwnerPutMoney( Owner->GUID() , Owner->TempData.Trade.Money , Owner->TempData.Trade.Money_Account );
		return;
    }

	if( Money_Account < 0 )
	{
		//Owner->Msg( "??Money_Account ���t" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_AccountError );
		Print( LV5 , "R_PutMoney" ,"Money_Account < 0 ?? RoleName = %d Money_Account= %d" , Utf8ToChar( Owner->RoleName() ).c_str() , Money_Account );
		S_OwnerPutMoney( Owner->GUID() , Owner->TempData.Trade.Money , Owner->TempData.Trade.Money_Account );
		return;
	}

    if( Money > Owner->PlayerBaseData->Body.Money )
    {
        //Owner->Msg( "��������" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_MoneyNotEnough );
		S_OwnerPutMoney( Owner->GUID() , Owner->TempData.Trade.Money , Owner->TempData.Trade.Money_Account );
        return;
    }

	if( Money_Account > Owner->PlayerBaseData->Body.Money_Account  )
	{
		//Owner->Msg( "�I�Ƥ���" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_AccountNotEnough );
		S_OwnerPutMoney( Owner->GUID() , Owner->TempData.Trade.Money , Owner->TempData.Trade.Money_Account );
		return;
	}

	if( Money_Account > Owner->OkAccountMoney() )
	{
		S_OwnerPutMoney( Owner->GUID() , Owner->TempData.Trade.Money , Owner->TempData.Trade.Money_Account );
		NetSrv_Other::SC_AccountMoneyTranError( Owner->GUID() );		
		return;
	}

	Owner->TempData.Trade.Money = Money;
	Owner->TempData.Trade.Money_Account = Money_Account;

    S_TargetPutMoney( Target->GUID() , Money , Money_Account );
    S_OwnerPutMoney( Owner->GUID() , Money , Money_Account );

    //���hOK�٦��ק�h����
    if( Target->TempData.Trade.SelectOK != false )
    {
        Target->TempData.Trade.SelectOK = false;
        //�q���ؼШ����T�w
//        S_CancelItemOK( Target->GUID() );
//        S_CancelItemOK( Owner->GUID() );
		S_TradeState( Owner , Target );
    }
}
//---------------------------------------------------------------------------
//�T�w
void NetSrv_TradeChild::R_ItemOK		( BaseItemObject*	OwnerObj )
{
    RoleDataEx*         Owner  = OwnerObj->Role();	
    BaseItemObject*     TargetObj = BaseItemObject::GetObj( Owner->TempData.Trade.TargetID );
    if( TargetObj == NULL )
        return;

    RoleDataEx *Target = TargetObj->Role();

	if( Owner->TempData.Trade.SelectOK != false )
		return;

    Owner->TempData.Trade.SelectOK = true;

	S_TradeState( Owner , Target );
	/*
    S_TargetItemOK( Target->GUID() );

    if( Target->TempData.Trade.SelectOK != false )
    {
        S_FinalOKEnable( Owner->GUID());
        S_FinalOKEnable( Target->GUID() );
    }
	*/
}
//---------------------------------------------------------------------------
//�M�w
void NetSrv_TradeChild::R_FinalOK		( BaseItemObject*	OwnerObj , const char* Password )
{
    int	i;
    RoleDataEx*         Owner  = OwnerObj->Role();	
    BaseItemObject*     TargetObj = BaseItemObject::GetObj( Owner->TempData.Trade.TargetID );
    if( TargetObj == NULL )
        return;

    RoleDataEx*         Target  = TargetObj->Role();	

	if( Owner->SockID() == -1 || Target->SockID() == -1 )
		return;

	if( Owner->TempData.Sys.OnChangeZone != false || Target->TempData.Sys.OnChangeZone != false )
	{
		S_FinalOkFailed( Owner->GUID() , EM_FinalOKFailed_Unknow );
		return;
	}

	if( Owner->TempData.Trade.FinalOK != false )
	{
		S_FinalOkFailed( Owner->GUID() , EM_FinalOKFailed_Unknow );
		return;
	}

	if( Owner->TempData.Trade.TargetDBID != Target->DBID() )
	{
		S_FinalOkFailed( Owner->GUID() , EM_FinalOKFailed_Unknow );
		return;
	}

	if(		Owner->TempData.Trade.SelectOK == false 
		||	Target->TempData.Trade.SelectOK == false  )
	{
		S_FinalOkFailed( Owner->GUID() , EM_FinalOKFailed_Unknow );
		return;
	}


	if( Owner->TempData.Trade.Money_Account != 0 )
	{
		MyMD5Class Md5;
		Md5.ComputeStringHash( Password );
		if( stricmp( Md5.GetMd5Str() , Owner->PlayerBaseData->Password.Begin() ) != 0 )
		{
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_SecondPasswordError );
			S_FinalOkFailed( Owner->GUID() , EM_FinalOKFailed_PasswordErr );
			return;
		}

	}


    //�p�G���誺�P�N���	
    if( Target->TempData.Trade.FinalOK != false )
    {
        int		TargetTradeCount = Target->CalTradeItemCount();
        int		OwnerTradeCount  = Owner->CalTradeItemCount();

        int		TargetBodyTradeCount = Target->CalBodyTradeItemCount();
        int		OwnerBodyTradeCount  = Owner->CalBodyTradeItemCount();

        int		TargetEmptyCount = Target->EmptyPacketCount();
        int		OwnerEmptyCount  = Owner->EmptyPacketCount();

        //1�������ƶq�ˬd
        if(		TargetBodyTradeCount + TargetEmptyCount < OwnerTradeCount 
            ||	OwnerBodyTradeCount + OwnerEmptyCount < TargetTradeCount 
			||  Owner->TempData.Trade.Money > Owner->PlayerBaseData->Body.Money 
			||  Target->TempData.Trade.Money > Target->PlayerBaseData->Body.Money 
			||	Owner->CheckAddBodyMoney(Target->TempData.Trade.Money - Owner->TempData.Trade.Money) == false
			||	Target->CheckAddBodyMoney(Owner->TempData.Trade.Money - Target->TempData.Trade.Money) == false	)
        {
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_Error );
            //Owner->Msg( "�������" );
			Target->Net_GameMsgEvent( EM_GameMessageEvent_Trade_Error );
            //Target->Msg( "�������" );
            S_TradeItemFalse( Owner->GUID());
            S_TradeItemFalse( Target->GUID() );

            Target->TempData.Trade.FinalOK = false;
            Target->TempData.Trade.SelectOK= false;
//			Target->PlayerBaseData->Body.Money += Target->TempData.Trade.Money ;
            //Target->AddBodyMoney( Target->TempData.Trade.Money , Owner->Base.Account_ID.Begin() , Owner->RoleName() , EM_ActionType_Tran );
            Target->TempData.Trade.Money = 0;

            Owner->TempData.Trade.FinalOK = false;
            Owner->TempData.Trade.SelectOK= false;
//			Owner->PlayerBaseData->Body.Money += Owner->TempData.Trade.Money ;
            //Owner->AddBodyMoney( Owner->TempData.Trade.Money , Target->Base.Account_ID.Begin() , Target->RoleName() , EM_ActionType_Tran );
            Owner->TempData.Trade.Money = 0;

            return;
        }

        //����(Client)�������~�M��
        S_ClearTradeItem( Owner->GUID() );
        S_ClearTradeItem( Target->GUID() );

        //�������~���X
        vector<ItemFieldStruct>	OwnerTrade;
        vector<ItemFieldStruct>	TargetTrade;
        ItemFieldStruct*	TempItem;
        ItemState_ENUM		WearPos;

        for( i = 0 ; i < Owner->PlayerBaseData->Body.Count ; i++ )
        {
            WearPos = Owner->PlayerBaseData->Body.Item[i].Pos;
            if(		WearPos >= EM_TRADE_ITEM1 
                &&  WearPos <= EM_TRADE_ITEM25 )
            {
                TempItem = &Owner->PlayerBaseData->Body.Item[i];
                TempItem->Pos = EM_ItemState_NONE;
                OwnerTrade.push_back( *TempItem);

				Target->Log_ItemTrade( Owner , EM_ActionType_Tran , *TempItem , "" );
                TempItem->Init();
            }
        }

        for( i = 0 ; i < Target->PlayerBaseData->Body.Count ; i++ )
        {
            WearPos = Target->PlayerBaseData->Body.Item[i].Pos;
            if(		WearPos >= EM_TRADE_ITEM1 
                &&  WearPos <= EM_TRADE_ITEM25 )
            {
                TempItem = &Target->PlayerBaseData->Body.Item[i];
                TempItem->Pos = EM_ItemState_NONE;
                TargetTrade.push_back( *TempItem);

				Owner->Log_ItemTrade( Target , EM_ActionType_Tran , *TempItem , "" );
                TempItem->Init();                
            }
        }

		if( TargetTrade.size() + OwnerTrade.size() == 1 )
		{
			GameObjDbStructEx* TradeItemDB;
			RelationTypeENUM Relation1;
			RelationTypeENUM Relation2;
			if( TargetTrade.size() != 0 )
			{
				TradeItemDB = Global::GetObjDB( TargetTrade[0].OrgObjID );
				if( TradeItemDB == NULL )
					return;

				Relation2 = TradeItemDB->Item.TargetRelation;
				Relation1 = TradeItemDB->Item.SenderRelation;
			}
			else
			{
				TradeItemDB = Global::GetObjDB( OwnerTrade[0].OrgObjID );

				if( TradeItemDB == NULL )
					return;

				Relation2 = TradeItemDB->Item.SenderRelation;
				Relation1 = TradeItemDB->Item.TargetRelation;				
			}
				
			if( TradeItemDB->IsItem() && TradeItemDB->Item.ItemType == EM_ItemType_Relation )
			{
				//��X
				BaseFriendStruct* OwnerFriend = NULL;
				BaseFriendStruct* TargetFriend = NULL;

				int OwnerPos = Owner->PlayerSelfData->FriendList.FindFriendInfo( EM_FriendListType_FamilyFriend, Target->DBID() , &OwnerFriend );
				int TargetPos = Target->PlayerSelfData->FriendList.FindFriendInfo( EM_FriendListType_FamilyFriend, Owner->DBID() , &TargetFriend );
				if( OwnerPos != -1 && TargetPos != -1 && OwnerFriend->IsEmpty() && TargetFriend->IsEmpty() )
				{
					OwnerFriend->DBID		= Target->DBID();
					OwnerFriend->Relation	= Relation1;
					OwnerFriend->Race		= Target->TempData.Attr.Race;		//�ر�
					OwnerFriend->Voc		= Target->TempData.Attr.Voc;		//¾�~
					OwnerFriend->Voc_Sub	= Target->TempData.Attr.Voc_Sub;	//��¾�~
					OwnerFriend->Sex		= Target->TempData.Attr.Sex;		//�ʧO
					OwnerFriend->JobLv		= Target->TempData.Attr.Level;	
					OwnerFriend->JobLv_Sub	= Target->TempData.Attr.Level_Sub;
					OwnerFriend->GuildID	= Target->BaseData.GuildID;
					OwnerFriend->TitleID	= Target->BaseData.TitleID;
					OwnerFriend->Time		= TimeStr::Now_Value();
					MyStrcpy( OwnerFriend->Name , Target->RoleName() , sizeof(OwnerFriend->Name) );
					OwnerFriend->Lv = 0;
					NetSrv_FriendList::SC_FixFriendInfo( Owner->GUID() , EM_FriendListType_FamilyFriend , OwnerPos , * OwnerFriend );
					NetSrv_FriendList::SChat_FriendListInfo( Owner->DBID() , Target->DBID() , true );

					TargetFriend->DBID		= Owner->DBID();
					TargetFriend->Relation	= Relation2;
					TargetFriend->Race		= Owner->TempData.Attr.Race;		//�ر�
					TargetFriend->Voc		= Owner->TempData.Attr.Voc;		//¾�~
					TargetFriend->Voc_Sub	= Owner->TempData.Attr.Voc_Sub;	//��¾�~
					TargetFriend->Sex		= Owner->TempData.Attr.Sex;		//�ʧO
					TargetFriend->JobLv		= Owner->TempData.Attr.Level;	
					TargetFriend->JobLv_Sub	= Owner->TempData.Attr.Level_Sub;
					TargetFriend->GuildID	= Owner->BaseData.GuildID;
					TargetFriend->TitleID	= Owner->BaseData.TitleID;
					TargetFriend->Time		= TimeStr::Now_Value();

					MyStrcpy( TargetFriend->Name , Owner->RoleName() , sizeof(TargetFriend->Name) );
					TargetFriend->Lv = 0;
					NetSrv_FriendList::SC_FixFriendInfo( Target->GUID() , EM_FriendListType_FamilyFriend , TargetPos , * TargetFriend );
					NetSrv_FriendList::SChat_FriendListInfo( Target->DBID() , Owner->DBID() , true );

					Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_RelationOK );
					Target->Net_GameMsgEvent( EM_GameMessageEvent_Trade_RelationOK );

					TargetTrade.clear();
					OwnerTrade.clear();
				}
				else
				{
					Target->Net_GameMsgEvent( EM_GameMessageEvent_Trade_RelationErr );
					Owner->Net_GameMsgEvent( EM_GameMessageEvent_Trade_RelationErr );
					//S_FinalOkFailed( Owner->GUID() , EM_FinalOKFailed_Unknow );
					//return;
					swap( TargetTrade , OwnerTrade );
				}
			}
		}

        //�p�G�ƶqOK�h������洫���ʰ�
        for( i = _MAX_BODY_BEGIN_POS_ ; i < Owner->PlayerBaseData->Body.Count && TargetTrade.size() != 0 ; i++ )
        {
            if( Owner->PlayerBaseData->Body.Item[i].IsEmpty() && Owner->CheckItemPosTimeFlag( 0 , i ) != false )
            {
                Owner->PlayerBaseData->Body.Item[i] = TargetTrade.back();
                NetSrv_Item::FixItemInfo_Body( Owner->GUID() , Owner->PlayerBaseData->Body.Item[i] , i );	
                TargetTrade.pop_back();
            }
        }

        for( i = _MAX_BODY_BEGIN_POS_ ; i < Target->PlayerBaseData->Body.Count && OwnerTrade.size() != 0 ; i++ )
        {
            if( Target->PlayerBaseData->Body.Item[i].IsEmpty() && Target->CheckItemPosTimeFlag( 0 , i ) != false )
            {
                Target->PlayerBaseData->Body.Item[i] = OwnerTrade.back();
                NetSrv_Item::FixItemInfo_Body( Target->GUID() , Target->PlayerBaseData->Body.Item[i] , i );	
                OwnerTrade.pop_back();
            }
        }

        S_TradeItemOK( Owner->GUID() );
        S_TradeItemOK( Target->GUID() );

        Target->AddBodyMoney( Owner->TempData.Trade.Money - Target->TempData.Trade.Money , Owner , EM_ActionType_Tran , true );
        Owner->AddBodyMoney( Target->TempData.Trade.Money - Owner->TempData.Trade.Money , Target , EM_ActionType_Tran , true );

        Target->AddBodyMoney_Account( Owner->TempData.Trade.Money_Account - Target->TempData.Trade.Money_Account , Owner , EM_ActionType_Tran , true );
        Owner->AddBodyMoney_Account( Target->TempData.Trade.Money_Account - Owner->TempData.Trade.Money_Account , Target , EM_ActionType_Tran , true );

        Owner->TempData.Trade.Init();
        Target->TempData.Trade.Init();

		//Log�O��
		Owner->PlayerTempData->Log.ActionMode.Trade = true;
		Target->PlayerTempData->Log.ActionMode.Trade = true;

        Owner->IsTrade( false );
        Target->IsTrade( false );
    }
    else
    {
        Owner->TempData.Trade.FinalOK = true ;
        //S_TargetFinalOK( Target->GUID() );
		S_TradeState( Owner , Target );
    }

}
//---------------------------------------------------------------------------
//������
void NetSrv_TradeChild::R_StopTrade		( BaseItemObject*	OwnerObj )
{
    RoleDataEx*         Owner  = OwnerObj->Role();	
    BaseItemObject*     TargetObj = BaseItemObject::GetObj( Owner->TempData.Trade.TargetID );

	if( Owner->IsTrade() == false ) 
		return;

	if( TargetObj != NULL )
	{
		RoleDataEx*         Target  = TargetObj->Role();	

		if(		Target->DBID() == Owner->TempData.Trade.TargetDBID 
			&&	Target->IsTrade() )
		{
			S_StopTrade( Target->GUID() , Owner->GUID() );
			Target->TempData.Trade.Init();
			Target->ClsAllTradeItem();
			Target->IsTrade( false );
		}
	}
	
	S_StopTrade( Owner->GUID() , Owner->TempData.Trade.TargetID );	

	Owner->TempData.Trade.Init();

	Owner->ClsAllTradeItem();
	Owner->IsTrade( false );
}

//---------------------------------------------------------------------------
void NetSrv_TradeChild::S_TradeState( RoleDataEx* Owner , RoleDataEx* Target )
{

	NetSrv_Trade::S_TradeState( Owner->GUID() , Owner->TempData.Trade.SelectOK , Owner->TempData.Trade.FinalOK 
								, Target->TempData.Trade.SelectOK , Target->TempData.Trade.FinalOK );
	NetSrv_Trade::S_TradeState( Target->GUID() , Target->TempData.Trade.SelectOK , Target->TempData.Trade.FinalOK 
								, Owner->TempData.Trade.SelectOK , Owner->TempData.Trade.FinalOK );
}
//---------------------------------------------------------------------------
//�����P�ؼХ���n��
void NetSrv_TradeChild::R_CancelTradeRequest( BaseItemObject*	Obj , int TargetID )
{
	RoleDataEx*	Owner = Obj->Role();
	if( TargetID != Owner->TempData.Trade.TargetID )
		return;

	BaseItemObject*     TargetObj = BaseItemObject::GetObj( TargetID );
	if( TargetObj == NULL )
		return;

	RoleDataEx*	Target = TargetObj->Role();
	if( Target->DBID() != Owner->TempData.Trade.TargetDBID )
		return;

	S_CancelTradeRequest( Target->GUID() , Owner->GUID() );
}