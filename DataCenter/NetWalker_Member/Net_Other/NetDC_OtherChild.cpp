#include "NetDC_OtherChild.h"
#include "../net_Talk/Net_TalkChild.h"
//bool									CNetDC_OtherChild::_IsLoadMoneyExchangeTabledOK = false;
vector< MoneyExchangeKeyValueStruct >	CNetDC_OtherChild::_MoneyExchangeKeyValueList;
MyLzoClass								CNetDC_OtherChild::_MyLzoZip;
int										CNetDC_OtherChild::_MoneyExchangeKeyValueZipSize = 0;
char									CNetDC_OtherChild::_MoneyExchangeKeyValueZip[0x10000];
bool									CNetDC_OtherChild::_IsLoadStringDBTabledOK = false;
vector < SysKeyStrChangeStruct >		CNetDC_OtherChild::_StringDBList;
int										CNetDC_OtherChild::_StringDBZipSize;
char									CNetDC_OtherChild::_StringDBZip[0x10000];
map< string , int >						CNetDC_OtherChild::_SysKeyValue;
vector<WeekZoneStruct>					CNetDC_OtherChild::_WeekInst;
vector< BuffScheduleStruct >			CNetDC_OtherChild::_BuffSchList;
//--------------------------------------------------------------------------------------
struct TempInitWorldProcStruct
{
	map<int,NameAllLanguageStruct> NameAllLanguage;
	int		StringCount;
};

bool    CNetDC_OtherChild::Init()
{

	CNetDC_Other::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetDC_OtherChild );
	Schedular()->Push( _OntimeProc , 10*1000 , NULL , NULL );
/*
	_RD_NormalDB_Global->SqlCmd( 0 ,  _SQLCmdInitProc_Global , _SQLCmdInitProcResult_Global , NULL , NULL );

	printf( "\n");

	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 1000 );
		_RD_NormalDB_Global->Process( );
		switch( i % 4 )
		{
		case 0:
			printf( "\rMoney Exchange Table Loading..... /        " );
			break;
		case 1:
			printf( "\rMoney Exchange Table Loading..... |        " );
			break;
		case 2:			
			printf( "\rMoney Exchange Table Loading..... \\        " );
			break;
		case 3:
			printf( "\rMoney Exchange Table Loading..... -        " );
			break;
		}

		if( _IsLoadMoneyExchangeTabledOK == true )
			break;
	}
	*/
	printf( "\n");



	_RD_NormalDB_Import->SqlCmd( 0 ,  _SQLCmdInitProc_Import , _SQLCmdInitProcResult_Import , NULL , NULL );

	printf( "\n");

	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 1000 );
		_RD_NormalDB_Import->Process( );
		switch( i % 4 )
		{
		case 0:
			printf( "\rStringDB Table Loading..... /        " );
			break;
		case 1:
			printf( "\rStringDB Table Loading..... |        " );
			break;
		case 2:			
			printf( "\rStringDB Table Loading..... \\        " );
			break;
		case 3:
			printf( "\rStringDB Table Loading..... -        " );
			break;
		}

		if( _IsLoadStringDBTabledOK == true )
			break;
	}
	printf( "\n");



	if( _IgnoreObjNameStringDB == false )
	{
		TempInitWorldProcStruct* TempData = NEW TempInitWorldProcStruct;

		_RD_NormalDB->SqlCmd( 0 ,  _SQLCmdInitProc_World , _SQLCmdInitProcResult_World , TempData , NULL );
		
		printf( "\n");
		_IsLoadStringDBTabledOK = false;
		for( int i = 0 ; i < 2000 ; i++ )
		{
			Sleep( 1000 );
			_RD_NormalDB->Process( );
			switch( i % 4 )
			{
			case 0:
				printf( "\rObject Name Table Process..... /        " );
				break;
			case 1:
				printf( "\rObject Name Table Process..... |        " );
				break;
			case 2:			
				printf( "\rObject Name Table Process..... \\        " );
				break;
			case 3:
				printf( "\rObject Name Table Process..... -        " );
				break;
			}

			if( _IsLoadStringDBTabledOK == true )
				break;
		}
	}

	return true;
}
//--------------------------------------------------------------------------------------
bool    CNetDC_OtherChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
		
	CNetDC_Other::_Release();

	return true;
}
//--------------------------------------------------------------------------------------
bool CNetDC_OtherChild::_SQLCmdLoadStringDB( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass	MyDBProc( sqlBase );

	
	_StringDBList.clear();
	wchar_t	Content[64];
	SysKeyStrChangeStruct Temp;	
	Temp.Init();
	MyDBProc.SqlCmd( "SELECT KeyStr,Content FROM StringDB" );

	MyDBProc.Bind( 1, SQL_C_CHAR , sizeof(Temp.KeyStr)	, (LPBYTE)Temp.KeyStr.Begin() );
	MyDBProc.Bind( 2, SQL_WCHAR  , sizeof(Content) 		, (LPBYTE)&Content );

	while( MyDBProc.Read() )
	{
		CharTransferClass	CharTransfer;
		CharTransfer.SetWCString( Content );
		Temp.Content = CharTransfer.GetUtf8String();
		_StringDBList.push_back( Temp );
		Temp.Init();
	};
	MyDBProc.Close( );
	return true;
}
void CNetDC_OtherChild::_SQLCmdLoadStringDBResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	_IsLoadStringDBTabledOK = true;
	{
		SysKeyStrChangeStruct*	TempData = new SysKeyStrChangeStruct[_StringDBList.size()];
		for( unsigned i = 0 ; i < _StringDBList.size() ; i++ )
		{
			TempData[i] = _StringDBList[i];
		}

		_StringDBZipSize = _MyLzoZip.Encode( (char*)TempData , (int)( sizeof(SysKeyStrChangeStruct)*_StringDBList.size() ) , _StringDBZip );

		delete TempData;
	}

	//通知所有server
	SL_All_DBStringTable( int( _StringDBList.size() ) ,  _StringDBZipSize , _StringDBZip );
	
}

