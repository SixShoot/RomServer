#pragma once

#include "../../mainproc/Global.h"
#include "PG/PG_Script.h"



class CNetSrv_Script : public Global
{
protected:

	static void _PG_Script_CtoL_QuestAccept			( int iCliID , int iSize , PVOID pData );    
	static void _PG_Script_CtoL_QuestComplete		( int iCliID , int iSize , PVOID pData );
	static void _PG_Script_CtoL_QuestDelete			( int iCliID , int iSize , PVOID pData );

	static void _PG_Script_CtoL_TouchEvent			( int iCliID , int iSize , PVOID pData );
	static void _PG_Script_CtoL_RangeEvent			( int iCliID , int iSize , PVOID pData );
	static void _PG_Script_CtoL_CollisionEvent		( int iCliID , int iSize , PVOID pData );

	static void _PG_Script_CtoL_TouchObj			( int iCliID , int iSize , PVOID pData );
	
	static void _PG_Script_CtoL_ClickQuestObj		( int iCliID , int iSize , PVOID pData );
	static void _PG_Script_CtoL_LoadQuestSpeakOption( int iCliID , int iSize , PVOID pData );
	static void _PG_Script_CtoL_ChoiceOption		( int iCliID , int iSize , PVOID pData );

	static void _PG_Script_CtoL_DelayPatrol			( int iCliID , int iSize , PVOID pData );

	static void _PG_Script_CtoL_AcceptBorderQuest	( int iCliID , int iSize , PVOID pData );
	static void _PG_Script_CtoL_CloseBorder			( int iCliID , int iSize , PVOID pData );

public:

    CNetSrv_Script(){};
    ~CNetSrv_Script(){};

    static bool			Init();
    static bool			Release(){ return true;};

	static void			OnEvent_RoleDeadDropItem	( RoleDataEx* pKiller, RoleDataEx* pDeader, vector< ItemFieldStruct >* pDropList, vector< int >* pOwnerList , vector< bool >* pLockList ) { m_pThis->On_RoleDeadDropItem( pKiller, pDeader, pDropList, pOwnerList , pLockList ); }
	static void			OnEvent_RoleLogin			( RoleDataEx* pRole ) { m_pThis->On_RoleLogin ( pRole ); }
	
	static void			OnEvent_RoleMove			( RoleDataEx* pRole ) { m_pThis->On_RoleMove( pRole ); }
	static void			OnEvent_RoleCast			( RoleDataEx* pRole ) { m_pThis->On_RoleCast( pRole ); }
	static void			OnEvent_RoleBeHit			( RoleDataEx* pRole ) { m_pThis->On_RoleBeHit( pRole ); }


	// Lua Function
	//-------------------------------------------------------------------------------------
	static bool			Lua_Quest_CheckRoleItem		( int iObjID, int iItemID, int iCount );
	static bool			Lua_Quest_DeleteItem		( int iObjID, int iItemID, int iCount );


	// 事件
	//-------------------------------------------------------------------------------------
	virtual void		On_QuestAccept				( BaseItemObject*	pObj, int iQuestNpcID, int iQuestID, bool bItemMode ) = 0;
		static void			QuestAcceptResult		( int iSockID, int iQuestField, int iQuestID, EM_QUESTCHECK_RESULT emResult );
	
	virtual void		On_QuestComplete			( BaseItemObject*	pObj, int iQuestNpcID, int iQuestID, int iChoiceID ) = 0;
		static void			QuestCompleteResult		( int iSockID, int iQuestNPCID, int iQuestField, int iQuestID, EM_QUESTCHECK_RESULT emResult );

	virtual void		On_QuestDelete				( BaseItemObject*	pObj, int iQuestField, int iQuestID ) = 0;
		static void			QuestDeleteResult		( int iSockID, int iQuestField, int iQuestID, EM_QUESTCHECK_RESULT emResult );


	virtual void		On_ClickQuestObj			( BaseItemObject*	pObj, int iQuestField, int iQuestID, int iClickObjID, int iClickObj ) = 0;
		static void			ClickQuestObjFailed		( int iSockID );


