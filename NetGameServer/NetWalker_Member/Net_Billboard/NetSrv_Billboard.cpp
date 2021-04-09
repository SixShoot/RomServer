#include "../NetWakerGSrvInc.h"
#include "NetSrv_Billboard.h"

//-------------------------------------------------------------------
NetSrv_Billboard* NetSrv_Billboard::This = NULL;
//-------------------------------------------------------------------
bool NetSrv_Billboard::_Init()
{
	Srv_NetCliReg( PG_Billboard_CtoL_CloseBillboardRequest			);
	Srv_NetCliReg( PG_Billboard_CtoL_RankInfoRequest				);
	Srv_NetCliReg( PG_Billboard_CtoL_SetAnonymous					);

	Srv_NetCliReg( PG_NewBillboard_CtoL_SearchRank					);
	Srv_NetCliReg( PG_NewBillboard_CtoL_SearchMyRank				);
	Srv_NetCliReg( PG_NewBillboard_CtoL_RegisterNote				);
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Billboard::_Release()
{
    return true;
}
void NetSrv_Billboard::_PG_NewBillboard_CtoL_RegisterNote	( int Sockid , int Size , void* Data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_NewBillboard_CtoL_RegisterNote* PG = (PG_NewBillboard_CtoL_RegisterNote*)Data;
	This->RC_New_RegisterNote( Player , PG->SortType , PG->Note.Begin() );
}
void NetSrv_Billboard::_PG_NewBillboard_CtoL_SearchRank( int Sockid , int Size , void* Data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_NewBillboard_CtoL_SearchRank* PG = (PG_NewBillboard_CtoL_SearchRank*)Data;
	This->RC_New_SearchRank( Player , PG->SortType , PG->MinRank , PG->MaxRank );
}
void NetSrv_Billboard::_PG_NewBillboard_CtoL_SearchMyRank( int Sockid , int Size , void* Data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_NewBillboard_CtoL_SearchMyRank* PG = (PG_NewBillboard_CtoL_SearchMyRank*)Data;
	This->RC_New_SearchMyRank( Player , PG->SortType );
}

void NetSrv_Billboard::_PG_Billboard_CtoL_SetAnonymous			 ( int Sockid , int Size , void* Data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_Billboard_CtoL_SetAnonymous* PG = (PG_Billboard_CtoL_SetAnonymous*)Data;
	This->R_SetAnonymous( Player , PG->IsAnonymous );
}

void NetSrv_Billboard::_PG_Billboard_CtoL_CloseBillboardRequest ( int Sockid , int Size , void* Data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_Billboard_CtoL_CloseBillboardRequest* PG = (PG_Billboard_CtoL_CloseBillboardRequest*)Data;
	This->R_CloseBillboardRequest( Player );
}

void NetSrv_Billboard::_PG_Billboard_CtoL_RankInfoRequest		 ( int Sockid , int Size , void* Data )
{
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );
	if( Player == NULL )
		return;

	PG_Billboard_CtoL_RankInfoRequest* PG = (PG_Billboard_CtoL_RankInfoRequest*)Data;
	This->R_RankInfoRequest( Player , PG->Type , PG->RankBegin );
}


void NetSrv_Billboard::SC_OpenBillboard		( int SenderID , int TargetNpcID )
{
	PG_Billboard_LtoC_OpenBillboard Send;
	Send.TargetNpcID = TargetNpcID;
	Global::SendToCli_ByGUID( SenderID , sizeof( Send ) , &Send );
}
void NetSrv_Billboard::SC_CloseBillboard	( int SenderID )
{
	PG_Billboard_LtoC_CloseBillboard Send;
	Global::SendToCli_ByGUID( SenderID , sizeof( Send ) , &Send );
}

void NetSrv_Billboard::SD_RankInfoRequest	( BillBoardMainTypeENUM	Type , int RankBegin , int CliDBID , int CliSockID , int CliProxyID )
{
	PG_Billboard_LtoD_RankInfoRequest Send;
	Send.Type = Type;
	Send.RankBegin = RankBegin;
	Send.CliDBID = CliDBID;
	Send.CliSockID = CliSockID;
	Send.CliProxyID = CliProxyID;

	Global::SendToDBCenter( sizeof(Send) , &Send );
}

void NetSrv_Billboard::SD_SetAnonymous		( int CliDBID , bool IsAnonymous )
{
	PG_Billboard_LtoD_SetAnonymous Send;
	Send.DBID = CliDBID;
	Send.IsAnonymous = IsAnonymous;

	Global::SendToDBCenter( sizeof(Send) , &Send );
}


void NetSrv_Billboard::SD_New_SearchRank	( int PlayerDBID , int SortType , int MinRank , int MaxRank )
{
	PG_NewBillboard_LtoD_SearchRank Send;
	Send.PlayerDBID = PlayerDBID;
	Send.SortType = SortType;
	Send.MinRank = MinRank;
	Send.MaxRank = MaxRank;
	Global::SendToDBCenter( sizeof(Send) , &Send );
}
void NetSrv_Billboard::SD_New_SearchMyRank	( int PlayerDBID , int SortType )
{
	PG_NewBillboard_LtoD_SearchMyRank Send;
	Send.PlayerDBID = PlayerDBID;
	Send.SortType = SortType;
	Global::SendToDBCenter( sizeof(Send) , &Send );
}
void NetSrv_Billboard::SD_New_Register_TopUpdate	( int PlayerDBID , int SortType , int Value )
{
	PG_NewBillboard_LtoD_Register Send;
	Send.PlayerDBID = PlayerDBID;
	Send.SortType = SortType;
	Send.SortValue = Value;
	Send.IsAdd = false;
	Send.IsTopUpdate = true;
	Global::SendToDBCenter( sizeof(Send) , &Send );
}
void NetSrv_Billboard::SD_New_Register_Update	( int PlayerDBID , int SortType , int Value )
{
	PG_NewBillboard_LtoD_Register Send;
	Send.PlayerDBID = PlayerDBID;
	Send.SortType = SortType;
	Send.SortValue = Value;
	Send.IsAdd = false;
	Send.IsTopUpdate = false;
	Global::SendToDBCenter( sizeof(Send) , &Send );
}
void NetSrv_Billboard::SD_New_Register_Add		( int PlayerDBID , int SortType , int Value )
{
	PG_NewBillboard_LtoD_Register Send;
	Send.PlayerDBID = PlayerDBID;
	Send.SortType = SortType;
	Send.SortValue = Value;
	Send.IsAdd = true;
	Send.IsTopUpdate = false;
	Global::SendToDBCenter( sizeof(Send) , &Send );
}

void NetSrv_Billboard::SD_New_RegisterNote		( int PlayerDBID , int SortType , const char* Note )
{
	PG_NewBillboard_LtoD_RegisterNote Send;
	Send.PlayerDBID = PlayerDBID;
	Send.SortType = SortType;
	Send.Note = Note;
	Global::SendToDBCenter( sizeof(Send) , &Send );	
}