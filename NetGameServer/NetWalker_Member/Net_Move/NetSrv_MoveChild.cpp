#ifndef _WIN32_WINNT
	#define _WIN32_WINNT  0x0400
#endif

#include <Windows.h>
#include <Winbase.h>

#include "../NetWakerGSrvInc.h"
#include "NetSrv_MoveChild.h"
#include "../Net_Treasure/NetSrv_TreasureChild.h"
#include "../Net_BattleGround/NetSrv_BattleGround_Child.h"
#include <math.h>
#include "../../mainproc/PartyInfoList/PartyInfoList.h"
#include "../../mainproc/GuildHouseManage/GuildHousesClass.h"
#include "../../mainproc/luaPlot/LuaPlot_Hsiang.h"

//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_MoveChild::Init()
{
    NetSrv_Move::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_MoveChild);

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_MoveChild::Release()
{
    if( This == NULL )
        return false;

	NetSrv_Move::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-------------------------------------------------------------------
//  事件繼承實做
//-------------------------------------------------------------------
void    NetSrv_MoveChild::PlayerMove(  BaseItemObject* Sender 
                                      , int MoveID  , float X , float Y , float Z , float Dir 
									  , ClientMoveTypeENUM Type 
									  , float vX , float vY , float vZ 
									  , int AttachObjID )
{
	/*
	//////////////////////////////////////
	static int LastCheckTime = clock() ;
	static int PacketCount = 0;

	PacketCount++;
	int		NowTime = clock() ;
	if( NowTime > LastCheckTime + 1000 )
	{		
		Print(LV2 , "CliConnect" , "Time =%d  PacketCount = %d" , NowTime - LastCheckTime , PacketCount );
		LastCheckTime = NowTime;
		PacketCount = 0;
	}

	//////////////////////////////////////
	*/
	char	Buf[512];
	RoleDataEx* Owner = Sender->Role();

	if( Owner->IsDead() )
		return;

	if(		CheckFloatAvlid( X ) == false 
		||  CheckFloatAvlid( Y ) == false 
		||  CheckFloatAvlid( Z ) == false 
		||  CheckFloatAvlid( Dir ) == false 
		||  CheckFloatAvlid( vX ) == false 
		||  CheckFloatAvlid( vY ) == false 
		||  CheckFloatAvlid( vZ ) == false )
	{
		Global::SendMsgToGMTools( LV5 , Owner->Account_ID() , "Client Postion Value Error" );
		return;
	}

    //暫時只開放自己控制自己的物件
    if( Owner->GUID() != MoveID )
        return ;

    if( Owner->ZoneID() != RoleDataEx::G_ZoneID )
        return ;

    if( Owner->SecRoomID() == -1 )
        return;
	//--------------------------------------------------------------------------------------------------------------
	if( Owner->TempData.Move.IsLockSetPos != false )
	{
		if(		FloatEQU( X , Owner->TempData.Move.LockSetPosX ) != false 
			&&  FloatEQU( Z , Owner->TempData.Move.LockSetPosZ ) != false )
		{
			Owner->TempData.Move.IsLockSetPos = false;

			Owner->TempData.Move.LCheckLineX = X;
			Owner->TempData.Move.LCheckLineY = Y;
			Owner->TempData.Move.LCheckLineZ = Z;

		}
		else
		{
			Owner->TempData.Move.PacketErrorCount ++;
			if( Owner->TempData.Move.PacketErrorCount >= 5 )
			{
				SetObjPos( Sender , Owner->TempData.Move.LOK_CliX , Owner->TempData.Move.LOK_CliY , Owner->TempData.Move.LOK_CliZ  , Owner->Dir() );
				Owner->TempData.Move.PacketErrorCount = 0;
			}
		}
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	//如果在乘坐狀況
	if(	Owner->BaseData.Mode.Move == false  )
		return;
	if( Owner->TempData.AttachObjID > 0 && Owner->TempData.AttachPos != 0 )
		return;
	//////////////////////////////////////////////////////////////////////////
	//如果只是加速Server玩家位置處理
	if( EM_ClientMoveType_OntimeSetCliPos == Type )
	{
		float moveSpeed = Owner->TempData.Attr.Fin.MoveSpeed;
		if( Owner->Length( X , Y , Z ) < moveSpeed / 10 + 2 )
		{
			//如果要移到的距離不太遠
			Owner->Pos()->X = X ;//+ ( pos.vX/4 );
			Owner->Pos()->Y = Y;
			Owner->Pos()->Z = Z ;//+ ( pos.vZ/4 );
			Owner->Pos()->Dir = Dir;

			Owner->TempData.Move.Tx = X;
			Owner->TempData.Move.Ty = Y;
			Owner->TempData.Move.Tz = Z;

			Owner->TempData.Move.Dx = vX * 0.1f;	
			Owner->TempData.Move.Dy = vY * 0.1f;	
			Owner->TempData.Move.Dz = vZ * 0.1f;	
		}
	
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	if( Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_Move ] > 2 )
		return ;
	Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_Move ] += (1.0f/300.0f);
	//////////////////////////////////////////////////////////////////////////
	//檢查目前位置與之前的位置是否可以移動
	bool IsNeedSetCheckLine = false;
	if( Global::Ini()->IsIgnoreCollision == false )
	{
		float CheckDx = Owner->TempData.Move.LCheckLineX - X;
		float CheckDy = Owner->TempData.Move.LCheckLineY - Y;
		float CheckDz = Owner->TempData.Move.LCheckLineZ - Z;

		if( CheckDx*CheckDx + CheckDy*CheckDy + CheckDz*CheckDz >= 16 )
		{
			if( Sender->Path()->CheckLine_Pure( Owner->TempData.Move.LCheckLineX , Owner->TempData.Move.LCheckLineY + 15 , Owner->TempData.Move.LCheckLineZ ,  X , Y + 15 , Z  ) == false )
			{
				Print( LV2 , "Move" , "CheckLine Error DBID=%d" , Owner->DBID() );
				SetObjPos( Sender , Owner->TempData.Move.CliX , Owner->TempData.Move.CliY , Owner->TempData.Move.CliZ  , Owner->Dir() );
				return;
			}

			IsNeedSetCheckLine = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	/*
	if( rand() % 10 == 0 )
	{
		//檢查所在位置是否有問題
		GameObjDbStructEx* ZoneObj = Global::GetObjDB( Def_ObjectClass_Zone + RoleDataEx::G_ZoneID % _DEF_ZONE_BASE_COUNT_ );
		if( ZoneObj != NULL )
		{
			if( ZoneObj->Zone.MapID != PathManageClass::MainMapID( X , Y , Z ) )
			{
				Owner->Msg( "Not in the control area" );
				int ZoneID = GlobalBase::GetParallelZoneID( Owner->SelfData.RevZoneID ,  Owner->PlayerTempData->ParallelZoneID );
				if( ZoneID == -1 )
				{
					Owner->Net_GameMsgEvent( EM_GameMessageEvent_ChanageZoneError_Failed );
					return;
				}
				Global::ChangeZone( Owner->GUID() , ZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir  );
				return;
			}
		}
	}
	*/
	//////////////////////////////////////////////////////////////////////////
	//檢查目標物件是否存在
	RoleDataEx* AttachObj = Global::GetRoleDataByGUID( AttachObjID );

	if(		EM_ClientMoveType_Mount == Type 
		||	EM_ClientMoveType_SetPetPos == Type )
	{
		if( Global::Ini()->IsHouseZone == false )
		{
			if( AttachObj == NULL )
				return;
			if( AttachObj->BaseData.Mode.Obstruct == false )
			{
				Owner->Net_GameMsgEvent( EM_GameMessageEvent_ClientDataError );			
				return;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	//距離總計(檢查距離是否有問題)
	float Dx = Owner->TempData.Move.CliX - X;
	float Dy = Owner->TempData.Move.CliY - Y;
	float Dz = Owner->TempData.Move.CliZ - Z;
	float MoveLen = sqrt( Dx * Dx + Dz * Dz );
	float TempVector = vX* vX + vZ * vZ ;
	float DSpeedRate = 1.0f;

	Owner->TempData.Move.CliX = X;
	Owner->TempData.Move.CliY = Y;
	Owner->TempData.Move.CliZ = Z;

	float DropY = Owner->TempData.Move.BeginJumpY - Y;
	float MoveSpeed = Owner->TempData.Attr.Fin.MoveSpeed; 
	float LastMaxMoveSpeed = Owner->TempData.Attr.LastMaxMoveSpeed;

	BaseItemObject* WagonObj = BaseItemObject::GetObj( Owner->TempData.AttachObjID );
	if( WagonObj != NULL )
	{
		RoleDataEx* WagonRole = WagonObj->Role();
		MoveSpeed = WagonRole->TempData.Attr.Fin.MoveSpeed; 
		LastMaxMoveSpeed = WagonRole->TempData.Attr.LastMaxMoveSpeed;

	}
	if(		Type == EM_ClientMoveType_Mount 
		&&	AttachObj != NULL )
	{
		MoveSpeed += AttachObj->TempData.Attr.Fin.MoveSpeed;
		LastMaxMoveSpeed += AttachObj->TempData.Attr.LastMaxMoveSpeed;
	}


	if( Dy > 0 )
	{
		DSpeedRate = MoveSpeed / (Dy * 0.25f + MoveSpeed);
		MoveLen *= DSpeedRate;
	}
	else
	{
		switch( Type )
		{
		case EM_ClientMoveType_Normal:
			{
				if( DropY < -100 )
				{
					SetObjPos( Sender , Owner->TempData.Move.LOK_CliX , Owner->TempData.Move.LOK_CliY , Owner->TempData.Move.LOK_CliZ  , Owner->Dir() );
					return;
				}

				if( Ini()->IsCheckHeight && Owner->TempData.Attr.Action.SrvSetPos == false )
				{
					if(		Owner->TempData.CliState.InWater == false 
						&&	Owner->TempData.Attr.Effect.Fly == false
						&&	Owner->TempData.Attr.Effect.WaterWalk == false	)
					{
						if(  /*DropY < -40 */ rand() % 20 == 0 )
						{
							float Height;
							bool CheckResult = Sender->Path()->Height( X , Y , Z , Height , 100 );
							if(		CheckResult == false 
								||	Y - Height > 50 )
							{
								bool IsCheckOk = false;
								float Dx[4] = {3,0,0,-3};
								float Dz[4] = {0,3,-3,0};
								for( int i = 0 ; i < 4 ; i++ )
								{
									CheckResult = Sender->Path()->Height( X+Dx[i] , Y , Z+Dz[i] , Height , 100 );
									if(		CheckResult != false 
										&&	Y-Height < 10	)
									{
										IsCheckOk = true;
										break;
									}
								}

								if( IsCheckOk== false )
								{
									Owner->Net_GameMsgEvent( EM_GameMessageEvent_ClientDataError );			
									return;
								}
							}
							Y = Height;
						}
					}
				}
				
			}
			break;
		case EM_ClientMoveType_Jump:
			if( DropY < -50 && AttachObj == NULL )
			{
				SetObjPos( Sender , Owner->TempData.Move.LOK_CliX , Owner->TempData.Move.LOK_CliY , Owner->TempData.Move.LOK_CliZ  , Owner->Dir() );
				return;
			}
			break;
		case EM_ClientMoveType_SetPos:		//中途換目標
			break;

		case EM_ClientMoveType_Mount:		//在會移動物件上(載具 如電梯)
			break;
		case EM_ClientMoveType_SetPetPos:		//在會移動物件上(載具 如電梯),修正寵物坐標
			break;
		default:
			return;
		}

	}


	//向量檢查
	if( ( TempVector ) > MoveSpeed * MoveSpeed * 1.2f * DSpeedRate * DSpeedRate )
	{
		Owner->TempData.Move.PacketErrorCount ++;
		if( Owner->TempData.Move.PacketErrorCount >= 5 )
		{
			sprintf( Buf , "Move Error Speed(vector) RoleName = %s Speed = %f Client Vector Speed = %f" , Utf8ToChar( Owner->RoleName() ).c_str()  , MoveSpeed , sqrt( TempVector ) );
			Print( LV2 , "Move" , Buf );

			//Owner->Net_GameMsgEvent( EM_GameMessageEvent_MoveVectorError );			


			if( Owner->TempData.Move.PacketErrorCount >= 10 )
			{
				Global::SendMsgToGMTools( LV5 , Owner->Account_ID() , Buf );
				Global::Log_PlayerMoveErr( Owner , EM_PlayerMoveErrType_VecterErr );
				//Sender->Destroy( "SYS PlayerMove" );
				SetObjPos( Sender , Owner->TempData.Move.LOK_CliX , Owner->TempData.Move.LOK_CliY , Owner->TempData.Move.LOK_CliZ  , Owner->Dir() );
				Owner->TempData.Move.PacketErrorCount = 0;
				return;
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	RoleDataEx* DropRole = Owner;
	if( Owner->TempData.AttachObjID > 0 )
	{
		//取出 載具 並移動載具
		if( WagonObj != NULL )
			DropRole = WagonObj->Role();
	}
	
	if( EM_ClientMoveType_Jump == Type  )
	{
		if( DropY > 1000  )
		{
			Print( LV3 , "Move" , "DropY > 1000   DBID = %d" , Owner->DBID() );
			DropRole->AddHP( Owner , -1 * DropRole->MaxHP() - 1000 );
			return;
		}
	}
	else
	{
		if( DropY >= 900 )
		{
			DropRole->AddHP( DropRole , DropRole->MaxHP() * -1 -1000 );
			return;
		}
		else if( Type != EM_ClientMoveType_Jump )
		{
			if( DropY > 100  )
			{
				if( DropY > 900 )
					DropY = 900;

				int Damage = int( ( DropY - 100 ) * DropRole->TempData.Attr.Fin.MaxHP / 800 );
				DropRole->AddHP( DropRole , float( Damage * -1 ) );
				S_PlayerDropDownDamage( DropRole->GUID () , Damage , 0 );
				if( DropRole->IsDead() )
					return;
			}
			else if( DropY < -50 )
			{
				if(		Type != EM_ClientMoveType_Mount 
					&&	Type != EM_ClientMoveType_SetPetPos )
				{
					Owner->Net_GameMsgEvent( EM_GameMessageEvent_ClientDataError );			
					return;
				}
			}
			Owner->TempData.Move.BeginJumpX = X;
			Owner->TempData.Move.BeginJumpY = Y;
			Owner->TempData.Move.BeginJumpZ = Z;
		}

	}
	//////////////////////////////////////////////////////////////////////////

	
	bool	IsStopProc = false;
	//如果受法術影響
	if( Owner->TempData.Attr.Effect.Stop )
	{		
		if(  FloatEQU( vX , 0) == false || FloatEQU( vZ , 0) == false )
		{
			Owner->TempData.Move.PacketErrorCount ++;
			if( Owner->TempData.Move.PacketErrorCount >= 5 )
			{
				Print( LV2 , "Move" , "Move Error Effect.Stop != false ");	
				Owner->TempData.Move.PacketErrorCount = 0;
			}
			return;
		}
		else
		{
			IsStopProc = true;
		}

	}

	//單次距離檢查
	if(		MoveLen > LastMaxMoveSpeed * 1.2f 
		&&	Global::Ini()->IsIgnoreCollision == false )
	{
		//Owner->TempData.Move.PacketErrorCount ++;
		if( MoveLen > LastMaxMoveSpeed * 2.4f )
		{
			Owner->TempData.Move.PacketErrorCount ++;
			sprintf( Buf , "Move Error Speed RoleName = %s Transportable distance = %f  Move distance = %f" , Utf8ToChar( Owner->RoleName() ).c_str() , Owner->TempData.Attr.LastMaxMoveSpeed , MoveLen );
			Print( LV2 , "Move" , Buf );

			Global::Log_PlayerMoveErr( Owner , EM_PlayerMoveErrType_MoveDistErr );
			Global::SendMsgToGMTools( LV5 , Owner->Account_ID() , Buf );	
			SetObjPos( Sender , Owner->TempData.Move.LOK_CliX , Owner->TempData.Move.LOK_CliY , Owner->TempData.Move.LOK_CliZ  , Owner->Dir() );
			Owner->TempData.Move.PacketErrorCount = 0;
			return;
		}

		if( IsStopProc != false )
			return;
	}

	Owner->TempData.Move.CalMoveLen += MoveLen;

	if( MoveLen >= 0.1 || vX != 0 || vZ != 0 || Type == EM_ClientMoveType_Jump )
	{
		Owner->TempData.BackInfo.BuffClearTime.Move = true;
		Owner->TempData.Magic.IsMove =true;
		Owner->TempData.Attr.Action.Sit = false;
	}
	
	//總距離檢查
	myUInt32 DTime = RoleDataEx::G_SysTime - Owner->TempData.Move.CliLastMoveTime;

	if( DTime > 10000 || ( MoveLen > 0.01 && TempVector < 0.01 ) )
	{
		SendAllPartyMemberPos( Owner );
	}

	//////////////////////////////////////////////////////////////////////////
	//移過

	//if( Owner->TempData.Move.CalMoveLen > Owner->TempData.Attr.LastMaxMoveSpeed * 10 )
	if( DTime > 10000  && Global::Ini()->IsIgnoreCollision == false )
	{
		float DLenPerSec = Owner->TempData.Move.CalMoveLen / ( DTime / 1000.0f);

		if( DLenPerSec > LastMaxMoveSpeed * 1.2f   )
		{
			sprintf( Buf , "Move Error Speed(Total) RoleName = %s Speed = %f Client Average Speed= %f" , Owner->RoleName() , LastMaxMoveSpeed , DLenPerSec );
			Print( LV2 , "Move" , Buf );
			//Owner->Net_GameMsgEvent( EM_GameMessageEvent_TotalMoveSpeedToFastError );
			Global::SendMsgToGMTools( LV5 , Owner->Account_ID() , Buf );
			Global::Log_PlayerMoveErr( Owner , EM_PlayerMoveErrType_TotalMoveDistErr );
			if( Owner->TempData.Move.MoveTotalDistErrCount >= 2 )
			{
				Print( LV3 , "Move" , "DLenPerSec > Owner->TempData.Attr.LastMaxMoveSpeed * 1.2f  DBID = %d" , Owner->DBID() );
				Owner->AddHP( Owner , -1 * Owner->MaxHP() -1000 );
			}
			else
			{
				int BackErrCount = Owner->TempData.Move.MoveTotalDistErrCount+1;
				SetObjPos( Sender , Owner->TempData.Move.LOK_CliX , Owner->TempData.Move.LOK_CliY , Owner->TempData.Move.LOK_CliZ  , Owner->Dir() );
				Owner->TempData.Move.MoveTotalDistErrCount = BackErrCount;
			}


			Owner->TempData.Attr.LastMaxMoveSpeed = Owner->TempData.Attr.Fin.MoveSpeed;
			Owner->TempData.Move.CalMoveLen = 0;
			Owner->TempData.Move.CliLastMoveTime = RoleDataEx::G_SysTime;			
			return;
		}
	}

	//以10秒為單位
	if(		DTime > 10000 
		||	AttachObj != NULL )
	{
		Owner->TempData.Move.MoveTotalDistErrCount= 0;
		Owner->TempData.Attr.LastMaxMoveSpeed = Owner->TempData.Attr.Fin.MoveSpeed;

		Owner->TempData.Move.CalMoveLen = 0;
		Owner->TempData.Move.CliLastMoveTime = RoleDataEx::G_SysTime;
		Owner->TempData.Move.LOK_CliX = X;
		Owner->TempData.Move.LOK_CliY = Y;
		Owner->TempData.Move.LOK_CliZ = Z;
	}

	//////////////////////////////////////////////////////////////////////////
	//--------------------------------------------------------------------------------------------------------------
	if( Type == EM_ClientMoveType_Jump && Owner->TempData.Move.MoveType != EM_ClientMoveType_Jump )
	{
		Owner->TempData.Move.BeginJumpTime = RoleDataEx::G_SysTime;
	}


	Owner->BaseData.Pos.X = X;
	Owner->BaseData.Pos.Y = Y;
	Owner->BaseData.Pos.Z = Z;
	Owner->BaseData.Pos.Dir = Dir;

	Owner->TempData.Move.Tx = X;
	Owner->TempData.Move.Ty = Y;
	Owner->TempData.Move.Tz = Z;

	Owner->TempData.Move.Dx = vX;	
	Owner->TempData.Move.Dy = vY;	
	Owner->TempData.Move.Dz = vZ;	
	Owner->TempData.Move.MoveType = Type;

	Owner->TempData.Attr.Action.SrvSetPos = false;
	MoveObj( Sender );

	/*if( Owner->TempData.AttachObjID <= 0 )
	{
		MoveObj( Sender );
	}
	else*/
	if( Owner->TempData.AttachObjID > 0 )
	{
		//取出 載具 並移動載具
		
		if( WagonObj != NULL )
		{
			RoleDataEx* WagonRole = WagonObj->Role();

			WagonObj->Path()->SetPos_NoCheckHeight( Owner->X() , Owner->Y() , Owner->Z() );
			WagonRole->BaseData.Pos.Dir = Owner->Dir();
			WagonRole->TempData.Move.Tx = WagonRole->TempData.Move.CliX = WagonRole->TempData.Move.LCheckLineX = WagonRole->Pos()->X = Owner->X();
			WagonRole->TempData.Move.Ty = WagonRole->TempData.Move.CliY = WagonRole->TempData.Move.LCheckLineY = WagonRole->Pos()->Y = Owner->Y();
			WagonRole->TempData.Move.Tz = WagonRole->TempData.Move.CliZ = WagonRole->TempData.Move.LCheckLineZ = WagonRole->Pos()->Z = Owner->Z();

			MoveObj( WagonObj );


		}
	}

	Owner->BaseData.Pos.X = X;// + ( vX/3 );
	Owner->BaseData.Pos.Z = Z;//+ ( vZ/3 );
	
//	Sender->Path()->SetPos( Owner->BaseData.Pos.X , Owner->BaseData.Pos.Y , Owner->BaseData.Pos.Z );
	//////////////////////////////////////////////////////////////////////////
	if( Owner->BaseData.Pos.Y <= -100000  )
	{
		Sender->Path()->SetPos( Owner->BaseData.Pos.X , Owner->BaseData.Pos.Y , Owner->BaseData.Pos.Z );
	}
	else if(	rand() % 20 == 0
			&&	Type == EM_ClientMoveType_Normal	)
	{
		/*float RetY = Sender->Path()->SetPos( Owner->BaseData.Pos.X , Owner->BaseData.Pos.Y , Owner->BaseData.Pos.Z );

		if( abs( RetY - Owner->BaseData.Pos.Y ) >= 10 )
		{
			Owner->TempData.Move.MoveHeightErrorCount++;
			Print( LV4 , "Move" , "Client DBID =%d X=%f ,Y=%f,Z=%f  RetY = %f"   ,  Owner->DBID() , Owner->BaseData.Pos.X , Owner->BaseData.Pos.Y , Owner->BaseData.Pos.Z , RetY );
		}*/
		Sender->Path()->SetPos_NoCheckHeight( Owner->BaseData.Pos.X , Owner->BaseData.Pos.Y , Owner->BaseData.Pos.Z );
	}
	else
	{
		Sender->Path()->SetPos_NoCheckHeight( Owner->BaseData.Pos.X , Owner->BaseData.Pos.Y , Owner->BaseData.Pos.Z );
	}
	//////////////////////////////////////////////////////////////////////////
	//移動寵物到主人位置
	if( Type == EM_ClientMoveType_SetPetPos )
	{
		float	DX[4] = { 5 , 5 , -5 ,-5 };
		float	DZ[4] = { 5 , -5 , 5 ,-5 };
		SummonPetInfoStruct& SummonPet = Owner->TempData.SummonPet;
		for( int i = 0 ; i < EM_SummonPetType_Max ; i++ )
		{
			RoleDataEx* Pet = Global::GetRoleDataByGUID( SummonPet.Info[i].PetID );
			if( Pet == NULL || Pet->IsAttackMode() )
				continue;
			if( Pet->OwnerDBID() != Owner->DBID() )
				continue;

			Pet->Pos()->X += DX[i];
			Pet->Pos()->Z += DZ[i];
			Global::SetPos( Pet->GUID() );
			//Pet->SetPos_NoCheckHeight( Owner->BaseData.Pos.X + DX[i] , Owner->BaseData.Pos.Y , Owner->BaseData.Pos.Z + DZ[i] )		
		}
	}
	//////////////////////////////////////////////////////////////////////////

	Owner->TempData.Move.PacketErrorCount = 0;


	_CheckDeadZone( Sender , Owner , X , Y , Z );

	if( IsNeedSetCheckLine )
	{
		Owner->TempData.Move.LCheckLineX = X;
		Owner->TempData.Move.LCheckLineY = Y;
		Owner->TempData.Move.LCheckLineZ = Z;
		Owner->TempData.Sys.NeedFixClientPos = true;
	}

	if( abs( int( Owner->TempData.Move.LCheckMovePGDir - Dir) ) > 0 )
		Owner->TempData.Sys.NeedFixClientPos = true;

	if(		Owner->TempData.Sys.NeedFixClientPos 
		||	Owner->TempData.AroundPlayer	<= _MAX_MOVE_SENDALL_COUNT_ )
	{
		Owner->TempData.Move.LCheckMovePGDir = Dir;
	}

	if( Owner->TempData.AroundPlayer <= _MAX_MOVE_SENDALL_COUNT_ )
	{
		Owner->TempData.Sys.NeedFixClientPos = false;
	}
}

void NetSrv_MoveChild::_CheckDeadZone( BaseItemObject* OwnerObj , RoleDataEx* Owner , float X , float Y , float Z )
{
	if(		Global::Ini()->IsNeedCheckDeadZone != false 
		&&	Owner->IsDead() == false )
	{

		float CheckDx = Owner->TempData.Move.LCheckDeadZoneX - X;
		float CheckDy = Owner->TempData.Move.LCheckDeadZoneY - Y;
		float CheckDz = Owner->TempData.Move.LCheckDeadZoneZ - Z;

		if( Owner->TempData.ZoneType != EM_ZoneType_NORMAL || CheckDx*CheckDx + CheckDy*CheckDy + CheckDz*CheckDz >= 10*10 )
		{
			Owner->TempData.Move.LCheckDeadZoneX = X;
			Owner->TempData.Move.LCheckDeadZoneY = Y;
			Owner->TempData.Move.LCheckDeadZoneZ = Z;

			float DamageRate = 1 ;
			ZoneTypeENUM ZoneType = (ZoneTypeENUM)OwnerObj->Path()->GetZoneType( Owner->BaseData.Pos.X , Owner->BaseData.Pos.Y , Owner->BaseData.Pos.Z , &DamageRate );

			if( Owner->TempData.ZoneType != ZoneType )
			{
				RoleDataEx *Other = Owner;
				vector< AttachObjInfoStruct >& AttachList = OwnerObj->AttachList();		
				for( int i = -1 ; i < (int)AttachList.size() ; i++ )
				{
					if( i != -1 )
					{
						Other = Global::GetRoleDataByGUID( AttachList[i].ItemID );
						if( Other == NULL || Other->IsPlayer() == false )
							continue;
					}

					Other->TempData.ZoneType = ZoneType;
					Other->TempData.ZoneDamgeRate = DamageRate;

					switch( ZoneType )
					{
					case EM_ZoneType_NORMAL:
						Other->TempData.DeadZoneCountDown = 0;
						Other->TempData.ZoneDamgeRate = 0;
						break;
					case EM_ZoneType_DEADZONE:
						if( Other->TempData.DeadZoneCountDown == 0 )
						{
							if( Other->BaseData.Voc != EM_Vocation_GameMaster )
							{
								Other->TempData.DeadZoneCountDown = 10;
							}
							Other->TempData.ZoneDamgeRate = 0;
						}
						break;
					case EM_ZoneType_DAMAGEZONE:
						{
							Other->TempData.DeadZoneCountDown = 0;
						}
						break;
					case EM_ZoneType_IMMEDIATEDEATHZONE:
						{
							Other->TempData.DeadZoneCountDown = 0;
							Other->TempData.ZoneDamgeRate = 0;
						}
						break;
					default:
						{
							Other->TempData.DeadZoneCountDown = 0;
							Other->TempData.ZoneDamgeRate = 0;
						}
						break;
					}
					S_DeadZone( Other->GUID() , ZoneType , Other->TempData.DeadZoneCountDown );
				}
			}
		}

	}

}
//-------------------------------------------------------------------
//物件移動
void NetSrv_MoveChild::MoveObj( RoleDataEx* Owner )
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObj( Owner->GUID() );
	if( OwnerObj == NULL )
		return;
	MoveObj( OwnerObj );
}
//物件移動
void    NetSrv_MoveChild::MoveObj( BaseItemObject* OwnerObj  )
{
    RoleDataEx*     Owner = OwnerObj->Role();

    PathProcStruct*	MoveInfo = &Owner->TempData.Move;

	if( Owner->BaseData.Mode.IsAllZoneVisible == false )
		MovePartition( OwnerObj );

    NormalMove( OwnerObj );
    //通知Partition Srv
/*	
	if( Owner->IsPlayer() )
	{
		CNetSrv_PartBase::MoveObj(	Owner->GUID() 
								  , Owner->TempData.Move.Tx 
								  , Owner->TempData.Move.Ty 
								  , Owner->TempData.Move.Tz 
								  , Owner->Dir() );
	}
	*/

	Owner->TempData.BackInfo.LX = Owner->Pos()->X;
	Owner->TempData.BackInfo.LZ = Owner->Pos()->Z;
}

//物件一動
void NetSrv_MoveChild::SendRangeMove( RoleDataEx* Owner , ClientMoveTypeENUM Type )
{

	Owner->TempData.Move.MoveType = Type;
	//_Def_View_Block_Range_
	PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

	int			i , j;
	PlayID*		TopID;

	for( i = 0 ; Block[i] != NULL ; i++ )
	{
		TopID = Block[i]->Begin;

		for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
		{
			BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );			
			if( OtherObj == NULL )
				continue;
			RoleDataEx* Other = OtherObj->Role();
			if( Other->IsPlayer() == false )
				continue;

			Move( OtherObj->GUID() , Owner );
		}
	}

}
//-------------------------------------------------------------------
//重設某物件的位置
bool    NetSrv_MoveChild::SetObjPos( BaseItemObject* OwnerObj ,  float X , float Y , float Z , float Dir )
{

    RoleDataEx*     Owner = OwnerObj->Role();

    PathProcStruct*	MoveInfo = &Owner->TempData.Move;

	Owner->Pos( X , Y , Z , Dir );

    if( Owner->SecRoomID() == -1 )
        return false;
    //---------------------------------------------------------------------------------------------
    //	重設移動PosList資訊
    //---------------------------------------------------------------------------------------------
    SpaceStruct		Pos;
    Pos.RoomID	= Owner->RoomID();
	Pos.Y = 0;

	Pos.X = Owner->TempData.Move.LockTargetPosX;
	Pos.Z = Owner->TempData.Move.LockTargetPosZ;
	_St->PosList.erase( Pos );


    Pos.X = (int)(X / _Def_Map_BaseSize_);
    Pos.Z = (int)(Z / _Def_Map_BaseSize_);

    _St->PosList[ Pos ] = Owner;
	Owner->TempData.Move.LockTargetPosX = Pos.X;
	Owner->TempData.Move.LockTargetPosZ = Pos.Z;
	Owner->TempData.Attr.Action.SrvSetPos = true;
       
    MovePartition( OwnerObj );

    //---------------------------------------------------------------------------------------------
	//如果為全域的物件 送給所有的玩家
	if( Owner->BaseData.Mode.IsAllZoneVisible != false )
	{
		SetPos_AllCli( Owner );
	}
	else
	{
		//_Def_View_Block_Range_
		PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

		int			i , j;
		PlayID*		TopID;

		for( i = 0 ; Block[i] != NULL ; i++ )
		{
			TopID = Block[i]->Begin;

			for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
			{
				BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
				if( OtherObj == NULL )
					continue;

				//送給鄒為的玩家
				SetPos( OtherObj->GUID() , Owner );
			}
		}
	}

   
    //-----------------------------------------------------------------------------------------------
    //通知Partition Srv
	if( Owner->IsPlayer() )
	{
		CNetSrv_PartBase::MoveObj( Owner->GUID() , Owner->TempData.Move.Tx , Owner->TempData.Move.Ty , Owner->TempData.Move.Tz , Owner->Dir() );
	}
    //-----------------------------------------------------------------------------------------------
    OwnerObj->Path()->SetPos( Owner->Pos()->X , Owner->Pos()->Y , Owner->Pos()->Z );
    return true;

}
//-------------------------------------------------------------------
//換Roomid
bool    NetSrv_MoveChild::ChangeRoom( BaseItemObject* OwnerObj , int RoomID )
{
	if( (unsigned)RoomID >= Global::St()->PrivateRoomInfoList.size() )
		return false;

    RoleDataEx*     Owner = OwnerObj->Role();

    if( !DelFromPartition( OwnerObj ) )
        return false;

    Owner->RoomID( RoomID );

    AddToPartition( OwnerObj );
    return true;
}
//-------------------------------------------------------------------
//物件所在區塊更新
bool	NetSrv_MoveChild::MovePartition( BaseItemObject* OwnerObj )
{
    RoleDataEx*     Owner = OwnerObj->Role();

    if( Owner->SecRoomID() == -1 )
        return false;

    int SecX = Global::CalSecX( Owner->TempData.Move.Tx );
    int SecZ = Global::CalSecZ( Owner->TempData.Move.Tz );

    if( Owner->SecX() == SecX && Owner->SecZ() == SecZ )
        return false;

    //計算分割區中心位置與目前位置差距是否到 1 個Block的距離
    int TempX = Owner->SecX() * Global::ViewBlockSize() + Global::ViewBlockSize() / 2;
    int TempZ = Owner->SecZ() * Global::ViewBlockSize() + Global::ViewBlockSize() / 2;

    if(		abs( int(TempX - Owner->X()) ) < Global::ViewBlockSize() * 2 / 3
        &&	abs( int(TempZ - Owner->Z()) ) < Global::ViewBlockSize() * 2 / 3 )
        return false;


	//---------------------------------------------------------------------------------------------------
	//如果有物件跟隨則讓跟隨物件到此點
	vector< AttachObjInfoStruct >& AttachList = OwnerObj->AttachList();		
	for( unsigned i = 0 ; i < AttachList.size() ; i++ )
	{
		BaseItemObject* OtherObj = Global::GetObj( AttachList[i].ItemID );
		if( OtherObj == NULL )
			continue;
		RoleDataEx* Other = OtherObj->Role();

		Other->TempData.Move.Tx = Other->Pos()->X = Owner->X();
		Other->TempData.Move.Ty = Other->Pos()->Y = Owner->Y();
		Other->TempData.Move.Tz = Other->Pos()->Z = Owner->Z();
		
		MovePartition( OtherObj );
	}

	if( Owner->TempData.Sys.PartitionLock != false ) 
		return false;

    //通知新進入的
    PlayIDInfo**	Block = Global::PartCompSearch( Owner , SecX , SecZ , _Def_View_Block_Range_ );

    if( Block == NULL )        
        return false;
	
	Global::PartMove( Owner , SecX , SecZ );


    int			i , j;
    PlayID*		TopID;

    PlayerGraphStruct	OwnerGraph;
    PlayerGraphStruct	OtherGraph;


	CreatePlayerGraph( Owner, &OwnerGraph );
    //Owner->PlayerGraph( OwnerGraph );

    for( i = 0 ; Block[i] != NULL ; i++ )
    {
        TopID = Block[i]->Begin;
        switch( Block[i]->Type )
        {
        case 1://新進入的人
            for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
            {
                //送給鄒為的玩家
                BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
                if( OtherObj == NULL )
                    continue;
				if( OtherObj->Role()->BaseData.Mode.IsAllZoneVisible != false )
				{
					_SendEnterViewInfo( OtherObj , OwnerObj );
				}
				else if( OwnerObj->Role()->BaseData.Mode.IsAllZoneVisible != false )
				{
					_SendEnterViewInfo( OwnerObj , OtherObj );
				}
				//if( OtherObj->Role()->BaseData.Mode.IsAllZoneVisible == false  )
				else
				{
					_SendAddPartInfo( OwnerObj , OtherObj , &OwnerGraph );
				}
				//else if(  )
            }
            break;
        case 2://離開視野範圍
            for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
            {
                //送給鄒為的玩家
                BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
                if( OtherObj == NULL )
                    continue;
                

				if( OtherObj->Role()->BaseData.Mode.IsAllZoneVisible != false )
				{
					_SendLeaveViewInfo( OtherObj , OwnerObj );
				}
				else if( OwnerObj->Role()->BaseData.Mode.IsAllZoneVisible != false )
				{
					_SendLeaveViewInfo( OwnerObj , OtherObj );
				}
				//if( OtherObj->Role()->BaseData.Mode.IsAllZoneVisible == false  )
				else
				{
					_SendDelPartInfo( OwnerObj , OtherObj  );
				}

				//if( OtherObj->Role()->BaseData.Mode.IsAllZoneVisible == false  )
				//	_SendDelPartInfo( OwnerObj , OtherObj );
				/*
                if( OtherObj->Role()->IsPlayer() )
                {
                    DelObject( OtherObj->GUID() , OwnerObj->GUID()  );

					if( OwnerObj->Role()->IsSpell() )
						NetSrv_Magic::S_EndSpell( OtherObj->GUID() , OwnerObj->Role()->TempData.Magic.SerialID );

                    OwnerObj->Role()->TempData.AroundPlayer--;
                }

                if( OwnerObj->Role()->IsPlayer() )
                {
                    DelObject( OwnerObj->GUID() , OtherObj->GUID()  );

					if( OtherObj->Role()->IsSpell() )
						NetSrv_Magic::S_EndSpell( OwnerObj->GUID() , OtherObj->Role()->TempData.Magic.SerialID );

                    OtherObj->Role()->TempData.AroundPlayer--;
                }
				*/
            }
            break;
        }
    }

    return true;
}
//-------------------------------------------------------------------
//一般移動
bool    NetSrv_MoveChild::NormalMove( BaseItemObject* OwnerObj )
{
    RoleDataEx*		Owner	 = OwnerObj->Role();

    if( Owner->SecRoomID() == -1 )
        return false;

	if( Owner->IsNPC() )
		Owner->TempData.Move.MoveType  = EM_ClientMoveType_Normal; 

/*	if( Owner->IsPlayer() )
	{
		if( Owner->TempData.AroundPlayer > 80 )
		{
			int ModValue = ( Owner->TempData.AroundPlayer - 80 ) / 10 + 2;
			if( rand() % ModValue != 0 )
				return true;
		}		
	}
*/
    //---------------------------------------------------------------------------------------------------
    //檢查物件是否在移動狀態 並設定
    //---------------------------------------------------------------------------------------------------
    //檢查移動距離 是否有移動
	if( FloatEQU( Owner->TempData.Move.Dx + Owner->TempData.Move.Dz , 0 ) )
	{
		Owner->TempData.Move.LastMoveTime = RoleDataEx::G_SysTime - 1000;
	}
	else
	{
		Owner->TempData.Move.LastMoveTime = RoleDataEx::G_SysTime + 1000;
	}

    //---------------------------------------------------------------------------------------------------
	if( Owner->IsNPC() && Owner->BaseData.Mode.IsAllZoneVisible != false )
	{
		set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
		set< BaseItemObject*>::iterator   PlayerObjIter;
		//計算每個房間的人數
		for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
		{
			BaseItemObject* OtherObj = *PlayerObjIter;
			if( OtherObj == NULL )
				continue;
			RoleDataEx* Other = OtherObj->Role();
			if( Other->RoomID() != Owner->RoomID() )
				continue;

			Move( OtherObj->GUID() , Owner );
		}	
	}
	else
	{
		/*
		bool IsSendMovePG = false;
		if( Owner->IsPlayer() )
		{
			if( int( Owner->TempData.Move.Dx + Owner->TempData.Move.Dz ) == 0 )
			{
				if( RoleDataEx::G_SysTime - Owner->PlayerTempData->LastSendMovePG > 1000 )
				{
					IsSendMovePG = true;
					Owner->PlayerTempData->LastSendMovePG = RoleDataEx::G_SysTime;
				}
			}
		}
		*/

		//_Def_View_Block_Range_
		PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

		int			i , j;
		PlayID*		TopID;

		for( i = 0 ; Block[i] != NULL ; i++ )
		{
			TopID = Block[i]->Begin;

			for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
			{
				BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );			
				if( OtherObj == NULL  )
					continue;

				RoleDataEx* Other = OtherObj->Role();
				if( Other->IsPlayer() == false )
					continue;

				if( OtherObj->GUID() == OwnerObj->GUID() )
					continue;

				if( Owner->GUID() != OtherObj->GUID() && Owner->BaseData.Mode.GMHide != false )
					continue;

				if( Other->PlayerTempData->SendPGMode.IgnoreAllMove != false )
					continue;

				if( Owner->IsPlayer() )
				{
					if( Other->PlayerTempData->SendPGMode.IgnorePlayerMove )
						continue;
					if( Other->PlayerTempData->SendPGMode.ReducePlayerMove )
						continue;
				}
				//如果距離超過400
				if( Owner->TempData.AroundPlayer > 40 )
				{
					if(  Owner->Length( Other ) > 300 )
						continue;
				}

				if(	Owner->TempData.AroundPlayer > _MAX_MOVE_SENDALL_COUNT_ )
				{
					if(  Owner->Length( Other ) > 400 )
						continue;
				}

				//送給週圍的玩家
				Move( OtherObj->GUID() , Owner );

			}
		}
	}
    //-----------------------------------------------------------------------------------------------
	if( Owner->IsPlayer() )
	{
		CNetSrv_PartBase::MoveObj(	Owner->GUID() 
			, Owner->TempData.Move.Tx 
			, Owner->TempData.Move.Ty 
			, Owner->TempData.Move.Tz 
			, Owner->Dir() );
	}

	//////////////////////////////////////////////////////////////////////////
	vector< AttachObjInfoStruct >& AttachList = OwnerObj->AttachList();		
	for( unsigned i = 0 ; i < AttachList.size() ; i++ )
	{
		BaseItemObject* OtherObj = Global::GetObj( AttachList[i].ItemID );
		if( OtherObj == NULL )
			continue;
		RoleDataEx* Other = OtherObj->Role();

		Other->TempData.Move.BeginJumpX = Other->TempData.Move.Tx = Other->TempData.Move.CliX = Other->TempData.Move.LCheckLineX = Other->Pos()->X = Owner->X();
		Other->TempData.Move.BeginJumpY =Other->TempData.Move.Ty = Other->TempData.Move.CliY = Other->TempData.Move.LCheckLineY = Other->Pos()->Y = Owner->Y();
		Other->TempData.Move.BeginJumpZ =Other->TempData.Move.Tz = Other->TempData.Move.CliZ = Other->TempData.Move.LCheckLineZ = Other->Pos()->Z = Owner->Z();

		if( Other->IsPlayer() )
		{
			CNetSrv_PartBase::MoveObj(	Other->GUID() 
				, Other->TempData.Move.Tx 
				, Other->TempData.Move.Ty 
				, Other->TempData.Move.Tz 
				, Other->Dir() );
		}
	}
    //-----------------------------------------------------------------------------------------------
    return true;
}
//-------------------------------------------------------------------
//加入一個物件
bool	NetSrv_MoveChild::AddToPartition( BaseItemObject* OwnerObj  )
{
    BaseItemObject* OtherObj;

    RoleDataEx *Owner = OwnerObj->Role();

	if( Owner->SecRoomID() != -1 )
		return false;

	Owner->TempData.BackInfo.Mode = Owner->BaseData.Mode;
	Owner->TempData.BackInfo.HP = (int)Owner->BaseData.HP;
	Owner->TempData.BackInfo.MP = (int)Owner->BaseData.MP;
	Owner->TempData.BackInfo.SP = (int)Owner->BaseData.SP;
	Owner->TempData.BackInfo.MaxHP = (int)Owner->TempData.Attr.Fin.MaxHP;
	Owner->TempData.BackInfo.MaxMP = (int)Owner->TempData.Attr.Fin.MaxMP;
	Owner->TempData.BackInfo.MaxSP = (int)Owner->TempData.Attr.Fin.MaxSP;
	Owner->TempData.BackInfo.MoveSpeed = (int)Owner->TempData.Attr.Fin.MoveSpeed;
	Owner->TempData.Attr.Action.SrvSetPos = true;
	//-------------------------------------------------------------------
	//移動資料重設
	Owner->TempData.Move.CliX = Owner->Pos()->X;
	Owner->TempData.Move.CliY = Owner->Pos()->Y;
	Owner->TempData.Move.CliZ = Owner->Pos()->Z;
	Owner->TempData.Move.LCheckLineX = Owner->Pos()->X;
	Owner->TempData.Move.LCheckLineY = Owner->Pos()->Y;
	Owner->TempData.Move.LCheckLineZ = Owner->Pos()->Z;

	Owner->TempData.Move.CalMoveLen = 0;
	Owner->TempData.Move.CliLastMoveTime = RoleDataEx::G_SysTime;

	Owner->TempData.Move.BeginJumpX = Owner->Pos()->X;
	Owner->TempData.Move.BeginJumpY = Owner->Pos()->Y;
	Owner->TempData.Move.BeginJumpZ = Owner->Pos()->Z;
	//-------------------------------------------------------------------
	if( Owner->IsPlayer() )
		S_RoomID( Owner->GUID() , Owner->RoomID() );
	//-------------------------------------------------------------------
	Owner->TempData.Attr.LAction = Owner->TempData.Attr.Action;
	PlayerGraphStruct	OwnerGraph;
	//Owner->PlayerGraph( OwnerGraph );
	CreatePlayerGraph( Owner, &OwnerGraph );

	//-------------------------------------------------------------------
    if( Global::PartAdd( Owner ) == false )
    {
        Print( Def_PrintLV3 , "AddToPartition" , "ERROR AddToPartition!! SecX , SecY != -1 RoleName = %s" , Utf8ToChar( Owner->RoleName() ).c_str() );
        return false;
    }
    
    OwnerObj->Path()->SetPos( Owner->Pos()->X , Owner->Pos()->Y , Owner->Pos()->Z );

	//----------------------------------------------------------------
	//通知Partition Srv
	if( Owner->IsPlayer() )
	{
		CNetSrv_PartBase::AddObj( Owner->GUID() , (char*)Owner->RoleName() , Owner->PlayerTempData->ParallelZoneID , Owner->Pos()->X , Owner->Pos()->Y , Owner->Pos()->Z , Owner->Pos()->Dir );
	}
	//----------------------------------------------------------------
	Owner->OnAddToPartition();
	Owner->TempData.Sys.LGUID = -1;


	if( Owner->IsPlayer() == false )
	{
		if( Owner->BaseData.Mode.IsAllZoneVisible != false )
		{
			set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
			set< BaseItemObject*>::iterator   PlayerObjIter;
			//計算每個房間的人數
			for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
			{
				BaseItemObject* OtherObj = *PlayerObjIter;
				if( OtherObj == NULL )
					continue;
				RoleDataEx* Other = OtherObj->Role();
				if( Other->RoomID() != Owner->RoomID() )
					continue;
				
				_SendAddPartInfo( OwnerObj , OtherObj , &OwnerGraph );
			}
			return true;
		}
	}
	else
	{
		if( Owner->TempData.Attr.Action.SendAllZoneObj )
		{
			Owner->TempData.Attr.Action.SendAllZoneObj = false;
			set<BaseItemObject* >&	NpcObjSet = *(BaseItemObject::NPCObjSet());
			set< BaseItemObject*>::iterator   NpcObjIter;
			for( NpcObjIter = NpcObjSet.begin() ; NpcObjIter != NpcObjSet.end() ; NpcObjIter++ )
			{
				BaseItemObject* OtherObj = *NpcObjIter;
				if( OtherObj == NULL )
					continue;
				RoleDataEx* Other = OtherObj->Role();
				if( Other->RoomID() != Owner->RoomID() )
					continue;
				if( Other->BaseData.Mode.IsAllZoneVisible == false )
					continue;

				_SendAddPartInfo( OwnerObj , OtherObj , &OwnerGraph );
			}
		}
	}

    PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

    int			i , j;
    PlayID*		TopID;

    for( i = 0 ; Block[i] != NULL ; i++ )
    {
        TopID = Block[i]->Begin;

        for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
        {
            OtherObj = BaseItemObject::GetObj( TopID->ID );
			

            if( OtherObj == NULL || OtherObj == OwnerObj )
                continue; 

			RoleDataEx* Other = OtherObj->Role();

			if(  Other->BaseData.Mode.IsAllZoneVisible != false )
			{
				_SendEnterViewInfo( OtherObj , OwnerObj );
				continue;
			}
			
            _SendAddPartInfo( OwnerObj , OtherObj , &OwnerGraph );

        }
    }

	//把自己的資料最後送
	_SendAddPartInfo( OwnerObj , OwnerObj , &OwnerGraph );

 

    //----------------------------------------------------------------
    return true;
}
//-------------------------------------------------------------------
//刪除一個物件
bool	NetSrv_MoveChild::DelFromPartition( BaseItemObject* OwnerObj )
{
    RoleDataEx *Owner = OwnerObj->Role();
    if( Owner == NULL || Owner->SecRoomID() == -1 )
        return false;
    //----------------------------------------------------------------
    //通知Partition Srv
	if( Owner->IsPlayer() )
	{
		CNetSrv_PartBase::DelObj( Owner->GUID() );
	}
    //----------------------------------------------------------------
	if( Owner->IsPlayer() == false )
	{
		if( Owner->BaseData.Mode.IsAllZoneVisible != false )
		{
			set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
			set< BaseItemObject*>::iterator   PlayerObjIter;
			//計算每個房間的人數
			for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
			{
				BaseItemObject* OtherObj = *PlayerObjIter;
				if( OtherObj == NULL )
					continue;
				RoleDataEx* Other = OtherObj->Role();
				if( Other->RoomID() != Owner->RoomID() )
					continue;

				_SendDelPartInfo( OwnerObj , OtherObj );
			}	

			Global::PartDel( Owner );

			//設定最後顯像位置
			Owner->TempData.BackInfo.LZoneID = RoleDataEx::G_ZoneID;
			Owner->TempData.BackInfo.LX		= Owner->Pos()->X;
			Owner->TempData.BackInfo.LZ		= Owner->Pos()->Z;

			return true;
		}
	}
    //_Def_View_Block_Range_
	PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

	int			i , j;
	PlayID*		TopID;

	for( i = 0 ; Block[i] != NULL ; i++ )
	{
		TopID = Block[i]->Begin;

		for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
		{
			BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
			if( OtherObj == NULL )
				continue; 

			if( OtherObj->Role()->BaseData.Mode.IsAllZoneVisible )
				continue;

			_SendDelPartInfo( OwnerObj , OtherObj );
		}
	}
	

    Global::PartDel( Owner );

	//設定最後顯像位置
	Owner->TempData.BackInfo.LZoneID = RoleDataEx::G_ZoneID;
	Owner->TempData.BackInfo.LX		= Owner->Pos()->X;
	Owner->TempData.BackInfo.LZ		= Owner->Pos()->Z;

    return true;
}
//-------------------------------------------------------------------
void NetSrv_MoveChild::_SendDelPartInfo( BaseItemObject* OwnerObj , BaseItemObject* OtherObj )
{
	
	RoleDataEx* Owner = OwnerObj->Role();
	RoleDataEx* Other = OtherObj->Role();

//	if( Owner->IsDestroy() != false || Other->IsDestroy() )
//		return;

	//if( OtherObj->Role()->IsPlayer() && !OtherObj->Role()->IsMirror() )
	if( Other->IsPlayer() && ( Owner->IsPlayer() == false || Owner->IsMirror() == false ) ) 
	{
		if( Other == Owner || Owner->BaseData.Mode.GMHide == false )
		{
			//通知角色刪除
			DelObject( OtherObj->GUID() , OwnerObj->GUID() , OwnerObj->Role()->WorldGUID() );
			if( OwnerObj->Role()->IsSpell() )
				NetSrv_Magic::S_EndSpell( OtherObj->GUID() , OwnerObj->Role()->TempData.Magic.SerialID );

			if(		OwnerObj->Role()->BaseData.IsShowTitle != false 
				&&	OwnerObj->Role()->BaseData.TitleID == 0 
				&&  OwnerObj->Role()->PlayerBaseData->TitleStr.Size() != 0 )
			{

			}
		}
		OwnerObj->Role()->TempData.AroundPlayer--;
	}

	if(     OtherObj->GUID() != OwnerObj->GUID() 
		&&  Owner->IsPlayer() && ( Other->IsPlayer() == false || Other->IsMirror() == false ) )
	{
		if( Other->BaseData.Mode.GMHide == false )
		{
			DelObject( OwnerObj->GUID() , OtherObj->GUID() , OtherObj->Role()->WorldGUID() );
			if( OtherObj->Role()->IsSpell() )
				NetSrv_Magic::S_EndSpell( OwnerObj->GUID() , OtherObj->Role()->TempData.Magic.SerialID );
		}
		/*
		DelObject( OwnerObj->GUID() , OtherObj->GUID() , OtherObj->Role()->WorldGUID() );
		if( OtherObj->Role()->IsSpell() )
			NetSrv_Magic::S_EndSpell( OwnerObj->GUID() , OtherObj->Role()->TempData.Magic.SerialID );
*/

		OtherObj->Role()->TempData.AroundPlayer--;
	}



}
//-------------------------------------------------------------------
void NetSrv_MoveChild::_SendEnterViewInfo( BaseItemObject* AllZoneObj , BaseItemObject* OwnerObj )
{
	RoleDataEx* Owner = OwnerObj->Role();
	RoleDataEx* Other = AllZoneObj->Role();

	if( Owner->IsPlayer() == false )
		return;

	if( !Other->BaseData.Look.IsEmpty() )
		LookFace( Owner->GUID() , Other );


	LookEQ( Owner->GUID() , Other );
	if( Other->TempData.CliActionID != 0 )
		DoAction( Owner->GUID() , Other->GUID() , Other->TempData.CliActionID );

	if( Other->TempData.C_AutoPlot.Size() != 0 )
	{
		SendCliAuto( Owner->GUID() , Other->GUID() , Other->TempData.C_AutoPlot.Begin() );
	}
	NetSrv_MagicChild::S_AddBuffInfo_List( Owner->GUID() , Other );


	if( Other->IsSpell() != false )
	{
		MagicProcInfo&	MagicProc = Other->TempData.Magic;
		NetSrv_MagicChild::S_BeginSpell( Owner->GUID() , Other->GUID() , MagicProc.TargetID , MagicProc.TargetX , MagicProc.TargetY , MagicProc.TargetZ , MagicProc.MagicCollectID , MagicProc.SerialID , int( MagicProc.SpellTime ) );
	}

	if(	 Other->BaseData.IsShowTitle != false &&  Other->BaseData.TitleID == -1 && Other->PlayerBaseData->TitleStr.Size() != 0  )
	{
		S_SetTitleID( Owner->GUID() , Other->GUID() , Other->BaseData.IsShowTitle , Other->BaseData.TitleID , Other->PlayerBaseData->TitleStr.Begin() );
	}

//	Other->TempData.AroundPlayer++;

	_Send_Relation( Owner , Other );

}
void NetSrv_MoveChild::_SendLeaveViewInfo( BaseItemObject* AllZoneObj , BaseItemObject* OwnerObj )
{
	RoleDataEx* Owner = OwnerObj->Role();
	RoleDataEx* Other = AllZoneObj->Role();

	if( Owner->IsPlayer() == false )
		return;

	if( Other->IsSpell() )
		NetSrv_Magic::S_EndSpell( Owner->GUID() , Other->TempData.Magic.SerialID );

	NetSrv_Magic::S_DelBuffInfo( Owner->GUID() , Other->GUID() , -1 , -1 );
//	Other->TempData.AroundPlayer--;

}

void NetSrv_MoveChild::_SendAddPartInfo( BaseItemObject* OwnerObj , BaseItemObject* OtherObj , PlayerGraphStruct*	OwnerGraph )
{
    RoleDataEx* Owner = OwnerObj->Role();
    RoleDataEx* Other = OtherObj->Role();
    PlayerGraphStruct	OtherGraph;

    if( Other->IsPlayer() && ( Owner->IsPlayer() == false || Owner->IsMirror() == false ) ) //&& !Other->IsMirror() )
    {
		if( Other == Owner || Owner->BaseData.Mode.GMHide == false )
		{
			AddObject( OtherObj->GUID() , OwnerObj->GUID() , Owner->WorldGUID() , Owner->DBID() , Owner->OwnerGUID() , Owner->TempData.AI.CampID , Owner->TempData.iWorldID, Owner->BaseData.Pos ,  *OwnerGraph );			
			AttachObjProc( OtherObj->GUID() , OwnerObj );
			if( Owner->TempData.PotInfo.IsEmpty() == false )
				FlowerPotInfo( Other->GUID() , Owner->GUID() , Owner->TempData.PotInfo );


			if( !Owner->BaseData.Look.IsEmpty() )
			{
				LookFace( OtherObj->GUID() , Owner );
			}

				
			LookEQ( OtherObj->GUID() , Owner );

			if( Owner->TempData.CliActionID != 0 )
				DoAction( OtherObj->GUID() , Owner->GUID() , Owner->TempData.CliActionID );

			if( Owner->TempData.C_AutoPlot.Size() != 0 )
			{
				SendCliAuto( OtherObj->GUID() , Owner->GUID() , Owner->TempData.C_AutoPlot.Begin() );
			}

			if( Owner->BaseData.Mode.IsAllZoneVisible == false  )
				NetSrv_MagicChild::S_AddBuffInfo_List( Other->GUID() , Owner );

			if( Owner->IsSpell() != false )
			{
				MagicProcInfo&	MagicProc = Owner->TempData.Magic;
				NetSrv_MagicChild::S_BeginSpell( Other->GUID() , Owner->GUID() , MagicProc.TargetID , MagicProc.TargetX , MagicProc.TargetY , MagicProc.TargetZ , MagicProc.MagicCollectID , MagicProc.SerialID , int( MagicProc.SpellTime ) );
			}

			if(	 Owner->BaseData.IsShowTitle != false &&  Owner->BaseData.TitleID == -1 && Owner->PlayerBaseData->TitleStr.Size() != 0  )
			{
				S_SetTitleID( Other->GUID() , Owner->GUID() , Owner->BaseData.IsShowTitle , Owner->BaseData.TitleID , Owner->PlayerBaseData->TitleStr.Begin() );
			}
			_Send_Relation( Other , Owner );

			if(		Owner->TempData.Attr.Effect.Ride 
				&&	Owner->BaseData.HorseColor[0] + Owner->BaseData.HorseColor[1] + Owner->BaseData.HorseColor[2] +Owner->BaseData.HorseColor[3] != 0 )
			{
				S_HorseColor( Other->GUID() , Owner->GUID() , Owner->BaseData.HorseColor );
			}
			if( Owner->TempData.ModelRate != 1.0f )
			{
				SendModelRate( Other->GUID() , Owner );
			}
			_Send_GuildBuildingInfo( Other , Owner );
		}		
        Owner->TempData.AroundPlayer++;
    }

    if(     OtherObj->GUID() != OwnerObj->GUID() 
			&& Owner->IsPlayer() && ( Other->IsPlayer() == false || Other->IsMirror() == false ) )
     //   &&  Owner->IsPlayer() && !Owner->IsMirror()  )
    {
		if( Other->BaseData.Mode.GMHide == false )
		{
			//Other->PlayerGraph( OtherGraph );
			CreatePlayerGraph( Other, &OtherGraph );
			
			AddObject( OwnerObj->GUID() , OtherObj->GUID() , Other->WorldGUID() , Other->DBID() , Other->OwnerGUID() , Other->TempData.AI.CampID , Other->TempData.iWorldID, Other->BaseData.Pos , OtherGraph );
			AttachObjProc( OwnerObj->GUID() , OtherObj );
			if( Other->TempData.PotInfo.IsEmpty() == false )
				FlowerPotInfo( Owner->GUID() , Other->GUID() , Other->TempData.PotInfo );

			if( !Other->BaseData.Look.IsEmpty() )
			{
				LookFace( OwnerObj->GUID() , Other );
			}

			LookEQ( OwnerObj->GUID() , OtherObj->Role() );

			if( Other->TempData.CliActionID != 0 )
				DoAction( Owner->GUID() , Other->GUID() , Other->TempData.CliActionID );

			if( Other->TempData.C_AutoPlot.Size() != 0 )
			{
				SendCliAuto( Owner->GUID() , Other->GUID() , Other->TempData.C_AutoPlot.Begin() );
			}
			if( Other->BaseData.Mode.IsAllZoneVisible == false  )
				NetSrv_MagicChild::S_AddBuffInfo_List( Owner->GUID() , Other );

			if( Other->IsSpell() != false )
			{
				MagicProcInfo&	MagicProc = Other->TempData.Magic;
				NetSrv_MagicChild::S_BeginSpell( Owner->GUID() , Other->GUID() , MagicProc.TargetID , MagicProc.TargetX , MagicProc.TargetY , MagicProc.TargetZ , MagicProc.MagicCollectID , MagicProc.SerialID , int( MagicProc.SpellTime ) );
			}

			if(	 Other->BaseData.IsShowTitle != false &&  Other->BaseData.TitleID == -1 && Other->PlayerBaseData->TitleStr.Size() != 0  )
			{
				S_SetTitleID( Owner->GUID() , Other->GUID() , Other->BaseData.IsShowTitle , Other->BaseData.TitleID , Other->PlayerBaseData->TitleStr.Begin() );
			}

			Other->TempData.AroundPlayer++;

			_Send_Relation( Owner , Other );

			if(		Other->TempData.Attr.Effect.Ride 
				&&	Other->BaseData.HorseColor[0] + Other->BaseData.HorseColor[1] + Other->BaseData.HorseColor[2] +Other->BaseData.HorseColor[3] != 0 )
			{
				S_HorseColor( Owner->GUID() , Other->GUID() , Other->BaseData.HorseColor );
			}

			if( Other->TempData.ModelRate != 1.0f )
			{
				SendModelRate( Owner->GUID() , Other );
			}

			_Send_GuildBuildingInfo( Owner , Other );
		}
        
    }
	
 //   Send_Relation( Owner , Other );
}

//通知週圍的人裝備資訊
void NetSrv_MoveChild::Send_LookEq( RoleDataEx* Owner )
{
    PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

    int			i , j;
    PlayID*		TopID;

    for( i = 0 ; Block[i] != NULL ; i++ )
    {
        TopID = Block[i]->Begin;

        for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
        {
            BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
            if( OtherObj == NULL )
                continue;
            if( OtherObj->Role()->IsNPC() != false )
                continue;

            LookEQ( OtherObj->GUID() , Owner );
        }
    }   
}

void NetSrv_MoveChild::R_DoAction( BaseItemObject* Sender , int GItemID , int ActionID , int TempAction )
{
    RoleDataEx* Owner = Sender->Role();

    if( Owner->GUID() != GItemID )
        return;

	if( Owner->IsNPC() && Owner->BaseData.Mode.IsAllZoneVisible != false )
	{
		set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
		set< BaseItemObject*>::iterator   PlayerObjIter;
		//計算每個房間的人數
		for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
		{
			BaseItemObject* OtherObj = *PlayerObjIter;
			if( OtherObj == NULL )
				continue;
			RoleDataEx* Other = OtherObj->Role();
			if( Other->RoomID() != Owner->RoomID() )
				continue;

			DoAction( OtherObj->GUID() , GItemID , ActionID , TempAction );
		}	
	}
	else
	{
		PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

		int			i , j;
		PlayID*		TopID;

		for( i = 0 ; Block[i] != NULL ; i++ )
		{
			TopID = Block[i]->Begin;

			for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
			{
				BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
				if( OtherObj == NULL || OtherObj->GUID() == Sender->GUID() )
					continue;
				if( OtherObj->Role()->IsNPC() != false )
					continue;

				DoAction( OtherObj->GUID() , GItemID , ActionID , TempAction );
			}
		}   
	}

    Owner->TempData.CliActionID = ActionID;

	RoleSpriteStatus Temp;
	Temp.value = ActionID;

	Owner->TempData.Attr.Action.Sit = Temp.isSit;
	Owner->TempData.Attr.Action.Unholster = Temp.isUnholster;
	//Owner->TempData.Attr.Action.Walk = Temp.isWalk;
	Owner->IsWalk( Temp.isWalk );
}

//通知週圍的人裝備資訊
void NetSrv_MoveChild::SendRange_ActionType( RoleDataEx* Owner )
{

    if( Owner->TempData.Attr.LAction._HiAction == Owner->TempData.Attr.Action._HiAction )
        return;

    Owner->TempData.Attr.LAction = Owner->TempData.Attr.Action;

    RoleDataEx* Other;
    Global::ResetRange( Owner , _Def_View_Block_Range_ );
    while( 1 ) 
    {
        Other = Global::GetRangePlayer();
        if( Other == NULL )
            break;

        SendActionType( Other->GUID() , Owner->GUID() , Owner->TempData.Attr.Action  );
    }
}

void NetSrv_MoveChild::R_JumpBegin( BaseItemObject* Sender , float X , float Y , float Z , float Dir 
                         , float vX , float vY , float vZ )
{

    RoleDataEx* Owner = Sender->Role();

	if( Owner->TempData.Move.IsLockSetPos != false )
		return;
/*
	//--------------------------------------------------------------------------------------------------------------
	//距離總計(檢查距離是否有問題)
	float Dx = Owner->TempData.Move.CliX - X;
	float Dz = Owner->TempData.Move.CliZ - Z;
	float MoveLen = sqrt( Dx * Dx + Dz * Dz );
	Owner->TempData.Move.CalMoveLen += MoveLen;

	Owner->TempData.Move.CliX = X;
	Owner->TempData.Move.CliY = Y;
	Owner->TempData.Move.CliZ = Z;
	*/
/*
	Owner->TempData.Move.BeginJumpX = X;
	Owner->TempData.Move.BeginJumpY = Y;
	Owner->TempData.Move.BeginJumpZ = Z;
	*/
	//--------------------------------------------------------------------------------------------------------------

	Owner->TempData.BackInfo.BuffClearTime.Move = true;
	Owner->TempData.Magic.IsMove =true;

    RoleDataEx* Other;
    Global::ResetRange( Owner , _Def_View_Block_Range_ );
    while( 1 ) 
    {
        Other = Global::GetRangePlayer();
        if( Other == NULL )
            break;
		if( Other->GUID() == Owner->GUID() )
			continue;

        S_JumpBegin( Other->GUID() , Owner->GUID() , X , Y , Z , Dir , vX , vY , vZ);
    }

}

void NetSrv_MoveChild::R_JumpEnd( BaseItemObject* Sender , float X , float Y , float Z , float Dir , int AttackObjID )
{
    RoleDataEx* Owner = Sender->Role();

	if( Owner->TempData.Move.IsLockSetPos != false )
		return;
	/*
	//--------------------------------------------------------------------------------------------------------------
	//距離總計(檢查距離是否有問題)
	float Dx = Owner->TempData.Move.CliX - X;
	float Dz = Owner->TempData.Move.CliZ - Z;
	float MoveLen = sqrt( Dx * Dx + Dz * Dz );
	Owner->TempData.Move.CalMoveLen += MoveLen;
	Owner->TempData.Move.CliX = X;
	Owner->TempData.Move.CliY = Y;
	Owner->TempData.Move.CliZ = Z;
	*/

    RoleDataEx* Other;
    Global::ResetRange( Owner , _Def_View_Block_Range_ );
    while( 1 ) 
    {
        Other = Global::GetRangePlayer();
        if( Other == NULL )
            break;

		if( Other->GUID() == Owner->GUID() )
			continue;

        S_JumpEnd( Other->GUID() , Owner->GUID() , X , Y , Z , Dir );
    }

	/*
	float Dy = Owner->TempData.Move.BeginJumpY - Y -50;
	if( Dy > 0 )
	{
		if( Dy > 100 )
			Dy = 100;

		int Damage = Dy * Owner->TempData.Attr.Fin.MaxHP / 100;
		Owner->AddHP( Owner , Damage * -1 );
		S_PlayerDropDownDamage( Owner->GUID () , Damage , 0 );
	}
	*/
}

//設定移動速度
void	NetSrv_MoveChild::SendRange_MoveInfo( RoleDataEx* Owner )
{
	Owner->TempData.BackInfo.JumpRate =  Owner->TempData.Attr.Mid.Body[EM_WearEqType_JumpRate];
	Owner->TempData.BackInfo.Gravity =  Owner->TempData.Attr.Mid.Body[EM_WearEqType_Gravity];

    RoleDataEx* Other;

	if( Owner->IsNPC() && Owner->BaseData.Mode.IsAllZoneVisible != false )
	{
		set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
		set< BaseItemObject*>::iterator   PlayerObjIter;
		//計算每個房間的人數
		for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
		{
			BaseItemObject* OtherObj = *PlayerObjIter;
			if( OtherObj == NULL )
				continue;

			Other = OtherObj->Role();
			if( Other->RoomID() != Owner->RoomID() )
				continue;

			SendMoveInfo( Other->GUID() , Owner );
		}	
	}
	else
	{
		Global::ResetRange( Owner , _Def_View_Block_Range_ );
		while( 1 ) 
		{
			Other = Global::GetRangePlayer();
			if( Other == NULL )
				break;

			SendMoveInfo( Other->GUID() , Owner );
		}
	}
}
//設定移動速度
void	NetSrv_MoveChild::SendRange_MoveSpeed( RoleDataEx* Owner )
{
	Owner->TempData.BackInfo.MoveSpeed = int( Owner->TempData.Attr.Fin.MoveSpeed );
    RoleDataEx* Other;

	if( Owner->IsNPC() && Owner->BaseData.Mode.IsAllZoneVisible != false )
	{
		set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
		set< BaseItemObject*>::iterator   PlayerObjIter;
		//計算每個房間的人數
		for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
		{
			BaseItemObject* OtherObj = *PlayerObjIter;
			if( OtherObj == NULL )
				continue;

			Other = OtherObj->Role();
			if( Other->RoomID() != Owner->RoomID() )
				continue;

			SendMoveSpeed( Other->GUID() , Owner );
		}	
	}
	else
	{
		Global::ResetRange( Owner , _Def_View_Block_Range_ );
		while( 1 ) 
		{
			Other = Global::GetRangePlayer();
			if( Other == NULL )
				break;

			SendMoveSpeed( Other->GUID() , Owner );
		}
	}
}


void NetSrv_MoveChild::_Send_Relation( RoleDataEx* SendObj , RoleDataEx* Target )
{
	if( SendObj->IsPlayer() == false )
		return;

	RoleRelationStruct Relation;

	if( Target->TempData.TouchPlot_Range != 0 )
	{
		Relation.TouckPlot = true;

		// 若是該物件有指定鎖定者, 則檢查發送者是否為鎖定者
		if( Target->PlayerTempData->ScriptInfo.emLockType != ENUM_SCRIPTSTATUS_LOCKTYPE_NOLOCK )
		{
			if( Target->PlayerTempData->ScriptInfo.emLockType == ENUM_SCRIPTSTATUS_LOCKTYPE_PERSONAL )
			{
				if( SendObj->DBID() != Target->PlayerTempData->ScriptInfo.iLockDBID )
				{
					Relation.TouckPlot = false;
				}
			}
		}
	}

	if( SendObj->CheckAttackable_IgnoreDead( Target ) != false )
		Relation.CanAttack = true;

	//檢查是否會攻擊玩家
	if(		Target->IsNPC() 
		&&  Target->BaseData.Mode.Searchenemy == false )
	{
		Relation.Enemy =  false;
	}
	else
	{
		Relation.Enemy = SendObj->CheckEnemy( Target );
	}



	// 檢查彼此的陣營關希, 若物件本身為任務物件, 對像為人, 則設置不能點擊
	if( Target->IsQuestNPC() != false )
	{
		GameObjDbStructEx* pObj = Global::GetObjDB( Target->BaseData.ItemInfo.OrgObjID );		
		if( pObj != NULL )
		{
			if( pObj->NPC.iQuestMode == 2 )	// 戰場物件的特殊規則
			{
				if( RoleDataEx::Camp.IsEnemy( SendObj->TempData.AI.CampID, Target->TempData.AI.CampID ) != false )
				{
					// 設定可點擊
					Relation.BGTouch	= true;
				}
				else
				{
					// 設定不可點擊
					Relation.BGTouch	= false;
				}
			}
		}

	}

	// 送出封包
	if( !Target->IsMirror() )
	{
		S_Relation( SendObj->GUID() , Target->GUID() , Relation );
		NetSrv_TreasureChild::Send_TreasureLootInfo( SendObj , Target );
	}
	
}


//送兩人的關係
void NetSrv_MoveChild::Send_Relation( RoleDataEx* Owner , RoleDataEx* Target )
{
	_Send_Relation( Owner , Target );
	_Send_Relation( Target , Owner );

	/*
	if(		Owner->TempData.Attr.Effect.Ride 
		&&	Owner->BaseData.HorseColor[0] + Owner->BaseData.HorseColor[1] + Owner->BaseData.HorseColor[2] +Owner->BaseData.HorseColor[3] != 0 )
	{
		S_HorseColor( Other->GUID() , Owner->GUID() , Owner->BaseData.HorseColor );
	}
	if(		Other->TempData.Attr.Effect.Ride 
		&&	Other->BaseData.HorseColor[0] + Other->BaseData.HorseColor[1] + Other->BaseData.HorseColor[2] +Other->BaseData.HorseColor[3] != 0 )
	{
		S_HorseColor( Owner->GUID() , Other->GUID() , Other->BaseData.HorseColor );
	}
	*/
	/*
	if( Owner->IsPlayer() )
	{
		RoleRelationStruct Relation;

		if( Target->TempData.TouchPlot.Size() != 0 )
			Relation.TouckPlot = true;

		if( Owner->CheckAttackable( Target ) != false )
			Relation.CanAttack = true;

		S_Relation( Owner->GUID() , Target->GUID() , Relation );
	}

	if( Target->IsPlayer() )
	{
		RoleRelationStruct Relation;

		if( Owner->TempData.TouchPlot.Size() != 0 )
			Relation.TouckPlot = true;

		if( Target->CheckAttackable( Owner ) != false )
			Relation.CanAttack = true;

		S_Relation( Target->GUID() , Owner->GUID() , Relation );
	}
	*/
}

//送周圍的關係
void NetSrv_MoveChild::SendRange_Relation( RoleDataEx* Owner )
{
    RoleDataEx* Other;
    Global::ResetRange( Owner , _Def_View_Block_Range_ );
    while( 1 ) 
    {
        Other = Global::GetRangePlayer();
        if( Other == NULL )
            break;	

        Send_Relation( Owner , Other );
    }
}

//通知所有Party成員自己的位置
void NetSrv_MoveChild::SendAllPartyMemberPos( RoleDataEx* Owner )
{
	//成員檢查
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( Owner->BaseData.PartyID );
	if( Party == NULL )
		return;
	
	for( int i = 0 ; i < (int)Party->Member.size() ; i++ )
	{
		if( Owner->DBID() == Party->Member[i].DBID )
			continue;

		S_PartyMemberPos( Party->Member[i].DBID , Owner->DBID() , Owner->GUID() , Owner->Pos()->X  , Owner->Pos()->Y , Owner->Pos()->Z );
	}
}

//物件一動
void NetSrv_MoveChild::SendRange_SpecialMove( int TargetID , SpecialMoveTypeENUM MoveType , RoleDataEx* Owner , int MagicCollectID )
{
	Owner->TempData.Move.PacketErrorCount = 0;

	RoleDataEx* Other;
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;	

		SpecialMove_BySockID( Other->TempData.Sys.SockID , Other->TempData.Sys.ProxyID , TargetID , MoveType , Owner , MagicCollectID );
	}
}

//通知某一玩家Model的縮放比例
void NetSrv_MoveChild::SendRange_ModelRate( int SendToID  )
{
	RoleDataEx* Owner = Global::GetRoleDataByGUID( SendToID );
	if( Owner == NULL )
		return;

	RoleDataEx* Other;
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;	

		SendModelRate( Other->GUID() , Owner );
	}
}

//Client 端環境載入完成，把角色加入分割區內
void NetSrv_MoveChild::R_ClientLoadingOK( BaseItemObject* Sender )
{
	RoleDataEx* Owner = Sender->Role();

/*	//清除client Buf
	NetSrv_Magic::S_DelBuffInfo( Owner->GUID() , Owner->GUID() , -1 , -1 );
	for( int i = 0 ; i < Owner->BaseData.Buff.Size() ; i++ )
	{
		BuffBaseStruct& Buf = Owner->BaseData.Buff[i];
		NetSrv_Magic::S_AddBuffInfo( Owner->GUID() , Owner->GUID() , Buf.OwnerID ,Buf.BuffID , Buf.Power ,Buf.Time );
	}
	*/

	// 戰場進入檢查
	if( CNetSrv_BattleGround_Child::GetBattleGroundType() != 0 )
	{
		((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->OnEvent_ClientLoaded( Sender );
	}


	Global::AddToPartition( Sender->GUID() , -1 );

	if( Sender->Role()->PlayerBaseData->DeadCountDown != 0 )
	{
		Sender->Role()->IsDead( true );
		NetSrv_AttackChild::SendRangeDead( Sender->Role() , -1 , -1 , Sender->Role()->PlayerBaseData->DeadCountDown , 0 );
	}

	LUA_VMClass::PCallByStrArg( "event_module(5)" , Sender->GUID() ,Sender->GUID() );
}

void NetSrv_MoveChild::SetPosture( BaseItemObject* Sender , int GItemID , int ActionID , int TempAction )
{
	This->R_DoAction( Sender, GItemID, ActionID, TempAction );
}

//(周圍) 角色特效通知
void NetSrv_MoveChild::SendRangeSpecialEfficacy ( int GItemID , SpecialEfficacyTypeENUM EffectType )
{
	RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
	if( Owner == NULL )
		return;

	RoleDataEx* Other;
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;

		S_SpecialEfficacy( Other->GUID() , Owner->GUID() , EffectType );
	}
}

//設定面向
void NetSrv_MoveChild::SendRangeSetDir( int GItemID , float Dir )
{
	RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
	if( Owner == NULL )
		return;

	RoleDataEx* Other;
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;

		S_SetDir( Other->GUID() , Owner->GUID() , Dir );
	}
}

//設定公會
void NetSrv_MoveChild::SendRangeSetGuildID( RoleDataEx* Owner )
{
	int GuildID = Owner->BaseData.GuildID;
	Owner->TempData.BackInfo.GuildID = GuildID;

	RoleDataEx* Other;
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;

		S_SetGuildID( Other->GUID() , Owner->GUID() , GuildID );
	}
}

//設定面向
void NetSrv_MoveChild::SendRangeSetTitleID( RoleDataEx* Owner )
{
	int		TitleID = Owner->BaseData.TitleID;
	bool	IsShowTitle = Owner->BaseData.IsShowTitle;
	Owner->TempData.BackInfo.TitleID = TitleID;
	Owner->TempData.BackInfo.IsShowTitle = IsShowTitle;

	RoleDataEx* Other;
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;

		S_SetTitleID( Other->GUID() , Owner->GUID() , IsShowTitle , TitleID , Owner->PlayerBaseData->TitleStr.Begin() );
	}
}

//設定長像
void NetSrv_MoveChild::SendRangeLookFace( RoleDataEx* Owner )
{
	RoleDataEx* Other;
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;
		LookFace( Other->GUID() , Owner );
	}
}

//設定長像
void NetSrv_MoveChild::SendRangeHorseColor( RoleDataEx* Owner )
{
	RoleDataEx* Other;
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;
		S_HorseColor( Other->GUID() , Owner->GUID() , Owner->BaseData.HorseColor );
	}
}

