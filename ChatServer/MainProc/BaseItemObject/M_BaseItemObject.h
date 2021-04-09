#ifndef _BASEITEMOBJECTCLASS_H_MASTER__
#define _BASEITEMOBJECTCLASS_H_MASTER__
#pragma		warning (disable:4786)


#include <list>
#include <vector>
#include <map>

#include "RecycleBin/recyclebin.h"
#include "functionschedular/functionschedular.h"
#include "roledata/RoleData.h"
//----------------------------------------------------------------------------------------
//最大連線數量
#define _Def_Max_Sock_ID_				0x8000
#define _Def_OnTimeProc_Time_           1000
#define _Def_Base_ZoneID_               10000       //區域id的最大值
//----------------------------------------------------------------------------------------
class ChatRoleData
{
public:
	
	int             SockID;
	int				ProxyID;
	int             DBID;
	StaticString< _MAX_ACCOUNT_SIZE_ >   Account;
	int             LocalID;		// Game Server GItemID 區域物件ID
	int             ZoneID;			//所在的區域Srv
	StaticString< _MAX_NAMERECORD_SIZE_ >   RoleName;
	int				PartyID;	
	int				MapZoneID;		//區域頻頻ID

	
	Voc_ENUM		Voc;
	Voc_ENUM		Voc_Sub;
	Sex_ENUM		Sex;
	StaticString< _MAX_FRIENDLIST_NOTE_ >   Note;	    //註解	
	LookStruct		Look;
	int				LV;
	int				LV_Sub;
	int				TitleID;
	int				GuildID;
	PlayerStateStruct	State;
	int				ParallelID;


	ChatRoleData()
	{
		memset( this , 0 , sizeof( *this ) );
		MapZoneID = -1;
	}
	void GetOnlinePlayerInfo( OnlinePlayerInfoStruct& Info )
	{
		Info.DBID		= DBID;
		Info.Name		= RoleName;	    //角色名稱	
		Info.GuildID	= GuildID;	//公會 ID
		Info.TitleID	= TitleID;
		Info.Note		= Note;	    //自己的註解	
		Info.GUID		= LocalID;	// 區域物件ID 
		Info.Voc		= Voc;
		Info.Voc_Sub	= Voc_Sub;
		Info.Sex		= Sex;
		Info.Look		= Look;
		Info.LV			= LV;
		Info.LV_Sub		= LV_Sub;
		Info.State		= State;	//玩家全區狀態
	}
};

class BaseItemObject
{
protected:
//----------------------------------------------------------------------------------------
//		靜態
//----------------------------------------------------------------------------------------
    static vector< BaseItemObject* >	    _AllPlayerObj_SockID;     //所有玩家物件
    static map< int , BaseItemObject* >     _AllPlayerObj_DBID;
    static map< string , BaseItemObject* >  _AllPlayerObj_Account;
    static map< string , BaseItemObject* >  _AllPlayerObj_RoleName;

    //丟給排程處理的函式
    static	int		_OnTimeProc_1Sec( SchedularInfo* Obj , void* InputClass );
public:
    static  void    Init( );
	static  void    Release( );
    static	void	OnTimeProc( );			    //主要看有沒有物件要刪除(定時要處理) 每秒
    static	int     GetMaxObjID( );

    static	BaseItemObject* GetObjBySockID( int SockID );           //取物件
    static	BaseItemObject* GetObjByDBID( int DBID );               //取物件
    static	BaseItemObject* GetObjByRoleName( const char* Name );   //取物件
    static	BaseItemObject* GetObjByAccount( const char* Account ); //取物件

	static  vector< BaseItemObject* >*	   GetAllPlayerObj_SockID( );

//	static bool		LoginCheck( )
protected:
//----------------------------------------------------------------------------------------
//		動態
//----------------------------------------------------------------------------------------
    bool                        _IsDestroy;     //是否要刪除

	ChatRoleData				_Role;

public:

	BaseItemObject( int SockID , int ProxyID , char* Account );    
	~BaseItemObject( );

    int     DBID()      { return _Role.DBID; }
    int     SockID()    { return _Role.SockID; }
    int     LocalID()   { return _Role.LocalID; }
    int     ZoneID()    { return _Role.ZoneID; }
    char*   RoleName( ) { return (char*)_Role.RoleName.Begin(); }
    char*   Account()   { return (char*)_Role.Account.Begin(); }
	int		MapZoneID()	{ return _Role.MapZoneID; };
	
    bool    DBID( int DBID );
    void    LocalID( int ID ){ _Role.LocalID = ID; }	//物件在區域內的號碼
	void	Destroy( );							//刪除物件
	bool	IsDestroy()		{ return _IsDestroy; }
    bool    RoleName( char* Name );
    void    ZoneID( int ID );
	void	MapZoneID( int ID );

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	int		PartyID(){ return _Role.PartyID; }
	void	PartyID( int Value ) { _Role.PartyID = Value; }
	ChatRoleData*	Role() { return & _Role; };
};

#endif