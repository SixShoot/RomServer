#pragma once

#include "../../mainproc/Global.h"
#include "PG/PG_PublicEncounter.h"

//---------------------------------------------------------------------
// class define
//---------------------------------------------------------------------
class CPublicEncounterVariable;
class CPublicEncounterVariableEventHandler;
class CPublicEncounterPlayer;
class CPublicEncounterObjective;
class CPublicEncounterPhase;
class CPublicEncounter;

class CPublicEncounterObjectiveGreater;
class CPublicEncounterObjectiveSmaller;
class CPublicEncounterObjectiveEqual;
class CPublicEncounterObjectiveNotEqual;
class CPublicEncounterObjectiveEqualAndGreater;
class CPublicEncounterObjectiveEqualAndSmaller;
class CPublicEncounterObjectiveTimeLimit;
//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
class CNetSrv_PublicEncounter : public Global
{
protected:
	static void _PG_PE_CtoL_RegionChanged( int iCliID , int iSize , PVOID pData );

	static void _OnCliConnect( int iCliID , std::string Account );
	static void _OnCliDisconnect( int iCliID );

public:
	static CNetSrv_PublicEncounter*	m_pThis;

public:
	CNetSrv_PublicEncounter(){};
	~CNetSrv_PublicEncounter(){};

	static bool	_Init();
	static bool	_Release();

public:
	virtual void CliConnect( BaseItemObject* pObj, int iSockID ) = 0;
	virtual void CliDisconnect( int iSockID ) = 0;
	virtual void OnRZ_CliRegionChanged(BaseItemObject* pObj, int iRegionID) = 0;
};

//---------------------------------------------------------------------
//Public Encounter Player
//---------------------------------------------------------------------
class CPublicEncounterPlayer
{
protected:
	CPublicEncounter*		m_Owner;
	int						m_DBID;
	int						m_OldPartyID;
	float					m_Score;
	unsigned long			m_RemoveTime;
	unsigned long			m_StatisticsClearTime;
	BattleGroundInfoStruct	m_TempStatictisData;
	int						m_TempHatePoint;
	float					m_ContributionPoint;
	int 					m_Status;
	

public:
	CPublicEncounterPlayer(CPublicEncounter* pOwner, int iDBID);
	virtual ~CPublicEncounterPlayer();

public:
	CPublicEncounter*	GetOwner()							{return m_Owner;}
	int					GetDBID()							{return m_DBID;}
	int					GetGUID();
	int					GetPartyID();
	BaseItemObject*		GetObj()							{return Global::GetPlayerObj_ByDBID(m_DBID);}
	RoleDataEx*			GetRole()							{return Global::GetRoleDataByDBID(m_DBID);}
	float				GetScore()							{return m_Score;}
	void				SetScore(float value);
	int					GetStatus()							{return m_Status;}
	void				SetStatus							(int value);
	int					GetRxDamage();
	int					GetTxDamage();
	int					GetTxHeal();
	int					GetLevel();
	int					GetTempHatePoint()					{return m_TempHatePoint;}
	void				SetTempHatePoint(int value)			{m_TempHatePoint;}
	float				GetContributionPoint()				{return m_ContributionPoint;}
	void				SetContributionPoint(float value)	{m_ContributionPoint;}

public:
	void				Process(unsigned long dwNow, unsigned long dwElapsedTime);
	bool				IsPartyChange();
	void				ClearBattleStatictics();
	void				ClearAllData();

public:
	__declspec(property(get = GetOwner												))	CPublicEncounter*	Owner;
	__declspec(property(get = GetDBID												))	int					DBID;
	__declspec(property(get = GetGUID												))	int					GUID;
	__declspec(property(get = GetPartyID											))	int					PartyID;
	__declspec(property(get = GetObj												))	BaseItemObject*		pObj;
	__declspec(property(get = GetRole												))	RoleDataEx*			pRole;
	__declspec(property(get = GetScore				, put = SetScore				))	float				Score;
	__declspec(property(get = GetStatus				, put = SetStatus				))	int					Status;
	__declspec(property(get = GetRxDamage											))	int					RxDamage;
	__declspec(property(get = GetTxDamage											))	int					TxDamage;
	__declspec(property(get = GetTxHeal												))	int					TxHeal;
	__declspec(property(get = GetContributionPoint	, put = SetContributionPoint	))	float				ContributionPoint;
	__declspec(property(get = GetTempHatePoint		, put = SetTempHatePoint		))	int					TempHatePoint;
	__declspec(property(get = GetLevel												))	int					Level;
};

