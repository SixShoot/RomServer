#include "NetDC_Houses.h"
//-------------------------------------------------------------------
CNetDC_Houses*	CNetDC_Houses::This = NULL;

//-------------------------------------------------------------------
bool CNetDC_Houses::_Release()
{
	return true;
}
//-------------------------------------------------------------------
bool CNetDC_Houses::_Init()
{
	_Net->RegOnSrvPacketFunction( EM_PG_Houses_LtoD_BuildHouseRequest		, _PG_Houses_LtoD_BuildHouseRequest			);
	_Net->RegOnSrvPacketFunction( EM_PG_Houses_LtoD_FindHouseRequest		, _PG_Houses_LtoD_FindHouseRequest			);
	_Net->RegOnSrvPacketFunction( EM_PG_Houses_LtoD_HouseInfoLoading		, _PG_Houses_LtoD_HouseInfoLoading			);
	_Net->RegOnSrvPacketFunction( EM_PG_Houses_LtoD_SaveHouseBase			, _PG_Houses_LtoD_SaveHouseBase				);
	_Net->RegOnSrvPacketFunction( EM_PG_Houses_LtoD_SaveItem				, _PG_Houses_LtoD_SaveItem					);
	_Net->RegOnSrvPacketFunction( EM_PG_Houses_LtoD_FindHouseRequest_ByName	, _PG_Houses_LtoD_FindHouseRequest_ByName	);

	return true;
}
void CNetDC_Houses::_PG_Houses_LtoD_FindHouseRequest_ByName		( int ServerID , int Size , void* Data )
{
	PG_Houses_LtoD_FindHouseRequest_ByName* PG = ( PG_Houses_LtoD_FindHouseRequest_ByName* )Data;
	This->RL_FindHouseRequest_ByName( ServerID , PG->PlayerDBID , PG->RoleName.Begin() , PG->Password.Begin() , PG->ManageLv );
}

void CNetDC_Houses::_PG_Houses_LtoD_BuildHouseRequest			( int ServerID , int Size , void* Data )	
{
	PG_Houses_LtoD_BuildHouseRequest* PG = ( PG_Houses_LtoD_BuildHouseRequest* )Data;
	This->RL_BuildHouseRequest( ServerID , PG->PlayerDBID , PG->HouseType , PG->Account_ID.Begin() );
}
void CNetDC_Houses::_PG_Houses_LtoD_FindHouseRequest			( int ServerID , int Size , void* Data )	
{
	PG_Houses_LtoD_FindHouseRequest* PG = ( PG_Houses_LtoD_FindHouseRequest* )Data;
	This->RL_FindHouseRequest( ServerID , PG->PlayerDBID , PG->HouseDBID , (char*)PG->Password.Begin() , PG->ManageLv );
}
void CNetDC_Houses::_PG_Houses_LtoD_HouseInfoLoading			( int ServerID , int Size , void* Data )	
{
	PG_Houses_LtoD_HouseInfoLoading* PG = ( PG_Houses_LtoD_HouseInfoLoading* )Data;
	This->RL_HouseInfoLoading( ServerID , PG->HouseDBID );
}
/*
void CNetDC_Houses::_PG_Houses_LtoD_SaveSwapBodyHouseItem		( int ServerID , int Size , void* Data )	
{
	PG_Houses_LtoD_SaveSwapBodyHouseItem* PG = ( PG_Houses_LtoD_SaveSwapBodyHouseItem* )Data;
	This->RL_SaveSwapBodyHouseItem( ServerID , PG->PlayerDBID , PG->BodyPos , PG->BodyItem , PG->HouseItem );
}
*/
void CNetDC_Houses::_PG_Houses_LtoD_SaveHouseBase				( int ServerID , int Size , void* Data )	
{
	PG_Houses_LtoD_SaveHouseBase* PG = ( PG_Houses_LtoD_SaveHouseBase* )Data;
	This->RL_SaveHouseBase( ServerID , PG->PlayerDBID , PG->HouseBaseInfo );
}
void CNetDC_Houses::_PG_Houses_LtoD_SaveItem					( int ServerID , int Size , void* Data )
{
	PG_Houses_LtoD_SaveItem* PG = ( PG_Houses_LtoD_SaveItem* )Data;
	This->RL_SaveItem( ServerID , PG->PlayerDBID , PG->Item );
}
//////////////////////////////////////////////////////////////////////////
void CNetDC_Houses::SL_BuildHouseResult				( int ServerID , int PlayerDBID , int HouseDBID , bool Result )
{
	PG_Houses_DtoL_BuildHouseResult Send;
	Send.PlayerDBID = PlayerDBID;
	Send.HouseDBID = HouseDBID;
	Send.Result = Result;
	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}
void CNetDC_Houses::SL_FindHouseResult				( int ServerID , int PlayerDBID , int HouseDBID , FindHouseResultENUM Result )
{
	PG_Houses_DtoL_FindHouseResult Send;
	Send.PlayerDBID = PlayerDBID;
	Send.HouseDBID = HouseDBID;
	Send.Result = Result;
	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}
void CNetDC_Houses::SL_HouseBaseInfo				( int ServerID , HouseBaseDBStruct& HouseBaseInfo )
{
	PG_Houses_DtoL_HouseBaseInfo Send;
	Send.HouseBaseInfo = HouseBaseInfo;
	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}
void CNetDC_Houses::SL_ItemInfo						( int ServerID , int HouseDBID , int ItemDBID , int ItemCount , HouseItemDBStruct Item[] )
{
	PG_Houses_DtoL_ItemInfo Send;
	Send.HouseDBID = HouseDBID;
	if( ItemCount > _DEF_MAX_HOUSE_FURNITURE + 10  )
		ItemCount = _DEF_MAX_HOUSE_FURNITURE + 10 ;
	Send.ItemCount = ItemCount;
	Send.ParentItemDBID = ItemDBID;
	memcpy( Send.Item , Item , sizeof(Send.Item ) );
	
	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}
void CNetDC_Houses::SL_HouseInfoLoadOK				( int ServerID , int HouseDBID )
{
	PG_Houses_DtoL_HouseInfoLoadOK Send;
	Send.HouseDBID = HouseDBID;
	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}

void CNetDC_Houses::SL_FriendDBID				( int ServerID , int HouseDBID , int FriendDBID )
{
	PG_Houses_DtoL_FriendDBID Send;
	Send.HouseDBID = HouseDBID;
	Send.FriendDBID = FriendDBID;
	Global::SendToSrvBySockID( ServerID  , sizeof( Send ) , &Send );
}