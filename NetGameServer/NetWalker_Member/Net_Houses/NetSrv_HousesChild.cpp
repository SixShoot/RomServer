#include "../NetWakerGSrvInc.h"
#include "NetSrv_HousesChild.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
#include "../../mainproc/HousesManage/HousesManageClass.h"
#include "MD5/Mymd5.h"
//--------------------------------------------------------------------------------------------------------------
map< int , HouseServantStruct >	NetSrv_HousesChild::_ServantList;
int		NetSrv_HousesChild::_ServantMaxID = 0;

//--------------------------------------------------------------------------------------------------------------
bool    NetSrv_HousesChild::Init()
{
	NetSrv_Houses::_Init();

	if( This != NULL)
		return false;

	This = NEW( NetSrv_HousesChild );

	Global::Schedular()->Push( _OnTime_UpdateServantHireInfo , 1000 , NULL, NULL );	
	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool    NetSrv_HousesChild::Release()
{
	if( This == NULL )
		return false;

	NetSrv_Houses::_Release();

	delete This;
	This = NULL;

	return true;

}

void NetSrv_HousesChild::RD_BuildHouseResult		( int PlayerDBID , int HouseDBID , bool Result )
{
	RoleDataEx* Player = Global::GetRoleDataByDBID( PlayerDBID );
	if( Player != NULL )
	{
		Player->PlayerBaseData->HouseDBID = HouseDBID;
		SC_BuildHouseResult( Player->GUID() , HouseDBID , Result );

		/*
		if( Result != false )
		{
			SC_BuildHouseResult( Player->GUID() , HouseDBID , Result );

		}
		else
		{
			Print( LV2 , "RD_BuildHouseResult" , "Build House Error , HouseDBID=%d PlayerDBID = %d" , HouseDBID , PlayerDBID );
		}
		*/
	}
}
void NetSrv_HousesChild::RD_FindHouseResult			( int PlayerDBID , int HouseDBID , FindHouseResultENUM Result )
{
	RoleDataEx* Player = Global::GetRoleDataByDBID( PlayerDBID );
	if( Player == NULL )
		return;

	switch( Result )
	{
	case EM_FindHouseResult_OK:
		SC_CloseVisitHouse( Player->GUID() );
		Player->PlayerTempData->VisitHouseDBID = HouseDBID;
		ChangeZone( Player->GUID() , g_ObjectData->GetSysKeyValue( "HouseZoneID" ) , 0 , 0 , 1 , 0 , 0 );

		if( Global::Ini()->IsHouseActionLog == true )
		{//Є±®a¶i¤J¤p«О®Й
			if( Player->PlayerBaseData->HouseDBID != HouseDBID)
				Log_House( Player , HouseDBID , -2 , EM_HouseActionType_IntoHouse , "" );
			else
				Log_House( Player , HouseDBID , Player->DBID() , EM_HouseActionType_IntoHouse , "" );
		}
		break;
	case EM_FindHouseResult_PasswordError:
	case EM_FindHouseResult_NotFind:
	case EM_FindHouseResult_Unknow:
		SC_FindHouseResult( Player->GUID() , Result );
		break;
	}
}
void NetSrv_HousesChild::RD_HouseBaseInfo			( HouseBaseDBStruct& HouseBaseInfo )
{
	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( HouseBaseInfo.HouseDBID );
	if( houseClass == NULL )
		return;

	houseClass->SetHouseBase( HouseBaseInfo );

	SendAllCli_FixHouseBase( houseClass->RoomID() , houseClass->GetHouseBase()->Info );
}
void NetSrv_HousesChild::RD_ItemInfo				( int HouseDBID , int ItemDBID , int ItemCount , HouseItemDBStruct Item[] )
{
	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( HouseDBID );
	if( houseClass == NULL )
		return;

	//і]©wЄ««~ёк®Ж
	for( int i = 0 ; i < ItemCount ; i++ )
	{
		if( houseClass->AddItem( Item[i] ) == false )
		{
			Print( LV5 , "RD_ItemInfo" , "house addItem error!! HouseDBID=%d ItemDBID=%d ParentItemDBID=%d ItemObjID=%d" 
				, HouseDBID 
				, Item[i].ItemDBID  
				, Item[i].ParentItemDBID
				, Item[i].Item.OrgObjID		);
		}
	}
	
	//¦pЄG¤Ј¬O¦a¤@јh¤Ј¶·­nіBІz
	if( ItemDBID != -1 && ItemDBID != -2 )
	{
		HouseItemStruct* houseItem = houseClass->GetItem( ItemDBID );
		if( houseItem == NULL )
			return;

		GameObjDbStructEx* ParentItemDB = Global::GetObjDB( houseItem->Info.Item.OrgObjID );
		if( ParentItemDB == NULL || ParentItemDB->IsItem() == false )
			return;

		if( ParentItemDB->Item.ItemType != EM_ItemType_Furniture )
			return;

		if(		ParentItemDB->Item.emFurnitureType != EM_FurnitureType_Coathanger
			&&	ParentItemDB->Item.emFurnitureType != EM_FurnitureType_WeaponSet
			&&	ParentItemDB->Item.emFurnitureType != EM_FurnitureType_WeaponAndCloth )
			return;
	}
	//іqЄѕ©Т¦іЄє¦іГцЄєЄ±®a
	RoleDataEx *Role;
	set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//­pєвЁC­У©Р¶ЎЄє¤HјЖ
	for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if( Obj == NULL )
			continue;
		Role = Obj->Role();
		if( Role->RoomID() != houseClass->RoomID() )
			continue;
		SC_ItemInfo( Role->GUID() , ItemDBID , ItemCount , Item );
	}


}

void NetSrv_HousesChild::SendCli_FriendDBIDList( int SendID , HousesManageClass* houseClass )
{
	//іqЄѕ©Т¦іЄє¦іГцЄєЄ±®a
	vector< int >&	DbidList = houseClass->FriendDBIDList();

	for( unsigned i = 0 ; i < DbidList.size() ; i++ )
	{
		SC_FriendDBID( SendID , houseClass->HouseBase().Info.HouseDBID , DbidList[i] );
	}
	
}

void NetSrv_HousesChild::RD_HouseInfoLoadOK			( int HouseDBID )
{
	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( HouseDBID );
	if( houseClass == NULL )
		return;

	houseClass->LoadOK();


	//іqЄѕ©Т¦іЄє¦іГцЄєЄ±®a
	RoleDataEx *Role;
	vector< RoleDataEx* > RoleList;

	set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//­pєвЁC­У©Р¶ЎЄє¤HјЖ
	for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if( Obj == NULL )
			continue;
		Role = Obj->Role();
		if( Role->RoomID() != houseClass->RoomID() )
			continue;
		RoleList.push_back( Role );
		SendCli_FriendDBIDList( Role->GUID() , houseClass );
		SC_HouseInfoLoadOK( Role->GUID() );

		if( Role->DBID() == houseClass->GetHouseBase()->Info.OwnerDBID )
			houseClass->GetHouseBase()->Info.UnCheckItemLogCount = 0;
	}

	//АЛ¬d¬O§_¦і¤k№І¤ЈЁЈ
	{
		vector< HouseItemStruct* >& servantItem = houseClass->HouseBase().ItemList[-2];
		map< int , int > servantPosMap;
		for( int i = 0 ; i < servantItem.size() ; i++ )
		{
			if( servantItem[i]->Info.Item.IsEmpty() )
				continue;
			servantPosMap[ servantItem[i]->Info.Pos ] = 1;
		}

		int HouseServantBoxID = g_ObjectData->GetSysKeyValue( "HouseServantBoxID" );

		for( int i = 0 ; i < MAX_SERVANT_COUNT ; i++ )
		{
			HouseServantStruct& servant = houseClass->HouseBase().Info.Servant[i];			
			if( servant.NPCObjID == 0 )
				break;
			//§дҐX№пАі¤k№ІЄєЄ«Ґу
			if( servantPosMap[i] == 0 )
			{
				//§в¤k№ІД_Ѕc­««ШҐXЁУ
				HouseItemDBStruct ServantBox;

				ServantBox.Item.Init();
				ServantBox.Item.OrgObjID = HouseServantBoxID;
				ServantBox.Item.Count = 1;
				ServantBox.Layout.Init();

				ServantBox.HouseDBID = houseClass->HouseBase().Info.HouseDBID;
				ServantBox.ItemDBID = -1;
				ServantBox.ParentItemDBID = -2;
				ServantBox.Pos = i;
				if( houseClass->AddItem( ServantBox ) )
				{
					for( unsigned i = 0 ; i < RoleList.size() ; i++ )
						SC_FixHouseItemProc( RoleList[i] , NULL , ServantBox );
				}
			}
		}
	}
}

void NetSrv_HousesChild::RC_VisitHouseRequest		( BaseItemObject* Obj , int HouseDBID , char* Password )
{
	RoleDataEx* Role = Obj->Role();
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_VisitHouse ) == false )
	{
		Role->Msg( "°СЖ[©Р«Оїпіж©|Ґј¶}±Т!" );
		return;
	}

	SD_FindHouseRequest( Obj->Role()->DBID() , HouseDBID , Password , Obj->Role()->PlayerBaseData->ManageLV );
}
void NetSrv_HousesChild::RC_VisitHouseRequest_ByName( BaseItemObject* Obj , char* RoleName , char* Password )
{
	RoleDataEx* Role = Obj->Role();
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_VisitHouse ) == false )
	{
		Role->Msg( "°СЖ[©Р«Оїпіж©|Ґј¶}±Т!" );
		return;
	}

	SD_FindHouseRequest_ByName( Obj->Role()->DBID() , RoleName , Password , Obj->Role()->PlayerBaseData->ManageLV );
}

