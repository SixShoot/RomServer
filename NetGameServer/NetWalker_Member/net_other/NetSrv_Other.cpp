#include "../NetWakerGSrvInc.h"
#include "NetSrv_Other.h"
//-------------------------------------------------------------------
NetSrv_Other*    NetSrv_Other::This = NULL;

//-------------------------------------------------------------------
bool NetSrv_Other::_Init()
{
	_Net->RegOnSrvPacketFunction( EM_PG_Other_LtoL_RequestPlayerPos		, _PG_Other_LtoL_RequestPlayerPos		);
	_Net->RegOnSrvPacketFunction( EM_PG_Other_LtoL_PlayerPos			, _PG_Other_LtoL_PlayerPos				);
	_Net->RegOnSrvPacketFunction( EM_PG_Other_LtoL_CallPlayer			, _PG_Other_LtoL_CallPlayer				);
	_Net->RegOnSrvPacketFunction( EM_PG_Other_DtoL_ExchangeMoneyTable	, _PG_Other_DtoL_ExchangeMoneyTable		);
	_Net->RegOnSrvPacketFunction( EM_PG_Other_DtoL_DBStringTable		, _PG_Other_DtoL_DBStringTable			);
	_Net->RegOnSrvPacketFunction( EM_PG_Other_LtoL_ResetInstance		, _PG_Other_LtoL_ResetInstance			);
	_Net->RegOnSrvPacketFunction( EM_PG_Other_DtoL_SysKeyValue			, _PG_Other_DtoL_SysKeyValue			);
	_Net->RegOnSrvPacketFunction( EM_PG_Other_DtoL_RoleRunPlot			, _PG_Other_DtoL_RoleRunPlot			);

	Srv_NetCliReg( PG_Other_CtoL_DialogSelectID				);
	Srv_NetCliReg( PG_Other_CtoL_BeginCastingRequest		);
	Srv_NetCliReg( PG_Other_CtoL_InterruptCastingRequest	);
	Srv_NetCliReg( PG_Other_CtoL_Performance	);
	Srv_NetCliReg( PG_Other_CtoL_TracertList	);
	Srv_NetCliReg( PG_Other_CtoL_RangeDataTransferRequest	);
	Srv_NetCliReg( PG_Other_CtoL_ColoringShopRequest		);
	Srv_NetCliReg( PG_Other_CtoL_PlayerInfoRequest			);
	Srv_NetCliReg( PG_Other_CtoL_SysKeyFunctionRequest		);
	Srv_NetCliReg( PG_Other_CtoL_AllObjectPosRequest		);
	Srv_NetCliReg( PG_Other_CtoL_RunGlobalPlotRequest		);
	Srv_NetCliReg( PG_Other_CtoL_HateListRequest			);
	Srv_NetCliReg( PG_Other_CtoL_ManagePasswordKey			);
	Srv_NetCliReg( PG_Other_CtoL_ManagePassword				);
	Srv_NetCliReg( PG_Other_CtoL_GiveItemPlot				);
	Srv_NetCliReg( PG_Other_CtoL_Find_DBID_RoleName			);
	Srv_NetCliReg( PG_Other_CtoL_CheckPassword				);
	Srv_NetCliReg( PG_Other_CtoL_DialogInput				);
	Srv_NetCliReg( PG_Other_CtoL_GmNotification				);
	Srv_NetCliReg( PG_Other_CtoL_ColoringHorse				);
	Srv_NetCliReg( PG_Other_CtoL_RoleNameDuplicateErr		);
	Srv_NetCliReg( PG_Other_CtoL_LookTarget					);
	Srv_NetCliReg( PG_Other_CtoL_SetCycleMagic				);
	Srv_NetCliReg( PG_Other_CtoL_CloseMaster				);
	Srv_NetCliReg( PG_Other_CtoL_SetSendPacketMode			);

	Srv_NetCliReg( PG_Other_CtoL_ResetInstanceRequest		);
	Srv_NetCliReg( PG_Other_CtoL_GmResetRole				);
	Srv_NetCliReg( PG_Other_CtoL_PartyDice					);
	Srv_NetCliReg( PG_Other_CtoL_BodyShop					);
	
	Srv_NetCliReg( PG_Other_CtoL_MoneyVendorReport			);
	Srv_NetCliReg( PG_Other_CtoL_BotReport					);
	Srv_NetCliReg( PG_Other_CtoL_UnlockBotReport			);

	Srv_NetCliReg( PG_Other_CtoL_SortBankRequest			);

	Srv_NetCliReg( PG_Other_CtoL_LoadClientData				);
	Srv_NetCliReg( PG_Other_CtoL_SaveClientData				);

	Srv_NetCliReg( PG_Other_CtoL_Suicide					);
	Srv_NetCliReg( PG_Other_CtoL_SpellExSkill				);

	Srv_NetCliReg( PG_Other_CtoL_ClientEventLog				);
	Srv_NetCliReg( PG_Other_CtoL_TransferData_Range			);

	Srv_NetCliReg( PG_Other_CtoL_SetRoleValue				);
	Srv_NetCliReg( PG_Other_CtoL_Client_Language			);

	Srv_NetCliReg( PG_Other_CtoL_SetNewTitleSys				);
	Srv_NetCliReg( PG_Other_CtoL_NewTitleSys_UseItem		);

	Srv_NetCliReg( PG_Other_CtoL_OpenSession				);
	Srv_NetCliReg( PG_Other_CtoL_CloseSession				);
	Srv_NetCliReg( PG_Other_CtoL_AutoReportPossibleCheater	);


	Server_Reg_Server_Packet( PG_Other_LtoL_PCall			);


	Server_Reg_Server_Packet	( PG_Other_DtoL_WeekInstances			);
	Srv_NetCliReg				( PG_Other_CtoL_WeekInstancesRequest 	);
	Srv_NetCliReg				( PG_Other_CtoL_WeekInstancesTeleport	);
	Srv_NetCliReg				( PG_Other_CtoL_WeekInstancesZoneReset	);
	Server_Reg_Server_Packet	( PG_Other_LtoL_WeekInstancesZoneReset	);
	Srv_NetCliReg				( PG_Other_CtoL_WeekInstancesReset	 	);

	Srv_NetCliReg( PG_Other_CtoL_PhantomRankUp 		);
	Srv_NetCliReg( PG_Other_CtoL_PhantomLevelUp 	);
	Srv_NetCliReg( PG_Other_CtoL_PhantomEQ 			);

	Server_Reg_Server_Packet	( PG_Other_DtoL_BuffSchedule 			);
	Server_Reg_Server_Packet	( PG_Other_DtoL_ItemCombinInfo 			);

    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Other::_Release()
{
    return true;
}

//-------------------------------------------------------------------
void NetSrv_Other::_PG_Other_DtoL_ItemCombinInfo( int NetID , int size , void* data )
{
	PG_Other_DtoL_ItemCombinInfo* pg = (PG_Other_DtoL_ItemCombinInfo*)data;
	This->RD_ItemCombinInfo( pg->GroupID , pg->ItemCombine );
}
void NetSrv_Other::_PG_Other_DtoL_BuffSchedule( int NetID , int size , void* data )
{
	PG_Other_DtoL_BuffSchedule* pg = (PG_Other_DtoL_BuffSchedule*)data;
	This->RD_BuffSchedule( pg->BuffID , pg->BuffLv , pg->BuffTime );
}
void NetSrv_Other::_PG_Other_CtoL_PhantomRankUp(  int NetID , int size , void* data )
{
	BaseItemObject*	obj = BaseItemObject::GetObjBySockID( NetID );
	if( obj == NULL )
		return ;
	PG_Other_CtoL_PhantomRankUp* pg = (PG_Other_CtoL_PhantomRankUp*)data;
	This->RC_PhantomRankUp( obj , pg->PhantomID , pg->Summon );
}
void NetSrv_Other::_PG_Other_CtoL_PhantomLevelUp(  int NetID , int size , void* data )
{
	BaseItemObject*	obj = BaseItemObject::GetObjBySockID( NetID );
	if( obj == NULL )
		return ;
	PG_Other_CtoL_PhantomLevelUp* pg = (PG_Other_CtoL_PhantomLevelUp*)data;
	This->RC_PhantomLevelUp( obj , pg->PhantomID );
}
void NetSrv_Other::_PG_Other_CtoL_PhantomEQ(  int NetID , int size , void* data )
{
	BaseItemObject*	obj = BaseItemObject::GetObjBySockID( NetID );
	if( obj == NULL )
		return ;
	PG_Other_CtoL_PhantomEQ* pg = (PG_Other_CtoL_PhantomEQ*)data;
	This->RC_PhantomEQ( obj , pg->EQ );
}

void NetSrv_Other::_PG_Other_DtoL_WeekInstances(  int NetID , int size , void* data )
{
	PG_Other_DtoL_WeekInstances* pg = (PG_Other_DtoL_WeekInstances*)data;
	This->RD_WeekInstances( pg->Count , pg->Info );
}
void NetSrv_Other::_PG_Other_CtoL_WeekInstancesRequest(  int NetID , int size , void* data )
{
	BaseItemObject*	obj = BaseItemObject::GetObjBySockID( NetID );
	if( obj == NULL )
		return ;
	PG_Other_CtoL_WeekInstancesRequest* pg = (PG_Other_CtoL_WeekInstancesRequest*)data;
	This->RC_WeekInstancesRequest( obj  );
}
void NetSrv_Other::_PG_Other_CtoL_WeekInstancesTeleport(  int NetID , int size , void* data )
{
	BaseItemObject*	obj = BaseItemObject::GetObjBySockID( NetID );
	if( obj == NULL )
		return ;
	PG_Other_CtoL_WeekInstancesTeleport* pg = (PG_Other_CtoL_WeekInstancesTeleport*)data;
	This->RC_WeekInstancesTeleport( obj , pg->ZoneID );
}
void NetSrv_Other::_PG_Other_CtoL_WeekInstancesZoneReset(  int NetID , int size , void* data )
{
	BaseItemObject*	obj = BaseItemObject::GetObjBySockID( NetID );
	if( obj == NULL )
		return ;
	PG_Other_CtoL_WeekInstancesZoneReset* pg = (PG_Other_CtoL_WeekInstancesZoneReset*)data;
	This->RC_WeekInstancesZoneReset( obj , pg->ZoneID );
}
void NetSrv_Other::_PG_Other_LtoL_WeekInstancesZoneReset(  int NetID , int size , void* data )
{
	PG_Other_LtoL_WeekInstancesZoneReset* pg = (PG_Other_LtoL_WeekInstancesZoneReset*)data;
	This->RL_WeekInstancesZoneReset( NetID  , pg->DBID , pg->PartyID );
}
void NetSrv_Other::_PG_Other_CtoL_WeekInstancesReset(  int NetID , int size , void* data )
{
	BaseItemObject*	obj = BaseItemObject::GetObjBySockID( NetID );
	if( obj == NULL )
		return ;
	PG_Other_CtoL_WeekInstancesReset* pg = (PG_Other_CtoL_WeekInstancesReset*)data;
	This->RC_WeekInstancesReset( obj  );
}
void NetSrv_Other::_PG_Other_CtoL_AutoReportPossibleCheater	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_AutoReportPossibleCheater* pg =(PG_Other_CtoL_AutoReportPossibleCheater*)Data;
	This->RC_AutoReportPossibleCheater( Obj , pg->TargetInfo , pg->Type );
}
void NetSrv_Other::_PG_Other_LtoL_PCall( int NetID , int Size , void* Data )
{
	PG_Other_LtoL_PCall* pg =(PG_Other_LtoL_PCall*)Data;
	This->RL_PCall( pg->PlotCmd.Begin() );
}
void NetSrv_Other::_PG_Other_CtoL_NewTitleSys_UseItem( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_NewTitleSys_UseItem* pg =(PG_Other_CtoL_NewTitleSys_UseItem*)Data;
	This->RC_NewTitleSys_UseItem( Obj , pg->PosType , pg->Pos );
}
void NetSrv_Other::_PG_Other_CtoL_SetNewTitleSys( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_SetNewTitleSys* pg =(PG_Other_CtoL_SetNewTitleSys*)Data;
	This->RC_SetNewTitleSys( Obj , pg->Pos , pg->TitleID );
}
void NetSrv_Other::_PG_Other_DtoL_SysKeyValue		( int NetID , int Size , void* Data )
{
	PG_Other_DtoL_SysKeyValue* pg =(PG_Other_DtoL_SysKeyValue*)Data;

	This->RD_SysKeyValue( pg->TotalCount , pg->ID , pg->KeyStr , pg->Value );
}

