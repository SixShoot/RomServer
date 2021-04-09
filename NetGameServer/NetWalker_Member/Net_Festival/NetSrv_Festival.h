#pragma once
#include "PG/PG_Festival.h"
#include "../../MainProc/Global.h"

class CNetSrv_Festival : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetSrv_Festival*	This;

	static void _OnCliConnect( int ID , string Account );

protected:
	static bool	_Init();
	static bool	_Release();

	static void _PG_Festival_DtoL_AllFestivalInfo( int Sockid , int Size , void* Data );

public:
	virtual void LoadFestivalInfo( void* Data ) = 0;
	virtual void CliConnect( int CliID , string Account) = 0;
};