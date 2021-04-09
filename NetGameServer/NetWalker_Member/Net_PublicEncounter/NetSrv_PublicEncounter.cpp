#include "NetSrv_PublicEncounter.h"
#include "../../MainProc/LuaPlot/LuaPlot.h"
#include "RoleData/ObjectDataClass.h"

//-------------------------------------------------------------------------------
//define block
//-------------------------------------------------------------------------------
#define PE_INIT_FIRST_TIME_PROC_TIME_GAP			1000 * 60
#define PE_TIME_PROC_TIME_GAP						1000
#define PE_OBJ_TIMELIMIT_SYNC_TIME_GAP				1000 * 10
#define PE_PLAYER_DISCONNECT_TIME_GAP				1000 * 60 * 5
#define PE_PLAYER_BATTLE_STATICTIS_CLEAR_TIME_GAP	1000 * 60 * 2

//-------------------------------------------------------------------------------
//static member block
//-------------------------------------------------------------------------------
CNetSrv_PublicEncounter*		CNetSrv_PublicEncounter::m_pThis			= NULL;

void*							PublicEncounterManager::m_PacketBuffer		= NULL;
unsigned long					PublicEncounterManager::m_PacketBufferSize	= 0;
vector<CPublicEncounter*>		PublicEncounterManager::m_PEList;
map<int, CPublicEncounter*>		PublicEncounterManager::m_PEIndex;
vector<int>						PublicEncounterManager::m_ConnectedClient;
map<int, int>					PublicEncounterManager::m_SockID_DBIDIndex;
map<string, int>				PublicEncounterManager::m_Variables;
MutilThread_CritSect			PublicEncounterManager::m_CritSectVar;

