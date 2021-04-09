#include "NetDC_GuildChild.h"	
#include "ReaderClass/CreateDBCmdClass.h"
#include "ReaderClass/DbSqlExecClass.h"
#include "../../MainProc/GuildManage//GuildManage.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
#include "RoleData/ObjectDataClass.h"
//-------------------------------------------------------------------
bool CNetDC_GuildChild::_IsLoadGuildOK = false;
vector<GuildBaseStruct>		CNetDC_GuildChild::_NeedUpDateGuild;		//���n��s�����|
vector<GuildMemberStruct>	CNetDC_GuildChild::_NeedUpDateMember;		//���n��s������
deque<int>					CNetDC_GuildChild::_OnConnectSrvID;
map< int, StructCrystal >	CNetDC_GuildChild::m_mapCrystal;
//-------------------------------------------------------------------
bool    CNetDC_GuildChild::Init()
{
	CNetDC_Guild::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetDC_GuildChild );

	_RD_NormalDB->SqlCmd( 0 ,  _SQLCmdInitProc , _SQLCmdInitProcResult , NULL , NULL );

	printf( "\n");

	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 1000 );
		_RD_NormalDB->Process( );
		switch( i % 4 )
		{
		case 0:
			printf( "\rGuild Info Loading..... /        " );
			break;
		case 1:
			printf( "\rGuild Info Loading..... |        " );
			break;
		case 2:			
			printf( "\rGuild Info Loading..... \\        " );
			break;
		case 3:
			printf( "\rGuild Info Loading..... -        " );
			break;
		}

		if( _IsLoadGuildOK == true )
			break;
	}
	printf( "\n");

	Schedular()->Push( _OnTimeProc , 10 * 1000 , This , NULL );  
	Schedular()->Push( _OnGuildFightBidTimeProc , 60 * 1000 , This , NULL );  


	// ���J m_mapCrystal
		StructCrystal Crystal;

		Crystal.iAttacker	= 0;
		Crystal.iDefender	= 0;
		Crystal.iBidAmount	= 0;

		Crystal.iStartTime	= 482400;
		Crystal.iEndTime	= 488100;

		Crystal.iFightTime	= 504000;

		m_mapCrystal[ 1 ] = Crystal;




	return true;
}

//--------------------------------------------------------------------------------------
bool    CNetDC_GuildChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	CNetDC_Guild::_Release();
	return true;

}
//--------------------------------------------------------------------------------------
struct CreateGuildTempStruct
{
	int		SrvSockID;
	int		PlayerDBID;
	string	GuildName;

	GuildBaseStruct GuildBase;
};
void CNetDC_GuildChild::RL_CreateGuild( int SrvSockID , int PlayerDBID , char* GuildName )
{
	if( CheckSqlStr( GuildName ) == false )
	{
		SL_CreateGuildResult( SrvSockID , PlayerDBID , GuildName , -1 , EM_CreateGuildResult_Failed_Name );
		return;
	}
	OnlinePlayerInfoStruct* PlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( PlayerDBID );
	if( PlayerInfo == NULL || g_ObjectData->GuildLvTable().size() == 0 )
	{
		return;
	}

//	CharacterNameRulesENUM  Ret = g_ObjectData->CheckCharacterNameRules( GuildName , Global::_LanguageType ,  );
	std::wstring outStrName;
	CharacterNameRulesENUM  Ret = g_ObjectData->CheckCharacterNameRules( GuildName , Global::_CountryType , outStrName );
	if( Ret != EM_CharacterNameRules_Rightful )
	{
	
		switch( Ret )
		{
		case EM_CharacterNameRules_Short:				// �W�٤ӵu,���X�k
			SL_CreateGuildResult( SrvSockID , PlayerDBID , GuildName , -1 , EM_CreateGuildResult_NameToShort );
			break;
		case EM_CharacterNameRules_Long:				// �W�٤Ӫ�,���X�k
			SL_CreateGuildResult( SrvSockID , PlayerDBID , GuildName , -1 , EM_CreateGuildResult_NameToLong );
			break;
		case EM_CharacterNameRules_Wrongful:			// �����~���Ÿ��Φr��
			SL_CreateGuildResult( SrvSockID , PlayerDBID , GuildName , -1 , EM_CreateGuildResult_NameIsWrongful );
			break;
		case EM_CharacterNameRules_Special:				// �S��W�٤����
			SL_CreateGuildResult( SrvSockID , PlayerDBID , GuildName , -1 , EM_CreateGuildResult_HasSpecialString );
			break;
		case EM_CharacterNameRules_BasicLetters:		// �u��ϥΰ򥻭^��r��
			SL_CreateGuildResult( SrvSockID , PlayerDBID , GuildName , -1 , EM_CreateGuildResult_OnlyBasicLetters );
			break;
		case EM_CharacterNameRules_EndLetter:			// �����r�����~(',")
			SL_CreateGuildResult( SrvSockID , PlayerDBID , GuildName , -1 , EM_CreateGuildResult_EndLetterError );
			break;
		}
		return;
	}


	GuildLvInfoStruct& GuildInfo =g_ObjectData->GuildLvTable()[0];

	CreateGuildTempStruct* TempData = NEW(CreateGuildTempStruct);
	TempData->SrvSockID = SrvSockID;
	TempData->PlayerDBID = PlayerDBID;
	TempData->GuildName = WCharToUtf8( outStrName.c_str() ).c_str();;

	TempData->GuildBase.Init();
	TempData->GuildBase.Level		= 0;
	TempData->GuildBase.LeaderName = PlayerInfo->Name;
	TempData->GuildBase.LeaderDBID = PlayerInfo->DBID;
	TempData->GuildBase.MaxMemberCount = GuildInfo.MemberCount;
	TempData->GuildBase.RankCount = GuildInfo.RankCount;

	//_RD_NormalDB->SqlCmd_Transcation( PlayerDBID ,  _SQLCmdCreateGuildProc , _SQLCmdCreateGuildProcResult , TempData , NULL );
	//��@Thread �B�z ���|�إ� �ҥH�i�H���� Table Lock
	_RD_NormalDB->SqlCmd( 2 ,  _SQLCmdCreateGuildProc , _SQLCmdCreateGuildProcResult , TempData , NULL );

}

bool CNetDC_GuildChild::_SQLCmdCreateGuildProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char	SqlCmd[8192];
	CreateGuildTempStruct* TempData = (CreateGuildTempStruct*) UserObj;

	MyDbSqlExecClass	MyDBProc( sqlBase );

	CharTransferClass	CharTransfer;
	CharTransfer.SetUtf8String( TempData->GuildName.c_str() );

	string GuildNameBinStr = StringToSqlX( (char*)CharTransfer.GetWCString() , CharTransfer.WCStrLen() * 2 , false );

	int		Count = 0;
	//�ˬd���|�W�٬O�_���_
	sprintf( SqlCmd , "SELECT COUNT(GUID) AS Expr1 FROM Guild_Base WHERE (GuildName = CAST( %s AS nvarchar(4000) )) ", GuildNameBinStr.c_str()  );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Count );
	MyDBProc.Read();
	MyDBProc.Close();

	if( Count != 0 )
	{
		return false;
	}

	int		MaxGuildID = 0;
	//���o�ثe�̤j���|ID
	MyDBProc.SqlCmd( "SELECT MAX(GUID) AS Expr1 FROM Guild_Base" );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&MaxGuildID );
	MyDBProc.Read();
	MyDBProc.Close();

	
	GuildBaseStruct& GuildBase = TempData->GuildBase;
	
	GuildBase.GuildID	= MaxGuildID+1;
	GuildBase.GuildName = TempData->GuildName.c_str();
	GuildBase.Rank[EM_GuildRank_Leader].Setting.Type = -1;
	GuildBase.RankCount = 1;
	GuildBase.Rank[EM_GuildRank_Leader].RankName	= "[GUILD_STR_LEADER]";//g_ObjectData->GetString( "GUILD_STR_LEADER" );
	GuildBase.Rank[EM_GuildRank_0].RankName			= "[GUILD_STR_MEMBER]";//g_ObjectData->GetString( "GUILD_STR_MEMBER" );



	//g_CritSect()->Enter();
	MyDBProc.SqlCmd_WriteOneLine(  _RDGuildBaseSql->GetInsertStr( &GuildBase ).c_str() );
	//g_CritSect()->Leave();


	sprintf( SqlCmd , "UPDATE RoleData Set GuildID = %d , GuildRank = 9 WHERE DBID= %d" , GuildBase.GuildID , TempData->PlayerDBID );
	MyDBProc.SqlCmd_WriteOneLine( SqlCmd );

	return true;

}

void CNetDC_GuildChild::_SQLCmdCreateGuildProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	CreateGuildTempStruct* TempData = (CreateGuildTempStruct*) UserObj;

	OnlinePlayerInfoStruct* PlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( TempData->PlayerDBID );

	if( ResultOK && PlayerInfo )
	{
		SL_CreateGuildResult( TempData->SrvSockID , TempData->PlayerDBID , (char*)TempData->GuildName.c_str() , TempData->GuildBase.GuildID , EM_CreateGuildResult_OK );

		GuildManageClass::This()->CreateGuild( TempData->GuildBase );

		GuildMemberStruct Member;
		Member.MemberDBID	= TempData->PlayerDBID;
		Member.GuildID		= TempData->GuildBase.GuildID ;
		Member.MemberName	= PlayerInfo->Name;
		Member.Rank			= EM_GuildRank_Leader;
		Member.RankTime		= TimeStr::Now_Value();
		Member.Voc			= PlayerInfo->Voc;
		Member.Voc_Sub		= PlayerInfo->Voc_Sub;
		Member.Sex			= PlayerInfo->Sex;
		Member.LV			= PlayerInfo->LV;
		Member.LV_Sub		= PlayerInfo->LV_Sub;
		Member.IsOnline		= true;

		GuildManageClass::This()->AddMember( Member );
		GuildManageClass::This()->SetMemberOnline( TempData->PlayerDBID , PlayerInfo );

		//�q���Ҧ�Local Guild���
		SL_All_AddGuild( TempData->GuildBase  );
		SL_All_AddGuildMember( Member );

		//��Ʈw�x�s
		//SaveNewGuild( NewGuild->Base );
		SaveNewGuildMember( Member );

		SendAllGuildMember_JoinGuildMember( Member.GuildID , Member );

		Global::Log_GuildLog( TempData->GuildBase.GuildID , EM_GuildActionType_CreateNewGuild , TempData->PlayerDBID , TempData->PlayerDBID );
	}
	else
	{
		SL_CreateGuildResult( TempData->SrvSockID , TempData->PlayerDBID , (char*)TempData->GuildName.c_str() , TempData->GuildBase.GuildID , EM_CreateGuildResult_Failed_Name );
	}

	delete TempData;
}

