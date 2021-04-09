#include "NetDC_DCBaseChild.h"	
#include "ReaderClass/CreateDBCmdClass.h"
#include "ReaderClass/DbSqlExecClass.h"
//#include "../Net_Login/NetDC_Login_Child.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
#include "StackWalker/StackWalker.h"
#include "FileVersion/FileVersion.h"
#include "../Net_Instance/Net_InstanceChild.h"
#include "../../AuroraAgentCli/AuroraAgentCli.h"
//-------------------------------------------------------------------
deque< DBNPCSaveInfoStruct >					CNetDC_DCBaseChild::_SaveNPCList;
//所有NPC移動旗子的資訊
map< int , vector< DB_NPC_MoveBaseStruct > >	CNetDC_DCBaseChild::_NPCMoveFlagInfo;
bool											CNetDC_DCBaseChild::_IsLoadOK_NPCMoveFlagInfo = false;
//GlobalSettingInfo								CNetDC_DCBaseChild::_GlobalSetting;
vector<ZoneConfigBaseStruct>					CNetDC_DCBaseChild::_GlobalSettingList;
bool											CNetDC_DCBaseChild::_IsGmCommandQuery = false;
bool											CNetDC_DCBaseChild::_IsProcDBSqlCmdOK= false;
//要處理的Sql命令
vector< ProcessCmdStruct >						CNetDC_DCBaseChild::_SqlCmdList[4];
//-------------------------------------------------------------------
bool    CNetDC_DCBaseChild::Init()
{
	CNetDC_DCBase::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetDC_DCBaseChild );

	string  GameDataBase 	= _Ini.Str( "Game_DataBase" );
	string  GlobalDataBase	= _Ini.Str( "Global_DataBase" );
	string	ImportDataBase	= _Ini.Str( "Import_DataBase" );
	string	LogDataBase 	= _Ini.Str( "Log_DataBase" );


	_RD_NormalDB->SqlCmd		( 0 ,  _SQLCmdDBReadWriteTestEvent , _SQLCmdDBReadWriteTestResultEvent , NULL , "DBName" , EM_ValueType_String , GameDataBase.c_str() , NULL );
	_RD_NormalDB_Global->SqlCmd	( 0 ,  _SQLCmdDBReadWriteTestEvent , _SQLCmdDBReadWriteTestResultEvent , NULL , "DBName" , EM_ValueType_String , GlobalDataBase.c_str() , NULL );
	_RD_NormalDB_Import->SqlCmd	( 0 ,  _SQLCmdDBReadWriteTestEvent , _SQLCmdDBReadWriteTestResultEvent , NULL , "DBName" , EM_ValueType_String , ImportDataBase.c_str() , NULL );
	_RD_NormalDB_Log->SqlCmd	( 0 ,  _SQLCmdDBReadWriteTestEvent , _SQLCmdDBReadWriteTestResultEvent , NULL , "DBName" , EM_ValueType_String , LogDataBase.c_str() , NULL );

	Sleep( 1000 );
	_RD_NormalDB->Process( );
	_RD_NormalDB_Global->Process( );
	_RD_NormalDB_Import->Process( );
	_RD_NormalDB_Log->Process( );


	_RD_NormalDB_Import->SqlCmd( 0 ,  _SQLCmdLoadSqlCmdEvent , _SQLCmdLoadSqlCmdResultEvent , NULL , NULL );
	printf( "\n");

	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 1000 );
		_RD_NormalDB->Process( );
		_RD_NormalDB_Global->Process( );
		_RD_NormalDB_Import->Process( );
		_RD_NormalDB_Log->Process( );
		switch( i % 4 )
		{
		case 0:
			printf( "\rSql Command Process ...../        " );
			break;
		case 1:
			printf( "\rSql Command Process .....|        " );
			break;
		case 2:			
			printf( "\rSql Command Process .....\\        " );
			break;
		case 3:
			printf( "\rSql Command Process .....-        " );
			break;
		}

		if(		_IsProcDBSqlCmdOK != false 
			&&	_SqlCmdList[0].size() == 0 
			&&	_SqlCmdList[1].size() == 0 
			&&	_SqlCmdList[2].size() == 0 
			&&	_SqlCmdList[3].size() == 0 	)
		{
			break;
		}
	}
	printf( "\n");

	_RD_NormalDB_Global->SqlCmd( 0 ,  _SQLCmdLoadAllNPCMoveInfoEvent , _SQLCmdLoadAllNPCMoveInfoResultEvent , NULL , NULL );

	printf( "\n");

	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 1000 );
		_RD_NormalDB_Global->Process( );
		switch( i % 4 )
		{
		case 0:
			printf( "\rNPCMoveInfo Loading...../        " );
			break;
		case 1:
			printf( "\rNPCMoveInfo Loading.....|        " );
			break;
		case 2:			
			printf( "\rNPCMoveInfo Loading.....\\        " );
			break;
		case 3:
			printf( "\rNPCMoveInfo Loading.....-        " );
			break;
		}

		if( _IsLoadOK_NPCMoveFlagInfo != false )
			break;
	}
	printf( "\n");


    Schedular()->Push( _OnTimeProc , 5000 , This , NULL );  


	return true;
}

