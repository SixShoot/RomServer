#pragma once


#include <map>
#include <deque>

//#include "..\NetWaker\GSrvNetWaker.h"
#include ".\NetSrv_Master_Login.h"	
#include ".\RecycleBin\RecycleBin.h"

//#include "..\..\ApexProxy\RaApexServer.h"


/*
enum ClientStatus
{
	EM_LIVINGINWORLD	= 0,
	EM_LOADINGACCOUNT	= 1,
	EM_CREATINGROLE		= 2,
	EM_WAITSELROLE		= 3,
};


struct ClientInfo
{
	ClientInfo() { m_iRoleCount = -1; }
	string							m_sAccountName;
	int								m_iClientID;
	int								m_iRoleCount;
	//vector< RoleData* >	m_vecRole;
	ClientStatus					m_emClientStatus;
	//map< int , RoleData* >	m_mapRoleData;	// 照 field 排序
	vector< RoleData* >		m_vecCreatingRole;		// 新創的角色, 等待 DataCenter 回傳角色資料 DBID 
	map< int , RoleData* >	m_mapRoleData;	// 照 DBID 排序
};
*/


struct StructAccountRoleData
{
	StructAccountRoleData()
	{
		bReady					= false;
		iLoadAccountClientID	= 0;
		iRoleCount				= 0;
		iCreateTick				= 0;
		iTotalRole				= 0;
	}

	bool							bReady;
	int								iTotalRole;
	int								iRoleCount;
	int								iCreateTick;
	map< int, PlayerRoleData >		mapRoleData;
	int								iLoadAccountClientID;
	PlayerAccountBaseInfoStruct		AccountBaseInfo;
};

class CNetSrv_MasterServer_Child : public CNetSrv_MasterServer
{
	

public:
	CNetSrv_MasterServer_Child(void);
	~CNetSrv_MasterServer_Child(void)	{};

	static bool		Init();
	static bool		Release();

	

	static void		OnEvent_DataSrvDisconnect	( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );

	static int		_OnTimeProc_10Sec			( SchedularInfo* Obj , void* InputClass );
	static int		_OnTimeProc_RefreshCaptcha	( SchedularInfo* Obj , void* InputClass );

	static int		OnEvent_WaitDeleteRoleData	( SchedularInfo* pInfo , PVOID pContext );
	static int		OnEvent_WaitAccountReady	( SchedularInfo* pInfo , PVOID pContext );	// 當帳號由 Gamer Server Logout 後, Master 會主動測試帳號是否儲存完畢. 並讀取角色資料
	static int		OnEvent_WaitLoadAccount		( SchedularInfo* pInfo , PVOID pContext );	// 當太多人登入時處理的機制
	static int		OnEvent_CheckAlive			( SchedularInfo* pInfo , PVOID pContext );	// 當 Client 太久沒動作時處理的機制
	static int		OnEvent_VivoxUpdate			( SchedularInfo* pInfo , PVOID pContext );	

	static int		OnEvent_CheckZonePlayer		( SchedularInfo* pInfo , PVOID pContext );

	static int		OnEvent_CheckAccountCache	( SchedularInfo* pInfo , PVOID pContext );
	

	virtual void	On_ClientConnect			( int iClientID , string sAccount );
	virtual void	On_ClientDisconnect			( int iClientID );

	//-----------------------------------------------------------------------
	// Login
	//-----------------------------------------------------------------------

	virtual void	On_LoadRoleData				( int iClientID, const char* pszAccount );
	
	virtual void	On_RoleDataCount			( const char* pszAccount, int iClientID, int iRoleCount , PlayerAccountBaseInfoStruct& AccountBase, bool bCache );
	virtual void	On_RoleData					( const char* pszAccount, int iClientID, int iDBID, PlayerRoleData* pRole, bool bCache );
	virtual void	On_RoleDataEnd				( int iClientID );
	

	virtual void	On_LoadAccountFailed		( int iClientID, EM_LoadAccountFailed emErrID );

	virtual void	On_DeleteRoleResult			( int iClientID, int iFieldIndex, EM_DeleteRoleResult emResult );

