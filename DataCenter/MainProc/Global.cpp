#include "Global.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
#include "../NetWalker_Member/NetWakerDataCenterInc.h"
#include "LuaPlot/LuaPlot.h"
//#include "RoleData/ObjectDataClass.h"
#include "GuildManage/GuildManage.h"
#include "../AuroraAgentCli/AuroraAgentCli.h"
//----------------------------------------------------------------------------------------------------
#define _Def_DataBase_ThreadCount_	300

IniFileClass	                                		Global::_Ini;
bool                                            		Global::_IsDelay = true;
bool									        		Global::_IsDestory = false;
int														Global::_SaveCount = 0;
int														Global::_ProcSaveCount = 0;
int														Global::_MaxSaveCount = 0;
int														Global::_BeginTime;
//int														Global::_DebugPacket	= 0;
//int														Global::_WorldID = 0;
DBTransferClass*                                		Global::_DBTransfer_Game   = NULL;		//db傳送資料的物件
DBTransferClass*                                		Global::_DBTransfer_Global = NULL;		//db傳送資料的物件
DBTransferClass*                                		Global::_DBTransfer_Import = NULL;		//db傳送資料的物件
DBTransferClass*                     					Global::_DBTransfer_Log = NULL;			//db傳送資料的物件
DBTransferClass*                     					Global::_DBTransfer_External = NULL;	//db傳送資料的物件
//DBStructTransferClass< RoleDataEx >*            		Global::_RoleDataDB  = NULL;    //db轉結構的物件
DBStructTransferClass< NPCData >*               		Global::_NPCDataDB   = NULL;    //db轉結構的物件
//DBStructTransferClass< DB_ItemFieldStruct >*    		Global::_RD_ItemDB = NULL;      //db轉結構的物件
//DBStructTransferClass< DB_AbilityStruct >*      		Global::_RD_AbilityDB = NULL;   //db轉結構的物件
//DBStructTransferClass< DB_BaseFriendStruct >*			Global::_RD_FriendDB = NULL;	//db轉結構的物件
DBStructTransferClass< DB_MailStruct >*					Global::_RD_MailDB = NULL;		//db轉結構的物件(直接使用SQL命令)
DBStructTransferClass< DB_ACItemStruct >*				Global::_RD_ACItemDB = NULL;	//db轉結構的物件(直接使用SQL命令)
DBStructTransferClass< DB_BillBoardRoleInfoStruct >*	Global::_RD_BillBoardDB = NULL;	//db轉結構的物件(直接使用SQL命令)

CountryTypeENUM			Global::_CountryType;

DBStructTransferClass< int >*						Global::_RD_NormalDB = NULL;	//db轉結構的物件(直接使用SQL命令)一般類型
DBStructTransferClass< int >*						Global::_RD_NormalDB_Global = NULL;	//db轉結構的物件(直接使用SQL命令)一般類型
DBStructTransferClass< int >*						Global::_RD_NormalDB_Import = NULL;
DBStructTransferClass< int >*						Global::_RD_NormalDB_Log = NULL;
DBStructTransferClass< int >*						Global::_RD_NormalDB_External = NULL;

//CreateDBCmdClass<RoleDataEx>*                 	  Global::_RoleDataSql = NULL;
//CreateDBCmdClass<DB_ItemFieldStruct>*         	  Global::_RDItemSql = NULL;
//CreateDBCmdClass<DB_AbilityStruct>*           	  Global::_RDAbilitySql = NULL;
//CreateDBCmdClass<DB_BaseFriendStruct>*				Global::_RDFriendSql = NULL;
CreateDBCmdClass<DB_MailItemStruct>*				Global::_RDMailItemSql = NULL;
CreateDBCmdClass<DB_MailStruct>*					Global::_RDMailBaseSql = NULL;
CreateDBCmdClass<DB_ACItemStruct>*					Global::_RDACBaseSql = NULL;
CreateDBCmdClass<DB_ACItemHistoryStruct>*			Global::_RDACHistorySql = NULL;
CreateDBCmdClass<DB_BillBoardRoleInfoStruct>*		Global::_RDBillBoardSql = NULL;
CreateDBCmdClass<HouseBaseDBStruct>*		   		Global::_RDHouseBaseSql = NULL;
CreateDBCmdClass<HouseItemDBStruct>*		   		Global::_RDHouseItemSql = NULL;

CreateDBCmdClass<DB_NPC_MoveBaseStruct>*			Global::_RDNPCMoveSql = NULL;

CreateDBCmdClass<GuildBaseStruct>*		   			Global::_RDGuildBaseSql = NULL;
CreateDBCmdClass<GuildMemberStruct>*		   		Global::_RDGuildMemberSql = NULL;
CreateDBCmdClass<GuildBoardStruct>*		   			Global::_RDGuildBoardSql = NULL;

CreateDBCmdClass<AccountBagItemFieldDBStruct>*		Global::_RDAccountBagSql = NULL;
CreateDBCmdClass<WebShopBagItemFieldDBStruct>*		Global::_RDWebShopBagSql = NULL;
CreateDBCmdClass<OfflineMessageDBStruct>*			Global::_RDOfflineMessageSql = NULL;