void CNetDC_GuildChild::RL_AddNewGuildMember( int SrvSockID , int GuildID , char* LeaderName , int LeaderDBID , char* NewMember , int MemberDBID )
{
	OnlinePlayerInfoStruct* PlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( MemberDBID );
	if( PlayerInfo == NULL )
	{
		SL_AddNewGuildMemberResult( SrvSockID , LeaderName ,LeaderDBID , GuildID , NewMember , false );
		return;
	}

	GuildMemberStruct Member;
	Member.GuildID		= GuildID;
	Member.MemberName	= NewMember;
	Member.MemberDBID	= MemberDBID;
	Member.Rank			= EM_GuildRank_0;
	Member.RankTime		= TimeStr::Now_Value();
	Member.Voc			= PlayerInfo->Voc;
	Member.Voc_Sub		= PlayerInfo->Voc_Sub;
	Member.Sex			= PlayerInfo->Sex;
	Member.LV			= PlayerInfo->LV;
	Member.LV_Sub		= PlayerInfo->LV_Sub;
	Member.IsOnline		= true;

	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildID );
	if(		Guild->Base.LeaderDBID != LeaderDBID 
		||	Guild->Base.IsReady == true			)
	{
		SL_AddNewGuildMemberResult( SrvSockID , LeaderName ,LeaderDBID , GuildID , NewMember , false );
		return;
	}

	if( (int)Guild->Member.size() >= Guild->Base.MaxMemberCount ) 
	{
		SL_AddNewGuildMemberResult( SrvSockID , LeaderName ,LeaderDBID , GuildID , NewMember , false );
		return;
	}

	bool Ret = GuildManageClass::This()->AddMember( Member );

	if( Ret == true )
	{
		GuildManageClass::This()->SetMemberOnline( MemberDBID , PlayerInfo );
		SL_All_AddGuildMember( Member );
		//��Ʈw�x�s
		SaveNewGuildMember( Member );

		SendAllGuildMember_JoinGuildMember( Member.GuildID , Member );
	}

	SL_AddNewGuildMemberResult( SrvSockID , LeaderName ,LeaderDBID , GuildID , NewMember , Ret );

	Global::Log_GuildLog( GuildID , EM_GuildActionType_JoinNewGuild , Guild->Base.LeaderDBID , MemberDBID );
}
//-------------------------------------------------------------------------------------
void CNetDC_GuildChild::RL_LeaveNewGuild( int SrvSockID , int LeaderDBID , char* PlayerName )
{
	//���o���|�������
	GuildMemberStruct* LeaderGuildMember = GuildManageClass::This()->GetMember( LeaderDBID );
	//Leader �S���|
	if( LeaderGuildMember == NULL )
	{
		SL_LeaveNewGuildResult( SrvSockID , LeaderDBID , PlayerName , false );
		return;
	}

	GuildStruct* Guild = LeaderGuildMember->Guild;

	if( Guild->Base.IsReady == true )
	{
		SL_LeaveNewGuildResult( SrvSockID , LeaderDBID , PlayerName , false );
		return;
	}

	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( Guild->Base.GuildID , PlayerName );
	if( Member == NULL )
	{
		SL_LeaveNewGuildResult( SrvSockID , LeaderDBID , PlayerName , false );
		return;
	}

	//�u���|�� or �ۤv ���v�O 
	if( Guild->Base.LeaderDBID != LeaderDBID && Member->MemberDBID != LeaderDBID )
	{
		SL_LeaveNewGuildResult( SrvSockID , LeaderDBID , PlayerName , false );
		return;
	}


	bool	Ret =false;
	int PlayerDBID = Member->MemberDBID;
	int GuildID = Member->GuildID;

	Global::Log_GuildLog( GuildID , EM_GuildActionType_LeaveNewGuild , Guild->Base.LeaderDBID , Member->MemberDBID );

	if( PlayerDBID == Guild->Base.LeaderDBID )
	{
		Global::Log_GuildLog( GuildID , EM_GuildActionType_DestroyNewGuild , Guild->Base.LeaderDBID , Member->MemberDBID );

		Ret = GuildManageClass::This()->DelGuild( GuildID );		
		//��Ʈw�R��
		DelGuild(  GuildID );
		SL_All_DelGuild( GuildID );

	}
	else
	{
		
		Ret = GuildManageClass::This()->DelMember( Member->MemberDBID );		
		//��Ʈw�R��
		DelGuildMember( PlayerDBID );
		SL_All_DelGuildMember( GuildID , PlayerName );
		SendAllGuildMember_LeaveGuildMember( GuildID , LeaderDBID , PlayerDBID );

	}

	SL_LeaveNewGuildResult( SrvSockID , LeaderDBID , PlayerName , Ret );

	
}
void CNetDC_GuildChild::RL_DelNewGuild  ( int SrvSockID , int LeaderDBID )
{
	
	//���o���|�������
	GuildMemberStruct* LeaderGuildMember = GuildManageClass::This()->GetMember( LeaderDBID );
	//Leader �S���|
	if( LeaderGuildMember == NULL )
	{
		SL_DelNewGuildResult( SrvSockID , LeaderDBID , false );
		return;
	}

	GuildStruct* Guild = LeaderGuildMember->Guild;

	if( Guild->Base.IsReady == true )
	{
		SL_DelNewGuildResult( SrvSockID , LeaderDBID , false );
		return;
	}

	int	GuildID = Guild->Base.GuildID;

	//�u���|�����v�O
	if( Guild->Base.LeaderDBID != LeaderDBID )
	{
		SL_DelNewGuildResult( SrvSockID , LeaderDBID , false );
		return;
	}

	Global::Log_GuildLog( GuildID , EM_GuildActionType_DestroyNewGuild , LeaderDBID , LeaderDBID );

	bool Ret = GuildManageClass::This()->DelGuild( GuildID );

	SL_DelNewGuildResult( SrvSockID , LeaderDBID , Ret );

	//��Ʈw�R��
	DelGuild( GuildID );
	SL_All_DelGuild( GuildID );


}
//--------------------------------------------------------------------------------------
void CNetDC_GuildChild::LocalSrvConnectProc( int SrvID )
{
	Print( LV1 , "OnLocalSrvConnect" , "SrvID =%d ~~~" , SrvID );
	//�q���s�W��Client�Ҧ����|�����
	int MaxGuild = GuildManageClass::This()->MaxGuildID();

	for( int i = 0 ; i < MaxGuild ; i++ )
	{
		GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( i );
		if( Guild == NULL )
			continue;

		Print( LV1 , "OnLocalSrvConnect" , "SL_AddGuild GuildID=%d  GuildName=%s" ,  Guild->Base.GuildID , Guild->Base.GuildName.Begin() );
		SL_AddGuild( SrvID , Guild->Base );

		//�e�����W��
		for( int j = 0 ; j < (int)Guild->Member.size() ; j++ )
		{
			Print( LV1 , "OnLocalSrvConnect" , "SL_AddGuildMember MemberDBID=%d MemberName=%s" ,  Guild->Member[j]->MemberDBID ,  Guild->Member[j]->MemberName.Begin() );
			SL_AddGuildMember( SrvID , *Guild->Member[j] );
		}
		if( Guild->Base.IsWar == true )
		{			
			//�q�����|�Ը�T
			SL_WarInfo( SrvID , true  , Guild->Base.GuildID , Guild->Base.WarGuildID , Guild->Base.WarTime - TimeStr::Now_Value() 	);
		}
	}
}
void CNetDC_GuildChild::OnLocalSrvConnect( int SrvID )
{
	_OnConnectSrvID.push_back(SrvID );
	/*
	Print( LV1 , "OnLocalSrvConnect" , "SrvID =%d ~~~" , SrvID );
	//�q���s�W��Client�Ҧ����|�����
	int MaxGuild = GuildManageClass::This()->MaxGuildID();

	for( int i = 0 ; i < MaxGuild ; i++ )
	{
		GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( i );
		if( Guild == NULL )
			continue;

		Print( LV1 , "OnLocalSrvConnect" , "SL_AddGuild GuildID=%d  GuildName=%s" ,  Guild->Base.GuildID , Guild->Base.GuildName.Begin() );
		SL_AddGuild( SrvID , Guild->Base );

		//�e�����W��
		for( int j = 0 ; j < (int)Guild->Member.size() ; j++ )
		{
			Print( LV1 , "OnLocalSrvConnect" , "SL_AddGuildMember MemberDBID=%d MemberName=%s" ,  Guild->Member[j]->MemberDBID ,  Guild->Member[j]->MemberName.Begin() );
			SL_AddGuildMember( SrvID , *Guild->Member[j] );
		}
		if( Guild->Base.IsWar == true )
		{			
			//�q�����|�Ը�T
			SL_WarInfo( SrvID , true  , Guild->Base.GuildID , Guild->Base.WarGuildID , Guild->Base.WarTime - TimeStr::Now_Value() 	);
		}
	}
*/
}
void CNetDC_GuildChild::RL_AddGuildMember	( int SrvSockID , int GuildID , char* MemberName , int MemberDBID , char* NewMemberName , int NewMemberDBID )
{
	OnlinePlayerInfoStruct* PlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( NewMemberDBID );
	if( PlayerInfo == NULL )
	{
		SL_AddGuildMemberResult( SrvSockID , MemberName ,MemberDBID , GuildID , NewMemberName , false );
		return;
	}

	GuildMemberStruct NewMember;
	NewMember.GuildID		= GuildID;
	NewMember.MemberName	= NewMemberName;
	NewMember.MemberDBID	= NewMemberDBID;
	NewMember.Rank			= EM_GuildRank_0;
	NewMember.RankTime		= TimeStr::Now_Value();
	NewMember.Voc			= PlayerInfo->Voc;
	NewMember.Voc_Sub		= PlayerInfo->Voc_Sub;
	NewMember.Sex			= PlayerInfo->Sex;
	NewMember.LV			= PlayerInfo->LV;
	NewMember.LV_Sub		= PlayerInfo->LV_Sub;
	NewMember.IsOnline		= true;

	//���o���|�������
	GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember( MemberDBID );
	if(		GuildMember == NULL 
		||	GuildMember->GuildID != GuildID 
		||	(int)GuildMember->Guild->Member.size() >= GuildMember->Guild->Base.MaxMemberCount	)
	{
		SL_AddGuildMemberResult( SrvSockID , MemberName ,MemberDBID , GuildID , NewMemberName , false );
		return;
	}
	GuildStruct* Guild = GuildMember->Guild;

	//�ˬd�O�_�i�H�۶�
	GuildRankENUM Rank = GuildMember->Rank;
	if( Guild->Base.Rank[ Rank ].Setting.Invite == false )
	{
		SL_AddGuildMemberResult( SrvSockID , MemberName ,MemberDBID , GuildID , NewMemberName , false );
		return;
	}

	
	if(	Guild->Base.IsReady == false )
	{
		SL_AddGuildMemberResult( SrvSockID , MemberName ,MemberDBID , GuildID , NewMemberName , false );
		return;
	}

	bool Ret = GuildManageClass::This()->AddMember( NewMember );

	if( Ret == true )
	{
		GuildManageClass::This()->SetMemberOnline( NewMemberDBID , PlayerInfo );
		SL_All_AddGuildMember( NewMember );
		//��Ʈw�x�s
		SaveNewGuildMember( NewMember );
		SendAllGuildMember_JoinGuildMember( NewMember.GuildID , NewMember );
		Global::Log_GuildLog( GuildID , EM_GuildActionType_Join , Guild->Base.LeaderDBID , NewMemberDBID  );
	}

	SL_AddGuildMemberResult( SrvSockID , MemberName ,MemberDBID , GuildID , NewMemberName , Ret );

}
void CNetDC_GuildChild::RL_LeaveGuild		( int SrvSockID , int LeaderDBID , char* PlayerName )
{

	//���o���|�������
	GuildMemberStruct* LeaderMember = GuildManageClass::This()->GetMember( LeaderDBID );

	//Leader �S���|
	if( LeaderMember == NULL )
	{
		SL_LeaveGuildResult( SrvSockID , LeaderDBID , PlayerName , false );
		return;
	}

	GuildStruct* Guild = LeaderMember->Guild;

	if( Guild->Base.IsReady == false )
	{
		SL_LeaveGuildResult( SrvSockID , LeaderDBID , PlayerName , false );
		return;
	}
	
	GuildMemberStruct* KickMember = GuildManageClass::This()->GetMember( Guild->Base.GuildID , PlayerName );
	if( KickMember == NULL )
	{
		SL_LeaveGuildResult( SrvSockID , LeaderDBID , PlayerName , false );
		return;
	}

	//�v���ˬd
	if( KickMember->MemberDBID != LeaderDBID )
	{
		if(		LeaderMember->Rank <= KickMember->Rank 
			||	Guild->Base.Rank[ LeaderMember->Rank ].Setting.Kick == false )
		{
			SL_LeaveGuildResult( SrvSockID , LeaderDBID , PlayerName , false );
			return;
		}
	}



	int PlayerDBID = KickMember->MemberDBID;
	int GuildID = KickMember->GuildID;

	Global::Log_GuildLog( GuildID , EM_GuildActionType_Leave , Guild->Base.LeaderDBID , PlayerDBID );


	bool Ret = GuildManageClass::This()->DelMember( PlayerDBID );		
	//��Ʈw�R��
	DelGuildMember( PlayerDBID );
	SL_All_DelGuildMember( GuildID , PlayerName );
	SL_LeaveGuildResult( SrvSockID , LeaderDBID , PlayerName , Ret );
	SendAllGuildMember_LeaveGuildMember( GuildID , LeaderDBID , PlayerDBID );


	if( Guild->Member.size() == 0 )
	{
		Global::Log_GuildLog( GuildID , EM_GuildActionType_Destroy , Guild->Base.LeaderDBID , PlayerDBID );

		Ret = GuildManageClass::This()->DelGuild( GuildID );		
		//��Ʈw�R��
		DelGuild(  GuildID );
		SL_All_DelGuild( GuildID );
		return;
	}

	//�p�G�O���|�|��
	if( Guild->Base.LeaderDBID == PlayerDBID )
	{
		//�B�z�|���~��
		int BestID = 0;
		for( int i = 0 ; i < (int)Guild->Member.size() ; i++ )
		{
			if( Guild->Member[BestID]->Rank > Guild->Member[i]->Rank )
				continue;
			if( Guild->Member[BestID]->Rank < Guild->Member[i]->Rank )
			{
				BestID = i;
				continue;
			}

			if( Guild->Member[BestID]->RankTime > Guild->Member[i]->Rank )
			{
				BestID = i;
			}
		}
		Global::Log_GuildLog( GuildID , EM_GuildActionType_ChangeLeader , Guild->Member[BestID]->MemberDBID , Guild->Base.LeaderDBID );
		//�q���Ҧ����|����?				
		SendAllGuildMember_ChangeLeader( Guild->Base.GuildID , Guild->Member[BestID]->MemberDBID , Guild->Base.LeaderDBID );

		Guild->Member[BestID]->Rank = EM_GuildRank_Leader;
		Guild->Member[BestID]->IsNeedSave = true;
	
		Guild->Base.LeaderDBID = Guild->Member[BestID]->MemberDBID;
		Guild->Base.LeaderName = Guild->Member[BestID]->MemberName;
		Guild->Base.IsNeedSave = true;

		 SL_All_ModifyGuild			( Guild->Base );
		 SL_All_ModifyGuildMember	( *(Guild->Member[BestID]) );
		

	}

}
void CNetDC_GuildChild::RL_DelGuild			( int SrvSockID , int LeaderDBID )
{
	// �Ȯɤ��ݭn  �S�k�����R�����|

	//���o���|�������
	GuildMemberStruct* LeaderGuildMember = GuildManageClass::This()->GetMember( LeaderDBID );
	//Leader �S���|
	if( LeaderGuildMember == NULL )
	{
		SL_DelNewGuildResult( SrvSockID , LeaderDBID , false );
		return;
	}

	GuildStruct* Guild = LeaderGuildMember->Guild;

/*	if( Guild->Base.IsReady == true )
	{
		SL_DelNewGuildResult( SrvSockID , LeaderDBID , false );
		return;
	}
	*/

	int	GuildID = Guild->Base.GuildID;

	//�u���|�����v�O
	if( Guild->Base.LeaderDBID != LeaderDBID )
	{
		SL_DelNewGuildResult( SrvSockID , LeaderDBID , false );
		return;
	}

	bool Ret = GuildManageClass::This()->DelGuild( GuildID );

	SL_DelNewGuildResult( SrvSockID , LeaderDBID , Ret );

	//��Ʈw�R��
	DelGuild( GuildID );
	SL_All_DelGuild( GuildID );
}

