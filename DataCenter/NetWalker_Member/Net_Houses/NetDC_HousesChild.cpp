#include "NetDC_HousesChild.h"


bool	CNetDC_HousesChild::_IsInitHouse_OK = false;
//--------------------------------------------------------------------------------------
bool    CNetDC_HousesChild::Init()
{
	srand( (unsigned)_time32( NULL ) );
	

	CNetDC_Houses::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetDC_HousesChild );

	_RD_NormalDB->SqlCmd( 0 ,  _SQLCmdInitProc , _SQLCmdInitProcResult , NULL , NULL );

	printf( "\n");

	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 1000 );
		_RD_NormalDB->Process( );
		switch( i % 4 )
		{
		case 0:
			printf( "\rInit HouseBase /        " );
			break;
		case 1:
			printf( "\rInit HouseBase |        " );
			break;
		case 2:			
			printf( "\rInit HouseBase \\        " );
			break;
		case 3:
			printf( "\rInit HouseBase -        " );
			break;
		}

		if( _IsInitHouse_OK == true )
			break;
	}
	printf( "\n");


	return true;
}
//--------------------------------------------------------------------------------------
bool    CNetDC_HousesChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	CNetDC_Houses::_Release();

	return true;
}
//--------------------------------------------------------------------------------------
//初始載入
bool 	CNetDC_HousesChild::_SQLCmdInitProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass		MyDBProc( sqlBase );
	MyDBProc.SqlCmd_WriteOneLine( "UPDATE Houses_Base SET IsLogin = 0" );
	return true;
}
void 	CNetDC_HousesChild::_SQLCmdInitProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	_IsInitHouse_OK = true;
}


struct BuildHouseTempStruct
{
	int 	ServerID;
	int 	PlayerDBID;
	int 	HouseType;
	string	Account_ID;

	//return Value
	int		HouseDBID;
};
void CNetDC_HousesChild::RL_BuildHouseRequest		( int ServerID , int PlayerDBID , int HouseType , const char* Account_ID )
{
	BuildHouseTempStruct* TempData = NEW(BuildHouseTempStruct);
	TempData->Account_ID = Account_ID;
	TempData->HouseType = HouseType;
	TempData->PlayerDBID = PlayerDBID;
	TempData->ServerID = ServerID;

	_RD_NormalDB->SqlCmd( 8 ,  _SQLCmdBuildHouse , _SQLCmdBuildHouseResult , TempData
		, NULL );

}

bool CNetDC_HousesChild::_SQLCmdBuildHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	BuildHouseTempStruct* TempData = (BuildHouseTempStruct*)UserObj;

	char SqlCmd[512];
//	int MaxHouseDBID = 0;

	int HouseDBID = -1;
	//檢查是否有房子
	MyDbSqlExecClass MyDBProc( sqlBase );
	sprintf( SqlCmd , "SELECT HouseDBID FROM Houses_Base WHERE OwnerDBID = %d" , TempData->PlayerDBID );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&HouseDBID );
	MyDBProc.Read();
	MyDBProc.Close();

	if( HouseDBID != -1)
	{
		//Arg.PushValue( "HouseDBID" , HouseDBID );
		TempData->HouseDBID = HouseDBID;
		return false;
	}

//	MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );		

/*
	//	sprintf( SqlCmd , "SELECT MAX(HouseDBID) AS Expr1 FROM Houses_Base WITH (TABLOCKX)" );
	sprintf( SqlCmd , "SELECT MAX(HouseDBID) AS Expr1 FROM Houses_Base" );

	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&MaxHouseDBID );
	MyDBProc.Read();
	MyDBProc.Close();
	MaxHouseDBID++;

	while( 1 )
	{
		int	Count = 0;
		//找出這個號碼是否為特殊號
		sprintf( SqlCmd , "SELECT Count(VipID) AS Expr1 FROM Houses_Vip_ID WHERE VipID = %d" , MaxHouseDBID );

		MyDBProc.SqlCmd( SqlCmd );
		MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Count );
		MyDBProc.Read();
		MyDBProc.Close();

		if( Count == 0 )
			break;

		MaxHouseDBID++;
	}

*/

	//建立房屋
	sprintf( SqlCmd , "INSERT Houses_Base(HouseType,OwnerDBID) VALUES(%d,%d)" ,  TempData->HouseType , TempData->PlayerDBID );
	MyDBProc.SqlCmd_WriteOneLine( SqlCmd );

	MyDBProc.SqlCmd( "SELECT SCOPE_IDENTITY()" );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempData->HouseDBID );
	MyDBProc.Read();
	MyDBProc.Close();

	sprintf( SqlCmd , "UPDATE RoleData Set HouseDBID = %d WHERE DBID = %d" , TempData->HouseDBID , TempData->PlayerDBID );
	MyDBProc.SqlCmd_WriteOneLine( SqlCmd );


	return true;
}
void CNetDC_HousesChild::_SQLCmdBuildHouseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	BuildHouseTempStruct* TempData = (BuildHouseTempStruct*)UserObj;
	//int HouseDBID = Arg.GetNumber( "HouseDBID" );sssss
	SL_BuildHouseResult( TempData->ServerID , TempData->PlayerDBID , TempData->HouseDBID , ResultOK );
	delete TempData;
}
//--------------------------------------------------------------------------------------
struct FindHouseTempStruct
{
	int ServerID;
	int PlayerDBID;
	int HouseDBID;
	string Password;
	string RoleName;
	int ManageLv;

