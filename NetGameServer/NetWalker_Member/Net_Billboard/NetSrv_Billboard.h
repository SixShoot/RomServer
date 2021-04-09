#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_Billboard.h"

class NetSrv_Billboard : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Billboard* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------
	static void _PG_Billboard_CtoL_CloseBillboardRequest ( int Sockid , int Size , void* Data );
	static void _PG_Billboard_CtoL_RankInfoRequest		 ( int Sockid , int Size , void* Data );
	static void _PG_Billboard_CtoL_SetAnonymous			 ( int Sockid , int Size , void* Data );

public:    
	static void SC_OpenBillboard	( int SenderID , int TargetNpcID );
	static void SC_CloseBillboard	( int SenderID );

	static void SD_RankInfoRequest	( BillBoardMainTypeENUM	Type , int RankBegin , int CliDBID , int CliSockID , int CliProxyID );
	static void SD_SetAnonymous		( int CliDBID , bool IsAnonymous );

	virtual void R_CloseBillboardRequest( BaseItemObject* PlayerObj ) = 0;
	virtual void R_RankInfoRequest( BaseItemObject* Player , BillBoardMainTypeENUM Type , int RankBegin ) = 0;
	virtual void R_SetAnonymous		( BaseItemObject* Player , bool IsAnonymous ) = 0;



	//////////////////////////////////////////////////////////////////////////
	// 新版的排行榜
	//////////////////////////////////////////////////////////////////////////
protected:
	static void _PG_NewBillboard_CtoL_SearchRank	( int Sockid , int Size , void* Data );
	static void _PG_NewBillboard_CtoL_SearchMyRank	( int Sockid , int Size , void* Data );
	static void _PG_NewBillboard_CtoL_RegisterNote	( int Sockid , int Size , void* Data );
public:
	virtual void RC_New_SearchRank	( BaseItemObject* Player , int SortType , int MinRank , int MaxRank ) = 0;
	virtual void RC_New_SearchMyRank( BaseItemObject* Player , int SortType ) = 0;
	virtual void RC_New_RegisterNote( BaseItemObject* Player , int SortType , const char* Note ) = 0;

	static void SD_New_SearchRank			( int PlayerDBID , int SortType , int MinRank , int MaxRank );
	static void SD_New_SearchMyRank			( int PlayerDBID , int SortType );
	static void SD_New_Register_TopUpdate	( int PlayerDBID , int SortType , int Value );
	static void SD_New_Register_Update		( int PlayerDBID , int SortType , int Value );
	static void SD_New_Register_Add			( int PlayerDBID , int SortType , int Value );
	static void SD_New_RegisterNote			( int PlayerDBID , int SortType , const char* Note );
	
};

