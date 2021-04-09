#include "GuildHousesClass.h"
#include "../Global.h"
#include "../pathmanage/PathManage.h"
#include "../../netwalker_member/NetWakerGSrvInc.h"

#include "Rune Engine/Rune/Rune.h"
#include "Rune Engine/Rune/Rune Engine NULL.h"
#include "Rune Engine/Rune/Fusion/RuFusion_Trekker.h"


#define _DEF_GUILDHOUSES_LIFETIME_	(60*10*1000)
//////////////////////////////////////////////////////////////////////////
map< int , GuildHousesManageClass* >	GuildHousesManageClass::_GuildIDMap;
GroudPointStruct						GuildHousesManageClass::_Map[ _MAX_MAP_POINT_COUNT_ ][ _MAX_MAP_POINT_COUNT_ ];
CreateDBCmdClass<GuildHouseBuildingInfoStruct>*		GuildHousesManageClass::_RDGuildHouseBuildingSql;
CreateDBCmdClass<GuildHouseItemStruct>*				GuildHousesManageClass::_RDGuildHouseItemSql;
CreateDBCmdClass<GuildHouseFurnitureItemStruct>*	GuildHousesManageClass::_RDGuildHouseFurnitureItemSql;
//////////////////////////////////////////////////////////////////////////
bool GuildHousesManageClass::Init()
{	
	GroudPointStruct Temp;
//	float			Height[4];

	for( int i = 0 ; i < 100 ; i ++ )
	{
		for( int j = 0 ; j < 100 ; j++ )
		{
			Temp.X = float( i*20 );
			Temp.Z = float( j*20 );
/*
			PathManageClass::Height( Temp.X-5 , 0 , Temp.Z-5 , Height[0] , 100 );
			PathManageClass::Height( Temp.X+5 , 0 , Temp.Z-5 , Height[1] , 100 );
			PathManageClass::Height( Temp.X-5 , 0 , Temp.Z+5 , Height[2] , 100 );
			PathManageClass::Height( Temp.X+5 , 0 , Temp.Z+5 , Height[3] , 100 );

			if(		abs( Height[0] - Height[1] ) > 5
				||	abs( Height[0] - Height[2] ) > 5
				||	abs( Height[0] - Height[3] ) > 5 )
				Temp.IsBuildOK = false;
			Temp.Y = Height[0];
				*/
			Temp.IsBuildOK = true;
			Temp.Y = 0;

			_Map[ i ][ j ] = Temp;
		}
	}

	_RDGuildHouseBuildingSql		= NEW CreateDBCmdClass<GuildHouseBuildingInfoStruct> ( RoleDataEx::ReaderRD_GuildHouseBuildingInfo()  , "GuildHouse_Building" );
	_RDGuildHouseItemSql			= NEW CreateDBCmdClass<GuildHouseItemStruct> ( RoleDataEx::ReaderRD_GuildHouseItem()  , "GuildHouse_Item" );
	_RDGuildHouseFurnitureItemSql	= NEW CreateDBCmdClass<GuildHouseFurnitureItemStruct> ( RoleDataEx::ReaderRD_GuildHouseFurnitureItem()  , "GuildHouse_FurnitureItem" );
	return true;
}

bool GuildHousesManageClass::GetObjPoint( int GItemID , const char* Point , float& outX , float& outY , float& outZ , float& outDir )
{
	CRuMatrix4x4 	matLocal;
	CRuMatrix4x4 	matWorld;

	RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
	if( Role == NULL )
		return false;

	GameObjDbStructEx* RoleObjDB = Global::GetObjDB( Role->BaseData.ItemInfo.OrgObjID );
	if( RoleObjDB  == NULL )
		return false;

	GameObjDbStructEx* RoleImageDB = Global::GetObjDB( RoleObjDB->ImageID ); 
	if( RoleImageDB == NULL )
		return false;

	CRuEntity* RoleEntiry = g_ruResourceManager->LoadEntity( RoleImageDB->Image.ACTWorld );

	if( RoleEntiry == NULL )
		return false;

	CRuEntity* LinkEntiry = RuEntity_FindLinkFrame( RoleEntiry , Point , false );

	if( LinkEntiry == NULL )
		return false;

	CRuVector3	RuPoint = LinkEntiry->GetTranslation();

	matLocal.SetToTranslation( RuPoint );
	const CRuQuaternion& Quaternion = LinkEntiry->GetRotation( ruFRAMETYPE_LOCAL );
	matLocal = Quaternion.GetMatrix() * matLocal;



	CRuMatrix4x4 matRotation;
	matRotation.SetToRotationEulerXYZ( 0, (Role->Dir()-90)*PI/180.0f, 0 );
	matLocal = matLocal * matRotation;



	CRuMatrix4x4 matMove;
	matMove.SetToTranslation( Role->X() , Role->Y()  , Role->Z() );
	matWorld = matLocal * matMove;

	//matLocal
	outX = matWorld._41;
	outY = matWorld._42;
	outZ = matWorld._43;

	CRuVector3 vecDir( 0 , 0 , -1 );
	vecDir = vecDir * matWorld;
	vecDir.x -=outX;
	vecDir.y -=outY;
	vecDir.z -=outZ;

	outDir = RoleDataEx::CalDir( vecDir.x , vecDir.z) /*+ 90*/;

	//
	ruSAFE_RELEASE(RoleEntiry);

	return true;
}


void GuildHousesManageClass::ClearAll()
{
//static map< int , GuildHousesManageClass* > _GuildIDMap;
//	map< int , GuildHousesManageClass* >::iterator Iter;
	/*
	for( Iter = _GuildIDMap.begin() ; Iter != _GuildIDMap.end() ; Iter++ )
	{	
		delete Iter->second;
	}
*/
	while( _GuildIDMap.size() != 0 )
	{
		delete _GuildIDMap.begin()->second;
	}

}

bool GuildHousesManageClass::Release()
{
	delete _RDGuildHouseBuildingSql;
	delete _RDGuildHouseItemSql;
	delete _RDGuildHouseFurnitureItemSql;
	return true;
}

void  GuildHousesManageClass::CalRentAll()
{
	map< int , GuildHousesManageClass* >::iterator Iter;

	for( Iter = _GuildIDMap.begin() ; Iter != _GuildIDMap.end() ; Iter++ )
	{
		Iter->second->CalRent();
	}
}

void		GuildHousesManageClass::CalRent( )
{

	//////////////////////////////////////////////////////////////////////////

	int	DTime = RoleDataEx::G_Now - _HouseBase.LastCalRentTime;

	int DHour = DTime/60/60;
	if( DHour <= 0 )
		return;

	_HouseBase.LastCalRentTime = RoleDataEx::G_Now - DTime % (60*60);

	char Buf[512];
	sprintf( Buf , "UPDATE GuildHouse_Base Set LastCalRentTime = %d where GuildID=%d" , _HouseBase.LastCalRentTime , _HouseBase.GuildID );
	Net_DCBase::SqlCommand( _HouseBase.GuildID , Buf );

	_IsNeedSave = true;

	int		Cost = 0;
	map< int , GuildHouseBuildingStruct >::iterator Iter;

	for( Iter = _BuildingMap.begin() ; Iter != _BuildingMap.end() ; Iter++ )
	{
		if( Iter->second.Info.IsActive == false )
			continue;

		GuildBuildingInfoTableStruct* BuildingTableInfo = g_ObjectData->GuildBuilding( Iter->second.Info.BuildingID );
		if( BuildingTableInfo == NULL )
			continue;

		Cost += BuildingTableInfo->MaintenanceCharge / 96;
	}

	if( Cost == 0 )
		return;
	
	Cost = Cost * DHour ;

	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( _HouseBase.GuildID );
	if( GuildInfo == NULL )
		return; 

	GuildResourceStruct& Resource = GuildInfo->Base.Resource;
	
	if( Resource.Gold < Cost)
	{
		int ActiveBuffID = g_ObjectData->GetSysKeyValue( "GuildHouseActiveBuff" );

		for( Iter = _BuildingMap.begin() ; Iter != _BuildingMap.end() ; Iter++ )
		{
			Iter->second.Info.IsActive = false;

			RoleDataEx* BuildRole = Global::GetRoleDataByGUID(  Iter->second.ObjGUID );
			if( BuildRole != NULL )
			{
				BuildRole->ClearBuff( ActiveBuffID , -1 );
			}
		}
		return;
	}
	Resource.Gold += Cost;

	GuildResourceStruct NeedResource;
	NeedResource.Init();
	NeedResource.Gold = Cost;
	NetSrv_Guild::SD_AddGuildResource( GuildInfo->Base.GuildID , GuildInfo->Base.LeaderDBID , NeedResource , "" , EM_AddGuildResourceType_Tax , 0 );
}

void	GuildHousesManageClass::OnTimeProcAll( )
{
	static int St_Count = 0;
	St_Count++;
	//計算維持廢
	//if( St_Count >= 6*60 )
	{
		St_Count = 0;
		CalRentAll(); 
	}


	vector< int > roomPlayerCountList;
	vector< GuildHousesManageClass* > delList;

	for( int i = 0 ; i < Global::Ini()->RoomCount ; i++ )
	{
		roomPlayerCountList.push_back( 0 );
	}

	set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//計算每個房間的人數
	for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if( Obj == NULL )
			continue;
		//第0 個房間不處理
		if( Obj->Role()->RoomID() == 0 || Obj->Role()->RoomID() == -1)
			continue;

		if( roomPlayerCountList.size() <= (unsigned)Obj->Role()->RoomID() )
			continue;

		//計算每個房間的人數
		roomPlayerCountList[ Obj->Role()->RoomID() ]++;

	}

	map< int , GuildHousesManageClass* >::iterator Iter;

	for( Iter = _GuildIDMap.begin() ; Iter != _GuildIDMap.end() ; Iter++ )
	{
		GuildHousesManageClass* houseClass = Iter->second;

		int roomID = houseClass->RoomID();

		if( houseClass->_State == EM_GuildHouseObjectState_OK )
		{			
			if( houseClass->_IsNeedSave != false 				
				||	houseClass->_LiveTime < (int)RoleDataEx::G_SysTime  )
			{
				houseClass->SaveProc();
				continue;
			}
		}

		if( (int)roomPlayerCountList.size() > roomID && roomPlayerCountList[ roomID ] > 0  )
		{
			houseClass->_LiveTime = RoleDataEx::G_SysTime + _DEF_GUILDHOUSES_LIFETIME_;
			continue;
		}
		if( (int)RoleDataEx::G_SysTime < houseClass->_LiveTime )
			continue;

		delList.push_back( houseClass );	

	}

	for( int i = 0 ; i < delList.size() ; i++ )
	{
		delete delList[i];
	}

}

