#include "BG_GuildWarManage.h"
#include "../../NetWakerGSrvInc.h"
#include "../../../mainproc/GuildHouseWarManage/GuildHouseWarManage.h"

#define  DEF_GUILDWAR_DB_PROCESS_ID	13

BG_GuildWarManageClass* BG_GuildWarManageClass::_This = NULL;
int  BG_GuildWarManageClass::_NextStatusTime = 0;

void BG_GuildWarManageClass::Init()
{
	if( _This != NULL )
		return;
	_This = NEW BG_GuildWarManageClass;
}
void BG_GuildWarManageClass::Release()
{
	if( _This != NULL )
	{
		delete _This;
		_This = NULL;
	}	
}

BG_GuildWarManageClass::BG_GuildWarManageClass( )
{

	_DebugTime_Hour = -1;
	_DebugTime_Day  = -1;

	RoleDataEx::G_Now = TimeStr::Now_Value() + RoleDataEx::SysDTime;

	//取得目前時間
	int NowTimeValue = ( RoleDataEx::G_Now / (60*60) + RoleDataEx::G_TimeZone - Global::Ini()->AdjustGuildWarTime )%4;	//戰爭時間

	switch( NowTimeValue )
	{
	case 0:
		_State = EM_GuildHouseWarState_Appointment;
		break;
	case 1:
		_State = EM_GuildHouseWarState_Prepare;
		break;
	case 2:
		_State = EM_GuildHouseWarState_Fight;
		break;
	case 3:
		_State = EM_GuildHouseWarState_FightEnd;
		break;

	}

	//_RDGuildHouseWarInfoSql	= NEW CreateDBCmdClass<GuildHouseWarInfoStruct> ( RoleDataEx::ReaderRD_GuildHouseWarInfo()  , "GuildHouseWar_Order" ) ;


	Global::Schedular()->Push( _OnTimeProc , 60000 , NULL , NULL );	
}

BG_GuildWarManageClass::~BG_GuildWarManageClass()
{
//	delete _RDGuildHouseWarInfoSql;
}


void	BG_GuildWarManageClass::Proc()
{
	static int  LProcessTime = ( RoleDataEx::G_Now/(60*60) + RoleDataEx::G_TimeZone - Global::Ini()->AdjustGuildWarTime )%4;
	//取得目前時間
	int NowTimeValue = ( RoleDataEx::G_Now/(60*60)  + RoleDataEx::G_TimeZone - Global::Ini()->AdjustGuildWarTime )%4;	//戰爭時間

	_NextStatusTime = 3600 - RoleDataEx::G_Now% (60*60) ;

	if( _DebugTime_Hour != -1 )
		NowTimeValue = _DebugTime_Hour % 4;

	if( LProcessTime == NowTimeValue )
		return;

	_NextStatusTime = 0;

	NowTimeValue = LProcessTime = (LProcessTime + 1)%4;
	 

	switch( NowTimeValue )
	{
	case 0:
		{
			BeginAppointment();
		}
		break;
	case 1:		//預約結束
		{
			if( _State == EM_GuildHouseWarState_Appointment )
				PerpareFight();
		}
		break;
	case 2:		//開戰
		{
			if( _State == EM_GuildHouseWarState_Appointment )
				PerpareFight();

			BeginFight();
		}
		break;
	case 3:	//公會戰結束
		{
			//通知Local 戰鬥結束
			EndFight();
		}
		break;
	}

	if(		_DebugTime_Hour != -1 
		&&	_DebugTime_Hour%4 != NowTimeValue )
		Proc();

	_NextStatusTime = 60*60;
}

int  BG_GuildWarManageClass::_OnTimeProc( SchedularInfo* Obj , void* InputClass )
{
	_This->Proc();
	Global::Schedular()->Push( _OnTimeProc , 1000*10 , NULL , NULL );	
	return 0;
}