void NetSrv_Other::_PG_Other_CtoL_SetRoleValue		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_SetRoleValue* pg =(PG_Other_CtoL_SetRoleValue*)Data;
	This->RC_SetRoleValue( Obj , pg->Type , pg->Value );
}

void NetSrv_Other::_PG_Other_CtoL_TransferData_Range( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_TransferData_Range* pg =(PG_Other_CtoL_TransferData_Range*)Data;
	This->RC_TransferData_Range( Obj , pg->Data );
}

void NetSrv_Other::_PG_Other_CtoL_ClientEventLog	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_ClientEventLog* pg =(PG_Other_CtoL_ClientEventLog*)Data;
	This->RC_ClientEventLog( Obj , pg->EventType );
}

void NetSrv_Other::_PG_Other_CtoL_SpellExSkill		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_SpellExSkill* pg =(PG_Other_CtoL_SpellExSkill*)Data;
	This->RC_SpellExSkill( Obj , pg->TargetID , pg->TargetX , pg->TargetY , pg->TargetZ  , pg->Info , pg->ShootAngle , pg->ExplodeTime );
}
void NetSrv_Other::_PG_Other_CtoL_Suicide					( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_Suicide* pg =(PG_Other_CtoL_Suicide*)Data;
	This->RC_Suicide( Obj , pg->Type );
}

void NetSrv_Other::_PG_Other_CtoL_LoadClientData			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_LoadClientData* pg =(PG_Other_CtoL_LoadClientData*)Data;
	This->RC_LoadClientData( Obj , pg->KeyID );
}
void NetSrv_Other::_PG_Other_CtoL_SaveClientData			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_SaveClientData* pg =(PG_Other_CtoL_SaveClientData*)Data;
	This->RC_SaveClientData( Obj , pg->KeyID , pg->PageID , pg->Size , pg->Data );
}
void NetSrv_Other::_PG_Other_CtoL_SortBankRequest			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_SortBankRequest* pg =(PG_Other_CtoL_SortBankRequest*)Data;
	This->RC_SortBankRequest( Obj );
}

void NetSrv_Other::_PG_Other_CtoL_UnlockBotReport			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_UnlockBotReport* pg =(PG_Other_CtoL_UnlockBotReport*)Data;
	This->RC_UnlockBotReport( Obj );
}

void NetSrv_Other::_PG_Other_CtoL_MoneyVendorReport			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_MoneyVendorReport* pg =(PG_Other_CtoL_MoneyVendorReport*)Data;
	This->RC_MoneyVendorReport( Obj , pg->Name.Begin() , pg->Content.Begin() );
}
void NetSrv_Other::_PG_Other_CtoL_BotReport					( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_BotReport* pg =(PG_Other_CtoL_BotReport*)Data;
	This->RC_BotReport( Obj , pg->Name.Begin() , pg->Content.Begin() );
}