void NetSrv_HousesChild::RC_CloseVisitHouse			( BaseItemObject* Obj )
{
	RoleDataEx* Role = Obj->Role();
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_VisitHouse ) == false )
	{
		Role->Msg( "°СЖ[©Р«Оїпіж©|Ґј¶}±Т!" );
		return;
	}

	Role->ClsOpenMenu();
}
void NetSrv_HousesChild::RC_SwapBodyHouseItem		( BaseItemObject* Obj , int HouseParentItemDBID , int HousePos , int BodyPos , bool IsFromBody )
{	
	RoleDataEx* Role = Obj->Role();

	if( Role->TempData.Sys.OnChangeZone != false )
		return;


	if( Role->BaseData.SysFlag.DisableTrade != false )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
		SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
		return;
	}

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerTempData->VisitHouseDBID );
	if( houseClass == NULL )
	{
		Role->Msg("§д¤ЈЁм©Р«Оёк®Ж");
		SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
		return;
	}

	if( houseClass->IsLoadOK() == false )
	{
		Print( LV5 , "RC_SwapBodyHouseItem" , "houseClass->IsLoadOK() == false ©Р«ОБЩЁSёь§№ґN­nЁDіBІzЄ««~ёк®Ж" );
		return;
	}

	HouseItemStruct* houseItem = houseClass->GetItem( HouseParentItemDBID , HousePos );
	if( houseItem == NULL )
	{
		Role->Msg( "ёк®Ж¦і°ЭГD" );
		SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
		return;
	}


	ItemFieldStruct* bodyItem = Role->GetBodyItem( BodyPos );
	if( bodyItem == NULL )
	{
		Role->Msg("Ґ]»qёк®Ж¦мёm¦і°ЭГD");
		SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
		return;
	}


	if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
	{
		//¬d¬Э¬O§_¬°¦n¤Н
		if( houseClass->CheckFriend( Role->DBID() ) == false )
		{
			SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
			Role->Msg("¤Ј¬O¦Ы¤vЄє©Р¤l,¤]«D¦n¤Н");
			return;
		}

		if( houseClass->HouseBase().Info.UnCheckItemLogCount >= MAX_HOUSE_ITEM_LOG_COUNT -1 )
		{
			SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
//			Role->Msg("¦№©Р¤lЄє¦n¤Н¦sЁъЄ««~ЎAҐD¤H¤У¤[ЁS¦і¬d¬Э");
			Role->Net_GameMsgEvent( EM_GameMessageEvent_House_TooManyUnCheckItem );
			return;
		}

		//¬d¬Э¬O§_і]©wҐi¦sЁъ
		HouseItemStruct* ParentHouseItem = houseClass->GetItem( HouseParentItemDBID );
		if(		ParentHouseItem == NULL 
			||	ParentHouseItem->Info.Item.IsEmpty() != false )
		{
			SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
			return;
		}


		if( bodyItem->IsEmpty() == false )
		{
			if( bodyItem->Mode.Trade == false )
			{
				SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
				return;
			}
			
			// TODO: Временный фикс дюпа через сундуки.
			// SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
			// return;			
			if( ParentHouseItem->Info.Layout.IsFriend_Put == false  )
			{
				SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
				return;
			}

		}


		if( houseItem->Info.Item.IsEmpty() == false  )
		{
			if( houseItem->Info.Item.Mode.Trade == false )
			{
				SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
				return;
			}
			
			// TODO: Временный фикс дюпа через сундуки.
			// SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
			// return;			
			if( ParentHouseItem->Info.Layout.IsFriend_Get == false  )
			{
				SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
				return;
			}			
		}
			

	}


	//////////////////////////////////////////////////////////////////////////
	houseClass->CheckAndClearCopyItem( Role );
	//////////////////////////////////////////////////////////////////////////
	//АЛ¬d	
	ItemFieldStruct orgBodyItem = *bodyItem;
	ItemFieldStruct orgHouseItem = houseItem->Info.Item;

	if( houseClass->CheckItemPos( HouseParentItemDBID , HousePos ) == false )
	{
		Role->Msg("¦мёm¶WҐXЅdіт");
		SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
		return;
	}
	
	
	if( houseClass->SwapItem( *bodyItem , HouseParentItemDBID , HousePos , IsFromBody ) == false )
	{
		Role->Msg( "Ґжґ«Ґў±С" );
		SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , false );
		return;
	}
	
	//¦pЄG¬O°СЖ[ЄB¤Н®a¦sЁъЄ««~ јgLog 
	if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
	{
		RoleDataEx* HouseMaster = Global::GetRoleDataByDBID( houseClass->HouseBase().Info.OwnerDBID );

		HouseFriendItemLogStruct TempItemLog;
		TempItemLog.Time = TimeStr::Now_Value();
		TempItemLog.FriendDBID = Role->DBID();

		if( orgBodyItem.IsEmpty()  )
		{
			if(  orgHouseItem.IsEmpty() == false )
			{
				TempItemLog.ItemID = orgHouseItem.OrgObjID;
				TempItemLog.Count = orgHouseItem.Count * -1;
				houseClass->HouseBase().Info.AddItemLog( TempItemLog );
				if( HouseMaster != NULL )
					NetSrv_Houses::SC_FriendItemLog	( HouseMaster->GUID() , TempItemLog );
			}		
		}
		else if( orgHouseItem.IsEmpty() )
		{
			TempItemLog.ItemID = orgBodyItem.OrgObjID;
			TempItemLog.Count = orgBodyItem.Count;
			houseClass->HouseBase().Info.AddItemLog( TempItemLog );
			if( HouseMaster != NULL )
				NetSrv_Houses::SC_FriendItemLog	( HouseMaster->GUID() , TempItemLog );
		}
		else if( orgBodyItem.OrgObjID == orgHouseItem.OrgObjID )
		{
			TempItemLog.ItemID = orgBodyItem.OrgObjID;
			TempItemLog.Count = houseItem->Info.Item.Count - orgHouseItem.Count;
			houseClass->HouseBase().Info.AddItemLog( TempItemLog );
			if( HouseMaster != NULL )
				NetSrv_Houses::SC_FriendItemLog	( HouseMaster->GUID() , TempItemLog );
		}
		else
		{
			TempItemLog.ItemID = orgBodyItem.OrgObjID;
			TempItemLog.Count = orgBodyItem.Count;
			houseClass->HouseBase().Info.AddItemLog( TempItemLog );
			if( HouseMaster != NULL )
				NetSrv_Houses::SC_FriendItemLog	( HouseMaster->GUID() , TempItemLog );

			TempItemLog.ItemID = orgHouseItem.OrgObjID;
			TempItemLog.Count = orgHouseItem.Count * -1;
			houseClass->HouseBase().Info.AddItemLog( TempItemLog );
			if( HouseMaster != NULL )
				NetSrv_Houses::SC_FriendItemLog	( HouseMaster->GUID() , TempItemLog );
		}

		if( HouseMaster != NULL )
		{
			houseClass->HouseBase().Info.UnCheckItemLogCount = 0;
		}

	}
	
	HouseItemStruct* houseItemParent = houseClass->GetItem( HouseParentItemDBID );
	if( Global::Ini()->IsHouseMoveItemLog == true && houseItemParent)
	{//іГ­С­IҐ]Є««~·hІѕ
		char Buf[128];
		if( orgBodyItem.IsEmpty()  )
		{
			if(  orgHouseItem.IsEmpty() == false )
			{
				if( houseItemParent->Info.ParentItemDBID == -2 )
				{
					HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[  houseItemParent->Info.Pos ];
					sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d [%d]_%d To Bag_%d" 
						, orgHouseItem.OrgObjID , orgHouseItem.Count , HouseServantInfo.NPCObjID , HousePos , BodyPos);
				}
				else
				{
					sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d [%d]_%d To Bag_%d" 
						, orgHouseItem.OrgObjID , orgHouseItem.Count , houseItemParent->Info.Item.OrgObjID , HousePos , BodyPos );
				}
				Log_House( Role , houseItem->Info.HouseDBID , houseClass->HouseBase().Info.OwnerDBID , EM_HouseActionType_Item_FurnitureToBag , Buf );
			}		
		}
		else if( orgHouseItem.IsEmpty() )
		{
			if( houseItemParent->Info.ParentItemDBID == -2 )
			{
				HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[  houseItemParent->Info.Pos ];
				sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d Bag_%d To [%d]_%d" 
					, orgBodyItem.OrgObjID , orgBodyItem.Count , BodyPos , HouseServantInfo.NPCObjID , HousePos );
			}
			else
			{
				sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d  Bag_%d To [%d]_%d" 
					, orgBodyItem.OrgObjID , orgBodyItem.Count , BodyPos , houseItemParent->Info.Item.OrgObjID , HousePos );
			}
			Log_House( Role , houseItem->Info.HouseDBID , houseClass->HouseBase().Info.OwnerDBID , EM_HouseActionType_Item_BagToFurniture , Buf );
		}
		else if( orgBodyItem.OrgObjID == orgHouseItem.OrgObjID )
		{
			sprintf_s( Buf , sizeof(Buf) , "path3 %d %d" , orgBodyItem.OrgObjID , houseItem->Info.Item.Count - orgHouseItem.Count );
			//bag->house
			if( houseItem->Info.Item.Count - orgHouseItem.Count > 0 )
			{
				if( houseItemParent->Info.ParentItemDBID == -2 )
				{
					HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[  houseItemParent->Info.Pos ];
					sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d Bag_%d To [%d]_%d" , orgHouseItem.OrgObjID , houseItem->Info.Item.Count - orgHouseItem.Count , BodyPos , HouseServantInfo.NPCObjID , HousePos );
				}
				else
				{
					sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d Bag_%d To [%d]_%d" , orgHouseItem.OrgObjID , houseItem->Info.Item.Count - orgHouseItem.Count , BodyPos , houseItemParent->Info.Item.OrgObjID , HousePos );
				}
				Log_House( Role , houseItem->Info.HouseDBID , houseClass->HouseBase().Info.OwnerDBID , EM_HouseActionType_Item_BagToFurniture , Buf );
			}
			else
			{
				if( houseItemParent->Info.ParentItemDBID == -2 )
				{
					HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[  houseItemParent->Info.Pos ];
					sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d [%d]_%d To Bag_%d" , orgBodyItem.OrgObjID , orgHouseItem.Count - houseItem->Info.Item.Count , HouseServantInfo.NPCObjID , HousePos , BodyPos );
				}
				else
				{
					sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d [%d]_%d To Bag_%d" , orgBodyItem.OrgObjID , orgHouseItem.Count - houseItem->Info.Item.Count , houseItemParent->Info.Item.OrgObjID , HousePos , BodyPos );
				}
				Log_House( Role , houseItem->Info.HouseDBID , houseClass->HouseBase().Info.OwnerDBID , EM_HouseActionType_Item_FurnitureToBag , Buf );
			}	
		}
		else
		{
			if( houseItemParent->Info.ParentItemDBID == -2 )
			{
				HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[  houseItemParent->Info.Pos ];
				sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d [%d]_%d To Bag_%d" 
					, orgHouseItem.OrgObjID , orgHouseItem.Count , HouseServantInfo.NPCObjID , HousePos , BodyPos );
			}
			else
			{
				sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d [%d]_%d To Bag_%d" 
					, orgHouseItem.OrgObjID , orgHouseItem.Count , houseItemParent->Info.Item.OrgObjID , HousePos , BodyPos );
			}
			Log_House( Role , houseItem->Info.HouseDBID , houseClass->HouseBase().Info.OwnerDBID , EM_HouseActionType_Item_FurnitureToBag , Buf );

			if( houseItemParent->Info.ParentItemDBID == -2 )
			{
				HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[  houseItemParent->Info.Pos ];
				sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d  Bag_%d To [%d]_%d " 
					, orgBodyItem.OrgObjID , orgBodyItem.Count , BodyPos , HouseServantInfo.NPCObjID , HousePos );
			}
			else
			{
				sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d  Bag_%d To [%d]_%d " 
					, orgBodyItem.OrgObjID , orgBodyItem.Count , BodyPos , houseItemParent->Info.Item.OrgObjID , HousePos );
			}
			Log_House( Role , houseItem->Info.HouseDBID , houseClass->HouseBase().Info.OwnerDBID , EM_HouseActionType_Item_BagToFurniture , Buf );
		}
	}
	//ёк®Ж­ЧҐї
	SC_FixHouseItemProc( Role , houseClass->GetItem( HouseParentItemDBID ) , houseItem->Info );
	Role->Net_FixItemInfo_Body( BodyPos );
	SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HousePos , HouseParentItemDBID , true );
	// TODO: Добавил для фикса дюпа через дом
	houseClass->CheckAndClearCopyItem( Role );
}



void NetSrv_HousesChild::RC_SwapHouseItemRequest	( BaseItemObject* Obj , int HouseParentItemDBID[2] , int HousePos[2] )
{
	RoleDataEx* Role = Obj->Role();


	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerBaseData->HouseDBID );
	if( houseClass == NULL )
	{
		SC_SwapHouseItemResult	( Role->GUID() , HouseParentItemDBID[0] , HousePos[0] , HouseParentItemDBID[1] , HousePos[1] , false );	
		Role->Msg("§д¤ЈЁм©Р«Оёк®Ж");
		return;
	}

	if( houseClass->IsLoadOK() == false )
	{
		Print( LV5 , "RC_SwapHouseItemRequest" , "houseClass->IsLoadOK() == false ©Р«ОБЩЁSёь§№ґN­nЁDіBІzЄ««~ёк®Ж" );
		return;
	}

	if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
	{
		if( houseClass->CheckFriend( Role->DBID() ) == false )
		{
			Role->Msg("¤Ј¬O¦Ы¤vЄє©Р¤l,¤]«D¦n¤Н");
			SC_SwapHouseItemResult	( Role->GUID() , HouseParentItemDBID[0] , HousePos[0] , HouseParentItemDBID[1] , HousePos[1] , false );	
			return;
		}

		HouseItemStruct* ParentHouseItem1 = houseClass->GetItem( HouseParentItemDBID[0] );
		HouseItemStruct* ParentHouseItem2 = houseClass->GetItem( HouseParentItemDBID[1] );

		if( ParentHouseItem1 == NULL || ParentHouseItem2 == NULL )
		{
			SC_SwapHouseItemResult	( Role->GUID() , HouseParentItemDBID[0] , HousePos[0] , HouseParentItemDBID[1] , HousePos[1] , false );	
			return;
		}

		// TODO: Временный фикс дюпа через сундуки.
		// SC_SwapHouseItemResult	( Role->GUID() , HouseParentItemDBID[0] , HousePos[0] , HouseParentItemDBID[1] , HousePos[1] , false );	
		// return;		
		if( ParentHouseItem1->Info.Layout.IsFriend_Put == false ||  ParentHouseItem2->Info.Layout.IsFriend_Put == false )
		{
			SC_SwapHouseItemResult	( Role->GUID() , HouseParentItemDBID[0] , HousePos[0] , HouseParentItemDBID[1] , HousePos[1] , false );	
			return;
		}
	}

	if( HousePos[0] == HousePos[1] && HouseParentItemDBID[0] == HouseParentItemDBID[1] )
	{
		SC_SwapHouseItemResult	( Role->GUID() , HouseParentItemDBID[0] , HousePos[0] , HouseParentItemDBID[1] , HousePos[1] , false );	
		Role->Msg( "¤Ј»Э·hІѕ" );
		return;
	}

	if( houseClass->CheckItemPos( HouseParentItemDBID[0] , HousePos[0] ) == false || houseClass->CheckItemPos( HouseParentItemDBID[1] , HousePos[1] ) == false )
	{
		SC_SwapHouseItemResult	( Role->GUID() , HouseParentItemDBID[0] , HousePos[0] , HouseParentItemDBID[1] , HousePos[1] , false );	
		Role->Msg( "¶W№LЅdіт" );
		return;
	}


	if( houseClass->SwapItem( HouseParentItemDBID[0] , HousePos[0] , HouseParentItemDBID[1] , HousePos[1] ) == false)
	{
		SC_SwapHouseItemResult	( Role->GUID() , HouseParentItemDBID[0] , HousePos[0] , HouseParentItemDBID[1] , HousePos[1] , false );	
		Role->Msg( "Є««~·hІѕҐў±С" );
		return;
	}

	


	HouseItemStruct* houseItem1 = houseClass->GetItem(  HouseParentItemDBID[0] ,  HousePos[0] );	
	SC_FixHouseItemProc( Role , houseClass->GetItem( HouseParentItemDBID[0] ) , houseItem1->Info );
	HouseItemStruct* houseItem2 = houseClass->GetItem(  HouseParentItemDBID[1] ,  HousePos[1] );
	SC_FixHouseItemProc( Role , houseClass->GetItem( HouseParentItemDBID[1] ) , houseItem2->Info );

	SC_SwapHouseItemResult	( Role->GUID() , HouseParentItemDBID[0] , HousePos[0] , HouseParentItemDBID[1] , HousePos[1] , true );	
}

