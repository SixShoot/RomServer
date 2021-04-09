#pragma once
#include "NetSrv_Treasure.h"

class NetSrv_TreasureChild : public NetSrv_Treasure
{

public:
    static bool Init();
    static bool Release();

	static bool _CheckTreasureEmpty( RoleDataEx* Owner , RoleDataEx* Treasure );

	static void SendRange_TreasureLootInfo( RoleDataEx* Treasure );
	static void Send_TreasureLootInfo( RoleDataEx* Owner , RoleDataEx* Treasure );

	virtual void RC_OpenRequest		( BaseItemObject* Obj , int ItemGUID );
	virtual void RC_GetItem	( BaseItemObject* Obj , int ItemGUID , int BodyPos , int ItemSerial );
	virtual void RC_Close	( BaseItemObject* Obj , int ItemGUID );

};

