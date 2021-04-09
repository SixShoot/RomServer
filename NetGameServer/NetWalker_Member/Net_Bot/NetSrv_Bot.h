#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_Bot.h"

class NetSrv_Bot : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Bot* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------
    static void	_PG_Bot_CtoL_RequestMovePath        ( int sockid , int size , void* data );

public:    
	static void S_MovePoint( int SendID , int Count , int Index , float X , float Y , float Z );

	virtual void	R_RequestMovePath( BaseItemObject* Obj , float Range , float X , float  Y , float Z ) = 0;
};