void	GuildHousesManageClass::SaveProc( )
{
	char	Buf[1024];
	if( _IsNeedSave == false  )
		return;

	if( Global::Net_ServerList->CheckServerExist( EM_SERVER_TYPE_DATACENTER ) == false )
		return;

//	_SaveTime = RoleDataEx::G_SysTime + 5*60*1000;
	_IsNeedSave = false;

	if(  RoleDataEx::G_ZoneID != 401 )
		return;



	if( _HouseBase.ItemMaxPageCount <= 0 || _HouseBase.ItemMaxPageCount > 10 )
	{
		Print( LV3 , "SaveProc" ,  "data error _HouseBase.ItemMaxPageCount = %d" , _HouseBase.ItemMaxPageCount );
		_HouseBase.ItemMaxPageCount = 1;		
	}
	if( _HouseBase.FurnitureMaxCount != 100 )
	{
		Print( LV3 , "SaveProc" ,  "data error _HouseBase.FurnitureMaxCount = %d" , _HouseBase.FurnitureMaxCount );
		_HouseBase.FurnitureMaxCount = 100;
	}
		

	//把公會資料重新儲存一次
	sprintf_s( Buf , sizeof(Buf) , "UPDATE GuildHouse_Base Set ItemMaxPageCount=%d , FurnitureMaxCount=%d,GuildStyle=%d,GuildStyleRight=%d WHERE GuildID = %d" 
		, _HouseBase.ItemMaxPageCount , _HouseBase.FurnitureMaxCount 
		, _HouseBase.GuildStyle , _HouseBase.GuildStyleRight
		, _HouseBase.GuildID );
	Net_DCBase::SqlCommand( rand() , Buf );

	//把刪除的建築物刪除
	for( unsigned i = 0 ; i < _DelBuildingDBID.size() ; i++ )
	{
		sprintf_s( Buf , sizeof(Buf) , "DELETE GuildHouse_Building WHERE GuildID=%d and BuildingDBID=%d" , _HouseBase.GuildID , _DelBuildingDBID[i] );
		Net_DCBase::SqlCommand( rand() , Buf );
	}
	_DelBuildingDBID.clear();

	map< int , GuildHouseBuildingStruct >::iterator Iter;
	for( Iter = _BuildingMap.begin() ; Iter != _BuildingMap.end() ; Iter++ )
	{
		switch( Iter->second.Info.State )
		{
		case EM_GuildHouseBuildingInfoState_Insert:
			{
				Net_DCBase::SqlCommand(  _HouseBase.GuildID , _RDGuildHouseBuildingSql->GetInsertStr( &Iter->second.Info ).c_str() );	
			}break;
		case EM_GuildHouseBuildingInfoState_Update:
			{
				sprintf_s( Buf , sizeof(Buf) , "WHERE GuildID = %d and BuildingDBID=%d" , _HouseBase.GuildID , Iter->second.Info.BuildingDBID );
				Net_DCBase::SqlCommand(  _HouseBase.GuildID , _RDGuildHouseBuildingSql->GetUpDateStr( &Iter->second.Info , Buf ).c_str() );	
			}break;
		}
		Iter->second.Info.State = EM_GuildHouseBuildingInfoState_None;
	}

	for( unsigned i = 0 ; i < _DelItemDBID.size() ; i++ )
	{
		sprintf_s( Buf , sizeof(Buf) , "DELETE GuildHouse_Item WHERE GuildID=%d and ItemDBID=%d" , _HouseBase.GuildID , _DelItemDBID[i] );
		Net_DCBase::SqlCommand( _HouseBase.GuildID , Buf );
	}
	_DelItemDBID.clear();

	for( unsigned i = 0 ; i  < _DEF_GUIDHOUSEITEM_MAX_PAGE_  ; i++ )
	{
		map< int , GuildHouseItemStruct >::iterator ItemIter;
		for( ItemIter = _ItemBox[i].begin() ; ItemIter != _ItemBox[i].end() ; ItemIter++ )
		{
			switch( ItemIter->second.State )
			{
			case EM_GuildHouseItemInfoState_Insert:
				Net_DCBase::SqlCommand(  _HouseBase.GuildID , _RDGuildHouseItemSql->GetInsertStr( &ItemIter->second ).c_str() );	
				break;
			case EM_GuildHouseItemInfoState_Update:
				sprintf_s( Buf , sizeof(Buf) , "WHERE GuildID = %d and ItemDBID=%d" , _HouseBase.GuildID , ItemIter->second.ItemDBID );
				Net_DCBase::SqlCommand(  _HouseBase.GuildID , _RDGuildHouseItemSql->GetUpDateStr( &ItemIter->second , Buf ).c_str() );	
				break;
			}
			ItemIter->second.State = EM_GuildHouseItemInfoState_None;
		}
	}

	for( unsigned i = 0 ; i  < _DelFurnitureDBID.size()  ; i++ )
	{
		sprintf_s( Buf , sizeof(Buf) , "DELETE GuildHouse_FurnitureItem WHERE GuildID=%d and ItemDBID=%d" , _HouseBase.GuildID , _DelFurnitureDBID[i] );
		Net_DCBase::SqlCommand( _HouseBase.GuildID , Buf );
	}
	_DelFurnitureDBID.clear();

	map< int , GuildHouseFurnitureItemStruct >::iterator FuriIter;
	for( FuriIter = _FurnitureItemMap.begin() ; FuriIter != _FurnitureItemMap.end() ; FuriIter++ )
	{
		switch( FuriIter->second.State )
		{
		case EM_GuildHouseItemInfoState_Insert:
			{
				Net_DCBase::SqlCommand(  _HouseBase.GuildID , _RDGuildHouseFurnitureItemSql->GetInsertStr( &FuriIter->second ).c_str() );	
			}break;
		case EM_GuildHouseItemInfoState_Update: 
			{
				sprintf_s( Buf , sizeof( Buf ) , "WHERE GuildID = %d and ItemDBID=%d" , _HouseBase.GuildID , FuriIter->second.ItemDBID );
				Net_DCBase::SqlCommand(  _HouseBase.GuildID , _RDGuildHouseFurnitureItemSql->GetUpDateStr( &FuriIter->second , Buf ).c_str() );	
			}break;
		}
		FuriIter->second.State = EM_GuildHouseItemInfoState_None;
	}
}

GuildHousesManageClass* GuildHousesManageClass::GetHouseObj_ByRoom( int RoomID )
{
	map< int , GuildHousesManageClass* >::iterator Iter;
	for( Iter = _GuildIDMap.begin() ; Iter != _GuildIDMap.end() ; Iter++ )
	{
		if( Iter->second->RoomID() == RoomID )
			return Iter->second;
	}

	return NULL;
}

GuildHousesManageClass* GuildHousesManageClass::GetHouseObj( int GuildID )
{
	map< int , GuildHousesManageClass* >::iterator Iter;
	Iter = _GuildIDMap.find( GuildID );
	if( Iter == _GuildIDMap.end() )
		return NULL;
	return Iter->second;
}

GuildHousesManageClass* GuildHousesManageClass::CreateHouse( int GuildID , int RoomID )
{
	Print( LV2 , "GuildHousesManageClass::CreateHouse" , "GuildID=%d RoomID=%d" , GuildID , RoomID );
	GuildHousesManageClass* houseClass = GetHouseObj( GuildID );
	if( houseClass != NULL )
	{
		Print( LV4 , "GuildHousesManageClass::CreateHouse" , "GuildID=%d RoomID=%d houseClass != NULL" , GuildID , RoomID );
		return NULL;
	}

	houseClass = NEW(GuildHousesManageClass);
	houseClass->_State = EM_GuildHouseObjectState_Loading;
	houseClass->_HouseBase.GuildID = GuildID;
	houseClass->_RoomID = RoomID;	

	_GuildIDMap[ GuildID ] = houseClass;

	Global::St()->PrivateRoomInfoList[ RoomID ].IsActive = true;

	return houseClass;
}


GuildHousesManageClass::GuildHousesManageClass()
{
	_State = EM_GuildHouseObjectState_None;
	_RoomID = -1;
	_SaveTime = RoleDataEx::G_SysTime	 + _DEF_GUILDHOUSES_LIFETIME_;
	_IsNeedSave = false;
	_MaxBuildingDBID = 1;
	_MaxItemDBID = 1;
	_MaxFurnitureDBID = 1;
	_X = _Y = _Z = _Dir = 0;
	_CampID = EM_CampID_Visitor;

	memset( _BuildingMask , 0 , sizeof(_BuildingMask) );
}

GuildHousesManageClass::~GuildHousesManageClass()
{
	_IsNeedSave = true;
	SaveProc( );
	_GuildIDMap.erase( _HouseBase.GuildID );
}

bool	GuildHousesManageClass::SetHouseBase( GuildHousesInfoStruct& Info )
{
	if( Info.GuildID != _HouseBase.GuildID )
		return false;

	_HouseBase = Info;

	return true;
}

void	GuildHousesManageClass::LoadOK()
{
	_State = EM_GuildHouseObjectState_OK;
}

