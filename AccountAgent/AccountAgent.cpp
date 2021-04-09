// WakerSwitch.cpp : Defines the entry point for the console application.
//
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <sys/stat.h>
#include <process.h>

#include "AccountAgent_Srv/AcSrvMainclass.h"
#include "AccountAgent_Srv/KeyBoardCmd.h"
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
//	LPVOID  LPV = Install( CrashCallback , NULL, "Test" );

    AccountMainClass*   AC;
    //--------------------------------------------------------------------------
    if( argc != 2 )
    {
        MessageBox(NULL , "Account Ini File (argc !=  2)" , "Error" ,MB_OK);
        return 0;
    }
	CreatePrint( );

	//啟動Controller Client
	ControllerClient::Init("Account", argv[1], EM_GAME_SERVER_TYPE_ACCOUNT);
	ControllerClient::Start();

    //--------------------------------------------------------------------------
    //設定視窗 Title 資訊
    //--------------------------------------------------------------------------
    char			Buf[512];
    struct _stat	statbuf;
    _stat( argv[0] , &statbuf );

    sprintf(Buf,"Account %s pid = %d ",ctime(&statbuf.st_mtime) , _getpid() );

    SetConsoleTitle(Buf);

    AC = NEW AccountMainClass( argv[1] );
	KeyBoardCmd KeyBoard( AC );

	//--------------------------------------------------------------------------
	//設定CrashDump類型
////	SetCrashDumpType(LPV, (CrashDumpTypeENUM)AC->Ini()->Int("CrashDumpType"));
	//--------------------------------------------------------------------------

#ifndef	_DEBUG
////	CrashProcess::Init(GetCurrentThread(), LPV);
//	CrashProcess::Start();
#endif

    while( 1 )
    {
		KeyBoard.KeyBoardInput();
        if( AC->Process() == false )
              break;

		ControllerClient::Process();

#ifndef	_DEBUG
//		CrashProcess::Check();
#endif
    }
    
    delete AC;

	ControllerClient::Stop();

#ifndef	_DEBUG
////	CrashProcess::Stop();
#endif

	MyAllocMemoryReport();
	MyAllocRelease();     

	DestroyPrint( );
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
	__except(GenerateCrashDumpFile(GetExceptionInformation(), "Account"))
	{
	}
	return reuslt;
}
