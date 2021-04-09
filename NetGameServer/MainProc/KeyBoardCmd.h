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
    //��l��
    KeyBoardCmd( );

    //�B�z��L��J
    bool    KeyBoardInput( );
    //GM�R�O�B�z
    bool	CmdProc        (  const char* Str );
    //-----------------------------------------------------------------------------------
    //Help �C�X�Ҧ���GM���O
    bool    Help		( );
    //�t�ΰT��
    bool    SysInfo     ( );

    //�����T
    bool    ObjInfo     ( );

    //�]�w�T���C�L����
    bool    PrintLV     ( );

    //�����{�� 
    bool    Exit        ( );

	//�ˬd�O�_�٬���
	bool    Ping        ( );

    //�O�����ˬd
    bool    MemoryCheck ( );

	//�O����C�L
	bool	MemoryReport( );

	//�u�W�]�w LUA
	bool    SetLua ( );

	//�^���u�W�H��
	bool	Census		();

	//�^���u�W�H��
	bool	Crash		();
	bool	CrashVec	();
	bool	CrashPrint	();

	//Net Core Event
	bool	NetCore		();

	//Net Core Event
	bool	SEND_MSG_TO_ZONE();

	//���յL�a�^���
	bool    InfiniteLoop();

	bool	ZoneTPBonus();
	bool	ZoneExpBonus();

	bool	ServerTotalItemLog();
    //----------------------------------------------------------------------------------------

	bool	ChangeParallel_CloseZone( );
protected:
	//�B�z�Ӧ�Controller�����O��J
	bool ControllerInput( );
};
