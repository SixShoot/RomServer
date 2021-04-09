#include "../NetWakerGSrvInc.h"
#include "NetSrv_ImportBoardChild.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_ImportBoardChild::Init()
{
    NetSrv_ImportBoard::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_ImportBoardChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_ImportBoardChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_ImportBoard::_Release();

    delete This;
    This = NULL;

    return true;
    
}

void NetSrv_ImportBoardChild::RC_ImportBoardRequest( BaseItemObject* Obj , float UpdateTime )
{
	SD_ImportBoardRequest( Obj->Role()->DBID() , UpdateTime );
}