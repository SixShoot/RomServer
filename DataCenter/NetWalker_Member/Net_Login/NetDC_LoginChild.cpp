#include "NetDC_LoginChild.h"
//*******************************************************************
struct OnTimeLoadUserData
{
	std::vector<LBBox>			Boxes;
	std::map<int, std::wstring> Descriptions;

	OnTimeLoadUserData()
	{

	}

	void Init()
	{
		Boxes.clear();
		Descriptions.clear();
	}
};

struct InsertStampUserData
{
	int			PlayerDBID;
	std::string	Account;

	InsertStampUserData()
	{
		PlayerDBID = 0;
		Account = "";
	}
};

struct InsertReceiptUserData
{
	int			PlayerDBID;
	std::string	Account;
	int			ID;

	InsertReceiptUserData()
	{
		PlayerDBID = 0;
		Account = "";
		ID = 0;
	}
};

struct UpdateStatusUserData
{
	int	ID;
	int Status;

	UpdateStatusUserData()
	{
		ID = 0;
		Status = 0;
	}
};

//*******************************************************************
//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool CNetDC_LoginChild::Init()
{
	CNetDC_Login::_Init();

	if(s_This != NULL)
		return false;

	s_This = NEW CNetDC_LoginChild();

	//Global::Schedular()->Push(OnTimeInit, 10000, NULL, NULL);

	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool CNetDC_LoginChild::Release()
{
	if(s_This == NULL)
		return false;

	delete s_This;
	s_This = NULL;

	CNetDC_Login::_Release();

	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
CNetDC_LoginChild* CNetDC_LoginChild::This()
{
	return (CNetDC_LoginChild*)s_This;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
CNetDC_LoginChild::CNetDC_LoginChild()
{
	m_LastIniTick = 0;
	m_LastYDay = 0;

	m_OnTimeLoadTemp = (void*)(NEW OnTimeLoadUserData());
	m_OnTimeLoadState = EM_ON_TIME_LOAD_STATE_WAITING;
	m_LastLoadTick = 0;

	ReadIni();
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
CNetDC_LoginChild::~CNetDC_LoginChild()
{
	if (m_OnTimeLoadTemp != NULL)
	{
		delete m_OnTimeLoadTemp;
		m_OnTimeLoadTemp = NULL;
	}
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::ReadIni()
{
	m_Enable = (Global::Ini()->IntDef("EnableLoginBouns", 1) == 1);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
int CNetDC_LoginChild::OnTimeInit(SchedularInfo* obj, void* inputclass)
{
	_RD_NormalDB_Import->SqlCmd(0, _SQLInit, _SQLInitResult, NULL, NULL);
	return 0;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
int CNetDC_LoginChild::OnTimeProc(SchedularInfo* obj, void* inputclass)
{
	unsigned long NowTick = GetTickCount();

	if ((NowTick - This()->m_LastIniTick) >= 60000)
	{
		This()->ReadIni();
		This()->m_LastIniTick = NowTick;
	}

	if (This()->m_Enable == true)
	{
		//-------------------------------------------------------
		time_t Now;
		time(&Now);
		struct tm *gmt;
		gmt = localtime(&Now);

		int& LastYDay = This()->m_LastYDay;
		std::map<std::string, LBOnlineTime>& AccountOnlineTime = This()->m_AccountOnlineTime;
		std::map<int, LBOnlineTime>& RoleOnlineTime = This()->m_RoleOnlineTime;

		if (LastYDay == 0)
		{
			LastYDay = gmt->tm_yday;
		}

		if (gmt->tm_yday != LastYDay)
		{
			for (std::map<std::string, LBOnlineTime>::iterator itAcc = AccountOnlineTime.begin(); itAcc != AccountOnlineTime.end(); itAcc++)
			{
				itAcc->second.Time = 0;
			}

			for (std::map<int, LBOnlineTime>::iterator itRole = RoleOnlineTime.begin(); itRole != RoleOnlineTime.end(); itRole++)
			{
				itRole->second.Time = 0;
			}
		}
		else
		{
			for (std::map<std::string, LBOnlineTime>::iterator itAcc = AccountOnlineTime.begin(); itAcc != AccountOnlineTime.end(); itAcc++)
			{
				if (itAcc->second.Enable == true)
				{
					itAcc->second.Time++;
				}
			}

			for (std::map<int, LBOnlineTime>::iterator itRole = RoleOnlineTime.begin(); itRole != RoleOnlineTime.end(); itRole++)
			{
				if (itRole->second.Enable == true)
				{
					itRole->second.Time++;
				}
			}
		}

		LastYDay = gmt->tm_yday;
		//-------------------------------------------------------

		//-------------------------------------------------------
		This()->m_OnTimeLoadLock.Enter();

		int& OnTimeLoadState = This()->m_OnTimeLoadState;

		switch(OnTimeLoadState)
		{
		case EM_ON_TIME_LOAD_STATE_WAITING:
			{				
				unsigned long& LastLoadTick = This()->m_LastLoadTick;

				if ((NowTick - LastLoadTick) >= 180000)
				{
					OnTimeLoadState = EM_ON_TIME_LOAD_STATE_LOADING;
					LastLoadTick = NowTick;
					This()->DB_OnTimeLoad();
				}
			}
			break;
		case EM_ON_TIME_LOAD_STATE_LOADING:
			break;
		case EM_ON_TIME_LOAD_STATE_FINISH:
			{
				OnTimeLoadUserData* pUserData = (OnTimeLoadUserData*)This()->m_OnTimeLoadTemp;

				std::map<int, LBProject>& Projects = This()->m_Projects;

				std::map<int, bool> UpdateMap;

				for (std::map<int, LBProject>::iterator it = Projects.begin(); it != Projects.end(); it++)
				{
					UpdateMap[it->second.Box.ID] = false;
				}

				for (unsigned i = 0 ; i < pUserData->Boxes.size(); ++i)
				{
					LBBox& Box = pUserData->Boxes[i];
					LBProject& Project = Projects[Box.ID];
					Project.Status = EM_LBStatus_Normal;
					Project.Box.Init();
					Project.Box = Box;
					UpdateMap[Box.ID] = true;
				}

				for (std::map<int, bool>::iterator it = UpdateMap.begin(); it != UpdateMap.end(); it++)
				{
					if (it->second == true)
						continue;

					Projects.erase(it->first);
				}

				This()->m_Descriptions.clear();
				This()->m_Descriptions = pUserData->Descriptions;

				pUserData->Init();

				OnTimeLoadState = EM_ON_TIME_LOAD_STATE_WAITING;
			}
			break;
		}

		This()->m_OnTimeLoadLock.Leave();
		//-------------------------------------------------------
	}

	Global::Schedular()->Push(OnTimeProc, 1000, NULL, NULL);

	return 0;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::DB_OnTimeLoad()
{
	This()->_RD_NormalDB_Import->SqlCmd(0, _SQLOnTimeLoad, _SQLOnTimeLoadResult, m_OnTimeLoadTemp, NULL);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::DB_InsertStamp(int playerdbid, const char* account)
{
	InsertStampUserData* pUserData = NEW InsertStampUserData();
	pUserData->PlayerDBID = playerdbid;
	pUserData->Account = account;

	This()->_RD_NormalDB_Import->SqlCmd(playerdbid, _SQLInsertStamp, _SQLInsertStampResult, pUserData, NULL);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::DB_InsertReceipt(int playerdbid, const char* account, int id)
{
	InsertReceiptUserData* pUserData = NEW InsertReceiptUserData();
	pUserData->PlayerDBID = playerdbid;
	pUserData->Account = account;
	pUserData->ID = id;

	This()->_RD_NormalDB_Import->SqlCmd(playerdbid, _SQLInsertReceipt, _SQLInsertReceiptResult, pUserData, NULL);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::DB_UpdateStatus(int id, int status)
{
	UpdateStatusUserData* pUserData = NEW UpdateStatusUserData();
	pUserData->ID	  = id;
	pUserData->Status = status;

	This()->_RD_NormalDB_Import->SqlCmd(0, _SQLUpdateStatus, _SQLUpdateStatusResult, pUserData, NULL);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::_SQLLoadLoginBonus(SqlBaseClass* sqlbase, std::vector<LBBox>& result)
{
	char Buff[2048] = {0};
	int _StartDate = 0;
	int _EndDate = 0;
	int Today = (int)RoleDataEx::G_Now_Float;

	MyDbSqlExecClass MyDBProc(sqlbase);

	MyDBProc.SqlCmd("SELECT [GUID], [ToWho], CAST([StartDate] AS float), CAST([EndDate] AS float), [Money] FROM [LoginBonusBase] WHERE [Status] = 0");

	int   GUID		= 0;
	int	  ToWho		= 0;
	float StartDate = 0;
	float EndDate	= 0;
	int   Money		= 0;
	MyDBProc.Bind(1, SQL_C_LONG	, SQL_DEFAULT, (LPBYTE)&GUID		);
	MyDBProc.Bind(2, SQL_C_LONG	, SQL_DEFAULT, (LPBYTE)&ToWho		);
	MyDBProc.Bind(3, SQL_C_FLOAT, SQL_DEFAULT, (LPBYTE)&StartDate	);
	MyDBProc.Bind(4, SQL_C_FLOAT, SQL_DEFAULT, (LPBYTE)&EndDate		);
	MyDBProc.Bind(5, SQL_C_LONG , SQL_DEFAULT, (LPBYTE)&Money		);

	std::vector<LBBox> Boxes;
	LBBox Box;

	while(MyDBProc.Read())
	{
		_StartDate = (int)StartDate;
		_EndDate = (int)EndDate;

		if (((_StartDate <= Today) || (_StartDate == 0)) && ((_EndDate >= Today) || (_EndDate == 0)))
		{
			Box.Init();
			Box.ID		  = GUID;
			Box.ToWho	  = (LBWhoENUM)ToWho;
			Box.StartDate = _StartDate;
			Box.EndDate	  = EndDate;
			Box.Money	  = Money;
			Boxes.push_back(Box);
		}
	}

	MyDBProc.Close();

	LBGoods Goods;
	bool BoxOK = true;
	GameObjDbStructEx* pObjDB = NULL;

	for (unsigned i = 0; i < Boxes.size(); ++i)
	{
		LBBox& Box = Boxes[i];

		BoxOK = true;

		//-----------------------------------------
		sprintf(Buff, "SELECT [OrgObjID], [Count] FROM [LoginBonusItem] WHERE [BaseGUID] = %d", Box.ID);
		MyDBProc.SqlCmd(Buff);

		int OrgObjID = 0;
		int Count	 = 0;

		MyDBProc.Bind(1, SQL_C_LONG, SQL_DEFAULT, (LPBYTE)&OrgObjID	);
		MyDBProc.Bind(2, SQL_C_LONG, SQL_DEFAULT, (LPBYTE)&Count	);

		while(MyDBProc.Read())
		{
			BoxOK = false;

			if ((OrgObjID > 0) && (Count > 0))
			{
				pObjDB = g_ObjectData->GetObj(OrgObjID);

				if (pObjDB != NULL)
				{
					if (pObjDB->IsItem() == true)
					{
						Goods.Init();
						Goods.Type			= EM_LBGoodsType_Item;
						Goods.Item.OrgObjID = OrgObjID;
						Goods.Item.Count	= Count;
						Box.Goods.push_back(Goods);
						BoxOK = true;
					}
				}
			}

			if (BoxOK == false)
				break;
		}

		MyDBProc.Close();
		//-----------------------------------------

		//-----------------------------------------
		if (BoxOK == true)
		{
			sprintf(Buff, "SELECT [MagicID], [MagicLv], [EffectTime] FROM [LoginBonusBoost] WHERE [BaseGUID] = %d", Box.ID);
			MyDBProc.SqlCmd(Buff);

			int MagicID    = 0;
			int MagicLv	   = 0;
			int EffectTime = 0;

			MyDBProc.Bind(1, SQL_C_LONG, SQL_DEFAULT, (LPBYTE)&MagicID		);
			MyDBProc.Bind(2, SQL_C_LONG, SQL_DEFAULT, (LPBYTE)&MagicLv		);
			MyDBProc.Bind(3, SQL_C_LONG, SQL_DEFAULT, (LPBYTE)&EffectTime	);

			while(MyDBProc.Read())
			{
				BoxOK = false;

				if ((MagicID > 0) && (MagicLv > 0) && (EffectTime > 0))
				{
					pObjDB = g_ObjectData->GetObj(MagicID);

					if (pObjDB != NULL)
					{
						if((pObjDB->IsMagicBase() == true) && (pObjDB->MagicBase.MagicFunc == EM_MAGICFUN_Assist))
						{
							Goods.Init();
							Goods.Type			   = EM_LBGoodsType_Boost;
							Goods.Boost.MagicID	   = MagicID;
							Goods.Boost.MagicLv	   = MagicLv;
							Goods.Boost.EffectTime = EffectTime;
							Box.Goods.push_back(Goods);
							BoxOK = true;
						}
					}
				}

				if (BoxOK == false)
					break;
			}

			MyDBProc.Close();
		}
		//-----------------------------------------

		//-----------------------------------------
		if (BoxOK == true)
		{
			//-----------------------------------------
			sprintf(Buff, "SELECT [Type], [Value1], [Value2], [Value3], [Value4] FROM [LoginBonusConstraint] WHERE [BaseGUID] = %d", Box.ID);
			MyDBProc.SqlCmd(Buff);

			LBConstraint Constraint;
			int Type   = 0;
			int Value1 = 0;
			int Value2 = 0;
			int Value3 = 0;
			int Value4 = 0;

			MyDBProc.Bind(1, SQL_C_LONG, SQL_DEFAULT, (LPBYTE)&Type		);
			MyDBProc.Bind(2, SQL_C_LONG, SQL_DEFAULT, (LPBYTE)&Value1	);
			MyDBProc.Bind(3, SQL_C_LONG, SQL_DEFAULT, (LPBYTE)&Value2	);
			MyDBProc.Bind(4, SQL_C_LONG, SQL_DEFAULT, (LPBYTE)&Value3	);
			MyDBProc.Bind(5, SQL_C_LONG, SQL_DEFAULT, (LPBYTE)&Value4	);

			while(MyDBProc.Read())
			{
				Constraint.Init();
				Constraint.Type = (LBConstraintTypeENUM)Type;
				Constraint.Value[0] = Value1;
				Constraint.Value[1] = Value2;
				Constraint.Value[2] = Value3;
				Constraint.Value[3] = Value4;

				Box.Constraints.push_back(Constraint);
			}

			MyDBProc.Close();
			//-----------------------------------------

			result.push_back(Box);
		}
		else
		{
			sprintf(Buff, "UPDATE [LoginBonusBase] SET [Status] = %d WHERE [GUID] = %d", EM_LBStatus_DataError, Box.ID);
			MyDBProc.SqlCmd_WriteOneLine(Buff);
		}
		//-----------------------------------------
	}
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::_SQLLoadDescriptions(SqlBaseClass* sqlbase, std::map<int, std::wstring>& result)
{
	char Buff[512] = {0};

	MyDbSqlExecClass MyDBProc(sqlbase);
	
	MyDBProc.SqlCmd("SELECT CAST([Date] AS float), [Description] FROM [LoginBonusDescription] WHERE ABS(DATEDIFF(day, [Date], GETDATE())) <= 90");

	wchar_t Description[2048] = {0};
	float Date = 0;
	MyDBProc.Bind(1, SQL_C_FLOAT, SQL_DEFAULT		 , (LPBYTE)&Date			);
	MyDBProc.Bind(2, SQL_C_WCHAR, sizeof(Description), (LPBYTE)&Description[0]	);

	while(MyDBProc.Read())
	{
		result[(int)Date] = Description;
	}

	MyDBProc.Close();
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool CNetDC_LoginChild::_SQLInit(std::vector<int>& vec, SqlBaseClass* sqlbase, void* userobj, ArgTransferStruct& arg)
{
	char Buff[512] = {0};

	MyDbSqlExecClass MyDBProc(sqlbase);

	char Account[_DEF_MAX_ACCOUNTNAME_SIZE_] = {0};
	int PlayerDBID = 0;
	float Date = 0;

	//-----------------------------------------
	std::vector<LBBox> Boxes;
	_SQLLoadLoginBonus(sqlbase, Boxes);

	for (unsigned i = 0; i < Boxes.size(); ++i)
	{
		LBBox& Box = Boxes[i];
		LBProject& Project = This()->m_Projects[Box.ID];
		Project.Box = Box;

		sprintf(Buff, "SELECT [Account], [PlayerDBID], CAST([ProcessTime] AS float) FROM [LoginBonusReceipt] WHERE [BaseGUID] = %d AND [WorldID] = %d AND DATEDIFF(day, [ProcessTime], GETDATE()) <= 90", Box.ID, Global::GetWorldID());
		MyDBProc.SqlCmd(Buff);

		MyDBProc.Bind(1, SQL_C_CHAR	, sizeof(Account), (LPBYTE)&Account[0]	);
		MyDBProc.Bind(2, SQL_C_LONG	, SQL_DEFAULT	 , (LPBYTE)&PlayerDBID	);
		MyDBProc.Bind(3, SQL_C_FLOAT, SQL_DEFAULT	 , (LPBYTE)&Date		);

		while(MyDBProc.Read())
		{
			Project.Receipts[Account][(int)Date].insert(PlayerDBID);
		}

		MyDBProc.Close();
	}	
	//-----------------------------------------

	//-----------------------------------------
	sprintf(Buff, "SELECT [Account], [PlayerDBID], CAST([Date] AS float) FROM [LoginBonusStamp] WHERE [WorldID] = %d AND DATEDIFF(day, [Date], GETDATE()) <= 30", Global::GetWorldID());
	MyDBProc.SqlCmd(Buff);

	MyDBProc.Bind(1, SQL_C_CHAR	, sizeof(Account), (LPBYTE)&Account[0]	);
	MyDBProc.Bind(2, SQL_C_LONG	, SQL_DEFAULT	 , (LPBYTE)&PlayerDBID	);
	MyDBProc.Bind(3, SQL_C_FLOAT, SQL_DEFAULT	 , (LPBYTE)&Date		);

	while(MyDBProc.Read())
	{
		This()->m_Stamps[Account][(int)Date].insert(PlayerDBID);
	}

	MyDBProc.Close();
	//-----------------------------------------

	//-----------------------------------------	
	_SQLLoadDescriptions(sqlbase, This()->m_Descriptions);
	//-----------------------------------------

	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::_SQLInitResult(std::vector<int>& vec, void* userobj, ArgTransferStruct& arg, bool resultok)
{
	unsigned long NowTick = GetTickCount();
	This()->m_LastIniTick = NowTick;
	This()->m_LastLoadTick = NowTick;
	Global::Schedular()->Push(OnTimeProc, 1000, NULL, NULL);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool CNetDC_LoginChild::_SQLOnTimeLoad(std::vector<int>& vec, SqlBaseClass* sqlbase, void* userobj, ArgTransferStruct& arg)
{
	OnTimeLoadUserData* pUserData = (OnTimeLoadUserData*)userobj;

	_SQLLoadLoginBonus(sqlbase, pUserData->Boxes);
	_SQLLoadDescriptions(sqlbase, pUserData->Descriptions);

	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::_SQLOnTimeLoadResult(std::vector<int>& vec, void* userobj, ArgTransferStruct& arg, bool resultok)
{
	This()->m_OnTimeLoadLock.Enter();
	This()->m_OnTimeLoadState = EM_ON_TIME_LOAD_STATE_FINISH;
	This()->m_OnTimeLoadLock.Leave();
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool CNetDC_LoginChild::_SQLInsertStamp(std::vector<int>& vec, SqlBaseClass* sqlbase, void* userobj, ArgTransferStruct& arg)
{
	InsertStampUserData* pUserData = (InsertStampUserData*)userobj;

	char Buff[512] = {0};

	MyDbSqlExecClass MyDBProc(sqlbase);

	std::string AccountX = StringToSqlX(pUserData->Account.c_str(), _MAX_ACCOUNT_SIZE_);

	sprintf(Buff, "INSERT INTO [LoginBonusStamp] ([Account], [PlayerDBID], [Date], [WorldID]) VALUES(%s, %d, GETDATE(), %d)", AccountX.c_str(), pUserData->PlayerDBID, Global::GetWorldID());
	MyDBProc.SqlCmd_WriteOneLine(Buff);

	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::_SQLInsertStampResult(std::vector<int>& vec, void* userobj, ArgTransferStruct& arg, bool resultok)
{
	delete userobj;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool CNetDC_LoginChild::_SQLInsertReceipt(std::vector<int>& vec, SqlBaseClass* sqlbase, void* userobj, ArgTransferStruct& arg)
{
	InsertReceiptUserData* pUserData = (InsertReceiptUserData*)userobj;

	char Buff[512] = {0};

	MyDbSqlExecClass MyDBProc(sqlbase);

	std::string AccountX = StringToSqlX(pUserData->Account.c_str(), _MAX_ACCOUNT_SIZE_);

	sprintf(Buff, "INSERT INTO [LoginBonusReceipt] ([BaseGUID], [Account], [PlayerDBID], [WorldID]) VALUES (%d, %s, %d, %d)", pUserData->ID, AccountX.c_str(), pUserData->PlayerDBID, Global::GetWorldID());
	MyDBProc.SqlCmd_WriteOneLine(Buff);

	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::_SQLInsertReceiptResult(std::vector<int>& vec, void* userobj, ArgTransferStruct& arg, bool resultok)
{
	delete userobj;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool CNetDC_LoginChild::_SQLUpdateStatus(std::vector<int>& vec, SqlBaseClass* sqlbase, void* userobj, ArgTransferStruct& arg)
{
	UpdateStatusUserData* pUserData = (UpdateStatusUserData*)userobj;

	char Buff[256] = {0};

	MyDbSqlExecClass MyDBProc(sqlbase);

	sprintf(Buff, "UPDATE [LoginBonusBase] SET [Status] = %d WHERE [GUID] = %d", pUserData->Status, pUserData->ID);
	MyDBProc.SqlCmd_WriteOneLine(Buff);

	return true;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::_SQLUpdateStatusResult(std::vector<int>& vec, void* userobj, ArgTransferStruct& arg, bool resultok)
{
	delete userobj;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::RL_RequestBonusList(int srvid, int playerdbid, const char* account, int page)
{
	if (m_Enable == false)
		return;

	Print(LV2, "LoginBonus", "RequestBonusList, playerdbid=%d, page=%d", playerdbid, page);

	int Today = (int)RoleDataEx::G_Now_Float;
	int Day = (Today + page);

	std::vector<LBBox> Boxes;
	LBBox Box;

	for (std::map<int, LBProject>::iterator it = m_Projects.begin(); it != m_Projects.end(); it++)
	{
		LBProject& Project = it->second;
		Box.Init();
		Box = Project.Box;

		//-----------------------------------------
		if (Box.Status == EM_LBBoxStatus_Avaliable)
		{
			std::set<int>& Receipts = Project.Receipts[account][Day];

			switch(Box.ToWho)
			{
			case EM_LB_Who_Role:
				{
					if (Receipts.find(playerdbid) != Receipts.end())
					{
						Box.Status = EM_LBBoxStatus_Requested;
					}
				}
				break;
			case EM_LB_Who_Account:
				{
					if (!Receipts.empty())
					{
						Box.Status = EM_LBBoxStatus_Requested;
					}
				}
				break;
			}
		}
		//-----------------------------------------

		//-----------------------------------------
		if (Box.Status == EM_LBBoxStatus_Avaliable)
		{
			if (page != 0)
			{
				Box.Status = EM_LBBoxStatus_Unavaliable;
			}
		}
		//-----------------------------------------

		//-----------------------------------------
		if (Box.Status == EM_LBBoxStatus_Avaliable)
		{
			for (unsigned i = 0; i < Box.Constraints.size(); ++i)
			{
				if (Box.Status != EM_LBBoxStatus_Avaliable)
					break;

				LBConstraint& LBC = Box.Constraints[i];

				switch(LBC.Type)
				{
				case EM_LBConstraintType_OnlineTime:
					{
						int* pTime = NULL;
						LBC.Remaining = 0;

						switch(LBC.OnlineTime.Who)
						{
						case EM_LB_Who_Role:
							pTime = &(m_RoleOnlineTime[playerdbid].Time);
							break;
						case EM_LB_Who_Account:
							pTime = &(m_AccountOnlineTime[account].Time);
							break;
						}

						if (pTime != NULL)
						{
							LBC.Remaining = LBC.OnlineTime.Time - (*pTime);

							if ((*pTime) < LBC.OnlineTime.Time)
							{
								Box.Status = EM_LBBoxStatus_Unavaliable;
							}
						}
					}
					break;
				case EM_LBConstraintType_LoginDays:
					{
						LBC.Remaining = LBC.LoginDays.Day;

						switch(LBC.LoginDays.Who)
						{
						case EM_LB_Who_Role:
						case EM_LB_Who_Account:
							{
								for (int i = 0; i < LBC.LoginDays.Day; ++i)
								{
									std::set<int>& Stamps = m_Stamps[account][Day-i];

									if (((LBC.LoginDays.Who == EM_LB_Who_Role) && (Stamps.find(playerdbid) == Stamps.end())) ||
										((LBC.LoginDays.Who == EM_LB_Who_Account) && Stamps.empty()))
									{
										Box.Status = EM_LBBoxStatus_Unavaliable;
										break;
									}

									LBC.Remaining--;
								}
							}
							break;
						}
					}
					break;
				}
			}
		}
		//-----------------------------------------

		Boxes.push_back(Box);
	}

	//-----------------------------------------
	time_t NowTime;
	time(&NowTime);
	struct tm* gmt;
	NowTime += (page*86400);
	gmt = localtime(&NowTime);

	SL_BonusList(srvid, playerdbid, &Boxes, (gmt->tm_year + 1900), (gmt->tm_mon + 1), gmt->tm_mday, page, m_Descriptions[Day].c_str());
	//-----------------------------------------
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::RL_RequestBonus(int srvid, int playerdbid, const char* account, int id)
{
	if (m_Enable == false)
		return;

	Print(LV2, "LoginBonus", "RequestBonus, playerdbid=%d, bonusid=%d", playerdbid, id);

	int Today = (int)RoleDataEx::G_Now_Float;

	LBRequestResultENUM Result = EM_LBRequestResult_OK;
	int Money = 0;
	std::vector<LBGoods>* pGoods = NULL;

	std::map<int, LBProject>::iterator itProject = m_Projects.find(id);

	if (itProject != m_Projects.end())
	{
		LBProject& Project = itProject->second;
		std::set<int>& Receipts = Project.Receipts[account][Today];
		LBBox& Box = Project.Box;

		switch(Project.Status)
		{
		case EM_LBStatus_Normal:
			{
				if ((((Box.StartDate <= Today) || (Box.StartDate == 0)) && (((int)Box.EndDate >= Today) || ((int)Box.EndDate == 0))) == false)
				{
					Result = EM_LBRequestResult_Expired;
				}

				if (Result == EM_LBRequestResult_OK)
				{
					switch(Box.ToWho)
					{
					case EM_LB_Who_Role:
						{
							if (Receipts.find(playerdbid) != Receipts.end())
							{
								Result = EM_LBRequestResult_Requested;
							}
						}
						break;
					case EM_LB_Who_Account:
						{
							if (!Receipts.empty())
							{
								Result = EM_LBRequestResult_Requested;
							}
						}
						break;
					}
				}

				if (Result == EM_LBRequestResult_OK)
				{
					//-----------------------------------------
					for (unsigned i = 0; i < Box.Constraints.size(); ++i)
					{
						if (Result != EM_LBRequestResult_OK)
							break;

						LBConstraint& LBC = Box.Constraints[i];

						switch(LBC.Type)
						{
						case EM_LBConstraintType_OnlineTime:
							{
								int* pTime = NULL;
								LBC.Remaining = 0;

								switch(LBC.OnlineTime.Who)
								{
								case EM_LB_Who_Role:
									pTime = &(m_RoleOnlineTime[playerdbid].Time);
									break;
								case EM_LB_Who_Account:
									pTime = &(m_AccountOnlineTime[account].Time);
									break;
								}

								if (pTime != NULL)
								{
									LBC.Remaining = LBC.OnlineTime.Time - *pTime;

									if (*pTime < LBC.OnlineTime.Time)
									{
										Result = EM_LBRequestResult_Constrained;
									}
								}
							}
							break;
						case EM_LBConstraintType_LoginDays:
							{
								LBC.Remaining = LBC.LoginDays.Day;

								switch(LBC.LoginDays.Who)
								{
								case EM_LB_Who_Role:
								case EM_LB_Who_Account:
									{
										for (int i = 0; i < LBC.LoginDays.Day; ++i)
										{
											std::set<int>& Stamps = m_Stamps[account][Today-i];

											if (((LBC.LoginDays.Who == EM_LB_Who_Role) && (Stamps.find(playerdbid) == Stamps.end())) ||
												((LBC.LoginDays.Who == EM_LB_Who_Account) && Stamps.empty()))
											{
												Result = EM_LBRequestResult_Constrained;
												break;
											}

											LBC.Remaining--;
										}
									}
									break;
								}
							}
							break;
						}
					}
					//-----------------------------------------
				}

				if (Result == EM_LBRequestResult_OK)
				{
					Money = Box.Money;
					pGoods = &Box.Goods;

					//標記為已領取
					Receipts.insert(playerdbid);
				}
			}
			break;
		case EM_LBStatus_DataError:
			Result = EM_LBRequestResult_ItemDataError;
			break;
		}
	}
	else
	{
		Result = EM_LBRequestResult_NotExist;
	}

	SL_RequestBonusResult(srvid, playerdbid, account, id, Money, pGoods, Result);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::RL_RequestBonusComplete(int srvid, int playerdbid, const char* account, int id, LBRequestResultENUM result)
{
	if (m_Enable == false)
		return;

	switch(result)
	{
	case EM_LBRequestResult_OK:
		DB_InsertReceipt(playerdbid, account, id);
		break;
	case EM_LBRequestResult_BagFull:
		{
			std::map<int, LBProject>::iterator it = m_Projects.find(id);

			if (it != m_Projects.end())
			{
				int Today = (int)RoleDataEx::G_Now_Float;
				it->second.Receipts[account][Today].erase(playerdbid);
			}
		}
		break;
	default:
		{
			std::map<int, LBProject>::iterator it = m_Projects.find(id);

			if (it != m_Projects.end())
			{
				it->second.Status = EM_LBStatus_DataError;
			}

			DB_UpdateStatus(id, EM_LBStatus_DataError);
		}
	}

	Print(LV2, "LoginBonus", "RequestBonusComplete, playerdbid=%d, bonusid=%d, result=%d", playerdbid, id, result);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::RL_CliConnect(int srvid, int playerdbid, const char* account)
{
	if (m_Enable == false)
		return;

	m_AccountOnlineTime[account].Enable = true;
	m_RoleOnlineTime[playerdbid].Enable = true;

	int Today = (int)RoleDataEx::G_Now_Float;

	//------------------------------------------------------
	std::set<int>& Stamps = m_Stamps[account][Today];

	if (Stamps.find(playerdbid) == Stamps.end())
	{
		Stamps.insert(playerdbid);
		DB_InsertStamp(playerdbid, account);
	}
	//------------------------------------------------------

	//------------------------------------------------------
	int MaxPage	= INT_MIN;
	int MinPage	= INT_MAX;
	int StartPage = 0;
	int EndPage = 0;

	for (std::map<int, LBProject>::iterator itProject = m_Projects.begin(); itProject != m_Projects.end(); itProject++)
	{
		LBBox& Box = itProject->second.Box;

		if (Box.StartDate <= 0)
		{
			StartPage = INT_MIN;
		}
		else
		{
			StartPage = Box.StartDate - Today;
		}

		if (Box.EndDate <= 0)
		{
			EndPage = INT_MAX;
		}
		else
		{
			EndPage = ((int)Box.EndDate) - Today;
		}

		MinPage = __min(MinPage, StartPage);
		MaxPage = __max(MaxPage, EndPage);
	}

	SL_BonusPeriod(srvid, playerdbid, MaxPage, MinPage);
	//------------------------------------------------------
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void CNetDC_LoginChild::RL_CliDisconnect(int srvid, int playerdbid, const char* account)
{
	if (m_Enable == false)
		return;

	m_AccountOnlineTime[account].Enable = false;
	m_RoleOnlineTime[playerdbid].Enable = false;
}