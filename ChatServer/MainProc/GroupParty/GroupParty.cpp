#include "../Global.h"
#include "GroupParty.h"
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//某人放棄 
bool		PartyTreasureStruct::PlayerGiveUp( int DBID )
{
	for( int i = 0 ; i < LootDBID.Size() ; i++ )
	{
		if( DBID == LootDBID[i] )
		{
			LootDBID.Erase( i );

			/*
			if( LootDBID.Size() == 1 && OwnerDBID == -1 )
			{
				OwnerDBID = LootDBID[0];
				Loot	  = 0;
				LootDBID.Clear();
			}
			*/
			LootBaseInfo Temp;
			Temp.DBID = DBID;
			Temp.Value = -1;
			LootInfo.Push_Back( Temp );

			/*
			if( LootDBID.Size() == 0 && OwnerDBID == -1 )
			{

//				OwnerDBID = PassDBID[ rand() % PassDBID.Size() ];
//				Loot	  = 0;
//				LootDBID.Clear();
			}
*/
			return true;
		}
	}
	return false;
}
bool	PartyTreasureStruct::PlayerLoot( int DBID , int LootValue )
{
	for( int i = 0 ; i < LootDBID.Size() ; i++ )
	{
		if( DBID == LootDBID[i] )
		{
			LootBaseInfo Temp;
			Temp.DBID = DBID;
			Temp.Value = LootValue;

			LootInfo.Push_Back( Temp );

			LootDBID.Erase( i );

			if( LootValue > Loot )
			{
				OwnerDBID = DBID;
				Loot	  = LootValue;
			}
			return true;
		}
	}
	return false;
}
int		PartyTreasureStruct::AutoShare( )
{
	if( OwnerDBID != -1 )
		return OwnerDBID;

	if( LootDBID.Size() <= 0 )
		return -1;

	OwnerDBID = LootDBID[ rand() % LootDBID.Size() ];
	LootDBID.Clear();

	return OwnerDBID;
}

//是否時間到(要自動分配)
bool	PartyTreasureStruct::IsTimeUp()
{
	if( ShareType == EM_PartyItemShare_Assign )
	{
		if( clock() - CreateTime >= _MAX_ITEM_LOOT_TIME_ASSIGN_ )
			return true;
	}
	else
	{
		if( clock() - CreateTime >= _MAX_ITEM_LOOT_TIME_ )
			return true;
	}

	return false;
}

