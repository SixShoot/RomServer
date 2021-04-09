#ifdef _DEBUG
	#define VLD_MAX_DATA_DUMP 256
	#include "LeakDetector/vld.h"
#endif
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
	g_pPrint->OutputBuffToFile();
	return TRUE;
}

int RunApp(int argc, char* argv[])
{
//	LPVOID  LPV = Install( CrashCallback , NULL, "Test" );
    //--------------------------------------------------------------------------
    IniFileClass	Ini;
    KeyBoardCmd*    KBCmd;
    //--------------------------------------------------------------------------
	CreatePrint( );

	//啟動Controller Client
	ControllerClient::Init("PlayerCenter", argv[1], EM_GAME_SERVER_TYPE_PLAYERCENTER);
	ControllerClient::Start();

    if( argc != 2 )
    {
        MessageBox( NULL , "GSrv Ini File (argc !=  2)" , "Error" ,MB_OK );
        return 0;
    }

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
	MSG				Msg;
    char			Buf[512];
    struct _stat	statbuf;
    _stat( argv[0] , &statbuf );

    sprintf(Buf,"RoleDataCenter Server %s pid = %d ",ctime(&statbuf.st_mtime) , _getpid() );

    SetConsoleTitle(Buf);

#ifndef	_DEBUG
//	CrashProcess::Init(GetCurrentThread(), LPV);
//	CrashProcess::Start();
#endif

#ifndef	_DEBUG
    try
#endif
    {        
		KBCmd = NEW( KeyBoardCmd );
        while( 1 )
        {
			if( PeekMessage( &Msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			} 

			KBCmd->KeyBoardInput( );
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
	

	Print( LV5 , "Destroy" , "isdestroy=%d reason=%d" , Global::IsDestory() , Global::DestoryReason() );
	g_pPrint->OutputBuffToFile();

    Global::Release();
	delete KBCmd;

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
	__except(GenerateCrashDumpFile(GetExceptionInformation(), "PlayerCenter"))
	{
	}
	return reuslt;
}
