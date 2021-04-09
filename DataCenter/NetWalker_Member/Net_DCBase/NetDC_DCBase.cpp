#include "PG/PG_DCBase.h"
#include "NetDC_DCBase.h"
//-------------------------------------------------------------------
CNetDC_DCBase*	CNetDC_DCBase::This = NULL;
//-------------------------------------------------------------------
bool CNetDC_DCBase::_Release()
{

	return false;
}
//-------------------------------------------------------------------
bool CNetDC_DCBase::_Init()
{

//	_Net->RegOnSrvPacketFunction			( EM_PG_DCBase_XtoD_SavePlayer						, _PG_DCBase_XtoD_SavePlayer		    );
	_Net->RegOnSrvPacketFunction			( EM_PG_DCBase_LtoD_SaveNPCRequest					, _PG_DCBase_LtoD_SaveNPCRequest		);
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_LtoD_DropNPCRequest					, _PG_DBBase_LtoD_DropNPCRequest		);
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_LtoD_LoadAllNPC						, _PG_DBBase_LtoD_LoadAllNPC		    );
//	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_XtoD_SetPlayerLiveTime				, _PG_DBBase_XtoD_SetPlayerLiveTime		);
//	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_XtoD_SetPlayerLiveTime_ByAccount		, _PG_DBBase_XtoD_SetPlayerLiveTime_ByAccount		);

	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_LtoD_LoadAllNPCMoveInfo				, _PG_DBBase_LtoD_LoadAllNPCMoveInfo	);
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_LtoD_SaveNPCMoveInfo					, _PG_DBBase_LtoD_SaveNPCMoveInfo		);

	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_LtoD_SqlCommand						, _PG_DBBase_LtoD_SqlCommand			);
//	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_LtoD_LogSqlCommand					, _PG_DBBase_LtoD_LogSqlCommand			);
//	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_XtoD_CheckRoleDataSize				, _PG_DBBase_XtoD_CheckRoleDataSize		);
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_LtoD_ZoneInfoRequest					, _PG_DBBase_LtoD_ZoneInfoRequest		);

	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_LtoD_GmCommandResult					, _PG_DBBase_LtoD_GmCommandResult		);
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_RDtoX_CheckRoleStruct				, _PG_DBBase_RDtoX_CheckRoleStruct		);
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_LtoD_LogData							, _PG_DBBase_LtoD_LogData				);
	_Net->RegOnLoginFunction( _OnLoginEvent  );
	return true;
}
//-------------------------------------------------------------------
void CNetDC_DCBase::_OnLoginEvent( string info )
{
	This->OnLogin();
}

void CNetDC_DCBase::_PG_DBBase_RDtoX_CheckRoleStruct				( int NetID , int Size , void* Data )
{
	PG_DBBase_RDtoX_CheckRoleStruct* PG = ( PG_DBBase_RDtoX_CheckRoleStruct* )Data;
	This->R_CheckRoleStruct( PG->Count , PG->Info ); 
}

void CNetDC_DCBase::_PG_DBBase_LtoD_LogData( int NetID , int Size , void* Data )
{
	PG_DBBase_LtoD_LogData* PG = ( PG_DBBase_LtoD_LogData* )Data;
	PBYTE LogData = (PBYTE)Data;
	LogData += sizeof(PG_DBBase_LtoD_LogData);
	This->R_LogData( PG->LogType , LogData, PG->DataSize);
}

