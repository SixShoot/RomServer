#include "../Net_ServerList/Net_ServerList_Child.h"
#include "./NetMaster_ZoneStatus.h"	


#include "PG/PG_Login.h"
#include "PG/PG_BTI.h"
#include "PG/PG_TALK.h"

//-------------------------------------------------------------------
CNetMaster_ZoneStatus*	CNetMaster_ZoneStatus::m_pThis		= NULL;

//-------------------------------------------------------------------
CNetMaster_ZoneStatus::CNetMaster_ZoneStatus(void)
{
}
//-------------------------------------------------------------------
CNetMaster_ZoneStatus::~CNetMaster_ZoneStatus(void)
{
}
//-------------------------------------------------------------------
bool CNetMaster_ZoneStatus::_Init()
{
	_Net->RegOnSrvPacketFunction			( EM_PG_ZoneStatus_L2M_ZoneSrvReg,				OnPacket_ZoneSrvReg				);
	_Net->RegOnSrvPacketFunction			( EM_PG_ZoneStatus_L2M_ZoneSrvOkay,				OnPacket_ZoneSrvOkay			);
	_Net->RegOnSrvPacketFunction			( EM_PG_ZoneStatus_L2M_RoleEnterZone,			OnPacket_RoleEnterZone			);
	_Net->RegOnSrvPacketFunction			( EM_PG_ZoneStatus_L2M_RoleLeaveZone,			OnPacket_RoleLeaveZone			);
	_Net->RegOnSrvPacketFunction			( EM_PG_ZoneStatus_L2M_RoleLeaveWorld,			OnPacket_RoleLeaveWorld			);
	_Net->RegOnSrvPacketFunction			( EM_PG_ZoneStatus_L2M_KickZonePlayerResult,	OnPacket_KickZonePlayerResult	);
	
	//_Net->RegOnSrvPacketFunction			( EM_PG_BTI_BTI2M_SetPlayerHours,				OnPacket_SetPlayerHours			);
	_Net->RegOnSrvPacketFunction			( EM_PG_Nonage_Nonage2M_SetPlayerHours,			OnPacket_SetPlayerHours			);
	
	
	return false;
}
//-------------------------------------------------------------------
bool CNetMaster_ZoneStatus::_Release()								{ return false;	}
//-------------------------------------------------------------------
void CNetMaster_ZoneStatus::OnPacket_SetPlayerHours( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_Nonage_Nonage2M_SetPlayerHours );
	m_pThis->On_BTI_SetPlayerHours( iServerID, pPacket->Account, pPacket->iHours );
}
//-------------------------------------------------------------------
void CNetMaster_ZoneStatus::OnPacket_KickZonePlayerResult		( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_ZoneStatus_L2M_KickZonePlayerResult );
	m_pThis->On_KickZonePlayerResult( iServerID, pPacket->iZoneID, pPacket->szAccountName.Begin(), pPacket->iResult );
}
//-------------------------------------------------------------------
void CNetMaster_ZoneStatus::OnPacket_ZoneSrvReg		( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_ZoneStatus_L2M_ZoneSrvReg );
	m_pThis->On_ZoneSrvReg( iServerID, pPacket->iZoneID, pPacket->iZoneItemID, pPacket->iRemainCounter );
}
//-------------------------------------------------------------------
void CNetMaster_ZoneStatus::ZoneSrvRegResult			( int iServerID, 	EM_ZoneSrvRegResult	emResult, int iZoneItemID )
{
	PG_ZoneStatus_M2L_ZoneSrvRegResult Packet;
	
	Packet.emResult			= emResult;
	Packet.iZoneItemID		= iZoneItemID;

	_Net->SendToSrv( iServerID, sizeof( PG_ZoneStatus_M2L_ZoneSrvRegResult ), &Packet );
}
//-------------------------------------------------------------------
void CNetMaster_ZoneStatus::RequestZoneSrvReg			( int iServerID )
{
	PG_ZoneStatus_M2L_RequestZoneSrvReg Packet;

	_Net->SendToSrv( iServerID, sizeof( PG_ZoneStatus_M2L_RequestZoneSrvReg ), &Packet );
}

//-------------------------------------------------------------------
void CNetMaster_ZoneStatus::OnPacket_ZoneSrvOkay	( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_ZoneStatus_L2M_ZoneSrvOkay );

	m_pThis->On_ZoneSrvOkay( iServerID, pPacket->iZoneID );
}

//-------------------------------------------------------------------
void CNetMaster_ZoneStatus::OnPacket_RoleEnterZone			( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_ZoneStatus_L2M_RoleEnterZone );
	m_pThis->On_RoleEnterZone( iServerID, pPacket->iLocalID, pPacket->iDBID );
}
//-------------------------------------------------------------------
void CNetMaster_ZoneStatus::OnPacket_RoleLeaveZone			( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_ZoneStatus_L2M_RoleLeaveZone );
	m_pThis->On_RoleLeaveZone( iServerID, pPacket->iLocalID, pPacket->iDBID );
}
//-------------------------------------------------------------------
void CNetMaster_ZoneStatus::OnPacket_RoleLeaveWorld			( int iServerID , int iSize , PVOID pData ) 
{
	M_PACKET( PG_ZoneStatus_L2M_RoleLeaveWorld );
	m_pThis->On_RoleLeaveWorld( iServerID, pPacket->iLocalID, pPacket->iDBID, pPacket->emEvent, pPacket->iExpToNextLevel );
}
//-------------------------------------------------------------------
void CNetMaster_ZoneStatus::SendLeaveWorld ( int iClientID, int iDBID )
{
	PG_Login_M2C_LeaveWorld	Packet;

	Packet.iDBID = iDBID;

	_Net->SendToCli( iClientID, sizeof( PG_Login_M2C_LeaveWorld ), &Packet );
}
//-------------------------------------------------------------------
bool CNetMaster_ZoneStatus::KickZonePlayer( int iSrvID, int iZoneID, const char* pszAccountName )
{
	PG_ZoneStatus_M2L_KickZonePlayer Packet;

	//MyStrcpy( Packet.szAccountName, pszAccountName, sizeof( Packet.szAccountName ) );
	Packet.szAccountName	= pszAccountName;

	// 檢查 iZoneID 是否仍存在

	 _Net->SendToSrv( iSrvID, sizeof( PG_ZoneStatus_M2L_KickZonePlayer ), &Packet );

	 return true;
}
//-------------------------------------------------------------------
void CNetMaster_ZoneStatus::SendMsgToAllPlayer( const char* Content )
{
	PG_Talk_LtoC_RunningMsgEx Send;
	Send.Type = EM_RunningMsgExType_Monitor;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1; 
	//SendToAllCli( Send.Size() , &Send );	
	SendToAllPlayer( Send.Size() , &Send );	
}
