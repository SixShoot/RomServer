#pragma once
#include "KeyBoardCmd.h" 
#include <conio.h>
#include "RemotetableInput/RemotableInput.h"
#include "ControllerClient/ControllerClient.h"
//----------------------------------------------------------------------------------------
//初始化
KeyBoardCmd::KeyBoardCmd( GatewayMainClass* S )
{
	g_remotableInput.Initialize();
	g_remotableInput.OutputStartupMessage();

	_GatewayMain= S;

    KeyBaordCmdManageStruct GMS;

    GMS.Init( "help" , &KeyBoardCmd::Help, "Help 列出全部或某英文字開頭的GM指令\n help a or help " );
    _CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "Exit" , &KeyBoardCmd::Exit, "結束程式" );
	_CmdMap[ GMS.Name ] = GMS;	
	
	GMS.Init( "Ping" , &KeyBoardCmd::Ping, "檢查是否還活著" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "SrvLoadOK" , &KeyBoardCmd::SrvLoadOK, "Server以經全開起了" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "NetCore" , &KeyBoardCmd::NetCore, "Display netcore number of event" );
	_CmdMap[ GMS.Name ] = GMS;	

/*
	GMS.Init( "Census" ,			&KeyBoardCmd::Census,				"Census" );				_CmdMap[ GMS.Name ] = GMS;
	GMS.Init( "Enable_Login" ,		&KeyBoardCmd::Enable_Login,			"Enable_Login" );		_CmdMap[ GMS.Name ] = GMS;
	GMS.Init( "Disable_Login" ,		&KeyBoardCmd::Disable_Login,		"Disable_Login" );		_CmdMap[ GMS.Name ] = GMS;
	GMS.Init( "ServerGroupOkay" ,	&KeyBoardCmd::ServerGroupOkay,		"ServerGroupOkay" );	_CmdMap[ GMS.Name ] = GMS;
	*/
}
//----------------------------------------------------------------------------------------
int         KeyBoardCmd::_Count( )
{
    return (int)_Val.size();
}
//----------------------------------------------------------------------------------------
float		KeyBoardCmd::_GetFloat( int ID )
{
    if( ID >= (int)_Val.size() )
        return 0;

    return (float) atof( (char*)_Val[ID].c_str() );

}
//----------------------------------------------------------------------------------------
int		KeyBoardCmd::_GetNum( int ID )
{
    if( ID >= (int)_Val.size() )
        return 0;

    return atol( (char*)_Val[ID].c_str() );

}
//----------------------------------------------------------------------------------------
char*	KeyBoardCmd::_GetStr( int ID )
{
    if( ID >= (int)_Val.size() )
        return "";

    return (char*)_Val[ID].c_str();

}
//----------------------------------------------------------------------------------------
//GM命令處理
bool	KeyBoardCmd::CmdProc( const char* Str )
{

    int		i;
    char	seps[] = " ,\t\n";
    char*   Arg;
    char	Buf[1024];

    _Val.clear();

    if( Str[0] == '\0' || strlen(Str) <= 1)
        return false;

    strcpy( Buf , Str );

    map< string , KeyBaordCmdManageStruct >::iterator Iter;

    char* token = strtok( Buf , seps );

    //轉成小寫
    _strlwr( token );

    Iter = _CmdMap.find( token );

    if( Iter == _CmdMap.end() )
    {
        PrintToController(false,  "\n指令有問題!! %s not find" , token );
        return false;
    }

    _Val.push_back( token );

    KeyBaordCmdManageStruct KBCmd =Iter->second;

    //取數值
    for( i = 0 ; ; i++ )
    {
        Arg = strtok( NULL , seps );
        if( Arg== NULL ) 
            break;

        _Val.push_back( Arg );
    }

    //檢查是否為查尋指令
    if( _Val.size() >= 2 && _Val[1][0] == '?' )
    {
        PrintToController(false,  "\n%s" , (char*)KBCmd.Help.c_str() );
        return true;
    }

    return (this->*KBCmd.Fun)( );

}

//----------------------------------------------------------------------------------------
//Help 列出所有的GM指令
bool	KeyBoardCmd::Help( )
{
    char*	Key = _strlwr( _GetStr( 1 ) );
    int		i;
    char	Buf[256];
    char	Temp[256];


    map< string , KeyBaordCmdManageStruct >::iterator Iter;

    PrintToController(false, "\n------------------ Help ------------------" );
    for( Iter = _CmdMap.begin() ; Iter != _CmdMap.end() ; )
    {
        Buf[0] = 0;
        for( i =0  ; i < 3 && Iter != _CmdMap.end();  Iter++)
        {
            if( Key[0] >= 'a' && Key[0] <= 'z' )
            {
                if( Iter->second.Name.c_str()[1] != Key[0] )
                    continue;
            }
            sprintf(Temp , "%-14s " , Iter->second.Name.c_str() );
            strcat( Buf , Temp );
            i++;
        }
        //Owner->Tell( Buf );
        PrintToController(false,  "\n%s" , Buf );

    }

    return true;
}
//----------------------------------------------------------------------------------------
//處理來自Controller的指令輸入
bool KeyBoardCmd::ControllerInput( )
{
	string CmdStr = "";
	if (!ControllerClient::GetCmd(CmdStr))
		return false;

	return CmdProc( CmdStr.c_str() );
}

