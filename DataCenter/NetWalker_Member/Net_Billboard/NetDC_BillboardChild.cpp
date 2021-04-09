#include "NetDC_BillboardChild.h"
#include "../Net_DCBase/NetDC_DCBaseChild.h"
//--------------------------------------------------------------------------------------
bool	CNetDC_BillboardChild::_IsInitReady = false;
int		CNetDC_BillboardChild::_NextUpdateTime;
bool	CNetDC_BillboardChild::_IsResetBillboard = false;

map< int , NewBillboardInfoBaseStruct >	CNetDC_BillboardChild::SortTypeRankMap;
//--------------------------------------------------------------------------------------
bool    CNetDC_BillboardChild::Init()
{
	CNetDC_Billboard::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetDC_BillboardChild );

	_RD_BillBoardDB->SqlCmd( 0 , _SQLCmdInitProc , _SQLCmdInitProcResult , NULL	, NULL );

	printf( "\n");
	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 1000 );
		_RD_BillBoardDB->Process( );

		switch( i % 4 )
		{
		case 0:
			printf( "\rBillboard Info Loading...../        " );
			break;
		case 1:
			printf( "\rBillboard Info Loading.....|        " );
			break;
		case 2:			
			printf( "\rBillboard Info Loading.....\\        " );
			break;
		case 3:
			printf( "\rBillboard Info Loading.....-        " );
			break;
		}

		if( _IsInitReady != false )
			break;
	}
	printf( "\n");

	//隔天6點更新
	_NextUpdateTime = TimeStr::Now_Value() / ( 60*60*24 )*( 60*60*24 ) + ( 60*60* (24+6 - Global::_TimeZone ) );

	Schedular()->Push( _OnTimeProc , 1000*( _NextUpdateTime - TimeStr::Now_Value() ) , This , NULL );	

	Schedular()->Push( _NewOnTimeProc , 1000 , This , NULL );	
	return true;
}
//--------------------------------------------------------------------------------------
bool    CNetDC_BillboardChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	CNetDC_Billboard::_Release();


	return true;

}
//--------------------------------------------------------------------------------------
//定時處理
int CNetDC_BillboardChild::_OnTimeProc( SchedularInfo* Obj , void* InputClass )
{	
	if( Global::_IsDestory != false )
		return 0;

	_NextUpdateTime += ( 60*60*24 );

	_RD_BillBoardDB->SqlCmd( 0 , _SQLCmdInitProc , _SQLCmdInitProcResult , NULL	, NULL );

	Schedular()->Push( _OnTimeProc , 1000*60*60*24 , This , NULL );	
	return 0;
}

//要求排名資料 RankBegin = -1 代表取自己等級附近
struct TempBillboardRankRequestInfoStruct
{
	int CliDBID;
	int CliSockID;
	int CliProxyID;
	BillBoardMainTypeENUM Type;
	int	RankBegin;
	int	RankMax;
};
void CNetDC_BillboardChild::R_RankInfoRequest( int CliDBID , int CliSockID , int CliProxyID , BillBoardMainTypeENUM Type , int RankBegin )
{
	TempBillboardRankRequestInfoStruct* Data = NEW( TempBillboardRankRequestInfoStruct );
	Data->CliDBID = CliDBID;
	Data->CliSockID = CliSockID;
	Data->CliProxyID = CliProxyID;
	Data->Type = Type;
	Data->RankBegin = RankBegin;

	_RD_BillBoardDB->SqlCmd( 0 , _SQLCmdReadProc , _SQLCmdReadProcResult , Data	, NULL );
}

void CNetDC_BillboardChild::R_SetAnonymous		( int CliDBID , bool IsAnonymous )
{
	_RD_BillBoardDB->SqlCmd( 0 , _SQLCmdSetAnonymousProc , _SQLCmdSetAnonymousProcResult , NULL	
		, "CliDBID"		, EM_ValueType_Int , (int)CliDBID 
		, "IsAnonymous" , EM_ValueType_Int , (int)IsAnonymous , NULL );
}