	//return value
	FindHouseResultENUM Result;
};
void CNetDC_HousesChild::RL_FindHouseRequest_ByName ( int ServerID , int PlayerDBID , const char* RoleName , const char* Password , int ManageLv )
{
	FindHouseTempStruct* TempData = NEW( FindHouseTempStruct );
	TempData->ServerID = ServerID;
	TempData->PlayerDBID = PlayerDBID;
	TempData->HouseDBID = -1;
	TempData->RoleName = RoleName;
	TempData->Password = Password;
	TempData->ManageLv = ManageLv;
	_RD_NormalDB->SqlCmd( PlayerDBID , _SQLCmdFindHouse , _SQLCmdFindHouseResult , TempData , NULL );
}
void CNetDC_HousesChild::RL_FindHouseRequest		( int ServerID , int PlayerDBID , int HouseDBID , char* Password , int ManageLv )
{
	FindHouseTempStruct* TempData = NEW( FindHouseTempStruct );
	TempData->ServerID = ServerID;
	TempData->PlayerDBID = PlayerDBID;
	TempData->HouseDBID = HouseDBID;
	TempData->Password = Password;
	TempData->ManageLv = ManageLv;
	_RD_NormalDB->SqlCmd( PlayerDBID , _SQLCmdFindHouse , _SQLCmdFindHouseResult , TempData , NULL );
}
bool CNetDC_HousesChild::_SQLCmdFindHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	FindHouseTempStruct* TempData = ( FindHouseTempStruct* )UserObj;
	char	Password[32];
	//char	AccountID[32];
	int		OwnerDBID;
	char	SqlCmd[512];
	
	MyDbSqlExecClass MyDBProc( sqlBase );

	//找出玩家的房屋號碼
	if( TempData->HouseDBID == -1 )
	{
		CharTransferClass	CharTransfer;
		CharTransfer.SetUtf8String( TempData->RoleName.c_str() );
		string RoleNameBinStr = StringToSqlX( (char*)CharTransfer.GetWCString() , CharTransfer.WCStrLen() * 2 , false );


		sprintf( SqlCmd , "SELECT HouseDBID FROM RoleData WHERE RoleName= CAST( %s AS nvarchar(4000) ) and IsDelete = 0" , RoleNameBinStr.c_str() );
		MyDBProc.SqlCmd( SqlCmd );
		MyDBProc.Bind( 1 , SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&(TempData->HouseDBID) );
		MyDBProc.Read( );
		MyDBProc.Close();
	}



	bool IsFind = false;
	sprintf( SqlCmd , "SELECT Password , OwnerDBID FROM Houses_Base WHERE HouseDBID = %d" , TempData->HouseDBID );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1 , SQL_C_CHAR , sizeof( Password ) , (LPBYTE)&Password );
