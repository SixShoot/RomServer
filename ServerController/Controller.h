#pragma once

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "IniFile/IniFile.h"
#include "NetBridge/NetBridge.h"
#include "smallobj/SmallObj.h"
#include "GameServer.h"
#include "Monitor.h"
#include <vector>
#include <map>

#define CTRL_CONFIG_FILE_NAME ".\\Config.ini"
#define CTRL_COMMAND_SAVE_INI "save ini"
#define CTRL_COMMAND_RELOAD_CONFIG "reload config"

#define INVALID_NET_ID 0xFFFFFFFF

#pragma managed(push, off)

//******************************************************************************************
class MonitorEventObj;
class GSEventObj;
class MemoryBlock;

//******************************************************************************************
#define PFUNC_NET_PG boost::function<void(unsigned long netid, unsigned long size, void *pg)>
#define PFUNC_KEY_STR boost::function<void(CGameServer * server, const char *str)>

//******************************************************************************************
class Controller
{
  protected:
	static Controller *_This;

  protected:
	IniFileClass m_Ini;
	std::string m_Country;
	int m_SysCpuUsage;
	int m_FreeMemory;
	int m_TotalMemory;
	std::wstring m_ComputerName;

	CNetBridge *m_NBMonitor;
	CNetBridge *m_NBGS;
	MonitorEventObj *m_MonitorEventObj;
	GSEventObj *m_GSEventObj;
	std::vector<PFUNC_NET_PG> m_PGFuncs;

	MemoryBlock *m_PacketBuffer;
	MemoryBlock *m_StringBuffer;
	MemoryBlock *m_ZipBuffer;

	std::vector<CGameServer *> m_Servers;
	std::map<std::wstring, CGameServer *> m_NameIndex;
	std::map<unsigned long, CGameServer *> m_NetIDIndex;
	std::map<std::string, PFUNC_KEY_STR> m_KeyStrFuncs;
	std::map<unsigned long, CMonitor *> m_Monitors;

  public:
	static void Init();
	static void Release();
	static void Process(unsigned long nowtick, unsigned long elapsedtime);

  public:
	static Controller *This() { return _This; }

  public:
	Controller();
	virtual ~Controller();

  protected:
	void _Process(unsigned long nowtick, unsigned long elapsedtime);

  protected:
	void InitPGFuncs();
	void InitKeyStrFunc();
	void RegKeyStrFunc(const char *key, PFUNC_KEY_STR pf);
	void ReadIniFile();

  public:
	void SendToMonitor(unsigned long netid, unsigned long size, void *pg);
	void SendToGS(unsigned long netid, unsigned long size, void *pg);

  public:
	void *GetPacketBuffer(unsigned long size);
	char *GetStringBuffer(unsigned long size);
	void *GetZipBuffer(unsigned long size);

  protected:
	void LoadConfig();
	void SaveConfig();
	void SaveConfigNew();
	CGameServer *AddGameServer(const wchar_t *regionname, const wchar_t *groupname, const wchar_t *servername, const wchar_t *filename, const wchar_t *cmdline, bool disable, bool attached = false);
	void RemoveGameServer(CGameServer *pdesc);
	CGameServer *GetServerAt(int index);
	CGameServer *GetServerByName(const wchar_t *regionname, const wchar_t *groupname, const wchar_t *servername);
	CGameServer *GetServerByNetID(unsigned long netid);
	CGameServer *GetServerByPID(unsigned long pid);

  protected:
	CMonitor *AddMonitor(unsigned long netid);
	void RemoveMonitor(unsigned long netid);

  protected:
	int _OnTimeProcCensus(SchedularInfo *obj, void *inputclass);

  public:
	void SGS_Connected(unsigned long netid);
	void SGS_Notification(unsigned long netid, const char *regionname, const char *groupname, const char *servername);
	void SGS_Command(unsigned long netid, const char *command, const char *servername);

