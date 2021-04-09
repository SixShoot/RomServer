#pragma once
#include "KeyBoardCmd.h" 
#include <conio.h>
#include "../NetWalker_Member/NetWakerGSrvInc.h"
#include "RemotetableInput/RemotableInput.h"
#include "GMCommand.h"


//----------------------------------------------------------------------------------------
//初始化
KeyBoardCmd::KeyBoardCmd( )
{
	g_remotableInput.Initialize();

    KeyBaordCmdManageStruct GMS;

	GMS.Init( "help" , &KeyBoardCmd::Help 
        , "Help 列出全部或某英文字開頭的GM指令\n help a or help " );
    _CmdMap[ GMS.Name ] = GMS;	

    GMS.Init( "sysinfo" , &KeyBoardCmd::SysInfo 
        , "系統資訊" );
    _CmdMap[ GMS.Name ] = GMS;	

    GMS.Init( "ObjInfo" , &KeyBoardCmd::ObjInfo 
        , "物件資訊" );
    _CmdMap[ GMS.Name ] = GMS;	

    GMS.Init( "PrintLV" , &KeyBoardCmd::PrintLV 
        , "PrintLV 列印資訊等級 \n 例 PrintLV 3" );
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

	GMS.Init( "Ping" , &KeyBoardCmd::Ping 
		, "檢查是否還活著" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "ChangeParallel" , &KeyBoardCmd::ChangeParallel_CloseZone 
		, "把所有角色換到第一個平行空間" );
	_CmdMap[ GMS.Name ] = GMS;


	GMS.Init( "SetLua" , &KeyBoardCmd::SetLua, "SETLUA LUAKEY " );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "Census" , &KeyBoardCmd::Census, "Zone Census" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "Crash" , &KeyBoardCmd::Crash, "Crash" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "CrashPrint" , &KeyBoardCmd::CrashPrint, "CrashPrint" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "CrashVec" , &KeyBoardCmd::CrashVec, "CrashVec" );
	_CmdMap[ GMS.Name ] = GMS;	



	GMS.Init( "NetCore" , &KeyBoardCmd::NetCore, "Display netcore number of event" );	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "SEND_MSG_TO_ZONE" , &KeyBoardCmd::SEND_MSG_TO_ZONE, "Send Message to this zone" );	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "InfiniteLoop" , &KeyBoardCmd::InfiniteLoop, "InfiniteLoop" );
	_CmdMap[ GMS.Name ] = GMS;	


	GMS.Init( "ZoneTPBonus" , &KeyBoardCmd::ZoneTPBonus 
		, "修改zone tp bonus" );
	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "ZoneExpBonus" , &KeyBoardCmd::ZoneExpBonus 
		, "修改zone exp bonus" );
	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "ServerTotalItemLog" , &KeyBoardCmd::ZoneExpBonus 
		, "開啟或關閉Server 物品總量的log , ServerTotalItemLog 0 or 1 " );
	_CmdMap[ GMS.Name ] = GMS;
}
//----------------------------------------------------------------------------------------
bool   KeyBoardCmd::InfiniteLoop()
{
	while (1)
	{
		Sleep(100);
	}

	return true;
}
//----------------------------------------------------------------------------------------
bool	KeyBoardCmd::Crash		()
{
	int iID = _GetNum( 1 );

	switch( iID )
	{
	case 0:
		{
			vector< int > temp;
			int a = temp[-1];
			printf( "%d", a );

		} break;
	
	default:
		{
			int *a = NULL;
			*a = 3;
		}
	}

	return true;
}

bool	KeyBoardCmd::CrashPrint()
{
//	char sz[2];
	int a = 0;
	PrintToController(false, "%d %s", 1, a );
	return true;
}

