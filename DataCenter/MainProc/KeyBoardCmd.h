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
    //��l��
    KeyBoardCmd( );

    //�B�z��L��J
    bool    KeyBoardInput( );
    //GM�R�O�B�z
    bool	CmdProc        (  const char* Str );
    //-----------------------------------------------------------------------------------
    //Help �C�X�Ҧ���GM���O
    bool    Help		( );

    //�����{�� 
    bool    Exit        ( );

	//�ˬd�O�_�٬���
	bool    Ping        ( );

	//�O�����ˬd
	bool    MemoryCheck ( );

	//�O����C�L
	bool	MemoryReport( );

	//�x�s����
	bool	TestSaveDB	( );

	bool	Census();

	//�����`�y���
	bool    ReloadFestival();

	bool	EnableWorldConfigTable();

	bool	ReloadStringDB();
    //----------------------------------------------------------------------------------------

protected:
	//�B�z�Ӧ�Controller�����O��J
	bool ControllerInput( );
};
