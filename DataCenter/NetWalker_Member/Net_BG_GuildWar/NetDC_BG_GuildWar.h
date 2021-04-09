
#pragma once
#include "PG/PG_BG_GuildWar.h"
#include "../../MainProc/Global.h"

class CNetDC_BG_GuildWar : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetDC_BG_GuildWar*	This;

	static bool				_Init();
	static bool				_Release();
	//-------------------------------------------------------------------	
	static void	_PG_BG_GuildWar_BLtoD_LoadInfo	( int ServerID , int Size , void* Data );
public:
    
	virtual void RBL_LoadInfo( int ServerID , int ZoneGroupID ) = 0;
		static void SBL_OrderInfo( int ServerID , int ZoneGroupID , vector< GuildHouseWarInfoStruct >& List );
	//-------------------------------------------------------------------	
	static void SBL_RegGuildWarRankInfo( GuildWarRankInfoStruct& Info , deque< GuildHouseWarHistoryStruct > Hisotry  );
};
