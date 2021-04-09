#include "Net_ZoneStatusChild.h"
#include "../../AuroraAgentCli/AuroraAgentCli.h"

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
bool Net_ZoneStatusChild::Init()
{
	Net_ZoneStatus::Init();

	if( This != NULL)
		return false;

	This = NEW( Net_ZoneStatusChild );

	return true;
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
bool Net_ZoneStatusChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;

	Net_ZoneStatus::Release();
	return true;
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_ZoneStatusChild::RL_RoleLeaveWorld( int ServerID, const char* pszAccountName, const char* pszRoleName, int iRoleDBID, int iWorldID, int iZoneID, int iRace,
							   int iVoc, int iVoc_Sub, int iSex, int iLV, int iLV_Sub )
{
	if (AuroraAgentCli::This != NULL)
	{
		AuroraAgentCli::This->SAA_PG_AURORA_AGENT_DCtoAA_ROLE_LEAVE_WORLD(pszAccountName, pszRoleName, iRoleDBID, iWorldID, iZoneID, iRace, iVoc, iVoc_Sub, iSex, iLV, iLV_Sub);
	}	
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_ZoneStatusChild::RL_RoleEnterZone( int ServerID, const char* pszAccount, int iDBID, int iZoneID, int iIsLogin )
{
	if (AuroraAgentCli::This != NULL)
	{
		if (iIsLogin > 0)
		{
			AuroraAgentCli::This->SAA_PG_AURORA_AGENT_DCtoAA_QUERY_PC_BENEFIT(pszAccount, iDBID, iZoneID);
		}
	}
}