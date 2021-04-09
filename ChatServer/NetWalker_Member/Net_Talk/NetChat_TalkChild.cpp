#include "../NetWakerChatInc.h"
#include "NetChat_TalkChild.h"
//-----------------------------------------------------------------
map< int , CliBoardInfoStruct >	 NetSrv_TalkChild::_CliBoardInfoMap;
//-----------------------------------------------------------------
bool    NetSrv_TalkChild::Init()
{
    NetSrv_Talk::_Init();

    if( This != NULL)
        return false;

    This = new NetSrv_TalkChild;

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_TalkChild::Release()
{
    if( This == NULL )
        return false;

    delete This;
    This = NULL;

    NetSrv_Talk::_Release();
    return true;
    
}
//-----------------------------------------------------------------
void NetSrv_TalkChild::R_SysChannel( int PlayerDBID , GroupObjectTypeEnum Type , int ChannelID , char* Name  , char* Context , TalkSenderInfo& SenderInfo )
{
	BaseItemObject* Sender = BaseItemObject::GetObjByDBID( PlayerDBID );

	if( Sender == NULL )
		return;

	Voc_ENUM Job = Sender->Role()->Voc;

	//暫時不攔 Cli的封包資料
	switch( Type )
	{
	case EM_GroupObjectType_SysZoneChannel: 
		{
			GroupObjectClass* Group = GroupObjectClass::GetObj( Type , Sender->MapZoneID() );
			if( Group == NULL )
				return;

			Send_ZoneChannel( Job , Group , ChannelID , EM_TalkChannelType_Normal , Name  , Context , SenderInfo );

		}
		break;
	case EM_GroupObjectType_SysChannel: 
		{
			Send_SysChannel( Job , ChannelID , EM_TalkChannelType_Normal , Name ,Context , SenderInfo );
			break;
		}
	case EM_GroupObjectType_PlayerChannel:
	case EM_GroupObjectType_Party:
	case EM_GroupObjectType_Guild:
		{
			if( EM_GroupObjectType_Party == Type )
				ChannelID = Sender->PartyID();

			GroupObjectClass* Group = GroupObjectClass::GetObj( Type , ChannelID );
			if( Group == NULL )
				return;

			if( Group->CheckInGroup( Sender ) == false )
			{
				Print( LV1 , "R_Channel" , "角色不屬於此頻道 Account=%s" , Sender->Account() );
				return;
			}
			Send_Channel( Job , Type , ChannelID , EM_TalkChannelType_Normal , Name  , Context , SenderInfo );

			if( Type == EM_GroupObjectType_PlayerChannel )
				Global::Log_Talk( EM_TalkLogType_PlayerChannel , ChannelID , PlayerDBID , 0 , Context );
			else if( Type == EM_GroupObjectType_Party )
				Global::Log_Talk( EM_TalkLogType_Party , ChannelID , PlayerDBID , 0 , Context );
			else if( Type == EM_GroupObjectType_Guild )
				Global::Log_Talk( EM_TalkLogType_Guild , ChannelID , PlayerDBID , 0 , Context );
		}
		break;

	}

}
//-----------------------------------------------------------------
void NetSrv_TalkChild::R_Channel( int SockID , GroupObjectTypeEnum Type , int ChannelID , char* Name  , char* Context , TalkSenderInfo& SenderInfo )
{

	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( SockID );

	if( Sender == NULL )
		return;

	Voc_ENUM Job = Sender->Role()->Voc;
	//暫時不攔 Cli的封包資料
	switch( Type )
	{
		/*
	case EM_GroupObjectType_SysZoneChannel: 
		{
			GroupObjectClass* Group = GroupObjectClass::GetObj( Type , Sender->MapZoneID() );
			if( Group == NULL )
				return;

			Send_ZoneChannel( Group , ChannelID , EM_TalkChannelType_Normal , Name  , Context );

		}
		break;
	case EM_GroupObjectType_SysChannel: 
	*/
	case EM_GroupObjectType_PlayerChannel:
	case EM_GroupObjectType_Party:
	case EM_GroupObjectType_Guild:
		{
			if( EM_GroupObjectType_Party == Type )
				ChannelID = Sender->PartyID();

			GroupObjectClass* Group = GroupObjectClass::GetObj( Type , ChannelID );
			if( Group == NULL )
				return;

			if( Group->CheckInGroup( Sender ) == false )
			{
				Print( LV1 , "R_Channel" , "角色不屬於此頻道 SockID=%d Account=%s" , SockID , Sender->Account() );
				return;
			}
			Send_Channel( Job , Type , ChannelID , EM_TalkChannelType_Normal , Name  , Context , SenderInfo );

			if( Type == EM_GroupObjectType_PlayerChannel )
				Global::Log_Talk( EM_TalkLogType_PlayerChannel , ChannelID , Sender->DBID() , 0 , Context );
			else if( Type == EM_GroupObjectType_Party )
				Global::Log_Talk( EM_TalkLogType_Party , ChannelID , Sender->DBID() , 0 , Context );
			else if( Type == EM_GroupObjectType_Guild )
				Global::Log_Talk( EM_TalkLogType_Guild , ChannelID , Sender->DBID() , 0 , Context );
		}
		break;

	}

}
//-----------------------------------------------------------------
void NetSrv_TalkChild::R_RegisterBillBoardInfo	( int Sockid , int DataSize , char* Data )
{
	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );
	if( Sender == NULL )
		return;
	if( DataSize > 1024 )
		return;

	CliBoardInfoStruct boardInfo;
	boardInfo.DataSize = DataSize;
	memcpy( boardInfo.Data , Data , DataSize );
	boardInfo.LiveTime = 60 * 60 * 24;
	boardInfo.PlayerDBID = Sender->DBID();

	//通知所有玩家
	SC_All_BillBoardInfo( Sender->DBID() , DataSize , Data );
	//_CliBoardInfoMap
	_CliBoardInfoMap[ Sender->DBID() ] = boardInfo;
}
void NetSrv_TalkChild::R_BillBoardLiveTime		( int Sockid , int LiveTime )
{
	BaseItemObject* Sender = BaseItemObject::GetObjBySockID( Sockid );
	if( Sender == NULL )
		return;

	map< int , CliBoardInfoStruct >::iterator Iter;
	Iter = _CliBoardInfoMap.find( Sender->DBID() );
	if( Iter == _CliBoardInfoMap.end() )
		return;

	Iter->second.LiveTime = LiveTime;

	if( LiveTime == 0 )
	{
		_CliBoardInfoMap.erase( Iter );
		SC_All_BillBoardDelete( Sender->DBID() );
	}
}

