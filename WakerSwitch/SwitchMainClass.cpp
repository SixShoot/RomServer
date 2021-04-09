#include	"SwitchMainClass.h"
#include	"RemotetableInput/RemotableInput.h"
//---------------------------------------------------------------------------------
SwitchMainClass::SwitchMainClass( char*	IniFile )
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
	Print( LV5 , "##Start Server##" , "Time %s" , TimeStr::Now_Time()  );

    string IP;
    int    Port;
    string GameName;
    int    GameID;

	if(_Ini.Int( "IsAutoLogin" ) == 1)
	{
		Switch()->SetUpdateListMode( true );
	}

	if(_Ini.Int( "NoPassworrd" ) == 1)
		SwitchClass::m_NeedPassword = false;

    IP   = _Ini.Str( "SwitchIP" );
    Port = _Ini.Int( "Switch_ProxyPort" );
    _Switch.HostProxy( (char*)IP.c_str() , Port );
    Print( LV1 , "SwitchMainClass" ,"HostProxy = %s:%d" , (char*)IP.c_str() , Port );   

    Port = _Ini.Int( "Switch_GSrvPort" );
    _Switch.HostGSrv( (char*)IP.c_str() , Port );
    Print( LV1 , "SwitchMainClass" , "Host Game Server = %s:%d", (char*)IP.c_str() , Port);   

	IP   = _Ini.Str( "SwitchIP_Outside" );
    Port = _Ini.Int( "Switch_CliPort" );
    _Switch.HostCli( (char*)IP.c_str() , Port );
    Print( LV1 , "SwitchMainClass" , "Host Game Cli = %s:%d" , (char*)IP.c_str() , Port );   


	IP   = _Ini.Str( "GatewayIP" );
	Port = _Ini.Int( "GatewayPort" );
	if( IP.length() != 0 )
		_Switch.ConnectGateway( (char*)IP.c_str() , Port );
	Print( LV1 , "SwitchMainClass" , "ConnectGateway = %s:%d" , (char*)IP.c_str() , Port );   



	GameName = _Ini.Str( "GameName" );

    GameID   = _Ini.Int( "GameID" );
	int GameGroupID = _Ini.Int( "GameGroupID" );

	SwitchClass::m_WorldID = GameID;

    IP       = _Ini.Str( "AccountIP" );
    Port     = _Ini.Int( "AccountPort" );

	if( SwitchClass::m_NeedPassword )
		_Switch.ConnectAc( (char*)IP.c_str() , Port , GameID , GameGroupID , (char*)GameName.c_str() );
	else
	{
		g_remotableInput.OutputStartupMessage();
	}

	IP       = _Ini.Str( "ServerListIP" );
	Port     = _Ini.Int( "ServerListSrvPort" );
	if( IP.length() != 0 )
		_Switch.ConnectServerList( (char*)IP.c_str() , Port );

	int iCheckClientRespond = _Ini.Int( "DisableSwitchCheckClientRespond" );
	if( iCheckClientRespond == 0 )
	{
		//default is open
		_Switch.Set_Client_NoRespond( true );
	}
	else
	{
		_Switch.Set_Client_NoRespond( false );
	}

	//載入重新導向的資料
	string RedirectIP;
	string RedirectCountry;
	int	   RedirectPort;
	char	Buf[512];
	for( int i = 1 ; ; i++ )
	{
		sprintf( Buf , "Redirect%d_Country" , i );
		RedirectCountry = _Ini.Str( Buf );
		if( RedirectCountry.length() == 0 )
			break;
		sprintf( Buf , "Redirect%d_IP"		, i );
		RedirectIP = _Ini.Str( Buf );

		sprintf( Buf , "Redirect%d_Port"	, i );
		RedirectPort = _Ini.Int( Buf );

		_Switch.SetRedirectHost( RedirectCountry , RedirectIP , RedirectPort );
	}
	
	_Switch.LoadApnicFile( _Ini.Str( "APNIC_File" ) );

	_Switch._ServerStatus = _Ini.Int( "ServerStatus" );
}
//---------------------------------------------------------------------------------
SwitchMainClass::~SwitchMainClass( )
{
}
//---------------------------------------------------------------------------------
//GateWay 資料交換處理
bool	SwitchMainClass::Process( )
{
    _Flush( );
    return !_ExitFlag;
}
//---------------------------------------------------------------------------------
void SwitchMainClass::_Flush( )
{
    _Switch.Process( );
}
//---------------------------------------------------------------------------------
//結束程式 
void    SwitchMainClass::Exit( )
{
	_ExitFlag = true;
}
//----------------------------------------------------------------------------------------
