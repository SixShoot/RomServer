//----------------------------------------------------------------------------------------
#include "LuaPlot.h" 
#include "lua/LuaWrapper/LuaWrap.h"
//----------------------------------------------------------------------------------------
#include "../../netwalker_member/Net_Script/NetSrv_Script.h"
#include "../../netwalker_member/NetWakerGSrvInc.h"
#include "../FlagPosList/FlagPosList.h"
#include "../AIProc/NpcAIBase.h"
#include "../magicproc/MagicProcess.h"
#include "../partyinfolist/PartyInfoList.h"
#include <vector>
#include <string>
#include "../../mainproc/pathmanage/NPCMoveFlagManage.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
#include "MD5/Mymd5.h"
#include "../../mainproc/HousesManage/HousesManageClass.h"
#include "../../mainproc/GuildHouseManage/GuildHousesClass.h"
#include "../../mainproc/GuildHouseWarManage/GuildHouseWarManage.h"
#include "../../netwalker_member/Net_BG_GuildWar/BG_GuildWarManage/BG_GuildWarManage.h"
//----------------------------------------------------------------------------------------
namespace  LuaPlotClass
{
	//----------------------------------------------------------------------------------------
	//共用資料
	vector< BaseSortStruct >	SortList;
	//----------------------------------------------------------------------------------------

	bool	LuaPlotClass::Init_Hsiang()
	{
		/*
		LuaRegisterFunc( "ItemSavePos"					, bool( int )												, ItemSavePos			);
		LuaRegisterFunc( "ItemTransportPos"				, bool(  )													, ItemTransportPos		);
		LuaRegisterFunc( "CheckItemTransportPos"		, bool(  )													, CheckItemTransportPos );

		LuaRegisterFunc( "CheckBuff"					, bool( int , int )											, CheckBuff				);

		LuaRegisterFunc( "SetDir"						, bool( int , float )										, SetDir				);
		LuaRegisterFunc( "CalDir"						, float( float , float)										, CalDir				);
		LuaRegisterFunc( "ResetColdown"					, bool( int )												, ResetColdown			);
		LuaRegisterFunc( "ChangeZone"					, bool( int , int , int , float , float , float , float )	, ChangeZone			);


		LuaRegisterFunc( "DialogCreate"					, bool( int,int,const char*)								, DialogCreate			);
		LuaRegisterFunc( "DialogSelectStr"				, bool( int,const char*)									, DialogSelectStr			);
		LuaRegisterFunc( "DialogSendOpen"				, bool( int )												, DialogSendOpen			);
		LuaRegisterFunc( "DialogSetContent"				, bool( int,const char*)									, DialogSetContent			);
		LuaRegisterFunc( "DialogGetResult"				, int( int )												, DialogGetResult			);
		LuaRegisterFunc( "DialogClose"					, bool( int )												, DialogClose			);
		*/
		return true;
	}
	bool	LuaPlotClass::Release_Hsiang( )
	{
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	//取得使用物品的資料
	//////////////////////////////////////////////////////////////////////////
	//刪除使用的物品
	bool	UseItemDestroy( )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( OwnerID() );

		if( Owner == NULL ) 
			return false;

		int UseItemID = Owner->TempData.Magic.UseItem;
		int	UseItemPos = Owner->TempData.Magic.UseItemPos;
		int	UseType	= Owner->TempData.Magic.UseItemType;

		ItemFieldStruct* Item = Owner->GetItem( UseItemPos , UseType );
		if( Item == NULL || Item->IsEmpty() )
			return false;

		if( Item->OrgObjID != UseItemID )
			return false;

		if( Item->Count <= 1 )
		{
			Global::Log_ItemDestroy( Owner , EM_ActionType_DestroyItem , *Item , -1 ,  -1 , "" );
			Item->Init();
		}
		else
		{
			Global::Log_ItemDestroy( Owner , EM_ActionType_DestroyItem , *Item , 1  ,  -1 , "" );
			Item->Count --;
		}
		Owner->Net_FixItemInfo( UseItemPos , UseType );

		if( Def_Item_Pos_EQ == UseType )
			Owner->TempData.UpdateInfo.Eq = true;

		return true;
	}
	//取得使用物品得耐久度
	int		UseItemDurable(  )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( OwnerID() );

		if( Owner == NULL ) 
			return -1;

		int UseItemID = Owner->TempData.Magic.UseItem;
		int	UseItemPos = Owner->TempData.Magic.UseItemPos;
		int	UseType	= Owner->TempData.Magic.UseItemType;

		ItemFieldStruct* Item = Owner->GetItem( UseItemPos , UseType );
		if( Item == NULL || Item->IsEmpty() )
			return -1;

		return Item->Durable;
	}

	//檢查使用的物品是否還在
	bool	CheckUseItem( )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( OwnerID() );

		if( Owner == NULL ) 
			return false;

		int UseItemID = Owner->TempData.Magic.UseItem;
		int	UseItemPos = Owner->TempData.Magic.UseItemPos;
		int	UseType	= Owner->TempData.Magic.UseItemType;

		ItemFieldStruct* Item = Owner->GetItem( UseItemPos , UseType );
		if( Item == NULL || Item->IsEmpty() )
			return false;

		if( Item->OrgObjID != UseItemID )
			return false;

		return true;
	}

	//--------------------------------------------------------------------------------
	//位置記錄物件處理 
	//--------------------------------------------------------------------------------
	//把位置記錄到物品上面
	bool	ItemSavePos( int NewIetmID  )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( OwnerID() );

		if( Owner == NULL ) 
			return false;

		int UseItemID = Owner->TempData.Magic.UseItem;
		int	UseItemPos = Owner->TempData.Magic.UseItemPos;
		int	UseType	= Owner->TempData.Magic.UseItemType;

		//檢查是否可以記錄
		char* ZoneName = PathManageClass::MapName( Owner->X() , Owner->Y(), Owner->Z() );
		if( ZoneName == NULL )
			return false;

		MapInfoStruct* MapInfo;
		//取區域物件
		GameObjDbStructEx* ZoneObj = Global::GetObjDB( Def_ObjectClass_Zone + RoleDataEx::G_ZoneID % _DEF_ZONE_BASE_COUNT_ );
		if( ZoneObj == NULL )
			return false;

		int MainMapID = PathManageClass::MainMapID( Owner->X() , Owner->Y(), Owner->Z() );

		if( MainMapID != ZoneObj->Zone.MapID )
			return false;

		bool IsFind = false;
		for( int i = 0 ; i < ZoneObj->Zone.MapCount ; i++ )
		{
			MapInfo = &ZoneObj->Zone.MapInfoList[i];
			if( stricmp( ZoneName , MapInfo->MapName ) == 0 )
			{
				IsFind = true;
				if( MapInfo->IsEnabledSave == false )
					return false;
				break;
			}
		}

		if( IsFind == false )
			return false;
		//取出物品
		ItemFieldStruct* Item = Owner->GetItem( UseItemPos , UseType );
		if( Item == NULL )
			return false;

		if( Item->OrgObjID != UseItemID )
			return false;

		GameObjDbStructEx* ItemDB = Global::GetObjDB( UseItemID );
		if( ItemDB->IsItem_Pure() == false )
			return false;

		GameObjDbStructEx* NewItemDB = Global::GetObjDB( NewIetmID );
		if( NewItemDB->IsItem_Pure() == false )
			return false;

		if( Item->Count != 1 )
			return false;

		Item->OrgObjID  = NewIetmID;
		Item->ImageObjectID = NewIetmID;
		Item->X			= int( Owner->Pos()->X );
		Item->Y			= int( Owner->Pos()->Y );
		Item->Z			= int( Owner->Pos()->Z );
		Item->ZoneID	= RoleDataEx::G_ZoneID %_DEF_ZONE_BASE_COUNT_;
		Item->MapID		= MapInfo->MapID;
		return true;
	}

	//--------------------------------------------------------------------------------
	//把角色傳送到某位置
	bool	ItemTransportPos( )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( OwnerID() );

		if( Owner == NULL ) 
			return false;

		if( Owner->TempData.Attr.Effect.TeleportDisable != false )
			return false;

		int UseItemID = Owner->TempData.Magic.UseItem;
		int	UseItemPos = Owner->TempData.Magic.UseItemPos;
		int	UseType	= Owner->TempData.Magic.UseItemType;

		//取出物品
		ItemFieldStruct* Item = Owner->GetItem( UseItemPos , UseType );
		if( Item == NULL || Item->IsEmpty() )
			return false;

		//int ZoneID = Item->ZoneID % _DEF_ZONE_BASE_COUNT_ + _DEF_ZONE_BASE_COUNT_ * Owner->PlayerTempData->ParallelZoneID;
		int ZoneID = GlobalBase::GetParallelZoneID( Item->ZoneID ,  Owner->PlayerTempData->ParallelZoneID );
		if( ZoneID == -1 )
		{
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_ChanageZoneError_Failed );
			return false;
		}

		Global::ChangeZone( Owner->GUID() , ZoneID , 0 , float( Item->X ) , float( Item->Y ) , float( Item->Z ) , Owner->Pos()->Dir );

		return true;
	}

	//撿查是否可以用物品傳送
	bool	CheckItemTransportPos( )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( OwnerID() );

		if( Owner == NULL ) 
			return false;

		if( Owner->TempData.Attr.Effect.TeleportDisable != false )
			return false;

		int UseItemID = Owner->TempData.Magic.UseItem;
		int	UseItemPos = Owner->TempData.Magic.UseItemPos;
		int	UseType	= Owner->TempData.Magic.UseItemType;

		//取出物品
		ItemFieldStruct* Item = Owner->GetItem( UseItemPos , UseType );
		if( Item == NULL )
			return false;

		//檢查 Server 是否已經開啟
		if( Global::Net_ServerList->CheckServer( EM_SERVER_TYPE_LOCAL , Item->ZoneID ) == false )
			return false;

		return true;
	}

	//檢查是否有某Buff
	bool	CheckBuff( int ItemGUID , int BuffID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( ItemGUID );
		if( Owner == NULL )
			return false;
		for( int i = 0 ; i < Owner->BaseData.Buff.Size() ; i++ )
		{
			if( Owner->BaseData.Buff[i].BuffID == BuffID )
				return true;
		}
		return false;
	}

	//設訂物件面向
	bool	SetDir( int GItemID , float Dir )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;
		Owner->BaseData.Pos.Dir = Dir;
		NetSrv_MoveChild::SendRangeSetDir( GItemID , Dir );
		return true;
	}

	//計算面向
	float	CalDir( float Dx , float Dz )
	{
		return RoleDataEx::CalDir( Dx , Dz );
	}

	bool	ResetColdown( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		for( int i = 5; i < DEF_MAX_COLDOWN_COUNT_JOB_ ; i++ )
		{
			if( g_ObjectData->GetNotColdownJob( i ) )
				continue;
			Owner->BaseData.Coldown.Job[i] = 0;
		}

		NetSrv_Magic::S_ResetColdown( Owner->GUID() , EM_ResetColdownType_Job_Normal );
		return true;
	}

	//換區
	bool	ChangeZone( int GItemID , int ZoneID , int RoomID , float X , float Y , float Z , float Dir )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		if(		Owner->TempData.Sys.OnChangeZone != false 
			||	Owner->TempData.IsDisabledChangeZone != false )
			return false;

		//ZoneID = ZoneID % _DEF_ZONE_BASE_COUNT_ + _DEF_ZONE_BASE_COUNT_ * Owner->PlayerTempData->ParallelZoneID;
		ZoneID = GlobalBase::GetParallelZoneID( ZoneID ,  Owner->PlayerTempData->ParallelZoneID );
		if( ZoneID == -1 )
		{
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_ChanageZoneError_Failed );
			return false;
		}


		Global::ChangeZone( GItemID , ZoneID , RoomID , X , Y , Z , Dir );
		return true;
	}

	//--------------------------------------------------------------------------------
	//	劇情 Dialog
	//--------------------------------------------------------------------------------
	//產生一個對話框
	bool	DialogCreate( int GItemID , int DialogType , const char* Content )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return false;

		Owner->PlayerTempData->DialogSelectID = -1;

		NetSrv_Other::S_OpenDialog( GItemID , (LuaDialogType_ENUM)DialogType , Content );
		return true;
	}
	//送對話選項
	bool	DialogSelectStr( int GItemID , const char* SelectStr )
	{		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
	if( Owner == NULL || Owner->IsPlayer() == false )
		return false;

	NetSrv_Other::S_DialogSelectStr( GItemID , SelectStr );
	return true;
	}
	//開啟對話框
	bool	DialogSendOpen( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return false;

		if( Owner->PlayerTempData->DialogSelectID == -2 )
			return false;

		NetSrv_Other::S_DialogSelectStrEnd( GItemID );
		Owner->PlayerTempData->DialogSelectID = -2;
		return true;
	}
	//重設定內容
	bool	DialogSetContent( int GItemID , const char* Content )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return false;

		NetSrv_Other::S_DialogSetContent( GItemID , Content );
		return true;
	}

	//重設定內容
	bool	DialogSetTitle( int GItemID , const char* Title )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return false;

		NetSrv_Other::S_DialogSetTitle( GItemID , Title );
		return true;
	}

	//取得回應資料
	int		DialogGetResult( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return -1;
		return Owner->PlayerTempData->DialogSelectID;
	}

	//取得回應資料
	const char*		DialogGetInputResult( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return NULL;

		const char* sInput = NULL;
		if( Owner->PlayerTempData->DialogSelectID != -1 && Owner->PlayerTempData->DialogSelectID != -2 )
		{
			sInput = NetSrv_Other::GetDialogInputStr( GItemID );			
		}
		return sInput;
	}

	//取得回應資料
	int	DialogGetInputCheckResult( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return NULL;

		int iCheckResult = 0;
		if( Owner->PlayerTempData->DialogSelectID != -1 && Owner->PlayerTempData->DialogSelectID != -2 )
		{
			iCheckResult = NetSrv_Other::GetDialogInputCheckResult( GItemID );			
		}
		return iCheckResult;
	}


	//要求關閉對話框
	bool		DialogClose( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return false;
		NetSrv_Other::S_CloseDialog( GItemID );
		return true;
	}

	//--------------------------------------------------------------------------------
	//設定長像
	bool	SetLook( int GItemID , int FaceID , int HairID , int HairColor , int BodyColor )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return false;

		GameObjDbStructEx* ObjDB = Global::GetObjDB( FaceID );
		if( ObjDB->IsBody() )
		{
			Owner->BaseData.Look.FaceID = FaceID;
		}

		ObjDB = Global::GetObjDB( HairID );
		if( ObjDB->IsBody() )
		{
			Owner->BaseData.Look.HairID = HairID;
		}

		if( HairColor != 0 )
		{
			HairColor = HairColor | 0xff000000;
			Owner->BaseData.Look.HairColor = HairColor;
		}

		if( BodyColor != 0 )
		{
			BodyColor = BodyColor | 0xff000000;
			Owner->BaseData.Look.BodyColor = BodyColor;
		}

		NetSrv_MoveChild::SendRangeLookFace( Owner );

		return true;		

	}

	//--------------------------------------------------------------------------------
	//設定卡片開關
	bool	SetCardFlag( int GItemID , int CardID , bool Flag )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return false;

		if( Flag != false )
			return Owner->AddCard( CardID );
		else
			return Owner->DelCard( CardID );

	}
	//讀取是否有卡片
	bool	GetCardFlag( int GItemID , int CardID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return false;

		int ID = CardID -  Def_ObjectClass_Card;

		return Owner->PlayerBaseData->Card.GetFlag( ID );
	}
	//--------------------------------------------------------------------------------
	//公會
	//--------------------------------------------------------------------------------
	enum GuildResultENUM
	{
		EM_GuildResult_OK			= 0	,	//成?
		EM_GuildResult_HasGuildErr	= 1 ,	//已有公會
		EM_GuildResult_NoGuildErr	= 2 ,	//沒有公會
		EM_GuildResult_DataErr		= 3	,	//資料有問題
		EM_GuildResult_InterfaceErr	= 4	,	//有介面還在開啟中
		EM_GuildResult_LeaderErr	= 5	,	//不是會長
	};
	//要求開啟建立新公會
	int	OpenCreateNewGuild( )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		GuildNPC  = Global::GetRoleDataByGUID( T->TargetID );
		if( Player == NULL || GuildNPC == NULL  )
		{
			Print( LV2 , "OpenCreateNewGuild" , "Player == NULL || GuildNPC == NULL Func = %d" , T->FuncName );
			return EM_GuildResult_DataErr;
		}

		if( Player->CheckOpenSomething() != false )
			return EM_GuildResult_InterfaceErr;

		if( Player->BaseData.GuildID != 0 )
			return EM_GuildResult_HasGuildErr;

		Player->TempData.ShopInfo.TargetID = GuildNPC->GUID();
		Player->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_CreateNewGuild;

		NetSrv_Guild::SC_OpenCreateGuild( T->OwnerID , T->TargetID );

		return EM_GuildResult_OK;
	}
	//要求開啟貢獻介面
	int		OpenGuildContribution( )
	{

		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		GuildNPC  = Global::GetRoleDataByGUID( T->TargetID );
		if( Player == NULL || GuildNPC == NULL  )
		{
			Print( LV2 , "OpenGuildContribution" , "Player == NULL || GuildNPC == NULL Func = %d" , T->FuncName );
			return EM_GuildResult_DataErr;
		}

		if( Player->CheckOpenSomething() != false )
			return EM_GuildResult_InterfaceErr;

		if( Player->BaseData.GuildID == 0 )
			return EM_GuildResult_NoGuildErr;


		Player->TempData.ShopInfo.TargetID = GuildNPC->GUID();
		Player->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_GuildContribution;

		NetSrv_Guild::SC_OpenGuildContribution( T->OwnerID , T->TargetID );

		return EM_GuildResult_OK;
	}
	//要求開啟貢獻介面
	int		OpenGuildShop( )
	{
		/*
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		GuildNPC  = Global::GetRoleDataByGUID( T->TargetID );
		if( Player == NULL || GuildNPC == NULL  )
		{
		Print( LV2 , "OpenGuildShop" , "Player == NULL || GuildNPC == NULL Func = %d" , T->FuncName );
		return EM_GuildResult_DataErr;
		}

		if( Player->CheckOpenSomething() != false )
		return EM_GuildResult_InterfaceErr;

		if( Player->BaseData.GuildID == 0 )
		return EM_GuildResult_NoGuildErr;

		GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember( Player->DBID() );
		if( GuildMember == false )
		return EM_GuildResult_DataErr;


		Player->TempData.ShopInfo.TargetID = GuildNPC->GUID();
		Player->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_GuildShop;

		NetSrv_Guild::SC_GuildShopOpen( T->OwnerID , T->TargetID , GuildMember->Guild->Base.Flag , GuildMember->Guild->Base.Score );
		*/
		return EM_GuildResult_OK;
	}
	//把連屬公會變成正式公會
	int		SetGuildReady( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return EM_GuildResult_DataErr;

		GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember( Owner->DBID() );
		if( GuildMember == NULL )
		{
			NetSrv_Guild::SC_SetGuildReady( GItemID , false );
			return EM_GuildResult_DataErr;
		}

		if( GuildMember->Guild->Base.LeaderDBID != Owner->DBID() )
		{
			NetSrv_Guild::SC_SetGuildReady( GItemID , false );
			return EM_GuildResult_LeaderErr;
		}

		NetSrv_Guild::SC_SetGuildReady( GItemID , true );
		NetSrv_Guild::SD_SetGuildReady	( GuildMember->GuildID );

		return EM_GuildResult_OK;
	}
	/*
	//購買公會的 0 階級數 1 成員
	int		AddGuildValue( int GItemID , int Type , int Value , int Score )
	{

	RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
	if( Owner == NULL )
	return EM_GuildResult_DataErr;

	if( Owner->BaseData.GuildID == 0 )			
	return EM_GuildResult_NoGuildErr;

	//		NetSrv_Guild::SD_BuyFunction( Owner->BaseData.GuildID , Owner->DBID() , Score , Type , Value );
	return EM_GuildResult_OK;
	}
	*/

	//取得公會階級	-1 代表沒公會
	int		GuildRank( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return -1;
		GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember( Owner->DBID() );
		if( GuildMember == NULL )
			return -1;

		return GuildMember->Rank;
	}

	// 0 沒有公會 1 聯屬公會 2 一般公會
	int		GuildState( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return -1;

		GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember( Owner->DBID() );
		if( GuildMember == NULL )
			return 0;

		if( GuildMember->Guild->Base.IsReady == false )
			return 1;

		return 2;
	}

	//取得公會人數
	int		GuildMemberCount( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return 0;
		GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember( Owner->DBID() );
		if( GuildMember == NULL )
			return 0;

		return int( GuildMember->Guild->Member.size() );
	}

	//取得某人公會資料
	int		GuildInfo( int GItemID , int Type )
	{
		enum GuildInfoTypeENUM
		{
			EM_GuildInfoType_Rank				,
			EM_GuildInfoType_GuildMemberCount	,
			EM_GuildInfoType_GuildLv			,
			EM_GuildInfoType_GuildHouse			,
			EM_GuildInfoType_GuildState			,

			EM_GuildInfoType_Resource_Gold		,
			EM_GuildInfoType_Resource_BonusGold	,
			EM_GuildInfoType_Resource_Mine		,
			EM_GuildInfoType_Resource_Wood		,
			EM_GuildInfoType_Resource_Herb		,
			EM_GuildInfoType_Resource_GuildRune	,
			EM_GuildInfoType_Resource_GuildStone,
		};

		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return 0;

		GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember( Owner->DBID() );
		if( GuildMember == NULL )
			return 0;

		switch( Type )
		{
		case EM_GuildInfoType_Rank:
			{
				return GuildMember->Rank;
			}
			break;
		case EM_GuildInfoType_GuildMemberCount:
			{
				return int( GuildMember->Guild->Member.size() );
			}
			break;
		case EM_GuildInfoType_GuildLv:
			{
				return GuildMember->Guild->Base.Level;
			}
			break;
		case EM_GuildInfoType_GuildHouse:
			{
				if( GuildMember->Guild->Base.IsOwnHouse == false )
					return 0;
				return 1;
			}
			break;
		case EM_GuildInfoType_GuildState:
			{
				if( GuildMember->Guild->Base.IsReady == false )
					return 1;

				return 2;
			}
			break;
		case EM_GuildInfoType_Resource_Gold:
		case EM_GuildInfoType_Resource_BonusGold:
		case EM_GuildInfoType_Resource_Mine:
		case EM_GuildInfoType_Resource_Wood:
		case EM_GuildInfoType_Resource_Herb:
		case EM_GuildInfoType_Resource_GuildRune:
		case EM_GuildInfoType_Resource_GuildStone:
			return GuildMember->Guild->Base.Resource._Value[ Type - EM_GuildInfoType_Resource_Gold];
			break;
		}
		return 0;
	}
	//儲存重生點	
	bool		SaveHomePoint( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		Owner->SelfData.RevPos = Owner->BaseData.Pos;
		Owner->SelfData.RevZoneID = Owner->BaseData.ZoneID;

		return true;
	}
	//傳送回重生點	
	bool		GoHomePoint( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		if( Owner->TempData.Attr.Effect.TeleportDisable != false )
			return false;

		//int ZoneID = Owner->SelfData.RevZoneID % _DEF_ZONE_BASE_COUNT_ + _DEF_ZONE_BASE_COUNT_ * Owner->PlayerTempData->ParallelZoneID;
		int ZoneID = GlobalBase::GetParallelZoneID( Owner->SelfData.RevZoneID ,  Owner->PlayerTempData->ParallelZoneID );
		if( ZoneID == -1 )
		{
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_ChanageZoneError_Failed );
			return false;
		}

		Global::ChangeZone( GItemID , ZoneID , 0 , Owner->SelfData.RevPos.X , Owner->SelfData.RevPos.Y , Owner->SelfData.RevPos.Z , Owner->SelfData.RevPos.Dir  );
		return true;
	}
	//--------------------------------------------------------------------------------
	//開啟樂透彩兌獎介面
	bool	OpenExchangeLottery( )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		NPC  = Global::GetRoleDataByGUID( T->TargetID );
		if( Player == NULL || NPC == NULL  )
		{
			Print( LV2 , "OpenExchangeLottery" , "Player == NULL || NPC == NULL Func = %d" , T->FuncName );
			return false;
		}

		if( Player->CheckOpenSomething() != false )
		{
			return false;
		}


		Player->TempData.ShopInfo.TargetID = NPC->GUID();
		Player->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_ExchangeLottery;

		NetSrv_Lottery::SC_OpenExchangePrize( T->OwnerID , T->TargetID );

		return true;
	}
	//開啟購買樂透彩介面
	bool	OpenBuyLottery( )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		NPC  = Global::GetRoleDataByGUID( T->TargetID );
		if( Player == NULL || NPC == NULL  )
		{
			Print( LV2 , "OpenBuyLottery" , "Player == NULL || NPC == NULL Func = %d" , T->FuncName );
			return false;
		}

		if( Player->CheckOpenSomething() != false )
			return false;


		Player->TempData.ShopInfo.TargetID = NPC->GUID();
		Player->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_BuyLottery;

		NetSrv_Lottery::SC_OpenBuyLottery( T->OwnerID , T->TargetID );

		return true;
	}
	 
	bool	OpenRare3EqExchangeItem()
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		NPC  = Global::GetRoleDataByGUID( T->TargetID );
		if( Player == NULL || NPC == NULL  )
		{
			Print( LV2 , "OpenRare3EqExchangeItem" , "Player == NULL || NPC == NULL Func = %d" , T->FuncName );
			return false;
		}

		if( Player->CheckOpenSomething() != false )
			return false;


		Player->TempData.ShopInfo.TargetID = NPC->GUID();
		Player->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_Rare3EqExchangeItem;

		NetSrv_ItemChild::SC_OpenRare3EqExchangeItem( T->OwnerID , T->TargetID );

		return true;
	}


	//開啟Client端介面
	bool	OpenClientMenu( int GItemID , int Type )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;
		//		if( Type >= EM_ClientMenuType_Max || Type < 0 )
		//			return false;

		NetSrv_Other::S_OpenClientMenu( GItemID , (ClientMenuType_ENUM)Type );
		return true;
	}

	//買房子
	bool	BuyHouse( int GItemID , int Type )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		if( Owner->PlayerBaseData->HouseDBID != -1 )
			return false;

		NetSrv_Houses::SD_BuildHouseRequest( Owner->DBID() , Type , (char*)Owner->Account_ID() );
		return true;
	}

	//增加房屋能量
	bool	AddHouseEnergy( int GItemID , int EnertyPoint )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;

		if( Role->TempData.Sys.OnChangeZone != false )
			return false;

		if( Role->PlayerBaseData->HouseDBID != Role->PlayerTempData->VisitHouseDBID )
		{
			Role->Msg("不是自己的房子");
			return false;
		}

		HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Role->PlayerBaseData->HouseDBID );
		if( houseClass == NULL )
		{
			Role->Msg("找不到房屋資料");
			return false;
		}

		HousesInfoStruct& HouseBase =houseClass->HouseBase();
		HouseBase.IsNeedSave = true;
		HouseBase.Info.EnergyPoint += EnertyPoint; 
		houseClass->SetSaveInfo();
	
		NetSrv_HousesChild::SendAllCli_FixHouseBase( Role->RoomID() , HouseBase.Info );
		return true;
	}

	//跑馬燈
	bool	RunningMsg( int GItemID , int Type , const char* Msg )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		//0 自己 1 區域 2 全部的玩家
		switch( Type )
		{
		case 0:
			NetSrv_Talk::SendRunningMsg( GItemID , Msg );
			break;
		case 1:
			NetSrv_Talk::SendRunningMsg_Zone( Msg );
			break;
		case 2:
			NetSrv_Talk::SendRunningMsg_All( Msg );
			break;
		case 3:
			NetSrv_Talk::SendRunningMsg_Room( Owner->RoomID() , Msg );
			break;
		}

		return true;
	}

	//跑馬燈
	bool	RunningMsgEx( int GItemID , int Type , int EventType , const char* Msg )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		//0 自己 1 區域 2 全部的玩家
		switch( Type )
		{
		case 0:
			NetSrv_Talk::SendRunningMsgEx( GItemID , (RunningMsgExTypeENUM)EventType , Msg );
			break;
		case 1:
			NetSrv_Talk::SendRunningMsgEx_Zone( (RunningMsgExTypeENUM)EventType , Msg );
			break;
		case 2:
			NetSrv_Talk::SendRunningMsgEx_All( (RunningMsgExTypeENUM)EventType , Msg );
			break;
		case 3:
			NetSrv_Talk::SendRunningMsgEx_Room( Owner->RoomID() , (RunningMsgExTypeENUM)EventType , Msg );
			break;
		}

		return true;
	}
	//儲存回傳點
	bool	SaveReturnPos( int GItemID , int ZoneID , float X , float Y , float Z , float Dir )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		if( Owner->RoomID() != 0 )
			return false;

		ZoneID = ZoneID % _DEF_ZONE_BASE_COUNT_;

		if( ZoneID >= 100 )
		{
			Owner->SelfData.ReturnZoneID = Global::Ini()->RevZoneID;
			Owner->SelfData.ReturnPos.X = float( Global::Ini()->RevX );
			Owner->SelfData.ReturnPos.Y = float( Global::Ini()->RevY );
			Owner->SelfData.ReturnPos.Z = float( Global::Ini()->RevZ );	
		}
		else
		{
			Owner->SelfData.ReturnZoneID = ZoneID;
			Owner->SelfData.ReturnPos.X = X;
			Owner->SelfData.ReturnPos.Y = Y;
			Owner->SelfData.ReturnPos.Z = Z;
		}
		Owner->SelfData.ReturnPos.Dir = Dir;
		return true;
	}

	bool	SaveReturnPos_ZoneDef( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		if( Global::Ini()->EnabledRevPoint == false )
			return false;

		Owner->SelfData.ReturnZoneID = Global::Ini()->RevZoneID;
		Owner->SelfData.ReturnPos.X = float( Global::Ini()->RevX );
		Owner->SelfData.ReturnPos.Y = float( Global::Ini()->RevY );
		Owner->SelfData.ReturnPos.Z = float( Global::Ini()->RevZ );
		Owner->SelfData.ReturnPos.Dir = float( rand()%360 );


		return true;
	}
	//傳回回傳點
	bool	GoReturnPos( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;


		int ZoneID = GlobalBase::GetParallelZoneID( Owner->SelfData.ReturnZoneID ,  Owner->PlayerTempData->ParallelZoneID );
		if( ZoneID == -1 )
		{
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_ChanageZoneError_Failed );
			return false;
		}
