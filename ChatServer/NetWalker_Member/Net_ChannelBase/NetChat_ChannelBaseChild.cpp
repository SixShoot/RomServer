#pragma warning (disable:4996)

#include "../NetWakerChatInc.h"
#include "NetChat_ChannelBaseChild.h"
#include "../../MainProc/GroupPlayerChannel/GroupPlayerChannel.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
#include "../../MainProc/groupparty/GroupParty.h"


int		NetSrv_ChannelBaseChild::_DSysTime = 0;
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_ChannelBaseChild::Init()
{
    NetSrv_ChannelBase::_Init();

    if( This != NULL)
        return false;

    This = new NetSrv_ChannelBaseChild;

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_ChannelBaseChild::Release()
{
    if( This == NULL )
        return false;

    delete This;
    This = NULL;

    NetSrv_ChannelBase::_Release();
    return true;
    
}
//-------------------------------------------------------------------
//  事件繼承實做
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::CliConnect( int CliID , string Account )
{
	GSrv_CliInfoStruct* CliNetInfo = _Net->GetCliNetInfo( CliID );
	if( CliNetInfo == NULL )
		return;

	BaseItemObject* OldObj = BaseItemObject::GetObjByAccount( (char*)Account.c_str() );
	if( OldObj != NULL )
		delete OldObj;

    BaseItemObject* Obj = new BaseItemObject( CliID , CliNetInfo->ProxyID , (char*)Account.c_str() );
	ClientConnect( CliID );

	Send_SysChannelInfo( CliID );

	SCli_SystemTime( CliID , TimeStr::Now_Value() + _DSysTime );

}
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::SetDSysTime		( int DSysTime )
{
	if( DSysTime != 0 )
		_DSysTime += DSysTime %( 24*60 ) * 60;
	else
		_DSysTime = 0;

	//__time32_t time = (__time32_t)(TimeStr::Now_Value() + _DSysTime);
	//tm* newTime = _localtime32(&time);
	SSrv_SendAll_SystemTime( TimeStr::Now_Value() + _DSysTime );
	SCli_SendAll_SystemTime( TimeStr::Now_Value() + _DSysTime );
}
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::CliDisconnect( int CliID )
{
    BaseItemObject* Obj = BaseItemObject::GetObjBySockID( CliID );

	if( Obj == NULL )
	{
		Print( LV2 ,  "CliDisconnect" , "找不到此SockID=%d" , CliID );
		return;
	}	

	multimap< int , GroupObjectClass* >::iterator  MulIter,BegIter;
	int Count = (int)GroupObjectClass::UserGroupInfo()->count( Obj->DBID() );
	int i;

	BegIter = MulIter = GroupObjectClass::UserGroupInfo()->find( Obj->DBID() );

	vector< GroupObjectClass*  > GroupObjList;
	for( i = 0 ; i < Count ; i++ , MulIter++ )
	{
		GroupObjectClass* GroupObj = MulIter->second;
		if( GroupObj->Type() != EM_GroupObjectType_PlayerChannel )
			continue;

		 GroupObjList.push_back( GroupObj );
	}

	for(  i = 0 ; i < (int)GroupObjList.size() ; i++ )
	{
		GroupObjectClass* GroupObj = GroupObjList[i];
		DelUser( Obj , GroupObj->Type() , GroupObj->ID() );
	}

	if( Obj->Role()->PartyID != -1 && Obj->Role()->State.HonorParty )
	{//榮譽組隊的話，隊長下線就解散對伍
		GroupParty* Party;
		Party = (GroupParty*)GroupObjectClass::GetObj( EM_GroupObjectType_Party , Obj->Role()->PartyID );
		if( Party != NULL )
		{
			vector< GroupPartyMemberStruct > member =  *Party->Member();
			for( int i = 0 ; i < member.size() ; i++ )
			{
				Party->DelUser( member[i].Obj );	
			}
			if( Party->Member()->size() == 0 )
			{
				delete Party;
			}
		}
	}

	Print( LV1 ,  "NetSrv_ChannelBaseChild::CliDisconnect" , " CliID = %d Account = %s" , CliID , Obj->Account() );

	if( Obj->Role()->RoleName.Size() != 0 )
		SL_DelOnlinePlayer( Obj->RoleName() , Obj->DBID() );

    Obj->Destroy();
}
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::Send_SysChannelInfo( int SockID )
{
	/*
	for( unsigned int i = 0 ; i < RoleDataEx::SysChannel.size() ; i++ )
	{
		SysChannelInfo( SockID , EM_GroupObjectType_SysChannel , i , (char*)RoleDataEx::SysChannel[i].c_str() );
	}
*/
	for( unsigned int i = 0 ; i < RoleDataEx::ZoneChannel.size() ; i++ )
	{
		SysChannelInfo( SockID , EM_GroupObjectType_SysZoneChannel , i , (char*)RoleDataEx::ZoneChannel[i].c_str() );
	}
}
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::DataTransmitToGSrv_Group( BaseItemObject* Sender , GroupObjectClass* TarGroup , int Size , PacketBaseMaxStruct* Data )
{

	set< int >* UserList = TarGroup->UserList_SockID();
	set< int >::iterator Iter;
	BaseItemObject*  Obj;

	for( Iter = UserList->begin() ; Iter != UserList->end() ; Iter ++ )
	{
		Obj = BaseItemObject::GetObjBySockID( *Iter );
		if( Obj == NULL )
			continue;
		SendToLocal( Obj->ZoneID() , Size , Data );
	}   
}
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::DataTransmitToCli_Group( BaseItemObject* Sender , GroupObjectClass* TarGroup , int Size , PacketBaseMaxStruct* Data )
{

	set< int >* UserList = TarGroup->UserList_SockID();
	set< int >::iterator Iter;

	for( Iter = UserList->begin() ; Iter != UserList->end() ; Iter ++ )
	{
		int SockID = *Iter ;
		
		SendToCli( SockID , Size , Data );
	}
}
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::DataTransmit_CliToCli_Name  ( BaseItemObject* Sender , BaseItemObject* ToName , int Size , char* Data )
{
	PG_ChannelBase_ChattoC_DataTransmitCliToCli Send;
	
	if( Size >= sizeof( Send.Data ) )
	{
		Print( LV2 , "DataTransmitToCli_Name" , "要求轉送資料過大 Size = %d" , Size );
		return;
	}

	Send.DataSize = Size;
	memcpy( Send.Data  , Data , Size );

	SendToCli( ToName->SockID() , sizeof( Send) , &Send );

}
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::DataTransmit_CliToCli_Group ( BaseItemObject* Sender , GroupObjectClass* TarGroup , int Size , char* Data )
{

	PG_ChannelBase_ChattoC_DataTransmitCliToCli Send;
	if( Size >= sizeof( Send.Data ) )
	{
		Print( LV2 , "DataTransmit_CliToCli_Group" , "要求轉送資料過大 Size = %d" , Size );
		return;
	}
	Send.DataSize = Size;
	memcpy( Send.Data  , Data , Size );

	set< int >* UserList = TarGroup->UserList_SockID();
	set< int >::iterator Iter;
	

	for( Iter = UserList->begin() ; Iter != UserList->end() ; Iter ++ )
	{
		int SockID = *Iter;

		SendToCli( SockID , sizeof( Send) , &Send );
	}   
}
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::DataTransmitToGSrv( BaseItemObject* Sender , BaseItemObject* TarObj , int Size , PacketBaseMaxStruct* Data )
{
    SendToLocal( TarObj->ZoneID() , Size , Data );
}
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::DataTransmitToCli( BaseItemObject* Sender , BaseItemObject* TarObj , int Size , PacketBaseMaxStruct* Data )
{
    SendToCli( TarObj->SockID() , Size , Data );
}
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::CreateChannel( BaseItemObject* Sender , GroupObjectTypeEnum Type , char* ChannelName , char* Password)
{   
    switch ( Type )
    {
	case EM_GroupObjectType_SysZoneChannel:
    case EM_GroupObjectType_SysChannel:
		{/*
			GroupObjectClass*   GroupObj;
			GroupObj = new GroupObjectClass( Type );			
			if( GroupObj->AddUser( Sender ) == false )
			{
				GroupObj->Destroy();
				CreateChannelFalse( Sender->ZoneID() , Type , Sender->DBID() , ChannelName );				
			} 
			else
			{
				CreateChannelOK( Sender->ZoneID() , Type , GroupObj->ID() , Sender->DBID() , ChannelName );
				AddUserOK( Sender->ZoneID() , Type , GroupObj->ID() , Sender->DBID() , ChannelName );
			}*/
		}
        break;
	case EM_GroupObjectType_PlayerChannel:
		{
			GroupPlayerChannel*   GroupObj;

			GroupObj = new GroupPlayerChannel( );
			if( GroupObj->ChannelName( ChannelName ) == false || GroupObj->AddUser( Sender ) == false )
			{
				GroupObj->Destroy();
				CreateChannelFalse( Sender->ZoneID() , Type , Sender->DBID() , ChannelName );
			}
			else
			{
				GroupObj->Password( Password );
				CreateChannelOK( Sender->ZoneID() , Type , GroupObj->ID() , Sender->DBID() , ChannelName );
				AddUserOK( Sender->ZoneID() , Type , GroupObj->ID() , Sender->DBID() , ChannelName , Sender->DBID() , Sender->RoleName() );
			}
		}
		break;
    case EM_GroupObjectType_Party:
        break;
    case EM_GroupObjectType_Guild:
        break;
    }
}
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::AddUser( BaseItemObject* User , GroupObjectTypeEnum Type , int ChannelID , char* ChannelName , char* Password , bool IsForceJoin  )
{
	switch( Type )
	{
	case EM_GroupObjectType_SysChannel:
	case EM_GroupObjectType_SysZoneChannel:
	case EM_GroupObjectType_Party:
	case EM_GroupObjectType_Guild:
		{
//			GroupObjectClass* GroupObj;
			AddUserFalse( User->ZoneID() , Type , ChannelID , User->DBID() , ChannelName );
		}
		break;

	case EM_GroupObjectType_PlayerChannel:
		{
			GroupPlayerChannel * PlayerChannel = GroupPlayerChannel::GetObj_ByName( ChannelName );
			if( PlayerChannel == NULL  && IsForceJoin == true )
			{
				NetSrv_ChannelBaseChild::CreateChannel( User , Type , ChannelName , Password );
				return;
			}		

			if(		PlayerChannel == NULL 
				||  strcmp( PlayerChannel->Password() , Password ) != 0 
				||	PlayerChannel->AddUser( User ) == false )
			{
				AddUserFalse( User->ZoneID() , Type , ChannelID , User->DBID() , ChannelName );
				return;
			}

			AddUserOK( User->ZoneID() , Type , PlayerChannel->ID() , User->DBID() , ChannelName , PlayerChannel->OwnerDBID() , (char*)PlayerChannel->OwnerName() );		

			set< int >& UserList_SockID  = (*PlayerChannel->UserList_SockID());
			set< int >::iterator Iter;
			for( Iter = UserList_SockID.begin() ; Iter != UserList_SockID.end() ; Iter++ )
			{
				if( User->SockID() != *Iter )
					SC_AddUserNotify(*Iter , Type , PlayerChannel->ID() , User->RoleName() );
			}
			

		}
		break;
	}
        
    
}
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::DelUser( BaseItemObject* User , GroupObjectTypeEnum Type , int ChannelID )
{
    GroupObjectClass* GroupObj = GroupObjectClass::GetObj( Type , ChannelID );
	if( GroupObj == NULL || GroupObj->DelUser( User ) == false )
	{
		DelUserFalse( User->ZoneID() , Type , ChannelID , User->DBID());
		return ;
	}

    DelUserOK( User->ZoneID() , Type , ChannelID , User->DBID() );

	set< int >& UserList_SockID  = (*GroupObj->UserList_SockID());
	set< int >::iterator Iter;
	for( Iter = UserList_SockID.begin() ; Iter != UserList_SockID.end() ; Iter++ )
	{
		if( User->SockID() != *Iter )
			SC_DelUserNotify(*Iter , Type , ChannelID , User->RoleName() );
	}

	switch( Type )
	{
	case EM_GroupObjectType_SysChannel    :	//系統
		break;
	case EM_GroupObjectType_SysZoneChannel:	//每個區域都有的
		break;
	case EM_GroupObjectType_PlayerChannel :	//玩家的
		{
			GroupPlayerChannel* ChannelObj = (GroupPlayerChannel*)GroupObj;
			if( GroupObj->UserList_SockID()->size() == 0 )
			{
				GroupObj->Destroy();
			}
			/*
			else if( User->DBID() == ChannelObj->OwnerDBID() )
			{
				BaseItemObject* NewLeader = BaseItemObject::GetObjBySockID( *(GroupObj->UserList_SockID()->begin()) );
				if( NewLeader != NULL )
				{
					SetManagerBase		( NewLeader , ChannelObj );
				}
			}
			*/
		}
		break;
	case EM_GroupObjectType_Party         :	//隊伍
		break;
	case EM_GroupObjectType_Guild         :	//工會
		break;

	}

}
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::ChangeRoleInfo( char* Account , OnlinePlayerInfoStruct& Info )
{
	_strlwr( Account );	
    BaseItemObject* Obj = BaseItemObject::GetObjByAccount( Account );
    if( Obj == NULL )
    {
        //Print( LV2 , "ChangeRoleInfo" ,"Obj == NULL ????註冊有問題!!" );
        return;
    }
	if( Obj->IsDestroy() )
	{
		Print( LV2 , "ChangeRoleInfo" ,"Obj->IsDestroy()  斷線後收到修正角色封包" );
		return;
	}

	bool IsFristTime = false;
	if( Obj->DBID() == -1 )
		IsFristTime = true;

    Obj->LocalID( Info.GUID );
    Obj->RoleName( (char*)Info.Name.Begin() );
    Obj->DBID( Info.DBID );
    Obj->ZoneID( Info.ZoneID );


	ChatRoleData* Role = Obj->Role();

	Role->Voc		= Info.Voc;
	Role->Voc_Sub	= Info.Voc_Sub;
	Role->Sex		= Info.Sex;
	Role->Note		= Info.Note;
	Role->Look		= Info.Look;
	Role->LV		= Info.LV;
	Role->LV_Sub	= Info.LV_Sub;

	Role->GuildID	= Info.GuildID;		//公會 ID
	Role->TitleID	= Info.TitleID;
	Role->State		= Info.State;		//玩家全區狀態
	Role->ParallelID = Info.ParallelID;

	SL_AddorSetOnlinePlayer  ( Info );
	//////////////////////////////////////////////////////////////////////////
	
	//清除自己多餘的資料
	Info.Account = "";
	Info.ProxyID = 0;
	Info.SockID  = 0;

	/*
	for( int i = 0 ; i < _MAX_FRIENDLIST_COUNT_ ; i++ )
	{
		if( Role->FriendList[i].Size() == 0 )
			continue;

		BaseItemObject* FriendObj = BaseItemObject::GetObjByRoleName( (char*)Role->FriendList[i].Begin() );
		if( FriendObj == NULL )
			continue;

		//上限互相通知
		NetSrv_FriendList::SC_ModifyInfo( FriendObj->Role()->SockID  , Info );

	}
*/
	//////////////////////////////////////////////////////////////////////////
	GroupObjectClass::ChangeRoleInfo( Obj );

}
//-------------------------------------------------------------------
void NetSrv_ChannelBaseChild::ChannelPlayerList	( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID )
{
	GroupObjectClass* GroupObj = GroupObjectClass::GetObj( Type , ChannelID );

	if( GroupObj == NULL )
	{
		//要求
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() ,  "頻道不存在" );
		return;
	}

	if( GroupObj->CheckInGroup( Sender ) == false )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() , "不在此頻道" );
		return;
	};

	vector< string > NameList;

	set< int >* UserList = GroupObj->UserList_SockID();
	set< int >::iterator Iter;

	for( Iter = UserList->begin() ; Iter != UserList->end() ; Iter++ )
	{	
		BaseItemObject* Obj = BaseItemObject::GetObjBySockID( *Iter );
		if( Obj == NULL )
			continue;
		NameList.push_back( Obj->RoleName() );
	}


	if( Type == EM_GroupObjectType_PlayerChannel )
	{
		GroupPlayerChannel* PlayerChannel = (GroupPlayerChannel*)GroupObj;
		const char* LeaderName = PlayerChannel->OwnerName();
		for( unsigned i = 1 ; i < NameList.size() ; i++ )
		{
			if( stricmp( LeaderName , NameList[i].c_str() ) == 0 )
			{
				swap( NameList[0] , NameList[i] );
				break;
			}
		}

	}

	ChannelPlayerListResult( Sender->SockID() , Type , ChannelID , NameList );

}
void NetSrv_ChannelBaseChild::JoinPlayerDisabled	( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID , bool JoinEnabled )
{
	if( Type != EM_GroupObjectType_PlayerChannel )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() ,  "不是玩家頻道" );
		return;
	}

	GroupPlayerChannel* GroupObj = (GroupPlayerChannel*)GroupObjectClass::GetObj( Type , ChannelID );

	if( GroupObj == NULL )
	{
		//要求
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() ,  "頻道不存在" );
		return;
	}

	if( GroupObj->JoinEnabled() == JoinEnabled )
	{
//		NetSrv_Talk::SysMsg( Sender->SockID() ,  "OK" );
		return;
	}

	GroupObj->JoinEnabled( JoinEnabled );
	
	//通知所有頻道內的玩家
	set< int >::iterator Iter;

	for( Iter = GroupObj->UserList_SockID()->begin() ; Iter != GroupObj->UserList_SockID()->end() ; Iter++ )
	{	
		int SockID = *Iter;
		JoinPlayerDisabledResult( SockID , ChannelID , Type , JoinEnabled );
	
	}

}
void NetSrv_ChannelBaseChild::KickPlayer			( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID , char* Name )
{
	char Buf[256];
	if( Type != EM_GroupObjectType_PlayerChannel )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() ,  "不是玩家頻道" );
		return;
	}

	GroupPlayerChannel* GroupObj = (GroupPlayerChannel*)GroupObjectClass::GetObj( Type , ChannelID );

	if( GroupObj == NULL )
	{
		//要求
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() ,  "頻道不存在" );
		return;
	}

	if(  GroupObj->OwnerDBID() != Sender->DBID() )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() ,  "非頻道擁有者" );	
		return;
	}
	
	BaseItemObject* KickPlayer = BaseItemObject::GetObjByRoleName( Name );
	if( KickPlayer == NULL )
	{
		sprintf( Buf , "找不到玩家 %s" , Name );
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() ,  Buf );
		return;
	}

	if( GroupObj->DelUser( KickPlayer ) == true )
	{
		sprintf( Buf ,"踢除玩家 %s" , Name );
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() ,  Buf );
		NetSrv_Talk::SysMsg_ChartoUtf8( KickPlayer->SockID() ,  "被踢出頻道" );

		set< int >& UserList_SockID  = (*GroupObj->UserList_SockID());
		set< int >::iterator Iter;
		for( Iter = UserList_SockID.begin() ; Iter != UserList_SockID.end() ; Iter++ )
		{
			SC_DelUserNotify(*Iter , Type , ChannelID , KickPlayer->RoleName() );
		}
		SC_DelUserNotify( KickPlayer->SockID()  , Type , ChannelID , KickPlayer->RoleName( ) );

	}
	else
	{
		sprintf( Buf , "玩家 %s 不在頻道" , Name );
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() ,  Buf );
	}
}
void NetSrv_ChannelBaseChild::SetManagerBase		( BaseItemObject* NewOwner , GroupPlayerChannel* GroupObj )
{
	GroupObj->Owner( NewOwner );	

	set< int >* UserList = GroupObj->UserList_SockID();
	set< int >::iterator Iter;

	for( Iter = UserList->begin() ; Iter != UserList->end() ; Iter ++ )
	{
		int SockID = *Iter ;

		SC_ChannelOwner( SockID , EM_GroupObjectType_PlayerChannel , GroupObj->ID() , NewOwner->RoleName() , NewOwner->DBID() );
	}
}
void NetSrv_ChannelBaseChild::SetManager			( BaseItemObject* Sender , GroupObjectTypeEnum Type , int ChannelID , char* Name )
{
	char Buf[256];
	if( Type != EM_GroupObjectType_PlayerChannel )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() ,  "不是玩家頻道" );
		SC_SetManagerResult( Sender->SockID()  , Type , ChannelID , false );
		return;
	}

	GroupPlayerChannel* GroupObj = (GroupPlayerChannel*)GroupObjectClass::GetObj( Type , ChannelID );

	if( GroupObj == NULL )
	{
		//要求
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() ,  "頻道不存在" );
		SC_SetManagerResult( Sender->SockID()  , Type , ChannelID , false );
		return;
	}

	if(  GroupObj->OwnerDBID() != Sender->DBID() )
	{
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() ,  "非頻道擁有者" );	
		SC_SetManagerResult( Sender->SockID()  , Type , ChannelID , false );
		return;
	}

	BaseItemObject* NewOwner = BaseItemObject::GetObjByRoleName( Name );
	if( NewOwner == NULL )
	{
		sprintf( Buf , "找不到玩家 %s" , Name );
		NetSrv_Talk::SysMsg_ChartoUtf8( Sender->SockID() ,  Buf );
		SC_SetManagerResult( Sender->SockID()  , Type , ChannelID , false );
		return;
	}

	GroupObj->Owner( NewOwner );	
	SC_SetManagerResult( Sender->SockID()  , Type , ChannelID , true );

	set< int >* UserList = GroupObj->UserList_SockID();
	set< int >::iterator Iter;

	for( Iter = UserList->begin() ; Iter != UserList->end() ; Iter ++ )
	{
		int SockID = *Iter ;

		SC_ChannelOwner( SockID , Type , ChannelID , NewOwner->RoleName() , NewOwner->DBID() );
	}

}

