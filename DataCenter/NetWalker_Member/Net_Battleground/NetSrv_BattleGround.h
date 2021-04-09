#pragma once

#include "../../mainproc/Global.h"
#include "PG/PG_BattleGround.h"



class CNetDC_BattleGround : public Global
{
protected:

	static void _PG_BattleGround_LtoD_GetArenaData				( int iCliID , int iSize , PVOID pData );    

public:

    CNetDC_BattleGround(){};
    ~CNetDC_BattleGround(){};

    static bool			Init();
    static bool			Release(){ return true;};
	

	static void			SL_UpdateArenaData					( int iSrvID, int iArenaType, int iTeamGUID, StructArenaInfo ArenaInfo );
	//-----------------------------------------------------------------------------
	static CNetDC_BattleGround*		m_pThis;
};