CreateDBCmdClass<GuildHousesInfoStruct>*			Global::_RDGuildHouseBaseSql = NULL;
CreateDBCmdClass<GuildHouseItemStruct>*		   		Global::_RDGuildHouseItemSql = NULL;
CreateDBCmdClass<GuildHouseBuildingInfoStruct>*		Global::_RDGuildHouseBuildingSql = NULL;
CreateDBCmdClass<GuildHouseFurnitureItemStruct>*	Global::_RDGuildHouseFurnitureSql = NULL;
CreateDBCmdClass<GuildHouseWarInfoStruct>*			Global::_RDGuildHouseWarInfoSql = NULL;
CreateDBCmdClass<GuildHouseWarHistoryStruct>*		Global::_RDGuildHouseWarHistorySql = NULL;

RecycleBin< DBRoleTempDataInfo >*               	Global::_ReadRole;
int													Global::_WorldID = -1;
int													Global::_TimeZone = 0;

//交易稅
float												Global::_AC_Tax_Money = 0;
float												Global::_AC_Tax_AccountMoney = 0;
bool												Global::_DisableLottery = false;
bool												Global::_EnableWorldConfigTable = false;
bool												Global::_IgnoreObjNameStringDB = false;
//--------------------------------------------------------------------------------------
bool     Global::Init( char* IniFile )
{
	//--------------------------------------------------------------------------------------
	_Ini.SetIniFile( "Global.ini" );
    if( _Ini.SetIniFile( IniFile ) == false )
    {
        MessageBox( NULL , "GSrv Ini File Read Error!!" , "Error" ,MB_OK );
        return false;
    }
	g_pPrint->SetPrintLV( _Ini.Int( "DebugPrintLv" ) );
	g_pPrint->SetFileOutputLV( _Ini.Int( "DebugOutputLv" ) );
	g_pPrint->SetOutputFileName( _Ini.Str( "DebugOutputFile" ).c_str() );
	g_pPrint->SetOutputFieldName( _Ini.Str( "DebugOutputField" ).c_str() );

	SqlBaseClass::Init( _Ini.Str( "DebugOutputField" ).c_str() , _Ini.Str( "DebugOutputFile" ).c_str() );

	Print( LV5 , "##開啟Server##" , "時間 %s" , TimeStr::Now_Time()  );
	//--------------------------------------------------------------------------------------
	GlobalBase::_Init();
	_ReadRole = NEW( RecycleBin< DBRoleTempDataInfo > );
//	_RoleBin = NEW( RecycleBin< RoleDataEx > );
	//--------------------------------------------------------------------------------------
//	_Schedular = FunctionSchedularClass::New( );
//	Print.Init( "Error.log" );
    //--------------------------------------------------------------------------------------
    //劇情資料初始化
    LuaPlotClass::Init( );
//    BaseItemObject::Init( );
	LUA_VMClass::Init();
	GuildManageClass::Init();

    //--------------------------------------------------------------------------------------
    string  GameDBerver			= _Ini.Str( "Game_DBServer" );
    string  GameDataBase 	    = _Ini.Str( "Game_DataBase" );
    string  GameAccount			= _Ini.Str( "Game_Account" );
    string  GamePassword		= _Ini.Str( "Game_Password" );

	string  GlobalDBerver		= _Ini.Str( "Global_DBServer" );
    string  GlobalDataBase		= _Ini.Str( "Global_DataBase" );
    string  GlobalAccount		= _Ini.Str( "Global_Account" );
    string  GlobalPassword		= _Ini.Str( "Global_Password" );

	string	ImportDBServer =  _Ini.Str( "Import_DBServer" );
	string	ImportDataBase =  _Ini.Str( "Import_DataBase" );
	string	ImportAccount  =  _Ini.Str( "Import_Account" );
	string	ImportPassword =  _Ini.Str( "Import_Password" );
	
	string	LogDBServer =  _Ini.Str( "Log_DBServer" );
	string	LogDataBase =  _Ini.Str( "Log_DataBase" );
	string	LogAccount  =  _Ini.Str( "Log_Account" );
	string	LogPassword =  _Ini.Str( "Log_Password" );

	_TimeZone =  _Ini.Int( "TimeZone" );
	RoleDataEx::G_TimeZone = _TimeZone;
	
	int DBThreadCount_Game =  _Ini.Int( "DBThreadCount_Game" );
	if( DBThreadCount_Game <= 0 )
		DBThreadCount_Game = _Def_DataBase_ThreadCount_;

	int DBThreadCount_Log =  _Ini.Int( "DBThreadCount_Log" );
	if( DBThreadCount_Log <= 0 )
		DBThreadCount_Log = 100;

	string GamePwd		= DecodePassword( GamePassword.c_str() , GameAccount.c_str() );
	string GlobalPwd	= DecodePassword( GlobalPassword.c_str() , GlobalAccount.c_str() );
	string ImportPwd	= DecodePassword( ImportPassword.c_str() , ImportAccount.c_str() );
	string LogPwd		= DecodePassword( LogPassword.c_str() , LogAccount.c_str() );	

    _DBTransfer_Game     = NEW DBTransferClass( (char*)GameDBerver.c_str() , (char*)GameDataBase.c_str() , (char*)GameAccount.c_str() , GamePwd.c_str() , DBThreadCount_Game  );
	_DBTransfer_Global   = NEW DBTransferClass( (char*)GlobalDBerver.c_str() , (char*)GlobalDataBase.c_str() , (char*)GlobalAccount.c_str() , GlobalPwd.c_str() , 2 );
	_DBTransfer_Import   = NEW DBTransferClass( (char*)ImportDBServer.c_str() , (char*)ImportDataBase.c_str() , (char*)ImportAccount.c_str() , ImportPwd.c_str() , 10 );
	_DBTransfer_Log	     = NEW DBTransferClass( (char*)LogDBServer.c_str() , (char*)LogDataBase.c_str() , (char*)LogAccount.c_str() , LogPwd.c_str() , DBThreadCount_Log );	


    _NPCDataDB      = NEW DBStructTransferClass< NPCData > ( _DBTransfer_Global , RoleDataEx::ReaderNPC()  , "NPCData" );
	_RD_MailDB		= NEW DBStructTransferClass< DB_MailStruct > ( _DBTransfer_Game , NULL  , "" ) ;				//db轉結構的物件
	_RD_ACItemDB	= NEW DBStructTransferClass< DB_ACItemStruct > ( _DBTransfer_Game , NULL  , "" ) ;				//db轉結構的物件
	_RD_BillBoardDB	= NEW DBStructTransferClass< DB_BillBoardRoleInfoStruct > ( _DBTransfer_Game , NULL  , "" ) ;				//db轉結構的物件

	_RD_NormalDB		  = NEW DBStructTransferClass< int > ( _DBTransfer_Game		, NULL  , "" );
	_RD_NormalDB_Global	  = NEW DBStructTransferClass< int > ( _DBTransfer_Global	, NULL  , "" );
	_RD_NormalDB_Import	  = NEW DBStructTransferClass< int > ( _DBTransfer_Import	, NULL  , "" );
	_RD_NormalDB_Log	  = NEW DBStructTransferClass< int > ( _DBTransfer_Log		, NULL  , "" );

	_RDMailItemSql  = NEW CreateDBCmdClass<DB_MailItemStruct > ( RoleDataEx::ReaderRD_MailItem()  , "Mail_Item" );
	_RDMailBaseSql  = NEW CreateDBCmdClass<DB_MailStruct > ( RoleDataEx::ReaderRD_MailBase()  , "Mail_Base" ) ;

	_RDACBaseSql    = NEW CreateDBCmdClass<DB_ACItemStruct > ( RoleDataEx::ReaderRD_ACBase()  , "AC_Base" ) ;
	_RDACHistorySql = NEW CreateDBCmdClass<DB_ACItemHistoryStruct > ( RoleDataEx::ReaderRD_ACHistory()  , "AC_History" ) ;

	_RDBillBoardSql = NEW CreateDBCmdClass<DB_BillBoardRoleInfoStruct > ( RoleDataEx::ReaderRD_BillBoard()  , "BillBoardRoleInfo" ) ;

	_RDNPCMoveSql   =  NEW CreateDBCmdClass<DB_NPC_MoveBaseStruct > ( RoleDataEx::ReaderRD_NPC_MoveBase()  , "NPC_MoveFlagDB" );

	_RDGuildBaseSql		= NEW CreateDBCmdClass<GuildBaseStruct > ( RoleDataEx::ReaderRD_GuildBase()  , "Guild_Base" );
	_RDGuildMemberSql	= NEW CreateDBCmdClass<GuildMemberStruct > ( RoleDataEx::ReaderRD_GuildMember()  , "RoleData" );

	_RDGuildBoardSql	= NEW CreateDBCmdClass<GuildBoardStruct > ( RoleDataEx::ReaderRD_GuildBoard()  , "Guild_Board" );

	_RDHouseBaseSql		= NEW CreateDBCmdClass<HouseBaseDBStruct> ( RoleDataEx::ReaderRD_HouseBase()  , "Houses_Base" );
	_RDHouseItemSql		= NEW CreateDBCmdClass<HouseItemDBStruct> ( RoleDataEx::ReaderRD_HouseItem()  , "Houses_Item" );

	_RDAccountBagSql	= NEW CreateDBCmdClass<AccountBagItemFieldDBStruct> ( RoleDataEx::ReaderRD_AccountBag()  , "AccountBag" );
	_RDWebShopBagSql	= NEW CreateDBCmdClass<WebShopBagItemFieldDBStruct> ( RoleDataEx::ReaderRD_WebShopBag()  , "WebShopBag" );
	_RDOfflineMessageSql= NEW CreateDBCmdClass<OfflineMessageDBStruct> ( RoleDataEx::ReaderRD_OfflineMessage()  , "OfflineMessage" );
	//公會屋戰資料
	_RDGuildHouseWarInfoSql= NEW CreateDBCmdClass<GuildHouseWarInfoStruct> ( RoleDataEx::ReaderRD_GuildHouseWarInfo()  , "GuildHouseWar_Order" );
	_RDGuildHouseWarHistorySql= NEW CreateDBCmdClass<GuildHouseWarHistoryStruct> ( RoleDataEx::ReaderRD_GuildHouseWarHistory()  , "GuildHouseWar_History" );

	_RDGuildHouseBaseSql		= NEW CreateDBCmdClass<GuildHousesInfoStruct> ( RoleDataEx::ReaderRD_GuildHouseInfo()  , "GuildHouse_Base" );
	_RDGuildHouseItemSql		= NEW CreateDBCmdClass<GuildHouseItemStruct> ( RoleDataEx::ReaderRD_GuildHouseItem()  , "GuildHouse_Item" );
	_RDGuildHouseBuildingSql	= NEW CreateDBCmdClass<GuildHouseBuildingInfoStruct> ( RoleDataEx::ReaderRD_GuildHouseBuildingInfo()  , "GuildHouse_Building" );
	_RDGuildHouseFurnitureSql	= NEW CreateDBCmdClass<GuildHouseFurnitureItemStruct> ( RoleDataEx::ReaderRD_GuildHouseFurnitureItem()  , "GuildHouse_FurnitureItem" ) ;
    //--------------------------------------------------------------------------------------
	_WorldID		= _Ini.Int( "WorldID" );
	//_DebugPacket	= _Ini.Int( "DebugPacket" );
	//--------------------------------------------------------------------------------------
	string ExternalDBServer =  _Ini.Str( "External_DBServer");
	string ExternalDataBase =  _Ini.Str( "External_DataBase");
	string ExternalAccount  =  _Ini.Str( "External_Account"	);
	string ExternalPassword =  _Ini.Str( "External_Password");
	string ExternalPwd	= DecodePassword( ExternalPassword.c_str() , ExternalAccount.c_str() );

	if (!ExternalDBServer.empty())
	{
		_DBTransfer_External = NEW DBTransferClass( (char*)ExternalDBServer.c_str() , (char*)ExternalDataBase.c_str() , (char*)ExternalAccount.c_str() , ExternalPwd.c_str() , 10 );
		_RD_NormalDB_External = NEW DBStructTransferClass< int > ( _DBTransfer_External , NULL  , "" );
	}

	//--------------------------------------------------------------------------------------
	//讀取物件資料
	string	DataPath  = _Ini.Str( "DataPath" ); 
	string      Datalanguage;
	string		Stringlanguage;

	// 讀取 Server Data 與 String 的設定
	if( _Ini.Str( "Datalanguage", Datalanguage ) == false )
	{
		Datalanguage = "tw";
	}

	Datalanguage = strlwr( (char*)Datalanguage.c_str() );

	if( _Ini.Str( "Stringlanguage", Stringlanguage ) == false )
	{
		Stringlanguage = "tw";
	}

	if( _Ini.Float( "AC_Tax_Money", _AC_Tax_Money ) == false )
	{
		_AC_Tax_Money = 0.94f;
	}
	if( _Ini.Float( "AC_Tax_AccountMoney", _AC_Tax_AccountMoney ) == false )
	{
		_AC_Tax_AccountMoney = 1;
	}

	int DisableLottery = _Ini.Int( "DisableLottery" );

	if( DisableLottery == 0 )
		_DisableLottery = false;
	else 
		_DisableLottery = true;


	if( _Ini.Int( "EnableWorldConfigTable" ) == 0 )
		_EnableWorldConfigTable = false;
	else 
		_EnableWorldConfigTable = true;

	if( _Ini.Int( "IgnoreObjNameStringDB" ) == 0 )
		_IgnoreObjNameStringDB = false;
	else 
		_IgnoreObjNameStringDB = true;

	//aurora agent server
	if (!_Ini.Str( "AuroraAgentIP").empty())
	{
		AuroraAgentCli::Init();
	}


	Stringlanguage = strlwr( (char*)Stringlanguage.c_str() );


	ObjectDataClass::InitRuneEngine( DataPath.c_str() );
	ObjectDataClass::Init( DataPath.c_str() , Datalanguage.c_str(), Stringlanguage.c_str() );
	//--------------------------------------------------------------------------------------
//	string LanguageTypeStr = _Ini.Str( "LanguageType" );
	_CountryType = (CountryTypeENUM)g_ObjectData->QueryCountryID( Datalanguage.c_str() );
	//--------------------------------------------------------------------------------------
	ObjectDataClass::LoadSpecialCharacterName( _Ini.Str("SpecialCharacterNameFile").c_str() );
	//--------------------------------------------------------------------------------------
	//網路封包資料初始化

	Net_ServerList->Init( NULL , EM_SERVER_TYPE_DATACENTER , DF_NO_LOCALID );    

	CNetDC_DCBaseChild::Init();
	CNetDC_MailChild::Init();
	CNetDC_ACChild::Init();
	CNetDC_GuildChild::Init();
	NetSrv_OnlinePlayerChild::Init();
	CNetDC_BillboardChild::Init();
	CNetDC_LotteryChild::Init();
	CNetDC_HousesChild::Init();
	NetSrv_GMToolsChild::Init();
	CNetDC_DepartmentStoreChild::Init();
	Net_TalkChild::Init();
	Net_ImportBoardChild::Init();
	Net_ItemChild::Init();
	CNetDC_OtherChild::Init();
	CNetDC_GuildHousesChild::Init();
	CNetDC_FestivalChild::Init();	
	Net_InstanceChild::Init();
	CNetDC_BattleGround_Child::Init();
	CNetDC_BG_GuildWarChild::Init();
	Net_ZoneStatusChild::Init();
	CNetDC_LoginChild::Init();

	_Net->RegGSrvNetID( EM_SERVER_TYPE_DATACENTER );
	_Net->ConnectType( EM_GSrvType_Active );
	//--------------------------------------------------------------------------------------
    string  IP   = _Ini.Str( "SwitchIP" );
    int     Port = _Ini.Int( "Switch_GSrvPort" );

    _Net->Connect( (char*)IP.c_str() , Port , "DataCenter" );

	//--------------------------------------------------------------------------------------
	Schedular()->Push( _OnTimeProc_10Sec_ , 10*1000 , NULL , NULL );


    return true;
}

