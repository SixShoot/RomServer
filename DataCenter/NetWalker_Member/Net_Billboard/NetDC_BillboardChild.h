#pragma once

#include "NetDC_Billboard.h"

//--------------------------------------------------------------------------------------------
struct NewBillboardInfoBaseStruct
{
	bool		OnUpdate;				//正在更新
//	bool		OnReset;				//正在重置
	unsigned	NextUpdateTime;			//下一次需要更新的時間
	vector< NewBillBoardRoleInfoStruct >	RankInfo;

	NewBillboardInfoBaseStruct()
	{
		OnUpdate = false;
		NextUpdateTime = 0;
	}
};


class CNetDC_BillboardChild : public CNetDC_Billboard
{

	//初始設定
	static bool _SQLCmdInitProc( vector<DB_BillBoardRoleInfoStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdInitProcResult ( vector<DB_BillBoardRoleInfoStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	
	//讀取名次資訊
	static bool _SQLCmdReadProc( vector<DB_BillBoardRoleInfoStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdReadProcResult ( vector<DB_BillBoardRoleInfoStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//設定匿名
	static bool _SQLCmdSetAnonymousProc( vector<DB_BillBoardRoleInfoStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdSetAnonymousProcResult ( vector<DB_BillBoardRoleInfoStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool	_IsInitReady;
	static int  _NextUpdateTime;
	static bool _IsResetBillboard;
	//定時處理
	static int _OnTimeProc( SchedularInfo* Obj , void* InputClass );

public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

	//要求排名資料 RankBegin = -1 代表取自己等級附近
	virtual void R_RankInfoRequest	( int CliDBID , int CliSockID , int CliProxyID , BillBoardMainTypeENUM Type , int RankBegin );
	virtual void R_SetAnonymous		( int CliDBID , bool IsAnonymous );

protected:
	//////////////////////////////////////////////////////////////////////////
	//新版的排行榜
	//////////////////////////////////////////////////////////////////////////
	static map< int , NewBillboardInfoBaseStruct >	SortTypeRankMap;
	//讀取名次資訊
	static bool _SQLCmdNewUpdateRankProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdNewUpdateRankProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdNewSearchMyRankProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdNewSearchMyRankProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdNewRegisterSortValueProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdNewRegisterSortValueProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdNewOnTimeProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdNewOnTimeProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdNewRegisterNoteProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdNewRegisterNoteProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );


	//定時處理
	static int _NewOnTimeProc( SchedularInfo* Obj , void* InputClass );
public:
	virtual void RL_New_SearchRank				( int PlayerDBID , int SortType , int MinRank , int MaxRank );
	virtual void RL_New_SearchMyRank			( int PlayerDBID , int SortType );
	virtual void RL_New_Register				( int PlayerDBID , int SortType , int SortValue , bool IsAdd , bool IsTopUpdate );
	virtual void RL_New_RegisterNote			( int PlayerDBID , int SortType , const char* Note );

	static void SendSearchRankResult( int PlayerDBID , int SortType , int MinRank , int MaxRank );
};
