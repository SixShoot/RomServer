#include "NetDC_DCBaseChild.h"	
#include "ReaderClass/CreateDBCmdClass.h"
#include "ReaderClass/DbSqlExecClass.h"

#include "StackWalker/StackWalker.h"
#include "FileVersion/FileVersion.h"
bool	CNetDC_DCBaseChild::_IsStart = true;
//-------------------------------------------------------------------
bool    CNetDC_DCBaseChild::Init()
{
	CNetDC_DCBase::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetDC_DCBaseChild );

	return true;
}

//--------------------------------------------------------------------------------------
bool    CNetDC_DCBaseChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	CNetDC_DCBase::_Release();
	return true;

}
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
void CNetDC_DCBaseChild::SavePlayer         ( int SrvNetID , int SaveTime , int DBID , int Start , int Size , void* Data )
{
	Print( Def_PrintLV1 , "SavePlayer" , "要求儲存資料 DBID = %d Start = %d Size = %d" , DBID , Start , Size );
    BaseItemObject* Obj = BaseItemObject::GetObj_DBID( DBID );
    if( Obj == NULL )
	{
		Print( Def_PrintLV4 , "SavePlayer" , "要求除存資料角色已經被登出 DBID = %d Start = %d Size = %d" , DBID , Start , Size );
        //SavePlayerFailed( SrvNetID , DBID );
		
		if( Start == 0 && Size == sizeof(PlayerRoleData) )
		{
			BaseItemObject* Obj = NEW BaseItemObject( (PlayerRoleData*)Data );

			Obj->ClearRoleDB( );

		}
        return;
    }

    if( Start < 0 || Start + Size > sizeof( PlayerRoleData ) )
    {
        Print( Def_PrintLV4 , "SavePlayer" , "要求儲存資料範圍有問題 DBID = %d Start = %d Size = %d" , DBID , Start , Size );
        SavePlayerFailed( SrvNetID , DBID );
        return;
    }
		
	if( Size == 0 )
	{
		return;
	}


	if( Start == 0 && Size == sizeof(PlayerRoleData) )
	{
		PlayerRoleData* Role = Obj->Role();
		PlayerRoleData* NewRoleData = (PlayerRoleData*)Data;
		NewRoleData->SetLink();

		if(	NewRoleData->Base.DBID != DBID )
		{
			Print( LV4 , "SavePlayer" , "要求儲存資料有問題 NewRoleData->DBID()(%d) != DBID (%d)" , NewRoleData->Base.DBID , DBID );
			return;
		}
		//memcpy( (char*)Role , ((char*)Data) + Start  , Size );
		//Role->SetLink();
		*Role = *NewRoleData;

		Role->BaseData.ZoneID = Role->BaseData.ZoneID % _DEF_ZONE_BASE_COUNT_;
		//生存期加十分鐘
		//Obj->SetLiveTime( 60*10*1000 );
		Obj->IsNeedSave( true );

		SaveTime /= 5000;
		if( Obj->SaveTime() > SaveTime )
			Obj->SaveTime( SaveTime );
	}
	else
	{
		Print( Def_PrintLV4 , "SavePlayer" , "目前只存完整資料,要求儲存資料範圍有問題 DBID = %d Start = %d Size = %d" , DBID , Start , Size );
	}
}
void CNetDC_DCBaseChild::SetPlayerLiveTime  ( int SrvNetID , int DBID , int LiveTime )
{
    BaseItemObject* Obj = BaseItemObject::GetObj_DBID( DBID );
    if( Obj == NULL )
    {
		Print( LV3 , "SetPlayerLiveTime" , "SetPlayerLiveTime　DBID(%d)　找不到" , DBID  );
        SetPlayerLiveTimeFailed( SrvNetID , DBID );
        return ;
    }

	Print( LV1 , "SetPlayerLiveTime" , "DBID(%d)　LiveTime(%d)" , DBID , LiveTime  );

    Obj->SetLiveTime( LiveTime );
    
}

void CNetDC_DCBaseChild::SetPlayerLiveTime_ByAccount  ( int SrvNetID , char* Account , int LiveTime )
{
	Print( LV1 , "SetPlayerLiveTime" , "Account(%s)　LiveTime(%d)" , Account , LiveTime  );

    vector< BaseItemObject*>* ObjList = BaseItemObject::GetObj_Account( Account );
    if( ObjList == NULL || ObjList->size() == 0 )
    {
		if( LiveTime == 0 )
		{
			UpdateAccountLogout( Account );
		}

        SetPlayerLiveTimeFailed_ByAccount( SrvNetID , Account );
        return ;
    }
    for( int i = 0 ; i < (int)ObjList->size() ; i++ )
    {
        (*ObjList)[i]->SetLiveTime( LiveTime );
    }

}

void CNetDC_DCBaseChild::R_CheckRoleDataSize		( int SrvNetID , int Size )
{
	if( Size != sizeof(PlayerRoleData) )
	{
		S_CheckRoleDataSizeResult( SrvNetID , false );
		Print( LV3 , "R_CheckRoleDataSize" , "SrvNetID=%d sizeof(RoleData)=%d  DataCenter sizeof(RoleData) = %d" , SrvNetID , Size , sizeof(RoleData) );
	}
	else
		S_CheckRoleDataSizeResult( SrvNetID , true );

}
//////////////////////////////////////////////////////////////////////////
void CNetDC_DCBaseChild::OnDataCenterConnect( )
{
	/*
	if( _IsStart == false )
		return;
	_IsStart = false;
	*/

	char	Buf[512];
	char	ExeFileName[MAX_PATH];
	//寫入Version資料
	GetModuleFileName( NULL, ExeFileName, MAX_PATH );

	CFileVersion Version;
	if (Version.Open(ExeFileName))
	{
		CString ver = Version.GetFixedFileVersion();
		const char* verStr = ver.GetString();
		sprintf( Buf , "INSERT INTO ServerStartLog( Version,Server,WorldID) VALUES ( '%s' , 'PLAYERCENTER',%d)" , verStr,Global::_Net->GetWorldID() );
		CNetDC_DCBase::ImportSqlCommand( Buf );
	}

};

void CNetDC_DCBaseChild::OnServersConnect( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
		S_CheckRoleStruct( ServerType, dwServerLocalID );
}