//--------------------------------------------------------------------------------------
bool     Global::Release( )
{
	AuroraAgentCli::Release();
	ObjectDataClass::Release();

	GlobalBase::_Release();
	LUA_VMClass::Release();
	GuildManageClass::Release();

	CNetDC_DCBaseChild::Release();
	CNetDC_MailChild::Release();
	CNetDC_ACChild::Release();
	CNetDC_GuildChild::Release();
	NetSrv_OnlinePlayerChild::Release();
	CNetDC_BillboardChild::Release();
	CNetDC_LotteryChild::Release();
	CNetDC_HousesChild::Release();
	NetSrv_GMToolsChild::Release();
	CNetDC_DepartmentStoreChild::Release();
	Net_TalkChild::Release();
	Net_ImportBoardChild::Release();
	CNetDC_OtherChild::Release();
//	CNetDC_HousesChild::Release();
	CNetDC_GuildHousesChild::Release();
//	CNetDC_GuildHousesWarChild::Release();
	Net_InstanceChild::Release();
	CNetDC_BattleGround_Child::Release();
	CNetDC_BG_GuildWarChild::Release();
	Net_ZoneStatusChild::Release();
	CNetDC_LoginChild::Release();
//	BaseItemObject::Release( );
	
//	_Schedular->Del();	

    delete _DBTransfer_Game;
	delete _DBTransfer_Global;
	delete _DBTransfer_Import;
	delete _DBTransfer_Log;

	if (_DBTransfer_External != NULL)
	{
		delete _DBTransfer_External;
	}

	delete _NPCDataDB;
	delete _ReadRole;

	delete _RD_MailDB;		
	delete _RD_ACItemDB;	
	delete _RD_BillBoardDB;

	delete _RD_NormalDB;
	delete _RD_NormalDB_Global;
	delete _RD_NormalDB_Import;
	delete _RD_NormalDB_Log;

	if (_RD_NormalDB_External != NULL)
	{
		delete _RD_NormalDB_External;
	}

	delete _RDMailItemSql;
	delete _RDMailBaseSql;
	delete _RDACBaseSql;
	delete _RDNPCMoveSql;
	delete _RDBillBoardSql;

	delete _RDGuildBaseSql;
	delete _RDGuildMemberSql;

	delete _RDHouseBaseSql;
	delete _RDAccountBagSql;
	delete _RDWebShopBagSql;
	delete _RDOfflineMessageSql;
	delete _RDGuildHouseWarInfoSql;
	delete _RDGuildHouseWarHistorySql;

	delete _RDGuildHouseBaseSql;
	delete _RDGuildHouseItemSql;
	delete _RDGuildHouseBuildingSql;
	delete _RDGuildHouseFurnitureSql;

    return true;
}