//建立房屋物件
int	GuildHousesManageClass::CreateBuildingObj( int OrgObjID , int BuildingDBID , float X , float Y , float Z , float Dir , ObjectModeStruct Mode , int BuildSize , bool IsActive )
{

	int BuildingGUID = Global::CreateObj( OrgObjID , X , Y , Z , Dir , 1 );

	BaseItemObject* BuildingObj = Global::GetObj( BuildingGUID );
	if( BuildingObj == NULL )
	{
		Print( LV1 , "CreateBuildingObj" ,"Building Object(%d) Create Error!!" , OrgObjID );
		return 0;
	}
	GameObjDbStructEx* NpcObjDB = Global::GetObjDB( OrgObjID );
	if( NpcObjDB == NULL )
		return 0;

	RoleDataEx* BuildingRole  = BuildingObj->Role();

//	if( _CampID != EM_CampID_Unknow )
	BuildingRole->TempData.AI.CampID = _CampID;

	BuildingRole->BaseData.Mode._Mode = Mode._Mode;
	BuildingRole->BaseData.Mode.DisableRotate	  = ( NpcObjDB->NPC.AutoRotate == false);
	BuildingRole->BaseData.Mode.HideName = true;

	BuildingRole->TempData.GuildBuildingDBID = BuildingDBID;
	BuildingRole->BaseData.GuildID = _HouseBase.GuildID;
	BuildingRole->TempData.GuildBuildingType = EM_GuildBuildingType_Building;



	Global::AddToPartition( BuildingGUID , RoomID() );

	GameObjDbStructEx* BuildingObjDB = Global::GetObjDB( OrgObjID );

	if( BuildingObjDB->IsNPC() || BuildingObjDB->IsQuestNPC() ) 
	{
		if( strlen( BuildingObjDB->NPC.szLuaInitScript ) != 0 )
		{
			BuildingObj->PlotVM()->CallLuaFunc( BuildingObjDB->NPC.szLuaInitScript , BuildingRole->GUID() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//把所有旁邊的玩家傳到入口處
	//////////////////////////////////////////////////////////////////////////
	{
		float X		= (float)g_ObjectData->GetSysKeyValue( "GuildHouse_X" );
		float Y		= (float)g_ObjectData->GetSysKeyValue( "GuildHouse_Y" );
		float Z		= (float)g_ObjectData->GetSysKeyValue( "GuildHouse_Z" );
		float Dir	= (float)g_ObjectData->GetSysKeyValue( "GuildHouse_Dir" );

		RoleDataEx*	Other;
		Global::ResetRange( BuildingRole , _Def_View_Block_RangeAttack_ );
		while( 1 ) 
		{
			Other = Global::GetRangePlayer();
			if( Other == NULL  )
				break;

			if( Other->Length( BuildingRole ) >= BuildSize )
				continue;
			Other->Net_GameMsgEvent( EM_GameMessageEvent_GuildHouse_TooNearBuilding );
			
			Global::CheckClientLoading_SetPos( Other->GUID() , X , Y , Z , Dir );
		}

	}
	//////////////////////////////////////////////////////////////////////////	
	//設定顯示的Buff
	if( IsActive )
	{
		int ActiveBuffID = g_ObjectData->GetSysKeyValue( "GuildHouseActiveBuff" );
		BuffBaseStruct* RetBuf = BuildingRole->AssistMagic( BuildingRole , ActiveBuffID , 0 , false , -1 );
		if( RetBuf != NULL )
			NetSrv_MagicChild::SendRange_AddBuffInfo( BuildingRole->GUID() , BuildingRole->GUID() ,  ActiveBuffID , RetBuf->Power , RetBuf->Time );
	}
	//////////////////////////////////////////////////////////////////////////
	//載入

	return BuildingGUID;
}

bool	GuildHousesManageClass::RebuildAllBuilding( )
{
	map< int , GuildHouseBuildingStruct >::iterator Iter;
	for( Iter = _BuildingMap.begin() ; Iter != _BuildingMap.end() ; Iter++ )
	{
		GuildHouseBuildingStruct& info = Iter->second;
		GuildBuildingInfoTableStruct* BuildingTableInfo = g_ObjectData->GuildBuilding( info.Info.BuildingID );
		if( BuildingTableInfo == NULL )
			continue;

		int ObjID = BuildingTableInfo->OrgObjID;
		if( _HouseBase.GuildStyle )
		{
			ObjID = g_ObjectData->GetGuildHouseReplaceNpc( _HouseBase.GuildStyle , ObjID );
		}
		
		RoleDataEx* BuildingRole = Global::GetRoleDataByGUID( info.ObjGUID );
		if( BuildingRole == NULL )
			continue;
//		if( BuildingRole->OrgObjID() == ObjID )
//			continue;

		Global::DelFromPartition( BuildingRole->GUID() );
		BuildingRole->BaseData.ItemInfo.OrgObjID = ObjID;
	}
	for( Iter = _BuildingMap.begin() ; Iter != _BuildingMap.end() ; Iter++ )
	{
		GuildHouseBuildingStruct& info = Iter->second;
//		if( info.Info.ParentDBID > 0 )
//			continue;
		ResetBuildingPos( info.Info.BuildingDBID );
	}

	for( Iter = _BuildingMap.begin() ; Iter != _BuildingMap.end() ; Iter++ )
	{
		GuildHouseBuildingStruct& info = Iter->second;
		GuildBuildingInfoTableStruct* BuildingTableInfo = g_ObjectData->GuildBuilding( info.Info.BuildingID );
		if( BuildingTableInfo == NULL )
			continue;

		RoleDataEx* BuildingRole = Global::GetRoleDataByGUID( info.ObjGUID );
		if( BuildingRole == NULL )
			continue;

		if( BuildingRole->SecRoomID() != -1 )
			continue;
		Global::AddToPartition( BuildingRole->GUID() , BuildingRole->RoomID() );
	}
	return true;
}

int	GuildHousesManageClass::AddBuilding( GuildHouseBuildingInfoStruct& Building )
{
	Print( LV1 , "GuildHousesManageClass::AddBuilding" , "GuildID=%d , BuildingDBID=%d , ParentDBID = %d" 
		, Building.GuildID , Building.BuildingDBID , Building.ParentDBID );

	if( Building.BuildingDBID >= _MaxBuildingDBID )
		_MaxBuildingDBID = Building.BuildingDBID + 1;

	GuildHouseBuildingStruct* ParentBuildingInfo = NULL;

	if( Building.ParentDBID > 0 )
	{
		map< int , GuildHouseBuildingStruct >::iterator Iter;
		Iter = _BuildingMap.find( Building.ParentDBID );
		if( Iter != _BuildingMap.end() )
			ParentBuildingInfo = &(Iter->second);
		else
			return 0;		
	}

	map< int , GuildHouseBuildingStruct >::iterator Iter;
	Iter = _BuildingMap.find( Building.BuildingDBID );

	if( Iter != _BuildingMap.end() )
		return 0;

	GuildBuildingInfoTableStruct* BuildingTableInfo = g_ObjectData->GuildBuilding( Building.BuildingID );
	if( BuildingTableInfo == NULL )
		return 0;


	//取得參考點位置
	float X , Y , Z , Dir;
	bool IsBuild = false;
	CRuMatrix4x4 	matLocal;
	CRuMatrix4x4 	matWorld;
	//bool Ret = MapPointStrPos( BuildingTableInfo , Building.ParentDBID , Building.PointStr , X , Y , Z , Dir , IsBuild );
	bool Ret = MapPointStrPos( BuildingTableInfo , Building.ParentDBID , Building.PointStr , &matLocal , IsBuild );

	//Dir = Building.Dir + Dir;
	if( Ret == false )
	{
		return 0;
	}

	if( IsBuild != false )
		return 0;

	CRuMatrix4x4 matRotation;
	matRotation.SetToRotationEulerXYZ( 0, (Building.Dir)*PI/180.0f, 0 );
	matLocal = matRotation * matLocal;
	//取得母物件資訊
	if( ParentBuildingInfo == NULL )
	{
		matWorld = matLocal;
	}
	else
	{		
		matWorld = matLocal * (*(CRuMatrix4x4*)(&ParentBuildingInfo->matWorld));
	}

	

	//matLocal
	X = matWorld._41;
	Y = matWorld._42;
	Z = matWorld._43;


	CRuVector3 vecDir( 0 , 0 , -1 );
	vecDir = vecDir * matWorld;
	vecDir.x -=X;
	vecDir.y -=Y;
	vecDir.z -=Z;

	Dir = RoleDataEx::CalDir( vecDir.x , vecDir.z);

	if( Building.BuildingDBID <= 0 )
	{
		Building.BuildingDBID = _MaxBuildingDBID;
		_MaxBuildingDBID++;
		Building.State = EM_GuildHouseBuildingInfoState_Insert;
	}

	//-------------------------------------------------------------------------------
	//建立物件
	//-------------------------------------------------------------------------------
	int ObjID = BuildingTableInfo->OrgObjID;
	if( _HouseBase.GuildStyle )
	{
		ObjID = g_ObjectData->GetGuildHouseReplaceNpc( _HouseBase.GuildStyle , ObjID );
	}

	int BuildingGUID = GuildHousesManageClass::CreateBuildingObj( ObjID , Building.BuildingDBID
											, X , Y , Z ,Dir , BuildingTableInfo->Mode , BuildingTableInfo->BuildSize , Building.IsActive );

	if( ParentBuildingInfo == NULL )
		SetMapMask( BuildingTableInfo , Building.PointStr );
	else
		ParentBuildingInfo->ChildBuildingDBIDList.push_back( Building.BuildingDBID );

	//-------------------------------------------------------------------------------
	GuildHouseBuildingStruct TempGuildBuilding;
	TempGuildBuilding.ObjGUID = BuildingGUID;
	TempGuildBuilding.Info = Building;

	memcpy( &(TempGuildBuilding.matWorld) , &matWorld , sizeof(matWorld) );

	_BuildingMap[ Building.BuildingDBID ] = TempGuildBuilding;

	_IsNeedSave = true;

	return Building.BuildingDBID;
}

bool	GuildHousesManageClass::DelBuilding( int BuildingDBID )
{

	map< int , GuildHouseBuildingStruct >::iterator Iter;
	Iter = _BuildingMap.find( BuildingDBID );
	if( Iter == _BuildingMap.end() )
		return false;

	GuildHouseBuildingStruct& BuildingInfo = Iter->second;

	if( BuildingInfo.ChildBuildingDBIDList.size() != 0 )
		return false;

	if( BuildingInfo.Info.ParentDBID == -1 )
	{
		GuildBuildingInfoTableStruct* BuildingTableInfo = g_ObjectData->GuildBuilding( BuildingInfo.Info.BuildingID );
		MapPointInfoStruct Temp = DecondeMapPointStr( BuildingInfo.Info.PointStr );
		//清除地板資料
		if( stricmp( Temp.PointStr , "ground" ) == 0 && BuildingTableInfo != NULL )
		{
			if(		unsigned( Temp.X + BuildingTableInfo->BlockSizeX ) >= _MAX_MAP_POINT_COUNT_ 
				||	unsigned( Temp.Y + BuildingTableInfo->BlockSizeY ) >= _MAX_MAP_POINT_COUNT_ )
				return false;

			for( int i = 0 ; i < BuildingTableInfo->BlockSizeX ; i++ )
			{
				for( int j = 0 ; j < BuildingTableInfo->BlockSizeY ; j++ )
				{
					_BuildingMask[Temp.X+i][Temp.Y+j] = false;
				}
			}	
		}
	}
	
	//-----------------------------------------------------------------------------------
	//如果有母物建
	map< int , GuildHouseBuildingStruct >::iterator ParentIter;
	ParentIter = _BuildingMap.find( BuildingInfo.Info.ParentDBID );

	if( ParentIter != _BuildingMap.end() )
	{
		GuildHouseBuildingStruct& ParentBuildingInfo = ParentIter->second;
		for( unsigned i = 0 ; i < ParentBuildingInfo.ChildBuildingDBIDList.size() ; i++ )
		{
			if( ParentBuildingInfo.ChildBuildingDBIDList[i] == BuildingDBID )
			{
				ParentBuildingInfo.ChildBuildingDBIDList.erase( ParentBuildingInfo.ChildBuildingDBIDList.begin() + i );
				break;
			}
		}
	}
	//-----------------------------------------------------------------------------------
	_DelBuildingDBID.push_back( BuildingInfo.Info.BuildingDBID );
	_IsNeedSave = true;

	RoleDataEx* BuildingRole = Global::GetRoleDataByGUID( BuildingInfo.ObjGUID );
	if( BuildingRole != NULL )
	{
		BuildingRole->TempData.GuildBuildingDBID = -1;
		BuildingRole->BaseData.GuildID = 0;
		BuildingRole->Destroy( "Building Destroy" );
	}
	//-----------------------------------------------------------------------------------
	//移除Map內的房屋資訊
	_BuildingMap.erase( Iter );


	return true;
}
int		GuildHousesManageClass::BuildingUpgrade( int BuildingDBID , int UpgradeBuildingID )
{
	map< int , GuildHouseBuildingStruct >::iterator Iter;
	Iter = _BuildingMap.find( BuildingDBID );
	if( Iter == _BuildingMap.end() )
		return 0;

	GuildHouseBuildingStruct& BInfo = Iter->second;

	GuildBuildingInfoTableStruct* BTableInfo = g_ObjectData->GuildBuilding( BInfo.Info.BuildingID );
	if( BTableInfo == NULL )
		return 0;

	GuildBuildingInfoTableStruct* Up_BTableInfo = g_ObjectData->GuildBuilding( UpgradeBuildingID );
	if( Up_BTableInfo == NULL )
		return 0;

//	if( stricmp( BTableInfo->TypeStr , Up_BTableInfo->TypeStr ) != 0 )
//		return false;

//	if( BTableInfo->Lv +1 != Up_BTableInfo->Lv )
//		return false;
	if( Up_BTableInfo->SrcBuildingID != BTableInfo->ID )
		return 0;

	RoleDataEx* BuildingRole = Global::GetRoleDataByGUID( BInfo.ObjGUID );
	if( BuildingRole == NULL )
		return 0;

	//////////////////////////////////////////////////////////////////////////
	//檢查是否有前置的建設
	//////////////////////////////////////////////////////////////////////////
	if(		GetMaxBuildLv( Up_BTableInfo->NeedTypeStr1) < Up_BTableInfo->NeedLv1 
		||	GetMaxBuildLv( Up_BTableInfo->NeedTypeStr2) < Up_BTableInfo->NeedLv2 )
		return 0; 
	//////////////////////////////////////////////////////////////////////////
	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( _HouseBase.GuildID );
	if( GuildInfo == NULL )
		return 0;

	GuildResourceStruct& Resource = GuildInfo->Base.Resource;

	for( int i = 0 ; i < 7 ; i++ )
	{
		if(		Resource._Value[i] < Up_BTableInfo->Resource._Value[i] 
		||	Up_BTableInfo->Resource._Value[i] < 0 )
		{
			return 0;
		}
	}

	int BuildingGUID = GuildHousesManageClass::CreateBuildingObj( Up_BTableInfo->OrgObjID , BInfo.Info.BuildingDBID
		, BuildingRole->X() , BuildingRole->Y() , BuildingRole->Z() ,BuildingRole->Dir() , Up_BTableInfo->Mode , Up_BTableInfo->BuildSize , BInfo.Info.IsActive );

	BuildingRole->TempData.GuildBuildingDBID = -1;
	BuildingRole->BaseData.GuildID = 0;
	BuildingRole->Destroy( "BuildingUpgrade" );

	BInfo.ObjGUID = BuildingGUID;
	BInfo.Info.BuildingID = UpgradeBuildingID;
	if( BInfo.Info.State == EM_GuildHouseBuildingInfoState_None )
		BInfo.Info.State = EM_GuildHouseBuildingInfoState_Update;


	ResetBuildingPos( BInfo.Info.BuildingDBID );
	_IsNeedSave = true;

	return BuildingGUID;
}

//解譯參考點字串
MapPointInfoStruct			GuildHousesManageClass::DecondeMapPointStr( const char * Str )
{
	char Buf[32];
	MyStrcpy( Buf , Str , sizeof(Buf) );
	_strlwr( Buf );

	MapPointInfoStruct Ret;

	Ret.Value[0] = 0;
	Ret.Value[1] = 0;

	if( strlen( Str ) == 0 )
	{
		MyStrcpy( Ret.PointStr , "ground" , sizeof(Ret.PointStr)  );
		return Ret;
	}

	char	seps[] = "_"; 

	char* token = strtok( Buf , seps );

	MyStrcpy( Ret.PointStr , token , sizeof(Ret.PointStr) );

	token = strtok( NULL , seps );
	if( token )
		Ret.Value[0] = atol( token );
	token = strtok( NULL , seps );
	if( token )
		Ret.Value[1] = atol( token );

	return Ret;
}

//取得參考點位置
bool	GuildHousesManageClass::MapPointStrPos( GuildBuildingInfoTableStruct* BuildingInfo , int ParentBuildingDBID , const char * Str , void* _matLocal , bool& IsBuild )
{
	IsBuild = false;
	CRuMatrix4x4& matLocal = (*(CRuMatrix4x4*)(_matLocal));
	MapPointInfoStruct Temp = DecondeMapPointStr( Str );

	if( stricmp( Temp.PointStr , "ground" ) == 0 )
	{
		if(		unsigned( Temp.X + BuildingInfo->BlockSizeX ) >= _MAX_MAP_POINT_COUNT_ 
			||	unsigned( Temp.Y + BuildingInfo->BlockSizeY ) >= _MAX_MAP_POINT_COUNT_ )
			return false;

		for( int i = 0 ; i < BuildingInfo->BlockSizeX ; i++ )
			for( int j = 0 ; j < BuildingInfo->BlockSizeY ; j++ )
			{
				GroudPointStruct& MapPoint = _Map[ Temp.X+i ][ Temp.Y+j ];
				if( MapPoint.IsBuildOK == false )
					return false;
			}

		GroudPointStruct MapPoint = _Map[ Temp.X ][ Temp.Y ];
		MapPoint.X += _X;
		MapPoint.Y += _Y;
		MapPoint.Z += _Z;

		CRuMatrix4x4 matMove;
		matMove.SetToTranslation( MapPoint.X , MapPoint.Y  , MapPoint.Z );
		matLocal.SetToRotationEulerXYZ( 0, ( _Dir )*PI/180.0f, 0 );
		matLocal = matLocal * matMove;
	}
	else
	{
		//if( ParentBuildingDBID )
		map< int , GuildHouseBuildingStruct >::iterator Iter = _BuildingMap.find( ParentBuildingDBID );
		if( Iter == _BuildingMap.end() )
			return false;

		//取得母物件的建築物資料
		GuildHouseBuildingStruct& ParentBuilding = Iter->second;
		GuildBuildingInfoTableStruct* ParentBuildingTableInfo = g_ObjectData->GuildBuilding( ParentBuilding.Info.BuildingID );
		if( ParentBuildingTableInfo == NULL )
			return false;

		GameObjDbStructEx* ParentBuildingObjDB = Global::GetObjDB( ParentBuildingTableInfo->OrgObjID );
		if( ParentBuildingObjDB  == NULL )
			return false;

		GameObjDbStructEx* ImageObjDB = Global::GetObjDB( ParentBuildingObjDB->ImageID ); 
		if( ImageObjDB == NULL )
			return false;

		CRuEntity* ParentEntiry = g_ruResourceManager->LoadEntity( ImageObjDB->Image.ACTWorld );

		if( ParentEntiry == NULL )
			return false;

		CRuEntity* LinkEntiry = RuEntity_FindLinkFrame( ParentEntiry , Str , false );

		if( LinkEntiry == NULL )
			return false;

		IsBuild = false;

		CRuVector3	Point = LinkEntiry->GetTranslation();

		matLocal.SetToTranslation( Point );
		const CRuQuaternion& Quaternion = LinkEntiry->GetRotation( ruFRAMETYPE_LOCAL );
		matLocal = Quaternion.GetMatrix() * matLocal;
//		CRuMatrix4x4 matTemp;
//		matTemp.SetToRotationEulerXYZ( 0 , PI , 0 );
//		matLocal = matTemp * matLocal;


/*
		vector = vector * Quaternion.GetMatrix();
		Dir = RoleDataEx::CalDir( vector.x , vector.z );

		RoleDataEx* ParentRole = Global::GetRoleDataByGUID( ParentBuilding.ObjGUID );
		if( ParentRole != NULL )
		{
			float T = ( ParentRole->Dir() - 90 )* float( 3.14 ); //180 / 57.2958f
			float Dx = Z * sin( T ) + X*cos( T );
			float Dz = Z * cos( T ) - X*sin( T );

			X = ParentRole->X() + Dx;
			Y+= ParentRole->Y();
			Z = ParentRole->Z() + Dz;
		}
		*/
		ruSAFE_RELEASE(ParentEntiry);
	}


	return true;
}
//設定Map Mask				
bool	GuildHousesManageClass::SetMapMask( GuildBuildingInfoTableStruct* BuildingTableInfo , const char *Point )
{
	MapPointInfoStruct Temp = DecondeMapPointStr( Point );

	if( stricmp( Temp.PointStr , "ground" ) == 0 )
	{
		for( int i = 0 ; i < BuildingTableInfo->BlockSizeX ; i++ )
			for( int j = 0 ; j < BuildingTableInfo->BlockSizeY ; j++ )
				_BuildingMask[Temp.X+i][Temp.Y+j] = true;	
	}
	return true;
}
//清除Map Mask				
bool	GuildHousesManageClass::ClearMapMask( GuildBuildingInfoTableStruct* BuildingTableInfo , const char *Point )
{
	MapPointInfoStruct Temp = DecondeMapPointStr( Point );

	if( stricmp( Temp.PointStr , "ground" ) == 0 )
	{
		for( int i = 0 ; i < BuildingTableInfo->BlockSizeX ; i++ )
			for( int j = 0 ; j < BuildingTableInfo->BlockSizeY ; j++ )
				_BuildingMask[Temp.X+i][Temp.Y+j] = false;
	}
	return true;
}

GuildHouseBuildingStruct*	GuildHousesManageClass::GetBuildingInfo( int BuildingDBID )
{
	map< int , GuildHouseBuildingStruct >::iterator Iter;

	Iter = _BuildingMap.find( BuildingDBID );
	if( Iter == _BuildingMap.end() )
		return NULL;

	return &(Iter->second);
}
GuildHouseBuildingStruct*	GuildHousesManageClass::GetBuildingInfo( int ParentBuildingDBID , const char* Point )
{
	map< int , GuildHouseBuildingStruct >::iterator Iter;

	for( Iter = _BuildingMap.begin() ; Iter != _BuildingMap.end() ; Iter++ )
	{
		GuildHouseBuildingStruct& Info = Iter->second;
		if(		Info.Info.ParentDBID == ParentBuildingDBID 
			&&	stricmp( Info.Info.PointStr , Point ) == 0 )
			return &Info;
	}
	return NULL;
}

map< int , GuildHouseItemStruct >* GuildHousesManageClass::ItemBox( int PageID )
{
	if( unsigned(PageID) >= (unsigned)_HouseBase.ItemMaxPageCount )
		return NULL;

	return &_ItemBox[PageID];
}

//////////////////////////////////////////////////////////////////////////
//公會倉庫
//////////////////////////////////////////////////////////////////////////
bool	GuildHousesManageClass::AddItem( GuildHouseItemStruct& Item )
{
	if( _DEF_GUIDHOUSEITEM_MAX_PAGE_ <= (unsigned)Item.PageID )
		return false;

	if( Item.GuildID != _HouseBase.GuildID )
		return false;

	Item.State = EM_GuildHouseItemInfoState_None;
	if( Item.ItemDBID == -1 )
	{
		Item.ItemDBID = _MaxItemDBID;
		Item.State = EM_GuildHouseItemInfoState_Insert;
		_MaxItemDBID++;

		_IsNeedSave = true;
	}
	else
	{
		if( Item.ItemDBID >= _MaxItemDBID )
			_MaxItemDBID = Item.ItemDBID+1;
	}

	map< int , GuildHouseItemStruct >::iterator Iter;

	Iter = _ItemBox[ Item.PageID ].find( Item.PosID );
	if( Iter != _ItemBox[ Item.PageID ].end() )
		return false;

	_ItemBox[ Item.PageID ][ Item.PosID ] = Item;
	return true;
}

//刪除物件
bool	GuildHousesManageClass::DelItem( int PageID , int Pos )
{
	if( _DEF_GUIDHOUSEITEM_MAX_PAGE_ <= (unsigned)PageID )
		return false;

	map< int , GuildHouseItemStruct >::iterator Iter;

	Iter = _ItemBox[ PageID ].find( Pos );
	if( Iter == _ItemBox[ PageID ].end() )
		return false;

	_DelItemDBID.push_back( Iter->second.ItemDBID );

	_ItemBox[ PageID ].erase( Iter );

	_IsNeedSave = true;
	return true;
}
//物品交換
bool	GuildHousesManageClass::SwapItem( RoleDataEx* Role , int PageID1 , int Pos1 , int PageID2 , int Pos2 )			//兩物品互換位置
{
	if( _DEF_GUIDHOUSEITEM_MAX_PAGE_ <= (unsigned)PageID1 || _DEF_GUIDHOUSEITEM_MAX_PAGE_ <= (unsigned)PageID2 )
		return false;

	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( Role->DBID() );	
	if( Member == NULL )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_GuildHouse_GuildInfoError );
		return false;
	}

	if( Role->GuildID() != _HouseBase.GuildID )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_GuildHouse_GuildInfoError );
		return false;
	}

	if( unsigned( Member->Rank ) >= EM_GuildRank_Count )
	{
		return false;
	}

	if( Member->Guild->Base.LeaderDBID != Role->DBID() )
	{
		if(		_ItemRight[ PageID1 ].Rank[ Member->Rank ].Put == false 
			||	_ItemRight[ PageID2 ].Rank[ Member->Rank ].Put == false  )
		{
			Role->Net_GameMsgEvent( EM_GameMessageEvent_GuildHouse_RightError );
			return false;
		}
	}

	map< int , GuildHouseItemStruct >::iterator ItemIter1, ItemIter2;

	ItemIter1 = _ItemBox[ PageID1 ].find( Pos1 );
	ItemIter2 = _ItemBox[ PageID2 ].find( Pos2 );

	if( ItemIter1 == _ItemBox[ PageID1 ].end() && ItemIter2 == _ItemBox[PageID2].end() )
		return false;

	if( ItemIter1 == _ItemBox[ PageID1 ].end() )
	{
		GuildHouseItemStruct Item = ItemIter2->second;
		Item.PageID = PageID1;
		Item.PosID = Pos1;
		if( Item.State == EM_GuildHouseItemInfoState_None )
			Item.State = EM_GuildHouseItemInfoState_Update;
		_ItemBox[PageID1][Pos1] = Item;
		_ItemBox[ PageID2 ].erase( ItemIter2 );

		if( PageID1 != PageID2 )
		{
			AddLog( PageID1 , EM_GuildHouseLootType_Move , Role->DBID() , Item.Item.OrgObjID , Item.Item.Count , PageID2 , PageID1 );
			AddLog( PageID2 , EM_GuildHouseLootType_Move , Role->DBID() , Item.Item.OrgObjID , Item.Item.Count , PageID2 , PageID1 );
		}
	}
	else if( ItemIter2 == _ItemBox[ PageID2 ].end() )
	{
		GuildHouseItemStruct Item = ItemIter1->second;
		Item.PageID = PageID2;
		Item.PosID = Pos2;
		if( Item.State == EM_GuildHouseItemInfoState_None )
			Item.State = EM_GuildHouseItemInfoState_Update;

		_ItemBox[PageID2][Pos2] = Item;
		_ItemBox[ PageID1 ].erase( ItemIter1 );
		
		if( PageID1 != PageID2 )
		{
			AddLog( PageID1 , EM_GuildHouseLootType_Move , Role->DBID() , Item.Item.OrgObjID , Item.Item.Count , PageID1 , PageID2 );
			AddLog( PageID2 , EM_GuildHouseLootType_Move , Role->DBID() , Item.Item.OrgObjID , Item.Item.Count , PageID1 , PageID2 );
		}

	}
	else
	{
		GameObjDbStructEx* OrgDB = g_ObjectData->GetObj( ItemIter1->second.Item.OrgObjID );
		if( OrgDB == NULL )
			return false;

		if( OrgDB->MaxHeap == 1 || ItemIter1->second.Item.OrgObjID != ItemIter2->second.Item.OrgObjID )
		{
			GuildHouseItemStruct Item1 = ItemIter1->second;
			GuildHouseItemStruct Item2 = ItemIter2->second;

			Item1.PageID = PageID2;
			Item2.PageID = PageID1;
			Item1.PosID = Pos2;
			Item2.PosID = Pos1;

			if( Item1.State == EM_GuildHouseItemInfoState_None )
				Item1.State = EM_GuildHouseItemInfoState_Update;
			if( Item2.State == EM_GuildHouseItemInfoState_None )
				Item2.State = EM_GuildHouseItemInfoState_Update;

			_ItemBox[PageID2][Pos2] = Item1;
			_ItemBox[PageID1][Pos1] = Item2;

			if( PageID1 != PageID2 )
			{
				AddLog( PageID1 , EM_GuildHouseLootType_Move , Role->DBID() , Item1.Item.OrgObjID , Item1.Item.Count , PageID1 , PageID2 );
				AddLog( PageID2 , EM_GuildHouseLootType_Move , Role->DBID() , Item1.Item.OrgObjID , Item1.Item.Count , PageID1 , PageID2 );
				AddLog( PageID1 , EM_GuildHouseLootType_Move , Role->DBID() , Item2.Item.OrgObjID , Item2.Item.Count , PageID2 , PageID1 );
				AddLog( PageID2 , EM_GuildHouseLootType_Move , Role->DBID() , Item2.Item.OrgObjID , Item2.Item.Count , PageID2 , PageID1 );
			}
		}
		else
		{
			if( OrgDB->MaxHeap >= ItemIter1->second.Item.Count + ItemIter2->second.Item.Count )
			{
				AddLog( PageID1 , EM_GuildHouseLootType_Move , Role->DBID() , ItemIter2->second.Item.OrgObjID , ItemIter2->second.Item.Count , PageID2 , PageID1 );
				AddLog( PageID2 , EM_GuildHouseLootType_Move , Role->DBID() , ItemIter2->second.Item.OrgObjID , ItemIter2->second.Item.Count , PageID2 , PageID1 );

				ItemIter1->second.Item.Count = ItemIter2->second.Item.Count + ItemIter1->second.Item.Count;
				DelItem( PageID2 , Pos2 );

				if( ItemIter1->second.State == EM_GuildHouseItemInfoState_None )
					ItemIter1->second.State = EM_GuildHouseItemInfoState_Update;
			}
			else
			{
				int OrgItem2Count = ItemIter2->second.Item.Count;
				ItemIter2->second.Item.Count = ItemIter2->second.Item.Count + ItemIter1->second.Item.Count - OrgDB->MaxHeap;
				ItemIter1->second.Item.Count = OrgDB->MaxHeap;

				int DItem2Count = ItemIter2->second.Item.Count - OrgItem2Count;
				if( DItem2Count > 0 )
				{
					if( PageID1 != PageID2 )
					{
						AddLog( PageID1 , EM_GuildHouseLootType_Move , Role->DBID() , ItemIter1->second.Item.OrgObjID , DItem2Count , PageID2 , PageID1 );
						AddLog( PageID2 , EM_GuildHouseLootType_Move , Role->DBID() , ItemIter1->second.Item.OrgObjID , DItem2Count , PageID2 , PageID1 );
					}
				}

				if( ItemIter1->second.State == EM_GuildHouseItemInfoState_None )
					ItemIter1->second.State = EM_GuildHouseItemInfoState_Update;
				if( ItemIter2->second.State == EM_GuildHouseItemInfoState_None )
					ItemIter2->second.State = EM_GuildHouseItemInfoState_Update;
			}
		}
	}
	_IsNeedSave = true;
	return true;
}
int		GuildHousesManageClass::GetLootCount( int PageID , int PlayerDBID )
{
	int GetCount = 0;
	int Time = TimeStr::Now_Value()/(24*60*60);	
	//map< int , vector<GuildItemStoreLogStruct> >	_ItemLog[ _DEF_GUIDHOUSEITEM_MAX_PAGE_ ];		// 每天的Log資料
	vector<GuildItemStoreLogStruct>& Vec = _ItemLog[PageID][ Time ];
	for( unsigned i = 0 ; i < Vec.size() ; i++ )
	{
		if( Vec[i].PlayerDBID == PlayerDBID && Vec[i].Type == EM_GuildHouseLootType_Get )
			GetCount++;
	}

	return GetCount;
}

