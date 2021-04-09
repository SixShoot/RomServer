#include "M_BaseItemObject.h"
#include "..\Global.h"
//----------------------------------------------------------------------------------------
//		靜態變數
//----------------------------------------------------------------------------------------
RecycleBin< RoleDataEx >	    BaseItemObject::_RoleDataRecycle;
vector< BaseItemObject* >	    BaseItemObject::_AllPlayerObj; 
//FunctionSchedularClass*			BaseItemObject::_Schedular;
//----------------------------------------------------------------------------------------
//		靜態
//----------------------------------------------------------------------------------------
void    BaseItemObject::Init( )
{
//	_Schedular = FunctionSchedularClass::New();
	//_Schedular = Global::Schedular();
    //設定定時執行OnTimeProc
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
//取得某個ID的物件資料
BaseItemObject* BaseItemObject::GetObj( int ID )
{

    if( (unsigned)ID >= _AllPlayerObj.size() )
        return NULL;

    return _AllPlayerObj[ ID ];

}
//----------------------------------------------------------------------------------------
//主要看有沒有物件要刪除(定時要處理) 每秒
void	BaseItemObject::OnTimeProc( )
{
    int     i;
    //----------------------------------------------------------------------------------------
    BaseItemObject*     Obj;
    //處理本區域所有的物件

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
//		動態
//----------------------------------------------------------------------------------------
BaseItemObject::BaseItemObject(  int SockID  )
{


    _Role = (RoleDataEx*)_RoleDataRecycle.NewObj();

    if( _Role == NULL)
    {
        //printf("\n產生過多的物件，角色資料物件產生不出來 or 資料為回收");
		Print(LV2, "BaseItemObject", "產生過多的物件，角色資料物件產生不出來 or 資料為回收");
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
        //printf( "~BaseItemObject  有問題 _SockID >= _AllPlayerObj.size() ????????????" );
		Print(LV2, "BaseItemObject", "~BaseItemObject  有問題 _SockID >= _AllPlayerObj.size() ????????????" );
        return ;
    }
    _AllPlayerObj[ _SockID ] = NULL;
    if( _Role == NULL )
    {
        //printf( "無法回收此角色資料" );
		Print(LV2, "BaseItemObject", "無法回收此角色資料" );
        return;
    }
    _RoleDataRecycle.DeleteObj( _Role );
}
//----------------------------------------------------------------------------------------
//取得角色資料
RoleDataEx*		BaseItemObject::Role( ) 
{ 
    return _Role; 
}	

void			BaseItemObject::Destroy( ) 
{ 
    _IsDestroy = true;
}
