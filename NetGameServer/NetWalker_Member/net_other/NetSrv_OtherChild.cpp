#include "../NetWakerGSrvInc.h"
#include "NetSrv_OtherChild.h"
#include "../../mainproc/magicproc/MagicProcess.h"
#include "MD5/Mymd5.h"
#include "../../mainproc/partyinfolist/PartyInfoList.h"
#include "../../MainProc/LuaPlot/LuaPlot.h"
//-----------------------------------------------------------------
int		NetSrv_OtherChild::MoneyKeyValueCount = 0;
int		NetSrv_OtherChild::MoneyKeyValueSize = 0;
char	NetSrv_OtherChild::MoneyKeyValueZip[0x10000];

int		NetSrv_OtherChild::DBStringCount;
int		NetSrv_OtherChild::DBStringSize;
char	NetSrv_OtherChild::DBStringZip[0x10000];
int		NetSrv_OtherChild::ManageKey = 0;
vector< map< int , MapMarkInfoStruct > > NetSrv_OtherChild::AllMarkInfo;
vector< map< int , int > >	NetSrv_OtherChild::AllRoomValueInfo;
map< string , int >			NetSrv_OtherChild::SysKeyValue;			//後台設定的SysKeyValue
vector<WeekZoneStruct>		NetSrv_OtherChild::WeekInst;
//-----------------------------------------------------------------
bool    NetSrv_OtherChild::Init()
{
    NetSrv_Other::_Init();

    if( This != NULL)
        return false;

	This = NEW NetSrv_OtherChild;	

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_OtherChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_Other::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
/*
void NetSrv_OtherChild::ExchangeResultFunction( char* UserAccount , int RoleGUID , GameItemExchangeInfoStruct& Info )
{
	char	Buf[512];
	RoleDataEx* Role = Global::GetRoleDataByGUID( RoleGUID );
	if( Role == NULL || Role->IsPlayer() == false )
	{
		//物品兌換有問題 回傳角色不存在 帳號 = %s Type = %d  物品ID = %d 數量=%d 
		sprintf( Buf , "Item Exchange Err ,  Return Role Not Find , Account = %s Type = %d  ObjID = %d Count=%d " , UserAccount , Info.Type , Info.ItemID , Info.Count );
		Print( LV3 , "ExchangeResultFunction" , Buf );

		Global::SendMsgToGMTools( LV5 , UserAccount , Buf );
		return;
	}

	if( stricmp( Role->Base.Account_ID.Begin() , UserAccount ) != 0 )
	{

		sprintf( Buf , "Item Exchange Err , Return Role Err , UseAccount =%s Account= %s  Role = %s Type = %d  ObjID = %d Count=%d" , UserAccount , Role->Account_ID() , Utf8ToChar( Role->RoleName() ).c_str() , Info.Type , Info.ItemID , Info.Count );
		Print( LV3 , "ExchangeResultFunction" , Buf );

		Global::SendMsgToGMTools( LV5 , UserAccount , Buf );
		return;
	}
	ItemFieldStruct Item;
	Item.Init();

	GameObjDbStructEx* ObjDB = Global::GetObjDB( Info.ItemID );
	if( ObjDB->IsItem() != false )
	{
		Item.OrgObjID	= Info.ItemID;					
		Item.Count		= Info.Count;						
		Item.ExValue	= Info.ExValue;
		Item.Inherent	= Info.Inherent;					 
		memcpy( Item.Rune , Info.Rune , sizeof(Item.Rune) );
		Item.MainColor	= Info.MainColor;
		Item.OffColor	= Info.OffColor;
		Item.Mode._Mode = Info.Mode;				
		Item.Mode.ExchangeItemSystem = 1;
		Item.Durable	= Item.Quality * ObjDB->Item.Durable;
		Item.ImageObjectID = Info.ImageObjID;
	}
	
	if( Info.Type == EM_ItemExchangeResultType_OK )
	{
		if( Info.Money > 0 )
			Role->AddBodyMoney( Info.Money , NULL , EM_ActionType_ItemExchange );
		if( Info.Money_Account > 0 )
			Role->AddBodyMoney_Account( Info.Money_Account , NULL , EM_ActionType_ItemExchange );
		Role->GiveItem( Item , EM_ActionType_ItemExchange , NULL );
	}

	S_ItemExchangeResult( Role->GUID() , (ItemExchangeResultTypeENUM)Info.Type , Item , Info.Money , Info.Money_Account);
}
*/
//-----------------------------------------------------------------
void NetSrv_OtherChild::R_RequestPlayerPos	( int ZoneID , int DBID , char* Name )
{
	BaseItemObject* PlayerObj = BaseItemObject::GetObjByPlayerName( Name );

	if( PlayerObj == NULL )
		return;

	S_PlayerPos( ZoneID , DBID , PlayerObj->Role()->RoomID() , *(PlayerObj->Role()->Pos()) );
}
void NetSrv_OtherChild::R_PlayerPos			( int ZoneID , int DBID , int RoomID , RolePosStruct& Pos )
{
	BaseItemObject* PlayerObj = BaseItemObject::GetObjByDBID( DBID );
	if( PlayerObj == NULL )
		return;

	Global::ChangeZone( PlayerObj->GUID() , ZoneID , RoomID , Pos.X , Pos.Y , Pos.Z , Pos.Dir );
}
void NetSrv_OtherChild::R_CallPlayer		( int ZoneID , RolePosStruct& Pos , char* Name , int RoomID )
{
	BaseItemObject* PlayerObj = BaseItemObject::GetObjByPlayerName( Name );

	if( PlayerObj == NULL )
		return;

	Global::ChangeZone( PlayerObj->GUID() , ZoneID , -1 , Pos.X , Pos.Y , Pos.Z , Pos.Dir );
}

void NetSrv_OtherChild::R_DialogSelectID	( BaseItemObject* Sender  , int SelectID )
{
	RoleDataEx* Owner = Sender->Role();
	if( Owner->PlayerTempData->DialogSelectID != -2 )
		return;
	Owner->PlayerTempData->DialogSelectID = SelectID;
	return;
}

void NetSrv_OtherChild::R_BeginCastingRequest( BaseItemObject* Sender , CastingTypeENUM Type , int ItemID , int TargetGUID )
{
	RoleDataEx* Owner = Sender->Role();

	//如果還在跑某個CastingBar
	if(		( Owner->PlayerTempData->CastingType != EM_CastingType_NULL && Owner->PlayerTempData->CastingType < EM_CastingType_OKRESULT )
		||	( Type != EM_CastingType_NULL && Type == EM_CastingType_OKRESULT) 
		||	Owner->IsSpell() != false )
	{
		S_BeginCastingResult( Sender->GUID() , ItemID , Type , 0 , false );
		return;
	}

	if( Type == EM_CastingType_Dissolution )
	{
		S_BeginCastingResult( Sender->GUID() , ItemID , Type , 3000 , true );
		Owner->PlayerTempData->CastingType = Type;
		Owner->PlayerTempData->CastingItemID = ItemID;
		Owner->PlayerTempData->CastingTime = RoleDataEx::G_SysTime + 3000;
		return;
	}
	
	GameObjDbStructEx* ObjDB = Global::GetObjDB( ItemID );
	if( ObjDB->IsRecipe() != false )
	{
		S_BeginCastingResult( Sender->GUID() , ItemID , Type , 3000 , true );

		Owner->PlayerTempData->CastingType = Type;
		Owner->PlayerTempData->CastingItemID = ItemID;
		Owner->PlayerTempData->CastingTime = RoleDataEx::G_SysTime + 3000;
		return;
	}

	if( ObjDB->IsCard() != false )
	{
		S_BeginCastingResult( Sender->GUID() , ItemID , Type , 3000 , true );

		Owner->PlayerTempData->CastingType = Type;
		Owner->PlayerTempData->CastingItemID = ItemID;
		Owner->PlayerTempData->CastingTime = RoleDataEx::G_SysTime + 3000;
		return;
	}

	if( ObjDB->IsItem() == false || ObjDB->Item.CastingTime == 0 )
	{
		S_BeginCastingResult( Sender->GUID() , ItemID , Type , 0 , false );
		return;
	}

	vector<MyVMValueStruct> RetList;
	if( strlen( ObjDB->Item.CheckUseScript ) != 0 )
	{
		bool Result = true;
		Owner->TempData.Magic.UseItem		= ItemID;
		Owner->TempData.Magic.UseItemPos	= 0;
		Owner->TempData.Magic.UseItemType	= Type;

		if( LUA_VMClass::PCallByStrArg( ObjDB->Item.CheckUseScript , Owner->GUID() , TargetGUID , &RetList , 1 ) )
		{
			if( RetList.size() != 0 )
			{
				MyVMValueStruct& Temp = RetList[0];
				if( Temp.Flag != true )
				{
					Result = false;
				}
			}

		}
		else
		{
			Result = false;
		}

		Owner->ClsUseItem();

		if( Result == false )
		{
			Owner->PlayerTempData->CastingType = EM_CastingType_NULL;
			Owner->PlayerTempData->CastingItemID = 0;
			Owner->PlayerTempData->CastingTime = 0;

			S_BeginCastingResult( Sender->GUID() , ItemID , Type , 0 , false );
			return;
		}
	}

	S_BeginCastingResult( Sender->GUID() , ItemID , Type , ObjDB->Item.CastingTime * 1000 , true );

	Owner->PlayerTempData->CastingType = Type;
	Owner->PlayerTempData->CastingItemID = ItemID;
	Owner->PlayerTempData->CastingTime = RoleDataEx::G_SysTime + ObjDB->Item.CastingTime * 1000;

}
void NetSrv_OtherChild::R_InterruptCastingRequest(  BaseItemObject* Sender  )
{
	RoleDataEx* Owner = Sender->Role();
	if(	Owner->PlayerTempData->CastingType != EM_CastingType_NULL )
	{
		S_InterruptCasting( Owner->GUID() , Owner->PlayerTempData->CastingType );
	}
	Owner->PlayerTempData->CastingType = EM_CastingType_NULL;
}

void NetSrv_OtherChild::R_RangeDataTransferRequest( BaseItemObject* Sender , int Range , int Size , const char* Data )
{
	S_RangeDataTransfer( Sender->GUID() , Range , Size , Data );
}

void NetSrv_OtherChild::R_ColoringShopRequest( BaseItemObject* Sender , ColoringStruct& ColorInfo , const char* Password )
{
	//////////////////////////////////////////////////////////////////////////
//	ColoringShopStruct* Color; 
	RoleDataEx* Owner = Sender->Role();
	ItemFieldStruct* Item;

	//////////////////////////////////////////////////////////////////////////
	//密碼檢查
	MyMD5Class Md5;
	Md5.ComputeStringHash( Password );
	if( stricmp( Md5.GetMd5Str() , Owner->PlayerBaseData->Password.Begin() ) != 0 )
	{
//		Owner->Net_GameMsgEvent( EM_GameMessageEvent_SecondPasswordError );
		S_ColoringShopResult( Owner->GUID() , EM_ColoringShopResult_SecondPasswordError );
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//int	ColoringCost = g_ObjectData->GetSysKeyValue( "ColoringCost" );
	int	ColoringCost = g_ObjectData->GetMoneyKeyValue( "ColoringCost" , 0 );
	int	ColoringHair = g_ObjectData->GetMoneyKeyValue( "ColoringHair" , 0 );
	int	ColoringBody = g_ObjectData->GetMoneyKeyValue( "ColoringBody" , 0 );
	int	HairStyle	 = g_ObjectData->GetMoneyKeyValue( "HairStyle" , 0 );
	int	FaceStyle	 = g_ObjectData->GetMoneyKeyValue( "FaceStyle" , 0 );
	
	int Cost = 0;
	int		EqPos[8] = { EM_EQWearPos_Head , EM_EQWearPos_Gloves , EM_EQWearPos_Shoes , EM_EQWearPos_Clothes , EM_EQWearPos_Pants , EM_EQWearPos_Back , EM_EQWearPos_Belt , EM_EQWearPos_Shoulder };
	//檢查是否要求資料沒有問題
	for( int i = 0 ; i < 8 ; i++ )
	{
		Item = Owner->GetEqItem( EqPos[i] );
		if( Item->IsEmpty()  )
		{
			if(	   ColorInfo.Items[i].MainColorEnabled != false
				|| ColorInfo.Items[i].OffColorEnabled  != false )
			{
				S_ColoringShopResult( Owner->GUID() , EM_ColoringShopResult_DataError );
				return;
			}
		}
		//查詢是否可以染色
		GameObjDbStructEx* ItemObjDB = Global::GetObjDB( Item->ImageObjectID );
		if( ItemObjDB == NULL || ItemObjDB->Mode.Coloring == false )
		{
			//S_ColoringShopResult( Owner->GUID() , EM_ColoringShopResult_DataError );
			//return;
			continue;
		}


		if( ColorInfo.Items[i].MainColorEnabled )
			Cost += ColoringCost;
		if( ColorInfo.Items[i].OffColorEnabled )
			Cost += ColoringCost;
	}

	if( ColorInfo.HairColorEnabled )
	{
		Cost = Cost + ColoringHair;
	}

	if( ColorInfo.BodyColorEnabled )
	{
		Cost = Cost + ColoringBody;
	}

	
	GameObjDbStructEx* HairObjDB = g_ObjectData->GetObj( ColorInfo.HairStyle );
	if( ColorInfo.HairStyle != -1 )
	{
		if( HairObjDB->IsBody() == false || HairObjDB->BodyObj.Type  != EM_BodyObjectType_Hair )
		{
			S_ColoringShopResult( Owner->GUID() , EM_ColoringShopResult_DataError );
			return;
		}
		
		if(		TestBit( HairObjDB->BodyObj.Limit.Sex._Sex , Owner->BaseData.Sex ) == 0 
			||	TestBit( HairObjDB->BodyObj.Limit.Race._Race , Owner->BaseData.Race ) == 0 
			||	HairObjDB->BodyObj.Limit.DepartmentStore == false		)
		{
			S_ColoringShopResult( Owner->GUID() , EM_ColoringShopResult_DataError );
			return;
		}

		Cost = Cost + HairStyle;
	}

	GameObjDbStructEx* FaceObjDB = g_ObjectData->GetObj( ColorInfo.FaceStyle );
	if( ColorInfo.FaceStyle != -1 )
	{
		if( FaceObjDB->IsBody() == false || FaceObjDB->BodyObj.Type  != EM_BodyObjectType_Face )
		{
			S_ColoringShopResult( Owner->GUID() , EM_ColoringShopResult_DataError );
			return;
		}

		if(		TestBit( FaceObjDB->BodyObj.Limit.Sex._Sex , Owner->BaseData.Sex ) == 0 
			||	TestBit( FaceObjDB->BodyObj.Limit.Race._Race , Owner->BaseData.Race ) == 0 
			||	FaceObjDB->BodyObj.Limit.DepartmentStore == false		)
		{
			S_ColoringShopResult( Owner->GUID() , EM_ColoringShopResult_DataError );
			return;
		}

		Cost = Cost + FaceStyle;
	}

	//檢查金錢
	if( Owner->PlayerBaseData->Body.Money_Account < Cost )
	{
		S_ColoringShopResult( Owner->GUID() , EM_ColoringShopResult_AccountMoneyError );
		return;
	}

	//檢查帳號幣現在是否能操作
	if (Global::AccountMoneyOperable() == false)
	{
		S_ColoringShopResult( Owner->GUID() , EM_ColoringShopResult_AccountMoneyError );
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//顏色處理
	//////////////////////////////////////////////////////////////////////////
	for( int i = 0 ; i < 8 ; i++ )
	{
		bool IsChange = false;
		Item = Owner->GetEqItem( EqPos[i] );
		if( Item->IsEmpty()  )
		{
			continue;
		}
		GameObjDbStructEx* ItemObjDB = Global::GetObjDB( Item->ImageObjectID );
		if( ItemObjDB == NULL || ItemObjDB->Mode.Coloring == false )
			continue;

		if( ColorInfo.Items[i].MainColorEnabled )
		{
			Item->MainColor = ColorInfo.Items[i].MainColor;
			IsChange = true;
		}

		if( ColorInfo.Items[i].OffColorEnabled )
		{
			Item->OffColor = ColorInfo.Items[i].OffColor;
			IsChange = true;
		}

		if( IsChange )
		{
			NetSrv_Item::FixItemInfo_EQ( Owner->GUID() , *Item , EqPos[i] );
		}
		
	}

	if( ColorInfo.HairColorEnabled )
	{
		Owner->BaseData.Look.HairColor = ColorInfo.HairColor;
	}

	if( ColorInfo.BodyColorEnabled )
	{
		Owner->BaseData.Look.BodyColor = ColorInfo.BodyColor;
	}
/*
	Color = g_ObjectData->GetColoringShop( ColorInfo.HairStyle );
	if( Color != NULL )
	{
		Owner->BaseData.Look.HairID = Color->ObjID;
	}

	Color = g_ObjectData->GetColoringShop( ColorInfo.FaceStyle );
	if( Color != NULL )
	{
		Owner->BaseData.Look.FaceID = Color->ObjID;
	}
	*/
	if( ColorInfo.HairStyle != -1 )
		Owner->BaseData.Look.HairID = ColorInfo.HairStyle;
	if( ColorInfo.FaceStyle != -1 )
		Owner->BaseData.Look.FaceID = ColorInfo.FaceStyle;

	Owner->AddBodyMoney_Account( Cost * -1 , NULL , EM_ActionType_ColoringShop_Player , true , true );
	
	//把角色移除再加入
	int RoomID = Owner->RoomID();
	Global::DelFromPartition( Owner->GUID() );
	Global::AddToPartition( Owner->GUID() , RoomID );

	S_ColoringShopResult( Owner->GUID() , EM_ColoringShopResult_OK );
	//////////////////////////////////////////////////////////////////////////
}
void NetSrv_OtherChild::R_PlayerInfoRequest		( BaseItemObject* Sender , int PlayerGItemID )
{
	RoleDataEx* Target = Global::GetRoleDataByGUID( PlayerGItemID );

	if( Target == NULL || Target->IsPlayer() == false )
	{
		S_PlayerInfo( Sender->GUID() , false , PlayerGItemID );
	}
	else
	{
		S_PlayerInfo( Sender->GUID() , true , PlayerGItemID , &Target->BaseData.EQ , Target->PlayerSelfData->FriendList.Note.Begin() );
	}
}

void NetSrv_OtherChild::R_SysKeyFunctionRequest	( BaseItemObject* Sender , int KeyID , int TargetGUID )
{
	RoleDataEx* Owner = Sender->Role();
	int MagicID = g_ObjectData->SysKeyFunction( KeyID );
	if(		Owner->BaseData.KeyItem.GetFlag( KeyID ) == false 
		||	MagicID == 0 )
	{
		S_SysKeyFunctionResult( Owner->GUID() , false , KeyID , TargetGUID );
		return;
	}
	RoleDataEx* Target = Global::GetRoleDataByGUID( TargetGUID );
	if( Target == NULL )
		Target = Owner;
	
	MagicProcessClass::SpellMagic( Owner->GUID() , TargetGUID , Target->X() , Target->Y() , Target->Z() , MagicID , 0 );	

	S_SysKeyFunctionResult( Owner->GUID() , true , KeyID , TargetGUID );
}

void NetSrv_OtherChild::R_AllObjectPosRequest		( BaseItemObject* Sender )
{
	vector<ObjectPosInfoStruct> PosInfo;
	RoleDataEx* Owner = Sender->Role();

	if( Owner->PlayerBaseData->ManageLV <= EM_Management_GameVisitor )
		return;

	vector< BaseItemObject* >& ZoneList = *BaseItemObject::GetZoneObjList();
	for( int i = 0 ; i < (int)ZoneList.size() && PosInfo.size() < 10000 ; i++ )
	{
		BaseItemObject* Obj = ZoneList[i];
		if( Obj == NULL )
			continue;
		if( Obj->Role()->RoomID() != 0 )
			continue;

		ObjectPosInfoStruct TempPos;
		MyStrcpy( TempPos.Name , Obj->Role()->RoleName() , sizeof( TempPos.Name ) );

		TempPos.X = Obj->Role()->X();
		TempPos.Y = Obj->Role()->Y();
		TempPos.Z = Obj->Role()->Z();
		TempPos.GUID = Obj->Role()->GUID();
		TempPos.OrgObjID = Obj->Role()->BaseData.ItemInfo.OrgObjID;
		TempPos.Level = Obj->Role()->Level();
		PosInfo.push_back( TempPos );
	}	
	S_ObjectPosInfo( Owner->GUID() , PosInfo );
}

void NetSrv_OtherChild::R_RunGlobalPlotRequest		( BaseItemObject*Obj , int PlotID , int Value )
{
	char	PlotName[512];
	sprintf( PlotName , "ClientGlobalSysFunction%d(%d)" , PlotID , Value );
	vector<MyVMValueStruct> RetList;
	bool Ret = true;
	if( LUA_VMClass::PCallByStrArg( PlotName , Obj->GUID() , Obj->GUID() , &RetList , 1 ) )
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

	S_RunGlobalPlotResult( Obj->GUID() , Ret );
}
void NetSrv_OtherChild::R_HateListRequest			( BaseItemObject*Obj , int NpcGUID )
{
	RoleDataEx* Owner = Obj->Role();
	if( Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_HateList ] > 1 )
		return;
	Owner->PlayerTempData->PacketCheckPoint[ EM_ClientPacketCheckType_HateList ] += (1.0f/60.0f);

	RoleDataEx* NPC = Global::GetRoleDataByGUID( NpcGUID );
	if( NPC == NULL || NPC->IsNPC() == false || NPC->IsAttack() == false )
	{
		NPCHateList NPCHate;
		NPCHate.Init();
		S_HateListResult( Obj->GUID() , NpcGUID , NPCHate );
		return;
	}

	S_HateListResult( Obj->GUID() , NpcGUID , NPC->TempData.NPCHate );
}

void NetSrv_OtherChild::R_ManagePasswordKey		( BaseItemObject* Obj )
{
	/*
	if( ManageKey == 0 )
	{
		while( 1 )
		{
			ManageKey = rand();
			if( ManageKey > 100 )
				break;
		}	
	}
	
	Obj->Role()->Msg( IntToStr(ManageKey).c_str() );
	*/
}

void NetSrv_OtherChild::R_ManagePassword		( BaseItemObject* Obj , const char* Md5Pwd )
{
	return;
	/*
	if( ManageKey == 0 )
		return;

	char* Key = DecodePassword( Md5Pwd , "d6F8PcZzFscs" );
	
	if( strcmp( Key  , IntToStr(ManageKey).c_str() ) == 0 )
	{
		Obj->Role()->Msg( "OK" );
		Obj->Role()->TempData.Sys.GM_Designed = true;
		//Obj->Role()->BaseData.SysFlag.EnableGMCmd = true;
		//Obj->Role()->SetValue( EM_RoleValue_SysFlag , Owner->BaseData.SysFlag._Value , NULL  );
	}

	ManageKey = 0;
	*/
}

void NetSrv_OtherChild::RC_CloseMaster( BaseItemObject* Obj , const char* Password )
{
	return;
	/*
	if( ManageKey == 0 )
		return;

	char* Key = DecodePassword( Password , "d6F8PcZzFscs" );

	if( strcmp( Key  , IntToStr(ManageKey).c_str() ) == 0 )
	{
		CNetSrv_ZoneStatus::SM_CloseMaster();
	}

	ManageKey = 0;
	*/
}

void NetSrv_OtherChild::R_GiveItemPlot				( BaseItemObject* Obj , bool IsSort , int ItemPos[4] )
{
	RoleDataEx* Owner = Obj->Role();

	if( Owner->TempData.Attr.Action.OpenType != EM_RoleOpenMenuType_PlotGiveItem )
		return;

	RoleDataEx* Target = Global::GetRoleDataByGUID( Owner->TempData.ShopInfo.TargetID );
	if( Target == NULL || Target->Length( Owner ) > 150 )
		return;

	if( Target->TempData.GiveItemPlot.Size() == 0 )
		return;

	ItemFieldStruct* BodyItem[4];
	ItemFieldStruct  EmptyItem;

	EmptyItem.Init();

	for( int i = 0 ; i < 4 ; i++ )
	{
		BodyItem[i] = Owner->GetBodyItem( ItemPos[i] );

		if( BodyItem[i] == NULL )
			BodyItem[i] = &EmptyItem;
	}

	ItemFieldStruct Item[4];
	for( int i = 0 ; i < 4 ; i++ )
	{
		Item[i] = *BodyItem[i];
	}

	if( IsSort != false )
	{
		for( int i = 3 ; i >= 1 ; i-- )
		{
			if( Item[i].OrgObjID == 0 )
				continue;

			for( int j = 0 ; j < i ; j++ )
			{
				if( Item[j].OrgObjID == 0 ) 
				{
					std::swap( Item[i] , Item[j] );
					break;
				}

				if( Item[i].OrgObjID == Item[j].OrgObjID ) 
				{
					Item[j].Count += Item[i].Count;

					for( int x = i ; x < 3 ; x++ )
					{
						Item[x] = Item[x+1];
					}
					Item[3].Init();
					continue;
				}

				if( Item[i].OrgObjID < Item[j].OrgObjID ) 
				{
					std::swap( Item[i] , Item[j] );
					continue;
				}
			}
		}
	}
	
	char PlotStr[512];
	sprintf( PlotStr , "%s(%d,%d,%d,%d,%d,%d,%d,%d)" , Target->TempData.GiveItemPlot.Begin() 
									, Item[0].OrgObjID , Item[0].Count 
									, Item[1].OrgObjID , Item[1].Count 
									, Item[2].OrgObjID , Item[2].Count 
									, Item[3].OrgObjID , Item[3].Count );

	vector<MyVMValueStruct> RetList;
	bool Ret = true;
	if( LUA_VMClass::PCallByStrArg( PlotStr , Owner->GUID() , Target->GUID() , &RetList , 1 ) )
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


	if( Ret != false )
	{
		for( int i = 0 ; i < 4 ; i++ )
		{

			if( BodyItem[i]->OrgObjID == 0 )
				continue;

			Log_ItemDestroy( Owner , EM_ActionType_PlotDestory , *(BodyItem[i]) , BodyItem[i]->Count , -1 , "" );	
			BodyItem[i]->Init();

			Owner->Net_FixItemInfo_Body( ItemPos[i] );
		}

	}

	Owner->TempData.Attr.Action.OpenType  = EM_RoleOpenMenuType_None;
	Owner->TempData.ShopInfo.TargetID = -1;
}

void NetSrv_OtherChild::R_CheckPassword			( BaseItemObject* Obj , char* Password )
{
	RoleDataEx *Owner = Obj->Role();

	MyMD5Class Md5;
	Md5.ComputeStringHash( Password );
	if( stricmp( Md5.GetMd5Str() , Owner->PlayerBaseData->Password.Begin() ) != 0 )
	{
		S_CheckPasswordResult( Owner->GUID() , Password , false );
	}
	else
	{
		S_CheckPasswordResult( Owner->GUID() , Password , true );
	}
}

//-----------------------------------------------------------------
void NetSrv_OtherChild::RC_Find_DBID_RoleName( BaseItemObject* Obj , int DBID , const char* RoleName )
{
	SD_Find_DBID_RoleName( Obj->Role()->DBID() , DBID , RoleName );
}
//-----------------------------------------------------------------
//void NetSrv_OtherChild::RC_SendPerformance(  BaseItemObject* Obj, const char* cMac, float MaxCPUUsage, float MaxMemUsage, float MaxFPS, float AverageCPUUsage, float AverageMemUsage, float AverageFPS, float MinCPUUsage, float MinMemUsage, float MinFPS, float Ping, float PosX, float PosY, float PosZ )
//{
//	SD_SendPerformance(  Obj->Role()->DBID(), cMac, MaxCPUUsage, MaxMemUsage, MaxFPS, AverageCPUUsage, AverageMemUsage, AverageFPS, MinCPUUsage, MinMemUsage, MinFPS, Ping, PosX, PosY, PosZ );
//}

void NetSrv_OtherChild::SendRange_GoodEvilValue		( RoleDataEx *Owner )
{
	float GoodEvilValue = Owner->PlayerBaseData->GoodEvil;
	GoodEvilTypeENUM GoodEvilType = Owner->GetGoodEvilType();

	RoleDataEx* Other;
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;

		S_GoodEvilValue		( Other->GUID()  , Owner->GUID() , GoodEvilValue , GoodEvilType );
	}
}

void NetSrv_OtherChild::SendRangeCampID( RoleDataEx *Owner , CampID_ENUM CampID )
{
	RoleDataEx* Other;
	Global::ResetRange( Owner , _Def_View_Block_Range_ );
	while( 1 ) 
	{
		Other = Global::GetRangePlayer();
		if( Other == NULL )
			break;
		S_CampID( Other->GUID()  , Owner->GUID() , CampID );
	}
}

void NetSrv_OtherChild::R_DialogInput( BaseItemObject* Sender  , int SelectID, const char* pszInput, int iCheckResult )
{
	int iGUID = Sender->GUID();
	Struct_DialogTempInfo Info;

	Info.iSelectID		= SelectID;
	Info.pszString		= pszInput;
	Info.isAllowString	= iCheckResult;

	m_mapDialogInfo[ iGUID ] = Info;

	Sender->Role()->PlayerTempData->DialogSelectID = SelectID;
}

void NetSrv_OtherChild::OnEvent_ClientDisconnect( BaseItemObject* pObj )
{
	int iGUID = pObj->GUID();

	map< int, Struct_DialogTempInfo >::iterator it = m_mapDialogInfo.find( iGUID );

	if( it != m_mapDialogInfo.end() )
	{
		m_mapDialogInfo.erase( it );
	}
}

void NetSrv_OtherChild::RD_ExchangeMoneyTable		( int Count , int DataSize , char* Data )
{
	MoneyKeyValueCount = Count;
	MoneyKeyValueSize = DataSize;
	memcpy( MoneyKeyValueZip , Data , DataSize );
	g_ObjectData->SetExhcnageMoneyKeyValue( Count , DataSize , Data );
}

void NetSrv_OtherChild::RD_DBStringTable			( int Count , int DataSize , char* Data )
{
	DBStringCount = Count;
	DBStringSize = DataSize;
	memcpy( DBStringZip , Data , DataSize );
	g_ObjectData->SetDBStringTable( Count , DataSize , DBStringZip );

	//通知所有Client
	SC_AllCli_DBStringTable( Count , DataSize , Data );
}

void NetSrv_OtherChild::RC_GmNotification ( BaseItemObject* Obj , const char* Subject, const char* Content, const char* Classification )
{
	//新增一筆資料到ImportDB GmReport table
	RoleDataEx* player = Obj->Role();

	//把Unicode Utf-8 轉為 char*
	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String( Subject );
	string subjectStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );

	charTransfer.SetUtf8String( Content );
	string contentStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );

	charTransfer.SetUtf8String( Classification );
	string classStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );

	char sqlBuf[8192];
	sprintf( sqlBuf , "INSERT INTO GMReport(PlayerDBID, Classification, Subject, Message, SendTime, ZoneID, WorldID, X, Y, Z)VALUES(%d, CAST( %s AS nvarchar(256) ), CAST( %s AS nvarchar(1024) ), CAST( %s AS nvarchar(1024) ), getdate(), %d, %d, %d, %d, %d)"
		, player->DBID()
		, classStr.c_str()
		, subjectStr.c_str()
		, contentStr.c_str()
		, RoleDataEx::G_ZoneID
		, Global::_Net->GetWorldID()
		, int(player->Pos()->X)
		, int(player->Pos()->Y)
		, int(player->Pos()->Z)		
		);

	Net_DCBase::SqlCommand_Import( player->DBID() , sqlBuf);
}