bool     Global::Process( )
{
	RoleDataEx::G_NowStr = TimeStr::Now();	
	RoleDataEx::G_Now = TimeStr::Now_Value() + RoleDataEx::SysDTime;
	RoleDataEx::G_Now_Float = TimeExchangeIntToFloat( RoleDataEx::G_Now + RoleDataEx::G_TimeZone * 60*60 );


    static int		ProcTime = timeGetTime();
    static int		LastProcDelayTime = 0;
    static int		LastProcDelayRecorder = 0;

    LastProcDelayTime = timeGetTime() -  ProcTime;
	_Net->Process( );
    if( LastProcDelayTime > 100 )
    {
		RoleDataEx::OnTimeProcAll( );

        ProcTime += 100;
        {

            LUA_VMClass::Process( );     

            _NPCDataDB->Process( );
			_RD_MailDB->Process( );
			_RD_ACItemDB->Process( );
			_RD_BillBoardDB->Process( );

			_RD_NormalDB->Process( );
			_RD_NormalDB_Global->Process();
			_RD_NormalDB_Import->Process();
			_RD_NormalDB_Log->Process();

			if (_RD_NormalDB_External != NULL)
			{
				_RD_NormalDB_External->Process();
			}

			SqlBaseClass::OnTimeProc( );

        }
    }

    if( LastProcDelayTime > 1000)
    {
        if( LastProcDelayRecorder < LastProcDelayTime )
        {
            LastProcDelayRecorder = LastProcDelayTime;
            Print( Def_PrintLV3, "Process" , "Program Delay %d " , LastProcDelayTime );
        }
        Global::_IsDelay = true;
    }
    else
    {
        LastProcDelayRecorder = 0;
        Global::_IsDelay = false;
    }

	//檢查是否所有的sql thread 已經在wait 狀態 所有要求都已經處理完成
	if( _IsDestory != false )
	{
		if(		_DBTransfer_Game->TotalRequestCount() == 0 
			&&	_DBTransfer_Global->TotalRequestCount() == 0 
			&&	_DBTransfer_Import->TotalRequestCount() == 0 
			&&	_DBTransfer_Log->TotalRequestCount() == 0 )
		{
			if (_DBTransfer_External != NULL)
			{
				if (_DBTransfer_External->TotalRequestCount() == 0)
				{
					return false;
				}
			}
			else
			{
				return false;
			}			
		}
	}

    return true;
}

