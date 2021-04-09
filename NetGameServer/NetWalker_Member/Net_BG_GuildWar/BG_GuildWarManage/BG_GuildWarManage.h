#pragma once
#include "../NetSrv_BG_GuildWarChild.h"
#include "DBStruct/DBStructTransfer.h"
#include "ReaderClass/CreateDBCmdClass.h"

class BG_GuildWarManageClass
{
public:
	static void Init();
	static void Release();
protected:
	static BG_GuildWarManageClass*		_This;
	static int  _NextStatusTime;

//	CreateDBCmdClass<GuildHouseWarInfoStruct>*		_RDGuildHouseWarInfoSql;
	vector< GuildHouseWarInfoStruct >	_List;
	GuildHouseWarStateENUM				_State;
	

	static int _OnTimeProc( SchedularInfo* Obj , void* InputClass );	
	static int _OnEvent_ClearGuildWar	 ( SchedularInfo* Obj, void* InputClass );
public:
	int									_DebugTime_Hour;
	int									_DebugTime_Day;

	static BG_GuildWarManageClass* This() { return _This; }
	static GuildHouseWarInfoStruct* GetGuildHouseWarInfo( int GuildID );


	BG_GuildWarManageClass();
	~BG_GuildWarManageClass();

	vector< GuildHouseWarInfoStruct >&		GetList()			{ return _List; };
	GuildHouseWarStateENUM					State()				{ return _State; }

	void	Proc();
	bool	WarRegister( int GuildID , int GuildLv , const char* GuildName , int Score , bool IsAcceptAssignment , int TargetGuild );
	bool	WarCancel( int GuildID );
	bool	PerpareFight( );
	bool	BeginFight( );
	bool	EndFight();
	bool	BeginAppointment();

	static int	NextStatusTime() { return _NextStatusTime; }


};