#ifndef _BASEITEMOBJECTCLASS_H_MASTER__
#define _BASEITEMOBJECTCLASS_H_MASTER__
#pragma		warning (disable:4786)


#include <list>
#include <vector>
#include <map>

#include "RecycleBin/recyclebin.h"
#include "functionschedular/functionschedular.h"
#include "../SrvPartitionProc/SrvPartitionProc.h"
#include "MapAreaManagement/MyMapAreaManagement.h"
#include "PG/NetWakerPGEnum.h"
#include "roledata/RoleBaseDef.h"
//----------------------------------------------------------------------------------------
struct ZoneMapNameStruct
{
	string	FileName;
	int		ZoneBase;

	bool operator == ( const ZoneMapNameStruct& P ) const 
	{
		if( P.ZoneBase == ZoneBase && P.FileName == FileName )
			return true;
		return false;
	}

};
//----------------------------------------------------------------------------------------
class BaseItemObject
{
protected:
//----------------------------------------------------------------------------------------
//		靜態
//----------------------------------------------------------------------------------------
	static vector< ZoneMapNameStruct >				_ZoneMapFileName;
	static map< int , int >							_ZoneIDToMapID;
    static vector< vector<BaseItemObject*> >		_AllObj;			//類似_AllObj[][];
    static vector<SrvPartitionProc*>				_AllSrvPart;		//Server註冊的資料
    static vector< vector<MyMapAreaManagement*> >	_AllObjPart;		//管理物件 分割區移動
    static int      _BlockSize;
public:
    // BlockSize  為分割區
    static  bool    Init( );
    static  bool    Release( );

	static  int		AddSrv( int ZoneID , string MapFileName , int SubMapID );
	static  bool	DelSrv( int ZoneID );

    static  BaseItemObject* GetObj( int GItemID );
    static  BaseItemObject* GetObj( int ZoneID , int ItemID );
	static  vector<BaseItemObject*>*    GetZoneObj( int ZoneID );
protected:
//----------------------------------------------------------------------------------------
//		動態
//----------------------------------------------------------------------------------------

    GItemData   _ItemID;
    int         _SecX;
    int         _SecY;
	int			_OrgParallelID;
	int			_ParallelID;
    bool        _IsDestroy;     //是否要刪除
    bool        _IsAddSrv;
    bool        _IsAddAllObj;
	string		_RoleName;

    bool        _Add( );
    bool        _Del( );

	SrvPartitionProc*				_SrvPart;   //Server註冊的資料
	MyMapAreaManagement*			_ObjPart;   //管理物件 分割區移動

public:

	BaseItemObject( int ZoneID , int ItemID , int ParallelID , char* RoleName , int X , int Y );    
	~BaseItemObject( );

	const char*					RoleName(){ return _RoleName.c_str(); }
    
    bool                        SetPos( int X , int Y );
    //*注意* 新增移除 Srv 會影響內容
    vector < SrvInfoStruct* >*  GetViewSrv( );  

};

#endif