void CNetDC_DCBase::_PG_DBBase_LtoD_GmCommandResult					( int NetID , int Size , void* Data )
{
	PG_DBBase_LtoD_GmCommandResult* PG = ( PG_DBBase_LtoD_GmCommandResult* )Data;
	This->RL_GmCommandResult( PG->GUID , PG->Result , PG->ResultStr ); 
}
/*
void CNetDC_DCBase::_PG_DBBase_LtoD_SaveRoleDataNotifyResult( int NetID , int Size , void* Data )
{
	PG_DBBase_LtoD_SaveRoleDataNotifyResult* PG = ( PG_DBBase_LtoD_SaveRoleDataNotifyResult* )Data;
	This->RL_SaveRoleDataNotifyResult( PG->Account.Begin() , PG->PlayerDBID , PG->ClientID , PG->MasterSockID , PG->Result ); 
}
*/
void CNetDC_DCBase::_PG_DBBase_LtoD_ZoneInfoRequest		( int NetID , int Size , void* Data )
{
	PG_DBBase_LtoD_ZoneInfoRequest* PG = ( PG_DBBase_LtoD_ZoneInfoRequest* )Data;
	This->R_ZoneInfoRequest( NetID , PG->ZoneID ); 
}
/*
void CNetDC_DCBase::_PG_DBBase_XtoD_CheckRoleDataSize	( int NetID , int Size , void* Data )
{
	PG_DBBase_XtoD_CheckRoleDataSize* PG = ( PG_DBBase_XtoD_CheckRoleDataSize* )Data;
	This->R_CheckRoleDataSize( NetID , PG->Size ); 
}
*/
void CNetDC_DCBase::_PG_DBBase_LtoD_SqlCommand			( int NetID , int Size , void* Data )
{
	PG_DBBase_LtoD_SqlCommand* PG = ( PG_DBBase_LtoD_SqlCommand* )Data;
	This->R_SqlCommand( PG->ThreadID , PG->Type , (char*)PG->Data.Begin() ); 
}
/*
void CNetDC_DCBase::_PG_DBBase_LtoD_LogSqlCommand			( int NetID , int Size , void* Data )
{
	PG_DBBase_LtoD_LogSqlCommand* PG = ( PG_DBBase_LtoD_LogSqlCommand* )Data;
	This->R_LogSqlCommand( (char*)PG->Data.Begin() ); 
}
*/

/*
void CNetDC_DCBase::_PG_DCBase_XtoD_SavePlayer          ( int NetID , int Size , void* Data )
{
    PG_DCBase_XtoD_SavePlayer* PG = ( PG_DCBase_XtoD_SavePlayer * )Data;
    This->SavePlayer( NetID , PG->SaveTime , PG->DBID , PG->Start , PG->Size , &PG->Player ); 
}
*/

void CNetDC_DCBase::_PG_DCBase_LtoD_SaveNPCRequest   ( int NetID , int Size , void* Data )
{
    PG_DCBase_LtoD_SaveNPCRequest* PG = ( PG_DCBase_LtoD_SaveNPCRequest* )Data;
    This->SaveNPCRequest( NetID , PG->LocalObjID , (RoleDataEx*)&PG->NPC );
}

