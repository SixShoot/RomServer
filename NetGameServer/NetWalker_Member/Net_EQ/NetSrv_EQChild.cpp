#include "../NetWakerGSrvInc.h"
#include "NetSrv_EQChild.h"
#include "../../mainproc/magicproc/MagicProcess.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_EQChild::Init()
{
    NetSrv_EQ::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_EQChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_EQChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_EQ::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
void NetSrv_EQChild::R_SetEQ_Body( BaseItemObject* Sender , ItemFieldStruct& Item , int ItemPos , EQWearPosENUM EQPos , bool IsFromEq  )
{
	char		Buf[512];
    RoleDataEx *Owner = Sender->Role();
    int         SendID  = Sender->GUID();
    if( Owner == NULL )
        return;

	if( EQPos >= _DEF_EQ_BACKUP_BASE_ )
	{
		//if( Owner->CheckKeyItem( g_ObjectData->SysValue().KeyItem_EQBackup ) == false )
		if( Owner->JobCount() < 2  )
		{
			return;
		}
	}

	ItemFieldStruct* EQItem = Owner->GetEqItem(EQPos);

	ItemFieldStruct* BodyItem = Owner->GetBodyItem(ItemPos);

	if( EQItem == NULL || BodyItem == NULL )
		return;

	if( EQItem->Pos != EM_ItemState_NONE || BodyItem->Pos != EM_ItemState_NONE )
		return;

	if( Owner->IsDead() )
	{
		S_SetEQ_Failed_Body( SendID , *BodyItem  , ItemPos , *EQItem , EQPos );
		return;
	}

	if( BodyItem->Mode.PkProtect != false && EQItem->Mode.PkProtect == false ) 
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
			S_SetEQ_Failed_Body( SendID , *BodyItem , ItemPos , *EQItem , EQPos );
			return;
		}
	}


    switch( Owner->ExchangeBodyEQItem( Item , ItemPos , *Owner->GetEqItem( EQPos ) , EQPos , IsFromEq  ) )
    {
    case EM_RoleData_PosErr:
        {
            Print( LV5 , "R_SetEQ_Body" ,"Pos Error Account = %s" , Owner->Account_ID() );

			sprintf( Buf , "Set(Bag->Eq) , Pos Error RoleName=%s ItemPos = %d EQPos = %d" , Global::GetRoleName_ASCII( Owner ).c_str() , ItemPos , EQPos );
			Global::SendMsgToGMTools( LV5 , Owner->Account_ID() , Buf );
            //Sender->Destroy( Buf );
			S_SetEQ_Failed_Body( SendID , *Owner->GetBodyItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos );
            return;
        }
        break;
    case EM_RoleData_OrgObjIDErr:
		{
			//Owner->Msg( "R_SetEQ_Body 交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_EQError_OrgObjID_Body );
			S_SetEQ_Failed_Body( SendID , *Owner->GetBodyItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos );
			return;
		}
    case EM_RoleData_DataInfoErr:
		{
			//Owner->Msg( "R_SetEQ_Body 交易資訊有問題" );
			//Owner->Net_GameMsgEvent( EM_GameMessageEvent_EQError_DataInfo_Body );
			S_SetEQ_Failed_Body( SendID , *Owner->GetBodyItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos );
			return;
		}
    case EM_RoleData_EQWearPosErr:
        {
            //Owner->Msg( "R_SetEQ_Body 交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_EQError_WearPos_Body );
            S_SetEQ_Failed_Body( SendID , *Owner->GetBodyItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos );
            return;
        }
	case EM_RoleData_Err_ItemUnique:
		{
			//Owner->Msg( "R_SetEQ_Body 交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_EQError_WearPos_ItemUnique );
			S_SetEQ_Failed_Body( SendID , *Owner->GetBodyItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos );
			return;
		}
    case EM_RoleData_OK:
		if( EM_EQWearPos_MainHand == EQPos )
		{
			Owner->BaseData.SP = 0;
			Owner->BaseData.SP_Sub = 0;
		}
		
        S_SetEQ_OK_Body( SendID , *Owner->GetBodyItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos  );
        break;
	case EM_RoleData_OKEx_SpecialEq:
		{
			NetSrv_Item::FixItemInfo_EQ		( SendID , *Owner->GetEqItem(EM_EQWearPos_MainHand)		, EM_EQWearPos_MainHand		);
			NetSrv_Item::FixItemInfo_EQ		( SendID , *Owner->GetEqItem(EM_EQWearPos_SecondHand)	, EM_EQWearPos_SecondHand	);
			NetSrv_Item::FixItemInfo_Body   ( SendID , *Owner->GetBodyItem( ItemPos )				, ItemPos					);
			S_SetEQResult( SendID , ENMU_EQ_SetEqOK );

//			S_SetEQ_OK_Body( SendID , *Owner->GetBodyItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos  );
		}
		break;
    }

    return;
}
//-----------------------------------------------------------------
void NetSrv_EQChild::R_SetEQ_Bank( BaseItemObject* Sender , ItemFieldStruct& Item , int ItemPos , EQWearPosENUM EQPos , bool IsFromEq )
{
	char		Buf[512];
    RoleDataEx *Owner = Sender->Role();
    int         SendID  = Sender->GUID();
    if( Owner == NULL )
        return;

	if( EQPos >= _DEF_EQ_BACKUP_BASE_ )
	{
		if( Owner->JobCount() < 2)
		{
			return;
		}
	}


	ItemFieldStruct* EQItem = Owner->GetEqItem(EQPos);
	ItemFieldStruct* BankItem = Owner->GetBankItem(ItemPos);

	if( EQItem == NULL || BankItem == NULL )
		return;

	if( Owner->IsDead() )
	{
		S_SetEQ_Failed_Bank( SendID , *BankItem , ItemPos , *EQItem , EQPos );
		return;
	}

	if( BankItem->Mode.PkProtect != false && EQItem->Mode.PkProtect == false ) 
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
			S_SetEQ_Failed_Bank( SendID , *BankItem , ItemPos , *EQItem , EQPos );
			return;
		}
	}



    switch( Owner->ExchangeBankEQItem( Item , ItemPos , *EQItem  , EQPos , IsFromEq ) )
    {
    case EM_RoleData_PosErr:
        {
            Print( LV5 , "R_SetEQ_Bank" , "Pos Error Account = %s" , Owner->Account_ID() );

			sprintf( Buf , "Set(Bank->Eq) Pos Error,RoleName=%s ItemPos = %d EQPos = %d" , Global::GetRoleName_ASCII( Owner ).c_str() , ItemPos , EQPos );
			Global::SendMsgToGMTools( LV5 , Owner->Account_ID() , Buf );

            Sender->Destroy( Buf );
            return;
        }
        break;
    case EM_RoleData_OrgObjIDErr:
		{
			//Owner->Msg( "R_SetEQ_Bank 交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_EQError_OrgObjID_Bank );
			S_SetEQ_Failed_Bank( SendID , *Owner->GetBankItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos );
			return;
		}
    case EM_RoleData_DataInfoErr:
		{
			//Owner->Msg( "R_SetEQ_Bank 交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_EQError_DataInfo_Bank );
			S_SetEQ_Failed_Bank( SendID , *Owner->GetBankItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos );
			return;
		}
    case EM_RoleData_EQWearPosErr:
        {
            //Owner->Msg( "R_SetEQ_Bank 交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_EQError_WearPos_Bank );
            S_SetEQ_Failed_Bank( SendID , *Owner->GetBankItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos );
            return;
        }
	case EM_RoleData_Err_ItemUnique:
		{
			//Owner->Msg( "R_SetEQ_Body 交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_EQError_WearPos_ItemUnique );
			S_SetEQ_Failed_Bank( SendID , *Owner->GetBankItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos );
			return;
		}
    case EM_RoleData_OK:
        S_SetEQ_OK_Bank( SendID , *Owner->GetBankItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos  );
        break;
	case EM_RoleData_OKEx_SpecialEq:
		NetSrv_Item::FixItemInfo_EQ		( SendID , *Owner->GetEqItem(EM_EQWearPos_MainHand)		, EM_EQWearPos_MainHand		);
		NetSrv_Item::FixItemInfo_EQ		( SendID , *Owner->GetEqItem(EM_EQWearPos_SecondHand)	, EM_EQWearPos_SecondHand	);
		NetSrv_Item::FixItemInfo_Bank   ( SendID , *Owner->GetBankItem( ItemPos )				, ItemPos					);
		S_SetEQResult( SendID , ENMU_EQ_SetEqOK );
		break;
    }

    return;
}
//-----------------------------------------------------------------
void NetSrv_EQChild::R_SetEQ_EQ( BaseItemObject* Sender , ItemFieldStruct& Item , int ItemPos , EQWearPosENUM EQPos )
{
	char		Buf[512];
    RoleDataEx *Owner = Sender->Role();
    int         SendID  = Sender->GUID();
    if( Owner == NULL )
        return;

    switch( Owner->ExchangeEQItem( Item , ItemPos , *Owner->GetEqItem( EQPos ) , EQPos ) )
    {
    case EM_RoleData_PosErr:
        {
            Print( LV5 , "R_SetEQ_EQ" , "Pos Error Account = %s" , Owner->Account_ID() );

			sprintf( Buf , "Set(EQ->EQ)Pos Error , RoleName=%s ItemPos = %d EQPos = %d" , Global::GetRoleName_ASCII( Owner ).c_str() , ItemPos , EQPos );
			Global::SendMsgToGMTools( LV5 , Owner->Account_ID() , Buf );
            Sender->Destroy( Buf );
            return;
        }
        break;
    case EM_RoleData_OrgObjIDErr:
		{
			//Owner->Msg( "交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_EQError_OrgObjID_EQ );
			S_SetEQ_Failed_EQ( SendID , *Owner->GetEqItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos );
			return;
		}
    case EM_RoleData_DataInfoErr:
		{
			//Owner->Msg( "交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_EQError_DataInfo_EQ );
			S_SetEQ_Failed_EQ( SendID , *Owner->GetEqItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos );
			return;
		}
    case EM_RoleData_EQWearPosErr:
        {
            //Owner->Msg( "交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_EQError_WearPos_EQ );
            S_SetEQ_Failed_EQ( SendID , *Owner->GetEqItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos );
            return;
        }
	case EM_RoleData_Err_ItemUnique:
		{
			//Owner->Msg( "R_SetEQ_Body 交易資訊有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_EQError_WearPos_ItemUnique );
			S_SetEQ_Failed_Body( SendID , *Owner->GetBodyItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos );
			return;
		}
    case EM_RoleData_OK:
        S_SetEQ_OK_EQ( SendID , *Owner->GetEqItem( ItemPos ) , ItemPos , *Owner->GetEqItem(EQPos) , EQPos  );
        break;
    }

    return;
}

