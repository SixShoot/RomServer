#include "./Netsrv_Master_Login.h"	
#include "../Net_ServerList/Net_ServerList_Child.h"
#include "../../mainproc/ClientInfo/ClientInfo.h"
//#include "../../BTI/RABTI.h"



//-------------------------------------------------------------------
CNetSrv_MasterServer*	CNetSrv_MasterServer::m_pThis		= NULL;
CCaptchaGenerator*		CNetSrv_MasterServer::m_CaptchaGenerator = NULL;

//-------------------------------------------------------------------
CNetSrv_MasterServer::CNetSrv_MasterServer(void)
{
}
//-------------------------------------------------------------------
CNetSrv_MasterServer::~CNetSrv_MasterServer(void)
{
}
//-------------------------------------------------------------------
bool CNetSrv_MasterServer::_Init()
{
    _Net->RegOnLoginFunction				( _OnLogin );
    _Net->RegOnLogoutFunction				( _OnLogout );
    _Net->RegOnLoginFailFunction			( _OnLogFailed ); 

    _Net->RegOnCliConnectFunction			( _OnCliConnect );
    _Net->RegOnCliDisconnectFunction		( _OnCliDisconnect );

	_Net->RegOnSrvPacketFunction			( EM_PG_Login_D2M_RoleData,					OnPacket_RoleData				);
	_Net->RegOnSrvPacketFunction			( EM_PG_Login_D2M_RoleDataCount,			OnPacket_RoleDataCount			);
	_Net->RegOnSrvPacketFunction			( EM_PG_Login_D2M_LoadAccountFailed,		OnPacket_LoadAccountFailed		);
	_Net->RegOnSrvPacketFunction			( EM_PG_Login_D2M_DeleteRoleResult,			OnPacket_DeleteRoleResult		);
	_Net->RegOnSrvPacketFunction			( EM_PG_Login_D2M_CreateRoleResult,			OnPacket_CreateRoleResult		);

	_Net->RegOnSrvPacketFunction			( EM_PG_Login_L2M_SetRoleToWorldResult,		OnPacket_SetRoleToWorldResult	);
	

	_Net->RegOnCliPacketFunction			( EM_PG_Login_C2M_DeleteRole,				OnPacket_DeleteRole				);
	_Net->RegOnCliPacketFunction			( EM_PG_Login_C2M_SelectRole,				OnPacket_SelectRole				);
	_Net->RegOnCliPacketFunction			( EM_PG_Login_C2M_SelectRole_EnterZone1,	OnPacket_SelectRole_EnterZone1	);
	_Net->RegOnCliPacketFunction			( EM_PG_Login_C2M_CreateRole,				OnPacket_CreateRole				);

	_Net->RegOnCliPacketFunction			( EM_PG_Login_C2M_LoadRoleData,				OnPacket_LoadRoleData			);

	_Net->RegOnCliPacketFunction			( EM_PG_Login_C2M_DelRoleRecoverRequest,	_PG_Login_C2M_DelRoleRecoverRequest		);
	_Net->RegOnCliPacketFunction			( EM_PG_Login_C2M_SetSecondPassword,		_PG_Login_C2M_SetSecondPassword			);

	_Net->RegOnCliPacketFunction			( EM_PG_Login_C2M_ZoneLoadingRequest,		_PG_Login_C2M_ZoneLoadingRequest		);

	_Net->RegOnCliPacketFunction			( EM_PG_Login_C2M_APEXTOSERVER,				_PG_Login_C2M_APEXTOSERVER				);

	_Net->RegOnCliPacketFunction			( EM_PG_Login_C2M_ChangeNameRequest,		_PG_Login_C2M_ChangeNameRequest			);
	_Net->RegOnSrvPacketFunction			( EM_PG_Login_D2M_ChangeNameResult,			_PG_Login_D2M_ChangeNameResult			);

	_Net->RegOnSrvPacketFunction			( EM_PG_Login_LtoM_CloseMaster,				_PG_Login_LtoM_CloseMaster				);

	_Net->RegOnCliPacketFunction			( EM_PG_Login_CtoM_SecondPassword,			_PG_Login_CtoM_SecondPassword			);
	_Net->RegOnSrvPacketFunction			( EM_PG_Login_PtoM_GetObjCountResult,		_PG_Login_PtoM_GetObjCountResult		);

	_Net->RegOnCliPacketFunction			( EM_PG_Login_CtoM_GameGuardReport,			_PG_Login_CtoM_GameGuardReport			);

	_Net->RegOnCliPacketFunction			( EM_PG_Login_CtoM_CreateVivoxAccount,		_PG_Login_CtoM_CreateVivoxAccount		);

	_Net->RegOnCliPacketFunction			( EM_PG_Login_CtoM_SecondPassword2,			_PG_Login_CtoM_SecondPassword2			);
	_Net->RegOnCliPacketFunction			( EM_PG_Login_CtoM_CaptchaRefresh,			_PG_Login_CtoM_CaptchaRefresh			);
	_Net->RegOnCliPacketFunction			( EM_PG_Login_CtoM_CaptchaReply,			_PG_Login_CtoM_CaptchaReply				);

	m_CaptchaGenerator = new CCaptchaGenerator("./Captcha", "Char_", "BG_", "NS_", Global::_CaptchaBackGroundImageCount, Global::_CaptchaNoiseImageCount, Global::_CaptchaMaxStringLength, Global::_CaptchaAlphabetCount);	

	return false;
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::_PG_Login_CtoM_CreateVivoxAccount( int iClientID , int iSize , PVOID pData )
{
	PG_Login_CtoM_CreateVivoxAccount* PG = (PG_Login_CtoM_CreateVivoxAccount*)pData;
	m_pThis->OnCreateVivoxAccount( iClientID, PG->Account.Begin() );
}

//-------------------------------------------------------------------
void CNetSrv_MasterServer::_PG_Login_CtoM_SecondPassword2( int iClientID , int iSize , PVOID pData )
{
	PG_Login_CtoM_SecondPassword2* PG = (PG_Login_CtoM_SecondPassword2*)pData;
	m_pThis->RC_SecondPassword2( iClientID , PG->Password);
}

//-------------------------------------------------------------------
void CNetSrv_MasterServer::_PG_Login_CtoM_CaptchaRefresh	( int iClientID , int iSize , PVOID pData )
{
	PG_Login_CtoM_CaptchaRefresh* PG = (PG_Login_CtoM_CaptchaRefresh*)pData;
	m_pThis->RC_CaptchaRefresh( iClientID );
}

//-------------------------------------------------------------------
void CNetSrv_MasterServer::_PG_Login_CtoM_CaptchaReply( int iClientID , int iSize , PVOID pData )
{
	PG_Login_CtoM_CaptchaReply* PG = (PG_Login_CtoM_CaptchaReply*)pData;
	m_pThis->RC_CaptchaReply( iClientID , PG->Captcha);
}

//-------------------------------------------------------------------
void CNetSrv_MasterServer::_PG_Login_CtoM_GameGuardReport( int iClientID , int iSize , PVOID pData )
{
	PG_Login_CtoM_GameGuardReport* PG = (PG_Login_CtoM_GameGuardReport*)pData;
	m_pThis->OnGameGuardReport( iClientID , PG->iType, PG->iSize, PG->Report.Begin() );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::_PG_Login_PtoM_GetObjCountResult( int iClientID , int iSize , PVOID pData )
{
	PG_Login_PtoM_GetObjCountResult* PG = (PG_Login_PtoM_GetObjCountResult*)pData;
	m_pThis->OnGetObjCountResult( iClientID , PG->iCount );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::_PG_Login_CtoM_SecondPassword( int iClientID , int iSize , PVOID pData )
{
	PG_Login_CtoM_SecondPassword* PG = (PG_Login_CtoM_SecondPassword*)pData;
	m_pThis->RC_SecondPassword( iClientID , PG->Password );
}

void CNetSrv_MasterServer::_PG_Login_LtoM_CloseMaster		( int iClientID , int iSize , PVOID pData )
{
	PG_Login_LtoM_CloseMaster* PG = (PG_Login_LtoM_CloseMaster*)pData;
	m_pThis->RM_CloseMaster( );
}
void CNetSrv_MasterServer::_PG_Login_C2M_ChangeNameRequest	( int iClientID , int iSize , PVOID pData )
{
	PG_Login_C2M_ChangeNameRequest* PG = (PG_Login_C2M_ChangeNameRequest*)pData;
	m_pThis->RC_ChangeNameRequest( iClientID , PG->DBID , PG->RoleName );
}
void CNetSrv_MasterServer::_PG_Login_D2M_ChangeNameResult	( int iClientID , int iSize , PVOID pData )
{
	PG_Login_D2M_ChangeNameResult* PG = (PG_Login_D2M_ChangeNameResult*)pData;
	m_pThis->RD_ChangeNameResult( PG->CliNetID , PG->DBID , PG->RoleName , PG->Result );
}
//-------------------------------------------------------------------

bool CNetSrv_MasterServer::_Release()
{
	delete m_CaptchaGenerator;
	m_CaptchaGenerator = NULL;
	return true;
}

//-------------------------------------------------------------------
void CNetSrv_MasterServer::_OnLogin( string Info )					{ int a = 3; }
//-------------------------------------------------------------------
void CNetSrv_MasterServer::_OnLogout( )								{ }
//------------------------------------------------------------------
void CNetSrv_MasterServer::_OnLogFailed( SYSLogFailedENUM Type )	{ }
//-------------------------------------------------------------------
void CNetSrv_MasterServer::_OnCliConnect( int iClientID , string sAccount ) 
{ 
	PG_Login_M2C_IdentifyMaster ptIdentifyMaster;
	_Net->SendToCli( iClientID, sizeof( PG_Login_M2C_IdentifyMaster ), &ptIdentifyMaster );

	PG_Login_MtoC_SetGameGuard	PacketGameGuard;
	PacketGameGuard.iEnable	= _EnableGameGuard;
	_Net->SendToCli( iClientID, sizeof( PacketGameGuard ), &PacketGameGuard );

	m_pThis->On_ClientConnect( iClientID , (char*)sAccount.c_str() ); 
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::_OnCliDisconnect( int iClientID )	{ m_pThis->On_ClientDisconnect( iClientID ); }
//-------------------------------------------------------------------
void CNetSrv_MasterServer::_PG_Login_C2M_APEXTOSERVER( int CliNetID , int iSize , PVOID pData )
{
	PG_Login_C2M_APEXTOSERVER*	Packet = (PG_Login_C2M_APEXTOSERVER*)pData;
	//Print( LV2, "APEX", "RECV from client len=%d", Packet.iSize );
	//m_pThis->On_ApexDataFromClient( CliNetID, Packet->iSize, (BYTE*)pDataPacket->pData );
	//m_pThis->On_ApexDataFromClient( CliNetID, Packet->iSize, ( ((BYTE*)pDataPacket) + sizeof(int)*2) );


	BYTE*	ptr			= (BYTE*)pData;	
	ptr	+= sizeof(int);

	int		iDataSize	= *((int*)ptr);
	ptr	+= sizeof(int);

	BYTE*	pApexData	= ptr;

	m_pThis->On_ApexDataFromClient( CliNetID, iDataSize, pApexData );
}
//----------------------------------------------------------------------------------------
void CNetSrv_MasterServer::_PG_Login_C2M_ZoneLoadingRequest( int CliNetID , int iSize , PVOID pData )
{
	PG_Login_C2M_ZoneLoadingRequest* PG = (PG_Login_C2M_ZoneLoadingRequest*)pData;
	m_pThis->On_ZoneLoadingRequest( CliNetID , PG->ZoneID );
}

void CNetSrv_MasterServer::_PG_Login_C2M_SetSecondPassword	( int CliNetID , int iSize , PVOID pData )
{
	PG_Login_C2M_SetSecondPassword* PG = (PG_Login_C2M_SetSecondPassword*)pData;
	PG->Password[32] = 0;
	m_pThis->On_SetSecondPassword( CliNetID , PG->Password );
}
void CNetSrv_MasterServer::_PG_Login_C2M_DelRoleRecoverRequest( int CliNetID , int iSize , PVOID pData )
{
	PG_Login_C2M_DelRoleRecoverRequest* PG = (PG_Login_C2M_DelRoleRecoverRequest*)pData;
	GSrv_CliInfoStruct* CliNetInfo = _Net->GetCliNetInfo( CliNetID );
	if( CliNetInfo == NULL )
		return;

	m_pThis->On_DelRoleRecoverRequest( CliNetID , CliNetInfo->ProxyID , PG->DBID );
	//m_pThis->On_DelRoleRecoverRequest( iClientID , (char*)sAccount.c_str() ); 
}

void CNetSrv_MasterServer::OnPacket_RoleData				( int iServerID , int iSize , PVOID pData )
{
	static PlayerRoleData Role;
	PG_Login_D2M_RoleData* ptRoleData = (PG_Login_D2M_RoleData*)pData;
	Role.Base			= ptRoleData->Base;
	Role.SelfData 		= ptRoleData->SelfData;
	Role.BaseData 		= ptRoleData->BaseData;
	Role.PlayerSelfData	= ptRoleData->PlayerSelfData;
	Role.PlayerBaseData = ptRoleData->PlayerBaseData;
	Role.PlayerTempData.Init();
	Role.TempData.Init();
	Role.SetLink();

	m_pThis->On_RoleData( Role.Base.Account_ID.Begin(), ptRoleData->iClientID, ptRoleData->iDBID, &Role, true );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::OnPacket_RoleDataCount			( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_Login_D2M_RoleDataCount );
	m_pThis->On_RoleDataCount( pPacket->Account_ID.Begin(), pPacket->iClientID, pPacket->iRoleCount , pPacket->AccountBaseInfo, true );
}
//-------------------------------------------------------------------

void CNetSrv_MasterServer::OnPacket_LoadAccountFailed		( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_Login_D2M_LoadAccountFailed );
	m_pThis->On_LoadAccountFailed( pPacket->iClientID, pPacket->emErrID );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::OnPacket_DeleteRoleResult		( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_Login_D2M_DeleteRoleResult );
	m_pThis->On_DeleteRoleResult( pPacket->iClientID, pPacket->iDBID, pPacket->emResult );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::OnPacket_CreateRoleResult		( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_Login_D2M_CreateRoleResult );
	m_pThis->On_CreateRoleResult( pPacket->iClientID, pPacket->emResult, pPacket->RoleName, pPacket->iDBID , pPacket->GiftItemID , pPacket->GiftItemCount , pPacket->Job , pPacket->Job_Sub , pPacket->Lv , pPacket->Lv_Sub );
}
//-------------------------------------------------------------------
// from Client
void CNetSrv_MasterServer::OnPacket_DeleteRole				( int iClientID , int iSize , PVOID pData )
{
	M_PACKET( PG_Login_C2M_DeleteRole );
	m_pThis->On_DeleteRole( iClientID, pPacket->szAccountName, pPacket->iDBID , pPacket->IsImmediately , (char*)pPacket->Password.Begin() );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::OnPacket_CreateRole				( int iClientID , int iSize , PVOID pData )
{
	PG_Login_C2M_CreateRole* p = (PG_Login_C2M_CreateRole*)pData;
	m_pThis->On_CreateRole( iClientID, p->szAccountName, p->iFieldIndex, &p->CreateRole );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::OnPacket_SelectRole				( int iClientID , int iSize , PVOID pData )
{
	M_PACKET( PG_Login_C2M_SelectRole );
	m_pThis->On_SelectRole( iClientID, pPacket->szAccountName, pPacket->iDBID , pPacket->ParallelZoneID , false );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::OnPacket_SelectRole_EnterZone1				( int iClientID , int iSize , PVOID pData )
{
	M_PACKET( PG_Login_C2M_SelectRole );
	m_pThis->On_SelectRole( iClientID, pPacket->szAccountName, pPacket->iDBID , pPacket->ParallelZoneID , true );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::OnPacket_SetRoleToWorldResult	( int iServerID , int iSize , PVOID pData )
{
	M_PACKET( PG_Login_L2M_SetRoleToWorldResult );
	m_pThis->On_SetRoleToWorldResult( iServerID, pPacket->iClientID, pPacket->iDBID, pPacket->emResult );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::OnPacket_LoadRoleData( int iClientID , int iSize , PVOID pData )
{
	M_PACKET( PG_Login_C2M_LoadRoleData );
	m_pThis->On_LoadRoleData( iClientID, pPacket->szAccountName  );
}

//-------------------------------------------------------------------
void CNetSrv_MasterServer::SendClientRoleData( int iClientID, int iDBID, SampleRoleData* pSamepleRoleData )
{
	PG_Login_M2C_SampleRoleData	ptSampleRole;
	ptSampleRole.iDBID			= iDBID;
	ptSampleRole.SampleRole		= *pSamepleRoleData;

	_Net->SendToCli( iClientID, sizeof( PG_Login_M2C_SampleRoleData ), &ptSampleRole );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::SendClientRoleDataEnd	( int iClientID , int iParallelZoneCount )
{
	PG_Login_M2C_SampleRoleDataEnd ptSampleRoleEnd;
	ptSampleRoleEnd.ParallelZoneCount = iParallelZoneCount;

	_Net->SendToCli( iClientID, sizeof( PG_Login_M2C_SampleRoleDataEnd ), &ptSampleRoleEnd );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::SetRoleToWorld			( int iClientID, PlayerRoleData* pRole , int ParallelZoneID , bool EnterZone1 )
{
	PG_Login_M2L_SetRoleToWorld Packet;
	
	Packet.iClientID	= iClientID;
//Packet.Role			= *pRole;
	Packet.Role.Copy( pRole );
	Packet.Role.PlayerTempData.ParallelZoneID = ParallelZoneID;


	if( ParallelZoneID < 0 || ParallelZoneID >= 100 )
	{
		SelectRoleResult( iClientID, pRole->Base.DBID, EM_SelectRole_ZoneNotExist );
		return;
	}

	int ToZoneID = GetParallelZoneID( pRole->BaseData.ZoneID , ParallelZoneID );
//	RolePosStruct	PosBk = pRole->BaseData.Pos;

	bool IsFaield = false;
	if( ToZoneID == -1 && EnterZone1 != false  )
	{
		Packet.Role.BaseData.Pos.X = -4070;
		Packet.Role.BaseData.Pos.Y = 245;
		Packet.Role.BaseData.Pos.Z = -8248;
		Packet.Role.BaseData.Pos.Dir = 47;
		Packet.Role.BaseData.ZoneID = 1;
		ToZoneID = GetParallelZoneID( 1 , ParallelZoneID );
	}

/*	if(		ToZoneID%_DEF_ZONE_BASE_COUNT_ <= 100 
		&& ( _PlayerCount[ParallelZoneID] >= _MaxPlayerCount ||  _ZonePlayerCount[ ToZoneID ] >= _MaxZonePlayerCount ) )
	{
		SelectRoleResult( iClientID, pRole->Base.DBID, EM_SelectRole_Parallel_Zone_Full_ );
		IsFaield = true;
	}
	else */
	if( ToZoneID == -1 || Global::SendToLocal( ToZoneID , sizeof( PG_Login_M2L_SetRoleToWorld ), &Packet ) == false )
	{
		// 失敗
		// 可能是 Zone Server 不存在或者是其它可能, 告知 Client
		SelectRoleResult( iClientID, pRole->Base.DBID, EM_SelectRole_ZoneNotExist );
		IsFaield = true;
	}
//	pRole->BaseData.Pos = PosBk;
	
	/*
	else
	{

		Print( LV1, "CNetSrv_MasterServer::SetRoleToWorld():"," Tring to set role to ZoneServer( %d ). But it wasnt exist!", pRole->BaseData.ZoneID );
	}
	*/
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::DeleteRole	( int PlayerCenterID , int iClientID, const char* pszAccountName, const char* pszRoleName, int iDBID , bool IsImmediately )
{
	PG_Login_M2D_DeleteRole Packet;

	MyStrcpy( Packet.szAccountName,	pszAccountName, sizeof( Packet.szAccountName )	);
	MyStrcpy( Packet.szRoleName,		pszRoleName, sizeof( Packet.szRoleName )		);

	Packet.iClientID		= iClientID;
	Packet.iDBID			= iDBID;
	Packet.IsImmediately	= IsImmediately;

	// Send the packet to dataCenter
	Global::SendToRoleDBCenter( PlayerCenterID , sizeof( PG_Login_M2D_DeleteRole ), &Packet );

}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::CreateRole				( int PlayerCenterID , int iClientID, const char* pszAccountName, PlayerRoleData* pNewRole )
{
	PG_Login_M2D_CreateRole Packet;

	MyStrcpy( Packet.szAccountName, pszAccountName, sizeof( Packet.szAccountName ) );

	Packet.iClientID		= iClientID;
	//Packet.Role				= *pNewRole;
	Packet.Role.Copy( pNewRole );
//	strcpy( Packet.szAccountName , pszAccountName );

	// Send the packet to dataCenter
	Global::SendToRoleDBCenter( PlayerCenterID , sizeof( PG_Login_M2D_CreateRole ), &Packet );

}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::CreateRoleResult		( int iClientID, int iDBID, EM_CreateRoleResult emResult, SampleRoleData* pSampleRoleData )
{
	PG_Login_M2C_CreateRoleResult Packet;

	Packet.iDBID		= iDBID;
	Packet.emResult		= emResult;
	

	memset( &Packet.GiftItemID , 0 , sizeof( Packet.GiftItemID ) );
	memset( &Packet.GiftItemCount , 0 , sizeof( Packet.GiftItemCount ) );

	ZeroMemory( &Packet.SampleRole, sizeof( Packet.SampleRole ) );

	if( pSampleRoleData )
	{
		Packet.SampleRole	= *pSampleRoleData;
	}

	_Net->SendToCli( iClientID, sizeof( PG_Login_M2C_CreateRoleResult ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::CreateRoleResult		( int iClientID, int iDBID, EM_CreateRoleResult emResult, SampleRoleData* pSampleRoleData , int GiftItemID[5] , int GiftItemCount[6] )
{
	PG_Login_M2C_CreateRoleResult Packet;

	Packet.iDBID		= iDBID;
	Packet.emResult		= emResult;
//	Packet.GiftItemID	= GiftItemID;

	memcpy( Packet.GiftItemID , GiftItemID , sizeof( Packet.GiftItemID ) );
	memcpy( Packet.GiftItemCount , GiftItemCount , sizeof( Packet.GiftItemCount ) );
	
	ZeroMemory( &Packet.SampleRole, sizeof( Packet.SampleRole ) );

	if( pSampleRoleData )
	{
		Packet.SampleRole	= *pSampleRoleData;
	}

	_Net->SendToCli( iClientID, sizeof( PG_Login_M2C_CreateRoleResult ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::DeleteRoleResult		( int iClientID, int iDBID, EM_DeleteRoleResult emResult )
{
	PG_Login_M2C_DeleteRoleResult Packet;
	
	Packet.emResult		= emResult;
	Packet.iDBID		= iDBID;

	_Net->SendToCli( iClientID, sizeof( PG_Login_M2C_DeleteRoleResult ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::SelectRoleResult		( int iClientID, int iDBID, EM_SelectRoleResult emResult )
{
	PG_Login_M2C_SelectRoleResult Packet;

	Packet.emResult	= emResult;
	Packet.iDBID	= iDBID;

	_Net->SendToCli( iClientID, sizeof( PG_Login_M2C_SelectRoleResult ), &Packet );


	if( emResult != EM_SelectRole_OK )
	{
		ClientInfo* pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );
		if( pClientInfo )
		{
			// 選角失敗了, 告知了 Client, 讓 Client 可以重選角色
			//pClientInfo->m_pSelectedRole	= NULL;
			pClientInfo->m_iSelectedRoleDBID = 0;
		}
	}
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::SendEnterWorld ( int iClientID )
{
	PG_Login_M2C_EnterWorld	Packet;

	_Net->SendToCli( iClientID, sizeof( PG_Login_M2C_EnterWorld ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::LoginResult ( int iClientID, EM_LoginResult emResult )
{
	PG_Login_M2C_LoginResult Packet;

	Packet.emResult = emResult;

	_Net->SendToCli( iClientID, sizeof( PG_Login_M2C_LoginResult ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::LoadDataEvent ( int iClientID, EM_LoadDataEvent emEvent, int iValue )
{
	PG_Login_M2C_LoadDataEvent Packet;

	Packet.emEvent	= emEvent;
	Packet.iValue	= iValue;

	_Net->SendToCli( iClientID, sizeof( PG_Login_M2C_LoadDataEvent ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::DelRoleRecoverRequest( int PlayerCenterID , int CliNetID , int ProxyID , int DBID )
{
	PG_Login_M2D_DelRoleRecoverRequest Send;
	Send.NetID = CliNetID;
	Send.ProxyID = ProxyID;
	Send.DBID = DBID;
	Global::SendToRoleDBCenter( PlayerCenterID , sizeof( Send ), &Send );	
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::SC_SetPasswordRequest( int CliNetID )
{
	PG_Login_M2C_SetSecondPasswordRequest Send;
	_Net->SendToCli( CliNetID, sizeof( Send ), &Send );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::SD_SetSecondPassword( int PlayerCenterID , const char* Account , const char* Password )
{
	PG_Login_M2D_SetSecondPassword Send;
	MyStrcpy( Send.Account , Account , sizeof(Send.Account) );
	MyStrcpy( Send.Password , Password , sizeof(Send.Password) );
	Global::SendToRoleDBCenter( PlayerCenterID , sizeof( Send ), &Send );	
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::ZoneLoading( int CliNetID , int ZoneID , int MaxZoneCount , vector<int>& Count )
{
	PG_Login_M2C_ZoneLoading Send;

	for( int i = 0 ; i < (int)Count.size() ; i++ )
	{
		Send.Count[i] = Count[i];
	}
	Send.ParallelCount = (int)Count.size();
	Send.ZoneID = ZoneID;
	Send.MaxZoneCount = MaxZoneCount;
	_Net->SendToCli( CliNetID, sizeof( Send ), &Send );
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::SL_BTI_RolePlayHours( int iSrvID, int iRoleDBID, int iHours )
{
	PG_Nonage_M2L_HealthTime Packet;

	Packet.iRoleDBID		= iRoleDBID;
	Packet.iHealthTime		= iHours;

	_Net->SendToSrv( iSrvID, sizeof( PG_Nonage_M2L_HealthTime ), &Packet );

}
//-------------------------------------------------------------------
void	CNetSrv_MasterServer::SC_APEXTOCLIENT( int CliNetID, int iSize, char* pData )
{
	int		iPacketSize =	sizeof(int) * 2 + iSize;
	BYTE*	pPacket		=	new BYTE[ iPacketSize ];
	BYTE*	ptr			=	pPacket;

	*((DWORD*)ptr)		=	EM_PG_Login_M2C_APEXTOCLIENT;
	ptr					=	ptr + sizeof( DWORD );

	*((DWORD*)ptr)		=	iSize;
	ptr					=	ptr + sizeof( DWORD );

	memcpy( ptr, pData, iSize );

	_Net->SendToCli( CliNetID, iPacketSize, pPacket );

	delete [] pPacket;
}
//-------------------------------------------------------------------
void CNetSrv_MasterServer::ApexKickPlayer( int iClientID )
{
	ClientInfo* pClientInfo	= NULL;
	pClientInfo = g_pClientManger->GetClientInfoByID( iClientID );

	if( pClientInfo )
	{
		char szBuffer[256];
		sprintf( szBuffer, "Master APEX KICK %d - %s", iClientID, pClientInfo->m_sAccountName.c_str() );
		Print( LV2, "APEX", szBuffer );
		_Net->CliLogout( iClientID, Net_ServerList->GetServerType(), Net_ServerList->GetLocalID(), pClientInfo->m_sAccountName.c_str(), szBuffer );
	}
}

void CNetSrv_MasterServer::SD_ChangeNameRequest ( int PlayerCenterID , int CliNetID , int DBID , char* NewRoleName )
{
	PG_Login_M2D_ChangeNameRequest Send;
	
	Send.DBID = DBID;
	Send.CliNetID = CliNetID;
	MyStrcpy( Send.RoleName , NewRoleName , sizeof(Send.RoleName) );
	
	Global::SendToRoleDBCenter( PlayerCenterID , sizeof( Send ), &Send );
}
void CNetSrv_MasterServer::SC_ChangeNameResult ( int CliNetID , int DBID , char* NewRoleName , ChangeNameResultENUM Result )
{
	PG_Login_M2C_ChangeNameResult Send;

	Send.DBID = DBID;
	MyStrcpy( Send.RoleName , NewRoleName , sizeof(Send.RoleName) );
	Send.Result = Result;

	_Net->SendToCli( CliNetID , sizeof( Send ), &Send );
}

void CNetSrv_MasterServer::SC_MaxRoleCount( int CliNetID , int RoleCount )
{
	PG_Login_MtoC_MaxRoleCount Send;
	Send.Count = RoleCount;
	_Net->SendToCli( CliNetID , sizeof( Send ), &Send );
}

void CNetSrv_MasterServer::SD_ReserveRoleRequest( int PlayerCenterID , int CliNetID , const char* Account )
{
	GSrv_CliInfoStruct* CliNetInfo = _Net->GetCliNetInfo( CliNetID );
	if( CliNetInfo == NULL )
		return;

	PG_Login_MtoD_ReserveRoleRequest Send;
	Send.CliNetID = CliNetID;
	Send.CliProxyID = CliNetInfo->ProxyID;
	Send.Account_ID = Account;
	Global::SendToRoleDBCenter( PlayerCenterID , sizeof( Send ), &Send );
}
void CNetSrv_MasterServer::SC_SecondPasswordRequest( int CliNetID )
{
	PG_Login_MtoC_SecondPasswordRequest Send;
	_Net->SendToCli( CliNetID , sizeof( Send ), &Send );
}
//---------------------------------------------------
void CNetSrv_MasterServer::SC_SecondPasswordResult( int CliNetID , bool Result )
{
	PG_Login_MtoC_SecondPasswordResult Send;
	Send.Result = Result;
	_Net->SendToCli( CliNetID , sizeof( Send ), &Send );
}

//---------------------------------------------------
void CNetSrv_MasterServer::SC_SecondPasswordRequest2( int CliNetID )
{
	PG_Login_MtoC_SecondPasswordRequest2 Send;
	_Net->SendToCli( CliNetID , sizeof( Send ), &Send );
}

//---------------------------------------------------
void CNetSrv_MasterServer::SC_SecondPasswordResult2( int CliNetID , bool Result )
{
	PG_Login_MtoC_SecondPasswordResult2 Send;
	Send.Result = Result;
	_Net->SendToCli( CliNetID , sizeof( Send ), &Send );
}

//---------------------------------------------------
void CNetSrv_MasterServer::SC_CaptchaCheck( int CliNetID , void* Captcha, unsigned long CaptchaSize )
{
	unsigned long dwSize = sizeof(PG_Login_MtoC_CaptchaCheck) + CaptchaSize;

	dwSize = ((dwSize / 4) + 1) * 4;

	void* pData = new BYTE[dwSize];
	memset(pData, 0, dwSize);

	PBYTE pTravaler = (PBYTE)pData;
	PG_Login_MtoC_CaptchaCheck* pHeader = (PG_Login_MtoC_CaptchaCheck*)pTravaler;
	pHeader->Command = EM_PG_Login_MtoC_CaptchaCheck;
	pHeader->CaptchaSize = CaptchaSize;

	pTravaler += sizeof(PG_Login_MtoC_CaptchaCheck);

	if (Captcha != NULL)
	{
		memcpy(pTravaler, Captcha, CaptchaSize);
	}
	else
	{
		pHeader->CaptchaSize = 0;
	}
	
	_Net->SendToCli( CliNetID , dwSize, pData );

	delete pData;
}

//---------------------------------------------------
void CNetSrv_MasterServer::SC_CaptchaCheckResult( int CliNetID , int Result )
{
	PG_Login_MtoC_CaptchaCheckResult Send;
	Send.Result = Result;
	_Net->SendToCli( CliNetID , sizeof( Send ), &Send );
}

//---------------------------------------------------
void CNetSrv_MasterServer::GetObjCount( int PlayerCenterID )
{
	PG_Login_MtoP_GetObjCount Packet;
	Global::SendToRoleDBCenter( PlayerCenterID , sizeof( Packet ), &Packet );
}
//---------------------------------------------------
void CNetSrv_MasterServer::SC_VivoxAccount( int iClientID, const char* pszAccount, const char* pszPassword, bool bCreateResult )
{
	PG_Login_MtoC_VivoxAccount	Packet;

	Packet.Account			= pszAccount;
	Packet.Password			= pszPassword;
	Packet.bCreateResult	= bCreateResult;

	Global::SendToCli( iClientID, sizeof( Packet ), &Packet );
}
//---------------------------------------------------
void CNetSrv_MasterServer::SC_VivoxInfo( int iClientID, const char* pszUrl, const char* pszCountryCode )
{
	PG_Login_MtoC_VivoxInfo		Packet;

	Packet.Url				= pszUrl;
	Packet.CountryCode		= pszCountryCode;

	Global::SendToCli( iClientID, sizeof( Packet ), &Packet );
}
//---------------------------------------------------
void CNetSrv_MasterServer::SD_SetCheckSecondPasswordResult( int PlayerCenterID , int iClientID, const char* pszAccount, int iResult )
{
	PG_Login_M2D_SetCheckSecondPasswordResult	Packet;

	Packet.iClientID = iClientID;
	strncpy( Packet.szAccountName, pszAccount, sizeof( Packet.szAccountName ) - 1 );
	Packet.szAccountName[ (_MAX_ACCOUNT_SIZE_ - 1 )] = 0;

	Packet.iResult	= iResult;
	
	Global::SendToRoleDBCenter( PlayerCenterID , sizeof( Packet ), &Packet );
}