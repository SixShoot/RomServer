#include "../NetWakerGSrvInc.h"
#include "NetSrv_GuildHousesChild.h"
#include "../../mainproc/GuildHouseManage/GuildHousesClass.h"
//--------------------------------------------------------------------------------------------------------------
bool    NetSrv_GuildHousesChild::Init()
{
	NetSrv_GuildHouses::_Init();

	if( This != NULL)
		return false;

	This = NEW( NetSrv_GuildHousesChild );

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool    NetSrv_GuildHousesChild::Release()
{
	if( This == NULL )
		return false;

	NetSrv_GuildHouses::_Release();

	delete This;
	This = NULL;

	return true;

}

void NetSrv_GuildHousesChild::RD_BuildHouseResult	( int PlayerDBID , int GuildID , bool Result )
{
	RoleDataEx* Owner = Global::GetRoleDataByDBID( PlayerDBID );
	if( Owner == NULL )
		return;

	SC_BuildHouseResult( Owner->GUID() , GuildID , Result );
}
void NetSrv_GuildHousesChild::RD_FindHouseResult	( int PlayerDBID , int GuildID , bool Result )
{
	RoleDataEx* Player = Global::GetRoleDataByDBID( PlayerDBID );
	if( Player == NULL )
		return;

	if( Result != false )
	{
		int GuildHouse_ZoneID = g_ObjectData->GetSysKeyValue( "GuildHouse_ZoneID" );
		float X		= (float)g_ObjectData->GetSysKeyValue( "GuildHouse_X" );
		float Y		= (float)g_ObjectData->GetSysKeyValue( "GuildHouse_Y" );
		float Z		= (float)g_ObjectData->GetSysKeyValue( "GuildHouse_Z" );
		float Dir	= (float)g_ObjectData->GetSysKeyValue( "GuildHouse_Dir" );
		SC_CloseVisitHouse( Player->GUID() );
		Player->PlayerTempData->VisitGuildID = GuildID;
		ChangeZone( Player->GUID() , GuildHouse_ZoneID , 0 , X , Y , Z , Dir );
	}
	else
	{
		SC_FindHouseResult( Player->GUID() , Result );
	}

}
void NetSrv_GuildHousesChild::RC_VisitHouseRequest	( BaseItemObject* Obj , int GuildID)
{
	RoleDataEx* Role = Obj->Role();
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_VisitGuildHouse ) == false )
		return;

	//如果目標公會不能參關則回傳錯誤
	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildID );	
	if( Guild == NULL || Guild->Base.IsOwnHouse == false )
	{
		SC_FindHouseResult( Role->GUID() , false );
		return;
	}
	if( Guild->Base.IsLockGuildHouse != false && GuildID != Role->GuildID() )
	{
		SC_FindHouseResult( Role->GUID() , false );
		return;
	}

	SD_FindHouseRequest( Obj->Role()->DBID() , GuildID );
}
void NetSrv_GuildHousesChild::RC_CloseVisitHouse	( BaseItemObject* Obj )
{
	RoleDataEx* Role = Obj->Role();
	if( Role->CheckOpenMenu( EM_RoleOpenMenuType_VisitGuildHouse ) == false )
		return;

	Role->ClsOpenMenu();
}
void NetSrv_GuildHousesChild::RC_LeaveHouse			( BaseItemObject* Obj , int PlayerDBID )
{
	RoleDataEx* Role = Obj->Role();
	if( Ini()->IsGuildHouseZone == false )
		return;

	/*
	HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerTempData->VisitHouseDBID );
	if( houseClass != NULL )
	{
		//		if( PlayerDBID != Role->DBID() && stricmp( houseClass->HouseBase().Info.AccountID.Begin() , Role->Account_ID() ) != 0 )
		if( PlayerDBID != Role->DBID() && PlayerDBID != houseClass->HouseBase().Info.OwnerDBID )
		{
			Role->Msg("沒有權限踢除其他玩家");
			return;
		}
	}
	*/
	RoleDataEx* Target = Global::GetRoleDataByDBID( PlayerDBID );
	if( Target == NULL )
	{
		Role->Msg("目標不存在");
		return;
	}

	int ZoneID = GlobalBase::GetParallelZoneID( Target->SelfData.ReturnZoneID ,  Target->PlayerTempData->ParallelZoneID );
	if( ZoneID == -1 )
		return;
	Global::ChangeZone( Role->GUID() , ZoneID , 0 , Target->SelfData.ReturnPos.X , Target->SelfData.ReturnPos.Y , Target->SelfData.ReturnPos.Z , Target->SelfData.ReturnPos.Dir );

}


void NetSrv_GuildHousesChild::RD_HouseBaseInfo		( GuildHousesInfoStruct& HouseBaseInfo )
{

	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( HouseBaseInfo.GuildID );
	if( houseClass == NULL )
		return;

	houseClass->SetHouseBase( HouseBaseInfo );

}
void NetSrv_GuildHousesChild::RD_ItemInfo			( int GuildID , int PageID , int ItemCount , GuildHouseItemStruct Item[ _DEF_GUIDHOUSEITEM_MAX_PAGEITEM_ ] )
{
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( GuildID );
	if( houseClass == NULL )
		return;

	for( int i = 0 ; i < ItemCount ; i++ )
	{
		Item[i].State = EM_GuildHouseItemInfoState_None;
		if( houseClass->AddItem( Item[i] ) == false )
		{
			Print( LV3 ,"RD_ItemInfo" , "Additem == false GuildID=%d PageID=%d PosID=%d" , Item[i].GuildID , Item[i].PageID , Item[i].PosID );
		};
	}
	
}
void NetSrv_GuildHousesChild::RD_HouseInfoLoadOK		( int GuildID )
{
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( GuildID );
	if( houseClass == NULL )
		return;

	houseClass->LoadOK();

	//通知所有的有關的玩家
	RoleDataEx *Role;

	set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//計算每個房間的人數
	for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if( Obj == NULL )
			continue;
		Role = Obj->Role();
		if( Role->RoomID() != houseClass->RoomID() )
			continue;

		SC_StoreConfigProc( Obj );
		SC_AllFurnitureItemInfo( Role->GUID() );
		SC_AllBuildingActiveType( Role->GUID() );
		SC_HouseInfoLoadOK( Role->GUID() );

	}
}
void NetSrv_GuildHousesChild::RD_BuildingInfo		( GuildHouseBuildingInfoStruct&	Building )
{
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Building.GuildID );
	if( houseClass == NULL )
		return;

	houseClass->AddBuilding( Building );
	
}

