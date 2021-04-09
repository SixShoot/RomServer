

#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_ImportBoard.h"

class Net_ImportBoard : public Global
{

protected:
	//-------------------------------------------------------------------
	static Net_ImportBoard*		This;

	static bool				_Init();
	static bool				_Release();

	static void _PG_ImportBoard_LtoD_ImportBoardRequest( int ServerID , int Size , void* Data );
	static void _PG_ImportBoard_LtoD_NewVersionRequest( int ServerID , int Size , void* Data );
public:
	Net_ImportBoard(void);
	~Net_ImportBoard(void);

	static void SC_All_ImportBoardNewVersion( float UpdateTime );
	static void SC_ImportBoardNewVersion( int SockID , int ProxyID , float UpdateTime );
	static void SC_ImportBoardInfo( int PlayerDBID , int DataCount , float UpdateTime , int Size , void* Data );

	virtual void RL_ImportBoardRequest( int PlayerDBID , float Update ) = 0;
	virtual void RL_NewVersionRequest(  int SockID , int ProxyID  ) = 0;
};
