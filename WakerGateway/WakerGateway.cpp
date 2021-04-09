// WakerSwitch.cpp : Defines the entry point for the console application.
//

#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <sys/stat.h>
#include <process.h>

#include "GatewayMainClass.h"
#include "KeyBoardCmd.h"
#include "debuglog/errormessage.h"
#include "ControllerClient/ControllerClient.h"
#include <tchar.h>
#include <DbgHelp.h>
#include <strsafe.h>
#include <createdump/CreateDump.h>
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

	char* IniFile = "Gateway.ini";
	//--------------------------------------------------------------------------
	if( argc == 2 )
	{
		IniFile = argv[1];
		//MessageBox(NULL , "Gateway Ini File (argc !=  2)" , "Error" ,MB_OK);
		//return 0;
	}
	CreatePrint();

	//啟動Controller Client 
	ControllerClient::Init("Gateway", "", EM_GAME_SERVER_TYPE_GATEWAY);
	ControllerClient::Start();

	//--------------------------------------------------------------------------
	//設定視窗 Title 資訊
	//--------------------------------------------------------------------------
	char			Buf[512];
	struct _stat	statbuf;
	_stat( argv[0] , &statbuf );

	sprintf(Buf,"WakerGateway %s pid = %d ",ctime(&statbuf.st_mtime) , _getpid() );

	SetConsoleTitle(Buf);

#ifndef	_DEBUG
	try
#endif
	{
		GatewayMainClass Gateway( IniFile );
		KeyBoardCmd KeyBoard( &Gateway );

		//--------------------------------------------------------------------------
		//設定CrashDump類型
//		SetCrashDumpType(LPV, (CrashDumpTypeENUM)Gateway.Ini()->Int("CrashDumpType"));
		//--------------------------------------------------------------------------

		while( 1 )
		{
			KeyBoard.KeyBoardInput( );
			if( Gateway.Process() == false )
				break;

			ControllerClient::Process();
		}
	}


#ifndef	_DEBUG
	catch(...)
	{

	}
#endif

	ControllerClient::Stop();

//	MyAllocMemoryReport();
//	MyAllocRelease();      
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
	__except(GenerateCrashDumpFile(GetExceptionInformation(), "Gateway"))
	{
	}
	return reuslt;
}