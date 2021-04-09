#pragma		warning (disable:4786)
#pragma     unmanaged
#include <math.h>
#include "NPCAIBase.h"
#include "../Global.h"
#include "../../NetWalker_Member/NetWakerGSrvInc.h"
#include "../magicproc/MagicProcess.h"
#include "../pathmanage/NPCMoveFlagManage.h"
//----------------------------------------------------------------------------------------------------------
NpcAIBaseClass::NpcAIBaseClass( BaseItemObject* Obj )
{
	_OwnerObj   = Obj;
	_Owner      = Obj->Role();
	_ProcTime   = 0;           //��J���ƥ󪺮ɶ�    
	_MoveToFlagEnabled	= true;
	_ClickToFaceEnabled = true;
	_IsBeginAttack = false;
	_IsMagicEscape = false;
	_IsStopLuaOnce = false;
}
NpcAIBaseClass::~NpcAIBaseClass( )
{

}
//��ƪ�l��
void	NpcAIBaseClass::DeadInit()
{
	_ProcTime = 0;           //��J���ƥ󪺮ɶ�    
	_X = _Y = _Z = 0;        //�i�J�԰��Ҧb����m
	_IsBeginAttack = false;
	_NPCMoveFlag.Init();

}
//----------------------------------------------------------------------------------------------------------
void    NpcAIBaseClass::SetState( NPCAIStateEnum State )
{
	if( _Owner->IsDead() )
		State = EM_AIState_Idle;

	if( State == _State )
		return; 
	switch( State )
	{
	case EM_AIState_None:               //������
		return;
	case EM_AIState_Idle:               //���m
		{
		}
		break;
	case EM_AIState_Moving:             //���ʤ�
		break;
	case EM_AIState_RandMoving:         //�üƲ���
		break;
	case EM_AIState_FlagMoving:         //�üƲ���
		break;
	case EM_AIState_Follow:             //���H�ؼ�
		break;
	case EM_AIState_UnderAttack:        //�Q����(�L�k�}�Ҿ԰�)
		break;
	case EM_AIState_Attack:             //�������A�����m
		{

			PathProcStruct& Move = _Owner->TempData.Move;
			if(     _State != EM_AIState_Atk_Follow 
				&&  _State != EM_AIState_Atk_KeepDistance 
				&&  _State != EM_AIState_Atk_Escape			)
			{

				Move.RetX = _Owner->X();
				Move.RetY = _Owner->Y();
				Move.RetZ = _Owner->Z();
				_Owner->StopMoveNow();
//				_OwnerObj->Path()->ClearPath();
//				_Owner->IsWalk( false );
				_IsBeginAttack = true;
			}
			_Owner->IsWalk( false );
		}
		break;
	case EM_AIState_Atk_Follow:         //�԰��� ���H
		break;
	case EM_AIState_Atk_KeepDistance:   //�԰��� �O���Z��		
		break;
	case EM_AIState_Atk_Escape:         //�԰��� �k�]
		break;
	}

	_State = State;
	//�O�����A�������ɶ�
	_ProcTime = RoleDataEx::G_SysTime;
	return;
}
//----------------------------------------------------------------------------------------------------------
//���m�B�z
int NpcAIBaseClass::IdleProc() 
{
	//SpellMagic :
	//    1 1/60�����| �ۤv �P �P��O�_���ݭn���U�k�N            

	//RandMove :
	//    1 ���]�w�üƲ��ʸ�T
	//   x2 RandMove coldown ����

	//Follow :            
	//    1 ���D�H �ó]�w�����H��� 

	//Attack :
	//     1 �j����ĤH
	//--------------------------------------------------------------------------------
	if( _Owner->TempData.Sys.SecRoomID == -1 )
	{
		return 10;
	}

	if( _Owner->IsDead() )
	{
		return 10;
	}

	if( _Owner->TempData.AroundPlayer == 0 && rand() % 10 != 0 )
	//if( _Owner->TempData.AroundPlayer == 0 )
	{
		return 10;
	}

	if( _Owner->IsAttack() )
	{
		SetState( EM_AIState_Attack );
		return 1;
	}

	if( _Owner->IsUnderAttack() )
	{
		SetState( EM_AIState_UnderAttack );
		return 1;
	}

	if( _Owner->BaseData.Mode.Searchenemy != false )
	{
		//if( _Owner->TempData.AroundPlayer != 0 ||	rand() % 5 == 0 )
		{
			RoleDataEx* Target = Global::SearchEnemy( _OwnerObj );
			if( Target != NULL )
			{
				_Owner->SetAttack( Target );
				SetState( EM_AIState_Attack );
				return 1;
			}
		}
	}

	if( GetAwayfromBattle() != false )
	{
		SetState( EM_AIState_Escape );
		return 1;
	}

	if( _Owner->IsFollow() )
	{
		RoleDataEx* Target = Global::GetRoleDataByGUID( _Owner->TargetID() );
		if( Target == NULL )
			Target = Global::GetRoleDataByGUID( _Owner->OwnerGUID() );

		if( Target == NULL )
			_Owner->IsFollow( false );
		else if( Target->IsMove() || _Owner->Length( Target ) > 20 )
		{
			SetState( EM_AIState_Follow );
			return 1;
		}
	}    
	else if(     _Owner->TempData.RandomMove.Enable != false 	)
	{  
		if( RoleDataEx::CheckTime( _Owner->TempData.RandomMove.CountDown ) )
		{
			SetState( EM_AIState_RandMoving );
			return 10;
		}
	}
	else if(		RoleDataEx::CheckTime( _NPCMoveFlag.CountDown - 1000 ) 
		&&	_Owner->OwnerGUID() == -1 )
	{	
		int DTime = int( _NPCMoveFlag.CountDown - RoleDataEx::G_SysTime )/ 100;
		if( DTime < 1 )
			DTime = 1;

		//�w�I����
		SetState( EM_AIState_FlagMoving );
		return DTime;
	}

	if( _Owner->IsMove() )
	{
		return 3;
	}
	//--------------------------------------------------------------------------------

	// 1/60 ���| �|�I�k
	if( rand() % 60 == 0 )
	{
		SpellMagicProc( EM_NPCSpellRightTime_Normal );
		/*
		GameObjDbStructEx* NPCObjDB = Global::GetObjDB( _Owner->BaseData.ItemInfo.OrgObjID );
		RoleDataEx* Target = NULL;
		if( NPCObjDB->IsNPC() )
		{
			for( int i = 0 ; i < _MAX_NPC_SPELL_MAGIC_COUNT_ ; i++ )
			{
				NPCSpellBaseStruct& SpellInfo = NPCObjDB->NPC.Spell[i];

				if( EM_NPCSpellRightTime_Normal != SpellInfo.RigthTime )
					continue; 

				//�I�k
				if( rand() % 100 > SpellInfo.Rate )
					continue;

				Target = SearchNPCMagicTarget( _Owner , SpellInfo.TargetType, SpellInfo.MagicID );
				if( Target  != NULL )
				{
					if( MagicProcessClass::SpellMagic( _Owner->GUID() , Target->GUID() ,  Target->Pos()->X , Target->Pos()->Y , Target->Pos()->Z  , SpellInfo.MagicID , SpellInfo.MagicLv ) )
					{
						if( strlen( NPCObjDB->NPC.Spell[i].SpellString ) != 0 )
							_Owner->Say( NPCObjDB->NPC.Spell[i].SpellString );
					}
					break;
				}
			}
		}
		*/
	}

	//�p�G�P��S���a
	return 10;
}
//----------------------------------------------------------------------------------------------------------
//�üƲ��ʳB�z
int NpcAIBaseClass::RandMovingProc()
{
	if( _Owner->BaseData.Mode.Move == false )
		return 10;

	if( rand() % 5 == 0  )
	{
		if( rand() %10 < 3 )
			_Owner->IsWalk( false );
		else
			_Owner->IsWalk( true );
	}

	if( _OwnerObj->Path()->StackCount() != 0 )
		return 10;

	NPC_RandomMoveStruct& RDMove = _Owner->TempData.RandomMove;
	float X,Y,Z;

	for( int i = 0 ; i < 8 ; i++ )
	{
		X = _Owner->BaseData.Pos.X + RDMove.Dist - float( rand()%( int( RDMove.Dist * 2 +1 ) ) );
		Y = _Owner->BaseData.Pos.Y ;
		Z = _Owner->BaseData.Pos.Z + RDMove.Dist - float( rand()%( int( RDMove.Dist * 2 +1 ) ) );
		if(     X > RDMove.CenterX + RDMove.MaxRange || X < RDMove.CenterX - RDMove.MaxRange 
			||  Z > RDMove.CenterZ + RDMove.MaxRange || Z < RDMove.CenterZ - RDMove.MaxRange  )
			continue;

		float Dx = _Owner->BaseData.Pos.X - X;
		float Dz = _Owner->BaseData.Pos.Z - Z;
		float Dist = __min( RDMove.Dist/2 , 40 );

		if( Dx * Dx + Dz * Dz < Dist* Dist )
			continue;

		break;
	}

	if(     X > RDMove.CenterX + RDMove.MaxRange || X < RDMove.CenterX - RDMove.MaxRange 
		||  Z > RDMove.CenterZ + RDMove.MaxRange || Z < RDMove.CenterZ - RDMove.MaxRange  )
	{
		X = RDMove.CenterX;
		Z = RDMove.CenterZ;
	}

	if( _OwnerObj->Path()->Height( X , Y , Z , Y , 40.0f ) == false )
		return 10;

	if( _OwnerObj->Path()->CountNode( X , Y , Z ) >= 15 )
		return 10;

	//_OwnerObj->Path()->Target( X , Y , Z );
	_OwnerObj->Path()->Target( X , Y , Z , _Owner->X() , _Owner->Y() , _Owner->Z() );

	int MoveTime = int( ( _OwnerObj->Path()->TotalLength() * 10 )   /  _Owner->MoveSpeed() + 0.5 );


	RDMove.CountDown = MoveTime *100 + RoleDataEx::G_SysTime;
	RDMove.CountDown += ( RDMove.Active  + rand() % RDMove.Active )*100;

	SetState( EM_AIState_Idle );
	return 5;
}
//----------------------------------------------------------------------------------------------------------
//�]�w�ثe�Ҧb���X�l
bool 	NpcAIBaseClass::SetNpcFlagPos( int FlagID ) 
{
	vector< NPC_MoveBaseStruct >& MoveListInfo = *(_OwnerObj->NPCMoveInfo()->MoveInfo());

	if( FlagID == -1 )
		_NPCMoveFlag.Index = int( MoveListInfo.size() - 1 );

	if( (unsigned)FlagID >= MoveListInfo.size() )
		return false;

	_NPCMoveFlag.Index = FlagID;

	NPC_MoveBaseStruct& MoveInfo = MoveListInfo[ FlagID ];
	_NPCMoveFlag.Type = EM_NPCMoveFlagAIType_MoveToNext;

	_NPCMoveFlag.TargetX	= MoveInfo.X;
	_NPCMoveFlag.TargetY	= MoveInfo.Y;
	_NPCMoveFlag.TargetZ	= MoveInfo.Z;
	_NPCMoveFlag.TargetDir	= MoveInfo.Dir;

	return true;
}
//----------------------------------------------------------------------------------------------------------
int NpcAIBaseClass::FlagMovingProc( )
{
	if( Global::St()->IsNPCMove == false )
	{
		SetState( EM_AIState_Idle );
		return 10;
	}

	if( _Owner->BaseData.Mode.Move == false )
	{
		SetState( EM_AIState_Idle );
		return 10;
	}

	if( _MoveToFlagEnabled == false )
	{
		SetState( EM_AIState_Idle );
		return 10;
	}

	if( _Owner->IsAttack() )
	{
		SetState( EM_AIState_Attack );
		return 1;
	}

	if( RoleDataEx::CheckTime( _Owner->TempData.iDelayPatrol ) == false )
	{
		SetState( EM_AIState_Idle );
		return 10;
	}

	if( RoleDataEx::CheckTime( _NPCMoveFlag.CountDown ) == false )
	{
		int DTime = int( _NPCMoveFlag.CountDown - RoleDataEx::G_SysTime )/ 100;
		if( DTime > 1 )
		{
			if( DTime > 10 )
				DTime = 10;
			SetState( EM_AIState_Idle );
			return DTime;
		}
	}

	//-----------------------------------------------------------------------------------------
	vector< NPC_MoveBaseStruct >& MoveListInfo = *(_OwnerObj->NPCMoveInfo()->MoveInfo());
	//�p�G�S��ƪ�����A�]�w�����I���_�l�I
	if( MoveListInfo.size() == 0 )
	{
		//�]�w�������I
		NPC_MoveBaseStruct Data;
		Data.X = _Owner->SelfData.RevPos.X;
		Data.Y = _Owner->SelfData.RevPos.Y;
		Data.Z = _Owner->SelfData.RevPos.Z;
		Data.Dir = _Owner->SelfData.RevPos.Dir;
		Data.Range = 0.0f;			//���ʰ��t�Z��
		Data.MoveType = EM_NPC_MoveType_Run;		//���ʤ覡
		Data.DirMode = EM_NPC_DirMode_Change;
		Data.WaitTime_Base = 60;	//�򥻵��ݮɶ�(��)
		Data.WaitTime_Rand = 30;	//�üƵ��ݮɶ�(��)
		Data.ActionType = -1;		//�ʧ@ID ( -1 ����)
		MoveListInfo.push_back( Data );
	}
	//-----------------------------------------------------------------------------------------
	//�p�G������h���B�z����
	GameObjDbStructEx* NPCObjDB = Global::GetObjDB( _Owner->BaseData.ItemInfo.OrgObjID );
	if( NPCObjDB != NULL )
	{
		if( NPCObjDB->NPC.CarryType == EM_NPCCarryType_Control )
		{
			SetState( EM_AIState_Idle );
			return 3;
		}
	}



	//-----------------------------------------------------------------------------------------

	for( int x=0 ; x < 3; x++ )
	{
		NPC_MoveBaseStruct MoveInfo;
		switch( _NPCMoveFlag.Type )
		{
		case EM_NPCMoveFlagAIType_MoveToNext:		//�w��F�ت��I
			{
				// ��F�w�I, �B�z��F���I���ƶ�

				//���U�@�B		
				_NPCMoveFlag.Index++;

				if( (unsigned)_NPCMoveFlag.Index >= MoveListInfo.size() )
					_NPCMoveFlag.Index = 0;


				MoveInfo = MoveListInfo[ _NPCMoveFlag.Index ];
				_NPCMoveFlag.IsUsePathFind = MoveInfo.IsUsePathFind;

				if( MoveInfo.Range < 1 || _Owner->BaseData.Mode.Gravity == false )
				{
					_NPCMoveFlag.TargetX	= MoveInfo.X;
					_NPCMoveFlag.TargetY	= MoveInfo.Y;
					_NPCMoveFlag.TargetZ	= MoveInfo.Z;
					_NPCMoveFlag.TargetDir	= MoveInfo.Dir;

				}
				else
				{
					float X , Y , Z;
					for( int i = 0 ; i < 8 ; i++ )
					{
						X = MoveInfo.X + rand() % int( MoveInfo.Range *2 ) - MoveInfo.Range;
						Y = MoveInfo.Y;
						Z = MoveInfo.Z + rand() % int( MoveInfo.Range *2 ) - MoveInfo.Range;

						float Dx = _Owner->BaseData.Pos.X - X;
						float Dz = _Owner->BaseData.Pos.Z - Z;

						if( _OwnerObj->Path()->Height( X , Y , Z , Y , 50.0f ) == false )
						{
							X = MoveInfo.X; 
							Y = MoveInfo.Y;
							Z = MoveInfo.Z;
							continue;
						};

						if( Dx * Dx + Dz * Dz < 40*40 )
							continue;

						if( _OwnerObj->Path()->CheckLine(	X , Y , Z , MoveInfo.X , MoveInfo.Y , MoveInfo.Z ) == false )
							continue;


						break;
					}
					_NPCMoveFlag.TargetX	= X;
					_NPCMoveFlag.TargetY	= Y;
					_NPCMoveFlag.TargetZ	= Z;
					_NPCMoveFlag.TargetDir	= MoveInfo.Dir;
				}
			}
			x = 1000;
			break;

		case EM_NPCMoveFlagAIType_Waitting:
			{
				if( _NPCMoveFlag.Index >= (int)MoveListInfo.size() )
				{
					_NPCMoveFlag.Index = 0;
					continue;
				}

				MoveInfo = MoveListInfo[ _NPCMoveFlag.Index ];

				// �ˬd���L�@��Ĳ�o
				if( MoveInfo.sLuaFunc.Size() != 0 && _IsStopLuaOnce == false )
				{						
					_OwnerObj->PlotVM()->CallLuaFunc( MoveInfo.sLuaFunc.Begin(), _Owner->GUID() , 10, NULL ); // ���J�@��
				}
				_IsStopLuaOnce = false;

				// �]�w���ʤ覡					

				switch( MoveInfo.MoveType )
				{
				case EM_NPC_MoveType_None:	_NPCMoveFlag.IsWalk = _Owner->IsWalk();	break; //���]�w�Τ��e��			
				case EM_NPC_MoveType_Walk:	_NPCMoveFlag.IsWalk = true;	break;	//���� 
				case EM_NPC_MoveType_Run:	_NPCMoveFlag.IsWalk = false; break; //�]��
				case EM_NPC_MoveType_Rand:	
					if( rand() % 10  >= 3 )	
						_NPCMoveFlag.IsWalk = true;
					else
						_NPCMoveFlag.IsWalk = false;
					break; //�����ζ]�B
				}
				_Owner->IsWalk( _NPCMoveFlag.IsWalk );

				// �]�w���I�����ݮɶ�

				int	WaitTime;
				if( MoveInfo.WaitTime_Rand > 0 )
					WaitTime = ( _NPCMoveFlag.WaitTime + MoveInfo.WaitTime_Base + rand() % ( MoveInfo.WaitTime_Rand ) );
				else
					WaitTime = ( _NPCMoveFlag.WaitTime + MoveInfo.WaitTime_Base );

				_NPCMoveFlag.CountDown = WaitTime * 1000 + RoleDataEx::G_SysTime;					
				_NPCMoveFlag.WaitTime = 0;

				// �Y�O NPC �����ݮɶ��μ@��, �h���� NPC �P���|�I���V�ۦP
				if( MoveInfo.DirMode != EM_NPC_DirMode_None && ( MoveInfo.WaitTime_Base != 0 || MoveInfo.sLuaFunc.Size() != 0 ) )
				{
					//_OwnerObj->Role()->SetValue( EM_RoleValue_Dir, MoveInfo.Dir );
					//					_OwnerObj->Role()->TempData.Move.Tx = MoveInfo.X;
					//					_OwnerObj->Role()->TempData.Move.Ty = MoveInfo.Y;
					//					_OwnerObj->Role()->TempData.Move.Tz = MoveInfo.Z;

					
					_OwnerObj->Role()->TempData.Move.ProcDelay = 0;
					_OwnerObj->Role()->TempData.Move.AddeDelay = 0;
					_OwnerObj->Role()->TempData.Move.Dx = 0;
					_OwnerObj->Role()->TempData.Move.Dy = 0;
					_OwnerObj->Role()->TempData.Move.Dz = 0;
					
					//NetSrv_MoveChild::Send
					if( _OwnerObj->Role()->Pos()->Dir	!= MoveInfo.Dir )
					{
						_OwnerObj->Role()->Pos()->Dir = MoveInfo.Dir;
 						NetSrv_MoveChild::MoveObj( _OwnerObj );
					}
					
				}

				_NPCMoveFlag.Type = EM_NPCMoveFlagAIType_MoveToNext;

				return 3;

			} break;
		case EM_NPCMoveFlagAIType_WaitMoveEx:
			{
				if(		_OwnerObj->Path()->StackCount() == 0 && 
					(	_Owner->TempData.Move.Dx == 0 || _Owner->TempData.Move.Dz == 0 )	)
				{
					_IsStopLuaOnce = true;
					_NPCMoveFlag.CountDown = RoleDataEx::G_SysTime + 1000;
					_NPCMoveFlag.WaitTime = 0;
					_NPCMoveFlag.Index--;
					_NPCMoveFlag.Type = EM_NPCMoveFlagAIType_MoveToNext;
					continue;
				}

				return 1;
			}
			break;
		case EM_NPCMoveFlagAIType_WaitMoveEx1:
			{
				if(		_OwnerObj->Path()->StackCount() == 0 && 
					(	_Owner->TempData.Move.Dx == 0 || _Owner->TempData.Move.Dz == 0 )	)
				{
					_NPCMoveFlag.CountDown = RoleDataEx::G_SysTime + 1000;
					_NPCMoveFlag.WaitTime = 0;
					_NPCMoveFlag.Index--;
					_NPCMoveFlag.Type = EM_NPCMoveFlagAIType_MoveToNext;
					continue;
				}

				return 1;
			}
			break;
		case EM_NPCMoveFlagAIType_Moving:		//�٦b���ʤ�
			{
				//�ˬd�ؼЬO�_��F
				float Dist = _Owner->Length3D( _NPCMoveFlag.TargetX , _NPCMoveFlag.TargetY , _NPCMoveFlag.TargetZ );
				if( Dist <= 20 )
				{
					// �]�w��F�w�I
					_NPCMoveFlag.Type = EM_NPCMoveFlagAIType_Waitting;

					continue;
				}

				//�߬d�O�_���Q���ʹL��m �ä��_����

				if(		_OwnerObj->Path()->StackCount() == 0 && 
					(	_Owner->TempData.Move.Dx == 0 || _Owner->TempData.Move.Dz == 0 )	)
				{
					//_NPCMoveFlag.Type = EM_NPCMoveFlagAIType_Waitting;
					_NPCMoveFlag.Type = EM_NPCMoveFlagAIType_WaitMoveEx1;
					continue;
				}

				SetState( EM_AIState_Idle );
				return 3;
			}
			break;
		}
	}

	//�p�G�Ӫ񤣲���
	if( _Owner->Length3D( _NPCMoveFlag.TargetX , _NPCMoveFlag.TargetY , _NPCMoveFlag.TargetZ) <= 5)
	{
		_NPCMoveFlag.Type = EM_NPCMoveFlagAIType_Waitting;
		return 1;
	}


	_NPCMoveFlag.Type = EM_NPCMoveFlagAIType_Moving;

	
	if( _Owner->BaseData.Mode.Gravity == false || _NPCMoveFlag.IsUsePathFind == false )
		_OwnerObj->Path()->Target( _NPCMoveFlag.TargetX , _NPCMoveFlag.TargetY , _NPCMoveFlag.TargetZ, false );
	else
		_OwnerObj->Path()->Target( _NPCMoveFlag.TargetX , _NPCMoveFlag.TargetY , _NPCMoveFlag.TargetZ , _Owner->X() , _Owner->Y() , _Owner->Z() );


	int MoveTime = int( ( _OwnerObj->Path()->TotalLength() * 10 )   /  _Owner->MoveSpeed() + 0.5 );

	if( MoveTime > 100 )
		MoveTime = 100;

	_NPCMoveFlag.CountDown = MoveTime *100 + RoleDataEx::G_SysTime;
	//_NPCMoveFlag.CountDown = RoleDataEx::G_SysTime + 100;

	SetState( EM_AIState_Idle );
	return 3;
}
//----------------------------------------------------------------------------------------------------------
//���H�B�z
int NpcAIBaseClass::FollowProc()
{
	RoleDataEx* Target = (RoleDataEx*)Global::GetRoleDataByGUID( _Owner->TargetID() );

	if( _Owner->IsAttack() )
	{
		SetState( EM_AIState_Attack ); 
		return 1;
	}

	if( Target == NULL )
	{		 		
		Target = Global::GetRoleDataByGUID( _Owner->OwnerGUID() );
		if( Target == NULL )
			return 5;
	}

	//�p�G�ؼв��ʤ覡���Onormal�h�����H
	if(		Target->TempData.Move.MoveType != EM_ClientMoveType_Normal
		&&	Target->TempData.Move.MoveType != EM_ClientMoveType_Jump	
		&&  Target->TempData.Move.MoveType != EM_ClientMoveType_Mount	)
		return 10;

	//�P�ؼжZ��
	float Length = _Owner->Length( Target );

	if( Length > 50 )
		_Owner->IsWalk( false );
	else
		_Owner->IsWalk( true );

	if( _OwnerObj->Path()->StackCount() > 1 )
	{
		return 5;
	}

	if( _Owner->BaseData.Mode.Searchenemy != false )
	{
		if( _Owner->TempData.AroundPlayer != 0 ||	rand() % 5 == 0 )
		{
			RoleDataEx* Target = Global::SearchEnemy( _OwnerObj );
			if( Target != NULL )
			{
				_Owner->SetAttack( Target );
				SetState( EM_AIState_Attack );
				return 1;
			}
		}
	}

	float Len = _Owner->Length( Target->X() , Target->Y() , Target->Z() ); 


	if( Target->IsMove() == false )
	{
		if( 30  > Len )
		{
			if( rand() % 40 || _Owner->IsAttackMode() )
				return 5;
		}
	}

	float X,Y,Z;
	if( Target->IsPlayer() )
	{
		if( Target->TempData.Move.MoveType != EM_ClientMoveType_Mount )
		{
			X = Target->BaseData.Pos.X + rand() % 60 - 30;
			Y = Target->BaseData.Pos.Y;
			Z = Target->BaseData.Pos.Z + rand() % 60 - 30 ;        
		}
		else
		{
			float Dx;
			float Dz;
			Target->CalDir( Target->Pos()->Dir + 120 + rand()%120  , Dx , Dz );
			X = Target->BaseData.Pos.X + Dx * 10;
			Y = Target->BaseData.Pos.Y;
			Z = Target->BaseData.Pos.Z + Dz * 10;        
		}
	}
	else
	{
		float Dx;
		float Dz;
		Target->CalDir( Target->Pos()->Dir + 30 , Dx , Dz );
		X = Target->BaseData.Pos.X - Dx * 30;
		Y = Target->BaseData.Pos.Y;
		Z = Target->BaseData.Pos.Z - Dz * 30;        
	}

	//_OwnerObj->Path()->Target( X , Y , Z , _Owner->X() , _Owner->Y() , _Owner->Z() );
	_OwnerObj->Path()->Target( X , Y , Z );

	//(�Ȯ�)
	//�U���B�z���ɶ�
	int MoveTime = int( ( _OwnerObj->Path()->TotalLength() * 10 )   /  _Owner->MoveSpeed() - 5 );
	int NextProcTime = __max( MoveTime , 3 );
	if( NextProcTime > 20 )
		NextProcTime = 20;

	return NextProcTime;
}
//----------------------------------------------------------------------------------------------------------
/*
//�D�԰��I�k�B�z
int NpcAIBaseClass::SpellMagicProc()
{
return 10;
}
*/
//----------------------------------------------------------------------------------------------------------
void NpcAIBaseClass::SpellMagicProc( NPCSpellRightTimeENUM ProcTime)
{
	if( _Owner->IsSpell() )
		return;

	vector<BaseSortStruct>*	SearchList;	
	RoleDataEx* SpellTarget = NULL;
	RoleDataEx* Target = (RoleDataEx*)Global::GetRoleDataByGUID( _Owner->AttackTargetID() );

	GameObjDbStructEx* NPCObjDB = Global::GetObjDB( _Owner->BaseData.ItemInfo.OrgObjID );

	//if( NPCObjDB == NULL )
	//	return;

	if( NPCObjDB->IsNPC() == false )
		return;

	if( NPCObjDB->IsQuestNPC() && NPCObjDB->NPC.iQuestNPCRefID <= 0 )
		return;

	_Owner->TempData.AI.KeepDistance = NPCObjDB->NPC.KeepDistance;

	for( int i = 0 ; i < _MAX_NPC_SPELL_MAGIC_COUNT_ ; i++ )
	{
		NPCSpellBaseStruct& SpellInfo = NPCObjDB->NPC.Spell[i];

		if(  ProcTime != SpellInfo.RigthTime )
		{
			if( ProcTime == EM_NPCSpellRightTime_None  )
			{
				if(		SpellInfo.RigthTime == EM_NPCSpellRightTime_OnAttack 
					||	SpellInfo.RigthTime == EM_NPCSpellRightTime_Normal )
					continue;
			}
			else 
				continue;
		
		}

		switch( SpellInfo.RigthTime )
		{
		case EM_NPCSpellRightTime_OnAttack:
		case EM_NPCSpellRightTime_Attack:			//�԰����A   (5��)
		case EM_NPCSpellRightTime_KeepDistance:
		case EM_NPCSpellRightTime_Normal:
			break;
		case EM_NPCSpellRightTime_AttackHPHalf:		//�԰�HP1/2  (5��)
			if( _Owner->MaxHP() < _Owner->HP()*2 )
				continue;
			break;

		case EM_NPCSpellRightTime_AttackHPQuarter:	//�԰�HP1/4  (5��)
			if( _Owner->MaxHP() < _Owner->HP()*4 )
				continue;
			break;
		case EM_NPCSpellRightTime_FriendDead:
			{
				bool IsFind = false;
				SearchList = Global::SearchRangeObject( _Owner , _Owner->X() , _Owner->Y() , _Owner->Z() , EM_SearchRange_Friend , 50	);
				for( int i = 0 ; i <  (int)SearchList->size() ; i++ )
				{
					RoleDataEx* Friend = (RoleDataEx* )(*SearchList)[i].Data;
					if( Friend->IsDead() )
					{
						IsFind = true;
						break;
					}
				}
				if( IsFind == false )
					continue;
			}
			break;
		default:
			continue;
		}

		//�I�k
		if( rand() % 100 > SpellInfo.Rate )
			continue;


		SpellTarget = SearchNPCMagicTarget( Target , SpellInfo.TargetType, SpellInfo.MagicID );
		if( SpellTarget != NULL )
		{
			GameObjDbStructEx* MagicCol = Global::GetObjDB( SpellInfo.MagicID );
			if( MagicCol == NULL )
				continue;

			if(	/*	MagicCol->MagicCol.IsMagicAttackDelay_TargetDist == false
				||	*/MagicCol->MagicCol.Flag.Ignore_Obstruct
				||	_OwnerObj->Path()->CheckLine_Magic(	_Owner->X() , _Owner->Y() , _Owner->Z() , SpellTarget->X() , SpellTarget->Y() , SpellTarget->Z() ) != false )
			{
				if( MagicProcessClass::SpellMagic( _Owner->GUID() , SpellTarget->GUID() ,  SpellTarget->Pos()->X , SpellTarget->Pos()->Y , SpellTarget->Pos()->Z  , SpellInfo.MagicID , SpellInfo.MagicLv ) )
				{
					if( strlen( NPCObjDB->NPC.Spell[i].SpellString ) != 0 )
					{
						char Buf[512];
						sprintf_s( Buf , sizeof(Buf) , "[Sys%d_spellstring%d]" , NPCObjDB->GUID , i + 1 );
						_Owner->Say( Buf );
					}
					break;
				}
			}
		}
	}
}