//身體與屋子交換物品
bool	GuildHousesManageClass::SwapItem( RoleDataEx* Role , ItemFieldStruct& bodyItem , int PageID , int Pos , bool isFromBody , int &GetCount )
{
	if( _DEF_GUIDHOUSEITEM_MAX_PAGE_ <= (unsigned)PageID )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_GuildHouse_RightError );
		return false;
	}


	if( Role->GuildID() != _HouseBase.GuildID )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_GuildHouse_GuildInfoError );
		return false;
	}

	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( Role->DBID() );	
	if( Member == NULL )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_GuildHouse_GuildInfoError );
		return false;
	}

		
	if(		bodyItem.IsEmpty() == false &&
		(	bodyItem.Mode.Trade == false 
		||	bodyItem.Mode.ItemLock != false 	) )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_GuildHouse_ItemNoTrade );
		return false;
	}

	//記算剩餘取的次數
	//int Rank = Member->Rank;
	if( unsigned( Member->Rank ) >= EM_GuildRank_Count )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_GuildHouse_RightError );
		return false;
	}

	int MaxGetCount = _ItemRight[ PageID ].Rank[ Member->Rank ].GetCount;

	//計算今天此人已存取的次數
	GetCount = GetLootCount( PageID , Role->DBID() );

	bool GetAbility = _ItemRight[ PageID ].Rank[ Member->Rank ].Get;
	bool PutAbility = _ItemRight[ PageID ].Rank[ Member->Rank ].Put;
	if( GetCount >= MaxGetCount )
	{
		//Role->Net_GameMsgEvent( EM_GameMessageEvent_GuildHouse_ItemCountError );
		GetAbility = false;
	}

	if( Member->Guild->Base.LeaderDBID == Role->DBID() )
	{
		GetAbility = true;
		PutAbility = true;
	}

	if( GetAbility == false && PutAbility == false )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_GuildHouse_RightError );
		return false;
	}

	int PlayerDBID = Role->DBID();

	map< int , GuildHouseItemStruct >::iterator Iter;

	Iter = _ItemBox[ PageID ].find( Pos );
	if( Iter == _ItemBox[ PageID ].end() )
	{
		if( bodyItem.IsEmpty() )
		{
			return false;
		}
		else
		{
			if( PutAbility == false )
				return false;
			GetCount++;

			GuildHouseItemStruct TempItem;
			TempItem.GuildID = _HouseBase.GuildID;
			TempItem.PageID = PageID;
			TempItem.PosID = Pos;
			TempItem.ItemDBID = -1;
			TempItem.State = EM_GuildHouseItemInfoState_None;
			TempItem.Item = bodyItem;
			AddItem( TempItem );
			AddLog( PageID , EM_GuildHouseLootType_Put , PlayerDBID , bodyItem.OrgObjID , bodyItem.Count );

			Global::Log_ItemTrade( -1 , -1 , PlayerDBID , 0 , 0 , EM_ActionType_GuildStoreHouse_Get , bodyItem , "" );

			bodyItem.Init();
		}
	}
	else
	{
		if( bodyItem.IsEmpty() )
		{
			if( GetAbility == false )
				return false;

			bodyItem = Iter->second.Item;
			DelItem( PageID , Pos );

			AddLog( PageID , EM_GuildHouseLootType_Get , PlayerDBID , bodyItem.OrgObjID , bodyItem.Count );
			Global::Log_ItemDestroy( Role , EM_ActionType_GuildStoreHouse_Put , bodyItem , -1 ,  -1 , "" );
		}
		else
		{
			if( GetAbility == false || PutAbility == false )
				return false;
			GetCount++;

			AddLog( PageID , EM_GuildHouseLootType_Get , PlayerDBID , Iter->second.Item.OrgObjID , Iter->second.Item.Count );
			AddLog( PageID , EM_GuildHouseLootType_Put , PlayerDBID , bodyItem.OrgObjID , bodyItem.Count );

			Global::Log_ItemTrade( -1 , -1 , PlayerDBID , 0 , 0 , EM_ActionType_GuildStoreHouse_Get , Iter->second.Item , "" );
			Global::Log_ItemDestroy( Role , EM_ActionType_GuildStoreHouse_Put , bodyItem , -1 ,  -1 , "" );

			swap( bodyItem , Iter->second.Item );
			if( Iter->second.State == EM_GuildHouseItemInfoState_None )
				Iter->second.State = EM_GuildHouseItemInfoState_Update;
		}
	}

	_IsNeedSave = true;
	return true;
}
//取得倉庫某位置的資料
GuildHouseItemStruct*	GuildHousesManageClass::GetItem( int PageID , int Pos )
{
	if( _DEF_GUIDHOUSEITEM_MAX_PAGE_ <= (unsigned)PageID )
		return NULL;

	map< int , GuildHouseItemStruct >::iterator Iter;

	Iter = _ItemBox[ PageID ].find( Pos );
	if( Iter == _ItemBox[ PageID ].end() )
		return NULL;

	return &(Iter->second);
}

