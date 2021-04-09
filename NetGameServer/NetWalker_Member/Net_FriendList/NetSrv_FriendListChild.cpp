#include "../NetWakerGSrvInc.h"
#include "NetSrv_FriendListChild.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_FriendListChild::Init()
{
    NetSrv_FriendList::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_FriendListChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_FriendListChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_FriendList::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
void NetSrv_FriendListChild::RC_InsertFriend( BaseItemObject* Obj , FriendListTypeENUM Type , int Pos , BaseFriendStruct& Info )
{
	RoleDataEx* Owner = Obj->Role();
	Info.Lv = 0;
	switch( Type )
	{
	case EM_FriendListType_Friend:
		Info.Relation = EM_RelationType_Friend;
		break;
	case EM_FriendListType_HateFriend:
		Info.Relation = EM_RelationType_Enemy;
		break;
	case EM_FriendListType_FamilyFriend:
		SC_InsertFriendResult( Obj->GUID() , Type , Pos , false );
		return;
	}

	FriendListInsertResultENUM Result = Owner->PlayerSelfData->FriendList.Insert( Type , Pos , Info );

	switch( Result )
	{
	case EM_FriendListInsertResult_OK:
		SChat_FriendListInfo( Owner->DBID() , Info.DBID , true );
		break;
	case EM_FriendListInsertResult_Modify:
		break;
	case EM_FriendListInsertResult_Failed:
		SC_InsertFriendResult( Obj->GUID() , Type , Pos , false );
		break;
	}
	SC_InsertFriendResult( Obj->GUID() , Type , Pos , true );
}
void NetSrv_FriendListChild::RC_EraseFriend( BaseItemObject* Obj , FriendListTypeENUM Type , int DBID )
{
	RoleDataEx* Owner = Obj->Role();

	bool Result = Owner->PlayerSelfData->FriendList.Erase( Type , DBID );

	if( Result != false )
		SChat_FriendListInfo( Owner->DBID() , DBID , false );

	SC_EraseFriendResult( Owner->GUID() , Type , DBID , Result );
}
void NetSrv_FriendListChild::RC_ModifyFriend( BaseItemObject* Obj , FriendListTypeENUM Type  , BaseFriendStruct& Info )
{
	RoleDataEx* Owner = Obj->Role();

	Info.Lv = 0;
	switch( Type )
	{
	case EM_FriendListType_Friend:
		Info.Relation = EM_RelationType_Friend;
		break;
	case EM_FriendListType_HateFriend:
		Info.Relation = EM_RelationType_Enemy;
		break;
	case EM_FriendListType_FamilyFriend:
		SC_ModifyFriendResult( Owner->GUID() , Type , Info.DBID , false );
		return;
	}

	bool Result = Owner->PlayerSelfData->FriendList.Modify( Type , Info );

	SC_ModifyFriendResult( Owner->GUID() , Type , Info.DBID , Result );
}

void NetSrv_FriendListChild::RC_ModifyNote( BaseItemObject* Obj , char* Note )
{
	Obj->Role()->PlayerSelfData->FriendList.Note = Note;
}

void NetSrv_FriendListChild::RC_FixClientData( BaseItemObject* Obj , int Pos , char* CliData )
{
	if( (unsigned)Pos >=  _MAX_BAD_FRIENDLIST_COUNT_ + _MAX_FRIENDLIST_GROUP_COUNT_  )
		return;
	memcpy( Obj->Role()->PlayerSelfData->FriendList.ClientData[Pos] , CliData , _MAX_NAMERECORD_SIZE_  );
}

void NetSrv_FriendListChild::RC_FixGroupSortID( BaseItemObject* Obj , char GroupSortID[ _MAX_FRIENDLIST_GROUP_COUNT_ ] )
{
	memcpy( Obj->Role()->PlayerSelfData->FriendList.GroupSortID , GroupSortID , _MAX_FRIENDLIST_GROUP_COUNT_  );
}

void NetSrv_FriendListChild::RC_SearchFindPartyPlayerRequest( BaseItemObject*	OwnerObj , Voc_ENUM MainJob , Voc_ENUM SubJob , int MinMainJobLv , int MaxMainJobLv , int MinSubJobLv , int MaxSubJobLv )
{
	RoleDataEx* Owner = OwnerObj->Role();
	map< int , OnlinePlayerInfoStruct* >& AllInfo = *(AllOnlinePlayerInfoClass::This()->GUIDMap());
	map< int , OnlinePlayerInfoStruct* >::iterator Iter;

	vector< OnlinePlayerInfoStruct* > ResultList;

	for( Iter = AllInfo.begin() ; Iter != AllInfo.end() ; Iter ++ )
	{		
		OnlinePlayerInfoStruct* PlayerInfo = Iter->second;
		if( PlayerInfo->State.FindParty == false )
			continue;

		if( PlayerInfo->State.HasParty != false )
			continue;

		//條件檢查 2007/11/13 不同分流可以彼此搜尋到
		//if( PlayerInfo->ZoneID / _DEF_ZONE_BASE_COUNT_ != Owner->G_ZoneID / _DEF_ZONE_BASE_COUNT_ )
		//	continue;

		if( MainJob != EM_Vocation_None && PlayerInfo->Voc != MainJob )
			continue;

		if( SubJob != EM_Vocation_None && PlayerInfo->Voc_Sub != SubJob )
			continue;

		if(	PlayerInfo->LV < MinMainJobLv || PlayerInfo->LV > MaxMainJobLv )
			continue;


		if( PlayerInfo->LV_Sub < MinSubJobLv || PlayerInfo->LV_Sub > MaxSubJobLv )
			continue;

		ResultList.push_back( PlayerInfo );
	}

	for( unsigned i = 0 ; i < ResultList.size() ; i++ )
	{
		OnlinePlayerInfoStruct* PlayerInfo = ResultList[i];
		SearchFindPartyPlayerStruct TempInfo;

		TempInfo.GuildID	= PlayerInfo->GuildID;
		TempInfo.MainJob	= PlayerInfo->Voc;
		TempInfo.MainJobLv	= PlayerInfo->LV;
		TempInfo.Name		= PlayerInfo->Name;
		TempInfo.SubJob		= PlayerInfo->Voc_Sub;
		TempInfo.SubJobLv	= PlayerInfo->LV_Sub;
		TempInfo.TitleID	= PlayerInfo->TitleID;
		TempInfo.ZoneID		= PlayerInfo->ZoneID;
		TempInfo.TitleStr	= PlayerInfo->TitleStr;

		S_SearchFindPartyPlayerResult( Owner->GUID() , (int)ResultList.size() , i , TempInfo );
	}

	if( ResultList.size() == 0 )
	{
		SearchFindPartyPlayerStruct TempInfo;
		S_SearchFindPartyPlayerResult( Owner->GUID() , 0 , 0 , TempInfo );
	}
}