/*
#include "../NetWakerGSrvInc.h"
#include "NetSrv_GuildHousesWarChild.h"
#include "../../mainproc/GuildHouseManage/GuildHousesClass.h"
#include "../../mainproc/GuildHouseWarManage/GuildHouseWarManage.h"
//--------------------------------------------------------------------------------------------------------------
bool    NetSrv_GuildHousesWarChild::Init()
{
	NetSrv_GuildHousesWar::_Init();

	if( This != NULL)
		return false;

	This = NEW( NetSrv_GuildHousesWarChild );

	return true;
}
//--------------------------------------------------------------------------------------------------------------
bool    NetSrv_GuildHousesWarChild::Release()
{
	if( This == NULL )
		return false;

	NetSrv_GuildHousesWar::_Release();

	delete This;
	This = NULL;

	return true;

}

void NetSrv_GuildHousesWarChild::RC_OpenMenu( BaseItemObject* Obj )
{
	SD_OpenMenu( Obj->Role()->DBID() );
}

void NetSrv_GuildHousesWarChild::RD_WarBegin( int GuildID[2] )
{
	if( Global::Ini()->IsGuildHouseWarZone == false )
		return;
	
	//通知所有的Client
	SC_AllPlayer_WarBegin( GuildID );

	GuildHouseWarFightStruct TempInfo;
	TempInfo.Base[0].GuildID = GuildID[0];
	TempInfo.Base[1].GuildID = GuildID[1];

//	GuildHouseWarManageClass::CreateHouse( &TempInfo );
	NEW GuildHouseWarManageClass( &TempInfo );

	SD_LoadHouseBaseInfo( );
}

void NetSrv_GuildHousesWarChild::RD_WarEnd( )
{
	if( Global::Ini()->IsGuildHouseWarZone == false )
		return;

	Global::Schedular()->Push( OnEvent_ClearGuildWar , 60*1000*10 , NULL, NULL );

	//////////////////////////////////////////////////////////////////////////
	//計算分數
	GuildHouseWarManageClass::WarEndProc_All();
	//////////////////////////////////////////////////////////////////////////
	SD_WarEndOK();
}

int NetSrv_GuildHousesWarChild::OnEvent_ClearGuildWar	 ( SchedularInfo* Obj, void* InputClass )
{
	GuildHouseWarManageClass::ClearAll();
	GuildHousesManageClass::ClearAll();

	vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;

	for( unsigned i = 1 ; i < RoomList.size() ; i++ )
	{
		if( RoomList[i].IsActive != false )
		{
			RoomList[i].IsActive = false;
			DelRoomMonster( i );
		}		
	}

	//把所有某區 房間的玩家傳出，清除此方間的怪物
	BaseItemObject* PlayerObj;
	for( PlayerObj = BaseItemObject::GetByOrder_Player(true) ; PlayerObj != NULL ; PlayerObj = BaseItemObject::GetByOrder_Player() )
	{
		RoleDataEx* Role = PlayerObj->Role();
		Global::ChangeZone( Role->GUID() 
			, Role->SelfData.ReturnZoneID , -1
			, Role->SelfData.ReturnPos.X
			, Role->SelfData.ReturnPos.Y
			, Role->SelfData.ReturnPos.Z	
			, Role->SelfData.ReturnPos.Dir		);	
	}
	return 0;			
}

void NetSrv_GuildHousesWarChild::RC_WarRegister( BaseItemObject* Obj , GuildHouseWarRegisterTypeENUM Type )
{
	RoleDataEx* Role = Obj->Role();
	SD_WarRegister( Role->DBID() , Type );
}

void NetSrv_GuildHousesWarChild::RC_EnterWar( BaseItemObject* Obj )
{
	if( Global::Ini()->IsGuildHouseWarZone != false )
		return;

	RoleDataEx* Owner = Obj->Role();

	int ZoneID = RoleDataEx::G_ZoneID % _DEF_ZONE_BASE_COUNT_;

	if( ZoneID >= 100 )
	{
		Owner->SelfData.ReturnZoneID = Global::Ini()->RevZoneID;
		Owner->SelfData.ReturnPos.X = float( Global::Ini()->RevX );
		Owner->SelfData.ReturnPos.Y = float( Global::Ini()->RevY );
		Owner->SelfData.ReturnPos.Z = float( Global::Ini()->RevZ );	
	}
	else
	{
		Owner->SelfData.ReturnZoneID = ZoneID;
		Owner->SelfData.ReturnPos.X = Owner->X();
		Owner->SelfData.ReturnPos.Y = Owner->Y();
		Owner->SelfData.ReturnPos.Z = Owner->Z();
	}
	Owner->SelfData.ReturnPos.Dir = Owner->Dir();

	Global::ChangeZone( Owner->GUID() , g_ObjectData->GetSysKeyValue( "GuildHouseWar_ZoneID" ) , -1 );	
}

void NetSrv_GuildHousesWarChild::RC_LeaveWar( BaseItemObject* Obj )
{
	if( Global::Ini()->IsGuildHouseWarZone == false )
		return;

	RoleDataEx* Owner = Obj->Role();

	Global::ChangeZone( Owner->GUID() 
		, Owner->SelfData.ReturnZoneID , -1
		, Owner->SelfData.ReturnPos.X
		, Owner->SelfData.ReturnPos.Y
		, Owner->SelfData.ReturnPos.Z	
		, Owner->SelfData.ReturnPos.Dir		);
}

void NetSrv_GuildHousesWarChild::RD_HouseBaseInfo		( GuildHousesInfoStruct& HouseBaseInfo )
{
	GuildHousesManageClass* house = GuildHousesManageClass::GetHouseObj( HouseBaseInfo.GuildID );

	if( house == NULL )
		return;

	house->SetHouseBase( HouseBaseInfo );

}
void NetSrv_GuildHousesWarChild::RD_BuildingInfo		( GuildHouseBuildingInfoStruct& Building )
{
	GuildHousesManageClass* house = GuildHousesManageClass::GetHouseObj( Building.GuildID );

	if( house == NULL )
		return;

	house->AddBuilding( Building );
}
void NetSrv_GuildHousesWarChild::RD_FurnitureItemInfo	( GuildHouseFurnitureItemStruct& Item )
{
	GuildHousesManageClass* house = GuildHousesManageClass::GetHouseObj( Item.GuildID );

	if( house == NULL )
		return;

	house->AddFurniture( Item );
}
void NetSrv_GuildHousesWarChild::RD_HouseInfoLoadOK		( int GuildID )
{
	GuildHousesManageClass* house = GuildHousesManageClass::GetHouseObj( GuildID );

	if( house == NULL )
		return;

	house->LoadOK();
}
void NetSrv_GuildHousesWarChild::RC_PricesRequst	( BaseItemObject* Obj )
{

}
void NetSrv_GuildHousesWarChild::RC_HistoryRequest		( BaseItemObject* Obj , int DayBefore )
{
	SD_HistoryRequest	( Obj->Role()->DBID() , DayBefore );
}
void NetSrv_GuildHousesWarChild::RC_PlayerScoreRequest( BaseItemObject* Obj )
{
	RoleDataEx* Role = Obj->Role();
	GuildHouseWarManageClass* WarInfo = GuildHouseWarManageClass::GetGuildWar_ByRoomID( Role->RoomID() );
	if( WarInfo == NULL )
		return;

	map< int , GuildWarPlayerInfoStruct >::iterator Iter;
	vector< GuildWarPlayerInfoStruct* > List;

	for( Iter = WarInfo->PlayerScoreMap().begin() ; Iter != WarInfo->PlayerScoreMap().end() ; Iter++ )
	{
		if(		Iter->second.GuildID == WarInfo->WarBase().Base[0].GuildID 
			||	Iter->second.GuildID == WarInfo->WarBase().Base[1].GuildID )
		{
			List.push_back( &(Iter->second) );
		}

	}

	SC_PlayerScore( Role->GUID() , List );
}
*/