void NetSrv_HousesChild::RC_HouseItemLayoutRequest	( BaseItemObject* Obj , int ItemDBID , House3DLayoutStruct& Layout )
{
	RoleDataEx* Role = Obj->Role();

	if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
	{
		Role->Msg("¤Ј¬O¦Ы¤vЄє©Р¤l");
		return;
	}

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerBaseData->HouseDBID );
	if( houseClass == NULL )
	{
		SC_HouseItemLayoutResult	( Role->GUID() , ItemDBID , false );	
		Role->Msg("§д¤ЈЁм©Р«Оёк®Ж");
		return;
	}
	HouseItemStruct* houseItem = houseClass->GetItem( ItemDBID );	
	if( houseItem == NULL || houseItem->SaveState == EM_HouseItemSaveState_Empty )
	{
		SC_HouseItemLayoutResult	( Role->GUID() , ItemDBID , false );	
		Role->Msg("§д¤ЈЁм©Р«ОЄ««~");
		return;
	}
	houseClass->SetSaveInfo();
	houseItem->Info.Layout = Layout;

	if( houseItem->SaveState == EM_HouseItemSaveState_Normal )
		houseItem->SaveState = EM_HouseItemSaveState_Update;

	houseClass->SetSaveInfo();

	if( Global::Ini()->IsHouseActionLog == true )
	{//®aЁг¦мёmВ\і]
		char Buf[128];
		int OrgObjID;
		if( houseItem->Info.ParentItemDBID == -2 )
		{
			HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[ houseItem->Info.Pos ];
			OrgObjID = HouseServantInfo.NPCObjID;
		}
		else
		{
			OrgObjID = houseItem->Info.Item.OrgObjID;
		}

		if( houseItem->Info.Layout.IsShow )
		{
			sprintf_s( Buf , sizeof(Buf) , "[%d] X: %f Y: %f Z: %f DIR: %f " , OrgObjID , houseItem->Info.Layout.X, houseItem->Info.Layout.Y,houseItem->Info.Layout.Z,houseItem->Info.Layout.Angle);
			Log_House( Role , houseItem->Info.HouseDBID , Role->DBID() , EM_HouseActionType_Furniture_Appear , Buf);
		}
		else
		{
			sprintf_s( Buf , sizeof(Buf) , "[%d] X: %f Y: %f Z: %f DIR: %f " , OrgObjID , houseItem->Info.Layout.X, houseItem->Info.Layout.Y,houseItem->Info.Layout.Z,houseItem->Info.Layout.Angle);
			Log_House( Role , houseItem->Info.HouseDBID , Role->DBID() , EM_HouseActionType_Furniture_Disappear , Buf);
		}
		Role->Msg( Buf );
	}

	SC_HouseItemLayoutResult	( Role->GUID() , ItemDBID , true );	
	SC_FixHouseItemProc( Role , NULL , houseItem->Info );
}
void NetSrv_HousesChild::RC_ChangeHouseRequest		( BaseItemObject* Obj , int HouseType )
{

}
void NetSrv_HousesChild::RC_BuyFunctionRequest		( BaseItemObject* Obj , int FunctionID )
{
	RoleDataEx* Role = Obj->Role();
	if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
	{
		Role->Msg("¤Ј¬O¦Ы¤vЄє©Р¤l");
		return;
	}

	
	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerBaseData->HouseDBID );
	if( houseClass == NULL )
	{
		SC_BuyFunctionResult( Role->GUID() , FunctionID , false );
		Role->Msg("§д¤ЈЁм©Р«Оёк®Ж");
		return;
	}

	
	HouseBaseDBStruct& houseBase = houseClass->HouseBase().Info;

	if( houseBase.ShopFunction.GetFlag( FunctionID) != false )
	{
		SC_BuyFunctionResult( Role->GUID() , FunctionID , false );
		Role->Msg("ёк®Ж¤w¦s¦b");
		return;
	}

	vector<HouseShopInfoStruct>&	houseShopInfo = g_ObjectData->HouseShop();
	
	if( (unsigned)FunctionID >= houseShopInfo.size() || FunctionID >= _MAX_HOUSES_SHOPFUNCTION_COUNT_  )
	{
		SC_BuyFunctionResult( Role->GUID() , FunctionID , false );
		Role->Msg("ёк®Ж¦і°ЭГD");
		return;
	}

	//Ёъ±oёк®Ж©Р«О°У©±ёк°T
	HouseShopInfoStruct houseShopFunc = houseShopInfo[ FunctionID ];

	int		OrgOpenEnergy = 0;
	//­pєвБК¶RЄє®ж¤l©Т¶·­nЄє»щїъ
	for( unsigned i = 0 ; i < houseShopInfo.size() ; i++ )
	{
		if( houseShopInfo[i].Type != 0 )
			continue;
		if( houseBase.ShopFunction.GetFlag( i ) == false )
			continue;

		OrgOpenEnergy += houseShopInfo[i].OpenEnergy;
	}

	int OpenEnergy = houseShopFunc.OpenEnergy -  OrgOpenEnergy;
	//Їа¶qАЛ¬d
	if( houseBase.EnergyPoint < OpenEnergy )
	{
		SC_BuyFunctionResult( Role->GUID() , FunctionID , false );
		Role->Msg("Їа¶q¤ЈЁ¬");
		return;
	}

	//ІM°Ј¤§«eЄєЇІҐОёк®Ж
	for( unsigned i = 0 ; i < houseShopInfo.size() ; i++ )
	{
		if( houseShopInfo[i].Type != 0 )
			continue;
		if( houseBase.ShopFunction.GetFlag( i ) == false )
			continue;
		houseBase.ShopFunction.SetFlagOff( i );
			
		houseBase.MaxItemCount -= houseShopInfo[i].FunitureCount;		
	}

	if( houseBase.ShopFunction.SetFlagOn( FunctionID ) != false )
	{
		if( OpenEnergy > 0 )
			houseBase.EnergyPoint -= OpenEnergy;
		houseBase.MaxItemCount += houseShopFunc.FunitureCount;
		
		if( Global::Ini()->IsHouseActionLog == true )
		{//Єб¶OЇа¶qЇІҐОЄЕ¶ЎЇа¶q
			char Buf[128];
			sprintf_s( Buf , sizeof(Buf) , "Cost %d Space become %d " , OpenEnergy < 0 ? 0 : OpenEnergy , houseBase.MaxItemCount );
			Log_House( Role , Role->PlayerBaseData->HouseDBID , Role->DBID() , EM_HouseActionType_SpaceIntro , Buf );
		}

		SC_BuyFunctionResult( Role->GUID() , FunctionID , true );
		SendAllCli_FixHouseBase( Role->RoomID() , houseBase );
		houseClass->SetSaveInfo();
	}
	else
	{
		SC_BuyFunctionResult( Role->GUID() , FunctionID , false );
	}



}
void NetSrv_HousesChild::RC_FunctionSettingRequest	( BaseItemObject* Obj , int FunctionID , int Mode )
{
	RoleDataEx* Role = Obj->Role();
	if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
	{
		Role->Msg("¤Ј¬O¦Ы¤vЄє©Р¤l");
		return;
	}

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerBaseData->HouseDBID );
	if( houseClass == NULL )
	{
		SC_FunctionSettingResult( Role->GUID() , FunctionID , 0 , false );
		Role->Msg("§д¤ЈЁм©Р«Оёк®Ж");
		return;
	}


	HouseBaseDBStruct& houseBase = houseClass->HouseBase().Info;

	if( houseBase.ShopFunction.GetFlag( FunctionID) == false )
	{
		SC_FunctionSettingResult( Role->GUID() , FunctionID , 0 , false );
		Role->Msg("ёк®Ж¤Ј¦s¦b");
		return;
	}
	vector<HouseShopInfoStruct>&	houseShopInfo = g_ObjectData->HouseShop();

	if( (unsigned)FunctionID >= houseShopInfo.size() )
	{
		SC_FunctionSettingResult( Role->GUID() , FunctionID , 0 , false );
		Role->Msg("ёк®Ж¦і°ЭГD");
		return;
	}

	//Ёъ±oёк®Ж©Р«О°У©±ёк°T
	HouseShopInfoStruct houseShopFunc = houseShopInfo[ FunctionID ];

	switch( Mode )
	{
	case 0:
		{
			if( houseBase.ShopFunction.SetFlagOff( FunctionID ) != false )
			{
				houseBase.MaxItemCount -= houseShopFunc.FunitureCount;			
				SC_FunctionSettingResult( Role->GUID() , FunctionID , 0 , true );
				SendAllCli_FixHouseBase( Role->RoomID() , houseBase );
			}
			else
				SC_FunctionSettingResult( Role->GUID() , FunctionID , 0 , false );

		}
		break;
	default:
			SC_FunctionSettingResult( Role->GUID() , FunctionID , 0 , false );
		break;
	}

}
void NetSrv_HousesChild::RC_BuyEnergyRequest		( BaseItemObject* Obj , int Money_Account , const char* Password )
{
	RoleDataEx* Role = Obj->Role();
	if( Role->TempData.Sys.OnChangeZone != false )
		return;

	if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
	{
		Role->Msg("¤Ј¬O¦Ы¤vЄє©Р¤l");
		SC_BuyEnergyResult( Role->GUID() , Money_Account , 0 , false );
		return;
	}

	//АЛ¬d¤G¦ё±KЅX
	MyMD5Class Md5;
	Md5.ComputeStringHash( Password );
	if( stricmp( Md5.GetMd5Str() , Role->PlayerBaseData->Password.Begin() ) != 0 )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_SecondPasswordError );
		SC_BuyEnergyResult( Role->GUID() , Money_Account , 0 , false );
		return;
	}

	//§д¬O§_¦і№пАіЄє»щїъҐiҐH¶R
	vector<HouseEnergyCostStruct>&		EnergyCostList = g_ObjectData->HouseEnergyCost();
	int EnertyPoint = 0;
	for( unsigned i = 0 ; i < EnergyCostList.size() ; i++ )
	{
		if( EnergyCostList[i].AccountMoneyCost == Money_Account )
		{
			EnertyPoint = EnergyCostList[i].HouseEnergy;
			break;
		}
	}
	if( EnertyPoint == 0 )
	{
		Role->Msg("БК¶RҐў±С");
		SC_BuyEnergyResult( Role->GUID() , Money_Account , 0 , false );
		return;
	}


	if( Money_Account < 0 || Role->PlayerBaseData->Body.Money_Account < Money_Account )
	{
		SC_BuyEnergyResult( Role->GUID() , Money_Account , 0 , false );
		return;
	}

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerBaseData->HouseDBID );
	if( houseClass == NULL )
	{
		SC_BuyEnergyResult( Role->GUID() , Money_Account , 0 , false );
		Role->Msg("§д¤ЈЁм©Р«Оёк®Ж");
		return;
	}

	if (Global::AccountMoneyOperable() == false)
	{
		SC_BuyEnergyResult( Role->GUID() , Money_Account , 0 , false );
		return;
	}

	Role->AddBodyMoney_Account( Money_Account*-1 , NULL , EM_ActionType_BuyHouseEnergy , true , true );

	HousesInfoStruct& HouseBase =houseClass->HouseBase();
	HouseBase.IsNeedSave = true;
	HouseBase.Info.EnergyPoint += EnertyPoint; 
	houseClass->SetSaveInfo();
	
	if( Global::Ini()->IsHouseActionLog == true )
	{//Єб¶Oґ№ЖpБК¶RЇа¶q
		char Buf[128];
		sprintf_s( Buf , sizeof(Buf) , "Cost %d To Buy Energy %d " , Money_Account , EnertyPoint );
		Log_House( Role , Role->PlayerBaseData->HouseDBID , Role->DBID() , EM_HouseActionType_EnergyIntro , Buf );
	}

	SC_BuyEnergyResult( Obj->GUID() , Money_Account , EnertyPoint , true );
	SendAllCli_FixHouseBase( Role->RoomID() , HouseBase.Info );

}
void NetSrv_HousesChild::RC_LeaveHouse				( BaseItemObject* Obj , int PlayerDBID )
{
	RoleDataEx* Role = Obj->Role();
	if( Ini()->IsHouseZone == false )
		return;

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerTempData->VisitHouseDBID );
	if( houseClass != NULL )
	{
//		if( PlayerDBID != Role->DBID() && stricmp( houseClass->HouseBase().Info.AccountID.Begin() , Role->Account_ID() ) != 0 )
		if( PlayerDBID != Role->DBID() && PlayerDBID != houseClass->HouseBase().Info.OwnerDBID )
		{
			Role->Msg("ЁS¦іЕv­­Ѕр°ЈЁдҐLЄ±®a");
			return;
		}
	}

	RoleDataEx* Target = Global::GetRoleDataByDBID( PlayerDBID );
	if( Target == NULL )
	{
		Role->Msg("ҐШјР¤Ј¦s¦b");
		return;
	}
	
	int ZoneID = GlobalBase::GetParallelZoneID( Target->SelfData.ReturnZoneID ,  Target->PlayerTempData->ParallelZoneID );
	if( ZoneID == -1 )
		return;
	
	if( Global::Ini()->IsHouseActionLog == true )
	{//Вч¶}¤p«О
		Log_House( Role , houseClass->HouseBase().Info.HouseDBID , houseClass->HouseBase().Info.OwnerDBID , EM_HouseActionType_LeaveHouse , "" );
	}

	Global::ChangeZone( Role->GUID() , ZoneID , 0 , Target->SelfData.ReturnPos.X , Target->SelfData.ReturnPos.Y , Target->SelfData.ReturnPos.Z , Target->SelfData.ReturnPos.Dir );
	Target->SelfData.ReturnZoneID = 0;
	Target->SelfData.ReturnPos.X = 0;
	Target->SelfData.ReturnPos.Y = 0;
	Target->SelfData.ReturnPos.Z = 0;
	Target->SelfData.ReturnPos.Dir = 0;

}

