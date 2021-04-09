#pragma once
#ifndef __NET_LOGIN_CHILD_H__
#define __NET_LOGIN_CHILD_H__

#include "Net_Login.h"

//****************************************************************************************
class Net_LoginChild : public Net_Login
{
public:
    static bool Init();
    static bool Release();

protected:
	bool CreateItem(int orgobjid, int count, std::vector<ItemFieldStruct>& items);
	bool CheckBagSpace(RoleDataEx* role, std::vector<ItemFieldStruct>& items);

public:
	virtual void RC_RequestBonusList  (BaseItemObject* obj, int page);
	virtual void RC_RequestBonus	  (BaseItemObject* obj, int id);
	virtual void RD_BonusList		  (BaseItemObject* obj, std::vector<LBBox>& boxes, int year, int month, int mday, int page, const wchar_t* description);
	virtual void RD_RequestBonusResult(int playerdbid, const char* account, int id, int money, std::vector<LBGoods>& goods, LBRequestResultENUM result);
	virtual void RD_BonusPeriod		  (BaseItemObject* obj, int maxpage, int minpage);

public:
	virtual void CliConnect	  (int playerdbid, const char* account);
	virtual void CliDisconnect(int playerdbid, const char* account);
};

#endif