//////////////////////////////////////////////////////////////////////////
//公會傢俱
//////////////////////////////////////////////////////////////////////////
//載入家俱物品
bool	GuildHousesManageClass::AddFurniture( GuildHouseFurnitureItemStruct& Item )
{
	if( _HouseBase.FurnitureMaxCount <= (int)_FurnitureItemMap.size() )
		return false;

	if( Item.GuildID != _HouseBase.GuildID )
		return false;

	Item.State = EM_GuildHouseItemInfoState_None;
	if( Item.ItemDBID == -1 )
	{
		Item.ItemDBID = _MaxFurnitureDBID;
		Item.State = EM_GuildHouseItemInfoState_Insert;
		_MaxFurnitureDBID++;

		_IsNeedSave = true;
	}
	else
	{
		if( Item.ItemDBID >= _MaxFurnitureDBID )
			_MaxFurnitureDBID = Item.ItemDBID+1;
	}

	map< int , GuildHouseFurnitureItemStruct >::iterator Iter;

	Iter = _FurnitureItemMap.find( Item.Pos );
	if( Iter != _FurnitureItemMap.end() )
		return false;

	_FurnitureItemMap[ Item.Pos ] = Item;
	
	//////////////////////////////////////////////////////////////////////////
	//處理顯像
	//////////////////////////////////////////////////////////////////////////
	if( Item.Layout.IsShow )
	{
		int ItemGUID = CreateBuildingFurniture( Item.Item , Item.ItemDBID , Item.Layout );		
		_FurnitureDBIDtoObjGUIDMap[ Item.ItemDBID ] = ItemGUID;
	}
	//////////////////////////////////////////////////////////////////////////
	return true;
}
//刪除家俱物品
bool	GuildHousesManageClass::DelFurniture( int Pos )
{
//	if( _DEF_GUIDHOUSEITEM_MAX_FURNITURE_ <= (unsigned)Pos )
//		return false;

	map< int , GuildHouseFurnitureItemStruct >::iterator Iter;

	Iter = _FurnitureItemMap.find( Pos );
	if( Iter == _FurnitureItemMap.end() )
		return false;

	map< int , int >::iterator DBIDIter = _FurnitureDBIDtoObjGUIDMap.find( Iter->second.ItemDBID );
	if( DBIDIter != _FurnitureDBIDtoObjGUIDMap.end() )
	{
		BaseItemObject* ItemObj = BaseItemObject::GetObj( DBIDIter->second );
		if( ItemObj != NULL )
			ItemObj->Destroy( "DelFurniture" );
		_FurnitureDBIDtoObjGUIDMap.erase( DBIDIter );
	}


	_DelFurnitureDBID.push_back( Iter->second.ItemDBID );
	_FurnitureItemMap.erase( Iter );

	_IsNeedSave = true;
	return true;
}
//家俱位置交換
bool	GuildHousesManageClass::SwapFurniture( int Pos1 , int Pos2 )
{
	map< int , GuildHouseFurnitureItemStruct >::iterator ItemIter1, ItemIter2;

	ItemIter1 = _FurnitureItemMap.find( Pos1 );
	ItemIter2 = _FurnitureItemMap.find( Pos2 );

	if( ItemIter1 == _FurnitureItemMap.end() && ItemIter2 == _FurnitureItemMap.end() )
		return false;

	if( ItemIter1 == _FurnitureItemMap.end() )
	{
		GuildHouseFurnitureItemStruct Item = ItemIter2->second;
		Item.Pos = Pos1;

		if( Item.State == EM_GuildHouseItemInfoState_None )
			Item.State = EM_GuildHouseItemInfoState_Update;

		_FurnitureItemMap[ Pos1 ] = Item;
		_FurnitureItemMap.erase( ItemIter2 );

//		if( ItemIter2->second.State == EM_GuildHouseItemInfoState_None )
//			ItemIter2->second.State = EM_GuildHouseItemInfoState_Update;
	}
	else if( ItemIter2 == _FurnitureItemMap.end() )
	{
		GuildHouseFurnitureItemStruct Item = ItemIter1->second;
		Item.Pos = Pos2;

		if( Item.State == EM_GuildHouseItemInfoState_None )
				Item.State = EM_GuildHouseItemInfoState_Update;

		_FurnitureItemMap[ Pos2 ] = Item;
		_FurnitureItemMap.erase( ItemIter1 );

		//	if( ItemIter1->second.State == EM_GuildHouseItemInfoState_None )
		//		ItemIter1->second.State = EM_GuildHouseItemInfoState_Update;

	}
	else
	{
		GuildHouseFurnitureItemStruct Item1 = ItemIter1->second;
		GuildHouseFurnitureItemStruct Item2 = ItemIter2->second;

		Item1.Pos = Pos2;
		Item2.Pos = Pos1;
		swap( Item1.State , Item2.State );

		if( Item1.State == EM_GuildHouseItemInfoState_None )
			Item1.State = EM_GuildHouseItemInfoState_Update;
		if( Item2.State == EM_GuildHouseItemInfoState_None )
			Item2.State = EM_GuildHouseItemInfoState_Update;

		_FurnitureItemMap[ Item1.Pos ] = Item1;
		_FurnitureItemMap[ Item2.Pos ] = Item2;

	}
	_IsNeedSave = true;
	return true;
}
//身體與家俱位置交換
bool	GuildHousesManageClass::SwapFurniture( RoleDataEx* Role , ItemFieldStruct& bodyItem , int Pos , bool isFromBody )
{
	//HouseItemStruct* furnitureItem = GetFurniture( Pos );	
	map< int , GuildHouseFurnitureItemStruct >::iterator furnitureIter1;

	furnitureIter1 = _FurnitureItemMap.find( Pos );
	GameObjDbStructEx* bodyItemDB = Global::GetObjDB( bodyItem.OrgObjID );

	if( bodyItemDB != NULL )
	{
		if( bodyItemDB->IsItem() != false && bodyItemDB->Item.ItemType != EM_ItemType_Furniture )
		{
			Role->Net_GameMsgEvent( EM_GameMessageEvent_GuildHouse_FurnitureError );
			return false;
		}

		if( bodyItemDB->Item.Furniture_Type  == EM_FurnitureType_HouseOnly )
		{
			Role->Net_GameMsgEvent( EM_GameMessageEvent_GuildHouse_FurnitureError );
			return false;
		}

		if( furnitureIter1 == _FurnitureItemMap.end() && bodyItemDB->IsItem() == false )
			return false;
	}

	if( furnitureIter1 != _FurnitureItemMap.end() && furnitureIter1->second.OwnerDBID != Role->DBID() )
	{
		Role->Net_GameMsgEvent( EM_GameMessageEvent_GuildHouse_FurnitureNotYours );
		return false;
	}

	if( furnitureIter1 == _FurnitureItemMap.end() )
	{
		if( bodyItem.IsEmpty() )
			return true;

		GuildHouseFurnitureItemStruct TempItem;
		TempItem.Init();
		TempItem.GuildID = Role->GuildID();
		TempItem.Item = bodyItem;
		TempItem.Pos = Pos;
		TempItem.OwnerDBID = Role->DBID();

		TempItem.ItemDBID = _MaxFurnitureDBID;
		TempItem.State = EM_GuildHouseItemInfoState_Insert;
		_MaxFurnitureDBID++;

		_FurnitureItemMap[ Pos ] = TempItem;

		Global::Log_ItemDestroy( Role , EM_ActionType_GuildHouse_Funiture_Put , bodyItem , -1 ,  -1 , "" );
		bodyItem.Init();
		_IsNeedSave = true;
		return true;
	}
	
	if( furnitureIter1->second.Layout.IsShow != false )
	{
		vector< BaseItemObject* >& ObjList = *BaseItemObject::GetZoneObjList();
		for( int i = 0 ; i < (int)ObjList.size() ; i++  )
		{
			BaseItemObject* BuildingObj = ObjList[i];
			if( BuildingObj == NULL )
				continue;

			if( BuildingObj->Role()->RoomID() != _RoomID )
				continue;

			if( BuildingObj->Role()->TempData.GuildBuildingType != EM_GuildBuildingType_Furniture )
				continue;

			if( BuildingObj->Role()->TempData.GuildFurnitureDBID == furnitureIter1->second.ItemDBID )
			{
				BuildingObj->Destroy( "SwapFurniture" );
			}

		}

	}

	if( bodyItemDB == NULL )
	{
		bodyItem = furnitureIter1->second.Item;
		_DelFurnitureDBID.push_back( furnitureIter1->second.ItemDBID );
		_FurnitureItemMap.erase( furnitureIter1 );

		Global::Log_ItemTrade( -1 , -1 , Role->DBID() , 0 , 0 , EM_ActionType_GuildHouse_Funiture_Get , bodyItem , "");
	}
	else
	{
		Global::Log_ItemDestroy( Role , EM_ActionType_GuildHouse_Funiture_Put , bodyItem , -1 ,  -1 , "" );
		Global::Log_ItemTrade( -1 , -1 , Role->DBID() , 0 , 0 , EM_ActionType_GuildHouse_Funiture_Get , furnitureIter1->second.Item , "" );

		std::swap( bodyItem , furnitureIter1->second.Item );
		furnitureIter1->second.Layout.Init();
		if( furnitureIter1->second.State == EM_GuildHouseItemInfoState_None )
			furnitureIter1->second.State = EM_GuildHouseItemInfoState_Update;
	}

	_IsNeedSave = true;

	return true;
}
//取得傢俱資料
GuildHouseFurnitureItemStruct* GuildHousesManageClass::GetFurniture( int Pos )
{

	map< int , GuildHouseFurnitureItemStruct >::iterator furnitureIter1;
	furnitureIter1 = _FurnitureItemMap.find( Pos );
	if( furnitureIter1 == _FurnitureItemMap.end() )
		return NULL;
	return &furnitureIter1->second;
}
//////////////////////////////////////////////////////////////////////////
//建立家俱物件
int	GuildHousesManageClass::CreateBuildingFurniture( ItemFieldStruct& Item , int FurnitureDBID , House3DLayoutStruct& Layout )
{
	GameObjDbStructEx* FurnitureObjDB = Global::GetObjDB( Item.OrgObjID );
	if( FurnitureObjDB->IsItem() == false || FurnitureObjDB->Item.ItemType != EM_ItemType_Furniture )
		return 0;

	GameObjDbStructEx* BuildingObjDB = Global::GetObjDB( FurnitureObjDB->Item.FurnitureNPCObjID );	
	if( BuildingObjDB == NULL )
		return 0;

	if( Layout.IsShow == false )
		return 0;

	CRuMatrix4x4 matLocal;
	CRuMatrix4x4 matMove;
	CRuMatrix4x4 matRotation;

	//static float Ddir = 1;
	static float DAng = -90;

	float LayoutDir = (Layout.Angle+ DAng);
	matMove.SetToTranslation( Layout.X , Layout.Y , Layout.Z );
	matRotation.SetToRotationEulerXYZ( 0, (LayoutDir)*PI/180.0f, 0 );
	matLocal = matRotation * matMove;	

	matMove.SetToTranslation( _X , _Y , _Z  );
//	matLocal = matLocal * matMove;
	matRotation.SetToRotationEulerXYZ( 0, ( _Dir )*PI/180.0f, 0 );
	matMove = matRotation * matMove;
	
	matLocal = matLocal * matMove;


	//matLocal
	float X = matLocal._41;
	float Y = matLocal._42;
	float Z = matLocal._43;


	CRuVector3 vecDir( 0 , 0 , -1 );
	vecDir = vecDir * matLocal;
	vecDir.x -=X;
	vecDir.y -=Y;
	vecDir.z -=Z;
	
	float Dir = RoleDataEx::CalDir( vecDir.x , vecDir.z);
	

	int ItemGUID = Global::CreateObj( FurnitureObjDB->Item.FurnitureNPCObjID , X , Y , Z , Dir , 1 );
//	int ItemGUID = Global::CreateObj( FurnitureObjDB->Item.FurnitureNPCObjID 
//		,_X+Layout.X , _Y+Layout.Y , _Z+ Layout.Z 
//		, _Dir + Layout.Angle , 1 );

	BaseItemObject* BuildingObj = Global::GetObj( ItemGUID );
	if( BuildingObj == NULL )
	{
		Print( LV1 , "CreateBuildingObj" ,"Building Object(%d) Create Error!!" , Item.OrgObjID );
		return 0;
	}

	RoleDataEx* BuildingRole  = BuildingObj->Role();

	BuildingRole->TempData.PotInfo = Item;
//	BuildingRole->BaseData.ItemInfo = Item;
	BuildingRole->TempData.AI.CampID = _CampID;

	BuildingRole->BaseData.Mode.Show = true;
	BuildingRole->BaseData.Mode.Obstruct = true;
	BuildingRole->BaseData.Mode.Gravity = false;
	BuildingRole->BaseData.Mode.ShowRoleHead = false;
	BuildingRole->BaseData.Mode.HideName = true;
//	if( Global::Ini()->IsGuildHouseZone != false )
		BuildingRole->BaseData.Mode.Fight = false;
//	else
//		BuildingRole->BaseData.Mode.Fight = true;

	BuildingRole->TempData.GuildFurnitureDBID = FurnitureDBID;
	BuildingRole->BaseData.GuildID = _HouseBase.GuildID;
	BuildingRole->TempData.GuildBuildingType = EM_GuildBuildingType_Furniture;

	BuildingRole->BaseData.vX = Layout.UpX;
	BuildingRole->BaseData.vY = Layout.UpY;
	BuildingRole->BaseData.vZ = Layout.UpZ;

	Global::AddToPartition( ItemGUID , RoomID() );

	if( BuildingObjDB->IsNPC() || BuildingObjDB->IsQuestNPC() ) 
	{
		if( strlen( BuildingObjDB->NPC.szLuaInitScript ) != 0 )
		{
			BuildingObj->PlotVM()->CallLuaFunc( BuildingObjDB->NPC.szLuaInitScript , BuildingRole->GUID() );
		}
	}

	return ItemGUID;
}
/*
//設定公會倉庫存取權限
bool	GuildHousesManageClass::SetItemRight( GuildStorehouseRightStruct Right , bool IsLoad )
{
	if( _DEF_GUIDHOUSEITEM_MAX_PAGE_ <= (unsigned)Right.PageID )
		return false;
	_ItemRight[ Right.PageID ] = Right;
}
*/
GuildStorehouseRightStruct*	GuildHousesManageClass::GetItemRight( int PageID )
{
	if( (unsigned)PageID >= _DEF_GUIDHOUSEITEM_MAX_PAGE_ )
		return NULL;

	return &_ItemRight[PageID];
}

