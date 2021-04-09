#include "NetSrv_Move.h"
#include "../../mainproc/PartyInfoList/PartyInfoList.h"
//-------------------------------------------------------------------
NetSrv_Move*    NetSrv_Move::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_Move::_Init()
{
    _Net->RegOnCliPacketFunction	( EM_PG_Move_CtoL_PlayerMoveObject     , _PG_Move_CtoL_PlayerMoveObject 
        , sizeof(PG_Move_CtoL_PlayerMoveObject) , sizeof(PG_Move_CtoL_PlayerMoveObject) );
	//_Net->RegOnCliPacketFunction	( EM_PG_Move_CtoL_PlayerMoveObject     , _PG_Move_CtoL_PlayerMoveObject );

	_Net->RegOnCliPacketFunction	( EM_PG_Move_CtoL_DoAction     , _PG_Move_CtoL_DoAction 
		, sizeof(PG_Move_CtoL_DoAction)			, sizeof(PG_Move_CtoL_DoAction) );


	_Net->RegOnCliPacketFunction	( EM_PG_Move_CtoL_JumpBegin     , _PG_Move_CtoL_JumpBegin 
		, sizeof(PG_Move_CtoL_JumpBegin)			, sizeof(PG_Move_CtoL_JumpBegin) );

	_Net->RegOnCliPacketFunction	( EM_PG_Move_CtoL_Ping     , _PG_Move_CtoL_Ping 
		, sizeof(PG_Move_CtoL_Ping)			, sizeof(PG_Move_CtoL_Ping) );


	_Net->RegOnCliPacketFunction	( EM_PG_Move_CtoL_ClientLoadingOK     , _PG_Move_CtoL_ClientLoadingOK 
		, sizeof(PG_Move_CtoL_ClientLoadingOK)			, sizeof(PG_Move_CtoL_ClientLoadingOK) );

	_Net->RegOnCliPacketFunction	( EM_PG_Move_CtoL_PingLog     , _PG_Move_CtoL_PingLog 
		, sizeof(PG_Move_CtoL_PingLog)							, sizeof(PG_Move_CtoL_PingLog) );

	_Net->RegOnCliPacketFunction	( EM_PG_Move_CtoL_UnlockRolePosRequest     , _PG_Move_CtoL_UnlockRolePosRequest 
		, sizeof(PG_Move_CtoL_UnlockRolePosRequest)							, sizeof(PG_Move_CtoL_UnlockRolePosRequest) );

	_Net->RegOnCliPacketFunction	( EM_PG_Move_CtoL_AttachObjRequest     , _PG_Move_CtoL_AttachObjRequest 
		, sizeof(PG_Move_CtoL_AttachObjRequest)							, sizeof(PG_Move_CtoL_AttachObjRequest) );

	_Net->RegOnCliPacketFunction	( EM_PG_Move_CtoL_SetPartition     , _PG_Move_CtoL_SetPartition 
		, sizeof(PG_Move_CtoL_SetPartition)							, sizeof(PG_Move_CtoL_SetPartition) );

    return false;
}
//-------------------------------------------------------------------
bool NetSrv_Move::_Release()
{
    return false;
}
//-------------------------------------------------------------------
void NetSrv_Move::_PG_Move_CtoL_SetPartition		( int Sockid , int Size , void* Data )
{
	PG_Move_CtoL_SetPartition*   PG = (PG_Move_CtoL_SetPartition*)Data;
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );

	if( Player == NULL )
		return;

	This->R_SetPartition( Player , PG->X , PG->Y , PG->Z );  
}
void NetSrv_Move::_PG_Move_CtoL_AttachObjRequest( int Sockid , int Size , void* Data )
{
	PG_Move_CtoL_AttachObjRequest*   PG = (PG_Move_CtoL_AttachObjRequest*)Data;
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );

	if( Player == NULL )
		return;

	This->R_AttachObjRequest( Player , PG->GItemID , PG->WagonItemID , PG->Type  , PG->ItemPos.Begin() , PG->WagonPos.Begin() , PG->AttachType );  
}

void NetSrv_Move::_PG_Move_CtoL_UnlockRolePosRequest( int Sockid , int Size , void* Data )
{
	PG_Move_CtoL_UnlockRolePosRequest*   PG = (PG_Move_CtoL_UnlockRolePosRequest*)Data;
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );

	if( Player == NULL )
		return;

	This->R_UnlockRolePosRequest( Player , PG->X , PG->Y , PG->Z , PG->Cancel );  
}

void NetSrv_Move::_PG_Move_CtoL_PingLog		( int Sockid , int Size , void* Data )
{
	PG_Move_CtoL_PingLog*   PG = (PG_Move_CtoL_PingLog*)Data;
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );

	if( Player == NULL )
		return;

	This->R_PingLog( Player , PG->ID , PG->Time , PG->ClietNetProcTime );  
}

void NetSrv_Move::_PG_Move_CtoL_ClientLoadingOK ( int Sockid , int Size , void* Data )
{
	PG_Move_CtoL_ClientLoadingOK*   PG = (PG_Move_CtoL_ClientLoadingOK*)Data;
	BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );

	if( Player == NULL )
		return;

	This->R_ClientLoadingOK( Player );  
}

void NetSrv_Move::_PG_Move_CtoL_Ping	          ( int SockID , int Size , void* Data )
{
	PG_Move_CtoL_Ping* PG = (PG_Move_CtoL_Ping*)Data;

    BaseItemObject* Player = BaseItemObject::GetObjBySockID( SockID );

    if( Player == NULL )
        return;

	//回送Ping
	S_Ping( SockID , PG->Time );

	This->R_Ping( Player , PG->Time );
}
void NetSrv_Move::_PG_Move_CtoL_PlayerMoveObject( int CliID , int Size , void* Data )
{
    PG_Move_CtoL_PlayerMoveObject*   PG = (PG_Move_CtoL_PlayerMoveObject*)Data;
    BaseItemObject* Player = BaseItemObject::GetObjBySockID( CliID );

    if( Player == NULL )
        return;

    This->PlayerMove( Player , PG->GItemID , PG->Pos.X , PG->Pos.Y , PG->Pos.Z , PG->Pos.Dir , PG->Type , PG->vX , PG->vY , PG->vZ , PG->AttachObjID );  
}

void NetSrv_Move::_PG_Move_CtoL_DoAction        ( int CliID , int Size , void* Data )
{
    PG_Move_CtoL_DoAction*   PG = (PG_Move_CtoL_DoAction*)Data;
    BaseItemObject* Player = BaseItemObject::GetObjBySockID( CliID );

    if( Player == NULL )
        return;

    This->R_DoAction( Player , PG->GItemID , PG->ActionID , PG->TempActionID );  
}