//初始設定
bool CNetDC_BillboardChild::_SQLCmdInitProc( vector<DB_BillBoardRoleInfoStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{

	MyDbSqlExecClass	MyDBProc( sqlBase );

	//暫時不處理 排行榜資料
	//MyDBProc.SqlCmd_WriteOneLine( "EXECUTE SortBillBoard" );
	return true;
}
void CNetDC_BillboardChild::_SQLCmdInitProcResult ( vector<DB_BillBoardRoleInfoStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	_IsInitReady = true;
	if( ResultOK == false )
	{
		printf( "\n排行榜初始失敗" );
	}
}

//讀取名次資訊
bool CNetDC_BillboardChild::_SQLCmdReadProc( vector<DB_BillBoardRoleInfoStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempBillboardRankRequestInfoStruct* UserData = (TempBillboardRankRequestInfoStruct*)UserObj;
	MyDbSqlExecClass	MyDBProc( sqlBase );
	char	Buf[256];
	char*	TypeName;

	switch( UserData->Type )
	{
	case EM_BillBoardMainType_Money:
		TypeName = "Rank_Money";
		break;
	case EM_BillBoardMainType_Title:
		TypeName = "Rank_TitleCount";
		break;
	case EM_BillBoardMainType_PK:		
		TypeName = "Rank_PKScore";
		break;

	case EM_BillBoardMainType_Job_Warrior:
		TypeName = "Rank_Job1";
		break;
	case EM_BillBoardMainType_Job_Ranger:
		TypeName = "Rank_Job2";
		break;
	case EM_BillBoardMainType_Job_Rogue:
		TypeName = "Rank_Job3";
		break;
	case EM_BillBoardMainType_Job_Wizard:
		TypeName = "Rank_Job4";
		break;
	case EM_BillBoardMainType_Job_Priest:
		TypeName = "Rank_Job5";
		break;
	case EM_BillBoardMainType_Job_Knight:	
		TypeName = "Rank_Job6";
		break;
	case EM_BillBoardMainType_Job_RuneDancer:
		TypeName = "Rank_Job7";
		break;
	case EM_BillBoardMainType_Job_Druid:		
		TypeName = "Rank_Job8";
		break;
	default:
		return false;
	}
	
	
	sprintf( Buf , "select Max(%s) from BillboardRoleInfo" , TypeName );
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind(  1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&UserData->RankMax );
	MyDBProc.Read();
	MyDBProc.Close();


	//表示找自己Rank附近
	if( UserData->RankBegin == -1 )
	{	
		sprintf( Buf , "SELECT %s FROM BillBoardRoleInfo WHERE DBID = %d" , TypeName , UserData->CliDBID );
		MyDBProc.SqlCmd( Buf );
		MyDBProc.Bind(  1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&UserData->RankBegin );
		MyDBProc.Read();
		MyDBProc.Close();
	}
	
	if( UserData->RankBegin <= 0 || UserData->RankBegin > UserData->RankMax )
	{
		//return false;
		UserData->RankBegin = UserData->RankMax;
	}
	char	WhereCmd[256];

	int	BeginRank = ( UserData->RankBegin - 1 ) /20 *20 + 1;
	int EndRank   = BeginRank + 19;

	UserData->RankBegin = BeginRank;

	sprintf( WhereCmd , "WHERE %s >= %d AND %s <= %d order by %s" , TypeName , BeginRank , TypeName, EndRank , TypeName );

	DB_BillBoardRoleInfoStruct TempBillBoardData;

	//g_CritSect()->Enter();
	MyDBProc.SqlCmd( _RDBillBoardSql->GetSelectStr(WhereCmd).c_str() );
	//g_CritSect()->Leave();
	

	MyDBProc.Bind( TempBillBoardData , RoleDataEx::ReaderRD_BillBoard() );

	while( MyDBProc.Read() )
	{	
		RoleDataEx::ReaderRD_BillBoard()->TransferWChartoUTF8( &TempBillBoardData , MyDBProc.wcTempBufList() );
		Data.push_back( TempBillBoardData );
	}

	MyDBProc.Close();

	return true;
}


void	SetBillBoardRoleBaseInfo( DB_BillBoardRoleInfoStruct& DB ,  BillBoardRoleInfoStruct& Info , int CliDBID )
{
//	Info.Rank			= DB.Rank_Money;
//	Info.MainRankInfo	= DB.Money;
	Info.IsAnonymous	= DB.IsAnonymous;	//匿名

	if( Info.IsAnonymous != false && DB.DBID != CliDBID )
		Info.RoleName = "--------"; 	
	else
		Info.RoleName = DB.RoleName;

	Info.DBID = DB.DBID;

	Info.Voc		= DB.Voc;		
	Info.Voc_Sub	= DB.Voc_Sub;	

	if( Info.Voc >= 1 && Info.Voc <= 8 )
	{
		Info.Lv = DB.JobLv[ Info.Voc - 1 ];
	}
	else
	{
		Info.Lv = 1;
	}

	if( Info.Voc_Sub >= 1 && Info.Voc_Sub <= 8 )
	{
		Info.Lv_Sub = DB.JobLv[ Info.Voc_Sub - 1 ];
	}
	else
	{
		Info.Lv_Sub = 0;
	}
								  
	Info.Money		= DB.Money;		
	Info.TitleCount	= DB.TitleCount;
	Info.PKScore	= DB.PKScore;	
}

void CNetDC_BillboardChild::_SQLCmdReadProcResult ( vector<DB_BillBoardRoleInfoStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempBillboardRankRequestInfoStruct* UserData = (TempBillboardRankRequestInfoStruct*)UserObj;

	if( ResultOK == false || Data.size() == 0 )
	{
		S_RankInfoResult( UserData->CliSockID , UserData->CliProxyID , UserData->Type ,-1 , -1 , UserData->RankMax ,  false );
	}
	else
	{
		int BeginRank;
		int	EndRank;

		BillBoardRoleInfoStruct TempBillBoardInfo;

		switch( UserData->Type )
		{
		case EM_BillBoardMainType_Money:
			{
				BeginRank = Data[0].Rank_Money;
				EndRank = Data[ Data.size() - 1 ].Rank_Money;
				S_RankInfoResult( UserData->CliSockID , UserData->CliProxyID , UserData->Type , BeginRank , EndRank ,  UserData->RankMax , true );
				
				for( int i = 0 ; i < (int)Data.size() ; i ++ )
				{
					TempBillBoardInfo.Rank			= Data[i].Rank_Money;
					TempBillBoardInfo.MainRankInfo	= Data[i].Money;
					SetBillBoardRoleBaseInfo( Data[i] , TempBillBoardInfo , UserData->CliDBID );
					S_RankInfoList( UserData->CliSockID , UserData->CliProxyID , TempBillBoardInfo );
				}
			}
			break;
		case EM_BillBoardMainType_Title:
			{
				BeginRank = Data[0].Rank_Title;
				EndRank = Data[ Data.size() - 1 ].Rank_Title;
				S_RankInfoResult( UserData->CliSockID , UserData->CliProxyID , UserData->Type , BeginRank , EndRank , UserData->RankMax , true );

				for( int i = 0 ; i < (int)Data.size() ; i ++ )
				{
					TempBillBoardInfo.Rank			= Data[i].Rank_Title;
					TempBillBoardInfo.MainRankInfo	= Data[i].TitleCount;
					SetBillBoardRoleBaseInfo( Data[i] , TempBillBoardInfo , UserData->CliDBID );
					S_RankInfoList( UserData->CliSockID , UserData->CliProxyID , TempBillBoardInfo );
				}
			}
			break;
		case EM_BillBoardMainType_PK:		
			{
				BeginRank = Data[0].Rank_PKScore;
				EndRank = Data[ Data.size() - 1 ].Rank_PKScore;
				S_RankInfoResult( UserData->CliSockID , UserData->CliProxyID , UserData->Type , BeginRank , EndRank , UserData->RankMax , true );

				for( int i = 0 ; i < (int)Data.size() ; i ++ )
				{
					TempBillBoardInfo.Rank			= Data[i].Rank_PKScore;
					TempBillBoardInfo.MainRankInfo	= Data[i].PKScore;
					SetBillBoardRoleBaseInfo( Data[i] , TempBillBoardInfo , UserData->CliDBID );
					S_RankInfoList( UserData->CliSockID , UserData->CliProxyID , TempBillBoardInfo );
				}
			}
			break;

		case EM_BillBoardMainType_Job_Warrior:
		case EM_BillBoardMainType_Job_Ranger:
		case EM_BillBoardMainType_Job_Rogue:
		case EM_BillBoardMainType_Job_Wizard:
		case EM_BillBoardMainType_Job_Priest:
		case EM_BillBoardMainType_Job_Knight:	
		case EM_BillBoardMainType_Job_RuneDancer:
		case EM_BillBoardMainType_Job_Druid:		
			{
				int JobID = UserData->Type - EM_BillBoardMainType_Job_Warrior;
				BeginRank = Data[0].Rank_Job[JobID];
				EndRank = Data[ Data.size() - 1 ].Rank_Job[JobID];
				S_RankInfoResult( UserData->CliSockID , UserData->CliProxyID , UserData->Type , BeginRank , EndRank , UserData->RankMax , true );

				for( int i = 0 ; i < (int)Data.size() ; i ++ )
				{
					TempBillBoardInfo.Rank			= Data[i].Rank_Job[JobID];
					TempBillBoardInfo.MainRankInfo	= Data[i].JobLv[JobID];
					SetBillBoardRoleBaseInfo( Data[i] , TempBillBoardInfo , UserData->CliDBID );
					S_RankInfoList( UserData->CliSockID , UserData->CliProxyID , TempBillBoardInfo );
				}
			}
			break;
		}

	}

	delete UserData;
}	

//設定匿名
bool CNetDC_BillboardChild::_SQLCmdSetAnonymousProc( vector<DB_BillBoardRoleInfoStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char	Buf[256];	
	int		CliDBID		= Arg.GetNumber( "CliDBID" );
	int		IsAnonymous = Arg.GetNumber( "IsAnonymous" );
	MyDbSqlExecClass	MyDBProc( sqlBase );

	sprintf( Buf , "UPDATE BillBoardRoleInfo SET IsAnonymous = %d WHERE DBID = %d" ,IsAnonymous , CliDBID );
	MyDBProc.SqlCmd_WriteOneLine( Buf );
	return true;
}
void CNetDC_BillboardChild::_SQLCmdSetAnonymousProcResult ( vector<DB_BillBoardRoleInfoStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{

}

//////////////////////////////////////////////////////////////////////////
struct TempNewSearchRankStruct
{
	int	PlayerDBID;
	int MinRank;
	int MaxRank;
	int SortType;
	vector< NewBillBoardRoleInfoStruct > Info;
	
	bool	IsSystemReset;	//系統排名重置 時要求的資料

	TempNewSearchRankStruct()
	{
		PlayerDBID = -1;
		IsSystemReset = false;
	}
};

void CNetDC_BillboardChild::RL_New_SearchRank		( int PlayerDBID , int SortType , int MinRank , int MaxRank )
{
	NewBillboardInfoBaseStruct& Info = SortTypeRankMap[SortType];
	

	if(		Info.NextUpdateTime <= TimeStr::Now_Value()  
		&&	Info.OnUpdate == false 	)
	{
		Info.OnUpdate = true;
		TempNewSearchRankStruct* TempData = NEW TempNewSearchRankStruct;
		TempData->PlayerDBID = PlayerDBID;
		TempData->SortType = SortType;
		TempData->MinRank = MinRank;
		TempData->MaxRank = MaxRank;
		
		_RD_NormalDB->SqlCmd( SortType , _SQLCmdNewUpdateRankProc , _SQLCmdNewUpdateRankProcResult , TempData , NULL );
	}
	else
	{
		SendSearchRankResult( PlayerDBID , SortType , MinRank , MaxRank );
	}
}

//讀取名次資訊
bool CNetDC_BillboardChild::_SQLCmdNewUpdateRankProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{

	TempNewSearchRankStruct* TempData = (TempNewSearchRankStruct*)UserObj;
	MyDbSqlExecClass	MyDBProc( sqlBase );
	CharTransferClass	CharTransfer;
	NewBillBoardRoleInfoStruct TempInfo;
	wchar_t	RoleName[32];
	wchar_t Note[64];
	char SqlCmd[512];
	sprintf( SqlCmd , "SELECT         TOP (200) BillBoardInfo.PlayerDBID, BillBoardInfo.SortValue, BillBoardInfo.LastRank, RoleData.RoleName, RoleData.LV, RoleData.LV_Sub, RoleData.Voc, RoleData.Voc_Sub, BillBoardInfo.Note,RoleData.GuildID FROM BillBoardInfo INNER JOIN RoleData ON BillBoardInfo.PlayerDBID = RoleData.DBID WHERE (BillBoardInfo.Type = %d) ORDER BY  BillBoardInfo.SortValue DESC" , TempData->SortType );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind(  1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.DBID );
	MyDBProc.Bind(  2, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.SortValue );
	MyDBProc.Bind(  3, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.LastRank );
	MyDBProc.Bind(  4, SQL_WCHAR, sizeof(RoleName) , (LPBYTE)&RoleName );
	MyDBProc.Bind(  5, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.Lv );
	MyDBProc.Bind(  6, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.Lv_Sub );
	MyDBProc.Bind(  7, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.Voc );
	MyDBProc.Bind(  8, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.Voc_Sub );
	MyDBProc.Bind(  9, SQL_WCHAR, sizeof(Note) , (LPBYTE)&Note );
	MyDBProc.Bind(  10, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempInfo.GuildID );

	int Rank = 0;
	while( MyDBProc.Read() )
	{
		Rank++;
		CharTransfer.SetWCString( RoleName );
		TempInfo.RoleName = CharTransfer.GetUtf8String();
		CharTransfer.SetWCString( Note );
		TempInfo.Note = CharTransfer.GetUtf8String();

		TempInfo.Rank = Rank;
		TempData->Info.push_back( TempInfo );
	}
	MyDBProc.Close();


	return true;
}
void CNetDC_BillboardChild::_SQLCmdNewUpdateRankProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempNewSearchRankStruct* TempData = (TempNewSearchRankStruct*)UserObj;
	NewBillboardInfoBaseStruct& Info = SortTypeRankMap[TempData->SortType];

	//如果正在做重置 則把資料丟棄
	Info.RankInfo = TempData->Info;
	Info.OnUpdate = false;
	Info.NextUpdateTime = TimeStr::Now_Value() + 20*60;
	
	if( TempData->IsSystemReset != false )
	{
		char SqlCmd[512];
		sprintf( SqlCmd , "DELETE FROM BillboardInfo WHERE Type = %d" , TempData->SortType );
		CNetDC_DCBaseChild::This->R_SqlCommand( TempData->SortType , EM_SqlCommandType_WorldDB ,  SqlCmd );

		for( unsigned i = 0 ; i < Info.RankInfo.size() ; i++ )
		{
			Info.RankInfo[i].LastRank = i + 1;
			Info.RankInfo[i].SortValue = 0;
			sprintf( SqlCmd , "INSERT BillBoardInfo(Type,PlayerDBID,SortValue,LastRank) VALUES(%d,%d,%d,%d)" , TempData->SortType , Info.RankInfo[i].DBID , 0 , Info.RankInfo[i].LastRank );
			CNetDC_DCBaseChild::This->R_SqlCommand( TempData->SortType , EM_SqlCommandType_WorldDB ,  SqlCmd );
		}

	}
	else
	{
		SendSearchRankResult( TempData->PlayerDBID , TempData->SortType , TempData->MinRank , TempData->MaxRank );
	}
	
	delete TempData;
}

struct TempNewSearchMyRankStruct
{
	int PlayerDBID;
	int SortType;

	//回傳
	int SortValue;
	int Rank;
	int LastRank;
	string Note;
	TempNewSearchMyRankStruct()
	{
		SortValue = 0;
		Rank = 0;
		LastRank = 0;
	}
};

void CNetDC_BillboardChild::RL_New_SearchMyRank		( int PlayerDBID , int SortType )
{
	TempNewSearchMyRankStruct* TempData = NEW TempNewSearchMyRankStruct;
	TempData->PlayerDBID = PlayerDBID;
	TempData->SortType = SortType;
	_RD_NormalDB->SqlCmd( SortType , _SQLCmdNewSearchMyRankProc , _SQLCmdNewSearchMyRankProcResult , TempData , NULL );
}

bool CNetDC_BillboardChild::_SQLCmdNewSearchMyRankProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempNewSearchMyRankStruct* TempData = (TempNewSearchMyRankStruct*)UserObj;
	MyDbSqlExecClass	MyDBProc( sqlBase );
	char SqlCmd[512];
	wchar_t Note[64];
	CharTransferClass	CharTransfer;

	bool IsFind = false;
	sprintf( SqlCmd , "SELECT SortValue, LastRank,Note FROM BillBoardInfo WHERE PlayerDBID=%d and Type =%d" , TempData->PlayerDBID , TempData->SortType );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind(  1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempData->SortValue );
	MyDBProc.Bind(  2, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempData->LastRank );
	MyDBProc.Bind(  3, SQL_WCHAR, sizeof(Note) , (LPBYTE)&Note );


	if( MyDBProc.Read() )
	{
		IsFind = true;
		CharTransfer.SetWCString( Note );
		TempData->Note = CharTransfer.GetUtf8String();
	}
	MyDBProc.Close();

	if( IsFind == false )
		return true;

	sprintf( SqlCmd , "SELECT COUNT(PlayerDBID) FROM BillBoardInfo WHERE SortValue >= %d and Type = %d" , TempData->SortValue , TempData->SortType );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempData->Rank );
	MyDBProc.Read();
	MyDBProc.Close();


	return true;
}
void CNetDC_BillboardChild::_SQLCmdNewSearchMyRankProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempNewSearchMyRankStruct* TempData = (TempNewSearchMyRankStruct*)UserObj;
	SC_New_SearchMyRankResult( TempData->PlayerDBID , TempData->SortType , TempData->Rank , TempData->SortValue , TempData->LastRank , TempData->Note.c_str() );
	delete TempData;
}

