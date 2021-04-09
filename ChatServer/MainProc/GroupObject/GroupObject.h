#ifndef __GROUPOBJECT_H__
#define __GROUPOBJECT_H__
#pragma		warning (disable:4786)

//#include <list>
#include <vector>
#include <map>
#include <set>

#include "../BaseItemObject/M_BaseItemObject.h"
#include "PG/PG_ChannelBase.h"

using namespace std;
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
class GroupObjectClass
{
protected:
    static vector< GroupObjectClass* >				_GroupList[ EM_GroupObjectType_Count ];
    static vector< int >							_UnusedIDList[ EM_GroupObjectType_Count ];
    static multimap< int , GroupObjectClass* >		_UserGroupInfo;			//記錄使用者有加入的群組 
    //----------------------------------------------------------------------------------------
    static	int		_OnTimeProc_Schedular( SchedularInfo* Obj , void* InputClass );
public:


    static bool     Init( );
	static bool     Release( );
    static bool     ProcessAll( );         //主要用來檢查是否有物件要刪除
    static GroupObjectClass* GetObj( GroupObjectTypeEnum Type , int ID );

	static int		MaxCount( GroupObjectTypeEnum Type );
    static bool     ReleaseAll( BaseItemObject* );
    //----------------------------------------------------------------------------------------
	static void		ChangeRoleInfo( BaseItemObject*  Obj );
	static multimap< int , GroupObjectClass* >*		UserGroupInfo(){ return &_UserGroupInfo; };			//記錄使用者有加入的群組 
protected:
    set< int >  _UserList_SockID;
    GroupObjectTypeEnum     _Type;
    int                     _ID;

    bool                    _IsDestroy;

public:

	GroupObjectClass( GroupObjectTypeEnum Type );    
	virtual ~GroupObjectClass( );

	GroupObjectTypeEnum     Type() { return _Type; };
    int         ID(){ return _ID; }
	bool		CheckInGroup( BaseItemObject* Obj );

    set< int >*     UserList_SockID() { return &_UserList_SockID; };

    virtual bool    AddUser( BaseItemObject* );
    virtual bool    DelUser( BaseItemObject* );	
	virtual bool	ChangeOwner( int PlayerDBID ){return false;};

	bool	IsReady(){ return !_IsDestroy; };
	void	Destroy(){ _IsDestroy = true; };
	

	virtual void Process(){};

    virtual void OnCreate(){};
    virtual void OnDestroy(){};
    virtual void OnAddUser( BaseItemObject*  Obj ){};
    virtual void OnDelUser( BaseItemObject*  Obj ){};
	virtual bool OnUserLogout( BaseItemObject*  Obj ){ return true;};	//return true 表示要執行 OnDelUser
	virtual void OnChangeRoleInfo( BaseItemObject*  Obj ){};			//當角色資料有更變或換區

};
//----------------------------------------------------------------------------------------
#endif