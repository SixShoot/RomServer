#include "GuildManage.h"
#include "../Global.h"
#ifdef _DATACENTER_SERVER_
#include "../../NetWalker_Member/NetWakerDataCenterInc.h"
#endif
//////////////////////////////////////////////////////////////////////////
GuildManageClass*		GuildManageClass::_This = NULL;

bool GuildManageClass::Init( )
{
	if( _This != NULL )
		return false;
	_This = NEW( GuildManageClass );

	return true;
}

bool GuildManageClass::Release( )
{
	if( _This == NULL )
		return false;

	delete _This;
	_This = NULL;

	return true;
}
//////////////////////////////////////////////////////////////////////////
GuildManageClass::GuildManageClass()
{
	//0 ��ID���ϥ�
	_GuildList.push_back( NULL );
}
GuildManageClass::~GuildManageClass()
{
	Clear();

}

void	GuildManageClass::Clear()
{
	for( int i = 0 ; i < (int)_GuildList.size() ; i++ )
	{
		if( _GuildList[i] != NULL )
		{
			_GuildList[i]->ClearMemory();
			delete _GuildList[i];
		}
	}

	_GuildList.clear();
	_GuildNameMap.clear();
	_MemberDBIDMap.clear();
	_DeclareWarSet.clear();
}

GuildStruct*	GuildManageClass::CreateGuild( GuildBaseStruct& Base )
{
	//���|ID �����D
	if( Base.GuildID <= 0 )
		return NULL;

	GuildStruct* GuildInfo;
	GuildInfo = GetGuildInfo( Base.GuildID );
	//���|�w�s�b
	if( GuildInfo != NULL )
		return NULL;

	GuildInfo = GetGuildInfo( Base.GuildName.Begin() );
	//���|�W�٭��_
	if( GuildInfo != NULL )
		return NULL;

	if( (unsigned)Base.Level < g_ObjectData->GuildLvTable().size() )
	{
		GuildLvInfoStruct& Info = g_ObjectData->GuildLvTable()[ Base.Level ];
		Base.MaxMemberCount = Info.MemberCount;
		Base.RankCount = Info.RankCount;
		if( Base.Level > 0 )
			Base.IsEnabledGuildBoard = true;
	}
	
	

	GuildStruct* NewGuildInfo = NEW(GuildStruct);
	NewGuildInfo->Base = Base;
	NewGuildInfo->Base.IsNeedSave = false;

	while( (int)_GuildList.size() <= Base.GuildID )
	{
		_GuildList.push_back( NULL );
	}
	_GuildList[ Base.GuildID ] = NewGuildInfo;

	_GuildNameMap[ Base.GuildName.Begin() ] = NewGuildInfo;

	return NewGuildInfo;
}
/*
GuildStruct*	GuildManageClass::CreateGuild( char* GuildName )
{
	GuildStruct* GuildInfo;

	GuildInfo = GetGuildInfo( GuildName );
	//���|�W�٭��_
	if( GuildInfo != NULL )
		return NULL;

	int  EmptyID = 0;
	//��Ŧ�
	for( unsigned i = 1 ; i < _GuildList.size() ; i++ )
	{
		if( _GuildList[i] == NULL )
		{
			EmptyID = (int)i;
			break;
		}
	}
	if( EmptyID == 0 )
	{
		EmptyID = (int)_GuildList.size();
		_GuildList.push_back( NULL );

	}

	GuildStruct* NewGuildInfo = NEW(GuildStruct);
	NewGuildInfo->Base.GuildID = -1;
	NewGuildInfo->Base.GuildName = GuildName;
	NewGuildInfo->Base.Rank[EM_GuildRank_Leader].Setting.Type = -1;
	NewGuildInfo->Base.RankCount = 1;
	NewGuildInfo->Base.Rank[EM_GuildRank_Leader].RankName	= "�|��";
	NewGuildInfo->Base.Rank[EM_GuildRank_0].RankName		= "�@��|��";

//	_GuildList[ EmptyID ] = NewGuildInfo;
	_GuildNameMap[ GuildName ] = NewGuildInfo;


	
	return NewGuildInfo;
}

//�]�w���|dbid ���p
bool	GuildManageClass::SetGuildDBID( GuildStruct* Guild )
{
	if( Guild->Base.GuildID <= 0 )
		return false;

	while( (int)_GuildList.size() <= Guild->Base.GuildID )
	{
		_GuildList.push_back( NULL )
	}
	_GuildList[ Guild->Base.GuildID ] = Guild;

	return true;
}
*/
//�R�����|
bool	GuildManageClass::DelGuild( int GuildID )
{
	GuildStruct* GuildInfo;

	GuildInfo = GetGuildInfo( GuildID );
	if( GuildInfo == NULL )
		return false;

	//�R�����|����
	for( int i = 0 ; i < (int) GuildInfo->Member.size() ; i++ )
	{
		_MemberDBIDMap.erase( GuildInfo->Member[i]->MemberDBID );
		delete GuildInfo->Member[i];
	}
	GuildInfo->Member.clear();

	if( _GuildNameMap.erase( GuildInfo->Base.GuildName.Begin() ) == 0 )
		return false;

#ifdef _DATACENTER_SERVER_
	if( GuildInfo->Base.IsWar != false )
	{
		WarDeclareEarse( GuildInfo->Base.GuildID , GuildInfo->Base.WarGuildID );
		CNetDC_GuildChild::SL_All_WarInfo( false  , GuildInfo->Base.GuildID , GuildInfo->Base.WarGuildID , 0 );
		CNetDC_GuildChild::SC_All_DeclareWarFinalResult( GuildInfo->Base.GuildID , GuildInfo->Base.WarGuildID , GuildInfo->Base.WarMyScore , GuildInfo->Base.WarEnemyScore , GuildInfo->Base.WarMyKillCount , GuildInfo->Base.WarEnemyKillCount );
	}
#endif



	delete GuildInfo;
	_GuildList[ GuildID ] = NULL;



	return true;
}

