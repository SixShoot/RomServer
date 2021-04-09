#pragma once

#include "PG/PG_Party.h"
#include "../../MainProc/Global.h"
//-------------------------------------------------------------------
class GroupParty;
//-------------------------------------------------------------------

class NetSrv_Party : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Party* This;
    static bool _Init();
    static bool _Release();

	static void	_OnSrvConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );

	static void _PG_Party_CtoChat_Invite		( int Sockid , int Size , void* Data );	
	static void _PG_Party_CtoChat_Join			( int Sockid , int Size , void* Data );
	static void _PG_Party_CtoChat_KickMember	( int Sockid , int Size , void* Data );
	static void _PG_Party_CtoChat_PartyRule		( int Sockid , int Size , void* Data );

	static void _PG_Party_LtoChat_PartyTreasure		( int Sockid , int Size , void* Data );
	static void _PG_Party_CtoChat_PartyTreasureLoot	( int Sockid , int Size , void* Data );
	static void _PG_Party_LtoChat_PartyRelogin		( int Sockid , int Size , void* Data );

	static void _PG_Party_CtoChat_SetMemberPosRequest( int Sockid , int Size , void* Data );
	static void _PG_Party_CtoChat_SetMemberMode		( int Sockid , int Size , void* Data );
public:
	//-------------------------------------------------------------------
	// 送出的封包
	//-------------------------------------------------------------------
	static void SC_Invite				( int SockID , char* LeaderName , int ClientInfo );
	static void SL_PartyBaseInfo		( PartyBaseInfoStruct& Info );
	static void SL_PartyBaseInfo		( int LocalID , PartyBaseInfoStruct& Info );
	static void SL_AddMember			( int PartyID , PartyMemberInfoStruct& Member );
	static void SL_AddMember			( int ServerLocalID , int PartyID , PartyMemberInfoStruct& Member );
	static void SL_DelMember			( int PartyID , PartyMemberInfoStruct& Member , bool IsOffline );
	static void SL_FixMember			( int PartyID , PartyMemberInfoStruct& Member );
	static void SL_PartyReloginResult	( int Sockid , int PartyID , int PlayerDBID , bool Result );

	static void SC_PartyBaseInfo		( int SockID , PartyBaseInfoStruct& Info );
	static void SC_AddMember			( int SockID , PartyMemberInfoStruct& Member );
	static void SC_DelMember			( int SockID , PartyMemberInfoStruct& Member );
	static void SC_FixMember			( int SockID , PartyMemberInfoStruct& Info );
	static void SC_SetMemberPosResult	( int SockID , bool Result );
	//-------------------------------------------------------------------
	// Client 來的要求
	//-------------------------------------------------------------------
	virtual void R_Invite( BaseItemObject* Sender , const char* InviteName , int ClientInfo ) = 0;
	virtual void R_Join( BaseItemObject* Sender , const char* LeaderName ) = 0;
	virtual void R_KickMember( BaseItemObject* Sender ,const char* MemberName , int PartyID ) = 0;
	virtual void R_PartyRule( BaseItemObject* Sender , PartyBaseInfoStruct& Info ) = 0;

	//-------------------------------------------------------------------
	//Party 物品分配
	//-------------------------------------------------------------------
	static void SC_PartyTreasure		( GroupParty* Party , int ItemVersion , PartyItemShareENUM LootType , ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_  >&  LootDBID  );
	static void SC_PartyTreasure		( int PlayerDBID , int ItemVersion , PartyItemShareENUM LootType , ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_  >&  LootDBID  );
	static void SC_PartyTreasureLoot	( GroupParty* Party , int DBID , int ItemVersion , int LootValue );
	static void SL_GiveItem				( int ZoneID , int DBID , int GItemID , ItemFieldStruct& Item );
	static void SC_GiveItemInfo			( GroupParty* Party , int DBID , ItemFieldStruct& Item , int ItemVersion );	


	virtual void R_PartyTreasure( int PartyID , ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_  >&  LootDBID ) = 0;
	virtual void R_PartyTreasureLoot( BaseItemObject* Sender , int ItemVersion , PartyTreasureLootTypeENUM Type , int DBID ) = 0;
	virtual void R_PartyRelogin( int SockID , int PartyID , int PlayerDBID ) = 0;

	virtual void OnGameSrvConnect( int dwServerLocalID ) = 0;

	virtual void R_SetMemberPosRequest( BaseItemObject* Sender , int MemberDBID[2] , int Pos[2] ) = 0;
	virtual void R_SetMemberMode( BaseItemObject* Sender , int MemberDBID , PartyMemberModeStruct Mode ) = 0;
};

