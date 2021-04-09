#ifndef __GLOBAL_H__
#define __GLOBAL_H__

//#include "../NetWaker/CliNetWaker.h"
#include <Windows.h>
#include "IniFile/IniFile.h"
#include "lua/myvm/lua_vmclass.h"
#include "LuaPlot/LuaPlot.h"
#include "BaseItemObject/M_BaseItemObject.h"
#include "GroupObject/GroupObject.h"
#include "RoleData/ObjectDataClass.h"

#include "GlobalBase.h"
#include "controllerClient/ControllerClient.h"

struct ItemSendBaseStruct
{
	int	PlayerDBID;
	ItemFieldStruct Item;
};

class Global : public GlobalBase
{
    static IniFileClass	        	_Ini;
    static bool                 	_IsDelay;       //¬O§_Server ¦bDelayª¬ºA
	static bool                 	_IsDestroy;
	static bool						_IsTalkLog;
	static int						_LastProcTime;
	static vector< ItemSendBaseStruct >	_ItemSendList;

public:

    static bool     Init( char* IniFile );
    static bool     Release( );
    static bool     Process( );
	static void		SendItemListProc();

	static GameObjDbStructEx*		GetObjDB( int OrgID );
	static void		Destroy();

	static void		Log_Talk			( TalkLogTypeENUM Type , int ChannelID , int FromPlayerDBID , int ToPlayerDBID , const char* Content );
	static vector< ItemSendBaseStruct >& ItemSendList()	{ return _ItemSendList; }
	static IniFileClass* Ini() {return &_Ini;}
};
#endif //__GLOBAL_H__