//--------------------------------------------------------------------------------------
bool    CNetDC_DCBaseChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	CNetDC_DCBase::_Release();
	return true;

}
//--------------------------------------------------------------------------------------
int  CNetDC_DCBaseChild::_OnTimeProc( SchedularInfo* Obj , void* InputClass )
{
	if( Global::_IsDestory != false )
		return 0;

    static int Count = 0;
	Count++;

    CNetDC_DCBaseChild* This = (CNetDC_DCBaseChild*)InputClass;
    DBNPCSaveInfoStruct* Data;

	g_CritSect()->Enter();
    if( _SaveNPCList.size() != 0 )
    {

        Data = &_SaveNPCList.front();
        if( Data->OnProcFlag != true )
        {
            Data->OnProcFlag = true;

			//獨立的Thread 處理
            _NPCDataDB->SqlCmd( 3 ,  _SQLCmdWriteNPCRequestEvent , _SQLCmdWriteNPCResultEvent , NULL , NULL );

        }
    }
	g_CritSect()->Leave();

	if( Global::_EnableWorldConfigTable != false &&  Count % 600 == 0  )
	{
		_RD_NormalDB->SqlCmd( rand() ,  _SQLCmdLoadGlobalInfoRequestEvent , _SQLCmdLoadGlobalInfoResultEvent , NULL, NULL );
	}

	if( Count % 50 == 0 && _IsGmCommandQuery == false )
	{
		_IsGmCommandQuery = true;
		_RD_NormalDB_Import->SqlCmd( rand() ,  _SQLCmdLoadGMCommandEvent , _SQLCmdLoadGMCommandResultEvent , NULL, NULL );
	}

    Schedular()->Push( _OnTimeProc , 100 , This , NULL );  
    return 0;
}

bool CNetDC_DCBaseChild::_SQLCmdLoadGMCommandEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char	Buf[2048];
	wchar_t	wCommand[512];
	MyDbSqlExecClass	MyDBProc( sqlBase );
	MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );

	sprintf( Buf , "SELECT GUID , ManageLv , PlayerDBID , Command , SendOffline FROM GMCommandRequest WITH (TABLOCKX) WHERE Type = 0 and WorldID = %d" , Global::GetWorldID() );
	MyDBProc.SqlCmd( Buf );
	GmCommandStruct TempCmd;
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempCmd.GUID );
	MyDBProc.Bind( 2, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempCmd.ManageLv );
	MyDBProc.Bind( 3, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempCmd.PlayerDBID );
	MyDBProc.Bind( 4, SQL_C_WCHAR, sizeof( wCommand ) , (LPBYTE)&wCommand );
	MyDBProc.Bind( 5, SQL_C_BIT, SQL_C_DEFAULT , (LPBYTE)&TempCmd.IsSendOffline );
	while( MyDBProc.Read() )
	{
		CharTransferClass	charTransfer;
		charTransfer.SetWCString( wCommand );
		
		GmCommandStruct* NewTemp = new GmCommandStruct;
		*NewTemp = TempCmd;
		int CmdStrSize = sizeof( NewTemp->Command )-1;
		MyStrcpy( NewTemp->Command , charTransfer.GetUtf8String() , CmdStrSize );
		Data.push_back( int(NewTemp) );
	}
	MyDBProc.Close();

	if( Data.size() )
	{
		sprintf( Buf , "UPDATE GMCommandRequest SET Type = 1 WHERE Type = 0 and WorldID = %d" , Global::GetWorldID() );
		MyDBProc.SqlCmd_WriteOneLine( Buf );
	}

	MyDBProc.SqlCmd_WriteOneLine( "COMMIT TRANSACTION" );
	return true;
}

void CNetDC_DCBaseChild::_SQLCmdLoadGMCommandResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	_IsGmCommandQuery = false;
	for( int i = 0 ; i < (int)Data.size() ; i++ )
	{
		GmCommandStruct* TempCmd = (GmCommandStruct*)(Data[i]);

		OnlinePlayerInfoStruct* Info = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( TempCmd->PlayerDBID );
		if( Info == NULL )
		{
			if( TempCmd->IsSendOffline )
			{
				CharTransferClass	charTransfer;
				charTransfer.SetUtf8String( TempCmd->Command );
				string MsgCmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );

				char Buf[ 8192 ];
				sprintf( Buf , "BEGIN declare @Message nvarchar( 512 );SET @Message=CAST( %s AS nvarchar(4000));EXECUTE CreateOfflineGMCommand %d,%d,@Message; END" 
					, MsgCmdBinStr.c_str()	
					,TempCmd->PlayerDBID
					,TempCmd->ManageLv );

				//Net_DCBase::SqlCommand( rand() , Buf );
				This->R_SqlCommand( rand() , EM_SqlCommandType_WorldDB , Buf );

				SaveGMCommandResult( TempCmd->GUID , EM_GmCommandResult_OfflineMsg , "Send offline" );
			}
			else
				SaveGMCommandResult( TempCmd->GUID , EM_GmCommandResult_Failed , "Player DBID Not Find" );
			continue;
		}
		//
		SL_GmCommandRequest( Info->ZoneID , *TempCmd );

		delete TempCmd;
	}
}


struct TempSaveGMCommandResultStruct
{
	int GUID;
	GmCommandResultENUM	Result;
	string ResultStr;
};

void CNetDC_DCBaseChild::SaveGMCommandResult			( int GUID , GmCommandResultENUM Result, char ResultStr[512] )
{
	TempSaveGMCommandResultStruct* Temp = new TempSaveGMCommandResultStruct;
	Temp->GUID = GUID;
	Temp->Result = Result;
	Temp->ResultStr = ResultStr;
	_RD_NormalDB_Import->SqlCmd( 0 ,  _SQLCmdSaveGMCommandEvent , _SQLCmdSaveGMCommandResultEvent , Temp , NULL );
}


//儲存GM回傳結果
bool CNetDC_DCBaseChild::_SQLCmdSaveGMCommandEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempSaveGMCommandResultStruct* Temp = ( TempSaveGMCommandResultStruct* ) UserObj;
	
	char	Buf[2048];
	MyDbSqlExecClass	MyDBProc( sqlBase );

	switch( Temp->Result )
	{
	case EM_GmCommandResult_Failed:
		sprintf( Buf , "UPDATE GMCommandRequest SET Type = 3 , Result = '%s' WHERE GUID = %d" , Temp->ResultStr.c_str() , Temp->GUID );
		break;
	case EM_GmCommandResult_OK:
		sprintf( Buf , "UPDATE GMCommandRequest SET Type = 2 , Result = '%s' WHERE GUID = %d" , Temp->ResultStr.c_str() , Temp->GUID );
		break;
	case EM_GmCommandResult_OfflineMsg:
		sprintf( Buf , "UPDATE GMCommandRequest SET Type = 4 , Result = '%s' WHERE GUID = %d" , Temp->ResultStr.c_str() , Temp->GUID );
		break;
	default:
		return false;
	}
	MyDBProc.SqlCmd( Buf );
	return true;
}

