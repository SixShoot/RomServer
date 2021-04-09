#include "../Global.h"
#include "GroupObject.h"
//#include "RecycleBin/recyclebin.h"
//#include "functionschedular/functionschedular.h"


#include <utility>

//----------------------------------------------------------------------------------------

vector< GroupObjectClass* >				GroupObjectClass::_GroupList[ EM_GroupObjectType_Count ];
vector< int >							GroupObjectClass::_UnusedIDList[ EM_GroupObjectType_Count ];
multimap< int , GroupObjectClass* >		GroupObjectClass::_UserGroupInfo;      //記錄使用者有加入的群組 

//----------------------------------------------------------------------------------------
int		GroupObjectClass::_OnTimeProc_Schedular( SchedularInfo* Obj , void* InputClass )
{
	ProcessAll( );
	Global::Schedular()->Push( _OnTimeProc_Schedular , _Def_OnTimeProc_Time_ , NULL , NULL );
	return 0;
}
//----------------------------------------------------------------------------------------
bool    GroupObjectClass::Init()
{
	_OnTimeProc_Schedular( NULL , NULL );

    return true;
}
bool     GroupObjectClass::Release( )
{
	return true;
}
//----------------------------------------------------------------------------------------
bool    GroupObjectClass::ProcessAll( )
{
	//檢查是否有頻道要刪除
	//_GroupList[ EM_GroupObjectType_Count ];
	for( int i = 0 ; i < EM_GroupObjectType_Count ; i++ )
	{
		for( int j = 0 ; j < (int)_GroupList[i].size() ; j++ )
		{
			GroupObjectClass* GObj = _GroupList[i][j];

			if( GObj == NULL )
				continue;
			if( GObj->_IsDestroy == false )
			{
				GObj->Process();
				continue;
			}

			_GroupList[i][j] = NULL;
			delete GObj;		
		}
	}

    return true;
}
//----------------------------------------------------------------------------------------
GroupObjectClass* GroupObjectClass::GetObj( GroupObjectTypeEnum Type , int ID )
{
    if( EM_GroupObjectType_Count <= Type )
        return NULL;

    if( _GroupList[ Type ].size() <= (unsigned)ID )
        return NULL;

    return _GroupList[ Type ][ ID ];
}
//----------------------------------------------------------------------------------------
int		GroupObjectClass::MaxCount( GroupObjectTypeEnum Type )
{
	if( EM_GroupObjectType_Count <= Type )
		return 0;

	return (int)_GroupList[ Type ].size();
}
//----------------------------------------------------------------------------------------
GroupObjectClass::GroupObjectClass( GroupObjectTypeEnum Type )
{
	_Type = Type;
    if( EM_GroupObjectType_Count <= _Type )
    {
        _IsDestroy = true;
        return;
    }

    _IsDestroy = false;
    
    if( _UnusedIDList[ _Type ].size() == 0 )
    {
        _UnusedIDList[ _Type ].push_back( int( _GroupList[ _Type ].size() ) );
        _GroupList[ _Type ].push_back( NULL );
    }

    _ID = _UnusedIDList[ _Type ].back();
    _UnusedIDList[ _Type ].pop_back();

    _GroupList[ _Type ][ _ID ] = this;

    OnCreate();

}
//----------------------------------------------------------------------------------------
GroupObjectClass::~GroupObjectClass( )
{
	if( EM_GroupObjectType_Count <= _Type )
	{
		return;
	}

    OnDestroy();

	if( _Type != EM_GroupObjectType_Party )
		_UnusedIDList[ _Type ].push_back( _ID );

    _GroupList[ _Type ][ _ID ] = NULL;

	/*
	//清除 所有加入此Channel 的個體資料
	set< int >::iterator Iter;
	for( Iter = _UserList_SockID.begin() ; Iter != _UserList_SockID.end() ; Iter++ )
	{	
		BaseItemObject* Obj = BaseItemObject::GetObjBySockID( *Iter );
		if( Obj == NULL )
			continue;

		multimap< int , GroupObjectClass* >::iterator  MulIter;
		int Count = (int)_UserGroupInfo.count( Obj->DBID() );

		MulIter = _UserGroupInfo.find( Obj->DBID() );

		for( int i = 0 ; i < Count ; i++ , MulIter++ )
		{
			if( MulIter->second == this )
			{
				_UserGroupInfo.erase( MulIter );
				break;
			}
		}
	}
	*/
	multimap< int , GroupObjectClass* >::iterator  MulIter;
	for( MulIter = _UserGroupInfo.begin() ; MulIter != _UserGroupInfo.end() ;  )
	{
		if( MulIter->second == this )
		{
			MulIter = _UserGroupInfo.erase( MulIter );		
		}
		else
		{
			MulIter++;
		}
	}
}
//----------------------------------------------------------------------------------------
bool    GroupObjectClass::AddUser( BaseItemObject*  Obj )
{

    pair< set<int>::iterator, bool > Result;
    Result = _UserList_SockID.insert( Obj->SockID() );
    
    if( Result.second == false )
        return false;
	
    typedef multimap< int , GroupObjectClass* >::value_type valueType;
    _UserGroupInfo.insert( valueType( Obj->DBID() , this ) );

    OnAddUser( Obj );
    return true;
}
//----------------------------------------------------------------------------------------
bool	GroupObjectClass::CheckInGroup( BaseItemObject* Obj )
{
	set< int >::iterator Iter = _UserList_SockID.find( Obj->SockID() );

	if( Iter == _UserList_SockID.end() )
		return false;

	return true;
}
//----------------------------------------------------------------------------------------
bool    GroupObjectClass::DelUser( BaseItemObject*  Obj )
{
    set< int >::iterator Iter = _UserList_SockID.find( Obj->SockID() );

    if( Iter == _UserList_SockID.end() )
        return false;


	OnDelUser( Obj );

    multimap< int , GroupObjectClass* >::iterator  MulIter;
    int Count = (int)_UserGroupInfo.count( Obj->DBID() );
    int i;

    MulIter = _UserGroupInfo.find( Obj->DBID() );

    for( i = 0 ; i < Count ; i++ , MulIter++ )
    {
        if( MulIter->second == this )
        {
            _UserGroupInfo.erase( MulIter );
            break;
        }
    }

    _UserList_SockID.erase( Iter );

    return true;
}
//----------------------------------------------------------------------------------------
bool     GroupObjectClass::ReleaseAll( BaseItemObject*  Obj )
{
	multimap< int , GroupObjectClass* >::iterator  MulIter,BegIter;
	int Count = (int)_UserGroupInfo.count( Obj->DBID() );
	int i;

	BegIter = MulIter = _UserGroupInfo.find( Obj->DBID() );

	for( i = 0 ; i < Count ; i++  )
	{
		GroupObjectClass* GroupObj = MulIter->second;

		MulIter = _UserGroupInfo.erase( MulIter );

		set< int >::iterator Iter = GroupObj->_UserList_SockID.find( Obj->SockID() );

		if( Iter == GroupObj->_UserList_SockID.end() )
			continue;

		if( GroupObj->OnUserLogout( Obj ) == true )
			GroupObj->OnDelUser( Obj );

		GroupObj->_UserList_SockID.erase( Iter );
	}

	return true;
}
//----------------------------------------------------------------------------------------
void		GroupObjectClass::ChangeRoleInfo(  BaseItemObject*  Obj )
{
	multimap< int , GroupObjectClass* >::iterator  MulIter;
	int Count = (int)_UserGroupInfo.count( Obj->DBID() );
	int i;

	MulIter = _UserGroupInfo.find( Obj->DBID() );

	for( i = 0 ; i < Count ; i++ , MulIter++ )
	{
		GroupObjectClass* GroupObj = MulIter->second;

		GroupObj->OnChangeRoleInfo( Obj );
	}
}
//----------------------------------------------------------------------------------------