//		if( Global::CheckZoneID( Owner->SelfData.ReturnZoneID ) == false )
//			return false;

		Global::ChangeZone( Owner->GUID() , ZoneID , 0 , Owner->SelfData.ReturnPos.X , Owner->SelfData.ReturnPos.Y , Owner->SelfData.ReturnPos.Z , Owner->SelfData.ReturnPos.Dir );
		Owner->SelfData.ReturnZoneID = 0;
		Owner->SelfData.ReturnPos.X = 0;
		Owner->SelfData.ReturnPos.Y = 0;
		Owner->SelfData.ReturnPos.Z = 0;
		Owner->SelfData.ReturnPos.Dir = 0;
		return true;
	}

	bool	GoReturnPosByNPC( int GItemID , int PlayerID  )
	{
		RoleDataEx* NPC		= Global::GetRoleDataByGUID( GItemID );
		RoleDataEx* Player	= Global::GetRoleDataByGUID( PlayerID );
		if( NPC == NULL || Player == NULL || Player->IsPlayer() == false || NPC->IsNPC() == false )
			return false;

		int ZoneID = GlobalBase::GetParallelZoneID( NPC->SelfData.ReturnZoneID ,  NPC->PlayerTempData->ParallelZoneID );
		if( ZoneID == -1 )
		{
			Player->Net_GameMsgEvent( EM_GameMessageEvent_ChanageZoneError_Failed );
			return false;
		}
		 

		if( Player->TempData.Attr.Effect.TeleportDisable != false )
			return false;

		Global::ChangeZone( Player->GUID() , ZoneID , 0 , NPC->SelfData.ReturnPos.X , NPC->SelfData.ReturnPos.Y , NPC->SelfData.ReturnPos.Z , NPC->SelfData.ReturnPos.Dir );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	//要求開啟玩家小屋參觀介面
	bool	OpenVisitHouse( bool IsVisitMyHouse )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		NPC  = Global::GetRoleDataByGUID( T->TargetID );
		if( Player == NULL || NPC == NULL )
		{
			Print( LV2 , "OpenVisitHouse" , "NPC == NULL || Bank->IsPlayer() Func = %d" , T->FuncName );
			return false;
		}
		
		/*StructBattleGroundQueue	Obj;
		if (((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->SearchRoleInWaitQueue( Player->DBID(), &Obj, false ))
		{
			Player->Msg("Wait Queue");
			return false;
		}

		StructBattleGroundRoleInfo BGRole;
		int iRoomID = ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->SearchRoleInBattleGround( Player->DBID(), &BGRole );
		if( iRoomID != 0 )
		{
			Player->Msg("Battle Ground");
			return false;
		}*/

		int ZoneID = GlobalBase::GetParallelZoneID( g_ObjectData->GetSysKeyValue( "HouseZoneID" ) ,  0 );
		if( ZoneID == -1 )
		{
			Player->Net_GameMsgEvent( EM_GameMessageEvent_ChanageZoneError_Failed );
			return false;
		}

		if( Player->CheckOpenSomething() != false )
			return false;

		Player->TempData.ShopInfo.TargetID = NPC->GUID();
		Player->TempData.Attr.Action.OpenType  = EM_RoleOpenMenuType_VisitHouse;

		NetSrv_Houses::SC_OpenVisitHouse( Player->GUID() , NPC->GUID() , IsVisitMyHouse );
		return true;
	}
	//建立玩家小屋
	bool	BuildHouse( int GItemID , int HouseType )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		if( Owner->PlayerBaseData->HouseDBID > 0 )
			return false;


		NetSrv_Houses::SD_BuildHouseRequest( Owner->DBID() , HouseType , (char*)Owner->Account_ID() );
		return true;
	}

	//設定為標準職業等級裝備
	bool	SetStandardWearEq( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		Global::SetStandardWearEq( Owner , 0 , 0 );
		return true;
	}

	//清除Magic Point 並給TP
	bool	SetStandardClearMagicPointAndSetTP( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		for( int i = 0 ; i < _MAX_SPSkill_COUNT_ + _MAX_NormalSkill_COUNT_ ; i++ )
		{
			Owner->SetValue( RoleValueName_ENUM( EM_RoleValue_NormalMagic + i ) , 0 , NULL );
		}

		int TpExp = 0;
		//tp計算
		for( int i = 1 ; i < Owner->Level() ; i++ )
		{
			TpExp += RoleDataEx::JobExpTable[ i - 1 ]/10;
			TpExp += ( i* 100 + 1000 );
		}
		Owner->SetValue( EM_RoleValue_TpExp , TpExp , NULL );
		Owner->SetValue( EM_RoleValue_STR , 0 , NULL);
		Owner->SetValue( EM_RoleValue_STA , 0 , NULL);
		Owner->SetValue( EM_RoleValue_INT , 0 , NULL);
		Owner->SetValue( EM_RoleValue_MND , 0 , NULL);
		Owner->SetValue( EM_RoleValue_AGI , 0 , NULL);
		Owner->SetValue( EM_RoleValue_Point , Owner->Level()-1 , NULL );

		return true;
	}

	//設定平行區域
	bool	SetParalledID( int GItemID , int ParalledID )
	{
		return Global::ChangeParalledID( GItemID , ParalledID );
	}

	//檢查區域是否開起
	bool	CheckZone( int ZoneID )
	{
		return Global::CheckZoneID( ZoneID );
	}

	enum ItemValueTypeENUM
	{
		EM_ItemValueType_OrgObjID		,
		EM_ItemValueType_Inherent1		,
		EM_ItemValueType_Inherent2		,
		EM_ItemValueType_Inherent3		,
		EM_ItemValueType_Inherent4		,
		EM_ItemValueType_Inherent5		,
		EM_ItemValueType_Inherent6		,
		EM_ItemValueType_Rune1			,
		EM_ItemValueType_Rune2			,
		EM_ItemValueType_Rune3			,
		EM_ItemValueType_Rune4			,
		EM_ItemValueType_Level			,
		EM_ItemValueType_Durable		,
		EM_ItemValueType_MaxDurable		,
		EM_ItemValueType_PowerQuality	,
		EM_ItemValueType_RuneVolume		,
	};

	bool	SetItemInfo( int GItemID , int PGType , int Pos , int ValueType , int Value )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;		

		ItemFieldStruct* Item;
		if( Pos == -1 )
		{
			//Item = Owner->GetItem( Owner->TempData.Magic.UseItemPos , PGType );
			Pos = Owner->TempData.Magic.UseItemPos;
		}

		Item = Owner->GetItem( Pos , PGType );

		if( Item == NULL )
			return false;

		GameObjDbStructEx* ItemDB = Global::GetObjDB( Item->OrgObjID );
		if( ItemDB == NULL )
			return false;

		switch( ValueType )
		{
		case EM_ItemValueType_OrgObjID:
			Item->OrgObjID = Value;
			Item->ImageObjectID = Value;
			break;
		case EM_ItemValueType_Inherent1:
			Item->Inherent(0,Value);
			break;
		case EM_ItemValueType_Inherent2:
			Item->Inherent(1,Value);
			break;
		case EM_ItemValueType_Inherent3:
			Item->Inherent(2,Value);
			break;
		case EM_ItemValueType_Inherent4:
			Item->Inherent(3,Value);
			break;
		case EM_ItemValueType_Inherent5:
			Item->Inherent(4,Value);
			break;
		case EM_ItemValueType_Inherent6:
			Item->Inherent(5,Value);
			break;
		case EM_ItemValueType_Rune1:
			Item->Rune(0,Value);
			break;
		case EM_ItemValueType_Rune2:
			Item->Rune(1,Value);
			break;
		case EM_ItemValueType_Rune3:
			Item->Rune(2,Value);
			break;
		case EM_ItemValueType_Rune4:
			Item->Rune(3,Value);
			break;
		case EM_ItemValueType_Level:
			Item->Level = Value;
			break;
		case  EM_ItemValueType_Durable:
			Item->Durable = Value;
			break;
		case EM_ItemValueType_MaxDurable:
			Item->Quality = Value / ItemDB->Item.Durable;
			break;
		case EM_ItemValueType_PowerQuality:
			Item->PowerQuality = Value;
			break;
		case  EM_ItemValueType_RuneVolume:
			Item->RuneVolume = Value;
			break;

		}
		Owner->Net_FixItemInfo( Pos , PGType );

		Owner->Log_ItemTrade(  Owner , EM_ActionType_LuaSetItemInfo , *Item , "" );

		return true;
	}
	//取得某欄位物品的資訊
	int		GetItemInfo( int GItemID , int PGType , int Pos , int ValueType )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return 0;		

		ItemFieldStruct* Item;
		if( Pos == -1 )
		{
			Item = Owner->GetItem( Owner->TempData.Magic.UseItemPos , PGType );
		}
		else
			Item = Owner->GetItem( Pos , PGType );

		if( Item == NULL )
			return 0;

		GameObjDbStructEx* ItemDB = Global::GetObjDB( Item->OrgObjID );
		if( ItemDB == NULL )
			return 0;

		switch( ValueType )
		{
		case EM_ItemValueType_OrgObjID:
			return Item->OrgObjID;
		case EM_ItemValueType_Inherent1:
			return Item->Inherent(0);
		case EM_ItemValueType_Inherent2:
			return Item->Inherent(1);
		case EM_ItemValueType_Inherent3:
			return Item->Inherent(2);
		case EM_ItemValueType_Inherent4:
			return Item->Inherent(3);
		case EM_ItemValueType_Inherent5:
			return Item->Inherent(4);
		case EM_ItemValueType_Inherent6:
			return Item->Inherent(5);
		case EM_ItemValueType_Rune1:
			return Item->Rune(0);
		case EM_ItemValueType_Rune2:
			return Item->Rune(1);
		case EM_ItemValueType_Rune3:
			return Item->Rune(2);
		case EM_ItemValueType_Rune4:
			return Item->Rune(3);
		case EM_ItemValueType_Level:
			return Item->Level;
		case  EM_ItemValueType_Durable:
			return Item->Durable;
		case EM_ItemValueType_MaxDurable:
			return Item->Quality * ItemDB->Item.Durable;
		case EM_ItemValueType_PowerQuality:
			return Item->PowerQuality;
		}
		return 0;
	}

	//取得名字
	const char*	GetName( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return "";

		return Global::GetRoleName( Owner );
	}

	//加輔助法術
	bool	AssistMagic( int GItemID , int MagicBaseID , int Time )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		GameObjDbStructEx* MagicBase = Global::GetObjDB( MagicBaseID );
		if( MagicBase == NULL )
			return false;

		if( MagicBase->IsMagicBase() != false && MagicBase->MagicBase.MagicFunc == EM_MAGICFUN_Assist )
		{
			BuffBaseStruct* Buff = Owner->AssistMagic( Owner , MagicBase->GUID , 0 , false , Time );
			//NetSrv_MagicChild::SendRange_AddBuffInfo( Owner->GUID() , Owner->GUID() , MagicBase->GUID , 0 , Time );
			if( Buff != NULL )
				NetSrv_MagicChild::SendRange_AddBuffInfo( Owner->GUID() , Owner->GUID() , MagicBase->GUID , Buff->Power , Buff->Time );
			//Owner->ReCalculateMagicAndEQ( );
			Owner->ReCalculateBuff();
			Owner->Calculate( );			
			return true;
		}
		return false;

	}
	//修理所有裝備
	bool	FixAllEq	( int GItemID , int Rate )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		bool isFix = false;
		for( int i = 0 ; i <= EM_EQWearPos_MaxCount ; i ++ )
		{
			if(		i == EM_EQWearPos_Ammo 
				||	i == EM_EQWearPos_Manufactory
				||	i == EM_EQWearPos_MagicTool1	
				||	i == EM_EQWearPos_MagicTool2	
				||	i == EM_EQWearPos_MagicTool3	)
				continue;
			ItemFieldStruct& Item = Owner->BaseData.EQ.Item[i];
			GameObjDbStructEx* ItemDB = Global::GetObjDB( Item.OrgObjID );
			if( ItemDB == NULL )
				continue;

			int Durable = ItemDB->Item.Durable * Item.Quality * Rate / 100 ;
			if( Durable > Item.Durable )
			{
				isFix = true;
				Item.Durable = Durable;
				Owner->Net_FixItemInfo_EQ( (EQWearPosENUM)i );
			}
		}
		Owner->TempData.UpdateInfo.Recalculate_All = true;
		return isFix;
	}
	//企劃Log
	bool	DesignLog	( int GItemID , int Type , const char* Content  )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;
		Global::Log_Design( Type , (char*)Content , Owner->DBID() , Owner->GuildID() );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	//計算有幾個空位
	int		EmptyPacketCount( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return 0;

		return Owner->EmptyPacketCount();
		/*
		int RetCount = 0;
		for( int i = _MAX_BODY_BEGIN_POS_ ; i < Owner->PlayerBaseData->Body.Count ; i++ )
		{	
		if( Owner->CheckItemPosTimeFlag( 0 , i ) == false )
		continue;

		if( Owner->PlayerBaseData->Body.Item[i].IsEmpty() )
		RetCount++;
		}
		return RetCount;
		*/
	}
	//計算包裹Queue
	int		QueuePacketCount( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return 0;

		return Owner->PlayerBaseData->ItemTemp.Size();
	}

	//區域PK旗標
	void	PKFlag			( int Type )
	{
		RoleDataEx::G_PKType = (PKTypeENUM)Type;
		NetSrv_Attack::SendAll_ZonePKFlag( RoleDataEx::G_PKType );
	}

	//清除陣營重生點
	void	ClearCampRevicePoint( )
	{
		Global::St()->CampReviveList.clear();
	}
	//設定陣營重生點
	bool	SetCampRevicePoint( int CampID , int ZoneID , float X , float Y , float Z , const char* LuaScript )
	{
		if( CampID > (int)Global::St()->CampReviveList.size() )
			return false;

		CampReviveStruct TempInfo;
		TempInfo.ZoneID = ZoneID;
		TempInfo.X = X;
		TempInfo.Y = Y;
		TempInfo.Z = Z;
		TempInfo.LuaScript = LuaScript;

		if( CampID == Global::St()->CampReviveList.size() )
			Global::St()->CampReviveList.push_back( TempInfo );
		else
			Global::St()->CampReviveList[CampID] = TempInfo;

		return true;
	}
	//檢查保箱是否還撿取完成
	bool	CheckTreasureDelete	( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		if( Owner->DynamicData.TreasureBox == NULL )
			return true;

		TreasureBoxStruct& TreasureBox = *(Owner->DynamicData.TreasureBox);
		if( TreasureBox.DeadTime + 60 < (int)RoleDataEx::G_Now )
		{
			if( TreasureBox.TreasurePartyID != -1 || TreasureBox.TreasureOwnerDBID != -1 )
			{
				TreasureBox.TreasurePartyID = -1;
				TreasureBox.TreasureOwnerDBID = -1;
				for( int i = 0 ; i < TreasureBox.Count ; i++ )
				{
					if( TreasureBox.Lock[i] == false )
						TreasureBox.OwnerDBID[i] = -1;
				}

				NetSrv_TreasureChild::SendRange_TreasureLootInfo( Owner );
			}
		}

		return Owner->CheckTreasureDelete();
	}

	//送信
	void	SendMail( const char* Name , const char* Title , const char* Content )
	{
		NetSrv_MailChild::SendSysMail( Name , Title , Content );
	}
	//送信
	void	SendMailEx( const char* fromName , const char* toName , const char* Title , const char* Content , int ItemID , int ItemCount , int Money )
	{
		MailBaseInfoStruct MailBaseInfo;

		memset( &MailBaseInfo , 0 , sizeof(MailBaseInfo) );
		MailBaseInfo.IsSystem = true;
		MailBaseInfo.LiveTime = 60*24*30;
		MailBaseInfo.OrgTargetName = toName;
		MailBaseInfo.OrgSendName = fromName;
		MailBaseInfo.Title = Title;
		if( Global::NewItemInit( MailBaseInfo.Item[0] , ItemID , 0 )  )
		{
			MailBaseInfo.Item[0].Count = ItemCount;
		}

		if( Money )
		{
			MailBaseInfo.IsSendMoney = true;
			MailBaseInfo.Money = Money;
		}

		NetSrv_MailChild::SD_SendMail( -1 , (char*)Content , MailBaseInfo , -1 );
		//NetSrv_MailChild::SendSysMailEx( fromName , Title , Content );
	}
	//設定兩人的關係
	bool	SetRelation(  int OwnerID , int TargetID , int Relation1 , float Lv1 , int Relation2 , float Lv2 )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( OwnerID );
		RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );

		if(	   Owner == NULL || Target == NULL
			|| Owner == Target 
			|| Owner->IsPlayer() == false 
			|| Target->IsPlayer() == false )
			return false;

		//找出
		BaseFriendStruct* OwnerFriend = NULL;
		BaseFriendStruct* TargetFriend = NULL;

		int OwnerPos = Owner->PlayerSelfData->FriendList.FindFriendInfo( EM_FriendListType_FamilyFriend, Target->DBID() , &OwnerFriend );
		int TargetPos = Target->PlayerSelfData->FriendList.FindFriendInfo( EM_FriendListType_FamilyFriend, Owner->DBID() , &TargetFriend );
		if( OwnerPos == -1 || TargetPos == -1 )
			return false;

		bool	IsOwnerInsert = true;
		bool	IsTargetInsert = true;
		if( OwnerFriend->IsEmpty() == false )
			IsOwnerInsert = false;
		if( TargetFriend->IsEmpty() == false )
			IsTargetInsert = false;


		OwnerFriend->DBID		= Target->DBID();
		OwnerFriend->Relation	= (RelationTypeENUM)Relation2;
		OwnerFriend->Race		= Target->TempData.Attr.Race;		//種族
		OwnerFriend->Voc		= Target->TempData.Attr.Voc;		//職業
		OwnerFriend->Voc_Sub	= Target->TempData.Attr.Voc_Sub;	//副職業
		OwnerFriend->Sex		= Target->TempData.Attr.Sex;		//性別
		OwnerFriend->JobLv		= Target->TempData.Attr.Level;	
		OwnerFriend->JobLv_Sub	= Target->TempData.Attr.Level_Sub;
		OwnerFriend->GuildID	= Target->BaseData.GuildID;
		OwnerFriend->TitleID	= Target->BaseData.TitleID;
		OwnerFriend->Time		= TimeStr::Now_Value();
		MyStrcpy( OwnerFriend->Name , Target->RoleName() , sizeof(OwnerFriend->Name) );
		OwnerFriend->Lv = Lv1;
		NetSrv_FriendList::SC_FixFriendInfo( Owner->GUID() , EM_FriendListType_FamilyFriend , OwnerPos , * OwnerFriend );
		NetSrv_FriendList::SChat_FriendListInfo( Owner->DBID() , Target->DBID() , IsOwnerInsert );

		TargetFriend->DBID		= Owner->DBID();
		TargetFriend->Relation	= (RelationTypeENUM)Relation1;
		TargetFriend->Race		= Owner->TempData.Attr.Race;		//種族
		TargetFriend->Voc		= Owner->TempData.Attr.Voc;		//職業
		TargetFriend->Voc_Sub	= Owner->TempData.Attr.Voc_Sub;	//副職業
		TargetFriend->Sex		= Owner->TempData.Attr.Sex;		//性別
		TargetFriend->JobLv		= Owner->TempData.Attr.Level;	
		TargetFriend->JobLv_Sub	= Owner->TempData.Attr.Level_Sub;
		TargetFriend->GuildID	= Owner->BaseData.GuildID;
		TargetFriend->TitleID	= Owner->BaseData.TitleID;
		TargetFriend->Time		= TimeStr::Now_Value();

		MyStrcpy( TargetFriend->Name , Owner->RoleName() , sizeof(OwnerFriend->Name) );
		TargetFriend->Lv = Lv2;
		NetSrv_FriendList::SC_FixFriendInfo( Target->GUID() , EM_FriendListType_FamilyFriend , TargetPos , * TargetFriend );
		NetSrv_FriendList::SChat_FriendListInfo( Target->DBID() , Owner->DBID() , IsTargetInsert );
		return true;
	}
	int		GetRelation( int OwnerID , int TargetID )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( OwnerID );
		RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );

		if(	   Owner == NULL || Target == NULL
			|| Owner->IsPlayer() == false 
			|| Target->IsPlayer() == false )
			return EM_RelationType_None;

		//找出
		BaseFriendStruct* OwnerFriend = NULL;

		if( Owner->PlayerSelfData->FriendList.FindFriendInfo( EM_FriendListType_FamilyFriend, Target->DBID() , &OwnerFriend ) == -1 )
			return EM_RelationType_None;

		if( OwnerFriend->IsEmpty( ) )
			return EM_RelationType_None;

		return OwnerFriend->Relation;
	}
	float	GetRelationLv( int OwnerID , int TargetID  )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( OwnerID );
		RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );

		if(	   Owner == NULL || Target == NULL
			|| Owner->IsPlayer() == false 
			|| Target->IsPlayer() == false )
			return 0;

		//找出
		BaseFriendStruct* OwnerFriend = NULL;

		if( Owner->PlayerSelfData->FriendList.FindFriendInfo( EM_FriendListType_FamilyFriend, Target->DBID() , &OwnerFriend ) == -1 )
			return 0;

		if( OwnerFriend->IsEmpty( ) )
			return 0;

		return OwnerFriend->Lv;
	}
	bool	SetRelationLv( int OwnerID , int TargetID , float Lv )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( OwnerID );
		RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );

		if(	   Owner == NULL || Target == NULL
			|| Owner->IsPlayer() == false 
			|| Target->IsPlayer() == false )
			return false;

		//找出
		BaseFriendStruct* OwnerFriend = NULL;

		int TempPos = Owner->PlayerSelfData->FriendList.FindFriendInfo( EM_FriendListType_FamilyFriend, Target->DBID() , &OwnerFriend );
		if( TempPos == -1 )
			return false;

		if( OwnerFriend->IsEmpty( ) )
			return false;

		//OwnerFriend->Lv = Lv;
		Owner->SetValue( (RoleValueName_ENUM)(EM_RoleValue_FamilyFriendListLv_Pos + TempPos) , OwnerFriend->Lv );

		return true;
	}
	bool	AddSkillValue (int OwnerID , int SkillID , float Value )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( OwnerID );
		if( Owner == NULL )
			return false;

		Owner->AddSkillValue( -1 , (SkillValueTypeENUM)SkillID , Value );
		return true;
	}

	int			GetRelation_DBID( int OwnerID , int Pos )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( OwnerID );
		if( Owner == NULL )
			return -1;

		return Owner->PlayerSelfData->FriendList.AllFriendList[ Pos ].DBID;

	}
	const char*	GetRelation_Name( int OwnerID , int Pos )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( OwnerID );
		if( Owner == NULL )
			return "";

		return Owner->PlayerSelfData->FriendList.AllFriendList[ Pos ].Name;
	}
	float		GetRelation_Lv( int OwnerID , int Pos )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( OwnerID );
		if( Owner == NULL )
			return -1;

		return Owner->PlayerSelfData->FriendList.AllFriendList[ Pos ].Lv;
	}

	int			GetRelation_Relation( int OwnerID , int Pos )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( OwnerID );
		if( Owner == NULL )
			return -1;

		return Owner->PlayerSelfData->FriendList.AllFriendList[ Pos ].Relation;
	}

	bool		SetRelation_Lv( int OwnerID , int Pos , float Lv )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( OwnerID );
		if( Owner == NULL )
			return false;

		Owner->PlayerSelfData->FriendList.AllFriendList[ Pos ].Lv = Lv;
		return true;
	}

	bool		SetRelation_Relation( int OwnerID , int Pos , int Relation )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( OwnerID );
		if( Owner == NULL )
			return false;

		Owner->PlayerSelfData->FriendList.AllFriendList[ Pos ].Relation = (RelationTypeENUM)Relation;
		return true;
	}

	bool		CallPlayer_DBID( int OwnerID , int TargetDBID )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( OwnerID );
		if( Owner == NULL )
			return false;
		if( Owner->BaseData.RoomID != 0 )
			return false;

		OnlinePlayerInfoStruct* Info = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( TargetDBID );
		if( Info == NULL )
			return false;

		NetSrv_Other::S_CallPlayer( *(Owner->Pos()) , (char*)Info->Name.Begin() , 0 );
		return true;
	}
	bool		GotoPlayer_DBID( int OwnerID , int TargetDBID )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( OwnerID );
		if( Owner == NULL )
			return false;

		OnlinePlayerInfoStruct* Info = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( TargetDBID );
		if( Info == NULL )
			return false;

		if( Info->RoomID != 0 )
			return false;

		NetSrv_Other::S_RequestPlayerPos( Owner->DBID() , (char*)Info->Name.Begin() );
		return true;

	}
	int		GetZoneID( int PlayerDBID )
	{
		OnlinePlayerInfoStruct* Info = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( PlayerDBID );
		if( Info == NULL )
			return -1;

		return Info->ZoneID;
	}

	//檢查二次密碼
	bool	CheckPassword( int OwnerID , const char* Password )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( OwnerID );
		if( Owner == NULL )
			return false;

		MyMD5Class Md5;
		Md5.ComputeStringHash( Password );
		if( stricmp( Md5.GetMd5Str() , Owner->PlayerBaseData->Password.Begin() ) != 0 )
		{
			Owner->Net_GameMsgEvent( EM_GameMessageEvent_SecondPasswordError );
			return false;
		}
		return true;
	}

	int		SysKeyValue( const char* KeyStr )
	{
		return g_ObjectData->GetSysKeyValue( KeyStr );
	}

	int		LanguageType( )
	{
		return Global::Ini()->CountryType;
	}

	int		GetMoneyKeyValue( const char* KeyStr , int Pos )
	{
		return g_ObjectData->GetMoneyKeyValue( KeyStr , Pos );
	}
	//////////////////////////////////////////////////////////////////////////
	//公會屋
	//////////////////////////////////////////////////////////////////////////
	//建立公會
	bool	BuildGuildHouse( int GItemID )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;
		if( Owner->BaseData.GuildID == 0 )
			return false;

		GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember( Owner->DBID() );
		if( GuildMember == NULL )
			return false;

		GuildStruct* GuildInfo = GuildMember->Guild;

		if( GuildInfo->Base.IsOwnHouse != false )
			return false;

		NetSrv_GuildHouses::SD_BuildHouseRequest( Owner->DBID() , Owner->GuildID() );
		return true;
	}

	//要求開啟 公會屋參觀介面
	bool	OpenVisitGuildHouse( bool IsVisitMyHouse )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		NPC  = Global::GetRoleDataByGUID( T->TargetID );
		if( Player == NULL || NPC == NULL )
		{
			Print( LV2 , "OpenVisitGuildHouse" , "NPC == NULL || NPC->IsPlayer() Func = %d" , T->FuncName );
			return false;
		}

		int ZoneID = GlobalBase::GetParallelZoneID( g_ObjectData->GetSysKeyValue( "GuildHouse_ZoneID" ) ,  0 );
		if( ZoneID == -1 )
		{
			Player->Net_GameMsgEvent( EM_GameMessageEvent_ChanageZoneError_Failed );
			return false;
		}

		if( IsVisitMyHouse != false )
		{
			GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember( Player->DBID() );
			if( GuildMember == NULL )
				return false;

			if( GuildMember->Guild->Base.IsOwnHouse == false )
				return false;
		}

		if( Player->CheckOpenSomething() != false )
			return false;

		Player->TempData.ShopInfo.TargetID = NPC->GUID();
		Player->TempData.Attr.Action.OpenType  = EM_RoleOpenMenuType_VisitGuildHouse;

		NetSrv_GuildHousesChild::SC_OpenVisitHouse( Player->GUID() , NPC->GUID() , IsVisitMyHouse );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	//設定房屋類型
	bool	SetHouseType( int HouseImageObjID )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		BaseItemObject* PlayerObj = Global::GetObj( T->OwnerID );
		if( PlayerObj == NULL )
			return false;
		RoleDataEx*     Player = PlayerObj->Role();
		if( Global::Ini()->IsHouseZone == false )
			return false;

		if( Player->PlayerTempData->VisitHouseDBID != Player->PlayerBaseData->HouseDBID )
			return false;

		HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Player->PlayerBaseData->HouseDBID );
		if( houseClass == NULL )
			return false;

		if( houseClass->GetHouseBase()->Info.HouseType == HouseImageObjID )
			return false;


		HousesInfoStruct* houseBase =  houseClass->GetHouseBase();
		vector< HouseItemStruct* >& ItemList = houseBase->ItemList[ -1 ];

		for( unsigned int i = 0 ; i < ItemList.size() ; i++ )
		{
			HouseItemStruct* houseItem = ItemList[i];
			if( houseItem == NULL )
				continue;

			if( houseItem->Info.Layout.IsShow != false )
			{
				houseItem->Info.Layout.IsShow = false;
				if( houseItem->SaveState == EM_HouseItemSaveState_Normal )
					houseItem->SaveState = EM_HouseItemSaveState_Update;
			}

		}

		vector< HouseItemStruct* >& ItemList1 = houseBase->ItemList[ -2 ];

		for( unsigned int i = 0 ; i < ItemList1.size() ; i++ )
		{
			HouseItemStruct* houseItem = ItemList1[i];
			if( houseItem == NULL )
				continue;

			if( houseItem->Info.Layout.IsShow != false )
			{
				houseItem->Info.Layout.IsShow = false;
				if( houseItem->SaveState == EM_HouseItemSaveState_Normal )
					houseItem->SaveState = EM_HouseItemSaveState_Update;
			}

		}

		houseBase->Info.HouseType = HouseImageObjID;
		houseBase->IsNeedSave = true;
		houseClass->SetSaveInfo();

		//NetSrv_Houses::SC_ChangeHouseResult( Player->GUID() , HouseImageObjID , true );
		//NetSrv_MoveChild::SetObjPos( PlayerObj , 0 , 0 , 0 , Player->Pos()->Dir );

		NetSrv_HousesChild::SendAllCli_ChangeHouseResult( Player->PlayerBaseData->HouseDBID , HouseImageObjID , true );		
		return true;
	}

	int		GetHouseType( )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		if( Global::Ini()->IsHouseZone == false )
			return 0;

		if( Player->PlayerTempData->VisitHouseDBID != Player->PlayerBaseData->HouseDBID )
			return 0;

		HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Player->PlayerBaseData->HouseDBID );
		if( houseClass == NULL )
			return 0;

		return houseClass->GetHouseBase()->Info.HouseType;
	}
	//////////////////////////////////////////////////////////////////////////
	// AI 處理
	//////////////////////////////////////////////////////////////////////////
	//取得仇恨列表內人數量
	int		HateListCount( int GItemID )
	{
		RoleDataEx*     Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return 0;
		return Owner->TempData.NPCHate.Hate.Size();
	}

	//取得仇恨列表中的某幾個位置的資料
	enum HateListInfoTypeENUM
	{
		EM_HateListInfoType_DBID	,
		EM_HateListInfoType_GItemID	,
		EM_HateListInfoType_HitPoint,
		EM_HateListInfoType_HatePoint,
		EM_HateListInfoType_TempHatePoint,
	};

	double		HateListInfo( int GItemID , int Pos , int Type )
	{
		RoleDataEx*     Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return 0;

		if( Pos >= Owner->TempData.NPCHate.Hate.Size() )
			return 0;
		NPCHateStruct& HateInfo = Owner->TempData.NPCHate.Hate[ Pos ];

		switch( Type )
		{
		case EM_HateListInfoType_DBID:
			return HateInfo.DBID;
		case EM_HateListInfoType_GItemID:
			return HateInfo.ItemID;
		case EM_HateListInfoType_HitPoint:
			return HateInfo.HitPoint;
		case EM_HateListInfoType_HatePoint:
			return HateInfo.HatePoint;
		case EM_HateListInfoType_TempHatePoint:
			return HateInfo.TempHatePoint;
		}
		return 0;
	}
	//設定仇恨列表中的某幾個位置的資料
	bool	SetHateListPoint( int GItemID , int Pos , float Value )
	{
		RoleDataEx*     Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		if( Pos >= Owner->TempData.NPCHate.Hate.Size() )
			return false;

		NPCHateStruct& HateInfo = Owner->TempData.NPCHate.Hate[ Pos ];
		HateInfo.HatePoint = Value;
		return true;
	}

	bool	ModifyRangeHatePoint( int GItemID , int Type , float Point )
	{
		RoleDataEx*     Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;


		Global::ResetRange( Owner , _Def_View_Block_Range_ );
		while( 1 )
		{
			RoleDataEx* Other = Global::GetRangeRole( );
			if( Other == NULL )
				break;
			if( Other->RoomID() != Owner->RoomID() )
				continue;

			StaticVector< NPCHateStruct , _MAX_HATELIST_SIZE_ >& Hate = Other->TempData.NPCHate.Hate;
			for( int i = 0 ; i < Hate.Size() ; i++ )
			{
				if( Hate[i].ItemID != GItemID )
					continue;
				
				if( Type == 0 )
				{
					Hate[i].HatePoint = ( Hate[i].HatePoint * Point );
				}
				else
				{
					Hate[i].HatePoint = ( Hate[i].HatePoint + Point );
				}
				break;
			}
		}

		return true;
	}


	//暫停或開啟NPC AI處理
	bool	EnableNpcAI( int GItemID , bool Flag )
	{
		BaseItemObject*     OwnerObj = Global::GetObj( GItemID );
		if( OwnerObj == NULL || OwnerObj->Role()->IsPlayer() )
			return false;

		OwnerObj->AI()->EnableAI( Flag );
		return true;
	}
	//清除愁恨表 TargetID = -1 表示全清
	bool	ClearHateList( int GItemID , int TargetID )
	{
		RoleDataEx*     Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		for( int i = 0 ; i < Owner->TempData.NPCHate.Hate.Size() ; i++ )
		{
			NPCHateStruct& HateInfo = Owner->TempData.NPCHate.Hate[ i ];
			HateInfo.HatePoint = 0;
		}
		Owner->TempData.NPCHate.MaxHitPoint = 0;

		return true;
	}

	//設定Mode
	enum	SetModeTypeENUM
	{
		EM_SetModeType_Show,
		EM_SetModeType_Mark,	    		
		EM_SetModeType_Obstruct,
		EM_SetModeType_Gravity,
		EM_SetModeType_Save,
		EM_SetModeType_Drag,
		EM_SetModeType_GMHide,
		EM_SetModeType_IsAllZoneVisible,
		EM_SetModeType_DisableRotate,
		EM_SetModeType_Strikback,
		EM_SetModeType_Move,
		EM_SetModeType_Searchenemy,
		EM_SetModeType_Revive,
		EM_SetModeType_Fight,
		EM_SetModeType_Escape,
		EM_SetModeType_SearchenemyIgnore,
		EM_SetModeType_AlignToSurface,	
		EM_SetModeType_HideName,	
		EM_SetModeType_HideMinimap,
		EM_SetModeType_ShowRoleHead,	
		EM_SetModeType_CollisionPlot,	
		EM_SetModeType_Treasure,
		EM_SetModeType_IgnoreNpcKill,
		EM_SetModeType_NotShowHPMP,
		EM_SetModeType_DisableAction,
	};

	bool	SetModeEx( int GItemID , int Type , bool Flag )
	{
		RoleDataEx*     Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer()  )
			return false;

		switch( Type )
		{
		case EM_SetModeType_Show:
			Owner->BaseData.Mode.Show = Flag;
			break;
		case EM_SetModeType_Mark:   		
			Owner->BaseData.Mode.Mark = Flag;
			break;
		case EM_SetModeType_Obstruct:
			Owner->BaseData.Mode.Obstruct = Flag;
			break;
		case EM_SetModeType_Gravity:
			Owner->BaseData.Mode.Gravity = Flag;
			break;
		case EM_SetModeType_Save:
			Owner->BaseData.Mode.Save = Flag;
			break;
		case EM_SetModeType_Drag:
			Owner->BaseData.Mode.Drag = Flag;
			break;
		case EM_SetModeType_GMHide:
			Owner->BaseData.Mode.GMHide = Flag;
			break;
		case EM_SetModeType_IsAllZoneVisible:
			Owner->BaseData.Mode.IsAllZoneVisible = Flag;
			break;
		case EM_SetModeType_DisableRotate:
			Owner->BaseData.Mode.DisableRotate = Flag;
			break;
		case EM_SetModeType_Strikback:
			Owner->BaseData.Mode.Strikback = Flag;
			break;
		case EM_SetModeType_Move:
			Owner->BaseData.Mode.Move = Flag;
			break;
		case EM_SetModeType_Searchenemy:
			Owner->BaseData.Mode.Searchenemy = Flag;
			break;
		case EM_SetModeType_Revive:
			Owner->BaseData.Mode.Revive = Flag;
			break;
		case EM_SetModeType_Fight:
			Owner->BaseData.Mode.Fight = Flag;
			break;
		case EM_SetModeType_Escape:
			Owner->BaseData.Mode.Escape = Flag;
			break;
		case EM_SetModeType_SearchenemyIgnore:
			Owner->BaseData.Mode.SearchenemyIgnore = Flag;
			break;
		case EM_SetModeType_AlignToSurface:
			Owner->BaseData.Mode.AlignToSurface = Flag;
			break;
		case EM_SetModeType_HideName:
			Owner->BaseData.Mode.HideName = Flag;
			break;
		case EM_SetModeType_HideMinimap:
			Owner->BaseData.Mode.HideMinimap = Flag;
			break;
		case EM_SetModeType_ShowRoleHead:
			Owner->BaseData.Mode.ShowRoleHead = Flag;
			break;
		case EM_SetModeType_CollisionPlot:
			Owner->BaseData.Mode.Mark = Flag;
			break;
		case EM_SetModeType_Treasure:
			Owner->BaseData.Mode.Mark = Flag;
			break;
		case EM_SetModeType_IgnoreNpcKill:
			Owner->BaseData.Mode.IgnoreNpcKill = Flag;
			break;
		case EM_SetModeType_NotShowHPMP:
			Owner->BaseData.Mode.NotShowHPMP = Flag;
			break;
		case EM_SetModeType_DisableAction:
			Owner->BaseData.Mode.DisableAction = Flag;
			break;
		}
		return true;
	}
	bool	GetModeEx( int GItemID , int Type  )
	{
		RoleDataEx*     Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL  )
			return false;

		switch( Type )
		{
		case EM_SetModeType_Show:
			return Owner->BaseData.Mode.Show;
		case EM_SetModeType_Mark:   		
			return Owner->BaseData.Mode.Mark;
		case EM_SetModeType_Obstruct:
			return Owner->BaseData.Mode.Obstruct;
		case EM_SetModeType_Gravity:
			return Owner->BaseData.Mode.Gravity;
		case EM_SetModeType_Save:
			return Owner->BaseData.Mode.Save;
		case EM_SetModeType_Drag:
			return Owner->BaseData.Mode.Drag;
		case EM_SetModeType_GMHide:
			return Owner->BaseData.Mode.GMHide;
		case EM_SetModeType_IsAllZoneVisible:
			return Owner->BaseData.Mode.IsAllZoneVisible;
		case EM_SetModeType_DisableRotate:
			return Owner->BaseData.Mode.DisableRotate;
		case EM_SetModeType_Strikback:
			return Owner->BaseData.Mode.Strikback;
		case EM_SetModeType_Move:
			return Owner->BaseData.Mode.Move;
		case EM_SetModeType_Searchenemy:
			return Owner->BaseData.Mode.Searchenemy;
		case EM_SetModeType_Revive:
			return Owner->BaseData.Mode.Revive;
		case EM_SetModeType_Fight:
			return Owner->BaseData.Mode.Fight;
		case EM_SetModeType_Escape:
			return Owner->BaseData.Mode.Escape;
		case EM_SetModeType_SearchenemyIgnore:
			return Owner->BaseData.Mode.SearchenemyIgnore;
		case EM_SetModeType_AlignToSurface:
			return Owner->BaseData.Mode.AlignToSurface;
		case EM_SetModeType_HideName:
			return Owner->BaseData.Mode.HideName;
		case EM_SetModeType_HideMinimap:
			return Owner->BaseData.Mode.HideMinimap;
		case EM_SetModeType_ShowRoleHead:
			return Owner->BaseData.Mode.ShowRoleHead;
		case EM_SetModeType_CollisionPlot:
			return Owner->BaseData.Mode.Mark;
		case EM_SetModeType_Treasure:
			return Owner->BaseData.Mode.Mark;
		case EM_SetModeType_IgnoreNpcKill:
			return Owner->BaseData.Mode.IgnoreNpcKill;
		case EM_SetModeType_NotShowHPMP:
			return Owner->BaseData.Mode.NotShowHPMP;
		case EM_SetModeType_DisableAction:
			return Owner->BaseData.Mode.DisableAction;
		}
		return false;
	}
	//檢查是否有組檔
	bool	CheckLine( int GItemID , float Tx , float Ty , float Tz )
	{
		BaseItemObject* OwnerObj = Global::GetObj( GItemID );
		if(  OwnerObj == NULL )
			return false;
		RoleDataEx*     Owner = OwnerObj->Role();
		if( Owner == NULL )
			return false;

		bool Ret = OwnerObj->Path()->CheckLine( Owner->X() , Owner->Y() , Owner->Z() , Tx , Ty , Tz );

		return Ret;
	}

	//取得某點的高度 (回傳等於 -100000 表示找不到高度)
	float		GetHeight( float X , float Y , float Z )
	{
		float	OutY = -100000;

		PlotRegStruct* T = LUA_VMClass::ProcReg();
		BaseItemObject* OwnerObj = Global::GetObj( T->OwnerID );
		if( OwnerObj == NULL )
			return 0;
	
		OwnerObj->Path()->Height( X , Y , Z , OutY , 1000 );

		return OutY;
	}

	//--------------------------------------------------------------------------------
	//給與裝備
	bool	GiveBodyItem_EQ( int GItemID , int OrgObjID , int Level , int RuneVolume )
	{
		PlotRegStruct* T = LUA_VMClass::ProcReg();
		RoleDataEx* NPC  = NULL;
		if( T != NULL )
		{
			NPC = Global::GetRoleDataByGUID( T->OwnerID );
			if( NPC == NULL || NPC->IsPlayer() )
			{
				NPC = Global::GetRoleDataByGUID( T->TargetID );
			}
		}

		RoleDataEx* Target  = Global::GetRoleDataByGUID( GItemID );
		if( Target == NULL )
		{
			Print( LV3 , "GiveBodyItem_EQ" , "Target == NULL ItemID=%d OrgID=%d",GItemID, OrgObjID );
			return false;
		}

		GameObjDbStructEx* OrgDB = Global::GetObjDB( OrgObjID );

		if( OrgDB == NULL )
			return false;

		if( OrgDB->IsArmor() == false && OrgDB->IsWeapon() == false )
			return false;


		ItemFieldStruct	Item;
		if( Global::NewItemInit( Item , OrgObjID , 0 ) == false )
			return false;

		Item.Count		= 1;
		Item.OrgObjID	= OrgObjID;
		Item.Level		= Level;
		Item.RuneVolume	= RuneVolume;

		if( Target->GiveItem( Item , EM_ActionType_PlotGive , NPC , "" ) == false )
			return false;

		return true;
	}
	//---------------------------------------------------------------------------------
	bool	CancelBuff			( int iRoleID, int iMagicID )
	{
		RoleDataEx*		pRole = Global::GetRoleDataByGUID( iRoleID );
		if( pRole == NULL )
			return false;
		return pRole->ClearBuff( iMagicID , -1 );
	}

	bool	CancelBuff_NoEvent	( int iRoleID	, int iMagicID )
	{
		RoleDataEx*		pRole = Global::GetRoleDataByGUID( iRoleID );
		if( pRole == NULL )
			return false;
		return pRole->ClearBuff_NoEvent( iMagicID , -1 );
	}


	bool	AddBuff			( int ItemGUID , int BuffID , int MagicLv , int EffectTime )
	{
		RoleDataEx*	Owner = Global::GetRoleDataByGUID( ItemGUID );
		if( Owner == NULL )
			return false;

		BuffBaseStruct* RetBuf = Owner->AssistMagic( Owner , BuffID , MagicLv , false , EffectTime );
		if( RetBuf == NULL )
			return false;

		//NetSrv_MagicChild::SendRange_AddBuffInfo( Owner->GUID() , Owner->GUID() ,  BuffID , MagicLv , EffectTime );
		NetSrv_MagicChild::SendRange_AddBuffInfo( Owner->GUID() , Owner->GUID() ,  BuffID , RetBuf->Power , RetBuf->Time );
		Owner->TempData.UpdateInfo.Recalculate_Buff = true;

		return true;
	}
	int		BuffCount		( int ItemGUID )
	{
		RoleDataEx*	Owner = Global::GetRoleDataByGUID( ItemGUID );
		if( Owner == NULL )
			return 0;

		return Owner->BaseData.Buff.Size();
	}

	enum BuffInfoTypeENUM
	{
		EM_BuffInfoType_OwnerID,
		EM_BuffInfoType_BuffID,         //魔法ID
		EM_BuffInfoType_Time,           //剩下時間(秒)
		EM_BuffInfoType_Power,			//威力( 魔法累積等級 )
		EM_BuffInfoType_Point,			//點數( 魔法盾可擋點數 or 次數)
	};
	int		BuffInfo		( int ItemGUID , int Pos , int Type )
	{
		RoleDataEx*	Owner = Global::GetRoleDataByGUID( ItemGUID );
		if( Owner == NULL )
			return 0;

		if( unsigned(Pos) >= (unsigned)Owner->BaseData.Buff.Size() )
			return 0;

		switch( Type )
		{
		case EM_BuffInfoType_OwnerID:
			return Owner->BaseData.Buff[Pos].OwnerID;
		case EM_BuffInfoType_BuffID:         //魔法ID
			return Owner->BaseData.Buff[Pos].BuffID;
		case EM_BuffInfoType_Time:           //剩下時間(秒)
			return Owner->BaseData.Buff[Pos].Time;
		case EM_BuffInfoType_Power:			//威力( 魔法累積等級 )
			return Owner->BaseData.Buff[Pos].Power;
		case EM_BuffInfoType_Point:			//點數( 魔法盾可擋點數 or 次數)
			return Owner->BaseData.Buff[Pos].Point;
		}
		return 0;
	}


	ReaderClass< DB_GameObjDbStruct >* GetObjectReader( GameObjDbStructEx* ObjDB )
	{
		ReaderClass< DB_GameObjDbStruct > *Reader = NULL;

		switch( ObjDB->Classification )
		{
		case EM_ObjectClass_None:
			break;
		case EM_ObjectClass_Player:
			Reader = GameObjDbStructEx::ObjReader_Player();
			break;
		case EM_ObjectClass_NPC:
			Reader = GameObjDbStructEx::ObjReader_NPC( );
			break;
		case EM_ObjectClass_Item:
			{
				switch( ObjDB->Item.Class )
				{
				case EM_Item_Normal:
					Reader = GameObjDbStructEx::ObjReader_Item();
					break;
				case EM_Item_Weapon:
					Reader = GameObjDbStructEx::ObjReader_Weapon();
					break;
				case EM_Item_Armor:
					Reader = GameObjDbStructEx::ObjReader_Armor();
					break;
				case EM_Item_Rune:
					Reader = GameObjDbStructEx::ObjReader_Rune();
					break;
				case EM_Item_Card:
					Reader = GameObjDbStructEx::ObjReader_Card();
					break;
				}
			}
			break;
		case EM_ObjectClass_MagicBase:
			Reader = GameObjDbStructEx::ObjReader_MagicBase();
			break;
		case EM_ObjectClass_BodyObj:
			Reader = GameObjDbStructEx::ObjReader_Body();
			break;
		case EM_ObjectClass_Attribute:
			Reader = GameObjDbStructEx::ObjReader_AddPower( );
			break;
		case EM_ObjectClass_QuestCollect:
			break;
		case EM_ObjectClass_QuestDetail:
			Reader = GameObjDbStructEx::ObjReader_QuestDetail();
			break;
		case EM_ObjectClass_Title:
			Reader = GameObjDbStructEx::ObjReader_Title();
			break;
		case EM_ObjectClass_KeyItem:
			Reader = GameObjDbStructEx::ObjReader_KeyItem();
			break;
		case EM_ObjectClass_Recipe:
			Reader = GameObjDbStructEx::ObjReader_Recipe();
			break;
		case EM_ObjectClass_Mine:
			Reader = GameObjDbStructEx::ObjReader_Mine( );
			break;
		case EM_ObjectClass_Flag:
			Reader = GameObjDbStructEx::ObjReader_Flag( );
			break;
		case EM_ObjectClass_Image:
			Reader = GameObjDbStructEx::ObjReader_Image( );
			break;
		case EM_ObjectClass_QuestNPC:
			Reader = GameObjDbStructEx::ObjReader_QuestNPC();
			break;
		case EM_ObjectClass_LearnMagic:
			Reader = GameObjDbStructEx::ObjReader_LearnMagic();
			break;
		case EM_ObjectClass_Shop:
			Reader = GameObjDbStructEx::ObjReader_Shop();
			break;
		case EM_ObjectClass_Suit:
			Reader = GameObjDbStructEx::ObjReader_Suit();
			break;
		case EM_ObjectClass_LuaScript:
			break;
		case EM_ObjectClass_Camp:
			Reader = GameObjDbStructEx::ObjReader_Camp();
			break;
		case EM_ObjectClass_Treasure:
			Reader = GameObjDbStructEx::ObjReader_Treasure();
			break;
		case EM_ObjectClass_MagicCollect:
			Reader = GameObjDbStructEx::ObjReader_MagicCollect();
			break;
		case EM_ObjectClass_EqRefineAbility:
			Reader = GameObjDbStructEx::ObjReader_EqRefineAbility();
			break;
		case EM_ObjectClass_Zone:
			Reader = GameObjDbStructEx::ObjReader_Zone();
			break;
		case EM_ObjectClass_CreateRole:
			Reader = GameObjDbStructEx::ObjReader_CreateRole();
			break;
		}
		return Reader;
	}

	double		GameObjInfo_Double		( int OrgObjID , const char* DB_FieldName )
	{
		GameObjDbStructEx* ObjDB = Global::GetObjDB( OrgObjID );
		if( ObjDB == NULL )
			return 0;

		ReaderClass< DB_GameObjDbStruct > *Reader = GetObjectReader( ObjDB );
		if( Reader == NULL )
			return 0;

		void* TempObjPoint = ObjDB;
		return Reader->Read( static_cast<DB_GameObjDbStruct*>(TempObjPoint) , DB_FieldName );
	}