/*
bool   Global::_SaveRoleSQLCmdRequestEvent ( vector<RoleDataEx>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	
	//MutilThread_CritSect	Thread_CritSect;
	int			TimeBegin = timeGetTime();
    char        Buf[512];
    int         i;
    bool        Result = true;
	MyDbSqlExecClass	MyDBProc( sqlBase );

    BaseItemObject* Obj = (BaseItemObject*)UserObj;

	if( Obj == NULL )
		return false;
    RoleDataEx*     Role = Obj->Role();
    RoleDataEx*     RoleDB = Obj->RoleDB();
    //儲存角色資料
    

	g_CritSect()->Enter();
	{
		DB_ItemFieldStruct  TempItemField;

		//printf( "\n----------------儲存SQL(%s)--------------" , Utf8ToChar( Role->RoleName() ).c_str() );

		sprintf( Buf , "Update RoleData_Account Set AccountMoney = %d , HouseDBID= %d where Account_ID = '%s'" 
					, Role->BaseData.Body.Money_Account 
					, Role->BaseData.HouseDBID
					, Role->Base.Account_ID.Begin() );
		MyDBProc.SqlCmd_WriteOneLine( Buf );


		sprintf( Buf , "where DBID = %d" , Role->Base.DBID );
		MyDBProc.SqlCmd_WriteOneLine( _RoleDataSql->GetUpDateStr( Role , Buf ) );

		//資料比對看是否有必要儲存

		for( i = 0 ; i < Role->BaseData.Body.Count ; i++ )
		{
			if( memcmp( &Role->BaseData.Body.Item[i] , &RoleDB->BaseData.Body.Item[i] , sizeof(Role->BaseData.Body.Item[i]) ) == 0 ) 
				continue;

			TempItemField.Info = Role->BaseData.Body.Item[i];		
			sprintf( Buf , "where FieldType = 1 and  FieldID = %d and DBID = %d" ,  i , Role->Base.DBID );

			MyDBProc.SqlCmd_WriteOneLine( _RDItemSql->GetUpDateStr( &TempItemField , Buf ) );
	        
		}

		for( i = 0 ; i < Role->BaseData.Bank.Count ; i++ )
		{
			if( memcmp( &Role->BaseData.Bank.Item[i] , &RoleDB->BaseData.Bank.Item[i] , sizeof(Role->BaseData.Bank.Item[i]) )== 0 ) 
				continue;

			TempItemField.Info = Role->BaseData.Bank.Item[i];
			sprintf( Buf , "where FieldType = 2 and  FieldID = %d and DBID = %d" ,  i , Role->Base.DBID );

			MyDBProc.SqlCmd_WriteOneLine( _RDItemSql->GetUpDateStr( &TempItemField , Buf ) );
		}

		for( i = 0 ; i < EM_EQWearPos_MaxCount ; i++ )
		{
			if( memcmp( &Role->BaseData.EQ.Item[i] , &RoleDB->BaseData.EQ.Item[i] , sizeof(Role->BaseData.EQ.Item[i]) )== 0 ) 
				continue;


			TempItemField.Info = Role->BaseData.EQ.Item[i];
			sprintf( Buf , "where FieldType = 3 and  FieldID = %d and DBID = %d" ,  i , Role->Base.DBID );

			MyDBProc.SqlCmd_WriteOneLine(  _RDItemSql->GetUpDateStr( &TempItemField , Buf ) );
		}

		DB_AbilityStruct  TempAbility;
		for( i = 0 ; i < EM_Max_Vocation ; i++ )
		{
			if( memcmp( &Role->BaseData.AbilityList[i] , &RoleDB->BaseData.AbilityList[i] , sizeof(Role->BaseData.AbilityList[i]) )== 0 ) 
				continue;

			TempAbility.Info = Role->BaseData.AbilityList[i];
			sprintf( Buf , "where Job = %d and DBID = %d" ,  i , Role->Base.DBID );

			MyDBProc.SqlCmd_WriteOneLine(  _RDAbilitySql->GetUpDateStr( &TempAbility , Buf ) );
		}

		//好有名單
		DB_BaseFriendStruct  TempFriend;
		FriendListStruct&	FriendList = Role->SelfData.FriendList;
		FriendListStruct&	FriendListDB = RoleDB->SelfData.FriendList;

		for( int i = 0 ; i < _MAX_FRIENDLIST_COUNT_ ; i++ )
		{
			if( memcmp( &(FriendList.FriendList[i]) , &(FriendListDB.FriendList[i]) , sizeof(FriendList.FriendList[i]) )== 0 ) 
				continue;

			if( FriendListDB.FriendList[i].IsSave == false )
			{
				TempFriend.Info = FriendList.FriendList[i];
				TempFriend.FieldID = i;
				TempFriend.DBID = Role->DBID();

				MyDBProc.SqlCmd_WriteOneLine(  _RDFriendSql->GetInsertStr( &TempFriend ) );
				
			}
			else
			{
				TempFriend.Info = FriendList.FriendList[i];

				sprintf( Buf , "where FieldID = %d and DBID = %d" ,  i , Role->Base.DBID );
				MyDBProc.SqlCmd_WriteOneLine( _RDFriendSql->GetUpDateStr( &TempFriend , Buf ) );
			}

			FriendList.FriendList[i].IsSave = true;
		}

		memcpy( RoleDB , Role , sizeof( *Role) );
		//printf( "\n----------------儲存SQL_End(%s)--------------" ,  Utf8ToChar( Role->RoleName() ).c_str() );
	}
	g_CritSect()->Leave();


	printf( "\n(**) Save角色(%s)　Time = %d", Utf8ToChar( Role->RoleName() ).c_str() ,  timeGetTime() - TimeBegin );
    return true;
}
void   Global::_SaveRoleSQLCmdReusltEventCB ( vector<RoleDataEx>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
    BaseItemObject* Obj = (BaseItemObject*)UserObj;

	if( Obj == NULL )
		return;
	
    Obj->IsNeedSave( false );
    Obj->IsSave( false );
	Obj->SaveTime( 12 * 30 );
    Obj->PopFromThread();
}
*/

