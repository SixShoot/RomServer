
#include "NetSrv_GMToolsChild.h"
#include <string.h>
//-----------------------------------------------------------------
bool    NetSrv_GMToolsChild::Init()
{
    NetSrv_GMTools::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_GMToolsChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_GMToolsChild::Release()
{
    if( This == NULL )
        return false;

    delete This;
    This = NULL;

    NetSrv_GMTools::_Release();
    return true;
    
}

void	NetSrv_GMToolsChild::R_ServerStateRequest( int NetID , ServerStateRequestTypeENUM	RequestType )
{
	switch( RequestType )
	{
	case EM_ServerStateRequestType_Normal:
		S_ServerStateResult( NetID , RequestType , "Partition" , "Partition" , "OK"
			, "" , ""
			, "" , ""
			, "" , ""
			, "" , "" );
		break;
	default:
		break;
	}
}