void NetSrv_OtherChild::SendClient_DBTable( int SendID )
{
	SC_ExchangeMoneyTable( SendID , MoneyKeyValueCount , MoneyKeyValueSize , MoneyKeyValueZip );
	SC_DBStringTable( SendID , DBStringCount , DBStringSize , DBStringZip );
}
void NetSrv_OtherChild::RC_ColoringHorse( BaseItemObject* Obj , int Color[4] , char* Password , int ItemPos )
{
	//////////////////////////////////////////////////////////////////////////
	RoleDataEx* Owner = Obj->Role();
	ItemFieldStruct* Item = Owner->GetBodyItem( ItemPos );

	if( Item == NULL || Item->IsEmpty() )
	{
		SC_ColoringHorseResult( Owner->GUID() , EM_ColoringShopResult_DataError );
		return; 
	}
	GameObjDbStructEx* ItemObjDB = Global::GetObjDB( Item->OrgObjID );
	if( ItemObjDB->IsItem_Pure() == false || ItemObjDB->Item.ItemType != EM_ItemType_SummonHorse )
	{
		SC_ColoringHorseResult( Owner->GUID() , EM_ColoringShopResult_DataError );
		return; 
	}

	int		ColorChangeCount = 0;

	for( int i = 0 ; i < 4 ; i++ )
	{
		if( Item->HorseColor[i] != Color[i] )
			ColorChangeCount ++;
	}
	
	/*
	if(		Item->HorseColor[0] == Color[0] 
		&&	Item->HorseColor[1] == Color[1] 
		&&	Item->HorseColor[2] == Color[2] 
		&&	Item->HorseColor[3] == Color[3] )
		*/
	if( ColorChangeCount == 0 )
	{
		SC_ColoringHorseResult( Owner->GUID() , EM_ColoringShopResult_DataError );
		return; 
	}
	//////////////////////////////////////////////////////////////////////////
	//密碼檢查
	MyMD5Class Md5;
	Md5.ComputeStringHash( Password );
	if( stricmp( Md5.GetMd5Str() , Owner->PlayerBaseData->Password.Begin() ) != 0 )
	{
		//		Owner->Net_GameMsgEvent( EM_GameMessageEvent_SecondPasswordError );
		SC_ColoringHorseResult( Owner->GUID() , EM_ColoringShopResult_SecondPasswordError );
		return;
	}



	//////////////////////////////////////////////////////////////////////////
	//int	ColoringCost = g_ObjectData->GetSysKeyValue( "ColoringHorseCost" ) * ColorChangeCount;
	int	ColoringCost = g_ObjectData->GetMoneyKeyValue( "ColoringHorseCost" , 0 ) * ColorChangeCount;

	if( ColoringCost > Owner->PlayerBaseData->Body.Money_Account )
	{
		SC_ColoringHorseResult( Owner->GUID() , EM_ColoringShopResult_AccountMoneyError );
		return;
	}

	if (Global::AccountMoneyOperable() == false)
	{
		SC_ColoringHorseResult( Owner->GUID() , EM_ColoringShopResult_AccountMoneyError );
		return;
	}

	memcpy( Item->HorseColor , Color , sizeof(Item->HorseColor) );
	Owner->AddBodyMoney_Account( ColoringCost * -1 , NULL , EM_ActionType_ColoringShop_Horse , true , true );

	Owner->Net_FixItemInfo_Body( ItemPos );
	Global::Log_ItemTrade( Owner->DBID() , -1 , Owner->DBID() , Owner->X() , Owner->Z() , EM_ActionType_ColoringShop_Horse , *Item , "" );

	SC_ColoringHorseResult( Owner->GUID() , EM_ColoringShopResult_OK );
	//////////////////////////////////////////////////////////////////////////
}