void NetSrv_MoveChild::_Send_GuildBuildingInfo( RoleDataEx* Owner , RoleDataEx* Target )
{
	if( Target->IsNPC() == false && Target->IsItem() == false )
		return;

	if( Target->TempData.GuildBuildingDBID <= 0 || Target->GuildID() <=0 )
		return;

	GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( Target->GuildID() );
	if( houseClass == NULL )
		return;

	switch( Target->TempData.GuildBuildingType )
	{
	case EM_GuildBuildingType_None:
		return;
	case EM_GuildBuildingType_Building:
		{
			GuildHouseBuildingStruct* BuildingInfo = houseClass->GetBuildingInfo( Target->TempData.GuildBuildingDBID );

			if( BuildingInfo == NULL )
				return;

			S_ObjGuildBuildingInfo( Owner->GUID()  , Target->GUID()
				, BuildingInfo->Info.BuildingDBID , BuildingInfo->Info.ParentDBID , BuildingInfo->Info.BuildingID , int( BuildingInfo->Info.Dir )
				, BuildingInfo->Info.PointStr );
		}
		break;
	case EM_GuildBuildingType_Furniture:
		{
			S_ObjGuildFurnitureInfo( Owner->GUID()  , Target->GUID() , Target->TempData.GuildFurnitureDBID );
		}
		break;
	}


}


