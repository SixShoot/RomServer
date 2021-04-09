#pragma once

#include "NetDC_Billboard.h"

//--------------------------------------------------------------------------------------------
struct NewBillboardInfoBaseStruct
{
	bool		OnUpdate;				//���b��s
//	bool		OnReset;				//���b���m
	unsigned	NextUpdateTime;			//�U�@���ݭn��s���ɶ�
	vector< NewBillBoardRoleInfoStruct >	RankInfo;

	NewBillboardInfoBaseStruct()
	{
		OnUpdate = false;
		NextUpdateTime = 0;
	}
};


class CNetDC_BillboardChild : public CNetDC_Billboard
{

	//��l�]�w
	static bool _SQLCmdInitProc( vector<DB_BillBoardRoleInfoStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdInitProcResult ( vector<DB_BillBoardRoleInfoStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	
	//Ū���W����T
	static bool _SQLCmdReadProc( vector<DB_BillBoardRoleInfoStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdReadProcResult ( vector<DB_BillBoardRoleInfoStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//�]�w�ΦW
	static bool _SQLCmdSetAnonymousProc( vector<DB_BillBoardRoleInfoStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdSetAnonymousProcResult ( vector<DB_BillBoardRoleInfoStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool	_IsInitReady;
	static int  _NextUpdateTime;
	static bool _IsResetBillboard;
	//�w�ɳB�z
	static int _OnTimeProc( SchedularInfo* Obj , void* InputClass );

public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

	//�n�D�ƦW��� RankBegin = -1 �N����ۤv���Ū���
	virtual void R_RankInfoRequest	( int CliDBID , int CliSockID , int CliProxyID , BillBoardMainTypeENUM Type , int RankBegin );
	virtual void R_SetAnonymous		( int CliDBID , bool IsAnonymous );

protected:
	//////////////////////////////////////////////////////////////////////////
	//�s�����Ʀ�]
	//////////////////////////////////////////////////////////////////////////
	static map< int , NewBillboardInfoBaseStruct >	SortTypeRankMap;
	//Ū���W����T
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


	//�w�ɳB�z
	static int _NewOnTimeProc( SchedularInfo* Obj , void* InputClass );
public:
	virtual void RL_New_SearchRank				( int PlayerDBID , int SortType , int MinRank , int MaxRank );
	virtual void RL_New_SearchMyRank			( int PlayerDBID , int SortType );
	virtual void RL_New_Register				( int PlayerDBID , int SortType , int SortValue , bool IsAdd , bool IsTopUpdate );
	virtual void RL_New_RegisterNote			( int PlayerDBID , int SortType , const char* Note );

	static void SendSearchRankResult( int PlayerDBID , int SortType , int MinRank , int MaxRank );
};
