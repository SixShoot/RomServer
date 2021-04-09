#include "M_BaseItemObject.h"
#include "../GroupObject/GroupObject.h"
#include "../GroupZoneChannel/GroupZoneChannel.h"
#include "../Global.h"
#include "../../NetWalker_Member/NetWakerChatInc.h"
//----------------------------------------------------------------------------------------
//		靜態變數
//----------------------------------------------------------------------------------------
vector< BaseItemObject* >	    BaseItemObject::_AllPlayerObj_SockID; 
map< int , BaseItemObject* >    BaseItemObject::_AllPlayerObj_DBID;
map< string , BaseItemObject* > BaseItemObject::_AllPlayerObj_Account;
map< string , BaseItemObject* > BaseItemObject::_AllPlayerObj_RoleName;

//----------------------------------------------------------------------------------------
//		靜態
//----------------------------------------------------------------------------------------
void    BaseItemObject::Init( )
{
    //設定定時執行OnTimeProc
    _OnTimeProc_1Sec( NULL , NULL );
}
void    BaseItemObject::Release( )
{

}
//----------------------------------------------------------------------------------------
int		BaseItemObject::_OnTimeProc_1Sec( SchedularInfo* Obj , void* InputClass )
{
    OnTimeProc( );
    Global::Schedular()->Push( _OnTimeProc_1Sec , _Def_OnTimeProc_Time_ , NULL , NULL );
    return 0;
}
//----------------------------------------------------------------------------------------
//取得某個ID的物件資料
BaseItemObject* BaseItemObject::GetObjBySockID( int ID )
{

    if( (unsigned)ID >= _AllPlayerObj_SockID.size() )
        return NULL;

    return _AllPlayerObj_SockID[ ID ];

}

BaseItemObject* BaseItemObject::GetObjByDBID( int DBID )
{
    map< int , BaseItemObject* >::iterator Iter;
    Iter = _AllPlayerObj_DBID.find( DBID );
    if( Iter == _AllPlayerObj_DBID.end() )
        return NULL;

    return Iter->second;
}

BaseItemObject* BaseItemObject::GetObjByRoleName( const  char* Name )
{
    map< string , BaseItemObject* >::iterator Iter;
    Iter = _AllPlayerObj_RoleName.find( Name );
    if( Iter == _AllPlayerObj_RoleName.end() )
        return NULL;

    return Iter->second;
}

BaseItemObject* BaseItemObject::GetObjByAccount( const char* Account )
{
    map< string , BaseItemObject* >::iterator Iter;
    Iter = _AllPlayerObj_Account.find( Account );
    if( Iter == _AllPlayerObj_Account.end() )
        return NULL;

    return Iter->second;
}