bool	GuildHousesManageClass::SetItemRight( GuildStorehouseRightStruct& Config )
{
	if( (unsigned)Config.PageID >= _DEF_GUIDHOUSEITEM_MAX_PAGE_ )
		return false;

	if( (unsigned)Config.PageID >= (unsigned)_HouseBase.ItemMaxPageCount )
		return false;

	_ItemRight[Config.PageID] = Config;
	return true;
}

vector<GuildItemStoreLogStruct>* GuildHousesManageClass::GetItemLog( int PageID , int DayBefore )
{
	int Time = TimeStr::Now_Value()/(24*60*60) - DayBefore;
	if( unsigned(PageID) >= _DEF_GUIDHOUSEITEM_MAX_PAGE_ )
		return NULL;

	return &( _ItemLog[PageID][Time] );
}

bool	GuildHousesManageClass::SetItemLog( GuildItemStoreLogStruct& Log )
{
	int Time = Log.Time/(24*60*60);

	if( unsigned(Log.PageID ) >= _DEF_GUIDHOUSEITEM_MAX_PAGE_ )
		return false;	

	_ItemLog[ Log.PageID ][Time].push_back( Log );
	return true;
}

void	GuildHousesManageClass::AddLog( int PageID , GuildHouseLootTypeENUM Type , int	PlayerDBID , int ItemOrgID , int ItemCount , int FromPageID , int ToPageID )
{
	GuildItemStoreLogStruct Log;
	Log.GuildID = _HouseBase.GuildID;
	Log.ItemCount = ItemCount;
	Log.ItemOrgID = ItemOrgID;
	Log.PlayerDBID = PlayerDBID;
	Log.Type = Type;
	Log.PageID = PageID;
	Log.Time = TimeStr::Now_Value();
	Log.FromPageID = FromPageID;
	Log.ToPageID = ToPageID;

	SetItemLog( Log );

	//通知所有線上玩家
	NetSrv_GuildHouses::SC_All_ItemLog( Log.GuildID , _RoomID , Log );

	char Buf[2048];
	sprintf_s( Buf , sizeof( Buf ), "INSERT INTO GuildHouse_ItemLog(GuildID, PageID, Time, Type, PlayerDBID, ItemOrgID, ItemCount, FromPageID , ToPageID )VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d)"
						,Log.GuildID , Log.PageID , Log.Time , Log.Type , Log.PlayerDBID , Log.ItemOrgID , Log.ItemCount  , FromPageID , ToPageID  );

	Net_DCBase::SqlCommand( Log.GuildID , Buf );
}

