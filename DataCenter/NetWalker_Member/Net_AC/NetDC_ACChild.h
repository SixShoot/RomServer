#pragma once
#include <boost/shared_ptr.hpp>
#include "NetDC_AC.h"
/*
//--------------------------------------------------------------------------------------------
struct DB_ACItemStruct
{
	int		ACDIBD;					//�bAC����ƮwID
	StaticString <_MAX_NAMERECORD_SIZE_>	SellerName;		//���W�r
	StaticString <_MAX_NAMERECORD_SIZE_>	BuyerName;		//�R��W�r

	int		NowPrices;
	int		BuyOutPrices;
	int		BuyerDBID;
	int		SellerDBID;
	int		LiveTime;				//�i�ͦs�ɶ�

	ItemFieldStruct		Item;		//�檺�F��

};
*/
//--------------------------------------------------------------------------------------------
typedef boost::shared_ptr<DB_ACItemStruct>   DB_ACItemStructPtr;
//--------------------------------------------------------------------------------------------
struct PlayerSelectManageStruct
{
	int			PlayerDBID;			//���a����ƮwID
	int			Pos;				//�w�ǰe��m
	vector< DB_ACItemStructPtr >	ItemList;			
	set< int >						ItemDBIDSet;	
	set< int >						OrderSet;		//���U�L���w��
	int			SellItemCount;
	int			SelectTime;			//�W�@���j�M�����ɶ�( �C���n�D�n�ۮ��� )
	bool		OnDbSearchFlag;		//���b��DB Search���u�@ �A���ɤ���B�z�s���n�D (�p���n�D���Client�o�X�R�O���W�c �åB���Q�ק諸�i��)
	int			LiveTime;			//�ͦs��
};
//--------------------------------------------------------------------------------------------
struct  ACItemHistoryTypeStruct
{
	int		OrgObjID;		//���~ID
	int		Inherent;		//�ѥͪ��[��O

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
	// ���쪺�ʥ]
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
	static map< int , DB_ACItemStructPtr >			_AllItemList;	//�Ҧ���檺���~
	static map< int , PlayerSelectManageStruct >	_PlayerList;	//�Ҧ����n�D�d�M�����a��T
	static vector< int >							_WaitDelACHistoryList;	//���ݧR��
	static vector< int >							_ProcDelACHistoryList;	//���n�R��(�u���w�ɳB�z�{�ǯ��)

	static map< ACItemHistoryTypeStruct , vector<DB_ACItemHistoryStruct> >	_HistoryList;	//���v�O��

	//�w�ɳB�z
	static int _OnTimeProc( SchedularInfo* Obj , void* InputClass );

protected:
	//--------------------------------------------------------------------
	// �@��欰�B�z
	//--------------------------------------------------------------------
	//�W�[���v�p�� �ç�h�l�����v��ƲM��
	static	bool AddHistoryData( DB_ACItemHistoryStruct* HistoryData );

public:

	//���o���~���v��T
	static	vector<DB_ACItemHistoryStruct>*	GetItemHistory( int OrgObjID , int Inherent );

	//--------------------------------------------------------------------
	//�ƥ�(�ʥ] �P��Ʈw �B�z)
	//--------------------------------------------------------------------
	//���a�j�M����Q�ק�
	static	bool	OnPlayerSelectItemChange( DB_ACItemStruct* Item );


};
