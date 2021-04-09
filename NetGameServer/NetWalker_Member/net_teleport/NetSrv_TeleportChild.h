#pragma once

#include "NetSrv_Teleport.h"

class NetSrv_TeleportChild : public NetSrv_Teleport
{
public:
    static bool Init();
    static bool Release();

	virtual void R_AddTeleportRequest	( BaseItemObject* Obj , int ItemPos , int TeleportPos , LocationSaveStruct& Info );
	virtual void R_DelTeleportRequest	( BaseItemObject* Obj , int TeleportPos );
	virtual void R_SwapTeleportRequest	( BaseItemObject* Obj , int TeleportPos[2] );
	virtual void R_ModifyTeleportRequest( BaseItemObject* Obj , int TeleportPos , LocationSaveStruct& Info );
	virtual void R_UseTeleportRequest	( BaseItemObject* Obj , UseTeleportTypeENUM Type , int ItemPos , int TeleportPos );

	virtual void RL_CreateTeleport		( int FromZone , float FromX , float FromY , float FromZ , int ToZone , float ToX , float ToY , float ToZ , const char* Name );
};

