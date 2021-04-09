#include "NetSrv_CliConnect.h"
#include "../net_battleground/NetSrv_BattleGround_Child.h"
#include "../net_serverlist/Net_ServerList_Child.h"
//-------------------------------------------------------------------
NetSrv_CliConnect*    NetSrv_CliConnect::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_CliConnect::_Init()
{

    _Net->RegOnCliConnectFunction       ( _OnCliConnect );
    _Net->RegOnCliDisconnectFunction    ( _OnCliDisconnect );
	_Net->RegOnLoginFunction			( _OnLogin );

    Srv_NetCliReg ( PG_CliConnect_CtoL_RequestLogout );
	Srv_NetCliReg ( PG_CliConnect_CtoL_ModifyNPCInfo );

	Srv_NetCliReg ( PG_CliConnect_CtoL_GetNPCMoveInfoRequest	);
	Srv_NetCliReg ( PG_CliConnect_CtoL_SaveNPCMoveInfo			);


	Srv_NetCliReg ( PG_CliConnect_CtoL_EditFlag	);
	Srv_NetCliReg ( PG_CliConnect_CtoL_DelFlag	);
	Srv_NetCliReg ( PG_CliConnect_CtoL_GetFlagList	);
	Srv_NetCliReg ( PG_CliConnect_CtoL_DelFlagGroup	);


	_Net->RegOnSrvPacketFunction	( EM_PG_CliConnect_LtoL_ChangeZoneNodify		, _PG_CliConnect_LtoL_ChangeZoneNodify	);
	_Net->RegOnSrvPacketFunction	( EM_PG_CliConnect_LtoL_ChangeZoneNodifyResult  , _PG_CliConnect_LtoL_ChangeZoneNodifyResult );
	_Net->RegOnSrvPacketFunction	( EM_PG_CliConnect_LtoL_DelTomb					, _PG_CliConnect_LtoL_DelTomb );


	Srv_NetCliReg ( PG_CliConnect_CtoL_ConnectToBattleWorldOK );
	_Net->RegOnOtherWorldPacketFunction	( EM_PG_CliConnect_LtoBL_ChangeZoneToOtherWorld			, _PG_CliConnect_LtoBL_ChangeZoneToOtherWorld		);
	_Net->RegOnOtherWorldPacketFunction	( EM_PG_CliConnect_BLtoL_ChangeZoneToOtherWorldResult	, _PG_CliConnect_BLtoL_ChangeZoneToOtherWorldResult	);
	_Net->RegOnOtherWorldPacketFunction	( EM_PG_CliConnect_LtoBL_ConnectCliAndGSrv				, _PG_CliConnect_LtoBL_ConnectCliAndGSrv			);
	_Net->RegOnOtherWorldPacketFunction	( EM_PG_CliConnect_BLtoL_RetNomorlWorld					, _PG_CliConnect_BLtoL_RetNomorlWorld				);
	_Net->RegOnOtherWorldPacketFunction	( EM_PG_CliConnect_BLtoBL_SendToPlayerByDBID			, _PG_CliConnect_BLtoBL_SendToPlayerByDBID			);
	_Net->RegOnOtherWorldPacketFunction	( EM_PG_CliConnect_LtoBL_ChangeZoneToOtherWorld_Zip		, _PG_CliConnect_LtoBL_ChangeZoneToOtherWorld_Zip	);
	_Net->RegOnOtherWorldPacketFunction	( EM_PG_CliConnect_BLtoL_RetNomorlWorld_Zip				, _PG_CliConnect_BLtoL_RetNomorlWorld_Zip			);
	
	_Net->RegOnOtherWorldPacketFunction	( EM_PG_CliConnect_BLtoL_BattleZone						, _PG_CliConnect_BLtoL_BattleZone			);
	_Net->RegOnOtherWorldPacketFunction	( EM_PG_CliConnect_LtoBL_ReturnWorldRequest				, _PG_CliConnect_LtoBL_ReturnWorldRequest	);
	_Net->RegOnOtherWorldPacketFunction	( EM_PG_CliConnect_BLtoL_ReturnWorldResult				, _PG_CliConnect_BLtoL_ReturnWorldResult	);


	_Net->RegOnOtherWorldPacketFunction	( EM_PG_CliConnect_BLtoL_BattleSavePlayer				, _PG_CliConnect_BLtoL_BattleSavePlayer				);
	_Net->RegOnOtherWorldPacketFunction	( EM_PG_CliConnect_BLtoL_BattleSavePlayer_Zip			, _PG_CliConnect_BLtoL_BattleSavePlayer_Zip			);

	Srv_NetCliReg ( PG_CliConnect_CtoL_WorldReturnOK );


	Global::Net_ServerList->RegServerLogoutEvent( EM_SERVER_TYPE_LOCAL , _OnLocalDisconnectEvent );
	Global::Net_ServerList->RegServerLoginEvent( EM_SERVER_TYPE_LOCAL , _OnLocalConnectEvent );

    return true;
}
//-------------------------------------------------------------------
bool NetSrv_CliConnect::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_CliConnect::_PG_CliConnect_BLtoL_BattleZone( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_CliConnect_BLtoL_BattleZone*  PG = (PG_CliConnect_BLtoL_BattleZone*)Data;
	This->RBL_BattleZone( FromWorldId , PG->ZoneID , FromNetID , PG->PlayerDBID );
}
void NetSrv_CliConnect::_PG_CliConnect_LtoBL_ReturnWorldRequest( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_CliConnect_LtoBL_ReturnWorldRequest*  PG = (PG_CliConnect_LtoBL_ReturnWorldRequest*)Data;
	This->RL_ReturnWorldRequest( FromWorldId , FromNetID , PG->PlayerDBID , PG->TestCount );
}
void NetSrv_CliConnect::_PG_CliConnect_BLtoL_ReturnWorldResult( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_CliConnect_BLtoL_ReturnWorldResult*  PG = (PG_CliConnect_BLtoL_ReturnWorldResult*)Data;
	This->RBL_ReturnWorldResult( FromWorldId , PG->PlayerDBID , PG->TestCount , PG->Result );
}

