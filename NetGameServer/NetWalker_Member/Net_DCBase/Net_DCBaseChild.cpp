#include "Net_DCBaseChild.h"	
#include "../../mainproc/FlagPosList/FlagPosList.h"
#include "../../mainproc/pathmanage/NPCMoveFlagManage.h"
#include "../../mainproc/HousesManage/HousesManageClass.h"

#include "StackWalker/StackWalker.h"
#include "FileVersion/FileVersion.h"
#include "../net_InstanceReset/NetSrv_InstanceChild.h"
#include "../net_other/NetSrv_OtherChild.h"
//-------------------------------------------------------------------
bool	Net_DCBaseChild::_IsLoadData = false;
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

	Net_DCBase::_Release();

	delete This;
	This = NULL;
	
	return true;

}
//--------------------------------------------------------------------------------------
void Net_DCBaseChild::SavePlayerFailed( int DBID )
{
	if( Global::Ini()->IsCheckRoleDataSize == false )
		return;
	//找出此角色並登出
	BaseItemObject* RoleObj = Global::GetPlayerObj_ByDBID( DBID );
	if( RoleObj == NULL )
		return;
	RoleObj->Role()->Msg( "Save Role Failed" );
	RoleObj->Destroy( "SYS SavePlayerFailed" );
}
void Net_DCBaseChild::SaveNPCResultOK( int LocalObjID , int NPCDBID )
{
    Print( LV1 , "SaveNPCResultOK" , "Save npc success DBID =%d" , NPCDBID );
	BaseItemObject* RoleObj = Global::GetObj( LocalObjID );
	if( RoleObj == NULL )
		return;

    RoleDataEx* Role = RoleObj->Role();
    if( Role == NULL )
    {
        Print( LV4 , "SaveNPCResultOK" , "Object Not Find (NPCDBID=%d)" , NPCDBID );
        return;
    }

	if( Role->Base.DBID == -2 )
	{
		Role->Base.DBID = NPCDBID;
		if( Role->IsFlag() )
		{
			FlagPosInfo* FlagData =  FlagPosClass::GetFlag( Role );
			if( FlagData != NULL )
			{
				FlagData->FlagDBID = NPCDBID;
			}
		}
		else
		{
			RoleObj->NPCMoveInfo( NPCDBID );
		}
	}
	//如果為旗子則是要影藏
	else if( Role->Base.DBID == -3 )
	{

		if( Role->IsFlag() )
		{
			FlagPosInfo* FlagData =  FlagPosClass::GetFlag( Role );

			if( FlagData != NULL )
			{
				FlagData->FlagDBID = NPCDBID;
				FlagData->FlagObj = NULL;

				Role->Base.DBID = -1;
				RoleObj->Destroy("SYS SaveNPCResultOK");
			}
		}
		else
		{
			Print( LV4 , "SaveNPCResultOK" , "DBID == -3 , Not Flag?" );
		}
	}
    else if( Role->Base.DBID != NPCDBID )
    {
		//找到的物件不正確
        Print( LV4 , "SaveNPCResultOK" , "Object Not find ,DBID != -2 && DBID != NPCDBID" );
    }
	else 
	{
		return;
	}


	// 初始劇情
		if( Role->SelfData.AutoPlot.Size() != 0 )
		{
			RoleObj->PlotVM()->CallLuaFunc( Role->SelfData.AutoPlot.Begin() , Role->GUID() , 0  );
		}

	// 樣版初始劇情
		GameObjDbStructEx* pDBObj = Global::GetObjDB( Role->BaseData.ItemInfo.OrgObjID );

		if( pDBObj && ( pDBObj->IsNPC() || pDBObj->IsQuestNPC() ) )
		{
			if( strlen( pDBObj->NPC.szLuaInitScript ) != 0 )
			{
				RoleObj->PlotVM()->CallLuaFunc( pDBObj->NPC.szLuaInitScript , RoleObj->GUID() );
			}
		}
     
	//如果物件還沒顯示，把他加入Partition
	if( Role->SecRoomID( ) == -1 )
		Global::AddToPartition( LocalObjID , Role->BaseData.RoomID );
}
void Net_DCBaseChild::SaveNPCResultFailed( int LocalObjID )
{

    Print( LV4 , "SaveNPCResultFailed", "" );
    RoleDataEx* Role = Global::GetRoleDataByGUID( LocalObjID );
    if( Role == NULL )
        return;

    if( Role->Base.DBID == -2 )
        Role->Base.DBID = -1;

	//如果物件還沒顯示，把他加入Partition
	if( Role->SecRoomID( ) != -1 )
		Global::AddToPartition( LocalObjID , Role->BaseData.RoomID );
	
}
void Net_DCBaseChild::DropNPCResultOK( int LocalObjID , int NPCDBID )
{
    Print( LV1 , "DropNPCResultOK",  "DBID =%d" , NPCDBID );
}
void Net_DCBaseChild::DropNPCResultFailed( int LocalObjID , int NPCDBID )
{
     Print( LV4 ,"DropNPCResultFailed" ,  "" );
}
void Net_DCBaseChild::LoadNPCResult( int TotalCount , int ID , NPCData* NPC )
{
    //BaseItemObject* LNPCObj = BaseItemObject::GetObjByDBID( NPC->d)
    BaseItemObject* LNPCObj = Global::SearchNpcDBID( NPC->DBID );
	int ItemID = -1;

    if( LNPCObj != NULL )
    {
        Print( LV1 , "LoadNPCResult" ,  "Repetition Load NPC Data??" );
        return;
    }
	

	/*
	const char* KeyValueName = g_ObjectData->GetString( NPC->RoleName.Begin() );
	if( strlen( KeyValueName ) > 0 )
	{
		NPC->RoleName = KeyValueName;
	}
	*/


	GameObjDbStructEx*	OrgDB = Global::GetObjDB( NPC->OrgObjID );
	if( OrgDB == NULL )
		return;

	if( OrgDB->IsFlagObj() )
	{
		//Global::CreateFlag( NPC->OrgObjID - Def_ObjectClass_Flag , NPC->PID , NPC->Pos.X , NPC->Pos.Y , NPC->Pos.Z , false );
		FlagPosClass::AddFlag( NULL , NPC->OrgObjID, NPC->PID  , NPC->Pos.X , NPC->Pos.Y , NPC->Pos.Z , NPC->Pos.Dir , NPC->DBID );
	}
	else
	{    
		if(		stricmp( OrgDB->Name , NPC->RoleName.Begin() ) != 0 
			||	OrgDB->IsQuestNPC() 
			||	OrgDB->IsMine() )
		{
			ItemID = Global::CreateObj_Macro(  NULL , NPC->OrgObjID , NPC->RoleName.Begin()
				, NPC->Pos.X , NPC->Pos.Y , NPC->Pos.Z , NPC->Pos.Dir , 1
				, NPC->AutoPlot.Begin() , NPC->PlotClassName.Begin() 
				, NPC->QuestID , NPC->Mode._Mode , NPC->PID , NPC->DBID , NPC->vX , NPC->vY , NPC->vZ , NPC->RoomID );
		}
		else if( OrgDB->IsNPC() )
		{
			GameObjDbStructEx*  NewNpcDB = OrgDB;
			for( int i = 0 ; i < 2 ; i++ )
			{
				if( Random( 100000 ) < OrgDB->NPC.RaiseChangeRate[i] )
				{				
					GameObjDbStructEx*  TempDB = Global::GetObjDB( OrgDB->NPC.RaiseChangeNPCID[i] );
					if( TempDB->IsNPC() == true )
					{
						NewNpcDB = TempDB;
						break;
					}
				}
			}
			ItemID = Global::CreateObj_Macro(  NULL , NewNpcDB->GUID , NewNpcDB->Name
				, NPC->Pos.X , NPC->Pos.Y , NPC->Pos.Z , NPC->Pos.Dir , 1
				, NPC->AutoPlot.Begin() , NPC->PlotClassName.Begin() 
				, NPC->QuestID , NPC->Mode._Mode , NPC->PID , NPC->DBID , NPC->vX , NPC->vY , NPC->vZ , NPC->RoomID );
		}
	}

	//------------------------------------------------------------------------------------------------------------
	//位置與區域檢查
	GameObjDbStructEx* ZoneObj = Global::GetObjDB( Def_ObjectClass_Zone + RoleDataEx::G_ZoneID % _DEF_ZONE_BASE_COUNT_ );
	if( ZoneObj != NULL )
	{
		if( ZoneObj->Zone.MapID != PathManageClass::MainMapID(NPC->Pos.X , NPC->Pos.Y , NPC->Pos.Z ) )
		{
			Print( LV5 , "LoadNPCResult" ,  "Control area Error , ItemID = %d Name=%s DBID=%d OrgObjID=%d" , ItemID , Utf8ToChar( NPC->RoleName ).c_str() , NPC->DBID , NPC->OrgObjID );
			//return;
		}

		if( TotalCount == ( ID + 1 ) )
		{
			if( strcmp( ZoneObj->Zone.LuaInitZone, "" ) != 0 )
			{
				// NPC 載完, 執行初始劇情
				char szLuaString[256];
				sprintf( szLuaString, "%s( %d )", ZoneObj->Zone.LuaInitZone, 0 );
				LUA_VMClass::PCallByStrArg( szLuaString, -1, -1, NULL , NULL );
			}

			Global::St()->IsLoadAllNpcComplete = true;
		}
	}



	

}
void Net_DCBaseChild::SetPlayerLiveTimeFailed( int DBID )
{
}
void Net_DCBaseChild::SetPlayerLiveTimeFailed_ByAccount( char* Acount )
{

}
//--------------------------------------------------------------------------------------
//DataCenter送NPC的移動位置資訊(因該發生在還沒有載入NPC時)
void Net_DCBaseChild::NPCMoveInfoResult( int ZoneID , int NPCDBID , int TotalCount , int IndexID , NPC_MoveBaseStruct& Base )
{
	static vector<NPC_MoveBaseStruct> List; 
	static int	LastNPCDBID = -1;

	if( IndexID <= 0 )
	{
		LastNPCDBID = NPCDBID;
	}

	assert( NPCDBID == LastNPCDBID );
	
	if( TotalCount != 0 )
		List.push_back( Base );

	if( TotalCount == List.size() )
	{
		vector< NPC_MoveBaseStruct >* MoveList = NPCMoveFlagManageClass::GetNPCMoveList( NPCDBID );	
		*MoveList = List;
		List.clear();
	}
	

}
//--------------------------------------------------------------------------------------
//檢查是否可以載入NPC
int  Net_DCBaseChild::_CheckLoadNPC( SchedularInfo* Obj , void* InputClass )
{
	if( BaseItemObject::GetMaxWorldGUID( ) == -1 )
	{
		Print( LV1 , "_CheckLoadNPC" , "BaseItemObject::GetMaxWorldGUID( ) == -1" );
		Schedular()->Push( _CheckLoadNPC , 5000 , NULL , NULL );
		return 0;
	}

	SD_ZoneInfoRequest( RoleDataEx::G_ZoneID );
	LoadAllNPCMoveInfo( RoleDataEx::G_ZoneID );
	LoadAllNPC( RoleDataEx::G_ZoneID );
	_IsLoadData = true;
	return 0;
}

