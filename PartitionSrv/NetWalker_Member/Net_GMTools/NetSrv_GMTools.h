#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_GMTools.h"

class NetSrv_GMTools : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_GMTools* This;
    static bool _Init();
    static bool _Release();		

	static void	_PG_GM_GMtoX_ServerStateRequest	( int sockid , int size , void* data );
public:    
	//---------------------------------------------------------------------------------------------
	static void S_ServerStateResult( int NetID , ServerStateRequestTypeENUM	RequestType , const char* Name , const char* Type , const char* State 
		, const char* Content1 , const char* Content2
		, const char* Content3 , const char* Content4
		, const char* Content5 , const char* Content6
		, const char* Content7 , const char* Content8 );
	//---------------------------------------------------------------------------------------------
	virtual void	R_ServerStateRequest( int NetID , ServerStateRequestTypeENUM	RequestType ) = 0;
};

