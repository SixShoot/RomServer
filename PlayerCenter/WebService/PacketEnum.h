#pragma once

#define		_MAX_ACCOUNT_STRING_LEN_	64
#define		_MAX_NAME_STRING_LEN_		32

enum	WebServiceCommandENUM
{
	EM_WebServiceCommand_None = -1 ,

	EM_PG_CtoS_AccountDataRequest		= 0	,
	EM_PG_StoC_AccountDataResult_Base		,
	EM_PG_StoC_AccountDataResult_RoleName	,
	EM_PG_CtoS_RoleValueRequest				,
	EM_PG_StoC_RoleValueResult				,
	EM_PG_CtoS_RoleItemRequest				,
	EM_PG_StoC_RoleItemResult				,
};


