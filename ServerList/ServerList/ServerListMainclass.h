#pragma once
#pragma warning (disable:4786)

#include "ServerListSrvClass.h"
#include "IniFile\IniFile.h"

class	ServerListMainClass 
{

    ServerListClass*  	_SrvList;

    IniFileClass		_Ini;
    //-----------------------------------------------------------------------------------
    //���檬�A
    //-----------------------------------------------------------------------------------
    int			_ExitFlag;

    //���e�B�z
    void		_Flush( );


public:	
	ServerListMainClass(char*	IniFile);
    virtual ~ServerListMainClass( );

	IniFileClass* Ini() {return &_Ini;}

    //GateWay ��ƥ洫�B�z
    bool	Process();

	void	Exit( );

	ServerListClass*  	SrvList() { return _SrvList; };
};

