#pragma once
#ifndef __NET_BILLING_H__
#define __NET_BILLING_H__

#include "PG/PG_Billing.h"
#include "../../MainProc/Global.h"
#include <map>

class Net_Billing : public Global
{
protected:
    static Net_Billing*					  This;
	static int							  CBID;
	static std::map<int, BillingResultCB> ResultCB;
	static int							  BillingSystemStatus;
	static bool							  BillingConnected;

protected:
	static bool _Init();
	static bool _Release();

protected:
	static void _PG_Billing_BtoL_BalanceCheckResult	  (int netid, int size, void* data);
	static void _PG_Billing_BtoL_AddAccountMoneyResult(int netid, int size, void* data);
	static void _PG_Billing_BtoL_BillingSystemStatus  (int netid, int size, void* data);

protected:
	static void	_OnBillingConnect    (EM_SERVER_TYPE servertype, unsigned long localid);
	static void	_OnBillingDisconnect (EM_SERVER_TYPE servertype, unsigned long localid);
	static void _OnCliConnect		 (int cliid, std::string account);

public:
	static void SB_BalanceCheck				 (const char* account, int playerdbid, int customid, BillingResultCB cb = NULL);
	static void SB_AddAccountMoney			 (const char* account, int playerdbid, int amount, int customid, BillingResultCB cb = NULL);
	static void SB_RequestBillingSystemStatus();
	static void SB_MoneyInfo				 (const char* account, int playerdbid, int bonusmoney, int phiriuscoin);

public:
	virtual void RB_BalanceCheckResult	 (BaseItemObject* obj, const char* account, int balance, int result, int customid, int cbid) = 0;
	virtual void RB_AddAccountMoneyResult(BaseItemObject* obj, const char* account, int balance, int result, int customid, int cbid) = 0;
	virtual void RB_BillingSystemStatus  (int status) = 0;

public:
	static bool BillingSystemAvaliable();

public:
	void CallResultCB(int cbid, void* userdata);
};

#endif