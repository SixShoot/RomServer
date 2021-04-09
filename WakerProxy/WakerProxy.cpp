// WakerProxy.cpp : Defines the entry point for the console application.
//


#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <sys/stat.h>
#include <process.h>

#include "ProxyMainClass.h"
#include "Malloc/MyAlloc.h"
#include "KeyBoardCmd.h"
#include <tchar.h>
#include <DbgHelp.h>
#include <strsafe.h>
#include "createdump/CreateDump.h"
//#include "CrashProcess/CrashProcess.h"
//#include "CrashRpt\crashrpt.h" 
//#pragma comment(lib, "CrashRpt.lib") 
//錯誤處理
BOOL WINAPI CrashCallback(LPVOID lpvState)
{
	return TRUE;
}

int RunApp(int argc, char* argv[])
{		

	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

    //--------------------------------------------------------------------------
    if( argc != 2 )
    {
        MessageBox(NULL , "Proxy Ini File (argc !=  2)" , "Error" ,MB_OK);
        return 0;
    }
//	LPVOID  LPV = Install( CrashCallback , NULL, "Test" );

	CreatePrint( );

	//啟動Controller Client
	ControllerClient::Init("Proxy", argv[1], EM_GAME_SERVER_TYPE_PROXY);
	ControllerClient::Start();

    //--------------------------------------------------------------------------
    //設定視窗 Title 資訊
    //--------------------------------------------------------------------------
    char			Buf[512];
    struct _stat	statbuf;
    _stat( argv[0] , &statbuf );

    sprintf(Buf,"WakerProxy %s pid = %d ini=%s",ctime(&statbuf.st_mtime) , _getpid() , argv[1] );

    SetConsoleTitle(Buf);

	ProxyMainClass*	Proxy = NEW ProxyMainClass( argv[1] );
	KeyBoardCmd*    KBCmd = NEW KeyBoardCmd( Proxy );

	//--------------------------------------------------------------------------
	//設定CrashDump類型
////	SetCrashDumpType(LPV, (CrashDumpTypeENUM)Proxy->Ini()->Int("CrashDumpType"));
	//--------------------------------------------------------------------------

#ifndef	_DEBUG
//	CrashProcess::Init(GetCurrentThread(), LPV);
//	CrashProcess::Start();
#endif

#ifndef	_DEBUG
    try
#endif
    {
        while( 1 )
        {
            KBCmd->KeyBoardInput( );
            if( Proxy->Process() == false )
                break;

			ControllerClient::Process();

#ifndef	_DEBUG
//			CrashProcess::Check();
#endif
        }
    }


#ifndef	_DEBUG
    catch(...)
    {

    }
#endif
	delete Proxy;
	delete KBCmd;

	ControllerClient::Stop();

#ifndef	_DEBUG
///	CrashProcess::Stop();
#endif

	MyAllocMemoryReport();
	MyAllocRelease();        

	DestroyPrint();

//	Uninstall( LPV );
    return 0;

}

int main(int argc, char* argv[])
{
	INT reuslt = 0;
	__try
	{
		reuslt = RunApp(argc, argv);
	}
	__except(GenerateCrashDumpFile(GetExceptionInformation(), "Proxy"))
	{
	}
	return reuslt;
}