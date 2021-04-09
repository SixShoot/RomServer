#pragma  once

#include "NetSrv_GMTools.h"
#include <map>
//------------------------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------------------------
class NetSrv_GMToolsChild : public NetSrv_GMTools
{
public:
    static bool Init();
    static bool Release();

	virtual void	R_ServerStateRequest( int NetID , ServerStateRequestTypeENUM	RequestType );
};

