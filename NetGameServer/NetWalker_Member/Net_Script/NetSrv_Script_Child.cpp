#include "../Net_Talk/NetSrv_Talk.h"

#include "NetSrv_Script_Child.h"
#include "../Net_ServerList/Net_ServerList_Child.h"
#include "../../mainproc/pathmanage/NPCMoveFlagManage.h"
#include "../Net_Gather/NetSrv_Gather_Child.h"
#include "../../MainProc/partyinfolist/PartyInfoList.h"
#include "../../MainProc/GuildManage/GuildManage.h"
#include "../net_DepartmentStore/NetSrv_DepartmentStoreChild.h"
//-------------------------------------------------------------------
bool    CNetSrv_Script_Child::Init()
{
	CNetSrv_Script::Init();

	if( m_pThis != NULL)
		return false;

	m_pThis = NEW( CNetSrv_Script_Child );

	return true;
}

//-------------------------------------------------------------------
bool    CNetSrv_Script_Child::Release()
{
	if( m_pThis == NULL )
		return false;

	delete m_pThis;
	m_pThis = NULL;

	CNetSrv_Script::Release();
	return false;
}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_QuestAccept ( BaseItemObject*	pObj, int iQuestNpcID, int iQuestID, bool bItemMode )
{
	RoleDataEx				*pRole			= pObj->Role();

	BaseItemObject			*pObj_QuestNPC	= NULL;
	RoleDataEx				*pQuestNPC		= NULL;	
	int						iQuestField		= -1;	
	EM_QUESTCHECK_RESULT	emResult;

	GameObjDbStruct*		pGameObj		= g_ObjectData->GetObj( iQuestID );
	GameObjectQuestDetail	*pQuest			= NULL;

	if( pGameObj )
		pQuest			= &pGameObj->QuestDetail;
	else
		return;


	if( pRole->TempData.Sys.OnChangeZone != false )
	{
		return;
	}

	// 非物品模式
		if( bItemMode == false )
		{
			// 檢查任務 NPC 在不在
				pObj_QuestNPC	= BaseItemObject::GetObj( iQuestNpcID );

				if( pObj_QuestNPC )
				{
					pQuestNPC = pObj_QuestNPC->Role();		
				}
				else
				{
					emResult	= EM_QUESTCHECK_RESULT_NOTEXIST;
					QuestAcceptResult( pObj->SockID(), iQuestField, iQuestID, emResult );
					return;
				}
			// 檢查任務 NPC 是否擁有該 QUEST
				{
					int					iObjGUID	= pQuestNPC->BaseData.ItemInfo.OrgObjID;
					GameObjDbStruct*	pNPCObj		= g_ObjectData->GetObj( iObjGUID );
					if( pNPCObj )
					{
						bool bFound = false;
						for( int i=0; i<_MAX_NPC_QUEST_COUNT_; i++ )
						{
							if( pNPCObj->NPC.iQuestID[i] == iQuestID )
							{
								bFound = true;
								break;
							}
						}

						if( bFound == false )
						{
							QuestAcceptResult( pObj->SockID(), iQuestField, iQuestID, EM_QUESTCHECK_RESULT_NOTEXIST );
							return;
						}
					}
					else
					{
						QuestAcceptResult( pObj->SockID(), iQuestField, iQuestID, EM_QUESTCHECK_RESULT_NOTEXIST );
						return;
					}
				}

			// 檢查與任務 NPC 距離
				float fRange = pRole->Length( pQuestNPC );

				if( fRange > ( _MAX_ROLE_TOUCH_RANGE_ * 2 ) )
				{
					QuestAcceptResult( pObj->SockID(), iQuestField, iQuestID, EM_QUESTCHECK_RESULT_TOOFAR );
					return;

				}

			// 檢查任務條件是否達到
				if( pRole->CheckQuest_IsAvailable( iQuestID, pQuest, true ) != EM_QUESTCHECK_RESULT_OK )
				{
					return;
				}

			// 檢查任務給予物品的空間是否足夠
				int iCountQuestItem = 0;
				for( int index = 0; index < _MAX_SIZE_QUEST_ITEM; index++ )
				{
					int iItemID = pQuest->iAccept_ItemID[ index ];
					if( iItemID != 0 )
					{
						GameObjDbStructEx*		pObj		= g_ObjectData->GetObj( iItemID );
						
						if( pObj != NULL && pObj->IsKeyItem() == false && pObj->IsTitle() == false )
						{
							iCountQuestItem++;
						}						
					}
					else
						break;
				}

				if( pRole->GetBodyFreeSlot() < iCountQuestItem )
				{
					NetSrv_Talk::ScriptMessage( pObj->GUID(), pObj->GUID(), 0, "[QUEST_MSG_ACCEPT_SLOTFULL]", 0 );
					NetSrv_Talk::ScriptMessage( pObj->GUID(), pObj->GUID(), 2, "[QUEST_MSG_ACCEPT_SLOTFULL]", 0 );
					return;
				}

		}

	// 檢查是否重覆任務, 若不是則不可重覆接已完成過的任務
		if( pQuest->iCheck_Loop == false )
		{
			if( pRole->CheckQuestHistory_IsComplete( iQuestID ) == true )
			{
				QuestAcceptResult( pObj->SockID(), iQuestField, iQuestID, EM_QUESTCHECK_RESULT_ALREADYDONE );
				return;
			}
		}


	// 檢查目前是否擁有該 Quest
		if( pRole->CheckQuestList_IsGotQuest( iQuestID ) != -1 )
		{
			QuestAcceptResult( pObj->SockID(), iQuestField, iQuestID, EM_QUESTCHECK_RESULT_ALREADYEXIST );
			return;

		}

	// 檢查是否還有空間可已新加任務
		int iQuestSize = pRole->Quest_Size();
		if( iQuestSize >= _MAX_QUEST_COUNT_ )
		{
			QuestAcceptResult( pObj->SockID(), iQuestField, iQuestID, EM_QUESTCHECK_RESULT_FULL );
			return;
		}

	
	// 更新 Quest 清單
	//------------------------------------------------------------------------------------
		iQuestField = pRole->Quest_Add( iQuestID );

		// 檢查有無打倒目標及給予物品設定

		// 給予物品
		if( pQuest->iAccept_ItemID[ 0 ] != 0 )
		{
			for( int index = 0; index < _MAX_SIZE_QUEST_ITEM; index++ )
			{
				int iItemID = pQuest->iAccept_ItemID[ index ];
				if( iItemID != 0 )
				{
					pRole->Sc_GiveItemToBuf( iItemID, pQuest->iAccept_ItemVal[ index ]  , EM_ActionType_PlotGive , NULL , "" );		// Quest Accept	

					char szStr[256];
					sprintf( szStr, "給予物品[ %d ][ %d ]個",iItemID, pQuest->iAccept_ItemVal[ index ]  );
					NetSrv_Talk::SysMsg( pObj->GUID(),szStr );

				}
				else
					break;
			}
		}


			// 檢查及設定打倒目標
			//pRole->CheckQuest_RequestKill_SetTempData( iQuestField, pQuest );

			// 檢查及設定打倒目標後掉落物品
			//pRole->CheckQuest_KillToGetItem_SetTempData( iQuestField, pQuest );

		// 檢查是否有給予物品


		//  執行劇情
		//------------------------------------------------------------------------------
			if( strlen( pQuest->szScript_OnAccept ) != 0 )
			{
				//vector< MyVMValueStruct > vecArg;
				//LUA_VMClass::PCallByStrArg( pQuest->szScript_OnClickObjEnd, iGUID, iClickObjID, &vecArg );
				//void	CallLuaFunc(string FuncName,int Target,unsigned int ExeTime = 0 , PlotRunTimeEvent* Listen = NULL );
				//pObj->PlotVM()->CallLuaFunc( pQuest->szScript_OnAccept, pObj->GUID() , 0 );
				if( pObj_QuestNPC != NULL )
				{
					char* pszLua = pQuest->szScript_OnAccept;									

					if( pszLua[0] != '@' )
					{
						pObj_QuestNPC->PlotVM()->CallLuaFunc( pszLua, pObj->GUID() , 0 );
					}
					else
					{
						pszLua++;
						LUA_VMClass::PCallByStrArg( pszLua, pObj_QuestNPC->GUID() , pObj->GUID() );
					}
				}
			}

	Global::Log_Quest( pObj->Role(), iQuestID, 0 );
	QuestAcceptResult( pObj->SockID(), iQuestField, iQuestID, EM_QUESTCHECK_RESULT_OK );
	
}
//-------------------------------------------------------------------
int CNetSrv_Script_Child::CheckCompleteLoopQuest ( int iQuestID, GameObjectQuestDetail* pQuest, RoleDataEx* pRole )
{
	
	__time32_t				LastResetTime;
	__time32_t				NowTime;
	struct	tm*				gmt;
	bool					bReset			= false;
	time_t*					pLastTime		= &( pRole->PlayerBaseData->Quest.LastCompleteTime );

	EM_QUESTCHECK_RESULT	emResult		= EM_QUESTCHECK_RESULT_OK;

	_time32( &NowTime );
	gmt = _localtime32( &NowTime );
	NowTime = (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60;

	//計算前天重置時間的秒數 ( 目前預設是早上 06:00 )

	if( gmt->tm_hour < DF_DAILY_RESET_TIME )
	{
		gmt->tm_hour	= DF_DAILY_RESET_TIME;// + RoleDataEx::G_TimeZone;
		gmt->tm_sec		= 0;
		gmt->tm_min		= 0;
		gmt->tm_mday	= gmt->tm_mday;
		LastResetTime	= (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60 - ( 24*60*60);
	}
	else
	{
		gmt->tm_hour	= DF_DAILY_RESET_TIME;// + RoleDataEx::G_TimeZone;
		gmt->tm_sec		= 0;
		gmt->tm_min		= 0;
		LastResetTime	= (int)_mktime32( gmt )+ RoleDataEx::G_TimeZone*60*60;
	}

	if( *pLastTime != 0 && *pLastTime <= LastResetTime && Global::Ini()->IsBattleWorld == false )
	{
		bReset = true;	// 超過時間, 可以完成
	}

	if( bReset == true )
	{
		int diff = abs( (int)( LastResetTime - *pLastTime ) );
		*pLastTime = (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60;

		Print( LV2, "ResetDaily", "CheckCompleteLoopQuest - account[ %s ] pLastTime[ %d ] diff[ %d ] ", pRole->Base.Account_ID.Begin(), *pLastTime, diff );

		pRole->PlayerBaseData->Quest.QuestDaily.ReSet();
		for( int i=0; i<DF_QUESTDAILY_MAXGROUP; i++ )
		{
			pRole->PlayerBaseData->Quest.iQuestGroup[i] = 0;
		}

		ResetDailyFlag( pRole, false );

		*pLastTime = (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60;

		pRole->PlayerTempData->Log.ActionMode.DailyQuest	= true;

		return EM_QUESTCHECK_RESULT_OK; // 可以完成任務
	}
	else
	{
		// 檢查完成是否超過
		// 每日任務
			if( emResult == EM_QUESTCHECK_RESULT_OK && pQuest->iControlType == 1 ) // 每日完成一次
			{
				int iID = iQuestID - Def_ObjectClass_QuestDetail;
				if( iID >= 0 && _MAX_QUEST_FLAG_COUNT_ > iID )
				{
					if( pRole->PlayerBaseData->Quest.QuestDaily.GetFlag( iID ) == true )
					{
						// 已經完成過
						emResult = EM_QUESTCHECK_RESULT_DAILYDONE;
					}			
				}
				else
				{
					emResult = EM_QUESTCHECK_RESULT_QUESTERR;
				}
			}

		// 群組限定
			if( emResult == EM_QUESTCHECK_RESULT_OK && pQuest->iQuestGroup != 0 ) // 每個群組限定一天只能完成 10 個任務
			{
				int iDailyLimit = 10;

				if( pQuest->iQuestGroup == 1 ) // 公會任務
				{
					int iTotalLV = (int)g_ObjectData->_GuildLvTable.size();
					GuildStruct* pGuild = GuildManageClass::This()->GetGuildInfo( pRole->BaseData.GuildID );

					if( pGuild != NULL && ( pGuild->Base.Level >= 1 && pGuild->Base.Level < iTotalLV  ) )
					{
						iDailyLimit = g_ObjectData->_GuildLvTable[ pGuild->Base.Level ].Quest;
					}
					else
					{
						emResult = EM_QUESTCHECK_RESULT_QUESTERR;
						return emResult;
					}
				}


				if( pQuest->iQuestGroup != 3 ) // public quest doesnt count
				{
					int iID = pQuest->iQuestGroup -1;
					if( iID >= 0 && iID < DF_QUESTDAILY_MAXGROUP )
					{
						if( pRole->PlayerBaseData->Quest.iQuestGroup[ iID ] >= iDailyLimit )
						{
							emResult = EM_QUESTCHECK_RESULT_GROUPFULL;
						}
					}
					else
					{
						emResult = EM_QUESTCHECK_RESULT_QUESTERR;
					}				
				}
			}		
	}

	return emResult;
}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_QuestComplete	( BaseItemObject*	pObj, int iQuestNpcID, int iQuestID, int iChoiceID )
{
	RoleDataEx				*pRole			= pObj->Role();
	BaseItemObject			*pObj_QuestNPC	= BaseItemObject::GetObj( iQuestNpcID );
	RoleDataEx				*pQuestNPC		= NULL;
	int						iQuestField		= -1;	
	EM_QUESTCHECK_RESULT	emResult		= EM_QUESTCHECK_RESULT_OK;

	if( pObj_QuestNPC )
	{
		pQuestNPC	= pObj_QuestNPC->Role();		
	}
	else
	{
		emResult	= EM_QUESTCHECK_RESULT_NOTEXIST;
	}

	if( pRole->TempData.Sys.OnChangeZone != false )
	{
		return;
	}

	if( pQuestNPC )
	{

		float fRange = pRole->Length( pQuestNPC );

		if( fRange <= ( _MAX_ROLE_TOUCH_RANGE_ * 2 ) )
		{
			// 先確定擁有該 Quest

			iQuestField = pRole->CheckQuestList_IsGotQuest( iQuestID );

			if( iQuestField != -1 )
			{
				GameObjDbStructEx*		pDataObj	= Global::GetObjDB( iQuestID );
				
				if( pDataObj == NULL )	return;				
					
				GameObjectQuestDetail *pQuest = &( pDataObj->QuestDetail );


				// 檢查任務 NPC 是不是忙錄中
				if( pQuestNPC->IsDisableQuest() == true )
				{
					// 任務完成前執行劇情, 於劇情內發出完成任務
					emResult = EM_QUESTCHECK_RESULT_RUNNINGSCRIPT;
					QuestCompleteResult( pObj->SockID(), iQuestNpcID, iQuestField, iQuestID, EM_QUESTCHECK_RESULT_RUNNINGSCRIPT );
					return;
				}
				
				// 檢查重複任務設定
				//--------------------------------------------------------------------
				
					emResult = ( EM_QUESTCHECK_RESULT )pRole->CheckQuest_IsComplete( iQuestField, iQuestID, pQuest );

					if( pQuest->iCheck_Loop == true && emResult == EM_QUESTCHECK_RESULT_OK )
					{
						emResult = ( EM_QUESTCHECK_RESULT )CheckCompleteLoopQuest( iQuestID, pQuest, pRole );
					}

					if( emResult != EM_QUESTCHECK_RESULT_OK )
					{
						QuestCompleteResult( pObj->SockID(), iQuestNpcID, iQuestField, iQuestID, emResult );
						return;
					}



				// 檢查該任務是否為指定另一NPC完成.若是則檢查
					if( pQuest->iRequest_QuestID != 0 && pQuest->iRequest_QuestID != pQuestNPC->BaseData.ItemInfo.OrgObjID )
					{
						return;
					}


				// 檢查是否還有空間可以收取獎賞物品及下階段的任務物品
					int iCountRewardItem = 0;
					if( pQuest->iReward_ItemID[ 0 ] != 0 )
					{
						for( int index = 0; index < _MAX_SIZE_QUEST_ITEM; index++ )
						{
							int						iItemID		= pQuest->iReward_ItemID[ index ];						
							if( iItemID != 0 )
							{
								GameObjDbStructEx*		pDataObj	= Global::GetObjDB( iItemID );								
								if( pDataObj != NULL && pDataObj->IsKeyItem() == false && pDataObj->IsTitle() == false )
									iCountRewardItem++;
							}
							else
								break;
						}
					}

					if( iChoiceID != -1 )
					{
						if( iChoiceID <= _MAX_SIZE_QUEST_ITEM && pQuest->iReward_ChoiceID[ iChoiceID ] != 0 )
						{
							GameObjDbStructEx*		pDataObj	= Global::GetObjDB( pQuest->iReward_ChoiceID[ iChoiceID ] );								
							if( pDataObj != NULL && pDataObj->IsKeyItem() == false && pDataObj->IsTitle() == false )
								iCountRewardItem++;
						}
					}

					if( pRole->GetBodyFreeSlot() < iCountRewardItem )
					{
						NetSrv_Talk::ScriptMessage( pObj->GUID(), pObj->GUID(), 0, "[QUEST_MSG_FINISH_SLOTFULL]", 0 );
						NetSrv_Talk::ScriptMessage( pObj->GUID(), pObj->GUID(), 2, "[QUEST_MSG_FINISH_SLOTFULL]", 0 );
						return;
					}

				// 檢查是否可以完成任務
				//--------------------------------------------------------------------
				if( emResult == EM_QUESTCHECK_RESULT_OK )
				{
					//Packet.emResult
					int iQuestLV = pQuest->iCheck_LV;

					
					// 檢查是否有給予物品及金錢名聲 ... 先發完成任務封包, 再處理給予物品

						

						// 刪除物品
						//-------------------------------------------------------------------------
							// 刪除 Money
								//--------------------------------------------------------------------
								if( pQuest->iRequest_Money != 0 )
								{
									if( pRole->Sc_GiveMoney( ( pQuest->iRequest_Money * -1 ) ) == true )
									{
										char szStr[256];
										//sprintf( szStr, "收回金錢[ %d ]",pQuest->iRequest_Money  );
										//NetSrv_Talk::SysMsg( pObj->GUID(),szStr );
									}
									else
									{
										QuestCompleteResult( pObj->SockID(), iQuestNpcID, iQuestField, iQuestID, EM_QUESTCHECK_RESULT_NOQUESTITEM );
										return;
									}
								}

							// 刪除物品
							//--------------------------------------------------------------------
								if( pQuest->iRequest_ItemID[ 0 ] != 0 )
								{
									for( int index = 0; index < _MAX_SIZE_QUEST_CONDITION; index++ )
									{
										int iItemID = pQuest->iRequest_ItemID[ index ];
										if( iItemID != 0 )
										{
											if( pRole->Sc_DelItemFromBody( iItemID, pQuest->iRequest_ItemVal[ index ] , EM_ActionType_PlotDestory ) == true )
											{
												char szStr[256];
												//sprintf( szStr, "刪除物品[ %d ][ %d ]個",iItemID, pQuest->iRequest_ItemVal[ index ]  );
												//NetSrv_Talk::SysMsg( pObj->GUID(),szStr );
											}
											else
											{
												QuestCompleteResult( pObj->SockID(), iQuestNpcID, iQuestField, iQuestID, EM_QUESTCHECK_RESULT_NOQUESTITEM );
												return;
											}
										}
										else
											break;
									}
								}

								for( int index = 0; index < _MAX_SIZE_QUEST_ITEM; index++ )
								{
									int iItemID		= pQuest->iDeleteQuestItem[ index ];
									int iItemCount	= pQuest->iDeleteQuestItemCount[ index ];

									if( iItemID != 0 )
									{
										int iBodyCount = pRole->Sc_GetItemNumFromRole( iItemID );

										if( iItemCount == 0 || iItemCount > iBodyCount )
										{
											iItemCount = iBodyCount;
										}

										pRole->Sc_DelItem( iItemID, iItemCount, EM_ActionType_PlotDestory );
									}
								}



							// 若是重複型任務, 則累計
							//--------------------------------------------------------------------
								if( pQuest->iCheck_Loop != 0 )
								{
									//if( pRole->PlayerBaseData->Quest.LastCompleteTime == 0 )									
									{
										__time32_t				NowTime;
										struct	tm*				gmt;

										_time32( &NowTime );

										gmt			= _localtime32( &NowTime );
										NowTime		= NowTime = (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60;
										
										pRole->PlayerBaseData->Quest.LastCompleteTime = NowTime;
									}

									if( pQuest->iControlType == 1 )
									{
										int iID = iQuestID - Def_ObjectClass_QuestDetail;
										if( iID >= 0 && _MAX_QUEST_FLAG_COUNT_ > iID )
										{
											pRole->PlayerBaseData->Quest.QuestDaily.SetFlagOn( iID );
										}
									}

									if( pQuest->iQuestGroup != 0 )
									{
										int iID = pQuest->iQuestGroup -1;
										if( iID >= 0 && iID < DF_QUESTDAILY_MAXGROUP )
										{
											if( pQuest->iQuestGroup != 3 )
												pRole->PlayerBaseData->Quest.iQuestGroup[ iID ]++;
										}
									}
								}

						// 給予榮譽
						//------------------------------------------------------------------
							if( pQuest->iReward_Honor != 0 )
							{
								RoleDataEx*			pMember = NULL;
								PartyInfoStruct*	Party	= PartyInfoListClass::This()->GetPartyInfo( pRole->BaseData.PartyID );	

								if( Party != NULL )
								{
									int iPartySize = (int)Party->Member.size();

									for( int i = 0; i < iPartySize; i++ )
									{
										// 如果在不同區域, 則取不到 Member 資料
										pMember = Global::GetRoleDataByGUID( Party->Member[ i ].GItemID );

										if( pMember && pMember != pRole && pRole->Length( pMember ) <= _MAX_PARTY_SHARE_RANGE_ && pMember->BaseData.RoleName == Party->Info.LeaderName )	
										{
											//CheckQuestRoleDead( pMember, pDeader, pDropList, pOwnerList );
											if( pMember->TempData.Attr.AllZoneState.HonorParty == true )
											{
												pMember->PlayerBaseData->MedalCount += pQuest->iReward_Honor;
												pMember->Net_FixRoleValue( EM_RoleValue_MedalCount, pMember->PlayerBaseData->MedalCount );

												char szMsg[512];
												sprintf( szMsg, "[QUEST_MSG_LEADERGETHONOR][$SETVAR1=%d]", pQuest->iReward_Honor );
												NetSrv_Talk::ScriptMessage( pMember->GUID(), pMember->GUID(), 2, szMsg, 0 );
											}
										}
									}
								}
							}
						

						// 給予物品
						//--------------------------------------------------------------------
							if( pQuest->iReward_ItemID[ 0 ] != 0 )
							{
								for( int index = 0; index < _MAX_SIZE_QUEST_ITEM; index++ )
								{
									int iItemID = pQuest->iReward_ItemID[ index ];
									if( iItemID != 0 )
									{
										pRole->Sc_GiveItemToBodyFreeSlot( iItemID, pQuest->iReward_ItemVal[ index ] , EM_ActionType_PlotGive , NULL , "" );

										char szStr[256];
										//sprintf( szStr, "給予物品[ %d ][ %d ]個",iItemID, pQuest->iReward_ItemVal[ index ]  );
										//NetSrv_Talk::SysMsg( pObj->GUID(),szStr );
									}
									else
										break;
								}
								//NetSrv_Talk::SysMsg( pObj->SockID(), "給予物品" );
								
							}

						// 給予選擇的物品
						//--------------------------------------------------------------------
							if( iChoiceID != -1 )
							{
								if( iChoiceID <= _MAX_SIZE_QUEST_ITEM && pQuest->iReward_ChoiceID[ iChoiceID ] != 0 )
								{
									pRole->Sc_GiveItemToBuf(  pQuest->iReward_ChoiceID[ iChoiceID ],  pQuest->iReward_ChoiceVal[ iChoiceID] , EM_ActionType_PlotGive , NULL , "" );
								}
								//NetSrv_Talk::SysMsg( pObj->GUID(), "選擇物品" );
							}

						// 給予名聲
						//--------------------------------------------------------------------
							for( int i=0; i<2; i++ )
							{
								if( pQuest->iReward_RepGroup[i] != 0 )
								{
									int iRepGroup = pQuest->iReward_RepGroup[i];
									if( iRepGroup >= 1 && iRepGroup <= _MAX_FAMOUS_COUNT_ )
									{
										pRole->AddValue( (RoleValueName_ENUM)( (int)EM_RoleValue_Famous + ( iRepGroup - 1 ) ), pQuest->iReward_Rep[i] );
									}
								}
							}

						// 給予 Money
						//--------------------------------------------------------------------
							if( pQuest->iReward_Money != 0 )
							{
								pRole->AddPlotMoney( iQuestLV,  pQuest->iReward_Money * RoleDataEx::GlobalSetting.QuestMoneyRate );
							}

						// 給予 EXP
						//--------------------------------------------------------------------
							float BaseExpRate =( 1 + pRole->TempData.Attr.Mid.Body[EM_WearEqType_Daily_EXP_Rate]/100 );
							int iExp = 0;
							if( pQuest->iReward_Exp != 0 )
							{
								if( pQuest->iQuestGroup != 2 )
									BaseExpRate = 1;

								iExp = pRole->AddPlotExp( iQuestLV, int(pQuest->iReward_Exp * BaseExpRate) );
							}

						// 給予 TP
						//--------------------------------------------------------------------
							float BaseTpRate =( 1 + pRole->TempData.Attr.Mid.Body[EM_WearEqType_Daily_TP_Rate]/100 );
							if( pQuest->iReward_TP != 0 )
							{
								if( pQuest->iQuestGroup != 2 )
									BaseTpRate = 1;

								int iTP = (int)( iExp * ((float)pQuest->iReward_TP / 100) );
								if( iTP != 0 )
								{
									pRole->AddPlotTP( int(iTP * BaseTpRate * RoleDataEx::GlobalSetting.QuestTpRate) );
								}							
							}
						//給特殊貨幣
							Global::AddMoneyBase( pRole , EM_ActionType_PlotGive , pQuest->iReward_CoinType , pQuest->iReward_SpecialCoin );

						//扣除特殊貨幣
							Global::AddMoneyBase( pRole , EM_ActionType_PlotGive , pQuest->iRequest_CoinType , ( pQuest->iRequest_SpecialCoin * -1 ) );


							// 檢查 SCRIPT 狀態
							//if( pQuestNPC->PlayerTempData->ScriptInfo.emMode == EM_SCRIPT_RUNNING )

							{									
								Global::Log_Quest( pRole, iQuestID, 1 );
								pRole->Quest_Complete( iQuestField, iQuestID );
								
								// 檢查是否有完成任務劇情
								if( strlen( pQuest->szScript_OnComplete ) != 0 )
								{
									CPlotEventQuestComplete* pEvent = NEW( CPlotEventQuestComplete );

									pEvent->m_iRoleID		= pObj->GUID();
									pEvent->m_iNPCID		= pObj_QuestNPC->GUID();
									pEvent->m_iQuestID		= iQuestID;

									//pQuestNPC->PlayerTempData->ScriptInfo.emMode = EM_SCRIPT_RUNNING;
									pQuestNPC->SetDisableQuest( true );

									char* pszLua = pQuest->szScript_OnComplete;									
									
									if( pszLua[0] != '@' )
									{
										pObj_QuestNPC->PlotVM()->CallLuaFunc( pszLua, pObj->GUID() , 0, pEvent );
									}
									else
									{
										pszLua++;
										LUA_VMClass::PCallByStrArg( pszLua, pObj_QuestNPC->GUID() , pObj->GUID() );
									}
							
									
								}

								if( pObj != NULL )
								{
									char LuaScriptBuf[1024];
									sprintf_s( LuaScriptBuf , sizeof(LuaScriptBuf) , "Lua_System_QuestComplete( %d, %d )" , pObj->GUID() , iQuestID );
									LUA_VMClass::PCallByStrArg( LuaScriptBuf , pObj->GUID() ,pObj->GUID() );
								}

							}

						// 完成任務, 並設定 QuestFlag
							
					QuestCompleteResult( pObj->SockID(), iQuestNpcID, iQuestField, iQuestID, EM_QUESTCHECK_RESULT_OK );
					//QuestCompleteResult( pObj->SockID(), iQuestField, iQuestID, emResult );
					return;

				}
			}
			else
			{
				emResult = EM_QUESTCHECK_RESULT_NOTLIST;
			}
		
		}
		else
		{
			// 不正常
			emResult = EM_QUESTCHECK_RESULT_TOOFAR;
		}
	}



	QuestCompleteResult( pObj->SockID(), iQuestNpcID, iQuestField, iQuestID, emResult );

}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_QuestDelete ( BaseItemObject*	pObj, int iQuestField, int iQuestID )
{
	RoleDataEx				*pRole			= pObj->Role();
	//int						i_tQuestField	= pRole->CheckQuestList_IsGotQuest( iQuestID );
	EM_QUESTCHECK_RESULT	emResult		= EM_QUESTCHECK_RESULT_NOTOK;

	if( iQuestField != -1 )
	{		
		if( pRole->Quest_Delete( iQuestField, iQuestID ) == true )
		{
			emResult = EM_QUESTCHECK_RESULT_OK;
			Global::Log_Quest( pRole, iQuestID, 2 );

			GameObjDbStructEx*		pDataObj	= Global::GetObjDB( iQuestID );

			if( pDataObj == NULL )	
				return;				

			GameObjectQuestDetail *pQuest = &( pDataObj->QuestDetail );


			for( int index = 0; index < _MAX_SIZE_QUEST_ITEM; index++ )
			{
				int iItemID		= pQuest->iDeleteQuestItem[ index ];
				int iItemCount	= pQuest->iDeleteQuestItemCount[ index ];

				if( iItemID != 0 )
				{
					int iBodyCount = pRole->Sc_GetItemNumFromRole( iItemID );;

					if( iItemCount == 0 || iItemCount > iBodyCount )
					{
						iItemCount = iBodyCount;
					}

					pRole->Sc_DelItem( iItemID, iItemCount, EM_ActionType_PlotDestory );
				}
			}


		}
		else
		{
			emResult = EM_QUESTCHECK_RESULT_NOTOK;
		}	
	}

	QuestDeleteResult( pObj->SockID(), iQuestField, iQuestID, emResult );
}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_RoleDeadDropItem( RoleDataEx* pKiller, RoleDataEx* pDeader, vector< ItemFieldStruct >* pDropList, vector< int >* pOwnerList , vector< bool >* pLockList )
{
	if( pKiller == NULL || pDeader == NULL )
		return;

	GameObjDbStructEx		*pObj			= NULL;

	QuestStruct*			pQuestBaseData	= &( pKiller->PlayerBaseData->Quest );
	int						iDeadObjID		= pDeader->BaseData.ItemInfo.OrgObjID;
	GameObjectQuestDetail*	pQuest			= NULL;
	
	GameObjectNpcStruct*	pDeadObj	= NULL;


	
	pObj = g_ObjectData->GetObj( iDeadObjID );

	if( pObj->IsNPC() )
		pDeadObj = &( pObj->NPC );
	else
		return;


	// 使用遞迴檢查隊友
		RoleDataEx*			pMember = NULL;
		PartyInfoStruct*	Party	= PartyInfoListClass::This()->GetPartyInfo( pKiller->BaseData.PartyID );	

		if( Party != NULL )
		{
			int iPartySize = (int)Party->Member.size();
			
			for( int i = 0; i < iPartySize; i++ )
			{
				// 如果在不同區域, 則取不到 Member 資料
					pMember = Global::GetRoleDataByGUID( Party->Member[ i ].GItemID );

				if( pMember && pMember != pKiller && pKiller->Length( pMember ) <= _MAX_PARTY_SHARE_RANGE_ )	
				{
					CheckQuestRoleDead( pMember, pDeader, pDropList, pOwnerList , pLockList, true );
					
				}
			}
		}

	// 檢查自己
		CheckQuestRoleDead( pKiller, pDeader, pDropList, pOwnerList , pLockList, false );


}
//----------------------------------------------------------------------------------------
void CNetSrv_Script_Child::CheckQuestRoleDead ( RoleDataEx* pKiller, RoleDataEx* pDeader, vector< ItemFieldStruct >* pDropList, vector< int >* pOwnerList , vector< bool >* pLockList,bool bTeam )
{
	GameObjDbStructEx		*pObj			= NULL;

	QuestStruct*			pQuestBaseData	= &( pKiller->PlayerBaseData->Quest );
	int						iDeadObjID		= pDeader->BaseData.ItemInfo.OrgObjID;
	GameObjectQuestDetail*	pQuest			= NULL;

	GameObjectNpcStruct*	pDeadObj	= NULL;

	bool					bFullSlotMsg	= false;

	if( pKiller == NULL || pDeader == NULL )
		return;

	pObj = g_ObjectData->GetObj( iDeadObjID );

	if( pObj->IsNPC() )
		pDeadObj = &( pObj->NPC );
	else
		return;

	if( pDeadObj )
	{
		int iQuestID = 0;
		
		for( int i = 0; i < _MAX_QUESTLINK_COUNT; i++ )
		{
			//if( pDeadObj->pQuestLink[ i ] != NULL )

			iQuestID = pDeadObj->iQuestLinkObj[ i ];

			if( iQuestID != 0 )
			{
				//iQuestID = pDeadObj->pQuestLink[ i ]->GUID;

				// 檢查角色身上有無相關任務
				for( int j = 0; j < _MAX_QUEST_COUNT_; j++ )
				{
					if( pQuestBaseData->QuestLog[ j ].iQuestID == iQuestID )
					{
						//pQuest = &( pDeadObj->pQuestLink[ i ]->QuestDetail );
						GameObjDbStructEx	*pQuestObj = g_ObjectData->GetObj( iQuestID );
						
						if( pQuestObj == NULL || ( pQuestObj->QuestDetail.bDisableTeam == true && bTeam == true ) )
							return;

						pQuest = &( pQuestObj->QuestDetail );

						// 檢查打倒計數
						for( int k = 0; k < _MAX_SIZE_QUEST_ITEM; k++ )
						{
							if( pQuest->iRequest_KillID[ k ] == iDeadObjID )
							{
								// 取出目前資料, 檢查是否達成
								int		KillRequest	= pQuest->iRequest_KillVal[ k ];
								//byte&	KillVal		= pKiller->PlayerBaseData->Quest.iQuestKillCount[ j ][ k ];
								byte&	KillVal		= pKiller->PlayerBaseData->Quest.QuestLog[ j ].iQuestKillCount[ k ];
								
								if( KillRequest > KillVal )
								{
									// 通知 Client 計數殺敵數
									KillVal++;
									char szStr[256];
									sprintf( szStr, "打到任務目標計數 [ %d / %d ]",KillVal, KillRequest  );									
									NetSrv_Talk::SysMsg( pKiller->GUID(),szStr );

									CNetSrv_Script::KillObjNotity( pKiller->GUID(), j, iQuestID, iDeadObjID );
								}
							}
						}

						// 檢查打倒給予物品
						for( int k = 0; k < _MAX_SIZE_QUEST_CONDITION; k++ )
						{
							if( pQuest->iProcess_KilltoGetItem_ObjID[ k ] == iDeadObjID )
							{
								// 取出目前資料, 檢查給予物品數量是否足夠

								int		iDropPercent	= pQuest->iProcess_KilltoGetItem_ProcPercent[ k ];
								int		iDropItem		= pQuest->iProcess_KilltoGetItem_GetItem[ k ];
								int		iDropItemVal	= pQuest->iProcess_KilltoGetItem_GetItemVal[ k ];

								// 檢查角色身上及銀行所擁有的物品是否足夠
								bool	bEnough			= false;
								int		iLackNum		= iDropItemVal;

								for( int k = 0; k < _MAX_SIZE_QUEST_CONDITION; k++ )
								{
									int iRequestItemID	= pQuest->iRequest_ItemID[ k ];

									if( iRequestItemID == iDropItem )
									{
				
										// 搜尋角色身上及銀行
										int iVal		= pKiller->Sc_GetItemNumFromRole( iRequestItemID );

										// 檢查物品 Queue 裡面
										int iNumQueue	= pKiller->CountItemFromQueue( iRequestItemID );

										if( pQuest->iRequest_ItemVal[ k ] <= ( iVal + iNumQueue ) )
										{
											bEnough = true;
										}
										else
										{
											iLackNum = pQuest->iRequest_ItemVal[ k ] - iVal;
										}								
									}
								}

								if( bEnough == false )
								{
									if( iDropItemVal > iLackNum  )
									{
										iDropItemVal = iLackNum;
									}


									bool bDrop = false;

									if( iDropPercent != 100 )
									{
										// 照機率掉落
										int iRand		= rand() % 100;

										if( iDropPercent >= iRand )
										{

											bDrop = true;

											// 給予物品
											//pKiller->Sc_GiveItemToBuf( iDropItem, iDropItemVal , EM_ActionType_PlotGive , NULL );

											char szStr[256];
											sprintf( szStr, "打到任務目標任務物品[ %d ][ %d ]個",iDropItem, iDropItemVal  );
											NetSrv_Talk::SysMsg( pKiller->GUID(),szStr );

										}
										else
										{
											bDrop = false;
											char szStr[256];
											sprintf( szStr, "打到任務目標但沒掉落任務物品" );
											NetSrv_Talk::SysMsg( pKiller->GUID(),szStr );

										}
									}
									else
									{
										// 一定給予
										// 給予物品
										//pKiller->Sc_GiveItemToBuf( iDropItem, iDropItemVal , EM_ActionType_PlotGive , NULL );

										bDrop = true;

									}

									if( bDrop == true )
									{
										GameObjDbStructEx* pItemObj = g_ObjectData->GetObj( iDropItem );

										if( pItemObj->IsKeyItem() )
										{
											pKiller->AddKeyItem( iDropItem );
										}
										else
										{
											ItemFieldStruct Item;

											if( Global::NewItemInit( Item , iDropItem , 0 ) == true )
											{
												Item.RuneVolume		= 0;
												Item.Count			= iDropItemVal;												
											}

											if( pDropList != NULL && pOwnerList != NULL && pLockList != NULL )
											{

												pDropList->push_back( Item );
												pOwnerList->push_back( pKiller->DBID() );
												pLockList->push_back( true );
											}
										}
									}
								}

							}
						}

					}
				}


			}
			else
				break;
		}



	}

	/*
	// 檢查 Quest 打倒目標設定


	//pKiller->CheckQuest_RequestKill_AddCount( pDeader->BaseData.ItemInfo.OrgObjID );
	QuestTempDataStruct*	pQuestTempData	= &( pKiller->TempData.QuestTempData );

	for( int index = 0; index < _MAX_QUEST_COUNT_; index++ )
	{
		if( pQuestTempData->QuestKill[ index ].iKillObj[ 0 ] != 0 )
		{
			// 檢查是單純打倒目標計數, 還是機率給予物品
			//----------------------------------------------------------------------
			if( pQuestTempData->QuestKill[ index ].iKillObj[ 0 ] != 0 )
			{
				// 單純打倒目標計數
				for( int iKillIndex = 0; iKillIndex < _MAX_QUEST_KILLKIND_COUNT_; iKillIndex++ )
				{
					int iKillObjID = pQuestTempData->QuestKill[ index ].iKillObj[ iKillIndex ];

					if( iKillObjID != 0 )
					{
						if( iKillObjID == iDeadObjID )
						{
							int		iQuestField = pQuestTempData->QuestKillIndexTable[ index ];
							int		iKillVal	= pQuestTempData->QuestKill[ index ].iKillVal[ iKillIndex ];
							byte	&KillCount	= pKiller->PlayerBaseData->Quest.iQuestKillCount[ iQuestField ][ iKillIndex ];

							if( iKillVal > KillCount )
							{
								KillCount++;

								char szStr[256];
								sprintf( szStr, "打到任務目標計數 [ %d / %d ]",KillCount, iKillVal  );
								NetSrv_Talk::SysMsg( pKiller->GUID(),szStr );

							}
						}
					}
					else
						break;
				}
			}
		}
	}

	for( int index = 0; index < _MAX_QUEST_COUNT_; index++ )
	{
		if( pQuestTempData->QuestKill[ index ].iKillObj[ 0 ] != 0 )
		{

			if( pQuestTempData->QuestKill[ index ].iDropItem[ 0 ] != 0 )
			{
				// 機率給予物品
				for( int iKillIndex = 0; iKillIndex < _MAX_QUEST_KILLKIND_COUNT_; iKillIndex++ )
				{
					if( pQuestTempData->QuestKill[ index ].iDropItem[ iKillIndex ] != 0 )
					{
						if( pQuestTempData->QuestKill[ index ].iKillObj[ iKillIndex ] == iDeadObjID )
						{							
							// 檢查人物身上及銀行特殊物品, 數量是否已達要求數量
							
							int iQuestFiled	= pQuestTempData->QuestKillIndexTable[ index ];

							if( iQuestFiled == -1 )
								return;

							int iQuestID	= pKiller->PlayerBaseData->Quest.iQuest[ iQuestFiled ];
														

							GameObjDbStruct	*pObj		= g_ObjectData->GetObj( iQuestID );

							if( pObj )
							{                        
								GameObjectQuestDetail	*pQuest = &( pObj->QuestDetail );						

								float	fDropPercent	= pQuestTempData->QuestKill[ index ].fDropPercent[ iKillIndex ];
								int		iDropItem		= pQuestTempData->QuestKill[ index ].iDropItem[ iKillIndex ];
								int		iDropItemVal	= pQuestTempData->QuestKill[ index ].iDropItemVal[ iKillIndex ];

								int		iValue = 0; // 身上及銀行所擁有的數量

								for( int iRequestIndex = 0; iRequestIndex < _MAX_QUEST_CLICKIND_COUNT_; iRequestIndex++ )
								{
									if( pQuest->iRequest_ItemID[ iRequestIndex ] == iDropItem )
									{
										if( pQuest->iRequest_ItemVal[ iRequestIndex ] <= iValue )
										{
											return;	// 已達到要求的數量, 不再給予
										}
										else
											break;
									}
								}

								if( fDropPercent != 100 )
								{
									// 照機率掉落
									int iPercent	= (int)( fDropPercent );
									int iRand		= rand() % 100;

									if( iPercent >= iRand )
									{
										// 給予物品
										pKiller->Sc_GiveItemToBuf( iDropItem, iDropItemVal );

										char szStr[256];
										sprintf( szStr, "打到任務目標任務物品[ %d ][ %d ]個",iDropItem, iDropItemVal  );
										NetSrv_Talk::SysMsg( pKiller->GUID(),szStr );

									}
									else
									{
										char szStr[256];
										sprintf( szStr, "打到任務目標但沒掉落任務物品" );
										NetSrv_Talk::SysMsg( pKiller->GUID(),szStr );


									}
								}
								else
								{
									// 一定給予
									// 給予物品
									pKiller->Sc_GiveItemToBuf( iDropItem, iDropItemVal );
									char szStr[256];
									sprintf( szStr, "打到任務目標任務物品[ %d ][ %d ]個",iDropItem, iDropItemVal  );
									NetSrv_Talk::SysMsg( pKiller->GUID(),szStr );
								}
							}
						}
					}
					else
						break;
				}
			}
		}
	}
	*/
}

void CNetSrv_Script_Child::On_RoleLogin( RoleDataEx* pRole )
{
	// 每日任務
	// 登入時檢查重覆任務時間是否超過
		{
			__time32_t				LastResetTime;
			__time32_t				NowTime;
			struct	tm*				gmt;
			bool					bReset			= false;
			time_t*					pLastTime		= &( pRole->PlayerBaseData->Quest.LastCompleteTime );

			EM_QUESTCHECK_RESULT	emResult		= EM_QUESTCHECK_RESULT_OK;

			_time32( &NowTime );
			gmt = _localtime32( &NowTime );
			NowTime = (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60;

			//計算前天重置時間的秒數 ( 目前預設是早上 06:00 )

			if( gmt->tm_hour < DF_DAILY_RESET_TIME )
			{
				gmt->tm_hour	= DF_DAILY_RESET_TIME;// + RoleDataEx::G_TimeZone;
				gmt->tm_sec		= 0;
				gmt->tm_min		= 0;
				gmt->tm_mday	= gmt->tm_mday;
				LastResetTime	= (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60 - ( 24*60*60);
			}
			else
			{
				gmt->tm_hour	= DF_DAILY_RESET_TIME;// + RoleDataEx::G_TimeZone;
				gmt->tm_sec		= 0;
				gmt->tm_min		= 0;
				LastResetTime	= (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60;
			}
			
			if( *pLastTime < LastResetTime && Global::Ini()->IsBattleWorld == false )
			{
				bReset = true;	// 超過時間, 可以完成
			}


			if( bReset == true )
			{
				Print( LV2, "ResetDaily", "On_RoleLogin - account[ %s ]", pRole->Base.Account_ID.Begin() );

				pRole->PlayerBaseData->Quest.QuestDaily.ReSet();
				for( int i=0; i<DF_QUESTDAILY_MAXGROUP; i++ )
				{
					pRole->PlayerBaseData->Quest.iQuestGroup[i] = 0;
				}

				pRole->PlayerTempData->Log.ActionMode.DailyQuest	= true;
											
				// 每日任務重置
				Print( LV2, "DailyQuest", "Account[ %s], Role[ %s ] dailyquest rested!, LastTime [ %d ], LastResetTime[ %d ]!", pRole->Base.Account_ID.Begin(), Global::GetRoleName_ASCII( pRole ).c_str(), (int)*pLastTime, (int)LastResetTime );
				*pLastTime = (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60;
				NetSrv_DepartmentStoreChild::SlotMachineFreeCountProc	( pRole );
			}
			else
			{
				if( pRole->TempData.Attr.Action.Login == true )
					Print( LV2, "DailyQuest", "Account[ %s], Role[ %s ] dailyquest didnt reset, LastTime [ %d ], LastResetTime[ %d ]!", pRole->Base.Account_ID.Begin(), Global::GetRoleName_ASCII( pRole ).c_str(), (int)*pLastTime, (int)LastResetTime  );
			}



		}


		ResetDailyFlag( pRole, false );


		// 檢查任務存不存在, 因為有活動, 有可能把任務刪除 ( 利用 TAG ). 所以已經接了任務的人要系統幫忙刪除
		{
			for( int index = 0; index < _MAX_QUEST_COUNT_; index++ )
			{
				int					iQuestID	= pRole->PlayerBaseData->Quest.QuestLog[ index ].iQuestID;
				GameObjDbStructEx*	pObjDB		= g_ObjectData->GetObj( iQuestID );

				if( pObjDB == NULL )
				{
					memset( &( pRole->PlayerBaseData->Quest.QuestLog[ index ] ), 0, sizeof( pRole->PlayerBaseData->Quest.QuestLog[ index ] ) );
				}

			}

		}


		/*
		if( pRole->PlayerBaseData->TimeFlag[ EM_TimeFlagEffect_DailyFlag ] <= 0 )
		{
			// 將所有重要物品取出檢查, 若是每日重置則清除

			for( int i=0; i<_MAX_FLAG_COUNT_; i++ )
			{				
				if( pRole->BaseData.KeyItem.GetFlag( i ) == true )
				{
					int iKeyID = i + Def_ObjectClass_KeyItem;
					GameObjDbStructEx* pObj = g_ObjectData->GetObj( iKeyID );

					if( pObj != NULL && pObj->KeyItem.Type == EM_KeyItemType_DailyFlag )
					{
						pRole->DelKeyItem( iKeyID );
					}
				}
			}
		}
		*/


	// 任務資料部份 ( 從 g_ObjectData 中將資先取出, 加快每次檢查的速度 )
	//--------------------------------------------------------------------
		// 打倒目標
		//QuestTempDataStruct	*pQuestTempData = &( pRole->TempData.QuestTempData );
		//ZeroMemory( pQuestTempData, sizeof( QuestTempDataStruct ) );

		/*
		int	iTempIndex = 0;

		for( int index = 0; index < _MAX_QUEST_COUNT_; index++ )
		{
			int iQuestID =  pRole->PlayerBaseData->Quest.iQuest[ index ];
			if( iQuestID != 0 )
			{
				GameObjectQuestDetail *pQuest = &( g_ObjectData->GetObj( iQuestID )->QuestDetail );

				// 檢查及設定打倒目標
					pRole->CheckQuest_RequestKill_SetTempData( index, pQuest );

				// 檢查及設定打倒目標後掉落物品
					pRole->CheckQuest_KillToGetItem_SetTempData( index, pQuest );
			}
			else
				break;
		}
		*/

		// 給予物品
}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_ClickQuestObj	( BaseItemObject*	pObj, int iQuestField, int iQuestID, int iClickObjID, int iClickObj )
{
	// 推入事件

	RoleDataEx			*pRole			= pObj->Role();
	int					iGUID			= pRole->GUID();
	BaseItemObject*		pClickObj		= Global::GetObj( iClickObjID );
	DWORD				dwTick			= GetTickCount();

	if( pClickObj == NULL )
	{
		Print( Def_PrintLV3 , "On_ClickQuestObj" ,"pClickObj donest exist" );
		OnCast_QuestObj( iGUID, EM_GATHER_FAILED );
		return;
	}

	if( pRole == NULL )
		return;

	CastingData*		pCastingData	= &pRole->PlayerTempData->CastData;
	CastingData*		pClickObjData	= &pClickObj->Role()->PlayerTempData->CastData;

	//if( pClickObj == NULL ) return;

	// 檢查鎖定者的位置與被點擊物件之間的距離, 若超過預設範圍則解鎖
	if( pClickObjData->iLockGUID != 0 )
	{

		int	iRemainLockTime		= 0;

		if( pClickObjData->iLockTime != 0 )
		{
			iRemainLockTime = abs( (int)dwTick - (int)pClickObjData->iLockTime );
		}
		

		BaseItemObject*		pSourceLocker = Global::GetObj( pClickObjData->iLockGUID );
		
		if( pSourceLocker == NULL )
		{
			pClickObjData->iLockGUID	= 0; // 解鎖
			pClickObjData->iLockTime	= 0;
		}
		else
		{

			if( pSourceLocker->Role()->Length( pClickObj->Role() ) > 100 ) 
			{
				pClickObjData->iLockGUID = 0; // 解鎖
				pClickObjData->iLockTime	= 0;
			}
		}
	}

	// Check Range
	if( pClickObj->Role()->Length( pRole ) > 100 )
	{
		OnCast_QuestObj( iGUID, EM_GATHER_NULLOBJ );
		return;
	}

	/*
	if( pClickObjData->iLockGUID != 0 )
	{
		OnCast_QuestObj( iGUID, EM_GATHER_OBJBUSY );
		NetSrv_Talk::SysMsg( pRole->GUID(),"On_ClickQuestObj: 該物件已被鎖定或已被刪除" );
		return;
	}
	*/

	if( pClickObj->Role()->TempData.Sys.SecRoomID == -1 )
	{
		OnCast_QuestObj( iGUID, EM_GATHER_NULLOBJ );
		NetSrv_Talk::SysMsg( pRole->GUID(),"On_ClickQuestObj: 已被刪除" );
		Print( Def_PrintLV3 , "On_ClickQuestObj" ,"QuestObj doesnt exist!!" );
		return;
	}

	if( pRole->PlayerBaseData->Quest.QuestLog[ iQuestField ].iQuestID != iQuestID )
	{
		OnCast_QuestObj( iGUID, EM_GATHER_FAILED );
		Print( Def_PrintLV3 , "On_ClickQuestObj" ,"QuestField and QuestID doesnt match!!" );
		return;
	}

	// 取出 Quest 資料並檢查角色身上是否已得到足夠的物品
	GameObjDbStructEx		*pObjDB = Global::GetObjDB( iQuestID );
	
	if( pObjDB )
	{
		GameObjectQuestDetail *pQuest = &( pObjDB->QuestDetail );

		int iItem		= 0;
		int iItemCount	= 0;

		// 根據 Quest 資料檢查
		for( int index = 0; index < _MAX_SIZE_QUEST_CONDITION; index++ )
		{
			if( pQuest->iProcess_ClickToGetItem_ObjID[ index ] == iClickObj )
			{
				//int		iPercent	= pQuest->iProcess_ClickToGetItem_ProcPercent[ index ];
				int		iItem				= pQuest->iProcess_ClickToGetItem_GetItem[ index ];
				int		iItemVal			= pQuest->iProcess_ClickToGetItem_GetItemVal[ index ];
				
				int		iRequest_ItemVal	= 0;

				for( int index = 0; index < _MAX_SIZE_QUEST_CONDITION; index ++ )
				{
					if( pQuest->iRequest_ItemID[ index ] == iItem )
					{
						iRequest_ItemVal =  pQuest->iRequest_ItemVal[ index ];
						break;
					}
				}
				
				bool	bCheckHaveItem = false;
				if( iRequest_ItemVal != 0 )
				{
					bCheckHaveItem 		= pRole->Sc_CheckItemFromRole( iItem, iRequest_ItemVal );
				}

				if( bCheckHaveItem == false )
				{
					// 檢查有無劇情需要執行
					int iLen = (int)strlen( pQuest->szScript_OnClickObjBegin );
					if( iLen != 0 )
					{
						BaseItemObject*		pTarget = Global::GetObj( iClickObjID );
						

						if( pTarget )
						{
							// 檢查該物件是否正在忙錄
							
							// 執行劇情
							vector< MyVMValueStruct > vecArg;
							LUA_VMClass::PCallByStrArg( pQuest->szScript_OnClickObjBegin, iGUID, iClickObjID, &vecArg );

							if( vecArg.size() > 0 )
							{
								MyVMValueStruct Val;
								Val = vecArg[0];

								if( Val.Type == MyVMValueType_Number )
								{
									if( Val.Number != 1 )
									{
										//ClickQuestObjResult( pRole->GUID(), EM_GATHER_FAILED, 0 , 0 );
										OnCast_QuestObj( iGUID, EM_GATHER_FAILED );
										return;
									}
								}
								else if( Val.Type == MyVMValueType_Boolean )
								{
									if( Val.Flag == false )
									{
										//ClickQuestObjResult( pRole->GUID(), EM_GATHER_FAILED, 0 , 0 );
										OnCast_QuestObj( iGUID, EM_GATHER_FAILED );
										return;
									}
								}
							}
							else
							{
								//NetSrv_Talk::SysMsg( pRole->GUID(),"On_ClickQuestObj: Script() 沒有參數傳回" );
								return;
							}



							//pObj->PlotVM()->CallLuaFunc( pQuest->szScript_OnClickObjBegin, iClickObjID, 0 );
/*							int iReturn = pObj->PlotVM()->PCallReturn( pQuest->szScript_OnClickObjBegin, iGUID, iClickObjID);

							if( iReturn != 1 )
								return;	// 劇情檢查不通過不執行
								*/
						}				
					}


					// 額外的設定
						pCastingData->iContext	= iQuestID; 
					
					// 開始時間條				
						CNetSrv_Gather_Child::SetCast( pRole, iClickObjID, _TIME_CLICKTOGETITEM,  OnCast_QuestObj, "" );

					//int iEventHandle				= Schedular()->Push( OnEvent_CastingFinished , _TIME_CLICKTOGETITEM, (VOID*)iGUID, NULL );	


					//pClickObjData->iLockGUID		= iGUID;

					//pCastingData->iEventHandle		= iEventHandle;
					//pCastingData->iQuestID			= iQuestID;
					//pCastingData->iClickObj			= iClickObj;
					//pCastingData->iClickObjID		= iClickObjID;


				}

				break;
			}
		}
	}

	




}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::OnCast_ScriptCast ( int iGUID, int iResult )
{
	ENUM_GATHER_RESULT	emResult		= (ENUM_GATHER_RESULT)iResult;

	if( emResult == EM_GATHER_BEGIN )
		return;

	// 共用資料
	//--------------------------------------------------------------------------------------------------------------------
	RoleDataEx*			pClickObj		= NULL;
	GameObjDbStructEx*	pClickObjDB		= NULL;

	bool				bFailed			= false;

	//int					iGUID			= (int)InputClass;
	BaseItemObject*		pObj			= BaseItemObject::GetObj( iGUID );
	BaseItemObject*		pClickBaseObj	= NULL;

	// 檢查人物還在不在
	//--------------------------------------------------------------------------------------------------------------------
		if( pObj == NULL )	
		{
			return;	// 人物已經下線了, 或是斷線, 反正就是人物物件不存在, 跳開不處理, 人也不在了
		}

		RoleDataEx			*pRole			= pObj->Role();						// 點擊物件的人物
		CastingData*		pCastingData	= &pRole->PlayerTempData->CastData;		// 點擊資料

		//int					iQuestID		= pCastingData->iQuestID;
		//int					iClickObj		= pCastingData->iClickObj;
		int					iClickObjID		= pCastingData->iClickObjID;

		pClickBaseObj	= Global::GetObj( iClickObjID );	// 取出被點擊物件

		if( pClickBaseObj )
		{
			pClickObj		=  pClickBaseObj->Role();

			if( pClickObj )
				pClickObjDB	= Global::GetObjDB( pClickObj->BaseData.ItemInfo.OrgObjID );
			else
				pClickObjDB	= NULL;
		}

	
	//--------------------------------------------------------------------------------------------------------------------

		if( emResult != EM_GATHER_ENDCAST )
		{
			//CNetSrv_Gather::ClickGatherObjResult( iGUID, emResult );
			//ScriptCastResult( iGUID, emResult );
			pCastingData->iContext	= emResult;

		}
		else
		{
			
			//--------------------------------------------------------------------------------------------------------------------

			// 重置施展資料
				//pCastingData->Clear();
			emResult				= EM_GATHER_SUCCESSED;
			pCastingData->iContext	= EM_GATHER_SUCCESSED;
		
			//ScriptCastResult( iGUID, EM_GATHER_ENDCAST )
		}

	if( pCastingData->szLuaEvent.Size() != 0 )
	{
		char szLuaBuffer[512];

		int		iObjID		= iGUID;
		int		iEventID	= pCastingData->iContext;

		sprintf( szLuaBuffer, "%s( %d, %d )", pCastingData->szLuaEvent.Begin(), iObjID, iEventID );						
		LUA_VMClass::PCallByStrArg( szLuaBuffer, iObjID, iClickObjID, NULL );
	}

		return;
}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::OnCast_QuestObj ( int iGUID, int iResult )
{
	ENUM_GATHER_RESULT	emResult		= (ENUM_GATHER_RESULT)iResult;

		if( emResult == EM_GATHER_BEGIN )
			return;


	// 共用資料
	//--------------------------------------------------------------------------------------------------------------------
		RoleDataEx*			pClickObj		= NULL;
		GameObjDbStructEx*	pClickObjDB		= NULL;

		bool				bFailed			= false;

		//int					iGUID			= (int)InputClass;
		BaseItemObject*		pObj			= BaseItemObject::GetObj( iGUID );
		BaseItemObject*		pClickBaseObj	= NULL;


	//--------------------------------------------------------------------------------------------------------------------

		if( emResult != EM_GATHER_ENDCAST )
		{
			//CNetSrv_Gather::ClickGatherObjResult( iGUID, emResult );
			ClickQuestObjResult( iGUID, emResult, 0 , 0 );
			return;
		}

	// 檢查人物還在不在
	//--------------------------------------------------------------------------------------------------------------------
		if( pObj == NULL )	
		{
			return;	// 人物已經下線了, 或是斷線, 反正就是人物物件不存在, 跳開不處理, 人也不在了
		}

		RoleDataEx			*pRole			= pObj->Role();						// 點擊物件的人物
		CastingData*		pCastingData	= &pRole->PlayerTempData->CastData;		// 點擊資料

		//int					iQuestID		= pCastingData->iQuestID;
		//int					iClickObj		= pCastingData->iClickObj;
		int					iClickObjID		= pCastingData->iClickObjID;

		pClickBaseObj	= Global::GetObj( iClickObjID );	// 取出被點擊物件

		//*暫時修改*
		if( pClickBaseObj == NULL )
		{
			ClickQuestObjResult( iGUID, EM_GATHER_FAILED, 0, 0 );
			return;
		}

		pClickObj		=  pClickBaseObj->Role();

		if( pClickObj )
			pClickObjDB	= Global::GetObjDB( pClickObj->BaseData.ItemInfo.OrgObjID );
		else
			pClickObjDB	= NULL;




	//--------------------------------------------------------------------------------------------------------------------
		if( emResult == EM_GATHER_ENDCAST  && pClickObj != NULL )
		{
			int				iQuestID		= pCastingData->iContext;
			int				iClickObj		= pClickObj->BaseData.ItemInfo.OrgObjID;
			int				iClickObjID		= pCastingData->iClickObjID;

			// 重置施展資料
			pCastingData->Clear();


			// 取出 Quest 資料並給予該物品
			GameObjDbStructEx		*pObjDB = Global::GetObjDB( iQuestID );
			if( !pObjDB ) 
			{ 
				Print( Def_PrintLV3 , "OnEvent_CastingFinished" , "GetObjDB() Failed!!" ); 
				ClickQuestObjResult( iGUID, EM_GATHER_FAILED, 0, 0 );
				return; 
			}

			GameObjectQuestDetail*	pQuest	= &( pObjDB->QuestDetail );
			BaseItemObject*			pTarget = Global::GetObj( iClickObjID );		


			if( pTarget != NULL )
			{
				// 打開鎖定
				pTarget->Role()->PlayerTempData->CastData.iLockGUID = 0;


				// 檢查有無劇情需要執行
				if( strlen( pQuest->szScript_OnClickObjEnd ) != 0 )
				{

					// 執行劇情
					//pObj->PlotVM()->CallLuaFunc( pQuest->szScript_OnClickObjBegin, iClickObjID, 0 );
					/*
					int iReturn = pObj->PlotVM()->PCallReturn( pQuest->szScript_OnClickObjEnd, iGUID, iClickObjID);

					if( iReturn != 1 )
					return 0;	// 劇情檢查不通過不執行
					*/

					vector< MyVMValueStruct > vecArg;
					LUA_VMClass::PCallByStrArg( pQuest->szScript_OnClickObjEnd, iGUID, iClickObjID, &vecArg );

					if( vecArg.size() > 0 )
					{
						MyVMValueStruct Val;
						Val = vecArg[0];

						if( Val.Type == MyVMValueType_Number )
						{
							if( Val.Number != 1 )
							{
								//NetSrv_Talk::SysMsg( pKiller->GUID(),"OnEvent_CastingFinished" , "pQuest->szScript_OnClickObjEnd tj" );
								//OnCast_QuestObj( pRole->GUID(), EM_GATHER_FAILED );
								ClickQuestObjResult( pRole->GUID(), EM_GATHER_FAILED, 0, 0 );
								return;	// 劇情檢查不通過不執行
							}
						}
						else if( Val.Type == MyVMValueType_Boolean )
						{
							if( Val.Flag == false )
							{
								//ClickQuestObjResult( pRole->GUID(), EM_GATHER_FAILED, 0 , 0 );
								ClickQuestObjResult( pRole->GUID(), EM_GATHER_FAILED, 0, 0 );
								return;
							}
						}
					}
					else
					{
						NetSrv_Talk::SysMsg( pRole->GUID(),"OnEvent_CastingFinished: Script() 沒有參數傳回" );
						ClickQuestObjResult( pRole->GUID(), EM_GATHER_FAILED, 0, 0 );
						return;
					}
				}
			}

			bool	bResult = true;
			int		iItem, iItemVal, iPercent;


			// 根據 Quest 資料給予物品 )
			for( int index = 0; index < _MAX_SIZE_QUEST_CONDITION; index++ )
			{
				if( pQuest->iProcess_ClickToGetItem_ObjID[ index ] == iClickObj )
				{
					iPercent	= pQuest->iProcess_ClickToGetItem_ProcPercent[ index ];
					iItem		= pQuest->iProcess_ClickToGetItem_GetItem[ index ];
					iItemVal	= pQuest->iProcess_ClickToGetItem_GetItemVal[ index ];

					if( iPercent == 100 )
					{
						// 一定給予
						pRole->Sc_GiveItemToBuf( iItem,  iItemVal , EM_ActionType_PlotGive , NULL , "" );
						bResult = true;
						break;
					}
					else
					{
						// 照機率掉落
						int iRand		= rand() % 100;
						if( iPercent >= iRand )
						{
							pRole->Sc_GiveItemToBuf( iItem,  iItemVal , EM_ActionType_PlotGive , NULL , "" );
							bResult = true;
							break;
						}
						else
						{
							//ClickQuestObjFailed( pObj->SockID() );
							//ClickQuestObjResult( pObj->SockID(), 1, iItem, iItemVal );
							bResult = false;
						}
					}
				}
			}

			if( bResult == true )
			{
				ClickQuestObjResult( pRole->GUID(), EM_GATHER_ENDCAST, iItem, iItemVal );
			}
			else
			{
				ClickQuestObjResult( pRole->GUID(), EM_GATHER_FAILED, iItem, iItemVal );
			}


			// 檢查若是需要關掉點選的物件關畢

			if( pTarget != NULL )
				pTarget->Role()->SetDisableClick( true );


			// 重制被點物件
			Global::ResetObj( iClickObjID );


		}
		else
		{
			ClickQuestObjResult( iGUID, EM_GATHER_FAILED, 0, 0 );
			return;
		}


}
//-------------------------------------------------------------------
/*
int CNetSrv_Script_Child::OnEvent_CastingFinished( SchedularInfo* Obj , void* InputClass)
{
	int				iGUID	= (int)InputClass;
	BaseItemObject*	pObj	= BaseItemObject::GetObj( iGUID );

	if( pObj == NULL )	return 0;


	RoleDataEx		*pRole			= pObj->Role();
	//pRole->DBID
	CastingData*	pCastingData	= &pRole->PlayerTempData->CastData;

	
	int				iQuestID		= pCastingData->iContext;
	int				iClickObj		= pCastingData->iClickObj;
	int				iClickObjID		= pCastingData->iClickObjID;

	// 重置施展資料
	pCastingData->Clear();


	// 取出 Quest 資料並給予該物品
	GameObjDbStructEx		*pObjDB = Global::GetObjDB( iQuestID );
	if( !pObjDB ) { Print( Def_PrintLV3 , "OnEvent_CastingFinished" , "GetObjDB() Failed!!" ); return 0; }

	GameObjectQuestDetail*	pQuest	= &( pObjDB->QuestDetail );
	BaseItemObject*			pTarget = Global::GetObj( iClickObjID );		

	
	if( pTarget != NULL )
	{
		// 打開鎖定
		pTarget->Role()->PlayerTempData->CastData.iLockGUID = 0;


		// 檢查有無劇情需要執行
		if( strlen( pQuest->szScript_OnClickObjEnd ) != 0 )
		{
									
			// 執行劇情
			//pObj->PlotVM()->CallLuaFunc( pQuest->szScript_OnClickObjBegin, iClickObjID, 0 );
			
			//int iReturn = pObj->PlotVM()->PCallReturn( pQuest->szScript_OnClickObjEnd, iGUID, iClickObjID);

			//if( iReturn != 1 )
				//return 0;	// 劇情檢查不通過不執行
				

			vector< MyVMValueStruct > vecArg;
			LUA_VMClass::PCallByStrArg( pQuest->szScript_OnClickObjEnd, iGUID, iClickObjID, &vecArg );

			if( vecArg.size() > 0 )
			{
				MyVMValueStruct Val;
				Val = vecArg[0];

				if( Val.Type == MyVMValueType_Number )
				{
					if( Val.Number != 1 )
					{
						//NetSrv_Talk::SysMsg( pKiller->GUID(),"OnEvent_CastingFinished" , "pQuest->szScript_OnClickObjEnd tj" );
						return 0;	// 劇情檢查不通過不執行
					}
				}
				else if( Val.Type == MyVMValueType_Boolean )
				{
					if( Val.Flag == false )
					{
						return 0;
					}
				}
			}
			else
			{
				NetSrv_Talk::SysMsg( pRole->GUID(),"OnEvent_CastingFinished: Script() 沒有參數傳回" );
				return 0;
			}
		}
	}

	bool	bResult = true;
	int		iItem, iItemVal, iPercent;


	// 根據 Quest 資料給予物品 )
	for( int index = 0; index < _MAX_SIZE_QUEST_CONDITION; index++ )
	{
		if( pQuest->iProcess_ClickToGetItem_ObjID[ index ] == iClickObj )
		{
			iPercent	= pQuest->iProcess_ClickToGetItem_ProcPercent[ index ];
			iItem		= pQuest->iProcess_ClickToGetItem_GetItem[ index ];
			iItemVal	= pQuest->iProcess_ClickToGetItem_GetItemVal[ index ];

			if( iPercent == 100 )
			{
				// 一定給予
				pRole->Sc_GiveItemToBuf( iItem,  iItemVal , EM_ActionType_PlotGive , NULL , "" ); 
			}
			else
			{
				// 照機率掉落
				int iRand		= rand() % 100;
				if( iPercent >= iRand )
				{
					pRole->Sc_GiveItemToBuf( iItem,  iItemVal , EM_ActionType_PlotGive , NULL , "" ); 
				}
				else
				{
					//ClickQuestObjFailed( pObj->SockID() );
					//ClickQuestObjResult( pObj->SockID(), 1, iItem, iItemVal );
					bResult = false;
				}
			}
		}
	}

	if( bResult == true )
	{
		ClickQuestObjResult( pRole->GUID(), 1, iItem, iItemVal );
	}


	// 檢查若是需要關掉點選的物件關畢
	pTarget->Role()->SetDisableClick( true );


	// 重制被點物件
	Global::ResetObj( iClickObjID );


	return 0;
}
*/
//-------------------------------------------------------------------
void		CNetSrv_Script_Child::On_TouchEvent( BaseItemObject*	pObj, int iEventObjID )
{
    //範圍檢查
    BaseItemObject* TargetObj = BaseItemObject::GetObj( iEventObjID );
    if( TargetObj == NULL )
    {
        Print( LV2 , "On_TouchEvent" , "target not find" );
        return;
    }
    RoleDataEx* Target = TargetObj->Role();

    if( Target->Length( pObj->Role() ) > Target->TempData.TouchPlot_Range + 200 )
    {
        Print( LV2 , "On_TouchEvent", "target out of range" );
        return;
    }

    ArgTransferStruct* Arg = pObj->PlotVM()->CallLuaFunc( Target->TempData.TouchPlot.Begin() , iEventObjID , 0 );
	if( Arg == NULL )
	{
		pObj->Role()->Msg( "target busy" );
	}

}
//-------------------------------------------------------------------
void		CNetSrv_Script_Child::On_RangeEvent( BaseItemObject*	pObj, int iEventObjID )
{
    //範圍檢查
    BaseItemObject* TargetObj = BaseItemObject::GetObj( iEventObjID );
    if( TargetObj == NULL )
    {
        Print( LV2 , "On_RangeEvent" , "target not exist" );
        return;
    }
    RoleDataEx* Target = TargetObj->Role();

    if( Target->Length( pObj->Role() ) > Target->TempData.RangePlot_Range + 200 )
    {
        Print( LV2 , "On_RangeEvent" , "target out of range" );
        return;
    }

    pObj->PlotVM()->CallLuaFunc( Target->TempData.RangePlot.Begin() , iEventObjID , 0 );
}
//-------------------------------------------------------------------
void		CNetSrv_Script_Child::On_CollisionEvent( BaseItemObject*	pObj, int iEventObjID )
{
	// 碰撞檢查
	BaseItemObject* TargetObj = BaseItemObject::GetObj( iEventObjID );
	if( TargetObj == NULL )
	{
		Print( LV2 , "On_RangeEvent" , "target not exist" );
		return;
	}
	RoleDataEx* Target = TargetObj->Role();

	if( Target->Length( pObj->Role() ) > 200 )
	{
		Print( LV2 , "On_RangeEvent" , "target out of range" );
		return;
	}

	pObj->PlotVM()->CallLuaFunc( Target->TempData.CollisionPlot.Begin() , iEventObjID , 0 );
}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_LoadQuestSpeakOption		( BaseItemObject*	pObj, int iNPCID )
{
	// 撿查該 NPC 是否有劇情可供觸發
	BaseItemObject*		TargetObj	= BaseItemObject::GetObj( iNPCID );

	if( TargetObj == NULL )
	{
		Print( LV2 , "On_LoadQuestSpeakOption" , "target not exist" );
		return;
	}
	RoleDataEx* Target = TargetObj->Role();

	if( Target->Length( pObj->Role() ) > _MAX_ROLE_TOUCH_MAXRANGE_ )
	{
		Print( LV2 , "On_LoadQuestSpeakOption" , "target out of range" );
		return;
	}

	GameObjDbStructEx	*pObjDB = Global::GetObjDB( Target->BaseData.ItemInfo.OrgObjID );
	
	if( pObjDB )
	{
		if( strlen( pObjDB->NPC.szLuaScript ) != 0 )
		{

			RoleDataEx* pRole = pObj->Role();

			// 重置對話資料
			pRole->PlayerTempData->SpeakInfo.Reset();

			pRole->PlayerTempData->SpeakInfo.iSpeakNPCID = iNPCID;

			//pObj->PlotVM()->PCall( Target->TempData.TouchPlot.Begin() , iNPCID , 0 );

				vector< MyVMValueStruct > vecArg;
				LUA_VMClass::PCallByStrArg( pObjDB->NPC.szLuaScript, pObj->GUID(), pRole->PlayerTempData->SpeakInfo.iSpeakNPCID, &vecArg );

			// 檢查新增的選項, 並作出封包傳送給 Client			
				SpeakOption( pObj->GUID() );			
		}
	}	
}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_ChoiceOption ( BaseItemObject*	pObj, int iOption )
{
	RoleDataEx* pRole = pObj->Role();

	// 發送一個封包給 Client 以便進行後續處理
	ChoiceOptionResult( pObj->GUID() );
	
	if( iOption <= pRole->PlayerTempData->SpeakInfo.SpeakOption.Size() && iOption >= 1 )
	{
		// 從資料中取出 Option 資料, 進行處理
			SpeakOptionStruct	Option	= pRole->PlayerTempData->SpeakInfo.SpeakOption[ ( iOption - 1 ) ];
			int					iNPCID	= pRole->PlayerTempData->SpeakInfo.iSpeakNPCID;

		// 重置對話資料
			pRole->PlayerTempData->SpeakInfo.Reset();

		// 進行 Lua 呼叫
			vector< MyVMValueStruct > vecArg;
			LUA_VMClass::PCallByStrArg( Option.szLuaFunc.Begin(), pObj->GUID(), iNPCID, &vecArg );
		
		// 檢查新增的選項, 並作出封包傳送給 Client
			if( pRole->PlayerTempData->SpeakInfo.SpeakOption.Size() != 0  || pRole->PlayerTempData->SpeakInfo.iLoadQuest != 0 || pRole->PlayerTempData->SpeakInfo.SpeakDetail.Size() != 0 )
			{
				pRole->PlayerTempData->SpeakInfo.iSpeakNPCID = iNPCID;
				SpeakOption( pObj->GUID() );
			}
	}
}
//-------------------------------------------------------------------
void CPlotEventQuestComplete::OnEnd()
{
	// 當任務完成後, 執行劇情, 劇情完成後觸發
	
	BaseItemObject	*pNPCObj	= BaseItemObject::GetObj( m_iNPCID );
	if( pNPCObj != NULL )
	{
		RoleDataEx*			pQuestNPC		= pNPCObj->Role();
		if( pQuestNPC != NULL )
		{
			//if( pQuestNPC->PlayerTempData->ScriptInfo.emMode == EM_SCRIPT_RUNNING )
			//{
			//	pQuestNPC->PlayerTempData->ScriptInfo.emMode = EM_SCRIPT_NONE;
			//}
			if( pQuestNPC->IsDisableQuest() == true )
			{
				pQuestNPC->SetDisableQuest( false );
			}
		}
	}	
}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_RoleMove ( RoleDataEx* pRole )
{
	




}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_RoleCast ( RoleDataEx* pRole )
{


}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_RoleBeHit ( RoleDataEx* pRole )
{
	if( pRole->PlayerTempData->CastData.iEventHandle != -1 )
	{
		pRole->PlayerTempData->CastData.bBeHit	= true;		// 設定施法過程被擊中
	}
}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_DelayPatrol ( BaseItemObject*	pObj, int iNPCID, int iTime )
{
	BaseItemObject	*pNPCObj	= BaseItemObject::GetObj( iNPCID );

	if( pNPCObj )
	{
		RoleDataEx*						pNPC		= pNPCObj->Role();
		vector< NPC_MoveBaseStruct >*	MoveList	= pNPCObj->NPCMoveInfo()->MoveInfo();

		if( MoveList->size() > 1 )
		{
			if( RoleDataEx::G_SysTime > pNPC->TempData.iDelayPatrol )
				pNPC->TempData.iDelayPatrol = RoleDataEx::G_SysTime;

			int		offset	= pNPC->TempData.iDelayPatrol - RoleDataEx::G_SysTime;
			int		value	= offset / _TIME_DELAYPATROL;

			if( value < 2 )
			{
				pNPC->TempData.iDelayPatrol = RoleDataEx::G_SysTime + offset + _TIME_DELAYPATROL;
				pNPC->StopMoveNow();
//				pNPCObj->Path()->ClearPath();
			}
		}
	}
}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::CheckQuest ( RoleDataEx* pRole )
{
	for( int i=0; i < _MAX_QUEST_COUNT_; i++ )
	{
		if( pRole->PlayerBaseData->Quest.QuestLog[ i ].iMode == 1 )	// 重新上線時
		{

		}
	}

}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_CloseBorder ( BaseItemObject*	pObj  )
{
	RoleDataEx*	pRole		= pObj->Role();
	pRole->TempData.ScriptBorder.Init();
}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_AcceptBorderQuest ( BaseItemObject*	pObj )
{
//	EM_QUESTCHECK_RESULT	emResult;

	RoleDataEx*			pRole		= pObj->Role();
	int					iQuestID	= pRole->TempData.ScriptBorder.iQuestID;
	GameObjDbStruct*	pGameObj	= g_ObjectData->GetObj( iQuestID );

	int					iQuestField = 0;
	
	if( pGameObj == NULL )
		return;

	On_QuestAccept( pObj, 0, iQuestID, true );

}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::ResetDailyQuest( RoleDataEx *pRole )
{
	// 對已連線的每個角色發出重置封包, 並將 Role 資料的每日任務設定關畢
	/*
	for( int i = 0 ; i < BaseItemObject::GetmaxZoneID( ) ; i++ )
	{
		BaseItemObject*	pObj = BaseItemObject::GetObj_ByLocalID( i );
		if( pObj == NULL )
			continue;
		else
		{
			RoleDataEx			*pRole		= pObj->Role();

			if( pRole->IsPlayer() )
			{
				// 重置每日任務
				pRole->PlayerBaseData->Quest.QuestDaily.ReSet();

				// 重置群組任務
				for( int i=0; i<DF_QUESTDAILY_MAXGROUP; i++ )
				{
					pRole->PlayerBaseData->Quest.iQuestGroup[i] = 0;
				}

				pRole->PlayerBaseData->Quest.LastCompleteTime = (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60;;

				// 發送封包通知 Client 重置任務
				CNetSrv_Script::ResetDailyQuest( pObj->GUID(), 1 );
			}
		}
	}
	*/

	if( pRole != NULL && pRole->IsPlayer() )
	{
		Print( LV2, "ResetDaily", "ResetDailyQuest - account[ %s ]", pRole->Base.Account_ID.Begin() );

		// 重置每日任務
		pRole->PlayerBaseData->Quest.QuestDaily.ReSet();

		// 重置群組任務
		for( int i=0; i<DF_QUESTDAILY_MAXGROUP; i++ )
		{
			pRole->PlayerBaseData->Quest.iQuestGroup[i] = 0;
		}

		pRole->PlayerTempData->Log.ActionMode.DailyQuest	= true;

		{
			__time32_t NowTime;
			struct	tm*				gmt;
			_time32( &NowTime );
			gmt = _localtime32( &NowTime );

			pRole->PlayerBaseData->Quest.LastCompleteTime = (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60;;
		}
		

		// 發送封包通知 Client 重置任務
		CNetSrv_Script::ResetDailyQuest( pRole->GUID(), 1 );
	}

}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::ResetDailyFlag( RoleDataEx *pRole, bool bForceReset )
{
	// 每日重要物品清除機制
	if( pRole != NULL && ( pRole->PlayerBaseData->TimeFlag[ EM_TimeFlagEffect_DailyFlag ] <= 0  || bForceReset == true ) )
	{
		// 將所有重要物品取出檢查, 若是每日重置則清除

		pRole->PlayerBaseData->TimeFlag[ EM_TimeFlagEffect_DailyFlag ] = 0;

		for( int i=0; i<_MAX_FLAG_COUNT_; i++ )
		{				
			if( pRole->BaseData.KeyItem.GetFlag( i ) == true )
			{
				int iKeyID = RoleDataEx::IDtoKeyItemObj( i );
				GameObjDbStructEx* pObj = g_ObjectData->GetObj( iKeyID );

				if( pObj != NULL && pObj->KeyItem.Type == EM_KeyItemType_DailyFlag )
				{
					pRole->DelKeyItem( iKeyID );
				}
			}
		}

		pRole->PlayerTempData->Log.ActionMode.DailyFlag		= true;

		Print( LV2, "ResetDaily", "Account[ %s], Role[ %s ] dailyflag rested!", pRole->Base.Account_ID.Begin(), Global::GetRoleName_ASCII( pRole ).c_str() );
	}
	else
	{
		if( pRole->TempData.Attr.Action.Login == true )
			Print( LV2, "ResetDaily", "Account[ %s], Role[ %s ] dailyflag didnt reset, remain time [%d]!", pRole->Base.Account_ID.Begin(), Global::GetRoleName_ASCII( pRole ).c_str(), pRole->PlayerBaseData->TimeFlag[ EM_TimeFlagEffect_DailyFlag ] );
	}
}

