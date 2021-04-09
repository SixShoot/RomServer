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
	EM_ClientStatus_Alive			= 0,	// 選好角色, 且活動於世界
	EM_LOADINGACCOUNT				= 1,
	EM_CREATINGROLE					= 2,
	EM_WAITSELROLE					= 3,
	
	EM_ClientStatus_WaitLeaveWorld			= 4,	// 角色已斷線, 但有進入 Zone Server 的角色, 等待 ZoneSrv 回應
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
	//map< int , RoleData* >	m_mapRoleData;			// 照 field 排序
	vector< PlayerRoleData* >		m_vecCreatingRole;	// 新創的角色, 等待 DataCenter 回傳角色資料 DBID 
	map< int , PlayerRoleData* >	m_mapRoleData;		// 照 DBID 排序

	//RoleData*						m_pSelectedRole;	// 進入遊戲的角色
	int								m_iSelectedRoleDBID;
	//int								m_iZoneID;
	set< int >						m_setZone;			// 紀錄目前角色進入的世界

	int								m_iLoadCount;		// 當帳號 Logout 後, Master 主動 Load Account 次數
	bool							m_bEnterWorld;
	int								m_iLastRespond;		// 上次回應的時間

//	int								m_iPlayTimeQuota;	// 防沉迷可玩時間
	bool							m_bPlayControl;		// 未成年防沉迷帳號

	bool							m_bOnline;			// 是否上線中
	int								m_iActive;
	PlayerAccountBaseInfoStruct		m_AccountBase;

	bool							m_bLoadedRole;	// 是否讀取了角色資料

	int								m_iIDCheckZonePlayer;
	
	int								m_iLoginTime;
	string							m_iLoginIP;

	bool							m_bCheckSecondPasswordOK;	
	std::string						m_CaptchaString;			//暫存的驗證碼字串
	int								m_LastCaptchSendTime;		//最後送出驗證碼的時間
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
	// 記錄目前在線上的帳號有那些


	map< int ,		ClientInfo* >	m_mapClientInfo;	// Index by ClientID
	map< int,		ClientInfo* >	m_mapDBID;			// 記錄那些人物是屬於特定帳號的
														// 這些只紀錄實際進入遊戲的角色

	//-----------------------------------------------------------------------
	RecycleBin< ClientInfo >		m_rClientInfo;		// 新建及回收 ClientInfo 的資料庫
	RecycleBin< PlayerRoleData >	m_rRole;			// 人物資料

};
//-----------------------------------------------------------------------