#pragma once

#include "NetDC_DepartmentStore.h"
//////////////////////////////////////////////////////////////////////////
//�갵�`�N�ƶ�
//1 ��lŪ���Ҧ��ӫ����
//2 �w�ɦ^�s�í��sŪ��( �^�s�ɪ`�N�O�_�����⥿�b�ʶR )
//////////////////////////////////////////////////////////////////////////

// �פJ�b�����M�Ϊ��A�ȦC�|.
enum e_ImportAccountMoney_ProcessStatus // do not over 0xFF (127) (tinyint)
{
    ImportAccountMoneyProcess_Null              = 0x00, // trap, invalid insert will get default value. (also for database update checking)

    ImportAccountMoneyProcess_FirstInsert       = 0x01, // �s���.
    ImportAccountMoneyProcess_CheckFailed       = 0x02, // �ˬd����.
    ImportAccountMoneyProcess_WithdrawFailed    = 0x03, // ���⥢��.
    ImportAccountMoneyProcess_Unlocked          = 0x04, // �����.

    ImportAccountMoneyProcess_Checking          = 0x20, // ���b�ˬd.
    ImportAccountMoneyProcess_Checked           = 0x21, // �w�g�ˬd.

    ImportAccountMoneyProcess_Withdraw          = 0x30, // ���b����.

    ImportAccountMoneyProcess_Locked            = 0x40, // �w�g��w.

    ImportAccountMoneyProcess_Max               = 0xFF, // �̤j��.
};

//--------------------------------------------------------------------------------------------
class CNetDC_DepartmentStoreChild : public CNetDC_DepartmentStore
{
	static bool														_IsEnterBroadcastingProc;
	static vector< string >											_BroadCastingMsg;
	static int														_BroadCastingCount;
	static int														_BroadCastingZipSize;
	static char														_BroadCastingZip[0x10000];
	
	static map< int , DepartmentStoreBaseInfoStruct >				_SellItemList;				//�c�檫�~�C��
	static map< int , DepartmentStoreBaseInfoStruct >				_SellItemList_DB;			//�c�檫�~�C��
	static map< int , vector< DepartmentStoreBaseInfoStruct* > >	_SellItemList_SellType;				
	static map< int , MemoryAllocStruct >							_SellItemList_SellType_Zip;

    static bool m_bImportAccountMoney_CheckAndLog;  // �O�_�}�ұb���פJ�����ˬd.

	static vector<int>	_SellType;
	static bool _IsSaveShopInfo;

	static bool _SQLCmdOnTimeProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdOnTimeProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

    // �פJ�b�������ƫe�ˬd(�I�s���� WebService)
	static bool _SQLCmdImportAccountMoneyCheck(vector<int>& Vec, SqlBaseClass* sqlBase, void* UserObj, ArgTransferStruct& Arg);
    static void _SQLCmdImportAccountMoneyCheckResult(vector<int>& Vec, void* UserObj, ArgTransferStruct& Arg, bool ResultOK);

	//�����I�ƹ�
	static bool _SQLCmdImportAccountMoneyProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdImportAccountMoneyResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//�^�s�I�ƹ�
	static bool _SQLCmdImportAccountMoneyErrProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdImportAccountMoneyErrResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//�I�����~
	static bool _SQLCmdItemExchangeProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdItemExchangeResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//�I�����~���Ѧ^�_
	static bool _SQLCmdItemExchangeErrProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdItemExchangeErrResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLOnTimeBroadCastingProcEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLOnTimeBroadCastingProcResultEvent( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdCheckSendMailEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdCheckSendMailResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//���o�����ӫ����F��
	static bool _SQLCmdImportWebShopItemProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdImportWebShopItemResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//�w�ɳB�z
	static int _OnTimeProc_( SchedularInfo* Obj , void* InputClass );
	static int _OnTimeProc_SellItemListZip( SchedularInfo* Obj , void* InputClass );
	//
	static void _SellItemListZipProc();

	//////////////////////////////////////////////////////////////////////////
	static SlotMachineBase					_SlotMachineBaseInfo[_MAX_SLOT_MACHINE_PRIZES_COUNT_];
	static vector< SlotMachineTypeBase >	_SlotMachineTypeInfo;

	static int  _OnTimeProc_SlotMachine( SchedularInfo* Obj , void* InputClass );
	static bool _SQLCmdLoadSlotMachineEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadSlotMachineResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//////////////////////////////////////////////////////////////////////////
public:
	//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

	virtual void RL_SellTypeRequest	( int SrvSockID , int CliSockID , int CliProxyID );
	virtual void RL_ShopInfoRequest	( int SrvSockID , int CliSockID , int CliProxyID , int SellType );
	virtual void RL_BuyItem			( int SrvSockID , int ItemGUID , int PlayerDBID , int PlayerAccountMoney , int Pos , int Count  );
	virtual void RL_BuyItemResult	( int SrvSockID , int ItemGUID , int PlayerDBID , bool Result , int Count );

	virtual void RL_ImportAccountMoney			( int ServerID , int PlayerDBID , const char* Account );
	virtual void RL_ImportAccountMoneyFailed	( int PlayerDBID , const char* Account , int Money_Account , int Money_Bonus , int ImportGUID );

	virtual void RL_ExchangeItemRequest( int ServerID , int PlayerDBID , const char* Account , char* ItemVersion , char* Password, int MapKey );
	virtual void RL_ExchangeItemResultOK( int ServerID , int PlayerDBID , int ExchangeItemDBID , bool Result , bool IsDataError );

	virtual void RL_RunningMessageRequest( int PlayerDBID );

	virtual void RL_CheckMailGift		( int SrvNetID , int PlayerDBID , int ItemGUID , int Count , const char* TargetName , int MapKey  );

	virtual void RL_WebBagRequest( int ServerID , int PlayerDBID, const char* Account ); 
	virtual void OnLocalSrvConnect( int ZoneID );
};
