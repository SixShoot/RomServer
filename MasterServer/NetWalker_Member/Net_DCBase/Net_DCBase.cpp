#include "Net_DCBase.h"
#include "PG/PG_DCBase.h"

//-------------------------------------------------------------------
Net_DCBase*	Net_DCBase::This = NULL;
//-------------------------------------------------------------------
bool Net_DCBase::_Release()
{

	return false;
}
//-------------------------------------------------------------------
bool Net_DCBase::_Init()
{

    _Net->RegOnSrvPacketFunction			( EM_PG_DBBase_RDtoX_SavePlayerError                , _PG_DBBase_RDtoX_SavePlayerError       		    );
    _Net->RegOnSrvPacketFunction			( EM_PG_DCBase_DtoL_SaveNPCResult					, _PG_DCBase_DtoL_SaveNPCResult      				);
    _Net->RegOnSrvPacketFunction			( EM_PG_DBBase_DtoL_DropNPCResult                   , _PG_DBBase_DtoL_DropNPCResult         		    );
    _Net->RegOnSrvPacketFunction			( EM_PG_DBBase_DtoL_LoadAllNPCResult                , _PG_DBBase_DtoL_LoadAllNPCResult      		    );
    _Net->RegOnSrvPacketFunction			( EM_PG_DBBase_RDtoX_SetPlayerLiveTimeError          , _PG_DBBase_RDtoX_SetPlayerLiveTimeError		    );
    _Net->RegOnSrvPacketFunction			( EM_PG_DBBase_RDtoX_SetPlayerLiveTimeError_ByAccount, _PG_DBBase_RDtoX_SetPlayerLiveTimeError_ByAccount);

	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_DtoL_NPCMoveInfoResult				, _PG_DBBase_DtoL_NPCMoveInfoResult					);
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_RDtoX_CheckRoleDataSizeResult		, _PG_DBBase_RDtoX_CheckRoleDataSizeResult			);
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_DtoL_ZoneInfoResult					, _PG_DBBase_DtoL_ZoneInfoResult					);
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_DtoL_GlobalZoneInfoResult			, _PG_DBBase_DtoL_GlobalZoneInfoResult				);

	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_DtoL_GmCommandRequest				, _PG_DBBase_DtoL_GmCommandRequest					);
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_DtoL_CheckRoleLiveTime				, _PG_DBBase_DtoL_CheckRoleLiveTime					);
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_RDtoX_CheckRoleStruct				, _PG_DBBase_RDtoX_CheckRoleStruct					);
//	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_DtoL_SaveRoleDataNotify				, _PG_DBBase_DtoL_SaveRoleDataNotify				);
	_Net->RegOnSrvPacketFunction			( EM_PG_DBBase_DtoL_GlobalZoneInfoList				, _PG_DBBase_DtoL_GlobalZoneInfoList				);

    Global::Net_ServerList->RegServerLoginEvent( EM_SERVER_TYPE_DATACENTER , _OnDataCenterConnectEvent );
	Global::Net_ServerList->RegServerLoginEvent( EM_SERVER_TYPE_ROLECENTER , _OnRoleCenterConnectEvent );
	Global::Net_ServerList->RegServerLogoutEvent( EM_SERVER_TYPE_ROLECENTER , _OnRoleCenterDisconnectEvent );
	return true;
}
//-------------------------------------------------------------------
void Net_DCBase::_PG_DBBase_DtoL_GlobalZoneInfoList				( int NetID , int Size , void* Data )
{
	PG_DBBase_DtoL_GlobalZoneInfoList* PG = ( PG_DBBase_DtoL_GlobalZoneInfoList* )Data;
	This->R_GlobalZoneInfoList( PG->Count , PG->Info ); 
}
void Net_DCBase::_PG_DBBase_RDtoX_CheckRoleStruct				( int NetID , int Size , void* Data )
{
	PG_DBBase_RDtoX_CheckRoleStruct* PG = ( PG_DBBase_RDtoX_CheckRoleStruct* )Data;
	This->R_CheckRoleStruct( PG->Count , PG->Info ); 
}

