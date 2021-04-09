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

	// �s�J�����
	pCastingData->iClickObjID		= iClickObjID;
	pCastingData->SourcePos			= pRole->BaseData.Pos;
	pCastingData->szLuaEvent		= pszLuaEvent;

	// �I������N�� -1 ��ܤ��ι��骫��, ��軡, �]�]�����H
	if( iClickObjID != 0 )
	{
		// Cancle buff

		//�M��Buff
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
			// ���Q�I������
			//CastingData*		pClickObjData	= &pClickObj->Role()->PlayerTempData->CastData;

			// �ˬd���󦳵L�Q��w
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
				
				// �p�G�Q�I������s�b, �B�Ӫ�����w�̦s�b, �B�Z���M�Ӫ���b 50 ����, �N�q���I���̸Ӫ��󦣿�
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
		
	// ���J�ƥ�, �ɶ���ᤩ�Ķ����~

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
	// ���J�ƥ�
	RoleDataEx			*pRole			= pObj->Role();
	int					iGUID			= pRole->GUID();

	BaseItemObject*		pClickObj		= Global::GetObj( iClickObjID );

	if( pClickObj == NULL || pClickObj->Role()->TempData.Sys.SecRoomID == -1 )	// == -1 ��ܤ��b���ΰϤ�
	{
		CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_NULLOBJ );
		return;
	}

	// ����P�@�w�I�����H���q, �n�O�H�������ʶW�i 100, �s����q 20 ��, �h���q����
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

	// �Z���ˬd
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
		NetSrv_Talk::SysMsg( pRole->GUID(),"�Ӫ���w�Q��w�Τw�Q�R��" );
		return;
	}
	*/

	// �ˬd�ޯ൥��
	int iSkillLV = 0;
	switch( pClickObjDB->Mine.Type )
	{
	case EM_GameObjectMineType_Mine: iSkillLV = (int)pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Mining ); break;
	case EM_GameObjectMineType_Wood: iSkillLV = (int)pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Lumbering ); break;
	case EM_GameObjectMineType_Herb: iSkillLV = (int)pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Herblism ); break;
	case EM_GameObjectMineType_Fish: iSkillLV = (int)pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Fishing ); break;
	}

	// �ˬd�ޯ������
	if( iSkillLV < pClickObjDB->Mine.LvLimit )
	{
		CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_LOWSKILL );
		return; // �ޯण��
	}

	float	fGatherTime = (float)_TIME_CLICKTOGETITEM;

	if( pRole->TempData.Attr.Mid.Body[ EM_WearEqType_GatherSpeedRate ] != 0 )
	{
		float fRate = pRole->TempData.Attr.Mid.Body[ EM_WearEqType_GatherSpeedRate ] / (float)100;
		fGatherTime = fGatherTime / ( 1 + fRate );
	}

	// ���J�ƥ�, �ɶ���ᤩ�Ķ����~
	//pCastingData->iEventHandle		= Global::Schedular()->Push( OnEvent_CastingFinished , _TIME_CHECKCASTEVENT_, (VOID*)iGUID, NULL );	
	if( CNetSrv_Gather_Child::SetCast( pRole, iClickObjID, fGatherTime,  OnCast_GatherObj, "" ) != false )
	{
		// ���I���̼�����w�Ķ��ʧ@

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

	// �ˬd�H���٦b���b
	//--------------------------------------------------------------------------------------------------------------------
	if( pObj == NULL )	
	{
		return 0;	// �H���w�g�U�u�F, �άO�_�u, �ϥ��N�O�H�����󤣦s�b, ���}���B�z, �H�]���b�F
	}

	RoleDataEx			*pRole			= pObj->Role();						// �I�����󪺤H��
	CastingData*		pCastingData	= &pRole->PlayerTempData->CastData;		// �I�����

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

		pClickBaseObj	= Global::GetObj( iClickObjID );	// ���X�Q�I������
		if( pClickBaseObj == NULL || pClickBaseObj->Role()->TempData.Sys.SecRoomID == -1 )	
		{
			bFailed		= true;
			emResult	= EM_GATHER_NULLOBJ;
			//CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_NULLOBJ );	// ���󤣦s�b, ���i������F�εo�ͩ_�Ǫ��{��, �o�䦳�o�͹L. �n�d
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




	// �ˬd�Q�I�������٦b���b
	//--------------------------------------------------------------------------------------------------------------------
	//if( pCastingData->iClickObjID == 0 || ( ( pClickBaseObj = BaseItemObject::GetObj( iClickObjID ) ) == NULL ) )
	//{
		// TODO: ���Ӥ��|�o�ͤ~��, �I����ƳQ�M��. �Y�u���o�ͪ���. Client ���s������, �h�����n�o�X�Ѱ��ʧ@���t�M�ʥ]
		// �]����ƳQ�M�ŤF, �ҥH�]���i�ા�D�O���Ӫ���Q��w
	//	pCastingData->Clear();

	//	m_pGatherCastFunc( iGUID, EM_GATHER_NULLOBJ );
	//	return 0;
	//}

	// �ˬd�H���欰�O�_�ݭn���_, �H���ˬd�ɶ��O�_��F
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
	
	// �ˬd�H�����L����
	//if( !bFailed && !(  FloatEQU( SourcePos.X , Pos.X ) && FloatEQU( SourcePos.Z ,  Pos.Z ) && FloatEQU( SourcePos.Y , Pos.Y ) ))
	float fCheckDis = 1.5;
	if( !bFailed && fDis > fCheckDis )
	{
		bFailed	= true;	// �H�����ʤF
		emResult	= EM_GATHER_FAILED;
	}

	// �ˬd�H���O���O�I�k�L�{��, �Q����
	if( bFailed == false && pCastingData->bBeHit != false )
	{
		pCastingData->bBeHit	= false;
		bFailed					= true;
		emResult				= EM_GATHER_FAILED;
	}

	// �ˬd�H���O���O���`
	if( bFailed == false && pRole->IsDead() == true )
	{
		bFailed					= true;
		emResult				= EM_GATHER_FAILED;
	}

	
	//if( pClickObj != NULL && pClickObj->PlayerTempData->CastData.iLockGUID != iGUID )
	if( pClickObj != NULL && pClickObj->TempData.iLockGUID != iGUID )
	{
		bFailed	= true;	// �H�����ʤF
		emResult	= EM_GATHER_FAILED;
	}
	


	// �ˬd�H���O���O�I�k

	// �ˬd�H���O���O�ϥΪ��~

	// �ˬd�H���O���O���ܰʧ@

	if( bFailed == false )
	{
		// �ˬd���S���W�L�ɶ�, �Y�ɶ��٨S��, �h�A�~���
		unsigned int dwSystem	= GetTickCount();
		unsigned int dwEndTime	= pCastingData->iEndTime;

		if( dwEndTime > dwSystem )
		{
			pCastingData->iEventHandle		= Global::Schedular()->Push( OnEvent_GatherCast , _TIME_CHECKCASTEVENT_, (VOID*)iGUID, NULL );
			return 0;
		}
	}	

	// ���m�I�i���
	//--------------------------------------------------------------------------------------------------------------------
	//pCastingData->Clear();	


	// ���X������, �øѰ���w
	//--------------------------------------------------------------------------------------------------------------------	
	if( pClickObj != NULL )
	{				
		//pClickObj->PlayerTempData->CastData.iLockGUID = 0;	// ���}��w
		//pClickObj->PlayerTempData->CastData.iLockTime	= GetTickCount() + 1000;
		pClickObj->TempData.iLockTime	= GetTickCount() + 1000;
	}


	// �ˬd�O���O���ѤF, �Y�O�h�o�e���ѵ� Client, �åB�Ѱ� Cast ���, �H�θ��ꪫ��
	//--------------------------------------------------------------------------------------------------------------------
	if( bFailed )
	{
		if( pCastingData->pCastFunc )
		{
		//CNetSrv_Gather::ClickGatherObjResult( iGUID, emResult );	// �o�e�I�����ѵ� Client
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
	// ���񰱤�ʧ@
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


		// ���񰱤�ʧ@
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

		// ���m�I�k�ɶ�
		//--------------------------------------------------------------------------------------------------------------------
			pCastingData->Clear();

		if( emResult != EM_GATHER_ENDCAST )
		{			
			// �o�X���ѰT��
				CNetSrv_Gather::ClickGatherObjResult( iGUID, emResult );
				CNetSrv_Gather_Child::PlayEndMotion( iGUID, pClickObjDB );
			return;
		}




	
	//--------------------------------------------------------------------------------------------------------------------
		if( emResult == EM_GATHER_ENDCAST )
		{
			// ���q�欰
			//--------------------------------------------------------------------------------------------------------------------
			bool	bResult = true;
			int		iItem, iItemVal;

			
			//-----------------------------------------------------------------------------------------------------------------------------------------

				float	fSuccessRate		= 0;				

				float	fBounsRate			= 0;
				int		iRate				= 0;	// �`�M�Ķ����v 10000 �� 100%								
				float	fBounsGatherRate	= 0;	
				float	fSkillPoint			= 0.0;	// �Ķ��ޯ��I��
				float	fGatherObjLV		= 0.0;	// �Q�Ķ����󵥯�

				float	fGlobalGatherVar	= 0.7f;

				int		iSuccessRate		= 0;

				// �p���l�Ķ�, �N�Ҧ����v�[�`
					for( int i = 0; i < 5; i++ )
					{
						if( pClickObjDB->Mine.GetItem[ i ] != 0 )
						{
							iRate += pClickObjDB->Mine.GetItemRate[ i ];
						}				
					}

				// �p��ޯ�[������~�ޯ��I��
					//int		iOffsetLV	= 0;
					//int		iSkillLV	= 0;
					
					switch( pClickObjDB->Mine.Type )
					{
					case EM_GameObjectMineType_Mine: fSkillPoint = pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Mining );	break;
					case EM_GameObjectMineType_Wood: fSkillPoint = pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Lumbering ); break;
					case EM_GameObjectMineType_Herb: fSkillPoint = pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Herblism );	break;
					case EM_GameObjectMineType_Fish: fSkillPoint = pRole->TempData.Attr.Fin.SkillValue.GetSkill( EM_SkillValueType_Fishing );	break;
					}

				
					fBounsGatherRate = ((float)( pRole->TempData.Attr.Mid.Body[EM_WearEqType_SkillValue_GatherRate] ) / 100 );		// �H 100 ����|
				
				// �Q�Ķ����󵥯�
					fGatherObjLV = (float)pClickObjDB->Mine.LvLimit;

				// �Ķ������ܼ�
					fGlobalGatherVar = 0.7f; // ��, ����� Table �[�J

				
					iSuccessRate	= (int)( (float) iRate * ( fSkillPoint + 1 - fGatherObjLV ) * 1.02 * fGlobalGatherVar * ( 1 + fBounsGatherRate ) );

					if( iSuccessRate < 10000 )
					{
						int iVal =  Global::Rand( 1, 10000 );				
						if( iVal > iSuccessRate ) 
						{
							// �e�X�Ķ����ѰT���� Client
							CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_FAILED );
							CNetSrv_Gather_Child::PlayEndMotion( iGUID, pClickObjDB );
							return;
						}
					}

				// �p��Ķ��u��[�� 11/09/07 �M�w���n���Ķ��u��o�F��F
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
				

				// �@�몫�~
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

				// �S��Ķ�
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

				// �p��o��Ķ�����
				//-----------------------------------------------------------------------------------
					int iSkillID			= 0;
					int	iGlobalGatherExpVar = 40;			// �ȥN

					// ���o���x�g���
						iGlobalGatherExpVar	= 40;	// ��

					switch( pClickObjDB->Mine.Type )
					{
					case EM_GameObjectMineType_Mine: iSkillID = EM_SkillValueType_Mining;break;
					case EM_GameObjectMineType_Wood: iSkillID = EM_SkillValueType_Lumbering; break;
					case EM_GameObjectMineType_Herb: iSkillID = EM_SkillValueType_Herblism; break;
					case EM_GameObjectMineType_Fish: iSkillID = EM_SkillValueType_Fishing; break;
					}

					// 09/07 �ק�g�礽��
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
				CNetSrv_Gather::ClickGatherObjResult( iGUID, EM_GATHER_GONE );	// �ƶq����
				CNetSrv_Gather_Child::PlayEndMotion( iGUID, pClickObjDB );
				return;
			}

			// ���m����
			//--------------------------------------------------------------------------------------------------------------------			
				if( pClickObjDB->Mine.CreateType == EM_GameObjectCreateType_EmptyDel )
				{
					int iReduceVal	= (int)pClickObjDB->Mine.iReduceVal;
					int iBaseCount	= (int)pClickObjDB->Mine.BaseCount;

					pClickObj->TempData.iGatherNumber--;	// ��֤@���q���q
					if( pClickObj->TempData.iGatherNumber == 0 )
					{
						DelFromPartition( pClickBaseObj->GUID() );
						ResetMineObj( pClickBaseObj );
						int iID = Global::Schedular()->Push( ResetObjProc , (int)( pClickObjDB->GenerateRate * 1000 ), (VOID*)pClickBaseObj->GUID(), NULL );
						Print( LV1, "GATHER", "Reset mine ID = [%d]", iID  );


						/*

						
						// �]�w�������, �A�������򥻲��q. ���ͦ�m�O�� �X�l����			
							DelFromPartition( pClickBaseObj->GUID() );

						// �ˬd�O�_�n���ͬ��䥦�q��
							
							// �ˬd���v�`�X
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

							// �p�G���ܪ���, �M�w��������
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

									// �����q������
									GameObjDbStructEx*  pChangeObj = Global::GetObjDB(  pClickObjDB->Mine.iGenerateObj[ iIndex ] );

									if( pChangeObj != NULL )
									{
										pClickObj->BaseData.ItemInfo.OrgObjID	= pClickObjDB->Mine.iGenerateObj[ iIndex ];
										iBaseCount								= pChangeObj->Mine.BaseCount;
										iReduceVal								= pChangeObj->Mine.iReduceVal;
									}

									
								}
													

						// �M�w��֪�l�q�γ]�w��l�q
						//-----------------------------------------------------------------------
							CNetSrv_Gather_Child::SetBaseCount( pClickObj, iBaseCount, iReduceVal );

						// ���]����
						//-----------------------------------------------------------------------
							pClickBaseObj->Path()->ClearPath();
							pClickObj->InitNPCData();

						// �q�X�l���X�y�Фέ��V
						//if( pClickObjDB->Mine.iFlagID != 0 )

						// �X�l���ͳW�h
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

									// ��X�Ҧ����ۦP�X�Ъ��q ( PID ), ���ˬd���m���X�Ц�m
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

									// �N�S���q���X�о�z�X��
										int iIndex = 0;
										for( vector< int >::iterator it = vecCount.begin(); it != vecCount.end(); it++ )
										{
											if( *it == 0 )
											{
												vecEmpty.push_back( iIndex );
											}

											iIndex++;
										}

									// �üƨ��X�@�ӪŪ���m, �ñN�q����m�]�w�i�h
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

						// ���J�ɶ��Ƶ{�����󭫥�
						//------------------------------------------------------------------
							Global::Schedular()->Push( ResetObjProc , (int)( pClickObjDB->GenerateRate * 1000 ), (VOID*)pClickBaseObj->GUID(), NULL );
						*/
					}
				}
				else
				{
					if( pClickObjDB->Mine.CreateType == EM_GameObjectCreateType_Always )
					{
						if( pClickObjDB->MaxWorldCount != 0 ) // �Y�� 0 ����, �h��ܨS���W��, ���N�����ƶq
						{
							pClickObj->TempData.iGatherNumber--;	// ��֤@���q���q
						}
					}
				}
		}
}
//--------------------------------------------------------------------------------------------------------------------			
void CNetSrv_Gather_Child::SetBaseCount ( RoleDataEx*	pMine, int iBaseCount, int iReduceVal )
{
	// �M�w��֪�l�q�γ]�w��l�q
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

	// �]�w�������, �A�������򥻲��q. ���ͦ�m�O�� �X�l����			
	//DelFromPartition( pMineObj->GUID() );

	// �ˬd�O�_�n���ͬ��䥦�q��

	// �ˬd���v�`�X
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

	// �p�G���ܪ���, �M�w��������
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

			// �����q������
			GameObjDbStructEx*  pChangeObj = Global::GetObjDB(  pMineObjDB->Mine.iGenerateObj[ iIndex ] );

			if( pChangeObj != NULL )
			{
				pMineObj->BaseData.ItemInfo.OrgObjID	= pMineObjDB->Mine.iGenerateObj[ iIndex ];

				iBaseCount								= (int)pChangeObj->Mine.BaseCount;
				iReduceVal								= (int)pChangeObj->Mine.iReduceVal;
			}


		}


	// �M�w��֪�l�q�γ]�w��l�q
	//-----------------------------------------------------------------------
	CNetSrv_Gather_Child::SetBaseCount( pMineObj, iBaseCount, iReduceVal );

	// ���]����
	//-----------------------------------------------------------------------
	pMineBaseObj->Path()->ClearPath();
	pMineObj->InitNPCData();

	// �q�X�l���X�y�Фέ��V
	//if( pClickObjDB->Mine.iFlagID != 0 )

	// �X�l���ͳW�h
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

			// ��X�Ҧ����ۦP�X�Ъ��q ( PID ), ���ˬd���m���X�Ц�m
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

			// �N�S���q���X�о�z�X��
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

			// �üƨ��X�@�ӪŪ���m, �ñN�q����m�]�w�i�h
			int iCountEmpty = (int)vecEmpty.size();

			if( iCountEmpty != 0 )
			{
				// ��@�Ӧ��Ī� Flag Position, �Y�O���X�����źX��, �h�q�Y���@�Ӧ��Ī��X�кؤJ
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
					// �q�Y�M��@�Ӧ��t����m, �n�O�䤣��h�L�X���~�T���� LOG. 
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

	// ���J�ɶ��Ƶ{�����󭫥�
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

	// ��X�Ҧ����ۦP�X�Ъ��q ( PID ), ���ˬd���m���X�Ц�m
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

	// �N�S���q���X�о�z�X��
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


	// �üƨ��X�@�ӪŪ���m, �ñN�q����m�]�w�i�h, �ñN��m�M��
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

				// ���s��@�ӪŪ���m
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
