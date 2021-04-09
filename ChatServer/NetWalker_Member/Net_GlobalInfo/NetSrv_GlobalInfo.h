#pragma once

#include "PG/PG_GlobalInfo.h"
#include "../../MainProc/Global.h"
//-------------------------------------------------------------------
class GroupParty;
//-------------------------------------------------------------------

class NetSrv_GlobalInfo : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_GlobalInfo* This;
    static bool _Init();
    static bool _Release();
		
public:

};

