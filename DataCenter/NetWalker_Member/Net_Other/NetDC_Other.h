
#pragma once
#include "PG/PG_Other.h"
#include "../../MainProc/Global.h"

class CNetDC_Other : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetDC_Other*	This;

	static bool				_Init();
	static bool				_Release();
	
	static void _OnLocalSrvConnectEvent ( EM_SERVER_TYPE ServerType, DWORD SrvID );

	static void _PG_Other_LtoD_Find_DBID_RoleName	( int ServerID , int Size , void* Data );	
	//static void _PG_Other_LtoD_Performance				( int ServerID , int Size , void* Data );	
	static void _PG_Other_LtoD_SelectDB				( int ServerID , int Size , void* Data );	

	static void _PG_Other_LtoD_LoadClientData			( int ServerID , int Size , void* Data );	
	static void _PG_Other_MtoD_LoadClientData_Account	( int ServerID , int Size , void* Data );	

	static void _PG_Other_LtoD_OpenSession	( int ServerID , int Size , void* Data );
	static void _PG_Other_LtoD_CloseSession	( int ServerID , int Size , void* Data );

public:

	virtual void OnLocalSrvConnect( int SrvID ) = 0;
	//////////////////////////////////////////////////////////////////////////
	//名稱 DBID 查尋
	virtual void RL_Find_DBID_RoleName( int CliDBID , int DBID , const char* RoleName ) = 0;
	//virtual void RL_SendPerformance( int CliDBID , const char* cMac, float MaxCPUUsage, float MaxMemUsage, float MaxFPS, float AverageCPUUsage, float AverageMemUsage, float AverageFPS, float MinCPUUsage, float MinMemUsage, float MinFPS ) = 0;
	static void SC_Find_DBID_RoleName( int SendDBID , int DBID , const char* RoleName , bool IsFind );

	//////////////////////////////////////////////////////////////////////////
	static void SL_ExchangeMoneyTable( int ZoneID , int Count ,  int DataSize , const char* Data );
	static void SL_DBStringTable( int ZoneID , int Count ,  int DataSize , const char* Data );
	static void SL_All_DBStringTable( int Count ,  int DataSize , const char* Data );

	//////////////////////////////////////////////////////////////////////////
	virtual void RL_SelectDB( int PlayerDBID , const char* DataType , const char* SelectCmd ) = 0;
//	static void SendAllLocal_ExchangeMoneyTable(  int Count , int DataSize , const char* Data );
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_LoadClientData( int PlayerDBID , int KeyID ) = 0;
	static void SC_LoadClientDataResult( int PlayerDBID , int KeyID , int Size , const char* Data );

	virtual void RM_LoadClientData_Account( int SockID , int ProxyID , const char* Account , int KeyID ) = 0;
	static void SC_LoadClientDataResult_Account( int SockID , int ProxyID , int KeyID , int Size , const char* Data );
	//////////////////////////////////////////////////////////////////////////
	static void SL_SysKeyValue( int SrvID , int TotalCount , int ID , const char* KeyStr , int Value );
	//////////////////////////////////////////////////////////////////////////
	static void SL_RoleRunPlot(int ZoneID, const char* LuaPlot, int DBID);

	//////////////////////////////////////////////////////////////////////////
	virtual void RL_OpenSession	( const char* Account, int RoleDBID, int ZoneID, SessionTypeENUM Type, int SessionID, int ValidTime ) = 0;
	virtual void RL_CloseSession( const char* Account, int RoleDBID, int ZoneID, SessionTypeENUM Type, int SessionID ) = 0;

	//////////////////////////////////////////////////////////////////////////
	//今天的複本
	static void SL_WeekInstances( int SrvID , vector<WeekZoneStruct>& zoneGroupIDList );

	//後台Buff設定
	static void SL_BuffSchedule( int zoneID , int buffID , int buffLv , int buffTime );

	//後台魔幻寶盒資料
	void SL_ItemCombinInfo( int SrvID , int groupID , ItemCombineStruct& itemCombine );
	void SL_AllItemCombinInfo( int SrvID );

};
