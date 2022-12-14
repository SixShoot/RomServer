#include "../NetWakerGSrvInc.h"
#include "NetSrv_Item.h"
//-------------------------------------------------------------------
NetSrv_Item*    NetSrv_Item::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_Item::_Init()
{
    Srv_NetCliReg( PG_Item_CtoL_GetGroundItem                );
    Srv_NetCliReg( PG_Item_CtoL_GetBufferItem                );
    Srv_NetCliReg( PG_Item_CtoL_DeleteItem                   );
    Srv_NetCliReg( PG_Item_CtoL_ExchangeField                );
    Srv_NetCliReg( PG_Item_CtoL_RequestBodyBankMoneyExchange );
    Srv_NetCliReg( PG_Item_CtoL_RequestBufferInfo            );
    Srv_NetCliReg( PG_Item_CtoL_ApartBodyItemRequest		 );
    Srv_NetCliReg( PG_Item_CtoL_RequestFixAllItemInfo        );
    Srv_NetCliReg( PG_Item_CtoL_CloseBank                    );
    Srv_NetCliReg( PG_Item_CtoL_UseItem                      );
	Srv_NetCliReg( PG_Item_CtoL_UseItemToItem_Notify         );
	Srv_NetCliReg( PG_Item_CtoL_ItemDissolutionRequest       );
	Srv_NetCliReg( PG_Item_CtoL_TakeOutCardRequest		     );
	Srv_NetCliReg( PG_Item_CtoL_EQCombinRequest			     );
	Srv_NetCliReg( PG_Item_CtoL_EQCombinClose			     );

	_Net->RegOnSrvPacketFunction( EM_PG_Item_DtoL_AccountBagInfoResult, _PG_Item_DtoL_AccountBagInfoResult );
		
	Srv_NetCliReg( PG_Item_CtoL_AccountBagGetItemRequest	 );
	Srv_NetCliReg( PG_Item_CtoL_AccountBagClose			     );
	Srv_NetCliReg( PG_Item_CtoL_MagicBoxRequest			     );

	Srv_NetCliReg( PG_Item_CtoL_GamebleItemState		     );
	Srv_NetCliReg( PG_Item_CtoL_GetGamebleItemState		     );
	Srv_NetCliReg( PG_Item_CtoL_CloseGamebleItemState	     );

	Srv_NetCliReg( PG_Item_CtoL_MagicStoneExchangeResult     );
	Srv_NetCliReg( PG_Item_CtoL_DrawOutRuneRequest			 );

	Srv_NetCliReg( PG_Item_CtoL_PutCoolClothRequest			 );
	Srv_NetCliReg( PG_Item_CtoL_DelCoolClothRequest			 );
	Srv_NetCliReg( PG_Item_CtoL_SetCoolSuitRequest			 );
	Srv_NetCliReg( PG_Item_CtoL_ShowCoolSuitIndexRequest	 );

	Srv_NetCliReg( PG_Item_CtoL_PutInExBagRequest			 );
	Srv_NetCliReg( PG_Item_CtoL_PopExBagRequest				 );
	Srv_NetCliReg( PG_Item_CtoL_UseExBagRequest				 );

	Srv_NetCliReg( PG_Item_CtoL_SwapCoolClothRequest		 );

	Srv_NetCliReg( PG_Item_CtoL_Rare3EqExchangeItemRequest	 );
	Srv_NetCliReg( PG_Item_CtoL_CloseRare3EqExchangeItem	 );

	Srv_NetCliReg( PG_Item_CtoL_ClsAttribute				 );
	Srv_NetCliReg( PG_Item_CtoL_MagicBoxAttrDrawOff			 );
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Item::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_Item::_PG_Item_CtoL_MagicBoxAttrDrawOff( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_MagicBoxAttrDrawOff* PG = (PG_Item_CtoL_MagicBoxAttrDrawOff*) Data;
	This->RC_MagicBoxAttrDrawOff( Obj );
}
void NetSrv_Item::_PG_Item_CtoL_ClsAttribute( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_ClsAttribute* PG = (PG_Item_CtoL_ClsAttribute*) Data;
	This->RC_ClsAttribute( Obj , PG->Type , PG->Pos , PG->AttributeID );
}
void NetSrv_Item::_PG_Item_CtoL_Rare3EqExchangeItemRequest( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_Rare3EqExchangeItemRequest* PG = (PG_Item_CtoL_Rare3EqExchangeItemRequest*) Data;
	This->RC_Rare3EqExchangeItemRequest( Obj , PG->Type , PG->ItemPos );
}
void NetSrv_Item::_PG_Item_CtoL_CloseRare3EqExchangeItem( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_CloseRare3EqExchangeItem* PG = (PG_Item_CtoL_CloseRare3EqExchangeItem*) Data;
	This->RC_CloseRare3EqExchangeItem( Obj );
}

void NetSrv_Item::_PG_Item_CtoL_SwapCoolClothRequest( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_SwapCoolClothRequest* PG = (PG_Item_CtoL_SwapCoolClothRequest*) Data;
	This->RC_SwapCoolClothRequest( Obj , PG->Pos );
}
void NetSrv_Item::_PG_Item_CtoL_PopExBagRequest( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_PopExBagRequest* PG = (PG_Item_CtoL_PopExBagRequest*) Data;
	This->RC_PopExBagRequest( Obj , PG->ExBagType , PG->ExBagPos , PG->BodyPos );
}
void NetSrv_Item::_PG_Item_CtoL_PutInExBagRequest( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_PutInExBagRequest* PG = (PG_Item_CtoL_PutInExBagRequest*) Data;
	This->RC_PutInExBagRequest( Obj , PG->ExBagType , PG->ItemID , PG->BodyPos );
}
void NetSrv_Item::_PG_Item_CtoL_UseExBagRequest( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_UseExBagRequest* PG = (PG_Item_CtoL_UseExBagRequest*) Data;
	This->RC_UseExBagRequest( Obj , PG->ExBagType , PG->BagPos );
}

void NetSrv_Item::_PG_Item_CtoL_PutCoolClothRequest			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_PutCoolClothRequest* PG = (PG_Item_CtoL_PutCoolClothRequest*) Data;
	This->RC_PutCoolClothRequest( Obj , PG->BodyPos , PG->CoolClothPos  );
}
void NetSrv_Item::_PG_Item_CtoL_DelCoolClothRequest			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_DelCoolClothRequest* PG = (PG_Item_CtoL_DelCoolClothRequest*) Data;
	This->RC_DelCoolClothRequest( Obj , PG->Pos , PG->ImageObjectID );
}
void NetSrv_Item::_PG_Item_CtoL_SetCoolSuitRequest			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_SetCoolSuitRequest* PG = (PG_Item_CtoL_SetCoolSuitRequest*) Data;
	This->RC_SetCoolSuitRequest( Obj , PG->SuitIndexID , PG->Info );
}
void NetSrv_Item::_PG_Item_CtoL_ShowCoolSuitIndexRequest	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_ShowCoolSuitIndexRequest* PG = (PG_Item_CtoL_ShowCoolSuitIndexRequest*) Data;
	This->RC_ShowCoolSuitIndexRequest( Obj , PG->CoolSuitIndexID );
}
void NetSrv_Item::_PG_Item_CtoL_DrawOutRuneRequest			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_DrawOutRuneRequest* PG = (PG_Item_CtoL_DrawOutRuneRequest*) Data;
	This->RC_DrawOutRuneRequest( Obj , PG->BodyPos , PG->RuneID );
}

