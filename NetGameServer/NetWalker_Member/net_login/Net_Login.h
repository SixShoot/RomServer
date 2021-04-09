#pragma once
#ifndef __NET_LOGIN_H__
#define __NET_LOGIN_H__

#include "PG/PG_Login.h"
#include "../../MainProc/Global.h"

class Net_Login : public Global
{
protected:
    static Net_Login* s_This;

protected:

	struct _LoginInfo
	{
		std::string Account;
		int			PlayerDBID;
	};

	std::map<int, _LoginInfo> m_LoginInfo;	//cliid - LoginInfo

protected:
	static void _PG_Login_CtoL_RequestBonusList	 (int netid, int size, void* data);
	static void _PG_Login_CtoL_RequestBonus		 (int netid, int size, void* data);
	static void _PG_Login_DtoL_BonusList		 (int netid, int size, void* data);
	static void _PG_Login_DtoL_RequestBonusResult(int netid, int size, void* data);
	static void _PG_Login_DtoL_BonusPeriod		 (int netid, int size, void* data);

protected:
	static void _OnCliConnect	(int cliid, std::string account);
	static void _OnCliDisconnect(int cliid);

protected:
	static bool _Init();
	static bool _Release();

public:
	static void SC_BonusList		   (int guid, std::vector<LBBox>* boxes, int year, int month, int mday, int page, const wchar_t* description);
	static void SC_RequestBonusResult  (int guid, int id, LBRequestResultENUM result);
	static void SC_BonusPeriod		   (int guid, int maxpage, int minpage);
	static void SD_RequestBonusList	   (int playerdbid, const char* account, int page);
	static void SD_RequestBonus		   (int playerdbid, const char* account, int id);
	static void SD_RequestBonusComplete(int playerdbid, const char* account, int id, LBRequestResultENUM result);
	static void SD_CliConnect		   (int playerdbid, const char* account);
	static void SD_CliDisconnect	   (int playerdbid, const char* account);

public:
	virtual void RC_RequestBonusList  (BaseItemObject* obj, int day) = 0;
	virtual void RC_RequestBonus	  (BaseItemObject* obj, int id) = 0;
	virtual void RD_BonusList		  (BaseItemObject* obj, std::vector<LBBox>& boxes, int year, int month, int mday, int page, const wchar_t* description) = 0;
	virtual void RD_RequestBonusResult(int playerdbid, const char* account, int id, int money, std::vector<LBGoods>& goods, LBRequestResultENUM result) = 0;
	virtual void RD_BonusPeriod		  (BaseItemObject* obj, int maxpage, int minpage) = 0;

public:
	virtual void CliConnect	  (int playerdbid, const char* account) = 0;
	virtual void CliDisconnect(int playerdbid, const char* account) = 0;
};

#endif