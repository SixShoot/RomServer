#include "Web_BaseClass.h"

//-------------------------------------------------------------------
Web_BaseClass*	Web_BaseClass::This = NULL;
TelnetClass*	Web_BaseClass::_Telnet = NULL;
//-------------------------------------------------------------------
bool Web_BaseClass::_Init()
{
	_Telnet = NEW TelnetClass;

	return true;
}
//-------------------------------------------------------------------
bool Web_BaseClass::_Release()
{
	delete _Telnet;
	return false;
}
//-------------------------------------------------------------------
void Web_BaseClass::_PG_CtoS_AccountDataRequest		( int Sockid , int Size , void* Data )
{
	PG_CtoS_AccountDataRequest* PG = (PG_CtoS_AccountDataRequest*)Data;
	if( Size != sizeof(PG_CtoS_AccountDataRequest) )	
		return;

	This->RC_AccountDataRequest( Sockid , PG->Account );
//	virtual void RC_AccountDataRequest	( ) = 0;
//	virtual void RC_RoleValueRequest	( ) = 0;
//	virtual void RC_RoleItemRequest		( ) = 0;
}
void Web_BaseClass::_PG_CtoS_RoleValueRequest		( int Sockid , int Size , void* Data )
{
	PG_CtoS_RoleValueRequest* PG = (PG_CtoS_RoleValueRequest*)Data;
	if( Size != sizeof(PG_CtoS_RoleValueRequest) )	
		return;

	This->RC_RoleValueRequest( Sockid , PG->RoleName , PG->ValueCount , PG->ValueType );

}
void Web_BaseClass::_PG_CtoS_RoleItemRequest		( int Sockid , int Size , void* Data )
{
	PG_CtoS_RoleItemRequest* PG = (PG_CtoS_RoleItemRequest*)Data;
	if( Size != sizeof(PG_CtoS_RoleItemRequest) )	
		return;

	This->RC_RoleItemRequest( Sockid , PG->RoleName , PG->FieldType );
}

void Web_BaseClass::SC_AccountDataResult_Base		( int NetID , const char* Account , int RoleCount )
{
	PG_StoC_AccountDataResult_Base Send;
	memcpy( Send.Account , Account , sizeof( Send.Account ) );
	_Telnet->Send( NetID , sizeof(Send) , &Send );
}
void Web_BaseClass::SC_AccountDataResult_RoleName	( int NetID , int DBID , const char* RoleName )
{
	PG_StoC_AccountDataResult_RoleName Send;
	Send.DBID = DBID;
	memcpy( Send.RoleName , RoleName , sizeof( Send.RoleName ) );
	_Telnet->Send( NetID , sizeof(Send) , &Send );
}

void Web_BaseClass::SC_RoleValueResult	( int NetID , const char* RoleName , int ValueCount , RoleValueStruct Value[] )
{
	PG_StoC_RoleValueResult Send;
	
	memcpy( Send.RoleName , RoleName , sizeof( Send.RoleName ) );
	memcpy( Send.Value , Value , sizeof( Send.Value ) );
	Send.ValueCount = ValueCount;
	_Telnet->Send( NetID , sizeof(Send) , &Send );
}

void Web_BaseClass::SC_RoleItemResult		( int NetID , int FieldType , vector<ItemFieldStruct>& Item )
{
	PG_StoC_RoleItemResult Send;
	Send.FieldType = FieldType;
	Send.ItemCount = Item.size();
	for( unsigned i = 0 ; i < Item.size() ; i++ )
	{
		Send.Item[i] = Item[i];
	}

	_Telnet->Send( NetID , Send.PGSize() , &Send );
}

void Web_BaseClass::Host( const char* IP , int Port )
{
	This->_Telnet->Host( IP , Port );
}
void Web_BaseClass::Process()
{
	This->_Telnet->Process();
}