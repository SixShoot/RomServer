#include "../NetWakerGSrvInc.h"
#include "NetSrv_GMToolsChild.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_GMToolsChild::Init()
{
    NetSrv_GMTools::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_GMToolsChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_GMToolsChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_GMTools::_Release();

    delete This;
    This = NULL;

    return true;
    
}


void    NetSrv_GMToolsChild::R_OnGMToolsConnect( int NetID )
{
    //送所有物件資訊到GMTools
    int		i;
    BaseItemObject*		OwnerObj;
    RoleDataEx*		Owner;

    //系統定時處理
    GItemData   IDInfo;
    IDInfo.Zone = RoleDataEx::G_ZoneID;

    for( i = 0 ; i < BaseItemObject::GetmaxZoneID() ; i ++ )
    {
        IDInfo.ID = i;

        OwnerObj = BaseItemObject::GetObj( IDInfo.GUID );

        if( OwnerObj == NULL )
            continue;
        Owner = OwnerObj->Role();
        S_ObjInfo( NetID , Owner );
    }
}

void    NetSrv_GMToolsChild::R_ObjectDataRequest( int NetID , int GUID )
{
    BaseItemObject*		OwnerObj;
    RoleDataEx*		Owner;

    OwnerObj = BaseItemObject::GetObj( GUID );

    if( OwnerObj == NULL )
        return;

    Owner = OwnerObj->Role();

    S_ObjectData( NetID , Owner );


}

void    NetSrv_GMToolsChild::R_ToPlayerMsg( int NetID , BaseItemObject*	Obj , const char* Name , const char* Msg )
{
    NetSrv_Talk::GMMsg( Obj->GUID() , Msg );
}

void    NetSrv_GMToolsChild::R_ToAllPlayerMsg( int NetID , const char* Msg )
{
   //NetSrv_Talk::GMMsg( -1 , Msg );
	NetSrv_Talk::SendRunningMsg_Zone( Msg );
}

void    NetSrv_GMToolsChild::R_GMCommand( int NetID , int PlayerGUID , const char* Account , int ManageLv , const char* Cmd )
{
	if( PlayerGUID == -1 )
	{
		BaseItemObject* PlayerObj;

		for( PlayerObj = BaseItemObject::GetByOrder_Player(true) ; PlayerObj != NULL ; PlayerObj = BaseItemObject::GetByOrder_Player() )
		{
			Global::Log_GMToolsGMCommand(  Account , ManageLv , PlayerObj->Role()->DBID() , Cmd );
			_St->GM.GMCmdProc( PlayerObj , Cmd , Account , (ManagementENUM)ManageLv );
		}

	}
	else
	{
		BaseItemObject*	Obj =	Global::GetObj( PlayerGUID );

		if( Obj == NULL )
			Global::Log_GMToolsGMCommand(  Account , ManageLv , -1 , Cmd );
		else
			Global::Log_GMToolsGMCommand(  Account , ManageLv , Obj->Role()->DBID() , Cmd );

		_St->GM.GMCmdProc( Obj , Cmd , Account , (ManagementENUM)ManageLv );
	}


}

void	NetSrv_GMToolsChild::R_ServerStateRequest( int NetID , ServerStateRequestTypeENUM	RequestType )
{
	char	Buf[8][64];

	int MaxRoomCount = 0;
	int RoomCount = 0;

	vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;

	MaxRoomCount = int( RoomList.size() ) - Ini()->BaseRoomCount;

	for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
	{
		if( RoomList[i].IsActive != false )
			RoomCount++;
	}

	sprintf( Buf[0] , "ParalledID=%d" , RoleDataEx::G_ZoneID / _DEF_ZONE_BASE_COUNT_);

	sprintf( Buf[1] , "ZoneID(%d/%d)"	, RoleDataEx::G_ZoneID % _DEF_ZONE_BASE_COUNT_
										, RoleDataEx::G_ZoneID);

	if( Global::Ini()->IsHouseZone )
	{
		sprintf( Buf[2] , "House Zone" , RoomCount , MaxRoomCount  );
	}
	else if( Global::Ini()->IsPrivateZone )
	{
		switch( Global::Ini()->PrivateZoneType )
		{
		case EM_PrivateZoneType_None:
			sprintf( Buf[2] , "(PZone) Normal"  );
			break;
		case EM_PrivateZoneType_Private:
			sprintf( Buf[2] , "(PZone) Private"  );
			break;
		case EM_PrivateZoneType_Party:
			sprintf( Buf[2] , "(PZone) Party"  );
			break;
		case EM_PrivateZoneType_Raid:			
			sprintf( Buf[2] , "(PZone) Raid"  );
			break;
		case EM_PrivateZoneType_Private_Party:
			sprintf( Buf[2] , "(PZone) Private Party"  );
			break;
		}
	}
	else
	{
		sprintf( Buf[2] , "Normal Zone"  );
	}

	sprintf( Buf[3] , "Room:%d/%d" , RoomCount , MaxRoomCount  );
	sprintf( Buf[4] , "Mirror:%d" , BaseItemObject::GetMirrorCount() );


	switch( RequestType )
	{
	case EM_ServerStateRequestType_Normal:
		S_ServerStateResult( NetID , RequestType , Global::Ini()->ZoneName.c_str() , "Local" , "OK"
			, Buf[0] , Buf[1]
			, Buf[2] , Buf[3]
			, Buf[4] , ""
			, "" , "" );
		break;
	default:
		break;
	}
}