void CNetDC_GuildChild::RL_ModifyGuildInfoRequest		( int SrvSockID , int PlayerDBID , GuildBaseStruct& Guild )
{
	//���o���|�������
	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );
	if( Member == NULL )
	{
		//�䤣�즹�H
		return;
	}

	GuildStruct* OldGuild = Member->Guild;

	if( Member->GuildID != Guild.GuildID )
	{
		return;
	}

	Guild.GuildName = OldGuild->Base.GuildName;

	//�p�G�����|�|�� or �t�ΰe�X���
	//------------------------------------------------------------------------------------------------
	if( Member->Guild->Base.LeaderDBID == PlayerDBID || PlayerDBID == -1)
	{
		//���|��
		if( Guild.LeaderDBID != PlayerDBID )
		{
			GuildMemberStruct* NewLeader = GuildManageClass::This()->GetMember( Guild.LeaderDBID );
			if( NewLeader == NULL || NewLeader->MemberName != Guild.LeaderName )
			{//��Ʀ����D

			}
			else
			{
				GuildMemberStruct* OldLeader = GuildManageClass::This()->GetMember( OldGuild->Base.LeaderDBID );
				if( OldLeader != NULL )
				{

					OldLeader->Rank = EM_GuildRank_8;
					OldLeader->RankTime = TimeStr::Now_Value();
					OldLeader->IsNeedSave = true;
					SL_All_ModifyGuildMember( *OldLeader );
				}

				Global::Log_GuildLog( Guild.GuildID , EM_GuildActionType_ChangeLeader , NewLeader->MemberDBID, OldGuild->Base.LeaderDBID );
				SendAllGuildMember_ChangeLeader( Guild.GuildID , NewLeader->MemberDBID , OldGuild->Base.LeaderDBID );

				OldGuild->Base.LeaderName = NewLeader->MemberName;
				OldGuild->Base.LeaderDBID = NewLeader->MemberDBID;
				OldGuild->Base.IsNeedSave = true;

				NewLeader->Rank = EM_GuildRank_Leader;
				NewLeader->IsNeedSave = true;
				SL_All_ModifyGuildMember( *NewLeader );
				
			}
		}

		if( OldGuild->Base.IsLockGuildHouse != Guild.IsLockGuildHouse )
		{
			OldGuild->Base.IsLockGuildHouse = Guild.IsLockGuildHouse;
			OldGuild->Base.IsNeedSave = true;
		}
		if( OldGuild->Base.IsRecruit != Guild.IsRecruit )
		{
			OldGuild->Base.IsRecruit = Guild.IsRecruit;
			OldGuild->Base.IsNeedSave = true;
		}

		if( OldGuild->Base.Medal != Guild.Medal )
		{
			OldGuild->Base.Medal = Guild.Medal;
			OldGuild->Base.IsNeedSave = true;
		}

		for( int i = 0 ; i < EM_GuildRank_Count ; i++  )
		{
			if( memcmp( &OldGuild->Base.Rank[i] , &Guild.Rank[i] , sizeof(Guild.Rank[i]) ) != 0 )
			{
				memcpy( &OldGuild->Base.Rank[i] , &Guild.Rank[i]  , sizeof(Guild.Rank[i]) );
				OldGuild->Base.IsNeedSave = true;
			}
		}

		for( int i = 0 ; i < _MAX_GUILD_GROUP_COUNT_ ; i++ )
		{
			if( OldGuild->Base.Group[i] != Guild.Group[i] )
			{
				OldGuild->Base.Group[i] = Guild.Group[i];
				OldGuild->Base.IsNeedSave = true;
			}
		}

		if( Guild.Introduce != OldGuild->Base.Introduce )
		{
			OldGuild->Base.Introduce = Guild.Introduce;
			OldGuild->Base.IsNeedSave = true;
		}

		/*
		if( Guild.RankCount != OldGuild->Base.RankCount )
		{
			if( Guild.RankCount > 1 && Guild.RankCount < EM_GuildRank_Count )
			{
				OldGuild->Base.RankCount = Guild.RankCount;
				OldGuild->Base.IsNeedSave = true;
				//�j�M���|���O�_���Q����������

				for( unsigned i = 0 ; i < OldGuild->Member.size() ; i++ )
				{
					GuildMemberStruct* GuildMember = OldGuild->Member[i];
					if(		GuildMember->Rank != EM_GuildRank_Leader
						&&	GuildMember->Rank >= Guild.RankCount )
					{
						GuildMember->Rank		= (GuildRankENUM)(Guild.RankCount - 1);
						GuildMember->IsNeedSave = true;
					}
				}
			}
		}*/
		if( Guild.RankCount != 0 )
		{
			Guild.RankCount += OldGuild->Base.RankCount;
			if( Guild.RankCount > 1 && Guild.RankCount < EM_GuildRank_Count )
			{
				OldGuild->Base.RankCount = Guild.RankCount;
				OldGuild->Base.IsNeedSave = true;
				//�j�M���|���O�_���Q����������
			}
		}

		if( Guild.MaxMemberCount != 0 )
		{
			Guild.MaxMemberCount += OldGuild->Base.MaxMemberCount;
			if( Guild.MaxMemberCount > 1 && Guild.MaxMemberCount <= 300 )
			{
				OldGuild->Base.MaxMemberCount = Guild.MaxMemberCount;
				OldGuild->Base.IsNeedSave = true;
			}
		}
/*
		if( Guild.Score != 0 )
		{
			Guild.Score += OldGuild->Base.Score;
			OldGuild->Base.Score = Guild.Score;
			OldGuild->Base.IsNeedSave = true;
		}
		*/

	}
	//------------------------------------------------------------------------------------------------
	//���o�v�����
	GuildManageSettingStruct RankSetting = OldGuild->Base.Rank[ Member->Rank ].Setting;

	if( RankSetting.SetGuildNote == true )
	{
		if( Guild.Note != OldGuild->Base.Note )
		{
			OldGuild->Base.Note = Guild.Note;
			OldGuild->Base.IsNeedSave = true;
		}
	}

	//�p�G���ק�
	if( OldGuild->Base.IsNeedSave )
	{
		SL_All_ModifyGuild( OldGuild->Base );
		SC_AllMember_GuildBaseInfoUpdate( OldGuild->Base );
	}

}
void CNetDC_GuildChild::RL_ModifyGuildMemberInfoRequest( int SrvSockID , int PlayerDBID , GuildMemberStruct& NewMemberInfo )
{
	//���o���|�������
	GuildMemberStruct* ManageMember = GuildManageClass::This()->GetMember( PlayerDBID );
	if( ManageMember == NULL )
	{
		//�䤣�즹�H
		return;
	}

	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( NewMemberInfo.MemberDBID );
	if( Member == NULL )
	{
		return;
	}

	//���P���|
	if( Member->GuildID != ManageMember->GuildID )
	{
		return;
	}

	//�ۤv�~�i�H�� SelfNote
	if( Member->MemberDBID == ManageMember->MemberDBID )
	{
		if( Member->SelfNote != NewMemberInfo.SelfNote )
		{
			Member->SelfNote = NewMemberInfo.SelfNote;
			Member->IsNeedSave = true;
		}
	}

	GuildManageSettingStruct RankSetting = Member->Guild->Base.Rank[ ManageMember->Rank ].Setting;

	//�դ��|����
	if( RankSetting.AdjustRank )
	{
		if( NewMemberInfo.Rank < Member->Guild->Base.RankCount )
		{
			if( ManageMember->Rank > Member->Rank )
			{
				if( NewMemberInfo.Rank != Member->Rank )
				{
					if( ManageMember->Rank > NewMemberInfo.Rank )
					{
						Member->Rank = NewMemberInfo.Rank;
						Member->RankTime = TimeStr::Now_Value();
						Member->IsNeedSave = true;

						SendAllGuildMember_ChangeMemberRank	( Member->GuildID , Member->MemberDBID , Member->Rank );
					}
				}
			}
		}
	}
	
	if( RankSetting.AdjustGroup )
	{
		if( NewMemberInfo.GroupID != Member->GroupID )
		{
			Member->GroupID = NewMemberInfo.GroupID;
			Member->IsNeedSave = true;
		}
	}

	if( RankSetting.EditMemberNote )
	{
		if( Member->LeaderNote != NewMemberInfo.LeaderNote )
		{
			Member->LeaderNote = NewMemberInfo.LeaderNote;
			Member->IsNeedSave = true;
		}
	}
	
	if( Member->IsNeedSave )
	{
		SL_All_ModifyGuildMember( *Member );
	}

}
//--------------------------------------------------------------------------------------
void	CNetDC_GuildChild::_WarProc( GuildStruct* Guild )
{
	//�p�G���ž� 
	if( Guild->Base.IsWar == false && Guild->Base.IsWarPrepare == false )
		return;

	if( Guild->Base.IsWarPrepare == true )
	{
		if( Guild->Base.WarDeclareTime + 5*60 > (int)TimeStr::Now_Value() )
		{
			if( Guild->Base.WarDeclareTime + 4*60 <= (int)TimeStr::Now_Value() )
			{
				SC_ALLMember_PrepareWar( Guild->Base.GuildID , Guild->Base.WarGuildID , 60 );
				SC_ALLMember_PrepareWar( Guild->Base.WarGuildID , Guild->Base.GuildID , 60 );
			}
			return;
		}
		
		Guild->Base.IsWarPrepare = false;
		Guild->Base.IsWar = true;
		SL_All_WarInfo( true  , Guild->Base.GuildID , Guild->Base.WarGuildID , Guild->Base.WarTime - TimeStr::Now_Value() );
		SL_All_ModifyGuild( Guild->Base );
		SC_AllMember_GuildBaseInfoUpdate( Guild->Base );
	}
	
	if( Guild->Base.WarTime > (int)TimeStr::Now_Value() )
		return;

	GuildBaseStruct& Base = Guild->Base;

	Base.IsWar = false;
	Base.IsNeedSave = true;

	//�B�z�Ԫ����G
	GuildStruct* TargetGuild = GuildManageClass::This()->GetGuildInfo( Base.WarGuildID );

	if( TargetGuild != NULL )
	{
		if( Base.WarEnemyScore > Base.WarMyScore )
		{
			Base.WarLostCount++;
			TargetGuild->Base.WarWinCount++;
		}
		else if( Base.WarEnemyScore < Base.WarMyScore )
		{
			Base.WarWinCount++;
			TargetGuild->Base.WarLostCount++;		
		}
		else
		{
			Base.WarEvenCount++;
			TargetGuild->Base.WarEvenCount++;		
		}
		TargetGuild->Base.IsNeedSave = true;
	}
	GuildManageClass::This()->WarDeclareEarse( Base.GuildID , Base.WarGuildID );
	SL_All_WarInfo( false  , Base.GuildID , Base.WarGuildID , 0 );


	SL_All_ModifyGuild( Base );

	if( TargetGuild != NULL )
	{
		SL_All_ModifyGuild( TargetGuild->Base );
		SC_All_DeclareWarFinalResult( Base.GuildID , Base.WarGuildID , Base.WarMyScore , Base.WarEnemyScore , Base.WarMyKillCount , Base.WarEnemyKillCount );
	}

	
}
//--------------------------------------------------------------------------------------
//�x�s���|���
void CNetDC_GuildChild::SaveGuildInfo( )
{
	g_CritSect()->Enter();
	if(		_NeedUpDateGuild.size() == 0 
		&&	_NeedUpDateMember.size() == 0 )
	{
		//�j�M�ݭn�x�s�����|
		int MaxGuildID = GuildManageClass::This()->MaxGuildID();

		for( int i = 0 ; i < MaxGuildID ; i++ )
		{
			GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( i );
			if( Guild == NULL )
				continue;

			_WarProc( Guild );

			if( Guild->Base.IsNeedSave == true )
			{
				_NeedUpDateGuild.push_back( Guild->Base );
				Guild->Base.IsNeedSave = false;
			}
			//�j�M�ݭn�x�s������
			for( unsigned j = 0 ; j < Guild->Member.size() ; j++ )
			{
				GuildMemberStruct* Member = Guild->Member[j];
				if( Member->IsNeedSave == true )
				{
					_NeedUpDateMember.push_back( *Member );
					Member->IsNeedSave = false;
				}
			}
		}

		if( _NeedUpDateGuild.size() != 0 || _NeedUpDateMember.size() != 0 )
		{
			_RD_NormalDB->SqlCmd( rand() ,  _SQLCmdUpDateProc , _SQLCmdUpDateProcResult , This , NULL );
		}
	}
	g_CritSect()->Leave();
}
//�w�ɳB�z
int  CNetDC_GuildChild::_OnTimeProc( SchedularInfo* Obj , void* InputClass )
{
	static int LoopCount = 0;
	LoopCount++;

	if( Global::_IsDestory == true )
		return 0;

	if( _OnConnectSrvID.size() )
	{
		LocalSrvConnectProc( ((CNetDC_GuildChild*)This)->_OnConnectSrvID.front() );
		_OnConnectSrvID.pop_front();
	}
	Schedular()->Push( _OnTimeProc , 1000 , This , NULL );  

	if( LoopCount < 60 )
		return 0;
	LoopCount = 0;
	SaveGuildInfo( );

	return 0;
}