void NetSrv_MoveChild::R_Ping( BaseItemObject* Sender , int Time )
{
	RoleDataEx* Owner = Sender->Role();

	if( IsDebuggerPresent() )
		Owner->TempData.PingCount = 0;

	Owner->TempData.PingCount++;
	if( Owner->TempData.PingCount >= 10 )
		Owner->Destroy( "PingCount >= 10" );
	
}

void NetSrv_MoveChild::CreatePlayerGraph( RoleDataEx* pRole, PlayerGraphStruct* pGraph )
{
	pRole->PlayerGraph( *pGraph );

	// 發送戰場相關資訊給 Client
	if( ( CNetSrv_BattleGround_Child::GetBattleGroundType() != 0 || Ini()->IsGuildHouseWarZone != false ) && pRole->IsPlayer() != false )
	{
		pGraph->iForceFlagID = ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->GetPlayerFlagID( pRole );
	}
	else
	{
		pGraph->iForceFlagID = 0;
	}
}

void NetSrv_MoveChild::R_PingLog( BaseItemObject* Player , int ID , int Time , int ClietNetProcTime )
{
	char	Buf[512];
	sprintf( Buf , "INSERT ClientPingLog (ID, Ping, ClientProcTime, PlayerDBID, ZoneID, WorldID) VALUES(%d,%d,%d,%d,%d,%d)" 
		, ID , timeGetTime() - Time , ClietNetProcTime , Player->Role()->DBID() , RoleDataEx::G_ZoneID , _Net->GetWorldID() );

	Net_DCBase::LogSqlCommand( Buf );
}