/*
	float		GameObjInfo_Float		( int OrgObjID , const char* DB_FieldName )
	{
		GameObjDbStructEx* ObjDB = Global::GetObjDB( OrgObjID );
		if( ObjDB == NULL )
			return 0;

		ReaderClass< DB_GameObjDbStruct > *Reader = GetObjectReader( ObjDB->Classification );
		if( Reader == NULL )
			return 0;

		void* TempObjPoint = ObjDB;
		return Reader->Read_Float( static_cast<DB_GameObjDbStruct*>(TempObjPoint) , DB_FieldName );
	}
	*/

	const char*	GameObjInfo_Str		( int OrgObjID , const char* DB_FieldName )
	{
		GameObjDbStructEx* ObjDB = Global::GetObjDB( OrgObjID );
		if( ObjDB == NULL )
			return "";
		ReaderClass< DB_GameObjDbStruct > *Reader = GetObjectReader( ObjDB );

		if( Reader == NULL )
			return "";

		void* TempObjPoint = ObjDB;
		return Reader->ReadStr( static_cast<DB_GameObjDbStruct*>(TempObjPoint) , DB_FieldName );
	}

	bool	WriteGameObjInfo_Double( int OrgObjID , const char* DB_FieldName , double value )
	{
		GameObjDbStructEx* ObjDB = Global::GetObjDB( OrgObjID );
		if( ObjDB == NULL )
			return false;

		ReaderClass< DB_GameObjDbStruct > *Reader = GetObjectReader( ObjDB );
		if( Reader == NULL )
			return false;

		void* TempObjPoint = ObjDB;
		return Reader->Write( static_cast<DB_GameObjDbStruct*>(TempObjPoint) , DB_FieldName , value );

	}

	bool		TouchTomb( )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		NPC  = Global::GetRoleDataByGUID( T->TargetID );

		if( Player == NULL || NPC == NULL )
			return false;

		if( NPC->BaseData.ItemInfo.OrgObjID != g_ObjectData->SysValue().TombObjID )
			return false;

		if( NPC->TempData.Sys.OwnerDBID != Player->DBID() )
			return false;

		TombStruct&	Tomb = Player->PlayerSelfData->Tomb;

		Player->AddValue( EM_RoleValue_DebtExp , Tomb.DebtExp * -1 );
		Player->Net_DeltaRoleValue( EM_RoleValue_DebtExp , Tomb.DebtExp * -1 );		

		Player->AddValue( EM_RoleValue_DebtTP , Tomb.DebtTp * -1 );
		Player->Net_DeltaRoleValue( EM_RoleValue_DebtTP , Tomb.DebtTp * -1 );		


		Player->AddValue( EM_RoleValue_EXP , Tomb.Exp * -1 );
		Player->Net_DeltaRoleValue( EM_RoleValue_EXP , Tomb.Exp * -1 );		


		Player->PlayerSelfData->Tomb.Init();
		NPC->Destroy( "Touch Tomb" );

		NetSrv_CliConnect::SC_TouchTomb( Player->GUID() );

		return true;

	}

	void	ClientSkyType	( int Type )
	{
		NetSrv_CliConnectChild::SetClientSkyProcType( (ClientSkyProcTypeENUM) Type );
	}

	//////////////////////////////////////////////////////////////////////////
	//給予金錢
	//////////////////////////////////////////////////////////////////////////
	bool	AddMoney( int GItemID , int ActionType , int Money )
	{
		RoleDataEx*     Player = Global::GetRoleDataByGUID( GItemID );		
		if( Player == NULL )
			return false;

		return Player->AddBodyMoney( Money , NULL , (ActionTypeENUM)ActionType , true );
	}

	bool	AddSpeicalCoin( int GItemID , int ActionType , int MoneyType , int Money )
	{
		RoleDataEx*     Player = Global::GetRoleDataByGUID( GItemID );		
		if( Player == NULL )
			return false;
		return Global::AddMoneyBase( Player , (ActionTypeENUM)ActionType , (PriceTypeENUM)MoneyType , Money );
	}
	bool	CheckSpeicalCoin( int GItemID , int MoneyType , int Money )
	{
		RoleDataEx*     Player = Global::GetRoleDataByGUID( GItemID );		
		if( Player == NULL )
			return false;
		return Global::CheckMoneyBase( Player , (PriceTypeENUM)MoneyType , Money );
	}

	bool	AddMoney_Bonus( int GItemID , int ActionType , int Money )
	{
		RoleDataEx*     Player = Global::GetRoleDataByGUID( GItemID );		
		if( Player == NULL )
			return false;

		return Player->AddMoney_Bonus( Money , NULL , (ActionTypeENUM)ActionType );
	}
	bool	AddMoney_Account( int GItemID , int ActionType , int Money )
	{
		RoleDataEx*     Player = Global::GetRoleDataByGUID( GItemID );		
		if( Player == NULL )
			return false;

		if (Global::AccountMoneyOperable() == false)
			return false;

		return Player->AddBodyMoney_Account( Money , NULL , (ActionTypeENUM)ActionType , true );
	}

	bool	AddMoneyEx_Account( int GItemID , int ActionType , int Money , bool LockMoneyProc )
	{
		RoleDataEx*     Player = Global::GetRoleDataByGUID( GItemID );		
		if( Player == NULL )
			return false;

		if (Global::AccountMoneyOperable() == false)
			return false;

		return Player->AddBodyMoney_Account( Money , NULL , (ActionTypeENUM)ActionType, true, LockMoneyProc );
	}
	//--------------------------------------------------------------------------------
	//讀取得公會建築資料
	enum GuildBuildValueTypeENUM
	{
		EM_GuildBuildValueType_GuildID			,
		EM_GuildBuildValueType_BuildingDBID		,
		EM_GuildBuildValueType_ParentDBID		,
		EM_GuildBuildValueType_BuildingID		,
		EM_GuildBuildValueType_IsActive			,
		EM_GuildBuildValueType_Value1			,
		EM_GuildBuildValueType_Value2			,
		EM_GuildBuildValueType_Value3			,
	};

	int		ReadGuildBuilding( int ItemGUID , int Type )
	{
		RoleDataEx*     BuildingRole = Global::GetRoleDataByGUID( ItemGUID );		
		if( BuildingRole == NULL )
			return 0;

		if( BuildingRole->TempData.GuildBuildingType != EM_GuildBuildingType_Building  )
			return 0;

		GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( BuildingRole->GuildID() );
		if( houseClass == NULL )
			return 0;

		GuildHouseBuildingStruct* BuildingInfo = houseClass->GetBuildingInfo( BuildingRole->TempData.GuildBuildingDBID );
		if( BuildingInfo == NULL )
			return 0;

		switch( Type )
		{
		case EM_GuildBuildValueType_GuildID:
			return BuildingInfo->Info.GuildID;
		case EM_GuildBuildValueType_BuildingDBID:
			return BuildingInfo->Info.BuildingDBID;
		case EM_GuildBuildValueType_ParentDBID:
			return BuildingInfo->Info.ParentDBID;
		case EM_GuildBuildValueType_BuildingID:
			return BuildingInfo->Info.BuildingID;
		case EM_GuildBuildValueType_IsActive:
			return BuildingInfo->Info.IsActive;
		case EM_GuildBuildValueType_Value1:
			return BuildingInfo->Info.Value1;
		case EM_GuildBuildValueType_Value2:
			return BuildingInfo->Info.Value2;
		case EM_GuildBuildValueType_Value3:
			return BuildingInfo->Info.Value3;
		}

		return 0;
	}
	bool	WriteGuildBuilding( int ItemGUID , int Type , int Value )
	{
		RoleDataEx*     BuildingRole = Global::GetRoleDataByGUID( ItemGUID );		
		if( BuildingRole == NULL )
			return false;

		if( BuildingRole->TempData.GuildBuildingType != EM_GuildBuildingType_Building  )
			return false;

		GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj( BuildingRole->GuildID() );
		if( houseClass == NULL )
			return false;

		GuildHouseBuildingStruct* BuildingInfo = houseClass->GetBuildingInfo( BuildingRole->TempData.GuildBuildingDBID );
		if( BuildingInfo == NULL )
			return false;

		if( BuildingInfo->Info.State == EM_GuildHouseBuildingInfoState_None )
			BuildingInfo->Info.State = EM_GuildHouseBuildingInfoState_Update;
		houseClass->NeedSave();


		switch( Type )
		{
		case EM_GuildBuildValueType_IsActive:
			BuildingInfo->Info.IsActive = (Value!=0);
			return true;
		case EM_GuildBuildValueType_Value1:
			BuildingInfo->Info.Value1 = Value;
			return true;
		case EM_GuildBuildValueType_Value2:
			BuildingInfo->Info.Value2 = Value;
			return true;
		case EM_GuildBuildValueType_Value3:
			BuildingInfo->Info.Value3 = Value;
			return true;
		}

		return false;
	}
	bool	AddGuildResource	( int PlayerGUID , int Gold , int BonusGold , int Mine , int Wood , int Herb , int GuildRune , int GuildStone , const char* RetRunplot )
	{
		RoleDataEx*     Role = Global::GetRoleDataByGUID( PlayerGUID );		
		if( Role == NULL )
			return false;

		GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember( Role->DBID() );
		if( GuildMember == NULL )
			return false;

		GuildResourceStruct Resource;
		Resource.Gold		= Gold;
		Resource.BonusGold	= BonusGold;
		Resource.Mine		= Mine;
		Resource.Wood		= Wood;
		Resource.Herb		= Herb;
		Resource.GuildRune	= GuildRune;
		Resource.GuildStone = GuildStone;

		if( Resource.IsEmpty() != false )
			return true;

		NetSrv_Guild::SD_AddGuildResource( Role->GuildID() , Role->DBID() , Resource , RetRunplot , EM_AddGuildResourceType_Plot , 0 );

		return true;
	}
	//--------------------------------------------------------------------------------
	//顯示Client端的時間