void NetSrv_OtherChild::RC_RoleNameDuplicateErr( BaseItemObject* Obj , const char* RoleName , int PlayerDBID[2] )
{
	if( PlayerDBID[0] == PlayerDBID[1] )
		return;

	RoleDataEx*	Player[2];
	char		Buf[2048];

	Player[0] = Global::GetRoleDataByDBID( PlayerDBID[0] );
	Player[1] = Global::GetRoleDataByDBID( PlayerDBID[1] );

	if(		Player[0] == NULL 
		||	Player[1] == NULL  )
	{
		sprintf( Buf , "Sender DBID=%d Duplicate Name = %s PlayerDBID(%d,%d) Error" , Obj->Role()->DBID() , RoleName , PlayerDBID[0] , PlayerDBID[1] );
	}
	else
	{
		sprintf( Buf , "Sender DBID=%d Duplicate Name = %s PlayerDBID(%d,%d) : Player1(name:%s,account:%s,check=%s,pos:%d,%d,%d) Player2(name:%s,account=%s,check=%s,pos:%d,%d,%d)"
			, Obj->Role()->DBID() , RoleName , PlayerDBID[0] , PlayerDBID[1]
			, Player[0]->RoleName() , Player[0]->Account_ID(), Player[0]->PlayerBaseData->Password.Begin(), int( Player[0]->X() ) , int( Player[0]->Y() ) , int( Player[0]->Z() )
			, Player[1]->RoleName() , Player[1]->Account_ID(), Player[1]->PlayerBaseData->Password.Begin(), int( Player[1]->X() ) , int( Player[1]->Y() ) , int( Player[1]->Z() )		);
	}

	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String( Buf );
	string CmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
	sprintf( Buf , "INSERT DebugMessage( MessageType , Message ) VALUES('RoleNameDuplicateErr', CAST( %s AS nvarchar(4000) ) )" , CmdBinStr.c_str() );

	Net_DCBase::LogSqlCommand( Buf );
}