//////////////////////////////////////////////////////////////////////////
// 公會建築
//////////////////////////////////////////////////////////////////////////
bool	NetSrv_GuildHousesChild::CheckBuildAbility( int PlayerDBID )
{
	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );	
	if( Member == NULL )
		return false;

	if( Member->Guild->Base.LeaderDBID == PlayerDBID )
		return true;

	int Rank = Member->Rank;
	if( (unsigned)Rank >= EM_GuildRank_Count )
		return false;

	return Member->Guild->Base.Rank[Rank].Setting.Building;
}

bool	NetSrv_GuildHousesChild::CheckFunitureAbility( int PlayerDBID )
{
	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );	
	if( Member == NULL )
		return false;

	if( Member->Guild->Base.LeaderDBID == PlayerDBID )
		return true;

	int Rank = Member->Rank;
	if( (unsigned)Rank >= EM_GuildRank_Count )
		return false;

	return Member->Guild->Base.Rank[Rank].Setting.Furniture;
}
bool	NetSrv_GuildHousesChild::CheckSetStyleAbility( int PlayerDBID )
{
	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );	
	if( Member == NULL )
		return false;

	if( Member->Guild->Base.LeaderDBID == PlayerDBID )
		return true;

	int Rank = Member->Rank;
	if( (unsigned)Rank >= EM_GuildRank_Count )
		return false;

	return Member->Guild->Base.Rank[Rank].Setting.SetGuildHouseStyle;
}


void NetSrv_GuildHousesChild::RC_CreateBuildingRequest	( BaseItemObject* Obj , char* Point , int BuildingID , int ParentBuildingDBID , float Dir )
{
	RoleDataEx* Role = Obj->Role();

	//-------------------------------------------------------------------------------------------------
	//檢查此人是否能存取
	if( CheckBuildAbility( Role->DBID() ) == false )
	{
		SC_CreateBuildingResult( Role->GUID() , Point , BuildingID , -1 , ParentBuildingDBID , false );
		return;
	}
	//-------------------------------------------------------------------------------------------------

	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Role->GuildID() );

	if( houseClass == NULL || houseClass->BuildingMap().size() >= _MAX_GUILD_BUILDING_COUNT_ || houseClass->RoomID() != Role->RoomID() )
	{
		SC_CreateBuildingResult( Role->GUID() , Point , BuildingID , -1 , ParentBuildingDBID , false );
		return;
	}


	GuildBuildingInfoTableStruct* BuildingTableInfo = g_ObjectData->GuildBuilding( BuildingID );
	if( BuildingTableInfo == NULL )
	{
		SC_CreateBuildingResult( Role->GUID() , Point , BuildingID , -1 , ParentBuildingDBID , false );
		return;
	}

	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( Role->GuildID() );
	if( GuildInfo == NULL )
	{	//沒有公會
		SC_CreateBuildingResult( Role->GUID() , Point , BuildingID , -1 , ParentBuildingDBID , false );	
		return; 
	}
	//////////////////////////////////////////////////////////////////////////
	//檢查公會等級
	if( GuildInfo->Base.Level < BuildingTableInfo->NeedGuildLv )
	{
		SC_CreateBuildingResult( Role->GUID() , Point , BuildingID , -1 , ParentBuildingDBID , false );	
		return; 
	}
	//////////////////////////////////////////////////////////////////////////
	//檢查是否有前置的建設
	//////////////////////////////////////////////////////////////////////////
	if(		houseClass->GetMaxBuildLv( BuildingTableInfo->NeedTypeStr1) < BuildingTableInfo->NeedLv1 
		||	houseClass->GetMaxBuildLv( BuildingTableInfo->NeedTypeStr2) < BuildingTableInfo->NeedLv2 )
	{
		SC_CreateBuildingResult( Role->GUID() , Point , BuildingID , -1 , ParentBuildingDBID , false );	
		return; 
	}
	//////////////////////////////////////////////////////////////////////////
	//收尋某個點的建築資料
	if( houseClass->GetBuildingInfo( ParentBuildingDBID , Point ) != NULL )
	{
		SC_CreateBuildingResult( Role->GUID() , Point , BuildingID , -1 , ParentBuildingDBID , false );	
		return; 
	};
	//////////////////////////////////////////////////////////////////////////


	GuildResourceStruct& Resource = GuildInfo->Base.Resource;

	for( int i = 0 ; i < 7 ; i++ )
	{
		if(		Resource._Value[i] < BuildingTableInfo->Resource._Value[i] 
			||	BuildingTableInfo->Resource._Value[i] < 0 )
		{
			SC_CreateBuildingResult( Role->GUID() , Point , BuildingID , -1 , ParentBuildingDBID , false );	
			return; 
		}
	}

	GuildHouseBuildingInfoStruct Building;
	Building.Init();


	Building.GuildID = Role->GuildID();
	Building.ParentDBID = ParentBuildingDBID;
	Building.BuildingID = BuildingID;
	Building.Dir	= Dir;

	strncpy( Building.PointStr , Point , sizeof(Building.PointStr) );
	memset( Building.Value , 0 , sizeof( Building.Value ) );

	int RetBuildingDBID = houseClass->AddBuilding( Building );

	bool Result = true;
	if( RetBuildingDBID == 0 )
		Result = false;

	GuildResourceStruct NeedResource;

	for( int i = 0 ; i < 7 ; i++ )
	{
		Resource._Value[i] -= BuildingTableInfo->Resource._Value[i];
		NeedResource._Value[i] = BuildingTableInfo->Resource._Value[i] * -1;
	}

	NetSrv_Guild::SD_AddGuildResource( Role->GuildID() , Role->DBID() , NeedResource , "" , EM_AddGuildResourceType_Build , BuildingID );
	SC_CreateBuildingResult( Role->GUID() , Point , BuildingID , RetBuildingDBID , ParentBuildingDBID , Result );

	//通知所有人有新的公會屋資料
	if( Result != false )
	{
		GuildHouseBuildingStruct*	GuildBuildingInfo = houseClass->GetBuildingInfo( RetBuildingDBID );
		if( GuildBuildingInfo != NULL  )
			SC_AllZone_NewBuildingInfo( Obj , GuildBuildingInfo->ObjGUID , Building );
	}
}
void NetSrv_GuildHousesChild::RC_BuildingUpgradeRequest	( BaseItemObject* Obj , int BuildingDBID , int UpgradeBuildingID )
{
	RoleDataEx* Role = Obj->Role();

	//-------------------------------------------------------------------------------------------------
	//檢查此人是否能存取
	if( CheckBuildAbility( Role->DBID() ) == false )
	{
		SC_BuildingUpgradeResult( Role->GUID() , -1 , BuildingDBID , UpgradeBuildingID , false );
		return;
	}
	//-------------------------------------------------------------------------------------------------

	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Role->GuildID() );

	if( houseClass == NULL )
		return;

	if( houseClass->RoomID() != Role->RoomID() )
		return;

	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( Role->GuildID() );
	if( GuildInfo == NULL )
	{	//沒有公會
		SC_BuildingUpgradeResult( Role->GUID() , -1 , BuildingDBID , UpgradeBuildingID , false );
		return; 
	}
	//////////////////////////////////////////////////////////////////////////
	//檢查公會等級
	GuildBuildingInfoTableStruct* Up_BTableInfo = g_ObjectData->GuildBuilding( UpgradeBuildingID );
	if( Up_BTableInfo == NULL || GuildInfo->Base.Level < Up_BTableInfo->NeedGuildLv )
	{
		SC_BuildingUpgradeResult( Role->GUID() , -1 , BuildingDBID , UpgradeBuildingID , false );
		return; 
	}

	int NewBuildGItemID = houseClass->BuildingUpgrade( BuildingDBID , UpgradeBuildingID );

	if( NewBuildGItemID != 0 )
	{
		GuildResourceStruct& Resource = GuildInfo->Base.Resource;
		GuildResourceStruct NeedResource;
		for( int i = 0 ; i < 7 ; i++ )
		{
			Resource._Value[i] -= Up_BTableInfo->Resource._Value[i];
			NeedResource._Value[i] = Up_BTableInfo->Resource._Value[i] * -1;
		}

		NetSrv_Guild::SD_AddGuildResource( Role->GuildID() , Role->DBID() , NeedResource , "" , EM_AddGuildResourceType_Upgrade , UpgradeBuildingID );
		SC_All_BuildingUpgradeResult( Obj , NewBuildGItemID , BuildingDBID , UpgradeBuildingID );
	}
	else
	{
		SC_BuildingUpgradeResult( Role->GUID() , -1 , BuildingDBID , UpgradeBuildingID , false );
	}

	
}
void NetSrv_GuildHousesChild::RC_DeleteBuildingRequest	( BaseItemObject* Obj , int BuildingDBID )
{
	RoleDataEx* Role = Obj->Role();

	//-------------------------------------------------------------------------------------------------
	//檢查此人是否能存取
	if( CheckBuildAbility( Role->DBID() ) == false )
	{
		SC_DeleteBuildingResult( Role->GUID() , BuildingDBID , false );
		return;
	}
	//-------------------------------------------------------------------------------------------------

	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Role->GuildID() );
	if( houseClass == NULL )
		return;

	if( houseClass->RoomID() != Role->RoomID() )
		return;

	GuildHouseBuildingStruct* BuildingInfo = houseClass->GetBuildingInfo( BuildingDBID );
	if( BuildingInfo == NULL )
		return;

	GuildBuildingInfoTableStruct* BuildingTableInfo = g_ObjectData->GuildBuilding( BuildingInfo->Info.BuildingID );
	if( BuildingTableInfo == NULL )
		return;

	bool Result = houseClass->DelBuilding( BuildingDBID );

	SC_DeleteBuildingResult( Role->GUID() , BuildingDBID , Result );

	if( Result != false )
	{
		GuildResourceStruct NeedResource = BuildingTableInfo->Resource;;
		NeedResource.Gold		= int( NeedResource.Gold * 0.65f + 0.5f);
		NeedResource.BonusGold	= int( NeedResource.BonusGold * 0.90f + 0.5f);
		NeedResource.Mine		= int( NeedResource.Mine * 0.65f + 0.5f);
		NeedResource.Wood		= int( NeedResource.Wood * 0.65f + 0.5f);
		NeedResource.Herb		= int( NeedResource.Herb * 0.65f + 0.5f);
		NeedResource.GuildRune	= int( NeedResource.GuildRune * 0.90f + 0.5f);
		NeedResource.GuildStone	= int( NeedResource.GuildStone * 0.90f + 0.5f);
		NetSrv_Guild::SD_AddGuildResource( Role->GuildID() , Role->DBID() , NeedResource , "" , EM_AddGuildResourceType_BuildDestroy , BuildingTableInfo->ID );

		SC_AllZone_DelBuildingInfo( Obj , BuildingDBID );
	}
}

