#pragma once
#include "roledata/RoleBaseDef.h"
#include <utility>
#include <set>
#include "../GuildHouseManage/GuildHousesClass.h"
#include "../Global.h"

class GuildHouseWarManageClass
{
protected:
	static vector< GuildHouseWarManageClass* >			_List;
	static map< int , GuildWarPlayerInfoStruct >		_PlayerScoreMap;
	static vector< vector<GuildWarPlayerInfoStruct*> >	_RoomPlayerScoreList;

	//戰事結束延後 讓玩家登出 與 清此區域
	static int _WarEndDelayProc		( SchedularInfo* Obj , void* InputClass );   
public:
	static void	ClearAll();
	static void OnTimeProcAll( );
	static GuildHouseWarManageClass*	GetGuildWar_ByRoomID( int RoomID );
	static void	WarEndProc_All(  );
	static void PlayerScoreProc();
	static map< int , GuildWarPlayerInfoStruct >& PlayerScoreMap(){ return _PlayerScoreMap;};
	static GuildWarPlayerInfoStruct& PlayerScore( int DBID );
	static vector<GuildWarPlayerInfoStruct*>* PlayerScoreList_ByRoomID( int RoomID );

//	static void	BeginFight();
protected:
	GuildHouseWarFightStruct			_WarBase;
	GuildHousesManageClass*				_GuildHouse[2];	
	int									_RoomID;
	vector< GuildWarPlayerInfoStruct* > _PlayerList[2];
public:
	GuildHouseWarManageClass( GuildHouseWarFightStruct* WarInfo );
	~GuildHouseWarManageClass();

	void	WarEndProc(  );

	bool	AddScore( int GuildID , int Score );
	GuildHouseWarFightStruct&			WarBase() { return _WarBase; };
	int		RoomID()	{ return _RoomID; };
	int		GetDataPos( int GuildID );
};
