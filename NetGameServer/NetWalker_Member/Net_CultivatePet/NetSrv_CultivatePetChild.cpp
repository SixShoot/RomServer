#include "../NetWakerGSrvInc.h"
#include "NetSrv_CultivatePetChild.h"
#include "../../mainproc/magicproc/MagicProcess.h"
#include "../../mainproc/PartyInfoList/PartyInfoList.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_CultivatePetChild::Init()
{
    NetSrv_CultivatePet::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_CultivatePetChild );	

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_CultivatePetChild::Release()
{
    if( This == NULL )
        return false;

	NetSrv_CultivatePet::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
void NetSrv_CultivatePetChild::RC_PushPet( BaseItemObject* Obj , int PetPos , int ItemPos )
{
	RoleDataEx* Owner = Obj->Role();
	ItemFieldStruct* PetItem = Owner->GetBodyItem( ItemPos );
	if( PetItem == NULL || (unsigned)PetPos >= MAX_CultivatePet_Count )
	{
		SC_PushPetResultFailed( Owner->GUID() );
		return;
	}

	if( Owner->CheckPetPosTimeFlag( PetPos ) == false )
	{
		SC_PushPetResultFailed( Owner->GUID() );
		return;
	}

	CultivatePetStruct&	PetBase	= Owner->PlayerBaseData->Pet.Base[ PetPos ];

	if( PetBase.PetOrgID != 0 )
	{
		SC_PushPetResultFailed( Owner->GUID() );
		return;
	}

	GameObjDbStructEx* ItemObjDB = Global::GetObjDB( PetItem->OrgObjID );
	if( ItemObjDB == NULL || ItemObjDB->Item.ItemType != EM_ItemType_Pet )
	{
		SC_PushPetResultFailed( Owner->GUID() );
		return;
	}
	GameObjDbStructEx* PetObjDB = Global::GetObjDB( PetItem->Pet.PetOrgID );
	if( PetObjDB->IsNPC() == false )
	{
		SC_PushPetResultFailed( Owner->GUID() );
		return;
	}
	//memset( PetBase , 0 , sizeof( PetBase ) );
	PetBase.init();

	PetBase.ItemOrgID	= PetItem->OrgObjID;
	//PetBase.Name		= PetObjDB->Name;
	PetBase.CreateTime	= PetItem->Pet.CreateTime;
	PetBase.PetOrgID	= PetItem->Pet.PetOrgID;
	PetBase.SkillPoint	= PetBase.TotalSkillPoint = PetItem->Pet.SkillPoint;
	PetBase.PetType		= PetItem->Pet.PetType;
	PetBase.Property	= (MagicResistENUM)PetItem->Pet.Property;		//屬性(地水火風光暗)
	PetBase.Lv			= PetItem->Pet.Lv;
	PetBase.Talent		= PetItem->Pet.Talent;

	PetBase.STR			= PetItem->Pet.STR;
	PetBase.STA			= PetItem->Pet.STA;
	PetBase.INT			= PetItem->Pet.INT;
	PetBase.MND			= PetItem->Pet.MND;
	PetBase.AGI			= PetItem->Pet.AGI;

	PetBase.Spirit		= ItemObjDB->Item.Pet.Spirit;

	PetBase.TrainCount	= PetItem->Pet.TrainCount;

//	PetBase.SetPetFlag( PetBase.PetType , true );

	PetItem->Init();
	Owner->Net_FixItemInfo_Body( ItemPos );

	SC_PushPetResultOK( Owner->GUID() , PetPos , PetBase );

	char LuaBuf[128];
	if( strlen( ItemObjDB->Item.Pet.LuaEvent ) != 0 )
	{
		sprintf( LuaBuf , "%s(%d,%d)" , ItemObjDB->Item.Pet.LuaEvent , PetPos , 0 );
		LUA_VMClass::PCallByStrArg( LuaBuf , Owner->GUID() , Owner->GUID() );
	}
}
void NetSrv_CultivatePetChild::RC_PopPet( BaseItemObject* Obj , int PetPos , int ItemPos )
{
	RoleDataEx* Owner = Obj->Role();

	if( Owner->CheckPetPosTimeFlag( PetPos ) == false )
	{
		SC_PopPetResult( Owner->GUID() , false , ItemPos );
		return;
	}

	if( (unsigned)PetPos >= MAX_CultivatePet_Count )
	{
		SC_PopPetResult( Owner->GUID() , false , ItemPos );
		return;
	}

	CultivatePetStruct&	PetBase	= Owner->PlayerBaseData->Pet.Base[ PetPos ];
	if( PetBase.Mode.IsSummon != false )
	{
		SC_PopPetResult( Owner->GUID() , false , ItemPos );
		return;
	}
	if( Owner->CheckItemPosTimeFlag( 0 , ItemPos ) == false )
	{
		SC_PopPetResult( Owner->GUID() , false , ItemPos );
		return;
	}
	
	ItemFieldStruct* PetItem = Owner->GetBodyItem( ItemPos );
	if( PetItem == NULL  )
	{
		if( PetBase.CheckEqEmpty() == false )
		{
			SC_PopPetResult( Owner->GUID() , false , ItemPos );
		}
		else
		{
			PetBase.init();
			SC_PopPetResult( Owner->GUID() , true , ItemPos );
		}
		return;
	}

	GameObjDbStructEx* PetObjDB = Global::GetObjDB( PetBase.ItemOrgID );
	if( PetObjDB == NULL )
		return;

	char LuaBuf[128];
	if( strlen( PetObjDB->Item.Pet.LuaEvent ) != 0 )
	{
		sprintf( LuaBuf , "%s(%d,%d)" , PetObjDB->Item.Pet.LuaEvent , PetPos , 1 );
		LUA_VMClass::PCallByStrArg( LuaBuf , Owner->GUID() , Owner->GUID() );
	}

/*	if(		PetItem->IsEmpty() 
		||	PetBase.PetOrgID == 0 )
	{
		SC_PopPetResult( Owner->GUID() , false );
		return;
	}
	*/

	PetItem->OrgObjID		= PetBase.ItemOrgID;
	PetItem->Pet.CreateTime = PetBase.CreateTime;
	PetItem->Pet.PetOrgID	= PetBase.PetOrgID;
	PetItem->Pet.SkillPoint	= PetBase.TotalSkillPoint;
	PetItem->Pet.PetType	= PetBase.PetType;
	PetItem->Pet.Property	= PetBase.Property;		//屬性(地水火風光暗)
	PetItem->Pet.Lv			= PetBase.Lv;
	PetItem->Pet.Talent		= PetBase.Talent;


	PetItem->Pet.STR		= PetBase.STR;
	PetItem->Pet.STA		= PetBase.STA;
	PetItem->Pet.INT		= PetBase.INT;
	PetItem->Pet.MND		= PetBase.MND;
	PetItem->Pet.AGI		= PetBase.AGI;
	PetItem->Pet.TrainCount	= PetBase.TrainCount;

	
/*	for( int i = 0 ; i < EM_CultivatePetLifeSkillType_Max ; i++ )
	{
		PetItem->Pet.LifeSkill[i] = unsigned char( PetBase.LifeSkill[i] );
	}*/
	Owner->Net_FixItemInfo_Body( ItemPos );
	PetBase.init();
	SC_PopPetResult( Owner->GUID() , true , ItemPos );
	
}
void NetSrv_CultivatePetChild::RC_PetName( BaseItemObject* Obj , int PetPos , const char* Name )
{
	RoleDataEx* Owner = Obj->Role();

	if( Owner->CheckPetPosTimeFlag( PetPos ) == false )
		return;

	if( (unsigned)PetPos >= MAX_CultivatePet_Count )
	{
		SC_PetNameResult( Owner->GUID() , false , Name );
		return;
	}

	CultivatePetStruct&	PetBase	= Owner->PlayerBaseData->Pet.Base[ PetPos ];
	if( PetBase.PetOrgID == 0 || PetBase.Mode.IsSummon != false )
	{
		SC_PetNameResult( Owner->GUID() , false , Name );
		return;
	}

	std::wstring outStrName;
	CharacterNameRulesENUM  Ret = g_ObjectData->CheckCharacterNameRules( Name , (CountryTypeENUM)Global::Ini()->CountryType , outStrName );
	if( Ret != EM_CharacterNameRules_Rightful )
	{
		SC_PetNameResult( Owner->GUID() , false , Name );
		return;
	}

	PetBase.Name = WCharToUtf8( outStrName.c_str() ).c_str();

	SC_PetNameResult( Owner->GUID() , true , PetBase.Name );
	
}
void NetSrv_CultivatePetChild::RC_PetEvent( BaseItemObject* Obj , int PetPos , int ItemPos , CultivatePetCommandTypeENUM Event )
{
	RoleDataEx* Owner = Obj->Role();

	if( Owner->CheckPetPosTimeFlag( PetPos ) == false )
		return;

	if( (unsigned)PetPos >= MAX_CultivatePet_Count )
	{
		SC_PetEventResult( Owner->GUID() , PetPos , Event , Event , 0 , false );
		return;
	}

	CultivatePetStruct&	PetBase	= Owner->PlayerBaseData->Pet.Base[ PetPos ];
	if( PetBase.PetOrgID == 0 )
	{
		SC_PetEventResult( Owner->GUID() , PetPos , Event , Event , 0 , false );
		return;
	}
	if(		PetBase.EventType != EM_CultivatePetCommandType_None 
		&&	Event != EM_CultivatePetCommandType_Return )
	{
		SC_PetEventResult( Owner->GUID() , PetPos , Event , Event , 0 , false );
		return;
	}

	CultivatePetCommandTypeENUM	Bk_EventType = PetBase.EventType;
		
	
	char	Buf[512];
	int		EventTime = 0;
	vector<MyVMValueStruct> RetList;
	RoleValueName_ENUM ValueType;
	switch( Event )
	{
	case EM_CultivatePetCommandType_OnTimeEvent:	//client 端每10秒通知Server 看那隻寵物(處理說話or特別事件)
		{
			sprintf_s( Buf , sizeof(Buf) , "PetCallBack_OnTime(%d,%d,%d,%d)" , PetPos , PetBase.PetOrgID , PetBase.PetType , PetBase.Property );
			LUA_VMClass::PCallByStrArg( Buf , Owner->GUID() , Owner->GUID() );
			PetBase.EventType = EM_CultivatePetCommandType_None;	
		}
		break;
	case EM_CultivatePetCommandType_Summon:			//召喚寵物
		{
			int PetID = CreatePet( Owner->GUID() , PetBase.PetOrgID , (char*)PetBase.Name.Begin() , PetBase.Lv , EM_SummonPetType_CultivatePet );
			BaseItemObject* PetObj = Global::GetObj( PetID );
			if( PetObj == NULL )
			{
				SC_PetEventResult( Owner->GUID() , PetPos , Event , Event , 0 , false );
				return;
			}
			RoleDataEx* PetRole = PetObj->Role();
			PetObj->Role()->SelfData.AutoPlot = "CultivatePet_AutoPlot";
			PetBase.EventType = EM_CultivatePetCommandType_Summon;	
			//////////////////////////////////////////////////////////////////////////
			Owner->TempData.UpdateInfo.Recalculate_All = true;
			PetObj->PlotVM()->CallLuaFunc( PetObj->Role()->SelfData.AutoPlot.Begin() , Owner->GUID() , 0  );
		}
		break;
	case EM_CultivatePetCommandType_Feed:			//餵食
		{
			ItemFieldStruct* bodyItem = Owner->GetBodyItem( ItemPos );
			if( bodyItem == NULL || bodyItem->IsEmpty() )
			{
				SC_PetEventResult( Owner->GUID() , PetPos , Event , Event , 0 , false );
				return;
			}
			
			GameObjDbStructEx* ItemDB = Global::GetObjDB( bodyItem->OrgObjID );
			if( ItemDB->IsItem() == false || ItemDB->Item.ItemType != EM_ItemType_Pet_Food )
			{
				SC_PetEventResult( Owner->GUID() , PetPos , Event , Event , 0 , false );
				return;
			}
			
/*			if( PetBase.Hunger + ItemDB->Item.PetFood.Hunger > _MAX_PET_HUNGER_ )
			{
				SC_PetEventResult( Owner->GUID() , PetPos , Event , Event , 0 , false );
				Owner->Msg("吃太飽了");
				return;
			}*/

			if( PetBase.Spirit + ItemDB->Item.PetFood.Spirit < 0 )
			{
				SC_PetEventResult( Owner->GUID() , PetPos , Event , Event , 0 , false );
				Owner->Msg("歷練不足");
				return;
			}

			if( ItemDB->Item.PetFood.Hunger != 0)
			{
				PetBase.Hunger += ItemDB->Item.PetFood.Hunger;
				if( PetBase.Hunger < 0 )
					PetBase.Hunger = 0;
				if( PetBase.Hunger > _MAX_PET_HUNGER_ )
				{
					PetBase.Hunger = _MAX_PET_HUNGER_;
					Owner->Net_GameMsgEvent( EM_GameMessageEvent_Pet_Hunger_Full );
				}
				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Hunger );
				Owner->Net_FixRoleValue( ValueType , PetBase.Hunger );
			}

			if( ItemDB->Item.PetFood.Spirit != 0)
			{
				PetBase.Spirit += ItemDB->Item.PetFood.Spirit;
				if( PetBase.Spirit < 0 )
					PetBase.Spirit = 0;
				if( PetBase.Spirit > _MAX_PET_UNDERGOPOINT_ )
					PetBase.Spirit = _MAX_PET_UNDERGOPOINT_;
				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Spirit );
				Owner->Net_FixRoleValue( ValueType , PetBase.Spirit );
			}

			if( ItemDB->Item.PetFood.SkillPoint != 0)
			{
				PetBase.SkillPoint += ItemDB->Item.PetFood.SkillPoint;
				if( PetBase.SkillPoint < 0 )
					PetBase.SkillPoint = 0;
				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_SKillPoint );
				Owner->Net_FixRoleValue( ValueType , PetBase.SkillPoint );

				PetBase.TotalSkillPoint += ItemDB->Item.PetFood.SkillPoint;
				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_TotalSkillPoint );
				Owner->Net_FixRoleValue( ValueType , PetBase.TotalSkillPoint );
			}
			if( ItemDB->Item.PetFood.Loyal != 0)
			{
				PetBase.Loyal += ItemDB->Item.PetFood.Loyal;
				if( PetBase.Loyal < 0 )
					PetBase.Loyal = 0;
				if( PetBase.Loyal > _MAX_PET_LOYAL_ )
				{
					PetBase.Loyal = _MAX_PET_LOYAL_;
					Owner->Net_GameMsgEvent( EM_GameMessageEvent_Pet_Loyal_Full );
				}
				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Loyal );
				Owner->Net_FixRoleValue( ValueType , PetBase.Loyal );
			}
			if( ItemDB->Item.PetFood.Talent != 0)
			{
				PetBase.Talent += ItemDB->Item.PetFood.Talent;
				if( PetBase.Talent < 0 )
					PetBase.Talent = 0;
				if( PetBase.Talent > _MAX_PET_TALENT_ )
					PetBase.Talent = _MAX_PET_TALENT_;
				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Talent );
				Owner->Net_FixRoleValue( ValueType , PetBase.Talent );
			}
			if( ItemDB->Item.PetFood.Exp != 0)
			{
				PetBase.Exp += ItemDB->Item.PetFood.Exp;
				ReCalPetLv( Owner , PetPos );
				//////////////////////////////////////////////////////////////////////////

				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Exp );
				Owner->Net_FixRoleValue( ValueType , PetBase.Exp );
			}

			switch( ItemDB->Item.PetFood.EffectType )
			{
			case EM_PetFoodEffectType_None:			//無
				break;
			case EM_PetFoodEffectType_STR:			//STR
				PetBase.STR += ItemDB->Item.PetFood.EffectValue;
				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Str );
				Owner->Net_FixRoleValue( ValueType , PetBase.STR );
				break;
			case EM_PetFoodEffectType_STA:			//STA
				PetBase.STA += ItemDB->Item.PetFood.EffectValue;
				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Sta );
				Owner->Net_FixRoleValue( ValueType , PetBase.STA );
				break;
			case EM_PetFoodEffectType_INT:			//INT
				PetBase.INT += ItemDB->Item.PetFood.EffectValue;
				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Int );
				Owner->Net_FixRoleValue( ValueType , PetBase.INT );
				break;
			case EM_PetFoodEffectType_MND:			//MND
				PetBase.MND += ItemDB->Item.PetFood.EffectValue;
				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Mnd );
				Owner->Net_FixRoleValue( ValueType , PetBase.MND );
				break;
			case EM_PetFoodEffectType_AGI:			//AGI
				PetBase.AGI += ItemDB->Item.PetFood.EffectValue;
				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Agi );
				Owner->Net_FixRoleValue( ValueType , PetBase.AGI );
				break;
			case EM_PetFoodEffectType_PetProperty:	//改變屬性
				PetBase.Property = (MagicResistENUM)ItemDB->Item.PetFood.EffectValue;
				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Property );
				Owner->Net_FixRoleValue( ValueType , PetBase.Property );
				break;
			case EM_PetFoodEffectType_PetObjID:		//改變外觀
				PetBase.PetOrgID = ItemDB->Item.PetFood.EffectValue;
				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_OrgObjID );
				Owner->Net_FixRoleValue( ValueType , PetBase.PetOrgID );
				break;
			}
	


			if( bodyItem->Count <= 1 )
				bodyItem->Init();
			else
				bodyItem->Count --;
			
			Owner->Net_FixItemInfo_Body( ItemPos );

		}
		break;
	case EM_CultivatePetCommandType_Travel:			//旅行
		{			
			sprintf_s( Buf , sizeof(Buf) , "PetCallBack_Travel_Begin(%d,%d,%d,%d)" , PetPos , PetBase.PetOrgID , PetBase.PetType , PetBase.Property );
			if( LUA_VMClass::PCallByStrArg( Buf , Owner->GUID() , Owner->GUID() , &RetList , 1 ) )
			{
				if( RetList.size() == 1 )
				{
					EventTime = RetList[0].Number_Int;
				}
			}

			if( EventTime != 0 )
			{
				PetBase.EventType = EM_CultivatePetCommandType_Travel;			
				PetBase.EventColdown = EventTime;
			}
		}
		break;
	case EM_CultivatePetCommandType_Free:			//放生
		PetBase.init();
		break;
	case EM_CultivatePetCommandType_Train:			//特訓
		{
			/*
			if(		PetBase.LastTran.AGI != 0 
				||	PetBase.LastTran.STR != 0 
				||	PetBase.LastTran.STA != 0 
				||	PetBase.LastTran.MND != 0 
				||	PetBase.LastTran.INT != 0 	)
			{
				SC_PetEventResult( Owner->GUID() , PetPos , Event , Event , 0 , false );
				Owner->Msg("還沒 驗收確定");
				return;
			}
			*/
/*			if( PetBase.TrainCount <= 0 )
			{
				SC_PetEventResult( Owner->GUID() , PetPos , Event , Event , 0 , false );
				Owner->Msg("沒訓練次數");
				return;
			}

			PetBase.TrainCount--;
			ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_TrainCount );
			Owner->Net_FixRoleValue( ValueType , PetBase.TrainCount );
			*/
			SC_PetEventProcess( Owner->GUID() , PetPos , Event );
			sprintf_s( Buf , sizeof(Buf) , "PetCallBack_Train(%d,%d,%d,%d)" , PetPos , PetBase.PetOrgID , PetBase.PetType , PetBase.Property );
			LUA_VMClass::PCallByStrArg( Buf , Owner->GUID() , Owner->GUID() );
		}
		break;
		/*
	case EM_CultivatePetCommandType_TrainCheck:		//驗收
		{
			sprintf_s( Buf , sizeof(Buf) , "PetCallBack_TrainCheck(%d,%d,%d,%d)" , PetPos , PetBase.PetOrgID , PetBase.PetType , PetBase.Property );
			LUA_VMClass::PCallByStrArg( Buf , Owner->GUID() , Owner->GUID() );
		}
		break;
	case EM_CultivatePetCommandType_ReTrain:		//複訓(使用物品)
		{
			int Pet_ReTrainItemID = g_ObjectData->SysValue().Pet_ReTrainItemID;
			ItemFieldStruct* bodyItem = Owner->GetBodyItem( ItemPos );
			if( bodyItem == NULL )
			{
				SC_PetEventResult( Owner->GUID() , PetPos , Event , Event , 0 , false );
				return;
			}

			Owner->DelOneItem( ItemPos , *bodyItem , EM_ActionType_PetRetrain_Destroy );
			PetBase.TrainCheckCount++;

			ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_TrainCheckCount );
			Owner->Net_FixRoleValue( ValueType , PetBase.TrainCheckCount );

			sprintf_s( Buf , sizeof(Buf) , "PetCallBack_TrainCheck(%d,%d,%d,%d)" , PetPos , PetBase.PetOrgID , PetBase.PetType , PetBase.Property );
			LUA_VMClass::PCallByStrArg( Buf , Owner->GUID() , Owner->GUID() );
		}
		break;		
	case EM_CultivatePetCommandType_TrainAccept:	//驗收確定
		{
			PetBase.TranAccept();
		}
		break;
*/
	case EM_CultivatePetCommandType_Return:			//召喚出的寵物回收
		{
			if( PetBase.EventType != EM_CultivatePetCommandType_Summon )
			{
				SC_PetEventResult( Owner->GUID() , PetPos , Event , Event , 0 , false );
				return;
			}
			PetStruct&	PetInfo = Owner->TempData.SummonPet.Info[ EM_SummonPetType_CultivatePet ];
			//把寵物回收
			BaseItemObject* OldPetObj = Global::GetObj( PetInfo.PetID );
			if(		OldPetObj != NULL 
				&&	OldPetObj->Role()->TempData.Sys.OwnerDBID == Owner->DBID() 
				&&  OldPetObj->Role()->BaseData.ItemInfo.OrgObjID == PetBase.PetOrgID )
			{
				OldPetObj->Destroy( "EM_CultivatePetCommandType_Return" );
			}
			PetInfo.PetID = -1;
			PetBase.EventType = EM_CultivatePetCommandType_None;
			Owner->TempData.UpdateInfo.Recalculate_All = true;

		}
		break;
	}

