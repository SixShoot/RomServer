#include "NetPart_PartBaseChild.h"	

//-------------------------------------------------------------------
bool    CNetPart_PartBaseChild::Init()
{
	CNetPart_PartBase::_Init();

	if( This != NULL)
		return false;

	This = new CNetPart_PartBaseChild;


	return true;
}

//--------------------------------------------------------------------------------------
bool    CNetPart_PartBaseChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	CNetPart_PartBase::_Release();
	return true;

}
//--------------------------------------------------------------------------------------
void CNetPart_PartBaseChild::SetLocal          ( int SrvNetID , int ZoneID , char* MapFileName , int MapID )
{
	BaseItemObject::AddSrv( ZoneID , MapFileName , MapID );
/*    BaseItemObject::AddSrv( ZoneID  , ViewMinX , ViewMinY , ViewMaxX-1 , ViewMaxY-1 
                                              , CtrlMinX , CtrlMinY , CtrlMaxX-1 , CtrlMaxY-1 );
*/
}
//--------------------------------------------------------------------------------------
void CNetPart_PartBaseChild::AddObj            ( int GItemID , char* RoleName , int ParallelID , float X , float Y , float Z , float Dir )
{
    GItemData IDInfo;
    IDInfo.GUID = GItemID;
    BaseItemObject* Obj = NEW BaseItemObject( IDInfo.Zone , IDInfo.ID , ParallelID , RoleName , (int)X , (int)Z ) ;

}
//--------------------------------------------------------------------------------------
void CNetPart_PartBaseChild::MoveObj           ( int GItemID  , float X , float Y , float Z , float Dir )
{
    GItemData IDInfo;
    IDInfo.GUID = GItemID;

	int ParallelID = IDInfo.Zone / _DEF_ZONE_BASE_COUNT_;

    BaseItemObject* Obj = BaseItemObject::GetObj( GItemID );
    if( Obj == NULL )
        return;
    Obj->SetPos( (int)X , (int)Z );

    //�q���Ҧ��P�� Srv �����󲣥�
    vector < SrvInfoStruct* >* SrvInfoList = Obj->GetViewSrv( );
    SrvInfoStruct* SrvInfo;
    if( SrvInfoList == NULL )
    {
        Print( Def_PrintLV3 , "MoveObj" , "MoveObj ���o SrvInfoStruct ���� GItemID=%d RoleName=%s" , GItemID , Obj->RoleName() );        
        return;
    }
	//////////////////////////////////////////////////////////////////////////
	if( SrvInfoList->size() == 0 )
	{
		//���ʦb�D����ϰ줺
		SL_NotInControlArea( IDInfo.Zone , GItemID );
	}
    //////////////////////////////////////////////////////////////////////////
    for( int i = 0 ; i < (int)SrvInfoList->size() ; i++  )
    {
        SrvInfo = (*SrvInfoList)[i];

        if( SrvInfo->SrvID == IDInfo.Zone )
            continue;

        MirrorMoveObj( SrvInfo->SrvID + ParallelID * _DEF_ZONE_BASE_COUNT_ , GItemID , X , Y , Z , Dir );
    }
    //---------------------------------------------------------------------

}
//--------------------------------------------------------------------------------------
void CNetPart_PartBaseChild::DelObj            ( int GItemID  )
{
    BaseItemObject* Obj = BaseItemObject::GetObj( GItemID );
    if( Obj != NULL )
        delete Obj;
}
//--------------------------------------------------------------------------------------
void CNetPart_PartBaseChild::DataTransfer      ( int GItemID , int DataSize , void* Data )
{
    int     i;
    GItemData IDInfo;
    IDInfo.GUID = GItemID;

    BaseItemObject* Obj = BaseItemObject::GetObj( GItemID );
    if( Obj == NULL )
        return;

    //�q���Ҧ��P�� Srv �����󲣥�
    vector < SrvInfoStruct* >* SrvInfoList = Obj->GetViewSrv( );
    SrvInfoStruct* SrvInfo;
    if( SrvInfoList == NULL )
    {
        Print( Def_PrintLV3 , "DataTransfer" , "DataTransfer ���o SrvInfoStruct ����"  );        
        return;
    }
    //---------------------------------------------------------------------
    for( i = 0 ; i < (int)SrvInfoList->size() ; i++  )
    {
        SrvInfo = (*SrvInfoList)[i];

        if( SrvInfo->SrvID == IDInfo.Zone )
            continue;

        SendToLocal( IDInfo.Zone , DataSize , Data );
    }

}
//--------------------------------------------------------------------------------------
void CNetPart_PartBaseChild::OnLocalSrvDisconnect(  int ZoneID )
{
    BaseItemObject::DelSrv( ZoneID );
}
//--------------------------------------------------------------------------------------