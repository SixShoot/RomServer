
#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_PartitionBase.h"


class CNetSrv_PartBase : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetSrv_PartBase*	This;

	static bool				_Init();
	static bool				_Release();

    static void _PG_PartBase_PtoL_OnAddtoOtherSrv     ( int NetID , int Size , void* Data );     
    static void _PG_PartBase_PtoL_OnAdd               ( int NetID , int Size , void* Data );     
    static void _PG_PartBase_PtoL_OnDelFromOtherSrv   ( int NetID , int Size , void* Data );     
    static void _PG_PartBase_PtoL_OnDel               ( int NetID , int Size , void* Data );    
    static void _PG_PartBase_PtoL_MoveObj             ( int NetID , int Size , void* Data );    
    static void _PG_PartBase_PtoL_ChangeZone          ( int NetID , int Size , void* Data );        
	static void _PG_PartBase_PtoL_NotInControlArea    ( int NetID , int Size , void* Data );        

    static void	_OnSrvConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
    static void	_OnSrvDisconnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	//-------------------------------------------------------------------	
public:
    static void SetLocal       ( int ZoneID , int MapID , const char* MapFileName );               

    static void AddObj         ( int GItemID , char* RoleName , int ParallelID , float X , float Y , float Z , float Dir );          
    static void MoveObj        ( int GItemID , float X , float Y , float Z , float Dir );          
    static void DelObj         ( int GItemID );         
    static void DataTransfer   ( int GItemID , int DataSize , void* Data );           


    virtual void OnAddtoOtherSrv    ( int OtherZoneID , int GItemID ) = 0;
    virtual void OnAdd              ( int GItemID ) = 0;;
    virtual void OnDelFromOtherSrv  ( int OtherZoneID , int GItemID ) = 0;;
    virtual void OnDel              ( int GItemID ) = 0;
    virtual void OnMirrorObjMove    ( int GItemID , float X , float Y , float Z , float Dir ) = 0;
    virtual void OnNotifyChangeZone ( int GItemID , int ToZoneID ) = 0;

    virtual void OnPartitionConnect ( ) = 0;;
    virtual void OnPartitionDisconnect ( ) = 0;;
	virtual void OnNotInControlArea( int GItemID ) = 0;
};
