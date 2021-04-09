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

	// �D���~�Ҧ�
		if( bItemMode == false )
		{
			// �ˬd���� NPC �b���b
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
			// �ˬd���� NPC �O�_�֦��� QUEST
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

			// �ˬd�P���� NPC �Z��
				float fRange = pRole->Length( pQuestNPC );

				if( fRange > ( _MAX_ROLE_TOUCH_RANGE_ * 2 ) )
				{
					QuestAcceptResult( pObj->SockID(), iQuestField, iQuestID, EM_QUESTCHECK_RESULT_TOOFAR );
					return;

				}

			// �ˬd���ȱ���O�_�F��
				if( pRole->CheckQuest_IsAvailable( iQuestID, pQuest, true ) != EM_QUESTCHECK_RESULT_OK )
				{
					return;
				}

			// �ˬd���ȵ������~���Ŷ��O�_����
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

	// �ˬd�O�_���Х���, �Y���O�h���i���б��w�����L������
		if( pQuest->iCheck_Loop == false )
		{
			if( pRole->CheckQuestHistory_IsComplete( iQuestID ) == true )
			{
				QuestAcceptResult( pObj->SockID(), iQuestField, iQuestID, EM_QUESTCHECK_RESULT_ALREADYDONE );
				return;
			}
		}


	// �ˬd�ثe�O�_�֦��� Quest
		if( pRole->CheckQuestList_IsGotQuest( iQuestID ) != -1 )
		{
			QuestAcceptResult( pObj->SockID(), iQuestField, iQuestID, EM_QUESTCHECK_RESULT_ALREADYEXIST );
			return;

		}

	// �ˬd�O�_�٦��Ŷ��i�w�s�[����
		int iQuestSize = pRole->Quest_Size();
		if( iQuestSize >= _MAX_QUEST_COUNT_ )
		{
			QuestAcceptResult( pObj->SockID(), iQuestField, iQuestID, EM_QUESTCHECK_RESULT_FULL );
			return;
		}

	
	// ��s Quest �M��
	//------------------------------------------------------------------------------------
		iQuestField = pRole->Quest_Add( iQuestID );

		// �ˬd���L���˥ؼФε������~�]�w

		// �������~
		if( pQuest->iAccept_ItemID[ 0 ] != 0 )
		{
			for( int index = 0; index < _MAX_SIZE_QUEST_ITEM; index++ )
			{
				int iItemID = pQuest->iAccept_ItemID[ index ];
				if( iItemID != 0 )
				{
					pRole->Sc_GiveItemToBuf( iItemID, pQuest->iAccept_ItemVal[ index ]  , EM_ActionType_PlotGive , NULL , "" );		// Quest Accept	

					char szStr[256];
					sprintf( szStr, "�������~[ %d ][ %d ]��",iItemID, pQuest->iAccept_ItemVal[ index ]  );
					NetSrv_Talk::SysMsg( pObj->GUID(),szStr );

				}
				else
					break;
			}
		}


			// �ˬd�γ]�w���˥ؼ�
			//pRole->CheckQuest_RequestKill_SetTempData( iQuestField, pQuest );

			// �ˬd�γ]�w���˥ؼЫᱼ�����~
			//pRole->CheckQuest_KillToGetItem_SetTempData( iQuestField, pQuest );

		// �ˬd�O�_���������~


		//  ����@��
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

	//�p��e�ѭ��m�ɶ������ ( �ثe�w�]�O���W 06:00 )

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
		bReset = true;	// �W�L�ɶ�, �i�H����
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

		return EM_QUESTCHECK_RESULT_OK; // �i�H��������
	}
	else
	{
		// �ˬd�����O�_�W�L
		// �C�����
			if( emResult == EM_QUESTCHECK_RESULT_OK && pQuest->iControlType == 1 ) // �C�駹���@��
			{
				int iID = iQuestID - Def_ObjectClass_QuestDetail;
				if( iID >= 0 && _MAX_QUEST_FLAG_COUNT_ > iID )
				{
					if( pRole->PlayerBaseData->Quest.QuestDaily.GetFlag( iID ) == true )
					{
						// �w�g�����L
						emResult = EM_QUESTCHECK_RESULT_DAILYDONE;
					}			
				}
				else
				{
					emResult = EM_QUESTCHECK_RESULT_QUESTERR;
				}
			}

		// �s�խ��w
			if( emResult == EM_QUESTCHECK_RESULT_OK && pQuest->iQuestGroup != 0 ) // �C�Ӹs�խ��w�@�ѥu�৹�� 10 �ӥ���
			{
				int iDailyLimit = 10;

				if( pQuest->iQuestGroup == 1 ) // ���|����
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
			// ���T�w�֦��� Quest

			iQuestField = pRole->CheckQuestList_IsGotQuest( iQuestID );

			if( iQuestField != -1 )
			{
				GameObjDbStructEx*		pDataObj	= Global::GetObjDB( iQuestID );
				
				if( pDataObj == NULL )	return;				
					
				GameObjectQuestDetail *pQuest = &( pDataObj->QuestDetail );


				// �ˬd���� NPC �O���O������
				if( pQuestNPC->IsDisableQuest() == true )
				{
					// ���ȧ����e����@��, ��@�����o�X��������
					emResult = EM_QUESTCHECK_RESULT_RUNNINGSCRIPT;
					QuestCompleteResult( pObj->SockID(), iQuestNpcID, iQuestField, iQuestID, EM_QUESTCHECK_RESULT_RUNNINGSCRIPT );
					return;
				}
				
				// �ˬd���ƥ��ȳ]�w
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



				// �ˬd�ӥ��ȬO�_�����w�t�@NPC����.�Y�O�h�ˬd
					if( pQuest->iRequest_QuestID != 0 && pQuest->iRequest_QuestID != pQuestNPC->BaseData.ItemInfo.OrgObjID )
					{
						return;
					}


				// �ˬd�O�_�٦��Ŷ��i�H�������ફ�~�ΤU���q�����Ȫ��~
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

				// �ˬd�O�_�i�H��������
				//--------------------------------------------------------------------
				if( emResult == EM_QUESTCHECK_RESULT_OK )
				{
					//Packet.emResult
					int iQuestLV = pQuest->iCheck_LV;

					
					// �ˬd�O�_���������~�Ϊ����W�n ... ���o�������ȫʥ], �A�B�z�������~

						

						// �R�����~
						//-------------------------------------------------------------------------
							// �R�� Money
								//--------------------------------------------------------------------
								if( pQuest->iRequest_Money != 0 )
								{
									if( pRole->Sc_GiveMoney( ( pQuest->iRequest_Money * -1 ) ) == true )
									{
										char szStr[256];
										//sprintf( szStr, "���^����[ %d ]",pQuest->iRequest_Money  );
										//NetSrv_Talk::SysMsg( pObj->GUID(),szStr );
									}
									else
									{
										QuestCompleteResult( pObj->SockID(), iQuestNpcID, iQuestField, iQuestID, EM_QUESTCHECK_RESULT_NOQUESTITEM );
										return;
									}
								}

							// �R�����~
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
												//sprintf( szStr, "�R�����~[ %d ][ %d ]��",iItemID, pQuest->iRequest_ItemVal[ index ]  );
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



							// �Y�O���ƫ�����, �h�֭p
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

						// �����a�A
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
										// �p�G�b���P�ϰ�, �h������ Member ���
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
						

						// �������~
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
										//sprintf( szStr, "�������~[ %d ][ %d ]��",iItemID, pQuest->iReward_ItemVal[ index ]  );
										//NetSrv_Talk::SysMsg( pObj->GUID(),szStr );
									}
									else
										break;
								}
								//NetSrv_Talk::SysMsg( pObj->SockID(), "�������~" );
								
							}

						// ������ܪ����~
						//--------------------------------------------------------------------
							if( iChoiceID != -1 )
							{
								if( iChoiceID <= _MAX_SIZE_QUEST_ITEM && pQuest->iReward_ChoiceID[ iChoiceID ] != 0 )
								{
									pRole->Sc_GiveItemToBuf(  pQuest->iReward_ChoiceID[ iChoiceID ],  pQuest->iReward_ChoiceVal[ iChoiceID] , EM_ActionType_PlotGive , NULL , "" );
								}
								//NetSrv_Talk::SysMsg( pObj->GUID(), "��ܪ��~" );
							}

						// �����W�n
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

						// ���� Money
						//--------------------------------------------------------------------
							if( pQuest->iReward_Money != 0 )
							{
								pRole->AddPlotMoney( iQuestLV,  pQuest->iReward_Money * RoleDataEx::GlobalSetting.QuestMoneyRate );
							}

						// ���� EXP
						//--------------------------------------------------------------------
							float BaseExpRate =( 1 + pRole->TempData.Attr.Mid.Body[EM_WearEqType_Daily_EXP_Rate]/100 );
							int iExp = 0;
							if( pQuest->iReward_Exp != 0 )
							{
								if( pQuest->iQuestGroup != 2 )
									BaseExpRate = 1;

								iExp = pRole->AddPlotExp( iQuestLV, int(pQuest->iReward_Exp * BaseExpRate) );
							}

						// ���� TP
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
						//���S��f��
							Global::AddMoneyBase( pRole , EM_ActionType_PlotGive , pQuest->iReward_CoinType , pQuest->iReward_SpecialCoin );

						//�����S��f��
							Global::AddMoneyBase( pRole , EM_ActionType_PlotGive , pQuest->iRequest_CoinType , ( pQuest->iRequest_SpecialCoin * -1 ) );


							// �ˬd SCRIPT ���A
							//if( pQuestNPC->PlayerTempData->ScriptInfo.emMode == EM_SCRIPT_RUNNING )

							{									
								Global::Log_Quest( pRole, iQuestID, 1 );
								pRole->Quest_Complete( iQuestField, iQuestID );
								
								// �ˬd�O�_���������ȼ@��
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

						// ��������, �ó]�w QuestFlag
							
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
			// �����`
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


	// �ϥλ��j�ˬd����
		RoleDataEx*			pMember = NULL;
		PartyInfoStruct*	Party	= PartyInfoListClass::This()->GetPartyInfo( pKiller->BaseData.PartyID );	

		if( Party != NULL )
		{
			int iPartySize = (int)Party->Member.size();
			
			for( int i = 0; i < iPartySize; i++ )
			{
				// �p�G�b���P�ϰ�, �h������ Member ���
					pMember = Global::GetRoleDataByGUID( Party->Member[ i ].GItemID );

				if( pMember && pMember != pKiller && pKiller->Length( pMember ) <= _MAX_PARTY_SHARE_RANGE_ )	
				{
					CheckQuestRoleDead( pMember, pDeader, pDropList, pOwnerList , pLockList, true );
					
				}
			}
		}

	// �ˬd�ۤv
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

				// �ˬd���⨭�W���L��������
				for( int j = 0; j < _MAX_QUEST_COUNT_; j++ )
				{
					if( pQuestBaseData->QuestLog[ j ].iQuestID == iQuestID )
					{
						//pQuest = &( pDeadObj->pQuestLink[ i ]->QuestDetail );
						GameObjDbStructEx	*pQuestObj = g_ObjectData->GetObj( iQuestID );
						
						if( pQuestObj == NULL || ( pQuestObj->QuestDetail.bDisableTeam == true && bTeam == true ) )
							return;

						pQuest = &( pQuestObj->QuestDetail );

						// �ˬd���˭p��
						for( int k = 0; k < _MAX_SIZE_QUEST_ITEM; k++ )
						{
							if( pQuest->iRequest_KillID[ k ] == iDeadObjID )
							{
								// ���X�ثe���, �ˬd�O�_�F��
								int		KillRequest	= pQuest->iRequest_KillVal[ k ];
								//byte&	KillVal		= pKiller->PlayerBaseData->Quest.iQuestKillCount[ j ][ k ];
								byte&	KillVal		= pKiller->PlayerBaseData->Quest.QuestLog[ j ].iQuestKillCount[ k ];
								
								if( KillRequest > KillVal )
								{
									// �q�� Client �p�Ʊ��ļ�
									KillVal++;
									char szStr[256];
									sprintf( szStr, "������ȥؼЭp�� [ %d / %d ]",KillVal, KillRequest  );									
									NetSrv_Talk::SysMsg( pKiller->GUID(),szStr );

									CNetSrv_Script::KillObjNotity( pKiller->GUID(), j, iQuestID, iDeadObjID );
								}
							}
						}

						// �ˬd���˵������~
						for( int k = 0; k < _MAX_SIZE_QUEST_CONDITION; k++ )
						{
							if( pQuest->iProcess_KilltoGetItem_ObjID[ k ] == iDeadObjID )
							{
								// ���X�ثe���, �ˬd�������~�ƶq�O�_����

								int		iDropPercent	= pQuest->iProcess_KilltoGetItem_ProcPercent[ k ];
								int		iDropItem		= pQuest->iProcess_KilltoGetItem_GetItem[ k ];
								int		iDropItemVal	= pQuest->iProcess_KilltoGetItem_GetItemVal[ k ];

								// �ˬd���⨭�W�λȦ�Ҿ֦������~�O�_����
								bool	bEnough			= false;
								int		iLackNum		= iDropItemVal;

								for( int k = 0; k < _MAX_SIZE_QUEST_CONDITION; k++ )
								{
									int iRequestItemID	= pQuest->iRequest_ItemID[ k ];

									if( iRequestItemID == iDropItem )
									{
				
										// �j�M���⨭�W�λȦ�
										int iVal		= pKiller->Sc_GetItemNumFromRole( iRequestItemID );

										// �ˬd���~ Queue �̭�
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
										// �Ӿ��v����
										int iRand		= rand() % 100;

										if( iDropPercent >= iRand )
										{

											bDrop = true;

											// �������~
											//pKiller->Sc_GiveItemToBuf( iDropItem, iDropItemVal , EM_ActionType_PlotGive , NULL );

											char szStr[256];
											sprintf( szStr, "������ȥؼХ��Ȫ��~[ %d ][ %d ]��",iDropItem, iDropItemVal  );
											NetSrv_Talk::SysMsg( pKiller->GUID(),szStr );

										}
										else
										{
											bDrop = false;
											char szStr[256];
											sprintf( szStr, "������ȥؼЦ��S�������Ȫ��~" );
											NetSrv_Talk::SysMsg( pKiller->GUID(),szStr );

										}
									}
									else
									{
										// �@�w����
										// �������~
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
	// �ˬd Quest ���˥ؼг]�w


	//pKiller->CheckQuest_RequestKill_AddCount( pDeader->BaseData.ItemInfo.OrgObjID );
	QuestTempDataStruct*	pQuestTempData	= &( pKiller->TempData.QuestTempData );

	for( int index = 0; index < _MAX_QUEST_COUNT_; index++ )
	{
		if( pQuestTempData->QuestKill[ index ].iKillObj[ 0 ] != 0 )
		{
			// �ˬd�O��¥��˥ؼЭp��, �٬O���v�������~
			//----------------------------------------------------------------------
			if( pQuestTempData->QuestKill[ index ].iKillObj[ 0 ] != 0 )
			{
				// ��¥��˥ؼЭp��
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
								sprintf( szStr, "������ȥؼЭp�� [ %d / %d ]",KillCount, iKillVal  );
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
				// ���v�������~
				for( int iKillIndex = 0; iKillIndex < _MAX_QUEST_KILLKIND_COUNT_; iKillIndex++ )
				{
					if( pQuestTempData->QuestKill[ index ].iDropItem[ iKillIndex ] != 0 )
					{
						if( pQuestTempData->QuestKill[ index ].iKillObj[ iKillIndex ] == iDeadObjID )
						{							
							// �ˬd�H�����W�λȦ�S���~, �ƶq�O�_�w�F�n�D�ƶq
							
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

								int		iValue = 0; // ���W�λȦ�Ҿ֦����ƶq

								for( int iRequestIndex = 0; iRequestIndex < _MAX_QUEST_CLICKIND_COUNT_; iRequestIndex++ )
								{
									if( pQuest->iRequest_ItemID[ iRequestIndex ] == iDropItem )
									{
										if( pQuest->iRequest_ItemVal[ iRequestIndex ] <= iValue )
										{
											return;	// �w�F��n�D���ƶq, ���A����
										}
										else
											break;
									}
								}

								if( fDropPercent != 100 )
								{
									// �Ӿ��v����
									int iPercent	= (int)( fDropPercent );
									int iRand		= rand() % 100;

									if( iPercent >= iRand )
									{
										// �������~
										pKiller->Sc_GiveItemToBuf( iDropItem, iDropItemVal );

										char szStr[256];
										sprintf( szStr, "������ȥؼХ��Ȫ��~[ %d ][ %d ]��",iDropItem, iDropItemVal  );
										NetSrv_Talk::SysMsg( pKiller->GUID(),szStr );

									}
									else
									{
										char szStr[256];
										sprintf( szStr, "������ȥؼЦ��S�������Ȫ��~" );
										NetSrv_Talk::SysMsg( pKiller->GUID(),szStr );


									}
								}
								else
								{
									// �@�w����
									// �������~
									pKiller->Sc_GiveItemToBuf( iDropItem, iDropItemVal );
									char szStr[256];
									sprintf( szStr, "������ȥؼХ��Ȫ��~[ %d ][ %d ]��",iDropItem, iDropItemVal  );
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
	// �C�����
	// �n�J���ˬd���Х��Ȯɶ��O�_�W�L
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

			//�p��e�ѭ��m�ɶ������ ( �ثe�w�]�O���W 06:00 )

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
				bReset = true;	// �W�L�ɶ�, �i�H����
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
											
				// �C����ȭ��m
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


		// �ˬd���Ȧs���s�b, �]��������, ���i�����ȧR�� ( �Q�� TAG ). �ҥH�w�g���F���Ȫ��H�n�t�������R��
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
			// �N�Ҧ����n���~���X�ˬd, �Y�O�C�魫�m�h�M��

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


	// ���ȸ�Ƴ��� ( �q g_ObjectData ���N������X, �[�֨C���ˬd���t�� )
	//--------------------------------------------------------------------
		// ���˥ؼ�
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

				// �ˬd�γ]�w���˥ؼ�
					pRole->CheckQuest_RequestKill_SetTempData( index, pQuest );

				// �ˬd�γ]�w���˥ؼЫᱼ�����~
					pRole->CheckQuest_KillToGetItem_SetTempData( index, pQuest );
			}
			else
				break;
		}
		*/

		// �������~
}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_ClickQuestObj	( BaseItemObject*	pObj, int iQuestField, int iQuestID, int iClickObjID, int iClickObj )
{
	// ���J�ƥ�

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

	// �ˬd��w�̪���m�P�Q�I�����󤧶����Z��, �Y�W�L�w�]�d��h����
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
			pClickObjData->iLockGUID	= 0; // ����
			pClickObjData->iLockTime	= 0;
		}
		else
		{

			if( pSourceLocker->Role()->Length( pClickObj->Role() ) > 100 ) 
			{
				pClickObjData->iLockGUID = 0; // ����
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
		NetSrv_Talk::SysMsg( pRole->GUID(),"On_ClickQuestObj: �Ӫ���w�Q��w�Τw�Q�R��" );
		return;
	}
	*/

	if( pClickObj->Role()->TempData.Sys.SecRoomID == -1 )
	{
		OnCast_QuestObj( iGUID, EM_GATHER_NULLOBJ );
		NetSrv_Talk::SysMsg( pRole->GUID(),"On_ClickQuestObj: �w�Q�R��" );
		Print( Def_PrintLV3 , "On_ClickQuestObj" ,"QuestObj doesnt exist!!" );
		return;
	}

	if( pRole->PlayerBaseData->Quest.QuestLog[ iQuestField ].iQuestID != iQuestID )
	{
		OnCast_QuestObj( iGUID, EM_GATHER_FAILED );
		Print( Def_PrintLV3 , "On_ClickQuestObj" ,"QuestField and QuestID doesnt match!!" );
		return;
	}

	// ���X Quest ��ƨ��ˬd���⨭�W�O�_�w�o�쨬�������~
	GameObjDbStructEx		*pObjDB = Global::GetObjDB( iQuestID );
	
	if( pObjDB )
	{
		GameObjectQuestDetail *pQuest = &( pObjDB->QuestDetail );

		int iItem		= 0;
		int iItemCount	= 0;

		// �ھ� Quest ����ˬd
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
					// �ˬd���L�@���ݭn����
					int iLen = (int)strlen( pQuest->szScript_OnClickObjBegin );
					if( iLen != 0 )
					{
						BaseItemObject*		pTarget = Global::GetObj( iClickObjID );
						

						if( pTarget )
						{
							// �ˬd�Ӫ���O�_���b����
							
							// ����@��
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
								//NetSrv_Talk::SysMsg( pRole->GUID(),"On_ClickQuestObj: Script() �S���ѼƶǦ^" );
								return;
							}



							//pObj->PlotVM()->CallLuaFunc( pQuest->szScript_OnClickObjBegin, iClickObjID, 0 );
/*							int iReturn = pObj->PlotVM()->PCallReturn( pQuest->szScript_OnClickObjBegin, iGUID, iClickObjID);

							if( iReturn != 1 )
								return;	// �@���ˬd���q�L������
								*/
						}				
					}


					// �B�~���]�w
						pCastingData->iContext	= iQuestID; 
					
					// �}�l�ɶ���				
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

	// �@�θ��
	//--------------------------------------------------------------------------------------------------------------------
	RoleDataEx*			pClickObj		= NULL;
	GameObjDbStructEx*	pClickObjDB		= NULL;

	bool				bFailed			= false;

	//int					iGUID			= (int)InputClass;
	BaseItemObject*		pObj			= BaseItemObject::GetObj( iGUID );
	BaseItemObject*		pClickBaseObj	= NULL;

	// �ˬd�H���٦b���b
	//--------------------------------------------------------------------------------------------------------------------
		if( pObj == NULL )	
		{
			return;	// �H���w�g�U�u�F, �άO�_�u, �ϥ��N�O�H�����󤣦s�b, ���}���B�z, �H�]���b�F
		}

		RoleDataEx			*pRole			= pObj->Role();						// �I�����󪺤H��
		CastingData*		pCastingData	= &pRole->PlayerTempData->CastData;		// �I�����

		//int					iQuestID		= pCastingData->iQuestID;
		//int					iClickObj		= pCastingData->iClickObj;
		int					iClickObjID		= pCastingData->iClickObjID;

		pClickBaseObj	= Global::GetObj( iClickObjID );	// ���X�Q�I������

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

			// ���m�I�i���
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


	// �@�θ��
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

	// �ˬd�H���٦b���b
	//--------------------------------------------------------------------------------------------------------------------
		if( pObj == NULL )	
		{
			return;	// �H���w�g�U�u�F, �άO�_�u, �ϥ��N�O�H�����󤣦s�b, ���}���B�z, �H�]���b�F
		}

		RoleDataEx			*pRole			= pObj->Role();						// �I�����󪺤H��
		CastingData*		pCastingData	= &pRole->PlayerTempData->CastData;		// �I�����

		//int					iQuestID		= pCastingData->iQuestID;
		//int					iClickObj		= pCastingData->iClickObj;
		int					iClickObjID		= pCastingData->iClickObjID;

		pClickBaseObj	= Global::GetObj( iClickObjID );	// ���X�Q�I������

		//*�Ȯɭק�*
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

			// ���m�I�i���
			pCastingData->Clear();


			// ���X Quest ��ƨõ����Ӫ��~
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
				// ���}��w
				pTarget->Role()->PlayerTempData->CastData.iLockGUID = 0;


				// �ˬd���L�@���ݭn����
				if( strlen( pQuest->szScript_OnClickObjEnd ) != 0 )
				{

					// ����@��
					//pObj->PlotVM()->CallLuaFunc( pQuest->szScript_OnClickObjBegin, iClickObjID, 0 );
					/*
					int iReturn = pObj->PlotVM()->PCallReturn( pQuest->szScript_OnClickObjEnd, iGUID, iClickObjID);

					if( iReturn != 1 )
					return 0;	// �@���ˬd���q�L������
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
								return;	// �@���ˬd���q�L������
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
						NetSrv_Talk::SysMsg( pRole->GUID(),"OnEvent_CastingFinished: Script() �S���ѼƶǦ^" );
						ClickQuestObjResult( pRole->GUID(), EM_GATHER_FAILED, 0, 0 );
						return;
					}
				}
			}

			bool	bResult = true;
			int		iItem, iItemVal, iPercent;


			// �ھ� Quest ��Ƶ������~ )
			for( int index = 0; index < _MAX_SIZE_QUEST_CONDITION; index++ )
			{
				if( pQuest->iProcess_ClickToGetItem_ObjID[ index ] == iClickObj )
				{
					iPercent	= pQuest->iProcess_ClickToGetItem_ProcPercent[ index ];
					iItem		= pQuest->iProcess_ClickToGetItem_GetItem[ index ];
					iItemVal	= pQuest->iProcess_ClickToGetItem_GetItemVal[ index ];

					if( iPercent == 100 )
					{
						// �@�w����
						pRole->Sc_GiveItemToBuf( iItem,  iItemVal , EM_ActionType_PlotGive , NULL , "" );
						bResult = true;
						break;
					}
					else
					{
						// �Ӿ��v����
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


			// �ˬd�Y�O�ݭn�����I�諸��������

			if( pTarget != NULL )
				pTarget->Role()->SetDisableClick( true );


			// ����Q�I����
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

	// ���m�I�i���
	pCastingData->Clear();


	// ���X Quest ��ƨõ����Ӫ��~
	GameObjDbStructEx		*pObjDB = Global::GetObjDB( iQuestID );
	if( !pObjDB ) { Print( Def_PrintLV3 , "OnEvent_CastingFinished" , "GetObjDB() Failed!!" ); return 0; }

	GameObjectQuestDetail*	pQuest	= &( pObjDB->QuestDetail );
	BaseItemObject*			pTarget = Global::GetObj( iClickObjID );		

	
	if( pTarget != NULL )
	{
		// ���}��w
		pTarget->Role()->PlayerTempData->CastData.iLockGUID = 0;


		// �ˬd���L�@���ݭn����
		if( strlen( pQuest->szScript_OnClickObjEnd ) != 0 )
		{
									
			// ����@��
			//pObj->PlotVM()->CallLuaFunc( pQuest->szScript_OnClickObjBegin, iClickObjID, 0 );
			
			//int iReturn = pObj->PlotVM()->PCallReturn( pQuest->szScript_OnClickObjEnd, iGUID, iClickObjID);

			//if( iReturn != 1 )
				//return 0;	// �@���ˬd���q�L������
				

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
						return 0;	// �@���ˬd���q�L������
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
				NetSrv_Talk::SysMsg( pRole->GUID(),"OnEvent_CastingFinished: Script() �S���ѼƶǦ^" );
				return 0;
			}
		}
	}

	bool	bResult = true;
	int		iItem, iItemVal, iPercent;


	// �ھ� Quest ��Ƶ������~ )
	for( int index = 0; index < _MAX_SIZE_QUEST_CONDITION; index++ )
	{
		if( pQuest->iProcess_ClickToGetItem_ObjID[ index ] == iClickObj )
		{
			iPercent	= pQuest->iProcess_ClickToGetItem_ProcPercent[ index ];
			iItem		= pQuest->iProcess_ClickToGetItem_GetItem[ index ];
			iItemVal	= pQuest->iProcess_ClickToGetItem_GetItemVal[ index ];

			if( iPercent == 100 )
			{
				// �@�w����
				pRole->Sc_GiveItemToBuf( iItem,  iItemVal , EM_ActionType_PlotGive , NULL , "" ); 
			}
			else
			{
				// �Ӿ��v����
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


	// �ˬd�Y�O�ݭn�����I�諸��������
	pTarget->Role()->SetDisableClick( true );


	// ����Q�I����
	Global::ResetObj( iClickObjID );


	return 0;
}
*/
//-------------------------------------------------------------------
void		CNetSrv_Script_Child::On_TouchEvent( BaseItemObject*	pObj, int iEventObjID )
{
    //�d���ˬd
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
    //�d���ˬd
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
	// �I���ˬd
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
	// �߬d�� NPC �O�_���@���i��Ĳ�o
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

			// ���m��ܸ��
			pRole->PlayerTempData->SpeakInfo.Reset();

			pRole->PlayerTempData->SpeakInfo.iSpeakNPCID = iNPCID;

			//pObj->PlotVM()->PCall( Target->TempData.TouchPlot.Begin() , iNPCID , 0 );

				vector< MyVMValueStruct > vecArg;
				LUA_VMClass::PCallByStrArg( pObjDB->NPC.szLuaScript, pObj->GUID(), pRole->PlayerTempData->SpeakInfo.iSpeakNPCID, &vecArg );

			// �ˬd�s�W���ﶵ, �ç@�X�ʥ]�ǰe�� Client			
				SpeakOption( pObj->GUID() );			
		}
	}	
}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::On_ChoiceOption ( BaseItemObject*	pObj, int iOption )
{
	RoleDataEx* pRole = pObj->Role();

	// �o�e�@�ӫʥ]�� Client �H�K�i�����B�z
	ChoiceOptionResult( pObj->GUID() );
	
	if( iOption <= pRole->PlayerTempData->SpeakInfo.SpeakOption.Size() && iOption >= 1 )
	{
		// �q��Ƥ����X Option ���, �i��B�z
			SpeakOptionStruct	Option	= pRole->PlayerTempData->SpeakInfo.SpeakOption[ ( iOption - 1 ) ];
			int					iNPCID	= pRole->PlayerTempData->SpeakInfo.iSpeakNPCID;

		// ���m��ܸ��
			pRole->PlayerTempData->SpeakInfo.Reset();

		// �i�� Lua �I�s
			vector< MyVMValueStruct > vecArg;
			LUA_VMClass::PCallByStrArg( Option.szLuaFunc.Begin(), pObj->GUID(), iNPCID, &vecArg );
		
		// �ˬd�s�W���ﶵ, �ç@�X�ʥ]�ǰe�� Client
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
	// ����ȧ�����, ����@��, �@��������Ĳ�o
	
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
		pRole->PlayerTempData->CastData.bBeHit	= true;		// �]�w�I�k�L�{�Q����
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
		if( pRole->PlayerBaseData->Quest.QuestLog[ i ].iMode == 1 )	// ���s�W�u��
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
	// ��w�s�u���C�Ө���o�X���m�ʥ], �ñN Role ��ƪ��C����ȳ]�w����
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
				// ���m�C�����
				pRole->PlayerBaseData->Quest.QuestDaily.ReSet();

				// ���m�s�ե���
				for( int i=0; i<DF_QUESTDAILY_MAXGROUP; i++ )
				{
					pRole->PlayerBaseData->Quest.iQuestGroup[i] = 0;
				}

				pRole->PlayerBaseData->Quest.LastCompleteTime = (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60;;

				// �o�e�ʥ]�q�� Client ���m����
				CNetSrv_Script::ResetDailyQuest( pObj->GUID(), 1 );
			}
		}
	}
	*/

	if( pRole != NULL && pRole->IsPlayer() )
	{
		Print( LV2, "ResetDaily", "ResetDailyQuest - account[ %s ]", pRole->Base.Account_ID.Begin() );

		// ���m�C�����
		pRole->PlayerBaseData->Quest.QuestDaily.ReSet();

		// ���m�s�ե���
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
		

		// �o�e�ʥ]�q�� Client ���m����
		CNetSrv_Script::ResetDailyQuest( pRole->GUID(), 1 );
	}

}
//-------------------------------------------------------------------
void CNetSrv_Script_Child::ResetDailyFlag( RoleDataEx *pRole, bool bForceReset )
{
	// �C�魫�n���~�M������
	if( pRole != NULL && ( pRole->PlayerBaseData->TimeFlag[ EM_TimeFlagEffect_DailyFlag ] <= 0  || bForceReset == true ) )
	{
		// �N�Ҧ����n���~���X�ˬd, �Y�O�C�魫�m�h�M��

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

