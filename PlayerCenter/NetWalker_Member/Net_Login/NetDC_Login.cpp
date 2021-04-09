#include "NetDC_Login.h"	
//#include "../ServerList/ServerList_Child.h"
//-------------------------------------------------------------------
int				CNetDC_Login::m_iSrvID		= -1;
CNetDC_Login*	CNetDC_Login::m_pThis		= NULL;

//-------------------------------------------------------------------
CNetDC_Login::CNetDC_Login(void)
{
}
//-------------------------------------------------------------------
CNetDC_Login::~CNetDC_Login(void)
{
}
//-------------------------------------------------------------------
bool CNetDC_Login::_Release()
{

	return false;
}
//-------------------------------------------------------------------
bool CNetDC_Login::_Init()
{

	_Net->RegOnSrvPacketFunction			( EM_PG_Login_M2D_LoadAccountData,		OnPacket_LoadAccountData	);
	_Net->RegOnSrvPacketFunction			( EM_PG_Login_M2D_CreateRole,			OnPacket_CreateRole			);
	_Net->RegOnSrvPacketFunction			( EM_PG_Login_M2D_DeleteRole,			OnPacket_DeleteRole			);

	_Net->RegOnSrvPacketFunction			( EM_PG_Login_M2D_DelRoleRecoverRequest			,_PG_Login_M2D_DelRoleRecoverRequest		);
	_Net->RegOnSrvPacketFunction			( EM_PG_Login_M2D_SetSecondPassword				,_PG_Login_M2D_SetSecondPassword			);

	_Net->RegOnSrvPacketFunction			( EM_PG_Login_M2D_ChangeNameRequest				,_PG_Login_M2D_ChangeNameRequest			);
	_Net->RegOnSrvPacketFunction			( EM_PG_Login_MtoD_ReserveRoleRequest			,_PG_Login_MtoD_ReserveRoleRequest			);

	_Net->RegOnSrvPacketFunction			( EM_PG_Login_MtoP_GetObjCount					,_PG_Login_MtoP_GetObjCount			);
	_Net->RegOnSrvPacketFunction			( EM_PG_Login_M2D_SetCheckSecondPasswordResult	,_PG_Login_M2D_SetCheckSecondPasswordResult			);

	return false;
}
//-------------------------------------------------------------------
void	CNetDC_Login::_PG_Login_M2D_SetCheckSecondPasswordResult	( int iServerID , int iSize , PVOID pData )
{
	PG_Login_M2D_SetCheckSecondPasswordResult* PG = (PG_Login_M2D_SetCheckSecondPasswordResult*)pData;
	m_pThis->RM_SetCheckSecondPasswordResult( iServerID, PG->iClientID, PG->szAccountName, PG->iResult );
}
//-------------------------------------------------------------------
void CNetDC_Login::_PG_Login_MtoP_GetObjCount			( int iServerID , int iSize , PVOID pData )
{
	PG_Login_MtoP_GetObjCount* PG = (PG_Login_MtoP_GetObjCount*)pData;
	m_pThis->RM_GetObjCount( iServerID );
}
//-------------------------------------------------------------------
void CNetDC_Login::_PG_Login_MtoD_ReserveRoleRequest	( int iServerID , int iSize , PVOID pData )
{
	PG_Login_MtoD_ReserveRoleRequest* PG = (PG_Login_MtoD_ReserveRoleRequest*)pData;
	m_pThis->RM_ReserveRoleRequest( iServerID , PG->CliNetID , PG->CliProxyID , PG->Account_ID.Begin() );
}
void CNetDC_Login::_PG_Login_M2D_ChangeNameRequest	( int iServerID , int iSize , PVOID pData )
{
	PG_Login_M2D_ChangeNameRequest* PG = (PG_Login_M2D_ChangeNameRequest*)pData;
	m_pThis->RM_ChangeNameRequest( iServerID , PG->CliNetID , PG->DBID , PG->RoleName );
}

