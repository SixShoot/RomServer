#pragma once

#include <vector>
#include <map>
#include <string>
#include "Global.h"

#pragma		warning (disable:4786)

using namespace std;
class KeyBoardCmd;
//----------------------------------------------------------------------------------------
typedef	bool (KeyBoardCmd::*CmdFunction)( );
//----------------------------------------------------------------------------------------
//	GM命令管理結構
//----------------------------------------------------------------------------------------
struct KeyBaordCmdManageStruct
{
    string	Name;
    string	Help;

    CmdFunction	Fun;
    void Init( string _Name , CmdFunction _Fun , string	_Help )
    {
        Name = _Name;
        Help = _Help;
        Fun  = _Fun;
        //_strlwr( (char*)Name.c_str() );
		std::transform( Name.begin(), Name.end(), Name.begin(), ::tolower );
    }
};
//----------------------------------------------------------------------------------------
class KeyBoardCmd : public Global
{
    map< string , KeyBaordCmdManageStruct  >   _CmdMap;

    vector< string > _Val;
    string  CmdStr;

    int		        _GetNum( int );
    char*	        _GetStr( int );
    float	        _GetFloat( int );
    int             _Count( );

public:
    //初始化
    KeyBoardCmd( );

    //處理鍵盤輸入
    bool    KeyBoardInput( );
    //GM命令處理
    bool	CmdProc        (  const char* Str );
    //-----------------------------------------------------------------------------------
    //Help 列出所有的GM指令
    bool    Help		( );
    //系統訊息
    bool    SysInfo     ( );

    //物件資訊
    bool    ObjInfo     ( );

    //設定訊息列印等級
    bool    PrintLV     ( );

    //結束程式 
    bool    Exit        ( );

	//檢查是否還活著
	bool    Ping        ( );

    //記憶體檢查
    bool    MemoryCheck ( );

	//記憶體列印
	bool	MemoryReport( );

	//線上設定 LUA
	bool    SetLua ( );

	//回報線上人數
	bool	Census		();

	//回報線上人數
	bool	Crash		();
	bool	CrashVec	();
	bool	CrashPrint	();

	//Net Core Event
	bool	NetCore		();

	//Net Core Event
	bool	SEND_MSG_TO_ZONE();

	//測試無窮回圈用
	bool    InfiniteLoop();

	bool	ZoneTPBonus();
	bool	ZoneExpBonus();

	bool	ServerTotalItemLog();
    //----------------------------------------------------------------------------------------

	bool	ChangeParallel_CloseZone( );
protected:
	//處理來自Controller的指令輸入
	bool ControllerInput( );
};