//初始載入
bool CNetDC_OtherChild::_SQLCmdInitProc_Import( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass	MyDBProc( sqlBase );

	{
		char	Buf[512];
		sprintf( Buf , "DELETE ServerStartLog WHERE WorldID=%d",Global::GetWorldID() );
		//把ServerStartTable 清除
		MyDBProc.SqlCmd_WriteOneLine( Buf );
		//MyDBProc.SqlCmd_WriteOneLine( "TRUNCATE TABLE ServerStartLog" );
	}

	{
		wchar_t	Content[64];
		SysKeyStrChangeStruct Temp;	
		Temp.Init();
		MyDBProc.SqlCmd( "SELECT KeyStr,Content FROM StringDB" );

		MyDBProc.Bind( 1, SQL_C_CHAR , sizeof(Temp.KeyStr)	, (LPBYTE)Temp.KeyStr.Begin() );
		MyDBProc.Bind( 2, SQL_WCHAR  , sizeof(Content) 		, (LPBYTE)&Content );

		while( MyDBProc.Read() )
		{
			CharTransferClass	CharTransfer;
			CharTransfer.SetWCString( Content );
			Temp.Content = CharTransfer.GetUtf8String();
			_StringDBList.push_back( Temp );
			Temp.Init();
		};
		MyDBProc.Close( );

	}

	{
		MoneyExchangeKeyValueStruct Temp;	
		MyDBProc.SqlCmd( "SELECT KeyStr,Value0,Value1,Value2,Value3 FROM Table_MoneyExchange" );
		MyDBProc.Bind( 1, SQL_C_CHAR , sizeof(Temp.KeyStr)	, (LPBYTE)&Temp.KeyStr );
		MyDBProc.Bind( 2, SQL_C_LONG , SQL_C_DEFAULT 		, (LPBYTE)&Temp.Value[0] );
		MyDBProc.Bind( 3, SQL_C_LONG , SQL_C_DEFAULT 		, (LPBYTE)&Temp.Value[1] );
		MyDBProc.Bind( 4, SQL_C_LONG , SQL_C_DEFAULT 		, (LPBYTE)&Temp.Value[2] );
		MyDBProc.Bind( 5, SQL_C_LONG , SQL_C_DEFAULT 		, (LPBYTE)&Temp.Value[3] );

		while( MyDBProc.Read() )
		{
			_MoneyExchangeKeyValueList.push_back( Temp );
		};
		MyDBProc.Close( );

	}

	{
		char	SqlCmd[512];
		sprintf( SqlCmd , "SELECT KeyStr,Value FROM SysKeyValue WHERE (WorldID = - 1) OR (WorldID = %d)" , Global::GetWorldID() );

		char	KeyStr[32];
		int		Value;
		MyDBProc.SqlCmd( SqlCmd );
		MyDBProc.Bind( 1, SQL_C_CHAR , sizeof(KeyStr)		, (LPBYTE)KeyStr );
		MyDBProc.Bind( 2, SQL_C_LONG , SQL_C_DEFAULT 		, (LPBYTE)&Value );

		while( MyDBProc.Read() )
		{
			_SysKeyValue[KeyStr] = Value;
		}
		MyDBProc.Close();

	}

	{
		WeekZoneStruct tmp;

		MyDBProc.SqlCmd( "SELECT Day, ZoneGroupID FROM WeekInstances" );
		MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT		, (LPBYTE)&tmp.Day );
		MyDBProc.Bind( 2, SQL_C_LONG , SQL_C_DEFAULT 		, (LPBYTE)&tmp.ZoneGroupID );

		while( MyDBProc.Read() )
		{
			_WeekInst.push_back( tmp );
		}
		MyDBProc.Close();
	}

	//魔幻寶盒後台資料
	{
		int groupID;
		ItemCombineStruct infoDB;
		MyDBProc.SqlCmd( "SELECT GroupID, ProductID, ProductCount, SourceID1, SourceID2, SourceID3, SourceID4, SourceID5, SourceCount1,SourceCount2, SourceCount3, SourceCount4, SourceCount5 FROM Table_ItemCombine" );

		MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT		, (LPBYTE)&groupID );
		MyDBProc.Bind( 2, SQL_C_LONG , SQL_C_DEFAULT		, (LPBYTE)&infoDB.ProductID );
		MyDBProc.Bind( 3, SQL_C_LONG , SQL_C_DEFAULT		, (LPBYTE)&infoDB.ProductCount );
		for( int i = 0 ; i < 5 ;i++ )
		{
			MyDBProc.Bind( 4+i, SQL_C_LONG , SQL_C_DEFAULT		, (LPBYTE)&infoDB.SourceID[i] );
			MyDBProc.Bind( 9+i, SQL_C_LONG , SQL_C_DEFAULT		, (LPBYTE)&infoDB.SourceCount[i] );
		}

		while( MyDBProc.Read() )
		{
			ItemCombineStruct info = infoDB;
			map< int , int > fixInfoMap;
			int x;
			for( x = 0 ; x < 5 ; x++ )
			{
				if( info.SourceCount[x] == 0 || info.SourceID[x] == 0 )
					continue;
				fixInfoMap[ info.SourceID[x] ] += info.SourceCount[x];
			}
			info.Init();

			map< int , int >::iterator iter;
			for( x = 0 , iter = fixInfoMap.begin() ; iter != fixInfoMap.end() ; iter++ , x++ )
			{
				info.SourceCount[x] = iter->second;
				info.SourceID[x] = iter->first;
			}
			int hashID = GetHashCode( &info , sizeof( info ) );
			info.ProductCount = infoDB.ProductCount;
			info.ProductID = infoDB.ProductID;
			RoleDataEx::ItemCombineListEx[groupID][hashID].push_back( info );		
		}
		MyDBProc.Close();
	}


	return true;
}
void CNetDC_OtherChild::_SQLCmdInitProcResult_Import ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{

	_IsLoadStringDBTabledOK = true;
	{
		SysKeyStrChangeStruct*	TempData = new SysKeyStrChangeStruct[_StringDBList.size()];
		for( unsigned i = 0 ; i < _StringDBList.size() ; i++ )
		{
			TempData[i] = _StringDBList[i];
		}

		_StringDBZipSize = _MyLzoZip.Encode( (char*)TempData , (int)( sizeof(SysKeyStrChangeStruct)*_StringDBList.size() ) , _StringDBZip );

		delete TempData;
	}

	{
		MoneyExchangeKeyValueStruct*	TempData = new MoneyExchangeKeyValueStruct[_MoneyExchangeKeyValueList.size()];
		for( unsigned i = 0 ; i < _MoneyExchangeKeyValueList.size() ; i++ )
		{
			TempData[i] = _MoneyExchangeKeyValueList[i];
		}

		_MoneyExchangeKeyValueZipSize = _MyLzoZip.Encode( (char*)TempData , (int)( sizeof(MoneyExchangeKeyValueStruct)*_MoneyExchangeKeyValueList.size() ) , _MoneyExchangeKeyValueZip );

		delete TempData;

	}
}