	virtual void	On_CreateRoleResult			( int iClientID, EM_CreateRoleResult emResult, const char* pszRoleName, int iDBID , int GiftItemID[5] , int GiftItemCount[5] , int Job , int Job_Sub , int Lv , int Lv_Sub);

	virtual void	On_CreateRole				( int iClientID, const char* pszAccount, int iFieldIndex, CreateRoleData* pCreateRoleData );

	virtual void	On_SelectRole				( int iClientID, const char* pszAccount, int iDBID , int ParallelZoneID , bool EnterZone1 );

	virtual void	On_DeleteRole				( int iClientID, const char* pszAccount, int iDBID , bool IsImmediately , char* Password );

	virtual void	On_SetRoleToWorldResult		( int iServerID, int iClientID, int iFieldIndex, EM_SelectRoleResult emResult );

	virtual void	On_DelRoleRecoverRequest	( int CliNetID , int ProxyID , int DBID );

	virtual void	On_SetSecondPassword		( int CliNetID , char* Password );

	virtual void	On_ZoneLoadingRequest		( int CliNetID , int ZoneID );

	virtual void	OnGetObjCountResult			( int iSrvID, int iCount );

	virtual void	OnGameGuardReport			( int iClientID, int iType, int iSize, const char* pszReport );

	static	void	GetVivoxPassword			(const char* pszAccount, std::string& sPassword );

	//-----------------------------------------------------------------------
	// APEX
	//-----------------------------------------------------------------------

	virtual void	On_ApexDataFromClient		( int CliNetID, int iSize, PVOID pData );

	//-----------------------------------------------------------------------

	ItemFieldStruct GetItemStruct				( int iObjID, int iCount );

	bool			CheckWaitList				();

	//-----------------------------------------------------------------------
	//修正角色名字
	//-----------------------------------------------------------------------
	virtual void RC_ChangeNameRequest			( int CliNetID , int DBID , char* NewRoleName );
	virtual void RD_ChangeNameResult			( int CliNetID , int DBID , char* NewRoleName , ChangeNameResultENUM Result );
	virtual void RM_CloseMaster					();

	virtual void LoadAccount					( int iClientID, string sAccount );

	virtual	bool CheckAccountCache				( const char* sAccount );
	virtual void ClearAccountCache				( const char* sAccount );
	//////////////////////////////////////////////////////////////////////////
	//要求二次密碼
	virtual void RC_SecondPassword( int CliNetID , const char* Password );

	virtual void RC_SecondPassword2( int CliNetID , const char* Password);
	virtual void RC_CaptchaRefresh( int CliNetID);
	virtual void RC_CaptchaReply( int CliNetID , const char* Captcha);
			void SetSecondPasswordCheckFailedTimes( int CliNetID, int CheckFailedTimes);
			void CheckSecondPassword2_Ok(int CliNetID);
	//////////////////////////////////////////////////////////////////////////

	virtual	void OnCreateVivoxAccount	( int iClientID, const char* pszAccount );

	
	// Begin to get account info

	// Select role
	//virtual	void On_



	//virtual void Test_CliToGSrv( int CliID  , int Data ) = 0;
	//virtual void GetTest_GSrvToGSrv( int CliID  , int Data ) = 0;

	//virtual void GetOtherGSrvMsg( int SrvID  , int Data ) = 0;

	//-----------------------------------------------------------------------
	//ClientInfo* GetClientInfo				( int		iClientID	);

	//-----------------------------------------------------------------------
	//int								m_iNumLoading;

	static map< string, DWORD >				m_mapLoadQueue;
	static deque< string >					m_vecWaitQueue;


	CRITICAL_SECTION						m_csBTI;

	//CApexServer								m_APEX;


	map< string, StructAccountRoleData* >	m_mapAccountRoleData;
	
	map< string, DWORD >					m_mapWaitQueuePriority;

	static int								m_iObjCount;

	int										m_iAgency[5];




	//map< string , ClientInfo* >	m_mapClientInfoByAccount;
	//map< int	, ClientInfo* >	m_mapClientInfoByClientID;
	/*
	map< int ,		ClientInfo* >	m_mapClientInfo;
	map< string,	int			>	m_mapAccount;

	RecycleBin< ClientInfo >		m_rClientInfo;
	RecycleBin< RoleData >			m_rRole;
	*/

	//map< str
};