void CNetDC_DCBase::_PG_DBBase_LtoD_DropNPCRequest      ( int NetID , int Size , void* Data )
{
    PG_DBBase_LtoD_DropNPCRequest* PG = ( PG_DBBase_LtoD_DropNPCRequest* )Data;
	This->DropNPCRequest( NetID , PG->LocalObjID , PG->DBID , PG->IsDelFlag , PG->DestroyAccount.Begin() );
}
void CNetDC_DCBase::_PG_DBBase_LtoD_LoadAllNPC          ( int NetID , int Size , void* Data )
{
    PG_DBBase_LtoD_LoadAllNPC* PG = ( PG_DBBase_LtoD_LoadAllNPC* )Data;
    This->LoadAllNPC( PG->ZoneID );
}
/*
void CNetDC_DCBase::_PG_DBBase_XtoD_SetPlayerLiveTime   ( int NetID , int Size , void* Data )
{
    PG_DBBase_XtoD_SetPlayerLiveTime* PG = ( PG_DBBase_XtoD_SetPlayerLiveTime* )Data;
    This->SetPlayerLiveTime( NetID , PG->DBID , PG->LiveTime );
}
*/
/*
void CNetDC_DCBase::_PG_DBBase_XtoD_SetPlayerLiveTime_ByAccount   ( int NetID , int Size , void* Data )
{
    PG_DBBase_XtoD_SetPlayerLiveTime_ByAccount* PG = ( PG_DBBase_XtoD_SetPlayerLiveTime_ByAccount* )Data;
    This->SetPlayerLiveTime_ByAccount( NetID , PG->Account , PG->LiveTime );
}
*/
void CNetDC_DCBase::_PG_DBBase_LtoD_LoadAllNPCMoveInfo				( int NetID , int Size , void* Data )
{
	PG_DBBase_LtoD_LoadAllNPCMoveInfo * PG = ( PG_DBBase_LtoD_LoadAllNPCMoveInfo* )Data;
	This->LoadAllNPCMoveInfo( PG->ZoneID );
}
void CNetDC_DCBase::_PG_DBBase_LtoD_SaveNPCMoveInfo					( int NetID , int Size , void* Data )
{
	PG_DBBase_LtoD_SaveNPCMoveInfo* PG = ( PG_DBBase_LtoD_SaveNPCMoveInfo* )Data;
	This->SaveNPCMoveInfo( PG->TotalCount , PG->ZoneID , PG->NPCDBID , PG->IndexID , PG->Base );
}
//-------------------------------------------------------------------
/*
void CNetDC_DCBase::SavePlayerFailed         ( int SrvNetID , int DBID )
{
    PG_DBBase_DtoX_SavePlayerError  Send;
    Send.DBID   = DBID;
    Send.Result = EM_SavePlayer_PlayerNotFind;
    _Net->SendToSrv( SrvNetID , sizeof( Send ) , &Send );

}
*/
void CNetDC_DCBase::SaveNPCResultOK      ( int SrvNetID , int LocalObjID , int NPCDBID )
{
    PG_DCBase_DtoL_SaveNPCResult Send;
    Send.LocalObjID = LocalObjID;
    Send.NPCDBID    = NPCDBID;
    Send.Result     = EM_SaveNPC_OK;
    _Net->SendToSrv( SrvNetID , sizeof( Send ) , &Send );
}
void CNetDC_DCBase::SaveNPCResultFailed   ( int SrvNetID , int LocalObjID )
{
    PG_DCBase_DtoL_SaveNPCResult Send;
    Send.LocalObjID = LocalObjID;
    Send.Result     = EM_SaveNPC_Failed;
    _Net->SendToSrv( SrvNetID , sizeof( Send ) , &Send );
}

void CNetDC_DCBase::DropNPCResultOK         ( int SrvNetID , int LocalObjID , int NPCDBID )
{
    PG_DBBase_DtoL_DropNPCResult Send;
    Send.DBID = NPCDBID;
    Send.LocalObjID = LocalObjID;
    Send.Result = EM_DropNPC_OK;
    _Net->SendToSrv( SrvNetID , sizeof( Send ) , &Send );
}
void CNetDC_DCBase::DropNPCResultFailed      ( int SrvNetID , int LocalObjID , int NPCDBID )
{
    PG_DBBase_DtoL_DropNPCResult Send;
    Send.DBID = NPCDBID;
    Send.LocalObjID = LocalObjID;
    Send.Result = EM_DropNPC_NotFind;
    _Net->SendToSrv( SrvNetID , sizeof( Send ) , &Send );
}

void CNetDC_DCBase::LoadNPCResult           ( int ZoneID , int TotalCount , int ID , NPCData* NPC )
{
    PG_DBBase_DtoL_LoadAllNPCResult Send;
    Send.ID         = ID;
    Send.TotalCount = TotalCount;
    Send.NPC        = *NPC;

    SendToLocal( ZoneID , sizeof( Send ) , &Send );
}