void Net_DCBase::_PG_DBBase_DtoL_CheckRoleLiveTime				( int NetID , int Size , void* Data )
{
	PG_DBBase_DtoL_CheckRoleLiveTime* PG = (PG_DBBase_DtoL_CheckRoleLiveTime*)Data;
	This->RD_CheckRoleLiveTime( PG->DBID );
}
void Net_DCBase::_PG_DBBase_DtoL_GmCommandRequest				( int NetID , int Size , void* Data )
{
	PG_DBBase_DtoL_GmCommandRequest* PG = (PG_DBBase_DtoL_GmCommandRequest*)Data;
	PG->Info.Command[511] = 0;
	This->RD_GmCommandRequest( PG->Info );
}
/*
void Net_DCBase::_PG_DBBase_DtoL_SaveRoleDataNotify				( int NetID , int Size , void* Data )
{
	PG_DBBase_DtoL_SaveRoleDataNotify* PG = (PG_DBBase_DtoL_SaveRoleDataNotify*)Data;
	This->RD_SaveRoleDataNotify( PG->Account.Begin() , PG->PlayerDBID , PG->ClientID , PG->MasterSockID );
}
*/
void Net_DCBase::_PG_DBBase_DtoL_GlobalZoneInfoResult			( int NetID , int Size , void* Data )
{
	PG_DBBase_DtoL_GlobalZoneInfoResult* PG = (PG_DBBase_DtoL_GlobalZoneInfoResult*)Data;
	This->RD_GlobalZoneInfoResult( PG->Info );
}
void Net_DCBase::_PG_DBBase_DtoL_ZoneInfoResult				( int NetID , int Size , void* Data )
{
	PG_DBBase_DtoL_ZoneInfoResult* PG = (PG_DBBase_DtoL_ZoneInfoResult*)Data;
	This->RD_ZoneInfoResult( PG->Info );
}
void Net_DCBase::_PG_DBBase_RDtoX_CheckRoleDataSizeResult	( int NetID , int Size , void* Data )
{
	PG_DBBase_RDtoX_CheckRoleDataSizeResult* PG = (PG_DBBase_RDtoX_CheckRoleDataSizeResult*)Data;
	This->RD_CheckRoleDataSizeResult( PG->Result );
}
void Net_DCBase::_OnDataCenterConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
    This->OnDataCenterConnect();
}
void Net_DCBase::_OnRoleCenterConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	This->OnRoleCenterConnect( dwServerLocalID );
}
void Net_DCBase::_OnRoleCenterDisconnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	This->OnRoleCenterDisconnect( dwServerLocalID );
}


void Net_DCBase::_PG_DBBase_RDtoX_SavePlayerError         ( int NetID , int Size , void* Data )
{
    PG_DBBase_RDtoX_SavePlayerError* PG = (PG_DBBase_RDtoX_SavePlayerError*)Data;
    if( PG->Result == EM_SavePlayer_PlayerNotFind )
        This->SavePlayerFailed( PG->DBID );
}
void Net_DCBase::_PG_DCBase_DtoL_SaveNPCResult        ( int NetID , int Size , void* Data )
{
    PG_DCBase_DtoL_SaveNPCResult* PG = (PG_DCBase_DtoL_SaveNPCResult*)Data;
    if( PG->Result == EM_SaveNPC_Failed)
    {
        This->SaveNPCResultFailed( PG->LocalObjID );
    }
    else if( PG->Result == EM_SaveNPC_OK )  
    {
        This->SaveNPCResultOK( PG->LocalObjID , PG->NPCDBID );
    }
}
void Net_DCBase::_PG_DBBase_DtoL_DropNPCResult           ( int NetID , int Size , void* Data )
{
    PG_DBBase_DtoL_DropNPCResult* PG = (PG_DBBase_DtoL_DropNPCResult*)Data;
    if( PG->Result == EM_DropNPC_NotFind)
    {
        This->DropNPCResultFailed( PG->LocalObjID , PG->DBID );
    }
    else if( PG->Result == EM_DropNPC_OK )  
    {
        This->DropNPCResultOK( PG->LocalObjID , PG->DBID );
    }
}
void Net_DCBase::_PG_DBBase_DtoL_LoadAllNPCResult        ( int NetID , int Size , void* Data )
{
    PG_DBBase_DtoL_LoadAllNPCResult* PG = (PG_DBBase_DtoL_LoadAllNPCResult*)Data;
    This->LoadNPCResult( PG->TotalCount , PG->ID , &PG->NPC );
}
void Net_DCBase::_PG_DBBase_RDtoX_SetPlayerLiveTimeError  ( int NetID , int Size , void* Data )
{    
    PG_DBBase_RDtoX_SetPlayerLiveTimeError* PG = (PG_DBBase_RDtoX_SetPlayerLiveTimeError*)Data;
    if( PG->Result == EM_SetPlayerLiveTime_PlayerNotFind)
    {
        This->SetPlayerLiveTimeFailed( PG->DBID );
    }
    else if( PG->Result == EM_SetPlayerLiveTime_OK )  
    {

    }
}
void Net_DCBase::_PG_DBBase_RDtoX_SetPlayerLiveTimeError_ByAccount  ( int NetID , int Size , void* Data )
{    
    PG_DBBase_RDtoX_SetPlayerLiveTimeError_ByAccount* PG = (PG_DBBase_RDtoX_SetPlayerLiveTimeError_ByAccount*)Data;
    if( PG->Result == EM_SetPlayerLiveTime_PlayerNotFind)
    {
        This->SetPlayerLiveTimeFailed_ByAccount( PG->Account );
    }
    else if( PG->Result == EM_SetPlayerLiveTime_OK )  
    {

    }
}


