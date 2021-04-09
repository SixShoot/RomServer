#pragma once

#include "NetSrv_Bot.h"

class NetSrv_BotChild : public NetSrv_Bot
{
public:
    static bool Init();
    static bool Release();

	virtual void	R_RequestMovePath( BaseItemObject* Obj , float Range , float X , float  Y , float Z );
	
};