/*
void		Global::SavePlayerDB( BaseItemObject* Obj )
{
    if( Obj->IsSave() != false )
    {
        return;
    }
    Obj->IsSave( true );


    Obj->PushToThread();
    _RoleDataDB->SqlCmd( Obj->RoleDB()->Base.DBID , _SaveRoleSQLCmdRequestEvent , _SaveRoleSQLCmdReusltEventCB , Obj , NULL  );


}
*/

bool Global::SendToCli_ByRoleName( const char* RoleName , int Size , void* Data )
{

	OnlinePlayerInfoStruct* Info = AllOnlinePlayerInfoClass::This()->GetInfo_ByName( (char*) RoleName );
	if( Info == NULL )
		return false;

	return _Net->SendToCli( Info->SockID , Info->ProxyID , Size , Data );
}

//////////////////////////////////////////////////////////////////////////
//測試DB儲存速度
void	Global::SaveTest( int Count )
{

	_SaveCount = 0;
	_ProcSaveCount = Count;
	_MaxSaveCount = Count;
	_BeginTime = timeGetTime();
	
	for( int i = 0 ; i < _Def_DataBase_ThreadCount_ ; i++ )
	{
		_ProcSaveCount--;
		_RD_NormalDB->SqlCmd( i , _TestDBSaveEventEventCB , _TestDBSaveEventResultCB , NULL , "ThreadID" , EM_ValueType_Int , i , NULL );
	}
}

