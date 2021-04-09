#include ".\Net_ServerList_Child.h"	
/*
#include "../Net_Talk/NetSrv_TalkChild.h"
#include "RoleData/ObjectDataClass.h"
*/
#include "../../MainProc/Global.h"
#include "RemotetableInput/RemotableInput.h"


#pragma unmanaged

void CNet_ServerList_Child::OnServerEvent_Shutdown ( int iServerID, int iCloseTime )
{
	if( iCloseTime == 0 )
	{
		Global::Destroy();
	}
}

void CNet_ServerList_Child::OnServerEvent_ServerReady( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{

	int a  =3;
}


void CNet_ServerList_Child::OnServerEvent_ServerLogin( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	if( ServerType == EM_SERVER_TYPE_MASTER )
	{
		// Set Monitor to be okay
			g_remotableInput.OutputStartupMessage();	// ³]©w OKAY
	}
}


void CNet_ServerList_Child::OnServerEvent_ServerLogOut( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	int a  =3;
}

#pragma managed