void NetSrv_Move::_PG_Move_CtoL_JumpBegin       ( int Sockid , int Size , void* Data )
{
    PG_Move_CtoL_JumpBegin*   PG = (PG_Move_CtoL_JumpBegin*)Data;
    BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );

    if( Player == NULL )
        return;

    This->R_JumpBegin( Player , PG->X , PG->Y , PG->Z , PG->Dir , PG->vX , PG->vY , PG->vZ );
}
void NetSrv_Move::_PG_Move_CtoL_JumpEnd         ( int Sockid , int Size , void* Data )
{
    PG_Move_CtoL_JumpEnd*   PG = (PG_Move_CtoL_JumpEnd*)Data;
    BaseItemObject* Player = BaseItemObject::GetObjBySockID( Sockid );

    if( Player == NULL )
        return;

    This->R_JumpEnd( Player , PG->X , PG->Y , PG->Z , PG->Dir , PG->AttackObjID );
}
//-------------------------------------------------------------------
/*
void NetSrv_Move::Move( int SendToID , int ItemID , float X , float Y , float Z , float Dir , float vX , float vY , float vZ , ClientMoveTypeENUM Type )
{
	PG_Move_LtoC_ObjectMove Send;

	Send.Pos.X    = X;
	Send.Pos.Y    = Y;
	Send.Pos.Z    = Z;
	Send.Pos.Dir  = Dir;
	Send.vX		  = vX;	
	Send.vY		  = vY;
	Send.vZ		  = vZ;
	Send.Type	  = Type;

	Send.GItemID    = ItemID;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
*/
//通知玩家某物件移動
void NetSrv_Move::Move( int SendToID , RoleDataEx* Item )
{
	if( Item->BaseData.Mode.DisableMovePG != false )
		return;

	if( Item->IsPlayer() )
	{
		PG_Move_LtoC_PlayerMove Send;

		Send.X    	= Item->TempData.Move.Tx;
		Send.Y    	= Item->TempData.Move.Ty;
		Send.Z    	= Item->TempData.Move.Tz;
		Send.Dir  	=  int(Item->BaseData.Pos.Dir)%360;
		if( Item->TempData.Move.Dx > 255 )
			Send.vX = 255;
		else if( Item->TempData.Move.Dx < -256 )
			Send.vX = -256;
		else
			Send.vX		= int( Item->TempData.Move.Dx );	

		if( Item->TempData.Move.Dz > 255 )
			Send.vZ = 255;
		else if( Item->TempData.Move.Dz < -256 )
			Send.vZ = -256;
		else
			Send.vZ		= int( Item->TempData.Move.Dz );

		Send.Type	= int( Item->TempData.Move.MoveType );

		Send.GItemID    = Item->GUID();
		Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );

	}
	else
	{
		PG_Move_LtoC_ObjectMove Send;

		Send.Pos.X    = Item->TempData.Move.Tx;
		Send.Pos.Y    = Item->TempData.Move.Ty;
		Send.Pos.Z    = Item->TempData.Move.Tz;
		Send.Pos.Dir  = Item->BaseData.Pos.Dir;

		Send.vX		  = Item->X();	
		Send.vY		  = Item->Y();	
		Send.vZ		  = Item->Z();

		Send.Type	  = Item->TempData.Move.MoveType;

		Send.GItemID    = Item->GUID();
		Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
	}

	/*

    PG_Move_LtoC_ObjectMove Send;

    Send.Pos.X    = Item->TempData.Move.Tx;
    Send.Pos.Y    = Item->TempData.Move.Ty;
    Send.Pos.Z    = Item->TempData.Move.Tz;
    Send.Pos.Dir  = Item->BaseData.Pos.Dir;
	if( Item->IsPlayer() )
	{
		Send.vX		  = Item->TempData.Move.Dx;	
		Send.vY		  = Item->TempData.Move.Dy;	
		Send.vZ		  = Item->TempData.Move.Dz;
	}
	else
	{
		Send.vX		  = Item->X();	
		Send.vY		  = Item->Y();	
		Send.vZ		  = Item->Z();
	}
	Send.Type	  = Item->TempData.Move.MoveType;

    Send.GItemID    = Item->GUID();
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
	*/
}