void CNetDC_BillboardChild::SendSearchRankResult( int PlayerDBID , int SortType , int MinRank , int MaxRank )
{
	NewBillboardInfoBaseStruct& Info = SortTypeRankMap[SortType];	

	if(		MinRank > Info.RankInfo.size() 
		||	MaxRank < 1 )
	{
		SC_New_SearchRankResult_Empty( PlayerDBID , SortType );
		return;
	}

	if( MinRank < 1 )
		MinRank = 1;
	
	if( MaxRank > Info.RankInfo.size() )
		MaxRank = Info.RankInfo.size();

	int TotalCount = MaxRank - MinRank + 1;

	for( unsigned i = 0 ; i < TotalCount; i++  )
		SC_New_SearchRankResult( PlayerDBID , SortType , TotalCount , i , Info.RankInfo[ MinRank-1+i ] );
}

struct TempNewRegisterStruct
{
	int PlayerDBID;
	int SortType;
	int SortValue;
	bool IsAdd;
	bool IsTopUpdate;
};
void CNetDC_BillboardChild::RL_New_Register( int PlayerDBID , int SortType , int SortValue , bool IsAdd , bool IsTopUpdate )
{

	TempNewRegisterStruct* TempData = NEW TempNewRegisterStruct;
	TempData->PlayerDBID = PlayerDBID;
	TempData->SortType = SortType;
	TempData->SortValue = SortValue;
	TempData->IsAdd = IsAdd;
	TempData->IsTopUpdate = IsTopUpdate;

	_RD_NormalDB->SqlCmd( SortType , _SQLCmdNewRegisterSortValueProc , _SQLCmdNewRegisterSortValueProcResult , TempData , NULL );
}
bool CNetDC_BillboardChild::_SQLCmdNewRegisterSortValueProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempNewRegisterStruct* TempData = (TempNewRegisterStruct*)UserObj;
	MyDbSqlExecClass	MyDBProc( sqlBase );
	//找資料是否存在
	bool	IsFind = false;
	int		Count = 0;
	char	SqlCmd[512];
	sprintf( SqlCmd , "SELECT COUNT(PlayerDBID) FROM BillBoardInfo WHERE PlayerDBID=%d and Type =%d" , TempData->PlayerDBID , TempData->SortType );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Count );
	MyDBProc.Read();
	MyDBProc.Close();

	if( Count == 0 )
		sprintf( SqlCmd , "INSERT BillBoardInfo(Type,PlayerDBID,SortValue) VALUES(%d,%d,%d)" , TempData->SortType , TempData->PlayerDBID , TempData->SortValue );
	else if( TempData->IsAdd )
		sprintf( SqlCmd , "UpDate BillBoardInfo Set SortValue = SortValue + %d WHERE PlayerDBID=%d and Type=%d" , TempData->SortValue , TempData->PlayerDBID , TempData->SortType );
	else if( TempData->IsTopUpdate )
		sprintf( SqlCmd , "UpDate BillBoardInfo Set SortValue = %d WHERE PlayerDBID=%d and Type=%d and SortValue < %d" , TempData->SortValue , TempData->PlayerDBID , TempData->SortType , TempData->SortValue );
	else
		sprintf( SqlCmd , "UpDate BillBoardInfo Set SortValue = %d WHERE PlayerDBID=%d and Type=%d" , TempData->SortValue , TempData->PlayerDBID , TempData->SortType );

	MyDBProc.SqlCmd_WriteOneLine( SqlCmd );
	return true;
}
void CNetDC_BillboardChild::_SQLCmdNewRegisterSortValueProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempNewRegisterStruct* TempData = (TempNewRegisterStruct*)UserObj;

	delete TempData;
}

