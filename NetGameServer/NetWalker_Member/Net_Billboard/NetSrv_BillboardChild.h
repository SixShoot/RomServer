#pragma  once

#include "NetSrv_Billboard.h"

class NetSrv_BillboardChild : public NetSrv_Billboard
{

public:
    static bool Init();
    static bool Release();

	virtual void R_CloseBillboardRequest( BaseItemObject* PlayerObj );
	virtual void R_RankInfoRequest( BaseItemObject* Player , BillBoardMainTypeENUM Type , int RankBegin );
	virtual void R_SetAnonymous		( BaseItemObject* Player , bool IsAnonymous );


	//////////////////////////////////////////////////////////////////////////
	//新版的排行榜
	//////////////////////////////////////////////////////////////////////////
	virtual void RC_New_SearchRank( BaseItemObject* Player , int SortType , int MinRank , int MaxRank );
	virtual void RC_New_SearchMyRank( BaseItemObject* Player , int SortType );
	virtual void RC_New_RegisterNote( BaseItemObject* Player , int SortType , const char* Note );
};