//�������m�B�z
int NpcAIBaseClass::AttackProc()
{
	bool IsBeginAttack = _IsBeginAttack;
	_IsBeginAttack = false;	

	//Follow :
	//    ��P�ؼжZ���L��
	//KeepDistance:            
	//    �p�G�����Z�����k�N�Χ����覡
	//Escape:
	//    �p�GHP�p�� �i���� * ( MaxHP / 10 ) �h���@�w���|�k��
	//SpellMagic:
	//    ( �u���v���� ���H)
	//    �p�G�����Z�����k�N�Χ����覡 �åB�ؼйL��

	RoleDataEx* Target = (RoleDataEx*)Global::GetRoleDataByGUID( _Owner->AttackTargetID() );
	if(		_Owner->IsAttack() == false
		||	Target == NULL )
	{
		//�p�G���d����
		if( _Owner->TempData.Sys.OwnerGUID != -1 )
		{
			SetState( EM_AIState_Idle );
			return 1;
		}
		else if( _Owner->IsDead() != true || _Owner->SecRoomID() != -1 ) 
		{
			Global::FindNewHateTarget( _Owner );
		}
		Target = (RoleDataEx*)Global::GetRoleDataByGUID( _Owner->AttackTargetID() );

		if( _Owner->IsAttack() == false || Target == NULL )
		{
			SetState( EM_AIState_Idle );
			return 50;
		}
		

	}

	int PTime = RoleDataEx::G_SysTime - _ProcTime;
	float Len = _Owner->Length( Target );

	if( rand() % 50 == 0 )
	{
		/*
		RoleDataEx* HateTarget = _Owner->SearchHate();
		if( HateTarget != NULL )
		{
			_Owner->SetAttack( HateTarget );
		}
		return 1;
		*/
		/* && _Owner->IsPet( ) != false*/
		RoleDataEx* PetOwner = Global::GetRoleDataByGUID( _Owner->OwnerGUID() );
		if( PetOwner != NULL && _Owner->IsPet( PetOwner ) != false )
		{
		}
		else if( _Owner->TempData.Attr.Effect.LockTarget == false )
		{
			RoleDataEx* HateTarget = _Owner->SearchHate();
			if( HateTarget != NULL )
			{
				_Owner->SetAttack( HateTarget );
			}
			return 1;
		}
	}


		//�p�G�������
	if( (  _Owner->TempData.Attr.Effect.Fear != false  )
		&& _Owner->TempData.Attr.Effect.Stop == false )
	{
		_IsMagicEscape = true;
		SetState( EM_AIState_Atk_Escape );
		return 1;
	}

	//�ˬd�O�_�n�k�]
	if( PTime  > 15000 &&  _Owner->CheckEscape( Target ) != false  )
	//	||	_Owner->TempData.Attr.Effect.Fear != false )
	{
		SetState( EM_AIState_Atk_Escape );
		return 1;
	}

	if( IsBeginAttack != false )
	{
		SpellMagicProc( EM_NPCSpellRightTime_OnAttack );
		/*
		//�O�_���Ұʭn�I�i���k�N
		GameObjDbStructEx* NPCObjDB = Global::GetObjDB( _Owner->BaseData.ItemInfo.OrgObjID );
		for( int i = 0 ; i < _MAX_NPC_SPELL_MAGIC_COUNT_ ; i++ )
		{
			NPCSpellBaseStruct& SpellInfo = NPCObjDB->NPC.Spell[i];

			if( SpellInfo.RigthTime != EM_NPCSpellRightTime_OnAttack )
				continue;
			
			//�I�k
			if( rand() % 100 > SpellInfo.Rate )
				continue;

			SpellTarget = SearchNPCMagicTarget( Target , SpellInfo.TargetType, SpellInfo.MagicID );
			if( SpellTarget != NULL )
			{
				if( MagicProcessClass::SpellMagic( _Owner->GUID() , SpellTarget->GUID() ,  SpellTarget->Pos()->X , SpellTarget->Pos()->Y , SpellTarget->Pos()->Z  , SpellInfo.MagicID , SpellInfo.MagicLv ) )
				{
					if( strlen( NPCObjDB->NPC.Spell[i].SpellString ) != 0 )
						_Owner->Say( NPCObjDB->NPC.Spell[i].SpellString );
				}
				break;
			}
		}
		*/
	}

	//----------------------------------------------------------------------------------------------------
	if(	_Owner->IsMove() == false
		&&	_Owner->CheckMainAttackRange( Target) == false
		&&	_OwnerObj->Path()->CheckLine( Target->X() , Target->Y()  , Target->Z() , _Owner->X() , _Owner->Y() , _Owner->Z() ) == false 
		&& _Owner->BaseData.Mode.Move != false
		&& _Owner->TempData.Attr.Effect.Stop == false )
	{
//		int KeepDistanceBK = _Owner->TempData.AI.KeepDistance;
		_Owner->TempData.AI.KeepDistance = 0;
		float NX , NZ , NY;
		Global::SearchAttackNode( _Owner , Target , NX , NY , NZ );
		_OwnerObj->Path()->Target( NX , NY , NZ , _Owner->X() , _Owner->Y() , _Owner->Z() );
//		_Owner->TempData.AI.KeepDistance = KeepDistanceBK;
		return 3;
	}

	int BoxY = int( _Owner->TempData.BoundRadiusY + Target->TempData.BoundRadiusY );

	if(		Target->IsMove() == false 
		&& BoxY - Len > 3
		&&	_OwnerObj->Path()->StackCount() == 0 )
	{
		SetState( EM_AIState_Atk_Follow );
		return 1;
	}


	if(		Len >= _Owner->TempData.AI.KeepDistance
		&& ( _OwnerObj->Path()->StackCount() < 2 || rand() % 5 == 0 ) )
	{
		if(		rand() %100  == 0
			&&	( Len <= (_Owner->TempData.BoundRadiusY + Target->TempData.BoundRadiusY - 3)
			||  Len > _Owner->TempData.BoundRadiusY + Target->TempData.BoundRadiusY + 5 ) )
		{
			_Owner->StopMove();
			SetState( EM_AIState_Atk_Follow );
			return 1;
		}
		else if( ( Len <= (_Owner->TempData.BoundRadiusY + Target->TempData.BoundRadiusY) / 2 && rand() % 10 == 0 )
			||  Len > _Owner->TempData.BoundRadiusY + Target->TempData.BoundRadiusY + 20 )
		{
			_Owner->StopMove();
			SetState( EM_AIState_Atk_Follow );
			//�p�G�����Z�����k�N�Χ����覡�i����
			return 1;
		} 

	}
	else
	{
		if(	Len <= 60 && rand() % 50 == 0  )
		{
			//�p�G���Ĥ@���԰�
//			_ProcTime -= 10000;
			if( _CheckNPCAtkMagicDistance( Target , 80 )  )
			{
				SetState( EM_AIState_Atk_KeepDistance );
				return 1;
			}
		}

	}
	//----------------------------------------------------------------------------------------------------

	if( !_Owner->ClockMod( 10 ) )
		return 1;

	SpellMagicProc( EM_NPCSpellRightTime_None );
	return 1;
}
//----------------------------------------------------------------------------------------------------------
//�Q�����B�z(�L�k��������)
int NpcAIBaseClass::UnderAttackProc()
{
	if( _Owner->OwnerGUID() != -1 )
	{
		SetState( EM_AIState_Idle );
		return 1;
	}

	if(		_Owner->BaseData.Mode.Strikback == false )
	{
		if( _Owner->BaseData.Mode.Escape != false )
			SetState( EM_AIState_Escape );
		_Owner->TempData.NPCHate.Init();
		return 1;
	}

	RoleDataEx* Target = _Owner->SearchHate();
	if( Target == NULL )
	{
		_Owner->TempData.NPCHate.Init();
		SetState( EM_AIState_Idle );
		return 1;
	}

	_Owner->SetAttack( Target );
	SetState( EM_AIState_Attack );
	return 1;
}
//----------------------------------------------------------------------------------------------------------
//�@��k�]
int NpcAIBaseClass::EscapeProc()
{
	/*
	int PTime = RoleDataEx::G_SysTime - _ProcTime;
	if( PTime  > 4000 )
	{
	if( _Owner->TempData.AI.ActionType == EM_ActionType_ESCAPE )
	{
	_Owner->TempData.AI.ActionType = EM_ActionType_NORMAL;
	_Owner->TargetID( -1 );
	}

	SetState( EM_AIState_Idle );
	return 1;
	}
	*/
	SetState( EM_AIState_Idle );

	RoleDataEx* Target = (RoleDataEx*)Global::GetRoleDataByGUID( _Owner->TargetID() );
	if( Target == NULL )
	{
		return 10;
	}

	if( _OwnerObj->Path()->StackCount() > 1 )
		return 5;


	float Dx = ( _Owner->X() - Target->X() );
	float Dz = ( _Owner->Z() - Target->Z() );

	float Len = sqrt( Dx*Dx + Dz*Dz );

	if( Len == 0 )
		Len = 0.001f;

	if( Len >= 100 )
	{
		return 10;
	}
	else
	{
		Dx /= Len;
		Dz /= Len;

		float T = 180 * float( 3.14 ); //180 / 57.2958f
		Dz = Dx * sin( T ) + Dz*cos( T );
		Dx = Dx * cos( T ) - Dz*sin( T );

	}
	
	float X, Y ,Z;

	for( int i = 0 ; i < 10 ; i++ )
	{
		float Destance = float( rand() % 40 + 40 );
		Dx = ( Dx )* Destance;
		Dz = ( Dz )* Destance;

		X = _Owner->BaseData.Pos.X  + Dx;
		Y = _Owner->BaseData.Pos.Y ;
		Z = _Owner->BaseData.Pos.Z  + Dz;
		
		//�ˬd�O�_���I�i�H��
		if( _OwnerObj->Path()->Height( X , Y , Z , Y , 30 ) == false )
			continue;

		if( _OwnerObj->Path()->CheckLine( X , Y  , Z , _Owner->X() , _Owner->Y() , _Owner->Z() ) == false )
			continue;

		_Owner->IsWalk( true );
		_OwnerObj->Path()->Target( X , Y , Z , _Owner->X() , _Owner->Y() , _Owner->Z() );
		//*�Ȯ�*
		int MoveTime = int( ( _OwnerObj->Path()->TotalLength() * 10 )   /  _Owner->MoveSpeed() + 0.5 );
		int Ret = __max( MoveTime , 10 );	

		break;
	}



	return 10;
}
//----------------------------------------------------------------------------------------------------------
//�������H�B�z
int NpcAIBaseClass::Atk_FollowProc()
{
/*	if(		EM_AIState_Atk_Follow == _LState 
		&&	)	
	{
		SetState( EM_AIState_Attack );
		return 1;
	}
*/
	//_Owner->StopMove();
	RoleDataEx* Target = (RoleDataEx*)Global::GetRoleDataByGUID( _Owner->AttackTargetID() );
	if(		Target == NULL 
		||	_Owner->IsDead() )
	{
		SetState( EM_AIState_Idle );
		return 10;
	}

	if( rand() % 50 == 0  )
	{
		/* && _Owner->IsPet( ) != false*/
		RoleDataEx* PetOwner = Global::GetRoleDataByGUID( _Owner->OwnerGUID() );
		if( PetOwner != NULL && _Owner->IsPet( PetOwner ) != false )
		{
		}
		else
		{
			RoleDataEx* HateTarget = _Owner->SearchHate();
			if( HateTarget != NULL )
			{
				_Owner->SetAttack( HateTarget );
			}
			return 1;
		}
	}

	float Len = Target->Length( _Owner->TempData.Move.Tx , _Owner->TempData.Move.Ty , _Owner->TempData.Move.Tz );

	if(	(  Len <= _Owner->TempData.AI.KeepDistance 
		|| _Owner->CheckMainAttackRange( Target )  )
			&&	EM_AIState_Atk_Follow == _LState	)
	{
		SetState( EM_AIState_Attack );
		return 1;
	}
	


			//�p�G�������
	if( ( /*_Owner->CheckPhyAttack( Target ) == false ||*/	_Owner->TempData.Attr.Effect.Fear != false  )
		&& _Owner->TempData.Attr.Effect.Stop == false )
	{
		_IsMagicEscape = true;
		SetState( EM_AIState_Atk_Escape );
		return 1;
	}

	SpellMagicProc( EM_NPCSpellRightTime_None );

	if( _Owner->IsSpell() )
		return 2;

//	if( _Owner->BaseData.Mode.Move == false )
//		return 5;


	float X = Target->BaseData.Pos.X;
	float Y = Target->BaseData.Pos.Y;
	float Z = Target->BaseData.Pos.Z;

	GameObjDbStructEx* OrgDB = Global::GetObjDB( _Owner->BaseData.ItemInfo.OrgObjID );
	float		FightRange = 400.0f;
	if( OrgDB->IsNPC() )
	{
		FightRange = float( OrgDB->NPC.FollowRange );
	}

	PathProcStruct& Move = _Owner->TempData.Move;

	if(     ( X - Move.RetX  )*( X - Move.RetX  )
		+	( Z - Move.RetZ  )*( Z - Move.RetZ  )        > FightRange*FightRange )
	{
		Global::FindNewHateTarget( _Owner , FightRange );
	}
	else
	{
		float NX , NZ , NY;
		Global::SearchAttackNode( _Owner , Target , NX , NY , NZ );
		_OwnerObj->Path()->Target( NX , NY , NZ , _Owner->X() , _Owner->Y() , _Owner->Z() );

	}

	//*�Ȯ�	*
	int MoveTime = int( ( _OwnerObj->Path()->TotalLength() * 10 )   /  _Owner->MoveSpeed() + 0.5 ) ;
	if( MoveTime > 20 )
		return MoveTime/2;

	return 10;
}
//----------------------------------------------------------------------------------------------------------
//���� �O���@��
int NpcAIBaseClass::Atk_KeepDistanceProc()
{
	int PTime = RoleDataEx::G_SysTime - _ProcTime;
	/*
	if( PTime  > 14000 )
	{
	if( _LongDistanceMagic == false )
	{
	SetState( EM_AIState_Attack );
	return 1;
	}
	}
	*/
	bool Result = false;
	RoleDataEx* Target = (RoleDataEx*)Global::GetRoleDataByGUID( _Owner->AttackTargetID() );

	if( Target == NULL )
	{
		SetState( EM_AIState_Attack );
		return 10;
	}


	float Dx = ( _Owner->X() - Target->X() );
	float Dz = ( _Owner->Z() - Target->Z() );
	if( Dx == 0 && Dz == 0 )
	{
		Dx = 0.01f;
		Dz = 0.01f;
	}

	float OrgLen = sqrt( Dx*Dx + Dz*Dz );

	if( OrgLen > 60 )
	{
		SetState( EM_AIState_Attack );
		return 10;
	}

	GameObjDbStructEx* OrgDB = Global::GetObjDB( _Owner->BaseData.ItemInfo.OrgObjID );
	float		FightRange = 400.0f;
	if( OrgDB->IsNPC() )
	{
		FightRange = float( OrgDB->NPC.FollowRange );
	}

	PathProcStruct& Move = _Owner->TempData.Move;

	if(     ( _Owner->X() - Move.RetX  )*( _Owner->X() - Move.RetX  )
		+	( _Owner->Z() - Move.RetZ  )*( _Owner->Z() - Move.RetZ  )  > FightRange*FightRange )
	{
		SetState( EM_AIState_Attack );
		return 10;
	}



	if(		(OrgLen > 50 && OrgLen < 150 )
		||	(OrgLen < 50 && rand() % 150 == 0 ) )
	{
		SpellMagicProc( EM_NPCSpellRightTime_KeepDistance );
		/*
		GameObjDbStructEx* NPCObjDB = Global::GetObjDB( _Owner->BaseData.ItemInfo.OrgObjID );
		for( int i = 0 ; i < _MAX_NPC_SPELL_MAGIC_COUNT_ ; i++ )
		{
			NPCSpellBaseStruct& SpellInfo = NPCObjDB->NPC.Spell[i];

			switch( SpellInfo.RigthTime )
			{
			case EM_NPCSpellRightTime_KeepDistance:			//�԰����A   (5��)
				break;
			default:
				continue;
			}

			//�I�k
			if( rand() % 100 > SpellInfo.Rate )
				continue;

			SpellTarget = SearchNPCMagicTarget( Target , SpellInfo.TargetType, SpellInfo.MagicID );
			//Result = false;
			if( SpellTarget == NULL )
				continue;
			Result = MagicProcessClass::SpellMagic( _Owner->GUID() , SpellTarget->GUID() ,  SpellTarget->Pos()->X , SpellTarget->Pos()->Y , SpellTarget->Pos()->Z  , SpellInfo.MagicID , SpellInfo.MagicLv );

			if( Result != false )
			{
				if( strlen( NPCObjDB->NPC.Spell[i].SpellString ) != 0 )
					_Owner->Say( NPCObjDB->NPC.Spell[i].SpellString );
				break;
			}
			
		}
*/
		if( _Owner->IsSpell() == false )
		{
			if( PTime  > 14000 )
			{
				SetState( EM_AIState_Attack );
				return 1;
			}	
		}

		return 10;
	}


	float X ,Y , Z;

	for( int i = 0 ; i < 10 ; i++ )
	{
		float T = float(rand() % 120 -60 ) / 180 * float( 3.14 );
		Dx = Dx * cos( T ) - Dz*sin( T );
		Dz = Dx * sin( T ) + Dz*cos( T );

		float Len = sqrt( Dx*Dx + Dz*Dz );

		float Destance = float( rand() % 20 + 55 );
		Dx = ( Dx / Len )* Destance;
		Dz = ( Dz / Len )* Destance;

		//*�Ȯ�*
		//�p�G����d�򤺴N������
		X = _Owner->BaseData.Pos.X  + Dx;
		Y = _Owner->BaseData.Pos.Y ;
		Z = _Owner->BaseData.Pos.Z  + Dz;

		//�ˬd�O�_���I�i�H��
		if( _OwnerObj->Path()->Height( X , Y , Z , Y , 30 ) == false )
			continue;

//		if( PathManageClass::CheckLine( X , Y , Z , _Owner->BaseData.Pos.X , _Owner->BaseData.Pos.Y , _Owner->BaseData.Pos.Z ) != false )
//			continue;
		if( _OwnerObj->Path()->FindPathNode( X , Y , Z ) == 0 )
			continue;

		if( _OwnerObj->Path()->CountNode( X , Y , Z ) >= 15 )
			continue;

		break;
	}

/*
	float T = float(rand() % 120 -60 ) / 180 * float( 3.14 );
	Dx = Dx * cos( T ) - Dz*sin( T );
	Dz = Dx * sin( T ) + Dz*cos( T );

	float Len = sqrt( Dx*Dx + Dz*Dz );

	float Destance = float( rand() % 20 + 55 );
	Dx = ( Dx / Len )* Destance;
	Dz = ( Dz / Len )* Destance;

	//*�Ȯ�*
	//�p�G����d�򤺴N������
	float X = Target->BaseData.Pos.X  + Dx;
	float Y = Target->BaseData.Pos.Y ;
	float Z = Target->BaseData.Pos.Z  + Dz;
*/

	_OwnerObj->Path()->Target( X , Y , Z , _Owner->X() , _Owner->Y() , _Owner->Z() );
	//*�Ȯ�*
	int MoveTime = int( ( _OwnerObj->Path()->TotalLength() * 10 )   /  _Owner->MoveSpeed() + 0.5 );
	int Ret = __max( MoveTime , 20 );  
	SetState( EM_AIState_Attack );
	return Ret;
}
//----------------------------------------------------------------------------------------------------------
//���� �k�]�B�z
int NpcAIBaseClass::Atk_EscapeProc()
{
	int PTime = RoleDataEx::G_SysTime - _ProcTime;
	if( (	  PTime  > 5000 
		  || _IsMagicEscape != false )
		  && _Owner->TempData.Attr.Effect.Fear == false )
	{

		if( PTime > 15000 || rand() % 5 == 0 || _IsMagicEscape != false )
		{
			_IsMagicEscape = false;
			SetState( EM_AIState_Attack );
			return 1;
		}
	}
	else if( _Owner->TempData.Attr.Effect.Fear != false )
	{
		_ProcTime = RoleDataEx::G_SysTime - 15000;
	}

	RoleDataEx* Target = (RoleDataEx*)Global::GetRoleDataByGUID( _Owner->AttackTargetID() );
	if( Target == NULL )
	{
		SetState( EM_AIState_Attack );
		return 10;
	}

	if( _OwnerObj->Path()->StackCount() > 1 )
		return 5;

	float OrgDx = ( Target->X() - _Owner->X() );
	float OrgDz = ( Target->Z() - _Owner->Z() );

	float Len = sqrt( OrgDx*OrgDx + OrgDz*OrgDz );;


	if( Len <= 300 )
	{
		if( _LState != EM_AIState_Atk_Escape )
		{
			float NextDir = _Owner->Dir() + 180.0f ;
			_Owner->CalDir( NextDir , OrgDx , OrgDz );
		}
		else if( _Owner->GUID() % 2 == 0 )
		{
			float NextDir = _Owner->Dir() + 20.0f ;
			_Owner->CalDir( NextDir , OrgDx , OrgDz );
		}
		else
		{
			float NextDir = _Owner->Dir() - 20.0f ;
			_Owner->CalDir( NextDir , OrgDx , OrgDz );
		}
	}
	else
	{
		return 10;
	}

	float X, Y , Z;

	for( int i = 0 ; i < 10 ; i++ )
	{
		float Destance = float( rand() % 40 + 55 );
		float Dx = ( OrgDx )* Destance;
		float Dz = ( OrgDz )* Destance;

		//*�Ȯ�*
		//�p�G����d�򤺴N������
		X = _Owner->BaseData.Pos.X  + Dx;
		Y = _Owner->BaseData.Pos.Y ;
		Z = _Owner->BaseData.Pos.Z  + Dz;

		//�ˬd�O�_���I�i�H��
		if( _OwnerObj->Path()->Height( X , Y , Z , Y , 30 ) == false )
			continue;

		if( _OwnerObj->Path()->CountNode( X , Y , Z ) >= 15 )
			continue;

		_OwnerObj->Role()->IsWalk( true );
		_OwnerObj->Path()->Target( X , Y , Z , _Owner->X() , _Owner->Y() , _Owner->Z() );
		//*�Ȯ�*
		int MoveTime = int( ( _OwnerObj->Path()->TotalLength() * 10 )   /  _Owner->MoveSpeed() + 0.5 );
		int Ret = __max( MoveTime , 5 );

		return Ret;
	}

	return 10;
}
//----------------------------------------------------------------------------------------------------------
bool NpcAIBaseClass::GetAwayfromBattle( )
{
	if(		_Owner->BaseData.Mode.Searchenemy != false 
		||	_Owner->BaseData.Mode.Move == false 
		||	_Owner->TempData.Attr.Effect.Stop != false
		||	_Owner->BaseData.Mode.Escape	== false	)
		return false;

	if( _Owner->TempData.AroundPlayer == 0 || _Owner->BaseData.Mode.Fight == false )
		return false;

	if( _Owner->OwnerGUID() != -1 )
		return false;

	//	if( _Owner->TargetID() != -1 )
	//		return false;

	RoleDataEx* Target = Global::SearchAttackModeObj( _OwnerObj , 50 );
	if( Target != NULL )
	{
		_Owner->TargetID( Target->GUID() );
		_Owner->TempData.AI.ActionType = EM_ActionType_ESCAPE;

		_Owner->StopMove();
//		_OwnerObj->Path()->ClearPath();

		return true;
	}
	return false;

}
//----------------------------------------------------------------------------------------------------------
//�ˬd�O�_�������k�N�����@���b�d��
bool NpcAIBaseClass::_CheckNPCAtkMagicDistance( RoleDataEx* Target , float Len )
{
	GameObjDbStructEx*	MagicDB;  
	GameObjDbStructEx* NPCObjDB = Global::GetObjDB( _Owner->BaseData.ItemInfo.OrgObjID );

	for( int i = 0 ; i < _MAX_NPC_SPELL_MAGIC_COUNT_ ; i++ )
	{
		if( NPCObjDB->NPC.Spell[i].RigthTime != EM_NPCSpellRightTime_KeepDistance )
			continue;

		MagicDB= Global::GetObjDB( NPCObjDB->NPC.Spell[i].MagicID );
		if( MagicDB == NULL )
			continue;
		if( MagicDB->IsMagicCollect() == false )
			continue;
		if( MagicDB->MagicCol.AttackDistance < Len )
			continue;
		if( rand() % 100 > NPCObjDB->NPC.Spell[i].Rate )
			continue;

//		if( strlen(NPCObjDB->NPC.Spell[i].SpellString) != 0 )
//			_Owner->Say( NPCObjDB->NPC.Spell[i].SpellString );


		//�ˬd�惡�ؼЬO�_�i�H�I�k
		if( _Owner->CheckCanSpell(  Target , Target->Pos()->X , Target->Pos()->Y , Target->Pos()->Z, MagicDB  ) != EM_GameMessageEvent_OK )
			continue;

		return true;		
	}
	return false;
}
//----------------------------------------------------------------------------------------------------------
RoleDataEx* NpcAIBaseClass::SearchNPCMagicTarget( RoleDataEx* Target , NPCSpellTargetENUM TargetType , int MagicID )
{
	vector<BaseSortStruct>*	SearchList;
	GameObjDbStructEx* MagicObjDB = Global::GetObjDB( MagicID );
	vector< RoleDataEx* > TargetList;

	if( Target == NULL )
		Target = _Owner;

	if( MagicObjDB->IsMagicCollect() == false )
		return NULL;

	GameObjDbStructEx*	AssistMagicDB = NULL;

	//�p�G�k�N�����绲�U�k�N
	for( int i = 0 ; i < _MAX_MAGICCOLLECT_COUNT_ ; i++ )	
	{
		//GameObjDbStructEx* MagicBaseObjDB = (GameObjDbStructEx*)MagicObjDB->MagicCol.Magic[0][i].MagicBaseDB;
		GameObjDbStructEx* MagicBaseObjDB = Global::GetObjDB( MagicObjDB->MagicCol.Magic[0][i].MagicBaseID );
		if( MagicBaseObjDB == NULL )
			continue;

		if( !MagicBaseObjDB->IsAssistMagic() )
			continue;

		AssistMagicDB = MagicBaseObjDB;
	}



	switch( TargetType )
	{
	case EM_NPCSpellTarget_Self:		//�ۤv
		if( AssistMagicDB != NULL && _Owner->CheckHasAssistMagic( AssistMagicDB ) != false )
			return NULL;
		return _Owner;
	case EM_NPCSpellTarget_Target:		//�ؼ�
		return Target;
	case EM_NPCSpellTarget_Target_HP_Half:		//�ؼ�(HP 1/2)
		{
			if( Target->MaxHP() >= Target->HP()*2 )
				return Target;
		}
		break;
	case EM_NPCSpellTarget_Target_HP_Quarter:	//�ؼ�(HP 1/4)
		{
			if( Target->MaxHP() >= Target->HP()*4 )
				return Target;
		}
		break;
	case EM_NPCSpellTarget_Target_Distance_30_:	//�ؼ�(�Z��30�H�W )
		{
			if( _Owner->Length( Target ) > 30 )
				return Target;
		}
		break;
	case EM_NPCSpellTarget_Target_Escape:		//�ؼ�(�k�])
		{
			if( _Owner->Length( Target ) < 30 )
				return NULL;

			//���V�p��(Target����Owner)
			if( Target->CalAngle( _Owner ) < 90 )
				return NULL;

			return Target;

		}
		break;
	case EM_NPCSpellTarget_Friend:				//�ͤ�( �]�A�ۤv )
		{
			SearchList = Global::SearchRangeObject( _Owner , _Owner->X() , _Owner->Y() , _Owner->Z() , EM_SearchRange_Friend , MagicObjDB->MagicCol.AttackDistance	);

			//Buff �ˬd
			for( unsigned int i = 0 ; i < (*SearchList).size() ; i++ )
			{
				RoleDataEx* Role = (RoleDataEx*)( (*SearchList)[i].Data );

				if( Role == NULL )
					continue;

				if( Role->IsAttack() == false )
					continue;

				if( AssistMagicDB != NULL &&  Role->CheckHasAssistMagic( AssistMagicDB ) != false )
					continue;
				TargetList.push_back( Role );
			}
			if( TargetList.size() == 0 )
				return NULL;

			int ID = rand() % int( TargetList.size() ); 
			return TargetList[ ID ];

		}
		break;
	case EM_NPCSpellTarget_Friend_HP_Half:		//�ͤ�(HP 1/2)
		{
			SearchList = Global::SearchRangeObject( _Owner , _Owner->X() , _Owner->Y() , _Owner->Z() , EM_SearchRange_Friend , MagicObjDB->MagicCol.AttackDistance	);

			//Buff �ˬd
			for( unsigned int i = 0 ; i < (*SearchList).size() ; i++ )
			{
				RoleDataEx* Role = (RoleDataEx*)( (*SearchList)[i].Data );

				if( Role == NULL )
					continue;

				if( Role->IsAttack() == false )
					continue;

				if( Role->MaxHP() < Role->HP()*2 )
					continue;

				if( AssistMagicDB != NULL &&  Role->CheckHasAssistMagic( AssistMagicDB ) != false )
					continue;

				TargetList.push_back( Role );
			}
			if( TargetList.size() == 0 )
				return NULL;

			int ID = rand() % int( TargetList.size() );
			return TargetList[ ID ];
		}
		break;
	case EM_NPCSpellTarget_Friend_HP_Quarter:	//�ͤ�(HP 1/4)
		{
			SearchList = Global::SearchRangeObject( _Owner , _Owner->X() , _Owner->Y() , _Owner->Z() , EM_SearchRange_Friend , MagicObjDB->MagicCol.AttackDistance	);

			//Buff �ˬd
			for( unsigned int i = 0 ; i < (*SearchList).size() ; i++ )
			{
				RoleDataEx* Role = (RoleDataEx*)( (*SearchList)[i].Data );

				if( Role == NULL )
					continue;

				if( Role->IsAttack() == false )
					continue;

				if( Role->MaxHP() < Role->HP()*4 )
					continue;

				if( AssistMagicDB != NULL &&  Role->CheckHasAssistMagic( AssistMagicDB ) != false )
					continue;

				TargetList.push_back( Role );
			}
			if( TargetList.size() == 0 )
				return NULL;

			int ID = rand() % int( TargetList.size() );
			return TargetList[ ID ];
		}
		break;
	case EM_NPCSpellTarget_Enemy:				//�Ĥ�
		{
			for( int i = 0 ; i < _Owner->TempData.NPCHate.Hate.Size() ;  i++ )
			{		
				RoleDataEx* HateRole = Global::GetRoleDataByGUID( _Owner->TempData.NPCHate.Hate[ i ].ItemID );
				if( HateRole == NULL )
					continue;
				if( _Owner->Length( HateRole ) > MagicObjDB->MagicCol.AttackDistance )
					continue;
				TargetList.push_back( HateRole );
			}
			if( TargetList.size() == 0 )
				return NULL;

			int ID = rand() % int( TargetList.size() );
			return TargetList[ ID ];
		}
		break;
	case EM_NPCSpellTarget_Enemy_HP_Half:		//�Ĥ�(HP 1/2)
		{
			for( int i = 0 ; i < _Owner->TempData.NPCHate.Hate.Size() ;  i++ )
			{		
				RoleDataEx* HateRole = Global::GetRoleDataByGUID( _Owner->TempData.NPCHate.Hate[ i ].ItemID );
				if( HateRole == NULL )
					continue;
				if( HateRole->MaxHP() < HateRole->HP() *2 )
					continue;
				if( _Owner->Length( HateRole ) > MagicObjDB->MagicCol.AttackDistance )
					continue;
				TargetList.push_back( HateRole );
			}
			if( TargetList.size() == 0 )
				return NULL;

			int ID = rand() % int( TargetList.size() );
			return TargetList[ ID ];

		}
		break;
	case EM_NPCSpellTarget_Enemy_HP_Quarter:	//�Ĥ�(HP 1/4)
		{
			for( int i = 0 ; i < _Owner->TempData.NPCHate.Hate.Size() ;  i++ )
			{		
				RoleDataEx* HateRole = Global::GetRoleDataByGUID( _Owner->TempData.NPCHate.Hate[ i ].ItemID );
				if( HateRole == NULL )
					continue;
				if( HateRole->MaxHP() < HateRole->HP() *4 )
					continue;
				if( _Owner->Length( HateRole ) > MagicObjDB->MagicCol.AttackDistance )
					continue;
				TargetList.push_back( HateRole );
			}
			if( TargetList.size() == 0 )
				return NULL;

			int ID = rand() % int( TargetList.size() );
			return TargetList[ ID ];
		}
		break;
	case EM_NPCSpellTarget_TargetSpell:				//�ؼЬI�k
		{
			if( Target->IsSpell() )
				return Target;
		}
		break;
	case EM_NPCSpellTarget_Enemy_RangeEnemySpell:	//��������I�k�ؼ� 
		{
			for( int i = 0 ; i < _Owner->TempData.NPCHate.Hate.Size() ;  i++ )
			{		
				RoleDataEx* HateRole = Global::GetRoleDataByGUID( _Owner->TempData.NPCHate.Hate[ i ].ItemID );
				if( HateRole == NULL )
					continue;
				if( HateRole->IsSpell() == false )
					continue;
				if( _Owner->Length( HateRole ) > MagicObjDB->MagicCol.AttackDistance )
					continue;
				TargetList.push_back( HateRole );
			}
			if( TargetList.size() == 0 )
				return NULL;

			int ID = rand() % int( TargetList.size() );
			return TargetList[ ID ];
		}
		break;
	case EM_NPCSpellTarget_Enemy_Second:			//����ĤG
		{
			if( _Owner->TempData.NPCHate.Hate.Size() < 2 )
				return NULL;

			RoleDataEx* HateRole = Global::GetRoleDataByGUID( _Owner->TempData.NPCHate.Hate[ 1 ].ItemID );
			if( HateRole == NULL )
				return NULL;

			if( _Owner->Length( HateRole ) > MagicObjDB->MagicCol.AttackDistance )
				return NULL;

			return HateRole;
		}
		break;
	case EM_NPCSpellTarget_Enemy_Last:				//����̤p
		{
			if( _Owner->TempData.NPCHate.Hate.Size() == 0 )
				return NULL;

			int LastID = _Owner->TempData.NPCHate.Hate.Size() -1;
			RoleDataEx* HateRole = Global::GetRoleDataByGUID( _Owner->TempData.NPCHate.Hate[ LastID ].ItemID );

			if( HateRole == NULL )
				return NULL;

			if( _Owner->Length( HateRole ) > MagicObjDB->MagicCol.AttackDistance )
				return NULL;

			return HateRole;
		}
		break;
	case EM_NPCSpellTarget_Enemy_Nearest:			//�����̪�
		{
			RoleDataEx* NearestRole = NULL;
			float NearestLength = 10000;
			for( int i = 0 ; i < _Owner->TempData.NPCHate.Hate.Size() ;  i++ )
			{		
				RoleDataEx* HateRole = Global::GetRoleDataByGUID( _Owner->TempData.NPCHate.Hate[ i ].ItemID );
				if( HateRole == NULL )
					continue;

				float Length = _Owner->Length( HateRole );

				if( Length > MagicObjDB->MagicCol.AttackDistance )
					continue;

				if( Length >= NearestLength )
					continue;

				NearestRole = HateRole;
				NearestLength = Length;
			}

			return NearestRole;
		}
		break;
	case EM_NPCSpellTarget_Enemy_Farest:			//�����̻���
		{
			RoleDataEx* FarRole = NULL;
			float FarLength = 0;
			for( int i = 0 ; i < _Owner->TempData.NPCHate.Hate.Size() ;  i++ )
			{		
				RoleDataEx* HateRole = Global::GetRoleDataByGUID( _Owner->TempData.NPCHate.Hate[ i ].ItemID );
				if( HateRole == NULL )
					continue;

				float Length = _Owner->Length( HateRole );

				if( Length > MagicObjDB->MagicCol.AttackDistance )
					continue;

				if( Length <= FarLength )
					continue;

				FarRole = HateRole;
				FarLength = Length;
			}

			return FarRole;
		}
		break;
	case EM_NPCSpellTarget_Master:				//�D�H
		{
			RoleDataEx* Master =  Global::GetRoleDataByGUID( _Owner->OwnerGUID() );
			if( Master != NULL )
				return Master;
		}
		break;
	case EM_NPCSpellTarget_Master_HP_Half:		//�D�H(HP 1/2)
		{
			RoleDataEx* Master =  Global::GetRoleDataByGUID( _Owner->OwnerGUID() );
			if( Master == NULL )
				return NULL;
			if( Master->MaxHP() >= Master->HP()*2 )
				return Master;
		}
		break;
	case EM_NPCSpellTarget_Master_HP_Quarter:	//�D�H(HP 1/4)
		{
			RoleDataEx* Master =  Global::GetRoleDataByGUID( _Owner->OwnerGUID() );
			if( Master == NULL )
				return NULL;
			if( Master->MaxHP() >= Master->HP()*4 )
				return Master;
		}
		break;


	}

	return NULL;
}

//�]�wNPC�X�в��ʶ}��
void	NpcAIBaseClass::SetMoveToFlagEnabled( bool Value )
{
	_MoveToFlagEnabled = Value;
/*
	if( _MoveToFlagEnabled == false  )
	{
		_Owner->StopMoveNow();
		_OwnerObj->Path()->ClearPath();
		SetState( EM_AIState_Idle );
	}
	*/
}