void NetSrv_EQChild::R_SetShowEQ( BaseItemObject* Obj , ShowEQStruct ShowEQ )
{
/*
	RoleDataEx* Owner = Obj->Role();

	ShowEQStruct& RoleShowEQ = Owner->PlayerBaseData->CoolSuitList[ Owner->PlayerBaseData->CoolSuitID ].ShowEQ;

	if( RoleShowEQ._Value == ShowEQ._Value )
		return;
	
	RoleShowEQ = ShowEQ;

	Owner->TempData.UpdateInfo.Eq = true;
*/

}

//通知週圍某角色出手
/*
void NetSrv_EQChild::SendRangeShowEQInfo	( RoleDataEx* Owner )
{
	RoleDataEx *Other; 

	Global::ResetRange( Owner , _Def_View_Block_Range_ );

	while( (Other = Global::GetRangePlayer()) != NULL )
	{
		S_ShowEQInfo( Other->GUID() , Owner->GUID() , Owner->PlayerBaseData->ShowEQ );
	}
}
*/
void NetSrv_EQChild::R_SwapBackupEQ	( BaseItemObject* Obj , int bkPosID )
{
	RoleDataEx* Role = Obj->Role();

	if( Role->TempData.Sys.OnChangeZone != false )
		return;

	if(		Role->JobCount() < 2
		||	Role->IsDead()		)
	{
		S_SwapBackupEQ( Obj->GUID() , bkPosID , false );
		return;
	}
		//check keyitem
	if( bkPosID )
	{
		char Buf[512];
		sprintf( Buf , "BackupEQSlot%d_KeyItemID" , bkPosID + 1 );
		int keyID = g_ObjectData->GetSysKeyValue( Buf );
		if( keyID && Role->CheckKeyItem( keyID ) == false )
		{
			S_SwapBackupEQ( Obj->GUID() , bkPosID , false );
			return;
		}
	}

	//如果有裝備正在使用中則不可交換
	for( int i = 0 ; i < EM_EQWearPos_MaxCount ; i++ )
	{
		ItemFieldStruct& Item = Role->BaseData.EQ.Item[i];
		if( Item.IsEmpty() || Item.Pos == EM_ItemState_NONE )
			continue;
		S_SwapBackupEQ( Obj->GUID() , bkPosID , false );
		return;
	}




	Role->SwapEQ( bkPosID );
	S_SwapBackupEQ( Obj->GUID() , bkPosID , true );
}
//套裝技能
void NetSrv_EQChild::R_LearnSuitSkill_Close	( BaseItemObject* Obj )
{
	RoleDataEx* Owner = Obj->Role();
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_SuitSkill ) == false )
	{
		return;
	}

	Owner->ClsOpenMenu();
}
void NetSrv_EQChild::R_LearnSuitSkillRequest( BaseItemObject* Obj , int ItemCount , int ItemPos[10] , int LearnSkillID )
{
	RoleDataEx* Owner = Obj->Role();
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_SuitSkill ) == false )
	{
		S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_NotOpen );
		return;
	}
	if( ItemCount > 10 || ItemCount < 1 )
	{
		S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_DataErr );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	GameObjDbStructEx* MagicCol = Global::GetObjDB( LearnSkillID );
	if( MagicCol->IsMagicCollect() == false )
	{
		S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_DataErr );
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	map< int , int >  ItemIdMap;
	//取得各欄位 是否有重複
	for( int i = 0 ; i < ItemCount ; i++ )
	{
		int Pos = ItemPos[i] % 0x10000;
		int Type= ItemPos[i] / 0x10000;
		ItemFieldStruct* Item = NULL;// = Owner->GetBodyItem( ItemPos[i] );
		switch( Type )
		{
		case 0://身體
			Item = Owner->GetBodyItem( Pos );
			break;
		case 1://裝備
			Item = Owner->GetEqItem( Pos );
			break;
		}
		if( Item == NULL )
		{
			S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_DataErr );
			return;
		}
		if( ItemIdMap[ Item->OrgObjID ] != 0 || Item->Mode.SuitSkill != 0 )
		{
			S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_DataErr );
			return;
		}
		ItemIdMap[ Item->OrgObjID ]++;
	}

	//////////////////////////////////////////////////////////////////////////
	//檢查是否是一套
	//////////////////////////////////////////////////////////////////////////
	map< int , int >::iterator Iter;
	Iter = ItemIdMap.begin();
	GameObjDbStructEx* ItemDB = Global::GetObjDB( Iter->first );
	if(		ItemDB == NULL 
		|| ( ItemDB->IsArmor() == false && ItemDB->IsWeapon() == false ) )
	{
		S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_DataErr );
		return;
	}

	int SuitID = ItemDB->Item.SuitID;
	GameObjDbStructEx* SuitDB = Global::GetObjDB( SuitID );
	if( SuitDB->IsSuit() == false )
	{
		S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_DataErr );
		return;
	}
	
	for( Iter = Iter++ ; Iter != ItemIdMap.end() ; Iter++ )
	{
		GameObjDbStructEx* ItemDB = Global::GetObjDB( Iter->first );
		if( ItemDB == NULL)
		{
			S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_DataErr );
			return;
		}
		if( ItemDB->Item.SuitID != SuitID )
		{
			S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_DataErr );
			return;
		}
	}
	
	if( SuitDB->Suit.TotalCount != ItemIdMap.size() )
	{
		S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_DataErr );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	//技能查詢 是否學過
	if( Owner->FindSuitSkill(LearnSkillID) != NULL )
	{
		S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_AlwaysLearn );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	{
		//檢查技能是否存在
		bool IsFind = false;
		for( int i = 0 ; i < 4 ; i++ )
		{
			if( SuitDB->Suit.SuitSkillID[i] == LearnSkillID )
			{
				IsFind = true;
				break;
			}
		}
		if( IsFind == false )
		{
			S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_DataErr );
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//檢查是否金錢足夠
	if( Owner->PlayerBaseData->Body.Money < SuitDB->Suit.SuitSkillMoney )
	{
		S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_MoneyErr );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	//設定物品標記並學習技能
	//////////////////////////////////////////////////////////////////////////
	for( int i = 0 ; i < ItemCount ; i++ )
	{
		int Pos = ItemPos[i] % 0x10000;
		int Type= ItemPos[i] / 0x10000;
		ItemFieldStruct* Item = NULL;// = Owner->GetBodyItem( ItemPos[i] );
		switch( Type )
		{
		case 0://身體
			Item = Owner->GetBodyItem( Pos );
			Item->Mode.SuitSkill = true;
			Owner->Net_FixItemInfo_Body( Pos );
			break;
		case 1://裝備
			Item = Owner->GetEqItem( Pos );
			Item->Mode.SuitSkill = true;
			Owner->Net_FixItemInfo_EQ( Pos );
			break;
		}
/*
		ItemFieldStruct* Item = Owner->GetBodyItem( ItemPos[i] );
		Item->Mode.SuitSkill = true;
		Owner->Net_FixItemInfo_Body( ItemPos[i] );
		*/
	}
	SuitSkllBaseStruct TempSuitSkillInfo;
	TempSuitSkillInfo.SkillID = LearnSkillID;

	SuitSkillStruct& SuitSkill = Owner->PlayerBaseData->SuitSkill;
	if( SuitSkill.LeanSkillCount >= _MAX_SUIT_SKILL_COUNT_ )
	{
		S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_SkillFull );
		return;
	}
	SuitSkill.LeanSkillList[ SuitSkill.LeanSkillCount ] = TempSuitSkillInfo ;
	SuitSkill.LeanSkillCount++;
	//扣錢
	Owner->AddBodyMoney( SuitDB->Suit.SuitSkillMoney*-1 , NULL , EM_ActionType_SuitSkillCost , true );
	S_LearnSuitSkillResult	( Owner->GUID() , LearnSkillID , EM_LearnSuitSkillResult_OK );
	
}
void NetSrv_EQChild::R_SetSuitSkillRequest	( BaseItemObject* Obj , int SkillID , int Pos )
{
	RoleDataEx* Owner = Obj->Role();
	SuitSkillStruct& SuitSkill = Owner->PlayerBaseData->SuitSkill;

	if( Owner->IsAttackMode() != false )
	{
		S_SetSuitSkillResult( Owner->GUID() , EM_SetSuitSkillResult_OnAttackMode );
		return;
	}
	GameObjDbStructEx* SkillDB = Global::GetObjDB( SkillID );
	if( SkillID  )
	{
		if(		SkillDB->IsMagicCollect() == false 
			||	(		SkillDB->MagicCol.Need[0].Type == EM_MagicSpellNeedType_Self_MainJob
					&&	SkillDB->MagicCol.Need[0].Value != Owner->BaseData.Voc) )
		{
			S_SetSuitSkillResult( Owner->GUID() , EM_SetSuitSkillResult_SkillNotFind );
			return;
		}
	}

	if( Pos < 0 
		|| Pos >= SuitSkill.MaxEqSkillCount 
		|| Pos >= _MAX_SUIT_SKILL_EQ_COUNT_		)
	{
		S_SetSuitSkillResult( Owner->GUID() , EM_SetSuitSkillResult_PosErr );
		return;
	}

	char Buf[512];
	if( SkillID != 0 )
	{
		//檢查是否有此技能
		SuitSkllBaseStruct* SkillBase = Owner->FindSuitSkill( SkillID );
		if( SkillBase == NULL )
		{
			S_SetSuitSkillResult( Owner->GUID() , EM_SetSuitSkillResult_SkillNotFind );
			return;
		}
		sprintf( Buf , "SuitSkillEqEvent(%d,0)" , SkillID );
	}
	else
	{
		sprintf( Buf , "SuitSkillEqEvent(%d,1)" , SuitSkill.EQ[ Pos ] );
	}

	LUA_VMClass::PCallByStrArg( Buf , Owner->GUID() ,Owner->GUID() );	

	SuitSkill.EQ[ Pos ] = SkillID;

	MagicClearTimeStruct ClearEvent;
	ClearEvent._Value = 0;
	ClearEvent.ChangeSuitSkill = true;
	Owner->ClearBuffByEvent( ClearEvent );

	for( int i = 0 ; ; i++ )
	{
		if(	   i >= SuitSkill.MaxEqSkillCount 
			|| i >= _MAX_SUIT_SKILL_EQ_COUNT_		)
			break;
		GameObjDbStructEx* magicColDB = g_ObjectData->GetObj( SuitSkill.EQ[i] );
		if( magicColDB->IsMagicCollect() == false )
			continue;
		BuffBaseStruct* Buff = Owner->AssistMagic( Owner , magicColDB->MagicCol.SuitPassiveSkill , 0 , false , 100000000 );
		if( Buff != NULL )
			NetSrv_MagicChild::SendRange_AddBuffInfo( Owner->GUID() , Owner->GUID() , magicColDB->MagicCol.SuitPassiveSkill , Buff->Power , Buff->Time );

	}

	S_SetSuitSkillResult( Owner->GUID() , EM_SetSuitSkillResult_OK );
}
void NetSrv_EQChild::R_UseSuitSkillRequest	( BaseItemObject* Obj , int SkillID , int TargetID , float PosX , float PosY , float PosZ  )
{
	RoleDataEx* Owner = Obj->Role();
	SuitSkillStruct& SuitSkill = Owner->PlayerBaseData->SuitSkill;
	if( Owner->FindSuitSkill_EQ( SkillID ) == false )
	{
		S_UseSuitSkillResult( Owner->GUID() , EM_UseSuitSkillResult_SkillNotFind );
		return;
	}
	if( Owner->IsSpell() )
		return;

	if( Owner->TempData.Attr.Effect.DisableJobSkill )
		return;

	MagicProcessClass::SpellMagic( Owner->GUID() , TargetID , PosX , PosY , PosZ  , SkillID , 0 );	

	S_UseSuitSkillResult( Owner->GUID() , EM_UseSuitSkillResult_OK );
}