void CNetDC_DCBaseChild::_SQLCmdSaveGMCommandResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempSaveGMCommandResultStruct* Temp = ( TempSaveGMCommandResultStruct* ) UserObj;

	delete Temp;
}




bool CNetDC_DCBaseChild::_SQLCmdWriteNPCRequestEvent( vector< NPCData >& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
    //MutilThread_CritSect          Thread_CritSect;

    char        Buf[256];
    bool        Result = true;


	MyDbSqlExecClass	MyDBProc( sqlBase );

	g_CritSect()->Enter();
	{
		DBNPCSaveInfoStruct* Data = &_SaveNPCList.front();
		//新角色		
		if( Data->NPC.DBID < 0 )
		{
			//檢查欄位是否已有資料
			//MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );		
			//sprintf( Buf , "SELECT MAX(DBID) AS Expr1 FROM NPCData WITH (TABLOCKX)" );
			sprintf( Buf , "SELECT MAX(DBID) AS Expr1 FROM NPCData" );

			MyDBProc.SqlCmd( Buf );
			MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Data->NPC.DBID );
			if( MyDBProc.Read() == false )
				Result = false;

			MyDBProc.Close();

			Data->NPC.DBID++;
			if( Result != false )
			{
				//2 資料儲存
				CreateDBCmdClass<NPCData> RoleDataSql( RoleDataEx::ReaderNPC() , "NPCData" );
				MyDBProc.SqlCmd_WriteOneLine( RoleDataSql.GetInsertStr( &Data->NPC ).c_str() );
			}
			//MyDBProc.SqlCmd_WriteOneLine( "COMMIT TRANSACTION" );
		}
		else
		{
			CreateDBCmdClass<NPCData> RoleDataSql( RoleDataEx::ReaderNPC() , "NPCData" );
			sprintf( Buf , "WHERE DBID = %d" , Data->NPC.DBID );
			MyDBProc.SqlCmd_WriteOneLine( RoleDataSql.GetUpDateStr( &Data->NPC , Buf ).c_str() );
		}	
	}
	g_CritSect()->Leave();

	return Result;
}

void CNetDC_DCBaseChild::_SQLCmdWriteNPCResultEvent ( vector< NPCData >& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
    

    if( _SaveNPCList.size() == 0 )
    {
        Print( Def_PrintLV4 , "_SQLCmdWriteNPCResultEvent" , "_DBWriteNPCDBEventCB _SaveNPCList.size() == 0 ?????" );
        return;
    }

	g_CritSect()->Enter();
		DBNPCSaveInfoStruct CmdInfo = _SaveNPCList.front();
	g_CritSect()->Leave();

    if( CmdInfo.OnProcFlag == false )
    {
        Print( Def_PrintLV4 , "_SQLCmdWriteNPCResultEvent" , "_DBWriteNPCDBEventCB Data->OnProcFlag == false ?????" );
        return;
    }

    if( ResultOK != false )
    {
        SaveNPCResultOK( CmdInfo.SrvNetID , CmdInfo.LocalObjID , CmdInfo.NPC.DBID );
    }
    else
    {
        SaveNPCResultFailed  ( CmdInfo.SrvNetID , CmdInfo.LocalObjID );
        Print( Def_PrintLV4 , "_SQLCmdWriteNPCResultEvent" , "_DBWriteNPCDBEventCB DB Save Error ?????" );
    }

	g_CritSect()->Enter();
		_SaveNPCList.pop_front();
	g_CritSect()->Leave();
}


//--------------------------------------------------------------------------------------
/*
void CNetDC_DCBaseChild::SavePlayer         ( int SrvNetID , int SaveTime , int DBID , int Start , int Size , void* Data )
{
	Print( Def_PrintLV1 , "SavePlayer" , "要求儲存資料 DBID = %d Start = %d Size = %d" , DBID , Start , Size );
    BaseItemObject* Obj = BaseItemObject::GetObj_DBID( DBID );
    if( Obj == NULL )
	{
		Print( Def_PrintLV4 , "SavePlayer" , "要求除存資料角色已經被登出 DBID = %d Start = %d Size = %d" , DBID , Start , Size );
        SavePlayerFailed( SrvNetID , DBID );
        return;
    }
    if( Start < 0 || Start + Size > sizeof( RoleData ) )
    {
        Print( Def_PrintLV4 , "SavePlayer" , "要求儲存資料範圍有問題 DBID = %d Start = %d Size = %d" , DBID , Start , Size );
        SavePlayerFailed( SrvNetID , DBID );
        return;
    }
    RoleDataEx* Role = Obj->Role();
    
    memcpy( (char*)Role , ((char*)Data) + Start  , Size );
	Role->BaseData.ZoneID = Role->BaseData.ZoneID % _DEF_ZONE_BASE_COUNT_;
    //生存期加十分鐘
    //Obj->SetLiveTime( 60*10*1000 );
	Obj->IsNeedSave( true );

	SaveTime /= 5000;
	if( Obj->SaveTime() > SaveTime )
		Obj->SaveTime( SaveTime );
}
*/
void CNetDC_DCBaseChild::SaveNPCRequest  ( int SrvNetID , int LocalObjID , RoleDataEx* NPC )
{
	if( 	NPC->BaseData.__MoneyCheckPoint			!= _MEMORY_CHECK_VALUE_ 
		||	NPC->SelfData.__MoneyCheckPoint			!= _MEMORY_CHECK_VALUE_  )
	{
		//資料有偏
		Print( LV4 , "SaveNPCRequest" , "__MoneyCheckPoint != _MEMORY_CHECK_VALUE_ " );
		return;
	}


    DBNPCSaveInfoStruct CmderInfo;
    CmderInfo.SrvNetID      = SrvNetID;
    CmderInfo.LocalObjID    = LocalObjID;
    NPC->GetNPCData( &CmderInfo.NPC );

	g_CritSect()->Enter();
		_SaveNPCList.push_back( CmderInfo );
	g_CritSect()->Leave();
    
}
void CNetDC_DCBaseChild::DropNPCRequest     ( int SrvNetID , int LocalObjID , int NPCDBID , bool IsDelFlag , const char* DestroyAccount )
{
    DBRoleTempDataInfo* UserData = _ReadRole->NewObj();
    UserData->LocalObjID = LocalObjID;
    UserData->DBID       = NPCDBID;
    UserData->SrvID      = SrvNetID;

    char SQLCmd[1024];
	if( IsDelFlag )
	{
		sprintf( SQLCmd , "DELETE NPCData WHERE (DBID = %d)" , NPCDBID );
	}
	else
	{
		sprintf( SQLCmd , "UPDATE NPCData SET IsDelflag = 1 , DestroyTime = getdate() , DestroyAccount='%s' WHERE (DBID = %d)" , DestroyAccount , NPCDBID );
	}
	_NPCDataDB->SQLCMDWrite( rand() , _DBDropNPCDBEventCB , UserData ,  SQLCmd );
}
void CNetDC_DCBaseChild::_DBDropNPCDBEventCB ( NPCData *Data , void* UserObj , char *Cmd , bool ResultOK )
{
    DBRoleTempDataInfo* UserData = (DBRoleTempDataInfo*)UserObj;
    _ReadRole->DeleteObj( UserData );

    if( ResultOK != false )
    {
        DropNPCResultOK( UserData->SrvID , UserData->LocalObjID, UserData->DBID );
    }
    else
    {
        Print( Def_PrintLV3 , "_DBDropNPCDBEventCB" , "NPC 刪除錯誤!" );
        DropNPCResultFailed( UserData->SrvID , UserData->LocalObjID, UserData->DBID );
    }
}