void NetSrv_TalkChild::OnTimeProcCliBoardInfo( )
{
	vector< int > DelList;
	map< int , CliBoardInfoStruct >::iterator Iter;	

	for( Iter = _CliBoardInfoMap.begin() ; Iter != _CliBoardInfoMap.end() ; Iter++ )
	{
		Iter->second.LiveTime -= 60;
		if( Iter->second.LiveTime < 0 )
		{
			DelList.push_back( Iter->second.PlayerDBID );
		}
	}

	for( unsigned i =0 ; i < DelList.size() ; i++ )
	{
		_CliBoardInfoMap.erase( DelList[i] );
		SC_All_BillBoardDelete( DelList[i] );
	}
}

void NetSrv_TalkChild::OnCliConnect				( int CliID , char* Account )
{
	//通知線上的資訊
	map< int , CliBoardInfoStruct >::iterator Iter;	

	for( Iter = _CliBoardInfoMap.begin() ; Iter != _CliBoardInfoMap.end() ; Iter++ )
	{
		SC_BillBoardInfo( CliID , Iter->second.PlayerDBID , Iter->second.DataSize , Iter->second.Data );
	}

}
void NetSrv_TalkChild::OnCliDisconnect			( int CliID )
{
	R_BillBoardLiveTime( CliID , 0 );
}