bool   Global::_TestDBSaveEventEventCB  ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	//HSTMT dbproc = sqlBase->Gethstmt();
	MyDbSqlExecClass	MyDBProc( sqlBase );	

	MyDBProc.SqlCmd_WriteOneLine( "Insert Log_TestDB(FromAccountName,FromCharName,ToAccountName,ToCharName,ActionType,ZoneName,ZoneID,PosX,PosZ,ItemName,ItemID,ItemSerial,ItemCount,IsGiveItem,Inherent,RuneVolume) Values( 'b' ,'ABCD' ,'a' ,'測試' ,3,'測試',1,-3830,-8270,'測試',210001,10000056,1,1,0,0)" );
	return true;
}

void   Global::_TestDBSaveEventResultCB( vector<int>& ,void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	int ThreadID = Arg.GetNumber( "ThreadID" );

	_SaveCount++;
	float	Time = float( timeGetTime() - _BeginTime ) / 1000;
	if( _SaveCount >= _MaxSaveCount )
	{
		//printf( "\n測試儲存Log 總共存 %d 筆,使用 %f 秒" , _SaveCount , Time  );
		Print(LV2, "Global", "\n測試儲存Log 總共存 %d 筆,使用 %f 秒" , _SaveCount , Time  );
	}
	else
	{
		if( _SaveCount % 1000 == 0 )
		{
			//printf( "\n測試儲存Log 總共存 %d 筆,使用 %f 秒" , _SaveCount , Time );
			Print(LV2, "Global", "\n測試儲存Log 總共存 %d 筆,使用 %f 秒" , _SaveCount , Time );
		}
		
		if( _ProcSaveCount-- > 0 )
			_RD_NormalDB->SqlCmd( ThreadID , _TestDBSaveEventEventCB , _TestDBSaveEventResultCB , NULL , "ThreadID" , EM_ValueType_Int , ThreadID , NULL );
	}

}
//////////////////////////////////////////////////////////////////////////
void		Global::Log_ServerStatus( )
{
	
	char Buf[2048];
	_DBTransfer_Game->ReCalculate();
	_DBTransfer_Log->ReCalculate();

	sprintf( Buf , "Insert DataCenterServerStatus( Log_MaxWriteCmdCount,Log_MinWriteCmdCount,Log_TotalWriteCmdCount,Log_TotalReadCmdCount,Log_ThreadCount ) VALUES(%d,%d,%d ,%d,%d)"
		, _DBTransfer_Log->MaxWriteCmdCount()
		, _DBTransfer_Log->MinWriteCmdCount()
		, _DBTransfer_Log->TotalWriteCmdCount()
		, _DBTransfer_Log->TotalReadCmdCount()
		, _DBTransfer_Log->ThreadCount()   );

	//CNetDC_DCBaseChild::This->R_LogSqlCommand( Buf );
	CNetDC_DCBaseChild::This->R_SqlCommand( rand() , EM_SqlCommandType_WorldLogDB ,  Buf );
	
}


void	Global::Log_GuildLog( int GuildID , GuildActionTypeENUM Type , int LeaderDBID , int PlayerDBID  )
{
	char Buf[2048];

	sprintf( Buf , "INSERT GuildLog(GuildID,Type,LeaderDBID,PlayerDBID) VALUES(%d,%d,%d,%d)"
				, GuildID , Type , LeaderDBID , PlayerDBID );

	CNetDC_DCBaseChild::This->R_SqlCommand( rand() , EM_SqlCommandType_WorldLogDB , Buf );
}


void	Global::Log_Money( int PlayerDBID , ActionTypeENUM Type , int Money )
{
	if( Money == 0 )
		return;

	char Buf[2048];
	sprintf( Buf , "Insert MoneyLog(TargetNPCDBID,TargetPlayerDBID,PlayerDBID,ActionType,ZoneID,PosX,PosZ,NewMoney,OldMoney ) VALUES( 0,0,%d,%d,0,0,0,%d,0)"
		, PlayerDBID
		, Type
		, Money );

		CNetDC_DCBaseChild::This->R_SqlCommand( rand() , EM_SqlCommandType_WorldLogDB , Buf );
}

void	Global::Log_Money_Account( int PlayerDBID , ActionTypeENUM Type , int Money )
{
	if( Money == 0 )
		return;

	char Buf[2048];
	sprintf( Buf , "Insert AccountMoneyLog(TargetNPCDBID,TargetPlayerDBID,PlayerDBID,ActionType,ZoneID,PosX,PosZ,NewAccountMoney,OldAccountMoney ) VALUES( 0,0,%d,%d,0,0,0,%d,0)"
		, PlayerDBID
		, Type
		, Money );

	CNetDC_DCBaseChild::This->R_SqlCommand( rand() , EM_SqlCommandType_WorldLogDB , Buf );
}

