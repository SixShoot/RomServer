
#ifndef	__ACCOUNTSRV_MAIN_CLASS_H__
#define __ACCOUNTSRV_MAIN_CLASS_H__
#pragma warning (disable:4786)

#include "AccountSrvClass.h"
#include "IniFile\IniFile.h"
#include "debuglog\errormessage.h"
#include "BaseFunction/BaseFunction.h"
#include "smallobj/SmallObj.h"

class	AccountMainClass
{

    AccountClass*  	_Account;

    IniFileClass	_Ini;
    //-----------------------------------------------------------------------------------
    //���檬�A
    //-----------------------------------------------------------------------------------
    int			_ExitFlag;

    //���e�B�z
    void		_Flush( );

public:
    AccountMainClass(char*	IniFile);
    virtual ~AccountMainClass( );


	IniFileClass* Ini() {return &_Ini;}

    //GateWay ��ƥ洫�B�z
    bool	Process();

	void	Exit( );
};
#endif //__ACCOUNTSRV_MAIN_CLASS_H__