void Net_DCBaseChild::OnDataCenterConnect( )
{

	char	Buf[512];
	char	ExeFileName[MAX_PATH];
	//寫入Version資料
	GetModuleFileName( NULL, ExeFileName, MAX_PATH );

	CFileVersion Version;
	if (Version.Open(ExeFileName))
	{
		CString ver = Version.GetFixedFileVersion();
		const char* verStr = ver.GetString();

		sprintf( Buf , "INSERT INTO ServerStartLog( Version,Server,WorldID) VALUES ( '%s' , 'Zone%d',%d )" 
			, verStr , Ini()->ZoneID ,Global::_Net->GetWorldID()	);
		Net_DCBase::SqlCommand_Import( rand() , Buf );
	}

	if( _IsLoadData == true )
		return;

	Schedular()->Push( _CheckLoadNPC , 5000 , NULL , NULL );

}
void Net_DCBaseChild::OnRoleCenterConnect( int playerCenterID )
{
	SD_CheckRoleDataSize( playerCenterID );
}

void Net_DCBaseChild::OnRoleCenterDisconnect( int playerCenterID )
{

}


void Net_DCBaseChild::RD_CheckRoleDataSizeResult( bool Result )
{
	if( Result == false )
	{
		if( Ini()->IsCheckRoleDataSize == true )
		{
			Print( LV5 , "RD_CheckRoleDataSizeResult" , "sizeof( RoleData ) !=  sizeof( DataCenter.RoleData) " );
			//MessageBox( NULL, "sizeof( RoleData ) !=  sizeof( DataCenter.RoleData)", "RD_CheckRoleDataSizeResult", MB_OK );
			Global::Destory();
			return;
		}
	}


}

