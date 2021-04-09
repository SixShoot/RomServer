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
        _strlwr( (char*)Name.c_str() );
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

    //結束程式 
    bool    Exit        ( );

	//檢查是否還活著
	bool    Ping        ( );

	//記憶體檢查
	bool    MemoryCheck ( );

	//記憶體列印
	bool	MemoryReport( );

	//儲存測試
	bool	TestSaveDB	( );

	bool	Census();

	//重載節慶資料
	bool    ReloadFestival();

	bool	EnableWorldConfigTable();

	bool	ReloadStringDB();
    //----------------------------------------------------------------------------------------

protected:
	//處理來自Controller的指令輸入
	bool ControllerInput( );
};
