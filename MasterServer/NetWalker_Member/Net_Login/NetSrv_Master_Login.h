//	Master Server
 
//	a. 提供給 Client 所有角色資料
//	b. 定時檢查 Client 是否仍在線上 .... 定義封包給 Client 所在的 Local Server , 並要求 Local Server 回應.

 
//	a.	void CNetSrv_MasterServer_Child::On_ClientConnect		( int iClientID , string sAccount ) 
//		發現重覆帳號處置
//	b.	程式中的 new 集中管理
//	c.	Client 重覆發送 SelectRole CreateRole DeleteRole


#pragma once

//#include "NetWaker/GSrvNetWaker.h"
#include "../../mainproc/Global.h"
#include "PG/PG_Login.h"
#include "../../mainproc/Captcha/CaptchaGenerator.h"

class CNetSrv_MasterServer : public Global
{
public:
	CNetSrv_MasterServer(void);
	~CNetSrv_MasterServer(void);

	//-------------------------------------------------------------------
	static CNetSrv_MasterServer*		m_pThis;

	static bool	_Init();
	static bool	_Release();

	static void	_OnLogin		( string Info );						// loging to switch server
	static void	_OnLogout		();										// logout
	static void	_OnLogFailed	( SYSLogFailedENUM );

	static void	_OnCliConnect	( int iClientID , string sAccount );	// Clinet connect to servers
	static void	_OnCliDisconnect( int iClientID );

	// from DataCenter
	static void	OnPacket_RoleData				( int iServerID , int iSize , PVOID pData );
	static void	OnPacket_RoleDataCount			( int iServerID , int iSize , PVOID pData );
	static void	OnPacket_LoadAccountFailed		( int iServerID , int iSize , PVOID pData );
	static void	OnPacket_DeleteRoleResult		( int iServerID , int iSize , PVOID pData );
	static void	OnPacket_CreateRoleResult		( int iServerID , int iSize , PVOID pData );


	// from Client
	static void OnPacket_LoadRoleData			( int iClientID , int iSize , PVOID pData );
	static void	OnPacket_DeleteRole				( int iClientID , int iSize , PVOID pData );
	static void	OnPacket_CreateRole				( int iClientID , int iSize , PVOID pData );
	static void	OnPacket_SelectRole				( int iClientID , int iSize , PVOID pData );
	static void	OnPacket_SelectRole_EnterZone1	( int iClientID , int iSize , PVOID pData );
	

	// from LocalServer
	static void	OnPacket_SetRoleToWorldResult	( int iServerID , int iSize , PVOID pData );

	static void	_PG_Login_C2M_DelRoleRecoverRequest( int iServerID , int iSize , PVOID pData );
	static void	_PG_Login_C2M_SetSecondPassword	( int iServerID , int iSize , PVOID pData );

	static void	_PG_Login_C2M_ZoneLoadingRequest( int iServerID , int iSize , PVOID pData );
	static void	_PG_Login_C2M_APEXTOSERVER		( int iServerID , int iSize , PVOID pData );

	
	//static void				_PGTest_CliToGSrv	( int Sockid , int Size , void* Data );
	//static void				_PGTest_GSrvToGSrv	( int Sockid , int Size , void* Data );

	static void _PG_Login_C2M_ChangeNameRequest	( int iClientID , int iSize , PVOID pData );
	static void	_PG_Login_D2M_ChangeNameResult	( int iClientID , int iSize , PVOID pData );

	static void	_PG_Login_LtoM_CloseMaster		( int iClientID , int iSize , PVOID pData );

	static void	_PG_Login_CtoM_SecondPassword	( int iClientID , int iSize , PVOID pData );

	static void	_PG_Login_PtoM_GetObjCountResult( int iClientID , int iSize , PVOID pData );
	static void	_PG_Login_CtoM_GameGuardReport	( int iClientID , int iSize , PVOID pData );

	static void	_PG_Login_CtoM_CreateVivoxAccount	( int iClientID , int iSize , PVOID pData );

