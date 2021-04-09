#pragma once

#ifndef __GAME_SERVER_DESCRIPTION_H__
#define __GAME_SERVER_DESCRIPTION_H__

#include <map>
#include <vector>
#include "controllerclient/PG_Controller.h"
#include "functionschedular/functionschedular.h"
#include "DbgHelp.h"

#pragma managed(push, off)

//***********************************************************************************************
class CGameServerConsole;
class CPerformanceCounter;
class CGameServer;

//***********************************************************************************************

//***********************************************************************************************
enum GSConsoleStatusENUM
{
	EM_CONSOLE_STATUS_IDLE			= 0,	
	EM_CONSOLE_STATUS_RUNNING		,
	EM_CONSOLE_STATUS_STOPPING		,
	EM_CONSOLE_STATUS_START_FAILED	,
	EM_CONSOLE_STATUS_RUNTIME_ERROR
};
//***********************************************************************************************
struct ExeInfo
{
	int	StartupOrder;
	int	SrvType;

	ExeInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

//***********************************************************************************************
class MyConsole
{
protected:
	int					m_Status;
	unsigned long		m_TTL;
	PROCESS_INFORMATION	m_Info;	
	unsigned long		m_ProcessorCount;
	INT64				m_LastKernelTime;
	INT64				m_LastUserTime;
	unsigned long		m_LastElapsedTime;
	int					m_CpuUsage;
	int					m_MemoryUsage;
	bool                m_Attached;

public:
	MyConsole();
	virtual	~MyConsole();

public:
	void Process(unsigned long nowtick, unsigned long elapsedtime);
	bool Start(const wchar_t* filepath, const wchar_t* cmdline, bool windowed = false);
	bool Start(unsigned long pid, bool attached = false);
	void Stop(unsigned long ttl);
	void Reset(unsigned long pid);
	bool CreateDump(const wchar_t* filename, unsigned long dumptype = EM_CONSOLE_DUMP_MIN);

protected:
	void CloseHandles();
	bool Terminate(bool forcibly = false);
	bool IsProcessAlive();
	void UpdateCpuUsage(unsigned long elapsedtime);
	void UpdateMemUsage();

public:
	unsigned long GetPID()	   {return m_Info.dwProcessId;}
	int			  GetStatus()  {return m_Status;}
	int			  GetMemUsage(){return m_MemoryUsage;}
	int			  GetCpuUsage(){return m_CpuUsage;}

public:
	__declspec(property(get = GetPID	 ))	unsigned long PID;
	__declspec(property(get = GetStatus	 ))	int			  Status;
	__declspec(property(get = GetMemUsage))	int			  MemUsage;
	__declspec(property(get = GetCpuUsage)) int			  CpuUsage;
};

//***********************************************************************************************
class CGameServer
{
protected:
	static std::map<std::wstring, ExeInfo> _ExeInfo;

protected:
	MyConsole	  m_Console;

	int			  m_Index;
	std::wstring  m_RegionName;
	std::wstring  m_GroupName;
	std::wstring  m_ServerName;
	std::wstring  m_ExePath;
	std::wstring  m_TempExePath;
	std::wstring  m_CmdLine;
	std::string	  m_FileVer;
	std::string	  m_CustomStatusString;

	bool		  m_Disabled;
	int			  m_StartupOrder;
	bool		  m_Attached;
	int			  m_WorldID;

	int			  m_Status;
	int			  m_CustomStatus;
	unsigned long m_NetID;
	int			  m_Type;
	int			  m_Census;

	//Ping control
	unsigned long m_LastResponseTick;

	//self stop flag
	bool		  m_SelfStop;