void NetSrv_Item::_PG_Item_CtoL_MagicStoneExchangeResult	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_MagicStoneExchangeResult* PG = (PG_Item_CtoL_MagicStoneExchangeResult*) Data;
	This->RC_MagicStoneExchangeResult( Obj , PG->AbilityID , PG->Result );
}
void NetSrv_Item::_PG_Item_CtoL_CloseGamebleItemState	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	//PG_Item_CtoL_CloseGamebleItemState* PG = (PG_Item_CtoL_CloseGamebleItemState*) Data;
	This->RC_CloseGamebleItemState( Obj );
}

void NetSrv_Item::_PG_Item_CtoL_GamebleItemState	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_GamebleItemState* PG = (PG_Item_CtoL_GamebleItemState*) Data;
	This->RC_GamebleItemState( Obj , PG->TableID , PG->ItemPos , PG->BodyItem , PG->LockFlag );
}
void NetSrv_Item::_PG_Item_CtoL_GetGamebleItemState	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_GetGamebleItemState* PG = (PG_Item_CtoL_GetGamebleItemState*) Data;
	This->RC_GetGamebleItemState( Obj , PG->GiveUp );
}

void NetSrv_Item::_PG_Item_CtoL_MagicBoxRequest				( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_MagicBoxRequest* PG = (PG_Item_CtoL_MagicBoxRequest*) Data;
	This->RC_MagicBoxRequest( Obj );
}
/*
void NetSrv_Item::_PG_Item_CtoL_SmeltEqtoMagicStoneRequest	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_SmeltEqtoMagicStoneRequest* PG = (PG_Item_CtoL_SmeltEqtoMagicStoneRequest*) Data;
	This->RC_SmeltEqtoMagicStoneRequest( Obj , PG->EQPos , PG->SmeltItemPos , PG->BoxPos );
}
void NetSrv_Item::_PG_Item_CtoL_RefineMagicStoneRequest		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_RefineMagicStoneRequest* PG = (PG_Item_CtoL_RefineMagicStoneRequest*) Data;
	This->RC_RefineMagicStoneRequest( Obj , PG->MagicStonePos , PG->BoxPos );
}
void NetSrv_Item::_PG_Item_CtoL_CombinEQandMagicStoneRequest( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_CombinEQandMagicStoneRequest* PG = (PG_Item_CtoL_CombinEQandMagicStoneRequest*) Data;
	This->RC_CombinEQandMagicStoneRequest( Obj , PG->MagicStonePos , PG->EQPos , PG->BoxPos );
}
void NetSrv_Item::_PG_Item_CtoL_RuneTransferRequest		( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_RuneTransferRequest* PG = (PG_Item_CtoL_RuneTransferRequest*) Data;
	This->RC_RuneTransferRequest( Obj , PG->RunePos , PG->BoxPos );
}
void NetSrv_Item::_PG_Item_CtoL_RefineRuneRequest			( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;

	PG_Item_CtoL_RefineRuneRequest* PG = (PG_Item_CtoL_RefineRuneRequest*) Data;
	This->RC_RefineRuneRequest( Obj , PG->RunePos , PG->BoxPos );
}
*/
void NetSrv_Item::_PG_Item_DtoL_AccountBagInfoResult		( int NetID , int Size , void* Data )
{
	PG_Item_DtoL_AccountBagInfoResult* PG = (PG_Item_DtoL_AccountBagInfoResult*) Data;
	This->RD_AccountBagInfoResult( PG->PlayerDBID , PG->MaxCount , PG->ItemDBID , PG->Item );
}
void NetSrv_Item::_PG_Item_CtoL_AccountBagGetItemRequest	( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Item_CtoL_AccountBagGetItemRequest* PG = (PG_Item_CtoL_AccountBagGetItemRequest*)Data;
	This->RC_AccountBagGetItemRequest( Obj , PG->ItemDBID , PG->BodyPos );
}
void NetSrv_Item::_PG_Item_CtoL_AccountBagClose				( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Item_CtoL_AccountBagClose* PG = (PG_Item_CtoL_AccountBagClose*)Data;
	This->RC_AccountBagClose( Obj );
}

