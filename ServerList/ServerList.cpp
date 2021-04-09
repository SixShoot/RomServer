// ServerList.cpp : Defines the entry point for the console application.
//
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <sys/stat.h>
#include <process.h>

#include "ServerList/ServerListMainclass.h"
#include "ServerList/KeyBoardCmd.h"
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
	ServerListMainClass*   SrvList;
	//--------------------------------------------------------------------------
	if( argc != 2 )
	{
		MessageBox( NULL , "ServerList Ini File (argc !=  2)" , "Error" ,MB_OK);
		return 0;
	}

	CreatePrint( );

	//啟動Controller Client
	ControllerClient::Init("ServerList", argv[1], EM_GAME_SERVER_TYPE_SERVER_LIST);
	ControllerClient::Start();

	//--------------------------------------------------------------------------
	//設定視窗 Title 資訊
	//--------------------------------------------------------------------------
	char			Buf[512];
	struct _stat	statbuf;
	_stat( argv[0] , &statbuf );

	sprintf(Buf,"ServerList %s pid = %d ",ctime(&statbuf.st_mtime) , _getpid() );

	SetConsoleTitle(Buf);

#ifndef	_DEBUG
////	CrashProcess::Init(GetCurrentThread(), LPV);
////	CrashProcess::Start();
#endif

#ifndef	_DEBUG
	try
#endif
	{
		SrvList = NEW ServerListMainClass( argv[1] );
		KeyBoardCmd KeyBoard( SrvList );

		//--------------------------------------------------------------------------
		//設定CrashDump類型
////		SetCrashDumpType(LPV, (CrashDumpTypeENUM)SrvList->Ini()->Int("CrashDumpType"));
		//--------------------------------------------------------------------------

		while( 1 )
		{
			KeyBoard.KeyBoardInput();
			if( SrvList->Process() == false )
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
	delete SrvList;

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
	__except(GenerateCrashDumpFile(GetExceptionInformation(), "ServerList" ))
	{
	}
	return reuslt;
}
