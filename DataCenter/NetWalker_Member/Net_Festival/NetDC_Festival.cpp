#include "NetDC_Festival.h"
//-------------------------------------------------------------------
CNetDC_Festival*	CNetDC_Festival::This = NULL;

bool CNetDC_Festival::_Init()
{
	_Net->RegOnSrvPacketFunction			( EM_PG_Festival_LtoD_RequestFestivalInfo	, _PG_Festival_LtoD_RequestFestivalInfo		);	
	return true;
}

bool CNetDC_Festival::_Release()
{
	return true;
}

void CNetDC_Festival::_PG_Festival_LtoD_RequestFestivalInfo( int ServerID , int Size , void* Data )
{
	PG_Festival_LtoD_RequestFestivalInfo* pg =(PG_Festival_LtoD_RequestFestivalInfo*)Data;

	This->RL_RequestFestivalInfo( pg->iZoneID );
}