//--------------------------------------------------------------------------------------
//�w�ɳB�z
int  CNetDC_GuildChild::_OnGuildFightBidTimeProc( SchedularInfo* Obj , void* InputClass )
{
	Schedular()->Push( _OnGuildFightBidTimeProc , 60000 , This , NULL );
	
	map< int, StructCrystal >::iterator it;

	SYSTEMTIME st;
	GetLocalTime( &st );              // gets current time
	
	int iTimeNowVal			= st.wSecond + ( st.wMinute * 60 ) + ( st.wHour * 3600 ) + ( st.wDayOfWeek * 86400 );

	for( it = m_mapCrystal.begin(); it != m_mapCrystal.end(); it++ )
	{
		int				iCrystalID	= it->first;
		StructCrystal	Crystal		= it->second;


		switch( Crystal.emStatus )
		{
		case EM_StructCrystal_WaitBidEnd:	// �����v�е����q����
			{
				int iEndTime = Crystal.iEndTime;

				if( Crystal.iAdditionEndTime != 0 )
				{
					iEndTime = Crystal.iAdditionEndTime;
				}

				if( iEndTime <= iTimeNowVal )
				{
					// �Ӥ����ɶ���, ����
					it->second.emStatus = EM_StructCrystal_WaitFight;

					// �o�����ӬO�o���Ӥ��|, �����H, ���ե� At

					// �q��Ĺ�a ( �����̤��| )
					SC_GuildFightAuctionEnd( Crystal.iAttacker, iCrystalID, Crystal.iDefender, Crystal.iAttacker, Crystal.iFightTime, Crystal.iBidAmount );



					// �q�����ê̤��|
					SC_GuildFightAuctionEnd( Crystal.iDefender, iCrystalID, Crystal.iDefender, Crystal.iAttacker, Crystal.iFightTime, Crystal.iBidAmount );
				}
			} break;

		case EM_StructCrystal_WaitFight:	// ���ݤ��|�Զ}�l
			{
				int iFightTime	= Crystal.iFightTime;
				

				if( iFightTime <= iTimeNowVal )
				{
					it->second.emStatus = EM_StructCrystal_Fighting;
				}
				else
				{
					int iRemainTime	= iFightTime - iTimeNowVal;
					if( iRemainTime < 300 )
					{
						// �o�e�����䤽�|, �i�����|�ԧY�N�󤭤�����}�l
						//SC_GuildFightBeginNotify( Member, iRemainTime );
						SendGuildFightBeginNotify( iCrystalID, Crystal.iDefenderGuildID, Crystal.iDefenderGuildID, iRemainTime );
					}

				}

				// �q�� BattleGround ZoneSrv �إߪŶ�, �θ��
				if( Crystal.iFightServerID != 0 )
					SL_CreateGuildFightRoom( Crystal.iFightServerID, iCrystalID, Crystal.iDefenderGuildID, Crystal.iAttackerGuildID );
				

			} break;
		}
	}

	return 0;
}
//--------------------------------------------------------------------------------------
void CNetDC_GuildChild::RL_SetGuildReady		( int GuildID )
{
	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( Guild == NULL )
		return;

	if( Guild->Base.IsReady == true )
		return;
	
	if( g_ObjectData->GuildLvTable().size() < 2 )
		return;

	GuildLvInfoStruct& GuildInfo =g_ObjectData->GuildLvTable()[1];


	Guild->Base.IsReady		= true;
	Guild->Base.IsNeedSave	= true;
	Guild->Base.Level		= 1;
	Guild->Base.IsEnabledGuildBoard = true;

	Guild->Base.MaxMemberCount = GuildInfo.MemberCount;
	Guild->Base.RankCount	= GuildInfo.RankCount;
	SC_ALLPlayer_GuildCreate( GuildID );
	SL_All_ModifyGuild( Guild->Base );
	SC_AllMember_GuildBaseInfoUpdate( Guild->Base );

	Global::Log_GuildLog( GuildID , EM_GuildActionType_RegularGuild , Guild->Base.LeaderDBID , Guild->Base.LeaderDBID );
}
//--------------------------------------------------------------------------------------
//��Ʈw�B�z
//--------------------------------------------------------------------------------------
//��l���J (**���ɨ�L������� ���i�H�s�� GuildManageClass�����**)
bool CNetDC_GuildChild::_SQLCmdInitProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass	MyDBProc( sqlBase );

	//���J�Ҧ������|���
	GuildBaseStruct	TempGuildBase;
	//g_CritSect()->Enter();
	MyDBProc.SqlCmd( _RDGuildBaseSql->GetSelectStr("WHERE DestroyTime=0").c_str() );
	//Print( LV1 , "_SQLCmdInitProc", _RDGuildBaseSql->GetSelectStr("") );
	//g_CritSect()->Leave();

	MyDBProc.Bind( TempGuildBase , RoleDataEx::ReaderRD_GuildBase() );

	int iReadCount = 0;
	while( MyDBProc.Read() )
	{
		iReadCount++;
		RoleDataEx::ReaderRD_GuildBase()->TransferWChartoUTF8( &TempGuildBase , MyDBProc.wcTempBufList() );
		GuildManageClass::This()->CreateGuild( TempGuildBase );
	}
	MyDBProc.Close();


	//���J�Ҧ������|����
	GuildMemberStruct	TempGuildMember;
	MyDBProc.SqlCmd( _RDGuildMemberSql->GetSelectStr("WHERE GuildID > 0 and IsDelete = 0").c_str() );

	MyDBProc.Bind( TempGuildMember , RoleDataEx::ReaderRD_GuildMember() );
	while( MyDBProc.Read() )
	{
		RoleDataEx::ReaderRD_GuildMember()->TransferWChartoUTF8( &TempGuildMember , MyDBProc.wcTempBufList() );
		if( GuildManageClass::This()->AddMember( TempGuildMember ) == false )
		{
			Print( LV1, "_SQLCmdInitProc", "guildmember (%s) cant join guild (%d) " , TempGuildMember.MemberName.Begin() , TempGuildMember.GuildID );

		}
	}
	MyDBProc.Close();


	
	int MaxGuildID = GuildManageClass::This()->MaxGuildID();
	Print( LV1, "_SQLCmdInitProc", "MaxGuildID() Count = %d\n", MaxGuildID );

	for( int i = 0 ; i < MaxGuildID ; i++ )
	{
		GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( i );
		if( Guild == NULL )
			continue;

		if( Guild->Base.IsWar == true )
		{
			GuildManageClass::This()->WarDeclareInsert( Guild->Base.GuildID , Guild->Base.WarGuildID );
		}

	}

	//���J���|�d���O���
	GuildBoardStruct	TempMessage;

	MyDBProc.SqlCmd( _RDGuildBoardSql->GetSelectStr("").c_str() );

	MyDBProc.Bind( TempMessage , RoleDataEx::ReaderRD_GuildBoard() );
	while( MyDBProc.Read() )
	{
		RoleDataEx::ReaderRD_GuildBoard()->TransferWChartoUTF8( &TempMessage , MyDBProc.wcTempBufList() );
		if( GuildManageClass::This()->AddBoardMessage( TempMessage , false ) == false )
		{
			printf( "_SQLCmdInitProc �d���[�J�����D" );
		}
	}
	MyDBProc.Close();


	//���J���|�Ծ��v����
	{
		GuildHouseWarHistoryStruct WarInfo;

		MyDBProc.SqlCmd( _RDGuildHouseWarHistorySql->GetSelectStr("").c_str() );

		MyDBProc.Bind( WarInfo , RoleDataEx::ReaderRD_GuildHouseWarHistory() );
		while( MyDBProc.Read() )
		{
			RoleDataEx::ReaderRD_GuildHouseWarHistory()->TransferWChartoUTF8( &WarInfo , MyDBProc.wcTempBufList() );
			GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( WarInfo.GuildID );
			if( Guild )
				Guild->GuildWarHistory.push_back( WarInfo );
		}
		MyDBProc.Close();

	}

	return true;
}
void CNetDC_GuildChild::_SQLCmdInitProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	_IsLoadGuildOK = true;
}
//��s�x�s
bool CNetDC_GuildChild::_SQLCmdUpDateProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char Buf[256];
	MyDbSqlExecClass	MyDBProc( sqlBase );

	//���|����x�s
	//g_CritSect()->Enter();
	for( unsigned i = 0 ; i < _NeedUpDateGuild.size() ; i++ )
	{
		GuildBaseStruct* Data = &_NeedUpDateGuild[i];

		sprintf( Buf , "where GUID = %d" , Data->GuildID );
		MyDBProc.SqlCmd_WriteOneLine( _RDGuildBaseSql->GetUpDateStr( Data , Buf ).c_str() );
	}

	for( unsigned i = 0 ; i < _NeedUpDateMember.size() ; i++ )
	{
		GuildMemberStruct* Data = &_NeedUpDateMember[i];

		sprintf( Buf , "where DBID = %d" , Data->MemberDBID );
		MyDBProc.SqlCmd_WriteOneLine( _RDGuildMemberSql->GetUpDateStr( Data , Buf ).c_str() );
	}
	//g_CritSect()->Leave();

	return true;
}
void CNetDC_GuildChild::_SQLCmdUpDateProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	g_CritSect()->Enter();
	_NeedUpDateGuild.clear();
	_NeedUpDateMember.clear();
	g_CritSect()->Leave();
}
/*
//���|�s�W
bool CNetDC_GuildChild::_SQLCmdNewGuildProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	GuildBaseStruct* NewGuild = (GuildBaseStruct*)UserObj;
	MyDbSqlExecClass	MyDBProc( sqlBase );

	g_CritSect()->Enter();
	MyDBProc.SqlCmd_WriteOneLine(  _RDGuildBaseSql->GetInsertStr( NewGuild ) );
	g_CritSect()->Leave();

	return true;
}
void CNetDC_GuildChild::_SQLCmdNewGuildProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	GuildBaseStruct* NewGuild = (GuildBaseStruct*)UserObj;
	delete NewGuild;
}
*/
//���|�����s�W
bool CNetDC_GuildChild::_SQLCmdNewGuildMemberProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	GuildMemberStruct* NewGuildMember = (GuildMemberStruct*)UserObj;
	MyDbSqlExecClass	MyDBProc( sqlBase );

	char Buf[256];
	sprintf( Buf ,  "Where DBID = %d" , NewGuildMember->MemberDBID  );

	//g_CritSect()->Enter();
		MyDBProc.SqlCmd_WriteOneLine(  _RDGuildMemberSql->GetUpDateStr( NewGuildMember , Buf ).c_str() );
	//g_CritSect()->Leave();

	return true;
}
void CNetDC_GuildChild::_SQLCmdNewGuildMemberProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	GuildMemberStruct* NewGuildMember = (GuildMemberStruct*)UserObj;
	delete NewGuildMember;
}
//���|�R��
bool CNetDC_GuildChild::_SQLCmdDelGuildProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char	Buf[512];
	MyDbSqlExecClass	MyDBProc( sqlBase );

	int	GuildID = Arg.GetNumber( "GuildID" );

	//sprintf( Buf , "Delete Guild_Base Where GUID = %d" , GuildID );
	sprintf( Buf , "UPDATE Guild_Base SET DestroyTime = getdate() Where GUID = %d" , GuildID );
	MyDBProc.SqlCmd_WriteOneLine(  Buf );

	//��Ҧ��������ݩ󦹤��|���M��
	sprintf( Buf , "UPDATE RoleData Set GuildID = 0 , GuildRank = 0 , GuildGroupID = 0 , GuildRankTime = 0 , GuildSelfNote = '' , GuildLeaderNote = ''  Where GuildID = %d" , GuildID );
	MyDBProc.SqlCmd_WriteOneLine(  Buf );

	return true;
}
void CNetDC_GuildChild::_SQLCmdDelGuildProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{

}
//���|�����R��
bool CNetDC_GuildChild::_SQLCmdDelGuildMemberProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char	Buf[512];
	MyDbSqlExecClass	MyDBProc( sqlBase );

	int	PlayerDBID = Arg.GetNumber( "PlayerDBID" );

	//sprintf( Buf , "UPDATE RoleData Set GuildID = 0 , GuildRank = 0 , GuildGroupID = 0 , GuildRankTime = 0 , GuildSelfNote = '' , GuildLeaderNote = ''  Where DBID = %d" , PlayerDBID );
	sprintf( Buf , "UPDATE RoleData Set GuildID = 0 , GuildRank = 0 , GuildGroupID = 0 , GuildRankTime = 0 , GuildSelfNote = '' , GuildLeaderNote = ''  , GuildLeaveTime = getdate() Where DBID = %d" , PlayerDBID );
	MyDBProc.SqlCmd_WriteOneLine(  Buf );

	return true;
}
void CNetDC_GuildChild::_SQLCmdDelGuildMemberProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
}
//--------------------------------------------------------------------------------------
//��Ʈw�x�s �s�W���|
/*
void CNetDC_GuildChild::SaveNewGuild( GuildBaseStruct& Guild )
{
	GuildBaseStruct* NewGuild = NEW(GuildBaseStruct);
	*NewGuild = Guild;
	_RD_NormalDB->SqlCmd( Guild.GuildID ,  _SQLCmdNewGuildProc , _SQLCmdNewGuildProcResult , NewGuild , NULL );
}
*/
//��Ʈw�x�s �s�W���|����
void CNetDC_GuildChild::SaveNewGuildMember( GuildMemberStruct& Member )
{
	GuildMemberStruct* NewGuildMember = NEW(GuildMemberStruct);
	*NewGuildMember = Member;
	_RD_NormalDB->SqlCmd( Member.MemberDBID ,  _SQLCmdNewGuildMemberProc , _SQLCmdNewGuildMemberProcResult , NewGuildMember , NULL );
}

