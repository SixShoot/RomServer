#include "PG/PG_BG_GuildWar.h"
#include "NetDC_BG_GuildWar.h"
//-------------------------------------------------------------------
CNetDC_BG_GuildWar*	CNetDC_BG_GuildWar::This = NULL;
//-------------------------------------------------------------------
bool CNetDC_BG_GuildWar::_Release()
{
	return false;
}
//-------------------------------------------------------------------
bool CNetDC_BG_GuildWar::_Init()
{
	_Net->RegOnSrvPacketFunction			( EM_PG_BG_GuildWar_BLtoD_LoadInfo		, _PG_BG_GuildWar_BLtoD_LoadInfo		);
	return true;
}
//-------------------------------------------------------------------
void	CNetDC_BG_GuildWar::_PG_BG_GuildWar_BLtoD_LoadInfo	( int ServerID , int Size , void* Data )
{
	PG_BG_GuildWar_BLtoD_LoadInfo* pg =(PG_BG_GuildWar_BLtoD_LoadInfo*)Data;
	This->RBL_LoadInfo( ServerID , pg->ZoneGroupID );
}

void CNetDC_BG_GuildWar::SBL_OrderInfo( int ServerID , int ZoneGroupID , vector< GuildHouseWarInfoStruct >& List )
{
	if( List.size() > 500 )
		return;

	PG_BG_GuildWar_DtoBL_OrderInfo Send;
	Send.Count = List.size();

	for( int i = 0 ; i < Send.Count ; i++ )
	{
		Send.List[i] = List[i];
	}
	SendToSrvBySockID( ServerID , sizeof(Send) , &Send );
}

void CNetDC_BG_GuildWar::SBL_RegGuildWarRankInfo( GuildWarRankInfoStruct& Info , deque< GuildHouseWarHistoryStruct > Hisotry )
{
	PG_BG_GuildWar_DtoBL_RegGuildWarRankInfo Send;
	Send.Info = Info;
	for( unsigned i = 0 ; i < Hisotry.size() ; i++ )
	{
		if( i >= 50 )
			break;
		Send.Hisotry[i] = Hisotry[i];
		Send.Hisotry[i].GuildID += 0x1000000 * GetWorldID();
	}
	Global::SendToOtherWorld_GSrvID( _Def_BattleGround_WorldID_ , _Def_BattleGround_GuildWar_ZoneID_ , sizeof( Send ) , &Send );
}