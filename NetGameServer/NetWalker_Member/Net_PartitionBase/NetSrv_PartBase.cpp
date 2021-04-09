#include "../Net_ServerList/Net_ServerList_Child.h"
#include "PG/PG_PartitionBase.h"
#include "NetSrv_PartBase.h"

//-------------------------------------------------------------------
CNetSrv_PartBase*	CNetSrv_PartBase::This = NULL;
//-------------------------------------------------------------------
bool CNetSrv_PartBase::_Release()
{

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool CNetSrv_PartBase::_Init()
{

    _Net->RegOnSrvPacketFunction( EM_PG_PartBase_PtoL_OnAddtoOtherSrv       , _PG_PartBase_PtoL_OnAddtoOtherSrv       );
    _Net->RegOnSrvPacketFunction( EM_PG_PartBase_PtoL_OnAdd                 , _PG_PartBase_PtoL_OnAdd                 );
    _Net->RegOnSrvPacketFunction( EM_PG_PartBase_PtoL_OnDelFromOtherSrv     , _PG_PartBase_PtoL_OnDelFromOtherSrv     );
    _Net->RegOnSrvPacketFunction( EM_PG_PartBase_PtoL_OnDel                 , _PG_PartBase_PtoL_OnDel                 );	
    _Net->RegOnSrvPacketFunction( EM_PG_PartBase_PtoL_MoveObj               , _PG_PartBase_PtoL_MoveObj               );	
    _Net->RegOnSrvPacketFunction( EM_PG_PartBase_PtoL_ChangeZone            , _PG_PartBase_PtoL_ChangeZone            );	
	_Net->RegOnSrvPacketFunction( EM_PG_PartBase_PtoL_NotInControlArea      , _PG_PartBase_PtoL_NotInControlArea      );	

    Global::Net_ServerList->RegServerLoginEvent( EM_SERVER_TYPE_PARTITION , _OnSrvConnectEvent );
    Global::Net_ServerList->RegServerLogoutEvent( EM_SERVER_TYPE_PARTITION , _OnSrvDisconnectEvent );
	return true;
}
//-----------------------------------------------------------------------------------------------------
void CNetSrv_PartBase::_PG_PartBase_PtoL_NotInControlArea    ( int NetID , int Size , void* Data )
{
    PG_PartBase_PtoL_NotInControlArea* PG = (PG_PartBase_PtoL_NotInControlArea*)Data;
	This->OnNotInControlArea( PG->GItemID );
}
//-----------------------------------------------------------------------------------------------------
void CNetSrv_PartBase::_OnSrvConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
    This->OnPartitionConnect();
}
//-----------------------------------------------------------------------------------------------------
void CNetSrv_PartBase::_OnSrvDisconnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
    This->OnPartitionDisconnect();
}
//-----------------------------------------------------------------------------------------------------
void CNetSrv_PartBase::_PG_PartBase_PtoL_OnAddtoOtherSrv     ( int NetID , int Size , void* Data )
{
    PG_PartBase_PtoL_OnAddtoOtherSrv* PG = (PG_PartBase_PtoL_OnAddtoOtherSrv*)Data;
    This->OnAddtoOtherSrv( PG->OtherZoneID , PG->GItemID );

}
void CNetSrv_PartBase::_PG_PartBase_PtoL_OnAdd               ( int NetID , int Size , void* Data )     
{
    PG_PartBase_PtoL_OnAdd* PG = (PG_PartBase_PtoL_OnAdd*)Data;
    This->OnAdd( PG->GItemID );
}
void CNetSrv_PartBase::_PG_PartBase_PtoL_OnDelFromOtherSrv   ( int NetID , int Size , void* Data )     
{
    PG_PartBase_PtoL_OnDelFromOtherSrv* PG = (PG_PartBase_PtoL_OnDelFromOtherSrv*)Data;
    This->OnDelFromOtherSrv( PG->OtherZoneID , PG->GItemID );
}
void CNetSrv_PartBase::_PG_PartBase_PtoL_OnDel               ( int NetID , int Size , void* Data )    
{
    PG_PartBase_PtoL_OnDel* PG = (PG_PartBase_PtoL_OnDel*)Data;
    This->OnDel( PG->GItemID );
}
void CNetSrv_PartBase::_PG_PartBase_PtoL_MoveObj               ( int NetID , int Size , void* Data )    
{
    PG_PartBase_PtoL_MoveObj* PG = (PG_PartBase_PtoL_MoveObj*)Data;
    This->OnMirrorObjMove( PG->GItemID , PG->PosX , PG->PosY , PG->PosZ , PG->Dir );
}
void CNetSrv_PartBase::_PG_PartBase_PtoL_ChangeZone          ( int NetID , int Size , void* Data )
{
    PG_PartBase_PtoL_ChangeZone* PG = (PG_PartBase_PtoL_ChangeZone*)Data;
    This->OnNotifyChangeZone( PG->GItemID , PG->ToZoneID );
}
//-----------------------------------------------------------------------------------------------------
void CNetSrv_PartBase::SetLocal       ( int ZoneID , int MapID , const char* MapFileName  )
{
    if( Ini()->IsPrivateZone != false )
        return;

    PG_PartBase_LtoP_SetLocal Send;
    Send.ZoneID = ZoneID;
	Send.MapID = MapID;
	strcpy( Send.MapFileName , MapFileName );

    SendToPartition( sizeof(Send) , &Send );
}


