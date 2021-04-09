#pragma	warning (disable:4786)
#pragma warning (disable:4949)
#include "SkillComboClass.h"
#include "../baseitemobject/BaseItemObject.h"
#include "../magicproc/MagicProcess.h"
#include "../Global.h"

bool ComboInfoCmp (ComboBaseInfoStruct* A, ComboBaseInfoStruct* B) 
{
	return A->Count < B->Count;
};

SkillComboClass::SkillComboClass( )
{

}

SkillComboClass::~SkillComboClass( )
{
	Release();
}

void SkillComboClass::Init()
{
	static char* MagicName[ ] = {"地","水","火","風","光","暗"};

	_ComboRoot.ID = -1;
	for( unsigned i = 0 ; i < 6 ; i++ )
	{

		ComboBaseInfoStruct* Temp = NEW( ComboBaseInfoStruct );

		Temp->Note = MagicName[ i ];
		Temp->Count = 1;
		Temp->Type[0] = (MagicComboTypeENUM)i;
		_ComboRoot.NextCombo[ i ] = Temp;

		_ComboList.push_back( Temp );

	}

	for( unsigned i = 0 ; i < RoleDataEx::ComboInfo.size() ; i++ )
	{
		ComboBaseInfoStruct* Temp  = NEW( ComboBaseInfoStruct );

		ComboTable& T = RoleDataEx::ComboInfo[i];
		for( int j = 0 ; j < 5 ; j++ )
		{
			if( T.Type[j] < 0 || T.Type[j] >= 6 )
				break;
			Temp->Type[j] = (MagicComboTypeENUM)T.Type[j];
			Temp->Note = Temp->Note + MagicName[ T.Type[j] ];
			Temp->Count++;
		}
		Temp->BonusMagicID = T.Bonus;
		Temp->FinalMagicID = T.Final;

		_ComboList.push_back( Temp );
	}

	sort( _ComboList.begin() , _ComboList.end() , ComboInfoCmp );
	for( int i = 0 ; i < (int)_ComboList.size() ; i++ )
	{
		ComboBaseInfoStruct* NowComboInfo = _ComboList[i];

		NowComboInfo->ID = i;

		switch( NowComboInfo->Count )
		{
		case 1:
			break;
		case 2:
			{
				for( int j = 0 ; j < (int)_ComboList.size() ; j++ )
				{
					ComboBaseInfoStruct* OrgComboInfo = _ComboList[ j ];

					if( OrgComboInfo->Count != 1 )
						continue;					

					if( OrgComboInfo->Type[0] == NowComboInfo->Type[0] )
					{
						OrgComboInfo->NextCombo[ NowComboInfo->Type[1] ] = NowComboInfo;
					}
					else if( OrgComboInfo->Type[0] == NowComboInfo->Type[1] )
					{
						OrgComboInfo->NextCombo[ NowComboInfo->Type[0] ] = NowComboInfo;
					}					
				}
				break;
			}
		default:
			{
				for( int j = 0 ; j < (int)_ComboList.size() ; j++ )
				{
					ComboBaseInfoStruct* OrgComboInfo = _ComboList[ j ];
					if( OrgComboInfo->Count != NowComboInfo->Count -1  )
						continue;

					if( memcmp( &OrgComboInfo->Type , &NowComboInfo->Type , sizeof(int) * OrgComboInfo->Count ) == 0 )
					{
						int Type = NowComboInfo->Type[ OrgComboInfo->Count ];
						OrgComboInfo->NextCombo[ Type ] = NowComboInfo;
					}
				}
				break;
			}
		}

	}
}
void SkillComboClass::Release()
{
	for( int i = 0 ; i < (int)_ComboList.size() ; i++ )
		delete _ComboList[i];

	_ComboList.clear();
	_ComboRoot.Init();
}


struct TempComboDelayStruct
{
	int				OwnerID;
	MagicProcInfo	MagicInfo;
};

int      SkillComboClass::_ComboDelayProc( SchedularInfo* Obj , void* InputClass )
{
	TempComboDelayStruct* TempData = (TempComboDelayStruct*)InputClass;
	RoleDataEx* Owner = Global::GetRoleDataByGUID( TempData->OwnerID );

	if( Owner != NULL )
	{
		MagicProcessClass::SysSpellMagic( Owner , &TempData->MagicInfo , 5 );
	}

	delete TempData;
	return 0;
}

