#include <Math.h>
#include "MagicProcess.h"
#include "../../NetWalker_Member/NetWakerGSrvInc.h"
#include "../Combo/SkillComboClass.h"

#define _DEF_MAX_MAGIC_TYPE_	256
//----------------------------------------------------------------------------------------
//靜態資料
//----------------------------------------------------------------------------------------
vector< MagicFunction >				MagicProcessClass::m_Magic;
vector < MagicTargetInfoStruct >    MagicProcessClass::m_TargetList;
vector < MagicTargetInfoStruct* >	MagicProcessClass::m_ProcTargetList;		//目前處理有效目標
//vector < MagicTargetInfoStruct >	MagicProcessClass::m_TargetListMemory;		//先配置出的處理 TargetList 與 ProcTargetList 的記憶體
int									MagicProcessClass::m_ProcMagicID = -1;
vector< MagicBaseCheckFunction >	MagicProcessClass::m_CheckMagicBase;		//檢查法術元素是否可施展
bool								MagicProcessClass::m_IsMissErase = false;	//如果法術沒命中則把此目標移除
int									MagicProcessClass::m_SysMagicCount = 0;		//系統法數目前數量
unsigned							MagicProcessClass::m_DisableSysMagicTime = 0;//停止系統法術一秒鐘
map<int,int>						MagicProcessClass::m_SysMagicIDCount;		//施展各種法處的次數
vector< int >						MagicProcessClass::m_ShootTarget;
//----------------------------------------------------------------------------------------
void MagicProcessClass::Init()
{
	int i;
	m_Magic.push_back( Magic00_AddHP		 );
	m_Magic.push_back( Magic01_Assist		 );
	m_Magic.push_back( Magic02_Teleport		 );
	m_Magic.push_back( Magic03_CreatePet	 );
	m_Magic.push_back( Magic04_SummonItem	 );
	m_Magic.push_back( Magic05_Steal		 );
	m_Magic.push_back( Magic06_ItemRunPlot	 );
	m_Magic.push_back( Magic07_RunPlot		 );
	m_Magic.push_back( Magic08_Raise		 );

	for( i = (int)m_Magic.size() ; i < _DEF_MAX_MAGIC_TYPE_ ; i ++ )
	{
		m_Magic.push_back( Magic_Empty );
	}
	/*
	m_Magic[ 0 ]		=  Magic00_AddHP;		
	m_Magic[ 1 ]		=  Magic01_Assist;		
	m_Magic[ 2 ]		=  Magic02_Teleport;
	m_Magic[ 3 ]		=  Magic03_CreatePet;	
	m_Magic[ 4 ]		=  Magic04_SummonItem;	
	m_Magic[ 5 ]		=  Magic05_Steal;		
	m_Magic[ 6 ]		=  Magic06_ItemRunPlot;	
	m_Magic[ 7 ]		=  Magic07_RunPlot;		
	m_Magic[ 8 ]		=  Magic08_Raise;		
	*/

	m_CheckMagicBase.push_back( Check_None				);
	m_CheckMagicBase.push_back( Check_Rand				);
	m_CheckMagicBase.push_back( Check_MyEQ				);
	m_CheckMagicBase.push_back( Check_TargetEQ			);
	m_CheckMagicBase.push_back( Check_MyEqType			);
	m_CheckMagicBase.push_back( Check_TargetEqType		);
	m_CheckMagicBase.push_back( Check_MyItem			);
	m_CheckMagicBase.push_back( Check_TargetItem		);
	m_CheckMagicBase.push_back( Check_MyBuff			);
	m_CheckMagicBase.push_back( Check_TargetBuff		);
	m_CheckMagicBase.push_back( Check_MyPos				);
	m_CheckMagicBase.push_back( Check_TargetPos			);
	m_CheckMagicBase.push_back( Check_MyAttackType		);
	m_CheckMagicBase.push_back( Check_TargetAttackType	);
	m_CheckMagicBase.push_back( Check_TargetRace		);
	m_CheckMagicBase.push_back( Check_MyHp				);
	m_CheckMagicBase.push_back( Check_Time				);
	m_CheckMagicBase.push_back( Check_Weather			);
	m_CheckMagicBase.push_back( Check_MyBuffGroup		);
	m_CheckMagicBase.push_back( Check_TargetBuffGroup	);
	m_CheckMagicBase.push_back( Check_MySkill			);
	m_CheckMagicBase.push_back( Check_LV				);

	for( i = (int)m_CheckMagicBase.size() ; i < _DEF_MAX_MAGIC_TYPE_ ; i ++ )
	{
		m_CheckMagicBase.push_back( Check_None );
	}
/*
	for( int i = 0 ; i < 200 ; i++ )
	{
		MagicTargetInfoStruct Temp;
		m_TargetListMemory.push_back( Temp );
	}
	*/
}
//----------------------------------------------------------------------------------------
//加入一個法術(以裝備模式加上)
bool MagicProcessClass::WearMagic( RoleDataEx* Owner , int MagicID )
{

	return true;
}
//----------------------------------------------------------------------------------------
//移除一個法術(以裝備模式移除)
bool MagicProcessClass::DropMagic( RoleDataEx* Owner , int MagicID )
{
	return true;
}
//----------------------------------------------------------------------------------------
bool MagicProcessClass::SpellMagic( int OwnerID , int TargetID , float TargetX , float TargetY , float TargetZ , int MagicID , int MagicLv , int ShootAngle )
{
	RoleDataEx*		    Owner  = Global::GetRoleDataByGUID( OwnerID );
	RoleDataEx*		    Target;  
	if( Owner == NULL || Owner->IsDead()  )
	{
		//Owner->Net_GameMsgEvent( EM_GameMessageEvent_MagicError_MagicIDError );
		NetSrv_MagicChild::S_BeginSpellFailed( OwnerID , MagicID , -1 );		
		return false;
	}
	if( Owner->TempData.Attr.Action.Sit != false )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_MagicError_IsSit );
		NetSrv_MagicChild::S_BeginSpellFailed( OwnerID , MagicID , -1 );
		return false;
	}

	GameObjDbStructEx*	OrgDB  = Global::GetObjDB( MagicID );
	if( OrgDB->IsMagicCollect() == false )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_MagicError_MagicIDError );
		NetSrv_MagicChild::S_BeginSpellFailed( OwnerID , MagicID , -1 );
		return false;
	}
/*
	//如果在騎馬中檢查是否可以使用 
	if( Owner->TempData.Attr.Effect.Ride != false && OrgDB->MagicCol.IsRideEnable == false )
	{
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_MagicError_Ride );
		NetSrv_MagicChild::S_BeginSpellFailed( OwnerID , MagicID , -1 );
		return false;
	}
*/
	if( Owner->IsSpell() )
	{
		Owner->Net_GameMsgEvent(  EM_GameMessageEvent_MagicError_OnSpell );
		NetSrv_MagicChild::S_BeginSpellFailed( OwnerID , MagicID , -1 );
		return false;
	}

	if( Owner->Level() < OrgDB->MagicCol.LimitLv )
	{
		Owner->Net_GameMsgEvent(  EM_GameMessageEvent_MagicError_Request );
		NetSrv_MagicChild::S_BeginSpellFailed( OwnerID , MagicID , -1 );
		return false;
	}

	//設定序號
	int SerialID = Owner->TempData.Magic.SetSerialID();


	if(		OrgDB->MagicCol.TargetType == EM_Releation_Self 
		||  OrgDB->MagicCol.TargetType == EM_Releation_Locatoin )
	{
		TargetID = OwnerID;
		Target = Owner;
	}
	else
	{
		Target = Global::GetRoleDataByGUID( TargetID );
		if( Target == NULL )
			Target = Owner;
		else if( Owner->CheckRelation( Target , OrgDB->MagicCol.TargetType , OrgDB->MagicCol.IsIgnoreTargetDead ) == false )
		{
			TargetID = OwnerID;
			Target = Owner;
		}
	}

	if( OrgDB->MagicCol.TargetType != EM_Releation_Locatoin )
	{
		if( Target->IsPlayer() )
		{
			TargetX = Target->TempData.Move.CliX;
			TargetY = Target->TempData.Move.CliY;
			TargetZ = Target->TempData.Move.CliZ;
		}
		else
		{
			TargetX = Target->X();
			TargetY = Target->Y();
			TargetZ = Target->Z();
		}
	}

	//利用劇情檢查
	vector<MyVMValueStruct> RetList;
	if( strlen( OrgDB->MagicCol.CheckUseScript ) != 0 )
	{
		bool Ret = true;
		if( LUA_VMClass::PCallByStrArg( OrgDB->MagicCol.CheckUseScript , Owner->GUID() , TargetID , &RetList , 1 ) )
		{
			if( RetList.size() != 1 )
			{
				Ret = false;
			}
			else
			{
				MyVMValueStruct& Temp = RetList[0];
				if( Temp.Flag != true )
					Ret = false;
			}

		}
		else 
			Ret = false;

		if( Ret == false )
		{
			NetSrv_MagicChild::S_BeginSpellFailed( OwnerID , MagicID , -1 );
			return false;
		}
	}

	if( Owner->IsPlayer() && Owner->Y() - TargetY > 50 && Owner->TempData.Move.MoveType != EM_ClientMoveType_Normal )
	{		
		NetSrv_MagicChild::S_BeginSpellFailed( OwnerID , MagicID , -1 );
		return false;
	}


	bool Ret = MagicProcessClass::SpellProc_BeginSpell( OwnerID , TargetID , TargetX , TargetY , TargetZ , MagicID , MagicLv );

	if( Ret != false )
	{
		/*
		Owner->BaseData.Coldown.AllMagic = 1;
		if( Owner->BaseData.Coldown.NeedUpdatePos.Find( DEF_COLDOWN_ALL_POS ) == -1 )
			Owner->BaseData.Coldown.NeedUpdatePos.Push_Back( DEF_COLDOWN_ALL_POS );
			*/

		//通知Client可以施法
		NetSrv_MagicChild::S_BeginSpellOK( OwnerID , MagicID , SerialID , Owner->TempData.Magic.TotalSpellTime , ShootAngle );
		//通知周圍Client準備施法
		NetSrv_MagicChild::BeginSpellRange( OwnerID , TargetID , TargetX , TargetY , TargetZ , MagicID , SerialID , Owner->TempData.Magic.TotalSpellTime );

		if( Owner->IsNPC() && Owner != Target && Owner->BaseData.Mode.DisableRotate == false )
			Owner->BaseData.Pos.Dir = Owner->CalDir( Target );

		if( Owner->TempData.Magic.TotalSpellTime  == 0 )
		{
//			for( int i = 0 ; i < 3 ; i++ )
//				MagicProcessClass::Process( Owner , &Owner->TempData.Magic );
		}

		//如果在騎馬中檢查是否可以使用 
		if( Owner->TempData.Attr.Effect.Ride != false && OrgDB->MagicCol.IsRideEnable == false )
		{
			//清除騎乘的Buf
			for( int i = 0 ; i < Owner->BaseData.Buff.Size() ; i++ )
			{
				GameObjDbStruct* MagicBase = Owner->BaseData.Buff[i].Magic;
				if( MagicBase == NULL || MagicBase->MagicBase.Effect.Ride != false )
				{
					Owner->BaseData.Buff[i].Time = 0;
					Owner->BaseData.Buff[i].IsErase = true;
				}
			}
		}

		if( OrgDB->MagicCol.Flag.SpellStopMove )
		{
			Owner->StopMoveNow();
		}
		
	}
	else
	{
		//通知不可施法
		NetSrv_MagicChild::S_BeginSpellFailed( OwnerID , MagicID , -1 );
	}

	return Ret;

}

//----------------------------------------------------------------------------------------
//施法處裡(準備施法)
bool MagicProcessClass::SpellProc_BeginSpell( int OwnerID , int TargetID , float TargetX , float TargetY , float TargetZ , int MagicID , int MagicLv  )
{

	MagicProcInfo*		MagicProc;
	GameObjDbStructEx*	OrgDB  = Global::GetObjDB( MagicID );

	BaseItemObject* OwnerObj = Global::GetObj( OwnerID );
	if(  OwnerObj == NULL )
		return false;

	//RoleDataEx*		    Owner  = Global::GetRoleDataByGUID( OwnerID );
	RoleDataEx*		    Owner  = OwnerObj->Role();
	RoleDataEx*		    Target = Global::GetRoleDataByGUID( TargetID );

	if( OrgDB->IsMagicCollect() == false )
		return false;

	if( OrgDB->MagicCol.EffectType == EM_Magic_Eq )
		return false;

	if( Owner == NULL )
		return false;

	if( Target == NULL )
		Target = Owner;

	MagicProc = &Owner->TempData.Magic;
	//--------------------------------------------------------------------------------------------------

	if( MagicProc->State != EM_MAGIC_PROC_STATE_NORMAL )
		return false;

	//檢查對此目標是否可以施法
	GameMessageEventENUM Ret = Owner->CheckCanBeginSpell(  Target , TargetX , TargetY , TargetZ , OrgDB , MagicLv );

	if( Ret != EM_GameMessageEvent_OK )
	{
		Owner->Net_GameMsgEvent( Ret );
		return false;
	}

	float X = Owner->X();
	float Y = Owner->Y();
	float Z = Owner->Z();
	if( Owner->IsPlayer() )
	{
		X = Owner->TempData.Move.CliX;
		Y = Owner->TempData.Move.CliY;
		Z = Owner->TempData.Move.CliZ;
	}

	if( OrgDB->MagicCol.Flag.Ignore_Obstruct == false )
	{
		if( OwnerObj->Path()->CheckLine_Magic( X , Y , Z , TargetX , TargetY , TargetZ ) == false )
		{
			RoleDataEx::LastProcErr = EM_RoleErrorMsg_SpellExSkill_TargetLineNotClear;
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_MagicError_TargetLineNotClear );
			return false;
		}
	}

	float		SpellTime = 0;
	
	if( OrgDB->MagicCol.Flag.ReferenceWeapon_Bow_SpellTime )
	{
		SpellTime = Owner->TempData.Attr.Fin.Bow_ATKSpeed * 100;
	}
	else if( OrgDB->MagicCol.Flag.ReferenceWeapon_Throw_SpellTime )
	{
		SpellTime = Owner->TempData.Attr.Fin.Throw_ATKSpeed * 100;
	}

	if( OrgDB->MagicCol.SpellTime != 0 )
	{
		SpellTime = float( OrgDB->MagicCol.SpellTime * 1000 ) + SpellTime;
		SpellTime  = SpellTime * ( Owner->TempData.Attr.MagicSpellSpeed() );
		if( SpellTime < 500 )
			SpellTime = 500;
	}

	if( OrgDB->MagicCol.IsDisableSpellBuffEffect == false && Owner->TempData.Attr.Effect.MagicSpell_ZeroTime )
	{
		SpellTime = 0;
	}


	MagicProc->InitMagicEnabled();
	MagicProc->MagicCollectID	= MagicID;
	MagicProc->ProcTime			= int( SpellTime ) + RoleDataEx::G_SysTime;
	MagicProc->State			= EM_MAGIC_PROC_STATE_PERPARE;
	MagicProc->TargetID			= TargetID;
	MagicProc->BeginX			= Owner->X();
	MagicProc->BeginY			= Owner->Y();
	MagicProc->BeginZ			= Owner->Z();
	MagicProc->IsMove			= false;
	MagicProc->TargetX  		= TargetX;
	MagicProc->TargetY  		= TargetY;
	MagicProc->TargetZ  		= TargetZ;
	MagicProc->SpellCount    	= OrgDB->MagicCol.SpellCount;
	MagicProc->NextSpellTime 	= int( OrgDB->MagicCol.NextSpellTime*1000 );
	MagicProc->IsSpellCheckOK	= true;
	MagicProc->IsSuccess		= false;
	MagicProc->MagicLv			= MagicLv;
	MagicProc->IsSystemMagic	= false;
	MagicProc->IsPayCost		= false;
	MagicProc->BaseShootDelayTime = 0;

	MagicClearTimeStruct ClearEvent;
	ClearEvent._Value = 0;

	if( OrgDB->MagicCol.Flag.IgnoreSpellMagicClear == false )
	{
		if( OrgDB->MagicCol.EffectType == EM_Magic_Magic )
			ClearEvent.Spell_Magic = true;
		else if( OrgDB->MagicCol.EffectType == EM_Magic_Phy )
			ClearEvent.Spell_Phy = true;
		ClearEvent.Spell = true;
		Owner->ClearBuffByEvent( ClearEvent );
	}


	if( OrgDB->MagicCol.SpellCount != 1 )
	{
		MagicProc->TotalSpellTime = int( SpellTime + (MagicProc->SpellCount-1) * ( MagicProc->NextSpellTime ));
	}
	else
	{
		MagicProc->TotalSpellTime = int( SpellTime );
	}
	
	
	return true;
}
//----------------------------------------------------------------------------------------	
void MagicProcessClass::RangeSelectCircle   ( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList )
{
	RoleDataEx* Other;
	int i;

	if( (*SearchList).size() == 0 )
		return;

	for( i = 0 ; i < (int)(*SearchList).size() ; i++ )
	{
		Other = (RoleDataEx*)(*SearchList)[i].Data ;

		MagicTargetInfoStruct Temp;// = &(m_TargetListMemory[i]);
		Temp.Role = Other;
		Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;

		m_TargetList.push_back( Temp );

		if( m_TargetList.size() >= 100 )
			break;

		if( (int)m_TargetList.size() >= OrgDB->MagicCol.EffectCount )
			break;
	}
}
void MagicProcessClass::RangeSelectLine1     ( RoleDataEx* Owner , MagicProcInfo* MagicProc  , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList )
{
	int  i , j;
	vector<float> ArrX;
	vector<float> ArrZ;

	if( (*SearchList).size() == 0 )
		return;

	//角色方向向量
	double Dir; 
	Dir = ( 360 - Owner->BaseData.Pos.Dir )/57.2958;

	double x = cos( Dir );
	double z = sin( Dir );

	if( OrgDB->MagicCol.EffectRange < 10 )
		OrgDB->MagicCol.EffectRange = 10;

	//計算子彈路徑
	for( j = 10 ; j <= OrgDB->MagicCol.EffectRange+20 ; j+= 20 )
	{
		ArrX.push_back( float( j* x + MagicProc->TargetX ) );
		ArrZ.push_back( float( j* z + MagicProc->TargetZ ) );
	}



	RoleDataEx* Other;
	for( i = 0 ; i < (int)(*SearchList).size() ; i++ )
	{
		Other = (RoleDataEx*)(*SearchList)[i].Data ;

		for( j = 0 ; j < (int)ArrX.size() ; j++ )
		{
			if( Other->Length( ArrX[j] , MagicProc->TargetY , ArrZ[j] ) < 15 + Other->TempData.BoundRadiusY )
				break;
		}
		if( j >= (int)ArrX.size() )
			continue;

		MagicTargetInfoStruct Temp;// = &(m_TargetListMemory[i]);
		Temp.Role = Other;
		Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;		

		m_TargetList.push_back( Temp );

		if( m_TargetList.size() >= 100 )
			break;

		if( (int)m_TargetList.size() >= OrgDB->MagicCol.EffectCount )
			break;
	}
}
//直線( 施法者 -> 目標 )
void MagicProcessClass::RangeSelectLine2     ( RoleDataEx* Owner  , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList )
{
	int  i , j;
	vector<float> ArrX;
	vector<float> ArrZ;
	RoleDataEx* Other;
	if( (*SearchList).size() == 0 )
		return;

	//重新排序
	for( i = 0 ; i < (int)(*SearchList).size() ; i++ )
	{
		Other = (RoleDataEx*)(*SearchList)[i].Data ;
		(*SearchList)[i].Value = Owner->Length3D( Other );
	}

	sort( (*SearchList).begin() , (*SearchList).end() );

	float Dx = ( MagicProc->TargetX - MagicProc->BeginX );
	float Dz = ( MagicProc->TargetZ - MagicProc->BeginZ );

	float Distance = sqrt( Dx * Dx + Dz * Dz ) ;
	Dx /= Distance;
	Dz /= Distance;

	int	EffectRange = int( Distance )+20;
	if( EffectRange < 10 )
		EffectRange = 10;

	//計算子彈路徑
	for( j = 10 ; j <= EffectRange  ; j+= 20 )
	{
		ArrX.push_back( float( j* Dx + MagicProc->BeginX ) );
		ArrZ.push_back( float( j* Dz + MagicProc->BeginZ ) );
	}


	
	for( i = 0 ; i < (int)(*SearchList).size() ; i++ )
	{
		Other = (RoleDataEx*)(*SearchList)[i].Data ;

		for( j = 0 ; j < (int)ArrX.size() ; j++ )
		{
			if( Other->Length( ArrX[j] , MagicProc->TargetY , ArrZ[j] ) < 15 + Other->TempData.BoundRadiusY )
				break;
		}
		if( j >= (int)ArrX.size() )
			continue;

		MagicTargetInfoStruct Temp;// = &(m_TargetListMemory[i]);
		Temp.Role = Other;
		Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;		

		m_TargetList.push_back( Temp );

		if( m_TargetList.size() >= 100 )
			break;

		if( (int)m_TargetList.size() >= OrgDB->MagicCol.EffectCount )
			break;
	}
}
//直線( 施法者 -> 目標 )
void MagicProcessClass::RangeSelectLine3     ( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList )
{
	int  i , j;
	vector<float> ArrX;
	vector<float> ArrZ;
	RoleDataEx* Other;

	if( (*SearchList).size() == 0 )
		return;

	//重新排序
	for( i = 0 ; i < (int)(*SearchList).size() ; i++ )
	{
		Other = (RoleDataEx*)(*SearchList)[i].Data ;
		(*SearchList)[i].Value = Owner->Length3D( Other );
	}

	sort( (*SearchList).begin() , (*SearchList).end() );


	float Dx = ( MagicProc->TargetX - MagicProc->BeginX );
	float Dz = ( MagicProc->TargetZ - MagicProc->BeginZ );

	float Distance = sqrt( Dx * Dx + Dz * Dz ) ;
	Dx /= Distance;
	Dz /= Distance;

	//計算子彈路徑
	if( OrgDB->MagicCol.EffectRange < 20 )
		OrgDB->MagicCol.EffectRange = 10;
	for( j = 10 ; j <= OrgDB->MagicCol.EffectRange+20  ; j+= 20 )
	{
		ArrX.push_back( float( j* Dx + MagicProc->BeginX ) );
		ArrZ.push_back( float( j* Dz + MagicProc->BeginZ ) );
	}



	for( i = 0 ; i < (int)(*SearchList).size() ; i++ )
	{
		Other = (RoleDataEx*)(*SearchList)[i].Data ;

		for( j = 0 ; j < (int)ArrX.size() ; j++ )
		{
			if( Other->Length( ArrX[j] , MagicProc->TargetY , ArrZ[j] ) < 15 + Other->TempData.BoundRadiusY )
				break;
		}
		if( j >= (int)ArrX.size() )
			continue;

		MagicTargetInfoStruct Temp;// = &(m_TargetListMemory[i]);
		Temp.Role = Other;
		Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;		

		m_TargetList.push_back( Temp );

		if( m_TargetList.size() >= 100 )
			break;

		if( (int)m_TargetList.size() >= OrgDB->MagicCol.EffectCount )
			break;
	}
}
void MagicProcessClass::RangeSelectSector   ( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList )
{
	RoleDataEx* Other;
	int i;

	RoleDataEx* Target = Global::GetRoleDataByGUID( MagicProc->TargetID );
	if( Target == NULL )
		return;

	if( (*SearchList).size() == 0 )
		return;

	for( i = 0 ; i < (int)(*SearchList).size() ; i++ )
	{
		Other = (RoleDataEx*)(*SearchList)[i].Data ;

		//角度
		if( Target->CalAngle( Other) > 30 )
			continue;

		MagicTargetInfoStruct Temp;// = &(m_TargetListMemory[i]);
		Temp.Role = Other;
		Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;

		m_TargetList.push_back( Temp );

		if( m_TargetList.size() >= 100 )
			break;

		if( (int)m_TargetList.size() >= OrgDB->MagicCol.EffectCount  + Other->TempData.BoundRadiusY )
			break;
	}
}
void MagicProcessClass::RangeSelectLightning1( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList )
{

	RoleDataEx* Other;
	int i;
	if( (*SearchList).size() == 0 )
		return;

	MagicTargetInfoStruct Temp;// = &(m_TargetListMemory[0]);

	Temp.Role = (RoleDataEx*)(*SearchList)[0].Data ;
	Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;

	m_TargetList.push_back( Temp );

	SearchList->erase( SearchList->begin( ) );

	for( i = 1 ; i < OrgDB->MagicCol.EffectCount ; i++ )
	{
		if( SearchList->size() == 0 )
			break;

		int RandValue = rand() % int( SearchList->size() );

		Other = (RoleDataEx*)(*SearchList)[ RandValue  ].Data ;

		//Temp = &(m_TargetListMemory[i]);

		Temp.Role = Other;
		Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;

		m_TargetList.push_back( Temp );

		SearchList->erase( SearchList->begin() + RandValue );
	}
}

void MagicProcessClass::RangeSelectLightning2( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList )
{

	RoleDataEx* Other;
	int i;
	if( (*SearchList).size() == 0 )
		return;

	MagicTargetInfoStruct Temp;// = &(m_TargetListMemory[0]);

	Temp.Role = (RoleDataEx*)(*SearchList)[0].Data ;
	Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;

	m_TargetList.push_back( Temp );


	for( i = 1 ; i < OrgDB->MagicCol.EffectCount ; i++ )
	{
		if( SearchList->size() == 1 )
			break;

		int RandValue = rand() % int( SearchList->size() );

		Other = (RoleDataEx*)(*SearchList)[ RandValue  ].Data ;

		//Temp = &(m_TargetListMemory[i]);

		if( m_TargetList.back().Role == Other )
		{
			i--;
			continue;
		}
		Temp.Role = Other;
		Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;

		m_TargetList.push_back( Temp );

	}
}
void MagicProcessClass::RangeSelectBox		( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList )
{
	int  i , j;
	vector<float> ArrX;
	vector<float> ArrZ;

	if( (*SearchList).size() == 0 )
		return;

	//角色方向向量
	double Dir; 
	Dir = ( 360 - Owner->BaseData.Pos.Dir )/57.2958;

	float EffectRange = OrgDB->MagicCol.EffectRange / 1.414f;
	RoleDataEx* Other;
	for( i = 0 ; i < (int)(*SearchList).size() ; i++ )
	{
		Other = (RoleDataEx*)(*SearchList)[i].Data ;

		if( Owner->Y() - Other->Y() > 100 )
			continue;

		float Dx = Other->X() - Owner->X();
		float Dz = Other->Z() - Owner->Z();


		float Nx =	Dx*cos(Dir) + Dz*sin(Dir);
		float Nz =  -Dx*sin(Dir)+ Dz*cos(Dir );

		if( abs(Nx) > EffectRange || abs(Nz) > EffectRange )
			continue;

		MagicTargetInfoStruct Temp;// = &(m_TargetListMemory[i]);
		Temp.Role = Other;
		Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;		

		m_TargetList.push_back( Temp );

		if( m_TargetList.size() >= 100 )
			break;

		if( (int)m_TargetList.size() >= OrgDB->MagicCol.EffectCount )
			break;
	}
	
}