void CNetDC_Login::_PG_Login_M2D_SetSecondPassword	( int iServerID , int iSize , PVOID pData )
{
	PG_Login_M2D_SetSecondPassword* PG = (PG_Login_M2D_SetSecondPassword*)pData;
	m_pThis->On_SetSecondPassword( PG->Account , PG->Password );
}
//-------------------------------------------------------------------
void CNetDC_Login::_PG_Login_M2D_DelRoleRecoverRequest( int iServerID , int iSize , PVOID pData )
{
	PG_Login_M2D_DelRoleRecoverRequest* PG = (PG_Login_M2D_DelRoleRecoverRequest*)pData;
	m_pThis->On_DelRoleRecoverRequest( PG->NetID , PG->ProxyID , PG->DBID );
}
void CNetDC_Login::OnPacket_LoadAccountData( int iServerID , int iSize , PVOID pData )
{
	// 收到 有人要求角色相關資料
	PG_Login_M2D_LoadAccountData* p_ptLoadAccountData = ( PG_Login_M2D_LoadAccountData* )pData;
	m_pThis->On_LoadAccount( iServerID, p_ptLoadAccountData->iClientID, p_ptLoadAccountData->szAccountName );
}
//-------------------------------------------------------------------
void CNetDC_Login::OnPacket_CreateRole( int iServerID , int iSize , PVOID pData )
{
	PG_Login_M2D_CreateRole* p_ptCreateRole = (PG_Login_M2D_CreateRole*)pData;
	m_pThis->On_CreateRole( iServerID, p_ptCreateRole->iClientID, p_ptCreateRole->szAccountName, &p_ptCreateRole->Role );
}
//-------------------------------------------------------------------
void CNetDC_Login::OnPacket_DeleteRole( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_Login_M2D_DeleteRole );

	m_pThis->On_DeleteRole( iServerID, pPacket->iClientID, pPacket->szAccountName, pPacket->iDBID, pPacket->szRoleName , pPacket->IsImmediately );
}
//-------------------------------------------------------------------
void CNetDC_Login::ReplyRoleData( int iServerID, int iClientID, int iDBID, PlayerRoleData* pRole )
{
	Print( LV2 , "ReplyRoleData" , "ClientID =%d DBID=%d" , iClientID , iDBID );

	//fixup friend list 
	{
		pRole->PlayerSelfData.FriendList.FixupData();
	}


	PG_Login_D2M_RoleData ptRoleData;

	ptRoleData.iClientID		= iClientID;
	ptRoleData.iDBID			= iDBID;
	//ptRoleData.Role				= *pRole;
	ptRoleData.Base				= pRole->Base;
	ptRoleData.SelfData			= pRole->SelfData;							//LocalServer 專用
	ptRoleData.BaseData			= pRole->BaseData;
	ptRoleData.PlayerSelfData	= pRole->PlayerSelfData;
	ptRoleData.PlayerBaseData	= pRole->PlayerBaseData;


	//////////////////////////////////////////////////////////////////////////
	//如果顯像裝備資訊沒初始化
	//////////////////////////////////////////////////////////////////////////
	if( ptRoleData.PlayerBaseData.CoolSuitID == -1 )
	{
		ptRoleData.PlayerBaseData.CoolSuitID = 0;
		ptRoleData.PlayerBaseData.CoolSuitList[0].Init();
	}
	//////////////////////////////////////////////////////////////////////////
	//寵物資料 狀態清除
	//////////////////////////////////////////////////////////////////////////
	for( int i = 0 ; i < MAX_CultivatePet_Count ; i++ )
	{
		CultivatePetStruct& PetBase = ptRoleData.PlayerBaseData.Pet.Base[ i ];
		PetBase.EventType = EM_CultivatePetCommandType_None;
	}
	//////////////////////////////////////////////////////////////////////////
	_Net->SendToSrv( iServerID, sizeof( PG_Login_D2M_RoleData ), &ptRoleData );
}
//-------------------------------------------------------------------
void CNetDC_Login::ReplyRoleDataCount( int iServerID, int iClientID, int iRoleCount , const char* Account , PlayerAccountBaseInfoStruct& AccountBase )
{
	Print( LV2 , "ReplyRoleDataCount" , "ClientID =%d RoleCount=%d Account=%s" , iClientID , iRoleCount , Account );
	PG_Login_D2M_RoleDataCount Packet;

	Packet.iClientID	= iClientID;
	Packet.iRoleCount	= iRoleCount;
	Packet.AccountBaseInfo = AccountBase;
	Packet.Account_ID	= Account;

	_Net->SendToSrv( iServerID, sizeof( PG_Login_D2M_RoleDataCount ), &Packet );
}
//-------------------------------------------------------------------
void CNetDC_Login::LoadAccountFailed( int iServerID, int iClientID, EM_LoadAccountFailed emErrID )
{
	PG_Login_D2M_LoadAccountFailed ptLoadAccountFailed;

	ptLoadAccountFailed.iClientID	= iClientID;
	ptLoadAccountFailed.emErrID		= emErrID;
	
	_Net->SendToSrv( iServerID, sizeof( PG_Login_D2M_LoadAccountFailed ), &ptLoadAccountFailed );
}
//-------------------------------------------------------------------
void CNetDC_Login::CreateRoleResult		( int iServerID, int iClientID, EM_CreateRoleResult emResult, string sRoleName, int iDBID  )
{
	PG_Login_D2M_CreateRoleResult ptCreateRoleResult;

	ptCreateRoleResult.iClientID	= iClientID;
	ptCreateRoleResult.emResult		= emResult;
	ptCreateRoleResult.iDBID		= iDBID;
	memset( ptCreateRoleResult.GiftItemID , 0 , sizeof( ptCreateRoleResult.GiftItemID ) );
	memset( ptCreateRoleResult.GiftItemCount , 0 , sizeof( ptCreateRoleResult.GiftItemCount ) );


	MyStrcpy( ptCreateRoleResult.RoleName, sRoleName.c_str() , sizeof( ptCreateRoleResult.RoleName ) );

	_Net->SendToSrv( iServerID, sizeof( PG_Login_D2M_CreateRoleResult ), &ptCreateRoleResult );
}

