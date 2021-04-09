#include	"ProxyMainClass.h"


ProxyMainClass::ProxyMainClass( char*	IniFile )
{
    _ExitFlag = false;
	
    if( _Ini.SetIniFile( IniFile ) == false )
    {
        _ExitFlag = true;  
        return ;
    }

	_Ini.SetIniFile( "Global.ini" );

	g_pPrint->SetPrintLV( _Ini.Int( "DebugPrintLv" ) );
	g_pPrint->SetFileOutputLV( _Ini.Int( "DebugOutputLv" ) );
	g_pPrint->SetOutputFileName( _Ini.Str( "DebugOutputFile" ).c_str() );
	g_pPrint->SetOutputFieldName( _Ini.Str( "DebugOutputField" ).c_str() );


	g_pNetStatus->SetPrintLV( _Ini.Int( "DebugPrintLv" ) );
	g_pNetStatus->SetFileOutputLV( _Ini.Int( "DebugOutputLv" ) );	


	char szNetStatusFileName[256];
	sprintf( szNetStatusFileName, "Netstatus_%s", _Ini.Str( "DebugOutputFile" ).c_str() );

	g_pNetStatus->SetOutputFileName( szNetStatusFileName );
	g_pNetStatus->SetOutputFieldName( _Ini.Str( "DebugOutputField" ).c_str() );



	Print( LV5 , "##開啟Server##" , "時間 %s" , TimeStr::Now_Time()  );

    string IP;
    int    Port;

    IP   = _Ini.Str( "SwitchIP" );
    Port = _Ini.Int( "Switch_ProxyPort" );
    _Proxy.ConnectSwitch(  (char*)IP.c_str() , Port  );
    Print( LV1 , "ProxyMainClass" , "\nConnect to Switch = %s:%d" , (char*)IP.c_str() , Port );   


	IP   = _Ini.Str( "ProxyIP" );
    Port = _Ini.Int( "Proxy_GSrvPort" );
    _Proxy.SetGSrvHost( (char*)IP.c_str() , Port );
    Print( LV1 , "ProxyMainClass" , "\nGSrv Host = %s:%d", (char*)IP.c_str() , Port );   

	IP   = _Ini.Str( "ProxyIP_Outside" );
    Port = _Ini.Int( "Proxy_CliPort" );
    _Proxy.SetCliHost( (char*)IP.c_str() , Port );
    Print( LV1 , "ProxyMainClass" , "\nClient Host = %s:%d", (char*)IP.c_str() , Port );  


	_Proxy._PacketDelay = _Ini.Int( "ProxyPacketDelay" );


	int iCheckClientRespond = _Ini.Int( "DisableProxyCheckClientRespond" );
	if( iCheckClientRespond == 0 )
	{
		//default is open
		_Proxy.Set_Client_NoRespond( true );
	}
	else
	{
		_Proxy.Set_Client_NoRespond( false );
	}

	/*
	int iCheckServerRespond = _Ini.Int( "DisableProxyCheckServerRespond" );
	if( iCheckServerRespond == 0 )
	{
		//default is open
		_Proxy.Set_Server_NoRespond( true );
	}
	else
	{
		_Proxy.Set_Server_NoRespond( false );
	}
	*/

}


ProxyMainClass::~ProxyMainClass( )
{
	
}


//GateWay 資料交換處理
bool	ProxyMainClass::Process( )
{
    _Flush( );
    return !_ExitFlag;
}


void ProxyMainClass::_Flush( )
{
    _Proxy.Process( );
}
