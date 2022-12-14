#pragma once
#ifndef __PG_CONTROLLER_H__
#define __PG_CONTROLLER_H__
#include <memory>
#pragma managed(push, off)
#pragma pack(push, 1)
#define MAX_SERVER_NAME_LENGTH 128
#define MAX_FILE_VERSION_LENGTH 128
#define MAX_COUNTRY_NAME_LENGTH 16
#define MAX_GS_COMMAND_LENGTH 48
#define MAX_CUSTOM_STATUS_STRING_LENGTH 48
#define MAX_SERVER_FILE_PATH_NAME_LENGTH 256
#define MAX_COMPUTER_NAME_LENGTH 16
#define UNIQUE_KEY_SIZE 8
static unsigned char UNIQUE_STOC_KEY[UNIQUE_KEY_SIZE] = {242, 65, 46, 241, 40, 152, 252, 92};
static unsigned char UNIQUE_MTOC_KEY[UNIQUE_KEY_SIZE] = {19, 230, 235, 162, 169, 201, 249, 145};
static unsigned char UNIQUE_CTOS_KEY[UNIQUE_KEY_SIZE] = {70, 248, 246, 187, 190, 50, 82, 187};
static unsigned char UNIQUE_CTOM_KEY[UNIQUE_KEY_SIZE] = {92, 10, 253, 21, 33, 178, 123, 147};
static unsigned char UNIQUE_ERROR_KEY[UNIQUE_KEY_SIZE] = {40, 28, 195, 250, 196, 147, 161, 181};

