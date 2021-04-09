#include "M_BaseItemObject.h"
#include "..\Global.h"
//----------------------------------------------------------------------------------------
//		�R�A�ܼ�
//----------------------------------------------------------------------------------------
RecycleBin< RoleDataEx >	    BaseItemObject::_RoleDataRecycle;
vector< BaseItemObject* >	    BaseItemObject::_AllPlayerObj; 
//FunctionSchedularClass*			BaseItemObject::_Schedular;
//----------------------------------------------------------------------------------------
//		�R�A
//----------------------------------------------------------------------------------------
void    BaseItemObject::Init( )
{
//	_Schedular = FunctionSchedularClass::New();
	//_Schedular = Global::Schedular();
    //�]�w�w�ɰ���OnTimeProc
    _OnTimeProc_1Sec( NULL , NULL );
}
//----------------------------------------------------------------------------------------
void    BaseItemObject::Release( )
{
//	_Schedular->Del();
}
//----------------------------------------------------------------------------------------
int		BaseItemObject::_OnTimeProc_1Sec( SchedularInfo* Obj , void* InputClass )
{
    OnTimeProc( );
		
	Global::Schedular()->Push( _OnTimeProc_1Sec , _Def_OnTimeProc_Time_ , NULL , NULL );
    return 0;
}
//----------------------------------------------------------------------------------------
bool	BaseItemObject::CheckMemory( )
{ 
    return _RoleDataRecycle.CheckAllMem( ); 
}
//----------------------------------------------------------------------------------------
//���o�Y��ID��������
BaseItemObject* BaseItemObject::GetObj( int ID )
{

    if( (unsigned)ID >= _AllPlayerObj.size() )
        return NULL;

    return _AllPlayerObj[ ID ];

}
//----------------------------------------------------------------------------------------
//�D�n�ݦ��S������n�R��(�w�ɭn�B�z) �C��
void	BaseItemObject::OnTimeProc( )
{
    int     i;
    //----------------------------------------------------------------------------------------
    BaseItemObject*     Obj;
    //�B�z���ϰ�Ҧ�������

    for( i = 0 ; i < GetMaxObjID() ; i++ )
    {
        Obj = _AllPlayerObj[ i ];
        if( Obj == NULL )
            continue;

        if( Obj->_IsDestroy  )
            continue;

        delete Obj;
    }
}
//----------------------------------------------------------------------------------------
int     BaseItemObject::GetMaxObjID( )
{
    return (int)_AllPlayerObj.size();
}
//----------------------------------------------------------------------------------------
//		�ʺA
//----------------------------------------------------------------------------------------
BaseItemObject::BaseItemObject(  int SockID  )
{


    _Role = (RoleDataEx*)_RoleDataRecycle.NewObj();

    if( _Role == NULL)
    {
        //printf("\n���͹L�h������A�����ƪ��󲣥ͤ��X�� or ��Ƭ��^��");
		Print(LV2, "BaseItemObject", "���͹L�h������A�����ƪ��󲣥ͤ��X�� or ��Ƭ��^��");
        return ;
    }

    _Role->Init();
    _SockID = SockID;
    _AllPlayerObj[ SockID ] = this;
}
//----------------------------------------------------------------------------------------
BaseItemObject::~BaseItemObject( )
{
    if( (unsigned)_SockID >= _AllPlayerObj.size() )
    {
        //printf( "~BaseItemObject  �����D _SockID >= _AllPlayerObj.size() ????????????" );
		Print(LV2, "BaseItemObject", "~BaseItemObject  �����D _SockID >= _AllPlayerObj.size() ????????????" );
        return ;
    }
    _AllPlayerObj[ _SockID ] = NULL;
    if( _Role == NULL )
    {
        //printf( "�L�k�^����������" );
		Print(LV2, "BaseItemObject", "�L�k�^����������" );
        return;
    }
    _RoleDataRecycle.DeleteObj( _Role );
}
//----------------------------------------------------------------------------------------
//���o������
RoleDataEx*		BaseItemObject::Role( ) 
{ 
    return _Role; 
}	

void			BaseItemObject::Destroy( ) 
{ 
    _IsDestroy = true;
}
