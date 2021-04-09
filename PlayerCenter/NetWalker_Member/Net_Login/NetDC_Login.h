//	Master Server
 
//	a. 提供給 Client 所有角色資料
//	b. 定時檢查 Client 是否仍在線上


#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_Login.h"

class CNetDC_Login : public Global
{
public:
	CNetDC_Login(void);
	~CNetDC_Login(void);
	//-------------------------------------------------------------------
	static int					m_iSrvID;
	static CNetDC_Login*		m_pThis;

protected:

	static bool				_Init();
	static bool				_Release();

	static void				OnPacket_LoadAccountData					( int iServerID , int iSize , PVOID pData );
	static void				OnPacket_CreateRole							( int iServerID , int iSize , PVOID pData );
	static void				OnPacket_DeleteRole							( int iServerID , int iSize , PVOID pData );
	static void				_PG_Login_M2D_DelRoleRecoverRequest			( int iServerID , int iSize , PVOID pData );
	static void				_PG_Login_M2D_SetSecondPassword				( int iServerID , int iSize , PVOID pData );
	static void				_PG_Login_M2D_ChangeNameRequest				( int iServerID , int iSize , PVOID pData );
	static void				_PG_Login_MtoD_ReserveRoleRequest			( int iServerID , int iSize , PVOID pData );
	static void				_PG_Login_MtoP_GetObjCount					( int iServerID , int iSize , PVOID pData );
	static void				_PG_Login_M2D_SetCheckSecondPasswordResult	( int iServerID , int iSize , PVOID pData );

	

	//-------------------------------------------------------------------	
public:
	//-------------------------------------------------------------------
	//  Login
	//------------------------------------------------------------------
    
	virtual	void		On_LoadAccount				( int iServerID, int iClientID, string sAccount ) = 0;

		static void			ReplyRoleDataCount		( int iServerID, int iClientID, int iRoleCount , const char* Account , PlayerAccountBaseInfoStruct& AccountBase );
			// 告訴 Master 該帳號共有多少筆人物資料

		static void			ReplyRoleData			( int iServerID, int iClientID, int iDBID, PlayerRoleData* pRole );
			// 使用該函式依序將角色資料送出

        static void			LoadAccountFailed		( int iServerID, int iClientID, EM_LoadAccountFailed emErrID );
			//(修正)如果失敗要回傳

	virtual	void		On_CreateRole				( int iServerID, int iClientID, string sAccountName, PlayerRoleData* pRole ) = 0;
		
		static void			CreateRoleResult		( int iServerID, int iClientID, EM_CreateRoleResult emResult, string sRoleName, int iDBID , int GiftItemID[5] , int GiftItemCount[5] , int Job , int Job_Sub , int Lv , int Lv_Sub );
		static void			CreateRoleResult		( int iServerID, int iClientID, EM_CreateRoleResult emResult, string sRoleName, int iDBID  );
			// 如果失敗 result 傳失敗, iDBID 傳 0,

	virtual	void		On_DeleteRole				( int iServerID, int iClientID, string sAccountName, int iDBID, string sRoleName , bool IsImmediately ) = 0;
		
		static void			DeleteRoleResult		( int iServerID, int iClientID, EM_DeleteRoleResult emResult, int iDBID );
			// 如果失敗 result 傳失敗, iDBID 傳 0, szRoleName 傳 ""
		
	virtual void		On_DelRoleRecoverRequest( int NetID , int ProxyID , int DBID ) = 0;
		static void			DelRoleRecoverResult( int CliNetID , int CliProxyID , int DBID , bool Result );

	virtual void		On_SetSecondPassword( char* Account , char* Password ) = 0;

	virtual void		RM_ChangeNameRequest( int iServerID , int CliNetID , int DBID , char* RoleName ) = 0;
		static void			SM_ChangeNameResult( int iServerID , int CliNetID , int DBID , const char* RoleName , ChangeNameResultENUM Result );


	virtual void		RM_ReserveRoleRequest( int iServerID , int CliNetID , int CliProxyID , const char* Account_ID ) = 0;
		static void			SC_ReserveRole( int CliNetID , int CliProxyID , ReserverRoleStruct&	Info );

	virtual void		RM_GetObjCount				( int iServerID ) = 0;
		static	void		SM_GetObjCountResult	( int iServerID, int iObjCount );

	virtual void		RM_SetCheckSecondPasswordResult( int iServerID, int iClientID, const char* szAccountName, int iResult ) = 0;
};