//	ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_EventColdown );
//	Owner->Net_FixRoleValue( ValueType , EventTime );

	SC_PetEventResult( Owner->GUID() , PetPos , PetBase.EventType , Event , EventTime , true );


}
void NetSrv_CultivatePetChild::RC_LearnSkill( BaseItemObject* Obj , int PetPos , int MagicID , int MagicLv )
{
	RoleDataEx* Owner = Obj->Role();

	if( Owner->CheckPetPosTimeFlag( PetPos ) == false )
		return;

	if(		(unsigned)PetPos >= MAX_CultivatePet_Count 
		||	MagicLv < 0 )
	{
		Owner->Msg( "寵物位置 或是 學習法術等級有問題" );
		SC_LearnSkillResult( Owner->GUID() , PetPos , MagicID , MagicLv, false );
		return;
	}

	GameObjDbStructEx* MagicColDB = Global::GetObjDB( MagicID );
	if( MagicColDB->IsMagicCollect() == false || MagicColDB->MagicCol.MaxSkillLv < MagicLv )
	{
		Owner->Msg( "法術ID 有問題 或是 可學習等級上限錯誤" );
		SC_LearnSkillResult( Owner->GUID() , PetPos , MagicID , MagicLv, false );
		return;
	}


	CultivatePetStruct&	PetBase	= Owner->PlayerBaseData->Pet.Base[ PetPos ];
	if( PetBase.PetOrgID == 0 )
	{
		Owner->Msg( "寵物資料不存在");
		SC_LearnSkillResult( Owner->GUID() , PetPos , MagicID , MagicLv, false );
		return;
	}

	if( PetBase.EventType == EM_CultivatePetCommandType_Summon )
	{
		Owner->Msg( "招喚中不能學習技能");
		SC_LearnSkillResult( Owner->GUID() , PetPos , MagicID , MagicLv, false );
		return;
	}

	int OrgMagicLv = -1;
	//察是否已有此法術
	for( int i = 0 ; i < PetBase.Magic.Size() ; i++ )
	{
		if( PetBase.Magic[i].ID == MagicID )
		{
			OrgMagicLv = PetBase.Magic[i].Lv;
			break;

		}
	}


	//找出此法術的學習資料表
	//CultivatePetLearnMagicTableStruct* MagicTable = g_ObjectData->GetCultivatePetLearnMagic( MagicID , MagicLv );
	CultivatePetLearnMagicTableStruct* MagicTable = g_ObjectData->GetNextCultivatePetLearnMagic( MagicID , OrgMagicLv );

	if(		MagicTable == NULL 
		||	MagicTable->PowerLv != MagicLv )
	{
		Owner->Msg("法術學習資料表錯誤 或是 學習等級有問題");
		SC_LearnSkillResult( Owner->GUID() , PetPos , MagicID , MagicLv, false );
		return;
	}

	if( MagicTable->SkillPoint < 0 || MagicTable->SkillPoint > PetBase.SkillPoint )
	{
		Owner->Msg("學習技能點數錯誤 或是 學習技能表的學習點數有問題");
		SC_LearnSkillResult( Owner->GUID() , PetPos , MagicID , MagicLv, false );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	//學習調件檢查
	if( MagicTable->Limit.PetProperty != -1 && PetBase.Property != MagicTable->Limit.PetProperty )
	{
		Owner->Msg("學習條件不符合 此寵物不能學習");
		SC_LearnSkillResult( Owner->GUID() , PetPos , MagicID , MagicLv, false );
		return;
	}
	
	if( MagicTable->Limit.PetType != -1 )
	{
		//if( MagicTable->Limit.Lock == 1)
		{
			if( PetBase.Property != MagicTable->Limit.PetType )
			{
				Owner->Msg("學習條件不符合 固定種族專屬");
				SC_LearnSkillResult( Owner->GUID() , PetPos , MagicID , MagicLv, false );
				return;
			}
		}
/*		else
		{
			if(  PetBase.CheckPetFlag( MagicTable->Limit.PetType ) == false )
			{
				Owner->Msg("學習條件不符合 沒有融合過此種寵物");
				SC_LearnSkillResult( Owner->GUID() , PetPos , MagicID , MagicLv, false );
				return;
			}			
		}*/

	}
	//數值檢查
	if(		MagicTable->Limit.Str > PetBase.STR 
		||	MagicTable->Limit.Sta > PetBase.STA 
		||	MagicTable->Limit.Mnd > PetBase.MND
		||	MagicTable->Limit.Agi > PetBase.AGI
		||	MagicTable->Limit.Int > PetBase.INT		)
	{
		Owner->Msg("學習條件不符合 屬性不足");
		SC_LearnSkillResult( Owner->GUID() , PetPos , MagicID , MagicLv, false );
		return;
	}

		
	//////////////////////////////////////////////////////////////////////////
	bool IsFind = false;
	//察是否已有此法術
	for( int i = 0 ; i < PetBase.Magic.Size() ; i++ )
	{
		if( PetBase.Magic[i].ID == MagicID )
		{
			if( PetBase.Magic[i].Lv >= MagicLv )
			{
				SC_LearnSkillResult( Owner->GUID() , PetPos , MagicID , MagicLv, false );
				return;
			}
			else
			{
				PetBase.Magic[i].Lv = MagicLv;
				IsFind = true;
			}
		}
	}
	
	if( IsFind == false ) 
	{
		MagicBaseStruct NewMagic;
		NewMagic.ID = MagicID;
		NewMagic.Lv = MagicLv;

		PetBase.Magic.Push_Back( NewMagic );
	}
	//Owner->TempData.UpdateInfo.Recalculate_All = true;
	RoleValueName_ENUM ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_SKillPoint);
	Owner->SetValue( ValueType , PetBase.SkillPoint - MagicTable->SkillPoint );
	SC_LearnSkillResult( Owner->GUID() , PetPos , MagicID , MagicLv, true );
	return;
}
void NetSrv_CultivatePetChild::RC_PetLifeSkill( BaseItemObject* Obj , int PetPos , CultivatePetLifeSkillTypeENUM SkillType , int LV , int TablePos )
{
	RoleValueName_ENUM ValueType;
	RoleDataEx* Owner = Obj->Role();

	if( (unsigned)PetPos >= MAX_CultivatePet_Count )
	{
		SC_PetLifeResult( Owner->GUID() , SkillType , PetPos , 0 , false );
		return;
	}

	CultivatePetStruct&	PetBase	= Owner->PlayerBaseData->Pet.Base[ PetPos ];
	if( PetBase.PetOrgID == 0 )
	{
		SC_PetLifeResult( Owner->GUID() , SkillType , PetPos , 0 , false );
		return;
	}

	if(		EM_CultivatePetLifeSkillType_None == SkillType 
		&&	PetBase.EventType == EM_CultivatePetCommandType_LiftSkill	)
	{
		PetBase.EventType = EM_CultivatePetCommandType_None;
		PetBase.EventColdown = 0;

		ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_EventColdown );
		Owner->Net_FixRoleValue( ValueType , PetBase.EventColdown );

		SC_PetLifeResult( Owner->GUID() , SkillType , PetPos , 0 , true );
		return;
	}