//��Ʈw�x�s �R�����|
void CNetDC_GuildChild::DelGuild( int GuildID )
{
	_RD_NormalDB->SqlCmd( GuildID ,  _SQLCmdDelGuildProc , _SQLCmdDelGuildProcResult , NULL 
							,"GuildID"		, EM_ValueType_Int , GuildID
							, NULL );
}
//��Ʈw�x�s �R�����|����
void CNetDC_GuildChild::DelGuildMember( int PlayerDBID )
{
	_RD_NormalDB->SqlCmd( PlayerDBID ,  _SQLCmdDelGuildMemberProc , _SQLCmdDelGuildMemberProcResult , NULL 
						,"PlayerDBID"		, EM_ValueType_Int , PlayerDBID
						, NULL );
}

//���|�T���s�W
bool CNetDC_GuildChild::_SQLCmdNewMessageProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	GuildBoardStruct* Message = (GuildBoardStruct*)UserObj;

	MyDbSqlExecClass	MyDBProc( sqlBase );

	//g_CritSect()->Enter();
	MyDBProc.SqlCmd_WriteOneLine(  _RDGuildBoardSql->GetInsertStr( Message ).c_str() );
	//g_CritSect()->Leave();

	return true;
}
void CNetDC_GuildChild::_SQLCmdNewMessageProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	GuildBoardStruct* Msg = (GuildBoardStruct*)UserObj;

	delete Msg;
}

//���|�T���R��
bool CNetDC_GuildChild::_SQLCmdDelMessageProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{	
	int GUID	= Arg.GetNumber( "GUID" );
	int GuildID = Arg.GetNumber( "GuildID" );

	char	Buf[256];
	MyDbSqlExecClass	MyDBProc( sqlBase );

//	MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );
	sprintf( Buf , "Delete Guild_Board Where GUID = %d and GuildID =%d" , GUID , GuildID );
	MyDBProc.SqlCmd_WriteOneLine(  Buf );

//	sprintf( Buf , "update Guild_Board Set GUID = GUID - 1 WHERE GUID > %d and GuildID = %d" , GUID , GuildID );
//	MyDBProc.SqlCmd_WriteOneLine(  Buf );

//	MyDBProc.SqlCmd_WriteOneLine( "COMMIT TRANSACTION" );

	return true;
}
void CNetDC_GuildChild::_SQLCmdDelMessageProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{

}

//���|�T���ק�
bool CNetDC_GuildChild::_SQLCmdUpdateMessageProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	GuildBoardStruct* Msg = (GuildBoardStruct*)UserObj;

	char Buf[256];
	MyDbSqlExecClass	MyDBProc( sqlBase );

	//���|����x�s
	//g_CritSect()->Enter();
	sprintf( Buf , "where GUID = %d and GuildID = %d " , Msg->GUID , Msg->GuildID );
	MyDBProc.SqlCmd_WriteOneLine( _RDGuildBoardSql->GetUpDateStr( Msg , Buf ).c_str() );
	//g_CritSect()->Leave();

	return true;
}
void CNetDC_GuildChild::_SQLCmdUpdateMessageProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	GuildBoardStruct* Message = (GuildBoardStruct*)UserObj;


	delete Message;
}
//--------------------------------------------------------------------------------------------
//��Ʈw �s�W�T��
void CNetDC_GuildChild::SaveGuildMessage( GuildBoardStruct& Msg )
{
	GuildBoardStruct* Message = NEW(GuildBoardStruct);
	*Message = Msg;
	_RD_NormalDB->SqlCmd( Msg.GUID ,  _SQLCmdNewMessageProc , _SQLCmdNewMessageProcResult , Message 
		, NULL );
}

//��Ʈw �R���T��
void CNetDC_GuildChild::DelGuildMessage( int GuildID , int GUID )
{
	//_RD_NormalDB->SqlCmd_Transcation( GUID ,  _SQLCmdDelMessageProc , _SQLCmdDelMessageProcResult , NULL
	_RD_NormalDB->SqlCmd( GUID ,  _SQLCmdDelMessageProc , _SQLCmdDelMessageProcResult , NULL
		,"GuildID"	, EM_ValueType_Int , GuildID
		,"GUID"		, EM_ValueType_Int , GUID
		, NULL );
}

//��Ʈw �ק�T��
void CNetDC_GuildChild::UpdateGuildMessage( GuildBoardStruct& Msg  )
{
	GuildBoardStruct* Message = NEW(GuildBoardStruct);
	*Message = Msg;
	_RD_NormalDB->SqlCmd( Msg.GUID ,  _SQLCmdUpdateMessageProc , _SQLCmdUpdateMessageProcResult , Message 
		, NULL );
}
//--------------------------------------------------------------------------------------------------------------------
//�d���O
//--------------------------------------------------------------------------------------------------------------------
void CNetDC_GuildChild::RL_BoardPostRequest			( int PlayerDBID , GuildBoardStruct& Message )
{
	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );
	if( Member == NULL )
	{
		SC_BoardPostResult( PlayerDBID , false );
		return;
	}

	GuildStruct* Guild = Member->Guild;

	if( Guild->Base.IsReady == false )
	{
		SC_BoardPostResult( PlayerDBID , false );
		return;
	}

	GuildRankENUM Rank = Member->Rank;
	if( Member->Guild->Base.Rank[ Rank ].Setting.PostMessage == false )
	{
		SC_BoardPostResult( PlayerDBID , false );
		return;
	}


	if( Guild->BoardAll.size() == 0 )
		Message.GUID = 0;
	else
		Message.GUID = Guild->BoardAll.back()->GUID + 1;

	Message.GuildID = Member->GuildID;
	Message.PlayerName = Member->MemberName;
	Message.Type._Type = 0;
	Message.Time		= TimeStr::Now_Value();

	//�R���d��
	while( Guild->BoardList.size() >= 60 )
	{
		DelGuildMessage( Guild->BoardList.back()->GuildID , Guild->BoardList.back()->GUID );
		Guild->BoardList.pop_back();
	}

	if( GuildManageClass::This()->AddBoardMessage( Message , true ) == false )
	{
		SC_BoardPostResult( PlayerDBID , false );
		return;
	}

	SaveGuildMessage( Message );
	GuildInfoChangeStruct ChangeMode;
	ChangeMode.GuildMessage = true;
	SC_GuildInfoChange	( Message.GuildID , ChangeMode );
	SC_BoardPostResult	( PlayerDBID , true );
}
void CNetDC_GuildChild::RL_BoardListRequest			( int PlayerDBID , int PageID )
{
	OnlinePlayerInfoStruct* PlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( PlayerDBID );
	if( PlayerInfo == NULL )
		return;

	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );
	if( Member == NULL )
	{
		SC_BoardListCount( PlayerDBID , 0 , 0 , PageID );
		return;
	}

	int Count = int( Member->Guild->BoardList.size() ) - PageID * 10;
	if( Count <= 0  )
	{
		SC_BoardListCount( PlayerDBID , int( Member->Guild->BoardList.size() ) , 0 , PageID );
		return;
	}

	if( Count >= 10 )
		Count = 10;

	SC_BoardListCount( PlayerDBID , int( Member->Guild->BoardList.size() ) , Count , PageID );

	int ID  = PageID * 10;

	for( int i = 0 ; i < Count ; i++ , ID++)
	{
		
		GuildBoardStruct* Msg = Member->Guild->BoardList[ID];
		SC_BoardListData( PlayerDBID , i , *Msg );
	}

}
void CNetDC_GuildChild::RL_BoardMessageRequest		( int PlayerDBID , int MessageGUID )
{
	OnlinePlayerInfoStruct* PlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( PlayerDBID );
	if( PlayerInfo == NULL )
		return;

	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );
	if( Member == NULL )
		return;

	GuildBoardStruct* Message = GuildManageClass::This()->GetMessage( Member->GuildID , MessageGUID );
	if( Message == NULL )
		return;

	SC_BoardMessageResult( PlayerDBID , *Message );

}
void CNetDC_GuildChild::RL_BoardModifyMessageRequest( int PlayerDBID , int MessageGUID, char* MessageStr )
{
	OnlinePlayerInfoStruct* PlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( PlayerDBID );
	if( PlayerInfo == NULL )
		return;

	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );
	if( Member == NULL )
	{
		SC_BoardModifyMessageResult( PlayerDBID , false );
		return;
	}

	GuildBoardStruct* Message = GuildManageClass::This()->GetMessage( Member->GuildID , MessageGUID );
	if( Message == NULL )
	{
		SC_BoardModifyMessageResult( PlayerDBID , false );
		return;
	}

	if( Message->PlayerName != Member->MemberName  )
	{
		SC_BoardModifyMessageResult( PlayerDBID , false );
		return;
	}
	Message->Message = MessageStr;

	SC_BoardModifyMessageResult( PlayerDBID , true );
	UpdateGuildMessage( *Message );
}
void CNetDC_GuildChild::RL_BoardModifyModeRequest	( int PlayerDBID , int MessageGUID , GuildBoardModeStruct	Mode )
{
	OnlinePlayerInfoStruct* PlayerInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( PlayerDBID );
	if( PlayerInfo == NULL )
		return;

	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );
	if( Member == NULL )
	{
		SC_BoardModifyModeResult( PlayerDBID , false );
		return;
	}

	GuildBoardStruct* Message = GuildManageClass::This()->GetMessage( Member->GuildID , MessageGUID );
	if( Message == NULL )
	{
		SC_BoardModifyModeResult( PlayerDBID , false );
		return;
	}

	GuildRankENUM Rank = Member->Rank;
	if( Member->Guild->Base.Rank[ Rank ].Setting.ManageMessage == false )
	{
		SC_BoardModifyModeResult( PlayerDBID , false );
		return;
	}

	if( Mode.Del )
	{
		if( GuildManageClass::This()->DelBoardMessage( Member->GuildID , MessageGUID ) == false ) 
		{
			SC_BoardModifyModeResult( PlayerDBID , false );
			return;
		}

		DelGuildMessage( Member->GuildID , MessageGUID );
	}
	else if( Mode.Top != Message->Type.Top )
	{
		if( Member->Guild->MessageSetTop( MessageGUID , Mode.Top ) == false )
		{
			SC_BoardModifyModeResult( PlayerDBID , false );
			return;
		}
		UpdateGuildMessage( *Message );
	}

	GuildInfoChangeStruct ChangeMode;
	ChangeMode.GuildMessage = true;
	SC_GuildInfoChange	( Member->GuildID , ChangeMode );

	SC_BoardModifyModeResult( PlayerDBID , true );

}

void CNetDC_GuildChild::SendAllGuildMember_JoinGuildMember				( int GuildID , GuildMemberStruct& Member )
{
	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( GuildInfo == NULL )
		return;

	SC_JoinGuildMember( Member.MemberDBID , Member );

	for( unsigned int i = 0 ;i < GuildInfo->Member.size() ; i++ )
	{
		GuildMemberStruct& OtherMember = *GuildInfo->Member[i];
		if( OtherMember.MemberDBID == Member.MemberDBID )
			continue;
		SC_JoinGuildMember( Member.MemberDBID , OtherMember );
		SC_JoinGuildMember( OtherMember.MemberDBID , Member );
	}
}
void CNetDC_GuildChild::SendAllGuildMember_LeaveGuildMember				( int GuildID , int LeaderDBID , int MemberDBID )
{
	SC_LeaveGuildMember( MemberDBID , LeaderDBID , MemberDBID );

	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( GuildInfo == NULL )
		return;
	
	for( unsigned int i = 0 ;i < GuildInfo->Member.size() ; i++ )
	{
		GuildMemberStruct& OtherMember = *GuildInfo->Member[i];
		SC_LeaveGuildMember( OtherMember.MemberDBID , LeaderDBID , MemberDBID );
	}
}
void CNetDC_GuildChild::SendAllGuildMember_ChangeLeader				( int GuildID , int NewLeaderDBID , int OldLeaderDBID )
{
	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( GuildInfo == NULL )
		return;

	for( unsigned int i = 0 ;i < GuildInfo->Member.size() ; i++ )
	{
		GuildMemberStruct& OtherMember = *GuildInfo->Member[i];
		SC_GuildLeaderChange( OtherMember.MemberDBID , NewLeaderDBID , OldLeaderDBID );
	}
}

