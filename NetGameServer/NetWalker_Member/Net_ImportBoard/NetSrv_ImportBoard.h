#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_ImportBoard.h"

class NetSrv_ImportBoard : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_ImportBoard* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------

	static void _PG_ImportBoard_CtoL_ImportBoardRequest	( int sockid , int size , void* data );

public:    

	static void SD_ImportBoardRequest( int PlayerDBID , float UpdateTime );

	virtual void RC_ImportBoardRequest( BaseItemObject* Obj , float UpdateTime ) = 0;

	static void SD_NewVersionRequest( int SockID , int ProxyID );
};



