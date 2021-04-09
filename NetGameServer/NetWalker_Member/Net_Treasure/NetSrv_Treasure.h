#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_Treasure.h"

class NetSrv_Treasure : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Treasure* This;
    static bool _Init();
    static bool _Release();

	static void _PG_Treasure_CtoL_GetItem		( int sockid , int size , void* data );
	static void _PG_Treasure_CtoL_Close			( int sockid , int size , void* data );
	static void _PG_Treasure_CtoL_OpenRequest	( int sockid , int size , void* data );
public:    
    
	static void SC_BaseInfo				( int SendID , int ItemGUID , bool IsEmpty );		
	static void SC_Open					( int SendID , int ItemGUID , int MaxCount , ItemFieldStruct Items[30] );		
	static void SC_OpenFailed			( int SendID , int ItemGUID );
	static void SC_GetItemResult		( int SendID , int ItemSerial , int BodyPos , TreasureGetItemResultENUM Result );

	
	virtual void RC_GetItem			( BaseItemObject* Obj , int ItemGUID , int BodyPos , int ItemSerial ) = 0;
	virtual void RC_Close			( BaseItemObject* Obj , int ItemGUID ) = 0;
	virtual void RC_OpenRequest		( BaseItemObject* Obj , int ItemGUID ) = 0;

};

