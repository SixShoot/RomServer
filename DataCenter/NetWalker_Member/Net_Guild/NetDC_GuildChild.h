#pragma once

#include "NetDC_Guild.h"

//--------------------------------------------------------------------------------------------
class CNetDC_GuildChild : public CNetDC_Guild
{
protected:
	
	static int  _OnTimeProc( SchedularInfo* Obj , void* InputClass );
	static int  _OnGuildFightBidTimeProc( SchedularInfo* Obj , void* InputClass );

	//��l���J
	static bool _SQLCmdInitProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdInitProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//��s�x�s
	static bool _SQLCmdUpDateProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdUpDateProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//���|�s�W
//	static bool _SQLCmdNewGuildProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
//	static void _SQLCmdNewGuildProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	static bool _SQLCmdCreateGuildProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdCreateGuildProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//���|�����s�W
	static bool _SQLCmdNewGuildMemberProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdNewGuildMemberProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//���|�R��
	static bool _SQLCmdDelGuildProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdDelGuildProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//���|�����R��
	static bool _SQLCmdDelGuildMemberProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdDelGuildMemberProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//////////////////////////////////////////////////////////////////////////
	//���|�T���s�W
	static bool _SQLCmdNewMessageProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdNewMessageProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//���|�T���ק�
	static bool _SQLCmdUpdateMessageProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdUpdateMessageProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//���|�T���R��
	static bool _SQLCmdDelMessageProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdDelMessageProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//��s���|�W��
	static bool _SQLChangeGuildNameProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLChangeGuildNameProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdSelectResourceLog( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdSelectResourceLogResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );


	static void	_WarProc( GuildStruct* Guild );

	static void LocalSrvConnectProc( int SrvID );
protected:
	static bool	_IsLoadGuildOK;
	static vector<GuildBaseStruct>		_NeedUpDateGuild;		//���n��s�����|
	static vector<GuildMemberStruct>	_NeedUpDateMember;		//���n��s������
	static deque<int>					_OnConnectSrvID;
	

public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();
//--------------------------------------------------------------------------------------------
	virtual void RL_CreateGuild( int SrvSockID , int PlayerDBID , char* GuildName );
	virtual void RL_AddNewGuildMember( int SrvSockID , int GuildID , char* LeaderName , int LeaderDBID , char* NewMember , int NewMemberDBID );
	virtual void RL_LeaveNewGuild( int SrvSockID , int LeaderDBID , char* PlayerName );
	virtual void RL_DelNewGuild  ( int SrvSockID , int LeaderDBID );
	virtual void OnLocalSrvConnect( int SrvID );

	virtual void RL_AddGuildMember		( int SrvSockID , int GuildID , char* MemberName , int MemberDBID , char* NewMember , int NewMemberDBID );
	virtual void RL_LeaveGuild			( int SrvSockID , int LeaderDBID , char* PlayerName );
	virtual void RL_DelGuild			( int SrvSockID , int LeaderDBID );

	virtual void RL_ModifyGuildInfoRequest		( int SrvSockID , int PlayerDBID , GuildBaseStruct& Guild );
	virtual void RL_ModifyGuildMemberInfoRequest( int SrvSockID , int PlayerDBID , GuildMemberStruct& Member);

	virtual	void RL_SetGuildReady		( int GuildID );
	//--------------------------------------------------------------------------------------------------------------------
	//�d���O
	//--------------------------------------------------------------------------------------------------------------------
	virtual void RL_BoardPostRequest			( int PlayerDBID , GuildBoardStruct& Message );
	virtual void RL_BoardListRequest			( int PlayerDBID , int PageID );
	virtual void RL_BoardMessageRequest			( int PlayerDBID , int MessageGUID );
	virtual void RL_BoardModifyMessageRequest	( int PlayerDBID , int MessageGUID , char* MessageStr );
	virtual void RL_BoardModifyModeRequest		( int PlayerDBID , int MessageGUID , GuildBoardModeStruct	Mode );

//	virtual void RL_BuyFunction					( int ServerID , int GuildID , int PlayerDBID , int Score , int Type , int Value );
//	virtual void RL_GuildShopBuy				( int PlayerDBID , int FlagID );
	//--------------------------------------------------------------------------------------------
	//���|���v��
	virtual void RL_GuildFightAuctionBid		( int LeaderDBID , int CrystalID , int BitScore );

