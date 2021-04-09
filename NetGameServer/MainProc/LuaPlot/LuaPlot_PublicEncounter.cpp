#include "LuaPlot_PublicEncounter.h"
#include "../../NetWalker_Member/Net_PublicEncounter/NetSrv_PublicEncounter.h"

namespace  LuaPlotClass
{
	//---------------------------------------------------------------
	void PE_Start(int iPEIndex, int iPHIndex)
	{
		PublicEncounterManager::PE_Start(iPEIndex, iPHIndex);
	}

	//---------------------------------------------------------------
	void PE_AddPE(const char* pszName, const char* pszDesc, int iIndex, int iTimeGap)
	{
		PublicEncounterManager::PE_AddPE(pszName, pszDesc, iIndex, iTimeGap);
	}

	//---------------------------------------------------------------
	void PE_AddRegion(int iPEIndex, int iRegionID)
	{
		PublicEncounterManager::PE_AddRegion(iPEIndex, iRegionID);
	}

	//---------------------------------------------------------------
	void PE_AddPhase(int iPEIndex, int iIndex, const char* pszName, const char* pszDesc, int iScore, int iSuccLogic, int iFailLogic, int iTimeGap)
	{
		PublicEncounterManager::PE_AddPhase(iPEIndex, iIndex, pszName, pszDesc, iScore, iSuccLogic, iFailLogic, iTimeGap);
	}

	//---------------------------------------------------------------
	void PE_JumpToPhase(int iPEIndex, int iPHIndex)
	{
		PublicEncounterManager::PE_JumpToPhase(iPEIndex, iPHIndex);
	}

	//---------------------------------------------------------------
	void PE_SetCallBackOnSuccess(int iPEIndex, const char* pszLuaFunc)
	{
		PublicEncounterManager::PE_SetCallBackOnSuccess(iPEIndex, pszLuaFunc);
	}

	//---------------------------------------------------------------
	void PE_SetCallBackOnFail(int iPEIndex, const char* pszLuaFunc)
	{
		PublicEncounterManager::PE_SetCallBackOnFail(iPEIndex, pszLuaFunc);
	}

	//---------------------------------------------------------------
	void PE_SetCallBackOnBalance(int iPEIndex, const char* pszLuaFunc)
	{
		PublicEncounterManager::PE_SetCallBackOnBalance(iPEIndex, pszLuaFunc);
	}

	//---------------------------------------------------------------
	void PE_SetCallBackOnPlayerEnter(int iPEIndex, const char* pszLuaFunc)
	{
		PublicEncounterManager::PE_SetCallBackOnPlayerEnter(iPEIndex, pszLuaFunc);
	}

	//---------------------------------------------------------------
	void PE_SetCallBackOnPlayerLeave(int iPEIndex, const char* pszLuaFunc)
	{
		PublicEncounterManager::PE_SetCallBackOnPlayerLeave(iPEIndex, pszLuaFunc);
	}

	//---------------------------------------------------------------
	void PE_SetPhaseBonusScore(int iPEIndex, float fBonus1, float fBonus2)
	{
		PublicEncounterManager::PE_SetPhaseBonusScore(iPEIndex, fBonus1, fBonus2);
	}

	//---------------------------------------------------------------
	void PE_GivePlayerScore(int iPEIndex, int iGUID, float fScore)
	{
		PublicEncounterManager::PE_GivePlayerScore(iPEIndex, iGUID, fScore);
	}
	//---------------------------------------------------------------
	void PE_GivePartyScore(int iPEIndex, int iPartyID, float fScore)
	{
		PublicEncounterManager::PE_GivePartyScore(iPEIndex, iPartyID, fScore);
	}

	//---------------------------------------------------------------
	void PE_GiveAllPlayersScore(int iPEIndex, float fScore)
	{
		PublicEncounterManager::PE_GiveAllPlayersScore(iPEIndex, fScore);
	}

	//---------------------------------------------------------------
	float PE_GetPlayerScore(int iPEIndex, int iGUID)
	{
		return PublicEncounterManager::PE_GetPlayerScore(iPEIndex, iGUID);
	}

