#pragma once
#include "NetChat_FriendList.h"


class NetSrv_FriendListChild : public NetSrv_FriendList
{
	//�̭� set �����Ҧ�DBID ���O �u�W���a
	//���U��Y����W�u�n�q�����H
	static map< int , set<int> >	_FriendToMeLink;	

	//Key �����Ҧ�DBID ���O �u�W���a
	//�n�ͲM��
	static map< int , set<int> >	_MeToFriendLink;
public:
    static bool Init();
    static bool Release();

	virtual void R_FriendListInfo( bool IsInsert , int PlayerDBID , int FriendDBID  );

	static void PlayerOnline( OnlinePlayerInfoStruct& Info );
	static void PlayerOffline( int DBID );
};