void NetSrv_MoveChild::SendRangeAttackObj( RoleDataEx* Owner , int AttachItemID , int AttachType , const char* ItemPos , const char* AttachPos , int Pos  )
{
	RoleDataEx* Other;
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;
		S_AttachObj( Other->GUID() , Owner->GUID() , AttachItemID , AttachType , ItemPos , AttachPos , Pos  );
	}
}
void NetSrv_MoveChild::SendRangeDetachObj( RoleDataEx* Owner )
{
	RoleDataEx* Other;
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;
		S_DetachObj( Other->GUID() , Owner->GUID() );
	}
}

void NetSrv_MoveChild::AttachObjProc( int SendID , BaseItemObject* WagonObj )
{
	vector< AttachObjInfoStruct >& AttachList = WagonObj->AttachList();			

	for( unsigned i = 0 ; i < AttachList.size() ; i++ )
	{
		AttachObjInfoStruct& Info = AttachList[i];
		RoleDataEx* Other = Global::GetRoleDataByGUID( Info.ItemID );
		if( Other == NULL )
		{
			//AttachList.erase( AttachList.begin() + i );
			//i--;
			AttachList[i].Init();
			continue;
		}
		S_AttachObj( SendID , Info.ItemID , WagonObj->GUID() , Info.Type , Info.ItemPos.c_str() , Info.WagonPos.c_str() , i );
	}
}