//---------------------------------------------------------------------
// public encounter objective class 
//---------------------------------------------------------------------
class CPublicEncounterObjective
{
protected:
	StructPEObjectiveBaseData		m_BaseData;
	CPublicEncounterPhase*			m_Owner;
	std::string						m_VarName;		// variable name which this term refers to
	CPublicEncounterPhase*			m_NextPhase;
	int								m_OldValue;

public:
	CPublicEncounterObjective(CPublicEncounterPhase* pOwner);
	virtual ~CPublicEncounterObjective();

public:
	int							GetIndex()									{return m_BaseData.iOBIndex;}
	void						SetIndex(int value)							{m_BaseData.iOBIndex = value;}
	CPublicEncounter*			GetPE();
	int							GetPHIndex();
	int							GetPEIndex();
	int							GetKind()									{return m_BaseData.iKind;}
	void						SetKind(int value)							{m_BaseData.iKind = value;}
	int							GetType()									{return m_BaseData.iType;}
	void						SetType(int value)							{m_BaseData.iType = value;}
	int							GetInitValue()								{return m_BaseData.iInitValue;}
	void						SetInitValue(int value)						{m_BaseData.iInitValue = value;}
	int							GetDoorsill()								{return m_BaseData.iDoorsill;}
	void						SetDoorsill(int value)						{m_BaseData.iDoorsill = value;}
	const char*					GetName()									{return m_BaseData.szNameKeyStr;}
	void						SetName(const char* value)					{strcpy(m_BaseData.szNameKeyStr, value);}
	const char*					GetDescription()							{return m_BaseData.szDescKeyStr;}
	void						SetDescription(const char* value)			{strcpy(m_BaseData.szDescKeyStr, value);}
	const char*					GetVarName()								{return m_VarName.c_str();}
	void						SetVarName(const char* value)				{m_VarName = value;}
	CPublicEncounterPhase*		GetNextPhase()								{return m_NextPhase;}
	void						SetNextPhase(CPublicEncounterPhase* value)	{m_NextPhase = value;}
	CPublicEncounterPhase*		GetOwner()									{return m_Owner;}	
	int							GetStatus()									{return m_BaseData.iStatus;}
	void						SetStatus(int value);
	bool						GetVisible();
	void						SetVisible(bool value);
	bool						GetMinorFlag();
	void						SetMinorFlag(bool value);
	StructPEObjectiveBaseData*	GetBaseData();
	virtual int					GetNowValue();
	virtual void				SetNowValue(int value);
	virtual std::string			GetTypeString()								{return "EM_OBJ_TYPE_NONE";}
	virtual std::string			GetOperatorString()							{return "";}

protected:
	virtual	bool				Check(unsigned long dwNow, unsigned long dwElapsedTime) = 0;
	virtual	void				OnValueChange(int iOldValue, int iNewValue); 

public:	
	virtual	void				_GetStatusString(vector<std::string>& vecStrings);
	
public:
	int							Process(unsigned long dwNow, unsigned long dwElapsedTime);
	void						Reset();

public:
	__declspec(property(get = GetIndex			, put = SetIndex			))	int							Index;
	__declspec(property(get = GetPE											))	CPublicEncounter*			OwnerPE;
	__declspec(property(get = GetPHIndex									))	int							PHIndex;
	__declspec(property(get = GetPEIndex									))	int							PEIndex;
	__declspec(property(get = GetKind			, put = SetKind				))	int							Kind;
	__declspec(property(get = GetType			, put = SetType				))	int							Type;
	__declspec(property(get = GetInitValue		, put = SetInitValue		))	int							InitValue;
	__declspec(property(get = GetDoorsill		, put = SetDoorsill			))	int							Doorsill;
	__declspec(property(get = GetNowValue		, put = SetNowValue			))	int							NowValue;
	__declspec(property(get = GetBaseData									))	StructPEObjectiveBaseData*	BaseData;
	__declspec(property(get = GetDescription	, put = SetDescription		))	const char*					Description;
	__declspec(property(get = GetOwner										))	CPublicEncounterPhase*		Owner;
	__declspec(property(get = GetName			, put = SetName				))	const char*					Name;
	__declspec(property(get = GetVarName		, put = SetVarName			))	const char*					VarName;
	__declspec(property(get = GetStatus			, put = SetStatus			))	int							Status;
	__declspec(property(get = GetVisible		, put = SetVisible			))	bool						Visible;
	__declspec(property(get = GetMinorFlag		, put = SetMinorFlag		))	bool						IsMinor;
	__declspec(property(get = GetNextPhase		, put = SetNextPhase		))	CPublicEncounterPhase*		NextPhase;
	__declspec(property(get = GetTypeString									))	std::string					TypeString;
	__declspec(property(get = GetOperatorString								))	std::string					OperatorString;

public:
	static CPublicEncounterObjective* CreateInstance(int iType, CPublicEncounterPhase* pOwner);
};

