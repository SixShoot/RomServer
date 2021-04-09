#pragma once
#include "KeyBoardCmd.h" 
#include <conio.h>
#include "../NetWalker_Member/NetWakerDataCenterInc.h"
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

	GMS.Init( "TestSaveDB" , &KeyBoardCmd::TestSaveDB 
		, "測試儲存資料庫\n例 TestSaveDB 100" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "MemoryCheck" , &KeyBoardCmd::MemoryCheck 
		, "記憶體檢查" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "MemoryReport" , &KeyBoardCmd::MemoryReport 
		, "記憶體列印" );
	_CmdMap[ GMS.Name ] = GMS;		

    GMS.Init( "Exit" , &KeyBoardCmd::Exit 
        , "結束程式" );
    _CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "Ping" , &KeyBoardCmd::Ping 
		, "檢查是否還活著" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "Census" , &KeyBoardCmd::Census, "Census" );	
	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "ReloadFestival" , &KeyBoardCmd::ReloadFestival, "ReloadFestival" );	
	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "EnableWorldConfigTable" , &KeyBoardCmd::EnableWorldConfigTable, "EnableWorldConfigTable 0 or 1" );	
	_CmdMap[ GMS.Name ] = GMS;

	GMS.Init( "ReloadStringDB" , &KeyBoardCmd::ReloadStringDB, "重載後台字串" );	
	_CmdMap[ GMS.Name ] = GMS;

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

    MyStrcpy( Buf , Str ,sizeof( Buf) );

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
        //printf( "\n%s" , (char*)KBCmd.Help.c_str() );
		PrintToController(false,  "\n%s" , (char*)KBCmd.Help.c_str() );
        return true;
    }

    return (this->*KBCmd.Fun)( );

}

//----------------------------------------------------------------------------------------
bool KeyBoardCmd::Census()
{
	//世界ID
	//printf( "World Census {},%d\n", Global::GetWorldID() );
	PrintToController(false,  "World Census {},%d\n", Global::GetWorldID() );
	return true;
}

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
	Print( LV4 , "KeyBoardCmd::Exit" ,  "Close Program" );
	CNetDC_GuildChild::SaveGuildInfo( );
    Global::Destory();
    return true;
}
//----------------------------------------------------------------------------------------
//儲存測試
bool	KeyBoardCmd::TestSaveDB	( )
{
//	Global::SaveTest( _GetNum( 1 ) );
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
//重載節慶資料
bool    KeyBoardCmd::ReloadFestival()
{
	CNetDC_FestivalChild::ReloadFestival();
	return true;
}

bool	KeyBoardCmd::EnableWorldConfigTable()
{
	int Value = _GetNum( 1 );
	if( Value == 0 )
		Global::_EnableWorldConfigTable = false;
	else
		Global::_EnableWorldConfigTable = true;

	return true;
}

bool	KeyBoardCmd::ReloadStringDB()
{
	CNetDC_OtherChild::ReloadStringDB();
	return true;
}