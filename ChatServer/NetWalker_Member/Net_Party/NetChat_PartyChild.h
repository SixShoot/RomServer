#pragma once

#include "NetChat_Party.h"

class NetSrv_PartyChild : public NetSrv_Party
{
public:
    static bool Init();
    static bool Release();

    //-------------------------------------------------------------------

    //-------------------------------------------------------------------
	virtual void R_Invite( BaseItemObject* Sender , const char* InviteName , int ClientInfo );
	virtual void R_Join( BaseItemObject* Sender , const char* LeaderName );
	virtual void R_KickMember( BaseItemObject* Sender , const char* MemberName , int PartyID );
	virtual void R_PartyRule( BaseItemObject* Sender , PartyBaseInfoStruct& Info );

	virtual void R_PartyTreasure( int PartyID , ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_  >&  LootDBID );
	virtual void R_PartyTreasureLoot( BaseItemObject* Sender , int ItemVersion , PartyTreasureLootTypeENUM Type , int DBID );
	virtual void R_PartyRelogin( int SockID , int PartyID , int PlayerDBID );
	virtual void R_SetMemberPosRequest( BaseItemObject* Sender , int MemberDBID[2] , int Pos[2] );
	virtual void R_SetMemberMode( BaseItemObject* Sender , int MemberDBID , PartyMemberModeStruct Mode );

	virtual void OnGameSrvConnect( int dwServerLocalID );
};