//--------------------------------------------------------------------------------------
//清除ObjNameStringDB
bool CNetDC_OtherChild::_SQLCmdInitProc_World( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempInitWorldProcStruct* TempData = ( TempInitWorldProcStruct* )UserObj;

	map<int,NameAllLanguageStruct>& NameAllLanguage = TempData->NameAllLanguage;
	ObjectDataClass::GetNameAllLanguage( NameAllLanguage );


	//計算checksum
	int	StringCount = 0;
	map<int,NameAllLanguageStruct>::iterator Iter;

	for( Iter = NameAllLanguage.begin() ; Iter != NameAllLanguage.end() ; Iter++ )
	{
		NameAllLanguageStruct& Info = Iter->second;

		for( int i = 0 ; i < EM_LanguageType_Count ; i++ )
		{
			StringCount += Info.Language[i].length();
		}
	}

	MyDbSqlExecClass	MyDBProc( sqlBase );

	int DBCheckSum = -1;

	//sprintf( Buf , "SELECT CheckSum Where KeyStr = 'ObjNameStringDB' "  );
	MyDBProc.SqlCmd( "SELECT CheckSum FROM Table_CheckList Where KeyStr = 'ObjNameStringDB' " );
	MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&DBCheckSum );
	MyDBProc.Read( );
	MyDBProc.Close( );
	
	if( StringCount == DBCheckSum )
		return false;

	if( DBCheckSum == -1 )
	{
		MyDBProc.SqlCmd_WriteOneLine( "INSERT Table_CheckList ( KeyStr ) VALUES ('ObjNameStringDB')" );
	}
	TempData->StringCount = StringCount;

	MyDBProc.SqlCmd_WriteOneLine( "TRUNCATE TABLE ObjNameStringDB" );
	return true;
}

