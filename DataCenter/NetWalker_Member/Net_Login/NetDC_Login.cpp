#include "NetDC_Login.h"

//*******************************************************************
CNetDC_Login* CNetDC_Login::s_This = NULL;

//*******************************************************************
//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool CNetDC_Login::_Init()
{
	//_Net->RegOnSrvPacketFunction(EM_PG_Login_LtoD_RequestBonusList		, _PG_Login_LtoD_RequestBonusList		);
	//_Net->RegOnSrvPacketFunction(EM_PG_Login_LtoD_RequestBonus			, _PG_Login_LtoD_RequestBonus			);
	//_Net->RegOnSrvPacketFunction(EM_PG_Login_LtoD_RequestBonusComplete	, _PG_Login_LtoD_RequestBonusComplete	);
	//_Net->RegOnSrvPacketFunction(EM_PG_Login_LtoD_CliConnect			, _PG_Login_LtoD_CliConnect				);
	//_Net->RegOnSrvPacketFunction(EM_PG_Login_LtoD_CliDisconnect			, _PG_Login_LtoD_CliDisconnect			);

	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool CNetDC_Login::_Release()
{
	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_Login::_PG_Login_LtoD_RequestBonusList(int srvid, int size, void* data)
{
	PG_Login_LtoD_RequestBonusList* PG = (PG_Login_LtoD_RequestBonusList*)data;
	s_This->RL_RequestBonusList(srvid, PG->PlayerDBID, PG->Account.Begin(), PG->Page);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_Login::_PG_Login_LtoD_RequestBonus(int srvid, int size, void* data)
{
	PG_Login_LtoD_RequestBonus* PG = (PG_Login_LtoD_RequestBonus*)data;
	s_This->RL_RequestBonus(srvid, PG->PlayerDBID, PG->Account.Begin(), PG->ID);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_Login::_PG_Login_LtoD_RequestBonusComplete(int srvid, int size, void* data)
{
	PG_Login_LtoD_RequestBonusComplete* PG = (PG_Login_LtoD_RequestBonusComplete*)data;
	s_This->RL_RequestBonusComplete(srvid, PG->PlayerDBID, PG->Account.Begin(), PG->ID, PG->Result);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_Login::_PG_Login_LtoD_CliConnect(int srvid, int size, void* data)
{
	PG_Login_LtoD_CliConnect* PG = (PG_Login_LtoD_CliConnect*)data;
	s_This->RL_CliConnect(srvid, PG->PlayerDBID, PG->Account.Begin());
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_Login::_PG_Login_LtoD_CliDisconnect(int srvid, int size, void* data)
{
	PG_Login_LtoD_CliDisconnect* PG = (PG_Login_LtoD_CliDisconnect*)data;
	s_This->RL_CliDisconnect(srvid, PG->PlayerDBID, PG->Account.Begin());
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_Login::SL_BonusList(int srvid, int playerdbid, std::vector<LBBox>* boxes, int year, int month, int mday, int page, const wchar_t* description)
{
	unsigned long dwSize = sizeof(PG_Login_DtoL_BonusList);
	dwSize += ((wcslen(description) + 1) * sizeof(wchar_t));

	if (boxes != NULL)
	{
		for (unsigned i = 0; i < boxes->size(); ++i)
		{
			LBBox& Box = (*boxes)[i];
			dwSize += sizeof(PG_LBBox);
			dwSize += (sizeof(LBGoods) * Box.Goods.size());
			dwSize += (sizeof(LBConstraint) * Box.Constraints.size());			
		}
	}

	dwSize = ((dwSize / 4) + 1) * 4;

	void* pPacket = NEW BYTE[dwSize];
	memset(pPacket, 0, dwSize);

	PBYTE pTraveler = (PBYTE)pPacket;

	PG_Login_DtoL_BonusList* pHeader = (PG_Login_DtoL_BonusList*)pPacket;
	pHeader->Command		 = EM_PG_Login_DtoL_BonusList;
	pHeader->PlayerDBID		 = playerdbid;
	pHeader->Year			 = year;
	pHeader->Month			 = month;
	pHeader->MDay			 = mday;
	pHeader->Page			 = page;
	pHeader->DescriptionSize = ((wcslen(description) + 1) * sizeof(wchar_t));

	pTraveler += sizeof(PG_Login_DtoL_BonusList);

	wchar_t* pDescription = (wchar_t*)pTraveler;
	wcscpy(pDescription, description);
	pTraveler += pHeader->DescriptionSize;

	if (boxes != NULL)
	{
		pHeader->BoxCount = boxes->size();

		for (unsigned i = 0; i < boxes->size(); ++i)
		{
			LBBox& Box = (*boxes)[i];

			PG_LBBox* pBox = (PG_LBBox*)pTraveler;
			pBox->ID			  = Box.ID;
			pBox->ToWho			  = Box.ToWho;
			pBox->StartDate		  = Box.StartDate;
			pBox->EndDate		  = Box.EndDate;
			pBox->GoodsCount	  = Box.Goods.size();
			pBox->ConstraintCount = Box.Constraints.size();
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

	Global::SendToSrvBySockID(srvid, dwSize, pPacket);

	delete pPacket;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_Login::SL_RequestBonusResult(int srvid, int playerdbid, const char* account, int id, int money, std::vector<LBGoods>* goods, LBRequestResultENUM result)
{
	unsigned long dwSize = sizeof(PG_Login_DtoL_RequestBonusResult);

	if (goods != NULL)
	{
		dwSize += (sizeof(LBGoods) * goods->size());
	}

	dwSize = ((dwSize / 4) + 1) * 4;

	void* pPacket = NEW BYTE[dwSize];
	memset(pPacket, 0, dwSize);

	PBYTE pTraveler = (PBYTE)pPacket;

	PG_Login_DtoL_RequestBonusResult* pHeader = (PG_Login_DtoL_RequestBonusResult*)pPacket;
	pHeader->Command	= EM_PG_Login_DtoL_RequestBonusResult;
	pHeader->Account	= account;
	pHeader->PlayerDBID = playerdbid;
	pHeader->ID			= id;
	pHeader->Money		= money;
	pHeader->Result		= result;

	pTraveler += sizeof(PG_Login_DtoL_RequestBonusResult);

	if (goods != NULL)
	{
		pHeader->Count = goods->size();

		for (unsigned i = 0; i < goods->size(); ++i)
		{
			LBGoods* pGoods = (LBGoods*)pTraveler;
			*pGoods = (*goods)[i];
			pTraveler += sizeof(LBGoods);
		}
	}

	Global::SendToSrvBySockID(srvid, dwSize, pPacket);

	delete pPacket;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_Login::SL_BonusPeriod(int srvid, int playerdbid, int maxpage, int minpage)
{
	PG_Login_DtoL_BonusPeriod Send;
	Send.PlayerDBID = playerdbid;
	Send.MaxPage = maxpage;
	Send.MinPage = minpage;

	Global::SendToSrvBySockID(srvid, sizeof(Send), &Send);
}