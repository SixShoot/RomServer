#include "Global.h"

#include "../Netwalker_Member/NetWakerMasterSrvInc.h"
#include "../Netwalker_Member/Net_ServerList/Net_ServerList_Child.h"
#include "BaseItemObject/M_BaseItemObject.h"
#include "../mainproc/ClientInfo/ClientInfo.h"


//FunctionSchedularClass*	Global::_Schedular = NULL;
IniFileClass	        	Global::_Ini;
bool                    	Global::_IsDelay;
bool						Global::_IsDestroy						= false;
bool						Global::_IsStopLogin					= false;
int							Global::_EnableLimit					= 0;
int							Global::_MaxPlayerCount					= 0;
int							Global::_MaxLimitPlayerCount			= 0;
int							Global::_EnableObjWait					= 1;
int							Global::_EnableGameGuard				= 0;

int							Global::_MaxZonePlayerCount				= 0;
int							Global::_MaxCreateRoleCount				= 0;
int							Global::_PlayerCenterCount				= 1;
int							Global::_IsEnablePlayHistoryLog			= 0;
int							Global::_MaxObjCount					= 3500;
int							Global::_PlayerCount[100];
map< int , vector<int> >	Global::_PlayerZoneCount;
map<int , int>				Global::_ZonePlayerCount;		//區域人數
CountryTypeENUM				Global::_CountryType;

int							Global::_IsEnable_DisconnectPriority	= 0;

char						Global::_szBTI_IP1[100];
int							Global::_iBTI_PORT1			= 0;

char						Global::_szBTI_IP2[100];
int							Global::_iBTI_PORT2			= 0;

bool						Global::_IsEnable_BTI		= false;
bool						Global::_IsEnable_APEX		= false;
bool						Global::_IsEnable_APEX_KICK	= false;
bool						Global::_NewAccountDisableCreateRole = false;
bool						Global::_CheckSecondPassword = false;
bool						Global::_CheckSecondPassword_ExtraCheck_Captcha = false;
int							Global::_CaptchaMaxStringLength	= 0;
int							Global::_CaptchaAlphabetCount	= 0;
int							Global::_CaptchaBackGroundImageCount = 0;
int							Global::_CaptchaNoiseImageCount = 0;
char						Global::_szVivoxUrl[256];
char						Global::_szVivoxAccount[50];
char						Global::_szVivoxPassword[50];

char						Global::_AgencyKeyword[10][16];