//定時處理
int		CNetDC_BillboardChild::_NewOnTimeProc( SchedularInfo* Obj , void* InputClass )
{

	int NextCheckTime = (60*60) - TimeStr::Now_Value() % (60*60);
	if( NextCheckTime <= 60 )
		NextCheckTime = 60;


	Schedular()->Push( _NewOnTimeProc , 1000* NextCheckTime  , This , NULL );	
	if( _IsResetBillboard == false )
	{
		_IsResetBillboard = true;
		_RD_NormalDB_Import->SqlCmd( 8 , _SQLCmdNewOnTimeProc , _SQLCmdNewOnTimeProcResult , NULL , NULL );
	}
	return 0;
}
bool	CNetDC_BillboardChild::_SQLCmdNewOnTimeProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass MyDBProc( sqlBase );
	char	SqlCmd[512];
	int		SortType;
	//收尋所有需要 重設定排序類型
	sprintf( SqlCmd , "SELECT SortType FROM BillboardResetTimeTable WHERE NextUpdateTime%d < getdate()" , Global::GetWorldID() );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&SortType );
	while( MyDBProc.Read() )
	{
		Data.push_back( SortType );
	}
	MyDBProc.Close();

	for( unsigned i = 0 ; i < Data.size() ; i++ )
	{
		sprintf( SqlCmd , "UPDATE BillboardResetTimeTable Set NextUpdateTime%d = DATEADD( month , ResetMonth , NextUpdateTime%d ) WHERE SortType=%d" , Global::GetWorldID() , Global::GetWorldID() , Data[i] );
		MyDBProc.SqlCmd_WriteOneLine( SqlCmd );
	}

	return true;
}