//////////////////////////////////////////////////////////////////////////
// 公會倉庫
//////////////////////////////////////////////////////////////////////////
void NetSrv_GuildHousesChild::RC_HouseItemRequest		( BaseItemObject* Obj , int PageID )
{
	RoleDataEx* Role = Obj->Role();

	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Role->GuildID() );
	if( houseClass == NULL || houseClass->RoomID() != Role->RoomID() )
		return;
	
	GuildHousesInfoStruct*	HouseBase = houseClass->GetHouseBase();
	if( HouseBase == NULL )
		return;

	//取得某一頁的房屋倉庫資料
	map< int , GuildHouseItemStruct >* PageItemList = houseClass->ItemBox( PageID );

	SC_HouseItemInfo( Obj->GUID() , HouseBase->ItemMaxPageCount , PageID , houseClass->GetLootCount( PageID,Role->DBID() ) , PageItemList );
}
void NetSrv_GuildHousesChild::RC_SwapBodyHouseItem		( BaseItemObject* Obj , int BodyPos , int HouseItemPageID , int HouseItemPos , bool IsFromBody )
{
	RoleDataEx* Role = Obj->Role();

	if( Role->TempData.Sys.OnChangeZone != false )
		return;


	if( Role->BaseData.SysFlag.DisableTrade != false )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
		SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HouseItemPageID , HouseItemPos , -1 , false );
		return;
	}

	/*
	if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
	{
		Role->Msg("不是自己的房子");
		return;
	}
	*/

	//GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Role->GuildID() );
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj_ByRoom( Role->RoomID() );
	if( houseClass == NULL || houseClass->CheckLoadOK() == false )
	{
		SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HouseItemPageID , HouseItemPos , -1 , false );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	ItemFieldStruct* bodyItem = Role->GetBodyItem( BodyPos );

	if( bodyItem == NULL )
	{
		Role->Msg("包裹資料位置有問題");
		SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HouseItemPageID , HouseItemPos , -1 , false );
		return;
	}
	
	int	GetCount;

	if( houseClass->SwapItem( Role , *bodyItem , HouseItemPageID , HouseItemPos , IsFromBody , GetCount ) == false )
	{
		Role->Msg( "交換失敗" );
		SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HouseItemPageID , HouseItemPos , GetCount , false );
		return;
	}


	GuildHouseItemStruct* houseItem = houseClass->GetItem( HouseItemPageID , HouseItemPos );

	SC_FixHouseItemProc( Role , houseItem  , HouseItemPageID , HouseItemPos );

	Role->Net_FixItemInfo_Body( BodyPos );
	SC_SwapBodyHouseItemResult( Role->GUID() , BodyPos , HouseItemPageID , HouseItemPos , GetCount , true );


}
void NetSrv_GuildHousesChild::RC_SwapHouseItemRequest	( BaseItemObject* Obj , int HouseItemPageID[2] , int HouseItemPos[2] )
{
	RoleDataEx* Role = Obj->Role();


	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Role->GuildID() );
	if( houseClass == NULL || houseClass->RoomID() != Role->RoomID() || houseClass->CheckLoadOK() == false )
	{
		SC_SwapHouseItemResult	( Role->GUID() , HouseItemPos , HouseItemPageID , false );	
		return;
	}

	if( HouseItemPageID[0] == HouseItemPageID[1] && HouseItemPos[0] == HouseItemPos[1] )
	{
		SC_SwapHouseItemResult	( Role->GUID() , HouseItemPos , HouseItemPageID , false );	
		Role->Msg( "不需搬移" );
		return;
	}

	if( houseClass->SwapItem( Role , HouseItemPageID[0] , HouseItemPos[0] , HouseItemPageID[1] , HouseItemPos[1] ) == false)
	{
		SC_SwapHouseItemResult	( Role->GUID() , HouseItemPos , HouseItemPageID , false );	
		Role->Msg( "物品搬移失敗" );
		return;
	}

	GuildHouseItemStruct* houseItem1 = houseClass->GetItem(  HouseItemPageID[0] ,  HouseItemPos[0] );	
	SC_FixHouseItemProc( Role , houseItem1 , HouseItemPageID[0] ,  HouseItemPos[0] );
	GuildHouseItemStruct* houseItem2 = houseClass->GetItem(  HouseItemPageID[1] ,  HouseItemPos[1] );
	SC_FixHouseItemProc( Role , houseItem2 , HouseItemPageID[1] ,  HouseItemPos[1] );

	SC_SwapHouseItemResult	( Role->GUID() , HouseItemPos , HouseItemPageID , true );	

}
void NetSrv_GuildHousesChild::SC_FixHouseItemProc			( RoleDataEx* Owner , GuildHouseItemStruct* HouseItem , int PageID , int ItemPos )
{
	GuildHouseItemStruct ItemTemp;
	if( HouseItem == NULL )
	{		
		ItemTemp.GuildID = Owner->GuildID();
		ItemTemp.ItemDBID = -1;
		ItemTemp.PageID = PageID;
		ItemTemp.PosID = ItemPos;
		ItemTemp.State = EM_GuildHouseItemInfoState_None;
		ItemTemp.Item.Init();
		HouseItem = &ItemTemp;
	}	

	PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

	int			i , j;
	PlayID*		TopID;

	for( i = 0 ; Block[i] != NULL ; i++ )
	{
		TopID = Block[i]->Begin;

		for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
		{
			BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
			if( OtherObj == NULL )
				continue;

			SC_FixHouseItem( OtherObj->GUID() , HouseItem );
		}
	}
}

