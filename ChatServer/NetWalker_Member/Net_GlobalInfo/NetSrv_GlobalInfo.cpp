#include "NetSrv_GlobalInfo.h"
//-------------------------------------------------------------------
NetSrv_GlobalInfo*    NetSrv_GlobalInfo::This         = NULL;
//-------------------------------------------------------------------
bool NetSrv_GlobalInfo::_Init()
{
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_GlobalInfo::_Release()
{
    return true;
}
//-------------------------------------------------------------------