void CNetDC_DCBaseChild::LoadAllNPC         ( int ZoneID )
{
//    char WhereCmd[1024];
    //sprintf( WhereCmd , "WHERE isDelFlag = 0 and ZoneID = %d ORDER BY OrgObjID DESC" , ZoneID % _DEF_ZONE_BASE_COUNT_ );
 //   _NPCDataDB->Read( _DBReadAllNPCDBEventCB , (void*)ZoneID , WhereCmd );
	_NPCDataDB->SqlCmd( ZoneID , _SQLCmdLoadAllNPCInfoEvent , _SQLCmdLoadAllNPCInfoResultEvent , (void*)ZoneID , NULL );

}
bool CNetDC_DCBaseChild::_SQLCmdLoadAllNPCInfoEvent( vector<NPCData>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	int ZoneID = int( UserObj );
	char WhereCmd[1024];
	sprintf( WhereCmd , "WHERE isDelFlag = 0 and ZoneID = %d ORDER BY OrgObjID DESC" , ZoneID % _DEF_ZONE_BASE_COUNT_ );

	CreateDBCmdClass<NPCData>* _NPCDataSql  = NEW CreateDBCmdClass<NPCData > ( RoleDataEx::ReaderNPC()  , "NPCData" );

	MyDbSqlExecClass MyDBProc( sqlBase );
	MyDBProc.SqlCmd( _NPCDataSql->GetSelectStr( WhereCmd ).c_str() );
	
	NPCData TempNpcData;

	MyDBProc.Bind( TempNpcData , RoleDataEx::ReaderNPC() );

	while( MyDBProc.Read() )
	{
		RoleDataEx::ReaderNPC()->TransferWChartoUTF8( &TempNpcData , MyDBProc.wcTempBufList() );
		Data.push_back( TempNpcData );
	}

	MyDBProc.Close();

	delete _NPCDataSql;
	return true;
}
void CNetDC_DCBaseChild::_SQLCmdLoadAllNPCInfoResultEvent ( vector<NPCData>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	int ZoneID = int( UserObj );
	if( ResultOK != false )
	{
		for( int i = 0 ; i < (int)Data.size() ; i++ )
		{
			LoadNPCResult( ZoneID , (int) Data.size() , i  , &Data[i] );        
		}

	}
	else
	{
		Print( Def_PrintLV3 , "_DBReadAllNPCDBEventCB" , "NPC 讀取錯誤!" );
	}
}
/*
void CNetDC_DCBaseChild::_DBReadAllNPCDBEventCB  ( vector<NPCData>& Data , void* UserObj , char *WhereCmd , bool ResultOK )
{
    int ZoneID = int( UserObj );
    if( ResultOK != false )
    {
        for( int i = 0 ; i < (int)Data.size() ; i++ )
        {
            LoadNPCResult( ZoneID , (int) Data.size() , i  , &Data[i] );        
        }
        
    }
    else
    {
        Print( Def_PrintLV3 , "_DBReadAllNPCDBEventCB" , "NPC 讀取錯誤!" );
    }
}
*/
/*
void CNetDC_DCBaseChild::SetPlayerLiveTime  ( int SrvNetID , int DBID , int LiveTime )
{
    BaseItemObject* Obj = BaseItemObject::GetObj_DBID( DBID );
    if( Obj == NULL )
    {
		Print( LV3 , "SetPlayerLiveTime" , "SetPlayerLiveTime　DBID(%d)　找不到" , DBID  );
        SetPlayerLiveTimeFailed( SrvNetID , DBID );
        return ;
    }

	Print( LV1 , "SetPlayerLiveTime" , "DBID(%d)　LiveTime(%d)" , DBID , LiveTime  );

    Obj->SetLiveTime( LiveTime );
    
}

void CNetDC_DCBaseChild::SetPlayerLiveTime_ByAccount  ( int SrvNetID , char* Account , int LiveTime )
{
	Print( LV1 , "SetPlayerLiveTime" , "Account(%s)　LiveTime(%d)" , Account , LiveTime  );

    vector< BaseItemObject*>* ObjList = BaseItemObject::GetObj_Account( Account );
    if( ObjList == NULL || ObjList->size() == 0 )
    {
        SetPlayerLiveTimeFailed_ByAccount( SrvNetID , Account );
        return ;
    }
    for( int i = 0 ; i < (int)ObjList->size() ; i++ )
    {
        (*ObjList)[i]->SetLiveTime( LiveTime );
    }

}
*/

