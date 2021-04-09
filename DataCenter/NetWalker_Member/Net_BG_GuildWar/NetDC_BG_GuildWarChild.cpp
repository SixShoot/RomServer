#include "NetDC_BG_GuildWarChild.h"	
#include "ReaderClass/CreateDBCmdClass.h"
#include "ReaderClass/DbSqlExecClass.h"
#include "../../MainProc/GuildManage/GuildManage.h"
//-------------------------------------------------------------------
//-------------------------------------------------------------------
bool    CNetDC_BG_GuildWarChild::Init()
{
	CNetDC_BG_GuildWar::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetDC_BG_GuildWarChild );
	Global::Schedular()->Push( _OnTimeProc , 1000*60 , NULL , NULL );	


	return true;
}
//--------------------------------------------------------------------------------------
bool    CNetDC_BG_GuildWarChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;

	CNetDC_BG_GuildWar::_Release();
	return true;

}

int  CNetDC_BG_GuildWarChild::_OnTimeProc( SchedularInfo* Obj , void* InputClass )
{
	vector< GuildStruct* >&	GuildList = GuildManageClass::This()->GuildList();			//公會列表
	
	for( unsigned i = 0 ; i < GuildList.size() ; i++ )
	{
		if( GuildList[i] == NULL )
			continue;

		GuildBaseStruct& info = GuildList[i]->Base;
		if( info.IsOwnHouse == false )
			continue;
		GuildWarRankInfoStruct rankInfo;
		rankInfo.GuildName = info.GuildName;	
		rankInfo.WorldGuildID = info.GuildID + 0x1000000 * GetWorldID();
		rankInfo.Score		= info.HousesWar.Score;
		rankInfo.WinCount	= info.HousesWar.WinCount;			//勝場
		rankInfo.EvenCount	= info.HousesWar.EvenCount;;		//平手
		rankInfo.LostCount	= info.HousesWar.LostCount;;		//敗場

		This->SBL_RegGuildWarRankInfo( rankInfo , GuildList[i]->GuildWarHistory );
	}


	Global::Schedular()->Push( _OnTimeProc , 1000*60 , NULL , NULL );	
	return 0;
}

struct TempLoadInfoStruct
{
	int ServerID;
	int ZoneGroupID;
	vector< GuildHouseWarInfoStruct > List;
};

void CNetDC_BG_GuildWarChild::RBL_LoadInfo( int ServerID , int ZoneGroupID )
{
	TempLoadInfoStruct* tempData = NEW TempLoadInfoStruct;
	tempData->ServerID = ServerID;
	tempData->ZoneGroupID = ZoneGroupID;
	
	_RD_NormalDB->SqlCmd( ServerID , _SQLCmdLoadInfo , _SQLCmdLoadInfoResult , tempData , NULL );
}
//讀取公會戰的資料
bool CNetDC_BG_GuildWarChild::_SQLCmdLoadInfo( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempLoadInfoStruct* tempData = (TempLoadInfoStruct*)UserObj;
	//char	SqlCmd[2048];
	char	WhereCmd[512];
	MyDbSqlExecClass	MyDBProc( sqlBase );

	sprintf( WhereCmd , "WHERE ZoneGroupID=%d" , tempData->ZoneGroupID );
	GuildHouseWarInfoStruct	TempGuildHouseWarInfo;
	MyDBProc.SqlCmd( _RDGuildHouseWarInfoSql->GetSelectStr( WhereCmd ).c_str() );
	MyDBProc.Bind( TempGuildHouseWarInfo , RoleDataEx::ReaderRD_GuildHouseWarInfo() );
	while( MyDBProc.Read() )
	{
		RoleDataEx::ReaderRD_GuildHouseWarInfo()->TransferWChartoUTF8( &TempGuildHouseWarInfo , MyDBProc.wcTempBufList() );
		tempData->List.push_back( TempGuildHouseWarInfo );
	}
	MyDBProc.Close();

//	char SqlCmd[512];
//	sprintf( SqlCmd , "DELETE GuildHouseWar_Order WHERE ZoneGroupID=%d and State <>%d and " , tempData->ZoneGroupID , EM_GuildHouseWarState_Appointment );
//	MyDBProc->SqlCmd_WriteOneLine( SqlCmd );
	return true;
}
void CNetDC_BG_GuildWarChild::_SQLCmdLoadInfoResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempLoadInfoStruct* tempData = (TempLoadInfoStruct*)UserObj;

	SBL_OrderInfo( tempData->ServerID , tempData->ZoneGroupID , tempData->List );

	delete tempData;
}
