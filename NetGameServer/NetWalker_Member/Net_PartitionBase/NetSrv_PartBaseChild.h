#pragma once

#include "NetSrv_PartBase.h"

//--------------------------------------------------------------------------------------------
class CNetSrv_PartBaseChild : public CNetSrv_PartBase
{

public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

    //轉送資料 - 只有Master才可以
    static bool DataTransfer_ByMaster   ( int GItemID , int DataSize , void* Data );           

    virtual void OnAddtoOtherSrv    ( int OtherZoneID , int GItemID );
    virtual void OnAdd              ( int GItemID );
    virtual void OnDelFromOtherSrv  ( int OtherZoneID , int GItemID );
    virtual void OnDel              ( int GItemID );
    virtual void OnMirrorObjMove    ( int GItemID , float X , float Y , float Z , float Dir );
    virtual void OnNotifyChangeZone ( int GItemID , int ToZoneID );

    virtual void OnPartitionConnect ( );
    virtual void OnPartitionDisconnect ( );

	virtual void OnNotInControlArea( int GItemID );
//--------------------------------------------------------------------------------------------
};
