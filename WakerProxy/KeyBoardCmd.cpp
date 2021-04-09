#pragma once
#include "KeyBoardCmd.h" 
#include <conio.h>
#include "RemotetableInput/RemotableInput.h"
//----------------------------------------------------------------------------------------
//��l��
KeyBoardCmd::KeyBoardCmd( ProxyMainClass* S )
{
	g_remotableInput.Initialize();
	_ProcObj= S;

    KeyBaordCmdManageStruct GMS;

    GMS.Init( "help" , &KeyBoardCmd::Help 
        , "Help �C�X�����άY�^��r�}�Y��GM���O\n help a or help " );
    _CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "Exit" , &KeyBoardCmd::Exit 
		, "�����{��" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "SockInfo" , &KeyBoardCmd::SockInfo
		, "�������e���" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "Ping" , &KeyBoardCmd::Ping 
		, "�ˬd�O�_�٬���" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "NetCore" , &KeyBoardCmd::NetCore, "Display netcore number of event" );	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "Set" , &KeyBoardCmd::SetValue, "Set KEY VAL" );	_CmdMap[ GMS.Name ] = GMS;	
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
	_ProcObj->Exit();
	return true;
}
//----------------------------------------------------------------------------------------
bool	KeyBoardCmd::SockInfo( )
{	
	
	CNetBridge*	    NetBSwitch = _ProcObj->Proxy()->NetBSwitch();
	CNetBridge*	    NetBGSrv   = _ProcObj->Proxy()->NetBGSrv();
    CNetBridge*	    NetBCli    = _ProcObj->Proxy()->NetBCli();

	/*
	printf("\n ==============================Switch==================================== " );	
	printf( "\n MsgCount = %d " , NetBSwitch->GetMsgCount() );

	printf( "\n����� �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBSwitch->GetBytesRecv( 1 ) 
		, NetBSwitch->GetBytesRecv( 60 )	/ 60
		, NetBSwitch->GetBytesRecv( 600 )	/ 600 
		, NetBSwitch->GetBytesRecv( 60*30 ) / 1800 
		, NetBSwitch->GetBytesRecv( 60*60 ) / 3600 );
	printf( "\n�e��� �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBSwitch->GetBytesSend( 1 ) 
		, NetBSwitch->GetBytesSend( 60 )	/ 60
		, NetBSwitch->GetBytesSend( 600 )	/ 600 
		, NetBSwitch->GetBytesSend( 60*30 ) / 1800 
		, NetBSwitch->GetBytesSend( 60*60 ) / 3600 );

	printf( "\n���ʥ] �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBSwitch->GetPacketsRecv( 1 ) 
		, NetBSwitch->GetPacketsRecv( 60 )	/ 60
		, NetBSwitch->GetPacketsRecv( 600 )	/ 600 
		, NetBSwitch->GetPacketsRecv( 60*30 ) / 1800 
		, NetBSwitch->GetPacketsRecv( 60*60 ) / 3600 );
	printf( "\n�e�ʥ] �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBSwitch->GetPacketsSend( 1 ) 
		, NetBSwitch->GetPacketsSend( 60 )	/ 60
		, NetBSwitch->GetPacketsSend( 600 )	/ 600 
		, NetBSwitch->GetPacketsSend( 60*30 ) / 1800 
		, NetBSwitch->GetPacketsSend( 60*60 ) / 3600 );

	printf("\n ==============================Game Server==================================== " );
	printf( "\n MsgCount = %d " , NetBGSrv->GetMsgCount() );

	printf( "\n����� �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBGSrv->GetBytesRecv( 1 ) 
		, NetBGSrv->GetBytesRecv( 60 )		/ 60
		, NetBGSrv->GetBytesRecv( 600 )		/ 600 
		, NetBGSrv->GetBytesRecv( 60*30 )	/ 1800 
		, NetBGSrv->GetBytesRecv( 60*60 )   / 3600 );
	printf( "\n�e��� �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBGSrv->GetBytesSend( 1 ) 
		, NetBGSrv->GetBytesSend( 60 )		/ 60
		, NetBGSrv->GetBytesSend( 600 )		/ 600 
		, NetBGSrv->GetBytesSend( 60*30 )	/ 1800 
		, NetBGSrv->GetBytesSend( 60*60 )   / 3600 );

	printf( "\n���ʥ] �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBGSrv->GetPacketsRecv( 1 ) 
		, NetBGSrv->GetPacketsRecv( 60 )	/ 60
		, NetBGSrv->GetPacketsRecv( 600 )	/ 600 
		, NetBGSrv->GetPacketsRecv( 60*30 ) / 1800 
		, NetBGSrv->GetPacketsRecv( 60*60 ) / 3600 );
	printf( "\n�e�ʥ] �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBGSrv->GetPacketsSend( 1 ) 
		, NetBGSrv->GetPacketsSend( 60 )	/ 60
		, NetBGSrv->GetPacketsSend( 600 )	/ 600 
		, NetBGSrv->GetPacketsSend( 60*30 ) / 1800 
		, NetBGSrv->GetPacketsSend( 60*60 ) / 3600 );

	printf("\n ============================== Client  ==================================== " );
	printf( "\n MsgCount = %d " , NetBCli->GetMsgCount() );

	printf( "\n����� �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBCli->GetBytesRecv( 1 ) 
		, NetBCli->GetBytesRecv( 60 )		/ 60
		, NetBCli->GetBytesRecv( 600 )		/ 600 
		, NetBCli->GetBytesRecv( 60*30 )	/ 1800 
		, NetBCli->GetBytesRecv( 60*60 )    / 3600 );
	printf( "\n�e��� �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBCli->GetBytesSend( 1 ) 
		, NetBCli->GetBytesSend( 60 )		/ 60
		, NetBCli->GetBytesSend( 600 )		/ 600 
		, NetBCli->GetBytesSend( 60*30 )	/ 1800 
		, NetBCli->GetBytesSend( 60*60 )    / 3600 );


	printf( "\n���ʥ] �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBCli->GetPacketsRecv( 1 ) 
		, NetBCli->GetPacketsRecv( 60 )	/ 60
		, NetBCli->GetPacketsRecv( 600 )	/ 600 
		, NetBCli->GetPacketsRecv( 60*30 ) / 1800 
		, NetBCli->GetPacketsRecv( 60*60 ) / 3600 );
	printf( "\n�e�ʥ] �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBCli->GetPacketsSend( 1 ) 
		, NetBCli->GetPacketsSend( 60 )	/ 60
		, NetBCli->GetPacketsSend( 600 )	/ 600 
		, NetBCli->GetPacketsSend( 60*30 ) / 1800 
		, NetBCli->GetPacketsSend( 60*60 ) / 3600 );
	*/

	PrintToController(false, "\n ==============================Switch==================================== " );	
	PrintToController(false,  "\n MsgCount = %d " , NetBSwitch->GetMsgCount() );

	PrintToController(false,  "\n����� �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBSwitch->GetBytesRecv( 1 ) 
		, NetBSwitch->GetBytesRecv( 60 )	/ 60
		, NetBSwitch->GetBytesRecv( 600 )	/ 600 
		, NetBSwitch->GetBytesRecv( 60*30 ) / 1800 
		, NetBSwitch->GetBytesRecv( 60*60 ) / 3600 );
	PrintToController(false,  "\n�e��� �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBSwitch->GetBytesSend( 1 ) 
		, NetBSwitch->GetBytesSend( 60 )	/ 60
		, NetBSwitch->GetBytesSend( 600 )	/ 600 
		, NetBSwitch->GetBytesSend( 60*30 ) / 1800 
		, NetBSwitch->GetBytesSend( 60*60 ) / 3600 );

	PrintToController(false,  "\n���ʥ] �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBSwitch->GetPacketsRecv( 1 ) 
		, NetBSwitch->GetPacketsRecv( 60 )	/ 60
		, NetBSwitch->GetPacketsRecv( 600 )	/ 600 
		, NetBSwitch->GetPacketsRecv( 60*30 ) / 1800 
		, NetBSwitch->GetPacketsRecv( 60*60 ) / 3600 );
	PrintToController(false,  "\n�e�ʥ] �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBSwitch->GetPacketsSend( 1 ) 
		, NetBSwitch->GetPacketsSend( 60 )	/ 60
		, NetBSwitch->GetPacketsSend( 600 )	/ 600 
		, NetBSwitch->GetPacketsSend( 60*30 ) / 1800 
		, NetBSwitch->GetPacketsSend( 60*60 ) / 3600 );

	PrintToController(false, "\n ==============================Game Server==================================== " );
	PrintToController(false,  "\n MsgCount = %d " , NetBGSrv->GetMsgCount() );

	PrintToController(false,  "\n����� �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBGSrv->GetBytesRecv( 1 ) 
		, NetBGSrv->GetBytesRecv( 60 )		/ 60
		, NetBGSrv->GetBytesRecv( 600 )		/ 600 
		, NetBGSrv->GetBytesRecv( 60*30 )	/ 1800 
		, NetBGSrv->GetBytesRecv( 60*60 )   / 3600 );
	PrintToController(false,  "\n�e��� �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBGSrv->GetBytesSend( 1 ) 
		, NetBGSrv->GetBytesSend( 60 )		/ 60
		, NetBGSrv->GetBytesSend( 600 )		/ 600 
		, NetBGSrv->GetBytesSend( 60*30 )	/ 1800 
		, NetBGSrv->GetBytesSend( 60*60 )   / 3600 );

	PrintToController(false,  "\n���ʥ] �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBGSrv->GetPacketsRecv( 1 ) 
		, NetBGSrv->GetPacketsRecv( 60 )	/ 60
		, NetBGSrv->GetPacketsRecv( 600 )	/ 600 
		, NetBGSrv->GetPacketsRecv( 60*30 ) / 1800 
		, NetBGSrv->GetPacketsRecv( 60*60 ) / 3600 );
	PrintToController(false,  "\n�e�ʥ] �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBGSrv->GetPacketsSend( 1 ) 
		, NetBGSrv->GetPacketsSend( 60 )	/ 60
		, NetBGSrv->GetPacketsSend( 600 )	/ 600 
		, NetBGSrv->GetPacketsSend( 60*30 ) / 1800 
		, NetBGSrv->GetPacketsSend( 60*60 ) / 3600 );

	PrintToController(false, "\n ============================== Client  ==================================== " );
	PrintToController(false,  "\n MsgCount = %d " , NetBCli->GetMsgCount() );

	PrintToController(false,  "\n����� �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBCli->GetBytesRecv( 1 ) 
		, NetBCli->GetBytesRecv( 60 )		/ 60
		, NetBCli->GetBytesRecv( 600 )		/ 600 
		, NetBCli->GetBytesRecv( 60*30 )	/ 1800 
		, NetBCli->GetBytesRecv( 60*60 )    / 3600 );
	PrintToController(false,  "\n�e��� �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBCli->GetBytesSend( 1 ) 
		, NetBCli->GetBytesSend( 60 )		/ 60
		, NetBCli->GetBytesSend( 600 )		/ 600 
		, NetBCli->GetBytesSend( 60*30 )	/ 1800 
		, NetBCli->GetBytesSend( 60*60 )    / 3600 );


	PrintToController(false,  "\n���ʥ] �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBCli->GetPacketsRecv( 1 ) 
		, NetBCli->GetPacketsRecv( 60 )	/ 60
		, NetBCli->GetPacketsRecv( 600 )	/ 600 
		, NetBCli->GetPacketsRecv( 60*30 ) / 1800 
		, NetBCli->GetPacketsRecv( 60*60 ) / 3600 );
	PrintToController(false,  "\n�e�ʥ] �� = %d  1���� = %d  10���� = %d 30���� = %d �p�� = %d " 
		, NetBCli->GetPacketsSend( 1 ) 
		, NetBCli->GetPacketsSend( 60 )	/ 60
		, NetBCli->GetPacketsSend( 600 )	/ 600 
		, NetBCli->GetPacketsSend( 60*30 ) / 1800 
		, NetBCli->GetPacketsSend( 60*60 ) / 3600 );
	return true;
}
///
///----------------------------------------------------------------------------------------
//�ˬd�O�_�٬���
bool    KeyBoardCmd::Ping        ( )
{
	//printf( "Ping\n" );
	PrintToController(false,  "Ping\n" );
	fflush( NULL );
	return true;
}
//----------------------------------------------------------------------------------------
bool	KeyBoardCmd::NetCore()
{
	CNetBridge*	    NetBSwitch = _ProcObj->Proxy()->NetBSwitch();
	CNetBridge*	    NetBGSrv   = _ProcObj->Proxy()->NetBGSrv();
	CNetBridge*	    NetBCli    = _ProcObj->Proxy()->NetBCli();

	DWORD dwAvgNewEvent		= 0;
	DWORD dwAvgProcEvent	= 0;
	DWORD dwQueueEvent		= 0;

	dwAvgNewEvent	= NetBGSrv->GetAvgNewEvent();
	dwAvgProcEvent	= NetBGSrv->GetAvgProcEvent();
	dwQueueEvent	= NetBGSrv->GetQueueEvent();

	Print( LV2, "NetCore", "Proxy Server Netcore: NewEvent = %d (s), ProcEvent = %d (s), QueueEvent = %d", dwAvgNewEvent, dwAvgProcEvent, dwQueueEvent );

	dwAvgNewEvent	= NetBSwitch->GetAvgNewEvent();
	dwAvgProcEvent	= NetBSwitch->GetAvgProcEvent();
	dwQueueEvent	= NetBSwitch->GetQueueEvent();

	Print( LV2, "NetCore", "Switch Switch Netcore: NewEvent = %d (s), ProcEvent = %d (s), QueueEvent = %d", dwAvgNewEvent, dwAvgProcEvent, dwQueueEvent );

	dwAvgNewEvent	= NetBCli->GetAvgNewEvent();
	dwAvgProcEvent	= NetBCli->GetAvgProcEvent();
	dwQueueEvent	= NetBCli->GetQueueEvent();

	Print( LV2, "NetCore", "Switch Client Netcore: NewEvent = %d (s), ProcEvent = %d (s), QueueEvent = %d", dwAvgNewEvent, dwAvgProcEvent, dwQueueEvent );

	return true;
}

//----------------------------------------------------------------------------------------
bool	KeyBoardCmd::SetValue()
{	
	const char* pszCommand	= _GetStr(1);
	int			Value		= _GetNum(2);

	if( stricmp( pszCommand, "PacketDelay") == 0 ) 
	{
		_ProcObj->Proxy()->_PacketDelay = Value;
	}

	return true;
}