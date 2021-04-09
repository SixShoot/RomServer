#include "GameServer.h"
#include "Controller.h"
#include "Monitor.h"
#include "controllerclient/PG_Controller.h"
#include "verinfo.h"
#include "debuglog/errormessage.h"
#include "SmallObj/SmallObj.h"
#include <psapi.h>
#include <stdio.h>
#include <time.h>
#include <direct.h>
#include <sys/stat.h>
#include <math.h>
#include "ClrUtils.h"

#pragma managed(push, off)
//***********************************************************************************************
#define GS_PING_TIME 5000
#define GS_WARNING_TIME 60000
#define GS_NO_RESPONSE_TIME 180000

//***********************************************************************************************
std::map<std::wstring, ExeInfo> CGameServer::_ExeInfo;

//***********************************************************************************************

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
MyConsole::MyConsole()
{
	SecureZeroMemory(&m_Info, sizeof(m_Info));

	m_Status = EM_CONSOLE_STATUS_IDLE;
	m_TTL = 0;
	m_ProcessorCount = 0;
	m_LastKernelTime = 0;
	m_LastUserTime = 0;
	m_LastElapsedTime = 0;
	m_CpuUsage = 0;
	m_MemoryUsage = 0;
	m_Attached = false;

	SYSTEM_INFO Sysinfo;
	GetSystemInfo(&Sysinfo);
	m_ProcessorCount = Sysinfo.dwNumberOfProcessors;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
MyConsole::~MyConsole()
{
	Terminate();
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void MyConsole::Process(unsigned long nowtick, unsigned long elapsedtime)
{
	unsigned long exitCode = 0;

	switch (m_Status)
	{
	case EM_CONSOLE_STATUS_IDLE:
		break;

	case EM_CONSOLE_STATUS_RUNNING:
	{
		if (IsProcessAlive())
		{
			m_LastElapsedTime += elapsedtime;

			if (m_LastElapsedTime >= 500)
			{
				UpdateCpuUsage(m_LastElapsedTime);
				UpdateMemUsage();

				m_LastElapsedTime = 0;
			}
		}
		else
		{
			m_Status = EM_CONSOLE_STATUS_RUNTIME_ERROR;
		}
	}
	break;
	case EM_CONSOLE_STATUS_STOPPING:
	{
		if (!IsProcessAlive())
		{
			Terminate();
		}
		else
		{
			// Deduct termination TTL
			m_TTL -= elapsedtime;

			// TTL 0 terminate process
			if (m_TTL <= 0)
			{
				Terminate(true);
				m_TTL = 0;
			}
		}
	}
	break;
	case EM_CONSOLE_STATUS_START_FAILED:
		break;
	case EM_CONSOLE_STATUS_RUNTIME_ERROR:
	{
		CloseHandles();
	}
	break;
	}
}

//-----------------------------------------------------------------------------------------------
// Start and termination
//-----------------------------------------------------------------------------------------------
bool MyConsole::Start(const wchar_t *filepath, const wchar_t *cmdline, bool windowed)
{
	// Spawn a child thread
	ZeroMemory(&m_Info, sizeof(PROCESS_INFORMATION));

	STARTUPINFOW si;
	ZeroMemory(&si, sizeof(STARTUPINFOW));
	si.cb = sizeof(STARTUPINFOW);

	unsigned long dwCreationFlags = 0;

	if (windowed)
	{
		dwCreationFlags = CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS;
	}
	else
	{
		dwCreationFlags = CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS;
	}

	// Setup current directory string
	std::wstring curDir = filepath;
	int Pos1 = curDir.find_last_of(L'\\');
	int Pos2 = curDir.find_last_of(L'/');
	curDir.erase(max(Pos1, Pos2));

	// Setup the command line string
	wchar_t CmdLine[512];
	SecureZeroMemory(CmdLine, sizeof(CmdLine));

	swprintf(CmdLine, 512, L"%s %s", filepath, cmdline);

	BOOL succeeded = CreateProcessW(
		filepath,
		CmdLine,
		NULL,
		NULL,
		FALSE,
		dwCreationFlags,
		NULL,
		curDir.c_str(),
		&si,
		&m_Info);

	if (!succeeded)
	{
		unsigned long errorCode = GetLastError();
		Print(5, "", "Failed to start process, error code is %x.", errorCode);
		Print(5, "", "CmdLine %ls", CmdLine);



		m_Status = EM_CONSOLE_STATUS_START_FAILED;

		return false;
	}

	// Mark process as having started
	m_Status = EM_CONSOLE_STATUS_RUNNING;

	return true;
}

//-----------------------------------------------------------------------------------------------
//force to combine a exist process ID
//-----------------------------------------------------------------------------------------------
bool MyConsole::Start(unsigned long pid, bool attached)
{
	// we have a started process, the process may be a duplicated game server process
	if ((m_Status == EM_CONSOLE_STATUS_RUNNING) && !attached)
	{
		//Terminate the exist process
		Terminate();
	}

	m_Info.dwProcessId = pid;
	m_Info.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	m_Attached = attached;
	m_Status = EM_CONSOLE_STATUS_RUNNING;
	return true;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void MyConsole::Stop(unsigned long ttl)
{
	m_TTL = ttl;
	m_Status = EM_CONSOLE_STATUS_STOPPING;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void MyConsole::Reset(unsigned long pid)
{
	if (pid != 0)
	{
		Runewaker::CLR_KillProcess(pid);
	}
	m_Status = EM_CONSOLE_STATUS_IDLE;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool MyConsole::CreateDump(const wchar_t *filename, unsigned long dumptype)
{
	bool Result = false;

	if ((m_Status == EM_CONSOLE_STATUS_RUNNING) && (PID > 0))
	{
		HANDLE hFile = CreateFileW(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
		{
			MINIDUMP_TYPE mdt = MiniDumpNormal;

			switch (dumptype)
			{
			case EM_CONSOLE_DUMP_MID:
			{
				mdt = (MINIDUMP_TYPE)(MiniDumpWithPrivateReadWriteMemory | MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpWithUnloadedModules);
			}
			break;
			case EM_CONSOLE_DUMP_MAX:
			{
				mdt = (MINIDUMP_TYPE)(MiniDumpWithFullMemory | MiniDumpWithHandleData | MiniDumpWithUnloadedModules);
			}
			break;
			}

			Result = (MiniDumpWriteDump(m_Info.hProcess, m_Info.dwProcessId, hFile, mdt, NULL, NULL, NULL) == TRUE);

			CloseHandle(hFile);

			if (!Result)
				Print(LV5, "MyConsole::CreateDump", "MiniDumpWriteDump failed. Error: %u", GetLastError());
		}
		else
		{
			Print(LV5, "MyConsole::CreateDump", "Create file %s failed. ", WcharToChar(filename).c_str());
		}
	}

	return Result;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void MyConsole::CloseHandles()
{
	if (m_Info.hProcess != NULL)
	{
		// Close child process handles
		CloseHandle(m_Info.hProcess);
	}

	if (m_Info.hThread != NULL)
	{
		CloseHandle(m_Info.hThread);
	}

	SecureZeroMemory(&m_Info, sizeof(m_Info));
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool MyConsole::Terminate(bool forcibly)
{
	if (forcibly)
	{
		if (m_Info.hProcess != NULL && !m_Attached)
		{
			// Forcibly terminate the child process
			if (TerminateProcess(m_Info.hProcess, 0))
			{
				CloseHandles();
			}
			else
			{
				Print(LV5, "MyConsole::Terminate", "Terminate failure PID = %d", m_Info.dwProcessId);
			}
		}
	}
	else
	{
		CloseHandles();
	}

	m_Status = EM_CONSOLE_STATUS_IDLE;

	return true;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool MyConsole::IsProcessAlive()
{
	bool Result = false;
	unsigned long dwExitCode = 0;

	if (GetExitCodeProcess(m_Info.hProcess, &dwExitCode))
	{
		if (dwExitCode == STILL_ACTIVE)
			Result = true;
	}

	return Result;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void MyConsole::UpdateCpuUsage(unsigned long elapsedtime)
{
	//FILETIME = 100-nanosecond intervals since January 1, 1601 (UTC)
	FILETIME ftCreate;
	FILETIME ftExit;
	FILETIME ftKernel;
	FILETIME ftUser;

	INT64 NowKernelTime = 0;
	INT64 NowUserTime = 0;
	INT64 KernelTimeDelta = 0;
	INT64 UserTimeDelta = 0;
	INT64 TotalCpuTime = 0;

	//Process time = KernelTime + UserTime
	GetProcessTimes(m_Info.hProcess, &ftCreate, &ftExit, &ftKernel, &ftUser);
	NowKernelTime = ((INT64)ftKernel.dwHighDateTime << 32) + ftKernel.dwLowDateTime;
	NowUserTime = ((INT64)ftUser.dwHighDateTime << 32) + ftUser.dwLowDateTime;

	TotalCpuTime = (NowKernelTime - m_LastKernelTime) + (NowUserTime - m_LastUserTime);

	m_LastKernelTime = NowKernelTime;
	m_LastUserTime = NowUserTime;

	//1s = 10^7 * 100ns
	//Usage% = ((Total CPU time[second] / ProcessorCount) / (elapsedtime[second])) * 100
	//+0.5 for round
	m_CpuUsage = (int)((((((double)(TotalCpuTime) / 10000000) / m_ProcessorCount) / ((double)elapsedtime / 1000)) * 100) + 0.5);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void MyConsole::UpdateMemUsage()
{
	m_MemoryUsage = Runewaker::CLR_GetMemUsage(m_Info.dwProcessId) / 1024;
}

//***********************************************************************************************

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CGameServer::Init()
{
	RegisterExe(L"account.exe", 0, EM_GAME_SERVER_TYPE_ACCOUNT);
	RegisterExe(L"serverlist.exe", 0, EM_GAME_SERVER_TYPE_SERVER_LIST);
	RegisterExe(L"auroraagent.exe", 0, EM_GAME_SERVER_TYPE_AURORAAGENT);
	RegisterExe(L"switch.exe", 1, EM_GAME_SERVER_TYPE_SWITCH);
	RegisterExe(L"wakerswitch.exe", 1, EM_GAME_SERVER_TYPE_SWITCH);
	RegisterExe(L"proxy.exe", 2, EM_GAME_SERVER_TYPE_PROXY);
	RegisterExe(L"wakerproxy.exe", 2, EM_GAME_SERVER_TYPE_PROXY);
	RegisterExe(L"master.exe", 3, EM_GAME_SERVER_TYPE_MASTER);
	RegisterExe(L"masterserver.exe", 3, EM_GAME_SERVER_TYPE_MASTER);
	RegisterExe(L"datacenter.exe", 4, EM_GAME_SERVER_TYPE_DATACENTER);
	RegisterExe(L"roledatacenter.exe", 4, EM_GAME_SERVER_TYPE_DATACENTER);
	RegisterExe(L"playercenter.exe", 4, EM_GAME_SERVER_TYPE_PLAYERCENTER);
	RegisterExe(L"nonageaccountserver.exe", 5, EM_GAME_SERVER_TYPE_NONAGE_ACCOUNT);
	RegisterExe(L"nonageserver.exe", 6, EM_GAME_SERVER_TYPE_NONAGE);
	RegisterExe(L"bti.exe", 6, EM_GAME_SERVER_TYPE_NONAGE);
	RegisterExe(L"chat.exe", 6, EM_GAME_SERVER_TYPE_CHAT);
	RegisterExe(L"chatserver.exe", 6, EM_GAME_SERVER_TYPE_CHAT);
	RegisterExe(L"partition.exe", 6, EM_GAME_SERVER_TYPE_PARTITION);
	RegisterExe(L"partitionsrv.exe", 6, EM_GAME_SERVER_TYPE_PARTITION);
	RegisterExe(L"billing.exe", 6, EM_GAME_SERVER_TYPE_BILLING);
	RegisterExe(L"gateway.exe", 7, EM_GAME_SERVER_TYPE_GATEWAY);
	RegisterExe(L"forward.exe", 7, EM_GAME_SERVER_TYPE_FORWARD);
	RegisterExe(L"luadebugagent.exe", 7, EM_GAME_SERVER_TYPE_LUADEBUGAGENT);
	RegisterExe(L"logcenterserver.exe", 7, EM_GAME_SERVER_TYPE_LOGCENTER);
	RegisterExe(L"netgameserver.exe", 8, EM_GAME_SERVER_TYPE_ZONE);
	RegisterExe(L"zone.exe", 8, EM_GAME_SERVER_TYPE_ZONE);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CGameServer::RegisterExe(const wchar_t *name, int order, int type)
{
	ExeInfo &Info = _ExeInfo[name];
	Info.StartupOrder = order;
	Info.SrvType = type;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
CGameServer::CGameServer()
{
	m_Index = -1;
	m_RegionName = L"";
	m_GroupName = L"";
	m_ServerName = L"";
	m_ExePath = L"";
	m_TempExePath = L"";
	m_CmdLine = L"";
	m_FileVer = "";
	m_CustomStatusString = "";

	m_Disabled = false;
	m_StartupOrder = 0;
	m_Attached = false;
	m_WorldID = 0;

	m_Status = EM_SERVER_STATUS_OFFLINE;
	m_CustomStatus = EM_SERVER_STATUS_NA;
	m_NetID = INVALID_NET_ID;
	m_Type = EM_GAME_SERVER_TYPE_NONE;
	m_Census = -1;
	m_SelfStop = false;

	UpdateResponseTick();
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CGameServer::Process(unsigned long nowtick, unsigned long elapsedtime)
{
	m_Console.Process(nowtick, elapsedtime);

	unsigned long ResponseTime = nowtick - m_LastResponseTick;

	switch (m_Status)
	{
	case EM_SERVER_STATUS_NA:
	case EM_SERVER_STATUS_DISABLED:
	case EM_SERVER_STATUS_OFFLINE:
	{
		//prevent status becoming "response warning" or "no response" after status back to online
		m_LastResponseTick = nowtick;

		if (m_TempExePath.empty() == false)
		{
			if (DeleteTempExeFile() == true)
			{
				m_TempExePath = L"";
			}
		}
	}
	break;
	case EM_SERVER_STATUS_CUSTOM_0:
	case EM_SERVER_STATUS_CUSTOM_1:
	case EM_SERVER_STATUS_CUSTOM_2:
	case EM_SERVER_STATUS_CUSTOM_3:
	case EM_SERVER_STATUS_RESPONSE_WARNING:
	case EM_SERVER_STATUS_NO_RESPONSE:
	case EM_SERVER_STATUS_ONLINE:
	{
		//------------------------------------
		//check console status
		switch (m_Console.Status)
		{
		case EM_CONSOLE_STATUS_RUNNING:
		{
			if (ResponseTime <= GS_WARNING_TIME)
			{
				Status = EM_SERVER_STATUS_ONLINE;
			}

			// Check elapsed time since ping request
			if (ResponseTime > GS_PING_TIME)
			{
				Controller::This()->SGS_Command(m_NetID, "Ping", ServerNameANSI.c_str());
			}

			if ((ResponseTime > GS_WARNING_TIME) && (ResponseTime <= GS_NO_RESPONSE_TIME))
			{
				Status = EM_SERVER_STATUS_RESPONSE_WARNING;
			}

			if (ResponseTime > GS_NO_RESPONSE_TIME)
			{
				Status = EM_SERVER_STATUS_NO_RESPONSE;
			}

			//controller - server console disconnect
			if (m_NetID == INVALID_NET_ID)
			{
				Status = EM_SERVER_STATUS_DISCONNECTED;
			}
		}
		break;
		case EM_CONSOLE_STATUS_STOPPING:
		{
			Status = EM_SERVER_STATUS_STOPPING;
		}
		break;
		case EM_CONSOLE_STATUS_IDLE:
		case EM_CONSOLE_STATUS_START_FAILED:
		case EM_CONSOLE_STATUS_RUNTIME_ERROR:
		{
			if (m_SelfStop == true)
			{
				Stop();
				Status = EM_SERVER_STATUS_OFFLINE;
			}
			else
			{
				Status = EM_SERVER_STATUS_ERROR;
			}
		}
		break;
		}
	}
	break;
	case EM_SERVER_STATUS_ERROR:
	{
		//prevent status becoming "response warning" or "no response" after status back to online
		m_LastResponseTick = nowtick;

		if (m_TempExePath.empty() == false)
		{
			if (DeleteTempExeFile() == true)
			{
				m_TempExePath = L"";
			}
		}
	}
	break;
	case EM_SERVER_STATUS_STARTING:
	{
		//------------------------------------
		//check console status
		switch (m_Console.Status)
		{
		case EM_CONSOLE_STATUS_IDLE:
		case EM_CONSOLE_STATUS_START_FAILED:
		case EM_CONSOLE_STATUS_RUNTIME_ERROR:
		{
			if (m_SelfStop == true)
			{
				Stop();
				Status = EM_SERVER_STATUS_OFFLINE;
			}
			else
			{
				Status = EM_SERVER_STATUS_ERROR;
			}
		}
		break;
		}
	}
	break;
	case EM_SERVER_STATUS_STOPPING:
	{
		if (m_Console.Status == EM_CONSOLE_STATUS_IDLE)
		{
			Status = EM_SERVER_STATUS_OFFLINE;
		}
	}
	break;
	case EM_SERVER_STATUS_DISCONNECTED:
	{
		//prevent status becoming "response warning" or "no response" after status back to online
		m_LastResponseTick = nowtick;
		if (m_NetID != INVALID_NET_ID)
		{
			Status = EM_SERVER_STATUS_ONLINE;
		}
	}
	break;
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool CGameServer::Start()
{
	bool Result = true;
	if (!m_Disabled)
	{
		ExePath = m_ExePath.c_str();

		if (m_Console.Start(m_ExePath.c_str(), m_CmdLine.c_str()))
		{
			Status = EM_SERVER_STATUS_STARTING;
			Result = true;
			Print(LV2, "CGameServer::Start", "EM_SERVER_STATUS_STARTING");
		}
		else
		{
			Status = EM_SERVER_STATUS_ERROR;
			Print(LV2, "CGameServer::Start", "EM_SERVER_STATUS_ERROR");
		}
	}
	return Result;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool CGameServer::Start(unsigned long pid)
{
	bool Result = false;

	if (!m_Disabled && (pid > 0))
	{
		if (m_Console.Start(pid, m_Attached))
		{
			Status = EM_SERVER_STATUS_ONLINE;
			Result = true;
		}
		else
		{
			Status = EM_SERVER_STATUS_ERROR;
		}
	}

	return Result;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool CGameServer::Stop()
{
	// Signal server to save
	Controller::This()->SGS_Command(m_NetID, "SaveAllExit", ServerNameANSI.c_str());

	// Signal server to exit
	Controller::This()->SGS_Command(m_NetID, "Exit", ServerNameANSI.c_str());

	//Terminate forcibly after 180 secs
	m_Console.Stop(180000);

	Status = EM_SERVER_STATUS_STOPPING;

	return true;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool CGameServer::Reset()
{
	//Terminate forcibly after 180 secs
	m_Console.Reset(PID);

	Status = EM_SERVER_STATUS_OFFLINE;

	return true;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool CGameServer::CreateDump(int dumptype)
{
	wchar_t Buff[MAX_PATH];

	time_t NowTime;
	time(&NowTime);
	struct tm *gmt;
	gmt = localtime(&NowTime);

	_wmkdir(L"DumpFiles");

	swprintf(Buff, MAX_PATH, L"DumpFiles\\%s_%s_%s_%d%02d%02d%02d%02d%02d.dmp", RegionName, GroupName, ServerName, 1900 + gmt->tm_year, 1 + gmt->tm_mon, gmt->tm_mday, gmt->tm_hour, gmt->tm_min, gmt->tm_sec);

	return m_Console.CreateDump(Buff, dumptype);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool CGameServer::DeleteTempExeFile()
{
	bool Result = false;
	struct _stat64i32 TempStat;

	if (_wstat(m_TempExePath.c_str(), &TempStat) == 0)
	{
		if (DeleteFileW(m_TempExePath.c_str()) == TRUE)
		{
			Result = true;
		}
	}
	else
	{
		Result = true;
	}

	return Result;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CGameServer::AddCommand(const char *caption, const char *command, bool inputrequired, bool enabled)
{
	std::map<std::string, GSCommand>::iterator it = m_CommandsMap.find(caption);

	if (it != m_CommandsMap.end())
	{
		(it->second).InputRequired = inputrequired ? 1 : 0;
		(it->second).Enabled = enabled ? 1 : 0;
		strcpy_s((it->second).Command, sizeof((it->second).Command), command);
	}
	else
	{
		GSCommand &Cmd = m_CommandsMap[caption];
		Cmd.Index = m_Index;
		Cmd.InputRequired = inputrequired ? 1 : 0;
		Cmd.Enabled = enabled ? 1 : 0;
		strcpy_s(Cmd.Caption, sizeof(Cmd.Caption), caption);
		strcpy_s(Cmd.Command, sizeof(Cmd.Command), command);

		m_Commands.push_back(&Cmd);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CGameServer::RemoveCommand(const char *caption)
{
	std::map<std::string, GSCommand>::iterator it = m_CommandsMap.find(caption);

	if (it != m_CommandsMap.end())
	{
		std::vector<GSCommand *>::iterator itVec;

		itVec = find(m_Commands.begin(), m_Commands.end(), &(it->second));

		if (itVec != m_Commands.end())
		{
			m_Commands.erase(itVec);
		}

		m_CommandsMap.erase(it);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CGameServer::EnableCommand(const char *caption)
{
	std::map<std::string, GSCommand>::iterator it = m_CommandsMap.find(caption);

	if (it != m_CommandsMap.end())
	{
		(it->second).Enabled = 1;
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CGameServer::DisableCommand(const char *caption)
{
	std::map<std::string, GSCommand>::iterator it = m_CommandsMap.find(caption);

	if (it != m_CommandsMap.end())
	{
		(it->second).Enabled = 0;
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CGameServer::UpdateResponseTick()
{
	m_LastResponseTick = GetTickCount();
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
std::string CGameServer::GetRegionNameANSI()
{
	return WcharToChar(m_RegionName.c_str());
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
std::string CGameServer::GetGroupNameANSI()
{
	return WcharToChar(m_GroupName.c_str());
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
std::string CGameServer::GetServerNameANSI()
{
	return WcharToChar(m_ServerName.c_str());
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
std::string CGameServer::GetExePathANSI()
{
	return WcharToChar(m_ExePath.c_str());
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CGameServer::SetExePath(const wchar_t *val)
{
	m_ExePath = val;

	CFileVersionInfo Ver;
	if (Ver.Open(ExePathANSI.c_str()) == TRUE)
	{
		char Buff[128] = {0};

		if (Ver.QueryStringValue(VI_STR_PRODUCTVERSION, Buff, 128) == TRUE)
		{
			m_FileVer = Buff;
		}
	}

	int Pos1 = m_ExePath.find_last_of(L"\\");
	int Pos2 = m_ExePath.find_last_of(L"/");
	std::wstring ExeName = m_ExePath.substr(max(Pos1, Pos2) + 1);

	_wcslwr((wchar_t *)ExeName.c_str());

	std::map<std::wstring, ExeInfo>::iterator it = _ExeInfo.find(ExeName.c_str());

	if (it != _ExeInfo.end())
	{
		m_StartupOrder = it->second.StartupOrder;
		m_Type = it->second.SrvType;
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CGameServer::SetStatus(int iVal)
{
	if (m_Status == iVal)
		return;

	m_Status = iVal;

	switch (m_Status)
	{
	case EM_SERVER_STATUS_NA:
		break;
	case EM_SERVER_STATUS_DISABLED:
		break;
	case EM_SERVER_STATUS_OFFLINE:
	{
		m_CustomStatus = 0;
		m_NetID = INVALID_NET_ID;
		m_Type = EM_GAME_SERVER_TYPE_NONE;
		m_SelfStop = false;
	}
	break;
	case EM_SERVER_STATUS_ONLINE:
		break;
	case EM_SERVER_STATUS_RESPONSE_WARNING:
		break;
	case EM_SERVER_STATUS_ERROR:
	{
		Controller::This()->SM_CriticalEvent(-1, m_Index, EM_CRITICAL_EVENT_CRASH, "Crashed!");

		Print(5, "", "[%s][%s][%s] %s", RegionNameANSI.c_str(), GroupNameANSI.c_str(), ServerNameANSI.c_str(), "Crashed");
	}
	break;
	case EM_SERVER_STATUS_STARTING:
	{
		Print(5, "", "[%s][%s][%s] %s", RegionNameANSI.c_str(), GroupNameANSI.c_str(), ServerNameANSI.c_str(), "starting");
	}
	break;
	case EM_SERVER_STATUS_STOPPING:
	{
		Print(5, "", "[%s][%s][%s] %s", RegionNameANSI.c_str(), GroupNameANSI.c_str(), ServerNameANSI.c_str(), "stopping");
	}
	break;
	case EM_SERVER_STATUS_CUSTOM_0:
	case EM_SERVER_STATUS_CUSTOM_1:
	case EM_SERVER_STATUS_CUSTOM_2:
	case EM_SERVER_STATUS_CUSTOM_3:
		break;
	case EM_SERVER_STATUS_NO_RESPONSE:
	{
		CreateDump(EM_CONSOLE_DUMP_MIN);

		Controller::This()->SM_CriticalEvent(-1, m_Index, EM_CRITICAL_EVENT_NO_RESPONSE, "No response!");

		Print(5, "", "[%s][%s][%s] %s", RegionNameANSI.c_str(), GroupNameANSI.c_str(), ServerNameANSI.c_str(), "No response!");
	}
	break;
	case EM_SERVER_STATUS_DISCONNECTED:
		break;
	}
}

#pragma managed(pop)