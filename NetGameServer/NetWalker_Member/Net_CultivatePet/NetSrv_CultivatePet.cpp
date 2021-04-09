#include "../NetWakerGSrvInc.h"
#include "NetSrv_CultivatePet.h"
//-------------------------------------------------------------------
NetSrv_CultivatePet*    NetSrv_CultivatePet::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_CultivatePet::_Init()
{
	Srv_NetCliReg ( PG_CultivatePet_CtoL_PushPet		);
	Srv_NetCliReg ( PG_CultivatePet_CtoL_PopPet			);
	Srv_NetCliReg ( PG_CultivatePet_CtoL_PetName		);
	Srv_NetCliReg ( PG_CultivatePet_CtoL_PetEvent		);
	Srv_NetCliReg ( PG_CultivatePet_CtoL_LearnSkill		);
	Srv_NetCliReg ( PG_CultivatePet_CtoL_PetLifeSkill	);
	Srv_NetCliReg ( PG_CultivatePet_CtoL_SwapItem		);
	Srv_NetCliReg ( PG_CultivatePet_CtoL_GetProductRequest);
	Srv_NetCliReg ( PG_CultivatePet_CtoL_PetMarge		);
	Srv_NetCliReg (  PG_CultivatePet_CtoL_RegPetCard 	);
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_CultivatePet::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_CultivatePet::_PG_CultivatePet_CtoL_RegPetCard( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_CultivatePet_CtoL_RegPetCard* pg = (PG_CultivatePet_CtoL_RegPetCard*)Data;
	This->RC_RegPetCard( Obj , pg->PetPos );
}
void NetSrv_CultivatePet::_PG_CultivatePet_CtoL_PetMarge			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_CultivatePet_CtoL_PetMarge* PG = (PG_CultivatePet_CtoL_PetMarge*)Data;
	This->RC_PetMarge( Obj , PG->PetCount , PG->PetPos );
}

void NetSrv_CultivatePet::_PG_CultivatePet_CtoL_GetProductRequest	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_CultivatePet_CtoL_GetProductRequest* PG = (PG_CultivatePet_CtoL_GetProductRequest*)Data;
	This->RC_GetProductRequest( Obj , PG->PetPos );
}

void NetSrv_CultivatePet::_PG_CultivatePet_CtoL_PushPet		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_CultivatePet_CtoL_PushPet* PG = (PG_CultivatePet_CtoL_PushPet*)Data;
	This->RC_PushPet( Obj , PG->PetPos , PG->ItemPos );

}
void NetSrv_CultivatePet::_PG_CultivatePet_CtoL_PopPet		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_CultivatePet_CtoL_PopPet* PG = (PG_CultivatePet_CtoL_PopPet*)Data;
	This->RC_PopPet( Obj , PG->PetPos , PG->ItemPos );
}
void NetSrv_CultivatePet::_PG_CultivatePet_CtoL_PetName		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_CultivatePet_CtoL_PetName* PG = (PG_CultivatePet_CtoL_PetName*)Data;
	This->RC_PetName( Obj , PG->PetPos , PG->Name.Begin() );
}
void NetSrv_CultivatePet::_PG_CultivatePet_CtoL_PetEvent	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_CultivatePet_CtoL_PetEvent* PG = (PG_CultivatePet_CtoL_PetEvent*)Data;
	This->RC_PetEvent( Obj , PG->PetPos , PG->ItemPos , PG->Event );
}
void NetSrv_CultivatePet::_PG_CultivatePet_CtoL_LearnSkill	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_CultivatePet_CtoL_LearnSkill* PG = (PG_CultivatePet_CtoL_LearnSkill*)Data;
	This->RC_LearnSkill( Obj , PG->PetPos , PG->MagicID , PG->MagicLv );
}
void NetSrv_CultivatePet::_PG_CultivatePet_CtoL_PetLifeSkill( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_CultivatePet_CtoL_PetLifeSkill* PG = (PG_CultivatePet_CtoL_PetLifeSkill*)Data;
	This->RC_PetLifeSkill( Obj , PG->PetPos , PG->SkillType , PG->Lv , PG->TablePos );
}
void NetSrv_CultivatePet::_PG_CultivatePet_CtoL_SwapItem	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_CultivatePet_CtoL_SwapItem* PG = (PG_CultivatePet_CtoL_SwapItem*)Data;
	This->RC_SwapItem( Obj , PG->PetPos , PG->PetItemPos , PG->BodyPos , PG->IsFromBody	);
}