void CNetDC_OtherChild::_SQLCmdInitProcResult_World( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	char InsertCmd[0x10000];
	_IsLoadStringDBTabledOK = true;

	TempInitWorldProcStruct* TempData = ( TempInitWorldProcStruct* )UserObj;
	map<int,NameAllLanguageStruct>& NameAllLanguage = TempData->NameAllLanguage;

	//如果不需要重載
	if( ResultOK == false )
	{
		delete TempData;
		return;
	}

	map<int,NameAllLanguageStruct>::iterator Iter;

	for( Iter = NameAllLanguage.begin() ; Iter != NameAllLanguage.end() ; Iter++ )
	{
		NameAllLanguageStruct& Info = Iter->second;

		CharTransferClass	CharTransfer[ EM_LanguageType_Count ];
		string				RoleNameBinStr[ EM_LanguageType_Count ];
		for( int i = 0 ; i < EM_LanguageType_Count ; i++ )
		{
			CharTransfer[i].SetUtf8String( Info.Language[i].c_str() );
			RoleNameBinStr[i] = StringToSqlX( (char*)CharTransfer[i].GetWCString() , CharTransfer[i].WCStrLen() * 2 , false );
		}
		sprintf_s( InsertCmd , sizeof(InsertCmd) , "Insert ObjNameStringDB(GUID, Language1,Language2,Language3,Language4,Language5,Language6,Language7,Language8,Language9,Language10,Language11,Language12,Language13,Language14,Language15,Language16,Language17,Language18,Language19,Language20,Language21,Language22 ) VALUES (%d,CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)),CAST( %s AS nvarchar(4000)) )"
			, Info.GUID 
			, RoleNameBinStr[0].c_str() 
			, RoleNameBinStr[1].c_str() 
			, RoleNameBinStr[2].c_str() 
			, RoleNameBinStr[3].c_str() 
			, RoleNameBinStr[4].c_str() 
			, RoleNameBinStr[5].c_str() 
			, RoleNameBinStr[6].c_str() 
			, RoleNameBinStr[7].c_str() 
			, RoleNameBinStr[8].c_str() 
			, RoleNameBinStr[9].c_str() 
			, RoleNameBinStr[10].c_str() 
			, RoleNameBinStr[11].c_str() 
			, RoleNameBinStr[12].c_str() 
			, RoleNameBinStr[13].c_str() 
			, RoleNameBinStr[14].c_str() 
			, RoleNameBinStr[15].c_str() 
			, RoleNameBinStr[16].c_str() 
			, RoleNameBinStr[17].c_str() 
			, RoleNameBinStr[18].c_str() 
			, RoleNameBinStr[19].c_str() 
			, RoleNameBinStr[20].c_str() 
			, RoleNameBinStr[21].c_str() 
			);
		_RD_NormalDB->SQLCMDWrite( Info.GUID , NULL , NULL , InsertCmd );
		

	}

	Sleep( 30000 );
	for( int i = 0 ; i < 400 ; i++ )
	{
		Sleep( 50 );
		_RD_NormalDB->Process( );

		_DBTransfer_Game->ReCalculate();
		if( _RD_NormalDB->DBConnect()->TotalWriteCmdCount() == 0 )
			break;

	}

	char SqlCmd[512];
	sprintf( SqlCmd , "UPDATE Table_CheckList SET KeyStr = 'ObjNameStringDB' , CheckSum = %d" , TempData->StringCount );
	_RD_NormalDB->SQLCMDWrite( rand() , NULL , NULL , SqlCmd );

	delete TempData;
}
//--------------------------------------------------------------------------------------
struct FindDBIDRoleName
{
	int		DBID;
	string	RoleName;
	int		CliDBID;

	bool	IsFind;
};
void CNetDC_OtherChild::RL_Find_DBID_RoleName( int CliDBID , int DBID , const char* RoleName )
{
	FindDBIDRoleName* TempData = NEW(FindDBIDRoleName);

	TempData->CliDBID = CliDBID;
	TempData->DBID = DBID;
	TempData->RoleName = RoleName;
	TempData->IsFind = false;
	_RD_NormalDB->SqlCmd( CliDBID , _SQLCmdFind_DBD_RoleName_Event , _SQLCmdFind_DBD_RoleName_EventResult , TempData  , NULL );
}

bool CNetDC_OtherChild::_SQLCmdFind_DBD_RoleName_Event( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char	Buf[512];
	FindDBIDRoleName* TempData = (FindDBIDRoleName*)UserObj;

	MyDbSqlExecClass	MyDBProc( sqlBase );

	if( TempData->DBID == -1 )
	{
		CharTransferClass	CharTransfer;
		CharTransfer.SetUtf8String( TempData->RoleName.c_str() );
		string RoleNameBinStr = StringToSqlX( (char*)CharTransfer.GetWCString() , CharTransfer.WCStrLen() * 2 , false );
		

		sprintf( Buf , "SELECT DBID FROM RoleData Where RoleName = CAST( %s AS nvarchar(4000) ) and IsDelete = 0" , RoleNameBinStr.c_str() );

		MyDBProc.SqlCmd( Buf );
		MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempData->DBID );
		MyDBProc.Read( );
		MyDBProc.Close( );

		if( TempData->DBID != -1 )
			TempData->IsFind = true;

	}
	else
	{
		wchar_t	RoleName[32];
		
		sprintf( Buf , "SELECT RoleName FROM RoleData Where DBID = %d and IsDelete = 0" , TempData->DBID );

		MyDBProc.SqlCmd( Buf );
		MyDBProc.Bind( 1, SQL_WCHAR , sizeof(RoleName) , (LPBYTE)&RoleName );

		if( MyDBProc.Read( ) )
			TempData->IsFind = true;
		MyDBProc.Close( );

		CharTransferClass	CharTransfer;
		CharTransfer.SetWCString( RoleName );
		TempData->RoleName = CharTransfer.GetUtf8String();
	}

	return true;

}
void CNetDC_OtherChild::_SQLCmdFind_DBD_RoleName_EventResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	FindDBIDRoleName* TempData = (FindDBIDRoleName*)UserObj;

	SC_Find_DBID_RoleName( TempData->CliDBID , TempData->DBID , TempData->RoleName.c_str() , TempData->IsFind );

	delete TempData;
}


