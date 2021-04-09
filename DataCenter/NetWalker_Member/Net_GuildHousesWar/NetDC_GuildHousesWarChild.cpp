/*
#include "NetDC_GuildHousesWarChild.h"
#include "../../MainProc/GuildManage/GuildManage.h"
#include "../NetWakerDataCenterInc.h"
#include "../../MainProc/GuildHousesWar/GuildHousesWarManageClass.h"
bool	CNetDC_GuildHousesWarChild::_IsInitReady = false;
//--------------------------------------------------------------------------------------
bool    CNetDC_GuildHousesWarChild::Init()
{
	

	CNetDC_GuildHousesWar::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetDC_GuildHousesWarChild );

	_RD_NormalDB->SqlCmd( 0 , _SQLCmdInit , _SQLCmdInitResult , NULL	, NULL );

	printf( "\n");
	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 1000 );
		_RD_NormalDB->Process( );

		switch( i % 4 )
		{
		case 0:
			printf( "\rGuildWar Info Loading...../        " );
			break;
		case 1:
			printf( "\rGuildWar Info Loading.....|        " );
			break;
		case 2:			
			printf( "\rGuildWar Info Loading.....\\        " );
			break;
		case 3:
			printf( "\rGuildWar Info Loading.....-        " );
			break;
		}

		if( _IsInitReady == true )
			break;
	}
	printf( "\n");

	GuildHousesWarManageClass::Init();
	return true;
}
//--------------------------------------------------------------------------------------
bool    CNetDC_GuildHousesWarChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	CNetDC_GuildHousesWar::_Release();

	return true;
}

void CNetDC_GuildHousesWarChild::RL_OpenMenu			( int PlayerDBID )
{
	vector< GuildHouseWarInfoStruct >& List = GuildHousesWarManageClass::GetList();
	SC_OpenMenu( PlayerDBID , GuildHousesWarManageClass::State() , List );
}
void CNetDC_GuildHousesWarChild::RL_WarRegister			( int PlayerDBID , GuildHouseWarRegisterTypeENUM Type )
{
	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );
	if( Member == NULL )
	{
		SC_WarRegisterResult( PlayerDBID , Type , false );
		return;
	}

	if( Member->Guild->Base.LeaderDBID != PlayerDBID )
	{
		SC_WarRegisterResult( PlayerDBID , Type , false );
		return;
	}

	bool Result = false;
	switch( Type )
	{
	case EM_GuildHouseWarRegisterType_Register:
		Result = GuildHousesWarManageClass::WarRegister( Member->GuildID );
		break;
	case EM_GuildHouseWarRegisterType_Cancel:
		Result = GuildHousesWarManageClass::WarCancel( Member->GuildID );
		break;		
	}

	SC_WarRegisterResult( PlayerDBID , Type , Result );
}

bool CNetDC_GuildHousesWarChild::_SQLCmdInit			( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	vector< GuildHouseWarInfoStruct >& List = GuildHousesWarManageClass::GetList();

	MyDbSqlExecClass MyDBProc( sqlBase );

	GuildHouseWarInfoStruct	TempGuildHouseWarInfo;
	MyDBProc.SqlCmd( _RDGuildHouseWarInfoSql->GetSelectStr("").c_str() );
	MyDBProc.Bind( TempGuildHouseWarInfo , RoleDataEx::ReaderRD_GuildHouseWarInfo() );
	while( MyDBProc.Read() )
	{
		List.push_back( TempGuildHouseWarInfo );
	}
	MyDBProc.Close();

	return true;
}
void CNetDC_GuildHousesWarChild::_SQLCmdInitResult	( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	_IsInitReady = true;
}
//////////////////////////////////////////////////////////////////////////
struct TempLoadHouseStruct
{
	int ServerID;
	int	GuildID;
	GuildHousesInfoStruct BaseInfo;
	vector< GuildHouseBuildingInfoStruct >		BuildingList;
	vector<GuildHouseFurnitureItemStruct>		FurnitureItemList;

};
void CNetDC_GuildHousesWarChild::RL_LoadHouseBaseInfo	( int ServerID )
{
	vector< GuildHouseWarInfoStruct >& List = GuildHousesWarManageClass::GetList();

	for( int i = 0 ; i < (int)List.size() ; i++ )
	{
		if( List[i].State == EM_GuildHouseWarState_Failed )
			continue;

		TempLoadHouseStruct* TempData = NEW( TempLoadHouseStruct );
		TempData->ServerID = ServerID;
		TempData->GuildID = List[i].GuildID;
		_RD_NormalDB->SqlCmd( i , _SQLCmdLoadGuildHouse , _SQLCmdLoadGuildHouseResult , TempData	, NULL );
	}
}
bool CNetDC_GuildHousesWarChild::_SQLCmdLoadGuildHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempLoadHouseStruct* TempData = (TempLoadHouseStruct*)UserObj;

	char	SqlWhereCmd[512];

	MyDbSqlExecClass MyDBProc( sqlBase );

	sprintf( SqlWhereCmd , "WHERE GuildID=%d " , TempData->GuildID );

	bool IsFind = false;
	MyDBProc.SqlCmd( _RDGuildHouseBaseSql->GetSelectStr( SqlWhereCmd ).c_str() );
	MyDBProc.Bind( TempData->BaseInfo , RoleDataEx::ReaderRD_GuildHouseInfo() );
	if( MyDBProc.Read( ) )
		IsFind = true;
	MyDBProc.Close();

	if( IsFind == false )
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	GuildHouseBuildingInfoStruct Building;
	Building.State = EM_GuildHouseBuildingInfoState_None;
	sprintf( SqlWhereCmd , "WHERE GuildID=%d" , TempData->GuildID );

	MyDBProc.SqlCmd( _RDGuildHouseBuildingSql->GetSelectStr( SqlWhereCmd ).c_str() );

	MyDBProc.Bind( Building , RoleDataEx::ReaderRD_GuildHouseBuildingInfo() );
	while( MyDBProc.Read() )
	{
		TempData->BuildingList.push_back( Building );
	}
	MyDBProc.Close();
	//////////////////////////////////////////////////////////////////////////
	//Åª¨ú³Ã­Ñ¦Cªí
	//////////////////////////////////////////////////////////////////////////
	GuildHouseFurnitureItemStruct FurnitureItem;
	FurnitureItem.Item.Pos = EM_ItemState_NONE;
	FurnitureItem.State =  EM_GuildHouseItemInfoState_None;
	sprintf( SqlWhereCmd , "WHERE GuildID=%d" , TempData->GuildID );

	MyDBProc.SqlCmd( _RDGuildHouseFurnitureSql->GetSelectStr( SqlWhereCmd ).c_str() );

	MyDBProc.Bind( FurnitureItem , RoleDataEx::ReaderRD_GuildHouseFurnitureItem() );
	while( MyDBProc.Read() )
	{		
		TempData->FurnitureItemList.push_back( FurnitureItem );
	}
	MyDBProc.Close();
	//////////////////////////////////////////////////////////////////////////
	return true;
}

void CNetDC_GuildHousesWarChild::_SQLCmdLoadGuildHouseResult	( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	if( ResultOK == false )
		return;
	TempLoadHouseStruct* TempData = (TempLoadHouseStruct*)UserObj;

	SL_HouseBaseInfo	( TempData->ServerID , TempData->BaseInfo );

	for( unsigned i = 0 ; i < TempData->BuildingList.size() ; i++ )
	{
		SL_BuildingInfo( TempData->ServerID , TempData->BuildingList[i] );
	}

	for( unsigned i = 0 ; i < TempData->FurnitureItemList.size() ; i++ )
	{
		SL_FurnitureItemInfo( TempData->ServerID , TempData->FurnitureItemList[i] );
	}

	SL_HouseInfoLoadOK	( TempData->ServerID , TempData->GuildID );


	delete TempData;
}
//////////////////////////////////////////////////////////////////////////

void CNetDC_GuildHousesWarChild::RL_WarEndOK			( int ServerID )
{
	GuildHousesWarManageClass::EndFight();
}
/////////////////////////////////////////////////////////////////////////
void CNetDC_GuildHousesWarChild::RL_DebugTime			( int ServerID , int Time_Day , int Time_Hour )
{
//	GuildHousesWarManageClass::DebugTime_Hour = Time_Hour;
//	GuildHousesWarManageClass::DebugTime_Day = Time_Day;
//	GuildHousesWarManageClass::Proc( );
}

struct TempHistoryRequestStruct
{
	int PlayerDBID;
	int DayBefore;

	vector<GuildHouseWarHistoryStruct> Result;
};

void CNetDC_GuildHousesWarChild::RL_HistoryRequest		( int PlayerDBID , int DayBefore )
{
	TempHistoryRequestStruct* TempData = NEW TempHistoryRequestStruct;
	TempData->DayBefore	 = DayBefore;
	TempData->PlayerDBID = PlayerDBID;

	_RD_NormalDB->SqlCmd( PlayerDBID , _SQLCmdLoadGuildHouse , _SQLCmdLoadGuildHouseResult , TempData	, NULL );
}
bool CNetDC_GuildHousesWarChild::_SQLCmdHistory		( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempHistoryRequestStruct* TempData = ( TempHistoryRequestStruct* )UserObj;
	int DayTime = RoleDataEx::G_SysTime / (24*60*60 ) - TempData->DayBefore;
	//////////////////////////////////////////////////////////////////////////
	char	SqlWhereCmd[512];

	MyDbSqlExecClass MyDBProc( sqlBase );
	sprintf( SqlWhereCmd , "WHERE DayTeim=%d " , DayTime );

	GuildHouseWarHistoryStruct Temp;
	MyDBProc.SqlCmd( _RDGuildHouseWarHistorySql->GetSelectStr( SqlWhereCmd ).c_str() );
	MyDBProc.Bind( Temp , RoleDataEx::ReaderRD_GuildHouseWarHistory() );
	while( MyDBProc.Read( ) )
	{
		TempData->Result.push_back( Temp );
	}
	MyDBProc.Close();
	//////////////////////////////////////////////////////////////////////////
	return true;
}
void CNetDC_GuildHousesWarChild::_SQLCmdHistoryResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempHistoryRequestStruct* TempData = ( TempHistoryRequestStruct* )UserObj;
	SC_HistoryResult( TempData->PlayerDBID , TempData->DayBefore , TempData->Result );
	delete TempData;
}
*/