bool	KeyBoardCmd::CrashVec()
{
	
	vector<int> temp;

	vector<int>::iterator it = temp.begin();
	it++;

	int b = *it;
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

//    int		i;
    char	seps[] = " ,\t\n";
    const char*   Arg;
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
        PrintToController(false, "\n指令有問題!! %s not find" , token );
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
	case 0:
		break;
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
bool    KeyBoardCmd::SysInfo             ( )
{
    unsigned i;    
    vector< ServerData* >*  LSrvDataList = Global::Net_ServerList->GetServerGroupData( EM_SERVER_TYPE_LOCAL );
    ServerData*             LSrvData;
    //所有聯線的資訊
    PrintToController(false,  "\n -------區域ID-網路ID----------名稱--------------------" );
    for( i = 0 ; i < LSrvDataList->size() ; i++ )
    {
        LSrvData = (*LSrvDataList)[ i ];
        if( LSrvData == NULL )
            continue;

        PrintToController(false,  "\n       | %4d | %4d |  %s" ,LSrvData->m_dwServerLocalID , LSrvData->m_dwServerID , LSrvData->m_szServerNamed );
    }

    PrintToController(false,  "\n ZoneID = %d  ZoneName = %s " , Ini()->ZoneID , Ini()->ZoneName.c_str() );
    
    BaseItemObject::ReportStat();

    PrintToController(false,  "\n --------------Global-------------------------------" );
    PrintToController(false,  "\n _OnLoginPlayerRecycle=%d  _OnLoginPlayer=%d _Partition=%d" ,  _St->OnLoginPlayerRecycle.MemSize() , _St->OnLoginPlayer.size() , _St->Partition.size() );
    for( i = 0 ; i < _St->Partition.size() ; i++ )
    {
        MyMapAreaManagement* Pt = _St->Partition[ i ];
        if( Pt == NULL )
            continue;
        PrintToController(false,  "\n  *Partition ID=%d Count=%d FreeMemory=%d" , i , Pt->Count() , Pt->FreeMemorySize() );
    }

    return true;
}
//----------------------------------------------------------------------------------------
//物件資訊
bool    KeyBoardCmd::ObjInfo( )
{
    int  Type = _GetNum( 1 );
    if( _Count() == 1 )
        BaseItemObject::ReportAllObjInfo( );
    else
        BaseItemObject::ReportObjInfo( Type );

    return true;
}
//----------------------------------------------------------------------------------------
//設定訊息列印等級
bool    KeyBoardCmd::PrintLV             ( )
{
    int  LV = _GetNum( 1 );

    Global::SetPrintLV( LV );

    return true;
}
//----------------------------------------------------------------------------------------
//結束程式 
bool    KeyBoardCmd::Exit( )
{
	SaveAllPlayer_byCrash();
    Global::Destory();
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
//檢查是否還活著
bool    KeyBoardCmd::Ping        ( )
{
	PrintToController(false,  "Ping\n" );
	fflush( NULL );
	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::SetLua()
{
	string LuaKey		= _GetStr(1);
	string LuaFunc		= _GetStr(2);

	if( strcmp( LuaKey.c_str(), "LuaPVP" ) == 0 )
	{
		Global::Ini()->LuaFunc_PVP = LuaFunc;
	}

	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::Census()
{
	unsigned int	NumPlayer	= BaseItemObject::PlayerCount();
	
	PrintToController(false,  "Zone Census {79CC1671-A46D-478a-9497-4933581B5B03},%d\n", NumPlayer );
	PrintToController(false,  "Add Input Command {79CC1671-A46D-478a-9497-4933581B5B03},SEND_MSG_TO_ZONE,SEND_MSG_TO_ZONE\n" );

	return true;
}
//----------------------------------------------------------------------------------------
bool KeyBoardCmd::NetCore()
{
	CNetBridge*	    Switch		= Net()->NetBSwitch();
	CNetBridge*	    Proxy		= Net()->NetBProxy();

	DWORD dwAvgNewEvent			= 0;
	DWORD dwAvgProcEvent		= 0;
	DWORD dwQueueEvent			= 0;

	dwAvgNewEvent	= Switch->GetAvgNewEvent();
	dwAvgProcEvent	= Switch->GetAvgProcEvent();
	dwQueueEvent	= Switch->GetQueueEvent();

	Print( LV2, "NetCore", "Zone-Switch Netcore: NewEvent = %d (s), ProcEvent = %d (s), QueueEvent = %d", dwAvgNewEvent, dwAvgProcEvent, dwQueueEvent );

	dwAvgNewEvent	= Proxy->GetAvgNewEvent();
	dwAvgProcEvent	= Proxy->GetAvgProcEvent();
	dwQueueEvent	= Proxy->GetQueueEvent();

	Print( LV2, "NetCore", "Zone-Proxy Netcore: NewEvent = %d (s), ProcEvent = %d (s), QueueEvent = %d", dwAvgNewEvent, dwAvgProcEvent, dwQueueEvent );

	return true;
}


bool KeyBoardCmd::SEND_MSG_TO_ZONE()
{

	const char* pszMsg = _GetStr(1);

	
	NetSrv_Talk::SendRunningMsg_Zone( pszMsg );

	return true;
}

bool	KeyBoardCmd::ChangeParallel_CloseZone( )
{
	if( RoleDataEx::G_ZoneID < _DEF_ZONE_BASE_COUNT_ )
		return false;
	Global::ChangeParallel_CloseZone( 0 );
	return true;
}

bool	KeyBoardCmd::ZoneTPBonus()
{
	int  Bonus = _GetNum( 1 );

	if( Bonus < 0 || Bonus > 200 )
		return false;

	Global::Ini()->ZoneValue_TPBonus = Bonus;
	RoleDataEx::GlobalSetting.TpRate = 1 + ( Bonus / 100.0f);

	return true;
}
bool	KeyBoardCmd::ZoneExpBonus()
{
	int  Bonus = _GetNum( 1 );

	if( Bonus < 0 || Bonus > 200 )
		return false;

	Global::Ini()->ZoneValue_EXPBonus = Bonus;
	RoleDataEx::GlobalSetting.ExpRate = 1 + ( Bonus / 100.0f);

	return true;
}

bool	KeyBoardCmd::ServerTotalItemLog( )
{
	int  Value = _GetNum( 1 );
	Global::Ini()->IsServerTotalItemLog = ( Value!= 0 );
	return true;
}