void Net_DCBaseChild::RD_ZoneInfoResult( LocalServerBaseDBInfoStruct& Info )
{
	
	//Global::St()->LocalDBInfo = Info;
	Global::St()->LocalDBInfo.ItemVersion		= Info.ItemVersion;				//目前物品的序號(0 - 9999999) 只有不可堆疊的物品有序號
	Global::St()->LocalDBInfo.HouseItemMaxDBID	= Info.HouseItemMaxDBID;


//	NetSrv_InstanceChild::SetInstanceLiveTime( TimeExchangeFloatToInt( Info.InstanceResetTime ) );

	//HousesManageClass::SetMaxItemDBID( Info.HouseItemMaxDBID );
}

void Net_DCBaseChild::RD_GlobalZoneInfoResult( GlobalServerDBInfoStruct& Info )
{
	RoleDataEx::GlobalSetting = Info;
	RoleDataEx::GlobalSetting.ExpRate += (Global::Ini()->ZoneValue_EXPBonus / 100.0f);
	RoleDataEx::GlobalSetting.TpRate += (Global::Ini()->ZoneValue_TPBonus / 100.0f);

	if( RoleDataEx::GlobalSetting.ExpRate < 1 )
		RoleDataEx::GlobalSetting.ExpRate = 1;

	if( RoleDataEx::GlobalSetting.TpRate < 1 )
		RoleDataEx::GlobalSetting.TpRate = 1;
//ZoneValue_EXPBouns
//ZoneValue_TPBouns

	/*
	RoleDataEx::GlobalSetting.ExpRate			= Info.ExpRate;
	RoleDataEx::GlobalSetting.DropTreasureRate= Info.DropTreasureRate;
	RoleDataEx::GlobalSetting.TpRate			= Info.TpRate;
	RoleDataEx::GlobalSetting.MoneyRate		= Info.MoneyRate;	
	*/
}

