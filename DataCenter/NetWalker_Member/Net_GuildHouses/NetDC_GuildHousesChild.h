#pragma once

#include "NetDC_GuildHouses.h"



//--------------------------------------------------------------------------------------------
class CNetDC_GuildHousesChild : public CNetDC_GuildHouses
{

	//--------------------------------------------------------------------------------------------
	//建立房屋
	static bool _SQLCmdBuildHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdBuildHouseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	//--------------------------------------------------------------------------------------------
	//搜尋房子
	static bool _SQLCmdFindHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdFindHouseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	//--------------------------------------------------------------------------------------------
	//載入房屋資料
	static bool _SQLCmdLoadHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadHouseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	//--------------------------------------------------------------------------------------------


public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

	virtual void RL_BuildHouseRequest	( int ServerID , int PlayerDBID , int GuildID  );	
	virtual void RL_FindHouseRequest	( int ServerID , int PlayerDBID , int GuildID );	
	virtual void RL_HouseInfoLoading	( int ServerID , int GuildID);	

};
