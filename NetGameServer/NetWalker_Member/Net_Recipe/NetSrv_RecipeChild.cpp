#include "../NetWakerGSrvInc.h"
#include "NetSrv_RecipeChild.h"
#include "../../mainproc/HousesManage/HousesManageClass.h"
#include <math.h>
//-----------------------------------------------------------------
bool    NetSrv_RecipeChild::Init()
{
    NetSrv_Recipe::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_RecipeChild );


    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_RecipeChild::Release()
{
    if( This == NULL )
        return false;

   NetSrv_Recipe::_Release();

    delete This;
    This = NULL;

     return true;
    
}
//-----------------------------------------------------------------
//Client �ݭn�D�X�����~
void NetSrv_RecipeChild::RC_LockResourceAndRefineRequest( BaseItemObject* Sender , int RecipeID )
{
/*	static bool IsInit = false;
	static int  ItemUpgradeRate[10] = { 0,0,0,0,0,0,0,0,0,0 };

	if( IsInit == false )
	{
		IsInit = true;
		ReadLuaArray( Sender->GUID() , "LuaS_RecipeItemUpgrade" , 10 , ItemUpgradeRate );
	}
*/
	RoleDataEx* Owner = Sender->Role();
	//�ˬd�t��
	GameObjDbStructEx*	RecipeDB;
	RecipeDB = Global::GetObjDB( RecipeID );
	if( RecipeDB->IsRecipe() == false )
	{
		SC_RefineResultError( Sender->GUID() , RecipeID , EM_RefineResult_RecipeError );
		return;
	}

	if( RecipeDB->Recipe.CraftFee < 0 )
		return;

	//�ˬd���n���~
	int KeyItemID = RecipeDB->Recipe.Request.KeyItemID;

	GameObjDbStructEx*	KeyItemDB = Global::GetObjDB( KeyItemID );

	if( KeyItemDB->IsKeyItem() && Owner->CheckKeyItem( KeyItemID ) == false )
	{
		SC_RefineResultError( Sender->GUID() , RecipeID , EM_RefineResult_RecipeError );
		return;
	}

	if( RecipeDB->Recipe.CraftFee != 0 )
	{
		if( Owner->BodyMoney() < RecipeDB->Recipe.CraftFee )
		{
			SC_RefineResultError( Sender->GUID() , RecipeID , EM_RefineResult_NoMoney );
			return;
		}
	}


	//�ˬd�ޯ�
	SkillValueTypeENUM SkillType = RecipeDB->Recipe.Request.Skill;

	if( SkillType != EM_SkillValueType_None )
	{
		float& SkillValue = Owner->TempData.Attr.Fin.SkillValue.Skill[ SkillType ];

		if(		RecipeDB->Recipe.Request.SkillLV > (int)SkillValue 
			|| 	Owner->TempData.Attr.MaxSkillValue.Skill[SkillType] == 0 )
		{
			SC_RefineResultError( Sender->GUID() , RecipeID , EM_RefineResult_SkillLvError );
			return;
		}
	}

	
	//Coldown�ˬd
	if( RecipeDB->Recipe.Coldown > 0 )
	{
		if( Owner->PlayerBaseData->RefineColdown.Size() == Owner->PlayerBaseData->RefineColdown.MaxSize() )
		{
			SC_RefineResultError( Sender->GUID() , RecipeID , EM_RefineResult_ColdownError );
			return;
		}

		for( int i = 0 ; i < Owner->PlayerBaseData->RefineColdown.Size() ; i++ )
		{
			if( Owner->PlayerBaseData->RefineColdown[i].RecipeID == RecipeID )
			{
				SC_RefineResultError( Sender->GUID() , RecipeID , EM_RefineResult_ColdownError );
				return;
			}
		}
	}
	
	//�ˬd�X������
	for( int i = 0 ; i < _MAX_RECIPE_SOURCE_COUNT_ ; i++ )
	{
		GameObjDbStructEx*	SourceDB = Global::GetObjDB( RecipeDB->Recipe.Source[i].OrgObjID );
		int	Count = RecipeDB->Recipe.Source[i].Count;

		if( SourceDB == NULL )
			continue;

		if( Owner->CalBodyItemCount( SourceDB->GUID ) < Count )
		{
			SC_RefineResultError( Sender->GUID() , RecipeID , EM_RefineResult_ResourceError );
			return;
		}
	}

	vector< ItemFieldStruct > ItemList;
	ItemFieldStruct Item;
	Item.Init();

	int DSkillLV = 0;

	if( SkillType != EM_SkillValueType_None )
	{
		float& SkillValue = Owner->TempData.Attr.Fin.SkillValue.Skill[ SkillType ];
		DSkillLV =  int(SkillValue) - RecipeDB->Recipe.Request.SkillLV ;
	}
	
	if( DSkillLV >= 20 )
		DSkillLV = 20;
	float AddNormalRate = float(DSkillLV) * 0.015f + 1.0f;
	float AddHQRate = float( Owner->GetHQExtraValue( SkillType ) ) / 100 + 1;
	//�X�����~
	//���~�@
	vector< int > RateList;
	for( int i = 0 ; i < 6 ; i++ )
	{
		RateList.push_back( RecipeDB->Recipe.Item1[i].Rate );
		//Total += RecipeDB->Recipe.Item1[i].Rate ;
	}
	//�@�몫�~
	RateList[0] = (int)( RateList[0] * AddNormalRate );
	//HQ�~
	RateList[1] = (int)( RateList[1] * AddHQRate );


	int Total = 0;
	for( int i = 0 ; i < 6 ; i++ )
	{
		Total += RateList[i];
	}


	if( Total < 100000 )
		Total = 100000;

	int	RandValue = Random( Total );
	
	for( int i = 0 ; i < (int) RateList.size() ; i++ )
	{
		int Rate = RateList[i];
		if( Rate > RandValue )
		{		
			Global::NewItemInit( Item , RecipeDB->Recipe.Item1[i].OrgObjID , 0 );
			Item.Count		= RecipeDB->Recipe.Item1[i].Count;
			float RandValue = float( rand() % 1250 );
			Item.OrgQuality = Item.Quality = 120 - int(sqrt( RandValue ) );
//			Item.OrgObjID	= RecipeDB->Recipe.Item1[i].OrgObjID;	
			Item.RuneVolume = 0;

			GameObjDbStructEx*	ItemDB = Global::GetObjDB(  Item.OrgObjID );
			
			if( ItemDB->IsItem() != false )
			{
				Item.Durable	= Item.Quality * ItemDB->Item.Durable;

				switch( i )
				{
				case 0:		//�@��
					break;
				case 1:		//HQ�~
					{
						int RandValue = rand()%1000;
						if( RandValue == 0 )
							Item.RuneVolume = 3;
						else if( RandValue < 10 )
							Item.RuneVolume = 2;
						else if( RandValue < 100 )
							Item.RuneVolume = 1;

						if( Item.RuneVolume > ItemDB->Item.HoleBase )
							Item.RuneVolume = ItemDB->Item.HoleBase;

						Item.Mode.IsHQ = true;
					}
					break;
				case 2:		//�Žg�@
					Item.RuneVolume = 1;
					break;
				case 3:		//�Žg�G
					Item.RuneVolume = 2;
					break;
				case 4:		//�Žg�T
					Item.RuneVolume = 3;
					break;
				case 5:		//�Žg�|
					Item.RuneVolume = 4;
					break;
				}

				ItemList.push_back( Item );
			}
			break;
		}
		RandValue -= Rate;
	}

	//�X�����~�G ~ �|
	for( int i = 0 ; i < 3 ; i++ )
	{
		RandValue = Random( 100000 );
		if( RandValue < int( RecipeDB->Recipe.ItemOther[i].Rate * AddNormalRate ) )
		{
			Global::NewItemInit( Item , RecipeDB->Recipe.ItemOther[i].OrgObjID , 0 );
			Item.Count		= RecipeDB->Recipe.ItemOther[i].Count;
			float RandValue = float( rand() % 1250 );
			Item.OrgQuality = Item.Quality = 120 - int(sqrt( RandValue ) );
//			Item.OrgObjID	= RecipeDB->Recipe.ItemOther[i].OrgObjID;

			GameObjDbStructEx*	ItemDB = Global::GetObjDB(  Item.OrgObjID );
			if( ItemDB->IsItem() != false )
			{
				Item.Durable	= Item.Quality * ItemDB->Item.Durable;
				ItemList.push_back( Item );
			}
		}
	}


	//�p�G�X������
	if( ItemList.size() == 0 )
	{
		//�R���Ҧ����ƪ��~
		for( int i = 0 ; i < _MAX_RECIPE_SOURCE_COUNT_ ; i++ )
		{
			GameObjDbStructEx*	SourceDB = Global::GetObjDB( RecipeDB->Recipe.Source[i].OrgObjID );
			int	Count = RecipeDB->Recipe.Source[i].Count;
			int DelCount = 0;

			if( SourceDB == NULL )
				continue;

			for( int i = 0 ; i < Count ; i++ )
			{
				if( rand() % 100 < SourceDB->Item.DisappearRate )
					DelCount ++;
			}
			if( DelCount == 0 )
				continue;

			Owner->DelBodyItem(  Count , SourceDB->GUID , EM_ActionType_CombineDestory );

			Item.Count		= DelCount;
			Item.OrgObjID	= SourceDB->GUID;

			ItemList.push_back( Item );
		}

		SC_RefineResultFailed( Owner->GUID() , RecipeID , ItemList  );

	}
	else 
	{//�X����
		//�R���Ҧ����ƪ��~
		for( int i = 0 ; i < _MAX_RECIPE_SOURCE_COUNT_ ; i++ )
		{
			GameObjDbStructEx*	SourceDB = Global::GetObjDB( RecipeDB->Recipe.Source[i].OrgObjID );
			int	Count = RecipeDB->Recipe.Source[i].Count;

			if( SourceDB == NULL || Count < 0 )
				continue;

			Owner->DelBodyItem( SourceDB->GUID , Count , EM_ActionType_CombineDestory );
		}

		//�������~
		for( int i = 0 ; i < (int)ItemList.size() ; i++ )
		{	
			//�ˬd�p�G�O�Z���Ψ���h�]�ͱ��W�h
			GameObjDbStructEx*	ItemObjDB = Global::GetObjDB( ItemList[i].OrgObjID );

			if( RecipeDB->Mode.VariableRare != false )
			{
				if(	  ( ItemObjDB->IsWeapon() && ItemObjDB->Item.WeaponPos != EM_WeaponPos_Ammo && ItemObjDB->Item.WeaponPos != EM_WeaponPos_Manufacture ) 
					||	ItemObjDB->IsArmor()	)
				{
					char Buf[512];
					sprintf( Buf , "SysRecipeItemUPgrade(%d)" , ItemObjDB->Rare );
					int UpgradeLv = 0;
					vector<MyVMValueStruct> retList;

					if( LUA_VMClass::PCallByStrArg( Buf , Owner->GUID() , Owner->GUID() , &retList , 1 ) )
					{
						if( retList.size() == 1 )
						{
							MyVMValueStruct& temp = retList[0];
							UpgradeLv = temp.Number_Int;
							//return temp.Number_Int;
						}
					}

					//ItemUpgradeRate
					/*
					int UpgradeLv = 0;
					for( UpgradeLv = 0 ; UpgradeLv < 2 ; UpgradeLv++  )
					{
						if( Random( 100000 ) > ItemUpgradeRate[ i + ItemObjDB->Rare ] )
							break;
					}
					*/
					
					int RareItemID = g_ObjectData->GetLifeSkillRareExID( ItemList[i].OrgObjID , UpgradeLv );
					if( RareItemID == 0 )
					{
						ItemList[i].Rare = UpgradeLv;	
					}
					else
					{
						ItemList[i].OrgObjID = ItemList[i].ImageObjectID = RareItemID;
					}
				}
			}

			Owner->GiveItem( ItemList[i] , EM_ActionType_CombineGet , NULL , "" );
		}

		// ����
		Owner->AddBodyMoney( ( RecipeDB->Recipe.CraftFee * -1 ), NULL, EM_ActionType_Buy , true );

		//�q�������X�����~
		SC_RefineResultOK( Owner->GUID() , RecipeID , ItemList  );

		//////////////////////////////////////////////////////////////////////////
		// Coldown �p��
		if( RecipeDB->Recipe.Coldown > 0  )
		{
			RefineBuffStruct TempRefineBuf;
			TempRefineBuf.RecipeID = RecipeID;
			TempRefineBuf.Coldown  = RecipeDB->Recipe.Coldown;
			Owner->PlayerBaseData->RefineColdown.Push_Back( TempRefineBuf );
			SC_RefineCooldown( Owner->GUID(), TempRefineBuf );
		}
		//////////////////////////////////////////////////////////////////////////

		if( Global::Ini()->IsHouseZone != false && Owner->PlayerTempData->VisitHouseDBID == Owner->PlayerBaseData->HouseDBID )
		{
			int	Addexp = 0;
			HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Owner->PlayerTempData->VisitHouseDBID );
			if( houseClass != NULL )
			{
				FurnitureAbilityStruct& AddAbility = houseClass->FurnitureAbility();
				switch( SkillType )
				{
				case EM_SkillValueType_BlackSmith:// ���K
					Addexp = (int)AddAbility.BlackSmithRate;
					break;
				case EM_SkillValueType_Carpenter:// ��u
					Addexp = (int)AddAbility.CarpenterRate;
					break;
				case EM_SkillValueType_MakeArmor:// �s��
					Addexp = (int)AddAbility.ArmorRate;
					break;
				case EM_SkillValueType_Tailor:// ���_
					Addexp = (int)AddAbility.TailorRate;
					break;
				case EM_SkillValueType_Cook:// �i��
					Addexp = (int)AddAbility.CookRate;
					break;
				case EM_SkillValueType_Alchemy:// �Ҫ�
					Addexp = (int)AddAbility.AlchemyRate;
					break;
				}
			}

			float fExpRate = 1;

			if( Owner->TempData.Attr.Mid.Body[ EM_WearEqType_CraftExpRate ] != 0 )
			{
				fExpRate = fExpRate + ( Owner->TempData.Attr.Mid.Body[ EM_WearEqType_CraftExpRate ] / (float)100 );					
			}

		

			Owner->AddSkillValue(  RecipeDB->Recipe.Request.SkillLV ,  SkillType , ( ( RecipeDB->Recipe.ExpRate *( 100 + Addexp ) / 100 ) * fExpRate ) );
		}
		else
		{
			float fExpRate = 1;

			if( Owner->TempData.Attr.Mid.Body[ EM_WearEqType_CraftExpRate ] != 0 )
			{
				fExpRate = fExpRate + ( Owner->TempData.Attr.Mid.Body[ EM_WearEqType_CraftExpRate ] / (float)100 );			
			}

			Owner->AddSkillValue(  RecipeDB->Recipe.Request.SkillLV ,  SkillType , ( RecipeDB->Recipe.ExpRate * fExpRate ) );
		}
		/*
		// �X���ޯ�g��ȧ޺�
		if( SkillValue <= _MAX_SKILL_LEVE_ && RoleDataEx::RefineExpTable.size() > SkillValue )
		{
			float SkillBaseExp = 1.0f / float( RoleDataEx::RefineExpTable [ int( SkillValue) ] );

			SkillValue += SkillBaseExp;
			if( SkillValue  > _MAX_SKILL_LEVE_ )
				SkillValue = _MAX_SKILL_LEVE_;

			Owner->Net_FixRoleValue( (RoleValueName_ENUM)(EM_RoleValue_SkillValue+SkillType) , SkillValue );
		}
		*/

	}

}
//-----------------------------------------------------------------------------
void NetSrv_RecipeChild::RC_LearnRecipe( BaseItemObject* Sender , int iRecipeID )
{
	int					iGUID			= Sender->GUID();
	GameObjDbStructEx*	pRecipe			= g_ObjectData->GetObj( iRecipeID );

	RoleDataEx*			pRole			= Sender->Role();

	if( pRole == NULL || pRecipe == NULL )
		return;

	if( pRecipe->IsRecipe() == false )
		return;

	if( pRecipe->Recipe.LearnFee < 0 )
		return;

	// �ˬd���L�ǹL
	if( pRole->CheckKeyItem( pRecipe->Recipe.Request.KeyItemID ) != false )
	{
		SC_LearnRecipeResult( iGUID, iRecipeID, EM_LearnRecipeResult_Learned );	// �w�g�Ƿ|�F
		return;
	}

	// �ˬd�H�������O�_����
	if( pRole->GetValue_Int( EM_RoleValue_Money ) < pRecipe->Recipe.LearnFee )
	{
		SC_LearnRecipeResult( iGUID, iRecipeID, EM_LearnRecipeResult_NoMoney );	// �w�g�Ƿ|�F
		return;
	}

	// �ˬd�ޯ��I�ư�����
	//if( pRole->BaseData.SkillValue.GetSkill( pRecipe->Recipe.Request.Skill ) < pRecipe->Recipe.Request.SkillLV )
	//{
	//	SC_LearnRecipeResult( iGUID, iRecipeID, EM_LearnRecipeResult_SkillTooLow );	// �w�g�Ƿ|�F
	//		return;
	//}


	// �@�� okay, ����, �����n���~, �]�w�Ƿ|
	pRole->AddBodyMoney( ( pRecipe->Recipe.LearnFee * -1 ), NULL, EM_ActionType_Buy , true );
	pRole->GiveItem( pRecipe->Recipe.Request.KeyItemID, 1, EM_ActionType_Buy, NULL , "" );

	SC_LearnRecipeResult( iGUID, iRecipeID, EM_LearnRecipeResult_OK );	// �w�g�Ƿ|�F
}