#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_Teleport.h"

class NetSrv_Teleport : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Teleport* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------
	static void _PG_Teleport_CtoL_AddTeleportRequest		( int Sockid , int Size , void* Data );	
	static void _PG_Teleport_CtoL_DelTeleportRequest		( int Sockid , int Size , void* Data );
	static void _PG_Teleport_CtoL_SwapTeleportRequest		( int Sockid , int Size , void* Data );
	static void _PG_Teleport_CtoL_ModifyTeleportRequest		( int Sockid , int Size , void* Data );
	static void _PG_Teleport_CtoL_UseTeleportRequest		( int Sockid , int Size , void* Data );
	static void _PG_Teleport_LtoL_CreateTeleport			( int Sockid , int Size , void* Data );
public:    

	static void SC_AddTeleportResult		( int SendID , int TeleportPos , LocationSaveStruct& Info , bool Result );
	static void SC_DelTeleportResult		( int SendID , int TeleportPos , bool Result );
	static void SC_SwapTeleportResult		( int SendID , int TeleportPos[2] , bool Result );
	static void SC_ModifyTeleportResult		( int SendID , int TeleportPos , LocationSaveStruct& Info , bool Result );
	static void SC_UseTeleportResult		( int SendID , UseTeleportTypeENUM Type , int ItemPos , int TeleportPos , bool Result );
	static void SL_CreateTeleport			( int FromZone , float FromX , float FromY , float FromZ , int ToZone , float ToX , float ToY , float ToZ , const char* Name );

	virtual void R_AddTeleportRequest	( BaseItemObject* Obj , int ItemPos , int TeleportPos , LocationSaveStruct& Info ) = 0;
	virtual void R_DelTeleportRequest	( BaseItemObject* Obj , int TeleportPos ) = 0;
	virtual void R_SwapTeleportRequest	( BaseItemObject* Obj , int TeleportPos[2] ) = 0;
	virtual void R_ModifyTeleportRequest( BaseItemObject* Obj , int TeleportPos , LocationSaveStruct& Info ) = 0;
	virtual void R_UseTeleportRequest	( BaseItemObject* Obj , UseTeleportTypeENUM Type , int ItemPos , int TeleportPos ) = 0;

	virtual void RL_CreateTeleport		( int FromZone , float FromX , float FromY , float FromZ , int ToZone , float ToX , float ToY , float ToZ , const char* Name ) = 0;
};

