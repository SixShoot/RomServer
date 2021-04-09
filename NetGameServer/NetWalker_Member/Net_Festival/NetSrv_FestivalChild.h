#pragma once
#include "NetSrv_Festival.h"
#include <vector>
#include <map>
#include <deque>

#define	FESTIVAL_CHECK_TIME_INTERVAL	60 * 1000

struct StructFestivalDescriptor
{
	int							iFestivalID;
	char						Name[_FESTIVAL_NAME_LENGTH];
	char						StartPlot[_MAX_LUA_FUNTIONNAMESIZE_];
	char						EndPlot[_MAX_LUA_FUNTIONNAMESIZE_];
	char						SceneControlString[_FESTIVAL_SCENE_CONTROL_STRING_LENGTH];
	int							iStatus;
	vector<StructFestivalTime>	vecTimes;

	StructFestivalDescriptor()
	{
		iFestivalID    = 0;
		memset( Name      , 0 , sizeof(Name) );
		memset( StartPlot , 0 , sizeof(StartPlot) );
		memset( EndPlot   , 0 , sizeof(EndPlot) );
		memset( SceneControlString, 0 , sizeof(SceneControlString) );
		iStatus = EM_FESTIVAL_STATUS_NONE;
		vecTimes.clear();
	}
};

struct StrucFestivalNpcStatusChange
{
    BaseItemObject* pObj;
	int				iStatus;
};

class CNetSrv_FestivalChild : public CNetSrv_Festival
{
protected:
	static map<int, StructFestivalDescriptor>	m_festivalInfo;

protected:
	static void RequestFestivalInfo();
	static void SendToClient_FestivalStatus(int iCliID, int iFestivalID, int iStatus, const char* pszControlString);
	static void SendToAllClient_FestivalStatus(int iFestivalID, int iStatus, const char* pszControlString);

	static int  CheckFestival(StructFestivalDescriptor& fesDesc);
	static void CheckAllFestival();
	static void OnFestivalCheck(); 
	static int  _OnTimeProc( SchedularInfo* Obj , void* InputClass );

public:
	static bool Init();
	static bool Release();

public:
	virtual void LoadFestivalInfo( void* Data );
	virtual void CliConnect( int CliID , string Account);
};