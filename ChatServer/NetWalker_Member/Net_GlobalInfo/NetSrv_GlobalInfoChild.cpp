#include "NetSrv_GlobalInfoChild.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_GlobalInfoChild::Init()
{
    NetSrv_GlobalInfo::_Init();

    if( This != NULL)
        return false;

    This = new NetSrv_GlobalInfoChild;

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_GlobalInfoChild::Release()
{
    if( This == NULL )
        return false;

    delete This;
    This = NULL;

    NetSrv_GlobalInfo::_Release();
    return true;
    
}
