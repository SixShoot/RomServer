#include "NetSrv_BG_GuildWarChild.h"
#include "../../mainproc/GuildManage/GuildManage.h"
#include "BG_GuildWarManage/BG_GuildWarManage.h"
#include "../../mainproc/GuildHouseManage/GuildHousesClass.h"
#include "../../mainproc/GuildHouseWarManage/GuildHouseWarManage.h"
#include "../NetWakerGSrvInc.h"
map<int , AllGuildRankInfoStruct >	NetSrv_BG_GuildWarChild::_AllWorldRankInfoMap;

bool    NetSrv_BG_GuildWarChild::Init()
{
	NetSrv_BG_GuildWar::_Init();

	if( This != NULL)
		return false;

	This = NEW( NetSrv_BG_GuildWarChild );

	
	if( Global::Ini()->IsBattleWorld && Global::Ini()->IsGuildHouseWarZone )
	{
		BG_GuildWarManageClass::Init();
	}

	return true;
}
//-----------------------------------------------------------------
bool    NetSrv_BG_GuildWarChild::Release()
{
	if( This == NULL )
		return false;

	if( Global::Ini()->IsBattleWorld && Global::Ini()->IsGuildHouseWarZone )
	{	
		BG_GuildWarManageClass::Release();
	}

	NetSrv_BG_GuildWar::_Release();

	delete This;
	This = NULL;

	return true;
}
//�n�D����A�����|�W�٭n�D
void NetSrv_BG_GuildWarChild::RC_GuildNameRequest( BaseItemObject* OwnerObj , int WorldGuildID )
{
	RoleDataEx* Owner = OwnerObj->Role();

	int WorldID = WorldGuildID / 0x1000000;
	SL_GuildNameRequest( WorldID , 1 , Owner->DBID() , WorldGuildID );
}
//�n�D����A�����|�W�٭n�D
void NetSrv_BG_GuildWarChild::RBL_GuildNameRequest( int FromWorldId , int FromNetID ,int PlayerDBID , int WorldGuildID )
{
	int GuildID = WorldGuildID & 0xffffff;

	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( GuildInfo == NULL )
		return;

	SBL_GuildNameResult( FromWorldId , FromNetID ,PlayerDBID , WorldGuildID , GuildInfo->Base.GuildName.Begin() );
	
}
//�n�D����A�����|�W�ٵ��G
void NetSrv_BG_GuildWarChild::RL_GuildNameResult( int FromWorldId , int FromNetID ,int PlayerDBID , int WorldGuildID , const char* GuildName )
{
	RoleDataEx* Role = Global::GetRoleDataByDBID( PlayerDBID );
	if( Role == NULL )
		return;

	SC_GuildNameResult( Role->GUID() , WorldGuildID , GuildName );
}

