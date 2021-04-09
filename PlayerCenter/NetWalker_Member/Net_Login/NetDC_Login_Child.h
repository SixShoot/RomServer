#pragma once

#include "NetDC_Login.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
class CNetDC_Login_Child : public CNetDC_Login
{
//--------------------------------------------------------------------------------------------
	static int _OnTimeProc_( SchedularInfo* Obj , void* InputClass );

	static int _LoadRoleDataProc( SchedularInfo* Obj , void* InputClass );

	//初始載入
	static bool 	_SQLCmdInitProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void 	_SQLCmdInitProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//初始設定
	static bool 	_SQLCmdOnTimeProc		( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void 	_SQLCmdOnTimeProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

    //login時取角色資料
	static bool     _DBLoadRole           ( vector<PlayerRoleData>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void     _DBLoadRoleResult     ( vector<PlayerRoleData>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

    static void     _DBReadItemDBEventCB    ( vector<DB_ItemFieldStruct>& Data , void* UserObj , char *WhereCmd , bool ResultOK );
    static void     _DBReadAbilityDBEventCB ( vector<DB_AbilityStruct>& Data , void* UserObj , char *WhereCmd , bool ResultOK );

	static bool     _DBLoadOtherDB	           ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void     _DBLoadOtherDBeResult     ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

    static bool     _DBCreateRole           ( vector<PlayerRoleData>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
    static void     _DBCreateRoleResult     ( vector<PlayerRoleData>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

    static void     _DBWriteNewRoleDBEventCB ( RoleDataEx *Data , void* UserObj , char *WhereCmd , bool ResultOK ){};

    static bool     _DBDeleteRole           ( vector<PlayerRoleData>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
    static void     _DBDeleteRoleResult     ( vector<PlayerRoleData>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool     _DBDelRoleRecover           ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void     _DBDelRoleRecoverResult     ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool     _DBSetRolePassword          ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void     _DBSetRolePasswordResult    ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool     _DBChangeRoleName          	( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void     _DBChangeRoleNameResult    	( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool     _DBCheckRoleName          	( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void     _DBCheckRoleNameResult    	( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool     _DBReserveRole				( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void     _DBReserveRoleResult		( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool     _DBSetSPWRecord		          ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void     _DBSetSPWRecordResult		 ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );



	static bool _IsInitRoleData_Acccount_OK;
	static bool	_CheckZeroSerialCode;
	static map< int , DBRoleTempDataInfo >		_WaitLoadRole;
	static set< string >						_OnLoadAccountSet;
public:

	CNetDC_Login_Child(void)	{};
	~CNetDC_Login_Child(void)	{};

	static bool Init();
	static bool Release();

	static set< string >&						OnLoadAccountSet(){ return _OnLoadAccountSet; };
    //-----------------------------------------------------------------------

	//-------------------------------------------------------------------
	//  Virtual Event
	//-------------------------------------------------------------------
	virtual	void		On_LoadAccount					( int iServerID, int iClientID, string sAccount );
	virtual	void		On_CreateRole					( int iServerID, int iClientID, string sAccountName, PlayerRoleData* pRole );
	virtual	void		On_DeleteRole					( int iServerID, int iClientID, string sAccountName, int iDBID, string sRoleName , bool IsImmediately );

	virtual void		On_DelRoleRecoverRequest		( int NetID , int ProxyID , int DBID );
	virtual void		On_SetSecondPassword			( char* Account , char* Password );
	virtual void		RM_ChangeNameRequest			( int iServerID , int CliNetID , int DBID , char* RoleName );

	virtual void		RM_ReserveRoleRequest			( int iServerID , int CliNetID , int CliProxyID , const char* Account_ID );
	virtual void		RM_GetObjCount					( int iServerID );
	virtual void		RM_SetCheckSecondPasswordResult	( int iServerID, int iClientID, const char* szAccountName, int iResult );

	//-----------------------------------------------------------------------
	static void			LoadAccount( DBRoleTempDataInfo* UserData );

#if CHECK_PLAYER_DATA_HASH_CODE
    // login hash record
    static void         AccountIDHashCode(const char* AccountID, std::string& HashCodeReturn);
    static bool         AccountDataHashCode(const char* AccountID, int AccountMoney, std::string& HashCodeReturn);
    static bool         AccountDataHashCodeSave(const char* AccountID, int AccountMoney, SqlBaseClass* sqlBase);
    static bool         AccountDataHashCodeCheck(const char* AccountID, int AccountMoney, SqlBaseClass* sqlBase);
    static void         PlayerDBIDHashCode(const char* AccountID, int PlayerDBID, std::string& HashCodeReturn);
    static bool         PlayerDataHashCode(const char* AccountID, int PlayerDBID, int PlayerMoney, std::string& HashCodeReturn);
    static bool         PlayerDataHashCodeSave(const char* AccountID, int PlayerDBID, int PlayerMoney, SqlBaseClass* sqlBase);
    static bool         PlayerDataHashCodeCheck(const char* AccountID, int PlayerDBID, int PlayerMoney, SqlBaseClass* sqlBase);
#endif

};