void NetSrv_HousesChild::RC_BoxInfoRequset			( BaseItemObject* Obj , int BoxItemDBID )
{
	RoleDataEx* Role = Obj->Role();
	if( Ini()->IsHouseZone == false )
	{
		SC_BoxInfoResult( Role->GUID() , false );
		return;
	}

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerTempData->VisitHouseDBID );
	if( houseClass == NULL )
	{
		SC_BoxInfoResult( Role->GUID() , false );
		return;
	}

	HouseItemStruct* houseItem = houseClass->GetItem( BoxItemDBID );	
	if( houseItem == NULL )
	{
		SC_BoxInfoResult( Role->GUID() , false );
		return;
	} 

	if( Role->PlayerTempData->VisitHouseDBID != Role->PlayerBaseData->HouseDBID )
	{
		if( houseClass->CheckFriend( Role->DBID() ) == false )
		{
			GameObjDbStructEx* BoxDB = g_ObjectData->GetObj( houseItem->Info.Item.OrgObjID );
			if( BoxDB == NULL || BoxDB->Item.emFurnitureType == EM_FurnitureType_Storage )
			{
				SC_BoxInfoResult( Role->GUID() , false );	
				return;
			}
		}
	}	

	vector<HouseItemDBStruct> ItemList;
	for( unsigned i = 0 ; i < houseItem->ChildList.size() ; i++)
		ItemList.push_back( houseItem->ChildList[i]->Info );

	SC_ItemInfo( Role->GUID() , BoxItemDBID , ItemList );

	SC_BoxInfoResult( Role->GUID() , true );
}

void NetSrv_HousesChild::RC_ExchangeEq				( BaseItemObject* Obj , int BoxItemDBID , int BoxPos , EQWearPosENUM EqPos )
{

	RoleDataEx* Role = Obj->Role();

	if( Role->TempData.Sys.OnChangeZone != false )
		return;


	if( Role->BaseData.SysFlag.DisableTrade != false )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
		SC_ExchangeEqResult( Role->GUID() , false );
		return;
	}

	if( Ini()->IsHouseZone == false || BoxItemDBID == -1 )
	{
		SC_ExchangeEqResult( Role->GUID() , false );
		return;
	}

/*	if( Role->PlayerTempData->VisitHouseDBID != Role->PlayerBaseData->HouseDBID )
	{
		SC_ExchangeEqResult( Role->GUID() , false );
		return;
	}
*/
	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerTempData->VisitHouseDBID );
	if( houseClass == NULL )
	{
		SC_ExchangeEqResult( Role->GUID() , false );
		return;
	}

	if( houseClass->IsLoadOK() == false )
	{
		Print( LV5 , "RC_ExchangeEq" , "houseClass->IsLoadOK() == false ©Р«ОБЩЁSёь§№ґN­nЁDіBІzЄ««~ёк®Ж" );
		return;
	}


	HouseItemStruct* houseItem = houseClass->GetItem( BoxItemDBID , BoxPos );	
	if( houseItem == NULL )
	{
		SC_ExchangeEqResult( Role->GUID() , false );
		return;
	}
	
	if( (unsigned int)EqPos >= EM_EQWearPos_MaxCount )
	{
		SC_ExchangeEqResult( Role->GUID() , false );
		return;
	}

	ItemFieldStruct* bodyItem = Role->GetEqItem( EqPos );
	if( bodyItem == NULL )
		return;

	if( houseItem->Info.Item.IsEmpty() != false 
		&& ( EqPos == EM_EQWearPos_MagicTool1 || EqPos == EM_EQWearPos_MagicTool2 || EqPos == EM_EQWearPos_MagicTool3  ) )
	{
		SC_ExchangeEqResult( Role->GUID() , false );
		return;
	}

	if( houseClass->CheckItemPos( BoxItemDBID , BoxPos ) == false )
	{
		SC_ExchangeEqResult( Role->GUID() , false );
		return;
	}

	if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
	{
		Role->Msg("¤Ј¬O¦Ы¤vЄє©Р¤l");
		//¬d¬Э¬O§_¬°¦n¤Н
		if( houseClass->CheckFriend( Role->DBID() ) == false )
			return;

		//¬d¬Э¬O§_і]©wҐi¦sЁъ
		HouseItemStruct* ParentHouseItem = houseClass->GetItem( BoxItemDBID );
		if( ParentHouseItem == NULL || ParentHouseItem->Info.Item.IsEmpty() == false )
		{
			SC_ExchangeEqResult( Role->GUID() , false );
			return;
		}

		if( bodyItem->IsEmpty() == false )
		{
			if( bodyItem->Mode.Trade == false )
			{
				SC_ExchangeEqResult( Role->GUID() , false );
				return;
			}
			
			// TODO: Временный фикс дюпа через сундуки.
			// SC_ExchangeEqResult( Role->GUID() , false );
			// return;
			if( ParentHouseItem->Info.Layout.IsFriend_Put == false  )
			{
				SC_ExchangeEqResult( Role->GUID() , false );
				return;
			}

		}


		if( houseItem->Info.Item.IsEmpty() == false  )
		{
			if( houseItem->Info.Item.Mode.Trade == false )
			{
				SC_ExchangeEqResult( Role->GUID() , false );
				return;
			}

			// TODO: Временный фикс дюпа через сундуки.
			// SC_ExchangeEqResult( Role->GUID() , false );
			// return;
			if( ParentHouseItem->Info.Layout.IsFriend_Get == false  )
			{
				SC_ExchangeEqResult( Role->GUID() , false );
				return;
			}			
		}


	}



	GameObjDbStructEx*	BodyOrgDB;
	BodyOrgDB   = GetObjDB( houseItem->Info.Item.OrgObjID );
	switch( Role->CheckSetEQ( EqPos , BodyOrgDB ,  houseItem->Info.Item.Mode.IgnoreLimitLv , houseItem->Info.Item.ImageObjectID ) )
	{
	case EM_CheckSetEQResult_OK:
		{
			ItemFieldStruct& EqItem = Role->BaseData.EQ.Item[ EqPos ];
			////////////////////////////////////////////////////////////////////
			//АЛ¬d№П§Оёк®Ж¬O§_ІЕ¦X
			if( BodyOrgDB != NULL )
			{
				GameObjDbStructEx*	ImageDB = GetObjDB( houseItem->Info.Item.ImageObjectID );
				if( ImageDB != NULL )
				{
					switch( ImageDB->Image.LimitSex._Sex )
					{
					case 1:
						if( Role->BaseData.Sex != EM_Sex_Boy )
							return SC_ExchangeEqResult( Role->GUID() , false );
						break;
					case 2:
						if( Role->BaseData.Sex != EM_Sex_Girl )
							return SC_ExchangeEqResult( Role->GUID() , false );						
						break;
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
			//ёЛіЖ«OЕ@јЖ¶qАЛ¬d
			if( houseItem->Info.Item.Mode.PkProtect != false && EqItem.Mode.PkProtect == false )
			{
				int Count = 0;
				for( int i = 0 ; i < EM_EQWearPos_MaxCount ; i++ )
				{
					if( Role->BaseData.EQ.Item[i].Mode.PkProtect != false ) 
						Count++;
				}
				if( Count >= 8 )
				{
					Role->Net_GameMsgEvent( EM_GameMessageEvent_EQError_PKProtectCountError );
					SC_ExchangeEqResult( Role->GUID() , false );
					return ;
				}

			}

			//////////////////////////////////////////////////////////////////////////
			HouseItemStruct* ParentHouseItem = houseClass->GetItem( BoxItemDBID );
			if( houseItem->SaveState != EM_HouseItemSaveState_Empty &&  EqItem.IsEmpty() == false )
			{//ЁвГдіЈ¦іЄF¦и
				if(		EqPos == EM_EQWearPos_MagicTool1 
					||	EqPos == EM_EQWearPos_MagicTool2 
					||	EqPos == EM_EQWearPos_MagicTool3	)
					Role->BaseData.EQ.Item[ EqPos ].Init();
				if( Global::Ini()->IsHouseMoveItemLog == true && houseItem )
				{
					char Buf[128];
					sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d [%d]_%d To EQ_%d" ,  houseItem->Info.Item.OrgObjID ,  houseItem->Info.Item.Count , ParentHouseItem->Info.Item.OrgObjID , BoxPos , EqPos );
					Log_House( Role , houseItem->Info.HouseDBID , houseClass->GetHouseBase()->Info.OwnerDBID , EM_HouseActionType_Item_FurnitureToEq , Buf );


					sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d EQ_%d To [%d]_%d" ,  Role->BaseData.EQ.Item[ EqPos ].OrgObjID ,  Role->BaseData.EQ.Item[ EqPos ].Count , EqPos , ParentHouseItem->Info.Item.OrgObjID , BoxPos );
					Log_House( Role , houseItem->Info.HouseDBID , houseClass->GetHouseBase()->Info.OwnerDBID , EM_HouseActionType_Item_EqToFurniture , Buf );
				}

				std::swap( houseItem->Info.Item , Role->BaseData.EQ.Item[ EqPos ] );				
			
				if( houseItem->SaveState == EM_HouseItemSaveState_Normal )
					houseItem->SaveState = EM_HouseItemSaveState_Update;

				houseClass->SetSaveInfo();
			}
			else if( houseItem->SaveState != EM_HouseItemSaveState_Empty )
			{//©Р«О¤є¦іЄF¦и
				if( Global::Ini()->IsHouseMoveItemLog == true && houseItem )
				{
					char Buf[128];
					sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d [%d]_%d To EQ_%d" ,  houseItem->Info.Item.OrgObjID ,  houseItem->Info.Item.Count , ParentHouseItem->Info.Item.OrgObjID , BoxPos , EqPos );
					Log_House( Role , houseItem->Info.HouseDBID , houseClass->GetHouseBase()->Info.OwnerDBID , EM_HouseActionType_Item_FurnitureToEq , Buf );
				}
				EqItem = houseItem->Info.Item;
				houseClass->DelItem( houseItem->Info.ItemDBID );
			}
			else 
			{//Ё­¤W¦іЄF¦и
				if( Global::Ini()->IsHouseMoveItemLog == true && houseItem )
				{
					char Buf[128];
					sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d EQ_%d To [%d]_%d" , EqItem.OrgObjID ,  EqItem.Count , EqPos , ParentHouseItem->Info.Item.OrgObjID , BoxPos );
					Log_House( Role , houseItem->Info.HouseDBID , houseClass->GetHouseBase()->Info.OwnerDBID , EM_HouseActionType_Item_EqToFurniture , Buf );
				}
				houseItem->Info.Item = EqItem;
				houseItem->Info.ItemDBID = -1;
				EqItem.Init();
				houseClass->AddItem( houseItem->Info );
			}

			Role->TempData.UpdateInfo.Eq   = true;

			SC_FixHouseItemProc( Role , houseClass->GetItem( BoxItemDBID )  , houseItem->Info );
			Role->Net_FixItemInfo_EQ( EqPos );

			SC_ExchangeEqResult( Role->GUID() , true );

			return;
		}
		break;
	default:
		{
			SC_ExchangeEqResult( Role->GUID() , false );
			return ;
		}
		break;
	}

	SC_ExchangeEqResult( Role->GUID() , false );

}
void NetSrv_HousesChild::RC_DestroyItemRequest		( BaseItemObject* Obj , int ItemDBID )
{
	RoleDataEx* Role = Obj->Role();

	if( Role->TempData.Sys.OnChangeZone != false )
		return;


	if( Role->BaseData.SysFlag.DisableTrade != false )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
		SC_DestroyItemResult( Role->GUID() , false );
		return;
	}

	if( Ini()->IsHouseZone == false )
	{
		SC_DestroyItemResult( Role->GUID() , false );
		return;
	}

	if( Role->PlayerTempData->VisitHouseDBID != Role->PlayerBaseData->HouseDBID )
	{
		SC_DestroyItemResult( Role->GUID() , false );
		return;
	}

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerTempData->VisitHouseDBID );
	if( houseClass == NULL )
	{
		SC_DestroyItemResult( Role->GUID() , false );
		return;
	}

	if( houseClass->IsLoadOK() == false )
	{
		Print( LV5 , "RC_DestroyItemRequest" , "houseClass->IsLoadOK() == false ©Р«ОБЩЁSёь§№ґN­nЁDіBІzЄ««~ёк®Ж" );
		return;
	}


	HouseItemStruct* houseItem = houseClass->GetItem( ItemDBID );	
	if( houseItem == NULL )
	{
		SC_DestroyItemResult( Role->GUID() , false );
		return;
	}

	int	ParentItemDBID = houseItem->Info.ParentItemDBID;


	if( houseItem->Info.Item.IsEmpty() != false || houseItem->SaveState == EM_HouseItemSaveState_Empty )
	{
		SC_DestroyItemResult( Role->GUID() , false );
		return;
	}
	bool Result = houseClass->DelItem( ItemDBID );

	if( Result != false )
	{
		SC_FixHouseItemProc( Role , houseClass->GetItem( ParentItemDBID )  , houseItem->Info );
	}

	SC_DestroyItemResult( Role->GUID() , Result );
}

