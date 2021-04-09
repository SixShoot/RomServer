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
//		�R�A
//----------------------------------------------------------------------------------------
	static vector< ZoneMapNameStruct >				_ZoneMapFileName;
	static map< int , int >							_ZoneIDToMapID;
    static vector< vector<BaseItemObject*> >		_AllObj;			//����_AllObj[][];
    static vector<SrvPartitionProc*>				_AllSrvPart;		//Server���U�����
    static vector< vector<MyMapAreaManagement*> >	_AllObjPart;		//�޲z���� ���ΰϲ���
    static int      _BlockSize;
public:
    // BlockSize  �����ΰ�
    static  bool    Init( );
    static  bool    Release( );

	static  int		AddSrv( int ZoneID , string MapFileName , int SubMapID );
	static  bool	DelSrv( int ZoneID );

    static  BaseItemObject* GetObj( int GItemID );
    static  BaseItemObject* GetObj( int ZoneID , int ItemID );
	static  vector<BaseItemObject*>*    GetZoneObj( int ZoneID );
protected:
//----------------------------------------------------------------------------------------
//		�ʺA
//----------------------------------------------------------------------------------------

    GItemData   _ItemID;
    int         _SecX;
    int         _SecY;
	int			_OrgParallelID;
	int			_ParallelID;
    bool        _IsDestroy;     //�O�_�n�R��
    bool        _IsAddSrv;
    bool        _IsAddAllObj;
	string		_RoleName;

    bool        _Add( );
    bool        _Del( );

	SrvPartitionProc*				_SrvPart;   //Server���U�����
	MyMapAreaManagement*			_ObjPart;   //�޲z���� ���ΰϲ���

public:

	BaseItemObject( int ZoneID , int ItemID , int ParallelID , char* RoleName , int X , int Y );    
	~BaseItemObject( );

	const char*					RoleName(){ return _RoleName.c_str(); }
    
    bool                        SetPos( int X , int Y );
    //*�`�N* �s�W���� Srv �|�v�T���e
    vector < SrvInfoStruct* >*  GetViewSrv( );  

};

#endif