	static void	_PG_Login_CtoM_SecondPassword2	( int iClientID , int iSize , PVOID pData );
	static void	_PG_Login_CtoM_CaptchaRefresh	( int iClientID , int iSize , PVOID pData );
	static void	_PG_Login_CtoM_CaptchaReply		( int iClientID , int iSize , PVOID pData );
	//-------------------------------------------------------------------	
public:
	//-------------------------------------------------------------------
	//  Login
	//-------------------------------------------------------------------

	// Begin to get account info

	virtual void On_ClientConnect			( int iClientID , string sAccount ) = 0;
	
		static	void LoginResult			( int iClientID, EM_LoginResult emResult );
		//static	void Request_DC_SampleRoleData( string sAccount, int iRoleID );

	virtual void On_LoadRoleData			( int iClientID, const char* pszAccount ) = 0;
		virtual void LoadAccount				( int iClientID, string sAccount ) = 0;
		
			// 向 DataCenter 索取某帳號的角色資料
			// iRoleID	== -1 表要求所有角色資料 , 否則的話則索取該 iRoleID 的角色資料

	virtual void On_ClientDisconnect		( int iClientID ) = 0;
		//static void DeleteClientInfo		( int iClientID );
	//-----------------------------------------------
	virtual void On_RoleDataCount			( const char* pszAccountID, int iClientID, int iRoleCount , PlayerAccountBaseInfoStruct& AccountBase, bool bCache ) = 0;
	virtual void On_RoleData				( const char* pszAccountID, int iClientID, int iDBID, PlayerRoleData* pRole, bool bCache ) = 0;
		static void SendClientRoleData		( int iClientID, int iDBID, SampleRoleData* pSamepleRoleData );
		static void SendClientRoleDataEnd	( int iClientID , int iParallelZoneCount );
	//-----------------------------------------------
	virtual void On_LoadAccountFailed		( int iClientID, EM_LoadAccountFailed emErrID ) = 0;

	static	void LoadDataEvent				( int iClientID, EM_LoadDataEvent emEvent, int iValue );

	//-----------------------------------------------
	virtual void On_DeleteRoleResult		( int iClientID, int iDBID, EM_DeleteRoleResult emResult  ) = 0;
		
		static void DeleteRoleResult		( int iClientID, int iDBID, EM_DeleteRoleResult emResult );
	//-----------------------------------------------
	virtual void On_CreateRoleResult		( int iClientID, EM_CreateRoleResult emResult, const char* pszRoleName, int iDBID , int GiftItemID[5] , int GiftItemCount[5] , int Job , int Job_Sub , int Lv , int Lv_Sub ) = 0;
		
		static void CreateRoleResult		( int iClientID, int iDBID, EM_CreateRoleResult emResult, SampleRoleData* pSampleRoleData , int GiftItemID[5] , int GiftItemCount[5] );
		static void CreateRoleResult		( int iClientID, int iDBID, EM_CreateRoleResult emResult, SampleRoleData* pSampleRoleData );
	//-----------------------------------------------	
	virtual void On_CreateRole				( int iClientID, const char* pszAccount, int iFieldIndex, CreateRoleData* pCreateRoleData ) = 0;
		
		static void CreateRole				( int PlayerCenterID , int iClientID, const char* pszAccountName, PlayerRoleData* pNewRole );

			// 向 DataCenter 要求建立人物
	//-----------------------------------------------
	virtual void On_SelectRole				( int iClientID, const char* pszAccount, int iDBID , int ParallelZoneID , bool EnterZone1 ) = 0;
		
		static void SetRoleToWorld			( int iClientID, PlayerRoleData* pRole , int ParallelZoneID , bool EnterZone1 );
	//-----------------------------------------------
	virtual void On_DeleteRole				( int iClientID, const char* pszAccount, int iDBID , bool IsImmediately , char* Password ) = 0;
		
		static void DeleteRole				( int PlayerCenterID , int iClientID, const char* pszAccountName, const char* pszRoleName, int iDBID , bool IsImmediately );
	//-----------------------------------------------
	virtual void On_SetRoleToWorldResult	( int iServerID, int iClientID, int iDBID, EM_SelectRoleResult emResult ) = 0;
		
		static void SelectRoleResult		( int iClientID, int iDBID, EM_SelectRoleResult emResult );
		static void	SendEnterWorld			( int iClientID );