bool	BG_GuildWarManageClass::WarRegister( int GuildID , int GuildLv , const char* GuildName , int Score , bool IsAcceptAssignment , int TargetGuild )
{
	if( EM_GuildHouseWarState_Appointment != _State )
		return false;

	for( int i = 0 ; i < (int)_List.size() ; i++ )
	{
		if( _List[i].GuildID == GuildID )
			return false;
	}

	GuildHouseWarInfoStruct Order;

	Order.ZoneGroupID	= RoleDataEx::G_ZoneID / 1000;
	Order.GuildID		= GuildID;
	Order.Score			= Score;	//積分
	Order.EnemyGuildID	= -1;		//對戰公會(-1表示沒有)
	Order.State			= EM_GuildHouseWarState_Appointment;
	Order.GuildName		= GuildName;
	Order.GuildLv		= GuildLv;
	Order.IsAcceptAssignment = IsAcceptAssignment;


	if( TargetGuild > 0 )
	{
		Order.IsAcceptAssignment = false;
		bool isFind = false;
		//檢查目標公會是否存在
		for( int i = 0 ; i < (int)_List.size() ; i++ )
		{
			GuildHouseWarInfoStruct& TOrder = _List[i];
			if( TOrder.GuildID == TargetGuild )
			{
				if( TOrder.IsAcceptAssignment && TOrder.EnemyGuildID == -1)
				{
					TOrder.EnemyGuildID = GuildID;
					Order.EnemyGuildID = TargetGuild;
					isFind = true;
				}
				break;
			}
		}

		if(isFind == false )
			return false;
	}

	_List.push_back( Order );
	return true;
}
bool	BG_GuildWarManageClass::WarCancel( int GuildID )
{
	if( EM_GuildHouseWarState_Appointment != _State )
		return false;

	int TGuild = 0;
	for( int i = 0 ; i < (int)_List.size() ; i++ )
	{
		GuildHouseWarInfoStruct& Order = _List[i];
		if( Order.GuildID == GuildID )
		{
			TGuild = Order.EnemyGuildID;
			_List.erase( _List.begin() + i );

			if( TGuild )
			{
				for( int j = 0 ; j < (int)_List.size() ; j++ )
				{
					GuildHouseWarInfoStruct& Order = _List[j];
					if( Order.GuildID == TGuild )
					{
						Order.EnemyGuildID = -1;
						if( Order.IsAcceptAssignment == false )
							WarCancel( Order.GuildID );
						break;
					}
				}
			}

			return true;
		}
	}
	return false;
}
bool	BG_GuildWarManageClass::PerpareFight( )
{
	map< int , vector< GuildHouseWarInfoStruct > > FightList;	
	vector< GuildHouseWarInfoStruct > OrderList;
	vector< GuildHouseWarInfoStruct > AssignFailedList;
	GuildHouseWarInfoStruct LastOrder;
	if( _State == EM_GuildHouseWarState_Appointment )
		NetSrv_BG_GuildWar::SL_AllWorld_Status( 1 , EM_GuildWarStatusType_Prepare , NextStatusTime() );

	_State = EM_GuildHouseWarState_Prepare;
	if( _List.size() == 0 )
		return false;

	for( unsigned i = 0 ; i < _List.size() ; i++ )
	{
		GuildHouseWarInfoStruct& Order = _List[i];
		if( Order.EnemyGuildID > 0 )
		{
			Order.IsAcceptAssignment = true;
			Order.State = EM_GuildHouseWarState_Prepare;

			if( Order.EnemyGuildID > Order.GuildID )
				FightList[ Order.EnemyGuildID].push_back( Order );
			else
				FightList[ Order.GuildID].push_back( Order );		
		}
		else
		{

			Order.State = EM_GuildHouseWarState_Failed;
			if( Order.IsAcceptAssignment == true )
				AssignFailedList.push_back( Order );
			else
				OrderList.push_back( Order );
		}
	}

	int FightGuildCount = (int)OrderList.size() & 0xfffe;
	sort( OrderList.begin() , OrderList.begin() + FightGuildCount );	
	_List.clear();


	for( int i = 0 ; i < FightGuildCount ; i+=2 )
	{
		if(		OrderList[i].EnemyGuildID != -1
			||	OrderList[i+1].EnemyGuildID != -1)						
			break;
		OrderList[i].EnemyGuildID = OrderList[i+1].GuildID;
		OrderList[i+1].EnemyGuildID = OrderList[i].GuildID;
		OrderList[i].State = EM_GuildHouseWarState_Prepare;
		OrderList[i+1].State = EM_GuildHouseWarState_Prepare;

		_List.push_back( OrderList[i] );
		_List.push_back( OrderList[i+1] );
	}

	{
		map< int , vector< GuildHouseWarInfoStruct > >::iterator iter;
		for( iter = FightList.begin() ; iter != FightList.end() ; iter++ )
		{
			vector< GuildHouseWarInfoStruct >& OrderV = iter->second;
			if( OrderV.size() != 2 )
			{
				Print( LV4 , "PerpareFight" , "OrderV.size() != 2" );
				continue;
			}
			_List.push_back( OrderV[0] );
			_List.push_back( OrderV[1] );

		}
	}

	for( unsigned i = 0 ; i < _List.size() ; i+=2 )
	{
		NetSrv_BG_GuildWarChild::WarPerpare	( _List[i].GuildID , _List[i+1].GuildID , _List[i].IsAcceptAssignment );
	}

	for( int i = 0 ; i < AssignFailedList.size() ; i++ )
	{
		_List.push_back( AssignFailedList[i]);
		
	}

	if( OrderList.size() && OrderList.back().State == EM_GuildHouseWarState_Failed )
	{
		_List.push_back( OrderList.back() );
	}


/*
	for( int i = 0 ; i < (int)_List.size() ; i++ )
	{
		char Buf[512];
		sprintf_s( Buf , sizeof(Buf) , "UPDATE GuildHouseWar_Order SET EnemyGuildID = %d , State=%d WHERE GuildID = %d" 
			, _List[i].EnemyGuildID , _List[i].State , _List[i].GuildID );
		Net_DCBase::SqlCommand( DEF_GUILDWAR_DB_PROCESS_ID , Buf );
	}
*/
	return true;
}
bool	BG_GuildWarManageClass::BeginFight( )
{

	if( EM_GuildHouseWarState_Prepare != _State )
		return false;

	NetSrv_BG_GuildWar::SL_AllWorld_Status( 1 , EM_GuildWarStatusType_Begin , NextStatusTime() );

	int FightGuildCount = (int)_List.size() & 0xfffe;
	for( int i = 0 ; i < FightGuildCount ; i+=2 )
	{
		_List[i].State = EM_GuildHouseWarState_Fight;
		_List[i+1].State = EM_GuildHouseWarState_Fight;		
	}
/*
	for( int i = 0 ; i < (int)_List.size() ; i++ )
	{
		char Buf[512];
		sprintf_s( Buf , sizeof(Buf) , "UPDATE GuildHouseWar_Order SET EnemyGuildID = %d , State=%d WHERE GuildID = %d" 
			, _List[i].EnemyGuildID , _List[i].State , _List[i].GuildID );
		Net_DCBase::SqlCommand( DEF_GUILDWAR_DB_PROCESS_ID , Buf );
	}
*/
	_State = EM_GuildHouseWarState_Fight;


	return true;
}
bool	BG_GuildWarManageClass::BeginAppointment()
{
	if( _State == EM_GuildHouseWarState_FightEnd )
		NetSrv_BG_GuildWar::SL_AllWorld_Status( 1 , EM_GuildWarStatusType_Appointment , NextStatusTime() );
	_List.clear();
	_State = EM_GuildHouseWarState_Appointment;
	return true;
}
bool	BG_GuildWarManageClass::EndFight()
{
//	char Buf[4096];
//	Net_DCBase::SqlCommand( DEF_GUILDWAR_DB_PROCESS_ID , "TRUNCATE TABLE GuildHouseWar_Order" );

	if( _State == EM_GuildHouseWarState_Fight )
		NetSrv_BG_GuildWar::SL_AllWorld_Status( 1 , EM_GuildWarStatusType_End , NextStatusTime() );

	Global::Schedular()->Push( _OnEvent_ClearGuildWar , 1000*20 , NULL, NULL );

	//通知所有孩在戰鬥的公會勝敗結果
	GuildHouseWarManageClass::WarEndProc_All();

	_State = EM_GuildHouseWarState_FightEnd;


	return true;
}