void CNetDC_DCBaseChild::LoadAllNPCMoveInfo			( int ZoneID )
{
	//_NPCMoveFlagInfo
	map< int , vector< DB_NPC_MoveBaseStruct > >::iterator Iter;

	g_CritSect()->Enter();
	for( Iter = _NPCMoveFlagInfo.begin() ; Iter != _NPCMoveFlagInfo.end() ; Iter++ )
	{
		vector< DB_NPC_MoveBaseStruct >& List = Iter->second;
		if( List.size() == 0 )
			continue;
		if( List[0].ZoneID != ZoneID % _DEF_ZONE_BASE_COUNT_ )
			continue;

		int dbid = List[0].NPCDBID;


		for( unsigned int i = 0 ; i < List.size() ; i++ )
		{
			NPCMoveInfoResult( ZoneID , List[i].NPCDBID , (int)List.size() ,  List[i].IndexID , List[i].Base );
		}		
	}
	g_CritSect()->Leave();
}

void CNetDC_DCBaseChild::SaveNPCMoveInfo			( int TotalCount , int ZoneID , int NPCDBID , int IndexID , NPC_MoveBaseStruct& Base )
{
	ZoneID = ZoneID % _DEF_ZONE_BASE_COUNT_;
	if( IndexID < 0 || IndexID > TotalCount )
		return;
	if( NPCDBID < 0 )
		return;

	g_CritSect()->Enter();

		vector< DB_NPC_MoveBaseStruct >& List = _NPCMoveFlagInfo[ NPCDBID ];
		while( (int)List.size() > IndexID )
		{
			List.pop_back();
		}

		DB_NPC_MoveBaseStruct	Temp;
		Temp.ZoneID = ZoneID;
		Temp.NPCDBID = NPCDBID;
		Temp.IndexID = IndexID;
		Temp.Base = Base;

		List.push_back( Temp );

	g_CritSect()->Leave();


	if( TotalCount == (int)List.size() )
	{
		_RD_NormalDB_Global->SqlCmd( 0 ,  _SQLCmdWriteNPCMoveRequestEvent , _SQLCmdWriteNPCMoveResultEvent , NULL 
			,"NPCDBID"		, EM_ValueType_Int , NPCDBID
			, NULL );
	}

}



bool CNetDC_DCBaseChild::_SQLCmdWriteNPCMoveRequestEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	int	NPCDBID = Arg.GetNumber( "NPCDBID" );

	MyDbSqlExecClass		MyDBProc( sqlBase );

	g_CritSect()->Enter();
		vector< DB_NPC_MoveBaseStruct >& OrgList = _NPCMoveFlagInfo[ NPCDBID ];
		vector< DB_NPC_MoveBaseStruct >	List = OrgList;
	g_CritSect()->Leave();
	
	char Buf[512];
	sprintf( Buf , "DELETE NPC_MoveFlagDB WHERE NPCDBID = %d" , NPCDBID  );
	MyDBProc.SqlCmd_WriteOneLine( Buf );		

	for( unsigned int i = 0 ; i < List.size() ;i++ )
	{
		MyDBProc.SqlCmd_WriteOneLine( _RDNPCMoveSql->GetInsertStr( &(List[i]) ).c_str() );
	}

	return true;
}
void CNetDC_DCBaseChild::_SQLCmdWriteNPCMoveResultEvent ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{

}

bool CNetDC_DCBaseChild::_SQLCmdLoadAllNPCMoveInfoEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass		MyDBProc( sqlBase );
	DB_NPC_MoveBaseStruct	TempMoveBase;
	//g_CritSect()->Enter();
	MyDBProc.SqlCmd( _RDNPCMoveSql->GetSelectStr("Order By IndexID").c_str() );
	//g_CritSect()->Leave();

	MyDBProc.Bind( TempMoveBase , RoleDataEx::ReaderRD_NPC_MoveBase() );
	while( MyDBProc.Read() )
	{
		g_CritSect()->Enter();
		{
//			RoleDataEx::ReaderRD_NPC_MoveBase()->TransferWChartoUTF8( &TempMoveBase );
			vector< DB_NPC_MoveBaseStruct >& List = _NPCMoveFlagInfo[ TempMoveBase.NPCDBID ];

			if( List.size() != TempMoveBase.IndexID )
				printf( "Error ?? List Size() != TempMoveBase.IndexID 旗子沒有聯續" );
			while( (int)List.size() < TempMoveBase.IndexID )
			{
				DB_NPC_MoveBaseStruct	Temp;
				memset( &Temp , 0 , sizeof( Temp ) );
				Temp.IndexID = (int)List.size();
				Temp.NPCDBID = TempMoveBase.NPCDBID;
				Temp.ZoneID = TempMoveBase.ZoneID;
				List.push_back( Temp );
			}
			List.push_back( TempMoveBase );
		}
		g_CritSect()->Leave();

	}
	MyDBProc.Close();	

	return true;
}

void CNetDC_DCBaseChild::_SQLCmdLoadAllNPCMoveInfoResultEvent ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	_IsLoadOK_NPCMoveFlagInfo = true;
}


void CNetDC_DCBaseChild::R_SqlCommand( int ThreadID , SqlCommandTypeENUM Type , char* SQLCmd )
{
	switch( Type )
	{
	case EM_SqlCommandType_GlobalDB:
		_RD_NormalDB_Global->SQLCMDWrite( ThreadID , _SQL_DBWriteEvent , this , SQLCmd );
		break;
	case EM_SqlCommandType_ImportDB:
		_RD_NormalDB_Import->SQLCMDWrite( ThreadID , _SQL_DBWriteEvent , this , SQLCmd );
		break;
	case EM_SqlCommandType_WorldDB:
		_RD_NormalDB->SQLCMDWrite( ThreadID , _SQL_DBWriteEvent , this , SQLCmd );
		break;
	case EM_SqlCommandType_WorldLogDB:
		_RD_NormalDB_Log->SQLCMDWrite( ThreadID , _SQL_DBWriteEvent , this , SQLCmd );
		break;
	}
	
}

