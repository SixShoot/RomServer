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
    //���e�B�z
    void	_Flush();       
public:
	

    GatewayMainClass(char*	IniFile);
    virtual ~GatewayMainClass();

	IniFileClass* Ini(){return &_Ini;}

    //GateWay ��ƥ洫�B�z
    bool    Process();
	void	Exit( );

	GatewayClass* Getway()	{ return &_Gateway; }
};
//---------------------------------------------------------------------------------

