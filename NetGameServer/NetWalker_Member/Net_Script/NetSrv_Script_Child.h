#pragma once

#include ".\NetSrv_Script.h"	


class CNetSrv_Script_Child : public CNetSrv_Script
{
public:
	CNetSrv_Script_Child(void)	{};
	~CNetSrv_Script_Child(void)	{};

	static bool			Init();
	static bool			Release();
	//static int			OnEvent_CastingFinished		( SchedularInfo* Obj, void* InputClass );

	virtual void		On_QuestAccept				( BaseItemObject*	pObj, int iQuestNpcID, int iQuestID, bool bItemMode );
	virtual void		On_QuestComplete			( BaseItemObject*	pObj, int iQuestNpcID, int iQuestID, int iChoiceID );
	virtual void		On_QuestDelete				( BaseItemObject*	pObj, int iQuestField, int iQuestID );
													  
	virtual	void		On_TouchEvent				( BaseItemObject*	pObj, int iEventObjID ) ;
	virtual	void		On_RangeEvent				( BaseItemObject*	pObj, int iEventObjID ) ;
	virtual	void		On_CollisionEvent			( BaseItemObject*	pObj, int iEventObjID ) ;
	

	virtual void		On_ClickQuestObj			( BaseItemObject*	pObj, int iQuestField, int iQuestID, int iClickObj, int iClickObjID );
	virtual void		On_LoadQuestSpeakOption		( BaseItemObject*	pObj, int iNPCID );

	virtual void		On_ChoiceOption				( BaseItemObject*	pObj, int iOption );
	virtual void		On_DelayPatrol				( BaseItemObject*	pObj, int iNPCID, int iTime );

	//-------------------------------------------------------------------------------------

	virtual void		On_RoleDeadDropItem			( RoleDataEx* pKiller, RoleDataEx* pDeader, vector< ItemFieldStruct >* pDropList, vector< int >* pOwnerList , vector< bool >* pLockList );
	
	virtual void		On_RoleLogin				( RoleDataEx* pRole );

	virtual void		On_RoleMove					( RoleDataEx* pRole );
	virtual void		On_RoleCast					( RoleDataEx* pRole );
	virtual void		On_RoleBeHit				( RoleDataEx* pRole );

	virtual	void		On_CloseBorder				( BaseItemObject*	pObj );
	virtual	void		On_AcceptBorderQuest		( BaseItemObject*	pObj );


	static	void		CheckQuest					( RoleDataEx* pRole );

	static void			OnCast_QuestObj				( int iGUID, int iResult );
	static void			OnCast_ScriptCast			( int iGUID, int iResult );

	static void			CheckQuestRoleDead			( RoleDataEx* pKiller, RoleDataEx* pDeader, vector< ItemFieldStruct >* pDropList, vector< int >* pOwnerList , vector< bool >* pLockList, bool bTeam );
	static	int			CheckCompleteLoopQuest		( int iQuestID, GameObjectQuestDetail* pQuest, RoleDataEx* pRole );

	static void			ResetDailyQuest				( RoleDataEx* pRole );
	static void			ResetDailyFlag				( RoleDataEx* pRole, bool bForceReset );
};

class	CPlotEventQuestComplete : public PlotRunTimeEvent
{
public:
	virtual void		OnEnd						();

	int					m_iNPCID;
	int					m_iRoleID;
	int					m_iQuestID;
};

/*
 組隊後寫法:
	組隊後要是打倒目標, 會在 Temp 找尋隊伍計數器, 找看看隊伍誰擁有 Quest. 
	再叛斷機率是給物品( 100 是每個人都有 )

 */