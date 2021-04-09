

#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_Talk.h"

class Net_Talk : public Global
{
public:
	Net_Talk(void);
	~Net_Talk(void);

	static void _PG_Talk_LtoD_OfflineMessageRequest( int ServerID , int Size , void* Data );
protected:
	//-------------------------------------------------------------------
	static Net_Talk*		This;

	static bool				_Init();
	static bool				_Release();


public:
	static void GameMsgEvent		( int SockID , int ProxyID , GameMessageEventENUM Event );
	static void SC_OfflineMessage	( int PlayerDBID , int FromDBID , float Time , const char* FromName , const char* ToName , const char* Content );
	static void SL_OfflineGMCommand	( int ServerID , int PlayerDBID , int ManageLv , const char* Content );
	static void SC_ALL_RunningMsg	( const char* Message );
	static void SysMsg				( int PlayerDBID  , const char* Content );
	
	virtual void RL_OfflineMessageRequest( int ServerID , int PlayerDBID ) = 0;


};