void CNetDC_GuildChild::SendAllGuildMember_ChangeMemberRank				( int GuildID , int MemberDBID , int Rank )
{
	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( GuildInfo == NULL )
		return;

	for( unsigned int i = 0 ;i < GuildInfo->Member.size() ; i++ )
	{
		GuildMemberStruct& OtherMember = *GuildInfo->Member[i];
		SC_GuildMemberRankChange( OtherMember.MemberDBID , MemberDBID , Rank );
	}
}
//////////////////////////////////////////////////////////////////////////
/*
void CNetDC_GuildChild::RL_BuyFunction					( int ServerID , int GuildID , int PlayerDBID , int Score , int Type , int Value )
{
	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( GuildInfo == NULL )
		return;

	if( GuildInfo->Base.Score < Score )
		return;

	switch( Type )
	{
	case 0:
		break;
	case 1:
		break;
	}
	GuildInfo->Base.Score -= Score;

	SL_All_ModifyGuild			( GuildInfo->Base );
}
*/
void CNetDC_GuildChild::RL_AddGuildResource				( int ServerID , int GuildID , int PlayerDBID , GuildResourceStruct& Resource , const char* RetPlot , AddGuildResourceTypeENUM LogType , int LogType_ObjID )
{
	bool IsNeedLog = false;
	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( GuildInfo == NULL )
	{
		SL_AddGuildResourceResult( ServerID , GuildID , PlayerDBID , Resource , RetPlot , false );
		return;
	}
	//check
	for( int i = 0 ; i < 7 ; i++ )
	{
		if( Resource._Value[i] != 0 )
			IsNeedLog = true;
		if( GuildInfo->Base.Resource._Value[i] + Resource._Value[i] < 0 )
		{
			SL_AddGuildResourceResult( ServerID , GuildID , PlayerDBID , Resource , RetPlot , false );
			return;
		}
	}

	for( int i = 0 ; i < 7 ; i++ )
	{
		GuildInfo->Base.Resource._Value[i] += Resource._Value[i];
	}
	GuildInfo->Base.IsNeedSave = true;
	//SL_All_ModifyGuild			( GuildInfo->Base );
	SL_All_ModifySimpleGuild( GuildInfo->Base );
	SC_AllMember_ModifySimpleGuild( GuildInfo->Base );

	if( IsNeedLog == true )
		Global::GuildResourceLog( GuildID , PlayerDBID , Resource , LogType , LogType_ObjID);
//	Global::Log_GuildLog( GuildID , EM_GuildActionType_AddScore , GuildInfo->Base.LeaderDBID , PlayerDBID  );

}
/*
void CNetDC_GuildChild::RL_GuildShopBuy				( int PlayerDBID , int FlagID )
{
	GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember( PlayerDBID );
	if( GuildMember == NULL )
		return;
	GuildStruct* Guild = GuildMember->Guild;
	vector<GuildShopInfoStruct>& GuildShop = g_ObjectData->GuildShop();
	if( Guild->Base.LeaderDBID != PlayerDBID || (unsigned)FlagID >= GuildShop.size() )
	{
		SC_GuildShopBuyResult( PlayerDBID , FlagID , false );
		return;
	}

	GuildShopInfoStruct& ShopFlagInfo = GuildShop[ FlagID ];
	//�n������
	if(		ShopFlagInfo.Cost > Guild->Base.Score 
		||	Guild->Base.Flag.GetFlag(FlagID)		== true )
	{
		SC_GuildShopBuyResult( PlayerDBID , FlagID , false );
		return;		
	}
	
	if( ShopFlagInfo.NeedFlagID != -1 && Guild->Base.Flag.GetFlag( ShopFlagInfo.NeedFlagID ) == false )
	{
		SC_GuildShopBuyResult( PlayerDBID , FlagID , false );
		return;		
	}

	if( Guild->Base.Flag.SetFlagOn( FlagID ) == false )
	{
		SC_GuildShopBuyResult( PlayerDBID , FlagID , false );
		return;		
	}

	Guild->Base.MaxMemberCount += ShopFlagInfo.MemberCount;
	Guild->Base.RankCount += ShopFlagInfo.RankCount;
	Guild->Base.Score -= ShopFlagInfo.Cost;
	
	if( ShopFlagInfo.EnabledGuildBoard == true 	)
		Guild->Base.IsEnabledGuildBoard = true;

	if( EM_GuildRank_Count <  Guild->Base.RankCount )
		Guild->Base.RankCount = EM_GuildRank_Count;
	
	Guild->Base.IsNeedSave = true;

	SC_GuildShopBuyResult	( PlayerDBID , FlagID , true );

	SL_All_ModifyGuild		( Guild->Base );

//	Global::Log_GuildLog( Guild->Base.GuildID , EM_GuildActionType_BuyFuntion , Guild->Base.LeaderDBID , PlayerDBID , ShopFlagInfo.Cost*-1 , Guild->Base.Score , FlagID );
}
*/
void CNetDC_GuildChild::RL_GuildFightAuctionBid		( int iBidderDBID , int CrystalID , int BidAmount )
{
	// �����, �ˬd�O�_�٦b�ɶ���, ( �g�| �ߤW 20:00 - 22:00 ), �C���U�Ы�, �Y�ɶ����� 3min, �h�W�[ 3min
	// �קK�h�H�P���v��, ���ˬd�U�Ъ��B, �O�_�W�L�쥻���B
	// �Ҽ{�q���e�@�ӤU�Ъ̤@�ӯS���ʥ], �����i�H���ξa�� NPC, �����ι�ܲ�����, �l�[�U��

	//int WeekDays	= 4;
	//int Hrs			
	
	StructCrystal		CrystalInfo;

	int					LastBidder			= 0;	// �W�@����Ъ̳ӧQ��
	int					LastBidderGuildID	= 0;	// �W�@����Ъ̳ӧQ�̤��|
	//int OwnerGuildID		= 0;	// ���ê̤��|

	int					Defender			= 0;	// ���ê̤��|�|�����v�Ъ�
	int					DefenderGuildID		= 0;	// ���ê̤��|

	//int Bidder				= 0;	// �ثe�n�D�v�Ъ�
	int					BidderGuildID		= 0;	// �ثe�v�Ъ̤��|
	int					BidderGuildPoint	= 0;	// �ثe�v�Ъ̪����|�n��;

	int					HighestAmount		= 0;	// �ثe�v�Ъ��B
	int					RemainTime			= 0;

	GuildStruct*		pBidderGuild		= NULL;
	GuildStruct*		pLastBidderGuild	= NULL;
	GuildMemberStruct*	GuildMember			= NULL;
	
	// ��X�e�@�� Bidder
		map< int, StructCrystal >::iterator it = m_mapCrystal.find( CrystalID );
		if( it == m_mapCrystal.end() )
		{
			LastBidderGuildID = 0;
			Print( LV3 , "RL_GuildFightAuctionBid" , "�S�����|���v�Ф������( %d ), Bidder( %d)", CrystalID, iBidderDBID );
			return;	// �S�����, �O���Q���\��
		}
		else
		{
			CrystalInfo					= it->second;

			Defender					= CrystalInfo.iDefender;
			DefenderGuildID				= CrystalInfo.iDefenderGuildID;

			LastBidder					= CrystalInfo.iAttacker;
			LastBidderGuildID			= CrystalInfo.iAttackerGuildID;

			HighestAmount				= CrystalInfo.iBidAmount;
		}


	// ��X�ثe�U�Ъ̪����|
		/*
		GuildMember = GuildManageClass::This()->GetMember( iBidderDBID );

		if( GuildMember == NULL || GuildMember->Guild == NULL)
			return;

		pBidderGuild		= GuildMember->Guild;
		BidderGuildID		= pGuild->Base.GuildID;
		BidderGuildPoint	= pGuild->Base.Score;
		*/

	// ��X�t�ήɶ�
		SYSTEMTIME st;
		GetLocalTime( &st );              // gets current time

		int iTimeNowVal			= st.wSecond + ( st.wMinute * 60 ) + ( st.wHour * 3600 ) + ( st.wDayOfWeek * 86400 );
		int	iStartTime			= CrystalInfo.iStartTime;
		int iEndTime			= CrystalInfo.iEndTime;
		int iAdditionEndTime	= 0;

		if( CrystalInfo.iAdditionEndTime != 0 )
		{
			iEndTime = CrystalInfo.iAdditionEndTime;
		}

		RemainTime = GetRemainTime( iStartTime, iEndTime );

		if( RemainTime < 180 )
		{
			iAdditionEndTime = iTimeNowVal + 180;
			int iTotalAdditionTime = iAdditionEndTime - CrystalInfo.iEndTime;
			if( iTotalAdditionTime > 3600 )
			{
				RemainTime = 0;
			}
		}

	// �ˬd�Ѿl�ɶ�
		if( RemainTime == 0 )
		{
			SC_GuildFightAuctionBidResult( iBidderDBID, CrystalID, DefenderGuildID, LastBidderGuildID, HighestAmount, EM_GuildFightAuctionBidResult_Timeout );
			return;
		}
 
	// �ˬd�U�Ъ��B�O�_�W�L�쥻���B
		if( BidAmount <= HighestAmount )	
		{
			SC_GuildFightAuctionBidResult( iBidderDBID, CrystalID, DefenderGuildID, LastBidderGuildID, HighestAmount, EM_GuildFightAuctionBidResult_Toolow );
			return;
		}

	// �ˬd�U�Ъ̪����|�Z���O�_����
		/*
		if( BidderGuildPoint < BidAmount )
		{
			SC_GuildFightAuctionBidResult( iBidderDBID, CrystalID, DefenderGuildID, LastBidderGuildID, HighestAmount, EM_GuildFightAuctionBidResult_Toolow );
			return;
		}
		*/

	// �ˬd�U�Ъ̪��䥦�������p
		for( map< int, StructCrystal >::iterator it = m_mapCrystal.begin(); it != m_mapCrystal.end(); it++ )
		{
			int				ID		= it->first;
			StructCrystal	Info	= it->second;

			if( Info.iAttackerGuildID == BidderGuildID && ID != CrystalID )
			{
				SC_GuildFightAuctionBidResult( iBidderDBID, CrystalID, DefenderGuildID, LastBidderGuildID, HighestAmount, EM_GuildFightAuctionBidResult_Bidding );
				return;
			}

			if( Info.iDefenderGuildID == BidderGuildID && Info.iDefenderGuildID != 0 )
			{
				SC_GuildFightAuctionBidResult( iBidderDBID, CrystalID, DefenderGuildID, LastBidderGuildID, HighestAmount, EM_GuildFightAuctionBidResult_Exist );
				return;
			}
		}


	// �����v�Ъ̤��|�Z��, ���k�٤W���v�Ъ̤��|�Z��
		/*
		pLastBidderGuild = GuildManageClass::This()->GetGuildInfo( LastBidderGuildID );
		if( pLastBidderGuild != NULL )
		{
			pLastBidderGuild->Base.Score	+= HighestAmount;
			SL_All_ModifyGuild			( pLastBidderGuild->Base );
		}

		if( pBidderGuild != NULL )
		{
			pBidderGuild->Base.Score		-= BidAmount;
			SL_All_ModifyGuild			( pBidderGuild->Base );
		}
		*/

	// �g�J�O����, �αN�v�е��G�g�J��Ʈw
		CrystalInfo.iAdditionEndTime	= iAdditionEndTime;
		CrystalInfo.iBidAmount			= BidAmount;

		CrystalInfo.iAttacker			= iBidderDBID;
		CrystalInfo.iAttackerGuildID	= BidderGuildID;
	
		m_mapCrystal[ CrystalID ] = CrystalInfo;	
		WriteCrystalInfoToDB( CrystalID, CrystalInfo.iAttacker, CrystalInfo.iDefender, CrystalInfo.iBidAmount, CrystalInfo.iAdditionEndTime );

	 
		SC_GuildFightAuctionBidResult( iBidderDBID, CrystalID, DefenderGuildID, BidderGuildID, HighestAmount, EM_GuildFightAuctionBidResult_Success );

	// �q���e�@��U�Ъ�
		SC_GuildFightAuctionBidNewOne( LastBidder, CrystalID, DefenderGuildID, BidderGuildID, HighestAmount, RemainTime );

}