/*
	bool	ClientClock( int GItemID ,  int BeginTime , int NowTime , int EndTime , int Type )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL || Role->IsPlayer() == false )
			return false;

		NetSrv_Other::SC_ClientClock( GItemID , BeginTime , NowTime , EndTime , Type );
		return true;
	}
	*/
	bool	ClockOpen( int GItemID, int CheckValue , int BeginTime , int NowTime , int EndTime , const char* ChangeZonePlot , const char* TimeUpPlot )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL || Role->IsPlayer() == false )
			return false;

		
		if( Role->PlayerTempData->LuaClock.Type != EM_LuaPlotClockType_Close )
			return false;

		Role->PlayerTempData->LuaClock.Type = EM_LuaPlotClockType_Open;
		Role->PlayerTempData->LuaClock.CheckValue = CheckValue;		
		Role->PlayerTempData->LuaClock.BeginTime = BeginTime;
		Role->PlayerTempData->LuaClock.EndTime = EndTime;
		Role->PlayerTempData->LuaClock.NowTime = NowTime;
		Role->PlayerTempData->LuaClock.ChangeZonePlot = ChangeZonePlot;
		Role->PlayerTempData->LuaClock.TimeUpPlot = TimeUpPlot;

		NetSrv_Other::SC_ClientClock( GItemID , BeginTime , NowTime , EndTime , EM_LuaPlotClockType_Open  );
		return true;

	}

	bool    ClockStop( int GItemID )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL || Role->IsPlayer() == false )
			return false;


		if( Role->PlayerTempData->LuaClock.Type != EM_LuaPlotClockType_Open )
			return false;

		Role->PlayerTempData->LuaClock.Type = EM_LuaPlotClockType_Stop;

		int BeginTime = Role->PlayerTempData->LuaClock.BeginTime;
		int NowTime = Role->PlayerTempData->LuaClock.NowTime;
		int EndTime = Role->PlayerTempData->LuaClock.EndTime; 

		NetSrv_Other::SC_ClientClock( GItemID , BeginTime , NowTime , EndTime , EM_LuaPlotClockType_Stop  );
		return  true;
	}
	bool    ClockClose( int GItemID )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL || Role->IsPlayer() == false )
			return false;


		if( Role->PlayerTempData->LuaClock.Type == EM_LuaPlotClockType_Close )
			return false;

		Role->PlayerTempData->LuaClock.Type = EM_LuaPlotClockType_Close;


		NetSrv_Other::SC_ClientClock( GItemID , 0 , 0 , 0 , EM_LuaPlotClockType_Close );
		return  true;

	}
	enum ClockValueTypeENUM
	{
		EM_ClockValueType_CheckValue ,
		EM_ClockValueType_BeginTime  ,
		EM_ClockValueType_NowTime	 ,
		EM_ClockValueType_EndTime    ,
		EM_ClockValueType_Type		 ,
	};
	int		ClockRead( int GItemID , int Type )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL || Role->IsPlayer() == false )
			return 0;

		switch( Type )
		{
		case EM_ClockValueType_CheckValue:
			return Role->PlayerTempData->LuaClock.CheckValue;
		case EM_ClockValueType_BeginTime:
			return Role->PlayerTempData->LuaClock.BeginTime;
		case EM_ClockValueType_NowTime:
			return Role->PlayerTempData->LuaClock.NowTime;
		case EM_ClockValueType_EndTime:
			return Role->PlayerTempData->LuaClock.EndTime;
		case EM_ClockValueType_Type:
			return Role->PlayerTempData->LuaClock.Type;
		}

		return 0;
	}

	bool	SysCastSpellLv_Pos	( int RoleID, float X , float Y , float Z , int MagicID , int MagicLv )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( RoleID );
		if( Role == NULL )
			return false;

		MagicProcInfo  MagicInfo;
		MagicInfo.Init();
		MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
		MagicInfo.TargetX			= X;
		MagicInfo.TargetY			= Y;
		MagicInfo.TargetZ			= Z;
		MagicInfo.TargetID			= RoleID;				//目標
		MagicInfo.MagicCollectID	= MagicID;			//法術
		MagicInfo.MagicLv			= MagicLv;
		bool Ret = MagicProcessClass::SysSpellMagic( Role , &MagicInfo );
		return Ret;
	}
	bool	SysCastSpellLv	( int RoleID, int TargetID, int MagicID , int MagicLv )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( RoleID );
		if( Role == NULL )
			return false;

		RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );
		if( Target != NULL && Target != Role )
		{
			Role->BaseData.Pos.Dir = Role->CalDir( Target );
		}

		MagicProcInfo  MagicInfo;
		MagicInfo.Init();
		MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
		MagicInfo.TargetID			= TargetID;				//目標
		MagicInfo.MagicCollectID	= MagicID;			//法術
		MagicInfo.MagicLv			= MagicLv;
		bool Ret = MagicProcessClass::SysSpellMagic( Role , &MagicInfo );
		return Ret;
	}
	//--------------------------------------------------------------------------------
	bool	InterruptMagic	( int GItemID )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;

		if( Role->IsSpell() == false )
			return false;

		Role->TempData.Magic.State = EM_MAGIC_PROC_STATE_INTERRUPT;
		return true;
	}

	const char* GetGuildName	( int GuildID )
	{
		GuildStruct* Guild = GuildManageClass::This()->GetGuildInfo( GuildID );
		if( Guild == NULL )
			return "";

		return Guild->Base.GuildName.Begin();
	}

	//檢查是否在自己的公會城內
	bool	CheckInMyGuildHouse( int PlayerGUID )
	{

		RoleDataEx* Role = Global::GetRoleDataByGUID( PlayerGUID );
		if( Role == NULL )
			return false;

		GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember( Role->DBID() );
		if( GuildMember == false )
			return false;

		GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj_ByRoom( Role->RoomID() );
		if( houseClass == NULL )
			return false;

		if( houseClass->GetHouseBase()->GuildID == Role->GuildID() )
			return true;

		return false;
	}

	//產生寵物物品
	bool	GiveItem_Pet( int GItemID , int ItemOrgID , int Lv , int Range , int MaxTotalPoint , int NpcOrgID )
	{
		PlotRegStruct* T = LUA_VMClass::ProcReg();
		RoleDataEx* NPC  = NULL;
		if( T != NULL )
		{
			NPC = Global::GetRoleDataByGUID( T->OwnerID );
			if( NPC == NULL || NPC->IsPlayer() )
			{
				NPC = Global::GetRoleDataByGUID( T->TargetID );
			}
		}

		RoleDataEx* Target  = Global::GetRoleDataByGUID( GItemID );
		if( Target == NULL )
		{
			Print( LV3 , "GiveBodyItem_EQ" , "Target == NULL ItemID=%d OrgID=%d",GItemID, ItemOrgID );
			return false;
		}

		GameObjDbStructEx* OrgDB = Global::GetObjDB( ItemOrgID );

		if( OrgDB == NULL )
			return false;

		GameObjDbStructEx* NpcDB = Global::GetObjDB( NpcOrgID );
		if( NpcDB->IsNPC() == false )
			return false;

		if( OrgDB->IsItem() == false  || OrgDB->Item.ItemType != EM_ItemType_Pet )
			return false;


		ItemFieldStruct	Item;
		if( Global::NewItemInit( Item , ItemOrgID , 0 ) == false )
			return false;

		Item.Count			= 1;
		Item.OrgObjID		= ItemOrgID;
		Item.Pet.CreateTime = RoleDataEx::G_Now;
		Item.Pet.PetOrgID	= NpcOrgID;//OrgDB->Item.Pet.PetObjID;
		Item.Pet.SkillPoint = OrgDB->Item.Pet.SkillPoint;
		Item.Pet.PetType	= OrgDB->Item.Pet.PetType;
		Item.Pet.Property	= OrgDB->Item.Pet.Property;
		Item.Pet.Lv			= Lv;
		Item.Pet.Talent		= unsigned short ( ( OrgDB->Item.Pet.Talent ) * ( 1 - rand()*0.2f / RAND_MAX ) );
		//Item.Pet.Spirit		= OrgDB->Item.Pet.Spirit;

		Item.Pet.STR		= int( OrgDB->Item.Pet.STR * 3 / 6 * ( 0.6+0.2f* RandFloat() ) * (Lv -1) + OrgDB->Item.Pet.STR );
		Item.Pet.STA		= int( OrgDB->Item.Pet.STA * 3 / 6 * ( 0.6+0.2f* RandFloat() ) * (Lv -1) + OrgDB->Item.Pet.STA );
		Item.Pet.INT		= int( OrgDB->Item.Pet.INT * 3 / 6 * ( 0.6+0.2f* RandFloat() ) * (Lv -1) + OrgDB->Item.Pet.INT );
		Item.Pet.MND		= int( OrgDB->Item.Pet.MND * 3 / 6 * ( 0.6+0.2f* RandFloat() ) * (Lv -1) + OrgDB->Item.Pet.MND );
		Item.Pet.AGI		= int( OrgDB->Item.Pet.AGI * 3 / 6 * ( 0.6+0.2f* RandFloat() ) * (Lv -1) + OrgDB->Item.Pet.AGI );

		Range++;
		if( Range <= 1 )
			Range = 1;
		int DStr = rand() % Range;
		int DSta = rand() % Range;
		int DInt = rand() % Range;
		int DMnd = rand() % Range;
		int DAgi = rand() % Range;

		int DTotalPoint =  DStr + DSta + DInt + DMnd + DAgi - MaxTotalPoint;

		for( int i = 0 ; i < DTotalPoint ; i++  )
		{
			switch( rand() % 5 )
			{
			case 0: DStr--;break;
			case 1: DSta--;break;
			case 2: DInt--;break;
			case 3: DMnd--;break;
			case 4: DAgi--;break;
			}
		}
		Item.Pet.STR += DStr;
		Item.Pet.STA += DSta;
		Item.Pet.INT += DInt;
		Item.Pet.MND += DMnd;
		Item.Pet.AGI += DAgi;

		if( Item.Pet.STR > 30000 )
			Item.Pet.STR = 0;
		if( Item.Pet.STA > 30000 )
			Item.Pet.STA = 0;
		if( Item.Pet.INT > 30000 )
			Item.Pet.INT = 0;
		if( Item.Pet.MND > 30000 )
			Item.Pet.MND = 0;
		if( Item.Pet.AGI > 30000 )
			Item.Pet.AGI = 0;


		if( Target->GiveItem( Item , EM_ActionType_PlotGive , NPC , "" ) == false )
			return false;

		return true;
	}
	//設定座騎顏色
	bool	SetHourseColor( int GItemID , int  Color1 , int Color2 , int Color3 , int Color4 )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;

		Role->BaseData.HorseColor[0] = Color1;
		Role->BaseData.HorseColor[1] = Color2;
		Role->BaseData.HorseColor[2] = Color3;
		Role->BaseData.HorseColor[3] = Color4;
		NetSrv_MoveChild::SendRangeHorseColor( Role );

		return true;
	}

	//交換裝備
	bool	SwapEQ( int GItemID , int posID )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL || Role->IsPlayer() == false )
			return false;

		Role->SwapEQ( posID );
		NetSrv_EQ::S_SwapBackupEQ( Role->GUID() , posID , true );
		return true;
	}

	//修改Buff資訊
	bool	ModifyBuff( int GItemID , int BuffID , int Power , int Time )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL  )
			return false;

		for( int i = 0 ; i < Role->BaseData.Buff.Size() ; i++ )
		{
			BuffBaseStruct& Buff = Role->BaseData.Buff[i];
			if( BuffID == Buff.BuffID )
			{
				Buff.Power = Power;
				Buff.Time = Time;

				NetSrv_MagicChild::SendRange_ModifyBuffInfo( Role->GUID() , Buff.OwnerID ,  BuffID , Power , Time );
				Role->TempData.UpdateInfo.Recalculate_Buff = true;

				return true;
			}
		}

		return false;
	}
	bool	CancelBuff_Pos( int GItemID , int Pos )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL  )
			return false;

		Role->ClearBuffByPos( Pos );
		return true;
	}

	//刪除職業
	bool	DelJob( int GItemID , int Job )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL  || Role->IsPlayer() == false )
			return false;
	
		if( unsigned(Job) >= EM_Max_Vocation )
			return false;
		
		if( Role->TempData.Attr.Voc == Job )
			return false;

		if( Role->BaseData.Voc_Sub == Job )
			Role->BaseData.Voc_Sub = EM_Vocation_None;
		memset( &(Role->PlayerBaseData->AbilityList[Job]) , 0 , sizeof(BaseAbilityStruct) );
		Role->TempData.UpdateInfo.VocChange = true;

		NetSrv_Other::SC_DelJob( GItemID , (Voc_ENUM)Job );
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// 公會屋戰
	//////////////////////////////////////////////////////////////////////////
	//增減公會戰積分
	bool	AddGuildWarScore( int GItemID , int Score )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;


		GuildHouseWarManageClass* warClass = GuildHouseWarManageClass::GetGuildWar_ByRoomID( Role->RoomID() );
		if( warClass == NULL )
			return false;

		return warClass->AddScore( Role->GuildID() , Score );
	}

	int		GetGuildWarScore( int GItemID )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return 0;


		GuildHouseWarManageClass* warClass = GuildHouseWarManageClass::GetGuildWar_ByRoomID( Role->RoomID() );
		if( warClass == NULL )
			return 0;

		return warClass->WarBase().GetScore( Role->GuildID() );
	}

	bool	AddGuildWarEnergy( int GItemID , int Value )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;


		GuildHouseWarManageClass* warClass = GuildHouseWarManageClass::GetGuildWar_ByRoomID( Role->RoomID() );
		if( warClass == NULL )
			return false;

		return warClass->WarBase().AddEnergy( Role->GuildID() , Value );
	}
	int		GetGuildWarEnergy( int GItemID )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return 0;


		GuildHouseWarManageClass* warClass = GuildHouseWarManageClass::GetGuildWar_ByRoomID( Role->RoomID() );
		if( warClass == NULL )
			return 0;

		return warClass->WarBase().GetEnergy( Role->GuildID() );
	}

	//增加公會戰場個人積分 ( Type = 0 擊破數 1 傷害量 2 防禦量（被傷害） 3 治療量 4 戰績取得 5 能源取得 6 佔領據點 7 總分)
	bool  AddGuildWarPlayerValue( int GItemID , int Type , int Score )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL  || Role->IsPlayer() == false )
			return false;
		GuildWarPlayerInfoStruct& Info = GuildHouseWarManageClass::PlayerScoreMap( )[ Role->DBID() ];
		
		Info.PlayerDBID 	= Role->DBID();
		
		if( unsigned(Type ) >= EM_GuildWarPlayerValue_Max )
			return false;

		Info._Score[ Type ] += Score;

		return true;
	}

	int		GetGuildWarPlayerValue( int GItemID , int Type )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL  || Role->IsPlayer() == false )
			return 0;
		GuildWarPlayerInfoStruct& Info = GuildHouseWarManageClass::PlayerScoreMap( )[ Role->DBID() ];

		Info.PlayerDBID 	= Role->DBID();

		if( unsigned(Type ) >= EM_GuildWarPlayerValue_Max )
			return 0;

		return Info._Score[ Type ];
	}
