#pragma once
#include "KeyBoardCmd.h" 
#include <conio.h>
#include "RemotetableInput/RemotableInput.h"
//----------------------------------------------------------------------------------------
//��l��
KeyBoardCmd::KeyBoardCmd( SwitchMainClass* S )
{
	g_remotableInput.Initialize();

	_SwitchMain= S;

    KeyBaordCmdManageStruct GMS;

    GMS.Init( "help" , &KeyBoardCmd::Help, "Help �C�X�����άY�^��r�}�Y��GM���O\n help a or help " );
    _CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "Exit" , &KeyBoardCmd::Exit, "�����{��" );
	_CmdMap[ GMS.Name ] = GMS;	
	
	GMS.Init( "Ping" , &KeyBoardCmd::Ping, "�ˬd�O�_�٬���" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "SrvLoadOK" , &KeyBoardCmd::SrvLoadOK, "Server�H�g���}�_�F" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "NetCore" , &KeyBoardCmd::NetCore, "Display netcore number of event" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "SetRedirectHost" , &KeyBoardCmd::SetRedirectHost, "�]�wRedirectHost Country Host Port" );
	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "ServerStatus" , &KeyBoardCmd::ServerStatus, "�]�wServerStatus N (N=0 �����`, N=1 ������, N=2 ������)" );
	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "Census" ,			&KeyBoardCmd::Census,				"Census" );				_CmdMap[ GMS.Name ] = GMS;
	GMS.Init( "ServerGroupOkay" ,	&KeyBoardCmd::ServerGroupOkay,		"ServerGroupOkay" );	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "Enable_Login" ,		&KeyBoardCmd::Enable_Login,			"Enable_Login" );		_CmdMap[ GMS.Name ] = GMS;
	GMS.Init( "Disable_Login" ,		&KeyBoardCmd::Disable_Login,		"Disable_Login" );		_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "EnableCheckClientNoRespond" ,		&KeyBoardCmd::Enable_ClientCheck_NoRespond,				"EnableCheckClientNoRespond" );			_CmdMap[ GMS.Name ] = GMS;
	GMS.Init( "DisableCheckClientNoRespond",		&KeyBoardCmd::Disable_ClientCheck_NoRespond,			"DisableCheckClientNoRespond" );		_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "EnableCheckSendBuffLimit",			&KeyBoardCmd::Enable_ClientCheck_SendBuffLimit,			"EnableCheckSendBuffLimit" );			_CmdMap[ GMS.Name ] = GMS;
	GMS.Init( "DisableCheckSendBuffLimit",			&KeyBoardCmd::Disable_ClientCheck_SendBuffLimit,		"DisableCheckSendBuffLimit" );			_CmdMap[ GMS.Name ] = GMS;
	
	GMS.Init( "SetSwitchStatus",					&KeyBoardCmd::Disable_ClientCheck_SendBuffLimit,		"DisableCheckSendBuffLimit" );			_CmdMap[ GMS.Name ] = GMS;

	

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
//GM�R�O�B�z
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

    //�ন�p�g
    _strlwr( token );

    Iter = _CmdMap.find( token );

    if( Iter == _CmdMap.end() )
    {
        //printf( "\n���O�����D!! %s not find" , token );
		PrintToController(false,  "\n���O�����D!! %s not find" , token );
        return false;
    }

    _Val.push_back( token );

    KeyBaordCmdManageStruct KBCmd =Iter->second;

    //���ƭ�
    for( i = 0 ; ; i++ )
    {
        Arg = strtok( NULL , seps );
        if( Arg== NULL ) 
            break;

        _Val.push_back( Arg );
    }

    //�ˬd�O�_���d�M���O
    if( _Val.size() >= 2 && _Val[1][0] == '?' )
    {
        //printf( "\n%s" , (char*)KBCmd.Help.c_str() );
		PrintToController(false,  "\n%s" , (char*)KBCmd.Help.c_str() );
        return true;
    }

    return (this->*KBCmd.Fun)( );

}