void NetSrv_OtherChild::RC_LookTarget				( BaseItemObject* Obj , int TargetGUID )
{
	Obj->Role()->TempData.LookTargetID = TargetGUID;
}

void NetSrv_OtherChild::RC_SetCycleMagic			( BaseItemObject* Obj , int MagicID , int MagicPos  )
{
	RoleDataEx* Role = Obj->Role();

	if( Role->IsSpell() != false && Role->TempData.Magic.MagicCollectID == Role->TempData.CycleMagic.MagicID )
	{
		Role->TempData.Magic.State = EM_MAGIC_PROC_STATE_INTERRUPT;
	}
	
	if( Role->CheckRoleMagic( MagicID , MagicPos ) == false  )
	{
		Role->TempData.CycleMagic.MagicID = 0;
		Role->TempData.CycleMagic.MagicPos = 0;
	}
	else
	{
		Role->TempData.CycleMagic.MagicID = MagicID;
		Role->TempData.CycleMagic.MagicPos = MagicPos;
	}
}

void NetSrv_OtherChild::RC_SetSendPacketMode		( BaseItemObject* Obj , SendPacketModeStruct& Mode )
{
	Obj->Role()->PlayerTempData->SendPGMode.Mode = Mode.Mode; 
}

void NetSrv_OtherChild::RC_ResetInstanceRequest( BaseItemObject* Obj , int ZoneID )
{
	RoleDataEx* Role = Obj->Role();

	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( Role->PartyID() );
	if( Party == NULL || strcmp( Party->Info.LeaderName , Role->RoleName() ) != 0 )
	{
		SC_ResetInstanceInfo( Obj->Role()->DBID() , ZoneID , true );
		return;
	}

	if( Net_ServerList->CheckServer( EM_SERVER_TYPE_LOCAL , ZoneID) == false )
	{
		SC_ResetInstanceInfo( Obj->Role()->DBID() , ZoneID , true );
		return;
	}
	SL_ResetInstance( ZoneID , Role->PartyID() , Role->DBID() );
}
void NetSrv_OtherChild::RL_ResetInstance( int PlayerDBID , int PartyID )
{
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( PartyID );
	if( Party == NULL  )
	{
		SC_ResetInstanceInfo( PlayerDBID , RoleDataEx::G_ZoneID , true );
		return;
	}
	
	if(		Ini()->IsPrivateZone != false 
		&&	Ini()->PrivateZoneType == EM_PrivateZoneType_None )
	{
		SC_ResetInstanceInfo( PlayerDBID , RoleDataEx::G_ZoneID , true );
		return;
	}

	bool	IsFindMember = false;
	for( int i = 0 ; i < (int)Party->Member.size() ; i++ )
	{
		if( Party->Member[i].ZoneID != RoleDataEx::G_ZoneID )
			continue;

		BaseItemObject* MemberObj = BaseItemObject::GetObj( Party->Member[i].GItemID );
		if( MemberObj == NULL )
			continue;

		SC_ResetInstanceInfo( PlayerDBID , RoleDataEx::G_ZoneID , false );
		return;
	}

	vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;
	for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
	{
		if( RoomList[i].OwnerPartyID == PartyID && RoomList[i].IsActive != false )
		{
			RoomList[i].Init();
			DelRoomMonster( i );
			break;	
		}
	}	
	SC_ResetInstanceInfo( PlayerDBID , RoleDataEx::G_ZoneID , true );
}

void NetSrv_OtherChild::RC_GmResetRolet			( BaseItemObject* Obj , int RoleBaseSize , int PlayerRoleBaseSize , BaseRoleData& BaseData , PlayerBaseRoleData& PlayerBaseData  )
{
	RoleDataEx* Owner = Obj->Role();
	
	if( Owner->PlayerBaseData->ManageLV != EM_Management_GameCreator )
		return;

	if( Owner->BaseData.SysFlag.EnableGMCmd == false )
		return;

	if( sizeof(BaseData) != RoleBaseSize || PlayerRoleBaseSize != sizeof(PlayerBaseData) )
	{
		Owner->Msg( "sizeof(BaseData) != RoleBaseSize || PlayerRoleBaseSize != sizeof(PlayerBaseData) " );
		return;
	}
	Global::DelFromPartition( Owner->GUID() );

	BaseData.PartyID	= Owner->BaseData.PartyID;
	BaseData.GuildID	= Owner->BaseData.GuildID;
	BaseData.RoomID		= Owner->BaseData.RoomID;
	BaseData.ZoneID		= Owner->BaseData.ZoneID;
	BaseData.Pos		= Owner->BaseData.Pos;
	BaseData.RoleName	= Owner->BaseData.RoleName;

	PlayerBaseData.ManageLV = Owner->PlayerBaseData->ManageLV;
	//////////////////////////////////////////////////////////////////////////

	Owner->BaseData = BaseData;
	*(Owner->PlayerBaseData) = PlayerBaseData;

	Owner->InitCheckRoleData();
	Owner->ChangeZoneInit();
	Owner->CheckAndClearCopyItem( );
	Owner->InitCal();

	NetSrv_CliConnectChild::SendPlayerItemID( Owner );

	Global::AddToPartition( Owner->GUID() , Owner->RoomID() );

}

void NetSrv_OtherChild::RC_PartyDice			( BaseItemObject* Obj , int Type )
{
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( Obj->Role()->PartyID() );
	if( Party == NULL  )
		return;
	
	int R = rand();

	for( int i = 0 ; i < (int)Party->Member.size() ; i++ )
	{

		SC_PartyDice( Party->Member[i].DBID , Type , R , Obj->Role()->DBID() );
	}

}

void NetSrv_OtherChild::RC_BodyShop				( BaseItemObject* Obj , char BoneScale[15] )
{
	RoleDataEx* Owner = Obj->Role();

	if( memcmp( BoneScale , Owner->BaseData.Look.BoneScale , sizeof( Owner->BaseData.Look.BoneScale ) ) == 0 )
	{
		SC_BodyShopResult( Owner->GUID() , false );
		return;
	}
	
	if (Global::AccountMoneyOperable() == false)
	{
		SC_BodyShopResult( Owner->GUID() , false );
		return;
	}

	int BodyShopCost = g_ObjectData->GetMoneyKeyValue( "BodyShop" , 0 );
	Owner->AddBodyMoney_Account( BodyShopCost * -1 , NULL , EM_ActionType_BodyShop , true , true );
	SC_BodyShopResult( Owner->GUID() , true );
	memcpy( Owner->BaseData.Look.BoneScale , BoneScale , sizeof( Owner->BaseData.Look.BoneScale ) ); 


	Global::DelFromPartition( Owner->GUID() );
	Global::AddToPartition( Owner->GUID() , Owner->RoomID() );
}

