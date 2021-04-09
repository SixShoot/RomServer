
#pragma once

#include "../../MainProc/Global.h"


class CNetPart_PartBase : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetPart_PartBase*	This;

	static bool				_Init();
	static bool				_Release();

    static void _PG_PartBase_LtoP_SetLocal          ( int NetID , int Size , void* Data );    
    static void _PG_PartBase_LtoP_AddObj            ( int NetID , int Size , void* Data );    
    static void _PG_PartBase_LtoP_MoveObj           ( int NetID , int Size , void* Data );    
    static void _PG_PartBase_LtoP_DelObj            ( int NetID , int Size , void* Data );   
    static void _PG_PartBase_LtoP_DataTransfer      ( int NetID , int Size , void* Data );     

    static void	_OnSrvDisconnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	//-------------------------------------------------------------------	
public:
    //-------------------------------------------------------------------	

    //物件所在的GameSrv : 物件加入某一個Srv 
    static void OnAddtoOtherSrv     ( int ZoneID , int OtherZoneID , int GItemID );
    //Mirror的物件的GameSrv: 某物件加入
    static bool OnAdd               ( int ZoneID , int GItemID );
    //物件所在的GameSrv : 物件在某Srv已被移除
    static void OnDelFromOtherSrv   ( int ZoneID , int OtherZoneID , int GItemID );
    //Mirror的物件的GameSrv : 物件移除
    static bool OnDel               ( int ZoneID , int GItemID );
    //命令Mirror的物件的移動
    static void MirrorMoveObj      ( int ZoneID , int GItemID , float X , float Y , float Z , float Dir );    
    //資料換區
    static void ChangeZone         ( int FromZoneID , int ToZoneID , int GItemID );
	//角色一到非控制區內
	static void SL_NotInControlArea( int ZoneID , int GItemID );
    //-------------------------------------------------------------------	
    virtual void SetLocal          ( int SrvNetID , int ZoneID , char* MapFileName , int MapID ) = 0;    
    virtual void AddObj            ( int GItemID , char* RoleName , int ParallelID , float X , float Y , float Z , float Dir ) = 0;    
    virtual void MoveObj           ( int GItemID , float X , float Y , float Z , float Dir ) = 0;    
    virtual void DelObj            ( int GItemID ) = 0;   
    virtual void DataTransfer      ( int GItemID ,int DataSize , void* Data ) = 0;     

    virtual void OnLocalSrvDisconnect   (  int dwServerLocalID ) = 0;
};