void NetSrv_HousesChild::RC_SetPassword				( BaseItemObject* Obj , const char* Password )
{
	RoleDataEx *Role = Obj->Role();
	if (Ini()->IsHouseZone == false)
	{
		SC_SetPasswordResult(Role->GUID(), false);
		return;
	}
	if (Role->PlayerTempData->VisitHouseDBID != Role->PlayerBaseData->HouseDBID)
	{
		SC_SetPasswordResult(Role->GUID(), false);
		return;
	}
	HousesManageClass *houseClass = HousesManageClass::GetHouseObj(Role->PlayerTempData->VisitHouseDBID);
	if (houseClass == NULL)
	{
		SC_SetPasswordResult(Role->GUID(), false);
		return;
	}
	if (Global::Ini()->IsHouseActionLog == true)
	{ //�ק�p�αK�X
		char Buf[128];
		sprintf_s(Buf, sizeof(Buf), "Old:%s  New:%s", (char *)houseClass->HouseBase().Info.Password.Begin(), Password);
		Log_House(Role, Role->PlayerBaseData->HouseDBID, Role->DBID(), EM_HouseActionType_ChangePassWord, Buf);
	}
	houseClass->HouseBase().Info.Password = Password;
	string CmdBinStr = StringToSqlX(Password, 32, false);
	char Buf[512];
	sprintf(Buf, "UPDATE HOUSES_BASE SET Password = CAST( %s AS varchar(31) ) WHERE HouseDBID = %d", CmdBinStr.c_str(), Role->PlayerBaseData->HouseDBID);
	Net_DCBase::SqlCommand(Role->PlayerBaseData->HouseDBID, Buf);
	SC_SetPasswordResult(Role->GUID(), true);
	return;
}

void NetSrv_HousesChild::SC_FixHouseItemProc			( RoleDataEx* Owner , HouseItemStruct*  HouseParentItem , HouseItemDBStruct& HouseItem , int RoomID )
{
	//bool IsSendAll = false;
//	HouseItemStruct* ParentHouseItem = houseClass->GetItem( HouseParentItemDBID );
/*
	if( HouseParentItem == NULL )
		IsSendAll = true;
	else 
	{
		GameObjDbStructEx* ParentItemDB = Global::GetObjDB( HouseParentItem->Info.Item.OrgObjID );
		if( ParentItemDB->IsItem() && ParentItemDB->Item.emFurnitureType != EM_FurnitureType_Storage )
			IsSendAll = true;
	}
	*/
	bool IsSendAll = true;
	if( Owner != NULL && RoomID == -1 )
		RoomID = Owner->RoomID();

	if( IsSendAll != false )
	{
		
		for( BaseItemObject* PlayerObj = BaseItemObject::GetByOrder_Player(true) ; PlayerObj != NULL ; PlayerObj = BaseItemObject::GetByOrder_Player() )
		{
			if( RoomID != PlayerObj->Role()->RoomID() )
				continue;
			SC_FixHouseItem( PlayerObj->GUID() , HouseItem );
		}

	}
	else if( Owner != NULL )
	{
		SC_FixHouseItem( Owner->GUID() , HouseItem );
	}

}

void NetSrv_HousesChild::SendAllCli_FixHouseBase		( int RoomID , HouseBaseDBStruct& HouseBaseInfo  )
{
//	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( HouseBaseInfo.HouseDBID );
//	if( houseClass == NULL )
//		return;

//	houseClass->SetHouseBase( HouseBaseInfo );

	//іqЄѕ©Т¦іЄє¦іГцЄєЄ±®a
	RoleDataEx *Role;
	set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//­pєвЁC­У©Р¶ЎЄє¤HјЖ
	for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if( Obj == NULL )
			continue;
		Role = Obj->Role();
		if( Role->RoomID() != RoomID )
			continue;
		SC_HouseBaseInfo( Role->GUID() , HouseBaseInfo );
	}
}

void NetSrv_HousesChild::RC_SetHouseName			( BaseItemObject* Obj , const char* HouseName )
{
	RoleDataEx* Role = Obj->Role();
	if( Ini()->IsHouseZone == false )
	{
		SC_SetHouseNameResult( Role->GUID() , EM_SetHouseNameResult_DataErr );
		return;
	}


	if( Role->PlayerTempData->VisitHouseDBID != Role->PlayerBaseData->HouseDBID )
	{
		SC_SetHouseNameResult( Role->GUID() , EM_SetHouseNameResult_DataErr );
		return;
	}

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj(  Role->PlayerBaseData->HouseDBID );
	if( houseClass == NULL )
	{
		SC_SetHouseNameResult( Role->GUID() , EM_SetHouseNameResult_DataErr );
		return;
	}

	std::wstring outStrName;
	CharacterNameRulesENUM  Ret = g_ObjectData->CheckCharacterNameRules( HouseName , (CountryTypeENUM)Global::Ini()->CountryType , outStrName );
	if( Ret != EM_CharacterNameRules_Rightful )
	{
		SC_SetHouseNameResult( Role->GUID() , EM_SetHouseNameResult_NameErr );
		return;
	}


	int EnergyCost = g_ObjectData->GetSysKeyValue( "House_ChangeName_Energy" );
	//Їа¶qАЛ¬d
	if( houseClass->HouseBase().Info.EnergyPoint < EnergyCost )
	{
		SC_SetHouseNameResult( Role->GUID() , EM_SetHouseNameResult_EnergyErr );
		return;
	}

	if( Global::Ini()->IsHouseActionLog == true )
	{//­Ч§п¤p«О¦WєЩ
		char Buf[512];
		sprintf_s( Buf , sizeof(Buf) , "Old: %s  New: %s" , (char*)houseClass->HouseBase().Info.HouseName.Begin() , WCharToUtf8( outStrName.c_str() ).c_str() );
		Log_House( Role , Role->PlayerBaseData->HouseDBID  , Role->DBID() , EM_HouseActionType_ChangeHouseName , Buf );
	}

	houseClass->HouseBase().Info.EnergyPoint -= EnergyCost;
	houseClass->HouseBase().IsNeedSave = true;
	houseClass->HouseBase().Info.HouseName = WCharToUtf8( outStrName.c_str() ).c_str();
	houseClass->SetSaveInfo();

	SendAllCli_FixHouseBase( Role->RoomID() , houseClass->HouseBase().Info );

	SC_SetHouseNameResult( Role->GUID() , EM_SetHouseNameResult_OK );
	return;
}


void NetSrv_HousesChild::RC_SetClientData			( BaseItemObject* Obj , const char* ClientData )
{
	RoleDataEx* Role = Obj->Role();
	if( Ini()->IsHouseZone == false )
	{
//		SC_SetHouseNameResult( Role->GUID() , EM_SetHouseNameResult_DataErr );
		return;
	}


	if( Role->PlayerTempData->VisitHouseDBID != Role->PlayerBaseData->HouseDBID )
	{
//		SC_SetHouseNameResult( Role->GUID() , EM_SetHouseNameResult_DataErr );
		return;
	}

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj(  Role->PlayerBaseData->HouseDBID );
	if( houseClass == NULL )
	{
//		SC_SetHouseNameResult( Role->GUID() , EM_SetHouseNameResult_DataErr );
		return;
	}

	houseClass->HouseBase().IsNeedSave = true;
	memcpy( houseClass->HouseBase().Info.ClientData , ClientData , sizeof(houseClass->HouseBase().Info.ClientData) );
	houseClass->SetSaveInfo();
	
	if( Global::Ini()->IsHouseActionLog == true )
	{//і]©w¤p«О¦n¤Н°Э­Ф»y
		char Buf[4096];
		sprintf_s( Buf , sizeof(Buf) , "Enter:%s  Leave:%s  Use:%s" , houseClass->HouseBase().Info.Friend_Enter , houseClass->HouseBase().Info.Friend_Leave , houseClass->HouseBase().Info.Friend_Use );
		Log_House( Role , Role->PlayerBaseData->HouseDBID , Role->DBID() , EM_HouseActionType_HouseGreet , Buf );
	}

	SendAllCli_FixHouseBase( Role->RoomID() , houseClass->HouseBase().Info );
//	SC_SetHouseNameResult( Role->GUID() , EM_SetHouseNameResult_OK );
	return;
}


//////////////////////////////////////////////////////////////////////////
//єШґУ
//////////////////////////////////////////////////////////////////////////
void NetSrv_HousesChild::RC_PlantClearRequest		( BaseItemObject* Obj , int HousePos )
{
	RoleDataEx* Role = Obj->Role();

	if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
	{
		Role->Msg("¤Ј¬O¦Ы¤vЄє©Р¤l");
		return;
	}

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerBaseData->HouseDBID );
	if( houseClass == NULL )
	{
		Role->Msg("§д¤ЈЁм©Р«Оёк®Ж");
		SC_PlantClearResult( Role->GUID() ,HousePos , EM_PlantClearResultType_Failed );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	//АЛ¬d	
	HouseItemStruct* PotItem = houseClass->GetItem( -1 , HousePos );
	if( PotItem == NULL )
	{
		Role->Msg("§д¤ЈЄб¬Цёк®Ж");
		SC_PlantClearResult( Role->GUID() ,HousePos , EM_PlantClearResultType_Failed );
		return;
	}

	GameObjDbStructEx* PlotItemObjDB = Global::GetObjDB( PotItem->Info.Item.OrgObjID );
	if( PlotItemObjDB == NULL || PlotItemObjDB->Item.Plant.Pot.Mode == 0 )	
	{
		Role->Msg("§д¤ЈЄб¬Цёк®Ж");
		SC_PlantClearResult( Role->GUID() ,HousePos , EM_PlantClearResultType_Failed );
		return;
	}

	memset( PotItem->Info.Item.Ability , 0 , sizeof(PotItem->Info.Item.Ability) );
	if( PotItem->SaveState == EM_HouseItemSaveState_Normal )
		PotItem->SaveState = EM_HouseItemSaveState_Update;

	houseClass->SetSaveInfo();
	SC_FixHouseItemProc( Role , NULL , PotItem->Info );
	SC_PlantClearResult( Role->GUID() ,HousePos , EM_PlantClearResultType_OK );
	return;

}
void NetSrv_HousesChild::RC_PlantItemRequest		( BaseItemObject* Obj , PlantItemTypeENUM Type , int PotHousePos , int ItemBodyPos )
{
	RoleDataEx* Role = Obj->Role();

	if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
	{
		Role->Msg("¤Ј¬O¦Ы¤vЄє©Р¤l");
		return;
	}

	float PlantSkillLv = Role->TempData.Attr.Fin.SkillValue.Plant;

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerBaseData->HouseDBID );
	if( houseClass == NULL )
	{
		Role->Msg("§д¤ЈЁм©Р«Оёк®Ж");
		SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_Failed_HouseNotFind );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	//АЛ¬d	
	HouseItemStruct* PotItemInfo = houseClass->GetItem( -1 , PotHousePos );
	if( PotItemInfo == NULL )
	{
		Role->Msg("§д¤ЈЄб¬Цёк®Ж");
		SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_Failed_HouseItemNotFind );
		return;
	}

	GameObjDbStructEx* PlotItemObjDB = Global::GetObjDB( PotItemInfo->Info.Item.OrgObjID );
	if( PlotItemObjDB == NULL || PlotItemObjDB->Item.Plant.Pot.Mode == 0 )	
	{
		Role->Msg("§д¤ЈЄб¬Цёк®Ж");
		SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_Failed_PotErr );
		return;
	}

	ItemFieldStruct* bodyItem = Role->GetBodyItem( ItemBodyPos );

	if( bodyItem == NULL )
	{
		Role->Msg("Ґ]»qёк®Ж¦мёm¦і°ЭГD");
		SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_Failed_BodyItemErr );
		return;
	}
	GameObjDbStructEx* BodyItemDB = Global::GetObjDB( bodyItem->OrgObjID );
	if( BodyItemDB->IsItem_Pure() == false )
	{
		Role->Msg("Є««~¦і°ЭГD");
		SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_Failed_BodyItemErr );
		return;
	}

	//¬dёЯ­nЁПҐОЄєЄ««~µҐЇЕ
	if(		BodyItemDB->Item.Plant.Level > PlantSkillLv 
		||	PlotItemObjDB->Item.Plant.Level > PlantSkillLv )
	{
		Role->Msg("єШґУµҐЇЕ¤ЈЁ¬");
		SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_Failed_SkillLvErr );
		return;
	}

	/*if( bodyItem->GrowRate >= 10000 )
	{
		Role->Msg("¤wёg¦ЁЄш§№¦Ё ¤ЈҐiДйµ@");
		SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_Failed_GrowRateErr );
		return;
	}*/

	ItemFieldStruct& PotItem = PotItemInfo->Info.Item;
	switch( Type )
	{
	case EM_PlantItem_Seed:
		{
			if( PotItem.Plant.OrgSeedID != 0 )
			{
				Role->Msg("¬Ц¤lёк®Жїщ»~");
				SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_Failed_PotErr );
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			//­pєвҐiҐHєШґX¬Ц
			if( houseClass->PlantItemCount() >= Role->PlayerBaseData->MaxPlantCount )
			{
				Role->Msg("єШґУ¶W№L¤W­­¶q");
				SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_Failed_PlantCount );
				return;
			}
			
			//////////////////////////////////////////////////////////////////////////
			bool IsOk = false;
			switch( BodyItemDB->Item.Plant.Seed.Type )
			{
			case EM_SeedType_None:	//¤Ј¬OєШ¤l
				break;
			case EM_SeedType_Tree:	//¤м
				if( PlotItemObjDB->Item.Plant.Pot.Tree != false )
					IsOk = true;
				break;
			case EM_SeedType_Grass:	//Їу
				if( PlotItemObjDB->Item.Plant.Pot.Grass != false )
					IsOk = true;
				break;
			case EM_SeedType_Mine:	//Дq
				if( PlotItemObjDB->Item.Plant.Pot.Mine != false )
					IsOk = true;
				break;
			}
			if( IsOk == false )
			{
				Role->Msg("¬Ц¤lёк®Жїщ»~");
				SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_Failed_PotErr );
				return;
			}
			//§дҐXєШ¤l
			int SeedID = 0;
			int TCount = 0;
			if( Role->GetTreasure( BodyItemDB->Item.Plant.Seed.RandSeed , SeedID  , TCount ) == false )
			{
				SeedID = bodyItem->OrgObjID;
			}

			PotItem.Plant.OrgSeedID		= bodyItem->OrgObjID - Def_ObjectClass_Item;			//­мҐ»ЄєєШ¤lё№ЅX
			PotItem.Plant.SeedID		= SeedID - Def_ObjectClass_Item;
			PotItem.Plant.CreateTime	= RoleDataEx::G_Now;
			PotItem.Plant.LastProcTime	= RoleDataEx::G_Now;		//іМ«біBІzЄє®Й¶Ў
			PotItem.Plant.HealthPoint	= 10000;			//°·±d«Ч
			PotItem.Plant.RarePoint		= 0;			//µ}¦і«Ч
			PotItem.Plant.WaterPoint	= 0;			//Аг«Ч (10­ї)
			PotItem.Plant.TopdressPoint	= 0;			//ѕi¤А (10­ї)

			PotItem.Plant.DecWaterPoint	= 0;		//«O¤ф¤U­°¶q(10­ї)	
			PotItem.Plant.DecTopdressPoint	= 0;	//ЄО®Ж¤U­°¶q(10­ї)

			PotItem.Plant.GrowRate		= 0;
			PotItem.Plant.FeedPoint		= 0;		//№Ў­№«Ч

			PotItem.Plant.IsLock		= false;//Вк©w«бЕЬ¦Ё°®іyЄб¤Ј·|¦ЁЄшЎA¤]µLЄk¦¬¦Ё
			PotItem.Plant.IsDead		= false;
