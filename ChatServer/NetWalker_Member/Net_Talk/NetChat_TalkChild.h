#pragma once

#include "NetChat_Talk.h"


struct CliBoardInfoStruct
{
	int 	PlayerDBID;
	int 	LiveTime;
	int 	DataSize;
	char	Data[1024];
};


class NetSrv_TalkChild : public NetSrv_Talk
{
	static map< int , CliBoardInfoStruct >	 _CliBoardInfoMap;
public:
    static bool Init();
    static bool Release();	

	static void OnTimeProcCliBoardInfo( );

	virtual void R_Channel					( int SockID , GroupObjectTypeEnum Type , int ChannelID , char* Name  , char* Context , TalkSenderInfo& SenderInfo );
	virtual void R_SysChannel				( int PlayerDBID , GroupObjectTypeEnum Type , int ChannelID , char* Name  , char* Context , TalkSenderInfo& SenderInfo );

	virtual void R_RegisterBillBoardInfo	( int Sockid , int DataSize , char* Data );
	virtual void R_BillBoardLiveTime		( int Sockid , int LiveTime );
	virtual void OnCliConnect				( int CliID , char* Account );
	virtual void OnCliDisconnect			( int CliID );
};

