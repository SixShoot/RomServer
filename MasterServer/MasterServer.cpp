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

//���~�B�z
BOOL WINAPI CrashCallback(LPVOID lpvState)
{
	g_pPrint->OutputBuffToFile();
	return TRUE;
}

//------------------------------------------------------------------------------
int RunApp(int argc, char* argv[])
{
//	LPVOID  LPV = Install( CrashCallback , NULL, "Test" );
    //--------------------------------------------------------------------------
    IniFileClass	Ini;
    KeyBoardCmd     KBCmd;

	printf("\n[Init]Init keyboard command\n");
	fflush(NULL);
	
	MSG				Msg;
	int				iReturn = 0;

    //--------------------------------------------------------------------------
 	CreatePrint();

	//�Ұ�Controller Client
	ControllerClient::Init("Master", argv[1], EM_GAME_SERVER_TYPE_MASTER);
	ControllerClient::Start();

	printf("\n[Init]CreatePrint\n");
	fflush(NULL);


	if( argc != 2 )
	{
		if( Global::Init( "Master.ini" ) == false )
		{
			return 0;
		}
	}
	else
	{
		Global::Init( argv[1] );
	}

	//--------------------------------------------------------------------------
	//�]�wCrashDump����
//	SetCrashDumpType(LPV, (CrashDumpTypeENUM)Global::_Ini.Int("CrashDumpType"));
	//--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    //�]�w���� Title ��T
    //--------------------------------------------------------------------------
    char			Buf[512];
    struct _stat	statbuf;
    _stat( argv[0] , &statbuf );

    sprintf(Buf,"Master Server %s pid = %d ",ctime(&statbuf.st_mtime) , _getpid() );

    SetConsoleTitle(Buf);

#ifndef	_DEBUG
//	CrashProcess::Init(GetCurrentThread(), LPV);
//	CrashProcess::Start();
#endif
	
	/*
#ifndef	_DEBUG
    try
#endif
	*/
    {        
        while( 1 )
        {
			if( PeekMessage( &Msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			} 

            KBCmd.KeyBoardInput( );
            if( Global::Process( ) == false )
                break;

			ControllerClient::Process();

#ifndef	_DEBUG
//			CrashProcess::Check();
#endif
        }

    }

/*
#ifndef	_DEBUG
    catch(...)
    {

    }
#endif
*/
    Global::Release();

	ControllerClient::Stop();

#ifndef	_DEBUG
////	CrashProcess::Stop();
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
	__except(GenerateCrashDumpFile(GetExceptionInformation(), "Master"))
	{
	}
	return reuslt;
}