void NetSrv_Other::_PG_Other_CtoL_BodyShop					( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_BodyShop* pg =(PG_Other_CtoL_BodyShop*)Data;
	This->RC_BodyShop( Obj , pg->BoneScale );
}
void NetSrv_Other::_PG_Other_CtoL_PartyDice					( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_PartyDice* pg =(PG_Other_CtoL_PartyDice*)Data;
	This->RC_PartyDice( Obj , pg->Type );
}

void NetSrv_Other::_PG_Other_CtoL_GmResetRole				( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_GmResetRole* pg =(PG_Other_CtoL_GmResetRole*)Data;
	This->RC_GmResetRolet( Obj , pg->RoleBaseSize , pg->PlayerRoleBaseSize , pg->BaseData , pg->PlayerBaseData  );
}

void NetSrv_Other::_PG_Other_CtoL_ResetInstanceRequest		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_ResetInstanceRequest* pg =(PG_Other_CtoL_ResetInstanceRequest	*)Data;
	This->RC_ResetInstanceRequest( Obj , pg->ZoneID );
}
void NetSrv_Other::_PG_Other_LtoL_ResetInstance				( int NetID , int Size , void* Data )
{
	PG_Other_LtoL_ResetInstance* pg =(PG_Other_LtoL_ResetInstance*)Data;
	This->RL_ResetInstance( pg->PlayerDBID , pg->PartyID );
}


void NetSrv_Other::_PG_Other_CtoL_SetSendPacketMode			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_SetSendPacketMode* pg =(PG_Other_CtoL_SetSendPacketMode	*)Data;
	This->RC_SetSendPacketMode( Obj , pg->Mode );
}
void NetSrv_Other::_PG_Other_CtoL_CloseMaster	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_CloseMaster* pg =(PG_Other_CtoL_CloseMaster	*)Data;
	This->RC_CloseMaster( Obj , pg->Password );
}

void NetSrv_Other::_PG_Other_CtoL_SetCycleMagic			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_SetCycleMagic	* pg =(PG_Other_CtoL_SetCycleMagic	*)Data;
	This->RC_SetCycleMagic( Obj  , pg->MagicID , pg->MagicPos );
}
void NetSrv_Other::_PG_Other_CtoL_LookTarget		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_LookTarget	* pg =(PG_Other_CtoL_LookTarget	*)Data;
	This->RC_LookTarget( Obj , pg->TargetGUID );
}
void NetSrv_Other::_PG_Other_CtoL_ColoringHorse		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_ColoringHorse	* pg =(PG_Other_CtoL_ColoringHorse	*)Data;
	This->RC_ColoringHorse( Obj , pg->Color , (char*)pg->Password.Begin() , pg->ItemPos );
}

void NetSrv_Other::_PG_Other_CtoL_RoleNameDuplicateErr		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_RoleNameDuplicateErr	* pg =(PG_Other_CtoL_RoleNameDuplicateErr	*)Data;
	This->RC_RoleNameDuplicateErr( Obj , pg->RoleName.Begin() , pg->PlayerDBID );
}

void NetSrv_Other::_PG_Other_DtoL_ExchangeMoneyTable( int NetID , int Size , void* Data )
{
	PG_Other_DtoL_ExchangeMoneyTable* pg =(PG_Other_DtoL_ExchangeMoneyTable*)Data;

	This->RD_ExchangeMoneyTable( pg->Count , pg->DataSize , pg->Data );
}
void NetSrv_Other::_PG_Other_DtoL_DBStringTable( int NetID , int Size , void* Data )
{
	PG_Other_DtoL_DBStringTable* pg =(PG_Other_DtoL_DBStringTable*)Data;

	This->RD_DBStringTable( pg->Count , pg->DataSize , pg->Data );
}

//-------------------------------------------------------------------
void NetSrv_Other::_PG_Other_CtoL_GmNotification( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj = BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_GmNotification* pg =(PG_Other_CtoL_GmNotification*)Data;

	This->RC_GmNotification( Obj, pg->Subject, pg->Content, pg->Classification);
}
//-------------------------------------------------------------------
void NetSrv_Other::_PG_Other_CtoL_DialogInput( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_DialogInput* pg =(PG_Other_CtoL_DialogInput*)Data;

	This->R_DialogInput( Obj, pg->SelectID, pg->pszString, pg->iCheckResult );
}
void NetSrv_Other::_PG_Other_CtoL_Find_DBID_RoleName	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_Find_DBID_RoleName* pg =(PG_Other_CtoL_Find_DBID_RoleName*)Data;
	This->RC_Find_DBID_RoleName( Obj , pg->DBID , (char*)pg->RoleName.Begin() );
}
void NetSrv_Other::_PG_Other_CtoL_Performance		( int NetID , int Size , void* Data )
{
	PG_Other_CtoL_Performance* PG =(PG_Other_CtoL_Performance*)Data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	char* Mac = PG->cMac;
	char InsertCmd[0x10000];
	__time32_t t = _time32(NULL);
	struct tm* tm = _localtime32(&t);
	sprintf_s( InsertCmd , sizeof(InsertCmd) , 
		"Insert PerformanceLog(Mac ,MaxCPUUsage ,MaxMemUsage, MaxFPS, AverageCPUUsage, AverageMemUsage, AverageFPS, MinCPUUsage, MinMemUsage, MinFPS, Ping, PosX, PosY, PosZ, DateTime )"
		"VALUES ('%s',%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,'%d-%d-%d %d:%d:%d')"
		,Mac, PG->MaxCPUUsage, PG->MaxMemUsage, PG->MaxFPS, PG->AverageCPUUsage, PG->AverageMemUsage, PG->AverageFPS, PG->MinCPUUsage, PG->MinMemUsage, PG->MinFPS, PG->Ping, PG->PosX, PG->PosY, PG->PosZ, tm->tm_year + 1900,	tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min,	tm->tm_sec);
	Net_DCBase::LogSqlCommand(InsertCmd);
	//This->RC_SendPerformance( Obj, Mac, PG->MaxCPUUsage, PG->MaxMemUsage, PG->MaxFPS, PG->AverageCPUUsage, PG->AverageMemUsage, PG->AverageFPS, PG->MinCPUUsage, PG->MinMemUsage, PG->MinFPS, PG->Ping, PG->PosX, PG->PosY, PG->PosZ );
}
void NetSrv_Other::_PG_Other_CtoL_TracertList		( int NetID , int Size , void* Data )
{
	PG_Other_CtoL_TracertList* PG =(PG_Other_CtoL_TracertList*)Data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	char InsertCmd[0x10000];
	__time32_t t = _time32(NULL);
	struct tm* tm = _localtime32(&t);
	sprintf_s( InsertCmd , sizeof(InsertCmd) , 
		"Insert TracertListLog(DBID ,Ping, Tracertlist, DateTime)"
		"VALUES (%d, %d, cast( '%s' as varbinary),'%d-%d-%d %d:%d:%d')"
		,PG->DBID, PG->Ping, StringToSqlX( (reinterpret_cast<const char*>(&PG->List)), sizeof(PG->List) , false ).c_str() , tm->tm_year + 1900,	tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min,	tm->tm_sec);
	Net_DCBase::LogSqlCommand(InsertCmd);

}

