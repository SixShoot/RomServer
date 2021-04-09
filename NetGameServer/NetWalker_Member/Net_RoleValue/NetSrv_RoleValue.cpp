#include "../NetWakerGSrvInc.h"
#include "NetSrv_RoleValue.h"
//-------------------------------------------------------------------
NetSrv_RoleValue*    NetSrv_RoleValue::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_RoleValue::_Init()
{

    Srv_NetCliReg( PG_RoleValue_CtoL_RequestFixRoleValue 	);
    Srv_NetCliReg( PG_RoleValue_CtoL_RequestSetPoint     	);
    Srv_NetCliReg( PG_RoleValue_CtoL_SetTitlRequest      	);

	Srv_NetCliReg( PG_RoleValue_CtoL_CloseChangeJobRequest	);
	Srv_NetCliReg( PG_RoleValue_CtoL_ChangeJobRequest		);

	Srv_NetCliReg( PG_RoleValue_CtoL_ClientLogout			);
	Srv_NetCliReg( PG_RoleValue_CtoL_SetPlayerAllZoneState	);
	Srv_NetCliReg( PG_RoleValue_CtoL_SetClientFlag			);

	Srv_NetCliReg( PG_RoleValue_CtoL_CancelClientLogout		);
	Srv_NetCliReg( PG_RoleValue_CtoL_ChangeParallel			);
	Srv_NetCliReg( PG_RoleValue_CtoL_CancelChangeParallel	);
	
	Srv_NetCliReg( PG_RoleValue_CtoL_ClientComputerInfo		);
	Srv_NetCliReg( PG_RoleValue_CtoL_SetClientData			);
	Srv_NetCliReg( PG_RoleValue_CtoL_ClientEnvironmentState		);
    return false;
}
//-------------------------------------------------------------------
bool NetSrv_RoleValue::_Release()
{
    return false;
}
//-------------------------------------------------------------------
void NetSrv_RoleValue::_PG_RoleValue_CtoL_ClientEnvironmentState	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_RoleValue_CtoL_ClientEnvironmentState* PG = (PG_RoleValue_CtoL_ClientEnvironmentState*)Data;
	This->R_ClientEnvironmentState( Obj , PG->State );
}
void NetSrv_RoleValue::_PG_RoleValue_CtoL_SetClientData			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_RoleValue_CtoL_SetClientData* PG = (PG_RoleValue_CtoL_SetClientData*)Data;
	This->R_SetClientData( Obj , PG->ClientData );
}
void NetSrv_RoleValue::_PG_RoleValue_CtoL_ClientComputerInfo		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_RoleValue_CtoL_ClientComputerInfo* PG = (PG_RoleValue_CtoL_ClientComputerInfo*)Data;
	This->R_ClientComputerInfo( Obj , PG->Info );
}

void NetSrv_RoleValue::_PG_RoleValue_CtoL_ChangeParallel			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_RoleValue_CtoL_ChangeParallel* PG = (PG_RoleValue_CtoL_ChangeParallel*)Data;
	This->R_ChangeParallel( Obj , PG->ParallelID );
}
void NetSrv_RoleValue::_PG_RoleValue_CtoL_CancelChangeParallel		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_RoleValue_CtoL_CancelChangeParallel* PG = (PG_RoleValue_CtoL_CancelChangeParallel*)Data;
	This->R_CancelChangeParallel( Obj );
}

void NetSrv_RoleValue::_PG_RoleValue_CtoL_CancelClientLogout	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_RoleValue_CtoL_CancelClientLogout* PG = (PG_RoleValue_CtoL_CancelClientLogout*)Data;
	This->R_CancelClientLogout( Obj );
}
void NetSrv_RoleValue::_PG_RoleValue_CtoL_SetClientFlag			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_RoleValue_CtoL_SetClientFlag* PG = (PG_RoleValue_CtoL_SetClientFlag*)Data;
	This->R_SetClientFlag( Obj , PG->ClientFlag );
}
void NetSrv_RoleValue::_PG_RoleValue_CtoL_SetPlayerAllZoneState			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_RoleValue_CtoL_SetPlayerAllZoneState* PG = (PG_RoleValue_CtoL_SetPlayerAllZoneState*)Data;
	This->R_SetPlayerAllZoneState( Obj , PG->State );

}
void NetSrv_RoleValue::_PG_RoleValue_CtoL_ClientLogout				( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_RoleValue_CtoL_ClientLogout* PG = (PG_RoleValue_CtoL_ClientLogout*)Data;
	This->R_ClientLogout( Obj );

}