//連攜前致處理
void		SkillComboClass::CheckSkillCombo_PreProc( RoleDataEx* Owner , int TargetID , MagicComboTypeENUM Type )
{
	RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );
	if( Target == NULL )
		return;

	//每個階段所能融忍的combo時間微調
	static int AdjustBeginTime[]	= { -1 , 1000 , 1000 , 1000 , 1000 , 1000 , 1000 , 1000 };
	static int AdjustEndTime[]		= { -1 , 5000 , 4250 , 3600 , 2600 , 2200 , 1800 , 1800 };
	//static int AdjustBeginTime[]	= { -1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 };
	//static int AdjustEndTime[]		= { -1 , 100000 , 100000 , 100000 , 100000 , 100000 , 100000 , 100000 };
	int ComboID = Target->TempData.SkillComboID;
	int	Time	= Target->TempData.SkillComboTime;

	Target->TempData.SkillComboTime = RoleDataEx::G_SysTime;
	//Target->TempData.SkillComboID = -1;

	map< int , ComboBaseInfoStruct* >::iterator Iter;
	if( ComboID == -1 )
	{
		Iter = _ComboRoot.NextCombo.find( Type );
		if( Iter != _ComboRoot.NextCombo.end() )
		{
			Target->TempData.SkillComboID = Iter->second->ID;
			Target->TempData.SkillComboPower = 0;
		}
		return;
	}

	if( (unsigned)ComboID > _ComboList.size() )
	{
		Target->TempData.SkillComboID = -1;
		Target->TempData.SkillComboPower = 0;
		return;
	}

	ComboBaseInfoStruct* NowComboInfo = _ComboList[ ComboID ];

	//檢查時間是否ok
	int DTime = RoleDataEx::G_SysTime - Time;
	if( DTime  < AdjustBeginTime[ NowComboInfo->Count ] || DTime  > AdjustEndTime[ NowComboInfo->Count ] )
	{
		Iter = _ComboRoot.NextCombo.find( Type );
		if( Iter != _ComboRoot.NextCombo.end() )
		{
			Target->TempData.SkillComboID = Iter->second->ID;
			Target->TempData.SkillComboPower = 0;
		}
		else
		{
			Target->TempData.SkillComboID = -1;
			Target->TempData.SkillComboPower = 0;
		}
		return;
	}
/*
	if( Type == EM_MagicComboType_Start )
	{

		MagicInfo.MagicCollectID	= NowComboInfo->FinalMagicID;	//法術
		MagicInfo.MagicLv			= Owner->TempData.SkillComboPower * -1;

		TempComboDelayStruct* TempData = NEW(TempComboDelayStruct);
		TempData->OwnerID = Owner->GUID();
		TempData->MagicInfo = MagicInfo;
		Global::Schedular()->Push( _ComboDelayProc , 500 , TempData , NULL );
		//MagicProcessClass::SysSpellMagic( Owner , &MagicInfo );
		Target->TempData.SkillComboID = -1;
		Target->TempData.SkillComboPower = 0;
		return;
	}

	Iter = NowComboInfo->NextCombo.find( Type );
	if( Iter == NowComboInfo->NextCombo.end() )
		return;


	MagicInfo.MagicCollectID	= Iter->second->BonusMagicID;	//法術
	MagicInfo.MagicLv			= Target->TempData.SkillComboPower * -1;

	TempComboDelayStruct* TempData = NEW(TempComboDelayStruct);
	TempData->OwnerID = Owner->GUID();
	TempData->MagicInfo = MagicInfo;
	Global::Schedular()->Push( _ComboDelayProc , 1000 , TempData , NULL );
	//MagicProcessClass::SysSpellMagic( Owner , &MagicInfo );

	Target->TempData.SkillComboID = Iter->second->ID;
	*/
	return ;

}

//檢查連攜是否成立
//回傳連攜的結果
void	SkillComboClass::CheckSkillCombo( RoleDataEx* Owner , int TargetID , MagicComboTypeENUM Type  )
{
	RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );
	if( Target == NULL )
		return;
