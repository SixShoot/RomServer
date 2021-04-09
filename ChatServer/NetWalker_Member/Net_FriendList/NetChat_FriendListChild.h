#pragma once
#include "NetChat_FriendList.h"


class NetSrv_FriendListChild : public NetSrv_FriendList
{
	//裡面 set 內的所有DBID 都是 線上玩家
	//註冊當某角色上線要通知的人
	static map< int , set<int> >	_FriendToMeLink;	

	//Key 內的所有DBID 都是 線上玩家
	//好友清單
	static map< int , set<int> >	_MeToFriendLink;
public:
    static bool Init();
    static bool Release();

	virtual void R_FriendListInfo( bool IsInsert , int PlayerDBID , int FriendDBID  );

	static void PlayerOnline( OnlinePlayerInfoStruct& Info );
	static void PlayerOffline( int DBID );
};

