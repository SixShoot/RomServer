#pragma once

#include "NetSrv_ImportBoard.h"

class NetSrv_ImportBoardChild : public NetSrv_ImportBoard
{
public:
    static bool Init();
    static bool Release();

	virtual void RC_ImportBoardRequest( BaseItemObject* Obj , float UpdateTime );
};