void NetSrv_RoleValue::_PG_RoleValue_CtoL_CloseChangeJobRequest			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_RoleValue_CtoL_CloseChangeJobRequest* PG = (PG_RoleValue_CtoL_CloseChangeJobRequest*)Data;
	This->R_CloseChangeJobRequest( Obj );
}
void NetSrv_RoleValue::_PG_RoleValue_CtoL_ChangeJobRequest			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_RoleValue_CtoL_ChangeJobRequest* PG = (PG_RoleValue_CtoL_ChangeJobRequest*)Data;
	This->R_ChangeJobRequest( Obj , PG->Job , PG->Job_Sub );
}

void NetSrv_RoleValue::_PG_RoleValue_CtoL_RequestFixRoleValue      ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;
    PG_RoleValue_CtoL_RequestFixRoleValue* PG = (PG_RoleValue_CtoL_RequestFixRoleValue*)Data;

    This->R_RequestFixRoleValue( Obj );
}
void NetSrv_RoleValue::_PG_RoleValue_CtoL_RequestSetPoint          ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;
    PG_RoleValue_CtoL_RequestSetPoint* PG = (PG_RoleValue_CtoL_RequestSetPoint*)Data;

    This->R_RequestSetPoint( Obj , PG->STR , PG->STA , PG->INT , PG->MND , PG->AGI );
}
void NetSrv_RoleValue::_PG_RoleValue_CtoL_SetTitlRequest           ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;
    PG_RoleValue_CtoL_SetTitlRequest* PG = (PG_RoleValue_CtoL_SetTitlRequest*)Data;

    This->R_SetTitlRequest( Obj , PG->TitleID , PG->IsShowTitle );
}
//-------------------------------------------------------------------
void NetSrv_RoleValue::S_FixAllRoleValue        (  int SendToID , RoleDataEx* Role )
{
    PG_RoleValue_LtoC_FixAllRoleValue Send;
    Send.Ability = *(Role->TempData.Attr.Ability);

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_RoleValue::S_DeltaRoleValue           (  int SendToID , RoleValueName_ENUM	ValueName , double Value )
{
    PG_RoleValue_LtoC_DeltaRoleValue Send;
    Send.ValueName  = ValueName;
    Send.Value      = Value;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_RoleValue::S_FixRoleValue           (  int SendToID , RoleValueName_ENUM	ValueName , double Value )
{
	PG_RoleValue_LtoC_FixRoleValue Send;
	Send.ValueName  = ValueName;
	Send.Value      = Value;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_RoleValue::S_GetTargetRoleValue           (  int SendToID , int iTargetID, RoleValueName_ENUM	ValueName , double Value )
{
	PG_RoleValue_LtoC_GetTargetRoleValue Send;
	
	Send.GUID		= iTargetID;
	Send.ValueName  = ValueName;
	Send.Value      = Value;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_RoleValue::S_ResultSetPoint_Failed         (  int SendToID )
{
    PG_RoleValue_LtoC_ResultSetPoint Send;
    Send.Result = false;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_RoleValue::S_ResultSetPoint_OK         (  int SendToID )
{
    PG_RoleValue_LtoC_ResultSetPoint Send;
    Send.Result = true;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_RoleValue::S_NotifyLvUp             (  int SendToID , int ItemID , int LV )
{
    PG_RoleValue_LtoC_NotifyLvUp Send;
    Send.ItemID = ItemID;
    Send.LV = LV;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_RoleValue::S_SetTitleFlag           (  int SendToID , int TitleID , bool Flag )
{
    PG_RoleValue_LtoC_SetTitleFlag Send;
    Send.TitleID = TitleID;
    Send.Flag    = Flag;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_RoleValue::S_SetFlag                (  int SendToID , int FlagID , bool Flag )
{
    PG_RoleValue_LtoC_SetFlag Send;
    Send.ID     = FlagID;
    Send.Flag   = Flag;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_RoleValue::S_SetTitleResult_OK         (  int SendToID , int TitleID )
{
    PG_RoleValue_LtoC_SetTitleResult Send;
    Send.TitleID = TitleID;
    Send.Result = true;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_RoleValue::S_SetTitleResult_Failed         (  int SendToID )
{
    PG_RoleValue_LtoC_SetTitleResult Send;
    Send.Result = false;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
/*
void NetSrv_RoleValue::S_Title                  (  int SendToID , int ItemID , bool IsShowTitle , int TitleID , const char* TitleStr )
{
    PG_RoleValue_LtoC_Title Send;
    Send.ItemID      = ItemID;
    Send.TitleID     = TitleID;
	Send.IsShowTitle = IsShowTitle;
	Send.TitleStr	 = TitleStr;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
*/
void NetSrv_RoleValue::S_RoleInfoChange         (  int SendToID , RoleDataEx* Role )
{
    PG_RoleValue_LtoC_RoleInfoChange Send;
    Send.Level      = Role->TempData.Attr.Level;
	Send.Level_Sub	= Role->TempData.Attr.Level_Sub;
	Send.Voc		= Role->TempData.Attr.Voc;
	Send.Voc_Sub	= Role->TempData.Attr.Voc_Sub;
	Send.ItemID		= Role->GUID();
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_RoleValue::S_OpenChangeJob         (  int SendToID , int TargetID )
{
	PG_RoleValue_LtoC_OpenChangeJob Send;
	Send.TargetID = TargetID;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_RoleValue::S_ChangeJobResult       (  int SendToID , ChangeJobResult_ENUM Result )
{
	PG_RoleValue_LtoC_ChangeJobResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_RoleValue::S_CloseChangeJob        (  int SendToID )
{
	PG_RoleValue_LtoC_CloseChangeJob Send;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_RoleValue::S_SetPlayerAllZoneState	( int SendToID , int GItemID , PlayerStateStruct& State )
{
	PG_RoleValue_LtoC_SetPlayerAllZoneState Send;
	Send.State._State = State._State;
	Send.GItemID = GItemID;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_RoleValue::S_ClientLogoutResult	( int SendToID , int Second )
{
	PG_RoleValue_LtoC_ClientLogoutResult Send;
	Send.Second = Second;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );	
}
void NetSrv_RoleValue::S_CancelClientLogout	( int SendToID )
{
	PG_RoleValue_LtoC_CancelClientLogout Send;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_RoleValue::S_WaitChangeParallel	( int SendToID , int WaitTime , bool Result )
{
	PG_RoleValue_LtoC_WaitChangeParallel Send;
	Send.Second = WaitTime;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_RoleValue::S_ClientComputerInfoRequest( int SendToID )
{
	PG_RoleValue_LtoC_ClientComputerInfoRequest Send;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_RoleValue::S_ObjMode( int SendToID , int ItemGUID , ObjectModeStruct& Mode )
{
	PG_RoleValue_LtoC_ObjMode Send;
	Send.ItemGUID	= ItemGUID;
	Send.Mode		= Mode;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_RoleValue::S_AllCli_ObjMode		( int ItemGUID , ObjectModeStruct& Mode )
{
	PG_RoleValue_LtoC_ObjMode Send;
	Send.ItemGUID	= ItemGUID;
	Send.Mode		= Mode;
	Global::SendToAllCli( sizeof( Send ) , &Send );
}

void NetSrv_RoleValue::S_ObjRoleInfo( int SendToID , RoleDataEx* fromRole )
{
	PG_RoleValue_LtoC_ObjRoleInfo Send;

	Send.Role.SetLink();
	
	RoleData* toRole = &Send.Role;

	toRole->Base = fromRole->Base;
	toRole->SelfData = fromRole->SelfData;			//LocalServer 專用
	toRole->BaseData = fromRole->BaseData;			//Client 與 Server 會用到
	toRole->TempData = fromRole->TempData;			//ClientServer 處理角色的暫存便數
	toRole->DynamicData.Init();						//動態資料，不會換區
	*(toRole->PlayerSelfData) = *(fromRole->PlayerSelfData);
	*(toRole->PlayerBaseData) = *(fromRole->PlayerBaseData);
	*(toRole->PlayerTempData) = *(fromRole->PlayerTempData);
	
	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );

}
