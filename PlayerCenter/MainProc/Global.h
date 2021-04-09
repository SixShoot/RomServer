#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#ifndef CHECK_PLAYER_DATA_HASH_CODE
#define CHECK_PLAYER_DATA_HASH_CODE 0   // 是否要開啟玩家資料的檢查機制(預防有人竄改資料庫資料)
#endif

//----------------------------------------------------------------------------------------------------
//#include "../NetWaker/CliNetWaker.h"
#include <Windows.h>
#include <WinSCard.h>
#include "ReaderClass/DbSqlExecClass.h"
#include "IniFile/IniFile.h"
#include "lua/myvm/lua_vmclass.h"
#include "debuglog/errormessage.h"
#include "DBStruct/DBStructTransfer.h"
#include "BaseItemObject/DC_BaseItemObject.h"
#include "ReaderClass/CreateDBCmdClass.h"
#include "RoleData/RoleDataEx.h"
#include "RoleData/ObjectDataClass.h"
#include "GlobalBase.h"
#include "ControllerClient/ControllerClient.h"
//----------------------------------------------------------------------------------------------------
enum CheckRoleResultENUM
{
	EM_CheckRoleResult_None		,
	EM_CheckRoleResult_OK		,
	EM_CheckRoleResult_Failed	,
};

struct DBRoleTempDataInfo
{
    string  	Account;
    int     	SrvID;
    int     	LocalObjID;
    int     	DBID;
    int     	Field;
	unsigned	IpNum;
	
	int			Job;
	int			Job_Sub;
	int			Lv;
	int			Lv_Sub;

	int			GiftItemID[5];
	int			GiftItemCount[5];

	CheckRoleResultENUM CheckNameResult;

    PlayerRoleData* Role;
    BaseItemObject* Obj;

	PlayerAccountBaseInfoStruct AccountBaseInfo;

	vector<DB_BaseFriendStruct>		FriendList;
	vector<DB_CultivatePetStruct>	PetList;

	void Init()
	{
		Job = -1;
		Job_Sub = -1;
		Lv = 1;
		Lv_Sub = 0;
		SrvID = 0;
		LocalObjID = 0;
		DBID = 0;
		Field = 0;
		IpNum = 0;
		memset( &GiftItemID  , 0 , sizeof( GiftItemID ) );
		memset( &GiftItemCount  , 0 , sizeof( GiftItemCount ) );
		CheckNameResult = EM_CheckRoleResult_None;
	    Role = NULL;
	    Obj = NULL;
		FriendList.clear();
		PetList.clear();
		memset( &AccountBaseInfo , 0 , sizeof( AccountBaseInfo ) ); 
	}
};
//----------------------------------------------------------------------------------------------------
enum PlayerCenterDestroyENUM
{
	EM_PlayerCenterDestroy_None						,	//沒解構
	EM_PlayerCenterDestroy_KeyBoardCmd_Exit			,	//鍵盤退出
	EM_PlayerCenterDestroy_KeyBoardCmd_SaveAllExit	,	//鍵盤退出
	EM_PlayerCenterDestroy_ServerList				,	//ServerList 通知退出

};
//----------------------------------------------------------------------------------------------------
class Global : public GlobalBase
{
protected:
    static RecycleBin< DBRoleTempDataInfo >*    					_ReadRole;
    static RecycleBin< PlayerRoleData >*           					_RoleBin;
    static IniFileClass	                        					_Ini;
    static DBTransferClass*                     					_DBTransfer_Game;   //db傳送資料的物件    
	static DBTransferClass*                     					_DBTransfer_Import; //db傳送資料的物件    
	static DBTransferClass*                     					_DBTransfer_Account; //db傳送資料的物件    
    static DBStructTransferClass< PlayerRoleData >* 				_RoleDataDB;      	//db轉結構的物件
    static DBStructTransferClass< DB_ItemFieldStruct >*				_RD_ItemDB;     	//db轉結構的物件
    static DBStructTransferClass< DB_AbilityStruct >*				_RD_AbilityDB;  	//db轉結構的物件
//    static DBStructTransferClass< DB_BaseFriendStruct >*			_RD_FriendDB;		//db轉結構的物件

	static DBStructTransferClass< int >*							_RD_NormalDB;			//db轉結構的物件(直接使用SQL命令)通用類型
	static DBStructTransferClass< int >*							_RD_ImportDB;			//db轉結構的物件(直接使用SQL命令)通用類型
	static DBStructTransferClass< int >*							_RD_AccountDB;			//db轉結構的物件(直接使用SQL命令)通用類型


	static CreateDBCmdClass<PlayerRoleData>*           		_RoleDataAccountSql;
    static CreateDBCmdClass<PlayerRoleData>*           		_RoleDataSql;
//	static CreateDBCmdClass<RoleDataEx>*           			_RoleDataExSql;
    static CreateDBCmdClass<DB_ItemFieldStruct>*   			_RDItemSql;
    static CreateDBCmdClass<DB_AbilityStruct>*     			_RDAbilitySql;
	static CreateDBCmdClass<DB_BaseFriendStruct>*  			_RDFriendSql;
	static CreateDBCmdClass<DB_CultivatePetStruct>*			_RDCultivatePetSql;

	//////////////////////////////////////////////////////////////////////////
    
    static bool                                 _IsDelay;
    static bool                                 _IsDestory;
	static bool									_WebServic;
	static PlayerCenterDestroyENUM				_DestoryReason;
	static string								_RoleStateStoreProcedure;
	//////////////////////////////////////////////////////////////////////////

    static void   _SaveRoleSQLCmdReusltEventCB ( vector<PlayerRoleData>& ,void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
    static bool   _SaveRoleSQLCmdRequestEvent ( vector<PlayerRoleData>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );

	static bool   _SQLCmdRequestEvent ( vector<PlayerRoleData>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void   _SQLCmdReusltEventCB ( vector<PlayerRoleData>& ,void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool   _SQLCmdGetRoleCountEvent ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void   _SQLCmdGetRoleCountReusltEventCB ( vector<int>& ,void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool   _SQLCmdWriteRoleCountEvent ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void   _SQLCmdWriteRoleCountReusltEventCB ( vector<int>& ,void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

public:
    static bool     Init( char* IniFile );
    static bool     Release( );
    static bool     Process( );
	static void		DBProcess();
	static void		Destory( PlayerCenterDestroyENUM DestoryReason ) { _IsDestory = true;_DestoryReason = DestoryReason; };

	static void		SavePlayerDB( BaseItemObject* );

	static bool		SendToCli_ByRoleName( const char* RoleName , int Size , void* Data );
	static IniFileClass*	Ini() {return &_Ini; };
	static void		UpdateAccountLogout( const char* Account );
	
	static void		UpdateRolePlayerCount( const char* Account );

	static bool		IsDestory() { return _IsDestory; };
	static PlayerCenterDestroyENUM	DestoryReason() { return _DestoryReason;}
	static string&	RoleStateStoreProcedure(){ return _RoleStateStoreProcedure; };
};
// 小輝自定 Define
/*
//------------------------------------------------------
#define M_PACKET( type ) type* pPacket = (type*)pData
*/
//----------------------------------------------------------------------------------------------------
#endif //__GLOBAL_H__