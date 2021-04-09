#include "Net_ZoneStatus.h"

Net_ZoneStatus*	Net_ZoneStatus::This	= NULL;

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
bool Net_ZoneStatus::Init()
{
	_Net->RegOnSrvPacketFunction( EM_PG_ZoneStatus_L2D_RoleLeaveWorld	, _PG_ZoneStatus_L2D_RoleLeaveWorld	);
	_Net->RegOnSrvPacketFunction( EM_PG_ZoneStatus_L2D_RoleEnterZone	, _PG_ZoneStatus_L2D_RoleEnterZone	);
	return true;
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
bool Net_ZoneStatus::Release()
{
	return true;
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_ZoneStatus::_PG_ZoneStatus_L2D_RoleLeaveWorld( int ServerID , int Size , void* Data )
{
	PG_ZoneStatus_L2D_RoleLeaveWorld* pg =(PG_ZoneStatus_L2D_RoleLeaveWorld*)Data;
	This->RL_RoleLeaveWorld( ServerID , pg->szAccountName, pg->szRoleName, pg->iRoleDBID, pg->iWorldID, pg->iZoneID, pg->iRace, pg->iVoc, pg->iVoc_Sub, pg->iSex, pg->iLV, pg->iLV_Sub );
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_ZoneStatus::_PG_ZoneStatus_L2D_RoleEnterZone( int ServerID , int Size , void* Data )
{
	PG_ZoneStatus_L2D_RoleEnterZone* pg =(PG_ZoneStatus_L2D_RoleEnterZone*)Data;
	This->RL_RoleEnterZone( ServerID , pg->szAccount, pg->iDBID, pg->iZoneID, pg->iIsLogin );
}