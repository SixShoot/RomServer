

#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_Item.h"

class Net_Item : public Global
{
protected:
	//-------------------------------------------------------------------
	static Net_Item*		This;

	static bool				_Init();
	static bool				_Release();

	static void _PG_Item_LtoD_AccountBagInfoRequest	( int ServerID , int Size , void* Data );
public:
	static void SL_AccountBagInfoResult( int SrvSockID , int PlayerDBID , int MaxCount , int ItemDBID[20] , ItemFieldStruct Item[20] );
	virtual void RL_AccountBagInfoRequest( int ServerID , int PlayerDBID , char* Account ) = 0;
};
