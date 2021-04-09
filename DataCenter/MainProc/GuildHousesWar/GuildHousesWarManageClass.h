/*
#pragma once
#include "roledata/RoleBaseDef.h"

class SchedularInfo;

//公會屋戰計錄
class GuildHousesWarManageClass
{
	static vector< GuildHouseWarInfoStruct >	_List;
//	static vector< GuildHouseWarHistoryStruct >	_HistoryList;
	static GuildHouseWarStateENUM				_State;

	static int  _OnTimeProc( SchedularInfo* Obj , void* InputClass );
public:
	static	void	Init();
	static  void	Proc();

	static int		DebugTime_Hour;
	static int		DebugTime_Day;
	static vector< GuildHouseWarInfoStruct >&		GetList()			{ return _List; };
//	static vector< GuildHouseWarHistoryStruct >&	GetHistoryList()	{ return _HistoryList; };
	static GuildHouseWarStateENUM				State(){ return _State; }

	static bool	WarRegister( int GuildID );
	static bool	WarCancel( int GuildID );
	static bool	PerpareFight( );
	static bool	BeginFight( );
	static bool	EndFight();


};*/