/*
//初始載入
bool CNetDC_OtherChild::_SQLCmdInitProc_Global( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass	MyDBProc( sqlBase );

	MoneyExchangeKeyValueStruct Temp;	
	MyDBProc.SqlCmd( "SELECT KeyStr,Value0,Value1,Value2,Value3 FROM Table_MoneyExchange" );
	MyDBProc.Bind( 1, SQL_C_CHAR , sizeof(Temp.KeyStr)	, (LPBYTE)&Temp.KeyStr );
	MyDBProc.Bind( 2, SQL_C_LONG , SQL_C_DEFAULT 		, (LPBYTE)&Temp.Value[0] );
	MyDBProc.Bind( 3, SQL_C_LONG , SQL_C_DEFAULT 		, (LPBYTE)&Temp.Value[1] );
	MyDBProc.Bind( 4, SQL_C_LONG , SQL_C_DEFAULT 		, (LPBYTE)&Temp.Value[2] );
	MyDBProc.Bind( 5, SQL_C_LONG , SQL_C_DEFAULT 		, (LPBYTE)&Temp.Value[3] );

	while( MyDBProc.Read() )
	{
		_MoneyExchangeKeyValueList.push_back( Temp );
	};
	MyDBProc.Close( );

	return true;
}
void CNetDC_OtherChild::_SQLCmdInitProcResult_Global ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	_IsLoadMoneyExchangeTabledOK = true;

	MoneyExchangeKeyValueStruct*	TempData = new MoneyExchangeKeyValueStruct[_MoneyExchangeKeyValueList.size()];
	for( unsigned i = 0 ; i < _MoneyExchangeKeyValueList.size() ; i++ )
	{
		TempData[i] = _MoneyExchangeKeyValueList[i];
	}

	_MoneyExchangeKeyValueZipSize = _MyLzoZip.Encode( (char*)TempData , sizeof(MoneyExchangeKeyValueStruct)*_MoneyExchangeKeyValueList.size() , _MoneyExchangeKeyValueZip );

	delete TempData;
}
*/
void CNetDC_OtherChild::OnLocalSrvConnect( int SrvID )
{
	SL_ExchangeMoneyTable( SrvID, int( _MoneyExchangeKeyValueList.size() ) ,  _MoneyExchangeKeyValueZipSize , _MoneyExchangeKeyValueZip );
	SL_DBStringTable( SrvID, int( _StringDBList.size() ) ,  _StringDBZipSize , _StringDBZip );
	SendSysDBKeyValue( SrvID );
	SL_WeekInstances( SrvID , _WeekInst );
	SL_AllItemCombinInfo( SrvID );

}

struct TempSelectDBStruct
{
	int		PlayerDBID;
	string	DataType;
	string	SelectCmd;

	vector< string > Result;
};

void CNetDC_OtherChild::RL_SelectDB( int PlayerDBID , const char* DataType , const char* SelectCmd )
{
	TempSelectDBStruct* TempData = NEW(TempSelectDBStruct);	
	TempData->PlayerDBID = PlayerDBID;
	TempData->DataType = DataType;
	TempData->SelectCmd = SelectCmd;

	_RD_NormalDB->SqlCmd( rand() , _SQLCmdSqlSelectCMD , _SQLCmdSqlSelectCMDResult , TempData  , NULL );
}

bool CNetDC_OtherChild::_SQLCmdSqlSelectCMD( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempSelectDBStruct* TempData = (TempSelectDBStruct*)UserObj;	

	MyDbSqlExecClass	MyDBProc( sqlBase );

	float   floatData[20];
	int		intData[20];
	char	charData[20][100];

	MyDBProc.SqlCmd( TempData->SelectCmd.c_str() );
	
	for( unsigned i = 0 ; i < TempData->DataType.size() ; i++ )
	{
		if( TempData->DataType.c_str()[i] == 'I' )
		{
			MyDBProc.Bind( i+1 , SQL_C_DEFAULT , 100	, (LPBYTE)&intData[i] );
		}
		else if( TempData->DataType.c_str()[i] == 'C' )
		{
			MyDBProc.Bind( i+1 , SQL_C_CHAR , 100	, (LPBYTE)&charData[i] );
		}
		else if( TempData->DataType.c_str()[i] == 'F' )
		{
			MyDBProc.Bind( i+1 , SQL_C_DEFAULT , 100	, (LPBYTE)&intData[i] );
		}

	}

	char	ResultStr[4096];

	for( int i = 0 ; i < 100 ; i++ )
	{
		if(  MyDBProc.Read() == false )
			break;

		ResultStr[0] = 0;
		for( unsigned i = 0 ; i < TempData->DataType.size() ; i++ )
		{
			if( TempData->DataType.c_str()[i] == 'I' )
			{
				sprintf( ResultStr , "%s %d" , ResultStr , intData[i] );
			}
			else if( TempData->DataType.c_str()[i] == 'C' )
			{
				sprintf( ResultStr , "%s %s" , ResultStr , charData[i] );
			}
			else if( TempData->DataType.c_str()[i] == 'F' )
			{
				sprintf( ResultStr , "%s %f" , ResultStr , floatData[i] );
			}
		}
		TempData->Result.push_back( ResultStr );
	}

	return true;
}

void CNetDC_OtherChild::_SQLCmdSqlSelectCMDResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempSelectDBStruct* TempData = (TempSelectDBStruct*)UserObj;	

	for( unsigned i = 0 ; i < TempData->Result.size() ; i++ )
	{
		Net_Talk::SysMsg( TempData->PlayerDBID , TempData->Result[i].c_str() );	
	}

	delete TempData;
}

void CNetDC_OtherChild::ReloadStringDB()
{
	CNetDC_OtherChild::_IsLoadStringDBTabledOK = false;
	_RD_NormalDB_Import->SqlCmd( 0 ,  CNetDC_OtherChild::_SQLCmdLoadStringDB , CNetDC_OtherChild::_SQLCmdLoadStringDBResult , NULL , NULL );

}

struct TempClientDataStruct
{
	int		Size;
	char	Data[1024];
};
struct TempClientLoadDataStruct
{
	int PlayerDBID;
	int	KeyID;
	vector< TempClientDataStruct > List;
};


