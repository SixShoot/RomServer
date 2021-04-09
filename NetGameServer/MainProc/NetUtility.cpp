#include "NetUtility.h"
#include "..\netwalker_member\Net_Script\NetSrv_Script.h"

void CNetUtility::OnEvent_RoleDead ( RoleDataEx* pKiller, RoleDataEx* pDeader )
{
	//CNetSrv_Script::OnEvent_RoleDead( pKiller, pDeader );
}
//-------------------------------------------------------------------------------------
void CNetUtility::OnEvent_RoleLogin ( RoleDataEx* pRole )
{
	CNetSrv_Script::OnEvent_RoleLogin( pRole );
}
//-------------------------------------------------------------------------------------
void CNetUtility::OnEvent_RoleMove ( RoleDataEx* pRole )
{
	if( pRole->PlayerTempData->CastData.iEventHandle != -1 )
	{
		CNetSrv_Script::OnEvent_RoleMove( pRole );
	}
}
//-------------------------------------------------------------------------------------
void CNetUtility::OnEvent_RoleCast ( RoleDataEx* pRole )
{
	if( pRole->PlayerTempData->CastData.iEventHandle != -1 )
	{
		CNetSrv_Script::OnEvent_RoleCast( pRole );
	}
}
//-------------------------------------------------------------------------------------
void CNetUtility::OnEvent_RoleBeHit ( RoleDataEx* pRole )
{
	if( pRole->PlayerTempData->CastData.iEventHandle != -1 )
	{
		CNetSrv_Script::OnEvent_RoleBeHit( pRole );
	}
}