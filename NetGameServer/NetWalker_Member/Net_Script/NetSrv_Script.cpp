#include "./NetSrv_Script.h"
#include "../Net_ServerList/Net_ServerList_Child.h"

#define Register_LuaFunc_NetSrv_Script

//#define Register_LuaFunc_NetSrv_Script
//	LuaRegisterFunc( "Lua_Quest_CheckRoleItem"		, bool( int, int, int )		, CNetSrv_Script::Lua_Quest_CheckRoleItem	);\
//	LuaRegisterFunc( "Lua_Quest_DeleteItem"			, bool( int, int, int )     , CNetSrv_Script::Lua_Quest_DeleteItem		);
	// LuaRegisterFunc( "DelFromPartition"	, bool( int )                           , DelFromPartition		);


// 靜態成員宣告
//-------------------------------------------------------------------
CNetSrv_Script*		CNetSrv_Script::m_pThis = NULL;
bool				CNetSrv_Script::m_pTempResult = false;

//-------------------------------------------------------------------
bool CNetSrv_Script::Init()
{
	Srv_NetCliReg( PG_Script_CtoL_QuestAccept			);
	Srv_NetCliReg( PG_Script_CtoL_QuestComplete			);	
	Srv_NetCliReg( PG_Script_CtoL_TouchEvent			);
	Srv_NetCliReg( PG_Script_CtoL_RangeEvent			);
	Srv_NetCliReg( PG_Script_CtoL_CollisionEvent		);
	Srv_NetCliReg( PG_Script_CtoL_ClickQuestObj			);
	Srv_NetCliReg( PG_Script_CtoL_QuestDelete			);
	Srv_NetCliReg( PG_Script_CtoL_LoadQuestSpeakOption	);
	Srv_NetCliReg( PG_Script_CtoL_ChoiceOption			);
	Srv_NetCliReg( PG_Script_CtoL_DelayPatrol			);
	Srv_NetCliReg( PG_Script_CtoL_AcceptBorderQuest		);
	Srv_NetCliReg( PG_Script_CtoL_CloseBorder			);
	
	return true;
}
//-------------------------------------------------------------------
void CNetSrv_Script::_PG_Script_CtoL_QuestAccept ( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_Script_CtoL_QuestAccept );
	M_CLIENT_OBJ( iCliID );
	
	m_pThis->On_QuestAccept( pObj, pPacket->iQuestNpcID, pPacket->iQuestID, false );
}
//-------------------------------------------------------------------
void CNetSrv_Script::_PG_Script_CtoL_QuestComplete ( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_Script_CtoL_QuestComplete );
	M_CLIENT_OBJ( iCliID );

	m_pThis->On_QuestComplete( pObj, pPacket->iQuestNpcID, pPacket->iQuestID, pPacket->iChoiceID );
}
//-------------------------------------------------------------------
void CNetSrv_Script::_PG_Script_CtoL_QuestDelete			( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_Script_CtoL_QuestDelete );
	M_CLIENT_OBJ( iCliID );

	m_pThis->On_QuestDelete( pObj, pPacket->iQuestField, pPacket->iQuestID );
}
//-------------------------------------------------------------------
void CNetSrv_Script::_PG_Script_CtoL_TouchEvent	( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_Script_CtoL_TouchEvent );
	M_CLIENT_OBJ( iCliID );

	m_pThis->On_TouchEvent( pObj, pPacket->iEventObjID );
}
//-------------------------------------------------------------------
void CNetSrv_Script::_PG_Script_CtoL_RangeEvent	( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_Script_CtoL_RangeEvent );
	M_CLIENT_OBJ( iCliID );

	m_pThis->On_RangeEvent( pObj, pPacket->iEventObjID );
}
//-------------------------------------------------------------------
void CNetSrv_Script::_PG_Script_CtoL_CollisionEvent		( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_Script_CtoL_CollisionEvent );
	M_CLIENT_OBJ( iCliID );

	m_pThis->On_CollisionEvent( pObj, pPacket->iEventObjID );
}
//-------------------------------------------------------------------
void CNetSrv_Script::_PG_Script_CtoL_ClickQuestObj		( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_Script_CtoL_ClickQuestObj );
	M_CLIENT_OBJ( iCliID );

	m_pThis->On_ClickQuestObj( pObj, pPacket->iQuestField, pPacket->iQuestID, pPacket->iClickObjID, pPacket->iClickObj );
}
//-------------------------------------------------------------------
void CNetSrv_Script::_PG_Script_CtoL_LoadQuestSpeakOption( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_Script_CtoL_LoadQuestSpeakOption );
	M_CLIENT_OBJ( iCliID );

	m_pThis->On_LoadQuestSpeakOption( pObj, pPacket->iNPCID );
}
//-------------------------------------------------------------------
void CNetSrv_Script::_PG_Script_CtoL_ChoiceOption( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_Script_CtoL_ChoiceOption );
	M_CLIENT_OBJ( iCliID );

	m_pThis->On_ChoiceOption( pObj, pPacket->iOptionID );
}
//-------------------------------------------------------------------
void CNetSrv_Script::_PG_Script_CtoL_DelayPatrol( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_Script_CtoL_DelayPatrol );
	M_CLIENT_OBJ( iCliID );

	m_pThis->On_DelayPatrol( pObj, pPacket->iObjID, pPacket->iTime );
}
//-------------------------------------------------------------------
void CNetSrv_Script::_PG_Script_CtoL_AcceptBorderQuest( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_Script_CtoL_AcceptBorderQuest );
	M_CLIENT_OBJ( iCliID );

	m_pThis->On_AcceptBorderQuest( pObj );
}
//-------------------------------------------------------------------
void CNetSrv_Script::_PG_Script_CtoL_CloseBorder ( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_Script_CtoL_CloseBorder );
	M_CLIENT_OBJ( iCliID );

	m_pThis->On_CloseBorder( pObj );
}
//-------------------------------------------------------------------
void CNetSrv_Script::SetQuestFlag ( BaseItemObject* pObj, int iQuestFlagID, int iVal )
{
	PG_Script_LtoC_QuestFlag Packet;

	Packet.iQuestFlagID = iQuestFlagID;
	Packet.iVal			= iVal;

	Global::SendToCli_ByGUID( pObj->GUID() , sizeof( Packet ) , &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_Script::QuestAcceptResult ( int iSockID, int iQuestField, int iQuestID, EM_QUESTCHECK_RESULT emResult )
{
	PG_Script_LtoC_QuestAccept_Result Packet;

	Packet.iQuestField	= iQuestField;
	Packet.iQuestID		= iQuestID;
	Packet.emResult		= emResult;

	Global::SendToCli( iSockID, sizeof( Packet ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_Script::QuestCompleteResult ( int iSockID, int iQuestNPCID, int iQuestField, int iQuestID, EM_QUESTCHECK_RESULT emResult )
{
	PG_Script_LtoC_QuestComplete_Result Packet;

	Packet.iQuestNPCID	= iQuestNPCID;
	Packet.iQuestField	= iQuestField;
	Packet.iQuestID		= iQuestID;
	Packet.emResult		= emResult;

	Global::SendToCli( iSockID, sizeof( Packet ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_Script::QuestDeleteResult ( int iSockID, int iQuestField, int iQuestID, EM_QUESTCHECK_RESULT emResult )
{
	PG_Script_LtoC_QuestDelete_Result Packet;

	Packet.iQuestField	= iQuestField;
	Packet.iQuestID		= iQuestID;
	Packet.emResult		= emResult;

	Global::SendToCli( iSockID, sizeof( Packet ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_Script::ClickQuestObjFailed		( int iSockID )
{
	PG_Script_CtoL_ClickQuestObjFailed Packet;
	Global::SendToCli( iSockID, sizeof( Packet ), &Packet );
}
//-------------------------------------------------------------------
bool CNetSrv_Script::Lua_Quest_CheckRoleItem		( int iObjID, int iItemID, int iCount )
{
	BaseItemObject	*pObj		= Global::GetObj( iObjID );
	
	if( pObj )
	{
		RoleDataEx		*pRole	= pObj->Role();
		
		return pRole->Sc_CheckItemFromRole( iItemID, iCount );
	}

	return false;
}
//-------------------------------------------------------------------
bool CNetSrv_Script::Lua_Quest_DeleteItem		( int iObjID, int iItemID, int iCount )
{


	return false;
}
//-------------------------------------------------------------------
void CNetSrv_Script::ClickQuestObjResult ( int iRoleID, int iResult, int iItemID, int iItemVal )
{
	PG_Script_LtoC_ClickQuestObjResult Packet;

	Packet.iResult	= iResult;
	Packet.iItemID	= iItemID;
	Packet.iItemVal	= iItemVal;

	Global::SendToCli_ByGUID( iRoleID, sizeof( Packet ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_Script::KillObjNotity ( int iRoleID, int iQuestField, int iQuestID, int iDeadObjID )
{
	PG_Script_LtoC_KillQuestObjNotify Packet;

	Packet.iQuestField	= iQuestField;
	Packet.iQuestID		= iQuestID;
	Packet.iDeadObjID	= iDeadObjID;

	Global::SendToCli_ByGUID( iRoleID, sizeof( Packet ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_Script::GetQuestObjNotify ( int iRoleID, int iQuestField, int iQuestID, int iObjGUID )
{
	PG_Script_LtoC_GetQuestObjNotify Packet;

	Packet.iQuestField	= iQuestField;
	Packet.iQuestID		= iQuestID;
	Packet.iObjGUID		= iObjGUID;

	Global::SendToCli_ByGUID( iRoleID, sizeof( Packet ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_Script::PlayMotion	 ( int iRoleID, const char* pszMotion )
{
	PG_Script_LtoC_PlayMotion Packet;

	Packet.iObjID		= iRoleID;
	//Packet.iMotionID	= iMotionID;
	strcpy( Packet.szMotion, pszMotion );

	Global::SendToCli_ByGUID( iRoleID, sizeof( Packet ), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_Script::SpeakOption ( int iRoleID )
{
	BaseItemObject* pObj	= BaseItemObject::GetObj( iRoleID );
	RoleDataEx*		pRole	= pObj->Role();
	if( pRole )
	{
		char				szBuffer[ _MAX_SPEAKDETAIL_SIZE_ + 1024 ];

		int					iOptionCount	= pRole->PlayerTempData->SpeakInfo.SpeakOption.Size();
		int					iszLen			= 0;
		int					iIconID			= 0;
		GamePGCommandEnum	Command			= EM_PG_Script_LtoC_SpeakOption;
		char*				ptr				= szBuffer;

		memcpy( (PVOID)ptr, &Command, sizeof( GamePGCommandEnum ) );
		ptr += sizeof( GamePGCommandEnum );

		// NPC ID
		memcpy( ptr, &pRole->PlayerTempData->SpeakInfo.iSpeakNPCID, sizeof(int) );
		ptr		+= sizeof( int );

		// LOAD QUEST
		memcpy( ptr, &pRole->PlayerTempData->SpeakInfo.iLoadQuest, sizeof(int) );
		ptr		+= sizeof( int );

		// OPTION COUNT
		memcpy( ptr, &iOptionCount, sizeof(int) );
		ptr		+= sizeof( int );

		// DETAIL LEN
		iszLen = (int)strlen( pRole->PlayerTempData->SpeakInfo.SpeakDetail.Begin() );
		if( iszLen != 0 )
		{
			iszLen += 1;
		}
		memcpy( ptr, &iszLen, sizeof(int) );
		ptr		+= sizeof( int );
		
		// DETAIL STRING
		memcpy( ptr, pRole->PlayerTempData->SpeakInfo.SpeakDetail.Begin(), iszLen );
		ptr += iszLen;

		if( iOptionCount != 0 )
		{					

			for( int i = 0; i < iOptionCount; i++ )
			{

				iIconID = pRole->PlayerTempData->SpeakInfo.SpeakOption[ i ].iIconID;
				memcpy( ptr, &iIconID, sizeof(int) );
				ptr += sizeof( int );

				iszLen = (int)strlen( pRole->PlayerTempData->SpeakInfo.SpeakOption[ i ].szOption.Begin() ) + 1;
				memcpy( ptr, &iszLen, sizeof(int) );
				ptr += sizeof( int );

				memcpy( ptr, pRole->PlayerTempData->SpeakInfo.SpeakOption[ i ].szOption.Begin(), iszLen );
				ptr += iszLen;
			}

		}

		int iSize = (int)( ptr - szBuffer );

		// 發送封包
		Global::SendToCli_ByGUID( iRoleID, iSize, szBuffer );

	}
}
//-------------------------------------------------------------------
void CNetSrv_Script::ChoiceOptionResult( int iRoleID )
{
	PG_Script_LtoC_ChoiceOptionResult Packet;

	Global::SendToCli_ByGUID( iRoleID, sizeof(Packet), &Packet );

}
//-------------------------------------------------------------------
void CNetSrv_Script::CloseSpeak( int iRoleID )
{
	PG_Script_LtoC_CloseSpeak Packet;
	Global::SendToCli_ByGUID( iRoleID, sizeof(Packet), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_Script::SetQuestState( int iRoleID, int iQuestID, int iQuestState )
{
	PG_Script_LtoC_SetQuestState Packet;

	Packet.iQuestID		= iQuestID;
	Packet.iState		= iQuestState;

	Global::SendToCli_ByGUID( iRoleID, sizeof(Packet), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_Script::ShowBorder ( int iCliID, int iQuestID, const char* pszTitle, const char* pszTexture )
{
	PG_Script_LtoC_ShowBorder Packet;

	Packet.iQuestID		= iQuestID;
	Packet.szTitle		= pszTitle;
	Packet.szTexture	= pszTexture;

	Global::SendToCli_ByGUID( iCliID, sizeof(Packet), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_Script::ClearBorder ( int iCliID )
{
	PG_Script_LtoC_ClearBorder Packet;
	Global::SendToCli_ByGUID( iCliID, sizeof(Packet), &Packet );
}
//-----------------------------------------------------------------------------
void CNetSrv_Script::AddBorderPage ( int iCliID, const char* pszPage )
{
	//PG_Script_LtoC_AddBorderPage Packet;
	
	int		iSize	= int( sizeof(int) + strlen( pszPage ) + 1 );
	BYTE*	pData	= NEW BYTE[ iSize ];
	BYTE*	ptr		= pData;
	
	GamePGCommandEnum Command = EM_PG_Script_LtoC_AddBorderPage;
	
	memcpy( ptr, &Command, sizeof( GamePGCommandEnum ) );
	ptr += sizeof( GamePGCommandEnum );

	memcpy( ptr, pszPage, ( strlen( pszPage ) + 1 ) );
	ptr += ( strlen( pszPage ) + 1 );


	Global::SendToCli_ByGUID( iCliID, iSize, pData );

	delete [] pData;



	//PG_Script_LtoC_AddBorderPage Packet;
	//Packet.sPageText = pszPage;
	//Global::SendToCli_ByGUID( iCliID, sizeof(Packet), &Packet );
}
//-----------------------------------------------------------------------------
void CNetSrv_Script::ScriptCast ( int iRoleID, const char* pszString, int iTime, int iMotionID, int iMotionEndID )
{
	PG_Script_LtoC_ScriptCast Packet;

	Packet.szString		= pszString;
	Packet.iTime		= iTime;
	Packet.iMotionID	= iMotionID;
	Packet.iMotionEndID	= iMotionEndID;

	Global::SendToCli_ByGUID( iRoleID, sizeof(Packet), &Packet );
}
//-----------------------------------------------------------------------------
void CNetSrv_Script::ScriptCastResult ( int iRoleID, int iResult )
{
	PG_Script_LtoC_ScriptCastResult Packet;

	Packet.iResult		= iResult;

	Global::SendToCli_ByGUID( iRoleID, sizeof(Packet), &Packet );
}

void CNetSrv_Script::ResetDailyQuest( int iRoleID, int iType )
{
	PG_Script_LtoC_ResetDailyQuest Packet;

	Packet.iType = iType;

	Global::SendToCli_ByGUID( iRoleID, sizeof(Packet), &Packet );
}
//-----------------------------------------------------------------------------
void CNetSrv_Script::AttachCastMotionToolID		( int iRoleID, int iToolID )
{
	PG_Script_LtoC_AttachCastMotionToolID	Packet;

	Packet.iToolID	= iToolID;
	Packet.iObjID	= iRoleID;

	BaseItemObject* pObj = BaseItemObject::GetObj( iRoleID );

	if( pObj == NULL )	
		return;

	RoleDataEx* Owner = pObj->Role();

	PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

	int			i , j;
	PlayID*		TopID;

	for( i = 0 ; Block[i] != NULL ; i++ )
	{
		TopID = Block[i]->Begin;

		for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
		{
			BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );

			//if( OtherObj == NULL || OtherObj->GUID() == pObj->GUID() )
			//	continue;

			if( OtherObj == NULL )
				continue;

			if( OtherObj->Role()->IsNPC() != false )
				continue;

			Global::SendToCli_ByGUID( OtherObj->GUID() , sizeof( Packet ) , &Packet );
		}
	}



	//Global::SendToCli_ByGUID( iRoleID, sizeof(Packet), &Packet );
}

void CNetSrv_Script::AttachTool( int iRoleID, int iToolID, const char* cszLinkID )
{
	PG_Script_LtoC_AttachTool	Packet;

	Packet.iToolID	= iToolID;
	Packet.LinkID	= cszLinkID;
	Packet.iObjID	= iRoleID;


	BaseItemObject* pObj = BaseItemObject::GetObj( iRoleID );

	if( pObj == NULL )	
		return;

	RoleDataEx* Owner = pObj->Role();

	PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

	int			i , j;
	PlayID*		TopID;

	for( i = 0 ; Block[i] != NULL ; i++ )
	{
		TopID = Block[i]->Begin;

		for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
		{
			BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );

			//if( OtherObj == NULL || OtherObj->GUID() == pObj->GUID() )
			//	continue;

			if( OtherObj == NULL )
				continue;

			if( OtherObj->Role()->IsNPC() != false )
				continue;

			Global::SendToCli_ByGUID( OtherObj->GUID() , sizeof( Packet ) , &Packet );
		}
	}
}
//-----------------------------------------------------------------------------
void CNetSrv_Script::StartClientCountDown		( int iRoleID, int iStartNumber, int iEndNumber, int iOffset, int iMode, int iShowStatusBar, const char* Str )
{
	PG_Script_LtoC_StartClientCountDown	Packet;

	Packet.iStartNumber   = iStartNumber;
	Packet.iEndNumber	  = iEndNumber;
	Packet.iOffset		  = iOffset;
	Packet.iMode		  = iMode;
	Packet.iShowStatusBar = iShowStatusBar;
	Packet.szText         = Str;

	Global::SendToCli_ByGUID( iRoleID, sizeof(Packet), &Packet );
}
//-----------------------------------------------------------------------------