void CNetDC_OtherChild::RL_LoadClientData( int PlayerDBID , int KeyID )
{
	TempClientLoadDataStruct* TempData = NEW TempClientLoadDataStruct;
	TempData->PlayerDBID = PlayerDBID;
	TempData->KeyID = KeyID;
	_RD_NormalDB->SqlCmd( PlayerDBID , _SQLCmdLoadClientDataCMD ,_SQLCmdLoadClientDataResult , TempData , NULL );
}
bool CNetDC_OtherChild::_SQLCmdLoadClientDataCMD( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempClientLoadDataStruct* TempData = ( TempClientLoadDataStruct* ) UserObj;

	TempClientDataStruct TempCliData;
	char	SqlCmd[2048];
	MyDbSqlExecClass	MyDBProc( sqlBase );

	sprintf( SqlCmd , "SELECT Size , Data FROM Table_ClientData WHERE DBID=%d and KeyID=%d ORDER BY KeyID" , TempData->PlayerDBID , TempData->KeyID );

	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG	, SQL_C_DEFAULT				, (LPBYTE)&TempCliData.Size );
	MyDBProc.Bind( 2, SQL_C_BINARY  , sizeof(TempCliData.Data) 	, (LPBYTE)&TempCliData.Data );
	while( MyDBProc.Read() )
	{
		TempData->List.push_back( TempCliData );
	};
	MyDBProc.Close( );

	return true;
}
void CNetDC_OtherChild::_SQLCmdLoadClientDataResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempClientLoadDataStruct* TempData = ( TempClientLoadDataStruct* ) UserObj;
	
	if( TempData->List.size() == 0 )
	{
		SC_LoadClientDataResult( TempData->PlayerDBID , TempData->KeyID , 0 , "" );
	}
	else
	{

		char	Data[0x10000];
		if( TempData->List.size() * 1024 > 0x10000 )
		{
			Print(LV2 , "_SQLCmdLoadClientDataResult" , "TempData->List.size() * 1024 > 0x10000 (PlayerDBID=%d KeyID=%d)" , TempData->PlayerDBID , TempData->KeyID );
			return;
		}
		int	TotalSize = 0;
		for( unsigned i = 0 ; i < TempData->List.size() ; i++ )
		{
			if( TempData->List[i].Size > 1024 )
			{
				Print(LV2 , "_SQLCmdLoadClientDataResult" , "TempData->List[i].Size > 1024 (PlayerDBID=%d KeyID=%d)" , TempData->PlayerDBID , TempData->KeyID );
				break;
			}
			memcpy( Data + i*1024 , TempData->List[i].Data , TempData->List[i].Size );
			TotalSize += TempData->List[i].Size;

		}

		SC_LoadClientDataResult( TempData->PlayerDBID , TempData->KeyID , TotalSize , Data );
	}
	delete TempData;
}

struct TempClientLoadDataStruct_Account
{
	string Account;
	int	KeyID;
	int SockID;
	int ProxyID;
	vector< TempClientDataStruct > List;
};

void CNetDC_OtherChild::RM_LoadClientData_Account( int SockID , int ProxyID , const char* Account , int KeyID )
{
	TempClientLoadDataStruct_Account* TempData = NEW TempClientLoadDataStruct_Account;
	TempData->Account = Account;
	TempData->KeyID = KeyID;
	TempData->ProxyID = ProxyID;
	TempData->SockID = SockID;
	_RD_NormalDB->SqlCmd( SockID , _SQLCmdLoadClientDataCMD_Account ,_SQLCmdLoadClientDataResult_Account , TempData , NULL );
}

bool CNetDC_OtherChild::_SQLCmdLoadClientDataCMD_Account( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempClientLoadDataStruct_Account* TempData = ( TempClientLoadDataStruct_Account* ) UserObj;

	TempClientDataStruct TempCliData;
	char	SqlCmd[2048];
	MyDbSqlExecClass	MyDBProc( sqlBase );

	sprintf( SqlCmd , "SELECT Size , Data FROM Table_ClientData_Account WHERE Account='%s' and KeyID=%d ORDER BY KeyID" , TempData->Account.c_str() , TempData->KeyID );

	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG	, SQL_C_DEFAULT				, (LPBYTE)&TempCliData.Size );
	MyDBProc.Bind( 2, SQL_C_BINARY  , sizeof(TempCliData.Data) 	, (LPBYTE)&TempCliData.Data );
	while( MyDBProc.Read() )
	{
		TempData->List.push_back( TempCliData );
	};
	MyDBProc.Close( );

	return true;
}
void CNetDC_OtherChild::_SQLCmdLoadClientDataResult_Account( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempClientLoadDataStruct_Account* TempData = ( TempClientLoadDataStruct_Account* ) UserObj;

	if( TempData->List.size() == 0 )
	{
		SC_LoadClientDataResult_Account( TempData->SockID , TempData->ProxyID , TempData->KeyID , 0 , "" );
	}
	else
	{

		char	Data[0x10000];
		if( TempData->List.size() * 1024 > 0x10000 )
		{
			Print(LV2 , "_SQLCmdLoadClientDataResult_Account" , "TempData->List.size() * 1024 > 0x10000 (Account=%s KeyID=%d)" , TempData->Account.c_str() , TempData->KeyID );
			return;
		}
		int	TotalSize = 0;
		for( unsigned i = 0 ; i < TempData->List.size() ; i++ )
		{
			if( TempData->List[i].Size > 1024 )
			{
				Print(LV2 , "_SQLCmdLoadClientDataResult_Account" , "TempData->List[i].Size > 1024 (Account=%s KeyID=%d)" , TempData->Account.c_str() , TempData->KeyID );
				break;
			}
			memcpy( Data + i*1024 , TempData->List[i].Data , TempData->List[i].Size );
			TotalSize += TempData->List[i].Size;

		}

		SC_LoadClientDataResult_Account( TempData->SockID , TempData->ProxyID , TempData->KeyID , TotalSize , Data );
	}
	delete TempData;

}