void	PartyTreasureStruct::Init()
{
	static int Value = 0;
	Value++;

	Version = Value;
	CreateTime = clock();		//內定東西要在五分中內自動分配
	Item.Init();
	
	OwnerDBID = -1;		// -1 代表沒有
	Loot = -1;			// 擲出的點數
}
bool    PartyTreasureStruct::CheckDBID( int DBID )
{
	for( int i = 0 ; i < LootDBID.Size() ; i++ )
	{
		if( DBID == LootDBID[i] )
		{		
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//設定Party規則
bool	GroupParty::SetRule( PartyBaseInfoStruct& Info )
{
	
	if( Info.Type == EM_PartyType_Party && _Info.Type == EM_PartyType_Raid )
		return false;

	bool CheckLeader = false;
	bool CheckAttack = false;
	bool CheckShare  = false;
	int  LeaderID = 0;
	Info.PartyID = _ID;
	//設定資訊檢查
	//簡查名子
	for( int i = 0 ; i < (int)_Member.size() ; i++  )
	{

		BaseItemObject* Obj = _Member[i].Obj;
		// 
		if( strcmp( Obj->RoleName() , Info.LeaderName.Begin() ) == 0  )
		{
			CheckLeader = true;
			LeaderID = i;
		}
		if( strcmp( Obj->RoleName() , Info.AttackName.Begin() ) == 0  )
			CheckAttack = true;
		if( strcmp( Obj->RoleName() , Info.ShareName.Begin() )  == 0  )
			CheckShare = true;	
	}

	if( CheckLeader == false )
	{
		return false;
	}
	if( LeaderID != 0 )
	{
		_Member.insert( _Member.begin() , _Member[ LeaderID ] );
		_Member.erase( _Member.begin() +  LeaderID + 1 );
		for( int i = 0 ; i < (int)_Member.size() ; i++  )
		{
			_Member[i].Info.MemberID = i;
		}
	}
	

	if( CheckAttack == false )
	{
		Info.AttackName.Clear();
	}
	if( CheckShare == false )
	{
		Info.ShareName.Clear();
	}

	if( Info.ItemShareType == EM_PartyItemShare_Assign && CheckShare == false )
	{
		return false;
	}
	if( memcmp(&_Info,&Info,sizeof(_Info)) == 0 )
	{
		return true;
	}


	//設定並通知所有成員
	for( int i = 0 ; i < (int)_Member.size() ; i++  )
	{

		BaseItemObject* Obj = _Member[i].Obj;
		NetSrv_Party::SC_PartyBaseInfo( Obj->SockID() , Info );
	}

	_Info = Info;
	NetSrv_Party::SL_PartyBaseInfo( Info );
	return true;
}
//---------------------------------------------------------------------------------------------
GroupParty::GroupParty():GroupObjectClass( EM_GroupObjectType_Party )
{
	_Info.PartyID = _ID;
	_MaxShareID = 0;
}
GroupParty::~GroupParty()
{
	//把所有東西分出去
	for( unsigned i  = 0 ; i < _Treasure.size() ; )
	//while( _Treasure.size() != 0 )
	{
		PartyTreasureStruct* Data = &_Treasure[ i ];
		for( int j = 0 ; j < Data->LootDBID.Size() ; j++ )
		{
			LootBaseInfo Temp;
			Temp.DBID = Data->LootDBID[j];
			Temp.Value = -1;
			Data->LootInfo.Push_Back( Temp );
		}


		Data->LootDBID.Clear();
		if( _DepartLootItem( Data , i ) == false )
			i++;
	}
}
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// 虛擬時做
//---------------------------------------------------------------------------------------------
void	GroupParty::OnCreate()
{

}
void	GroupParty::OnDestroy()
{
	//通知所有成員
	for( int i = 0 ; i < (int)_Member.size(); i++ )
	{
		GroupPartyMemberStruct& MemberInfo = _Member[i];
		NetSrv_Party::SL_DelMember( _Info.PartyID , MemberInfo.Info , false ) ;
		NetSrv_Party::SC_DelMember( MemberInfo.Obj->SockID() , MemberInfo.Info );
	}
}
void	GroupParty::OnAddUser( BaseItemObject*  Obj )
{
	bool	IsEmptyNumber[_MAX_PARTY_COUNT_];
	memset( IsEmptyNumber , 0 , sizeof( IsEmptyNumber ) );
	//表示第一個人
	if( _UserList_SockID.size() == 1 )
	{
		_Info.PartyID = _ID;
		_Info.LeaderName = Obj->RoleName();
		_Info.ShareName  = "";
		_Info.AttackName = Obj->RoleName();
		_Info.Roll_CanUse	= false;
		_Info.RollTypeLV	= 1;
		//_Info.ItemShareType = EM_PartyItemShare_Roll;		
		_Info.ItemShareType = EM_PartyItemShare_InOrder;		
		_Info.Type			= EM_PartyType_Party;		

		NetSrv_Party::SL_PartyBaseInfo( _Info );
	}
	
	GroupPartyMemberStruct MemberInfo;
	MemberInfo.Info.DBID		= Obj->DBID();
	MemberInfo.Info.GItemID		= Obj->LocalID();
	MemberInfo.Info.MemberID	= (int)_Member.size();
	MemberInfo.Info.Name		= Obj->RoleName();
	MemberInfo.Info.Voc			= Obj->Role()->Voc;
	MemberInfo.Info.Voc_Sub		= Obj->Role()->Voc_Sub;
	MemberInfo.Info.Level		= Obj->Role()->LV;
	MemberInfo.Info.Level_Sub	= Obj->Role()->LV_Sub;
	MemberInfo.Info.Sex			= Obj->Role()->Sex;
	MemberInfo.Obj				= Obj;
	MemberInfo.Info.Mode._Mode	= 0;
	MemberInfo.ItemShareOrder	= _MaxShareID++;
	MemberInfo.Info.PartyNo		= 0;
	MemberInfo.Info.ParallelID	= Obj->Role()->ParallelID;


	for( unsigned int i = 0 ; i < _Member.size() ; i++ )
	{
		if( _Member[i].Info.PartyNo >= _MAX_PARTY_COUNT_ )
			_Member[i].Info.PartyNo = _MAX_PARTY_COUNT_ -1;

			IsEmptyNumber[ _Member[i].Info.PartyNo ] = true;
	}

	for( unsigned int i = 0 ; i < _OfflineMember.size() ; i++ )
	{
		if( _OfflineMember[i].PartyNo >= _MAX_PARTY_COUNT_ )
			_OfflineMember[i].PartyNo = _MAX_PARTY_COUNT_ -1;
		IsEmptyNumber[ _OfflineMember[i].PartyNo ] = true;
	}


	for( int i = 0 ; i < _MAX_PARTY_COUNT_ ; i++ )
	{
		if( IsEmptyNumber[i] == false )
		{
			MemberInfo.Info.PartyNo = i;
			break;
		}
	}

	_Member.push_back( MemberInfo );

	NetSrv_Party::SC_PartyBaseInfo( Obj->SockID() , _Info );

	NetSrv_Party::SL_AddMember( _Info.PartyID , MemberInfo.Info ) ;

	//設定並通知所有成員
	Obj->PartyID( _ID );

	for( unsigned int i = 0 ; i < _Member.size() ; i++ )
	{
		if( Obj != _Member[i].Obj )
		{
			NetSrv_Party::SC_AddMember( Obj->SockID() , _Member[i].Info );
		}
		NetSrv_Party::SC_AddMember( _Member[i].Obj->SockID() , MemberInfo.Info );
	}


	//通知所有下線的成員
	for( unsigned int i = 0 ; i < _OfflineMember.size() ; i++ )
	{
		NetSrv_Party::SC_AddMember( Obj->SockID() , _OfflineMember[i] );
	}

}
//////////////////////////////////////////////////////////////////////////
int		GroupParty::MemberCount( )
{ 
	return (int)( _Member.size() + _OfflineMember.size() ); 
}

int		GroupParty::OnlineMemberCount( )
{ 
	return (int)( _Member.size() ); 
}
//////////////////////////////////////////////////////////////////////////
//回傳false  代表不會觸發 OnDel User
bool	GroupParty::OnUserLogout( BaseItemObject*  Obj )
{
	int			MemberID;
	bool		IsNeedSendPartyInfo = false;
	int			i;
	for( MemberID = 0 ; MemberID < (int)_Member.size(); MemberID++ )
	{
		if( _Member[MemberID].Info.DBID == Obj->DBID() )		
			break;
	}

	if( MemberID == _Member.size() )
		return false;

	ClearPlayerLootInfo( Obj->DBID() );

	GroupPartyMemberStruct& MemberInfo = _Member[MemberID];
	NetSrv_Party::SL_DelMember( _Info.PartyID , MemberInfo.Info , true ) ;


	if( MemberID == 0 && _Member.size() >= 2 )
	{
		_Info.LeaderName = _Member[1].Info.Name;
		if( strcmp( _Info.ShareName.Begin() , MemberInfo.Info.Name.Begin() ) == 0 )
		{
			_Info.ShareName  = _Member[1].Info.Name;
		}
		if( strcmp( _Info.AttackName.Begin() , MemberInfo.Info.Name.Begin() ) == 0 )
		{
			_Info.AttackName  = _Member[1].Info.Name;
		}
		IsNeedSendPartyInfo = true;
	}
	else
	{
		if( strcmp( _Info.ShareName.Begin() , MemberInfo.Info.Name.Begin() ) == 0 )
		{
			_Info.ShareName  = _Member[0].Info.Name;
			IsNeedSendPartyInfo = true;
		}
		if( strcmp( _Info.AttackName.Begin() , MemberInfo.Info.Name.Begin() ) == 0 )
		{
			_Info.AttackName  = _Member[0].Info.Name;
			IsNeedSendPartyInfo = true;
		}
	}



	MemberInfo.Info.GItemID = -1;
	MemberInfo.Info.MemberID = -1;
	Obj->PartyID( -1 );

	_OfflineMember.push_back( MemberInfo.Info );
	_Member.erase( _Member.begin() + MemberID );

	if( _Member.size() == 0 )
	{
		_OfflineMember.clear();
	}

	for( i = 0 ; i < (int)_Member.size(); i++ )
	{
		_Member[i].Info.MemberID = i;
	}

	if( IsNeedSendPartyInfo == true)
	{
		NetSrv_Party::SL_PartyBaseInfo( _Info );
		for( unsigned int i = 0 ; i < _Member.size() ; i++ )
		{
			NetSrv_Party::SC_PartyBaseInfo( _Member[i].Obj->SockID() , _Info );
		}
	}

	return false;
}
//刪除下線的玩家
bool	GroupParty::DelOfflineUser( const char* MemberName )
{
	set< int >::iterator Iter;	
	for( int i = 0 ; i < (int)_OfflineMember.size(); i++ )
	{
		if( strcmp( MemberName , _OfflineMember[i].Name.Begin() ) == 0 )
		{
			//通知線上玩家
			for( Iter = _UserList_SockID.begin() ; Iter != _UserList_SockID.end() ; Iter++ )
			{
				//BaseItemObject* TempObj = *Iter;
				int SockID = *Iter;
				NetSrv_Party::SC_DelMember( SockID , _OfflineMember[i] );
			}

			_OfflineMember.erase( _OfflineMember.begin() + i );
			

			if( _Member.size() == 1 && MemberCount() == 1 )
			{
				GroupPartyMemberStruct& M = _Member[0];	

//				NetSrv_Party::SL_DelMember( _Info.PartyID , M.Info ) ;
				NetSrv_Party::SC_DelMember( M.Obj->SockID() , M.Info );

				M.Obj->PartyID( -1 );
				_Member.clear();
				_OfflineMember.clear();

			}

			return true;
		}
	}

	return false;
}
bool	GroupParty::DelOfflineUser( int MemberDBID )
{
	set< int >::iterator Iter;	
	for( int i = 0 ; i < (int)_OfflineMember.size(); i++ )
	{
		if( MemberDBID == _OfflineMember[i].DBID )
		{
			//通知線上玩家
			for( Iter = _UserList_SockID.begin() ; Iter != _UserList_SockID.end() ; Iter++ )
			{
				//BaseItemObject* TempObj = *Iter;
				int SockID = *Iter;
				NetSrv_Party::SC_DelMember( SockID , _OfflineMember[i] );
			}

			_OfflineMember.erase( _OfflineMember.begin() + i );



			if( _Member.size() == 1 && MemberCount() == 1 )
			{
				GroupPartyMemberStruct& M = _Member[0];	

//				NetSrv_Party::SL_DelMember( _Info.PartyID , M.Info ) ;
				NetSrv_Party::SC_DelMember( M.Obj->SockID() , M.Info );

				M.Obj->PartyID( -1 );
				_Member.clear();
				_OfflineMember.clear();

			}

			return true;
		}
	}

	return false;
}

//清除某玩家的 擲寶資訊
void	GroupParty::ClearPlayerLootInfo( int DBID )
{/*
	for( int i = (int)_Treasure.size() -1  ; i >= 0  ; i-- )
	{
		PartyTreasureStruct& ItemInfo = _Treasure[i];
		for( int j = 0 ; j < ItemInfo.LootInfo.Size() ; j++ )
		{
			if( ItemInfo.LootInfo[ j ].DBID == DBID )
			{
				ItemInfo.LootInfo[ j ].IsOnline = false;
				//ItemInfo.LootInfo.Erase( j );
				break;
			}
		}
/*
		for( int j = 0 ; j < (int)ItemInfo.LootDBID.Size() ; j++ )
		{
			if( ItemInfo.LootDBID[ j ] == DBID )
			{
				ItemInfo.LootDBID.Erase( j );
				break;
			}
		}
*/
		/*
		if( ItemInfo.OwnerDBID == DBID )
		{
			int MaxDBID = -1;
			int MaxValue = -1;

			for( int j = 0 ; j < (int)ItemInfo.LootInfo.Size() ; j++ )
			{
				if( ItemInfo.LootInfo[ j ].Value > MaxValue  && ItemInfo.LootInfo[ j ].IsOnline == true )
				{
					MaxValue = ItemInfo.LootInfo[ j ].Value;
					MaxDBID  = ItemInfo.LootInfo[ j ].DBID;
				}
			}
			if( MaxValue != -1 )
			{
				ItemInfo.OwnerDBID = MaxDBID;
				ItemInfo.Loot = MaxValue;
			}
		}
//		_DepartLootItem( &ItemInfo , i );

	}
*/
}

void	GroupParty::ResetPlayerLootInfo( int DBID )
{
	for( int i = (int)_Treasure.size() -1  ; i >= 0  ; i-- )
	{
		bool IsFind = false;
		PartyTreasureStruct& ItemInfo = _Treasure[i];
		for( int j = 0 ; j < ItemInfo.LootDBID.Size() ; j++ )
		{
			if( ItemInfo.LootDBID[ j ] == DBID )
			{
				NetSrv_Party::SC_PartyTreasure( DBID , ItemInfo.Version , ItemInfo.ShareType , ItemInfo.Item  , ItemInfo.LootDBID  );
				break;
			}
		}
	}
}

void	GroupParty::OnDelUser( BaseItemObject*  Obj )
{
	int			i;
	int			MemberID;
	bool		IsNeedSendPartyInfo = false;
	for( MemberID = 0 ; MemberID < (int)_Member.size(); MemberID++ )
	{
		if( _Member[MemberID].Info.DBID == Obj->DBID() )		
			break;
	}

	if( MemberID == _Member.size() )
		return;

	GroupPartyMemberStruct& MemberInfo = _Member[MemberID];

	//如果只剩下兩個成員
	if( MemberCount() <= 2 )
	{

	}
	else
	{
		//如果為隊長
		if( MemberID == 0 && _Member.size() >= 2 )
		{
			_Info.LeaderName = _Member[1].Info.Name;
			if( strcmp( _Info.ShareName.Begin() , MemberInfo.Info.Name.Begin() ) == 0 )
			{
				_Info.ShareName  = _Member[1].Info.Name;
			}
			if( strcmp( _Info.AttackName.Begin() , MemberInfo.Info.Name.Begin() ) == 0 )
			{
				_Info.AttackName  = _Member[1].Info.Name;
			}
			IsNeedSendPartyInfo = true;
		}
		else
		{
			if( strcmp( _Info.ShareName.Begin() , MemberInfo.Info.Name.Begin() ) == 0 )
			{
				_Info.ShareName  = _Member[0].Info.Name;
				IsNeedSendPartyInfo = true;
			}
			if( strcmp( _Info.AttackName.Begin() , MemberInfo.Info.Name.Begin() ) == 0 )
			{
				_Info.AttackName  = _Member[0].Info.Name;
				IsNeedSendPartyInfo = true;
			}
		}
	}

	
	NetSrv_Party::SL_DelMember( _Info.PartyID , MemberInfo.Info , false ) ;

	//設定並通知所有成員
	set<int>::iterator Iter;

	for( Iter = _UserList_SockID.begin() ; Iter != _UserList_SockID.end() ; Iter++ )
	{
		//BaseItemObject* TempObj = *Iter;
		int SockID = *Iter;
		NetSrv_Party::SC_DelMember( SockID , MemberInfo.Info );
	}

	_Member.erase( _Member.begin() + MemberID );

	ClearPlayerLootInfo( Obj->DBID() );
/*
	//處理物品分配
	for( i = (int)_Treasure.size() -1  ; i >= 0  ; i-- )
	{
		PartyTreasureStruct& ItemInfo = _Treasure[i];
		for( int j = 0 ; j < ItemInfo.LootInfo.Size() ; j++ )
		{
			if( ItemInfo.LootInfo[ j ].DBID == Obj->DBID() )
			{
				ItemInfo.LootInfo.Erase( j );
				break;
			}
		}

		for( int j = 0 ; j < (int)ItemInfo.LootDBID.Size() ; j++ )
		{
			if( ItemInfo.LootDBID[ j ] == Obj->DBID() )
			{
				ItemInfo.LootDBID.Erase( j );
				break;
			}
		}

		if( ItemInfo.OwnerDBID == Obj->DBID() )
		{
			int MaxDBID = -1;
			int MaxValue = -1;

			for( int j = 0 ; j < (int)ItemInfo.LootInfo.Size() ; j++ )
			{
				if( ItemInfo.LootInfo[ j ].Value > MaxValue  )
				{
					MaxValue = ItemInfo.LootInfo[ j ].Value;
					MaxDBID  = ItemInfo.LootInfo[ j ].DBID;
				}
			}
			if( MaxValue != -1 )
			{
				ItemInfo.OwnerDBID = MaxDBID;
				ItemInfo.Loot = MaxValue;
			}
		}
		_DepartLootItem( &ItemInfo , i );

	}
*/
	if( _Member.size() == 1 && MemberCount() == 1 )
	{
		GroupPartyMemberStruct& M = _Member[0];	

		NetSrv_Party::SL_DelMember( _Info.PartyID , M.Info , false ) ;
		NetSrv_Party::SC_DelMember( M.Obj->SockID() , M.Info );


		M.Obj->PartyID( -1 );
		_Member.clear();
		_OfflineMember.clear();

	}
	else
	{
		for( i = 0 ; i < (int)_Member.size(); i++ )
		{
			_Member[i].Info.MemberID = i;
		}

		if( IsNeedSendPartyInfo == true)
		{
			NetSrv_Party::SL_PartyBaseInfo( _Info );
			for( unsigned int i = 0 ; i < _Member.size() ; i++ )
			{
				NetSrv_Party::SC_PartyBaseInfo( _Member[i].Obj->SockID() , _Info );
			}
		}
	}

	Obj->PartyID( -1 );
}




void	GroupParty::OnChangeRoleInfo( BaseItemObject*  Obj )		//當角色資料有更變或換區
{
	int i;
	for( i = 0 ; i < (int)_Member.size(); i++ )
	{
		if( _Member[i].Info.DBID == Obj->DBID() )		
			break;
	}

	if( i == _Member.size() )
		return;

	GroupPartyMemberStruct& MemberInfo = _Member[i];
	MemberInfo.Info.GItemID	= Obj->LocalID();
	MemberInfo.Info.Voc = Obj->Role()->Voc;
	MemberInfo.Info.Voc_Sub = Obj->Role()->Voc_Sub;
	MemberInfo.Info.Level = Obj->Role()->LV;
	MemberInfo.Info.Level_Sub = Obj->Role()->LV_Sub;
	MemberInfo.Info.Sex = Obj->Role()->Sex;
	MemberInfo.Info.State = Obj->Role()->State;
	MemberInfo.Info.ParallelID = Obj->Role()->ParallelID;
	
	/*
	NetSrv_Party::SL_FixMember( _Info.PartyID , MemberInfo.Info ) ;

	//設定並通知所有成員
	set<int>::iterator Iter;

	for( Iter = _UserList_SockID.begin() ; Iter != _UserList_SockID.end() ; Iter++ )
	{
		int SockID = *Iter;
		NetSrv_Party::SC_FixMember( SockID , MemberInfo.Info );
	}
	*/
	FixMemberInfo( &MemberInfo.Info );
}

GroupPartyMemberStruct*  GroupParty::Member( int ID )
{
	if( _Member.size() <= (unsigned) ID )
	{
		return NULL;
	}
	return &_Member[ID];
}

GroupPartyMemberStruct*  GroupParty::MemberByDBID( int DBID )
{
	for( int i = 0 ; i < (int)_Member.size() ; i++ )
	{
		if( _Member[i].Obj->DBID() == DBID )
			return &_Member[i];
	}
	return NULL;
}
PartyMemberInfoStruct*	GroupParty::OfflineMemberByDBID( int DBID )
{
	for( int i = 0 ; i < (int)_OfflineMember.size() ; i++ )
	{
		if( _OfflineMember[i].DBID == DBID )
			return &_OfflineMember[i];
	}
	return NULL;
}

PartyMemberInfoStruct*	GroupParty::MemberByDBID_All( int DBID )
{
	for( int i = 0 ; i < (int)_Member.size() ; i++ )
	{
		if( _Member[i].Obj->DBID() == DBID )
			return &_Member[i].Info;
	}

	for( int i = 0 ; i < (int)_OfflineMember.size() ; i++ )
	{
		if( _OfflineMember[i].DBID == DBID )
			return &_OfflineMember[i];
	}
	return NULL;
}

void GroupParty::Process( )
{
	//處理物品分配
	if( _Treasure.size() != 0 )
	{
		//for( unsigned i  = 0 ; i < _Treasure.size() ; i++ )
		while( 1 )
		{
			PartyTreasureStruct* Data = &_Treasure[0];
			//分配時間還沒到
			if( Data->IsTimeUp() == false )
				break;

			for( int i = 0 ; i < Data->LootDBID.Size() ; i++ )
			{
				LootBaseInfo Temp;
				Temp.DBID = Data->LootDBID[i];
				Temp.Value = -1;
				Data->LootInfo.Push_Back( Temp );
			}

			Data->LootDBID.Clear();
			_DepartLootItem( Data , 0 );

			break;
		}
	}
}

//輪流分配
bool	GroupParty::_ShareInorder( ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_ >& LootDBID )
{
	GameObjDbStructEx* OrgDB = Global::GetObjDB( Item.OrgObjID );
	if( OrgDB == NULL )
		return false;

	int MinDBID = -1;
	int MinValue = 0x7fffffff;

	switch( _Info.ItemShareType )
	{
	case EM_PartyItemShare_Roll:
	case EM_PartyItemShare_Assign:
	case EM_PartyItemShare_InOrder:
		if( _Info.RollTypeLV <= OrgDB->Rare )
		{
			return false;
		}
//		if( _Info.RollTypeLV == 1 && Item.Inherent != 0 )
//			return false;

		break;
	default:
		return false;
	}	
	
	for( int i = 0 ; i < (int)_Member.size() ; i++ )
	{
		if( MinValue > _Member[i].ItemShareOrder )
		{
			//檢查有沒有權限
			bool CheckDBID = false;
			for( int j = 0 ; j < LootDBID.Size() ; j++ )
			{
				if( LootDBID[j] == _Member[i].Obj->DBID() )
				{
					CheckDBID = true;
					break;
				}
			}
			if( CheckDBID == true )
			{
				MinValue = _Member[i].ItemShareOrder;
				MinDBID = _Member[i].Obj->DBID();
			}
		}
	}
	GiveItemByDBID( MinDBID , Item , -1 );
	return true;
	
}

void	GroupParty::AddLootItem( ItemFieldStruct& Item , StaticVector< int , _MAX_PARTY_COUNT_ >& LootDBID )
{
	if( LootDBID.Size() <= 0 )
		return;
	if( LootDBID.Size() == 1 )
	{
		GiveItemByDBID( LootDBID[0], Item , -1 );		
		return;
	}
/*
	GameObjDbStructEx* OrgDB = Global::GetObjDB( Item.OrgObjID );
	if( OrgDB == NULL )
		return;

	//檢查那些人才可以使用
	if( _Info.Roll_CanUse == true &&  OrgDB->IsItem() )
	{
		StaticVector< int , _MAX_PARTY_COUNT_ > CanLootDBID;

		for( int i =0 ; i < LootDBID.Size() ; i++ )
		{
			int DBID = LootDBID[ i ];
			GroupPartyMemberStruct* Member;

			Member = MemberByDBID( DBID );
			if( DBID == NULL )
			{
				Print( LV2 , "_ShareInorder" , "LootDBID 資料有問題 DBID == NULL" );
				continue;
			}

			if( TestBit( OrgDB->Limit.Voc._Voc , Member->Obj->Role()->Voc ) == true )
			{
				CanLootDBID.Push_Back( DBID );
			}			
		}

		if( CanLootDBID.Size() != 0 )
			LootDBID = CanLootDBID;
	}
*/
	if( _ShareInorder( Item , LootDBID ) )
		return;


	PartyTreasureStruct Treasure;
	Treasure.Init();
	Treasure.Item = Item;
	Treasure.LootDBID = LootDBID;
	Treasure.LootInfo.Clear();
	Treasure.ShareType = _Info.ItemShareType;
	_Treasure.push_back( Treasure );

	NetSrv_PartyChild::SC_PartyTreasure( this , Treasure.Version , _Info.ItemShareType , Item , LootDBID );
}

void	GroupParty::PlayerLoot( int DBID , int ItemVersion , int AddValue )
{
	int ID;
	PartyTreasureStruct* Data;

	for( ID = 0 ; ID < (int)_Treasure.size() ; ID++ )
	{
		Data = &_Treasure[ID];

		if( Data->Version == ItemVersion )
			break;
	}
	
	if( ID >= (int)_Treasure.size() )
	{
		Print( LV2 , "PlayerLoot" , "ID >= _Treasure.size() Maybe treasure automate deal out " );
		return;
	}

	//如果為玩家指定 則不可以 擲骰子
	if(		Data->ShareType != EM_PartyItemShare_Roll 
		&&	Data->ShareType != EM_PartyItemShare_InOrder)
	{
		return;
	}

	int LootValue;
	while( 1 )
	{
		LootValue = rand() % 100 + AddValue;
		if( LootValue != Data->Loot )
			break;
	}

	if( Data->PlayerLoot( DBID , LootValue ) )
	{
		NetSrv_PartyChild::SC_PartyTreasureLoot( this , DBID , ItemVersion , LootValue );

		_DepartLootItem( Data , ID );
		/*
		//表示所有人都處理完成
		if( Data->LootDBID.Size() == 0 )
		{
			if( Data->OwnerDBID != -1 )
				GiveItemByDBID( Data->OwnerDBID , Data->Item , Data->Version );
			_Treasure.erase( _Treasure.begin() + ID );
		}
		*/
	}
	
}
//分配Loot的物品
bool	GroupParty::_DepartLootItem( PartyTreasureStruct* Data , int ID )
{
	int MinDBID = -1;
	int MinValue = 0x7ffffff;
	//如果只剩下一個人
	if( Data->LootDBID.Size() == 1  && Data->LootInfo.Size() ==  0 )
	{
		GiveItemByDBID( Data->LootDBID[0] , Data->Item , Data->Version );
		_Treasure.erase( _Treasure.begin() + ID );
		return true;
	}
	//表示所有人都處理完成
	if( Data->LootDBID.Size() != 0 )
	{
		return false;
	}
	if( Data->OwnerDBID != -1 )
	{
		GiveItemByDBID( Data->OwnerDBID , Data->Item , Data->Version );
	}
	else
	{
		if( Data->LootInfo.Size() == 0 )
		{
			return false;
		}

		if( _Member.size() == 0 )
		{
			MinValue = 0;
			MinDBID = Data->LootInfo[ rand()%Data->LootInfo.Size()].DBID;
		}
		else
		{
			for( int i = 0 ; i < (int)_Member.size() ; i++ )
			{
				if( MinValue > _Member[i].ItemShareOrder )
				{
					//檢查有沒有權限
					bool CheckDBID = false;
					for( int j = 0 ; j < Data->LootInfo.Size() ; j++ )
					{
						if( Data->LootInfo[j].DBID == _Member[i].Obj->DBID() )
						{
							CheckDBID = true;
							break;
						}
					}
					if( CheckDBID == true )
					{
						MinValue = _Member[i].ItemShareOrder;
						MinDBID = _Member[i].Obj->DBID();
					}
				}
			}

		}

		GiveItemByDBID( MinDBID , Data->Item , Data->Version );
	}

	_Treasure.erase( _Treasure.begin() + ID );

	return true;

}

void	GroupParty::PlayerGiveUp( int DBID , int ItemVersion )
{
	int ID;
	PartyTreasureStruct* Data;
	for( ID = 0 ; ID < (int)_Treasure.size() ; ID++ )
	{
		Data = &_Treasure[ID];

		if( Data->Version == ItemVersion )
			break;
	}

	if( ID >= (int)_Treasure.size() )
	{
		Print( LV2 , "PlayerGiveUp" , "ID >= _Treasure.size() Maybe treasure automate deal out " );
		return;
	}

	if( Data->PlayerGiveUp( DBID ) )
	{
		NetSrv_PartyChild::SC_PartyTreasureLoot( this , DBID , ItemVersion , -1 );
		_DepartLootItem( Data , ID );
		/*
		//表示所有人都處理完成
		if( Data->LootDBID.Size() == 0 )
		{
			if( Data->OwnerDBID != -1 )
				GiveItemByDBID( Data->OwnerDBID , Data->Item , Data->Version );
			
			else
			{
				int MinDBID = 0;
				int MinValue = 0x7fffffff;
				StaticVector< int , _MAX_PARTY_COUNT_ >& LootDBID = Data->PassDBID;

				for( int i = 0 ; i < (int)_Member.size() ; i++ )
				{
					if( MinValue > _Member[i].ItemShareOrder )
					{
						//檢查有沒有權限
						bool CheckDBID = false;
						for( int j = 0 ; j < LootDBID.Size() ; j++ )
						{
							if( LootDBID[i] == _Member[i].Obj->DBID() )
							{
								CheckDBID = true;
								break;
							}
						}
						if( CheckDBID == true )
						{
							MinValue = _Member[i].ItemShareOrder;
							MinDBID = _Member[i].Obj->DBID();
						}
					}
				}
				
			}
		}
		*/
		//_Treasure.erase( _Treasure.begin() + ID );
		
	}
}

void	GroupParty::PlayerAssist( int SenderDBID , int DBID , int ItemVersion )
{
	BaseItemObject* Sender = BaseItemObject::GetObjByDBID( SenderDBID );         //取物件	
	if( Sender == NULL )
	{
		Print( LV2 , "GiveItemByDBID" , "??找不到BaseItemObject資料 SenderDBID = %d" , DBID );
		return;
	}
	int ID;
	PartyTreasureStruct* Data;
	for( ID = 0 ; ID < (int)_Treasure.size() ; ID++ )
	{
		Data = &_Treasure[ID];

		if( Data->Version == ItemVersion )
			break;
	}

	if( ID >= (int)_Treasure.size() )
	{
		Print( LV2 , "PlayerAssist" , "ID >= _Treasure.size() Maybe treasure automate deal out " );
		return;
	}

	if( Data->ShareType != EM_PartyItemShare_Assign )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "不是物品分配者 模式" );
		//NetSrv_Talk::GameMsgEvent( Sender->SockID(), EM_GameMessageEvent_Socal_AddFriendNoPeople );
		return;
	}

	if( strcmp( Sender->RoleName() , _Info.ShareName.Begin() ) != 0 )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "不是物品分配者" );
		return;
	}

	//指定失敗
	if( Data->CheckDBID( DBID ) == false )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "DBID 有問題" );
		NetSrv_Talk::GameMsgEvent( Sender->SockID(), EM_GameMessageEvent_Party_AssistPlayerDBID_Error );
		return;
	}

	GiveItemByDBID( DBID , Data->Item , Data->Version );
	_Treasure.erase( _Treasure.begin() + ID );

}

