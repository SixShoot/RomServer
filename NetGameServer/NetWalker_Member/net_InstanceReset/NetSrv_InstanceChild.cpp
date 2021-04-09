#include "../NetWakerGSrvInc.h"
#include "NetSrv_InstanceChild.h"
//-----------------------------------------------------------------
map< int , map< int , int > >	NetSrv_InstanceChild::_PartyKeyValue;	//PartyKeyValue[PartyKey][KeyID]
map< int , bool >				NetSrv_InstanceChild::_UpdatePartyLiveTime;
unsigned						NetSrv_InstanceChild::_InstanceLiveTime = 0;		//Instance 可存活時間
unsigned						NetSrv_InstanceChild::_NextInstanceLiveTime = 0;	//Instance 可存活時間
map< int , int >				NetSrv_InstanceChild::_PartyKeyMap;
WeekDayStruct					NetSrv_InstanceChild::_ResetDay;
int								NetSrv_InstanceChild::_MaxPartyKeyID = 1;
//-----------------------------------------------------------------
bool    NetSrv_InstanceChild::Init()
{
    NetSrv_Instance::_Init();

    if( This != NULL)
        return false;

	This = NEW NetSrv_InstanceChild;	

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_InstanceChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_Instance::_Release();

    delete This;
    This = NULL;

    return true;
}

void NetSrv_InstanceChild::RD_ResetTime( WeekDayStruct& ResetDay )
{
	
	_ResetDay = ResetDay;
	
	__time32_t				NowTime;
	struct	tm*				gmt;

	_time32( &NowTime );
	gmt = _localtime32( &NowTime );

	int	i = 0;
	for( i = 1 ; i < 7 ; i++ )
	{
		if( _ResetDay.Day[ (i+gmt->tm_wday)%7 ] != false )
			break;
	}

	gmt->tm_hour = 6;
	gmt->tm_sec = 0;
	gmt->tm_min = 0;
	gmt->tm_mday = gmt->tm_mday + i;

	_InstanceLiveTime = (int)_mktime32( gmt );

	Global::St()->LocalDBInfo.InstanceResetTime = TimeExchangeIntToFloat( _InstanceLiveTime + RoleDataEx::G_TimeZone * 60*60);

	//計算下下次的更新時間
	int tempday = i;
	for( i = 1 ; i < 7 ; i++ )
	{
		if( _ResetDay.Day[ (tempday+i+gmt->tm_wday)%7 ] != false )
			break;
	}
	gmt->tm_mday = gmt->tm_mday + i;
	_NextInstanceLiveTime = (int)_mktime32( gmt );

	Global::St()->LocalDBInfo.InstanceNextResetTime = TimeExchangeIntToFloat( _NextInstanceLiveTime  + RoleDataEx::G_TimeZone * 60*60 );	

}

void NetSrv_InstanceChild::RD_PlayInfo( int PartyKey , int KeyID , int KeyValue )
{
	if( _MaxPartyKeyID 	<= PartyKey )
		_MaxPartyKeyID = PartyKey + 1;
	_PartyKeyValue[PartyKey][KeyID] = KeyValue;
}

void NetSrv_InstanceChild::SetInstanceLiveTime( unsigned Time )
{
	_InstanceLiveTime = Time;
	CheckInstanceLiveTime();
}