int  NetSrv_MoveChild::_UnlockRolePosSchedular( SchedularInfo* Obj , void* InputClass )
{
	int DBID 		= Obj->GetNumber("dbid");
	int GUID 		= Obj->GetNumber("guid");
	int Time 		= Obj->GetNumber("time");

	//BaseItemObject* OwnerObj = BaseItemObject::GetObj( GUID );
	BaseItemObject* OwnerObj = BaseItemObject::GetObjByDBID( DBID );
	if( OwnerObj == NULL )
		return 0;

	RoleDataEx* Owner = OwnerObj->Role();
	if(		Owner == NULL || DBID != Owner->DBID() )
		return 0;

	if(		Owner->TempData.Sys.WaitUnlockPos == false 
		||	Owner->TempData.Sys.OnChangeZone != false 
		||	Owner->IsDead() != false )
	{
		Owner->TempData.Sys.WaitUnlockPos = false;
		S_UnlockRolePosOK( GUID , false );
		return 0;
	}

	if( Owner->IsNPC() )
		return 0;


	if( Time <= 0 )
	{
		Owner->TempData.Sys.WaitUnlockPos = false;
		POINT3D RevPoint;
		S_UnlockRolePosOK( GUID , true );
		if( Global::Ini()->IsHouseZone != false || Global::Ini()->IsGuildHouseZone != false )
			ChangeZone( GUID , Owner->SelfData.ReturnZoneID , 0 , Owner->SelfData.ReturnPos.X , Owner->SelfData.ReturnPos.Y , Owner->SelfData.ReturnPos.Z );
		else if( Global::Ini()->EnabledRevPoint != false )
		{
			if( Global::Ini()->RevZoneID == RoleDataEx::G_ZoneID )
				ChangeZone( GUID , Global::Ini()->RevZoneID , Owner->RoomID() , Global::Ini()->RevX , Global::Ini()->RevY , Global::Ini()->RevZ );
			else
				ChangeZone( GUID , Global::Ini()->RevZoneID , 0 , Global::Ini()->RevX , Global::Ini()->RevY , Global::Ini()->RevZ );
		}
		else if( OwnerObj->Path()->FindZoneRespawnPoint( Owner->Pos()->X , Owner->Pos()->Y , Owner->Pos()->Z , RevPoint ) != false )
			ChangeZone( GUID , RoleDataEx::G_ZoneID , Owner->RoomID() ,RevPoint.x , RevPoint.y , RevPoint.z );
		else
			ChangeZone( GUID , Owner->SelfData.RevZoneID , 0 ,Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z );

		return 0;
	}

	char Buf[256];
	sprintf( Buf , "--%d--" , Time );
	Owner->Msg( Buf );
	Time --;

	Schedular()->Push( _UnlockRolePosSchedular , 1000 , NULL 
		, "dbid" 		, EM_ValueType_Int , DBID
		, "guid" 		, EM_ValueType_Int , GUID
		, "time" 		, EM_ValueType_Int , Time
		, NULL );

	return 0;
}

