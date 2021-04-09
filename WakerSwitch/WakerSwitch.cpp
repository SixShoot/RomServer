// WakerSwitch.cpp : Defines the entry point for the console application.
//
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <sys/stat.h>
#include <process.h>

#include "SwitchMainClass.h"
#include "KeyBoardCmd.h"
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
	return TRUE;
}

int RunApp(int argc, char* argv[])
{
//	LPVOID  LPV = Install( CrashCallback , NULL, "Test" );
    //--------------------------------------------------------------------------
    if( argc != 2 )
    {
        MessageBox(NULL , "Switch Ini File (argc !=  2)" , "Error" ,MB_OK);
        return 0;
    }
	CreatePrint();

	//�Ұ�Controller Client
	ControllerClient::Init("Switch", argv[1], EM_GAME_SERVER_TYPE_SWITCH);
	ControllerClient::Start();

    //--------------------------------------------------------------------------
    //�]�w���� Title ��T
    //--------------------------------------------------------------------------
    char			Buf[512];
    struct _stat	statbuf;
    _stat( argv[0] , &statbuf );

    sprintf(Buf,"WakerSwitch %s pid = %d ",ctime(&statbuf.st_mtime) , _getpid() );

    SetConsoleTitle(Buf);

#ifndef	_DEBUG
//	CrashProcess::Init(GetCurrentThread(), LPV);
//	CrashProcess::Start();
#endif

#ifndef	_DEBUG
    try
#endif
    {
        SwitchMainClass Switch( argv[1] );
		KeyBoardCmd KeyBoard( &Switch );

		//--------------------------------------------------------------------------
		//�]�wCrashDump����
//		SetCrashDumpType(LPV, (CrashDumpTypeENUM)Switch.Ini()->Int("CrashDumpType"));
		//--------------------------------------------------------------------------

        while( 1 )
        {
			KeyBoard.KeyBoardInput( );
            if( Switch.Process() == false )
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

	ControllerClient::Stop();

#ifndef	_DEBUG
//	CrashProcess::Stop();
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
	__except(GenerateCrashDumpFile(GetExceptionInformation(), "Switch"))
	{
	}
	return reuslt;
}