	//---------------------------------------------------------------
	void PE_GetPartyScore(int iPEIndex, int iPartyID, const char* pszLuaCallback)
	{
		PublicEncounterManager::PE_GetPartyScore(iPEIndex, iPartyID, pszLuaCallback);
	}

	//---------------------------------------------------------------
	void PE_GetAllPlayersScore(int iPEIndex, const char* pszLuaCallback)
	{
		PublicEncounterManager::PE_GetAllPlayersScore(iPEIndex, pszLuaCallback);
	}

	//---------------------------------------------------------------
	void PE_GetTopNPlayersScore(int iPEIndex, int iTopN, const char* pszLuaCallback)
	{
		PublicEncounterManager::PE_GetTopNPlayersScore(iPEIndex, iTopN, pszLuaCallback);
	}

	//---------------------------------------------------------------
	void PE_SetPlayerScore(int iPEIndex, int iGUID, float fScore)
	{
		PublicEncounterManager::PE_SetPlayerScore(iPEIndex, iGUID, fScore);
	}

	//---------------------------------------------------------------
	void PE_CountHatePoint(int iPEIndex, int iGUID)
	{
		PublicEncounterManager::PE_CountHatePoint(iPEIndex, iGUID);
	}

	//---------------------------------------------------------------
	int	PE_GetActivePhaseIndex(int iPEIndex)
	{
		return PublicEncounterManager::PE_GetActivePhaseIndex(iPEIndex);
	}

	//---------------------------------------------------------------
	int PE_GetActivePlayerGUIDCount(int iPEIndex)
	{
		return PublicEncounterManager::PE_GetActivePlayerGUIDCount(iPEIndex);
	}

	//---------------------------------------------------------------
	int PE_GetActivePlayerGUID(int iPEIndex)
	{
		return PublicEncounterManager::PE_GetActivePlayerGUID(iPEIndex);
	}

	//---------------------------------------------------------------
	void PE_SetScoreVisible(int iPEIndex, bool bVisible)
	{
		PublicEncounterManager::PE_SetScoreVisible(iPEIndex, bVisible);
	}

	//---------------------------------------------------------------
	void PE_SetUIVisible(int iPEIndex, bool bVisible)
	{
		PublicEncounterManager::PE_SetUIVisible(iPEIndex, bVisible);
	}

	//---------------------------------------------------------------
	void PE_RefreshConfig()
	{
		PublicEncounterManager::PE_RefreshConfig();
	}

	//---------------------------------------------------------------
	void PE_PH_AddObjective_Greater(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, const char* pszVarName, int iIniValue, int iDoorsill, bool bSucTerm)
	{
		PublicEncounterManager::PE_PH_AddObjective(iPEIndex, iPHIndex, iIndex, pszName, pszDesc, EM_OBJ_TYPE_GREATER, pszVarName, iIniValue, iDoorsill, bSucTerm);
	}

	//---------------------------------------------------------------
	void PE_PH_AddObjective_Smaller(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, const char* pszVarName, int iIniValue, int iDoorsill, bool bSucTerm)
	{
		PublicEncounterManager::PE_PH_AddObjective(iPEIndex, iPHIndex, iIndex, pszName, pszDesc, EM_OBJ_TYPE_SMALLER, pszVarName, iIniValue, iDoorsill, bSucTerm);
	}

	//---------------------------------------------------------------
	void PE_PH_AddObjective_Equal(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, const char* pszVarName, int iIniValue, int iDoorsill, bool bSucTerm)
	{
		PublicEncounterManager::PE_PH_AddObjective(iPEIndex, iPHIndex, iIndex, pszName, pszDesc, EM_OBJ_TYPE_EQUAL, pszVarName, iIniValue, iDoorsill, bSucTerm);
	}

	//---------------------------------------------------------------
	void PE_PH_AddObjective_NotEqual(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, const char* pszVarName, int iIniValue, int iDoorsill, bool bSucTerm)
	{
		PublicEncounterManager::PE_PH_AddObjective(iPEIndex, iPHIndex, iIndex, pszName, pszDesc, EM_OBJ_TYPE_NOT_EQUAL, pszVarName, iIniValue, iDoorsill, bSucTerm);
	}

