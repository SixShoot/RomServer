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
// Server �ݰe�T���� Client �� or �����( Monitor -> dll )
class IMonitorDllCmd
{
public:
	virtual gcroot<Form^> GetForm() = 0;
};
//////////////////////////////////////////////////////////////////////////
// client �ݰe�T���� Server ��( dll -> Monitor )
class IMonitorDllNotify
{	
public:
	virtual void	Begin( ) = 0;
	virtual void	Close( ) = 0;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//��l��
DLLAPI_MONITOR IMonitorDllCmd*  DllCreateSrvMonitorPluginFunc( char* IniFile , IMonitorDllNotify* FromNotify );
//��������귽
DLLAPI_MONITOR void  DllDestorySrvMonitorPluginFunc( );


typedef IMonitorDllCmd* (*CreateSrvMonitorPluginFunc)( char* IniFile , IMonitorDllNotify* FromNotify );
typedef void (*DestroySrvMonitorPluginFunc)( void );
//////////////////////////////////////////////////////////////////////////