int							Global::_IsEnableAgencyCCU				= 0;
//--------------------------------------------------------------------------------------
bool     Global::Init( char* IniFile )
{
	RoleDataEx::St_GetObjDB				= GetObjDB;
	_Ini.SetIniFile( "Global.ini" );

	//printf("\n[Init] Set Ini File\n");
	Print(LV2, "Global", "[Init] Set Ini File\n");
	fflush(NULL);

    if( _Ini.SetIniFile( IniFile ) == false )
    {
		::MessageBox( NULL , "GSrv Ini File Read Error!!" , "Error" ,MB_OK );
        return false;
    }


	g_pPrint->SetPrintLV( _Ini.Int( "DebugPrintLv" ) );
	g_pPrint->SetFileOutputLV( _Ini.Int( "DebugOutputLv" ) );
	g_pPrint->SetOutputFileName( _Ini.Str( "DebugOutputFile" ).c_str() );
	g_pPrint->SetOutputFieldName( _Ini.Str( "DebugOutputField" ).c_str() );


	Print( LV5 , "##開啟Server##" , "時間 %s" , TimeStr::Now_Time()  );
	

	_MaxPlayerCount		= _Ini.Int( "MaxPlayerCount" );
	if( _MaxPlayerCount == 0 )
		_MaxPlayerCount = 1800;

	_EnableGameGuard		= _Ini.Int( "EnableGameGuard" );

	_MaxLimitPlayerCount		= _Ini.Int( "MaxLimitPlayerCount" );
	if( _MaxLimitPlayerCount == 0 )
		_MaxLimitPlayerCount = 2000;

	_MaxObjCount				= _Ini.Int( "MaxObjCount" );
	if( _MaxObjCount == 0 )
		_MaxObjCount = 3500;

	if( _Ini.Int( "EnableObjWait", _EnableObjWait ) == false )
		_EnableObjWait = 1;

	_MaxCreateRoleCount		= _Ini.Int( "MaxCreateRoleCount" );
	if( _MaxCreateRoleCount == 0 )
		_MaxCreateRoleCount = 10;

	_PlayerCenterCount		= _Ini.Int( "PlayerCenterCount" );
	if( _PlayerCenterCount == 0 )
		_PlayerCenterCount = 1;


	_MaxZonePlayerCount		= _Ini.Int( "MaxZonePlayerCount" );
	if( _MaxZonePlayerCount == 0 )
		_MaxZonePlayerCount = 150;

	_IsEnableAgencyCCU = _Ini.Int( "IsEnableAgencyCCU" );


	// 是否啟用排隊系統斷線優先登入權

	if( _Ini.Int( "IsEnable_DisconnectPriority", _IsEnable_DisconnectPriority ) == false )
		_IsEnable_DisconnectPriority = 0;


	if( _Ini.Int( "IsEnablePlayHistoryLog", _IsEnablePlayHistoryLog ) == false )
		_IsEnablePlayHistoryLog = 0;


	int CheckValue;
	_CheckSecondPassword = false;
	if( _Ini.Int( "CheckSecondPassword", CheckValue ) != false )
	{
		_CheckSecondPassword = ( CheckValue != 0 );
	}

	_CheckSecondPassword_ExtraCheck_Captcha = false;
	if( _Ini.Int( "CheckSecondPassword_ExtraCheck_Captcha", CheckValue ) != false )
	{
		_CheckSecondPassword_ExtraCheck_Captcha = ( CheckValue != 0 );
	}

	//驗證碼字串長度預設為4, 字母跟數字各兩個
	_CaptchaMaxStringLength = _Ini.IntDef("CaptchaMaxStringLength", 4);
	_CaptchaAlphabetCount	= _Ini.IntDef("CaptchaAlphabetCount", 2);

	//背景圖預設2張, 破壞紋預設4張
	_CaptchaBackGroundImageCount = _Ini.IntDef("CaptchaBackGroundImageCount", 2);
	_CaptchaNoiseImageCount		 = _Ini.IntDef("CaptchaNoiseImageCount", 4);

	_EnableLimit		= _Ini.Int( "EnableWait" );
	_NewAccountDisableCreateRole = (_Ini.Int( "NewAccountDisableCreateRole" ) != 0 ); 

	if( _Ini.Int( "IsEnable_BTI" ) == 0 )
	{
		_IsEnable_BTI = false;
	}
	else
	{
		_IsEnable_BTI = true;
	}

	if( _Ini.Int( "IsEnable_APEX" ) == 0 )
	{
		_IsEnable_APEX = false;
	}
	else
	{
		_IsEnable_APEX = true;
	}

	if( _Ini.Int( "IsEnable_APEX_KICK" ) == 0 )
	{
		_IsEnable_APEX = false;
	}
	else
	{
		_IsEnable_APEX = true;
	}

	

	strcpy_s( _szVivoxUrl,		sizeof( _szVivoxUrl ),		_Ini.Str( "VivoxUrl" ).c_str() );
	strcpy_s( _szVivoxAccount,	sizeof( _szVivoxAccount ),	_Ini.Str( "VivoxAccount" ).c_str() );
	strcpy_s( _szVivoxPassword,	sizeof( _szVivoxPassword ), _Ini.Str( "VivoxPassword" ).c_str() );


	for( int i=1; i<=10;i++)
	{
		char szKey[256];
		sprintf( szKey, "AgencyKeyword%d", i );
		strcpy_s( _AgencyKeyword[(i-1)],	sizeof( _AgencyKeyword[(i-1)] ), _Ini.Str( szKey ).c_str() );
	}

	strcpy_s( _szBTI_IP1, sizeof( _szBTI_IP1 ), _Ini.Str( "BTI_IP1" ).c_str() );
	_iBTI_PORT1	= _Ini.Int( "BTI_PORT1" );

	strcpy_s( _szBTI_IP2, sizeof( _szBTI_IP2 ), _Ini.Str( "BTI_IP2" ).c_str() );
	_iBTI_PORT2	= _Ini.Int( "BTI_PORT2" );

	memset( _PlayerCount , 0 , sizeof(_PlayerCount) );

	//printf("\n[Init] Ini loaded\n");
	Print(LV2, "Global", "[Init] Ini loaded\n");
	fflush(NULL);

	// 讀取 Server Data 與 String 的設定
	string LanguageTypeStr = _Ini.Str( "Stringlanguage" );
	string DatalanguageStr = _Ini.Str( "Datalanguage" );

	//--------------------------------------------------------------------------------------
	ObjectDataClass::InitRuneEngine( _Ini.Str("DataPath").c_str() );
	ObjectDataClass::Init( _Ini.Str("DataPath").c_str() , DatalanguageStr.c_str() , LanguageTypeStr.c_str() );
	_CountryType = (CountryTypeENUM)g_ObjectData->QueryCountryID( DatalanguageStr.c_str() );
	//printf("\n[Init] ObjectDataClass::Init\n");
	Print(LV2, "Global", "\n[Init] ObjectDataClass::Init\n");
	fflush(NULL);
	//--------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------
	ObjectDataClass::LoadSpecialCharacterName( _Ini.Str("SpecialCharacterNameFile").c_str() );
	//printf("\n[Init] ObjectDataClass::LoadSpecialCharacterName\n");
	Print(LV2, "Global", "[Init] ObjectDataClass::LoadSpecialCharacterName\n");
	fflush(NULL);

	//--------------------------------------------------------------------------------------
	GlobalBase::_Init();
	
	
	//--------------------------------------------------------------------------------------
	BaseItemObject::Init( );
	//printf("\n[Init] BaseItemObject::Init\n");	fflush(NULL);
	Print(LV2, "Global", "[Init] BaseItemObject::Init\n");	fflush(NULL);

    //--------------------------------------------------------------------------------------
	LUA_VMClass::Init();
	//printf("\n[Init] LUA_VMClass::Init\n");	fflush(NULL);
	Print(LV2, "Global", "[Init] LUA_VMClass::Init\n");	fflush(NULL);
    //劇情資料初始化
    LuaPlotClass::Init( );
	//printf("\n[Init] LuaPlotClass::Init\n");	fflush(NULL);
	Print(LV2, "Global", "[Init] LuaPlotClass::Init\n");	fflush(NULL);

	//--------------------------------------------------------------------------------------
	CClientManger::Init();
	//printf("\n[Init] CClientManger::Init\n");	fflush(NULL);
	Print(LV2, "Global", "[Init] CClientManger::Init\n");	fflush(NULL);
    //--------------------------------------------------------------------------------------
    //網路封包資料初始化
    //--------------------------------------------------------------------------------------
    //NetSrv_TestChild::Init();
	Net_ServerList->Init(  NULL , EM_SERVER_TYPE_MASTER , DF_NO_LOCALID );
	
	
	Net_DCBaseChild::Init();
	CNetSrv_MasterServer_Child::Init();
	CNetMaster_ZoneStatus_Child::Init();
	NetSrv_OnlinePlayerChild::Init();
	NetSrv_GMToolsChild::Init();
	Net_OtherChild::Init();

	_Net->RegGSrvNetID( EM_SERVER_TYPE_MASTER );
	//--------------------------------------------------------------------------------------
	// ServerList 
	//--------------------------------------------------------------------------------------
	
	//CServer
    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------
    //GSrvNetWaker::Init();
	_Net->Connect( _Ini.Str( "SwitchIP" ).c_str() , _Ini.Int( "Switch_GSrvPort" ) , "MasterServer" );
	//printf("\n[Init] _Net->Connect\n");	fflush(NULL);
	Print(LV2, "Global", "[Init] _Net->Connect\n");	fflush(NULL);
	if( _Ini.Int( "IsBattleGroundServer" ) != 1 )
		_Net->ConnectType( EM_GSrvType_Active );


	//CNetSrv_MasterServer_Child::m_pThis->m_vivox.m_country		= _Ini.Str( "Vivoxlanguage" );
	//CNetSrv_MasterServer_Child::m_pThis->m_vivox.m_url			= _szVivoxUrl;
	//CNetSrv_MasterServer_Child::m_pThis->m_vivox.m_clienturl	= _Ini.Str( "VivoxClientUrl" );
	//CNetSrv_MasterServer_Child::m_pThis->m_vivox.m_account		= _szVivoxAccount;
	//CNetSrv_MasterServer_Child::m_pThis->m_vivox.m_password		= _szVivoxPassword;

	
    //GSrvNetWaker::Connect( _Ini.Str( "SwitchIP" ) , _Ini.Int( "Switch_GServerPort" ) , "MasterServer" );
    //GSrvNetWaker::ConnectType( EM_GSrvType_Active );

	

    return true;
}
//--------------------------------------------------------------------------------------
bool     Global::Release( )
{
	//--------------------------------------------------------------------------------------
	GlobalBase::_Release();
	//--------------------------------------------------------------------------------------
	CClientManger::Release();
	//--------------------------------------------------------------------------------------
//	CNet_ServerList_Child::Release( );
	Net_DCBaseChild::Release();
	CNetSrv_MasterServer_Child::Release();
	CNetMaster_ZoneStatus_Child::Release();
	NetSrv_OnlinePlayerChild::Release();
	NetSrv_GMToolsChild::Release();
	Net_OtherChild::Release();
	//--------------------------------------------------------------------------------------
	LUA_VMClass::Release();
//	_Schedular->Del();
	BaseItemObject::Release( );
	ObjectDataClass::Release();
	
    return true;
}
//--------------------------------------------------------------------------------------
bool     Global::Process( )
{
	RoleDataEx::G_NowStr = TimeStr::Now();	
	RoleDataEx::G_Now = TimeStr::Now_Value() + RoleDataEx::SysDTime;

    static int		ProcTime = timeGetTime();
    static int		LastProcDelayTime = 0;
    static int		LastProcDelayRecorder = 0;
    
    LastProcDelayTime = timeGetTime() -  ProcTime;
	_Net->Process();
    if( LastProcDelayTime > 100 )
    {
        ProcTime += 100;
        {
            //GSrvNetWaker::Process();
//            _Schedular->Process( );
            LUA_VMClass::Process( );
        }
    }

    if( LastProcDelayTime > 1000)
    {
        if( LastProcDelayRecorder < LastProcDelayTime )
        {
            LastProcDelayRecorder = LastProcDelayTime;
            //Global::Output( 0,"Program Delay %d " , LastProcDelayTime );
			Print( Def_PrintLV2 , "Process" , "Program Delay %d " , LastProcDelayTime );
        }
        Global::_IsDelay = true;
    }
    else
    {
        LastProcDelayRecorder = 0;
        Global::_IsDelay = false;
    }
    //Sleep( 10 );
	//SleepEx( 10, true );
    return !_IsDestroy;
}