//**********************************************************************
enum GameServerStatusENUM
{
	EM_SERVER_STATUS_NA = 0,								//!< Not applicable
	EM_SERVER_STATUS_OFFLINE = 100,								//!< Server is off line
	EM_SERVER_STATUS_ONLINE,									//!< Server is on line
	EM_SERVER_STATUS_CUSTOM_0 = 200,								//!< Custom state 0
	EM_SERVER_STATUS_CUSTOM_1,									//!< Custom state 1
	EM_SERVER_STATUS_CUSTOM_2,									//!< Custom state 2
	EM_SERVER_STATUS_CUSTOM_3,									//!< Custom state 3
	EM_SERVER_STATUS_DISABLED = 300,								//!< Server is disabled
	EM_SERVER_STATUS_STARTING,									//!< Server is starting
	EM_SERVER_STATUS_STOPPING,									//!< Server is stopping
	EM_SERVER_STATUS_RESPONSE_WARNING = 400,									//!< Server is in response warning state
	EM_SERVER_STATUS_DISCONNECTED = 500,								//!< Server is disconnected
	EM_SERVER_STATUS_NO_RESPONSE,                                   //!< Server is in no response state
	EM_SERVER_STATUS_ERROR,									//!< Server is in error state		
	EM_SERVER_STATUS_FORCE_DWORD = 0x7FFFFFFF
};
//**********************************************************************
enum ControllerPacketTypeENUM
{
	EM_PG_CtoS_CONNECTED = 1000,								//!< Notify game server connected
	EM_PG_StoC_REGISTRATION,									//!< Reg. information received from game server
	EM_PG_CtoS_NOTIFICATION,									//!< Information send to game server
	EM_PG_StoC_NOTIFICATION_OK,									//!< game server reply receive data ok
	EM_PG_CtoS_COMMAND_TO_SERVER,									//!< Commands strings that should be send to game server
	EM_PG_StoC_CONSOLE_OUTPUT,									//!< Output strings from game server
	EM_PG_StoC_COMMAND_TO_CONTROLLER,									//!< Massage that game server send to controller
	EM_PG_StoC_SERVER_TYPE,									//!< Game server type
	EM_PG_StoC_SERVER_READY,
	EM_PG_CtoM_SERVER_INFO = 1100,								//!< Notify monitor all server info (compressed packet)
	EM_PG_CtoM_CLEAR_SERVER_INFO,									//!< Notify monitor clear all server info
	EM_PG_CtoM_CONSOLE_OUTPUT,									//!< Server output packet(compressed packet)
	EM_PG_CtoM_SERVER_STATUS,									//!< Server dynamic info reply packet
	EM_PG_CtoM_MEM_USAGE,									//!< Server memory usage
	EM_PG_CtoM_CPU_USAGE,									//!< Server CPU usage
	EM_PG_CtoM_CONTROLLER_STOPPING,									//!< Notify monitor that controller will stop service
	EM_PG_CtoM_EVENT_MESSAGE,									//!< Server important event message
	EM_PG_CtoM_CRITICAL_EVENT,									//!< Server error message
	EM_PG_CtoM_SEND_MESSAGE_WITH_MSN,									//!< Notify monitors that the message should send with msn
	EM_PG_CtoM_CENSUS,
	EM_PG_CtoM_WAIT_COUNT,
	EM_PG_CtoM_DYNAMIC_COMMANDS,
	EM_PG_CtoM_AUX,
	EM_PG_CtoM_INFO_ON_START_UP,
	EM_PG_CtoM_POPUP_MESSAGE,
	EM_PG_MtoC_START_SERVER = 1200,								//!< Server start packet
	EM_PG_MtoC_STOP_SERVER,									//!< Server stop packet
	EM_PG_MtoC_SERVER_COMMAND,									//!< Server command packet
	EM_PG_MtoC_SET_CUSTOM_STATUS,									//!< Server custom status packet
	EM_PG_MtoC_ENABLE_SERVER_OUTPUT,									//!< Server output enable/disable packet
	EM_PG_MtoC_RELOAD_CONFIG,									//!< Reload configuration
	EM_PG_MtoC_ENABLE_SERVER,									//!< Enable/disable server
	EM_PG_MtoC_STOP_ALL_SERVER_NOTIFICATION,									//!< Notify controller that server will stop itself
	EM_PG_MtoC_REQUEST_SERVER_STATUS,
	EM_PG_MtoC_REQUEST_CPU_USAGE,
	EM_PG_MtoC_REQUEST_MEM_USAGE,
	EM_PG_MtoC_REQUEST_DYNAMIC_COMMANDS,
	EM_PG_MtoC_REQUEST_CENSUS,
	EM_PG_MtoC_CREATE_DUMP,
	EM_PG_MtoC_RESET_SERVER,									//!< Server reset packet
	EM_PG_MAX_PACKET_COUNT = 1300,
	EM_PG_FORCE_DWORD = 0x7FFFFFFF
};
//**********************************************************************
enum GSRequestTypeENUM
{
	GAME_SERVER_REQUEST_NONE = 0,
	GAME_SERVER_REQUEST_STOP_ME,
	GAME_SERVER_REQUEST_SEND_MSN
};
//**********************************************************************
enum GSTypeENUM
{
	EM_GAME_SERVER_TYPE_NONE = 0,
	EM_GAME_SERVER_TYPE_ACCOUNT,
	EM_GAME_SERVER_TYPE_SERVER_LIST,
	EM_GAME_SERVER_TYPE_SWITCH,
	EM_GAME_SERVER_TYPE_PROXY,
	EM_GAME_SERVER_TYPE_MASTER,
	EM_GAME_SERVER_TYPE_DATACENTER,
	EM_GAME_SERVER_TYPE_PLAYERCENTER,
	EM_GAME_SERVER_TYPE_CHAT,
	EM_GAME_SERVER_TYPE_PARTITION,
	EM_GAME_SERVER_TYPE_ZONE,
	EM_GAME_SERVER_TYPE_NONAGE,
	EM_GAME_SERVER_TYPE_NONAGE_ACCOUNT,
	EM_GAME_SERVER_TYPE_FORWARD,
	EM_GAME_SERVER_TYPE_GATEWAY,
	EM_GAME_SERVER_TYPE_LOGCENTER,
	EM_GAME_SERVER_TYPE_AURORAAGENT,
	EM_GAME_SERVER_TYPE_LUADEBUGAGENT,
	EM_GAME_SERVER_TYPE_BILLING,
};
//**********************************************************************
enum GameServerConsoleDumpType
{
	EM_CONSOLE_DUMP_MIN = 0,
	EM_CONSOLE_DUMP_MID,
	EM_CONSOLE_DUMP_MAX
};
enum ENUM_CRITICAL_EVENT
{
	EM_CRITICAL_EVENT_CRASH = 0,
	EM_CRITICAL_EVENT_NO_RESPONSE,
	EM_CRITICAL_EVENT_DISCONNECTED,
	EM_CRITICAL_EVENT_SERVER_REPORT
};
static unsigned char* GetUniqueKey(int pgid)
{
	switch(pgid)
	{
		case EM_PG_StoC_REGISTRATION:
		case EM_PG_StoC_NOTIFICATION_OK:
		case EM_PG_StoC_CONSOLE_OUTPUT:
		case EM_PG_StoC_COMMAND_TO_CONTROLLER:
		case EM_PG_StoC_SERVER_TYPE:
		case EM_PG_StoC_SERVER_READY:
			{
				return UNIQUE_STOC_KEY;
			}
		case EM_PG_MtoC_START_SERVER:
		case EM_PG_MtoC_STOP_SERVER:
		case EM_PG_MtoC_SERVER_COMMAND:
		case EM_PG_MtoC_SET_CUSTOM_STATUS:
		case EM_PG_MtoC_ENABLE_SERVER_OUTPUT:
		case EM_PG_MtoC_RELOAD_CONFIG:
		case EM_PG_MtoC_ENABLE_SERVER:
		case EM_PG_MtoC_STOP_ALL_SERVER_NOTIFICATION:
		case EM_PG_MtoC_REQUEST_SERVER_STATUS:
		case EM_PG_MtoC_REQUEST_CPU_USAGE:
		case EM_PG_MtoC_REQUEST_MEM_USAGE:
		case EM_PG_MtoC_REQUEST_DYNAMIC_COMMANDS:
		case EM_PG_MtoC_REQUEST_CENSUS:
		case EM_PG_MtoC_CREATE_DUMP:
		case EM_PG_MtoC_RESET_SERVER:
			{
				return UNIQUE_MTOC_KEY;
			}
		case EM_PG_CtoS_CONNECTED:
		case EM_PG_CtoS_NOTIFICATION:
		case EM_PG_CtoS_COMMAND_TO_SERVER:
			{
				return UNIQUE_CTOS_KEY;
			}
		case EM_PG_CtoM_SERVER_INFO:
		case EM_PG_CtoM_CLEAR_SERVER_INFO:
		case EM_PG_CtoM_CONSOLE_OUTPUT:
		case EM_PG_CtoM_SERVER_STATUS:
		case EM_PG_CtoM_MEM_USAGE:
		case EM_PG_CtoM_CPU_USAGE:
		case EM_PG_CtoM_CONTROLLER_STOPPING:
		case EM_PG_CtoM_CRITICAL_EVENT:
		case EM_PG_CtoM_CENSUS:
		case EM_PG_CtoM_WAIT_COUNT:
		case EM_PG_CtoM_DYNAMIC_COMMANDS:
		case EM_PG_CtoM_AUX:
		case EM_PG_CtoM_INFO_ON_START_UP:
		case EM_PG_CtoM_POPUP_MESSAGE:
			{
				return UNIQUE_CTOM_KEY;
			}
	}
	
	return UNIQUE_ERROR_KEY;
}