//通知玩家某物件特殊移動
void NetSrv_Move::SpecialMove( int SendToID , int TargetID , SpecialMoveTypeENUM MoveType , RoleDataEx* Owner , int MagicCollectID )
{
	PG_Move_LtoC_SpecialMove	Send;
	Send.X = Owner->Pos()->X;
	Send.Y = Owner->Pos()->Y;
	Send.Z = Owner->Pos()->Z;
	Send.Dir = Owner->Pos()->Dir;
	Send.MoveType = MoveType;
	Send.TargetGUID = TargetID;
	Send.ItemGUID = Owner->GUID();
	Send.MagicCollectID = MagicCollectID;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Move::SpecialMove_BySockID( int SockID , int ProxyID , int TargetID , SpecialMoveTypeENUM	MoveType , RoleDataEx* Owner , int MagicCollectID )
{
	PG_Move_LtoC_SpecialMove	Send;
	Send.X = Owner->Pos()->X;
	Send.Y = Owner->Pos()->Y;
	Send.Z = Owner->Pos()->Z;
	Send.Dir = Owner->Pos()->Dir;
	Send.MoveType = MoveType;
	Send.TargetGUID = TargetID;
	Send.ItemGUID = Owner->GUID();
	Send.MagicCollectID = MagicCollectID;

	Global::SendToCli_ByProxyID( SockID , ProxyID , sizeof(Send) , &Send );
//	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}



//設定一個顯像物件
void NetSrv_Move::AddObject(  int SendToID , int GItemID , int WorldGUID , int DBID , int MasterGUID , CampID_ENUM CampID , int iWorldID, RolePosStruct& Pos , PlayerGraphStruct& Graph )
{
//    Print( Def_PrintLV1 , "AddObject  SendToID = %x GItemID = %x" ,  SendToID , GItemID );
//    if( Global::IsMirror( SendToID ) )
//       return;s

    PG_Move_LtoC_AddToPartition Send;
    Send.GItemID = GItemID;
	/*
	if( LGItemID == -1 )
		Send.LGItemID = GItemID;
	else
		Send.LGItemID  = LGItemID;
		*/
	Send.WorldGUID		= WorldGUID;
    Send.GraphObj		= Graph;
    Send.Pos			= Pos;
	Send.DBID			= DBID;
	Send.MasterGUID		= MasterGUID;
	Send.CampID			= CampID;
	Send.iWorldID		= iWorldID;
//    Item->GetGraphic( &(Send.GraphObj) );
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

//把某一個物件移除
void NetSrv_Move::DelObject( int SendToID , int GItemID , int WorldGUID )
{
//    Print( Def_PrintLV1 , "DelObject  SendToID = %x GItemID = %x" ,  SendToID , GItemID );
//    if( Global::IsMirror( SendToID ) )
//       return;

    PG_Move_LtoC_DelFromPartition Send;
    Send.GItemID = GItemID;
    Send.WorldGUID  = WorldGUID;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}


//重設位置
void NetSrv_Move::SetPos_AllCli(  RoleDataEx* Item )
{
	PG_Move_LtoC_SetPostion Send;
	Send.GItemID = Item->GUID();
	Send.Pos   = *(Item->Pos() );

	Item->TempData.Move.IsLockSetPos = true;
	Item->TempData.Move.LockSetPosX = Send.Pos.X;
	Item->TempData.Move.LockSetPosZ = Send.Pos.Z;

	Item->TempData.Move.BeginJumpX = Item->Pos()->X;
	Item->TempData.Move.BeginJumpY = Item->Pos()->Y;
	Item->TempData.Move.BeginJumpZ = Item->Pos()->Z;
	Item->TempData.Move.MoveType = EM_ClientMoveType_Normal;


	Global::SendToAllCli( sizeof( Send ) , &Send );
}

//重設位置
void NetSrv_Move::SetPos( int SendToID , RoleDataEx* Item )
{
    PG_Move_LtoC_SetPostion Send;
    Send.GItemID = Item->GUID();
    Send.Pos   = *(Item->Pos() );

	Item->TempData.Move.IsLockSetPos = true;
	Item->TempData.Move.LockSetPosX = Send.Pos.X;
	Item->TempData.Move.LockSetPosZ = Send.Pos.Z;

	Item->TempData.Move.BeginJumpX = Item->Pos()->X;
	Item->TempData.Move.BeginJumpY = Item->Pos()->Y;
	Item->TempData.Move.BeginJumpZ = Item->Pos()->Z;
	Item->TempData.Move.MoveType = EM_ClientMoveType_Normal;


    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

//做動作
void NetSrv_Move::DoAction( int SendToID , int GItemID , int ActionID  , int TempAction )
{
    PG_Move_LtoC_DoAction Send;
    Send.GItemID      = GItemID;
//    Send.WorldGUID  = WorldGUID;
    Send.ActionID     = ActionID;
    Send.TempActionID = TempAction;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

//設定移動速度
void NetSrv_Move::SendMoveSpeed( int SendToID , RoleDataEx* Item )
{
    PG_Move_LtoC_MoveSpeed Send;
    Send.GItemID = Item->GUID();
    Send.Speed   = Item->TempData.Attr.Fin.MoveSpeed;
//    Send.WorldGUID  = Item->WorldGUID();
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Move::SendMoveInfo( int SendToID , RoleDataEx* Item )
{
	PG_Move_LtoC_MoveInfo Send;
	Send.GItemID = Item->GUID();
	Send.Gravity = Item->TempData.Attr.Mid.Body[ EM_WearEqType_Gravity ];
	Send.JumpRate= Item->TempData.Attr.Mid.Body[ EM_WearEqType_JumpRate ];
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

//通知某一玩家Model的縮放比例

void NetSrv_Move::SendModelRate( int SendToID , RoleDataEx* Item )
{
    PG_Move_LtoC_ModelRate Send;
    Send.GItemID   = Item->GUID();
    Send.ModelRate = Item->TempData.ModelRate;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

//-------------------------------------------------------------------
//通知某玩家的裝備顯像資料
void NetSrv_Move::LookEQ( int SendToID , RoleDataEx* Item )
{
	PG_Move_LtoC_EQInfo Send;
	Send.ItemID					= Item->GUID();


	if( Item->IsPlayer() )
	{
		CoolSuitBase& Info = Item->PlayerBaseData->CoolSuitList[ Item->PlayerBaseData->CoolSuitID ];
		//Send.ShowEQ					= Info.ShowEQ;
		Send.ShowEQ._Value			= -1;
		Send.EQLook.EQ.MainHand		= Item->BaseData.EQ.MainHand.ImageObjectID;
		Send.EQLook.EQ.SecondHand	= Item->BaseData.EQ.SecondHand.ImageObjectID;
		Send.EQLook.EQ.Bow			= Item->BaseData.EQ.Bow.ImageObjectID;
		Send.EQLook.EQ.Manufacture	= Item->BaseData.EQ.Manufacture.ImageObjectID;

		Send.EQLook.MainColor.MainHand		= Item->BaseData.EQ.MainHand.MainColor;
		Send.EQLook.MainColor.SecondHand	= Item->BaseData.EQ.SecondHand.MainColor;
		Send.EQLook.MainColor.Bow			= Item->BaseData.EQ.Bow.MainColor;
		Send.EQLook.MainColor.Manufacture	= Item->BaseData.EQ.Manufacture.MainColor;

		Send.EQLook.OffColor.MainHand		= Item->BaseData.EQ.MainHand.OffColor;
		Send.EQLook.OffColor.SecondHand		= Item->BaseData.EQ.SecondHand.OffColor;
		Send.EQLook.OffColor.Bow			= Item->BaseData.EQ.Bow.OffColor;
		Send.EQLook.OffColor.Manufacture	= Item->BaseData.EQ.Manufacture.OffColor;

		Send.EQLook.Level.MainHand		= Item->BaseData.EQ.MainHand.Level;
		Send.EQLook.Level.SecondHand	= Item->BaseData.EQ.SecondHand.Level;
		Send.EQLook.Level.Bow			= Item->BaseData.EQ.Bow.Level;
		Send.EQLook.Level.Manufacture	= Item->BaseData.EQ.Manufacture.Level;

		if( Info.ShowEQ.Head == false )
		{
			Send.EQLook.EQ.Head			= 0;
			Send.EQLook.MainColor.Head	= 0;
			Send.EQLook.OffColor.Head	= 0;
			Send.EQLook.Level.Head		= 0;
		}
		else if( Info.Head.IsEmpty() == false )
		{
			Send.EQLook.EQ.Head			= Info.Head.ImageObjectID;
			Send.EQLook.MainColor.Head	= Info.Head.MainColor;
			Send.EQLook.OffColor.Head	= Info.Head.OffColor;
			Send.EQLook.Level.Head		= 0;
		}
		else
		{
			Send.EQLook.EQ.Head			= Item->BaseData.EQ.Head.ImageObjectID;
			Send.EQLook.MainColor.Head	= Item->BaseData.EQ.Head.MainColor;
			Send.EQLook.OffColor.Head	= Item->BaseData.EQ.Head.OffColor;
			Send.EQLook.Level.Head		= Item->BaseData.EQ.Head.Level;
		}

		if( Info.ShowEQ.Gloves == false )
		{
			Send.EQLook.EQ.Gloves			= 0;
			Send.EQLook.MainColor.Gloves	= 0;
			Send.EQLook.OffColor.Gloves		= 0;
			Send.EQLook.Level.Gloves		= 0;
		}
		else if( Info.Gloves.IsEmpty() == false )
		{
			Send.EQLook.EQ.Gloves			= Info.Gloves.ImageObjectID;
			Send.EQLook.MainColor.Gloves	= Info.Gloves.MainColor;
			Send.EQLook.OffColor.Gloves		= Info.Gloves.OffColor;
			Send.EQLook.Level.Gloves		= 0;
		}
		else
		{
			Send.EQLook.EQ.Gloves			= Item->BaseData.EQ.Gloves.ImageObjectID;
			Send.EQLook.MainColor.Gloves	= Item->BaseData.EQ.Gloves.MainColor;
			Send.EQLook.OffColor.Gloves		= Item->BaseData.EQ.Gloves.OffColor;
			Send.EQLook.Level.Gloves		= Item->BaseData.EQ.Gloves.Level;
		}

		if( Info.ShowEQ.Shoes == false )
		{
			Send.EQLook.EQ.Shoes			= 0;
			Send.EQLook.MainColor.Shoes	= 0;
			Send.EQLook.OffColor.Shoes		= 0;
			Send.EQLook.Level.Shoes		= 0;
		}
		else if( Info.Shoes.IsEmpty() == false )
		{
			Send.EQLook.EQ.Shoes			= Info.Shoes.ImageObjectID;
			Send.EQLook.MainColor.Shoes		= Info.Shoes.MainColor;
			Send.EQLook.OffColor.Shoes		= Info.Shoes.OffColor;
			Send.EQLook.Level.Shoes			= 0;
		}
		else
		{
			Send.EQLook.EQ.Shoes			= Item->BaseData.EQ.Shoes.ImageObjectID;
			Send.EQLook.MainColor.Shoes		= Item->BaseData.EQ.Shoes.MainColor;
			Send.EQLook.OffColor.Shoes		= Item->BaseData.EQ.Shoes.OffColor;
			Send.EQLook.Level.Shoes			= Item->BaseData.EQ.Shoes.Level;
		}

		if( Info.ShowEQ.Clothes == false )
		{
			Send.EQLook.EQ.Clothes			= 0;
			Send.EQLook.MainColor.Clothes	= 0;
			Send.EQLook.OffColor.Clothes	= 0;
			Send.EQLook.Level.Clothes		= 0;
		}
		else if( Info.Clothes.IsEmpty() == false )
		{
			Send.EQLook.EQ.Clothes			= Info.Clothes.ImageObjectID;
			Send.EQLook.MainColor.Clothes	= Info.Clothes.MainColor;
			Send.EQLook.OffColor.Clothes	= Info.Clothes.OffColor;
			Send.EQLook.Level.Clothes		= 0;
		}
		else
		{
			Send.EQLook.EQ.Clothes			= Item->BaseData.EQ.Clothes.ImageObjectID;
			Send.EQLook.MainColor.Clothes		= Item->BaseData.EQ.Clothes.MainColor;
			Send.EQLook.OffColor.Clothes		= Item->BaseData.EQ.Clothes.OffColor;
			Send.EQLook.Level.Clothes			= Item->BaseData.EQ.Clothes.Level;
		}

		if( Info.ShowEQ.Pants == false )
		{
			Send.EQLook.EQ.Pants		= 0;
			Send.EQLook.MainColor.Pants	= 0;
			Send.EQLook.OffColor.Pants	= 0;
			Send.EQLook.Level.Pants		= 0;
		}
		else if( Info.Pants.IsEmpty() == false )
		{
			Send.EQLook.EQ.Pants		= Info.Pants.ImageObjectID;
			Send.EQLook.MainColor.Pants	= Info.Pants.MainColor;
			Send.EQLook.OffColor.Pants	= Info.Pants.OffColor;
			Send.EQLook.Level.Pants		= 0;
		}
		else
		{
			Send.EQLook.EQ.Pants			= Item->BaseData.EQ.Pants.ImageObjectID;
			Send.EQLook.MainColor.Pants		= Item->BaseData.EQ.Pants.MainColor;
			Send.EQLook.OffColor.Pants		= Item->BaseData.EQ.Pants.OffColor;
			Send.EQLook.Level.Pants			= Item->BaseData.EQ.Pants.Level;
		}

		if( Info.ShowEQ.Back == false )
		{
			Send.EQLook.EQ.Back		= 0;
			Send.EQLook.MainColor.Back	= 0;
			Send.EQLook.OffColor.Back	= 0;
			Send.EQLook.Level.Back		= 0;
		}
		else if( Info.Back.IsEmpty() == false )
		{
			Send.EQLook.EQ.Back			= Info.Back.ImageObjectID;
			Send.EQLook.MainColor.Back	= Info.Back.MainColor;
			Send.EQLook.OffColor.Back	= Info.Back.OffColor;
			Send.EQLook.Level.Back		= 0;
		}
		else
		{
			Send.EQLook.EQ.Back				= Item->BaseData.EQ.Back.ImageObjectID;
			Send.EQLook.MainColor.Back		= Item->BaseData.EQ.Back.MainColor;
			Send.EQLook.OffColor.Back		= Item->BaseData.EQ.Back.OffColor;
			Send.EQLook.Level.Back			= Item->BaseData.EQ.Back.Level;
		}

		if( Info.ShowEQ.Belt == false )
		{
			Send.EQLook.EQ.Belt		= 0;
			Send.EQLook.MainColor.Belt	= 0;
			Send.EQLook.OffColor.Belt	= 0;
			Send.EQLook.Level.Belt		= 0;
		}
		else if( Info.Belt.IsEmpty() == false )
		{
			Send.EQLook.EQ.Belt			= Info.Belt.ImageObjectID;
			Send.EQLook.MainColor.Belt	= Info.Belt.MainColor;
			Send.EQLook.OffColor.Belt	= Info.Belt.OffColor;
			Send.EQLook.Level.Belt		= 0;
		}
		else
		{
			Send.EQLook.EQ.Belt				= Item->BaseData.EQ.Belt.ImageObjectID;
			Send.EQLook.MainColor.Belt		= Item->BaseData.EQ.Belt.MainColor;
			Send.EQLook.OffColor.Belt		= Item->BaseData.EQ.Belt.OffColor;
			Send.EQLook.Level.Belt			= Item->BaseData.EQ.Belt.Level;
		}

		if( Info.ShowEQ.Shoulder == false )
		{
			Send.EQLook.EQ.Shoulder		= 0;
			Send.EQLook.MainColor.Shoulder	= 0;
			Send.EQLook.OffColor.Shoulder	= 0;
			Send.EQLook.Level.Shoulder		= 0;
		}
		else if( Info.Shoulder.IsEmpty() == false )
		{
			Send.EQLook.EQ.Shoulder			= Info.Shoulder.ImageObjectID;
			Send.EQLook.MainColor.Shoulder	= Info.Shoulder.MainColor;
			Send.EQLook.OffColor.Shoulder	= Info.Shoulder.OffColor;
			Send.EQLook.Level.Shoulder		= 0;
		}
		else
		{
			Send.EQLook.EQ.Shoulder				= Item->BaseData.EQ.Shoulder.ImageObjectID;
			Send.EQLook.MainColor.Shoulder		= Item->BaseData.EQ.Shoulder.MainColor;
			Send.EQLook.OffColor.Shoulder		= Item->BaseData.EQ.Shoulder.OffColor;
			Send.EQLook.Level.Shoulder			= Item->BaseData.EQ.Shoulder.Level;
		}

		if( Info.ShowEQ.Ornament == false )
		{
			Send.EQLook.EQ.Ornament		= 0;
			Send.EQLook.MainColor.Ornament	= 0;
			Send.EQLook.OffColor.Ornament	= 0;
			Send.EQLook.Level.Ornament		= 0;
		}
		else if( Info.Ornament.IsEmpty() == false )
		{
			Send.EQLook.EQ.Ornament			= Info.Ornament.ImageObjectID;
			Send.EQLook.MainColor.Ornament	= Info.Ornament.MainColor;
			Send.EQLook.OffColor.Ornament	= Info.Ornament.OffColor;
			Send.EQLook.Level.Ornament		= 0;
		}
		else
		{
			Send.EQLook.EQ.Ornament				= Item->BaseData.EQ.Ornament.ImageObjectID;
			Send.EQLook.MainColor.Ornament		= Item->BaseData.EQ.Ornament.MainColor;
			Send.EQLook.OffColor.Ornament		= Item->BaseData.EQ.Ornament.OffColor;
			Send.EQLook.Level.Ornament			= Item->BaseData.EQ.Ornament.Level;
		}
	}
	else
	{
		bool isNeedSend = false;
		for( int i = 0 ; i < EM_EQWearPos_MaxCount ; i++ )
		{
			if( Item->BaseData.EQ.Item[i].IsEmpty() == false )
			{
				isNeedSend = true;
				break;
			}
		}
		if( isNeedSend == false )
			return;
		Send.ShowEQ._Value			= 0;
		Send.EQLook.EQ.Head			= Item->BaseData.EQ.Head.ImageObjectID;
		Send.EQLook.EQ.Gloves	    = Item->BaseData.EQ.Gloves.ImageObjectID;
		Send.EQLook.EQ.Shoes	    = Item->BaseData.EQ.Shoes.ImageObjectID;
		Send.EQLook.EQ.Clothes	    = Item->BaseData.EQ.Clothes.ImageObjectID;
		Send.EQLook.EQ.Pants	    = Item->BaseData.EQ.Pants.ImageObjectID;
		Send.EQLook.EQ.Back			= Item->BaseData.EQ.Back.ImageObjectID;
		Send.EQLook.EQ.Belt			= Item->BaseData.EQ.Belt.ImageObjectID;
		Send.EQLook.EQ.Shoulder		= Item->BaseData.EQ.Shoulder.ImageObjectID;
		Send.EQLook.EQ.MainHand		= Item->BaseData.EQ.MainHand.ImageObjectID;
		Send.EQLook.EQ.SecondHand	= Item->BaseData.EQ.SecondHand.ImageObjectID;
		Send.EQLook.EQ.Bow			= Item->BaseData.EQ.Bow.ImageObjectID;
		Send.EQLook.EQ.Manufacture	= Item->BaseData.EQ.Manufacture.ImageObjectID;
		Send.EQLook.EQ.Ornament		= Item->BaseData.EQ.Ornament.ImageObjectID;


		Send.EQLook.MainColor.Head			= Item->BaseData.EQ.Head.MainColor;
		Send.EQLook.MainColor.Gloves	    = Item->BaseData.EQ.Gloves.MainColor;
		Send.EQLook.MainColor.Shoes			= Item->BaseData.EQ.Shoes.MainColor;
		Send.EQLook.MainColor.Clothes	    = Item->BaseData.EQ.Clothes.MainColor;
		Send.EQLook.MainColor.Pants			= Item->BaseData.EQ.Pants.MainColor;
		Send.EQLook.MainColor.Back			= Item->BaseData.EQ.Back.MainColor;
		Send.EQLook.MainColor.Belt			= Item->BaseData.EQ.Belt.MainColor;
		Send.EQLook.MainColor.Shoulder		= Item->BaseData.EQ.Shoulder.MainColor;
		Send.EQLook.MainColor.MainHand		= Item->BaseData.EQ.MainHand.MainColor;
		Send.EQLook.MainColor.SecondHand	= Item->BaseData.EQ.SecondHand.MainColor;
		Send.EQLook.MainColor.Bow			= Item->BaseData.EQ.Bow.MainColor;
		Send.EQLook.MainColor.Manufacture	= Item->BaseData.EQ.Manufacture.MainColor;
		Send.EQLook.MainColor.Ornament		= Item->BaseData.EQ.Ornament.MainColor;

		Send.EQLook.OffColor.Head			= Item->BaseData.EQ.Head.OffColor;
		Send.EQLook.OffColor.Gloves			= Item->BaseData.EQ.Gloves.OffColor;
		Send.EQLook.OffColor.Shoes			= Item->BaseData.EQ.Shoes.OffColor;
		Send.EQLook.OffColor.Clothes	    = Item->BaseData.EQ.Clothes.OffColor;
		Send.EQLook.OffColor.Pants			= Item->BaseData.EQ.Pants.OffColor;
		Send.EQLook.OffColor.Back			= Item->BaseData.EQ.Back.OffColor;
		Send.EQLook.OffColor.Belt			= Item->BaseData.EQ.Belt.OffColor;
		Send.EQLook.OffColor.Shoulder		= Item->BaseData.EQ.Shoulder.OffColor;
		Send.EQLook.OffColor.MainHand		= Item->BaseData.EQ.MainHand.OffColor;
		Send.EQLook.OffColor.SecondHand		= Item->BaseData.EQ.SecondHand.OffColor;
		Send.EQLook.OffColor.Bow			= Item->BaseData.EQ.Bow.OffColor;
		Send.EQLook.OffColor.Manufacture	= Item->BaseData.EQ.Manufacture.OffColor;
		Send.EQLook.OffColor.Ornament		= Item->BaseData.EQ.Ornament.OffColor;

		Send.EQLook.Level.Head			= Item->BaseData.EQ.Head.Level;
		Send.EQLook.Level.Gloves		= Item->BaseData.EQ.Gloves.Level;
		Send.EQLook.Level.Shoes			= Item->BaseData.EQ.Shoes.Level;
		Send.EQLook.Level.Clothes	    = Item->BaseData.EQ.Clothes.Level;
		Send.EQLook.Level.Pants			= Item->BaseData.EQ.Pants.Level;
		Send.EQLook.Level.Back			= Item->BaseData.EQ.Back.Level;
		Send.EQLook.Level.Belt			= Item->BaseData.EQ.Belt.Level;
		Send.EQLook.Level.Shoulder		= Item->BaseData.EQ.Shoulder.Level;
		Send.EQLook.Level.MainHand		= Item->BaseData.EQ.MainHand.Level;
		Send.EQLook.Level.SecondHand	= Item->BaseData.EQ.SecondHand.Level;
		Send.EQLook.Level.Bow			= Item->BaseData.EQ.Bow.Level;
		Send.EQLook.Level.Manufacture	= Item->BaseData.EQ.Manufacture.Level;
		Send.EQLook.Level.Ornament		= Item->BaseData.EQ.Ornament.Level;

		Send.ShowEQ._Value = -1;
	}


    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
//通知某玩家的裝備顯像資料
/*
void NetSrv_Move::LookEQ( int SendToID , int ItemID , EQWearPosENUM Pos , int EQ )
{
    PG_Move_LtoC_FixEQInfo Send;
    Send.ItemID = ItemID;
    Send.Pos    = Pos;
    Send.EQ     = EQ;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}*/
//-------------------------------------------------------------------
//通知某玩家的角色容貌微調
void NetSrv_Move::LookFace( int SendToID , RoleDataEx* Item )
{
    PG_Move_LtoC_LookFace Send;
    Send.GItemID = Item->GUID();
    Send.Look = Item->BaseData.Look;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
//通知CliAutoPlot
void NetSrv_Move::SendCliAuto( int SendToID , int GItemID , const char* Plot )
{
    PG_Move_LtoC_ClientAutoPlot Send;
    Send.AutoPlot = Plot;
    Send.GItemID = GItemID;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//通知物件目前狀態
void NetSrv_Move::SendActionType( int SendToID , int GItemID , RoleActionStruct	Action )
{
    PG_Move_LtoC_ActionType Send;
    Send.GItemID = GItemID;
    Send.Action = Action;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

//Server 通知 周圍玩家某角色要做跳越
void NetSrv_Move::S_JumpBegin( int SendToID , int GItemID , float X , float Y , float Z , float Dir , float vX , float vY , float vZ )
{
    PG_Move_LtoC_JumpBegin Send;
    Send.GItemID = GItemID;
    Send.X = X;
    Send.Y = Y;
    Send.Z = Z;
    Send.Dir = Dir;
    Send.vX = vX;
    Send.vY = vY;
    Send.vZ = vZ;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

//Server 通知 Client跳越結束
void NetSrv_Move::S_JumpEnd  ( int SendToID , int GItemID , float X , float Y , float Z , float Dir )
{
    PG_Move_LtoC_JumpEnd Send;
    Send.GItemID = GItemID;
    Send.X = X;
    Send.Y = Y;
    Send.Z = Z;
    Send.Dir = Dir;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

//通知某物件與玩家的關係
void NetSrv_Move::S_Relation( int SendID , int TargetID , RoleRelationStruct R )
{
    PG_Move_LtoC_Relation Send;
    Send.GItemID = TargetID;
    Send.Relation = R;

    Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

//通知Party成員自己的位置
void NetSrv_Move::S_PartyMemberPos( int SendToDBID , int DBID , int GItemID , float X , float Y , float Z )
{
	PG_Move_LtoC_PartyMemberMove Send;
	Send.DBID = DBID;
	Send.GItemID = GItemID;
	Send.X = X;
	Send.Y = Y;
	Send.Z = Z;
	Global::SendToCli_ByDBID( SendToDBID , sizeof(Send) , &Send );
}

void NetSrv_Move::S_PlayerMotion( int SendToID , int ObjID , int MotionID )
{
	PG_Move_LtoC_PlayMotion Packet;

	Packet.iObjID		= ObjID;
	Packet.iMotionID	= MotionID;
	Global::SendToCli_ByGUID( SendToID , sizeof( Packet ) , &Packet );
}
void NetSrv_Move::S_SetNPCMotion( BaseItemObject* pObj, int iMotionID , int iIdleMotionID )
{
	if( pObj == NULL )	return;

	PG_Move_LtoC_PlayMotion Packet;

	Packet.iObjID		= pObj->GUID();
	Packet.iMotionID	= iMotionID;

    RoleDataEx* Owner = pObj->Role();
	Owner->TempData.IdleMotionID	= iIdleMotionID;

	if( pObj->Role()->BaseData.Mode.IsAllZoneVisible )
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

			Global::SendToCli_ByGUID( OtherObj->GUID() , sizeof( Packet ) , &Packet );
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
				if( OtherObj == NULL )
					continue;
				if( OtherObj->Role()->IsNPC() != false )
					continue;

				Global::SendToCli_ByGUID( OtherObj->GUID() , sizeof( Packet ) , &Packet );
			}
		}   		
	}
   
}

void NetSrv_Move::S_SetNPCIdleMotion( BaseItemObject* pObj, int iMotionID )
{
	if( pObj == NULL )	return;

	PG_Move_LtoC_SetIdleMotion Packet;

	Packet.iObjID		= pObj->GUID();
	Packet.iMotionID	= iMotionID;

	RoleDataEx* Owner = pObj->Role();
	
	Owner->TempData.iDefIdleAction = iMotionID;

	if( pObj->Role()->BaseData.Mode.IsAllZoneVisible )
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

			Global::SendToCli_ByGUID( OtherObj->GUID() , sizeof( Packet ) , &Packet );
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
				if( OtherObj == NULL || OtherObj->GUID() == pObj->GUID() )
					continue;
				if( OtherObj->Role()->IsNPC() != false )
					continue;

				Global::SendToCli_ByGUID( OtherObj->GUID() , sizeof( Packet ) , &Packet );
			}
		}   
	}

}

//回送Ping
void NetSrv_Move::S_Ping( int SockID , int RetTime )
{
	PG_Move_LtoC_Ping Send;
	Send.Time = RetTime;
	Global::SendToCli( SockID , sizeof( Send ) , &Send );
}

//通知Server認定Client需要Loading 等待Client 回應
void	NetSrv_Move::S_ClientLoading( int SendID , float CameraX , float CameraY , float CameraZ  )
{
	PG_Move_LtoC_ClientLoading Send;
	Send.CameraX = CameraX;
	Send.CameraY = CameraY;
	Send.CameraZ = CameraZ;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_Move::SendObjMinimapIcon ( int SendID, int iIconID )
{
	RoleDataEx* pRole = Global::GetRoleDataByGUID( SendID );

	if( pRole )
	{
		pRole->TempData.iMinimapIcon = iIconID;

		PG_Move_LtoC_SetMinimapIcon Packet;

		Packet.iObjID	= SendID;
		Packet.iIconID	= iIconID;

		// 發送給物件附近的 Player

		PlayIDInfo**	Block = Global::PartSearch( pRole , _Def_View_Block_Range_ );

		int			i , j;
		PlayID*		TopID;

		for( i = 0 ; Block[i] != NULL ; i++ )
		{
			TopID = Block[i]->Begin;

			for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
			{
				BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
				if( OtherObj == NULL || OtherObj->GUID() == SendID )
					continue;

				if( OtherObj->Role()->IsNPC() != false )
					continue;

				//DoAction( OtherObj->GUID() , GItemID , ActionID , TempAction );
				Global::SendToCli_ByGUID( OtherObj->GUID(), sizeof( PG_Move_LtoC_SetMinimapIcon ), &Packet );
			}
		}   
	}
}

void NetSrv_Move::S_SpecialEfficacy ( int SendID , int GItemID , SpecialEfficacyTypeENUM EffectType )
{
	PG_Move_LtoC_SpecialEfficacy	Send;
	Send.EffectType = EffectType;
	Send.ItemGUID   = GItemID;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

//設定方向
void NetSrv_Move::S_SetDir( int SendID , int GItemID , float Dir )
{
	PG_Move_LtoC_SetDir Send;
	Send.GItemID = GItemID;
	Send.Dir = Dir;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

//設定角色公會ID
void NetSrv_Move::S_SetGuildID( int SendID , int GItemID , int GuildID )
{
	PG_Move_LtoC_SetGuildID Send;
	Send.GItemID = GItemID;
	Send.GuildID = GuildID;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
//設定角色頭銜ID
void NetSrv_Move::S_SetTitleID( int SendID , int GItemID , bool IsShowTitle , int TitleID , const char* TitleStr )
{
	PG_Move_LtoC_SetTitleID Send;
	Send.GItemID = GItemID;
	Send.TitleID = TitleID;
	Send.IsShowTitle = IsShowTitle;
	Send.TitleStr = TitleStr;

	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_Move::S_PlayerDropDownDamage( int SendID , int Damage , int Type )
{
	if( Damage == 0 )
		return;

	PG_Move_LtoC_PlayerDropDownDamage Send;
	Send.Damage = Damage;
	Send.Type = Type;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_Move::S_ObjGuildBuildingInfo( int SendID , int GItemID , int BuildingDBID , int BuildingParentDBID , int BuildingID , int Dir , const char* Point )
{
	PG_Move_LtoC_ObjGuildBuildingInfo Send;
	Send.GItemID = GItemID;
	Send.BuildingDBID = BuildingDBID;
	Send.BuildingParentDBID = BuildingParentDBID;
	Send.BuildingID = BuildingID;
	Send.Dir = Dir;
	MyStrcpy( Send.Point , Point , sizeof(Send.Point) );
	
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_Move::S_ObjGuildFurnitureInfo( int SendID , int GItemID , int FurnitureDBID )
{
	PG_Move_LtoC_ObjGuildFurnitureInfo Send;
	Send.GItemID = GItemID;
	Send.FurnitureDBID = FurnitureDBID;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}


//送座騎顏色
void NetSrv_Move::S_HorseColor( int SendID , int GItemID , int Color[4] )
{
	PG_Move_LtoC_HorseColor Send;
	Send.GItemID = GItemID;
	memcpy( Send.Color , Color , sizeof(Send.Color) );

	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

//通知所再的RoomID
void NetSrv_Move::S_RoomID( int SendID , int RoomID )
{
	PG_Move_LtoC_RoomID Send;
	Send.RoomID = RoomID;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

//PingLog
void NetSrv_Move::S_All_PingLog( int ID , int Time )
{
	PG_Move_LtoC_PingLog Send;
	Send.ID = ID;
	Send.Time = Time;
	Global::SendToAllCli( sizeof(Send) , &Send );
}
void NetSrv_Move::S_DeadZone( int SendID , ZoneTypeENUM Type , int DeadCountDown )
{	
	PG_Move_LtoC_DeadZone Send;
	Send.Type = Type;
	Send.DeadCountDown = DeadCountDown;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_Move::S_AttachObj( int SendID , int GItemID , int AttachItemID , int AttachType , const char* ItemPos , const char* AttachPos , int Pos  )
{
	PG_Move_LtoC_AttachObj Send;
	Send.GItemID = GItemID;
	Send.AttachItemID = AttachItemID;
	Send.AttachType = AttachType;
	Send.AttachPos = AttachPos;
	Send.ItemPos = ItemPos;
	Send.Pos = Pos;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_Move::S_DetachObj( int SendID , int GItemID )
{
	PG_Move_LtoC_DetachObj Send;
	Send.GItemID = GItemID;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

//-----------------------------------------------------------------------------------------
//戰場額外的封包
//-----------------------------------------------------------------------------------------
//新建立的玩家角色
void NetSrv_Move::S_BattleGround_AddPlayer( int SendID , RoleDataEx* Role )
{
	PG_Move_LtoC_BattleGround_AddPlayer Send;
	Send.Info.GItemID = Role->GUID();
	Send.Info.DBID	  = Role->DBID();	
	Send.Info.WorldID = Role->TempData.iWorldID;
	Send.Info.RoleName = Role->RoleName();
	Send.Info.X = Role->X();
	Send.Info.Y = Role->Y();
	Send.Info.Z = Role->Z();

	Send.Info.HP = Role->BaseData.HP;
	Send.Info.MP = Role->BaseData.MP;
	Send.Info.SP = Role->BaseData.SP;
	Send.Info.SP_Sub = Role->BaseData.SP_Sub;
	Send.Info.MaxHP = Role->MaxHP();
	Send.Info.MaxMP = Role->MaxMP();

	Send.Info.Race		= Role->BaseData.Race;
	Send.Info.Job		= Role->BaseData.Voc;
	Send.Info.Job_Sub	= Role->BaseData.Voc_Sub;
	Send.Info.Look		= Role->BaseData.Look;

	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
//刪除的玩家角色
void NetSrv_Move::S_BattleGround_DelPlayer( int SendID , int GItemID )
{
	PG_Move_LtoC_BattleGround_DelPlayer Send;
	Send.GItemID = GItemID;
	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

//新建立的玩家角色
void NetSrv_Move::S_AllCli_BattleGround_AddPlayer( RoleDataEx* Role )
{
	PG_Move_LtoC_BattleGround_AddPlayer Send;
	Send.Info.GItemID = Role->GUID();
	Send.Info.DBID	= Role->DBID();
	Send.Info.WorldID = Role->TempData.iWorldID;
	Send.Info.RoleName = Role->RoleName();
	Send.Info.X = Role->X();
	Send.Info.Y = Role->Y();
	Send.Info.Z = Role->Z();

	Send.Info.HP = Role->BaseData.HP;
	Send.Info.MP = Role->BaseData.MP;
	Send.Info.SP = Role->BaseData.SP;
	Send.Info.SP_Sub = Role->BaseData.SP_Sub;
	Send.Info.MaxHP = Role->MaxHP();
	Send.Info.MaxMP = Role->MaxMP();

	Send.Info.Race		= Role->BaseData.Race;
	Send.Info.Job		= Role->BaseData.Voc;
	Send.Info.Job_Sub	= Role->BaseData.Voc_Sub;
	Send.Info.Look		= Role->BaseData.Look;

	Global::SendToAllCli(  sizeof( Send ) , &Send );
}
//刪除的玩家角色
void NetSrv_Move::S_AllCli_BattleGround_DelPlayer( int GItemID )
{
	PG_Move_LtoC_BattleGround_DelPlayer Send;
	Send.GItemID = GItemID;
	Global::SendToAllCli( sizeof( Send ) , &Send );
}

//定時 位置的更新
void NetSrv_Move::S_AllCli_BattleGround_PlayerPos( RoleDataEx* Role  )
{
	PG_Move_LtoC_BattleGround_PlayerPos Send;
	Send.GItemID = Role->GUID();
	Send.X = Role->X();
	Send.Y = Role->Y();
	Send.Z = Role->Z();

	//Global::SendToAllCli( sizeof( Send ) , &Send );
	if( Role->IsPlayer() == false )
		return;

	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( Role->BaseData.PartyID );
	if( Party == NULL )
		return;

	for( int i = 0 ; i < (int)Party->Member.size() ; i++ )
	{
		if( Role->DBID() == Party->Member[i].DBID )
			continue;

		Global::SendToCli( Party->Member[i].GItemID , sizeof( Send ) , &Send );
	}
	
}
//定時 血量的更新
void NetSrv_Move::S_AllCli_BattleGround_PlayerHPMP( RoleDataEx* Role  )
{
	PG_Move_LtoC_BattleGround_PlayerHPMP Send;

	Send.HP = Role->BaseData.HP;
	Send.MP = Role->BaseData.MP;
	Send.SP = Role->BaseData.SP;
	Send.SP_Sub = Role->BaseData.SP_Sub;
	Send.MaxHP = Role->MaxHP();
	Send.MaxMP = Role->MaxMP();
	//Global::SendToAllCli( sizeof( Send ) , &Send );
	if( Role->IsPlayer() == false )
		return;

	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( Role->BaseData.PartyID );
	if( Party == NULL )
		return;

	for( int i = 0 ; i < (int)Party->Member.size() ; i++ )
	{
		if( Role->DBID() == Party->Member[i].DBID )
			continue;

		Global::SendToCli( Party->Member[i].GItemID , sizeof( Send ) , &Send );
	}
}

void NetSrv_Move::S_UnlockRolePosOK( int GItemID , bool Result )
{
	PG_Move_LtoC_UnlockRolePosOK Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( GItemID , sizeof( Send ) , &Send );
}

void NetSrv_Move::S_UnlockRolePosTime( int GItemID , int Time )
{
	PG_Move_LtoC_UnlockRolePosTime Send;
	Send.Time = Time;
	Global::SendToCli_ByGUID( GItemID , sizeof( Send ) , &Send );
}

void NetSrv_Move::SC_AttachObjResult( int GItemID , AttachObjRequestENUM Type , AttachObjResultENUM Result , int Pos )
{
	PG_Move_LtoC_AttachObjResult Send;
	Send.Result = Result;
	Send.Type = Type;
	Send.Pos = Pos;
	Global::SendToCli_ByGUID( GItemID , sizeof( Send ) , &Send );
}
//設定花盆資訊
void NetSrv_Move::FlowerPotInfo( int SendToID , int GItemID , ItemFieldStruct& Item )
{
	PG_Move_LtoC_FlowerPotInfo Send;
	Send.GItemID = GItemID;
	Send.Item = Item;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Move::SC_ColdownInfo( int SendToID , int GItemID , MagicColdownStruct& Coldown )
{
	PG_Move_LtoC_ColdownInfo Send;
	Send.GItemID = GItemID;
	Send.Coldown = Coldown;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Move::SC_PlayerPos( vector<PlayerBaseInfoStruct> List )
{
	PG_Move_LtoC_PlayerPos Send;
	Send.Size = (int)List.size();
	for( int i = 0 ; i < Send.Size ; i++ )
	{
		Send.Info[i] = List[i];
	}
	for( int i = 0 ; i < Send.Size ; i++ )
	{
		Global::SendToCli_ByDBID( List[i].DBID , Send.PGSize() , &Send );
	}
}
//////////////////////////////////////////////////////////////////////////
//通知所有玩家 此區域玩家的相關資料
void NetSrv_Move::SC_BattleGround_PlayerInfo_Zip( int RoomID , vector<BattleGroundPlayerHPMPBaseStruct>& List )
{
	if( List.size() > 500 )
		return;

	BattleGroundPlayerHPMPBaseStruct Data[500];
	for( unsigned i = 0 ; i < List.size() ; i++ )
		Data[i] = List[i];

	MyLzoClass myZip;
	PG_Move_LtoC_BattleGround_PlayerInfo_Zip Send;
	Send.Count = (int)List.size();
	Send.Size = myZip.Encode( (char*)Data , sizeof( BattleGroundPlayerHPMPBaseStruct ) * Send.Count , (char*)&Send.Data );
	Global::SendToCli_ByRoomID( RoomID , Send.PGSize() , &Send );
}
void NetSrv_Move::SC_FixPlayerPos( vector<RoleDataEx*>& toList , vector<RoleDataEx*>& moveList )
{
	map< int , vector< PlayerPosSimpleStruct > > moveInfo;

	PG_Move_LtoC_AllPlayerPos send;
	send.PlayerCount = int( __min( 1000 , (int)moveList.size() ) );
	for( int i = 0 ; i < send.PlayerCount ; i++ )
	{
		RoleDataEx* role = moveList[i];
		send.List[i].GUID = role->GUID();
		send.List[i].X = (int)role->X();
		send.List[i].Y = (int)role->Y();
		send.List[i].Z = (int)role->Z();
		send.List[i].Dir = role->Dir();
	}

	for( unsigned i = 0 ; i < toList.size() ; i++ )
	{
		Global::SendToCli( toList[i]->SockID() , send.PGSize() , &send );
	}

	//MProc()->SendToCli( cliID , sizeof(send) , &send );


}