#pragma once

#include "../../Mainproc/Global.h"
#include "PG/PG_ZoneStatus.h"

class CNetMaster_ZoneStatus : public Global
{
public:
	CNetMaster_ZoneStatus(void);
	~CNetMaster_ZoneStatus(void);

protected:
	//-------------------------------------------------------------------
	static CNetMaster_ZoneStatus*			m_pThis;

	static bool	_Init();
	static bool	_Release();

	// from LocalServer
	static void	OnPacket_ZoneSrvReg				( int iServerID , int iSize , PVOID pData );
	static void	OnPacket_ZoneSrvOkay			( int iServerID , int iSize , PVOID pData );
	static void	OnPacket_RoleEnterZone			( int iServerID , int iSize , PVOID pData );
	static void	OnPacket_RoleLeaveZone			( int iServerID , int iSize , PVOID pData );
	static void	OnPacket_RoleLeaveWorld			( int iServerID , int iSize , PVOID pData );
	static void	OnPacket_KickZonePlayerResult	( int iServerID , int iSize , PVOID pData );
	static void	OnPacket_SetPlayerHours			( int iServerID , int iSize , PVOID pData );

	



public:
	//-------------------------------------------------------------------
	//  Local Server Reg
	//-------------------------------------------------------------------
	static	void RequestZoneSrvReg				( int iServerID ); 	

	virtual void On_ZoneSrvReg					( int iServerID, int iZoneID, int iZoneItemID, int iRemainCounter ) = 0;
	
		static	void ZoneSrvRegResult			( int iServerID, 	EM_ZoneSrvRegResult	emResult, int iZoneItemID );

	virtual void On_ZoneSrvOkay					( int iServerID, int iZoneID ) = 0;
		// 告訴 Master ZoneSrv 準備好了

	//-----------------------------------------------
	virtual void On_RoleLeaveWorld				( int iServerID, int iZoneSrvID, int iDBID, EM_RoleLeaveWorld emEvent, int iExpToNextLevel ) = 0;

		static void SendLeaveWorld				( int iClientID, int iDBID );

	//-----------------------------------------------
	virtual void On_RoleEnterZone				( int iSrvID, int iZoneSrvID, int iDBID ) = 0;
	virtual void On_RoleLeaveZone				( int iSrvID, int iZoneSrvID, int iDBID ) = 0;


	virtual void	On_KickZonePlayerResult		( int iSrvID, int iZoneID, const char* pszAccountName, int iResult ) = 0;
	static bool		KickZonePlayer				( int iSrvID, int iZoneID, const char* pszAccountName );


	virtual	void	On_BTI_SetPlayerHours		( int iSrvID, const char* pszAccountName, int iHours ) = 0;


	static void		SendMsgToAllPlayer			( const char* Content );


};

