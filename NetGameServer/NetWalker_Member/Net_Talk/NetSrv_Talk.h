#ifndef __NETWAKER_TALK_H__
#define __NETWAKER_TALK_H__
#include "../../MainProc/Global.h"
#include "PG/PG_Talk.h"

class NetSrv_Talk : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Talk* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------

    static void _PG_Talk_TalkTo         		( int Sockid , int Size , void* Data );
    static void _PG_Talk_Range          		( int Sockid , int Size , void* Data );
    static void _PG_Talk_CtoL_GMCommand 		( int Sockid , int Size , void* Data );
    static void _PG_GM_CtoL_SendMsg     		( int Sockid , int Size , void* Data );
	static void _PG_Talk_Channel				( int Sockid , int Size , void* Data );
	static void _PG_Talk_DtoL_OfflineGMCommand	( int Sockid , int Size , void* Data );

public:    
    static void TalkTo          ( int Sender , bool IsSystem , const char* TalkerName , const char* ToName , const char* Content );    //同Srv
    static void TalkToByRoleName( BaseItemObject* Sender , bool IsSystem , const char* Name , const char* Content , TalkSenderInfo& SenderInfo ); 

    static void Range           ( int Sayer  , bool IsSystem , const char* Content, bool bNpcGossip , TalkSenderInfo* SenderInfo = NULL );
	static void SayTo			( int SendID , int SayerID , const char* Content );
    static void SysMsg          ( int Sender , const char* Content );
	static void Yell	        ( int Sayer  , bool IsSystem , const char* Content, int iRangeBlock );
	

	static void	EventMsg		( int iRoleGUID, EM_EVENT_MSG emEvent, ... );
    static void GMMsg           ( int SenderID , const char* Content );

	static void	ScriptMessage	( int iSayerID, int iTargetID, int iType,  const char* sEmoteString, int iColor );

	static void GameMsgEvent		( int SenderID , GameMessageEventENUM Event );
	static void GameMsgEventData	( int SenderID , int iNumArg, GameMessageEventDataENUM Event 
										, GameMessageDataTypeENUM Type1 , int Value1 
										, GameMessageDataTypeENUM Type2 = EM_GameMessageDataType_None , int Value2 = 0
										, GameMessageDataTypeENUM Type3 = EM_GameMessageDataType_None , int Value3 = 0
										, GameMessageDataTypeENUM Type4 = EM_GameMessageDataType_None , int Value4 = 0
										, GameMessageDataTypeENUM Type5 = EM_GameMessageDataType_None , int Value5 = 0
										);

	static void Party_All       ( Voc_ENUM Job , int PartyID , TalkChannelTypeENUM MsgType , const char* Name , const char* Content , TalkSenderInfo& SenderInfo );
	static void Guild_All       ( Voc_ENUM Job , int GuildID , TalkChannelTypeENUM MsgType , const char* Name  , const char* Content , TalkSenderInfo& SenderInfo );

	static void SendRunningMsg		( int GUID , const char* Content );
	static void SendRunningMsg_Zone	( const char* Content );
	static void SendRunningMsg_All	( const char* Content );
	static void SendRunningMsg_Room ( int RoomID , const char* Content  );

	static void SendRunningMsgEx		( int GUID , RunningMsgExTypeENUM Type , const char* Content );
	static void SendRunningMsgEx_Zone	( RunningMsgExTypeENUM Type , const char* Content );
	static void SendRunningMsgEx_All	( RunningMsgExTypeENUM Type , const char* Content );
	static void SendRunningMsgEx_Room	( int RoomID , RunningMsgExTypeENUM Type , const char* Content  );

	//送系統頻道
	static void SChat_SysChannel( int PlayerDBID , GroupObjectTypeEnum Type , TalkChannelTypeENUM MsgType , int ChannelID , char* Name  , char* Context , TalkSenderInfo& SenderInfo  );

	static void SD_OfflineMessageRequest( int PlayerDBID );
    //-------------------------------------------------------------------
    //收到的事件
    //-------------------------------------------------------------------
    virtual void Player_TalkTo      ( BaseItemObject* Sender , const char* TargetName , int ContentSize , const char* Content , TalkSenderInfo& SenderInfo ) = 0;
    virtual void Player_Range       ( BaseItemObject* Sender , int ContentSize , const char* Content , TalkSenderInfo& SenderInfo  ) = 0;
    virtual void Player_GMCommand   ( BaseItemObject* Sender , int ContentSize , const char* Content ) = 0;
    virtual void ToGMTools          ( BaseItemObject* Sender , const char* Content ) = 0;
    //-------------------------------------------------------------------
	virtual void R_Guild			( BaseItemObject* Sender , int ContentSize , const char* Content , TalkSenderInfo& SenderInfo ) = 0;
	virtual void R_SysChannel		( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID , int ContentSize , char* Content , TalkSenderInfo& SenderInfo ) = 0;
	virtual void RD_OfflineGMCommand( int PlayerDBID , ManagementENUM ManageLv , const char* Content ) = 0;
};

#endif //__NETWAKER_TALK_H__