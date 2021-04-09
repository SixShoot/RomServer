#include "NetSrv_Festival.h"

CNetSrv_Festival*	CNetSrv_Festival::This = NULL;

bool CNetSrv_Festival::_Init()
{
	_Net->RegOnCliConnectFunction( _OnCliConnect );

	_Net->RegOnSrvPacketFunction( EM_PG_Festival_DtoL_FestivalInfo, _PG_Festival_DtoL_AllFestivalInfo );
	return true;
}

bool CNetSrv_Festival::_Release()
{
	return true;
}

void CNetSrv_Festival::_OnCliConnect( int ID , string Account )
{
	This->CliConnect( ID , (char*)Account.c_str() );
}

void CNetSrv_Festival::_PG_Festival_DtoL_AllFestivalInfo( int Sockid , int Size , void* Data )
{
	This->LoadFestivalInfo( Data );
}