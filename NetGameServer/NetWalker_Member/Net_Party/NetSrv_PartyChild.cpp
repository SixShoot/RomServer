#include "../NetWakerGSrvInc.h"
#include "NetSrv_PartyChild.h"
#include "../../mainproc/PartyInfoList/PartyInfoList.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_PartyChild::Init()
{
    NetSrv_Party::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_PartyChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_PartyChild::Release()
{
    if( This == NULL )
        return false;

	NetSrv_Party::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-------------------------------------------------------------------
//chat 
void NetSrv_PartyChild::R_PartyBaseInfo	( PartyBaseInfoStruct& PartyInfo )
{
	PartyInfoListClass::This()->AddPartyBaseInfo( PartyInfo );
}
void NetSrv_PartyChild::R_AddMember		( int PartyID , PartyMemberInfoStruct& Member )
{
	//成員檢查
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( PartyID );
	if( Party != NULL )
	{
		for( int i = 0 ; i < (int)Party->Member.size() ; i++ )
		{
			if( Party->Member[i].ZoneID != RoleDataEx::G_ZoneID )
				continue;

			BaseItemObject* MemberObj = BaseItemObject::GetObj( Party->Member[i].GItemID );
			if( MemberObj == NULL )
				continue;
			
			NetSrv_Attack::SC_PartyMemberMaxHPMPSP	( Member.DBID , MemberObj->Role() );
			NetSrv_Attack::SC_PartyMemberHPMPSP		( Member.DBID , MemberObj->Role() );
			NetSrv_Magic::S_PartyMemberBuff( Member.DBID , MemberObj->Role() );
			NetSrv_Move::S_PartyMemberPos( Member.DBID , MemberObj->Role()->DBID() , MemberObj->Role()->GUID() , MemberObj->Role()->Pos()->X  , MemberObj->Role()->Pos()->Y , MemberObj->Role()->Pos()->Z );
		}
	}
		

	PartyInfoListClass::This()->AddMember( PartyID , Member );
//	BaseItemObject* Obj = BaseItemObject::GetObj( Member.GItemID );
	BaseItemObject* Obj = BaseItemObject::GetObjByDBID( Member.DBID );
	if( Obj == NULL )
		return;
	Obj->Role()->BaseData.PartyID = PartyID;

	if( Obj->Role()->TempData.Attr.AllZoneState.HasParty == false )
	{
		Obj->Role()->TempData.Attr.AllZoneState.HasParty = true;
		Obj->Role()->TempData.UpdateInfo.AllZoneInfoChange = 1;
	}

	//通知所有成原 HPMP資訊
	NetSrv_AttackChild::SendPartyMaxHPMaxMP( Obj->Role() );
	NetSrv_AttackChild::SendPartyHPMP( Obj->Role() );
	NetSrv_MoveChild::SendAllPartyMemberPos( Obj->Role() );
	Obj->Role()->TempData.BackInfo.ZonePlayerInfo.BuffIDCheckSum = 0;
	NetSrv_Magic::S_AllMember_PartyMemberBuff( Obj->Role() );
}
void NetSrv_PartyChild::R_DelMember		( int PartyID , PartyMemberInfoStruct& Member , bool IsOffline )
{
	PartyInfoListClass::This()->DelMember( PartyID , Member );

	if( IsOffline != false )
		return;

	BaseItemObject* Obj = BaseItemObject::GetObj( Member.GItemID );
	if( Obj == NULL )
		return;
	if( Obj->Role()->BaseData.PartyID != PartyID )
	{
		Print( LV2 , "R_DelMember" ,"Obj->Role()->BaseData.PartyID != PartyID " );
		return;
	}
	Obj->Role()->BaseData.PartyID = -1;

	if( Obj->Role()->TempData.Attr.AllZoneState.HasParty != false)
	{
		Obj->Role()->TempData.Attr.AllZoneState.HasParty = false;
		Obj->Role()->TempData.UpdateInfo.AllZoneInfoChange = 1;
	}
}
void NetSrv_PartyChild::R_FixMember		( int PartyID , PartyMemberInfoStruct& Member )
{
	PartyInfoListClass::This()->FixMember( PartyID , Member );
}
//-------------------------------------------------------------------
void NetSrv_PartyChild::R_GiveItem( int DBID , int GItemID , ItemFieldStruct& Item )
{
	RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
	if( Role == NULL || Role->DBID() != DBID )
	{
		Print( LV2 , "R_GiveItem" , "Role == NULL || Role->DBID() != DBID" );
		return;
	}
	Role->GiveItem( Item , EM_ActionType_FromParty , NULL , "" );
}


bool NetSrv_PartyChild::SendItemToParty( StaticVector< BaseItemObject* , _MAX_PARTY_COUNT_  >& LootObj , ItemFieldStruct& Item )
{
	GameObjDbStructEx* OrgDB = Global::GetObjDB( Item.OrgObjID );
	if( OrgDB == NULL )
		return false;

	if( LootObj.Size() == 0 )
		return false;

	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( LootObj[0]->Role()->BaseData.PartyID );
	if( Party == NULL )
		return false;

	StaticVector< int , _MAX_PARTY_COUNT_  > LootDBID;
	StaticVector< int , _MAX_PARTY_COUNT_  > AllLootDBID;
	for( int i = 0 ; i < LootObj.Size() ; i++ )
	{
		int DBID = LootObj[i]->Role()->DBID();
		BaseItemObject* OtherObj = LootObj[i];//BaseItemObject::GetObjByDBID( DBID );

		if( OtherObj == NULL )
			continue;

		RoleValueData* Attr = &(OtherObj->Role()->TempData.Attr);

		AllLootDBID.Push_Back( DBID );
		
		if( Party->Info.Roll_CanUse == false )
			continue;

		//檢查職業
		if( OrgDB->Limit.Voc._Voc != 0 && TestBit( OrgDB->Limit.Voc._Voc , Attr->Voc ) == false )
			continue;

		//檢查技能
		if( OrgDB->CheckItemAndEqSkill( Attr->Fin.WearSkill ) == false )
			continue;

		LootDBID.Push_Back( DBID );
	}

	//沒人能用
	if( LootDBID.Size() == 0 )
		LootDBID = AllLootDBID;



	S_PartyTreasure( LootObj[0]->Role()->BaseData.PartyID , Item , LootDBID );

	
	return true;
}

void NetSrv_PartyChild::OnChatServerDisconnect( )
{
	PartyInfoListClass::Init( );
}

void NetSrv_PartyChild::R_PartyReloginResult( int PartyID , int PlayerDBID , bool Result )
{
	BaseItemObject *Obj = BaseItemObject::GetObjByDBID( PlayerDBID );

	if( Obj == NULL )
		return;

	if( Result != false )
		Obj->Role()->BaseData.PartyID =  PartyID ;
	else
		Obj->Role()->BaseData.PartyID =  -1;

}

void NetSrv_PartyChild::RC_Zone_Invite				( BaseItemObject* Obj , int InviteDBID , int ClientInfo )
{
	if( Global::Ini()->IsZonePartyEnabled == false || Global::Ini()->IsDisableParty != false )
		return;
	

	//通知邀請者
	RoleDataEx* Owner = Obj->Role();
	BaseItemObject* TargetObj = Global::GetPlayerObj_ByDBID( InviteDBID );
	if( TargetObj == NULL )
	{
		Owner->Msg( "找不到目標" );
		return;
	}

	//查看看是否為隊長
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( Owner->PartyID() );
	if( Party != NULL )
	{

		if( Party->Member.size() != 0 && Party->Member[0].DBID != Owner->DBID() )
		{
			Owner->Msg( "非隊長不可邀請" );
			return;
		}
	}

	SC_Zone_Invite( TargetObj->GUID() , Owner->DBID() , ClientInfo );

}

bool NetSrv_PartyChild::Zone_Join( int GItemID , int PartyID )
{
	RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
	if( Owner == NULL || PartyID > 1000 )
		return false;

	PartyMemberInfoStruct NewMember;
	NewMember.Init();

	PartyInfoStruct* Party = NULL;

	while( 1 )
	{
		Party = PartyInfoListClass::This()->GetPartyInfo( PartyID );
		if( Party != NULL )
		{
			if( Party->Info.LeaderName.Size() == 0 )
			{
				Party->Info.LeaderName = Owner->RoleName();
			}
			break;
		}

		PartyBaseInfoStruct NewPartyInfo;
		NewPartyInfo.Type = EM_PartyType_Raid;
		NewPartyInfo.RollTypeLV = 100;
		NewPartyInfo.Roll_CanUse = false;
		NewPartyInfo.LeaderName = "";
		PartyInfoListClass::This()->AddPartyBaseInfo_Zone( NewPartyInfo );

	}

	//取得隊長的DBID
	//BaseItemObject* LeaderObj = Global::GetObjByName( Party->Member[0].DBID )


	vector< PartyMemberInfoStruct >& PartyMember = Party->Member;
	//找空的PartyNo
	int		NewPartyNo;

	for( NewPartyNo = 0 ; NewPartyNo < 100 ; NewPartyNo++  )
	{
		bool	IsFind = false;

		for( unsigned i = 0 ; i < PartyMember.size() ; i++ )
		{
			if( PartyMember[i].PartyNo == NewPartyNo )
			{
				IsFind = true;
				break;
			}
		}

		if( IsFind == false )
			break;
	}

	NewMember.MemberID	= (int)PartyMember.size();
	NewMember.PartyNo	= NewPartyNo;					//小隊ID
	NewMember.Name		= Owner->RoleName();	
	NewMember.GItemID	= Owner->GUID();		// -1 代表下線狀態
	NewMember.DBID		= Owner->DBID();
	NewMember.Voc		= Owner->BaseData.Voc;								//職業
	NewMember.Voc_Sub	= Owner->BaseData.Voc_Sub;							//副職業
	NewMember.Level		= Owner->TempData.Attr.Level;
	NewMember.Level_Sub = Owner->TempData.Attr.Level_Sub;
	NewMember.Sex		= Owner->BaseData.Sex;
	Owner->BaseData.PartyID = Party->Info.PartyID;

	if( Party->Member.size() == 0 )
		SC_Zone_PartyBase			( Owner->GUID() , Party->Info.PartyID , Owner->DBID() );
	else
		SC_Zone_PartyBase			( Owner->GUID() , Party->Info.PartyID , Party->Member[0].DBID );

	for( unsigned i = 0 ; i < PartyMember.size() ; i++ )
	{
		SC_Zone_AddMember			( PartyMember[i].GItemID , Party->Info.PartyID , Owner->DBID() , NewMember.PartyNo , NewMember.MemberID );
		SC_Zone_AddMember			( Owner->GUID() , Party->Info.PartyID , PartyMember[i].DBID , PartyMember[i].PartyNo , PartyMember[i].MemberID );
	}
	SC_Zone_AddMember			( Owner->GUID() , Party->Info.PartyID , Owner->DBID() , NewMember.PartyNo , NewMember.MemberID );

	PartyInfoListClass::This()->AddMember( Party->Info.PartyID , NewMember );
	return true;	
}

bool	 NetSrv_PartyChild::Zone_Clear( int PartyID )
{

	//查看看是否為隊長
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( PartyID );
	if( Party == NULL )
	{
		return false;  
	}


	vector< PartyMemberInfoStruct >& PartyMember = Party->Member;

	for( unsigned i = 0 ; i < PartyMember.size() ; i++ )
	{
		RoleDataEx* Member = Global::GetRoleDataByGUID( PartyMember[i].GItemID );
		if( Member == NULL )
			continue;
		SC_Zone_DelMember	( Member->GUID() , Member->PartyID() , Member->DBID()  );
		Member->BaseData.PartyID = -1;
	}

	PartyMember.clear();
	return true;
}

void NetSrv_PartyChild::RC_Zone_Join				( BaseItemObject* Obj , int LeaderDBID )
{
	if( Global::Ini()->IsZonePartyEnabled == false || Global::Ini()->IsDisableParty != false )
		return;


	RoleDataEx* Owner = Obj->Role();
	BaseItemObject* LeaderObj = Global::GetPlayerObj_ByDBID( LeaderDBID );
	if( LeaderObj == NULL )
	{
		Owner->Msg( "找不到隊長目標" );
		return;
	}
	RoleDataEx* Leader = LeaderObj->Role();
	PartyMemberInfoStruct NewMember;
	NewMember.Init();

	//查看看是否為隊長
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( Leader->PartyID() );
	if( Party == NULL )
	{
		PartyBaseInfoStruct NewPartyInfo;
		NewPartyInfo.Type = EM_PartyType_Raid;
		NewPartyInfo.RollTypeLV = 100;
		NewPartyInfo.Roll_CanUse = false;
		NewPartyInfo.LeaderName = Leader->RoleName();
		int PartyID = PartyInfoListClass::This()->AddPartyBaseInfo_Zone( NewPartyInfo );

		NewMember.MemberID	= 0;
		NewMember.PartyNo	= 0;					//小隊ID
		NewMember.Name		= Leader->RoleName();	
		NewMember.GItemID	= Leader->GUID();		// -1 代表下線狀態
		NewMember.DBID		= Leader->DBID();
		NewMember.Voc		= Leader->BaseData.Voc;								//職業
		NewMember.Voc_Sub	= Leader->BaseData.Voc_Sub;							//副職業
		NewMember.Level		= Leader->TempData.Attr.Level;
		NewMember.Level_Sub = Leader->TempData.Attr.Level_Sub;
		NewMember.Sex		= Leader->BaseData.Sex;
		Leader->BaseData.PartyID = PartyID;

		PartyInfoListClass::This()->AddMember( PartyID , NewMember );

		NewMember.MemberID	= 1;
		NewMember.PartyNo	= 1;					//小隊ID
		NewMember.Name		= Owner->RoleName();	
		NewMember.GItemID	= Owner->GUID();		// -1 代表下線狀態
		NewMember.DBID		= Owner->DBID();
		NewMember.Voc		= Owner->BaseData.Voc;								//職業
		NewMember.Voc_Sub	= Owner->BaseData.Voc_Sub;							//副職業
		NewMember.Level		= Owner->TempData.Attr.Level;
		NewMember.Level_Sub = Owner->TempData.Attr.Level_Sub;
		NewMember.Sex		= Owner->BaseData.Sex;
		Owner->BaseData.PartyID = PartyID;

		PartyInfoListClass::This()->AddMember( PartyID , NewMember );

		//通知Client
		SC_Zone_PartyBase			( Leader->GUID() , PartyID , Leader->DBID() );
		SC_Zone_PartyBase			( Owner->GUID() , PartyID , Leader->DBID() );

		vector< PartyMemberInfoStruct >& PartyMember = PartyInfoListClass::This()->GetPartyInfo( PartyID )->Member;

		for( unsigned i = 0 ; i < PartyMember.size() ; i++ )
		{
			SC_Zone_AddMember			( Leader->GUID() , PartyID , PartyMember[i].DBID , PartyMember[i].PartyNo , PartyMember[i].MemberID );
			SC_Zone_AddMember			( Owner->GUID() , PartyID , PartyMember[i].DBID , PartyMember[i].PartyNo , PartyMember[i].MemberID );
		}
	}
	else
	{
		vector< PartyMemberInfoStruct >& PartyMember = Party->Member;
		//找空的PartyNo
		int		NewPartyNo;
		
		for( NewPartyNo = 0 ; NewPartyNo < 100 ; NewPartyNo++  )
		{
			bool	IsFind = false;

			for( unsigned i = 0 ; i < PartyMember.size() ; i++ )
			{
				if( PartyMember[i].PartyNo == NewPartyNo )
				{
					IsFind = true;
					break;
				}
			}

			if( IsFind == false )
				break;
		}

		NewMember.MemberID	= (int)PartyMember.size();
		NewMember.PartyNo	= NewPartyNo;					//小隊ID
		NewMember.Name		= Owner->RoleName();	
		NewMember.GItemID	= Owner->GUID();		// -1 代表下線狀態
		NewMember.DBID		= Owner->DBID();
		NewMember.Voc		= Owner->BaseData.Voc;								//職業
		NewMember.Voc_Sub	= Owner->BaseData.Voc_Sub;							//副職業
		NewMember.Level		= Owner->TempData.Attr.Level;
		NewMember.Level_Sub = Owner->TempData.Attr.Level_Sub;
		NewMember.Sex		= Owner->BaseData.Sex;
		Owner->BaseData.PartyID = Party->Info.PartyID;
		
		SC_Zone_PartyBase			( Owner->GUID() , Party->Info.PartyID , Leader->DBID() );
		for( unsigned i = 0 ; i < PartyMember.size() ; i++ )
		{
			SC_Zone_AddMember			( PartyMember[i].GItemID , Party->Info.PartyID , Owner->DBID() , NewMember.PartyNo , NewMember.MemberID );
			SC_Zone_AddMember			( Owner->GUID() , Party->Info.PartyID , PartyMember[i].DBID , PartyMember[i].PartyNo , PartyMember[i].MemberID );
		}
		SC_Zone_AddMember			( Owner->GUID() , Party->Info.PartyID , Owner->DBID() , NewMember.PartyNo , NewMember.MemberID );

		PartyInfoListClass::This()->AddMember( Party->Info.PartyID , NewMember );
	}

}
void NetSrv_PartyChild::RC_Zone_KickMember			( BaseItemObject* Obj , int MemberDBID )
{
	if( Global::Ini()->IsZonePartyEnabled == false  )
		return;

	RoleDataEx* Owner = Obj->Role();
	BaseItemObject* MemberObj = Global::GetPlayerObj_ByDBID( MemberDBID );
	if( MemberObj == NULL )
	{
		Owner->Msg( "找不到目標" );
		return;
	}
	RoleDataEx* Member = MemberObj->Role();

	if( MemberObj->Role()->PartyID() != Owner->PartyID() )
	{
		Owner->Msg( "此人不在隊伍內" );
		return;
	}

	//查看看是否為隊長
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( Owner->PartyID() );
	if( Party == NULL )
	{
		Owner->Msg( "沒有party資料" );
		return; 
	}

	if( Party->Member.size() == 0 )
		return;

	if( Party->Member[0].DBID != Owner->DBID() && Owner->DBID() != MemberDBID )
	{
		Owner->Msg( "非隊長不可踢人" );
		return;
	}


	vector< PartyMemberInfoStruct >& PartyMember = Party->Member;

	if( PartyMember.size() == 2 && Global::Ini()->IsDisableParty == false )
	{
		for( unsigned i = 0 ; i < PartyMember.size() ; i++ )
		{
			RoleDataEx* Other = Global::GetRoleDataByGUID( PartyMember[i].GItemID );
			if( Other == NULL )
				continue;
			Other->BaseData.PartyID = -1;
		}

		SC_Zone_DelMember( PartyMember[0].GItemID , Owner->PartyID() , PartyMember[0].DBID );
		SC_Zone_DelMember( PartyMember[1].GItemID , Owner->PartyID() , PartyMember[1].DBID );
		PartyInfoListClass::This()->DelPartyID_Zone( Owner->PartyID() );
		PartyMember.clear();
	}
	else
	{
		int ErasePos = -1;
		for( unsigned i = 0 ; i < PartyMember.size() ; i++ )
		{
			if( PartyMember[i].DBID == MemberDBID )
				ErasePos = i;

			SC_Zone_DelMember( PartyMember[i].GItemID , Member->PartyID() , Member->DBID() );
		}

		if( ErasePos != -1 )
		{
			PartyMember.erase( PartyMember.begin() + ErasePos );

			if(  ErasePos == 0 )
			{
				for( unsigned i = 0 ; i < PartyMember.size() ; i++ )
				{
					SC_Zone_PartyBase	( PartyMember[i].GItemID , Owner->PartyID() , PartyMember[0].DBID );
				}
			}
		}
		Member->BaseData.PartyID = -1;
	}


}
void NetSrv_PartyChild::RC_Zone_SetMemberPosRequest	( BaseItemObject* Obj , int MemberDBID[2] , int Pos[2] )
{
	if( Global::Ini()->IsZonePartyEnabled == false || Global::Ini()->IsDisableParty != false )
		return;

	RoleDataEx* Owner = Obj->Role();
	//查看看是否為隊長
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( Owner->PartyID() );
	if( Party == NULL )
	{
		SC_Zone_SetMemberPosResult( Owner->GUID() , false );
		Owner->Msg( "沒有party資料" );
		return; 
	}
	if( Party->Member[0].DBID != Owner->DBID() )
	{
		SC_Zone_SetMemberPosResult( Owner->GUID() , false );
		Owner->Msg( "非隊長不可以整理隊伍" );
		return;
	}

	vector< PartyMemberInfoStruct >& PartyMember = Party->Member;
	for( unsigned i = 0 ; i < PartyMember.size() ; i++ )
	{
		for( int j = 0 ; j < 2 ; j++ )
		{
			if( PartyMember[i].DBID == MemberDBID[j] )
			{
				PartyMember[i].PartyNo = Pos[j];
				SC_Member_Zone_FixMember( Owner->PartyID() , MemberDBID[j] , Pos[j] , i );
			}
		}
	}
	SC_Zone_SetMemberPosResult( Owner->GUID() , true );
}

void NetSrv_PartyChild::RC_Zone_ChangeLeader		( BaseItemObject* Obj , int NewLeaderDBID )
{
	if( Global::Ini()->IsZonePartyEnabled == false || Global::Ini()->IsDisableParty != false )
		return;

	RoleDataEx* Owner = Obj->Role();

	if( NewLeaderDBID == Owner->DBID())
	{
		SC_Zone_ChangeLeaderResult( Owner->GUID() , false );
		return;
	}

	//查看看是否為隊長
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( Owner->PartyID() );
	if( Party == NULL )
	{
		SC_Zone_ChangeLeaderResult( Owner->GUID() , false );
		Owner->Msg( "沒有party資料" );
		return; 
	}
	if( Party->Member[0].DBID != Owner->DBID()  )
	{
		SC_Zone_ChangeLeaderResult( Owner->GUID() , false );
		Owner->Msg( "非隊長不可以整理隊伍" );
		return;
	}
	
	//找出新隊長
	bool Ret = false;
	for( unsigned i = 1 ; i < Party->Member.size() ; i++ )
	{
		if( Party->Member[i].DBID == NewLeaderDBID )
		{
			swap( Party->Member[i] , Party->Member[0] );
			//SC_Zone_ChangeLeaderResult( Owner->GUID() , true );
			Ret = true;
			break;
		}
	}

	if( Ret )
	{
		for( unsigned i = 0 ; i < Party->Member.size() ; i++ )
		{
			SC_Zone_PartyBase( Party->Member[i].GItemID , Owner->PartyID() , NewLeaderDBID );
		}
	}

	SC_Zone_ChangeLeaderResult( Owner->GUID() , Ret );
}

void NetSrv_PartyChild::RC_Zone_Talk				( BaseItemObject* Obj , const char* Content )
{
	if( Global::Ini()->IsZonePartyEnabled == false  )
		return;

	RoleDataEx* Owner = Obj->Role();
	SC_Zone_Talk( Owner->GUID()  , Content  );
}

void NetSrv_PartyChild::RC_GetMemberInfo(  BaseItemObject* Obj , int LeaderDBID )
{
	PartyMemberInfoStruct Member;

	OnlinePlayerInfoStruct* LeaderInfo = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( LeaderDBID );
	if( LeaderInfo == NULL )
	{
		SC_GetMemberInfoResult( Obj->Role()->DBID() , LeaderDBID , 0, 0 , Member );
		return;
	}
	
	//RoleDataEx* LeaderRole = Global::GetNPCObj_ByDBID( LeaderDBID )
	if( LeaderInfo->ZoneID != RoleDataEx::G_ZoneID )
	{
		SL_GetMemberInfo( LeaderInfo->ZoneID , Obj->Role()->DBID()  ,LeaderDBID );
	}
	else
	{
		RL_GetMemberInfo	( Obj->Role()->DBID() , LeaderDBID );
	}

}
void NetSrv_PartyChild::RL_GetMemberInfo	( int RequestPlayerDBID , int LeaderDBID )
{
	PartyMemberInfoStruct Member;
	RoleDataEx* LeaderRole = Global::GetRoleDataByDBID( LeaderDBID );
	if( LeaderRole == NULL )
	{
		SC_GetMemberInfoResult( RequestPlayerDBID , LeaderDBID , 0, 0 , Member );
		return;
	}

	
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( LeaderRole->PartyID() );
	if( Party == NULL )
	{
		Member.Init();

		Member.Name		= LeaderRole->RoleName();	
		Member.GItemID	= LeaderRole->GUID();
		Member.DBID		= LeaderRole->DBID();
		Member.Voc		= LeaderRole->TempData.Attr.Voc;
		Member.Voc_Sub	= LeaderRole->TempData.Attr.Voc_Sub;
		Member.Level	= LeaderRole->TempData.Attr.Level;
		Member.Level_Sub = LeaderRole->TempData.Attr.Level_Sub;
		Member.Sex		= LeaderRole->BaseData.Sex;
		Member.ParallelID = LeaderRole->PlayerTempData->ParallelZoneID;

		SC_GetMemberInfoResult( RequestPlayerDBID , LeaderDBID , 1, 0 , Member );
		return; 
	}
	else
	{
		for( unsigned i = 0 ; i < Party->Member.size() ; i++ )
		{
			SC_GetMemberInfoResult( RequestPlayerDBID , LeaderDBID , (int)Party->Member.size() , i , Party->Member[i] );
		}

	}
}