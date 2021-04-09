#include "Global.h"
#include "../NetWalker_Member/NetWakerPartInc.h"

#include "Rune Engine/Rune/Rune.h"
#include "Rune Engine/Rune/Rune Engine NULL.h"
#include "Rune Engine/Rune/Fusion/RuFusion_Trekker.h"


IniFileClass	            Global::_Ini;
bool                        Global::_IsDelay;
bool                        Global::_IsDestroy = false;
//FunctionSchedularClass*     Global::_Schedular;
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

	if( RuInitialize_Core() == FALSE )
	{
		Print( LV2 , "PathManageClass::Init" , "RuInitialize_Core() == FALSE " );
		return false;
	}

	// Initialize NULL engine
	if( RuInitialize_NULL() == FALSE )
	{
		Print( LV2 , "PathManageClass::Init" , "RuInitialize_NULL() == FALSE " );
		return false;
	}
/*
	// Initialize scene library
	if( RuInitialize_Scene(NULL) == FALSE )
	{
		Print( LV2 , "PathManageClass::Init" , "RuInitialize_NULL() == FALSE " );
		return false;
	
	}
	*/
	string  RootPath = Global::Ini()->Str("ResourcePath");
	// Setup resource manager's root path
	g_ruResourceManager->SetRootDirectory( RootPath.c_str() );

	if ( g_ruResourceManager )
	{
		g_ruResourceManager->LoadPackageFile( "fdb\\model.fdb"  );
	}

    GlobalBase::_Init();
//    Print.Init( "Chat.log" );
//    _Schedular = FunctionSchedularClass::New( );


    BaseItemObject::Init( );

	
    //--------------------------------------------------------------------------------------
    //網路封包資料初始化
    //--------------------------------------------------------------------------------------

	 //Net_ServerList->Init( NULL , EM_SERVER_TYPE_PARTITION , DF_REQUEST_LOCALID );
	Net_ServerList->Init( NULL , EM_SERVER_TYPE_PARTITION , DF_NO_LOCALID );

    CNetPart_PartBaseChild::Init();	
	NetSrv_OnlinePlayerChild::Init();
	NetSrv_GMToolsChild::Init();
    //--------------------------------------------------------------------------------------

    _Net->Connect( _Ini.Str( "SwitchIP" ).c_str() , _Ini.Int( "Switch_GSrvPort" ) , "Partition" );
//    _Net.ConnectType( EM_GSrvType_Active );


    return true;
}
//--------------------------------------------------------------------------------------
bool     Global::Release( )
{
    GlobalBase::_Release();
	CNetPart_PartBaseChild::Release();
	NetSrv_OnlinePlayerChild::Release();
	NetSrv_GMToolsChild::Release();
//    _Schedular->Del( );
	
    return true;
}
//--------------------------------------------------------------------------------------
bool     Global::Process( )
{
    static int		ProcTime = timeGetTime();
    static int		LastProcDelayTime = 0;
    static int		LastProcDelayRecorder = 0;
    
    LastProcDelayTime = timeGetTime() -  ProcTime;

	_Net->Process();

    if( LastProcDelayTime > 100 )
    {
        ProcTime += 100;
        {
            Schedular()->Process( );
        }
    }

    if( LastProcDelayTime > 1000)
    {
        if( LastProcDelayRecorder < LastProcDelayTime )
        {
            LastProcDelayRecorder = LastProcDelayTime;
            Print( Def_PrintLV1 , "Process" , "Program Delay %d " , LastProcDelayTime );
        }
        Global::_IsDelay = true;
    }
    else
    {
        LastProcDelayRecorder = 0;
        Global::_IsDelay = false;
    }
//    Sleep( 10 );
    return !_IsDestroy;
}