void NetSrv_InstanceChild::CheckInstanceLiveTime( )
{
	if( Global::Ini()->InstanceSaveID == -1 )
		return;

	if( _InstanceLiveTime > RoleDataEx::G_Now )
		return;

	//////////////////////////////////////////////////////////////////////////
	//計算下次的處理時間
	__time32_t				NowTime;
	struct	tm*				gmt;

	_time32( &NowTime );
	gmt = _localtime32( &NowTime );

	int	i = 0;
	for( i = 1 ; i < 7 ; i++ )
	{
		if( _ResetDay.Day[ ( i+gmt->tm_wday)%7 ] != false )
			break;
	}
	
	gmt->tm_hour = 6 /*+ RoleDataEx::G_TimeZone*/;
	gmt->tm_sec = 0;
	gmt->tm_min = 0;
	gmt->tm_mday = gmt->tm_mday + i;

	_InstanceLiveTime = (int)_mktime32( gmt );

	Global::St()->LocalDBInfo.InstanceResetTime = TimeExchangeIntToFloat( _InstanceLiveTime + RoleDataEx::G_TimeZone * 60*60  );

	//計算下下次的更新時間
	int tempday = i;
	for( i = 1 ; i < 7 ; i++ )
	{
		if( _ResetDay.Day[ (tempday+i+gmt->tm_wday)%7 ] != false )
			break;
	}
	gmt->tm_mday = gmt->tm_mday + i;
	_NextInstanceLiveTime = (int)_mktime32( gmt );
	Global::St()->LocalDBInfo.InstanceNextResetTime = TimeExchangeIntToFloat( _NextInstanceLiveTime  + RoleDataEx::G_TimeZone * 60*60  );


	//////////////////////////////////////////////////////////////////////////
	//把角色全部傳出副本
	BaseItemObject* PlayerObj;
	for( PlayerObj = BaseItemObject::GetByOrder_Player(true) ; PlayerObj != NULL ; PlayerObj = BaseItemObject::GetByOrder_Player() )
	{
		RoleDataEx* Role = PlayerObj->Role();
		int ZoneID = GlobalBase::GetParallelZoneID( Ini()->RevZoneID ,  Role->PlayerTempData->ParallelZoneID );
		Global::ChangeZone( Role->GUID() , ZoneID , 0 , (float)Ini()->RevX , (float)Ini()->RevY , (float)Ini()->RevZ , 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	//清除資料
//	_PartyKeyValue.clear();
//	_PartyKeyMap.clear();
	//////////////////////////////////////////////////////////////////////////
	//送命令清除DB
//	char SqlCmd[512];
//	sprintf( SqlCmd , "DELETE InstancePlayState WHERE ZoneID = %d" , RoleDataEx::G_ZoneID );
//	Net_DCBase::SqlCommand( RoleDataEx::G_ZoneID , SqlCmd );
	//////////////////////////////////////////////////////////////////////////
	//清除所有副本
	//把超過設定時間的房間刪除
	for( int i = 1 ; i < Ini()->RoomCount ; i++ )
	{
		PrivateRoomInfoStruct& RoomInfo = Global::St()->PrivateRoomInfoList[i];

		if( RoomInfo.IsActive != false )
		{			
			RoomInfo.Init();
			DelRoomMonster( i );
		}
	}

	SC_AllPlayer_InstanceReset( Global::Ini()->InstanceSaveID );
}

void NetSrv_InstanceChild::RC_JoinInstanceResult( BaseItemObject* Player , bool Result )
{
	RoleDataEx* Owner = Player->Role();

	PrivateRoomInfoStruct& RoomInfo = Global::St()->PrivateRoomInfoList[ Owner->RoomID() ];
	int&	  KeyID			= Owner->PlayerBaseData->InstanceSetting.KeyID[ Global::Ini()->InstanceSaveID ];
	unsigned& LiveTime		= Owner->PlayerBaseData->InstanceSetting.LiveTime[ Global::Ini()->InstanceSaveID ];
	unsigned& ExtendTime	= Owner->PlayerBaseData->InstanceSetting.ExtendTime[ Global::Ini()->InstanceSaveID ];

	if(	Result == false &&	KeyID == -1 )
	{
		//把角色傳出去
		int ZoneID = GlobalBase::GetParallelZoneID( Ini()->RevZoneID ,  Owner->PlayerTempData->ParallelZoneID );
		if( ZoneID == -1 )
		{
			Global::ChangeZone( Owner->GUID() , Owner->SelfData.RevZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir );
			return;
		}
		Global::ChangeZone( Owner->GUID() , ZoneID , 0 , (float)Ini()->RevX , (float)Ini()->RevY , (float)Ini()->RevZ , 0 );
		return;
	}

	KeyID	 = RoomInfo.InstanceKeyID;
	LiveTime = NetSrv_InstanceChild::_InstanceLiveTime;
	ExtendTime = NetSrv_InstanceChild::_InstanceLiveTime;
	NetSrv_Instance::SC_ResetTime( Owner->GUID() , LiveTime , Global::Ini()->InstanceSaveID , KeyID , false ) ;
}

void NetSrv_InstanceChild::OnChatDisconnect( )
{
	_PartyKeyMap.clear();
}
void NetSrv_InstanceChild::OnChatConnect( )
{

}

//副本進度儲存展期處理
void NetSrv_InstanceChild::RC_InstanceExtend( BaseItemObject* PlayerObj , int ZoneID , int SavePos , int ExtendTime )
{
	RoleDataEx* Player = PlayerObj->Role();

	if( _MAX_INSTANCE_SETTING_COUNT_ <= SavePos )
		return;

	int&		KeyID		= Player->PlayerBaseData->InstanceSetting.KeyID[ SavePos ];
	unsigned&	LiveTime	= Player->PlayerBaseData->InstanceSetting.LiveTime[ SavePos ];

	SL_InstanceExtend( ZoneID , Player->DBID() , LiveTime , ExtendTime , KeyID , SavePos );
}
void NetSrv_InstanceChild::RL_InstanceExtend( int SrvSockID , int PlayerDBID , int ZoneID , int LiveTime , int ExtendTime , int KeyID , int SavePos )
{
	if( SavePos != Global::Ini()->InstanceSaveID )
		return;

	//查詢KeyID 是否存在
	if( _PartyKeyValue.end() == _PartyKeyValue.find( KeyID ))
	{
		SL_InstanceExtendResult( SrvSockID , PlayerDBID , ZoneID , 0 , 0 , SavePos , 0 );
		return;
	}

	if((unsigned int)LiveTime != _InstanceLiveTime )
		LiveTime = 0;

	if((unsigned int)ExtendTime < _InstanceLiveTime )
		ExtendTime = 0;

	SL_InstanceExtendResult( SrvSockID , PlayerDBID , ZoneID , ExtendTime , LiveTime , SavePos , KeyID );
}
void NetSrv_InstanceChild::RL_InstanceExtendResult( int PlayerDBID , int ZoneID , int ExtendTime , int LiveTime , int SavePos , int KeyID )
{
	RoleDataEx* Player = Global::GetRoleDataByDBID( PlayerDBID );

	if( Player == NULL )
		return;

	if( _MAX_INSTANCE_SETTING_COUNT_ <= SavePos )
		return; 

	Player->PlayerBaseData->InstanceSetting.KeyID[ SavePos ] = KeyID;
	Player->PlayerBaseData->InstanceSetting.LiveTime[ SavePos ] = LiveTime;
	Player->PlayerBaseData->InstanceSetting.ExtendTime[ SavePos ] = ExtendTime;

	SC_FixInstanceExtend( PlayerDBID , ZoneID , ExtendTime , LiveTime , KeyID );
}

void NetSrv_InstanceChild::RC_InstanceRestTimeRequest( BaseItemObject* PlayerObj  , int ZoneID , int SavePos )
{
	RoleDataEx* Player = PlayerObj->Role();
	if( Player == NULL )
		return;

	if( _MAX_INSTANCE_SETTING_COUNT_ <= SavePos )
		return;

	int&		KeyID		= Player->PlayerBaseData->InstanceSetting.KeyID[ SavePos ];

	SL_InstanceRestTimeRequest( ZoneID , Player->DBID() , KeyID );
}
void NetSrv_InstanceChild::RL_InstanceRestTimeRequest( int PlayerDBID , int KeyID )
{
	if( _PartyKeyValue.end() == _PartyKeyValue.find( KeyID ))
	{
		
		SC_InstanceRestTime( PlayerDBID , RoleDataEx::G_ZoneID , -1 , _InstanceLiveTime , _NextInstanceLiveTime );
		return;
	}
	SC_InstanceRestTime( PlayerDBID , RoleDataEx::G_ZoneID , KeyID , _InstanceLiveTime , _NextInstanceLiveTime );
}
//--------------------------------------------------------------------
void NetSrv_InstanceChild::RD_SetWorldVar( const char* pszVarName, int iVarValue )
{
	m_mapVar[ pszVarName ] = iVarValue;
}
//--------------------------------------------------------------------
void NetSrv_InstanceChild::RC_GetWorldVar( BaseItemObject* Player, const char* pszVarName )
{
	int iValue = 0;

	map< string, int >::iterator it = m_mapVar.find( pszVarName );

	if( it != m_mapVar.end() )
	{
		iValue = it->second;
	}

	PG_Instance_LtoC_GetWorldVarResult	Packet;

	Packet.VarName	= pszVarName;
	Packet.VarValue	= iValue;

	Global::SendToCli_ByGUID( Player->GUID() , sizeof(Packet) , &Packet );
}


void 	NetSrv_InstanceChild::SetWorldVar		( const char* pszVarName, int iVarValue )
{
	((NetSrv_InstanceChild*)This)->m_mapVar[ pszVarName ] = iVarValue;	

	PG_Instance_LtoD_SetWorldVar	Packet;

	Packet.VarName	= pszVarName;
	Packet.VarValue	= iVarValue;

	SendToDBCenter( sizeof(Packet), &Packet );
}

int 	NetSrv_InstanceChild::GetWorldVar		( const char* pszVarName )
{
	int iValue = 0;
	map< string, int >::iterator it = ((NetSrv_InstanceChild*)This)->m_mapVar.find( pszVarName );
	
	if( it != ((NetSrv_InstanceChild*)This)->m_mapVar.end() )
	{
		iValue = it->second;
	}

	return iValue;
}
