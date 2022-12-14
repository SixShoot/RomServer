#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_Item.h"

class NetSrv_Item : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Item* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------
    static void _PG_Item_CtoL_GetGroundItem               	( int NetID , int Size , void* Data );
    static void _PG_Item_CtoL_GetBufferItem               	( int NetID , int Size , void* Data );
    static void _PG_Item_CtoL_DeleteItem                  	( int NetID , int Size , void* Data );
    static void _PG_Item_CtoL_ExchangeField               	( int NetID , int Size , void* Data );
    static void _PG_Item_CtoL_RequestBodyBankMoneyExchange	( int NetID , int Size , void* Data );
    static void _PG_Item_CtoL_RequestBufferInfo           	( int NetID , int Size , void* Data );
    static void _PG_Item_CtoL_ApartBodyItemRequest        	( int NetID , int Size , void* Data );
    static void _PG_Item_CtoL_RequestFixAllItemInfo       	( int NetID , int Size , void* Data );
    static void _PG_Item_CtoL_CloseBank                   	( int NetID , int Size , void* Data );
    static void _PG_Item_CtoL_UseItem                     	( int NetID , int Size , void* Data );

	static void _PG_Item_CtoL_UseItemToItem_Notify        	( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_ItemDissolutionRequest      	( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_TakeOutCardRequest		  	( int NetID , int Size , void* Data );

	static void _PG_Item_CtoL_EQCombinRequest				( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_EQCombinClose					( int NetID , int Size , void* Data );

	static void _PG_Item_DtoL_AccountBagInfoResult			( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_AccountBagGetItemRequest		( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_AccountBagClose				( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_MagicBoxRequest				( int NetID , int Size , void* Data );	

	static void _PG_Item_CtoL_GamebleItemState				( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_GetGamebleItemState			( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_CloseGamebleItemState			( int NetID , int Size , void* Data );

	static void _PG_Item_CtoL_MagicStoneExchangeResult		( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_DrawOutRuneRequest			( int NetID , int Size , void* Data );

	static void _PG_Item_CtoL_PutCoolClothRequest			( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_DelCoolClothRequest			( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_SetCoolSuitRequest			( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_ShowCoolSuitIndexRequest		( int NetID , int Size , void* Data );

	static void _PG_Item_CtoL_PutInExBagRequest				( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_PopExBagRequest				( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_UseExBagRequest				( int NetID , int Size , void* Data );

	static void _PG_Item_CtoL_SwapCoolClothRequest			( int NetID , int Size , void* Data );

	static void _PG_Item_CtoL_Rare3EqExchangeItemRequest	( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_CloseRare3EqExchangeItem		( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_ClsAttribute					( int NetID , int Size , void* Data );
	static void _PG_Item_CtoL_MagicBoxAttrDrawOff			( int NetID , int Size , void* Data );
public:                                                   
    //------------------------------------------------------------------------------------------------------------------------
    static void GetItemOK                       ( int SendToID , ItemFieldStruct& Item , short Pos );
	static void GetItemOK                       ( int SendToID );
    static void GetItemFailed                   ( int SendToID );
    static void GetBufferItemOK                 ( int SendToID , ItemFieldStruct& Item , short Pos , int PopCount );
	static void DelBufferItemOK                 ( int SendToID , ItemFieldStruct& Item  );
    static void GetBufferItemFailed             ( int SendToID );

    static void ClsItemInBuffer                 ( int SendToID );
    static void NewItemInBuffer                 ( int SendToID , ItemFieldStruct& Item );           
    static void DeleteItemOK_Body               ( int SendToID , ItemFieldStruct& Item , int Pos  );   
    static void DeleteItemOK_EQ                 ( int SendToID , ItemFieldStruct& Item , int Pos  );   
    static void DeleteItemOK_Bank               ( int SendToID , ItemFieldStruct& Item , int Pos  );   
    static void DeleteItemFailed                ( int SendToID );   

    static void ExchangeField_Body              ( int SendToID , ItemFieldStruct& Item1 , int Pos1 
                                                               , ItemFieldStruct& Item2 , int Pos2 , int ClientID );   
    static void ExchangeField_Bank              ( int SendToID , ItemFieldStruct& Item1 , int Pos1 
                                                               , ItemFieldStruct& Item2 , int Pos2 , int ClientID );   
    static void ExchangeField_BodyBank          ( int SendToID , ItemFieldStruct& BodyItem , int Pos1 
                                                               , ItemFieldStruct& BankItem , int Pos2 , int ClientID );   

    static void ExchangeFieldFailed_Body        ( int SendToID , ItemFieldStruct& Item1 , int Pos1 
                                                               , ItemFieldStruct& Item2 , int Pos2 , int ClientID );   
    static void ExchangeFieldFailed_Bank        ( int SendToID , ItemFieldStruct& Item1 , int Pos1 
                                                               , ItemFieldStruct& Item2 , int Pos2 , int ClientID );   
    static void ExchangeFieldFailed_BodyBank    ( int SendToID , ItemFieldStruct& BodyItem , int BodyPos 
                                                               , ItemFieldStruct& BankItem , int BankPos , int ClientID );   

//    static void BodyBankMoneyExchangeOK         ( int SendToID , int BodyMoney , int BankMoney );
  //  static void BodyBankMoneyExchangeFailed     ( int SendToID , int BodyMoney , int BankMoney );
    static void FixItemInfo_Body                ( int SendToID , ItemFieldStruct& Item , int Pos );
    static void FixItemInfo_Bank                ( int SendToID , ItemFieldStruct& Item , int Pos );
    static void FixItemInfo_EQ                  ( int SendToID , ItemFieldStruct& Item , int Pos );
    static void FixItemInfo                     ( int SendToID , ItemFieldStruct& Item , int Pos , int Type );
    static void FixAllItemInfo                  ( int SendToID , RoleData* Role );   
    static void FixMoney                        ( int SendToID , int BodyMoney , int BankMoney , int BodyMoney_Account , int Money_Bonus );   
    static void MaxItemCount                    ( int SendToID , int MaxBody , int MaxBank );   
    static void OpenBank                        ( int SendToID , int GItemID );
    static void CloseBank                       ( int SendToID );
    static void UseItemOK                       ( int SendToID , int Pos , int Type );   
    static void UseItemFailed                   ( int SendToID , int Pos , int Type );   
	static void FixKeyItemFlag					( int SendToID , int KeyItemObjID , bool Value );
	static void FixTitleFlag					( int SendToID , int TitleObjID , bool Value );
	static void FixCardFlag						( int SendToID , int CardObjID , bool Value );

	static void	UseItemtoItemResult_OK			( int SendToID );
	static void	UseItemtoItemResult_Failed		( int SendToID , ItemFieldStruct& UseItem  , int UseItemPos , ItemFieldStruct& TargetItem , int TargetItemPos , int TargetPosType );

	static void UsePowerLight					( int SendToID , int PlayerGUID , int PowerLightGUID );
	static void EqRefineResult					( int SendToID , ItemFieldStruct& Item , EqRefineResultENUM	Result );
	static void S_ItemDissolutionResult			( int SendToID , ItemFieldStruct& Item , ItemDissolutionResultENUM Result );
	static void S_TakeOutCardResult				( int SendToID , int CardObjID , bool Result );
	static void S_ApartBodyItemResult			( int SendToID , bool Result  , short	PosSrc =0 , short PosDes = 0 );
	static void S_DigHoleResult					( int SendToID , bool Result );

	//ª««~¦X¦¨
	static void S_EQCombinOpen					( int SendToID , int TargetNPC , int ItemPos , int Type );
	static void S_EQCombinClose					( int SendToID );
	static void S_EQCombinResult				( int SendToID , bool Result );
	static void S_ItemTimeout					( int SendToID , ItemTimeoutPosENUM Pos  , ItemFieldStruct& Item );
	static void SC_DelItemMsg					( int SendToID , ItemFieldStruct& Item );
    //------------------------------------------------------------------------------------------------------------------------
    virtual void R_GetGroundItem               ( BaseItemObject* Sender , int ItemID , int Pos ) = 0;
    virtual void R_GetBufferItem               ( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos , int Count ) = 0;  
    virtual void R_DeleteItem_Body             ( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos ) = 0;
    virtual void R_DeleteItem_Bank             ( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos ) = 0;
    virtual void R_DeleteItem_EQ               ( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos ) = 0;
    virtual void R_ExchangeField_Body          ( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos1 
                                                                        , ItemFieldStruct& Item2 , int Pos2 , int ClientID ) = 0;

    virtual void R_ExchangeField_Bank          ( BaseItemObject* Sender , ItemFieldStruct& Item1 , int Pos1 
                                                                        , ItemFieldStruct& Item2 , int Pos2 , int ClientID ) = 0;

    virtual void R_ExchangeField_BodyBank      ( BaseItemObject* Sender , ItemFieldStruct& Item1 , int Pos1 
                                                                        , ItemFieldStruct& Item2 , int Pos2 , int ClientID ) = 0;

    virtual void R_ExchangeField_BankBody      ( BaseItemObject* Sender , ItemFieldStruct& Item1 , int Pos1 
                                                                        , ItemFieldStruct& Item2 , int Pos2 , int ClientID ) = 0;

    virtual void R_RequestBodyBankMoneyExchange( BaseItemObject* Sender , int Count ) = 0;

    virtual void R_RequestBufferInfo           ( BaseItemObject* Sender ) = 0;
    virtual void R_ApartBodyItemRequest        ( BaseItemObject* Sender  , ItemFieldStruct& ItemSrc , int PosSrc , int PosDes , int CountDes ) = 0;
    virtual void R_RequestFixAllItemInfo       ( BaseItemObject* Sender ) = 0;
    virtual void R_CloseBank                   ( BaseItemObject* Sender ) = 0;
    virtual void R_UseItem                     ( BaseItemObject* Sender , int Pos , int Type , int TargetID , float TargetX , float TargetY ,float TargetZ , ItemFieldStruct& Item ) = 0;
	static void SC_Range_UseEq				   ( RoleDataEx* Owner , int EqID );
    
	virtual void R_UseItemToItem_Notify			( BaseItemObject* Sender , ItemFieldStruct& UseItem , int UseItemPos , ItemFieldStruct& TargetItem , int TargetItemPos , int TargetPosType , char* Password ) = 0;
	

	virtual void R_ItemDissolutionRequest		( BaseItemObject* Sender , ItemFieldStruct& Item , int Pos ) = 0;

	virtual void R_TakeOutCardRequest			( BaseItemObject* Sender , int CardID ) = 0;
	virtual void R_EQCombinRequest				( BaseItemObject* Sender , int CombinItemPos[ 2 ] , int CheckItemPos ) = 0;
	virtual void R_EQCombinClose				( BaseItemObject* Sender ) = 0;

	//////////////////////////////////////////////////////////////////////////
	//Account Bag
	//////////////////////////////////////////////////////////////////////////
	static void SD_AccountBagInfoRequest	( int PlayerDBID , const char* Account );	
	static void SC_OpenAccountBag			( int SendToID , int TargetNPC , int MaxCount , int ItemDBID[20] , ItemFieldStruct Item[20] );	
	static void SC_AccountBagGetItemResult	( int SendToID , int BodyPos , int ItemDBID , AccountBagGetItemResultENUM Result );		
	virtual void RD_AccountBagInfoResult	( int PlayerDBID , int MaxCount , int ItemDBID[20] , ItemFieldStruct Item[20] ) = 0;
	virtual void RC_AccountBagGetItemRequest( BaseItemObject* Sender , int ItemDBID , int BodyPos ) = 0;
	virtual void RC_AccountBagClose			( BaseItemObject* Sender ) = 0;
	//////////////////////////////////////////////////////////////////////////
	// Rune Refine & MagicStone System
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_MagicBoxRequest			( BaseItemObject* Obj ) = 0;
	static void SC_MagicBoxResult			( int SendToID , ItemFieldStruct* NewItem , bool Result );
	//////////////////////////////////////////////////////////////////////////
	//賭屬性
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_GamebleItemState		( BaseItemObject* Obj , int TableID , int ItemPos , ItemFieldStruct& BodyItem , int LockFlag ) = 0;
	virtual void RC_GetGamebleItemState		( BaseItemObject* Obj , bool GiveUp ) = 0;
	virtual void RC_CloseGamebleItemState	( BaseItemObject* Obj ) = 0;

	static void SC_GamebleItemStateResult	( int SendToID , GamebleItemStateResultENUM Result , int TableID  , int StatePos , int StateID );
	static void SC_GetGamebleItemStateResult( int SendToID , bool Result );
	static void SC_OpenGamebleItemState		( int SendToID , int TargetID );
	static void SC_CloseGamebleItemState	( int SendToID );
	//////////////////////////////////////////////////////////////////////////
	static void SC_OpenMagicStoneExchange	( int SendToID , int MagicStoneID , int Lv , int Money1 , int Money2 );
		virtual void RC_MagicStoneExchangeResult( BaseItemObject* Obj , int AbilityID[3] , bool Result ) = 0;
			static void SC_MagicStoneExchangeResultOK		( int SendToID , ItemFieldStruct& Item , int Money[2] );
			static void SC_MagicStoneExchangeResultFailed	( int SendToID , MagicStoneExchangeResultENUM Result );
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_DrawOutRuneRequest		( BaseItemObject* Obj , int BodyPos , int RuneID ) = 0;
		static void SC_DrawOutRuneResult	( int SendToID , DrawOutRuneResultENUM Result );
	//////////////////////////////////////////////////////////////////////////
	//魔法衣櫥
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_PutCoolClothRequest		( BaseItemObject* Obj , int BodyPos , int CoolClothPos ) = 0;
		static void SC_PutCoolClothResult		( int SendToID , PutCoolClothResultENUM	Result );

	virtual void RC_DelCoolClothRequest		( BaseItemObject* Obj , int Pos , int ImageObjectID ) = 0;
		static void SC_DelCoolClothResult		( int SendToID , bool Result );

	virtual void RC_SetCoolSuitRequest		( BaseItemObject* Obj , int SuitIndexID , CoolSuitBase& Info ) = 0;
		static void SC_SetCoolSuitResult		( int SendToID , bool Result );

	virtual void RC_ShowCoolSuitIndexRequest( BaseItemObject* Obj , int CoolSuitIndexID ) = 0;
		static void SC_ShowCoolSuitIndexResult	( int SendToID , bool Result );

	virtual void RC_SwapCoolClothRequest	( BaseItemObject* Obj , int Pos[2] ) = 0;
		static void SC_SwapCoolClothResult	( int SendToID , bool Result );
	//////////////////////////////////////////////////////////////////////////
	//特殊背包
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_PutInExBagRequest		( BaseItemObject* Obj , ExBagTypeENUM ExBagType , int ItemID , int BodyPos ) = 0;
		static void SC_PutInExBagResult			( int SendToID , PutInExBagResultENUM Result );

	virtual void RC_PopExBagRequest			( BaseItemObject* Obj , ExBagTypeENUM ExBagType , int ExBagPos , int BodyPos ) = 0;
		static void SC_PopExBagResult			( int SendToID , PopExBagResultENUM Result );

	virtual void RC_UseExBagRequest			( BaseItemObject* Obj , ExBagTypeENUM ExBagType , int BagPos ) = 0;
		static void SC_UseExBagResult			( int SendToID , bool Result );
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	static void SC_OpenRare3EqExchangeItem( int SendToID , int TargetNPCID );
		virtual void RC_Rare3EqExchangeItemRequest( BaseItemObject* Obj , int Type , int ItemPos[6] ) = 0;
			static void SC_Rare3EqExchangeItemResult( int SendToID , Rare3EqExchangeItemRequestENUM Result );

	virtual void RC_CloseRare3EqExchangeItem( BaseItemObject* Obj ) = 0;

	virtual void RC_ClsAttribute			( BaseItemObject* Obj , int Type , int ItemPos , int AttributeID ) = 0;
		static void SC_ClsAttributeResult( int SendtoID , bool Result );

	virtual void RC_MagicBoxAttrDrawOff( BaseItemObject* Obj ) = 0;
		static void SC_MagicBoxAttrDrawOffResult( int SendtoID , bool Result );
};

