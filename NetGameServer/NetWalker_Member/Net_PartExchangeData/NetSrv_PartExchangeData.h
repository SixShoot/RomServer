
#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_PartExchangeData.h"


class CNetSrv_PartExchangeData : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetSrv_PartExchangeData*	This;

	static bool	_Init();
	static bool	_Release();

    static void _PG_Part_LtoL_NewMirrorRole      ( int NetID , int Size , void* Data );  
    static void _PG_Part_LtoL_ModifyMirrorRole   ( int NetID , int Size , void* Data );  
    static void _PG_Part_LtoL_SettingStr         ( int NetID , int Size , void* Data );  
    static void _PG_Part_LtoL_SettingNumber      ( int NetID , int Size , void* Data );  
	//-------------------------------------------------------------------	
public:

    static bool NewMirrorRole     ( int SendZoneID , int GItemID , RoleData* Role );
    static void ModifyMirrorRole  ( int GItemID , int Offset , int Size , void* Data );
    static void SettingStr        ( int GItemID , RoleValueName_ENUM ValueID , char* Data );
    static void SettingNumber     ( int GItemID , RoleValueName_ENUM ValueID , float Value );

    virtual void OnNewMirrorRole    ( int GItemID , RoleData* Role ) = 0;
    virtual void OnModifyMirrorRole ( int GItemID , int Offset , int Size , void* Data ) = 0;
    virtual void OnSettingStr       ( int GItemID , RoleValueName_ENUM ValueID , char* Data ) = 0;
    virtual void OnSettingNumber    ( int GItemID , RoleValueName_ENUM ValueID , float Value ) = 0;

//    virtual void OnMirrorMove       ( )
//    virtual void OnMirrorSetPos     ( )
};
