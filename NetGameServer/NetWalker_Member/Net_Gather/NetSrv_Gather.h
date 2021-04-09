#pragma once

#include "../../mainproc/Global.h"
#include "PG/PG_Gather.h"

typedef	void		(*GatherCallBackFunc)( int iRoleID, int iResult );

class CNetSrv_Gather : public Global
{
protected:

	static void _PG_Gather_CtoL_ClickGatherObj		( int iCliID , int iSize , PVOID pData );    
	static void _PG_Gather_CtoL_CancelGather		( int iCliID , int iSize , PVOID pData );    

public:

    CNetSrv_Gather(){};
    ~CNetSrv_Gather(){};

    static bool			Init();
    static bool			Release(){ return true;};

	// 事件
	//-------------------------------------------------------------------------------------
	virtual void		On_ClickGatherObj			( BaseItemObject*	pObj, int iClickObjID ) = 0;
	virtual	void		OnCancelGather				( BaseItemObject*	pObj, int iClickObjID ) = 0;

	static	void		ClickGatherObjResult		( int iRoleID, int iResult );
	static	void		GatherMotion				( int iRoleID, int iMotionID, int iMineType );
	
	// 資料
	//-------------------------------------------------------------------------------------
	static CNetSrv_Gather*		m_pThis;

};