void MagicProcessClass::RangeSelectHorizontal     ( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList )
{
	int  i , j;
	vector<float> ArrX;
	vector<float> ArrZ;

	if( (*SearchList).size() == 0 )
		return;

	//角色方向向量
	double Dir; 
	Dir = ( 360 - Owner->BaseData.Pos.Dir + 90 )/57.2958;

	double x = cos( Dir );
	double z = sin( Dir );

	if( OrgDB->MagicCol.EffectRange < 10 )
		OrgDB->MagicCol.EffectRange = 10;

	//計算子彈路徑
	for( j = 10 - OrgDB->MagicCol.EffectRange  ; j <= OrgDB->MagicCol.EffectRange  ; j+= 20 )
	{
		ArrX.push_back( float( j* x + MagicProc->TargetX ) );
		ArrZ.push_back( float( j* z + MagicProc->TargetZ ) );
	}

	RoleDataEx* Other;
	for( i = 0 ; i < (int)(*SearchList).size() ; i++ )
	{
		Other = (RoleDataEx*)(*SearchList)[i].Data ;

		for( j = 0 ; j < (int)ArrX.size() ; j++ )
		{
			if( Other->Length( ArrX[j] , MagicProc->TargetY , ArrZ[j] ) < 15 + Other->TempData.BoundRadiusY )
				break;
		}
		if( j >= (int)ArrX.size() )
			continue;

		MagicTargetInfoStruct Temp;// = &(m_TargetListMemory[i]);
		Temp.Role = Other;
		Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;		

		m_TargetList.push_back( Temp );

		if( m_TargetList.size() >= 100 )
			break;

		if( (int)m_TargetList.size() >= OrgDB->MagicCol.EffectCount )
			break;
	}
}
//----------------------------------------------------------------------------------------	
struct TempSysSpellProcStruct
{
	int				OwnerID;
	MagicProcInfo	MagicProc;
};

int  MagicProcessClass::_SysSpellMagicProc( SchedularInfo* Obj , void* InputClass )
{
	TempSysSpellProcStruct* TempData = (TempSysSpellProcStruct*)InputClass;

	m_SysMagicCount--;
	m_SysMagicIDCount[TempData->MagicProc.MagicCollectID ] --;

	if( RoleDataEx::G_Now <= m_DisableSysMagicTime )
	{
		delete TempData;
		return 0;
	}

	RoleDataEx* Owner = Global::GetRoleDataByGUID( TempData->OwnerID );
	if( Owner == NULL )
	{
		delete TempData;
		return 0;
	}
	MagicProcInfo* MagicProc = &TempData->MagicProc;

	RoleDataEx*		Target = Global::GetRoleDataByGUID( MagicProc->TargetID );
	if( Target == NULL )
		Target = Owner;

	GameObjDbStructEx*		MagicCollectOrgDB = Global::GetObjDB( MagicProc->MagicCollectID );
	if( MagicCollectOrgDB == NULL )
		return 0;

	if( MagicCollectOrgDB->MagicCol.Flag.IgnoreSpellMagicClear == false )
	{
		MagicClearTimeStruct ClearEvent;
		ClearEvent._Value = 0;

		if( MagicCollectOrgDB->MagicCol.EffectType == EM_Magic_Magic )
			ClearEvent.Spell_Magic = true;
		else if( MagicCollectOrgDB->MagicCol.EffectType == EM_Magic_Phy )
			ClearEvent.Spell_Phy = true;

		ClearEvent.Spell = true;
		Owner->ClearBuffByEvent( ClearEvent );
	}

//	Owner->ClearBuffByEvent( ClearEvent );

	MagicEffectStruct Effect;
	Effect._Value[0] = Owner->TempData.Attr.Effect._Value[0];
	Effect._Value[1] = Owner->TempData.Attr.Effect._Value[1];
	Owner->TempData.Attr.Effect.UseMagicDisable = false;
	Owner->TempData.Attr.Effect.UsePhyDisable = false;
	MagicProc->SetSerialID();
	NetSrv_MagicChild::SysBeginShootRange( Owner->GUID() , MagicProc->TargetID , MagicProc->MagicCollectID , MagicProc->SerialID , MagicProc->TargetX , MagicProc->TargetY , MagicProc->TargetZ );

	if( MagicProc->IsNoRebound == true)
		RoleDataEx::IsDisableMagicRebound = true;
	SpellProc_OnSpell( Owner , MagicProc  );
	RoleDataEx::IsDisableMagicRebound = false;

	NetSrv_MagicChild::SysEndShootRange( Owner->GUID() , MagicProc->SerialID );
	Owner->TempData.Attr.Effect.UseMagicDisable = Effect.UseMagicDisable;
	Owner->TempData.Attr.Effect.UsePhyDisable = Effect.UsePhyDisable;

	

	delete TempData;

	return 0;
}

