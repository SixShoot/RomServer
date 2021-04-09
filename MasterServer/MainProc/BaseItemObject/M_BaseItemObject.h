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
//�̤j�s�u�ƶq
#define _Def_Max_Sock_ID_				0x8000
#define _Def_OnTimeProc_Time_           1000
#define _Def_Base_ZoneID_               10000       //�ϰ�id���̤j��
//----------------------------------------------------------------------------------------
class BaseItemObject
{
protected:
//----------------------------------------------------------------------------------------
//		�R�A
//----------------------------------------------------------------------------------------
    static RecycleBin< RoleDataEx >		_RoleDataRecycle;  //����귽�^������
    static vector< BaseItemObject* >	_AllPlayerObj;     //�Ҧ����a����
	static FunctionSchedularClass*		_Schedular;
    //�ᵹ�Ƶ{�B�z���禡
    static	int		_OnTimeProc_1Sec( SchedularInfo* Obj , void* InputClass );
public:
    static  void    Init( );
	static  void    Release( );
    static	void	OnTimeProc( );			    //�D�n�ݦ��S������n�R��(�w�ɭn�B�z) �C��
    static	bool	CheckMemory( );
    static	int     GetMaxObjID( );

    static	BaseItemObject* GetObj( int );      //������
protected:
//----------------------------------------------------------------------------------------
//		�ʺA
//----------------------------------------------------------------------------------------
	RoleDataEx*			    	_Role;	        //������
    bool                        _IsDestroy;     //�O�_�n�R��
    int                         _SockID;
public:

	BaseItemObject( int SockID );    
	~BaseItemObject( );

	RoleDataEx*		Role( );	        //���o������
	void			Destroy( );         //�R������


};

#endif