//			HousesManageClass::PlantItemGrowProc( PotItem );

			Role->Log_ItemDestroy(  EM_ActionType_Plant_Destroy , *bodyItem , 1 , "" );		
			if( bodyItem->Count <= 1 )
				bodyItem->Init();
			else
				bodyItem->Count --;

			if( PotItemInfo->SaveState == EM_HouseItemSaveState_Normal )
				PotItemInfo->SaveState = EM_HouseItemSaveState_Update;

			houseClass->SetSaveInfo();
			//ёк®Ж­ЧҐї
			SC_FixHouseItemProc( Role , NULL , PotItemInfo->Info );
			Role->Net_FixItemInfo_Body( ItemBodyPos );
			SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_OK );
			return;
		}
		break;
	case EM_PlantItem_Water:
	case EM_PlantItem_Topdress:
	case EM_PlantItem_MagicWater:
		{
			if( BodyItemDB->Item.Plant.Manure.Type != Type  )
			{
				SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_Failed );
				return;
			}

			if( PotItem.Plant.FeedPoint + BodyItemDB->Item.Plant.Manure.FeedPoint*100 > 10000 )
			{
				//№L№Ў­№
				SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_Failed );
				return;
			}

			GameObjDbStructEx* SeedItemDB = Global::GetObjDB( PotItem.Plant.SeedID + Def_ObjectClass_Item );
			if( SeedItemDB == NULL || SeedItemDB->Item.Plant.Seed.Type == EM_SeedType_None )
			{
				SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_Failed );
				return;
			}
			float ArgRare = 1;
			switch( SeedItemDB->Item.Plant.Seed.Type )
			{
			case EM_SeedType_Tree:	//¤м
				{
					ArgRare = PlotItemObjDB->Item.Plant.Pot.EvnPoint[0][1];
				}
				break;
			case EM_SeedType_Grass:	//Їу
				{
					ArgRare = PlotItemObjDB->Item.Plant.Pot.EvnPoint[1][1];
				}
				break;
			case EM_SeedType_Mine:	//Дq
				{
					ArgRare = PlotItemObjDB->Item.Plant.Pot.EvnPoint[2][1];
				}
				break;
			}

			ItemFieldStruct PotItemBackup = PotItem;

			PotItem.Plant.RarePoint += int(BodyItemDB->Item.Plant.Manure.AddRare*ArgRare*100);
			if( PotItem.Plant.RarePoint > 10000 )
				PotItem.Plant.RarePoint = 10000;

			PotItem.Plant.HealthPoint += int(BodyItemDB->Item.Plant.Manure.AddHealth )*100;
			if( PotItem.Plant.HealthPoint > 10000 )
				PotItem.Plant.HealthPoint = 10000;
			PotItem.Plant.FeedPoint += BodyItemDB->Item.Plant.Manure.FeedPoint*100;


			PotItem.Plant.DecWaterPoint =  short((PotItem.Plant.DecWaterPoint * PotItem.Plant.WaterPoint / 100 + BodyItemDB->Item.Plant.Manure.AddWater*100 *BodyItemDB->Item.Plant.DecWater)   / max( 1 , ( PotItem.Plant.WaterPoint/100 + BodyItemDB->Item.Plant.Manure.AddWater ) ));
			PotItem.Plant.DecTopdressPoint = short((PotItem.Plant.DecTopdressPoint * PotItem.Plant.TopdressPoint / 100 + BodyItemDB->Item.Plant.Manure.AddTopdress*100 *BodyItemDB->Item.Plant.DecTopdress)   / max( 1 , ( PotItem.Plant.TopdressPoint/100 + BodyItemDB->Item.Plant.Manure.AddTopdress ) ));

			PotItem.Plant.WaterPoint += ( BodyItemDB->Item.Plant.Manure.AddWater * 100 );
			if( PotItem.Plant.WaterPoint > 10000 )
				PotItem.Plant.WaterPoint = 10000;
			PotItem.Plant.TopdressPoint += ( BodyItemDB->Item.Plant.Manure.AddTopdress * 100 );
			if( PotItem.Plant.TopdressPoint > 10000 )
				PotItem.Plant.TopdressPoint = 10000;

			PotItem.Plant.GrowRate += ( BodyItemDB->Item.Plant.Manure.AddGrow * 100 );
			if( PotItem.Plant.GrowRate > 20000 )
				PotItem.Plant.GrowRate = 20000;

			if( memcmp( &PotItemBackup , &PotItem , sizeof(PotItemBackup) ) == 0 ) 
			{
				SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_Failed_ItemNoUse );
				return;
			}

			if( BodyItemDB->Mode.Expense != false )
			{
				Role->Log_ItemDestroy(  EM_ActionType_Plant_Destroy , *bodyItem , 1 , "" );		
				if( bodyItem->Count <= 1 )
					bodyItem->Init();
				else
					bodyItem->Count --;
			}
			if( PotItemInfo->SaveState == EM_HouseItemSaveState_Normal )
				PotItemInfo->SaveState = EM_HouseItemSaveState_Update;
			houseClass->SetSaveInfo();
			SC_FixHouseItemProc( Role , NULL , PotItemInfo->Info );
			Role->Net_FixItemInfo_Body( ItemBodyPos );
			SC_PlantItemResult( Role->GUID() , Type , PotHousePos ,ItemBodyPos , EM_PlantItemResultType_OK );

			return;
		}
		break;
	}
	return;
}
void NetSrv_HousesChild::RC_PlantLockRequest		( BaseItemObject* Obj , int HousePos )
{
	RoleDataEx* Role = Obj->Role();

	if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
	{
		Role->Msg("¤Ј¬O¦Ы¤vЄє©Р¤l");
		return;
	}

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerBaseData->HouseDBID );
	if( houseClass == NULL )
	{
		Role->Msg("§д¤ЈЁм©Р«Оёк®Ж");
		SC_PlantLockResult( Role->GUID() ,HousePos , EM_PlantLockResultType_Failed );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	//АЛ¬d	
	HouseItemStruct* PotItemInfo = houseClass->GetItem( -1 , HousePos );
	if( PotItemInfo == NULL )
	{
		Role->Msg("§д¤ЈЄб¬Цёк®Ж");
		SC_PlantLockResult( Role->GUID() ,HousePos , EM_PlantLockResultType_Failed );
		return;
	}
	ItemFieldStruct& PotItem = PotItemInfo->Info.Item;
	GameObjDbStructEx* PlotItemObjDB = Global::GetObjDB( PotItem.OrgObjID );
	if( PlotItemObjDB == NULL || PlotItemObjDB->Item.Plant.Pot.Mode == 0 )	
	{
		Role->Msg("§д¤ЈЄб¬Цёк®Ж");
		SC_PlantLockResult( Role->GUID() ,HousePos , EM_PlantLockResultType_Failed );
		return;
	}
	if( PotItem.Plant.IsLock != false || PotItem.Plant.SeedID == 0 )
	{
		Role->Msg("ЁSєШЄF¦и");
		SC_PlantLockResult( Role->GUID() ,HousePos , EM_PlantLockResultType_Failed );
		return;
	}
	PotItem.Plant.IsLock = true;
	if( PotItemInfo->SaveState == EM_HouseItemSaveState_Normal )
		PotItemInfo->SaveState = EM_HouseItemSaveState_Update;
	houseClass->SetSaveInfo();
	SC_FixHouseItemProc( Role , NULL , PotItemInfo->Info );
	SC_PlantLockResult( Role->GUID() ,HousePos , EM_PlantLockResultType_OK );
	return;	
}
void NetSrv_HousesChild::RC_PlantGetProductRequest	( BaseItemObject* Obj , int HousePos )
{

	RoleDataEx* Role = Obj->Role();

	if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
	{
		Role->Msg("¤Ј¬O¦Ы¤vЄє©Р¤l");
		return;
	}

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerBaseData->HouseDBID );
	if( houseClass == NULL )
	{
		Role->Msg("§д¤ЈЁм©Р«Оёк®Ж");
		SC_PlantGetProductResult( Role->GUID() ,HousePos , EM_PlantGetProductResultType_Failed );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	//АЛ¬d	
	HouseItemStruct* PotItemInfo = houseClass->GetItem( -1 , HousePos );
	if( PotItemInfo == NULL )
	{
		Role->Msg("§д¤ЈЄб¬Цёк®Ж");
		SC_PlantGetProductResult( Role->GUID() ,HousePos , EM_PlantGetProductResultType_Failed );
		return;
	}
	ItemFieldStruct& PotItem = PotItemInfo->Info.Item;
	GameObjDbStructEx* PlotItemObjDB = Global::GetObjDB( PotItem.OrgObjID );
	if( PlotItemObjDB == NULL || PlotItemObjDB->Item.Plant.Pot.Mode == 0 )	
	{
		Role->Msg("§д¤ЈЄб¬Цёк®Ж");
		SC_PlantGetProductResult( Role->GUID() ,HousePos , EM_PlantGetProductResultType_Failed );
		return;
	}
	if( PotItem.Plant.IsLock != false || PotItem.Plant.GrowRate < 10000  )
	{
		Role->Msg("ЁSЄF¦иҐi¦¬¦Ё");
		SC_PlantGetProductResult( Role->GUID() ,HousePos , EM_PlantGetProductResultType_NoFruits );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	//Ёъ±oІЈЄ«
	//////////////////////////////////////////////////////////////////////////
	
	int SkillDLv = 0;
	GameObjDbStructEx* SeedItemObjDB = Global::GetObjDB( PotItem.Plant.SeedID + Def_ObjectClass_Item );
	if( SeedItemObjDB != NULL )
	{
		SkillDLv = int( Role->TempData.Attr.Fin.SkillValue.Plant ) - SeedItemObjDB->Item.Plant.Level;
	}
	PlantProductTableStruct& Product = g_ObjectData->GetPlantProduct( PotItem.Plant.SeedID + Def_ObjectClass_Item , int(PotItem.Plant.HealthPoint/100 ) , int(PotItem.Plant.RarePoint/100) );
	Role->GiveItem( Product.DropID , 1 , EM_ActionType_Plant_Product , NULL , "" );
	if( SkillDLv >= 5 && rand() % 100 < 20 )
		Role->GiveItem( Product.DropID_DLv5 , 1 , EM_ActionType_Plant_Product , NULL , "" );
	if( SkillDLv >= 10 && rand() % 100 < 10 )
		Role->GiveItem( Product.DropID_DLv10 , 1 , EM_ActionType_Plant_Product , NULL , "" );
	
	//////////////////////////////////////////////////////////////////////////
	memset( PotItem.Ability , 0 , sizeof(PotItem.Ability) );
	if( PotItemInfo->SaveState == EM_HouseItemSaveState_Normal )
		PotItemInfo->SaveState = EM_HouseItemSaveState_Update;
	houseClass->SetSaveInfo();
	SC_FixHouseItemProc( Role , NULL , PotItemInfo->Info );
	SC_PlantGetProductResult( Role->GUID() ,HousePos , EM_PlantGetProductResultType_OK );

	Role->AddSkillValue( SeedItemObjDB->Item.Plant.Level , EM_SkillValueType_Plant , g_ObjectData->SysValue().Plant.FruitsExp * Product.ExpRate  );

	//Obj->PlotVM()->CallLuaFunc( Product.LuaScript , Role->GUID() );
	LUA_VMClass::PCallByStrArg( Product.LuaScript , Role->GUID() , Role->GUID() );

	return;
}

void NetSrv_HousesChild::RD_FriendDBID				( int HouseDBID , int FriendDBID )
{
	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( HouseDBID );	
	if( houseClass == NULL )
		return;
	houseClass->FriendDBIDList().push_back( FriendDBID );
}
void NetSrv_HousesChild::RC_ModifyFriendDBID		( BaseItemObject* Obj , bool IsDelete , int FriendDBID )
{
	char	SqlCmd[512];
	RoleDataEx* Role = Obj->Role();
	int HouseDBID = Role->PlayerBaseData->HouseDBID;
	if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
	{
		Role->Msg("¤Ј¬O¦Ы¤vЄє©Р¤l");
		return;
	}

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( HouseDBID );
	if( houseClass == NULL )
		return;

	vector< int >& DBIDList = houseClass->FriendDBIDList();
	vector< int >::iterator Iter;

	Iter =std::find( DBIDList.begin() , DBIDList.end() , FriendDBID );
	if( IsDelete )
	{
		if( Iter == DBIDList.end() )
			return;

		DBIDList.erase( Iter );
		
		sprintf( SqlCmd , "DELETE Houses_FriendList WHERE HouseDBID=%d and FriendDBID=%d" , HouseDBID , FriendDBID );
		Net_DCBase::SqlCommand( HouseDBID , SqlCmd );
		
	}
	else
	{
		if( Iter != DBIDList.end() )
			return;
		DBIDList.push_back( FriendDBID );
		sprintf( SqlCmd , "INSERT Houses_FriendList(HouseDBID,FriendDBID) VALUES( %d,%d)" , HouseDBID , FriendDBID );
		Net_DCBase::SqlCommand( HouseDBID , SqlCmd );
	}

	SendAllCli_ModifyFriendDBID( HouseDBID , IsDelete , FriendDBID );
}

void NetSrv_HousesChild::SendAllCli_ChangeHouseResult( int HouseDBID , int HouseType , bool Result )
{
	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( HouseDBID );
	if( houseClass == NULL )
		return;

	//іqЄѕ©Т¦іЄє¦іГцЄєЄ±®a
	RoleDataEx *Role;
	set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//­pєвЁC­У©Р¶ЎЄє¤HјЖ
	for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if( Obj == NULL )
			continue;
		Role = Obj->Role();
		if( Role->RoomID() != houseClass->RoomID() )
			continue;
		if( Result != false )
		{
			NetSrv_MoveChild::SetObjPos( Obj , 0 , 0 , 0 , Role->Pos()->Dir );
		}
		SC_ChangeHouseResult( Role->GUID() , HouseType , Result );
		if( Global::Ini()->IsHouseActionLog == true && Result )
		{//ЕЬ§у©Р«О®ж§Ѕ
			char Buf[128];
			sprintf_s( Buf , sizeof(Buf) , "[%d]" , Role->DBID() , HouseType );
			Log_House( Role , HouseDBID , Role->DBID() , EM_HouseActionType_ChangeHouseType , Buf );
		}
	}

}

