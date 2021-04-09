#include "Net_Login.h"

//*******************************************************************
Net_Login* Net_Login::s_This = NULL;

//*******************************************************************

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool Net_Login::_Init()
{
	Srv_NetCliReg(PG_Login_CtoL_RequestBonusList);
	Srv_NetCliReg(PG_Login_CtoL_RequestBonus	);

	_Net->RegOnSrvPacketFunction(EM_PG_Login_DtoL_BonusList			, _PG_Login_DtoL_BonusList			);
	_Net->RegOnSrvPacketFunction(EM_PG_Login_DtoL_RequestBonusResult, _PG_Login_DtoL_RequestBonusResult	);
	_Net->RegOnSrvPacketFunction(EM_PG_Login_DtoL_BonusPeriod		, _PG_Login_DtoL_BonusPeriod		);

	_Net->RegOnCliConnectFunction(_OnCliConnect);
	_Net->RegOnCliDisconnectFunction(_OnCliDisconnect);

	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool Net_Login::_Release()
{
	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::_PG_Login_CtoL_RequestBonusList(int netid, int size, void* data)
{
	PG_Login_CtoL_RequestBonusList* pg = (PG_Login_CtoL_RequestBonusList*)data;

	BaseItemObject*	obj = BaseItemObject::GetObjBySockID(netid);
	if(obj == NULL)
		return;

	s_This->RC_RequestBonusList(obj, pg->Page);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::_PG_Login_CtoL_RequestBonus(int netid, int size, void* data)
{
	PG_Login_CtoL_RequestBonus* pg = (PG_Login_CtoL_RequestBonus*)data;

	BaseItemObject*	obj = BaseItemObject::GetObjBySockID(netid);
	if(obj == NULL)
		return;

	s_This->RC_RequestBonus(obj, pg->ID);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::_PG_Login_DtoL_BonusList(int netid, int size, void* data)
{
	PG_Login_DtoL_BonusList* pg = (PG_Login_DtoL_BonusList*)data;

	BaseItemObject* obj = BaseItemObject::GetObjByDBID(pg->PlayerDBID);
	if(obj == NULL)
		return;

	PBYTE pTraveler = (PBYTE)pg;
	pTraveler += sizeof(PG_Login_DtoL_BonusList);

	LBBox Box;
	std::vector<LBBox> Boxes;

	std::wstring Description = (wchar_t*)pTraveler;	
	pTraveler += pg->DescriptionSize;

	for (int i = 0; i < pg->BoxCount; ++i)
	{
		Box.Init();

		PG_LBBox* pBox = (PG_LBBox*)pTraveler;
		Box.ID		  = pBox->ID;
		Box.ToWho	  = pBox->ToWho;
		Box.StartDate = pBox->StartDate;
		Box.EndDate	  = pBox->EndDate;
		Box.Money	  = pBox->Money;
		Box.Status	  = pBox->Status;
		pTraveler += sizeof(PG_LBBox);

		for (int j = 0; j < pBox->GoodsCount; ++j)
		{
			LBGoods* pGoods = (LBGoods*)pTraveler;
			Box.Goods.push_back(*pGoods);
			pTraveler += sizeof(LBGoods);
		}

		for (int j = 0; j < pBox->ConstraintCount; ++j)
		{
			LBConstraint* pConstraint = (LBConstraint*)pTraveler;
			Box.Constraints.push_back(*pConstraint);
			pTraveler += sizeof(LBConstraint);
		}

		Boxes.push_back(Box);
	}

	s_This->RD_BonusList(obj, Boxes, pg->Year, pg->Month, pg->MDay, pg->Page, Description.c_str());
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::_PG_Login_DtoL_RequestBonusResult(int netid, int size, void* data)
{
	PG_Login_DtoL_RequestBonusResult* pg = (PG_Login_DtoL_RequestBonusResult*)data;

	PBYTE pTraveler = (PBYTE)pg;
	pTraveler += sizeof(PG_Login_DtoL_RequestBonusResult);

	std::vector<LBGoods> Goods;

	for (int i = 0; i < pg->Count; ++i)
	{
		LBGoods* pGoods = (LBGoods*)pTraveler;
		Goods.push_back(*pGoods);
		pTraveler += sizeof(LBGoods);
	}

	s_This->RD_RequestBonusResult(pg->PlayerDBID, pg->Account.SafeStr(), pg->ID, pg->Money, Goods, pg->Result);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::_PG_Login_DtoL_BonusPeriod(int netid, int size, void* data)
{
	PG_Login_DtoL_BonusPeriod* pg = (PG_Login_DtoL_BonusPeriod*)data;

	BaseItemObject* obj = BaseItemObject::GetObjByDBID(pg->PlayerDBID);
	if(obj == NULL)
		return;

	s_This->RD_BonusPeriod(obj, pg->MaxPage, pg->MinPage);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::_OnCliConnect(int cliid, std::string account)
{
	BaseItemObject* obj = Global::GetObjBySockID(cliid);

	if (obj == NULL)
		return;

	RoleDataEx* role = obj->Role();

	if (role == NULL)
		return;

	_LoginInfo& Info = s_This->m_LoginInfo[cliid];
	Info.PlayerDBID  = role->DBID();
	Info.Account	 = role->Account_ID();

	s_This->CliConnect(Info.PlayerDBID, Info.Account.c_str());
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::_OnCliDisconnect(int cliid)
{
	std::map<int, _LoginInfo>::iterator it = s_This->m_LoginInfo.find(cliid);

	if (it != s_This->m_LoginInfo.end())
	{
		s_This->CliDisconnect(it->second.PlayerDBID, it->second.Account.c_str());
	}

	s_This->m_LoginInfo.erase(cliid);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::SC_BonusList(int guid, std::vector<LBBox>* boxes, int year, int month, int mday, int page, const wchar_t* description)
{
	unsigned long dwSize = sizeof(PG_Login_LtoC_BonusList);
	dwSize += (((int)wcslen(description) + 1) * sizeof(wchar_t));

	if (boxes != NULL)
	{
		for (unsigned i = 0; i < boxes->size(); ++i)
		{
			LBBox& Box = (*boxes)[i];
			dwSize += sizeof(PG_LBBox);
			dwSize += (sizeof(LBGoods) * (int)Box.Goods.size());
			dwSize += (sizeof(LBConstraint) * (int)Box.Constraints.size());
		}
	}

	dwSize = ((dwSize / 4) + 1) * 4;

	void* pPacket = NEW BYTE[dwSize];
	memset(pPacket, 0, dwSize);

	PBYTE pTraveler = (PBYTE)pPacket;

	PG_Login_LtoC_BonusList* pHeader = (PG_Login_LtoC_BonusList*)pPacket;
	pHeader->Command = EM_PG_Login_LtoC_BonusList;
	pHeader->Year			 = year;
	pHeader->Month			 = month;
	pHeader->MDay			 = mday;
	pHeader->Page			 = page;
	pHeader->DescriptionSize = (int)((wcslen(description) + 1) * sizeof(wchar_t));

	pTraveler += sizeof(PG_Login_LtoC_BonusList);

	wchar_t* pTitle = (wchar_t*)pTraveler;
	wcscpy(pTitle, description);
	pTraveler += pHeader->DescriptionSize;

	if (boxes != NULL)
	{
		pHeader->BoxCount = (int)boxes->size();

		for (unsigned i = 0; i < boxes->size(); ++i)
		{
			LBBox& Box = (*boxes)[i];

			PG_LBBox* pBox = (PG_LBBox*)pTraveler;
			pBox->ID			  = Box.ID;
			pBox->ToWho			  = Box.ToWho;
			pBox->StartDate		  = Box.StartDate;
			pBox->EndDate		  = Box.EndDate;
			pBox->GoodsCount	  = (int)Box.Goods.size();
			pBox->ConstraintCount = (int)Box.Constraints.size();
			pBox->Money			  = Box.Money;
			pBox->Status		  = Box.Status;
			pTraveler += sizeof(PG_LBBox);

			for (unsigned j = 0; j < Box.Goods.size(); ++j)
			{
				LBGoods* pGoods = (LBGoods*)pTraveler;
				*pGoods = Box.Goods[j];
				pTraveler += sizeof(LBGoods);
			}

			for (unsigned j = 0; j < Box.Constraints.size(); ++j)
			{
				LBConstraint* pConstraint = (LBConstraint*)pTraveler;
				*pConstraint = Box.Constraints[j];
				pTraveler += sizeof(LBConstraint);
			}
		}
	}

	Global::SendToCli_ByGUID(guid, dwSize, pPacket);

	delete pPacket;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::SC_RequestBonusResult(int guid, int id, LBRequestResultENUM result)
{
	PG_Login_LtoC_RequestBonusResult Send;
	Send.ID		  = id;
	Send.Result	  = result;

	Global::SendToCli_ByGUID(guid, sizeof(Send), &Send);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::SC_BonusPeriod(int guid, int maxpage, int minpage)
{
	PG_Login_LtoC_BonusPeriod Send;
	Send.MaxPage = maxpage;
	Send.MinPage = minpage;

	Global::SendToCli_ByGUID(guid, sizeof(Send), &Send);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::SD_RequestBonusList(int playerdbid, const char* account, int page)
{
	PG_Login_LtoD_RequestBonusList Send;
	Send.Account	= account;
	Send.PlayerDBID	= playerdbid;
	Send.Page		= page;

	Global::SendToDBCenter(sizeof(Send), &Send);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::SD_RequestBonus(int playerdbid, const char* account, int id)
{
	PG_Login_LtoD_RequestBonus Send;
	Send.Account	= account;
	Send.PlayerDBID	= playerdbid;
	Send.ID		    = id;

	Global::SendToDBCenter(sizeof(Send), &Send);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::SD_RequestBonusComplete(int playerdbid, const char* account, int id, LBRequestResultENUM result)
{
	PG_Login_LtoD_RequestBonusComplete Send;
	Send.Account    = account;
	Send.PlayerDBID = playerdbid;
	Send.ID			= id;
	Send.Result		= result;

	Global::SendToDBCenter(sizeof(Send), &Send);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::SD_CliConnect(int playerdbid, const char* account)
{
	PG_Login_LtoD_CliConnect Send;
	Send.Account    = account;
	Send.PlayerDBID = playerdbid;

	Global::SendToDBCenter(sizeof(Send), &Send);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void Net_Login::SD_CliDisconnect(int playerdbid, const char* account)
{
	PG_Login_LtoD_CliDisconnect Send;
	Send.Account    = account;
	Send.PlayerDBID = playerdbid;

	Global::SendToDBCenter(sizeof(Send), &Send);
}