	virtual void RL_GuildFightInfoRequest		( int iRequestPlayerGUID, int iCrystalID );
	//--------------------------------------------------------------------------------------------
	//��Ʈw�x�s �s�W���|
//	static void SaveNewGuild( GuildBaseStruct& Guild );
	//��Ʈw�x�s �s�W���|����
	static void SaveNewGuildMember( GuildMemberStruct& Member );

	//��Ʈw�x�s �R�����|
	static void DelGuild( int GuildID );
	//��Ʈw�x�s �R�����|����
	static void DelGuildMember( int PlayerDBID );

	//--------------------------------------------------------------------------------------------
	//��Ʈw �s�W�T��
	static void SaveGuildMessage( GuildBoardStruct& Msg );

	//��Ʈw �R���T��
	static void DelGuildMessage( int GuildID , int MessageGUID  );

	//��Ʈw �ק�T��
	static void UpdateGuildMessage( GuildBoardStruct& Msg  );

	//--------------------------------------------------------------------------------------------------------------------
	//�q�����a�s�W�������|����
	//--------------------------------------------------------------------------------------------------------------------
	static void SendAllGuildMember_JoinGuildMember				( int GuildID , GuildMemberStruct& Member );
	static void SendAllGuildMember_LeaveGuildMember				( int GuildID , int LeaderDBID , int MemberDBID );
	static void SendAllGuildMember_ChangeLeader					( int GuildID , int NewLeaderDBID , int OldLeaderDBID );
	static void SendAllGuildMember_ChangeMemberRank				( int GuildID , int MemberDBID , int Rank );

	static void	WriteCrystalInfoToDB							( int iCrystalID, int iAttacker, int iDefender, int iBidAmount, int iAdditionTime );
	static	int	GetRemainTime									( int iStartTime, int iEndTime );
	static void	SendGuildFightBeginNotify						( int iCrystalID, int iDefenderGuildID, int iAttackerGuildID, int iRemainTime );

	static map< int, StructCrystal >	m_mapCrystal;

	//////////////////////////////////////////////////////////////////////////
	//���|�ۥ�PK
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_DeclareWar				( int ServerID , int Type , int PlayerDBID , int TargetGuildID );
	virtual void RL_DeclareWarScore			( int GuildID[2] , int Score , int KillCount );

	//���|�X�]�w
	virtual void RL_SetGuildFlagInfoRequest( int PlayerDBID , GuildFlagStruct& Flag );

	//////////////////////////////////////////////////////////////////////////
	//���|��W
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_ChangeGuildNameRequest	( int GuildID , int PlayerDBID , char* GuildName );
	virtual void RL_SetNeedChangeName		( int GuildID );
	//////////////////////////////////////////////////////////////////////////
	//���|�ɯ�
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_GuildUpgradeRequest( int GuildID , int PlayerDBID , int GuildLv );

	//////////////////////////////////////////////////////////////////////////
	//���|���m
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_AddGuildResource	( int ServerID , int GuildID , int PlayerDBID , GuildResourceStruct& Resource , const char* Ret_Runplot , AddGuildResourceTypeENUM LogType , int LogType_ObjID );
	virtual void RL_SelectContributionLog( int PlayerDBID , int DayBefore );

	//�x�s���|���
	static void SaveGuildInfo();

	//////////////////////////////////////////////////////////////////////////
	virtual void RL_GuildWarEndResult( int GuildID , GameResultENUM GameResult , int GuildScore , GuildWarBaseInfoStruct& TargetGuild );

	//�n�D���|�Ծ��v���
	virtual void RL_GuildWarHistroy( int ServerID , int playerDBID );
};
