#pragma once

#include "NetDC_Other.h"

//--------------------------------------------------------------------------------------------
class CNetDC_OtherChild : public CNetDC_Other
{
	
	static bool _SQLCmdFind_DBD_RoleName_Event( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdFind_DBD_RoleName_EventResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//初始載入
	static bool _SQLCmdInitProc_Global( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdInitProcResult_Global ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//初始載入
	static bool _SQLCmdInitProc_Import( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdInitProcResult_Import ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//清除ObjNameStringDB
	static bool _SQLCmdInitProc_World( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdInitProcResult_World( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdLoadStringDB( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadStringDBResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdSqlSelectCMD( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdSqlSelectCMDResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	
	static bool _SQLCmdLoadClientDataCMD( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadClientDataResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdLoadClientDataCMD_Account( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadClientDataResult_Account( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdOpenSession( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdOpenSessionResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdCloseSession( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdCloseSessionResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLOnTimeBuffScheduleProcEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLOnTimeBuffScheduleProcEventResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static int _OntimeProc( SchedularInfo* Obj , void* InputClass );

//	static bool										_IsLoadMoneyExchangeTabledOK;
	static vector< MoneyExchangeKeyValueStruct >	_MoneyExchangeKeyValueList;
	static int										_MoneyExchangeKeyValueZipSize;
	static char										_MoneyExchangeKeyValueZip[0x10000];

	static bool										_IsLoadStringDBTabledOK;
	static vector < SysKeyStrChangeStruct >			_StringDBList;
	static int										_StringDBZipSize;
	static char										_StringDBZip[0x10000];

	static map< string , int >						_SysKeyValue;

	static MyLzoClass								_MyLzoZip;

	static vector<WeekZoneStruct>					_WeekInst;
	static vector< BuffScheduleStruct >				_BuffSchList;
public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

	virtual void OnLocalSrvConnect( int SrvID );
	virtual void RL_Find_DBID_RoleName( int CliDBID , int DBID , const char* RoleName );
	//virtual void RL_SendPerformance( int CliDBID, const char* cMac, float MaxCPUUsage, float MaxMemUsage, float MaxFPS, float AverageCPUUsage, float AverageMemUsage, float AverageFPS, float MinCPUUsage, float MinMemUsage, float MinFPS );
	
	virtual void RL_SelectDB( int PlayerDBID , const char* DataType , const char* SelectCmd );

	static void ReloadStringDB();

	virtual void RL_LoadClientData( int PlayerDBID , int KeyID );
	virtual void RM_LoadClientData_Account( int SockID , int ProxyID , const char* Account , int KeyID );

	static void	SendSysDBKeyValue( int SrvID );

	virtual void RL_OpenSession	( const char* Account, int RoleDBID, int ZoneID, SessionTypeENUM Type, int SessionID, int ValidTime );
	virtual void RL_CloseSession( const char* Account, int RoleDBID, int ZoneID, SessionTypeENUM Type, int SessionID );
};