//	MyDBProc.Bind( 2 , SQL_C_CHAR , sizeof( AccountID ), (LPBYTE)&AccountID );
	MyDBProc.Bind( 2 , SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&OwnerDBID );

	if( MyDBProc.Read( ) )
		IsFind = true;;
	MyDBProc.Close();

	if( IsFind == false )
	{
		TempData->Result = EM_FindHouseResult_NotFind;
		return false;
	}

	/*
	int Count = 0;
	//找出是否是參觀自己的房子
	sprintf( SqlCmd , "SELECT COUNT( DBID )FROM RoleData WHERE DBID = %d and Account_ID = '%s'" , TempData->PlayerDBID , AccountID );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1 , SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&Count );
	MyDBProc.Read( );
	MyDBProc.Close();

	if( Count == 0 && strlen( Password ) != 0 && strcmp( Password , TempData->Password.c_str() ) !=  0 )
	{
		TempData->Result = EM_FindHouseResult_PasswordError;
		return false;
	}
*/
	if(		OwnerDBID != TempData->PlayerDBID  
		&&	TempData->ManageLv < EM_Management_GameVisitor
		&&	strlen( Password ) != 0 && strcmp( Password , TempData->Password.c_str() ) !=  0 )
	{
		TempData->Result = EM_FindHouseResult_PasswordError;
		return false;
	}
	TempData->Result = EM_FindHouseResult_OK;

	return true;
}
void CNetDC_HousesChild::_SQLCmdFindHouseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	FindHouseTempStruct* TempData = ( FindHouseTempStruct* )UserObj;
	SL_FindHouseResult( TempData->ServerID , TempData->PlayerDBID , TempData->HouseDBID , TempData->Result );

	delete TempData;
}
//--------------------------------------------------------------------------------------
struct HouseInfoLoadingTempStruct
{
	int		ServerID;
	int		HouseDBID;

	//return value;
	HouseBaseDBStruct HouseBaseInfo;

	vector< HouseItemDBStruct > HouseItem;
	vector< int > FriendDBIDList;
};
//載入房屋資料
void CNetDC_HousesChild::RL_HouseInfoLoading		( int ServerID , int HouseDBID )
{
	HouseInfoLoadingTempStruct* TempData = NEW( HouseInfoLoadingTempStruct );
	TempData->HouseDBID = HouseDBID;
	TempData->ServerID = ServerID;

	_RD_NormalDB->SqlCmd( HouseDBID , _SQLCmdLoadHouse , _SQLCmdLoadHouseResult , TempData , NULL );
}

bool CNetDC_HousesChild::_SQLCmdLoadHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	HouseInfoLoadingTempStruct* TempData = (HouseInfoLoadingTempStruct*)UserObj;
	char	SqlWhereCmd[512];

	MyDbSqlExecClass MyDBProc( sqlBase );
	//g_CritSect()->Enter();
	sprintf( SqlWhereCmd , "WHERE HouseDBID=%d " , TempData->HouseDBID );
	
	bool IsFind = false;
	MyDBProc.SqlCmd( _RDHouseBaseSql->GetSelectStr( SqlWhereCmd ).c_str() );
	MyDBProc.Bind( TempData->HouseBaseInfo , RoleDataEx::ReaderRD_HouseBase() );
	if( MyDBProc.Read( ) )
	{
		RoleDataEx::ReaderRD_HouseBase()->TransferWChartoUTF8( &TempData->HouseBaseInfo , MyDBProc.wcTempBufList() );
		IsFind = true;
	}
	MyDBProc.Close();

	if( IsFind == false )
	{
		//g_CritSect()->Leave();
		return false;
	}
	
	int MaxItemPos = -1;
	int	LastParentItemDBID = -1; 
	HouseItemDBStruct HouseItem;
	HouseItem.Init();
	sprintf( SqlWhereCmd , "WHERE HouseDBID=%d ORDER BY ParentItemDBID , HousePos " , TempData->HouseDBID );

	MyDBProc.SqlCmd( _RDHouseItemSql->GetSelectStr( SqlWhereCmd ).c_str() );

	MyDBProc.Bind( HouseItem , RoleDataEx::ReaderRD_HouseItem() );
	while( MyDBProc.Read() )
	{
		if( LastParentItemDBID != HouseItem.ParentItemDBID )
		{
			MaxItemPos = -1;
			LastParentItemDBID = HouseItem.ParentItemDBID;
		}
		if( MaxItemPos == HouseItem.Pos )
			HouseItem.Pos = MaxItemPos +1;

		MaxItemPos = HouseItem.Pos;

		TempData->HouseItem.push_back( HouseItem );	
	}
	MyDBProc.Close();
	char	SqlCmd[512];
	int		FriendDBID;
	sprintf( SqlCmd , "Select FriendDBID FROM Houses_FriendList WHERE HouseDBID = %d" , TempData->HouseDBID );
	MyDBProc.SqlCmd( SqlCmd );

	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&FriendDBID );
	while( MyDBProc.Read() )
	{
		TempData->FriendDBIDList.push_back( FriendDBID );
	}
	MyDBProc.Close();
	return true;
}