/*
	if( SkillType == EM_CultivatePetLifeSkillType_None )
	{
		PetBase.EventType = EM_CultivatePetCommandType_None;
		PetBase.EventColdown = 0;
		ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_EventColdown );
		Owner->Net_FixRoleValue( ValueType , PetBase.EventColdown );

		SC_PetLifeResult( Owner->GUID() , SkillType , PetPos , 0 , true );
		return;
	}
	*/
	if( PetBase.EventType != EM_CultivatePetCommandType_None )
	{
		SC_PetLifeResult( Owner->GUID() , SkillType , PetPos , 0 , true );
		return;
	}


	if( unsigned( SkillType ) >= EM_CultivatePetLifeSkillType_Max )
	{
		SC_PetLifeResult( Owner->GUID() , SkillType , PetPos , 0 , false );
		return;
	}



	CultivatePetLifeSkillTableStruct* Table = GetVectorPosPoint( g_ObjectData->_CultivatePetLifeSkillTable , TablePos  );

	if( Table == NULL || Table->SkillType != SkillType || Table->Need.SkillLv > PetBase.LifeSkill[SkillType] )
	{
		SC_PetLifeResult( Owner->GUID() , SkillType , PetPos , 0 , false );
		return;
	}

	if(		Table->Need.ToolID != 0 
		&&	Table->Need.ToolID != PetBase.Tools.OrgObjID )
	{
		SC_PetLifeResult( Owner->GUID() , SkillType , PetPos , 0 , false );
		return;
	}

	if(		Table->Need.ItemCount != 0 
		&&	(		Table->Need.ItemID != PetBase.Source.OrgObjID 
				||	Table->Need.ItemCount > PetBase.Source.Count )	)
	{
		SC_PetLifeResult( Owner->GUID() , SkillType , PetPos , 0 , false );
		return;
	}

	if( PetBase.LifeSkillTablePos != TablePos )
	{
		if(		PetBase.Product[0].IsEmpty() == false 
			||	PetBase.Product[1].IsEmpty() == false 
			||	PetBase.Product[2].IsEmpty() == false )
		{
			SC_PetLifeResult( Owner->GUID() , SkillType , PetPos , 0 , false );
			return;
		}
	}

	PetBase.EventType = EM_CultivatePetCommandType_LiftSkill;
	PetBase.LifeSkillTablePos	= TablePos;
	PetBase.EventColdown		= Table->EventTime;

	SC_PetLifeResult( Owner->GUID() , SkillType , PetPos , Table->EventTime , true );

	ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_EventColdown );
	Owner->Net_FixRoleValue( ValueType , PetBase.EventColdown );

}
void NetSrv_CultivatePetChild::RC_SwapItem( BaseItemObject* Obj , int PetPos , CultivatePetEQTypeENUM PetItemPos , int BodyPos , bool IsFromBody )
{
	RoleDataEx* Owner = Obj->Role();
	if(	(unsigned)PetPos >= MAX_CultivatePet_Count )
		
	{
		SC_SwapItemResult( Owner->GUID() , false );
		return;
	}

	CultivatePetStruct&	PetBase	= Owner->PlayerBaseData->Pet.Base[ PetPos ];
	ItemFieldStruct* BodyItem = Owner->GetBodyItem( BodyPos );
	if(		PetBase.PetOrgID == 0 
		||	BodyItem == NULL		
		||	(unsigned)PetItemPos >= EM_CultivatePetEQType_MAX 
		||	PetBase.EventType != EM_CultivatePetCommandType_None )
	{
		SC_SwapItemResult( Owner->GUID() , false );
		return;
	}

	if(		PetItemPos == EM_CultivatePetEQType_Product1
		||	PetItemPos == EM_CultivatePetEQType_Product2 
		||	PetItemPos == EM_CultivatePetEQType_Product3	)
	{
		SC_SwapItemResult( Owner->GUID() , false );
		return;
	}


	ItemFieldStruct* PetItem = &PetBase.EQ[ PetItemPos ];

	if( BodyItem->IsEmpty() != false)
	{
		*BodyItem = *PetItem;
		PetItem->Init();
	}
	else
	{
		GameObjDbStructEx* BodyItemDB = Global::GetObjDB( BodyItem->OrgObjID );
		if( BodyItemDB->IsItem() == false )
		{
			SC_SwapItemResult( Owner->GUID() , false );
			return;
		}

		if( BodyItem->OrgObjID == PetItem->OrgObjID )
		{
			int TotalCount = BodyItem->Count + PetItem->Count;
			
			if( TotalCount <= BodyItemDB->MaxHeap )
			{
				PetItem->Count = TotalCount;
				BodyItem->Init();
			}
			else
			{
				PetItem->Count = BodyItemDB->MaxHeap;
				BodyItem->Count = TotalCount - BodyItemDB->MaxHeap;
			}

			if( IsFromBody == false )
			{
				std::swap( *PetItem , *BodyItem );
			}
		}
		else
		{
			bool IsOK = true;
			switch( PetItemPos )
			{
			case EM_CultivatePetEQType_Tools:
				if( BodyItemDB->Item.ItemType != EM_ItemType_Pet_Tools )
					IsOK = false;
				break;
			case EM_CultivatePetEQType_Source:
				break;
			case EM_CultivatePetEQType_Product1:
			case EM_CultivatePetEQType_Product2:
			case EM_CultivatePetEQType_Product3:
				break;
			case EM_CultivatePetEQType_Other0:
			case EM_CultivatePetEQType_Other1:
			case EM_CultivatePetEQType_Other2:
			case EM_CultivatePetEQType_Other3:
				if( BodyItemDB->Item.ItemType != EM_ItemType_Pet_Furniture )
					IsOK = false;
				break;
			}

			if( IsOK == false )
			{
				SC_SwapItemResult( Owner->GUID() , false );
				return;
			}


			std::swap( *PetItem , *BodyItem );
		}

	}
	
	Owner->Net_FixItemInfo_Body( BodyPos );
	SC_FixItem( Owner->GUID() , PetPos , PetItemPos , *PetItem );
	SC_SwapItemResult( Owner->GUID() , true );

}

