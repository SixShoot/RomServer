#pragma once

#include ".\NetSrv_Gather.h"	

class CNetSrv_Gather_Child : public CNetSrv_Gather
{
public:
	CNetSrv_Gather_Child(void)	{};
	~CNetSrv_Gather_Child(void)	{};

	static bool			Init();
	static bool			Release();

	static int			OnEvent_GatherCast			( SchedularInfo* Obj, void* InputClass );
	static int			OnEvent_MineCheck			( SchedularInfo* Obj, void* InputClass );

	static bool			SetCast						( RoleDataEx *pRole, int iClickObjID, int iTime, GatherCallBackFunc pFunc, const char* pszLuaEvent );
	static bool			SetCastFailed				( RoleDataEx *pRole );
	static void			OnCast_GatherObj			( int iGUID, int iResult );
	//static int			OnEvent_CheckCasting	( SchedularInfo* Obj, void* InputClass );

	virtual void		On_ClickGatherObj			( BaseItemObject*	pObj, int iClickObjID );
	virtual	void		OnCancelGather				( BaseItemObject*	pObj, int iClickObjID );

	static void			PlayEndMotion				( int iGUID, GameObjDbStructEx*	pClickObjDB	);

	//-----------------------------------------------------------------------------------
	static	void		SetBaseCount				( RoleDataEx *pMine, int iBaseCount, int iReduceVal );
	
	static	void		ResetMineObj				( BaseItemObject* pObj );	
	static	void		GenerateMine				( int iRoomID, int iFlagID, int iMineID,	int iMineCount );

	//static GatherCallBackFunc	m_pGatherCastFunc;

	map< int, vector< int > >		m_mapMineObj;				// FlagID, ObjGUID
//	map< int, vector< int > >		m_mapEmptyMineFlag;				// FlagID, MineCount
};