//---------------------------------------------------------------------
// term : flag
//---------------------------------------------------------------------
class CPublicEncounterObjectiveGreater : public CPublicEncounterObjective
{
public:
	CPublicEncounterObjectiveGreater(CPublicEncounterPhase* pOwner);
	virtual ~CPublicEncounterObjectiveGreater();

public:
	std::string	GetTypeString()		{return "EM_OBJ_TYPE_GREATER";}
	std::string	GetOperatorString() {return ">";}

protected:
	bool	Check(unsigned long dwNow, unsigned long dwElapsedTime);
};

//---------------------------------------------------------------------
// term : Smaller
//---------------------------------------------------------------------
class CPublicEncounterObjectiveSmaller : public CPublicEncounterObjective
{
public:
	CPublicEncounterObjectiveSmaller(CPublicEncounterPhase* pOwner);
	virtual ~CPublicEncounterObjectiveSmaller();

public:
	std::string	GetTypeString()		{return "EM_OBJ_TYPE_SMALLER";}
	std::string	GetOperatorString() {return "<";}

protected:
	bool	Check(unsigned long dwNow, unsigned long dwElapsedTime);
};

//---------------------------------------------------------------------
// term : Equal
//---------------------------------------------------------------------
class CPublicEncounterObjectiveEqual : public CPublicEncounterObjective
{
public:
	CPublicEncounterObjectiveEqual(CPublicEncounterPhase* pOwner);
	virtual ~CPublicEncounterObjectiveEqual();

public:
	std::string	GetTypeString()		{return "EM_OBJ_TYPE_EQUAL";}
	std::string	GetOperatorString() {return "=";}

protected:
	bool	Check(unsigned long dwNow, unsigned long dwElapsedTime);
};
//---------------------------------------------------------------------
// term : NotEqual
//---------------------------------------------------------------------
class CPublicEncounterObjectiveNotEqual : public CPublicEncounterObjective
{
public:
	CPublicEncounterObjectiveNotEqual(CPublicEncounterPhase* pOwner);
	virtual ~CPublicEncounterObjectiveNotEqual();

public:
	std::string	GetTypeString()		{return "EM_OBJ_TYPE_NOT_EQUAL";}
	std::string	GetOperatorString() {return "!=";}

protected:
	bool	Check(unsigned long dwNow, unsigned long dwElapsedTime);
};
//---------------------------------------------------------------------
// term : EqualAndGreater
//---------------------------------------------------------------------
class CPublicEncounterObjectiveEqualAndGreater : public CPublicEncounterObjective
{
public:
	CPublicEncounterObjectiveEqualAndGreater(CPublicEncounterPhase* pOwner);
	virtual ~CPublicEncounterObjectiveEqualAndGreater();

public:
	std::string	GetTypeString()		{return "EM_OBJ_TYPE_EQUAL_AND_GREATER";}
	std::string	GetOperatorString() {return ">=";}

protected:
	bool	Check(unsigned long dwNow, unsigned long dwElapsedTime);
};
//---------------------------------------------------------------------
// term : EqualAndSmaller
//---------------------------------------------------------------------
class CPublicEncounterObjectiveEqualAndSmaller : public CPublicEncounterObjective
{
public:
	CPublicEncounterObjectiveEqualAndSmaller(CPublicEncounterPhase* pOwner);
	virtual ~CPublicEncounterObjectiveEqualAndSmaller();

public:
	std::string	GetTypeString()		{return "EM_OBJ_TYPE_EQUAL_AND_SMALLER";}
	std::string	GetOperatorString() {return "<=";}

protected:
	bool	Check(unsigned long dwNow, unsigned long dwElapsedTime);
};

//---------------------------------------------------------------------
// term : EqualAndSmaller
//---------------------------------------------------------------------
class CPublicEncounterObjectiveTimeLimit : public CPublicEncounterObjective
{
protected:
	unsigned long	m_LastSyncTick;

public:
	CPublicEncounterObjectiveTimeLimit(CPublicEncounterPhase* pOwner);
	virtual ~CPublicEncounterObjectiveTimeLimit();

public:
	std::string		GetTypeString()				{return "EM_OBJ_TYPE_TIME_LIMIT";}

protected:
	bool		Check(unsigned long dwNow, unsigned long dwElapsedTime);
	void		OnValueChange(int iOldValue, int iNewValue);

public:
	void		_GetOperatorString(vector<std::string>& vecStrings);
};

//---------------------------------------------------------------------
// Public Encounter Phase class
//---------------------------------------------------------------------
class CPublicEncounterPhase
{
protected:
	CPublicEncounter*						m_Owner;
	StructPEPhaseBaseData					m_BaseData;
	CPublicEncounterPhase*					m_NextPhase;

	unsigned long							m_NextPhaseTimeGap;				// how long will next phase begin
	unsigned long							m_WaitTime;