void NetSrv_MoveChild::R_UnlockRolePosRequest( BaseItemObject* Player , float X , float Y , float Z , bool Cancel )
{
	RoleDataEx* Owner = Player->Role();
	
	if( Cancel != false )
	{
		Owner->TempData.Sys.WaitUnlockPos = false;
		return;
	}
	if( Owner->TempData.Sys.WaitUnlockPos != false )
		return;

//	if( Owner->TempData.Sys.ChangeZoneTime != false )
//		return;

	if( Global::Ini()->IsBattleWorld != false )
		return;

	Owner->TempData.Sys.WaitUnlockPos = true;

	//通知要倒數多久
	S_UnlockRolePosTime( Owner->GUID() , Global::Ini()->WaitUnlockPosTime );

	Print( LV2 , "R_UnlockRolePosRequest" , "DBID =%d X=%f Y=%f Z=%f" , Owner->DBID() , X , Y , Z );

	Schedular()->Push( _UnlockRolePosSchedular , 1000 , NULL 
		, "dbid" 		, EM_ValueType_Int , Owner->DBID()
		, "guid" 		, EM_ValueType_Int , Owner->GUID()
		, "time" 		, EM_ValueType_Int , Global::Ini()->WaitUnlockPosTime
		, NULL );
}

void NetSrv_MoveChild::R_AttachObjRequest( BaseItemObject* Player , int GItemID , int WagonItemID , AttachObjRequestENUM Type  , const char* ItemPos , const char* WagonPos , int AttachType )
{
	RoleDataEx* Role = Player->Role();

	RoleDataEx* Wagon = Global::GetRoleDataByGUID( WagonItemID );
	if(		Wagon == NULL 
		||	Role->TempData.Attr.Effect.Ride != false 
		||	Role->TempData.Attr.Effect.DisableWagon != false )
	{
		SC_AttachObjResult( Player->GUID() , Type , EN_AttachObjResult_Failed , -1 );
		return;
	}

	switch( Type )
	{
	case EM_AttachObjRequest_Attach:
		{

			if(		Role->CheckEnemy( Wagon ) != false 
				||	Role->TempData.Sys.OnChangeZone == true
				||	Wagon->TempData.Sys.OnChangeZone == true )
			{
				SC_AttachObjResult( Player->GUID() , Type , EN_AttachObjResult_Failed , -1 );
				return;
			}

			if( LuaPlotClass::AttachObj( Role->GUID() , WagonItemID , AttachType , ItemPos , WagonPos ) != false )			
				SC_AttachObjResult( Player->GUID() , Type , EN_AttachObjResult_OK , Role->TempData.AttachPos );			
			else
				SC_AttachObjResult( Player->GUID() , Type , EN_AttachObjResult_Failed , Role->TempData.AttachPos );
		}
		break;
	case EM_AttachObjRequest_Detach:
		{
			if( GItemID != Player->GUID() && WagonItemID != Player->GUID() )
			{
				SC_AttachObjResult( Player->GUID() , Type , EN_AttachObjResult_Failed , Role->TempData.AttachPos );
			}

			else if( LuaPlotClass::DetachObj( GItemID ) != false )
				SC_AttachObjResult( Player->GUID() , Type , EN_AttachObjResult_OK , Role->TempData.AttachPos );			
			else
				SC_AttachObjResult( Player->GUID() , Type , EN_AttachObjResult_Failed , Role->TempData.AttachPos );
		}
		break;
	}
}