void NetSrv_GuildHousesChild::SC_FixHouseFurnitureProc			( BaseItemObject* Obj , int GuildID , GuildHouseFurnitureItemStruct* HouseItem , int ItemPos )
{
	GuildHouseFurnitureItemStruct ItemTemp;
	if( HouseItem == NULL )
	{		
		ItemTemp.Init();
		ItemTemp.GuildID = GuildID;
		ItemTemp.ItemDBID = -1;
		ItemTemp.Pos = ItemPos;
		ItemTemp.State = EM_GuildHouseItemInfoState_None;

		HouseItem = &ItemTemp;
	}	
	SC_All_FixHouseFurniture( Obj , *HouseItem );
	
}
//////////////////////////////////////////////////////////////////////////
// 要求建築物資料
//////////////////////////////////////////////////////////////////////////
void NetSrv_GuildHousesChild::RC_AllBuildingInfoRequest( BaseItemObject* Obj , int GuildID )
{
	SC_AllBuildingInfo( Obj->GUID() , GuildID );
}
//////////////////////////////////////////////////////////////////////////
// 傢俱
//////////////////////////////////////////////////////////////////////////
void NetSrv_GuildHousesChild::RD_FurnitureItemInfo			( GuildHouseFurnitureItemStruct& Item )
{
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Item.GuildID );
	if( houseClass == NULL )
		return;
	
	houseClass->AddFurniture( Item );

}
void NetSrv_GuildHousesChild::RC_SwapBodyHouseFurniture		( BaseItemObject* Obj , int BodyPos , int HousePos )
{
	RoleDataEx* Role = Obj->Role();

	//-------------------------------------------------------------------------------------------------
	//檢查此人是否能存取
	if( CheckFunitureAbility( Role->DBID() ) == false )
	{
		SC_SwapBodyHouseFurnitureResult( Role->GUID() , BodyPos , HousePos , false );
		return;
	}
	//-------------------------------------------------------------------------------------------------

	if( Role->TempData.Sys.OnChangeZone != false )
		return;

	if( Role->BaseData.SysFlag.DisableTrade != false )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
		SC_SwapBodyHouseFurnitureResult( Role->GUID() , BodyPos , HousePos , false );
		return;
	}

	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Role->GuildID() );
	if( houseClass == NULL || houseClass->CheckLoadOK() == false )
	{
		SC_SwapBodyHouseFurnitureResult( Role->GUID() , BodyPos , HousePos , false );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	if( houseClass->GetHouseBase()->FurnitureMaxCount <= HousePos )
	{
		SC_SwapBodyHouseFurnitureResult( Role->GUID() , BodyPos , HousePos , false );
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	ItemFieldStruct* bodyItem = Role->GetBodyItem( BodyPos );

	if( bodyItem == NULL )
	{
		Role->Msg("包裹資料位置有問題");
		SC_SwapBodyHouseFurnitureResult( Role->GUID() , BodyPos , HousePos , false );
		return;
	}


	if( houseClass->SwapFurniture( Role , *bodyItem , HousePos , true ) == false )
	{
		Role->Msg( "交換失敗" );
		SC_SwapBodyHouseFurnitureResult( Role->GUID() , BodyPos , HousePos , false );
		return;
	}


	GuildHouseFurnitureItemStruct* houseItem = houseClass->GetFurniture( HousePos );
	//if( houseItem )
	//	SC_All_FixHouseFurniture( houseItem );
	SC_FixHouseFurnitureProc( Obj , Role->GuildID() , houseItem , HousePos );

	Role->Net_FixItemInfo_Body( BodyPos );
	SC_SwapBodyHouseFurnitureResult( Role->GUID() , BodyPos , HousePos , true );
}
void NetSrv_GuildHousesChild::RC_SwapHouseFurnitureRequest	( BaseItemObject* Obj , int Pos[2] )
{
	RoleDataEx* Role = Obj->Role();

	//-------------------------------------------------------------------------------------------------
	//檢查此人是否能存取
	if( CheckFunitureAbility( Role->DBID() ) == false )
	{
		SC_SwapHouseFurnitureResult( Role->GUID() , Pos , false );
		return;
	}
	//-------------------------------------------------------------------------------------------------

	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Role->GuildID() );
	if( houseClass == NULL || houseClass->CheckLoadOK() == false )
	{
		SC_SwapHouseFurnitureResult( Role->GUID() , Pos , false );	
		return;
	}

	if( Pos[1] == Pos[0] )
	{
		SC_SwapHouseFurnitureResult( Role->GUID() , Pos , false );
		Role->Msg( "不需搬移" );
		return;
	}

	if( houseClass->SwapFurniture( Pos[0] , Pos[1] ) == false)
	{
		SC_SwapHouseFurnitureResult( Role->GUID() , Pos , false );	
		Role->Msg( "物品搬移失敗" );
		return;
	}

	GuildHouseFurnitureItemStruct* houseItem1 = houseClass->GetFurniture( Pos[0] );	
	SC_FixHouseFurnitureProc	( Obj , Role->GuildID() , houseItem1 , Pos[0] );

	GuildHouseFurnitureItemStruct* houseItem2 = houseClass->GetFurniture(  Pos[1] );
	SC_FixHouseFurnitureProc	( Obj , Role->GuildID() , houseItem2 , Pos[1] );


	SC_SwapHouseFurnitureResult( Role->GUID() , Pos , true );	
}
void NetSrv_GuildHousesChild::RC_HouseItemLayoutRequest		( BaseItemObject* Obj , int BuildingObjID , int Pos , House3DLayoutStruct&	Layout )
{

	//-------------------------------------------------------------------------------------------------
	//檢查此人是否能存取
	if( CheckFunitureAbility( Obj->Role()->DBID() ) == false )
	{
		SC_HouseItemLayoutResult( Obj->GUID() , Pos , false );
		return;
	}
	//-------------------------------------------------------------------------------------------------
	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( Obj->Role()->DBID() );	
	if( Member == NULL  )
		return;

	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Obj->Role()->GuildID() );
	if( houseClass == NULL )
	{
		SC_HouseItemLayoutResult( Obj->GUID() , Pos , false );
		return;
	}

	GuildHouseFurnitureItemStruct* houseItem = houseClass->GetFurniture( Pos );	
	if( houseItem == NULL )	
	{
		SC_HouseItemLayoutResult( Obj->GUID() , Pos , false );
		return;
	}

	if( Obj->Role()->DBID() != houseItem->OwnerDBID  && Obj->Role()->DBID() != Member->Guild->Base.LeaderDBID )
	{
		SC_HouseItemLayoutResult( Obj->GUID() , Pos , false );
		return;
	}

	map< int , int >::iterator Iter = houseClass->FurnitureDBIDtoObjGUIDMap().find( houseItem->ItemDBID );
	if( Iter == houseClass->FurnitureDBIDtoObjGUIDMap().end() )
	{
		BuildingObjID = -1;
	}
	else
	{
		BuildingObjID = Iter->second;
		houseClass->FurnitureDBIDtoObjGUIDMap().erase( Iter );
	}

	BaseItemObject* BuildingObj = Global::GetObj( BuildingObjID );

	if(		BuildingObj != NULL 
		&&	BuildingObj->Role()->TempData.GuildFurnitureDBID != houseItem->ItemDBID 
		&&  BuildingObj->Role()->TempData.GuildBuildingType == EM_GuildBuildingType_Furniture )
	{
		SC_HouseItemLayoutResult( Obj->GUID() , Pos , false );
		return;
	}
	
	if( BuildingObj != NULL  )
	{
		BuildingObj->Destroy( "RC_HouseItemLayoutRequest" );
	}

	houseItem->Layout = Layout;
	if( houseItem->State == EM_GuildHouseItemInfoState_None )
		houseItem->State = EM_GuildHouseItemInfoState_Update;
	houseClass->NeedSave();

	int ItemGUID = houseClass->CreateBuildingFurniture( houseItem->Item , houseItem->ItemDBID , houseItem->Layout );
	//_FurnitureDBIDtoObjGUIDMap[ houseItem->Item.ItemDBID ] = ItemGUID;
	houseClass->FurnitureDBIDtoObjGUIDMap()[ houseItem->ItemDBID ] = ItemGUID;
	SC_FixHouseFurnitureProc	( Obj , Obj->Role()->GuildID() , houseItem , Pos );
}

