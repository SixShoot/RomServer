#include "Net_BillingChild.h"

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
Net_BillingChild::Net_BillingChild()
{

}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
Net_BillingChild::~Net_BillingChild()
{

}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
bool Net_BillingChild::Init()
{
    Net_Billing::_Init();

    if( This != NULL)
        return false;

    This = new Net_BillingChild();

    return true;
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
bool Net_BillingChild::Release()
{
    if( This == NULL )
        return false;

    delete This;
    This = NULL;

    Net_Billing::_Release();
    return true;
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_BillingChild::RB_BalanceCheckResult(BaseItemObject* obj, const char* account, int balance, int result, int customid, int cbid)
{
	RoleDataEx* pRole = obj->Role();

	if (pRole != NULL)
	{
		pRole->TempData.IsDisabledChangeZone = false;

		if (result == EM_BillingResult_OK)
		{
			if (balance != pRole->PlayerBaseData->Body.Money_Account)
			{
				pRole->PlayerBaseData->Body.Money_Account = balance;
				pRole->Net_FixMoney();

				Print(LV2 , "RB_BalanceCheckResult" , "Fix Balance, Account = %s, balance = %d", account, balance);
			}
			else
			{
				Print(LV2 , "RB_BalanceCheckResult" , "Same Balance, Account = %s, balance = %d", account, balance);
			}
		}
		else
		{
			Print(LV2 , "RB_BalanceCheckResult" , "Result failed!!");
		}

		//©I¥sCallback¨ç¦¡
		BillingResultData Data;
		Data.PlayerDBID	= pRole->DBID();
		Data.Account	= pRole->Account_ID();
		Data.Balance	= balance;
		Data.Result		= result;
		Data.CustomID	= customid;

		CallResultCB(cbid, &Data);
	}
	else
	{
		Print(LV2 , "RB_BalanceCheckResult" , "Role = NULL!!");
	}
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_BillingChild::RB_AddAccountMoneyResult(BaseItemObject* obj, const char* account, int balance, int result, int customid, int cbid)
{
	RoleDataEx* pRole = obj->Role();

	if (pRole != NULL)
	{
		pRole->TempData.IsDisabledChangeZone = false;

		if (result == EM_BillingResult_OK)
		{
			if (balance != pRole->PlayerBaseData->Body.Money_Account)
			{
				pRole->PlayerBaseData->Body.Money_Account = balance;
				pRole->Net_FixMoney();

				Print(LV2 , "RB_AddAccountMoneyResult" , "Fix Balance, Account = %s, balance = %d", account, balance);
			}
			else
			{
				Print(LV2 , "RB_AddAccountMoneyResult" , "Same Balance, Account = %s, balance = %d", account, balance);
			}
		}
		else
		{
			Print(LV2 , "RB_AddAccountMoneyResult" , "Result failed!!");
		}

		//©I¥sCallback¨ç¦¡
		BillingResultData Data;
		Data.PlayerDBID	= pRole->DBID();
		Data.Account	= pRole->Account_ID();
		Data.Balance	= balance;
		Data.Result		= result;
		Data.CustomID	= customid;

		CallResultCB(cbid, &Data);
	}
	else
	{
		Print(LV2 , "RB_BalanceCheckResult" , "Role = NULL!!");
	}
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void Net_BillingChild::RB_BillingSystemStatus(int status)
{
	BillingSystemStatus = status;
}

//**********************************************************************************************