//�}�ҵ��U���|�Ԥ���
void NetSrv_BG_GuildWarChild::RC_OpenMenuRequest( BaseItemObject* OwnerObj )
{
	RoleDataEx* Owner = OwnerObj->Role();
	for( int i = 0 ; i < 10 ; i++ )
		SBL_OpenMenuRequest( _Def_BattleGround_WorldID_ , _Def_BattleGround_GuildWar_ZoneID_ + i * 1000 , Owner->DBID() );
}
//�n�D���|�Ը�T
void NetSrv_BG_GuildWarChild::RL_OpenMenuRequest( int FromWorldId , int FromNetID ,int PlayerDBID )
{
	vector< GuildHouseWarInfoStruct >& List = BG_GuildWarManageClass::This()->GetList();
	GuildHouseWarStateENUM State = BG_GuildWarManageClass::This()->State();
	SL_OpenMenuResult( FromWorldId , FromNetID , PlayerDBID , State , BG_GuildWarManageClass::This()->NextStatusTime() , List );
}
//���|�Զ}�Ҫ��p
void NetSrv_BG_GuildWarChild::RBL_OpenMenuResult( int FromWorldId , int FromNetID ,int PlayerDBID , int ZoneGroupID , int NextTime , GuildHouseWarStateENUM State , int Count , GuildHouseWarInfoStruct List[1000] )
{
	RoleDataEx* Owner = Global::GetRoleDataByDBID( PlayerDBID );
	if( Owner == NULL )
		return;

	SC_OpenMenuResult( Owner->GUID() , PlayerDBID , ZoneGroupID , State , NextTime , Count , List );

}
//���U���|��( or ���� )
void NetSrv_BG_GuildWarChild::RC_RegisterRequest( BaseItemObject* OwnerObj , int ZoneGroupID , GuildWarRegisterTypeENUM Type , int GuildCount , bool IsAcceptAssignment , int TargetGuild )
{
	RoleDataEx* Owner = OwnerObj->Role();
	
	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( Owner->GuildID() );
	if( GuildInfo == NULL || GuildInfo->Base.IsOwnHouse == false )
	{
		GuildHouseWarInfoStruct EmptyInfo;
		SC_RegisterResult( Owner->GUID() , Type , EmptyInfo , EM_GuildWarRegisterResult_NoGuildHouse );
		Owner->Msg("�S�����|��");
		return;
	}

	GuildMemberStruct* memberInfo =  GuildManageClass::This()->GetMember( Owner->DBID() );
	if( memberInfo == NULL )
	{
		Owner->Msg( "guild member not find" );
		return;
	}
	if( (unsigned)memberInfo->Rank >= EM_GuildRank_Count )
		return;

	GuildManageSettingStruct& rankSetting = GuildInfo->Base.Rank[ memberInfo->Rank ].Setting;

	if(		GuildInfo->Base.LeaderDBID != Owner->DBID() 
		&&	rankSetting.GuildWarRegister == false  )
	{
		GuildHouseWarInfoStruct EmptyInfo;
		SC_RegisterResult( Owner->GUID() , Type , EmptyInfo , EM_GuildWarRegisterResult_NotLeader );
		Owner->Msg("�u���|���i�H�ϥ�");
		return;
	}

	if( IsAcceptAssignment == false && Type == EM_GuildWarRegisterType_Register )
	{
		if((myUInt32)GuildInfo->Base.HousesWar.NextWarTime > RoleDataEx::G_Now )
		{
			GuildHouseWarInfoStruct EmptyInfo;
			SC_RegisterResult( Owner->GUID() , Type , EmptyInfo , EM_GuildWarRegisterResult_OnceaDay );
			Owner->Msg("�@�Ѥ@�����|��");
			return;
		}
	}

	//���|�Ԯɶ��ˬd
	int nowHour = ( RoleDataEx::G_Now/(60*60)  + RoleDataEx::G_TimeZone )%24;	//�Ԫ��ɶ�
	if(		nowHour >= Ini()->DiableGuildHouseWarTime_Begin 
		&&	nowHour < Ini()->DiableGuildHouseWarTime_End )
	{
		GuildHouseWarInfoStruct EmptyInfo;
		SC_RegisterResult( Owner->GUID() , Type , EmptyInfo , EM_GuildWarRegisterResult_GuildWarClose );
		return;
	}

	SBL_RegisterRequest( _Def_BattleGround_WorldID_ , _Def_BattleGround_GuildWar_ZoneID_ + 1000 * ZoneGroupID  , GuildInfo->Base.GuildID , GuildInfo->Base.Level , GuildInfo->Base.GuildName.Begin() , GuildInfo->Base.HousesWar.Score , Type , GuildCount , Owner->DBID() , IsAcceptAssignment , TargetGuild );
}

