#include "../NetWakerGSrvInc.h"
#include "NetSrv_FriendList.h"
//-------------------------------------------------------------------
NetSrv_FriendList*    NetSrv_FriendList::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_FriendList::_Init()
{
	Srv_NetCliReg( PG_FriendList_CtoL_InsertFriend 	);
	Srv_NetCliReg( PG_FriendList_CtoL_EraseFriend  	);
	Srv_NetCliReg( PG_FriendList_CtoL_ModifyFriend 	);
	Srv_NetCliReg( PG_FriendList_CtoL_ModifyNote   	);
	Srv_NetCliReg( PG_FriendList_CtoL_FixClientData );
	Srv_NetCliReg( PG_FriendList_CtoL_FixGroupSortID );
	Srv_NetCliReg( PG_Friend_CtoL_SearchFindPartyPlayerRequest );

    return true;
}
//-------------------------------------------------------------------
bool NetSrv_FriendList::_Release()
{
    return true;
}

void NetSrv_FriendList::_PG_Friend_CtoL_SearchFindPartyPlayerRequest	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Friend_CtoL_SearchFindPartyPlayerRequest* pg =(PG_Friend_CtoL_SearchFindPartyPlayerRequest*)Data;
	This->RC_SearchFindPartyPlayerRequest( Obj , pg->MainJob , pg->SubJob , pg->MinMainJobLv , pg->MaxMainJobLv , pg->MinSubJobLv , pg->MaxSubJobLv );
}


void NetSrv_FriendList::_PG_FriendList_CtoL_FixGroupSortID( int sockid , int size , void* data )	
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_FriendList_CtoL_FixGroupSortID* PG = (PG_FriendList_CtoL_FixGroupSortID*)data;
	This->RC_FixGroupSortID( Obj , PG->GroupSortID );
}

void NetSrv_FriendList::_PG_FriendList_CtoL_FixClientData( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_FriendList_CtoL_FixClientData* PG = (PG_FriendList_CtoL_FixClientData*)data;
	This->RC_FixClientData( Obj , PG->Pos , PG->CliData );
}

void	NetSrv_FriendList::_PG_FriendList_CtoL_ModifyNote		( int sockid , int size , void* data )	
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_FriendList_CtoL_ModifyNote* PG = (PG_FriendList_CtoL_ModifyNote*)data;
	This->RC_ModifyNote( Obj , (char*)PG->Note.Begin() );
}

void	NetSrv_FriendList::_PG_FriendList_CtoL_InsertFriend		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_FriendList_CtoL_InsertFriend* PG = (PG_FriendList_CtoL_InsertFriend*)data;
	This->RC_InsertFriend( Obj , PG->Type , PG->Pos , PG->Info );
}
void	NetSrv_FriendList::_PG_FriendList_CtoL_EraseFriend		( int sockid , int size , void* data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_FriendList_CtoL_EraseFriend* PG = (PG_FriendList_CtoL_EraseFriend*)data;
	This->RC_EraseFriend( Obj , PG->Type , PG->DBID );
}
void	NetSrv_FriendList::_PG_FriendList_CtoL_ModifyFriend		( int sockid , int size , void* data )	
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( sockid );
	if( Obj == NULL )
		return ;

	PG_FriendList_CtoL_ModifyFriend* PG = (PG_FriendList_CtoL_ModifyFriend*)data;
	This->RC_ModifyFriend( Obj , PG->Type , PG->Info );
}

void NetSrv_FriendList::SC_InsertFriendResult( int SendToID , FriendListTypeENUM Type , int Pos , bool Result )
{
	PG_FriendList_LtoC_InsertFriendResult Send;
	Send.Type = Type;
	Send.Pos = Pos;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
void NetSrv_FriendList::SChat_FriendListInfo( int PlayerDBID , int FriendDBID , bool IsInsert )
{
	PG_FriendList_LtoChat_FriendListInfo Send;
	Send.PlayerDBID = PlayerDBID;
	Send.FriendDBID = FriendDBID;
	Send.IsInsert = IsInsert;			// true:Insert  false:Eraset;
	Global::SendToChat( sizeof(Send) , &Send );
}
void NetSrv_FriendList::SC_EraseFriendResult( int SendToID , FriendListTypeENUM Type , int DBID , bool Result )
{
	PG_FriendList_LtoC_EraseFriendResult Send;
	Send.Type = Type;
	Send.DBID = DBID;
	Send.Result = Result;

	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}
void NetSrv_FriendList::SC_ModifyFriendResult( int SendToID , FriendListTypeENUM Type , int DBID , bool Result )
{
	PG_FriendList_LtoC_ModifyFriendResult Send;
	Send.Type = Type;
	Send.DBID = DBID;
	Send.Result = Result;

	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );
}

void NetSrv_FriendList::SC_FriendListInfo_Zip ( int SendToID , FriendListStruct& FriendList )
{
	PG_FriendList_LtoC_FriendListInfo_Zip Send;

	MyLzoClass myZip;
	Send.Size = myZip.Encode( (char*)(&FriendList) , sizeof( FriendListStruct ) , (char*)&Send.Data );

	Global::SendToCli_ByGUID( SendToID , Send.PGSize() , &Send );	
}

void NetSrv_FriendList::SC_FixFriendInfo	  ( int SendToID , FriendListTypeENUM Type , int Pos , BaseFriendStruct& Info )
{
	PG_FriendList_LtoC_FixFriendInfo Send;
	Send.Type = Type;
	Send.Pos = Pos;
	Send.Info = Info;
	Global::SendToCli_ByGUID( SendToID , sizeof(Send) , &Send );	
}

void NetSrv_FriendList::S_SearchFindPartyPlayerResult(	int SendToID , int Count , int Index , SearchFindPartyPlayerStruct& Info )
{
	PG_Friend_LtoC_SearchFindPartyPlayerResult Send;
	Send.Count = Count;
	Send.Index = Index;
	Send.Info = Info;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
