#include "NetDC_GuildHousesChild.h"
#include "../../MainProc/GuildManage/GuildManage.h"
#include "../NetWakerDataCenterInc.h"
//--------------------------------------------------------------------------------------
bool    CNetDC_GuildHousesChild::Init()
{
	CNetDC_GuildHouses::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetDC_GuildHousesChild );

	return true;
}
//--------------------------------------------------------------------------------------
bool    CNetDC_GuildHousesChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	CNetDC_GuildHouses::_Release();

	return true;
}
//--------------------------------------------------------------------------------------
struct BuildGuildHouseTempStruct
{
	int 	ServerID;
	int 	PlayerDBID;
	int		GuildID;
};
//--------------------------------------------------------------------------------------------
void CNetDC_GuildHousesChild::RL_BuildHouseRequest	( int ServerID , int PlayerDBID , int GuildID  )
{
	BuildGuildHouseTempStruct* TempData = NEW(BuildGuildHouseTempStruct);
	TempData->GuildID = GuildID;
	TempData->PlayerDBID = PlayerDBID;
	TempData->ServerID = ServerID;

	_RD_NormalDB->SqlCmd( GuildID ,  _SQLCmdBuildHouse , _SQLCmdBuildHouseResult , TempData
		, NULL );
}
//建立房屋
bool CNetDC_GuildHousesChild::_SQLCmdBuildHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	BuildGuildHouseTempStruct* TempData = (BuildGuildHouseTempStruct*)UserObj;

	char SqlCmd[512];
	int Count = 0;
	//檢查是否有房子
	MyDbSqlExecClass MyDBProc( sqlBase );
	sprintf( SqlCmd , "SELECT COUNT(GuildID) FROM GuildHouse_Base WHERE GuildID = %d" , TempData->GuildID );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Count );
	MyDBProc.Read();
	MyDBProc.Close();

	if( Count != 0 )
		return true;

	//建立房屋
	sprintf( SqlCmd , "INSERT GuildHouse_Base(GuildID) VALUES(%d)" , TempData->GuildID );
	MyDBProc.SqlCmd_WriteOneLine( SqlCmd );

	//////////////////////////////////////////////////////////////////////////
	//建立樣板建築
	//////////////////////////////////////////////////////////////////////////
	{
		CreateDBCmdClass<GuildHouseBuildingInfoStruct>*	RDGuildHouseBuildingSql = NEW CreateDBCmdClass<GuildHouseBuildingInfoStruct>(RoleDataEx::ReaderRD_GuildHouseBuildingInfo() , "GuildHouse_Building" );
		vector< GuildHouseBuildingInfoStruct >	_BaseGuildBuildingInfo;	
		GuildHouseBuildingInfoStruct TempGuildHouse;
		for( unsigned int i = 0 ; i < g_ObjectData->_GuildBaseBuildingTable.size() ; i++ )
		{
			GuildBaseBuildingTableStruct& Temp1 = g_ObjectData->_GuildBaseBuildingTable[i];
			TempGuildHouse.GuildID = TempData->GuildID;
			TempGuildHouse.BuildingDBID = Temp1.GUID;
			TempGuildHouse.BuildingID = Temp1.BuildID;
			TempGuildHouse.ParentDBID = Temp1.ParentID;
			TempGuildHouse.Dir = 0;
			MyStrcpy( TempGuildHouse.PointStr , Temp1.BuildPoint , sizeof( TempGuildHouse.PointStr ) );
			memset( TempGuildHouse.Value , 0 , sizeof(TempGuildHouse.Value) );

			MyDBProc.SqlCmd_WriteOneLine( RDGuildHouseBuildingSql->GetInsertStr( &TempGuildHouse ).c_str() );
		}
		delete RDGuildHouseBuildingSql;
	}
	//////////////////////////////////////////////////////////////////////////
	//建立權限資料
	//////////////////////////////////////////////////////////////////////////
	{
		char	Buf[0x10000];
		GuildStorehouseRightStruct TempConfig;

		for( int i = 0 ; i < _DEF_GUIDHOUSEITEM_MAX_PAGE_ ; i ++ )
		{
			string StrData = StringToSqlX( (char*)TempConfig.Rank , sizeof( TempConfig.Rank ), false );
			sprintf( Buf , "Insert GuildHouse_Item_Right (GuildID,PageID,Config) VALUES(%d,%d,CAST( %s AS varbinary(80) ) )" , TempData->GuildID , i , StrData.c_str() );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	return true;
}


void CNetDC_GuildHousesChild::_SQLCmdBuildHouseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	BuildGuildHouseTempStruct* TempData = (BuildGuildHouseTempStruct*)UserObj;

	if( ResultOK == true )
	{
		GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( TempData->GuildID );
		if( Guild != NULL && Guild->Base.IsOwnHouse == false )
		{
			Guild->Base.IsOwnHouse		= true;
			Guild->Base.IsNeedSave		= true;
			CNetDC_GuildChild::SL_All_ModifyGuild( Guild->Base );
			CNetDC_GuildChild::SC_AllMember_GuildBaseInfoUpdate( Guild->Base );
		}
	}

	SL_BuildHouseResult( TempData->ServerID , TempData->PlayerDBID , TempData->GuildID , ResultOK );
	delete TempData;
}
//--------------------------------------------------------------------------------------