void Net_DCBase::_PG_DBBase_DtoL_NPCMoveInfoResult				( int NetID , int Size , void* Data )
{
	PG_DBBase_DtoL_NPCMoveInfoResult* PG = (PG_DBBase_DtoL_NPCMoveInfoResult*)Data;
	This->NPCMoveInfoResult( PG->ZoneID , PG->NPCDBID , PG->TotalCount , PG->IndexID , PG->Base );
}
//-------------------------------------------------------------------
void Net_DCBase::SavePlayer( int ID ,PlayerRoleData* Role , int Start , int Size  , int SaveTime )
{	
	if(     Start + Size > (int)sizeof( PlayerRoleData ) )
	{
		Print( Def_PrintLV3 , "SavePlayer" , "Start + Size > (int)sizeof( RoleData )" );
		return;
	}
	  
	PG_DCBase_XtoRD_SavePlayer Send;
	Send.DBID   = Role->Base.DBID;
//	

	if( Start == 0 && Size == sizeof(PlayerRoleData) )
	{
		Send.Player.Copy( Role );

		RoleDataEx* TempRole = (RoleDataEx*)&Send.Player;
		TempRole->SaveDB_DelTime();

		Send.Player.Base.LogoutTime = RoleDataEx::G_Now;
		if( Role->TempData.Move.CliX != 0 || Role->TempData.Move.CliY != 0 || Role->TempData.Move.CliZ != 0 )
		{
			Send.Player.BaseData.Pos.X = Role->TempData.Move.CliX;
			Send.Player.BaseData.Pos.Y = Role->TempData.Move.CliY;
			Send.Player.BaseData.Pos.Z = Role->TempData.Move.CliZ;
		}
	}
	else
	{
		memcpy( &Send.Player , ((char*)(Role)) + Start , Size );
		//目前只做全部儲存
		return;
	}

	Send.Size   = Size;
	Send.Start  = Start ;
	Send.SaveTime = SaveTime;

	SendToRoleDBCenter( ID , sizeof( Send ) , &Send );
	
}
void Net_DCBase::SaveNPCRequest( RoleDataEx* NPC )
{
    PG_DCBase_LtoD_SaveNPCRequest Send;
    if( NPC->Base.DBID == -2 )
    {
        Print( LV3 , "SaveNPCRequest", "" );
        return;
    }

	GameObjDbStructEx* ObjDB = Global::GetObjDB( NPC->BaseData.ItemInfo.OrgObjID );
	if( ObjDB == NULL )
		return;
	
	if( stricmp( ObjDB->Name , NPC->RoleName() ) == 0  )
		NPC->BaseData.RoleName = "";

    Send.LocalObjID = NPC->TempData.Sys.GUID;
    memcpy( &Send.NPC , NPC , sizeof( Send.NPC ) );

	Send.NPC.BaseData.__MoneyCheckPoint	= _MEMORY_CHECK_VALUE_;
	Send.NPC.SelfData.__MoneyCheckPoint	= _MEMORY_CHECK_VALUE_;

    if( NPC->Base.DBID == -1 )
    {
        NPC->Base.DBID = -2;
    }

    SendToDBCenter( sizeof( Send ) , &Send );
}
void Net_DCBase::DropNPCRequest( int NPCDBID , const char* DestroyAccount )
{
	PG_DBBase_LtoD_DropNPCRequest Send;
	Send.DBID = NPCDBID;
	Send.LocalObjID = -1;
	Send.IsDelFlag = false;
	Send.DestroyAccount = DestroyAccount;

	SendToDBCenter( sizeof( Send ) , &Send );
}