/*
	bool	AddGuildWarValue( int GItemID , int Type , int Value )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;

		GuildHouseWarManageClass* warClass = GuildHouseWarManageClass::GetGuildWar_ByRoomID( Role->RoomID() );
		if( warClass == NULL )
			return false;

		return warClass->WarBase().AddWarValue( Role->GuildID() , Type , Value );

	}
	int		GetGuildWarValue( int GItemID , int Type )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;

		GuildHouseWarManageClass* warClass = GuildHouseWarManageClass::GetGuildWar_ByRoomID( Role->RoomID() );
		if( warClass == NULL )
			return false;

		return warClass->WarBase().GetWarValue( Role->GuildID() , Type );
	}
*/
	const char* GetGuildName_ByID( int RoomID , int ID )
	{
		if( (unsigned)ID >= 2 )
			return "";

		GuildHouseWarManageClass* warClass = GuildHouseWarManageClass::GetGuildWar_ByRoomID( RoomID );
		if( warClass == NULL )
			return "";
		int GuildID = warClass->WarBase().Base[ID].GuildID;
		GuildHouseWarInfoStruct* Info = BG_GuildWarManageClass::GetGuildHouseWarInfo( GuildID );
		if( Info == NULL )
			return "";
		return Info->GuildName.Begin();
	}

	int		GetGuildWarScore_ByID( int RoomID , int ID )
	{
		if( (unsigned)ID >= 2 )
			return -1;

		GuildHouseWarManageClass* warClass = GuildHouseWarManageClass::GetGuildWar_ByRoomID( RoomID );
		if( warClass == NULL )
			return -1;

		return warClass->WarBase().Base[ID].Score;
	}
	//////////////////////////////////////////////////////////////////////////
	// 拷貝刪除房間
	bool	CopyRoomMonster	( int FromRoomID , int ToRoomID )
	{
		return Global::CopyRoomMonster( FromRoomID , ToRoomID );
	}
	bool	DelRoomMonster	( int RoomID )
	{
		return Global::DelRoomMonster( RoomID );
	}
	//////////////////////////////////////////////////////////////////////////
	//讀取所在位置的地圖id
	int		FindMapID( float X , float Y , float Z )
	{
		return PathManageClass::FindMapID( X , Y , Z );
	}
	//檢查所在位置是否在此地圖
	bool	CheckMapID( float X , float Y , float Z , int MapID )
	{
		return PathManageClass::CheckMapID( X , Y , Z , MapID );
	}
	//////////////////////////////////////////////////////////////////////////
	bool	CheckAttachAble( int WagonItemID )
	{
		BaseItemObject* WagonObj = Global::GetObj( WagonItemID );

		if( WagonObj == NULL )
			return false;

		RoleDataEx* Wagon = WagonObj->Role();

		vector< AttachObjInfoStruct >& AttachList = WagonObj->AttachList();		
		//////////////////////////////////////////////////////////////////////////
		//看是否有空的位置
		int InsertPos = (int)AttachList.size();
		for( int i = 0 ; i < (int)AttachList.size() ; i++ )
		{
			if( AttachList[i].IsEmpty() != false )
			{
				InsertPos = i;
				break;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		int CarryCount = Wagon->CalCarryCount();

		if( Wagon->IsPlayer() )
		{
			if( InsertPos >= CarryCount )
				return false;
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool	CheckAttach( int WagonItemID , int GItemID )
	{
		BaseItemObject* WagonObj = Global::GetObj( WagonItemID );

		if( WagonObj == NULL )
			return false;

		vector< AttachObjInfoStruct >& AttachList = WagonObj->AttachList();		
		for( unsigned i = 0 ; i < AttachList.size() ; i++ )
		{
			if( AttachList[i].ItemID == GItemID )
				return  true;
			return CheckAttach( AttachList[i].ItemID , GItemID );
		}
		return false;
	}

	//上載具
	bool	AttachObj( int GItemID , int WagonItemID , int Type , const char* ItemPos , const char* WagonPos )
	{
		BaseItemObject* WagonObj = Global::GetObj( WagonItemID );
		RoleDataEx*		Role	 = Global::GetRoleDataByGUID( GItemID );
		if( WagonObj == NULL || Role == NULL )
			return false;

		RoleDataEx* Wagon = WagonObj->Role();
		GameObjDbStructEx* WagonObjDB = Global::GetObjDB( Wagon->BaseData.ItemInfo.OrgObjID );
		if( WagonObjDB == NULL )
			return false;
		if(		WagonObjDB->IsNPC()
			&&	Global::CheckLuaScript( WagonItemID , GItemID , WagonObjDB->NPC.CheckCarryLuaScript ) == false )
			return false;

		if( CheckAttach( WagonItemID , GItemID ) != false )
			return false;

		BaseItemObject* OldWagonObj = Global::GetObj( Role->TempData.AttachObjID );
		if( OldWagonObj != NULL )
		{
			vector< AttachObjInfoStruct >& OldAttachList = OldWagonObj->AttachList();
			for( unsigned i = 0 ; i < OldAttachList.size() ; i++ )
			{
				if( OldAttachList[i].ItemID == GItemID )
				{
					OldAttachList[i].Init();
					break;
				}
			}
		}

		vector< AttachObjInfoStruct >& AttachList = WagonObj->AttachList();		
		//////////////////////////////////////////////////////////////////////////
		//看是否有空的位置
		int InsertPos = (int)AttachList.size();
		for( int i = 0 ; i < (int)AttachList.size() ; i++ )
		{
			if( AttachList[i].IsEmpty() != false )
			{
				InsertPos = i;
				break;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		int CarryCount = Wagon->CalCarryCount();

		if( Role->IsPlayer() )
		{
			//if( InsertPos >= AttachList.size() && CarryCount <= AttachList.size() )
			//	return false;
			if( InsertPos >= CarryCount )
				return false;
		}
		

		//如果可控制的第一個可以移動

		//GameObjDbStructEx* WagonObjDB = Global::GetObjDB( Wagon->BaseData.ItemInfo.OrgObjID );
		if(		WagonObjDB->IsNPC()	
			&&	WagonObjDB->NPC.CarryType == EM_NPCCarryType_Control 
			&&	InsertPos == 0 )
		{
			Role->BaseData.Mode.Move	= true;
			Wagon->BaseData.Mode.DisableMovePG = true;
		}
		else
			Role->BaseData.Mode.Move	= false;

		Role->TempData.AttachObjID	= WagonItemID;

	
		AttachObjInfoStruct AttachInfo;
		AttachInfo.ItemID = GItemID;
		AttachInfo.ItemPos	= ItemPos;	//連結位置
		AttachInfo.WagonPos = WagonPos;
		AttachInfo.Type = Type;

		//////////////////////////////////////////////////////////////////////////
		//看是否有空的位置
		bool IsInsert = false;
		for( int i = 0 ; i < (int)AttachList.size() ; i++ )
		{
			if( AttachList[i].IsEmpty() != false )
			{
				AttachList[i] = AttachInfo;
				IsInsert =true;
				Role->TempData.AttachPos = i;
				break;
			}
		}
		if( IsInsert == false )
		{
			Role->TempData.AttachPos = (int)AttachList.size();
			AttachList.push_back( AttachInfo );
		}
		//////////////////////////////////////////////////////////////////////////




		Role->TempData.Move.Tx = Role->BaseData.Pos.X = Wagon->BaseData.Pos.X;
		Role->TempData.Move.Ty = Role->BaseData.Pos.Y = Wagon->BaseData.Pos.Y;
		Role->TempData.Move.Tz =  Role->BaseData.Pos.Z = Wagon->BaseData.Pos.Z;
		NetSrv_MoveChild::MoveObj( Role );

		NetSrv_MoveChild::SendRangeAttackObj( Role , WagonItemID , Type , ItemPos , WagonPos , Role->TempData.AttachPos );
		NetSrv_Move::SC_ColdownInfo( Role->GUID() , WagonItemID , Wagon->BaseData.Coldown );

		//如果載具有劇情觸發劇情
		if( WagonObjDB->IsNPC() != false && strlen( WagonObjDB->NPC.CarryLuaScript ) != 0 )
		{
			char Buf[512];
			sprintf( Buf , "%s(%d,1)" , WagonObjDB->NPC.CarryLuaScript , Role->TempData.AttachPos );
			LUA_VMClass::PCallByStrArg( Buf , Role->GUID() , WagonItemID );
		}

		return true;
	}
	//下載具
	bool	DetachObj( int GItemID )
	{
		RoleDataEx*		Role	 = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;

		BaseItemObject* OldWagonObj = Global::GetObj( Role->TempData.AttachObjID );
		if( OldWagonObj == NULL )
			return false;
		
		vector< AttachObjInfoStruct >& OldAttachList = OldWagonObj->AttachList();
		for( unsigned i = 0 ; i < OldAttachList.size() ; i++ )
		{
			if( OldAttachList[i].ItemID == GItemID )
			{
				//OldAttachList.erase( OldAttachList.begin() + i );
				OldAttachList[i].Init();
				break;
			}
		}

		Role->BaseData.Mode.Move = true;
		Role->TempData.AttachObjID = -1;
		Role->TempData.Move.IsLockSetPos = false;

		if(		Role->IsDestroy( ) == false
			&&	OldWagonObj->Role()->IsDestroy() == false )
		{
			NetSrv_MoveChild::SendRangeDetachObj( Role );
		}

		GameObjDbStructEx* WagonObjDB = Global::GetObjDB( OldWagonObj->Role()->BaseData.ItemInfo.OrgObjID );
		//如果載具有劇情觸發劇情
		if( WagonObjDB->IsNPC() != false && strlen( WagonObjDB->NPC.CarryLuaScript ) != 0 )
		{
			char Buf[512];
			sprintf( Buf , "%s(-1,0)" , WagonObjDB->NPC.CarryLuaScript , Role->TempData.AttachPos );
			LUA_VMClass::PCallByStrArg( Buf , Role->GUID() , OldWagonObj->GUID() );
		}
	
		if(		WagonObjDB->IsNPC() != false 
			&& 	WagonObjDB->NPC.CarryType == EM_NPCCarryType_Control 
			&&  Role->TempData.AttachPos == 0 )
			OldWagonObj->Role()->BaseData.Mode.DisableMovePG = false;


		return true;
	}

	//中斷目前的施法
	bool	MagicInterrupt( int GItemID )
	{
		RoleDataEx*		Role	 = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;

		Role->TempData.Magic.State = EM_MAGIC_PROC_STATE_INTERRUPT;
		NetSrv_MagicChild::CancleSpellMagicRange( Role->GUID() , Role->TempData.Magic.SerialID );
		return true;
	}	
	//////////////////////////////////////////////////////////////////////////
	//重新計算角色數值˙
	bool	ReCalculate( int GItemID )
	{
		RoleDataEx*		Role	 = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;
		Role->OnTimeProc( true );
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	bool	SetSmallGameMenuType	( int GItemID , int MenuID , int Type )
	{
		RoleDataEx*		Role	 = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;
		NetSrv_Other::SC_SetSmallGameMenuType( GItemID , MenuID , (SmallGameMenuTypeENUM)Type );
		return true;
	}
	bool	SetSmallGameMenuValue	( int GItemID , int MenuID , int ID , int Value )
	{
		RoleDataEx*		Role	 = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;
		NetSrv_Other::SC_SetSmallGameMenuValue( GItemID , MenuID , ID , Value );
		return true;
	}
	bool	SetSmallGameMenuStr		( int GItemID , int MenuID , int ID , const char* Content )
	{
		RoleDataEx*		Role	 = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;
		NetSrv_Other::SC_SetSmallGameMenuStr( GItemID , MenuID , ID , Content );
		return true;
	}

	//搜尋可檢的屍體
	int		SearchTreasure			( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return 0;

		if( Owner->IsAttack() != false )
			return 0;
		//取得主人資料
		RoleDataEx* Master = Global::GetRoleDataByGUID( Owner->OwnerGUID() );
		if( Master == NULL )
			return 0;

		float	MinLength = 1000;
		int		TreasureID = 0;

		Global::ResetRange( Owner , _Def_View_Block_Range_ );
		while( 1 )
		{
			RoleDataEx* Treasure = Global::GetRangeRole( );
			if( Treasure == NULL )
				break;

			if(		Treasure == NULL 
				||	Treasure->DynamicData.TreasureBox == NULL
				||	Treasure->BaseData.Mode.Treasure == false 
				||	Treasure->DynamicData.TreasureBox->IsPetLoot == true
				||  Treasure->Length( Owner ) > 300		 )
				continue;

			//檢查寶箱是否有人開啟中
			RoleDataEx* TreasureTarget = Global::GetRoleDataByGUID( Treasure->TempData.Trade.TargetID );
			if( TreasureTarget != NULL)
			{
				if( Treasure->Length( TreasureTarget ) < 100 )
					continue;
				else
					Treasure->TempData.Trade.TargetID = -1;
			}

			if( NetSrv_TreasureChild::_CheckTreasureEmpty( Master , Treasure ) != false )
				continue;

			float TLen = Owner->Length( Treasure );
			if( MinLength > TLen )
			{
				MinLength = TLen;
				TreasureID = Treasure->GUID();
			}
		}
		return TreasureID;
	}

	//檢取屍體
	bool	GetTreasure				( int GItemID , int PetID , int TreasureID )
	{
		BaseItemObject* OwnerObj = Global::GetObj( GItemID );

		if( OwnerObj == NULL )
			return false;

		RoleDataEx* Owner = OwnerObj->Role();
		RoleDataEx* Treasure = Global::GetRoleDataByGUID( TreasureID );
		RoleDataEx* Pet  = Global::GetRoleDataByGUID( PetID );

		if( Owner == NULL || Treasure == NULL || PetID == NULL )
			return false;

		RoleDataEx* TreasureTarget = Global::GetRoleDataByGUID( Treasure->TempData.Trade.TargetID );
		if( TreasureTarget != NULL)
		{
			if( Treasure->Length( TreasureTarget ) < 100 )
				return false;
			else
			{
				Treasure->TempData.Trade.TargetID = -1;
				NetSrv_TreasureChild::SendRange_TreasureLootInfo( Treasure );
			}
		}

		if( NetSrv_TreasureChild::_CheckTreasureEmpty( Owner , Treasure ) != false )
			return false;

		if(		Treasure->BaseData.Mode.Treasure == false 
			||  Treasure->Length( Pet ) > 100 
			||  Treasure->DynamicData.TreasureBox == NULL )
			return false;

		bool IsPartyShareItem = false;
		StaticVector< BaseItemObject* , _MAX_PARTY_COUNT_  > LootObj;	
		int	 RollTypeLV = 0;
		if( Owner->PartyID() != -1 )
		{
			PartyInfoStruct*	Party = PartyInfoListClass::This()->GetPartyInfo( Owner->PartyID() );
			if( Party != NULL )
			{
				RollTypeLV = Party->Info.RollTypeLV;
				for( unsigned int i = 0 ; i < Party->Member.size() ; i++ )
				{
					BaseItemObject* M = Global::GetPlayerObj_ByDBID( Party->Member[i].DBID );

					if( M == NULL )
						continue;

					if( Treasure->Length( M->Role() ) > _MAX_PARTY_SHARE_RANGE_ )
						continue;				

					if( Treasure->RoomID() != M->Role()->RoomID() )
						continue;

					if( M->Role()->DBID() == Owner->DBID() )
						continue;

					LootObj.Push_Back( M );
				}
			}
		}
		LootObj.Push_Back( OwnerObj );


		TreasureBoxStruct& TreasureBox = *(Treasure->DynamicData.TreasureBox);
		TreasureBox.IsPetLoot = true;

		for( int i = 0 ; i < _MAX_BODY_COUNT_ ; i++ )
		{
			int	ItemOwnerDBID = Treasure->DynamicData.TreasureBox->OwnerDBID[ i ];
			ItemFieldStruct& TreasureItem = Treasure->DynamicData.TreasureBox->Item[ i ];

			if( TreasureItem.IsEmpty()	)
				continue;

			GameObjDbStructEx* TreasureItemDB = Global::GetObjDB( TreasureItem.OrgObjID );
			if( TreasureItemDB == NULL ) 
				continue;

			if(		RollTypeLV <= TreasureItemDB->Rare 
				&&	ItemOwnerDBID == -1 )					
				IsPartyShareItem = true;
			else
				IsPartyShareItem = false;

			if( ItemOwnerDBID != -1 )
			{
				if( ItemOwnerDBID != Owner->DBID() )
					continue;
			}
			else
			{
				if( TreasureBox.TreasureOwnerDBID != -1 )
				{
					if( TreasureBox.TreasureOwnerDBID != Owner->DBID() )
						continue;
				}
				else if( TreasureBox.TreasurePartyID != -1 )
				{
					if( TreasureBox.TreasurePartyID != Owner->PartyID() )
						continue;
				}
			}

			if(		TreasureItem.Money != 0 
				&&	TreasureItem.OrgObjID == g_ObjectData->SysValue().Money_GoldObjID )
			{
				int DropMoney = TreasureItem.Money;
				int	ModMoney = TreasureItem.Money % LootObj.Size();
				DropMoney /= LootObj.Size();

				for( int i = 0 ; i < LootObj.Size() ; i++ )
				{
					int Money = DropMoney;
					if( LootObj.Size() - 1 == i )
						Money += ModMoney;
					else
					{
						if( rand() % 2 && ModMoney > 0 )
						{
							Money++;
							ModMoney--;
						}
					}
					LootObj[i]->Role()->AddBodyMoney( DropMoney , NULL , EM_ActionType_KillMonster , true );	
				}
			}
			else if( LootObj.Size() == 1 || IsPartyShareItem == false )
			{
				if( TreasureItemDB->IsItem() && TreasureItemDB->Item.BodyMaxCount != 0 )
				{
					//計算物品數量
					int Count = Owner->CalBodyItemCount( TreasureItemDB->GUID ) + Owner->CalBankItemCount( TreasureItemDB->GUID ) + Owner->CalEqItemCount( TreasureItemDB->GUID ) + Owner->CountItemFromQueue( TreasureItemDB->GUID );
					if( Count >= TreasureItemDB->Item.BodyMaxCount )
					{
						continue;
					}
					if( Count + TreasureItem.Count > TreasureItemDB->Item.BodyMaxCount )
					{
						int giveCount = TreasureItemDB->Item.BodyMaxCount - Count;
						TreasureItem.Count -= giveCount;
						Owner->GiveItem( TreasureItem.OrgObjID , giveCount , EM_ActionType_Loot , Treasure , "" );
						continue;
					}
				}
				
				Owner->GiveItem( TreasureItem , EM_ActionType_Loot , Treasure , "" );
			}
			else
			{
				NetSrv_PartyChild::SendItemToParty( LootObj , TreasureItem );
			}

			TreasureItem.Init();
		}

		NetSrv_TreasureChild::SendRange_TreasureLootInfo( Treasure );
		return true;
	}
	bool	StopMove				( int GItemID , bool Immediately )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;
		if( Immediately != false )
			Owner->StopMoveNow();
		else
			Owner->StopMove();
		return true;
	}
	
	int		PartyInstanceLv			( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return -1;

		PartyInfoStruct*	Party = PartyInfoListClass::This()->GetPartyInfo( Owner->PartyID() );
		if( Party == NULL )
			return -1;

		return Party->Info.InstanceLv;
	}

	//儲存副本進度
	bool	WriteInstanceArg		( int KeyID , int Value )
	{
		int InstanceSaveID = Global::Ini()->InstanceSaveID;
		if( InstanceSaveID == -1 )
			return false;
		RoleDataEx* Owner = Global::GetRoleDataByGUID( OwnerID() );
		if( Owner == NULL )
			return false;

		PrivateRoomInfoStruct& RoomInfo = Global::St()->PrivateRoomInfoList[ Owner->RoomID() ];
		if( RoomInfo.InstanceKeyID == -1 )
		{
			RoomInfo.InstanceKeyID = NetSrv_InstanceChild::_MaxPartyKeyID;
			NetSrv_InstanceChild::_MaxPartyKeyID++;
			if( Owner->PartyID() != -1 )
				NetSrv_InstanceChild::_PartyKeyMap[ Owner->PartyID() ] = RoomInfo.InstanceKeyID;

			BaseItemObject* PlayerObj;
			for( PlayerObj = BaseItemObject::GetByOrder_Player(true) ; PlayerObj != NULL ; PlayerObj = BaseItemObject::GetByOrder_Player() )
			{
				RoleDataEx* Role = PlayerObj->Role();
				if( Role->RoomID() != Owner->RoomID() )
					continue;
				Role->PlayerBaseData->InstanceSetting.KeyID[ InstanceSaveID ]		= RoomInfo.InstanceKeyID;
				Role->PlayerBaseData->InstanceSetting.LiveTime[ InstanceSaveID ]	= NetSrv_InstanceChild::_InstanceLiveTime;
				Role->PlayerBaseData->InstanceSetting.ExtendTime[ InstanceSaveID ]	= NetSrv_InstanceChild::_InstanceLiveTime;
				NetSrv_Instance::SC_ResetTime( Role->GUID() , NetSrv_InstanceChild::_InstanceLiveTime , InstanceSaveID ,RoomInfo.InstanceKeyID , false ) ;
			}

		}
		else
		{
			if( RoomInfo.InstanceKeyID < 0 )
				return false;
		}

	
		map< int , int >& ValueMap = NetSrv_InstanceChild::_PartyKeyValue[ RoomInfo.InstanceKeyID ];
		ValueMap[ KeyID ] = Value;
		

		 //DB存進度資料
		//SELECT         GUID, PartyKey, ZoneID, KeyID, KeyValue
		//	FROM             InstancePlayState
		char SqlCmd[512];
		sprintf( SqlCmd , "DELETE InstancePlayState WHERE PartyKey=%d and ZoneID=%d and KeyID=%d" , RoomInfo.InstanceKeyID , RoleDataEx::G_ZoneID , KeyID );
		Net_DCBase::SqlCommand( RoleDataEx::G_ZoneID , SqlCmd );
		sprintf( SqlCmd , "INSERT InstancePlayState ( PartyKey,ZoneID,KeyID,KeyValue ) VALUES(%d,%d,%d,%d )" , RoomInfo.InstanceKeyID , RoleDataEx::G_ZoneID , KeyID,Value );
		Net_DCBase::SqlCommand( RoleDataEx::G_ZoneID , SqlCmd );


		sprintf( SqlCmd , "UPDATE InstancePlayState SET LiveTime = getdate() + 14 WHERE ZoneID = %d and PartyKey =%d " 
			, RoleDataEx::G_ZoneID , RoomInfo.InstanceKeyID	);
		Net_DCBase::SqlCommand( RoleDataEx::G_ZoneID , SqlCmd );
		return true;
	}
	//讀取副本進度
	int		ReadInstanceArg			( int RoomID, int KeyID )
	{	
		PrivateRoomInfoStruct& RoomInfo = Global::St()->PrivateRoomInfoList[ RoomID ];
		if( RoomInfo.InstanceKeyID <= -1 )
		{
			return 0;
		}

		map< int , int >& ValueMap = NetSrv_InstanceChild::_PartyKeyValue[ RoomInfo.InstanceKeyID ];

		return ValueMap[ KeyID ];

		
	}

	//讀取遊戲時間
	int		GameTime				( )
	{
		return ( RoleDataEx::G_Now +RoleDataEx::G_TimeZone * 60 *60 ) % (60*60*3);
	}

	//////////////////////////////////////////////////////////////////////////
	//讀取帳號旗標
	bool	ReadAccountFlag			( int GItemID , int Pos )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;
		return Role->Base.AccountFlag.GetFlag( Pos );
	}
	//寫入帳號旗標
	bool	WriteAccountFlag		( int GItemID , int Pos , bool Value )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;

		if( Value != false )
			Role->Base.AccountFlag.SetFlagOn( Pos );
		else
			Role->Base.AccountFlag.SetFlagOff( Pos );
		NetSrv_Other::SC_FixAccountFlag( Role );
		return true;
	}

	int	ReadAccountFlagValue			( int GItemID , int Pos , int PosCount )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return 0;
		return Role->Base.AccountFlag.GetFlagValue( Pos , PosCount );
	}

	bool WriteAccountFlagValue			( int GItemID , int Pos , int PosCount , int Value )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
			return false;
		Role->Base.AccountFlag.SetFlagValue( Pos , PosCount , Value );
		NetSrv_Other::SC_FixAccountFlag( Role );
		return true;
	}


	bool	ResetMapMark			( int RoomID )
	{
		return NetSrv_OtherChild::ResetMapMark( RoomID );
	}
	bool	SetMapMark				( int RoomID , int MarkID , float X , float Y , float Z , const char* TipStr , int IconID )
	{
		return NetSrv_OtherChild::SetMapMark( RoomID , MarkID , X , Y , Z , TipStr , IconID );
	}
	bool	ClsMapMark				( int RoomID , int MarkID )
	{
		return NetSrv_OtherChild::ClsMapMark( RoomID , MarkID );
	}
	//////////////////////////////////////////////////////////////////////////
	//設定區域變數
	bool	ResetRoomValue			( int RoomID )
	{
		return NetSrv_OtherChild::ResetRoomValue( RoomID );
	}
	bool	SetRoomValue			( int RoomID , int KeyID , int Value )
	{
		return NetSrv_OtherChild::SetRoomValue( RoomID , KeyID , Value );
	}
	int		GetRoomValue			( int RoomID , int KeyID )
	{
		return NetSrv_OtherChild::GetRoomValue( RoomID , KeyID  );
	}
	bool	ClsRoomValue			( int RoomID , int KeyID )
	{
		return NetSrv_OtherChild::ClsRoomValue( RoomID , KeyID );
	}
	//物件的參考點上建物件
	int		CreateObj_ByObjPoint	( int GItemID , int ObjID , const char* Point )
	{
		PlotRegStruct* T = LUA_VMClass::ProcReg();
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return 0;

		float X , Y , Z  , Dir;
		bool Ret = GuildHousesManageClass::GetObjPoint( GItemID , Point , X , Y , Z , Dir );
		if( Ret == false )
			return 0;
		int NewGItemID = Global::CreateObj( ObjID , X , Y , Z , Dir , 1 );
		RoleDataEx* NewRole = Global::GetRoleDataByGUID( NewGItemID );
		if( NewRole == NULL )
			return 0;

		NewRole->BaseData.Mode.Gravity = false;
		NewRole->BaseData.RoomID = Owner->RoomID();

		Print( LV2 , "CreateObj_ByObjPoint" , "OrgObjID=%d roomid=%d , Lua=%s" , ObjID , NewRole->RoomID() , T->FuncName );
		return NewGItemID;
	}
	//排序資料
	void	SortList_Reset()
	{
		SortList.clear();
	}
	void	SortList_Push( int KeyID , int SortValue )
	{
		BaseSortStruct SortBase;
		SortBase.Value = SortValue;
		SortBase.Data = (void*)KeyID;
		SortList.push_back( SortBase );
	}

	int		SortList_Sort()
	{
		sort( SortList.begin() , SortList.end() );
		return (int)SortList.size();
	}

	int		SortList_Get()
	{
		if( SortList.size()	== 0 )
			return 0;
		int RetValue = (int)SortList.back().Data;
		SortList.pop_back();
		return RetValue;
	}

	//////////////////////////////////////////////////////////////////////////
	//企劃設定暫時可施展的法術
	bool	SetExSkill( int GItemID , int SpellerID , int SkillID , int SkillLv )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		StaticVector< ExSkillBaseStruct , _MAX_EX_SKILL_COUNT_ >&	ExSkill = Owner->PlayerTempData->ExSkill; //劇情設定額外可以使用的法術
		if( ExSkill.Size() >= _MAX_EX_SKILL_COUNT_ )
			return false;

		//找是否有重複的資料
		for( int i = 0 ; i < ExSkill.Size() ; i++ )
		{
			if(		ExSkill[i].SpellerID == SpellerID 
				&&	ExSkill[i].SkillID == SkillID )
				return false;
		}

		ExSkillBaseStruct Temp;
		Temp.SpellerID = SpellerID;
		Temp.SkillID = SkillID;
		Temp.SkillLv = SkillLv;
		ExSkill.Push_Back( Temp );
		NetSrv_Other::SC_AddExSkill( GItemID , Temp );
		return true;

	}
	bool	DelExSkill( int GItemID , int SpellerID , int SkillID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		StaticVector< ExSkillBaseStruct , _MAX_EX_SKILL_COUNT_ >&	ExSkill = Owner->PlayerTempData->ExSkill; //劇情設定額外可以使用的法術

		bool Ret = false;
		//找是否有重複的資料
		for( int i = 0 ; i < ExSkill.Size() ; i++ )
		{
			if(		( ExSkill[i].SpellerID == SpellerID || SpellerID == -1 )
				&&	( ExSkill[i].SkillID == SkillID || SkillID == -1 )			)
			{
				NetSrv_Other::SC_DelExSkill( GItemID , ExSkill[i] );
				ExSkill.Erase( i );
				i--;
				Ret = true;
			}
		}

		return Ret;
	}
	//通知Client 公會戰最後結果
	void	SendGuildWarFinalScore  ( int RoomID , int PlayerDBID , int PrizeType , int	Score , int OrderID )
	{
		RoleData* Owner = Global::GetRoleDataByDBID( PlayerDBID );
		if( Owner == NULL )
			return;

		NetSrv_BG_GuildWar::SC_PlayerFinalInfo( RoomID , PlayerDBID , PrizeType , Score , OrderID , Owner->BaseData.Voc , Owner->BaseData.Voc_Sub , Owner->TempData.Attr.Level , Owner->TempData.Attr.Level_Sub );
	}
	//////////////////////////////////////////////////////////////////////////
	//進入世界
	bool	EnterWorld( int GItemID , int WorldID , int ZoneID , int RoomID , float X , float Y , float Z , float Dir )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false ;

		Global::ChangeWorld( GItemID , Owner , RoleDataEx::G_ZoneID , *(Owner->Pos()) , WorldID , ZoneID , RoomID , X , Y , Z , Dir );
		return true;
	}
	//離開某世界
	bool	LeaveWorld( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false ;

		NetSrv_CliConnectChild::WorldReturnNotifyProc( Owner->GUID() );
		Owner->LiveTime(10*1000 , "Return World"  );

		return true;
	}

	//排行榜
	bool	Billboard_Update		( int GItemID , int SortType , int Value )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return false;

		if( Owner->PlayerBaseData->ManageLV != EM_Management_Nornal )
			return false;

		NetSrv_Billboard::SD_New_Register_Update( Owner->DBID() , SortType , Value );
		return true;
	}
	bool	Billboard_TopUpdate		( int GItemID , int SortType , int Value )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return false;

		if( Owner->PlayerBaseData->ManageLV != EM_Management_Nornal )
			return false;

		NetSrv_Billboard::SD_New_Register_TopUpdate( Owner->DBID() , SortType , Value );
		return true;
	}

	bool	Billboard_Add			( int GItemID , int SortType , int Value )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return false;

		if( Owner->PlayerBaseData->ManageLV != EM_Management_Nornal )
			return false;

		NetSrv_Billboard::SD_New_Register_Add( Owner->DBID() , SortType , Value );

		return true;
	}
	//開啟兌換魔石的介面
	bool	OpenMagicStoneShop		( int MagicStoneID , int Lv , int Money1 , int Money2  )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		Shop  = Global::GetRoleDataByGUID( T->TargetID );
		if( Player == NULL || Shop == NULL )
		{
			Print( LV2 , "OpenMagicStoneShop" , "Shop == NULL || Shop->IsPlayer() Func = %d" , T->FuncName );
			return false;
		}

		if(		Player->TempData.ShopInfo.TargetID		== Shop->GUID() 
			&&	Player->TempData.Attr.Action.OpenType	== EM_RoleOpenMenuType_MagicStoneExchange )
			return false;

		if( Player->TempData.Attr.Action.OpenType != EM_RoleOpenMenuType_MagicStoneExchange && Player->CheckOpenSomething() != false )
			return false;

		//金錢檢查