void	GroupParty::GiveItemByDBID( int DBID  , ItemFieldStruct& Item , int ItemVersion )
{
	BaseItemObject* Member = BaseItemObject::GetObjByDBID( DBID );         //取物件	
	if( Member == NULL )
	{
		ItemSendBaseStruct Temp;
		Temp.Item = Item;
		Temp.PlayerDBID = DBID;
		Global::ItemSendList().push_back( Temp );
		//Print( LV2 , "GiveItemByDBID" , "??找不到BaseItemObject資料 DBID = %d" , DBID );
	}
	else
	{
		//通知Local
		NetSrv_PartyChild::SL_GiveItem( Member->ZoneID() , Member->DBID() , Member->LocalID() , Item );
	}
	
	for( unsigned int i = 0 ; i < _Member.size() ; i++ )
	{
		if( _Member[i].Obj == Member )
		{
			_Member[i].ItemShareOrder = _MaxShareID++;
			break;
		}
	}

	//通知玩家
	NetSrv_PartyChild::SC_GiveItemInfo( this , DBID , Item , ItemVersion );
	return;
}

//重新加入下線玩家
bool	GroupParty::AddLoginUser( int DBID )
{

	PartyMemberInfoStruct* OfflineMember = OfflineMemberByDBID( DBID );
	if( OfflineMember == NULL )
		return false;

	int PartyNo = OfflineMember->PartyNo;

	BaseItemObject*  Obj = BaseItemObject::GetObjByDBID( DBID );
	if( Obj == NULL )
	{
		DelOfflineUser( DBID );
		return false;
	}

	bool IsFind = false;
	//找出此人
	for( int i = 0 ; i < (int)_OfflineMember.size(); i++ )
	{
		if( DBID == _OfflineMember[i].DBID )
		{
			IsFind = true;
			_OfflineMember.erase( _OfflineMember.begin() + i );
			break;
		}
	}
	if( IsFind == false )
		return false;

	GroupPartyMemberStruct MemberInfo;
	MemberInfo.Info.DBID		= Obj->DBID();
	MemberInfo.Info.GItemID		= Obj->LocalID();
	MemberInfo.Info.MemberID	= (int)_Member.size();
	MemberInfo.Info.Name		= Obj->RoleName();
	MemberInfo.Info.Voc			= Obj->Role()->Voc;
	MemberInfo.Info.Voc_Sub		= Obj->Role()->Voc_Sub;
	MemberInfo.Info.Level		= Obj->Role()->LV;
	MemberInfo.Info.Level_Sub	= Obj->Role()->LV_Sub;
	MemberInfo.Info.Sex			= Obj->Role()->Sex;
	MemberInfo.Obj				= Obj;
	MemberInfo.ItemShareOrder	= _MaxShareID++;
	MemberInfo.Info.PartyNo		= 0;
	MemberInfo.Info.Mode._Mode	= 0;
	MemberInfo.Info.State._State = 0;

//	bool	IsEmptyNumber[_MAX_PARTY_COUNT_];
//	memset( IsEmptyNumber , 0 , sizeof( IsEmptyNumber ) );

	/*
	for( unsigned int i = 0 ; _Member.size() ; i++ )
	{
		if( _Member[i].Info.PartyNo >= _MAX_PARTY_COUNT_ )
			_Member[i].Info.PartyNo = _MAX_PARTY_COUNT_ - 1;

		IsEmptyNumber[ _Member[i].Info.PartyNo ] = true;
	}

	for( int i = 0 ; i < _MAX_PARTY_COUNT_ ; i++ )
	{
		if( IsEmptyNumber[ i ] == false )
		{
			MemberInfo.Info.PartyNo = i;
			break;
		}
	}
*/
	MemberInfo.Info.PartyNo = PartyNo;
	_Member.push_back( MemberInfo );

	NetSrv_Party::SC_PartyBaseInfo( Obj->SockID() , _Info );

	NetSrv_Party::SL_AddMember( _Info.PartyID , MemberInfo.Info ) ;

	//設定並通知所有成員
	Obj->PartyID( _ID );

	for( unsigned int i = 0 ; i < _Member.size() ; i++ )
	{
		//if( Obj != _Member[i].Obj )
		{
			NetSrv_Party::SC_AddMember( Obj->SockID() , _Member[i].Info );
		}
		//NetSrv_Party::SC_AddMember( _Member[i].Obj->SockID() , MemberInfo.Info );
	}

	OnChangeRoleInfo( Obj );


	//通知所有下線的成員
	for( unsigned int i = 0 ; i < _OfflineMember.size() ; i++ )
	{
		NetSrv_Party::SC_AddMember( Obj->SockID() , _OfflineMember[i] );
	}

	//////////////////////////////////////////////////////////////////////////
	//重設掉寶資料
	ResetPlayerLootInfo( Obj->DBID() );
	//////////////////////////////////////////////////////////////////////////
	//插入管理資料
	pair< set<int>::iterator, bool > Result;
	Result = _UserList_SockID.insert( Obj->SockID() );

	if( Result.second == false )
		return false;


	typedef multimap< int , GroupObjectClass* >::value_type valueType;
	_UserGroupInfo.insert( valueType( Obj->DBID() , this ) );

	return true;
}