//發送所有玩家的位置
void NetSrv_MoveChild::SendAllPlayerPos( )
{
	map< int , vector< PlayerBaseInfoStruct > > PlayerPosMap;

	set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if( Obj == NULL )
			continue;

		RoleDataEx* Role = Obj->Role();

		PlayerBaseInfoStruct Temp;

		Temp.DBID	= Role->DBID();
		Temp.X		= Role->X();
		Temp.Y		= Role->Y();
		Temp.Z		= Role->Z();

		Temp.IsAttack = Role->IsAttackMode();

		PlayerPosMap[Role->RoomID()].push_back( Temp );
	}

	
	for( map< int , vector< PlayerBaseInfoStruct > >::iterator Iter = PlayerPosMap.begin() 
		; Iter != PlayerPosMap.end() ; Iter++ )
	{
		SC_PlayerPos( Iter->second );
	}
}

void NetSrv_MoveChild::R_SetPartition			( BaseItemObject* OwnerObj , float X , float Y , float Z )
{
    RoleDataEx*     Owner = OwnerObj->Role();

    if( Owner->SecRoomID() == -1 )
        return ;

	if( Owner->TempData.Sys.PartitionLock == false ) 
		return ;


    int SecX = Global::CalSecX( X );
    int SecZ = Global::CalSecZ( Z );

    if( Owner->SecX() == SecX && Owner->SecZ() == SecZ )
        return;

    //計算分割區中心位置與目前位置差距是否到 1 個Block的距離
    int TempX = Owner->SecX() * Global::ViewBlockSize() + Global::ViewBlockSize() / 2;
    int TempZ = Owner->SecZ() * Global::ViewBlockSize() + Global::ViewBlockSize() / 2;

    if(		abs( int(TempX - Owner->X()) ) < Global::ViewBlockSize() * 2 / 3
        &&	abs( int(TempZ - Owner->Z()) ) < Global::ViewBlockSize() * 2 / 3 )
        return;


    //通知新進入的
    PlayIDInfo**	Block = Global::PartCompSearch( Owner , SecX , SecZ , _Def_View_Block_Range_ );

    if( Block == NULL )        
        return;
	
	Global::PartMove( Owner , SecX , SecZ );


    int			i , j;
    PlayID*		TopID;

    PlayerGraphStruct	OwnerGraph;
    PlayerGraphStruct	OtherGraph;


	CreatePlayerGraph( Owner, &OwnerGraph );

    for( i = 0 ; Block[i] != NULL ; i++ )
    {
        TopID = Block[i]->Begin;
        switch( Block[i]->Type )
        {
        case 1://新進入的人
            for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
            {
                //送給鄒為的玩家
                BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
                if( OtherObj == NULL )
                    continue;
				if( OtherObj->Role()->BaseData.Mode.IsAllZoneVisible != false )
				{
					_SendEnterViewInfo( OtherObj , OwnerObj );
				}
				else if( OwnerObj->Role()->BaseData.Mode.IsAllZoneVisible != false )
				{
					_SendEnterViewInfo( OwnerObj , OtherObj );
				}
				else
				{
					_SendAddPartInfo( OwnerObj , OtherObj , &OwnerGraph );
				}
            }
            break;
        case 2://離開視野範圍
            for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
            {
                //送給鄒為的玩家
                BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
                if( OtherObj == NULL )
                    continue;
                

				if( OtherObj->Role()->BaseData.Mode.IsAllZoneVisible != false )
				{
					_SendLeaveViewInfo( OtherObj , OwnerObj );
				}
				else if( OwnerObj->Role()->BaseData.Mode.IsAllZoneVisible != false )
				{
					_SendLeaveViewInfo( OwnerObj , OtherObj );
				}
				else
				{
					_SendDelPartInfo( OwnerObj , OtherObj  );
				}
            }
            break;
        }
    }
    return;
}

