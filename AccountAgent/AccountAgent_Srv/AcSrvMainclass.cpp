#include "AcSrvMainclass.h"



AccountMainClass::AccountMainClass( char*	IniFile )
{
    _ExitFlag = false;
	_Ini.SetIniFile( "Global.ini" );
    if( _Ini.SetIniFile( IniFile ) == false )
    {
        _ExitFlag = true;  
        return ;
    }
    string  AccountIP   = _Ini.Str( "AccountIP" );
    int     AccountPort = _Ini.Int( "AccountPort" );

	_Account = NEW AccountClass(&_Ini);
    _Account->Host( (char*)AccountIP.c_str() , AccountPort );
    //printf("\nHost = %s:%d", (char*)AccountIP.c_str() , AccountPort );   
	PrintToController(false, "\nHost = %s:%d", (char*)AccountIP.c_str() , AccountPort );   

	g_pPrint->SetPrintLV( _Ini.Int( "DebugPrintLv" ) );
	g_pPrint->SetFileOutputLV( _Ini.Int( "DebugOutputLv" ) );
	g_pPrint->SetOutputFileName( _Ini.Str( "DebugOutputFile" ).c_str() );
	g_pPrint->SetOutputFieldName( _Ini.Str( "DebugOutputField" ).c_str() );    

	Print( LV5 , "##Start Server##" , "Time %s" , TimeStr::Now_Time()  );

}



AccountMainClass::~AccountMainClass( )
{
	delete _Account;
}


//GateWay 資料交換處理
bool	AccountMainClass::Process( )
{
    _Flush( );
    return !_ExitFlag;
}


void AccountMainClass::_Flush( )
{
    _Account->Process( );
}

void	AccountMainClass::Exit( )
{
	_ExitFlag = true;
}