void Net_DCBase::DropNPCRequest( RoleDataEx* NPC )
{
	if( NPC->TempData.Sys.DestroyAccount.Size() == 0 )
	{
		Print( LV5 , "DropNPCRequest" , "Unkwon DestroyAccount LiveTime =%d" , NPC->SelfData.LiveTime );
		return;
	}
	Print( LV2 , "DropNPCRequest" , "DestroyAccount=%s LiveTime =%d" ,  NPC->TempData.Sys.DestroyAccount.Begin() , NPC->SelfData.LiveTime );
    PG_DBBase_LtoD_DropNPCRequest Send;
    Send.DBID = NPC->Base.DBID;
    Send.LocalObjID = NPC->TempData.Sys.GUID;
	//Send.IsDelFlag = NPC->IsFlag();
	Send.IsDelFlag = false;
	Send.DestroyAccount = NPC->TempData.Sys.DestroyAccount;

    SendToDBCenter( sizeof( Send ) , &Send );
}
void Net_DCBase::LoadAllNPC( int ZoneID )
{
    PG_DBBase_LtoD_LoadAllNPC Send;
    Send.ZoneID = ZoneID;

    SendToDBCenter( sizeof( Send ) , &Send );
}
void Net_DCBase::SetPlayerLiveTime( int PlayerCenterID , int DBID , int LiveTime )
{
    PG_DBBase_XtoRD_SetPlayerLiveTime Send;
    Send.DBID = DBID;
    Send.LiveTime = LiveTime;

    SendToRoleDBCenter( PlayerCenterID , sizeof( Send ) , &Send );
}

