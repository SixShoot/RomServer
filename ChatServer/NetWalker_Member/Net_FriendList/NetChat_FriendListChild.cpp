#include "../NetWakerChatInc.h"
#include "NetChat_FriendListChild.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
//-----------------------------------------------------------------
map< int , set<int> >	NetSrv_FriendListChild::_FriendToMeLink;	
map< int , set<int> >	NetSrv_FriendListChild::_MeToFriendLink;
//-----------------------------------------------------------------
bool    NetSrv_FriendListChild::Init()
{
    NetSrv_FriendList::_Init();

    if( This != NULL)
        return false;

    This = new NetSrv_FriendListChild;

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_FriendListChild::Release()
{
    if( This == NULL )
        return false;

    delete This;
    This = NULL;

    NetSrv_FriendList::_Release();
    return true;
    
}

void NetSrv_FriendListChild::R_FriendListInfo( bool IsInsert , int PlayerDBID , int FriendDBID  )
{
	if( IsInsert )
	{	
		_FriendToMeLink[ FriendDBID ].insert( PlayerDBID );
		_MeToFriendLink[ PlayerDBID ].insert( FriendDBID );

		OnlinePlayerInfoStruct* FriendInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( FriendDBID );

		if( FriendInfo != NULL )
		{
			SC_PlayerOnlineInfo( PlayerDBID , FriendDBID , (char*)FriendInfo->Name.Begin()  , FriendInfo->Race , FriendInfo->Voc , FriendInfo->Voc_Sub , FriendInfo->Sex
				, FriendInfo->LV , FriendInfo->LV_Sub , FriendInfo->GuildID , FriendInfo->TitleID);
		}

		//คWญญ
	}
	else
	{
		_FriendToMeLink[ FriendDBID ].erase( PlayerDBID );
		_MeToFriendLink[ PlayerDBID ].erase( FriendDBID );
	}
}

void NetSrv_FriendListChild::PlayerOnline( OnlinePlayerInfoStruct& Info )
{
	set<int>& FriendToMeSet = _FriendToMeLink[Info.DBID];
	set<int>::iterator Iter;

	for( Iter = FriendToMeSet.begin() ; Iter != FriendToMeSet.end() ; Iter++ )
	{
		SC_PlayerOnlineInfo( *Iter , Info.DBID , (char*)Info.Name.Begin() , Info.Race , Info.Voc , Info.Voc_Sub , Info.Sex
			, Info.LV , Info.LV_Sub , Info.GuildID , Info.TitleID );
	}


	set<int>& MeToFriendSet = _MeToFriendLink[Info.DBID];
	for( Iter = MeToFriendSet.begin() ; Iter != MeToFriendSet.end() ; Iter++ )
	{
		OnlinePlayerInfoStruct* FriendInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( *Iter );
		
		if( FriendInfo == NULL )
			continue;

		SC_PlayerOnlineInfo( Info.DBID , *Iter , (char*)FriendInfo->Name.Begin()  , FriendInfo->Race , FriendInfo->Voc , FriendInfo->Voc_Sub , FriendInfo->Sex
			, FriendInfo->LV , FriendInfo->LV_Sub , FriendInfo->GuildID , FriendInfo->TitleID);
	}
}
void NetSrv_FriendListChild::PlayerOffline( int DBID )
{
	set<int>& FriendToMeLink = _FriendToMeLink[DBID];
	set<int>::iterator Iter;

	for( Iter = FriendToMeLink.begin() ; Iter != FriendToMeLink.end() ; Iter++ )
	{
		SC_PlayerOfflineInfo( *Iter , DBID );
	}

	_MeToFriendLink[ DBID ].clear();
}