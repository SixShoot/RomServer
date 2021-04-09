#pragma once
#include <boost/shared_ptr.hpp>
#include "NetDC_AC.h"
/*
//--------------------------------------------------------------------------------------------
struct DB_ACItemStruct
{
	int		ACDIBD;					//在AC的資料庫ID
	StaticString <_MAX_NAMERECORD_SIZE_>	SellerName;		//賣方名字
	StaticString <_MAX_NAMERECORD_SIZE_>	BuyerName;		//買方名字

	int		NowPrices;
	int		BuyOutPrices;
	int		BuyerDBID;
	int		SellerDBID;
	int		LiveTime;				//可生存時間

	ItemFieldStruct		Item;		//賣的東西

};
*/
//--------------------------------------------------------------------------------------------
typedef boost::shared_ptr<DB_ACItemStruct>   DB_ACItemStructPtr;
//--------------------------------------------------------------------------------------------
struct PlayerSelectManageStruct
{
	int			PlayerDBID;			//玩家的資料庫ID
	int			Pos;				//已傳送位置
	vector< DB_ACItemStructPtr >	ItemList;			
	set< int >						ItemDBIDSet;	
	set< int >						OrderSet;		//有下過的定單
	int			SellItemCount;
	int			SelectTime;			//上一次搜尋玩成時間( 每次要求要相格兩秒 )
	bool		OnDbSearchFlag;		//正在做DB Search的工作 ，此時不能處理新的要求 (如有要求表示Client發出命令太頻繁 並且有被修改的可能)
	int			LiveTime;			//生存期
};
//--------------------------------------------------------------------------------------------
struct  ACItemHistoryTypeStruct
{
	int		OrgObjID;		//物品ID
	int		Inherent;		//天生附加能力

	bool operator < ( const ACItemHistoryTypeStruct& P ) const
	{
		if( OrgObjID < P.OrgObjID )
		{
			return true;
		}
		/*
		else if(	OrgObjID == P.OrgObjID 
				&&	Inherent < P.Inherent	)
		{
			return true;
		}
*/
		return false;
	};
};
//--------------------------------------------------------------------------------------------
enum	NewPriceResultENUM
{
	EM_NewPriceResult_Failed	,
	EM_NewPriceResult_NewPrice	,
	EM_NewPriceResult_BuyOut	,
};
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
class CNetDC_ACChild : public CNetDC_AC
{

public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

	//--------------------------------------------------------------------------------------------
	// 收到的封包
	//--------------------------------------------------------------------------------------------
	virtual void RL_SearchItem			( int DBID , ACSearchConditionStruct& Data );
	virtual void RL_GetNextSearchItem	( int DBID );
	virtual void RL_GetMyACItemInfo		( int DBID );
	virtual void RL_BuyItem				( int SrvSockID , int BuyerDBID , int ACItemDBID , int Prices , PriceTypeENUM PricesType );
	virtual void RL_SellItem			( int SrvSockID , int SellerDBID , ItemFieldStruct& Item , int Prices , int BuyOutPrices , PriceTypeENUM	PricesType , int LiveTime , int TaxMoney);
	virtual void RL_CancelSellItem		( int DBID , int ACItemDBID );
	virtual void RL_CloseAC				( int DBID );
	virtual void RL_OpenAC				( int DBID , int NpcGUID );
	virtual void RL_ItemHistoryRequest	( int DBID , int OrgObjID , int Inherent  );
//--------------------------------------------------------------------------------------------

protected:
	static bool	_IsInitReady;

	static bool _SQLCmdInitProc( vector<DB_ACItemStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdInitProcResult ( vector<DB_ACItemStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdSellProc( vector<DB_ACItemStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdSellProcResult ( vector<DB_ACItemStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdCancelSellProc( vector<DB_ACItemStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdCancelSellProcResult ( vector<DB_ACItemStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdSearchProc( vector<DB_ACItemStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdSearchProcResult ( vector<DB_ACItemStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdBuyProc( vector<DB_ACItemStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdBuyProcResult ( vector<DB_ACItemStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdOpenProc( vector<DB_ACItemStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdOpenProcResult ( vector<DB_ACItemStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLOnTimeProc( vector<DB_ACItemStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLOnTimeProcResult ( vector<DB_ACItemStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLImportOnTimeProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLImportOnTimeProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static void _SysSellItem( ItemFieldStruct& Item , int Prices , int BuyOutPrices , PriceTypeENUM	PricesType , int LiveTime );
protected:

	static int										_AcSelectMaxCount;
	static map< int , DB_ACItemStructPtr >			_AllItemList;	//所有拍賣的物品
	static map< int , PlayerSelectManageStruct >	_PlayerList;	//所有有要求查尋的玩家資訊
	static vector< int >							_WaitDelACHistoryList;	//等待刪除
	static vector< int >							_ProcDelACHistoryList;	//正要刪除(只有定時處理程序能用)

	static map< ACItemHistoryTypeStruct , vector<DB_ACItemHistoryStruct> >	_HistoryList;	//歷史記錄

	//定時處理
	static int _OnTimeProc( SchedularInfo* Obj , void* InputClass );

protected:
	//--------------------------------------------------------------------
	// 一般行為處理
	//--------------------------------------------------------------------
	//增加歷史計錄 並把多餘的歷史資料清除
	static	bool AddHistoryData( DB_ACItemHistoryStruct* HistoryData );

public:

	//取得物品歷史資訊
	static	vector<DB_ACItemHistoryStruct>*	GetItemHistory( int OrgObjID , int Inherent );

	//--------------------------------------------------------------------
	//事件(封包 與資料庫 處理)
	//--------------------------------------------------------------------
	//當玩家搜尋物件被修改
	static	bool	OnPlayerSelectItemChange( DB_ACItemStruct* Item );


};
