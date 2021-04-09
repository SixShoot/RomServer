#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_GMTools.h"

class NetSrv_GMTools : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_GMTools* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------
    static void	_EventGMToolsConnect		    ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
    static void	_PG_GM_GMtoL_ObjectDataRequest  ( int sockid , int size , void* data );
    static void	_PG_GM_GMtoL_SendMsg            ( int sockid , int size , void* data );
	static void	_PG_GM_GMtoL_SendGMCommand		( int sockid , int size , void* data );
	static void	_PG_GM_GMtoX_ServerStateRequest	( int sockid , int size , void* data );
public:    
    
    static void S_ZoneInfo( int NetID , int ZoneID , const char* ZoneName , int ObjectCount  , int PlayerCount , int NPCCount );
    static void S_ObjInfo( int NetID , RoleDataEx* Role );     
    static void S_DelObjInfo( int NetID , RoleDataEx* Role ); 
    static void S_ObjectData( int NetID , RoleDataEx* Role );
    static void S_PlayerMsg( int NetID , char* Name , char* Msg );
    static void S_SrvMsgToAllGMTools( int LV , char* Tile , char* Context );
	static void S_ServerStateResult( int NetID , ServerStateRequestTypeENUM	RequestType , const char* Name , const char* Type , const char* State 
		, const char* Content1 , const char* Content2
		, const char* Content3 , const char* Content4
		, const char* Content5 , const char* Content6
		, const char* Content7 , const char* Content8 );

    virtual void    R_OnGMToolsConnect( int NetID ) = 0;
    virtual void    R_ObjectDataRequest( int NetID , int GUID )=0;
    virtual void    R_ToPlayerMsg( int NetID , BaseItemObject*	Obj , const char* Name , const char* Msg ) = 0;
    virtual void    R_ToAllPlayerMsg( int NetID , const char* Msg ) = 0;
	virtual void	R_GMCommand( int NetID , int PlayerGUID , const char* Account , int ManageLv, const char* Cmd ) = 0;
	virtual void	R_ServerStateRequest( int NetID , ServerStateRequestTypeENUM	RequestType ) = 0;
};

