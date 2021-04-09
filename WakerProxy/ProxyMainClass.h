#ifndef		__PROXYMAINCLASS_H_2004_8_24__
#define		__PROXYMAINCLASS_H_2004_8_24__
/************************************
NetWaker �T���^��B�z����
Revision By: hsiang
Revised on 2004/8/23 �U�� 04:31:19
Comments: 
************************************/
#pragma warning (disable:4786)

#include "ProxyClass.h"
#include "IniFile\IniFile.h"


class	ProxyMainClass
{
    ProxyClass  	_Proxy;
    IniFileClass	_Ini;
    int			    _ExitFlag;


    void		_Flush();
public:
    ProxyMainClass(char*	IniFile);
    virtual ~ProxyMainClass();

	IniFileClass* Ini() {return &_Ini;}

    //GateWay ��ƥ洫�B�z
    bool	Process();

	void	Exit() { _ExitFlag = 1; };

	ProxyClass* Proxy()	{ return &_Proxy; }
};

#endif  //__PROXYMAINCLASS_H_2004_8_24__
