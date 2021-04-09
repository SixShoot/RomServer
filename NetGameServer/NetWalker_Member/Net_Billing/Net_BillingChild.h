#pragma once
#ifndef __NET_BILLING_CHILD_H__
#define __NET_BILLING_CHILD_H__

#include "Net_Billing.h"

//****************************************************************************************
class Net_BillingChild : public Net_Billing
{
public:
	Net_BillingChild();
	virtual ~Net_BillingChild();

public:
    static bool Init();
    static bool Release();

public:
	virtual void RB_BalanceCheckResult	 (BaseItemObject* obj, const char* account, int balance, int result, int customid, int cbid);
	virtual void RB_AddAccountMoneyResult(BaseItemObject* obj, const char* account, int balance, int result, int customid, int cbid);
	virtual void RB_BillingSystemStatus  (int status);
};

#endif