bool	GuildManageClass::AddMember( GuildMemberStruct& Member )
{
	GuildStruct* GuildInfo;
	GuildInfo = GetGuildInfo( Member.GuildID );
	if( GuildInfo == NULL )
		return false;

	GuildMemberStruct* AddMember = NEW(GuildMemberStruct);
	*AddMember = Member;
	AddMember->Guild	= GuildInfo;
	AddMember->IsOnline = false;

	GuildInfo->Member.push_back( AddMember );

	//�߬d�O�_�t�J�����D
	if( _MemberDBIDMap.find( Member.MemberDBID ) != _MemberDBIDMap.end() )
	{
		return false;
	}

	_MemberDBIDMap[ Member.MemberDBID ] = AddMember;

	return true;
}

//�䦨����DBID
GuildMemberStruct* GuildManageClass::GetMember( int GuildID , char* MemberName )
{
	map< int , GuildMemberStruct* >::iterator Iter;
	GuildStruct* GuildInfo;
	GuildInfo = GetGuildInfo( GuildID );
	if( GuildInfo == NULL )
		return NULL;

	for( int i = 0 ; i < (int) GuildInfo->Member.size() ; i++ )
	{
		GuildMemberStruct* OtherMember =  GuildInfo->Member[i];

		if( strcmp( OtherMember->MemberName.Begin() , MemberName ) == 0 )
		{
			
			return OtherMember;
		}
	}
	return NULL;
}

bool	GuildManageClass::DelMember( int GuildID , char* MemberName )
{
	map< int , GuildMemberStruct* >::iterator Iter;
	GuildStruct* GuildInfo;
	GuildInfo = GetGuildInfo( GuildID );
	if( GuildInfo == NULL )
		return false;

	for( int i = 0 ; i < (int) GuildInfo->Member.size() ; i++ )
	{
		GuildMemberStruct* OtherMember =  GuildInfo->Member[i];

		if( strcmp( OtherMember->MemberName.Begin() , MemberName ) == 0 )
		{
			Iter = _MemberDBIDMap.find( OtherMember->MemberDBID );
			if( Iter == _MemberDBIDMap.end() )
			{
				return false;
			}

			_MemberDBIDMap.erase( Iter );
			delete OtherMember;
			GuildInfo->Member.erase( GuildInfo->Member.begin() + i );
			
			return true;
		}
	}

	return false;
}

//�R������
bool	GuildManageClass::DelMember( int PlayerDBID )
{
	map< int , GuildMemberStruct* >::iterator Iter;
	Iter = _MemberDBIDMap.find( PlayerDBID );
	if( Iter == _MemberDBIDMap.end() )
		return false;

	GuildMemberStruct* Member = Iter->second;
	
	GuildStruct* GuildInfo;
	GuildInfo = GetGuildInfo( Member->GuildID );
	if( GuildInfo == NULL )
		return false;

	for( int i = 0 ; i < (int) GuildInfo->Member.size() ; i++ )
	{
		GuildMemberStruct* OtherMember =  GuildInfo->Member[i];
		if( OtherMember->MemberDBID == PlayerDBID )
		{
			_MemberDBIDMap.erase( Iter );
			
			GuildInfo->Member.erase( GuildInfo->Member.begin() + i );
			/*
			if( OtherMember->IsOnline )
				GuildInfo->OnlineMemberDBIDMap.erase( PlayerDBID );
			else
				GuildInfo->OfflineMemberDBIDMap.erase( PlayerDBID );
*/
			delete OtherMember;
			return true;
		}
	}

	return false;
	
}


