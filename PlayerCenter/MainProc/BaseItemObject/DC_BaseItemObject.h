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
//�̤j�s�u�ƶq
#define _Def_Max_Sock_ID_				0x8000
#define _Def_OnTimeProc_Time_           5000
//----------------------------------------------------------------------------------------
class BaseItemObject
{
protected:
//----------------------------------------------------------------------------------------
//		�R�A
//----------------------------------------------------------------------------------------
	struct StaticStruct
	{
		RecycleBin< PlayerRoleData >		    RoleDataRecycle;	//����귽�^������
		map< int , BaseItemObject* >	        AllPlayerObj;		//�Ҧ����a����
		multimap< string , BaseItemObject* >	AllPlayerObj_ByAccounID;
		map< int , BaseItemObject* >	        AllPlayerObj_DBID;  //�Ҧ����a����

		vector< BaseItemObject* >				DelList;     
		vector< BaseItemObject* >				Ret;				//�^�ǼȦs���


		int										WaitLogoutCount;	//���ݵn�X
		int										NeedSaveCount;
		int										SaveCount;

		StaticStruct();
		~StaticStruct();
	};

	static StaticStruct*	_St;

    //�ᵹ�Ƶ{�B�z���禡
    static	int		_OnTimeProc_Sec( SchedularInfo* Obj , void* InputClass );
public:
    static  void    Init( );
	static  void    Release( );
    static	void	OnTimeProc( );			                //�D�n�ݦ��S������n�R��(�w�ɭn�B�z) �C��
	static	void	SaveAll_Block();						//�����x�s��
    static	bool	CheckMemory( );
    static  bool    CheckAccountEmpty( string Account );    //�ˬd�O�_�Y�� Account �O�Ū�
	static  bool    CheckAccountIsLoading( string Account );    //�ˬd�O�_�Y�� Account �O�Ū�


    static	BaseItemObject* GetObj( int );                  //������
    static  BaseItemObject* GetObj_DBID( int );
    static  vector< BaseItemObject*>* GetObj_Account( char* Account );
	static  BaseItemObject* GetObj_Name( const char* Name );
	static  StaticStruct*	St()	{ return _St;};
protected:
//----------------------------------------------------------------------------------------
//		�ʺA
//----------------------------------------------------------------------------------------
	int							_CheckMemory[256];	//�ˬd�ΰO����
	PlayerRoleData*		    	_Role;	            //������	
	//PlayerRoleData*		    	_RoleDB;			//������_��Ʈw����
	char*				    	_RoleDBZip;			//������_��Ʈw����
	int					    	_RoleDBZipSize;			//������_��Ʈw����
    int                         _DBID;
    string                      _AccountID;         //�b��
    int                         _LiveTime;
    int                         _SaveTime;          // -1 �N�����x�s
	bool						_IsNeedSave;
    bool                        _IsSave;            //���b�x�s
	bool						_IsLoadOK;			//���JOK
    int                         _ThreadUseCount;    //
	int							_DelErrCount;		//���X��������
	bool						_IsNeedSaveAll;		//�ݭn�����x�s
	bool						_IsInDelList;		//�w�g��J�n�R�����C��
public:

	BaseItemObject( PlayerRoleData* Role );    
	~BaseItemObject( );

	PlayerRoleData*		Role( );	            //���o������
	char*				RoleDBZip( );	            //���o������
	void				SetRoleDBZip( PlayerRoleData* role );
	bool				GetRoleDB( PlayerRoleData& role );

	void			Destroy( );             //�R������
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