//		if(		Global::CheckMoneyBase( Player , g_ObjectData->SysValue().MagicStoneShopCostType[0] , Money1 ) == false 
//			||	Global::CheckMoneyBase( Player , g_ObjectData->SysValue().MagicStoneShopCostType[1] , Money2 ) == false  )
//			return false;


		Player->PlayerTempData->MagicStoneSopeInfo.MagicStoneID = MagicStoneID;
		Player->PlayerTempData->MagicStoneSopeInfo.Lv = Lv;
		Player->PlayerTempData->MagicStoneSopeInfo.Money[0] = Money1;
		Player->PlayerTempData->MagicStoneSopeInfo.Money[1] = Money2;

		Player->TempData.Attr.Action.OpenType	 = EM_RoleOpenMenuType_MagicStoneExchange;
		Player->TempData.ShopInfo.TargetID	= Shop->GUID();
		Player->PlayerTempData->Log.ActionMode.Shop	 = true;
	
		NetSrv_Item::SC_OpenMagicStoneExchange( Player->GUID() , MagicStoneID , Lv , Money1 , Money2 );

		return true;
	}

	//設定縮放大小
	bool	SetModelScale			( int GItemID , float Scale )
	{
		RoleDataEx*     Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;
		Owner->TempData.ModelRate = Scale;
		NetSrv_MoveChild::SendRange_ModelRate( Owner->GUID() );
		return true;
	}

	//lua 觸發Client事件
	void ClientEvent		( int SendID , int GItemID , int EventType , const char* Content )
	{
		NetSrv_Other::SC_LuaClientEvent( SendID , GItemID , EventType , Content ); 
	}
	void ClientEvent_Range	( int GItemID , int EventType , const char* Content )
	{
		NetSrv_Other::SC_Range_LuaClientEvent(  GItemID , EventType , Content ); 
	}

	//女僕數值
	int		ReadServantValue		( int ServantID , int Type )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( OwnerID() );
		if( Owner == NULL )
			return 0;

		HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Owner->PlayerTempData->VisitHouseDBID );
		if( houseClass == NULL )
			return 0;