void NetSrv_Item::_PG_Item_CtoL_EQCombinClose				( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Item_CtoL_EQCombinClose* PG = (PG_Item_CtoL_EQCombinClose*)Data;
	This->R_EQCombinClose( Obj );
}
void NetSrv_Item::_PG_Item_CtoL_EQCombinRequest				( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Item_CtoL_EQCombinRequest* PG = (PG_Item_CtoL_EQCombinRequest*)Data;
	This->R_EQCombinRequest( Obj , PG->CombinItemPos , PG->CheckItemPos );
}

void NetSrv_Item::_PG_Item_CtoL_TakeOutCardRequest		  ( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Item_CtoL_TakeOutCardRequest* PG = (PG_Item_CtoL_TakeOutCardRequest*)Data;
	This->R_TakeOutCardRequest( Obj , PG->CardObjID );
}
void NetSrv_Item::_PG_Item_CtoL_ItemDissolutionRequest        ( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Item_CtoL_ItemDissolutionRequest* PG = (PG_Item_CtoL_ItemDissolutionRequest*)Data;
	This->R_ItemDissolutionRequest( Obj , PG->Item , PG->Pos );
}

void NetSrv_Item::_PG_Item_CtoL_UseItemToItem_Notify        ( int NetID , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
	if( Obj == NULL )
		return ;
	PG_Item_CtoL_UseItemToItem_Notify* PG = (PG_Item_CtoL_UseItemToItem_Notify*)Data;
	This->R_UseItemToItem_Notify( Obj , PG->UseItem , PG->UseItemPos , PG->TargetItem , PG->TargetItemPos , PG->TargetPosType , (char*)PG->Password.Begin() );
}