GuildStruct*	GuildManageClass::GetGuildInfo( int GuildID )
{
	if( (unsigned)GuildID >= _GuildList.size() )
		return NULL;

	return _GuildList[ GuildID ];
}
GuildStruct*	GuildManageClass::GetGuildInfo( const char* GuildName )
{
	map< string , GuildStruct* >::iterator Iter;
	Iter = _GuildNameMap.find( GuildName );
	if( Iter == _GuildNameMap.end() )
		return NULL;

	return Iter->second;
}

//���o���|�������
GuildMemberStruct* GuildManageClass::GetMember( int PlayerDBID )
{
	map< int , GuildMemberStruct* >::iterator Iter;
	Iter = _MemberDBIDMap.find( PlayerDBID );
	if( Iter == _MemberDBIDMap.end() )
		return NULL;

	return Iter->second;

}

//�]�w�Y�����W�u
bool	GuildManageClass::SetMemberOnline( int PlayerDBID , OnlinePlayerInfoStruct* Info )
{
	GuildMemberStruct* Member = GetMember( PlayerDBID );
	if( Member == NULL )
		return false;

	Member->MemberName = Info->Name;

	Member->IsOnline = true;
	Member->Voc			= Info->Voc;
	Member->Voc_Sub		= Info->Voc_Sub;
	Member->Sex			= Info->Sex;
	Member->LV			= Info->LV;
	Member->LV_Sub		= Info->LV_Sub;

	return true;
}
//�]�w�Y�����U�u
bool	GuildManageClass::SetMemberOffline( int PlayerDBID )
{
	GuildMemberStruct* Member = GetMember( PlayerDBID );
	if( Member == NULL )
		return false;

	Member->IsOnline = false;
	Member->LogoutTime = TimeStr::Now_Value();

	return true;
}

//�ק綠�|���
bool	GuildManageClass::ModifySimpleGuild( SimpleGuildInfoStruct& SimpleGuildInfo  )
{
	GuildStruct* Guild = GetGuildInfo( SimpleGuildInfo.GuildID );
	if( Guild == NULL )
		return false;

	Guild->Base.Resource = SimpleGuildInfo.Resource;
	/*
	Guild->Base.WarMyScore = SimpleGuildInfo.WarMyScore;
	Guild->Base.WarMyKillCount = SimpleGuildInfo.WarMyKillCount;
	Guild->Base.WarEnemyScore = SimpleGuildInfo.WarEnemyScore;
	Guild->Base.WarEnemyKillCount = SimpleGuildInfo.WarEnemyKillCount;
	*/
	return true;
}

//�ק綠�|���
bool	GuildManageClass::ModifyGuild( GuildBaseStruct& Base )
{
	GuildStruct* Guild = GetGuildInfo( Base.GuildID );
	if( Guild == NULL )
		return false;
	//-----------------------------------------------------------------------
	/*
	//����ˬd
	if( Guild->Base.GuildName != Base.GuildName  )
		return false;
*/
//	if( Guild->Base.LeaderDBID != Base.LeaderDBID  )
//		return false;
	//-----------------------------------------------------------------------
	/*
	//�]�w��� Copy
	memcpy( Guild->Base.Group , Base.Group , sizeof(Base.Group) );
	Guild->Base.IsRecruit = Base.IsRecruit;
	Guild->Base.Medal = Base.Medal;
	memcpy( Guild->Base.Rank , Base.Rank , sizeof(Base.Rank) );	
	Guild->Base.Note = Base.Note;
	Guild->Base.Introduce = Base.Introduce;
	Guild->Base.IsReady = Base.IsReady;
	Guild->Base.RankCount = Base.RankCount;
*/
	Guild->Base = Base;
	//-----------------------------------------------------------------------
	Guild->Base.IsNeedSave = true;
	return true;
}


//�ק令�����
bool	GuildManageClass::ModifyMember(  GuildMemberStruct& Info )
{
	GuildMemberStruct* Member = GetMember( Info.MemberDBID );
	if( Member == NULL )
		return false;
	//-----------------------------------------------------------------------
	//����ˬd
	if( Member->GuildID != Info.GuildID )
		return false;
	if( Member->MemberDBID != Info.MemberDBID )
		return false;
	//-----------------------------------------------------------------------
	//�]�w��� Copy
	Member->Rank = Info.Rank;
	Member->GroupID = Info.GroupID;
	Member->SelfNote = Info.SelfNote;
	Member->LeaderNote = Info.LeaderNote;
	Member->RankTime = Info.RankTime;
	//-----------------------------------------------------------------------
	Member->IsNeedSave = true;
	return true;
}

