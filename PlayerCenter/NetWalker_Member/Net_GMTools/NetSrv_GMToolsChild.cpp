
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

	//char	Buf[8][64];
	/*
	sprintf( Buf[0] , "WaitLogout:%d" , BaseItemObject::St()->WaitLogoutCount );
	sprintf( Buf[1] , "ObjCount:%d" , BaseItemObject::St()->AllPlayerObj_DBID.size() );
	sprintf( Buf[2] , "NeedSaveCount:%d" , BaseItemObject::St()->NeedSaveCount );
	sprintf( Buf[3] , "SaveCount:%d" , BaseItemObject::St()->SaveCount );
*/
	switch( RequestType )
	{
	case EM_ServerStateRequestType_Normal:
		S_ServerStateResult( NetID , RequestType , "RoleDBCenter" , "RoleDBCenter" , "OK"
			, "" , ""
			, "" , ""
			, "" , ""
			, "" , "" );
		break;
	default:
		break;
	}
}