#include "NetDC_Other.h"
#include "../net_serverlist/Net_ServerList_Child.h"
//-------------------------------------------------------------------
CNetDC_Other*	CNetDC_Other::This = NULL;

//-------------------------------------------------------------------
bool CNetDC_Other::_Release()
{
	return true;
}
//-------------------------------------------------------------------
bool CNetDC_Other::_Init()
{
	Net_ServerList->RegServerReadyEvent( EM_SERVER_TYPE_LOCAL , _OnLocalSrvConnectEvent );

	_Net->RegOnSrvPacketFunction( EM_PG_Other_LtoD_Find_DBID_RoleName		, _PG_Other_LtoD_Find_DBID_RoleName		);
	//_Net->RegOnSrvPacketFunction( EM_PG_Other_LtoD_Performance		, _PG_Other_LtoD_Performance		);
	_Net->RegOnSrvPacketFunction( EM_PG_Other_LtoD_SelectDB					, _PG_Other_LtoD_SelectDB				);
	_Net->RegOnSrvPacketFunction( EM_PG_Other_LtoD_LoadClientData			, _PG_Other_LtoD_LoadClientData			);
	_Net->RegOnSrvPacketFunction( EM_PG_Other_MtoD_LoadClientData_Account	, _PG_Other_MtoD_LoadClientData_Account	);
	_Net->RegOnSrvPacketFunction( EM_PG_Other_LtoD_OpenSession				, _PG_Other_LtoD_OpenSession			);
	_Net->RegOnSrvPacketFunction( EM_PG_Other_LtoD_CloseSession				, _PG_Other_LtoD_CloseSession			);
	return true;
}

void CNetDC_Other::_PG_Other_MtoD_LoadClientData_Account	( int ServerID , int Size , void* Data )
{
	PG_Other_MtoD_LoadClientData_Account* pg =(PG_Other_MtoD_LoadClientData_Account*)Data;
	This->RM_LoadClientData_Account( pg->SockID , pg->ProxyID , pg->Account.Begin() , pg->KeyID );
}

void CNetDC_Other::_PG_Other_LtoD_LoadClientData		( int ServerID , int Size , void* Data )	
{
	PG_Other_LtoD_LoadClientData* pg =(PG_Other_LtoD_LoadClientData*)Data;
	This->RL_LoadClientData( pg->PlayerDBID , pg->KeyID );
}

void CNetDC_Other::_OnLocalSrvConnectEvent ( EM_SERVER_TYPE ServerType, DWORD SrvID )
{
	This->OnLocalSrvConnect( SrvID );
}

void CNetDC_Other::_PG_Other_LtoD_SelectDB			( int ServerID , int Size , void* Data )	
{
	PG_Other_LtoD_SelectDB* pg =(PG_Other_LtoD_SelectDB*)Data;
	This->RL_SelectDB( pg->PlayerDBID , pg->DataType.Begin() , pg->SelectCmd.Begin() );
}

void CNetDC_Other::_PG_Other_LtoD_Find_DBID_RoleName( int ServerID , int Size , void* Data )
{
	PG_Other_LtoD_Find_DBID_RoleName* pg =(PG_Other_LtoD_Find_DBID_RoleName*)Data;
	This->RL_Find_DBID_RoleName( pg->CliDBID , pg->DBID , (char*)pg->RoleName.Begin() );
}
//void CNetDC_Other::_PG_Other_LtoD_Performance( int ServerID , int Size , void* Data )
//{
//	PG_Other_LtoD_Performance* pg =(PG_Other_LtoD_Performance*)Data;
//	char* Mac = pg->cMac;
//	This->RL_SendPerformance( pg->CliDBID, Mac, pg->MaxCPUUsage, pg->MaxMemUsage, pg->MaxFPS, pg->AverageCPUUsage, pg->AverageMemUsage, pg->AverageFPS, pg->MinCPUUsage, pg->MinMemUsage, pg->MinFPS );
//}

void CNetDC_Other::_PG_Other_LtoD_OpenSession( int ServerID , int Size , void* Data )
{
	PG_Other_LtoD_OpenSession* pg =(PG_Other_LtoD_OpenSession*)Data;
	This->RL_OpenSession( pg->Account.Begin(), pg->RoleDBID, pg->ZoneID, pg->Type, pg->SessionID, pg->ValidTime );
}

void CNetDC_Other::_PG_Other_LtoD_CloseSession( int ServerID , int Size , void* Data )
{
	PG_Other_LtoD_CloseSession* pg =(PG_Other_LtoD_CloseSession*)Data;
	This->RL_CloseSession( pg->Account.Begin(), pg->RoleDBID, pg->ZoneID, pg->Type, pg->SessionID );
}

void CNetDC_Other::SC_Find_DBID_RoleName( int SendDBID , int DBID , const char* RoleName , bool IsFind )
{
	PG_Other_DtoC_Find_DBID_RoleName Send;
	Send.DBID = DBID;
	Send.RoleName = RoleName;
	Send.IsFind = IsFind;
	Global::SendToCli_ByDBID( SendDBID , sizeof(Send) , &Send );
}