void	CNetDC_BillboardChild::_SQLCmdNewOnTimeProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	_IsResetBillboard = false;

	//把所有需要更新的排名重新載入一次
	for( unsigned i = 0 ; i < Data.size() ; i++ )
	{
		NewBillboardInfoBaseStruct& Info = SortTypeRankMap[ Data[i] ];

		TempNewSearchRankStruct* TempData = NEW TempNewSearchRankStruct;
		TempData->SortType = Data[i];
		TempData->IsSystemReset = true;
		Info.OnUpdate = true;
		_RD_NormalDB->SqlCmd( TempData->SortType , _SQLCmdNewUpdateRankProc , _SQLCmdNewUpdateRankProcResult , TempData , NULL );
	}
}

struct TempRegisterNoteStruct
{
	int PlayerDBID;
	int SortType;
	string Note;
};

void	CNetDC_BillboardChild::RL_New_RegisterNote			( int PlayerDBID , int SortType , const char* Note )
{
	TempRegisterNoteStruct* TempData = NEW TempRegisterNoteStruct;
	TempData->PlayerDBID = PlayerDBID;
	TempData->SortType = SortType;
	TempData->Note = Note;
	_RD_NormalDB->SqlCmd( SortType , _SQLCmdNewRegisterNoteProc , _SQLCmdNewRegisterNoteProcResult , TempData , NULL );
}

