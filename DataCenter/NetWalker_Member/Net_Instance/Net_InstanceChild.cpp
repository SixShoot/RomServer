#include "Net_InstanceChild.h"	
//-------------------------------------------------------------------
bool Net_InstanceChild::_IsInitReady = false;

bool    Net_InstanceChild::Init()
{
	Net_Instance::_Init();

	if( This != NULL)
		return false;

	This = NEW( Net_InstanceChild );

	((Net_InstanceChild*)This)->m_worldVarReady	= false;

	map< string, int >*			pmapVar = new map< string, int >;

	_RD_NormalDB->SqlCmd( 0 , _DBGetWorldVar , _DBGetWorldVarResult , (PVOID)pmapVar	, NULL );

	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 1000 );
		_RD_NormalDB->Process( );

		if( _IsInitReady == true )
			break;
	}

	return true;
}
//--------------------------------------------------------------------------------------
bool    Net_InstanceChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	Net_Instance::_Release();
	return true;

}
//--------------------------------------------------------------------------------------
void Net_InstanceChild::RL_SetWorldVar	( int SrvID , const char* pszVarName, int iVarValue )
{

	// 更新 MapCache 資料, 並發送給所有的 Server
	m_mapVar[ pszVarName ] = iVarValue;

	// Send to all server
	SL_WorldVar( 0, pszVarName, iVarValue  );

	// 將資料寫進 DB
	_RD_NormalDB->SqlCmd( 0, _DBSetWorldVar , _DBSetWorldVarResult , this 
		, "VarName"		, EM_ValueType_String   , pszVarName
		, "VarValue"	, EM_ValueType_int		, iVarValue
		, NULL );
}
//--------------------------------------------------------------------------------------
bool Net_InstanceChild::_DBSetWorldVar( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char*   VarName      = Arg.GetString( "VarName" );
	int		VarValue	 = Arg.GetNumber( "VarValue"	);

	char	CmdBuf[512];

	MyDbSqlExecClass		MyDBProc( sqlBase );

	sprintf( CmdBuf , "IF( Select count( VarName ) From WorldVar where  VarName = '%s' ) = 0 Insert WorldVar(VarName) Values( '%s' ) ELSE UPDATE WorldVar SET VarValue = %d WHERE VarName = '%s'", VarName, VarName, VarValue, VarName );	

	int							iVal		= 0;



	sprintf( CmdBuf , "Select count( VarName ) From WorldVar where  VarName = '%s'", VarName );


	MyDBProc.SqlCmd( CmdBuf );	

	MyDBProc.Bind( 1, SQL_C_LONG,	SQL_C_DEFAULT,			(LPBYTE)&iVal );
	MyDBProc.Read();
	MyDBProc.Close();

	if( iVal == 0 )
	{
		sprintf( CmdBuf , "INSERT INTO WorldVar(VarName,VarValue) Values( '%s', %d )", VarName, VarValue );

		if( MyDBProc.SqlCmd_WriteOneLine( CmdBuf ) == false )
			return false;
	}
	else
	{
		sprintf( CmdBuf , "UPDATE WorldVar SET VarValue = %d WHERE VarName = '%s'", VarValue, VarName  );

		if( MyDBProc.SqlCmd_WriteOneLine( CmdBuf ) == false )
			return false;
	}



	//if( MyDBProc.SqlCmd_WriteOneLine( CmdBuf ) == false )
	//	return false;

	return true;
}
//--------------------------------------------------------------------------------------
void Net_InstanceChild::_DBSetWorldVarResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{

}
//--------------------------------------------------------------------------------------
/*
void Net_InstanceChild::RL_GetWorldVar( int SrvID , int iDBID, const char* pszVarName )
{
	map< string, int >::iterator it = m_mapVar.find( pszVarName );

	if( it != m_mapVar.end() )
	{
		
	}

	

	m_mapReadingVar.insert( make_pair( pszVarName, iDBID ) );

	if( m_mapReadingVar.count( pszVarName ) == 1 )
	{
		
		// 將資料寫進 DB
		_RD_NormalDB->SqlCmd( 0, _DBGetWorldVar , _DBGetWorldVarResult , this 
			, "DBID"		, EM_ValueType_int			 , iDBID
			, "VarName"		, EM_ValueType_String		, pszVarName
			, NULL );
	}
}
*/
//--------------------------------------------------------------------------------------
bool Net_InstanceChild::_DBGetWorldVar( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	int							iVal		= 0;
	char						szVarName[512];

	map< string, int >*			pmapVar =  (map< string, int >*) UserObj;

	//_RD_NormalDB->SqlCmd( 0 , _DBGetWorldVar , _DBGetWorldVarResult , (PVOID)pmapVar	, NULL );

	char	CmdBuf[512];

	MyDbSqlExecClass		MyDBProc( sqlBase );

	sprintf( CmdBuf , "SELECT VarName,VarValue FROM WorldVar" );

	

	MyDBProc.SqlCmd( CmdBuf );	

	
	MyDBProc.Bind( 1, SQL_C_CHAR,	sizeof( szVarName ),	(LPBYTE)&szVarName );
	MyDBProc.Bind( 2, SQL_C_LONG,	SQL_C_DEFAULT,			(LPBYTE)&iVal );

	while( MyDBProc.Read() )
	{
		pmapVar->insert( make_pair( szVarName, iVal ) );
	}

	MyDBProc.Close();

	return true;
}
//--------------------------------------------------------------------------------------
void Net_InstanceChild::_DBGetWorldVarResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	map< string, int >*	pMapVar				= ((map< string, int >*)UserObj);
	
	((Net_InstanceChild*)This)->m_mapVar	= *pMapVar;
	_IsInitReady							= true;

	((Net_InstanceChild*)This)->m_worldVarReady							= true;

	for( set< UINT >::iterator its = ((Net_InstanceChild*)This)->m_setServerRequireWorldVar.begin(); its != ((Net_InstanceChild*)This)->m_setServerRequireWorldVar.end(); its++ )
	{
		for( map< string, int >::iterator it = ((Net_InstanceChild*)This)->m_mapVar.begin(); it != ((Net_InstanceChild*)This)->m_mapVar.end(); it++ )
		{
			((Net_InstanceChild*)This)->SL_WorldVar( (*its), it->first.c_str(), it->second );
		}
	}

	((Net_InstanceChild*)This)->m_setServerRequireWorldVar.clear();


	delete pMapVar;
}
//--------------------------------------------------------------------------------------
void Net_InstanceChild::OnZoneSrvConnect( EM_SERVER_TYPE ServerType, DWORD SrvID )
{
	if( m_worldVarReady == true )
	{
		for( map< string, int >::iterator it = m_mapVar.begin(); it != m_mapVar.end(); it++ )
		{
			SL_WorldVar( SrvID, it->first.c_str(), it->second );
		}
	}
	else
	{
		m_setServerRequireWorldVar.insert( SrvID );
	}
}