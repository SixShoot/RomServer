#include "NetSrv_PartExchangeData.h"
#include "../NetWakerGSrvInc.h"
#include "PG/PG_PartExchangeData.h"
//-------------------------------------------------------------------
CNetSrv_PartExchangeData*	CNetSrv_PartExchangeData::This = NULL;
//-------------------------------------------------------------------
bool CNetSrv_PartExchangeData::_Release()
{

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool CNetSrv_PartExchangeData::_Init()
{
    _Net->RegOnSrvPacketFunction( EM_PG_Part_LtoL_NewMirrorRole       , _PG_Part_LtoL_NewMirrorRole         );
    _Net->RegOnSrvPacketFunction( EM_PG_Part_LtoL_ModifyMirrorRole    , _PG_Part_LtoL_ModifyMirrorRole      );
    _Net->RegOnSrvPacketFunction( EM_PG_Part_LtoL_SettingStr          , _PG_Part_LtoL_SettingStr            );
    _Net->RegOnSrvPacketFunction( EM_PG_Part_LtoL_SettingNumber       , _PG_Part_LtoL_SettingNumber         );
	return true;
}
//-----------------------------------------------------------------------------------------------------
void CNetSrv_PartExchangeData::_PG_Part_LtoL_NewMirrorRole      ( int NetID , int Size , void* Data )  
{
    PG_Part_LtoL_NewMirrorRole* PG = (PG_Part_LtoL_NewMirrorRole*)Data;
	PG->Role.NpcInitLink();
    This->OnNewMirrorRole( PG->GItemID , &(PG->Role) );
}
void CNetSrv_PartExchangeData::_PG_Part_LtoL_ModifyMirrorRole   ( int NetID , int Size , void* Data )  
{
    PG_Part_LtoL_ModifyMirrorRole* PG = (PG_Part_LtoL_ModifyMirrorRole*)Data;
    This->OnModifyMirrorRole( PG->GItemID , PG->Offset , PG->DataSize , &(PG->Role) );
}
void CNetSrv_PartExchangeData::_PG_Part_LtoL_SettingStr         ( int NetID , int Size , void* Data )  
{
    PG_Part_LtoL_SettingStr* PG = (PG_Part_LtoL_SettingStr*)Data;
    This->OnSettingStr( PG->GItemID , (RoleValueName_ENUM)PG->ValueID , PG->Str );
}
void CNetSrv_PartExchangeData::_PG_Part_LtoL_SettingNumber      ( int NetID , int Size , void* Data )  
{
    PG_Part_LtoL_SettingNumber* PG = (PG_Part_LtoL_SettingNumber*)Data;
    This->OnSettingNumber( PG->GItemID , (RoleValueName_ENUM)PG->ValueID , PG->Value );
}
//-------------------------------------------------------------------	
bool CNetSrv_PartExchangeData::NewMirrorRole     ( int SendZoneID , int GItemID , RoleData* Role )
{
    PG_Part_LtoL_NewMirrorRole Send;
    Send.GItemID = GItemID;
    Send.Role = *Role;

    return SendToLocal( SendZoneID , sizeof(Send) , &Send );
}
void CNetSrv_PartExchangeData::ModifyMirrorRole  ( int GItemID , int Offset , int Size , void* Data )
{
    PG_Part_LtoL_ModifyMirrorRole Send; 
    Send.GItemID = GItemID;
    Send.Offset = Offset;
    Send.DataSize = Size;
    memcpy( &(Send.Role) , Data , Size );
    CNetSrv_PartBase::DataTransfer( GItemID , Send.Size() , &Send );
}
void CNetSrv_PartExchangeData::SettingStr        ( int GItemID , RoleValueName_ENUM  ValueID , char* Data )
{
    PG_Part_LtoL_SettingStr Send;
    Send.GItemID = GItemID;
    Send.ValueID = ValueID;
    MyStrcpy( Send.Str , Data , sizeof(Send.Str) );
    
    CNetSrv_PartBase::DataTransfer( GItemID , sizeof(Send) , &Send );
}
void CNetSrv_PartExchangeData::SettingNumber     ( int GItemID , RoleValueName_ENUM  ValueID , float Value )
{
    PG_Part_LtoL_SettingNumber Send;
    Send.GItemID = GItemID;
    Send.ValueID = ValueID;
    Send.Value = Value;

    CNetSrv_PartBase::DataTransfer( GItemID , sizeof(Send) , &Send );
}