void  CNetDC_DCBaseChild::_SQL_DBWriteEvent( int *Data , void* UserObj , char *WhereCmd , bool ResultOK )
{
	if( ResultOK == false )
	{
		Print( LV1 , "_SQL_DBWriteEvent" , "SQL Command Error : %s" , WhereCmd );
	}
}

/*
void CNetDC_DCBaseChild::R_CheckRoleDataSize		( int SrvNetID , int Size )
{
	if( Size != sizeof(RoleData) )
	{
		S_CheckRoleDataSizeResult( SrvNetID , false );
		Print( LV3 , "R_CheckRoleDataSize" , "SrvNetID=%d sizeof(RoleData)=%d  DataCenter sizeof(RoleData) = %d" , SrvNetID , Size , sizeof(RoleData) );
	}
	else
		S_CheckRoleDataSizeResult( SrvNetID , true );

}
*/
//////////////////////////////////////////////////////////////////////////
struct InstancePlayStatusStruct
{
	int PartyKey;
	int KeyID;
	int KeyValue;
};

struct TempZoneInfoRequsetStruct
{
	int NetID;
	int ZoneID;

	//return
	LocalServerBaseDBInfoStruct LocalInfo;
	WeekDayStruct				ResetDay;

	vector< InstancePlayStatusStruct > InstancePlayStatusList;
};

void CNetDC_DCBaseChild::R_ZoneInfoRequest			( int NetID , int ZoneID ) 
{
	TempZoneInfoRequsetStruct* TempData = NEW(TempZoneInfoRequsetStruct);
	TempData->NetID = NetID;
	TempData->ZoneID = ZoneID;

//	_RD_NormalDB->SqlCmd( rand() ,  _SQLCmdLoadZoneInfoRequestEvent , _SQLCmdLoadZoneInfoResultEvent , TempData 
//				, NULL );
	_RD_NormalDB_Global->SqlCmd( rand() ,  _SQLCmdLoadInstanceResetTime , _SQLCmdLoadInstanceResetTime , TempData 
					, NULL );

}

void CNetDC_DCBaseChild::R_LogData( int LogType, void* LogData, unsigned long DataSize )
{
	if (AuroraAgentCli::This != NULL)
	{
		AuroraAgentCli::This->SAA_PG_AURORA_AGENT_DCtoAA_LOG_DATA(LogType, LogData, DataSize);
	}
}