/*
void CNetDC_DCBase::SetPlayerLiveTimeFailed  ( int SrvNetID , int DBID )
{
    PG_DBBase_DtoX_SetPlayerLiveTimeError Send;
    Send.DBID = DBID;
    Send.Result = EM_SetPlayerLiveTime_PlayerNotFind;
    _Net->SendToSrv( SrvNetID , sizeof( Send ) , &Send );
}
*/
/*
void CNetDC_DCBase::SetPlayerLiveTimeFailed_ByAccount  ( int SrvNetID , char* Account )
{
    PG_DBBase_DtoX_SetPlayerLiveTimeError_ByAccount Send;
//    Send.DBID = DBID;
    MyStrcpy( Send.Account , Account , sizeof(Send.Account) );
    Send.Result = EM_SetPlayerLiveTime_PlayerNotFind;
    _Net->SendToSrv( SrvNetID , sizeof( Send ) , &Send );
}
*/
void CNetDC_DCBase::NPCMoveInfoResult( int ZoneID , int NPCDBID , int TotalCount , int IndexID , NPC_MoveBaseStruct& Base )
{
	PG_DBBase_DtoL_NPCMoveInfoResult Send;
	Send.Base = Base;
	Send.ZoneID = ZoneID;
	Send.NPCDBID = NPCDBID;
	Send.TotalCount = TotalCount;
	Send.IndexID = IndexID;

	SendToLocal( ZoneID , sizeof( Send ) , &Send );
}
/*
void CNetDC_DCBase::S_CheckRoleDataSizeResult( int SrvNetID , bool Result )
{
	PG_DBBase_DtoX_CheckRoleDataSizeResult Send;
	Send.Result = Result;
	_Net->SendToSrv( SrvNetID , sizeof( Send ) , &Send );
}
*/
void CNetDC_DCBase::S_ZoneInfoResult		( int SrvNetID , LocalServerBaseDBInfoStruct& Info )
{
	PG_DBBase_DtoL_ZoneInfoResult Send;
	Send.Info = Info;
	_Net->SendToSrv( SrvNetID , sizeof( Send ) , &Send );
}
void CNetDC_DCBase::S_GlobalZoneInfoResult		( GlobalServerDBInfoStruct& Info )
{
	PG_DBBase_DtoL_GlobalZoneInfoResult Send;
	Send.Info = Info;
	SendToAllLocal( sizeof( Send ) , &Send );
}

void CNetDC_DCBase::SL_GmCommandRequest			( int LocalID , GmCommandStruct& Info )
{
	PG_DBBase_DtoL_GmCommandRequest Send;
	Send.Info = Info;
	Send.Info.Command[511] = 0;

	SendToLocal( LocalID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
/*
void CNetDC_DCBase::SL_SaveRoleDataNotify		( int LocalID , int PlayerDBID , char* Account , int ClientID , int MasterSockID )
{
	PG_DBBase_DtoL_SaveRoleDataNotify Send;
	Send.Account = Account;
	Send.PlayerDBID = PlayerDBID;
	Send.ClientID = ClientID;
	Send.MasterSockID = MasterSockID;
	SendToLocal( LocalID , sizeof(Send) , &Send );
}
*/
//////////////////////////////////////////////////////////////////////////
void CNetDC_DCBase::R_CheckRoleStruct( int Count , CheckKeyAddressStruct Info[1000] )
{
	ReaderClass<PlayerRoleData>* Reader = RoleDataEx::ReaderBase( );
	map<string,ReadInfoStruct>&  ReadInfo = *( Reader->ReadInfo() );
	map<string,ReadInfoStruct>::iterator Iter;

	if( Count != ReadInfo.size() )
	{
		PrintToController(true, "R_CheckRoleStruct false Count != ReadInfo.size()  " );
		return;
	}

	int i = 0;
	for( Iter = ReadInfo.begin() ; Iter != ReadInfo.end() ; Iter++ , i++)
	{		
		if( stricmp( Info[i].Key , Iter->first.c_str() ) != 0 )
		{
			PrintToController(true, "R_CheckRoleStruct false stricmp( Info[i].Key , Iter->first.c_str() ) != 0 Key=%s " , Info[i].Key  );
			return;
		}
		if( Info[i].Address != Iter->second.Point )
		{
			PrintToController(true, "R_CheckRoleStruct false Info[i].Address != Iter->second.Point Key=%s Point=%d-%d" , Info[i].Key , Info[i].Address , Iter->second.Point );
			return;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CNetDC_DCBase::S_GlobalZoneInfoList		( vector<ZoneConfigBaseStruct>& List )
{
	PG_DBBase_DtoL_GlobalZoneInfoList Send;

	Send.Count = (int)List.size();
	for( int i = 0 ; i < Send.Count ; i++ )
	{
		Send.Info[i] = List[i];
	}
	SendToAllLocal( Send.PGSize() , &Send );
}
//////////////////////////////////////////////////////////////////////////