	//virtual void		On_ClickQuestObjCheckItem	( 

	virtual	void		On_TouchEvent				( BaseItemObject*	pObj, int iEventObjID ) = 0;
	virtual	void		On_RangeEvent				( BaseItemObject*	pObj, int iEventObjID ) = 0;
	virtual	void		On_CollisionEvent			( BaseItemObject*	pObj, int iEventObjID ) = 0;
	

	virtual void		On_RoleDeadDropItem			( RoleDataEx* pKiller, RoleDataEx* pDeader, vector< ItemFieldStruct >* pDropList, vector< int >* pOwnerList , vector< bool >* pLockList ) = 0;
	virtual void		On_RoleLogin				( RoleDataEx* pRole ) = 0;

	virtual void		On_RoleMove					( RoleDataEx* pRole ) = 0;
	virtual void		On_RoleCast					( RoleDataEx* pRole ) = 0;
	virtual void		On_RoleBeHit				( RoleDataEx* pRole ) = 0;

	

	static	void		ClickQuestObjResult			( int iRoleID, int iResult, int iItemID, int iItemVal );
	static	void		KillObjNotity				( int iRoleID, int iQuestField, int iQuestID, int iDeadObjID );
	static	void		GetQuestObjNotify			( int iRoleID, int iQuestField, int iQuestID, int iObjGUID );

	static	void		ResetDailyQuest				( int iRoleID, int iType );

	// 對話介面用
	//-------------------------------------------------------------------------------------
	virtual void		On_LoadQuestSpeakOption		( BaseItemObject*	pObj, int iNPCID ) = 0;
		static void			SpeakOption				( int iRoleID );

	virtual void		On_ChoiceOption				( BaseItemObject*	pObj, int iOption ) = 0;
		static void			ChoiceOptionResult		( int iRoleID );

		static void			CloseSpeak				( int iRoleID );

	virtual void		On_DelayPatrol				( BaseItemObject*	pObj, int iNPCID, int iTime ) = 0;


	// 石碑介面
	//-------------------------------------------------------------------------------------
	static		void	ShowBorder					( int iCliID, int iQuestID, const char* pszTile, const char* pszTexture );
	static		void	ClearBorder					( int iCliID );
	static		void	AddBorderPage				( int iCliID, const char* pszPage );

	virtual		void	On_CloseBorder				( BaseItemObject*	pObj  ) = 0;
	virtual		void	On_AcceptBorderQuest		( BaseItemObject*	pObj  ) = 0;








	// 劇情
	//-------------------------------------------------------------------------------------
	static	void		PlayMotion					( int iRoleID, const char* pszMotion );
	static	void		ScriptCast					( int iRoleID, const char* pszString, int iTime, int iMotionID, int iMotionEndID );
	static	void		ScriptCastResult			( int iRoleID, int iResult );

	// 從 Server 處理中取得的事件
	//-------------------------------------------------------------------------------------
	
	

	static	void		GiveRoleItem				( int iRoleID, int iItemID, int iItemVal )		{};
	static	void		GiveRoleMoney				( int iRoleID, int iMomney )					{};
	static	void		GiveRoleExp					( int iRoleID, int iExp )						{};
	static	void		GiveRoleRep					( int iRoleID, int iRep )						{};
	static	void		GiveRoleSpell				( int iRoleID, int iSpell )						{};

	static	void		SetRoleFlag					( int iRoleID, int iFlag, int iVal )			{};

	static	void		SetQuestFlag				( BaseItemObject* pObj, int iQuestFlagID, int iVal );
	static	void		SetQuestState				( int iRoleID, int iQuestID, int iQuestState );


	static	void		AttachCastMotionToolID		( int iRoleID, int iToolID );
	static	void		AttachTool					( int iRoleID, int iToolID, const char* cszLinkID );

	static	void		StartClientCountDown		( int iRoleID, int iStartNumber, int iEndNumber, int iOffset, int iMode, int iShowStatusBar, const char* Str );

	
	// 資料
	//-------------------------------------------------------------------------------------
	static CNetSrv_Script*		m_pThis;
	static bool					m_pTempResult;

};
