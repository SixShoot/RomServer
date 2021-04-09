#include "Net_LoginChild.h"
#include "../../MainProc/LuaPlot/LuaPlot.h"

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool Net_LoginChild::Init()
{
	if(s_This != NULL)
		return false;

	Net_Login::_Init();

	s_This = new Net_LoginChild();

	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool Net_LoginChild::Release()
{
	if(s_This == NULL)
		return false;

	Net_Login::_Release();

	delete s_This;
	s_This = NULL;

	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool Net_LoginChild::CreateItem(int orgobjid, int count, std::vector<ItemFieldStruct>& items)
{
	GameObjDbStructEx* pObjDB = NULL;

	pObjDB = g_ObjectData->GetObj(orgobjid);

	//物品資料錯誤
	if((pObjDB == NULL) || (count <= 0))
		return false;

	if (pObjDB->IsItem() == false)
		return false;

	int MaxHeap = pObjDB->MaxHeap;

	//只要還有剩餘數量, 就繼續產生新物品
	while (count > 0)
	{
		ItemFieldStruct	Temp;
		Temp.Init();

		if (Global::NewItemInit(Temp, orgobjid, 0) == false)
		{
			return false;
		}

		//剩餘數量超過最大堆疊數
		if (count > MaxHeap)
		{
			Temp.Count = MaxHeap;
			count	  -= MaxHeap;
		}
		else
		{
			//剩餘數量小於最大堆疊數
			Temp.Count = count;
			count	   = 0;
		}

		items.push_back(Temp);
	}

	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool Net_LoginChild::CheckBagSpace(RoleDataEx* role, std::vector<ItemFieldStruct>& items)
{
	if (role == NULL)
		return false;

	//檢查人物身上的暫存空間
	if((items.size() + role->PlayerBaseData->ItemTemp.Size()) >= _MAX_TEMP_COUNT_)
		return false;

	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_LoginChild::RC_RequestBonusList(BaseItemObject* obj, int page)
{
	RoleDataEx* role = obj->Role();

	if (role == NULL)
		return;

	SD_RequestBonusList(role->DBID(), role->Account_ID(), page);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_LoginChild::RC_RequestBonus(BaseItemObject* obj, int id)
{
	RoleDataEx* role = obj->Role();

	if (role == NULL)
		return;

	SD_RequestBonus(role->DBID(), role->Account_ID(), id);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_LoginChild::RD_BonusList(BaseItemObject* obj, std::vector<LBBox>& boxes, int year, int month, int mday, int page, const wchar_t* description)
{
	RoleDataEx* role = obj->Role();

	if (role == NULL)
		return;

	SC_BonusList(role->GUID(), &boxes, year, month, mday, page, description);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_LoginChild::RD_RequestBonusResult(int playerdbid, const char* account, int id, int money, std::vector<LBGoods>& goods, LBRequestResultENUM result)
{
	//----------------------------------------------------------
	BaseItemObject* obj = BaseItemObject::GetObjByDBID(playerdbid);

	if (obj == NULL)
	{
		SD_RequestBonusComplete(playerdbid, account, id, EM_LBRequestResult_BagFull);
		return;
	}

	RoleDataEx* role = obj->Role();

	if (role == NULL)
	{
		SD_RequestBonusComplete(playerdbid, account, id, EM_LBRequestResult_BagFull);
		return;
	}
	//----------------------------------------------------------

	//----------------------------------------------------------
	if (result != EM_LBRequestResult_OK)
	{
		SC_RequestBonusResult(role->GUID(), id, result);
		return;
	}
	//----------------------------------------------------------

	//----------------------------------------------------------
	//嘗試產生物品
	std::vector<ItemFieldStruct> Items;

	for (unsigned i = 0; i < goods.size(); ++i)
	{
		LBGoods& GoodsRef = goods[i];

		if (GoodsRef.Type != EM_LBGoodsType_Item)
			continue;

		if (CreateItem(GoodsRef.Item.OrgObjID, GoodsRef.Item.Count, Items) == false)
		{
			result = EM_LBRequestResult_ItemDataError;
			break;
		}
	}
	//----------------------------------------------------------

	//----------------------------------------------------------
	if (result == EM_LBRequestResult_ItemDataError)
	{
		SD_RequestBonusComplete(role->DBID(), role->Account_ID(), id, EM_LBRequestResult_ItemDataError);
		SC_RequestBonusResult(role->GUID(), id, EM_LBRequestResult_ItemDataError);
		return;
	}
	//----------------------------------------------------------

	//----------------------------------------------------------
	if (CheckBagSpace(role, Items) == false)
	{
		SD_RequestBonusComplete(role->DBID(), role->Account_ID(), id, EM_LBRequestResult_BagFull);
		SC_RequestBonusResult(role->GUID(), id, EM_LBRequestResult_BagFull);
		return;
	}
	//----------------------------------------------------------

	//----------------------------------------------------------
	//給予物品
	for (unsigned i = 0; i < Items.size(); ++i)
	{
		role->GiveItem(Items[i], EM_ActionType_LoginBonus, NULL, "");
	}
	//----------------------------------------------------------

	//----------------------------------------------------------
	//給予遊戲幣
	if (money > 0)
	{
		role->AddBodyMoney(money, NULL, EM_ActionType_LoginBonus, true);
	}
	//----------------------------------------------------------

	//----------------------------------------------------------
	//給予Buff
	for (unsigned i = 0; i < goods.size(); ++i)
	{
		LBGoods& GoodsRef = goods[i];

		if (GoodsRef.Type != EM_LBGoodsType_Boost)
			continue;

		LuaPlotClass::AddBuff(role->GUID(), GoodsRef.Boost.MagicID, GoodsRef.Boost.MagicLv, GoodsRef.Boost.EffectTime);
	}
	//----------------------------------------------------------

	SC_RequestBonusResult(role->GUID(), id, EM_LBRequestResult_OK);
	SD_RequestBonusComplete(role->DBID(), role->Account_ID(), id, EM_LBRequestResult_OK);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_LoginChild::RD_BonusPeriod(BaseItemObject* obj, int maxpage, int minpage)
{
	RoleDataEx* role = obj->Role();

	if (role == NULL)
		return;

	SC_BonusPeriod(role->GUID(), maxpage, minpage);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_LoginChild::CliConnect(int playerdbid, const char* account)
{
	SD_CliConnect(playerdbid, account);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_LoginChild::CliDisconnect(int playerdbid, const char* account)
{
	SD_CliDisconnect(playerdbid, account);
}