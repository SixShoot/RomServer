#include "NetDC_BillBoard.h"
//-------------------------------------------------------------------
CNetDC_Billboard*	CNetDC_Billboard::This = NULL;
//-------------------------------------------------------------------
bool CNetDC_Billboard::_Release()
{

	return true;
}
//-------------------------------------------------------------------
bool CNetDC_Billboard::_Init()
{
	_Net->RegOnSrvPacketFunction( EM_PG_Billboard_LtoD_RankInfoRequest		, _PG_Billboard_LtoD_RankInfoRequest	);
	_Net->RegOnSrvPacketFunction( EM_PG_Billboard_LtoD_SetAnonymous			, _PG_Billboard_LtoD_SetAnonymous		);


	_Net->RegOnSrvPacketFunction( EM_PG_NewBillboard_LtoD_SearchRank		, _PG_NewBillboard_LtoD_SearchRank		);
	_Net->RegOnSrvPacketFunction( EM_PG_NewBillboard_LtoD_SearchMyRank		, _PG_NewBillboard_LtoD_SearchMyRank	);
	_Net->RegOnSrvPacketFunction( EM_PG_NewBillboard_LtoD_Register			, _PG_NewBillboard_LtoD_Register		);
	_Net->RegOnSrvPacketFunction( EM_PG_NewBillboard_LtoD_RegisterNote		, _PG_NewBillboard_LtoD_RegisterNote	);

	return true;
}
//-------------------------------------------------------------------
void CNetDC_Billboard::_PG_NewBillboard_LtoD_RegisterNote	( int ServerID , int Size , void* Data )
{
	PG_NewBillboard_LtoD_RegisterNote* PG = (PG_NewBillboard_LtoD_RegisterNote*)Data;
	This->RL_New_RegisterNote( PG->PlayerDBID , PG->SortType , PG->Note.Begin() );
}
void CNetDC_Billboard::_PG_NewBillboard_LtoD_Register		( int ServerID , int Size , void* Data )
{
	PG_NewBillboard_LtoD_Register* PG = (PG_NewBillboard_LtoD_Register*)Data;
	This->RL_New_Register( PG->PlayerDBID , PG->SortType , PG->SortValue , PG->IsAdd , PG->IsTopUpdate );
}
void CNetDC_Billboard::_PG_NewBillboard_LtoD_SearchRank		( int ServerID , int Size , void* Data )
{
	PG_NewBillboard_LtoD_SearchRank* PG = (PG_NewBillboard_LtoD_SearchRank*)Data;
	This->RL_New_SearchRank( PG->PlayerDBID , PG->SortType , PG->MinRank , PG->MaxRank );
}
void CNetDC_Billboard::_PG_NewBillboard_LtoD_SearchMyRank	( int ServerID , int Size , void* Data )
{
	PG_NewBillboard_LtoD_SearchMyRank* PG = (PG_NewBillboard_LtoD_SearchMyRank*)Data;
	This->RL_New_SearchMyRank( PG->PlayerDBID , PG->SortType );
}

void CNetDC_Billboard::_PG_Billboard_LtoD_SetAnonymous		( int ServerID , int Size , void* Data )
{
	PG_Billboard_LtoD_SetAnonymous* PG = (PG_Billboard_LtoD_SetAnonymous*)Data;

	This->R_SetAnonymous( PG->DBID , PG->IsAnonymous );
}
void CNetDC_Billboard::_PG_Billboard_LtoD_RankInfoRequest	( int ServerID , int Size , void* Data )
{
	PG_Billboard_LtoD_RankInfoRequest* PG = (PG_Billboard_LtoD_RankInfoRequest*)Data;

	This->R_RankInfoRequest( PG->CliDBID , PG->CliSockID , PG->CliProxyID , PG->Type , PG->RankBegin );
}
//-------------------------------------------------------------------
void CNetDC_Billboard::S_RankInfoResult( int CliSockID , int CliProxyID , BillBoardMainTypeENUM	Type , int RankBegin , int RankEnd , int RankMax , bool Result )
{
	PG_Billboard_DtoC_RankInfoResult Send;
	Send.Type = Type;
	Send.RankBegin = RankBegin;
	Send.RankEnd = RankEnd;
	Send.RankMax = RankMax;
	Send.Result = Result;

	Global::SendToCli_ByProxyID( CliSockID , CliProxyID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
void CNetDC_Billboard::S_RankInfoList	( int CliSockID , int CliProxyID , BillBoardRoleInfoStruct&	Info )
{
	PG_Billboard_DtoC_RankInfoList Send;
	Send.Info = Info;

	Global::SendToCli_ByProxyID( CliSockID , CliProxyID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
void CNetDC_Billboard::SC_New_SearchRankResult	( int PlayerDBID , int SortType , int TotalCount , int ID , NewBillBoardRoleInfoStruct&	Info )
{
	PG_NewBillboard_DtoC_SearchRankResult Send;
	Send.SortType = SortType;
	Send.TotalCount = TotalCount;
	Send.ID = ID;
	Send.Info = Info;
	Global::SendToCli_ByDBID( PlayerDBID , sizeof(Send) , &Send );
}
void CNetDC_Billboard::SC_New_SearchRankResult_Empty	( int PlayerDBID , int SortType )
{
	PG_NewBillboard_DtoC_SearchRankResult Send;
	Send.SortType = SortType;
	Send.TotalCount = 0;
	Send.ID = 0;
	Global::SendToCli_ByDBID( PlayerDBID , sizeof(Send) , &Send );
}
void CNetDC_Billboard::SC_New_SearchMyRankResult		( int PlayerDBID , int SortType , int MyRank , int SortValue , int MyLastRank , const char* Note )
{
	PG_NewBillboard_DtoC_SearchMyRankResult Send;
	Send.SortType = SortType;		//排行類型
	Send.MyRank = MyRank;
	Send.SortValue = SortValue;
	Send.MyLastRank = MyLastRank;
	Send.MyNote = Note;
	Global::SendToCli_ByDBID( PlayerDBID , sizeof(Send) , &Send );
}