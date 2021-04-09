#ifndef __GLOBAL_H__
#define __GLOBAL_H__

//#include "../NetWaker/CliNetWaker.h"
#include <Windows.h>
#include "BaseItemObject/BaseItemObject.h"
#include "IniFile/IniFile.h"

#include "GlobalBase.h"
#include "ControllerClient/ControllerClient.h"

class Global : public GlobalBase
{
    static IniFileClass	                _Ini;
    static bool                         _IsDelay;       //¬O§_Server ¦bDelayª¬ºA
    static bool                         _IsDestroy;       
//    static FunctionSchedularClass*      _Schedular;

public:
    static bool     Init( char* IniFile );
    static bool     Release( );
    static bool     Process( );

    static void     Destroy( ){ _IsDestroy = true; };
	static IniFileClass*	Ini() { return &_Ini; };

	static GameObjDbStructEx* GetObjDB( int ) { return NULL; };
   
};
#endif //__GLOBAL_H__