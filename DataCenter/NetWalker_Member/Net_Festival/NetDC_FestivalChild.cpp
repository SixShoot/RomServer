#include "NetDC_FestivalChild.h"
#define FESTIVAL_THREAD_NUMBER 16

vector<StructFestivalDescriptor>		CNetDC_FestivalChild::m_Festivals;
CreateDBCmdClass<StructDBFestivalInfo>* CNetDC_FestivalChild::m_RDFestivalInfoSql	= NULL;
//CreateDBCmdClass<StructDBFestivalTime>* CNetDC_FestivalChild::m_RDFestivalTimeSql	= NULL;
bool									CNetDC_FestivalChild::_IsInitReady			= false;

//--------------------------------------------------------------------------------------------------------------
//      ****** StructDBFestivalTime ******
//--------------------------------------------------------------------------------------------------------------
ReaderClass<StructDBFestivalInfo>* StructDBFestivalInfo::Reader( )
{
	static  ReaderClass<StructDBFestivalInfo> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		StructDBFestivalInfo* Pt = NULL;
		Reader.SetData( "GUID"				, &Pt->iGUID			, ENUM_Type_IntPoint);
		Reader.SetData( "Name"				, Pt->Name				, ENUM_Type_CharString	, sizeof( Pt->Name ) );
		Reader.SetData( "FestivalID"		, &Pt->iFestivalID		, ENUM_Type_IntPoint);
		Reader.SetData( "StartPlot"			, Pt->StartPlot			, ENUM_Type_CharString	, sizeof( Pt->StartPlot ) );
		Reader.SetData( "EndPlot"			, Pt->EndPlot			, ENUM_Type_CharString	, sizeof( Pt->EndPlot ) );
		Reader.SetData( "SceneControlString", Pt->SceneControlString, ENUM_Type_CharString	, sizeof( Pt->SceneControlString ) );
		Reader.SetData( "StartTime"			, &Pt->fStartTime		, ENUM_Type_SmallDateTime);
		Reader.SetData( "EndTime"			, &Pt->fEndTime			, ENUM_Type_SmallDateTime);
		IsReady = true;

	}      
	return &Reader;
}
/*
//--------------------------------------------------------------------------------------------------------------
//      ****** StructDBFestivalTime ******
//--------------------------------------------------------------------------------------------------------------
ReaderClass<StructDBFestivalTime>* StructDBFestivalTime::Reader( )
{
	static  ReaderClass<StructDBFestivalTime> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		StructDBFestivalTime* Pt = NULL;
		Reader.SetData( "GUID"				, &Pt->iGUID			, ENUM_Type_IntPoint);
		Reader.SetData( "FestivalGUID"		, &Pt->iFestivalGUID	, ENUM_Type_IntPoint);
		Reader.SetData( "StartTime"			, &Pt->fStartTime		, ENUM_Type_SmallDateTime);
		Reader.SetData( "EndTime"			, &Pt->fEndTime			, ENUM_Type_SmallDateTime);
		IsReady = true;

	}      
	return &Reader;
}
*/

bool CNetDC_FestivalChild::Init()
{
	CNetDC_Festival::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetDC_FestivalChild );

	((CNetDC_FestivalChild*)This)->m_RDFestivalInfoSql  = NEW CreateDBCmdClass<StructDBFestivalInfo> ( StructDBFestivalInfo::Reader()  , "FestivalInfo" );
	//((CNetDC_FestivalChild*)This)->m_RDFestivalTimeSql  = NEW CreateDBCmdClass<StructDBFestivalTime> ( StructDBFestivalTime::Reader()  , "FestivalTime" );
	
	_RD_NormalDB_Import->SqlCmd( FESTIVAL_THREAD_NUMBER , _SQLCmdGetFestivalInfo , _SQLCmdGetFestivalInfoResult , NULL , NULL );

	printf( "\n");
	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 1000 );
		_RD_NormalDB_Import->Process( );

		switch( i % 4 )
		{
		case 0:
			printf( "\rFestival Info Loading...../        " );
			break;
		case 1:
			printf( "\rFestival Info Loading.....|        " );
			break;
		case 2:			
			printf( "\rFestival Info Loading.....\\        " );
			break;
		case 3:
			printf( "\rFestival Info Loading.....-        " );
			break;
		}

		if( _IsInitReady == true )
			break;
	}
	printf( "\n");

	Schedular()->Push( _OnTimeProc , _FESTIVAL_INFO_LOAD_TIME_GAP , This , NULL );  

	return true;
}

bool CNetDC_FestivalChild::Release()
{
	delete ((CNetDC_FestivalChild*)This)->m_RDFestivalInfoSql;
	//delete ((CNetDC_FestivalChild*)This)->m_RDFestivalTimeSql;
	return true;
}