//----------------------------------------------------------------------------------------
//Help �C�X�Ҧ���GM���O
bool	KeyBoardCmd::Help( )
{
    char*	Key = _strlwr( _GetStr( 1 ) );
    int		i;
    char	Buf[256];
    char	Temp[256];


    map< string , KeyBaordCmdManageStruct >::iterator Iter;

    //printf( "\n------------------ Help ------------------" );
	PrintToController(false,  "\n------------------ Help ------------------" );
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
        //printf( "\n%s" , Buf );
		PrintToController(false,  "\n%s" , Buf );

    }

    return true;
}

//----------------------------------------------------------------------------------------
//�B�z�Ӧ�Controller�����O��J
bool KeyBoardCmd::ControllerInput( )
{
	string CmdStr = "";
	if (!ControllerClient::GetCmd(CmdStr))
		return false;

	return CmdProc( CmdStr.c_str() );
}

//----------------------------------------------------------------------------------------
//�B�z��L��J
bool    KeyBoardCmd::KeyBoardInput( )
{
	int     Key;
	bool    Ret;

	// Flush output
	fflush(NULL);

	//�յ۱���controller����J, �@������@�����O
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
        Ret = CmdProc( CmdStr.c_str() );
        CmdStr.clear();
        printf("\n");
        return Ret; 
    case 8:  //BACK SPACE
        if( CmdStr.size() > 0)
            CmdStr.erase( CmdStr.size()-1 , 1 );
        break;
    default:
        CmdStr += Key;
        break;
    }
    return false;
}
//----------------------------------------------------------------------------------------
//�����{�� 
bool    KeyBoardCmd::Exit( )
{
	_SwitchMain->Exit();
	return true;
}
//----------------------------------------------------------------------------------------
//�ˬd�O�_�٬���
bool    KeyBoardCmd::Ping        ( )
{
	//printf( "Ping\n" );
	PrintToController(false,  "Ping\n" );
	fflush( NULL );
	return true;
}
//----------------------------------------------------------------------------------------
bool	KeyBoardCmd::SrvLoadOK		( )
{
	_SwitchMain->Switch()->SetUpdateListMode( true );
	//printf( "\nOK\n" );
	PrintToController(false,  "\nOK\n" );
	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::NetCore()
{
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

	return true;
}

bool KeyBoardCmd::Census()
{
	//printf( "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable Login,Enable_Login\n" );
	//printf( "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable Login,Disable_Login\n" );

	PrintToController(false,  "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},Online,ServerStatus 0\n" );
	PrintToController(false,  "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},Offline,ServerStatus 1\n" );
	PrintToController(false,  "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},Hide,ServerStatus 2\n" );

	PrintToController(false,  "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable CheckClientNoRespond,EnableCheckClientNoRespond\n" );
	PrintToController(false,  "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable CheckClientNoRespond,DisableCheckClientNoRespond\n" );

	PrintToController(false,  "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable CheckClientSendBuffLimit,EnableCheckSendBuffLimit\n" );
	PrintToController(false,  "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable CheckClientSendBuffLimit,DisableCheckSendBuffLimit\n" );


	// ServerList Display Mode
		//if( _SwitchMain->Switch()->GetUpdateListMode() == true )
		if( _SwitchMain->Switch()->GetSwitchStatus() == 0 )
		{
			PrintToController(false,  "Set Custom State {79CC1671-A46D-478a-9497-4933581B5B03},-1,Enable\n" );
			
			PrintToController(false,  "Disable Command {79CC1671-A46D-478a-9497-4933581B5B03},Online\n" );
			PrintToController(false,  "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Offline\n" );
			PrintToController(false,  "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Hide\n" );
		}
		else
		if( _SwitchMain->Switch()->GetSwitchStatus() == 1 )
		{	
			//printf( "Set Custom State {79CC1671-A46D-478a-9497-4933581B5B03},0,Disable\n" );
			//printf( "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable Login\n" );
			//printf( "Disable Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable Login\n" );

			PrintToController(false,  "Set Custom State {79CC1671-A46D-478a-9497-4933581B5B03},0,Disable\n" );
			
			PrintToController(false,  "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Online\n" );
			PrintToController(false,  "Disable Command {79CC1671-A46D-478a-9497-4933581B5B03},Offline\n" );
			PrintToController(false,  "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Hide\n" );
		}
		else
		if( _SwitchMain->Switch()->GetSwitchStatus() == 2 )
		{	
			//printf( "Set Custom State {79CC1671-A46D-478a-9497-4933581B5B03},0,Disable\n" );
			//printf( "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable Login\n" );
			//printf( "Disable Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable Login\n" );

			PrintToController(false,  "Set Custom State {79CC1671-A46D-478a-9497-4933581B5B03},0,Disable\n" );
			
			PrintToController(false,  "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Online\n" );
			PrintToController(false,  "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Offline\n" );
			PrintToController(false,  "Disable Command {79CC1671-A46D-478a-9497-4933581B5B03},Hide\n" );
		}


	// Client Check No Respond
		if( _SwitchMain->Switch()->m_bCheck_Client_NoRespond == true )
		{
			PrintToController(false,  "Disable Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable CheckClientNoRespond\n" );
			PrintToController(false,  "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable CheckClientNoRespond\n" );
		}
		else
		{	
			PrintToController(false,  "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable CheckClientNoRespond\n" );
			PrintToController(false,  "Disable Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable CheckClientNoRespond\n" );
		}

	// Client Check CheckSendBuffLimit
		if( _SwitchMain->Switch()->m_bCheck_Client_SendBuffLimit == true )
		{
			PrintToController(false,  "Disable Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable CheckClientSendBuffLimit\n" );
			PrintToController(false,  "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable CheckClientSendBuffLimit\n" );
		}
		else
		{	
			PrintToController(false,  "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable CheckClientSendBuffLimit\n" );
			PrintToController(false,  "Disable Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable CheckClientSendBuffLimit\n" );
		}

	return true;
}

bool	KeyBoardCmd::Enable_Login()
{
	_SwitchMain->Switch()->SetUpdateListMode( true );
	Census();	// Reflush
	return true;
}

bool	KeyBoardCmd::Disable_Login()
{
	_SwitchMain->Switch()->SetUpdateListMode( false );
	Census();	// Reflush
	return true;
}
//-------------------------------------------
bool KeyBoardCmd::ServerGroupOkay()
{
	_SwitchMain->Switch()->SetUpdateListMode( true );
	Census();	// Reflush
	return true;
}

bool	KeyBoardCmd::Enable_ClientCheck_NoRespond()
{
	_SwitchMain->Switch()->Set_Client_NoRespond( true );
	Census();	// Reflush
	return true;
}

bool	KeyBoardCmd::Disable_ClientCheck_NoRespond()
{
	_SwitchMain->Switch()->Set_Client_NoRespond( false );
	Census();	// Reflush
	return true;
}

bool	KeyBoardCmd::Enable_ClientCheck_SendBuffLimit()
{
	_SwitchMain->Switch()->Set_Client_SendBuffLimit( true, 204800 );
	Census();	// Reflush
	return true;
}

bool	KeyBoardCmd::Disable_ClientCheck_SendBuffLimit()
{
	_SwitchMain->Switch()->Set_Client_SendBuffLimit( false, 204800 );
	Census();	// Reflush
	return true;
}


bool	KeyBoardCmd::Enable_Server_CheckNoRespond()
{
	_SwitchMain->Switch()->m_bCheck_Client_NoRespond = true;
	Census();	// Reflush
	return true;
}

bool	KeyBoardCmd::Disable_Server_CheckNoRespond()
{
	_SwitchMain->Switch()->m_bCheck_Server_NoRespond = false;
	Census();	// Reflush
	return true;
}

//ServerStatus 0 �����`, 1 ������, 2 ������
bool	KeyBoardCmd::ServerStatus( )
{
	_SwitchMain->Switch()->_ServerStatus = _GetNum( 1 );
	return true;
}

bool	KeyBoardCmd::Enable_Server_CheckSendBuffLimit()
{
	_SwitchMain->Switch()->m_bCheck_Server_SendBuffLimit = true;
	Census();	// Reflush
	return true;
}

bool	KeyBoardCmd::Disable_Server_CheckSendBuffLimit()
{
	_SwitchMain->Switch()->m_bCheck_Server_SendBuffLimit = false;
	Census();	// Reflush
	return true;
}

bool	KeyBoardCmd::SetRedirectHost( )
{
	string Country  = _GetStr( 1 );
	string Host		= _GetStr( 2 );
	int	   Port		= _GetNum( 3 );

	_SwitchMain->Switch()->SetRedirectHost( Country , Host , Port );


	return true;
}