void	GuildHousesManageClass::SetPostion( float X , float Y , float Z , float Dir )
{
	_X = X;
	_Y = Y;
	_Z = Z;
	_Dir = Dir;
}

void	GuildHousesManageClass::SetPlayerPostion( float X , float Y , float Z , float Dir )
{
	_PX = X;
	_PY = Y;
	_PZ = Z;
	_PDir = Dir;
}

void	GuildHousesManageClass::GetPlayerPostion( float& X , float& Y , float& Z , float& Dir )
{
	X = _PX;
	Y = _PY;
	Z = _PZ;
	Dir = _PDir;
}

//取得某個建築物的等級
int	GuildHousesManageClass::GetMaxBuildLv( const char* TypeStr )
{
	int		MaxLv = 0;
	map< int , GuildHouseBuildingStruct >::iterator Iter;

	for( Iter = _BuildingMap.begin() ; Iter != _BuildingMap.end() ; Iter++ )
	{
		GuildBuildingInfoTableStruct* BuildingTableInfo = g_ObjectData->GuildBuilding( Iter->second.Info.BuildingID );
		if( BuildingTableInfo == NULL )
			continue;
		if(		stricmp( TypeStr , BuildingTableInfo->TypeStr ) == 0 
			&&	MaxLv < BuildingTableInfo->Lv )
			MaxLv = BuildingTableInfo->Lv;
	}

	return MaxLv;
}

