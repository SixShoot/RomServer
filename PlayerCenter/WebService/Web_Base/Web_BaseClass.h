#pragma once
#include "../../MainProc/Global.h"
#include "PGWeb_Base.h"
#include "../TelnetClass.h"
class Web_BaseClass 
{
protected:
	//-------------------------------------------------------------------
	static bool				_Init();
	static bool				_Release();

	static TelnetClass*		_Telnet;
	//-------------------------------------------------------------------	
	static void _PG_CtoS_AccountDataRequest		( int Sockid , int Size , void* Data );
	static void _PG_CtoS_RoleValueRequest		( int Sockid , int Size , void* Data );
	static void _PG_CtoS_RoleItemRequest		( int Sockid , int Size , void* Data );

public:
	static Web_BaseClass*	This;
    //-------------------------------------------------------------------	
	static void Host( const char* IP , int Port );
	static void Process();
	//-------------------------------------------------------------------	
	virtual void RC_AccountDataRequest	( int NetID , const char* Account ) = 0;
		static void SC_AccountDataResult_Base		( int NetID , const char* Account , int RoleCount );
		static void SC_AccountDataResult_RoleName	( int NetID , int DBID , const char* RoleName );

	virtual void RC_RoleValueRequest	( int NetID , const char* RoleName , int ValueCount , RoleValueName_ENUM ValueType[] ) = 0;
		static void SC_RoleValueResult		( int NetID , const char* RoleName , int ValueCount , RoleValueStruct Value[] );

	virtual void RC_RoleItemRequest		( int NetID , const char* RoleName , int FieldType  ) = 0;
		static void SC_RoleItemResult		( int NetID , int FieldType , vector<ItemFieldStruct>& Item );


};