int	Global::GetPCenterID( const char* account )
{
	int hashID = GetHashCode( account , strlen( account ) );
	return hashID % _PlayerCenterCount;
}

//重新計算線上人物資料
void Global::RecalculateOnlinePlayer()
{
	//有開的zone
	bool	OpenZoneMap[100][50];
	memset( &OpenZoneMap , 0 , sizeof(OpenZoneMap) );

	memset( _PlayerCount , 0 , sizeof(_PlayerCount) );
	_PlayerZoneCount.clear();
	_ZonePlayerCount.clear();

	int		ParZoneCount[100];
	int		MaxParZonePlayerCount[100];
	memset( MaxParZonePlayerCount , 0 , sizeof(MaxParZonePlayerCount) );

	memset( ParZoneCount , 0 , sizeof(ParZoneCount) );

	vector< ServerData* >& LocalSrvList = Net_ServerList->GetLocalList();
	//重算區域是否開起
	for( int i = 0 ; i < (int)LocalSrvList.size() ; i++ )
	{
		if( LocalSrvList[i] == NULL )
			continue;

		int ZoneID = LocalSrvList[i]->m_dwServerLocalID;
		int BaseZoneID = ZoneID % _DEF_ZONE_BASE_COUNT_;
		int ParID = ZoneID / _DEF_ZONE_BASE_COUNT_;

		if( LocalSrvList[i]->m_bServerRdy != false )
		{
			while( ParID >= (int)_PlayerZoneCount[ BaseZoneID ].size() )
			{
				vector<int> TempVector;
				_PlayerZoneCount[  BaseZoneID ].push_back( -1 ); 
			}

			_PlayerZoneCount[  BaseZoneID ][ ParID ] = 0;
			if( BaseZoneID > 0 && BaseZoneID < 100 )
			{
				ParZoneCount[ ParID ] ++;
				OpenZoneMap[BaseZoneID][ParID] = true;
			}

			
		}
	}
	
	int MaxParZoneCount = 1;
	int	ParCount = 0;
	for( ParCount = 0 ; ParCount < 100 ; ParCount++ )
	{
		if( ParZoneCount[ ParCount ] <= 0 )
			break;

		if( MaxParZoneCount < ParZoneCount[ ParCount ]  )
			MaxParZoneCount = ParZoneCount[ ParCount ];
	}

	
	//g_pClientManger->m_iActiveRole = (int)g_pClientManger->m_mapDBID.size();
	//if( g_pClientManger->m_iActiveRole < )

	map< int,		ClientInfo* >::iterator Iter;;
	for( Iter  = g_pClientManger->m_mapDBID.begin() ; Iter != g_pClientManger->m_mapDBID.end() ; Iter++ )
	{		
		if( Iter->second->m_setZone.size() == 0 )
		{
			continue;
		}
		
		set< int >::iterator ZoneIter = Iter->second->m_setZone.begin();
		_ZonePlayerCount[ *ZoneIter ]++;

		int ParID = *ZoneIter / _DEF_ZONE_BASE_COUNT_;
		
		int	ZoneBase = (*ZoneIter) % _DEF_ZONE_BASE_COUNT_;

		if( ZoneBase > 0 && ZoneBase < 100 )
		{
			_PlayerCount[ ParID ]++;

			if( ParID == 0 )
			{
				for( int i = 1 ; i < MaxParZoneCount ; i++ )
				{
					if( OpenZoneMap[ ZoneBase ][i] == false )
						_PlayerCount[i]++;
				}
			}

		}

	}

	//每個平行空世界 每個Zone的平均人數
	for( int i = 0 ; i < Net_ServerList->GetParallelZoneCount() ; i++ )
	{
		if( ParZoneCount[ i ] != 0 )
			_PlayerCount[ i ] = _PlayerCount[ i ] / MaxParZoneCount;
			//_PlayerCount[ i ] = _PlayerCount[ i ] / ParZoneCount[ i ];
	}
	
	//取最大的平行世界的人數
	/*
	for( int i = 0 ; i < LocalSrvList.size() ; i++ )
	{
		int ParID = i / _DEF_ZONE_BASE_COUNT_;
		if( MaxParZonePlayerCount[ ParID] < _PlayerCount[i] )
			MaxParZonePlayerCount[ ParID] = _PlayerCount[i];
	}
	*/
	//_ZonePlayerCount
	map<int , int>::iterator MapIter;
	for( MapIter = _ZonePlayerCount.begin() ; MapIter != _ZonePlayerCount.end() ; MapIter++ )
	{
		int ParID = MapIter->first / _DEF_ZONE_BASE_COUNT_;
		if( MaxParZonePlayerCount[ ParID] < MapIter->second )
			MaxParZonePlayerCount[ ParID] = MapIter->second;
	}

	float TotalPlayerRate = float( g_pClientManger->GetActive() ) / _MaxPlayerCount;

	//暫時
	if( Global::_NewAccountDisableCreateRole != false )
	{
		int TempPlayerCount[100];
		for( int i = 0 ; i < ParCount ; i++ )
			TempPlayerCount[i] = _MaxZonePlayerCount;
		Net()->RegServerLoading( _MaxZonePlayerCount , ParCount , TempPlayerCount , TotalPlayerRate );
	}
	else
	{
		Net()->RegServerLoading( _MaxZonePlayerCount , ParCount , MaxParZonePlayerCount , TotalPlayerRate );
	}
}

