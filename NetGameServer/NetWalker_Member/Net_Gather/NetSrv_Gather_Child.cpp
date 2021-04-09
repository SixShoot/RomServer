#include "../Net_Talk/NetSrv_Talk.h"		// for NetSrv_Talk::SysMsg()
#include "../../MainProc/FlagPosList/FlagPosList.h"

#include "NetSrv_Gather_Child.h"

#include "Rune Engine\rune\fusion\RuFusion_Actor.h"

#include <math.h>

//GatherCallBackFunc	CNetSrv_Gather_Child::m_pGatherCastFunc = NULL;
//-------------------------------------------------------------------
bool    CNetSrv_Gather_Child::Init()
{
	CNetSrv_Gather::Init();

	if( m_pThis != NULL)
		return false;

	m_pThis = NEW( CNetSrv_Gather_Child );

	Global::Schedular()->Push( OnEvent_MineCheck, 600000, NULL, NULL );

	return true;
}

//-------------------------------------------------------------------
bool    CNetSrv_Gather_Child::Release()
{
	if( m_pThis == NULL )
		return false;

	delete m_pThis;
	m_pThis = NULL;

	CNetSrv_Gather::Release();
	return false;
}
//-------------------------------------------------------------------
bool CNetSrv_Gather_Child::SetCast( RoleDataEx *pRole, int iClickObjID, int iTime, GatherCallBackFunc pFunc, const char* pszLuaEvent )
{
	int					iGUID			= pRole->GUID();
	CastingData*		pCastingData	= &pRole->PlayerTempData->CastData;
	//BaseItemObject*		pClickObj		= Global::GetObj( iClickObjID );
	RoleDataEx*			pClickObj		= Global::GetRoleDataByGUID( iClickObjID );

	DWORD				dwTick			= GetTickCount();

	//if( m_pGatherCastFunc != NULL )
	//	return;

	if( pClickObj == NULL && iClickObjID != 0 )
		return false;

	if( pCastingData->pCastFunc != NULL )
	{
		((GatherCallBackFunc)pCastingData->pCastFunc)( iGUID, EM_GATHER_FAILED );
		pCastingData->pCastFunc		= NULL;
		
		if( pCastingData->iEventHandle	!= -1 )
		{
			Global::Schedular()->Erase( pCastingData->iEventHandle );
			pCastingData->iEventHandle = -1;
		}

		//if( pClickObj != NULL && iGUID == pClickObj->Role()->PlayerTempData->CastData.iLockGUID )
		if( pClickObj != NULL && iGUID == pClickObj->TempData.iLockGUID )
		{
			pClickObj->TempData.iLockGUID	= iGUID;
			pClickObj->TempData.iLockTime	= dwTick + 1000;
			//pClickObj->Role()->PlayerTempData->CastData.iLockGUID	= iGUID;
			//pClickObj->Role()->PlayerTempData->CastData.iLockTime	= dwTick + 1000;
		}

		return false;
	}
	else
		pCastingData->pCastFunc = (PVOID)pFunc;

	// 存入資料讓
	pCastingData->iClickObjID		= iClickObjID;
	pCastingData->SourcePos			= pRole->BaseData.Pos;
	pCastingData->szLuaEvent		= pszLuaEvent;

	// 點擊物件代號 -1 表示不用實體物件, 比方說, 包包中的信
	if( iClickObjID != 0 )
	{
		// Cancle buff

		//清除Buff
		for( int i = 0 ; i < pRole->BaseData.Buff.Size() ; i++ )
		{
			GameObjDbStructEx*	pBuffObj		= Global::GetObjDB( pRole->BaseData.Buff[i].BuffID );
			if( pBuffObj->MagicBase.Effect.Sneak != false || pBuffObj->MagicBase.Effect.Cover != false )
			{
				pRole->ClearBuff( pRole->BaseData.Buff[i].BuffID , -1 );
			}
		}
		

		if( pClickObj == NULL && pCastingData->pCastFunc != NULL )
		{
			((GatherCallBackFunc)pCastingData->pCastFunc)( iGUID, EM_GATHER_NULLOBJ );
			pCastingData->pCastFunc		= NULL;

			if( pCastingData->iEventHandle	!= -1 )
			{
				Global::Schedular()->Erase( pCastingData->iEventHandle );
				pCastingData->iEventHandle = -1;
			}

			return false;
		}
		else
		{
			// 鎖住被點的物件
			//CastingData*		pClickObjData	= &pClickObj->Role()->PlayerTempData->CastData;

			// 檢查物件有無被鎖定
			//if( pClickObjData->iLockGUID != 0 && (GatherCallBackFunc)pCastingData->pCastFunc != NULL )
			if( pClickObj->TempData.iLockGUID != 0 && (GatherCallBackFunc)pCastingData->pCastFunc != NULL )
			{
				BaseItemObject*		pLockSource			= Global::GetObj( pClickObj->TempData.iLockGUID );
				int					iRemainLockTime		= 0;
				
				//if( pClickObj->Role()->PlayerTempData->CastData.iLockTime != 0 )
				if( pClickObj->TempData.iLockTime != 0 )
				{
					//iRemainLockTime = (int)pClickObj->Role()->PlayerTempData->CastData.iLockTime - (int)dwTick;
					iRemainLockTime = (int)pClickObj->TempData.iLockTime - (int)dwTick;
				}
				
				// 如果被點擊物件存在, 且該物件的鎖定者存在, 且距離和該物件在 50 之內, 就通知點擊者該物件忙錄
				if( pLockSource != NULL && iRemainLockTime > 0 && ( pClickObj->TempData.iLockGUID != iGUID && pLockSource->Role()->Length( pClickObj ) < 120 ) )
				{
					((GatherCallBackFunc)pCastingData->pCastFunc)( iGUID, EM_GATHER_OBJBUSY );
					pCastingData->pCastFunc		= NULL;
					if( pCastingData->iEventHandle	!= -1 )
					{
						Global::Schedular()->Erase( pCastingData->iEventHandle );
						pCastingData->iEventHandle = -1;
					}
					return false;
				}
			}
		}

		pClickObj->TempData.iLockGUID = iGUID;
		pClickObj->TempData.iLockTime = dwTick + iTime + 1000;
	}

	pCastingData->iEndTime = dwTick + iTime;
		
	// 推入事件, 時間到後予採集物品

	if( pCastingData->iEventHandle != -1 )
	{
		Global::Schedular()->Erase( pCastingData->iEventHandle );
		Print( LV1, "GATHER", "erase gather event [%d]", pCastingData->iEventHandle  );
	}

	((GatherCallBackFunc)pCastingData->pCastFunc)( iGUID, EM_GATHER_BEGIN );
	pCastingData->iEventHandle			= Global::Schedular()->Push( OnEvent_GatherCast , _TIME_CHECKCASTEVENT_, (VOID*)iGUID, NULL );

	return true;
}

