#pragma once

#include "Net_OnlinePlayer.h"

class NetSrv_OnlinePlayerChild : public NetSrv_OnlinePlayer
{
public:
	static bool IsChatReady;
    static bool Init();
    static bool Release();
	//-------------------------------------------------------------------
	//�u�W���a������ƳB�z
	virtual void RChat_AddorSetOnlinePlayer( OnlinePlayerInfoStruct& Info );
	virtual void RChat_DelOnlinePlayer( char* Name , int DBID );
	virtual void OnChatDisconnect( );
	virtual void OnChatConnect( );
};

