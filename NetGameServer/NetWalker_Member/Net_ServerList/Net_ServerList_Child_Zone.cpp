#include ".\Net_ServerList_Child.h"	
#include "../Net_Talk/NetSrv_TalkChild.h"
#include "RoleData/ObjectDataClass.h"
#include "../../MainProc/Global.h"

#include "RemotetableInput/RemotableInput.h"
//#include <Windows.h>


#pragma unmanaged

void CNet_ServerList_Child::OnServerEvent_Shutdown( int iServerID, int iCloseTime )
{
	static bool bShutdown = false;
	char szMsg[1024];

	// Board cast to all client
		if( iCloseTime > 5 && iCloseTime % 5 == 0 )	// 每 5min 公告一次
		{		
			sprintf( szMsg, "[ZONE_MSGKEY_MIN_SHUTDOWN][$SETVAR1=%d]", iCloseTime );
			NetSrv_Talk::SendRunningMsg_Zone( szMsg );
		}

		if( iCloseTime <= 5 && bShutdown == false )
		{
			bShutdown = true;
			// 最後 5min
			Global::m_iShutdownTimes	= iCloseTime * 60;	// 每秒扣一次
			Global::Schedular()->Push( Global::_CheckZoneShutdown, 1000, NULL , NULL );
		}
}

void	CNet_ServerList_Child::OnServerEvent_ServerReady			( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	int a = 3;
}

void	CNet_ServerList_Child::OnServerEvent_ServerLogin			( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	if( ServerType == EM_SERVER_TYPE_MASTER )
	{
		// Set Monitor to be okay
			g_remotableInput.OutputStartupMessage();	// 設定 OKAY

		// Command
			PrintToController(false,  "Add Input Command {79CC1671-A46D-478a-9497-4933581B5B03},SEND_MSG_TO_ZONE,SEND_MSG_TO_ZONE\n" );
	}

/*	if( ServerType == EM_SERVER_TYPE_DATACENTER )
	{
		Global::Log_Verion();
	}
*/
}

void	CNet_ServerList_Child::OnServerEvent_ServerLogOut			( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	int a = 3;

}

#pragma managed