//���U���|��( or ���� )
void NetSrv_BG_GuildWarChild::RL_RegisterRequest( int FromWorldId , int FromNetID , GuildWarRegisterInfoStruct& Reg )
{
	int WorldGuildID = Reg.GuildID | ( FromWorldId * 0x1000000 );
	GuildHouseWarInfoStruct Info;
	vector< GuildHouseWarInfoStruct >&	List = BG_GuildWarManageClass::This()->GetList();
	bool Result = false;

	switch( Reg.Type )
	{
	case EM_GuildWarRegisterType_Register:
		if( Reg.GuildCount != List.size() )
		{
			SL_RegisterResult( FromWorldId , FromNetID , Reg.GuildID , Reg.Type , Info , EM_GuildWarRegisterResult_GuildCountErr , Reg.PlayerDBID );
			return;
		}
		if( List.size() > _MAX_GUILD_COUNT_GUILDWAR_ )
		{
			SL_RegisterResult( FromWorldId , FromNetID , Reg.GuildID , Reg.Type , Info , EM_GuildWarRegisterResult_Full , Reg.PlayerDBID );
			return;
		}


		Result = BG_GuildWarManageClass::This()->WarRegister( WorldGuildID , Reg.GuildLv , Reg.GuildName , Reg.Score , Reg.IsAcceptAssignment , Reg.TargetGuild );

		//���X�����|�����
		for( int i = 0 ; i < (int)List.size() ; i++ )
		{
			if( List[i].GuildID == WorldGuildID )
			{
				Info = List[i];
				break;
			}
		}
		break;
	case EM_GuildWarRegisterType_Cancel:
		//���X�����|�����
		for( int i = 0 ; i < (int)List.size() ; i++ )
		{
			if( List[i].GuildID == WorldGuildID )
			{
				Info = List[i];
				break;
			}
		}
		Result = BG_GuildWarManageClass::This()->WarCancel( WorldGuildID );
		break;		
	}
	if( Result != false )
		SL_RegisterResult( FromWorldId , FromNetID , Reg.GuildID , Reg.Type , Info , EM_GuildWarRegisterResult_OK , Reg.PlayerDBID );
	else
		SL_RegisterResult( FromWorldId , FromNetID , Reg.GuildID , Reg.Type , Info , EM_GuildWarRegisterResult_UnknowErr , Reg.PlayerDBID );
}

//���U���|��( or ���� )���G
void NetSrv_BG_GuildWarChild::RBL_RegisterResult( int FromWorldId , int FromNetID ,int GuildID , GuildWarRegisterTypeENUM Type , GuildHouseWarInfoStruct& Info , GuildWarRegisterResultENUM Result , int PlayerDBID )
{
	RoleDataEx* Leader = Global::GetRoleDataByDBID( PlayerDBID );
	if( Leader == NULL )
		return;

	SC_RegisterResult( Leader->GUID() , Type , Info , Result );
}
//���|�Ԫ��p( �}�l , �ǳ� , ���� )
void NetSrv_BG_GuildWarChild::RBL_Status( int FromWorldId , int FromNetID , GuildWarStatusTypeENUM Status , int NextTime )
{
	SC_AllPlayer_Status( Status , NextTime );
}

struct TempLoadGuildHouseInfo
{
	int FromWorldId;
	int FromNetID;
	int GuildID;
};

bool  NetSrv_BG_GuildWarChild::_SendHouseInfo( int FromWorldId , int FromNetID ,int GuildID )
{
	int OrgGuildID = GuildID & 0xffffff;

	GuildHousesManageClass* GuildHouse = GuildHousesManageClass::GetHouseObj( OrgGuildID );

	if( GuildHouse == NULL || GuildHouse->CheckLoadOK() == false )
	{
		return false;
	}

	SBL_HouseBaseInfo( FromWorldId , FromNetID , *(GuildHouse->GetHouseBase()) );

	//�e�ؿv���T��
	{
		map< int , GuildHouseBuildingStruct >&	BuildingMap = GuildHouse->BuildingMap( );
		map< int , GuildHouseBuildingStruct >::iterator Iter;
		for( Iter = BuildingMap.begin() ; Iter != BuildingMap.end() ; Iter++ )
		{
			SBL_HouseBuildingInfo( FromWorldId , FromNetID , Iter->second.Info );
		}
	}

	SBL_HouseLoadOK( FromWorldId , FromNetID , GuildID );
	return true;
}

