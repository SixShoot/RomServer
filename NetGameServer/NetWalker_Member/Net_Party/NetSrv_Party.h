#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_Party.h"

class NetSrv_Party : public Global
{
public:
    //-------------------------------------------------------------------
    static NetSrv_Party* This;
protected:
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------
	static void _PG_Party_ChattoL_PartyBaseInfo			( int NetID , int Size , void* Data );		
	static void _PG_Party_ChattoL_AddMember				( int NetID , int Size , void* Data );		
	static void _PG_Party_ChattoL_DelMember				( int NetID , int Size , void* Data );		
	static void _PG_Party_ChattoL_FixMember				( int NetID , int Size , void* Data );		
    //-------------------------------------------------------------------
	static void _PG_Party_ChattoL_GiveItem				( int NetID , int Size , void* Data );
	static void _PG_Party_ChattoL_PartyReloginResult	( int NetID , int Size , void* Data );

	static void _PG_ZoneParty_CtoL_Invite				( int NetID , int Size , void* Data );
	static void _PG_ZoneParty_CtoL_Join					( int NetID , int Size , void* Data );
	static void _PG_ZoneParty_CtoL_KickMember			( int NetID , int Size , void* Data );
	static void _PG_ZoneParty_CtoL_SetMemberPosRequest	( int NetID , int Size , void* Data );
	static void _PG_ZoneParty_CtoL_SetMemberPosResult	( int NetID , int Size , void* Data );
	static void _PG_ZoneParty_CtoL_ChangeLeader			( int NetID , int Size , void* Data );
	static void _PG_ZoneParty_CtoL_Talk					( int NetID , int Size , void* Data );

	static void _PG_Party_CtoL_GetMemberInfo			( int NetID , int Size , void* Data );
	static void _PG_Party_LtoL_GetMemberInfo			( int NetID , int Size , void* Data );

	static void	_OnSrvDisconnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
public:    
	//-------------------------------------------------------------------
	/*
	static void	S_PartyMemberInfo( int GItemID , PartyMemberBaseInfo& Info );
	static void	S_AllParty_PartyMemberInfo( int PartyID , PartyMemberBaseInfo& Info );
	*/
	//-------------------------------------------------------------------
	//chat 
	virtual void R_PartyBaseInfo	( PartyBaseInfoStruct& PartyInfo ) = 0;
	virtual void R_AddMember		( int PartyID , PartyMemberInfoStruct& Member ) = 0;
	virtual	void R_DelMember		( int PartyID , PartyMemberInfoStruct& Member , bool IsOffline ) = 0;
	virtual	void R_FixMember		( int PartyID , PartyMemberInfoStruct& Member ) = 0;
	//-------------------------------------------------------------------
	//Party 物品分配
	//-------------------------------------------------------------------
	//隊伍內獲得物品
	static void S_PartyTreasure( int PartyID , ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_ >& LootDBID );
	static void S_Relogin( int PartyID , int PlayerDBID );

	virtual void R_GiveItem( int DBID , int GItemID , ItemFieldStruct& Item ) = 0;
	virtual void R_PartyReloginResult( int PartyID , int PlayerDBID , bool Result ) = 0;
	virtual void OnChatServerDisconnect( ) = 0;

	static void S_MemberLogout( int ToMemberDBID , int LogoutMemberDBID );
	//-------------------------------------------------------------------
	//戰場區域 Party
	//-------------------------------------------------------------------
	static void SC_Zone_Invite				( int SendID , int InviteDBID , int ClientInfo );
	static void SC_Zone_AddMember			( int SendID , int PartyID , int MemberDBID , int PartyNo , int MemberID );
	static void SC_Zone_DelMember			( int SendID , int PartyID , int MemberDBID );
	static void SC_Zone_FixMember			( int SendID , int PartyID , int MemberDBID , int PartyNo , int MemberID );
	static void SC_Member_Zone_FixMember	( int PartyID , int MemberDBID , int PartyNo , int MemberID );
	static void SC_Zone_SetMemberPosResult	( int SendID , bool Result  );
	static void SC_Zone_PartyBase			( int SendID , int PartyID , int LeaderDBID );
	static void SC_Zone_ChangeLeaderResult	( int SendID , bool Result );
	static void SC_Zone_Talk				( int GItemID , const char* Content  );

	virtual void RC_Zone_Invite				( BaseItemObject* Obj , int InviteDBID , int ClientInfo ) = 0;
	virtual void RC_Zone_Join				( BaseItemObject* Obj , int LeaderDBID ) = 0;
	virtual void RC_Zone_KickMember			( BaseItemObject* Obj , int MemberDBID ) = 0;
	virtual void RC_Zone_SetMemberPosRequest( BaseItemObject* Obj , int MemberDBID[2] , int Pos[2] ) = 0;
	virtual void RC_Zone_ChangeLeader		( BaseItemObject* Obj , int NewLeaderDBID ) = 0;
	virtual void RC_Zone_Talk				( BaseItemObject* Obj , const char* Content ) = 0;
	//-------------------------------------------------------------------
	virtual void RC_GetMemberInfo			(  BaseItemObject* Obj , int LeaderDBID ) = 0;
	virtual void RL_GetMemberInfo			( int RequestPlayerDBID , int LeaderDBID ) = 0;
	virtual void SL_GetMemberInfo			( int ZoneID , int RequestPlayerDBID , int LeaderDBID );
	static  void SC_GetMemberInfoResult		( int SendDBID , int LeaderDBID , int TotalCount , int ID , PartyMemberInfoStruct& Member );
};

