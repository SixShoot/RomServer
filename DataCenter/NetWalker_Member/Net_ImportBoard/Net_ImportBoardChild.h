#pragma once

#include "Net_ImportBoard.h"

//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
class Net_ImportBoardChild : public Net_ImportBoard
{
	
	static vector< ImportBoardBaseStruct >		_ImportBoardInfo;
	static int			_LzoSize;
	static int			_DataCount;
	static char*		_LzoData;
	static float		_LastUpdateTime;
	static float		_SerialNo;

	static bool _SQLCmdOnTimeProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdOnTimeProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//定時處理
	static int _OnTimeProc_( SchedularInfo* Obj , void* InputClass );
public:
	static bool Init();
	static bool Release();


	virtual void RL_ImportBoardRequest( int PlayerDBID , float Update );
	virtual void RL_NewVersionRequest(  int SockID , int ProxyID  );
};
