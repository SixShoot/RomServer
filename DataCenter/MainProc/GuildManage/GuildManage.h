#pragma once
#include "roledata/RoleBaseDef.h"
#include <utility>
#include <set>

//////////////////////////////////////////////////////////////////////////
//公會資料管理 的物件
//////////////////////////////////////////////////////////////////////////
class GuildManageClass
{
protected:
	static GuildManageClass *_This;
public:

	static bool Init( );
	static bool Release( );
	static GuildManageClass* This(){ return _This; };

	vector< GuildStruct* >			_GuildList;			//公會列表
	map< string , GuildStruct* >	_GuildNameMap;		//公會名稱Map
	
	map< int , GuildMemberStruct* > _MemberDBIDMap;		//公會成員DBID Map

	set< pair<int,int> >			_DeclareWarSet;		//公會敵對關系表	

	public:
	GuildManageClass();
	~GuildManageClass();

	//------------------------------------------------------------------------
	//清除所有的記憶體
	//------------------------------------------------------------------------
	void	Clear();
	//------------------------------------------------------------------------
	//公會
	//------------------------------------------------------------------------
	//建立公會
	GuildStruct*	CreateGuild( GuildBaseStruct& Base );
	
	//修改公會資料
	bool			ModifyGuild( GuildBaseStruct& Base );
	//修改公會資料
	bool			ModifySimpleGuild( SimpleGuildInfoStruct& SimpleGuildInfo  );

	//刪除公會
	bool	DelGuild( int GuildID );
	//------------------------------------------------------------------------
	//公會成員
	//------------------------------------------------------------------------
	//增加成員
	bool	AddMember( GuildMemberStruct& Member );
	//刪除成員
	bool	DelMember( int GuildID , char* MemberName );
	//刪除成員
	bool	DelMember( int PlayerDBID );
	//修改成員資料
	bool	ModifyMember(  GuildMemberStruct& Member );
	
	//設定某成員上線
	bool	SetMemberOnline( int PlayerDBID , OnlinePlayerInfoStruct* Info );
	//設定某成員下線
	bool	SetMemberOffline( int PlayerDBID );

	//------------------------------------------------------------------
	//		討論板資料
	//------------------------------------------------------------------
	//新增訊息
	bool	AddBoardMessage( GuildBoardStruct& Message , bool CheckGUID );
	
	//移除訊息
	bool	DelBoardMessage( int GuildID , int MessageGUID );

	//------------------------------------------------------------------------
	//資料取得
	//------------------------------------------------------------------------
	//取得公會資料
	GuildStruct*	GetGuildInfo( int GuildID );
	GuildStruct*	GetGuildInfo( const char* GuildName );

	//取得公會成員資料
	GuildMemberStruct* GetMember( int PlayerDBID );
	//取得公會成員資料
	GuildMemberStruct* GetMember( int GuildID , char* MemberName );

	//取得訊息
	GuildBoardStruct* GetMessage( int GuildID , int GUID );

	//最大的公會id
	int		MaxGuildID(){ return (int)_GuildList.size(); };

	vector< GuildStruct* >&	GuildList() { return _GuildList; };			//公會列表
	//------------------------------------------------------------------------
	//公會自由宣戰
	//------------------------------------------------------------------------
	set< pair<int,int> >*		DeclareWarSet() { return &_DeclareWarSet; }		//公會敵對關系表	
	bool	WarDeclareInsert( int Guild1 , int Guild2 );
	void	WarDeclareEarse( int Guild1 , int Guild2 );
	bool	IsWarDeclare( int Guild1 , int Guild2 );
};