	float									m_BonusScore1;
	float									m_BonusScore2;

	bool									m_IsCountContribution;

	vector<CPublicEncounterObjective*>		m_SuccessObjectives;
	vector<CPublicEncounterObjective*>		m_FailObjectives;
	vector<CPublicEncounterObjective*>		m_Objectives;
	map<int, CPublicEncounterObjective*>	m_ObjectivesIndex;

public:
	CPublicEncounterPhase(CPublicEncounter* pOnwer);
	virtual ~CPublicEncounterPhase();

public:
	int									GetIndex()									{return m_BaseData.iPHIndex;}
	void								SetIndex(int value)							{m_BaseData.iPHIndex = value;}
	int									GetPEIndex();
	StructPEPhaseBaseData*				GetBaseData();
	const char*							GetDescription()							{return m_BaseData.szDescKeyStr;}
	void								SetDescription(const char* value)			{strcpy(m_BaseData.szDescKeyStr, value);}
	CPublicEncounter*					GetOwner()									{return m_Owner;}
	const char*							GetName()									{return m_BaseData.szNameKeyStr;}
	void								SetName(const char* value)					{strcpy(m_BaseData.szNameKeyStr, value);}
	int									GetStatus()									{return m_BaseData.iStatus;}
	void								SetStatus(int value);
	CPublicEncounterPhase*				GetNextPhase()								{return m_NextPhase;}
	void								SetNextPhase(CPublicEncounterPhase* value)	{m_NextPhase = value;}
	float								GetPhaseScore()								{return m_BaseData.fPHScore;}
	void								SetPhaseScore(float fValue)					{m_BaseData.fPHScore = fValue;}
	float								GetBonusScore1()							{return m_BonusScore1;}
	void								SetBonusScore1(float fValue)				{m_BonusScore1 = fValue;}
	float								GetBonusScore2()							{return m_BonusScore2;}
	void								SetBonusScore2(float fValue)				{m_BonusScore2 = fValue;}
	bool								GetIsCountContribution()					{return m_IsCountContribution;}
	void								SetIsCountContribution(bool value)			{m_IsCountContribution = value;}
	int									GetSuccLogic()								{return m_BaseData.iSuccLogic;}
	void								SetSuccLogic(int value)						{m_BaseData.iSuccLogic = value;}
	int									GetFailLogic()								{return m_BaseData.iFailLogic;}
	void								SetFailLogic(int value)						{m_BaseData.iFailLogic = value;}
	unsigned long						GetNextPhaseTimeGap()						{return m_NextPhaseTimeGap;}
	void								SetNextPhaseTimeGap(unsigned long value)	{m_NextPhaseTimeGap = value;}
	vector<CPublicEncounterObjective*>&	GetObjectives()								{return m_Objectives;}

public:
	CPublicEncounterObjective*			AddObjective(const char* pszName, const char* pszDesc, int iIndex, int iType, const char* pszVarName, int iInitValue, int iDoorsill, int iKind);
	CPublicEncounterObjective*			GetObjective(int iOBIndex);
	void								RemoveObject(int iOBIndex);
	void								SetObjectiveNextPhase(int iOBIndex, CPublicEncounterPhase* pPhase);
	void								SetAllSuccObjectiveNextPhase(CPublicEncounterPhase* pPhase);
	void								SetAllFailObjectiveNextPhase(CPublicEncounterPhase* pPhase);

public:
	void								_GetStatusString(vector<std::string>& vecStrings);

public:
	void								Reset();
	int									Process(unsigned long dwNow, unsigned long dwElapsedTime);

public:
	__declspec(property(get = GetIndex					, put = SetIndex				))	int									Index;
	__declspec(property(get = GetPEIndex												))	int									PEIndex;
	__declspec(property(get = GetBaseData												))	StructPEPhaseBaseData*				BaseData;
	__declspec(property(get = GetDescription			, put = SetDescription			))	const char*							Description;
	__declspec(property(get = GetOwner													))	CPublicEncounter*					Owner;
	__declspec(property(get = GetName					, put = SetName					))	const char*							Name;
	__declspec(property(get = GetStatus					, put = SetStatus				))	int									Status;
	__declspec(property(get = GetNextPhase				, put = SetNextPhase			))	CPublicEncounterPhase*				NextPhase;
	__declspec(property(get = GetPhaseScore				, put = SetPhaseScore			))	float								PhaseScore;
	__declspec(property(get = GetBonusScore1			, put = SetBonusScore1			))	float								BonusScore1;
	__declspec(property(get = GetBonusScore2			, put = SetBonusScore2			))	float								BonusScore2;
	__declspec(property(get = GetIsCountContribution	, put = SetIsCountContribution	))	bool								IsCountContribution;
	__declspec(property(get = GetSuccLogic				, put = SetSuccLogic			))	int									SuccLogic;
	__declspec(property(get = GetFailLogic				, put = SetFailLogic			))	int									FailLogic;
	__declspec(property(get = GetNextPhaseTimeGap		, put = SetNextPhaseTimeGap		))	unsigned long						NextPhaseTimeGap;
	__declspec(property(get = GetObjectives												))	vector<CPublicEncounterObjective*>&	Objectives;
};

