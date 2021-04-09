#pragma once

#include "NetDC_DCBase.h"
//--------------------------------------------------------------------------------------------
class CNetDC_DCBaseChild : public CNetDC_DCBase
{

    
    static int  _OnTimeProc( SchedularInfo* Obj , void* InputClass );
	static bool	_IsStart;
public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

    virtual void SavePlayer                 ( int SrvNetID , int SaveTime , int DBID , int Start , int Size , void* Data );
    virtual void SetPlayerLiveTime          ( int SrvNetID , int DBID , int LiveTime );
    virtual void SetPlayerLiveTime_ByAccount( int SrvNetID , char* Account , int LiveTime );

	virtual void R_CheckRoleDataSize		( int SrvNetID , int Size );
	virtual void OnDataCenterConnect( );
	virtual void OnServersConnect( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
//--------------------------------------------------------------------------------------------
};