GameObjDbStructEx*    Global::GetObjDB( int OrgID )
{
	if( OrgID == 0 )
		return NULL;
	return g_ObjectData->GetObj( OrgID );
}

//Log儲存
void		Global::Log_ServerStatus( )
{
	char Buf[2048];
	int		EnterWorldCount = 0;
	int		SelectRoleCount = 0;
	int		WaitLeaveWorldCount = 0;

	map< string , ClientInfo* >::iterator Iter;

	//1 計算所有管理的物件進入世界 與沒進入的量
	for( Iter = g_pClientManger->m_mapAccount.begin() ; Iter != g_pClientManger->m_mapAccount.end() ; Iter++ )
	{
		if( Iter->second->m_emClientStatus == EM_ClientStatus_WaitLeaveWorld )
			WaitLeaveWorldCount++;

		/*
		if( Iter->second->m_bEnterWorld != false )
			EnterWorldCount++;
		else
			SelectRoleCount++;
		*/
	}

	EnterWorldCount = (int)g_pClientManger->m_mapDBID.size();
	SelectRoleCount	= (int)g_pClientManger->m_mapClientInfo.size() - EnterWorldCount;


	sprintf( Buf , "Insert MasterServerStatus(EnterWorldPlayer,SelectRolePlayer,WaitLeaveWorld,LoadQueue) VALUES(%d,%d,%d,%d)"
		, EnterWorldCount
		, SelectRoleCount
		, WaitLeaveWorldCount
		, CNetSrv_MasterServer_Child::m_vecWaitQueue.size() );

	Net_DCBase::LogSqlCommand( Buf );

	if( _IsEnableAgencyCCU == 1 )
	{
		for( int i=0; i<5; i++ )
		{
			if( strlen( _AgencyKeyword[i] ) != 0 )
			{
				sprintf( Buf , "Insert AgencyCCU(AgencyID,AgencyCCU) VALUES(%d,%d)", (i+1), ((CNetSrv_MasterServer_Child*)CNetSrv_MasterServer_Child::m_pThis)->m_iAgency[i] );
				Net_DCBase::LogSqlCommand( Buf );
			}
		}
	}
}