struct FindGuildHouseTempStruct
{
	int ServerID;
	int PlayerDBID;
	int	GuildID;
};
void CNetDC_GuildHousesChild::RL_FindHouseRequest	( int ServerID , int PlayerDBID , int GuildID )
{
	FindGuildHouseTempStruct* TempData = NEW( FindGuildHouseTempStruct );
	TempData->ServerID = ServerID;
	TempData->PlayerDBID = PlayerDBID;
	TempData->GuildID = GuildID;

	_RD_NormalDB->SqlCmd( PlayerDBID , _SQLCmdFindHouse , _SQLCmdFindHouseResult , TempData , NULL );
}
//--------------------------------------------------------------------------------------
//搜尋房子
bool CNetDC_GuildHousesChild::_SQLCmdFindHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	FindGuildHouseTempStruct* TempData = ( FindGuildHouseTempStruct* )UserObj;

	char SqlCmd[512];
	int Count = 0;
	//檢查是否有房子
	MyDbSqlExecClass MyDBProc( sqlBase );
	sprintf( SqlCmd , "SELECT COUNT(GuildID) FROM GuildHouse_Base WHERE GuildID = %d" , TempData->GuildID );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Count );
	MyDBProc.Read();
	MyDBProc.Close();

	if( Count == 0 )
		return false;

	return true;
}
void CNetDC_GuildHousesChild::_SQLCmdFindHouseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	FindGuildHouseTempStruct* TempData = ( FindGuildHouseTempStruct* )UserObj;

	SL_FindHouseResult( TempData->ServerID , TempData->PlayerDBID , TempData->GuildID , ResultOK );
	delete TempData;
}
//--------------------------------------------------------------------------------------
struct GuildHouseInfoLoadingTempStruct
{
	int		ServerID;
	int		GuildID;

	GuildHousesInfoStruct BaseInfo;
	vector< GuildHouseBuildingInfoStruct >		BuildingList;
	map< int , vector< GuildHouseItemStruct > >	ItemPage;
	vector< GuildStorehouseRightStruct >		Config;
	vector<GuildHouseFurnitureItemStruct>		FurnitureItemList;
	vector< GuildItemStoreLogStruct >			ItemLog;
};

void CNetDC_GuildHousesChild::RL_HouseInfoLoading	( int ServerID , int GuildID )
{
	GuildHouseInfoLoadingTempStruct* TempData = NEW( GuildHouseInfoLoadingTempStruct );
	TempData->GuildID = GuildID;
	TempData->ServerID = ServerID;

	_RD_NormalDB->SqlCmd( GuildID , _SQLCmdLoadHouse , _SQLCmdLoadHouseResult , TempData , NULL );
}