//////////////////////////////////////////////////////////////////////////
// 公會倉庫 權限設定
//////////////////////////////////////////////////////////////////////////
void NetSrv_GuildHousesChild::RD_StoreConfig( GuildStorehouseRightStruct& Config )
{
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Config.GuildID );
	if( houseClass == NULL )
		return;
	
	houseClass->SetItemRight( Config );

}
void NetSrv_GuildHousesChild::RC_SetStoreConfig( BaseItemObject* Obj , GuildStorehouseRightStruct& Config )
{
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Obj->Role()->GuildID() );
	if( houseClass == NULL )
		return;

	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( Obj->Role()->GuildID()  );
	if( GuildInfo == NULL )
	{	
		//Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_GuildNoFound );
		return; 
	}

	//檢查是否為會長
	if( GuildInfo->Base.LeaderDBID != Obj->Role()->DBID()  )
	{
		//Owner->Net_GameMsgEvent( EM_GameMessageEvent_Guild_NotLeader );
		SC_SetStoreConfigResult( Obj->GUID() , false );
		return;
	}
	
	if( houseClass->SetItemRight( Config ) != false )
	{
		char	Buf[ 4096 ];
		string StrData = StringToSqlX( (char*)Config.Rank , sizeof( Config.Rank ), false );
		sprintf( Buf , "update GuildHouse_Item_Right Set Config = CAST( %s AS varbinary(80) ) WHERE GuildID=%d and PageID=%d" , StrData.c_str() , Config.GuildID , Config.PageID );
		Net_DCBase::SqlCommand( Config.GuildID , Buf );
		

		SC_AllZone_StoreConfig( Obj , Config );
		SC_SetStoreConfigResult( Obj->GUID() , true );
	}
	else
	{
		SC_SetStoreConfigResult( Obj->GUID() , false );
	}
}