//-------------------------------------------------------------------
void CNetSrv_Gather_Child::On_ClickGatherObj ( BaseItemObject*	pObj, int iClickObjID )
{
	// 推入事件
	RoleDataEx			*pRole			= pObj->Role();
	int					iGUID			= pRole->GUID();

	BaseItemObject*		pClickObj		= Global::GetObj( iClickObjID );

	if( pClickObj == NULL || pClickObj->Role()->TempData.Sys.SecRoomID == -1 )	// == -1 表示不在分割區內
	{
		CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_NULLOBJ );
		return;
	}

	// 防止同一定點機器人掛礦, 要是人物不移動超告 100, 連續採礦 20 次, 則讓礦失效
	if( pClickObj->Role()->Length( pRole->PlayerTempData->MineCheck.X , pRole->PlayerTempData->MineCheck.Y , pRole->PlayerTempData->MineCheck.Z ) < 100 )
	{
#ifndef _DEBUG
		pRole->PlayerTempData->MineCheck.Count++;
		if( pRole->PlayerTempData->MineCheck.Count > 20 )
		{
			CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_NULLOBJ );
			return;
		}
#endif
	}
	else
	{
		pRole->PlayerTempData->MineCheck.X = pClickObj->Role()->X();
		pRole->PlayerTempData->MineCheck.Y = pClickObj->Role()->Y(); 
		pRole->PlayerTempData->MineCheck.Z = pClickObj->Role()->Z();
		pRole->PlayerTempData->MineCheck.Count = 0;
	}

	// 距離檢查
	if( pClickObj->Role()->Length( pRole ) > 100 )
	{
		CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_NULLOBJ );
		return;
	}



	CastingData*		pCastingData	= &pRole->PlayerTempData->CastData;
	CastingData*		pClickObjData	= &pClickObj->Role()->PlayerTempData->CastData;
	GameObjDbStructEx*	pClickObjDB		= Global::GetObjDB( pClickObj->Role()->BaseData.ItemInfo.OrgObjID );

	if( pClickObjDB->Classification != EM_ObjectClass_Mine )
	{
		CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_NULLOBJ );
		return;
	}

	/*
	if( pClickObjData->iLockGUID != 0 || pClickObj->Role()->TempData.Sys.SecRoomID == -1 )
	{
		NetSrv_Talk::SysMsg( pRole->GUID(),"該物件已被鎖定或已被刪除" );
		return;
	}
	*/

	// 檢查技能等級
	int iSkillLV = 0;
	switch( pClickObjDB->Mine.Type )
	{
	case EM_GameObjectMineType_Mine: iSkillLV = (int)pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Mining ); break;
	case EM_GameObjectMineType_Wood: iSkillLV = (int)pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Lumbering ); break;
	case EM_GameObjectMineType_Herb: iSkillLV = (int)pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Herblism ); break;
	case EM_GameObjectMineType_Fish: iSkillLV = (int)pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Fishing ); break;
	}

	// 檢查技能夠不夠
	if( iSkillLV < pClickObjDB->Mine.LvLimit )
	{
		CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_LOWSKILL );
		return; // 技能不夠
	}

	float	fGatherTime = (float)_TIME_CLICKTOGETITEM;

	if( pRole->TempData.Attr.Mid.Body[ EM_WearEqType_GatherSpeedRate ] != 0 )
	{
		float fRate = pRole->TempData.Attr.Mid.Body[ EM_WearEqType_GatherSpeedRate ] / (float)100;
		fGatherTime = fGatherTime / ( 1 + fRate );
	}

	// 推入事件, 時間到後予採集物品
	//pCastingData->iEventHandle		= Global::Schedular()->Push( OnEvent_CastingFinished , _TIME_CHECKCASTEVENT_, (VOID*)iGUID, NULL );	
	if( CNetSrv_Gather_Child::SetCast( pRole, iClickObjID, fGatherTime,  OnCast_GatherObj, "" ) != false )
	{
		// 讓點擊者播放指定採集動作

		switch( pClickObjDB->Mine.Type )
		{
		case EM_GameObjectMineType_Mine:
			CNetSrv_Gather::GatherMotion( iGUID, ruFUSION_ACTORSTATE_MINING_BEGIN, pClickObjDB->Mine.Type ); break;
		case EM_GameObjectMineType_Wood:
			CNetSrv_Gather::GatherMotion( iGUID, ruFUSION_ACTORSTATE_LUMBERJACK_BEGIN, pClickObjDB->Mine.Type );		break;
		case EM_GameObjectMineType_Herb:
			CNetSrv_Gather::GatherMotion( iGUID, ruFUSION_ACTORSTATE_GATHER_BEGIN, pClickObjDB->Mine.Type );	break;
		}
	}
}
//-------------------------------------------------------------------SetCast
int CNetSrv_Gather_Child::OnEvent_GatherCast( SchedularInfo* Obj , void* InputClass)
{
	ENUM_GATHER_RESULT	emResult		= EM_GATHER_NONE;
	
	
	RoleDataEx*			pClickObj		= NULL;
	GameObjDbStructEx*	pClickObjDB		= NULL;

	bool				bFailed			= false;

	int					iGUID			= (int)InputClass;
	BaseItemObject*		pObj			= BaseItemObject::GetObj( iGUID );
	BaseItemObject*		pClickBaseObj	= NULL;

	// 檢查人物還在不在
	//--------------------------------------------------------------------------------------------------------------------
	if( pObj == NULL )	
	{
		return 0;	// 人物已經下線了, 或是斷線, 反正就是人物物件不存在, 跳開不處理, 人也不在了
	}

	RoleDataEx			*pRole			= pObj->Role();						// 點擊物件的人物
	CastingData*		pCastingData	= &pRole->PlayerTempData->CastData;		// 點擊資料

	if( pRole->PlayerTempData->CastData.pCastFunc == NULL )
	{
		Print( LV5, "GATHER", "pCastingData->pCastFunc = NULL" );
		pCastingData->iEventHandle = -1;
		return 0;
	}


	if( pRole->PlayerTempData->CastData.iEventHandle == -1 )
	{
		Print( LV5, "GATHER", "pCastingData->iEventHandle = -1" );
	}


	int					iQuestID		= pCastingData->iContext;
	//int					iClickObj		= pCastingData->iClickObj;
	int					iClickObjID		= pCastingData->iClickObjID;

	if( iClickObjID != 0 )
	{

		pClickBaseObj	= Global::GetObj( iClickObjID );	// 取出被點擊物件
		if( pClickBaseObj == NULL || pClickBaseObj->Role()->TempData.Sys.SecRoomID == -1 )	
		{
			bFailed		= true;
			emResult	= EM_GATHER_NULLOBJ;
			//CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_NULLOBJ );	// 物件不存在, 有可能消失了或發生奇怪的現像, 這邊有發生過. 要查
			((GatherCallBackFunc)pCastingData->pCastFunc)( iGUID, EM_GATHER_NULLOBJ );
			pCastingData->pCastFunc		= NULL;
			pCastingData->iEventHandle	= -1;
			return 0;
		}
		else
		{
			pClickObj		=  pClickBaseObj->Role();
			pClickObjDB		= Global::GetObjDB( pClickObj->BaseData.ItemInfo.OrgObjID );	
		}
	}




	// 檢查被點擊物件還在不在
	//--------------------------------------------------------------------------------------------------------------------
	//if( pCastingData->iClickObjID == 0 || ( ( pClickBaseObj = BaseItemObject::GetObj( iClickObjID ) ) == NULL ) )
	//{
		// TODO: 應該不會發生才對, 點擊資料被清空. 若真的發生的話. Client 仍存活的話, 則必須要發出解除動作的配套封包
		// 因為資料被清空了, 所以也不可能知道是那個物件被鎖定
	//	pCastingData->Clear();

	//	m_pGatherCastFunc( iGUID, EM_GATHER_NULLOBJ );
	//	return 0;
	//}

	// 檢查人物行為是否需要中斷, 以及檢查時間是否到了
	//--------------------------------------------------------------------------------------------------------------------
	RolePosStruct	Pos				= pRole->BaseData.Pos;
	RolePosStruct	SourcePos		= pCastingData->SourcePos;

	float fDis = 0;
	{
		float X = Pos.X - SourcePos.X;
		float Y = 0;
		float Z = Pos.Z - SourcePos.Z;
		fDis = sqrt( X*X + Y*Y + Z*Z);
	}
	
	// 檢查人物有無移動
	//if( !bFailed && !(  FloatEQU( SourcePos.X , Pos.X ) && FloatEQU( SourcePos.Z ,  Pos.Z ) && FloatEQU( SourcePos.Y , Pos.Y ) ))
	float fCheckDis = 1.5;
	if( !bFailed && fDis > fCheckDis )
	{
		bFailed	= true;	// 人物移動了
		emResult	= EM_GATHER_FAILED;
	}

	// 檢查人物是不是施法過程中, 被擊中
	if( bFailed == false && pCastingData->bBeHit != false )
	{
		pCastingData->bBeHit	= false;
		bFailed					= true;
		emResult				= EM_GATHER_FAILED;
	}

	// 檢查人物是不是死亡
	if( bFailed == false && pRole->IsDead() == true )
	{
		bFailed					= true;
		emResult				= EM_GATHER_FAILED;
	}

	
	//if( pClickObj != NULL && pClickObj->PlayerTempData->CastData.iLockGUID != iGUID )
	if( pClickObj != NULL && pClickObj->TempData.iLockGUID != iGUID )
	{
		bFailed	= true;	// 人物移動了
		emResult	= EM_GATHER_FAILED;
	}
	


	// 檢查人物是不是施法

	// 檢查人物是不是使用物品

	// 檢查人物是不是改變動作

	if( bFailed == false )
	{
		// 檢查有沒有超過時間, 若時間還沒到, 則再繼續推
		unsigned int dwSystem	= GetTickCount();
		unsigned int dwEndTime	= pCastingData->iEndTime;

		if( dwEndTime > dwSystem )
		{
			pCastingData->iEventHandle		= Global::Schedular()->Push( OnEvent_GatherCast , _TIME_CHECKCASTEVENT_, (VOID*)iGUID, NULL );
			return 0;
		}
	}	

	// 重置施展資料
	//--------------------------------------------------------------------------------------------------------------------
	//pCastingData->Clear();	


	// 取出物件資料, 並解除鎖定
	//--------------------------------------------------------------------------------------------------------------------	
	if( pClickObj != NULL )
	{				
		//pClickObj->PlayerTempData->CastData.iLockGUID = 0;	// 打開鎖定
		//pClickObj->PlayerTempData->CastData.iLockTime	= GetTickCount() + 1000;
		pClickObj->TempData.iLockTime	= GetTickCount() + 1000;
	}


	// 檢查是不是失敗了, 若是則發送失敗給 Client, 並且解除 Cast 資料, 以及解鎖物件
	//--------------------------------------------------------------------------------------------------------------------
	if( bFailed )
	{
		if( pCastingData->pCastFunc )
		{
		//CNetSrv_Gather::ClickGatherObjResult( iGUID, emResult );	// 發送點擊失敗給 Client
			((GatherCallBackFunc)pCastingData->pCastFunc)( iGUID, emResult );
			pCastingData->pCastFunc = NULL;
			Global::Schedular()->Erase( pCastingData->iEventHandle );
			pCastingData->iEventHandle = -1;
		}
		return 0;
	}

	
	if( pCastingData->pCastFunc != NULL )
	{
		((GatherCallBackFunc)pCastingData->pCastFunc)( iGUID, EM_GATHER_ENDCAST );

		if( pClickObjDB != NULL )
		{
			if( strlen( pClickObjDB->Mine.szLuaGetItemScript ) != 0 )
			{
				LUA_VMClass::PCallByStrArg( pClickObjDB->Mine.szLuaGetItemScript, iGUID, iClickObjID, NULL );
			}
		}
	}
	else
	{
		Print( LV5, "GATHER", "pCastingData->pCastFunc = NULL" );
		pCastingData->iEventHandle = -1;
		return 0;
	}
	
	pCastingData->pCastFunc			= NULL;
	pCastingData->iEventHandle		= -1;


	return 0;
}
//-------------------------------------------------------------------
void CNetSrv_Gather_Child::PlayEndMotion( int iGUID, GameObjDbStructEx*	pClickObjDB	)
{
	// 播放停止動作
	if( pClickObjDB )
	{
		switch( pClickObjDB->Mine.Type )
		{
		case EM_GameObjectMineType_Mine:
			CNetSrv_Gather::GatherMotion( iGUID, ruFUSION_ACTORSTATE_MINING_END, pClickObjDB->Mine.Type  );		break;
		case EM_GameObjectMineType_Wood:
			CNetSrv_Gather::GatherMotion( iGUID, ruFUSION_ACTORSTATE_LUMBERJACK_END, pClickObjDB->Mine.Type  );		break;
		case EM_GameObjectMineType_Herb:
			CNetSrv_Gather::GatherMotion( iGUID, ruFUSION_ACTORSTATE_GATHER_END, pClickObjDB->Mine.Type );	break;
		}
	}
}
//-------------------------------------------------------------------
void CNetSrv_Gather_Child::OnCast_GatherObj ( int iGUID, int iResult )
{
	ENUM_GATHER_RESULT	emResult		= (ENUM_GATHER_RESULT)iResult;

	if( emResult == EM_GATHER_BEGIN )
	{
		CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_BEGIN );	
		return;
	}
	
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

		if( pClickBaseObj == NULL )
		{
			CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_NULLOBJ );	
			return;	// test
		}

		pClickObj		=  pClickBaseObj->Role();
		pClickObjDB		= Global::GetObjDB( pClickObj->BaseData.ItemInfo.OrgObjID );

		if( pClickObj->TempData.Sys.SecRoomID == -1 )
		{
			CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_NULLOBJ );
			CNetSrv_Gather_Child::PlayEndMotion( iGUID, pClickObjDB );
			return;
		}


		// 播放停止動作
		/*
		if( pClickObjDB )
		{
			switch( pClickObjDB->Mine.Type )
			{
			case EM_GameObjectMineType_Mine:
				CNetSrv_Gather::GatherMotion( iGUID, ruFUSION_ACTORSTATE_MINING_END, pClickObjDB->Mine.Type  );		break;
			case EM_GameObjectMineType_Wood:
				CNetSrv_Gather::GatherMotion( iGUID, ruFUSION_ACTORSTATE_LUMBERJACK_END, pClickObjDB->Mine.Type  );		break;
			case EM_GameObjectMineType_Herb:
				CNetSrv_Gather::GatherMotion( iGUID, ruFUSION_ACTORSTATE_GATHER_END, pClickObjDB->Mine.Type );	break;
			}
		}

		*/

		// 重置施法時間
		//--------------------------------------------------------------------------------------------------------------------
			pCastingData->Clear();

		if( emResult != EM_GATHER_ENDCAST )
		{			
			// 發出失敗訊息
				CNetSrv_Gather::ClickGatherObjResult( iGUID, emResult );
				CNetSrv_Gather_Child::PlayEndMotion( iGUID, pClickObjDB );
			return;
		}




	
	//--------------------------------------------------------------------------------------------------------------------
		if( emResult == EM_GATHER_ENDCAST )
		{
			// 採礦行為
			//--------------------------------------------------------------------------------------------------------------------
			bool	bResult = true;
			int		iItem, iItemVal;

			
			//-----------------------------------------------------------------------------------------------------------------------------------------

				float	fSuccessRate		= 0;				

				float	fBounsRate			= 0;
				int		iRate				= 0;	// 總和採集機率 10000 為 100%								
				float	fBounsGatherRate	= 0;	
				float	fSkillPoint			= 0.0;	// 採集技能點數
				float	fGatherObjLV		= 0.0;	// 被採集物件等級

				float	fGlobalGatherVar	= 0.7f;

				int		iSuccessRate		= 0;

				// 計算原始採集, 將所有機率加總
					for( int i = 0; i < 5; i++ )
					{
						if( pClickObjDB->Mine.GetItem[ i ] != 0 )
						{
							iRate += pClickObjDB->Mine.GetItemRate[ i ];
						}				
					}

				// 計算技能加乘後實繼技能點數
					//int		iOffsetLV	= 0;
					//int		iSkillLV	= 0;
					
					switch( pClickObjDB->Mine.Type )
					{
					case EM_GameObjectMineType_Mine: fSkillPoint = pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Mining );	break;
					case EM_GameObjectMineType_Wood: fSkillPoint = pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Lumbering ); break;
					case EM_GameObjectMineType_Herb: fSkillPoint = pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Herblism );	break;
					case EM_GameObjectMineType_Fish: fSkillPoint = pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Fishing );	break;
					}

				
					fBounsGatherRate = ((float)( pRole->TempData.Attr.Mid.Body[EM_WearEqType_SkillValue_GatherRate] ) / 100 );		// 以 100 為單會
				
				// 被採集物件等級
					fGatherObjLV = (float)pClickObjDB->Mine.LvLimit;

				// 採集控制變數
					fGlobalGatherVar = 0.7f; // 暫, 之後由 Table 加入

				
					iSuccessRate	= (int)( (float) iRate * ( fSkillPoint + 1 - fGatherObjLV ) * 1.02 * fGlobalGatherVar * ( 1 + fBounsGatherRate ) );

					if( iSuccessRate < 10000 )
					{
						int iVal =  Global::Rand( 1, 10000 );				
						if( iVal > iSuccessRate ) 
						{
							// 送出採集失敗訊息給 Client
							CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_FAILED );
							CNetSrv_Gather_Child::PlayEndMotion( iGUID, pClickObjDB );
							return;
						}
					}

				// 計算採集工具加乘 11/09/07 決定不要有採集工具這東西了
					/*
					GameObjDbStructEx*	pToolObjDB	= Global::GetObjDB( pRole->BaseData.EQ.Manufacture.OrgObjID );

					if( pToolObjDB )
					{
						int iToolLV	= pToolObjDB->Item.ManuFactureLV;
						switch( iToolLV )
						{
						case 1: fBounsRate = (float)1.00; break;
						case 2: fBounsRate = (float)1.05; break;
						case 3: fBounsRate = (float)1.10; break;
						case 4: fBounsRate = (float)1.15; break;
						case 5: fBounsRate = (float)1.20; break;	
						}
					}
					

					fSuccessRate = fSuccessRate * fBounsRate;
					*/

				
					

			if( pClickObj->TempData.iGatherNumber != 0 )
			{
				

				// 一般物品
				int iRand	= Global::Rand( 1, iRate );
				int	iCount	= 0;
				int iIndex	= 0;

				for( iIndex = 0; iIndex < 5; iIndex++ )
				{
					iCount += pClickObjDB->Mine.GetItemRate[ iIndex ];
					if( iCount >= iRand )
					{
						break;
					}
				}

				iItem		= pClickObjDB->Mine.GetItem[ iIndex ];
				iItemVal	= pClickObjDB->Mine.GetItemCount[ iIndex ];

				if( pRole->TempData.Attr.Mid.Body[ EM_WearEqType_GatherGenerateRate ] != 0 )
				{
					float fRate = pRole->TempData.Attr.Mid.Body[ EM_WearEqType_GatherGenerateRate ] / (float)100;			
					iItemVal	= iItemVal * ( 1 + fRate );
				}

				pRole->Sc_GiveItemToBuf( iItem, iItemVal , EM_ActionType_Gather , NULL , "" );		// GATHER

				// 特殊採集
				for( int i = 5; i <= 6; i++ )
				{
					if( pClickObjDB->Mine.GetItem[ i ] != 0 )
					{
						int iRareRand = Global::Rand( 1, 10000 );
						if( iRareRand <= pClickObjDB->Mine.GetItemRate[ i ] )
						{
							pRole->Sc_GiveItemToBuf( pClickObjDB->Mine.GetItem[ i ], pClickObjDB->Mine.GetItemCount[ i ] , EM_ActionType_Gather , NULL , "" );	// GATHER
						}						
					}
				}

				// 計算得到採集等集
				//-----------------------------------------------------------------------------------
					int iSkillID			= 0;
					int	iGlobalGatherExpVar = 40;			// 暫代

					// 取得基儲經驗值
						iGlobalGatherExpVar	= 40;	// 暫

					switch( pClickObjDB->Mine.Type )
					{
					case EM_GameObjectMineType_Mine: iSkillID = EM_SkillValueType_Mining;break;
					case EM_GameObjectMineType_Wood: iSkillID = EM_SkillValueType_Lumbering; break;
					case EM_GameObjectMineType_Herb: iSkillID = EM_SkillValueType_Herblism; break;
					case EM_GameObjectMineType_Fish: iSkillID = EM_SkillValueType_Fishing; break;
					}

					// 09/07 修改經驗公式
					//float fGatherExp = ( (float) iGlobalGatherExpVar ) / ( fSkillPoint + 1 - fGatherObjLV );

					int iGatherLV1		= 1;
					int iGatherLV2		= fSkillPoint - fGatherObjLV - 5;
					int iMaxGatherLV	= 1;

					if( iGatherLV2 > iGatherLV1 )
						iMaxGatherLV = iGatherLV2;


					float fGatherExp = ( (float) iGlobalGatherExpVar ) / iMaxGatherLV;

					if( pRole->TempData.Attr.Mid.Body[ EM_WearEqType_GatherExpRate ] != 0 )
					{
						float fRate = pRole->TempData.Attr.Mid.Body[ EM_WearEqType_GatherExpRate ] / 100;
						fGatherExp = fGatherExp * ( 1 + fRate );
					}
	


					pRole->AddSkillValue( pClickObjDB->Mine.LvLimit, (SkillValueTypeENUM)iSkillID, fGatherExp );

				
				//--------------------------------------------------------------------
					CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_SUCCESSED );
					CNetSrv_Gather_Child::PlayEndMotion( iGUID, pClickObjDB );
			}
			else
			{
				CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_GONE );	// 數量不夠
				CNetSrv_Gather_Child::PlayEndMotion( iGUID, pClickObjDB );
				return;
			}

			// 重置物件
			//--------------------------------------------------------------------------------------------------------------------			
				if( pClickObjDB->Mine.CreateType == EM_GameObjectCreateType_EmptyDel )
				{
					int iReduceVal	= (int)pClickObjDB->Mine.iReduceVal;
					int iBaseCount	= (int)pClickObjDB->Mine.BaseCount;

					pClickObj->TempData.iGatherNumber--;	// 減少一個礦產量
					if( pClickObj->TempData.iGatherNumber == 0 )
					{
						DelFromPartition( pClickBaseObj->GUID() );
						ResetMineObj( pClickBaseObj );
						int iID = Global::Schedular()->Push( ResetObjProc , (int)( pClickObjDB->GenerateRate * 1000 ), (VOID*)pClickBaseObj->GUID(), NULL );
						Print( LV1, "GATHER", "Reset mine ID = [%d]", iID  );


						/*

						
						// 設定物件消失, 再次給予基本產量. 重生位置是由 旗子產生			
							DelFromPartition( pClickBaseObj->GUID() );

						// 檢查是否要重生為其它礦種
							
							// 檢查機率總合
							//------------------------------------------------------------------------------------
								bool	bChangeObj = false;
								int		iTotalGenerateObjRate = 0;
								for( int i = 0; i < 5; i++ )
								{
									iTotalGenerateObjRate += pClickObjDB->Mine.iGenerateObjRate[i];
								}

								if( iTotalGenerateObjRate < 10000 )
								{
									int iVal =  Global::Rand( 1, 10000 );
									
									if( iVal <= iTotalGenerateObjRate )
									{
										bChangeObj = true;
									}									
								}
								else
								{
									bChangeObj = true;
								}

							// 如果改變物件, 決定物件類型
							//------------------------------------------------------------------------------------
								if( bChangeObj != false )
								{
									int		iVal	= Global::Rand( 1, iTotalGenerateObjRate );
									int		iIndex	= 0;
									int		iCount	= 0;

									for( iIndex = 0; iIndex < 5; iIndex++ )
									{
										iCount += pClickObjDB->Mine.iGenerateObjRate[iIndex];
										if( iCount >= iVal )
										{
											break;
										}
									}

									// 改變礦的種類
									GameObjDbStructEx*  pChangeObj = Global::GetObjDB(  pClickObjDB->Mine.iGenerateObj[ iIndex ] );

									if( pChangeObj != NULL )
									{
										pClickObj->BaseData.ItemInfo.OrgObjID	= pClickObjDB->Mine.iGenerateObj[ iIndex ];
										iBaseCount								= pChangeObj->Mine.BaseCount;
										iReduceVal								= pChangeObj->Mine.iReduceVal;
									}

									
								}
													

						// 決定減少初始量及設定初始量
						//-----------------------------------------------------------------------
							CNetSrv_Gather_Child::SetBaseCount( pClickObj, iBaseCount, iReduceVal );

						// 重設物件
						//-----------------------------------------------------------------------
							pClickBaseObj->Path()->ClearPath();
							pClickObj->InitNPCData();

						// 從旗子取出座標及面向
						//if( pClickObjDB->Mine.iFlagID != 0 )

						// 旗子重生規則
						//-----------------------------------------------------------------------
							int iFlagID = pClickBaseObj->Role()->SelfData.PID;
						
							//int iFlagID = 580010;

							if( iFlagID != 0 )
							{
								vector< BaseItemObject* >	vecObj;
								vector< int >				vecCount;
								vector< int >				vecEmpty;

								int iFlagCount = FlagPosClass::Count( iFlagID );
								if( iFlagCount != 0 )
								{
									vecCount.resize( iFlagCount );

									// 找出所有有相同旗標的礦 ( PID ), 並檢查其放置的旗標位置
										for( int i = 0 ; i < BaseItemObject::GetmaxZoneID( ) ; i++ )
										{
											BaseItemObject*	pObj = BaseItemObject::GetObj_ByLocalID( i );
											if( pObj == NULL )
												continue;
											else
											{
												RoleDataEx		*pMine		= pObj->Role();
												

												if( pMine->IsMine() && pMine->SelfData.PID == iFlagID )
												{
													//vecObj.push_back( pObj );
													for( int j=0; j < iFlagCount; j++ )
													{
														FlagPosInfo*	pFlagObj	= FlagPosClass::GetFlag( iFlagID , j );
														if( pMine->BaseData.Pos.X == pFlagObj->X && pMine->BaseData.Pos.Y == pFlagObj->Y && pMine->BaseData.Pos.Z == pFlagObj->Z )
														{
															vecCount[j] = vecCount[j] +1;
														}
													}											
												}
											}
										}

									// 將沒有礦的旗標整理出來
										int iIndex = 0;
										for( vector< int >::iterator it = vecCount.begin(); it != vecCount.end(); it++ )
										{
											if( *it == 0 )
											{
												vecEmpty.push_back( iIndex );
											}

											iIndex++;
										}

									// 亂數取出一個空的位置, 並將礦的位置設定進去
										int iCountEmpty = vecEmpty.size();

										if( iCountEmpty != 0 )
										{
											int				iRandPos	= Global::Rand( 0, ( iCountEmpty - 1 ) );
											FlagPosInfo*	pFlagObj	= FlagPosClass::GetFlag( iFlagID , vecEmpty[iRandPos] );

											pClickObj->BaseData.Pos.X	= pFlagObj->X;
											pClickObj->BaseData.Pos.Y	= pFlagObj->Y;
											pClickObj->BaseData.Pos.Z	= pFlagObj->Z;
											pClickObj->BaseData.Pos.Dir = pFlagObj->Dir;

											pClickObj->InitCal();
										}							
								}
							}

						// 推入時間排程讓物件重生
						//------------------------------------------------------------------
							Global::Schedular()->Push( ResetObjProc , (int)( pClickObjDB->GenerateRate * 1000 ), (VOID*)pClickBaseObj->GUID(), NULL );
						*/
					}
				}
				else
				{
					if( pClickObjDB->Mine.CreateType == EM_GameObjectCreateType_Always )
					{
						if( pClickObjDB->MaxWorldCount != 0 ) // 若為 0 的話, 則表示沒有上限, 那就不扣數量
						{
							pClickObj->TempData.iGatherNumber--;	// 減少一個礦產量
						}
					}
				}
		}
}
//--------------------------------------------------------------------------------------------------------------------			
void CNetSrv_Gather_Child::SetBaseCount ( RoleDataEx*	pMine, int iBaseCount, int iReduceVal )
{
	// 決定減少初始量及設定初始量
	//-----------------------------------------------------------------------
	if( iReduceVal != 0 )
	{
		int iVal = Global::Rand( 0, iReduceVal );
		int iCal = iBaseCount - iVal;

		if( iCal <= 0 )
		{
			iCal = 1;
		}

		pMine->TempData.iGatherNumber = iCal;
	}
	else
	{
		pMine->TempData.iGatherNumber = iBaseCount;
	}

}
//--------------------------------------------------------------------------------------------------------------------			
void CNetSrv_Gather_Child::ResetMineObj( BaseItemObject* pMineBaseObj )
{
	if( pMineBaseObj == NULL )
		return;

	RoleDataEx*			pMineObj	= pMineBaseObj->Role();
	GameObjDbStructEx*	pMineObjDB	= Global::GetObjDB( pMineObj->BaseData.ItemInfo.OrgObjID );

	if( pMineObjDB == NULL || pMineObjDB->IsMine() == false )
		return;

	int iReduceVal	= (int)pMineObjDB->Mine.iReduceVal;
	int iBaseCount	= (int)pMineObjDB->Mine.BaseCount;

	// 設定物件消失, 再次給予基本產量. 重生位置是由 旗子產生			
	//DelFromPartition( pMineObj->GUID() );

	// 檢查是否要重生為其它礦種

	// 檢查機率總合
	//------------------------------------------------------------------------------------
		bool	bChangeObj = false;
		int		iTotalGenerateObjRate = 0;
		for( int i = 0; i < 5; i++ )
		{
			iTotalGenerateObjRate += pMineObjDB->Mine.iGenerateObjRate[i];
		}

		if( iTotalGenerateObjRate < 10000 )
		{
			int iVal =  Global::Rand( 1, 10000 );

			if( iVal <= iTotalGenerateObjRate )
			{
				bChangeObj = true;
			}									
		}
		else
		{
			bChangeObj = true;
		}

	// 如果改變物件, 決定物件類型
	//------------------------------------------------------------------------------------
		if( bChangeObj != false )
		{
			int		iVal	= Global::Rand( 1, iTotalGenerateObjRate );
			int		iIndex	= 0;
			int		iCount	= 0;

			for( iIndex = 0; iIndex < 5; iIndex++ )
			{
				iCount += pMineObjDB->Mine.iGenerateObjRate[iIndex];
				if( iCount >= iVal )
				{
					break;
				}
			}

			// 改變礦的種類
			GameObjDbStructEx*  pChangeObj = Global::GetObjDB(  pMineObjDB->Mine.iGenerateObj[ iIndex ] );

			if( pChangeObj != NULL )
			{
				pMineObj->BaseData.ItemInfo.OrgObjID	= pMineObjDB->Mine.iGenerateObj[ iIndex ];

				iBaseCount								= (int)pChangeObj->Mine.BaseCount;
				iReduceVal								= (int)pChangeObj->Mine.iReduceVal;
			}


		}


	// 決定減少初始量及設定初始量
	//-----------------------------------------------------------------------
	CNetSrv_Gather_Child::SetBaseCount( pMineObj, iBaseCount, iReduceVal );

	// 重設物件
	//-----------------------------------------------------------------------
	pMineBaseObj->Path()->ClearPath();
	pMineObj->InitNPCData();

	// 從旗子取出座標及面向
	//if( pClickObjDB->Mine.iFlagID != 0 )

	// 旗子重生規則
	//-----------------------------------------------------------------------
	int iFlagID = pMineBaseObj->Role()->SelfData.PID;

	//int iFlagID = 580010;

	if( iFlagID != 0 )
	{
		vector< BaseItemObject* >	vecObj;
		vector< int >				vecCount;
		vector< int >				vecEmpty;

		int iFlagCount = FlagPosClass::Count( iFlagID );
		if( iFlagCount != 0 )
		{
			vecCount.resize( iFlagCount );

			// 找出所有有相同旗標的礦 ( PID ), 並檢查其放置的旗標位置
			for( int i = 0 ; i < BaseItemObject::GetmaxZoneID( ) ; i++ )
			{
				BaseItemObject*	pObj = BaseItemObject::GetObj_ByLocalID( i );
				if( pObj == NULL )
					continue;
				else
				{
					RoleDataEx		*pMine		= pObj->Role();


					if( pMine->IsMine() && pMine->SelfData.PID == iFlagID )
					{
						//vecObj.push_back( pObj );
						for( int j=0; j < iFlagCount; j++ )
						{
							FlagPosInfo*	pFlagObj	= FlagPosClass::GetFlag( iFlagID , j );
							//if( pMine->BaseData.Pos.X == pFlagObj->X && pMine->BaseData.Pos.Y == pFlagObj->Y && pMine->BaseData.Pos.Z == pFlagObj->Z )
							if( pMine->BaseData.Pos.X == pFlagObj->X && pMine->BaseData.Pos.Y == pFlagObj->Y && pMine->BaseData.Pos.Z == pFlagObj->Z )
							{
								vecCount[j] = vecCount[j] +1;
							}
						}											
					}
				}
			}

			// 將沒有礦的旗標整理出來
			int iIndex = 0;
			for( vector< int >::iterator it = vecCount.begin(); it != vecCount.end(); it++ )
			{
				FlagPosInfo*	pFlagObj	= FlagPosClass::GetFlag( iFlagID , iIndex );

				if( *it == 0 && pFlagObj->IsEmpty() == false )
				{
					vecEmpty.push_back( iIndex );
				}

				iIndex++;
			}

			// 亂數取出一個空的位置, 並將礦的位置設定進去
			int iCountEmpty = (int)vecEmpty.size();

			if( iCountEmpty != 0 )
			{
				// 找一個有效的 Flag Position, 若是取出的為空旗標, 則從頭找到一個有效的旗標種入
				int				iRandPos	= Global::Rand( 0, ( iCountEmpty - 1 ) );
				int				iFlag		= vecEmpty[iRandPos];
				FlagPosInfo*	pFlagObj	= FlagPosClass::GetFlag( iFlagID , iFlag );

				if( pFlagObj->IsEmpty() == false )
				{
					pMineObj->BaseData.Pos.X	= pFlagObj->X;
					pMineObj->BaseData.Pos.Y	= pFlagObj->Y;
					pMineObj->BaseData.Pos.Z	= pFlagObj->Z;
					pMineObj->BaseData.Pos.Dir	= pFlagObj->Dir;

					Print( LV2, "Gather", "ResetMine[ %d ], PID[ %d ], Flag[ %d ], ( %f, %f, %f )", pMineObj->GUID(), iFlagID, iFlag, pFlagObj->X , pFlagObj->Y , pFlagObj->Z );

					pMineObj->InitCal();
				}
				else
				{
					// 從頭尋找一個有系的位置, 要是找不到則印出錯誤訊息給 LOG. 
					vector < FlagPosInfo >* pFlagList = FlagPosClass::GetFlagList( iFlagID );
					
					for( vector < FlagPosInfo >::iterator it = pFlagList->begin(); it != pFlagList->end(); it++ )
					{
						FlagPosInfo Flag = *it;

						if( Flag.IsEmpty() == false )
						{
							pMineObj->BaseData.Pos.X	= Flag.X;
							pMineObj->BaseData.Pos.Y	= Flag.Y;
							pMineObj->BaseData.Pos.Z	= Flag.Z;
							pMineObj->BaseData.Pos.Dir	= Flag.Dir;

							pMineObj->InitCal();							
							break;
						}
					}

					Print( LV5, "GATHER", "Generating mine failed, whole flaglist is empty"	);
				}
			}							
		}
	}

	// 推入時間排程讓物件重生
	//------------------------------------------------------------------
	//Global::Schedular()->Push( ResetObjProc , (int)( pMineObjDB->GenerateRate * 1000 ), (VOID*)pMineBaseObj->GUID(), NULL );
}
//------------------------------------------------------------------
void CNetSrv_Gather_Child::GenerateMine( int iRoomID, int iFlagID, int iMineID,	int iMineCount )
{
	vector< BaseItemObject* >	vecObj;
	vector< int >				vecCount;
	list< int >					listEmpty;

	int iFlagCount = FlagPosClass::Count( iFlagID );

	if( iFlagCount == 0 )
		return;

	vecCount.resize( iFlagCount );

	// 找出所有有相同旗標的礦 ( PID ), 並檢查其放置的旗標位置
		for( int i = 0 ; i < BaseItemObject::GetmaxZoneID( ) ; i++ )
		{
			BaseItemObject*	pObj = BaseItemObject::GetObj_ByLocalID( i );
			if( pObj == NULL )
				continue;
			else
			{
				RoleDataEx		*pMine		= pObj->Role();

				if( pMine->IsMine() && pMine->SelfData.PID == iFlagID )
				{
					for( int j=0; j < iFlagCount; j++ )
					{
						FlagPosInfo*	pFlagObj	= FlagPosClass::GetFlag( iFlagID , j );
						if( pMine->BaseData.Pos.X == pFlagObj->X && pMine->BaseData.Pos.Y == pFlagObj->Y && pMine->BaseData.Pos.Z == pFlagObj->Z )
						{
							vecCount[j] = vecCount[j] +1;
						}
					}											
				}
			}
		}

	// 將沒有礦的旗標整理出來
		int iIndex = 0;
		for( vector< int >::iterator it = vecCount.begin(); it != vecCount.end(); it++ )
		{
			FlagPosInfo*	pFlagObj	= FlagPosClass::GetFlag( iFlagID , iIndex );

			if( *it == 0 && pFlagObj->IsEmpty() == false )
			{
				listEmpty.push_back( iIndex );
			}

			iIndex++;
		}


	// 亂數取出一個空的位置, 並將礦的位置設定進去, 並將位置清空
		for( int i = 0; i < iMineCount; i++ )
		{
			int iCountEmpty = (int)listEmpty.size();
			if( iCountEmpty != 0 )
			{
				int				iRandPos	= Global::Rand( 0, ( iCountEmpty - 1 ) );

				list< int >::iterator it;
				int iIndex = 0;
				for( it = listEmpty.begin(); it != listEmpty.end(); it++ )
				{
					if( iIndex == iRandPos )
						break;

					iIndex++;
				}

				if( it != listEmpty.end() )
				{
					int				iID			= *it;
					FlagPosInfo*	pFlagObj	= FlagPosClass::GetFlag( iFlagID , iID );

					if( pFlagObj != NULL )
					{
						listEmpty.erase( it );
							 
						/*
						pMineObj->BaseData.Pos.X	= pFlagObj->X;
						pMineObj->BaseData.Pos.Y	= pFlagObj->Y;
						pMineObj->BaseData.Pos.Z	= pFlagObj->Z;
						pMineObj->BaseData.Pos.Dir = pFlagObj->Dir;

						pMineObj->InitCal();
						*/

						int				iObjGUID = Global::CreateObj(  iMineID , pFlagObj->X , pFlagObj->Y , pFlagObj->Z , pFlagObj->Dir , 1 );
						BaseItemObject* pBaseObj = BaseItemObject::GetObj( iObjGUID );

						Print( LV2, "Gather", "GenerateMine[ %d ], PID[ %d ] ( %f, %f, %f )", iObjGUID, iFlagID, pFlagObj->X , pFlagObj->Y , pFlagObj->Z );

						if( pBaseObj )
						{
							pBaseObj->Role()->SelfData.PID = iFlagID;
							Global::AddToPartition( iObjGUID , iRoomID );
						}

						((CNetSrv_Gather_Child*)m_pThis)->m_mapMineObj[ iFlagID ].push_back( iObjGUID );
					}
				}

			}
		}




}

