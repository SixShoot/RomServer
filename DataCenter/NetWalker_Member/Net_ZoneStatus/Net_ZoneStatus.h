#pragma once
#ifndef	__NET_ZONE_STATUS_H__
#define __NET_ZONE_STATUS_H__

#include "../../MainProc/Global.h"
#include "PG/PG_ZoneStatus.h"

class Net_ZoneStatus : public Global
{
protected:
	static Net_ZoneStatus*	This;

public:
	static bool	Init();
	static bool	Release();

protected:
	static void _PG_ZoneStatus_L2D_RoleLeaveWorld( int ServerID , int Size , void* Data );
	static void _PG_ZoneStatus_L2D_RoleEnterZone( int ServerID , int Size , void* Data );

public:
	virtual void RL_RoleLeaveWorld( int ServerID, const char* pszAccountName, const char* pszRoleName, int iRoleDBID, int iWorldID, int iZoneID, int iRace,
		int iVoc, int iVoc_Sub, int iSex, int iLV, int iLV_Sub ) = 0;
	virtual void RL_RoleEnterZone( int ServerID, const char* pszAccount, int iDBID, int iZoneID, int iIsLogin ) = 0;
};

#endif