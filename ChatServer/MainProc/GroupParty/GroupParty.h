#ifndef __GROUPPARTY_H__
#define __GROUPPARTY_H__
#pragma		warning (disable:4786)
//----------------------------------------------------------------------------------------
#include "../GroupObject/GroupObject.h"
#include "../../NetWalker_Member/NetWakerChatInc.h"
#include "roledata/RoleDataEx.h"

//十分鐘
#define _MAX_ITEM_LOOT_TIME_		(  1000 * 60 * 5 )
#define _MAX_ITEM_LOOT_TIME_ASSIGN_	(  1000 * 60 * 10 )

struct LootBaseInfo
{
	int		DBID;
	int		Value;
};

struct PartyTreasureStruct
{
	int					CreateTime;		//內定東西要在五分中內自動分配
	int					Version;
	ItemFieldStruct		Item;
	StaticVector< int , _MAX_PARTY_COUNT_  > LootDBID;
	StaticVector< LootBaseInfo , _MAX_PARTY_COUNT_  > LootInfo;
	int					OwnerDBID;		// -1 代表沒有
	int					Loot;			// 擲出的點數
	PartyItemShareENUM	ShareType;		// 分配模式

	bool    CheckDBID( int DBID );
	//某人放棄 
	bool	PlayerGiveUp( int DBID );
	bool	PlayerLoot( int DBID , int LootValue );
	int		AutoShare( );
	
	//是否時間到(要自動分配)
	bool	IsTimeUp();

	void	Init();
};

struct GroupPartyMemberStruct
{
	PartyMemberInfoStruct Info;
	BaseItemObject* Obj;
	//物品分享順序
	int	ItemShareOrder;
};

class GroupParty : public GroupObjectClass
{
protected:
	PartyBaseInfoStruct					_Info;
	vector< GroupPartyMemberStruct >	_Member;
	vector< PartyMemberInfoStruct >		_OfflineMember;
	vector< PartyTreasureStruct >		_Treasure;
	int									_MaxShareID;
	

	//-----------------------------------------------------------------------
	//簡化處理
	//分配Loot的物品
	bool	_DepartLootItem( PartyTreasureStruct* Data , int ID );
	
	//輪流分配 (可以分配回傳成)
	bool	_ShareInorder( ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_ >& LootDBID );

public:
	GroupParty();
	virtual ~GroupParty();

	//設定Party規則
	bool	SetRule( PartyBaseInfoStruct& _Info );
	PartyBaseInfoStruct*	Info(){ return &_Info; };
	int		MemberCount( );
	int		OnlineMemberCount( );
	GroupPartyMemberStruct*	Member( int ID );
	GroupPartyMemberStruct*	MemberByDBID( int DBID );
	PartyMemberInfoStruct*	OfflineMemberByDBID( int DBID );
	PartyMemberInfoStruct*	MemberByDBID_All( int DBID );		//線上線下的都找

	vector< GroupPartyMemberStruct >*	Member() { return &_Member; };

	//----------------------------------------------------
	//分享物品
	//----------------------------------------------------
	void	AddLootItem( ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_ >& LootDBID );
//	void	AutoShare( int ItemVersion );
	void	PlayerLoot( int DBID , int ItemVersion , int AddValue );
	void	PlayerGiveUp( int DBID , int ItemVersion );
	void	PlayerAssist( int SenderDBID , int DBID , int ItemVersion );

	//通知隊伍全員 某人得到物品  並且通知Local Server
	void	GiveItemByDBID( int DBID  , ItemFieldStruct& Item , int ItemVersion );

	//刪除下線的玩家
	bool	DelOfflineUser( const char* MemberName );
	bool	DelOfflineUser( int MemberDBID );



	//重新加入下線玩家
	bool	AddLoginUser( int DBID );

	//清除某玩家的 擲寶資訊( 設定下線)
	void	ClearPlayerLootInfo( int DBID );
	//清除某玩家的 擲寶資訊( 設定上線)
	void	ResetPlayerLootInfo( int DBID );

	//設定Party 類型
	void	SetPartyType( PartyTypeENUM Type );
	//修正成員位置
	bool	SetPartyMemberPos( int MemberDBID[2] , int Pos[2] );

	//修正成員資料
	void	FixMemberInfo( PartyMemberInfoStruct* Member );
	//----------------------------------------------------

	virtual void	Process();

	virtual void	OnCreate();
	virtual void	OnDestroy();
	virtual bool	OnUserLogout( BaseItemObject*  Obj );
	virtual void	OnAddUser( BaseItemObject*  Obj );
	virtual void	OnDelUser( BaseItemObject*  Obj );
	virtual void	OnChangeRoleInfo( BaseItemObject*  Obj );		//當角色資料有更變或換區
};

//----------------------------------------------------------------------------------------
#endif