//重行設定某個建築的位置
bool	GuildHousesManageClass::ResetBuildingPos( int BuildingDBID )
{
	map< int , GuildHouseBuildingStruct >::iterator Iter;
	Iter = _BuildingMap.find( BuildingDBID );
	if( Iter == _BuildingMap.end() )
		return false;

	GuildHouseBuildingStruct& BuildingInfo = Iter->second;

	if( BuildingInfo.ChildBuildingDBIDList.size() == 0 )
		return true;

	_IsNeedSave = true;

	for( unsigned i = 0 ; i < BuildingInfo.ChildBuildingDBIDList.size() ; i++ )
	{
		Iter = _BuildingMap.find( BuildingInfo.ChildBuildingDBIDList[i] );
		if( Iter == _BuildingMap.end() )
			continue;
		GuildHouseBuildingStruct& ChildBuildingInfo = Iter->second;
		GuildHouseBuildingInfoStruct& Building = ChildBuildingInfo.Info;

		GuildBuildingInfoTableStruct* BuildingTableInfo = g_ObjectData->GuildBuilding( ChildBuildingInfo.Info.BuildingID );
		if( BuildingTableInfo == NULL )
			continue;

		//取得參考點位置
		float X , Y , Z , Dir;
		bool IsBuild = false;
		CRuMatrix4x4 	matLocal;
		CRuMatrix4x4 	matWorld;

		bool Ret = MapPointStrPos( BuildingTableInfo , Building.ParentDBID , Building.PointStr , &matLocal , IsBuild );
		if( Ret == false )
			continue;


		CRuMatrix4x4 matRotation;
		matRotation.SetToRotationEulerXYZ( 0, (Building.Dir)*PI/180.0f, 0 );
		matLocal = matRotation * matLocal;
		matWorld = matLocal * (*(CRuMatrix4x4*)(&BuildingInfo.matWorld));

		//matLocal
		X = matWorld._41;
		Y = matWorld._42;
		Z = matWorld._43;

		CRuVector3 vecDir( 0 , 0 , -1 );
		vecDir = vecDir * matWorld;
		vecDir.x -=X;
		vecDir.y -=Y;
		vecDir.z -=Z;

		Dir = RoleDataEx::CalDir( vecDir.x , vecDir.z);

		memcpy( &(ChildBuildingInfo.matWorld) , &matWorld , sizeof(matWorld) );

		RoleDataEx* Role = Global::GetRoleDataByGUID( ChildBuildingInfo.ObjGUID );
		if( Role == NULL )
			continue;
		Role->Pos()->X = X;
		Role->Pos()->Y = Y;
		Role->Pos()->Z = Z;
		Role->Pos()->Dir = Dir;
		//ObjGUID
		Global::SetPos( Role->GUID() );

	}


	return true;
}