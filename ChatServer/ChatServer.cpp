#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <sys/stat.h>
#include <process.h>

#include "NetWaker/GSrvNetWaker.h"
#include "MainProc/Global.h"
#include "MainProc/KeyBoardCmd.h"
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

int RunApp(int argc, char* argv[])
{
	KeyBoardCmd     KBCmd;
    //--------------------------------------------------------------------------
//	LPVOID LPV = Install( CrashCallback , NULL, "Test" );
    //--------------------------------------------------------------------------
    if( argc != 2 )
    {
        MessageBox( NULL , "Chat Ini File (argc !=  2)" , "Error" ,MB_OK );
        return 0;
    }

	CreatePrint( );

	//�Ұ�Controller Client
	ControllerClient::Init("Chat", argv[1], EM_GAME_SERVER_TYPE_CHAT);
	ControllerClient::Start();

    if( Global::Init( argv[1] ) == false )
    {
        return 0;
    }

	//--------------------------------------------------------------------------
	//�]�wCrashDump����
//	SetCrashDumpType(LPV, (CrashDumpTypeENUM)Global::Ini()->Int("CrashDumpType"));
	//--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    //�]�w���� Title ��T
    //--------------------------------------------------------------------------
    char			Buf[512];
    struct _stat	statbuf;
    _stat( argv[0] , &statbuf );

    sprintf(Buf,"Chat Server %s pid = %d ",ctime(&statbuf.st_mtime) , _getpid() );

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
////	CrashProcess::Stop();
#endif

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
	__except(GenerateCrashDumpFile(GetExceptionInformation(), "Chat"))
	{
	}
	return reuslt;
}