void NetSrv_HousesChild::SendAllCli_AllHouseItem		( int HouseDBID )
{
	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( HouseDBID );
	if( houseClass == NULL )
		return;

	for( int i = 0 ; i < 200 ; i++ )
	{
		HouseItemStruct* houseItem = houseClass->GetItem( -1 , i );
		if( houseItem == NULL )
			continue;

		GameObjDbStructEx* ParentItemDB = Global::GetObjDB( houseItem->Info.Item.OrgObjID );
		if( ParentItemDB == NULL || ParentItemDB->IsItem() == false )
			return;

		if( ParentItemDB->Item.ItemType != EM_ItemType_Furniture )
			return;

		if(		ParentItemDB->Item.emFurnitureType != EM_FurnitureType_Coathanger
			&&	ParentItemDB->Item.emFurnitureType != EM_FurnitureType_WeaponSet
			&&	ParentItemDB->Item.emFurnitureType != EM_FurnitureType_WeaponAndCloth )
			return;

		//іqЄѕ©Т¦іЄє¦іГцЄєЄ±®a
		RoleDataEx *Role;
		set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
		set< BaseItemObject*>::iterator   PlayerObjIter;
		//­pєвЁC­У©Р¶ЎЄє¤HјЖ
		for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
		{
			BaseItemObject* Obj = *PlayerObjIter;
			if( Obj == NULL )
				continue;
			Role = Obj->Role();
			if( Role->RoomID() != houseClass->RoomID() )
				continue;
			SC_ItemInfo( Role->GUID() , houseItem->Info.ItemDBID , houseItem->ChildList );
		}
	}
}

void NetSrv_HousesChild::SendAllCli_ModifyFriendDBID	( int HouseDBID , bool IsDelete , int FriendDBID )
{
	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( HouseDBID );
	if( houseClass == NULL )
		return;

	//іqЄѕ©Т¦іЄє¦іГцЄєЄ±®a
	RoleDataEx *Role;
	set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//­pєвЁC­У©Р¶ЎЄє¤HјЖ
	for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if( Obj == NULL )
			continue;
		Role = Obj->Role();
		if( Role->RoomID() != houseClass->RoomID() )
			continue;
		
		if( Global::Ini()->IsHouseActionLog == true )
		{
			char Buf[128];
			if( IsDelete )
			{
				sprintf_s( Buf , sizeof(Buf) , "[%d]" , FriendDBID );
				Log_House( Role , HouseDBID , Role->DBID() , EM_HouseActionType_DeleteFriend , Buf );
			}
			else
			{
				sprintf_s( Buf , sizeof(Buf) , "[%d]" , FriendDBID );
				Log_House( Role , HouseDBID , Role->DBID() , EM_HouseActionType_AddFriend , Buf );
			}
			Role->Msg( Buf );
		}

		SC_ModifyFriendDBID( Role->GUID() , IsDelete , FriendDBID );
	}

}

void NetSrv_HousesChild::RC_ClearItemLogRequest		( BaseItemObject* Obj )
{
	RoleDataEx* Owner = Obj->Role();
	if( Owner->PlayerBaseData->HouseDBID != Owner->PlayerTempData->VisitHouseDBID )
	{
		SC_ClearItemLogResult( Owner->GUID() , false );
		return;
	}

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Owner->PlayerBaseData->HouseDBID );
	if( houseClass == NULL )
		return;

	houseClass->GetHouseBase()->Info.ClearItemLog();
	houseClass->SetSaveInfo();
	SC_ClearItemLogResult( Owner->GUID() , true );
}