void NetSrv_MoveChild::SendAllBattlePlayerInfo( )
{
	map< int , vector<BattleGroundPlayerHPMPBaseStruct> > RoomPlayerInfo;
	BattleGroundPlayerHPMPBaseStruct Temp;

	set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//計算每個房間的人數
	for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
	{
		BaseItemObject* OtherObj = *PlayerObjIter;
		if( OtherObj == NULL )
			continue;
		RoleDataEx* Other = OtherObj->Role();

		Temp.GItemID = Other->GUID();
		Temp.X = (int)Other->X();
		Temp.Y = (int)Other->Y();
		Temp.Z = (int)Other->Z();
		Temp.HP = (int)Other->HP();
		Temp.MP = (int)Other->MP();
		Temp.SP = (int)Other->SP();
		Temp.SP_Sub = (int)Other->BaseData.SP_Sub;
		Temp.MaxHP = (int)Other->MaxHP();
		Temp.MaxMP = (int)Other->MaxMP();
		
		RoomPlayerInfo[Other->RoomID()].push_back( Temp );
	}	

	map< int , vector<BattleGroundPlayerHPMPBaseStruct> >::iterator Iter;

	for( Iter = RoomPlayerInfo.begin() ; Iter != RoomPlayerInfo.end() ; Iter++ )
	{
		SC_BattleGround_PlayerInfo_Zip( Iter->first , Iter->second );
	}

}

//處理需要修正座標的玩家
void NetSrv_MoveChild::FixAllPlayerPosProc	( )
{
	int blockSize = Ini()->BlockSize * 2.5;

	map< INT64 , vector<RoleDataEx*> > allPlayerMap;
	map< INT64 , vector<RoleDataEx*> > movePlayerMap;

	set<BaseItemObject* >&	playerObjSet = *(BaseItemObject::PlayerObjSet());
	set<BaseItemObject* >::iterator iter;
	for(  iter = playerObjSet.begin() ; iter != playerObjSet.end() ; iter++ )
	{
		RoleDataEx* role = (*iter)->Role();
		int secX = int( role->X() - Ini()->ViewMinX  ) / blockSize;
		int secZ = int( role->Z() - Ini()->ViewMinZ  ) / blockSize;
		INT64 keyValue = secX + secZ * 0x10000 + role->RoomID() * 0x100000000;
		allPlayerMap[keyValue].push_back( role );
	}


	for(  iter = playerObjSet.begin() ; iter != playerObjSet.end() ; iter++ )
	{
		RoleDataEx* role = (*iter)->Role();
		if( role->TempData.Sys.NeedFixClientPos == false )
			continue;
		role->TempData.Sys.NeedFixClientPos = false;
		int secX = int( role->X() - Ini()->ViewMinX  ) / blockSize;
		int secZ = int( role->Z() - Ini()->ViewMinZ  ) / blockSize;

		for( int i = -2 ; i < 3 ; i++ )
		{			
			for( int j = -2 ; j < 3 ; j++ )
			{
				INT64 keyValue = (secX+i) + (secZ+j) * 0x10000 + role->RoomID() * 0x100000000;
				if( allPlayerMap.find( keyValue ) == allPlayerMap.end() )
					continue;
				movePlayerMap[keyValue].push_back( role );
			}
		}
	}

	map< INT64 , vector<RoleDataEx*> >::iterator moveIter;
	for( moveIter = movePlayerMap.begin() ; moveIter != movePlayerMap.end() ; moveIter++ )
	{
		vector< RoleDataEx* >& moveList = moveIter->second;
		vector< RoleDataEx* >& toList = allPlayerMap[ moveIter->first ];
		SC_FixPlayerPos( toList , moveList );
	}

}