#pragma once

#include ".\NetSrv_BattleGround.h"	


class CNetDC_BattleGround_Child : public CNetDC_BattleGround
{
public:
	CNetDC_BattleGround_Child(void)	{};
	~CNetDC_BattleGround_Child(void)	{};

	static bool			Init();
	static bool			Release();

	virtual void		OnRZ_GetArenaData			( int iSrvID, int iArenaType, int iTeamGUID );


};
