#include "../NetWakerGSrvInc.h"
#include "NetSrv_BillboardChild.h"
#include <string.h>
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_BillboardChild::Init()
{
    NetSrv_Billboard::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_BillboardChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_BillboardChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_Billboard::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
void NetSrv_BillboardChild::R_CloseBillboardRequest( BaseItemObject* PlayerObj )
{
	RoleDataEx* Owner = PlayerObj->Role();

	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_Billboard) != false )
	{
		Owner->TempData.ShopInfo.TargetID = -1;
		Owner->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_Billboard;
		SC_CloseBillboard( Owner->GUID() );
	}
	else
	{
		//Owner->Msg("排行榜沒開啟");
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Shop_BillBoardNoOpen );
	}
}
void NetSrv_BillboardChild::R_RankInfoRequest( BaseItemObject* PlayerObj , BillBoardMainTypeENUM Type , int RankBegin )
{
	RoleDataEx* Owner = PlayerObj->Role();
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_Billboard) != false )
	{
		RoleDataEx* Target = Global::GetRoleDataByGUID( Owner->TempData.ShopInfo.TargetID  );
		if( Target == NULL || Target->Length(Owner) >= 200 )
		{
			SC_CloseBillboard( Owner->GUID() );
		}
		else
			SD_RankInfoRequest( Type , RankBegin , Owner->DBID() , Owner->TempData.Sys.SockID , Owner->TempData.Sys.ProxyID );
	}	
	else
	{
		//Owner->Msg("排行榜沒開啟");
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Shop_BillBoardNoOpen );
	}

}
void NetSrv_BillboardChild::R_SetAnonymous		( BaseItemObject* PlayerObj , bool IsAnonymous )
{
	RoleDataEx* Owner = PlayerObj->Role();
	if( Owner->CheckOpenMenu( EM_RoleOpenMenuType_Billboard) != false )
	{
		if( Owner->PlayerBaseData->IsBillboardAnonymous != IsAnonymous )
		{
			Owner->PlayerBaseData->IsBillboardAnonymous = IsAnonymous;
			SD_SetAnonymous( Owner->DBID() , IsAnonymous );
		}
	}	
	else
	{
		//Owner->Msg("排行榜沒開啟");
		Owner->Net_GameMsgEvent( EM_GameMessageEvent_Shop_BillBoardNoOpen );
	}
}
//-----------------------------------------------------------------
void NetSrv_BillboardChild::RC_New_SearchRank( BaseItemObject* Player , int SortType , int MinRank , int MaxRank )
{
	SD_New_SearchRank( Player->Role()->DBID() , SortType , MinRank , MaxRank );
}
void NetSrv_BillboardChild::RC_New_SearchMyRank( BaseItemObject* Player , int SortType )
{
	SD_New_SearchMyRank( Player->Role()->DBID() , SortType );
}
void NetSrv_BillboardChild::RC_New_RegisterNote( BaseItemObject* Player , int SortType , const char* Note )
{
	SD_New_RegisterNote( Player->Role()->DBID() , SortType , Note );
}