bool CNetDC_BillboardChild::_SQLCmdNewRegisterNoteProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempRegisterNoteStruct* TempData = (TempRegisterNoteStruct*)UserObj;
	MyDbSqlExecClass	MyDBProc( sqlBase );
	//找資料是否存在
	bool	IsFind = false;
	int		Count = 0;
	char	SqlCmd[512];
	sprintf( SqlCmd , "SELECT COUNT(PlayerDBID) FROM BillBoardInfo WHERE PlayerDBID=%d and Type =%d" , TempData->PlayerDBID , TempData->SortType );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Count );
	MyDBProc.Read();
	MyDBProc.Close();

	CharTransferClass	CharTransfer;
	CharTransfer.SetUtf8String( TempData->Note.c_str() );

	string NoteBinStr = StringToSqlX( (char*)CharTransfer.GetWCString() , CharTransfer.WCStrLen() * 2 , false );

	if( Count == 0 )
		sprintf( SqlCmd , "INSERT BillBoardInfo(Type,PlayerDBID,Note) VALUES(%d,%d, CAST( %s AS nvarchar(4000) ))" , TempData->SortType , TempData->PlayerDBID , NoteBinStr.c_str() );
	else
		sprintf( SqlCmd , "UpDate BillBoardInfo Set Note=CAST( %s AS nvarchar(4000) ) WHERE PlayerDBID=%d and Type=%d" , NoteBinStr.c_str() , TempData->PlayerDBID , TempData->SortType );

	MyDBProc.SqlCmd_WriteOneLine( SqlCmd );
	return true;
}
void CNetDC_BillboardChild::_SQLCmdNewRegisterNoteProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempRegisterNoteStruct* TempData = (TempRegisterNoteStruct*)UserObj;


	delete TempData;
}
