#include "Net_OnlinePlayer.h"
#include "../../MainProc/Global.h"
#include "../net_serverlist/Net_ServerList_Child.h"
//-------------------------------------------------------------------
NetSrv_OnlinePlayer*    NetSrv_OnlinePlayer::This         = NULL;
//-------------------------------------------------------------------
bool NetSrv_OnlinePlayer::_Init()
{
	_Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_ChattoL_AddorSetOnlinePlayer, _PG_ChannelBase_ChattoL_AddorSetOnlinePlayer  );
	_Net->RegOnSrvPacketFunction	( EM_PG_ChannelBase_ChattoL_DelOnlinePlayer     , _PG_ChannelBase_ChattoL_DelOnlinePlayer       );

	Global::Net_ServerList->RegServerLogoutEvent( EM_SERVER_TYPE_CHAT , _OnChatDisconnectEvent );
	Global::Net_ServerList->RegServerLoginEvent( EM_SERVER_TYPE_CHAT , _OnChatConnectEvent );

    return true;
}
//-------------------------------------------------------------------
bool NetSrv_OnlinePlayer::_Release()
{
    return true;
}

//-------------------------------------------------------------------
void NetSrv_OnlinePlayer::_OnChatConnectEvent    ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	This->OnChatConnect( );
}
void NetSrv_OnlinePlayer::_OnChatDisconnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
	This->OnChatDisconnect( );
}
void NetSrv_OnlinePlayer::_PG_ChannelBase_ChattoL_AddorSetOnlinePlayer	( int Sockid , int Size , void* Data )
{
	PG_ChannelBase_ChattoL_AddorSetOnlinePlayer* PG = ( PG_ChannelBase_ChattoL_AddorSetOnlinePlayer* )Data;
	This->RChat_AddorSetOnlinePlayer( PG->Info );
}
void NetSrv_OnlinePlayer::_PG_ChannelBase_ChattoL_DelOnlinePlayer	( int Sockid , int Size , void* Data )
{
	PG_ChannelBase_ChattoL_DelOnlinePlayer* PG = ( PG_ChannelBase_ChattoL_DelOnlinePlayer* )Data;
	This->RChat_DelOnlinePlayer( (char*)PG->Name.Begin() , PG->DBID );
}
