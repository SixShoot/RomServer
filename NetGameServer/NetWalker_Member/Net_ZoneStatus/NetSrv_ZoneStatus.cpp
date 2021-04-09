#include "./NetSrv_ZoneStatus.h"
#include "../Net_ServerList/Net_ServerList_Child.h"

//-------------------------------------------------------------------
CNetSrv_ZoneStatus*	CNetSrv_ZoneStatus::m_pThis		= NULL;

//-------------------------------------------------------------------
CNetSrv_ZoneStatus::CNetSrv_ZoneStatus(void)
{
}
//-------------------------------------------------------------------
CNetSrv_ZoneStatus::~CNetSrv_ZoneStatus(void)
{
}
//-------------------------------------------------------------------
bool CNetSrv_ZoneStatus::_Init()
{

	_Net->RegOnSrvPacketFunction			( EM_PG_Login_M2L_SetRoleToWorld,				OnPacket_SetRoleToWorld			);
	
	_Net->RegOnSrvPacketFunction			( EM_PG_ZoneStatus_M2L_RequestZoneSrvReg,		OnPacket_RequestZoneSrvReg		);
	_Net->RegOnSrvPacketFunction			( EM_PG_ZoneStatus_M2L_ZoneSrvRegResult,		OnPacket_ZoneSrvRegResult		);

//	_Net->RegOnSrvPacketFunction			( EM_PG_Login_M2L_BTIPlayTime,					OnPacket_BTIPlayTime			);
	_Net->RegOnSrvPacketFunction			( EM_PG_Nonage_M2L_HealthTime,					OnPacket_BTIPlayTime			);
	_Net->RegOnSrvPacketFunction			( EM_PG_ZoneStatus_M2L_KickZonePlayer,			OnPacket_KickZonePlayer			);

	
	return false;
}
//-------------------------------------------------------------------
bool CNetSrv_ZoneStatus::_Release()								{ return false;	}
//-------------------------------------------------------------------
void CNetSrv_ZoneStatus::OnPacket_KickZonePlayer		( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_ZoneStatus_M2L_KickZonePlayer );
	m_pThis->On_KickZonePlayer( iServerID, pPacket->szAccountName );
}