//-------------------------------------------------------------------------------
//compare function
//-------------------------------------------------------------------------------
bool PE_Campare_Player_Score(CPublicEncounterPlayer* p1, CPublicEncounterPlayer* p2)
{
	bool Result = false;

	if (p1 && p2)
	{
		Result = (p1->Score < p2->Score);
	}

	return Result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CNetSrv_PublicEncounter
////////////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CNetSrv_PublicEncounter::_Init()
{
	_Net->RegOnCliConnectFunction( _OnCliConnect );
	_Net->RegOnCliDisconnectFunction( _OnCliDisconnect );

	Srv_NetCliReg( PG_PE_CtoL_RegionChanged	);

	PublicEncounterManager::Init();

	return true;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CNetSrv_PublicEncounter::_Release()
{
	PublicEncounterManager::Release();
	return true;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CNetSrv_PublicEncounter::_PG_PE_CtoL_RegionChanged( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_PE_CtoL_RegionChanged );
	M_CLIENT_OBJ( iCliID );
	m_pThis->OnRZ_CliRegionChanged(pObj, pPacket->iRegionID );
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CNetSrv_PublicEncounter::_OnCliConnect( int iCliID , string Account )
{
	M_CLIENT_OBJ( iCliID );

    m_pThis->CliConnect( pObj, iCliID);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CNetSrv_PublicEncounter::_OnCliDisconnect( int iCliID )
{
	 m_pThis->CliDisconnect( iCliID );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CPublicEncounterPlayer
////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterPlayer::CPublicEncounterPlayer(CPublicEncounter* pOwner, int iDBID)
{
	m_Owner					= pOwner;
	m_DBID					= iDBID;
	m_OldPartyID			= -1;
	m_Score					= 0;
	m_RemoveTime			= 0;
	m_StatisticsClearTime	= 0;
	m_ContributionPoint		= 0;
	m_TempHatePoint			= 0;
	m_Status				= PE_PLAYER_DEACTIVATED;

	SecureZeroMemory(&m_TempStatictisData, sizeof(m_TempStatictisData));
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterPlayer::~CPublicEncounterPlayer()
{

}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterPlayer::SetScore(float value)
{
	if (m_Score != value)
	{
		m_Score = value;

		PublicEncounterManager::SC_PlayerScore(DBID, m_Owner->Index, (int)m_Score);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterPlayer::SetStatus(int value)
{
	if (m_Status != value)
	{
		m_Status = value;

		switch(Status)
		{
		case PE_PLAYER_DEACTIVATED:
			{
				m_StatisticsClearTime = GetTickCount() + PE_PLAYER_BATTLE_STATICTIS_CLEAR_TIME_GAP;

				//store battle statics

				if (pRole)
				{
					m_TempStatictisData = pRole->PlayerTempData->BGInfo;
				}
			}
			break;
		case PE_PLAYER_ACTIVATED:
			{
				//restore temp battle statics
				if (pRole)
				{
					pRole->PlayerTempData->BGInfo = m_TempStatictisData;
				}
			
				m_StatisticsClearTime = 0;
				m_RemoveTime          = 0;
			}
			break;
		case PE_PLAYER_DISCONNECTED:
			m_RemoveTime = GetTickCount() + PE_PLAYER_DISCONNECT_TIME_GAP;
			break;
		case PE_PLAYER_DISCONNECTED_WAIT_REMOVE:
			break;
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int	CPublicEncounterPlayer::GetGUID()
{
	int Result = -1;

	if (pObj)
	{
		Result = pObj->GUID();
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int	CPublicEncounterPlayer::GetPartyID()
{
	int Result = -1;

	if (pRole)
	{
		Result = pRole->PartyID();
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int	CPublicEncounterPlayer::GetRxDamage()
{
	int Result = 0;

	if (pRole)
	{
		switch(Status)
		{
		case PE_PLAYER_DEACTIVATED:
			Result = m_TempStatictisData.iDefVal;
			break;
		case PE_PLAYER_ACTIVATED:
			Result = pRole->PlayerTempData->BGInfo.iDefVal;
			break;
		}
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int CPublicEncounterPlayer::GetTxDamage()
{
	int Result = 0;

	if (pRole)
	{
		switch(Status)
		{
		case PE_PLAYER_DEACTIVATED:
			Result = m_TempStatictisData.iDamageVal;
			break;
		case PE_PLAYER_ACTIVATED:
			Result = pRole->PlayerTempData->BGInfo.iDamageVal;
			break;
		}
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int CPublicEncounterPlayer::GetTxHeal()
{
	int Result = 0;

	if (pRole)
	{
		switch(Status)
		{
		case PE_PLAYER_DEACTIVATED:
			Result = m_TempStatictisData.iHealVal;
			break;
		case PE_PLAYER_ACTIVATED:
			Result = pRole->PlayerTempData->BGInfo.iHealVal;
			break;
		}
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int	CPublicEncounterPlayer::GetLevel()
{
	int Result = 0;

	if (pRole)
	{
		Result = pRole->Level();
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterPlayer::Process(unsigned long dwNow, unsigned long dwElapsedTime)
{
	switch(Status)
	{
	case PE_PLAYER_DEACTIVATED:
		{
			if (IsPartyChange())
			{
				Score = 0;
				m_OldPartyID = PartyID;
			}

			if (m_StatisticsClearTime > 0)
			{
				if (dwNow >= m_StatisticsClearTime)
				{
					ClearBattleStatictics();	
					m_StatisticsClearTime = 0;
				}
			}
		}
		break;
	case PE_PLAYER_ACTIVATED:
		{
			if (IsPartyChange())
			{
				Score = 0;
				m_OldPartyID = PartyID;
			}			
		}
		break;
	case PE_PLAYER_DISCONNECTED:
		{
			if (m_Status == PE_PLAYER_DISCONNECTED)
			{
				if (dwNow >= m_RemoveTime)
				{
					Status = PE_PLAYER_DISCONNECTED_WAIT_REMOVE;
					m_RemoveTime = 0;
				}
			}
		}
		break;
	case PE_PLAYER_DISCONNECTED_WAIT_REMOVE:
		break;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CPublicEncounterPlayer::IsPartyChange()
{
	bool Result = false;

	if (pRole)
	{
		if (PartyID != m_OldPartyID)
		{
			Result = true;
		}
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterPlayer::ClearBattleStatictics()
{
	if (pRole)
	{
		memset( &(pRole->PlayerTempData->BGInfo) , 0 , sizeof( pRole->PlayerTempData->BGInfo ));
		SecureZeroMemory(&m_TempStatictisData, sizeof(m_TempStatictisData));
		m_TempHatePoint = 0;
		m_ContributionPoint = 0;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterPlayer::ClearAllData()
{
	ClearBattleStatictics();
	Score = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CPublicEncounterObjective
////////////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjective::CPublicEncounterObjective(CPublicEncounterPhase* pOwner)
{
	Status    = EM_OBJ_STATUS_PROCESSING;
	m_Owner	  = pOwner;
	m_NextPhase = NULL;
	m_OldValue = 0;
	IsMinor = false;
	Visible = true;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjective::~CPublicEncounterObjective()
{

}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounter* CPublicEncounterObjective::GetPE()
{
	CPublicEncounter* Result = NULL;

	if (m_Owner)
	{
		Result = m_Owner->Owner;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int	CPublicEncounterObjective::GetPHIndex()
{
	int iResult = 0;

	if (m_Owner)
	{
		iResult = m_Owner->Index;
	}

	return iResult;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int CPublicEncounterObjective::GetPEIndex()
{
	int iResult = 0;

	if (m_Owner)
	{
		if (m_Owner->Owner)
		{
			iResult = m_Owner->Owner->Index;
		}		
	}

	return iResult;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterObjective::SetStatus(int value)
{
	if (m_BaseData.iStatus != value)
	{
		m_BaseData.iStatus = value;

		PublicEncounterManager::SC_ObjectiveStatus(-1, PEIndex, PHIndex, Index, m_BaseData.iStatus);

		CPublicEncounterPhase* pPH = m_Owner;
		CPublicEncounter* pPE = NULL;

		if (pPH)
		{
			pPE = pPH->Owner;
		}

		switch(m_BaseData.iStatus)
		{
		case EM_OBJ_STATUS_PROCESSING:
			break;
		case EM_OBJ_STATUS_ACHIEVED:
			{
				if (pPE && pPH)
				{
					Print(LV2, "PE", "%s objective achieved", Name);
					pPE->OnOBAchieve(this);
				}
				else
				{
					Print(LV2, "PE", "call objective %s OnSuccess failed, PE = NULL or PH = NULL", Name);
				}				
			}
			break;
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CPublicEncounterObjective::GetVisible()
{
	return ((m_BaseData.iFlag & EM_OBJ_FLAG_VISIBLE) == EM_OBJ_FLAG_VISIBLE);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterObjective::SetVisible(bool value)
{
	if(value)
	{
		(m_BaseData.iFlag |= EM_OBJ_FLAG_VISIBLE);
	}
	else
	{
		m_BaseData.iFlag &= (~EM_OBJ_FLAG_VISIBLE);
	}

	PublicEncounterManager::SC_ObjectiveFlag(-1, PEIndex, PHIndex, Index, m_BaseData.iFlag);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CPublicEncounterObjective::GetMinorFlag()
{
	return ((m_BaseData.iFlag & EM_OBJ_FLAG_MINOR) == EM_OBJ_FLAG_MINOR);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterObjective::SetMinorFlag(bool value)
{
	if(value)
	{
		(m_BaseData.iFlag |= EM_OBJ_FLAG_MINOR);
	}
	else
	{
		m_BaseData.iFlag &= (~EM_OBJ_FLAG_MINOR);
	}

	PublicEncounterManager::SC_ObjectiveFlag(-1, PEIndex, PHIndex, Index, m_BaseData.iFlag);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
StructPEObjectiveBaseData*	CPublicEncounterObjective::GetBaseData()
{
	m_BaseData.iPHIndex  = PHIndex;
	m_BaseData.iPEIndex  = PEIndex;
	m_BaseData.iNowValue = NowValue;
	return &m_BaseData;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int	CPublicEncounterObjective::GetNowValue()
{
	return PublicEncounterManager::GetVar(m_VarName.c_str());
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterObjective::SetNowValue(int value)
{
	PublicEncounterManager::SetVar(m_VarName.c_str(), value);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterObjective::_GetStatusString(vector<string>& vecStrings)
{
	char buff[128];
	sprintf(buff, "Objective : Name = %s, Index = %d", Name, Index);
	vecStrings.push_back(buff);

	if (NextPhase)
	{
		sprintf(buff, "NaxtPhase : Name = %s, Index = %d", NextPhase->Name, NextPhase->Index);
	}
	else
	{
		sprintf(buff, "NaxtPhase : NULL");
	}
	vecStrings.push_back(buff);

	switch(Status)
	{
	case EM_OBJ_STATUS_PROCESSING:
		vecStrings.push_back("Status = EM_OBJ_STATUS_PROCESSING");
		break;
	case EM_OBJ_STATUS_ACHIEVED:
		vecStrings.push_back("Status = EM_OBJ_STATUS_ACHIEVED");
		break;
	}

	sprintf(buff, "%s : %s %s %d expected, %s = %d now", TypeString.c_str(), m_VarName.c_str(), OperatorString.c_str(), Doorsill,  m_VarName.c_str(), NowValue);

	vecStrings.push_back(buff);

	vecStrings.push_back("\n");
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterObjective::OnValueChange(int iOldValue, int iNewValue)
{
	PublicEncounterManager::SC_ObjectiveValueChage(-1, PEIndex, PHIndex, Index, iNewValue);
	Print(LV2, "PE", "%s value changed, now value=%d, threshold=%d", Name, iOldValue, iNewValue, Doorsill);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int CPublicEncounterObjective::Process(unsigned long dwNow, unsigned long dwElapsedTime)
{
	if (m_OldValue != NowValue)
	{
		OnValueChange(m_OldValue, NowValue);
		m_OldValue = NowValue;
	}	

	switch(Status)
	{
	case EM_OBJ_STATUS_PROCESSING:
		{
			if (Check(dwNow, dwElapsedTime))
			{
				Status = EM_OBJ_STATUS_ACHIEVED;
			}			
		}
		break;
	case EM_OBJ_STATUS_ACHIEVED:
		break;
	}

	return Status;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterObjective::Reset()
{
	Status = EM_OBJ_STATUS_PROCESSING;

	NowValue = InitValue;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjective* CPublicEncounterObjective::CreateInstance(int iType, CPublicEncounterPhase* pOwner)
{
	CPublicEncounterObjective* Result = NULL;
	switch(iType)
	{
	case EM_OBJ_TYPE_GREATER:
		Result = (CPublicEncounterObjective*) (new CPublicEncounterObjectiveGreater(pOwner));
		break;
	case EM_OBJ_TYPE_SMALLER:
		Result = (CPublicEncounterObjective*) (new CPublicEncounterObjectiveSmaller(pOwner));
		break;
	case EM_OBJ_TYPE_EQUAL:
		Result = (CPublicEncounterObjective*) (new CPublicEncounterObjectiveEqual(pOwner));
		break;
	case EM_OBJ_TYPE_NOT_EQUAL:
		Result = (CPublicEncounterObjective*) (new CPublicEncounterObjectiveNotEqual(pOwner));
		break;
	case EM_OBJ_TYPE_EQUAL_AND_GREATER:
		Result = (CPublicEncounterObjective*) (new CPublicEncounterObjectiveEqualAndGreater(pOwner));
		break;
	case EM_OBJ_TYPE_EQUAL_AND_SMALLER:
		Result = (CPublicEncounterObjective*) (new CPublicEncounterObjectiveEqualAndSmaller(pOwner));
		break;
	case EM_OBJ_TYPE_TIME_LIMIT:
		Result = (CPublicEncounterObjective*) (new CPublicEncounterObjectiveTimeLimit(pOwner));
		break;
	}

	return Result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CPublicEncounterObjectiveTerm_Greater
////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjectiveGreater::CPublicEncounterObjectiveGreater(CPublicEncounterPhase* pOwner)
: CPublicEncounterObjective(pOwner)
{
	Type = EM_OBJ_TYPE_GREATER;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjectiveGreater::~CPublicEncounterObjectiveGreater()
{

}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CPublicEncounterObjectiveGreater::Check(unsigned long dwNow, unsigned long dwElapsedTime)
{
	return (NowValue > Doorsill);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CPublicEncounterObjectiveTerm_Smaller
////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjectiveSmaller::CPublicEncounterObjectiveSmaller(CPublicEncounterPhase* pOwner)
: CPublicEncounterObjective(pOwner)
{
	Type = EM_OBJ_TYPE_SMALLER;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjectiveSmaller::~CPublicEncounterObjectiveSmaller()
{

}


//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CPublicEncounterObjectiveSmaller::Check(unsigned long dwNow, unsigned long dwElapsedTime)
{
	return (NowValue < Doorsill);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CPublicEncounterObjectiveTerm_Smaller
////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjectiveEqual::CPublicEncounterObjectiveEqual(CPublicEncounterPhase* pOwner)
: CPublicEncounterObjective(pOwner)
{
	Type = EM_OBJ_TYPE_EQUAL;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjectiveEqual::~CPublicEncounterObjectiveEqual()
{

}


//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CPublicEncounterObjectiveEqual::Check(unsigned long dwNow, unsigned long dwElapsedTime)
{
	return (NowValue == Doorsill);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CPublicEncounterObjectiveTerm_NotEqual
////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjectiveNotEqual::CPublicEncounterObjectiveNotEqual(CPublicEncounterPhase* pOwner)
: CPublicEncounterObjective(pOwner)
{
	Type = EM_OBJ_TYPE_NOT_EQUAL;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjectiveNotEqual::~CPublicEncounterObjectiveNotEqual()
{

}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CPublicEncounterObjectiveNotEqual::Check(unsigned long dwNow, unsigned long dwElapsedTime)
{
	return (NowValue != Doorsill);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CPublicEncounterObjectiveTerm_EqualAndGreater
////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjectiveEqualAndGreater::CPublicEncounterObjectiveEqualAndGreater(CPublicEncounterPhase* pOwner)
: CPublicEncounterObjective(pOwner)
{
	Type = EM_OBJ_TYPE_EQUAL_AND_GREATER;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjectiveEqualAndGreater::~CPublicEncounterObjectiveEqualAndGreater()
{

}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CPublicEncounterObjectiveEqualAndGreater::Check(unsigned long dwNow, unsigned long dwElapsedTime)
{
	return (NowValue >= Doorsill);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CPublicEncounterObjectiveTerm_EqualAndSmaller
////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjectiveEqualAndSmaller::CPublicEncounterObjectiveEqualAndSmaller(CPublicEncounterPhase* pOwner)
: CPublicEncounterObjective(pOwner)
{
	Type = EM_OBJ_TYPE_EQUAL_AND_SMALLER;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjectiveEqualAndSmaller::~CPublicEncounterObjectiveEqualAndSmaller()
{

}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CPublicEncounterObjectiveEqualAndSmaller::Check(unsigned long dwNow, unsigned long dwElapsedTime)
{
	return (NowValue <= Doorsill);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CPublicEncounterObjectiveTerm_TimeLimit
////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjectiveTimeLimit::CPublicEncounterObjectiveTimeLimit(CPublicEncounterPhase* pOwner)
: CPublicEncounterObjective(pOwner)
{
	Type = EM_OBJ_TYPE_TIME_LIMIT;
	m_LastSyncTick = GetTickCount();
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjectiveTimeLimit::~CPublicEncounterObjectiveTimeLimit()
{

}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CPublicEncounterObjectiveTimeLimit::Check(unsigned long dwNow, unsigned long dwElapsedTime)
{
	bool Result = false;

	int tmpValue = NowValue;

	if (tmpValue == 0)
	{
		Result = true;
	}
	else
	{
		//we have to finish the phase in time (count down)
		if (InitValue > 0)
		{
			tmpValue -= dwElapsedTime;
			if (tmpValue <=0)
			{
				tmpValue = 0;
				Result = true;
			}
		}
		//we have to hold certain time (time advance)
		else
		{
			tmpValue += dwElapsedTime;
			if (tmpValue >= 0)
			{
				tmpValue = 0;
				Result = true;
			}
		}
	}

	NowValue = tmpValue;

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterObjectiveTimeLimit::_GetOperatorString(vector<string>& vecStrings)
{
	char buff[128];
	sprintf(buff, "Objective : Name = %s, Index = %d", Name, Index);
	vecStrings.push_back(buff);

	if (NextPhase)
	{
		sprintf(buff, "NaxtPhase : Name = %s, Index = %d", NextPhase->Name, NextPhase->Index);
	}
	else
	{
		sprintf(buff, "NaxtPhase : NULL");
	}

	vecStrings.push_back(buff);

	switch(Status)
	{
	case EM_OBJ_STATUS_PROCESSING:
		vecStrings.push_back("Status = EM_OBJ_STATUS_PROCESSING");
		break;
	case EM_OBJ_STATUS_ACHIEVED:
		vecStrings.push_back("Status = EM_OBJ_STATUS_ACHIEVED");
		break;
	}

	sprintf(buff, "%s : Limit = %d, Time remain = %d", GetTypeString().c_str(), InitValue, NowValue);

	vecStrings.push_back(buff);

	vecStrings.push_back("\n");
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterObjectiveTimeLimit::OnValueChange(int iOldValue, int iNewValue)
{
	unsigned long dwNowTime = GetTickCount();
	if ((dwNowTime - m_LastSyncTick) >= PE_OBJ_TIMELIMIT_SYNC_TIME_GAP)
	{
		PublicEncounterManager::SC_ObjectiveValueChage(-1, PEIndex, PHIndex, Index, iNewValue);
		m_LastSyncTick = dwNowTime;
		Print(LV2, "PE", "%s time sync, now value=%d", Name, iNewValue);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CPublicEncounterPhase
////////////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterPhase::CPublicEncounterPhase(CPublicEncounter* pOnwer)
{
	m_Owner					= pOnwer;
	m_NextPhase				= NULL;
	Status					= EM_PHASE_STATUS_PROCESSING;
	m_NextPhaseTimeGap		= 0;
	m_WaitTime				= 0;
	m_BonusScore1			= 0;
	m_BonusScore2			= 0;
	m_IsCountContribution	= false;

	m_SuccessObjectives.clear();
	m_FailObjectives.clear();
	m_Objectives.clear();
	m_ObjectivesIndex.clear();
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterPhase::~CPublicEncounterPhase()
{
	for (int i = 0; i < (int)m_Objectives.size(); ++i )
	{
		CPublicEncounterObjective* pOB = m_Objectives[i];
		if (pOB)
		{
			delete pOB;
		}
	}

	m_SuccessObjectives.clear();
	m_FailObjectives.clear();
	m_Objectives.clear();
	m_ObjectivesIndex.clear();
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int CPublicEncounterPhase::GetPEIndex()
{
	int Result = 0;

	if (m_Owner)
	{
		Result = Owner->Index;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
StructPEPhaseBaseData* CPublicEncounterPhase::GetBaseData()
{
	m_BaseData.iPEIndex = PEIndex;
	return &m_BaseData;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterPhase::SetStatus(int value)
{
	if (m_BaseData.iStatus != value)
	{
		m_BaseData.iStatus = value;

		PublicEncounterManager::SC_PhaseStatus(-1, PEIndex, Index, m_BaseData.iStatus);

		CPublicEncounter* pPE = m_Owner;

		switch(m_BaseData.iStatus)
		{
		case EM_PHASE_STATUS_PROCESSING:
			break;
		case EM_PHASE_STATUS_SUCCESS:
			{
				if (pPE)
				{
					Print(LV2, "PE", "%s phase success", Name);
					pPE->OnPHSuccess(this);
				}
				else
				{
					Print(LV2, "PE", "call phase %s OnSuccess Failed, PE = NULL", Name);
				}
			}
			break;
		case EM_PHASE_STATUS_FAILED:
			{
				if (pPE)
				{
					Print(LV2, "PE", "%s phase failed", Name);

					pPE->OnPHFail(this);
				}
				else
				{
					Print(LV2, "PE", "call phase %s OnFail failed, PE = NULL", Name);
				}
			}
			break;
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjective* CPublicEncounterPhase::AddObjective(const char* pszName, const char* pszDesc, int iIndex, int iType, const char* pszVarName, int iInitValue, int iDoorsill, int iKind)
{
	CPublicEncounterObjective* Result = GetObjective(iIndex);

	StructPEObjectiveBaseData bakBaseData;
	CPublicEncounterPhase*	  bakNextPhase = NULL;

	bool bBackup = (Result != NULL);

	if (bBackup)
	{
		bakBaseData = *(Result->BaseData);
		bakNextPhase = Result->NextPhase;
		RemoveObject(iIndex);
		Result = NULL;
	}

	Result = CPublicEncounterObjective::CreateInstance(iType, this);

	if (Result != NULL)
	{
		if (bBackup)
		{
			*(Result->BaseData) = bakBaseData;
			Result->NextPhase = bakNextPhase;
		}

		Result->Name		= pszName;
		Result->Description = pszDesc;
		Result->Index		= iIndex;
		Result->Type		= iType;
		Result->VarName		= pszVarName;
		Result->InitValue	= iInitValue;
		Result->Doorsill	= iDoorsill;
		Result->Kind		= iKind;

		m_ObjectivesIndex[iIndex] = Result;
		m_Objectives.push_back(Result);

		switch(iKind)
		{
		case EM_OBJ_KIND_SUCCESSFUL:
			m_SuccessObjectives.push_back(Result);
			break;
		case EM_OBJ_KIND_FAIL:
			m_FailObjectives.push_back(Result);
			break;
		}
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjective* CPublicEncounterPhase::GetObjective(int iOBIndex)
{
	CPublicEncounterObjective* Result = NULL;

	map<int, CPublicEncounterObjective*>::iterator it = m_ObjectivesIndex.find(iOBIndex);

	if (it != m_ObjectivesIndex.end())
	{
		Result = it->second;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterPhase::RemoveObject(int iOBIndex)
{
	map<int, CPublicEncounterObjective*>::iterator it = m_ObjectivesIndex.find(iOBIndex);

	if (it != m_ObjectivesIndex.end())
	{
		CPublicEncounterObjective* pTarget = it->second;

		vector<CPublicEncounterObjective*>::iterator itST	= find(m_SuccessObjectives.begin(), m_SuccessObjectives.end(), pTarget);
		vector<CPublicEncounterObjective*>::iterator itFT	= find(m_FailObjectives.begin(), m_FailObjectives.end(), pTarget);
		vector<CPublicEncounterObjective*>::iterator itList = find(m_Objectives.begin(), m_Objectives.end(), pTarget);

		if (itST != m_SuccessObjectives.end())
		{
			m_SuccessObjectives.erase(itST);
		}

		if (itFT != m_FailObjectives.end())
		{
			m_FailObjectives.erase(itFT);
		}

		if (itList != m_Objectives.end())
		{
			m_Objectives.erase(itList);
		}

		if (pTarget != NULL)
		{
			delete pTarget;
		}

		m_ObjectivesIndex.erase(it);
	}	
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterPhase::SetObjectiveNextPhase(int iOBIndex, CPublicEncounterPhase* pPhase)
{
	CPublicEncounterObjective* pOB = GetObjective(iOBIndex);

	if (pOB)
	{
		switch(pOB->Kind)
		{
		case EM_OBJ_KIND_SUCCESSFUL:
			{
				switch(SuccLogic)
				{
				case EM_OBJ_LOGIC_AND:
					{
						for (int i = 0; i < (int)m_SuccessObjectives.size(); ++i)
						{
							pOB = m_SuccessObjectives[i];
							if (pOB)
							{
								pOB->NextPhase = pPhase;
							}
						}
					}
					break;
				case EM_OBJ_LOGIC_OR:
					pOB->NextPhase = pPhase;
					break;
				}
				break;
			}
		case EM_OBJ_KIND_FAIL:
			{
				switch(FailLogic)
				{
				case EM_OBJ_LOGIC_AND:
					{
						for (int i = 0; i < (int)m_FailObjectives.size(); ++i)
						{
							pOB = m_FailObjectives[i];
							if (pOB)
							{
								pOB->NextPhase = pPhase;
							}
						}
					}
					break;
				case EM_OBJ_LOGIC_OR:
					pOB->NextPhase = pPhase;
					break;
				}
				break;
			}
			
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterPhase::SetAllSuccObjectiveNextPhase(CPublicEncounterPhase* pPhase)
{
	for (int i = 0; i < (int)m_SuccessObjectives.size(); ++i)
	{
		CPublicEncounterObjective* pOB = m_SuccessObjectives[i];
		if (pOB)
		{
			pOB->NextPhase = pPhase;
		}
	}
}
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterPhase::SetAllFailObjectiveNextPhase(CPublicEncounterPhase* pPhase)
{
	for (int i = 0; i < (int)m_FailObjectives.size(); ++i)
	{
		CPublicEncounterObjective* pOB = m_FailObjectives[i];
		if (pOB)
		{
			pOB->NextPhase = pPhase;
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterPhase::_GetStatusString(vector<string>& vecStrings)
{
	char buff[128];

	vecStrings.push_back("---------------------------------");

	sprintf(buff, "Phase : Name = %s, Index = %d", Name, Index);
	vecStrings.push_back(buff);	

	vecStrings.push_back("---------------------------------");

	switch(Status)
	{
	case EM_PHASE_STATUS_PROCESSING:
		vecStrings.push_back("Status = EM_PHASE_STATUS_PROCESSING");
		break;
	case EM_PHASE_STATUS_SUCCESS:
		vecStrings.push_back("Status = EM_PHASE_STATUS_SUCCESS");
		break;
	case EM_PHASE_STATUS_SUCCESS_WAIT:
		vecStrings.push_back("Status = EM_PHASE_STATUS_SUCCESS_WAIT");
		break;
	case EM_PHASE_STATUS_FAILED:
		vecStrings.push_back("Status = EM_PHASE_STATUS_FAILED");
		break;
	case EM_PHASE_STATUS_FAILED_WAIT:
		vecStrings.push_back("Status = EM_PHASE_STATUS_FAILED_WAIT");
		break;
	}

	for (int i = 0; i < (int)m_Objectives.size(); ++i)
	{
		CPublicEncounterObjective* pOB = m_Objectives[i];

		if (pOB)
		{
			pOB->_GetStatusString(vecStrings);
		}
	}
	vecStrings.push_back("\n");
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounterPhase::Reset()
{
	for (int i = 0; i < (int)m_Objectives.size() ;++i)
	{
		m_Objectives[i]->Reset();
	}

	Status = EM_PHASE_STATUS_PROCESSING;
	m_IsCountContribution = false;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int CPublicEncounterPhase::Process(unsigned long dwNow, unsigned long dwElapsedTime)
{
	switch(Status)
	{
	case EM_PHASE_STATUS_PROCESSING:
		{
			bool bSucc = true;
			bool bFail = true;
			int iOBResult = 0;

			CPublicEncounterObjective* pOB = NULL;
			CPublicEncounterPhase* _NextPhase = NULL;

			if (m_FailObjectives.size() > 0)
			{
				//----------------------------------------------------------
				// check fail condition first
				switch(FailLogic)
				{
				case EM_OBJ_LOGIC_AND:
					{
						//all objective achieved, then the phase failed.
						for (int i = 0; i < (int)m_FailObjectives.size(); ++i)
						{
							pOB = m_FailObjectives[i];

							if (pOB)
							{
								iOBResult = pOB->Process(dwNow, dwElapsedTime);

								if (!pOB->IsMinor)
								{
									if (iOBResult == EM_OBJ_STATUS_PROCESSING)
									{
										bFail = false;
										break;
									}
									else
									{
										_NextPhase = pOB->NextPhase;
									}
								}
							}
						}
					}
					break;
				case EM_OBJ_LOGIC_OR:
					{
						bFail = false;

						//any one objective achieved, then the phase failed.
						for (int i = 0; i < (int)m_FailObjectives.size(); ++i)
						{
							pOB = m_FailObjectives[i];

							if (pOB)
							{
								iOBResult = pOB->Process(dwNow, dwElapsedTime);

								if (!pOB->IsMinor)
								{
									if (iOBResult == EM_OBJ_STATUS_ACHIEVED)
									{
										bFail = true;
										_NextPhase = pOB->NextPhase;
										break;
									}
								}
							}
						}
					}
					break;
				}
				//----------------------------------------------------------
			}
			else
			{
				bFail = false;
			}

			if (m_SuccessObjectives.size() > 0)
			{
				//----------------------------------------------------------
				//then check successful condition
				switch(SuccLogic)
				{
				case EM_OBJ_LOGIC_AND:
					{
						//all objective achieved, then the phase success.
						for (int i = 0; i < (int)m_SuccessObjectives.size(); ++i)
						{
							pOB = m_SuccessObjectives[i];

							if (pOB)
							{
								iOBResult = pOB->Process(dwNow, dwElapsedTime);

								if (!pOB->IsMinor)
								{
									if (iOBResult == EM_OBJ_STATUS_PROCESSING)
									{
										bSucc = false;
										break;
									}
									else
									{
										_NextPhase = pOB->NextPhase;
									}
								}
							}
						}
					}
					break;
				case EM_OBJ_LOGIC_OR:
					{
						bSucc = false;

						//any one objective achieved, then the phase success.
						for (int i = 0; i < (int)m_SuccessObjectives.size(); ++i)
						{
							pOB = m_SuccessObjectives[i];

							if (pOB)
							{
								iOBResult = pOB->Process(dwNow, dwElapsedTime);

								if (!pOB->IsMinor)
								{
									if (iOBResult == EM_OBJ_STATUS_ACHIEVED)
									{
										bSucc = true;
										_NextPhase = pOB->NextPhase;
										break;
									}
								}
							}
						}
					}
					break;
				}
				//----------------------------------------------------------
			}
			else
			{
				bSucc = true;
			}

			int _Status = EM_PHASE_STATUS_PROCESSING;

			if (bFail)
			{
				_Status = EM_PHASE_STATUS_FAILED_WAIT;
				m_WaitTime = m_NextPhaseTimeGap;
				NextPhase = _NextPhase;
			}

			if (bSucc)
			{
				_Status = EM_PHASE_STATUS_SUCCESS_WAIT;
				m_WaitTime = m_NextPhaseTimeGap;
				NextPhase = _NextPhase;
			}

			Status = _Status;
		}
		break;
	case EM_PHASE_STATUS_SUCCESS:
		break;
	case EM_PHASE_STATUS_SUCCESS_WAIT:
		{
			if (m_WaitTime > dwElapsedTime)
			{
				m_WaitTime -= dwElapsedTime;
			}
			else
			{
				m_WaitTime = 0;
			}
			
			if (m_WaitTime == 0)
			{
				Status = EM_PHASE_STATUS_SUCCESS;
			}
		}
		break;
	case EM_PHASE_STATUS_FAILED:
		break;
	case EM_PHASE_STATUS_FAILED_WAIT:
		{
			if (m_WaitTime > dwElapsedTime)
			{
				m_WaitTime -= dwElapsedTime;
			}
			else
			{
				m_WaitTime = 0;
			}

			if (m_WaitTime == 0)
			{
				Status = EM_PHASE_STATUS_FAILED;
			}
		}
		break;
	}

	return Status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CPublicEncounter
////////////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounter::CPublicEncounter(const char* pszName)
{
	Name = pszName;

	m_Regions.clear();
	m_Phases.clear();
	m_PhasesIndex.clear();
	m_Players.clear();

	m_ResetBeginTime	= 0;
	m_ResetTimeGap		= 0;

	m_LuaOnBegin		= "";
	m_LuaOnSuccess		= "";
	m_LuaOnFail			= "";

	m_LuaOnPHBegin		= "";
	m_LuaOnPHSuccess	= "";
	m_LuaOnPHFail		= "";

	m_LuaOnOBAchieve	= "";
	m_LuaOnBalance		= "";

	m_LuaOnPlayerEnter	= "";
	m_LuaOnPlayerLeave	= "";

	Status				= EM_PE_STATUS_SLEEPING;
	ScoreVisible		= true;
	UIVisible			= true;
	m_ActivePhase		= NULL;

	m_NowTime			= GetTickCount();
	m_PreTime			= m_NowTime;	
	m_ElapsedTime		= 0;
	m_ActivePhaseIndex  = 0;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounter::~CPublicEncounter()
{
	for (int i = 0; i < (int)m_Phases.size(); ++i)
	{
		CPublicEncounterPhase* pPH = m_Phases[i];
		if (pPH)
		{
			delete pPH;
		}
	}

	map<int, CPublicEncounterPlayer*>::iterator it;

	for (it = m_Players.begin(); it != m_Players.end(); it++)
	{
		if (it->second)
		{
			delete it->second;
		}
	}

	m_Players.clear();
	m_Phases.clear();
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::SetActivePhase(CPublicEncounterPhase* value)
{
	m_ActivePhase = value;

	int iActivePhaseIndex = -1;

	if (m_ActivePhase)
	{
		iActivePhaseIndex = m_ActivePhase->Index;		

		OnPHBegin(m_ActivePhase);

		Print(LV2, "PE", "%s current phase = %s", Name, m_ActivePhase->Name);
	}

	PublicEncounterManager::SC_PEActivePhaseChange(-1, Index, iActivePhaseIndex);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::SetStatus(int value)
{	
	if (m_BaseData.iStatus != value)
	{
		m_BaseData.iStatus = value;

		PublicEncounterManager::SC_PEStatus(-1, Index, Status);

		vector<MyVMValueStruct> vecArgs;
		vector<MyVMValueStruct> vecResult;
		MyVMValueStruct tmpArg;

		switch(m_BaseData.iStatus)
		{
		case EM_PE_STATUS_SLEEPING:
			break;
		case EM_PE_STATUS_PAUSE:
			break;
		case EM_PE_STATUS_PROCESSING:
			break;
		case EM_PE_STATUS_WAIT_FOR_RESET:
			{
				m_ResetBeginTime = m_NowTime + m_ResetTimeGap;
				Print(LV2, "PE", "%s will be reset after %d ms", Name, m_ResetTimeGap);
			}
			break;
		case EM_PE_STATUS_SUCCESS:
			{
				vecArgs.clear();
				vecResult.clear();

				//PEIndex
				tmpArg.Type = MyVMValueType_Number;
				tmpArg.Number = (double)Index;
				vecArgs.push_back(tmpArg);

				//PHIndex
				tmpArg.Type = MyVMValueType_Number;
				tmpArg.Number = 0;
				vecArgs.push_back(tmpArg);

				//OBIndex
				tmpArg.Type = MyVMValueType_Number;
				tmpArg.Number = 0;
				vecArgs.push_back(tmpArg);

				//area event is successful, call OnEventSuccess lua if we have it
				PublicEncounterManager::CallLuaFunc(m_LuaOnSuccess, &vecArgs, &vecResult);

				//Balance the public encounter, count scores and rewards
				OnBalance();

				//prepare to restart public encounter
				Status = EM_PE_STATUS_WAIT_FOR_RESET;

				PublicEncounterManager::SC_PESuccess(-1, Index);
			}
			break;
		case EM_PE_STATUS_FAILED:
			{
				vecArgs.clear();
				vecResult.clear();

				//PEIndex
				tmpArg.Type = MyVMValueType_Number;
				tmpArg.Number = (double)Index;
				vecArgs.push_back(tmpArg);

				//PHIndex
				tmpArg.Type = MyVMValueType_Number;
				tmpArg.Number = 0;
				vecArgs.push_back(tmpArg);

				//OBIndex
				tmpArg.Type = MyVMValueType_Number;
				tmpArg.Number = 0;
				vecArgs.push_back(tmpArg);

				// phase failed then event failed too, call OnEventFail if we have it
				PublicEncounterManager::CallLuaFunc(m_LuaOnFail, &vecArgs, &vecResult);

				//Balance the public encounter, count scores and rewards
				OnBalance();

				//prepare to restart public encounter
				Status = EM_PE_STATUS_WAIT_FOR_RESET;

				PublicEncounterManager::SC_PEFailed(-1, Index);				
			}
			break;
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CPublicEncounter::GetScoreVisible()
{
	return ((m_BaseData.iFlag & EM_PE_FLAG_SCORE_VISIBLE) == EM_PE_FLAG_SCORE_VISIBLE);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::SetScoreVisible(bool value)
{
	if(value)
	{
		(m_BaseData.iFlag |= EM_PE_FLAG_SCORE_VISIBLE);
	}
	else
	{
		m_BaseData.iFlag &= (~EM_PE_FLAG_SCORE_VISIBLE);
	}

	PublicEncounterManager::SC_PEFlag(-1, Index, m_BaseData.iFlag);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CPublicEncounter::GetUIVisible()
{
	return ((m_BaseData.iFlag & EM_PE_FLAG_UI_VISIBLE) == EM_PE_FLAG_UI_VISIBLE);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::SetUIVisible(bool value)
{
	if(value)
	{
		(m_BaseData.iFlag |= EM_PE_FLAG_UI_VISIBLE);
	}
	else
	{
		m_BaseData.iFlag &= (~EM_PE_FLAG_UI_VISIBLE);
	}

	PublicEncounterManager::SC_PEFlag(-1, Index, m_BaseData.iFlag);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::AddRegion(int iRegionID)
{	
	StructPERegionData newRegion;
	newRegion.iPEIndex  = Index;
	newRegion.iRegionID = iRegionID;
	m_Regions.push_back(newRegion);
}


//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::AddPhase(const char* pszName, const char* pszDesc, int iIndex, float fScore, int iSuccLogic, int iFailLogic, int iTimeGap)
{
	CPublicEncounterPhase* pPH = GetPhase(iIndex);

	if (pPH == NULL)
	{
		pPH = new CPublicEncounterPhase(this);
		m_PhasesIndex[iIndex] = pPH;
		m_Phases.push_back(pPH);
	}

	pPH->Index				= iIndex;
	pPH->Name				= pszName;
	pPH->Description		= pszDesc;
	pPH->PhaseScore			= fScore;
	pPH->SuccLogic			= iSuccLogic;
	pPH->FailLogic			= iFailLogic;
	pPH->NextPhaseTimeGap	= iTimeGap;
}

//-------------------------------------------------------------------------------
// Jump to specific phase
//-------------------------------------------------------------------------------
bool CPublicEncounter::JumpToPhase(int iIndex)
{
	bool Result = false;

	CPublicEncounterPhase* pTarget = GetPhase(iIndex);

	if (pTarget)
	{
		ActivePhase = pTarget;
		ActivePhase->Reset();

		Print(LV2, "PE", "%s Jump to phase %s", Name, ActivePhase->Name);

		Result = true;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::SetPhaseBonusScore(float fBonusScore1, float fBonusScore2)
{
	for (int i = 0; i < (int)m_Phases.size(); ++i)
	{
		m_Phases[i]->BonusScore1 = fBonusScore1;
		m_Phases[i]->BonusScore2 = fBonusScore2;
	}
}


//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::GivePlayerScore(int iGUID , float fScore)
{
	CPublicEncounterPlayer* pPlayer = GetPlayer(iGUID);

	if (pPlayer)
	{
		pPlayer->Score += fScore;
	}
	else
	{
		Print(LV2, "PE", "Player(GUID = %d) not find", iGUID);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::GivePartyScore(int iPartyID, float fScore)
{
	vector<int> vecParty;

	GetParty(iPartyID, vecParty);
	
	int iNum = (int)vecParty.size();

	if (iNum > 0)
	{
		float fShareScore = fScore / (float)iNum;

		for(int i = 0; i < iNum; ++i)
		{
			GivePlayerScore(vecParty[i], fShareScore);
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::GiveAllPlayersScore(float fScore)
{
	map<int, CPublicEncounterPlayer*>::iterator itPlayer;

	for (itPlayer = m_Players.begin(); itPlayer != m_Players.end(); itPlayer++)
	{
		if (itPlayer->second)
		{
			itPlayer->second->Score += fScore;
		}
	}
}


//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
float CPublicEncounter::GetPlayerScore(int iGUID)
{
	float Result = 0;

	CPublicEncounterPlayer* pPlayer = GetPlayer(iGUID);

	if (pPlayer)
	{
		Result = pPlayer->Score;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::GetPartyScore(int iPartyID, vector<int>& vecGUID, vector<float>& vecScore)
{
	GetParty(iPartyID, vecGUID);

	for (int i = 0; i < (int)vecGUID.size(); ++i)
	{
		vecScore.push_back(GetPlayerScore(vecGUID[i]));
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::GetTopNPlayersScore(vector<int>& vecGUID, vector<float>& vecScore, int iTopNum)
{
	//-------------------------------
	//Sort player's score
	vector<CPublicEncounterPlayer*> vecPlayerList;

	map<int, CPublicEncounterPlayer*>::iterator itPlayer;

	for (itPlayer = m_Players.begin(); itPlayer != m_Players.end(); itPlayer++)
	{
		if (itPlayer->second)
		{
			vecPlayerList.push_back(itPlayer->second);
		}
	}

	sort(vecPlayerList.begin(), vecPlayerList.end(), PE_Campare_Player_Score);
	//-------------------------------

	int iPlayerNum = (int)vecPlayerList.size();

	if (iTopNum > 0)
	{
		iPlayerNum = iTopNum;
	}

	for (int i = 0; i < iPlayerNum; ++i)
	{
		CPublicEncounterPlayer* pPlayer = vecPlayerList[i];

		if (pPlayer)
		{
			vecGUID.push_back(pPlayer->GUID);
			vecScore.push_back(pPlayer->Score);
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::SetPlayerScore(int iGUID, float fScore)
{
	CPublicEncounterPlayer* pPlayer = GetPlayer(iGUID);

	if (pPlayer)
	{
		pPlayer->Score = fScore;
	}

}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::CountHatePoint(int iGUID)
{
	RoleDataEx* pNpc = Global::GetRoleDataByGUID( iGUID );

	if (pNpc)
	{
		for (int i = 0; i < pNpc->TempData.NPCHate.Hate.Size(); ++i)
		{
			NPCHateStruct& HateInfo = pNpc->TempData.NPCHate.Hate[i];

			map<int, CPublicEncounterPlayer*>::iterator itPlayer = m_Players.find(HateInfo.ItemID);

			if (itPlayer != m_Players.end())
			{
				if (itPlayer->second)
				{
					itPlayer->second->TempHatePoint += HateInfo.TotalHatePoint();
				}				
			}
		}
	}

	if (ActivePhase)
	{
		ActivePhase->IsCountContribution = true;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int	CPublicEncounter::GetActivePlayerGUIDCount()
{
	m_TempActivePlayerGUIDs.clear();

	map<int, CPublicEncounterPlayer*>::iterator it;

	for (it = m_Players.begin(); it != m_Players.end(); it++)
	{
		CPublicEncounterPlayer* pPlayer = it->second;
		if (pPlayer)
		{
			if (IsPlayerInPE(pPlayer->pObj))
			{
				m_TempActivePlayerGUIDs.push_back(pPlayer->GUID);
			}
		}		
	}

	return (int)m_TempActivePlayerGUIDs.size();
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int	CPublicEncounter::GetActivePlayerGUID()
{
	int Result = -1;

	if (m_TempActivePlayerGUIDs.size() > 0)
	{
		Result = m_TempActivePlayerGUIDs.front();
		m_TempActivePlayerGUIDs.pop_front();
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::_GetDetailString(vector<string>& vecStrings)
{
	_GetStatusString(vecStrings);

	for (int i = 0 ; i < (int)m_Phases.size(); ++i)
	{
		CPublicEncounterPhase* pPH = m_Phases[i];

		if (pPH)
		{
			pPH->_GetStatusString(vecStrings);
		}
	}

	vecStrings.push_back("================================");
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::_GetStatusString(vector<string>& vecStrings)
{
	char buff[128];

	vecStrings.push_back("================================");

	sprintf(buff, "Public Encounter : Name = %s, Index = %d", Name, Index);
	vecStrings.push_back(buff);

	vecStrings.push_back("================================");

	if (ActivePhase)
	{
		sprintf(buff, "Active Phase = %s, Phase Index = %d", ActivePhase->Name, ActivePhase->Index);
		vecStrings.push_back(buff);
	}
	else
	{
		vecStrings.push_back("No active phase.");
	}

	vecStrings.push_back("RegionIDs :");

	for (int i = 0; i < (int)m_Regions.size(); ++i)
	{
		sprintf(buff, "%d", m_Regions[i].iRegionID);
		vecStrings.push_back(buff);
	}

	switch(Status)
	{
	case EM_PE_STATUS_SLEEPING:
		vecStrings.push_back("Status = EM_PE_STATUS_SLEEPING");
		break;
	case EM_PE_STATUS_PAUSE:
		vecStrings.push_back("Status = EM_PE_STATUS_PAUSE");
		break;
	case EM_PE_STATUS_PROCESSING:
		vecStrings.push_back("Status = EM_PE_STATUS_PROCESSING");
		break;
	case EM_PE_STATUS_WAIT_FOR_RESET:
		vecStrings.push_back("Status = EM_PE_STATUS_WAIT_FOR_RESET");
		break;
	case EM_PE_STATUS_SUCCESS:
		vecStrings.push_back("Status = EM_PE_STATUS_WAIT_FOR_RESET");
		break;
	case EM_PE_STATUS_FAILED:
		vecStrings.push_back("Status = EM_PE_STATUS_FAILED");	
		break;
	}

	vecStrings.push_back("\n");
}

//-------------------------------------------------------------------------------
// start public encounter with specific phase, if we don't give phase name, start with
// first phase
//-------------------------------------------------------------------------------
bool CPublicEncounter::Start(int iPHIndex)
{
	CPublicEncounterPhase* pPH = GetPhase(iPHIndex);

	if (pPH)
	{
		Stop();

		OnBegin();

		ActivePhase = pPH;
		m_ActivePhaseIndex = iPHIndex;
		Status = EM_PE_STATUS_PROCESSING;

		Print(LV2, "PE", "%s is started", Name);
	}
	else
	{
		Print(LV2, "PE", "%s is failed when starting.", Name);
	}

	return (Status == EM_PE_STATUS_PROCESSING);
}

//-------------------------------------------------------------------------------
//restart public encounter
//-------------------------------------------------------------------------------
void CPublicEncounter::Reset()
{
	Stop();
	Start(m_ActivePhaseIndex);

	//
	map<int, CPublicEncounterPlayer*>::iterator itPlayer;

	for (itPlayer = m_Players.begin(); itPlayer != m_Players.end(); itPlayer++)
	{
		CPublicEncounterPlayer* pPlayer = itPlayer->second;
		if (pPlayer)
		{
			if (IsPlayerInPE(pPlayer->pObj))
			{
				pPlayer->Status = PE_PLAYER_ACTIVATED;
				PublicEncounterManager::SC_PEActivated(pPlayer->DBID, Index, ((ActivePhase == NULL) ? -1 : ActivePhase->Index));
			}
			else
			{
				pPlayer->Status = PE_PLAYER_DEACTIVATED;
				PublicEncounterManager::SC_PEDeactivated(pPlayer->DBID, Index);
			}
		}
	}

	Print(LV2, "PE", "%s is restarted", Name);
}

//-------------------------------------------------------------------------------
//Process public encounter
//-------------------------------------------------------------------------------
void CPublicEncounter::Process()
{
	//----------------------
	//update time
	m_NowTime	  = GetTickCount();
	m_ElapsedTime = m_NowTime - m_PreTime;
	//----------------------

	//-----------------------------------------------------------------------------------------
	//process Player status	
	CPublicEncounterPlayer* pPlayer = NULL;
	map<int, CPublicEncounterPlayer*>::iterator itPlayer;

	//remove disconnected player
	for (itPlayer = m_Players.begin(); itPlayer != m_Players.end(); itPlayer++)
	{
		pPlayer = itPlayer->second;

		if (pPlayer)
		{
			if (pPlayer->Status == PE_PLAYER_DISCONNECTED_WAIT_REMOVE)
			{
				itPlayer = m_Players.erase(itPlayer);

				delete pPlayer;

				if (itPlayer == m_Players.end())
				{
					break;
				}
			}
		}
	}

	//check player party status
	for (itPlayer = m_Players.begin(); itPlayer != m_Players.end(); itPlayer++)
	{
		pPlayer = itPlayer->second;

		if (pPlayer)
		{
			pPlayer->Process(m_NowTime, m_ElapsedTime);
		}
	}
	//-----------------------------------------------------------------------------------------

	//-----------------------------------------------------------------------------------------
	switch(Status)
	{
	case EM_PE_STATUS_SLEEPING:
		break;
	case EM_PE_STATUS_PAUSE:
		break;
	case EM_PE_STATUS_PROCESSING:
		{
			//---------------------------------------------------------------------------------
			//process current phase
			if (ActivePhase)
			{
				switch(ActivePhase->Process(m_NowTime, m_ElapsedTime))
				{
				case EM_PHASE_STATUS_PROCESSING:
					break;
				case EM_PHASE_STATUS_SUCCESS:
					{
						//if there is next phase, shift to next phase otherwise PE is successful.
						if (!AdvanceToNextPhase())
						{
							Status = EM_PE_STATUS_SUCCESS;
						}
					}
					break;
				case EM_PHASE_STATUS_SUCCESS_WAIT:
					break;
				case EM_PHASE_STATUS_FAILED:
					{
						//if there is next phase, shift to next phase otherwise PE is failed.
						if (!AdvanceToNextPhase())
						{
							Status = EM_PE_STATUS_FAILED;
						}
					}
					break;
				case EM_PHASE_STATUS_FAILED_WAIT:
					break;
				}
			}
			//There is no active phase, this is a wrong state
			else
			{
				Status = EM_PE_STATUS_SLEEPING;
			}
		}
		break;
	case EM_PE_STATUS_SUCCESS:
		break;
	case EM_PE_STATUS_FAILED:
		break;
	case EM_PE_STATUS_WAIT_FOR_RESET:
		{
			// check restart begin time
			if (m_NowTime >= m_ResetBeginTime)
			{
				Reset();
			}
		}
		break;
	}

	m_PreTime = m_NowTime;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::Stop()
{
	Status      = EM_PE_STATUS_SLEEPING;
	ActivePhase = NULL;

	//--------------------------------------------
	//reset all phase
	for (int i = 0; i < (int)m_Phases.size(); ++i )
	{
		CPublicEncounterPhase* pPhase = m_Phases[i];
		if (pPhase != NULL)
		{
			pPhase->Reset();
		}
	}
	//--------------------------------------------

	//-----------------------------------------------------------------------------------------
	//clear all Player's Data
	map<int, CPublicEncounterPlayer*>::iterator itPlayer;

	for (itPlayer = m_Players.begin(); itPlayer != m_Players.end(); itPlayer++)
	{
		if (itPlayer->second)
		{
			itPlayer->second->ClearAllData();
		}
	}
	//-----------------------------------------------------------------------------------------

	Print(LV2, "PE", "%s stopped", Name);
}

//-------------------------------------------------------------------------------
// shift to next phase
//-------------------------------------------------------------------------------
bool CPublicEncounter::AdvanceToNextPhase()
{	
	if (ActivePhase)
	{
		ActivePhase = ActivePhase->NextPhase;
	}

	return (ActivePhase != NULL);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::CliConnect(int iDBID)
{
	BaseItemObject* pObj = Global::GetPlayerObj_ByDBID(iDBID);

	if (pObj)
	{
		//Index by DBID first;
		map<int, CPublicEncounterPlayer*>::iterator itDBID = m_Players.find(iDBID);

		CPublicEncounterPlayer* pPlayer = NULL;

		//existing player
		if (itDBID != m_Players.end())
		{
			pPlayer = itDBID->second;
		}
		//new player
		else
		{
			pPlayer = new CPublicEncounterPlayer(this, iDBID);
			m_Players[iDBID] = pPlayer;
		}

		if (pPlayer)
		{
			if (IsPlayerInPE(pObj))
			{
				pPlayer->Status = PE_PLAYER_ACTIVATED;
			}
			else
			{
				pPlayer->Status = PE_PLAYER_DEACTIVATED;
			}
		}
	}
}


//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::CliDisconnect(int iDBID)
{
	map<int, CPublicEncounterPlayer*>::iterator it = m_Players.find(iDBID);

	if (it != m_Players.end())
	{
		CPublicEncounterPlayer* pPlayer = it->second;

		if (pPlayer != NULL)
		{
			pPlayer->Status = PE_PLAYER_DISCONNECTED;
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::CliRegionChanged(BaseItemObject* pObj)
{
	if ((pObj == NULL) || (pObj->Role() == NULL))
		return;

	int iDBID = pObj->Role()->DBID();

	map<int, CPublicEncounterPlayer*>::iterator it = m_Players.find(iDBID);
	if (it != m_Players.end())
	{
		CPublicEncounterPlayer* pPlayer = it->second;

		if (pPlayer != NULL)
		{
			if (IsPlayerInPE(pObj))
			{
				pPlayer->Status = PE_PLAYER_ACTIVATED;
				OnPlayerEnter(pPlayer);
				PublicEncounterManager::SC_PEActivated(iDBID, Index, ((ActivePhase == NULL) ? -1 : ActivePhase->Index));
			}
			else
			{
				pPlayer->Status = PE_PLAYER_DEACTIVATED;
				OnPlayerLeave(pPlayer);
				PublicEncounterManager::SC_PEDeactivated(iDBID, Index);
			}
		}
	}
}


//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterPhase* CPublicEncounter::GetPhase(int iIndex)
{
	CPublicEncounterPhase* Result = NULL;

	map<int, CPublicEncounterPhase*>::iterator it = m_PhasesIndex.find(iIndex);

	if (it != m_PhasesIndex.end())
	{
		Result = it->second;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterPlayer*	CPublicEncounter::GetPlayer(int iGUID)
{
	CPublicEncounterPlayer* Result = NULL;

	RoleDataEx* pRole = Global::GetRoleDataByGUID(iGUID);

	if (pRole)
	{
		map<int, CPublicEncounterPlayer*>::iterator it = m_Players.find(pRole->DBID());

		if (it != m_Players.end())
		{
			Result = it->second;
		}
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::GetParty(int iPartyID, vector<int>& vecParty)
{
	vecParty.clear();

	map<int, CPublicEncounterPlayer*>::iterator it;

	for(it = m_Players.begin(); it != m_Players.end(); it++)
	{
		CPublicEncounterPlayer* pPlayer = it->second;
		if (pPlayer)
		{
			if (pPlayer->PartyID == iPartyID)
			{
				vecParty.push_back(pPlayer->GUID);
			}			
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::GetAllParty(map<int, vector<int>>& mapParty)
{
	mapParty.clear();

	map<int, CPublicEncounterPlayer*>::iterator it;

	for(it = m_Players.begin(); it != m_Players.end(); it++)
	{
		CPublicEncounterPlayer* pPlayer = it->second;

		if (pPlayer)
		{
			if (pPlayer->pRole)
			{
				int iGUID = pPlayer->GUID;
				int iPartyID = pPlayer->PartyID;

				if (pPlayer->PartyID >= 0)
				{
					map<int, vector<int>>::iterator itParty = mapParty.find(iPartyID);

					if (itParty != mapParty.end())
					{
						vector<int>::iterator itPlayer = find(itParty->second.begin(), itParty->second.end(), iGUID);

						if (itPlayer == itParty->second.end())
						{
							itParty->second.push_back(iGUID);
						}
					}
					else
					{
						vector<int> vecNewParty;

						vecNewParty.push_back(iGUID);

						mapParty[iPartyID] = vecNewParty;
					}
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CPublicEncounter::IsPlayerInRegion(BaseItemObject* pObj, int iRegionID)
{
	bool Result = false;

	if (pObj)
	{
		RoleDataEx* pRole = pObj->Role();

		if (pRole)
		{
			Result = PathManageClass::CheckMapID(pRole->X(), pRole->Y(), pRole->Z(), iRegionID);
		}
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool CPublicEncounter::IsPlayerInPE(BaseItemObject* pObj)
{
	bool Result = false;

	if (pObj)
	{
		RoleDataEx* pRole = pObj->Role();

		if (pRole)
		{
			int MainRegionID = RoleDataEx::G_ZoneID + 10000;
			int RegionID = PathManageClass::FindMapID(pRole->X(), pRole->Y(), pRole->Z());			

			for (int i = 0; i < (int)m_Regions.size(); ++i)
			{
				if (m_Regions[i].iRegionID == MainRegionID)
				{
					Result = true;
					break;
				}

				if (m_Regions[i].iRegionID == RegionID)
				{
					Result = true;
					break;
				}
			}
		}
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::OnRefreshConfig()
{
	map<int, CPublicEncounterPlayer*>::iterator it;

	for (it = m_Players.begin(); it != m_Players.end(); it++)
	{
		CPublicEncounterPlayer* pPlayer = it->second;

		if (pPlayer != NULL)
		{
			if (pPlayer->pObj != NULL)
			{
				if (IsPlayerInPE(pPlayer->pObj))
				{
					PublicEncounterManager::SC_PEActivated(pPlayer->DBID, Index, ((ActivePhase == NULL) ? -1 : ActivePhase->Index));
				}
				else
				{
					PublicEncounterManager::SC_PEDeactivated(pPlayer->DBID, Index);
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::OnBegin()
{
	vector<MyVMValueStruct> vecArgs;
	vector<MyVMValueStruct> vecResult;

	MyVMValueStruct tmpArg;

	//PEIndex
	tmpArg.Type = MyVMValueType_Number;
	tmpArg.Number = (double)Index;
	vecArgs.push_back(tmpArg);

	PublicEncounterManager::CallLuaFunc(LuaOnBegin, &vecArgs, &vecResult);
}

//-------------------------------------------------------------------------------
// finalize public encounter, count scores and rewards
//-------------------------------------------------------------------------------
void CPublicEncounter::OnBalance()
{
	//-------------------------------
	//Sort player's score
	vector<CPublicEncounterPlayer*> vecPlayerList;

	map<int, CPublicEncounterPlayer*>::iterator itPlayer;

	for (itPlayer = m_Players.begin(); itPlayer != m_Players.end(); itPlayer++)
	{
		if (itPlayer->second)
		{
			vecPlayerList.push_back(itPlayer->second);
		}
	}

	sort(vecPlayerList.begin(), vecPlayerList.end(), PE_Campare_Player_Score);
	//-------------------------------

	//-------------------------------
	//call user define callback function to calculate the token number that each player will get
	int iPlayerNum = (int)vecPlayerList.size();

	vector<MyVMValueStruct> vecArgs;
	vector<MyVMValueStruct> vecResult;

	MyVMValueStruct tmpArg;

	for (int i = 0; i < iPlayerNum; ++i)
	{
		CPublicEncounterPlayer* pPlayer = vecPlayerList[i];

		vecArgs.clear();
		vecResult.clear();

		if (pPlayer)
		{
			if (pPlayer->pObj)
			{
				//PEIndex
				tmpArg.Type = MyVMValueType_Number;
				tmpArg.Number = (double)Index;
				vecArgs.push_back(tmpArg);

				//Player's GUID
				tmpArg.Type = MyVMValueType_Number;
				tmpArg.Number = (double)(pPlayer->pObj->GUID());
				vecArgs.push_back(tmpArg);

				//Player's score
				tmpArg.Type = MyVMValueType_Number;
				tmpArg.Number = (double)(pPlayer->Score);
				vecArgs.push_back(tmpArg);

				PublicEncounterManager::CallLuaFunc(LuaOnBalance, &vecArgs, &vecResult);
			}
		}
	}
	//-------------------------------
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::OnPHBegin(CPublicEncounterPhase* pPH)
{
	if (pPH == NULL)
		return;

	vector<MyVMValueStruct> vecArgs;
	vector<MyVMValueStruct> vecResult;

	MyVMValueStruct tmpArg;

	//PEIndex
	tmpArg.Type = MyVMValueType_Number;
	tmpArg.Number = (double)Index;
	vecArgs.push_back(tmpArg);

	//PHIndex
	tmpArg.Type = MyVMValueType_Number;
	tmpArg.Number = (double)pPH->Index;
	vecArgs.push_back(tmpArg);

	//OBIndex
	tmpArg.Type = MyVMValueType_Number;
	tmpArg.Number = 0;
	vecArgs.push_back(tmpArg);

	PublicEncounterManager::CallLuaFunc(LuaOnPHBegin, &vecArgs, &vecResult);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::OnPHSuccess(CPublicEncounterPhase* pPH)
{
	if (pPH == NULL)
		return;

	vector<MyVMValueStruct> vecArgs;
	vector<MyVMValueStruct> vecResult;

	MyVMValueStruct tmpArg;

	//PEIndex
	tmpArg.Type = MyVMValueType_Number;
	tmpArg.Number = (double)Index;
	vecArgs.push_back(tmpArg);

	//PHIndex
	tmpArg.Type = MyVMValueType_Number;
	tmpArg.Number = (double)pPH->Index;
	vecArgs.push_back(tmpArg);

	//OBIndex
	tmpArg.Type = MyVMValueType_Number;
	tmpArg.Number = 0;
	vecArgs.push_back(tmpArg);


	OnPHBalance(pPH);
	PublicEncounterManager::CallLuaFunc(LuaOnPHSuccess, &vecArgs, &vecResult);
	PublicEncounterManager::SC_PhaseSuccess(-1, Index, pPH->Index);
}
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::OnPHFail(CPublicEncounterPhase* pPH)
{
	if (pPH == NULL)
		return;

	vector<MyVMValueStruct> vecArgs;
	vector<MyVMValueStruct> vecResult;

	MyVMValueStruct tmpArg;

	//PEIndex
	tmpArg.Type = MyVMValueType_Number;
	tmpArg.Number = (double)Index;
	vecArgs.push_back(tmpArg);

	//PHIndex
	tmpArg.Type = MyVMValueType_Number;
	tmpArg.Number = pPH->Index;
	vecArgs.push_back(tmpArg);

	//OBIndex
	tmpArg.Type = MyVMValueType_Number;
	tmpArg.Number = 0;
	vecArgs.push_back(tmpArg);

	OnPHBalance(pPH);
	PublicEncounterManager::CallLuaFunc(LuaOnPHFail, &vecArgs, &vecResult);
	PublicEncounterManager::SC_PhaseFailed(-1, Index, pPH->Index);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::OnPHBalance(CPublicEncounterPhase* pPH)
{
	if (!pPH)
		return;

	//----------------------------------------------------------
	//give phase party bonus score
	map<int, vector<int>> mapAllParty;

	GetAllParty(mapAllParty);

	map<int, vector<int>>::iterator it;

	for (it = mapAllParty.begin(); it != mapAllParty.end(); it++)
	{
		vector<int> vecParty = it->second;

		//if you are in a party then you will get the bonus score1
		float fTotalBonusScore = pPH->BonusScore1;

		//In a raid, you can get extra bonus score = bonus score2
		if (vecParty.size() > 6)
		{
			fTotalBonusScore += pPH->BonusScore2;
		}

		//Give each player in party the bonus score.
		for (int i = 0; i < (int)vecParty.size(); ++i)
		{
			GivePlayerScore(vecParty[i], fTotalBonusScore);
		}
	}
	//----------------------------------------------------------

	if (pPH->IsCountContribution)
	{
		//----------------------------------------------------------
		//Give bonus score from Player's contribution for the phase

		//Count total contribution point
		float fTotalContribution = 0;

		CPublicEncounterPlayer* pPlayer = NULL;

		map<int, CPublicEncounterPlayer*>::iterator itPlayer;
		for (itPlayer = m_Players.begin(); itPlayer != m_Players.end(); itPlayer++)
		{
			pPlayer = itPlayer->second;
			if (pPlayer)
			{
				pPlayer->ContributionPoint = PublicEncounterManager::CalculateContribution(pPlayer->TempHatePoint, pPlayer->RxDamage, pPlayer->TxDamage, pPlayer->TxHeal, pPlayer->Level);
				fTotalContribution += pPlayer->ContributionPoint;
			}
		}

		//avoid divide by zero
		if (fTotalContribution > 0)
		{
			//Give each player score by their contribution percentage
			for (itPlayer = m_Players.begin(); itPlayer != m_Players.end(); itPlayer++)
			{
				pPlayer = itPlayer->second;
				if (pPlayer)
				{
					pPlayer->Score += pPH->PhaseScore * (pPlayer->ContributionPoint / fTotalContribution);
					pPlayer->ClearBattleStatictics();
				}
			}		
		}
		//----------------------------------------------------------
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::OnOBAchieve(CPublicEncounterObjective* pOB)
{
	if (pOB == NULL)
		return;

	CPublicEncounterPhase* pPH = pOB->Owner;

	if (pPH == NULL)
		return;

	vector<MyVMValueStruct> vecArgs;
	vector<MyVMValueStruct> vecResult;

	MyVMValueStruct tmpArg;

	//PEIndex
	tmpArg.Type = MyVMValueType_Number;
	tmpArg.Number = (double)Index;
	vecArgs.push_back(tmpArg);

	//PHIndex
	tmpArg.Type = MyVMValueType_Number;
	tmpArg.Number = (double)pPH->Index;
	vecArgs.push_back(tmpArg);

	//OBIndex
	tmpArg.Type = MyVMValueType_Number;
	tmpArg.Number = (double)pOB->Index;
	vecArgs.push_back(tmpArg);

	PublicEncounterManager::CallLuaFunc(LuaOnOBAchieve, &vecArgs, &vecResult);
	PublicEncounterManager::SC_ObjectiveAchieved(-1, Index, pPH->Index, pOB->Index);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::OnPlayerEnter(CPublicEncounterPlayer* pPlayer)
{
	if( pPlayer != NULL && Global::GetPlayerObj_ByDBID(pPlayer->GetDBID()) != NULL )
	{
		BaseItemObject* pObj = Global::GetPlayerObj_ByDBID(pPlayer->GetDBID());

		vector<MyVMValueStruct> vecArgs;
		vector<MyVMValueStruct> vecResult;

		MyVMValueStruct tmpArg;

		//PEIndex
		tmpArg.Type = MyVMValueType_Number;
		tmpArg.Number = (double)Index;
		vecArgs.push_back(tmpArg);

		//Player's GUID
		tmpArg.Type = MyVMValueType_Number;
		tmpArg.Number = (double)(pObj->GUID());
		vecArgs.push_back(tmpArg);

		PublicEncounterManager::CallLuaFunc(LuaOnPlayerEnter, &vecArgs, &vecResult);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void CPublicEncounter::OnPlayerLeave(CPublicEncounterPlayer* pPlayer)
{
	if( pPlayer != NULL && Global::GetPlayerObj_ByDBID(pPlayer->GetDBID()) != NULL )
	{
		BaseItemObject* pObj = Global::GetPlayerObj_ByDBID(pPlayer->GetDBID());
		
		vector<MyVMValueStruct> vecArgs;
		vector<MyVMValueStruct> vecResult;

		MyVMValueStruct tmpArg;

		//PEIndex
		tmpArg.Type = MyVMValueType_Number;
		tmpArg.Number = (double)Index;
		vecArgs.push_back(tmpArg);

		//Player's GUID
		tmpArg.Type = MyVMValueType_Number;
		tmpArg.Number = (double)(pObj->GUID());
		vecArgs.push_back(tmpArg);

		PublicEncounterManager::CallLuaFunc(LuaOnPlayerLeave, &vecArgs, &vecResult);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// class PublicEncounterManager
////////////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::Init()
{
	//create packet buffer
	if (m_PacketBuffer == NULL)
	{
		//4 KB buffer
		m_PacketBufferSize = 1024 * 4;
		m_PacketBuffer = NEW BYTE[ m_PacketBufferSize ]; 
	}

	LoadObjectDB();

	Global::Schedular()->Push( Process , PE_INIT_FIRST_TIME_PROC_TIME_GAP, NULL, NULL );

	Print(LV2, "PE Manager","Initialize Done.");
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::LoadObjectDB()
{
	GameObjDbStructEx* pObjDB = NULL;
	GameObjDbStructEx* pParentObjDB = NULL;

	char Buff[256];
	std::string szName = "";
	std::string szNote = "";
	int iGUID = 0;
	int iPEIndex = 0;
	int iPHIndex = 0;
	int iOBIndex = 0;

	//------------------------------------------------------------
	//load vars
	m_Variables.clear();
	for (iGUID = 855000; iGUID < 899999; ++iGUID)
	{
		pObjDB = g_ObjectData->GetObj(iGUID);

		if (pObjDB != NULL)
		{
			SetVar(pObjDB->PE.Data.VAR.VarName, 0);
		}
	}
	//------------------------------------------------------------

	//------------------------------------------------------------
	//load pe
	for (iGUID = 790000; iGUID < 791999; ++iGUID)
	{
		pObjDB = g_ObjectData->GetObj(iGUID);

		if (pObjDB != NULL)
		{
			int iZoneID = Global::Ini()->ZoneID;
			iZoneID = ( iZoneID % _DEF_ZONE_BASE_COUNT_ ) + Def_ObjectClass_Zone;

			if (iZoneID == pObjDB->PE.Data.PE.ZoneID)
			{
				sprintf( Buff , "Sys%d_name", iGUID);
				szName = Buff;
				sprintf( Buff , "Sys%d_note", iGUID);
				szNote = Buff;

				iPEIndex = iGUID;

				PE_AddPE(szName.c_str(), szNote.c_str(), iPEIndex, (pObjDB->PE.Data.PE.CoolDown * 1000));

				//flag
				PE_SetScoreVisible(iPEIndex, pObjDB->PE.Flag.PE.ScoreVisible);
				PE_SetUIVisible(iPEIndex, pObjDB->PE.Flag.PE.UIVisible);

				//region
				for (int i = 0; i < 10; ++i)
				{
					if (pObjDB->PE.Data.PE.SubZoneID[i] > 0)
					{
						PE_AddRegion(iPEIndex, pObjDB->PE.Data.PE.SubZoneID[i]);
					}
				}

				//lua event
				PE_SetCallBackOnBegin(iPEIndex, pObjDB->PE.LuaEvent.PE.OnBegin);
				PE_SetCallBackOnSuccess(iPEIndex, pObjDB->PE.LuaEvent.PE.OnSuccess);
				PE_SetCallBackOnFail(iPEIndex, pObjDB->PE.LuaEvent.PE.OnFail);
				PE_SetCallBackOnBalance(iPEIndex, pObjDB->PE.LuaEvent.PE.OnBalance);
				PE_SetCallBackOnPlayerEnter(iPEIndex, pObjDB->PE.LuaEvent.PE.OnPlayerEnter);
				PE_SetCallBackOnPlayerLeave(iPEIndex, pObjDB->PE.LuaEvent.PE.OnPlayerLeave);	
				PE_PH_SetCallBackOnBegin(iPEIndex, pObjDB->PE.LuaEvent.PE.OnPHBegin);
				PE_PH_SetCallBackOnSuccess(iPEIndex, pObjDB->PE.LuaEvent.PE.OnPHSuccess);
				PE_PH_SetCallBackOnFail(iPEIndex, pObjDB->PE.LuaEvent.PE.OnPHFail);
				PE_OB_SetCallBackOnAchieve(iPEIndex, pObjDB->PE.LuaEvent.PE.OnOBAchieve);
			}
		}
	}
	//------------------------------------------------------------

	//------------------------------------------------------------
	//load ph
	for (iGUID = 792000; iGUID < 809999; ++iGUID)
	{
		pObjDB = g_ObjectData->GetObj(iGUID);

		if (pObjDB != NULL)
		{			
			sprintf( Buff , "Sys%d_name", iGUID);
			szName = Buff;
			sprintf( Buff , "Sys%d_note", iGUID);
			szNote = Buff;

			iPEIndex = pObjDB->PE.ParentGUID;
			iPHIndex = (pObjDB->PE.Data.PH.PHIndex + 1);

			PE_AddPhase(iPEIndex, iPHIndex, szName.c_str(), szNote.c_str(), pObjDB->PE.Data.PH.Score, pObjDB->PE.Flag.PH.SLogic, pObjDB->PE.Flag.PH.FLogic, pObjDB->PE.Data.PH.NPHTimeGap);
			PE_PH_SetBonusScore(iPEIndex, iPHIndex, (float)pObjDB->PE.Data.PH.BonusScore1, (float)pObjDB->PE.Data.PH.BonusScore2);
		}
	}
	//------------------------------------------------------------

	//------------------------------------------------------------
	//load ob
	for (iGUID = 810000; iGUID < 854999; ++iGUID)
	{
		pObjDB = g_ObjectData->GetObj(iGUID);

		if (pObjDB != NULL)
		{
			GameObjDbStructEx* pPHObjDB	 = g_ObjectData->GetObj(pObjDB->PE.ParentGUID);
			GameObjDbStructEx* pNPHObjDB = g_ObjectData->GetObj(pObjDB->PE.Data.OB.NPHGUID);

			if (pPHObjDB != NULL)
			{
				iPEIndex = pPHObjDB->PE.ParentGUID;
				iPHIndex = (pPHObjDB->PE.Data.PH.PHIndex + 1);

				CPublicEncounterPhase* pPH = GetPH(iPEIndex, iPHIndex);

				if (pPH != NULL)
				{
					sprintf( Buff , "Sys%d_name", iGUID);
					szName = Buff;
					sprintf( Buff , "Sys%d_note", iGUID);
					szNote = Buff;
					
					if (pObjDB->PE.Data.OB.OBCamp == 0)
					{
						iOBIndex = (pObjDB->PE.Data.OB.OBIndex + 101);
					}
					else
					{
						iOBIndex = (pObjDB->PE.Data.OB.OBIndex + 201);
					}
					

					PE_PH_AddObjective(iPEIndex, iPHIndex, iOBIndex, szName.c_str(), szNote.c_str() ,pObjDB->PE.Data.OB.OBType, pObjDB->PE.Data.OB.VarName, pObjDB->PE.Data.OB.InitValue, pObjDB->PE.Data.OB.Threshold, (pObjDB->PE.Data.OB.OBCamp == 0));

					if (pNPHObjDB != NULL)
					{
						CPublicEncounterPhase* pNPH = GetPH(iPEIndex, (pNPHObjDB->PE.Data.PH.PHIndex + 1));
						if (pNPH != NULL)
						{
							PE_OB_SetNextPhase(iPEIndex, iPHIndex, iOBIndex, pNPH->Index);
						}
					}
				}
			}
		}
	}
	//------------------------------------------------------------
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::Release()
{
	for (int i = 0; i < (int)m_PEList.size(); ++i)
	{
		if (m_PEList[i])
		{
			delete m_PEList[i];
		}
	}
	
	m_PEList.clear();
	m_PEIndex.clear();
	m_ConnectedClient.clear();
	m_SockID_DBIDIndex.clear();
	m_Variables.clear();

	if (m_PacketBuffer)
	{
		delete m_PacketBuffer;
		m_PacketBuffer = NULL;
		m_PacketBufferSize = 0;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int PublicEncounterManager::Process(SchedularInfo* Obj, void* InputClass)
{
	for (int i = 0; i < (int)m_PEList.size(); ++i)
	{
		CPublicEncounter* pPE = m_PEList[i];

		if (pPE)
		{
			pPE->Process();
		}
	}

	Global::Schedular()->Push( Process , PE_TIME_PROC_TIME_GAP, NULL, NULL );
	return 0;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounter* PublicEncounterManager::GetPE(int iPEIndex)
{
	CPublicEncounter* Result = NULL;

	map<int, CPublicEncounter*>::iterator it = m_PEIndex.find(iPEIndex);

	if (it != m_PEIndex.end())
	{
		Result = it->second;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterPhase* PublicEncounterManager::GetPH(int iPEIndex, int iPHIndex)
{
	CPublicEncounterPhase* Result = NULL;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		Result = pPE->GetPhase(iPHIndex);
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
CPublicEncounterObjective* PublicEncounterManager::GetOB(int iPEIndex, int iPHIndex, int iOBIndex)
{
	CPublicEncounterObjective* Result = NULL;

	CPublicEncounterPhase* pPH = GetPH(iPEIndex, iPHIndex);

	if (pPH)
	{
		Result = pPH->GetObjective(iOBIndex);
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int	PublicEncounterManager::GetVar(const char* pszName)
{
	int iResult = 0;

	string _VarName = ToLower(pszName);

	map<string, int>::iterator it = m_Variables.find(_VarName.c_str());

	if (it != m_Variables.end())
	{
		iResult = it->second;
	}

	return iResult;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SetVar(const char* pszName, int value)
{
	string _VarName = ToLower(pszName);

	m_CritSectVar.Enter();
	m_Variables[_VarName.c_str()] = value;
	m_CritSectVar.Leave();
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::AddVar(const char* pszName, int value)
{
	SetVar(pszName, GetVar(pszName) + value);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::CallLuaFunc(string strFuncName, vector<MyVMValueStruct>* pVecArgs, vector<MyVMValueStruct>* pVecResult)
{
	if (strFuncName.empty())
		return;

	Print(LV2, "PE Manager","Call %s.", strFuncName.c_str());

	if (!LUA_VMClass::PCall( strFuncName.c_str() , -1 , -1 , pVecArgs, pVecResult))
	{
		Print(LV2, "PE Manager","Call Lua %s failed.", strFuncName.c_str());
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
string PublicEncounterManager::ToLower(const char* pszString)
{
	char tmpBuff[128];

	SecureZeroMemory(tmpBuff, sizeof(tmpBuff));

	strcpy(tmpBuff, pszString);

	_strlwr(tmpBuff);

	return tmpBuff;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
float PublicEncounterManager::CalculateContribution(int iHate, int iRxDmg, int iTxDmg, int iTxHeal, int iLv)
{
	float Result = 0;

	Result = (float)iHate + (float)iRxDmg + (float)iTxDmg + (float)iTxHeal;

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::CliConnect(BaseItemObject* pObj, int iSockID)
{
	if ((pObj == NULL) || (pObj->Role() == NULL))
	{
		Print(LV2, "PE", "CliConnect : Invalid BaseItemObject");
		return;
	}
	RoleDataEx* pRole = pObj->Role();

	vector<int>::iterator itSock = find(m_ConnectedClient.begin(), m_ConnectedClient.end(), iSockID);

	//new connection
	if (itSock == m_ConnectedClient.end())
	{
		m_SockID_DBIDIndex[iSockID] = pRole->DBID();
		m_ConnectedClient.push_back(iSockID);
	}

	for (int i = 0; i < (int)m_PEList.size(); ++i)
	{
		CPublicEncounter* pPE = m_PEList[i];

		if (pPE)
		{
			pPE->CliConnect(pRole->DBID());
		}
	}
	//------------------------------------------

	SC_PEConfigData(iSockID);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::CliDisconnect(int iSockID)
{
	vector<int>::iterator itSock = find(m_ConnectedClient.begin(), m_ConnectedClient.end(), iSockID);

	//existing connection
	if (itSock != m_ConnectedClient.end())
	{
		map<int, int>::iterator itDBID = m_SockID_DBIDIndex.find(iSockID);

		if (itDBID != m_SockID_DBIDIndex.end())
		{
			//------------------------------------------
			for (int i = 0; i < (int)m_PEList.size(); ++i)
			{
				CPublicEncounter* pPE = m_PEList[i];

				if (pPE)
				{
					pPE->CliDisconnect(itDBID->second);
				}
			}
			//------------------------------------------

			m_SockID_DBIDIndex.erase(itDBID);
		}

		m_ConnectedClient.erase(itSock);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::CliRegionChanged(BaseItemObject* pObj, int iRegionID)
{
	if ((pObj == NULL) || (pObj->Role() == NULL))
	{
		Print(LV2, "PE", "CliRegionChanged : Invalid BaseItemObject");
		return;
	}

	for (int i = 0; i < (int)m_PEList.size(); ++i)
	{
		CPublicEncounter* pPE = m_PEList[i];

		if (pPE)
		{
			pPE->CliRegionChanged(pObj);
		}
	}
	//------------------------------------------
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SendToAllPlayers(int iSize, PVOID pData)
{
	for (int i = 0; i < (int)m_ConnectedClient.size(); ++i)
	{
		Global::SendToCli(m_ConnectedClient[i], iSize , pData);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_PEConfigData(int iSockID)
{
	unsigned long dwPacketSize = 0;

	CPublicEncounter* pPE			   = NULL;
	CPublicEncounterPhase* pPH		   = NULL;
	CPublicEncounterObjective* pOB	   = NULL;

	//-----------------------------------------------------------------
	//count packet size
	dwPacketSize += sizeof(int); //PacketID
	dwPacketSize += sizeof(int); //public encounter number

	for (int i = 0; i < (int)m_PEList.size(); ++i)
	{
		pPE = m_PEList[i];

		dwPacketSize += sizeof(StructPEBaseData);												// public encounter base data
		dwPacketSize += sizeof(int);															// Region Number
		dwPacketSize += sizeof(int);															// Phase Number
		dwPacketSize += sizeof(StructPERegionData) * ((int)pPE->Regions.size());				// Region data size
		dwPacketSize += sizeof(StructPEPhaseBaseData)* ((int)pPE->Phases.size());;				// Phase base data

		for (int j = 0; j < (int)pPE->Phases.size(); ++j)
		{
			pPH = pPE->Phases[j];
			dwPacketSize += sizeof(int);														// Objective Number
			dwPacketSize += sizeof(StructPEObjectiveBaseData) * ((int)pPH->Objectives.size());	// Objective base data
		}
	}
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	//create and fill packet

	//realloc memory
	if (dwPacketSize > m_PacketBufferSize)
	{
		delete m_PacketBuffer;

		m_PacketBufferSize = dwPacketSize;

		m_PacketBuffer = NEW BYTE[ dwPacketSize ];
	}

	SecureZeroMemory(m_PacketBuffer, m_PacketBufferSize);

	PBYTE ptr = (PBYTE)m_PacketBuffer;

	//PacketID
	*((int*)ptr)	= EM_PG_PE_LtoC_PEConfigData;
	ptr		       += sizeof(int);

	//count PE number
	int iPENum = (int)m_PEList.size();

	*((int*)ptr) = iPENum;
	ptr			+= sizeof(int);

	for (int i = 0; i < (int)m_PEList.size(); ++i)
	{
		pPE = m_PEList[i];

		// write PE base data
		StructPEBaseData* pPEBaseDest = (StructPEBaseData*)ptr;
		StructPEBaseData* pPEBaseSrc  = pPE->BaseData;

		*pPEBaseDest  = *pPEBaseSrc;
		ptr			 += sizeof(StructPEBaseData);

		// write region number
		int iRGNum  = (int)pPE->Regions.size();

		*((int*)ptr) = iRGNum;
		ptr			+= sizeof(int);

		// write phase number
		int iPHNum = (int)pPE->Phases.size();

		*((int*)ptr) = iPHNum;
		ptr			+= sizeof(int);

		//write Region data
		for (int j = 0; j < iRGNum; ++j)
		{
			StructPERegionData* pRGDataDest = (StructPERegionData*)ptr;
			StructPERegionData* pRGDataSrc  = &(pPE->Regions[j]);

			*pRGDataDest = *pRGDataSrc;
			ptr			+= sizeof(StructPERegionData);
		}

		//write phase data
		for (int j = 0; j < iPHNum; ++j)
		{
			pPH	= pPE->Phases[j];

			//Phase base data
			StructPEPhaseBaseData* pPHBaseDest = (StructPEPhaseBaseData*)ptr;
			StructPEPhaseBaseData* pPHBaseSrc  = pPH->BaseData;

			*pPHBaseDest = *pPHBaseSrc;
			ptr			+= sizeof(StructPEPhaseBaseData);

			// Objective number
			int iOBNum = (int)pPH->Objectives.size();

			*((int*)ptr) = iOBNum;
			ptr			+= sizeof(int);

			//write Objective Data
			for (int k = 0; k < iOBNum; ++k)
			{
				pOB = pPH->Objectives[k];

				//Objective base data
				StructPEObjectiveBaseData* pOBBaseDest = (StructPEObjectiveBaseData*)ptr;
				StructPEObjectiveBaseData* pOBBaseSrc  = pOB->BaseData;

				*pOBBaseDest = *pOBBaseSrc;
				ptr			+= sizeof(StructPEObjectiveBaseData);
			}	
		}	
	}
	//-----------------------------------------------------------------

	if (iSockID >= 0)
	{
		Global::SendToCli(iSockID, dwPacketSize , m_PacketBuffer);
	}
	else
	{
		SendToAllPlayers(dwPacketSize , m_PacketBuffer);
	}	
}
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_PEStatus(int iDBID, int iPEIndex, int iStatus)
{
	PG_PE_LtoC_PEStatus Send;
	Send.iPEIndex = iPEIndex;
	Send.iStatus  = iStatus;

	if (iDBID >= 0)
	{
		Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
	}
	else
	{
		SendToAllPlayers(sizeof(Send) , &Send);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_PhaseStatus(int iDBID, int iPEIndex, int iPHIndex, int iStatus)
{
	PG_PE_LtoC_PhaseStatus Send;
	Send.iPEIndex = iPEIndex;
	Send.iPHIndex = iPHIndex;
	Send.iStatus  = iStatus;

	if (iDBID >= 0)
	{
		Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
	}
	else
	{
		SendToAllPlayers(sizeof(Send) , &Send);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_ObjectiveStatus(int iDBID, int iPEIndex, int iPHIndex, int iOBIndex, int iStatus)
{
	PG_PE_LtoC_ObjectiveStatus Send;
	Send.iPEIndex = iPEIndex;
	Send.iPHIndex = iPHIndex;
	Send.iOBIndex = iOBIndex;
	Send.iStatus  = iStatus;

	if (iDBID >= 0)
	{
		Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
	}
	else
	{
		SendToAllPlayers(sizeof(Send) , &Send);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_PlayerScore(int iDBID, int iPEIndex, int iScore)
{
	PG_PE_LtoC_PlayerScore Send;
	Send.iPEIndex = iPEIndex;
	Send.iScore   = iScore;
	Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_PEActivated(int iDBID, int iPEIndex, int iActivePHIndex)
{	
	PG_PE_LtoC_PEActivated Send;
	Send.iPEIndex       = iPEIndex;
	Send.iActivePHIndex = iActivePHIndex;

	Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_PEDeactivated(int iDBID, int iPEIndex)
{

	PG_PE_LtoC_PEDeactivated Send;
	Send.iPEIndex = iPEIndex;

	Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_PEActivePhaseChange(int iDBID, int iPEIndex, int iActivePHIndex)
{
	PG_PE_LtoC_PEActivePhaseChange Send;
	Send.iPEIndex		= iPEIndex;
	Send.iActivePHIndex = iActivePHIndex;

	if (iDBID >= 0)
	{		
		Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
	}
	else
	{
		SendToAllPlayers(sizeof(Send), &Send);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_ObjectiveValueChage(int iDBID, int iPEIndex, int iPHIndex, int iOBIndex, int value)
{
	PG_PE_LtoC_ObjectiveValueChage Send;
	Send.iPEIndex	= iPEIndex;
	Send.iPHIndex	= iPHIndex;
	Send.iOBIndex	= iOBIndex;
	Send.iNowValue	= value;

	if (iDBID >= 0)
	{		
		Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
	}
	else
	{
		SendToAllPlayers(sizeof(Send), &Send);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_PESuccess(int iDBID, int iPEIndex)
{
	PG_PE_LtoC_PESuccess Send;
	Send.iPEIndex	= iPEIndex;

	if (iDBID >= 0)
	{		
		Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
	}
	else
	{
		SendToAllPlayers(sizeof(Send), &Send);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_PEFailed(int iDBID, int iPEIndex)
{
	PG_PE_LtoC_PEFailed Send;
	Send.iPEIndex	= iPEIndex;

	if (iDBID >= 0)
	{		
		Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
	}
	else
	{
		SendToAllPlayers(sizeof(Send), &Send);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_PhaseSuccess(int iDBID, int iPEIndex, int iPHIndex)
{
	PG_PE_LtoC_PhaseSuccess Send;
	Send.iPEIndex	= iPEIndex;
	Send.iPHIndex	= iPHIndex;

	if (iDBID >= 0)
	{		
		Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
	}
	else
	{
		SendToAllPlayers(sizeof(Send), &Send);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_PhaseFailed(int iDBID, int iPEIndex, int iPHIndex)
{
	PG_PE_LtoC_PhaseFailed Send;
	Send.iPEIndex	= iPEIndex;
	Send.iPHIndex	= iPHIndex;

	if (iDBID >= 0)
	{		
		Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
	}
	else
	{
		SendToAllPlayers(sizeof(Send), &Send);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_ObjectiveAchieved(int iDBID, int iPEIndex, int iPHIndex, int iOBIndex)
{
	PG_PE_LtoC_ObjectiveAchieved Send;
	Send.iPEIndex	= iPEIndex;
	Send.iPHIndex	= iPHIndex;
	Send.iOBIndex	= iOBIndex;

	if (iDBID >= 0)
	{		
		Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
	}
	else
	{
		SendToAllPlayers(sizeof(Send), &Send);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_ObjectiveFlag(int iDBID, int iPEIndex, int iPHIndex, int iOBIndex, int iFlag)
{
	PG_PE_LtoC_ObjectiveFlag Send;
	Send.iPEIndex	= iPEIndex;
	Send.iPHIndex	= iPHIndex;
	Send.iOBIndex	= iOBIndex;
	Send.iFlag		= iFlag;

	if (iDBID >= 0)
	{		
		Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
	}
	else
	{
		SendToAllPlayers(sizeof(Send), &Send);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::SC_PEFlag(int iDBID, int iPEIndex, int iFlag)
{
	PG_PE_LtoC_PEFlag Send;
	Send.iPEIndex	= iPEIndex;
	Send.iFlag		= iFlag;

	if (iDBID >= 0)
	{		
		Global::SendToCli_ByDBID(iDBID, sizeof(Send) , &Send);
	}
	else
	{
		SendToAllPlayers(sizeof(Send), &Send);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_Start(int iPEIndex, int iPHIndex)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->Start(iPHIndex);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_AddPE(const char* pszPEName, const char* pszDesc, int iIndex, int iTimeGap)
{
	CPublicEncounter* pPE = GetPE(iIndex);

	if (pPE == NULL)
	{
		pPE = new CPublicEncounter(pszPEName);
		m_PEList.push_back(pPE);
	}

	pPE->Description = pszDesc;
	pPE->Index = iIndex;
	pPE->ResetTimeGap = iTimeGap;	

	m_PEIndex[iIndex] = pPE;

	Print(LV2, "PE Manager","public encounter %s is added.", pszPEName);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_AddRegion(int iPEIndex, int iRegionID)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->AddRegion(iRegionID);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_AddPhase(int iPEIndex, int iIndex, const char* pszName, const char* pszDesc, int iScore, int iSuccLogic, int iFailLogic, int iTimeGap)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->AddPhase(pszName, pszDesc, iIndex, (float)iScore, iSuccLogic, iFailLogic, iTimeGap);

		Print(LV2, "PE Manager","Add a new phase %s to PE %s.", pPE->Name, pszName);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_JumpToPhase(int iPEIndex, int iPHIndex)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);
	if (pPE)
	{
		pPE->JumpToPhase(iPHIndex);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_SetCallBackOnBegin(int iPEIndex, const char* pszLuaFunc)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);
	if (pPE)
	{
		pPE->LuaOnBegin = pszLuaFunc;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_SetCallBackOnSuccess(int iPEIndex, const char* pszLuaFunc)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);
	if (pPE)
	{
		pPE->LuaOnSuccess = pszLuaFunc;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_SetCallBackOnFail(int iPEIndex, const char* pszLuaFunc)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);
	if (pPE)
	{
		pPE->LuaOnFail = pszLuaFunc;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_SetCallBackOnBalance(int iPEIndex, const char* pszLuaFunc)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);
	if (pPE)
	{
		pPE->LuaOnBalance = pszLuaFunc;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_SetCallBackOnPlayerEnter(int iPEIndex, const char* pszLuaFunc)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);
	if (pPE)
	{
		pPE->LuaOnPlayerEnter = pszLuaFunc;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_SetCallBackOnPlayerLeave(int iPEIndex, const char* pszLuaFunc)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);
	if (pPE)
	{
		pPE->LuaOnPlayerLeave = pszLuaFunc;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_SetPhaseBonusScore(int iPEIndex, float fBonus1, float fBonus2)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);
	if (pPE)
	{
		pPE->SetPhaseBonusScore(fBonus1, fBonus2);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_GivePlayerScore(int iPEIndex, int iGUID, float fScore)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->GivePlayerScore(iGUID, fScore);
	}
	else
	{
		Print(LV2, "PE", "PE(Index = %d) not find", iPEIndex);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_GivePartyScore(int iPEIndex, int iPartyID, float fScore)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->GivePartyScore(iPartyID, fScore);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_GiveAllPlayersScore(int iPEIndex, float fScore)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->GiveAllPlayersScore(fScore);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
float PublicEncounterManager::PE_GetPlayerScore(int iPEIndex, int iGUID)
{
	float Result = 0;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		Result = pPE->GetPlayerScore(iGUID);
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_GetPartyScore(int iPEIndex, int iPartyID, const char* pszLuaCallback)
{
	if (strlen(pszLuaCallback) == 0)
		return;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		//------------------------------------------
		//Get all player's score
		vector<int> vecGUID;
		vector<float> vecScore;

		pPE->GetPartyScore(iPartyID, vecGUID, vecScore);
		//------------------------------------------

		int iNum = (int)vecGUID.size();

		vector<MyVMValueStruct> vecArgs;
		vector<MyVMValueStruct> vecResult;

		MyVMValueStruct tmpArg;

		for (int i = 0; i < iNum; ++i)
		{
			vecArgs.clear();
			vecResult.clear();

			//PEIndex
			tmpArg.Type = MyVMValueType_Number;
			tmpArg.Number = (double)pPE->Index;
			vecArgs.push_back(tmpArg);

			//Party ID
			tmpArg.Type = MyVMValueType_Number;
			tmpArg.Number = (double)iPartyID;
			vecArgs.push_back(tmpArg);

			//Players Number
			tmpArg.Type = MyVMValueType_Number;
			tmpArg.Number = (double)iNum;
			vecArgs.push_back(tmpArg);


			//Player's GUID
			tmpArg.Type = MyVMValueType_Number;
			tmpArg.Number = (double)(vecGUID[i]);
			vecArgs.push_back(tmpArg);

			//Player's score
			tmpArg.Type = MyVMValueType_Number;
			tmpArg.Number = (double)(vecScore[i]);
			vecArgs.push_back(tmpArg);

			PublicEncounterManager::CallLuaFunc(pszLuaCallback, &vecArgs, &vecResult);
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_GetAllPlayersScore(int iPEIndex, const char* pszLuaCallback)
{
	PE_GetTopNPlayersScore(iPEIndex, 0, pszLuaCallback);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_GetTopNPlayersScore(int iPEIndex, int iTopN, const char* pszLuaCallback)
{
	if (strlen(pszLuaCallback) == 0)
		return;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		//------------------------------------------
		//Get all player's score
		vector<int> vecGUID;
		vector<float> vecScore;

		pPE->GetTopNPlayersScore(vecGUID, vecScore, iTopN);
		//------------------------------------------

		vector<MyVMValueStruct> vecArgs;
		vector<MyVMValueStruct> vecResult;

		MyVMValueStruct tmpArg;

		for (int i = 0; i < (int)vecGUID.size(); ++i)
		{
			vecArgs.clear();
			vecResult.clear();

			//PEIndex
			tmpArg.Type = MyVMValueType_Number;
			tmpArg.Number = (double)pPE->Index;
			vecArgs.push_back(tmpArg);

			//Player's GUID
			tmpArg.Type = MyVMValueType_Number;
			tmpArg.Number = (double)(vecGUID[i]);
			vecArgs.push_back(tmpArg);

			//Player's score
			tmpArg.Type = MyVMValueType_Number;
			tmpArg.Number = (double)(vecScore[i]);
			vecArgs.push_back(tmpArg);

			PublicEncounterManager::CallLuaFunc(pszLuaCallback, &vecArgs, &vecResult);
		}		
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_SetPlayerScore(int iPEIndex, int iGUID, float fScore)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->SetPlayerScore(iGUID, fScore);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_CountHatePoint(int iPEIndex, int iGUID)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->CountHatePoint(iGUID);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int	PublicEncounterManager::PE_GetActivePhaseIndex(int iPEIndex)
{
	int Result = -1;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		if (pPE->ActivePhase)
		{
			Result = pPE->ActivePhase->Index;
		}
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int PublicEncounterManager::PE_GetActivePlayerGUIDCount(int iPEIndex)
{
	int Result = 0;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		Result = pPE->GetActivePlayerGUIDCount();
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int PublicEncounterManager::PE_GetActivePlayerGUID(int iPEIndex)
{
	int Result = -1;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		Result = pPE->GetActivePlayerGUID();
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_SetScoreVisible(int iPEIndex, bool bVisible)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->ScoreVisible = bVisible;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_SetUIVisible(int iPEIndex, bool bVisible)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->UIVisible = bVisible;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_RefreshConfig()
{
	SC_PEConfigData(-1);

	for (int i = 0; i < (int)m_PEList.size(); ++i)
	{
		CPublicEncounter* pPE = m_PEList[i];

		if (pPE != NULL)
		{
			pPE->OnRefreshConfig();
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_PH_AddObjective(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, int iType, const char* pszVarName, int iInitValue, int iDoorsill, bool bSucc)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		CPublicEncounterPhase* pPH = pPE->GetPhase(iPHIndex);

		if (pPH)
		{
			CPublicEncounterObjective* pOB = NULL;

			if (bSucc)
			{
				pOB = pPH->AddObjective(pszName, pszDesc, iIndex, iType, pszVarName, iInitValue, iDoorsill, EM_OBJ_KIND_SUCCESSFUL);
			}
			else
			{
				pOB = pPH->AddObjective(pszName, pszDesc, iIndex, iType, pszVarName, iInitValue, iDoorsill, EM_OBJ_KIND_FAIL);
			}
		}
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_PH_SetCallBackOnBegin(int iPEIndex, const char* pszLuaFunc)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->LuaOnPHBegin = pszLuaFunc;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_PH_SetCallBackOnSuccess(int iPEIndex, const char* pszLuaFunc)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->LuaOnPHSuccess = pszLuaFunc;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_PH_SetCallBackOnFail(int iPEIndex, const char* pszLuaFunc)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->LuaOnPHFail = pszLuaFunc;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_PH_SetBonusScore(int iPEIndex, int iPHIndex, float fBonus1, float fBonus2)
{
	CPublicEncounterPhase* pPH = GetPH(iPEIndex, iPHIndex);

	if (pPH)
	{
		pPH->BonusScore1 = fBonus1;
		pPH->BonusScore2 = fBonus2;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_PH_SetNextPhase(int iPEIndex, int iPHIndex, int iTgtPHIndex)
{
	CPublicEncounterPhase* pPH = GetPH(iPEIndex, iPHIndex);
	CPublicEncounterPhase* pTgtPH = GetPH(iPEIndex, iTgtPHIndex);

	if (pPH)
	{
		pPH->SetAllSuccObjectiveNextPhase(pTgtPH);
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_OB_SetCallBackOnAchieve(int iPEIndex, const char* pszLuaFunc)
{
	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->LuaOnOBAchieve = pszLuaFunc;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_OB_SetNextPhase(int iPEIndex, int iPHIndex, int iOBIndex, int iTgtPHIndex)
{
	 CPublicEncounterObjective* pOB = GetOB(iPEIndex, iPHIndex, iOBIndex);
	 CPublicEncounterPhase* pPH = GetPH(iPEIndex, iTgtPHIndex);

	 if (pOB)
	 {
		 pOB->NextPhase = pPH;
	 }
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_OB_SetVisible(int iPEIndex, int iPHIndex, int iOBIndex, bool bVisible)
{
	CPublicEncounterObjective* pOB = GetOB(iPEIndex, iPHIndex, iOBIndex);

	if (pOB)
	{
		pOB->Visible = bVisible;
	}
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
int PublicEncounterManager::PE_VAR_GetVar(const char* pszVarName)
{
	return GetVar(pszVarName);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_VAR_SetVar(const char* pszVarName, int value)
{
	SetVar(pszVarName, value);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void PublicEncounterManager::PE_VAR_AddVar(const char* pszVarName, int value)
{
	AddVar(pszVarName, value);
}


//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool PublicEncounterManager::_ReleasePE(int iPEIndex)
{
	bool Result = false;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		vector<CPublicEncounter*>::iterator itList = find(m_PEList.begin(), m_PEList.end(), pPE);

		if (itList != m_PEList.end())
		{
			m_PEList.erase(itList);
		}

		map<int, CPublicEncounter*>::iterator itIndex = m_PEIndex.find(iPEIndex);

		if (itIndex != m_PEIndex.end())
		{
			m_PEIndex.erase(itIndex);
		}

		delete pPE;

		Result = true;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool PublicEncounterManager::_GetPEStatus(int iPEIndex, vector<string>& vecString)
{
	bool Result = false;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->_GetStatusString(vecString);
		Result = true;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool PublicEncounterManager::_GetPEDetail(int iPEIndex, vector<string>& vecString)
{
	bool Result = false;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->_GetDetailString(vecString);
		Result = true;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool PublicEncounterManager::_ResetPE(int iPEIndex)
{
	bool Result = false;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->Reset();
		Result = true;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool PublicEncounterManager::_ListAllVar(vector<string>& vecNames, vector<int>& vecValues)
{
	vecNames.clear();
	vecValues.clear();

	map<string, int>::iterator it;

	for (it = m_Variables.begin(); it != m_Variables.end(); it++)
	{
		vecNames.push_back(it->first.c_str());
		vecValues.push_back(it->second);
	}

	return true;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool PublicEncounterManager::_PEInstSucc(int iPEIndex)
{
	bool Result = false;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->Status = EM_PE_STATUS_SUCCESS;
		Result = true;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool PublicEncounterManager::_PEInstFail(int iPEIndex)
{
	bool Result = false;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->Status = EM_PE_STATUS_FAILED;
		Result = true;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool PublicEncounterManager::_JumpToPhase(int iPEIndex, int iPHIndex)
{
	bool Result = false;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->JumpToPhase(iPHIndex);
		Result = true;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool PublicEncounterManager::_PHInstSucc(int iPEIndex, int iPHIndex)
{
	bool Result = false;

	CPublicEncounterPhase* pPH = GetPH(iPEIndex, iPHIndex);

	if (pPH)
	{
		pPH->Status = EM_PHASE_STATUS_SUCCESS;
		Result = true;
	}
	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool PublicEncounterManager::_PHInstFail(int iPEIndex, int iPHIndex)
{
	bool Result = false;

	CPublicEncounterPhase* pPH = GetPH(iPEIndex, iPHIndex);

	if (pPH)
	{
		pPH->Status = EM_PHASE_STATUS_FAILED;
		Result = true;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool PublicEncounterManager::_OBInstAchieve(int iPEIndex, int iPHIndex, int iOBIndex)
{
	bool Result = false;

	CPublicEncounterObjective* pOB = GetOB(iPEIndex, iPHIndex, iOBIndex);

	if (pOB)
	{
		pOB->Status = EM_OBJ_STATUS_ACHIEVED;
		Result = true;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
bool PublicEncounterManager::_GiveScore(int iPEIndex, int iGUID, float fScore)
{
	bool Result = false;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		pPE->GivePlayerScore(iGUID, fScore);
		Result = true;
	}

	return Result;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
float PublicEncounterManager::_GetScore(int iPEIndex, int iGUID)
{
	float Result = 0;

	CPublicEncounter* pPE = GetPE(iPEIndex);

	if (pPE)
	{
		Result = pPE->GetPlayerScore(iGUID);
	}

	return Result;
}