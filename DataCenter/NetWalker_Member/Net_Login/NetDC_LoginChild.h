#pragma once

#include "NetDC_Login.h"

//*******************************************************************
enum OnTimeLoadStateENUM
{
	EM_ON_TIME_LOAD_STATE_WAITING	,
	EM_ON_TIME_LOAD_STATE_LOADING	,
	EM_ON_TIME_LOAD_STATE_FINISH	,
};


struct LBOnlineTime
{
	int  Time;
	bool Enable;

	LBOnlineTime()
	{
		Init();
	}

	void Init()
	{
		Time = 0;
		Enable = false;
	}
};

struct LBProject
{
	LBStatusENUM										Status;
	LBBox												Box;
	std::map<std::string, std::map<int, std::set<int>>> Receipts;

	LBProject()
	{
		Init();
	}

	void Init()
	{
		Status = EM_LBStatus_Normal;
		Box.Init();
		Receipts.clear();
	}
};

//*******************************************************************
class CNetDC_LoginChild : public CNetDC_Login
{
protected:
	bool												m_Enable;

	int													m_LastYDay;
	std::map<std::string, LBOnlineTime>					m_AccountOnlineTime;
	std::map<int, LBOnlineTime>							m_RoleOnlineTime;
	
	std::map<int, LBProject>							m_Projects;
	std::map<int, std::wstring>							m_Descriptions;
	std::map<std::string, std::map<int, std::set<int>>> m_Stamps;

	unsigned long										m_LastIniTick;

	MutilThread_CritSect								m_OnTimeLoadLock;
	void*												m_OnTimeLoadTemp;
	int													m_OnTimeLoadState;
	unsigned long										m_LastLoadTick;

public:
	static bool Init();
	static bool Release();
	static CNetDC_LoginChild* This();

public:
	CNetDC_LoginChild();
	virtual ~CNetDC_LoginChild();

protected:
	void ReadIni();

protected:
	static int OnTimeInit(SchedularInfo* obj, void* inputclass);
	static int OnTimeProc(SchedularInfo* obj, void* inputclass);

protected:
	void DB_OnTimeLoad	 ();
	void DB_InsertStamp	 (int playerdbid, const char* account);
	void DB_InsertReceipt(int playerdbid, const char* account, int id);
	void DB_UpdateStatus (int id, int status);

protected:
	static void _SQLLoadLoginBonus	(SqlBaseClass* sqlbase, std::vector<LBBox>& result);
	static void _SQLLoadDescriptions(SqlBaseClass* sqlbase, std::map<int, std::wstring>& result);

protected:
	static bool _SQLInit				(std::vector<int>& vec, SqlBaseClass* sqlbase, void* userobj, ArgTransferStruct& arg);
	static void _SQLInitResult			(std::vector<int>& vec, void* userobj, ArgTransferStruct& arg, bool resultok);
	static bool _SQLOnTimeLoad			(std::vector<int>& vec, SqlBaseClass* sqlbase, void* userobj, ArgTransferStruct& arg);
	static void _SQLOnTimeLoadResult	(std::vector<int>& vec, void* userobj, ArgTransferStruct& arg, bool resultok);
	static bool _SQLInsertStamp			(std::vector<int>& vec, SqlBaseClass* sqlbase, void* userobj, ArgTransferStruct& arg);
	static void _SQLInsertStampResult	(std::vector<int>& vec, void* userobj, ArgTransferStruct& arg, bool resultok);
	static bool _SQLInsertReceipt		(std::vector<int>& vec, SqlBaseClass* sqlbase, void* userobj, ArgTransferStruct& arg);
	static void _SQLInsertReceiptResult	(std::vector<int>& vec, void* userobj, ArgTransferStruct& arg, bool resultok);
	static bool _SQLUpdateStatus		(std::vector<int>& vec, SqlBaseClass* sqlbase, void* userobj, ArgTransferStruct& arg);
	static void _SQLUpdateStatusResult	(std::vector<int>& vec, void* userobj, ArgTransferStruct& arg, bool resultok);

public:
	virtual void RL_RequestBonusList	(int srvid, int playerdbid, const char* account, int page);
	virtual void RL_RequestBonus		(int srvid, int playerdbid, const char* account, int id);
	virtual void RL_RequestBonusComplete(int srvid, int playerdbid, const char* account, int id, LBRequestResultENUM result);
	virtual void RL_CliConnect			(int srvid, int playerdbid, const char* account);
	virtual void RL_CliDisconnect		(int srvid, int playerdbid, const char* account);
};
//*******************************************************************