//修正成員位置
bool GroupParty::SetPartyMemberPos( int MemberDBID[2] , int Pos[2] )
{
	int BkPos[2];

	if(		(unsigned)Pos[0] > _MAX_PARTY_COUNT_ 
		||	(unsigned)Pos[1] > _MAX_PARTY_COUNT_ )
	{		
		return false;
	}
	for( int i = 0 ; i < 2 ; i++ )
	{
		PartyMemberInfoStruct*	Member = MemberByDBID_All( MemberDBID[i] );
		if( Member == NULL )
			continue;
		BkPos[ i ] = Member->PartyNo;
		Member->PartyNo = Pos[i];
	}

	//檢查是否有重復的ID
	bool	IsEmptyNumber[_MAX_PARTY_COUNT_];
	memset( IsEmptyNumber , 0 , sizeof( IsEmptyNumber ) );

	for( int i = 0 ; i < (int)_Member.size() ; i++ )
	{
		if( _Member[i].Info.PartyNo >= _MAX_PARTY_COUNT_ )
			_Member[i].Info.PartyNo = _MAX_PARTY_COUNT_ - 1;

		int PartyNo = _Member[i].Info.PartyNo;

		if( IsEmptyNumber[ PartyNo ] == true  )
		{
			//表示重覆
			for( int i = 0 ; i < 2 ; i++ )
			{
				PartyMemberInfoStruct*	Member = MemberByDBID_All( MemberDBID[i] );
				if( Member == NULL )
					continue;
				Member->PartyNo = BkPos[i];
			}			
			return false;
		}
		IsEmptyNumber[ PartyNo ] = true;
	}

	for( int i = 0 ; i < 2 ; i++ )
	{
		PartyMemberInfoStruct*	Member = MemberByDBID_All( MemberDBID[i] );
		if( Member == NULL )
			continue;

		FixMemberInfo( Member );

	}

	return true;
}

//設定Party 類型
void	GroupParty::SetPartyType( PartyTypeENUM Type )
{
	_Info.Type =Type;
	return;
}

//修正成員資料
void	GroupParty::FixMemberInfo( PartyMemberInfoStruct* Member )
{
	NetSrv_Party::SL_FixMember( Info()->PartyID , *Member ) ;

	//設定並通知所有成員
	set<int>::iterator Iter;
	for( Iter = _UserList_SockID.begin() ; Iter != _UserList_SockID.end() ; Iter++ )
	{
		int SockID = *Iter;
		NetSrv_Party::SC_FixMember( SockID , *Member );
	}
}