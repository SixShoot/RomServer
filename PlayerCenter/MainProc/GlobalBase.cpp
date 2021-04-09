#include <time.h>
#include <Windows.h>
#include <Psapi.h>
#include "GlobalBase.h"
#include "../netwalker_member/Net_ServerList/Net_ServerList_Child.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"

#define CHECK_SYSTEM_STATUS_TIME_GAP 60000

//---------------------------------------------------------------------------
GSrvNetWaker*                   GlobalBase::_Net = NULL;
CNet_ServerList_Child*          GlobalBase::Net_ServerList = NULL;
//---------------------------------------------------------------------------
void    GlobalBase::_Init()
{
    if( _Net == NULL )
        _Net = NEW( GSrvNetWaker );

    srand( (unsigned)_time32( NULL ) );

    if( Net_ServerList  == NULL )
        Net_ServerList = NEW CNet_ServerList_Child() ;

	AllOnlinePlayerInfoClass::Init();

	Schedular()->Push( CheckSystemStatus , CHECK_SYSTEM_STATUS_TIME_GAP , NULL , NULL );
}
//---------------------------------------------------------------------------
void    GlobalBase::_Release()
{
    if( _Net != NULL )
    {
        delete _Net;
        _Net = NULL;
    }

    if( Net_ServerList != NULL )
    {
        delete Net_ServerList;
        Net_ServerList = NULL;
    }

	AllOnlinePlayerInfoClass::Release();
}
//---------------------------------------------------------------------------
bool GlobalBase::SendToLocal        ( int LocalID , int Size , const void* Data )
{
    return Net_ServerList->SendToLocal( LocalID , Size , (void*)Data );
}
//---------------------------------------------------------------------------
bool GlobalBase::SendToMaster       ( int Size , const void* Data ) 
{
    return Net_ServerList->SendToMaster( Size , (void*)Data );
}
//---------------------------------------------------------------------------
bool GlobalBase::SendToAllGMTools( int Size , const void* Data ) 
{
    return Net_ServerList->SendToAllGMTools( Size , (void*)Data );
}
//---------------------------------------------------------------------------
void GlobalBase::SendToAllServer	( int Size , const void* Data )
{
	_Net->SendToAllSrv( Size , Data );
}
//---------------------------------------------------------------------------
bool GlobalBase::SendToAllLocal		( int Size , const void* Data )
{
	return Net_ServerList->SendToAllLocal( Size , (void*)Data );
}
//---------------------------------------------------------------------------
bool GlobalBase::SendToChat         ( int Size , const void* Data )
{
    return Net_ServerList->SendToChat( Size , (void*)Data );
}
//---------------------------------------------------------------------------
bool GlobalBase::SendToDBCenter     ( int Size , const void* Data )
{
    return Net_ServerList->SendToDataCenter( Size , (void*)Data );
}
//---------------------------------------------------------------------------
bool GlobalBase::SendToLogCenter( int Size , const void* Data )
{
    return Net_ServerList->SendToLogCenter( Size , (void*)Data );
}
//---------------------------------------------------------------------------
bool GlobalBase::SendToRoleDBCenter  ( int PlayerCenterID , int Size , const void* Data )
{
	return Net_ServerList->SendToRoleDBCenter( PlayerCenterID , Size , (void*)Data );
}
//---------------------------------------------------------------------------
bool GlobalBase::SendToPartition    ( int Size , const void* Data)
{
    return Net_ServerList->SendToPartition( Size , (void*)Data );
}
//---------------------------------------------------------------------------
void GlobalBase::SetPrintLV( int LV )
{
	g_pPrint->SetPrintLV( LV );
}

//--------------------------------------------------------------------------------------
bool GlobalBase::SendToCli   ( int SockID , int Size , const void* Data )
{
    return _Net->SendToCli( SockID , Size , Data );
}
//--------------------------------------------------------------------------------------
bool GlobalBase::SendToCli_ByProxyID	( int SockID  , int ProxyID , int Size , const void* Data )
{
	return _Net->SendToCli( SockID , ProxyID , Size , Data );
}
//--------------------------------------------------------------------------------------
void GlobalBase::SendToAllCli( int Size , const void* Data )
{
    _Net->SendToAllCli( Size , Data );
}
//--------------------------------------------------------------------------------------
void GlobalBase::SendToAllPlayer    	( int Size , const void* Data )
{
	_Net->SendToAllPlayer( Size , Data );
}
//--------------------------------------------------------------------------------------
bool GlobalBase::SendToSrvBySockID  ( int SockID , int Size , const void* Data )
{
    return _Net->SendToSrv( SockID , Size , Data );
}
//--------------------------------------------------------------------------------------
int GlobalBase::GetZoneNetID       ( int ZoneID )
{
    return Net_ServerList->LocalID_To_SrvID( ZoneID );
}
//--------------------------------------------------------------------------------------
bool GlobalBase::SendToCli_ByDBID	( int DBID , int Size , const void* Data )
{
	OnlinePlayerInfoStruct* Info = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( DBID );
	if( Info == NULL )
		return false;

	return _Net->SendToCli( Info->SockID , Info->ProxyID , Size , (void*)Data );
}