	//commands
	std::vector<GSCommand*>			 m_Commands;
	std::map<std::string, GSCommand> m_CommandsMap;

public:
	static void Init();
	static void RegisterExe(const wchar_t* name, int order, int type);

public:
	CGameServer();

public:
	void Process(unsigned long nowtick, unsigned long elapsedtime);

public:
	bool Start();
	bool Start(unsigned long pid);
	bool Stop();
	bool Reset();
	bool CreateDump(int dumptype);
	bool DeleteTempExeFile();

public:
	void AddCommand(const char* caption, const char* command, bool inputrequired, bool enabled);
	void RemoveCommand(const char* caption);
	void EnableCommand(const char* caption);
	void DisableCommand(const char* caption);

public:
	void UpdateResponseTick();	

public:
	int			   GetIndex()							 {return m_Index;}
	void		   SetIndex(int val)					 {m_Index = val;}
	const wchar_t* GetRegionName()						 {return m_RegionName.c_str();}
	void		   SetRegionName(const wchar_t* val)	 {m_RegionName = val;}
	std::string	   GetRegionNameANSI();
	const wchar_t* GetGroupName()					     {return m_GroupName.c_str();}
	void		   SetGroupName(const wchar_t* val)		 {m_GroupName = val;}
	std::string	   GetGroupNameANSI();
	const wchar_t* GetServerName()						 {return m_ServerName.c_str();}
	void		   SetServerName(const wchar_t* val)	 {m_ServerName = val;}
	std::string	   GetServerNameANSI();
	const wchar_t* GetExePath()							 {return m_ExePath.c_str();}
	std::string	   GetExePathANSI();
	void		   SetExePath(const wchar_t* val);	
	const wchar_t* GetCmdLine()							 {return m_CmdLine.c_str();}
	void		   SetCmdLine(const wchar_t* val)		 {m_CmdLine = val;}
	const char*	   GetExeFileVer()						 {return m_FileVer.c_str();}
	const char*	   GetCustomStatusString()				 {return m_CustomStatusString.c_str();}
	void		   SetCustomStatusString(const char* val){m_CustomStatusString = val;}
	bool		   GetDisabled()						 {return m_Disabled;}
	void		   SetDisabled(bool val)				 {m_Disabled = val;}
	int			   GetStartupOrder()					 {return m_StartupOrder;}
	bool		   GetAttached()						 {return m_Attached;}
	void		   SetAttached(bool val)				 {m_Attached = val;}
	int			   GetStatus()							 {return m_Status;}
	void		   SetStatus(int val);
	int			   GetCustomStatus()					 {return m_CustomStatus;}
	void		   SetCustomStatus(int val)				 {m_CustomStatus = val;}
	unsigned long  GetNetID()							 {return m_NetID;}
	void		   SetNetID(unsigned long val)			 {m_NetID = val;}
	int			   GetWorldID()							 {return m_WorldID;}
	void		   SetWorldID(int val)					 {m_WorldID = val;}	
	int			   GetServerType()						 {return m_Type;}
	void		   SetServerType(int val)				 {m_Type = val;}
	int			   GetCensus()							 {return m_Census;}
	void		   SetCensus(int val)					 {m_Census = val;}
	unsigned long  GetPID()								 {return m_Console.PID;}
	int			   GetMemUsage()						 {return m_Console.MemUsage;}
	int			   GetCpuUsage()						 {return m_Console.CpuUsage;}
	bool		   GetSelfStop()						 {return m_SelfStop;}
	void		   SetSelfStop(bool val)				 {m_SelfStop = val;}
	std::vector<GSCommand*>& GetCommands()				 {return m_Commands;}

public:
	__declspec(property(get = GetIndex				,put = SetIndex				)) int			  Index;
	__declspec(property(get = GetRegionName			,put = SetRegionName		)) const wchar_t* RegionName;
	__declspec(property(get = GetRegionNameANSI									)) std::string	  RegionNameANSI;
	__declspec(property(get = GetGroupName			,put = SetGroupName			)) const wchar_t* GroupName;
	__declspec(property(get = GetGroupNameANSI									)) std::string	  GroupNameANSI;
	__declspec(property(get = GetServerName			,put = SetServerName		)) const wchar_t* ServerName;
	__declspec(property(get = GetServerNameANSI									)) std::string	  ServerNameANSI;
	__declspec(property(get = GetExePath			,put = SetExePath			)) const wchar_t* ExePath;
	__declspec(property(get = GetExePathANSI									)) std::string	  ExePathANSI;
	__declspec(property(get = GetCmdLine			,put = SetCmdLine 			)) const wchar_t* CmdLine;
	__declspec(property(get = GetExeFileVer										)) const char*	  ExeFileVer;
	__declspec(property(get = GetCustomStatusString ,put = SetCustomStatusString)) const char*	  CustomStatusString;
	__declspec(property(get = GetDisabled			,put = SetDisabled			)) bool			  Disabled;
	__declspec(property(get = GetStartupOrder									)) int			  StartupOrder;
	__declspec(property(get = GetAttached			,put = SetAttached			)) bool			  Attached;
	__declspec(property(get = GetStatus				,put = SetStatus			)) int			  Status;
	__declspec(property(get = GetCustomStatus		,put = SetCustomStatus		)) int			  CustomStatus;
	__declspec(property(get = GetNetID				,put = SetNetID				)) unsigned long  NetID;
	__declspec(property(get = GetWorldID			,put = SetWorldID			)) int			  WorldID;
	__declspec(property(get = GetServerType			,put = SetServerType		)) int			  Type;
	__declspec(property(get = GetCensus				,put = SetCensus			)) int			  Census;
	__declspec(property(get = GetPID											)) unsigned long  PID;
	__declspec(property(get = GetMemUsage										)) int			  MemUsage;
	__declspec(property(get = GetCpuUsage										)) int			  CpuUsage;
	__declspec(property(get = GetSelfStop			,put = SetSelfStop			)) bool			  SelfStop;
	__declspec(property(get = GetCommands										)) std::vector<GSCommand*>& Commands;
};

//***********************************************************************************************

#pragma managed(pop)
#endif