bool NetSrv_GuildHousesChild::SC_StoreConfigProc( BaseItemObject* Obj  )
{
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Obj->Role()->GuildID() );
	if( houseClass == NULL )
		return false;

	for( int i = 0 ; i < _DEF_GUIDHOUSEITEM_MAX_PAGE_ ; i++ )
	{
		if( houseClass->GetHouseBase()->ItemMaxPageCount <= i )
			break;
		GuildStorehouseRightStruct* Config = houseClass->GetItemRight( i );
		if( Config == NULL )
			continue;
		Config->GuildID = Obj->Role()->GuildID();
		Config->PageID	= i;
		SC_StoreConfig( Obj->GUID() , *Config );
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
// 公會倉庫 Log
//////////////////////////////////////////////////////////////////////////
void NetSrv_GuildHousesChild::RD_ItemLog			( GuildItemStoreLogStruct& ItemLog )
{
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( ItemLog.GuildID );
	if( houseClass == NULL )
		return ;

	houseClass->SetItemLog( ItemLog );
//	vector<GuildItemStoreLogStruct>* GetItemLog( int PageID , int DayBefore );
//	bool							 SetItemLog( GuildItemStoreLogStruct& Log );
}
void NetSrv_GuildHousesChild::RC_ItemLogRequest		( BaseItemObject* Obj , int PageID , int DayBefore )
{
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Obj->Role()->GuildID() );
	if( houseClass == NULL )
		return ;
	vector<GuildItemStoreLogStruct>* LogItemList = houseClass->GetItemLog( PageID , DayBefore );
	SC_ItemLogList( Obj->GUID() , LogItemList );
}

void NetSrv_GuildHousesChild::RC_ActiveBuildingRequest	( BaseItemObject* Obj , int BuildingDBID , bool IsActive )
{
	RoleDataEx* Role = Obj->Role();
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Obj->Role()->GuildID() );
	if( houseClass == NULL )
		return;
	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( Role->GuildID() );
	if( GuildInfo == NULL )
	{	//沒有公會
		return; 
	}
	if( GuildInfo->Base.LeaderDBID != Role->DBID()  )
	{
		SC_ActiveBuildingResult( Obj->GUID() , BuildingDBID , IsActive , false );
		return;
	}
	GuildHouseBuildingStruct* BuildingInfo = houseClass->GetBuildingInfo( BuildingDBID );

	if( BuildingInfo == NULL )
	{
		SC_ActiveBuildingResult( Obj->GUID() , BuildingDBID , IsActive , false );
		return;
	}

	RoleDataEx* BuildRole = Global::GetRoleDataByGUID(  BuildingInfo->ObjGUID );
	if( BuildRole == NULL )
	{
		SC_ActiveBuildingResult( Obj->GUID() , BuildingDBID , IsActive , false );
		return;
	}


	if( BuildingInfo->Info.IsActive != IsActive && IsActive != false )
	{
		GuildResourceStruct& Resource = GuildInfo->Base.Resource;

		//扣錢		
		GuildBuildingInfoTableStruct* BuildingTableInfo = g_ObjectData->GuildBuilding( BuildingInfo->Info.BuildingID );
		if( BuildingTableInfo == NULL )
		{
			Print( LV3 , "RC_ActiveBuildingRequest" , "BuildingTableInfo == NULL BuildingID=%d" , BuildingInfo->Info.BuildingID );
			return;
		}
		
		if( Resource.Gold < BuildingTableInfo->MaintenanceCharge*-1 )
		{
			SC_ActiveBuildingResult( Obj->GUID() , BuildingDBID , IsActive , false );
			return;
		}
		Resource.Gold += BuildingTableInfo->MaintenanceCharge;
		GuildResourceStruct NeedResource;
		NeedResource.Init();
		NeedResource.Gold = BuildingTableInfo->MaintenanceCharge;
		NetSrv_Guild::SD_AddGuildResource( Role->GuildID() , Role->DBID() , NeedResource , "" , EM_AddGuildResourceType_Active , BuildingInfo->Info.BuildingID );
	}



	BuildingInfo->Info.IsActive = IsActive;
	int ActiveBuffID = g_ObjectData->GetSysKeyValue( "GuildHouseActiveBuff" );
	//設定顯示的Buff
	if( IsActive )
	{
		BuffBaseStruct* RetBuf = BuildRole->AssistMagic( BuildRole , ActiveBuffID , 0 , false , -1 );
		if( RetBuf != NULL )
			NetSrv_MagicChild::SendRange_AddBuffInfo( BuildRole->GUID() , BuildRole->GUID() ,  RetBuf->BuffID , RetBuf->Power , RetBuf->Time );
	}
	else
	{
		BuildRole->ClearBuff( ActiveBuffID , -1 );
	}

	if( BuildingInfo->Info.State == EM_GuildHouseBuildingInfoState_None )
		BuildingInfo->Info.State = EM_GuildHouseBuildingInfoState_Update;
	houseClass->NeedSave();


	SC_ActiveBuildingResult( Obj->GUID() , BuildingDBID , IsActive , true );
	SC_All_BuildingActiveType( Role->RoomID() , BuildingDBID , IsActive );
}

