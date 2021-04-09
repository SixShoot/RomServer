#pragma once

#include "PG/PG_FriendList.h"
#include "../../MainProc/Global.h"
//-------------------------------------------------------------------

class NetSrv_FriendList : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_FriendList* This;
    static bool _Init();
    static bool _Release();

	static void _PG_FriendList_LtoChat_FriendListInfo(  int NetID , int Size , void* Data );


public:
	static void SC_PlayerOnlineInfo(  int SendToDBID , int PlayerDBID , const char* RoleName  
									, Race_ENUM Race , Voc_ENUM Voc , Voc_ENUM Voc_Sub , Sex_ENUM Sex
									, int LV , int LV_Sub , int GuildID , int TitleID  );
	static void SC_PlayerOfflineInfo( int SendToDBID , int PlayerDBID );
	
	virtual void R_FriendListInfo( bool IsInsert , int PlayerDBID , int FriendDBID  ) = 0;
};
//-------------------------------------------------------------------