/*
	//每個階段所能融忍的combo時間微調
	//static int AdjustBeginTime[]	= { -1 , 2000 , 1800 , 1600 , 1500 , 1400 , 1400 , 1400 };
	//static int AdjustEndTime[]		= { -1 , 4000 , 3200 , 2600 , 2200 , 1900 , 1900 , 1900 };
	static int AdjustBeginTime[]	= { -1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 };
	static int AdjustEndTime[]		= { -1 , 100000 , 100000 , 100000 , 100000 , 100000 , 100000 , 100000 };
	int ComboID = Target->TempData.SkillComboID;
	int	Time	= Target->TempData.SkillComboTime;

	Target->TempData.SkillComboTime = RoleDataEx::G_SysTime;
	Target->TempData.SkillComboID = -1;

	MagicProcInfo  MagicInfo;
	MagicInfo.State		= EM_MAGIC_PROC_STATE_PERPARE;
	MagicInfo.TargetID	= TargetID;				//目標


	map< int , ComboBaseInfoStruct* >::iterator Iter;
	if( ComboID == -1 )
	{
		Iter = _ComboRoot.NextCombo.find( Type );
		if( Iter != _ComboRoot.NextCombo.end() )
		{
			Target->TempData.SkillComboID = Iter->second->ID;
			Target->TempData.SkillComboPower = 0;
		}
		return;
	}

	if( (unsigned)ComboID > _ComboList.size() )
	{
		Target->TempData.SkillComboID = -1;
		Target->TempData.SkillComboPower = 0;
		return;
	}

	ComboBaseInfoStruct* NowComboInfo = _ComboList[ ComboID ];

	//檢查時間是否ok
	int DTime = RoleDataEx::G_SysTime - Time;
	if( DTime  < AdjustBeginTime[ NowComboInfo->Count ] || DTime  > AdjustEndTime[ NowComboInfo->Count ] )
	{
		Iter = _ComboRoot.NextCombo.find( Type );
		if( Iter != _ComboRoot.NextCombo.end() )
		{
			Target->TempData.SkillComboID = Iter->second->ID;
			Target->TempData.SkillComboPower = 0;
		}
		else
		{
			Target->TempData.SkillComboID = -1;
			Target->TempData.SkillComboPower = 0;
		}
		return;
	}
*/
	int ComboID = Target->TempData.SkillComboID;

	if( ComboID == -1 )
		return;

	ComboBaseInfoStruct* NowComboInfo = _ComboList[ ComboID ];
	map< int , ComboBaseInfoStruct* >::iterator Iter;

	MagicProcInfo  MagicInfo;
	MagicInfo.State		= EM_MAGIC_PROC_STATE_PERPARE;
	MagicInfo.TargetID	= TargetID;				//目標

	if( Type == EM_MagicComboType_Start )
	{
		
		MagicInfo.MagicCollectID	= NowComboInfo->FinalMagicID;	//法術
		MagicInfo.MagicLv			= Target->TempData.SkillComboPower * -1;

		TempComboDelayStruct* TempData = NEW(TempComboDelayStruct);
		TempData->OwnerID = Owner->GUID();
		TempData->MagicInfo = MagicInfo;
		Global::Schedular()->Push( _ComboDelayProc , 1000 , TempData , NULL );
		//MagicProcessClass::SysSpellMagic( Owner , &MagicInfo );
		Target->TempData.SkillComboID = -1;
		Target->TempData.SkillComboPower = 0;
		return;
	}

	Iter = NowComboInfo->NextCombo.find( Type );
	if( Iter == NowComboInfo->NextCombo.end() )
		return;


	MagicInfo.MagicCollectID	= Iter->second->BonusMagicID;	//法術
	MagicInfo.MagicLv			= Target->TempData.SkillComboPower * -1;

	TempComboDelayStruct* TempData = NEW(TempComboDelayStruct);
	TempData->OwnerID = Owner->GUID();
	TempData->MagicInfo = MagicInfo;
	Global::Schedular()->Push( _ComboDelayProc , 1000 , TempData , NULL );
	//MagicProcessClass::SysSpellMagic( Owner , &MagicInfo );

	Target->TempData.SkillComboID = Iter->second->ID;
	return ;
}

//取得此Combo效果帶來的能量累積倍率
float		SkillComboClass::GetPowerRate( int ComboID )
{
	return 0.5f;
//	static float PowerRate[] = { 0.2f , 0.4f , 0.5f , 0.7f , 0.9f , 1.0f , 1.0f , 1.0f };
//	if( unsigned( ComboID ) >= _ComboList.size() )
//		return 0.5;

//	ComboBaseInfoStruct* NowComboInfo = _ComboList[ ComboID ];
//	return PowerRate[ NowComboInfo->Count ];

}

int		SkillComboClass::CalComboPower( RoleDataEx* Target , int AddPower )
{

	if( AddPower < 0 )
		Target->TempData.SkillComboPower = int( Target->TempData.SkillComboPower * 0.5 + AddPower *  GetPowerRate( Target->TempData.SkillComboID ) );

	return Target->TempData.SkillComboPower;
}