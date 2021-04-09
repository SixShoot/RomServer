#pragma once

#include "NetSrv_PartExchangeData.h"

//--------------------------------------------------------------------------------------------
class CNetSrv_PartExchangeDataChild : public CNetSrv_PartExchangeData
{

public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

    virtual void OnNewMirrorRole    ( int GItemID , RoleData* Role );
    virtual void OnModifyMirrorRole ( int GItemID , int Offset , int Size , void* Data );
    virtual void OnSettingStr       ( int GItemID , RoleValueName_ENUM ValueID , char* Data );
    virtual void OnSettingNumber    ( int GItemID , RoleValueName_ENUM ValueID , float Value );
//--------------------------------------------------------------------------------------------
};