int  NetSrv_BG_GuildWarChild::_LoadGuildHouseProc( SchedularInfo* Obj , void* InputClass )
{
	TempLoadGuildHouseInfo* TempData = (TempLoadGuildHouseInfo*)InputClass;
	int FromNetID = TempData->FromNetID;
	int FromWorldId = TempData->FromWorldId;
	int GuildID = TempData->GuildID;

	if( _SendHouseInfo( TempData->FromWorldId , TempData->FromNetID , TempData->GuildID ) != false )
	{
		delete TempData;
	}
	else
	{
		Global::Schedular()->Push( _LoadGuildHouseProc , 2000 , TempData , NULL );
	}
	return 0;
}
//�n�D���|�θ��(��zone401�����)
void NetSrv_BG_GuildWarChild::RBL_HouseLoadRequest( int FromWorldId , int FromNetID ,int GuildID )
{

	int OrgGuildID = GuildID & 0xffffff;
	GuildHousesManageClass* GuildHouse = GuildHousesManageClass::GetHouseObj( OrgGuildID );
	if( GuildHouse == NULL )
	{
		int RoomID = -1;
		vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;
		for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
		{
			if( RoomList[i].IsActive == false )
			{
				RoomList[i].IsActive = true;
				RoomID = i;
				break;
			}
		}

		if( RoomID == -1 )
		{
			Print( LV5 , "RBL_HouseLoadRequest" , "RoomID == -1 room not find" );
			return;
		}

		GuildHouse = GuildHousesManageClass::CreateHouse( OrgGuildID , RoomID );

		NetSrv_GuildHouses::SD_HouseInfoLoading	( OrgGuildID );

		TempLoadGuildHouseInfo* TempData = NEW TempLoadGuildHouseInfo;
		TempData->FromNetID = FromNetID;
		TempData->FromWorldId = FromWorldId;
		TempData->GuildID = GuildID;
		Global::Schedular()->Push( _LoadGuildHouseProc , 2000 , TempData , NULL );
	}
	else
	{
		if( _SendHouseInfo( FromWorldId , FromNetID ,GuildID ) == false )
		{
			TempLoadGuildHouseInfo* TempData = NEW TempLoadGuildHouseInfo;
			TempData->FromNetID = FromNetID;
			TempData->FromWorldId = FromWorldId;
			TempData->GuildID = GuildID;
			Global::Schedular()->Push( _LoadGuildHouseProc , 2000 , TempData , NULL );
		}
	}

}
//���|�ΰ򥻸��
void NetSrv_BG_GuildWarChild::RL_HouseBaseInfo( int FromWorldId , int FromNetID ,GuildHousesInfoStruct& HouseBaseInfo )
{
	HouseBaseInfo.GuildID = ( HouseBaseInfo.GuildID & 0xffffff ) + FromWorldId * 0x1000000;
	GuildHousesManageClass* house = GuildHousesManageClass::GetHouseObj( HouseBaseInfo.GuildID );

	if( house == NULL )
		return;

	if( house->CheckLoadOK() != false )
	{
		Print( LV3 , "RL_HouseBaseInfo" , "House info load already" );
		return;
	}

	
	house->SetHouseBase( HouseBaseInfo );
}
//���|�Ϋؿv���
void NetSrv_BG_GuildWarChild::RL_HouseBuildingInfo( int FromWorldId , int FromNetID ,GuildHouseBuildingInfoStruct Building )
{
	Building.GuildID = ( Building.GuildID & 0xffffff ) + FromWorldId * 0x1000000;
	GuildHousesManageClass* house = GuildHousesManageClass::GetHouseObj( Building.GuildID );

	if( house == NULL )
		return;
	if( house->CheckLoadOK() != false )
		return;

	house->AddBuilding( Building );
}
//���|�ήa����
void NetSrv_BG_GuildWarChild::RL_HouseFurnitureInfo( int FromWorldId , int FromNetID ,GuildHouseFurnitureItemStruct Item )
{
	Item.GuildID = ( Item.GuildID & 0xffffff ) + FromWorldId * 0x1000000;
	GuildHousesManageClass* house = GuildHousesManageClass::GetHouseObj( Item.GuildID );

	if( house == NULL )
		return;
	if( house->CheckLoadOK() != false )
		return;

	house->AddFurniture( Item );
}
//���|�θ��Ū������
void NetSrv_BG_GuildWarChild::RL_HouseLoadOK( int FromWorldId , int FromNetID ,int GuildID )
{
	GuildID = ( GuildID & 0xffffff ) + FromWorldId * 0x1000000;
	GuildHousesManageClass* house = GuildHousesManageClass::GetHouseObj( GuildID );

	if( house == NULL )
		return;
	if( house->CheckLoadOK() != false )
		return;

	house->LoadOK();


	//////////////////////////////////////////////////////////////////////////
	//�p�G��Ӥ��|�γ����J�����h�]�@�Ӥ��|�ξԪ�l�@��
	GuildHouseWarInfoStruct* Info = BG_GuildWarManageClass::GetGuildHouseWarInfo( GuildID );
	if( Info == NULL )
		return;
		
	GuildHousesManageClass* enemyHouse = GuildHousesManageClass::GetHouseObj( Info->EnemyGuildID );
	if( enemyHouse == NULL || enemyHouse->CheckLoadOK() == false )
		return;

	char Buf[128];
	sprintf( Buf , "Event_GuildWarInit(%d)" , enemyHouse->RoomID() );
	LUA_VMClass::PCallByStrArg( Buf , 0 ,0 );
	//////////////////////////////////////////////////////////////////////////
	//�䦹�h�Ҧ�����O�_���n�B�z���|��
	set<BaseItemObject* >&	NpcObjSet = *(BaseItemObject::NPCObjSet());
	set< BaseItemObject*>::iterator   NpcObjIter;
	for( NpcObjIter = NpcObjSet.begin() ; NpcObjIter != NpcObjSet.end() ; NpcObjIter++ )
	{
		BaseItemObject* Obj = *NpcObjIter;
		if( Obj == NULL )
			continue;

		if( Obj->Role()->RoomID() != enemyHouse->RoomID() )
			continue;
		GameObjDbStructEx* ObjDB = Global::GetObjDB( Obj->Role()->BaseData.ItemInfo.OrgObjID );
		if( ObjDB->IsNPC() == false || ObjDB->NPC.ReCalPath == false )
			continue;
		//���o�ϧθ��
		GameObjDbStructEx* ImageObjDB = Global::GetObjDB( ObjDB->ImageID );
		if( ImageObjDB == NULL )
			continue;

		PathManageClass::Room_AddModel( enemyHouse->RoomID() 
							, Obj->Role()->X() 
							, Obj->Role()->Y() 
							, Obj->Role()->Z() 
							, Obj->Role()->Dir() 
							, ImageObjDB->Image.ACTWorld );
	}

	//////////////////////////////////////////////////////////////////////////
	PathManageClass::Room_BuildPath( house->RoomID() );

}
//�n�D�i�J���|�Գ�
void NetSrv_BG_GuildWarChild::RC_EnterRequest( BaseItemObject* OwnerObj , int ZoneGroupID )
{
	RoleDataEx* Owner = OwnerObj->Role();
	
	if( RoleDataEx::G_ZoneID % 1000 >= 100 )
	{
		SC_EnterResult( Owner->DBID() , false );
		return;
	}

	//�����@���ˬd
	bool Ret = CheckLuaScript( Owner->GUID() , Owner->GUID() , "CheckEnterGuildWar()" );
	if( Ret == false )
	{
		SC_EnterResult( Owner->DBID() , false );
		return;
	}

	SBL_EnterRequest( _Def_BattleGround_WorldID_ , _Def_BattleGround_GuildWar_ZoneID_ + ZoneGroupID * 1000 , Owner->DBID() , Owner->GuildID() );
}
//�n�D�i�J���|�Գ�
void NetSrv_BG_GuildWarChild::RL_EnterRequest( int FromWorldId , int FromNetID ,int PlayerDBID , int GuildID )
{
	if( BG_GuildWarManageClass::This()->State() != EM_GuildHouseWarState_Fight )
	{
		SL_EnterResult( FromWorldId , FromNetID , PlayerDBID , GuildID , false );
		return;
	}

	GuildID = ( GuildID & 0xffffff ) + FromWorldId * 0x1000000 ;
	GuildHousesManageClass* house = GuildHousesManageClass::GetHouseObj( GuildID );
	if( house == NULL )
	{
		SL_EnterResult( FromWorldId , FromNetID , PlayerDBID , GuildID , false );
		return;
	}

	SL_EnterResult( FromWorldId , FromNetID , PlayerDBID , GuildID , true );
	return;
}
//�n�D�i�J���G
void NetSrv_BG_GuildWarChild::RBL_EnterResult( int FromWorldId , int FromNetID , int ZoneGroupID , int PlayerDBID , int GuildID , bool Result )
{
	RoleDataEx* Role = Global::GetRoleDataByDBID( PlayerDBID );
	if( Role == NULL )
		return;

	SC_EnterResult( PlayerDBID , Result );

	if( Result != false )
	{
		if( RoleDataEx::G_ZoneID % 1000 > 100 )
		{
			Global::ChangeWorld( Role->GUID(), Role, Role->SelfData.ReturnZoneID, Role->SelfData.ReturnPos, _Def_BattleGround_WorldID_ , _Def_BattleGround_GuildWar_ZoneID_ + 1000 * ZoneGroupID , -1 , 0 , 0 , 0 ,0 );
		}
		else
		{
			Global::ChangeWorld( Role->GUID(), Role, RoleDataEx::G_ZoneID , *( Role->Pos() ) , _Def_BattleGround_WorldID_ , _Def_BattleGround_GuildWar_ZoneID_ + 1000 * ZoneGroupID , -1 , 0 , 0 , 0 ,0 );
		}
	}

}
//�n�D�i�J���G
void NetSrv_BG_GuildWarChild::RC_LeaveRequest( BaseItemObject* OwnerObj )
{
	RoleDataEx* Role = OwnerObj->Role();
	//�Ǧ^��Ӫ�Server
	NetSrv_CliConnectChild::WorldReturnNotifyProc( Role->GUID() );
	Role->LiveTime(10*1000 , "Return World"  );
}
//////////////////////////////////////////////////////////////////////////
struct TempBGLoadGuildHouseStruct
{
	int	GuildID;
	int CountDown;
};