void NetSrv_Item::_PG_Item_CtoL_GetGroundItem               ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;
    PG_Item_CtoL_GetGroundItem* PG = (PG_Item_CtoL_GetGroundItem*)Data;
    This->R_GetGroundItem( Obj , PG->ItemID , PG->Pos );
}
void NetSrv_Item::_PG_Item_CtoL_GetBufferItem               ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;
    PG_Item_CtoL_GetBufferItem* PG = (PG_Item_CtoL_GetBufferItem*)Data;
    This->R_GetBufferItem( Obj , PG->Item , PG->Pos , PG->Count );
}
void NetSrv_Item::_PG_Item_CtoL_DeleteItem                  ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;
    PG_Item_CtoL_DeleteItem* PG = (PG_Item_CtoL_DeleteItem*)Data;
    if( PG->Type == Def_Item_Pos_Body  )
        This->R_DeleteItem_Body( Obj , PG->Item , PG->Pos );
    else if( PG->Type == Def_Item_Pos_Bank )
        This->R_DeleteItem_Bank( Obj , PG->Item , PG->Pos );
    else
        This->R_DeleteItem_EQ( Obj , PG->Item , PG->Pos );

}
void NetSrv_Item::_PG_Item_CtoL_ExchangeField               ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;

    PG_Item_CtoL_ExchangeField *PG = (PG_Item_CtoL_ExchangeField*)Data;
    if(		PG->Type[ 0 ] == Def_Item_Pos_Body  
        &&	PG->Type[ 1 ] == Def_Item_Pos_Body )
    {
        This->R_ExchangeField_Body(		    Obj
                                        ,	(PG->Item[0]) , PG->Pos[0] 
                                        ,	(PG->Item[1]) , PG->Pos[1] 
										,	PG->ClientID );
    }
    else if(	PG->Type[ 0 ] == Def_Item_Pos_Bank  
        &&		PG->Type[ 1 ] == Def_Item_Pos_Bank )
    {
        This->R_ExchangeField_Bank(		    Obj
                                        ,	(PG->Item[0]) , PG->Pos[0] 
                                        ,	(PG->Item[1]) , PG->Pos[1]  
										,	PG->ClientID );
    }
    else if(	PG->Type[ 0 ] == Def_Item_Pos_Body
        &&		PG->Type[ 1 ] == Def_Item_Pos_Bank )
    {
        This->R_ExchangeField_BodyBank(		Obj
                                            ,	(PG->Item[0]) , PG->Pos[0] 
                                            ,	(PG->Item[1]) , PG->Pos[1] 
											,	PG->ClientID );
    }
    else if(	PG->Type[ 0 ] == Def_Item_Pos_Bank
        &&		PG->Type[ 1 ] == Def_Item_Pos_Body )
    {
        This->R_ExchangeField_BankBody(		Obj
                                            ,	(PG->Item[0]) , PG->Pos[0] 
                                            ,	(PG->Item[1]) , PG->Pos[1] 
											,	PG->ClientID );
    }
    
}
void NetSrv_Item::_PG_Item_CtoL_RequestBodyBankMoneyExchange( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;

    PG_Item_CtoL_RequestBodyBankMoneyExchange *PG = (PG_Item_CtoL_RequestBodyBankMoneyExchange*)Data;
    
    This->R_RequestBodyBankMoneyExchange( Obj , PG->Count );
}
void NetSrv_Item::_PG_Item_CtoL_RequestBufferInfo           ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;

    PG_Item_CtoL_RequestBufferInfo *PG = (PG_Item_CtoL_RequestBufferInfo*)Data;
    This->R_RequestBufferInfo( Obj );


}
void NetSrv_Item::_PG_Item_CtoL_ApartBodyItemRequest       ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;

    PG_Item_CtoL_ApartBodyItemRequest  *PG = (PG_Item_CtoL_ApartBodyItemRequest *)Data;
    This->R_ApartBodyItemRequest( Obj , PG->Item , PG->PosSrc , PG->PosDes , PG->CountDes );

}
void NetSrv_Item::_PG_Item_CtoL_RequestFixAllItemInfo       ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;

    PG_Item_CtoL_RequestFixAllItemInfo  *PG = (PG_Item_CtoL_RequestFixAllItemInfo *)Data;
    
    This->R_RequestFixAllItemInfo( Obj );
    
}
void NetSrv_Item::_PG_Item_CtoL_CloseBank                   ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;

    PG_Item_CtoL_CloseBank  *PG = (PG_Item_CtoL_CloseBank *)Data;

    This->R_CloseBank( Obj );
}
void NetSrv_Item::_PG_Item_CtoL_UseItem                     ( int NetID , int Size , void* Data )
{
    BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( NetID );
    if( Obj == NULL )
        return ;

    PG_Item_CtoL_UseItem  *PG = (PG_Item_CtoL_UseItem *)Data;

	This->R_UseItem( Obj , PG->Pos , PG->Type , PG->TargetID , PG->TargetX , PG->TargetY , PG->TargetZ , PG->Item );
    
    
}
//--------------------------------------------------------------------------------------------------------------------------
void NetSrv_Item::GetItemOK               ( int SendToID , ItemFieldStruct& Item , short Pos )
{
    FixItemInfo_Body( SendToID , Item , Pos );

    PG_Item_LtoC_GetItemResult Send;
    Send.Result = EM_Item_ItemOK;
	Send.Count  = Item.Count;
	Send.ItemID = Item.OrgObjID;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::GetItemOK               ( int SendToID )
{
	PG_Item_LtoC_GetItemResult Send;
	Send.Result = EM_Item_ItemOK;
	Send.Count = -1;
	Send.ItemID = -1;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::GetItemFailed           ( int SendToID )
{
    PG_Item_LtoC_GetItemResult Send;
    Send.Result = EM_Item_GetGroundItemFailed;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::GetBufferItemOK         ( int SendToID , ItemFieldStruct& Item , short Pos , int Count )
{
    FixItemInfo_Body( SendToID , Item , Pos );

    PG_Item_LtoC_GetItemResult Send;
	Send.Count = Count;
	Send.ItemID = Item.OrgObjID;
    Send.Result = EM_Item_GetItemInBufferOK;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::DelBufferItemOK         ( int SendToID , ItemFieldStruct& Item )
{

	PG_Item_LtoC_GetItemResult Send;
	Send.Count = Item.Count;
	Send.ItemID = Item.OrgObjID;
	Send.Result = EM_Item_DelItemInBufferOK;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::GetBufferItemFailed     ( int SendToID )
{
    PG_Item_LtoC_GetItemResult Send;
    Send.Result = EM_Item_GetItemInBufferFailed;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::ClsItemInBuffer         ( int SendToID  )           
{
    PG_Item_LtoC_ClsItemInBuffer   Send;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::NewItemInBuffer         ( int SendToID , ItemFieldStruct& Item )           
{
    PG_Item_LtoC_NewItemInBuffer   Send;
    Send.Item = Item;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::DeleteItemOK_Body            ( int SendToID , ItemFieldStruct& Item , int Pos )   
{
	ItemFieldStruct EmptyFiled;
	EmptyFiled.Init();
    FixItemInfo_Body( SendToID , EmptyFiled , Pos );

    PG_Item_LtoC_DeleteItem Send;
    Send.Item = Item;
    //Send.Pos = Pos;
    Send.Type = Def_Item_Pos_Body;
    Send.Result = EM_Item_ItemOK;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::DeleteItemOK_Bank            ( int SendToID , ItemFieldStruct& Item , int Pos )   
{
	ItemFieldStruct EmptyFiled;
	EmptyFiled.Init();
    FixItemInfo_Bank( SendToID , EmptyFiled , Pos );

    PG_Item_LtoC_DeleteItem Send;
    Send.Item = Item;
//    Send.Pos = Pos;
    Send.Type = Def_Item_Pos_Bank;
    Send.Result = EM_Item_ItemOK;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::DeleteItemOK_EQ            ( int SendToID , ItemFieldStruct& Item , int Pos )   
{
	ItemFieldStruct EmptyFiled;
	EmptyFiled.Init();
    FixItemInfo_EQ( SendToID , EmptyFiled , Pos );

    PG_Item_LtoC_DeleteItem Send;
    Send.Item = Item;
//    Send.Pos = Pos;
    Send.Type = Def_Item_Pos_EQ;
    Send.Result = EM_Item_ItemOK;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::DeleteItemFailed        ( int SendToID )
{
    PG_Item_LtoC_DeleteItem Send;
    Send.Result = EM_Item_DeleteItemFailed;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::ExchangeField_Body      ( int SendToID , ItemFieldStruct& Item1 , int Pos1 
                                                         , ItemFieldStruct& Item2 , int Pos2 , int ClientID )
{
    FixItemInfo_Body( SendToID , Item1 , Pos1 );
    FixItemInfo_Body( SendToID , Item2 , Pos2 );

    PG_Item_LtoC_ExchangeField Send;
    Send.Type[0] = Def_Item_Pos_Body;
    Send.Type[1] = Def_Item_Pos_Body;
    Send.Result = EM_Item_ItemOK;
	Send.ClientID = ClientID;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::ExchangeField_Bank      ( int SendToID , ItemFieldStruct& Item1 , int Pos1 
                                                         , ItemFieldStruct& Item2 , int Pos2 , int ClientID )
{
    FixItemInfo_Bank( SendToID , Item1 , Pos1 );
    FixItemInfo_Bank( SendToID , Item2 , Pos2 );

    PG_Item_LtoC_ExchangeField Send;
    Send.Type[0] = Def_Item_Pos_Bank;
    Send.Type[1] = Def_Item_Pos_Bank;
    Send.Result = EM_Item_ItemOK;
	Send.ClientID = ClientID;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );

}
void NetSrv_Item::ExchangeField_BodyBank  ( int SendToID , ItemFieldStruct& Item1 , int Pos1 
                                                         , ItemFieldStruct& Item2 , int Pos2 , int ClientID )
{
	if( Global::Ini()->IsHouseMoveItemLog == true )
	{//背包和銀行交換
		char Buf[128];
		RoleDataEx* Player = Global::GetRoleDataByGUID( SendToID );
		if( Item1.OrgObjID && Item2.OrgObjID )
		{//從銀行和背包交換
			sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d Bank_%d To Bag_%d" , Item1.OrgObjID , Item1.Count , Pos2 , Pos1 );
			Log_House( Player , -1 , -1 , EM_HouseActionType_Item_BankToBag , Buf );
			
			sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d Bag_%d To Bank_%d" , Item2.OrgObjID , Item2.Count , Pos1 , Pos2 );
			Log_House( Player , -1 , -1 , EM_HouseActionType_Item_BagToBank , Buf );
		}
		else if( Item1.OrgObjID )
		{//從銀行移到背包
			sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d Bank_%d To Bag_%d" , Item1.OrgObjID , Item1.Count , Pos2 , Pos1 );
			Log_House( Player , -1 , -1 , EM_HouseActionType_Item_BankToBag , Buf );
		}
		else
		{//從背包移到銀行
			sprintf_s( Buf , sizeof(Buf) , "Item[%d] Amount %d Bag_%d To Bank_%d" , Item2.OrgObjID , Item2.Count , Pos1 , Pos2 );
			Log_House( Player , -1 , -1 , EM_HouseActionType_Item_BagToBank , Buf );
		}
		
		Player->Msg( Buf );
	}
	FixItemInfo_Body( SendToID , Item1 , Pos1 );
    FixItemInfo_Bank( SendToID , Item2 , Pos2 );

    PG_Item_LtoC_ExchangeField Send;
    Send.Type[0] = Def_Item_Pos_Body;
    Send.Type[1] = Def_Item_Pos_Bank;
    Send.Result = EM_Item_ItemOK;
	Send.ClientID = ClientID;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );

}
/*
void NetSrv_Item::BodyBankMoneyExchange   ( int SendToID , int Count )
{
    PG_Item_CtoL_RequestBodyBankMoneyExchange Send;
    Send.Count = Count;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
*/
void NetSrv_Item::FixItemInfo                     ( int SendToID , ItemFieldStruct& Item , int Pos , int Type )
{
	if( &Item == NULL )
		return;
    PG_Item_LtoC_FixItemInfo Send;
    Send.Item = Item;
    Send.Pos = Pos;
    Send.Type = Type;
/*    if( Send.Item.Coldown < (int)RoleDataEx::G_SysTime )
        Send.Item.Coldown = 0;
    else
        Send.Item.Coldown -= RoleDataEx::G_SysTime;
*/

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::FixItemInfo_Body                ( int SendToID , ItemFieldStruct& Item , int Pos )
{
	if( &Item == NULL )
		return;
    FixItemInfo( SendToID , Item , Pos , Def_Item_Pos_Body );
    /*
    PG_Item_LtoC_FixItemInfo Send;
    Send.Item = Item;
    Send.Pos = Pos;
    Send.Type = Def_Item_Pos_Body;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
    */
}
void NetSrv_Item::FixItemInfo_Bank                ( int SendToID , ItemFieldStruct& Item , int Pos )
{
	if( &Item == NULL )
		return;

    FixItemInfo( SendToID , Item , Pos , Def_Item_Pos_Bank );
    /*
    PG_Item_LtoC_FixItemInfo Send;
    Send.Item = Item;
    Send.Pos = Pos;
    Send.Type = Def_Item_Pos_Bank;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
    */
}
void NetSrv_Item::FixItemInfo_EQ                  ( int SendToID , ItemFieldStruct& Item , int Pos )
{
	if( &Item == NULL )
		return;

    FixItemInfo( SendToID , Item , Pos , Def_Item_Pos_EQ );
    /*
    PG_Item_LtoC_FixItemInfo Send;
    Send.Item = Item;
    Send.Pos = Pos;
    Send.Type = Def_Item_Pos_EQ;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
    */
}

void NetSrv_Item::FixAllItemInfo          ( int SendToID , RoleData* Role  )   
{
    PG_Item_LtoC_FixAllItemInfo Send;
	Send.Body = Role->PlayerBaseData->Body;
    Send.Bank = Role->PlayerBaseData->Bank;
    Send.EQ   = Role->BaseData.EQ;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::FixMoney                ( int SendToID , int BodyMoney , int BankMoney , int BodyMoney_Account , int Money_Bonus )   
{
    PG_Item_LtoC_FixMoney Send;
    Send.BankMoney = BankMoney;
    Send.BodyMoney = BodyMoney;
	Send.Money_Bonus = Money_Bonus;
	Send.BodyMoney_Account = BodyMoney_Account;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::MaxItemCount( int SendToID , int MaxBody , int MaxBank )   
{
    PG_Item_LtoC_MaxItemCount Send;
    Send.MaxBank = MaxBank;
    Send.MaxBody = MaxBody;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::OpenBank( int SendToID , int GItemID )   
{
    PG_Item_LtoC_OpenBank Send;
	Send.GItemID = GItemID;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::CloseBank( int SendToID )
{
	PG_Item_LtoC_CloseBank Send;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::UseItemOK( int SendToID , int Pos , int Type )   
{
    PG_Item_LtoC_UseItemResult Send;
    Send.Pos = Pos;
    Send.Type = Type;
    Send.Result = true;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::UseItemFailed       ( int SendToID , int Pos , int Type )   
{
    PG_Item_LtoC_UseItemResult Send;
    Send.Pos = Pos;
    Send.Type = Type;
    Send.Result = false;

    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::S_DigHoleResult	  ( int SendToID , bool Result )
{
	PG_Item_LtoC_DigHoleResult Send;
	Send.Result = Result;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::ExchangeFieldFailed_Body       ( int SendToID  , ItemFieldStruct& Item1 , int Pos1 
                                                                 , ItemFieldStruct& Item2 , int Pos2 , int ClientID )
{
    FixItemInfo_Body( SendToID , Item1 , Pos1 );
    FixItemInfo_Body( SendToID , Item2 , Pos2 );

    PG_Item_LtoC_ExchangeField Send;
    Send.Type[0] = Def_Item_Pos_Body;
    Send.Type[1] = Def_Item_Pos_Body;
    Send.Result = EM_Item_ExchangeFieldFailed;
	Send.ClientID = ClientID;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::ExchangeFieldFailed_Bank      ( int SendToID  , ItemFieldStruct& Item1 , int Pos1 
                                                                , ItemFieldStruct& Item2 , int Pos2 , int ClientID )
{
    FixItemInfo_Bank( SendToID , Item1 , Pos1 );
    FixItemInfo_Bank( SendToID , Item2 , Pos2 );

    PG_Item_LtoC_ExchangeField Send;
    Send.Type[0] = Def_Item_Pos_Bank;
    Send.Type[1] = Def_Item_Pos_Bank;
    Send.Result = EM_Item_ExchangeFieldFailed;
	Send.ClientID = ClientID;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::ExchangeFieldFailed_BodyBank( int SendToID  , ItemFieldStruct& BodyItem , int BodyPos 
                                                              , ItemFieldStruct& BankItem , int BankPos , int ClientID )
{
    FixItemInfo_Body( SendToID , BodyItem , BodyPos );
    FixItemInfo_Bank( SendToID , BankItem , BankPos );

    PG_Item_LtoC_ExchangeField Send;
    Send.Type[0] = Def_Item_Pos_Body;
    Send.Type[1] = Def_Item_Pos_Bank;
    Send.Result = EM_Item_ExchangeFieldFailed;
	Send.ClientID = ClientID;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
/*
void NetSrv_Item::BodyBankMoneyExchangeOK         ( int SendToID , int BodyMoney , int BankMoney )
{
    FixMoney( SendToID  , BodyMoney , BankMoney, -1 , -1 );
    PG_Item_LtoC_BodyBankMoneyExchange Send;
    Send.Result = EM_Item_ItemOK;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::BodyBankMoneyExchangeFailed     ( int SendToID , int BodyMoney , int BankMoney )
{
    FixMoney( SendToID  , BodyMoney , BankMoney , -1 , -1 );
    PG_Item_LtoC_BodyBankMoneyExchange Send;
    Send.Result = EM_Item_ExchangeMoneyFailed;
    Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
*/
void NetSrv_Item::FixKeyItemFlag					( int SendToID , int KeyItemObjID , bool Value )
{
	PG_Item_LtoC_SetKeyItemFlag Send;
	Send.KeyItemID = KeyItemObjID;
	Send.Value = Value;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::FixTitleFlag					( int SendToID , int TitleObjID , bool Value )
{
	PG_Item_LtoC_SetTitleFlag Send;
	Send.TitleObjID = TitleObjID;
	Send.Value = Value;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::FixCardFlag					( int SendToID , int CardObjID , bool Value )
{
	PG_Item_LtoC_SetCardFlag Send;
	Send.CardObjID = CardObjID;
	Send.Value = Value;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}



void NetSrv_Item::UseItemtoItemResult_OK			( int SendToID  )
{
	PG_Item_LtoC_UseItemToItem_Result Send;
	Send.Result = true;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::UseItemtoItemResult_Failed		( int SendToID , ItemFieldStruct& UseItem  , int UseItemPos , ItemFieldStruct& TargetItem , int TargetItemPos , int TargetPosType )
{
	FixItemInfo_Body( SendToID , UseItem , UseItemPos );
	FixItemInfo		( SendToID , TargetItem , TargetItemPos , TargetPosType );

	PG_Item_LtoC_UseItemToItem_Result Send;
	Send.Result = false;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}


void NetSrv_Item::UsePowerLight	( int SendToID , int PlayerGUID , int PowerLightGUID )
{
	PG_Item_LtoC_UsePowerLight Send;
	Send.PowerLightGUID = PowerLightGUID;
	Send.PlayerGUID = PlayerGUID;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::EqRefineResult					( int SendToID , ItemFieldStruct& Item ,	EqRefineResultENUM	Result )
{
	PG_Item_LtoC_EqRefineResult Send;
	Send.Item = Item;
	Send.Result = Result;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::S_ItemDissolutionResult			( int SendToID , ItemFieldStruct& Item , ItemDissolutionResultENUM Result )
{
	PG_Item_LtoC_ItemDissolutionResult Send;
	Send.Item = Item;
	Send.Result = Result;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::S_TakeOutCardResult				( int SendToID , int CardObjID , bool Result )
{
	PG_Item_LtoC_TakeOutCardResult Send;
	Send.CardObjID = CardObjID;
	Send.Result = Result;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::S_ApartBodyItemResult			( int SendToID , bool Result  , short	PosSrc , short PosDes )
{
	PG_Item_LtoC_ApartBodyItemResult	Send;
	Send.Result = Result;
	Send.PosSrc = PosSrc;
	Send.PosDes = PosDes;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

//ª««~¦X¦¨
void NetSrv_Item::S_EQCombinOpen				( int SendToID , int TargetNPC  , int ItemPos , int Type )
{
	PG_Item_LtoC_EQCombinOpen Send;
	Send.TargetNPC	= TargetNPC;
	Send.ItemPos	= ItemPos;
	Send.Type		= Type;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::S_EQCombinClose					( int SendToID )
{
	PG_Item_LtoC_EQCombinClose Send;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::S_EQCombinResult				( int SendToID , bool Result )
{
	PG_Item_LtoC_EQCombinResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::SD_AccountBagInfoRequest	( int PlayerDBID , const char* Account )
{
	PG_Item_LtoD_AccountBagInfoRequest Send;
	Send.Account = Account;
	Send.PlayerDBID = PlayerDBID;
	Global::SendToDBCenter( sizeof(Send) , &Send );
}
void NetSrv_Item::SC_OpenAccountBag			( int SendToID , int TargetNPC , int MaxCount , int ItemDBID[20] , ItemFieldStruct Item[20] )
{
	PG_Item_LtoC_OpenAccountBag Send;

	Send.TargetNPC = TargetNPC;
	Send.MaxCount = MaxCount;
	memcpy( Send.Item , Item , sizeof( Send.Item ) );
	memcpy( Send.ItemDBID , ItemDBID , sizeof( Send.ItemDBID ) );

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_AccountBagGetItemResult( int SendToID , int BodyPos , int ItemDBID , AccountBagGetItemResultENUM Result )
{
	PG_Item_LtoC_AccountBagGetItemResult Send;
	Send.BodyPos = BodyPos;
	Send.ItemDBID = ItemDBID;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::S_ItemTimeout					( int SendToID , ItemTimeoutPosENUM Pos  , ItemFieldStruct& Item )
{
	PG_Item_LtoC_ItemTimeout Send;
	
	Send.Item = Item;
	Send.Pos = Pos;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
/*
void NetSrv_Item::SC_SmeltEqtoMagicStoneResult	( int SendToID , int EQPos , int SmeltItemPos , int BoxPos , bool Result )
{
	PG_Item_LtoC_SmeltEqtoMagicStoneResult Send;

	Send.EQPos = EQPos;
	Send.SmeltItemPos = SmeltItemPos;
	Send.BoxPos = BoxPos;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_RefineMagicStoneResult		( int SendToID , int MagicStonePos[3] , int BoxPos , bool Result )
{
	PG_Item_LtoC_RefineMagicStoneResult Send;

	memcpy( Send.MagicStonePos , MagicStonePos , sizeof( Send.MagicStonePos ) );
	Send.BoxPos = BoxPos;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_CombinEQandMagicStoneResult( int SendToID , int MagicStonePos[3] , int EQPos , int BoxPos , bool Result )
{
	PG_Item_LtoC_CombinEQandMagicStoneResult Send;

	memcpy( Send.MagicStonePos , MagicStonePos , sizeof( Send.MagicStonePos ) );
	Send.EQPos = EQPos;
	Send.BoxPos = BoxPos;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_RuneTransferResult		( int SendToID , int RunePos[2] , int BoxPos , bool Result )
{
	PG_Item_LtoC_RuneTransferResult Send;

	Send.RunePos[0] = RunePos[0];
	Send.RunePos[1] = RunePos[1];
	Send.BoxPos = BoxPos;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_RefineRuneResult			( int SendToID , int RunePos[5] , int BoxPos , bool Result )
{
	PG_Item_LtoC_RefineRuneResult Send;
	memcpy( Send.RunePos , RunePos , sizeof( Send.RunePos ) );
	Send.BoxPos = BoxPos;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
*/
void NetSrv_Item::SC_MagicBoxResult			( int SendToID , ItemFieldStruct* NewItem , bool Result )
{
	PG_Item_LtoC_MagicBoxResult Send;
	
	Send.Item = *NewItem;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::SC_Range_UseEq				   ( RoleDataEx* Owner , int EqID )
{
	PG_Item_LtoC_UseEq Send;
	Send.EqID = EqID;
	Send.PlayerGUID = Owner->GUID();
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		RoleDataEx* Other = Global::GetRangePlayer();
		if( Other == NULL || !Other->IsPlayer() )
			break;
		Global::SendToCli_ByGUID( Other->GUID() , sizeof( Send ) , &Send );
		//UsePowerLight( Other->GUID() , Owner->GUID() , EqID );
	}
}

void NetSrv_Item::SC_GamebleItemStateResult	( int SendToID , GamebleItemStateResultENUM Result , int TableID , int StatePos , int StateID )
{
	PG_Item_LtoC_GamebleItemStateResult Send;
	Send.TableID = TableID;
	Send.StateID = StateID;
	Send.StatePos = StatePos;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_GetGamebleItemStateResult( int SendToID , bool Result )
{
	PG_Item_LtoC_GetGamebleItemStateResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_DelItemMsg( int SendToID , ItemFieldStruct& Item  )
{
	PG_Item_LtoC_DelItemMsg Send;
	Send.Item = Item;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_OpenGamebleItemState	( int SendToID , int TargetID )
{
	PG_Item_LtoC_OpenGamebleItemState Send;
	Send.TargetID = TargetID;

	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_CloseGamebleItemState	( int SendToID )
{
	PG_Item_LtoC_CloseGamebleItemState Send;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_OpenMagicStoneExchange	( int SendToID , int MagicStoneID , int Lv , int Money1 , int Money2 )
{
	PG_Item_LtoC_OpenMagicStoneExchange Send;
	Send.MagicStoneID = MagicStoneID;
	Send.Lv = Lv;
	Send.Money[0] = Money1;
	Send.Money[1] = Money2;
	
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_MagicStoneExchangeResultOK	( int SendToID , ItemFieldStruct& Item  , int Money[2] )
{
	PG_Item_LtoC_MagicStoneExchangeResult Send;
	Send.Item = Item;
	Send.Result = EM_MagicStoneExchangeResult_OK;
	Send.Money[0] = Money[0];
	Send.Money[1] = Money[1];
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_MagicStoneExchangeResultFailed	( int SendToID  , MagicStoneExchangeResultENUM	Result )
{
	PG_Item_LtoC_MagicStoneExchangeResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_DrawOutRuneResult		( int SendToID , DrawOutRuneResultENUM Result )
{
	PG_Item_LtoC_DrawOutRuneResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::SC_PutCoolClothResult( int SendToID , PutCoolClothResultENUM	Result )
{
	PG_Item_LtoC_PutCoolClothResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_DelCoolClothResult( int SendToID , bool Result )
{
	PG_Item_LtoC_DelCoolClothResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_SetCoolSuitResult( int SendToID , bool Result )
{
	PG_Item_LtoC_SetCoolSuitResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );

}
void NetSrv_Item::SC_ShowCoolSuitIndexResult( int SendToID , bool Result )
{
	PG_Item_LtoC_ShowCoolSuitIndexResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::SC_PutInExBagResult		( int SendToID , PutInExBagResultENUM Result )
{
	PG_Item_LtoC_PutInExBagResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_UseExBagResult			( int SendToID , bool Result )
{
	PG_Item_LtoC_UseExBagResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::SC_SwapCoolClothResult	( int SendToID , bool Result )
{
	PG_Item_LtoC_SwapCoolClothResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}

void NetSrv_Item::SC_OpenRare3EqExchangeItem( int SendToID , int TargetNPCID )
{
	PG_Item_LtoC_OpenRare3EqExchangeItem Send;
	Send.TargetNPCID = TargetNPCID;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_Rare3EqExchangeItemResult( int SendToID , Rare3EqExchangeItemRequestENUM Result )
{
	PG_Item_LtoC_Rare3EqExchangeItemResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_PopExBagResult			( int SendToID , PopExBagResultENUM Result )
{
	PG_Item_LtoC_PopExBagResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendToID , sizeof( Send ) , &Send );
}
void NetSrv_Item::SC_ClsAttributeResult( int SendtoID , bool Result )
{
	PG_Item_LtoC_ClsAttributeResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );
}

void NetSrv_Item::SC_MagicBoxAttrDrawOffResult( int SendtoID , bool Result )
{
	PG_Item_LtoC_MagicBoxAttrDrawOffResult Send;
	Send.Result = Result;
	Global::SendToCli_ByGUID( SendtoID , sizeof( Send ) , &Send );
}