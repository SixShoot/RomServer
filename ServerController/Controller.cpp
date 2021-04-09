#include "Controller.h"
#include "debuglog/errormessage.h"
#include "controllerclient/PG_Controller.h"
#include <psapi.h>
#include "zlib/zlib.h"
#include "functionschedular/functionschedular.h"
#include "ClrUtils.h"
#include "iostream"
#include <io.h>
#include <fcntl.h>
#include "ControllerXML.h"
#include <fstream>
#include <string>
//***********************************************************************************************
#define SERVER_COMMAND_CHANGEPARALLEL "ChangeParallel"
#define SERVER_COMMAND_CENSUS "Census"

#define MONITOR_SERVICE_PORT 36579
#define GS_SERVICE_PORT 36580

// String symbol define
#define KEY_STR_DEFAULT "Default"
#define KEY_STR_SERVER_READY "SERVER READY IDSTR = {6EC9D721-D946-4906-AB79-5AF7B35241CB}"
#define KEY_STR_SET_CUSTOM_STATE "Set Custom State {79CC1671-A46D-478a-9497-4933581B5B03}"
#define KEY_STR_SERVER_ERROR "Server Error {FE58BC7B-ECE9-4d9a-85D0-ED6433ACAF6D}"
#define KEY_STR_WORLD_CENSUS "World Census {8B1B5C78-AB81-4c0f-B0EF-A380269E44C5}"
#define KEY_STR_ZONE_CENSUS "Zone Census {79CC1671-A46D-478a-9497-4933581B5B03}"
#define KEY_STR_ADD_COMMAND "Add Command {79CC1671-A46D-478a-9497-4933581B5B03}"
#define KEY_STR_ADD_INPUT_COMMAND "Add Input Command {79CC1671-A46D-478a-9497-4933581B5B03}"
#define KEY_STR_REMOVE_COMMAND "Remove Command {79CC1671-A46D-478a-9497-4933581B5B03}"
#define KEY_STR_ENABLE_COMMAND "Enable Command {79CC1671-A46D-478a-9497-4933581B5B03}"
#define KEY_STR_DISABLE_COMMAND "Disable Command {79CC1671-A46D-478a-9497-4933581B5B03}"
#define KEY_STR_WORLD_ID "World ID {8E3FD4DA-BF63-4C6C-AA7D-013218CED9CA}"
#define KEY_STR_WAIT_COUNT "WaitCount {7E52C067-80A5-42AA-AA4D-6D25469B3C00}"
#define KEY_STR_AUX_1 "Aux1 {E9E41FAF-A7D0-4762-8446-C8E85CDC5065}"
#define KEY_STR_AUX_2 "Aux2 {E4D780A7-873B-4888-8133-023DBCD680B4}"

//***********************************************************************************************

#pragma managed(push, off)

//***********************************************************************************************
Controller *Controller::_This = NULL;

//***********************************************************************************************
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void _Trim(std::string &str, char keyword)
{
	std::string::size_type pos = str.find_last_not_of(keyword);
	if (pos != std::string::npos)
	{
		str.erase(pos + 1);
	}

	pos = str.find_first_not_of(keyword);

	if (pos != std::string::npos)
	{
		str.erase(0, pos);
	}
}

string WtoS(const wchar_t *text)
{
	wstring ws(text);
	string str(ws.begin(), ws.end());
	return str;
}

