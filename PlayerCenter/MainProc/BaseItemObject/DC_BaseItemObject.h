#ifndef _BASEITEMOBJECTCLASS_H_DATACENTER__
#define _BASEITEMOBJECTCLASS_H_DATACENTER__
#pragma		warning (disable:4786)


#include <list>
#include <vector>
#include <map>

#include "RecycleBin/recyclebin.h"
#include "roledata/RoleDataEx.h"
#include "functionschedular/functionschedular.h"
//----------------------------------------------------------------------------------------
//最大連線數量
#define _Def_Max_Sock_ID_				0x8000
#define _Def_OnTimeProc_Time_           5000
//----------------------------------------------------------------------------------------
class BaseItemObject
{
protected:
//----------------------------------------------------------------------------------------
//		靜態
//----------------------------------------------------------------------------------------
	struct StaticStruct
	{
		RecycleBin< PlayerRoleData >		    RoleDataRecycle;	//角色資源回收物件
		map< int , BaseItemObject* >	        AllPlayerObj;		//所有玩家物件
		multimap< string , BaseItemObject* >	AllPlayerObj_ByAccounID;
		map< int , BaseItemObject* >	        AllPlayerObj_DBID;  //所有玩家物件

		vector< BaseItemObject* >				DelList;     
		vector< BaseItemObject* >				Ret;				//回傳暫存資料


		int										WaitLogoutCount;	//等待登出
		int										NeedSaveCount;
		int										SaveCount;

		StaticStruct();
		~StaticStruct();
	};

	static StaticStruct*	_St;

    //丟給排程處理的函式
    static	int		_OnTimeProc_Sec( SchedularInfo* Obj , void* InputClass );
public:
    static  void    Init( );
	static  void    Release( );
    static	void	OnTimeProc( );			                //主要看有沒有物件要刪除(定時要處理) 每秒
	static	void	SaveAll_Block();						//等待儲存完
    static	bool	CheckMemory( );
    static  bool    CheckAccountEmpty( string Account );    //檢查是否某個 Account 是空的
	static  bool    CheckAccountIsLoading( string Account );    //檢查是否某個 Account 是空的


    static	BaseItemObject* GetObj( int );                  //取物件
    static  BaseItemObject* GetObj_DBID( int );
    static  vector< BaseItemObject*>* GetObj_Account( char* Account );
	static  BaseItemObject* GetObj_Name( const char* Name );
	static  StaticStruct*	St()	{ return _St;};
protected:
//----------------------------------------------------------------------------------------
//		動態
//----------------------------------------------------------------------------------------
	int							_CheckMemory[256];	//檢查用記憶體
	PlayerRoleData*		    	_Role;	            //角色資料	
	//PlayerRoleData*		    	_RoleDB;			//角色資料_資料庫貝份
	char*				    	_RoleDBZip;			//角色資料_資料庫貝份
	int					    	_RoleDBZipSize;			//角色資料_資料庫貝份
    int                         _DBID;
    string                      _AccountID;         //帳號
    int                         _LiveTime;
    int                         _SaveTime;          // -1 代表不用儲存
	bool						_IsNeedSave;
    bool                        _IsSave;            //正在儲存
	bool						_IsLoadOK;			//載入OK
    int                         _ThreadUseCount;    //
	int							_DelErrCount;		//殺幾次殺不掉
	bool						_IsNeedSaveAll;		//需要全部儲存
	bool						_IsInDelList;		//已經丟入要刪除的列表
public:

	BaseItemObject( PlayerRoleData* Role );    
	~BaseItemObject( );

	PlayerRoleData*		Role( );	            //取得角色資料
	char*				RoleDBZip( );	            //取得角色資料
	void				SetRoleDBZip( PlayerRoleData* role );
	bool				GetRoleDB( PlayerRoleData& role );

	void			Destroy( );             //刪除物件
    void            SetLiveTime( int Value );
	void			ClearRoleDB();

	bool			IsNeedSave() { return _IsNeedSave;}
	void			IsNeedSave( bool Flag ) { _IsNeedSave = Flag; }
    bool			IsSave() { return _IsSave;}
    void			IsSave( bool Flag ) { _IsSave = Flag; }
	void            SaveTime( int Value ){ _SaveTime = Value; };
	int				SaveTime( ){ return _SaveTime;} ;
    
    void            PushToThread( );
    void            PopFromThread( );
	bool			IsLoadOK()	{ return _IsLoadOK; };
	void			IsLoadOK( bool Value )	{ _IsLoadOK = Value; };
	bool			IsNeedSaveAll()	{ return _IsNeedSaveAll; };
	void			IsNeedSaveAll( bool Value )	{ _IsNeedSaveAll = Value ; };

//----------------------------------------------------------------------------------------
};

#endif//_BASEITEMOBJECTCLASS_H_DATACENTER__