#pragma once

#include "Web_BaseClass.h"
//--------------------------------------------------------------------------------------------
class Web_BaseClassChild : public Web_BaseClass
{

	static int _WaitLoadAccountProc_( SchedularInfo* Obj , void* InputClass );
public:
	//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();
	//--------------------------------------------------------------------------------------------
	void RC_AccountDataRequest	( int NetID , const char* Account );
	void RC_RoleValueRequest	( int NetID , const char* RoleName , int ValueCount , RoleValueName_ENUM ValueType[] );
	void RC_RoleItemRequest		( int NetID , const char* RoleName , int FieldType  );
};
