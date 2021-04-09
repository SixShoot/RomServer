#include "GuildHouseWarManage.h"
#include "../../netwalker_member/Net_BG_GuildWar/BG_GuildWarManage/BG_GuildWarManage.h"
#include "../Global.h"
#include "../../netwalker_member/NetWakerGSrvInc.h"
//////////////////////////////////////////////////////////////////////////
vector< GuildHouseWarManageClass* >			GuildHouseWarManageClass::_List;
map< int , GuildWarPlayerInfoStruct >		GuildHouseWarManageClass::_PlayerScoreMap;
vector< vector<GuildWarPlayerInfoStruct*> >	GuildHouseWarManageClass::_RoomPlayerScoreList;
//////////////////////////////////////////////////////////////////////////
GuildHouseWarManageClass::GuildHouseWarManageClass( GuildHouseWarFightStruct* WarInfo )
{
	//_WarBase = *WarInfo;
	_WarBase.Base[0].GuildID = WarInfo->Base[0].GuildID ;
	_WarBase.Base[1].GuildID = WarInfo->Base[1].GuildID ;
	_WarBase.IsAssignment = WarInfo->IsAssignment;

	_WarBase.IsReady = false;

	_List.push_back( this );
	_RoomID = (int)_List.size();

	GuildHousesManageClass* House1 = GuildHousesManageClass::CreateHouse( WarInfo->Base[0].GuildID , _RoomID );
	GuildHousesManageClass* House2 = GuildHousesManageClass::CreateHouse( WarInfo->Base[1].GuildID , _RoomID );

	if( House1 == NULL || House2 == NULL )
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//建立路徑處理機制
	PathManageClass::Room_CreatePath( _RoomID );
	//////////////////////////////////////////////////////////////////////////


	House1->SetPostion(		(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_X1" ) 
		,	(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_Y1" ) 
		,	(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_Z1" ) 
		,	(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_Dir1" ) );

	House2->SetPostion(		(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_X2" ) 
		,	(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_Y2" ) 
		,	(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_Z2" ) 
		,	(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_Dir2" ) );

	House1->SetPlayerPostion(	(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_Player_X1" ) 
		,	(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_Player_Y1" ) 
		,	(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_Player_Z1" ) 
		,	(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_Player_Dir1" ) );

	House2->SetPlayerPostion(	(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_Player_X2" ) 
		,	(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_Player_Y2" ) 
		,	(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_Player_Z2" ) 
		,	(float)g_ObjectData->GetSysKeyValue( "GuildHouseWar_Player_Dir2" ) );


	House1->SetCampID( EM_CampID_WF_A );
	House2->SetCampID( EM_CampID_WF_B );
	_WarBase.IsReady = true;


	_GuildHouse[0] = House1;
	_GuildHouse[1] = House2;

	Global::CopyRoomMonster( 0, _RoomID );
}
//////////////////////////////////////////////////////////////////////////
GuildHouseWarManageClass::~GuildHouseWarManageClass()
{
	_List[ _RoomID -1 ] = NULL;

}
//////////////////////////////////////////////////////////////////////////
void	GuildHouseWarManageClass::ClearAll()
{
	for( unsigned int i = 0 ; i < _List.size() ; i++ )
	{
		if( _List[i] != NULL )
			delete _List[i];
	}
	_List.clear();
	_PlayerScoreMap.clear();
	_RoomPlayerScoreList.clear();

}
//////////////////////////////////////////////////////////////////////////
void GuildHouseWarManageClass::OnTimeProcAll( )
{
	static int ProcessCount = 0;
	for( unsigned int i = 0 ; i < _List.size() ; i++ )	
	{
		GuildHouseWarManageClass* GuildWar = _List[i];
		if( GuildWar == NULL )
			continue;
		if( GuildWar->_WarBase.IsReady == false )
			continue;

		if( ProcessCount % 5 == 0 )
		{
			NetSrv_BG_GuildWar::SC_AllRoom_GuildScore( GuildWar->_RoomID , GuildWar->_WarBase  );
		}

		if( GuildWar->_WarBase.Base[0].Score == 0 || GuildWar->_WarBase.Base[1].Score == 0)
		{
			PlayerScoreProc( );
			GuildWar->WarEndProc(  );
		}
	}

	if( ProcessCount++ % 10 == 0 )
	{
		PlayerScoreProc( );
	}
}
//////////////////////////////////////////////////////////////////////////
void GuildHouseWarManageClass::PlayerScoreProc( )
{
	_RoomPlayerScoreList.clear();
	//計算所有玩家的分數
	BaseItemObject* PlayerObj;
	for( PlayerObj = BaseItemObject::GetByOrder_Player(true) ; PlayerObj != NULL ; PlayerObj = BaseItemObject::GetByOrder_Player() )
	{
		RoleDataEx* Role = PlayerObj->Role();
		GuildWarPlayerInfoStruct& Info = _PlayerScoreMap[ Role->DBID() ];
		Info.PlayerDBID 	= Role->DBID();
		Info.RoleName		= Role->RoleName();
		Info.GuildID		= Role->GuildID();
		Info.HitPoint		+= Role->PlayerTempData->BGInfo.iDamageVal;
		Info.KillPoint		+= Role->PlayerTempData->BGInfo.iKillVal;
		Info.HealPoint		+= Role->PlayerTempData->BGInfo.iHealVal;
		Info.DefPoint		+= Role->PlayerTempData->BGInfo.iDefVal;

		memset( &(Role->PlayerTempData->BGInfo) , 0 , sizeof( Role->PlayerTempData->BGInfo ) );
		
		if( (unsigned)Role->RoomID() > 1000 )
			continue;

		//更新 每一層 的角色分數
		while( Role->RoomID() >= (int)_RoomPlayerScoreList.size() )
		{
			vector<GuildWarPlayerInfoStruct*> Temp;
			_RoomPlayerScoreList.push_back( Temp );
		}
		_RoomPlayerScoreList[ Role->RoomID() ].push_back( &Info );
	
	}
/*
	for( PlayerObj = BaseItemObject::GetByOrder_Player(true) ; PlayerObj != NULL ; PlayerObj = BaseItemObject::GetByOrder_Player() )
	{
		RoleDataEx* Role = PlayerObj->Role();
		vector<GuildWarPlayerInfoStruct*>* PlayerList = GuildHouseWarManageClass::PlayerScoreList_ByRoomID( Role->RoomID() );

		for( unsigned i = 0 ; i < PlayerList->size() ; i++ )
		{
			NetSrv_BG_GuildWar::SC_PlayerInfoResult( Role->GUID() , PlayerList->size() , i , *((*PlayerList)[i]) );
		}
	}
*/
	for( unsigned i = 0 ; i < _RoomPlayerScoreList.size() ; i++ )
	{
		vector<GuildWarPlayerInfoStruct*>& PlayerList = _RoomPlayerScoreList[i];
		if( PlayerList.size() == 0 )
			continue;
		NetSrv_BG_GuildWar::SC_AllRoom_PlayerListInfo_Zip( i , PlayerList );

	}
}
//////////////////////////////////////////////////////////////////////////
GuildHouseWarManageClass*	GuildHouseWarManageClass::GetGuildWar_ByRoomID( int RoomID )
{
	if(	(unsigned)(RoomID-1) >= _List.size() )		
		return NULL;

	return _List[ RoomID-1 ];
}
//////////////////////////////////////////////////////////////////////////
void	GuildHouseWarManageClass::WarEndProc_All(  )
{
	OnTimeProcAll( );
	PlayerScoreProc( );
	for( unsigned int i = 0 ; i < _List.size() ; i++ )	
	{
		GuildHouseWarManageClass* GuildWar = _List[i];
		if( GuildWar == NULL )
			continue;
		if( GuildWar->_WarBase.IsReady == false )
			continue;

		GuildWar->WarEndProc( );		
	}


}
//////////////////////////////////////////////////////////////////////////
void	GuildHouseWarManageClass::WarEndProc( )
{
	if( _WarBase.IsEndWar != false )
		return;
	
	_WarBase.IsEndWar = true;

	map< int , GuildWarPlayerInfoStruct >::iterator Iter;
	_PlayerList[0].clear();
	_PlayerList[1].clear();

	for( Iter = _PlayerScoreMap.begin() ; Iter != _PlayerScoreMap.end() ; Iter++ )
	{
		if( Iter->second.GuildID == _WarBase.Base[0].GuildID )
		{
			_PlayerList[0].push_back( &(Iter->second) );
		}
		else if( Iter->second.GuildID == _WarBase.Base[1].GuildID )
		{
			_PlayerList[1].push_back( &(Iter->second) );
		}
	}
	char Buf[128];
	sprintf( Buf , "Event_GuildWarEnd(%d,%d)" , RoomID() , int(_WarBase.IsAssignment) );
	LUA_VMClass::PCallByStrArg( Buf , 0 ,0 );


	//通知積分改變
	int GuildID[2];
	int Score[2];
	int ToWorldId[2];
	GuildHouseWarInfoStruct* Info[2];

	for( int i = 0 ; i < 2 ; i++ )
	{
		GuildID[i] = _WarBase.Base[i].GuildID;
		Score[i] = _WarBase.Base[i].Score;
		ToWorldId[i] = GuildID[i] / 0x1000000;
		Info[i] = BG_GuildWarManageClass::GetGuildHouseWarInfo( GuildID[ i ] );
	}
	if( Score[0] > Score[1] )
	{
		if( _WarBase.IsAssignment == false )
		{
			int DScore = Info[1]->Score/10;
			Info[0]->Score += DScore;
			Info[1]->Score -= DScore;
			NetSrv_BG_GuildWar::SL_EndWarResult( ToWorldId[0]  , 401 , GuildID[0] , EM_GameResult_Win , DScore , Info[1] );
			NetSrv_BG_GuildWar::SL_EndWarResult( ToWorldId[1]  , 401 , GuildID[1] , EM_GameResult_Lost , DScore*-1 , Info[0] );
		}
		else
		{
			NetSrv_BG_GuildWar::SL_EndWarResult( ToWorldId[0]  , 401 , GuildID[0] , EM_GameResult_Win , 0 , Info[1] );
			NetSrv_BG_GuildWar::SL_EndWarResult( ToWorldId[1]  , 401 , GuildID[1] , EM_GameResult_Lost , 0 , Info[0] );
		}
	}
	else if( Score[0] < Score[1] )
	{
		if( _WarBase.IsAssignment == false )
		{
			int DScore = Info[0]->Score/10;
			Info[1]->Score += DScore;
			Info[0]->Score -= DScore;			
			NetSrv_BG_GuildWar::SL_EndWarResult( ToWorldId[0]  , 401 , GuildID[0] , EM_GameResult_Lost , DScore*-1 , Info[1] );
			NetSrv_BG_GuildWar::SL_EndWarResult( ToWorldId[1]  , 401 , GuildID[1] , EM_GameResult_Win , DScore , Info[0] );
		}
		else
		{
			NetSrv_BG_GuildWar::SL_EndWarResult( ToWorldId[0]  , 401 , GuildID[0] , EM_GameResult_Lost , 0 , Info[1] );
			NetSrv_BG_GuildWar::SL_EndWarResult( ToWorldId[1]  , 401 , GuildID[1] , EM_GameResult_Win , 0 , Info[0] );
		}
	}
	else
	{
		NetSrv_BG_GuildWar::SL_EndWarResult( ToWorldId[0]  , 401 , GuildID[0] , EM_GameResult_Even , 0 , Info[1] );
		NetSrv_BG_GuildWar::SL_EndWarResult( ToWorldId[1]  , 401 , GuildID[1] , EM_GameResult_Even , 0 , Info[0] );	
	}
	//////////////////////////////////////////////////////////////////////////
	int WinGuildID;
	int LostGuildID;

	if( Score[0] > Score[1] )
	{
		WinGuildID = GuildID[0];
		LostGuildID= GuildID[1];
	}
	else
	{
		LostGuildID= GuildID[0];
		WinGuildID = GuildID[1];
	}

	vector< GuildHouseWarInfoStruct >& List = BG_GuildWarManageClass::This()->GetList();
	for( unsigned i = 0 ; i < List.size() ; i++ )
	{
		if( Score[0] == Score[1] )
		{
			if(		List[i].GuildID == GuildID[0]
				||	List[i].GuildID == GuildID[1] )
				{
					List[i].State = EM_GuildHouseWarState_FightEnd_Even;
					continue;
				}
		}

		if( List[i].GuildID == LostGuildID ) 
			List[i].State = EM_GuildHouseWarState_FightEnd_Lost;

		if( List[i].GuildID == WinGuildID ) 
			List[i].State = EM_GuildHouseWarState_FightEnd_Win;
	}
	//////////////////////////////////////////////////////////////////////////
	//公會戰Log


	//////////////////////////////////////////////////////////////////////////
	//勝敗處理
	NetSrv_BG_GuildWar::SC_AllRoom_GuildScore( RoomID() , WarBase() );
	NetSrv_BG_GuildWar::SC_AllRoom_EndWar( RoomID() );
	
	//////////////////////////////////////////////////////////////////////////
	Global::Schedular()->Push( _WarEndDelayProc , 20*1000 , NULL 
						, "RoomID" 		, EM_ValueType_Int , RoomID()
						, NULL );
}
//////////////////////////////////////////////////////////////////////////
//戰事結束延後 讓玩家登出 與 清此區域
int GuildHouseWarManageClass::_WarEndDelayProc		( SchedularInfo* Obj , void* InputClass )   
{
	int RoomID 		= Obj->GetNumber("RoomID");
	GuildHouseWarManageClass* GuildWar = GuildHouseWarManageClass::GetGuildWar_ByRoomID( RoomID );
	if( GuildWar == NULL )
		return 0;
	//GuildHouseWarManageClass* GuildWar = (GuildHouseWarManageClass*)InputClass;
	//把所有某區 房間的玩家傳出，清除此方間的怪物
	BaseItemObject* PlayerObj;
	for( PlayerObj = BaseItemObject::GetByOrder_Player(true) ; PlayerObj != NULL ; PlayerObj = BaseItemObject::GetByOrder_Player() )
	{
		RoleDataEx* Role = PlayerObj->Role();
		
		if( Role->RoomID() == GuildWar->_RoomID )
		{
			NetSrv_CliConnectChild::WorldReturnNotifyProc( Role->GUID() );
			Role->LiveTime(10*1000 , "Return World"  );
			/*
			Global::ChangeZone( Role->GUID() 
				, Role->SelfData.ReturnZoneID , -1
				, Role->SelfData.ReturnPos.X
				, Role->SelfData.ReturnPos.Y
				, Role->SelfData.ReturnPos.Z	
				, Role->SelfData.ReturnPos.Dir		);
				*/
		}
	}
	Global::DelRoomMonster( GuildWar->_RoomID );

	return 0;
}
//////////////////////////////////////////////////////////////////////////
bool	GuildHouseWarManageClass::AddScore( int GuildID , int Score )
{
	return _WarBase.AddScore( GuildID , Score );
}
GuildWarPlayerInfoStruct& GuildHouseWarManageClass::PlayerScore( int DBID )
{
	static GuildWarPlayerInfoStruct StInfo;
	map< int , GuildWarPlayerInfoStruct >::iterator Iter;

	Iter = _PlayerScoreMap.find( DBID );
	if( Iter == _PlayerScoreMap.end() )
		return StInfo;

	return Iter->second;
}
int		GuildHouseWarManageClass::GetDataPos( int GuildID )
{
	for( int i = 0 ; i < 2 ; i++ )
	{
		if( _GuildHouse[i]->GetHouseBase()->GuildID == GuildID )
			return i;
	}
	return -1;
}
vector<GuildWarPlayerInfoStruct*>* GuildHouseWarManageClass::PlayerScoreList_ByRoomID( int RoomID )
{
	if( unsigned(RoomID) >= _RoomPlayerScoreList.size() )
		return NULL;
	return &_RoomPlayerScoreList[ RoomID ];
}
/*
void	GuildHouseWarManageClass::BeginFight( )
{
	//static vector< GuildHouseWarManageClass* >			_List;
	for( unsigned i = 0 ; i < _List.size() ; i++ )
	{
		_List[i]->_WarBase.BeginTime = RoleDataEx::G_Now_Float;
		_List[i]->_WarBase.EndTime = RoleDataEx::G_Now_Float + 1 / 24.0f;
	}
}
*/