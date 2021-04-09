/*
#pragma once

#include "NetDC_GuildHousesWar.h"

//--------------------------------------------------------------------------------------------
class CNetDC_GuildHousesWarChild : public CNetDC_GuildHousesWar
{
	static bool						_IsInitReady;
	//--------------------------------------------------------------------------------------------
	static bool _SQLCmdInit			( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdInitResult	( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdLoadGuildHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadGuildHouseResult	( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdHistory		( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdHistoryResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

	virtual void RL_OpenMenu			( int PlayerDBID );
	virtual void RL_WarRegister			( int PlayerDBID , GuildHouseWarRegisterTypeENUM Type );
	virtual void RL_LoadHouseBaseInfo	( int ServerID );
	virtual void RL_WarEndOK			( int ServerID );
	virtual void RL_DebugTime			( int ServerID , int Time_Day , int Time_Hour );
	virtual void RL_HistoryRequest		( int PlayerDBID , int DayBefore );

	
};
*/