void CNetDC_FestivalChild::RL_RequestFestivalInfo( int iZoneID )
{
	Print(LV2, "Festival", "Receive festival info request from Zone %d", iZoneID);

	int iPacketSize = 0;

	void* pPacket = PrepareFestivalInfoPacket(iPacketSize);

	if (pPacket != NULL)
	{
		Global::SendToLocal(iZoneID, iPacketSize, pPacket);

		delete pPacket;
	}
}

bool CNetDC_FestivalChild::_SQLCmdGetFestivalInfo( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass	MyDBProc( sqlBase );
	char Buf[256];

	//-------------------------------------------------------------------
	//read all festival information
	Print(LV2, "Festival", "Loading festival information...");

	StructDBFestivalInfo	dbInfoData;
	map<int, StructFestivalDescriptor*> mapFestivalsIndex;
	map<int, StructFestivalDescriptor*>::iterator it;

	MyDBProc.SqlCmd( m_RDFestivalInfoSql->GetSelectStr("").c_str() );
	MyDBProc.Bind(dbInfoData, StructDBFestivalInfo::Reader());

	m_Festivals.clear();
	mapFestivalsIndex.clear();

	while( MyDBProc.Read() )
	{

		it = mapFestivalsIndex.find(dbInfoData.iFestivalID);

		StructFestivalDescriptor* tmpDesc = NULL;

		if (it != mapFestivalsIndex.end())
		{
			tmpDesc = it->second;
		}
		else
		{
			StructFestivalDescriptor newDesc;
			newDesc.vecTimes.clear();

			newDesc.iGUID		  = dbInfoData.iGUID;
			newDesc.iFestivalID   = dbInfoData.iFestivalID;
			strcpy(newDesc.Name     , dbInfoData.Name);
			strcpy(newDesc.StartPlot, dbInfoData.StartPlot);
			strcpy(newDesc.EndPlot  , dbInfoData.EndPlot);
			strcpy(newDesc.SceneControlString  , dbInfoData.SceneControlString);
			m_Festivals.push_back(newDesc);

			tmpDesc = &m_Festivals[m_Festivals.size() - 1];

			mapFestivalsIndex[dbInfoData.iFestivalID] = tmpDesc;
		}		

		if (tmpDesc != NULL)
		{
			StructFestivalTime newTimeData;

			newTimeData.dwStartTime = (unsigned)TimeExchangeFloatToInt(dbInfoData.fStartTime);
			newTimeData.dwEndTime   = (unsigned)TimeExchangeFloatToInt(dbInfoData.fEndTime);
			tmpDesc->vecTimes.push_back(newTimeData);
		}
		
	}

	MyDBProc.Close();

	//-------------------------------------------------------------------

	/*
	//-------------------------------------------------------------------
	//read all time period of each festival
	Print(LV2, "Festival", "Loading festival time...");

	for ( int i = 0; i < m_Festivals.size(); ++i)
	{
		StructFestivalTime newTimeData;
		StructDBFestivalTime dbTimeData;

		sprintf(Buf, "WHERE FestivalGUID = %d", m_Festivals[i].iGUID );

		MyDBProc.SqlCmd( m_RDFestivalTimeSql->GetSelectStr(Buf).c_str() );
		MyDBProc.Bind(dbTimeData, StructDBFestivalTime::Reader());			

		m_Festivals[i].vecTimes.clear();

		while( MyDBProc.Read() )
		{
			newTimeData.dwStartTime = (unsigned)TimeExchangeFloatToInt(dbTimeData.fStartTime);
			newTimeData.dwEndTime   = (unsigned)TimeExchangeFloatToInt(dbTimeData.fEndTime);
			m_Festivals[i].vecTimes.push_back(newTimeData);

			Print(LV2, "Festival", "Import festival time: Name = %s , StartTime = %d, EndTime = %d", m_Festivals[i].Name, newTimeData.dwStartTime, newTimeData.dwEndTime);
		}

		MyDBProc.Close();
	}
	//-------------------------------------------------------------------
	*/

	return true;
}

void CNetDC_FestivalChild::_SQLCmdGetFestivalInfoResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{

	for (int i = 0; i < m_Festivals.size(); ++i)
	{
		StructFestivalDescriptor* pDesc = &m_Festivals[i];

		Print(LV2, "", "Festival : Name = %s , TagID = %d, StartPlot = %s, EndPlot = %s, SceneControlString = %s", pDesc->Name, pDesc->iFestivalID, pDesc->StartPlot, pDesc->EndPlot, pDesc->SceneControlString);

		for (int j = 0; j < pDesc->vecTimes.size(); ++j)
		{
			StructFestivalTime* pTime = &pDesc->vecTimes[j];
			Print(LV2, "", "Time %d : StartTime = %d, EndTime = %d", i, pTime->dwStartTime, pTime->dwEndTime);
		}
	}

	//Send festival information to all local, also send when new local connected
	Print(LV2, "Festival", "Festival data loaded...");

	SL_All_FestivalInfo();
	_IsInitReady = true;
}

