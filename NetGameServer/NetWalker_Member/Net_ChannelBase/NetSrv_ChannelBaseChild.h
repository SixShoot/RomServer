#ifndef __NETWAKER_CHANNELBASECHILD_H__
#define __NETWAKER_CHANNELBASECHILD_H__

#include "NetSrv_ChannelBase.h"

class NetSrv_ChannelBaseChild : public NetSrv_ChannelBase
{
public:
    static bool Init();
    static bool Release();

    //-------------------------------------------------------------------
    //  事件繼承實做(chat)
    //-------------------------------------------------------------------
    virtual void CreateChannelOK     ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* ChannelName);
    virtual void CreateChannelFalse  ( GroupObjectTypeEnum Type , int UserDBID  , char* ChannelName);
    virtual void AddUserOK           ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* ChannelName , int ChannelOwnerDBID , char* ChannelOwnerName );
    virtual void AddUserFalse        ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* ChannelName );
    virtual void DelUserOK           ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID  );
    virtual void DelUserFalse        ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID  );
    virtual void OnChatConnect       ( EM_SERVER_TYPE ServerType , DWORD ZoneID  );
	virtual void OnChatDisconnect    ( EM_SERVER_TYPE ServerType , DWORD ZoneID  );

	//線上玩家網路資料處理
//	virtual void RChat_AddorSetOnlinePlayer( OnlinePlayerInfoStruct& Info );
//	virtual void RChat_DelOnlinePlayer( char* Name , int DBID );

	virtual void OnSystemTimeNotify	 ( int Time );
	//-------------------------------------------------------------------
	//  事件繼承實做(client)
	//-------------------------------------------------------------------
	virtual void R_CreateChannel( BaseItemObject* Player  , char* ChannelName , char* Password );
	virtual void R_JoinChannel( BaseItemObject* Player  , char* ChannelName , char* Password , bool IsForceJoin );
	virtual void R_LeaveChannel( BaseItemObject* Player  , int ChannelID );

};

#endif //__NETWAKER_CHANNELBASECHILD_H__