void Net_DCBase::SetPlayerLiveTime_ByAccount( int PlayerCenterID , char* Account , int LiveTime )
{
    PG_DBBase_XtoRD_SetPlayerLiveTime_ByAccount Send;
    //Send.DBID = DBID;
    MyStrcpy( Send.Account , Account , sizeof(Send.Account ) );
    Send.LiveTime = LiveTime;

    SendToRoleDBCenter( PlayerCenterID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
//要求載入所有npc移動位置的資訊
void Net_DCBase::LoadAllNPCMoveInfo( int ZoneID )
{
	PG_DBBase_LtoD_LoadAllNPCMoveInfo Send;
	Send.ZoneID = ZoneID;
	SendToDBCenter( sizeof( Send ) , &Send );
}

//要求儲存npc移動位置
//void Net_DCBase::SaveNPCMoveInfo( int TotalCount , int ZoneID , int NPCDBID , int IndexID , NPC_MoveBaseStruct& Base )
void Net_DCBase::SaveNPCMoveInfo( int ZoneID , int NPCDBID , vector<NPC_MoveBaseStruct>& List )
{
	PG_DBBase_LtoD_SaveNPCMoveInfo Send;
	Send.TotalCount = (int)List.size();
	Send.ZoneID		= ZoneID;
	Send.NPCDBID	= NPCDBID;
	

	SendToDBCenter( sizeof( Send ) , &Send );

	if( List.size() == 0 )
	{
		Send.IndexID	= -1;
		SendToDBCenter( sizeof( Send ) , &Send );
		return;
	}

	for( int i = 0 ; i < (int)List.size() ; i++ )
	{
		Send.IndexID	= i;
		Send.Base		= List[i];
		SendToDBCenter( sizeof( Send ) , &Send );
	}
}

//-------------------------------------------------------------------	
//要求執行 SQL 命令
void	Net_DCBase::SqlCommand_Import( int ThreadID , const char* SqlCmd )
{
	PG_DBBase_LtoD_SqlCommand Send;
	Send.Type = EM_SqlCommandType_ImportDB;
	Send.ThreadID = ThreadID;
	Send.Data = SqlCmd;
	Send.Size = Send.Data.Size() + 1;
	SendToDBCenter( Send.PGSize() , &Send );
}
void	Net_DCBase::SqlCommand( int ThreadID , const char* SqlCmd )
{
	PG_DBBase_LtoD_SqlCommand Send;
	Send.Type = EM_SqlCommandType_WorldDB;
	Send.ThreadID = ThreadID;
	Send.Data = SqlCmd;
	Send.Size = Send.Data.Size() + 1;
	SendToDBCenter( Send.PGSize() , &Send );
}
void	Net_DCBase::LogSqlCommand( const char* SqlCmd )
{
	PG_DBBase_LtoD_SqlCommand Send;
	Send.Type = EM_SqlCommandType_WorldLogDB;
	Send.ThreadID = rand();
	Send.Data = SqlCmd;
	Send.Size = Send.Data.Size() + 1;
	SendToDBCenter( Send.PGSize() , &Send );
}

void	Net_DCBase::LogSqlCommand( int ThreadID , const char* SqlCmd )
{
	PG_DBBase_LtoD_SqlCommand Send;
	Send.Type = EM_SqlCommandType_WorldLogDB;
	Send.ThreadID = ThreadID;
	Send.Data = SqlCmd;
	Send.Size = Send.Data.Size() + 1;
	SendToDBCenter( Send.PGSize() , &Send );
}

void Net_DCBase::SD_LogData(int LogType, void* Data, unsigned long DataSize)
{
	unsigned long PacketSize = sizeof(PG_DBBase_LtoD_LogData) + DataSize;

	PacketSize = ((PacketSize / 4) + 1) * 4;

	void* pData = NEW BYTE[PacketSize];
	memset(pData, 0, PacketSize);
	PBYTE pTraveler = (PBYTE)pData;

	PG_DBBase_LtoD_LogData* pHeader = (PG_DBBase_LtoD_LogData*)pTraveler;
	pHeader->Command = EM_PG_DBBase_LtoD_LogData;
	pHeader->LogType = LogType;
	pHeader->DataSize = DataSize;

	pTraveler += sizeof(PG_DBBase_LtoD_LogData);

	memcpy(pTraveler, Data, DataSize);

	SendToDBCenter( PacketSize , pData );

	delete [] pData;
}


//檢查角色資料大小是否與datacenter相同
void	Net_DCBase::SD_CheckRoleDataSize( int PlayerCenterID )
{
	PG_DBBase_XtoRD_CheckRoleDataSize Send;
	SendToRoleDBCenter( PlayerCenterID , sizeof( Send ) , &Send );
}

void	Net_DCBase::SD_ZoneInfoRequest( int ZoneID )
{
	PG_DBBase_LtoD_ZoneInfoRequest Send;
	Send.ZoneID = ZoneID;
	SendToDBCenter( sizeof( Send ) , &Send );
}
/*
void	Net_DCBase::SD_SaveRoleDataNotifyResult( const char* Account , int PlayerDBID , int ClientID , int MasterSockID , bool Result )
{
	PG_DBBase_LtoD_SaveRoleDataNotifyResult Send;
	Send.Account = Account;
	Send.PlayerDBID = PlayerDBID;
	Send.Result = Result;
	Send.ClientID = ClientID;
	Send.MasterSockID = MasterSockID;
	SendToDBCenter( sizeof( Send ) , &Send );
}*/
void Net_DCBase::SD_GmCommandResult( int GUID , GmCommandResultENUM Result , const char ResultStr[512] )
{
	PG_DBBase_LtoD_GmCommandResult Send;
	Send.GUID = GUID;
	Send.Result = Result;
//	memcpy( Send.ResultStr , ResultStr , sizeof( Send.ResultStr ) );
	strncpy( Send.ResultStr , ResultStr , sizeof( Send.ResultStr ) );
	SendToDBCenter( sizeof( Send ) , &Send );
}


//////////////////////////////////////////////////////////////////////////
void Net_DCBase::R_CheckRoleStruct( int Count , CheckKeyAddressStruct Info[1000] )
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