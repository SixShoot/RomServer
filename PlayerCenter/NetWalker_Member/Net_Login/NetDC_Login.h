//	Master Server
 
//	a. ���ѵ� Client �Ҧ�������
//	b. �w���ˬd Client �O�_���b�u�W


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
			// �i�D Master �ӱb���@���h�ֵ��H�����

		static void			ReplyRoleData			( int iServerID, int iClientID, int iDBID, PlayerRoleData* pRole );
			// �ϥθӨ禡�̧ǱN�����ưe�X

        static void			LoadAccountFailed		( int iServerID, int iClientID, EM_LoadAccountFailed emErrID );
			//(�ץ�)�p�G���ѭn�^��

	virtual	void		On_CreateRole				( int iServerID, int iClientID, string sAccountName, PlayerRoleData* pRole ) = 0;
		
		static void			CreateRoleResult		( int iServerID, int iClientID, EM_CreateRoleResult emResult, string sRoleName, int iDBID , int GiftItemID[5] , int GiftItemCount[5] , int Job , int Job_Sub , int Lv , int Lv_Sub );
		static void			CreateRoleResult		( int iServerID, int iClientID, EM_CreateRoleResult emResult, string sRoleName, int iDBID  );
			// �p�G���� result �ǥ���, iDBID �� 0,

	virtual	void		On_DeleteRole				( int iServerID, int iClientID, string sAccountName, int iDBID, string sRoleName , bool IsImmediately ) = 0;
		
		static void			DeleteRoleResult		( int iServerID, int iClientID, EM_DeleteRoleResult emResult, int iDBID );
			// �p�G���� result �ǥ���, iDBID �� 0, szRoleName �� ""
		
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