//		if( Owner->PlayerTempData->VisitHouseDBID != Owner->PlayerBaseData->HouseDBID )
//			IsVisitor = true;

		if( (unsigned)ServantID >= MAX_SERVANT_COUNT )
			return 0;

		HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[ ServantID ];

		if( HouseServantInfo.NPCObjID <= 0 )
			return 0;

		return HouseServantInfo.ReadValue( (HouseServantValueTypeENUM)Type );
	}
	bool	WriteServantValue		( int ServantID , int Type , int Value )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( OwnerID() );
		if( Owner == NULL )
			return false;

		HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Owner->PlayerTempData->VisitHouseDBID );
		if( houseClass == NULL )
			return false;

		if( Owner->PlayerTempData->VisitHouseDBID != Owner->PlayerBaseData->HouseDBID )
			return false;

		if( (unsigned)ServantID >= MAX_SERVANT_COUNT )
			return false;

		HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[ ServantID ];

		if( HouseServantInfo.NPCObjID <= 0 )
			return false;		

		if( HouseServantInfo.WriteValue( (HouseServantValueTypeENUM)Type , Value ) )
		{
			houseClass->SetSaveInfo();
			NetSrv_Houses::SC_FixServantValue( Owner->RoomID() , ServantID , (HouseServantValueTypeENUM)Type , Value );
		}
		return false;
	}

	bool		ReadServantFlag		( int ServantID , int Index )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( OwnerID() );
		if( Owner == NULL )
			return 0;

		HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Owner->PlayerTempData->VisitHouseDBID );
		if( houseClass == NULL )
			return 0;

		if( (unsigned)ServantID >= MAX_SERVANT_COUNT )
			return 0;

		HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[ ServantID ];

		if( HouseServantInfo.NPCObjID <= 0 )
			return 0;

		return HouseServantInfo.Flag.GetFlag( Index );
		//return HouseServantInfo.ReadValue( (HouseServantValueTypeENUM)Type );
	}
	bool	WriteServantFlag		( int ServantID , int Index , bool Result )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( OwnerID() );
		if( Owner == NULL )
			return false;

		HousesManageClass* houseClass = HousesManageClass::GetHouseObj( Owner->PlayerTempData->VisitHouseDBID );
		if( houseClass == NULL )
			return false;

		if( Owner->PlayerTempData->VisitHouseDBID != Owner->PlayerBaseData->HouseDBID )
			return false;

		if( (unsigned)ServantID >= MAX_SERVANT_COUNT )
			return false;

		HouseServantStruct& HouseServantInfo = houseClass->GetHouseBase()->Info.Servant[ ServantID ];

		if( HouseServantInfo.NPCObjID <= 0 )
			return false;		

		if( Result != false )
			HouseServantInfo.Flag.SetFlagOn( Index );
		else
			HouseServantInfo.Flag.SetFlagOff( Index );
		return true;
	}

	//取得DB設定的keyvalue
	int		GetDBKeyValue			( const char* KeyStr )
	{
		return NetSrv_OtherChild::GetDBKeyValue( (char*)KeyStr );
	}

	//////////////////////////////////////////////////////////////////////////
	//檢查人物是否在線上
	bool	CheckOnlinePlayer		( int DBID )
	{
		OnlinePlayerInfoStruct* Info = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( DBID );
		if( Info == NULL )
			return false;
		return true;
	}

	//重新計算寵物等級
	bool		ReCalPetLv	( int GItemID , int Pos )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL || Owner->IsPlayer() == false )
			return false;
		NetSrv_CultivatePetChild::ReCalPetLv( Owner , Pos );
		return true;
	}
	//取得目前系統時間 19000年到目前過了幾秒
	int			GetNowTime()
	{
		return RoleDataEx::G_Now + RoleDataEx::G_TimeZone * 60 *60;
	}

	//DBID 轉 GUID