void Net_DCBaseChild::RD_GmCommandRequest( GmCommandStruct&	Info )
{
	BaseItemObject*	Obj =	Global::GetPlayerObj_ByDBID( Info.PlayerDBID );

	if( Obj == NULL )
	{
		//SD_GmCommandResult( Info.GUID , false , "Player DBID Not Find( Player Change Zone )" );
		SD_GmCommandResult( Info.GUID , EM_GmCommandResult_Failed , "Player Not Online ( Player Change Zone or DBID Error or Player not Online )" );
		return;
	}
	bool Ret = _St->GM.GMCmdProc( Obj , Info.Command , Obj->Role()->RoleName() , (ManagementENUM)Info.ManageLv );
	if( Ret == false )
		SD_GmCommandResult( Info.GUID , EM_GmCommandResult_Failed , _St->GM.CmdResultStr() );
	else
		SD_GmCommandResult( Info.GUID , EM_GmCommandResult_OK , _St->GM.CmdResultStr() );
}

/*
void Net_DCBaseChild::RD_SaveRoleDataNotify( const char* Account , int PlayerDBID , int ClientID , int MasterSockID )
{
	//找出此角色
	BaseItemObject* OwnerObj = BaseItemObject::GetObjByDBID( PlayerDBID );
	if( OwnerObj != NULL && OwnerObj->Role()->IsDestroy() == false )
	{
		OwnerObj->Role()->SaveDB_All();
		OwnerObj->Destroy( "RD_SaveRoleDataNotify" );
		SD_SaveRoleDataNotifyResult( Account , PlayerDBID , ClientID , MasterSockID , true );
	}
	else
	{
		SD_SaveRoleDataNotifyResult( Account , PlayerDBID , ClientID , MasterSockID , false );
	}
}
*/

