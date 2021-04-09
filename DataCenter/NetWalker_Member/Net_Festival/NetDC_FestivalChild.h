#pragma once
#include "NetDC_Festival.h"

#define _FESTIVAL_INFO_LOAD_TIME_GAP  60*60*1000 // 1hr

struct StructDBFestivalInfo
{
	int   iGUID;
	int   iFestivalID;
	char  Name[_FESTIVAL_NAME_LENGTH];	
	char  StartPlot[_MAX_LUA_FUNTIONNAMESIZE_];
	char  EndPlot[_MAX_LUA_FUNTIONNAMESIZE_];
	char  SceneControlString[_FESTIVAL_SCENE_CONTROL_STRING_LENGTH];
	float fStartTime;
	float fEndTime;

	StructDBFestivalInfo()
	{
		memset( this , 0 , sizeof(*this) );
	}

	static ReaderClass<StructDBFestivalInfo>* Reader();
};

/*
struct StructDBFestivalTime
{
	int   iGUID;
	int   iFestivalGUID;
	float fStartTime;
	float fEndTime;

	StructDBFestivalTime()
	{
		memset( this , 0 , sizeof(*this) );
	}

	static ReaderClass<StructDBFestivalTime>* Reader();
};
*/

struct StructFestivalDescriptor
{
	int							iGUID;
	int							iFestivalID;
	char						Name[_FESTIVAL_NAME_LENGTH];
	char						StartPlot[_MAX_LUA_FUNTIONNAMESIZE_];
	char						EndPlot[_MAX_LUA_FUNTIONNAMESIZE_];
	char						SceneControlString[_FESTIVAL_SCENE_CONTROL_STRING_LENGTH];
	vector<StructFestivalTime>	vecTimes;

	StructFestivalDescriptor()
	{
		iGUID		  = 0;
		iFestivalID   = 0;
		memset( Name	  , 0 , sizeof(Name) );
		memset( StartPlot , 0 , sizeof(StartPlot) );
		memset( EndPlot   , 0 , sizeof(EndPlot) );
		memset( SceneControlString, 0 , sizeof(EndPlot) );
		vecTimes.clear();
	}
};

class CNetDC_FestivalChild : public CNetDC_Festival
{
protected:
	static bool						_IsInitReady;

protected:
	static vector<StructFestivalDescriptor> m_Festivals;
	static CreateDBCmdClass<StructDBFestivalInfo>* m_RDFestivalInfoSql;
	//static CreateDBCmdClass<StructDBFestivalTime>* m_RDFestivalTimeSql;	
	
protected:
	static bool _SQLCmdGetFestivalInfo( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdGetFestivalInfoResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//-------------------------------------------------------------------	
	static int  _OnTimeProc( SchedularInfo* Obj , void* InputClass );

	static void* PrepareFestivalInfoPacket(int& iPacketSize);
	static void SL_All_FestivalInfo();

public:
	static bool Init();
	static bool Release();

	//For test
	static void ReloadFestival();

public:
	virtual void RL_RequestFestivalInfo( int iZoneID );
};