void CNetDC_GuildChild::RL_GuildFightInfoRequest ( int iRequestPlayerDBID, int iCrystalID )
{ 
	// �]�w���|�v�Ъ�����, ���ݦP�@��, ��軡�g�|, �����ɶ����ݦb 24:00 �e

	// ���쪱�a�n�D�ثe�v�и��, �ˬd�v�Ъ��p, �ثeĹ�a, �ثe�v�Ъ��B

	// �ˬd�O�_�W�L�v�Юɶ�

	// �쥻�n�ˬd�v�Ъ̬O�_, �֦��ΰѻP�䥦�������v��, �b��U�Ы�A����ܿ��~

	int							PlayerGuildID	= 0;
	int							OtherCrystalID	= 0;
	int							GuildPoint		= 0;

	int							LastBidGuildID	= 0;
	int							LastBidAmount	= 0;
	int							RemainTime		= 0;
	int							FightTime		= 0;
	EM_GuildFightInfo_Result	emResult		= EM_GuildFightInfo_Result_Okay;

	StructCrystal				CrystalInfo;

	// ��X�v�и��
		map< int, StructCrystal >::iterator it = m_mapCrystal.find( iCrystalID );
		if( it == m_mapCrystal.end() )
		{
			Print( LV3 , "RL_GuildFightInfoRequest" , "�S�����|���v�Ф������( %d ), Bidder( %d)", iCrystalID, iRequestPlayerDBID );
			return;	// �S�����, �O���Q���\��

		}
		else
		{
			CrystalInfo		= it->second;
			LastBidAmount	= CrystalInfo.iBidAmount;
			LastBidGuildID	= CrystalInfo.iAttackerGuildID;
			FightTime		= CrystalInfo.iFightTime;
		}

	// �ˬd�d�ߪ̤��|���
		/*
		GuildMemberStruct*	GuildMember = GuildManageClass::This()->GetMember( iRequestPlayerDBID );
		GuildStruct*		pGuild		= NULL;

		if( GuildMember == NULL || GuildMember->Guild == NULL )
			return;

		pGuild			= GuildMember->Guild;
		PlayerGuildID	= pGuild->Base.GuildID;
		GuildPoint		= pGuild->Base.Score;
		*/
	

	// �ˬd�d�ߪ̬O�_�֦��ΰѻP�䥦�������v��
		for( map< int, StructCrystal >::iterator it = m_mapCrystal.begin(); it != m_mapCrystal.end(); it++ )
		{
			int				ID		= it->first;
			StructCrystal	Info	= it->second;

			if( Info.iAttackerGuildID == PlayerGuildID && ID != iCrystalID && Info.iAttackerGuildID != 0 )
			{
				emResult		= EM_GuildFightInfo_Result_Bidding;
				OtherCrystalID	= ID;
				break;
			}

			if( Info.iDefenderGuildID == PlayerGuildID && Info.iDefenderGuildID != 0 )
			{
				emResult		= EM_GuildFightInfo_Result_Exist;
				OtherCrystalID	= ID;
				break;
			}
		}

	// �ˬd�v�Юɶ�
	// ���X�t�ήɶ���, �p��X�ƭȫ��� , �̤j�� 694860

		SYSTEMTIME st;
		GetLocalTime( &st );              // gets current time

		int iTimeNowVal			= st.wSecond + ( st.wMinute * 60 ) + ( st.wHour * 3600 ) + ( st.wDayOfWeek * 86400 );
		int	iStartTime			= CrystalInfo.iStartTime;
		int iEndTime			= CrystalInfo.iEndTime;
		int iAdditionEndTime	= 0;

		if( CrystalInfo.iAdditionEndTime != 0 )
		{
			iEndTime = CrystalInfo.iAdditionEndTime;
		}

		RemainTime = GetRemainTime( iStartTime, iEndTime );

		if( RemainTime == 0 )
		{
			emResult = EM_GuildFightInfo_Result_Timeout;
		}


	// �o�e���n�D��
		SC_GuildFightInfo( iRequestPlayerDBID, iCrystalID, CrystalInfo.iDefenderGuildID, LastBidGuildID, LastBidAmount, RemainTime, FightTime, OtherCrystalID, GuildPoint, emResult );


	// �ƥ��
	// �ƥ�N�X / �}�l��� ( 1-7 ) / �}�l�ɶ� ( 0000 ) / ������� ( 1-7 ) / �����ɶ� ( 0000 )

	// �v�ХN�X
	// �����N�X / �ثe�֦��� / �D�Ԫ� ( �v�Ъ̤]�O��o��� ) / �ثe�v�ХX��


}

//------------------------------------------------------------------------------------------------------------------
int CNetDC_GuildChild::GetRemainTime( int iStartTime, int iEndTime )
{
	int	RemainTime = 0;
	SYSTEMTIME st;
	GetLocalTime( &st );              // gets current time

	int iTimeNowVal			= st.wSecond + ( st.wMinute * 60 ) + ( st.wHour * 3600 ) + ( st.wDayOfWeek * 86400 );

	if( !( iTimeNowVal >= iStartTime && iTimeNowVal <= iEndTime ))
	{
		RemainTime = 0; // �ˬd  �ˬd�O�_�٦b�ɶ���, ( �g�| �ߤW 20:00 - 22:00 )
	}
	else
	{
		RemainTime = iEndTime - iTimeNowVal;
	}

	return RemainTime;
}

//------------------------------------------------------------------------------------------------------------------
void CNetDC_GuildChild::WriteCrystalInfoToDB( int iCrystalID, int iAttacker, int iDefender, int iBidAmount, int iAdditionTime )
{
	// �g�J��ƨ� DB

}
//------------------------------------------------------------------------------------------------------------------
void CNetDC_GuildChild::SendGuildFightBeginNotify( int iCrystalID, int iDefenderGuildID, int iAttackerGuildID, int iRemainTime )
{
	if( iDefenderGuildID != 0 )
	{
		GuildStruct* pGuild = GuildManageClass::This()->GetGuildInfo( iDefenderGuildID );

		if( pGuild != NULL )
		{
			for( vector<GuildMemberStruct*>::iterator it = pGuild->Member.begin(); it != pGuild->Member.end(); it++ )
			{
				GuildMemberStruct* pMember = *it;
				if( pMember )
				{
					SC_GuildFightBeginNotify( pMember->MemberDBID, iCrystalID, iRemainTime );
				}
			}
		}
	}

	if( iAttackerGuildID != 0 )
	{
		GuildStruct* pGuild = GuildManageClass::This()->GetGuildInfo( iAttackerGuildID );

		if( pGuild != NULL )
		{
			for( vector<GuildMemberStruct*>::iterator it = pGuild->Member.begin(); it != pGuild->Member.end(); it++ )
			{
				GuildMemberStruct* pMember = *it;
				if( pMember )
				{
					SC_GuildFightBeginNotify( pMember->MemberDBID, iCrystalID, iRemainTime );
				}
			}
		}
	}


}

void CNetDC_GuildChild::RL_DeclareWar	( int ServerID , int Type , int PlayerDBID , int TargetGuildID )
{
	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );
	if( Member == NULL )
	{
		SL_DeclareWarResult( ServerID , Type , PlayerDBID , -1 , TargetGuildID , EM_DeclareWarResult_DataErr );
		return;
	}

	GuildStruct* Guild = Member->Guild;

	int		PKTime;
	int		PKMoney;

	g_ObjectData->GetGuildWarDeclareTypeInfo( Type , PKTime , PKMoney );
	if( PKTime == 0 )
	{
		SL_DeclareWarResult( ServerID , Type , PlayerDBID , Guild->Base.GuildID , TargetGuildID , EM_DeclareWarResult_DataErr );
		return;
	}

	if( Guild->Base.Resource.Gold < PKMoney )
	{
		SL_DeclareWarResult( ServerID , Type , PlayerDBID , Guild->Base.GuildID , TargetGuildID , EM_DeclareWarResult_MonyeErr );
		return;
	}

	if( Guild->Base.IsWar == true )
	{
		SL_DeclareWarResult( ServerID , Type , PlayerDBID , Guild->Base.GuildID , TargetGuildID , EM_DeclareWarResult_OnWarErr );
		return;
	}

	if( Guild->Base.IsWarPrepare == true )
	{
		SL_DeclareWarResult( ServerID , Type , PlayerDBID , Guild->Base.GuildID , TargetGuildID , EM_DeclareWarResult_OnWarPrepareErr );
		return;
	}

	if( Guild->Base.WarTime + 30*60 >  (int)TimeStr::Now_Value() )
	{
		SL_DeclareWarResult( ServerID , Type , PlayerDBID , Guild->Base.GuildID , TargetGuildID , EM_DeclareWarResult_TimeErr );
		return;
	}

	if( GuildManageClass::This()->IsWarDeclare( Guild->Base.GuildID , TargetGuildID ) == true  )
	{
		SL_DeclareWarResult( ServerID , Type , PlayerDBID , Guild->Base.GuildID , TargetGuildID , EM_DeclareWarResult_OnWarErr );
		return;
	}

	Guild->Base.IsWar = false;
	Guild->Base.IsWarPrepare = true;
	Guild->Base.WarGuildID = TargetGuildID;
	Guild->Base.WarMyScore = 0;
	Guild->Base.WarEnemyScore = 0;
	Guild->Base.WarMyKillCount = 0;
	Guild->Base.WarEnemyKillCount = 0;
	Guild->Base.WarTime	= TimeStr::Now_Value() + PKTime * 60 + 5*60;
//	Guild->Base.Score -= PKScore;
	Guild->Base.Resource.Gold -= PKMoney;
	Guild->Base.IsNeedSave = true;
	Guild->Base.WarDeclareTime = TimeStr::Now_Value();

	GuildManageClass::This()->WarDeclareInsert( Guild->Base.GuildID , TargetGuildID );

	SL_All_ModifyGuild( Guild->Base );
	SC_AllMember_GuildBaseInfoUpdate( Guild->Base );

	SL_DeclareWarResult( ServerID , Type , PlayerDBID , Guild->Base.GuildID , TargetGuildID , EM_DeclareWarResult_OK );
	//SL_All_WarInfo( true  , Guild->Base.GuildID , TargetGuildID , PKTime*60*60 );
	SC_ALLMember_PrepareWar( Guild->Base.GuildID , TargetGuildID , 5*60 );
	SC_ALLMember_PrepareWar( TargetGuildID , Guild->Base.GuildID , 5*60 );
}

void CNetDC_GuildChild::RL_DeclareWarScore			( int GuildID[2] , int Score , int KillCount )
{
	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildID[0] );
	if( Guild == NULL || Guild->Base.IsWar == false || Guild->Base.WarGuildID != GuildID[1] )
	{
		Guild = GuildManageClass::This()->GetGuildInfo( GuildID[1] );
		if( Guild == NULL || Guild->Base.IsWar == false || Guild->Base.WarGuildID != GuildID[0] )
		{
			//�S����Ԫ����|
			return;
		}
	}

	if( Guild->Base.GuildID == GuildID[0] )
	{
		Guild->Base.WarMyKillCount += KillCount;
		Guild->Base.WarMyScore += Score;
	}
	else
	{
		Guild->Base.WarEnemyKillCount += KillCount;
		Guild->Base.WarEnemyScore += Score;
	}
	Guild->Base.IsNeedSave = true;
}

//���|�X�]�w
void CNetDC_GuildChild::RL_SetGuildFlagInfoRequest( int PlayerDBID , GuildFlagStruct& Flag )
{
	int SetFlagCost = g_ObjectData->GetSysKeyValue( "Guild_SetFlagCost" );

	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );
	if( Member == NULL )
	{
		SC_SetGuildFlagInfoResult( PlayerDBID , EM_SetGuildFlagInfoResult_GuildError );
		return;
	}

	GuildStruct* Guild = Member->Guild;

	//�ˬd�O�_�����|�|��
	if( Guild->Base.LeaderDBID != PlayerDBID )
	{		
		SC_SetGuildFlagInfoResult( PlayerDBID , EM_SetGuildFlagInfoResult_LeaderError );
		return;
	}

	if( Guild->Base.Resource.Gold < SetFlagCost )
	{		
		SC_SetGuildFlagInfoResult( PlayerDBID , EM_SetGuildFlagInfoResult_ScoreError );
		return;
	}

	Guild->Base.Resource.Gold -= SetFlagCost;
	Guild->Base.GuildFlag = Flag;
	Guild->Base.IsNeedSave = true;
	SC_ALL_GuildBaseInfo( Guild->Base );
	SL_All_ModifyGuild( Guild->Base );
	SC_AllMember_GuildBaseInfoUpdate( Guild->Base );
	SC_SetGuildFlagInfoResult( PlayerDBID , EM_SetGuildFlagInfoResult_OK );
}

//////////////////////////////////////////////////////////////////////////
//���|��W
//////////////////////////////////////////////////////////////////////////
struct ChangeGuildNameRequestTempStruct
{
	int		GuildID;
	int		PlayerDBID;
	string	GuildName;
};