void CNetDC_Other::SL_ExchangeMoneyTable( int ZoneID , int Count ,  int DataSize , const char* Data )
{
	PG_Other_DtoL_ExchangeMoneyTable Send;
	Send.Count = Count;
	Send.DataSize = DataSize;
	memcpy( Send.Data , Data , DataSize );
	Global::SendToLocal( ZoneID , Send.Size() , &Send );
}

void CNetDC_Other::SL_DBStringTable( int ZoneID , int Count ,  int DataSize , const char* Data )
{
	PG_Other_DtoL_DBStringTable Send;
	Send.Count = Count;
	Send.DataSize = DataSize;
	memcpy( Send.Data , Data , DataSize );
	Global::SendToLocal( ZoneID , Send.Size() , &Send );
}
/*
void CNetDC_Other::SendAllLocal_ExchangeMoneyTable( int Count , int DataSize , const char* Data )
{
	PG_Other_DtoL_ExchangeMoneyTable Send;
	Send.Count = Count;
	Send.DataSize = DataSize;
	memcpy( Send.Data , Data , DataSize );
	Global::SendToAllLocal( ZoneID , Send.Size() , &Send );
}*/
void CNetDC_Other::SL_All_DBStringTable( int Count ,  int DataSize , const char* Data )
{
	PG_Other_DtoL_DBStringTable Send;
	Send.Count = Count;
	Send.DataSize = DataSize;
	memcpy( Send.Data , Data , DataSize );
	//Global::SendToLocal( ZoneID , Send.Size() , &Send );
	Global::SendToAllLocal( Send.Size() , &Send );
}
void CNetDC_Other::SC_LoadClientDataResult( int PlayerDBID , int KeyID , int Size , const char* Data )
{
	PG_Other_DtoC_LoadClientDataResult Send;
	Send.KeyID = KeyID;
	Send.Size = Size;
	memcpy( Send.Data , Data , Size );
	Global::SendToCli_ByDBID( PlayerDBID , Send.PGSize() , &Send );
}

void CNetDC_Other::SC_LoadClientDataResult_Account( int SockID , int ProxyID , int KeyID , int Size , const char* Data )
{
	PG_Other_DtoC_LoadClientDataResult_Account Send;
	Send.KeyID = KeyID;
	Send.Size = Size;
	memcpy( Send.Data , Data , Size );
	Global::SendToCli_ByProxyID(  SockID , ProxyID , Send.PGSize() , &Send );
}

void CNetDC_Other::SL_SysKeyValue( int SrvID , int TotalCount , int ID , const char* KeyStr , int Value )
{
	PG_Other_DtoL_SysKeyValue Send;
	Send.TotalCount = TotalCount;
	Send.ID = ID;
	strncpy( Send.KeyStr , KeyStr , sizeof( Send.KeyStr ) );
	Send.Value = Value;
	//Global::SendToLocal( ZoneID , sizeof(Send) , &Send );
	Global::SendToLocal( SrvID , sizeof(Send) , &Send );
}

void CNetDC_Other::SL_RoleRunPlot(int ZoneID, const char* LuaPlot, int DBID)
{
	PG_Other_DtoL_RoleRunPlot Send;
	Send.LuaPlot = LuaPlot;
	Send.LuaPlot.SafeStr();
	Send.DBID = DBID;

	Global::SendToLocal( ZoneID , sizeof(Send) , &Send );
}
void CNetDC_Other::SL_WeekInstances( int SrvID , vector<WeekZoneStruct>& zoneGroupIDList )
{
	PG_Other_DtoL_WeekInstances Send;

	Send.Count = zoneGroupIDList.size();
	for( int i = 0 ; i < Send.Count ; i++ )
		Send.Info[i] = zoneGroupIDList[i];

	Global::SendToLocal( SrvID , sizeof(Send) , &Send );
}
//後台Buff設定
void CNetDC_Other::SL_BuffSchedule( int zoneID , int buffID , int buffLv , int buffTime )
{
	PG_Other_DtoL_BuffSchedule send;
	send.BuffID = buffID;
	send.BuffLv = buffLv;
	send.BuffTime = buffTime;

	if( zoneID != -1 )
	{
		Global::SendToLocal( zoneID , sizeof(send) , &send );
	}
	else
	{
		Global::SendToAllLocal( sizeof(send) , &send );
	}
}
//後台魔幻寶盒資料
void CNetDC_Other::SL_AllItemCombinInfo( int SrvID )
{
	map< int , map< int , vector< ItemCombineStruct > > >::iterator iter1;
	map< int , vector< ItemCombineStruct > >::iterator iter2;

	for( iter1 = RoleDataEx::ItemCombineListEx.begin() ; iter1 != RoleDataEx::ItemCombineListEx.end() ; iter1++ )
	{
		for( iter2 = iter1->second.begin() ; iter2 != iter1->second.end() ; iter2++ )
		{
			vector< ItemCombineStruct >& list = iter2->second;
			for( unsigned i = 0 ; i < list.size() ; i++ )
				SL_ItemCombinInfo( SrvID , iter1->first , list[i] );
		}
	}
}
void CNetDC_Other::SL_ItemCombinInfo( int SrvID , int groupID , ItemCombineStruct& itemCombine )
{
	PG_Other_DtoL_ItemCombinInfo send;
	send.GroupID = groupID;
	send.ItemCombine = itemCombine;
	Global::SendToLocal( SrvID , sizeof(send) , &send );
}