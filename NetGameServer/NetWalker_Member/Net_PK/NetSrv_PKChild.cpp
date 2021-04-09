#include "../NetWakerGSrvInc.h"
#include "NetSrv_PKChild.h"
#include <string.h>
//-----------------------------------------------------------------
#define _DEF_MAX_PK_RANGE_	500
#define _DEF_PK_TIME_		300
//-----------------------------------------------------------------
bool    NetSrv_PKChild::Init()
{
    NetSrv_PK::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_PKChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_PKChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_PK::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
void	NetSrv_PKChild::SetWinLost( RoleDataEx* Winner , RoleDataEx* Loser )
{
	SC_PKEnd( Winner->GUID() , Winner->GUID() , Loser->GUID() , false );
	SC_PKEnd( Loser->GUID() , Winner->GUID() , Loser->GUID() , false );
	Winner->SetPKOff();
	Loser->SetPKOff();

	int Score = Loser->PlayerBaseData->PKScore / 100;
	Loser->PlayerBaseData->PKScore -= Score;
	Winner->PlayerBaseData->PKScore += Score;

	Loser->StopAttack();
	Winner->StopAttack();
	Loser->TempData.NPCHate.Init();
	Winner->TempData.NPCHate.Init();
}
//-----------------------------------------------------------------
int  NetSrv_PKChild::_RolePKBeginState( SchedularInfo* Obj , void* InputClass )    //檢查PK狀態
{
	int	DBID1 = Obj->GetNumber("DBID1");
	int	GUID1 = Obj->GetNumber("GUID1");

	int	DBID2 = Obj->GetNumber("DBID2");
	int	GUID2 = Obj->GetNumber("GUID2");

	RoleDataEx* Owner  = Global::GetRoleDataByGUID( GUID1 );
	RoleDataEx* Target = Global::GetRoleDataByGUID( GUID2 );
	PKInfoStruct*	OPKInfo;
	PKInfoStruct*	TPKInfo;

	if( Owner != NULL )
	{
		OPKInfo = &Owner->PlayerTempData->PKInfo;
		if( Owner->DBID() != DBID1  )
			Owner = NULL;
	}
	if( Target != NULL )
	{
		TPKInfo = &Target->PlayerTempData->PKInfo;

		if( Target->DBID() != DBID2 )
			Target = NULL;
	}

	if( Owner == NULL && Target == NULL )
		return 0;


	//距離判斷
	if( Owner != NULL && Target != NULL )
	{
		if(		OPKInfo->State == EM_PKState_End 
			||	TPKInfo->State == EM_PKState_End )
		{

			Target->SetPKOff();
			Owner->SetPKOff();

			return 0;
		}

		OPKInfo->Clock -= 3;
		TPKInfo->Clock -= 3;

		float OLength = Owner->Length( TPKInfo->X , TPKInfo->Y , TPKInfo->Z ); 
		float TLength = Target->Length( TPKInfo->X , TPKInfo->Y , TPKInfo->Z );

		if( OLength > _DEF_MAX_PK_RANGE_  && TLength > _DEF_MAX_PK_RANGE_ )
		{
			if( OPKInfo->State == EM_PKState_Begin_OutRange && TPKInfo->State == EM_PKState_Begin_OutRange )
			{	//平手
				SC_PKEnd( Owner->GUID() , Owner->GUID() , Target->GUID() , true );
				SC_PKEnd( Target->GUID() , Owner->GUID() , Target->GUID() , true );
				Target->SetPKOff();
				Owner->SetPKOff();
				return 0;
			}
			else if( TPKInfo->State == EM_PKState_Begin_OutRange  )
			{
				SetWinLost( Owner , Target );
				return 0;
			}
			else if( OPKInfo->State == EM_PKState_Begin_OutRange  )
			{
				SetWinLost( Target , Owner );
				return 0;
			}
			else
			{
				OPKInfo->State = EM_PKState_Begin_OutRange;
				TPKInfo->State = EM_PKState_Begin_OutRange;
			}
		}	
		else if( OLength > _DEF_MAX_PK_RANGE_ )
		{
			if( OPKInfo->State == EM_PKState_Begin_OutRange )
			{
				SetWinLost( Target , Owner );
				return 0;
			}

			TPKInfo->State = EM_PKState_Begin;
			OPKInfo->State = EM_PKState_Begin_OutRange;	
		}
		else if( TLength > _DEF_MAX_PK_RANGE_ )
		{
			if( TPKInfo->State == EM_PKState_Begin_OutRange )
			{
				SetWinLost( Owner , Target );
				return 0;
			}

			TPKInfo->State = EM_PKState_Begin_OutRange;	
			OPKInfo->State = EM_PKState_Begin;
		}
		else
		{
			TPKInfo->State = EM_PKState_Begin;	
			OPKInfo->State = EM_PKState_Begin;
		}

		//時間到
		if( OPKInfo->Clock <= 0 )
		{
			SC_PKEnd( Owner->GUID() , Target->GUID() , Owner->GUID() , true );
			SC_PKEnd( Target->GUID() , Target->GUID() , Owner->GUID() , true );
			Target->SetPKOff();
			Owner->SetPKOff();
			return 0;
		}
	}
	else if( Owner != NULL  )
	{
		SC_PKEnd( Owner->GUID() , Owner->GUID() , -1 , false );
		Owner->SetPKOff();
		return 0;
	}
	else if( Target != NULL  )
	{
		SC_PKEnd( Target->GUID() , Target->GUID() , -1 , true );
		Target->SetPKOff();
		return 0;
	}

	Schedular()->Push( _RolePKBeginState , 5000 , NULL ,
		"GUID1" , EM_ValueType_int , GUID1	,
		"DBID1" , EM_ValueType_int , DBID1 	, 
		"GUID2" , EM_ValueType_int , GUID2	,
		"DBID2" , EM_ValueType_int , DBID2 	, 
		NULL );
	return 0;
}

int  NetSrv_PKChild::_RolePKPrepareState( SchedularInfo* Obj , void* InputClass )    //檢查PK狀態
{
	int	DBID1 = Obj->GetNumber("DBID1");
	int	GUID1 = Obj->GetNumber("GUID1");

	int	DBID2 = Obj->GetNumber("DBID2");
	int	GUID2 = Obj->GetNumber("GUID2");

	RoleDataEx* Owner  = Global::GetRoleDataByGUID( GUID1 );
	RoleDataEx* Target = Global::GetRoleDataByGUID( GUID2 );
	PKInfoStruct*	OPKInfo;
	PKInfoStruct*	TPKInfo;

	if( Owner != NULL )
	{
		OPKInfo = &Owner->PlayerTempData->PKInfo;
		if( Owner->DBID() != DBID1  )
			Owner = NULL;
	}
	if( Target != NULL )
	{
		TPKInfo = &Target->PlayerTempData->PKInfo;

		if( Target->DBID() != DBID2 )
			Target = NULL;
	}

	if( Owner == NULL && Target == NULL )
		return 0;

	if( Owner != NULL && Target != NULL )
	{
		if(		OPKInfo->State == EM_PKState_End 
			||	TPKInfo->State == EM_PKState_End )
		{
			Owner->SetPKOff();
			Target->SetPKOff();
			return 0;
		}

		if(		Target->Length( TPKInfo->X , TPKInfo->Y , TPKInfo->Z ) > _DEF_MAX_PK_RANGE_ 
			||	Owner ->Length( OPKInfo->X , OPKInfo->Y , OPKInfo->Z ) > _DEF_MAX_PK_RANGE_ )
		{
			Owner->SetPKOff();
			Target->SetPKOff();
			SC_PKCancel( Owner->GUID() );
			SC_PKCancel( Target->GUID() );
			return 0;
		}


		if( OPKInfo->State == EM_PKState_Prepare && TPKInfo->State == EM_PKState_Prepare  )
		{
			OPKInfo->State = EM_PKState_Begin;
			OPKInfo->Clock = _DEF_PK_TIME_;		//五分鐘
			TPKInfo->State = EM_PKState_Begin;
			TPKInfo->Clock = _DEF_PK_TIME_;	
			SC_PKBegin( Owner->GUID() , _DEF_PK_TIME_);
			SC_PKBegin( Target->GUID() , _DEF_PK_TIME_ );
			
		}
		else 
		{
			Owner->SetPKOff();
			Target->SetPKOff();
			//Owner->Msg( "PK處理有問題" );
			//Target->Msg( "PK處理有問題" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_PK_PKError );
			Target->Net_GameMsgEvent( EM_GameMessageEvent_PK_PKError );
			return 0;
		}
	}
	else 
	{
		if( Owner != NULL )
		{
			SC_PKCancel( Owner->GUID() );
			Owner->SetPKOff();

		}
		else if( Target != NULL )
		{
			SC_PKCancel( Target->GUID() );
			Target->SetPKOff();
		}
		return 0;
	}
	
	Owner->TempData.Attr.Action.PK = true;
	Target->TempData.Attr.Action.PK = true;

	Schedular()->Push( _RolePKBeginState , 5000 , NULL ,
		"GUID1" , EM_ValueType_int , GUID1	,
		"DBID1" , EM_ValueType_int , DBID1 	, 
		"GUID2" , EM_ValueType_int , GUID2	,
		"DBID2" , EM_ValueType_int , DBID2 	, 
		NULL );
	return 0;
}
void NetSrv_PKChild::R_PKInviteRequest	( BaseItemObject* OwnerObj , int TargetID )
{
	RoleDataEx* Owner = OwnerObj->Role();
	PKInfoStruct&	PKInfo = Owner->PlayerTempData->PKInfo;

	if( Global::Ini()->IsDisableDual != false )
	{
		SC_PKInviteResult( Owner->GUID() , TargetID , false );
		return;
	}
	if( PKInfo.State !=  EM_PKState_None )
	{

		switch( PKInfo.State )
		{
		case EM_PKState_Invite:		//要請
			//Owner->Msg( "PK要請失敗 已要請其他目標" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_PK_InviteError );
			break;
		case EM_PKState_Prepare:	//準備PK
			//Owner->Msg( "PK要請失敗 已在PK中" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_PK_Prepar );
			break;
		case EM_PKState_Begin:		//開始PK
		case EM_PKState_Begin_OutRange:
			//Owner->Msg( "PK要請失敗 已在PK中" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_PK_Begin );
			break;			
		}		
		SC_PKInviteResult( Owner->GUID() , TargetID , false );
		return;
		
	}

	RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );
	if( Target == NULL || Target->IsPlayer() == false )
	{
		//Owner->Msg( "PK要請失敗 角色不存在" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_PK_TargetNotExist );
		SC_PKInviteResult( Owner->GUID() , TargetID , false );
		return;
	}

	if( Target->Length( Owner ) > 200 )
	{
		//Owner->Msg( "PK要請失敗 目標距離太遠" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_PK_TargetTooFar );
		SC_PKInviteResult( Owner->GUID() , TargetID , false );
		return;
	}

	if( Target->PlayerTempData->PKInfo.State != EM_PKState_None )
	{
		switch( Target->PlayerTempData->PKInfo.State )
		{
		case EM_PKState_Invite:		//要請
			{
				//Owner->Msg( "PK要請失敗 目標已要請其他人" );
				Owner->Net_GameMsgEvent( EM_GameMessageEvent_PK_TargetInviteOther );
				break;
			}
		case EM_PKState_Prepare:	//準備PK
		case EM_PKState_Begin:		//開始PK
		case EM_PKState_Begin_OutRange:
			//Owner->Msg( "PK要請失敗 目標PK中" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_PK_TargetPK );
			break;
		}
		SC_PKInviteResult( Owner->GUID() , TargetID , false );
		return;
	}

	PKInfo.X = Owner->Pos()->X;
	PKInfo.Y = Owner->Pos()->Y;
	PKInfo.Z = Owner->Pos()->Z;
	PKInfo.State = EM_PKState_Invite;
	PKInfo.TargetID = Target->GUID();
	PKInfo.TargetDBID = Target->DBID();
	SC_PKInviteNotify( Target->GUID() , Owner->GUID() , Owner->PlayerBaseData->PKScore );

}
void NetSrv_PKChild::R_PKInviteResult	( BaseItemObject* OwnerObj , int TargetID , bool Result )
{
	RoleDataEx*		Owner = OwnerObj->Role();
	RoleDataEx*		OrgInvite = Global::GetRoleDataByGUID( TargetID );
	if( OrgInvite == NULL || OrgInvite->IsPlayer() == false )
	{
		//Owner->Msg( "PK回應資料有問題" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_PK_PKInviteResultError );
		return;
	}

	PKInfoStruct&	OrgPKInfo = OrgInvite->PlayerTempData->PKInfo;
	PKInfoStruct&	PKInfo = Owner->PlayerTempData->PKInfo;

	if( OrgPKInfo.TargetID != Owner->GUID() || OrgPKInfo.TargetDBID != Owner->DBID() )
	{
		//Owner->Msg( "PK回應資料有問題" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_PK_PKInviteResultError );
		return;
	}

	if( OrgPKInfo.State != EM_PKState_Invite )
	{
		//Owner->Msg( "PK回應資料有問題" );
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_PK_PKInviteResultError );
		return;
	}


	if( Result != false )
	{
		PKInfo.Clock = 0;
		PKInfo.TargetDBID = OrgInvite->DBID();
		PKInfo.TargetID = OrgInvite->GUID();
		PKInfo.State = EM_PKState_Prepare;
		PKInfo.X = OrgPKInfo.X;
		PKInfo.Y = OrgPKInfo.Y;
		PKInfo.Z = OrgPKInfo.Z;

		OrgPKInfo.State = EM_PKState_Prepare;
		SC_PKInviteResult( TargetID , Owner->GUID() , true );
		SC_PKPrepare( OrgInvite->GUID() , OrgInvite->GUID() , Owner->GUID() , OrgInvite->PlayerBaseData->PKScore , Owner->PlayerBaseData->PKScore
					, int( PKInfo.X ) , int( PKInfo.Y ) , int( PKInfo.Z )  , _DEF_MAX_PK_RANGE_ , 5 );
		SC_PKPrepare( Owner->GUID() , OrgInvite->GUID() , Owner->GUID() , OrgInvite->PlayerBaseData->PKScore , Owner->PlayerBaseData->PKScore
					, int( PKInfo.X ) , int( PKInfo.Y ) , int( PKInfo.Z ) , _DEF_MAX_PK_RANGE_ , 5 );


		Schedular()->Push( _RolePKPrepareState , 5000 , NULL ,
			"GUID1" , EM_ValueType_int , OrgInvite->GUID()	,
			"DBID1" , EM_ValueType_int , OrgInvite->DBID() 	,
			"GUID2" , EM_ValueType_int , Owner->GUID()	,
			"DBID2" , EM_ValueType_int , Owner->DBID() 	,
			NULL );
	}
	else
	{
		OrgPKInfo.Init();
		SC_PKInviteResult( TargetID , Owner->GUID() , false );
	}
	

}
void NetSrv_PKChild::R_PKCancel			( BaseItemObject* OwnerObj )
{
	RoleDataEx*		Owner  = OwnerObj->Role();
	PKInfoStruct&	PKInfo = Owner->PlayerTempData->PKInfo;

	RoleDataEx*		Target = Global::GetRoleDataByGUID( PKInfo.TargetID );
	if( Target == NULL )
		return;
	SC_PKCancel( Owner->GUID() );
	SC_PKCancel( Target->GUID() );

	switch( PKInfo.State )
	{
	case EM_PKState_None:		//一般狀態
		break;
	case EM_PKState_Invite:		//要請
		{
			PKInfo.Init();
			break;
		}
	case EM_PKState_Prepare:	//準備PK
		{
			PKInfo.State = EM_PKState_End;
			Target->PlayerTempData->PKInfo.State = EM_PKState_End;
		}
		break;
	case EM_PKState_Begin:		//開始PK
	case EM_PKState_Begin_OutRange:		//開始PK 超過範圍
		{
			SetWinLost( Target , Owner );

			PKInfo.State = EM_PKState_End;
			Target->PlayerTempData->PKInfo.State = EM_PKState_End;
		}
	}

}