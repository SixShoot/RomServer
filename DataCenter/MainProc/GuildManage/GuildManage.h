#pragma once
#include "roledata/RoleBaseDef.h"
#include <utility>
#include <set>

//////////////////////////////////////////////////////////////////////////
//���|��ƺ޲z ������
//////////////////////////////////////////////////////////////////////////
class GuildManageClass
{
protected:
	static GuildManageClass *_This;
public:

	static bool Init( );
	static bool Release( );
	static GuildManageClass* This(){ return _This; };

	vector< GuildStruct* >			_GuildList;			//���|�C��
	map< string , GuildStruct* >	_GuildNameMap;		//���|�W��Map
	
	map< int , GuildMemberStruct* > _MemberDBIDMap;		//���|����DBID Map

	set< pair<int,int> >			_DeclareWarSet;		//���|�Ĺ����t��	

	public:
	GuildManageClass();
	~GuildManageClass();

	//------------------------------------------------------------------------
	//�M���Ҧ����O����
	//------------------------------------------------------------------------
	void	Clear();
	//------------------------------------------------------------------------
	//���|
	//------------------------------------------------------------------------
	//�إߤ��|
	GuildStruct*	CreateGuild( GuildBaseStruct& Base );
	
	//�ק綠�|���
	bool			ModifyGuild( GuildBaseStruct& Base );
	//�ק綠�|���
	bool			ModifySimpleGuild( SimpleGuildInfoStruct& SimpleGuildInfo  );

	//�R�����|
	bool	DelGuild( int GuildID );
	//------------------------------------------------------------------------
	//���|����
	//------------------------------------------------------------------------
	//�W�[����
	bool	AddMember( GuildMemberStruct& Member );
	//�R������
	bool	DelMember( int GuildID , char* MemberName );
	//�R������
	bool	DelMember( int PlayerDBID );
	//�ק令�����
	bool	ModifyMember(  GuildMemberStruct& Member );
	
	//�]�w�Y�����W�u
	bool	SetMemberOnline( int PlayerDBID , OnlinePlayerInfoStruct* Info );
	//�]�w�Y�����U�u
	bool	SetMemberOffline( int PlayerDBID );

	//------------------------------------------------------------------
	//		�Q�תO���
	//------------------------------------------------------------------
	//�s�W�T��
	bool	AddBoardMessage( GuildBoardStruct& Message , bool CheckGUID );
	
	//�����T��
	bool	DelBoardMessage( int GuildID , int MessageGUID );

	//------------------------------------------------------------------------
	//��ƨ��o
	//------------------------------------------------------------------------
	//���o���|���
	GuildStruct*	GetGuildInfo( int GuildID );
	GuildStruct*	GetGuildInfo( const char* GuildName );

	//���o���|�������
	GuildMemberStruct* GetMember( int PlayerDBID );
	//���o���|�������
	GuildMemberStruct* GetMember( int GuildID , char* MemberName );

	//���o�T��
	GuildBoardStruct* GetMessage( int GuildID , int GUID );

	//�̤j�����|id
	int		MaxGuildID(){ return (int)_GuildList.size(); };

	vector< GuildStruct* >&	GuildList() { return _GuildList; };			//���|�C��
	//------------------------------------------------------------------------
	//���|�ۥѫž�
	//------------------------------------------------------------------------
	set< pair<int,int> >*		DeclareWarSet() { return &_DeclareWarSet; }		//���|�Ĺ����t��	
	bool	WarDeclareInsert( int Guild1 , int Guild2 );
	void	WarDeclareEarse( int Guild1 , int Guild2 );
	bool	IsWarDeclare( int Guild1 , int Guild2 );
};