void NetSrv_GuildHousesChild::RC_BuyItemPageRequest		( BaseItemObject* Obj , int PageID )
{
	RoleDataEx* Role = Obj->Role();
	if( g_ObjectData->_GuildHousePageCostTable.size() <= (unsigned)PageID )
	{
		SC_BuyItemPageResult( Obj->GUID() , PageID , false );
		return;
	}

	GuildHouseItemPageCostTableStruct&	ItemPageCost = g_ObjectData->_GuildHousePageCostTable[ PageID ];

	//GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Obj->Role()->GuildID() );
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj_ByRoom( Obj->Role()->RoomID() );
	if( houseClass == NULL )
		return;
	
	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( houseClass->GetHouseBase()->GuildID );
	if( GuildInfo == NULL )
	{	//沒有公會
		return; 
	}

	if( GuildInfo->Base.LeaderDBID != Role->DBID()  )
	{
		SC_BuyItemPageResult( Obj->GUID() , PageID , false );	
		return;
	}

	if( ItemPageCost.ItemPageID != houseClass->GetHouseBase()->ItemMaxPageCount )
	{
		SC_BuyItemPageResult( Obj->GUID() , PageID , false );	
		return;
	}

	//查看資源是否足夠
	GuildResourceStruct& Resource = GuildInfo->Base.Resource;

	for( int i = 0 ; i < 7 ; i++ )
	{
		if(		Resource._Value[i] < ItemPageCost.Resource._Value[i] 
		||	ItemPageCost.Resource._Value[i] < 0 )
		{
			SC_BuyItemPageResult( Obj->GUID() , PageID , false );	
			return; 
		}
	}

	GuildResourceStruct NeedResource;
	for( int i = 0 ; i < 7 ; i++ )
	{
		Resource._Value[i] -= ItemPageCost.Resource._Value[i];
		NeedResource._Value[i] = ItemPageCost.Resource._Value[i] * -1;
	}

	NetSrv_Guild::SD_AddGuildResource( Role->GuildID() , Role->DBID() , NeedResource , "" , EM_AddGuildResourceType_Buy_ItemPage , 0 );
	houseClass->GetHouseBase()->ItemMaxPageCount++;
	houseClass->NeedSave();

	SC_StoreConfigProc( Obj );
	SC_All_BuyItemPageResult( Obj , PageID , true );
}

void NetSrv_GuildHousesChild::SC_AllFurnitureItemInfo		( int SendID )
{
	//BaseItemObject* Obj
	RoleDataEx* Role = Global::GetRoleDataByGUID( SendID );
	if( Role == NULL )
		return;
	GuildHousesManageClass* GuildHouse = GuildHousesManageClass::GetHouseObj_ByRoom( Role->RoomID() );
	if( GuildHouse == NULL )
		return;
	map< int , GuildHouseFurnitureItemStruct >::iterator Iter;

	for( Iter = GuildHouse->FurnitureItemMap().begin() ; Iter != GuildHouse->FurnitureItemMap().end() ; Iter++ )
	{
		SC_FurnitureItemInfo( Role->GUID() , Iter->second );
	}
}

void NetSrv_GuildHousesChild::SC_AllBuildingActiveType	( int PlayerGUID )
{
	RoleDataEx* Role = Global::GetRoleDataByGUID( PlayerGUID );
	if( Role == NULL )
		return;
	GuildHousesManageClass* GuildHouse = GuildHousesManageClass::GetHouseObj_ByRoom( Role->RoomID() );
	if( GuildHouse == NULL )
		return;
	map< int , GuildHouseBuildingStruct >::iterator Iter;

	for( Iter = GuildHouse->BuildingMap().begin() ; Iter != GuildHouse->BuildingMap().end() ; Iter++ )
	{
		if( Iter->second.Info.IsActive == false )
			continue;
		SC_BuildingActiveType( Role->GUID() , Iter->second.Info.BuildingDBID , true );
	}
}

void NetSrv_GuildHousesChild::RC_FurnitureSendBack			( BaseItemObject* Obj , int Pos )
{
	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( Obj->Role()->DBID() );	

	if( Member == NULL || Member->Guild->Base.LeaderDBID != Obj->Role()->DBID() )
	{
		SC_FurnitureSendBackResult( Obj->GUID() , Pos , false );
		return ;
	}
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Obj->Role()->GuildID() );
	if( houseClass == NULL )
	{
		SC_FurnitureSendBackResult( Obj->GUID() , Pos , false );
		return;
	}

	GuildHouseFurnitureItemStruct* houseItem = houseClass->GetFurniture( Pos );	
	if( houseItem == NULL )	
	{
		SC_FurnitureSendBackResult( Obj->GUID() , Pos , false );
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	MailBaseInfoStruct MailBaseInfo;
	MailBaseInfo.Init();
	//memset( &MailBaseInfo , 0 , sizeof(MailBaseInfo) );
	MailBaseInfo.Item[0] = houseItem->Item;

	MailBaseInfo.IsSystem = true;
	MailBaseInfo.LiveTime = 60*24*30;
	//MailBaseInfo.OrgTargetName = SendToName;
	MailBaseInfo.OrgSendName = Obj->Role()->RoleName();
	MailBaseInfo.Title = "[SYS_GUILDHOUSE_SENDBACK_TITLE]";

	char	Buf[512];
	sprintf( Buf ,"[SYS_GUILDHOUSE_SENDBACK_CONTENT][$SETVAR1=[%d]]" , houseItem->Item.OrgObjID	);

	NetSrv_Mail::SD_SendMail( -1 , "" , MailBaseInfo , houseItem->OwnerDBID );
	//////////////////////////////////////////////////////////////////////////
	houseClass->DelFurniture( Pos );

	SC_FixHouseFurnitureProc	( Obj , Obj->Role()->GuildID() , NULL , Pos );
	SC_FurnitureSendBackResult( Obj->GUID() , Pos , true );
}