char* GlobalBase::GetPlayerName		( int DBID )
{
	OnlinePlayerInfoStruct* Info = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( DBID );
	if( Info == NULL )
		return NULL;

	return (char*)Info->Name.Begin();
}
//--------------------------------------------------------------------------------------
//取得某區域有幾個平行空間
int		GlobalBase::GetParallelCount			( int ZoneID )
{
	vector< ServerData* >& LocalList = Net_ServerList->GetLocalList();
	int ParallelCount = 0;
	for( unsigned int i = ZoneID ; i < LocalList.size() ; i+=_DEF_ZONE_BASE_COUNT_ )
	{
		if( LocalList[i] != NULL && LocalList[i]->m_bServerRdy != false )
		{
			ParallelCount++;
		}
	}
	return ParallelCount;
}
//--------------------------------------------------------------------------------------
int		GlobalBase::GetParallelMaxID			( int ZoneID )
{

	vector< ServerData* >& LocalList = Net_ServerList->GetLocalList();
	int ParallelMax = -1;
	for( unsigned int i = ZoneID ; i < LocalList.size() ; i+=_DEF_ZONE_BASE_COUNT_ )
	{
		if( LocalList[i] != NULL && LocalList[i]->m_bServerRdy != false )
		{
			ParallelMax = i/ _DEF_ZONE_BASE_COUNT_;
		}
	}
	return ParallelMax;

}
//--------------------------------------------------------------------------------------
//取得有效的區域
int		GlobalBase::GetParallelZoneID			( int ZoneID , int ParallelID )
{
	ZoneID = ZoneID % _DEF_ZONE_BASE_COUNT_;

	vector< ServerData* >& LocalList = Net_ServerList->GetLocalList();

	int MaxParallelID = GetParallelMaxID( ZoneID ) + 1 ;

	if( MaxParallelID <= 0 )
		return -1;

	unsigned int		ParallelZoneID = ZoneID + ( ParallelID % MaxParallelID ) * _DEF_ZONE_BASE_COUNT_;
	
	for( unsigned int i = 0 ; i < LocalList.size() ; i+=_DEF_ZONE_BASE_COUNT_ )
	{
		if( LocalList[ ParallelZoneID ] != NULL && LocalList[ParallelZoneID]->m_bServerRdy != false )
			return ParallelZoneID;

		ParallelZoneID += _DEF_ZONE_BASE_COUNT_;
		if( ParallelZoneID >= LocalList.size() )
			ParallelZoneID = ParallelZoneID % _DEF_ZONE_BASE_COUNT_;
	}
	return -1;
}
//--------------------------------------------------------------------------------------
//檢查區域是否有開
bool	GlobalBase::CheckZoneID					( int ZoneID )
{
	vector< ServerData* >& LocalList = Net_ServerList->GetLocalList();

	if( LocalList.size() <= (unsigned) ZoneID )
		return false;

	if( LocalList[ZoneID] != NULL && LocalList[ZoneID]->m_bServerRdy != false )
		return true;

	return false;
}
//--------------------------------------------------------------------------------------
//取Client 的 IP Num
unsigned GlobalBase::GetClientIpNum				( int SockID )
{
	return _Net->GetIpNum( SockID );
}

string   GlobalBase::GetClientIpNumStr			( int SockID )
{
	union
	{
		unsigned IPnum;
		struct
		{
			unsigned char IP[4];
		};
	};
	
	IPnum = _Net->GetIpNum( SockID );

	char Buf[32];
	sprintf_s( Buf , sizeof(Buf) , "%d.%d.%d.%d" , IP[0],IP[1],IP[2],IP[3] );
	return Buf;
}

//--------------------------------------------------------------------------------------
int GlobalBase::CheckSystemStatus( SchedularInfo* Obj , void* InputClass )
{
	PROCESS_MEMORY_COUNTERS pmc;
	HANDLE hProcess;

	DWORD dwPID = GetCurrentProcessId();

	hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, dwPID );

	GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc));

	//-----------------------------

	Print( LV2, "SYSTEM", "Memory[ %d B][ %d MB]", pmc.WorkingSetSize, pmc.WorkingSetSize / 1024 / 1024 );

	Schedular()->Push( CheckSystemStatus , CHECK_SYSTEM_STATUS_TIME_GAP , NULL , NULL );

	return 0;
}
