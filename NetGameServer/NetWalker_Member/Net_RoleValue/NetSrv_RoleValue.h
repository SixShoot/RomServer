#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_RoleValue.h"

class NetSrv_RoleValue : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_RoleValue* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------
    static void _PG_RoleValue_CtoL_RequestFixRoleValue      ( int NetID , int Size , void* Data );
    static void _PG_RoleValue_CtoL_RequestSetPoint          ( int NetID , int Size , void* Data );
    static void _PG_RoleValue_CtoL_SetTitlRequest           ( int NetID , int Size , void* Data );

	static void _PG_RoleValue_CtoL_CloseChangeJobRequest	( int NetID , int Size , void* Data );
	static void _PG_RoleValue_CtoL_ChangeJobRequest			( int NetID , int Size , void* Data );

	static void _PG_RoleValue_CtoL_ClientLogout				( int NetID , int Size , void* Data );

	static void _PG_RoleValue_CtoL_SetPlayerAllZoneState	( int NetID , int Size , void* Data );

	static void _PG_RoleValue_CtoL_SetClientFlag			( int NetID , int Size , void* Data );
	static void _PG_RoleValue_CtoL_CancelClientLogout		( int NetID , int Size , void* Data );
				
	static void _PG_RoleValue_CtoL_ChangeParallel			( int NetID , int Size , void* Data );
	static void _PG_RoleValue_CtoL_CancelChangeParallel		( int NetID , int Size , void* Data );
	static void _PG_RoleValue_CtoL_ClientComputerInfo		( int NetID , int Size , void* Data );
	static void _PG_RoleValue_CtoL_SetClientData			( int NetID , int Size , void* Data );
	static void _PG_RoleValue_CtoL_ClientEnvironmentState	( int NetID , int Size , void* Data );
public:    

    static void S_FixAllRoleValue       ( int SendToID , RoleDataEx* Role );
    static void S_FixRoleValue          ( int SendToID , RoleValueName_ENUM	ValueName , double Value );
	static void S_DeltaRoleValue        ( int SendToID , RoleValueName_ENUM ValueName , double Value );
    static void S_ResultSetPoint_Failed ( int SendToID );
    static void S_ResultSetPoint_OK     ( int SendToID );
    static void S_NotifyLvUp            ( int SendToID , int ItemID , int LV );
    static void S_SetTitleFlag          ( int SendToID , int TitleID , bool Flag );
    static void S_SetFlag               ( int SendToID , int FlagID  , bool Flag );
    static void S_SetTitleResult_OK     ( int SendToID , int TitleID );
    static void S_SetTitleResult_Failed ( int SendToID );
    static void S_RoleInfoChange        ( int SendToID , RoleDataEx* Role );
	static void S_GetTargetRoleValue	 (  int SendToID , int iTargetID, RoleValueName_ENUM	ValueName , double Value );
    
	static void S_OpenChangeJob         ( int SendToID , int TargetID );
	static void S_ChangeJobResult       ( int SendToID , ChangeJobResult_ENUM Result );
	static void S_CloseChangeJob        ( int SendToID );
	static void S_SetPlayerAllZoneState	( int SendToID , int GItemID , PlayerStateStruct& State );

	static void S_ClientLogoutResult	( int SendToID , int Second );
	static void S_CancelClientLogout	( int SendToID );
	static void S_WaitChangeParallel	( int SendToID , int WaitTime , bool Result );
	static void S_ClientComputerInfoRequest( int SendToID );
	static void S_ObjMode				( int SendToID , int ItemGUID , ObjectModeStruct& Mode );
	static void S_AllCli_ObjMode		( int ItemGUID , ObjectModeStruct& Mode );
	static void S_ObjRoleInfo			( int SendToID , RoleDataEx* role );


    virtual void R_RequestFixRoleValue  ( BaseItemObject*	Obj ) = 0;
    virtual void R_RequestSetPoint      ( BaseItemObject*	Obj , int STR , int STA , int INT , int MND , int AGI ) = 0;
    virtual void R_SetTitlRequest       ( BaseItemObject*	Obj , int TitleID , bool IsShowTitle ) = 0;

	virtual void R_CloseChangeJobRequest( BaseItemObject*	Obj  ) = 0;
	virtual void R_ChangeJobRequest		( BaseItemObject*	Obj , int Job , int Job_Sub ) = 0;

	virtual void R_ClientLogout			( BaseItemObject*	Obj ) = 0;
	virtual void R_CancelClientLogout	( BaseItemObject*   Obj ) = 0;
	virtual void R_SetPlayerAllZoneState( BaseItemObject*	Obj , PlayerStateStruct& State ) = 0;

	virtual void R_SetClientFlag		( BaseItemObject*	Obj , StaticFlag<_MAX_CLIENT_FLAG_COUNT_>& ClientFlag ) = 0;
	
	virtual void R_ChangeParallel		( BaseItemObject*	Obj , int ParallelID ) = 0;
	virtual void R_CancelChangeParallel	( BaseItemObject*	Obj ) = 0;

	virtual void R_ClientComputerInfo	( BaseItemObject*	Obj , ClientComputerInfoStruct& Info ) = 0;
	virtual void R_SetClientData		( BaseItemObject*	Obj , char* ClientData ) = 0;
	virtual void R_ClientEnvironmentState( BaseItemObject*	Obj , ClientStateStruct& State ) = 0;
};

