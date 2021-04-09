#include ".\Net_ServerList_Child.h"	
#include "RemotetableInput/RemotableInput.h"

/*
#include "../Net_Talk/NetSrv_TalkChild.h"
#include "RoleData/ObjectDataClass.h"
#include "../../MainProc/Global.h"
*/

#pragma unmanaged

void CNet_ServerList_Child::OnServerEvent_Shutdown ( int iServerID, int iCloseTime )
{
	/*
	char szMsg[1024];

	// Board cast to all client
		if( iCloseTime >= 1 )
		{
			sprintf( szMsg, g_ObjectData->GetString( "ZONE_MSG_MIN_SHUTDOWN"), (iCloseTime*5) );
			NetSrv_Talk::SendRunningMsg_Zone( szMsg );
		}

		if( iCloseTime == 1 )
		{
			// 最後 5min
			Global::m_iShutdownTimes	= 5 * 60;	// 每秒扣一次
			Global::Schedular()->Push( Global::_CheckZoneShutdown, 1000, NULL , NULL );
		}
	*/
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
			g_remotableInput.OutputStartupMessage();	// 設定 OKAY


	}
}


void CNet_ServerList_Child::OnServerEvent_ServerLogOut( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	int a  =3;
}
#pragma managed