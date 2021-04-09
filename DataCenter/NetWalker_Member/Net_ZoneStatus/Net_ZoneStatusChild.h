#pragma once
#ifndef	__NET_ZONE_STATUS_CHILD_H__
#define __NET_ZONE_STATUS_CHILD_H__

#include "Net_ZoneStatus.h"

class Net_ZoneStatusChild : public Net_ZoneStatus
{
public:
	static bool	Init();
	static bool	Release();

public:
	virtual void RL_RoleLeaveWorld( int ServerID, const char* pszAccountName, const char* pszRoleName, int iRoleDBID, int iWorldID, int iZoneID, int iRace,
		int iVoc, int iVoc_Sub, int iSex, int iLV, int iLV_Sub );
	virtual void RL_RoleEnterZone( int ServerID, const char* pszAccount, int iDBID, int iZoneID, int iIsLogin );
};


#endif