#pragma once

#include "Net_Talk.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
class Net_TalkChild : public Net_Talk
{
	static bool _SQLCmdOfflineMsgEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdOfflineMsgResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLOnTimeBroadCastingProcEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLOnTimeBroadCastingProcResultEvent( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//定時處理
	static int _OnTimeProc( SchedularInfo* Obj , void* InputClass );
	static bool	IsEnterBroadcastingProc;
public:
	Net_TalkChild(void)		{};
	~Net_TalkChild(void)	{};

	static bool Init();
	static bool Release();

	virtual void RL_OfflineMessageRequest( int ServerID , int PlayerDBID );
};
