#ifndef __GROUPPARTY_H__
#define __GROUPPARTY_H__
#pragma		warning (disable:4786)
//----------------------------------------------------------------------------------------
#include "../GroupObject/GroupObject.h"
#include "../../NetWalker_Member/NetWakerChatInc.h"
#include "roledata/RoleDataEx.h"

//�Q����
#define _MAX_ITEM_LOOT_TIME_		(  1000 * 60 * 5 )
#define _MAX_ITEM_LOOT_TIME_ASSIGN_	(  1000 * 60 * 10 )

struct LootBaseInfo
{
	int		DBID;
	int		Value;
};

struct PartyTreasureStruct
{
	int					CreateTime;		//���w�F��n�b���������۰ʤ��t
	int					Version;
	ItemFieldStruct		Item;
	StaticVector< int , _MAX_PARTY_COUNT_  > LootDBID;
	StaticVector< LootBaseInfo , _MAX_PARTY_COUNT_  > LootInfo;
	int					OwnerDBID;		// -1 �N��S��
	int					Loot;			// �Y�X���I��
	PartyItemShareENUM	ShareType;		// ���t�Ҧ�

	bool    CheckDBID( int DBID );
	//�Y�H��� 
	bool	PlayerGiveUp( int DBID );
	bool	PlayerLoot( int DBID , int LootValue );
	int		AutoShare( );
	
	//�O�_�ɶ���(�n�۰ʤ��t)
	bool	IsTimeUp();

	void	Init();
};

struct GroupPartyMemberStruct
{
	PartyMemberInfoStruct Info;
	BaseItemObject* Obj;
	//���~���ɶ���
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
	//²�ƳB�z
	//���tLoot�����~
	bool	_DepartLootItem( PartyTreasureStruct* Data , int ID );
	
	//���y���t (�i�H���t�^�Ǧ�)
	bool	_ShareInorder( ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_ >& LootDBID );

public:
	GroupParty();
	virtual ~GroupParty();

	//�]�wParty�W�h
	bool	SetRule( PartyBaseInfoStruct& _Info );
	PartyBaseInfoStruct*	Info(){ return &_Info; };
	int		MemberCount( );
	int		OnlineMemberCount( );
	GroupPartyMemberStruct*	Member( int ID );
	GroupPartyMemberStruct*	MemberByDBID( int DBID );
	PartyMemberInfoStruct*	OfflineMemberByDBID( int DBID );
	PartyMemberInfoStruct*	MemberByDBID_All( int DBID );		//�u�W�u�U������

	vector< GroupPartyMemberStruct >*	Member() { return &_Member; };

	//----------------------------------------------------
	//���ɪ��~
	//----------------------------------------------------
	void	AddLootItem( ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_ >& LootDBID );
//	void	AutoShare( int ItemVersion );
	void	PlayerLoot( int DBID , int ItemVersion , int AddValue );
	void	PlayerGiveUp( int DBID , int ItemVersion );
	void	PlayerAssist( int SenderDBID , int DBID , int ItemVersion );

	//�q��������� �Y�H�o�쪫�~  �åB�q��Local Server
	void	GiveItemByDBID( int DBID  , ItemFieldStruct& Item , int ItemVersion );

	//�R���U�u�����a
	bool	DelOfflineUser( const char* MemberName );
	bool	DelOfflineUser( int MemberDBID );



	//���s�[�J�U�u���a
	bool	AddLoginUser( int DBID );

	//�M���Y���a�� �Y�_��T( �]�w�U�u)
	void	ClearPlayerLootInfo( int DBID );
	//�M���Y���a�� �Y�_��T( �]�w�W�u)
	void	ResetPlayerLootInfo( int DBID );

	//�]�wParty ����
	void	SetPartyType( PartyTypeENUM Type );
	//�ץ�������m
	bool	SetPartyMemberPos( int MemberDBID[2] , int Pos[2] );

	//�ץ��������
	void	FixMemberInfo( PartyMemberInfoStruct* Member );
	//----------------------------------------------------

	virtual void	Process();

	virtual void	OnCreate();
	virtual void	OnDestroy();
	virtual bool	OnUserLogout( BaseItemObject*  Obj );
	virtual void	OnAddUser( BaseItemObject*  Obj );
	virtual void	OnDelUser( BaseItemObject*  Obj );
	virtual void	OnChangeRoleInfo( BaseItemObject*  Obj );		//�����Ʀ����ܩδ���
};

//----------------------------------------------------------------------------------------
#endif