void NetSrv_Other::_PG_Other_CtoL_CheckPassword				( int NetID , int Size , void* Data )
{ 
	PG_Other_CtoL_CheckPassword* PG =(PG_Other_CtoL_CheckPassword*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	This->R_CheckPassword( Obj , (char*)PG->Password.Begin() );
}
void NetSrv_Other::_PG_Other_CtoL_GiveItemPlot				( int NetID , int Size , void* Data )
{
	PG_Other_CtoL_GiveItemPlot* PG =(PG_Other_CtoL_GiveItemPlot*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	This->R_GiveItemPlot( Obj , PG->IsSort , PG->ItemPos );
}
void NetSrv_Other::_PG_Other_CtoL_ManagePasswordKey			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	This->R_ManagePasswordKey( Obj );
}
void NetSrv_Other::_PG_Other_CtoL_ManagePassword			( int NetID , int Size , void* Data )
{
	PG_Other_CtoL_ManagePassword* PG =(PG_Other_CtoL_ManagePassword*)Data;
	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	This->R_ManagePassword( Obj , PG->MD5Pwd.Begin() );
}

void NetSrv_Other::_PG_Other_CtoL_HateListRequest		( int NetID , int Size , void* Data )
{
	PG_Other_CtoL_HateListRequest* PG =(PG_Other_CtoL_HateListRequest*)Data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	This->R_HateListRequest( Obj , PG->NpcGUID );
}

void NetSrv_Other::_PG_Other_CtoL_RunGlobalPlotRequest	( int NetID , int Size , void* Data )
{
	PG_Other_CtoL_RunGlobalPlotRequest* PG =(PG_Other_CtoL_RunGlobalPlotRequest*)Data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	This->R_RunGlobalPlotRequest( Obj , PG->PlotID , PG->Value );
}
void NetSrv_Other::_PG_Other_CtoL_AllObjectPosRequest	( int NetID , int Size , void* Data )
{
	PG_Other_CtoL_AllObjectPosRequest* PG =(PG_Other_CtoL_AllObjectPosRequest*)Data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	This->R_AllObjectPosRequest( Obj );
}

void NetSrv_Other::_PG_Other_CtoL_SysKeyFunctionRequest		( int NetID , int Size , void* Data )
{
	PG_Other_CtoL_SysKeyFunctionRequest* PG =(PG_Other_CtoL_SysKeyFunctionRequest*)Data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	This->R_SysKeyFunctionRequest( Obj , PG->KeyID , PG->TargetGUID );
}

void NetSrv_Other::_PG_Other_CtoL_PlayerInfoRequest			( int NetID , int Size , void* Data )
{
	PG_Other_CtoL_PlayerInfoRequest* PG =(PG_Other_CtoL_PlayerInfoRequest*)Data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	This->R_PlayerInfoRequest( Obj , PG->GItemID );
}
void NetSrv_Other::_PG_Other_CtoL_ColoringShopRequest			( int NetID , int Size , void* Data )
{
	PG_Other_CtoL_ColoringShopRequest* PG =(PG_Other_CtoL_ColoringShopRequest*)Data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	This->R_ColoringShopRequest( Obj , PG->ColorInfo , PG->Password.Begin() );
}

void NetSrv_Other::_PG_Other_CtoL_RangeDataTransferRequest		( int NetID , int Size , void* Data )
{
	PG_Other_CtoL_RangeDataTransferRequest* PG =(PG_Other_CtoL_RangeDataTransferRequest*)Data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	This->R_RangeDataTransferRequest( Obj , PG->Range , PG->DataSize , PG->Data );
}

void NetSrv_Other::_PG_Other_CtoL_BeginCastingRequest			( int NetID , int Size , void* Data )
{
	PG_Other_CtoL_BeginCastingRequest* PG =(PG_Other_CtoL_BeginCastingRequest*)Data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	This->R_BeginCastingRequest( Obj , PG->Type , PG->ItemID , PG->TargetGUID );
}
void NetSrv_Other::_PG_Other_CtoL_InterruptCastingRequest		( int NetID , int Size , void* Data )
{
	PG_Other_CtoL_InterruptCastingRequest* PG =(PG_Other_CtoL_InterruptCastingRequest*)Data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	This->R_InterruptCastingRequest( Obj );
}

void NetSrv_Other::_PG_Other_CtoL_DialogSelectID				( int Sockid , int Size , void* Data )
{
	PG_Other_CtoL_DialogSelectID* PG =(PG_Other_CtoL_DialogSelectID*)Data;

	BaseItemObject*	Obj =	Global::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return ;

	This->R_DialogSelectID( Obj , PG->SelectID );
}

void NetSrv_Other::_PG_Other_LtoL_RequestPlayerPos	( int NetID , int Size , void* Data )
{
    PG_Other_LtoL_RequestPlayerPos* pg =(PG_Other_LtoL_RequestPlayerPos*)Data;
	This->R_RequestPlayerPos( pg->ZoneID , pg->DBID , (char*)pg->Name.Begin() );
}
void NetSrv_Other::_PG_Other_LtoL_PlayerPos			( int NetID , int Size , void* Data )	
{
	PG_Other_LtoL_PlayerPos* pg =(PG_Other_LtoL_PlayerPos*)Data;
	This->R_PlayerPos( pg->ZoneID , pg->DBID , pg->RoomID , pg->Pos );
}
void NetSrv_Other::_PG_Other_LtoL_CallPlayer		( int NetID , int Size , void* Data )
{
	PG_Other_LtoL_CallPlayer* pg =(PG_Other_LtoL_CallPlayer*)Data;
	This->R_CallPlayer( pg->ZoneID , pg->Pos , (char*)pg->Name.Begin() , pg->RoomID );
}

void NetSrv_Other::_PG_Other_CtoL_Client_Language( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_Client_Language* pg =(PG_Other_CtoL_Client_Language*)Data;

	This->RC_ClientLanguage( Obj , pg->iLanguage );
}

void NetSrv_Other::_PG_Other_DtoL_RoleRunPlot( int NetID , int Size , void* Data )
{
	PG_Other_DtoL_RoleRunPlot* pg =(PG_Other_DtoL_RoleRunPlot*)Data;
	This->RD_RoleRunPlot( pg->LuaPlot.Begin() , pg->DBID );
}

void NetSrv_Other::_PG_Other_CtoL_OpenSession( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_OpenSession* pg = (PG_Other_CtoL_OpenSession*)Data;
	This->RC_OpenSession( Obj, pg->Type, pg->CBID );
}

void NetSrv_Other::_PG_Other_CtoL_CloseSession( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	Global::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Other_CtoL_CloseSession* pg =(PG_Other_CtoL_CloseSession*)Data;
	This->RC_CloseSession( Obj, pg->Type, pg->SessionID );
}

void NetSrv_Other::S_RequestPlayerPos	( int DBID , char* Name )
{
	PG_Other_LtoL_RequestPlayerPos Send;
	Send.DBID = DBID;
	Send.Name = Name;
	Send.ZoneID = RoleDataEx::G_ZoneID;

	Global::SendToAllLocal( sizeof( Send ) , &Send );
}
void NetSrv_Other::S_PlayerPos			( int SendZoneID , int DBID , int RoomID , RolePosStruct& Pos )
{
	PG_Other_LtoL_PlayerPos Send;
	Send.DBID = DBID;
	Send.Pos  = Pos;
	Send.ZoneID = RoleDataEx::G_ZoneID;
	Send.RoomID = RoomID;

	SendToLocal( SendZoneID , sizeof( Send ) , &Send );
}
void NetSrv_Other::S_CallPlayer			( RolePosStruct& Pos , char* Name , int RoomID )
{
	PG_Other_LtoL_CallPlayer Send;
	Send.Pos	= Pos;
	Send.Name	= Name;
	Send.RoomID = RoomID;
	Send.ZoneID = RoleDataEx::G_ZoneID;

	Global::SendToAllLocal( sizeof( Send ) , &Send );
}

void NetSrv_Other::S_OpenDialog			( int SendID , LuaDialogType_ENUM Type , const char* Content )
{
	PG_Other_LtoC_OpenDialog Send;
	Send.Type = Type;
	Send.Content = Content;
	SendToCli_ByGUID( SendID , sizeof(Send ) , &Send );
}
void NetSrv_Other::S_DialogSelectStr	( int SendID , const char* SelectStr )
{
	PG_Other_LtoC_DialogSelectStr Send;
	Send.SelectStr = SelectStr;
	SendToCli_ByGUID( SendID , sizeof(Send ) , &Send );
}
void NetSrv_Other::S_DialogSelectStrEnd	( int SendID )
{
	This->m_mapDialogInfo.erase( SendID );
	PG_Other_LtoC_DialogSelectStrEnd Send;
	SendToCli_ByGUID( SendID , sizeof(Send ) , &Send );
}
void NetSrv_Other::S_CloseDialog		( int SendID )
{
	This->m_mapDialogInfo.erase( SendID );

	PG_Other_LtoC_CloseDialog Send;
	SendToCli_ByGUID( SendID , sizeof(Send ) , &Send );
}

void NetSrv_Other::S_DialogSetContent	( int SendID , const char* Content )
{
	PG_Other_LtoC_DialogSetContent Send;
	Send.Content = Content;
	SendToCli_ByGUID( SendID , sizeof(Send ) , &Send );
}
/*
void NetSrv_Other::S_SystemMessage		( int SendID , SystemMessageENUM Msg )
{
	PG_Other_XtoC_SystemMessage	Send;
	Send.Msg = Msg;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
*/
/*
void NetSrv_Other::S_SystemMessageData	( int SendID , SystemMessageDataENUM Msg , int Size , const char* Data )
{
	PG_Other_XtoC_SystemMessageData	Send;
	Send.Msg = Msg;
	Send.DataSize = Size;
	memcpy( Send.Data , Data , Size );
	SendToCli_ByGUID( SendID ,Send.Size() , &Send );
}
*/
void NetSrv_Other::S_BeginCastingResult( int SendID , int ItemID , CastingTypeENUM Type , int CastingTime , bool Result )
{
	PG_Other_LtoC_BeginCastingResult Send;
	Send.Type = Type ;
	Send.Result = Result;
	Send.CastingTime = CastingTime;
	Send.ItemID = ItemID;
	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );

}
void NetSrv_Other::S_InterruptCasting	( int SendID , CastingTypeENUM Type )
{
	PG_Other_LtoC_InterruptCasting Send;
	Send.Type = Type ;
	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );
}
void NetSrv_Other::S_CastingOK			( int SendID , CastingTypeENUM Type )
{
	PG_Other_LtoC_CastingOK Send;
	Send.Type = Type ;
	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );
}

