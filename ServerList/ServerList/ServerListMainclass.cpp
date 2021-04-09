#include "ServerListMainclass.h"


ServerListMainClass::ServerListMainClass( char*	IniFile )
{
    _ExitFlag = false;
	_Ini.SetIniFile( "Global.ini" );
    if( _Ini.SetIniFile( IniFile ) == false )
    {
        _ExitFlag = true;  
        return ;
    }
    string  ServerListIP		= _Ini.Str( "ServerListIP" );
	string  ServerListIP_Outside= _Ini.Str( "ServerListIP_Outside" );
    int     ServerList_CliPort	= _Ini.Int( "ServerListCliPort" );
	int     ServerList_SrvPort	= _Ini.Int( "ServerListSrvPort" );

	_SrvList = NEW ServerListClass( &_Ini );
	_SrvList->Host( (char*)ServerListIP.c_str() , (char*)ServerListIP_Outside.c_str() , ServerList_CliPort , ServerList_SrvPort  );
	_SrvList->_ServerListMode = _Ini.Int( "ServerListMode" );

    //printf("\nHost = %s:CliPort = %d  SrvPort = %d", (char*)ServerListIP.c_str() , ServerList_CliPort , ServerList_SrvPort );   
	PrintToController(false, "\nHost = %s:CliPort = %d  SrvPort = %d",(char*)ServerListIP.c_str() , ServerList_CliPort , ServerList_SrvPort );

	g_pPrint->SetPrintLV( _Ini.Int( "DebugPrintLv" ) );
	g_pPrint->SetFileOutputLV( _Ini.Int( "DebugOutputLv" ) );
	g_pPrint->SetOutputFileName( _Ini.Str( "DebugOutputFile" ).c_str() );
	g_pPrint->SetOutputFieldName( _Ini.Str( "DebugOutputField" ).c_str() );
	Print( LV5 , "##開啟Server##" , "時間 %s" , TimeStr::Now_Time()  );
}



ServerListMainClass::~ServerListMainClass( )
{
	delete 	_SrvList;
}


//GateWay 資料交換處理
bool	ServerListMainClass::Process( )
{
    _Flush( );
    return !_ExitFlag;
}


void ServerListMainClass::_Flush( )
{
    _SrvList->Process( );
}

void	ServerListMainClass::Exit( )
{
	_ExitFlag = true;
}