//---------------------------------------------------------------------
//Area Event class
//---------------------------------------------------------------------
class CPublicEncounter
{
protected:
	//-----------------------------------------------------------------
	//Definition Data
	StructPEBaseData							m_BaseData;

	vector<StructPERegionData>					m_Regions;					// indicate which regions area event occurred.
	vector<CPublicEncounterPhase*>				m_Phases;
	map<int, CPublicEncounterPhase*>			m_PhasesIndex;

	map<int, CPublicEncounterPlayer*>			m_Players;					// index by DBID
	std::deque<int>								m_TempActivePlayerGUIDs;	// 

	unsigned long								m_ResetBeginTime;			// the time that reset process really begin.
	unsigned long								m_ResetTimeGap;				// how long will reset process begin after receiving reset signal

	std::string									m_LuaOnBegin;				// OnBegin event, call Lua script
	std::string									m_LuaOnSuccess;				// OnSuccess event, call Lua script
	std::string									m_LuaOnFail;				// OnFailed event, call Lua script

	std::string									m_LuaOnPHBegin;				// Phase Begin event, call Lua script
	std::string									m_LuaOnPHSuccess;			// Phase Success event, call Lua script
	std::string									m_LuaOnPHFail;				// Phase Failed event, call Lua script

	std::string									m_LuaOnOBAchieve;			// OnSuccess event, call Lua script
	std::string									m_LuaOnBalance;				// OnBanace event when PE end

	std::string									m_LuaOnPlayerEnter;			// OnPlayerEnter event when player enters any region of this PE
	std::string									m_LuaOnPlayerLeave;			// OnPlayerEnter event when player leaves any region of this PE
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	//RunTime Data
	int											m_ActivePhaseIndex;
	CPublicEncounterPhase*						m_ActivePhase;				// active phase

