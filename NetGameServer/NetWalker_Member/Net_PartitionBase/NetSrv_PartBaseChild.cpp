#include "../../NetWalker_Member/NetWakerGSrvInc.h"
#include "NetSrv_PartBaseChild.h"	
//-------------------------------------------------------------------
bool    CNetSrv_PartBaseChild::Init()
{
	CNetSrv_PartBase::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetSrv_PartBaseChild );


	return true;
}

//--------------------------------------------------------------------------------------
bool    CNetSrv_PartBaseChild::Release()
{
	if( This == NULL )
		return false;

	CNetSrv_PartBase::_Release();

	delete This;
	This = NULL;
	
	return true;

}
//--------------------------------------------------------------------------------------
void CNetSrv_PartBaseChild::OnAddtoOtherSrv    ( int OtherZoneID , int GItemID )
{
    Print( Def_PrintLV1 , "OnAddtoOtherSrv" , "OtherZoneID = %d GItemID = %x" , OtherZoneID , GItemID );
    BaseItemObject* Obj = BaseItemObject::GetObj( GItemID );
    if( Obj == NULL )
    {
        Print( Def_PrintLV3 , "OnAddtoOtherSrv" , "GItemID = %d BaseItemObject not finde" , GItemID );
        return;
    }
    CNetSrv_PartExchangeData::NewMirrorRole( OtherZoneID , GItemID , Obj->Role() );
	Obj->Role()->TempData.Sys.MirrorCount ++;
}
//--------------------------------------------------------------------------------------
void CNetSrv_PartBaseChild::OnAdd              ( int GItemID )
{
    //Print( Def_PrintLV1 , "OnAdd  GItemID = %x" , GItemID );
}
//--------------------------------------------------------------------------------------
void CNetSrv_PartBaseChild::OnDelFromOtherSrv  ( int OtherZoneID , int GItemID )
{
    BaseItemObject* Obj = BaseItemObject::GetObj( GItemID );
    if( Obj == NULL )
    {
        Print( Def_PrintLV3 , "OnDelFromOtherSrv" , "GItemID = %x ( object not exist , maybe object already delete or changezone)" , GItemID );
        return;
    }

    Obj->Role()->TempData.Sys.MirrorCount --;
}
//--------------------------------------------------------------------------------------
void CNetSrv_PartBaseChild::OnDel              ( int GItemID )
{
    Print( Def_PrintLV1 , "OnDel" , " GItemID = %x" ,  GItemID );

    BaseItemObject* Obj = BaseItemObject::GetObj( GItemID );
    if( Obj == NULL )
    {
        Print( Def_PrintLV3 , "OnDel" , "GItemID = %x BaseItemObject not find" , GItemID );
        return;
    }
	if( Obj->IsMirror() == false )
	{
		Print( Def_PrintLV3 , "OnDel" , "GItemID = %x Error not Mirror object" , GItemID );
		return;
	}
    Obj->Destroy( "SYS CNetSrv_PartBaseChild::OnDel" );
}
//--------------------------------------------------------------------------------------
void CNetSrv_PartBaseChild::OnMirrorObjMove    ( int GItemID , float X , float Y , float Z , float Dir )
{
    BaseItemObject* OwnerObj = BaseItemObject::GetObj( GItemID );
    if( OwnerObj == NULL )
    {
		//物件資料尚未送到
        return;
    }
    RoleDataEx* Owner = OwnerObj->Role();

    Owner->Pos( X , Y , Z , Dir );
    NetSrv_MoveChild::MovePartition( OwnerObj );
}
//--------------------------------------------------------------------------------------
void CNetSrv_PartBaseChild::OnPartitionDisconnect ( )
{
    BaseItemObject* Obj;
    for(    Obj = BaseItemObject::GetByOrder_MirrorObj(true ) ;
            Obj != NULL ;
            Obj = BaseItemObject::GetByOrder_MirrorObj()  )
	{
		if( Obj->IsMirror() )
            Obj->Destroy( "SYS OnPartitionDisconnect" );
	}
}
//--------------------------------------------------------------------------------------
void CNetSrv_PartBaseChild::OnPartitionConnect ( )
{
	SetLocal( Global::Ini()->ZoneID , Global::Ini()->MapID , Global::Ini()->MapFileName.c_str() );

    int                 i;
    BaseItemObject*     TempObj;
    RoleDataEx*         Role;
    int     MaxID = BaseItemObject::GetmaxZoneID();    
    GItemData   IDInfo;
    IDInfo.Zone = RoleDataEx::G_ZoneID;

    for( i = 0 ; i < MaxID ; i++ )
    {
        IDInfo.ID = i;
        TempObj = BaseItemObject::GetObj( IDInfo.GUID );
        if( TempObj == NULL )
            continue;
        Role = TempObj->Role();
            
        CNetSrv_PartBase::AddObj( Role->GUID() , (char*)Role->RoleName() , Role->PlayerTempData->ParallelZoneID , Role->Pos()->X , Role->Pos()->Y , Role->Pos()->Z , Role->Pos()->Dir  );
    }

}
//--------------------------------------------------------------------------------------
//轉送資料 - 只有Master才可以
bool CNetSrv_PartBaseChild::DataTransfer_ByMaster   ( int GItemID , int DataSize , void* Data )
{
    GItemData IDInfo;
    IDInfo.GUID = GItemID;

    if( IDInfo.Zone != RoleDataEx::G_ZoneID )
        return false;

    DataTransfer( GItemID ,DataSize , Data  );

    return true;
}
//--------------------------------------------------------------------------------------
void CNetSrv_PartBaseChild::OnNotifyChangeZone ( int GItemID , int ToZoneID )
{
    GItemData IDInfo;
    IDInfo.GUID = GItemID;

    if( IDInfo.Zone != RoleDataEx::G_ZoneID )
        return ;

    BaseItemObject* OwnerObj = BaseItemObject::GetObj( GItemID );
    if( OwnerObj == NULL )
    {
        Print( Def_PrintLV2 , "OnNotifyChangeZone" , "GItemID = %d Error , OwnerObj == NULL" , GItemID );
        return;
    }
    RoleDataEx* Owner = OwnerObj->Role();

	if( RoleDataEx::CheckTime( Owner->TempData.Sys.ChangeZoneTime ) == false  )
    {
        //換區速度過快 
        Print( Def_PrintLV2 , "OnNotifyChangeZone" , "GItemID = %d Change zone too often" , GItemID );
        return;
    }

    if( Owner->ZoneID() != RoleDataEx::G_ZoneID )
    {
        Print( Def_PrintLV2 , "OnNotifyChangeZone" , "GItemID = %d  on changezone aleary" , GItemID );
        return;
    }

    if( Owner->IsPlayer() == false )
    {
        return;
    }
    else
    {
//		ToZoneID = ToZoneID % _DEF_ZONE_BASE_COUNT_ + _DEF_ZONE_BASE_COUNT_ * Owner->PlayerTempData->ParallelZoneID;
		if( Global::CheckZoneID( ToZoneID ) != false )
		{
			Owner->ZoneID( ToZoneID );
			Owner->Pos()->X = Owner->TempData.Move.CliX;
			Owner->Pos()->Y = Owner->TempData.Move.CliY;
			Owner->Pos()->Z = Owner->TempData.Move.CliZ;
			Owner->TempData.Attr.Action.ChangeZone = true;
			NetSrv_CliConnectChild::ChangeZone( Owner );
		}
		
    }
}
//--------------------------------------------------------------------------------------
void CNetSrv_PartBaseChild::OnNotInControlArea( int GItemID )
{

	BaseItemObject* OwnerObj = BaseItemObject::GetObj( GItemID );
	if( OwnerObj == NULL )
	{
		Print( Def_PrintLV2 , "OnNotInControlArea" , "GItemID = %d Error , OwnerObj == NULL" , GItemID );
		return;
	}
	RoleDataEx* Owner = OwnerObj->Role();

	int ZoneID = GlobalBase::GetParallelZoneID( Owner->SelfData.RevZoneID ,  Owner->PlayerTempData->ParallelZoneID );
	if( ZoneID == -1 )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_ChanageZoneError_Failed );
		return;
	}

	Owner->Msg( "It's not in control area!" );
	Global::ChangeZone( GItemID , ZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir  );

}