//系統施法
bool	MagicProcessClass::SysSpellMagic ( RoleDataEx* Owner , MagicProcInfo* MagicProc , int DelayTime )
{
	if( RoleDataEx::G_Now <= m_DisableSysMagicTime )
	{
		return false;
	}

	MagicProc->IsSystemMagic = true;
	GameObjDbStructEx*		MagicCollectOrgDB = Global::GetObjDB( MagicProc->MagicCollectID );
	RoleDataEx*		Target = Global::GetRoleDataByGUID( MagicProc->TargetID );

	if( MagicCollectOrgDB == NULL )
		return false;

	if(		MagicCollectOrgDB->MagicCol.TargetType == EM_Releation_Self 
		||  MagicCollectOrgDB->MagicCol.TargetType == EM_Releation_Locatoin )
	{
		MagicProc->TargetID = Owner->GUID();
		Target = Owner;
	}

	if( Target == NULL )
		Target = Owner;

	if(  MagicCollectOrgDB->MagicCol.TargetType != EM_Releation_Locatoin )
	{
		MagicProc->TargetX = Target->X();
		MagicProc->TargetY = Target->Y();
		MagicProc->TargetZ = Target->Z();
	}

	if( Owner->CheckCanSpell( Target , MagicProc->TargetX , MagicProc->TargetY , MagicProc->TargetZ , MagicCollectOrgDB ) != EM_GameMessageEvent_OK )
	{
		return false;
	}

	//利用劇情檢查
	vector<MyVMValueStruct> RetList;
	if( strlen( MagicCollectOrgDB->MagicCol.CheckUseScript ) != 0 )
	{
		bool Ret = true;
		if( LUA_VMClass::PCallByStrArg( MagicCollectOrgDB->MagicCol.CheckUseScript , Owner->GUID() , MagicProc->TargetID , &RetList , 1 ) )
		{
			if( RetList.size() != 1 )
			{
				Ret = false;
			}
			else
			{
				MyVMValueStruct& Temp = RetList[0];
				if( Temp.Flag != true )
					Ret = false;
			}

		}
		else 
			Ret = false;

		if( Ret == false )
		{
			return false;
		}
	}

	TempSysSpellProcStruct* TempData = NEW(TempSysSpellProcStruct);
	TempData->OwnerID = Owner->GUID();
	TempData->MagicProc = *MagicProc;

	m_SysMagicCount++;
	m_SysMagicIDCount[TempData->MagicProc.MagicCollectID ] ++;
	if( m_SysMagicCount >= 2000 )
	{
		m_DisableSysMagicTime = RoleDataEx::G_Now + 1;
		Print( LV5 , "SysSpellMagic" , "m_SysMagicCount >= 2000 , system magic spell times error! (loop??)" );
		map<int,int>::iterator Iter;
		for( Iter = m_SysMagicIDCount.begin() ; Iter != m_SysMagicIDCount.end() ; Iter++ )
		{
			if( Iter->second == 0 )
				continue;
			Print( LV5 , "SysSpellMagic" , "MagicCollectID = %d Count=%d" , Iter->first , Iter->second );
		}

	}

	TempData->MagicProc.BeginX = Owner->X();
	TempData->MagicProc.BeginY = Owner->Y();
	TempData->MagicProc.BeginZ = Owner->Z();


	Global::Schedular()->Push( _SysSpellMagicProc , DelayTime*100 , TempData , NULL );
	/*
	MagicEffectStruct Effect;
	Effect._Value = Owner->TempData.Attr.Effect._Value;
	Owner->TempData.Attr.Effect.UseMagicDisable = false;
	Owner->TempData.Attr.Effect.UsePhyDisable = false;
	MagicProc->SetSerialID();
	NetSrv_MagicChild::SysBeginShootRange( Owner->GUID() , MagicProc->TargetID , MagicProc->MagicCollectID , MagicProc->SerialID );
	SpellProc_OnSpell( Owner , MagicProc  );
	NetSrv_MagicChild::SysEndShootRange( Owner->GUID() , MagicProc->SerialID );
	Owner->TempData.Attr.Effect.UseMagicDisable = Effect.UseMagicDisable;
	Owner->TempData.Attr.Effect.UsePhyDisable = Effect.UsePhyDisable;
*/
	return true;
}
//計算目標資訊
void		MagicProcessClass::_Cal_TargetList			( RoleDataEx* Owner , RoleDataEx* Target, GameObjDbStructEx* MagicCollectOrgDB , MagicProcInfo* MagicProc )
{
		vector<BaseSortStruct>*	SearchList = NULL;
	m_TargetList.clear();
/*
	if( MagicCollectOrgDB->MagicCol.TargetType == EM_Releation_Self )
	{
		Target = Owner;
		TargetID = OwnerID;
	}
	*/

	if( MagicCollectOrgDB->MagicCol.RangeType == EM_MagicEffect_Target )
	{
		MagicTargetInfoStruct Temp;// = &m_TargetListMemory[0];
		Temp.Role = Target;
		Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;

		if( Owner->CheckRelation( Target , MagicCollectOrgDB->MagicCol.TargetType , MagicCollectOrgDB->MagicCol.IsIgnoreTargetDead ) != false)
		{
			m_TargetList.push_back( Temp );
		}

	}
	else
	{
		float TargetX , TargetY , TargetZ;
		if( MagicCollectOrgDB->MagicCol.TargetType == EM_Releation_Locatoin )
		{
			TargetX = MagicProc->TargetX;
			TargetY = MagicProc->TargetY;
			TargetZ = MagicProc->TargetZ;
//			Target  = Owner;
//			TargetID = Owner->GUID();
		}
		else
		{
			MagicProc->TargetX = TargetX = Target->Pos()->X;
			MagicProc->TargetY = TargetY = Target->Pos()->Y;
			MagicProc->TargetZ = TargetZ = Target->Pos()->Z;
		}

		if( EM_MagicEffect_Plot == MagicCollectOrgDB->MagicCol.RangeType)
		{
			LUA_VMClass::PCallByStrArg( MagicCollectOrgDB->MagicCol.SelectTargetScript , Owner->GUID() , Target->GUID() );
			return;
		}
		


		switch( MagicCollectOrgDB->MagicCol.RangeType )
		{
		case EM_MagicEffect_Good_Range:	//好的範圍法術
			SearchList = Global::SearchRangeObject( Owner , TargetX , TargetY , TargetZ , EM_SearchRange_Friend , MagicCollectOrgDB->MagicCol.EffectRange	);
			break;
		case EM_MagicEffect_Good_Member:	//好的成員法術
			SearchList = Global::SearchRangeObject( Owner , TargetX , TargetY , TargetZ , EM_SearchRange_Member , MagicCollectOrgDB->MagicCol.EffectRange	);
			break;
		case EM_MagicEffect_Bad_Range:	//壞的範圍法術
			SearchList = Global::SearchRangeObject( Owner , TargetX , TargetY , TargetZ , EM_SearchRange_Enemy  , MagicCollectOrgDB->MagicCol.EffectRange	);
			break;
		case EM_MagicEffect_AllObj:
			SearchList = Global::SearchRangeObject( Owner , TargetX , TargetY , TargetZ , EM_SearchRange_All  , MagicCollectOrgDB->MagicCol.EffectRange	);
			break;
		case EM_MagicEffect_AllPlayer:
			SearchList = Global::SearchRangeObject( Owner , TargetX , TargetY , TargetZ , EM_SearchRange_Player  , MagicCollectOrgDB->MagicCol.EffectRange	);
			break;
		case EM_MagicEffect_AllMonster:
			SearchList = Global::SearchRangeObject( Owner , TargetX , TargetY , TargetZ , EM_SearchRange_NPC  , MagicCollectOrgDB->MagicCol.EffectRange	);
			break;
		case EM_MagicEffect_AllPlayer_Enemy:
			SearchList = Global::SearchRangeObject( Owner , TargetX , TargetY , TargetZ , EM_SearchRange_Player_Enemy  , MagicCollectOrgDB->MagicCol.EffectRange	);
			break;
		case EM_MagicEffect_AllPlayer_Friend:
			SearchList = Global::SearchRangeObject( Owner , TargetX , TargetY , TargetZ , EM_SearchRange_Player_Friend  , MagicCollectOrgDB->MagicCol.EffectRange	);
			break;
		default:
			SearchList = NULL;
			break;
		}

		//選取方式
		if( SearchList != NULL )
		{

			switch( MagicCollectOrgDB->MagicCol.RangeSelectType )
			{
			case EM_MagicSpellSelectType_Circle:	//圓形
				RangeSelectCircle( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Line1:		//直線( 目標面向  有效距離)
				RangeSelectLine1( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Line2:		//直線( 施法者 -> 目標 )
				RangeSelectLine2( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Line3:		//直線( 施法者向目標  有效距離 )
				RangeSelectLine3( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Fan:		//扇形
				RangeSelectSector( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Lighting1:	//連鎖電
				RangeSelectLightning1( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Lighting2:	//連鎖電( 可重覆選 )
				RangeSelectLightning2( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Horizontal://橫線
				RangeSelectHorizontal( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Box://橫線
				RangeSelectBox( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			default:
				break;
			}
		}
	}

	BaseItemObject* OwnerObj = Global::GetObj( Owner->GUID() );
	if( OwnerObj == NULL )
		return;
	//搜尋所有角色與目標之間是否有阻檔
	for( unsigned i = 0 ; i < m_TargetList.size() ; i++ )
	{
		RolePosStruct* Pos = m_TargetList[i].Role->Pos();
		//目標列表內的與 目標的距離檢查
		
		if( MagicCollectOrgDB->MagicCol.TargetType == EM_Releation_Locatoin )
		{
			if( MagicCollectOrgDB->MagicCol.RangeType != EM_MagicEffect_Target)
			{
				if(		m_TargetList[i].Role->Length3D( MagicProc->TargetX , MagicProc->TargetY , MagicProc->TargetZ) >= MagicCollectOrgDB->MagicCol.EffectRange + 1 + m_TargetList[i].Role->TempData.BoundRadiusY   
					||	abs( m_TargetList[i].Role->Y() - MagicProc->TargetY ) > MagicCollectOrgDB->MagicCol.AttackDistance )
				{
					m_TargetList[i].IsDel = true;
					continue;
				}
			}
			else
			{
				if(	abs( m_TargetList[i].Role->Y() - MagicProc->TargetY ) > MagicCollectOrgDB->MagicCol.AttackDistance )
				{
					m_TargetList[i].IsDel = true;
					continue;
				}
			}

		}
		else
		{
/*			if( Target->Length3D( m_TargetList[i].Role ) >= MagicCollectOrgDB->MagicCol.EffectRange + 1 )
			{
				m_TargetList[i].IsDel = true;
				continue;
			}*/
		}
		if( MagicCollectOrgDB->MagicCol.Flag.Ignore_Obstruct == false && MagicCollectOrgDB->MagicCol.RangeType != EM_MagicEffect_Target )
		{
			if( OwnerObj->Path()->CheckLine_Magic( Pos->X , Pos->Y , Pos->Z , MagicProc->TargetX , MagicProc->TargetY , MagicProc->TargetZ ) == false )
			{
				m_TargetList[i].IsDel = true;
			}
			else
			{
				m_TargetList[i].IsDel = false;
			}
		}
	}

}
//施法處裡(開始施法)
bool MagicProcessClass::SpellProc_OnSpell( RoleDataEx* Owner , MagicProcInfo* _MagicProc  )
{


	MagicProcInfo*			MagicProc; 
	if( _MagicProc == NULL )
		MagicProc = &Owner->TempData.Magic;
	else
	{
		MagicProc = _MagicProc;
	}	

	GameObjDbStructEx*		MagicCollectOrgDB = Global::GetObjDB( MagicProc->MagicCollectID );
	GameObjDbStructEx*		MagicBaseOrgDB  ;

	if( MagicCollectOrgDB == NULL )
	{
		Print( LV2 , "SpellProc_OnSpell" , "MagicCollectOrgDB == NULL" );
		return false;
	}

	Owner->TempData.BackInfo.LastSkillHitPoint = 0;


	int OwnerID  = Owner->GUID();
	//int TargetID = MagicProc->TargetID;	

	RoleDataEx*		Target = Global::GetRoleDataByGUID( MagicProc->TargetID );


	MagicProc->State = EM_MAGIC_PROC_STATE_SHOOT;

	if( Target == NULL )
	{
		Target = Owner;
	}

	if( Target->IsSpell() )
	{
		//取得施法的資料
		GameObjDbStructEx* TargetMagicCol = Global::GetObjDB( Target->TempData.Magic.MagicCollectID );
		if( TargetMagicCol->IsMagicCollect() && TargetMagicCol->MagicCol.IsLockTarget && Target->BaseData.Mode.DisableRotate == false  )
		{
			Target->BaseData.Pos.Dir = Target->CalDir( Owner );
		}
	}
	else
	{
		if( MagicCollectOrgDB->MagicCol.IsLockTarget && Owner->BaseData.Mode.DisableRotate == false )
		{
			Owner->BaseData.Pos.Dir = Owner->CalDir( Target );
		}
	}

	//如果目標在攻擊模式
	if( Target != Owner )
	{
		bool	IsTargetSeeOwner = true;
		if( Target->CalAngle( Owner ) > 135 )
		{
			IsTargetSeeOwner = false;
		}

		if( Target->IsAttack() || IsTargetSeeOwner != false )
		{
			//Owner->TempData.BackInfo.BuffClearTime.TargetOnAttack =true;
			MagicClearTimeStruct ClearEvent;
			ClearEvent._Value = 0;
			ClearEvent.TargetOnAttack = true;
			Owner->ClearBuffByEvent( ClearEvent );
		}
	} 
	else
	{
		if( MagicCollectOrgDB->MagicCol.RangeType != EM_MagicEffect_Target )
		{
			Owner->TempData.BackInfo.BuffClearTime.TargetOnAttack =true;
			Owner->ClearBuffByEvent(  );
		}
	}

	if(		MagicCollectOrgDB->MagicCol.TargetType == EM_Releation_Self 
		||	(		MagicCollectOrgDB->MagicCol.RangeType != EM_MagicEffect_Target 
				&&	MagicCollectOrgDB->MagicCol.TargetType == EM_Releation_Locatoin) )
		Target = Owner;


	_Cal_TargetList( Owner , Target , MagicCollectOrgDB , MagicProc );

/*
	m_TargetList.clear();

	if( MagicCollectOrgDB->MagicCol.TargetType == EM_Releation_Self )
	{
		Target = Owner;
		TargetID = OwnerID;
	}

	if( MagicCollectOrgDB->MagicCol.RangeType == EM_MagicEffect_Target )
	{
		MagicTargetInfoStruct Temp;// = &m_TargetListMemory[0];
		Temp.Role = Target;
		Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;
		
		if( Owner->CheckRelation( Target , MagicCollectOrgDB->MagicCol.TargetType  ) != false)
		{
			m_TargetList.push_back( Temp );
		}
		
	}
	else
	{
		float TargetX , TargetY , TargetZ;
		if( MagicCollectOrgDB->MagicCol.TargetType == EM_Releation_Locatoin )
		{
			TargetX = MagicProc->TargetX;
			TargetY = MagicProc->TargetY;
			TargetZ = MagicProc->TargetZ;
			Target  = Owner;
			TargetID = OwnerID;
		}
		else
		{
			MagicProc->TargetX = TargetX = Target->Pos()->X;
			MagicProc->TargetY = TargetY = Target->Pos()->Y;
			MagicProc->TargetZ = TargetZ = Target->Pos()->Z;
		}

		switch( MagicCollectOrgDB->MagicCol.RangeType )
		{
		case EM_MagicEffect_Good_Range:	//好的範圍法術
			SearchList = Global::SearchRangeObject( Owner , TargetX , TargetY , TargetZ , EM_SearchRange_Friend , MagicCollectOrgDB->MagicCol.EffectRange	);
			break;
		case EM_MagicEffect_Good_Member:	//好的成員法術
			SearchList = Global::SearchRangeObject( Owner , TargetX , TargetY , TargetZ , EM_SearchRange_Member , MagicCollectOrgDB->MagicCol.EffectRange	);
			break;
		case EM_MagicEffect_Bad_Range:	//壞的範圍法術
			SearchList = Global::SearchRangeObject( Owner , TargetX , TargetY , TargetZ , EM_SearchRange_Enemy  , MagicCollectOrgDB->MagicCol.EffectRange	);
			break;
		case EM_MagicEffect_AllObj:
			SearchList = Global::SearchRangeObject( Owner , TargetX , TargetY , TargetZ , EM_SearchRange_All  , MagicCollectOrgDB->MagicCol.EffectRange	);
			break;
		case EM_MagicEffect_AllPlayer:
			SearchList = Global::SearchRangeObject( Owner , TargetX , TargetY , TargetZ , EM_SearchRange_Player  , MagicCollectOrgDB->MagicCol.EffectRange	);
			break;
		case EM_MagicEffect_AllMonster:
			SearchList = Global::SearchRangeObject( Owner , TargetX , TargetY , TargetZ , EM_SearchRange_NPC  , MagicCollectOrgDB->MagicCol.EffectRange	);
			break;
		default:
			SearchList = NULL;
			break;
		}

		//選取方式
		if( SearchList != NULL )
		{

			switch( MagicCollectOrgDB->MagicCol.RangeSelectType )
			{
			case EM_MagicSpellSelectType_Circle:	//圓形
				RangeSelectCircle( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Line1:		//直線( 目標面向  有效距離)
				RangeSelectLine1( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Line2:		//直線( 施法者 -> 目標 )
				RangeSelectLine2( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Line3:		//直線( 施法者向目標  有效距離 )
				RangeSelectLine3( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Fan:		//扇形
				RangeSelectSector( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Lighting1:	//連鎖電
				RangeSelectLightning1( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Lighting2:	//連鎖電( 可重覆選 )
				RangeSelectLightning2( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;
			case EM_MagicSpellSelectType_Horizontal://橫線
				RangeSelectHorizontal( Owner , MagicProc , MagicCollectOrgDB , SearchList );
				break;

			default:
				break;
			}
		}
	}

	//搜尋所有角色與目標之間是否有阻檔
	for( unsigned i = 0 ; i < m_TargetList.size() ; i++ )
	{
		RolePosStruct* Pos = m_TargetList[i].Role->Pos();
		if( PathManageClass::CheckLine( Pos->X , Pos->Y , Pos->Z , MagicProc->TargetX , MagicProc->TargetY , MagicProc->TargetZ ) == false )
		{
			m_TargetList[i].IsDel = true;
		}
		else
		{
			m_TargetList[i].IsDel = false;
		}
	}
*/
	MagicBaseOrgDB = Global::GetObjDB(  MagicCollectOrgDB->MagicCol.Magic[0][0].MagicBaseID );
	CalTargetListHitType( Owner , MagicCollectOrgDB , MagicBaseOrgDB );


	for( int GroupID = 0 ; GroupID < _MAX_MAGICCOLLECT_GROUP_COUNT_ ; GroupID++ )
	{

		//把狀態清除
		for( unsigned i = 0 ; i < m_TargetList.size() ; i++ )
		{
			m_TargetList[i].IsSpellOneMagic = false;
		}


			CalTargetListHitType( Owner , MagicCollectOrgDB , MagicBaseOrgDB );

		//是否有施展法術
		//		bool	IsSpellOneMagic = false;
		for( int i = 0  ; i < _MAX_MAGICCOLLECT_COUNT_ ; i++ )
		{
			if( MagicProc->MagicEnabled[GroupID][i] == false )
				continue;

			MagicCollectCondition& MagicCnd = MagicCollectOrgDB->MagicCol.Magic[GroupID][i];

			MagicBaseOrgDB = Global::GetObjDB( MagicCnd.MagicBaseID );
			if( MagicBaseOrgDB == NULL )
				continue;
			

			switch( MagicCollectOrgDB->MagicCol.MagicSpellGroupType )
			{
			case EM_MagicSpellGroupType_Normal:			//全部獨立計算
				{
					//CalTargetListHitType( Owner , MagicCollectOrgDB , MagicBaseOrgDB );
					//計算命重率與命中類型					
					m_IsMissErase = false;
				}
				break;
			case EM_MagicSpellGroupType_Vampire:		//第一個法術命中才會處發第二與第三組法術
			case EM_MagicSpellGroupType_Vampire1:
			case EM_MagicSpellGroupType_FirstGroupHit:	//第一個法術命中才會處發第二與第三組法術
				{
					if( GroupID == 0 )
						m_IsMissErase = true;
					else
						m_IsMissErase = false;
				}
				break;
			case EM_MagicSpellGroupType_PrvGroupHit:	//前一組法術命中，下一組才會處理
				{
					m_IsMissErase = true;
				}				
				break;
			}

			m_ProcMagicID = MagicCnd.MagicBaseID;

			switch( MagicCollectOrgDB->MagicCol.MagicSpellGroupType )
			{
			case EM_MagicSpellGroupType_Normal:			//全部獨立計算
				break;
			case EM_MagicSpellGroupType_FirstGroupHit:	//第一個法術命中才會處發第二與第三組法術
				{
					if( GroupID != 0 )
					{
						for( unsigned int i = 0 ; i < m_TargetList.size() ; i ++ )
						{
							m_TargetList[i].HitType = EM_MAGIC_HIT_TYPE_HIT;
						}

					}

				}
				break;
			case EM_MagicSpellGroupType_PrvGroupHit:	//前一組法術命中，下一組才會處理
				{
				}
				break;
			case EM_MagicSpellGroupType_Vampire:		//第一個法術命中才會處發第二與第三組法術
				{
					if( GroupID != 0 )
					{
						bool IsSpellOnce = false;
						for( int i = 0 ; i < m_TargetList.size() ; i++ )
						{
							if( m_TargetList[i].IsDel == false )
							{
								IsSpellOnce = true;
								break;
							}
						}
						m_TargetList.clear();
						if( IsSpellOnce != false  )
						{
							MagicTargetInfoStruct Temp;// = &m_TargetListMemory[0];
							Temp.Role = Owner;
							Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;
							m_TargetList.push_back( Temp );
						}
					}
				}
				break;
			case EM_MagicSpellGroupType_Vampire1:
				{
					if( GroupID == 2 )
					{
						bool IsSpellOnce = false;
						for( int i = 0 ; i < m_TargetList.size() ; i++ )
						{
							if( m_TargetList[i].IsDel == false )
							{
								IsSpellOnce = true;
								break;
							}
						}
						m_TargetList.clear();
						if( IsSpellOnce != false  )
						{
							MagicTargetInfoStruct Temp;// = &m_TargetListMemory[0];
							Temp.Role = Owner;
							Temp.HitType = EM_MAGIC_HIT_TYPE_HIT;
							m_TargetList.push_back( Temp );
						}
					}

				}
				break;
			}

			//依條件設定是否可以對目標施展此法術
			m_CheckMagicBase[ MagicCnd.FuncType ]( Owner , MagicCnd.Arg[0] , MagicCnd.Arg[1] );

			for( unsigned i = 0 ; i < m_ProcTargetList.size() ; i++ )
			{
				MagicTargetInfoStruct* TargetInfo = m_ProcTargetList[i];
				if( TargetInfo->Role->TempData.UpdateInfo.IsDualDead != false )
				{
					TargetInfo->HitType = EM_MAGIC_HIT_TYPE_CANCEL;
				}
			}

			if( m_ProcTargetList.size() != 0 )
			{
				CheckUseLuaProc( Owner , MagicBaseOrgDB );
				RoleDataEx::IsDisablePVPRule_Temp = MagicCollectOrgDB->MagicCol.IsDiabledPvPRule;
				m_Magic[ MagicBaseOrgDB->MagicBase.MagicFunc ]( Owner , MagicBaseOrgDB , MagicProc );
				RoleDataEx::IsDisablePVPRule_Temp = false;
			}

			m_ProcMagicID = -1;
		}

		//如果前面一個群組沒有施任何法術
		switch( MagicCollectOrgDB->MagicCol.MagicSpellGroupType )
		{
		case EM_MagicSpellGroupType_Normal:			//全部獨立計算
			break;
		case EM_MagicSpellGroupType_Vampire:
		case EM_MagicSpellGroupType_Vampire1:
		case EM_MagicSpellGroupType_FirstGroupHit:	//第一個法術命中才會處發第二與第三組法術
			{
				if( GroupID == 0 )
				{
					if( MagicCollectOrgDB->MagicCol.RangeType == EM_MagicEffect_Target )
					{
						for( unsigned i =  0  ; i < m_TargetList.size() ; i++ )
						{			
							if( m_TargetList[i].IsSpellOneMagic == false )
							{
								m_TargetList[i].IsDel = true;
							}
						}				
					}
				}
			}
			break;
		case EM_MagicSpellGroupType_PrvGroupHit:	//前一組法術命中，下一組才會處理
			{
				for( unsigned i =  0  ; i < m_TargetList.size() ; i++ )
				{			
					if( m_TargetList[i].IsSpellOneMagic == false )
					{
						m_TargetList[i].IsDel = true;
					}
				}	
			}				
			break;
		}

	}

	if( MagicProc->IsSuccess == false && MagicProc->IsPayCost == false )
	{
		MagicProc->IsSuccess = true;
		//刪除消耗
		Owner->DelMagicCost( MagicCollectOrgDB , MagicProc->MagicLv  );
		if( MagicProc->IsSystemMagic == false )
			Owner->SpecialAttackBuffProc_SelfSpell( Target );
	}
	
	return true;
}
//計算命重率與命中類型
void MagicProcessClass::CalTargetListHitType( RoleDataEx* Owner , GameObjDbStructEx* MagicCollectOrgDB , GameObjDbStructEx* MagicBaseOrgDB )
{
	if( MagicBaseOrgDB == NULL || MagicCollectOrgDB == NULL )
		return;

	if( Owner->BaseData.Mode.RelyOwnerPower != false )
	{
		RoleDataEx* MasterOwner = Global::GetRoleDataByGUID( Owner->OwnerGUID() );
		if( MasterOwner != NULL )
			Owner = MasterOwner;
	}	

	int i ;
	int HitRate = 0;

	int			CritHit = 0;

	for( i = 0 ; i < (int)m_TargetList.size() ; i++  )
	{
		MagicTargetInfoStruct& Data = m_TargetList[i];
		RoleDataEx* Target = Data.Role;

		int DLv = Owner->Level() - Data.Role->Level();

		if( MagicCollectOrgDB->MagicCol.IsIgnoreInvincible == false )
		{
			if( MagicBaseOrgDB->MagicBase.Setting.GoodMagic == false )
			{
				if( MagicBaseOrgDB->MagicBase.EffectType == EM_Magic_Magic && Data.Role->TempData.Attr.Effect.BadMagicInvincible )
				{
					Data.HitType = EM_MAGIC_HIT_TYPE_NO_EFFECT;
					continue;
				}
				else if( MagicBaseOrgDB->MagicBase.EffectType == EM_Magic_Phy && Data.Role->TempData.Attr.Effect.BadPhyInvincible )
				{
					Data.HitType = EM_MAGIC_HIT_TYPE_NO_EFFECT;
					continue;
				}
			}
			else
			{
				if( MagicBaseOrgDB->MagicBase.EffectType == EM_Magic_Magic && Data.Role->TempData.Attr.Effect.GoodMagicInvincible )
				{
					Data.HitType = EM_MAGIC_HIT_TYPE_NO_EFFECT;
					continue;
				}
				else if( MagicBaseOrgDB->MagicBase.EffectType == EM_Magic_Phy && Data.Role->TempData.Attr.Effect.GoodPhyInvincible )
				{
					Data.HitType = EM_MAGIC_HIT_TYPE_NO_EFFECT;
					continue;
				}
			}
		}

		float DHitRate = 1;

		if(  MagicCollectOrgDB->MagicCol.HitRateFunc != EM_HitRateFunc_Base )
		{
			if( MagicCollectOrgDB->MagicCol.EffectType == EM_Magic_Phy )
			{
				int PhyHitRate = 100;
				int PhyMissRate = 0;
				int PhyDodgeRate = 0;

				Owner->CalAttackTypeInfo( Target  , EM_AttackHandType_Main , PhyHitRate , PhyDodgeRate , PhyMissRate );

				if( rand() % 100 < PhyMissRate )
				{
					Data.HitType = EM_MAGIC_HIT_TYPE_MISS;
					continue;
				}

				DHitRate = float( PhyHitRate ) /100;
			}
			else
			{

				DHitRate = ( Owner->TempData.Attr.Fin.MagicHitRate - Target->TempData.Attr.Fin.MagicDodgeRate ) / __max( Owner->TempData.Attr.Fin.MagicHitRate , Target->TempData.Attr.Fin.MagicDodgeRate );
				DHitRate = ( DHitRate * Owner->FixArg().MagHit_Range + Owner->FixArg().MagHit_Base ) /100;
				float AbsHitRate = ( Owner->TempData.Attr.Mid.Body[ EM_WearEqType_MagicHitRate ] - Target->TempData.Attr.Mid.Body[EM_WearEqType_MagicDodgeRate] ) / 100.0f;
				DHitRate = DHitRate + AbsHitRate;
			}
		}

		//命中率計算
		switch( MagicCollectOrgDB->MagicCol.HitRateFunc )
		{
		case EM_HitRateFunc_Base:				//定值
			HitRate = int( MagicCollectOrgDB->MagicCol.HitRateArg[0] );
			break;
		case EM_HitRateFunc_DLV:				//等級差
			{
				//agi 計算
				//float HitRateFix = ( (Owner->TempData.Attr.Fin.AGI*1.5f + 20 )  - Data.Role->TempData.Attr.Fin.AGI )/ __max( ( Owner->TempData.Attr.Fin.AGI*1.5f + 20 )  , Data.Role->TempData.Attr.Fin.AGI)*25 ;


				if( Owner->IsPlayer() &&  Data.Role->IsPlayer()  )
				{
					HitRate = int( MagicCollectOrgDB->MagicCol.HitRateArg[0] * DHitRate );
				}
				else
				{
					HitRate = int( (MagicCollectOrgDB->MagicCol.HitRateArg[0] + DLv * MagicCollectOrgDB->MagicCol.HitRateArg[1] )* DHitRate);
				}
			}
			break;
		case EM_HitRateFunc_DLVxDLV:			//等級差平方
			{
				if( Owner->IsPlayer() &&  Data.Role->IsPlayer()  )
				{
					HitRate = int( MagicCollectOrgDB->MagicCol.HitRateArg[0] * DHitRate);
				}
				else
				{
					if( DLv < 0 )
					{
						HitRate = int( (MagicCollectOrgDB->MagicCol.HitRateArg[0] - DLv * DLv * MagicCollectOrgDB->MagicCol.HitRateArg[1] )*DHitRate );
					}
					else
					{
						HitRate = int( (MagicCollectOrgDB->MagicCol.HitRateArg[0] + DLv * DLv * MagicCollectOrgDB->MagicCol.HitRateArg[1] )*DHitRate );
					}

				}

			}
			break;
		case EM_HitRateFunc_DecPerPersion:		//依人次降低
			{
				HitRate = int( (MagicCollectOrgDB->MagicCol.HitRateArg[0] + i * MagicCollectOrgDB->MagicCol.HitRateArg[1] )*DHitRate);
			}
			break;

		case EM_HitRateFunc_Shoot:		
			{
				float ShootSkill = 0;// = Target->GetSkillValue( EM_SkillValueType_Define );
				switch( Owner->TempData.Attr.BowWeaponType )
				{
				case EM_Weapon_Bow:
					ShootSkill = Owner->GetSkillValue( EM_SkillValueType_Bow );
					break;
				case EM_Weapon_CossBow:
					ShootSkill = Owner->GetSkillValue( EM_SkillValueType_CossBow );
					break;
				case EM_Weapon_Gun:
					ShootSkill = Owner->GetSkillValue( EM_SkillValueType_Gun );
					break;
				}

				float SkillDLv = ShootSkill / 2 - Data.Role->Level();
				if( DLv < 0 )
					SkillDLv +=DLv*2;

				if( SkillDLv > 20 )
					SkillDLv = 20;
				else if( SkillDLv < -20 )
					SkillDLv = -20;

				HitRate = int( ( MagicCollectOrgDB->MagicCol.HitRateArg[0] + SkillDLv ) * DHitRate );

				if( Owner->IsPlayer() &&  Data.Role->IsPlayer()  )
				{
					//HitRate = int( MagicCollectOrgDB->MagicCol.HitRateArg[0] + HitRateFix );
				}
				else
				{
					HitRate = int( HitRate + DLv * MagicCollectOrgDB->MagicCol.HitRateArg[1]  );
				}

				//float DSkillRate = ( WeaponSkill - TargetDefSkill ) / __max( 40 , __max( WeaponSkill , TargetDefSkill ) );
				//HitRate = 100;
				//HitRate = int( MagicCollectOrgDB->MagicCol.HitRateArg[0] + i * MagicCollectOrgDB->MagicCol.HitRateArg[1] );
			}
			break;

		}
/*
		if( Owner->IsPlayer() != false && Data.Role->IsPlayer() == false )
		{
			if( HitRate < 5 )
				HitRate = 5;
		}
		else
		{
			if( HitRate < 50 )
				HitRate = 50;
		}
		*/

		if( rand() % 100 >= HitRate )
		{//命中
			Data.HitType = EM_MAGIC_HIT_TYPE_DODGE;
			continue;
		}
		Data.HitType = EM_MAGIC_HIT_TYPE_HIT;
	}

}

//----------------------------------------------------------------------------------------
struct TempShootProcStruct
{
	int				OwnerID;
	MagicProcInfo	MagicProc;
	bool			SendEndMagic;
};
int      MagicProcessClass::_ShootProc( SchedularInfo* Obj , void* InputClass )
{
	TempShootProcStruct* TempData = (TempShootProcStruct*)InputClass;

	RoleDataEx*		Owner = Global::GetRoleDataByGUID( TempData->OwnerID );
	MagicProcInfo*	MagicProc = &TempData->MagicProc;

	if( Owner == NULL )
	{
		delete TempData;
		return 0;
	}

	if( MagicProc->SpellCount <= 1 )
	{
		GameObjDbStructEx*		OrgDB = Global::GetObjDB( MagicProc->MagicCollectID );
		Global::St()->ComboProc.CheckSkillCombo_PreProc( Owner ,  MagicProc->TargetID  , OrgDB->MagicCol.ComboType );
	}

	if( Owner != NULL )
	{		
		if( Owner->BaseData.HP > 0)
		{//如果施法者已死，就法術中斷
			Owner->TempData.BackInfo.BuffClearTime.Spell = true;
			SpellProc_OnSpell( Owner , MagicProc );

			if( TempData->SendEndMagic )
				NetSrv_MagicChild::EndShootRange( Owner->GUID() , MagicProc->SerialID );
		}
		

		//消除施法消失的Buff
		Owner->ClearBuffByEvent( );
	}

	if( MagicProc->SpellCount <= 1 )
	{
		GameObjDbStructEx*		OrgDB = Global::GetObjDB( MagicProc->MagicCollectID );
		Global::St()->ComboProc.CheckSkillCombo( Owner ,  MagicProc->TargetID  , OrgDB->MagicCol.ComboType );
	}
	delete TempData;


	return 0;
}

int MagicProcessClass::Process( RoleDataEx* Owner , MagicProcInfo*	MagicProc  )
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObj( Owner->GUID() );
	if( OwnerObj == NULL )
		return 0;

	if( EM_MAGIC_PROC_STATE_NORMAL == MagicProc->State && &Owner->TempData.Magic == MagicProc )
	{
		if(  Owner->TempData.CycleMagic.MagicID != 0 )
		{
			int MagicID = Owner->TempData.CycleMagic.MagicID;
			int MagicPos = Owner->TempData.CycleMagic.MagicPos;

			RoleDataEx* Target = Global::GetRoleDataByGUID( Owner->TempData.LookTargetID );
			GameObjDbStructEx* MagicDB = Global::GetObjDB( MagicID );
			if( Target != NULL && MagicDB != NULL )
			{		
				if( MagicDB->MagicCol.Flag.Interrupt_OnMove == false || Owner->IsMove() == false )
				{
					GameMessageEventENUM Ret = EM_GameMessageEvent_Failed;

					//if( Owner->TempData.CycleMagic.MessageDelayCount < 20 )
					{
						Ret = Owner->CheckCanBeginSpell( Target , Target->X() , Target->Y() , Target->Z()
							, MagicDB ,  Owner->GetMagicLv( MagicPos ) );
					}

					if( Ret == EM_GameMessageEvent_OK )
					{
						BaseItemObject* OwnerObj = BaseItemObject::GetObj( Owner->GUID() );
						NetSrv_MagicChild::This->R_SpellRequest( OwnerObj , Owner->GUID() 
							, Owner->TempData.LookTargetID 
							, Owner->TempData.CycleMagic.MagicID 
							, Owner->TempData.CycleMagic.MagicPos );

						//Owner->TempData.CycleMagic.MessageDelayCount = 0;
					}
					else
					{
						//Owner->TempData.CycleMagic.MessageDelayCount++;
						return 1;
					}

				}
			}
		}
	}



	int	Ret = 1;

	switch( MagicProc->State )
	{
	case EM_MAGIC_PROC_STATE_NORMAL:					//一般非施法狀態
		return 2;
	case EM_MAGIC_PROC_STATE_PERPARE:					//準備施法中

	case EM_MAGIC_PROC_STATE_SHOOT:
		{
			if( Owner->CheckMagicInterrupt() )
			{
				MagicProc->State = EM_MAGIC_PROC_STATE_INTERRUPT;
				return 1;
			}

			RoleDataEx*				Target = Global::GetRoleDataByGUID( MagicProc->TargetID );
			GameObjDbStructEx*		MagicCollectOrgDB = Global::GetObjDB( MagicProc->MagicCollectID );

			if( Target == NULL )
			{
				MagicProc->State = EM_MAGIC_PROC_STATE_INTERRUPT;
				return 1;
			}


			if( Owner->IsNPC() )
			{
				//檢查直線是否有阻檔
				if( MagicCollectOrgDB->MagicCol.TargetType != EM_Releation_Locatoin )
				{
					MagicProc->TargetX = Target->X();
					MagicProc->TargetY = Target->Y();
					MagicProc->TargetZ = Target->Z();
				}

				//檢查對此目標是否可以施法
				GameMessageEventENUM Ret = Owner->CheckCanSpell( Target , MagicProc->TargetX , MagicProc->TargetY , MagicProc->TargetZ , MagicCollectOrgDB );
				if(  Ret != EM_GameMessageEvent_OK  )
				{
					Owner->Net_GameMsgEvent( Ret );
					MagicProc->State = EM_MAGIC_PROC_STATE_INTERRUPT;
					return 1;
				}
				if( MagicCollectOrgDB->MagicCol.Flag.Ignore_Obstruct == false )
				{
					if( OwnerObj->Path()->CheckLine_Magic( Owner->X() , Owner->Y() , Owner->Z() , MagicProc->TargetX , MagicProc->TargetY , MagicProc->TargetZ ) == false )
					{
						Owner->Net_GameMsgEvent( EM_GameMessageEvent_MagicError_TargetLineNotClear );
						MagicProc->State = EM_MAGIC_PROC_STATE_INTERRUPT;
						return 1;
					}
				}

				if( RoleDataEx::CheckTime( MagicProc->ProcTime ) == false )
					return 2;
			}
			else
			{
				if( RoleDataEx::CheckTime( MagicProc->ProcTime ) == false )
					return 1;

				//檢查對此目標是否可以施法
				GameMessageEventENUM Ret = Owner->CheckCanSpell( Target , MagicProc->TargetX , MagicProc->TargetY , MagicProc->TargetZ , MagicCollectOrgDB );
				if(  Ret != EM_GameMessageEvent_OK || Target == NULL )
				{
					if( Target != NULL )
						Owner->Net_GameMsgEvent( Ret );
					MagicProc->State = EM_MAGIC_PROC_STATE_INTERRUPT;
					return 1;
				}

				if( MagicCollectOrgDB->MagicCol.Flag.Ignore_Obstruct == false )
				{
					if( OwnerObj->Path()->CheckLine_Magic( Owner->TempData.Move.CliX , Owner->TempData.Move.CliY , Owner->TempData.Move.CliZ , MagicProc->TargetX , MagicProc->TargetY , MagicProc->TargetZ ) == false )
					{
						Owner->Net_GameMsgEvent( EM_GameMessageEvent_MagicError_TargetLineNotClear );
						MagicProc->State = EM_MAGIC_PROC_STATE_INTERRUPT;
						return 1;
					}
				}
			}

/*
			//////////////////////////////////////////////////////////////////////////
			//目標計算
			//////////////////////////////////////////////////////////////////////////
			_Cal_TargetList( Owner , Target, MagicCollectOrgDB , MagicProc );
			vector<int> TargetGUIDList;
			for( unsigned i = 0 ; i < m_TargetList.size() ; i++ )
			{
				if( m_TargetList[i].IsDel || m_TargetList[i].HitType == EM_MAGIC_HIT_TYPE_CANCEL )
					continue;

				TargetGUIDList.push_back( m_TargetList[i].Role->GUID() );
			}

			NetSrv_MagicChild::BeginShootRange( Owner->GUID() , MagicProc->SerialID , TargetGUIDList );
			*/
			//////////////////////////////////////////////////////////////////////////

			GameObjDbStructEx*		OrgDB = Global::GetObjDB( MagicProc->MagicCollectID );

			int ShootDelayTime = 0;

			if( OrgDB->MagicCol.Flag.DescDurable != false )
			{
				//如果參照遠程武器
				if( OrgDB->MagicCol.Flag.ReferenceWeapon_Bow_SpellTime != false )
				{
					int DecDurable = Owner->TempData.Sys.KillRate * 2;
					Owner->EqDurableProcess( EM_EQWearPos_Bow , DecDurable );
				}
				else if( OrgDB->MagicCol.Flag.ReferenceWeapon_Throw_SpellTime != false )
				{

				}
				else
				{
					int DecDurable = Owner->TempData.Sys.KillRate * 2;
					Owner->EqDurableProcess( EM_EQWearPos_MainHand , DecDurable );
				}
			}

			if( OrgDB->MagicCol.MagicAttackDelay > 0 )
			{
				ShootDelayTime = ShootDelayTime + int( OrgDB->MagicCol.MagicAttackDelay * 1000 );
				//MagicProc->ProcTime = MagicProc->ProcTime + int( OrgDB->MagicCol.MagicAttackDelay * 1000 );
			}
			//if( OrgDB->MagicCol.IsMagicAttackDelay_TargetDist && Target != NULL )
			if( OrgDB->MagicCol.IsMagicAttackDelay_TargetDist  )
			{
				int Len = int( Owner->Length3D( MagicProc->TargetX, MagicProc->TargetY ,MagicProc->TargetZ ) );
				if( Len > 1000 )
					Len = 1000;
				//MagicProc->ProcTime += Len * 5;
				ShootDelayTime += Len * 5;
			}

			if( MagicProc->BaseShootDelayTime > ShootDelayTime )
				ShootDelayTime = MagicProc->BaseShootDelayTime;

			//衝撞處理
			if( OrgDB->MagicCol.Flag.Dash && Owner->TempData.Attr.Effect.Stop == false )
			{
				//移到目標
				float NX , NZ , NY;
				Global::SearchAttackNode( Owner , MagicProc->TargetX , MagicProc->TargetY , MagicProc->TargetZ , Owner->TempData.BoundRadiusY , NX , NY , NZ );

				float Dx = NX - Owner->Pos()->X;
				float Dz = NZ - Owner->Pos()->Z;
				float Dir = Owner->CalDir( Dx , Dz );

				Owner->Pos( NX , NY , NZ , Dir );
				Owner->TempData.Move.Dx = 0;	
				Owner->TempData.Move.Dy = 0;	
				Owner->TempData.Move.Dz = 0;	
				Owner->TempData.Move.MoveType = EM_ClientMoveType_Normal;
				Owner->TempData.Move.IsLockSetPos = true;
				Owner->TempData.Move.LockSetPosX = Owner->BaseData.Pos.X;
				Owner->TempData.Move.LockSetPosZ = Owner->BaseData.Pos.Z;

				Owner->TempData.Move.CalMoveLen -= 100;
				
				BaseItemObject* OwnerClass = BaseItemObject::GetObj( Owner->GUID() );

				assert( OwnerClass != NULL );
				OwnerClass->Path()->SetPos( NX , NY , NZ );
				Owner->SetDelayAttack( ShootDelayTime / 100 + 5 );

				//通知周圍
				NetSrv_MoveChild::SendRange_SpecialMove( MagicProc->TargetID , EM_SpecialMoveType_Dash , Owner , MagicProc->MagicCollectID );
			}

			//清除施法消失的Buff
//			Owner->TempData.BackInfo.BuffClearTime.Spell = true;

			//if( strlen( MagicCollectOrgDB->MagicCol.ShootTargetScript )  )
			if( MagicCollectOrgDB->MagicCol.TargetType == EM_Releation_Plot )
			{
				m_ShootTarget.clear();
				LUA_VMClass::PCallByStrArg( MagicCollectOrgDB->MagicCol.ShootTargetScript , Owner->GUID() , MagicProc->TargetID );

				//vector<int> TargetGUIDList;
				//for( unsigned i = 0 ; i < m_ShootTarget.size() ; i++ )
				//	TargetGUIDList.push_back( m_ShootTarget[i] );
				NetSrv_MagicChild::BeginShootRange( Owner->GUID() , MagicProc->SerialID , m_ShootTarget );

				for( unsigned i = 0 ; i < m_ShootTarget.size() ; i++ )
				{
					//////////////////////////////////////////////////////////////////////////
					//目標計算
					//////////////////////////////////////////////////////////////////////////
					//NetSrv_MagicChild::BeginShootRange( Owner->GUID() , MagicProc->SerialID , TargetGUIDList );

					TempShootProcStruct* TempData = NEW(TempShootProcStruct);
					TempData->SendEndMagic = (i == 0 );
					TempData->OwnerID = Owner->GUID();
					TempData->MagicProc = *MagicProc;
					TempData->MagicProc.TargetID = m_ShootTarget[i];
					if( ShootDelayTime > 500 )
					{
						Global::Schedular()->Push( _ShootProc , ShootDelayTime , TempData , NULL );
					}
					else
					{
						_ShootProc( NULL , TempData );
					}
				}
			
			}
			else
			{
				//////////////////////////////////////////////////////////////////////////
				//目標計算
				//////////////////////////////////////////////////////////////////////////
				_Cal_TargetList( Owner , Target, MagicCollectOrgDB , MagicProc );
				vector<int> TargetGUIDList;
				for( unsigned i = 0 ; i < m_TargetList.size() ; i++ )
				{
					if( m_TargetList[i].IsDel || m_TargetList[i].HitType == EM_MAGIC_HIT_TYPE_CANCEL )
						continue;

					TargetGUIDList.push_back( m_TargetList[i].Role->GUID() );
				}

				NetSrv_MagicChild::BeginShootRange( Owner->GUID() , MagicProc->SerialID , TargetGUIDList );

				TempShootProcStruct* TempData = NEW(TempShootProcStruct);
				TempData->OwnerID = Owner->GUID();
				TempData->MagicProc = *MagicProc;
				TempData->SendEndMagic = true;

				if( ShootDelayTime > 500 )
				{
					Global::Schedular()->Push( _ShootProc , ShootDelayTime , TempData , NULL );
				}
				else
				{
					_ShootProc( NULL , TempData );
				}
			}
			

			MagicProc->SpellCount--;
			MagicProc->IsSuccess = true;
			if( MagicProc->SpellCount <= 0 )
			{
				MagicProc->State = EM_MAGIC_PROC_STATE_ENDSPELL;
				break;
			}
			MagicProc->ProcTime = MagicProc->ProcTime + MagicProc->NextSpellTime;
			break;

		}
	case EM_MAGIC_PROC_STATE_SPELL:
		{	
/*
			if( RoleDataEx::CheckTime( MagicProc->ProcTime ) == false )
				break;

			SpellProc_OnSpell( Owner );

			//消除施法消失的Buff
			Owner->ClearBuffByEvent( );


			if( MagicProc->State == EM_MAGIC_PROC_STATE_INTERRUPT  )
				break;

			MagicProc->ProcTime = MagicProc->ProcTime + MagicProc->NextSpellTime;

			MagicProc->SpellCount--;
			if( MagicProc->SpellCount <= 0 )
			{
				MagicProc->State = EM_MAGIC_PROC_STATE_ENDSPELL;
				break;
			}
			*/
		}break;
	case EM_MAGIC_PROC_STATE_INTERRUPT:				//施法中斷
		{
			GameObjDbStructEx*		OrgDB = Global::GetObjDB( MagicProc->MagicCollectID );

			if( MagicProc->IsSuccess  )
			{
				int ColdownTime = OrgDB->MagicCol.Coldown.Time;
				int AllColdownTime = OrgDB->MagicCol.Coldown.TimeAllMagic ;
				if(  OrgDB->MagicCol.Flag.AllColdown_Reference_Weapon != false )
					AllColdownTime += int( Owner->TempData.Attr.Fin.Main_ATKSpeed + 5 )/10;

				if( ColdownTime <= 0 )
				{
					if( OrgDB->MagicCol.Flag.ReferenceWeapon_Bow )
						ColdownTime += int( (Owner->TempData.Attr.Fin.Bow_ATKSpeed+5)/10 );
					else if( OrgDB->MagicCol.Flag.ReferenceWeapon_Throw )
						ColdownTime += int( (Owner->TempData.Attr.Fin.Throw_ATKSpeed+5)/10 );
				}
								
				Owner->DelUseItem();
				Owner->SetMagicColdown( OrgDB->MagicCol.Coldown.Class , OrgDB->MagicCol.Coldown.Type , ColdownTime , AllColdownTime );
				NetSrv_MagicChild::S_MagicEnd( Owner->GUID() , OrgDB->GUID , ColdownTime , AllColdownTime );

				Owner->TempData.Move.AddeDelay += int( OrgDB->MagicCol.MoveDelayTime*10);
			}
			else
				Owner->ClsUseItem();

			if(		MagicProc->IsSuccess == false 
				||	MagicProc->SpellCount > 1	)
			{
				NetSrv_MagicChild::CancleSpellMagicRange( Owner->GUID() , MagicProc->SerialID );
			}
			else
			{
				NetSrv_MagicChild::EndSpellRange( Owner->GUID() , MagicProc->SerialID );
			}
			MagicProc->State = EM_MAGIC_PROC_STATE_NORMAL;
			Owner->SetDelayAttack( int(Owner->TempData.Attr.Fin.Main_ATKSpeed) );
		}break;
	case EM_MAGIC_PROC_STATE_ENDSPELL:				//施法剛結束
		{
			RoleDataEx*				Target = Global::GetRoleDataByGUID( MagicProc->TargetID );
			if( Target == NULL )
				Target = Owner;
			GameObjDbStructEx*		OrgDB = Global::GetObjDB( MagicProc->MagicCollectID );
			if( MagicProc->IsSuccess )
			{

				int ColdownTime = OrgDB->MagicCol.Coldown.Time;
				int AllColdownTime = OrgDB->MagicCol.Coldown.TimeAllMagic ;
				if(  OrgDB->MagicCol.Flag.AllColdown_Reference_Weapon != false )
					AllColdownTime += int( Owner->TempData.Attr.Fin.Main_ATKSpeed + 5 )/10;

				if( ColdownTime <= 0 )
				{
					if( OrgDB->MagicCol.Flag.ReferenceWeapon_Bow )
						ColdownTime += int( Owner->TempData.Attr.Fin.Bow_ATKSpeed+5)/10;
					else if( OrgDB->MagicCol.Flag.ReferenceWeapon_Throw )
						ColdownTime += int( Owner->TempData.Attr.Fin.Throw_ATKSpeed+5)/10;
				}

				if( OrgDB->MagicCol.Flag.ReferenceWeapon_Bow &&  Owner->IsPlayer() != false  )
				{
					switch( Owner->TempData.Attr.BowWeaponType )
					{
					case EM_Weapon_Bow:				// 遠程武器 弓
						Owner->AddSkillValue( Target->Level() , EM_SkillValueType_Bow , float( 3 * Owner->TempData.Sys.KillRate ) );
						break;
					case EM_Weapon_CossBow:			// 遠程武器 十字弓
						Owner->AddSkillValue( Target->Level() , EM_SkillValueType_CossBow , float( 3 * Owner->TempData.Sys.KillRate ) );
						break;
					case EM_Weapon_Gun:				// 遠程武器 槍
						Owner->AddSkillValue( Target->Level() , EM_SkillValueType_Gun , float( 3 * Owner->TempData.Sys.KillRate ) );
						break;
					}

					//int DecDurable = Owner->TempData.Sys.KillRate * 2 ;
					//Owner->EqDurableProcess( EM_EQWearPos_Bow , DecDurable );
				}

				
				Owner->DelUseItem();
				Owner->SetMagicColdown( OrgDB->MagicCol.Coldown.Class , OrgDB->MagicCol.Coldown.Type , ColdownTime , AllColdownTime );
				NetSrv_MagicChild::S_MagicEnd( Owner->GUID() , OrgDB->GUID , ColdownTime , OrgDB->MagicCol.Coldown.TimeAllMagic );

				if( OrgDB->MagicCol.IsStopAttack != false )
					Owner->StopAttack();

			}
			else
				Owner->ClsUseItem();
			NetSrv_MagicChild::EndSpellRange( Owner->GUID() , MagicProc->SerialID );
			MagicProc->State = EM_MAGIC_PROC_STATE_NORMAL;
			Owner->SetDelayAttack( int(Owner->TempData.Attr.Fin.Main_ATKSpeed) );
			Owner->TempData.Move.AddeDelay += int( OrgDB->MagicCol.MoveDelayTime*10);

			if( OrgDB->MagicCol.NextMagicID != 0 )
			{
				MagicProcessClass::SpellMagic( Owner->GUID() , MagicProc->TargetID  , MagicProc->TargetX , MagicProc->TargetY , MagicProc->TargetZ , OrgDB->MagicCol.NextMagicID , MagicProc->MagicLv );
			}
		}
		break;
	}

	return Ret;
}
//----------------------------------------------------------------------------------------
bool MagicProcessClass::Magic_Empty	( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc )
{
	return false;
}
//----------------------------------------------------------------------------------------
//加血的法術
bool MagicProcessClass::Magic00_AddHP			(		RoleDataEx*		Owner 
												 ,		GameObjDbStructEx*	MagicBaseDB	
												 ,		MagicProcInfo* MagicProc	)
{
	static vector< MagicAtkEffectInfoStruct > List;
	List.clear();

	GameObjDbStructEx*	MagicCollectOrgDB = Global::GetObjDB( MagicProc->MagicCollectID );

	if( MagicCollectOrgDB == NULL )
		return false;


	MagicAttackStruct&	Atk = MagicBaseDB->MagicBase.Attack;
	int     i;

	float	OrgPower = Atk.DmgPower + Atk.DmgPower * Atk.DmgPower_SkillLVArg * MagicProc->MagicLv /100;
	float   PurePower;

	float	SpellTime = MagicCollectOrgDB->MagicCol.SpellTime;
	if( SpellTime < 1 || MagicCollectOrgDB->MagicCol.IsFixDmgEffect )
		SpellTime = 1;

	float FixPowerRate = 1.0f;

	if( MagicCollectOrgDB->MagicCol.RangeType != EM_MagicEffect_Target )
		FixPowerRate = 0.6f;

	float FinFixPowerRate = 1.0f; 
	if( MagicCollectOrgDB->MagicCol.IsShareDamage )
	{
		FinFixPowerRate = __max( MagicCollectOrgDB->MagicCol.DecreaseArg/100.0f ,  1.0f / m_ProcTargetList.size() );		
	}


	//switch( MagicBaseDB->MagicBase.EffectType )
	switch( MagicBaseDB->MagicBase.AtkCalType )
	{
	case EM_MagicAttackCalBase_Physics:		//物理計算
		{	
			//秒商值
			float DPS =  Owner->TempData.Attr.Fin.Main_DMG * 10 * SpellTime / Owner->TempData.Attr.Fin.Main_ATKSpeed;
			if( MagicCollectOrgDB->MagicCol.IsIgnoreDmgEffect != false )
				PurePower =		OrgPower;
			else
				PurePower =		OrgPower * DPS;

			if( PurePower > 0)
			{
				PurePower = PurePower;
				if( MagicBaseDB->MagicBase.Setting.IgnoreSpellCureMagicClear == false )
					Owner->TempData.BackInfo.BuffClearTime.Spell_Cure = true;
			}
			else
			{
				PurePower = PurePower -	DPS * Owner->FixArg().Dmg_MagArg * FixPowerRate;

				if( MagicBaseDB->MagicBase.Setting.IgnoreSpellAttackMagicClear == false )
					Owner->TempData.BackInfo.BuffClearTime.Spell_PhyAttack = true;
			}

			if(	MagicBaseDB->MagicBase.EffectType == EM_Magic_Magic	 )
				PurePower *= Owner->TempData.Attr.MagicPow( MagicBaseDB->MagicBase.MagicType ) ;

			for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
			{
				RoleDataEx* Other = m_ProcTargetList[i]->Role;
				float  Power = 	PurePower * ( 1 - ( Atk.Rand - Atk.Rand * 2 * float( rand() ) / 0x7fff ) /100 );
				List.push_back( CalPhyDamage(  Owner , m_ProcTargetList[i] , Power , MagicCollectOrgDB , MagicBaseDB , MagicProc->MagicLv , FinFixPowerRate ) );
				if( PurePower < 0 
					&&	Other->TempData.Attr.Effect.BadMagicInvincible == false
					&&	Other->TempData.Attr.Effect.BadPhyInvincible == false		)
					Other->CalHate( Owner , 0 , 1 );
				if( MagicCollectOrgDB->MagicCol.IsShareDamage == false )
					PurePower = PurePower * (100 - MagicCollectOrgDB->MagicCol.DecreaseArg ) /100 ;

			}

		}
		break;
	case EM_MagicAttackCalBase_Physics_Point:		//物理計算
		{	
			PurePower =		OrgPower;

			if( PurePower > 0)
			{
				PurePower = PurePower;
				if( MagicBaseDB->MagicBase.Setting.IgnoreSpellCureMagicClear == false )
					Owner->TempData.BackInfo.BuffClearTime.Spell_Cure = true;

			}
			else
			{
				if( MagicCollectOrgDB->MagicCol.IsIgnoreDmgEffect != false )
				{
				//	PurePower =		PurePower;
				}
				else if( MagicCollectOrgDB->MagicCol.IsFixDmgEffect != false )
					PurePower = PurePower -	Owner->TempData.Attr.Fin.Main_DMG * Owner->FixArg().Dmg_MagArg*FixPowerRate;
				else
					PurePower = PurePower -	Owner->TempData.Attr.Fin.Main_DMG * Owner->FixArg().Dmg_MagArg*FixPowerRate * SpellTime;

				if( MagicBaseDB->MagicBase.Setting.IgnoreSpellAttackMagicClear == false )
					Owner->TempData.BackInfo.BuffClearTime.Spell_PhyAttack = true;
			}

			if(	MagicBaseDB->MagicBase.EffectType == EM_Magic_Magic	 )
				PurePower *= Owner->TempData.Attr.MagicPow( MagicBaseDB->MagicBase.MagicType ) ;

			
			for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
			{
				RoleDataEx* Other = m_ProcTargetList[i]->Role;
				float  Power = 	PurePower*( 1 - ( Atk.Rand - Atk.Rand * 2 * float( rand() ) / 0x7fff ) /100 );
				List.push_back( CalPhyDamage(  Owner , m_ProcTargetList[i] , Power , MagicCollectOrgDB , MagicBaseDB , MagicProc->MagicLv , FinFixPowerRate ) );

				if( PurePower < 0 
					&&	Other->TempData.Attr.Effect.BadMagicInvincible == false
					&&	Other->TempData.Attr.Effect.BadPhyInvincible == false					)
					Other->CalHate( Owner , 0 , 1 );

				if( MagicCollectOrgDB->MagicCol.IsShareDamage == false )
					PurePower = PurePower * (100 - MagicCollectOrgDB->MagicCol.DecreaseArg ) /100 ;
			}

		}
		break;
	case EM_MagicAttackCalBase_Magic:		//法術計算
		{
			PurePower = OrgPower ;//* Owner->TempData.Attr.MagicPow( MagicBaseDB->MagicBase.MagicType );
			
			float ExRate = 1;
			if( Owner->IsPlayer() )
			{
				//ExRate = 0.5 + ( MagicProc->MagicLv / Owner->Level() ) /2;
				ExRate = 0.6f + abs( float(MagicProc->MagicLv) * 0.01f );
			}

			if( PurePower > 0)
			{
				if( MagicCollectOrgDB->MagicCol.IsIgnoreDmgEffect != false )
				{
				}
				else if( MagicCollectOrgDB->MagicCol.IsFixDmgEffect != false )
					PurePower = PurePower +	ExRate * Owner->TempData.Attr.Fin.MDMG * Owner->FixArg().Dmg_HealArg;
				else
					PurePower = PurePower +	ExRate * Owner->TempData.Attr.Fin.MDMG * Owner->FixArg().Dmg_HealArg * SpellTime;

				if( MagicBaseDB->MagicBase.Setting.IgnoreSpellCureMagicClear == false )
					Owner->TempData.BackInfo.BuffClearTime.Spell_Cure = true;
			}
			else
			{
				if( MagicCollectOrgDB->MagicCol.IsIgnoreDmgEffect != false )
				{
				}
				else if( MagicCollectOrgDB->MagicCol.IsFixDmgEffect != false )
					PurePower = PurePower -	ExRate * Owner->TempData.Attr.Fin.MDMG * Owner->FixArg().MDmg_MagArg * FixPowerRate;
				else
					PurePower = PurePower -	ExRate * Owner->TempData.Attr.Fin.MDMG * Owner->FixArg().MDmg_MagArg * SpellTime*FixPowerRate;

				if(		MagicBaseDB->MagicBase.Setting.IgnoreSpellAttackMagicClear == false )
					Owner->TempData.BackInfo.BuffClearTime.Spell_MagicAttack = true;
			}

			PurePower *= Owner->TempData.Attr.MagicPow( MagicBaseDB->MagicBase.MagicType ) ;

			if( OrgPower < 0 )
			{
				PurePower -= Owner->TempData.Attr.Mid.Body[EM_WearEqType_MDmgPoint];
				if( PurePower > -1 )
					PurePower = -1;
			}

			for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
			{
				RoleDataEx* Other = m_ProcTargetList[i]->Role;
				float  Power = 	PurePower* ( 1 - ( Atk.Rand - Atk.Rand * 2 * float( rand() ) / 0x7fff ) / 100 );

				List.push_back( CalMagicDamage(  Owner , m_ProcTargetList[i] , Power , MagicCollectOrgDB , MagicBaseDB , MagicProc->MagicLv , FinFixPowerRate ) );
				if(		PurePower < 0 
					&&	Other->TempData.Attr.Effect.BadMagicInvincible == false
					&&	Other->TempData.Attr.Effect.BadPhyInvincible == false					)
					Other->CalHate( Owner , 0 , 1 );
				if( MagicCollectOrgDB->MagicCol.IsShareDamage == false )
					PurePower = PurePower * (100 - MagicCollectOrgDB->MagicCol.DecreaseArg ) /100 ;
			}
//			if( MagicBaseDB->MagicBase.Setting.IgnoreSpellAttackMagicClear == false )
//				Owner->TempData.BackInfo.BuffClearTime.Spell_MagicAttack = true;
		}
		break;
	case EM_MagicAttackCalBase_Throw :		//投擲計算	
		{

			PurePower = OrgPower * Owner->TempData.Attr.Fin.Throw_DMG ;
			for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
			{
				RoleDataEx* Other = m_ProcTargetList[i]->Role;
				float  Power = 	PurePower* ( 1 - ( Atk.Rand - Atk.Rand * 2 * float( rand() ) / 0x7fff ) / 100 );
				float PowerRate = 1;
				if( PowerRate < 0.2f )
					PowerRate = 0.2f;
				Power = Power * PowerRate;
				List.push_back( CalPhyDamage(  Owner , m_ProcTargetList[i] , Power , MagicCollectOrgDB , MagicBaseDB , MagicProc->MagicLv , FinFixPowerRate ) );

				if(		PurePower < 0 
					&&	Other->TempData.Attr.Effect.BadMagicInvincible == false
					&&	Other->TempData.Attr.Effect.BadPhyInvincible == false					)
					Other->CalHate( Owner , 0 , 1 );

				if( MagicCollectOrgDB->MagicCol.IsShareDamage == false )
					PurePower = PurePower * (100 - MagicCollectOrgDB->MagicCol.DecreaseArg ) /100 ;
				if( MagicBaseDB->MagicBase.Setting.IgnoreSpellCureMagicClear == false )
					Owner->TempData.BackInfo.BuffClearTime.Spell_PhyAttack = true;
			}

		}
		break;
	case EM_MagicAttackCalBase_Shoot:		//射擊計算
		PurePower = OrgPower * ( Owner->TempData.Attr.Fin.Bow_DMG * 10 / Owner->TempData.Attr.Fin.Bow_ATKSpeed );

		if( PurePower > 0)
			PurePower = PurePower;
		else
		{
			PurePower = PurePower -	Owner->TempData.Attr.Fin.Bow_DMG *  Owner->FixArg().Dmg_MagArg * FixPowerRate;
		}
		for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
		{
			RoleDataEx* Other = m_ProcTargetList[i]->Role;
			float  Power = 	PurePower* ( 1 - ( Atk.Rand - Atk.Rand * 2 * float( rand() ) / 0x7fff )/ 100 );
			float PowerRate = 1;
			Power = Power * PowerRate;
			List.push_back( CalPhyDamage(  Owner , m_ProcTargetList[i] , Power , MagicCollectOrgDB , MagicBaseDB , MagicProc->MagicLv , FinFixPowerRate ) );

			if(		PurePower < 0 
				&&	Other->TempData.Attr.Effect.BadMagicInvincible == false
				&&	Other->TempData.Attr.Effect.BadPhyInvincible == false					)
				Other->CalHate( Owner , 0 , 1 );

			if( MagicCollectOrgDB->MagicCol.IsShareDamage == false )
				PurePower = PurePower * (100 - MagicCollectOrgDB->MagicCol.DecreaseArg ) /100 ;
		}
		if( MagicBaseDB->MagicBase.Setting.IgnoreSpellAttackMagicClear == false )
			Owner->TempData.BackInfo.BuffClearTime.Spell_PhyAttack = true;
		break;
	case EM_MagicAttackCalBase_Fix:			//定值( 不計算 )
		{
			PurePower = OrgPower;
			if( PurePower > 0 )
			{
				if( MagicBaseDB->MagicBase.Setting.IgnoreSpellCureMagicClear == false )
					Owner->TempData.BackInfo.BuffClearTime.Spell_Cure = true;
			}

			MagicAtkEffectInfoStruct Temp;
			float	AddFixPower	= MagicBaseDB->MagicBase.Attack.FixValue 
				* Owner->GetValue( MagicBaseDB->MagicBase.Attack.FixType )
				* ( 1 + MagicProc->MagicLv * MagicBaseDB->MagicBase.Attack.FixDmg_SkillLVArg / 100 );

			
			for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )			
			{
				RoleDataEx* Target = m_ProcTargetList[i]->Role;
				Temp.GItemID = m_ProcTargetList[i]->Role->GUID();
				switch( m_ProcTargetList[i]->HitType )
				{
				case EM_MAGIC_HIT_TYPE_HIT:			//命中
				case EM_MAGIC_HIT_TYPE_CRITICAL:	//致命一擊
					{
						if( m_ProcTargetList[i]->Role->TempData.Attr.DisableMagicSet & MagicBaseDB->MagicBase.MagicGroupSet )
						{
							Temp.Value = 0;
							Temp.Result = EM_MagicAtkResult_Absorb;
						}
						else
						{
							Temp.Value = PurePower + AddFixPower;
							if( Temp.Value < 0)
							{
								if( MagicBaseDB->MagicBase.SettingEx.IgnoreMagicAbsorb == false )
									Temp.Value = Target->TempData.Attr.CalMagicDMG(Temp.Value );


								if( MagicCollectOrgDB->MagicCol.RangeType != EM_MagicEffect_Target )
								{//ae魔法時

									Temp.Value = Temp.Value * Owner->TempData.Attr.Mid.BodyRate[ EM_WearEqType_AEMagicPowerRate ] * Target->TempData.Attr.Mid.BodyRate[ EM_WearEqType_AEMagicDefRate ];
								}
								else
								{
								}
							}
							Temp.Result = EM_MagicAtkResult_Normal;
						}
					}
					break;
				case EM_MAGIC_HIT_TYPE_MISS:		//MISS
					Temp.Value = 0;
					Temp.Result = EM_MagicAtkResult_Miss;
					break;
				case EM_MAGIC_HIT_TYPE_DODGE:		//MISS
					Temp.Value = 0;
					Temp.Result = EM_MagicAtkResult_Dodge;
					break;
				case EM_MAGIC_HIT_TYPE_CANCEL:
					Temp.Value = 0;
					Temp.Result = EM_MagicAtkResult_Cancel;
					break;
				case EM_MAGIC_HIT_TYPE_NO_EFFECT:	//無效
					Temp.Value = 0;
					Temp.Result = EM_MagicAtkResult_NoEffect;
					break;
				}
				if(		m_ProcTargetList[i]->Role->IsPlayer() && Owner->IsPlayer() 
					&&	Temp.Value <0 )
				{
					Temp.Value /= 2;
				}

				if( m_ProcTargetList[i]->Role->IsDead() )
					Temp.Value = 0;

				int Damage = int(Temp.Value * -1 * FinFixPowerRate );
				if( MagicCollectOrgDB->MagicCol.IsIgnoreInvincible == false )
					m_ProcTargetList[i]->Role->MagicShieldAbsorbProc( Owner , MagicBaseDB , Damage );
				Temp.OrgValue = float( Damage * -1 );
				if( Damage != 0 )
				{
					Temp.OrgValue = Temp.Value = float( Damage * -1 );
					Temp.Result = EM_MagicAtkResult_Normal;	
				}
				else
				{
					Temp.OrgValue = Temp.Value = 0;
					Temp.Result = EM_MagicAtkResult_Absorb;	
				}

				List.push_back( Temp );

				if( PurePower < 0 )
					Target->CalHate( Owner , 0 , 1 );

				if( MagicCollectOrgDB->MagicCol.IsShareDamage == false )
					PurePower = PurePower * (100 - MagicCollectOrgDB->MagicCol.DecreaseArg ) /100 ;

			}
		}		
		break;

	case EM_MagicAttackCalBase_HolyHeal:	//神聖治療計算( 負值表示Miss  )
		{
			PurePower = OrgPower;

			float MNDPower = Owner->TempData.Attr.Fin.MND * 0.3f *SpellTime;
			if( PurePower * 5 < MNDPower )
				PurePower = PurePower * 6;
			else
				PurePower = PurePower +  MNDPower;

			if( MagicCollectOrgDB->MagicCol.IsIgnoreDmgEffect != false )
			{
			}
			else if( MagicCollectOrgDB->MagicCol.IsFixDmgEffect != false )
				PurePower = PurePower +	Owner->TempData.Attr.Fin.MDMG * Owner->FixArg().Dmg_HealArg;
			else
				PurePower = PurePower +	Owner->TempData.Attr.Fin.MDMG * Owner->FixArg().Dmg_HealArg*SpellTime;


			PurePower = PurePower * ( Owner->TempData.Attr.Fin.MPow * Owner->TempData.Attr.Mid.BodyRate[EM_WearEqType_HealPowerRate] ) + Owner->TempData.Attr.Mid.Body[EM_WearEqType_HealPoint] * SpellTime;

			if( PurePower <= 0 )
				break;

			//需撿查是否為不死系
			for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
			{
				RoleDataEx* Target = m_ProcTargetList[i]->Role;

				float  Power = 	PurePower* ( 1 - ( Atk.Rand - Atk.Rand * 2 * float( rand() ) / 0x7fff ) / 100 );

				if( MagicBaseDB->MagicBase.SettingEx.IgnoreMagicAbsorb == false )
				{
					Power = Power * ( 1 + Target->TempData.Attr.Mid.Body[EM_WearEqType_HealAbsorbRate]/100 )
						+ Target->TempData.Attr.Mid.Body[EM_WearEqType_HealAbsorb];
				}

				if( Power < 0 )
					Power = 0;

				if( Target->BaseData.Race == EM_Race_Undead )
					Power *= -1;
				
				if( Target->BaseData.Mode.Fight == false && Power < 0 )
				{
					Power = 0;
				}

				List.push_back( CalMagicDamage(  Owner , m_ProcTargetList[i] , Power , MagicCollectOrgDB , MagicBaseDB , MagicProc->MagicLv , FinFixPowerRate ) );
				if( PurePower < 0 )
					Target->CalHate( Owner , 0 , 1 );

				if( MagicCollectOrgDB->MagicCol.IsShareDamage == false )
					PurePower = PurePower * (100 - MagicCollectOrgDB->MagicCol.DecreaseArg ) /100 ;
			}
			if( MagicBaseDB->MagicBase.Setting.IgnoreSpellCureMagicClear == false )
				Owner->TempData.BackInfo.BuffClearTime.Spell_Cure = true;
		}
		break;
	case EM_MagicAttackCalBase_DarkHeal:	//不死系療計算( 負值表示Miss  )
		{
			if( MagicCollectOrgDB->MagicCol.IsIgnoreDmgEffect != false )
				PurePower = OrgPower;
			else if( MagicCollectOrgDB->MagicCol.IsFixDmgEffect != false )
				PurePower = OrgPower +	Owner->TempData.Attr.Fin.MDMG * Owner->FixArg().MDmg_MagArg;
			else
				PurePower = OrgPower +	Owner->TempData.Attr.Fin.MDMG * Owner->FixArg().MDmg_MagArg*SpellTime;

			PurePower = PurePower * Owner->TempData.Attr.Fin.MPow;
			if( PurePower <= 0 )
				break;
			//需撿查是否為不死系

			for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
			{
				RoleDataEx* Target = m_ProcTargetList[i]->Role;
				float  Power = 	PurePower* ( 1 - ( Atk.Rand - Atk.Rand * 2 * float( rand() ) / 0x7fff ) / 100 );
				
				if( Target->BaseData.Race != EM_Race_Undead )
					Power *= -1;
				
				List.push_back( CalMagicDamage(  Owner , m_ProcTargetList[i] , Power , MagicCollectOrgDB , MagicBaseDB , MagicProc->MagicLv , FinFixPowerRate ) );

				if( PurePower < 0 )
					Target->CalHate( Owner , 0 , 1 );

				if( MagicCollectOrgDB->MagicCol.IsShareDamage == false )
					PurePower = PurePower * (100 - MagicCollectOrgDB->MagicCol.DecreaseArg ) /100 ;
			}
			if( MagicBaseDB->MagicBase.Setting.IgnoreSpellCureMagicClear == false )
				Owner->TempData.BackInfo.BuffClearTime.Spell_Cure = true;
		}
		break;
	case EM_MagicAttackCalBase_SpecialAction:
		{
			MagicAtkEffectInfoStruct	Effect;

			for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
			{
				RoleDataEx* Target = m_ProcTargetList[i]->Role;
				Effect.GItemID  = Target->GUID();
				Effect.Value	= 0;
				
				if( m_ProcTargetList[i]->HitType != EM_MAGIC_HIT_TYPE_HIT )
					Effect.Result	= EM_MagicAtkResult_Miss;
				else
					Effect.Result	= EM_MagicAtkResult_SpecialAction;

				//Effect.Result	= EM_MagicAtkResult_SpecialAction;
				Effect.OrgValue = 0;
				List.push_back( Effect );
			}

		}
		break;
	case EM_MagicAttackCalBase_FixPer:
		{
			PurePower = OrgPower;
			if( PurePower > 0 )
			{
				if( MagicBaseDB->MagicBase.Setting.IgnoreSpellCureMagicClear == false )
					Owner->TempData.BackInfo.BuffClearTime.Spell_Cure = true;
			}
			MagicAtkEffectInfoStruct Temp;
			for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )			
			{
				RoleDataEx* Target = m_ProcTargetList[i]->Role;
				Temp.GItemID = Target->GUID();
				switch( m_ProcTargetList[i]->HitType )
				{
				case EM_MAGIC_HIT_TYPE_HIT:			//命中
				case EM_MAGIC_HIT_TYPE_CRITICAL:	//致命一擊
					if( Target->TempData.Attr.DisableMagicSet & MagicBaseDB->MagicBase.MagicGroupSet )
					{
						Temp.Value = 0;
						Temp.Result = EM_MagicAtkResult_Absorb;
					}
					else
					{
						switch( Atk.AtkType )
						{
						case EM_MagicAttackType_HP:
							//QQ 要把魔防吸收率算進去
							//分成物理和法術攻擊
							if( MagicBaseDB->MagicBase.Setting.FixDotPower )
							{
								Temp.Value = Target->TempData.Attr.Fin.MaxHP * PurePower /100;
							}
							else if(MagicBaseDB->MagicBase.EffectType == EM_Magic_Phy)
							{//物理
								if( MagicBaseDB->MagicBase.SettingEx.IgnoreMagicAbsorb == false )
									Temp.Value = Target->TempData.Attr.CalWeaponDMG(Target->TempData.Attr.Fin.MaxHP * PurePower) / 100;
								else
									Temp.Value = Target->TempData.Attr.Fin.MaxHP * PurePower / 100;

							}
							else
							{//法術
								if( MagicBaseDB->MagicBase.SettingEx.IgnoreMagicAbsorb == false )
									Temp.Value = Target->TempData.Attr.CalMagicDMG(Target->TempData.Attr.Fin.MaxHP*PurePower) / 100;	
								else
									Temp.Value = Target->TempData.Attr.Fin.MaxHP * PurePower / 100;

								if( MagicCollectOrgDB->MagicCol.RangeType != EM_MagicEffect_Target )
								{//ae魔法時
									Temp.Value = Temp.Value * Owner->TempData.Attr.Mid.BodyRate[ EM_WearEqType_AEMagicPowerRate ] * Target->TempData.Attr.Mid.BodyRate[ EM_WearEqType_AEMagicDefRate ];
								}
								else
								{
								}
							}
							
							break;
						case EM_MagicAttackType_MP:
							Temp.Value = Target->TempData.Attr.Fin.MaxMP * PurePower /100;
							break;
						default:

							Temp.Value = PurePower;
							break;
						}

						Temp.Result = EM_MagicAtkResult_Normal;
					}

					break;
				case EM_MAGIC_HIT_TYPE_MISS:		//MISS
					Temp.Value = 0;
					Temp.Result = EM_MagicAtkResult_Miss;
					break;
				case EM_MAGIC_HIT_TYPE_DODGE:		//MISS
					Temp.Value = 0;
					Temp.Result = EM_MagicAtkResult_Dodge;
					break;
				case EM_MAGIC_HIT_TYPE_CANCEL:
					Temp.Value = 0;
					Temp.Result = EM_MagicAtkResult_Cancel;
					break;
				case EM_MAGIC_HIT_TYPE_NO_EFFECT:	//無效
					Temp.Value = 0;
					Temp.Result = EM_MagicAtkResult_NoEffect;
					break;
				}

				if( m_ProcTargetList[i]->Role->IsDead() )
					Temp.Value = 0;

				int Damage = int(Temp.Value * -1 );
				if(  Atk.AtkType  == EM_MagicAttackType_HP )
				{
					if( MagicCollectOrgDB->MagicCol.IsIgnoreInvincible == false )
						Target->MagicShieldAbsorbProc( Owner , MagicBaseDB , Damage );
				}
				Temp.OrgValue = float( Damage * -1 );
				if( Damage != 0 )
				{
					Temp.OrgValue = Temp.Value = float( Damage * -1 );
					Temp.Result = EM_MagicAtkResult_Normal;	
				}
				else
				{
					Temp.OrgValue = Temp.Value = 0;
					Temp.Result = EM_MagicAtkResult_Absorb;	
				}

				//Temp.OrgValue = Temp.Value;
				Temp.Value *= FinFixPowerRate;
				List.push_back( Temp );

				if( PurePower < 0 )
					Target->CalHate( Owner , 0 , 1 );

				if( MagicCollectOrgDB->MagicCol.IsShareDamage == false )
					PurePower = PurePower * (100 - MagicCollectOrgDB->MagicCol.DecreaseArg ) /100 ;
			}
		}		
		break;

		case EM_MagicAttackCalBase_MagicEx:		//法術計算
		{
			PurePower = OrgPower;

			if( PurePower > 0)
			{
				if( MagicBaseDB->MagicBase.Setting.IgnoreSpellCureMagicClear == false )
					Owner->TempData.BackInfo.BuffClearTime.Spell_Cure = true;
			}
			else
			{
				PurePower = PurePower *	Owner->TempData.Attr.Fin.MDMG * Owner->FixArg().MDmg_MagArg * FixPowerRate;

				if(		MagicBaseDB->MagicBase.Setting.IgnoreSpellAttackMagicClear == false )
					Owner->TempData.BackInfo.BuffClearTime.Spell_MagicAttack = true;
			}

			PurePower *= Owner->TempData.Attr.MagicPow( MagicBaseDB->MagicBase.MagicType );
			if( OrgPower < 0 )
			{
				PurePower -= Owner->TempData.Attr.Mid.Body[EM_WearEqType_MDmgPoint];
				if( PurePower > -1 )
					PurePower = -1;
			}

			for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
			{
				RoleDataEx* Other = m_ProcTargetList[i]->Role;
				float  Power = 	PurePower* ( 1 - ( Atk.Rand - Atk.Rand * 2 * float( rand() ) / 0x7fff ) / 100 );

				List.push_back( CalMagicDamage(  Owner , m_ProcTargetList[i] , Power , MagicCollectOrgDB , MagicBaseDB , MagicProc->MagicLv , FinFixPowerRate ) );
				if(		PurePower < 0 
					&&	Other->TempData.Attr.Effect.BadMagicInvincible == false
					&&	Other->TempData.Attr.Effect.BadPhyInvincible == false					)
					Other->CalHate( Owner , 0 , 1 );
				if( MagicCollectOrgDB->MagicCol.IsShareDamage == false )
					PurePower = PurePower * (100 - MagicCollectOrgDB->MagicCol.DecreaseArg ) /100 ;
			}
		}
		break;

		default:
			return true;
	}


	//////////////////////////////////////////////////////////////////////////	

	for( unsigned i =  0  ; i < List.size() ; i++ )
	{	
		RoleDataEx* Target = m_ProcTargetList[i]->Role;
		char	LuaScriptBuf[512];	
		if( strlen( MagicBaseDB->MagicBase.EndUseLua ) != 0 )
		{
			sprintf_s( LuaScriptBuf , sizeof(LuaScriptBuf) , "%s(%d)" , MagicBaseDB->MagicBase.EndUseLua , List[i].Result );
			LUA_VMClass::PCallByStrArg( LuaScriptBuf , Owner->GUID() ,Target->GUID() );
		}

		if(		List[i].Result == EM_MagicAtkResult_Cancel )
		{
			if( m_IsMissErase  )
				m_ProcTargetList[i]->IsDel = true;
		}
		else if(		List[i].Result == EM_MagicAtkResult_Absorb 
				||	List[i].Result == EM_MagicAtkResult_NoEffect
				||	List[i].Result == EM_MagicAtkResult_Miss 
				||	List[i].Result == EM_MagicAtkResult_ShieldBlock
				||	List[i].Result == EM_MagicAtkResult_Dodge
				||	( List[i].Value == 0 &&  List[i].Result != EM_MagicAtkResult_SpecialAction ) )
		{
			if( m_IsMissErase  )
				m_ProcTargetList[i]->IsDel = true;
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
		else
		{
		
			//int HateValue = MagicBaseDB->MagicBase.HateCost * ( MagicBaseDB->MagicBase.HateCost/100 ) 
			int HateValue = int( MagicBaseDB->MagicBase.HateCost + MagicBaseDB->MagicBase.HateCost * Atk.DmgPower_SkillLVArg * MagicProc->MagicLv /100 );
			m_ProcTargetList[i]->Role->CalHate( Owner , 0 , HateValue );
			m_ProcTargetList[i]->IsSpellOneMagic = true;


			// Server 產生傷害事件
			if( Target->IsPlayer() && List[i].Value < 0 )
			{
				CNetSrv_Script_Child::OnEvent_RoleBeHit( Target );
			}

			//如果目標是NPC處發劇情事件	(攻擊劇情)
			vector<MyVMValueStruct> RetList;
			if( Target->IsNPC() )
			{

				char	LuaScriptBuf[512];
				GameObjDbStructEx* NpcDB = Global::GetObjDB( Target->BaseData.ItemInfo.OrgObjID );
				if( NpcDB->IsNPC() && strlen(NpcDB->NPC.LuaEvent_OnAttackMagic) != 0 )
				{
					sprintf_s( LuaScriptBuf , sizeof(LuaScriptBuf) , "%s(%d,%f)" , NpcDB->NPC.LuaEvent_OnAttackMagic , MagicBaseDB->GUID , List[i].Value );
					if( LUA_VMClass::PCallByStrArg( LuaScriptBuf , Owner->GUID() , Target->GUID() , &RetList , 1 ) )
					{
						MyVMValueStruct& Temp = RetList[0];
						if( RetList.size() == 1 )
							List[i].Value = float( Temp.Number );
					}
				}
			}

		}
	}

	//////////////////////////////////////////////////////////////////////////

	if( List.size() != 0 )
	{
		if( List.size() != m_ProcTargetList.size() )
		{
			Print(LV4,"Magic00_AddHP" , "List.Size()=%d m_ProcTargetList.size()=%d MagicBaseDB->GUID=%d", List.size() , m_ProcTargetList.size() , MagicBaseDB->GUID );
		}

		switch( Atk.AtkType )
		{
		case EM_MagicAttackType_HP:
			for( int i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
			{
				RoleDataEx* Target = m_ProcTargetList[i]->Role;
				if( i >= List.size() )
					break;
				MagicAtkEffectInfoStruct& Temp = List[i];

				if( MagicCollectOrgDB->MagicCol.IsDiabledPvPRule != false && Owner->IsPlayer() && Target->IsPlayer() )
					Global::AddHP( Target , Target , Temp.Value , MagicBaseDB->MagicBase.Attack.HateRate );
				else
					Global::AddHP( Target , Owner , Temp.Value , MagicBaseDB->MagicBase.Attack.HateRate );
				
				Owner->TempData.BackInfo.LastSkillHitPoint = int( Temp.Value );
//				if( Temp.Value < 0 )
//					Target->TempData.SkillComboPower += int( Temp.Value * Global::St()->ComboProc.GetPowerRate( Target->TempData.SkillComboID ) );
				if( m_ProcTargetList[i]->IsDel == false )
					Global::St()->ComboProc.CalComboPower( Target , int( Temp.Value ) );
				/*

				if( int( Temp.Value ) >= 0  )
					continue;

				if( Target->IsNPC() )
					continue;

				int DecDurable = Target->TempData.Sys.KillRate * 2;
				//裝備耐久計算
				for( int i = EM_EQWearPos_Head ; i < EM_EQWearPos_MaxCount ; i++ )
				{
					Target->EqDurableProcess( (EQWearPosENUM) i , DecDurable );
				}
*/
				//////////////////////////////////////////////////////////////////////////
			}  
			break;
		case EM_MagicAttackType_MP:
			for( int i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
			{
				RoleDataEx* Target = m_ProcTargetList[i]->Role;
				if( i >= List.size() )
					break;
				MagicAtkEffectInfoStruct& Temp = List[i];

				Global::AddMP( Target , Owner , Temp.Value , MagicBaseDB->MagicBase.Attack.HateRate );

				Owner->TempData.BackInfo.LastSkillHitPoint = int( Temp.Value );
				//if( Temp.Value < 0 )
				//	Target->TempData.SkillComboPower += int( Temp.Value * Global::St()->ComboProc.GetPowerRate( Target->TempData.SkillComboID ) );
				Global::St()->ComboProc.CalComboPower( Target , int( Temp.Value ) );

			}  
			break;
		case EM_MagicAttackType_SP_Warrior:
			for( int i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
			{
				RoleDataEx* Target = m_ProcTargetList[i]->Role;
				if( i >= List.size() )
					break;
				MagicAtkEffectInfoStruct& Temp = List[i];
				if(		Target->BaseData.Voc == EM_Vocation_Warrior
					||	Target->BaseData.Voc == EM_Vocation_Psyron )
				{
					Global::AddSP( Target , Owner , Temp.Value , MagicBaseDB->MagicBase.Attack.HateRate );
				}
				else if(	Target->BaseData.Voc_Sub == EM_Vocation_Warrior 
						||	Target->BaseData.Voc_Sub == EM_Vocation_Psyron )
				{
					Global::AddSP_Sub( Target , Owner , Temp.Value , MagicBaseDB->MagicBase.Attack.HateRate );
				}
				else 
				{
					Temp.Result = EM_MagicAtkResult_Miss;
					Temp.Value  = 0;
				}
				Owner->TempData.BackInfo.LastSkillHitPoint = int( Temp.Value );
			}  
			break;
		case EM_MagicAttackType_SP_Ranger:
			for( int i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
			{
				RoleDataEx* Target = m_ProcTargetList[i]->Role;
				if( i >= List.size() )
					break;
				MagicAtkEffectInfoStruct& Temp = List[i];
				if(		Target->BaseData.Voc == EM_Vocation_Ranger 
					||	Target->BaseData.Voc == EM_Vocation_Harpsyn )
				{
					Global::AddSP( Target , Owner , Temp.Value );
				}
				else if(	Target->BaseData.Voc_Sub == EM_Vocation_Ranger 
						||	Target->BaseData.Voc_Sub == EM_Vocation_Harpsyn )
				{
					Global::AddSP_Sub( Target , Owner , Temp.Value , MagicBaseDB->MagicBase.Attack.HateRate );
				}
				else 
				{
					Temp.Result = EM_MagicAtkResult_Miss;
					Temp.Value  = 0;
				}
				Owner->TempData.BackInfo.LastSkillHitPoint = int( Temp.Value );
			}  
			break;
		case EM_MagicAttackType_SP_Rogue:
			for( int i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
			{
				RoleDataEx* Target = m_ProcTargetList[i]->Role;
				if( i >= List.size() )
					break;
				MagicAtkEffectInfoStruct& Temp = List[i];
				if(		Target->BaseData.Voc == EM_Vocation_Rogue
					||	Target->BaseData.Voc == EM_Vocation_Duelist)
				{
					Global::AddSP( Target , Owner , Temp.Value , MagicBaseDB->MagicBase.Attack.HateRate );
				}
				else if(	Target->BaseData.Voc_Sub == EM_Vocation_Rogue 
						||	Target->BaseData.Voc_Sub == EM_Vocation_Duelist )
				{
					Global::AddSP_Sub( Target , Owner , Temp.Value , MagicBaseDB->MagicBase.Attack.HateRate );
				}
				else 
				{
					Temp.Result = EM_MagicAtkResult_Miss;
					Temp.Value  = 0;
				}
				Owner->TempData.BackInfo.LastSkillHitPoint = int( Temp.Value );
			}  
			break;
		case EM_MagicAttackType_StomachPoint:
			for( int i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
			{
				RoleDataEx* Target = m_ProcTargetList[i]->Role;
				if( i >= List.size() )
					break;
				MagicAtkEffectInfoStruct& Temp = List[i];

				Target->AddStomachPoint( int(Temp.Value) );

				Owner->TempData.BackInfo.LastSkillHitPoint = int( Temp.Value );
			}  
			break;
		}
	}
	
//	RoleDataEx::IsDisablePVPRule_Temp = false;

	//擊退
	if(		MagicBaseDB->MagicBase.SpecialAction.StrikeBack || MagicBaseDB->MagicBase.SpecialAction.Bomb ) 
//		&&  Owner->TempData.Attr.Effect.Stop == false 
//		&&	Owner->TempData.Attr.Effect.DisableStrikeBack == false 
//		&&  Owner->BaseData.Mode.Move	!= false )
	{
		for( unsigned i =  0  ; i < List.size() ; i++ )
		{
			if(		List[i].Result == EM_MagicAtkResult_Absorb 
				||	List[i].Result == EM_MagicAtkResult_NoEffect
				||	List[i].Result == EM_MagicAtkResult_Miss 
				||	List[i].Result == EM_MagicAtkResult_Cancel		)
			{
			}
			else
			{
				int Dest = MagicBaseDB->MagicBase.HitBackDist;
				SpecialMoveTypeENUM StrickType; 

				if( MagicBaseDB->MagicBase.SpecialAction.StrikeBack )
				{
					StrickType = EM_SpecialMoveType_StrikeBack;
				}
				else if( MagicBaseDB->MagicBase.SpecialAction.Bomb )
				{
					StrickType = EM_SpecialMoveType_Bomb;
				}

				RoleDataEx* Other = m_ProcTargetList[i]->Role;

				if(		Other->TempData.Attr.Effect.Stop != false 
					||	Other->TempData.Attr.Effect.DisableStrikeBack != false 
					||	Other->BaseData.Mode.Move	== false )
					continue;

				//移到目標
				float NX=0 , NZ=0;
				Global::SearchStrikeBackNode( Other , Owner->X() , Owner->Y() , Owner->Z() , Dest , NX , NZ );

				Other->Pos( NX , Other->Y(), NZ , Other->Dir() );
				Other->TempData.Move.Dx = 0;	
				Other->TempData.Move.Dy = 0;	
				Other->TempData.Move.Dz = 0;	
				Other->TempData.Move.MoveType = EM_ClientMoveType_Normal;
				Other->TempData.Move.IsLockSetPos = 0;
				Other->TempData.Move.LockSetPosX = Other->BaseData.Pos.X;
				Other->TempData.Move.LockSetPosZ = Other->BaseData.Pos.Z;

				BaseItemObject* OtherClass = BaseItemObject::GetObj( Other->GUID() );

				if( OtherClass == NULL )
				{
					assert( OtherClass != NULL );
					continue;
				}

				OtherClass->Path()->SetPos( NX , Other->Y(), NZ );				

				NetSrv_MoveChild::SendRange_SpecialMove( Owner->GUID() , StrickType , Other , MagicProc->MagicCollectID );

			}
		}
	}
	//清除仇恨( 會轉嫁 類似加血計算 )
	if( MagicBaseDB->MagicBase.SpecialAction.EraseHate  )
	{
		for( unsigned i =  0  ; i < List.size() ; i++ )
		{
			if(		List[i].Result == EM_MagicAtkResult_Absorb 
				||	List[i].Result == EM_MagicAtkResult_NoEffect
				||	List[i].Result == EM_MagicAtkResult_Miss 
				||	List[i].Result == EM_MagicAtkResult_Cancel	
				||	List[i].Result == EM_MagicAtkResult_ShieldBlock
				||	List[i].Result == EM_MagicAtkResult_Dodge
				)
			{
			}
			else
			{
				RoleDataEx* Other = m_ProcTargetList[i]->Role;
				ClsHateList( Owner , Other , MagicBaseDB->MagicBase.HateCost );
			}
		}
	}
	//施法中斷
	if( MagicBaseDB->MagicBase.SpecialAction.SpellInterrupt )
	{
		for( unsigned i =  0  ; i < List.size() ; i++ )
		{			
			if(		List[i].Result == EM_MagicAtkResult_Absorb 
				||	List[i].Result == EM_MagicAtkResult_NoEffect
				||	List[i].Result == EM_MagicAtkResult_Miss 
				||	List[i].Result == EM_MagicAtkResult_Cancel	
				||	List[i].Result == EM_MagicAtkResult_ShieldBlock
				||	List[i].Result == EM_MagicAtkResult_Dodge			)
			{
			}
			else
			{
				RoleDataEx* Other = m_ProcTargetList[i]->Role;
				if( Other->TempData.Magic.State != EM_MAGIC_PROC_STATE_NORMAL )
				{
					GameObjDbStructEx* MagicCol = Global::GetObjDB( Other->TempData.Magic.MagicCollectID );
					if( MagicCol != NULL && MagicCol->MagicCol.Flag.NoInterrupt == false )
					{
						Other->TempData.Magic.State = EM_MAGIC_PROC_STATE_INTERRUPT;
					}					
				}
			}
		}
	}

	//仇恨清除為1
	if( MagicBaseDB->MagicBase.SpecialAction.HateOnePoint  )
	{
		for( unsigned i =  0  ; i < List.size() ; i++ )
		{
			if(		List[i].Result == EM_MagicAtkResult_Absorb 
				||	List[i].Result == EM_MagicAtkResult_NoEffect
				||	List[i].Result == EM_MagicAtkResult_Miss 
				||	List[i].Result == EM_MagicAtkResult_Cancel	
				||	List[i].Result == EM_MagicAtkResult_ShieldBlock	
				||	List[i].Result == EM_MagicAtkResult_Dodge
				)
			{
			}
			else
			{
				RoleDataEx* Other = m_ProcTargetList[i]->Role;
				ClsHateListToOnePoint( Owner , Other , MagicBaseDB->MagicBase.HateCost );
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
    NetSrv_MagicChild::MagicAttackRange( Owner->GUID() , MagicBaseDB->GUID , MagicProc->SerialID , Atk.AtkType, List );


	//當被攻擊時怒氣計算
//	if(		( MagicCollectOrgDB->MagicCol.RangeType == EM_MagicEffect_Target || MagicCollectOrgDB->MagicCol.RangeType == EM_MagicEffect_Bad_Range )	 )
	{
		switch( Atk.AtkType )
		{
		case EM_MagicAttackType_HP:
			{
				//貝份目前資料
				vector < MagicTargetInfoStruct* > ProcTargetListBackup = m_ProcTargetList;
				vector< MagicTargetInfoStruct > TargetListBackup = m_TargetList;

				for( int i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
				{
					RoleDataEx* Target = m_ProcTargetList[i]->Role;
					if( i >= List.size() )
						break;
					MagicAtkEffectInfoStruct& Temp = List[i];

					//if( int( Temp.Value ) >= 0  )
					//	continue;

					ATTACK_DMGTYPE_ENUM DmgType;
					switch( Temp.Result )
					{
					case EM_MagicAtkResult_Miss:
						DmgType = EM_ATTACK_DMGTYPE_MISS;
						break;
					case EM_MagicAtkResult_Cancel:
						DmgType = EM_ATTACK_DMGTYPE_MISS;
						break;
					case EM_MagicAtkResult_Normal:
						DmgType = EM_ATTACK_DMGTYPE_NORMAL;
						break;
					case EM_MagicAtkResult_Critial:
						DmgType = EM_ATTACK_DMGTYPE_DOUBLE;
						break;
					case EM_MagicAtkResult_NoEffect:
						DmgType = EM_ATTACK_DMGTYPE_NO_EFFECT;
						break;
					case EM_MagicAtkResult_Absorb:
						DmgType = EM_ATTACK_DMGTYPE_ABSORB;
						break;
					case EM_MagicAtkResult_NoEffect_Dead :
						DmgType = EM_ATTACK_DMGTYPE_NO_EFFECT;
						break;
					default:
						DmgType = EM_ATTACK_DMGTYPE_NO_EFFECT;
						break;
					}
					
					if( MagicCollectOrgDB->MagicCol.RangeType == EM_MagicEffect_Target)
					{
						if( MagicProc->IsSystemMagic == false )
						{
							if( MagicBaseDB->MagicBase.EffectType == EM_Magic_Magic )
								Owner->OnAttack( Target , EM_OnAttackType_Magic , DmgType , Temp.Value , MagicCollectOrgDB->GUID , MagicBaseDB->GUID );
							else
								Owner->OnAttack( Target , EM_OnAttackType_Skill , DmgType , Temp.Value , MagicCollectOrgDB->GUID , MagicBaseDB->GUID );
						}
						else
						{//不算怒氣，但特別的buff仍要執行
							if( MagicBaseDB->MagicBase.EffectType == EM_Magic_Magic )
								Owner->SpecialAttackBuffProc( Target , EM_OnAttackType_Magic , DmgType , 0 , MagicCollectOrgDB->GUID , MagicBaseDB->GUID );
							else
								Owner->SpecialAttackBuffProc( Target , EM_OnAttackType_Skill , DmgType , 0 , MagicCollectOrgDB->GUID , MagicBaseDB->GUID );
						}
						
					}
					else
					{//不算怒氣，但特別的buff仍要執行
						if( MagicBaseDB->MagicBase.EffectType == EM_Magic_Magic )
							Owner->SpecialAttackBuffProc( Target , EM_OnAttackType_Magic , DmgType , Temp.Value , MagicCollectOrgDB->GUID , MagicBaseDB->GUID);
						else
							Owner->SpecialAttackBuffProc( Target , EM_OnAttackType_Skill , DmgType , Temp.Value , MagicCollectOrgDB->GUID , MagicBaseDB->GUID);
					}
				}  
				m_ProcTargetList = ProcTargetListBackup;
				m_TargetList = TargetListBackup;
			}
		}
	}
	return true;
}
//----------------------------------------------------------------------------------------
//計算物理單人傷害
MagicAtkEffectInfoStruct MagicProcessClass::CalPhyDamage( RoleDataEx* Owner , MagicTargetInfoStruct* TargetInfo , float Power , GameObjDbStructEx* MagicCollectOrgDB , GameObjDbStructEx*	MagicBaseDB , int MagicLv , float powerRate )
{
	RoleDataEx* Target = TargetInfo->Role;

	if( Owner->BaseData.Mode.RelyOwnerPower != false )
	{
		RoleDataEx* MasterOwner = Global::GetRoleDataByGUID( Owner->OwnerGUID() );
		if( MasterOwner != NULL )
			Owner = MasterOwner;
	}	

	Power +=	MagicBaseDB->MagicBase.Attack.FixValue 
				* Owner->GetValue( MagicBaseDB->MagicBase.Attack.FixType )
				* ( 1 + MagicLv * MagicBaseDB->MagicBase.Attack.FixDmg_SkillLVArg / 100 );

	if( Owner->IsPlayer() != false && Target->IsPlayer() != false )
	{
		if( Power < 0 )
			Power *= RoleDataEx::GlobalSetting.PK_DamageRate;;
	}
	else if( Owner->IsNPC() && Target->IsPlayer() )
	{
		if( Power < 0 )
			Power *= RoleDataEx::GlobalSetting.NPC_DamageRate;;
	}

	int PowerDLv = Owner->Level() - Target->Level(); 
	if( Power < 0 )
	{
		Power = Power * Global::DLvRate( PowerDLv , Owner->IsPlayer() );
		Power = Target->CalResistDMG( MagicBaseDB->MagicBase.MagicType , Power );
	}


	//int		CritHit		= int( Owner->TempData.Attr.Fin.Main_CriticalHit );
	int		CritHit		= int( Owner->TempData.Attr.CalCritialRate( Owner->TempData.Attr.Fin.Main_CriticalHit , Target->Level() , Target->TempData.Attr.Fin.ResistCriticalHit ) * 100 );
	float	CritPower	= Owner->TempData.Attr.Fin.CriticalHitPower;
//	float	AddFixPower	= MagicBaseDB->MagicBase.Attack.FixValue 
//						* Owner->GetValue( MagicBaseDB->MagicBase.Attack.FixType )
//						* ( 1 + MagicLv * MagicBaseDB->MagicBase.Attack.FixDmg_SkillLVArg / 100 );

	MagicAtkEffectInfoStruct Ret;
	Ret.GItemID = Target->GUID();
	Ret.Result	= EM_MagicAtkResult_Normal;
	Ret.Value	= 0;

	if( Target->IsDead() )
	{
		Ret.Result = EM_MagicAtkResult_NoEffect_Dead;
		return Ret;
	}

	//計算等級差( 取 -10 ~ 10 之間)
	int DLV = RangeValue( -10 , 10 , Owner->Level() -Target->Level() );


	int	CritHit_DLV = int( CritHit + DLV * Owner->FixArg().Crit_DLV) + MagicBaseDB->MagicBase.Attack.CritialRate;
	CritHit_DLV = RangeValue( 0 , 100 , CritHit_DLV );

	switch(	TargetInfo->HitType )
	{
	case EM_MAGIC_HIT_TYPE_HIT:			//命中
		{
			if( Target->TempData.Attr.DisableMagicSet & MagicBaseDB->MagicBase.MagicGroupSet )
			{
				Ret.Result = EM_MagicAtkResult_Absorb;
				Ret.Value = 0;
				break;
			}
			
			float PowerRate = 1;
			bool IsCritial = false;

			Ret.Result = EM_MagicAtkResult_Normal;

			RoleValueData&	TAttr = Target->TempData.Attr;
			RoleValueData&	OAttr = Owner->TempData.Attr;
			//盾檔
			if( MagicBaseDB->MagicBase.Attack.AtkType == EM_MagicAttackType_HP )
			{
				if( TAttr.CalShieldBlockRate( OAttr.Level , OAttr.Fin.ResistBlockRate )*100 > rand() % 100  )
				{
					//return EM_ATTACK_DMGTYPE_SHIELD_BLOCK;
					Ret.Result = EM_MagicAtkResult_ShieldBlock;
					return Ret;
				}
				else if( TAttr.CalParryRate( OAttr.Level , OAttr.Fin.ResistParryRate )*100 > rand() % 100  )
				{
					//return EM_ATTACK_DMGTYPE_HALF;
					Ret.Result = EM_MagicAtkResult_Parry;
					Power *= 0.5f;				
					//AddFixPower *= 0.5f;
				}
				//格檔
				else if( CritHit_DLV > rand() % 100 )
				{
					IsCritial = true;
					Ret.Result = EM_MagicAtkResult_Critial;
					Owner->TempData.BackInfo.LastCriticalTime  = RoleDataEx::G_SysTime;
					Target->TempData.BackInfo.LastBeCriticalTime  = RoleDataEx::G_SysTime;
				}
			}
			else
			{
				if( CritHit_DLV > rand() % 100 )
				{
					IsCritial = true;
					Ret.Result = EM_MagicAtkResult_Critial;
					Owner->TempData.BackInfo.LastCriticalTime  = RoleDataEx::G_SysTime;
					Target->TempData.BackInfo.LastBeCriticalTime  = RoleDataEx::G_SysTime;
				}
			}
			switch( MagicBaseDB->MagicBase.AtkCalType )
			{
			case EM_MagicAttackCalBase_Physics_Point://物理計算
			case EM_MagicAttackCalBase_Physics://物理計算
				{
					float OAtk = ( Owner->TempData.Attr.Fin.ATK + 100 );
					float TDef = Target->TempData.Attr.Fin.DEF + 200;
//					if( IsCritial != false )						
//						TDef *= _DEF_CRITIAL_DEF_RATE_;
					PowerRate = 1 + ( OAtk - TDef )/ __max(OAtk , TDef );
				}
				break;
			case EM_MagicAttackCalBase_Throw://投擲計算
			/*	{
					float OAtk = ( Owner->TempData.Attr.Fin.AGI*2 + 50 );
					float TDef = Target->TempData.Attr.Fin.DEF + 100;
					if( IsCritial != false )						
						TDef *= _DEF_CRITIAL_DEF_RATE_;
					PowerRate = 1 + ( OAtk - TDef )/ __max(OAtk , TDef );
				}
				break;*/
			case EM_MagicAttackCalBase_Shoot://射擊計算
				{
/*					float OAtk = ( Owner->TempData.Attr.Fin.ATK + 100 );
					float TDef = Target->TempData.Attr.Fin.DEF + 200;
					if( IsCritial != false )						
						TDef *= _DEF_CRITIAL_DEF_RATE_;
					PowerRate = 1 + ( OAtk - TDef )/ __max(OAtk , TDef );*/
					float OAtk = ( Owner->TempData.Attr.Fin.ATK + 100 );
					float TDef = Target->TempData.Attr.Fin.DEF + 200;
					PowerRate = 1 + ( OAtk - TDef )/ __max(OAtk , TDef ) ;
				}
				break;
			case EM_MagicAttackCalBase_Magic://法術計算
			case EM_MagicAttackCalBase_Fix://定值
			case EM_MagicAttackCalBase_HolyHeal://神聖治療計算( 負值表示Miss  )
			case EM_MagicAttackCalBase_DarkHeal://不死系療計算( 負值表示Miss  )
			case EM_MagicAttackCalBase_SpecialAction://沒攻擊只有特殊行為
			case EM_MagicAttackCalBase_FixPer://百分比
				break;
			}

			if( IsCritial )
			{
				//PowerRate *= CritPower;
				Power *= CritPower;
			}

			if( PowerRate < 0.2f )
				PowerRate = 0.2f;

/*			if( Owner->CheckPhyAttack( Target ) == false )
			{
				Ret.Result = EM_MagicAtkResult_NoEffect;
				return Ret;
			}*/

			//如果為加血 不算抗性
			if( Power >= 0 )
			{
				Ret.Value = Power;
				return Ret;
			}
			if( MagicBaseDB->MagicBase.SpecialAction.IgnoreDefine )
			{
				Ret.Value = Power;
			}
			else
			{
				//float Damage = Power* PowerRate* -1;
				Power *= PowerRate;
				if( MagicBaseDB->MagicBase.SettingEx.IgnoreMagicAbsorb == false )
					Power = Target->TempData.Attr.CalWeaponDMG( Power ) ;

				if( Power == 0 )
					Ret.Result = EM_MagicAtkResult_Absorb;
				else
					Ret.Value = Power;		
			}

		}
		break;
	case EM_MAGIC_HIT_TYPE_MISS:		//MISS
		{
			Ret.Result = EM_MagicAtkResult_Miss;
			return Ret;
		}
		break;
	case EM_MAGIC_HIT_TYPE_DODGE:		//MISS
		{
			Ret.Result = EM_MagicAtkResult_Dodge;
			return Ret;
		}
	case EM_MAGIC_HIT_TYPE_CANCEL:
		{
			Ret.Result = EM_MagicAtkResult_Cancel;
			return Ret;
		}
		break;
	case EM_MAGIC_HIT_TYPE_NO_EFFECT:		//無效
		{
			Ret.Result = EM_MagicAtkResult_NoEffect;			
			return Ret;
		}
		break;
	}

	//////////////////////////////////////////////////////////////////////////	
	//處理魔法盾
	Target->CalHate( Owner , 0 , MagicBaseDB->MagicBase.HateCost );

	//如果為NPC計算物理抗性
	if( Target->IsNPC() && MagicBaseDB->MagicBase.PhyAttackType != EM_PhyResist_NULL && Ret.Value < 0 )
	{
		GameObjDbStructEx* NPCDB = g_ObjectData->GetObj( Target->BaseData.ItemInfo.OrgObjID );
		if( NPCDB != NULL )
		{
			Ret.Value *=  NPCDB->NPC.PhyResistRate[ MagicBaseDB->MagicBase.PhyAttackType ];
			if( Ret.Value > -1 )
				Ret.Value =-1;
		}
	}
	
	int Damage = int( Ret.Value * -1 * powerRate );

	if( Damage > 0 )
	{
		RoleDataEx* OTarget = Target->GetOrgOwner();
		//1是否為AE
		if( MagicCollectOrgDB->MagicCol.RangeType != EM_MagicEffect_Target )
		{
			Damage = Damage * Owner->TempData.Attr.Mid.BodyRate[ EM_WearEqType_AEMagicPowerRate ] * Target->TempData.Attr.Mid.BodyRate[ EM_WearEqType_AEMagicDefRate ];
		}

		
		if( OTarget->IsPlayer() )
			Damage = Damage * Owner->TempData.Attr.Mid.BodyRate[ EM_WearEqType_AttackPlayerPowerRate ];
		else 
			Damage = Damage * Owner->TempData.Attr.Mid.BodyRate[ EM_WearEqType_AttackNPCPowerRate ];

		RoleDataEx* OrgOwner = Owner->GetOrgOwner();
		if( OrgOwner->IsPlayer() )
			Damage = Damage *  Target->TempData.Attr.Mid.BodyRate[ EM_WearEqType_PlayerDefRate ];
		else
			Damage = Damage *  Target->TempData.Attr.Mid.BodyRate[ EM_WearEqType_NPCDefRate ];

		Ret.Value = Damage * -1;
	}

	int AbsorbDamage = 0;
	if( MagicBaseDB->MagicBase.Attack.AtkType == EM_MagicAttackType_HP )
	{
		if( MagicCollectOrgDB->MagicCol.IsIgnoreInvincible == false )
			AbsorbDamage = Target->MagicShieldAbsorbProc( Owner , MagicBaseDB , Damage );
	}
	if( AbsorbDamage == 0 )
		return Ret;

	if( Damage != 0 )
	{
		Ret.OrgValue = Ret.Value = float( Damage * -1 );
		Ret.Result = EM_MagicAtkResult_Normal;	
	}
	else
	{
		Ret.OrgValue = Ret.Value = 0;
		Ret.Result = EM_MagicAtkResult_Absorb;	
	}

	return Ret;

}
//計算法術單人傷害
MagicAtkEffectInfoStruct MagicProcessClass::CalMagicDamage( RoleDataEx* Owner , MagicTargetInfoStruct* TargetInfo , float Power , GameObjDbStructEx* MagicCollectOrgDB , GameObjDbStructEx* MagicBaseDB , int MagicLv , float powerRate )
{

	RoleDataEx* Target = TargetInfo->Role;

	if( Owner->BaseData.Mode.RelyOwnerPower != false )
	{
		RoleDataEx* MasterOwner = Global::GetRoleDataByGUID( Owner->OwnerGUID() );
		if( MasterOwner != NULL )
			Owner = MasterOwner;
	}	

	Power +=  MagicBaseDB->MagicBase.Attack.FixValue 
			* Owner->GetValue( MagicBaseDB->MagicBase.Attack.FixType )
			* ( 1 + MagicLv * MagicBaseDB->MagicBase.Attack.FixDmg_SkillLVArg / 100 );

	int PowerDLv = Owner->Level() - Target->Level(); 
	if( Power < 0 )
	{
		Power = Power * Global::DLvRate( PowerDLv , Owner->IsPlayer() );
		Power = Target->CalResistDMG( MagicBaseDB->MagicBase.MagicType , Power );
	}

	if( Owner->IsPlayer() != false && Target->IsPlayer() != false )
	{
		if( Power < 0 )
			Power *= RoleDataEx::GlobalSetting.PK_DamageRate;;
	}
	else if( Owner->IsNPC() != false && Target->IsPlayer() != false )
	{
		if( Power < 0 )
			Power *= RoleDataEx::GlobalSetting.NPC_DamageRate;;
	}

//	int		CritHit		= int( Owner->TempData.Attr.Fin.MagicCriticalHit );
	int		CritHit		= int( Owner->TempData.Attr.CalCritialRate( Owner->TempData.Attr.Fin.MagicCriticalHit , Target->Level() , Target->TempData.Attr.Fin.ResistCriticalHit ) * 100 );
	float	CritPower	= Owner->TempData.Attr.Fin.MagicCriticalHitPower;
//	float	AddFixPower	= MagicBaseDB->MagicBase.Attack.FixValue 
//						* Owner->GetValue( MagicBaseDB->MagicBase.Attack.FixType )
//						* ( 1 + MagicLv * MagicBaseDB->MagicBase.Attack.FixDmg_SkillLVArg / 100 );

	MagicAtkEffectInfoStruct Ret;
	Ret.GItemID = Target->GUID();
	Ret.Result	= EM_MagicAtkResult_Normal;
	Ret.Value	= 0;

	if( Target->IsDead() )
	{
		Ret.Result = EM_MagicAtkResult_NoEffect_Dead;
		return Ret;
	}
	//計算等級差( 取 -10 ~ 10 之間)
	int DLV = RangeValue( -10 , 10 , Owner->Level() -Target->Level() );


	int	CritHit_DLV = int( CritHit + DLV * Owner->FixArg().Crit_DLV) + MagicBaseDB->MagicBase.Attack.CritialRate;
	CritHit_DLV = RangeValue( 0 , 100 , CritHit_DLV );


	switch(	TargetInfo->HitType )
	{
	case EM_MAGIC_HIT_TYPE_HIT:			//命中
		{
			if( Target->TempData.Attr.DisableMagicSet & MagicBaseDB->MagicBase.MagicGroupSet )
			{
				Ret.Result = EM_MagicAtkResult_Absorb;
				Ret.Value = 0;
				return Ret;
			}

			float PowerRate = 1;
			bool IsCritial = false;

			Ret.Result = EM_MagicAtkResult_Normal;

			if( CritHit_DLV > rand() % 100 )
			{
				//計算魔法Critial 抗性
//				if( rand()%100 >= Target->TempData.Attr.Mid.Body[EM_WearEqType_ResistMagicCritRate] )
				{
					IsCritial = true;
					Ret.Result = EM_MagicAtkResult_Critial;

					Owner->TempData.BackInfo.LastMagicCriticalTime  = RoleDataEx::G_SysTime;
					Target->TempData.BackInfo.LastBeMagicCriticalTime  = RoleDataEx::G_SysTime;
				}
			}

			switch( MagicBaseDB->MagicBase.AtkCalType )
			{
			case EM_MagicAttackCalBase_Magic://法術計算
			case EM_MagicAttackCalBase_MagicEx://法術計算
			case EM_MagicAttackCalBase_HolyHeal://神聖治療計算( 負值表示Miss  )
			case EM_MagicAttackCalBase_DarkHeal://不死系療計算( 負值表示Miss  )
				{
					float OAtk = ( Owner->TempData.Attr.Fin.MATK + 100 );
					float TDef = Target->TempData.Attr.Fin.MDEF + 200;
					PowerRate = 1 + ( OAtk - TDef )/ __max(OAtk , TDef );
				}
				break;
			case EM_MagicAttackCalBase_Physics_Point://物理計算
			case EM_MagicAttackCalBase_Physics://物理計算
			case EM_MagicAttackCalBase_Throw://投擲計算
			case EM_MagicAttackCalBase_Shoot://射擊計算
			case EM_MagicAttackCalBase_Fix://定值
			case EM_MagicAttackCalBase_SpecialAction://沒攻擊只有特殊行為
			case EM_MagicAttackCalBase_FixPer://百分比
				break;
			}

			if( IsCritial )
			{
				//PowerRate *= CritPower;
				//Ret.Value *= CritPower;
				Power *= CritPower;
			}

			if( PowerRate < 0.2f )
				PowerRate = 0.2f;

			//如果為加血 不算抗性
			if( Power >= 0  )
			{
				Ret.Value = Power;
				return Ret;
			}

			
			Power = Power * PowerRate;

			if( MagicBaseDB->MagicBase.SpecialAction.IgnoreDefine )
			{
				Ret.Value = Power;
			}
			else
			{
				if( MagicBaseDB->MagicBase.SettingEx.IgnoreMagicAbsorb == false )
					Power = Target->TempData.Attr.CalMagicDMG( Power );

				if( Power == 0 )
					Ret.Result = EM_MagicAtkResult_Absorb;


				Ret.Value = Power;
			}
		}
		break;
	case EM_MAGIC_HIT_TYPE_MISS:		//MISS
		{
			Ret.Result = EM_MagicAtkResult_Miss;
			return Ret;
		}
		break;
	case EM_MAGIC_HIT_TYPE_DODGE:		//MISS
		{
			
			Ret.Result = EM_MagicAtkResult_Dodge;
			return Ret;
		}
		break;

	case EM_MAGIC_HIT_TYPE_CANCEL:
		{
			Ret.Result = EM_MagicAtkResult_Cancel;
			return Ret;
		}
		break;
	case EM_MAGIC_HIT_TYPE_NO_EFFECT:		//無效
		{
			Ret.Result = EM_MagicAtkResult_NoEffect;			
			return Ret;
		}
		break;
	}

	//////////////////////////////////////////////////////////////////////////	
	//處理魔法盾
	//Target->CalHate( Owner , 0 , MagicBaseDB->MagicBase.HateCost );
	int AbsorbDamage = 0;
	int Damage = int( Ret.Value * -1 * powerRate );

	if( Damage > 0 )
	{
		RoleDataEx* OTarget = Target->GetOrgOwner();
		//1是否為AE
		if( MagicCollectOrgDB->MagicCol.RangeType != EM_MagicEffect_Target )
			Damage = Damage * Owner->TempData.Attr.Mid.BodyRate[ EM_WearEqType_AEMagicPowerRate ] * Target->TempData.Attr.Mid.BodyRate[ EM_WearEqType_AEMagicDefRate ];

		if( OTarget->IsPlayer() )
			Damage = Damage * Owner->TempData.Attr.Mid.BodyRate[ EM_WearEqType_AttackPlayerPowerRate ];
		else 
			Damage = Damage * Owner->TempData.Attr.Mid.BodyRate[ EM_WearEqType_AttackNPCPowerRate ];

		RoleDataEx* OrgOwner = Owner->GetOrgOwner();
		if( OrgOwner->IsPlayer() )
			Damage = Damage *  Target->TempData.Attr.Mid.BodyRate[ EM_WearEqType_PlayerDefRate ];
		else
			Damage = Damage *  Target->TempData.Attr.Mid.BodyRate[ EM_WearEqType_NPCDefRate ];

		Ret.Value = Damage * -1;
	}

	if( MagicBaseDB->MagicBase.Attack.AtkType == EM_MagicAttackType_HP )
	{
		if( MagicCollectOrgDB->MagicCol.IsIgnoreInvincible == false )
			AbsorbDamage = Target->MagicShieldAbsorbProc( Owner , MagicBaseDB , Damage );
	}
	if( AbsorbDamage == 0 )
		return Ret;


	if( Damage != 0 )
	{
		Ret.OrgValue = Ret.Value = float( Damage * -1 );
		Ret.Result = EM_MagicAtkResult_Normal;	
	}
	else
	{
		Ret.OrgValue = Ret.Value = 0;
		Ret.Result = EM_MagicAtkResult_Absorb;	
	}
	return Ret;
}
//////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
bool MagicProcessClass::Magic01_Assist		( RoleDataEx*	Owner 
											, GameObjDbStructEx*	OrgDB	
											, MagicProcInfo* MagicProc		)
{
	int	MagicPowerLv = 0;
	GameObjDbStructEx*	MagicCollectOrgDB = Global::GetObjDB( MagicProc->MagicCollectID );

	if( MagicCollectOrgDB == NULL )
		return false;

	float	SpellTime = MagicCollectOrgDB->MagicCol.SpellTime;
	if( SpellTime < 1 )
		SpellTime = 1;

    int     i;
	bool	IsCanAttack = true;
    static vector< MagicAssistEffectInfoStruct > List;

    List.clear();

	if( OrgDB->MagicBase.ClearTime.Spell != false )
		Owner->TempData.BackInfo.BuffClearTime.Spell = false;

//	if( MagicCollectOrgDB->MagicCol.IsDiabledPvPRule  )
//		RoleDataEx::IsDisablePVPRule_Temp = true;
	
    for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
    {
        MagicAssistEffectInfoStruct Temp;
		RoleDataEx* Target = m_ProcTargetList[i]->Role;
		if( Target->TempData.Attr.DisableMagicSet & OrgDB->MagicBase.MagicGroupSet )
		{
			Temp.EffectTime =   0;
			Temp.Result = EM_MagicAtkResult_NoEffect;
		}
		else if( m_ProcTargetList[i]->HitType == EM_MAGIC_HIT_TYPE_CANCEL ) 
		{
			Temp.EffectTime =   0;
			Temp.Result = EM_MagicAtkResult_Cancel;
		}
		else if( m_ProcTargetList[i]->HitType == EM_MAGIC_HIT_TYPE_NO_EFFECT ) 
		{
			Temp.EffectTime =   0;
			Temp.Result = EM_MagicAtkResult_NoEffect;
		}

        else if(		IsCanAttack != false 
			&&	m_ProcTargetList[i]->HitType == EM_MAGIC_HIT_TYPE_HIT )
        {

			if( OrgDB->MagicBase.Effect.Ride != false && Global::Ini()->IsEnabledRide == false )
			{
				Temp.EffectTime =   0;
				Temp.Result = EM_MagicAtkResult_Miss;
			}
			else
			{
				if( Target->IsNPC() && OrgDB->MagicBase.AssistType != EM_Magic_Type_NULL )
				{
					GameObjDbStructEx* NPCObjDB = Global::GetObjDB( Target->BaseData.ItemInfo.OrgObjID );
					if( NPCObjDB->IsNPC() )
					{
						for( int i = 0 ; i < _MAX_NPC_AVOID_MAGIC_TYPE_COUNT_ ; i++ )
						{
							if( NPCObjDB->NPC.AvoidMagicType[i] == OrgDB->MagicBase.AssistType )
							{
								Temp.Result = EM_MagicAtkResult_Miss;
								break;
							}
						}
					}
				}
				
				if( Temp.Result != EM_MagicAtkResult_Miss )
				{
					//如果目標是NPC處發劇情事件	(輔助法術劇情)
					vector<MyVMValueStruct> RetList;
					if( Target->IsNPC() )
					{
						char	LuaScriptBuf[512];
						GameObjDbStructEx* NpcDB = Global::GetObjDB( Target->BaseData.ItemInfo.OrgObjID );
						if( NpcDB->IsNPC() && strlen(NpcDB->NPC.LuaEvent_OnAssistMagic) != 0 )
						{
							sprintf_s( LuaScriptBuf , sizeof(LuaScriptBuf) , "%s(%d)" , NpcDB->NPC.LuaEvent_OnAssistMagic, OrgDB->GUID  );
							LUA_VMClass::PCallByStrArg( LuaScriptBuf , Owner->GUID() , Target->GUID() , &RetList , 1 );
						}
					}

					if( RetList.size() == 1 && RetList[0].Flag == false )
					{
						Temp.EffectTime =   0;
						Temp.Result = EM_MagicAtkResult_Miss;
					}
					else
					{
						RoleDataEx* Other = m_ProcTargetList[i]->Role;
						if( Other->IsDead() )
						{
							Temp.EffectTime = 0;
							Temp.Result = EM_MagicAtkResult_NoEffect_Dead;
						}
						else
						{

							MagicPowerLv = Owner->CalAddDotMagicLv( Target , OrgDB , (int)SpellTime , MagicProc->MagicLv ) + MagicProc->MagicLv;

							if( OrgDB->MagicBase.Setting.ShowPowerLv != false )
								MagicPowerLv = 0;

							BuffBaseStruct* RetBuf = NULL;
							int RetTime = 0;
							
							int DLV = Owner->Level() - Target->Level();

							int EffectTime = int( OrgDB->MagicBase.EffectTime );

							if( EffectTime >= 0 )
							{
								EffectTime = int( OrgDB->MagicBase.EffectTime * ( 1 + OrgDB->MagicBase.EffectTime_SkillLVArg * MagicProc->MagicLv / 100 ) 
								- DLV * OrgDB->MagicBase.DLV_EffectTime  );
							}
							else
								EffectTime = 100000000;



							if( Other->IsPlayer() && Owner->IsPlayer() && Other != Owner )
								RetBuf = Owner->AssistMagic( Other , OrgDB , MagicPowerLv , true , EffectTime );
							else
								RetBuf = Owner->AssistMagic( Other , OrgDB , MagicPowerLv , false , EffectTime );

							if( RetBuf == NULL || RetBuf->Time == 0 )
							{
								Temp.Result = EM_MagicAtkResult_NoEffect;
								Temp.EffectTime = 0;
								Temp.MagicLv	= 0;
							}
							else
							{
								Temp.Result = EM_MagicAtkResult_Normal;
								Temp.EffectTime = RetBuf->Time;
								Temp.MagicLv	= RetBuf->Power;

								if( OrgDB->MagicBase.Setting.GoodMagic != false )
								{
									Global::GoodEvilAtkInfoProc( Owner , Other , 1 );
								}
								else
								{
									Global::GoodEvilAtkInfoProc( Owner , Other , -1 );
								}
								//////////////////////////////////////////////////////////////////////////
								
								if( OrgDB->MagicBase.Setting.DotMagic != false )
								{									
									DotMagicStruct& DotMagic = OrgDB->MagicBase.DotMagic;
									if(		DotMagic.Type == EM_DotMagicType_HP
										||	DotMagic.Type == EM_DotMagicType_HP_Per
										||	DotMagic.Type == EM_DotMagicType_MP
										||	DotMagic.Type == EM_DotMagicType_MP_Per		)
									{
										//如果為HP Dot加血
										if( DotMagic.Base > 0 )
										{
											if( OrgDB->MagicBase.Setting.IgnoreSpellCureMagicClear == false )
												Owner->TempData.BackInfo.BuffClearTime.Spell_Cure = true;
										}
										else if( DotMagic.Base < 0 )
										{
											if( OrgDB->MagicBase.Setting.IgnoreSpellAttackMagicClear == false )
												Owner->TempData.BackInfo.BuffClearTime.Spell_MagicAttack = true;
											Owner->TempData.BackInfo.BuffClearTime.Attack = true;
										}
									}
								}
								//////////////////////////////////////////////////////////////////////////
							}
						}

						//if( OrgDB->MagicBase.Setting.ShowBuffTime == false && Temp.EffectTime != 0 )
						//	Temp.EffectTime = -1;

						if( OrgDB->MagicBase.HateCost != 0 )
							Other->CalHate( Owner , 0 , OrgDB->MagicBase.HateCost );
						else if( OrgDB->MagicBase.Setting.GoodMagic == false 
							&&	Other->TempData.Attr.Effect.BadMagicInvincible == false
							&&	Other->TempData.Attr.Effect.BadPhyInvincible == false					)
							Other->CalHate( Owner , 0 , 1 );


						//					bool IsStopMove = Other->TempData.Attr.Effect.Stop;
						Other->OnTimeProc( true );
						if( OrgDB->MagicBase.Effect.Stop != false )
						{
							if( Other->TempData.Attr.Effect.Stop != false )
								Other->StopMoveNow();
						}

						if( OrgDB->MagicBase.Effect.Ride != false )
						{
							memcpy( Other->BaseData.HorseColor , MagicProc->HorseColor , sizeof(Other->BaseData.HorseColor) );
							NetSrv_MoveChild::SendRangeHorseColor( Other );
						}

						if( OrgDB->MagicBase.Setting.CancelBadMagic != false )
						{
							//收尋週圍
							vector<BaseSortStruct>& SearchList = *(Global::SearchRangeObject( Other , Other->X() , Owner->Y() , Other->Z() , EM_SearchRange_All  , 400 ) );
							for( unsigned i = 0 ; i< SearchList.size() ; i++ )
							{
								RoleDataEx* Target = (RoleDataEx*)SearchList[i].Data;
								if( Target->IsSpell() == false  )
									continue;
								if( Target->AttackTargetID() == Other->GUID() )
								{
									GameObjDbStructEx* TMagicCol = Global::GetObjDB( Target->TempData.Magic.MagicCollectID );
									if( TMagicCol == NULL )
										continue;
									if(	TMagicCol->MagicCol.EffectRange  == EM_MagicEffect_Target 
										&&	TMagicCol->MagicCol.TargetType == EM_Releation_Enemy	)										
									{
										Target->TempData.Magic.State = EM_MAGIC_PROC_STATE_INTERRUPT;
									}
								}
							}
						}
					}
				}
			}
			
        }
        else
        {
            Temp.EffectTime =   0;
            Temp.Result = EM_MagicAtkResult_Miss;
        }
        Temp.GItemID = m_ProcTargetList[i]->Role->GUID();
//		Temp.MagicLv = MagicProc->MagicLv;
        List.push_back( Temp );
    }

	for( unsigned i =  0  ; i < List.size() ; i++ )
	{			
		RoleDataEx* Target = m_ProcTargetList[i]->Role;
		char	LuaScriptBuf[512];	
		if( strlen( OrgDB->MagicBase.EndUseLua ) != 0 )
		{
			sprintf_s( LuaScriptBuf , sizeof(LuaScriptBuf) , "%s(%d)" , OrgDB->MagicBase.EndUseLua , List[i].Result );
			LUA_VMClass::PCallByStrArg( LuaScriptBuf , Owner->GUID() ,Target->GUID() );
		}

		if(		List[i].Result == EM_MagicAtkResult_Absorb 
			||	List[i].Result == EM_MagicAtkResult_NoEffect
			||	List[i].Result == EM_MagicAtkResult_Miss )
		{
			if( m_IsMissErase )
				m_ProcTargetList[i]->IsDel = true;
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
		else if( List[i].Result == EM_MagicAtkResult_Cancel )
		{
			if( m_IsMissErase )
				m_ProcTargetList[i]->IsDel = true;

		}
		else
		{
			m_ProcTargetList[i]->IsSpellOneMagic = true;

			if( m_ProcTargetList[i]->Role->IsPlayer() && OrgDB->MagicBase.Setting.GoodMagic == false )
			{
				CNetSrv_Script_Child::OnEvent_RoleBeHit( m_ProcTargetList[i]->Role );
			}
		}
	}

	NetSrv_MagicChild::MagicAssistRange(	Owner->GUID()
										,	OrgDB->GUID		
										,	MagicProc->SerialID										
                                        ,   List );
	
	
//	RoleDataEx::IsDisablePVPRule_Temp = false;
	return true;
}
//----------------------------------------------------------------------------------------
//	傳送法術
//----------------------------------------------------------------------------------------
bool MagicProcessClass::Magic02_Teleport		( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc )
{
	
	//-------------------------------------------------------------------------------
	//先把物品消耗扣除
	GameObjDbStructEx* MagicCollectOrgDB = Global::GetObjDB( MagicProc->MagicCollectID );
	MagicProc->IsPayCost = true;
	Owner->DelUseItem();
	//刪除消耗
	Owner->DelMagicCost( MagicCollectOrgDB , MagicProc->MagicLv  );	
	MagicProc->State = EM_MAGIC_PROC_STATE_ENDSPELL;
	//-------------------------------------------------------------------------------

	int     i;
	float	X , Y , Z;
	static vector< MagicAssistEffectInfoStruct > List;

	List.clear();
	MagicAssistEffectInfoStruct Temp;
	Temp.EffectTime =   0;

	for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
	{
		RoleDataEx* Target = m_ProcTargetList[i]->Role;



		switch( m_ProcTargetList[i]->HitType )
		{
		case EM_MAGIC_HIT_TYPE_HIT:			//命中
		case EM_MAGIC_HIT_TYPE_CRITICAL:	//致命一擊
			{
				if( Target->TempData.Attr.DisableMagicSet & OrgDB->MagicBase.MagicGroupSet )
				{
					Temp.Result = EM_MagicAtkResult_Miss;
					break;
				}

				if( Target->TempData.Attr.Effect.TeleportDisable != false )
				{
					Temp.Result = EM_MagicAtkResult_Miss;
					break;
				}

				Temp.Result = EM_MagicAtkResult_Normal;
				
				if( MagicProc->TargetX != 0 && MagicProc->TargetZ != 0 )
				{
					X = Target->Pos()->X - MagicProc->TargetX + OrgDB->MagicBase.Teleport.X;
					Y = float( OrgDB->MagicBase.Teleport.Y );
					Z = Target->Pos()->Z - MagicProc->TargetZ + OrgDB->MagicBase.Teleport.Z;
				}
				else
				{
					X = float( OrgDB->MagicBase.Teleport.X );
					Y = float( OrgDB->MagicBase.Teleport.Y );
					Z = float( OrgDB->MagicBase.Teleport.Z );
				}
				
				if( Owner == Target )
				{

				}

				if(		Owner->TempData.Sys.OnChangeZone != false 
					||	Owner->TempData.IsDisabledChangeZone != false )
					return false;

				//int ZoneID = OrgDB->MagicBase.Teleport.ZoneID  % _DEF_ZONE_BASE_COUNT_ + _DEF_ZONE_BASE_COUNT_ * Owner->PlayerTempData->ParallelZoneID;
				int ZoneID = GlobalBase::GetParallelZoneID( OrgDB->MagicBase.Teleport.ZoneID ,  Owner->PlayerTempData->ParallelZoneID );
				if( ZoneID == -1 )
					return false;
				Global::ChangeZone( Target->GUID() , ZoneID , -1 , X , Y , Z , Target->Pos()->Dir );
			}

			break;
		case EM_MAGIC_HIT_TYPE_MISS:		//MISS
			Temp.Result = EM_MagicAtkResult_Miss;
			break;
		case EM_MAGIC_HIT_TYPE_DODGE:		//MISS
			Temp.Result = EM_MagicAtkResult_Dodge;
			break;


		case EM_MAGIC_HIT_TYPE_CANCEL:
			Temp.Result = EM_MagicAtkResult_Cancel;
			break;
		case EM_MAGIC_HIT_TYPE_NO_EFFECT:	//無效
			Temp.Result = EM_MagicAtkResult_NoEffect;
			break;

		}

		Temp.GItemID = Target->GUID();
		Temp.MagicLv = MagicProc->MagicLv;
		List.push_back( Temp );
	}

	for( unsigned i =  0  ; i < List.size() ; i++ )
	{		
		RoleDataEx* Target = m_ProcTargetList[i]->Role;
		char	LuaScriptBuf[512];	
		if( strlen( OrgDB->MagicBase.EndUseLua ) != 0 )
		{
			sprintf_s( LuaScriptBuf , sizeof(LuaScriptBuf) , "%s(%d)" , OrgDB->MagicBase.EndUseLua , List[i].Result );
			LUA_VMClass::PCallByStrArg( LuaScriptBuf , Owner->GUID() ,Target->GUID() );
		}

		if(		List[i].Result == EM_MagicAtkResult_Absorb 
			||	List[i].Result == EM_MagicAtkResult_NoEffect
			||	List[i].Result == EM_MagicAtkResult_Miss )
		{
			if( m_IsMissErase != false )
				m_ProcTargetList[i]->IsDel = true;
				m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
		else if( List[i].Result == EM_MagicAtkResult_Cancel )
		{
			if( m_IsMissErase )
				m_ProcTargetList[i]->IsDel = true;

		}
		else
		{
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
	}

	NetSrv_MagicChild::MagicAssistRange(	Owner->GUID()
		,	OrgDB->GUID
		,	MagicProc->SerialID
		,   List );

 
	return true;
}
//----------------------------------------------------------------------------------------
bool MagicProcessClass::Magic03_CreatePet		( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc )
{
	int     i;
	static vector< MagicAssistEffectInfoStruct > List;

	List.clear();
	MagicAssistEffectInfoStruct Temp;
	Temp.EffectTime =   0;

	int     OrgObjID = OrgDB->MagicBase.SummonCreature.ObjID;
	char*   Name  = "";
	
	int	DLV = 1;
	if( OrgDB->MagicBase.SummonCreature.RangeLevel >=1 )
		DLV = OrgDB->MagicBase.SummonCreature.RangeLevel;

	int	PetID;
	RoleDataEx* Pet;

	for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
	{
		RoleDataEx* Target = m_ProcTargetList[i]->Role;
		int LV;

		if( OrgDB->MagicBase.SummonCreature.Level <= 0 )
			LV = rand( ) % DLV + Target->Level() - OrgDB->MagicBase.SummonCreature.Level;
		else
			LV = rand( ) % DLV + OrgDB->MagicBase.SummonCreature.Level;

		LV = LV + int( OrgDB->MagicBase.SummonCreature.SkillLvArg * MagicProc->MagicLv ); 

		Temp.Result = EM_MagicAtkResult_Normal;



		if( OrgDB->MagicBase.SummonCreature.Type == EM_MagicSummonCreatureType_Pet )
		{
			PetID = Global::CreatePet( Target->GUID() , OrgObjID , Name , LV , EM_SummonPetType_Normal , OrgDB->GUID );
		}
		else if( OrgDB->MagicBase.SummonCreature.Type == EM_MagicSummonCreatureType_Guard )
		{
			PetID = Global::CreateGuard( Target->GUID() , OrgObjID , Name , LV , true , OrgDB->MagicBase.SummonCreature.GroupID , OrgDB->GUID );
		}
		else if( OrgDB->MagicBase.SummonCreature.Type == EM_MagicSummonCreatureType_Guard_NoAttack )
		{
			PetID = Global::CreateGuard( Target->GUID() , OrgObjID , Name , LV , false , OrgDB->MagicBase.SummonCreature.GroupID , OrgDB->GUID );
		}
		BaseItemObject* PetObj = Global::GetObj( PetID );
		if( PetObj != NULL )
		{
			Pet = PetObj->Role();
			if( Pet != NULL && OrgDB->MagicBase.SummonCreature.LiveTime != -1 )
				Pet->SelfData.LiveTime = OrgDB->MagicBase.SummonCreature.LiveTime * 1000 + RoleDataEx::G_SysTime;

			if( OrgDB->MagicBase.SummonCreature.InitLua[0] != 0 )
			{
				Pet->SelfData.AutoPlot = OrgDB->MagicBase.SummonCreature.InitLua;
				PetObj->PlotVM()->CallLuaFunc( Pet->SelfData.AutoPlot.Begin() , PetID );
			}
			NetSrv_Magic::S_CreatePetRangeInfo( PetID );
		}

		Temp.GItemID = Target->GUID();
		Temp.MagicLv = MagicProc->MagicLv;
		//Target->SetPetSkill( OrgDB );

		List.push_back( Temp );
	}

	for( unsigned i =  0  ; i < List.size() ; i++ )
	{			
		RoleDataEx* Target = m_ProcTargetList[i]->Role;
		char	LuaScriptBuf[512];	
		if( strlen( OrgDB->MagicBase.EndUseLua ) != 0 )
		{
			sprintf_s( LuaScriptBuf , sizeof(LuaScriptBuf) , "%s(%d)" , OrgDB->MagicBase.EndUseLua , List[i].Result );
			LUA_VMClass::PCallByStrArg( LuaScriptBuf , Owner->GUID() ,Target->GUID() );
		}

		if(		List[i].Result == EM_MagicAtkResult_Absorb 
			||	List[i].Result == EM_MagicAtkResult_NoEffect
			||	List[i].Result == EM_MagicAtkResult_Miss )
		{
			if( m_IsMissErase != false )
				m_ProcTargetList[i]->IsDel = true;
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
		else if( List[i].Result == EM_MagicAtkResult_Cancel )
		{
			if( m_IsMissErase )
				m_ProcTargetList[i]->IsDel = true;

		}
		else
		{
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
	}



	NetSrv_MagicChild::MagicAssistRange(	Owner->GUID()
		,	OrgDB->GUID
		,	MagicProc->SerialID
		,   List );


	return true;
}
//----------------------------------------------------------------------------------------
bool MagicProcessClass::Magic04_SummonItem			( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc )
{

	int     i;
	static vector< MagicAssistEffectInfoStruct > List;

	List.clear();
	MagicAssistEffectInfoStruct Temp;
	Temp.EffectTime =   0;

	for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
	{
		Temp.Result = EM_MagicAtkResult_Normal;
		m_ProcTargetList[i]->Role->GiveItem( OrgDB->MagicBase.SummonItem.ObjID , 1 , EM_ActionType_MagicSummonItem , NULL , "" );

		Temp.GItemID = m_ProcTargetList[i]->Role->GUID();
		Temp.MagicLv = MagicProc->MagicLv;
		List.push_back( Temp );
	}

	for( unsigned i =  0  ; i < List.size() ; i++ )
	{	
		RoleDataEx* Target = m_ProcTargetList[i]->Role;
		char	LuaScriptBuf[512];	
		if( strlen( OrgDB->MagicBase.EndUseLua ) != 0 )
		{
			sprintf_s( LuaScriptBuf , sizeof(LuaScriptBuf) , "%s(%d)" , OrgDB->MagicBase.EndUseLua , List[i].Result );
			LUA_VMClass::PCallByStrArg( LuaScriptBuf , Owner->GUID() ,Target->GUID() );
		}

		if(		List[i].Result == EM_MagicAtkResult_Absorb 
			||	List[i].Result == EM_MagicAtkResult_NoEffect
			||	List[i].Result == EM_MagicAtkResult_Miss )
		{
			if( m_IsMissErase != false )
				m_ProcTargetList[i]->IsDel = true;
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
		else if( List[i].Result == EM_MagicAtkResult_Cancel )
		{
			if( m_IsMissErase )
				m_ProcTargetList[i]->IsDel = true;

		}
		else
		{
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
	}

	NetSrv_MagicChild::MagicAssistRange(	Owner->GUID()
		,	OrgDB->GUID
		,	MagicProc->SerialID
		,   List );


	return true;
}
//----------------------------------------------------------------------------------------
bool MagicProcessClass::Magic05_Steal				( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc )
{

	int     i;
	static vector< MagicAssistEffectInfoStruct > List;

	List.clear();
	MagicAssistEffectInfoStruct Temp;
	Temp.EffectTime =   0;

	for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
	{
		RoleDataEx* Target = m_ProcTargetList[i]->Role;

		switch( m_ProcTargetList[i]->HitType )
		{
		case EM_MAGIC_HIT_TYPE_HIT:			//命中
		case EM_MAGIC_HIT_TYPE_CRITICAL:	//致命一擊
			{
				if( Target->TempData.Attr.DisableMagicSet & OrgDB->MagicBase.MagicGroupSet )
				{
					Temp.Result = EM_MagicAtkResult_Miss;
					break;
				}

				 
				if(		Target->IsNPC() 
					&&	rand() %100 <= OrgDB->MagicBase.Steal.SuccessRate + Owner->TempData.Attr.Mid.Body[ EM_WearEqType_StealRate ] )
				{
					Temp.Result = EM_MagicAtkResult_Normal;

					//如果沒錢
					if( Target->PlayerBaseData->Body.Money == 0 )
					{			
						if( Target->PlayerBaseData->Body.Count == 0 )
						{
							Temp.Result = EM_MagicAtkResult_Miss;
						}				
						else
						{
							Owner->GiveItem( Target->PlayerBaseData->Body.Item[ Target->PlayerBaseData->Body.Count - 1 ] , EM_ActionType_Steal , Target , "" );
							Target->PlayerBaseData->Body.Count --;
						}
					}
					else
					{
						if( rand() % (Target->PlayerBaseData->Body.Count + 1 ) == 0 )
						{
							Owner->AddBodyMoney( Target->PlayerBaseData->Body.Money , Target , EM_ActionType_Steal , true );
							Target->PlayerBaseData->Body.Money = 0;
						}
						else
						{
							Owner->GiveItem( Target->PlayerBaseData->Body.Item[ Target->PlayerBaseData->Body.Count - 1 ] , EM_ActionType_Steal , Target , "" );
							Target->PlayerBaseData->Body.Count --;
						}
					}
				}
				else
				{
					Temp.Result = EM_MagicAtkResult_Miss;
					Target->CalHate( Owner , 0 , 1 );
				}

				
			}

			break;
		case EM_MAGIC_HIT_TYPE_MISS:		//MISS
			Temp.Result = EM_MagicAtkResult_Miss;
			break;
		case EM_MAGIC_HIT_TYPE_DODGE:		//MISS
			Temp.Result = EM_MagicAtkResult_Dodge;
			break;

		case EM_MAGIC_HIT_TYPE_CANCEL:
			Temp.Result = EM_MagicAtkResult_Cancel;
			break;
		case EM_MAGIC_HIT_TYPE_NO_EFFECT:	//無效
			Temp.Result = EM_MagicAtkResult_NoEffect;
			break;

		}

		Temp.GItemID = Target->GUID();
		Temp.GItemID = MagicProc->MagicLv;
		List.push_back( Temp );
	}

	for( unsigned i =  0  ; i < List.size() ; i++ )
	{			
		RoleDataEx* Target = m_ProcTargetList[i]->Role;
		char	LuaScriptBuf[512];	
		if( strlen( OrgDB->MagicBase.EndUseLua ) != 0 )
		{
			sprintf_s( LuaScriptBuf , sizeof(LuaScriptBuf) , "%s(%d)" , OrgDB->MagicBase.EndUseLua , List[i].Result );
			LUA_VMClass::PCallByStrArg( LuaScriptBuf , Owner->GUID() ,Target->GUID() );
		}

		if(		List[i].Result == EM_MagicAtkResult_Absorb 
			||	List[i].Result == EM_MagicAtkResult_NoEffect
			||	List[i].Result == EM_MagicAtkResult_Miss )
		{
			if( m_IsMissErase != false )
				m_ProcTargetList[i]->IsDel = true;
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
		else if( List[i].Result == EM_MagicAtkResult_Cancel )
		{
			if( m_IsMissErase )
				m_ProcTargetList[i]->IsDel = true;

		}
		else
		{
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
	}

	NetSrv_MagicChild::MagicAssistRange(	Owner->GUID()
		,	OrgDB->GUID
		,	MagicProc->SerialID
		,   List );


	return true;
}
//----------------------------------------------------------------------------------------
bool MagicProcessClass::Magic06_ItemRunPlot			( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc )
{

	MagicItemRunPlot& ItemRunPlot = OrgDB->MagicBase.ItemRunPlot;

	if( ItemRunPlot.GroupID != 0 )
	{
		//找周圍是否有此類的Group 需先消去
		vector< RoleDataEx* >& ChildRole = *( Global::SearchRangeChildObject( Owner , ItemRunPlot.GroupID ) );

		for( int i = 0 ; i < (int)ChildRole.size() ; i++ )
		{
			ChildRole[i]->Destroy( "Magic06_ItemRunPlot");
		}
	}

	ObjectModeStruct	Mode;	
	Mode._Mode = 0;
	Mode.Show = Mode.Gravity = Mode.SearchenemyIgnore = 1 ;
//	Mode._Mode = Mode._Mode | OrgDB->MagicBase.ItemRunPlot.Mode._Mode;

	Mode.Mark			= OrgDB->MagicBase.ItemRunPlot.Mark;
	Mode.Fight			= OrgDB->MagicBase.ItemRunPlot.Fight;
	Mode.NotShowHPMP	= OrgDB->MagicBase.ItemRunPlot.NotShowHPMP;
	Mode.EnemyHide		= OrgDB->MagicBase.ItemRunPlot.EnemyHide;

	float	X	= MagicProc->TargetX;
	float	Y	= MagicProc->TargetY;
	float	Z	= MagicProc->TargetZ;
	float	Dir = Owner->CalDir( X - Owner->Pos()->X  , Z - Owner->Pos()->Z );

	int ItemID = Global::CreateObj( ItemRunPlot.ObjID , X , Y , Z , Dir , 1 );

	BaseItemObject* ItemClass = Global::GetObj( ItemID );
	if( ItemClass == NULL )
	{
		//Owner->Msg("建立物件失敗!!");
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_MagicError_CreateObjectError );
		return false;
	}

	RoleDataEx* ItemRole = ItemClass->Role();


	ItemRole->BaseData.Mode._Mode = Mode._Mode;
	ItemRole->TempData.Sys.OwnerGUID	= Owner->GUID();
	ItemRole->TempData.Sys.OwnerDBID		= Owner->DBID();
	ItemRole->TempData.Sys.OwnerGroupID	= ItemRunPlot.GroupID;
	ItemRole->TempData.AI.CampID = Owner->TempData.AI.CampID;
	//死亡觸發的法術
	ItemRole->TempData.OnDeadMagic = ItemRunPlot.OnDeadMagic;


	Global::AddToPartition( ItemID , Owner->BaseData.RoomID );

	if( strlen( ItemRunPlot.PlotName ) != 0 )
		ItemClass->PlotVM()->CallLuaFunc( ItemRunPlot.PlotName , Owner->GUID() , 0 );

	switch( ItemRunPlot.Type )
	{
	case EM_MagicItemRunPlot_Plot:	//劇情類

		break;
	case EM_MagicItemRunPlot_Mine:	//地雷類
		{
			ArgTransferStruct* Arg = ItemClass->PlotVM()->CallLuaFunc( "Sys_MineProcess" , Owner->GUID() , 0 );
			Arg->PushValue( "MagicObjID" , OrgDB->GUID );
			Arg->PushValue( "MagicLv" , MagicProc->MagicLv );
		}
		break;
	case EM_MagicItemRunPlot_Staff:	//插杖類
		SpellMagic( ItemID , ItemID , X , Y , Z , ItemRunPlot.UseMagic , MagicProc->MagicLv );
		break;
	}

	ItemRole->OwnerGUID( Owner->GUID() );
	ItemRole->BaseData.Mode.RelyOwnerPower = !(OrgDB->MagicBase.ItemRunPlot.NotRelyOwnerPower);

	ItemRole->LiveTime( ItemRunPlot.LiveTime * 1000 ,"SYS Magic06_ItemRunPlot" );


	static vector< MagicAssistEffectInfoStruct > List;

	NetSrv_MagicChild::MagicAssistRange(	Owner->GUID()
		,	OrgDB->GUID
		,	MagicProc->SerialID
		,   List );


	for( unsigned i =  0  ; i < List.size() ; i++ )
	{	
		RoleDataEx* Target = m_ProcTargetList[i]->Role;
		char	LuaScriptBuf[512];	
		if( strlen( OrgDB->MagicBase.EndUseLua ) != 0 )
		{
			sprintf_s( LuaScriptBuf , sizeof(LuaScriptBuf) , "%s(%d)" , OrgDB->MagicBase.EndUseLua , List[i].Result );
			LUA_VMClass::PCallByStrArg( LuaScriptBuf , Owner->GUID() ,Target->GUID() );
		}

		if(		List[i].Result == EM_MagicAtkResult_Absorb 
			||	List[i].Result == EM_MagicAtkResult_NoEffect
			||	List[i].Result == EM_MagicAtkResult_Miss )
		{
			if( m_IsMissErase != false )
				m_ProcTargetList[i]->IsDel = true;
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
		else if( List[i].Result == EM_MagicAtkResult_Cancel )
		{
			if( m_IsMissErase )
				m_ProcTargetList[i]->IsDel = true;

		}
		else
		{
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
	}
	return true;
}
//----------------------------------------------------------------------------------------
bool MagicProcessClass::Magic07_RunPlot				( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc )
{
	int     i;
	static vector< MagicAssistEffectInfoStruct > List;

	List.clear();
	MagicAssistEffectInfoStruct Temp;
	Temp.EffectTime =   0;
	char*	PlotName= OrgDB->MagicBase.RunPlot.PlotName ;

	BaseItemObject* OwnerObj = BaseItemObject::GetObj( Owner->GUID() );
	if( OwnerObj == NULL )
		return false;

	for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
	{
		switch( m_ProcTargetList[i]->HitType )
		{
		case EM_MAGIC_HIT_TYPE_HIT:			//命中
		case EM_MAGIC_HIT_TYPE_CRITICAL:	//致命一擊
			{
				Temp.Result = EM_MagicAtkResult_Normal;			
				Temp.GItemID = m_ProcTargetList[i]->Role->GUID();
				LUA_VMClass::PCallByStrArg( PlotName , Owner->GUID() , Temp.GItemID );
			}
			break;
		case EM_MAGIC_HIT_TYPE_CANCEL:
			Temp.Result = EM_MagicAtkResult_Cancel;
			break;
		case EM_MAGIC_HIT_TYPE_NO_EFFECT:	//無效
			Temp.Result = EM_MagicAtkResult_NoEffect;
			break;
		case EM_MAGIC_HIT_TYPE_DODGE:		//MISS
			Temp.Result = EM_MagicAtkResult_Dodge;
			break;

		case EM_MAGIC_HIT_TYPE_MISS:		//MISS
		default:
			Temp.Result = EM_MagicAtkResult_Miss;
			break;
		}
	
		Temp.GItemID = m_ProcTargetList[i]->Role->GUID();
		Temp.MagicLv = MagicProc->MagicLv;
		List.push_back( Temp );
		
	}

	NetSrv_MagicChild::MagicAssistRange(	Owner->GUID()
		,	OrgDB->GUID
		,	MagicProc->SerialID
		,   List );

	for( unsigned i =  0  ; i < List.size() ; i++ )
	{	
		RoleDataEx* Target = m_ProcTargetList[i]->Role;
		char	LuaScriptBuf[512];	
		if( strlen( OrgDB->MagicBase.EndUseLua ) != 0 )
		{
			sprintf_s( LuaScriptBuf , sizeof(LuaScriptBuf) , "%s(%d)" , OrgDB->MagicBase.EndUseLua , List[i].Result );
			LUA_VMClass::PCallByStrArg( LuaScriptBuf , Owner->GUID() ,Target->GUID() );
		}

		if(		List[i].Result == EM_MagicAtkResult_Absorb 
			||	List[i].Result == EM_MagicAtkResult_NoEffect
			||	List[i].Result == EM_MagicAtkResult_Miss )
		{
			if( m_IsMissErase != false )
				m_ProcTargetList[i]->IsDel = true;
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
		else if( List[i].Result == EM_MagicAtkResult_Cancel )
		{
			if( m_IsMissErase )
				m_ProcTargetList[i]->IsDel = true;

		}
		else
		{
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
	}
	return true;

}
//----------------------------------------------------------------------------------------
bool MagicProcessClass::Magic08_Raise				( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc )
{

	int     i;
	static vector< MagicAssistEffectInfoStruct > List;

	List.clear();
	MagicAssistEffectInfoStruct Temp;
	Temp.EffectTime =   0;

	for( i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
	{
		RoleDataEx* Target = m_ProcTargetList[i]->Role;
		BaseItemObject* TargetObj = BaseItemObject::GetObj( Target->GUID() );

		if( Target->IsDead() == false )
			Temp.Result = EM_MagicAtkResult_NoEffect;
		else
		{
			Temp.Result = EM_MagicAtkResult_Normal;
			NetSrv_Magic::S_RaiseMagicNotify		( Target->GUID() , Owner->GUID() , OrgDB->GUID );

			Target->PlayerBaseData->RaiseInfo.Mode.MagicRaiseEnabled = true;
			int ExpPrecnt = OrgDB->MagicBase.Raise.ExpPrecnt + ( MagicProc->MagicLv * OrgDB->MagicBase.Revive_SkillLVArg * OrgDB->MagicBase.Raise.ExpPrecnt / 100 );
			if( ExpPrecnt > 100 )
				ExpPrecnt = 100;
			if( Target->PlayerBaseData->RaiseInfo.ExpRate < ExpPrecnt )
			{
				Target->PlayerBaseData->RaiseInfo.ExpRate = ExpPrecnt;				
			}
			Target->PlayerBaseData->RaiseInfo.RevX = Owner->X();
			Target->PlayerBaseData->RaiseInfo.RevY = Owner->Y();
			Target->PlayerBaseData->RaiseInfo.RevZ = Owner->Z();

		}

		Temp.GItemID = m_ProcTargetList[i]->Role->GUID();
		Temp.MagicLv = MagicProc->MagicLv;
		List.push_back( Temp );
	}
	if( OrgDB->MagicBase.Setting.IgnoreSpellCureMagicClear == false )
		Owner->TempData.BackInfo.BuffClearTime.Spell_Cure = true;

	NetSrv_MagicChild::MagicAssistRange(	Owner->GUID()
		,	OrgDB->GUID
		,	MagicProc->SerialID
		,   List );

	for( unsigned i =  0  ; i < List.size() ; i++ )
	{	
		RoleDataEx* Target = m_ProcTargetList[i]->Role;
		char	LuaScriptBuf[512];	
		if( strlen( OrgDB->MagicBase.EndUseLua ) != 0 )
		{
			sprintf_s( LuaScriptBuf , sizeof(LuaScriptBuf) , "%s(%d)" , OrgDB->MagicBase.EndUseLua , List[i].Result );
			LUA_VMClass::PCallByStrArg( LuaScriptBuf , Owner->GUID() ,Target->GUID() );
		}

		if(		List[i].Result == EM_MagicAtkResult_Absorb 
			||	List[i].Result == EM_MagicAtkResult_NoEffect
			||	List[i].Result == EM_MagicAtkResult_Miss )
		{
			if( m_IsMissErase != false )
				m_ProcTargetList[i]->IsDel = true;
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
		else if( List[i].Result == EM_MagicAtkResult_Cancel )
		{
			if( m_IsMissErase )
				m_ProcTargetList[i]->IsDel = true;

		}
		else
		{
			m_ProcTargetList[i]->IsSpellOneMagic = true;
		}
	}
	return true;

}
//----------------------------------------------------------------------------------------
int  MagicProcessClass::ProcMagicID( )
{
	return m_ProcMagicID;
}
//////////////////////////////////////////////////////////////////////////
//不檢查
void	MagicProcessClass::Check_None		( RoleDataEx* Owner , int Arg1 , int Arg2 )
{
	m_ProcTargetList.clear();

	for( unsigned i = 0 ; i < m_TargetList.size() ; i++ )
	{
		if(		m_TargetList[i].IsDel != false 
			||	m_TargetList[i].IsSpellOneMagic != false )
			continue;
		m_ProcTargetList.push_back( &m_TargetList[i] );
	}
}
//亂數
void	MagicProcessClass::Check_Rand		( RoleDataEx* Owner , int RandValue , int Arg2 )
{
	m_ProcTargetList.clear();
	for( unsigned i = 0 ; i < m_TargetList.size() ; i++ )
	{
		if(		m_TargetList[i].IsDel != false 
			||	m_TargetList[i].IsSpellOneMagic != false )
			continue;

		if( rand() % 100 >= RandValue )
			continue;

		m_ProcTargetList.push_back( &m_TargetList[i] );
	}

}
//裝備
void	MagicProcessClass::Check_MyEQ		( RoleDataEx* Owner , int RandValue , int EQObjID )
{
	bool IsFindEQ = false;
	m_ProcTargetList.clear();
	for( int j = 0 ; j < EM_EQWearPos_MaxCount ; j++ )
	{
		ItemFieldStruct& Item = Owner->BaseData.EQ.Item[j];
		if( Item.OrgObjID != EQObjID )
			continue;

		if( Item.Mode.EQCannotEquipment )
			continue;

		IsFindEQ = true;
		break;
	}

	if( IsFindEQ == false )
		return;


	for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
	{		
		if(		m_TargetList[ID].IsDel != false 
			||	m_TargetList[ID].IsSpellOneMagic != false )
			continue;

		if( rand() % 100 >= RandValue )
			continue;

		m_ProcTargetList.push_back( &m_TargetList[ID] );
	}

}
void	MagicProcessClass::Check_TargetEQ	( RoleDataEx* Owner , int RandValue , int EQObjID )
{
	m_ProcTargetList.clear();
	for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
	{		
		if(		m_TargetList[ID].IsDel != false 
			||	m_TargetList[ID].IsSpellOneMagic != false )
			continue;

		if( rand() % 100 >= RandValue )
			continue;

		RoleDataEx* Other = m_TargetList[ID].Role;

		for( int j = 0 ; j < EM_EQWearPos_MaxCount ; j++ )
		{
			ItemFieldStruct& Item = Other->BaseData.EQ.Item[j];
			if( Item.OrgObjID != EQObjID )
				continue;

			if( Item.Mode.EQCannotEquipment )
				continue;

			m_ProcTargetList.push_back( &m_TargetList[ID] );
			break;
		}

	}

}
//自己裝備類型( 機率 , 裝備類型 )	
void	MagicProcessClass::Check_MyEqType			( RoleDataEx* Owner , int RandValue , int Eqtype )			
{
	m_ProcTargetList.clear();
	GameObjectWeaponENUM WeaponType = (GameObjectWeaponENUM)Eqtype;
	if( Owner->TempData.Attr.MainWeaponType == WeaponType && Owner->BaseData.EQ.MainHand.Mode.EQCannotEquipment == false )
	{

		for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
		{		
			if(		m_TargetList[ID].IsDel != false 
				||	m_TargetList[ID].IsSpellOneMagic != false )
				continue;

			if( rand() % 100 >= RandValue )
				continue;

			m_ProcTargetList.push_back( &m_TargetList[ID] );
		}
	}
}
//目標裝備類型( 機率 , 裝備類型 )	
void	MagicProcessClass::Check_TargetEqType		( RoleDataEx* Owner , int RandValue , int Eqtype )			
{
	m_ProcTargetList.clear();
	GameObjectWeaponENUM WeaponType = (GameObjectWeaponENUM)Eqtype;
	for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
	{		
		if(		m_TargetList[ID].IsDel != false 
			||	m_TargetList[ID].IsSpellOneMagic != false )
			continue;

		RoleDataEx* Other = m_TargetList[ID].Role;
		if( Other->TempData.Attr.MainWeaponType != WeaponType && Other->BaseData.EQ.MainHand.Mode.EQCannotEquipment != false )
			continue;

		if( rand() % 100 >= RandValue )
			continue;

		m_ProcTargetList.push_back( &m_TargetList[ID] );
	}
}

//物品
void	MagicProcessClass::Check_MyItem		( RoleDataEx* Owner , int RandValue , int ItemObjID )
{
	m_ProcTargetList.clear();
	bool	IsFindItem = false;

	if( Owner->CheckKeyItem( ItemObjID ) )
		IsFindItem = true;

	if( IsFindItem == false )
	{
		for( int j = 0 ; j < Owner->PlayerBaseData->Body.Count ; j++ )
		{
			ItemFieldStruct& Item = Owner->PlayerBaseData->Body.Item[j];
			if( Item.OrgObjID == ItemObjID )
			{
				IsFindItem = true;			
				break;
			}
		}
	}

	if( IsFindItem == false )
	{
		for( int j = 0 ; j < EM_EQWearPos_MaxCount ; j++ )
		{
			ItemFieldStruct& Item = Owner->BaseData.EQ.Item[j];
			if( Item.OrgObjID != ItemObjID )
				continue;

			IsFindItem = true;
			break;
		}
	}


	if( IsFindItem == false )
		return;
	

	for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
	{		
		if(		m_TargetList[ID].IsDel != false 
			||	m_TargetList[ID].IsSpellOneMagic != false )
			continue;

		RoleDataEx* Other = m_TargetList[ID].Role;	

		if( rand() % 100 >= RandValue )
			continue;

		m_ProcTargetList.push_back( &m_TargetList[ID] );		
	}
}
void	MagicProcessClass::Check_TargetItem	( RoleDataEx* Owner , int RandValue , int ItemObjID )
{
	m_ProcTargetList.clear();
	bool	IsFindItem;

	for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
	{		
		if(		m_TargetList[ID].IsDel != false 
			||	m_TargetList[ID].IsSpellOneMagic != false )
			continue;

		RoleDataEx* Other = m_TargetList[ID].Role;	

		if( rand() % 100 >= RandValue )
			continue;

		GameObjDbStructEx* ItemObj = Global::GetObjDB( ItemObjID );
		if( ItemObj->IsKeyItem() != false )
		{
			if( Other->CheckKeyItem( ItemObjID ) != false )
			{
				m_ProcTargetList.push_back( &m_TargetList[ID] );
				continue;
			}
		}

		if( ItemObj->IsItem() == false )
		{
			continue;
		}

		IsFindItem = false;
		for( int j = 0 ; j < Other->PlayerBaseData->Body.Count ; j++ )
		{
			ItemFieldStruct& Item = Other->PlayerBaseData->Body.Item[j];
			if( Item.OrgObjID == ItemObjID )
			{
				IsFindItem = true;
				m_ProcTargetList.push_back( &m_TargetList[ID] );
				break;
			}
		}
		if( IsFindItem != false )
		{
			continue;
		}

		for( int j = 0 ; j < EM_EQWearPos_MaxCount ; j++ )
		{
			ItemFieldStruct& Item = Other->BaseData.EQ.Item[j];
			if( Item.OrgObjID != ItemObjID )
				continue;

			m_ProcTargetList.push_back( &m_TargetList[ID] );
			break;
		}

	}
}
//buff
void	MagicProcessClass::Check_MyBuff		( RoleDataEx* Owner , int RandValue , int BuffID )
{
	m_ProcTargetList.clear();
	bool IsFindBuf = false;

	for( int j = 0 ; j < Owner->BaseData.Buff.Size() ; j++ )
	{
		BuffBaseStruct& Buf = Owner->BaseData.Buff[j];
		if( Buf.BuffID != BuffID )
			continue;
		IsFindBuf = true;
		break;
	}

	if( IsFindBuf == false )
		return;

	for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
	{		
		if(		m_TargetList[ID].IsDel != false 
			||	m_TargetList[ID].IsSpellOneMagic != false )
			continue;

		if( rand() % 100 >= RandValue )
			continue;

		m_ProcTargetList.push_back( &m_TargetList[ID] );

	}
}
void	MagicProcessClass::Check_TargetBuff	( RoleDataEx* Owner , int RandValue , int BuffID )
{
	m_ProcTargetList.clear();

	for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
	{		
		if(		m_TargetList[ID].IsDel != false 
			||	m_TargetList[ID].IsSpellOneMagic != false )
			continue;

		if( rand() % 100 >= RandValue )
			continue;

		RoleDataEx* Other = m_TargetList[ID].Role;


		for( int j = 0 ; j < Other->BaseData.Buff.Size() ; j++ )
		{
			BuffBaseStruct& Buf = Other->BaseData.Buff[j];
			if( Buf.BuffID != BuffID )
				continue;
			m_ProcTargetList.push_back( &m_TargetList[ID] );
		}
	}
}
//自己戰鬥( 機率 , 0非戰鬥/1戰鬥  )
void	MagicProcessClass::Check_MyAttackType		( RoleDataEx* Owner , int RandValue , int Type )
{
	m_ProcTargetList.clear();
	if(		Owner->IsAttackMode() && Type != 1 
		|| !Owner->IsAttackMode() && Type != 0 ) 
		return;

	for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
	{		
		if( rand() % 100 >= RandValue )
			continue;

		RoleDataEx* Other = m_TargetList[ID].Role;
		m_ProcTargetList.push_back( &m_TargetList[ID] );
	}
}
//目標戰鬥( 機率 , 0非戰鬥/1戰鬥  )
void	MagicProcessClass::Check_TargetAttackType	( RoleDataEx* Owner , int RandValue , int Type )
{
	m_ProcTargetList.clear();
	
	for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
	{		
		RoleDataEx* Target = m_TargetList[ID].Role;
		if(		Target->IsAttackMode() && Type != 1 
			|| !Target->IsAttackMode() && Type != 0 ) 
			continue;

		if( rand() % 100 >= RandValue )
			continue;

		RoleDataEx* Other = m_TargetList[ID].Role;
		m_ProcTargetList.push_back( &m_TargetList[ID] );
	}
}
//目標種族( 機率 , 種族ID  )
void	MagicProcessClass::Check_TargetRace		( RoleDataEx* Owner , int RandValue , int Type )
{
	m_ProcTargetList.clear();
	for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
	{		
		RoleDataEx* Target = m_TargetList[ID].Role;
		if(		Target->BaseData.Race != Type )
			continue;

		if( rand() % 100 >= RandValue )
			continue;

		RoleDataEx* Other = m_TargetList[ID].Role;
		m_ProcTargetList.push_back( &m_TargetList[ID] );
	}
}
//自己HP低於( 機率 , HP百分比 )
void	MagicProcessClass::Check_MyHp				( RoleDataEx* Owner , int RandValue , int Type )
{
	m_ProcTargetList.clear();
	if( int( Owner->TempData.Attr.Fin.MaxHP ) >= Owner->BaseData.HP * 100 / Type )
		return;
	
	for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
	{		
		if( rand() % 100 >= RandValue )
			continue;

		RoleDataEx* Other = m_TargetList[ID].Role;
		m_ProcTargetList.push_back( &m_TargetList[ID] );
	}
}

//位置
void	MagicProcessClass::Check_MyPos		( RoleDataEx* Owner , int RandValue , int PosID )
{
	m_ProcTargetList.clear();
}
void	MagicProcessClass::Check_TargetPos	( RoleDataEx* Owner , int RandValue , int PosID )
{
	m_ProcTargetList.clear();
}


//時間
void	MagicProcessClass::Check_Time		( RoleDataEx* Owner , int RandValue , int Time )
{
	m_ProcTargetList.clear();
}
//氣候
void	MagicProcessClass::Check_Weather		( RoleDataEx* Owner , int RandValue , int WeatherID )
{
	m_ProcTargetList.clear();
}

//自己Buff Group( 機率 , 擁有某Buff群組 )
void	MagicProcessClass::Check_MyBuffGroup		( RoleDataEx* Owner , int RandValue , int MagicGroupID )
{
	m_ProcTargetList.clear();
	bool IsFindBuf = false;

	for( int j = 0 ; j < Owner->BaseData.Buff.Size() ; j++ )
	{
		BuffBaseStruct& Buf = Owner->BaseData.Buff[j];
		if( Buf.Magic->MagicBase.MagicGroupID  != MagicGroupID )
			continue;
		IsFindBuf = true;
		break;
	}

	if( IsFindBuf == false )
		return;

	for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
	{		
		if(		m_TargetList[ID].IsDel != false 
			||	m_TargetList[ID].IsSpellOneMagic != false )
			continue;

		if( rand() % 100 >= RandValue )
			continue;

		m_ProcTargetList.push_back( &m_TargetList[ID] );

	}
}
//目標Buff Group( 機率 , 擁有某Buff群組 )
void	MagicProcessClass::Check_TargetBuffGroup		( RoleDataEx* Owner , int RandValue , int MagicGroupID )
{
	m_ProcTargetList.clear();

	for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
	{		
		if(		m_TargetList[ID].IsDel != false 
			||	m_TargetList[ID].IsSpellOneMagic != false )
			continue;

		if( rand() % 100 >= RandValue )
			continue;

		RoleDataEx* Other = m_TargetList[ID].Role;


		for( int j = 0 ; j < Other->BaseData.Buff.Size() ; j++ )
		{
			BuffBaseStruct& Buf = Other->BaseData.Buff[j];
			if( Buf.Magic->MagicBase.MagicGroupID  != MagicGroupID )
				continue;
			m_ProcTargetList.push_back( &m_TargetList[ID] );
		}
	}
}
void	MagicProcessClass::Check_LV( RoleDataEx* Owner , int RandValue , int Lv )
{
	m_ProcTargetList.clear();
	for( unsigned i = 0 ; i < m_TargetList.size() ; i++ )
	{
		if(		m_TargetList[i].IsDel != false 
			||	m_TargetList[i].IsSpellOneMagic != false )
			continue;
		if( Owner->Level() < Lv )
			continue;

		if( rand() % 100 >= RandValue )
			continue;

		m_ProcTargetList.push_back( &m_TargetList[i] );
	}
}

void	MagicProcessClass::Check_MySkill(  RoleDataEx* Owner , int RandValue , int SkillID  )
{
	bool IsFind = false;
	m_ProcTargetList.clear();

	for( int i = 0 ; i < _MAX_SPSkill_COUNT_+_MAX_NormalSkill_COUNT_+_MAX_NormalSkill_COUNT_  ; i++ )
	{
		if( Owner->PlayerTempData->Skill.AllSkill[i] == SkillID )
		{
			IsFind = true;
			break;
		}
	}

	if( IsFind == false )
		return;

	for( unsigned ID = 0 ; ID < m_TargetList.size() ; ID++ )
	{		
		if(		m_TargetList[ID].IsDel != false 
			||	m_TargetList[ID].IsSpellOneMagic != false )
			continue;

		if( rand() % 100 >= RandValue )
			continue;

		m_ProcTargetList.push_back( &m_TargetList[ID] );

	}

	return ;
}

//施法清除某人的仇恨值
void	MagicProcessClass::ClsHateList( RoleDataEx* Speller , RoleDataEx* Target , int AddSpellerHatePoint  )
{
	vector<BaseSortStruct>& SearchResult = *( Global::SearchRangeObject( Speller , Target->X() , Target->Y() , Target->Z() , EM_SearchRange_NPC , 300 ) );

	for( int j = 0 ; j <  (int)SearchResult.size() ; j++ )
	{
		RoleDataEx* Other = (RoleDataEx*)SearchResult[j].Data;

		NPCHateList& NPCHate = Other->TempData.NPCHate;
		//搜尋是否在仇恨表內		
		for( int i = 0 ; i < NPCHate.Hate.Size() ; i++ )
		{
			if(	NPCHate.Hate[i].ItemID == Target->GUID() )
			{
				if( Other->AttackTargetID() == Target->GUID() )
				{
					NPCHate.MaxHitPoint = 0;
				}
				NPCHate.Hate.Erase( i );

				if( AddSpellerHatePoint <= 0 )
				{
					if( NPCHate.Hate.Size() == 0 )
					{
						Other->StopAttack();
					}
					else
					{
						//找出有最高仇恨的人
						NPCHateStruct* MaxHate = NPCHate.GetMaxHate();
						if( MaxHate != NULL )
						{
							NPCHate.MaxHitPoint = MaxHate->TotalHatePoint();
							Other->AttackTarget( MaxHate->ItemID );	
						}
					}

				}
				else
				{
					Other->CalHate( Speller , 0 , AddSpellerHatePoint );
				}

				break;
			}	
		}
	}

	Target->StopAttack();
	Target->IsAttackMode( false );

}

//施法清除某人的仇恨值
void	MagicProcessClass::ClsHateListToOnePoint( RoleDataEx* Speller , RoleDataEx* Target , int AddSpellerHatePoint  )
{
	vector<BaseSortStruct>& SearchResult = *( Global::SearchRangeObject( Speller , Target->X() , Target->Y() , Target->Z() , EM_SearchRange_NPC , 300 ) );

	for( int j = 0 ; j <  (int)SearchResult.size() ; j++ )
	{
		RoleDataEx* Other = (RoleDataEx*)SearchResult[j].Data;

		NPCHateList& NPCHate = Other->TempData.NPCHate;
		//搜尋是否在仇恨表內		
		for( int i = 0 ; i < NPCHate.Hate.Size() ; i++ )
		{
			if(	NPCHate.Hate[i].ItemID == Target->GUID() )
			{
				if( Other->AttackTargetID() == Target->GUID() )
				{
					NPCHate.MaxHitPoint = 0;
				}
				NPCHate.Hate[i].HatePoint = 1;

				if( AddSpellerHatePoint > 0 )				
				{
					Other->CalHate( Speller , 0 , AddSpellerHatePoint );
				}
				else
				{
					//找出有最高仇恨的人
					NPCHateStruct* MaxHate = NPCHate.GetMaxHate();
					if( MaxHate != NULL )
					{
						NPCHate.MaxHitPoint = MaxHate->TotalHatePoint();
						Other->AttackTarget( MaxHate->ItemID );					
					}
				}

				break;
			}	
		}
	}

	Target->StopAttack();
}

void	MagicProcessClass::CheckUseLuaProc( RoleDataEx* Owner , GameObjDbStructEx* MagicBaseOrgDB )
{
	if( strlen( MagicBaseOrgDB->MagicBase.CheckUseLua ) == 0 )
		return;

	for( int i = 0 ; i < (int)m_ProcTargetList.size() ; i++ )
	{
		if( m_ProcTargetList[i]->HitType != EM_MAGIC_HIT_TYPE_HIT )
			continue;

		RoleDataEx* Other = m_ProcTargetList[i]->Role;
		vector<MyVMValueStruct> RetList;

		if( LUA_VMClass::PCallByStrArg( MagicBaseOrgDB->MagicBase.CheckUseLua , Owner->GUID() , Other->GUID() , &RetList , 1 ) != false )
		{
			if( RetList.size() == 1 && RetList[0].Flag == false)
			{
				//m_ProcTargetList[i]->HitType = EM_MAGIC_HIT_TYPE_CANCEL;
				m_ProcTargetList.erase( m_ProcTargetList.begin() + i );
				i--;
			}		
		}
	}
}