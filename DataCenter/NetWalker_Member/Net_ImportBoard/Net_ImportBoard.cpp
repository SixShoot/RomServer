#include "Net_ImportBoard.h"	
Net_ImportBoard*	Net_ImportBoard::This		= NULL;

//-------------------------------------------------------------------
Net_ImportBoard::Net_ImportBoard(void)
{
}
//-------------------------------------------------------------------
Net_ImportBoard::~Net_ImportBoard(void)
{
}
//-------------------------------------------------------------------
bool Net_ImportBoard::_Release()
{

	return true;
}
//-------------------------------------------------------------------
bool Net_ImportBoard::_Init()
{
	_Net->RegOnSrvPacketFunction( EM_PG_ImportBoard_LtoD_ImportBoardRequest	, _PG_ImportBoard_LtoD_ImportBoardRequest	);
	_Net->RegOnSrvPacketFunction( EM_PG_ImportBoard_LtoD_NewVersionRequest	, _PG_ImportBoard_LtoD_NewVersionRequest	);
	return true;
}

//------------------------------------------------------
void Net_ImportBoard::_PG_ImportBoard_LtoD_NewVersionRequest( int ServerID , int Size , void* Data )
{
	PG_ImportBoard_LtoD_NewVersionRequest* PG = (PG_ImportBoard_LtoD_NewVersionRequest*)Data;

	This->RL_NewVersionRequest( PG->SockID , PG->ProxyID );
}
//------------------------------------------------------
void Net_ImportBoard::_PG_ImportBoard_LtoD_ImportBoardRequest( int ServerID , int Size , void* Data )
{
	PG_ImportBoard_LtoD_ImportBoardRequest* PG = (PG_ImportBoard_LtoD_ImportBoardRequest*)Data;

	This->RL_ImportBoardRequest( PG->PlayerDBID , PG->UpdateTime );
}
//------------------------------------------------------
void Net_ImportBoard::SC_All_ImportBoardNewVersion( float UpdateTime )
{
	PG_ImportBoard_DtoC_ImportBoardNewVersion Send;
	Send.UpdateTime = UpdateTime;
	Global::SendToAllPlayer( sizeof( Send) , &Send );
}
//------------------------------------------------------
void Net_ImportBoard::SC_ImportBoardInfo( int PlayerDBID , int DataCount , float UpdateTime , int Size , void* Data )
{
	PG_ImportBoard_DtoC_ImportBoardInfo Send;
	Send.UpdateTime = UpdateTime;
	Send.DataSize = Size;
	Send.DataCount = DataCount;
	if( Size > 0 && Data != NULL )
		memcpy( Send.Data , Data , Size );

	Global::SendToCli_ByDBID( PlayerDBID , Send.PGSize() , &Send );
}
//------------------------------------------------------
void Net_ImportBoard::SC_ImportBoardNewVersion( int SockID , int ProxyID , float UpdateTime )
{
	PG_ImportBoard_DtoC_ImportBoardNewVersion Send;
	Send.UpdateTime = UpdateTime;
	//Global::SendToAllPlayer( sizeof( Send) , &Send );
	Global::SendToCli_ByProxyID( SockID , ProxyID , sizeof( Send) , &Send );
}