/*	int		PlayerDBIDtoGUID		( int DBID )
	{
		BaseItemObject* obj = BaseItemObject::GetObjByDBID( DBID );
		if( obj == NULL )
			return -1;

		return obj->Role()->GUID();
	}*/
	//鎖定血量 0 以下表示不鎖定
	bool	LockHP( int GItemID , int HP , const char* LuaEvent )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;
		Owner->TempData.LockHP = HP;
		Owner->TempData.LockHPEvent = LuaEvent;
		return true;
	}
	//特殊移動
	bool	SpecialMove( int GItemID , float X , float Y , float Z , int Type )
	{
		BaseItemObject* OwnerObj = BaseItemObject::GetObj( GItemID );
		if( OwnerObj == NULL )
			return false;
		RoleDataEx* Owner = OwnerObj->Role();

		if( Owner->IsNPC() )
		{
			int Len = int( Owner->Length3D( X , Y , Z ) );
			int MoveTime = 3 + Len / 10;
			Owner->TempData.Move.AddeDelay += MoveTime;
		}

		Owner->TempData.Move.Tx = Owner->BaseData.Pos.X = X;
		Owner->TempData.Move.Ty = Owner->BaseData.Pos.Y = Y;
		Owner->TempData.Move.Tz = Owner->BaseData.Pos.Z = Z;

		Owner->TempData.Move.Dx = 0;
		Owner->TempData.Move.Dz = 0;
		Owner->TempData.Move.Dy = 0;

		Owner->TempData.Move.LockSetPosX = Owner->BaseData.Pos.X;
		Owner->TempData.Move.LockSetPosZ = Owner->BaseData.Pos.Z;
		Owner->TempData.Move.IsLockSetPos = true;

		OwnerObj->Path()->SetPos( Owner->X() , Owner->Y() , Owner->Z() );
		NetSrv_MoveChild::SendRange_SpecialMove( 0 , (SpecialMoveTypeENUM)Type , Owner , 0 );
		return true;

	}

	//設定法術命中目標
	bool	PushMagicExplodeTarget( int TargetID )
	{
		RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );
		if( Target == NULL )
			return false;
		MagicTargetInfoStruct MTarget;
		MTarget.Role = Target;
		MagicProcessClass::m_TargetList.push_back( MTarget );
		return true;
	}

	bool	PushMagicShootTarget( int TargetID )
	{
		RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );
		if( Target == NULL )
			return false;

		MagicProcessClass::m_ShootTarget.push_back( TargetID );
		return true;
	}
	//要求其他區域跑PCall
	bool	ZonePCall( int zoneID , const char* plotCmd )
	{
		return NetSrv_Other::SL_PCall( zoneID , plotCmd );
	}

#define Def_InitBgIndependence_RoleLv1			0x01	//數值清空，等同新創角色
#define Def_InitBgIndependence_MaxWeaponSkill	0x02	//自動提升熟練度至最高
#define Def_InitBgIndependence_MaxMagicSkill	0x04	//技能等級永遠保持在最高狀態
#define Def_InitBgIndependence_TitleClear		0x08	//清空頭銜
#define Def_InitBgIndependence_KeyItemClear		0x10	//清空重要物品
#define Def_InitBgIndependence_PetClear			0x20	//清空寵物資訊
#define Def_InitBgIndependence_QuestClear		0x40	//清空任務
#define Def_InitBgIndependence_BuffClear		0x80	//清空Buff
	//獨立遊戲 戰場									
	bool	InitBgIndependence( int GItemID , int Mode )
	{
		if( Global::Ini()->BG_Independence_Game == false )
		{
			Print( LV2 , "InitBgIndependence" , "Warning Global::Ini()->BG_Independence_Game == false");
			return false;
		}
		RoleDataEx* Role = Global::GetRoleDataByGUID( GItemID );
		if( Role == NULL )
		{
			Print( LV2 , "InitBgIndependence" , "Warning Role == NULL");
			return false;
		}
		Role->PlayerTempData->SellItemBackup.Clear();
		NetSrv_Shop::S_FixSellItemBackup( Role );

		if( Mode & Def_InitBgIndependence_RoleLv1 )
		{
			for( int i = 0 ; i < EM_Max_Vocation ; i++ )
			{
				BaseAbilityStruct& Ability = Role->PlayerBaseData->AbilityList[ i ];

				if( Ability.Level == 0 )
				{
					memset( &Ability , 0 , sizeof( BaseAbilityStruct ) );
					continue;
				}
				memset( &Ability , 0 , sizeof( BaseAbilityStruct ) );
				Ability.Level = 1;
			}			

			memset( &Role->PlayerBaseData->SuitSkill , 0 , sizeof(Role->PlayerBaseData->SuitSkill) );
			memset( &Role->BaseData.SkillValue , 0 , sizeof(Role->BaseData.SkillValue) );
		}

		if( Mode & Def_InitBgIndependence_MaxWeaponSkill )
		{
			for( int i = 0 ; i< _MAX_SKILL_COUNT_ ; i++ )
			{
				Role->BaseData.SkillValue.Skill[i] = 1000;
			}			
		}

		if( Mode & Def_InitBgIndependence_MaxMagicSkill )
		{
			for( int i = 0 ; i < EM_Max_Vocation ; i++ )
			{
				BaseAbilityStruct& Ability = Role->PlayerBaseData->AbilityList[ i ];
				for( int i = 0 ; i < _MAX_SPSkill_COUNT_ + _MAX_NormalSkill_COUNT_ ; i++ )
				{
					Ability.AllSkillLv[i] = 200;
				}
			}			
		}

		if( Mode & Def_InitBgIndependence_TitleClear )
		{
			Role->BaseData.TitleID = 0;
			memset( &Role->PlayerBaseData->Title , 0 , sizeof(Role->PlayerBaseData->Title) );
			memset( &Role->PlayerBaseData->TitleSys , 0 , sizeof(Role->PlayerBaseData->TitleSys) );
			Role->PlayerBaseData->TitleStr.Clear();
			Role->PlayerBaseData->TitleCount = 0;
		}


		if( Mode & Def_InitBgIndependence_KeyItemClear )
		{
			memset( &Role->BaseData.KeyItem , 0 , sizeof(Role->BaseData.KeyItem) );
		}
		if( Mode & Def_InitBgIndependence_PetClear )
		{
			memset( &Role->PlayerBaseData->Pet , 0 , sizeof(Role->PlayerBaseData->Pet) );
		}
		if( Mode & Def_InitBgIndependence_QuestClear )
		{
			memset( &Role->PlayerBaseData->Quest , 0 , sizeof(Role->PlayerBaseData->Quest) );
		}
		if( Mode & Def_InitBgIndependence_BuffClear )
		{
			Role->BaseData.Buff.Clear();
		}

		//NetSrv_CliConnectChild::SendPlayerDataInfo_ZIP( Role );
		Role->InitCal();
		return true;
	}
		//取得最後執行隱藏的劇情
	const char*	GetLastHidePlotStr( int OwnerID )
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID( OwnerID );
		if( Role == NULL )
			return "";
		return Role->TempData.HideInfo.PlotStr.Begin();

	}
	//////////////////////////////////////////////////////////////////////////
	//線段組檔
	//////////////////////////////////////////////////////////////////////////
	void	ClearLineBlock( int roomID )
	{
		LineBlockManageClass& lbManage = Global::St()->LuaLineBlock[roomID];
		lbManage.Clear();
	}
	void	AddLineBlock( int roomID , int id , float x1 , float y1 , float x2 , float y2 )
	{
		LineBlockManageClass& lbManage = Global::St()->LuaLineBlock[roomID];
		lbManage.AddLine(id , x1 , y1 , x2 , y2 );
	}
	bool	DelLineBlock( int roomID , int id )
	{
		LineBlockManageClass& lbManage = Global::St()->LuaLineBlock[roomID];
		return lbManage.DelLine( id );
	}
	bool	CheckLineBlock( int roomID , float x1 , float y1 , float x2 , float y2 )
	{
		LineBlockManageClass& lbManage = Global::St()->LuaLineBlock[roomID];
		return lbManage.CheckCollion( x1 , y1 , x2 , y2 );
	}

	//////////////////////////////////////////////////////////////////////////
	//重置副本傳送次數
	bool	WeekInstancesReset(  )
	{
		BaseItemObject* obj = BaseItemObject::GetObj( OwnerID() );
		if( obj == NULL )
			return false;

		return NetSrv_OtherChild::WeekInstancesResetProc( obj );
	}
}