	unsigned long								m_PreTime;					// pre tick
	unsigned long								m_NowTime;					// now tick
	unsigned long								m_ElapsedTime;				// elapsed time
	//-----------------------------------------------------------------

public:
	CPublicEncounter(const char* pszName);
	virtual ~CPublicEncounter();

public:
	int								GetIndex()									{return m_BaseData.iPEIndex;}
	void							SetIndex(int value)							{m_BaseData.iPEIndex = value;}
	StructPEBaseData*				GetBaseData()								{return &m_BaseData;}
	CPublicEncounterPhase*			GetActivePhase()							{return m_ActivePhase;}
	void							SetActivePhase(CPublicEncounterPhase* value);
	int								GetStatus()									{return m_BaseData.iStatus;}
	void							SetStatus(int value);
	const char*						GetName()									{return m_BaseData.szNameKeyStr;}
	void							SetName(const char* value)					{strcpy(m_BaseData.szNameKeyStr, value);}
	unsigned long					GetResetTimeGap()							{return m_ResetTimeGap;}
	void							SetResetTimeGap(unsigned long value)		{m_ResetTimeGap = value;}
	const char*						GetDescription()							{return m_BaseData.szDescKeyStr;}
	void							SetDescription(const char* value)			{strcpy(m_BaseData.szDescKeyStr, value);}
	vector<StructPERegionData>&		GetRegions()								{return m_Regions;}
	vector<CPublicEncounterPhase*>&	GetPhases()									{return m_Phases;}
	bool							GetScoreVisible();
	void							SetScoreVisible(bool value);
	bool							GetUIVisible();
	void							SetUIVisible(bool value);
	const char*						GetLuaOnBegin()								{return m_LuaOnBegin.c_str();}
	void							SetLuaOnBegin(const char* value)				{m_LuaOnBegin = value;}
	const char*						GetLuaOnSuccess()							{return m_LuaOnSuccess.c_str();}
	void							SetLuaOnSuccess(const char* value)			{m_LuaOnSuccess = value;}
	const char*						GetLuaOnFail()								{return m_LuaOnFail.c_str();}
	void							SetLuaOnFail(const char* value)				{m_LuaOnFail = value;}
	const char*						GetLuaOnPHBegin()							{return m_LuaOnPHBegin.c_str();}
	void							SetLuaOnPHBegin(const char* value)			{m_LuaOnPHBegin = value;}
	const char*						GetLuaOnPHSuccess()							{return m_LuaOnPHSuccess.c_str();}
	void							SetLuaOnPHSuccess(const char* value)		{m_LuaOnPHSuccess = value;}
	const char*						GetLuaOnPHFail()							{return m_LuaOnPHFail.c_str();}
	void							SetLuaOnPHFail(const char* value)			{m_LuaOnPHFail = value;}
	const char*						GetLuaOnOBAchieve()							{return m_LuaOnOBAchieve.c_str();}
	void							SetLuaOnOBAchieve(const char* value)		{m_LuaOnOBAchieve = value;}
	const char*						GetLuaOnBalance()							{return m_LuaOnBalance.c_str();}
	void							SetLuaOnBalance(const char* value)			{m_LuaOnBalance = value;}
	const char*						GetLuaOnPlayerEnter()						{return m_LuaOnPlayerEnter.c_str();}
	void							SetLuaOnPlayerEnter(const char* value)		{m_LuaOnPlayerEnter = value;}
	const char*						GetLuaOnPlayerLeave()						{return m_LuaOnPlayerLeave.c_str();}
	void							SetLuaOnPlayerLeave(const char* value)		{m_LuaOnPlayerLeave = value;}	

public:
	void							AddRegion(int iRegionID);
	void							AddPhase(const char* pszName, const char* pszDesc, int iIndex, float fScore, int iSuccLogic, int iFailLogic, int iTimeGap);	
	bool							JumpToPhase(int iIndex);
	void							SetPhaseBonusScore(float fBonusScore1, float fBonusScore2);
	void							GivePlayerScore(int iGUID, float fScore);
	void							GivePartyScore(int iPartyID, float fScore);
	void							GiveAllPlayersScore(float fScore);
	float							GetPlayerScore(int iGUID);
	void							GetPartyScore(int iPartyID, vector<int>& vecGUID, vector<float>& vecScore);
	void							GetTopNPlayersScore(vector<int>& vecGUID, vector<float>& vecScore, int iTopNum = 0);	
	void							SetPlayerScore(int iGUID, float fScore);
	void							CountHatePoint(int iGUID);
	int								GetActivePlayerGUIDCount();
	int								GetActivePlayerGUID();

public:
	void							_GetDetailString(vector<std::string>& vecStrings);
	void							_GetStatusString(vector<std::string>& vecStrings);

public:
	bool							Start(int iPHIndex);
	void							Reset();
	void							Process();
	void							Stop();
	bool							AdvanceToNextPhase();

public:
	void							CliConnect(int iDBID);
	void							CliDisconnect(int iDBID);
	void							CliRegionChanged(BaseItemObject* pObj);	

	CPublicEncounterPhase*			GetPhase(int iIndex);
	CPublicEncounterPlayer*			GetPlayer(int iGUID);
	void							GetParty(int iPartyID, vector<int>& vecParty);
	void							GetAllParty(map<int, vector<int>>& mapParty);

	bool							IsPlayerInRegion(BaseItemObject* pObj, int iRegionID);
	bool							IsPlayerInPE(BaseItemObject* pObj);

	void							OnRefreshConfig();

public:
	void							OnBegin();
	void							OnBalance();
	void							OnPHBegin(CPublicEncounterPhase* pPH);
	void							OnPHSuccess(CPublicEncounterPhase* pPH);
	void							OnPHFail(CPublicEncounterPhase* pPH);
	void							OnPHBalance(CPublicEncounterPhase* pPH);
	void							OnOBAchieve(CPublicEncounterObjective* pOB);
	void							OnPlayerEnter(CPublicEncounterPlayer* pPlayer);
	void							OnPlayerLeave(CPublicEncounterPlayer* pPlayer);
	
public:
	__declspec(property(get = GetIndex				, put = SetIndex			))	int								Index;
	__declspec(property(get = GetBaseData										))	StructPEBaseData*				BaseData;
	__declspec(property(get = GetActivePhase		, put = SetActivePhase		))	CPublicEncounterPhase*			ActivePhase;
	__declspec(property(get = GetStatus				, put = SetStatus			))	int								Status;
	__declspec(property(get = GetName				, put = SetName				))	const char*						Name;
	__declspec(property(get = GetResetTimeGap		, put = SetResetTimeGap		))	unsigned long					ResetTimeGap;
	__declspec(property(get = GetDescription		, put = SetDescription		))	const char*						Description;
	__declspec(property(get = GetRegions										))	vector<StructPERegionData>&		Regions;
	__declspec(property(get = GetPhases											))	vector<CPublicEncounterPhase*>&	Phases;
	__declspec(property(get = GetScoreVisible		, put = SetScoreVisible		))	bool							ScoreVisible;
	__declspec(property(get = GetUIVisible			, put = SetUIVisible		))	bool							UIVisible;
	__declspec(property(get = GetLuaOnBegin			, put = SetLuaOnBegin		))	const char*						LuaOnBegin;
	__declspec(property(get = GetLuaOnSuccess		, put = SetLuaOnSuccess		))	const char*						LuaOnSuccess;
	__declspec(property(get = GetLuaOnFail			, put = SetLuaOnFail		))	const char*						LuaOnFail;
	__declspec(property(get = GetLuaOnPHBegin		, put = SetLuaOnPHBegin		))	const char*						LuaOnPHBegin;
	__declspec(property(get = GetLuaOnPHSuccess		, put = SetLuaOnPHSuccess	))	const char*						LuaOnPHSuccess;
	__declspec(property(get = GetLuaOnPHFail		, put = SetLuaOnPHFail		))	const char*						LuaOnPHFail;
	__declspec(property(get = GetLuaOnOBAchieve		, put = SetLuaOnOBAchieve	))	const char*						LuaOnOBAchieve;
	__declspec(property(get = GetLuaOnBalance		, put = SetLuaOnBalance		))	const char*						LuaOnBalance;
	__declspec(property(get = GetLuaOnPlayerEnter	, put = SetLuaOnPlayerEnter	))	const char*						LuaOnPlayerEnter;
	__declspec(property(get = GetLuaOnPlayerLeave	, put = SetLuaOnPlayerLeave	))	const char*						LuaOnPlayerLeave;
};


