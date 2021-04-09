#pragma once

#include <set>
#include <map>
#include <string>

#include "roledata/RoleDataEx.h"
//#include "../../"
#include "./RecycleBin/RecycleBin.h"

enum ClientStatus
{
	EM_ClientStatus_Init			= -1,
	EM_ClientStatus_Alive			= 0,	// ��n����, �B���ʩ�@��
	EM_LOADINGACCOUNT				= 1,
	EM_CREATINGROLE					= 2,
	EM_WAITSELROLE					= 3,
	
	EM_ClientStatus_WaitLeaveWorld			= 4,	// ����w�_�u, �����i�J Zone Server ������, ���� ZoneSrv �^��
	EM_ClientStatus_WaitDelete				= 5,
	EM_ClientStatus_ReloadingData			= 6,
	EM_ClientStatus_LoadingData				= 7,
	EM_ClientStatus_RoleLeaveZone			= 8,
	EM_ClientStatus_WaitEnterZone			= 9,

	EM_ClientStatus_WaitLastAccountLeave		= 10,
	EM_ClientStatus_WaitAskLoadData				= 11,
	EM_ClientStatus_WaitLastAccountLeaveZone	= 12,

	EM_ClientStatus_WaitConnectZone				= 13,
	EM_ClientStatus_InGame						= 14,
};

struct ClientInfo
{
	ClientInfo()	
	{
		Init();					
	}

	void Init()			
	{
		m_iRoleCount = -1;  m_iLoadCount = 0; m_bEnterWorld = false; m_bOnline = false; m_iActive =0;
		m_iIDCheckZonePlayer = -1; 
		m_bPlayControl	= false;
		m_iSelectedRoleDBID = 0;	// m_pSelectedRole = NULL;
		m_bLoadedRole		= false;
		m_bCheckSecondPasswordOK = false;
		m_iLoginTime			= 0;
		m_CaptchaString = "";
		m_LastCaptchSendTime = 0;
	}

	string							m_sAccountName;
	int								m_iClientID;
	int								m_iRoleCount;
	//vector< RoleData* >	m_vecRole;
	ClientStatus					m_emClientStatus;
	//map< int , RoleData* >	m_mapRoleData;			// �� field �Ƨ�
	vector< PlayerRoleData* >		m_vecCreatingRole;	// �s�Ъ�����, ���� DataCenter �^�Ǩ����� DBID 
	map< int , PlayerRoleData* >	m_mapRoleData;		// �� DBID �Ƨ�

	//RoleData*						m_pSelectedRole;	// �i�J�C��������
	int								m_iSelectedRoleDBID;
	//int								m_iZoneID;
	set< int >						m_setZone;			// �����ثe����i�J���@��

	int								m_iLoadCount;		// ��b�� Logout ��, Master �D�� Load Account ����
	bool							m_bEnterWorld;
	int								m_iLastRespond;		// �W���^�����ɶ�

//	int								m_iPlayTimeQuota;	// ���I�g�i���ɶ�
	bool							m_bPlayControl;		// �����~���I�g�b��

	bool							m_bOnline;			// �O�_�W�u��
	int								m_iActive;
	PlayerAccountBaseInfoStruct		m_AccountBase;

	bool							m_bLoadedRole;	// �O�_Ū���F������

	int								m_iIDCheckZonePlayer;
	
	int								m_iLoginTime;
	string							m_iLoginIP;

	bool							m_bCheckSecondPasswordOK;	
	std::string						m_CaptchaString;			//�Ȧs�����ҽX�r��
	int								m_LastCaptchSendTime;		//�̫�e�X���ҽX���ɶ�
};

class CClientManger;
extern CClientManger*				g_pClientManger;
//-----------------------------------------------------------------------
class CClientManger
{
public:
	CClientManger()									{	m_iActiveRole = 0; m_iLoadedRole = 0; }
	static	void		Init						();
	static	void		Release						();

	//int					GetActive					() { return m_mapAccount.size(); }
	//int					GetActive					() { return m_mapAccount.size(); }
	int					GetActive					() { return m_iActiveRole; }
	void				AddActive					() { m_iActiveRole++; }
	void				DelActive					() { if( m_iActiveRole > 0 ) m_iActiveRole--; }


	int					m_iActiveRole;
	int					m_iLoadedRole;

	ClientInfo*			GetClientInfoByID			( int iClientID			);
	ClientInfo*			GetClientInfoByDBID			( int iDBID				);
	ClientInfo*			GetClientInfoByAccount		( string sAccount		);

	void				SetClientInfoByID			( int iClientID,	ClientInfo* pClientInfo );
	void				SetClientInfoByDBID			( int iDBID,		ClientInfo* pClientInfo );
	void				SetClientInfoByAccount		( string sAccount,	ClientInfo* pClientInfo );

	void				EraseClientInfoByID			( int iClientID			)	{ m_mapClientInfo.erase( iClientID );	}
	void				EraseClientInfoByDBID		( int iDBID				)	{ m_mapDBID.erase( iDBID );				}
	void				EraseClientInfoByAccount	( string sAccount		)	{ m_mapAccount.erase( sAccount );		}

	void				ClearRoleFromClientInfo		( ClientInfo* pClientInfo );

	//ClientInfo*			CreateClientInfo			( int iClientID, string sAccount );
	ClientInfo*			CreateClientInfo			();

	
	//void				EraseClientInfo				( ClientInfo* pClient, bool bEraseData	);
	void				DeleteClientInfo			( ClientInfo* pClientInfo );

	void				AddRoleToClientInfo			( ClientInfo* pClient, int iDBID, PlayerRoleData* pRole );
	bool				DelRoleToClientInfo			( ClientInfo* pClient, int iDBID );

	//bool				RoleEnterWorld				( int iDBID, ClientInfo* pClientInfo );
	//bool				RoleLeaveWorld				( int iDBID, ClientInfo* pClientInfo );

	PlayerRoleData*		CreateRoleData				();
	void				DeleteRoleData				( PlayerRoleData* pObj );
	


	//-----------------------------------------------------------------------

	map< string,	ClientInfo*	>	m_mapAccount;		// Index by AccountName, Data as ClientID
	// �O���ثe�b�u�W���b��������


	map< int ,		ClientInfo* >	m_mapClientInfo;	// Index by ClientID
	map< int,		ClientInfo* >	m_mapDBID;			// �O�����ǤH���O�ݩ�S�w�b����
														// �o�ǥu������ڶi�J�C��������

	//-----------------------------------------------------------------------
	RecycleBin< ClientInfo >		m_rClientInfo;		// �s�ؤΦ^�� ClientInfo ����Ʈw
	RecycleBin< PlayerRoleData >	m_rRole;			// �H�����

};
//-----------------------------------------------------------------------