	virtual void On_DelRoleRecoverRequest	( int CliNetID , int ProxyID , int DBID ) = 0;
		static void	DelRoleRecoverRequest	( int PlayerCenterID , int CliNetID , int ProxyID , int DBID );
	//-----------------------------------------------

	static void	SC_SetPasswordRequest( int CliNetID );
		virtual void On_SetSecondPassword( int CliNetID , char* Password ) = 0;
			static void SD_SetSecondPassword( int PlayerCenterID , const char* Account , const char* Password );
	//	virtual void OnCli_GetRoleSetting		( );
	

	// 
	//virtual void Test_CliToGSrv( int CliID  , int Data ) = 0;
	//virtual void GetTest_GSrvToGSrv( int CliID  , int Data ) = 0;

	//virtual void GetOtherGSrvMsg( int SrvID  , int Data ) = 0;
	//------------------------------------------------
	virtual void On_ZoneLoadingRequest( int CliNetID , int ZoneID ) = 0;
		static void ZoneLoading( int CliNetID , int ZoneID , int MaxZoneCount , vector<int>& Count );

	// BTI
	static void	SL_BTI_RolePlayHours			( int iSrvID, int iRoleDBID, int iHours );


	// APEX
	//----------------------------------------------------------------------------------
	static	void	SC_APEXTOCLIENT				( int CliNetID, int iSize, char* pData );
	static	void	ApexKickPlayer				( int CliNetID );

	virtual void	On_ApexDataFromClient		( int CliNetID, int iSize, PVOID pData ) = 0;

	//////////////////////////////////////////////////////////////////////////
	//修正角色名字
	virtual void RC_ChangeNameRequest( int CliNetID , int DBID , char* NewRoleName ) = 0;
		static void SD_ChangeNameRequest ( int PlayerCenterID , int CliNetID , int DBID , char* NewRoleName );
			virtual void RD_ChangeNameResult ( int CliNetID , int DBID , char* NewRoleName , ChangeNameResultENUM Result ) = 0;
				static void SC_ChangeNameResult ( int CliNetID , int DBID , char* NewRoleName , ChangeNameResultENUM Result );

	virtual void	RM_CloseMaster( ) = 0;

	static void SC_MaxRoleCount( int CliNetID , int RoleCount );
	static void SD_ReserveRoleRequest( int PlayerCenterID , int CliNetID , const char* Account );
	//////////////////////////////////////////////////////////////////////////
	//登入二次密碼
	static void SC_SecondPasswordRequest( int CliNetID );
		virtual void RC_SecondPassword( int CliNetID , const char* Password ) = 0;
			static void SC_SecondPasswordResult( int CliNetID , bool Result );

	//登入二次密碼 - 包含驗證碼
	static	void SC_SecondPasswordRequest2( int CliNetID );
	virtual void RC_SecondPassword2( int CliNetID , const char* Password) = 0;
	static	void SC_SecondPasswordResult2( int CliNetID , bool Result );

	static	void SC_CaptchaCheck( int CliNetID , void* Captcha, unsigned long CaptchaSize );
	virtual void RC_CaptchaRefresh( int CliNetID) = 0;
	virtual void RC_CaptchaReply( int CliNetID , const char* Captcha) = 0;
	static	void SC_CaptchaCheckResult( int CliNetID , int Result );
	//////////////////////////////////////////////////////////////////////////
	static void	SD_SetCheckSecondPasswordResult			( int PlayerCenterID , int iClientID, const char* pszAccount, int iResult );

	static void GetObjCount( int PlayerCenterID );
		virtual void OnGetObjCountResult( int iSrvID, int iCount ) = 0;

	virtual void	OnGameGuardReport		( int iClientID, int iType, int iSize, const char* pszReport ) = 0;


	static	void	SC_VivoxAccount			( int iClientID, const char* pszAccount, const char* pszPassword, bool bCreateResult );
	static	void	SC_VivoxInfo			( int iClientID, const char* pszUrl, const char* pszCountryCode );
	virtual	void	OnCreateVivoxAccount	( int iClientID, const char* pszAccount ) = 0;

///*..*/	CVivox									m_vivox;
	static CCaptchaGenerator*				m_CaptchaGenerator;
};
