#include "../NetWakerGSrvInc.h"
#include "NetSrv_ChannelBaseChild.h"
#include "../../mainproc/PartyInfoList/PartyInfoList.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_ChannelBaseChild::Init()
{
    NetSrv_ChannelBase::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_ChannelBaseChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_ChannelBaseChild::Release()
{
    if( This == NULL )
        return false;

	NetSrv_ChannelBase::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------

int OnChatConnectSchedular( SchedularInfo* SchedularObj , void* InputClass)
{
	//把所有的角色重新註冊
	RoleDataEx*     Owner;

	set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//計算每個房間的人數
	for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if( Obj == NULL )
			continue;

		Owner = Obj->Role();

		OnlinePlayerInfoStruct Info;
		Owner->GetOnlinePlayerInfo( Info );
		NetSrv_ChannelBase::ChangeRoleInfo  ( Owner->Account_ID() , Info );
	}

	return 0;
}
//-----------------------------------------------------------------
void NetSrv_ChannelBaseChild::OnChatConnect( EM_SERVER_TYPE ServerType , DWORD ZoneID  )
{
	//把所有的角色重新註冊
	Global::Schedular()->Push( OnChatConnectSchedular , 2000 , NULL , NULL );
}
//-----------------------------------------------------------------
void NetSrv_ChannelBaseChild::OnChatDisconnect    ( EM_SERVER_TYPE ServerType , DWORD ZoneID  )
{
	PartyInfoListClass::This()->Clear();

	RoleDataEx*     Owner;

	set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//計算每個房間的人數
	for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if( Obj == NULL )
			continue;
		Owner = Obj->Role();
		Owner->BaseData.PartyID = -1;		
	}

	//清除線上玩家資料
	AllOnlinePlayerInfoClass::This()->Clear();
}
//-----------------------------------------------------------------
void NetSrv_ChannelBaseChild::CreateChannelOK     ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* ChannelName )
{
    Print( Def_PrintLV1 , "CreateChannelOK" , "Type=%d ChannelID=%d UserDBID=%d Name=%s" , Type , ChannelID , UserDBID , ChannelName );
	BaseItemObject* Obj = BaseItemObject::GetObjByDBID( UserDBID );
	
	switch( Type )
	{
	case EM_GroupObjectType_SysChannel:
		break;
	case EM_GroupObjectType_PlayerChannel:
		if( Obj == NULL )
			break;

		S_CreateChannelOK( Obj->GUID() , Type , ChannelID , ChannelName );
		
		break;
	case EM_GroupObjectType_Party:
		break;
	case EM_GroupObjectType_Guild:
		break;	
	}
}
//-----------------------------------------------------------------
void NetSrv_ChannelBaseChild::CreateChannelFalse  ( GroupObjectTypeEnum Type , int UserDBID , char* ChannelName )
{
    Print( Def_PrintLV1 , "CreateChannelFalse" , "Type=%d  UserDBID=%d Name=%s" , Type , UserDBID , ChannelName);
	BaseItemObject* Obj = BaseItemObject::GetObjByDBID( UserDBID );
	switch( Type )
	{
	case EM_GroupObjectType_SysChannel:
		break;
	case EM_GroupObjectType_PlayerChannel:
		if( Obj == NULL )
			break;

		S_CreateChannelFalse( Obj->GUID() , Type , ChannelName );

		break;
	case EM_GroupObjectType_Party:
		break;
	case EM_GroupObjectType_Guild:
		break;	
	}
}
//-----------------------------------------------------------------
void NetSrv_ChannelBaseChild::AddUserOK           ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* ChannelName , int ChannelOwnerDBID , char* ChannelOwnerName )
{	
	//static void S_JoinChannelFalse( int GUID , GroupObjectTypeEnum Type , int ChannelID );
    Print( Def_PrintLV1 , "AddUserOK" , "Type=%d ChannelID=%d UserDBID=%d Name=%s" , Type , ChannelID , UserDBID , ChannelName );
	BaseItemObject* Obj = BaseItemObject::GetObjByDBID( UserDBID );

	switch( Type )
	{
	case EM_GroupObjectType_SysChannel:
		break;
	case EM_GroupObjectType_PlayerChannel:
		if( Obj == NULL )
			break;

		S_JoinChannelOK( Obj->GUID() , Type , ChannelID , ChannelName , ChannelOwnerDBID ,  ChannelOwnerName );

		break;
	case EM_GroupObjectType_Party:
		break;
	case EM_GroupObjectType_Guild:
		break;	
	}
}
//-----------------------------------------------------------------
void NetSrv_ChannelBaseChild::AddUserFalse        ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID , char* ChannelName )
{
    Print( Def_PrintLV1 , "AddUserFalse" , "Type=%d ChannelID=%d UserDBID=%d Name=%s" , Type , ChannelID , UserDBID , ChannelName );
	BaseItemObject* Obj = BaseItemObject::GetObjByDBID( UserDBID );
	if( Obj == NULL )
		return;


	switch( Type )
	{
	case EM_GroupObjectType_SysChannel:
		break;
	case EM_GroupObjectType_PlayerChannel:

		S_JoinChannelFalse( Obj->GUID() , Type , ChannelID , ChannelName );

		break;
	case EM_GroupObjectType_Party:
		break;
	case EM_GroupObjectType_Guild:
		break;	
	}
}
//-----------------------------------------------------------------
void NetSrv_ChannelBaseChild::DelUserOK           ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID  )
{
    Print( Def_PrintLV1 , "DelUserOK" , "Type=%d ChannelID=%d UserDBID=%d " , Type , ChannelID , UserDBID );
	BaseItemObject* Obj = BaseItemObject::GetObjByDBID( UserDBID );
	if( Obj == NULL )
		return;

	S_LeaveChannelOK( Obj->GUID() , Type , ChannelID );
}
//-----------------------------------------------------------------
void NetSrv_ChannelBaseChild::DelUserFalse        ( GroupObjectTypeEnum Type , int ChannelID , int UserDBID  )
{
    Print( Def_PrintLV1 , "DelUserFalse" , "Type=%d ChannelID=%d UserDBID=%d " , Type , ChannelID , UserDBID );
	BaseItemObject* Obj = BaseItemObject::GetObjByDBID( UserDBID );
	if( Obj == NULL )
		return;

	S_LeaveChannelFalse( Obj->GUID() , Type , ChannelID );
}
//-----------------------------------------------------------------
//-----------------------------------------------------------------
void NetSrv_ChannelBaseChild::R_CreateChannel( BaseItemObject* Player  , char* ChannelName , char* Password )
{
	CreateChannel( Player->Role()->DBID() , EM_GroupObjectType_PlayerChannel, ChannelName , Password );
}
void NetSrv_ChannelBaseChild::R_JoinChannel( BaseItemObject* Player  , char* ChannelName , char* Password , bool IsForceJoin )
{
//	AddUser( Player->Role()->DBID() , Player->Role()->DBID() , EM_GroupObjectType_PlayerChannel , ChannelID , Password );
	AddUser_PlayerChannel( Player->Role()->DBID() , ChannelName , Password , IsForceJoin );
}
void NetSrv_ChannelBaseChild::R_LeaveChannel( BaseItemObject* Player  , int ChannelID )
{
	DelUser( Player->Role()->DBID() , EM_GroupObjectType_PlayerChannel , ChannelID );
}

void NetSrv_ChannelBaseChild::OnSystemTimeNotify	 ( int Time )
{
	RoleDataEx::SysDTime = Time - TimeStr::Now_Value();
}
