#ifndef		__PROXYMAINCLASS_H_2004_8_24__
#define		__PROXYMAINCLASS_H_2004_8_24__
/************************************
NetWaker 訊息回圈處理物件
Revision By: hsiang
Revised on 2004/8/23 下午 04:31:19
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

    //GateWay 資料交換處理
    bool	Process();

	void	Exit() { _ExitFlag = 1; };

	ProxyClass* Proxy()	{ return &_Proxy; }
};

#endif  //__PROXYMAINCLASS_H_2004_8_24__