void	NetSrv_CultivatePet::SC_PushPetResultOK	( int SendToID , int PetPos , CultivatePetStruct& PetInfo )
{
	PG_CultivatePet_LtoC_PushPetResult Send;
	Send.PetPos = PetPos;
	Send.PetInfo = PetInfo;
	Send.Result = true;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void	NetSrv_CultivatePet::SC_PushPetResultFailed( int SendToID  )
{
	PG_CultivatePet_LtoC_PushPetResult Send;
	Send.Result = false;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void	NetSrv_CultivatePet::SC_PopPetResult	( int SendToID , bool Result , int ItemPos )
{
	PG_CultivatePet_LtoC_PopPetResult Send;
	Send.Result = Result;
	Send.ItemPos = ItemPos;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void	NetSrv_CultivatePet::SC_PetNameResult	( int SendToID , bool Result , const char* Name )
{
	PG_CultivatePet_LtoC_PetNameResult Send;
	Send.Result = Result;
	if( Name != NULL )
		Send.Name = Name;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void	NetSrv_CultivatePet::SC_PetEventResult	( int SendToID , int PetPos , CultivatePetCommandTypeENUM OrgEvent , CultivatePetCommandTypeENUM Event , int EventTime , bool Result )
{
	PG_CultivatePet_LtoC_PetEventResult Send;
	Send.Result 	= Result;
	Send.PetPos 	= PetPos;
	Send.OrgEvent	= OrgEvent;
	Send.Event		= Event;
	Send.EventTime	= EventTime;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void	NetSrv_CultivatePet::SC_LearnSkillResult( int SendToID , int PetPos , int MagicID , int MagicLv , bool Result )
{
	PG_CultivatePet_LtoC_LearnSkillResult Send;
	Send.Result 	= Result;
	Send.PetPos 	= PetPos;
	Send.MagicID	= MagicID;
	Send.MagicLv	= MagicLv;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void	NetSrv_CultivatePet::SC_PetLifeResult	( int SendToID , CultivatePetLifeSkillTypeENUM	SkillType , int PetPos , int EventTime , bool Result )
{
	PG_CultivatePet_LtoC_PetLifeResult Send;
	Send.Result 	= Result;
	Send.SkillType 	= SkillType;
	Send.EventTime	= EventTime;
	Send.PetPos		= PetPos;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void	NetSrv_CultivatePet::SC_SwapItemResult	( int SendToID , bool Result )
{
	PG_CultivatePet_LtoC_SwapItemResult Send;
	Send.Result 	= Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void	NetSrv_CultivatePet::SC_FixItem			( int SendToID , int PetPos , int PetItemPos , ItemFieldStruct& Item )
{
	PG_CultivatePet_LtoC_FixItem Send;
	Send.PetPos 	= PetPos;
	Send.PetItemPos	= PetItemPos;
	Send.Item		= Item;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void	NetSrv_CultivatePet::SC_FixPetInfo			( int SendToID , int PetPos , CultivatePetStruct&	PetBase )
{
	PG_CultivatePet_LtoC_FixPetBase Send;
	Send.PetPos 	= PetPos;
	Send.PetBase	= PetBase;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_CultivatePet::SC_GetProductResult		( int SendToID , bool Result )
{
	PG_CultivatePet_LtoC_GetProductResult Send;
	Send.Result 	= Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_CultivatePet::SC_PetEventProcess		( int SendToID , int PetPos , CultivatePetCommandTypeENUM Event )
{
	PG_CultivatePet_LtoC_PetEventProcess Send;
	Send.Event 	= Event;
	Send.PetPos = PetPos;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );

}
void NetSrv_CultivatePet::SC_PetMargeResultOK		( int SendToID , int PetCount , int PetPos[3] , CultivatePetStruct& PetBase )
{
	PG_CultivatePet_LtoC_PetMargeResult Send;
	Send.PetBase = PetBase;
	Send.PetCount = PetCount;
	memcpy( Send.PetPos , PetPos , sizeof(Send.PetPos) );
	Send.Result = EM_PetMargeResult_OK;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_CultivatePet::SC_PetMargeResultFailed	( int SendToID , PetMargeResultENUM Result )
{
	PG_CultivatePet_LtoC_PetMargeResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_CultivatePet::SC_RegPetCardResult( int SendToID  , bool result )
{
	PG_CultivatePet_LtoC_RegPetCardResult Send;
	Send.Result = result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