/*
void NetSrv_Other::S_ItemExchangeResult( int SendID , ItemExchangeResultTypeENUM	ResultType , ItemFieldStruct& Item , int Money , int Money_Account )
{
	PG_Other_LtoC_ItemExchangeResult Send;
	Send.ResultType = ResultType;
	Send.Item = Item;
	Send.Money = Money;
	Send.Money_Account = Money_Account;

	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );
}
*/
void NetSrv_Other::S_RangeDataTransfer	( int SendID , int Range , int Size , const char* Data )
{
	if( Size > _MAX_CLIENT_TRANSFER_DATA_SIZE )
		return;

	PG_Other_LtoC_RangeDataTransfer Send;
	memcpy( Send.Data , Data , Size );
	Send.DataSize = Size;

	RoleDataEx* Owner = Global::GetRoleDataByGUID( SendID );
	if( Owner == NULL )
		return;

	RoleDataEx* Other;
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;

		if( Other->Length( Owner ) > Range )
			continue;

		SendToCli_ByGUID( Other->GUID() , Send.Size() , &Send );
	}
}
void NetSrv_Other::S_ColoringShopResult( int SendID , ColoringShopResultENUM Result )
{
	PG_Other_LtoC_ColoringShopResult Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );
}
void NetSrv_Other::S_PlayerInfo		( int SendID , bool Result , int GItemID , EQStruct* EQ , const char* Note )
{
	PG_Other_LtoC_PlayerInfoResult Send;
	Send.Result = Result;
	Send.GItemID = GItemID;
	if( EQ != NULL )
		Send.EQ = *EQ;
	if( Note != NULL )
		Send.Note = Note;

	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );
}

void NetSrv_Other::S_SysKeyFunctionResult( int SendID , bool Result , int KeyID , int TargetGUID )
{
	PG_Other_LtoC_SysKeyFunctionResult Send;
	Send.Result = Result;
	Send.KeyID = KeyID;
	Send.TargetGUID = TargetGUID;
	
	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );
}
void NetSrv_Other::S_ObjectPosInfo		( int SendID , vector<ObjectPosInfoStruct>& Info )
{
//	if( Info.size() > 10000 )
//		return;
	MyLzoClass myZip;

	PG_Other_LtoC_ObjectPosInfo Send;
	Send.Count = int( Info.size() );
	Send.DataSize =  myZip.Encode( (char*)(&Info[0]) , sizeof( ObjectPosInfoStruct ) * Send.Count , (char*)&Send.Data );

	SendToCli_ByGUID( SendID , Send.Size() , &Send );
}

void NetSrv_Other::S_OpenClientMenu	( int SendID , ClientMenuType_ENUM Type )
{
	PG_Other_LtoC_OpenClientMenu Send;
	Send.MenuType = Type;
	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );
}

void NetSrv_Other::S_DialogSetTitle( int SendID , const char* Title )
{
	PG_Other_LtoC_DialogSetTitle Send;
	Send.Title = Title;
	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );
}

void NetSrv_Other::S_RunGlobalPlotResult( int SendID , bool Result )
{
	PG_Other_LtoC_RunGlobalPlotResult Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );
}

void NetSrv_Other::S_HateListResult	( int SendID , int NpcGUID , NPCHateList& NPCHate )
{
	PG_Other_LtoC_HateListResult Send;
	Send.NpcGUID = NpcGUID;
	Send.NPCHate = NPCHate;
	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );
}
void NetSrv_Other::S_OpenGiveItemPlot	( int SendID , int TargetGUID , bool IsSort , int Count )
{
	PG_Other_LtoC_OpenGiveItemPlot Send;
	Send.IsSort = IsSort;
	Send.Count = Count;
	Send.TargetGUID = TargetGUID;
	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );
}

