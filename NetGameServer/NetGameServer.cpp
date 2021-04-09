#include "ServerDll.h"
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <sys/stat.h>
#include <process.h>

#include "Malloc/MyAlloc.h"
#include "NetWaker/GSrvNetWaker.h"
#include "MainProc/Global.h"
#include "MainProc/KeyBoardCmd.h"
#include "netwalker_member/NetWakerGSrvInc.h"
#include <tchar.h>
#include <DbgHelp.h>
#include <strsafe.h>
#include "md5/Mymd5.h"
#include "createdump/CreateDump.h"
//---------------------------------------------------------------------------------------------------
//主回圈Class
class MainProcClass : public IFormCommand
{
	IGameSrvPluginNotify*	_FromNotify;		//通知from事件
	MutilThread_CritSect    _Thread_CritSect;
	DWORD					_ThreadID;		
	HANDLE					_ThreadHandle;
	bool					_IsDestroy;
	bool					_IsThreadRuning;
protected:
	static DWORD WINAPI ThreadProcess( void* _This )
	{		
		MainProcClass* This = (MainProcClass*)_This;
		This->_IsThreadRuning = true;
		#ifdef NDEBUG
		try
		{
			This->Process( );
		}
		catch (...)
		{
			//當機資料儲存
			//記錄處理那個物件 or 玩家當的
		}
		#else
			This->Process( );
		#endif

		This->_IsThreadRuning = false;
		return 0;
	};

	void Process(  )
	{ 
		while( _IsDestroy == false )
		{
			_Thread_CritSect.Enter();
			{
				if( Global::Process( ) == false )
					break;
			}
			_Thread_CritSect.Leave();			
		}				
	};

public:
	~MainProcClass()
	{
		Release();
	};

	bool	Init( char* IniFile , IGameSrvPluginNotify* FromNotify )
	{
		_IsThreadRuning = false;
		_IsDestroy	  = false;
		_FromNotify   = FromNotify;		
		bool Ret = Global::Init( IniFile );
		Global::St()->FromNotify = FromNotify;

		if( Ret != false )
		{
			 _ThreadHandle = CreateThread( NULL , 0 , ThreadProcess , (LPVOID) this , 0 , &_ThreadID );
		}

		return Ret;
	};

	bool	Release( )
	{
		return Global::Release();
	};


	//超過10秒沒更新角色時間
	bool	IsCrash( )
	{
		return (unsigned)TimeStr::Now_Value() > RoleDataEx::G_Now + 10000 - RoleDataEx::SysDTime;
	};

	void	Destroy()
	{
		_IsDestroy = true;
	};

	bool	IsThreadRuning()
	{
		return _IsThreadRuning;
	}

	virtual	void Command( char* cmd )
	{	

	};

	//IFormCommand
	virtual	GameObjDbStruct*	GetObjDB( int OrgObjID )
	{
		GameObjDbStruct* RetObjDB;
		_Thread_CritSect.Enter();
		{
			RetObjDB = Global::GetObjDB( OrgObjID );
		}
		_Thread_CritSect.Leave();
		return RetObjDB;
	}
	virtual RoleDataEx*			GetRole( int GUID )
	{
		RoleDataEx* RetRole;
		_Thread_CritSect.Enter();
		{
			RetRole = Global::GetRoleDataByGUID( GUID );
		}
		_Thread_CritSect.Leave();
		return RetRole;
	}
	virtual GSrvInfoClass*		GetZoneInfo( )
	{
		GSrvInfoClass* RetIni;
		_Thread_CritSect.Enter();
		{
			RetIni = (GSrvInfoClass*)Global::Ini();
		}
		_Thread_CritSect.Leave();
		return RetIni;
	}
	virtual	void				SysMsg( int GUID , char* Msg )
	{
		_Thread_CritSect.Enter();
		{
			NetSrv_Talk::SysMsg( GUID , Msg );
		}		
		_Thread_CritSect.Leave();
	}
};

MainProcClass*	g_MainProc = NULL;

//初始化
DLLAPI IFormCommand*  DllCreateServerPluginFunc( char* IniFile , IGameSrvPluginNotify* FromNotify )
{
	if( g_MainProc != NULL )
		return NULL;

	g_MainProc = NEW( MainProcClass );
	if( g_MainProc->Init( IniFile , FromNotify ) == false )
		return NULL;

	return g_MainProc;
}

//結束釋放資源
DLLAPI void  DllDestoryServerPluginFunc( )
{
	delete g_MainProc;
}
//---------------------------------------------------------------------------------------------------
//如果不是使用DLL模式
#ifdef DLL_EXPORTS_GAMESERVER
BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#else

//***************************************************************************************************

#include "smallobj/SmallObj.h"
//#include "CrashProcess/CrashProcess.h"
//#include "CrashRpt\crashrpt.h" 
//#pragma comment(lib, "CrashRpt.lib") 

//錯誤處理
BOOL WINAPI CrashCallback(LPVOID lpvState)
{
	//資料回存
	Global::SaveAllPlayer_byCrash();
	g_pPrint->OutputBuffToFile();
	return TRUE;
}

int RunApp(int argc, char* argv[])
{
	//LPVOID LPV = Install( CrashCallback , NULL, "Test" );
	//LPVOID LPV = Install( CrashCallback , NULL, "Test" );

	//設定錯誤回報
	IniFileClass	Ini;
	KeyBoardCmd     KBCmd;

	CreatePrint( );

	//啟動Controller Client 
	ControllerClient::Init("Zone", "", EM_GAME_SERVER_TYPE_ZONE);
	ControllerClient::Start();

	//--------------------------------------------------------------------------
	if( argc != 2 )
	{
		if( Global::Init( "GServer.ini" ) == false )
		{
			return 0;
		}
	}
	else
	{
		if( Global::Init( argv[1] ) == false )
		{
			return 0;
		};
	}

	//--------------------------------------------------------------------------
	//設定CrashDump類型
	//SetCrashDumpType(LPV, (CrashDumpTypeENUM)Global::Ini()->Int("CrashDumpType"));
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	//設定視窗 Title 資訊
	//--------------------------------------------------------------------------
	char			Buf[512];
	struct _stat	statbuf;
	_stat( argv[0] , &statbuf );

	sprintf(Buf,"WakerGameSrv %s pid = %d (%d)%s",ctime(&statbuf.st_mtime) , _getpid() , Global::Ini()->ZoneID , Utf8ToChar( Global::Ini()->ZoneName.c_str() ).c_str() );

	SetConsoleTitle(Buf);
	
#ifndef	_DEBUG
	//CrashProcess::Init(GetCurrentThread(), LPV);
	//CrashProcess::SetOnCheck(Global::OnCrashProcessCheck);
	//CrashProcess::SetOnExit(Global::OnCrashProcessExit);
	//CrashProcess::Start();
#endif

	while( 1 )
	{

			KBCmd.KeyBoardInput( );
			if( Global::Process( ) == false )
				break;

			ControllerClient::Process();

#ifndef	_DEBUG
			//CrashProcess::Check();
#endif

	}

	Global::Release();

	ControllerClient::Stop();

#ifndef	_DEBUG
	//CrashProcess::Stop();
#endif

	MyAllocMemoryReport();
	MyAllocRelease();        

	DestroyPrint( );

	//Uninstall( LPV );

	return 0;
}

#endif
int main(int argc, char* argv[])
{
	INT reuslt = 0;
	__try
	{
		reuslt = RunApp(argc, argv);
	}
	__except(GenerateCrashDumpFile(GetExceptionInformation(), "Zone"))
	{
	}
	return reuslt;
}