	//---------------------------------------------------------------
	void PE_PH_AddObjective_EqualAndGreater(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, const char* pszVarName, int iIniValue, int iDoorsill, bool bSucTerm)
	{
		PublicEncounterManager::PE_PH_AddObjective(iPEIndex, iPHIndex, iIndex, pszName, pszDesc, EM_OBJ_TYPE_EQUAL_AND_GREATER, pszVarName, iIniValue, iDoorsill, bSucTerm);
	}

	//---------------------------------------------------------------
	void PE_PH_AddObjective_EqualAndSmaller(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, const char* pszVarName, int iIniValue, int iDoorsill, bool bSucTerm)
	{
		PublicEncounterManager::PE_PH_AddObjective(iPEIndex, iPHIndex, iIndex, pszName, pszDesc, EM_OBJ_TYPE_EQUAL_AND_SMALLER, pszVarName, iIniValue, iDoorsill, bSucTerm);
	}

	//---------------------------------------------------------------
	void PE_PH_AddObjective_TimeLimit(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, const char* pszVarName, int iIniValue, int iDoorsill, bool bSucTerm)
	{
		PublicEncounterManager::PE_PH_AddObjective(iPEIndex, iPHIndex, iIndex, pszName, pszDesc, EM_OBJ_TYPE_TIME_LIMIT, pszVarName, iIniValue, iDoorsill, bSucTerm);
	}

	//---------------------------------------------------------------
	void PE_PH_SetCallBackOnSuccess(int iPEIndex, const char* pszLuaFunc)
	{
		PublicEncounterManager::PE_PH_SetCallBackOnSuccess(iPEIndex, pszLuaFunc);
	}

	//---------------------------------------------------------------
	void PE_PH_SetCallBackOnFail(int iPEIndex, const char* pszLuaFunc)
	{
		PublicEncounterManager::PE_PH_SetCallBackOnFail(iPEIndex, pszLuaFunc);
	}

	//---------------------------------------------------------------
	void PE_PH_SetBonusScore(int iPEIndex, int iPHIndex, float fBonus1, float fBonus2)
	{
		PublicEncounterManager::PE_PH_SetBonusScore(iPEIndex, iPHIndex, fBonus1, fBonus2);
	}

	//---------------------------------------------------------------
	void PE_PH_SetNextPhase(int iPEIndex, int iPHIndex, int iTgtPHIndex)
	{
		PublicEncounterManager::PE_PH_SetNextPhase(iPEIndex, iPHIndex, iTgtPHIndex);
	}

	//---------------------------------------------------------------
	void PE_OB_SetCallBackOnAchieve(int iPEIndex, const char* pszLuaFunc)
	{
		PublicEncounterManager::PE_OB_SetCallBackOnAchieve(iPEIndex, pszLuaFunc);
	}

	//---------------------------------------------------------------
	void PE_OB_SetNextPhase(int iPEIndex, int iPHIndex, int iOBIndex, int iTgtPHIndex)
	{
		PublicEncounterManager::PE_OB_SetNextPhase(iPEIndex, iPHIndex, iOBIndex, iTgtPHIndex);
	}

	//---------------------------------------------------------------
	void PE_OB_SetVisible(int iPEIndex, int iPHIndex, int iOBIndex, bool bVisible)
	{
		PublicEncounterManager::PE_OB_SetVisible(iPEIndex, iPHIndex, iOBIndex, bVisible);
	}

	//---------------------------------------------------------------
	int PE_VAR_GetVar(const char* pszVarName)
	{
		return PublicEncounterManager::PE_VAR_GetVar(pszVarName);
	}

	//---------------------------------------------------------------
	void PE_VAR_SetVar(const char* pszVarName, int iValue)
	{
		PublicEncounterManager::PE_VAR_SetVar(pszVarName, iValue);
	}

	//---------------------------------------------------------------
	void PE_VAR_AddVar(const char* pszVarName, int iValue)
	{
		PublicEncounterManager::PE_VAR_AddVar(pszVarName, iValue);
	}

}