void NetSrv_Other::S_CheckPasswordResult( int SendID , char* Password , bool Result )
{
	PG_Other_LtoC_CheckPasswordResult Send;
	Send.Password = Password;
	Send.Result = Result;
	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );
}
void NetSrv_Other::SD_Find_DBID_RoleName( int CliDBID , int FindDBID , const char* FindRoleName )
{
	PG_Other_LtoD_Find_DBID_RoleName Send;
	Send.DBID = FindDBID;
	Send.RoleName = FindRoleName;
	Send.CliDBID = CliDBID;
	Global::SendToDBCenter( sizeof(Send) , &Send );
}
//void NetSrv_Other::SD_SendPerformance		(int CliDBID , const char* cMac, float MaxCPUUsage, float MaxMemUsage, float MaxFPS, float AverageCPUUsage, float AverageMemUsage, float AverageFPS, float MinCPUUsage, float MinMemUsage, float MinFPS, float Ping, float PosX, float PosY, float PosZ )
//{
//	PG_Other_LtoD_Performance Send;
//	Send.CliDBID = CliDBID;
//	strcpy(Send.cMac, cMac);
//	Send.MaxCPUUsage = MaxCPUUsage;
//	Send.MaxMemUsage = MaxMemUsage;
//	Send.MaxFPS = MaxFPS;
//	Send.AverageCPUUsage = AverageCPUUsage;
//	Send.AverageMemUsage = AverageMemUsage;
//	Send.AverageFPS = AverageFPS;
//	Send.MinCPUUsage = MinCPUUsage;
//	Send.MinMemUsage = MinMemUsage;
//	Send.MinFPS = MinFPS;
//	Send.Ping = Ping;
//	Send.PosX = PosX;
//	Send.PosY = PosY;
//	Send.PosZ = PosZ;
//	Global::SendToDBCenter( sizeof(Send) , &Send );
//}
void NetSrv_Other::S_GoodEvilValue		( int SendID , int PlayerGUID , float GoodEvilValue , GoodEvilTypeENUM GoodEvilType )
{
	PG_Other_LtoC_GoodEvilValue Send;
	Send.PlayerGUID = PlayerGUID;
	Send.GoodEvilValue = GoodEvilValue;
	Send.GoodEvilType = GoodEvilType;
	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );
}

void NetSrv_Other::S_CampID			( int SendID , int GItemID , CampID_ENUM CampID )
{
	PG_Other_LtoC_CampID Send;
	Send.GItemID = GItemID;
	Send.CampID = CampID;
	SendToCli_ByGUID( SendID ,sizeof( Send ) , &Send );
}

int NetSrv_Other::GetDialogInputCheckResult( int iObjID )
{
	map< int, Struct_DialogTempInfo >::iterator it = This->m_mapDialogInfo.find( iObjID );

	const char* pszStr = NULL;

	if( it != This->m_mapDialogInfo.end() )
	{
		return it->second.isAllowString;
	}

	return 0;
}


const char* NetSrv_Other::GetDialogInputStr( int iObjID )
{
	map< int, Struct_DialogTempInfo >::iterator it = This->m_mapDialogInfo.find( iObjID );

	const char* pszStr = NULL;

	if( it != This->m_mapDialogInfo.end() )
	{
		pszStr = it->second.pszString.Begin();
	}

	return pszStr;
}

void NetSrv_Other::SC_ExchangeMoneyTable( int SendID , int Count , int DataSize , const char* Data )
{
	PG_Other_LtoC_ExchangeMoneyTable Send;
	Send.Count = Count;
	Send.DataSize = DataSize;
	memcpy( Send.Data , Data , DataSize );
	SendToCli_ByGUID( SendID , Send.Size() , &Send );
}
void NetSrv_Other::SC_DBStringTable( int SendID , int Count , int DataSize , const char* Data )
{
	PG_Other_LtoC_DBStringTable Send;
	Send.Count = Count;
	Send.DataSize = DataSize;
	memcpy( Send.Data , Data , DataSize );
	SendToCli_ByGUID( SendID , Send.Size() , &Send );
}

void NetSrv_Other::SC_AllCli_DBStringTable	( int Count , int DataSize , const char* Data )
{
	PG_Other_LtoC_DBStringTable Send;
	Send.Count = Count;
	Send.DataSize = DataSize;
	memcpy( Send.Data , Data , DataSize );
	//SendToCli_ByGUID( SendID , Send.Size() , &Send );
	SendToAllCli( Send.Size() , &Send );
}

void NetSrv_Other::SC_ColoringHorseResult( int SendID , ColoringShopResultENUM Result  )
{
	PG_Other_LtoC_ColoringHorseResult Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}
void NetSrv_Other::SC_PlayerEvent		( int SendID , PlayerEventTypeENUM Type )
{
	PG_Other_LtoC_PlayerEvent Send;
	Send.Type = Type;
	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

//////////////////////////////////////////////////////////////////////////
//死亡爆出的裝備
//////////////////////////////////////////////////////////////////////////
void NetSrv_Other::SC_PKDeadDropItem( int SendID , ItemFieldStruct& Item )
{
	PG_Other_LtoC_PKDeadDropItem Send;
	Send.Item = Item;
	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Other::SC_ClientClock( int SendID , int BeginTime , int NowTime , int EndTime , int Type )
{
	PG_Other_LtoC_ClientClock Send;
	Send.BeginTime = BeginTime;
	Send.EndTime = EndTime;
	Send.NowTime = NowTime;
	Send.Type = Type;
	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

//////////////////////////////////////////////////////////////////////////
// 通知帳號幣到期時間
//////////////////////////////////////////////////////////////////////////
void NetSrv_Other::SC_AccountMoneyLockInfo( int SendID , int LockAccountMoney[ Def_AccountMoneyLockTime_Max_Count_ ] , int LockAccount_Forever )
{
	PG_Other_LtoC_AccountMoneyLockInfo Send;
	Send.LockAccount_Forever = LockAccount_Forever;
	memcpy( Send.LockAccountMoney , LockAccountMoney , sizeof( Send.LockAccountMoney ) );
	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );
}

void NetSrv_Other::SC_AccountMoneyTranError( int SendID )
{
	PG_Other_LtoC_AccountMoneyTranError Send;
	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );	
}

void NetSrv_Other::SC_SetCycleMagic		( int SendToID  , int MagicID , int MagicPos  )
{
	PG_Other_LtoC_SetCycleMagic Send;
	Send.MagicID = MagicID;
	Send.MagicPos = MagicPos;
	SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );	
}
//////////////////////////////////////////////////////////////////////////
//DB Select 資料
//////////////////////////////////////////////////////////////////////////
void NetSrv_Other::SD_SelectDB			( int PlayerDBID , const char* DataType , const char* SelectCmd )
{
	PG_Other_LtoD_SelectDB Send;
	Send.DataType = DataType;
	Send.SelectCmd = SelectCmd;
	Send.PlayerDBID = PlayerDBID;
	SendToDBCenter( sizeof(Send) , &Send );
}
//////////////////////////////////////////////////////////////////////////
//清除某一個職業
void NetSrv_Other::SC_DelJob				( int SendToID , Voc_ENUM Job )
{	
	PG_Other_LtoC_DelJob Send;
	Send.Job = Job;
	SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );	
}

void NetSrv_Other::SC_SetSmallGameMenuType		( int SendID , int MenuID , SmallGameMenuTypeENUM Type )
{
	PG_Other_LtoC_SetSmallGameMenuType Send;
	Send.MenuID = MenuID;
	Send.Type = Type;
	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );	
}
void NetSrv_Other::SC_SetSmallGameMenuValue		( int SendID , int MenuID , int ID , int Value )
{
	PG_Other_LtoC_SetSmallGameMenuValue Send;
	Send.MenuID = MenuID;
	Send.ID = ID;
	Send.Value = Value;
	SendToCli_ByGUID( SendID , sizeof(Send) , &Send );	
}
void NetSrv_Other::SC_SetSmallGameMenuStr		( int SendID , int MenuID , int ID , const char* Content )
{
	PG_Other_LtoC_SetSmallGameMenuStr Send;
	Send.MenuID = MenuID;
	Send.ID = ID;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1;
	SendToCli_ByGUID( SendID , Send.PGSize() , &Send );	
}

void NetSrv_Other::SC_ResetInstanceResult	( int SendID , bool Result )
{
	PG_Other_LtoC_ResetInstanceResult Send;
	Send.Result = Result;	
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );	
}
void NetSrv_Other::SL_ResetInstance			( int ZoneID , int PartyID , int PlayerDBID )
{
	PG_Other_LtoL_ResetInstance Send;
	Send.PartyID = PartyID;
	Send.PlayerDBID = PlayerDBID;

	SendToLocal( ZoneID , sizeof( Send ) , &Send );
	//SendToAllLocal( sizeof( Send ) , &Send );
}
void NetSrv_Other::SC_ResetInstanceInfo		( int SendDBID , int ZoneID , bool IsReset )
{
	PG_Other_LtoC_ResetInstanceInfo Send;
	Send.IsReset = IsReset;
	Send.ZoneID = ZoneID;
	SendToCli_ByDBID( SendDBID , sizeof( Send ) , &Send );	
}

