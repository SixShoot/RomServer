#pragma once

#include <queue>

#include "NetMaster_ZoneStatus.h"



enum EM_LOCALSERVER_STATUS
{
	EM_LSRV_STATUS_OK					=0,
	EM_LSRV_STATUS_SENDINGREG			=1,		// 等待 Master 分配 ID
	EM_LSRV_STATUS_WAITINGREG			=2,		// 等待 Master 分配 ID
	EM_LSRV_STATUS_WAITINGLSRV			=3,		// 等待 LServer 回應 準備 okay 狀態
	EM_LSRV_STATUS_TIMEOUTCLOSING		=4,		// 等待 Master 分配 ID
	
};

struct LocalServerStatus
{
public:
	EM_LOCALSERVER_STATUS	emStatus;
	int						iTimeEventID_ZoneSrvReg;
	int						iLocalID;
	int						iServerID;					// 網路代號
};

class CNetMaster_ZoneStatus_Child : public CNetMaster_ZoneStatus
{
public:
	CNetMaster_ZoneStatus_Child(void);
	~CNetMaster_ZoneStatus_Child(void);

	static CNetMaster_ZoneStatus_Child*	m_pThis;
	static bool							m_bInitDone;

	static bool		Init();
	static bool		Release();

	static int		OnEvent_StopAllServer		( SchedularInfo* pInfo , PVOID pContext );	// 排程檢查停機倒數階段
	static int		OnEvent_CheckReloginAccount	( SchedularInfo* pInfo , PVOID pContext );	// 檢查強登角色機制

	//-----------------------------------------------------------------------
	// Local Server Reg
	//-----------------------------------------------------------------------
	
	virtual void	On_ZoneSrvReg				( int iServerID, int iZoneID, int iZoneItemID, int iRemainCounter );
	virtual void	On_ZoneSrvOkay				( int iServerID, int iZoneID );

	//-----------------------------------------------------------------------
	int				GetFreeZoneItemID();

	static void		OnEvent_ZoneSrvConnected	( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	static void		OnEvent_ZoneSrvDisconnect	( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );

	static void		OnEvent_NonageSrvConnected	( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	

	//static int		OnEvent_LocalServerRegTimeOut	( SchedularInfo* pInfo , PVOID pContext );
	static int		OnEvent_CheckRegStatus		( SchedularInfo* pInfo , PVOID pContext );
	
	//void			CheckRegStatus					();


	virtual void	On_RoleLeaveWorld			( int iServerID, int iZoneSrvID, int iDBID, EM_RoleLeaveWorld emEvent, int iExpToNextLevel  );

	virtual void	On_RoleEnterZone			( int iSrvID, int iZoneSrvID, int iDBID );
	virtual void	On_RoleLeaveZone			( int iSrvID, int iZoneSrvID, int iDBID );

	virtual void	On_KickZonePlayerResult		( int iSrvID, int iZoneID, const char* pszAccountName, int iResult );

	virtual void	On_BTI_SetPlayerHours		( int iSrvID, const char* pszAccountName, int iHours );
	


	map< int , LocalServerStatus* >			m_mapZoneSrvStatus;
	set< int >								m_setUsedZoneItemID;	// 使用中的 ZoneItemID


	int										m_iShutdownMin;			// 停機倒數
};
