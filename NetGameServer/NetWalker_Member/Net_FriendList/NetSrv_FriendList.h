#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_FriendList.h"

class NetSrv_FriendList : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_FriendList* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------

	static void	_PG_FriendList_CtoL_InsertFriend		( int sockid , int size , void* data );	
	static void	_PG_FriendList_CtoL_EraseFriend			( int sockid , int size , void* data );
	static void	_PG_FriendList_CtoL_ModifyFriend		( int sockid , int size , void* data );	
	static void	_PG_FriendList_CtoL_ModifyNote			( int sockid , int size , void* data );	
	static void	_PG_FriendList_CtoL_FixClientData		( int sockid , int size , void* data );	
	static void	_PG_FriendList_CtoL_FixGroupSortID		( int sockid , int size , void* data );	
	static void _PG_Friend_CtoL_SearchFindPartyPlayerRequest	( int NetID , int Size , void* Data );	
public:    
	static void SC_InsertFriendResult ( int SendToID , FriendListTypeENUM Type , int Pos , bool Result );
	static void SChat_FriendListInfo  ( int PlayerDBID , int FriendDBID , bool IsInsert );
	static void SC_EraseFriendResult  ( int SendToID , FriendListTypeENUM Type , int DBID , bool Result );
	static void SC_ModifyFriendResult ( int SendToID , FriendListTypeENUM Type , int DBID , bool Result );
	static void SC_FriendListInfo_Zip ( int SendToID , FriendListStruct& FriendList );
	static void SC_FixFriendInfo	  ( int SendToID , FriendListTypeENUM Type , int Pos , BaseFriendStruct& Info );
	static void S_SearchFindPartyPlayerResult(	  int SendToID , int Count , int Index , SearchFindPartyPlayerStruct& Info );

	virtual void RC_InsertFriend( BaseItemObject* Obj , FriendListTypeENUM Type , int Pos , BaseFriendStruct& Info ) = 0;
	virtual void RC_EraseFriend( BaseItemObject* Obj , FriendListTypeENUM Type , int DBID ) = 0;
	virtual void RC_ModifyFriend( BaseItemObject* Obj , FriendListTypeENUM Type , BaseFriendStruct& Info ) = 0;
	virtual void RC_ModifyNote( BaseItemObject* Obj , char* Note ) = 0;
	virtual void RC_FixClientData( BaseItemObject* Obj , int Pos , char* CliData ) = 0; 	
	virtual void RC_FixGroupSortID( BaseItemObject* Obj , char GroupSortID[ _MAX_FRIENDLIST_GROUP_COUNT_ ] ) = 0;
	virtual void RC_SearchFindPartyPlayerRequest( BaseItemObject* OwnerObj , Voc_ENUM MainJob , Voc_ENUM SubJob , int MinMainJobLv , int MaxMainJobLv , int MinSubJobLv , int MaxSubJobLv ) = 0;
};

