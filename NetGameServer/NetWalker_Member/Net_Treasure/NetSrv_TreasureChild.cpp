#include "../NetWakerGSrvInc.h"
#include "NetSrv_TreasureChild.h"
#include "../../mainproc/magicproc/MagicProcess.h"
#include "../../mainproc/PartyInfoList/PartyInfoList.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_TreasureChild::Init()
{
    NetSrv_Treasure::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_TreasureChild );	

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_TreasureChild::Release()
{
    if( This == NULL )
        return false;

	NetSrv_Treasure::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
void NetSrv_TreasureChild::RC_OpenRequest( BaseItemObject* Obj , int ItemGUID )
{
	RoleDataEx* Owner = Obj->Role();
	RoleDataEx* Treasure = Global::GetRoleDataByGUID( ItemGUID );
	if(		Treasure == NULL 
		||	Treasure->DynamicData.TreasureBox == NULL
		||	Treasure->BaseData.Mode.Treasure == false 
		||  Treasure->Length( Owner ) > 100		 )
	{
		SC_OpenFailed( Obj->GUID() , ItemGUID );
		return;
	}

	//檢查寶箱是否有人開啟中
	RoleDataEx* TreasureTarget = Global::GetRoleDataByGUID( Treasure->TempData.Trade.TargetID );
	if( TreasureTarget != NULL)
	{
		if( Treasure->Length( TreasureTarget ) < 100 )
		{
			SC_OpenFailed( Owner->GUID() , ItemGUID );
			return;
		}
		else
		{
			Treasure->TempData.Trade.TargetID = -1;
			SendRange_TreasureLootInfo( Treasure );
		}
	}
	if( _CheckTreasureEmpty( Owner , Treasure ) != false )
	{
		SC_OpenFailed( Owner->GUID() , ItemGUID );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	int MaxCount = 0;
	ItemFieldStruct Items[30];

	bool	IsNormalLootOK = true;
	TreasureBoxStruct& Box = *(Treasure->DynamicData.TreasureBox);

	if( Box.TreasurePartyID != -1 && Owner->PartyID() != Box.TreasurePartyID )
		IsNormalLootOK = false;

	if( Box.TreasureOwnerDBID != -1 && Box.TreasureOwnerDBID != Owner->DBID() )
		IsNormalLootOK = false;

	for( int i = 0 ; i < Box.Count ; i++ )
	{
		if( Box.Item[i].IsEmpty() )
			continue;

		if(		( Box.OwnerDBID[i] == -1 && IsNormalLootOK != false )
			||	Box.OwnerDBID[i] == Owner->DBID() 
			||	( Owner->PartyID() == Box.TreasurePartyID && Box.OwnerDBID[i] == -1 ))
		{
			Items[MaxCount] = Box.Item[i];
			Items[MaxCount].Serial = i;
			MaxCount++;
			if( MaxCount >= 30 )
				break;
		}			
	}
	Treasure->TempData.Trade.TargetID = Owner->GUID();
	SC_Open( Owner->GUID() , Treasure->GUID() , MaxCount , Items );
	//////////////////////////////////////////////////////////////////////////
}
void NetSrv_TreasureChild::RC_GetItem( BaseItemObject* Obj , int ItemGUID , int BodyPos , int ItemSerial )
{
	RoleDataEx* Owner = Obj->Role();
	RoleDataEx* Treasure = Global::GetRoleDataByGUID( ItemGUID );
	if(		Treasure == NULL 
		||	Treasure->BaseData.Mode.Treasure == false 
		||  Treasure->Length( Owner ) > 100 
		||  (unsigned)ItemSerial >= (unsigned)Treasure->DynamicData.TreasureBox->Count	
		||	(unsigned)BodyPos >= (unsigned)Owner->PlayerBaseData->Body.Count 
		||  Treasure->DynamicData.TreasureBox == NULL )
	{
		SC_GetItemResult( Obj->GUID() , ItemSerial , BodyPos , EM_TreasureGetItemResult_Faild );
		return;
	}
	
	int	ItemOwnerDBID = Treasure->DynamicData.TreasureBox->OwnerDBID[ ItemSerial ];
	ItemFieldStruct& TreasureItem = Treasure->DynamicData.TreasureBox->Item[ ItemSerial ];
	ItemFieldStruct& BodyItem = Owner->PlayerBaseData->Body.Item[ BodyPos ];

	if( TreasureItem.IsEmpty()	)
	{
		SC_GetItemResult( Obj->GUID() , ItemSerial , BodyPos  , EM_TreasureGetItemResult_Faild );
		return;
	}

	//TreasureItem.Serial = 
	if( TreasureItem.Serial == 0 )
	{
		TreasureItem.CreateTime = RoleDataEx::G_Now;
		TreasureItem.Serial		= Global::GenerateItemVersion( );
	}
		

	GameObjDbStructEx* TreasureItemDB = Global::GetObjDB( TreasureItem.OrgObjID );
	if( TreasureItemDB == NULL ) 
	{
		SC_GetItemResult( Obj->GUID() , ItemSerial , BodyPos  , EM_TreasureGetItemResult_Faild );
		return;
	}
	bool	IsNoShare = false;	
	if( TreasureItemDB->IsItem() && TreasureItemDB->Item.BodyMaxCount != 0 )
	{
		IsNoShare = true;
		//計算物品數量
		int Count = Owner->CalBodyItemCount( TreasureItemDB->GUID ) + Owner->CalBankItemCount( TreasureItemDB->GUID ) + Owner->CalEqItemCount( TreasureItemDB->GUID ) + Owner->CountItemFromQueue( TreasureItemDB->GUID );
		if( Count > TreasureItemDB->Item.BodyMaxCount )
		{
			SC_GetItemResult( Obj->GUID() , ItemSerial , BodyPos  , EM_TreasureGetItemResult_MaxCountErr );
			return;
		}
	}

	TreasureBoxStruct& TreasureBox = *(Treasure->DynamicData.TreasureBox);

	if( ItemOwnerDBID != -1 )
	{
		if( ItemOwnerDBID != Owner->DBID() )
		{
			SC_GetItemResult( Obj->GUID() , ItemSerial , BodyPos , EM_TreasureGetItemResult_Faild );
			return;
		}
	}
	else
	{
		if( TreasureBox.TreasureOwnerDBID != -1 )
		{		
			if( TreasureBox.TreasureOwnerDBID != Owner->DBID() )
			{
				if( TreasureBox.TreasurePartyID == Owner->PartyID() && ItemOwnerDBID == -1 && Owner->PartyID() != -1 )
				{

				}
				else
				{
					SC_GetItemResult( Obj->GUID() , ItemSerial , BodyPos , EM_TreasureGetItemResult_Faild );
					return;
				}
			}
		}
		else if( TreasureBox.TreasurePartyID != -1 )
		{
			if( TreasureBox.TreasurePartyID != Owner->PartyID() )
			{
				SC_GetItemResult( Obj->GUID() , ItemSerial , BodyPos , EM_TreasureGetItemResult_Faild );
				return;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool IsPartyShareItem = false;
	StaticVector< BaseItemObject* , _MAX_PARTY_COUNT_  > LootObj;	
	//if( TreasureBox.TreasurePartyID != -1 )
	if( Owner->PartyID() != -1 && IsNoShare == false )
	{
		PartyInfoStruct*	Party = PartyInfoListClass::This()->GetPartyInfo( Owner->PartyID() );
		if( Party != NULL )
		{
			if( Party->Info.RollTypeLV <= TreasureItemDB->Rare )
				IsPartyShareItem = true;

			for( unsigned int i = 0 ; i < Party->Member.size() ; i++ )
			{
				BaseItemObject* M = Global::GetPlayerObj_ByDBID( Party->Member[i].DBID );

				if( M == NULL )
					continue;

				if( Treasure->Length( M->Role() ) > _MAX_PARTY_SHARE_RANGE_ )
					continue;				

				if( Treasure->RoomID() != M->Role()->RoomID() )
					continue;

				if( M->Role()->DBID() == Obj->Role()->DBID() )
					continue;

				LootObj.Push_Back( M );
			}
		}
	}
	LootObj.Push_Back( Obj );

	if( ItemOwnerDBID != -1 )
		IsPartyShareItem = false;

	if( TreasureItem.OrgObjID == g_ObjectData->SysValue().Money_GoldObjID )
	{
		int DropMoney = TreasureItem.Money;

		int	ModMoney = TreasureItem.Money % LootObj.Size();
		DropMoney /= LootObj.Size();

		for( int i = 0 ; i < LootObj.Size() ; i++ )
		{
			int Money = DropMoney;
			if( LootObj.Size() - 1 == i )
				Money += ModMoney;
			else
			{
				if( rand() % 2 && ModMoney > 0 )
				{
					Money++;
					ModMoney--;
				}
			}
			LootObj[i]->Role()->AddBodyMoney( DropMoney , NULL , EM_ActionType_KillMonster , true );	
		}

		SC_GetItemResult( Obj->GUID() , ItemSerial , BodyPos , EM_TreasureGetItemResult_OK );
	}
	else if( LootObj.Size() == 1 || IsPartyShareItem == false )
	{
		Global::Log_ItemTrade( -1 , Treasure->DBID() , Owner->DBID() , (int)Owner->X(), (int)Owner->Z() , EM_ActionType_Loot , TreasureItem , "" );

		if( TreasureItemDB->IsItem() && TreasureItemDB->Mode.AutoUseItem )
		{
			Owner->AutoUseItem( TreasureItemDB );
			Owner->Net_FixItemInfo_Body( BodyPos );
			SC_GetItemResult( Obj->GUID() , ItemSerial , BodyPos , EM_TreasureGetItemResult_OK );
		}
		else if( TreasureItemDB->Mode.Unique != false && Owner->CheckItemInBody( TreasureItemDB->GUID ) != false )
		{
			Owner->Net_FixItemInfo_Body( BodyPos );
			SC_GetItemResult( Obj->GUID() , ItemSerial , BodyPos , EM_TreasureGetItemResult_ItemUnique );
		}
		else if( BodyItem.IsEmpty()  )
		{
			BodyItem = TreasureItem;
			Owner->Net_FixItemInfo_Body( BodyPos );
			SC_GetItemResult( Obj->GUID() , ItemSerial , BodyPos , EM_TreasureGetItemResult_OK );
		}
		else
		{
			if(		BodyItem.OrgObjID != TreasureItem.OrgObjID 
				||	TreasureItemDB->MaxHeap < BodyItem.Count + TreasureItem.Count
				||	BodyItem.Mode.HideCount != false)
			{
				Owner->Net_FixItemInfo_Body( BodyPos );
				SC_GetItemResult( Obj->GUID() , ItemSerial , BodyPos , EM_TreasureGetItemResult_Faild );
				return;
			}
			BodyItem.Count += TreasureItem.Count;
			Owner->Net_FixItemInfo_Body( BodyPos );
			SC_GetItemResult( Obj->GUID() , ItemSerial , BodyPos , EM_TreasureGetItemResult_OK );
		}
//		Owner->Net_FixItemInfo_Body( BodyPos );
	}
	else
	{
		NetSrv_PartyChild::SendItemToParty( LootObj , TreasureItem );
		Owner->Net_FixItemInfo_Body( BodyPos );
		SC_GetItemResult( Obj->GUID() , ItemSerial , BodyPos , EM_TreasureGetItemResult_PartyQueue );
	}

	TreasureItem.Init();

	
}
void NetSrv_TreasureChild::RC_Close( BaseItemObject* Obj , int ItemGUID )
{
	RoleDataEx* Owner = Obj->Role();
	RoleDataEx* Treasure = Global::GetRoleDataByGUID( ItemGUID );
	if(		Treasure == NULL 
		||	Treasure->BaseData.Mode.Treasure == false 
		||  Treasure->TempData.Trade.TargetID != Owner->GUID() 		)
	{
		SC_OpenFailed( Obj->GUID() , ItemGUID );
		return;
	}
	
	bool IsFind = false;
	if( Treasure->DynamicData.TreasureBox != NULL  )
	{
		TreasureBoxStruct& TreasureBox = *(Treasure->DynamicData.TreasureBox);

		//檢查是否物品都取完了
		for( int i = 0 ; i < TreasureBox.Count ; i++ )
		{
			if( TreasureBox.Item[i].IsEmpty() == false )
			{
				IsFind = true;
				break;
			}
		}

		if( IsFind )
		{
			Treasure->TempData.Trade.TargetID = -1;
		}
		else
		{
			if(		Treasure->SelfData.LiveTime != -1 
				&&	Treasure->Base.DBID == -1 )
			{
				TreasureBox.LiveTime = 0;
				Treasure->LiveTime( 0 , "Treasure Empty" );
			}
			TreasureBox.Count = 0;
			Treasure->BaseData.Mode.Treasure = false;
			Treasure->TempData.Trade.TargetID = -1;
		}


		if( TreasureBox.TreasureOwnerDBID == Owner->DBID() )
		{
			TreasureBox.TreasureOwnerDBID = -1;
			for( int i = 0 ; i < TreasureBox.Count ; i++ )
			{
				if( TreasureBox.Lock[i] == false )
				{
					TreasureBox.OwnerDBID[i] = -1;
				}
			}
		}
	}


	SendRange_TreasureLootInfo( Treasure );
	/*
	if( IsFind )
	{
		Treasure->TempData.Trade.TargetID = -1;
	}
	else
	{
		if(		Treasure->SelfData.LiveTime != -1 
			&&	Treasure->Base.DBID == -1 )
		{
			if( Treasure->DynamicData.TreasureBox != NULL )
			{
				TreasureBox.LiveTime = 0;
				Treasure->LiveTime( 0 , "Treasure Empty" );
			}
		}
		Treasure->BaseData.Mode.Treasure = false;
		Treasure->TempData.Trade.TargetID = -1;
	}
	*/
}
//-----------------------------------------------------------------
void NetSrv_TreasureChild::SendRange_TreasureLootInfo( RoleDataEx* Treasure )
{/*
	if( Treasure->BaseData.Mode.Treasure == false )
	{
		return;
	}*/

	RoleDataEx* Other;
	Global::ResetRange( Treasure , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;
		
		bool IsEmpty = _CheckTreasureEmpty( Other , Treasure );

		SC_BaseInfo( Other->GUID() , Treasure->GUID() , IsEmpty );
	}
}

void NetSrv_TreasureChild::Send_TreasureLootInfo( RoleDataEx* Owner , RoleDataEx* Treasure )
{
	if( Treasure->BaseData.Mode.Treasure == false )
		return;
	if( Owner->IsPlayer() == false )
		return;

	bool IsEmpty = _CheckTreasureEmpty( Owner , Treasure );
	SC_BaseInfo( Owner->GUID() , Treasure->GUID() , IsEmpty );
}


bool NetSrv_TreasureChild::_CheckTreasureEmpty( RoleDataEx* Owner , RoleDataEx* Treasure )
{
	bool	IsNormalLootOK = true;

	TreasureBoxStruct* Box = Treasure->DynamicData.TreasureBox;

	if( Box == NULL )
		return true;
	

	if( Box->TreasurePartyID != -1 && Owner->PartyID() != Box->TreasurePartyID )
		IsNormalLootOK = false;

	if( Box->TreasureOwnerDBID != -1 && Box->TreasureOwnerDBID != Owner->DBID() )
		IsNormalLootOK = false;

	bool IsEmpty = true;
	for( int i = 0 ; i < Box->Count ; i++ )
	{
		if( Box->Item[i].IsEmpty() )
			continue;
		IsEmpty = false;

		if( Box->OwnerDBID[i] == -1 &&  Owner->PartyID() == Box->TreasurePartyID && Owner->PartyID() != -1 )
			return false;

		if(		( Box->OwnerDBID[i] == -1 && IsNormalLootOK != false )
			||	Box->OwnerDBID[i] == Owner->DBID() )
		{
			return false;
		}			

	}

	if( IsEmpty )
	{
		Box->Count = 0;
//		delete Box;
//		Treasure->DynamicData.TreasureBox = NULL;
	}

	return true;
}