//**********************************************************************
struct GSConsoleOutput
{
	int  Len;
	char String[1];
	GSConsoleOutput()
	{
		memset(this, 0, sizeof(*this));
	}
};
//**********************************************************************
struct GSInfo
{
	wchar_t RegionName[MAX_SERVER_NAME_LENGTH];
	wchar_t GroupName[MAX_SERVER_NAME_LENGTH];
	wchar_t ServerName[MAX_SERVER_NAME_LENGTH];
	wchar_t	ComputerName[MAX_COMPUTER_NAME_LENGTH];
	char FileVersion[MAX_FILE_VERSION_LENGTH];
	int Index;
	int StartupOrder;
	int WorldID;
	int PID;
	int Status;
	int	SrvType;
	GSInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};
//**********************************************************************
struct GSStatus
{
	int Index;
	int WorldID;
	int PID;
	int Status;
	int SrvType;
	char CustomStatusString[MAX_CUSTOM_STATUS_STRING_LENGTH];
	GSStatus()
	{
		memset(this, 0, sizeof(*this));
	}
};
//**********************************************************************
struct GSMemUsage
{
	int	Index;
	int	TotalMemory;
	int	FreeMemory;
	int	Usage;
	GSMemUsage()
	{
		memset(this, 0, sizeof(*this));
	}
};
//**********************************************************************
struct GSCpuUsage
{
	int	Index;
	int SystemWide;
	int	Usage;
	GSCpuUsage()
	{
		memset(this, 0, sizeof(*this));
	}
};
//**********************************************************************
struct GSCommand
{
	int	Index;
	int InputRequired;
	int Enabled;
	char Caption[MAX_GS_COMMAND_LENGTH];
	char Command[MAX_GS_COMMAND_LENGTH];
	GSCommand()
	{
		memset(this, 0, sizeof(*this));
	}
};
//**********************************************************************
struct GSCensus
{
	int	Index;
	int	Census;
	GSCensus()
	{
		memset(this, 0, sizeof(*this));
	}
};
//**********************************************************************
struct PG_CONTROLLER_BASE
{
	int PGID;
	char Key[UNIQUE_KEY_SIZE];
};
//**********************************************************************
struct PG_CtoM_SERVER_INFO : public PG_CONTROLLER_BASE
{
	int Count;
	GSInfo Info[1];
	PG_CtoM_SERVER_INFO()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_CtoM_SERVER_INFO;
		memcpy(Key, UNIQUE_CTOM_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoM_CLEAR_SERVER_INFO : public PG_CONTROLLER_BASE
{
	PG_CtoM_CLEAR_SERVER_INFO()
	{
		PGID = EM_PG_CtoM_CLEAR_SERVER_INFO;
		memcpy(Key, UNIQUE_CTOM_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoM_CONSOLE_OUTPUT : public PG_CONTROLLER_BASE
{
	int Index;
	int Count;
	GSConsoleOutput Output[1];
	PG_CtoM_CONSOLE_OUTPUT()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_CtoM_CONSOLE_OUTPUT;
		memcpy(Key, UNIQUE_CTOM_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoM_SERVER_STATUS : public PG_CONTROLLER_BASE
{
	int Count;
	GSStatus Status[1];
	PG_CtoM_SERVER_STATUS()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_CtoM_SERVER_STATUS;
		memcpy(Key, UNIQUE_CTOM_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoM_MEM_USAGE : public PG_CONTROLLER_BASE
{
	int Count;
	GSMemUsage Usage[1];
	PG_CtoM_MEM_USAGE()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_CtoM_MEM_USAGE;
		memcpy(Key, UNIQUE_CTOM_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoM_CPU_USAGE : public PG_CONTROLLER_BASE
{
	int Count;
	GSCpuUsage Usage[1];
	PG_CtoM_CPU_USAGE()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_CtoM_CPU_USAGE;
		memcpy(Key, UNIQUE_CTOM_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoM_CONTROLLER_STOPPING : public PG_CONTROLLER_BASE
{
	PG_CtoM_CONTROLLER_STOPPING()
	{
		PGID = EM_PG_CtoM_CONTROLLER_STOPPING;
		memcpy(Key, UNIQUE_CTOM_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoM_CRITICAL_EVENT : public PG_CONTROLLER_BASE
{
	int Index;							//!< Server index
	int EventType;
	char Time[32];
	char Message[1];
	PG_CtoM_CRITICAL_EVENT()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_CtoM_CRITICAL_EVENT;
		memcpy(Key, UNIQUE_CTOM_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoM_CENSUS : public PG_CONTROLLER_BASE
{
	int Count;
	GSCensus Census[1];
	PG_CtoM_CENSUS()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_CtoM_CENSUS;
		memcpy(Key, UNIQUE_CTOM_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoM_WAIT_COUNT : public PG_CONTROLLER_BASE
{
	int	Index;
	int	WaitingCount;
	int	LoadingCount;
	PG_CtoM_WAIT_COUNT()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_CtoM_WAIT_COUNT;
		memcpy(Key, UNIQUE_CTOM_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoM_DYNAMIC_COMMANDS : public PG_CONTROLLER_BASE
{
	int Count;
	GSCommand Command[1];
	PG_CtoM_DYNAMIC_COMMANDS()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_CtoM_DYNAMIC_COMMANDS;
		memcpy(Key, UNIQUE_CTOM_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoM_AUX : public PG_CONTROLLER_BASE
{
	int Index;
	int Aux;
	int Value;
	PG_CtoM_AUX()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_CtoM_AUX;
		memcpy(Key, UNIQUE_CTOM_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoM_INFO_ON_START_UP : public PG_CONTROLLER_BASE
{
	int Index;
	char FileVersion[MAX_FILE_VERSION_LENGTH];
	PG_CtoM_INFO_ON_START_UP()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_CtoM_INFO_ON_START_UP;
		memcpy(Key, UNIQUE_CTOM_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoM_POPUP_MESSAGE : public PG_CONTROLLER_BASE
{
	int Len;
	char Message[1];
	PG_CtoM_POPUP_MESSAGE()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_CtoM_POPUP_MESSAGE;
		memcpy(Key, UNIQUE_CTOM_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_START_SERVER : public PG_CONTROLLER_BASE
{
	int	Index;
	PG_MtoC_START_SERVER()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_MtoC_START_SERVER;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_STOP_SERVER : public PG_CONTROLLER_BASE
{
	int	Index;
	PG_MtoC_STOP_SERVER()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_MtoC_STOP_SERVER;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_SERVER_COMMAND : public PG_CONTROLLER_BASE
{
	int Index;
	int Len;
	char Command[1];
	PG_MtoC_SERVER_COMMAND()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_MtoC_SERVER_COMMAND;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_SET_CUSTOM_STATUS : public PG_CONTROLLER_BASE
{
	int Index;
	int Status;
	char String[32];
	PG_MtoC_SET_CUSTOM_STATUS()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_MtoC_SET_CUSTOM_STATUS;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_ENABLE_SERVER_OUTPUT : public PG_CONTROLLER_BASE
{
	int	Index;
	PG_MtoC_ENABLE_SERVER_OUTPUT()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_MtoC_ENABLE_SERVER_OUTPUT;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_RELOAD_CONFIG : public PG_CONTROLLER_BASE
{
	PG_MtoC_RELOAD_CONFIG()
	{
		PGID = EM_PG_MtoC_RELOAD_CONFIG;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_ENABLE_SERVER : public PG_CONTROLLER_BASE
{
	int	Index;
	int	Enable;
	PG_MtoC_ENABLE_SERVER()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_MtoC_ENABLE_SERVER;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_STOP_ALL_SERVER_NOTIFICATION : public PG_CONTROLLER_BASE
{
	PG_MtoC_STOP_ALL_SERVER_NOTIFICATION()
	{
		PGID = EM_PG_MtoC_STOP_ALL_SERVER_NOTIFICATION;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_REQUEST_SERVER_STATUS : public PG_CONTROLLER_BASE
{
	PG_MtoC_REQUEST_SERVER_STATUS()
	{
		PGID = EM_PG_MtoC_REQUEST_SERVER_STATUS;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_REQUEST_CPU_USAGE : public PG_CONTROLLER_BASE
{
	PG_MtoC_REQUEST_CPU_USAGE()
	{
		PGID = EM_PG_MtoC_REQUEST_CPU_USAGE;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_REQUEST_MEM_USAGE : public PG_CONTROLLER_BASE
{
	PG_MtoC_REQUEST_MEM_USAGE()
	{
		PGID = EM_PG_MtoC_REQUEST_MEM_USAGE;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_REQUEST_DYNAMIC_COMMANDS : public PG_CONTROLLER_BASE
{
	PG_MtoC_REQUEST_DYNAMIC_COMMANDS()
	{
		PGID = EM_PG_MtoC_REQUEST_DYNAMIC_COMMANDS;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_REQUEST_CENSUS : public PG_CONTROLLER_BASE
{
	PG_MtoC_REQUEST_CENSUS()
	{
		PGID = EM_PG_MtoC_REQUEST_CENSUS;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_CREATE_DUMP : public PG_CONTROLLER_BASE
{
	int Index;
	int DumpType;
	PG_MtoC_CREATE_DUMP()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_MtoC_CREATE_DUMP;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_MtoC_RESET_SERVER : public PG_CONTROLLER_BASE
{
	int	Index;
	PG_MtoC_RESET_SERVER()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_MtoC_RESET_SERVER;
		memcpy(Key, UNIQUE_MTOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoS_CONNECTED : public PG_CONTROLLER_BASE
{
	PG_CtoS_CONNECTED()
	{
		PGID = EM_PG_CtoS_CONNECTED;
		memcpy(Key, UNIQUE_CTOS_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_StoC_REGISTRATION : public PG_CONTROLLER_BASE
{
	unsigned long PID;
	char RegionName[MAX_SERVER_NAME_LENGTH];
	char GroupName[MAX_SERVER_NAME_LENGTH];
	char ServerName[MAX_SERVER_NAME_LENGTH];
	char CmdLine[MAX_SERVER_FILE_PATH_NAME_LENGTH];
	PG_StoC_REGISTRATION()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_StoC_REGISTRATION;
		memcpy(Key, UNIQUE_STOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoS_NOTIFICATION : public PG_CONTROLLER_BASE
{
	char RegionName[MAX_SERVER_NAME_LENGTH];
	char GroupName[MAX_SERVER_NAME_LENGTH];
	char ServerName[MAX_SERVER_NAME_LENGTH];
	PG_CtoS_NOTIFICATION()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_CtoS_NOTIFICATION;
		memcpy(Key, UNIQUE_CTOS_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_StoC_NOTIFICATION_OK : public PG_CONTROLLER_BASE
{
	PG_StoC_NOTIFICATION_OK()
	{
		PGID = EM_PG_StoC_NOTIFICATION_OK;
		memcpy(Key, UNIQUE_STOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_CtoS_COMMAND_TO_SERVER : public PG_CONTROLLER_BASE
{
	int Len;
	char Cmd[1];
	PG_CtoS_COMMAND_TO_SERVER()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_CtoS_COMMAND_TO_SERVER;
		memcpy(Key, UNIQUE_CTOS_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_StoC_CONSOLE_OUTPUT : public PG_CONTROLLER_BASE
{
	int Len;
	char Output[1];
	PG_StoC_CONSOLE_OUTPUT()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_StoC_CONSOLE_OUTPUT;
		memcpy(Key, UNIQUE_STOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_StoC_COMMAND_TO_CONTROLLER : public PG_CONTROLLER_BASE
{
	int Command;
	int Len;
	char Message[1];
	PG_StoC_COMMAND_TO_CONTROLLER()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_StoC_COMMAND_TO_CONTROLLER;
		memcpy(Key, UNIQUE_STOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_StoC_SERVER_TYPE : public PG_CONTROLLER_BASE
{
	int Type;
	PG_StoC_SERVER_TYPE()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_StoC_SERVER_TYPE;
		memcpy(Key, UNIQUE_STOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
struct PG_StoC_SERVER_READY : public PG_CONTROLLER_BASE
{
	PG_StoC_SERVER_READY()
	{
		memset(this, 0, sizeof(*this));
		PGID = EM_PG_StoC_SERVER_READY;
		memcpy(Key, UNIQUE_STOC_KEY, UNIQUE_KEY_SIZE);
	}
};
//**********************************************************************
#pragma pack(pop)
#pragma managed(pop)
#endif