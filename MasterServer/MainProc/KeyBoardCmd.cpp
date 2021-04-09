#pragma once
#include "KeyBoardCmd.h" 
#include <conio.h>
#include "../NetWalker_Member/NetWakerMasterSrvInc.h"

#include "./ClientInfo/ClientInfo.h"
#include "../netwalker_member/net_login/NetSrv_Master_Login_Child.h"
#include "RemotetableInput/RemotableInput.h"

//----------------------------------------------------------------------------------------
//初始化
KeyBoardCmd::KeyBoardCmd( )
{
	g_remotableInput.Initialize();

    KeyBaordCmdManageStruct GMS;

    GMS.Init( "help" , &KeyBoardCmd::Help 
        , "Help 列出全部或某英文字開頭的GM指令\n help a or help " );
    _CmdMap[ GMS.Name ] = GMS;	

    GMS.Init( "Exit" , &KeyBoardCmd::Exit 
        , "結束程式" );
    _CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "MemoryCheck" , &KeyBoardCmd::MemoryCheck 
		, "記憶體檢查" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "MemoryReport" , &KeyBoardCmd::MemoryReport 
		, "記憶體列印" );
	_CmdMap[ GMS.Name ] = GMS;		

	GMS.Init( "NewAccountDisableCreateRole" , &KeyBoardCmd::NewAccountDisableCreateRole 
		, "新手不能創角" );
	_CmdMap[ GMS.Name ] = GMS;		


	GMS.Init( "NETINFO" , &KeyBoardCmd::NetInfo, "網路狀態" );
	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "SetWorldlimit" , &KeyBoardCmd::SetWorldlimit, "SetWorldlimit" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "SetZonelimit" , &KeyBoardCmd::SetZonelimit, "SetZonelimit" );
	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "Setlimit" , &KeyBoardCmd::Setlimit, "Setlimit" );	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "AccOkay" , &KeyBoardCmd::AccOkay, "AccOkay" );
	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "AccFailed" , &KeyBoardCmd::AccFailed, "AccFailed" );
	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "AccStop" , &KeyBoardCmd::AccStop, "AccStop" );
	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "Census" , &KeyBoardCmd::Census, "Census" );	_CmdMap[ GMS.Name ] = GMS;


	GMS.Init( "KickAll" , &KeyBoardCmd::KickAll, "KickAll" );	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "STOP_ALL_SERVER_AFTER_30MIN" ,	&KeyBoardCmd::STOP_ALL_SERVER_AFTER_30MIN,	"STOP_ALL_SERVER_AFTER_30MIN" );		_CmdMap[ GMS.Name ] = GMS;
	GMS.Init( "STOP_ALL_SERVER_AFTER_15MIN" ,	&KeyBoardCmd::STOP_ALL_SERVER_AFTER_15MIN,	"STOP_ALL_SERVER_AFTER_15MIN" );		_CmdMap[ GMS.Name ] = GMS;
	GMS.Init( "STOP_ALL_SERVER_AFTER_5MIN" ,	&KeyBoardCmd::STOP_ALL_SERVER_AFTER_5MIN,	"STOP_ALL_SERVER_AFTER_5MIN" );		_CmdMap[ GMS.Name ] = GMS;
	GMS.Init( "STOP_ALL_SERVER_AFTER_1MIN" ,	&KeyBoardCmd::STOP_ALL_SERVER_AFTER_1MIN,	"STOP_ALL_SERVER_AFTER_1MIN" );		_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "SEND_MSG_TO_ALLPLAYER",	&KeyBoardCmd::SEND_MSG_TO_ALLPLAYER,	"SEND_MSG_TO_ALLPLAYER" );		_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "Enable_WaitQueue" ,	&KeyBoardCmd::Enable_WaitQueue,	"Enable_WaitQueue" );		_CmdMap[ GMS.Name ] = GMS;
	GMS.Init( "Disable_WaitQueue" ,	&KeyBoardCmd::Disable_WaitQueue,	"Disable_WaitQueue" );		_CmdMap[ GMS.Name ] = GMS;


	GMS.Init( "SetMaxObj" ,		&KeyBoardCmd::Disable_WaitQueue,	"Disable_WaitQueue" );		_CmdMap[ GMS.Name ] = GMS;
	GMS.Init( "SetObjWait" ,	&KeyBoardCmd::Disable_WaitQueue,	"Disable_WaitQueue" );		_CmdMap[ GMS.Name ] = GMS;
	GMS.Init( "SetGameGuard" ,	&KeyBoardCmd::Disable_WaitQueue,	"Disable_WaitQueue" );		_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "SetValue" ,		&KeyBoardCmd::SetValue,	"SetValue" );			_CmdMap[ GMS.Name ] = GMS;
	GMS.Init( "VivoxTest" ,		&KeyBoardCmd::VivoxTest,	"VivoxTest" );			_CmdMap[ GMS.Name ] = GMS;



}

