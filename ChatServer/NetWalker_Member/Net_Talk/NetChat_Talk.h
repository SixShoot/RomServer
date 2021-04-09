#pragma once

#include "PG/PG_Talk.h"
#include "../../MainProc/Global.h"
//-------------------------------------------------------------------

class NetSrv_Talk : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Talk* This;
    static bool _Init();
    static bool _Release();

	static void _OnCliConnect( int ID , string Account );
	static void _OnCliDisconnect( int CliID );


	static void _PG_Talk_Channel						( int Sockid , int Size , void* Data );
	static void _PG_Talk_LtoChat_SysChannel				( int Sockid , int Size , void* Data );

	static void _PG_Talk_CtoChat_RegisterBillBoardInfo	( int Sockid , int Size , void* Data );
	static void _PG_Talk_CtoChat_BillBoardLiveTime		( int Sockid , int Size , void* Data );

public:
	//-------------------------------------------------------------------
	// °e¥Xªº«Ê¥]
	//-------------------------------------------------------------------

	static void TalkTo( Voc_ENUM Job , int SockID , char* TalkName , char* Context );
	static void SysMsg( int SockID , char* Msg );
	static void SysMsg_ChartoUtf8( int SockID , char* Msg );
	static void Channel( int SockID , Voc_ENUM Job , GroupObjectTypeEnum Type , int ID , TalkChannelTypeENUM MsgType , char* TalkName , char* Context , TalkSenderInfo& SenderInfo );

	static void	Send_Channel( Voc_ENUM Job , GroupObjectTypeEnum Type , int ChannelID , TalkChannelTypeENUM MsgType , char* Name  , char* Context , TalkSenderInfo& SenderInfo );
	static void	Send_ZoneChannel( Voc_ENUM Job , GroupObjectClass* Group , int ChannelID , TalkChannelTypeENUM MsgType , char* Name  , char* Context , TalkSenderInfo& SenderInfo );
	static void Send_SysChannel( Voc_ENUM Job , int ChannelID , TalkChannelTypeENUM MsgType , char* Name  , char* Context , TalkSenderInfo& SenderInfo );

	static void GameMsgEvent	( int iSockID , GameMessageEventENUM Event );

	//-------------------------------------------------------------------
	virtual void R_Channel( int SockID , GroupObjectTypeEnum Type , int ChannelID , char* Name  , char* Context , TalkSenderInfo& SenderInfo ) = 0;
	virtual void R_SysChannel( int PlayerDBID , GroupObjectTypeEnum Type , int ChannelID , char* Name  , char* Context , TalkSenderInfo& SenderInfo ) = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual void R_RegisterBillBoardInfo	( int Sockid , int DataSize , char* Data ) = 0;
	virtual void R_BillBoardLiveTime		( int Sockid , int LiveTime ) = 0;
	static void SC_BillBoardInfo			( int Sockid , int PlayerDBID , int DataSize , char* Data );
	static void SC_All_BillBoardInfo		( int PlayerDBID , int DataSize , char* Data );
	static void SC_All_BillBoardDelete		( int PlayerDBID );

	virtual void OnCliConnect				( int CliID , char* Account ) = 0;
	virtual void OnCliDisconnect			( int CliID ) = 0;

};


//-------------------------------------------------------------------