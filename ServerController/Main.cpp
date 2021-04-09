#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include <winsvc.h>
#include <process.h>
#include <iostream>
#include <string>
//#include "CrashRpt/crashrpt.h"
#include "debuglog/errormessage.h"
#include "inifile/IniFile.h"
#include "functionschedular/functionschedular.h"
#include "netbridge/NetBridge.h"
#include "Controller.h"
#include "ClrUtils.h"

//**********************************************************************************************************************************
#define WIN_SERVICE_NAME "RAServerController"
#define WIN_SERVICE_DISPLAY_NAME "RAServerController"

#pragma managed(push, off)

//**********************************************************************************************************************************
void Install();
void UnInstall();
void WINAPI ServiceEntry(unsigned long argc, char **argv);
void WINAPI ServiceHandler(unsigned long ctrl);
void ServiceThread(void *userdata);

//**********************************************************************************************************************************
SERVICE_TABLE_ENTRY g_ServiceEntry[] =
	{
		{WIN_SERVICE_NAME, ServiceEntry},
		{NULL, NULL}};

SERVICE_STATUS_HANDLE g_hServiceStatus;
SERVICE_STATUS g_ServiceStatus;
bool g_ServiceRunning = false;

//**********************************************************************************************************************************
int _tmain(int argc, _TCHAR *argv[])
{
	bool RunService = true;
	unsigned long Err = 0;

	//--------------------------------------------------
	//install crash report
	//CrashRpt::Install(OnGenerateCrashRpt, OnCrashRptGenerated);
	//--------------------------------------------------

	//--------------------------------------------------
	//create print instance
	CreatePrint();
	//--------------------------------------------------

	//--------------------------------------------------
	//set current directory
	wchar_t Buff[MAX_PATH];
	GetModuleFileNameW(NULL, Buff, MAX_PATH);
	std::wstring Dir = Buff;
	int Pos1 = Dir.find_last_of(L'\\');
	int Pos2 = Dir.find_last_of(L'/');
	Dir = Dir.substr(0, max(Pos1, Pos2) + 1);
	SetCurrentDirectoryW(Dir.c_str());
	//--------------------------------------------------

	//--------------------------------------------------
	IniFileClass Ini;
	Ini.SetIniFile("Config.ini");

	int DebugPrintLv = Ini.Int("DebugPrintLv");
	int DebugOutputLv = Ini.Int("DebugOutputLv");
	std::string DebugOutputFile = Ini.Str("DebugOutputFile");
	std::string DebugOutputField = Ini.Str("DebugOutputField");
	g_pPrint->SetPrintLV(DebugPrintLv);
	g_pPrint->SetFileOutputLV(DebugOutputLv);
	g_pPrint->SetOutputFileName(DebugOutputFile.c_str());
	g_pPrint->SetOutputFieldName(DebugOutputField.c_str());

	Print(LV5, "Service", "Start ServiceCtrlDispatcher");
	//--------------------------------------------------

	if (argc >= 2)
	{
		if (_stricmp("-install", argv[1]) == 0)
		{
			if (argc >= 3)
			{
				if (_stricmp("-u", argv[2]) == 0)
				{
					UnInstall();
					RunService = false;
				}
			}
			else
			{
				Install();
				RunService = false;
			}
		}
	}

	if (RunService == true)
	{
		Print(LV5, "Service", "Start ServiceCtrlDispatcher RunService == true");
		//init performance count
		Runewaker::CLR_InitPCManager();
		Print(LV5, "Service", "Runewaker::CLR_InitPCManager()");
		//initialize
		Controller::Init();
		Print(LV5, "Service", "Controller::Init()");
		//begin service thread
		g_ServiceRunning = true;
		if (_beginthread(ServiceThread, 0, NULL) == -1)
		{
			g_ServiceRunning = false;
			Err = GetLastError();
			Print(LV5, "Service", "StartService failed, error code = %d\n", Err);
		}

		//enter service entry
		if (!StartServiceCtrlDispatcher(g_ServiceEntry))
		{
			g_ServiceRunning = false;
			Err = GetLastError();
			Print(LV5, "Service", "StartServiceCtrlDispatcher failed, error code = %d\n", Err);
		}

		Controller::Release();
	}
	else
	{
		Print(LV5, "Service", "Start ServiceCtrlDispatcher RunService == false");
	}

	//--------------------------------------------------
	//
	DestroyPrint();
	//--------------------------------------------------

	//--------------------------------------------------
	//uninstall crash report
	//CrashRpt::Uninstall();
	//--------------------------------------------------

	return 0;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Install()
{
	unsigned long Err = 0;

	// initialize variables for .exe and .log file names
	char ModuleFile[MAX_PATH];
	unsigned long dwSize = GetModuleFileName(NULL, ModuleFile, MAX_PATH);
	ModuleFile[dwSize] = 0;

	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (schSCManager == 0)
	{
		Err = GetLastError();
		Print(LV5, "Service", "OpenSCManager failed, error code = %d\n", Err);
	}
	else
	{
		SC_HANDLE schService = CreateServiceA(
			schSCManager,											 /* SCManager database      */
			WIN_SERVICE_NAME,										 /* name of service         */
			WIN_SERVICE_DISPLAY_NAME,								 /* service name to display */
			SERVICE_ALL_ACCESS,										 /* desired access          */
			SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, /* service type            */
			SERVICE_AUTO_START,										 /* start type              */
			SERVICE_ERROR_NORMAL,									 /* error control type      */
			ModuleFile,												 /* service's binary        */
			NULL,													 /* no load ordering group  */
			NULL,													 /* no tag identifier       */
			NULL,													 /* no dependencies         */
			NULL,													 /* LocalSystem account     */
			NULL													 /* no password             */
		);

		if (schService == 0)
		{
			Err = GetLastError();
			Print(LV5, "Service", "Failed to create service, error code = %d\n", Err);
		}
		else
		{
			Print(LV5, "Service", "Service installed.\n");

			CloseServiceHandle(schService);
		}

		CloseServiceHandle(schSCManager);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void UnInstall()
{
	unsigned long Err = 0;

	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager == 0)
	{
		Err = GetLastError();
		Print(LV5, "Service", "OpenSCManager failed, error code = %d\n", Err);
	}
	else
	{
		SC_HANDLE schService = OpenService(schSCManager, WIN_SERVICE_NAME, SERVICE_ALL_ACCESS);
		if (schService == 0)
		{
			Err = GetLastError();
			Print(LV5, "Service", "OpenService failed, error code = %d\n", Err);
		}
		else
		{
			if (!DeleteService(schService))
			{
				Print(LV5, "Service", "Failed to delete service.\n");
			}
			else
			{
				Print(LV5, "Service", "Service removed.\n");
			}
			CloseServiceHandle(schService);
		}
		CloseServiceHandle(schSCManager);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void WINAPI ServiceEntry(unsigned long argc, char **argv)
{
	unsigned long status = 0;
	unsigned long specificError = 0xfffffff;
	unsigned long Err = 0;

	g_ServiceStatus.dwServiceType = SERVICE_WIN32;
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;
	g_ServiceStatus.dwWaitHint = 0;

	g_hServiceStatus = RegisterServiceCtrlHandler(WIN_SERVICE_NAME, ServiceHandler);
	if (g_hServiceStatus == 0)
	{
		Err = GetLastError();
		Print(LV5, "Service", "RegisterServiceCtrlHandler failed, error code = %d\n", Err);
		return;
	}

	// Initialization complete - report running status
	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	g_ServiceStatus.dwCheckPoint = 0;
	g_ServiceStatus.dwWaitHint = 0;
	if (!SetServiceStatus(g_hServiceStatus, &g_ServiceStatus))
	{
		Err = GetLastError();
		Print(LV5, "Service", "SetServiceStatus failed, error code = %d\n", Err);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void WINAPI ServiceHandler(unsigned long ctrl)
{
	unsigned long Err = 0;

	switch (ctrl)
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
	{
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwCheckPoint = 0;
		g_ServiceStatus.dwWaitHint = 0;

		g_ServiceRunning = false;
	}
	break;
	case SERVICE_CONTROL_PAUSE:
	{
		g_ServiceStatus.dwCurrentState = SERVICE_PAUSED;
	}
	break;
	case SERVICE_CONTROL_CONTINUE:
	{
		g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	}
	break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	default:
	{
		Print(LV5, "Service", "Unrecognized opcode %d\n");
	}
	};

	if (!SetServiceStatus(g_hServiceStatus, &g_ServiceStatus))
	{
		Err = GetLastError();
		Print(LV5, "Service", "SetServiceStatus failed, error code = %d\n", Err);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void ServiceThread(void *userdata)
{
	while (g_ServiceRunning == true)
	{
		//----------------------------------------
		//time
		static unsigned long LastTick = GetTickCount();

		unsigned long ElapsedTime = 0;
		unsigned long NowTick = GetTickCount();

		if (NowTick >= LastTick)
		{
			ElapsedTime = NowTick - LastTick;
		}
		else
		{
			ElapsedTime = (ULONG_MAX - LastTick) + NowTick;
		}

		LastTick = NowTick;
		//----------------------------------------

		//process performance counter
		Runewaker::CLR_ProcessPCManager(NowTick, ElapsedTime);

		//process controller
		Controller::Process(NowTick, ElapsedTime);

		//process scheduler
		g_Schedular.Process();

		//flush net bridge
		CNetBridge::WaitAll(10);

		Sleep(100);
	}
}

#pragma managed(pop)