void NetSrv_OtherChild::RC_MoneyVendorReport( BaseItemObject* Obj , const char* Name , const char* Content )
{
	BaseItemObject* TargetObj = Global::GetObjByName( (char*)Name );
	if( TargetObj == NULL )
		return;
	RoleDataEx* Target = TargetObj->Role();

	if( Target->PlayerBaseData->Defendant.MoneyTraderCount > _DEF_MAX_MONEY_VENTOR_COUNT_ )
		return;

	int IncValue = 1;
	if( Target->Level() < 5 )
		IncValue = 20;
	else if( Target->Level() < 10 )
		IncValue = 10;
	else if( Target->Level() < 20 )
		IncValue = 5;
	else 
		IncValue = 1;

	if( Target->PlayerBaseData->Defendant.MoneyTraderCount == 0 )
		Target->PlayerBaseData->Defendant.MoneyTraderCount = IncValue;
	else
		Target->PlayerBaseData->Defendant.MoneyTraderCount = float( int( Target->PlayerBaseData->Defendant.MoneyTraderCount - 0.01 ) + IncValue + 1 );

	if( Target->PlayerBaseData->Defendant.MoneyTraderCount > _DEF_MAX_MONEY_VENTOR_COUNT_ )
		Target->PlayerBaseData->Defendant.MoneyTraderCount  = _DEF_MAX_MONEY_VENTOR_COUNT_;

	Global::Log_Warning_MoneyVendor( Target, Obj->Role()->DBID() , Content );


	SC_MoneyVendorReport( Target->GUID() , int(Target->PlayerBaseData->Defendant.MoneyTraderCount) );
}
void NetSrv_OtherChild::RC_BotReport( BaseItemObject* Obj , const char* Name , const char* Content )
{

	BaseItemObject* TargetObj = Global::GetObjByName( (char*)Name );
	if( TargetObj == NULL )
		return;

	RoleDataEx* Target = TargetObj->Role();

	if( RoleDataEx::G_SysTime - Target->TempData.Move.LastMoveTime > 60*1000 )
	{
		SC_UnlockBotReport( Obj->GUID() , Target->RoleName() , EM_UnlockBotType_TargetNoMove );
		return;
	}

	//目標檢舉已經在處理中
	if( myUInt32(Target->PlayerBaseData->Defendant.BotReportTime + 60*60) > RoleDataEx::G_Now )
	{	
		SC_UnlockBotReport( Obj->GUID() , Target->RoleName() , EM_UnlockBotType_OnPrcess );
		return;
	}

	//目標最近有 被檢舉 並解開
	if( myUInt32(Target->PlayerBaseData->Defendant.BotReportProtectTime) > RoleDataEx::G_Now )
	{	
		SC_UnlockBotReport( Obj->GUID() , Target->RoleName() , EM_UnlockBotType_OnProtect );
		return;
	}

	Target->PlayerBaseData->Defendant.AutoBotCount++;
	Target->PlayerBaseData->Defendant.BotReportDBID = Obj->Role()->DBID();
	Target->PlayerBaseData->Defendant.BotReportTime = RoleDataEx::G_Now;

	Global::Log_Warning_Bot( Target , Obj->Role()->GUID() ,Content );

	SC_BotReport( Target->GUID() , int(Target->PlayerBaseData->Defendant.AutoBotCount) );
	SC_UnlockBotReport( Obj->GUID() , Target->RoleName() , EM_UnlockBotType_OnPrcess );
}

void NetSrv_OtherChild::RC_UnlockBotReport			( BaseItemObject* Obj )
{

	RoleDataEx* Role = Obj->Role();

	if( (myUInt32)(Role->PlayerBaseData->Defendant.BotReportTime + 60*60) < RoleDataEx::G_Now )
	{	
		return;
	}
	
	Role->PlayerBaseData->Defendant.AutoBotCount--;
	if( Role->PlayerBaseData->Defendant.AutoBotCount < 0 )
		Role->PlayerBaseData->Defendant.AutoBotCount = 0;
	//Target->PlayerBaseData->Defendant.BotReportDBID = Obj->Role()->DBID();
	Role->PlayerBaseData->Defendant.BotReportProtectTime = RoleDataEx::G_Now + 60*60;
	Role->PlayerBaseData->Defendant.BotReportTime = 0;

	SC_UnlockBotReport( Role->PlayerBaseData->Defendant.BotReportDBID , Role->RoleName() , EM_UnlockBotType_Unlock );
	
}

bool	CompareItemOrder( ItemFieldStruct& Item1 , ItemFieldStruct& Item2 )
{
	GameObjDbStruct* ItemDB1 = Global::GetObjDB( Item1.OrgObjID );
	GameObjDbStruct* ItemDB2 = Global::GetObjDB( Item2.OrgObjID );

	if( ItemDB1 == NULL && ItemDB2 == NULL )
		return false;

	return  g_ObjectData->CompareItemOrder( ItemDB1 , ItemDB2 ) > 0;
}


void NetSrv_OtherChild::RC_SortBankRequest			( BaseItemObject* Obj )
{
	RoleDataEx* Role = Obj->Role();

	BankItemStruct   Bank = Role->PlayerBaseData->Bank;

	//std::sort( &Bank.Item[0] , &Bank.Item[ Bank.Count ] , CompareItemOrder );
	for( int i = 0 ; i < 5 ; i++ )
		std::sort( &Bank.Item[i*40] , &Bank.Item[ i*40 + 40] , CompareItemOrder );

	std::sort( &Bank.Item[_DEF_BANK_MALL_BEGIN_POS_] , &Bank.Item[ _DEF_BANK_MALL_BEGIN_POS_ + _DEF_BANK_MALL_COUNT_] , CompareItemOrder );
	
	for( unsigned i = 0 ; i < _MAX_BANK_COUNT_ ; i++ )
	{
		if( Bank.Item[i] != Role->PlayerBaseData->Bank.Item[i] )
		{
			//Role->Net_FixItemInfo_Bank( i );
			NetSrv_Item::FixItemInfo_Bank( Role->GUID() , Bank.Item[i] , i );
		}
	}

	Role->PlayerBaseData->Bank = Bank;
	SC_SortBankEnd( Role->GUID() );
}

void NetSrv_OtherChild::RC_LoadClientData			( BaseItemObject* Obj , int KeyID )
{
	SD_LoadClientData( Obj->Role()->DBID() , KeyID );
}
void NetSrv_OtherChild::RC_SaveClientData			( BaseItemObject* Obj , int KeyID , int PageID , int Size , const char* Data )
{
	RoleDataEx* Role = Obj->Role();

	if( Size > 1024 || Size <= 0 )
	{
		SC_SaveClientDataResult( Role->GUID() ,  KeyID , false );
		return;
	}

	char SqlCmd[0x1000];
	if( PageID == 0 )
	{
		sprintf( SqlCmd , "DELETE Table_ClientData WHERE DBID=%d and KeyID=%d" 
			, Role->DBID()
			, KeyID );

		Net_DCBase::SqlCommand( Role->DBID() , SqlCmd );
	}


	string CmdBinStr = StringToSqlX( Data , Size , false );
	sprintf( SqlCmd , "INSERT Table_ClientData(DBID,KeyID,PageID,Size,Data) VALUES(%d,%d,%d,%d,cast(%s as binary(1024)))" 
			, Role->DBID()
			, KeyID
			, PageID 
			, Size 
			, CmdBinStr.c_str() );
	Net_DCBase::SqlCommand( Role->DBID() , SqlCmd );

	SC_SaveClientDataResult( Role->GUID() ,  KeyID , true );
}

//////////////////////////////////////////////////////////////////////////
//設定地圖 Iocn 與 Tip
bool  NetSrv_OtherChild::ResetMapMark	( int RoomID )
{
	if( (unsigned)RoomID >= AllMarkInfo.size() )
	{
		return false;
	}
	AllMarkInfo[RoomID].clear();
	SC_AllRoom_ClsMapMark( RoomID , -1 );
	return true;
}
bool NetSrv_OtherChild::SetMapMark		( int RoomID , int MarkID , float X , float Y , float Z , const char* TipStr , int IconID )
{
	if( unsigned(RoomID) > 1000 )
		return false;

	while( AllMarkInfo.size() <= RoomID )	
	{
		map< int , MapMarkInfoStruct > Temp;
		AllMarkInfo.push_back( Temp );
	}

	MapMarkInfoStruct MapInfo;
	MapInfo.IconID = IconID;
	MapInfo.MarkID = MarkID;
	MapInfo.Tip = TipStr;
	MapInfo.X = X;
	MapInfo.Y = Y;
	MapInfo.Z = Z;

	AllMarkInfo[RoomID][MarkID] = MapInfo;
	SC_AllRoom_SetMapMark( RoomID , MarkID , X , Y , Z , TipStr , IconID );
	return true;
}
bool NetSrv_OtherChild::ClsMapMark		( int RoomID , int MarkID )
{
	if( (unsigned)RoomID >= AllMarkInfo.size() )
	{
		return false;
	}

	AllMarkInfo[RoomID].erase( MarkID );
	SC_AllRoom_ClsMapMark( RoomID , MarkID );
	return true;
}

void NetSrv_OtherChild::OnEnterSendMapMark( int SendID )
{
	RoleDataEx* Role = Global::GetRoleDataByGUID( SendID );
	if( Role == NULL )
		return;

	if( (unsigned)Role->RoomID() >= AllMarkInfo.size() )
		return;

	map< int , MapMarkInfoStruct >& MapInfoList = AllMarkInfo[ Role->RoomID() ];
	map< int , MapMarkInfoStruct >::iterator Iter;
	for( Iter = MapInfoList.begin() ; Iter != MapInfoList.end() ; Iter++ )
	{
		MapMarkInfoStruct& MapInfo = Iter->second;
		SC_SetMapMark( SendID , MapInfo.MarkID , MapInfo.X , MapInfo.Y , MapInfo.Z , MapInfo.Tip.Begin() , MapInfo.IconID );
	}
}

void NetSrv_OtherChild::RC_Suicide				( BaseItemObject* Obj , int Type )
{
	Obj->Role()->AddHP( Obj->Role() , -1 * Obj->Role()->MaxHP() -1000);
	Print( LV4 , "RC_Suicide" , "Type = %d" , Type );
}

bool NetSrv_OtherChild::ResetRoomValue	( int RoomID )
{
	if( (unsigned)RoomID >= AllRoomValueInfo.size() )
	{
		return false;
	}
	AllRoomValueInfo[RoomID].clear();
	SC_AllRoom_ClsRoomValue( RoomID , -1 );
	return true;

}
bool NetSrv_OtherChild::SetRoomValue	( int RoomID , int KeyID , int Value )
{
	if( unsigned(RoomID) > 1000 )
		return false;

	while( AllRoomValueInfo.size() <= RoomID )	
	{
		map< int , int > Temp;
		AllRoomValueInfo.push_back( Temp );
	}

	AllRoomValueInfo[RoomID][KeyID] = Value;
	SC_AllRoom_SetRoomValue( RoomID , KeyID , Value );
	return true;
}
int  NetSrv_OtherChild::GetRoomValue	( int RoomID , int KeyID )
{
	if( AllRoomValueInfo.size() <= RoomID )	
	{
		return 0;
	}

	map< int , int >::iterator Iter = AllRoomValueInfo[RoomID].find( KeyID );
	if( Iter == AllRoomValueInfo[RoomID].end() )
		return 0;

	return Iter->second;
}
bool NetSrv_OtherChild::ClsRoomValue	( int RoomID , int KeyID )
{
	if( (unsigned)RoomID >= AllRoomValueInfo.size() )
	{
		return false;
	}

	AllRoomValueInfo[RoomID].erase( KeyID );
	SC_AllRoom_ClsRoomValue( RoomID , KeyID );
	return true;
}

