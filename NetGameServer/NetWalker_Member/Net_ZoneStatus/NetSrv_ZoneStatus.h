#pragma once

#include "../../mainproc/Global.h"
#include "PG/PG_Login.h"
#include "PG/PG_ZoneStatus.h"


class CNetSrv_ZoneStatus : public Global
{
public:
	CNetSrv_ZoneStatus(void);
	~CNetSrv_ZoneStatus(void);

protected:
	//-------------------------------------------------------------------
	static CNetSrv_ZoneStatus*		m_pThis;

	static bool			_Init();
	static bool			_Release();

	// from Master
	static void			OnPacket_SetRoleToWorld			( int iServerID , int iSize , PVOID pData );
	static void			OnPacket_RequestZoneSrvReg		( int iServerID , int iSize , PVOID pData );
	static void			OnPacket_ZoneSrvRegResult		( int iServerID , int iSize , PVOID pData );
	static void			OnPacket_BTIPlayTime			( int iServerID , int iSize , PVOID pData );
	static void			OnPacket_KickZonePlayer			( int iServerID , int iSize , PVOID pData );

public:
	//-------------------------------------------------------------------
	//  Login
	//-------------------------------------------------------------------
	virtual void		On_SetRoleToWorld			( int iServerID, int iClientID, PlayerRoleData* pRole ) = 0;		
	static void				SetRoleToWorldResult	( int iServerID, int iClientID, int iDBID, EM_SelectRoleResult emResult );

	virtual	void		On_RequestZoneSrvReg		( int iServerID ) = 0;
	static void				ZoneSrvReg				( int iServerID, int iZoneID, int iZoneItemID, int iRemainCount );
			// iZoneItemID		����o�e�ұo�쪺 Zone Item ���	, �Y�� LSrv �O�s�}���S�� ZoneItemID , �h�ݵo DF_NO_ZONEITEMID
			// iRemainCount		�᭱�٦��h�� ZoneItemID �٨S���o	�Y�᭱�S���䥦 ZoneItemID �n�o�e, �h�] 0

	virtual void		On_ZoneSrvRegResult			( int iServerID, EM_ZoneSrvRegResult emRegResult, int iZoneItemID ) = 0;

	static	void		ZoneSrvOkay					( int iServerID );

	virtual void		On_BTIPlayTime				( int iServerID, int iRoleDBID, int iHours ) = 0;
	virtual void		On_KickZonePlayer			( int iServerID, const char* szAccountName ) = 0;
	//-------------------------------------------------------------------
	//  Status Check
	//-------------------------------------------------------------------

	static void SetRoleEnterZone					( int iDBID, int iLocalID );
	static void SetRoleLeaveZone					( int iDBID, int iLocalID );
	static void SetRoleLeaveWorld					( int iDBID, int iLocalID, EM_RoleLeaveWorld emEvent );
		// ��H�� �i�J/���} �ӳ� Local Server �q�� Master ���ܸ�ƥ�

	//-------------------------------------------------------------------
	//  Kick Zone Player
	//-------------------------------------------------------------------
	static void			KickZonePlayerResult		( int iSrvID, int iZoneID, const char* szAccountName, int iResult );

	//�n�D��Master�n�J��������
	static void			SM_CloseMaster				();
};
