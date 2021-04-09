#include "./NetSrv_Gather.h"
#include "../Net_ServerList/Net_ServerList_Child.h"

#define Register_LuaFunc_NetSrv_Script


//#define Register_LuaFunc_NetSrv_Script
//	LuaRegisterFunc( "Lua_Quest_CheckRoleItem"		, bool( int, int, int )		, CNetSrv_Script::Lua_Quest_CheckRoleItem	);\
//	LuaRegisterFunc( "Lua_Quest_DeleteItem"			, bool( int, int, int )     , CNetSrv_Script::Lua_Quest_DeleteItem		);
	// LuaRegisterFunc( "DelFromPartition"	, bool( int )                           , DelFromPartition		);


// ÀRºA¦¨­û«Å§i
//-------------------------------------------------------------------
CNetSrv_Gather*		CNetSrv_Gather::m_pThis = NULL;

//-------------------------------------------------------------------
bool CNetSrv_Gather::Init()
{
	Srv_NetCliReg( PG_Gather_CtoL_ClickGatherObj		);
	Srv_NetCliReg( PG_Gather_CtoL_CancelGather			);
	
	return true;
}
//-------------------------------------------------------------------
void CNetSrv_Gather::_PG_Gather_CtoL_CancelGather		( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_Gather_CtoL_CancelGather );
	M_CLIENT_OBJ( iCliID );

	m_pThis->OnCancelGather( pObj, pPacket->iObjID );	
}
//-------------------------------------------------------------------
void CNetSrv_Gather::_PG_Gather_CtoL_ClickGatherObj ( int iCliID , int iSize , PVOID pData )
{
	M_PACKET( PG_Gather_CtoL_ClickGatherObj );
	M_CLIENT_OBJ( iCliID );
	
	m_pThis->On_ClickGatherObj( pObj, pPacket->iObjID );
}
//-------------------------------------------------------------------
void CNetSrv_Gather::ClickGatherObjResult ( int iRoleID, int iResult )
{
	PG_Gather_LtoC_ClickGatherObj_Result Packet;

	Packet.iResult	= iResult;

	Global::SendToCli_ByGUID( iRoleID, sizeof(Packet), &Packet );
}
//-------------------------------------------------------------------
void CNetSrv_Gather::GatherMotion ( int iRoleID, int iMotionID, int iMineType )
{
	PG_Gather_LtoC_GatherMotion Packet;

	Packet.iGatherID	= iRoleID;
	Packet.iMotionID	= iMotionID;
	Packet.iMineType	= iMineType;

	BaseItemObject* pObj = BaseItemObject::GetObj( iRoleID );

	if( pObj == NULL )	
		return;

	RoleDataEx* Owner = pObj->Role();

	PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

	int			i , j;
	PlayID*		TopID;

	for( i = 0 ; Block[i] != NULL ; i++ )
	{
		TopID = Block[i]->Begin;

		for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
		{
			BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );

			//if( OtherObj == NULL || OtherObj->GUID() == pObj->GUID() )
			//	continue;

			if( OtherObj == NULL )
				continue;

			if( OtherObj->Role()->IsNPC() != false )
				continue;

			Global::SendToCli_ByGUID( OtherObj->GUID() , sizeof( Packet ) , &Packet );
		}
	}   

	//Global::SendToCli_ByGUID( iRoleID, sizeof(Packet), &Packet );
}