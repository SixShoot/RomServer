/*
#include "GuildHousesWarManageClass.h"
#include "../GuildManage/GuildManage.h"
#include "../Global.h"
#include "../../NetWalker_Member/Net_GuildHousesWar/NetDC_GuildHousesWarChild.h"


vector< GuildHouseWarInfoStruct >		GuildHousesWarManageClass::_List;
//vector< GuildHouseWarHistoryStruct >	GuildHousesWarManageClass::_HistoryList;
//bool									GuildHousesWarManageClass::_IsAppointment = false;
GuildHouseWarStateENUM					GuildHousesWarManageClass::_State = EM_GuildHouseWarState_None;
int										GuildHousesWarManageClass::DebugTime_Hour = -1;
int										GuildHousesWarManageClass::DebugTime_Day = -1;

void	GuildHousesWarManageClass::Init()
{
	RoleDataEx::G_Now = TimeStr::Now_Value() + RoleDataEx::SysDTime;

	//取得目前時間
	int NowTimeValue = ( RoleDataEx::G_Now / (60*60) + RoleDataEx::G_TimeZone )%24;	//戰爭時間

	switch( NowTimeValue )
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
		_State = EM_GuildHouseWarState_Prepare;
		break;
	case 20:
		_State = EM_GuildHouseWarState_Fight;
		_List.clear();
		break;
	case 21:
	case 22:
	case 23:
		_State = EM_GuildHouseWarState_Appointment;
		break;
	}

	Global::Schedular()->Push( _OnTimeProc , 1000 , NULL , NULL );	
}

bool	GuildHousesWarManageClass::WarRegister( int GuildID )
{
	//if( _IsAppointment == false )
		//return false;
	if( EM_GuildHouseWarState_Appointment != _State )
		return false;

	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( Guild == NULL || Guild->Base.IsOwnHouse == false )
		return false;

	for( int i = 0 ; i < (int)_List.size() ; i++ )
	{
		if( _List[i].GuildID == GuildID )
			return false;
	}

	GuildHouseWarInfoStruct Order;

	//int NextDay			= ( 1 + RoleDataEx::G_Now/(60*60*24) ) * (60*60*24) + ( RoleDataEx::G_TimeZone + 20)*60*60;	//戰爭時間

	Order.GuildID		= GuildID;
	//Order.FightTime		= TimeExchangeIntToFloat( NextDay );		//戰爭時間
	Order.Score			= Guild->Base.HousesWar.Score;				//積分
	Order.EnemyGuildID	= -1;		//對戰公會(-1表示沒有)
	Order.State			= EM_GuildHouseWarState_Appointment;

	_List.push_back( Order );

	Global::_RD_NormalDB->SQLCMDWrite( 13 , NULL , NULL , (char*)Global::_RDGuildHouseWarInfoSql->GetInsertStr( &Order ).c_str() );

	return true;
}

bool	GuildHousesWarManageClass::WarCancel( int GuildID )
{
	if( EM_GuildHouseWarState_Appointment != _State )
		return false;

	for( int i = 0 ; i < (int)_List.size() ; i++ )
	{
		if( _List[i].GuildID == GuildID )
		{
			_List.erase( _List.begin() + i );
			char Buf[512];
			sprintf_s( Buf , sizeof(Buf) , "DELETE GuildHouseWar_Order WHERE GuildID = %d" , GuildID );
			Global::_RD_NormalDB->SQLCMDWrite( 13 , NULL , NULL , Buf );
			return true;
		}
	}
	return false;
}

bool	GuildHousesWarManageClass::BeginFight( )
{
	if( EM_GuildHouseWarState_Prepare != _State )
		return false;

	int FightGuildCount = (int)_List.size() & 0xfffe;
	for( int i = 0 ; i < FightGuildCount ; i+=2 )
	{
		_List[i].State = EM_GuildHouseWarState_Fight;
		_List[i+1].State = EM_GuildHouseWarState_Fight;

		CNetDC_GuildHousesWar::SL_WarBegin( _List[i].GuildID , _List[i+1].GuildID );
	}

	for( int i = 0 ; i < (int)_List.size() ; i++ )
	{
		char Buf[512];
		sprintf_s( Buf , sizeof(Buf) , "UPDATE GuildHouseWar_Order SET EnemyGuildID = %d , State=%d WHERE GuildID = %d" 
			, _List[i].EnemyGuildID , _List[i].State , _List[i].GuildID );
		Global::_RD_NormalDB->SQLCMDWrite( 13 , NULL , NULL , Buf );
	}

	_State = EM_GuildHouseWarState_Fight;
	return true;
}

bool	GuildHousesWarManageClass::PerpareFight( )
{
	_State = EM_GuildHouseWarState_Prepare;
	if( _List.size() == 0 )
		return false;


	_List.back().State = EM_GuildHouseWarState_Failed;
	int FightGuildCount = (int)_List.size() & 0xfffe;
	sort( _List.begin() , _List.begin() + FightGuildCount );	

	for( int i = 0 ; i < FightGuildCount ; i+=2 )
	{
		_List[i].EnemyGuildID = _List[i+1].GuildID;
		_List[i+1].EnemyGuildID = _List[i].GuildID;
		_List[i].State = EM_GuildHouseWarState_Prepare;
		_List[i+1].State = EM_GuildHouseWarState_Prepare;
	}

	for( int i = 0 ; i < (int)_List.size() ; i++ )
	{
		char Buf[512];
		sprintf_s( Buf , sizeof(Buf) , "UPDATE GuildHouseWar_Order SET EnemyGuildID = %d , State=%d WHERE GuildID = %d" 
						, _List[i].EnemyGuildID , _List[i].State , _List[i].GuildID );
		Global::_RD_NormalDB->SQLCMDWrite( 13 , NULL , NULL , Buf );
	}
	
	return true;
}

bool	GuildHousesWarManageClass::EndFight()
{
	char Buf[4096];
	Global::_RD_NormalDB->SQLCMDWrite( 13 , NULL , NULL , "TRUNCATE TABLE GuildHouseWar_Order" );

//	if( _List.size() >= 2 )
//		CNetDC_GuildHousesWar::SL_WarEnd( );

	_List.clear();
	_State = EM_GuildHouseWarState_Appointment;

	//////////////////////////////////////////////////////////////////////////
	//如果是星期六則戰完清除積分
	//////////////////////////////////////////////////////////////////////////
	int NowTimeDay = ( RoleDataEx::G_Now/(60*60)  + RoleDataEx::G_TimeZone )/24 % 7 ;	//戰爭時間
	if( DebugTime_Day != -1 )
		NowTimeDay = DebugTime_Day;

	if( NowTimeDay == 6 )	
	{

		int NowTime			= RoleDataEx::G_Now + RoleDataEx::G_TimeZone*60*60;	//戰爭時間

//		Order.GuildID		= GuildID;
//		Order.FightTime		= TimeExchangeIntToFloat( NextDay );		//戰爭時間

		int MaxGuild = GuildManageClass::This()->MaxGuildID();

		for( int i = 0 ; i < MaxGuild ; i++ )
		{
			GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( i );
			if( Guild == NULL )
				continue;

			if(		Guild->Base.HousesWar.EvenCount == 0 
				&&	Guild->Base.HousesWar.WinCount == 0 
				&&	Guild->Base.HousesWar.LostCount == 0 )
				continue;


			sprintf_s( Buf , sizeof(Buf) , "INSERT GuildHouseWar(GuildID,FightTime,Score,WinCount,LostCount,EvenCount) VALUES(%d,%f,%d%d%d%d)" 
				, Guild->Base.GuildID 
				, TimeExchangeIntToFloat( NowTime )
				, Guild->Base.HousesWar.Score
				, Guild->Base.HousesWar.WinCount
				, Guild->Base.HousesWar.LostCount
				, Guild->Base.HousesWar.EvenCount	);
			Global::_RD_NormalDB->SQLCMDWrite( rand() , NULL , NULL , Buf );

			Guild->Base.HousesWar.EvenCount = 0;
			Guild->Base.HousesWar.WinCount  = 0; 
			Guild->Base.HousesWar.LostCount = 0;
			Guild->Base.HousesWar.Score		= 1000;
		}

		sprintf_s( Buf , sizeof(Buf) , "UPDATE Guild_Base SET HouesWar_WinCount = 0 , HouesWar_LostCount  = 0 , HouesWar_EvenCount = 0 , HouesWar_Score = 1000" );
		Global::_RD_NormalDB->SQLCMDWrite( rand() , NULL , NULL , Buf );

	}
	//////////////////////////////////////////////////////////////////////////
	return true;
}

#define Def_GuildHouseWar_OnTimeProc_Time  10	
//void	GuildHousesWarManageClass::_OnTimeProc()
int  GuildHousesWarManageClass::_OnTimeProc( SchedularInfo* Obj , void* InputClass )
{
	Proc();
	Global::Schedular()->Push( _OnTimeProc , 1000*10 , NULL , NULL );	
	return 0;
}

void	GuildHousesWarManageClass::Proc( )
{
	static int  LProcessTime = ( RoleDataEx::G_Now/(60*60) + RoleDataEx::G_TimeZone )%24;
	//取得目前時間
	int NowTimeValue = ( RoleDataEx::G_Now/(60*60)  + RoleDataEx::G_TimeZone )%24;	//戰爭時間

	if( DebugTime_Hour != -1 )
		NowTimeValue = DebugTime_Hour;

	if( LProcessTime == NowTimeValue )
	{
		return;
	}

	LProcessTime = NowTimeValue;

	switch( NowTimeValue )
	{
	case 0:		//預約結束
		{
			PerpareFight();
		}
		break;
	case 20:	//開戰
		{
			BeginFight();
		}
		break;
	case 21:	//公會戰結束
		{
			//通知Local 戰鬥結束
			//EndFight();
			CNetDC_GuildHousesWar::SL_WarEnd( );			
		}
		break;
	}
}
*/