void NetSrv_CliConnect::_OnLocalDisconnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	This->OnLocalDisconnect( dwServerLocalID );
}
void NetSrv_CliConnect::_OnLocalConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	This->OnLocalConnect( dwServerLocalID );
}


void NetSrv_CliConnect::_OnLogin ( string Info )
{
	This->OnLogin( );
}
void NetSrv_CliConnect::_OnCliConnect( int ID , string Account )
{
    This->CliConnect( ID , (char*)Account.c_str() );
}
//-------------------------------------------------------------------
void NetSrv_CliConnect::_OnCliDisconnect( int CliID )
{
    BaseItemObject* Player = BaseItemObject::GetObjBySockID( CliID );

    if( Player == NULL )
        return;

    This->CliDisconnect( Player );
}
//-------------------------------------------------------------------
//收到的封包
//-------------------------------------------------------------------
void NetSrv_CliConnect::_PG_CliConnect_BLtoBL_SendToPlayerByDBID	( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_CliConnect_BLtoBL_SendToPlayerByDBID*   PG = (PG_CliConnect_BLtoBL_SendToPlayerByDBID*)Data;
	This->RBL_SendToPlayerByDBID( FromWorldId , FromNetID , PG->PlayerDBID , PG->DataSize , PG->Data );
}

void NetSrv_CliConnect::_PG_CliConnect_CtoL_WorldReturnOK( int Sockid , int Size , void* Data )
{

	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	This->RC_WorldReturnOK( Player ); 
}

void NetSrv_CliConnect::_PG_CliConnect_LtoL_DelTomb( int Sockid , int Size , void* Data )
{
	PG_CliConnect_LtoL_DelTomb*   PG = (PG_CliConnect_LtoL_DelTomb*)Data;
	This->RL_DelTomb( PG->TombGUID , PG->OwnerDBID );
}

