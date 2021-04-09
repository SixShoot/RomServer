#include "PG/PG_PartitionBase.h"
#include "NetPart_PartBase.h"
#include "../NetWakerPartInc.h"
//-------------------------------------------------------------------
CNetPart_PartBase*	CNetPart_PartBase::This = NULL;
//-------------------------------------------------------------------
bool CNetPart_PartBase::_Release()
{

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool CNetPart_PartBase::_Init()
{
    _Net->RegOnSrvPacketFunction( EM_PG_PartBase_LtoP_SetLocal       , _PG_PartBase_LtoP_SetLocal       );
    _Net->RegOnSrvPacketFunction( EM_PG_PartBase_LtoP_AddObj         , _PG_PartBase_LtoP_AddObj         );
    _Net->RegOnSrvPacketFunction( EM_PG_PartBase_LtoP_MoveObj        , _PG_PartBase_LtoP_MoveObj        );
    _Net->RegOnSrvPacketFunction( EM_PG_PartBase_LtoP_DelObj         , _PG_PartBase_LtoP_DelObj         );
    _Net->RegOnSrvPacketFunction( EM_PG_PartBase_LtoP_DataTransfer   , _PG_PartBase_LtoP_DataTransfer   );	

    //CNet_ServerList_Child::RegServerLogoutEvent( EM_SERVER_TYPE_LOCAL , _OnSrvDisconnectEvent );
	Global::Net_ServerList->RegServerLogoutEvent( EM_SERVER_TYPE_LOCAL , _OnSrvDisconnectEvent );
	return true;
}
//-----------------------------------------------------------------------------------------------------
void CNetPart_PartBase::_PG_PartBase_LtoP_SetLocal          ( int NetID , int Size , void* Data )
{
    PG_PartBase_LtoP_SetLocal* PG = (PG_PartBase_LtoP_SetLocal*)Data;
    This->SetLocal( NetID , PG->ZoneID , PG->MapFileName , PG->MapID );
}
void CNetPart_PartBase::_PG_PartBase_LtoP_AddObj            ( int NetID , int Size , void* Data )  
{
    PG_PartBase_LtoP_AddObj* PG = (PG_PartBase_LtoP_AddObj*)Data;
    This->AddObj( PG->GItemID , PG->RoleName , PG->ParallelID , PG->PosX , PG->PosY , PG->PosZ , PG->Dir );
}
void CNetPart_PartBase::_PG_PartBase_LtoP_MoveObj           ( int NetID , int Size , void* Data )  
{
    PG_PartBase_LtoP_MoveObj* PG = (PG_PartBase_LtoP_MoveObj*)Data;
    This->MoveObj( PG->GItemID , PG->PosX , PG->PosY , PG->PosZ , PG->Dir );
}
void CNetPart_PartBase::_PG_PartBase_LtoP_DelObj            ( int NetID , int Size , void* Data ) 
{
    PG_PartBase_LtoP_DelObj* PG = (PG_PartBase_LtoP_DelObj*)Data;
    This->DelObj( PG->GItemID );
}
void CNetPart_PartBase::_PG_PartBase_LtoP_DataTransfer      ( int NetID , int Size , void* Data ) 
{
    PG_PartBase_LtoP_DataTransfer* PG = (PG_PartBase_LtoP_DataTransfer*)Data;
    This->DataTransfer( PG->GItemID , PG->DataSize , PG->Data );
}
void CNetPart_PartBase::_OnSrvDisconnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
    This->OnLocalSrvDisconnect( dwServerLocalID );
}
//-----------------------------------------------------------------------------------------------------
//物件所在的GameSrv : 物件加入某一個Srv 
void CNetPart_PartBase::OnAddtoOtherSrv     ( int ZoneID , int OtherZoneID , int GItemID )
{
    PG_PartBase_PtoL_OnAddtoOtherSrv Send;
    Send.GItemID = GItemID;
    Send.OtherZoneID = OtherZoneID;
    SendToLocal( ZoneID , sizeof( Send ) , &Send );
}
//-----------------------------------------------------------------------------------------------------
//Mirror的物件的GameSrv: 某物件加入
bool CNetPart_PartBase::OnAdd               ( int ZoneID , int GItemID )
{
    PG_PartBase_PtoL_OnAdd Send;
    Send.GItemID = GItemID;

    return SendToLocal( ZoneID , sizeof( Send ) , &Send );
}
//-----------------------------------------------------------------------------------------------------
//物件所在的GameSrv : 物件在某Srv已被移除
void CNetPart_PartBase::OnDelFromOtherSrv   ( int ZoneID , int OtherZoneID , int GItemID )
{
    PG_PartBase_PtoL_OnDelFromOtherSrv Send;
    Send.GItemID = GItemID;
    Send.OtherZoneID = OtherZoneID;

    SendToLocal( ZoneID , sizeof( Send ) , &Send );
}
//-----------------------------------------------------------------------------------------------------
//Mirror的物件的GameSrv : 物件移除
bool CNetPart_PartBase::OnDel               ( int ZoneID , int GItemID )
{
    PG_PartBase_PtoL_OnDel Send;
    Send.GItemID = GItemID;

    return SendToLocal( ZoneID , sizeof( Send ) , &Send );
}
//角色一到非控制區內
void CNetPart_PartBase::SL_NotInControlArea( int ZoneID , int GItemID )
{
	PG_PartBase_PtoL_NotInControlArea Send;
	Send.GItemID = GItemID;
	SendToLocal( ZoneID , sizeof( Send ) , &Send );
}
//-----------------------------------------------------------------------------------------------------
//命令Mirror的物件的移動
void CNetPart_PartBase::MirrorMoveObj      ( int ZoneID , int GItemID , float X , float Y , float Z , float Dir )
{
    PG_PartBase_PtoL_MoveObj Send;

    Send.GItemID = GItemID;
    Send.PosX = X;
    Send.PosY = Y;
    Send.PosZ = Z;
    Send.Dir = Dir;

    SendToLocal( ZoneID , sizeof( Send ) , &Send );
}
//-----------------------------------------------------------------------------------------------------
//資料換區
void CNetPart_PartBase::ChangeZone         ( int FromZoneID , int ToZoneID , int GItemID )
{
    PG_PartBase_PtoL_ChangeZone Send;
    Send.ToZoneID = ToZoneID;
    Send.GItemID  = GItemID;

    SendToLocal( FromZoneID , sizeof( Send ) , &Send );
}
//-----------------------------------------------------------------------------------------------------