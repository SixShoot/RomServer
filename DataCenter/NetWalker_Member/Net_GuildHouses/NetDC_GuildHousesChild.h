#pragma once

#include "NetDC_GuildHouses.h"



//--------------------------------------------------------------------------------------------
class CNetDC_GuildHousesChild : public CNetDC_GuildHouses
{

	//--------------------------------------------------------------------------------------------
	//�إߩЫ�
	static bool _SQLCmdBuildHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdBuildHouseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	//--------------------------------------------------------------------------------------------
	//�j�M�Фl
	static bool _SQLCmdFindHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdFindHouseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	//--------------------------------------------------------------------------------------------
	//���J�Ыθ��
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
