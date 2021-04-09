#pragma once

#include "NetSrv_Party.h"

class NetSrv_PartyChild : public NetSrv_Party
{
public:
    static bool Init();
    static bool Release();

	static bool SendItemToParty( StaticVector< BaseItemObject* , _MAX_PARTY_COUNT_  >& LootObj , ItemFieldStruct& Item );

	//-------------------------------------------------------------------
	//chat 
	virtual void R_PartyBaseInfo	( PartyBaseInfoStruct& PartyInfo );
	virtual void R_AddMember		( int PartyID , PartyMemberInfoStruct& Member );
	virtual	void R_DelMember		( int PartyID , PartyMemberInfoStruct& Member , bool IsOffline );
	virtual	void R_FixMember		( int PartyID , PartyMemberInfoStruct& Member );
	//-------------------------------------------------------------------
	virtual void R_GiveItem( int DBID , int GItemID , ItemFieldStruct& Item );
	virtual void R_PartyReloginResult( int PartyID , int PlayerDBID , bool Result );
	virtual void OnChatServerDisconnect( );

	virtual void RC_GetMemberInfo	(  BaseItemObject* Obj , int LeaderDBID );
	virtual void RL_GetMemberInfo	( int RequestPlayerDBID , int LeaderDBID );
	//-------------------------------------------------------------------
	//¾Ô³õ°Ï°ì Party
	//-------------------------------------------------------------------
	static bool  Zone_Join( int GItemID , int PartyID );
	static bool	 Zone_Clear( int PartyID );

	virtual void RC_Zone_Invite				( BaseItemObject* Obj , int InviteDBID , int ClientInfo );
	virtual void RC_Zone_Join				( BaseItemObject* Obj , int LeaderDBID );
	virtual void RC_Zone_KickMember			( BaseItemObject* Obj , int MemberDBID );
	virtual void RC_Zone_SetMemberPosRequest( BaseItemObject* Obj , int MemberDBID[2] , int Pos[2] );
	virtual void RC_Zone_ChangeLeader		( BaseItemObject* Obj , int NewLeaderDBID );
	virtual void RC_Zone_Talk				( BaseItemObject* Obj , const char* Content );

};

