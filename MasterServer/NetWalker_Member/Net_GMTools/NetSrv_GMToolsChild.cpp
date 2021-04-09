
#include "NetSrv_GMToolsChild.h"
#include <string.h>
#include "../net_login/NetSrv_Master_Login_Child.h"
#include "../../mainproc/ClientInfo/ClientInfo.h"
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

	/*
	
	EM_ClientStatus_WaitLeaveWorld	= 4,	// 角色已斷線, 但有進入 Zone Server 的角色, 等待 ZoneSrv 回應
	EM_ClientStatus_WaitDelete		= 5,
	EM_ClientStatus_ReloadingData	= 6,
	EM_ClientStatus_LoadingData		= 7,
	EM_ClientStatus_RoleLeaveZone	= 8,
	EM_ClientStatus_WaitEnterZone	= 9,
	*/


	int		EnterWorldCount = 0;
	int		SelectRoleCount = 0;
	int		WaitLeaveWorldCount = 0;
	
	map< string , ClientInfo* >::iterator Iter;

	//1 計算所有管理的物件進入世界 與沒進入的量
	for( Iter = g_pClientManger->m_mapAccount.begin() ; Iter != g_pClientManger->m_mapAccount.end() ; Iter++ )
	{
		if( Iter->second->m_emClientStatus == EM_ClientStatus_WaitLeaveWorld )
			WaitLeaveWorldCount++;

		/*
		if( Iter->second->m_bEnterWorld != false )
			EnterWorldCount++;
		else
			SelectRoleCount++;
		*/
	}

	EnterWorldCount = (int)g_pClientManger->m_mapDBID.size();
	SelectRoleCount	= (int)g_pClientManger->m_mapAccount.size() - EnterWorldCount;


	char Buf[8][64];
	sprintf( Buf[0] , "EnterWord:%d" , EnterWorldCount );
	sprintf( Buf[1] , "SelectRole:%d" , SelectRoleCount );
	sprintf( Buf[2] , "WaitLeaveWorld:%d" , WaitLeaveWorldCount );
	sprintf( Buf[3] , "LoadQueue:%d" , CNetSrv_MasterServer_Child::m_mapLoadQueue.size() );
	sprintf( Buf[4] , "WaitQueue:%d" , CNetSrv_MasterServer_Child::m_vecWaitQueue.size() );



	switch( RequestType )
	{
	case EM_ServerStateRequestType_Normal:
		S_ServerStateResult( NetID , RequestType , "Master" , "Master" , "OK"
			, Buf[0] , Buf[1]
			, Buf[2] , Buf[3]
			, Buf[4] , ""
			, "" , "" );
		break;
	default:
		break;
	}
}