#include	"GatewayMainClass.h"
//---------------------------------------------------------------------------------
GatewayMainClass::GatewayMainClass( char*	IniFile )
{
	
    _ExitFlag = false;

	_Ini.SetIniFile( "Global.ini" );
    if( _Ini.SetIniFile( IniFile ) == false )
    {
        _ExitFlag = true;  
        return ;
    }

	g_pPrint->SetPrintLV( _Ini.Int( "DebugPrintLv" ) );
	g_pPrint->SetFileOutputLV( _Ini.Int( "DebugOutputLv" ) );
	g_pPrint->SetOutputFileName( _Ini.Str( "DebugOutputFile" ).c_str() );
	g_pPrint->SetOutputFieldName( _Ini.Str( "DebugOutputField" ).c_str() );
	Print( LV5 , "##開啟Server##" , "時間 %s" , TimeStr::Now_Time()  );

    string IP;
    int    Port;

    IP   = _Ini.Str( "GatewayIP" );
    Port = _Ini.Int( "Gateway_SwitchPort" );

    _Gateway.HostSwitch( (char*)IP.c_str() , Port );
    Print( LV1 , "GatewayMainClass" ,"HostSwitch = %s:%d" , (char*)IP.c_str() , Port );   


}
//---------------------------------------------------------------------------------
GatewayMainClass::~GatewayMainClass( )
{
}
//---------------------------------------------------------------------------------
//GateWay 資料交換處理
bool	GatewayMainClass::Process( )
{
    _Flush( );
    return !_ExitFlag;
}
//---------------------------------------------------------------------------------
void GatewayMainClass::_Flush( )
{
    _Gateway.Process( );
}
//---------------------------------------------------------------------------------
//結束程式 
void    GatewayMainClass::Exit( )
{
	_ExitFlag = true;
}
//----------------------------------------------------------------------------------------