bool CNetDC_DCBaseChild::_SQLCmdLoadInstanceResetTime( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempZoneInfoRequsetStruct* TempData = (TempZoneInfoRequsetStruct*)UserObj;

	//bool	Day[7];

	MyDbSqlExecClass		MyDBProc( sqlBase );
	char Buf[512];
	sprintf( Buf , "SELECT Sunday,Monday,Tuesday,Wednesday,Thursday,Firday,Saturday FROM InstanceResetTime WHERE ZoneID = %d" , TempData->ZoneID );
	MyDBProc.SqlCmd( Buf );

	for( int i = 0 ; i < 7 ; i++ )
		MyDBProc.Bind( 1+i, SQL_C_BIT, SQL_C_DEFAULT , (LPBYTE)&TempData->ResetDay.Day[i] );

	if( MyDBProc.Read() == false )
	{
		/*
		TempData->ResetDay.Sunday		= Day[0];
		TempData->ResetDay.Monday		= Day[1];
		TempData->ResetDay.Tuesday		= Day[2];
		TempData->ResetDay.Wednesday	= Day[3];
		TempData->ResetDay.Thursday		= Day[4];
		TempData->ResetDay.Firday		= Day[5];
		TempData->ResetDay.Saturday		= Day[6];
		*/
	}
	MyDBProc.Close();

	return true;
}
void CNetDC_DCBaseChild::_SQLCmdLoadInstanceResetTime( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{

	_RD_NormalDB->SqlCmd( rand() ,  _SQLCmdLoadZoneInfoRequestEvent , _SQLCmdLoadZoneInfoResultEvent , UserObj 
		, NULL );
}

bool CNetDC_DCBaseChild::_SQLCmdLoadZoneInfoRequestEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char Buf[512];

	TempZoneInfoRequsetStruct* TempData = (TempZoneInfoRequsetStruct*)UserObj;

	MyDbSqlExecClass		MyDBProc( sqlBase );

	bool IsSelectOk = true;
	sprintf( Buf , "SELECT ItemVersion , CAST( InstanceResetTime as float ) FROM ZoneInfo WHERE ZoneID = %d" , TempData->ZoneID );
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempData->LocalInfo.ItemVersion );
	MyDBProc.Bind( 2, SQL_C_FLOAT, SQL_C_DEFAULT , (LPBYTE)&TempData->LocalInfo.InstanceResetTime );
	if( MyDBProc.Read() == false )
	{
		TempData->LocalInfo.HouseItemMaxDBID = 0;
		TempData->LocalInfo.ItemVersion = 0;
		TempData->LocalInfo.InstanceResetTime = 0;
		IsSelectOk = false;
	}
	MyDBProc.Close();

	if( IsSelectOk == false )
	{
		sprintf( Buf , "INSERT ZoneInfo( ZoneID ) VALUES( %d )",TempData->ZoneID );
		MyDBProc.SqlCmd_WriteOneLine( Buf );
	}

	TempData->LocalInfo.ItemVersion++;

	//讀取副本共用變數
	InstancePlayStatusStruct InstTemp;
	sprintf( Buf , "SELECT PartyKey , KeyID , KeyValue  FROM InstancePlayState WHERE ZoneID = %d and LiveTime > getdate()" , TempData->ZoneID );
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&InstTemp.PartyKey );
	MyDBProc.Bind( 2, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&InstTemp.KeyID	 );
	MyDBProc.Bind( 3, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&InstTemp.KeyValue );
	while( MyDBProc.Read()  )
	{
		TempData->InstancePlayStatusList.push_back( InstTemp );
	}
	MyDBProc.Close();


	return true;
}
void CNetDC_DCBaseChild::_SQLCmdLoadZoneInfoResultEvent ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempZoneInfoRequsetStruct* TempData = (TempZoneInfoRequsetStruct*)UserObj;

	Net_InstanceChild::SL_ResetTime( TempData->ZoneID , TempData->ResetDay );

	vector< InstancePlayStatusStruct >& StatusList = TempData->InstancePlayStatusList;

	for( unsigned i = 0 ; i < StatusList.size() ; i++ )
	{
		Net_InstanceChild::SL_PlayInfo( TempData->ZoneID , StatusList[i].PartyKey , StatusList[i].KeyID , StatusList[i].KeyValue );
	}


	S_ZoneInfoResult( TempData->NetID , TempData->LocalInfo );

	delete TempData;
}
//////////////////////////////////////////////////////////////////////////
bool CNetDC_DCBaseChild::_SQLCmdLoadGlobalInfoRequestEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char	SqlCmd[512];
	_GlobalSettingList.clear();
	ZoneConfigBaseStruct	TempZoneConfig;

	MyDbSqlExecClass		MyDBProc( sqlBase );
	sprintf( SqlCmd , "SELECT  ZoneID , CmdStr , Value , Str FROM WorldConfigTable WHERE BeginTime <= getdate() and EndTime >= getdate()" );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT						, (LPBYTE)&TempZoneConfig.ZoneID );
	MyDBProc.Bind( 2, SQL_C_CHAR, sizeof( TempZoneConfig.CmdStr )	, (LPBYTE) TempZoneConfig.CmdStr.Begin() );
	MyDBProc.Bind( 3, SQL_C_LONG, SQL_C_DEFAULT						, (LPBYTE)&TempZoneConfig.Value );
	MyDBProc.Bind( 4, SQL_C_CHAR, sizeof( TempZoneConfig.Str )		, (LPBYTE) TempZoneConfig.Str.Begin() );
	while( MyDBProc.Read() )
	{
		_GlobalSettingList.push_back( TempZoneConfig );
	}	
	MyDBProc.Close();
	/*
	char	Buf[512];
	char	CmdStr[256];
	int		Value;

	MyDbSqlExecClass		MyDBProc( sqlBase );
	sprintf( Buf , "SELECT  CmdStr , Value FROM WorldConfigTable" );
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1, SQL_C_CHAR, sizeof( CmdStr ) , (LPBYTE)&CmdStr );
	MyDBProc.Bind( 2, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Value );
	while( MyDBProc.Read() )
	{
		if( stricmp( CmdStr , "ExpRate" ) == 0 )
		{
			_GlobalSetting.Info.ExpRate = Value / 100.0f;
			if( _GlobalSetting.Info.ExpRate < 1 )
				_GlobalSetting.Info.ExpRate = 1;
		}
		else if( stricmp( CmdStr , "DropTreasureRate" ) == 0 )
		{
			_GlobalSetting.Info.DropTreasureRate = Value/ 100.0f;
			if( _GlobalSetting.Info.DropTreasureRate < 1 )
				_GlobalSetting.Info.DropTreasureRate = 1;
		}
		else if( stricmp( CmdStr , "TpRate" ) == 0 )
		{
			_GlobalSetting.Info.TpRate = Value/ 100.0f;
			if( _GlobalSetting.Info.TpRate < 1 )
				_GlobalSetting.Info.TpRate = 1;
		}
		else if( stricmp( CmdStr , "MoneyRate" ) == 0 )
		{
			_GlobalSetting.Info.MoneyRate = Value/ 100.0f;
		}
		else if( stricmp( CmdStr , "PK_DotRate" ) == 0 )
		{
			_GlobalSetting.Info.PK_DotRate = Value/ 100.0f;
		}
		else if( stricmp( CmdStr , "PK_DamageRate" ) == 0 )
		{
			_GlobalSetting.Info.PK_DamageRate = Value/ 100.0f;
		}
		else if( stricmp( CmdStr , "LearnMagicID" ) == 0 )
		{
			_GlobalSetting.Info.LearnMagicID = Value;
		}
		else if( stricmp( CmdStr , "Version" ) == 0 )
		{
			_GlobalSetting.Info.Version = (SystemVersionENUM)Value;
		}
	}	
	MyDBProc.Close();

	//////////////////////////////////////////////////////////////////////////
	//載入 所有 GiveSysten 的資料
//sprintf( Buf , "SELECT MAX(DBID) AS Expr1 FROM NPCData WITH (TABLOCKX)" );
*/

	return true;
}
void CNetDC_DCBaseChild::_SQLCmdLoadGlobalInfoResultEvent ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
//	S_GlobalZoneInfoResult( _GlobalSetting.Info );
	S_GlobalZoneInfoList( _GlobalSettingList );
}
void CNetDC_DCBaseChild::RL_GmCommandResult			( int GUID , GmCommandResultENUM Result , char  ResultStr[512] )
{
	SaveGMCommandResult( GUID , Result , ResultStr );
}

void CNetDC_DCBaseChild::OnLogin( )
{
	//////////////////////////////////////////////////////////////////////////
	//開啟log
	//////////////////////////////////////////////////////////////////////////
	char	Buf[512];
	char	ExeFileName[MAX_PATH];
	//寫入Version資料
	GetModuleFileName( NULL, ExeFileName, MAX_PATH );

	CFileVersion Version;
	if (Version.Open(ExeFileName))
	{
		CString ver = Version.GetFixedFileVersion();
		const char* verStr = ver.GetString();
		sprintf( Buf , "INSERT INTO ServerStartLog( Version,Server,WorldID) VALUES ( '%s' , 'DATACENTER',%d )" , verStr ,Global::_Net->GetWorldID() );
		This->R_SqlCommand( rand() , EM_SqlCommandType_ImportDB , Buf );
	}
}

