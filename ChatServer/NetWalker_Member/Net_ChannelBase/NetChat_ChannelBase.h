#pragma once

#include "PG/PG_ChannelBase.h"
#include "../../MainProc/Global.h"

class NetSrv_ChannelBase : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_ChannelBase* This;
    static bool _Init();
    static bool _Release();

    static void _OnCliConnect( int ID , string Account );
    static void _OnCliDisconnect( int CliID );

	static void _OnSrvConnect( int SrvSockID , string SrvName );

	static void _OnLocalSrvConnect( EM_SERVER_TYPE ServerType , DWORD ZoneID );

	static void _PG_ChannelBase_CtoChat_DataTransmitToCli_Name	( int Sockid , int Size , void* Data );
	static void _PG_ChannelBase_CtoChat_DataTransmitToCli_Group ( int Sockid , int Size , void* Data );
    static void _PG_ChannelBase_toChat_DataTransmitToGSrv_Group ( int Sockid , int Size , void* Data );
    static void _PG_ChannelBase_toChat_DataTransmitToCli_Group  ( int Sockid , int Size , void* Data );
    static void _PG_ChannelBase_toChat_DataTransmitToSrv        ( int Sockid , int Size , void* Data );
    static void _PG_ChannelBase_toChat_DataTransmitToCli        ( int Sockid , int Size , void* Data );
    static void _PG_ChannelBase_LtoChat_ChangeRoleInfo          ( int Sockid , int Size , void* Data );
    static void _PG_ChannelBase_LtoChat_CreateChannel           ( int Sockid , int Size , void* Data );
    static void _PG_ChannelBase_LtoChat_AddUser                 ( int Sockid , int Size , void* Data );
	static void _PG_ChannelBase_LtoChat_DelUser                 ( int Sockid , int Size , void* Data );
  
	static void _PG_ChannelBase_CtoChat_ChannelPlayerList		( int Sockid , int Size , void* Data );
	static void _PG_ChannelBase_CtoChat_JoinPlayerDisabled		( int Sockid , int Size , void* Data );
	static void _PG_ChannelBase_CtoChat_KickPlayer				( int Sockid , int Size , void* Data );
	static void _PG_ChannelBase_CtoChat_SetManager				( int Sockid , int Size , void* Data );

	static void _PG_ChannelBase_CtoChat_ZoneChannelID			( int Sockid , int Size , void* Data );

	static void _PG_ChannelBase_LtoChat_SetDSysTime				( int Sockid , int Size , void* Data );
	static void _PG_ChannelBase_ChattoL_SetChannelOwner			( int Sockid , int Size , void* Data );
public:
    //-------------------------------------------------------------------

    //-------------------------------------------------------------------
	//通知 Client 連上 Chat
	static void ClientConnect		( int SockID );	
	static void SC_PlayerNotFind	( int SockID , const char* Name );

    static void CreateChannelOK     ( int GSrvID , GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* Name );
    static void CreateChannelFalse  ( int GSrvID , GroupObjectTypeEnum Type , int UserDBID  , char* Name );
    static void AddUserOK           ( int GSrvID , GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* Name , int ChannelOwnerDBID , char* ChannelOwnerName );
    static void AddUserFalse        ( int GSrvID , GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* Name );
    static void DelUserOK           ( int GSrvID , GroupObjectTypeEnum Type , int ChannelID , int UserDBID  );
    static void DelUserFalse        ( int GSrvID , GroupObjectTypeEnum Type , int ChannelID , int UserDBID  );

	static void SL_AddorSetOnlinePlayer  ( OnlinePlayerInfoStruct& Info );
	static void SL_AddorSetOnlinePlayer  ( int SockID , OnlinePlayerInfoStruct& Info );
	static void SL_DelOnlinePlayer  ( char* Name , int DBID );

	static void SC_SetManagerResult	( int SockID , GroupObjectTypeEnum Type , int ChannelID , bool Result );
	static void SC_ChannelOwner		( int SockID , GroupObjectTypeEnum Type , int ChannelID , char* OwnerName , int OwnerDBID );

	//頻道玩家列表
	static void ChannelPlayerListResult( int SockID , GroupObjectTypeEnum Type , int ChannelID , vector< string > &Name );
	//通知所有此頻道的玩家
	static void JoinPlayerDisabledResult( int SockID , int ChannelID , GroupObjectTypeEnum Type , bool JoinEnabled );

	//通知玩家有那些系統頻道
	static void SysChannelInfo( int SockID , GroupObjectTypeEnum Type , int ChannelID , char* Name );

	//通知Server or Client 系統時間
	static void SSrv_SystemTime( int LocalID , int NowTime );
	static void SCli_SystemTime( int SockID , int NowTime );

	static void SSrv_SendAll_SystemTime( int NowTime );
	static void SCli_SendAll_SystemTime( int NowTime );

	static void SC_AddUserNotify( int SockID , GroupObjectTypeEnum Type , int ChannelID , const  char* Name );
	static void SC_DelUserNotify( int SockID , GroupObjectTypeEnum Type , int ChannelID , const  char* Name );

    //-------------------------------------------------------------------
    //  事件繼承實做
    //-------------------------------------------------------------------
    virtual void CliConnect( int CliID , string Account) = 0;
    virtual void CliDisconnect( int CliID ) = 0;
	virtual void OnSrvConnect( int SockID ) = 0;
	virtual void OnLocalSrvConnect( int LocalID ) = 0;

    virtual void DataTransmitToGSrv_Group( BaseItemObject* Sender , GroupObjectClass* TarGroup , int Size , PacketBaseMaxStruct* Data ) = 0;
    virtual void DataTransmitToCli_Group ( BaseItemObject* Sender , GroupObjectClass* TarGroup , int Size , PacketBaseMaxStruct* Data ) = 0;
    virtual void DataTransmitToGSrv     ( BaseItemObject* Sender , BaseItemObject* TarObj , int Size , PacketBaseMaxStruct* Data ) = 0;
    virtual void DataTransmitToCli      ( BaseItemObject* Sender , BaseItemObject* TarObj , int Size , PacketBaseMaxStruct* Data ) = 0; 

	virtual void DataTransmit_CliToCli_Group ( BaseItemObject* Sender , GroupObjectClass* TarGroup , int Size , char* Data ) = 0;
    virtual void DataTransmit_CliToCli_Name  ( BaseItemObject* Sender , BaseItemObject* ToName , int Size , char* Data ) = 0;

    virtual void ChangeRoleInfo( char* Account , OnlinePlayerInfoStruct& Info ) = 0;

    virtual void CreateChannel( BaseItemObject* Sender , GroupObjectTypeEnum Type , char* ChannelName , char* Password ) = 0 ;
    virtual void AddUser( BaseItemObject* User , GroupObjectTypeEnum Type , int ChannelID , char* ChannelName , char* Password , bool IsForceJoin ) = 0;
    virtual void DelUser( BaseItemObject* User , GroupObjectTypeEnum Type , int ChannelID ) = 0;


	virtual void ChannelPlayerList	( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID ) = 0;
	virtual void JoinPlayerDisabled	( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID , bool JoinEnabled ) = 0;
	virtual void KickPlayer			( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID , char* Name ) = 0;
	virtual void SetManager			( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID , char* Name ) = 0;

	virtual void ZoneChannelID		( BaseItemObject* Sender , int ZoneChannelID ) = 0;
	virtual void SetDSysTime		( int DSysTime ) = 0;
	virtual void SetChannelOwner	( GroupObjectTypeEnum Type , int ChannelID , int PlayerDBID ) = 0;
};