void CNetSrv_PartBase::AddObj         (  int GItemID ,  char* RoleName , int ParallelID , float X , float Y , float Z , float Dir  )
{
    if( Ini()->IsPrivateZone != false )
        return;

    GItemData   IDInfo;
    IDInfo.GUID = GItemID;
    if( IDInfo.Zone != RoleDataEx::G_ZoneID )
        return;

    PG_PartBase_LtoP_AddObj Send;
    Send.GItemID = GItemID;
    Send.PosX = X;
    Send.PosY = Y;
    Send.PosZ = Z;
    Send.Dir  = Dir;
	Send.ParallelID = ParallelID;
	MyStrcpy( Send.RoleName , RoleName , sizeof( Send.RoleName ) );

    SendToPartition( sizeof(Send) , &Send );
}
void CNetSrv_PartBase::MoveObj        ( int GItemID , float X , float Y , float Z , float Dir  )
{
    if( Ini()->IsPrivateZone != false )
        return;

    GItemData   IDInfo;
    IDInfo.GUID = GItemID;
    if( IDInfo.Zone != RoleDataEx::G_ZoneID )
        return;

    RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
    
    //if( Role == NULL || Role->TempData.Sys.MirrorCount <= 0 )
    //    return;
    //assert( Role->TempData.Sys.MirrorCount >0 );

    PG_PartBase_LtoP_MoveObj Send;
    Send.GItemID = GItemID;
    Send.PosX = X;
    Send.PosY = Y;
    Send.PosZ = Z;
    Send.Dir  = Dir;

    SendToPartition( sizeof(Send) , &Send );
}
void CNetSrv_PartBase::DelObj         ( int GItemID )
{
    if( Ini()->IsPrivateZone != false )
        return;

    GItemData   IDInfo;
    IDInfo.GUID = GItemID;
    if( IDInfo.Zone != RoleDataEx::G_ZoneID )
        return;

    PG_PartBase_LtoP_DelObj Send;
    Send.GItemID = GItemID;

    SendToPartition( sizeof(Send) , &Send );
}
void CNetSrv_PartBase::DataTransfer   ( int GItemID , int DataSize , void* Data )
{
    if( Ini()->IsPrivateZone != false )
        return;

    PG_PartBase_LtoP_DataTransfer Send;
    Send.GItemID = GItemID;
    if( DataSize > sizeof(Send.Data) )
        Send.DataSize = sizeof(Send.Data);
    else
        Send.DataSize = DataSize;
    
    memcpy( Send.Data , Data , Send.DataSize );

    SendToPartition( Send.Size() , &Send );
}
