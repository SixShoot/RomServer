#pragma once

#include "NetSrv_GMTools.h"

class NetSrv_GMToolsChild : public NetSrv_GMTools
{
public:
    static bool Init();
    static bool Release();


    virtual void    R_OnGMToolsConnect( int NetID );
    virtual void    R_ObjectDataRequest( int NetID , int GUID );
    virtual void    R_ToPlayerMsg( int NetID , BaseItemObject*	Obj , const char* Name , const char* Msg );
    virtual void    R_ToAllPlayerMsg( int NetID , const char* Msg );
	virtual void	R_GMCommand( int NetID , int PlayerGUID , const char* Account , int ManageLv , const char* Cmd );
	virtual void	R_ServerStateRequest( int NetID , ServerStateRequestTypeENUM	RequestType );
};