void	CNetDC_OtherChild::SendSysDBKeyValue( int SrvID )
{
	//_SysKeyValue
	map< string , int >::iterator Iter;
	int i = 0;
	for( Iter = _SysKeyValue.begin() ; Iter != _SysKeyValue.end() ; Iter++ , i++ )
	{
		SL_SysKeyValue( SrvID , _SysKeyValue.size() , i , (char*)Iter->first.c_str() , Iter->second );
	}
}

struct TempOpenSessionStruct
{
	std::string	Account;
	int			RoleDBID;
	int			WorldID;
	int			ZoneID;
	int			Type;
	int			SessionID;
	int			ValidTime;

	TempOpenSessionStruct()
	{
		Account   = "";
		RoleDBID  = 0;
		WorldID   = 0;
		ZoneID	  = 0;
		Type	  = 0;
		SessionID = 0;
		ValidTime = 0;
	}
};

struct TempCloseSessionStruct
{
	std::string	Account;
	int			RoleDBID;
	int			WorldID;
	int			ZoneID;
	int			Type;
	int			SessionID;

	TempCloseSessionStruct()
	{
		Account   = "";
		RoleDBID  = 0;
		WorldID   = 0;
		ZoneID	  = 0;
		Type	  = 0;
		SessionID = 0;
	}
};

void CNetDC_OtherChild::RL_OpenSession( const char* Account, int RoleDBID, int ZoneID, SessionTypeENUM Type, int SessionID, int ValidTime )
{
	if (_RD_NormalDB_External == NULL)
		return;

	TempOpenSessionStruct* TempData = NEW TempOpenSessionStruct();
	TempData->Account	= Account;
	TempData->RoleDBID	= RoleDBID;
	TempData->Type		= Type;
	TempData->WorldID	= _WorldID;
	TempData->ZoneID	= ZoneID;
	TempData->SessionID = SessionID;
	TempData->ValidTime = ValidTime;

	_RD_NormalDB_External->SqlCmd(RoleDBID , _SQLCmdOpenSession ,_SQLCmdOpenSessionResult , TempData , NULL);
}

void CNetDC_OtherChild::RL_CloseSession( const char* Account, int RoleDBID, int ZoneID, SessionTypeENUM Type, int SessionID )
{
	if (_RD_NormalDB_External == NULL)
		return;

	TempCloseSessionStruct* TempData = NEW TempCloseSessionStruct();
	TempData->Account	= Account;
	TempData->RoleDBID	= RoleDBID;
	TempData->Type		= Type;
	TempData->WorldID	= _WorldID;
	TempData->ZoneID	= ZoneID;
	TempData->SessionID = SessionID;

	_RD_NormalDB_External->SqlCmd(RoleDBID , _SQLCmdCloseSession ,_SQLCmdCloseSessionResult , TempData , NULL);
}

bool CNetDC_OtherChild::_SQLCmdOpenSession( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	string SPName = _Ini.Str("External_OpenSessionSP");

	if (SPName.empty())
		return false;

	TempOpenSessionStruct* pUserData = (TempOpenSessionStruct*)UserObj;

	MyDbSqlExecClass MyDBProc( sqlBase );		
	SQLHSTMT hstmt = MyDBProc.GetStatementHandle();
	
	try
	{
		char Buff[256];
		sprintf(Buff, "{call %s(?, ?, ?, ?, ?, ?, ?)}", SPName.c_str());

		SQLRETURN retcode;
		SQLLEN cbValue = SQL_NTS;

		retcode = SQLPrepare(hstmt, (SQLCHAR*)Buff, SQL_NTS);

		retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR	, SQL_CHAR		, 64			, 0, (SQLPOINTER)pUserData->Account.c_str()	, 64							, &cbValue);
		retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG	, SQL_INTEGER	, SQL_C_DEFAULT	, 0, &pUserData->RoleDBID					, sizeof(pUserData->RoleDBID)	, &cbValue);
		retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_LONG	, SQL_INTEGER	, SQL_C_DEFAULT	, 0, &pUserData->WorldID					, sizeof(pUserData->WorldID)	, &cbValue);
		retcode = SQLBindParameter(hstmt, 4, SQL_PARAM_INPUT, SQL_C_LONG	, SQL_INTEGER	, SQL_C_DEFAULT	, 0, &pUserData->ZoneID						, sizeof(pUserData->ZoneID)		, &cbValue);
		retcode = SQLBindParameter(hstmt, 5, SQL_PARAM_INPUT, SQL_C_LONG	, SQL_INTEGER	, SQL_C_DEFAULT	, 0, &pUserData->Type						, sizeof(pUserData->Type)		, &cbValue);
		retcode = SQLBindParameter(hstmt, 6, SQL_PARAM_INPUT, SQL_C_LONG	, SQL_INTEGER	, SQL_C_DEFAULT	, 0, &pUserData->SessionID					, sizeof(pUserData->SessionID)	, &cbValue);
		retcode = SQLBindParameter(hstmt, 7, SQL_PARAM_INPUT, SQL_C_LONG	, SQL_INTEGER	, SQL_C_DEFAULT	, 0, &pUserData->ValidTime					, sizeof(pUserData->ValidTime)	, &cbValue);
		retcode = SQLExecute(hstmt);
	}
	catch(...)
	{
		Print( LV5 , "ODBC ERROR", "CRITICAL ODBC DRIVER ERROR!" );
	}

	return true;
}