bool CNetDC_GuildHousesChild::_SQLCmdLoadHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	GuildHouseInfoLoadingTempStruct* TempData = (GuildHouseInfoLoadingTempStruct*)UserObj;
	char	SqlWhereCmd[512];

	MyDbSqlExecClass MyDBProc( sqlBase );
	//g_CritSect()->Enter();
	sprintf( SqlWhereCmd , "WHERE GuildID=%d " , TempData->GuildID );

	bool IsFind = false;
	MyDBProc.SqlCmd( _RDGuildHouseBaseSql->GetSelectStr( SqlWhereCmd ).c_str() );
	MyDBProc.Bind( TempData->BaseInfo , RoleDataEx::ReaderRD_GuildHouseInfo() );
	if( MyDBProc.Read( ) )
		IsFind = true;
	MyDBProc.Close();

	if( IsFind == false )
	{
		//g_CritSect()->Leave();
		return false;
	}

	GuildHouseItemStruct Item;
	Item.State = EM_GuildHouseItemInfoState_None;
	sprintf( SqlWhereCmd , "WHERE GuildID=%d ORDER BY PageID " , TempData->GuildID );

	MyDBProc.SqlCmd( _RDGuildHouseItemSql->GetSelectStr( SqlWhereCmd ).c_str() );

	MyDBProc.Bind( Item , RoleDataEx::ReaderRD_GuildHouseItem() );
	while( MyDBProc.Read() )
	{
		Item.Item.Pos = EM_ItemState_NONE;
		TempData->ItemPage[ Item.PageID ].push_back( Item );
	}
	MyDBProc.Close();

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
	//讀取傢俱列表
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
	//倉庫設定
	//////////////////////////////////////////////////////////////////////////
	{
		char	Buf[1024];
		GuildStorehouseRightStruct StoreConfig;
		StoreConfig.GuildID = TempData->GuildID;

		sprintf( Buf , "SELECT  PageID , Config FROM GuildHouse_Item_Right WHERE GuildID =%d" , TempData->GuildID );

		MyDBProc.SqlCmd( Buf );
		MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&StoreConfig.PageID );
		MyDBProc.Bind( 2, SQL_C_BINARY , sizeof(StoreConfig.Rank) , (LPBYTE)&StoreConfig.Rank );

		while( MyDBProc.Read() )
		{
			TempData->Config.push_back( StoreConfig );
		}
		MyDBProc.Close();

	}
	//////////////////////////////////////////////////////////////////////////
	//倉庫Log
	//////////////////////////////////////////////////////////////////////////
	{
		int Time = TimeStr::Now_Value()/(24*60*60*7) * (24*60*60*7);
		char	Buf[1024];
		GuildItemStoreLogStruct ItemLog;
		ItemLog.GuildID = TempData->GuildID;

		sprintf( Buf , "SELECT  Time , PlayerDBID , ItemOrgID , ItemCount , PageID , Type , FromPageID , ToPageID FROM GuildHouse_ItemLog WHERE GuildID =%d and Time > %d" , TempData->GuildID , Time );

		MyDBProc.SqlCmd( Buf );
		MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ItemLog.Time );
		MyDBProc.Bind( 2, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ItemLog.PlayerDBID );
		MyDBProc.Bind( 3, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ItemLog.ItemOrgID );
		MyDBProc.Bind( 4, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ItemLog.ItemCount );
		MyDBProc.Bind( 5, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ItemLog.PageID );
		MyDBProc.Bind( 6, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ItemLog.Type );
		MyDBProc.Bind( 7, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ItemLog.FromPageID );
		MyDBProc.Bind( 8, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ItemLog.ToPageID );

		while( MyDBProc.Read() )
		{
			TempData->ItemLog.push_back( ItemLog );
		}
		MyDBProc.Close();

	}
	//////////////////////////////////////////////////////////////////////////
	//g_CritSect()->Leave();

	return true;
}
void CNetDC_GuildHousesChild::_SQLCmdLoadHouseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	if( ResultOK == false )
		return;

	GuildHouseInfoLoadingTempStruct* TempData = (GuildHouseInfoLoadingTempStruct*)UserObj;

	SL_HouseBaseInfo	( TempData->ServerID , TempData->BaseInfo );

	map< int , vector< GuildHouseItemStruct > >::iterator Iter;
	for( Iter = TempData->ItemPage.begin() ; Iter != TempData->ItemPage.end() ; Iter++ )
	{
		SL_ItemInfo			( TempData->ServerID , TempData->GuildID , Iter->first , Iter->second );
	}
	
	for( unsigned i = 0 ; i < TempData->BuildingList.size() ; i++ )
	{
		SL_BuildingInfo( TempData->ServerID , TempData->BuildingList[i] );
	}

	for( unsigned i = 0 ; i < TempData->FurnitureItemList.size() ; i++ )
	{
		SL_FurnitureItemInfo( TempData->ServerID , TempData->FurnitureItemList[i] );
	}

	for( unsigned i = 0 ; i < TempData->Config.size() ; i++ )
	{
		SL_StoreConfig( TempData->ServerID , TempData->Config[i] );
	}

	for( unsigned i = 0 ; i < TempData->ItemLog.size() ; i++ )
	{
		SL_ItemLog( TempData->ServerID , TempData->ItemLog[i] );
	}

	SL_HouseInfoLoadOK	( TempData->ServerID , TempData->GuildID );



	delete TempData;
}
//--------------------------------------------------------------------------------------