void CNetDC_HousesChild::_SQLCmdLoadHouseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	HouseInfoLoadingTempStruct* TempData = ( HouseInfoLoadingTempStruct*)UserObj;
	
	//通知要求區域
	SL_HouseBaseInfo( TempData->ServerID , TempData->HouseBaseInfo );

	int ItemCount = 0;
	int ParentItemDBID = -1;
	HouseItemDBStruct Item[_DEF_MAX_HOUSE_FURNITURE+10];

	map< int , bool > CheckPosMap;


	if( TempData->HouseItem.size() == 0 )
	{
		SL_ItemInfo( TempData->ServerID , TempData->HouseDBID , -1 , 0 , Item );
	}
	else
	{
		for( unsigned i = 0 ; i < TempData->HouseItem.size() ; i++ )
		{
			if( ParentItemDBID  != TempData->HouseItem[i].ParentItemDBID )
			{
				if( ItemCount != 0 )
					SL_ItemInfo( TempData->ServerID , TempData->HouseDBID , Item[0].ParentItemDBID , ItemCount , Item );

				ParentItemDBID = TempData->HouseItem[i].ParentItemDBID;
				ItemCount = 0;
				CheckPosMap.clear();
			}
			int& pos = TempData->HouseItem[i].Pos;
			if( CheckPosMap[pos] == true )
			{
				for( pos = 0 ; ; pos++ )
				{
					if( CheckPosMap[pos] == false )
					{
						break;
					}
				}
			}
			CheckPosMap[pos] = true;
			Item[ ItemCount++ ] = TempData->HouseItem[i];
		}

		if( ItemCount != 0 )
			SL_ItemInfo( TempData->ServerID , TempData->HouseDBID , Item[0].ParentItemDBID , ItemCount , Item );
	}
	
	for( unsigned i = 0 ; i < TempData->FriendDBIDList.size() ; i++ )
	{
		SL_FriendDBID( TempData->ServerID , TempData->HouseDBID , TempData->FriendDBIDList[i] );
	}


	SL_HouseInfoLoadOK( TempData->ServerID , TempData->HouseDBID );

	delete UserObj;
}
//--------------------------------------------------------------------------------------
/*
struct SaveSwapBodyHouseItemTempStruct
{
	int ServerID;
	int PlayerDBID;
	int BodyPos;
	ItemFieldStruct BodyItem;
	HouseItemDBStruct HouseItem;
};
void CNetDC_HousesChild::RL_SaveSwapBodyHouseItem	( int ServerID , int PlayerDBID , int BodyPos , ItemFieldStruct& BodyItem ,	HouseItemDBStruct& HouseItem )
{
	//找出此玩家
	BaseItemObject* Obj = BaseItemObject::GetObj_DBID( PlayerDBID );
	if( Obj == NULL )
		return;
	RoleDataEx* Role = Obj->Role();
	RoleDataEx* RoleDB = Obj->RoleDB();

	ItemFieldStruct* Item	= Role->GetBodyItem( BodyPos );
	ItemFieldStruct* ItemDB = RoleDB->GetBodyItem( BodyPos );

	if( Item == NULL || ItemDB == NULL )
		return;

	*Item	= BodyItem;
	*ItemDB = BodyItem;

	SaveSwapBodyHouseItemTempStruct* TempData = NEW(SaveSwapBodyHouseItemTempStruct);
	TempData->ServerID = ServerID;
	TempData->PlayerDBID = PlayerDBID;
	TempData->BodyPos = BodyPos;
	TempData->BodyItem = BodyItem;
	TempData->HouseItem = HouseItem;

	_RD_NormalDB->SqlCmd( PlayerDBID , _SQLCmdSaveSwapItem , _SQLCmdSaveSwapItemResult , TempData , NULL );
}
//立即儲存身體與屋子交換的物品資料
bool CNetDC_HousesChild::_SQLCmdSaveSwapItem( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	SaveSwapBodyHouseItemTempStruct* TempData = (SaveSwapBodyHouseItemTempStruct*)UserObj;

	char SqlWhereCmd[512];
	MyDbSqlExecClass MyDBProc( sqlBase );

	g_CritSect()->Enter();
	sprintf( SqlWhereCmd , "WHERE ItemDBID = %d and HouseDBID=%d" , TempData->HouseItem.HouseDBID , TempData->HouseItem.ItemDBID );	
	MyDBProc.SqlCmd_WriteOneLine( _RDHouseItemSql->GetUpDateStr( &TempData->HouseItem , SqlWhereCmd ) );

	DB_ItemFieldStruct BodyItem;
	BodyItem.DBID = TempData->PlayerDBID;
	BodyItem.FieldID = TempData->BodyPos;
	BodyItem.FieldType = 1;
	BodyItem.Info = TempData->BodyItem;
	sprintf( SqlWhereCmd , "WHERE FieldType = 1 and  FieldID = %d and DBID = %d" ,  TempData->BodyPos , TempData->PlayerDBID );
	MyDBProc.SqlCmd_WriteOneLine( _RDItemSql->GetUpDateStr( &BodyItem , SqlWhereCmd ) );
	g_CritSect()->Leave();

	return true;
}
void CNetDC_HousesChild::_SQLCmdSaveSwapItemResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	SaveSwapBodyHouseItemTempStruct* TempData = (SaveSwapBodyHouseItemTempStruct*)UserObj;
	delete TempData;
}
*/
//--------------------------------------------------------------------------------------
struct SaveHouseBaseTempStruct
{
	int ServerID;
	int PlayerDBID;
	HouseBaseDBStruct HouseBaseInfo;
};
//儲存房屋基本資料
void CNetDC_HousesChild::RL_SaveHouseBase			( int ServerID , int PlayerDBID , HouseBaseDBStruct& HouseBaseInfo )
{
	SaveHouseBaseTempStruct* TempData = NEW(SaveHouseBaseTempStruct);
	TempData->ServerID = ServerID;
	TempData->PlayerDBID = PlayerDBID;
	TempData->HouseBaseInfo = HouseBaseInfo;

	_RD_NormalDB->SqlCmd( HouseBaseInfo.HouseDBID , _SQLCmdSaveHouseBase , _SQLCmdSaveHouseBaseResult , TempData , NULL );
}
bool CNetDC_HousesChild::_SQLCmdSaveHouseBase( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	SaveHouseBaseTempStruct* TempData = ( SaveHouseBaseTempStruct*)UserObj;

	char SqlWhereCmd[512];
	MyDbSqlExecClass MyDBProc( sqlBase );
	sprintf( SqlWhereCmd , "WHERE HouseDBID = %d" , TempData->HouseBaseInfo.HouseDBID );

	//g_CritSect()->Enter();

	MyDBProc.SqlCmd_WriteOneLine( _RDHouseBaseSql->GetUpDateStr( &TempData->HouseBaseInfo , SqlWhereCmd ).c_str() );

	//g_CritSect()->Leave();
	return true;
}
void CNetDC_HousesChild::_SQLCmdSaveHouseBaseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	SaveHouseBaseTempStruct* TempData = ( SaveHouseBaseTempStruct*)UserObj;

	delete TempData;
}
//--------------------------------------------------------------------------------------
struct SaveItemTempStruct
{
	int ServerID;
	int PlayerDBID;
	HouseItemDBStruct Item;
};
//儲存房屋物品資料
void CNetDC_HousesChild::RL_SaveItem		( int ServerID , int PlayerDBID , HouseItemDBStruct& Item )
{
	SaveItemTempStruct* TempData = NEW( SaveItemTempStruct );
	TempData->ServerID = ServerID;
	TempData->PlayerDBID = PlayerDBID;
	TempData->Item = Item;
	
	_RD_NormalDB->SqlCmd( Item.HouseDBID , _SQLCmdSaveHouseItem , _SQLCmdSaveHouseItemResult , TempData , NULL );
}
bool CNetDC_HousesChild::_SQLCmdSaveHouseItem( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	SaveItemTempStruct* TempData = ( SaveItemTempStruct*)UserObj;

	char SqlWhereCmd[512];
	MyDbSqlExecClass MyDBProc( sqlBase );
	sprintf( SqlWhereCmd , "WHERE ItemDBID=%d and HouseDBID = %d" , TempData->Item.ItemDBID , TempData->Item.HouseDBID );

	//g_CritSect()->Enter();
	MyDBProc.SqlCmd_WriteOneLine( _RDHouseItemSql->GetUpDateStr( &TempData->Item , SqlWhereCmd ).c_str() );
	//g_CritSect()->Leave();

	return true;	

}

void CNetDC_HousesChild::_SQLCmdSaveHouseItemResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	SaveItemTempStruct* TempData = ( SaveItemTempStruct*)UserObj;
	delete TempData;
}
//--------------------------------------------------------------------------------------
