#pragma once
#pragma warning (disable:4786)

#include "ServerListSrvClass.h"
#include "IniFile\IniFile.h"

class	ServerListMainClass 
{

    ServerListClass*  	_SrvList;

    IniFileClass		_Ini;
    //-----------------------------------------------------------------------------------
    //執行狀態
    //-----------------------------------------------------------------------------------
    int			_ExitFlag;

    //收送處理
    void		_Flush( );


public:	
	ServerListMainClass(char*	IniFile);
    virtual ~ServerListMainClass( );

	IniFileClass* Ini() {return &_Ini;}

    //GateWay 資料交換處理
    bool	Process();

	void	Exit( );

	ServerListClass*  	SrvList() { return _SrvList; };
};

