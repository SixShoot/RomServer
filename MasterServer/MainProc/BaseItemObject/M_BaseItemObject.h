#ifndef _BASEITEMOBJECTCLASS_H_MASTER__
#define _BASEITEMOBJECTCLASS_H_MASTER__
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
#define _Def_OnTimeProc_Time_           1000
#define _Def_Base_ZoneID_               10000       //區域id的最大值
//----------------------------------------------------------------------------------------
class BaseItemObject
{
protected:
//----------------------------------------------------------------------------------------
//		靜態
//----------------------------------------------------------------------------------------
    static RecycleBin< RoleDataEx >		_RoleDataRecycle;  //角色資源回收物件
    static vector< BaseItemObject* >	_AllPlayerObj;     //所有玩家物件
	static FunctionSchedularClass*		_Schedular;
    //丟給排程處理的函式
    static	int		_OnTimeProc_1Sec( SchedularInfo* Obj , void* InputClass );
public:
    static  void    Init( );
	static  void    Release( );
    static	void	OnTimeProc( );			    //主要看有沒有物件要刪除(定時要處理) 每秒
    static	bool	CheckMemory( );
    static	int     GetMaxObjID( );

    static	BaseItemObject* GetObj( int );      //取物件
protected:
//----------------------------------------------------------------------------------------
//		動態
//----------------------------------------------------------------------------------------
	RoleDataEx*			    	_Role;	        //角色資料
    bool                        _IsDestroy;     //是否要刪除
    int                         _SockID;
public:

	BaseItemObject( int SockID );    
	~BaseItemObject( );

	RoleDataEx*		Role( );	        //取得角色資料
	void			Destroy( );         //刪除物件


};

#endif