void NetSrv_GuildHousesChild::RC_BuyHouseStyle		( BaseItemObject* Obj , int Type )
{
	RoleDataEx* Role = Obj->Role();
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj_ByRoom( Obj->Role()->RoomID() );
	if( houseClass == NULL )
		return;

	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( houseClass->GetHouseBase()->GuildID );
	if( GuildInfo == NULL )
		return; 

	//檢查此人是否能存取
	if( CheckSetStyleAbility( Role->DBID() ) == false )
	{
		SC_BuyHouseStyleResult( Role->GUID() , Type , false );
		return;
	}
	if( (unsigned)GuildInfo->Base.Level >= g_ObjectData->GuildLvTable().size() )
	{
		SC_BuyHouseStyleResult( Role->GUID() , Type , false );
		return;
	}
	//檢查工會等級是否可以房屋樣式修改
	GuildLvInfoStruct& GuildLvInfo = g_ObjectData->GuildLvTable()[ GuildInfo->Base.Level ];
	if( GuildLvInfo.ChangeGuildStyle == 0 )
	{
		SC_BuyHouseStyleResult( Role->GUID() , Type , false );
		return;
	}

/*
	if( GuildInfo->Base.LeaderDBID != Role->DBID()  )
	{
		SC_BuyHouseStyleResult( Obj->GUID() , false );	
		return;
	}

	if(  houseClass->GetHouseBase()->GuildStyleRight & ( 1 << Type )  )
	{
		SC_BuyHouseStyleResult( Obj->GUID() , false );	
		return;
	}
	*/
	if( houseClass->GetHouseBase()->GuildStyle == Type )
	{
		SC_BuyHouseStyleResult( Obj->GUID() , Type , false );	
		return;
	}

	GuildHouseNpcReplaceCostTableStruct* CostInfo = g_ObjectData->GetGuildHouseNpcReplaceCost( Type );
	if( CostInfo == NULL )
	{
		SC_BuyHouseStyleResult( Obj->GUID() , Type , false );
		return;
	}

	if( GuildInfo->Base.Level < CostInfo->GuildLv )
	{
		SC_BuyHouseStyleResult( Obj->GUID() , Type , false );
		return;
	}

	//查看資源是否足夠
	GuildResourceStruct& Resource = GuildInfo->Base.Resource;

	for( int i = 0 ; i < 7 ; i++ )
	{
		if(		Resource._Value[i] < CostInfo->Cost._Value[i] 
		||	CostInfo->Cost._Value[i] < 0 )
		{
			SC_BuyHouseStyleResult( Obj->GUID() , Type , false );
			return; 
		}
	}

	GuildResourceStruct NeedResource;
	for( int i = 0 ; i < 7 ; i++ )
	{
		Resource._Value[i] -= CostInfo->Cost._Value[i];
		NeedResource._Value[i] = CostInfo->Cost._Value[i] * -1;
	}

	NetSrv_Guild::SD_AddGuildResource( Role->GuildID() , Role->DBID() , NeedResource , "" , EM_AddGuildResourceType_Buy_GuildStyle , 0 );
	houseClass->GetHouseBase()->GuildStyleRight |= ( 1 << Type );
	houseClass->GetHouseBase()->GuildStyle = Type;
	houseClass->NeedSave();

	//reset funiture
	{
		{
			map< int , GuildHouseFurnitureItemStruct >::iterator Iter;
			for( Iter = houseClass->FurnitureItemMap().begin() ; Iter != houseClass->FurnitureItemMap().end() ; Iter++ )
			{
				int pos = Iter->first;
				GuildHouseFurnitureItemStruct& item = Iter->second;

				item.Layout.IsShow = false;
				if( item.State == EM_GuildHouseItemInfoState_None )
					item.State = EM_GuildHouseItemInfoState_Update;

				SC_FixHouseFurnitureProc	( Obj , Obj->Role()->GuildID() , &item , item.Pos );
			}
		}

		{
			map< int , int >::iterator Iter;
			for( Iter = houseClass->FurnitureDBIDtoObjGUIDMap().begin() ; Iter != houseClass->FurnitureDBIDtoObjGUIDMap().end() ; Iter++ )
			{
				BaseItemObject* BuildingObj = Global::GetObj( Iter->second );

				if( BuildingObj != NULL  )
				{
					BuildingObj->Destroy( "RC_HouseItemLayoutRequest" );
				}
			}
			houseClass->FurnitureDBIDtoObjGUIDMap().clear();
		}
		houseClass->NeedSave();

	}


	houseClass->RebuildAllBuilding();
	SC_Room_BuyHouseStyleResultOk( Role->RoomID() ,  Type );

	{		
		float X		= (float)g_ObjectData->GetSysKeyValue( "GuildHouse_X" );
		float Y		= (float)g_ObjectData->GetSysKeyValue( "GuildHouse_Y" );
		float Z		= (float)g_ObjectData->GetSysKeyValue( "GuildHouse_Z" );
		float Dir	= (float)g_ObjectData->GetSysKeyValue( "GuildHouse_Dir" );

		vector< BaseItemObject* >&  ZoneObjList = *(BaseItemObject::GetZoneObjList());
		for( unsigned int i = 0 ; i < ZoneObjList.size() ; i++ )
		{
			BaseItemObject* TempObj = ZoneObjList[i];
			if( TempObj == NULL )
				continue;
			if(	TempObj->Role()->RoomID() != Role->RoomID() 	 )
				continue;
			if( TempObj->Role()->IsPlayer() == false )
				continue;

			ChangeZone( TempObj->Role()->GUID() , RoleDataEx::G_ZoneID  , Role->RoomID() , X+ Random( -20 , 20 ) , Y , Z+ Random( -20 , 20 ) , Dir );	
		}

		

	}
}
/*
void NetSrv_GuildHousesChild::RC_SetHouseStyle		( BaseItemObject* Obj , int Type )
{
	
	RoleDataEx* Role = Obj->Role();

	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj_ByRoom( Obj->Role()->RoomID() );
	if( houseClass == NULL )
	{
		SC_SetHouseStyleResult( Role->GUID() , false );
		return;
	}
	if( ( houseClass->GetHouseBase()->GuildStyleRight & ( 1 << Type ) ) == false )
	{
		SC_SetHouseStyleResult( Role->GUID() , false );
		return;
	}
	//-------------------------------------------------------------------------------------------------
	//檢查此人是否能存取
	if( CheckSetStyleAbility( Role->DBID() ) == false )
	{
		SC_SetHouseStyleResult( Role->GUID() , false );
		return;
	}

	houseClass->GetHouseBase()->GuildStyle = Type;
	houseClass->NeedSave();
	houseClass->RebuildAllBuilding();

	SC_SetHouseStyleResult( Role->GUID() , true );
	return;
	
}
*/
void NetSrv_GuildHousesChild::RC_HouseStyleRequest	( BaseItemObject* Obj )
{
	RoleDataEx* Role = Obj->Role();
	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj_ByRoom( Obj->Role()->RoomID() );
	if( houseClass == NULL )
	{	
		return;
	}

	SC_HouseStyle( Role->GUID() , houseClass->GetHouseBase()->GuildStyle );
}