int BG_GuildWarManageClass::_OnEvent_ClearGuildWar	 ( SchedularInfo* Obj, void* InputClass )
{
	GuildHouseWarManageClass::ClearAll();
	GuildHousesManageClass::ClearAll();

	vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;

	for( unsigned i = 1 ; i < RoomList.size() ; i++ )
	{
		if( RoomList[i].IsActive != false )
		{
			RoomList[i].Init();
			Global::DelRoomMonster( i );

			PathManageClass::Room_ReleasePath( i );
		}		
	}

	
	BaseItemObject* PlayerObj;
	for( PlayerObj = BaseItemObject::GetByOrder_Player(true) ; PlayerObj != NULL ; PlayerObj = BaseItemObject::GetByOrder_Player() )
	{
		RoleDataEx* Role = PlayerObj->Role();
		NetSrv_CliConnectChild::WorldReturnNotifyProc( Role->GUID() );
		Role->LiveTime(10*1000 , "Return World"  );
	}
	return 0;			
}

GuildHouseWarInfoStruct* BG_GuildWarManageClass::GetGuildHouseWarInfo( int GuildID )
{
	vector< GuildHouseWarInfoStruct >&	List = This()->GetList();
	//取出此公會的資料
	for( int i = 0 ; i < (int)List.size() ; i++ )
	{
		if( List[i].GuildID == GuildID )
		{
			return &List[i];
		}
	}
	return NULL;
}
