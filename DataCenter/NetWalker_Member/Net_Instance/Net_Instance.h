

#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_Instance.h"

class Net_Instance : public Global
{

protected:
	//-------------------------------------------------------------------
	static Net_Instance*	This;

	static bool				_Init();
	static bool				_Release();

	//-------------------------------------------------------------------

	static void				_PG_Instance_LtoD_SetWorldVar	( int ServerID , int Size , void* Data );	
	//static void				_PG_Instance_LtoD_GetWorldVar	( int ServerID , int Size , void* Data );
	
	static void				_OnLocalSrvConnectEvent			( EM_SERVER_TYPE ServerType, DWORD SrvID );

	
	
public:
	Net_Instance(void);
	~Net_Instance(void);

	virtual	void			OnZoneSrvConnect	( EM_SERVER_TYPE ServerType, DWORD SrvID ) = 0;
	
	virtual void			RL_SetWorldVar		( int SrvID , const char* pszVarName, int iVarValue ) = 0;
	//virtual void			RL_GetWorldVar		( int SrvID , int iDBID, const char* pszVarName ) = 0;

	//-------------------------------------------------------------------
	static	void			SL_ResetTime		( int ZoneID , WeekDayStruct& ResetDay );
	static	void			SL_PlayInfo			( int ZoneID , int PartyKey , int KeyID , int KeyValue );		
	static	void			SL_WorldVar			( int ZoneID , const char* pszVarName, int iVarValue );
	//-------------------------------------------------------------------
};
