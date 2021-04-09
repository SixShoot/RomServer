#pragma once

#include "NetSrv_FriendList.h"

class NetSrv_FriendListChild : public NetSrv_FriendList
{

public:
    static bool Init();
    static bool Release();

	virtual void RC_InsertFriend( BaseItemObject* Obj , FriendListTypeENUM Type , int Pos , BaseFriendStruct& Info );
	virtual void RC_EraseFriend( BaseItemObject* Obj , FriendListTypeENUM Type , int DBID );
	virtual void RC_ModifyFriend( BaseItemObject* Obj , FriendListTypeENUM Type , BaseFriendStruct& Info ) ;
	virtual void RC_ModifyNote( BaseItemObject* Obj , char* Note );
	virtual void RC_FixClientData( BaseItemObject* Obj , int Pos , char* CliData ); 	
	virtual void RC_FixGroupSortID( BaseItemObject* Obj , char GroupSortID[ _MAX_FRIENDLIST_GROUP_COUNT_ ] );
	virtual void RC_SearchFindPartyPlayerRequest( BaseItemObject* OwnerObj , Voc_ENUM MainJob , Voc_ENUM SubJob , int MinMainJobLv , int MaxMainJobLv , int MinSubJobLv , int MaxSubJobLv );
};

