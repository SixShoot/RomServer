#ifndef __NETWAKER_TALKCHILD_H__
#define __NETWAKER_TALKCHILD_H__

#include "NetSrv_Talk.h"

class NetSrv_TalkChild : public NetSrv_Talk
{
public:
    static bool Init();
    static bool Release();


    virtual void Player_TalkTo      ( BaseItemObject* Seneder , const char* TargetName , int ContentSize , const char* Content , TalkSenderInfo& SenderInfo );
    virtual void Player_Range       ( BaseItemObject* Sender , int ContentSize , const char* Content , TalkSenderInfo& SenderInfo );
//    virtual void Player_Zone        ( BaseItemObject* Sender , int ContentSize , const char* Content );
    virtual void Player_GMCommand   ( BaseItemObject* Sender , int ContentSize , const char* Content );
    virtual void ToGMTools          ( BaseItemObject* Sender , const char* Content );

	virtual void R_Guild			( BaseItemObject* Sender , int ContentSize , const char* Content , TalkSenderInfo& SenderInfo );
	virtual void R_SysChannel		( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID , int ContentSize , char* Content , TalkSenderInfo& SenderInfo );
	virtual void RD_OfflineGMCommand( int PlayerDBID , ManagementENUM ManageLv , const char* Content );
};

#endif //__NETWAKER_TALKCHILD_H__