int CNetSrv_Gather_Child::OnEvent_MineCheck	 ( SchedularInfo* Obj, void* InputClass )
{
	CNetSrv_Gather_Child* pThis = (CNetSrv_Gather_Child*)m_pThis;
	
	// 	
	for( map< int, vector< int > >::iterator itMap = pThis->m_mapMineObj.begin(); itMap != pThis->m_mapMineObj.end(); itMap++ )
	{
		for( vector< int >::iterator it = itMap->second.begin(); it != itMap->second.end(); it++ )
		{
			int iObjGUID			= *it;
			BaseItemObject* pObj	= BaseItemObject::GetObj( iObjGUID );

			if( pObj == NULL )
				return 0;

			RoleDataEx* pRoleObj	= pObj->Role();

			if( pRoleObj->BaseData.Pos.X == 0 && pRoleObj->BaseData.Pos.Y == 0 && pRoleObj->BaseData.Pos.Z == 0 )
			{
				Print( LV5, "Gather", "Mine[ %d ], PID[ %d ] position zero", iObjGUID, pRoleObj->SelfData.PID );

				// 重新找一個空的位置
				CNetSrv_Gather_Child::ResetMineObj( pObj );
			}
		}
	}

	return 0;
}
//------------------------------------------------------------------------------------
void CNetSrv_Gather_Child::OnCancelGather( BaseItemObject*	pObj, int iClickObjID )
{
	RoleDataEx			*pRole			= pObj->Role();
	CastingData*		pCastingData	= &pRole->PlayerTempData->CastData;

	if( pCastingData->iClickObjID == iClickObjID && pCastingData->bBeHit == false )
	{
		pCastingData->bBeHit = true;
	}
}
