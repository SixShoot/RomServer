#pragma once
#pragma warning (disable:4786)

#include "GatewayClass.h"
#include "IniFile\IniFile.h"

//---------------------------------------------------------------------------------
class	GatewayMainClass 
{
	GatewayClass	_Gateway;
    IniFileClass	_Ini;
    int			    _ExitFlag;
    //收送處理
    void	_Flush();       
public:
	

    GatewayMainClass(char*	IniFile);
    virtual ~GatewayMainClass();

	IniFileClass* Ini(){return &_Ini;}

    //GateWay 資料交換處理
    bool    Process();
	void	Exit( );

	GatewayClass* Getway()	{ return &_Gateway; }
};
//---------------------------------------------------------------------------------