vector< BaseItemObject* >*	   BaseItemObject::GetAllPlayerObj_SockID( )
{
	return &_AllPlayerObj_SockID;
}
//----------------------------------------------------------------------------------------
//主要看有沒有物件要刪除(定時要處理) 每秒
void	BaseItemObject::OnTimeProc( )
{
    int     i;
    //----------------------------------------------------------------------------------------
    BaseItemObject*     Obj;
    //處理本區域所有的物件

    for( i = 0 ; i < GetMaxObjID() ; i++ )
    {
        Obj = _AllPlayerObj_SockID[ i ];
        if( Obj == NULL )
            continue;

        if( Obj->_IsDestroy == false  )
            continue;

        delete Obj;
    }
}
//----------------------------------------------------------------------------------------
int     BaseItemObject::GetMaxObjID( )
{
    return (int)_AllPlayerObj_SockID.size();
}
//----------------------------------------------------------------------------------------
//		動態
//----------------------------------------------------------------------------------------
BaseItemObject::BaseItemObject(  int SockID  , int ProxyID , char* Account )
{
    pair< map< string , BaseItemObject* >::iterator , bool > InsertResult;

	_strlwr( Account );	

    _Role.SockID     = SockID;
	_Role.ProxyID	 = ProxyID;
    _Role.Account    = Account;
    _Role.DBID       = -1;    
    _Role.LocalID    = -1;
	_Role.PartyID	= -1;

	_IsDestroy  = false;

    while((int) _AllPlayerObj_SockID.size() <= SockID )
        _AllPlayerObj_SockID.push_back( NULL );

    _AllPlayerObj_SockID[ SockID ] = this;
    //_AllPlayerObj_Account[ Account ] = this;
    InsertResult = _AllPlayerObj_Account.insert( map< string , BaseItemObject* >::value_type( Account , this ) );
    if( InsertResult.second == false )
    {
        _Role.Account = "";
        _IsDestroy = true;
		Print( LV2 , "BaseItemObject" , "此帳號尚未清除???? Acount=%s" , Account );
        return;
    }

}
//----------------------------------------------------------------------------------------
BaseItemObject::~BaseItemObject( )
{

	NetSrv_FriendListChild::PlayerOffline( this->DBID() );
	GroupObjectClass::ReleaseAll( this );

    if( (unsigned)_Role.SockID >= _AllPlayerObj_SockID.size() )
    {
        Print(LV2, "BaseItemObject", "~BaseItemObject  有問題 _SockID >= _AllPlayerObj_SockID.size() ????????????" );
        return ;
    }
    _AllPlayerObj_SockID[ _Role.SockID ] = NULL;

    if( _Role.DBID != -1 )
    {
        _AllPlayerObj_DBID.erase( _Role.DBID );
    }
    
    if( _Role.RoleName.Size() != 0 )
    {
        _AllPlayerObj_RoleName.erase( _Role.RoleName.Begin() );
    }

    _AllPlayerObj_Account.erase( _Role.Account.Begin() );
}
//----------------------------------------------------------------------------------------
void		BaseItemObject::Destroy( ) 
{ 
    _IsDestroy = true;
}
//----------------------------------------------------------------------------------------
bool        BaseItemObject::DBID( int DBID )
{
    if( _Role.DBID != -1 )
        return false;
    pair< map< int , BaseItemObject* >::iterator , bool > InsertResult;
    InsertResult = _AllPlayerObj_DBID.insert( map< int , BaseItemObject* >::value_type( DBID , this ) );
    if( InsertResult.second == false )
        return false;

    _Role.DBID = DBID;
    return true;
}
//----------------------------------------------------------------------------------------
bool    BaseItemObject::RoleName( char* Name )
{
    if( _Role.RoleName.Size() != 0 )
        return false;

    pair< map< string , BaseItemObject* >::iterator , bool > InsertResult;
    InsertResult = _AllPlayerObj_RoleName.insert( map< string , BaseItemObject* >::value_type( Name , this ) );
    if( InsertResult.second == false )
        return false;

    _Role.RoleName = Name;
    return true;

}
//----------------------------------------------------------------------------------------
void    BaseItemObject::ZoneID( int ID )
{ 
	if( ID == _Role.ZoneID )
		return;

	if( _Role.ZoneID == -1 )
	{	
		OnlinePlayerInfoStruct Info;
		_Role.GetOnlinePlayerInfo( Info );
//		NetSrv_FriendListChild::SC_ModifyInfo( _Role.SockID , Info  );

	}

	_Role.ZoneID = ID; 
}
//----------------------------------------------------------------------------------------
void		BaseItemObject::MapZoneID( int ID )
{
	GroupZoneChannel* NewZoneChannel = (GroupZoneChannel*)GroupObjectClass::GetObj( EM_GroupObjectType_SysZoneChannel , ID );

	if( NewZoneChannel == NULL )
		return;

	GroupZoneChannel* ZoneChannel = (GroupZoneChannel*)GroupObjectClass::GetObj( EM_GroupObjectType_SysZoneChannel , _Role.MapZoneID );
	if( ZoneChannel != NULL )
	{
		if( ZoneChannel->DelUser( this ) == false )
			Print( LV2 , "MapZoneID" , "DelUser( this ) == false " );
	}

	NewZoneChannel->AddUser( this );
	_Role.MapZoneID = ID;
	/*
	if( NewZoneChannel->AddUser( this ) == false )
		_Role.MapZoneID = -1;

	else
		_Role.MapZoneID = ID;
	*/
}
//----------------------------------------------------------------------------------------