	void SM_ServerInfo(unsigned long netid, std::vector<CGameServer *> &servers);
	void SM_ClearServerInfo(unsigned long netid);
	void SM_ConsoleOutput(unsigned long netid, int index, const char *output);
	void SM_ServerStatsu(unsigned long netid);
	void SM_MemUsage(unsigned long netid);
	void SM_CpuUsage(unsigned long netid);
	void SM_ControllerStopping(unsigned long netid);
	void SM_CriticalEvent(unsigned long netid, int index, int eventtype, const char *message);
	void SM_Census(unsigned long netid);
	void SM_WaitCount(unsigned long netid, int index, int waitingcount, int loadingcount);
	void SM_DynamicCommands(unsigned long netid);
	void SM_Aux(unsigned long netid, int index, int aux, int value);
	void SM_InfoOnStartUp(unsigned long netid, int index, const char *fileversion);
	void SM_PopupMessage(unsigned long netid, const char *message);

  protected:
	void _ON_MSG_DEFAULT(CGameServer *pserver, const char *output);
	void _ON_MSG_SERVER_READY(CGameServer *pserver, const char *output);
	void _ON_MSG_SET_CUSTOM_STATE(CGameServer *pserver, const char *output);
	void _ON_MSG_SERVER_ERROR(CGameServer *pserver, const char *output);
	void _ON_MSG_WORLD_CENSUS(CGameServer *pserver, const char *output);
	void _ON_MSG_ZONE_CENSUS(CGameServer *pserver, const char *output);
	void _ON_MSG_ADD_COMMAND(CGameServer *pserver, const char *output);
	void _ON_MSG_ADD_INPUT_COMMAND(CGameServer *pserver, const char *output);
	void _ON_MSG_REMOVE_COMMAND(CGameServer *pserver, const char *output);
	void _ON_MSG_ENABLE_COMMAND(CGameServer *pserver, const char *output);
	void _ON_MSG_DISENABLE_COMMAND(CGameServer *pserver, const char *output);
	void _ON_MSG_WORLD_ID(CGameServer *pserver, const char *output);
	void _ON_MSG_WAIT_COUNT(CGameServer *pserver, const char *output);
	void _ON_MSG_AUX1(CGameServer *pserver, const char *output);
	void _ON_MSG_AUX2(CGameServer *pserver, const char *output);

  protected:
	void On_PG_MtoC_START_SERVER(int index);
	void On_PG_MtoC_STOP_SERVER(int index);
	void On_PG_MtoC_SERVER_COMMAND(int index, const char *command);
	void On_PG_MtoC_SET_CUSTOM_STATUS(int index, int status, const char *str);
	void On_PG_MtoC_ENABLE_SERVER_OUTPUT(unsigned long netid, int index);
	void On_PG_MtoC_RELOAD_CONFIG();
	void On_PG_MtoC_ENABLE_SERVER(int index, int enable);
	void On_PG_MtoC_STOP_ALL_SERVER_NOTIFICATION();
	void On_PG_MtoC_REQUEST_SERVER_STATUS(unsigned long netid);
	void On_PG_MtoC_REQUEST_CPU_USAGE(unsigned long netid);
	void On_PG_MtoC_REQUEST_MEM_USAGE(unsigned long netid);
	void On_PG_MtoC_REQUEST_DYNAMIC_COMMANDS(unsigned long netid);
	void On_PG_MtoC_REQUEST_CENSUS(unsigned long netid);
	void On_PG_MtoC_CREATE_DUMP(unsigned long netid, int index, int dumptype);
	void On_PG_MtoC_RESET_SERVER(int index);

	void On_PG_StoC_REGISTRATION(unsigned long netid, unsigned long pid, const char *regionname, const char *groupname, const char *servername, const char *cmdline);
	void On_PG_StoC_CONSOLE_OUTPUT(unsigned long netid, const char *output);
	void On_PG_StoC_SERVER_TYPE(unsigned long netid, int type);
	void On_PG_StoC_SERVER_READY(unsigned long netid);

