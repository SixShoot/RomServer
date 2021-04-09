#include "NetSrv_ZoneStatus_Child.h"
#include "../Net_ServerList/Net_ServerList_Child.h"
#include <set>
//-------------------------------------------------------------------
bool    CNetSrv_ZoneStatus_Child::Init()
{
	CNetSrv_ZoneStatus_Child::_Init();

	if( m_pThis != NULL)
		return false;

	m_pThis = NEW( CNetSrv_ZoneStatus_Child );

	return true;
}

//-------------------------------------------------------------------
bool    CNetSrv_ZoneStatus_Child::Release()
{
	if( m_pThis == NULL )
		return false;

	CNetSrv_ZoneStatus_Child::_Release();

	delete m_pThis;
	m_pThis = NULL;
	
    return false;
}
//-------------------------------------------------------------------
void CNetSrv_ZoneStatus_Child::On_SetRoleToWorld			( int iServerID, int iClientID, PlayerRoleData* pRole )
{
	if( Global::St()->IsDisableLogin != false )
	{
		SetRoleToWorldResult( iServerID , iClientID , pRole->Base.DBID , EM_SelectRole_Parallel_Zone_Full_ );
		return;
	}

	PlayerRoleData Role = *pRole;

    pRole->TempData.Sys.WorldGUID = -1 ;

	Role.TempData.Attr.Action.Login = true;
	Role.BaseData.SP = Role.BaseData.SP_Sub = 0;

    Global::AddOnLoginPlayer( &Role );
    SetRoleToWorldResult( iServerID , iClientID , pRole->Base.DBID , EM_SelectRole_OK );
}
//-------------------------------------------------------------------

int g_TempZoneItemID = DF_NO_ZONEITEMID;

void CNetSrv_ZoneStatus_Child::On_ZoneSrvRegResult			( int iServerID, EM_ZoneSrvRegResult emRegResult, int iZoneItemID )
{
    switch( emRegResult )
    {
    case EM_ZoneSrvRegResult_OK:
        {
            
            Print( Def_PrintLV1, "On_LocalSrvRegResult" , "Reg Ok!! Got ZoneItemID(%d)",iZoneItemID );
//            if( CNet_ServerList_Child::IsInit() == false )
            {
                //CNet_ServerList_Child::Init( _Net , EM_SERVER_TYPE_LOCAL , _Ini.ZoneID );			
            }        

            if( iZoneItemID != -1)
			{
				g_TempZoneItemID = iZoneItemID;
                BaseItemObject::SetMaxWorldGUID( iZoneItemID * _Def_WorldItemID_Base_ );
			}

            ZoneSrvOkay( iServerID );
			GlobalBase::Net_ServerList->ServerRdy();


        }	
        break;
    case EM_ZoneSrvRegResult_OUT_ZONEITEMID:
    case EM_ZoneSrvRegResult_ZONEID_IN_USE:
    case EM_ZoneSrvRegResult_MasterSrvInitDone:
        {
            Print( LV3 , "On_ZoneSrvRegResult" , "Failed Type = %d" , emRegResult );
            Global::Destory();
        }
        break;
    }

}
//-------------------------------------------------------------------
void		CNetSrv_ZoneStatus_Child::On_RequestZoneSrvReg	( int iServerID )
{
    int                  i;
    set< int >           UsedItemIDSet;
    set< int >::iterator Iter;

    BaseItemObject::GetUsedWorldItemIDBlock( UsedItemIDSet );

    Iter = UsedItemIDSet.begin();
    int Count = (int)UsedItemIDSet.size();

    if( Count == 0 )
    {
        ZoneSrvReg( iServerID, Global::Net_ServerList->m_dwServerLocalID, -1, 0 );
    }
    else
    {
        for( i = 1 ; i <= Count ; i++ , Iter++ )
        {
            ZoneSrvReg( iServerID , RoleDataEx::G_ZoneID , *Iter , ( Count - 1 )  ); // 0 1
        }
    }
}
//-------------------------------------------------------------------
void CNetSrv_ZoneStatus_Child::On_BTIPlayTime( int iServerID, int iRoleDBID, int iPlayerTimeQuota )
{
	// 收到 Moliyo BTI 系統, 有玩家超過, 有納入防沈迷  目前已經玩超過 iHours小時
	BaseItemObject* OwnerObj = BaseItemObject::GetObjByDBID( iRoleDBID );
	if( OwnerObj == NULL )
		return;
	RoleDataEx* Owner = OwnerObj->Role();
	//Owner->PlayerBaseData->PlayTimeQuota = (2 - iHours)*( 60*60 );

	Owner->SetValue( EM_RoleValue_PlayTimeQuota ,  iPlayerTimeQuota );
}

void CNetSrv_ZoneStatus_Child::On_KickZonePlayer			( int iServerID, const char* szAccountName )
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObjByAccountName( szAccountName );

	if( OwnerObj == NULL )
	{
		KickZonePlayerResult( iServerID, RoleDataEx::G_ZoneID, szAccountName, 0 );
		return;
	}
	else
	{
		OwnerObj->Destroy( "On_KickZonePlayer" );
		KickZonePlayerResult( iServerID, RoleDataEx::G_ZoneID, szAccountName, 1 );
	}
	
}