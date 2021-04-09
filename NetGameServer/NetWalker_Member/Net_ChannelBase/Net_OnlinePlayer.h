#pragma once

#include "PG/PG_ChannelBase.h"
#include "../../MainProc/Global.h"

class NetSrv_OnlinePlayer : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_OnlinePlayer* This;
    static bool _Init();
    static bool _Release();

	static void _PG_ChannelBase_ChattoL_AddorSetOnlinePlayer( int Sockid , int Size , void* Data );
	static void _PG_ChannelBase_ChattoL_DelOnlinePlayer		( int Sockid , int Size , void* Data );

	static void	_OnChatConnectEvent    ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	static void	_OnChatDisconnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );

public:
	//線上玩家網路資料處理
	virtual void RChat_AddorSetOnlinePlayer( OnlinePlayerInfoStruct& Info ) = 0;
	virtual void RChat_DelOnlinePlayer( char* Name , int DBID ) = 0;
	virtual void OnChatDisconnect( ) = 0;
	virtual void OnChatConnect( ) = 0;
};

