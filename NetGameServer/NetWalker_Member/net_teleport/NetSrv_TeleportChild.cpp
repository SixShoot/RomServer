#include "../NetWakerGSrvInc.h"
#include "NetSrv_TeleportChild.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_TeleportChild::Init()
{
    NetSrv_Teleport::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_TeleportChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_TeleportChild::Release()
{
    if( This == NULL )
        return false;

	NetSrv_Teleport::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
void NetSrv_TeleportChild::R_AddTeleportRequest	( BaseItemObject* Obj , int ItemPos , int TeleportPos , LocationSaveStruct& Info )
{
	RoleDataEx* Owner = Obj->Role();
	
	if( (unsigned)TeleportPos >= _MAX_LOCATION_SAVE_COUNT_ || RoleDataEx::G_ZoneID % 1000 >= 100 )
	{
		Owner->Net_FixItemInfo_Body( ItemPos );
		SC_AddTeleportResult( Owner->GUID() , TeleportPos , Info , false );
		return;
	}
	int	AddTeleportID = g_ObjectData->GetSysKeyValue( "Teleport_SaveItem" );

	if( Owner->PlayerBaseData->VipMember.Term < RoleDataEx::G_Now_Float )
	{
		Owner->PlayerBaseData->VipMember.Term = 0;
		Owner->PlayerBaseData->VipMember._Flag = 0;
		Owner->PlayerBaseData->VipMember.Type = 0;
	}

	ItemFieldStruct* BodyItem = NULL;
	if(	Owner->PlayerBaseData->VipMember.Flag_Teleport_Save == false )
	{
		BodyItem = Owner->GetBodyItem( ItemPos );	
		if( BodyItem == NULL || BodyItem->IsEmpty() != false || BodyItem->OrgObjID != AddTeleportID	)
		{
			Owner->Net_FixItemInfo_Body( ItemPos );
			SC_AddTeleportResult( Owner->GUID() , TeleportPos , Info , false );
			return;
		}
	}
	LocationSaveStruct& SrvInfo = Owner->PlayerBaseData->LocationSave[ TeleportPos ];

	Info.ZoneID = RoleDataEx::G_ZoneID % 1000;
	Info.X = Owner->TempData.Move.CliX;
	Info.Y = Owner->TempData.Move.CliY;
	Info.Z = Owner->TempData.Move.CliZ;

	SrvInfo = Info;

	if( BodyItem != NULL )
	{
		Log_ItemDestroy( Owner , EM_ActionType_UseItemDestroy , *BodyItem , 1 , -1 , "" );
		if( BodyItem->Count <= 1 )
			BodyItem->Init();
		else
			BodyItem->Count --;
	}
	
	Owner->Net_FixItemInfo_Body( ItemPos );
	SC_AddTeleportResult( Owner->GUID() , TeleportPos , Info , true );

}
void NetSrv_TeleportChild::R_DelTeleportRequest	( BaseItemObject* Obj , int TeleportPos )
{
	RoleDataEx* Owner = Obj->Role();

	if( (unsigned)TeleportPos >= _MAX_LOCATION_SAVE_COUNT_ )
	{
		SC_DelTeleportResult( Owner->GUID() , TeleportPos , false );
		return;
	}

	LocationSaveStruct& SrvInfo = Owner->PlayerBaseData->LocationSave[ TeleportPos ];
	SrvInfo.Init();
	SC_DelTeleportResult( Owner->GUID() , TeleportPos , true );
}
void NetSrv_TeleportChild::R_SwapTeleportRequest	( BaseItemObject* Obj , int TeleportPos[2] )
{
	RoleDataEx* Owner = Obj->Role();

	if(		(unsigned)TeleportPos[0] >= _MAX_LOCATION_SAVE_COUNT_ 
		||	(unsigned)TeleportPos[1] >= _MAX_LOCATION_SAVE_COUNT_ )
	{
		SC_SwapTeleportResult( Owner->GUID() , TeleportPos , false );
		return;
	}

	LocationSaveStruct& SrvInfo1 = Owner->PlayerBaseData->LocationSave[ TeleportPos[0] ];
	LocationSaveStruct& SrvInfo2 = Owner->PlayerBaseData->LocationSave[ TeleportPos[1] ];

	std::swap( SrvInfo1 , SrvInfo2 );
	
	SC_SwapTeleportResult( Owner->GUID() , TeleportPos , true );
}
void NetSrv_TeleportChild::R_ModifyTeleportRequest( BaseItemObject* Obj , int TeleportPos , LocationSaveStruct& Info )
{
	RoleDataEx* Owner = Obj->Role();

	if( (unsigned)TeleportPos >= _MAX_LOCATION_SAVE_COUNT_ )
	{
		SC_ModifyTeleportResult( Owner->GUID() , TeleportPos , Info , false );
		return;
	}

	LocationSaveStruct& SrvInfo = Owner->PlayerBaseData->LocationSave[ TeleportPos ];

	Info.ZoneID = SrvInfo.ZoneID;	
	Info.X = SrvInfo.X;
	Info.Y = SrvInfo.Y;
	Info.Z = SrvInfo.Z;

	SrvInfo = Info;

	SC_ModifyTeleportResult( Owner->GUID() , TeleportPos , Info , true );
}
void NetSrv_TeleportChild::R_UseTeleportRequest	( BaseItemObject* Obj , UseTeleportTypeENUM Type , int ItemPos , int TeleportPos )
{
	RoleDataEx* Owner = Obj->Role();

	if(		(unsigned)TeleportPos >= _MAX_LOCATION_SAVE_COUNT_ 
		||	 RoleDataEx::G_ZoneID % 1000 >= 100 
		||	Owner->IsDead() != false )
	{
		Owner->Net_FixItemInfo_Body( ItemPos );
		SC_UseTeleportResult( Owner->GUID() , Type , ItemPos , TeleportPos , false );
		return;
	}

	if( Owner->PlayerBaseData->VipMember.Term < RoleDataEx::G_Now_Float )
	{
		Owner->PlayerBaseData->VipMember.Term = 0;
		Owner->PlayerBaseData->VipMember._Flag = 0;
		Owner->PlayerBaseData->VipMember.Type = 0;
	}

	ItemFieldStruct* BodyItem = NULL;
	if(		Owner->PlayerBaseData->VipMember.Flag_Teleport == true
		&&	Type == EM_UseTeleportType_Normal )
	{

	}
	else
	{
		char	Buf[256];
		sprintf( Buf , "Teleport_UseItem%d" , Type+1 );
		int	AddTeleportID = g_ObjectData->GetSysKeyValue( Buf );

		BodyItem = Owner->GetBodyItem( ItemPos );	
		if( BodyItem == NULL || BodyItem->IsEmpty() != false || BodyItem->OrgObjID != AddTeleportID	)
		{
			Owner->Net_FixItemInfo_Body( ItemPos );
			SC_UseTeleportResult( Owner->GUID() , Type , ItemPos , TeleportPos , false );
			return;
		}
	}

	if(  Owner->TempData.Attr.Effect.TeleportDisable != false )
	{
		Owner->Net_FixItemInfo_Body( ItemPos );
		SC_UseTeleportResult( Owner->GUID() , Type , ItemPos , TeleportPos , false );
		return;
	}

	LocationSaveStruct& SrvInfo = Owner->PlayerBaseData->LocationSave[ TeleportPos ];

	if( SrvInfo.ZoneID% 1000  >= 100 )
	{
		Owner->Net_FixItemInfo_Body( ItemPos );
		SC_UseTeleportResult( Owner->GUID() , Type , ItemPos , TeleportPos , false );
		return;
	}

	int ZoneID = GlobalBase::GetParallelZoneID( SrvInfo.ZoneID ,  Owner->PlayerTempData->ParallelZoneID );
	if( ZoneID == -1 )
	{
		Owner->Net_FixItemInfo_Body( ItemPos );
		SC_UseTeleportResult( Owner->GUID() , Type , ItemPos , TeleportPos , false );
		return;
	}

	if( BodyItem != NULL )
	{
		Log_ItemDestroy( Owner , EM_ActionType_UseItemDestroy , *BodyItem , 1 , -1 , "" );
		if( BodyItem->Count <= 1 )
			BodyItem->Init();
		else
			BodyItem->Count --;
	}
	Owner->Net_FixItemInfo_Body( ItemPos );
	SC_UseTeleportResult( Owner->GUID() , Type , ItemPos , TeleportPos ,  true );


	switch( Type )
	{
	case EM_UseTeleportType_Normal:		//一般傳送
		Global::ChangeZone( Owner->GUID() , ZoneID , 0 , SrvInfo.X , SrvInfo.Y , SrvInfo.Z  , Owner->Pos()->Dir );
		break;
	case EM_UseTeleportType_OneWayHole:	//單向傳送
		Global::CreateTeleport( Owner->X() , Owner->Y() , Owner->Z() , ZoneID ,SrvInfo.X , SrvInfo.Y , SrvInfo.Z  , Owner->RoleName() , 2 );		
		break;
	case EM_UseTeleportType_TwoWayHole:	//雙向傳送
		Global::CreateTeleport( Owner->X() , Owner->Y() , Owner->Z() , ZoneID ,SrvInfo.X , SrvInfo.Y , SrvInfo.Z  , Owner->RoleName() );		
		SL_CreateTeleport( RoleDataEx::G_ZoneID , Owner->X(), Owner->Y() , Owner->Z() , ZoneID , SrvInfo.X , SrvInfo.Y , SrvInfo.Z  , Owner->RoleName() );
		break;
	}
}

void NetSrv_TeleportChild::RL_CreateTeleport		( int FromZone , float FromX , float FromY , float FromZ , int ToZone , float ToX , float ToY , float ToZ , const char* Name ) 
{
	Global::CreateTeleport( ToX , ToY , ToZ , FromZone , FromX , FromY , FromZ , Name , 1 );
}