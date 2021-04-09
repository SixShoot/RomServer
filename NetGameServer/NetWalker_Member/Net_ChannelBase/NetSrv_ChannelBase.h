#ifndef __NETWAKER_CHANNELBASE_H__
#define __NETWAKER_CHANNELBASE_H__

#include "../../MainProc/Global.h"
#include "PG/PG_ChannelBase.h"

class NetSrv_ChannelBase : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_ChannelBase* This;
    static bool _Init();
    static bool _Release();

    static void _PG_ChannelBase_ChattoL_CreateChannelResult ( int Sockid , int Size , void* Data );
    static void _PG_ChannelBase_ChattoL_AddUserResult       ( int Sockid , int Size , void* Data );
    static void _PG_ChannelBase_ChattoL_DelUserResult       ( int Sockid , int Size , void* Data );

	static void _PG_ChannelBase_ChattoAll_SystemTime	    ( int Sockid , int Size , void* Data );
    //-------------------------------------------------------------------
    static void _OnChatConnect( EM_SERVER_TYPE ServerType , DWORD ZoneID  );
	static void _OnChatDisconnect( EM_SERVER_TYPE ServerType , DWORD ZoneID  );
    //-------------------------------------------------------------------
	//Client端來的要求
	static void _PG_ChannelBase_CtoL_CreateChannel		( int Sockid , int Size , void* Data );
	static void _PG_ChannelBase_CtoL_JoinChannel		( int Sockid , int Size , void* Data );
	static void _PG_ChannelBase_CtoL_LeaveChannel		( int Sockid , int Size , void* Data );
	//-------------------------------------------------------------------
	static void _PG_ChannelBase_ChattoL_AddorSetOnlinePlayer( int Sockid , int Size , void* Data );
	static void _PG_ChannelBase_ChattoL_DelOnlinePlayer		( int Sockid , int Size , void* Data );
	//-------------------------------------------------------------------
public:    
    //-------------------------------------------------------------------
     
    //-------------------------------------------------------------------
    static void DataTransmitToGSrv_Group( int SenderDBID , GroupObjectTypeEnum Type , int ChannelID , int Size , const void* Data );
    static void DataTransmitToCli_Group ( int SenderDBID , GroupObjectTypeEnum Type , int ChannelID , int Size , const void* Data );
//    static void DataTransmitToGSrv_DBID ( int SenderDBID , int TargetDBID , int Size , const void* Data );
//    static void DataTransmitToCli_DBID  ( int SenderDBID , int TargetDBID , int Size , const void* Data );
//    static void DataTransmitToGSrv_RoleName ( int SenderDBID , const char* TarName , int Size , const void* Data );
//    static void DataTransmitToCli_RoleName  ( int SenderDBID , const char* TarName , int Size , const void* Data );

    static void ChangeRoleInfo  ( const char* Account , OnlinePlayerInfoStruct& Info );

    static void CreateChannel( int SenderDBID , GroupObjectTypeEnum Type , char* ChannelName , char* Password );
	//
	static void AddUser_PlayerChannel( int SenderDBID , char* ChannelName, char* Password , bool IsForceJoin );
    static void AddUser( int SenderDBID , int AddDBID , GroupObjectTypeEnum Type , int ChannelID , char* Password );
    static void DelUser( int DelDBID , GroupObjectTypeEnum Type , int ChannelID );

	static void SetDSysTime( int DSysTime );
    //-------------------------------------------------------------------
    //  事件繼承實做
    //-------------------------------------------------------------------
    virtual void CreateChannelOK     ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* ChannelName ) = 0;
    virtual void CreateChannelFalse  ( GroupObjectTypeEnum Type , int UserDBID , char* ChannelName ) = 0;
    virtual void AddUserOK           ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* ChannelName , int ChannelOwnerDBID , char* ChannelOwnerName ) = 0;
    virtual void AddUserFalse        ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* ChannelName ) = 0;
    virtual void DelUserOK           ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID  ) = 0;
    virtual void DelUserFalse        ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID  ) = 0;
    virtual void OnChatConnect       ( EM_SERVER_TYPE ServerType , DWORD ZoneID  ) = 0;
	virtual void OnChatDisconnect    ( EM_SERVER_TYPE ServerType , DWORD ZoneID  ) = 0;


	virtual void OnSystemTimeNotify	 ( int Time ) = 0;
	//線上玩家網路資料處理
//	virtual void RChat_AddorSetOnlinePlayer( OnlinePlayerInfoStruct& Info ) = 0;
//	virtual void RChat_DelOnlinePlayer( char* Name , int DBID ) = 0;

	//-------------------------------------------------------------------
	//  送封包 Client
	//-------------------------------------------------------------------
	static void S_CreateChannelOK( int GUID , GroupObjectTypeEnum Type , int ChannelID , char* ChannelName );
	static void S_CreateChannelFalse( int GUID , GroupObjectTypeEnum Type , char* ChannelName );
	static void S_JoinChannelOK( int GUID , GroupObjectTypeEnum Type , int ChannelID , char* ChannelName , int ChannelOwnerDBID , char* ChannelOwnerName );
	static void S_JoinChannelFalse( int GUID , GroupObjectTypeEnum Type , int ChannelID , char* ChannelName );
	static void S_LeaveChannelOK( int GUID , GroupObjectTypeEnum Type , int ChannelID );
	static void S_LeaveChannelFalse( int GUID , GroupObjectTypeEnum Type , int ChannelID );
	//-------------------------------------------------------------------
	//  事件繼承實做 Client
	//-------------------------------------------------------------------
	virtual void R_CreateChannel( BaseItemObject* Player  , char* ChannelName , char* Password ) = 0;
	virtual void R_JoinChannel( BaseItemObject* Player  , char* ChannelName , char* Password , bool IsForceJoin ) = 0;
	virtual void R_LeaveChannel( BaseItemObject* Player  , int ChannelID ) = 0;
	//////////////////////////////////////////////////////////////////////////
	static void S_ChangeChannelOwner( GroupObjectTypeEnum Type , int ChannelID , int PlayerDBID );
};

#endif //__NETWAKER_CHANNELBASE_H__