void NetSrv_CliConnect::_PG_CliConnect_LtoBL_ChangeZoneToOtherWorld_Zip		( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_CliConnect_LtoBL_ChangeZoneToOtherWorld_Zip*   PG = (PG_CliConnect_LtoBL_ChangeZoneToOtherWorld_Zip*)Data;

	PlayerRoleData TempRole;

	MyLzoClass myZip;
	int RetSize = myZip.Decode( PG->Data , PG->Size , sizeof(TempRole) , (char*)&TempRole );
	assert( RetSize != 0 );
	This->RBL_ChangeZoneToOtherWorld( FromWorldId , FromNetID , TempRole , PG->RoleSize );
}
void NetSrv_CliConnect::_PG_CliConnect_LtoBL_ChangeZoneToOtherWorld			( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_CliConnect_LtoBL_ChangeZoneToOtherWorld*   PG = (PG_CliConnect_LtoBL_ChangeZoneToOtherWorld*)Data;
	This->RBL_ChangeZoneToOtherWorld( FromWorldId , FromNetID , PG->Role , PG->RoleSize );
}
void NetSrv_CliConnect::_PG_CliConnect_BLtoL_ChangeZoneToOtherWorldResult	( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_CliConnect_BLtoL_ChangeZoneToOtherWorldResult*   PG = (PG_CliConnect_BLtoL_ChangeZoneToOtherWorldResult*)Data;
	This->RBL_ChangeZoneToOtherWorldResult( FromWorldId , FromNetID , PG->RoleDBID , PG->IPStr , PG->Port , PG->Result );
}
void NetSrv_CliConnect::_PG_CliConnect_LtoBL_ConnectCliAndGSrv				( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_CliConnect_LtoBL_ConnectCliAndGSrv*   PG = (PG_CliConnect_LtoBL_ConnectCliAndGSrv*)Data;
	This->RBL_ConnectCliAndGSrv( FromWorldId , FromNetID , PG->CliNetID  );
}
void NetSrv_CliConnect::_PG_CliConnect_BLtoL_RetNomorlWorld_Zip					( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_CliConnect_BLtoL_RetNomorlWorld_Zip*   PG = (PG_CliConnect_BLtoL_RetNomorlWorld_Zip*)Data;

	PlayerRoleData TempRole;

	MyLzoClass myZip;
	int RetSize = myZip.Decode( PG->Data , PG->Size , sizeof(TempRole) , (char*)&TempRole );
	assert( RetSize != 0 );

	TempRole.SetLink();
	TempRole.DynamicData.TreasureBox = NULL;

	This->RBL_RetNomorlWorld( FromWorldId , FromNetID , PG->PlayerDBID , (RoleDataEx*)&(TempRole) );
}

void NetSrv_CliConnect::_PG_CliConnect_BLtoL_RetNomorlWorld					( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_CliConnect_BLtoL_RetNomorlWorld*   PG = (PG_CliConnect_BLtoL_RetNomorlWorld*)Data;

	PG->Role.SetLink();
	PG->Role.DynamicData.TreasureBox = NULL;

	This->RBL_RetNomorlWorld( FromWorldId , FromNetID , PG->PlayerDBID , (RoleDataEx*)&(PG->Role) );
}
void NetSrv_CliConnect::_PG_CliConnect_BLtoL_BattleSavePlayer_Zip				( int FromWorldId , int FromNetID , int Size , void* Data )
{

	PG_CliConnect_BLtoL_BattleSavePlayer_Zip*   PG = (PG_CliConnect_BLtoL_BattleSavePlayer_Zip*)Data;

	PlayerRoleData TempRole;

	MyLzoClass myZip;
	int RetSize = myZip.Decode( PG->Data , PG->Size , sizeof(TempRole) , (char*)&TempRole );
	assert( RetSize != 0 );

	TempRole.SetLink();
	TempRole.DynamicData.TreasureBox = NULL;
	/*
	PG->Role.SetLink();
	PG->Role.DynamicData.TreasureBox = NULL;
*/
	This->RBL_BattleSavePlayer( FromWorldId , FromNetID , PG->PlayerDBID , (RoleDataEx*)&(TempRole) );
}
void NetSrv_CliConnect::_PG_CliConnect_BLtoL_BattleSavePlayer					( int FromWorldId , int FromNetID , int Size , void* Data )
{
	PG_CliConnect_BLtoL_BattleSavePlayer*   PG = (PG_CliConnect_BLtoL_BattleSavePlayer*)Data;

	PG->Role.SetLink();
	PG->Role.DynamicData.TreasureBox = NULL;

	This->RBL_BattleSavePlayer( FromWorldId , FromNetID , PG->PlayerDBID , (RoleDataEx*)&(PG->Role) );
}

void NetSrv_CliConnect::_PG_CliConnect_CtoL_ConnectToBattleWorldOK			( int Sockid , int Size , void* Data )
{
	PG_CliConnect_CtoL_ConnectToBattleWorldOK*   PG = (PG_CliConnect_CtoL_ConnectToBattleWorldOK*)Data;
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );

	if( Player == NULL )
		return;

	This->RC_ConnectToBattleWorldOK( Player , PG->CliNetID );  
}

void NetSrv_CliConnect::_PG_CliConnect_CtoL_RequestLogout( int CliID, int Size , void* Data )
{
    PG_CliConnect_CtoL_RequestLogout*   PG = (PG_CliConnect_CtoL_RequestLogout*)Data;
    BaseItemObject* Player = BaseItemObject::GetObjBySockID( CliID );

    if( Player == NULL )
        return;

    This->CliLogout( Player );  
}

void NetSrv_CliConnect::_PG_CliConnect_LtoL_ChangeZoneNodify( int Sockid , int Size , void* Data )
{
	PG_CliConnect_LtoL_ChangeZoneNodify*   PG = (PG_CliConnect_LtoL_ChangeZoneNodify*)Data;
	
	PG->Role.DynamicData.TreasureBox = NULL;
	PG->Role.SetLink(); 
	//This->OnChangeZone( (RoleDataEx*)&(PG->Role) , PG->SrcZoneID );
	This->OnChangeZone( &PG->Role , PG->SrcZoneID );	
    //Global::AddOnLoginPlayer( (RoleDataEx*)pRole );
}