wstring StoW(string str)
{
	wstring wstr(str.begin(), str.end());
	return wstr;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void _Trim(std::wstring &str, wchar_t keyword)
{
	std::wstring::size_type pos = str.find_last_not_of(keyword);
	if (pos != std::wstring::npos)
	{
		str.erase(pos + 1);
	}

	pos = str.find_first_not_of(keyword);

	if (pos != std::wstring::npos)
	{
		str.erase(0, pos);
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void _StringTokenize(const char *str, const char *splitor, std::vector<std::string> &tokens)
{
	int Len = strlen(str) + 1;
	char *Buff = new char[Len];
	memset(Buff, 0, Len);
	strcpy(Buff, str);

	char *Token = NULL;
	tokens.clear();

	Token = strtok(Buff, splitor);
	while (Token != NULL)
	{
		tokens.push_back(Token);
		Token = strtok(NULL, splitor);
	}

	delete Buff;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void _StringTokenize(const wchar_t *str, const wchar_t *splitor, std::vector<std::wstring> &tokens)
{
	int Len = wcslen(str) + 1;
	wchar_t *Buff = new wchar_t[Len];
	memset(Buff, 0, (Len * sizeof(wchar_t)));

	wcscpy(Buff, str);

	wchar_t *Token = NULL;
	tokens.clear();

	Token = wcstok(Buff, splitor);
	while (Token != NULL)
	{
		tokens.push_back(Token);
		Token = wcstok(NULL, splitor);
	}

	delete Buff;
}
//***********************************************************************************************

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::Init()
{
	Print(LV2, "Controller", "Controller::Init()");
	CGameServer::Init();

	if (_This == NULL)
	{
		Print(LV2, "Controller", "if (_This == NULL)");
		_This = new Controller();
	}

	Print(LV2, "Controller", "Init Ok.");
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::Release()
{
	if (_This != NULL)
	{
		_This->SM_ControllerStopping(-1);
		CNetBridge::WaitAll(10);

		delete _This;
		_This = NULL;
	}
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::Process(unsigned long nowtick, unsigned long elapsedtime)
{
	if (_This != NULL)
	{
		_This->_Process(nowtick, elapsedtime);
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
Controller::Controller()
{
	Print(LV2, "Controller", "Controller::Controller()");
	m_Ini.SetIniFile(CTRL_CONFIG_FILE_NAME);
	m_Country = m_Ini.Str("Country");
	Print(LV2, "Controller", "country %s", m_Country.c_str());
	//------------------------------------
	//8KB buffer
	m_PacketBuffer = new MemoryBlock(8 * 1024);

	//512 byte buffer
	m_StringBuffer = new MemoryBlock(512);

	//4KB buffer
	m_ZipBuffer = new MemoryBlock(4 * 1024);
	//------------------------------------

	//------------------------------------
	//Get Local Computer Name
	Print(LV2, "Controller", "Get Local Computer Name");
	unsigned long dwBufSize = MAX_COMPUTERNAME_LENGTH + 1;
	wchar_t *Buff = (wchar_t *)GetStringBuffer(dwBufSize * sizeof(wchar_t));
	if (GetComputerNameW(Buff, &dwBufSize))
	{
		m_ComputerName = Buff;
	}
	//------------------------------------

	//------------------------------------
	Print(LV2, "Controller", "new MonitorEventObj()");
	m_MonitorEventObj = new MonitorEventObj();
	m_GSEventObj = new GSEventObj();

	m_NBMonitor = CreateNetBridge();
	m_NBMonitor->Host(0, MONITOR_SERVICE_PORT, m_MonitorEventObj, FALSE, FALSE);

	m_NBGS = CreateNetBridge();
	m_NBGS->Host(0, GS_SERVICE_PORT, m_GSEventObj, FALSE, FALSE);

	m_NBMonitor->SetOutPutCB(&Controller::_OnNBMonitorError);
	m_NBGS->SetOutPutCB(&Controller::_OnNBGSError);
	//------------------------------------

	//------------------------------------
	InitPGFuncs();
	Print(LV2, "Controller", "InitPGFuncs()");
	InitKeyStrFunc();
	Print(LV2, "Controller", "InitKeyStrFunc()");
	//------------------------------------

	//------------------------------------
	g_Schedular.Push(boost::bind<int>(&Controller::_OnTimeProcCensus, this, _1, _2), 3000, NULL, NULL);
	Print(LV2, "Controller", "g_Schedular.Push(boost::bind<int>(&Controller::_OnTimeProcCensus, this, _1, _2), 3000, NULL, NULL)");
	//------------------------------------

	//------------------------------------
	//load config
	LoadConfig();
	Print(LV2, "Controller", "LoadConfig()");
	//------------------------------------
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
Controller::~Controller()
{
	DeleteNetBridge(m_NBMonitor);
	DeleteNetBridge(m_NBGS);
	delete m_MonitorEventObj;
	delete m_GSEventObj;

	for (int i = 0; i < (int)m_Servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];
		if (pServer != NULL)
		{
			delete pServer;
		}
	}

	delete m_PacketBuffer;
	delete m_StringBuffer;
	delete m_ZipBuffer;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::_Process(unsigned long nowtick, unsigned long elapsedtime)
{
	// Iterate through all registered servers
	for (int i = 0; i < (int)m_Servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];

		if (pServer != NULL)
		{
			pServer->Process(nowtick, elapsedtime);
		}
	}

	// Iterate through all connected monitors
	for (std::map<unsigned long, CMonitor *>::iterator it = m_Monitors.begin(); it != m_Monitors.end(); it++)
	{
		CMonitor *pMonitor = it->second;
		if (pMonitor != NULL)
		{
			pMonitor->Process(nowtick, elapsedtime);
		}
	}

	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	m_FreeMemory = (int)(statex.ullAvailPhys * 1.0 / (1024 * 1024));
	m_TotalMemory = (int)(statex.ullTotalPhys * 1.0 / (1024 * 1024));
	m_SysCpuUsage = Runewaker::CLR_GetSysCpuUsage();
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::InitPGFuncs()
{
	for (int i = 0; i < EM_PG_MAX_PACKET_COUNT; ++i)
	{
		m_PGFuncs.push_back(NULL);
	}

	m_PGFuncs[EM_PG_MtoC_START_SERVER] = boost::bind<void>(&Controller::_PG_MtoC_START_SERVER, this, _1, _2, _3);
	m_PGFuncs[EM_PG_MtoC_STOP_SERVER] = boost::bind<void>(&Controller::_PG_MtoC_STOP_SERVER, this, _1, _2, _3);
	m_PGFuncs[EM_PG_MtoC_RESET_SERVER] = boost::bind<void>(&Controller::_PG_MtoC_RESET_SERVER, this, _1, _2, _3);
	m_PGFuncs[EM_PG_MtoC_SERVER_COMMAND] = boost::bind<void>(&Controller::_PG_MtoC_SERVER_COMMAND, this, _1, _2, _3);
	m_PGFuncs[EM_PG_MtoC_SET_CUSTOM_STATUS] = boost::bind<void>(&Controller::_PG_MtoC_SET_CUSTOM_STATUS, this, _1, _2, _3);
	m_PGFuncs[EM_PG_MtoC_ENABLE_SERVER_OUTPUT] = boost::bind<void>(&Controller::_PG_MtoC_ENABLE_SERVER_OUTPUT, this, _1, _2, _3);
	m_PGFuncs[EM_PG_MtoC_RELOAD_CONFIG] = boost::bind<void>(&Controller::_PG_MtoC_RELOAD_CONFIG, this, _1, _2, _3);
	m_PGFuncs[EM_PG_MtoC_ENABLE_SERVER] = boost::bind<void>(&Controller::_PG_MtoC_ENABLE_SERVER, this, _1, _2, _3);
	m_PGFuncs[EM_PG_MtoC_STOP_ALL_SERVER_NOTIFICATION] = boost::bind<void>(&Controller::_PG_MtoC_STOP_ALL_SERVER_NOTIFICATION, this, _1, _2, _3);
	m_PGFuncs[EM_PG_StoC_REGISTRATION] = boost::bind<void>(&Controller::_PG_StoC_REGISTRATION, this, _1, _2, _3);
	m_PGFuncs[EM_PG_StoC_CONSOLE_OUTPUT] = boost::bind<void>(&Controller::_PG_StoC_CONSOLE_OUTPUT, this, _1, _2, _3);
	m_PGFuncs[EM_PG_StoC_COMMAND_TO_CONTROLLER] = boost::bind<void>(&Controller::_PG_StoC_COMMAND_TO_CONTROLLER, this, _1, _2, _3);
	m_PGFuncs[EM_PG_StoC_SERVER_TYPE] = boost::bind<void>(&Controller::_PG_StoC_SERVER_TYPE, this, _1, _2, _3);
	m_PGFuncs[EM_PG_StoC_SERVER_READY] = boost::bind<void>(&Controller::_PG_StoC_SERVER_READY, this, _1, _2, _3);
	m_PGFuncs[EM_PG_MtoC_REQUEST_SERVER_STATUS] = boost::bind<void>(&Controller::_PG_MtoC_REQUEST_SERVER_STATUS, this, _1, _2, _3);
	m_PGFuncs[EM_PG_MtoC_REQUEST_CPU_USAGE] = boost::bind<void>(&Controller::_PG_MtoC_REQUEST_CPU_USAGE, this, _1, _2, _3);
	m_PGFuncs[EM_PG_MtoC_REQUEST_MEM_USAGE] = boost::bind<void>(&Controller::_PG_MtoC_REQUEST_MEM_USAGE, this, _1, _2, _3);
	m_PGFuncs[EM_PG_MtoC_REQUEST_DYNAMIC_COMMANDS] = boost::bind<void>(&Controller::_PG_MtoC_REQUEST_DYNAMIC_COMMANDS, this, _1, _2, _3);
	m_PGFuncs[EM_PG_MtoC_REQUEST_CENSUS] = boost::bind<void>(&Controller::_PG_MtoC_REQUEST_CENSUS, this, _1, _2, _3);
	m_PGFuncs[EM_PG_MtoC_CREATE_DUMP] = boost::bind<void>(&Controller::_PG_MtoC_CREATE_DUMP, this, _1, _2, _3);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::InitKeyStrFunc()
{
	//Initialize Server Descriptor call back function
	RegKeyStrFunc(KEY_STR_SERVER_READY, boost::bind<void>(&Controller::_ON_MSG_SERVER_READY, this, _1, _2));
	RegKeyStrFunc(KEY_STR_SET_CUSTOM_STATE, boost::bind<void>(&Controller::_ON_MSG_SET_CUSTOM_STATE, this, _1, _2));
	RegKeyStrFunc(KEY_STR_SERVER_ERROR, boost::bind<void>(&Controller::_ON_MSG_SERVER_ERROR, this, _1, _2));
	RegKeyStrFunc(KEY_STR_WORLD_CENSUS, boost::bind<void>(&Controller::_ON_MSG_WORLD_CENSUS, this, _1, _2));
	RegKeyStrFunc(KEY_STR_ZONE_CENSUS, boost::bind<void>(&Controller::_ON_MSG_ZONE_CENSUS, this, _1, _2));
	RegKeyStrFunc(KEY_STR_ADD_COMMAND, boost::bind<void>(&Controller::_ON_MSG_ADD_COMMAND, this, _1, _2));
	RegKeyStrFunc(KEY_STR_ADD_INPUT_COMMAND, boost::bind<void>(&Controller::_ON_MSG_ADD_INPUT_COMMAND, this, _1, _2));
	RegKeyStrFunc(KEY_STR_REMOVE_COMMAND, boost::bind<void>(&Controller::_ON_MSG_REMOVE_COMMAND, this, _1, _2));
	RegKeyStrFunc(KEY_STR_ENABLE_COMMAND, boost::bind<void>(&Controller::_ON_MSG_ENABLE_COMMAND, this, _1, _2));
	RegKeyStrFunc(KEY_STR_DISABLE_COMMAND, boost::bind<void>(&Controller::_ON_MSG_DISENABLE_COMMAND, this, _1, _2));
	RegKeyStrFunc(KEY_STR_WORLD_ID, boost::bind<void>(&Controller::_ON_MSG_WORLD_ID, this, _1, _2));
	RegKeyStrFunc(KEY_STR_WAIT_COUNT, boost::bind<void>(&Controller::_ON_MSG_WAIT_COUNT, this, _1, _2));
	RegKeyStrFunc(KEY_STR_AUX_1, boost::bind<void>(&Controller::_ON_MSG_AUX1, this, _1, _2));
	RegKeyStrFunc(KEY_STR_AUX_2, boost::bind<void>(&Controller::_ON_MSG_AUX2, this, _1, _2));
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::RegKeyStrFunc(const char *key, PFUNC_KEY_STR pf)
{
	std::string Key = key;
	_strlwr((char *)Key.c_str());
	m_KeyStrFuncs[Key.c_str()] = pf;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SendToMonitor(unsigned long netid, unsigned long size, void *pg)
{
	unsigned long BoundSize = compressBound(size);
	unsigned long ZipSize = BoundSize;
	unsigned long SendSize = 0;

	unsigned long BuffSize = BoundSize + sizeof(unsigned long);

	void *pData = GetZipBuffer(BuffSize);

	PBYTE pTraveler = (PBYTE)pData;

	(*(PDWORD)pTraveler) = size;
	pTraveler += sizeof(unsigned long);

	if (compress(pTraveler, &ZipSize, (PBYTE)pg, size) == Z_OK)
	{
		SendSize = ZipSize + sizeof(unsigned long);

		if (netid == -1)
		{
			for (std::map<unsigned long, CMonitor *>::iterator it = m_Monitors.begin(); it != m_Monitors.end(); it++)
			{
				m_NBMonitor->Send(it->first, SendSize, pData);
			}
		}
		else
		{
			m_NBMonitor->Send(netid, SendSize, pData);
		}
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SendToGS(unsigned long netid, unsigned long size, void *pg)
{
	m_NBGS->Send(netid, size, pg);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void *Controller::GetPacketBuffer(unsigned long size)
{
	m_PacketBuffer->Size = size;
	return m_PacketBuffer->Memory;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
char *Controller::GetStringBuffer(unsigned long size)
{
	m_StringBuffer->Size = size;
	return (char *)m_StringBuffer->Memory;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void *Controller::GetZipBuffer(unsigned long size)
{
	m_ZipBuffer->Size = size;
	return m_ZipBuffer->Memory;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::LoadConfig()
{
	//SaveConfigNew();
	//Print(LV2, "LoadConfig", "LoadConfig()");
	//FILE *pFile = _wfopen(L".\\RAServerController.ini", L"r,ccs=UNICODE");

	//if (pFile == NULL)
	//{
	//	//Print(LV2, "LoadConfig", "if (pFile == NULL)");
	//	return;
	//}
	//std::vector<std::vector<wstring>> Configs;
	//std::vector<std::wstring> Tokens;

	//wchar_t *Line = (wchar_t *)GetStringBuffer(2048);

	//while (fgetws(Line, 1024, pFile) != NULL)
	//{
	//	Print(LV2, "LINE", "%ls", Line);
	//	Tokens.clear();
	//	_StringTokenize(Line, L"\t\"", Tokens);
	//	Configs.push_back(Tokens);
	//}

	//fclose(pFile);
	std::vector<CGameServer *> tmpServers;
	tmpServers.clear();
	//for (int i = 0; i < (int)Configs.size(); ++i)
	//{
	//	std::vector<std::wstring> &Paras = Configs[i];
	//	bool disabled = ((Paras.size() >= 6) && (_wcsicmp(Paras[5].c_str(), L"disabled") == 0));
	//	CGameServer *pServer = GetServerByName(Paras[0].c_str(), Paras[1].c_str(), Paras[2].c_str());
	//	if (pServer == NULL)
	//	{
	//		pServer = new CGameServer();
	//	}
	//	pServer->RegionName = Paras[0].c_str();
	//	pServer->GroupName = Paras[1].c_str();
	//	pServer->ServerName = Paras[2].c_str();
	//	pServer->ExePath = Paras[3].c_str();
	//	pServer->CmdLine = Paras[4].c_str();
	//	pServer->Disabled = disabled;
	//	tmpServers.push_back(pServer);
	//}
	std::locale::global(std::locale("rus"));
	std::ifstream fileXml(L".\\RAServerController.xml");
	if (!fileXml.is_open())
		return;
	std::string xmlData((std::istreambuf_iterator<char>(fileXml)), std::istreambuf_iterator<char>());
	ControllerXML *controllerXML = new ControllerXML;
	if (Serializable::fromXML(xmlData, controllerXML))
	{
		for (size_t i = 0; i < controllerXML->Servers.size(); i++)
		{
			ServerXML *serv = controllerXML->Servers.getItem(i);
			CGameServer *pServer = GetServerByName(StoW(serv->RegionName.value()).c_str(), StoW(serv->GroupName.value()).c_str(), StoW(serv->ServerName.value()).c_str());
			if (pServer == NULL)
			{
				pServer = new CGameServer();
			}
			WCHAR regionName[1024];
			int nLen = MultiByteToWideChar(CP_UTF8, 0, serv->RegionName.value().c_str(), -1, regionName, 1024);
			//pServer->RegionName = StoW(serv->RegionName.value()).c_str();
			pServer->RegionName = regionName;

			WCHAR groupName[1024];
			nLen = MultiByteToWideChar(CP_UTF8, 0, serv->GroupName.value().c_str(), -1, groupName, 1024);
			//pServer->GroupName = StoW(serv->GroupName.value()).c_str();
			pServer->GroupName = groupName;

			WCHAR serverName[1024];
			nLen = MultiByteToWideChar(CP_UTF8, 0, serv->ServerName.value().c_str(), -1, serverName, 1024);
			//pServer->ServerName = StoW(serv->ServerName.value()).c_str();
			pServer->ServerName = serverName;

			WCHAR exePath[1024];
			nLen = MultiByteToWideChar(CP_UTF8, 0, serv->ExePath.value().c_str(), -1, exePath, 1024);
			//pServer->ExePath = StoW(serv->ExePath.value()).c_str();
			pServer->ExePath = exePath;

			WCHAR cmdLine[1024];
			nLen = MultiByteToWideChar(CP_UTF8, 0, serv->CmdLine.value().c_str(), -1, cmdLine, 1024);
			//pServer->CmdLine = StoW(serv->CmdLine.value()).c_str();
			pServer->CmdLine = cmdLine;

			pServer->Disabled = serv->Disabled.value();
			tmpServers.push_back(pServer);
		}
	}
	fileXml.close();
	delete (controllerXML);

	for (int i = 0; i < (int)m_Servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];

		if (pServer == NULL)
			continue;

		std::vector<CGameServer *>::iterator it;
		it = find(tmpServers.begin(), tmpServers.end(), pServer);

		if (it == tmpServers.end())
		{
			delete pServer;
		}
	}
	//Print(LV2, "LoadConfig", "for (int i = 0; i < (int)m_Servers.size(); ++i)");
	//servers reloaded
	m_Servers.clear();
	m_Servers = tmpServers;

	//rebuild index
	m_NameIndex.clear();
	m_NetIDIndex.clear();

	for (int i = 0; i < (int)m_Servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];

		if (pServer == NULL)
			continue;

		pServer->Index = i;

		wchar_t *Buff = (wchar_t *)GetStringBuffer(512);

		swprintf(Buff, 256, L"%s_%s_%s", pServer->RegionName, pServer->GroupName, pServer->ServerName);
		//Print(LV2, "LoadConfig", "%ls", Buff);
		m_NameIndex[Buff] = pServer;

		if (pServer->NetID != INVALID_NET_ID)
		{
			m_NetIDIndex[pServer->NetID] = pServer;
		}
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::SaveConfig()
{
	FILE *pFile = _wfopen(L".\\RAServerController.ini", L"w,ccs=UNICODE");

	if (pFile == NULL)
		return;

	wchar_t *Buff = (wchar_t *)GetStringBuffer(2048);

	for (int i = 0; i < (int)m_Servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];

		if (pServer == NULL)
			continue;

		if (pServer->Attached)
			continue;

		if (pServer->Disabled)
		{
			swprintf(Buff, 1024, L"\"%s\"\t\"%s\"\t\"%s\"\t\"%s\"\t\"%s\"\t\"disabled\"\r\n", pServer->RegionName, pServer->GroupName, pServer->ServerName, pServer->ExePath, pServer->CmdLine);
		}
		else
		{
			swprintf(Buff, 1024, L"\"%s\"\t\"%s\"\t\"%s\"\t\"%s\"\t\"%s\"\r\n", pServer->RegionName, pServer->GroupName, pServer->ServerName, pServer->ExePath, pServer->CmdLine);
		}

		fputws(Buff, pFile);
	}

	fclose(pFile);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
CGameServer *Controller::AddGameServer(const wchar_t *regionname, const wchar_t *groupname, const wchar_t *servername, const wchar_t *filename, const wchar_t *cmdline, bool disable, bool attached)
{
	CGameServer *Result = new CGameServer();
	Result->RegionName = regionname;
	Result->GroupName = groupname;
	Result->ServerName = servername;
	Result->ExePath = filename;
	Result->CmdLine = cmdline;
	Result->Disabled = disable;
	Result->Attached = attached;

	if (attached == true)
	{
		Result->GroupName = m_ComputerName.c_str();
	}

	m_Servers.push_back(Result);

	Result->Index = m_Servers.size() - 1;

	wchar_t *Buff = (wchar_t *)GetStringBuffer(512);

	swprintf(Buff, 256, L"%s_%s_%s", regionname, groupname, servername);

	m_NameIndex[Buff] = Result;
	return Result;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::RemoveGameServer(CGameServer *pserver)
{
	if (pserver == NULL)
		return;

	//clear Name Index
	wchar_t *Buff = (wchar_t *)GetStringBuffer(512);

	swprintf(Buff, 256, L"%s_%s_%s", pserver->RegionName, pserver->GroupName, pserver->ServerName);

	std::map<std::wstring, CGameServer *>::iterator itName = m_NameIndex.find(Buff);

	if (itName != m_NameIndex.end())
	{
		m_NameIndex.erase(itName);
	}

	//clear NetID Index
	std::map<unsigned long, CGameServer *>::iterator itNetID = m_NetIDIndex.find(pserver->NetID);

	if (itNetID != m_NetIDIndex.end())
	{
		m_NetIDIndex.erase(itNetID);
	}

	//remove and delete server descriptor
	std::vector<CGameServer *>::iterator it;
	it = find(m_Servers.begin(), m_Servers.end(), pserver);

	if (it != m_Servers.end())
	{
		m_Servers.erase(it);
	}

	delete pserver;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
CGameServer *Controller::GetServerAt(int index)
{
	CGameServer *Result = NULL;

	if ((index >= 0) && (index < (int)m_Servers.size()))
	{
		Result = m_Servers[index];
	}
	return Result;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
CGameServer *Controller::GetServerByName(const wchar_t *regionname, const wchar_t *groupname, const wchar_t *servername)
{
	CGameServer *Result = NULL;

	wchar_t *Buff = (wchar_t *)GetStringBuffer(512);

	swprintf(Buff, 256, L"%s_%s_%s", regionname, groupname, servername);

	std::map<std::wstring, CGameServer *>::iterator it = m_NameIndex.find(Buff);

	if (it != m_NameIndex.end())
	{
		Result = it->second;
	}

	return Result;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
CGameServer *Controller::GetServerByNetID(unsigned long netid)
{
	CGameServer *Result = NULL;

	std::map<unsigned long, CGameServer *>::iterator it = m_NetIDIndex.find(netid);

	if (it != m_NetIDIndex.end())
	{
		Result = it->second;
	}

	return Result;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
CGameServer *Controller::GetServerByPID(unsigned long pid)
{
	CGameServer *Result = NULL;

	for (int i = 0; i < (int)m_Servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];

		if (pServer)
		{
			if (pServer->PID == pid)
			{
				Result = pServer;
				break;
			}
		}
	}

	return Result;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
CMonitor *Controller::AddMonitor(unsigned long netid)
{
	CMonitor *Result = NULL;

	std::map<unsigned long, CMonitor *>::iterator it = m_Monitors.find(netid);

	if (it == m_Monitors.end())
	{
		Result = new CMonitor(netid);

		m_Monitors[netid] = Result;
	}

	return Result;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::RemoveMonitor(unsigned long netid)
{
	std::map<unsigned long, CMonitor *>::iterator it = m_Monitors.find(netid);

	if (it != m_Monitors.end())
	{
		if (it->second)
		{
			delete it->second;
		}

		m_Monitors.erase(it);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
int Controller::_OnTimeProcCensus(SchedularInfo *obj, void *inputclass)
{
	for (int i = 0; i < (int)m_Servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];
		if (pServer != NULL)
		{
			SGS_Command(pServer->NetID, SERVER_COMMAND_CENSUS, pServer->ServerNameANSI.c_str());
		}
	}

	g_Schedular.Push(boost::bind<int>(&Controller::_OnTimeProcCensus, this, _1, _2), 3000, NULL, NULL);
	return 0;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SGS_Connected(unsigned long netid)
{
	PG_CtoS_CONNECTED Send;
	SendToGS(netid, sizeof(Send), &Send);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SGS_Notification(unsigned long netid, const char *regionname, const char *groupname, const char *servername)
{
	PG_CtoS_NOTIFICATION Send;
	strncpy(Send.RegionName, regionname, sizeof(Send.RegionName));
	strncpy(Send.GroupName, groupname, sizeof(Send.GroupName));
	strncpy(Send.ServerName, servername, sizeof(Send.ServerName));
	SendToGS(netid, sizeof(Send), &Send);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SGS_Command(unsigned long netid, const char *command, const char *servername)
{
	int Len = strlen(command);
	int Size = sizeof(PG_CtoS_COMMAND_TO_SERVER) + Len + 1;

	void *pData = GetPacketBuffer(Size);

	PG_CtoS_COMMAND_TO_SERVER *pHeader = (PG_CtoS_COMMAND_TO_SERVER *)pData;
	pHeader->PGID = EM_PG_CtoS_COMMAND_TO_SERVER;
	memcpy(pHeader->Key, UNIQUE_CTOS_KEY, UNIQUE_KEY_SIZE);
	pHeader->Len = Len;
	strncpy(pHeader->Cmd, command, Len);

	SendToGS(netid, Size, pData);

	if ((_stricmp("census", command) != 0) && (_stricmp("ping", command) != 0))
	{
		Print(5, "", "Send command %s to server %s", command, servername);
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SM_ServerInfo(unsigned long netid, std::vector<CGameServer *> &servers)
{
	unsigned long Size = sizeof(PG_CtoM_SERVER_INFO) + (sizeof(GSInfo) * servers.size());
	Size = ((Size / 4) + 1) * 4;

	void *pData = GetPacketBuffer(Size);
	PBYTE pTraveler = (PBYTE)pData;

	PG_CtoM_SERVER_INFO *pHeader = (PG_CtoM_SERVER_INFO *)pTraveler;
	pHeader->PGID = EM_PG_CtoM_SERVER_INFO;
	pHeader->Count = servers.size();

	for (int i = 0; i < (int)servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];

		if (pServer != NULL)
		{
			GSInfo *pInfo = &(pHeader->Info[i]);
			wcscpy(pInfo->RegionName, pServer->RegionName);
			wcscpy(pInfo->GroupName, pServer->GroupName);
			wcscpy(pInfo->ServerName, pServer->ServerName);
			wcscpy(pInfo->ComputerName, m_ComputerName.c_str());
			strcpy(pInfo->FileVersion, pServer->ExeFileVer);

			pInfo->Index = pServer->Index;
			pInfo->StartupOrder = pServer->StartupOrder;
			pInfo->WorldID = pServer->WorldID;
			pInfo->PID = pServer->PID;
			pInfo->Status = pServer->CustomStatus < pServer->Status ? pServer->Status : pServer->CustomStatus;
			pInfo->SrvType = pServer->Type;

			if (pServer->Disabled == true)
			{
				pInfo->Status = EM_SERVER_STATUS_DISABLED;
			}
		}
	}

	SendToMonitor(netid, Size, pData);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SM_ClearServerInfo(unsigned long netid)
{
	PG_CtoM_CLEAR_SERVER_INFO Send;

	SendToMonitor(netid, sizeof(Send), &Send);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SM_ConsoleOutput(unsigned long netid, int index, const char *output)
{
	unsigned long Size = sizeof(PG_CtoM_CONSOLE_OUTPUT) + sizeof(GSConsoleOutput) + strlen(output);

	Size = ((Size / 4) + 1) * 4;

	void *pData = GetPacketBuffer(Size);
	PBYTE pTraveler = (PBYTE)pData;

	PG_CtoM_CONSOLE_OUTPUT *pHeader = (PG_CtoM_CONSOLE_OUTPUT *)pData;
	pHeader->PGID = EM_PG_CtoM_CONSOLE_OUTPUT;
	pHeader->Index = index;
	pHeader->Count = 1;
	pHeader->Output[0].Len = strlen(output);
	strcpy(pHeader->Output[0].String, output);

	SendToMonitor(netid, Size, pData);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SM_ServerStatsu(unsigned long netid)
{
	unsigned long Size = sizeof(PG_CtoM_SERVER_STATUS) + (m_Servers.size() * sizeof(GSStatus));

	Size = ((Size / 4) + 1) * 4;

	void *pData = GetPacketBuffer(Size);
	PBYTE pTraveler = (PBYTE)pData;

	PG_CtoM_SERVER_STATUS *pHeader = (PG_CtoM_SERVER_STATUS *)pTraveler;
	pHeader->PGID = EM_PG_CtoM_SERVER_STATUS;
	pHeader->Count = m_Servers.size();

	for (int i = 0; i < (int)m_Servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];

		if (pServer != NULL)
		{
			GSStatus *pStatus = &pHeader->Status[i];
			pStatus->Index = pServer->Index;
			pStatus->WorldID = pServer->WorldID;
			pStatus->PID = pServer->PID;
			pStatus->SrvType = pServer->Type;
			pStatus->Status = pServer->CustomStatus < pServer->Status ? pServer->Status : pServer->CustomStatus;
			strncpy(pStatus->CustomStatusString, pServer->CustomStatusString, sizeof(pStatus->CustomStatusString));

			if (pServer->Disabled)
			{
				pStatus->Status = EM_SERVER_STATUS_DISABLED;
			}
		}
	}

	SendToMonitor(netid, Size, pData);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SM_MemUsage(unsigned long netid)
{
	unsigned long Size = sizeof(PG_CtoM_MEM_USAGE) + (m_Servers.size() * sizeof(GSMemUsage));
	Size = ((Size / 4) + 1) * 4;

	void *pData = GetPacketBuffer(Size);
	PBYTE pTraveler = (PBYTE)pData;

	PG_CtoM_MEM_USAGE *pHeader = (PG_CtoM_MEM_USAGE *)pTraveler;
	pHeader->PGID = EM_PG_CtoM_MEM_USAGE;
	pHeader->Count = m_Servers.size();

	for (int i = 0; i < (int)m_Servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];
		if (pServer != NULL)
		{
			GSMemUsage *pUsage = &pHeader->Usage[i];
			pUsage->Index = i;
			pUsage->TotalMemory = m_TotalMemory;
			pUsage->FreeMemory = m_FreeMemory;
			pUsage->Usage = pServer->MemUsage;
		}
	}

	SendToMonitor(netid, Size, pData);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SM_CpuUsage(unsigned long netid)
{
	unsigned long Size = sizeof(PG_CtoM_CPU_USAGE) + (m_Servers.size() * sizeof(GSCpuUsage));
	Size = ((Size / 4) + 1) * 4;

	void *pData = GetPacketBuffer(Size);
	PBYTE pTraveler = (PBYTE)pData;

	PG_CtoM_CPU_USAGE *pHeader = (PG_CtoM_CPU_USAGE *)pTraveler;
	pHeader->PGID = EM_PG_CtoM_CPU_USAGE;
	pHeader->Count = m_Servers.size();

	for (int i = 0; i < (int)m_Servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];

		if (pServer != NULL)
		{
			GSCpuUsage *pUsage = &pHeader->Usage[i];
			pUsage->Index = i;
			pUsage->SystemWide = m_SysCpuUsage;
			pUsage->Usage = pServer->CpuUsage;
		}
	}

	SendToMonitor(netid, Size, pData);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SM_ControllerStopping(unsigned long netid)
{
	PG_CtoM_CONTROLLER_STOPPING Send;

	SendToMonitor(netid, sizeof(Send), &Send);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SM_CriticalEvent(unsigned long netid, int index, int eventtype, const char *message)
{
	int Len = strlen(message) + 1;
	unsigned long Size = sizeof(PG_CtoM_CRITICAL_EVENT) + Len;
	Size = ((Size / 4) + 1) * 4;

	void *pData = GetPacketBuffer(Size);
	PBYTE pTraveler = (PBYTE)pData;

	PG_CtoM_CRITICAL_EVENT *pHeader = (PG_CtoM_CRITICAL_EVENT *)pData;

	pHeader->PGID = EM_PG_CtoM_CRITICAL_EVENT;
	pHeader->Index = index;
	pHeader->EventType = eventtype;
	strcpy(pHeader->Time, TimeStr::Now());
	strcpy(pHeader->Message, message);

	SendToMonitor(netid, Size, pData);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SM_Census(unsigned long netid)
{
	//-----------------------------------------------------
	std::vector<CGameServer *> Servers;
	for (int i = 0; i < (int)m_Servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];
		if (pServer != NULL)
		{
			if (pServer->Census >= 0)
			{
				Servers.push_back(pServer);
			}
		}
	}
	//-----------------------------------------------------

	unsigned long Size = sizeof(PG_CtoM_CENSUS) + Servers.size() * sizeof(GSCensus);
	Size = ((Size / 4) + 1) * 4;

	void *pData = GetPacketBuffer(Size);
	PBYTE pTraveler = (PBYTE)pData;

	PG_CtoM_CENSUS *pHeader = (PG_CtoM_CENSUS *)pTraveler;
	pHeader->PGID = EM_PG_CtoM_CENSUS;
	pHeader->Count = Servers.size();

	for (int i = 0; i < (int)Servers.size(); ++i)
	{
		CGameServer *pServer = Servers[i];
		pHeader->Census[i].Index = pServer->Index;
		pHeader->Census[i].Census = pServer->Census;
	}

	SendToMonitor(netid, Size, pData);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SM_WaitCount(unsigned long netid, int index, int waitingcount, int loadingcount)
{
	PG_CtoM_WAIT_COUNT Send;
	Send.Index = index;
	Send.WaitingCount = waitingcount;
	Send.LoadingCount = loadingcount;

	SendToMonitor(netid, sizeof(Send), &Send);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SM_DynamicCommands(unsigned long netid)
{
	//-------------------------------------------------------
	int Count = 0;
	for (int i = 0; i < (int)m_Servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];
		if (pServer != NULL)
		{
			Count += pServer->Commands.size();
		}
	}
	//-------------------------------------------------------

	unsigned long Size = sizeof(PG_CtoM_DYNAMIC_COMMANDS) + (Count * sizeof(GSCommand));
	Size = ((Size / 4) + 1) * 4;

	void *pData = GetPacketBuffer(Size);
	PBYTE pTraveler = (PBYTE)pData;

	PG_CtoM_DYNAMIC_COMMANDS *pHeader = (PG_CtoM_DYNAMIC_COMMANDS *)pTraveler;
	pHeader->PGID = EM_PG_CtoM_DYNAMIC_COMMANDS;
	pHeader->Count = Count;

	pTraveler += sizeof(PG_CtoM_DYNAMIC_COMMANDS);

	for (int i = 0; i < (int)m_Servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];
		if (pServer != NULL)
		{
			for (int j = 0; j < (int)pServer->Commands.size(); ++j)
			{
				GSCommand *pCommand = pServer->Commands[j];

				if (pCommand != NULL)
				{
					(*(GSCommand *)pTraveler) = *pCommand;
				}

				pTraveler += sizeof(GSCommand);
			}
		}
	}

	SendToMonitor(netid, Size, pData);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SM_Aux(unsigned long netid, int index, int aux, int value)
{
	PG_CtoM_AUX Send;
	Send.Index = index;
	Send.Aux = aux;
	Send.Value = value;

	SendToMonitor(netid, sizeof(Send), &Send);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SM_InfoOnStartUp(unsigned long netid, int index, const char *fileversion)
{
	PG_CtoM_INFO_ON_START_UP Send;
	Send.Index = index;
	strcpy(Send.FileVersion, fileversion);

	SendToMonitor(netid, sizeof(Send), &Send);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::SM_PopupMessage(unsigned long netid, const char *message)
{
	int Len = strlen(message);
	unsigned long Size = sizeof(PG_CtoM_POPUP_MESSAGE) + Len + 1;
	Size = ((Size / 4) + 1) * 4;

	void *pData = GetPacketBuffer(Size);

	PG_CtoM_POPUP_MESSAGE *pHeader = (PG_CtoM_POPUP_MESSAGE *)pData;
	pHeader->PGID = EM_PG_CtoM_POPUP_MESSAGE;
	pHeader->Len = Len;
	strcpy(pHeader->Message, message);

	SendToMonitor(netid, Size, pData);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_ON_MSG_DEFAULT(CGameServer *pserver, const char *output)
{
	for (std::map<unsigned long, CMonitor *>::iterator it = m_Monitors.begin(); it != m_Monitors.end(); it++)
	{
		CMonitor *pMonitor = it->second;

		if (pMonitor != NULL)
		{
			if (pMonitor->SelectedIndex == pserver->Index)
			{
				SM_ConsoleOutput(pMonitor->NetID, pserver->Index, output);
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_ON_MSG_SERVER_READY(CGameServer *pserver, const char *output)
{
	if (pserver->Status == EM_SERVER_STATUS_STARTING)
	{
		pserver->Status = EM_SERVER_STATUS_ONLINE;
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_ON_MSG_SET_CUSTOM_STATE(CGameServer *pserver, const char *output)
{
	std::vector<std::string> vecTokens;
	_StringTokenize(output, ",\n", vecTokens);

	if (vecTokens.size() >= 3)
	{
		int CustomStatus = atoi(vecTokens[1].c_str());
		if (CustomStatus == -1)
		{
			pserver->CustomStatus = 0;
		}
		else
		{
			pserver->CustomStatus = EM_SERVER_STATUS_CUSTOM_0;
		}

		pserver->CustomStatusString = vecTokens[2].c_str();
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_ON_MSG_SERVER_ERROR(CGameServer *pserver, const char *output)
{
	std::vector<std::string> vecTokens;
	_StringTokenize(output, ",\n", vecTokens);

	if (vecTokens.size() >= 2)
	{
		SM_CriticalEvent(-1, pserver->Index, EM_CRITICAL_EVENT_SERVER_REPORT, vecTokens[1].c_str());

		Print(5, "", "[%s][%s][%s] %s", pserver->RegionNameANSI.c_str(), pserver->GroupNameANSI.c_str(), pserver->ServerNameANSI.c_str(), vecTokens[1].c_str());
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_ON_MSG_WORLD_CENSUS(CGameServer *pserver, const char *output)
{
	std::vector<std::string> vecTokens;
	_StringTokenize(output, ",\n", vecTokens);

	if (vecTokens.size() >= 2)
	{
		int Census = atoi(vecTokens[1].c_str());

		pserver->Census = Census;
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_ON_MSG_ZONE_CENSUS(CGameServer *pserver, const char *output)
{
	std::vector<std::string> vecTokens;
	_StringTokenize(output, ",\n", vecTokens);

	if (vecTokens.size() >= 2)
	{
		int Census = atoi(vecTokens[1].c_str());

		pserver->Census = Census;
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_ON_MSG_ADD_COMMAND(CGameServer *pserver, const char *output)
{
	std::vector<std::string> vecTokens;
	_StringTokenize(output, ",\n", vecTokens);

	if (vecTokens.size() >= 3)
	{
		std::string &Caption = vecTokens[1];
		std::string &Command = vecTokens[2];

		pserver->AddCommand(Caption.c_str(), Command.c_str(), false, true);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_ON_MSG_ADD_INPUT_COMMAND(CGameServer *pserver, const char *output)
{
	std::vector<std::string> vecTokens;
	_StringTokenize(output, ",\n", vecTokens);

	if (vecTokens.size() >= 3)
	{
		std::string &Caption = vecTokens[1];
		std::string &Command = vecTokens[2];

		pserver->AddCommand(Caption.c_str(), Command.c_str(), true, true);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_ON_MSG_REMOVE_COMMAND(CGameServer *pserver, const char *output)
{
	std::vector<std::string> vecTokens;
	_StringTokenize(output, ",\n", vecTokens);

	if (vecTokens.size() >= 2)
	{
		std::string &Caption = vecTokens[1];
		pserver->RemoveCommand(Caption.c_str());
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_ON_MSG_ENABLE_COMMAND(CGameServer *pserver, const char *output)
{
	std::vector<std::string> vecTokens;
	_StringTokenize(output, ",\n", vecTokens);

	if (vecTokens.size() >= 2)
	{
		std::string &Caption = vecTokens[1];
		pserver->EnableCommand(Caption.c_str());
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_ON_MSG_DISENABLE_COMMAND(CGameServer *pserver, const char *output)
{
	std::vector<std::string> vecTokens;
	_StringTokenize(output, ",\n", vecTokens);

	if (vecTokens.size() >= 2)
	{
		std::string &Caption = vecTokens[1];
		pserver->DisableCommand(Caption.c_str());
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_ON_MSG_WORLD_ID(CGameServer *pserver, const char *output)
{
	std::vector<std::string> vecTokens;
	_StringTokenize(output, ",\n", vecTokens);
	if (vecTokens.size() >= 2)
	{
		pserver->WorldID = atoi(vecTokens[1].c_str());
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_ON_MSG_WAIT_COUNT(CGameServer *pserver, const char *output)
{
	//WaitCount {7E52C067-80A5-42AA-AA4D-6D25469B3C00},W[%d],L[%d]
	std::vector<std::string> vecTokens1;
	std::vector<std::string> vecTokens2;

	_StringTokenize(output, ",\n", vecTokens1);
	if (vecTokens1.size() >= 3)
	{
		int WaitingCount = 0;
		int LoadingCount = 0;

		_StringTokenize(vecTokens1[1].c_str(), "[]", vecTokens2);

		if (vecTokens2.size() >= 2)
		{
			WaitingCount = atoi(vecTokens2[1].c_str());
		}

		vecTokens2.clear();

		_StringTokenize(vecTokens1[2].c_str(), "[]", vecTokens2);

		if (vecTokens2.size() >= 2)
		{
			LoadingCount = atoi(vecTokens2[1].c_str());
		}

		SM_WaitCount(-1, pserver->Index, WaitingCount, LoadingCount);
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::_ON_MSG_AUX1(CGameServer *pserver, const char *output)
{
	//Aux1 {E9E41FAF-A7D0-4762-8446-C8E85CDC5065},%d
	std::vector<std::string> vecTokens;
	_StringTokenize(output, ",\n", vecTokens);
	if (vecTokens.size() >= 2)
	{
		SM_Aux(-1, pserver->Index, 0, atoi(vecTokens[1].c_str()));
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::_ON_MSG_AUX2(CGameServer *pserver, const char *output)
{
	//Aux2 {E4D780A7-873B-4888-8133-023DBCD680B4},%d
	std::vector<std::string> vecTokens;
	_StringTokenize(output, ",\n", vecTokens);
	if (vecTokens.size() >= 2)
	{
		SM_Aux(-1, pserver->Index, 1, atoi(vecTokens[1].c_str()));
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_START_SERVER(int index)
{
	CGameServer *pServer = GetServerAt(index);
	if (pServer != NULL)
	{
		pServer->Start();

		SM_InfoOnStartUp(-1, pServer->Index, pServer->ExeFileVer);
	}
	else
	{
		Print(LV2, "On_PG_MtoC_START_SERVER", "Start fail %d is NULL", index);
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_STOP_SERVER(int index)
{
	CGameServer *pServer = GetServerAt(index);
	if (pServer != NULL)
	{
		pServer->Stop();
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_SERVER_COMMAND(int index, const char *command)
{
	CGameServer *pServer = GetServerAt(index);

	if (pServer != NULL)
	{
		if ((_stricmp(command, SERVER_COMMAND_CHANGEPARALLEL) == 0) && (pServer->Type == EM_GAME_SERVER_TYPE_ZONE))
		{
			pServer->SelfStop = true;
		}

		SGS_Command(pServer->NetID, command, pServer->ServerNameANSI.c_str());
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_SET_CUSTOM_STATUS(int index, int status, const char *str)
{
	CGameServer *pServer = GetServerAt(index);

	if (pServer != NULL)
	{
		pServer->CustomStatus = status;
		pServer->CustomStatusString = str;
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_ENABLE_SERVER_OUTPUT(unsigned long netid, int index)
{
	std::map<unsigned long, CMonitor *>::iterator it = m_Monitors.find(netid);

	if (it != m_Monitors.end())
	{
		CMonitor *pMonitor = it->second;
		if (pMonitor != NULL)
		{
			pMonitor->SelectedIndex = index;
		}
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_RELOAD_CONFIG()
{
	// Send a server info clear packet to all connected clients
	SM_ClearServerInfo(-1);

	//reload config file
	LoadConfig();

	//Send new server info to all monitor
	SM_ServerInfo(-1, m_Servers);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_ENABLE_SERVER(int index, int enable)
{
	CGameServer *pServer = GetServerAt(index);

	if (pServer != NULL)
	{
		pServer->Disabled = (enable == 0);
		SaveConfig();
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_STOP_ALL_SERVER_NOTIFICATION()
{
	for (int i = 0; i < (int)m_Servers.size(); ++i)
	{
		CGameServer *pServer = m_Servers[i];
		if (pServer != NULL)
		{
			pServer->SelfStop = true;
		}
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_REQUEST_SERVER_STATUS(unsigned long netid)
{
	SM_ServerStatsu(netid);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_REQUEST_CPU_USAGE(unsigned long netid)
{
	SM_CpuUsage(netid);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_REQUEST_MEM_USAGE(unsigned long netid)
{
	SM_MemUsage(netid);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_REQUEST_DYNAMIC_COMMANDS(unsigned long netid)
{
	SM_DynamicCommands(netid);
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_REQUEST_CENSUS(unsigned long netid)
{
	SM_Census(netid);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_CREATE_DUMP(unsigned long netid, int index, int dumptype)
{
	CGameServer *pServer = GetServerAt(index);

	if (pServer)
	{
		if (pServer->CreateDump(dumptype) == true)
		{
			SM_PopupMessage(netid, "Dump file created");
		}
		else
		{
			SM_PopupMessage(netid, "Create dump file failed.");
		}
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_MtoC_RESET_SERVER(int index)
{
	CGameServer *pServer = GetServerAt(index);
	if (pServer != NULL)
	{
		pServer->Reset();
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_StoC_CONSOLE_OUTPUT(unsigned long netid, const char *output)
{
	CGameServer *pServer = GetServerByNetID(netid);
	if (pServer != NULL)
	{
		std::string Content = output;

		int Index1 = Content.find_first_of('{');
		int Index2 = Content.find_first_of('}');

		//We may get a key str
		if (Index1 >= 0 && Index2 >= 0)
		{
			std::string Key = Content.substr(0, Index2 + 1);
			_Trim(Key, '\n');
			_strlwr((char *)Key.c_str());

			std::map<std::string, PFUNC_KEY_STR>::iterator it = m_KeyStrFuncs.find(Key.c_str());

			if (it != m_KeyStrFuncs.end())
			{
				(it->second)(pServer, output);
			}
			else
			{
				_ON_MSG_DEFAULT(pServer, output);
			}
		}
		else
		{
			_ON_MSG_DEFAULT(pServer, output);
		}

		pServer->UpdateResponseTick();
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_StoC_REGISTRATION(unsigned long netid, unsigned long pid, const char *regionname, const char *groupname, const char *servername, const char *cmdline)
{
	std::wstring RegionName = CharToWchar(regionname);
	std::wstring GroupName = CharToWchar(groupname);
	std::wstring ServerName = CharToWchar(servername);
	std::wstring CmdLine = CharToWchar(cmdline);

	// index correspond descriptor object by PID
	CGameServer *pServer = GetServerByPID(pid);

	// the game server is not created by controller (may be game server that try to reconnect or is not a member defined in controller ini file)
	if (pServer == NULL)
	{
		// index correspond descriptor object by region name, group name and server name
		pServer = GetServerByName(RegionName.c_str(), GroupName.c_str(), ServerName.c_str());

		if (pServer == NULL)
		{
			//---------------------------------------
			//Get Module File Name by PID
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

			HMODULE hMods[128];
			unsigned long cbNeeded;

			wchar_t *Buff = (wchar_t *)GetStringBuffer(512);

			if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
			{
				GetModuleFileNameExW(hProcess, hMods[0], Buff, 512);
			}

			CloseHandle(hProcess);

			std::wstring FilePath = Buff;

			//---------------------------------------
			//determine server name
			Buff = (wchar_t *)GetStringBuffer(512);

			//prevent duplicate "PID" in server Name
			if (wcsstr(ServerName.c_str(), L"PID") == NULL)
			{
				swprintf(Buff, 256, L"%s(PID %d)", ServerName.c_str(), pid);
			}
			else
			{
				swprintf(Buff, 256, L"%s", ServerName.c_str());
			}
			//---------------------------------------

			//create a new CGameServer object
			pServer = AddGameServer(L"Attached", L"UnknownGroup", Buff, FilePath.c_str(), CmdLine.c_str(), false, true);

			if (pServer != NULL)
			{
				pServer->Start(pid);
			}

			//---------------------------------------------------------
			//Send game server information to all monitor
			std::vector<CGameServer *> Servers;
			Servers.push_back(pServer);
			SM_ServerInfo(-1, Servers);
			//---------------------------------------------------------

			Print(LV2, "", "Attached game server [%s] is connected", WcharToChar(Buff).c_str());
		}
		else
		{
			//try to start server with PID
			pServer->Start(pid);

			Print(LV2, "", "Game server [%s][%s][%s] is reconnected.", pServer->RegionNameANSI.c_str(), pServer->GroupNameANSI.c_str(), pServer->ServerNameANSI.c_str());
		}
	}
	else
	{
		Print(LV2, "", "Game server [%s][%s][%s] registration.", pServer->RegionNameANSI.c_str(), pServer->GroupNameANSI.c_str(), pServer->ServerNameANSI.c_str());
	}

	if (pServer)
	{
		pServer->NetID = netid;

		//reg netid
		m_NetIDIndex[netid] = pServer;

		//Send region, group, server name to game server
		SGS_Notification(netid, pServer->RegionNameANSI.c_str(), pServer->GroupNameANSI.c_str(), pServer->ServerNameANSI.c_str());
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_StoC_SERVER_TYPE(unsigned long netid, int type)
{
	CGameServer *pServer = GetServerByNetID(netid);

	if (pServer)
	{
		pServer->Type = type;
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Controller::On_PG_StoC_SERVER_READY(unsigned long netid)
{
	CGameServer *pServer = GetServerByNetID(netid);

	if (pServer)
	{
		if (pServer->Status == EM_SERVER_STATUS_STARTING)
		{
			pServer->Status = EM_SERVER_STATUS_ONLINE;
		}
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_START_SERVER(unsigned long netid, unsigned long size, void *pg)
{
	PG_MtoC_START_SERVER *PG = (PG_MtoC_START_SERVER *)pg;
	On_PG_MtoC_START_SERVER(PG->Index);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_STOP_SERVER(unsigned long netid, unsigned long size, void *pg)
{
	PG_MtoC_STOP_SERVER *PG = (PG_MtoC_STOP_SERVER *)pg;
	On_PG_MtoC_STOP_SERVER(PG->Index);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_SERVER_COMMAND(unsigned long netid, unsigned long size, void *pg)
{
	PG_MtoC_SERVER_COMMAND *PG = (PG_MtoC_SERVER_COMMAND *)pg;
	On_PG_MtoC_SERVER_COMMAND(PG->Index, PG->Command);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_SET_CUSTOM_STATUS(unsigned long netid, unsigned long size, void *pg)
{
	PG_MtoC_SET_CUSTOM_STATUS *PG = (PG_MtoC_SET_CUSTOM_STATUS *)pg;
	On_PG_MtoC_SET_CUSTOM_STATUS(PG->Index, PG->Status, PG->String);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_ENABLE_SERVER_OUTPUT(unsigned long netid, unsigned long size, void *pg)
{
	PG_MtoC_ENABLE_SERVER_OUTPUT *PG = (PG_MtoC_ENABLE_SERVER_OUTPUT *)pg;
	On_PG_MtoC_ENABLE_SERVER_OUTPUT(netid, PG->Index);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_RELOAD_CONFIG(unsigned long netid, unsigned long size, void *pg)
{
	Print(2, "", "Reload Config");
	On_PG_MtoC_RELOAD_CONFIG();
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_ENABLE_SERVER(unsigned long netid, unsigned long size, void *pg)
{
	PG_MtoC_ENABLE_SERVER *PG = (PG_MtoC_ENABLE_SERVER *)pg;
	On_PG_MtoC_ENABLE_SERVER(PG->Index, PG->Enable);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_STOP_ALL_SERVER_NOTIFICATION(unsigned long netid, unsigned long size, void *pg)
{
	On_PG_MtoC_STOP_ALL_SERVER_NOTIFICATION();
}
//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_REQUEST_SERVER_STATUS(unsigned long netid, unsigned long size, void *pg)
{
	On_PG_MtoC_REQUEST_SERVER_STATUS(netid);
}
//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_REQUEST_CPU_USAGE(unsigned long netid, unsigned long size, void *pg)
{
	On_PG_MtoC_REQUEST_CPU_USAGE(netid);
}
//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_REQUEST_MEM_USAGE(unsigned long netid, unsigned long size, void *pg)
{
	On_PG_MtoC_REQUEST_MEM_USAGE(netid);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_REQUEST_DYNAMIC_COMMANDS(unsigned long netid, unsigned long size, void *pg)
{
	On_PG_MtoC_REQUEST_DYNAMIC_COMMANDS(netid);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_REQUEST_CENSUS(unsigned long netid, unsigned long size, void *pg)
{
	On_PG_MtoC_REQUEST_CENSUS(netid);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_CREATE_DUMP(unsigned long netid, unsigned long size, void *pg)
{
	PG_MtoC_CREATE_DUMP *PG = (PG_MtoC_CREATE_DUMP *)pg;
	On_PG_MtoC_CREATE_DUMP(netid, PG->Index, PG->DumpType);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_MtoC_RESET_SERVER(unsigned long netid, unsigned long size, void *pg)
{
	PG_MtoC_RESET_SERVER *PG = (PG_MtoC_RESET_SERVER *)pg;
	On_PG_MtoC_RESET_SERVER(PG->Index);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_StoC_REGISTRATION(unsigned long netid, unsigned long size, void *pg)
{
	PG_StoC_REGISTRATION *PG = (PG_StoC_REGISTRATION *)pg;
	On_PG_StoC_REGISTRATION(netid, PG->PID, PG->RegionName, PG->GroupName, PG->ServerName, PG->CmdLine);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_StoC_CONSOLE_OUTPUT(unsigned long netid, unsigned long size, void *pg)
{
	PG_StoC_CONSOLE_OUTPUT *PG = (PG_StoC_CONSOLE_OUTPUT *)pg;
	On_PG_StoC_CONSOLE_OUTPUT(netid, PG->Output);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_StoC_COMMAND_TO_CONTROLLER(unsigned long netid, unsigned long size, void *pg)
{
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_StoC_SERVER_TYPE(unsigned long netid, unsigned long size, void *pg)
{
	PG_StoC_SERVER_TYPE *PG = (PG_StoC_SERVER_TYPE *)pg;
	On_PG_StoC_SERVER_TYPE(netid, PG->Type);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_PG_StoC_SERVER_READY(unsigned long netid, unsigned long size, void *pg)
{
	On_PG_StoC_SERVER_READY(netid);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool Controller::_OnRecv(unsigned long netid, unsigned long size, void *pg)
{
	bool Result = false;

	if (pg != NULL)
	{
		PG_CONTROLLER_BASE *PG = (PG_CONTROLLER_BASE *)pg;
		if (PG->PGID < (int)m_PGFuncs.size())
		{
			if (memcmp(PG->Key, GetUniqueKey(PG->PGID), UNIQUE_KEY_SIZE) != 0)
			{
				Print(5, "Controller::_OnRecv", "Key error packet id:%d", PG->PGID);
				return Result;
			}
			
			PFUNC_NET_PG pFunc = m_PGFuncs[PG->PGID];

			if (pFunc != NULL)
			{
				pFunc(netid, size, pg);
				Result = true;
			}
		}
	}

	return Result;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_OnMonitorConnected(unsigned long netid)
{
	Print(2, "", "Monitor OnConnect", netid);

	AddMonitor(netid);

	SM_ServerInfo(netid, m_Servers);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_OnMonitorDisconnected(unsigned long netid)
{
	Print(2, "", "Monitor OnDisconnect", netid);
	RemoveMonitor(netid);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_OnGSConnected(unsigned long netid)
{
	Print(2, "", "_OnGSConnected %d", netid);

	//Connected notify
	SGS_Connected(netid);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_OnGSDisconnected(unsigned long netid)
{
	CGameServer *pServer = GetServerByNetID(netid);

	if (pServer != NULL)
	{
		pServer->NetID = INVALID_NET_ID;

		Print(5, "", "Server [%s] disconnect", pServer->ServerNameANSI.c_str());

		//we don't keep any unknown server
		if (pServer->Attached)
		{
			pServer->Stop();
			RemoveGameServer(pServer);
		}
	}

	m_NetIDIndex.erase(netid);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_OnNBMonitorError(int lv, const char *str)
{
	Print(lv, "_OnNBMonitorError", "%s", str);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void Controller::_OnNBGSError(int lv, const char *str)
{
	Print(lv, "_OnNBGSError", "%s", str);
}

//***********************************************************************************************

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool MonitorEventObj::OnRecv(unsigned long netid, unsigned long size, void *data)
{
	if (Controller::This() != NULL)
	{
		return Controller::This()->_OnRecv(netid, size, data);
	}

	return true;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void MonitorEventObj::OnConnectFailed(unsigned long netid, unsigned long failedcode)
{
	Print(5, "", "Monitor OnConnectFailed failed code=%d", failedcode);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void MonitorEventObj::OnConnect(unsigned long netid)
{
	if (Controller::This() != NULL)
	{
		Controller::This()->_OnMonitorConnected(netid);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void MonitorEventObj::OnDisconnect(unsigned long netid)
{
	if (Controller::This() != NULL)
	{
		Controller::This()->_OnMonitorDisconnected(netid);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
CEventObj *MonitorEventObj::OnAccept(unsigned long netid)
{
	return this;
}

//***********************************************************************************************
bool GSEventObj::OnRecv(unsigned long netid, unsigned long size, void *data)
{
	if (Controller::This() != NULL)
	{
		return Controller::This()->_OnRecv(netid, size, data);
	}

	return true;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void GSEventObj::OnConnectFailed(unsigned long netid, unsigned long failedcode)
{
	Print(5, "", "OnConnectFailed failed code=%d.", failedcode);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void GSEventObj::OnConnect(unsigned long netid)
{
	if (Controller::This() != NULL)
	{
		Controller::This()->_OnGSConnected(netid);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void GSEventObj::OnDisconnect(unsigned long netid)
{
	if (Controller::This() != NULL)
	{
		Controller::This()->_OnGSDisconnected(netid);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
CEventObj *GSEventObj::OnAccept(unsigned long netid)
{
	return this;
}

//***********************************************************************************************
//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
MemoryBlock::MemoryBlock(unsigned long size)
{
	m_Memory = NULL;
	m_Size = 0;

	if (size > 0)
	{
		SetSize(size);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
MemoryBlock::~MemoryBlock()
{
	Clear();
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void MemoryBlock::Clear()
{
	if (m_Memory != NULL)
	{
		delete[] m_Memory;
		m_Memory = NULL;
		m_Size = 0;
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void MemoryBlock::SetSize(unsigned long val)
{
	if (val > m_Size)
	{
		Clear();

		m_Size = ((val / 4) + 1) * 4 + 64;
		m_Memory = new BYTE[m_Size];
	}

	SecureZeroMemory(m_Memory, m_Size);
}

void Controller::SaveConfigNew()
{
	//return;
	FILE *pFile = _wfopen(L".\\RAServerControllerNew.ini", L"w,ccs=UNICODE");
	if (pFile == NULL)
		return;
	std::vector<CGameServer *> m_ServersNew;
	CGameServer *serv = new CGameServer();
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"Service";
	serv->ServerName = L"Account";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Common/Account.exe";
	serv->CmdLine = L"account.ini";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"Service";
	serv->ServerName = L"Serverlist";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Common/ServerList.exe";
	serv->CmdLine = L"serverlist.ini";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"Service";
	serv->ServerName = L"Switch";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Service/Switch.exe";
	serv->CmdLine = L"Switch.ini";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"Service";
	serv->ServerName = L"Proxy";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Service/Proxy.exe";
	serv->CmdLine = L"Proxy.ini";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"Service";
	serv->ServerName = L"Master";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Service/Master.exe";
	serv->CmdLine = L"Master.ini";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"Service";
	serv->ServerName = L"PlayerCenter";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Service/PlayerCenter.exe";
	serv->CmdLine = L"PlayerCenter.ini";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"Service";
	serv->ServerName = L"DataCenter";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Service/DataCenter.exe";
	serv->CmdLine = L"DataCenter.ini";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"Service";
	serv->ServerName = L"Partition";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Service/Partition.exe";
	serv->CmdLine = L"Partition.ini";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"Service";
	serv->ServerName = L"Chat";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Service/Chat.exe";
	serv->CmdLine = L"Chat.ini";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"Service";
	serv->ServerName = L"Gateway";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Service/Gateway.exe";
	serv->CmdLine = L"Gateway.ini";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"Service";
	serv->ServerName = L"LuaDebugAgent";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Service/LuaDebugAgent.exe";
	serv->CmdLine = L"LuaDebugAgent.ini";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	//serv = new CGameServer();
	//serv->RegionName = L"Realm_01";
	//serv->GroupName = L"ZONE_M";
	//serv->ServerName = L"ZONE_1 [Howling Mountains]";
	//serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	//serv->CmdLine = L"Zone_1.INI";
	//serv->Disabled = false;
	//m_ServersNew.push_back(serv);
	//serv = new CGameServer();
	//serv->RegionName = L"Realm_01";
	//serv->GroupName = L"ZONE_M";
	//serv->ServerName = L"ZONE_2 [Silverspring]";
	//serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	//serv->CmdLine = L"Zone_2.INI";
	//serv->Disabled = false;
	//m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_400 [Home]";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_400.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	//serv = new CGameServer();
	//serv->RegionName = L"Realm_01";
	//serv->GroupName = L"ZONE_M";
	//serv->ServerName = L"ZONE_16 [Southern Janost Forest]";
	//serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	//serv->CmdLine = L"Zone_16.INI";
	//serv->Disabled = false;
	//m_ServersNew.push_back(serv);
	//serv = new CGameServer();
	//serv->RegionName = L"Realm_01";
	//serv->GroupName = L"ZONE_M";
	//serv->ServerName = L"ZONE_17 [Northern Janost Forest]";
	//serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	//serv->CmdLine = L"Zone_17.INI";
	//serv->Disabled = false;
	//m_ServersNew.push_back(serv);
	//serv = new CGameServer();
	//serv->RegionName = L"Realm_01";
	//serv->GroupName = L"ZONE_M";
	//serv->ServerName = L"ZONE_18 [Limo Desert]";
	//serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	//serv->CmdLine = L"Zone_18.INI";
	//serv->Disabled = false;
	//m_ServersNew.push_back(serv);
	//serv = new CGameServer();
	//serv->RegionName = L"Realm_01";
	//serv->GroupName = L"ZONE_M";
	//serv->ServerName = L"ZONE_19 [Land of Malevolence]";
	//serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	//serv->CmdLine = L"Zone_19.INI";
	//serv->Disabled = false;
	//m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_1 Wailing Mountains";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_1.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_2 Silverspring";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_2.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_3 Ravenfell";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_3.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_4 Aslan";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_4.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_5 Hybora Highlands";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_5.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_6 Dustbloom Canyon";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_6.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_7 Weeping Coast";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_7.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_8 Savage Lands";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_8.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_9 Mount Al Turaj";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_9.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_10 Sascilia Steppes";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_10.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_11 Dragonfang Ridge";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_11.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_12 Elven Citadel";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_12.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_13 Z13_Pelico_Coast";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_13.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_14 Z14_Xaviera";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_14.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_15 Thunderhoof_Mesa";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_15.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_16 South_Jenotar_Forest";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_16.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_17 Northern_Janost_Forest";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_17.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_18 Lymor_Desert";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_18.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_19 Sinister_Lands";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_19.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_20 Z20_Red_Hills";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_20.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_21 Taturin_Gulf";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_21.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_22 Z22_Old_Rojan_Kingdom";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_22.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_23 Z23_Queslana";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_23.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_24 Z24_merdin_tundra";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_24.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_25 Z25_Serbayt_pass";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_25.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_26 Z26_Zhyro";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_26.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_27 Z27_wailing_fjord";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_27.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_28 Z28_hurtekke_jungle";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_28.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_29 Z29_Xadia_Basin";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_29.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_30 Z30_kathalan";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_30.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_31 Z31_ifantrish_crypt";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_31.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_32 Z32_Splitwater";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_32.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_33 Z33_Faytear_Uplands";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_33.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_34 Z34_tasuq_island";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_34.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_35 world_7_02_cluhiss_island";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_35.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_36 world_7_03_enoh_island";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_36.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_37 world_7_04_vort_island";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_37.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_38 world_7_05_zacehs_island";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_38.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_66 Tutorial";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_66.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_80 inst_fireboot_fortress";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_80.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_81 bg_arcane_arena_01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_81.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_90 Z90_atlantica";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_90.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_100 Tutorial";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_100.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_101 dgn_cavern_of_trials";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_101.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_102 dgn_forlorn_monastary";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_102.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_103 dgn_reliquary_of_reflections";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_103.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_104 dgn_mystic_altar";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_104.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_105 dgn_kalturok_queen_chamber";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_105.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_106 dgn_borsburs";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_106.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_107 dgn_kleeon_temple";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_107.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_108 dgn_the_treasure_trove";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_108.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_109 hybora_labyrinth_boss_01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_109.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_110 dgn_wretched_grotto";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_110.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_111 hybora_labyrinth_boss_01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_111.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_112 hybora_labyrinth_boss_01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_112.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_113 dgn_mulgrum_relic";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_113.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_114 dgn_mulgrum_relic";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_114.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_115 dgn_echoes_of_the_sea";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_115.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_116 dgn_the_origin1";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_116.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_117 dgn_hall_of_survivors";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_117.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_118 dgn_moonspring_hollow";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_118.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_119 dgn_dragons_nest";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_119.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_120 dgn_naga_acropolis_zurhidon";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_120.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_121 dgn_sand_vortex";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_121.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_122 dgn_naga_acropolis_lair";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_122.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_123 dgn_hall_of_survivors";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_123.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_124 dgn_naga_acropolis_zurhidon";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_124.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_125 dgn_naga_acropolis_lair";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_125.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_126 dgn_echoes_of_the_sea";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_126.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_127 dgn_daelanis_jail";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_127.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_128 dgn_daelanis_jail";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_128.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_129 dgn_venadurken_arena";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_129.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_130 dgn_pesche_temple_01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_130.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_131 dgn_venadurken_arena";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_131.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_132 dgn_pesche_temple_01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_132.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_133 dgn_temple_pakale";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_133.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_134 dgn_kafkes_tomb";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_134.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_135 dgn_kafkes_tomb";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_135.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_136 dgn_graf_castle";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_136.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_137 dgn_graf_castle";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_137.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_138 dgn_graf_castle";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_138.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_139 dgn_sardo_bastille";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_139.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_140 dgn_sardo_bastille";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_140.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_141 dgn_tomb_of_seven_heroes";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_141.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_142 dgn_tomb_of_seven_heroes";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_142.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_143 dgn_tomb_of_seven_heroes";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_143.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_144 dgn_enchanted_entrance";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_144.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_145 dgn_enchanted_entrance";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_145.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_146 dgn_qrich_cadaver_den";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_146.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_147 dgn_qrich_cadaver_den";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_147.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_148 dgn_qrich_cadaver_den";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_148.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_149 dgn_bedim";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_149.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_150 dgn_bedim";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_150.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_151 dgn_boutman_haunt";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_151.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_152 dgn_boutman_haunt";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_152.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_153 dgn_boutman_haunt";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_153.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_154 dgn_bellatia_forts";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_154.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_155 dgn_bellatia_forts";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_155.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_156 dgn_bellatia_forts";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_156.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_157 dgn_leviathan_lair";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_157.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_158 dgn_leviathan_lair";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_158.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_159 dgn_leviathan_lair";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_159.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_160 dgn_muraifen";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_160.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_161 dgn_muraifen";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_161.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_162 dgn_muraifen";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_162.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_163 dgn_throne_catacomb";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_163.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_164 dgn_throne_catacomb";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_164.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_165 dgn_throne_catacomb";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_165.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_166 dgn_kathalan_secret_cellar";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_166.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_167 dgn_kathalan_secret_cellar";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_167.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_168 dgn_kathalan_secret_cellar";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_168.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_169 dgn_skull_rock";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_169.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_170 dgn_skull_rock";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_170.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_171 dgn_skull_rock";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_171.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_172 dgn_mardroke";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_172.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_173 dgn_mardroke";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_173.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_174 dgn_mardroke";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_174.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_175 dgn_Raven_Heart";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_175.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_176 dgn_Raven_Heart";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_176.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_177 dgn_Raven_Heart";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_177.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_178 dgn_ritual_canyon";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_178.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_179 dgn_ritual_canyon";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_179.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_180 dgn_ritual_canyon";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_180.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_181 dgn_frostsaber_upland";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_181.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_182 dgn_frostsaber_upland";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_182.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_183 dgn_frostsaber_upland";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_183.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_184 dgn_solarmuse_alter";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_184.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_185 dgn_solarmuse_alter";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_185.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_186 dgn_solarmuse_alter";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_186.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_187 dgn_dead_soul_tomb";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_187.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_188 dgn_dead_soul_tomb";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_188.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_189 dgn_dead_soul_tomb";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_189.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_190 bg_world_battle_01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_190.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_191 dgn_world_battle_02";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_191.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_192 bg_world_battle_01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_192.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_193 bg_world_battle_01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_193.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_194 dgn_chaotic_vortex_realm_vile";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_194.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_196 dgn_the_origin1";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_196.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_197 dgn_echoes_of_the_sea";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_197.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_198 dgn_dragons_nest";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_198.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_199 dgn_chaotic_vortex_realm_vile";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_199.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_200 dgn_cavern_of_trials";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_200.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_201 tiledgn_bloody_gallery";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_201.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_202 tiledgn_hybora_labyrinth_01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_202.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_203 tiledgn_hybora_labyrinth_02";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_203.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_204 tiledgn_hybora_labyrinth_03";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_204.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_205 tiledgn_hybora_labyrinth_01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_205.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_206 tiledgn_hybora_labyrinth_02";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_206.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_207 tiledgn_hybora_labyrinth_03";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_207.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_208 dgn_varanas_dreamscape";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_208.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_209 DNG_DAELANIS_SEWER";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_209.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_210 dgn_menorca_empire_ruins";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_210.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_211 inst_fireboot_fortress";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_211.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_212 dgn_taffrock_southside";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_212.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_215 dgn_mirror_world01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_215.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_216 dgn_mirror_world01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_216.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_217 dgn_mirror_world01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_217.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_218 dgn_mirror_world02";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_218.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_219 dgn_mirror_world02";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_219.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_220 dgn_mirror_world02";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_220.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_221 dgn_mirror_world03";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_221.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_222 dgn_mirror_world03";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_222.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_223 dgn_mirror_world03";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_223.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_224 dgn_mirror_world04";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_224.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_225 dgn_mirror_world04";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_225.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_226 dgn_mirror_world04";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_226.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_227 dgn_mirror_world05";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_227.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_228 dgn_mirror_world05";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_228.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_229 dgn_mirror_world05";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_229.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_230 dgn_mirror_world06";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_230.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_231 dgn_mirror_world06";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_231.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_232 dgn_mirror_world06";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_232.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_233 dgn_mirror_world07";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_233.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_234 dgn_mirror_world07";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_234.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_235 dgn_mirror_world07";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_235.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_250 tiledgn_windmill_basement";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_250.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_251 tiledgn_laskotar_arcane_chamber";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_251.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_252 tiledgn_citadel_agriza";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_252.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_253 DNG_ROOM_OF_LADY_VICTORIA";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_253.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_254 dgn_pantheon";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_254.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_255 dgn_pantheon";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_255.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_256 dgn_pantheon";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_256.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_257 dgn_corpus_haven";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_257.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_300 eventdgn_crimson_nightmare";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_300.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_301 eventdgn_myconid_farm";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_301.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_302 eventdgn_wind_wild_animus";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_302.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_303 eventdgn_wedding_hall";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_303.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_304 eventdgn_pet_system";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_304.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_305 wedding_hall_2";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_305.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_306 wedding_hall_2";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_306.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_307 wedding_hall_2";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_307.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_308 eventdgn_festival";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_308.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_350 eventdgn_racetrack_01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_350.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_351 eventdgn_maladinas_illusion";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_351.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_352 eventdgn_goblin_mines";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_352.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_353 eventdgn_maladinas_illusion_2";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_353.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_354 eventdgn_training_grounds";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_354.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_355 eventdgn_ancient_relic";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_355.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_356 eventdgn_ancient_dream";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_356.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_357 eventdgn_secret_garden";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_357.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_358 Hybora Highlands";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_358.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_359 dgn_cavern_of_trials1";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_359.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_360 andor_training_range";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_360.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_361 eventdgn_find_the_difference";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_361.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_401 house_guild_001";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_401.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_402 bg_guildsiege_001";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_402.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_410 bg_coliseum_1v1_001";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_410.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_411 bg_coliseum_6v6_001";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_411.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_430 bg_coliseum_3v3_001";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_430.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_431 bg_ctf_001";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_431.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_432 bg_tower_defense_001";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_432.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_440 bg_coliseum_1v1_001";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_440.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_441 eventdgn_racetrack_01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_441.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_442 bg_coliseum_3v3_001";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_442.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_443 bg_coliseum_6v6_001";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_443.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_444 bg_ctf_001";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_444.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_445 bg_tower_defense_001";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_445.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_446 bg_battlefield_001";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_446.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_447 bg_coliseum_6v6_002";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_447.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_448 bg_arcane_arena_01";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_448.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_942 Wailing Mountains";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_942.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_950 Z26_Zhyro";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_950.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_M";
	serv->ServerName = L"ZONE_958 DGN_STUDIO";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_958.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);
	serv = new CGameServer();
	serv->RegionName = L"Realm_01";
	serv->GroupName = L"ZONE_NEW";
	serv->ServerName = L"ZONE_977 HOUSE MAP";
	serv->ExePath = L"c:/Runewaker/Server/Realm_01_Zone/Zone.exe";
	serv->CmdLine = L"Zone_977.INI";
	serv->Disabled = false;
	m_ServersNew.push_back(serv);

	ControllerXML *controllerXMLNew = new ControllerXML;

	wchar_t *Buff = (wchar_t *)GetStringBuffer(2048);
	for (int i = 0; i < (int)m_ServersNew.size(); ++i)
	{
		CGameServer *pServer = m_ServersNew[i];

		if (pServer == NULL)
			continue;

		if (pServer->Attached)
			continue;

		if (pServer->Disabled)
		{
			swprintf(Buff, 1024, L"\"%s\"\t\"%s\"\t\"%s\"\t\"%s\"\t\"%s\"\t\"disabled\"\r\n", pServer->RegionName, pServer->GroupName, pServer->ServerName, pServer->ExePath, pServer->CmdLine);
		}
		else
		{
			swprintf(Buff, 1024, L"\"%s\"\t\"%s\"\t\"%s\"\t\"%s\"\t\"%s\"\r\n", pServer->RegionName, pServer->GroupName, pServer->ServerName, pServer->ExePath, pServer->CmdLine);
		}
		ServerXML *server = controllerXMLNew->Servers.newElement();
		std::stringstream regionName;
		regionName << WtoS(pServer->RegionName);
		server->RegionName = regionName.str();
		std::stringstream groupName;
		groupName << WtoS(pServer->GroupName);
		server->GroupName = groupName.str();
		std::stringstream serverName;
		serverName << WtoS(pServer->ServerName);
		server->ServerName = serverName.str();
		std::stringstream exePath;
		exePath << WtoS(pServer->ExePath);
		server->ExePath = exePath.str();
		std::stringstream cmdLine;
		cmdLine << WtoS(pServer->CmdLine);
		server->CmdLine = cmdLine.str();
		//server->ExePath = pServer->ExePath;
		//server->CmdLine = pServer->CmdLine;
		server->Disabled = pServer->Disabled;
		fputws(Buff, pFile);
	}

	string xmlDataNew = controllerXMLNew->toXML();
	std::ofstream out("RAServerControllerNew.xml");
	out << xmlDataNew;
	out.close();
	fclose(pFile);
	delete (controllerXMLNew);
	return;
	std::locale::global(std::locale("rus"));
	std::ifstream fileXml(L".\\RAServerController.xml");
	if (!fileXml.is_open())
		return;
	std::string xmlData((std::istreambuf_iterator<char>(fileXml)), std::istreambuf_iterator<char>());
	cout << xmlData << std::endl;
	ControllerXML *controllerXML = new ControllerXML;
	if (Serializable::fromXML(xmlData, controllerXML))
	{
		for (size_t i = 0; i < controllerXML->Servers.size(); i++)
		{
			ServerXML *serv = controllerXML->Servers.getItem(i);
			cout << "-------------------------------------------" << std::endl;
			cout << "cout " << serv->GroupName.value() << std::endl;
			wchar_t serverName[MAX_SERVER_NAME_LENGTH];	

			WCHAR wbuff[MAX_SERVER_NAME_LENGTH];
			int nLen = MultiByteToWideChar(CP_UTF8, 0, serv->GroupName.value().c_str(), -1, wbuff, MAX_SERVER_NAME_LENGTH);
			wcout << "wcout wbuff " << wbuff << std::endl;
			//std::wstring widestr = std::wstring(serv->GroupName.value().begin(), serv->GroupName.value().end());
			//const wchar_t* widecstr = widestr.c_str();
			//wcout << "wcout const wchar_t " << widecstr << std::endl;
			//wcout << "wcout StoW " << StoW(serv->GroupName.value()) << std::endl;
			//wcout << "wcout StoWc " << StoW(serv->GroupName.value()).c_str() << std::endl;
			//StoW(serv->RegionName.value()).c_str();
		}
	}
	fileXml.close();
	delete (controllerXML);
	while(true)
	{
	}
}

#pragma managed(pop)