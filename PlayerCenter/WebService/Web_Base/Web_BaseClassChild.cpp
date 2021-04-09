#include "Web_BaseClassChild.h"
#include "../../NetWalker_Member/NetWakerRoleDataCenterInc.h"
//-------------------------------------------------------------------
bool    Web_BaseClassChild::Init()
{
	Web_BaseClass::_Init();

	if( This != NULL)
		return false;

	This = NEW( Web_BaseClassChild );

	return true;
}

//--------------------------------------------------------------------------------------
bool    Web_BaseClassChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	Web_BaseClass::_Release();
	return true;

}

struct TempLoadAccountInfoStruct
{
	int		NetID;
	string	Account;
};

void Web_BaseClassChild::RC_AccountDataRequest	( int NetID , const char* Account )
{
	TempLoadAccountInfoStruct* TempData = NEW TempLoadAccountInfoStruct;
	TempData->NetID = NetID;
	TempData->Account = Account;

	set< string >::iterator Iter = CNetDC_Login_Child::OnLoadAccountSet().find( Account );
	if( Iter == CNetDC_Login_Child::OnLoadAccountSet().end() )
	{
		CNetDC_Login_Child::m_pThis->On_LoadAccount( 0 , 0 , Account );
	}
	Global::Schedular()->Push( _WaitLoadAccountProc_ , 1000, TempData , NULL );
}

int Web_BaseClassChild::_WaitLoadAccountProc_( SchedularInfo* Obj , void* InputClass )
{
	TempLoadAccountInfoStruct* TempData = ( TempLoadAccountInfoStruct * )InputClass;

	vector< BaseItemObject*>& RoleList = *( BaseItemObject::GetObj_Account( (char*)TempData->Account.c_str() ) );
	
	SC_AccountDataResult_Base( TempData->NetID , TempData->Account.c_str() , RoleList.size() );

	for( unsigned i = 0 ; i < RoleList.size() ; i++ )
	{
		RoleDataEx* Role = (RoleDataEx*)RoleList[i]->Role();		

		SC_AccountDataResult_RoleName( TempData->NetID , Role->DBID() , Role->RoleName() );
	}

	Global::Schedular()->Push( _WaitLoadAccountProc_ , 1000, TempData , NULL );
	return 0;
}

void Web_BaseClassChild::RC_RoleValueRequest	( int NetID , const char* RoleName , int ValueCount , RoleValueName_ENUM ValueType[] )
{
	RoleValueStruct ValueResult[0x1000];
	BaseItemObject* RoleObj = BaseItemObject::GetObj_Name( RoleName );
	if( RoleObj == NULL )
	{
		SC_RoleValueResult( NetID , RoleName , 0 , ValueResult );
		return;
	}
	RoleDataEx* Role = (RoleDataEx*)RoleObj->Role();
	Role->InitCal();

	for( int i = 0 ; i < ValueCount ; i++ )
	{
		ValueResult[i].Type = ValueType[i];
		ValueResult[i].Value = (float)Role->GetValue( ValueType[i] );
	}

	SC_RoleValueResult( NetID , RoleName , ValueCount , ValueResult );
}
void Web_BaseClassChild::RC_RoleItemRequest		( int NetID , const char* RoleName , int FieldType  )
{
	/*
	BaseItemObject* RoleObj = BaseItemObject::GetObj_Name( RoleName );
	if( RoleObj == NULL )
	{
		SC_RoleValueResult( NetID , RoleName , 0 , ValueResult );
		return;
	}
	RoleDataEx* Role = (RoleDataEx*)RoleObj->Role();
	*/
}