void CNetSrv_ZoneStatus::OnPacket_BTIPlayTime			( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_Nonage_M2L_HealthTime );
	m_pThis->On_BTIPlayTime( iServerID, pPacket->iRoleDBID, pPacket->iHealthTime );
}
//-------------------------------------------------------------------
void CNetSrv_ZoneStatus::OnPacket_SetRoleToWorld ( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_Login_M2L_SetRoleToWorld );
	pPacket->Role.SetLink();
	pPacket->Role.DynamicData.TreasureBox = NULL;
	m_pThis->On_SetRoleToWorld( iServerID, pPacket->iClientID, &pPacket->Role );
}
//-------------------------------------------------------------------
void CNetSrv_ZoneStatus::OnPacket_ZoneSrvRegResult	( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_ZoneStatus_M2L_ZoneSrvRegResult );
	m_pThis->On_ZoneSrvRegResult( iServerID, pPacket->emResult, pPacket->iZoneItemID );
}
//-------------------------------------------------------------------
void CNetSrv_ZoneStatus::SetRoleToWorldResult	( int iServerID, int iClientID, int iDBID, EM_SelectRoleResult emResult )
{
	PG_Login_L2M_SetRoleToWorldResult Packet;

	Packet.iClientID	= iClientID;
	Packet.iDBID		= iDBID;
	Packet.emResult		= emResult;

	_Net->SendToSrv( iServerID, sizeof( PG_Login_L2M_SetRoleToWorldResult ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_ZoneStatus::SetRoleEnterZone( int iDBID, int iLocalID )
{
	PG_ZoneStatus_L2M_RoleEnterZone	Packet;

	Packet.iDBID	= iDBID;
	Packet.iLocalID	= iLocalID;

    Global::SendToMaster( sizeof( PG_ZoneStatus_L2M_RoleEnterZone ), &Packet );

	RoleDataEx* pRole = Global::GetRoleDataByDBID( iDBID );

	//send part of role data to datacenter
	if (pRole != NULL)
	{
		PG_ZoneStatus_L2D_RoleEnterZone PacketToD;
		PacketToD.szAccount = pRole->Account_ID();
		PacketToD.iDBID		= pRole->DBID();
		PacketToD.iZoneID	= iLocalID;
		PacketToD.iIsLogin	= pRole->TempData.Attr.Action.Login;

		Global::SendToDBCenter(sizeof(PacketToD), &PacketToD);
	}
}
//-------------------------------------------------------------------
void CNetSrv_ZoneStatus::SetRoleLeaveZone( int iDBID, int iLocalID )
{
	PG_ZoneStatus_L2M_RoleLeaveZone	Packet;

	Packet.iDBID	= iDBID;
	Packet.iLocalID	= iLocalID;

	Global::SendToMaster( sizeof( PG_ZoneStatus_L2M_RoleLeaveZone ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_ZoneStatus::SetRoleLeaveWorld( int iDBID, int iLocalID, EM_RoleLeaveWorld emEvent )
{
	PG_ZoneStatus_L2M_RoleLeaveWorld	Packet;

	Packet.iDBID	= iDBID;
	Packet.iLocalID	= iLocalID;
	Packet.emEvent	= emEvent;

	RoleDataEx* pRole = Global::GetRoleDataByDBID( iDBID );

	if( pRole && pRole->TempData.Attr.Ability  )
	{
		Packet.iExpToNextLevel	= pRole->GetLVExp( pRole->BaseData.Voc ) - pRole->TempData.Attr.Ability->EXP;
	}	

	Global::SendToMaster( sizeof( PG_ZoneStatus_L2M_RoleLeaveWorld ), &Packet );

	//send part of role data to datacenter, for KR middle DB.
	if (pRole != NULL)
	{
		PG_ZoneStatus_L2D_RoleLeaveWorld PacketToD;
		PacketToD.szAccountName = pRole->Account_ID();
		PacketToD.szRoleName	= Global::GetRoleName( pRole );
		PacketToD.iRoleDBID		= pRole->DBID();
		PacketToD.iWorldID		= Global::Net()->GetWorldID();
		PacketToD.iZoneID		= iLocalID;
		PacketToD.iRace			= pRole->BaseData.Race;
		PacketToD.iVoc			= pRole->BaseData.Voc;
		PacketToD.iVoc_Sub		= pRole->BaseData.Voc_Sub;
		PacketToD.iSex			= pRole->BaseData.Sex;

		if ((pRole->BaseData.Voc >= 0) && (pRole->BaseData.Voc < EM_Max_Vocation))
		{
			PacketToD.iLV = pRole->PlayerBaseData->AbilityList[pRole->BaseData.Voc].Level;
		}
		else
		{
			PacketToD.iLV = 0;
		}

		if ((pRole->BaseData.Voc_Sub >= 0) && (pRole->BaseData.Voc_Sub < EM_Max_Vocation))
		{
			PacketToD.iLV_Sub = pRole->PlayerBaseData->AbilityList[pRole->BaseData.Voc_Sub].Level;
		}
		else
		{
			PacketToD.iLV_Sub = 0;
		}

		Global::SendToDBCenter(sizeof(PacketToD), &PacketToD);
	}	
}
//-------------------------------------------------------------------
void CNetSrv_ZoneStatus::ZoneSrvReg					( int iServerID, int iZoneID, int iZoneItemID, int iRemainCount )
{
	PG_ZoneStatus_L2M_ZoneSrvReg Packet;

	Packet.iZoneID			= iZoneID;
	Packet.iZoneItemID		= iZoneItemID;
	Packet.iRemainCounter	= iRemainCount;

	//CNet_ServerList_Child::SendToMaster( sizeof( PG_ZoneStatus_L2M_LocalSrvReg ), &Packet );
	_Net->SendToSrv( iServerID, sizeof( PG_ZoneStatus_L2M_ZoneSrvReg ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_ZoneStatus::OnPacket_RequestZoneSrvReg		( int iServerID , int iSize , PVOID pData )
{
	m_pThis->On_RequestZoneSrvReg( iServerID );
}
//-------------------------------------------------------------------
void CNetSrv_ZoneStatus::ZoneSrvOkay				( int iServerID )
{
	PG_ZoneStatus_L2M_ZoneSrvOkay Packet;

    Packet.iZoneID			= Global::Net_ServerList->m_dwServerLocalID;

	_Net->SendToSrv( iServerID, sizeof( PG_ZoneStatus_L2M_ZoneSrvOkay ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_ZoneStatus::KickZonePlayerResult( int iSrvID, int iZoneID, const char* szAccountName, int iResult )
{
	PG_ZoneStatus_L2M_KickZonePlayerResult	Packet;

	Packet.szAccountName	= szAccountName;
	Packet.iResult			= iResult;
	Packet.iZoneID			= iZoneID;

	_Net->SendToSrv( iSrvID, sizeof( PG_ZoneStatus_L2M_KickZonePlayerResult ), &Packet );
}
//-------------------------------------------------------------------
//要求把Master登入角色關閉
void	CNetSrv_ZoneStatus::SM_CloseMaster				()
{
	PG_Login_LtoM_CloseMaster Send;
	SendToMaster( sizeof(Send) , &Send );
}