void NetSrv_CliConnect::_PG_CliConnect_LtoL_ChangeZoneNodifyResult( int Sockid , int Size , void* Data )
{
	PG_CliConnect_LtoL_ChangeZoneNodifyResult*   PG = (PG_CliConnect_LtoL_ChangeZoneNodifyResult*)Data;
	This->OnChangeZoneNodifyResult( PG->ZoneID , PG->GItemID , PG->X , PG->Y , PG->Z , PG->Result );
}

void NetSrv_CliConnect::_PG_CliConnect_CtoL_ModifyNPCInfo   ( int Sockid , int Size , void* Data )
{
    PG_CliConnect_CtoL_ModifyNPCInfo*   PG = (PG_CliConnect_CtoL_ModifyNPCInfo*)Data;
    This->OnModify( Sockid , &PG->Info );

}
void NetSrv_CliConnect::_PG_CliConnect_CtoL_GetNPCMoveInfoRequest	( int Sockid , int Size , void* Data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_CliConnect_CtoL_GetNPCMoveInfoRequest*   PG = (PG_CliConnect_CtoL_GetNPCMoveInfoRequest*)Data;
	This->RC_GetNPCMoveInfoRequest( Player , PG->GUID );

}
void NetSrv_CliConnect::_PG_CliConnect_CtoL_SaveNPCMoveInfo			( int Sockid , int Size , void* Data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_CliConnect_CtoL_SaveNPCMoveInfo*   PG = (PG_CliConnect_CtoL_SaveNPCMoveInfo*)Data;
	This->RC_SaveNPCMoveInfo( Player , PG->GUID , PG->TotalCount , PG->IndexID , PG->Base );
}
void NetSrv_CliConnect::_PG_CliConnect_CtoL_GetFlagList				( int Sockid , int Size , void* Data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	//PG_CliConnect_CtoL_GetFlagList Packet = ( PG_CliConnect_CtoL_GetFlagList* ) Data;
	This->RC_GetFlagList( Player );
}
void NetSrv_CliConnect::_PG_CliConnect_CtoL_EditFlag				( int Sockid , int Size , void* Data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_CliConnect_CtoL_EditFlag* Packet = ( PG_CliConnect_CtoL_EditFlag* ) Data;
	This->RC_EditFlag( Player, Packet->ObjID, Packet->ID, Packet->X, Packet->Y, Packet->Z, Packet->Dir, Packet->FlagDBID );
}
void NetSrv_CliConnect::_PG_CliConnect_CtoL_DelFlag					( int Sockid , int Size , void* Data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_CliConnect_CtoL_DelFlag* Packet = ( PG_CliConnect_CtoL_DelFlag* ) Data;
	This->RC_DelFlag( Player, Packet->GroupID, Packet->FlagID );
}
void NetSrv_CliConnect::_PG_CliConnect_CtoL_DelFlagGroup			( int Sockid , int Size , void* Data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_CliConnect_CtoL_DelFlagGroup* Packet = ( PG_CliConnect_CtoL_DelFlagGroup* ) Data;

	This->RC_DelFlagGroup( Player, Packet->iFlagGroupID );
}
//-------------------------------------------------------------------
// 資料送出
//-------------------------------------------------------------------
// 傳送區域基本資料
void NetSrv_CliConnect::SendZoneInfo( RoleDataEx* Player )
{
    PG_CliConnect_LtoC_ZoneInfo   Send;

    Send.ZoneID						= Player->ZoneID();
	Send.MapID						= Global::Ini()->MapID;
	Send.CampObjID					= Global::St()->Ini.CampStatuteObj;
//	Send.LanguageType				= (LanguageTypeENUM)Global::St()->Ini.LanguageType;
	Send.ZoneMode.Mode				= 0;
	Send.ZoneMode.IsPvE				= Global::Ini()->IsPvE;
	//Send.ZoneMode.IsBattleGround	= IsBattleGround;
	Send.ZoneMode.IsBattleGround	= Global::Ini()->IsBattleWorld;
	Send.ZoneMode.IsDisablePVPRule	= Global::Ini()->IsDisablePVPRule;
	Send.ZoneMode.IsDisableTrade	= Global::Ini()->IsDisableTrade;
	Send.ZoneMode.IsDisableDual		= Global::Ini()->IsDisableDual;
	Send.ZoneMode.IsZonePartyEnabled= Global::Ini()->IsZonePartyEnabled;
	Send.ZoneMode.IsDisableParty	= Global::Ini()->IsDisableParty;
	Send.ZoneMode.IsNoSwimming		= Global::Ini()->IsNoSwimming;
	Send.ZoneMode.IsCheckHeight		= Global::Ini()->IsCheckHeight;

	Send.ZoneMode.IsDisableSendGift				= Global::Ini()->IsDisableSendGift;
	Send.ZoneMode.AcAccountMoneyTrade	= Global::Ini()->AC_AccountMoneyTrade;
	Send.ZoneMode.AcItemAccountMoneyTrade	= Global::Ini()->AC_Item_AccountMoneyTrade;

	Send.BG_iNumTeam				= ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->m_BG_iNumTeam;
	Send.BG_iNumTeamPeople			= ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->m_BG_iNumTeamPeople;

    Global::SendToCli( Player->SockID() , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
//送角色區域ID資料
void NetSrv_CliConnect::SendPlayerItemID( RoleDataEx* Player )
{
    PG_CliConnect_LtoC_PlayerItemID   Send;
    Send.GItemID = Player->GUID();
    Send.WorldGUID = Player->WorldGUID();
	Send.DBID = Player->DBID();

    Global::SendToCli( Player->SockID() , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
//更新某人的角色資料
void NetSrv_CliConnect::SendPlayerDataInfo( RoleDataEx* Player )
{
    PG_CliConnect_LtoC_PlayerData     Send;
    Send.BaseData			= Player->BaseData;
	Send.PlayerBaseData		= *(Player->PlayerBaseData);
	Send.Base				= Player->Base;
//	Send.LearnMagicID	= RoleDataEx::GlobalSetting.LearnMagicID;
//	Send.Version		= RoleDataEx::GlobalSetting.Version;
    Global::SendToCli( Player->SockID() , sizeof( Send ) , &Send );
}
//送區域版本資料
void NetSrv_CliConnect::SendSysVersionInfo( int SockID , int LearnMagicID , SystemVersionENUM Version , float PK_DotRate )
{
	PG_CliConnect_LtoC_SysVersionInfo Send;
	Send.LearnMagicID = LearnMagicID;
	Send.Version = Version;
	Send.PK_DotRate = PK_DotRate;
	Global::SendToCli( SockID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
//通知角色準備換區
void NetSrv_CliConnect::SendPrepareChangeZone( RoleDataEx* Player )
{
    PG_CliConnect_LtoC_PrepareChangeZone  Send;
    Global::SendToCli( Player->SockID() , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
//通知角色換區OK
void NetSrv_CliConnect::SendChangeZoneOK( RoleDataEx* Player )
{
    PG_CliConnect_LtoC_ChangeZoneOK  Send;
    Global::SendToCli( Player->SockID() , sizeof( Send ) , &Send );
}

//-------------------------------------------------------------------
//L to L
//通知換區資訊 
bool NetSrv_CliConnect::SendChangeZone( RoleDataEx* Player )
{
    PG_CliConnect_LtoL_ChangeZoneNodify Send;
    
	if( Player->IsPlayer() )
	{
		Send.Role = *( PlayerRoleData* )Player;
	}
	else
	{
		Send.Role.Copy( Player );
		//memcpy( &Send.Role , Player , sizeof( RoleData ) );		
	}

    Send.Role.TempData.Sys.LGUID = Player->GUID();
    Send.Role.TempData.Sys.SecRoomID = -1;
	Send.SrcZoneID = RoleDataEx::G_ZoneID;
	Send.Role.TempData.BackInfo.LZoneID = RoleDataEx::G_ZoneID;

    //Send.Role.SaveDB_DelTime();
	RoleDataEx* Role = (RoleDataEx*)(&Send.Role);
	Role->SaveDB_DelTime();

    if( Global::SendToLocal( Player->BaseData.ZoneID , sizeof(Send) , &Send ) == false )
	{
		Player->BaseData.ZoneID = Player->BaseData.ZoneID % _DEF_ZONE_BASE_COUNT_;
		if( Player->BaseData.ZoneID == RoleDataEx::G_ZoneID )
		{
			return false;
		}
		if( Global::SendToLocal( Player->BaseData.ZoneID , sizeof(Send) , &Send ) == false )
		{
			Print( Def_PrintLV3 , "SendChangeZone" , "change zone failed!! ZoneID =%d" , Player->BaseData.ZoneID );
			return false;
		};
	}

    return true;
}
//-------------------------------------------------------------------
//通知Client 修改物件資訊
void NetSrv_CliConnect::SendModifyInfo( int SendID , RoleDataEx* ModifyObj )
{
    PG_CliConnect_LtoC_ModifyNPCInfo Send;

    Send.Info.GItemID   = ModifyObj->GUID();
    Send.Info.OrgObjID  = ModifyObj->BaseData.ItemInfo.OrgObjID;	
    Send.Info.X         = ModifyObj->SelfData.RevPos.X ;
    Send.Info.Y         = ModifyObj->SelfData.RevPos.Y ;			
    Send.Info.Z         = ModifyObj->SelfData.RevPos.Z ;			
    Send.Info.Dir       = ModifyObj->SelfData.RevPos.Dir ;			
    Send.Info.QuestID   = ModifyObj->SelfData.NpcQuestID;
    Send.Info.Mode      = ModifyObj->BaseData.Mode._Mode;        
    Send.Info.PID       = ModifyObj->SelfData.PID;         
    Send.Info.Name      = ModifyObj->RoleName();
    Send.Info.AutoPlot  = ModifyObj->SelfData.AutoPlot;
    Send.Info.ClassName = ModifyObj->SelfData.PlotClassName;
	Send.Info.vX		= ModifyObj->BaseData.vX;
	Send.Info.vY		= ModifyObj->BaseData.vY;
	Send.Info.vZ		= ModifyObj->BaseData.vZ;

    Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_CliConnect::SendChangeZoneNotifyResult( int ToZoneID , int GItemID , float X , float Y , float Z , ChangeZoneNodifyResult_ENUM Result )
{
	PG_CliConnect_LtoL_ChangeZoneNodifyResult Send;
	Send.GItemID = GItemID;
	Send.ZoneID = RoleDataEx::G_ZoneID;
	Send.X = X;
	Send.Y = Y;
	Send.Z = Z;
	Send.Result = Result;
	Global::SendToLocal( ToZoneID , sizeof(Send) , &Send );
}

//通知NPC移動資料
void NetSrv_CliConnect::SendNPCMoveInfo( int SendID , int NPCGUID ,  vector<NPC_MoveBaseStruct>& List )
{
	PG_CliConnect_LtoC_GetNPCMoveInfoResult Send;
	Send.GUID = NPCGUID;
	Send.TotalCount = (int)List.size();

	if( List.size() == 0 )
	{
		Send.IndexID = -1; 
		Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
		return;
	}

	for( unsigned i = 0 ; i < List.size() ; i++  )
	{
		Send.IndexID = i;
		Send.Base = List[i];
		Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );	
	}
}

void NetSrv_CliConnect::EditFlagResult ( int CliID, int iResult )
{
	PG_CliConnect_LtoC_EditFlagResult Packet;
	Packet.iResult = iResult;
	Global::SendToCli_ByGUID( CliID , sizeof( Packet ) , &Packet );	
}

void NetSrv_CliConnect::DelFlagResult ( int CliID, int iResult )
{
	PG_CliConnect_LtoC_DelFlagResult Packet;
	Packet.iResult = iResult;
	Global::SendToCli_ByGUID( CliID , sizeof( Packet ) , &Packet );	
}

void NetSrv_CliConnect::DelFlagGroupResult ( int CliID, int iResult )
{
	PG_CliConnect_LtoC_DelFlagGroupResult Packet;
	Packet.iResult = iResult;
	Global::SendToCli_ByGUID( CliID , sizeof( Packet ) , &Packet );	
}

void NetSrv_CliConnect::CliLogoutResult( int CliID , bool Result )
{
	PG_CliConnect_LtoC_LogoutResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( CliID , sizeof( Send ) , &Send );	
}

void NetSrv_CliConnect::S_ParalledID		( int CliID , int ParalledID )
{
	PG_CliConnect_LtoC_ParalledID Send;
	Send.ParalledID = ParalledID;
	Global::SendToCli_ByGUID( CliID , sizeof( Send ) , &Send );	
}
//更新某人的角色資料(有壓縮)
void NetSrv_CliConnect::SendPlayerDataInfo_ZIP( RoleDataEx* Player )
{
	struct
	{
		BaseRoleData			BaseData;
		PlayerBaseRoleData	 	PlayerBaseData;
		SysBaseRoleData			Base;
	}TempRoleBase;
	
	TempRoleBase.BaseData		= Player->BaseData;
	TempRoleBase.PlayerBaseData = *(Player->PlayerBaseData);
	TempRoleBase.Base			= Player->Base;

	PG_CliConnect_LtoC_PlayerData_Zip Send;

	MyLzoClass myZip;
	Send.Size = myZip.Encode( (char*)(&TempRoleBase) , sizeof( TempRoleBase ) , (char*)&Send.Data );

	Global::SendToCli_ByGUID( Player->GUID() , Send.PGSize() , &Send );	
}

void NetSrv_CliConnect::SBL_ChangeZoneToOtherWorld			( int ToWorldID , int ToGSrvID , PlayerRoleData& Role )
{
	/*
	PG_CliConnect_LtoBL_ChangeZoneToOtherWorld Send;
	Send.Role = Role;
	Send.RoleSize = sizeof(PlayerRoleData);
	Global::SendToOtherWorld_GSrvID( ToWorldID , ToGSrvID , sizeof( Send ) , &Send );
	*/

	PG_CliConnect_LtoBL_ChangeZoneToOtherWorld_Zip Send;

	Send.RoleSize = sizeof(PlayerRoleData);
	MyLzoClass myZip;
	Send.Size = myZip.Encode( (char*)(&Role) , sizeof( PlayerRoleData) , (char*)&Send.Data );

	Global::SendToOtherWorld_GSrvID( ToWorldID , ToGSrvID , Send.PGSize() , &Send );	

}
void NetSrv_CliConnect::SBL_ChangeZoneToOtherWorldResult	( int ToWorldID , int ToNetID , int RoleDBID , char* IPStr , int Port , ChangeZoneToOtherWorldResultENUM Result )
{
	PG_CliConnect_BLtoL_ChangeZoneToOtherWorldResult Send;
	Send.RoleDBID = RoleDBID % _DEF_MAX_DBID_COUNT_;
	Send.Result = Result;
	MyStrcpy( Send.IPStr , IPStr , sizeof(Send.IPStr) );
	Send.Port = Port;
	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , sizeof( Send ) , &Send );
}
void NetSrv_CliConnect::SBL_ConnectCliAndGSrv				( int ToWorldID , int ToNetID , int CliNetID  )
{
	PG_CliConnect_LtoBL_ConnectCliAndGSrv Send;
	Send.CliNetID = CliNetID;

	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , sizeof( Send ) , &Send );
}
void NetSrv_CliConnect::SBL_RetNomorlWorld					( int ToWorldID , int ToNetID , int PlayerDBID , RoleDataEx* Role , int OrgWorldGUID )
{
	if( ToWorldID == -1 && ToNetID == -1)
		return;
/*
	PG_CliConnect_BLtoL_RetNomorlWorld Send;
	Send.PlayerDBID = PlayerDBID % _DEF_MAX_DBID_COUNT_;

	Send.Role = *(PlayerRoleData*)Role;

	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , sizeof( Send ) , &Send );
	*/
	PG_CliConnect_BLtoL_RetNomorlWorld_Zip Send;
	Send.PlayerDBID = PlayerDBID % _DEF_MAX_DBID_COUNT_;
	int bkWorldGUID = Role->WorldGUID();
	Role->TempData.Sys.WorldGUID = OrgWorldGUID;
	MyLzoClass myZip;
	Send.Size = myZip.Encode( (char*)Role , sizeof( PlayerRoleData ) , (char*)&Send.Data );
	Role->TempData.Sys.WorldGUID = bkWorldGUID;
	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , Send.PGSize() , &Send );
}
void NetSrv_CliConnect::SBL_BattleSavePlayer					( int ToWorldID , int ToNetID , int PlayerDBID , RoleDataEx* Role , int OrgWorldGUID )
{
	if( ToWorldID == -1 && ToNetID == -1)
		return;
/*
	PG_CliConnect_BLtoL_BattleSavePlayer Send;
	Send.PlayerDBID = PlayerDBID % _DEF_MAX_DBID_COUNT_;

	Send.Role = *(PlayerRoleData*)Role;

	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , sizeof( Send ) , &Send );
	*/

	PG_CliConnect_BLtoL_BattleSavePlayer_Zip Send;
	Send.PlayerDBID = PlayerDBID % _DEF_MAX_DBID_COUNT_;

	int bkWorldGUID = Role->WorldGUID();
	Role->TempData.Sys.WorldGUID = OrgWorldGUID;

	MyLzoClass myZip;
	Send.Size = myZip.Encode( (char*)Role , sizeof( PlayerRoleData ) , (char*)&Send.Data );

	Role->TempData.Sys.WorldGUID = bkWorldGUID;

	Global::SendToOtherWorld_NetID( ToWorldID , ToNetID , Send.PGSize() , &Send );
}

void NetSrv_CliConnect::SL_DelTomb( int ZoneID , int TombGUID , int OwnerDBID )
{
	PG_CliConnect_LtoL_DelTomb Send;
	Send.TombGUID = TombGUID;
	Send.OwnerDBID = OwnerDBID;
	Global::SendToLocal( ZoneID , sizeof(Send) , &Send );
}

void NetSrv_CliConnect::SC_SkyProcType( int SendToID , ClientSkyProcTypeENUM Type )
{
	PG_CliConnect_LtoC_SkyProcType Send;
	Send.Type = Type;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );		
}

void NetSrv_CliConnect::SC_TouchTomb( int SendToID )
{
	PG_CliConnect_LtoC_TouchTomb Send;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );		
}
void NetSrv_CliConnect::SC_ConnectToBattleWorld				( int SendToID , char* IP , int Port )
{
	PG_CliConnect_LtoC_ConnectToBattleWorld Send;
	MyStrcpy( Send.IPStr , IP , sizeof( Send.IPStr ) );
	Send.Port = Port;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );		
}

void NetSrv_CliConnect::SC_TombInfo( int SendToID , TombStruct& Tomb )
{
	PG_CliConnect_LtoC_TombInfo Send;
	Send.CreateTime = Tomb.CreateTime;

	Send.ZoneID = Tomb.ZoneID;
	Send.X		= Tomb.X;
	Send.Y		= Tomb.Y;
	Send.Z		= Tomb.Z;
	Send.DebtExp= Tomb.DebtExp;
	Send.DebtTp	= Tomb.DebtTp;
	Send.Exp	= Tomb.Exp;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );		
}
void NetSrv_CliConnect::SC_WorldReturnNotify( int SendToID , int ZoneID )
{
	PG_CliConnect_LtoC_WorldReturnNotify Send;
	Send.ZoneID = ZoneID;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );		
}
void NetSrv_CliConnect::SBL_SendToZonePlayerByDBID( int WorldID, int ZoneID, int ClientDBID, int Size, void* Data )
{
	PG_CliConnect_BLtoBL_SendToPlayerByDBID Send;
	Send.PlayerDBID = ClientDBID;
	Send.DataSize = Size;
	memcpy( Send.Data , Data , Size );
	Global::SendToOtherWorld_GSrvID( WorldID , ZoneID , Send.PGSize() , &Send );
}