//------------------------------------------------------------------
//		�Q�תO���
//------------------------------------------------------------------
//�s�W�T��
bool	GuildManageClass::AddBoardMessage( GuildBoardStruct& Message , bool CheckGUID )
{
	GuildStruct* Guild = GetGuildInfo( Message.GuildID );
	if( Guild == NULL )
		return false;
	
	if( CheckGUID != false && Guild->BoardAll.size() != 0 )
	{
		if( Message.GUID != Guild->BoardAll.back()->GUID +1 )
			return false;
	}

	GuildBoardStruct* BoardMessage = NEW( GuildBoardStruct );
	*BoardMessage = Message;
	
	Guild->BoardAll.push_back( BoardMessage );

	int PosID = 0;
	for( unsigned i = 0 ; i < Guild->BoardList.size() ; i++ )
	{
		PosID = i;
		if( Guild->BoardList[i]->Type.Top == false )
			break;
	}
	Guild->BoardList.insert( Guild->BoardList.begin() + PosID , BoardMessage );

	return true;
}
//�����T��
bool	GuildManageClass::DelBoardMessage( int GuildID ,  int GUID )
{
	GuildStruct* Guild = GetGuildInfo( GuildID );
	if( Guild == NULL )
		return false;

	GuildBoardStruct* BoardMessage = GetMessage( GuildID , GUID );
	if( BoardMessage == NULL )
		return false;
	/*
	int BoardTopID = Guild->BoardAll[0]->GUID;

	
	int ID = GUID - BoardTopID;
	if( (unsigned)ID >= Guild->BoardAll.size() )
		return false;

	Guild->BoardAll[ ID ]->Type.Del = true;
*/
	BoardMessage->Type.Del = true;

	//��BoardList����
	for( unsigned i = 0 ; i < Guild->BoardList.size() ; i++  )
	{
		if( Guild->BoardList[i]->GUID == GUID )
		{
			Guild->BoardList.erase( Guild->BoardList.begin() + i );
			break;
		}
		//else
		//	Guild->BoardList[i]->GUID = i;
	}


	return true;
}
/*
//�ק�T��
bool	GuildManageClass::ModifyBoardMessage( GuildBoardStruct& Message )
{
	GuildStruct* Guild = GetGuildInfo( Message.GuildID );
	if( Guild == NULL )
		return false;

	for( unsigned i = 0 ; i < Guild->BoardAll.size() ; i++ )
	{
		if( Guild->BoardAll[i]->GUID == Message.GUID )
		{
			( *Guild->BoardAll[i] ) = Message;
			break;
		}
	}

	return true;
}
*/

GuildBoardStruct* GuildManageClass::GetMessage( int GuildID , int GUID )
{
	GuildStruct* Guild = GetGuildInfo( GuildID );
	if( Guild == NULL )
		return NULL;

	if( Guild->BoardAll.size() == 0 )
		return NULL;

	for( unsigned i = 0 ; i< Guild->BoardAll.size() ; i++ )
	{
		GuildBoardStruct* BoardInfo = Guild->BoardAll[ i ];
		if( BoardInfo == NULL )
			continue;
		if( BoardInfo->GUID == GUID )
			return BoardInfo;
	}
	return NULL;
	//int BoardTopID = Guild->BoardAll[0]->GUID;
	//int ID = GUID - BoardTopID;
	//if( (unsigned)ID >= Guild->BoardAll.size() )
	//	return NULL;

	//return Guild->BoardAll[ ID ];
}

bool	GuildManageClass::WarDeclareInsert( int Guild1 , int Guild2 )
{
	pair<int,int> guildPair;
	if( Guild1 == Guild2 )
		return false;

	if( Guild1 > Guild2 )
	{
		guildPair.first = Guild2;
		guildPair.second = Guild1;
	}
	else
	{
		guildPair.first = Guild1;
		guildPair.second = Guild2;
	}
	pair< set<pair<int,int>>::iterator, bool > InsertRet = _DeclareWarSet.insert( guildPair );
	return InsertRet.second;
}
void	GuildManageClass::WarDeclareEarse( int Guild1 , int Guild2 )
{
	pair<int,int> guildPair;
	if( Guild1 == Guild2 )
		return;

	if( Guild1 > Guild2 )
	{
		guildPair.first = Guild2;
		guildPair.second = Guild1;
	}
	else
	{
		guildPair.first = Guild1;
		guildPair.second = Guild2;
	}

	_DeclareWarSet.erase( guildPair );
}
bool	GuildManageClass::IsWarDeclare( int Guild1 , int Guild2 )
{
	pair<int,int> guildPair;
	if( Guild1 == Guild2 )
		return false;

	if( Guild1 > Guild2 )
	{
		guildPair.first = Guild2;
		guildPair.second = Guild1;
	}
	else
	{
		guildPair.first = Guild1;
		guildPair.second = Guild2;
	}

	if( _DeclareWarSet.find( guildPair ) != _DeclareWarSet.end() )
		return true;

	return false;
}