void Net_DCBaseChild::RD_CheckRoleLiveTime( int DBID )
{
	BaseItemObject*	Obj =	Global::GetPlayerObj_ByDBID( DBID );

	if( Obj == NULL )
	{
		Print( LV3 , "RD_CheckRoleLiveTime" , "Owner->IsDestory() == NULL  " );
		return;
	}

	RoleDataEx* Owner = Obj->Role();

	if( Owner->IsPlayer() == false )
		return;

	if( Obj->SockID() == -1 )
	{
		return;
	}

	if( Owner->IsDestroy() == true )
	{
		Print( LV3 , "RD_CheckRoleLiveTime" , "Owner->IsDestory() == true " );
		SetPlayerLiveTime( GetPCenterID(Owner->Account_ID()) , DBID , 60*1000 );
	}
	else
	{
		Print( LV3 , "RD_CheckRoleLiveTime" , "Owner->IsDestory() == false " );
		SetPlayerLiveTime( GetPCenterID(Owner->Account_ID()) , DBID , 30*60*1000 );
	}
}
/*
void Net_DCBaseChild::R_CheckRoleStruct( int Count , CheckKeyAddressStruct Info[1000] )
{
	ReaderClass<PlayerRoleData>* Reader = RoleDataEx::ReaderBase( );
	map<string,ReadInfoStruct>&  ReadInfo = *( Reader->ReadInfo() );
	map<string,ReadInfoStruct>::iterator Iter;

	if( Count != ReadInfo.size() )
	{
		PrintToController(true, "(Zone)R_CheckRoleStruct false Count != ReadInfo.size()  " );
		return;
	}

	int i = 0;
	for( Iter = ReadInfo.begin() ; Iter != ReadInfo.end() ; Iter++ , i++)
	{		
		if( stricmp( Info[i].Key , Iter->first.c_str() ) != 0 )
		{
			PrintToController(true, "(Zone)R_CheckRoleStruct false stricmp( Info[i].Key , Iter->first.c_str() ) != 0 Key=%s " , Info[i].Key  );
			return;
		}
		if( Info[i].Address != Iter->second.Point )
		{
			PrintToController(true, "(Zone)R_CheckRoleStruct false Info[i].Address != Iter->second.Point Key=%s Point=%d-%d" , Info[i].Key , Info[i].Address , Iter->second.Point );
			return;
		}
	}
}
*/
void Net_DCBaseChild::R_GlobalZoneInfoList( int Count , ZoneConfigBaseStruct Info[1000] )
{
	RoleDataEx::GlobalSetting.Init();
	for( int i = 0 ; i < Count ; i++ )
	{
		if( Info[i].ZoneID != -1 &&  Info[i].ZoneID  != RoleDataEx::G_ZoneID % 1000 )
			continue;
		const char*	CmdStr	= Info[i].CmdStr.Begin();
		int		Value	= Info[i].Value;
		const char*	Str		= Info[i].Str.Begin();
		if( stricmp( CmdStr , "ExpRate" ) == 0 )
		{
			RoleDataEx::GlobalSetting.ExpRate = Value / 100.0f;
			if( RoleDataEx::GlobalSetting.ExpRate < 1 )
				RoleDataEx::GlobalSetting.ExpRate = 1;
		}
		else if( stricmp( CmdStr , "DropTreasureRate" ) == 0 )
		{
			RoleDataEx::GlobalSetting.DropTreasureRate = Value/ 100.0f;
			if( RoleDataEx::GlobalSetting.DropTreasureRate < 1 )
				RoleDataEx::GlobalSetting.DropTreasureRate = 1;
		}
		else if( stricmp( CmdStr , "TpRate" ) == 0 )
		{
			RoleDataEx::GlobalSetting.TpRate = Value/ 100.0f;
			if( RoleDataEx::GlobalSetting.TpRate < 1 )
				RoleDataEx::GlobalSetting.TpRate = 1;
		}
		else if( stricmp( CmdStr , "QuestTpRate" ) == 0 )
		{
			RoleDataEx::GlobalSetting.QuestTpRate = Value/ 100.0f;
			if( RoleDataEx::GlobalSetting.QuestTpRate < 1 )
				RoleDataEx::GlobalSetting.QuestTpRate = 1;
		}
		else if( stricmp( CmdStr , "MoneyRate" ) == 0 )
		{
			RoleDataEx::GlobalSetting.MoneyRate = Value/ 100.0f;
		}
		else if( stricmp( CmdStr , "QuestMoneyRate" ) == 0 )
		{
			RoleDataEx::GlobalSetting.QuestMoneyRate = Value/ 100.0f;
		}
		else if( stricmp( CmdStr , "RelationExpRate" ) == 0 )
		{
			RoleDataEx::GlobalSetting.RelationExpRate = Value/ 100.0f;
			if( RoleDataEx::GlobalSetting.RelationExpRate < 1 )
				RoleDataEx::GlobalSetting.RelationExpRate = 1;
		}
		else if( stricmp( CmdStr , "PK_DotRate" ) == 0 )
		{
			RoleDataEx::GlobalSetting.PK_DotRate = Value/ 100.0f;
		}
		else if( stricmp( CmdStr , "PK_DamageRate" ) == 0 )
		{
			RoleDataEx::GlobalSetting.PK_DamageRate = Value/ 100.0f;
		}
		else if( stricmp( CmdStr , "LearnMagicID" ) == 0 )
		{
			RoleDataEx::GlobalSetting.LearnMagicID = Value;
		}
		else if( stricmp( CmdStr , "Version" ) == 0 )
		{
			RoleDataEx::GlobalSetting.Version = (SystemVersionENUM)Value;
		}
		else if( stricmp( CmdStr , "NPC_DamageRate" ) == 0 )
		{
			RoleDataEx::GlobalSetting.NPC_DamageRate = Value/ 100.0f;
		}
		else if( stricmp( CmdStr , "EqRefineRate" ) == 0 )
		{
			RoleDataEx::GlobalSetting.EqRefineRate = Value/ 100.0f;
		}
		else if( stricmp( CmdStr , "GlobalDropTable1" ) == 0 )
		{
			RoleDataEx::GlobalSetting.GlobalDropTable[0].LuaCheck = Str;
			RoleDataEx::GlobalSetting.GlobalDropTable[0].TableID  = Value;
		}
		else if( stricmp( CmdStr , "GlobalDropTable2" ) == 0 )
		{
			RoleDataEx::GlobalSetting.GlobalDropTable[1].LuaCheck = Str;
			RoleDataEx::GlobalSetting.GlobalDropTable[1].TableID  = Value;
		}
		else if( stricmp( CmdStr , "GlobalDropTable3" ) == 0 )
		{
			RoleDataEx::GlobalSetting.GlobalDropTable[2].LuaCheck = Str;
			RoleDataEx::GlobalSetting.GlobalDropTable[2].TableID  = Value;
		}
		else if( stricmp( CmdStr , "GlobalDropTable4" ) == 0 )
		{
			RoleDataEx::GlobalSetting.GlobalDropTable[3].LuaCheck = Str;
			RoleDataEx::GlobalSetting.GlobalDropTable[3].TableID  = Value;
		}
		else if( stricmp( CmdStr , "GlobalDropTable5" ) == 0 )
		{
			RoleDataEx::GlobalSetting.GlobalDropTable[4].LuaCheck = Str;
			RoleDataEx::GlobalSetting.GlobalDropTable[4].TableID  = Value;
		}
		else if( stricmp( CmdStr , "ItemCombinGroupID" ) == 0 )
		{
			if( RoleDataEx::ItemCombineListExGroupID != Value )
			{
				RoleDataEx::ItemCombineListExGroupID = Value;
				NetSrv_Other::SC_ALL_ItemCombinGroupID( Value );
			}
		}

	}
}