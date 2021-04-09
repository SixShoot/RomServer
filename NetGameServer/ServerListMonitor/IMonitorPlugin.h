#pragma once
#using <mscorlib.dll>
#include <vcclr.h>

#ifdef DLL_EXPORTS_GAMESERVERMONITOR
	#define DLLAPI_MONITOR extern "C" __declspec(dllexport)
#elif  DLL_IMPORTS_GAMESERVERMONITOR
	#define DLLAPI_MONITOR __declspec(dllimport)
#else 
	#define DLLAPI_MONITOR
#endif

using namespace System::Windows::Forms;
//////////////////////////////////////////////////////////////////////////
// Server 端送訊息給 Client 端 or 取資料( Monitor -> dll )
class IMonitorDllCmd
{
public:
	virtual gcroot<Form^> GetForm() = 0;
};
//////////////////////////////////////////////////////////////////////////
// client 端送訊息給 Server 端( dll -> Monitor )
class IMonitorDllNotify
{	
public:
	virtual void	Begin( ) = 0;
	virtual void	Close( ) = 0;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//初始化
DLLAPI_MONITOR IMonitorDllCmd*  DllCreateSrvMonitorPluginFunc( char* IniFile , IMonitorDllNotify* FromNotify );
//結束釋放資源
DLLAPI_MONITOR void  DllDestorySrvMonitorPluginFunc( );


typedef IMonitorDllCmd* (*CreateSrvMonitorPluginFunc)( char* IniFile , IMonitorDllNotify* FromNotify );
typedef void (*DestroySrvMonitorPluginFunc)( void );
//////////////////////////////////////////////////////////////////////////