void CNetDC_OtherChild::_SQLCmdOpenSessionResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	delete UserObj;
}

bool CNetDC_OtherChild::_SQLCmdCloseSession( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{	
	string SPName = _Ini.Str( "External_CloseSessionSP");

	if (SPName.empty())
		return false;

	TempCloseSessionStruct* pUserData = (TempCloseSessionStruct*)UserObj;

	MyDbSqlExecClass MyDBProc( sqlBase );		
	SQLHSTMT hstmt = MyDBProc.GetStatementHandle();

	try
	{
		char Buff[256];
		sprintf(Buff, "{call %s(?, ?, ?, ?, ?, ?)}", SPName.c_str());

		SQLRETURN retcode;
		SQLLEN cbValue = SQL_NTS;

		retcode = SQLPrepare(hstmt, (SQLCHAR*)Buff, SQL_NTS);

		retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR	, SQL_CHAR		, 64			, 0, (SQLPOINTER)pUserData->Account.c_str()	, 64							, &cbValue);
		retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG	, SQL_INTEGER	, SQL_C_DEFAULT	, 0, &pUserData->RoleDBID					, sizeof(pUserData->RoleDBID)	, &cbValue);
		retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_LONG	, SQL_INTEGER	, SQL_C_DEFAULT	, 0, &pUserData->WorldID					, sizeof(pUserData->WorldID)	, &cbValue);
		retcode = SQLBindParameter(hstmt, 4, SQL_PARAM_INPUT, SQL_C_LONG	, SQL_INTEGER	, SQL_C_DEFAULT	, 0, &pUserData->ZoneID						, sizeof(pUserData->ZoneID)		, &cbValue);
		retcode = SQLBindParameter(hstmt, 5, SQL_PARAM_INPUT, SQL_C_LONG	, SQL_INTEGER	, SQL_C_DEFAULT	, 0, &pUserData->Type						, sizeof(pUserData->Type)		, &cbValue);
		retcode = SQLBindParameter(hstmt, 6, SQL_PARAM_INPUT, SQL_C_LONG	, SQL_INTEGER	, SQL_C_DEFAULT	, 0, &pUserData->SessionID					, sizeof(pUserData->SessionID)	, &cbValue);
		retcode = SQLExecute(hstmt);
	}
	catch(...)
	{
		Print( LV5 , "ODBC ERROR", "CRITICAL ODBC DRIVER ERROR!" );
	}

	return true;
}

void CNetDC_OtherChild::_SQLCmdCloseSessionResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	delete UserObj;
}
bool CNetDC_OtherChild::_SQLOnTimeBuffScheduleProcEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass	myDBProc( sqlBase );
	_BuffSchList.clear();
	BuffScheduleStruct tmpInfo;

	char	cmdBuf[4096];

	//sprintf( cmdBuf , "SELECT  ZoneID , CAST( EndTime as float) , BuffID , BuffLv FROM BuffSchedule Where  (StartTime < GETDATE()) and (EndTime > GETDATE()) and ( worldID=-1 or worldID = %d)" , _WorldID );
	sprintf( cmdBuf , "SELECT  ZoneID , (DATEDIFF(MINUTE, 0, EndTime) / (24.0 * 60.0)) , BuffID , BuffLv FROM BuffSchedule Where (StartTime < GETDATE()) and (EndTime > GETDATE()) and ( worldID=-1 or worldID = %d)" , _WorldID );

	myDBProc.SqlCmd( cmdBuf );
	myDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&tmpInfo.ZoneID );
	myDBProc.Bind( 2, SQL_C_FLOAT ,SQL_C_DEFAULT , (LPBYTE)&tmpInfo.EndTime );
	myDBProc.Bind( 3, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&tmpInfo.BuffID );
	myDBProc.Bind( 4, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&tmpInfo.BuffLv );
	while( myDBProc.Read( ) )
	{	
		_BuffSchList.push_back( tmpInfo );
	}
	myDBProc.Close();
	return true;

}
void CNetDC_OtherChild::_SQLOnTimeBuffScheduleProcEventResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	for( unsigned i = 0 ; i < _BuffSchList.size() ; i++ )
	{
		BuffScheduleStruct& info = _BuffSchList[i];
		int buffTime = TimeExchangeFloatToInt( info.EndTime ) - ( RoleDataEx::G_Now + RoleDataEx::G_TimeZone * 60 * 60) ;
		if( buffTime < 60 )
			continue;
		SL_BuffSchedule(  info.ZoneID , info.BuffID , info.BuffLv , buffTime );
	}
}

int CNetDC_OtherChild::_OntimeProc( SchedularInfo* Obj , void* InputClass )
{
	_RD_NormalDB_Import->SqlCmd(1 , _SQLOnTimeBuffScheduleProcEvent ,_SQLOnTimeBuffScheduleProcEventResult , NULL , NULL);
	Schedular()->Push( _OntimeProc , 10*1000 , NULL , NULL );
	return 0;
}

