#include "../NetWakerGSrvInc.h"
#include "NetSrv_RoleValueChild.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_RoleValueChild::Init()
{
    NetSrv_RoleValue::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_RoleValueChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_RoleValueChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_RoleValue::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
void NetSrv_RoleValueChild::R_RequestFixRoleValue   ( BaseItemObject*	Obj )
{
    
}
void NetSrv_RoleValueChild::R_RequestSetPoint       ( BaseItemObject*	Obj , int STR , int STA , int INT , int MND , int AGI )
{
    RoleDataEx* Owner = Obj->Role();

    BaseAbilityStruct* Ability = Owner->TempData.Attr.Ability;

	int NeedPoint = STR + STA + INT+ MND+ AGI;

    if( Ability->Point < NeedPoint )
    {
        S_ResultSetPoint_Failed( Owner->GUID() );
    }
    else
    {   
		Owner->AddValue( EM_RoleValue_STR , STR );
		Owner->AddValue( EM_RoleValue_STA , STA );
		Owner->AddValue( EM_RoleValue_INT , INT );
		Owner->AddValue( EM_RoleValue_MND , MND );
		Owner->AddValue( EM_RoleValue_AGI , AGI );
		Owner->AddValue( EM_RoleValue_Point , NeedPoint*-1 );
		/*
        switch( ValueName )
        {
        case EM_RoleValue_STR:
        case EM_RoleValue_STA:
        case EM_RoleValue_INT:
        case EM_RoleValue_MND:
        case EM_RoleValue_AGI:

            Owner->AddValue( ValueName , Point );
            Owner->AddValue( EM_RoleValue_Point , NeedPoint*-1 );
            S_ResultSetPoint_OK( Owner->GUID() );
            break;
        default:
            S_ResultSetPoint_Failed( Owner->GUID() );
            break;
        }
		*/
		S_ResultSetPoint_OK( Owner->GUID() );
    }
}
void NetSrv_RoleValueChild::R_SetTitlRequest        ( BaseItemObject*	Obj , int TitleID , bool IsShowTitle )
{
	
	RoleDataEx* Owner = Obj->Role();

	if( TitleID > 0 )
	{
		if( Owner->CheckWearTitle( TitleID) == false )
		{
			S_SetTitleResult_Failed( Owner->GUID() );
			return;
		}		
	}
	else if( TitleID < -1 )
	{
		TitleID = 0;
	}

	S_SetTitleResult_OK( Owner->GUID() , TitleID );

	//////////////////////////////////////////////////////////////////////////
	//穿脫頭銜觸發
	//////////////////////////////////////////////////////////////////////////	
	{
		char	luaEvent[128];
		GameObjDbStructEx* oldTitleObjDB = Global::GetObjDB( Owner->BaseData.TitleID );
		if( oldTitleObjDB != NULL && strlen(oldTitleObjDB->Title.LuaEvent) != 0)
		{
			sprintf( luaEvent , "%s(0)",oldTitleObjDB->Title.LuaEvent );
			LUA_VMClass::PCallByStrArg( luaEvent , Owner->GUID() , Owner->GUID()  );
		}
	}
	
	Owner->BaseData.TitleID = TitleID;
	Owner->BaseData.IsShowTitle = IsShowTitle;
	Owner->TempData.UpdateInfo.Recalculate_All = true;

	//////////////////////////////////////////////////////////////////////////
	{
		char	luaEvent[128];
		GameObjDbStructEx* titleObjDB = Global::GetObjDB( TitleID );
		if( titleObjDB != NULL  && strlen(titleObjDB->Title.LuaEvent) != 0 )
		{
			sprintf( luaEvent , "%s(1)",titleObjDB->Title.LuaEvent );
			LUA_VMClass::PCallByStrArg( luaEvent , Owner->GUID() , Owner->GUID()  );
		}
	}
	//////////////////////////////////////////////////////////////////////////

}
//-----------------------------------------------------------------
//通知週圍的人升級
void NetSrv_RoleValueChild::NotifyLvUp( RoleDataEx*	Owner )
{    

    RoleDataEx*	Other;

    Global::ResetRange( Owner , _Def_View_Block_RangeAttack_ );
    while( 1 ) 
    {
        Other = Global::GetRangePlayer();
        if( Other == NULL )
            break;

        S_NotifyLvUp( Other->TempData.Sys.GUID , Owner->TempData.Sys.GUID , Owner->TempData.Attr.Ability->Level );
    }
	
	BaseItemObject* OwnerObj = Global::GetObj( Owner->GUID() );
	if( OwnerObj != NULL )
		OwnerObj->PlotVM()->PCall( "Lua_System_Role_LV_UP", Owner->GUID() , Owner->GUID() );
}

/*
//通知周圍玩家某一個人的頭銜資訊
void NetSrv_RoleValueChild::SendRangeTitleInfo( RoleDataEx*	Owner )
{
    RoleDataEx*	Other;
    Global::ResetRange( Owner , _Def_View_Block_RangeAttack_ );
    while( 1 ) 
    {
        Other = Global::GetRangePlayer();
        if( Other == NULL )
            break;

        S_Title( Other->TempData.Sys.GUID , Owner->TempData.Sys.GUID , Owner->BaseData.IsShowTitle , Owner->BaseData.TitleID , Owner->PlayerBaseData->TitleStr.Begin() );
    } 
}
*/

//(當等級上升時)通知周圍玩家某一個人的等級 或換職業
void NetSrv_RoleValueChild::SendRangeRoleInfo( RoleDataEx*	Owner )
{
    RoleDataEx*	Other;
    Global::ResetRange( Owner , _Def_View_Block_Range_ );
    while( 1 ) 
    {
        Other = Global::GetRangePlayer();
        if( Other == NULL )
            break;

		S_RoleInfoChange( Other->GUID() , Owner );
    } 
}

//通知周圍的人某物件的mode
void NetSrv_RoleValueChild::SendRangeObjMode( RoleDataEx*	Owner )
{
	if( Owner->TempData.BackInfo.Mode._Mode == Owner->BaseData.Mode._Mode )
		return;

	Owner->TempData.BackInfo.Mode._Mode = Owner->BaseData.Mode._Mode;

	if( Owner->BaseData.Mode.IsAllZoneVisible == false )
	{
		RoleDataEx*	Other;
		Global::ResetRange( Owner , _Def_View_Block_Range_ );
		while( 1 ) 
		{
			Other = Global::GetRangePlayer();
			if( Other == NULL )
				break;

			S_ObjMode( Other->GUID() , Owner->GUID() , Owner->BaseData.Mode );
		} 
	}
	else
	{
		S_AllCli_ObjMode( Owner->GUID() , Owner->BaseData.Mode );
	}
}


void NetSrv_RoleValueChild::R_CloseChangeJobRequest		( BaseItemObject*	Obj  )
{
    RoleDataEx* Owner = Obj->Role();


	if( Owner->TempData.Attr.Action.OpenType != EM_RoleOpenMenuType_ChangeJob )
		return;

	Owner->ClsOpenMenu();
	//Owner->TempData.ShopInfo.Init();
	//Owner->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_None;

	S_CloseChangeJob( Obj->GUID() );
}
void NetSrv_RoleValueChild::R_ChangeJobRequest		( BaseItemObject*	Obj , int Job , int Job_Sub )
{
    RoleDataEx* Owner = Obj->Role();

	if(	Owner->CheckOpenMenu( EM_RoleOpenMenuType_ChangeJob) == false )
	{
		S_ChangeJobResult( Obj->GUID() , EM_ChangeJobResult_Failed );
		return;
	}
	
	if( (unsigned) Job > EM_Max_Vocation || Owner->PlayerBaseData->AbilityList[ Job ].Level == 0 )
	{
		S_ChangeJobResult( Obj->GUID() , EM_ChangeJobResult_Failed );
		return;
	}
	
	if(		Job_Sub != -1 
		&& ( (unsigned) Job_Sub > EM_Max_Vocation || Owner->PlayerBaseData->AbilityList[ Job_Sub ].Level == 0) )
	{
		S_ChangeJobResult( Obj->GUID() , EM_ChangeJobResult_Failed );
		return;
	}
	if( Owner->BaseData.Voc == Job && Owner->BaseData.Voc_Sub == Job_Sub )
	{
		S_ChangeJobResult( Obj->GUID() , EM_ChangeJobResult_Failed );
		return;
	}

	if( Owner->IsSpell() != false )
	{
		S_ChangeJobResult( Obj->GUID() , EM_ChangeJobResult_Failed );
		return;
	}


	//清除法術Buff
	//清除Buff
	for( int i = 0 ; i < Owner->BaseData.Buff.Size() ; i++ )
	{
		GameObjDbStructEx*	Magic = (GameObjDbStructEx*)Owner->BaseData.Buff[i].Magic;
		if(		Magic->IsMagicBase() == false
			||	(	Magic->MagicBase.Setting.DeadNotClear == false 	&&	Magic->MagicBase.Setting.GoodMagic )
			||	Magic->MagicBase.ClearTime.ChangeJob )
		{
			Owner->ClearBuffByPos( i );
			i--;
		}
	}

	if( Global::Ini()->IsHouseActionLog == true )
	{//改變職業
		char Buf[128];
		sprintf_s( Buf , sizeof(Buf) , "Old:[%d] _ [%d]  New:[%d] _ [%d] " , Owner->BaseData.Voc , Owner->BaseData.Voc_Sub , Job , Job_Sub );
		Log_House( Owner , -1 , -1 , EM_HouseActionType_ChangeVoc , Buf );
	}

	Owner->SetValue( EM_RoleValue_VOC		, Job , NULL );
	Owner->SetValue( EM_RoleValue_VOC_SUB	, Job_Sub , NULL );

	if( Owner->CheckWearTitle( Owner->BaseData.TitleID) == false )
	{
		Owner->BaseData.TitleID = 0;
	}
	//Owner->InitCal();
	Owner->OnTimeProc( true );

	Owner->BaseData.HP = Owner->TempData.Attr.Fin.MaxHP;
	Owner->BaseData.MP = Owner->TempData.Attr.Fin.MaxMP;
	Owner->BaseData.SP = 0;
	Owner->BaseData.SP_Sub = 0;

	NetSrv_AttackChild::SendRangeMaxHPMPInfo( Owner );
	NetSrv_AttackChild::SendRangeHPInfo( Owner );


	S_ChangeJobResult( Obj->GUID() , EM_ChangeJobResult_OK );
}

void NetSrv_RoleValueChild::R_CancelClientLogout	 ( BaseItemObject*  Obj )
{
	Obj->Role()->TempData.Move.LastMoveTime = RoleDataEx::G_SysTime + 10000;
}

void NetSrv_RoleValueChild::R_ClientLogout			( BaseItemObject*	Obj )
{
	if( Obj->Role()->TempData.Sys.WaitLogout != false )
	{
		return;
	}

	S_ClientLogoutResult( Obj->GUID() , 10 );

	Obj->Role()->TempData.Move.LastMoveTime = 0;
	Obj->Role()->TempData.Sys.WaitLogout = true;

	Schedular()->Push( _ClientLogoutSchedular , 1000 , NULL 
		, "dbid" , EM_ValueType_Int , Obj->Role()->DBID()  
		, "guid" , EM_ValueType_Int , Obj->GUID()  
		, "time" , EM_ValueType_Int , 12
		, NULL );
}

int  NetSrv_RoleValueChild::_ClientLogoutSchedular( SchedularInfo* Obj , void* InputClass )
{
	int DBID = Obj->GetNumber("dbid");
	int GUID = Obj->GetNumber("guid");
	int Time = Obj->GetNumber("time");
	
	RoleDataEx* Owner = Global::GetRoleDataByGUID( GUID );
	if(		Owner == NULL 
		||	DBID != Owner->DBID() 
		||	Owner->IsMove() )
	{
		if( Owner != NULL )
		{
			Owner->TempData.Sys.WaitLogout = false;
			S_CancelClientLogout( Owner->GUID() );
		}
		return 0;
	}
	if( Owner->TempData.Sys.WaitLogout == false )
		return 0;

	if( Owner->IsNPC() )
		return 0;

	if( Time <= 0 )
	{
		NetSrv_CliConnect::CliLogoutResult( Owner->GUID() , true );
		Owner->Destroy( "_ClientLogoutSchedular" );
		return 0;
	}

	char Buf[256];
	sprintf( Buf , "--%d--" , Time );
	Owner->Msg( Buf );
	Time --;

	Schedular()->Push( _ClientLogoutSchedular , 1000 , NULL 
		, "dbid" , EM_ValueType_Int , DBID
		, "guid" , EM_ValueType_Int , GUID
		, "time" , EM_ValueType_Int , Time
		, NULL );

	return 0;
}

void NetSrv_RoleValueChild::R_SetPlayerAllZoneState		( BaseItemObject*	Obj , PlayerStateStruct& State )
{
	RoleDataEx* Owner = Obj->Role();

//	if( Owner->BaseData.PartyID >= 0 )
//		State.FindParty = false;

	if( Owner->TempData.Attr.AllZoneState._State == State._State )
		return;

	Owner->BaseData.SysFlag.FindParty = Owner->TempData.Attr.AllZoneState.FindParty;

	Owner->TempData.Attr.AllZoneState._State = State._State&0xffff;

	Owner->TempData.UpdateInfo.AllZoneInfoChange = 1;
	SendRangeSetPlayerAllZoneState( Owner );
}

//通知周圍玩家 全區角色旗標
void NetSrv_RoleValueChild::SendRangeSetPlayerAllZoneState( RoleDataEx* Owner )
{
	RoleDataEx*	Other;
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;

		S_SetPlayerAllZoneState( Other->GUID() , Owner->GUID() , Other->TempData.Attr.AllZoneState );
	} 
}
void NetSrv_RoleValueChild::R_SetClientFlag		( BaseItemObject*	Obj , StaticFlag<_MAX_CLIENT_FLAG_COUNT_>& ClientFlag )
{
	Obj->Role()->PlayerBaseData->ClientFlag = ClientFlag;
//	memcpy( (char*)&(Obj->Role()->PlayerBaseData->ClientFlag) , (char*)&ClientFlag , sizeof(Obj->Role()->PlayerBaseData->ClientFlag) );
}
//-----------------------------------------------------------------
void NetSrv_RoleValueChild::R_ChangeParallel		( BaseItemObject*	Obj , int ParallelID )
{
	if( Obj->Role()->TempData.Sys.WaitChangeParallel != false )
	{
		S_WaitChangeParallel( Obj->GUID() , 0 , false );
		return;
	}

	if( Obj->Role()->IsDead() != false )
	{
		S_WaitChangeParallel( Obj->GUID() , 0 , false );
		return;
	}

	
	//int ZoneID = GlobalBase::GetParallelZoneID( Role->G_ZoneID , ParalledID );
	int ZoneID = GlobalBase::GetParallelZoneID( Obj->Role()->G_ZoneID , ParallelID );

	if( ZoneID == RoleDataEx::G_ZoneID )
	{
		//ChangeParalledID( GUID , ParallelID );
		ChangeParalledID( Obj->GUID() , ParallelID );
		return;
	}




	S_WaitChangeParallel( Obj->GUID() , 20 , true );

	Obj->Role()->TempData.Sys.WaitChangeParallel = true;

	Schedular()->Push( _ClientChangeParallelSchedular , 1000 , NULL 
		, "dbid" 		, EM_ValueType_Int , Obj->Role()->DBID()  
		, "guid" 		, EM_ValueType_Int , Obj->GUID()  
		, "time" 		, EM_ValueType_Int , 22
		, "ParallelID"	, EM_ValueType_Int , ParallelID
		, NULL );
}
void NetSrv_RoleValueChild::R_CancelChangeParallel	( BaseItemObject*	Obj )
{
	Obj->Role()->TempData.Sys.WaitChangeParallel = false;
}
int  NetSrv_RoleValueChild::_ClientChangeParallelSchedular( SchedularInfo* Obj , void* InputClass )
{
	int DBID 		= Obj->GetNumber("dbid");
	int GUID 		= Obj->GetNumber("guid");
	int Time 		= Obj->GetNumber("time");
	int ParallelID	= Obj->GetNumber( "ParallelID" );

	RoleDataEx* Owner = Global::GetRoleDataByGUID( GUID );
	if(		Owner == NULL || DBID != Owner->DBID() )
		return 0;

	if( Owner->TempData.Sys.WaitChangeParallel == false )
		return 0;

	if( Owner->IsNPC() )
		return 0;

	if(		Owner->IsDead() != false 
		||	( Owner->IsMove() && Time < 16 ) )
	{
		S_WaitChangeParallel( Obj->GUID() , 0 , false );
		return 0;
	}


	if( Time <= 0 )
	{
		ChangeParalledID( GUID , ParallelID );
		return 0;
	}

	char Buf[256];
	sprintf( Buf , "--%d--" , Time );
	Owner->Msg( Buf );
	Time --;

	Schedular()->Push( _ClientChangeParallelSchedular , 1000 , NULL 
		, "dbid" 		, EM_ValueType_Int , DBID
		, "guid" 		, EM_ValueType_Int , GUID
		, "time" 		, EM_ValueType_Int , Time
		, "ParallelID"	, EM_ValueType_Int , ParallelID
		, NULL );

	return 0;
}

void NetSrv_RoleValueChild::R_ClientComputerInfo	( BaseItemObject*	Obj , ClientComputerInfoStruct& Info )
{
	Global::Log_ClientComputerInfo( Obj->Role() , Info );
}

void NetSrv_RoleValueChild::R_SetClientData		( BaseItemObject*	Obj , char* ClientData )
{
	memcpy( Obj->Role()->PlayerBaseData->ClientData , ClientData , sizeof( Obj->Role()->PlayerBaseData->ClientData ) );
}
void NetSrv_RoleValueChild::R_ClientEnvironmentState	( BaseItemObject*	Obj , ClientStateStruct& State )
{
	RoleDataEx* role = Obj->Role();
	role->TempData.CliState = State;
	if( Ini()->IsNoSwimming == true )
	{
		State.InWater = false;
		role->AddHP( role , role->MaxHP() * -1 - 1000 );
	}

	if( State.InWater )
	{
		role->TempData.BackInfo.BuffClearTime.OnWater = true;
	}
	else
	{
		role->TempData.BackInfo.BuffClearTime.OnNoWater = true;
	}
}