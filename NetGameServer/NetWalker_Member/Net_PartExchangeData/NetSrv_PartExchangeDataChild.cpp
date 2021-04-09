//#include "NetSrv_PartExchangeDataChild.h"	
#include "../../NetWalker_Member/NetWakerGSrvInc.h"

//-------------------------------------------------------------------
bool    CNetSrv_PartExchangeDataChild::Init()
{
	CNetSrv_PartExchangeData::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetSrv_PartExchangeDataChild );


	return true;
}

//--------------------------------------------------------------------------------------
bool    CNetSrv_PartExchangeDataChild::Release()
{
	if( This == NULL )
		return false;

	CNetSrv_PartExchangeData::_Release();

	delete This;
	This = NULL;
	
	return true;

}
//--------------------------------------------------------------------------------------
void CNetSrv_PartExchangeDataChild::OnNewMirrorRole    ( int GItemID , RoleData* Role )
{
    Print( Def_PrintLV1 , "OnNewMirrorRole" , "GItemID = %x" , GItemID );
    //----------------------------------------------------------------
	//檢查此物件是否已存在
	if( BaseItemObject::GetObj( GItemID) != NULL )
	{
		Print(Def_PrintLV2 , "OnNewMirrorRole" , "Object Exist!!!(%x)", GItemID);
		return;
	}
	//----------------------------------------------------------------
	//如果 WorldGUID的物件已存在先刪除
	BaseItemObject* LObj = BaseItemObject::GetObjByWorldGUID( Role->TempData.Sys.WorldGUID );
	if( LObj != NULL )
	{
		DelFromPartition( LObj->GUID() );
		//LObj->ReleaseSock();
		delete LObj;
	}

	if( Role->Base.Type  != EM_ObjectClass_Player )
	{
		return;
	}

	//----------------------------------------------------------------
    BaseItemObject*		Obj		= NEW BaseItemObject( GItemID , Role->TempData.Sys.WorldGUID );
    RoleDataEx*			Owner	= Obj->Role();

	if( Owner == NULL )
		return;

    //memcpy( Owner , Role , sizeof(RoleDataEx) );	
	Owner->Copy( Role );
	
    //----------------------------------------------------------------
    //暫時
//Owner->TempData.Attr.Fin.MoveSpeed = 2;
//Owner->Base.Type = EM_ObjectType_Player;
//Owner->BaseData.RoomID = 0;
    //----------------------------------------------------------------
    Owner->ChangeZoneInit( );
	Owner->SelfData.LiveTime = -1;
	Owner->BaseData.Mode.Fight = false;
    //----------------------------------------------------------------
    NetSrv_MoveChild::AddToPartition( Obj );
    //----------------------------------------------------------------

}
void CNetSrv_PartExchangeDataChild::OnModifyMirrorRole ( int GItemID , int Offset , int Size , void* Data )
{

}
void CNetSrv_PartExchangeDataChild::OnSettingStr       ( int GItemID , RoleValueName_ENUM ValueID , char* Data )
{

}
void CNetSrv_PartExchangeDataChild::OnSettingNumber    ( int GItemID , RoleValueName_ENUM ValueID , float Value )
{
    BaseItemObject* OwnerObj = BaseItemObject::GetObj( GItemID );
    if( OwnerObj == NULL )
        return;
    RoleDataEx* Owner = OwnerObj->Role();
    Owner->SetValue( ValueID , (int)Value , NULL );
}
//--------------------------------------------------------------------------------------