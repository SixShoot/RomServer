#include "Global.h"
#include "../NetWalker_Member/NetWakerChatInc.h"
#include "GroupZoneChannel/GroupZoneChannel.h"
#include "StackWalker/StackWalker.h"
//#include "CrashRpt/crashrpt.h" 

IniFileClass	            	Global::_Ini;
bool                        	Global::_IsDelay;
bool							Global::_IsDestroy = false;
bool							Global::_IsTalkLog = false;
int								Global::_LastProcTime=0;
vector< ItemSendBaseStruct >	Global::_ItemSendList;
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
	Print( LV5 , "##開啟Server##" , "時間 %s" , TimeStr::Now_Time()  );

    GlobalBase::_Init();

    //--------------------------------------------------------------------------------------
	
    //劇情資料初始化
    LuaPlotClass::Init( );
	LUA_VMClass::Init();


    //--------------------------------------------------------------------------------------
    //網路封包資料初始化
    //--------------------------------------------------------------------------------------
    NetSrv_ChannelBaseChild::Init();
	NetSrv_OnlinePlayerChild::Init();
	NetSrv_TalkChild::Init();
	NetSrv_PartyChild::Init();
	NetSrv_FriendListChild::Init();
	NetSrv_GMToolsChild::Init();
	Net_DCBaseChild::Init();
	_Net->RegGSrvNetID( EM_SERVER_TYPE_CHAT );
	

    Net_ServerList->Init( NULL , EM_SERVER_TYPE_CHAT , DF_NO_LOCALID );
	BaseItemObject::Init( );
	GroupObjectClass::Init( );

	string Datalanguage;
	string Stringlanguage;
	// 讀取 Server Data 與 String 的設定
	if( _Ini.Str( "Datalanguage", Datalanguage ) == false )
	{
		Datalanguage = "rw";
	}

	Datalanguage = strlwr( (char*)Datalanguage.c_str() );

	if( _Ini.Str( "Stringlanguage", Stringlanguage ) == false )
	{
		Stringlanguage = "";
	}

	ObjectDataClass::InitRuneEngine( _Ini.Str("DataPath").c_str() );
	ObjectDataClass::Init( _Ini.Str( "DataPath" ).c_str() , Datalanguage.c_str() , Stringlanguage.c_str() );

	RoleDataEx::G_TimeZone = _Ini.Int( "TimeZone" );


	if( _Ini.Int( "IsTalkLog" ) != 0 )
		_IsTalkLog = true;
	else
		_IsTalkLog = false;
    //--------------------------------------------------------------------------------------

    _Net->Connect( _Ini.Str( "SwitchIP" ).c_str() , _Ini.Int( "Switch_GSrvPort" ) , "ChatSrv" );
//    _Net->ConnectType( EM_GSrvType_Active );
	if( _Ini.Int( "IsBattleGroundServer" ) != 1 )
		_Net->ConnectType( EM_GSrvType_Active );
    //--------------------------------------------------------------------------------------
	//區域頻道建立
	GroupZoneChannel::Init();
	//--------------------------------------------------------------------------------------

    return true;
}
//--------------------------------------------------------------------------------------
bool     Global::Release( )
{
	//--------------------------------------------------------------------------------------
	//網路封包資料初始化
	//--------------------------------------------------------------------------------------
	NetSrv_ChannelBaseChild::Release( );
	NetSrv_OnlinePlayerChild::Release();
	NetSrv_TalkChild::Release();
	NetSrv_PartyChild::Release();
	NetSrv_FriendListChild::Release();
	NetSrv_GMToolsChild::Release();
	Net_DCBaseChild::Release();
	//--------------------------------------------------------------------------------------
	BaseItemObject::Release( );
	GroupObjectClass::Release( );
	//--------------------------------------------------------------------------------------
	

//    _Schedular->Del();
    GlobalBase::_Release();
	LUA_VMClass::Release();

	GroupZoneChannel::Release();
	
    return true;
}
//--------------------------------------------------------------------------------------
bool     Global::Process( )
{
	static int		ProcCount = 0;
    static int		ProcTime = timeGetTime();
    static int		LastProcDelayTime = 0;
    static int		LastProcDelayRecorder = 0;

    _LastProcTime = ProcTime;

    LastProcDelayTime = timeGetTime() -  ProcTime;
	_Net->Process();
    if( LastProcDelayTime > 100 )
    {
		ProcCount++;
        ProcTime += 100;
        
        Schedular()->Process( );
        LUA_VMClass::Process( );    

		if( ProcCount % 100 == 0 )
		{
			SendItemListProc();
		}

		if( ProcCount % 600 == 0 )
		{
			NetSrv_TalkChild::OnTimeProcCliBoardInfo( );
		}
    }

    if( LastProcDelayTime > 1000)
    {
        if( LastProcDelayRecorder < LastProcDelayTime )
        {
            LastProcDelayRecorder = LastProcDelayTime;
            Print( Def_PrintLV2 , "Process" , "Program Delay %d " , LastProcDelayTime );
        }
        Global::_IsDelay = true;
    }
    else
    {
        LastProcDelayRecorder = 0;
        Global::_IsDelay = false;
    }
//    SleepEx( 10, true );

    return !_IsDestroy;
}

GameObjDbStructEx*    Global::GetObjDB( int OrgID )
{
	if( OrgID == 0 )
		return NULL;
	return g_ObjectData->GetObj( OrgID );
	//return _St->ObjectDB.GetObject( OrgID );
}

void		Global::Destroy()
{
	_IsDestroy = true;
}


void	Global::Log_Talk			( TalkLogTypeENUM Type , int ChannelID , int FromPlayerDBID , int ToPlayerDBID , const char* Content )
{

	if( _IsTalkLog == false )
		return;

	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String( Content );
	string CmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );


	char Buf[ 4096];
	sprintf( Buf , "INSERT INTO TalkLog( Type , ChannelID , FromPlayerDBID , ToPlayerDBID , Content ) VALUES ( %d,%d,%d,%d, CAST( %s AS nvarchar(4000) ) )" 
		, Type 
		, ChannelID
		, FromPlayerDBID
		, ToPlayerDBID
		, CmdBinStr.c_str()	);
	Net_DCBase::LogSqlCommand( Buf );

}

void		Global::SendItemListProc()
{
	//vector< ItemSendBaseStruct >	_ItemSendList;
	vector< ItemSendBaseStruct >	BkItemSendList = _ItemSendList;
	_ItemSendList.clear();

	for( unsigned i = 0 ; i < BkItemSendList.size() ; i++ )
	{
		ItemSendBaseStruct& Info = BkItemSendList[i];
		BaseItemObject* Player = BaseItemObject::GetObjByDBID( Info.PlayerDBID );         //取物件	
		if( Player == NULL )
		{
			_ItemSendList.push_back( Info );
			continue;
		}
		NetSrv_PartyChild::SL_GiveItem( Player->ZoneID() , Player->DBID() , Player->LocalID() , Info.Item );
	}

}