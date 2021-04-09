#pragma once
#include "../PacketEnum.h"
#include "roledata/RoleBaseDef.h"
//////////////////////////////////////////////////////////////////////////
//要求帳號資料
//////////////////////////////////////////////////////////////////////////
struct PG_CtoS_AccountDataRequest
{
	WebServiceCommandENUM	Command;
	char			Account[_MAX_ACCOUNT_STRING_LEN_];

	PG_CtoS_AccountDataRequest()
	{ Command = EM_PG_CtoS_AccountDataRequest; }
};

//通知帳號有幾個角色
struct PG_StoC_AccountDataResult_Base
{
	WebServiceCommandENUM	Command;
	char			Account[_MAX_ACCOUNT_STRING_LEN_];
	int				RoleCount;

	PG_StoC_AccountDataResult_Base()
	{ Command = EM_PG_StoC_AccountDataResult_Base; }
};

struct PG_StoC_AccountDataResult_RoleName
{
	WebServiceCommandENUM	Command;
	int				DBID;
	char			RoleName[_MAX_NAME_STRING_LEN_];


	PG_StoC_AccountDataResult_RoleName()
	{ Command = EM_PG_StoC_AccountDataResult_RoleName; }
};

//////////////////////////////////////////////////////////////////////////
//要求角色數值資料
//////////////////////////////////////////////////////////////////////////
struct PG_CtoS_RoleValueRequest
{
	WebServiceCommandENUM		Command;
	char				RoleName[_MAX_NAME_STRING_LEN_];
	int					ValueCount;
	RoleValueName_ENUM	ValueType[ 0x1000 ];

	PG_CtoS_RoleValueRequest()
	{ Command = EM_PG_CtoS_RoleValueRequest; }
};

struct RoleValueStruct
{
	RoleValueName_ENUM	Type;
	float				Value;
};

//回應數值資料
struct PG_StoC_RoleValueResult
{
	WebServiceCommandENUM		Command;
	char				RoleName[_MAX_NAME_STRING_LEN_];
	int					ValueCount;
	RoleValueStruct		Value[0x1000];

	PG_StoC_RoleValueResult()
	{ Command = EM_PG_StoC_RoleValueResult; }

	int	PGSize()
	{
		return sizeof(*this) - sizeof(Value ) + ValueCount * sizeof(RoleValueStruct);
	}
};

//////////////////////////////////////////////////////////////////////////
//要求角色物品資料
//////////////////////////////////////////////////////////////////////////
struct PG_CtoS_RoleItemRequest
{
	WebServiceCommandENUM		Command;
	char				RoleName[_MAX_NAME_STRING_LEN_];
	int					FieldType;

	PG_CtoS_RoleItemRequest()
	{ Command = EM_PG_CtoS_RoleItemRequest; }
};

struct PG_StoC_RoleItemResult
{
	WebServiceCommandENUM		Command;
	int					FieldType;
	int					ItemCount;
	ItemFieldStruct		Item[0x1000];

	PG_StoC_RoleItemResult()
	{ Command = EM_PG_StoC_RoleItemResult; }

	int	PGSize()
	{
		return sizeof(*this) - sizeof(Value ) + ItemCount * sizeof(ItemFieldStruct);
	}
};
/*
//////////////////////////////////////////////////////////////////////////
//要求物件數值資料
//////////////////////////////////////////////////////////////////////////
struct PG_CtoS_ObjValueRequest
{
	WebServiceCommandENUM		Command;
	int					ObjID;			//物品樣板ID
	int					ValueCount;
	RoleValueName_ENUM	ValueType[ 0x1000 ];

	PG_CtoS_ObjValueRequest()
	{ Command = EM_PG_CtoS_ObjValueRequest; }
};

struct ObjValueStruct
{
	RoleValueName_ENUM	Type;
	float				Value;
};

//回應數值資料
struct PG_StoC_RoleValueResult
{
	WebServiceCommandENUM		Command;
	char				RoleName[_MAX_NAME_STRING_LEN_];
	int					ValueCount;
	RoleValueStruct		Value[0x1000];

	PG_StoC_RoleValueResult()
	{ Command = EM_PG_StoC_RoleValueResult; }

	int	PGSize()
	{
		return sizeof(*this) - sizeof(Value ) + ValueCount * sizeof(RoleValueStruct);
	}
};
*/