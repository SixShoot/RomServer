#include "NetDC_Item.h"	

Net_Item*	Net_Item::This		= NULL;

//-------------------------------------------------------------------
bool Net_Item::_Release()
{

	return true;
}
//-------------------------------------------------------------------
bool Net_Item::_Init()
{
	_Net->RegOnSrvPacketFunction			( EM_PG_Item_LtoD_AccountBagInfoRequest				, _PG_Item_LtoD_AccountBagInfoRequest	);
	return true;
}
//-------------------------------------------------------------------
void Net_Item::_PG_Item_LtoD_AccountBagInfoRequest	( int ServerID , int Size , void* Data )
{
	PG_Item_LtoD_AccountBagInfoRequest* PG = ( PG_Item_LtoD_AccountBagInfoRequest* )Data;
	This->RL_AccountBagInfoRequest( ServerID , PG->PlayerDBID , (char*)PG->Account.Begin() );
}
//-------------------------------------------------------------------
void Net_Item::SL_AccountBagInfoResult( int SrvSockID , int PlayerDBID , int MaxCount , int ItemDBID[20] , ItemFieldStruct Item[20] )
{
	PG_Item_DtoL_AccountBagInfoResult Send;

	Send.PlayerDBID = PlayerDBID;
	Send.MaxCount = MaxCount;
	memcpy( Send.ItemDBID , ItemDBID , sizeof(Send.ItemDBID) );
	memcpy( Send.Item , Item , sizeof(Send.Item) );

	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );	
}