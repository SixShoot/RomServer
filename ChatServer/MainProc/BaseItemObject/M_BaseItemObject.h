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
//�̤j�s�u�ƶq
#define _Def_Max_Sock_ID_				0x8000
#define _Def_OnTimeProc_Time_           1000
#define _Def_Base_ZoneID_               10000       //�ϰ�id���̤j��
//----------------------------------------------------------------------------------------
class ChatRoleData
{
public:
	
	int             SockID;
	int				ProxyID;
	int             DBID;
	StaticString< _MAX_ACCOUNT_SIZE_ >   Account;
	int             LocalID;		// Game Server GItemID �ϰ쪫��ID
	int             ZoneID;			//�Ҧb���ϰ�Srv
	StaticString< _MAX_NAMERECORD_SIZE_ >   RoleName;
	int				PartyID;	
	int				MapZoneID;		//�ϰ��W�WID

	
	Voc_ENUM		Voc;
	Voc_ENUM		Voc_Sub;
	Sex_ENUM		Sex;
	StaticString< _MAX_FRIENDLIST_NOTE_ >   Note;	    //����	
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
		Info.Name		= RoleName;	    //����W��	
		Info.GuildID	= GuildID;	//���| ID
		Info.TitleID	= TitleID;
		Info.Note		= Note;	    //�ۤv������	
		Info.GUID		= LocalID;	// �ϰ쪫��ID 
		Info.Voc		= Voc;
		Info.Voc_Sub	= Voc_Sub;
		Info.Sex		= Sex;
		Info.Look		= Look;
		Info.LV			= LV;
		Info.LV_Sub		= LV_Sub;
		Info.State		= State;	//���a���Ϫ��A
	}
};

class BaseItemObject
{
protected:
//----------------------------------------------------------------------------------------
//		�R�A
//----------------------------------------------------------------------------------------
    static vector< BaseItemObject* >	    _AllPlayerObj_SockID;     //�Ҧ����a����
    static map< int , BaseItemObject* >     _AllPlayerObj_DBID;
    static map< string , BaseItemObject* >  _AllPlayerObj_Account;
    static map< string , BaseItemObject* >  _AllPlayerObj_RoleName;

    //�ᵹ�Ƶ{�B�z���禡
    static	int		_OnTimeProc_1Sec( SchedularInfo* Obj , void* InputClass );
public:
    static  void    Init( );
	static  void    Release( );
    static	void	OnTimeProc( );			    //�D�n�ݦ��S������n�R��(�w�ɭn�B�z) �C��
    static	int     GetMaxObjID( );

    static	BaseItemObject* GetObjBySockID( int SockID );           //������
    static	BaseItemObject* GetObjByDBID( int DBID );               //������
    static	BaseItemObject* GetObjByRoleName( const char* Name );   //������
    static	BaseItemObject* GetObjByAccount( const char* Account ); //������

	static  vector< BaseItemObject* >*	   GetAllPlayerObj_SockID( );

//	static bool		LoginCheck( )
protected:
//----------------------------------------------------------------------------------------
//		�ʺA
//----------------------------------------------------------------------------------------
    bool                        _IsDestroy;     //�O�_�n�R��

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
    void    LocalID( int ID ){ _Role.LocalID = ID; }	//����b�ϰ줺�����X
	void	Destroy( );							//�R������
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