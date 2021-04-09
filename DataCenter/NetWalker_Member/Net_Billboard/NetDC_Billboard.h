
#pragma once
#include "PG/PG_Billboard.h"
#include "../../MainProc/Global.h"

class CNetDC_Billboard : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetDC_Billboard*	This;

	static bool				_Init();
	static bool				_Release();
	//-------------------------------------------------------------------	
	static void _PG_Billboard_LtoD_RankInfoRequest	( int ServerID , int Size , void* Data );
	static void _PG_Billboard_LtoD_SetAnonymous		( int ServerID , int Size , void* Data );
public:

	static void S_RankInfoResult( int CliSockID , int CliProxyID , BillBoardMainTypeENUM	Type , int RankBegin , int RankEnd , int RankMax , bool Result );
	static void S_RankInfoList	( int CliSockID , int CliProxyID , BillBoardRoleInfoStruct&	Info );


	//要求排名資料 RankBegin = -1 代表取自己等級附近
	virtual void R_RankInfoRequest	( int CliDBID , int CliSockID , int CliProxyID , BillBoardMainTypeENUM Type , int RankBegin ) = 0;
	virtual void R_SetAnonymous		( int CliDBID , bool IsAnonymous ) = 0 ;

protected:
	static void _PG_NewBillboard_LtoD_SearchRank	( int ServerID , int Size , void* Data );
	static void _PG_NewBillboard_LtoD_SearchMyRank	( int ServerID , int Size , void* Data );
	static void _PG_NewBillboard_LtoD_Register		( int ServerID , int Size , void* Data );
	static void _PG_NewBillboard_LtoD_RegisterNote	( int ServerID , int Size , void* Data );
public:
	virtual void RL_New_SearchRank				( int PlayerDBID , int SortType , int MinRank , int MaxRank ) = 0;
	virtual void RL_New_SearchMyRank			( int PlayerDBID , int SortType ) = 0;
	virtual void RL_New_Register				( int PlayerDBID , int SortType , int SortValue , bool IsAdd , bool IsTopUpdate ) = 0;
	virtual void RL_New_RegisterNote			( int PlayerDBID , int SortType , const char* Note ) = 0;

	static void SC_New_SearchRankResult			( int PlayerDBID , int SortType , int TotalCount , int ID , NewBillBoardRoleInfoStruct&	Info );
	static void SC_New_SearchRankResult_Empty	( int PlayerDBID , int SortType );
	static void SC_New_SearchMyRankResult		( int PlayerDBID , int SortType , int MyRank , int SortValue , int MyLastRank , const char* Note );
};