int  NetSrv_BG_GuildWarChild::_BG_LoadGuildHouseProc( SchedularInfo* Obj , void* InputClass )
{
	TempBGLoadGuildHouseStruct* TempData = (TempBGLoadGuildHouseStruct*)InputClass;

	//�ݤ��|�θ�ƬO�_���J����
	GuildHousesManageClass * GuildHouse = GuildHousesManageClass::GetHouseObj( TempData->GuildID );

	if( GuildHouse == NULL )
		return 0;

	if( GuildHouse->CheckLoadOK() == false )
	{
		TempData->CountDown --;
		if( TempData->CountDown < 0 )
		{
			Print( LV5 , "_BG_LoadGuildHouseProc" , "Load guild house error"  );
			return 0;
		}
		int GuildID = TempData->GuildID & 0xffffff;
		int ToWorldID = TempData->GuildID / 0x1000000;
		SL_HouseLoadRequest( ToWorldID , 401 , GuildID );
		Global::Schedular()->Push( _BG_LoadGuildHouseProc , 60*1000 , TempData , NULL );
	}
	Print( LV2 , "_BG_LoadGuildHouseProc" , "Load guild house OK"  );
	return 0;
}


void NetSrv_BG_GuildWarChild::WarPerpare		( int GuildID1 , int GuildID2 , bool IsAssignment )
{
	GuildHouseWarFightStruct TempInfo;
	TempInfo.Base[0].GuildID = GuildID1;
	TempInfo.Base[1].GuildID = GuildID2;
	TempInfo.IsAssignment = IsAssignment;

	NEW GuildHouseWarManageClass( &TempInfo );

	for( int i = 0 ; i < 2 ; i++ )
	{
		TempBGLoadGuildHouseStruct* TempData = NEW TempBGLoadGuildHouseStruct;
		TempData->CountDown = 10;
		TempData->GuildID = TempInfo.Base[i].GuildID;
		Global::Schedular()->Push( _BG_LoadGuildHouseProc , 100 , TempData , NULL );
	}

	
}
//�]�w�ثe���|�Ԯɶ�
void  NetSrv_BG_GuildWarChild::RL_DebugTime( int FromWorldId , int FromNetID , int Time_Day , int Time_Hour )
{
	BG_GuildWarManageClass::This()->_DebugTime_Day = Time_Day;
	BG_GuildWarManageClass::This()->_DebugTime_Hour = Time_Hour %24;
}

