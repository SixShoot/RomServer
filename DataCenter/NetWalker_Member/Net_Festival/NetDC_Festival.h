#pragma once
#include "PG/PG_Festival.h"
#include "../../MainProc/Global.h"
#include <vector>

class CNetDC_Festival : public Global
{
public:
	static void _PG_Festival_LtoD_RequestFestivalInfo( int ServerID , int Size , void* Data );

protected:
	//-------------------------------------------------------------------
	static CNetDC_Festival*	This;

	static bool				_Init();
	static bool				_Release();

public:
	virtual void RL_RequestFestivalInfo( int iZoneID ) = 0;
};