void	Global::Log_Money_Bonus( int PlayerDBID , ActionTypeENUM Type , int Money )
{
	if( Money == 0 )
		return;

	char Buf[2048];
	sprintf( Buf , "Insert BonusMoneyLog(TargetNPCDBID,TargetPlayerDBID,PlayerDBID,ActionType,ZoneID,PosX,PosZ,NewMoney,OldMoney ) VALUES( 0,0,%d,%d,0,0,0,%d,0)"
		, PlayerDBID
		, Type
		, Money );

	CNetDC_DCBaseChild::This->R_SqlCommand( rand() , EM_SqlCommandType_WorldLogDB , Buf );
}


int		Global::_OnTimeProc_10Sec_( SchedularInfo* Obj , void* InputClass )
{
	if( Global::_IsDestory != false )
		return 0;
	Log_ServerStatus( );
	Schedular()->Push( _OnTimeProc_10Sec_ , 10*1000 , NULL , NULL );
	return 0;
}

void	Global::GuildResourceLog( int GuildID , int PlayerDBID , GuildResourceStruct& Resource , int LogType , int LogType_ObjID )
{
	char Buf[2048];
	int	Time = ( TimeStr::Now_Value() + 60*60*RoleDataEx::G_TimeZone ) / (60*60*24);

	sprintf( Buf , "Execute GuildContributionLog %d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
			, GuildID
			, PlayerDBID
			, Time
			, Resource._Value[0]
			, Resource._Value[1]
			, Resource._Value[2]
			, Resource._Value[3]
			, Resource._Value[4]
			, Resource._Value[5]
			, Resource._Value[6]			);
	CNetDC_DCBaseChild::This->R_SqlCommand( GuildID , EM_SqlCommandType_WorldDB , Buf );


	sprintf( Buf , "INSERT INTO GuildContributionLog(GuildID, PlayerDBID, Gold, BonusGold, Mine, Wood, Herb, GuildRune, GuildStone, LogType, LogType_ObjID) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
		, GuildID
		, PlayerDBID
		, Resource._Value[0]
		, Resource._Value[1]
		, Resource._Value[2]
		, Resource._Value[3]
		, Resource._Value[4]
		, Resource._Value[5]
		, Resource._Value[6]	
		, LogType 
		, LogType_ObjID			);

	CNetDC_DCBaseChild::This->R_SqlCommand( rand() , EM_SqlCommandType_WorldLogDB , Buf );
}

void		Global::Log_AC( DB_ACItemStruct* Item , AcActionTypeENUM Type , int BuyerDBID , int EventMoney  )
{
	char Buf[2048];

	sprintf( Buf , "Insert AcLog(AcDBID,Type,SellerDBID,BuyerDBID,ItemID,ItemCount,EventMoney,PriceType) VALUES(%d,%d,%d,%d,%d,%d,%d,%d)"
		, Item->GUID
		, Type 
		, Item->SellerDBID
		, Item->BuyerDBID
		, Item->Item.OrgObjID
		, Item->Item.Count
		, EventMoney 
		, Item->PricesType);

	CNetDC_DCBaseChild::This->R_SqlCommand( rand() , EM_SqlCommandType_WorldLogDB , Buf );
}

int		Global::GetWorldID()
{
	int Ret = _Net->GetWorldID();
	if( Ret != -1 && Ret != _WorldID )
		Print( LV5 , "GetWorldID" , "( _Net->GetWorldID() != _WorldID )_WorldID Setting Error"  ); 
	return _WorldID;
}

//----------------------------------------------------------------------------------------
void     Global::SendToOtherWorld_GSrvID( int WorldID , int GSrvID , int Size , void* Data )
{
	_Net->SendToOtherWorld_GSrvType( WorldID , EM_SERVER_TYPE_LOCAL , GSrvID , Size , Data );
}
//----------------------------------------------------------------------------------------
void     Global::SendToOtherWorld_NetID( int WorldID , int NetID , int Size , void* Data )
{
	_Net->SendToOtherWorld_NetID( WorldID , NetID , Size , Data );
}
//----------------------------------------------------------------------------------------
void   Global::SendToOtherWorld_ServerType( int WorldID , EM_SERVER_TYPE iServerType , int Size , void* Data )
{
	//_Net->SendToOtherWorld_GSrvID( WorldID , GSrvID , Size , Data );
	_Net->SendToOtherWorld_GSrvType( WorldID , iServerType , 0 , Size , Data );
}
//----------------------------------------------------------------------------------------
/*
void	 Global::SendToOtherWorldZoneClient_ByDBID( int WorldID , int ZoneID , int ClientDBID, int Size, void* Data )
{
	int iDBID = ClientDBID % _DEF_MAX_DBID_COUNT_;
	NetSrv_CliConnect::SBL_SendToZonePlayerByDBID( WorldID, ZoneID, iDBID, Size, Data );
}
//----------------------------------------------------------------------------------------
void	 Global::SendToOtherWorldSrvClient_ByDBID( int WorldID , int SrvID , int ClientDBID, int Size, void* Data )
{
	int iDBID = ClientDBID % _DEF_MAX_DBID_COUNT_;
	NetSrv_CliConnect::SBL_SendToSrvPlayerByDBID( WorldID, SrvID, iDBID, Size, Data );
}
//----------------------------------------------------------------------------------------
*/