void NetSrv_OtherChild::OnEnterSendRoomValue( int SendID )
{
	RoleDataEx* Role = Global::GetRoleDataByGUID( SendID );
	if( Role == NULL )
		return;

	if( (unsigned)Role->RoomID() >= AllRoomValueInfo.size() )
		return;

	map< int , int >& ValueList = AllRoomValueInfo[ Role->RoomID() ];
	map< int , int >::iterator Iter;
	for( Iter = ValueList.begin() ; Iter != ValueList.end() ; Iter++ )
	{
		SC_SetRoomValue( SendID , Iter->first , Iter->second );
	}
}
void NetSrv_OtherChild::RC_SpellExSkill( BaseItemObject* Obj , int TargetID , float TargetX , float TargetY , float TargetZ , ExSkillBaseStruct& Info , int ShootAngle , int ExplodeTime )
{
	RoleDataEx* Owner = Obj->Role();
	

	StaticVector< ExSkillBaseStruct , _MAX_EX_SKILL_COUNT_ >&	ExSkill = Owner->PlayerTempData->ExSkill; //劇情設定額外可以使用的法術

	bool IsFind = false;
	
	for( int i = 0 ; i < ExSkill.Size() ; i++ )
	{
		if( ExSkill[i] == Info )
		{
			IsFind = true;
			break;
		}
	}

	if( IsFind == false )
	{
		SC_SpellExSkillResult( Owner->GUID() , false , EM_RoleErrorMsg_SpellExSkill_SkillNotFind );
		return;
	}

	RoleDataEx* Speller = Global::GetRoleDataByGUID( Info.SpellerID );
	if( Speller == NULL )
	{
		SC_SpellExSkillResult( Owner->GUID() , false , EM_RoleErrorMsg_SpellExSkill_SpellerNotExist );
		return;
	}

	RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );
	if( Target == NULL )
		Target = Speller;

	GameObjDbStructEx* SpellMagicCol = Global::GetObjDB( Info.SkillID );
	if( SpellMagicCol == NULL )
	{
		SC_SpellExSkillResult( Owner->GUID() , false , EM_RoleErrorMsg_SpellExSkill_MagicColErr );
		return;
	}


	if( Speller->IsSpell() != false )
	{
		if( Speller->TempData.Magic.MagicCollectID == Info.SkillID )
		{
			SC_SpellExSkillResult( Owner->GUID() , false , EM_RoleErrorMsg_SpellExSkill_OnSpellErr );
			return;
		}
		//取得正在施展的法術資料
		GameObjDbStructEx* MagicCol = Global::GetObjDB( Info.SkillID );

		//取得正在施展的法術資料		
		if( MagicCol != NULL && MagicCol->MagicCol.Flag.Interrupt_SpellOtherMagic )
		{
			if( Speller->TempData.Magic.State != EM_MAGIC_PROC_STATE_NORMAL )
				Speller->TempData.Magic.State = EM_MAGIC_PROC_STATE_INTERRUPT;

			MagicProcessClass::Process( Speller , &(Speller->TempData.Magic) );
			SC_SpellExSkillResult( Owner->GUID() , true , EM_RoleErrorMsg_None );
		}
		else
		{
			SC_SpellExSkillResult( Owner->GUID() , false , EM_RoleErrorMsg_SpellExSkill_OnSpellErr );
			return;
		}

	}

	RoleDataEx::LastProcErr = EM_RoleErrorMsg_None;
	bool Ret = MagicProcessClass::SpellMagic( Speller->GUID() , TargetID , TargetX , TargetY , TargetZ , Info.SkillID , Info.SkillLv , ShootAngle );	
	if( Ret != false )
	{
		Speller->TempData.Magic.BaseShootDelayTime = ExplodeTime;
	}
	SC_SpellExSkillResult( Owner->GUID() , Ret , RoleDataEx::LastProcErr );
}

void NetSrv_OtherChild::RC_ClientEventLog		( BaseItemObject* Obj , int EventType )
{
	char Buf[512];
	sprintf( Buf , "INSERT ClientEventLog(PlayerDBID,EventType) VALUES(%d,%d)" , Obj->Role()->DBID() , EventType );
	Net_DCBase::LogSqlCommand( Buf );
}
void NetSrv_OtherChild::RC_TransferData_Range	( BaseItemObject* Obj , char Data[20] )
{	
	SC_TransferData_Range( Obj , Data );
}

void NetSrv_OtherChild::RC_SetRoleValue		( BaseItemObject* Obj , RoleValueName_ENUM Type , double Value )
{
	RoleDataEx* Owner = Obj->Role();
	if( Owner->PlayerBaseData->ManageLV >= EM_Management_GameCreator )
	{
		if( Owner->BaseData.SysFlag.EnableGMCmd == false )
			return;

		Owner->SetValue( Type , Value );
		SC_SetRoleValueResult( Owner->GUID() , true );
		return;
	}
	else
	{

		int ID = Type - EM_RoleValue_ClinetSetting;
		if( ID < _MAX_CLIENT_SETTING_SIZE_  )
		{
			Owner->PlayerBaseData->ClinetSetting[ID] = int(Value);
			Owner->Net_FixRoleValue( Type , Value );
			SC_SetRoleValueResult( Owner->GUID() , true );
			return;
		}	
	}

	SC_SetRoleValueResult( Owner->GUID() , false );
}

void NetSrv_OtherChild::RD_SysKeyValue( int TotalCount , int ID , char* KeyStr , int Value )
{
	if( ID == 0 )
		SysKeyValue.clear();

	_strlwr( KeyStr );
	SysKeyValue[KeyStr] = Value;
}

int NetSrv_OtherChild::GetDBKeyValue( char* KeyStr )
{
	_strlwr( KeyStr );
	map< string , int >::iterator Iter;
	Iter = SysKeyValue.find( KeyStr );
	if( Iter == SysKeyValue.end() )
		return 0;
	return Iter->second;
}

void NetSrv_OtherChild::RC_ClientLanguage( BaseItemObject* Obj, int iLanguage )
{
	Obj->CliMiscInfo()->iLanguage = iLanguage;
}

void NetSrv_OtherChild::RD_RoleRunPlot( const char* LuaPlot, int DBID )
{
	BaseItemObject* PlayerObj = BaseItemObject::GetObjByDBID( DBID );
	if( PlayerObj == NULL )
		return;

	RoleDataEx* Owner = PlayerObj->Role();

	if (Owner == NULL)
		return;

	if (Owner->IsPlayer() == false)
		return;

	if (strlen(LuaPlot) > 0)
	{
		PlayerObj->PlotVM()->CallLuaFunc( LuaPlot , Owner->GUID() );
	}	
}