//----------------------------------------------------------------------------------------
const char* MyStrToken( char* Buf , char** NextBufPoint )
{
	if( Buf == NULL || NextBufPoint == NULL )
		return NULL;

	static string Str;
	Str.clear();

	bool IsDoubleQuotes = false;	

	for(;;Buf++)
	{	if( *Buf == 0 )	
	return NULL;

	if( *Buf == '\"' )
	{
		Buf++;
		IsDoubleQuotes = true;
		break;
	}
	bool IsFind = false;

	if( *Buf == '\t' || *Buf == ' ' )
		continue;

	//		if( *Buf == ',' )
	//		{
	//			*NextBufPoint = Buf+1;
	//			return "";
	//		}
	break;
	}

	if( IsDoubleQuotes != false )
	{
		for( ;;Buf++ )
		{
			if( *Buf == 0 )
			{
				*NextBufPoint = NULL;
				return Str.c_str();
			}
			if( *Buf == '\"' )
				break;

			Str.push_back( *Buf );

		}
	}

	for( ;;Buf++ )
	{
		if( *Buf == 0 )
		{
			*NextBufPoint = NULL;
			return Str.c_str();
		}
		if( *Buf == ',' )
			break;
		if( IsDoubleQuotes == false && *Buf == ' ')
			break;
		if( IsDoubleQuotes == false )
			Str.push_back( *Buf );
	}	

	*NextBufPoint = Buf+1;
	return Str.c_str();
}
//----------------------------------------------------------------------------------------
bool	KeyBoardCmd::AccStop()
{
	//g_pBTI->PlayerLogout( "arrarr1011" );
	return 0;
}
//----------------------------------------------------------------------------------------
bool	KeyBoardCmd::AccOkay()
{
	//g_pBTI->PlayerLogin( "arrarr1011", "192.168.1.1" );
	return 0;
}
//----------------------------------------------------------------------------------------
bool	KeyBoardCmd::AccFailed()
{
	//g_pBTI->PlayerLogin( "arrarr1011", "192.168.1.1" );
	//g_pBTI->m_mapNumBTIPlayer[1] = 3;
	//g_pBTI->CheckAccount();

	int iDBID	= 0;
	int iHours	= 0;

	int iSrvID	= 0;

	for( map< string, ClientInfo* >::iterator it = g_pClientManger->m_mapAccount.begin(); it != g_pClientManger->m_mapAccount.end(); it++ )
	{
		const char* pAccountSource	= ( it->first ).c_str();
		ClientInfo* pClientInfo		= it->second;

		for( set< int >::iterator itZ = pClientInfo->m_setZone.begin(); itZ != pClientInfo->m_setZone.end(); itZ++ )
		{
			iSrvID = *itZ;
		}




	}


	//CNetSrv_MasterServer::SL_BTI_RolePlayHours( iSrvID, iDBID, iHours );
	return 0;
}
//----------------------------------------------------------------------------------------
bool    KeyBoardCmd::SetWorldlimit( )
{
	_MaxLimitPlayerCount = _GetNum(1);
	return true;
}
//----------------------------------------------------------------------------------------
bool    KeyBoardCmd::SetZonelimit( )
{
	_MaxZonePlayerCount = _GetNum(1);
	return true;
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
    
    char	seps[] = " ,\t\n";
    const	char*   Arg;
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
        //printf( "\n指令有問題!! %s not find" , token );
		PrintToController(false,  "\n指令有問題!! %s not find" , token );
        return false;
    }

    _Val.push_back( token );

    KeyBaordCmdManageStruct KBCmd =Iter->second;


	char* NextBufPos;
	if( strlen(Str) == strlen(Buf) )
	{
		NextBufPos = NULL;
	}
	else
	{
		for( NextBufPos = Buf ; *NextBufPos!= 0 ; NextBufPos++ );
		NextBufPos++;
	}

	for( int i = 0 ; i < 30 ; i++ )
	{
		Arg = MyStrToken( NextBufPos , &NextBufPos );
		if( Arg== NULL ) 
			break;
		_Val.push_back( Arg );
	}


	/*
    //取數值
    for( i = 0 ; ; i++ )
    {
        Arg = strtok( NULL , seps );
        if( Arg== NULL ) 
            break;

        _Val.push_back( Arg );
    }
	*/

    //檢查是否為查尋指令
    if( _Val.size() >= 2 && _Val[1][0] == '?' )
    {
        //printf( "\n%s" , (char*)KBCmd.Help.c_str() );
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
//結束程式 
bool    KeyBoardCmd::Exit( )
{
    Global::Destory();
    return true;
}
//----------------------------------------------------------------------------------------
bool	KeyBoardCmd::NetInfo		( )
{
	//Global:Net
	return true;
}
//----------------------------------------------------------------------------------------
//檢查是否還活著
bool    KeyBoardCmd::Ping        ( )
{
	//printf( "Ping\n" );
	PrintToController(false,  "Ping\n" );
	fflush( NULL );
	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::Census()
{
	unsigned int	NumPlayer		= (unsigned)g_pClientManger->m_mapDBID.size();
	unsigned int	NumActPlayer	= g_pClientManger->GetActive();
	unsigned int	WaitPlayer		= (unsigned)CNetSrv_MasterServer_Child::m_vecWaitQueue.size();
	unsigned int	PlayerLimit		= _MaxLimitPlayerCount;
	unsigned int	LoadingPlayer	= (unsigned)CNetSrv_MasterServer_Child::m_mapLoadQueue.size();

	//printf( "World Census {8B1B5C78-AB81-4c0f-B0EF-A380269E44C5},%d", NumPlayer );// (%d/%d)\n", NumPlayer, NumActPlayer,   );
	//printf( "NumActPlayer[ %d ]/[ %d ], EnableLimit = %d", NumActPlayer, PlayerLimit, _EnableLimit );	fflush( NULL );

	PrintToController(false,  "World Census {8B1B5C78-AB81-4c0f-B0EF-A380269E44C5},%d", ( NumActPlayer +  WaitPlayer ) );// (%d/%d)\n", NumPlayer, NumActPlayer,   );
	PrintToController(false,  "WaitCount {7E52C067-80A5-42AA-AA4D-6D25469B3C00},W[%d],L[%d]", WaitPlayer,LoadingPlayer );
	//PrintToController(false,  "Act[ %d ] Wait[ %d ] Limit[ %d ], EnableLimit = %d", NumActPlayer, WaitPlayer, _MaxLimitPlayerCount, _EnableLimit );	fflush( NULL );

	RemoteCommand();

	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::KickAll()
{

	map< int	, ClientInfo* >::iterator it;

	for( it = g_pClientManger->m_mapClientInfo.begin(); it != g_pClientManger->m_mapClientInfo.end(); it++ )
	{
		int				iClientID	= it->first;
		ClientInfo*		pClientInfo	= it->second;

		_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), "Console Command KickAll" );
	}

	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::STOP_ALL_SERVER_AFTER_30MIN()
{
	CNetMaster_ZoneStatus_Child::m_pThis->m_iShutdownMin	= 30; //15 * 60 * 1000; 停機 15min 倒數
	Schedular()->Push( CNetMaster_ZoneStatus_Child::OnEvent_StopAllServer, CNetMaster_ZoneStatus_Child::m_pThis->m_iShutdownMin, CNetMaster_ZoneStatus_Child::m_pThis, NULL );
	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::STOP_ALL_SERVER_AFTER_15MIN()
{
	CNetMaster_ZoneStatus_Child::m_pThis->m_iShutdownMin	= 15; //15 * 60 * 1000; 停機 15min 倒數
	Schedular()->Push( CNetMaster_ZoneStatus_Child::OnEvent_StopAllServer, CNetMaster_ZoneStatus_Child::m_pThis->m_iShutdownMin, CNetMaster_ZoneStatus_Child::m_pThis, NULL );
	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::STOP_ALL_SERVER_AFTER_5MIN()
{
	CNetMaster_ZoneStatus_Child::m_pThis->m_iShutdownMin	= 5; //15 * 60 * 1000; 停機 15min 倒數
	Schedular()->Push( CNetMaster_ZoneStatus_Child::OnEvent_StopAllServer, CNetMaster_ZoneStatus_Child::m_pThis->m_iShutdownMin, CNetMaster_ZoneStatus_Child::m_pThis, NULL );
	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::STOP_ALL_SERVER_AFTER_1MIN()
{
	CNetMaster_ZoneStatus_Child::m_pThis->m_iShutdownMin	= 1; //15 * 60 * 1000; 停機 15min 倒數
	Schedular()->Push( CNetMaster_ZoneStatus_Child::OnEvent_StopAllServer, CNetMaster_ZoneStatus_Child::m_pThis->m_iShutdownMin, CNetMaster_ZoneStatus_Child::m_pThis, NULL );
	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::SEND_MSG_TO_ALLPLAYER()
{
	//CNetMaster_ZoneStatus_Child::m_pThis->m_iShutdownMin	= 1; //15 * 60 * 1000; 停機 15min 倒數
	//Schedular()->Push( CNetMaster_ZoneStatus_Child::OnEvent_StopAllServer, CNetMaster_ZoneStatus_Child::m_pThis->m_iShutdownMin, CNetMaster_ZoneStatus_Child::m_pThis, NULL );
	
	//CharTransferClass	StringObj;

	const char* pMsg = _GetStr(1);

	//StringObj.SetString( pMsg );

	CNetMaster_ZoneStatus_Child::SendMsgToAllPlayer( pMsg );
	return true;
}
//----------------------------------------------------------------------------------------
void KeyBoardCmd::RemoteCommand()
{
	// Add command
	//printf( "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},STOP_ALL_SERVER_AFTER_30MIN,STOP_ALL_SERVER_AFTER_30MIN\n" );
	//printf( "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},STOP_ALL_SERVER_AFTER_15MIN,STOP_ALL_SERVER_AFTER_15MIN\n" );
	//printf( "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},STOP_ALL_SERVER_AFTER_5MIN,STOP_ALL_SERVER_AFTER_5MIN\n" );
	//printf( "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},STOP_ALL_SERVER_AFTER_1MIN,STOP_ALL_SERVER_AFTER_1MIN\n" );
	//printf( "Add Input Command {79CC1671-A46D-478a-9497-4933581B5B03},SEND_MSG_TO_ALLPLAYER,SEND_MSG_TO_ALLPLAYER\n" );

	PrintToController(false,  "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},STOP_ALL_SERVER_AFTER_30MIN,STOP_ALL_SERVER_AFTER_30MIN\n" );
	PrintToController(false,  "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},STOP_ALL_SERVER_AFTER_15MIN,STOP_ALL_SERVER_AFTER_15MIN\n" );
	PrintToController(false,  "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},STOP_ALL_SERVER_AFTER_5MIN,STOP_ALL_SERVER_AFTER_5MIN\n" );
	PrintToController(false,  "Add Command {79CC1671-A46D-478a-9497-4933581B5B03},STOP_ALL_SERVER_AFTER_1MIN,STOP_ALL_SERVER_AFTER_1MIN\n" );
	PrintToController(false,  "Add Input Command {79CC1671-A46D-478a-9497-4933581B5B03},SEND_MSG_TO_ALLPLAYER,SEND_MSG_TO_ALLPLAYER\n" );


	PrintToController( false,"Add Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable WaitQueue,Enable_WaitQueue\n" );
	PrintToController( false,"Add Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable WaitQueue,Disable_WaitQueue\n" );

	// ServerList Display Mode
	if( _EnableLimit == 1 )
	{
		PrintToController( false,"Disable Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable WaitQueue\n" );
		PrintToController( false,"Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable WaitQueue\n" );
	}
	else
	{	
		PrintToController( false,"Enable Command {79CC1671-A46D-478a-9497-4933581B5B03},Enable WaitQueue\n" );
		PrintToController( false,"Disable Command {79CC1671-A46D-478a-9497-4933581B5B03},Disable WaitQueue\n" );
	}




}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::Setlimit()
{
	_EnableLimit = _GetNum(1);
	//printf( "_EnableLimit = %d" , _EnableLimit );
	PrintToController(false,  "_EnableLimit = %d" , _EnableLimit );
	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::Enable_WaitQueue()
{
	_EnableLimit = 1;
	RemoteCommand();
	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::Disable_WaitQueue()
{
	_EnableLimit = 0;
	RemoteCommand();
	return true;
}
//----------------------------------------------------------------------------------------
//記憶體檢查
bool    KeyBoardCmd::MemoryCheck ( )
{
	MyAllocMemoryCheck( );
	return true;
}
//----------------------------------------------------------------------------------------
//記憶體列印
bool	KeyBoardCmd::MemoryReport( )
{
	MyAllocMemoryReport( );
	return true;
}
//----------------------------------------------------------------------------------------
bool	KeyBoardCmd::NewAccountDisableCreateRole	()
{
	int Value = _GetNum(1);

	Global::_NewAccountDisableCreateRole = (Value!=0);
	return true;
}
//----------------------------------------------------------------------------------------
bool	KeyBoardCmd::SetValue()
{	
	const char* pszCommand	= _GetStr(1);
	int			Value		= _GetNum(2);

	if( stricmp( pszCommand, "EnableObjWait") == 0 ) 
	{
		Global::_EnableObjWait		= Value;
	}
	else 
	if( stricmp( pszCommand, "MaxObjCount") == 0 ) 
	{
		Global::_MaxObjCount		= Value;
	}
	else
	if( stricmp( pszCommand, "EnableGameGuard") == 0 ) 
	{
		Global::_EnableGameGuard	= Value;
	}

	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::VivoxTest()
{
	//CNetSrv_MasterServer::m_pThis->m_vivox.Login( "romtw01", "test" );
	return true;
}