//---------------------------------------------------------------------
//Manager class for Public Encounter
//---------------------------------------------------------------------
class PublicEncounterManager
{
protected:
	static void*						m_PacketBuffer;
	static unsigned long				m_PacketBufferSize;
	static vector<CPublicEncounter*>	m_PEList;
	static map<int, CPublicEncounter*>	m_PEIndex;
	static vector<int>					m_ConnectedClient;			//SockID
	static map<int, int>				m_SockID_DBIDIndex;			//SockID - DBID
	static map<std::string, int>		m_Variables;
	static MutilThread_CritSect			m_CritSectVar;

public:
	static void							Init();
	static void							LoadObjectDB();
	static void							Release();
	static int							Process(SchedularInfo* Obj, void* InputClass);
	static CPublicEncounter*			GetPE(int iPEIndex);
	static CPublicEncounterPhase*		GetPH(int iPEIndex, int iPHIndex);
	static CPublicEncounterObjective*	GetOB(int iPEIndex, int iPHIndex, int iOBIndex);
	static int							GetVar(const char* pszName);
	static void							SetVar(const char* pszName, int value);	
	static void							AddVar(const char* pszName, int value);
	static void							CallLuaFunc(std::string strFuncName, vector<MyVMValueStruct>* pVecArgs, vector<MyVMValueStruct>* pVecResult);
	static std::string					ToLower(const char* pszString);
	static float						CalculateContribution(int iHate, int iRxDmg, int iTxDmg, int iTxHeal, int iLv);

public:
	static void							CliConnect(BaseItemObject* pObj, int iSockID);
	static void							CliDisconnect(int iSockID);
	static void							CliRegionChanged(BaseItemObject* pObj, int iRegionID);

public:
	static void							SendToAllPlayers(int iSize, PVOID pData);
	static void							SC_PEConfigData(int iSockID);
	static void							SC_PEStatus(int iDBID, int iPEIndex, int iStatus);
	static void							SC_PhaseStatus(int iDBID, int iPEIndex, int iPHIndex, int iStatus);
	static void							SC_ObjectiveStatus(int iDBID, int iPEIndex, int iPHIndex, int iOBIndex, int iStatus);
	static void							SC_PlayerScore(int iDBID, int iPEIndex, int iScore);
	static void							SC_PEActivated(int iDBID, int iPEIndex, int iActivePHIndex);
	static void							SC_PEDeactivated(int iDBID, int iPEIndex);
	static void							SC_PEActivePhaseChange(int iDBID, int iPEIndex, int iActivePHIndex);
	static void							SC_ObjectiveValueChage(int iDBID, int iPEIndex, int iPHIndex, int iOBIndex, int value);
	static void							SC_PESuccess(int iDBID, int iPEIndex);
	static void							SC_PEFailed(int iDBID, int iPEIndex);
	static void							SC_PhaseSuccess(int iDBID, int iPEIndex, int iPHIndex);
	static void							SC_PhaseFailed(int iDBID, int iPEIndex, int iPHIndex);
	static void							SC_ObjectiveAchieved(int iDBID, int iPEIndex, int iPHIndex, int iOBIndex);
	static void							SC_ObjectiveFlag(int iDBID, int iPEIndex, int iPHIndex, int iOBIndex, int iFlag);
	static void							SC_PEFlag(int iDBID, int iPEIndex, int iFlag);

public:
	static void							PE_Start(int iPEIndex, int iPHIndex);
	static void							PE_AddPE(const char* pszName,const char* pszDesc, int iIndex, int iTimeGap);
	static void							PE_AddRegion(int iPEIndex, int iRegionID);
	static void							PE_AddPhase(int iPEIndex, int iIndex, const char* pszName, const char* pszDesc, int iScore, int iSuccLogic, int iFailLogic, int iTimeGap);
	static void							PE_JumpToPhase(int iPEIndex, int iPHIndex);
	static void							PE_SetCallBackOnBegin(int iPEIndex, const char* pszLuaFunc);
	static void							PE_SetCallBackOnSuccess(int iPEIndex, const char* pszLuaFunc);
	static void							PE_SetCallBackOnFail(int iPEIndex, const char* pszLuaFunc);
	static void							PE_SetCallBackOnBalance(int iPEIndex, const char* pszLuaFunc);
	static void							PE_SetCallBackOnPlayerEnter(int iPEIndex, const char* pszLuaFunc);
	static void							PE_SetCallBackOnPlayerLeave(int iPEIndex, const char* pszLuaFunc);
	static void							PE_SetPhaseBonusScore(int iPEIndex, float fBonus1, float fBonus2);
	static void							PE_GivePlayerScore(int iPEIndex, int iGUID, float fScore);
	static void							PE_GivePartyScore(int iPEIndex, int iPartyID, float fScore);
	static void							PE_GiveAllPlayersScore(int iPEIndex, float fScore);
	static float						PE_GetPlayerScore(int iPEIndex, int iGUID);
	static void							PE_GetPartyScore(int iPEIndex, int iPartyID, const char* pszLuaCallback);
	static void							PE_GetAllPlayersScore(int iPEIndex, const char* pszLuaCallback);
	static void							PE_GetTopNPlayersScore(int iPEIndex, int iTopN, const char* pszLuaCallback);
	static void							PE_SetPlayerScore(int iPEIndex, int iGUID, float fScore);
	static void							PE_CountHatePoint(int iPEIndex, int iGUID);
	static int							PE_GetActivePhaseIndex(int iPEIndex);
	static int							PE_GetActivePlayerGUIDCount(int iPEIndex);
	static int							PE_GetActivePlayerGUID(int iPEIndex);
	static void							PE_SetScoreVisible(int iPEIndex, bool bVisible);
	static void							PE_SetUIVisible(int iPEIndex, bool bVisible);
	static void							PE_RefreshConfig();
	static void							PE_PH_AddObjective(int iPEIndex, int iPHIndex, int iIndex, const char* pszName, const char* pszDesc, int iType, const char* pszVarName, int iInitValue, int iDoorsill, bool bSucc);
	static void							PE_PH_SetCallBackOnBegin(int iPEIndex, const char* pszLuaFunc);
	static void							PE_PH_SetCallBackOnSuccess(int iPEIndex, const char* pszLuaFunc);
	static void							PE_PH_SetCallBackOnFail(int iPEIndex, const char* pszLuaFunc);
	static void							PE_PH_SetBonusScore(int iPEIndex, int iPHIndex, float fBonus1, float fBonus2);
	static void							PE_PH_SetNextPhase(int iPEIndex, int iPHIndex, int iTgtPHIndex);
	static void							PE_OB_SetCallBackOnAchieve(int iPEIndex, const char* pszLuaFunc);
	static void							PE_OB_SetNextPhase(int iPEIndex, int iPHIndex, int iOBIndex, int iTgtPHIndex);
	static void							PE_OB_SetVisible(int iPEIndex, int iPHIndex, int iOBIndex, bool bVisible);
	static int							PE_VAR_GetVar(const char* pszVarName);
	static void							PE_VAR_SetVar(const char* pszVarName, int value);
	static void							PE_VAR_AddVar(const char* pszVarName, int value);

public:
	static bool							_ReleasePE(int iPEIndex);
	static bool							_GetPEStatus(int iPEIndex, vector<std::string>& vecString);
	static bool							_GetPEDetail(int iPEIndex, vector<std::string>& vecString);
	static bool							_ResetPE(int iPEIndex);
	static bool							_ListAllVar(vector<std::string>& vecNames, vector<int>& vecValues);
	static bool							_PEInstSucc(int iPEIndex);
	static bool							_PEInstFail(int iPEIndex);
	static bool							_JumpToPhase(int iPEIndex, int iPHIndex);
	static bool							_PHInstSucc(int iPEIndex, int iPHIndex);
	static bool							_PHInstFail(int iPEIndex, int iPHIndex);
	static bool							_OBInstAchieve(int iPEIndex, int iPHIndex, int iOBIndex);
	static bool							_GiveScore(int iPEIndex, int iGUID, float fScore);
	static float						_GetScore(int iPEIndex, int iGUID);
};