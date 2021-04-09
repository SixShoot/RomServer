#pragma once

#include "NetChat_ChannelBase.h"

class GroupPlayerChannel;

class NetSrv_ChannelBaseChild : public NetSrv_ChannelBase
{
protected:
	static int	_DSysTime;

public:
    static bool Init();
    static bool Release();
	//-------------------------------------------------------------------
	static void Send_SysChannelInfo ( int SockID );
	static void SetManagerBase		( BaseItemObject* NewOwner , GroupPlayerChannel* GroupObj );
    //-------------------------------------------------------------------

    //-------------------------------------------------------------------
    virtual void CliConnect( int CliID , string Account);
    virtual void CliDisconnect( int CliID );

	virtual void OnSrvConnect( int SockID );

    virtual void DataTransmitToGSrv_Group( BaseItemObject* Sender  , GroupObjectClass* TarGroup , int Size , PacketBaseMaxStruct* Data );
	virtual void DataTransmitToCli_Group ( BaseItemObject* Sender  , GroupObjectClass* TarGroup , int Size , PacketBaseMaxStruct* Data );
    virtual void DataTransmitToGSrv      ( BaseItemObject* Sender  , BaseItemObject* TarObj , int Size , PacketBaseMaxStruct* Data );
    virtual void DataTransmitToCli       ( BaseItemObject* Sender  , BaseItemObject* TarObj , int Size , PacketBaseMaxStruct* Data );

	virtual void DataTransmit_CliToCli_Group ( BaseItemObject* Sender , GroupObjectClass* TarGroup , int Size , char* Data );	
	virtual void DataTransmit_CliToCli_Name  ( BaseItemObject* Sender , BaseItemObject* ToName , int Size , char* Data );

    virtual void ChangeRoleInfo( char* Account , OnlinePlayerInfoStruct& Info );

    virtual void CreateChannel( BaseItemObject* Sender , GroupObjectTypeEnum Type , char* ChannelName , char* Password );
    virtual void AddUser( BaseItemObject* User , GroupObjectTypeEnum Type , int ChannelID , char* ChannelName , char* Password , bool IsForceJoin );
    virtual void DelUser( BaseItemObject* User , GroupObjectTypeEnum Type , int ChannelID );

	virtual void ChannelPlayerList	( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID );
	virtual void JoinPlayerDisabled	( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID , bool JoinEnabled );
	virtual void KickPlayer			( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID , char* Name );
	virtual void SetManager			( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID , char* Name );

	virtual void ZoneChannelID		( BaseItemObject* Sender , int ZoneChannelID );

	virtual void OnLocalSrvConnect( int LocalID );

	virtual void SetDSysTime		( int DSysTime );

	virtual void SetChannelOwner	( GroupObjectTypeEnum Type , int ChannelID , int PlayerDBID );
};

