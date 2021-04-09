#include "NetChat_FriendList.h"
#include "../../MainProc/Global.h"
//-------------------------------------------------------------------
NetSrv_FriendList*    NetSrv_FriendList::This         = NULL;
//-------------------------------------------------------------------
bool NetSrv_FriendList::_Init()
{
	_Net->RegOnSrvPacketFunction	( EM_PG_FriendList_LtoChat_FriendListInfo	, _PG_FriendList_LtoChat_FriendListInfo			);
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_FriendList::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_FriendList::_PG_FriendList_LtoChat_FriendListInfo(  int NetID , int Size , void* Data )
{
	PG_FriendList_LtoChat_FriendListInfo* PG =(PG_FriendList_LtoChat_FriendListInfo*)Data;

	This->R_FriendListInfo( PG->IsInsert , PG->PlayerDBID , PG->FriendDBID );
}

void NetSrv_FriendList::SC_PlayerOnlineInfo( int SendToDBID , int PlayerDBID , const char* RoleName  
											, Race_ENUM Race , Voc_ENUM Voc , Voc_ENUM Voc_Sub , Sex_ENUM Sex
											, int LV , int LV_Sub , int GuildID , int TitleID )
{
	//gm不處理上線通知
	if( Voc == EM_Vocation_GameMaster )
		return;
	PG_FriendList_ChattoC_PlayerOnlineInfo Send;
	Send.RoleName = RoleName;
	Send.PlayerDBID = PlayerDBID;
	Send.Race = Race;
	Send.Voc = Voc;
	Send.Voc_Sub = Voc_Sub;
	Send.Sex = Sex;
	Send.LV = LV;
	Send.LV_Sub = LV_Sub;
	Send.GuildID = GuildID;
	Send.TitleID = TitleID;

	Global::SendToCli_ByDBID( SendToDBID , sizeof( Send ) , &Send );
}

void NetSrv_FriendList::SC_PlayerOfflineInfo( int SendToDBID , int PlayerDBID )
{
	PG_FriendList_ChattoC_PlayerOfflineInfo Send;
	Send.PlayerDBID = PlayerDBID; 
	Global::SendToCli_ByDBID( SendToDBID , sizeof( Send ) , &Send );
}