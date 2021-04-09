#include "Net_Billing.h"
#include "../Net_ServerList/Net_ServerList_Child.h"

Net_Billing*				   Net_Billing::This = NULL;
int							   Net_Billing::CBID = 0;
std::map<int, BillingResultCB> Net_Billing::ResultCB;
int							   Net_Billing::BillingSystemStatus = EM_BillingSystemStatus_Offline;
bool						   Net_Billing::BillingConnected = false;

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
bool Net_Billing::_Init()
{
	_Net->RegOnSrvPacketFunction(EM_PG_Billing_BtoL_BalanceCheckResult	 , _PG_Billing_BtoL_BalanceCheckResult   );
	_Net->RegOnSrvPacketFunction(EM_PG_Billing_BtoL_AddAccountMoneyResult, _PG_Billing_BtoL_AddAccountMoneyResult);
	_Net->RegOnSrvPacketFunction(EM_PG_Billing_BtoL_BillingSystemStatus	 , _PG_Billing_BtoL_BillingSystemStatus	 );

	Global::Net_ServerList->RegServerLoginEvent(EM_SERVER_TYPE_BILLING, _OnBillingConnect);
	Global::Net_ServerList->RegServerLogoutEvent(EM_SERVER_TYPE_BILLING, _OnBillingDisconnect);
	_Net->RegOnCliConnectFunction( _OnCliConnect );
    return true;
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
bool Net_Billing::_Release()
{
    return true;
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_Billing::_PG_Billing_BtoL_BalanceCheckResult(int netid, int size, void* data)
{
	PG_Billing_BtoL_BalanceCheckResult* PG = (PG_Billing_BtoL_BalanceCheckResult*)data;
	BaseItemObject* Obj = BaseItemObject::GetObjByDBID(PG->PlayerDBID);
	if(Obj == NULL)
		return;

	This->RB_BalanceCheckResult(Obj, PG->Account.Begin(), PG->Balance, PG->Result, PG->CustomID, PG->CBID);
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_Billing::_PG_Billing_BtoL_AddAccountMoneyResult(int netid, int size, void* data)
{
	PG_Billing_BtoL_AddAccountMoneyResult* PG = (PG_Billing_BtoL_AddAccountMoneyResult*)data;
	BaseItemObject* Obj = BaseItemObject::GetObjByDBID(PG->PlayerDBID);
	if(Obj == NULL)
		return;

	This->RB_AddAccountMoneyResult(Obj, PG->Account.Begin(), PG->Balance, PG->Result, PG->CustomID, PG->CBID);
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_Billing::_PG_Billing_BtoL_BillingSystemStatus(int netid, int size, void* data)
{
	PG_Billing_BtoL_BillingSystemStatus* PG = (PG_Billing_BtoL_BillingSystemStatus*)data;
	This->RB_BillingSystemStatus(PG->Status);
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_Billing::_OnBillingConnect(EM_SERVER_TYPE servertype, unsigned long localid)
{
	SB_RequestBillingSystemStatus();
	BillingConnected = true;
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_Billing::_OnBillingDisconnect(EM_SERVER_TYPE servertype, unsigned long localid)
{
	BillingConnected = false;
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_Billing::_OnCliConnect(int cliid, std::string account)
{
	M_CLIENT_OBJ(cliid);

	if (pObj != NULL)
	{
		pObj->Role()->Net_BillingNotify2();
	}
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_Billing::SB_BalanceCheck(const char* account, int playerdbid, int customid, BillingResultCB cb)
{
	if (Global::St()->IsUseBillingServer == false)
		return;

	RoleDataEx* pRole = GetRoleDataByDBID(playerdbid);

	if (pRole != NULL)
	{
		PG_Billing_LtoB_BalanceCheck Send;
		Send.Account	= account;
		Send.PlayerDBID = playerdbid;
		Send.CustomID	= customid;

		if (cb != NULL)
		{
			CBID++;
			ResultCB[CBID] = cb;
			Send.CBID = CBID;
		}

		if (Global::Net_ServerList->SendToServer(EM_SERVER_TYPE_BILLING, DF_NO_LOCALID, sizeof(Send), &Send) == true)
		{
			Print(LV2 , "SB_BalanceCheck" , "Account = %s", account);

			//暫時不能換區
			pRole->TempData.IsDisabledChangeZone = true;
		}
		else
		{
			if (cb != NULL)
			{
				ResultCB.erase(CBID);
			}

			Print(LV2 , "SB_BalanceCheck" , "Send Failed!!");
		}
	}
	else
	{
		Print(LV2 , "SB_BalanceCheck" , "Role = NULL!!");
	}
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_Billing::SB_AddAccountMoney(const char* account, int playerdbid, int amount, int customid, BillingResultCB cb)
{
	if (Global::St()->IsUseBillingServer == false)
		return;

	RoleDataEx* pRole = GetRoleDataByDBID(playerdbid);

	if (pRole != NULL)
	{
		PG_Billing_LtoB_AddAccountMoney Send;
		Send.Account	= account;
		Send.PlayerDBID = playerdbid;
		Send.Amount		= amount;
		Send.CustomID	= customid;

		if (cb != NULL)
		{
			CBID++;
			ResultCB[CBID] = cb;
			Send.CBID = CBID;
		}

		if (Global::Net_ServerList->SendToServer(EM_SERVER_TYPE_BILLING, DF_NO_LOCALID, sizeof(Send), &Send) == true)
		{
			Print(LV2 , "SB_AddAccountMoney" , "Account = %s", account);

			//暫時不能換區
			pRole->TempData.IsDisabledChangeZone = true;
		}
		else
		{
			if (cb != NULL)
			{
				ResultCB.erase(CBID);
			}
		}
	}
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_Billing::SB_RequestBillingSystemStatus()
{
	PG_Billing_LtoB_RequestBillingSystemStatus Send;
	Global::Net_ServerList->SendToServer(EM_SERVER_TYPE_BILLING, DF_NO_LOCALID, sizeof(Send), &Send);
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_Billing::SB_MoneyInfo(const char* account, int playerdbid, int bonusmoney, int phiriuscoin)
{
	if (Global::St()->IsUseBillingServer == false)
		return;

	PG_Billing_LtoB_MoneyInfo Send;
	Send.Account = account;
	Send.PlayerDBID = playerdbid;
	Send.WorldID = _Net->GetWorldID();
	Send.BonusMoney = bonusmoney;
	Send.PhiriusCoin = phiriuscoin;

	Global::Net_ServerList->SendToServer(EM_SERVER_TYPE_BILLING, DF_NO_LOCALID, sizeof(Send), &Send);
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
bool Net_Billing::BillingSystemAvaliable()
{
	return BillingConnected && (BillingSystemStatus == EM_BillingSystemStatus_Online);
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_Billing::CallResultCB(int cbid, void* userdata)
{
	std::map<int, BillingResultCB>::iterator it = ResultCB.find(cbid);

	if (it != ResultCB.end())
	{
		BillingResultCB cb = it->second;
		if (cb != NULL)
		{
			cb(userdata);
		}
		ResultCB.erase(it);
	}
}