//�q���Y�H�Գ�������
void NetSrv_BG_GuildWarChild::SC_GuildScore_ByRoomID( int SendToID , int RoomID )
{
	GuildHouseWarManageClass* GuildWar = GuildHouseWarManageClass::GetGuildWar_ByRoomID( RoomID );
	SC_GuildScore( SendToID , GuildWar->WarBase() );
}
//Client �ݭn�D���|�Գ����a���
void NetSrv_BG_GuildWarChild::RC_PlayerInfoRequest( BaseItemObject* Obj )
{

}

//���|�Ե��G
void NetSrv_BG_GuildWarChild::RBL_EndWarResult( int WorldGuildID , GameResultENUM GameResult , int GuildScore , GuildWarBaseInfoStruct& TargetGuild )
{

	int GuildID = WorldGuildID & 0xffffff;
	NetSrv_Guild::SD_EndWarResult( GuildID , GameResult , GuildScore , TargetGuild );

}
void NetSrv_BG_GuildWarChild::RD_OrderInfo( int Count , GuildHouseWarInfoStruct List[] )
{
	vector< GuildHouseWarInfoStruct >& GuildWarList = BG_GuildWarManageClass::This()->GetList();

	if( BG_GuildWarManageClass::This() == NULL  )
		return;

	GuildHouseWarStateENUM State = BG_GuildWarManageClass::This()->State();

	if( State == EM_GuildHouseWarState_Appointment )
	{
		GuildWarList.clear();
		for( int i = 0 ; i < Count ; i++ )
		{
			if( List[i].State != EM_GuildHouseWarState_Appointment ) 
				continue;
			GuildWarList.push_back( List[i] );
		}
	}
	else if( State == EM_GuildHouseWarState_Prepare )
	{
		GuildWarList.clear();
		for( int i = 0 ; i < Count ; i++ )
		{
			if(		List[i].State != EM_GuildHouseWarState_Appointment )
				continue;

			GuildWarList.push_back( List[i] );		
		}
		State = EM_GuildHouseWarState_Appointment;
		BG_GuildWarManageClass::This()->PerpareFight();
	}

}

