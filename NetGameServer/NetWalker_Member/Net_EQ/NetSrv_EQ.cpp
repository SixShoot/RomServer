#include "../NetWakerGSrvInc.h"
#include "NetSrv_EQ.h"
//-------------------------------------------------------------------
NetSrv_EQ*    NetSrv_EQ::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_EQ::_Init()
{
    Srv_NetCliReg( PG_EQ_CtoL_SetEQ			);
	Srv_NetCliReg( PG_EQ_CtoL_SetShowEQ		);
	Srv_NetCliReg( PG_EQ_CtoL_SwapBackupEQ	);

	Srv_NetCliReg( PG_EQ_CtoL_LearnSuitSkill_Close	);
	Srv_NetCliReg( PG_EQ_CtoL_LearnSuitSkillRequest	);
	Srv_NetCliReg( PG_EQ_CtoL_SetSuitSkillRequest	);
	Srv_NetCliReg( PG_EQ_CtoL_UseSuitSkillRequest	);

    return false;
}
//-------------------------------------------------------------------
bool NetSrv_EQ::_Release()
{
    return false;
}
//-------------------------------------------------------------------
void NetSrv_EQ::_PG_EQ_CtoL_LearnSuitSkill_Close	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_EQ_CtoL_LearnSuitSkill_Close* PG = (PG_EQ_CtoL_LearnSuitSkill_Close*)Data;
	This->R_LearnSuitSkill_Close( Obj );
}
void NetSrv_EQ::_PG_EQ_CtoL_LearnSuitSkillRequest	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_EQ_CtoL_LearnSuitSkillRequest* PG = (PG_EQ_CtoL_LearnSuitSkillRequest*)Data;
	This->R_LearnSuitSkillRequest( Obj , PG->ItemCount , PG->ItemPos , PG->LearnSkillID );
}
void NetSrv_EQ::_PG_EQ_CtoL_SetSuitSkillRequest		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_EQ_CtoL_SetSuitSkillRequest* PG = (PG_EQ_CtoL_SetSuitSkillRequest*)Data;
	This->R_SetSuitSkillRequest( Obj , PG->SkillID , PG->Pos );
}
void NetSrv_EQ::_PG_EQ_CtoL_UseSuitSkillRequest		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_EQ_CtoL_UseSuitSkillRequest* PG = (PG_EQ_CtoL_UseSuitSkillRequest*)Data;
	This->R_UseSuitSkillRequest( Obj , PG->SkillID , PG->TargetID , PG->PosX , PG->PosY , PG->PosZ );
}