int CNetDC_FestivalChild::_OnTimeProc( SchedularInfo* Obj , void* InputClass )
{
	Print(LV2, "Festival", "_OnTimeProc");

	//Get Festival Data from DB
	_RD_NormalDB_Import->SqlCmd( FESTIVAL_THREAD_NUMBER , _SQLCmdGetFestivalInfo , _SQLCmdGetFestivalInfoResult , NULL , NULL );

	Print(LV2, "Festival", "Next _OnTimeProc after %d", _FESTIVAL_INFO_LOAD_TIME_GAP);
	Schedular()->Push( _OnTimeProc , _FESTIVAL_INFO_LOAD_TIME_GAP , This , NULL );  
	return 0;
}

void* CNetDC_FestivalChild::PrepareFestivalInfoPacket(int& iPacketSize)
{
	void* pPacket = NULL;

	__try
	{
		//              PacketID        Festival number                        FestivalID,      Name,                   StartPlot,                  EndPolt,                    SceneControlString                     FestivalTime number
		iPacketSize = sizeof(DWORD) + sizeof(DWORD)    + m_Festivals.size() * (sizeof(DWORD) + _FESTIVAL_NAME_LENGTH + _MAX_LUA_FUNTIONNAMESIZE_ + _MAX_LUA_FUNTIONNAMESIZE_ + _FESTIVAL_SCENE_CONTROL_STRING_LENGTH + sizeof(DWORD));

		//Add FestivalTime size
		for ( int i = 0; i < m_Festivals.size(); ++i)
		{
			iPacketSize += m_Festivals[i].vecTimes.size() * sizeof(StructFestivalTime);
		}

		pPacket					= NEW BYTE[ iPacketSize ];		
		memset(pPacket, 0, iPacketSize);


		PBYTE ptr				= (PBYTE)pPacket;		

		// PacketID
		*((DWORD*)ptr)			= EM_PG_Festival_DtoL_FestivalInfo;
		ptr						+= sizeof(DWORD);

		// Festival number
		*((DWORD*)ptr)			= m_Festivals.size();
		ptr						+= sizeof(DWORD);

		// Festival Data
		for ( int i = 0; i < m_Festivals.size(); ++i)
		{
			// FestivalID
			*((DWORD*)ptr)			 = m_Festivals[i].iFestivalID;
			ptr						 += sizeof(DWORD);

			// FestivalName
			memcpy(ptr, m_Festivals[i].Name, _FESTIVAL_NAME_LENGTH);
			ptr						 += _FESTIVAL_NAME_LENGTH;

			// StartPlot
			memcpy(ptr, m_Festivals[i].StartPlot, _MAX_LUA_FUNTIONNAMESIZE_);
			ptr						 += _MAX_LUA_FUNTIONNAMESIZE_;

			// EndPlot
			memcpy(ptr, m_Festivals[i].EndPlot, _MAX_LUA_FUNTIONNAMESIZE_);
			ptr						 += _MAX_LUA_FUNTIONNAMESIZE_;

			memcpy(ptr, m_Festivals[i].SceneControlString, _FESTIVAL_SCENE_CONTROL_STRING_LENGTH);
			ptr						 += _FESTIVAL_SCENE_CONTROL_STRING_LENGTH;

			// Time number
			*((DWORD*)ptr)			 = m_Festivals[i].vecTimes.size();
			ptr						 += sizeof(DWORD);

			//Time Data
			for (int j = 0; j < m_Festivals[i].vecTimes.size(); ++j)
			{
				*((StructFestivalTime*)ptr)	= m_Festivals[i].vecTimes[j];
				ptr						    += sizeof(StructFestivalTime);
			}
		}
	}
	__except( ExceptionFilter(GetExceptionCode(), GetExceptionInformation()) )
	{
		Print( LV5 , "Festival" , "PrepareFestivalInfoPacket Crash");

		if (pPacket != NULL)
		{
			delete pPacket;
			pPacket = NULL;
		}
	}

	return pPacket;
}

void CNetDC_FestivalChild::SL_All_FestivalInfo()
{
	Print(LV2, "Festival", "Send festival data to all local...");

	int iPacketSize = 0;

	void* pPacket = PrepareFestivalInfoPacket(iPacketSize);

	if (pPacket != NULL)
	{
		Global::SendToAllLocal( iPacketSize, pPacket );

		delete pPacket;
	}
}

void CNetDC_FestivalChild::ReloadFestival()
{	
	Print(LV2, "Festival", "Reload Festival data...");

	_RD_NormalDB_Import->SqlCmd( FESTIVAL_THREAD_NUMBER , _SQLCmdGetFestivalInfo , _SQLCmdGetFestivalInfoResult , NULL , NULL );
}
