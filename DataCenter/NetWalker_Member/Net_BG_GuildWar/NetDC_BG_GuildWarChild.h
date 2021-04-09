#pragma once
#include <boost/shared_ptr.hpp>
#include "NetDC_BG_GuildWar.h"

//--------------------------------------------------------------------------------------------
class CNetDC_BG_GuildWarChild : public CNetDC_BG_GuildWar
{

	//讀取公會戰的資料
	static bool _SQLCmdLoadInfo( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadInfoResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static int _OnTimeProc( SchedularInfo* Obj , void* InputClass );
public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

	virtual void RBL_LoadInfo( int ServerID , int ZoneGroupID );

};