void CNetDC_GuildChild::RL_SetNeedChangeName		( int GuildID )
{
	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( GuildInfo == NULL )
		return;

	GuildInfo->Base.IsNeedChangeName = true;
}
void CNetDC_GuildChild::RL_ChangeGuildNameRequest	( int GuildID , int PlayerDBID , char* GuildName )
{
	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );
	if( Member == NULL )
	{
		SC_ChangeGuildNameResult	( PlayerDBID , EM_ChangeGuildNameResult_UnknowErr );
		return;
	}

	GuildStruct* Guild = Member->Guild;

	//�ˬd�O�_�����|�|��
	if( Guild->Base.LeaderDBID != PlayerDBID )
	{		
		SC_ChangeGuildNameResult	( PlayerDBID , EM_ChangeGuildNameResult_NotLeader );
		return;
	}

	if( Guild->Base.IsNeedChangeName == false  )
	{		
		SC_ChangeGuildNameResult	( PlayerDBID , EM_ChangeGuildNameResult_FlagError );
		return;
	}

	//CharacterNameRulesENUM  Ret = g_ObjectData->CheckCharacterNameRules( GuildName , Global::_LanguageType );
	std::wstring outStrName;
	CharacterNameRulesENUM  Ret = g_ObjectData->CheckCharacterNameRules( GuildName , Global::_CountryType , outStrName );
	if( Ret != EM_CharacterNameRules_Rightful )
	{

		switch( Ret )
		{
		case EM_CharacterNameRules_Short:				// �W�٤ӵu,���X�k
			SC_ChangeGuildNameResult	( PlayerDBID , EM_ChangeGuildNameResult_NameToShort );
			break;
		case EM_CharacterNameRules_Long:				// �W�٤Ӫ�,���X�k
			SC_ChangeGuildNameResult	( PlayerDBID , EM_ChangeGuildNameResult_NameToLong );
			break;
		case EM_CharacterNameRules_Wrongful:			// �����~���Ÿ��Φr��
			SC_ChangeGuildNameResult	( PlayerDBID , EM_ChangeGuildNameResult_NameIsWrongful );
			break;
		case EM_CharacterNameRules_Special:				// �S��W�٤����
			SC_ChangeGuildNameResult	( PlayerDBID , EM_ChangeGuildNameResult_HasSpecialString );
			break;
		case EM_CharacterNameRules_BasicLetters:		// �u��ϥΰ򥻭^��r��
			SC_ChangeGuildNameResult	( PlayerDBID , EM_ChangeGuildNameResult_OnlyBasicLetters );
			break;
		case EM_CharacterNameRules_EndLetter:			// �����r�����~(',")
			SC_ChangeGuildNameResult	( PlayerDBID , EM_ChangeGuildNameResult_EndLetterError );
			break;
		}
		return;
	}


	Guild->Base.IsNeedChangeName = false;

	ChangeGuildNameRequestTempStruct* Temp = NEW(ChangeGuildNameRequestTempStruct);
	Temp->GuildID =GuildID;
	Temp->GuildName = WCharToUtf8( outStrName.c_str() ).c_str();;
	Temp->PlayerDBID = PlayerDBID;

	_RD_NormalDB->SqlCmd( GuildID ,  _SQLChangeGuildNameProc , _SQLChangeGuildNameProcResult , Temp , NULL );

	/*
	Guild->Base.IsNeedChangeName = false;
	Guild->Base.GuildName = GuildName;

	Guild->Base.IsNeedSave = true;
	SC_ALL_GuildBaseInfo( Guild->Base );
	SL_All_ModifyGuild( Guild->Base );
	SC_SetGuildFlagInfoResult( PlayerDBID , EM_SetGuildFlagInfoResult_OK );
	*/
}

//��s���|�W��
bool CNetDC_GuildChild::_SQLChangeGuildNameProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	ChangeGuildNameRequestTempStruct* UserData = (ChangeGuildNameRequestTempStruct*)UserObj;


	char	SqlCmd[512];

	MyDbSqlExecClass	MyDBProc( sqlBase );

	CharTransferClass	CharTransfer;
	CharTransfer.SetUtf8String( UserData->GuildName.c_str() );

	string GuildNameBinStr = StringToSqlX( (char*)CharTransfer.GetWCString() , CharTransfer.WCStrLen() * 2 , false );

	int		Count = 0;
	//�ˬd���|�W�٬O�_���_
	sprintf( SqlCmd , "SELECT COUNT(GUID) AS Expr1 FROM Guild_Base WHERE (GuildName = CAST( %s AS nvarchar(4000) )) ", GuildNameBinStr.c_str()  );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Count );
	MyDBProc.Read();
	MyDBProc.Close();

	if( Count != 0 )
		return false;

	sprintf( SqlCmd , "UPDATE Guild_Base Set GuildName = CAST( %s AS nvarchar(4000))  WHERE GUID = %d" , GuildNameBinStr.c_str() , UserData->GuildID );
	MyDBProc.SqlCmd_WriteOneLine( SqlCmd );

	return true;

}

void CNetDC_GuildChild::_SQLChangeGuildNameProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	ChangeGuildNameRequestTempStruct* UserData = (ChangeGuildNameRequestTempStruct*)UserObj;

	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( UserData->PlayerDBID );
	if( Member == NULL )
	{
		SC_ChangeGuildNameResult	( UserData->PlayerDBID , EM_ChangeGuildNameResult_UnknowErr );
		return;
	}

	GuildStruct* Guild = Member->Guild;

	if( ResultOK == false )
	{
		Guild->Base.IsNeedChangeName = true;

		SC_ChangeGuildNameResult( UserData->PlayerDBID , EM_ChangeGuildNameResult_NameUsed );
	}
	else
	{
		Guild->Base.GuildName = UserData->GuildName.c_str();
		Guild->Base.IsNeedSave = true;

		SL_All_ModifyGuild( Guild->Base );
		SC_AllMember_GuildBaseInfoUpdate( Guild->Base );
		SC_ChangeGuildNameResult( UserData->PlayerDBID , EM_ChangeGuildNameResult_OK );
		SC_ALL_ChangeGuildName( Guild->Base.GuildID , Guild->Base.GuildName.Begin() );
	}
	delete UserData;
}
//////////////////////////////////////////////////////////////////////////
//���|�ɯ�
//////////////////////////////////////////////////////////////////////////
void CNetDC_GuildChild::RL_GuildUpgradeRequest( int GuildID , int PlayerDBID , int GuildLv )
{
	vector< GuildLvInfoStruct >& GuildLvTable = g_ObjectData->GuildLvTable();
	
	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );
	if( Member == NULL )
	{
		SC_GuildUpgradeResult	( PlayerDBID , GuildLv , false );
		return;
	}

	GuildStruct* Guild = Member->Guild;	
	if(		Guild->Base.Level + 1 != GuildLv 
		||	(int)GuildLvTable.size() <= GuildLv 
		||	Guild->Base.LeaderDBID != PlayerDBID 
		||	Guild->Base.IsReady	== false )
	{
		SC_GuildUpgradeResult	( PlayerDBID , GuildLv , false );
		return;
	}

	//�ˬd�O�_���������귽
	for( int i = 0 ; i < 7 ; i++ )
	{
		if( GuildLvTable[GuildLv]._UpgradeValue[i] > Guild->Base.Resource._Value[i] )
		{
			SC_GuildUpgradeResult	( PlayerDBID , GuildLv , false );
			return;
		}
	}

	for( int i = 0 ; i < 7 ; i++ )
	{
		Guild->Base.Resource._Value[i] -= GuildLvTable[GuildLv]._UpgradeValue[i];
	}
	Guild->Base.Level++;	
	Guild->Base.MaxMemberCount = GuildLvTable[GuildLv].MemberCount;
	Guild->Base.RankCount	   = GuildLvTable[GuildLv].RankCount;
	
	Guild->Base.IsNeedSave = true;
	SL_All_ModifyGuild( Guild->Base );
	SC_AllMember_GuildBaseInfoUpdate( Guild->Base );
	SC_GuildUpgradeResult	( PlayerDBID , GuildLv , true );
	SC_AllMember_Guild_LevelUp( Guild->Base.GuildID , GuildLv );
}

struct TempSelectContributionLogStruct
{
	int PlayerDBID;
	int GuildID;
	int DayBefore;

	vector<ContributionInfoLogStruct> LogList;
};

void CNetDC_GuildChild::RL_SelectContributionLog( int PlayerDBID , int DayBefore )
{
	GuildMemberStruct* Member = GuildManageClass::This()->GetMember( PlayerDBID );
	if( Member == NULL )
	{
		vector<ContributionInfoLogStruct> EmptyLogList;
		SC_SelectContributionLogResult( PlayerDBID , EmptyLogList );
		return;
	}
	
	GuildStruct* Guild = Member->Guild;	
	TempSelectContributionLogStruct* TempData = NEW(TempSelectContributionLogStruct);
	TempData->GuildID = Guild->Base.GuildID;
	TempData->PlayerDBID = PlayerDBID;
	TempData->DayBefore = DayBefore;
	_RD_NormalDB->SqlCmd( 0 ,  _SQLCmdSelectResourceLog , _SQLCmdSelectResourceLogResult , TempData , NULL );
}

bool CNetDC_GuildChild::_SQLCmdSelectResourceLog( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass	MyDBProc( sqlBase );
	TempSelectContributionLogStruct* TempData = (TempSelectContributionLogStruct*)UserObj;
	
	//int Time = TimeStr::Now_Value()/(24*60*60) - TempData->DayBefore;
	int	Time = ( TimeStr::Now_Value() + 60*60*RoleDataEx::G_TimeZone ) / (60*60*24) - TempData->DayBefore;

	ContributionInfoLogStruct TempLog;
	TempLog.Time = Time;
	
	char	SqlCmd[2048];
	sprintf( SqlCmd , "SELECT PlayerDBID,GuildStone, GuildRune, Herb, Wood, Mine, BonusGold, Gold  FROM Guild_ContributionLog WHERE Time=%d and GuildID=%d"
		, Time , TempData->GuildID		);
	MyDBProc.SqlCmd( SqlCmd );

	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempLog.PlayerDBID );
	MyDBProc.Bind( 2, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempLog.Resource.GuildStone );
	MyDBProc.Bind( 3, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempLog.Resource.GuildRune );
	MyDBProc.Bind( 4, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempLog.Resource.Herb );
	MyDBProc.Bind( 5, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempLog.Resource.Wood );
	MyDBProc.Bind( 6, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempLog.Resource.Mine );
	MyDBProc.Bind( 7, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempLog.Resource.BonusGold );
	MyDBProc.Bind( 8, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempLog.Resource.Gold );

	while( MyDBProc.Read() )
		TempData->LogList.push_back( TempLog );
	MyDBProc.Close();

	return true;
}
void CNetDC_GuildChild::_SQLCmdSelectResourceLogResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempSelectContributionLogStruct* TempData = (TempSelectContributionLogStruct*)UserObj;

	SC_SelectContributionLogResult( TempData->PlayerDBID , TempData->LogList );
	delete TempData;
}

void CNetDC_GuildChild::RL_GuildWarEndResult( int GuildID , GameResultENUM GameResult , int GuildScore , GuildWarBaseInfoStruct& TargetGuild )
{
	static int adjustGuildWarTime = Ini()->Int( "AdjustGuildWarTime" );
	GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildID );
	if( Guild == NULL )
	{
		Print( LV4 , "CNetDC_GuildChild::RL_GuildWarEndResult"  , "Guild == NULL GuildID = %d" , GuildID );
		return;
	}


	Guild->Base.HousesWar.Score += GuildScore;
	switch( GameResult )
	{
	case EM_GameResult_Win:
		Guild->Base.HousesWar.WinCount++;
		break;
	case EM_GameResult_Lost:
		Guild->Base.HousesWar.LostCount++;
		break;
	case EM_GameResult_Even:
		Guild->Base.HousesWar.EvenCount++;
		break;
	}

	if( TargetGuild.IsAssignment  == false )
	{
		int NextWarTime = ( RoleDataEx::G_Now / (60*60) + RoleDataEx::G_TimeZone + 21  )/24* ( 24 * 60 * 60 ) - RoleDataEx::G_TimeZone * 60 * 60;	//�Ԫ��ɶ�
		//�p��U�����|�Ԯɶ�
		Guild->Base.HousesWar.NextWarTime = NextWarTime;
	}

	//war log
	{
		GuildHouseWarHistoryStruct WarInfo;
		WarInfo.GuildID = GuildID;
		WarInfo.GuildScore = Guild->Base.HousesWar.Score;
		WarInfo.Result = GameResult;
		WarInfo.EndFightTime = RoleDataEx::G_Now_Float;
		WarInfo.TargetGuildID = TargetGuild.Guild;
		WarInfo.TargetGuildName = TargetGuild.GuildName;
		WarInfo.TargetGuildScore = TargetGuild.Score;
		Global::_RD_NormalDB->SQLCMDWrite( GuildID , NULL , NULL , (char*)_RDGuildHouseWarHistorySql->GetInsertStr(&WarInfo).c_str() );
		Guild->GuildWarHistory.push_back( WarInfo );
		if( Guild->GuildWarHistory.size() > 50 )
		{
			Guild->GuildWarHistory.pop_front();
			char sqlCmd[512];
			sprintf( sqlCmd , "DELETE TOP (1) FROM  GuildHouseWar_History WHERE GuidlD = %d" , GuildID );
			Global::_RD_NormalDB->SQLCMDWrite( GuildID , NULL , NULL , sqlCmd );
		}
	}


	Guild->Base.IsNeedSave = true;
	SL_All_ModifyGuild( Guild->Base );
}
//�n�D���|�Ծ��v���
void CNetDC_GuildChild::RL_GuildWarHistroy( int ServerID , int playerDBID )
{
	//���o���|�������
	GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember( playerDBID );
	//Leader �S���|
	if( GuildMember == NULL )
	{		
		return;
	}

	GuildStruct* Guild = GuildMember->Guild;
	SC_GuildWarHistroyResult( playerDBID , Guild->GuildWarHistory );

}