#include "../NetWakerGSrvInc.h"
#include "NetSrv_CliConnectChild.h"
#include "../../mainproc/pathmanage/NPCMoveFlagManage.h"
#include "../../MainProc/FlagPosList/FlagPosList.h"
#include "../../mainproc/HousesManage/HousesManageClass.h"
#include "../../mainproc/GuildHouseManage/GuildHousesClass.h"
#include "../../mainproc/GuildHouseWarManage/GuildHouseWarManage.h"
#include "../../mainproc/LuaPlot/LuaPlot.h"
//-----------------------------------------------------------------
ClientSkyProcTypeENUM NetSrv_CliConnectChild::ClientSkyType = EM_ClientSkyProcType_Normal;
set< int > NetSrv_CliConnectChild::AllZoneSet;
//-----------------------------------------------------------------
bool    NetSrv_CliConnectChild::Init()
{
    NetSrv_CliConnect::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_CliConnectChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_CliConnectChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_CliConnect::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-------------------------------------------------------------------
//  事件繼承實做
//-------------------------------------------------------------------
void NetSrv_CliConnectChild::CliConnect( int CliID , string Account )
{

    Print( Def_PrintLV1 , "CliConnect" , "id=%d account=%s\n", CliID , Account.c_str() );
    //----------------------------------------------------------------
    // 檢查此 CliID 是否再要登入此 GSrv List 內 
    //----------------------------------------------------------------
    GSrv_CliInfoStruct* CliNetInfo = _Net->GetCliNetInfo( CliID );
    
	PlayerRoleData* LoginPlayerBase = Global::GetOnLoginPlayer( Account );
	
	if( LoginPlayerBase == NULL )
    {
        Print( LV2 , "CliConnect" , "Account not find LoginPlayer == NULL,  Account(%s)", Account.c_str() );
		_Net->CliLogout( CliID, Net_ServerList->m_ServerType, Net_ServerList->m_dwServerLocalID, Account.c_str(), "CliConnect Account Not Find!" );
        return;
    }
	else if( CliNetInfo == NULL )
	{
        Print( LV2 , "CliConnect" , "Account not find CliNetInfo == NULL,  Account(%s)", Account.c_str() );
		_Net->CliLogout( CliID, Net_ServerList->m_ServerType, Net_ServerList->m_dwServerLocalID, Account.c_str(), "CliConnect Account Not Find!" );
        return;	
	}

    RoleDataEx* LoginPlayer = (RoleDataEx*)LoginPlayerBase;

	if( Global::Ini()->MaxRoleLevel != 0 )
	{
		LoginPlayer->PlayerBaseData->MaxLv = Global::Ini()->MaxRoleLevel;
	}
	//----------------------------------------------------------------
	//如果有Mirror的同物件，把他移除Partition
	BaseItemObject* MirrorObj = BaseItemObject::GetObjByWorldGUID( LoginPlayer->WorldGUID() );
	if( MirrorObj != NULL && MirrorObj->IsMirror() )
	{
		Global::DelFromPartition( MirrorObj->GUID() );
		delete MirrorObj;
	}

    //----------------------------------------------------------------
	BaseItemObject*		Obj;
	RoleDataEx*			Owner;

	BaseItemObject*		OldObj = BaseItemObject::GetObjByDBID( LoginPlayer->DBID() );

	if( OldObj )
	{
		if(		Global::Ini()->IsBattleWorld  
			||	OldObj->Role()->TempData.Sys.OnChangeWorld  )
		{
			delete OldObj;
			OldObj = NULL;
		}
	}

	if( OldObj != NULL )
	{

		Obj = OldObj;
		Global::DelFromPartition( Obj->Role()->GUID() );
		Obj->ReleaseSock();
		Obj->ReleaseAccountName();
		Obj->ReleaseDBID();
		Owner	= Obj->Role();
		Owner->TempData.Attr.Action.Login = LoginPlayer->TempData.Attr.Action.Login;
		
		
		Obj->ReleaseWorldGUID( );
		if( LoginPlayer->WorldGUID() == -1 )
		{
			Obj->CreateWorldGUID();
			Owner->WorldGUID( Obj->WorldGUID() );
		}
		else
		{
			Obj->SetWorldGUID( LoginPlayer->WorldGUID() );
			Owner->WorldGUID( LoginPlayer->WorldGUID() );
		}
		
	}
	else
	{
		if( Global::Ini()->IsBattleWorld )
		{
			Obj		= NEW BaseItemObject( EM_ItemType_Player , -1 );		
			Obj->SetOrgWorldGUID( LoginPlayer->WorldGUID() );
		}
		else
		{
			Obj		= NEW BaseItemObject( EM_ItemType_Player , LoginPlayer->WorldGUID() );		
		}

		Owner	= Obj->Role();
		int     GUID = Owner->GUID();
		int     WorldGUID = Owner->WorldGUID();
		int     LoginTempID = Owner->PlayerTempData->LoginTempID;

		//memcpy( Owner , LoginPlayer , sizeof(RoleDataEx) );
		Owner->Copy( LoginPlayer );
		Owner->GUID( GUID );
		Owner->WorldGUID( WorldGUID );
		Owner->PlayerTempData->LoginTempID = LoginTempID;
		Owner->TempData.Attr.AllZoneState.FindParty = Owner->BaseData.SysFlag.FindParty;
	}

//	if( Global::Ini()->IsBattleWorld == false )
//		Owner->SetZoneKey();
	///////////////////////////////////////////////////////////
	//pre-init attr.ability info 
	Owner->InitCal();
	///////////////////////////////////////////////////////////
	BGIndependenceGameProc( Owner );
	///////////////////////////////////////////////////////////

	bool IsLogin = Owner->TempData.Attr.Action.Login;
    
    Global::DelOnLoginPlayer( Account );

	if( IsLogin )
	{
		Global::LoginInitProc( Owner );
	}


	if( stricmp( Account.c_str() , Owner->Base.Account_ID.Begin() ) != 0 )
	{
		Print( Def_PrintLV3 , "CliConnect" ,  "(Error)stricmp( Account.c_str() , Owner->Base.Account_ID.Begin() ) != 0" );
		Obj->Destroy( "SYS CliConnect stricmp( Account.c_str() , Owner->Base.Account_ID.Begin() ) != 0" );
		_Net->CliLogout( CliID , Net_ServerList->m_ServerType, Net_ServerList->m_dwServerLocalID, Account.c_str(), "SYS CliConnect stricmp( Account.c_str() , Owner->Base.Account_ID.Begin() ) != 0" );
		return;
	}


	if( Obj->SetDBID( Owner->DBID() ) == false )
	{
		Print( Def_PrintLV3 , "CliConnect" ,  "(Error)CliConnect SetDBID Error!!" );
		Obj->Destroy( "SYS CliConnect (Error)CliConnect SetDBID Error!!" );
		_Net->CliLogout( CliID , Net_ServerList->m_ServerType, Net_ServerList->m_dwServerLocalID, Account.c_str(), "CliConnect SetDBID Error!! " );
		return;

	}

    if( Obj->SetSock( CliID ) == false )
    {
        Print( Def_PrintLV3 , "CliConnect" ,  "(Error)CliConnect SetSock Error!!" );
        Obj->Destroy( "SYS CliConnect (Error)CliConnect SetSock Error!!" );
        _Net->CliLogout( CliID, Net_ServerList->m_ServerType, Net_ServerList->m_dwServerLocalID, Account.c_str(),"CliConnect SetSock Error!!" );
        return;
    }


    if( Obj->SetAccountName( Owner->Account_ID() ) == false )
    {
        Print( Def_PrintLV3 , "CliConnect" , "(Error)CliConnect SetAccountName Error!!" );
        Obj->Destroy( "SYS CliConnect (Error)CliConnect SetAccountName Error!!" );
        _Net->CliLogout( CliID, Net_ServerList->m_ServerType, Net_ServerList->m_dwServerLocalID, Account.c_str(), "CliConnect SetAccountName Error!!" );
        return;       
    }

    Owner->ProxyID( CliNetInfo->ProxyID );
    //----------------------------------------------------------------
    //暫時
    Owner->TempData.BoundRadiusX  = 5;
	Owner->TempData.BoundRadiusY  = 5;
	Owner->TempData.Sys.OwnerGUID = -1;
    Owner->TempData.ShopInfo.TargetID = -1;
	Owner->TempData.Attr.Ability = &Owner->PlayerBaseData->AbilityList[ 1 ];

	if( RoleDataEx::G_IsPvE != false )
	{
		Owner->TempData.AI.CampID = EM_CampID_Good;
	}
	else
	{
		if( Owner->PlayerBaseData->GoodEvil >=0 )
		{
			Owner->TempData.AI.CampID = EM_CampID_NewHand;

			for( int i = 0 ; i < EM_Max_Vocation ; i++ )
			{
				if( Owner->PlayerBaseData->AbilityList[i].Level > 15 )
				{
					Owner->TempData.AI.CampID = EM_CampID_Good;
					break;
				}

			}
		}
		else
			Owner->TempData.AI.CampID = EM_CampID_Evil;
	}


	Owner->TempData.Sys.KillRate = 1;
	Owner->PlayerTempData->DialogSelectID = -1;
	Owner->PlayerTempData->CastingType = EM_CastingType_NULL;
	Owner->TempData.SkillComboID = -1;
	Owner->TempData.IsDisabledChangeZone = false;
	Owner->SelfData.LiveTime = -1;
	Owner->TempData.Sys.OnChangeZone = false;
	Owner->TempData.Sys.OnChangeWorld = false;
	Owner->TempData.Sys.WaitLogout = false;
	Owner->TempData.Sys.WaitChangeParallel = false;
	
	//Owner->BaseData.Coldown.Init();
	Owner->TempData.SummonPet.Init();
	Owner->TempData.Attr.Bas.Effect._Value[0] = -1;
	Owner->TempData.Attr.Bas.Effect._Value[1] = -1;

	Owner->TempData.Move.CliX = Owner->BaseData.Pos.X;
	Owner->TempData.Move.CliY = Owner->BaseData.Pos.Y;
	Owner->TempData.Move.CliZ = Owner->BaseData.Pos.Z;
	Owner->PlayerTempData->PKInfo.Init();

	if( Global::Ini()->IsEnabledGMCommand != false )
		Owner->PlayerBaseData->ManageLV = EM_Management_GameCreator;

    //----------------------------------------------------------------
    //設定角色資料與初始化
    //----------------------------------------------------------------
    Owner->InitCheckRoleData();
	Owner->ChangeZoneInit();
	Owner->CheckAndClearCopyItem( );
    Owner->ResetItemInfo();
    //初始化備份檢查
    Owner->InitOnTimeCheck();
	Owner->InitCal();
	Owner->GetTitle( );	//以目前的等級資料取得頭銜
	Owner->TempData.Attr.Action.SendAllZoneObj = true;
	//////////////////////////////////////////////////////////////////////////
	if( Global::St()->WaitServerClose != false )
	{
	//	Global::ChangeZone( Owner->GUID() , Owner->ZoneID() % _DEF_ZONE_BASE_COUNT_ 
	//		, 0 , Owner->Pos()->X , Owner->Pos()->Y , Owner->Pos()->Z , Owner->Pos()->Dir );
		Global::ChangeParalledID( Owner->GUID() , 0 );
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//清除騎馬的Buff
	if( Global::Ini()->IsEnabledRide == false )
	{
		for( int i =  Owner->BaseData.Buff.Size() -1  ; i >= 0 ; i-- )
		{
			BuffBaseStruct&		Buff	= Owner->BaseData.Buff[i];
			GameObjDbStructEx*  MagBuf	= Global::GetObjDB( Buff.BuffID );
			if( MagBuf->MagicBase.Effect.Ride != false )
			{
				Owner->ClearBuffByPos( i );
				//Owner->BaseData.Buff.Erase( i );
				Owner->TempData.UpdateInfo.Recalculate_Buff = true;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

    //----------------------------------------------------------------
	//通知Master人物進入
	CNetSrv_ZoneStatus::SetRoleEnterZone( Owner->DBID() , RoleDataEx::G_ZoneID );
	Global::Log_EnterZone( Owner );
    //----------------------------------------------------------------

	if( CNetSrv_BattleGround_Child::GetBattleGroundType() != 0 )
	{
		// 戰場登入設地
		//----------------------------------------------------------------
		if( ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->OnEvent_ClientConnect( Obj ) != false )
		{				
			return;	// 人物沒有權力在戰場, 已被傳送到 REV ZONE
		}
	}

	if( IsLogin )
	{
		Owner->ProcOffLineTime();
		SendSysVersionInfo( Owner->SockID() , RoleDataEx::GlobalSetting.LearnMagicID , RoleDataEx::GlobalSetting.Version , RoleDataEx::GlobalSetting.PK_DotRate );

		// 設定人物所屬世界
		Owner->TempData.iWorldID = _Net->GetWorldID();

		if( Ini()->DisabledSavePosition != false )
		{
			if( Global::Ini()->IsReturnSavePoint != false )
			{
				int ZoneID = GlobalBase::GetParallelZoneID( Owner->SelfData.ReturnZoneID ,  Owner->PlayerTempData->ParallelZoneID );
				if( ZoneID != -1 )
				{
					Global::SafeChangeZone( Owner->GUID() , ZoneID , 0 , Owner->SelfData.ReturnPos.X , Owner->SelfData.ReturnPos.Y , Owner->SelfData.ReturnPos.Z , Owner->SelfData.ReturnPos.Dir );
					Owner->SelfData.ReturnZoneID = 0;
					Owner->SelfData.ReturnPos.X = 0;
					Owner->SelfData.ReturnPos.Y = 0;
					Owner->SelfData.ReturnPos.Z = 0;
					Owner->SelfData.ReturnPos.Dir = 0;
				}
				else
				{
					Global::SafeChangeZone( Owner->GUID() , Owner->SelfData.RevZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir );	
				}
				return;
			}
			else
			{
				int ZoneID = GlobalBase::GetParallelZoneID( Ini()->RevZoneID ,  Owner->PlayerTempData->ParallelZoneID );
				if( ZoneID == -1 )
				{
					Global::SafeChangeZone( Owner->GUID() , Owner->SelfData.RevZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir );
					return;
				}
				Global::SafeChangeZone( Owner->GUID() , ZoneID , 0 , (float)Ini()->RevX , (float)Ini()->RevY , (float)Ini()->RevZ , 0 );

			}
			return;
		}
		else if( Ini()->IsHouseZone != false )
		{
			Owner->PlayerTempData->VisitHouseDBID = Owner->PlayerBaseData->HouseDBID;
			if( Owner->PlayerBaseData->HouseDBID <= 0 )
			{
				int ZoneID = GlobalBase::GetParallelZoneID( Owner->SelfData.ReturnZoneID ,  Owner->PlayerTempData->ParallelZoneID );
				if( ZoneID == -1 || ZoneID == RoleDataEx::G_ZoneID )
				{
					Global::SafeChangeZone( Owner->GUID() , Owner->SelfData.RevZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir );
					return;				
				}
				Global::SafeChangeZone( Owner->GUID() , ZoneID , 0 , Owner->SelfData.ReturnPos.X , Owner->SelfData.ReturnPos.Y , Owner->SelfData.ReturnPos.Z , Owner->SelfData.ReturnPos.Dir );
				return;
			}			
		}
		else if( Ini()->IsGuildHouseZone != false  )
		{
			int ZoneID = GlobalBase::GetParallelZoneID( Owner->SelfData.ReturnZoneID ,  Owner->PlayerTempData->ParallelZoneID );
			if( ZoneID == -1 || ZoneID == RoleDataEx::G_ZoneID )
			{
				Global::SafeChangeZone( Owner->GUID() , Owner->SelfData.RevZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir );
				return;				
			}
			Global::SafeChangeZone( Owner->GUID() , ZoneID , 0 , Owner->SelfData.ReturnPos.X , Owner->SelfData.ReturnPos.Y , Owner->SelfData.ReturnPos.Z , Owner->SelfData.ReturnPos.Dir );
			return;
		}
		else if( Ini()->IsGuildHouseWarZone != false )
		{
			int ZoneID = GlobalBase::GetParallelZoneID( Owner->SelfData.ReturnZoneID ,  Owner->PlayerTempData->ParallelZoneID );
			if( ZoneID == -1 || ZoneID == RoleDataEx::G_ZoneID )
			{
				Global::SafeChangeZone( Owner->GUID() , Owner->SelfData.RevZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir );
				return;				
			}
			Global::SafeChangeZone( Owner->GUID() , ZoneID , 0 , Owner->SelfData.ReturnPos.X , Owner->SelfData.ReturnPos.Y , Owner->SelfData.ReturnPos.Z , Owner->SelfData.ReturnPos.Dir );
			return;
		}
		//如果是不可存位置的獨立Zone,如果所屬空間沒了則傳走
		else if( Ini()->IsPrivateZone != false && Ini()->PrivateZoneType != EM_PrivateZoneType_None )
		{

			bool IsFindRoom = false;
			switch( Ini()->PrivateZoneType )
			{
			case EM_PrivateZoneType_None:
				break;
			case EM_PrivateZoneType_Private:
				{
					//bool	IsFind = false;
					vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;
					for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
					{
						if( RoomList[i].OwnerDBID == Owner->DBID() )
						{
							IsFindRoom = true;
							Owner->RoomID( i );
							break;	
						}
					}

				/*	if( IsFind == false )
					{	
						int ZoneID = GlobalBase::GetParallelZoneID( Ini()->RevZoneID ,  Owner->PlayerTempData->ParallelZoneID );
						if( ZoneID != -1)
							Global::ChangeZone( Owner->GUID() , ZoneID , 0 , (float)Ini()->RevX , (float)Ini()->RevY , (float)Ini()->RevZ , 0 );
						else
							Global::ChangeZone( Owner->GUID() , Owner->SelfData.RevZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir );
						return;
					}*/
				}	
				break;
			case EM_PrivateZoneType_Party:
				{
					//bool	IsFind = false;
					vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;
					for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
					{
						if( RoomList[i].OwnerPartyID == Owner->PartyID() )
						{
							IsFindRoom = true;
							Owner->RoomID( i );
							break;	
						}
					}

				/*	if( IsFind == false )
					{	
						//換區處理
						//int ZoneID = Ini()->RevZoneID  % _DEF_ZONE_BASE_COUNT_ + _DEF_ZONE_BASE_COUNT_ * Owner->PlayerTempData->ParallelZoneID;
						int ZoneID = GlobalBase::GetParallelZoneID( Ini()->RevZoneID ,  Owner->PlayerTempData->ParallelZoneID );
						if( ZoneID == -1 )
						{
							Global::ChangeZone( Owner->GUID() , Owner->SelfData.RevZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir );
							return;
						}
						Global::ChangeZone( Owner->GUID() , ZoneID , 0 , (float)Ini()->RevX , (float)Ini()->RevY , (float)Ini()->RevZ , 0 );
						return;
					}*/
				}	
				break;
			case EM_PrivateZoneType_Raid:
				break;
			case EM_PrivateZoneType_Private_Party:
				{
					//bool	IsFind = false;
					vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;
					for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
					{			
						if( RoomList[i].OwnerPartyID != -1 )
						{
							if( Owner->PartyID() == RoomList[i].OwnerPartyID )
							{
								IsFindRoom = true;
								Owner->RoomID( i );
								break;
							}
						}
					}

					if( IsFindRoom == false && Owner->PartyID() == -1 )
					{
						for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
						{			
							if( RoomList[i].OwnerDBID == Owner->DBID() )
							{
								IsFindRoom = true;
								Owner->RoomID( i );
								break;
							}
						}
					}

				}	
				break;
			}

			//人數計算
			if( Global::RoomPlayerCount( Owner->RoomID() ) >= Global::Ini()->RoomPlayerCount )
			{
				IsFindRoom = false;
			}


			if( IsFindRoom != false )
			{
				vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;
				if( RoomList[ Owner->RoomID() ].RoomKey != Owner->PlayerBaseData->RoomKey )
					IsFindRoom = false;
			}


			if( IsFindRoom == false )
			{	
				if( Global::Ini()->IsReturnSavePoint != false )
				{
					int ZoneID = GlobalBase::GetParallelZoneID( Owner->SelfData.ReturnZoneID ,  Owner->PlayerTempData->ParallelZoneID );
					if( ZoneID != -1 )
					{
						Global::SafeChangeZone( Owner->GUID() , ZoneID , 0 , Owner->SelfData.ReturnPos.X , Owner->SelfData.ReturnPos.Y , Owner->SelfData.ReturnPos.Z , Owner->SelfData.ReturnPos.Dir );
						Owner->SelfData.ReturnZoneID = 0;
						Owner->SelfData.ReturnPos.X = 0;
						Owner->SelfData.ReturnPos.Y = 0;
						Owner->SelfData.ReturnPos.Z = 0;
						Owner->SelfData.ReturnPos.Dir = 0;
					}
					else
					{
						Global::SafeChangeZone( Owner->GUID() , Owner->SelfData.RevZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir );
					}
					return;
				}
				else
				{
					int ZoneID = GlobalBase::GetParallelZoneID( Ini()->RevZoneID ,  Owner->PlayerTempData->ParallelZoneID );
					if( ZoneID == -1 )
					{
						Global::SafeChangeZone( Owner->GUID() , Owner->SelfData.RevZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir );
						return;
					}
					Global::SafeChangeZone( Owner->GUID() , ZoneID , 0 , (float)Ini()->RevX , (float)Ini()->RevY , (float)Ini()->RevZ , 0 );

				}
				return;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
    //送角色區域ID資料
    SendPlayerItemID( Owner );
	//////////////////////////////////////////////////////////////////////////
	if( Ini()->IsHouseZone )
	{
		//把角色移到房屋進入點
		Owner->Pos()->X = 0;
		Owner->Pos()->Y = 1;
		Owner->Pos()->Z = 0;

		//檢查是否有此Zone存在
		HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Owner->PlayerTempData->VisitHouseDBID );
		if( houseClass != NULL )
		{
			if( houseClass->State() == EM_HouseObjectState_OK )
			{
				vector<HouseItemDBStruct> ItemList;
				vector<HouseItemStruct*>& HouseItemList = houseClass->HouseBase().ItemList[-1];
				for( unsigned i = 0 ; i < HouseItemList.size() ; i++)
				{
					ItemList.push_back( HouseItemList[i]->Info );
				}
				Owner->RoomID( houseClass->RoomID() );

				NetSrv_Houses::SC_HouseInfoLoading			( Owner->GUID() );
				NetSrv_Houses::SC_HouseBaseInfo				( Owner->GUID() , houseClass->HouseBase().Info );
				NetSrv_Houses::SC_ItemInfo					( Owner->GUID() , -1 , ItemList );
				//送衣架資料
				for( unsigned i = 0 ; i < ItemList.size() ; i++ )
				{
					HouseItemStruct* houseItem = houseClass->GetItem( ItemList[i].ItemDBID );//&ItemList[i];	
					if( houseItem == NULL )
						continue;

					GameObjDbStructEx* ParentItemDB = Global::GetObjDB( houseItem->Info.Item.OrgObjID );
					if( ParentItemDB == NULL || ParentItemDB->IsItem() == false )
						continue;

					if( ParentItemDB->Item.ItemType != EM_ItemType_Furniture )
						continue;

					vector<HouseItemDBStruct> ItemList;
					for( unsigned j = 0 ; j < houseItem->ChildList.size() ; j++)
						ItemList.push_back( houseItem->ChildList[j]->Info );

					NetSrv_Houses::SC_ItemInfo( Owner->GUID() , houseItem->Info.ItemDBID , ItemList );
				}


				vector<HouseItemStruct*>& HouseItemList2 = houseClass->HouseBase().ItemList[-2];
				ItemList.clear();
				for( unsigned i = 0 ; i < HouseItemList2.size() ; i++)
				{
					ItemList.push_back( HouseItemList2[i]->Info );
				}
				NetSrv_Houses::SC_ItemInfo( Owner->GUID() , -2 , ItemList );
				
				//送女僕資料
				for( int i = 0 ; i < 6 ; i++ )
				{
					HouseItemStruct* houseItem = houseClass->GetItem( -2 , i );//&ItemList[i];	
					if( houseItem == NULL )
						continue;

					GameObjDbStructEx* ParentItemDB = Global::GetObjDB( houseItem->Info.Item.OrgObjID );
					if( ParentItemDB == NULL || ParentItemDB->IsItem() == false )
						continue;

					ItemList.push_back( houseItem->Info );
					vector<HouseItemDBStruct> ItemList;
					for( unsigned j = 0 ; j < houseItem->ChildList.size() ; j++)
						ItemList.push_back( houseItem->ChildList[j]->Info );

					NetSrv_Houses::SC_ItemInfo( Owner->GUID() , houseItem->Info.ItemDBID , ItemList );
				}

				NetSrv_HousesChild::SendCli_FriendDBIDList	( Owner->GUID() , houseClass );
				NetSrv_Houses::SC_HouseInfoLoadOK			( Owner->GUID() );

				if( Owner->DBID() == houseClass->GetHouseBase()->Info.OwnerDBID )
					houseClass->GetHouseBase()->Info.UnCheckItemLogCount = 0;
				
			}
			else
			{
				//通知Client 等待載入資料
				NetSrv_Houses::SC_HouseInfoLoading( Owner->GUID() );
				Print( LV3 , "CliConnect" , " Waiting House Info account_id='%s'" , Account.c_str() );
			}

		}
		else
		{
			bool IsFind = false;
			vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;
			for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
			{
				if( RoomList[i].IsActive == false )
				{
					RoomList[i].IsActive = true;
					Owner->RoomID( i );
					IsFind = true;
					break;
				}
			}
			if( IsFind == false )
			{
				Owner->Msg("房屋處理忙碌中");
				int ZoneID = GlobalBase::GetParallelZoneID( Owner->SelfData.ReturnZoneID ,  Owner->PlayerTempData->ParallelZoneID );
				if( ZoneID == -1 || ZoneID == RoleDataEx::G_ZoneID )
				{
					Global::SafeChangeZone( Owner->GUID() , Owner->SelfData.RevZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir );
					return;
				}
				Global::SafeChangeZone( Owner->GUID() , ZoneID , 0 , Owner->SelfData.ReturnPos.X , Owner->SelfData.ReturnPos.Y , Owner->SelfData.ReturnPos.Z , Owner->SelfData.ReturnPos.Dir );
				return;
			}

			houseClass = HousesManageClass::CreateHouse( Owner->PlayerTempData->VisitHouseDBID , Owner->RoomID() );

			//通知DB載入資料
			NetSrv_Houses::SD_HouseInfoLoading(  Owner->PlayerTempData->VisitHouseDBID );
			//通知Client 等待載入資料
			NetSrv_Houses::SC_HouseInfoLoading( Owner->GUID() );
		}
		 
		if( Owner->PlayerTempData->VisitHouseDBID == Owner->PlayerBaseData->HouseDBID )
		{
			houseClass->SetOwnerID( Owner->GUID() );
			if( IsLogin )
				houseClass->CalOffLineTime( Owner->Base.LogoutTime );
		}
	}
	else if( Ini()->IsGuildHouseZone  )
	{
		//檢查是否有此Zone存在
		GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Owner->PlayerTempData->VisitGuildID );
		/*if( Owner->BaseData.Voc == EM_Vocation_GameMaster )
		{
			
		}
		else */if( houseClass != NULL )
		{
			if( houseClass->State() == EM_GuildHouseObjectState_OK )
			{
				Owner->RoomID( houseClass->RoomID() );
				NetSrv_GuildHousesChild::SC_HouseInfoLoading	( Owner->GUID() , Owner->PlayerTempData->VisitGuildID );
				NetSrv_GuildHousesChild::SC_StoreConfigProc( Obj );
				NetSrv_GuildHousesChild::SC_AllFurnitureItemInfo( Owner->GUID() );
				NetSrv_GuildHousesChild::SC_AllBuildingActiveType( Owner->GUID() );
				NetSrv_GuildHousesChild::SC_HouseInfoLoadOK	( Owner->GUID() );
			}
			else
			{
				//通知Client 等待載入資料
				NetSrv_GuildHouses::SC_HouseInfoLoading( Owner->GUID() , Owner->PlayerTempData->VisitGuildID );
			}
		}
		else
		{
			bool IsFind = false;
			vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;
			for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
			{
				if( RoomList[i].IsActive == false )
				{
					RoomList[i].IsActive = true;
					Owner->RoomID( i );
					IsFind = true;
					break;
				}
			}
			if( IsFind == false )
			{
				Owner->Msg("房屋處理忙碌中");
				int ZoneID = GlobalBase::GetParallelZoneID( Owner->SelfData.ReturnZoneID ,  Owner->PlayerTempData->ParallelZoneID );
				if( ZoneID == -1 )
				{
					Global::SafeChangeZone( Owner->GUID() , Owner->SelfData.RevZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , 0 );
					return;
				}
				Global::SafeChangeZone( Owner->GUID() , ZoneID , 0 , Owner->SelfData.ReturnPos.X , Owner->SelfData.ReturnPos.Y , Owner->SelfData.ReturnPos.Z , Owner->SelfData.ReturnPos.Dir );
				return;
			}

			houseClass = GuildHousesManageClass::CreateHouse( Owner->PlayerTempData->VisitGuildID , Owner->RoomID() );
			//通知DB載入資料
			NetSrv_GuildHouses::SD_HouseInfoLoading(  Owner->PlayerTempData->VisitHouseDBID );
			//通知Client 等待載入資料
			NetSrv_GuildHouses::SC_HouseInfoLoading( Owner->GUID() , Owner->PlayerTempData->VisitHouseDBID );
		}
	}
	else if( Global::Ini()->IsGuildHouseWarZone != false  )
	{		
		//取得公會
		GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Owner->GuildID() );
		if( Owner->TempData.Attr.Voc == EM_Vocation_GameMaster && houseClass == NULL )
		{

		}
		else if( houseClass == NULL || houseClass->CheckLoadOK() == false )
		{
			int ZoneID = GlobalBase::GetParallelZoneID( Owner->SelfData.ReturnZoneID ,  Owner->PlayerTempData->ParallelZoneID );
			if( ZoneID == -1 || ZoneID == RoleDataEx::G_ZoneID )
			{
				Global::SafeChangeZone( Owner->GUID() , Owner->SelfData.RevZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , 0 );
				return;
			}
			Global::SafeChangeZone( Owner->GUID() , ZoneID , 0 , Owner->SelfData.ReturnPos.X , Owner->SelfData.ReturnPos.Y , Owner->SelfData.ReturnPos.Z , Owner->SelfData.ReturnPos.Dir );
			return;
		}
		else
		{
			Owner->TempData.AI.CampID = houseClass->GetCampID();
			Owner->RoomID( houseClass->RoomID() );
			NetSrv_BG_GuildWarChild::SC_GuildScore_ByRoomID( Owner->GUID() , houseClass->RoomID() );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//房間檢查
	//////////////////////////////////////////////////////////////////////////
	if( (unsigned)Owner->RoomID() >= Global::St()->PrivateRoomInfoList.size() )
	{
		Owner->RoomID( 0 );
	}


	//----------------------------------------------------------------
	// 初始化 Quest 所需的 QuestTempData
	CNetSrv_Script::OnEvent_RoleLogin( Owner );



	//第一次登入才送
	if( IsLogin )
	{

		//下限有計算的Buf處理
//		Owner->ProcOffLineTime();

		//更新某人的角色資料
		//SendPlayerDataInfo( Owner );
		SendPlayerDataInfo_ZIP( Owner );

		NetSrv_GuildChild::SendAllGuildBaseInfo( Owner->GUID() );

		S_ParalledID( Owner->GUID() , Owner->PlayerTempData->ParallelZoneID );

		NetSrv_Shop::S_FixSellItemBackup( Owner );

		NetSrv_Other::SC_AllItemCombinInfo( Owner->GUID() );
	}
/*
	if( CNetSrv_BattleGround_Child::GetBattleGroundType() != 0 )
	{
		//int iFlagiD = CNetSrv_BattleGround_Child::GetFlagID( Owner->TempData.AI.CampID );
		SendZoneInfo( Owner, true );
	}
	else
	{
		SendZoneInfo( Owner, false );
	}
*/
	SendZoneInfo( Owner );

    //通知角色換區OK
    SendChangeZoneOK( Owner );    

    //----------------------------------------------------------------
    Owner->SaveDB_AddTime();
    //----------------------------------------------------------------
    //送物品資料
    NetSrv_ItemChild::FixAllBufferInfo( Obj );

	Obj->Path()->SetPos( Owner->Pos()->X , Owner->Pos()->Y , Owner->Pos()->Z );
	//----------------------------------------------------------------
	//Chat 註冊
	{
		OnlinePlayerInfoStruct Info;
		Owner->GetOnlinePlayerInfo( Info );
		NetSrv_ChannelBase::ChangeRoleInfo  (  Owner->Account_ID() , Info );
	}
	//----------------------------------------------------------------
	//(如果第一次登入)
	if( IsLogin )
	{
		//清除下線消失的Buff
		//Owner->TempData.BackInfo.BuffClearTime.Logout = true;

		//初始資料
		Owner->TempData.BackInfo.LZoneID = -1;

		//送好友資訊
		Owner->TempData.Attr.Action.Login = false;

		NetSrv_FriendList::SC_FriendListInfo_Zip( Owner->GUID() , Owner->PlayerSelfData->FriendList );

		for( int i = 0 ; i < _MAX_FRIENDLIST_ALL_COUNT_ ; i++ )
		{
			BaseFriendStruct& TempF = Owner->PlayerSelfData->FriendList.AllFriendList[i];
			if( TempF.IsEmpty() )
				continue;
			NetSrv_FriendList::SChat_FriendListInfo( Owner->DBID() , TempF.DBID , true );
		}	

		//----------------------------------------------------------------
		//重新加入Party
		if( Owner->BaseData.PartyID != -1 )
		{
			NetSrv_Party::S_Relogin( Owner->PartyID() , Owner->DBID() );
		}

		//要求有多少信件尚未讀取
		NetSrv_Mail::SD_UnReadMailCountRequest( Owner->DBID(), Owner->TempData.Sys.SockID, Owner->TempData.Sys.ProxyID );

		//檢查是否有初始劇情
		if( Owner->SelfData.AutoPlot.Size() != 0 )
		{
			LUA_VMClass::PCall( Owner->SelfData.AutoPlot.Begin() , Owner->GUID() , Owner->GUID() );
		}
		//////////////////////////////////////////////////////////////////////////
		//公會敵對狀況資料
		SendLoginWarDeclare( Owner );		
		//////////////////////////////////////////////////////////////////////////
		NetSrv_OtherChild::SendClient_DBTable( Owner->GUID() );

		NetSrv_Talk::SD_OfflineMessageRequest( Owner->DBID() );
		NetSrv_ImportBoard::SD_NewVersionRequest( Owner->TempData.Sys.SockID, Owner->TempData.Sys.ProxyID  );

		//取網頁商城資料
		//NetSrv_DepartmentStore::SD_WebBagRequest( Owner );
	}
	
	//----------------------------------------------------------------
	//清除換區消失的Buff
	//Owner->TempData.BackInfo.BuffClearTime.ChangeZone = true;

    Global::CheckClientLoading_AddToPartition( Owner->GUID() , -1 );




    PlayerRegProc   ( Obj );

	//設定DB的生存奇
	Net_DCBase::SetPlayerLiveTime( GetPCenterID(Owner->Account_ID()) , Owner->Base.DBID , 60*60*1000 );

	NetSrv_DepartmentStore::SD_ImportAccountMoney( Owner->Base.DBID , Owner->Account_ID() );
	//----------------------------------------------------------------
	NetSrv_Attack::S_ZonePKFlag( Owner->GUID() , RoleDataEx::G_PKType );

	Owner->SaveDB_Test();
	Owner->ClearBuffByEvent();
	NetSrv_Other::SC_ItemCombinGroupID( Owner->GUID() ,  RoleDataEx::ItemCombineListExGroupID);
	//////////////////////////////////////////////////////////////////////////
	//強PK Buff 處理
	if( Global::Ini()->DisableGoodEvil == false )
	{
		GoodEvilTypeENUM GoodEvilType =	Owner->GetGoodEvilType( );
		Owner->AssistMagic( Owner , g_ObjectData->SysValue().GoodEvilBuf[GoodEvilType-EM_GoodEvil_Demon ] , 0 , false , -1 );
	}
	if( Global::Ini()->IsHouseZone != false )
	{
		Owner->AssistMagic( Owner , g_ObjectData->SysValue().HouseBuff , 0 , false , -1 );
	}

	//////////////////////////////////////////////////////////////////////////

	if( ClientSkyType != EM_ClientSkyProcType_Normal )
		SC_SkyProcType( Owner->GUID() , ClientSkyType );
	//////////////////////////////////////////////////////////////////////////
	//墓碑處理
	//////////////////////////////////////////////////////////////////////////
	Global::TombProcess( Owner );
	if( IsLogin  )
		NetSrv_CliConnect::SC_TombInfo( Owner->GUID() , Owner->PlayerSelfData->Tomb );
	//////////////////////////////////////////////////////////////////////////
	//墓碑處理
	//////////////////////////////////////////////////////////////////////////
	if( IsLogin && Owner->BaseData.Mode.GMHide != false )
	{
		Owner->AssistMagic( Owner , g_ObjectData->GetSysKeyValue( "GMHideBuff" ) , 0 , false , -1 );
	}
	//////////////////////////////////////////////////////////////////////////
	//劇情時鐘處理
	if( Owner->PlayerTempData->LuaClock.Type != EM_LuaPlotClockType_Close )
	{
		Obj->PlotVM()->PCall( Owner->PlayerTempData->LuaClock.ChangeZonePlot.Begin() , Owner->GUID() , Owner->GUID() );
	}

	if( Global::Ini()->EnterZoneScript.size() != 0 )
	{
		Obj->PlotVM()->CallLuaFunc( Global::Ini()->EnterZoneScript.c_str() , Owner->GUID() );
	}

	if( Global::Ini()->EnterZoneScript_Pcall.size() != 0 )
	{
		LUA_VMClass::PCall(Global::Ini()->EnterZoneScript_Pcall.c_str() , Owner->GUID() , Owner->GUID() );
	}


	SendAllPlayerInfoPlayerEnterProc( Obj );

	//送地圖標誌的旗標資訊
	NetSrv_OtherChild::OnEnterSendMapMark( Owner->GUID() );
	NetSrv_OtherChild::OnEnterSendRoomValue( Owner->GUID() );
	
	//通知所在分流
	{
		char Buf[512];
		sprintf( Buf , "分流%d" , Owner->PlayerTempData->ParallelZoneID + 1 );
		Owner->Msg( Buf );
	}

	if( IsLogin )
	{
		LUA_VMClass::PCallByStrArg( "event_module(3)" , Owner->GUID() ,Owner->GUID() );
		//通知哪些zone有開
		set<int>::iterator Iter;
		for( Iter = AllZoneSet.begin() ; Iter != AllZoneSet.end() ;Iter++ )
		{
			SC_ZoneOpenState( Owner->GUID() , *Iter , true );
		}
	}
	else
		LUA_VMClass::PCallByStrArg( "event_module(2)" , Owner->GUID() ,Owner->GUID() );


	if( Global::Ini()->InstanceSaveID != -1 )
	{
		PrivateRoomInfoStruct& RoomInfo = Global::St()->PrivateRoomInfoList[ Owner->RoomID() ];

		{	//如果此party已經有key
			map< int , int >::iterator PartyIter = NetSrv_InstanceChild::_PartyKeyMap.find( Owner->PartyID() );
			if( PartyIter != NetSrv_InstanceChild::_PartyKeyMap.end() )
			{
				RoomInfo.InstanceKeyID = PartyIter->second;
			}
		}

		int&	  KeyID		= Owner->PlayerBaseData->InstanceSetting.KeyID[ Global::Ini()->InstanceSaveID ];
		unsigned& LiveTime	= Owner->PlayerBaseData->InstanceSetting.LiveTime[ Global::Ini()->InstanceSaveID ];
		unsigned& ExtendTime	= Owner->PlayerBaseData->InstanceSetting.ExtendTime[ Global::Ini()->InstanceSaveID ];


		if( LiveTime != NetSrv_InstanceChild::_InstanceLiveTime )
		{
			if( ExtendTime > NetSrv_InstanceChild::_InstanceLiveTime )
			{
				LiveTime = ExtendTime;
			}
			else
				KeyID = -1;
		}

		if( RoomInfo.InstanceKeyID == -2 )
		{
			RoomInfo.InstanceKeyID = KeyID;
			NetSrv_Instance::SC_AllMember_InstanceCreate( Owner->PartyID() , KeyID , Owner->DBID() );
		}
		else if( KeyID == RoomInfo.InstanceKeyID )
		{

		}
		else if( KeyID == -1  )
		{
			//通知Client keyID會被修改
			NetSrv_Instance::SC_JoinInstanceRequest( Owner->GUID() );
		}
		else
		{
			//把角色傳出去
			int ZoneID = GlobalBase::GetParallelZoneID( Ini()->RevZoneID ,  Owner->PlayerTempData->ParallelZoneID );
			if( ZoneID == -1 )
			{
				Global::SafeChangeZone( Owner->GUID() , Owner->SelfData.RevZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir );
				return;
			}
			Global::SafeChangeZone( Owner->GUID() , ZoneID , 0 , (float)Ini()->RevX , (float)Ini()->RevY , (float)Ini()->RevZ , 0 );
			return;
		}
		NetSrv_Instance::SC_ResetTime( Owner->GUID() , NetSrv_InstanceChild::_InstanceLiveTime , Global::Ini()->InstanceSaveID , KeyID , true );
	}

	Owner->TempData.Attr.Action.ChangeZone = false;
	Global::TimeFlagProc( Owner );


	if( Ini()->BG_Independence_Game == true )
	{
		SendPlayerDataInfo_ZIP( Owner );
		NetSrv_Shop::S_FixSellItemBackup( Owner );
	}

	if( Ini()->IsBattleWorld )
	{
		SL_BattleZone( Owner->PlayerTempData->ChangeWorld.WorldID , Owner->PlayerTempData->ChangeWorld.RetZoneID , Owner->DBID() );
	}

	NetSrv_OtherChild::SendWeekInstancesLiveTime( Owner );
	NetSrv_OtherChild::PhantomChangeProc( Owner );

	Print( LV2 , "CliConnect" , "End  DBID=%d Role=%s RoomID=%d" , Owner->DBID() , Global::GetRoleName_ASCII( Owner ).c_str() , Owner->RoomID() );
}
//戰場角色登入處理
void NetSrv_CliConnectChild::SendAllPlayerInfoPlayerEnterProc	( BaseItemObject* PlayerObj )
{
	RoleDataEx* Owner = PlayerObj->Role();
	if( Global::Ini()->IsZonePartyEnabled == false )
		return;
	//通知所有玩家
	NetSrv_Move::S_AllCli_BattleGround_AddPlayer( Owner );

	//取得所有其他人的資訊
	BaseItemObject* OtherObj;
	for( OtherObj = BaseItemObject::GetByOrder_Player(true) ; OtherObj != NULL ; OtherObj = BaseItemObject::GetByOrder_Player() )
	{	
		if( OtherObj->GUID() == PlayerObj->GUID() )
			continue;

		NetSrv_Move::S_BattleGround_AddPlayer( Owner->GUID() , OtherObj->Role() );
	}

	Owner->TempData.BackInfo.ZonePlayerInfo.LX = Owner->X();
	Owner->TempData.BackInfo.ZonePlayerInfo.LZ = Owner->Z();
	Owner->TempData.BackInfo.ZonePlayerInfo.LHPMPSPSum = Owner->HPMPSPSum();
}
//-------------------------------------------------------------------
void NetSrv_CliConnectChild::SendLoginWarDeclare( RoleDataEx* Player )
{	
	if( Global::Ini()->IsBattleWorld )
		return;

	vector< GuildStruct* >& GuildList = GuildManageClass::This()->GuildList();

	for( unsigned i = 0 ; i < GuildList.size() ; i++ )
	{
		GuildStruct* Guild = GuildList[i];
		if( Guild == NULL )
			continue;
		if( Guild->Base.IsWar == false )
			continue;
		NetSrv_Guild::SC_LoginWarInfo( Player->GUID() , Guild->Base.GuildID , Guild->Base.WarGuildID , Guild->Base.WarTime - TimeStr::Now_Value() , Guild->Base.WarDeclareTime );
	}
}
//-------------------------------------------------------------------
void NetSrv_CliConnectChild::CliDisconnect( BaseItemObject* Player )
{
	RoleDataEx* Role = Player->Role();
	if( Role->ZoneID() == RoleDataEx::G_ZoneID )
		Print( LV2 , "CliDisconnect" , "~~" );

	Role->SaveDB_All();
    Player->ReleaseSock();

	Role->TempData.Sys.OnChangeZone = true;

	if( Global::Ini()->IsBattleWorld )
	{
		Role->LiveTime( 0 , "SYS CliDisconnect" );
	}
	else if(	Role->IsAttackMode() != false 
			||	Role->TempData.Attr.Action.PK != false )
		Role->LiveTime( 20*1000 , "SYS CliDisconnect" );
	else
		Role->LiveTime( 5*1000 , "SYS CliDisconnect" );

	if( Role->TempData.Sys.OnChangeWorld )
		SBL_ReturnWorldRequest( Role->PlayerTempData->ChangeWorld.WorldID , Role->PlayerTempData->ChangeWorld.RetZoneID , Role->DBID() , 0 );
}
//-------------------------------------------------------------------
//Client 端要求登出由戲 (30 秒倒數)
void NetSrv_CliConnectChild::CliLogout( BaseItemObject* Player )
{
	if( Player->Role()->IsAttackMode() != false || Player->Role()->TempData.Attr.Action.PK != false )
	{
		//Player->Role()->Msg( "戰鬥中不可登出" );
		Player->Role()->Net_GameMsgEvent( EM_GameMessageEvent_FightNoLogout );
		CliLogoutResult( Player->GUID() , false );
	}
	else
	{
		Player->Role()->LiveTime( 0 , "client ask to leave" );
		CliLogoutResult( Player->GUID() , true );
//		Global::CliLogout( Player->Role()->TempData.Sys.SockID , "Client Logout Request" );
	}
}
//-------------------------------------------------------------------
//查詢儲存進度的房間是否有存在
int		NetSrv_CliConnectChild::FindInstanceRoom( RoleDataEx* Player  )
{
	if( Global::Ini()->InstanceSaveID == -1 )
		return -1;

	int&	  KeyID			= Player->PlayerBaseData->InstanceSetting.KeyID[ Global::Ini()->InstanceSaveID ];
	unsigned& LiveTime		= Player->PlayerBaseData->InstanceSetting.LiveTime[ Global::Ini()->InstanceSaveID ];
	unsigned& ExtendTime	= Player->PlayerBaseData->InstanceSetting.ExtendTime[ Global::Ini()->InstanceSaveID ];

	if( KeyID == -1 )
		return -1;

	if(		LiveTime  != NetSrv_InstanceChild::_InstanceLiveTime  
		&&	ExtendTime != NetSrv_InstanceChild::_InstanceLiveTime )

		return -1;
	

	vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;
	for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
	{
		if( RoomList[i].IsActive == false )
			continue;
		
		if( RoomList[i].InstanceKeyID == KeyID )
			return i;
	}
	return -1;	
}
//有人要求換到此區域
void NetSrv_CliConnectChild::OnChangeZone ( PlayerRoleData* PlayerBase , int SrcZoneID )
{
	RoleDataEx* Player =(RoleDataEx*)PlayerBase;

	if( Global::St()->IsDisableLogin != false )
	{
		SendChangeZoneNotifyResult( SrcZoneID , Player->GUID() , Player->X() , Player->Y(), Player->Z(), EM_ChangeZoneNodifyResult_Failed );
		return;
	}


	if( Player->IsNPC() )
	{
		Global::CreateChangeZoneNPC( Player );
		return;
	}

	if( CNetSrv_BattleGround_Child::GetBattleGroundType() != 0 )
	{
		if( ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->CheckRoleIsExistBattleGround( Player->DBID() ) != false )
		{
			SendChangeZoneNotifyResult( SrcZoneID , Player->GUID() , Player->X() , Player->Y(), Player->Z(), EM_ChangeZoneNodifyResult_NoBattleGroundData );
			return;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//清除換區消失的物品
	//Player->InitChangeZone();
	//////////////////////////////////////////////////////////////////////////
	if( Global::Ini()->IsGuildHouseWarZone != false && Player->RoomID() == -1)
	{
		//取得角色公會資料
		GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Player->GuildID() );
		if( houseClass == NULL || houseClass->CheckLoadOK() == false )
		{
			SendChangeZoneNotifyResult( SrcZoneID , Player->GUID() , Player->X() , Player->Y(), Player->Z(), EM_ChangeZoneNodifyResult_Failed );
			return;
		}
		Player->RoomID(  houseClass->RoomID() );
		houseClass->GetPlayerPostion( Player->BaseData.Pos.X , Player->BaseData.Pos.Y , Player->BaseData.Pos.Z , Player->BaseData.Pos.Dir );		
	}
	else if( Global::Ini()->IsGuildHouseZone )
	{
		//如果物件數量過多 ,並且要進入的公會沒有載入則不能進入
		GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Player->PlayerTempData->VisitGuildID );
		if( houseClass == NULL && BaseItemObject::GetZoneObjList()->size() > 30000 )
		{
			SendChangeZoneNotifyResult( SrcZoneID , Player->GUID() , Player->X() , Player->Y(), Player->Z(), EM_ChangeZoneNodifyResult_Failed );
			return;
		}
	}
	else
	{
		if( FloatEQU( Player->X() , 0 ) && FloatEQU( Player->Y() , 0 ) && FloatEQU( Player->Z() , 0 ) )
		{
			Player->Pos()->X = (float)Ini()->CtrlMinX;
			Player->Pos()->Y = 0;
			Player->Pos()->Z = (float)Ini()->CtrlMinZ;

			//隨便找隻NPC
			BaseItemObject*	NPCObj = BaseItemObject::GetByOrder_NPC( true );
			if( NPCObj != NULL )
			{
				Player->Pos()->X = NPCObj->Role()->Pos()->X;
				Player->Pos()->Y = NPCObj->Role()->Pos()->Y+10;
				Player->Pos()->Z = NPCObj->Role()->Pos()->Z;
			}
		}
	}

	Player->ZoneID( RoleDataEx::G_ZoneID );

	//如果為獨立Zone 檢查是否有空間可以跳去
	if(		Player->RoomID() == -1
		&&	Global::Ini()->IsPrivateZone 
		&&	Global::Ini()->PrivateZoneType != EM_PrivateZoneType_None 
		&&  Global::Ini()->RoomCount >= 2 )
	{
		//1 找是否已經有屬於此人的獨立空間
		switch( Global::Ini()->PrivateZoneType )
		{
		case EM_PrivateZoneType_Private:
			{	
				int		EmptyRoomID = -1;
				bool	IsFind = false;
				bool	IsDuplicate = false;
				vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;
				for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
				{
					if( RoomList[i].IsActive == false && EmptyRoomID == -1 )
					{
						EmptyRoomID = i;
						continue;
					}

					if( RoomList[i].OwnerDBID == Player->DBID() )
					{
						IsFind = true;
						Player->RoomID( i );
						Player->PlayerBaseData->RoomKey = RoomList[ i ].RoomKey;
						break;	
					}
				}
				if( IsFind == false )
				{
					if(		EmptyRoomID == -1 
						||	BaseItemObject::GetZoneObjList()->size() > 30000	)
					{
						SendChangeZoneNotifyResult( SrcZoneID , Player->GUID() , Player->X() , Player->Y(), Player->Z(), EM_ChangeZoneNodifyResult_PraviteZone_NoRoom );
						return;
					}

					//查詢是否已有重複的進度在跑
					if( FindInstanceRoom( Player  ) != -1 )
					{
						SendChangeZoneNotifyResult( SrcZoneID , Player->GUID() , Player->X() , Player->Y(), Player->Z(), EM_ChangeZoneNodifyResult_InstancePartyKey_Duplicate );
						return;
					}


					//初始化區域
					CopyRoomMonster( Player->TempData.ChangeZoneInfo.CopyRoomID ,  EmptyRoomID );
					RoomList[ EmptyRoomID ].Init();
					RoomList[ EmptyRoomID ].IsActive = true;
					RoomList[ EmptyRoomID ].OwnerDBID = Player->DBID();
					if( Ini()->ZoneLiveTime >= 1 )
						RoomList[ EmptyRoomID ].LiveTime =  Ini()->ZoneLiveTime * 60 + RoleDataEx::G_Now;
					else
						RoomList[ EmptyRoomID ].LiveTime =  0;
					
					Player->RoomID( EmptyRoomID );
					Player->PlayerBaseData->RoomKey = RoomList[ EmptyRoomID ].RoomKey;
				}
			}
			break;
		case EM_PrivateZoneType_Party:
			{
				if( Player->PartyID() == -1 )
					return;

				int		EmptyRoomID = -1;
				bool	IsFind = false;
				vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;
				for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
				{
					if( RoomList[i].IsActive == false && EmptyRoomID == -1 )
					{
						EmptyRoomID = i;
						continue;
					}

					if( RoomList[i].OwnerPartyID == Player->PartyID() )
					{
						IsFind = true;
						Player->RoomID( i );
						Player->PlayerBaseData->RoomKey = RoomList[ i ].RoomKey;
						break;	
					}
				}
				//人數計算
				if( Global::RoomPlayerCount( Player->RoomID() ) >= Global::Ini()->RoomPlayerCount )
				{
					SendChangeZoneNotifyResult( SrcZoneID , Player->GUID() , Player->X() , Player->Y(), Player->Z(), EM_ChangeZoneNodifyResult_PraviteZone_NoRoom );
					return;
				}

				if( IsFind == false )
				{
					if(		EmptyRoomID == -1 
						||	BaseItemObject::GetZoneObjList()->size() > 30000	)	
					{
						SendChangeZoneNotifyResult( SrcZoneID , Player->GUID() , Player->X() , Player->Y(), Player->Z(), EM_ChangeZoneNodifyResult_PraviteZone_NoRoom );
						return;
					}

					//查詢是否已有重複的進度在跑
					if( FindInstanceRoom( Player  ) != -1 )
					{
						SendChangeZoneNotifyResult( SrcZoneID , Player->GUID() , Player->X() , Player->Y(), Player->Z(), EM_ChangeZoneNodifyResult_InstancePartyKey_Duplicate );
						return;
					}

					//初始化區域
					CopyRoomMonster( Player->TempData.ChangeZoneInfo.CopyRoomID , EmptyRoomID );
					RoomList[ EmptyRoomID ].Init();
					RoomList[ EmptyRoomID ].IsActive = true;
					RoomList[ EmptyRoomID ].OwnerDBID	 = Player->DBID();
					RoomList[ EmptyRoomID ].OwnerPartyID = Player->PartyID();

					Player->RoomID( EmptyRoomID );
					Player->PlayerBaseData->RoomKey = RoomList[ EmptyRoomID ].RoomKey;
				}
			}
			break;
		case EM_PrivateZoneType_Raid:
			return;
		case EM_PrivateZoneType_Private_Party:
			{
				int		EmptyRoomID = -1;
				bool	IsFind = false;
				vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;
				for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
				{
					if( RoomList[i].IsActive == false && EmptyRoomID == -1 )
					{
						EmptyRoomID = i;
						continue;
					}

					if( RoomList[i].OwnerPartyID != -1 )
					{
						if( Player->PartyID() == RoomList[i].OwnerPartyID )
						{
							IsFind = true;
							Player->RoomID( i );
							Player->PlayerBaseData->RoomKey = RoomList[ i ].RoomKey;
							break;
						}
					}
					else
					{
						if( RoomList[i].OwnerDBID == Player->DBID() && Player->PartyID() == -1 )
						{
							IsFind = true;
							Player->RoomID( i );
							Player->PlayerBaseData->RoomKey = RoomList[ i ].RoomKey;
							break;
						}
					}
				}

				//人數計算
				if(		Player->BaseData.Voc != EM_Vocation_GameMaster
					&&	Player->TempData.Attr.Effect.IgnoreInstancePlayer == false 
					&&  Global::RoomPlayerCount( Player->RoomID() ) >= Global::Ini()->RoomPlayerCount )
				{
					SendChangeZoneNotifyResult( SrcZoneID , Player->GUID() , Player->X() , Player->Y(), Player->Z(), EM_ChangeZoneNodifyResult_PraviteZone_NoRoom );
					return;
				}

				if( IsFind == false )
				{
					if(		EmptyRoomID == -1
						||	BaseItemObject::GetZoneObjList()->size() > 30000	)
					{
						SendChangeZoneNotifyResult( SrcZoneID , Player->GUID() , Player->X() , Player->Y(), Player->Z(), EM_ChangeZoneNodifyResult_PraviteZone_NoRoom );
						return;
					}

					//查詢是否已有重複的進度在跑
					if( FindInstanceRoom( Player  ) != -1 )
					{
						SendChangeZoneNotifyResult( SrcZoneID , Player->GUID() , Player->X() , Player->Y(), Player->Z(), EM_ChangeZoneNodifyResult_InstancePartyKey_Duplicate );
						return;
					}


					//初始化區域
					CopyRoomMonster( Player->TempData.ChangeZoneInfo.CopyRoomID , EmptyRoomID );
					RoomList[ EmptyRoomID ].Init();
					RoomList[ EmptyRoomID ].IsActive = true;
					if( Player->PartyID() != -1 )
					{
						RoomList[ EmptyRoomID ].OwnerDBID	 = -1;
						RoomList[ EmptyRoomID ].OwnerPartyID = Player->PartyID();
					}
					else
					{
						RoomList[ EmptyRoomID ].OwnerDBID	 = Player->DBID();
						RoomList[ EmptyRoomID ].OwnerPartyID = -1;
					}
					Player->RoomID( EmptyRoomID );
				}
			}
			break;

		}
	}

	if( Player->RoomID() >= Global::Ini()->RoomCount )
		Player->RoomID( 0 );

	if( Global::Ini()->InstanceSaveID != -1 )
	{
		PrivateRoomInfoStruct& RoomInfo = Global::St()->PrivateRoomInfoList[ Player->RoomID() ];

		//如果此party已經有key
		map< int , int >::iterator PartyIter = NetSrv_InstanceChild::_PartyKeyMap.find( Player->PartyID() );
		if( PartyIter != NetSrv_InstanceChild::_PartyKeyMap.end() )
		{
			RoomInfo.InstanceKeyID = PartyIter->second;
		}
		
		int&	  KeyID		= Player->PlayerBaseData->InstanceSetting.KeyID[ Global::Ini()->InstanceSaveID ];
		unsigned& LiveTime	= Player->PlayerBaseData->InstanceSetting.LiveTime[ Global::Ini()->InstanceSaveID ];
		unsigned& ExtendTime	= Player->PlayerBaseData->InstanceSetting.ExtendTime[ Global::Ini()->InstanceSaveID ];

		if(		LiveTime  != NetSrv_InstanceChild::_InstanceLiveTime 
			&&	ExtendTime < NetSrv_InstanceChild::_InstanceLiveTime )
		{
			KeyID = -1;
		}
		else
		{
			if( NetSrv_InstanceChild::_UpdatePartyLiveTime[KeyID] == false )
			{
				char	SqlCmd[512];
				NetSrv_InstanceChild::_UpdatePartyLiveTime[KeyID] = true;
				sprintf( SqlCmd , "UPDATE InstancePlayState SET LiveTime = getdate() + 14 WHERE ZoneID = %d and PartyKey =%d " 
								, RoleDataEx::G_ZoneID , KeyID	);
				Net_DCBase::SqlCommand( RoleDataEx::G_ZoneID , SqlCmd );

			}

		}

		if( RoomInfo.InstanceKeyID == -2 )
		{
		}
		else if( KeyID == RoomInfo.InstanceKeyID )
		{
		}
		else if( KeyID == -1  )
		{
		}
		else
		{
			SendChangeZoneNotifyResult( SrcZoneID , Player->GUID() , Player->X() , Player->Y(), Player->Z(), EM_ChangeZoneNodifyResult_InstancePartyKeyErr );
			return;
		}

	}


    Global::AddOnLoginPlayer( PlayerBase );

	//如果有此玩家（還沒解構的）,立即解構
	{
		BaseItemObject*		OldObj = BaseItemObject::GetObjByDBID( PlayerBase->Base.DBID );
		if( OldObj != NULL )
			delete OldObj;
	}
	

	//回應原來的 Server
	SendChangeZoneNotifyResult( SrcZoneID , Player->GUID() , Player->X() , Player->Y(), Player->Z(), EM_ChangeZoneNodifyResult_OK );
}
//-------------------------------------------------------------------
bool NetSrv_CliConnectChild::ChangeZone( RoleDataEx* Player )
{
	if( Player->TempData.IsDisabledChangeZone != false )
		return false;
    if( Player->ZoneID() == RoleDataEx::G_ZoneID )
        return false;

    SendPrepareChangeZone( Player );
    if( SendChangeZone( Player ) == false )
		Player->ZoneID( RoleDataEx::G_ZoneID );
	return true;
}
//-------------------------------------------------------------------
//要求修改物件
void NetSrv_CliConnectChild::OnModify( int CliID , ModifyNPCInfo* Info )
{
    BaseItemObject* OwnerObj = BaseItemObject::GetObjBySockID( CliID );
    if( OwnerObj == NULL )
        return;

	RoleDataEx* Owner = OwnerObj->Role();

	if(		Owner->PlayerBaseData->ManageLV <= EM_Management_GameMaster 
		&&	Global::Ini()->IsEnabledGMCommand == false )
	{
		Owner->Msg( "角色權限不足，無法使用GM命令" );
		return;
	}

    BaseItemObject* ModifyObj = BaseItemObject::GetObj( Info->GItemID );
    if( ModifyObj == NULL )
    {
        Owner->Msg( "修改失敗" );
        return;
    }
	GameObjDbStructEx* ObjDB = Global::GetObjDB( Info->OrgObjID );
	if( ObjDB == NULL )
	{
		Owner->Msg("無此物件");
		return;
	}

	ObjectModeStruct Mode;
    RoleDataEx* Modify = ModifyObj->Role();
    const char*		Name="";

	if( stricmp( Info->Name.Begin() , "-" ) == 0 )
		Name = "";
	else if( stricmp( Info->Name.Begin() , ObjDB->Name ) != 0 )
		Name = Info->Name.Begin();
    int		    OrgObjID	= Info->OrgObjID;
    //const char*	Name		= Info->Name.Begin();
    float	    x			= Info->X;
    float	    y			= Info->Y;
    float	    z			= Info->Z;
    float	    dir			= Info->Dir;
    int         count       = 1;
    const char* AutoPlot    = Info->AutoPlot.Begin();
    const char* ClassName   = Info->ClassName.Begin();
    int         QuestID     = Info->QuestID;
    Mode._Mode				= Info->Mode;
    int         PID         = Info->PID;

	float		vx			= Info->vX;
	float		vy			= Info->vY;
	float		vz			= Info->vZ;

    //簡查資料是否有修改
    if(     Modify->BaseData.ItemInfo.OrgObjID  == OrgObjID
        &&  Modify->BaseData.RoleName == Name 
        &&  FloatEQU( x , Modify->Pos()->X ) 
        &&  FloatEQU( y , Modify->Pos()->Y ) 
        &&  FloatEQU( z , Modify->Pos()->Z ) 
        &&  FloatEQU( dir , Modify->Pos()->Dir ) 
        &&  Modify->BaseData.ItemInfo.Count == 1
        &&  Modify->SelfData.AutoPlot == AutoPlot
        &&  Modify->SelfData.PlotClassName == ClassName
        &&  Modify->SelfData.NpcQuestID == QuestID
        &&  Modify->BaseData.Mode._Mode == Mode._Mode
        &&  Modify->SelfData.PID == PID        
		&&	Modify->BaseData.vX == vx 
		&&	Modify->BaseData.vY == vy 
		&&	Modify->BaseData.vZ == vz )
        return;


	int OrgNPCDBID = Modify->Base.DBID;


    if( OrgNPCDBID != -1 && Mode.Save == false )
    {
		Modify->TempData.Sys.DestroyAccount = Owner->Account_ID();
        Net_DCBase::DropNPCRequest( Modify );
		Global::CreateObj_Macro( Owner , OrgObjID , Name , x , y , z , dir , count 
			, AutoPlot , ClassName , QuestID , Mode._Mode , PID , -1 , vx , vy , vz	, Owner->RoomID() );
    }
	else
	{
		Modify->Base.DBID = -1;
		int ItemID = Global::CreateObj_Macro( Owner , OrgObjID , Name , x , y , z , dir , count 
			, AutoPlot , ClassName , QuestID , Mode._Mode , PID , OrgNPCDBID , vx , vy , vz , Owner->RoomID() );

		if( OrgNPCDBID != -1 )
		{
			RoleDataEx* NewRoleObj = Global::GetRoleDataByGUID( ItemID );
			//assert( NewRoleObj != NULL );
			if( NewRoleObj != NULL )
			{
				NewRoleObj->TempData.CreateAccount = Owner->Account_ID();
				Net_DCBase::SaveNPCRequest( NewRoleObj );
			}
			else
				Owner->Msg( "修改失敗 ItemID 有問題" );
		}

	}

    ModifyObj->Destroy( "SYS OnModify" );
}

void NetSrv_CliConnectChild::OnChangeZoneNodifyResult( int ZoneID , int GItemID , float X , float Y , float Z , ChangeZoneNodifyResult_ENUM Result )
{
	BaseItemObject* Obj = BaseItemObject::GetObj( GItemID );
	if( Obj == NULL )
		return;

	RoleDataEx* Owner = Obj->Role();

	Print( LV2 , "OnChangeZoneNodifyResult" , "Player DBID =%d Pos(%d,%d,%d,%d) Result=%d" , Owner->DBID() , ZoneID , int(X) , int(Y) , int(Z) , Result );

	switch( Result )
	{
	case EM_ChangeZoneNodifyResult_OK:
		{
			if( Owner->SelfData.LiveTime == 0 )
				return;
			
			Owner->SelfData.LiveTime = 0;

			Global::DelFromPartition( GItemID );
			Obj->Role()->ZoneID( ZoneID );
			Obj->Role()->Pos()->X = X;
			Obj->Role()->Pos()->Y = Y;
			Obj->Role()->Pos()->Z = Z;
			Obj->Role()->TempData.Move.CliX = X;
			Obj->Role()->TempData.Move.CliY = Y;
			Obj->Role()->TempData.Move.CliZ = Z;
			Obj->Role()->TempData.Move.LCheckLineX = X;
			Obj->Role()->TempData.Move.LCheckLineY = Y;
			Obj->Role()->TempData.Move.LCheckLineZ = Z;			


			Obj->Destroy( "SYS OnChangeZoneNodifyResult" );
			_Net->ConnectCliAndSrv( GetZoneNetID( ZoneID ), Obj->Role()->TempData.Sys.SockID );

			//把所有有關此物件的寵物換區
			vector< BaseItemObject* >& ObjList = *BaseItemObject::GetZoneObjList();
			for( int i = 0 ; i < (int)ObjList.size() ; i++  )
			{
				BaseItemObject* PetObj = ObjList[i];
				if( PetObj == NULL )
					continue;

				//墓碑則不處理
				if( PetObj->Role()->BaseData.ItemInfo.OrgObjID == g_ObjectData->SysValue().TombObjID )
					continue;


				if(		PetObj->Role()->BaseData.Mode.Move == false 
					&&	PetObj->Role()->IsPet( Owner ) == false )
					continue;

				if( PetObj->Role()->DBID() > 0 )
					continue;
				
				if( PetObj->Role()->TempData.Sys.OwnerDBID == Owner->DBID() )
				{
					PetObj->Role()->ZoneID( ZoneID );
					SendChangeZone( PetObj->Role() );
					PetObj->Destroy( "SYS OnChangeZoneNodifyResult" );
				}
			}
			
		}
		break;
	case EM_ChangeZoneNodifyResult_PraviteZone_NoRoom:
		Obj->Role()->TempData.Sys.OnChangeZone = false;
		//Obj->Role()->Msg( "換區失敗　獨立Zone滿了" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_ChanageZoneError_PraviteZoneFull );
		break;
	case EM_ChangeZoneNodifyResult_Failed:
		Obj->Role()->TempData.Sys.OnChangeZone = false;
		//Obj->Role()->Msg( "換區失敗" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_ChanageZoneError_Failed );
		break;
	case EM_ChangeZoneNodifyResult_InstancePartyKeyErr:
		Obj->Role()->TempData.Sys.OnChangeZone = false;
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_ChanageZoneError_InstanceKey );
		break;
	case EM_ChangeZoneNodifyResult_InstancePartyKey_Duplicate:
		Obj->Role()->TempData.Sys.OnChangeZone = false;
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_ChanageZoneError_InstanceKeyDuplicate );
		break;
	default:
		Obj->Role()->TempData.Sys.OnChangeZone = false;
		break;
	}
	
	if(		Obj->Role()->LiveTime() <= 30000  )	 
		Obj->Role()->TempData.Sys.OnChangeZone = true;
}

void NetSrv_CliConnectChild::RC_GetNPCMoveInfoRequest( BaseItemObject* Player , int NpcGUID )
{
	//if( !( Player->Role()->IsNPC() || Player->Role()->IsQuestNPC() ) )
	//	return;
	BaseItemObject* pObj = BaseItemObject::GetObj( NpcGUID );

	if( pObj == NULL || pObj->Role()->IsNPC() == false ) // IsNPC() 函式包函了 NPC 與 QuestNPC
		return;

	RoleDataEx* Owner = pObj->Role();

	if(		Owner->PlayerBaseData->ManageLV <= EM_Management_GameMaster 
		&&	Global::Ini()->IsEnabledGMCommand == false )
	{
		Owner->Msg( "角色權限不足，無法使用GM命令" );
		return;
	}

	vector< NPC_MoveBaseStruct >* MoveList = pObj->NPCMoveInfo()->MoveInfo();

	SendNPCMoveInfo( Player->GUID() , NpcGUID , *MoveList );

}
void NetSrv_CliConnectChild::RC_SaveNPCMoveInfo( BaseItemObject* Player , int NpcGUID , int TotalCount , int IndexID , NPC_MoveBaseStruct& Base )
{
	BaseItemObject* pObj = BaseItemObject::GetObj( NpcGUID );

	if( pObj == NULL || pObj->Role()->IsNPC() == false ) // IsNPC() 函式包函了 NPC 與 QuestNPC
		return;

	RoleDataEx* Owner = pObj->Role();

	if(		Owner->PlayerBaseData->ManageLV <= EM_Management_GameMaster 
		&&	Global::Ini()->IsEnabledGMCommand == false )
	{
		Owner->Msg( "角色權限不足，無法使用GM命令" );
		return;
	}

	if( pObj->Role()->DBID() < 0 )
	{
		pObj->Role()->Msg( "NPC 需要儲存才可以設定路徑" );
		return;
	}

	vector< NPC_MoveBaseStruct >* MoveList = pObj->NPCMoveInfo()->MoveInfo();	
	if( IndexID <= 0 )
	{
		MoveList->clear();
	}

	if( IndexID == (int)(MoveList->size() ) )
	{
		MoveList->push_back( Base );
	}

	if( TotalCount == (int)(MoveList->size() ) )
	{
		Net_DCBase::SaveNPCMoveInfo( RoleDataEx::G_ZoneID , pObj->Role()->DBID() , *MoveList );
	}

/*
	if( !Player->Role()->IsNPC() )
		return;

	vector< NPC_MoveBaseStruct >* MoveList = Player->NPCMoveInfo()->MoveInfo();	
	if( IndexID <= 0 )
	{
		MoveList->clear();
	}
	
	if( IndexID == (int)(MoveList->size() ) )
	{
		MoveList->push_back( Base );
	}

	if( TotalCount == (int)(MoveList->size() ) )
	{
		Net_DCBase::SaveNPCMoveInfo( RoleDataEx::G_ZoneID , Player->Role()->DBID() , *MoveList );
	}
*/
}

void NetSrv_CliConnectChild::RC_GetFlagList		( BaseItemObject* Player )
{

	RoleDataEx* Owner = Player->Role();

	if(		Owner->PlayerBaseData->ManageLV <= EM_Management_GameMaster 
		&&	Global::Ini()->IsEnabledGMCommand == false )
	{
		Owner->Msg( "角色權限不足，無法使用GM命令" );
		return;
	}


	PG_CliConnect_LtoC_GetFlagListResult Packet;

	struct TempFlagInfo
	{
		int             ObjID;      //類別
		int             ID;         //編號
		float           X;
		float           Y;
		float           Z;    
		float			Dir;
		int				FlagDBID;	//旗子資料庫ID
	} FlagInfo;

	
	vector< vector < FlagPosInfo > >*			pVecFlagList	= FlagPosClass::FlagList();
	vector< vector < FlagPosInfo > >::iterator	it;

	int iFlagCount		= 0;
	int iTotalFlagCount	= 0;

	int iTotalGroup = (int)pVecFlagList->size();

	for( it = pVecFlagList->begin(); it != pVecFlagList->end(); it++ )
	{
		iTotalFlagCount += int( (*it).size() );
	}

	// 封包 Title + TotalGroup + N* ( 每個 Group 的 Count + 資料 )
	int		iPacketSize		= ( sizeof( FlagPosInfo ) * iTotalFlagCount ) + ( sizeof( int) * ( 2 + iTotalGroup ) );
	BYTE*	pData			= NEW BYTE[ iPacketSize ];
	BYTE*	ptr				= pData;

	// Command
	memcpy( ptr, &Packet.Command, sizeof(Packet.Command) );
	ptr += sizeof( Packet.Command );

	// Total
	memcpy( ptr, &iTotalGroup, sizeof(iTotalGroup) );
	ptr += sizeof( iTotalGroup );


	// Flag Data
	for( it = pVecFlagList->begin(); it != pVecFlagList->end(); it++ )
	{
		iFlagCount = int( (*it).size() );
		memcpy( ptr, &iFlagCount, sizeof(iFlagCount) ) ;
		ptr += sizeof( iFlagCount );

		for( vector < FlagPosInfo >::iterator itFlag = (*it).begin(); itFlag != (*it).end(); itFlag++ )
		{
			FlagInfo.ObjID		= (*itFlag).ObjID;
			FlagInfo.ID			= (*itFlag).ID;
			FlagInfo.X			= (*itFlag).X;
			FlagInfo.Y			= (*itFlag).Y;
			FlagInfo.Z			= (*itFlag).Z;
			FlagInfo.Dir		= (*itFlag).Dir;
			FlagInfo.FlagDBID	= (*itFlag).FlagDBID;

			memcpy( ptr, &(FlagInfo), sizeof( TempFlagInfo ) );
			ptr += sizeof( TempFlagInfo );
		}
	}

	Global::SendToCli_ByGUID( Player->GUID() , iPacketSize , pData );
}

void NetSrv_CliConnectChild::RC_EditFlag		( BaseItemObject* Player, int iFlagGroup, int iFlagID, float X, float Y, float Z, float Dir, int FlagDBID )
{
	RoleDataEx* Owner = Player->Role();

	if(		Owner->PlayerBaseData->ManageLV <= EM_Management_GameMaster 
		&&	Global::Ini()->IsEnabledGMCommand == false )
	{
		Owner->Msg( "角色權限不足，無法使用GM命令" );
		return;
	}

	int DBID = -1;
	//
	FlagPosInfo* FlagData =  FlagPosClass::GetFlag( iFlagGroup , iFlagID );
	if( FlagData != NULL )
		DBID = FlagData->FlagDBID;

	int Ret = CreateFlag( iFlagGroup, iFlagID , X , Y , Z , Dir , DBID , true );
	//int Ret = FlagPosClass::AddFlag( NULL , iFlagGroup , iFlagID , X , Y , Z , Dir );
	FlagPosClass::Hide( iFlagGroup );
	EditFlagResult( Player->GUID(), Ret );
}

void NetSrv_CliConnectChild::RC_DelFlag			( BaseItemObject* Player, int iFlagGroup, int iFlagID )
{
	RoleDataEx* Owner = Player->Role();

	if(		Owner->PlayerBaseData->ManageLV <= EM_Management_GameMaster 
		&&	Global::Ini()->IsEnabledGMCommand == false )
	{
		Owner->Msg( "角色權限不足，無法使用GM命令" );
		return;
	}

	int iResult = (int)FlagPosClass::DelFlag( iFlagGroup , iFlagID , Owner->Account_ID() );
	DelFlagResult( Player->GUID(), iResult );


}

void NetSrv_CliConnectChild::RC_DelFlagGroup	( BaseItemObject* Player, int iFlagGroup )
{
	RoleDataEx* Owner = Player->Role();

	if(		Owner->PlayerBaseData->ManageLV <= EM_Management_GameMaster 
		&&	Global::Ini()->IsEnabledGMCommand == false )
	{
		Owner->Msg( "角色權限不足，無法使用GM命令" );
		return;
	}

	int iResult = (int)FlagPosClass::DelFlag_Group( iFlagGroup , Owner->Account_ID() );
	DelFlagGroupResult( Player->GUID(), iResult );
}

void NetSrv_CliConnectChild::RC_ConnectToBattleWorldOK	( BaseItemObject* Player , int CliNetID )
{
	RoleDataEx* Owner = Player->Role();

	SBL_ConnectCliAndGSrv( Owner->PlayerTempData->ChangeWorld.WorldID , Owner->PlayerTempData->ChangeWorld.WorldNetID , CliNetID );

	//////////////////////////////////////////////////////////////////////////
	//如果是載具上面所有人下載具
	vector< AttachObjInfoStruct >& AttachList = Player->AttachList();
	for( unsigned i = 0 ; i < AttachList.size() ; i++ )
	{
		if( AttachList[i].IsEmpty() )
			continue;
		LuaPlotClass::DetachObj( AttachList[i].ItemID );
	}
	LuaPlotClass::DetachObj( Owner->GUID() );
	//////////////////////////////////////////////////////////////////////////

	Global::DelFromPartition( Owner->GUID() );
}

void NetSrv_CliConnectChild::RBL_ChangeZoneToOtherWorld( int FromWorldId , int FromNetID , PlayerRoleData& RoleBase , int RoleSize )
{
	if( sizeof( PlayerRoleData ) != RoleSize  )
	{
		SBL_ChangeZoneToOtherWorldResult( FromWorldId , FromNetID , RoleBase.Base.DBID , (char*)(Global::Ini()->SwitchIP_Outside.c_str()) , Global::Ini()->Switch_CliPort , EM_ChangeZoneToOtherWorldResult_Failed );
		return;
	}
	RoleBase.PlayerTempData.ChangeWorld.WorldID = FromWorldId;
	RoleBase.PlayerTempData.ChangeWorld.WorldNetID = FromNetID;
	RoleBase.Base.DBID += FromWorldId * _DEF_MAX_DBID_COUNT_; 
	RoleBase.BaseData.GuildID |= ( FromWorldId * 0x1000000 ) ;
//	RoleBase.TempData.Sys.WorldGUID = -1;
	//////////////////////////////////////////////////////////////////////////
	//清除寵物資訊
	for( int i = 0 ; i < MAX_CultivatePet_Count ; i++ )
	{
		CultivatePetStruct& PetTemp = RoleBase.PlayerBaseData.Pet.Base[i];
		if( PetTemp.IsEmpty() )
			continue;
		GameObjDbStructEx* ItemEggDB = Global::GetObjDB( PetTemp.ItemOrgID );
		if( ItemEggDB == NULL )
			continue;

		if( PetTemp.EventType == EM_CultivatePetCommandType_Summon )
			PetTemp.EventType = EM_CultivatePetCommandType_None;
	}
	//////////////////////////////////////////////////////////////////////////
	if( Global::Ini()->IsGuildHouseWarZone != false && RoleBase.BaseData.RoomID == -1)
	{
		//看是否可以進入公會戰場

		//取得角色公會資料
		GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( RoleBase.BaseData.GuildID );
		if( houseClass == NULL || houseClass->CheckLoadOK() == false )
		{
			SBL_ChangeZoneToOtherWorldResult( FromWorldId , FromNetID , RoleBase.Base.DBID , (char*)(Global::Ini()->SwitchIP_Outside.c_str()) , Global::Ini()->Switch_CliPort , EM_ChangeZoneToOtherWorldResult_Failed );
			return;
		}

		GuildHouseWarManageClass* GuildWar = GuildHouseWarManageClass::GetGuildWar_ByRoomID( houseClass->RoomID() );
		if(		GuildWar ==  NULL 
			||	GuildWar->WarBase().IsReady == false 
			||	GuildWar->WarBase().IsEndWar != false )
		{
			SBL_ChangeZoneToOtherWorldResult( FromWorldId , FromNetID , RoleBase.Base.DBID , (char*)(Global::Ini()->SwitchIP_Outside.c_str()) , Global::Ini()->Switch_CliPort , EM_ChangeZoneToOtherWorldResult_Failed );
			return;
		}

		//計算已進入的人數
		if( Global::Ini()->Max_GuildWorld_PlayerCount != 0 )
		{
			int GuildPlayerCount = 0;

			for(	BaseItemObject* PlayerObj = BaseItemObject::GetByOrder_Player( true ) ;
					PlayerObj != NULL ; 
					PlayerObj = BaseItemObject::GetByOrder_Player() )
			{
				if( PlayerObj->Role()->GuildID() == RoleBase.BaseData.GuildID )
					GuildPlayerCount++;
			}

			//計算登入中的角色
			map<string , PlayerRoleData*>::iterator Iter;
			for( Iter = St()->OnLoginPlayer.begin() ; Iter != St()->OnLoginPlayer.end() ; Iter++ )
			{
				if( Iter->second->BaseData.GuildID == RoleBase.BaseData.GuildID )
					GuildPlayerCount++;
			}

			if( GuildPlayerCount >= Global::Ini()->Max_GuildWorld_PlayerCount )
			{
				SBL_ChangeZoneToOtherWorldResult( FromWorldId , FromNetID , RoleBase.Base.DBID , (char*)(Global::Ini()->SwitchIP_Outside.c_str()) , Global::Ini()->Switch_CliPort , EM_ChangeZoneToOtherWorldResult_Full );
				return;
			}

		}


		RoleBase.BaseData.RoomID = houseClass->RoomID();
		houseClass->GetPlayerPostion( RoleBase.BaseData.Pos.X , RoleBase.BaseData.Pos.Y , RoleBase.BaseData.Pos.Z , RoleBase.BaseData.Pos.Dir );		
	}

	//如果是寵物


	bool Ret = Global::AddOnLoginPlayer( &RoleBase );
	if( Ret == false )
		SBL_ChangeZoneToOtherWorldResult( FromWorldId , FromNetID , RoleBase.Base.DBID , (char*)(Global::Ini()->SwitchIP_Outside.c_str()) , Global::Ini()->Switch_CliPort , EM_ChangeZoneToOtherWorldResult_Failed );
	else
		SBL_ChangeZoneToOtherWorldResult( FromWorldId , FromNetID , RoleBase.Base.DBID , (char*)(Global::Ini()->SwitchIP_Outside.c_str()) , Global::Ini()->Switch_CliPort , EM_ChangeZoneToOtherWorldResult_OK );
}
void NetSrv_CliConnectChild::RBL_ChangeZoneToOtherWorldResult( int FromWorldId , int FromNetID , int RoleDBID , char* IPStr , int Port , ChangeZoneToOtherWorldResultENUM Result )
{
	RoleDataEx* Role = Global::GetRoleDataByDBID( RoleDBID );
	if( Role == NULL )
		return;

	if( Result != EM_ChangeZoneToOtherWorldResult_OK )
	{
		if( Role->LiveTime() <= 30000 ) 
			Role->TempData.Sys.OnChangeZone = true;
		else
			Role->TempData.Sys.OnChangeZone = false;
		Role->TempData.Sys.OnChangeWorld = false;

		switch( Result )
		{
			case EM_ChangeZoneToOtherWorldResult_Failed:

				break;
			case EM_ChangeZoneToOtherWorldResult_Full:		//人數已滿
				Role->Net_GameMsgEvent( EM_GameMessageEvent_GuildWar_PlayerCountFull );
				break;
		}

		return;
	}

	if( Role->IsPlayer()   )
	{
		//把所有寵物都移到自己的位置
		vector< BaseItemObject* >& ObjList = *BaseItemObject::GetZoneObjList();
		for( int i = 0 ; i < (int)ObjList.size() ; i++  )
		{
			BaseItemObject* PetObj = ObjList[i];
			if( PetObj == NULL || PetObj->Role()->IsDestroy() != false )
				continue;

			if( PetObj->Role()->BaseData.Mode.Move == false )
				continue;

			if( PetObj->Role()->TempData.Sys.OwnerDBID == Role->DBID() && Role->DBID() != -1 )
			{
				PetObj->Destroy( "chenage world destroy" );
			}
		}
	}
	Role->TempData.UpdateInfo.AllZoneInfoChange = true;
	Role->TempData.Sys.OnChangeZone = true;
	Role->TempData.Sys.OnChangeWorld = true;
	Role->PlayerTempData->ChangeWorld.WorldID = FromWorldId;
	Role->PlayerTempData->ChangeWorld.WorldNetID = FromNetID;
//	Role->TempData.Sys.WorldGUID = -1;
	SC_ConnectToBattleWorld( Role->GUID() , IPStr , Port );
}
void NetSrv_CliConnectChild::RBL_ConnectCliAndGSrv( int FromWorldId , int FromNetID , int CliNetID  )
{
	_Net->ConnectCliAndSrv( _Net->NetID() , CliNetID );
}
void NetSrv_CliConnectChild::RBL_BattleSavePlayer( int FromWorldId , int FromNetID , int PlayerDBID , RoleDataEx* RetRole )
{
	RoleDataEx* Role = Global::GetRoleDataByDBID( PlayerDBID );
	if( Role == NULL || Role->TempData.Sys.OnChangeWorld == false  )
	{
		//送到戰場請角色登出
		return;
	}

	if( RetRole->PlayerTempData->BgReturn.IsIndependenceGame )
	{
		if( RetRole->PlayerTempData->BgReturn.MoneyProcType == EM_BGIndependenceGameProc_Rewrite )
			Role->PlayerBaseData->Body.Money = RetRole->PlayerBaseData->Body.Money;
		return;
	}

	if( Role->WorldGUID() != RetRole->WorldGUID() )
	{
		Print( LV5 , "RBL_BattleSavePlayer" , "ZoneKey Error(cheat mode??) , playerDBID=%d" , PlayerDBID );
		return;
	}
	RetRole->PlayerTempData->ChangeWorld = Role->PlayerTempData->ChangeWorld;

	//拷貝  物品 經驗值 buff資料
	//////////////////////////////////////////////////////////////////////////
	//RetRole->TempData.Sys.WorldGUID = Role->TempData.Sys.WorldGUID;
//	RetRole->TempData.Sys		= Role->TempData.Sys;
	RetRole->BaseData.PartyID	= Role->BaseData.PartyID;
	RetRole->BaseData.GuildID	= Role->BaseData.GuildID;
	RetRole->Base.DBID			= Role->Base.DBID;
	RetRole->BaseData.RoomID	= Role->BaseData.RoomID;
	RetRole->BaseData.ZoneID	= Role->BaseData.ZoneID;
	RetRole->BaseData.Pos		= Role->BaseData.Pos;
	RetRole->BaseData.GuildLeaveTime = Role->BaseData.GuildLeaveTime;
	RetRole->PlayerBaseData->Quest.LastCompleteTime = Role->PlayerBaseData->Quest.LastCompleteTime;

	//////////////////////////////////////////////////////////////////////////
	Role->PlayerSelfData->DestroyItemLog = RetRole->PlayerSelfData->DestroyItemLog;
	Role->PlayerSelfData->MonsterHunter = RetRole->PlayerSelfData->MonsterHunter;
	Role->TempData.BackInfo.BuffClearTime.ChangeZone =true;
	Role->BaseData = RetRole->BaseData;
	*(Role->PlayerBaseData) = *(RetRole->PlayerBaseData);

	if( Role->BaseData.HP == 0 )
		Role->BaseData.HP = 1;

	Role->InitCheckRoleData();
}
void NetSrv_CliConnectChild::BGIndependenceGameProc( RoleDataEx* Owner )
{
	BattleGroundReturnValueStruct& BgReturn = Owner->PlayerTempData->BgReturn;
	BgReturn.Init();
	if( Ini()->BG_Independence_Game == false )
		return;

	BgReturn.IsIndependenceGame = true;
	BgReturn.MoneyProcType		= (BGIndependenceGameProcENUM)Ini()->BG_Independence_Game_Money;
	BgReturn.ItemProcType		= (BGIndependenceGameProcENUM)Ini()->BG_Independence_Game_Item;
	BgReturn.KeyItemProcType	= (BGIndependenceGameProcENUM)Ini()->BG_Independence_Game_KeyItem;
	BgReturn.TitleProcType		= (BGIndependenceGameProcENUM)Ini()->BG_Independence_Game_Title;
	BgReturn.CardProcType		= (BGIndependenceGameProcENUM)Ini()->BG_Independence_Game_Card;
	BgReturn.RetLua				= Ini()->BG_OrgWorld_LeaveScript.c_str();

	Owner->BaseData.EQ.Init();					
	memset( &Owner->PlayerBaseData->Body.Item , 0 , sizeof(Owner->PlayerBaseData->Body.Item ) );
	Owner->PlayerBaseData->Bank.Init();			
	memset( &Owner->PlayerBaseData->EQBackup , 0 , sizeof( Owner->PlayerBaseData->EQBackup ) );
	Owner->PlayerBaseData->ItemTemp.Clear();		
	memset( &Owner->PlayerSelfData->DestroyItemLog , 0 , sizeof( Owner->PlayerSelfData->DestroyItemLog ) );
	memset( &Owner->PlayerBaseData->HorseExBag , 0 , sizeof( Owner->PlayerBaseData->HorseExBag ) );
	memset( &Owner->PlayerBaseData->PetExBag , 0 , sizeof( Owner->PlayerBaseData->PetExBag ) );
	Owner->PlayerBaseData->Card.ReSet();

	if( BgReturn.MoneyProcType != EM_BGIndependenceGameProc_Rewrite )
		Owner->PlayerBaseData->Body.Money = 0;
}
void NetSrv_CliConnectChild::IndependenceGameReturnProc( RoleDataEx* Role , RoleDataEx* RetRole )
{
	BattleGroundReturnValueStruct& BgReturn = RetRole->PlayerTempData->BgReturn;
	Role->PlayerTempData->BgReturn = BgReturn;
	if( BgReturn.IsIndependenceGame == false )
	{
		if( strlen( BgReturn.RetLua.Begin() )  )
		{
			LUA_VMClass::PCall( BgReturn.RetLua.Begin() , Role->GUID() , Role->GUID() );
		}
		return;
	}


	switch( BgReturn.ItemProcType )
	{
	case EM_BGIndependenceGameProc_None:
		break;
	case EM_BGIndependenceGameProc_Rewrite:
		{
			/*
			Role->BaseData.EQ					= RetRole->BaseData.EQ;
			Role->PlayerBaseData->Body.Item		= RetRole->PlayerBaseData->Body.Item;
			Role->PlayerBaseData->Bank			= RetRole->PlayerBaseData->Bank;
			Role->PlayerBaseData->EQBackup		= RetRole->PlayerBaseData->EQBackup;
			Role->PlayerBaseData->ItemTemp		= RetRole->PlayerBaseData->ItemTemp;
			Role->PlayerSelfData->DestroyItemLog = RetRole->PlayerSelfData->DestroyItemLog;
			*/
		}
		break;
	case EM_BGIndependenceGameProc_Merge:
		{
			for( int i = 0 ; i< _MAX_BODY_COUNT_ ; i++ )
			{
				ItemFieldStruct& item = Role->PlayerBaseData->Body.Item[i];
				if( item.IsEmpty() )
					continue;
				Role->GiveItem( item , EM_ActionType_BG_IndependenceGame , NULL , "" );
			}

			for( int i = 0 ; i< Role->PlayerBaseData->ItemTemp.Size() ; i++ )
			{
				ItemFieldStruct& item = Role->PlayerBaseData->ItemTemp[i];
				if( item.IsEmpty() )
					continue;
				Role->GiveItem( item , EM_ActionType_BG_IndependenceGame , NULL , "" );
			}
		}
		break;
	}

	switch( BgReturn.KeyItemProcType )
	{
	case EM_BGIndependenceGameProc_None:
		break;
	case EM_BGIndependenceGameProc_Rewrite:
		{
			Role->BaseData.KeyItem = RetRole->BaseData.KeyItem;
		}
		break;
	case EM_BGIndependenceGameProc_Merge:
		{
			for( int i = 0 ; i < RetRole->BaseData.KeyItem.Size() ; i++ )
			{
				if( RetRole->BaseData.KeyItem.GetFlag(i) == false )
					continue;
				Role->BaseData.KeyItem.SetFlagOn( i );
			}
		}
		break;
	}
	switch( BgReturn.MoneyProcType )
	{
	case EM_BGIndependenceGameProc_None:
		break;
	case EM_BGIndependenceGameProc_Rewrite:
		{
			Role->PlayerBaseData->Body.Money = RetRole->PlayerBaseData->Body.Money;
		}
		break;
	case EM_BGIndependenceGameProc_Merge:
		{
			Role->AddBodyMoney( RetRole->PlayerBaseData->Body.Money , NULL , EM_ActionType_BG_IndependenceGame , false );
		}
		break;
	}

	switch( BgReturn.TitleProcType )
	{
	case EM_BGIndependenceGameProc_None:
		break;
	case EM_BGIndependenceGameProc_Rewrite:
		{
			Role->PlayerBaseData->Title = RetRole->PlayerBaseData->Title;
		}
		break;
	case EM_BGIndependenceGameProc_Merge:
		{
			for( int i = 0 ; i < RetRole->PlayerBaseData->Title.Size() ; i++ )
			{
				if( RetRole->PlayerBaseData->Title.GetFlag(i) == false )
					continue;
				Role->PlayerBaseData->Title.SetFlagOn( i );
			}
		}
		break;
	}

	switch( BgReturn.CardProcType )
	{
	case EM_BGIndependenceGameProc_None:
		break;
	case EM_BGIndependenceGameProc_Rewrite:
		break;
	case EM_BGIndependenceGameProc_Merge:
		{
			for( int i = 0 ; i < RetRole->PlayerBaseData->Card.Size() ; i++ )
			{
				if( RetRole->PlayerBaseData->Card.GetFlag(i) == false )
					continue;
				Role->PlayerBaseData->Card.SetFlagOn( i );
			}
		}
		break;
	}


	if( strlen( BgReturn.RetLua.Begin() )  )
	{
		LUA_VMClass::PCall( BgReturn.RetLua.Begin() , Role->GUID() , Role->GUID() );
	}

}
void NetSrv_CliConnectChild::RBL_RetNomorlWorld( int FromWorldId , int FromNetID , int PlayerDBID , RoleDataEx* RetRole )
{
	BaseItemObject* RoleObj = BaseItemObject::GetObjByDBID( PlayerDBID );
	if( RoleObj == NULL )
		return;
	RoleDataEx* Role = RoleObj->Role();
	if(	Role->TempData.Sys.OnChangeWorld == false 	)
		return;

	if( Role->WorldGUID() != RetRole->WorldGUID() )
	{
		Print( LV5 , "RBL_RetNomorlWorld" , "ZoneKey Error(cheat mode??) , playerDBID=%d" , PlayerDBID );
		return;
	}

	Global::DelFromPartition( Role->GUID() );
	//////////////////////////////////////////////////////////////////////////

	if( RetRole->PlayerTempData->BgReturn.IsIndependenceGame == false )
	{
		RetRole->TempData.Sys.WorldGUID = Role->TempData.Sys.WorldGUID;
		RetRole->BaseData.PartyID	= Role->BaseData.PartyID;
		RetRole->BaseData.GuildID	= Role->BaseData.GuildID;
		RetRole->Base.DBID			= Role->Base.DBID;
		RetRole->BaseData.RoomID	= Role->BaseData.RoomID;
		RetRole->BaseData.ZoneID	= Role->BaseData.ZoneID;
		RetRole->BaseData.Pos		= Role->BaseData.Pos;
		//每日任務被份
		RetRole->BaseData.GuildLeaveTime = Role->BaseData.GuildLeaveTime;
		RetRole->PlayerBaseData->Quest = Role->PlayerBaseData->Quest;
		//////////////////////////////////////////////////////////////////////////
		Role->PlayerSelfData->DestroyItemLog = RetRole->PlayerSelfData->DestroyItemLog;
		Role->PlayerSelfData->MonsterHunter = RetRole->PlayerSelfData->MonsterHunter;
		
		Role->BaseData = RetRole->BaseData;
		*(Role->PlayerBaseData) = *(RetRole->PlayerBaseData);
	}
	else
	{
		IndependenceGameReturnProc( Role , RetRole );
	}

	Role->TempData.Attr.Action.ChangeZone = true;
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	Role->ChangeZoneInit();
	//設定角色資料與初始化
	//----------------------------------------------------------------
	Role->InitCheckRoleData();
	Role->CheckAndClearCopyItem( );
	Role->ResetItemInfo();
	//初始化備份檢查
	Role->InitOnTimeCheck();
	Role->InitCal();
	Role->GetTitle( );	//以目前的等級資料取得頭銜
	Role->TempData.Attr.Action.SendAllZoneObj = true;
	Role->TempData.BackInfo.BuffClearTime.ChangeZone =true;
	Role->TempData.Sys.OnChangeZone		= false;
	Role->TempData.Sys.OnChangeWorld	= false;
	Role->TempData.UpdateInfo.AllZoneInfoChange = true;

	if( Role->LiveTime() <= 20000 )
		Role->TempData.Sys.OnChangeZone	= true;

	if( Role->BaseData.HP == 0 )
		Role->BaseData.HP = 1;

	Role->IsDead( false );
	Role->PlayerBaseData->DeadCountDown	= 0;
	//////////////////////////////////////////////////////////////////////////
	//清除寵物資訊
	for( int i = 0 ; i < MAX_CultivatePet_Count ; i++ )
	{
		CultivatePetStruct& PetTemp = Role->PlayerBaseData->Pet.Base[i];
		if( PetTemp.IsEmpty() )
			continue;
		GameObjDbStructEx* ItemEggDB = Global::GetObjDB( PetTemp.ItemOrgID );
		if( ItemEggDB == NULL )
			continue;

		if( PetTemp.EventType == EM_CultivatePetCommandType_Summon )
			PetTemp.EventType = EM_CultivatePetCommandType_None;
	}
	//////////////////////////////////////////////////////////////////////////
	//強PK Buff 處理
	if( Global::Ini()->DisableGoodEvil == false )
	{
		GoodEvilTypeENUM GoodEvilType =	Role->GetGoodEvilType( );
		Role->AssistMagic( Role , g_ObjectData->SysValue().GoodEvilBuf[GoodEvilType-EM_GoodEvil_Demon ] , 0 , false , -1 );
	}
	//////////////////////////////////////////////////////////////////////////	
	//如果是載具上面所有人下載具
	vector< AttachObjInfoStruct >& AttachList = RoleObj->AttachList();
	for( unsigned i = 0 ; i < AttachList.size() ; i++ )
	{
		if( AttachList[i].IsEmpty() )
			continue;
		LuaPlotClass::DetachObj( AttachList[i].ItemID );
	}
	LuaPlotClass::DetachObj( Role->GUID() );
	//////////////////////////////////////////////////////////////////////////

//	SendPlayerDataInfo_ZIP( Role );


	if( CheckZoneID( Role->PlayerTempData->ChangeWorld.RetZoneID ) == false )
	{
		Role->PlayerTempData->ChangeWorld.RetZoneID = RoleDataEx::G_ZoneID;
		Role->PlayerTempData->ChangeWorld.RetPos = *(Role->Pos());
	}

	Role->PlayerTempData->LuaClock = RetRole->PlayerTempData->LuaClock;

	if( Role->PlayerTempData->ChangeWorld.RetZoneID == RoleDataEx::G_ZoneID )
	{
		*(Role->Pos()) = Role->PlayerTempData->ChangeWorld.RetPos;
		Role->TempData.BackInfo.LZoneID = -1;
//		Role->ZoneID( Role->SelfData.RevZoneID );
		SendPlayerItemID( Role );
		SendZoneInfo( Role );
		SendChangeZoneOK( Role ); 
		//Global::CheckClientLoading_AddToPartition( Role->GUID() , -1 ); 
		NetSrv_Move::S_ClientLoading( Role->GUID() , Role->Pos()->X , Role->Pos()->Y , Role->Pos()->Z );
		NetSrv_Attack::S_ZonePKFlag( Role->GUID() , RoleDataEx::G_PKType );
		if( ClientSkyType != EM_ClientSkyProcType_Normal )
			SC_SkyProcType( Role->GUID() , ClientSkyType );

		LUA_VMClass::PCallByStrArg( "event_module(2)" , Role->GUID() ,Role->GUID() );

		//////////////////////////////////////////////////////////////////////////
		//劇情時鐘處理
		if( Role->PlayerTempData->LuaClock.Type != EM_LuaPlotClockType_Close )
		{
			RoleObj->PlotVM()->PCall( Role->PlayerTempData->LuaClock.ChangeZonePlot.Begin() , Role->GUID() , Role->GUID() );
		}
	}
	else
	{
		Global::ChangeZone( Role->GUID() , Role->PlayerTempData->ChangeWorld.RetZoneID , 0 , Role->PlayerTempData->ChangeWorld.RetPos.X , Role->PlayerTempData->ChangeWorld.RetPos.Y , Role->PlayerTempData->ChangeWorld.RetPos.Z );
	}

	Role->TempData.Attr.Action.ChangeZone = false;



	SendPlayerDataInfo_ZIP( Role );
	NetSrv_Shop::S_FixSellItemBackup( Role );
}

void NetSrv_CliConnectChild::RL_DelTomb( int TombGUID , int OwnerDBID )
{
	RoleDataEx* TombRole = Global::GetRoleDataByGUID( TombGUID );

	if( TombRole != NULL && TombRole->TempData.Sys.OwnerDBID == OwnerDBID )
	{
		TombRole->LiveTime( 0 , "RL_DelTomb" );
	}
}

void NetSrv_CliConnectChild::SetClientSkyProcType( ClientSkyProcTypeENUM Type )
{
	if( ClientSkyType == Type )
		return;

	ClientSkyType = Type;

	//把所有有關此物件的寵物換區
	static set<BaseItemObject* >& PlayerObjSet = *(BaseItemObject::PlayerObjSet());

	set<BaseItemObject* >::iterator Iter;

	for( Iter = PlayerObjSet.begin() ; Iter != PlayerObjSet.end() ; Iter++ )
	{
		BaseItemObject* Obj = *Iter;
		if( Obj == NULL )
			continue;
		SC_SkyProcType( Obj->Role()->GUID() , Type );
	}
}

void NetSrv_CliConnectChild::OnLogin( )
{
	//_Net->RegGSrvNetID( EM_SERVER_TYPE_LOCAL , RoleDataEx::G_ZoneID );

	//NetSrv_Other::SC_AllPlayer_ZoneOpen();
}

void NetSrv_CliConnectChild::RC_WorldReturnOK( BaseItemObject* Player  )  
{
	//如果不是戰場server則不處理
	if( Global::Ini()->IsBattleWorld == false )
		return;

	RoleDataEx* Role = Player->Role();

	if( Global::Ini()->BG_BG_LeaveScript.size() )
		LUA_VMClass::PCall( Global::Ini()->BG_BG_LeaveScript.c_str() , Role->GUID() , Role->GUID() );

	NetSrv_CliConnect::SBL_RetNomorlWorld( Role->PlayerTempData->ChangeWorld.WorldID , Role->PlayerTempData->ChangeWorld.WorldNetID , Role->DBID() , Role , Player->OrgWorldGUID());
	Role->PlayerTempData->ChangeWorld.WorldID = -1;
	Role->PlayerTempData->ChangeWorld.WorldNetID = -1;
	Role->Destroy( "ReturnWorld" );
}

void NetSrv_CliConnectChild::RBL_SendToPlayerByDBID( int FromWorldId , int FromNetID , int PlayerDBID , int DataSize , char* Data )
{
	Global::SendToCli_ByDBID( PlayerDBID , DataSize , Data );
}

void NetSrv_CliConnectChild::WorldReturnNotifyProc( int GItemID )
{
	RoleDataEx* Player = Global::GetRoleDataByGUID( GItemID );
	if( Player == NULL )
		return;

	if( Global::Ini()->IsBattleWorld == false )
		return;


	//把所有寵物都移到自己的位置
	vector< BaseItemObject* >& ObjList = *BaseItemObject::GetZoneObjList();
	for( int i = 0 ; i < (int)ObjList.size() ; i++  )
	{
		BaseItemObject* PetObj = ObjList[i];
		if( PetObj == NULL )
			continue;
		RoleDataEx* Pet = PetObj->Role();

		if( Pet->TempData.Sys.OwnerDBID == Player->DBID() && Player->DBID() != -1 )
		{
			PetObj->Destroy( "return world destroy" );
			if( Pet->IsPet( Player ) != false )
				NetSrv_Magic::S_DeletePet( Player->GUID() , Pet->GUID() , Pet->TempData.SummonPet.Type );
		}
	}

	Global::DelFromPartition( Player->GUID() );
	SC_WorldReturnNotify( GItemID );
}

void NetSrv_CliConnectChild::OnLocalConnect( int ZoneID )
{
	AllZoneSet.insert( ZoneID );
	SC_AllCli_ZoneOpenState( ZoneID , true );
	NetSrv_Other::SC_AllCli_ZoneOpen( ZoneID );
}
void NetSrv_CliConnectChild::OnLocalDisconnect( int ZoneID )
{
	//AllZoneSet.insert( ZoneID );
	AllZoneSet.erase( ZoneID );
	SC_AllCli_ZoneOpenState( ZoneID , false );
}

void NetSrv_CliConnectChild::RBL_BattleZone( int FromWorldId , int ZoneID , int FromNetID , int PlayerDBID )
{
	RoleDataEx* Player = Global::GetRoleDataByDBID( PlayerDBID );
	if( Player == NULL )
		return;

	Player->PlayerTempData->ChangeWorld.WorldID = FromWorldId;
	Player->PlayerTempData->ChangeWorld.RetZoneID = ZoneID;
	Player->PlayerTempData->ChangeWorld.WorldNetID = FromNetID;
}
void NetSrv_CliConnectChild::RL_ReturnWorldRequest( int FromWorldId , int FromNetID , int PlayerDBID , int TestCount )
{
	PlayerDBID += FromWorldId * _DEF_MAX_DBID_COUNT_; 	
	BaseItemObject* PlayerObj =  BaseItemObject::GetObjByDBID( PlayerDBID );
	if( PlayerObj == NULL )
	{
		SL_ReturnWorldResult( FromWorldId , FromNetID , PlayerDBID , TestCount , false );
		return;
	}

	RC_WorldReturnOK( PlayerObj );
	SL_ReturnWorldResult( FromWorldId , FromNetID , PlayerDBID , TestCount , true);
}
void NetSrv_CliConnectChild::RBL_ReturnWorldResult( int FromWorldId , int PlayerDBID , int TestCount , bool Result )
{
	if( Result == true )
		return;

	if( TestCount == 0 )
	{
		RoleDataEx* Player = Global::GetRoleDataByDBID( PlayerDBID );
		if( Player )
		{
			ChangeWorldStruct& info = Player->PlayerTempData->ChangeWorld;
			SBL_ReturnWorldRequest( info.WorldID , info.RetZoneID , PlayerDBID , 1 );
		}
	}
}