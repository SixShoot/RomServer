#pragma once

#include <vector>
#include <map>
#include <string>
#include "GatewayMainClass.h"
#pragma		warning (disable:4786)

using namespace std;
class KeyBoardCmd;
//----------------------------------------------------------------------------------------
typedef	bool (KeyBoardCmd::*CmdFunction)( );
//----------------------------------------------------------------------------------------
//	GM�R�O�޲z���c
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
    string				_CmdStr;
	GatewayMainClass*	_GatewayMain;
    int		        _GetNum( int );
    char*	        _GetStr( int );
    float	        _GetFloat( int );
    int             _Count( );

public:
    //��l��
	KeyBoardCmd( GatewayMainClass* S );

    //�B�z��L��J
    bool    KeyBoardInput( );
    //GM�R�O�B�z
    bool	CmdProc     (  const char* Str );
    //-----------------------------------------------------------------------------------
	bool	Help		();

	//�ˬd�O�_�٬���
	bool    Ping        ();

	bool	Exit		();

	bool	SrvLoadOK	();

	bool	NetCore		();

protected:
	//�B�z�Ӧ�Controller�����O��J
	bool ControllerInput( );
	
	/*
	// Server Monitor
	//-----------------------------------------------------------------------------------
	bool	Census			();
	bool	Enable_Login	();
	bool	Disable_Login	();
	bool	ServerGroupOkay	();
*/
};
