#pragma once

#include ".\NetSrv_ZoneStatus.h"	

class CNetSrv_ZoneStatus_Child : public CNetSrv_ZoneStatus
{
public:
	CNetSrv_ZoneStatus_Child(void)	{};
	~CNetSrv_ZoneStatus_Child(void)	{};

	static bool	Init();
	static bool Release();

	//-----------------------------------------------------------------------
	// Login
	//-----------------------------------------------------------------------

	virtual	void		On_RequestZoneSrvReg	( int iServerID );

	virtual void		On_SetRoleToWorld		( int iServerID, int iClientID, PlayerRoleData* pRole );


	// Local Server Reg
	virtual void		On_ZoneSrvRegResult	( int iServerID, EM_ZoneSrvRegResult emRegResult, int iZoneItemID );

	virtual void		On_BTIPlayTime				( int iServerID, int iRoleDBID, int iHours );

	virtual void		On_KickZonePlayer			( int iServerID, const char* szAccountName );
};
