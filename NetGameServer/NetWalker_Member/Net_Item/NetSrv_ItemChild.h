#pragma once

#include "NetSrv_Item.h"

class NetSrv_ItemChild : public NetSrv_Item
{
	static int  _PowerLightProc( SchedularInfo* Obj , void* InputClass );		//效果光球處理
public:
    static bool Init();
    static bool Release();

    //修正所有身上未取的資料
    static void FixAllBufferInfo( BaseItemObject* Sender );
	static void	SendRange_UsePowerLight( int PlayerGUID , int PowerLightGUID );


    virtual void R_GetGroundItem               ( BaseItemObject* Sender , int ItemID , int Pos );
    virtual void R_GetBufferItem               ( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos , int Count );  
    virtual void R_DeleteItem_Body             ( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos );
    virtual void R_DeleteItem_Bank             ( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos );
    virtual void R_DeleteItem_EQ               ( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos );
    virtual void R_ExchangeField_Body          ( BaseItemObject* Sender , ItemFieldStruct& Item1 , int Pos1 
                                                                        , ItemFieldStruct& Item2 , int Pos2 , int ClientID );

    virtual void R_ExchangeField_Bank          ( BaseItemObject* Sender , ItemFieldStruct& Item1 , int Pos1 
                                                                        , ItemFieldStruct& Item2 , int Pos2 , int ClientID );

    virtual void R_ExchangeField_BodyBank      ( BaseItemObject* Sender , ItemFieldStruct& Item1 , int Pos1 
                                                                        , ItemFieldStruct& Item2 , int Pos2 , int ClientID );

    virtual void R_ExchangeField_BankBody      ( BaseItemObject* Sender , ItemFieldStruct& Item1 , int Pos1 
                                                                        , ItemFieldStruct& Item2 , int Pos2 , int ClientID );

    virtual void R_RequestBodyBankMoneyExchange( BaseItemObject* Sender , int Count );

    virtual void R_RequestBufferInfo           ( BaseItemObject* Sender );
    virtual void R_ApartBodyItemRequest        ( BaseItemObject* Sender  , ItemFieldStruct& ItemSrc , int PosSrc , int PosDes , int CountDes );
    virtual void R_RequestFixAllItemInfo       ( BaseItemObject* Sender );
    virtual void R_CloseBank                   ( BaseItemObject* Sender );
    virtual void R_UseItem                     ( BaseItemObject* Sender , int Pos , int Type , int TargetID , float TargetX , float TargetY ,float TargetZ  , ItemFieldStruct& Item );
//    virtual void R_UseItem_Body                ( BaseItemObject* Sender , int Pos , int Type , int TargetID , ItemFieldStruct& Item );

	virtual void R_UseItemToItem_Notify		   ( BaseItemObject* Sender , ItemFieldStruct& UseItem , int UseItemPos , ItemFieldStruct& TargetItem , int TargetItemPos , int	TargetPosType , char* Password );

	virtual void R_ItemDissolutionRequest		( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos );
	virtual void R_TakeOutCardRequest			( BaseItemObject* Sender , int CardID );
	virtual void R_EQCombinRequest				( BaseItemObject* Sender , int CombinItemPos[ 2 ] , int CheckItemPos );	
	virtual void R_EQCombinClose				( BaseItemObject* Sender );

	virtual void RD_AccountBagInfoResult	( int PlayerDBID , int MaxCount , int ItemDBID[20] , ItemFieldStruct Item[20] );
	virtual void RC_AccountBagGetItemRequest( BaseItemObject* Sender , int ItemDBID , int BodyPos );
	virtual void RC_AccountBagClose			( BaseItemObject* Sender );
	//////////////////////////////////////////////////////////////////////////
	// Rune Refine & MagicStone System
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_MagicBoxRequest			( BaseItemObject* Obj );
	//////////////////////////////////////////////////////////////////////////
	//賭屬性
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_GamebleItemState		( BaseItemObject* Obj , int TableID , int ItemPos , ItemFieldStruct& BodyItem , int LockFlag );
	virtual void RC_GetGamebleItemState		( BaseItemObject* Obj , bool GiveUp );
	virtual void RC_CloseGamebleItemState	( BaseItemObject* Obj );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_MagicStoneExchangeResult( BaseItemObject* Obj , int AbilityID[3] , bool Result );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_DrawOutRuneRequest( BaseItemObject* Obj , int BodyPos , int RuneID );

	//////////////////////////////////////////////////////////////////////////
	//魔法衣櫥
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_PutCoolClothRequest		( BaseItemObject* Obj , int BodyPos , int CoolClothPos );
	virtual void RC_DelCoolClothRequest		( BaseItemObject* Obj , int Pos , int ImageObjectID );
	virtual void RC_SetCoolSuitRequest		( BaseItemObject* Obj , int SuitIndexID , CoolSuitBase& Info );
	virtual void RC_ShowCoolSuitIndexRequest( BaseItemObject* Obj , int CoolSuitIndexID );
	virtual void RC_SwapCoolClothRequest	( BaseItemObject* Obj , int Pos[2] );
	//////////////////////////////////////////////////////////////////////////
	//特殊背包
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_PutInExBagRequest		( BaseItemObject* Obj , ExBagTypeENUM ExBagType , int ItemID , int BodyPos );
	virtual void RC_PopExBagRequest			( BaseItemObject* Obj , ExBagTypeENUM ExBagType , int ExBagPos , int BodyPos );
	virtual void RC_UseExBagRequest			( BaseItemObject* Obj , ExBagTypeENUM ExBagType , int BagPos );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_Rare3EqExchangeItemRequest( BaseItemObject* Obj , int Type , int ItemPos[6] );
	virtual void RC_CloseRare3EqExchangeItem( BaseItemObject* Obj );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_ClsAttribute			( BaseItemObject* Obj , int Type , int ItemPos , int AttributeID );
	virtual void RC_MagicBoxAttrDrawOff		( BaseItemObject* Obj );
};