void NetSrv_EQ::_PG_EQ_CtoL_SwapBackupEQ	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_EQ_CtoL_SwapBackupEQ* PG = (PG_EQ_CtoL_SwapBackupEQ*)Data;
	This->R_SwapBackupEQ( Obj , PG->BackupPosID );
}
void NetSrv_EQ::_PG_EQ_CtoL_SetShowEQ	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_EQ_CtoL_SetShowEQ* PG = (PG_EQ_CtoL_SetShowEQ*)Data;
	This->R_SetShowEQ( Obj , PG->ShowEQ );
}
void NetSrv_EQ::_PG_EQ_CtoL_SetEQ   ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;
    PG_EQ_CtoL_SetEQ* PG = (PG_EQ_CtoL_SetEQ*)Data;


    switch( PG->PosType )
    {
    case Def_Item_Pos_Body:
        This->R_SetEQ_Body( Obj , PG->Item , PG->ItemPos , PG->EqPos , PG->IsFromEq );
        break;
    case Def_Item_Pos_Bank:
        This->R_SetEQ_Bank( Obj , PG->Item , PG->ItemPos , PG->EqPos , PG->IsFromEq );    
        break;
    case Def_Item_Pos_EQ:
        This->R_SetEQ_EQ( Obj , PG->Item , PG->ItemPos , PG->EqPos );    
        break;

    }
    

}
//-------------------------------------------------------------------
void NetSrv_EQ::S_SetEQResult		( int SendToID , EQResult_ENUM Result )
{
	PG_EQ_LtoC_SetEQResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_EQ::S_SetEQ_OK_Body(  int SendToID , ItemFieldStruct& Item , int ItemPos , ItemFieldStruct& EQ , int EQPos )
{
    NetSrv_Item::FixItemInfo_Body ( SendToID , Item , ItemPos );
    NetSrv_Item::FixItemInfo_EQ   ( SendToID , EQ   , EQPos);

    PG_EQ_LtoC_SetEQResult Send;
    Send.Result = ENMU_EQ_SetEqOK;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
void NetSrv_EQ::S_SetEQ_Failed_Body(  int SendToID , ItemFieldStruct& Item , int ItemPos , ItemFieldStruct& EQ , int EQPos )
{
    NetSrv_Item::FixItemInfo_Body ( SendToID , Item , ItemPos );
    NetSrv_Item::FixItemInfo_EQ   ( SendToID , EQ   , EQPos);

    PG_EQ_LtoC_SetEQResult Send;
    Send.Result = ENMU_EQ_SetEqFailed;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
void NetSrv_EQ::S_SetEQ_OK_Bank(  int SendToID , ItemFieldStruct& Item , int ItemPos , ItemFieldStruct& EQ , int EQPos )
{
	if( Global::Ini()->IsHouseMoveItemLog == true )
	{//銀行和裝備交換
		char Buf[128];
		RoleDataEx* Player = Global::GetRoleDataByGUID( SendToID );
		if( Item.OrgObjID && EQ.OrgObjID )
		{//從銀行和裝備交換
			sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d Bank_%d To EQ_%d" , EQ.OrgObjID , EQ.Count , ItemPos , EQPos );
			Log_House( Player , -1 , -1 , EM_HouseActionType_Item_BankToEq , Buf );

			sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d EQ_%d To Bank_%d" , Item.OrgObjID , Item.Count , EQPos , ItemPos );
			Log_House( Player , -1 , -1 , EM_HouseActionType_Item_EqToBank , Buf );
		}
		else if( EQ.OrgObjID )
		{//從銀行移到裝備
			sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d Bank_%d To EQ_%d" , EQ.OrgObjID , EQ.Count , ItemPos , EQPos );
			Log_House( Player , -1 , -1 , EM_HouseActionType_Item_BankToEq , Buf );
		}
		else
		{//從裝備移到銀行
			sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d EQ_%d To Bank_%d" , Item.OrgObjID , Item.Count , EQPos , ItemPos );
			Log_House( Player , -1 , -1 , EM_HouseActionType_Item_EqToBank , Buf );
		}

		Player->Msg( Buf );
	}
	NetSrv_Item::FixItemInfo_Bank ( SendToID , Item , ItemPos );
    NetSrv_Item::FixItemInfo_EQ   ( SendToID , EQ   , EQPos);

    PG_EQ_LtoC_SetEQResult Send;
    Send.Result = ENMU_EQ_SetEqOK;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
void NetSrv_EQ::S_SetEQ_Failed_Bank(  int SendToID , ItemFieldStruct& Item , int ItemPos , ItemFieldStruct& EQ , int EQPos )
{
    NetSrv_Item::FixItemInfo_Bank ( SendToID , Item , ItemPos );
    NetSrv_Item::FixItemInfo_EQ   ( SendToID , EQ   , EQPos);

    PG_EQ_LtoC_SetEQResult Send;
    Send.Result = ENMU_EQ_SetEqFailed;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
void NetSrv_EQ::S_SetEQ_OK_EQ(  int SendToID , ItemFieldStruct& Item , int ItemPos , ItemFieldStruct& EQ , int EQPos )
{
    NetSrv_Item::FixItemInfo_EQ   ( SendToID , Item , ItemPos );
    NetSrv_Item::FixItemInfo_EQ   ( SendToID , EQ   , EQPos);

    PG_EQ_LtoC_SetEQResult Send;
    Send.Result = ENMU_EQ_SetEqOK;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
void NetSrv_EQ::S_SetEQ_Failed_EQ(  int SendToID , ItemFieldStruct& Item , int ItemPos , ItemFieldStruct& EQ , int EQPos )
{
    NetSrv_Item::FixItemInfo_EQ   ( SendToID , Item , ItemPos );
    NetSrv_Item::FixItemInfo_EQ   ( SendToID , EQ   , EQPos);

    PG_EQ_LtoC_SetEQResult Send;
    Send.Result = ENMU_EQ_SetEqFailed;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
void NetSrv_EQ::S_EQBroken			( int SendToID , EQWearPosENUM Pos )
{
	PG_EQ_LtoC_EQBroken Send;
	Send.Pos = Pos;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_EQ::S_ShowEQInfo		( int SendToID , int GItemID , ShowEQStruct ShowEQ )
{
	PG_EQ_LtoC_SetShowEQ Send;
	Send.GItemID = GItemID;
	Send.ShowEQ._Value = ShowEQ._Value;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_EQ::S_SwapBackupEQ		( int SendToID , int BkPosID , bool Result )
{
	PG_EQ_LtoC_SwapBackupEQ Send;
	Send.BackupPosID = BkPosID;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_EQ::S_LearnSuitSkill_Open	( int SendToID , int TargetID )
{
	PG_EQ_LtoC_LearnSuitSkill_Open Send;
	Send.TargetID = TargetID;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_EQ::S_LearnSuitSkillResult	( int SendToID , int LearnSkillID , LearnSuitSkillResultENUM Result )
{
	PG_EQ_LtoC_LearnSuitSkillResult Send;
	Send.LearnSkillID = LearnSkillID;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_EQ::S_SetSuitSkillResult	( int SendToID , SetSuitSkillResultENUM Result )
{
	PG_EQ_LtoC_SetSuitSkillResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_EQ::S_UseSuitSkillResult	( int SendToID , UseSuitSkillResultENUM Result )
{
	PG_EQ_LtoC_UseSuitSkillResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}