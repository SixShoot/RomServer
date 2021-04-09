#pragma once

#include "NetDC_Item.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
class Net_ItemChild : public Net_Item
{

	//提領Account Bag 的資料
	static bool _SQLCmdAccountBagProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdAccountBagResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

public:
	static bool Init();
	static bool Release();

	virtual void RL_AccountBagInfoRequest( int ServerID , int PlayerDBID , char* Account );
};