void NetSrv_ChannelBaseChild::ZoneChannelID		( BaseItemObject* Sender , int ZoneChannelID )
{
	Sender->MapZoneID( ZoneChannelID );
}

void NetSrv_ChannelBaseChild::OnSrvConnect( int SrvSockID )
{

	vector< BaseItemObject* >& ObjList = *BaseItemObject::GetAllPlayerObj_SockID( );

	map< int , OnlinePlayerInfoStruct* >::iterator Iter;
	map< int , OnlinePlayerInfoStruct* >* DBIDMap = AllOnlinePlayerInfoClass::This()->DBIDMap();

	for( Iter = DBIDMap->begin() ; Iter != DBIDMap->end() ; Iter++ )	
	{
		//if( Iter->second->IsDestroy() == false )
		SL_AddorSetOnlinePlayer  ( SrvSockID , *( Iter->second ) );
	}

}

void NetSrv_ChannelBaseChild::OnLocalSrvConnect( int LocalID )
{
	SSrv_SystemTime( LocalID , TimeStr::Now_Value() + _DSysTime );
}

void NetSrv_ChannelBaseChild::SetChannelOwner	( GroupObjectTypeEnum Type , int ChannelID , int PlayerDBID )
{
	GroupObjectClass* GroupObj = GroupObjectClass::GetObj( Type , ChannelID );
	if( GroupObj == NULL  )
		return;
	GroupObj->ChangeOwner( PlayerDBID );
}