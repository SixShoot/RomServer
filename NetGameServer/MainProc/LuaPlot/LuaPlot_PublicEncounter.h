#pragma		once
#pragma		warning (disable:4786)

#include "..\Global.h"

using namespace std;

//------------------------------------------------------------------------------------
namespace  LuaPlotClass
{
	void			PE_Start							(int iPEIndex, int iPHIndex);
	void			PE_AddPE							(const char* pszName, const char* pszDesc, int iIndex, int iTimeGap);
	void			PE_AddRegion						(int iPEIndex, int iRegionID);	
	void			PE_AddPhase							(int iPEIndex, int iIndex, const char* pszName, const char* pszDesc, int iScore, int iSuccLogic, int iFailLogic, int iTimeGap);
	void			PE_JumpToPhase						(int iPEIndex, int iPHIndex);
	void			PE_SetCallBackOnSuccess				(int iPEIndex, const char* pszLuaFunc);
	void			PE_SetCallBackOnFail				(int iPEIndex, const char* pszLuaFunc);
	void			PE_SetCallBackOnBalance				(int iPEIndex, const char* pszLuaFunc);
	void			PE_SetCallBackOnPlayerEnter			(int iPEIndex, const char* pszLuaFunc);
	void			PE_SetCallBackOnPlayerLeave			(int iPEIndex, const char* pszLuaFunc);
	void			PE_SetPhaseBonusScore				(int iPEIndex, float fBonus1, float fBonus2);
	void			PE_GivePlayerScore					(int iPEIndex, int iGUID, float fScore);
	void			PE_GivePartyScore					(int iPEIndex, int iPartyID, float fScore);
	void			PE_GiveAllPlayersScore				(int iPEIndex, float fScore);
	float			PE_GetPlayerScore					(int iPEIndex, int iGUID);
	void			PE_GetPartyScore					(int iPEIndex, int iPartyID, const char* pszLuaCallback);
	void			PE_GetAllPlayersScore				(int iPEIndex, const char* pszLuaCallback);
	void			PE_GetTopNPlayersScore				(int iPEIndex, int iTopN, const char* pszLuaCallback);
	void			PE_SetPlayerScore					(int iPEIndex, int iGUID, float fScore);
	void			PE_CountHatePoint					(int iPEIndex, int iGUID);
	int				PE_GetActivePhaseIndex				(int iPEIndex);
	int				PE_GetActivePlayerGUIDCount			(int iPEIndex);
	int				PE_GetActivePlayerGUID				(int iPEIndex);
	void			PE_SetScoreVisible					(int iPEIndex, bool bVisible);
	void			PE_SetUIVisible						(int iPEIndex, bool bVisible);
	void			PE_RefreshConfig					();

	void			PE_PH_AddObjective_Greater			(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, const char* pszVarName, int iIniValue, int iDoorsill, bool bSucTerm);
	void			PE_PH_AddObjective_Smaller			(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, const char* pszVarName, int iIniValue, int iDoorsill, bool bSucTerm);
	void			PE_PH_AddObjective_Equal			(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, const char* pszVarName, int iIniValue, int iDoorsill, bool bSucTerm);
	void			PE_PH_AddObjective_NotEqual 		(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, const char* pszVarName, int iIniValue, int iDoorsill, bool bSucTerm);
	void			PE_PH_AddObjective_EqualAndGreater	(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, const char* pszVarName, int iIniValue, int iDoorsill, bool bSucTerm);
	void			PE_PH_AddObjective_EqualAndSmaller 	(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, const char* pszVarName, int iIniValue, int iDoorsill, bool bSucTerm);
	void			PE_PH_AddObjective_TimeLimit 		(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, const char* pszVarName, int iIniValue, int iDoorsill, bool bSucTerm);
	void			PE_PH_SetCallBackOnSuccess			(int iPEIndex, const char* pszLuaFunc);
	void			PE_PH_SetCallBackOnFail				(int iPEIndex, const char* pszLuaFunc);
	void			PE_PH_SetBonusScore					(int iPEIndex, int iPHIndex, float fBonus1, float fBonus2);
	void			PE_PH_SetNextPhase					(int iPEIndex, int iPHIndex, int iTgtPHIndex);

	void			PE_OB_SetCallBackOnAchieve			(int iPEIndex, const char* pszLuaFunc);
	void			PE_OB_SetNextPhase					(int iPEIndex, int iPHIndex, int iOBIndex, int iTgtPHIndex);
	void			PE_OB_SetVisible					(int iPEIndex, int iPHIndex, int iOBIndex, bool bVisible);

	int				PE_VAR_GetVar						(const char* pszVarName);
	void			PE_VAR_SetVar						(const char* pszVarName, int iValue);
	void			PE_VAR_AddVar						(const char* pszVarName, int iValue);
}