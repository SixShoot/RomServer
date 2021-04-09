#include "Net_DCBaseChild.h"	
#include "StackWalker/StackWalker.h"
#include "FileVersion/FileVersion.h"
bool	Net_DCBaseChild::_IsStart = true;

//-------------------------------------------------------------------
bool    Net_DCBaseChild::Init()
{
	Net_DCBase::_Init();

	if( This != NULL)
		return false;

	This = NEW( Net_DCBaseChild );

	return true;
}

//--------------------------------------------------------------------------------------
bool    Net_DCBaseChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	Net_DCBase::_Release();
	return true;

}
//--------------------------------------------------------------------------------------
void Net_DCBaseChild::SavePlayerFailed( int DBID )
{

}
void Net_DCBaseChild::SaveNPCResultOK( int LocalObjID , int NPCDBID )
{

}
void Net_DCBaseChild::SaveNPCResultFailed( int LocalObjID )
{

 
}
void Net_DCBaseChild::DropNPCResultOK( int LocalObjID , int NPCDBID )
{

}
void Net_DCBaseChild::DropNPCResultFailed( int LocalObjID , int NPCDBID )
{
     Print( LV4 , "DropNPCResultFailed" , "刪除儲存 NPC 失敗" );
}
void Net_DCBaseChild::LoadNPCResult( int TotalCount , int ID , NPCData* NPC )
{
}
void Net_DCBaseChild::SetPlayerLiveTimeFailed( int DBID )
{
}
void Net_DCBaseChild::SetPlayerLiveTimeFailed_ByAccount( char* Acount )
{

}
void Net_DCBaseChild::NPCMoveInfoResult( int ZoneID , int NPCDBID , int TotalCount , int IndexID , NPC_MoveBaseStruct& Base )
{
}
void Net_DCBaseChild::RD_CheckRoleDataSizeResult( bool Result )
{
	if( Result == false )
	{
		Print( LV5 , "RD_CheckRoleDataSizeResult" , "sizeof( RoleData ) 與 DataCenter的不同" );
		Global::Destory();
		return;
	}
}
void Net_DCBaseChild::OnDataCenterConnect( )
{
/*	if( _IsStart == false )
		return;
	_IsStart = false;*/

	char	Buf[512];
	char	ExeFileName[MAX_PATH];
	//寫入Version資料
	GetModuleFileName( NULL, ExeFileName, MAX_PATH );

	CFileVersion Version;
	if (Version.Open(ExeFileName))
	{
		CString ver = Version.GetFixedFileVersion();
		const char* verStr = ver.GetString();
		sprintf( Buf , "INSERT INTO ServerStartLog( Version,Server,WorldID) VALUES ( '%s' , 'MASTER',%d )" , verStr,Global::_Net->GetWorldID() );
		Net_DCBase::SqlCommand_Import( rand() , Buf );
	}

}
void Net_DCBaseChild::RD_ZoneInfoResult( LocalServerBaseDBInfoStruct& Info )
{
}
void Net_DCBaseChild::RD_GlobalZoneInfoResult( GlobalServerDBInfoStruct& Info )
{

}
void Net_DCBaseChild::RD_GmCommandRequest( GmCommandStruct&	Info )
{

}
//--------------------------------------------------------------------------------------
void Net_DCBaseChild::OnRoleCenterConnect( int playerCenterID )
{
	SD_CheckRoleDataSize( playerCenterID );
}
void Net_DCBaseChild::OnRoleCenterDisconnect( int playerCenterID )
{

}


/*
void Net_DCBaseChild::R_CheckRoleStruct( int Count , CheckKeyAddressStruct Info[1000] )
{
	ReaderClass<PlayerRoleData>* Reader = RoleDataEx::ReaderBase( );
	map<string,ReadInfoStruct>&  ReadInfo = *( Reader->ReadInfo() );
	map<string,ReadInfoStruct>::iterator Iter;

	if( Count != ReadInfo.size() )
	{
		PrintToController(true, "(Master)R_CheckRoleStruct false Count != ReadInfo.size()  " );
		return;
	}

	int i = 0;
	for( Iter = ReadInfo.begin() ; Iter != ReadInfo.end() ; Iter++ , i++)
	{		
		if( stricmp( Info[i].Key , Iter->first.c_str() ) != 0 )
		{
			PrintToController(true, "(Master)R_CheckRoleStruct false stricmp( Info[i].Key , Iter->first.c_str() ) != 0" );
			return;
		}
		if( Info[i].Address != Iter->second.Point )
		{
			PrintToController(true, "(Master)R_CheckRoleStruct false Info[i].Address != Iter->second.Point" );
			return;
		}
	}


}
*/