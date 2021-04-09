#pragma warning (disable:4996)

#include "Net_OnlinePlayerChild.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
#ifdef _LOCAL_SERVER_
	#include "../../MainProc/GuildManage/GuildManage.h"
#endif
#ifdef _DATACENTER_SERVER_
	#include "../../MainProc/GuildManage/GuildManage.h"
	#include "../Net_Guild/NetDC_Guild.h"
#endif

#ifdef _CHAT_SERVER_
	#include "../Net_FriendList/NetChat_FriendListChild.h"
#endif

bool	NetSrv_OnlinePlayerChild::IsChatReady = false;
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_OnlinePlayerChild::Init()
{
    NetSrv_OnlinePlayer::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_OnlinePlayerChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_OnlinePlayerChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_OnlinePlayer::_Release();

    delete This;
    This = NULL;

    return true;
    
}
void NetSrv_OnlinePlayerChild::RChat_AddorSetOnlinePlayer( OnlinePlayerInfoStruct& Info )
{
	Print( LV1 , "RChat_AddorSetOnlinePlayer" , "RoleName=%s" , Utf8ToChar( Info.Name.Begin() ).c_str() ) ;
#ifdef _DATACENTER_SERVER_
	if( AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( Info.DBID) != NULL )
	{
		//通知所有公會成員
		CNetDC_Guild::SC_ALLMember_PlayerOnline( Info.DBID , true );
	}
#endif

#ifdef _CHAT_SERVER_
	bool IsFirstTime = false;
	if( AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( Info.DBID) == NULL )
	{
		IsFirstTime = true;
	}
#endif

	AllOnlinePlayerInfoClass::This()->AddorSetPlayer( Info );

#ifdef _CHAT_SERVER_
	if( IsFirstTime )
	{
		//通知所有公會成員
		NetSrv_FriendListChild::PlayerOnline( Info );
	}
#endif



#ifdef _LOCAL_SERVER_
	GuildManageClass::This()->SetMemberOnline( Info.DBID , &Info );
#endif
#ifdef _DATACENTER_SERVER_
	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( Info.GuildID );
	if( GuildInfo != NULL )
	{
		if( Info.DBID == GuildInfo->Base.LeaderDBID )
		{
			if( stricmp( GuildInfo->Base.LeaderName.Begin() , Info.Name.Begin() ) != 0 )
			{
				GuildInfo->Base.LeaderName = Info.Name;
				GuildInfo->Base.IsNeedSave = true;
				CNetDC_Guild::SL_All_ModifyGuild( GuildInfo->Base );
				CNetDC_Guild::SC_AllMember_GuildBaseInfoUpdate( GuildInfo->Base );
			}
		}
	}

	GuildManageClass::This()->SetMemberOnline( Info.DBID , &Info);
#endif

	
}
void NetSrv_OnlinePlayerChild::RChat_DelOnlinePlayer( char* Name , int DBID )
{
	Print( LV1 , "RChat_DelOnlinePlayer" , "RoleName=%s" , Utf8ToChar( Name ).c_str()  );

	if( AllOnlinePlayerInfoClass::This()->DelPlayer( DBID ) == false )
	{
		Print( LV2 , "RChat_DelOnlinePlayer" , "RoleName=%s Erase Error" ,  Utf8ToChar( Name ).c_str()  );
	}
#ifdef _LOCAL_SERVER_
	GuildManageClass::This()->SetMemberOffline( DBID );
#endif
#ifdef _DATACENTER_SERVER_
	CNetDC_Guild::SC_ALLMember_PlayerOnline( DBID , false );
#endif
}

void NetSrv_OnlinePlayerChild::OnChatDisconnect( )
{
	IsChatReady = false;
	AllOnlinePlayerInfoClass::This()->Clear();
}
void NetSrv_OnlinePlayerChild::OnChatConnect( )
{
	IsChatReady = true;
}