void NetSrv_CliConnect::SBL_SendToSrvPlayerByDBID( int WorldID, int SrvID, int ClientDBID, int Size, void* Data )
{
	PG_CliConnect_BLtoBL_SendToPlayerByDBID Send;
	Send.PlayerDBID = ClientDBID;
	Send.DataSize = Size;
	memcpy( Send.Data , Data , Size );
	Global::SendToOtherWorld_NetID( WorldID , SrvID , Send.PGSize() , &Send );
}
void NetSrv_CliConnect::SC_ZoneOpenState( int SendtoID , int ZoneID , bool IsOpen )
{
	PG_CliConnect_LtoC_ZoneOpenState Send;
	Send.IsOpen = IsOpen;
	Send.ZoneID = ZoneID;
	Global::SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );	
}
void NetSrv_CliConnect::SC_AllCli_ZoneOpenState( int ZoneID , bool IsOpen )
{
	PG_CliConnect_LtoC_ZoneOpenState Send;
	Send.IsOpen = IsOpen;
	Send.ZoneID = ZoneID;
	//Global::SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );	
	Global::SendToAllCli( sizeof(Send) , &Send );
}
void NetSrv_CliConnect::SL_BattleZone( int WorldID, int ZoneID , int PlayerDBID  )
{
	PG_CliConnect_BLtoL_BattleZone Send;
	Send.ZoneID = RoleDataEx::G_ZoneID;
	Send.PlayerDBID = PlayerDBID % _DEF_MAX_DBID_COUNT_;
	Global::SendToOtherWorld_GSrvID( WorldID , ZoneID , sizeof(Send) , &Send );
}
void NetSrv_CliConnect::SBL_ReturnWorldRequest( int WorldID, int ZoneID , int PlayerDBID , int TestCount )
{
	PG_CliConnect_LtoBL_ReturnWorldRequest Send;
	Send.TestCount = TestCount;
	Send.PlayerDBID = PlayerDBID;
	Global::SendToOtherWorld_GSrvID( WorldID , ZoneID , sizeof(Send) , &Send );
}
void NetSrv_CliConnect::SL_ReturnWorldResult( int WorldID, int NetID , int PlayerDBID , int TestCount , bool Result )
{
	PG_CliConnect_BLtoL_ReturnWorldResult Send;
	Send.TestCount = TestCount;
	Send.PlayerDBID = PlayerDBID % _DEF_MAX_DBID_COUNT_ ;
	Send.Result = Result;
	Global::SendToOtherWorld_NetID( WorldID , NetID , sizeof(Send) , &Send );
}
