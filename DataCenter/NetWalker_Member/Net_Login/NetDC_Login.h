#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_Login.h"

class CNetDC_Login : public Global
{
protected:
	static CNetDC_Login* s_This;

protected:
	static void _PG_Login_LtoD_RequestBonusList	   (int srvid, int size, void* data);
	static void _PG_Login_LtoD_RequestBonus		   (int srvid, int size, void* data);
	static void _PG_Login_LtoD_RequestBonusComplete(int srvid, int size, void* data);
	static void _PG_Login_LtoD_CliConnect		   (int srvid, int size, void* data);
	static void _PG_Login_LtoD_CliDisconnect	   (int srvid, int size, void* data);

protected:
	static bool	_Init();
	static bool	_Release();

public:
	static void SL_BonusList		 (int srvid, int playerdbid, std::vector<LBBox>* boxes, int year, int month, int mday, int page, const wchar_t* description);
	static void SL_RequestBonusResult(int srvid, int playerdbid, const char* account, int id, int money, std::vector<LBGoods>* goods, LBRequestResultENUM result);
	static void SL_BonusPeriod		 (int srvid, int playerdbid, int maxpage, int minpage);

public:
	virtual void RL_RequestBonusList	(int srvid, int playerdbid, const char* account, int page) = 0;
	virtual void RL_RequestBonus		(int srvid, int playerdbid, const char* account, int id) = 0;
	virtual void RL_RequestBonusComplete(int srvid, int playerdbid, const char* account, int id, LBRequestResultENUM result) = 0;
	virtual void RL_CliConnect			(int srvid, int playerdbid, const char* account) = 0;
	virtual void RL_CliDisconnect		(int srvid, int playerdbid, const char* account) = 0;
};

