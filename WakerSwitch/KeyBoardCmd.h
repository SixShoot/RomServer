#pragma once

#include <vector>
#include <map>
#include <string>
#include "SwitchMainClass.h"
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
class KeyBoardCmd
{
protected:
    map< string , KeyBaordCmdManageStruct  >   _CmdMap;

    vector< string >	_Val;
    string				CmdStr;
	SwitchMainClass*	_SwitchMain;
    int		        _GetNum( int );
    char*	        _GetStr( int );
    float	        _GetFloat( int );
    int             _Count( );

public:
    //初始化
	KeyBoardCmd( SwitchMainClass* S );

    //處理鍵盤輸入
    bool    KeyBoardInput( );
    //GM命令處理
    bool	CmdProc     (  const char* Str );
    //-----------------------------------------------------------------------------------
	bool	Help		();

	//檢查是否還活著
	bool    Ping        ();

	bool	Exit		();

	bool	SrvLoadOK	();

	bool	NetCore		();
	
	// Server Monitor
	//-----------------------------------------------------------------------------------
	bool	Census			();
	bool	Enable_Login	();
	bool	Disable_Login	();
	bool	ServerGroupOkay	();

	bool	SetRedirectHost( );


	bool	Enable_ClientCheck_NoRespond();
	bool	Disable_ClientCheck_NoRespond();

	bool	Enable_ClientCheck_SendBuffLimit();
	bool	Disable_ClientCheck_SendBuffLimit();

	bool	Enable_Server_CheckNoRespond();
	bool	Disable_Server_CheckNoRespond();

	bool	Enable_Server_CheckSendBuffLimit();
	bool	Disable_Server_CheckSendBuffLimit();

	bool	ServerStatus();


	bool	SetSwitchStatus();
protected:
	//處理來自Controller的指令輸入
	bool ControllerInput( );

};