//////////////////////////////////////////////////////////////////////////
//開機存寫資料庫測試
bool CNetDC_DCBaseChild::_SQLCmdDBReadWriteTestEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
//	char* DBName = Arg.GetString( "DBName" );
	int   CheckNum = TimeStr::Now_Value();

	MyDbSqlExecClass		MyDBProc( sqlBase );
	char	Buf[512];
	
	sprintf( Buf , "DELETE ReadWriteTestTable WHERE WorldID=%d" , Global::GetWorldID() );
	MyDBProc.SqlCmd_WriteOneLine( Buf );
	sprintf( Buf , "Insert ReadWriteTestTable (WorldID,CheckNum) VALUES ( %d,%d)" , Global::GetWorldID() , CheckNum );
	MyDBProc.SqlCmd_WriteOneLine( Buf );

	sprintf( Buf , "SELECT CheckNum FROM ReadWriteTestTable WHERE WorldID = %d" , Global::GetWorldID() );
	MyDBProc.SqlCmd( Buf );
	int		SelectCheckNum = 0;

	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&SelectCheckNum );
	if( MyDBProc.Read() == false )
		return false;

	MyDBProc.Close();

	if( CheckNum != SelectCheckNum )
		return false;

	return true;
}
void CNetDC_DCBaseChild::_SQLCmdDBReadWriteTestResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	char* DBName = Arg.GetString( "DBName" );

	if( ResultOK != false )
	{
		Print( LV5 , "DB Read Write Test" , "DBName=%s OK" , DBName );
	}
	else
	{
		Print( LV5 , "DB Read Write Test" , "DBName=%s Failed" , DBName );
	}
}
//////////////////////////////////////////////////////////////////////////

//在import db讀取要處理的 db sql 命令
bool CNetDC_DCBaseChild::_SQLCmdLoadSqlCmdEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass		MyDBProc( sqlBase );
	char	Buf[512];

	ProcessCmdStruct TempProcCmd;
	int				DBType;

	sprintf( Buf , "SELECT GUID, SqlCmd, DBType FROM SqlCommandTable WHERE (WorldID = %d) AND (DestroyTime = 0)" , Global::GetWorldID() );
	MyDBProc.SqlCmd( Buf );

	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT				 , (LPBYTE)&TempProcCmd.GUID	);
	MyDBProc.Bind( 2, SQL_C_CHAR, sizeof(TempProcCmd.SqlCmd) , (LPBYTE)&TempProcCmd.SqlCmd	);
	MyDBProc.Bind( 3, SQL_C_LONG, SQL_C_DEFAULT				 , (LPBYTE)&DBType				);

	while( MyDBProc.Read() )
	{
		if( DBType < 0 || DBType >=4 )
			continue;
		_SqlCmdList[DBType].push_back( TempProcCmd );
	}
	MyDBProc.Close();

	
	for( unsigned i = 0 ; i < 4 ; i++ )
		for( unsigned j = 0 ; j < _SqlCmdList[i].size() ; j++ )
		{
			sprintf( Buf , "UPDATE SqlCommandTable SET DestroyTime = getdate() WHERE GUID =%d" , _SqlCmdList[i][j].GUID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
	return true;
}
void CNetDC_DCBaseChild::_SQLCmdLoadSqlCmdResultEvent ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	_IsProcDBSqlCmdOK = true;

	_RD_NormalDB->SqlCmd		( 0 ,  _SQLCmdProcSqlCmdEvent , _SQLCmdProcSqlCmdResultEvent , &_SqlCmdList[2] , NULL );
	_RD_NormalDB_Global->SqlCmd	( 0 ,  _SQLCmdProcSqlCmdEvent , _SQLCmdProcSqlCmdResultEvent , &_SqlCmdList[0] , NULL );
	_RD_NormalDB_Import->SqlCmd	( 0 ,  _SQLCmdProcSqlCmdEvent , _SQLCmdProcSqlCmdResultEvent , &_SqlCmdList[1] , NULL );
	_RD_NormalDB_Log->SqlCmd	( 0 ,  _SQLCmdProcSqlCmdEvent , _SQLCmdProcSqlCmdResultEvent , &_SqlCmdList[3] , NULL );
}

bool CNetDC_DCBaseChild::_SQLCmdProcSqlCmdEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass		MyDBProc( sqlBase );
	vector< ProcessCmdStruct >& SqlCmdList = *(vector< ProcessCmdStruct >*)UserObj;
	
	for( unsigned i = 0 ; i < SqlCmdList.size() ; i++ )
	{
		MyDBProc.SqlCmd_WriteOneLine( SqlCmdList[i].SqlCmd );
	}
	return true;
}
void CNetDC_DCBaseChild::_SQLCmdProcSqlCmdResultEvent ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	vector< ProcessCmdStruct >& SqlCmdList = *(vector< ProcessCmdStruct >*)UserObj;

	SqlCmdList.clear();
}
/*
void CNetDC_DCBaseChild::R_CheckRoleStruct( int Count , CheckKeyAddressStruct Info[1000] )
{
	ReaderClass<PlayerRoleData>* Reader = RoleDataEx::ReaderBase( );
	map<string,ReadInfoStruct>&  ReadInfo = *( Reader->ReadInfo() );
	map<string,ReadInfoStruct>::iterator Iter;

	if( Count != ReadInfo.size() )
	{
		PrintToController(true, "(DataCenter)R_CheckRoleStruct false Count != ReadInfo.size()  " );
		return;
	}

	int i = 0;
	for( Iter = ReadInfo.begin() ; Iter != ReadInfo.end() ; Iter++ , i++)
	{		
		if( stricmp( Info[i].Key , Iter->first.c_str() ) != 0 )
		{
			PrintToController(true, "(DataCenter)R_CheckRoleStruct false stricmp( Info[i].Key , Iter->first.c_str() ) != 0" );
			return;
		}
		if( Info[i].Address != Iter->second.Point )
		{
			PrintToController(true, "(DataCenter)R_CheckRoleStruct false Info[i].Address != Iter->second.Point" );
			return;
		}
	}
}
*/
//////////////////////////////////////////////////////////////////////////