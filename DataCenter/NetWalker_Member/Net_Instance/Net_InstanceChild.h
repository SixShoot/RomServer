#pragma once

#include "Net_Instance.h"

//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
class Net_InstanceChild : public Net_Instance
{

public:
	static	bool			Init();
	static	bool			Release();
	//--------------------------------------------------------------------------------------------
	static	bool			_DBSetWorldVar		        ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static	void			_DBSetWorldVarResult		( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static	bool			_DBGetWorldVar		        ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static	void			_DBGetWorldVarResult		( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	//--------------------------------------------------------------------------------------------
	virtual void			RL_SetWorldVar				( int SrvID , const char* pszVarName, int iVarValue );
	//virtual void			RL_GetWorldVar				( int SrvID , int iDBID, const char* pszVarName );

	virtual	void			OnZoneSrvConnect	( EM_SERVER_TYPE ServerType, DWORD SrvID );
	

	// for world var system
	map< string, int >			m_mapVar;
	map< string, int >			m_mapModifiedVar;	
	multimap< string, int >		m_mapReadingVar;
	
	set< UINT >					m_setServerRequireWorldVar;
	bool						m_worldVarReady;

	static bool					_IsInitReady;
};
