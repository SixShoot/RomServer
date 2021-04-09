#include "../Global.h"
#include "GroupPlayerChannel.h"


map< string , GroupPlayerChannel* >		GroupPlayerChannel::_NameList;				//頻道名稱 


GroupPlayerChannel::GroupPlayerChannel():GroupObjectClass( EM_GroupObjectType_PlayerChannel )
{
	_JoinEnabled = true;
	_OwnerSockID = -1;
}

GroupPlayerChannel::~GroupPlayerChannel()
{
	ChannelName( "" );
}
//----------------------------------------------------------------------------------------
const char*	GroupPlayerChannel::Password( )
{
	return (char*)_Password.c_str();
}
void		GroupPlayerChannel::Password( string Pwd )
{
	_Password = Pwd;
}
//----------------------------------------------------------------------------------------
bool		GroupPlayerChannel::JoinEnabled( )
{ 
	return _JoinEnabled; 
}
void 		GroupPlayerChannel::JoinEnabled( bool Value )
{ 
	_JoinEnabled = Value; 
}
//----------------------------------------------------------------------------------------
const char*	GroupPlayerChannel::ChannelName( )
{
	return (char*)_ChannelName.c_str();
}
//----------------------------------------------------------------------------------------
bool		GroupPlayerChannel::ChannelName( string Name )	
{
	map< string , GroupPlayerChannel* >::iterator  Iter;

	if( Name.size() != 0 )
	{
		Iter = _NameList.find( Name );
		if( Iter != _NameList.end() )
		{
			return false;
		}		

		_NameList[ Name ] = this;
	}

	if( _ChannelName.size() != 0 )
	{
		Iter = _NameList.find( _ChannelName );
		if( Iter != _NameList.end() )
		{
			_NameList.erase( Iter );
		}
	}
	_ChannelName = Name;

	return true;
}
//----------------------------------------------------------------------------------------
GroupPlayerChannel* GroupPlayerChannel::GetObj_ByName( string ChannelName )
{
	map< string , GroupPlayerChannel* >::iterator  Iter;

	Iter = _NameList.find( ChannelName );
	if( Iter == _NameList.end() )
		return NULL;
	return Iter->second;
}
//----------------------------------------------------------------------------------------
bool    GroupPlayerChannel::ChangeOwner( int PlayerDBID )
{
	BaseItemObject* Obj = BaseItemObject::GetObjByDBID( *(_UserList_SockID.begin()) );
	if( Obj == NULL )
		return false;
	NetSrv_ChannelBaseChild::SetManagerBase( Obj , this );
	return true;
}
bool    GroupPlayerChannel::AddUser( BaseItemObject* Player )
{
	if( _JoinEnabled == false )
		return false;

	if( GroupObjectClass::AddUser( Player ) == false )
		return false;

	if( _OwnerSockID == -1 )
		Owner( Player );
	return true;
}
//----------------------------------------------------------------------------------------
bool    GroupPlayerChannel::DelUser( BaseItemObject* Player )
{
	if( GroupObjectClass::DelUser( Player ) == false )
		return false;


	if( Player->DBID() == OwnerDBID() )
	{
		if( _UserList_SockID.size() != 0 )
		{
			BaseItemObject* Obj = BaseItemObject::GetObjBySockID( *(_UserList_SockID.begin()) );
			if( Obj != NULL )
				NetSrv_ChannelBaseChild::SetManagerBase( Obj , this );
		}
	}
/*
	if( Player->SockID() == _OwnerSockID )
	{
		if( _UserList_SockID.size() != 0 )
		{
			BaseItemObject* Obj = BaseItemObject::GetObjBySockID( *(_UserList_SockID.begin()) );
			if( Obj != NULL )
				Owner( Obj );
		}
		else
			_OwnerSockID = -1;
	}
	*/
	return true;
}
//----------------------------------------------------------------------------------------
/*
BaseItemObject*	GroupPlayerChannel::Owner( )
{
	BaseItemObject* Owner = BaseItemObject::GetObjBySockID( _OwnerSockID );
	return Owner;
}
*/
void	GroupPlayerChannel::Owner( BaseItemObject* Player )
{
	if( _OwnerSockID == Player->SockID() )
		return;

	_OwnerSockID = Player->SockID();
	_OwnerName = Player->RoleName();
	_OwnerDBID = Player->DBID();
	/*
	//通知所有頻道內的玩家
	char Buf[256];
	sprintf( Buf , "%s為頻道主人" , Player->RoleName() );
	NetSrv_TalkChild::Send_Channel( EM_GroupObjectType_PlayerChannel , _ID , EM_TalkChannelType_Sys , "" , Buf );
	*/
}