void NetSrv_OtherChild::RC_SetNewTitleSys	( BaseItemObject* Obj , NewTitleSysPosENUM Pos , int TitleID )
{
	RoleDataEx* Owner = Obj->Role();
	TitleSysStruct&	TitleSys = Owner->PlayerBaseData->TitleSys;

	GameObjDbStructEx* TitleObjDB = GetObjDB( TitleID );
	if( TitleID != 0 )
	{
		if(		TitleObjDB->IsTitle() == false 
			||	Owner->PlayerBaseData->Title.GetFlag(TitleID - Def_ObjectClass_Title ) == false )
		{
			SC_SetNewTitleSysResult( Owner->GUID() , false );
			return;
		}
		if( TitleObjDB->Rare == 0 || TitleObjDB->Rare > TitleSys.Lv )
		{
			SC_SetNewTitleSysResult( Owner->GUID() , false );
			return;
		}

		bool CheckOK = false;
		switch( Pos )
		{
		case EM_NewTitleSysPos_Quest:
			if( TitleObjDB->Title.Classify[0] ==  EM_TitleClassify_Quest )
				CheckOK = true;
			break;
		case EM_NewTitleSysPos_Challenge:
			if( TitleObjDB->Title.Classify[0] ==  EM_TitleClassify_Challenge )
				CheckOK = true;
			break;
		case EM_NewTitleSysPos_Sys:
			if( TitleObjDB->Title.Classify[0] ==  EM_TitleClassify_Sys )
				CheckOK = true;
			break;
		case EM_NewTitleSysPos_Activitie:
			if( TitleObjDB->Title.Classify[0] ==  EM_TitleClassify_Activitie )
				CheckOK = true;
			break;
		}

		if( CheckOK == false )
		{
			SC_SetNewTitleSysResult( Owner->GUID() , false );
			return;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//穿脫頭銜觸發
	//////////////////////////////////////////////////////////////////////////	
	{
		int orgTitleID = Owner->GetValue( RoleValueName_ENUM( EM_RoleValue_TitleSys_TitleID1 + Pos ) );
		char	luaEvent[128];
		GameObjDbStructEx* oldTitleObjDB = Global::GetObjDB( orgTitleID );
		if( oldTitleObjDB->IsTitle() && strlen(oldTitleObjDB->Title.TitleSysLuaEvent) != 0)
		{
			sprintf( luaEvent , "%s(0)",oldTitleObjDB->Title.TitleSysLuaEvent );
			LUA_VMClass::PCallByStrArg( luaEvent , Owner->GUID() , Owner->GUID()  );
		}
	}

	Owner->SetValue( RoleValueName_ENUM( EM_RoleValue_TitleSys_TitleID1 + Pos ) , TitleID );

	{
		char	luaEvent[128];
		if( TitleObjDB->IsTitle() && strlen(TitleObjDB->Title.TitleSysLuaEvent) != 0)
		{
			sprintf( luaEvent , "%s(1)",TitleObjDB->Title.TitleSysLuaEvent );
			LUA_VMClass::PCallByStrArg( luaEvent , Owner->GUID() , Owner->GUID()  );
		}
	}


	SC_SetNewTitleSysResult( Owner->GUID() , true );
}

void NetSrv_OtherChild::RC_NewTitleSys_UseItem( BaseItemObject* Obj , int Type , int Pos )
{
	RoleDataEx* Role = Obj->Role();
	ItemFieldStruct* SrvItem = Role->GetItem( Pos , Type );
	if( SrvItem == NULL )
	{
		SC_NewTitleSys_UseItemResult( Role->GUID() , EM_NewTitleSysUseItemResult_ItemNotFind );
		return;
	}

	GameObjDbStructEx* ObjDB = Global::GetObjDB( SrvItem->OrgObjID );
	if(		ObjDB->IsItem_Pure() == false 
		||	ObjDB->Item.ItemType != EM_ItemType_TitleItem )
	{
		SC_NewTitleSys_UseItemResult( Role->GUID() , EM_NewTitleSysUseItemResult_ItemNotFind );
		return;
	}
	
	if( Role->PlayerBaseData->TitleSys.Famous >= 100 && ObjDB->Item.TitleSys.Famous > 0 )
	{
		SC_NewTitleSys_UseItemResult( Role->GUID() , EM_NewTitleSysUseItemResult_NoEffect );
		return;
	}

	int maxExp = g_ObjectData->GetSysKeyValueArg( "TitleSys_LvExp_0%d" , Role->PlayerBaseData->TitleSys.Lv );

	if( Role->PlayerBaseData->TitleSys.Exp >= maxExp && ObjDB->Item.TitleSys.Exp > 0  )
	{
		SC_NewTitleSys_UseItemResult( Role->GUID() , EM_NewTitleSysUseItemResult_NoEffect );
		return;
	}

	Role->Log_ItemDestroy(  EM_ActionType_UseItemDestroy , *SrvItem , 1 , "" );		
	//清除欄位資料
	if( SrvItem->Count <= 1 )
		SrvItem->Init();
	else
		SrvItem->Count --;

	Role->Net_FixItemInfo( Pos , Type );

	Role->AddValue( EM_RoleValue_TitleSys_Famous , ObjDB->Item.TitleSys.Famous );
	if( Role->PlayerBaseData->TitleSys.Exp + ObjDB->Item.TitleSys.Exp <= maxExp )
		Role->AddValue( EM_RoleValue_TitleSys_Exp , ObjDB->Item.TitleSys.Exp );
	else
		Role->SetValue( EM_RoleValue_TitleSys_Exp , maxExp );

	
	if( ObjDB->Item.ItemUseOKScript[0] != 0 )
		Role->CallLuaFunc( Role->GUID() , ObjDB->Item.ItemUseOKScript );

	SC_NewTitleSys_UseItemResult( Role->GUID() , EM_NewTitleSysUseItemResult_OK );
}
void NetSrv_OtherChild::RL_PCall( const char* plotStr )
{
	LUA_VMClass::PCallByStrArg( plotStr , 0 ,0 );
}

void NetSrv_OtherChild::RC_OpenSession( BaseItemObject* Obj, SessionTypeENUM Type, int CBID )
{
	static bool RandInit = false;

	if (RandInit == false)
	{
		srand(time(NULL));
		RandInit = true;
	}
	
	int SessionID = (RAND_MAX + 1)*(long)rand() + rand();

	if (SessionID == 0)
	{
		SessionID = 1;
	}

	RoleDataEx* Role = Obj->Role();

	SD_OpenSession(Role->Account_ID(), Role->DBID(), Type, SessionID, 10 * 60);
	SC_OpenSessionResult(Role->GUID(), Type, SessionID, CBID);
}

void NetSrv_OtherChild::RC_CloseSession( BaseItemObject* Obj, SessionTypeENUM Type, int SessionID )
{
	RoleDataEx* Role = Obj->Role();

	SD_CloseSession(Role->Account_ID(), Role->DBID(), Type, SessionID);
}
void NetSrv_OtherChild::RC_AutoReportPossibleCheater( BaseItemObject* Obj , AutoReportBaseInfoStruct& TargetInfo , AutoReportPossibleCheaterENUM Type )
{
	Log_Warning_PossibleCheater( Obj->Role() , TargetInfo , Type );
}

//今天的複本
void NetSrv_OtherChild::RD_WeekInstances( int count , WeekZoneStruct info[] )
{
	WeekInst.clear();
	for( int i = 0 ; i < count ; i++ )
		WeekInst.push_back( info[i] );
}
//要求今天的複本
void NetSrv_OtherChild::RC_WeekInstancesRequest( BaseItemObject* obj  )
{
	RoleDataEx* role = obj->Role();
	int day = (RoleDataEx::G_Day - 3) %7;

	ResetWeekInstCount( role );
	SC_WeekInstances( role->GUID() , day , WeekInst );	
}
//要求傳送到某副本
void NetSrv_OtherChild::RC_WeekInstancesTeleport( BaseItemObject* obj , int zoneID )
{
	RoleDataEx* role = obj->Role();
	ResetWeekInstCount( role );

	int day = (RoleDataEx::G_Day - 3) %7;

	if(		role->TempData.Sys.OnChangeZone 
		||	role->TempData.IsDisabledChangeZone )
		return;

	if( RoleDataEx::G_ZoneID % _DEF_ZONE_BASE_COUNT_ >= 100 )
	{
		role->Msg( "can't teleport in this zone" );
		return;
	}

	if( CheckZoneID( zoneID) == false )
	{
		role->Msg( "zone not open" );
		return;
	}

	GameObjDbStructEx* zoneDB = g_ObjectData->GetObj(zoneID + Def_ObjectClass_Zone);
	if( zoneDB == NULL )
	{
		role->Msg( "zoneDB data error!!" );
		return;
	}

	int instType = zoneDB->Zone.MirrorInstanceType;

	//檢查今天是否有開啟
	WeekZoneStruct* info = NULL;
	for( int i = 0 ; i < WeekInst.size() ; i++ )
	{
		if( WeekInst[i].Day == day && WeekInst[i].ZoneGroupID == instType )
		{
			info = &WeekInst[i];
			break;
		}
	}
	if( info == NULL )
	{
		SC_WeekInstancesTeleportResult( role->GUID() , false );
		return;
	}

	if( instType <= 0 || instType > DEF_MAX_WEEK_INS_TYPE )
	{
		role->Msg( "MirrorInstanceType data err!!" );
		return;
	}

	instType--;
	
	//進入條件不符合
	if(		role->Level() < zoneDB->Zone.MirrorLimitLv
		||	role->PlayerBaseData->WeekInstance[ instType ] == 0 )
	{
		SC_WeekInstancesTeleportResult( role->GUID() , false );
		return;
	}

	SL_WeekInstancesZoneReset( zoneID , role->DBID() , role->PartyID() );
		
	float x = zoneDB->Zone.Mirror_X;
	float y = zoneDB->Zone.Mirror_Y;
	float z = zoneDB->Zone.Mirror_Z;
	
	role->AddValue( (RoleValueName_ENUM)(EM_RoleValue_WeekInstnaces+instType) , -1 );
	LuaPlotClass::SaveReturnPos( role->GUID() , role->ZoneID() , role->X() , role->Y() , role->Z() , role->Dir() );
	ChangeZone( role->GUID() , zoneID , -1 , x , y , z , 0 );

	SC_WeekInstancesTeleportResult( role->GUID() , true );
}
//要求重置某副本
void NetSrv_OtherChild::RC_WeekInstancesZoneReset( BaseItemObject* obj , int zoneID )
{
	RoleDataEx* role = obj->Role();
	SL_WeekInstancesZoneReset( zoneID , role->DBID() , role->PartyID() );
}
//要求重置某副本
void NetSrv_OtherChild::RL_WeekInstancesZoneReset( int netID , int dbid , int partyID )
{
	//把超過設定時間的房間刪除
	for( int i = 1 ; i < Ini()->RoomCount ; i++ )
	{
		PrivateRoomInfoStruct& RoomInfo = Global::St()->PrivateRoomInfoList[i];

		if( RoomInfo.IsActive )
			continue;

		if( RoomInfo.OwnerPartyID != -1 && partyID != RoomInfo.OwnerPartyID  )
			continue;

		if( RoomInfo.OwnerDBID != -1 && dbid != RoomInfo.OwnerDBID  )
			continue;

		RoomInfo.Init();
		DelRoomMonster( i );

		//把此區域的人傳走
		{
			set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
			set< BaseItemObject*>::iterator   PlayerObjIter;
			BaseItemObject* Obj;
			RoleDataEx* other;
			//計算每個房間的人數
			for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
			{
				Obj = *PlayerObjIter;
				if( Obj == NULL )
					continue;
				other = Obj->Role();
				if( other->RoomID() != i )
					continue;

				if( LuaPlotClass::GoReturnPos( other->GUID() ) == false )
				{
					ChangeZone( Obj->GUID() , Ini()->RevZoneID , 0 , (float)Ini()->RevX , (float)Ini()->RevY , (float)Ini()->RevZ , 0 );
				}
			}
		}
		break;
	}
}
bool NetSrv_OtherChild::WeekInstancesResetProc( BaseItemObject* obj  )
{
	RoleDataEx* role = obj->Role();
	int WeekInstReset = g_ObjectData->GetMoneyKeyValue( "WeekInstReset" , 0 );

	WeekInstReset *= ( role->PlayerBaseData->WeekResetCount + 1 );

	bool checkReset = false;
	for( int i = 0 ; i < DEF_MAX_WEEK_INS_TYPE ; i++ )
	{
		if( role->PlayerBaseData->WeekInstance[ i ] != DEF_MAX_WEEK_INS_TYPE_DAY_COUNT )
		{
			checkReset = true;
			break;
		}
	}

	if ( checkReset == false)
		return false;

	for( int i = 0 ; i < DEF_MAX_WEEK_INS_TYPE ; i++ )
	{
		role->SetValue( (RoleValueName_ENUM)(EM_RoleValue_WeekInstnaces+i) , DEF_MAX_WEEK_INS_TYPE_DAY_COUNT );
	}

	role->AddValue( (RoleValueName_ENUM)(EM_RoleValue_WeekResetCount) , 1 );

	return true;
}
//重置副本傳送次數
void NetSrv_OtherChild::RC_WeekInstancesReset( BaseItemObject* obj  )
{
	RoleDataEx* role = obj->Role();
	int WeekInstReset = g_ObjectData->GetMoneyKeyValue( "WeekInstReset" , 0 );

	WeekInstReset *= ( role->PlayerBaseData->WeekResetCount + 1 );

	bool checkReset = false;
	for( int i = 0 ; i < DEF_MAX_WEEK_INS_TYPE ; i++ )
	{
		if( role->PlayerBaseData->WeekInstance[ i ] != DEF_MAX_WEEK_INS_TYPE_DAY_COUNT )
		{
			checkReset = true;
			break;
		}
	}

	if (	checkReset == false 
		||	Global::AccountMoneyOperable() == false)
	{
		SC_WeekInstancesReset( role->GUID() , false );
		return;
	}

	if( role->AddBodyMoney_Account( WeekInstReset * -1 , NULL , EM_ActionType_WeekInstReset , true , true ) == false )
	{
		SC_WeekInstancesReset( role->GUID() , false );
		return;
	}

	for( int i = 0 ; i < DEF_MAX_WEEK_INS_TYPE ; i++ )
	{
		role->SetValue( (RoleValueName_ENUM)(EM_RoleValue_WeekInstnaces+i) , DEF_MAX_WEEK_INS_TYPE_DAY_COUNT );
	}

	role->AddValue( (RoleValueName_ENUM)(EM_RoleValue_WeekResetCount) , 1 );
	
	SC_WeekInstancesReset( role->GUID() , true );	
}

void NetSrv_OtherChild::ResetWeekInstCount( RoleDataEx* role )
{
	if( role->PlayerBaseData->WeekInstDay == RoleDataEx::G_Day )
		return;

	role->PlayerBaseData->WeekInstDay = RoleDataEx::G_Day;
		role->SetValue( (RoleValueName_ENUM)(EM_RoleValue_WeekResetCount) , 0 );
	for( int i = 0 ; i < DEF_MAX_WEEK_INS_TYPE ; i++ )
	{
		role->SetValue( (RoleValueName_ENUM)(EM_RoleValue_WeekInstnaces+i) , DEF_MAX_WEEK_INS_TYPE_DAY_COUNT );
	}

}

void NetSrv_OtherChild::SendWeekInstancesLiveTime( RoleDataEx* role )
{
	if(		role->RoomID() == 0
		||	(unsigned)role->RoomID() > Global::St()->PrivateRoomInfoList.size() )
		return;

	PrivateRoomInfoStruct& RoomInfo = Global::St()->PrivateRoomInfoList[role->RoomID()];

	if( RoomInfo.LiveTime == 0 )
		return;
	int sec = RoomInfo.LiveTime - RoleDataEx::G_Now;
	if( sec < 0 )
		sec = 0;

	SC_WeekInstancesLiveTime( role->GUID() , sec );
}

//魔靈升階(或產生)
void NetSrv_OtherChild::RC_PhantomRankUp( BaseItemObject* obj , int phantomID , bool summon )
{
	RoleDataEx* role = obj->Role();

	GameObjDbStructEx* objDB = Global::GetObjDB( phantomID );
	if( objDB->IsPhantom() == false )
	{
		SC_PhantomRankUpResult( role->GUID() , summon , false );
		return;
	}

	if ( objDB->Phantom.Release == false )
	{
		SC_PhantomRankUpResult( role->GUID() , summon , false );
		return;
	}

	int pos = phantomID % 1000;

	if( pos >= MAX_PHANTOM_COUNT )
	{
		role->Msg( "phantomID err , pos >= MAX_PHANTOM_COUNT" );
		SC_PhantomRankUpResult( role->GUID() , summon , false );
		return;
	}
	
	PhantomBase& info = role->PlayerBaseData->Phantom.Info[pos];

	int nextRank = info.Rank+1;

	map< int,PhantomRankTableStruct >::iterator itRankInfo = RoleDataEx::PhantomRank.find( nextRank );

	if ( itRankInfo == RoleDataEx::PhantomRank.end() )
	{
		role->Msg( "Next rank info not found!" );
		SC_PhantomRankUpResult( role->GUID() , summon , false );
		return;
	}

	PhantomRankTableStruct& rankInfo = itRankInfo->second;

	if( LuaPlotClass::DelBodyItem( role->GUID() , objDB->Phantom.RankUpMaterial , rankInfo.NeedFragment ) == false )
	{
		SC_PhantomRankUpResult( role->GUID() , summon , false );
		return;
	}
	info.Rank++;
	if( info.Lv == 0 )
		info.Lv = 1;

	SC_FixPhantom( role->GUID() , phantomID , info );
	PhantomChangeProc( role );

	SC_PhantomRankUpResult( role->GUID() , summon , true );
}
//魔靈升級
void NetSrv_OtherChild::RC_PhantomLevelUp( BaseItemObject* obj , int phantomID )
{
	RoleDataEx* role = obj->Role();

	GameObjDbStructEx* objDB = Global::GetObjDB( phantomID );
	if( objDB->IsPhantom() == false )
	{
		SC_PhantomLevelUpResult( role->GUID() , false );
		return;
	}

	if ( objDB->Phantom.Release == false )
	{
		SC_PhantomLevelUpResult( role->GUID() , false );
		return;
	}

	int pos = phantomID % 1000;

	if( pos >= MAX_PHANTOM_COUNT )
	{
		role->Msg( "phantomID err , pos >= MAX_PHANTOM_COUNT" );
		SC_PhantomLevelUpResult( role->GUID() , false );
		return;
	}
	
	PhantomBase& info = role->PlayerBaseData->Phantom.Info[pos];

	map< int,PhantomRankTableStruct >::iterator itRankInfo = RoleDataEx::PhantomRank.find( info.Rank );

	if ( itRankInfo == RoleDataEx::PhantomRank.end() )
	{
		role->Msg( "Rank info not found!" );
		SC_PhantomLevelUpResult( role->GUID() , false );
		return;
	}

	PhantomRankTableStruct& rankInfo = itRankInfo->second;

	if( info.Lv >= rankInfo.MaxLevel )
	{
		role->Msg( "info.Lv >= rankInfo.MaxLevel" );
		SC_PhantomLevelUpResult( role->GUID() , false );
		return;
	}

	map< int,map < int,PhantomLvTableStruct > >::iterator itLvUpType = RoleDataEx::PhantomLv.find( objDB->Phantom.LevelUpType+1 );

	if (itLvUpType == RoleDataEx::PhantomLv.end())
	{
		role->Msg( "Level up type not found" );
		SC_PhantomLevelUpResult( role->GUID() , false );
		return;
	}

	map < int,PhantomLvTableStruct >& lvTable = itLvUpType->second;

	int nextLv = info.Lv+1;
	map < int,PhantomLvTableStruct >::iterator itLvTable = lvTable.find( nextLv );

	if (itLvTable == lvTable.end())
	{
		role->Msg( "Level up table not found" );
		SC_PhantomLevelUpResult( role->GUID() , false );
		return;
	}

	PhantomLvTableStruct& rankLv = itLvTable->second;

	{
		map< int , int > materialCount;
		for( int i = 0 ; i < 4 ; i++ )
		{
			if( rankLv.Material[i] == 0 )
				continue;
			materialCount[ rankLv.Material[i] ] ++;
		}
		
		map< int , int >::iterator iter;
		for( iter = materialCount.begin() ; iter != materialCount.end() ; iter++ )
		{
			if( LuaPlotClass::CountBodyItem( role->GUID() , iter->first ) < iter->second )
			{
				role->Msg( "material not enough" );
				SC_PhantomLevelUpResult( role->GUID() , false );
				return;
			}
		}

		for( iter = materialCount.begin() ; iter != materialCount.end() ; iter++ )
		{
			if( LuaPlotClass::DelBodyItem( role->GUID() , iter->first , iter->second ) == false )
			{
				role->Msg( "material not enough" );
				SC_PhantomLevelUpResult( role->GUID() , false );
				return;
			}
		}
	}

	info.Lv++;
	PhantomChangeProc( role );
	SC_FixPhantom( role->GUID() , phantomID , info );
	SC_PhantomLevelUpResult( role->GUID() , true );
}
//魔靈裝備
void NetSrv_OtherChild::RC_PhantomEQ( BaseItemObject* obj , int eq[] )
{
	RoleDataEx* role = obj->Role();

	PhantomStruct& info = role->PlayerBaseData->Phantom;

	for( int i = 0 ; i < 2 ; i++ )
	{
		int pos = eq[i] % 1000;
		if( pos >= MAX_PHANTOM_COUNT )
			continue;
		if( eq[i] != 0 )
		{
			GameObjDbStructEx* objDB = Global::GetObjDB( eq[i] );

			if( objDB->IsPhantom() == false )
				eq[i] = 0;
			else if (objDB->Phantom.Release == false)
				eq[i] = 0;
			if( info.Info[ pos ].Lv == 0 )
				eq[i] = 0;
		}

		info.EQ[i] = eq[i];
	}
	
	//if( info.EQ[0] < info.EQ[1] )
	//	swap( info.EQ[0] , info.EQ[1] );

	PhantomChangeProc( role );
	SC_FixPhantomEQ( role->GUID() , eq );
	SC_PhantomEQResult( role->GUID() , true );
}

void NetSrv_OtherChild::PhantomChangeProc( RoleDataEx* role )
{
	//清buff
	{
		MagicClearTimeStruct ClearEvent;
		ClearEvent._Value = 0;
		ClearEvent.ChangeEQPhantom = true;
		role->ClearBuffByEvent( ClearEvent );
	}

	PhantomStruct& info = role->PlayerBaseData->Phantom;

	//key = 大id + 小id * 0x100000000
	INT64 id = 0;
	if ( info.EQ[0] > info.EQ[1] )
	{
		id = info.EQ[0] + info.EQ[1] * 0x100000000;
	}
	else
	{
		id = info.EQ[1] + info.EQ[0] * 0x100000000;
	}

	info.SkillID = RoleDataEx::PhantomSkill[ id ];
	if( info.SkillID )
		info.SkillLv = ( info.Info[ info.EQ[0]%1000 ].Lv + info.Info[ info.EQ[1]%1000 ].Lv ) /2;
	else
		info.SkillLv = 0;

    GameObjDbStructEx* orgDB =  Global::GetObjDB( info.SkillID );
	if( orgDB->IsMagicBase() )
	{
		role->AssistMagic( role , info.SkillID , info.SkillLv , false , 60 );
		NetSrv_MagicChild::SendRange_AddBuffInfo( role->GUID() , role->GUID() ,  info.SkillID , info.SkillLv , 100000000 );	
	}

	role->TempData.UpdateInfo.ReSetTalbe = true;		
}
//後台Buff設定
void NetSrv_OtherChild::RD_BuffSchedule( int buffID , int buffLv , int buffTime )
{

	BaseItemObject* PlayerObj;
	for( PlayerObj = BaseItemObject::GetByOrder_Player(true) ; PlayerObj != NULL ; PlayerObj = BaseItemObject::GetByOrder_Player() )
	{
		RoleDataEx* Role = PlayerObj->Role();
		if( Role->CheckBuff( buffID ) )
			continue;
		BuffBaseStruct* Buff = Role->AssistMagic( Role , buffID , buffLv , false , buffTime  );
		if( Buff != NULL )
			NetSrv_MagicChild::SendRange_AddBuffInfo( Role->GUID() , Role->GUID() , buffID , Buff->Power , Buff->Time );

	}

}
//後台魔幻寶盒資料
void NetSrv_OtherChild::RD_ItemCombinInfo( int groupID , ItemCombineStruct& info )
{
	ItemCombineStruct infobk = info;
	info.ProductCount = 0;
	info.ProductID = 0;

	int hashID = GetHashCode( &info , sizeof( info ) );
	RoleDataEx::ItemCombineListEx[groupID][hashID].push_back( infobk );
	/*
	ItemCombineStruct info = itemCombine;
	map< int , int > fixInfoMap;
	int x;
	for( x = 0 ; x < 5 ; x++ )
	{
		if( info.SourceCount[x] == 0 || info.SourceID[x] == 0 )
			continue;
		fixInfoMap[ info.SourceID[x] ] += info.SourceCount[x];
	}
	info.Init();

	map< int , int >::iterator iter;
	for( x = 0 , iter = fixInfoMap.begin() ; iter != fixInfoMap.end() ; iter++ , x++ )
	{
		info.SourceCount[x] = iter->second;
		info.SourceID[x] = iter->first;
	}
	int hashID = GetHashCode( &info , sizeof( info ) );
	info.ProductCount = itemCombine.ProductCount;
	info.ProductID = itemCombine.ProductID;
	RoleDataEx::ItemCombineListEx[groupID][hashID].push_back( info );
	*/
}