//////////////////////////////////////////////////////////////////////////
//¤k№І
//////////////////////////////////////////////////////////////////////////
//­nЁD©Ы¶Т¦CЄн
void NetSrv_HousesChild::RC_ServantHireListRequest( BaseItemObject* OwnerObj )
{
	RoleDataEx* Owner = OwnerObj->Role();
	if( Owner->PlayerTempData->VisitHouseDBID != Owner->PlayerBaseData->HouseDBID )
		return;

	map< int , HouseServantStruct >::iterator Iter;
	
	int Count = 1;
	for( Iter = _ServantList.begin() ; Iter != _ServantList.end() ; Iter++ , Count++ )
	{
		SC_ServantHireList( OwnerObj->Role()->GUID() , Iter->first , Count == _ServantList.size() , Iter->second );
	}

	//АЛ¬d¬O§_¦і¦№¤k№І±MДЭЄєЅc¤l

}
//©Ы¶Т¤k№І
void NetSrv_HousesChild::RC_ServantHireRequest( BaseItemObject* OwnerObj , int ID , int Pos )
{
	RoleDataEx* Owner = OwnerObj->Role();
	if( Owner->PlayerTempData->VisitHouseDBID != Owner->PlayerBaseData->HouseDBID )
		return;	

	if( (unsigned)Pos >= MAX_SERVANT_COUNT )
		return;

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Owner->PlayerTempData->VisitHouseDBID );
	if( houseClass == NULL )
		return;

	HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[ Pos ];

	if( HouseServantInfo.NPCObjID > 0 )
		return;

	map< int , HouseServantStruct >::iterator Iter = _ServantList.find( ID );
	if( Iter == _ServantList.end() || Iter->second.IsHire != false )
	{
		SC_ServantHireResultFailed( Owner->GUID() , ID );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	//¶±ҐОЄчїъАЛ¬d
	//////////////////////////////////////////////////////////////////////////
	//float CostRate[] = {1.0f ,1.5f , 2.25f , 3.37f , 5.06f , 7.59f };
	float CostRate[] = {1.0f ,2.0f , 4.0f , 8.0f , 16.0f , 32.0f };
	int ServantCount = 0;
	for( int i = 0 ; i < MAX_SERVANT_COUNT ; i++ )
		if( houseClass->GetHouseBase()->Info.Servant[ i ].NPCObjID > 0  )
			ServantCount++;

	int	CostMoney = int( CostRate[ServantCount] * Iter->second.Cost );

	if( Owner->BodyMoney() < CostMoney )
	{
		SC_ServantHireResultFailed( Owner->GUID() , ID );
		return;
	}
	Owner->AddBodyMoney( CostMoney * -1 , NULL , EM_ActionType_ServantHire , true  );


	//////////////////////////////////////////////////////////////////////////
	//µ№¤k№І©сёЛіЖЄєД_Ѕc
	//////////////////////////////////////////////////////////////////////////
	GameObjDbStructEx* ServantBoxDB = Global::GetObjDB( g_ObjectData->GetSysKeyValue( "HouseServantBoxID" ) );
	if( ServantBoxDB == NULL )
		return;

	HouseItemDBStruct ServantBox;

	ServantBox.Item.Init();
	ServantBox.Item.OrgObjID = ServantBoxDB->GUID;
	ServantBox.Item.Count = 1;
	ServantBox.Layout.Init();

	ServantBox.HouseDBID = houseClass->HouseBase().Info.HouseDBID;
	ServantBox.ItemDBID = -1;
	ServantBox.ParentItemDBID = -2;
	ServantBox.Pos = Pos;

	if( houseClass->AddItem( ServantBox ) != false )
	{
		SC_FixHouseItemProc( Owner , NULL , ServantBox );
	}

	//////////////////////////////////////////////////////////////////////////
	HouseServantInfo = Iter->second;
	Iter->second.IsHire = true;
	HouseServantInfo.Cost = CostMoney;
	
	if( Global::Ini()->IsHouseServantLog == true )
	{
		char Buf[128];
		sprintf_s( Buf , sizeof(Buf) , " Cost %d To Buy [%d] " , HouseServantInfo.Cost , HouseServantInfo.NPCObjID );
		Log_House( Owner , ServantBox.HouseDBID , Owner->DBID() , EM_HouseActionType_Servant_Employ , Buf );
	}

	houseClass->SetSaveInfo();
	SC_ServantHireResultOK( Owner->GUID() , ID , Pos , HouseServantInfo );

}
//­nЁD§R°Ј¤k№І
void NetSrv_HousesChild::RC_ServantDelRequest( BaseItemObject* OwnerObj , int ServantID )
{
	RoleDataEx* Owner = OwnerObj->Role();
	if( Owner->PlayerTempData->VisitHouseDBID != Owner->PlayerBaseData->HouseDBID )
		return;

	if( (unsigned)ServantID >= MAX_SERVANT_COUNT )
		return;



	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Owner->PlayerTempData->VisitHouseDBID );
	if( houseClass == NULL )
		return;

	HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[ ServantID ];

	if( HouseServantInfo.NPCObjID <= 0 )
	{
		SC_ServantDelResult( Owner->GUID() , ServantID , false );
		return;
	}
	
	if( Global::Ini()->IsHouseServantLog == true )
	{
		char Buf[128];
		sprintf_s( Buf , sizeof(Buf) , " [%d] " , Owner->DBID() , HouseServantInfo.NPCObjID );
		Log_House( Owner , Owner->PlayerBaseData->HouseDBID , Owner->DBID() , EM_HouseActionType_Servant_Fire , Buf );
	}

	HouseServantInfo.Init();
	houseClass->SetSaveInfo();
	SC_ServantDelResult( Owner->GUID() , ServantID , true );

	//§R°Ј¦№¤k№ІЄєЅc¤l
}
//¤k№І¤¬°КЁЖҐу
void NetSrv_HousesChild::RC_ServantEventRequest( BaseItemObject* OwnerObj , int ServantID , int EventType1 , int EventType2 )
{

	bool	IsVisitor = false;
	RoleDataEx* Owner = OwnerObj->Role();
	if( Owner->PlayerTempData->VisitHouseDBID != Owner->PlayerBaseData->HouseDBID )
		IsVisitor = true;

	if( (unsigned)ServantID >= MAX_SERVANT_COUNT )
		return;



	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Owner->PlayerTempData->VisitHouseDBID );
	if( houseClass == NULL )
		return;

	HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[ ServantID ];

	if( HouseServantInfo.NPCObjID <= 0 )
		return;

	vector< ServantEventTableStruct >&	ServantEventList = g_ObjectData->_ServantEventList;
	
	vector< int >	EventPosList;

	bool CheckOK = true;
	for( unsigned i = 0 ; i < ServantEventList.size() ; i++ )
	{		

		if(		ServantEventList[i].EventType[0] != EventType1 
			||	ServantEventList[i].EventType[1] != EventType2 )
			continue;

		if(		ServantEventList[i].Check.Character != ( HouseServantInfo.Character % 10 )
			&&	ServantEventList[i].Check.Character != -1 )
			continue;

		CheckOK = true;
		switch( ServantEventList[i].Check.Servant )
		{
		case EM_ServantCondition_Servant_Boy:
			if( HouseServantInfo.Sex != 0)
				CheckOK = false;
			break;
		case EM_ServantCondition_Servant_Girl:
			if( HouseServantInfo.Sex != 1)
				CheckOK = false;
			break;
		case EM_ServantCondition_Servant_Monster:
			if( HouseServantInfo.Sex != 2)
				CheckOK = false;
			break;
		case EM_ServantCondition_Servant_All:
			break;
		}

		if( CheckOK == false )
			continue;

		switch( ServantEventList[i].Check.Master )
		{
		case EM_ServantCondition_Master_MasterBoy:
			if( IsVisitor != false || Owner->BaseData.Sex != EM_Sex_Boy )
				CheckOK = false;
			break;
		case EM_ServantCondition_Master_MasterGirl:
			if( IsVisitor != false || Owner->BaseData.Sex != EM_Sex_Girl )
				CheckOK = false;
			break;
		case EM_ServantCondition_Master_Boy:
			if( Owner->BaseData.Sex != EM_Sex_Boy || IsVisitor == false )
				CheckOK = false;
			break;
		case EM_ServantCondition_Master_Girl:
			if( Owner->BaseData.Sex != EM_Sex_Girl || IsVisitor == false )
				CheckOK = false;
			break;
		case EM_ServantCondition_Master_Master:
			if( IsVisitor )
				CheckOK = false;
			break;
		case EM_ServantCondition_Master_Visitor:
			if( IsVisitor == false )
				CheckOK = false;
			break;
		case EM_ServantCondition_Master_All:
			break;
		}

		if( CheckOK == false )
			continue;

		//јЖ­ИАЛ¬d
		for( int j = 0 ; j < 8 ; j++ )
		{
			if(		HouseServantInfo.Value[j] > ServantEventList[i].MaxValue[j]
				||	HouseServantInfo.Value[j] < ServantEventList[i].MinValue[j] )
				{
					CheckOK = false;
					break;
				}
		}

		if( CheckOK == false )
			continue;

		//for( int j = 0 ; j < ServantEventList[i].Rate ; j ++ )
		if( ServantEventList[i].Rate > rand() % 100 )
		{
			EventPosList.push_back( i );
		}
	}

	if( EventPosList.size() == 0 )
		return;


	for( int i = 0 ; i < 3 ; i++ )
	{
		char LuaScript[ 512 ];
		bool IsCheckOK = true;
		int EventID = EventPosList[ rand()%	EventPosList.size() ];
		//ServantEventList
		if(		strlen( ServantEventList[EventID].LuaCheckScript ) != 0 )
		{
			IsCheckOK = false;			
			sprintf( LuaScript , "%s(%d)" ,  ServantEventList[EventID].LuaCheckScript , ServantID );

			if( Global::LuaCheckFunction( Owner->GUID() , Owner->GUID() , LuaScript ) != false )
			{
				IsCheckOK = true;
			}
		}
		if( IsCheckOK == false )
			continue;

		if( strlen( ServantEventList[EventID].LuaScript ) != NULL )	
		{
			sprintf( LuaScript , "%s(%d)" ,  ServantEventList[EventID].LuaScript , ServantID );
			OwnerObj->PlotVM()->PCallByStrArg( LuaScript , Owner->GUID() , 0 );
		}
		
		if( Global::Ini()->IsHouseServantLog == true )
		{
			char Buf[128];
			sprintf_s( Buf , sizeof(Buf) , "[%d]" , HouseServantInfo.NPCObjID );
			Log_House( Owner , Owner->PlayerBaseData->HouseDBID , Owner->DBID() , EM_HouseActionType_Servant_Talk  + EventType2 - 20 , Buf );
		}

		SC_ServantEvent( Owner->GUID() , ServantID , EventID );
		return;
	}

	houseClass->SetSaveInfo();

}
//¤k№ІВ\і]
void NetSrv_HousesChild::RC_ServantLayout( BaseItemObject* OwnerObj , int ServantID , House3DLayoutStruct& Layout )
{
	RoleDataEx* Owner = OwnerObj->Role();
	if( Owner->PlayerTempData->VisitHouseDBID != Owner->PlayerBaseData->HouseDBID )
		return;

	if( (unsigned)ServantID >= MAX_SERVANT_COUNT )
		return;

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Owner->PlayerTempData->VisitHouseDBID );
	if( houseClass == NULL )
		return;

	HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[ ServantID ];

	HouseServantInfo.Layout = Layout;
	houseClass->SetSaveInfo();
}
//¤k№І©R¦W
void NetSrv_HousesChild::RC_ServantRenameRequest( BaseItemObject* OwnerObj , int ServantID , const char* Name )
{
	RoleDataEx* Owner = OwnerObj->Role();
	if( Owner->PlayerTempData->VisitHouseDBID != Owner->PlayerBaseData->HouseDBID )
		return;

	if( (unsigned)ServantID >= MAX_SERVANT_COUNT )
		return;

	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Owner->PlayerTempData->VisitHouseDBID );
	if( houseClass == NULL )
		return;

	HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[ ServantID ];

	HouseServantInfo.Name = Name;
	houseClass->SetSaveInfo();
	SC_ServantRenameResult( Owner->GUID() , Name , true );
}

int CalRandValue( int Base , float DRate )
{
	//float Value = float( Base );
	float Rate = ( rand() / 65536.0f * 2 - 1 ) * DRate;

	return Base - int( Base * DRate );
}
//©w®ЙІЈҐНҐi¶±ҐО¤k№І
int	NetSrv_HousesChild::_OnTime_UpdateServantHireInfo	(SchedularInfo* Obj, void* InputClass )
{
	if( Global::Ini()->IsHouseZone == false )
		return 0;

	if( g_ObjectData->_ServantHire_NPCObjID.size() == 0 )
		return 0;

	if( RoleDataEx::G_Now == 0 )
	{
		Global::Schedular()->Push( _OnTime_UpdateServantHireInfo , 1000 , NULL, NULL );	
		return 0 ;
	}
	
	_ServantList.clear();

	HouseServantStruct ServantTemp;
	
	for( int i = 0 ; i < 20 ; i++ )
	{
		_ServantMaxID++;
		ServantTemp.CreateTime = RoleDataEx::G_Now + i;
		vector< int > *Temp;		

		Temp = &(g_ObjectData->_ServantHire_NPCObjID);
		if( Temp->size() == 0 )
		{
			Print( LV4 , "_OnTime_UpdateServantHireInfo" , "g_ObjectData->_ServantHire_NPCObjID size= 0" );
			break;
		}

		int NpcRandID = rand() % (int)Temp->size();

		ServantTemp.NPCObjID		= (*Temp)[ NpcRandID ];
		int LookNPCObjID =  g_ObjectData->_ServantHire_LookNPCObjID[ NpcRandID ];

		Temp = &(g_ObjectData->_ServantHire_FaceID[ LookNPCObjID ] );
		if( Temp->size() == 0 )
		{
			Print( LV4 , "_OnTime_UpdateServantHireInfo" , "_ServantHire_FaceID size= 0 LookNPCObjID=%d" , LookNPCObjID );
			break;
		}
		ServantTemp.Look.FaceID		= (*Temp)[ rand() % Temp->size() ];

		Temp = &(g_ObjectData->_ServantHire_HairID[ LookNPCObjID ] );
		if( Temp->size() == 0 )
		{
			Print( LV4 , "_OnTime_UpdateServantHireInfo" , "_ServantHire_HairID size= 0 LookNPCObjID=%d" , LookNPCObjID );
			break;
		}
		ServantTemp.Look.HairID		= (*Temp)[ rand() % Temp->size() ];

		Temp = &(g_ObjectData->_ServantHire_HairColor[ LookNPCObjID ] );
		if( Temp->size() == 0 )
		{
			Print( LV4 , "_OnTime_UpdateServantHireInfo" , "_ServantHire_HairColor size= 0 LookNPCObjID=%d" , LookNPCObjID );
			break;
		}
		ServantTemp.Look.HairColor		= (*Temp)[ rand() % Temp->size() ];

		Temp = &(g_ObjectData->_ServantHire_SkinColor[ LookNPCObjID ] );
		if( Temp->size() == 0 )
		{
			Print( LV4 , "_OnTime_UpdateServantHireInfo" , "_ServantHire_SkinColor size= 0 LookNPCObjID=%d" , LookNPCObjID );
			break;
		}
		ServantTemp.Look.BodyColor		= (*Temp)[ rand() % Temp->size() ];

		Temp = &(g_ObjectData->_ServantHire_ClothColor1[ LookNPCObjID ] );
		if( Temp->size() == 0 )
			ServantTemp.ClothColor[0]   = 0;
		else
			ServantTemp.ClothColor[0]	= (*Temp)[ rand() % Temp->size() ];

		Temp = &(g_ObjectData->_ServantHire_ClothColor2[ LookNPCObjID ] );
		if( Temp->size() == 0 )
			ServantTemp.ClothColor[1]		= 0;
		else
			ServantTemp.ClothColor[1]		= (*Temp)[ rand() % Temp->size() ];

		Temp = &(g_ObjectData->_ServantHire_Character[ LookNPCObjID ] );
		if( Temp->size() == 0 )
		{
			Print( LV4 , "_OnTime_UpdateServantHireInfo" , "_ServantHire_Character size= 0 LookNPCObjID=%d" , LookNPCObjID );
			break;
		}
		ServantTemp.Character			= (*Temp)[ rand() % Temp->size() ];
		
		ServantStartValueTableStruct& ValueTable = g_ObjectData->_ServantStartValueMap[ ServantTemp.NPCObjID ];
		ServantTemp.Sex	= ValueTable.Sex;
		ServantTemp.Cost = ValueTable.Cost;
		for( int j = 0 ; j < 8 ; j++ )
		{
			ServantTemp.MaxValue[j] = CalRandValue( ValueTable.MaxValue[j] , 0.2f );
			ServantTemp.Value[j] = CalRandValue( ValueTable.StartValue[j] , 0.2f ); 

			if( ServantTemp.Value[j] > ServantTemp.MaxValue[j] )
				ServantTemp.MaxValue[j] = ServantTemp.Value[j];
		}

		//Ёъ±onpcЕг№іёк®Ж
		GameObjDbStructEx* NpcObjDB = Global::GetObjDB( ServantTemp.NPCObjID );
		if( NpcObjDB == NULL )
		{
			Print( LV3 , "_OnTime_UpdateServantHireInfo" , "NPCObjID=%d" , ServantTemp.NPCObjID );
			continue;
		}

		GameObjDbStructEx* ImageObjDB = Global::GetObjDB( NpcObjDB->ImageID );
		if( ImageObjDB == NULL )
		{
			Print( LV3 , "_OnTime_UpdateServantHireInfo" , "Image=%d" , NpcObjDB->ImageID );
			//qqґъёХјИ®Йmark±ј
			continue;
		}

		for( int i = 0 ; i < 15 ; i++ )
		{
			ServantTemp.Look.BoneScale[i] = 100;
		}
		//qqґъёХјИ®Йmark±ј
		ServantTemp.Look.BoneScale[ 3 ] = char( ImageObjDB->Image.BoneScaleChest   );	
		ServantTemp.Look.BoneScale[ 5 ] = char( ImageObjDB->Image.BoneScaleButtock );	
		ServantTemp.Look.BoneScale[ 4 ] = char( ImageObjDB->Image.BoneScaleHand    );	
		ServantTemp.Look.BoneScale[ 8 ] = char( ImageObjDB->Image.BoneScaleLeg	   );	
		ServantTemp.Look.BoneScale[ 1 ] = char( ImageObjDB->Image.BoneScaleBra	   );	
		

		_ServantList[_ServantMaxID] = ServantTemp;
	}

	Print( LV2 , "_OnTime_UpdateServantHireInfo" , "Process.... _ServantList.size() = %d _ServantMaxID=%d" , _ServantList.size() , _ServantMaxID );

	Global::Schedular()->Push( _OnTime_UpdateServantHireInfo , 60*60*1000 , NULL, NULL );	
	return 0;
}