void NetSrv_BG_GuildWarChild::OnDataCenterConnect( )
{
/*	if( RoleDataEx::G_ZoneID % 1000 != _Def_BattleGround_GuildWar_ZoneID_ )
		return;
	SD_LoadInfo( RoleDataEx::G_ZoneID / 1000 );
	*/
}

void NetSrv_BG_GuildWarChild::RD_RegGuildWarRankInfo( GuildWarRankInfoStruct& Info , GuildHouseWarHistoryStruct Hisotry[50] )
{
	AllGuildRankInfoStruct temp;
	temp.Info = Info;
	memcpy( temp.Hisotry , Hisotry , sizeof( temp.Hisotry ) );
	_AllWorldRankInfoMap[Info.WorldGuildID] = temp;
}
void NetSrv_BG_GuildWarChild::RC_GuildWarRankInfoRequest( BaseItemObject* Obj , GuildWarRankInfoTypeENUM Type , int WorldGuildID )
{
	RoleDataEx* Role = Obj->Role();
	SBL_GuildWarRankInfoRequest( Role->DBID() , WorldGuildID , Type );
}
void NetSrv_BG_GuildWarChild::RL_GuildWarRankInfoRequest( int FromWorldId , int FromNetID , int PlayerDBID , GuildWarRankInfoTypeENUM Type , int WorldGuildID )
{
	map<int , AllGuildRankInfoStruct >::iterator iter;

	switch ( Type )
	{
	case EM_GuildWarRankInfoType_RankList:
		{
			for( iter = _AllWorldRankInfoMap.begin() ; iter != _AllWorldRankInfoMap.end() ; iter++ )
			{
				SL_GuildWarRankInfo( FromWorldId , FromNetID , PlayerDBID , iter->second.Info );
			}
		}
		break;
	case EM_GuildWarRankInfoType_History:
		{
			iter = _AllWorldRankInfoMap.find( WorldGuildID );
			if( iter == _AllWorldRankInfoMap.end() )
				break;
			SL_GuildWarHisotry( FromWorldId , FromNetID , PlayerDBID , iter->second.Hisotry );
		}
		break;
	}
}
void NetSrv_BG_GuildWarChild::RBL_GuildWarHisotry( int PlayerDBID , GuildHouseWarHistoryStruct Hisotry[50] )
{
	SC_GuildWarHisotry( PlayerDBID , Hisotry );
}
void NetSrv_BG_GuildWarChild::RBL_GuildWarRankInfo( GuildWarRankInfoStruct& Info , int PlayerDBID )
{
	SC_GuildWarRankInfo( PlayerDBID , Info );
}