//----------------------------------------------------------------------------------------
//處理鍵盤輸入
bool    KeyBoardCmd::KeyBoardInput( )
{
	int     Key;
	bool    Ret;

	// Flush output
	fflush(NULL);

	//試著接收controller的輸入, 一次執行一項指令
	if (ControllerInput())
		return true;

	// Check input
	if( g_remotableInput.InputAvailable() == false )
		return false;

	Key = g_remotableInput.GetChar();

    switch( Key )
    {
	case 10:
	case 13: //Enter
        Ret = CmdProc( _CmdStr.c_str() );
        _CmdStr.clear();
        printf("\n");
        return Ret; 
    case 8:  //BACK SPACE
        if( _CmdStr.size() > 0)
            _CmdStr.erase( _CmdStr.size()-1 , 1 );
        break;
    default:
        _CmdStr += Key;
        break;
    }
    return false;
}
//----------------------------------------------------------------------------------------
//結束程式 
bool    KeyBoardCmd::Exit( )
{
	_GatewayMain->Exit();
	return true;
}
//----------------------------------------------------------------------------------------
//檢查是否還活著
bool    KeyBoardCmd::Ping        ( )
{
	PrintToController(false, "Ping\n" );
	fflush( NULL );
	return true;
}
//----------------------------------------------------------------------------------------
bool	KeyBoardCmd::SrvLoadOK		( )
{
	//_SwitchMain->Switch()->SetUpdateListMode( true );
	PrintToController(false, "\nOK\n" );
	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::NetCore()
{
	/*
	DWORD dwAvgNewEvent		= 0;
	DWORD dwAvgProcEvent	= 0;
	DWORD dwQueueEvent		= 0;

	dwAvgNewEvent	= _SwitchMain->Switch()->GetServer()->GetAvgNewEvent();
	dwAvgProcEvent	= _SwitchMain->Switch()->GetServer()->GetAvgProcEvent();
	dwQueueEvent	= _SwitchMain->Switch()->GetServer()->GetQueueEvent();

	Print( LV2, "NetCore", "Switch Server Netcore: NewEvent = %d (s), ProcEvent = %d (s), QueueEvent = %d", dwAvgNewEvent, dwAvgProcEvent, dwQueueEvent );

	dwAvgNewEvent	= _SwitchMain->Switch()->GetProxy()->GetAvgNewEvent();
	dwAvgProcEvent	= _SwitchMain->Switch()->GetProxy()->GetAvgProcEvent();
	dwQueueEvent	= _SwitchMain->Switch()->GetProxy()->GetQueueEvent();

	Print( LV2, "NetCore", "Switch Proxy Netcore: NewEvent = %d (s), ProcEvent = %d (s), QueueEvent = %d", dwAvgNewEvent, dwAvgProcEvent, dwQueueEvent );

	dwAvgNewEvent	= _SwitchMain->Switch()->GetClient()->GetAvgNewEvent();
	dwAvgProcEvent	= _SwitchMain->Switch()->GetClient()->GetAvgProcEvent();
	dwQueueEvent	= _SwitchMain->Switch()->GetClient()->GetQueueEvent();

	Print( LV2, "NetCore", "Switch Client Netcore: NewEvent = %d (s), ProcEvent = %d (s), QueueEvent = %d", dwAvgNewEvent, dwAvgProcEvent, dwQueueEvent );
*/
	return true;
}
/*
bool KeyBoardCmd::Census()
{
	printf( "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable Login,Enable_Login\n" );
	printf( "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable Login,Disable_Login\n" );

	// ServerList Display Mode
		if( _SwitchMain->Switch()->GetUpdateListMode() == true )
		{
			printf( "Set Custom State {79CC1671-A46D-478a-9497-4933581B5B03},-1,Enable\n" );
			printf( "Disable Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable Login\n" );
			printf( "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable Login\n" );
		}
		else
		{	
			printf( "Set Custom State {79CC1671-A46D-478a-9497-4933581B5B03},0,Disable\n" );
			printf( "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable Login\n" );
			printf( "Disable Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable Login\n" );
		}

	return true;
}

bool	KeyBoardCmd::Enable_Login()
{
//	_SwitchMain->Switch()->SetUpdateListMode( true );
	Census();	// Reflush
	return true;
}

bool	KeyBoardCmd::Disable_Login()
{
//	_SwitchMain->Switch()->SetUpdateListMode( false );
	Census();	// Reflush
	return true;
}
//-------------------------------------------
bool KeyBoardCmd::ServerGroupOkay()
{
//	_SwitchMain->Switch()->SetUpdateListMode( true );
	Census();	// Reflush
	return true;
}
*/