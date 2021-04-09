#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <sys/stat.h>
#include <process.h>

#include "NetWaker/GSrvNetWaker.h"
#include "MainProc/Global.h"
#include "MainProc/KeyBoardCmd.h"
#include "Malloc/MyAlloc.h"
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
    //--------------------------------------------------------------------------
    KeyBoardCmd     KBCmd;
    //--------------------------------------------------------------------------
    if( argc != 2 )
    {
        MessageBox( NULL , "Partition Ini File (argc !=  2)" , "Error" ,MB_OK );
        return 0;
    }

	CreatePrint( );

	//啟動Controller Client
	ControllerClient::Init("Partition", argv[1], EM_GAME_SERVER_TYPE_PARTITION);
	ControllerClient::Start();

    if( Global::Init( argv[1] ) == false )
    {
        return 0;
    }

	//--------------------------------------------------------------------------
	//設定CrashDump類型
//	SetCrashDumpType(LPV, (CrashDumpTypeENUM)Global::Ini()->Int("CrashDumpType"));
	//--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    //設定視窗 Title 資訊
    //--------------------------------------------------------------------------
    char			Buf[512];
    struct _stat	statbuf;
    _stat( argv[0] , &statbuf );

    sprintf(Buf,"Partition Server %s pid = %d ",ctime(&statbuf.st_mtime) , _getpid() );

    SetConsoleTitle(Buf);

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
            KBCmd.KeyBoardInput( );
            if( Global::Process( ) == false )
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

    Global::Release();

	ControllerClient::Stop();

#ifndef	_DEBUG
//	CrashProcess::Stop();
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
	__except(GenerateCrashDumpFile(GetExceptionInformation(), "Partition"))
	{
	}
	return reuslt;
}
