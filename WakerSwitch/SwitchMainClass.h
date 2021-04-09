#ifndef		__SWITCHMAINCLASS_H_2004_8_24__
#define		__SWITCHMAINCLASS_H_2004_8_24__
#pragma warning (disable:4786)

#include "SwitchClass.h"
#include "IniFile\IniFile.h"

//---------------------------------------------------------------------------------
class	SwitchMainClass 
{

    SwitchClass  	_Switch;
    IniFileClass	_Ini;
    int			    _ExitFlag;
    //���e�B�z
    void	_Flush();       
public:
	

    SwitchMainClass(char*	IniFile);
    virtual ~SwitchMainClass();

	IniFileClass* Ini() {return &_Ini;}

    //GateWay ��ƥ洫�B�z
    bool    Process();
	void	Exit( );
	SwitchClass*  	Switch() { return &_Switch; };
};
//---------------------------------------------------------------------------------
#endif  //__SWITCHMAINCLASS_H_2004_8_24__