void NetSrv_CultivatePetChild::RC_GetProductRequest	( BaseItemObject* Obj , int PetPos )
{
	RoleDataEx* Owner = Obj->Role();
	if(	(unsigned)PetPos >= MAX_CultivatePet_Count )
	{
		SC_GetProductResult( Owner->GUID() , false );
		return;
	}

	CultivatePetStruct&	PetBase	= Owner->PlayerBaseData->Pet.Base[ PetPos ];

	for( int i = 0 ; i < MAX_CultivatePet_Product_Count_ ; i++ )
	{
		ItemFieldStruct& PetItem = PetBase.Product[i];
		if( PetItem.IsEmpty() )
			continue;

		PetItem.Serial		= GenerateItemVersion( );
		PetItem.CreateTime	= RoleDataEx::G_Now;

		GameObjDbStructEx* PetItemDB = Global::GetObjDB( PetItem.OrgObjID );
		if( PetItemDB == NULL )
			continue;
		for( int j = _MAX_BODY_BEGIN_POS_ ; j < Owner->PlayerBaseData->Body.Count ; j++ )
		{
			if( Owner->CheckItemPosTimeFlag( 0 , j ) == false )
				continue;

			ItemFieldStruct& BodyItem = Owner->PlayerBaseData->Body.Item[j];

			if( BodyItem.IsEmpty() )
			{
				BodyItem = PetItem;
				BodyItem.Count = 0;
				//新的要產生新序號，不然有可能重覆被取消掉
				BodyItem.Serial = GenerateItemVersion( );
			}

			if( BodyItem.OrgObjID != PetItem.OrgObjID  )
				continue;
			if( BodyItem.Count == PetItemDB->MaxHeap )
				continue;

			if( PetItem.Count + BodyItem.Count > PetItemDB->MaxHeap )
			{
				PetItem.Count = PetItem.Count + BodyItem.Count - PetItemDB->MaxHeap;
				BodyItem.Count = PetItemDB->MaxHeap;
				Owner->Net_FixItemInfo_Body( j );
			}
			else
			{
				BodyItem.Count = PetItem.Count + BodyItem.Count;
				PetItem.Init();						
				Owner->Net_FixItemInfo_Body( j );
				break;
			}			
		}

		SC_FixItem( Owner->GUID() , PetPos , EM_CultivatePetEQType_Product1 + i , PetItem );
		
		if( PetItem.IsEmpty() == false )
		{
			Owner->Msg( "欄位不夠" );
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_Pet_BagFull );
			break;
		}
	}

	SC_GetProductResult( Owner->GUID() , true );
}
void NetSrv_CultivatePetChild::ReCalPetLv			( RoleDataEx* Owner, int PetPos )
{
	CultivatePetStruct&	PetBase	= Owner->PlayerBaseData->Pet.Base[ PetPos ];
	RoleValueName_ENUM ValueType;
	if( PetBase.Lv == 0 )
		PetBase.Lv = 1;

	//取得目前等級所需要的exp
	while(		(unsigned) PetBase.Lv < Owner->PlayerBaseData->MaxLv
		&&		(unsigned) PetBase.Lv <= (unsigned)RoleDataEx::PetExpTable.size() )
	{
		int LevelUpExp = RoleDataEx::PetExpTable[ PetBase.Lv ];
		if( LevelUpExp > PetBase.Exp )
			return;
		
		PetBase.Exp -= LevelUpExp;
		ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Exp );
		Owner->Net_FixRoleValue( ValueType , PetBase.Exp );

		PetBase.Lv++;
		ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Lv );
		Owner->Net_FixRoleValue( ValueType , PetBase.Lv );
		//////////////////////////////////////////////////////////////////////////
		//增加數值
		GameObjDbStructEx* ItemEggDB = Global::GetObjDB( PetBase.ItemOrgID );
		if( ItemEggDB == NULL )
			return;
		PetBase.STR += ItemEggDB->Item.Pet.STR * 0.5f * PetBase.Talent / 100;
		PetBase.STA += ItemEggDB->Item.Pet.STA * 0.5f * PetBase.Talent / 100;
		PetBase.MND += ItemEggDB->Item.Pet.MND * 0.5f * PetBase.Talent / 100;
		PetBase.AGI += ItemEggDB->Item.Pet.AGI * 0.5f * PetBase.Talent / 100;
		PetBase.INT += ItemEggDB->Item.Pet.INT * 0.5f * PetBase.Talent / 100;

		PetBase.SkillPoint		+= ( PetBase.Talent + PetBase.Loyal )/2;
		PetBase.TotalSkillPoint += ( PetBase.Talent + PetBase.Loyal )/2;

		ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Str );
		Owner->Net_FixRoleValue( ValueType , PetBase.STR );
		ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Sta );
		Owner->Net_FixRoleValue( ValueType , PetBase.STA );
		ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Agi );
		Owner->Net_FixRoleValue( ValueType , PetBase.AGI );
		ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Int );
		Owner->Net_FixRoleValue( ValueType , PetBase.INT );
		ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Mnd );
		Owner->Net_FixRoleValue( ValueType , PetBase.MND );

		ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_SKillPoint );
		Owner->Net_FixRoleValue( ValueType , PetBase.SkillPoint );
		ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_TotalSkillPoint );
		Owner->Net_FixRoleValue( ValueType , PetBase.TotalSkillPoint );
		//////////////////////////////////////////////////////////////////////////
		//Call lua function
		char	Buf[128];
		sprintf_s( Buf , sizeof(Buf) , "PetCallBack_LevelUp(%d,%d,%d,%d)" , PetPos , PetBase.PetOrgID , PetBase.PetType , PetBase.Property );
		LUA_VMClass::PCallByStrArg( Buf , Owner->GUID() , Owner->GUID() ) ;
	}

	if( (unsigned) PetBase.Lv >= (unsigned)Owner->PlayerBaseData->MaxLv )
	{
		PetBase.Exp = 0;
		ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos + EM_CultivatePetStructValueType_Exp );
		Owner->Net_FixRoleValue( ValueType , PetBase.Exp );
	}
}
void NetSrv_CultivatePetChild::RC_PetMarge			( BaseItemObject* Obj , int PetCount , int PetPos[3] )
{
	RoleDataEx* Owner = Obj->Role();

	if( PetCount != 2 && PetCount != 3 )
	{
		SC_PetMargeResultFailed	( Owner->GUID() , EM_PetMargeResult_DataErr );
		return;
	}

	if( (myUInt32)Owner->PlayerBaseData->Pet.NextMergeResetTime < RoleDataEx::G_Now )
	{
		Owner->SetValue(  EM_RoleValue_CultivatePet_NextMergeResetTime
						, int( ( RoleDataEx::G_Now + (RoleDataEx::G_TimeZone-6)*60*60 )/60/60/24 * 60*60*24 + (30 - RoleDataEx::G_TimeZone)*60*60 ) );

		for( int i = 0 ; i < MAX_CultivatePet_Count ; i++ )
		{
			CultivatePetStruct& PetTemp = Owner->PlayerBaseData->Pet.Base[i];
			if( PetTemp.IsEmpty() )
				continue;
			PetTemp.DayMergeCount = 0;
			RoleValueName_ENUM ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * i + EM_CultivatePetStructValueType_MergeCount );
			Owner->Net_FixRoleValue( ValueType , 0 );
		}
	}


	
	//檢查寵物是否存在
	CultivatePetStruct*	PetBase[3] = { NULL , NULL , NULL };

	for( int i = 0 ; i < PetCount ; i++ )
	{
		if( (unsigned)PetPos[i] >= MAX_CultivatePet_Count )
		{
			SC_PetMargeResultFailed	( Owner->GUID() , EM_PetMargeResult_DataErr );
			return;
		}
		PetBase[i] = &(Owner->PlayerBaseData->Pet.Base[ PetPos[i] ]);

		if( PetBase[i]->EventType != EM_CultivatePetCommandType_None )
		{
			SC_PetMargeResultFailed	( Owner->GUID() , EM_PetMargeResult_DataErr );
			return;
		}
	}
	
	if(		PetBase[0] == PetBase[1] 
		||	PetBase[1] == PetBase[2] 
		||	PetBase[0] == PetBase[2] )
	{
		SC_PetMargeResultFailed	( Owner->GUID() , EM_PetMargeResult_DataErr );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	//計算種物類型
	{
		for( int i = 1 ; i < PetCount ; i++ )
		{
			if( PetBase == NULL )
				break;

			if(		PetBase[i]->Property != EM_MagicResist_None 
				&&	PetBase[i]->Property != PetBase[0]->Property )
			{
				SC_PetMargeResultFailed	( Owner->GUID() , EM_PetMargeResult_DataErr );
				return;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Result;

	//////////////////////////////////////////////////////////////////////////
	//融合條件檢查
	for( int i = 1 ; i < PetCount ; i++ )
	{
		//等級檢查(子體等級高於主體 ，不得相融。)
		if( PetBase[0]->Lv < PetBase[i]->Lv  )
		{
			SC_PetMargeResultFailed	( Owner->GUID() , EM_PetMargeResult_LevelErr );
			return;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	if( PetBase[0]->DayMergeCount >= 3 )
	{
		SC_PetMargeResultFailed	( Owner->GUID() , EM_PetMargeResult_MergeCountErr );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	if( PetCount == 3 )
	{
		int TicketID = g_ObjectData->GetSysKeyValue( "Pet_TriMergeTicket" );
		if( Owner->DelBodyItem( TicketID , 1 , EM_ActionType_DestroyItem_PetTriMerge ) == false )
		{
			SC_PetMargeResultFailed	( Owner->GUID() , EM_PetMargeResult_MergeTicketErr );
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//數值計算
	//1 當 子體天資 > 主體天資 時，融合後之數值為 [(子體天資 - 主體天資)*0.25]+主體天資
	//2 融合後，主體將獲得歷練值數值為：主體歷練 + 子體歷練
	//3 融合後，主體將獲得經驗值數值為：1000/( 1 + | 主體等級 - 子體等級 | )
	//4 當 該子體能力 > 該主體能力 時，融合後之數值為 [(子體能力 - 主體能力)*0.45]+主體能力
	

	CultivatePetStruct PetBk = *PetBase[0];

	for( int i = 1 ; i < PetCount ; i++ )
	{
		if( PetBk.Talent < PetBase[i]->Talent )
			PetBase[0]->Talent += ( PetBase[i]->Talent - PetBk.Talent ) *0.25f;

		if( PetBase[0]->Talent > _MAX_PET_TALENT_ )
			PetBase[0]->Talent = _MAX_PET_TALENT_;

		PetBase[0]->Spirit += PetBase[i]->Spirit;
		if( PetBase[0]->Spirit > _MAX_PET_UNDERGOPOINT_ )
			PetBase[0]->Spirit = _MAX_PET_UNDERGOPOINT_;

		PetBase[0]->Exp += 1000/( 1 + abs( PetBk.Lv - PetBase[i]->Lv ) );

		if( PetBk.STR < PetBase[i]->STR )
			PetBase[0]->STR += ( PetBase[i]->STR - PetBk.STR )*0.45f;
		if( PetBk.STA < PetBase[i]->STA )
			PetBase[0]->STA += ( PetBase[i]->STA - PetBk.STA )*0.45f;
		if( PetBk.MND < PetBase[i]->MND )
			PetBase[0]->MND += ( PetBase[i]->MND - PetBk.MND )*0.45f;
		if( PetBk.INT < PetBase[i]->INT )
			PetBase[0]->INT += ( PetBase[i]->INT - PetBk.INT )*0.45f;
		if( PetBk.AGI < PetBase[i]->AGI )
			PetBase[0]->AGI += ( PetBase[i]->AGI - PetBk.AGI )*0.45f;
	}

	ReCalPetLv( Owner , PetPos[0] );

	RoleValueName_ENUM ValueType;

	ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos[0] + EM_CultivatePetStructValueType_Talent );
	Owner->Net_FixRoleValue( ValueType , PetBase[0]->Talent );
	ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos[0] + EM_CultivatePetStructValueType_Spirit );
	Owner->Net_FixRoleValue( ValueType , PetBase[0]->Spirit );
	ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos[0] + EM_CultivatePetStructValueType_Exp );
	Owner->Net_FixRoleValue( ValueType , PetBase[0]->Exp );

	ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos[0] + EM_CultivatePetStructValueType_Str );
	Owner->Net_FixRoleValue( ValueType , PetBase[0]->STR );
	ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos[0] + EM_CultivatePetStructValueType_Sta );
	Owner->Net_FixRoleValue( ValueType , PetBase[0]->STA );
	ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos[0] + EM_CultivatePetStructValueType_Agi );
	Owner->Net_FixRoleValue( ValueType , PetBase[0]->AGI );
	ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos[0] + EM_CultivatePetStructValueType_Int );
	Owner->Net_FixRoleValue( ValueType , PetBase[0]->INT );
	ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos[0] + EM_CultivatePetStructValueType_Mnd );
	Owner->Net_FixRoleValue( ValueType , PetBase[0]->MND );


	//////////////////////////////////////////////////////////////////////////
	for( int i = 1 ; i < PetCount ; i++ )
		PetBase[i]->init();

	PetBase[0]->DayMergeCount++;

	ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * PetPos[0] + EM_CultivatePetStructValueType_MergeCount );
	Owner->Net_FixRoleValue( ValueType , PetBase[0]->DayMergeCount );

	//確定融合
	SC_PetMargeResultOK( Owner->GUID() , PetCount , PetPos , *(PetBase[0]) );
}

void NetSrv_CultivatePetChild::RC_RegPetCard( BaseItemObject* obj , int petPos )
{
	/*
	RoleDataEx* Owner = obj->Role();
	if( (unsigned)petPos >= MAX_CultivatePet_Count )
	{
		SC_RegPetCardResult( Owner->GUID() , false );
		return;
	}
	CultivatePetStruct& Pet = Owner->PlayerBaseData->Pet.Base[petPos];

	//檢查此寵物對應的ID
	int CardID = -1;
	for ( unsigned i = 0 ; i < g_ObjectData->_PetCardList.size() ; i++ )
	{
		if( g_ObjectData->_PetCardList[i] == Pet.PetOrgID )
		{
			CardID = i;
			break;
		}
	}
	if( CardID == -1 )
	{
		SC_RegPetCardResult( Owner->GUID() , false );
		return;
	}

	Owner->SetValue( (RoleValueName_ENUM)(EM_RoleValue_PetCard + CardID) , 1 );
	
	SC_RegPetCardResult( Owner->GUID() , true );
	*/
}