void CNetDC_Login::CreateRoleResult( int iServerID, int iClientID, EM_CreateRoleResult emResult, string sRoleName, int iDBID , int GiftItemID[5] , int GiftItemCount[5] , int Job , int Job_Sub , int Lv , int Lv_Sub )
{
	PG_Login_D2M_CreateRoleResult ptCreateRoleResult;
	
	ptCreateRoleResult.iClientID	= iClientID;
	ptCreateRoleResult.emResult		= emResult;
	//ptCreateRoleResult.iFieldIndex	= iFieldIndex;
	ptCreateRoleResult.iDBID		= iDBID;
//	ptCreateRoleResult.GiftItemID = GiftItemID;
	memcpy( ptCreateRoleResult.GiftItemID , GiftItemID , sizeof( ptCreateRoleResult.GiftItemID ) );
	memcpy( ptCreateRoleResult.GiftItemCount , GiftItemCount , sizeof( ptCreateRoleResult.GiftItemCount ) );
	ptCreateRoleResult.Lv = Lv;
	ptCreateRoleResult.Lv_Sub = Lv_Sub;
	ptCreateRoleResult.Job = Job;
	ptCreateRoleResult.Job_Sub = Job_Sub;


	MyStrcpy( ptCreateRoleResult.RoleName, sRoleName.c_str() , sizeof( ptCreateRoleResult.RoleName ) );
	
	_Net->SendToSrv( iServerID, sizeof( PG_Login_D2M_CreateRoleResult ), &ptCreateRoleResult );
}
//-------------------------------------------------------------------
void CNetDC_Login::DeleteRoleResult( int iServerID, int iClientID, EM_DeleteRoleResult emResult, int iDBID )
{
	PG_Login_D2M_DeleteRoleResult Packet;

	Packet.iDBID		= iDBID;
	Packet.iClientID	= iClientID;

	Packet.emResult		= emResult;

	_Net->SendToSrv( iServerID, sizeof( PG_Login_D2M_DeleteRoleResult ), &Packet );
}

void	CNetDC_Login::DelRoleRecoverResult( int CliNetID , int CliProxyID , int DBID , bool Result )
{
	PG_Login_D2C_DelRoleRecoverResult Send;
	Send.DBID = DBID;
	Send.Result = Result;
	_Net->SendToCli( CliNetID , CliProxyID , sizeof(Send) , &Send );
}

void	CNetDC_Login::SM_ChangeNameResult( int iServerID , int CliNetID , int DBID , const char* RoleName , ChangeNameResultENUM Result )
{
	PG_Login_D2M_ChangeNameResult Send;
	Send.DBID = DBID;
	Send.CliNetID = CliNetID;
	Send.Result = Result;
	MyStrcpy( Send.RoleName , RoleName , sizeof(Send.RoleName) );
	_Net->SendToSrv( iServerID , sizeof(Send) , &Send );
}
void	CNetDC_Login::SC_ReserveRole( int CliNetID , int CliProxyID , ReserverRoleStruct&	Info )
{
	PG_Login_DtoC_ReserveRole Send;
	Send.Info = Info;
	_Net->SendToCli( CliNetID , CliProxyID , sizeof(Send) , &Send );
}

void CNetDC_Login::SM_GetObjCountResult	( int iServerID, int iObjCount )
{
	PG_Login_PtoM_GetObjCountResult	Packet;

	Packet.iCount = iObjCount;

	_Net->SendToSrv( iServerID, sizeof( Packet ), &Packet );
}