void NetSrv_Other::SC_PartyDice			( int SendDBID , int Type , int Rand , int PlayerDBID )
{
	PG_Other_LtoC_PartyDice Send;
	Send.Type = Type;
	Send.Rand = Rand;
	Send.PlayerDBID = PlayerDBID;
	SendToCli_ByDBID( SendDBID , sizeof( Send ) , &Send );	
}

void NetSrv_Other::SC_BodyShopResult		( int SendID , bool Result )
{
	PG_Other_LtoC_BodyShopResult Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );	
}

void NetSrv_Other::SC_MoneyVendorReport	( int SendID , int Count )
{
	PG_Other_LtoC_MoneyVendorReport Send;
	Send.Count = Count;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_Other::SC_BotReport			( int SendID , int Count )
{
	PG_Other_LtoC_BotReport Send;
	Send.Count = Count;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_Other::SC_UnlockBotReport		( int SendID , const char* Name , UnlockBotTypeENUM	Type )
{
	PG_Other_LtoC_UnlockBotReport Send;
	Send.Name = Name;
	Send.Type = Type;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_Other::SC_SortBankEnd			( int SendID )
{
	PG_Other_LtoC_SortBankEnd Send;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_Other::SD_LoadClientData			( int PlayerDBID , int KeyID )
{
	PG_Other_LtoD_LoadClientData Send;
	Send.KeyID = KeyID;
	Send.PlayerDBID = PlayerDBID;
	SendToDBCenter( sizeof(Send) , &Send ); 
}
void NetSrv_Other::SC_SaveClientDataResult		( int SendID , int KeyID , bool Result )
{
	PG_Other_LtoC_SaveClientDataResult Send;
	Send.KeyID = KeyID;
	Send.Result = Result;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_Other::SC_SetMapMark			( int SendID , int MarkID , float X , float Y , float Z , const char* TipStr , int IconID )
{
	PG_Other_LtoC_SetMapMark Send;
	Send.Info.IconID = IconID;
	Send.Info.MarkID = MarkID;
	Send.Info.X = X;
	Send.Info.Y = Y;
	Send.Info.Z = Z;
	Send.Info.Tip = TipStr;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_Other::SC_ClsMapMark			( int SendID , int MarkID )
{
	PG_Other_LtoC_ClsMapMark Send;
	Send.MarkID = MarkID;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_Other::SC_AllRoom_SetMapMark	( int RoomID , int MarkID , float X , float Y , float Z , const char* TipStr , int IconID )
{
	PG_Other_LtoC_SetMapMark Send;
	Send.Info.IconID = IconID;
	Send.Info.MarkID = MarkID;
	Send.Info.X = X;
	Send.Info.Y = Y;
	Send.Info.Z = Z;
	Send.Info.Tip = TipStr;
	SendToCli_ByRoomID( RoomID , sizeof( Send ) , &Send );
}
void NetSrv_Other::SC_AllRoom_ClsMapMark	( int RoomID , int MarkID )
{
	PG_Other_LtoC_ClsMapMark Send;
	Send.MarkID = MarkID;
	SendToCli_ByRoomID( RoomID , sizeof( Send ) , &Send );
}
void NetSrv_Other::SC_SetRoomValue		( int SendID , int KeyID , int Value )
{
	PG_Other_LtoC_SetRoomValue Send;
	Send.KeyID = KeyID;
	Send.Value = Value;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_Other::SC_ClsRoomValue		( int SendID , int KeyID )
{
	PG_Other_LtoC_ClsRoomValue Send;
	Send.KeyID = KeyID;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_Other::SC_AllRoom_SetRoomValue	( int RoomID , int KeyID , int Value )
{
	PG_Other_LtoC_SetRoomValue Send;
	Send.KeyID = KeyID;
	Send.Value = Value;
	SendToCli_ByRoomID( RoomID , sizeof( Send ) , &Send );
}
void NetSrv_Other::SC_AllRoom_ClsRoomValue	( int RoomID , int KeyID )
{
	PG_Other_LtoC_ClsRoomValue Send;
	Send.KeyID = KeyID;
	SendToCli_ByRoomID( RoomID , sizeof( Send ) , &Send );
}

//通知Client 新增移除某個法術
void NetSrv_Other::SC_AddExSkill			( int SendID , ExSkillBaseStruct& Info )
{
	PG_Other_LtoC_AddExSkill Send;
	Send.Info = Info;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_Other::SC_DelExSkill			( int SendID , ExSkillBaseStruct& Info )
{
	PG_Other_LtoC_DelExSkill Send;
	Send.Info = Info;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_Other::SC_SpellExSkillResult	( int SendID , bool Result , RoleErrorMsgENUM Msg )
{
	PG_Other_LtoC_SpellExSkillResult Send;
	Send.Result = Result;
	Send.Msg = Msg;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
//////////////////////////////////////////////////////////////////////////
void NetSrv_Other::SC_Range_BaseObjValueChange		( int GItemID , ObjValueChangeENUM Type , const char* ValueStr , int Value  )
{
	RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
	if( Owner == NULL )
		return;

	if( Owner->SecRoomID() == -1 )
		return;

	PG_Other_LtoC_BaseObjValueChange Send;
	Send.GItemID = GItemID;
	Send.Type = Type;
	Send.ValueStr = ValueStr;
	Send.Value = Value;

	SendToCli_Range( Owner , _Def_View_Block_Range_ , sizeof(Send) , &Send );
}
void NetSrv_Other::SC_TransferData_Range	( BaseItemObject* Obj , char Data[20] )
{
	PG_Other_LtoC_TransferData_Range Send;
	Send.GItemID = Obj->Role()->GUID();
	memcpy( Send.Data , Data , sizeof(Send.Data) );
	SendToCli_Range( Obj->Role() , _Def_View_Block_Range_ , sizeof(Send) , &Send );
}
//Lua 通知client 觸發劇情
void NetSrv_Other::SC_LuaClientEvent		( int SendID , int GItemID , int EventType , const char* Content )
{
	PG_Other_LtoC_LuaClientEvent Send;
	Send.GItemID = GItemID;
	Send.EventType = EventType;
	Send.Content = Content;
	Send.Size = Send.Content.Size() + 1;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
//	SendToCli_Range( Obj->Role() , _Def_View_Block_Range_ , sizeof(Send) , &Send );
}

void NetSrv_Other::SC_Range_LuaClientEvent		( int GItemID , int EventType , const char* Content )
{
	RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
	if( Owner == NULL )
		return;

	PG_Other_LtoC_LuaClientEvent Send;
	Send.GItemID = GItemID;
	Send.EventType = EventType;
	Send.Content = Content;
	Send.Size = Send.Content.Size() + 1;
	SendToCli_Range( Owner, _Def_View_Block_Range_ , Send.PGSize() , &Send );
}
void NetSrv_Other::SC_SetRoleValueResult	( int SendID , bool Result )
{
	PG_Other_LtoC_SetRoleValueResult Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

//通知Client zone 開啟
void NetSrv_Other::SC_AllCli_ZoneOpen	( int ZoneId )
{
	PG_Other_LtoC_ZoneOpen Send;
	Send.ZoneID = ZoneId;
	SendToAllCli( sizeof(Send) , &Send );
}
void NetSrv_Other::SC_SetNewTitleSysResult( int SendID , bool Result )
{
	PG_Other_LtoC_SetNewTitleSysResult Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_Other::SC_NewTitleSys_UseItemResult( int SendID , NewTitleSysUseItemResultENUM Result )
{
	PG_Other_LtoC_NewTitleSys_UseItemResult Send;
	Send.Result = Result;
	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
bool NetSrv_Other::SL_PCall( int zoneID , const char* plotStr  )
{
	PG_Other_LtoL_PCall Send;
	Send.PlotCmd = plotStr;
	return SendToLocal( zoneID , sizeof( Send ) , &Send );
}

void NetSrv_Other::SC_OpenSessionResult(int SendID, SessionTypeENUM Type, int SessionID, int CBID)
{
	PG_Other_LtoC_OpenSessionResult Send;
	Send.Type	   = Type;
	Send.SessionID = SessionID;
	Send.CBID	   = CBID;

	SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_Other::SD_OpenSession(const char* Account, int RoleDBID, SessionTypeENUM Type, int SessionID, int ValidTime)
{
	PG_Other_LtoD_OpenSession Send;
	Send.Account	= Account;
	Send.RoleDBID	= RoleDBID;	
	Send.ZoneID		= RoleDataEx::G_ZoneID;
	Send.Type		= Type;	
	Send.SessionID	= SessionID;
	Send.ValidTime	= ValidTime;

	SendToDBCenter( sizeof(Send) , &Send );
}

void NetSrv_Other::SD_CloseSession(const char* Account, int RoleDBID, SessionTypeENUM Type, int SessionID)
{
	PG_Other_LtoD_CloseSession Send;
	Send.Account	= Account;
	Send.RoleDBID	= RoleDBID;
	Send.ZoneID		= RoleDataEx::G_ZoneID;
	Send.Type		= Type;	
	Send.SessionID	= SessionID;

	SendToDBCenter( sizeof(Send) , &Send ); 
}
void NetSrv_Other::SC_FixAccountFlag( RoleDataEx* role )
{
	PG_Other_LtoC_FixAccountFlag send;
	send.AccountFlag = role->Base.AccountFlag;
	SendToCli_ByGUID( role->GUID() , sizeof(send) , &send );
}
//今天的複本
void NetSrv_Other::SC_WeekInstances( int guid , int day , vector<WeekZoneStruct>& zoneGroupIDList )
{
	PG_Other_LtoC_WeekInstances send;
	send.Day = day;
	send.Count = (int)zoneGroupIDList.size();
	for( int i = 0 ; i < send.Count ; i++ )
		send.Info[i] = zoneGroupIDList[i];
	SendToCli_ByGUID( guid , sizeof(send) , &send );
}
//要求結果
void NetSrv_Other::SC_WeekInstancesTeleportResult( int guid , bool result )
{
	PG_Other_LtoC_WeekInstancesTeleportResult send;
	send.Result = result;
	SendToCli_ByGUID( guid , sizeof(send) , &send );
}
//要求重置某副本
void NetSrv_Other::SL_WeekInstancesZoneReset( int zoneID , int dbid , int partyID )
{
	PG_Other_LtoL_WeekInstancesZoneReset send;
	send.DBID = dbid;
	send.PartyID = partyID;
	SendToSrvBySockID( GetZoneNetID(zoneID) , sizeof(send) , &send );
}
//重置副本傳送次數結果
void NetSrv_Other::SC_WeekInstancesReset( int guid , bool result )
{
	PG_Other_LtoC_WeekInstancesReset send;
	send.Result = result;
	SendToCli_ByGUID( guid , sizeof(send) , &send );
}
//複本剩下的生存時間
void NetSrv_Other::SC_WeekInstancesLiveTime( int guid , int sec )
{
	PG_Other_LtoC_WeekInstancesLiveTime send;
	send.Sec = sec;
	SendToCli_ByGUID( guid , sizeof(send) , &send );
}
//開啟鏡世界副本介面
void NetSrv_Other::SC_OpenWeekInstancesFrame( int guid )
{
	PG_Other_LtoC_OpenWeekInstancesFrame send;
	SendToCli_ByGUID( guid , sizeof(send) , &send );
}
//魔靈升階結果
void NetSrv_Other::SC_PhantomRankUpResult( int guid , bool summon , bool result )
{
	PG_Other_LtoC_PhantomRankUpResult send;
	send.Summon = summon;
	send.Result = result;
	SendToCli_ByGUID( guid , sizeof(send) , &send );
}
//魔靈升級結果
void NetSrv_Other::SC_PhantomLevelUpResult( int guid , bool result )
{
	PG_Other_LtoC_PhantomLevelUpResult send;
	send.Result = result;
	SendToCli_ByGUID( guid , sizeof(send) , &send );
}
//魔靈裝備結果
void NetSrv_Other::SC_PhantomEQResult( int guid , bool result )
{
	PG_Other_LtoC_PhantomEQResult send;
	send.Result = result;
	SendToCli_ByGUID( guid , sizeof(send) , &send );
}
//修正魔靈裝備
void NetSrv_Other::SC_FixPhantomEQ( int guid , int eQ[] )
{
	PG_Other_LtoC_FixPhantomEQ send;
	send.EQ[0] = eQ[0];
	send.EQ[1] = eQ[1];
	SendToCli_ByGUID( guid , sizeof(send) , &send );
}
//修正魔靈資料
void NetSrv_Other::SC_FixPhantom( int guid , int phantomID , PhantomBase& info )
{
	PG_Other_LtoC_FixPhantom send;
	send.PhantomID = phantomID;
	send.Info = info;
	SendToCli_ByGUID( guid , sizeof(send) , &send );
}
//開啟幻靈介面
void NetSrv_Other::SC_OpenPhantomFrame( int guid )
{
	PG_Other_LtoC_OpenPhantomFrame send;
	SendToCli_ByGUID( guid , sizeof(send) , &send );
}
//後台魔幻寶盒資料
void NetSrv_Other::SC_ItemCombinInfo( int guid , int groupID , ItemCombineStruct& itemCombine )
{
	PG_Other_LtoC_ItemCombinInfo send;
	send.GroupID = groupID;
	send.ItemCombine = itemCombine;
	SendToCli_ByGUID( guid , sizeof(send) , &send );
}
void NetSrv_Other::SC_AllItemCombinInfo( int guid )
{
	SC_ItemCombinGroupID( guid , RoleDataEx::ItemCombineListExGroupID );
	map< int , map< int , vector< ItemCombineStruct > > >::iterator iter1;
	map< int , vector< ItemCombineStruct > >::iterator iter2;

	for( iter1 = RoleDataEx::ItemCombineListEx.begin() ; iter1 != RoleDataEx::ItemCombineListEx.end() ; iter1++ )
	{
		for( iter2 = iter1->second.begin() ; iter2 != iter1->second.end() ; iter2++ )
		{
			vector< ItemCombineStruct >& list = iter2->second;
			for( unsigned i = 0 ; i < list.size() ; i++ )
				SC_ItemCombinInfo( guid , iter1->first , list[i] );
		}
	}
}
//後台魔幻寶盒索引ID
void NetSrv_Other::SC_ItemCombinGroupID( int guid , int groupID )
{
	PG_Other_LtoC_ItemCombinGroupID send;
	send.GroupID = groupID;
	SendToCli_ByGUID( guid , sizeof(send) , &send );
}
void NetSrv_Other::SC_ALL_ItemCombinGroupID( int groupID )
{
	PG_Other_LtoC_ItemCombinGroupID send;
	send.GroupID = groupID;
	SendToAllCli( sizeof(send) , &send );
}