  protected:
	void _PG_MtoC_START_SERVER(unsigned long netid, unsigned long size, void *pg);
	void _PG_MtoC_STOP_SERVER(unsigned long netid, unsigned long size, void *pg);
	void _PG_MtoC_SERVER_COMMAND(unsigned long netid, unsigned long size, void *pg);
	void _PG_MtoC_SET_CUSTOM_STATUS(unsigned long netid, unsigned long size, void *pg);
	void _PG_MtoC_ENABLE_SERVER_OUTPUT(unsigned long netid, unsigned long size, void *pg);
	void _PG_MtoC_RELOAD_CONFIG(unsigned long netid, unsigned long size, void *pg);
	void _PG_MtoC_ENABLE_SERVER(unsigned long netid, unsigned long size, void *pg);
	void _PG_MtoC_STOP_ALL_SERVER_NOTIFICATION(unsigned long netid, unsigned long size, void *pg);
	void _PG_MtoC_REQUEST_SERVER_STATUS(unsigned long netid, unsigned long size, void *pg);
	void _PG_MtoC_REQUEST_CPU_USAGE(unsigned long netid, unsigned long size, void *pg);
	void _PG_MtoC_REQUEST_MEM_USAGE(unsigned long netid, unsigned long size, void *pg);
	void _PG_MtoC_REQUEST_DYNAMIC_COMMANDS(unsigned long netid, unsigned long size, void *pg);
	void _PG_MtoC_REQUEST_CENSUS(unsigned long netid, unsigned long size, void *pg);
	void _PG_MtoC_CREATE_DUMP(unsigned long netid, unsigned long size, void *pg);
	void _PG_MtoC_RESET_SERVER(unsigned long netid, unsigned long size, void *pg);

	void _PG_StoC_REGISTRATION(unsigned long netid, unsigned long size, void *pg);
	void _PG_StoC_CONSOLE_OUTPUT(unsigned long netid, unsigned long size, void *pg);
	void _PG_StoC_COMMAND_TO_CONTROLLER(unsigned long netid, unsigned long size, void *pg);
	void _PG_StoC_SERVER_TYPE(unsigned long netid, unsigned long size, void *pg);
	void _PG_StoC_SERVER_READY(unsigned long netid, unsigned long size, void *pg);

  public:
	bool _OnRecv(unsigned long netid, unsigned long size, void *pg);
	void _OnMonitorConnected(unsigned long netid);
	void _OnMonitorDisconnected(unsigned long netid);
	void _OnGSConnected(unsigned long netid);
	void _OnGSDisconnected(unsigned long netid);

  public:
	static void _OnNBMonitorError(int lv, const char *msg);
	static void _OnNBGSError(int lv, const char *msg);
};

//******************************************************************************************
class MonitorEventObj : public CEventObj
{
  public:
	MonitorEventObj() {}
	virtual ~MonitorEventObj() {}

	virtual bool OnRecv(unsigned long netid, unsigned long size, void *data);
	virtual void OnConnectFailed(unsigned long netid, unsigned long failedcode);
	virtual void OnConnect(unsigned long netid);
	virtual void OnDisconnect(unsigned long netid);
	virtual CEventObj *OnAccept(unsigned long netid);
};

//******************************************************************************************
class GSEventObj : public CEventObj
{
  public:
	GSEventObj() {}
	virtual ~GSEventObj() {}

	virtual bool OnRecv(unsigned long netid, unsigned long size, void *data);
	virtual void OnConnectFailed(unsigned long netid, unsigned long failedcode);
	virtual void OnConnect(unsigned long netid);
	virtual void OnDisconnect(unsigned long netid);
	virtual CEventObj *OnAccept(unsigned long netid);
};
//******************************************************************************************
class MemoryBlock
{
  private:
	void *m_Memory;
	unsigned long m_Size;

  public:
	MemoryBlock(unsigned long size = 0);
	virtual ~MemoryBlock();

  public:
	void Clear();

  public:
	void *GetMemory() { return m_Memory; }
	unsigned long GetSize() { return m_Size; }
	void SetSize(unsigned long val);

  public:
	__declspec(property(get = GetMemory)) void *Memory;
	__declspec(property(get = GetSize, put = SetSize)) unsigned long Size;
};
//******************************************************************************************

#pragma managed(pop)
#endif