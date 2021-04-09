#include "NetDC_DepartmentStoreChild.h"

#include "NetDC_DepartmentStore_WebServiceAccess.h"

bool													CNetDC_DepartmentStoreChild::_IsEnterBroadcastingProc = false;
vector< string >										CNetDC_DepartmentStoreChild::_BroadCastingMsg;
int														CNetDC_DepartmentStoreChild::_BroadCastingZipSize = 0;
int														CNetDC_DepartmentStoreChild::_BroadCastingCount = 0;
char													CNetDC_DepartmentStoreChild::_BroadCastingZip[0x10000];

map< int , DepartmentStoreBaseInfoStruct >				CNetDC_DepartmentStoreChild::_SellItemList;				
map< int , DepartmentStoreBaseInfoStruct >				CNetDC_DepartmentStoreChild::_SellItemList_DB;				
map< int , vector< DepartmentStoreBaseInfoStruct* > >	CNetDC_DepartmentStoreChild::_SellItemList_SellType;				
map< int , MemoryAllocStruct >							CNetDC_DepartmentStoreChild::_SellItemList_SellType_Zip;

bool													CNetDC_DepartmentStoreChild::_IsSaveShopInfo = false;
vector<int>												CNetDC_DepartmentStoreChild::_SellType;
SlotMachineBase											CNetDC_DepartmentStoreChild::_SlotMachineBaseInfo[_MAX_SLOT_MACHINE_PRIZES_COUNT_];
vector< SlotMachineTypeBase >							CNetDC_DepartmentStoreChild::_SlotMachineTypeInfo;


bool    CNetDC_DepartmentStoreChild::m_bImportAccountMoney_CheckAndLog = false;
//--------------------------------------------------------------------------------------
bool    CNetDC_DepartmentStoreChild::Init()
{
	CNetDC_DepartmentStore::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetDC_DepartmentStoreChild );

	Global::Schedular()->Push( _OnTimeProc_ , 5*1000, NULL , NULL );
	Global::Schedular()->Push( _OnTimeProc_SellItemListZip , 60*1000, NULL , NULL );
	Global::Schedular()->Push( _OnTimeProc_SlotMachine , 5*1000, NULL , NULL );

    IniFileClass* pIni = Global::Ini();
    if (pIni->Int("UseWebServiceCheck") == 1)
    {
        m_bImportAccountMoney_CheckAndLog = true;
    }

	return true;
}
//--------------------------------------------------------------------------------------
bool    CNetDC_DepartmentStoreChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	CNetDC_DepartmentStore::_Release();

	return true;
}
//--------------------------------------------------------------------------------------
int CNetDC_DepartmentStoreChild::_OnTimeProc_SellItemListZip( SchedularInfo* Obj , void* InputClass )
{
	_SellItemListZipProc();
	Global::Schedular()->Push( _OnTimeProc_SellItemListZip , 10*1000, NULL , NULL );
	return 0;
}

//定時處理
int CNetDC_DepartmentStoreChild::_OnTimeProc_( SchedularInfo* Obj , void* InputClass )
{
	if( Global::_IsDestory == true )
		return 0;

	if( _SellItemList_DB.size() != 0 )
	{
		Global::Schedular()->Push( _OnTimeProc_ , 60*1000*5, NULL , NULL );
		return 0;
	}


	_IsSaveShopInfo = true;
	_SellItemList_DB = _SellItemList;

	_RD_NormalDB_Import->SqlCmd( 1 , _SQLCmdOnTimeProc , _SQLCmdOnTimeProcResult , NULL 	, NULL );
	Global::Schedular()->Push( _OnTimeProc_ , 60*1000, NULL , NULL );


	static vector<string>	MessageList;
	if( _IsEnterBroadcastingProc == false )
	{
		_IsEnterBroadcastingProc = true;
		MessageList.clear();
		_RD_NormalDB_Import->SqlCmd( rand() , _SQLOnTimeBroadCastingProcEvent , _SQLOnTimeBroadCastingProcResultEvent , &MessageList , NULL );		 
	}
	return 0;
}
//--------------------------------------------------------------------------------------
bool CNetDC_DepartmentStoreChild::_SQLCmdOnTimeProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{		

	char	SqlCmd[1024];
	MyDbSqlExecClass MyDBProc( sqlBase );
//	MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );
	//寫入所有的資料

//	g_CritSectTreadOnly()->Enter();
	{
		map< int , DepartmentStoreBaseInfoStruct >::iterator Iter;
		for( Iter = _SellItemList_DB.begin() ; Iter != _SellItemList_DB.end() ; Iter++ )
		{
			if( Iter->second.Sell.Count == 0 )
				continue;

			sprintf( SqlCmd , "UPDATE NewShopInfo Set Sell_Count = Sell_Count+%d WHERE GUID =%d" , Iter->second.Sell.Count , Iter->second.GUID );
			MyDBProc.SqlCmd_WriteOneLine( SqlCmd );

		}
		_SellItemList_DB.clear();

		time_t Now;
		time( &Now );
		struct tm *gmt;
		gmt = localtime( &Now );

		//讀取資料庫所有資訊
		CreateDBCmdClass<DB_DepartmentStoreBaseInfoStruct>* _ShopSql  = NEW CreateDBCmdClass<DB_DepartmentStoreBaseInfoStruct > ( RoleDataEx::ReaderRD_DepartmentStoreItem()  , "NewShopInfo" );
		char	SqlWhereCmd[512]={0};
		float   NowValue = TimeExchangeIntToFloat( TimeStr::Now_Value() + Global::_TimeZone*60*60 );
		sprintf( SqlWhereCmd , "WHERE ( ( Sell_BeginTime < %f or Sell_BeginTime = 0 ) and ( Sell_EndTime > %f or Sell_EndTime = 0) and World%d=1  ) and ( EffectDay = -1 or EffectDay = %d)" , NowValue , NowValue , Global::GetWorldID() , gmt->tm_wday );

		MyDBProc.SqlCmd( _ShopSql->GetSelectStr( SqlWhereCmd ).c_str() );

		DB_DepartmentStoreBaseInfoStruct TempItem;
		memset( &TempItem , 0 , sizeof( TempItem ) );

		MyDBProc.Bind( TempItem , RoleDataEx::ReaderRD_DepartmentStoreItem() );

		while( MyDBProc.Read() )
		{
//			RoleDataEx::ReaderRD_DepartmentStoreItem()->TransferWChartoUTF8( &TempItem , MyDBProc.wcTempBufList() );
			TempItem.Item.Item.OrgQuality = 100;
			TempItem.Item.Item.Quality = 100;
			TempItem.Item.Item.PowerQuality = 10;

			if( TempItem.Item.Item.OrgObjID != 0 )
			{
				TempItem.Item.Item.Count = 1;
				for( int i = 0 ; i < 8 ; i++ )
				{
					if( TempItem.ItemCountList[i] >= 0x100 )
						TempItem.ItemCountList[i] = 0xff;
					TempItem.Item.Item.IntAbility[i] = TempItem.ItemIDList[i]*0x100 + TempItem.ItemCountList[i];
				}
			}
			else
			{
				TempItem.Item.Item.OrgObjID  = TempItem.ItemIDList[0];
				TempItem.Item.Item.Count	= TempItem.ItemCountList[0];
				memset( TempItem.Item.Item.Ability , 0 , sizeof(TempItem.Item.Item.Ability) );
			}

			GameObjDbStructEx* ItemDB = g_ObjectData->GetObj( TempItem.Item.Item.OrgObjID );
			if( ItemDB == NULL  )
				continue;

			if(  ItemDB->Mode.DepartmentStore == false )
			{
				if(		ItemDB->Mode.DepartmentStore_Free == false 
					||	TempItem.Item.Sell.Cost_Free == 0 )
				{
					continue;
				}
			}
			else
			{
				if(		ItemDB->Mode.DepartmentStore_Free == false 
					&&	TempItem.Item.Sell.Cost_Free != 0 )
				{
					continue;
				}
			}

			if(		TempItem.Item.Sell.Cost <= 0
				&&	TempItem.Item.Sell.Cost_Free <= 0
				&&	TempItem.Item.Sell.Cost_Bonus <= 0)
				continue;

			TempItem.Item.Item.ImageObjectID = TempItem.Item.Item.OrgObjID;

			_SellItemList_DB[ TempItem.Item.GUID ] = TempItem.Item ;
		}
		MyDBProc.Close();

		delete _ShopSql;
	}
//	g_CritSectTreadOnly()->Leave();

	return true;
}
void CNetDC_DepartmentStoreChild::_SQLCmdOnTimeProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{

	set<int> SellTypeSet;

	_SellItemList = _SellItemList_DB;
	_SellItemList_DB.clear();
	_SellItemList_SellType.clear();

	map< int , DepartmentStoreBaseInfoStruct >::iterator Iter;

	for( Iter = _SellItemList.begin() ; Iter != _SellItemList.end() ; Iter++ )
	{
		ItemFieldStruct& Item = Iter->second.Item ;
		GameObjDbStructEx* OrgDB = g_ObjectData->GetObj( Item.OrgObjID );
		if( OrgDB == NULL )
			continue;
		if( Item.ExValue == 0 )
			Item.Quality = 100;
		Item.Durable = OrgDB->Item.Durable * Item.Quality;

		if( OrgDB->Mode.DurableStart0 == true )
			Item.Durable = 0;

		if( Item.Mode._Mode == 0 )
		{
			Item.Mode.Trade = !(OrgDB->Mode.PickupBound);
			if( OrgDB->IsArmor() || OrgDB->IsWeapon() )
				Item.Mode.EQSoulBound = OrgDB->Mode.EQSoulBound;
			else
				Item.Mode.EQSoulBound = false;
		}
	}

	//map< int , DepartmentStoreBaseInfoStruct >::iterator Iter;
	for( Iter = _SellItemList.begin() ; Iter != _SellItemList.end() ; Iter++ )
	{
		SellTypeSet.insert( Iter->second.SellType[0] );
		_SellItemList_SellType[ Iter->second.SellType[0] ].push_back( &Iter->second );
	}
	_SellType.clear();

	set<int>::iterator SetIter;
	for( SetIter = SellTypeSet.begin() ; SetIter != SellTypeSet.end() ; SetIter++ )
	{		
		_SellType.push_back( *SetIter );
	}

	//////////////////////////////////////////////////////////////////////////
	//建立Zip資料
	_SellItemListZipProc();
	//////////////////////////////////////////////////////////////////////////

	_IsSaveShopInfo = false;
}
//--------------------------------------------------------------------------------------
void CNetDC_DepartmentStoreChild::_SellItemListZipProc()
{
	_SellItemList_SellType_Zip.clear();
	int	Count = 0;
	DepartmentStoreBaseInfoStruct* SellItem = NEW DepartmentStoreBaseInfoStruct[1000];
	char*	DataBuff = NEW char[0x50000];

	map< int , vector< DepartmentStoreBaseInfoStruct* > >::iterator Iter;
	for( Iter = _SellItemList_SellType.begin() ; Iter != _SellItemList_SellType.end() ; Iter++ )
	{
		vector< DepartmentStoreBaseInfoStruct* >& Vec = Iter->second;
		Count = 0;
		for( unsigned int i = 0 ; i < Vec.size() ; i++ )
		{
			if( Count >= 1000 )
				break;

			SellItem[ Count ] = *Vec[i];
			Count++;
		}

		MemoryAllocStruct& Memory = _SellItemList_SellType_Zip[ Iter->first ];
		MyLzoClass LzoProc;
		int ZipSize = LzoProc.Encode( (char*)SellItem , sizeof(DepartmentStoreBaseInfoStruct)*Count , (char*)DataBuff  );
		Memory.Alloc( ZipSize );
		memcpy( Memory.Data() , DataBuff , ZipSize );
	}
	delete DataBuff;
	delete SellItem;
}
//--------------------------------------------------------------------------------------
void CNetDC_DepartmentStoreChild::RL_SellTypeRequest	( int SrvSockID , int CliSockID , int CliProxyID )
{
	SC_SellType( CliSockID , CliProxyID , _SellType );
}

void CNetDC_DepartmentStoreChild::RL_ShopInfoRequest	( int SrvSockID , int CliSockID , int CliProxyID , int SellType )
{
//	SC_ShopInfo( CliSockID , CliProxyID , _SellItemList_SellType[ SellType ] );
	MemoryAllocStruct& Memory = _SellItemList_SellType_Zip[ SellType ];
	SC_ShopInfoZip( CliSockID , CliProxyID 
		, (int)_SellItemList_SellType[ SellType ].size() 
		, (int)Memory.Size() , Memory.Data()  );
}
void CNetDC_DepartmentStoreChild::RL_BuyItem			( int SrvSockID , int ItemGUID , int PlayerDBID , int PlayerAccountMoney , int Pos , int Count )
{

	DepartmentStoreBaseInfoStruct& BuyItem = _SellItemList[ ItemGUID ];

	//檢查時間
	if( BuyItem.Sell.EndTime != 0 )
	{
		int Time = TimeExchangeFloatToInt( BuyItem.Sell.EndTime ) - RoleDataEx::G_TimeZone * 60 * 60;
		if( (unsigned)Time <= RoleDataEx::G_Now )
		{
			SL_BuyItemResult( SrvSockID , PlayerDBID , EM_DepartmentStoreBuyItemResult_TimeOut );
			return;
		}
	}


	if( BuyItem.GUID != ItemGUID )
	{
		SL_BuyItemResult( SrvSockID , PlayerDBID , EM_DepartmentStoreBuyItemResult_Err );
		return;
	}
	if( BuyItem.Item.Count == 0 || Count == 0 )
	{
		SL_BuyItemResult( SrvSockID , PlayerDBID , EM_DepartmentStoreBuyItemResult_Err );
		return;
	}

	int StackCount = Count / BuyItem.Item.Count;
	if( StackCount * BuyItem.Item.Count != Count )
	{
		SL_BuyItemResult( SrvSockID , PlayerDBID , EM_DepartmentStoreBuyItemResult_Err );
		return;
	}



	if( BuyItem.Sell.Cost * StackCount > PlayerAccountMoney )
	{
		SL_BuyItemResult( SrvSockID , PlayerDBID , EM_DepartmentStoreBuyItemResult_MoneyErr );
		return;
	}

	if( BuyItem.Sell.MaxCount != 0 )
	{
		if( BuyItem.Sell.MaxCount < BuyItem.Sell.Count  + BuyItem.Sell.OrgCount + Count )
		{
			SL_BuyItemResult( SrvSockID , PlayerDBID , EM_DepartmentStoreBuyItemResult_SellOut );
			return;
		}
	}

	SL_BuyItemResult( SrvSockID , PlayerDBID , EM_DepartmentStoreBuyItemResult_OK , &BuyItem , Count , Pos );
	return;


}
void CNetDC_DepartmentStoreChild::RL_BuyItemResult		( int SrvSockID , int ItemGUID , int PlayerDBID , bool Result , int Count )
{
	if( Result == false )
		return;

	DepartmentStoreBaseInfoStruct& BuyItem = _SellItemList[ ItemGUID ];	
	//if( BuyItem.Sell.MaxCount != 0 )
	{
		BuyItem.Sell.Count = BuyItem.Sell.Count + Count;
	}
}
//////////////////////////////////////////////////////////////////////////
struct TempImportAccountMoneyReturnStruct
{
	int	GUID;
	int MoneyAccount;
	int MoneyBonus;
	int LockDay;
};
struct TempImportAccountMoneyStruct
{
	 int ServerID;
	 int PlayerDBID;
	 string Account;

	vector< TempImportAccountMoneyReturnStruct > Ret;
};
//--------------------------------------------------------------------------------
// 匯入帳號幣之事前準備(呼叫遠端 WebService)
bool CNetDC_DepartmentStoreChild::_SQLCmdImportAccountMoneyCheck(vector<int>& Vec, SqlBaseClass* sqlBase, void* UserObj, ArgTransferStruct& Arg)
{
    TempImportAccountMoneyStruct* TempData = static_cast<TempImportAccountMoneyStruct*>(UserObj);
    vector<int> DBIDList;
    int	ImportDBID = 0;
    int ImportAccountMoney = 0;
    int	ImportBonusMoney = 0;
    int	ImportLockDay = 0;
    char CmdBuf[512];
    MyDbSqlExecClass MyDBProc(sqlBase);

    // 先鎖定相關資料.
    sprintf(CmdBuf,
            "SP_ImportAccountMoney_CheckValueChange %d, '%s', %d;",
            ImportAccountMoneyProcess_Checking, TempData->Account.c_str(), Global::GetWorldID());
    MyDBProc.SqlCmd_WriteOneLine(CmdBuf);

    // 找出尚未檢查的需求.
    sprintf(CmdBuf,
            "SELECT GUID, Money_Account, Money_Bonus, DATEDIFF(day, getdate(), LockDay) FROM ImportAccountMoney WHERE ProcessTime = %d AND ToAccount = '%s' AND WorldID = %d",
            ImportAccountMoneyProcess_Checking, TempData->Account.c_str(), Global::GetWorldID());

    MyDBProc.SqlCmd(CmdBuf);
    MyDBProc.Bind(1, SQL_C_LONG, SQL_C_DEFAULT, (LPBYTE)&ImportDBID);
    MyDBProc.Bind(2, SQL_C_LONG, SQL_C_DEFAULT, (LPBYTE)&ImportAccountMoney);
    MyDBProc.Bind(3, SQL_C_LONG, SQL_C_DEFAULT, (LPBYTE)&ImportBonusMoney);
    MyDBProc.Bind(4, SQL_C_LONG, SQL_C_DEFAULT, (LPBYTE)&ImportLockDay);

    IniFileClass* pIni = Global::Ini();
    WebServiceAccess* pWSA = NULL;
    bool bResultAll = true;

    while (MyDBProc.Read())
    {
        bool bResult = false;

        // 確認點數幣(連結到營運商提供的遠端網頁做再次驗證)
        // 注意: 以下動作有可能會需要較長時間等待.
        //pWSA = new WebServiceAccess("http://localhost:8064/WebService_ImportCheck.asmx?wsdl", "WebService_ImportCheck", "");
        pWSA = new WebServiceAccess(pIni->Str("WebServiceWSDL").c_str(), pIni->Str("WebServiceName").c_str(), pIni->Str("WebServicePort").c_str());
        if (pWSA)
        {
            pWSA->Call_ImportAccountMoneyCheck(&bResult, ImportDBID, ImportLockDay, ImportAccountMoney, ImportBonusMoney, CmdBuf);

            if (bResult)
            {
                Print(LV1, "_SQLCmdImportAccountMoneyCheck Success", "GUID=%d LockDay=%d MoneyAccount=%d MoneyBonus=%d",
                        ImportDBID, ImportLockDay, ImportAccountMoney, ImportBonusMoney);
            }
            else
            {
                Print(LV2, "_SQLCmdImportAccountMoneyCheck Failed", "GUID=%d LockDay=%d MoneyAccount=%d MoneyBonus=%d",
                        ImportDBID, ImportLockDay, ImportAccountMoney, ImportBonusMoney);
            }

            delete pWSA;
            // 備註: 因為是遠端服務, 所以執行前才產生物件, 執行後馬上刪除.
        }
        else
        {
            Print(LV3, "_SQLCmdImportAccountMoneyCheck", "Cannot Create WebServiceAccess Object!");
        }

        bResultAll &= bResult;
    };
    MyDBProc.Close();

    // 遠端全部確認後, 才能導入原本流程.
    if (bResultAll)
    {
        // 解除鎖定, 改為已經檢查.
        sprintf(CmdBuf,
                "SP_ImportAccountMoney_CheckValueSet %d, '%s', %d, %d;",
                ImportAccountMoneyProcess_Checking, TempData->Account.c_str(), Global::GetWorldID(),
                ImportAccountMoneyProcess_Checked);
        MyDBProc.SqlCmd_WriteOneLine(CmdBuf);

        _RD_NormalDB_Import->SqlCmd(6, _SQLCmdImportAccountMoneyProc, _SQLCmdImportAccountMoneyResult, TempData, NULL);

        return true;
    }
    else
    {
        // 解除鎖定, 改為檢查失敗.
        sprintf(CmdBuf,
                "SP_ImportAccountMoney_CheckValueSet %d, '%s', %d, %d;",
                ImportAccountMoneyProcess_Checking, TempData->Account.c_str(), Global::GetWorldID(),
                ImportAccountMoneyProcess_CheckFailed);
        MyDBProc.SqlCmd_WriteOneLine(CmdBuf);

        //_RD_NormalDB_Import->SqlCmd(6, _SQLCmdImportAccountMoneyErrProc, _SQLCmdImportAccountMoneyErrResult, TempData, NULL);

        return false;
    }
};
void CNetDC_DepartmentStoreChild::_SQLCmdImportAccountMoneyCheckResult(vector<int>& Vec, void* UserObj, ArgTransferStruct& Arg, bool ResultOK)
{
    // ?
}
//--------------------------------------------------------------------------------
void CNetDC_DepartmentStoreChild::RL_ImportAccountMoney			( int ServerID , int PlayerDBID , const char* Account )
{
	TempImportAccountMoneyStruct* TempData = NEW( TempImportAccountMoneyStruct );

	TempData->Account = Account;
	TempData->PlayerDBID = PlayerDBID;
	TempData->ServerID = ServerID;

//	TempData->RetMoneyAccount = 0;
//	TempData->RetMoneyBonus = 0;

	//獨立Thread 處理
    if (m_bImportAccountMoney_CheckAndLog)
    {
        _RD_NormalDB_Import->SqlCmd(PlayerDBID, _SQLCmdImportAccountMoneyCheck, _SQLCmdImportAccountMoneyCheckResult, TempData, NULL);
    }
    else
    {
        _RD_NormalDB_Import->SqlCmd(6, _SQLCmdImportAccountMoneyProc, _SQLCmdImportAccountMoneyResult, TempData, NULL);
    }
}
//提領點數幣
bool CNetDC_DepartmentStoreChild::_SQLCmdImportAccountMoneyProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempImportAccountMoneyStruct* TempData  = (TempImportAccountMoneyStruct*)UserObj;
	vector< int > DBIDList;
	int	ImportDBID = 0;
	int ImportAccountMoney = 0;
	int	ImportBonusMoney = 0;
	int	ImportLockDay = 0;
	char	CmdBuf[512];
	MyDbSqlExecClass MyDBProc( sqlBase );
//	MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );

    if (m_bImportAccountMoney_CheckAndLog)
    {
        sprintf( CmdBuf , "Select GUID , Money_Account , Money_Bonus , DATEDIFF( day , getdate() , LockDay ) From ImportAccountMoney WHERE ProcessTime = %d and ToAccount = '%s' and WorldID=%d", ImportAccountMoneyProcess_Checked, TempData->Account.c_str() , Global::GetWorldID() );
    }
    else
    {
        sprintf( CmdBuf , "Select GUID , Money_Account , Money_Bonus , DATEDIFF( day , getdate() , LockDay ) From ImportAccountMoney WHERE ProcessTime = 0 and ToAccount = '%s' and WorldID=%d", TempData->Account.c_str() , Global::GetWorldID() );
    }

	MyDBProc.SqlCmd( CmdBuf );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ImportDBID );
	MyDBProc.Bind( 2, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ImportAccountMoney );
	MyDBProc.Bind( 3, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ImportBonusMoney );
	MyDBProc.Bind( 4, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&ImportLockDay );

	while( MyDBProc.Read( ) )
	{
		DBIDList.push_back( ImportDBID );
		TempImportAccountMoneyReturnStruct Temp;
		Temp.GUID = ImportDBID;
		Temp.LockDay = ImportLockDay;
		Temp.MoneyAccount = ImportAccountMoney;
		Temp.MoneyBonus = ImportBonusMoney;
		TempData->Ret.push_back( Temp );
	};
	MyDBProc.Close();


	for( unsigned i = 0 ; i < DBIDList.size() ; i++ )
	{
		sprintf( CmdBuf , "UPDATE ImportAccountMoney SET ProcessTime = getdate() WHERE GUID=%d" , DBIDList[i] );
		MyDBProc.SqlCmd_WriteOneLine( CmdBuf );
	}

//	TempData->RetMoneyAccount = ImportAccountMoney;
//	TempData->RetMoneyBonus = ImportBonusMoney;

//	MyDBProc.SqlCmd_WriteOneLine( "COMMIT TRANSACTION" );
	return true;
}
void CNetDC_DepartmentStoreChild::_SQLCmdImportAccountMoneyResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempImportAccountMoneyStruct* TempData  = (TempImportAccountMoneyStruct*)UserObj;

	if( TempData->Ret.size() == 0 )
	{
		SL_ImportAccountMoneyResult( TempData->ServerID , TempData->PlayerDBID , TempData->Account.c_str() , 0 , 0 , 0 , 0 );
	}
	else
	{
		for( unsigned i = 0 ;  i < TempData->Ret.size() ; i++ )
			SL_ImportAccountMoneyResult( TempData->ServerID , TempData->PlayerDBID , TempData->Account.c_str() 
					, TempData->Ret[i].MoneyAccount , TempData->Ret[i].MoneyBonus , TempData->Ret[i].LockDay , TempData->Ret[i].GUID );
	}

	delete TempData;
}
//////////////////////////////////////////////////////////////////////////
struct TempImportAccountMoneyFailedStruct
{
	int PlayerDBID;
	string Account;
	int Money_Account;
	int Money_Bonus;
	int ImportGUID;
};
void CNetDC_DepartmentStoreChild::RL_ImportAccountMoneyFailed	( int PlayerDBID , const char* Account , int Money_Account , int Money_Bonus , int ImportGUID )
{
	Print( LV3 , "RL_ImportAccountMoneyFailed" , "PlayetDBID=%d Account=%s Money_Account=%d ImportGUID=%d" , PlayerDBID , Account , Money_Account , ImportGUID );
	if( Money_Bonus == 0 && Money_Account == 0 )
		return;

	TempImportAccountMoneyFailedStruct* TempData = NEW( TempImportAccountMoneyFailedStruct );
	TempData->Account = Account;
	TempData->PlayerDBID = PlayerDBID;
	TempData->Money_Account = Money_Account;
	TempData->Money_Bonus	= Money_Bonus;
	TempData->ImportGUID = ImportGUID;

	_RD_NormalDB_Import->SqlCmd( PlayerDBID , _SQLCmdImportAccountMoneyErrProc , _SQLCmdImportAccountMoneyErrResult , TempData , NULL );	
}

//回存點數幣
bool CNetDC_DepartmentStoreChild::_SQLCmdImportAccountMoneyErrProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempImportAccountMoneyFailedStruct* TempData  = (TempImportAccountMoneyFailedStruct*)UserObj;
	char	CmdBuf[512];
	MyDbSqlExecClass MyDBProc( sqlBase );
	sprintf( CmdBuf , "Update ImportAccountMoney Set ProcessTime= 0 where GUID=%d" , TempData->ImportGUID );
	MyDBProc.SqlCmd_WriteOneLine( CmdBuf );

	return true;
}
void CNetDC_DepartmentStoreChild::_SQLCmdImportAccountMoneyErrResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempImportAccountMoneyFailedStruct* TempData  = (TempImportAccountMoneyFailedStruct*)UserObj;
	Print( LV3 , "_SQLCmdImportAccountMoneyErrResult" , "PlayetDBID=%d Account=%s Money_Account=%d ImportGUID=%d" , TempData->PlayerDBID , TempData->Account.c_str() , TempData->ImportGUID );	

	delete TempData;
}
//////////////////////////////////////////////////////////////////////////
// 兌換物品的資料結構
struct  TempExchangeItemStruct
{
	//要求的資料
	int	ServerID;
	string Account;
	int PlayerDBID;
	string ItemSerial;
	string Password;
	int MapKey;

	//搜尋回來的資料
	bool				IsUsed;
	ItemFieldStruct		Item;
	int					Money;
	int					Money_Account;
	int					ItemWorldID;
	int					ItemGUID;
	ItemExchangeResultENUM		Type;
	int					LockDay;

	TempExchangeItemStruct()
	{
		Item.Init();
	}
};

void CNetDC_DepartmentStoreChild::RL_ExchangeItemRequest( int ServerID , int PlayerDBID , const char* Account , char* ItemSerial , char* Password, int MapKey )
{
	TempExchangeItemStruct* ExchangeInfo = NEW( TempExchangeItemStruct ) ;

	if( CheckSqlStr( ItemSerial ) == false || CheckSqlStr( Password ) == false )
	{
		return;
	}

	ExchangeInfo->ServerID = ServerID;
	ExchangeInfo->Account = Account;
	ExchangeInfo->PlayerDBID = PlayerDBID;
	ExchangeInfo->ItemSerial = ItemSerial;
	ExchangeInfo->Password = Password;
	ExchangeInfo->MapKey = MapKey;

	_RD_NormalDB_Import->SqlCmd( 7 ,  _SQLCmdItemExchangeProc , _SQLCmdItemExchangeResult , ExchangeInfo , NULL );
}

//兌換物品
bool CNetDC_DepartmentStoreChild::_SQLCmdItemExchangeProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char Buff[512];
	TempExchangeItemStruct* ExchangeInfo = (TempExchangeItemStruct*)UserObj;

	MyDbSqlExecClass MyDBProc(sqlBase);

	std::string AccountX = StringToSqlX(ExchangeInfo->Account.c_str(), _DEF_MAX_ACCOUNTNAME_SIZE_);
	int Count = 0;
	bool DataError = false;
	bool Repeat = false;
	bool AccountLimit = false;
	bool RoleLimit = false;
	int RepeatCount = 0;

	//找出此Key的資料
	sprintf(Buff, "SELECT [GUID], [IsUsed], [DataError], [ItemID], [Count], [Ability], [ImageObjID], [ExValue], [Mode], [Money], [Money_Account], [WorldID], DATEDIFF(day, getdate(), [Money_Account_LockDay]), [Repeat], [RepeatAccountLimit], [RepeatRoleLimit], [RepeatCount] FROM [ExchangeItemTable] WITH (TABLOCKX) WHERE [ItemSerial] = '%s' AND [ItemKey] = '%s'", ExchangeInfo->ItemSerial.c_str(), ExchangeInfo->Password.c_str());
	MyDBProc.SqlCmd(Buff);

	int idx = 1;
	MyDBProc.Bind(idx++, SQL_C_LONG, SQL_C_DEFAULT						, &ExchangeInfo->ItemGUID			);
	MyDBProc.Bind(idx++, SQL_C_BIT , SQL_C_DEFAULT						, &ExchangeInfo->IsUsed				);
	MyDBProc.Bind(idx++, SQL_C_BIT , SQL_C_DEFAULT						, &DataError						);
	MyDBProc.Bind(idx++, SQL_C_LONG, SQL_C_DEFAULT						, &ExchangeInfo->Item.OrgObjID		);
	MyDBProc.Bind(idx++, SQL_C_LONG, SQL_C_DEFAULT						, &ExchangeInfo->Item.Count			);
	MyDBProc.Bind(idx++, SQL_C_CHAR, sizeof(ExchangeInfo->Item.Ability) , &ExchangeInfo->Item.Ability		);
	MyDBProc.Bind(idx++, SQL_C_LONG, SQL_C_DEFAULT						, &ExchangeInfo->Item.ImageObjectID );
	MyDBProc.Bind(idx++, SQL_C_LONG, SQL_C_DEFAULT						, &ExchangeInfo->Item.ExValue		);
	MyDBProc.Bind(idx++, SQL_C_LONG, SQL_C_DEFAULT						, &ExchangeInfo->Item.Mode			);
	MyDBProc.Bind(idx++, SQL_C_LONG, SQL_C_DEFAULT						, &ExchangeInfo->Money				);
	MyDBProc.Bind(idx++, SQL_C_LONG, SQL_C_DEFAULT						, &ExchangeInfo->Money_Account		);
	MyDBProc.Bind(idx++, SQL_C_LONG, SQL_C_DEFAULT						, &ExchangeInfo->ItemWorldID		);
	MyDBProc.Bind(idx++, SQL_C_LONG, SQL_C_DEFAULT						, &ExchangeInfo->LockDay			);
	MyDBProc.Bind(idx++, SQL_C_BIT , SQL_C_DEFAULT						, &Repeat							);
	MyDBProc.Bind(idx++, SQL_C_BIT , SQL_C_DEFAULT						, &AccountLimit						);
	MyDBProc.Bind(idx++, SQL_C_BIT , SQL_C_DEFAULT						, &RoleLimit						);
	MyDBProc.Bind(idx++, SQL_C_LONG, SQL_C_DEFAULT						, &RepeatCount						);

	int Result = MyDBProc.Read();
	MyDBProc.Close();

	ExchangeInfo->Item.Mode.ExchangeItemSystem = 1;

	if(Result == false)
	{
		ExchangeInfo->Type = EM_ItemExchangeResult_Failed;
		return false;
	}

	if(DataError == true)
	{
		ExchangeInfo->Type = EM_ItemExchangeResult_DataError;
		return false;
	}

	if((ExchangeInfo->ItemWorldID != -1) && (Global::GetWorldID() != ExchangeInfo->ItemWorldID))
	{
		ExchangeInfo->Type = EM_ItemExchangeResult_WorldError;
		return false;
	}

	if (Repeat == true)
	{
		if (RepeatCount <= 0)
		{
			ExchangeInfo->Type = EM_ItemExchangeResult_Depleted;
			return false;
		}
	}

	if(ExchangeInfo->IsUsed == true)
	{
		ExchangeInfo->Type = EM_ItemExchangeResult_ItemUsed;
		return false;
	}

	ExchangeInfo->Type = EM_ItemExchangeResult_OK;

	//----------------------------------------
	//重複兌換檢查
	if (Repeat == true)
	{
		//----------------------------------------
		//帳號限制檢查
		if (ExchangeInfo->Type == EM_ItemExchangeResult_OK)
		{
			if (AccountLimit == true)
			{
				Count = 0;
				sprintf(Buff, "SELECT COUNT([GUID]) AS [Count] FROM [ExchangeItemLog] WHERE [ExchangeID] = %d AND [Account] = '%s'", ExchangeInfo->ItemGUID, ExchangeInfo->Account.c_str());
				MyDBProc.SqlCmd(Buff);
				MyDBProc.Bind(1, SQL_C_LONG, SQL_C_DEFAULT, (LPBYTE)&Count);
				MyDBProc.Read();
				MyDBProc.Close();

				if (Count > 0)
				{
					ExchangeInfo->Type = EM_ItemExchangeResult_AccountLimited;
				}
			}
		}
		//----------------------------------------

		//----------------------------------------
		//角色限制檢查
		if (ExchangeInfo->Type == EM_ItemExchangeResult_OK)
		{
			if (RoleLimit == true)
			{
				Count = 0;
				sprintf(Buff, "SELECT COUNT([GUID]) AS [Count] FROM [ExchangeItemLog] WHERE [ExchangeID] = %d AND [PlayerDBID] = %d", ExchangeInfo->ItemGUID, ExchangeInfo->PlayerDBID);
				MyDBProc.SqlCmd(Buff);
				MyDBProc.Bind(1, SQL_C_LONG, SQL_C_DEFAULT, (LPBYTE)&Count);
				MyDBProc.Read();
				MyDBProc.Close();

				if (Count > 0)
				{
					ExchangeInfo->Type = EM_ItemExchangeResult_RoleLimited;
				}
			}
		}
		//----------------------------------------
	}
	else
	{
		//單筆兌換紀錄檢查
		Count = 0;
		sprintf(Buff, "SELECT COUNT([GUID]) AS [Count] FROM [ExchangeItemLog] WHERE [ExchangeID] = %d AND [PlayerDBID] = %d", ExchangeInfo->ItemGUID, ExchangeInfo->PlayerDBID);
		MyDBProc.SqlCmd(Buff);
		MyDBProc.Bind(1, SQL_C_LONG, SQL_C_DEFAULT, (LPBYTE)&Count);
		MyDBProc.Read();
		MyDBProc.Close();

		if (Count > 0)
		{
			ExchangeInfo->Type = EM_ItemExchangeResult_ItemUsed;
		}
	}
	//----------------------------------------

	switch(ExchangeInfo->Type)
	{
	case EM_ItemExchangeResult_OK:
		{
			//資料沒有問題, 先將兌換卷標記為已兌換
			sprintf(Buff, "INSERT INTO [ExchangeItemLog] ([ExchangeID], [Account], [WorldID], [PlayerDBID]) VALUES (%d, %s, %d, %d)", ExchangeInfo->ItemGUID, AccountX.c_str(), Global::GetWorldID(), ExchangeInfo->PlayerDBID);
			MyDBProc.SqlCmd_WriteOneLine(Buff);

			//減少並更新可重複領次數
			if (Repeat == true)
			{
				RepeatCount--;

				sprintf(Buff, "UPDATE [ExchangeItemTable] SET [RepeatCount] = %d WHERE [GUID] = %d", RepeatCount, ExchangeInfo->ItemGUID);
				MyDBProc.SqlCmd_WriteOneLine(Buff);

				if (RepeatCount <= 0)
				{
					sprintf(Buff, "UPDATE [ExchangeItemTable] SET [IsUsed] = 1 WHERE [GUID] = %d", ExchangeInfo->ItemGUID);
					MyDBProc.SqlCmd_WriteOneLine(Buff);
				}
			}
			else
			{
				sprintf(Buff, "UPDATE [ExchangeItemTable] SET [IsUsed] = 1 WHERE [GUID] = %d", ExchangeInfo->ItemGUID);
				MyDBProc.SqlCmd_WriteOneLine(Buff);
			}
		}
		break;
	case EM_ItemExchangeResult_ItemUsed:
		{
			sprintf(Buff, "UPDATE [ExchangeItemTable] SET [IsUsed] = 1 WHERE [GUID] = %d", ExchangeInfo->ItemGUID);
			MyDBProc.SqlCmd_WriteOneLine(Buff);
		}
		break;
	}

	return true;
}
void CNetDC_DepartmentStoreChild::_SQLCmdItemExchangeResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempExchangeItemStruct* ExchangeInfo = (TempExchangeItemStruct*)UserObj;

	SL_ExchangeItemResult( ExchangeInfo->ServerID ,	ExchangeInfo->PlayerDBID , ExchangeInfo->Item , ExchangeInfo->Money , ExchangeInfo->Money_Account , ExchangeInfo->ItemGUID , ExchangeInfo->Type, ExchangeInfo->LockDay, ExchangeInfo->MapKey );

	delete ExchangeInfo;
}
//////////////////////////////////////////////////////////////////////////
struct  TempExchangeItemErrStruct
{
	int	ServerID;
	int PlayerDBID;
	int ExchangeItemDBID;
	bool IsDataError;
};

void CNetDC_DepartmentStoreChild::RL_ExchangeItemResultOK( int ServerID , int PlayerDBID , int ExchangeItemDBID , bool Result , bool IsDataError )
{
	if( Result == true )
		return;

	TempExchangeItemErrStruct* ExchangeInfo = NEW( TempExchangeItemErrStruct ) ;
	ExchangeInfo->ServerID = ServerID;
	ExchangeInfo->PlayerDBID = PlayerDBID;
	ExchangeInfo->ExchangeItemDBID = ExchangeItemDBID;
	ExchangeInfo->IsDataError = IsDataError;

	_RD_NormalDB_Import->SqlCmd( rand() ,  _SQLCmdItemExchangeErrProc , _SQLCmdItemExchangeErrResult , ExchangeInfo , NULL );
}


//兌換物品失敗回復
bool CNetDC_DepartmentStoreChild::_SQLCmdItemExchangeErrProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempExchangeItemErrStruct* ExchangeInfo = (TempExchangeItemErrStruct*)UserObj;

	char Buff[512];

	MyDbSqlExecClass MyDBProc( sqlBase );

	sprintf(Buff, "SELECT [Repeat], [RepeatCount] FROM [ExchangeItemTable] WHERE [GUID] = %d", ExchangeInfo->ExchangeItemDBID);
	MyDBProc.SqlCmd(Buff);

	int LogGUID = 0;
	bool Repeat = false;
	int RepeatCount = 0;

	MyDBProc.Bind(1, SQL_C_BIT , SQL_C_DEFAULT, (LPBYTE)&Repeat		);
	MyDBProc.Bind(2, SQL_C_LONG, SQL_C_DEFAULT, (LPBYTE)&RepeatCount);

	if (MyDBProc.Read())
	{
		MyDBProc.Close();

		if(ExchangeInfo->IsDataError == true)
		{
			sprintf(Buff, "UPDATE [ExchangeItemTable] SET [IsUsed] = 0, [DataError] = 1 WHERE [GUID] = %d", ExchangeInfo->ExchangeItemDBID);
			MyDBProc.SqlCmd_WriteOneLine(Buff);
		}
		else
		{
			sprintf(Buff, "UPDATE [ExchangeItemTable] SET [IsUsed] = 0 WHERE [GUID] = %d", ExchangeInfo->ExchangeItemDBID);
			MyDBProc.SqlCmd_WriteOneLine(Buff);
		}

		sprintf(Buff, "SELECT MAX([GUID]) AS [LogGUID] FROM [ExchangeItemLog] WHERE [ExchangeID] = %d AND [PlayerDBID] = %d AND [WorldID] = %d", ExchangeInfo->ExchangeItemDBID, ExchangeInfo->PlayerDBID, Global::GetWorldID());
		MyDBProc.SqlCmd(Buff);
		MyDBProc.Bind(1, SQL_C_LONG, SQL_C_DEFAULT, (LPBYTE)&LogGUID);

		if (MyDBProc.Read())
		{
			MyDBProc.Close();

			sprintf(Buff, "DELETE FROM [ExchangeItemLog] WHERE [GUID] = %d", LogGUID);
			MyDBProc.SqlCmd_WriteOneLine(Buff);
		}
		else
		{
			MyDBProc.Close();
		}

		if (Repeat == true)
		{
			RepeatCount++;

			sprintf(Buff, "UPDATE [ExchangeItemTable] SET [RepeatCount] = %d WHERE [GUID] = %d", RepeatCount, ExchangeInfo->ExchangeItemDBID);
			MyDBProc.SqlCmd_WriteOneLine(Buff);
		}
	}
	else
	{
		MyDBProc.Close();
	}

	return true;
}
void CNetDC_DepartmentStoreChild::_SQLCmdItemExchangeErrResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempExchangeItemErrStruct* ExchangeInfo = (TempExchangeItemErrStruct*)UserObj;

	delete ExchangeInfo;
}


//////////////////////////////////////////////////////////////////////////
bool CNetDC_DepartmentStoreChild::_SQLOnTimeBroadCastingProcEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	vector<string>& MessageList = *((vector<string>*)UserObj);

	MyDbSqlExecClass	MyDBProc( sqlBase );
	CharTransferClass	CharTransfer;

	vector< int > GuildList;

	char	CmdBuf[4096];
	int		GUID;
	wchar_t	Msg[512];

	sprintf( CmdBuf , "SELECT GUID , Message FROM BroadcastingTable Where  (WorldID = %d) AND (Type=1) AND (GETDATE() < EndTime) AND (StartTime < GETDATE())" , Global::GetWorldID()  );

	MyDBProc.SqlCmd( CmdBuf );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&GUID );
	MyDBProc.Bind( 2, SQL_WCHAR , sizeof(Msg) , (LPBYTE)&Msg );

	while( MyDBProc.Read( ) )
	{	
		GuildList.push_back( GUID );
		CharTransfer.SetWCString( Msg );
		MessageList.push_back( CharTransfer.GetUtf8String() );
	}

	return true;
}
void CNetDC_DepartmentStoreChild::_SQLOnTimeBroadCastingProcResultEvent( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	vector<string>& MessageList = *((vector<string>*)UserObj);
	_BroadCastingMsg = MessageList;
	char	StringBuff[0x10000];
	//char	pStr = StringBuff;
	int		DataSize = 0;
	memset( StringBuff , 0 , sizeof(StringBuff) );
	_BroadCastingCount = (int)MessageList.size();

	for( unsigned int i = 0 ; i < MessageList.size() ; i++ )
	{
		int StrLen = (int)MessageList[i].length();
		if( DataSize + StrLen >= 0x10000  )
		{
			_BroadCastingCount = i;
			break;
		}

		strcpy( DataSize + StringBuff , MessageList[i].c_str() );
		DataSize += (StrLen + 1);
	}

	MyLzoClass LzoProc;
	_BroadCastingZipSize = LzoProc.Encode( (char*)StringBuff , DataSize , _BroadCastingZip );


	_IsEnterBroadcastingProc = false;
}

void CNetDC_DepartmentStoreChild::RL_RunningMessageRequest( int PlayerDBID )
{
	SC_RunningMessage_Zip( PlayerDBID , _BroadCastingCount , _BroadCastingZipSize , _BroadCastingZip );
}

struct CheckSendGiftMailTempStruct
{
	int SrvSockID;
	int PlayerDBID;
	int TargetDBID;
	int	MapKey;
	string TargetName;
	DepartmentStoreBaseInfoStruct Item;
};
void CNetDC_DepartmentStoreChild::RL_CheckMailGift		( int SrvNetID , int PlayerDBID , int ItemGUID , int Count , const char* TargetName , int MapKey )
{

	DepartmentStoreBaseInfoStruct& BuyItem = _SellItemList[ ItemGUID ];

	//檢查時間
	if( BuyItem.Sell.EndTime != 0 )
	{
		int Time = TimeExchangeFloatToInt( BuyItem.Sell.EndTime ) - RoleDataEx::G_TimeZone * 60 * 60;
		if( (unsigned)Time <= RoleDataEx::G_Now )
		{
			SL_CheckMailGiftResult( SrvNetID , MapKey , -1 , EM_CheckMailGiftResultType_TimeOut );
			return;
		}
	}

	if( BuyItem.GUID != ItemGUID )
	{
		SL_CheckMailGiftResult( SrvNetID , MapKey , -1 , EM_CheckMailGiftResultType_DataError );
		return;
	}


	if( BuyItem.Item.Count == 0 || Count == 0 )
	{
		SL_CheckMailGiftResult( SrvNetID , MapKey , -1 , EM_CheckMailGiftResultType_DataError );
		return;
	}

	int StackCount = Count / BuyItem.Item.Count;
	if( StackCount * BuyItem.Item.Count != Count )
	{
		SL_CheckMailGiftResult( SrvNetID , MapKey , -1 , EM_CheckMailGiftResultType_DataError );
		return;
	}

	if( BuyItem.Sell.MaxCount != 0 )
	{
		if( BuyItem.Sell.MaxCount < BuyItem.Sell.Count  + BuyItem.Sell.OrgCount + Count )
		{
			SL_CheckMailGiftResult( SrvNetID , MapKey , -1 , EM_CheckMailGiftResultType_SellOut );
			return;
		}
	}

	//信件檢查

	CheckSendGiftMailTempStruct* TempData = new CheckSendGiftMailTempStruct;
	TempData->MapKey = MapKey;
	TempData->PlayerDBID = PlayerDBID;
	TempData->SrvSockID = SrvNetID;
	TempData->Item = BuyItem;
	TempData->TargetName = TargetName;
	
	_RD_NormalDB->SqlCmd( PlayerDBID , _SQLCmdCheckSendMailEvent , _SQLCmdCheckSendMailResultEvent , TempData  , NULL );
	return;
}

bool CNetDC_DepartmentStoreChild::_SQLCmdCheckSendMailEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	CheckSendGiftMailTempStruct* TempData = (CheckSendGiftMailTempStruct*)UserObj;

	char		Buf[1024];
	bool        Result = true;

	MyDbSqlExecClass	MyDBProc( sqlBase );

	CharTransferClass	CharTransfer;
	CharTransfer.SetUtf8String( TempData->TargetName.c_str() );

	string RoleNameBinStr = StringToSqlX( (char*)CharTransfer.GetWCString() , CharTransfer.WCStrLen() * 2 , false );

	//------------------------------------------------------------------------------------------
	//1 搜尋目標是否存在
	int TargetDBID = -1;
	sprintf( Buf , "SELECT DBID FROM RoleData Where RoleName = CAST( %s AS nvarchar(4000) ) and IsDelete = 0" , RoleNameBinStr.c_str() );

	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TargetDBID );
	MyDBProc.Read( );
	MyDBProc.Close( );

	if( TargetDBID == -1 || TargetDBID == TempData->PlayerDBID )
	{
		//找不到
		//Arg.PushValue( "ResultCode" , EM_SendMailResult_TargetNotFind );
		return false;
	}
	TempData->TargetDBID = TargetDBID;
	return true;


}
void CNetDC_DepartmentStoreChild::_SQLCmdCheckSendMailResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	CheckSendGiftMailTempStruct* TempData = (CheckSendGiftMailTempStruct*)UserObj;

	if( ResultOK == true )
	{
		SL_CheckMailGiftResult( TempData->SrvSockID , TempData->MapKey , TempData->TargetDBID , EM_CheckMailGiftResultType_OK , &(TempData->Item) );
	}
	else
	{
		SL_CheckMailGiftResult( TempData->SrvSockID , TempData->MapKey , -1 , EM_CheckMailGiftResultType_TargetNotFind );
	}

	delete TempData;
}


struct TempWebBagRequestStruct
{
	int ServerID;
	int PlayerDBID;
	string Account;

	//map< int , ItemFieldStruct > ItemList;
	vector< WebShopBagItemFieldDBStruct > ItemList;
};
void CNetDC_DepartmentStoreChild::RL_WebBagRequest( int ServerID , int PlayerDBID, const char* Account ) 
{
	TempWebBagRequestStruct* TempData = NEW TempWebBagRequestStruct;
	TempData->ServerID = ServerID;
	TempData->PlayerDBID = PlayerDBID;
	TempData->Account = Account;
	_RD_NormalDB_Import->SqlCmd( PlayerDBID , _SQLCmdImportWebShopItemProc , _SQLCmdImportWebShopItemResult , TempData 	, NULL );
}

bool CNetDC_DepartmentStoreChild::_SQLCmdImportWebShopItemProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempWebBagRequestStruct* TempData = (TempWebBagRequestStruct*)UserObj;

	MyDbSqlExecClass MyDBProc( sqlBase );
	char SqlCmd[512];
	sprintf( SqlCmd , "WHERE Account_ID = '%s' and ( WorldID=-1 or WorldID=%d ) and ( PlayerDBID=-1 or PlayerDBID=%d ) and DestroyTime = 0 and Status = 0"
		, TempData->Account.c_str()
		, Global::GetWorldID()
		, TempData->PlayerDBID		);
	MyDBProc.SqlCmd( _RDWebShopBagSql->GetSelectStr( SqlCmd ).c_str() );

	WebShopBagItemFieldDBStruct TempAccountBagItem;
	TempAccountBagItem.Init();
	MyDBProc.Bind( TempAccountBagItem , RoleDataEx::ReaderRD_WebShopBag() );
	int Count = 0;
	while( MyDBProc.Read( ) )
	{
		RoleDataEx::ReaderRD_WebShopBag()->TransferWChartoUTF8( &TempAccountBagItem , MyDBProc.wcTempBufList() );
		TempData->ItemList.push_back( TempAccountBagItem );
	}

	MyDBProc.Close();

	return true;
}
void CNetDC_DepartmentStoreChild::_SQLCmdImportWebShopItemResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempWebBagRequestStruct* TempData = (TempWebBagRequestStruct*)UserObj;

	for( unsigned int i = 0 ; i < TempData->ItemList.size() ; i++ )
	{
		WebShopBagItemFieldDBStruct& webItem = TempData->ItemList[i];
		SL_WebBagResult( TempData->ServerID , TempData->PlayerDBID , webItem.ItemDBID , &(webItem.Item) , false , webItem.Message , webItem.PriceType , webItem.Price, webItem.AddBonusMoney );
	}
	SL_WebBagResult( TempData->ServerID , TempData->PlayerDBID , -1 , NULL , true , "" , 0 , 0 , 0 );

	delete TempData;
}

struct TempLoadSlotMachine
{
	SlotMachineBase					SlotMachineBaseInfo[_MAX_SLOT_MACHINE_PRIZES_COUNT_];
	vector< SlotMachineTypeBase >	TypeInfo;
	TempLoadSlotMachine()
	{
		memset( SlotMachineBaseInfo , 0 , sizeof(SlotMachineBaseInfo) );
	}
};
int  CNetDC_DepartmentStoreChild::_OnTimeProc_SlotMachine( SchedularInfo* Obj , void* InputClass )
{
	if( Global::_IsDestory == true )
		return 0;

	TempLoadSlotMachine* tempData = NEW TempLoadSlotMachine;
	_RD_NormalDB_Import->SqlCmd( 1 , _SQLCmdLoadSlotMachineEvent , _SQLCmdLoadSlotMachineResultEvent , tempData , NULL );
	Global::Schedular()->Push( _OnTimeProc_SlotMachine , 10*60*1000, NULL , NULL );

	return 0;
}

bool CNetDC_DepartmentStoreChild::_SQLCmdLoadSlotMachineEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempLoadSlotMachine* TempData = static_cast<TempLoadSlotMachine*>(UserObj);

	MyDbSqlExecClass	MyDBProc( sqlBase );
	{
		SlotMachineBase TempBase;
		MyDBProc.SqlCmd( "SELECT Type, Pos, Rate FROM SlotMachineBase order by pos" );
		MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempBase.Type );
		MyDBProc.Bind( 2, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempBase.Pos );
		MyDBProc.Bind( 3, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempBase.Rate );
		for( int i = 0 ; i< _MAX_SLOT_MACHINE_PRIZES_COUNT_ ; i++ )
		{
			if( MyDBProc.Read( ) == false )
			{			
				MyDBProc.Close( );
				return false;
			}
			TempBase.Pos = i;
			TempData->SlotMachineBaseInfo[i] = TempBase;

		}
		MyDBProc.Close( );
	}

	{
		SlotMachineTypeBase TempType;
		MyDBProc.SqlCmd( "SELECT Type, Rate, ItemID, ItemCount FROM SlotMachine_TypeInfo" );
		MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempType.Type );
		MyDBProc.Bind( 2, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempType.Rate );
		MyDBProc.Bind( 3, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempType.ItemID );
		MyDBProc.Bind( 4, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempType.ItemCount );

		while( MyDBProc.Read( ) )
		{
			TempData->TypeInfo.push_back( TempType );
		}
		MyDBProc.Close( );
	}

	return true;	

}
void CNetDC_DepartmentStoreChild::_SQLCmdLoadSlotMachineResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempLoadSlotMachine* TempData = static_cast<TempLoadSlotMachine*>(UserObj);

	if( TempData->TypeInfo.size() == 0 )
	{

	}
	else if(	memcmp( TempData->SlotMachineBaseInfo , _SlotMachineBaseInfo , sizeof(_SlotMachineBaseInfo)) == 0 
			&&	TempData->TypeInfo.size() == _SlotMachineTypeInfo.size()
			&&	memcmp( &TempData->TypeInfo[0] , &_SlotMachineTypeInfo[0] , _SlotMachineTypeInfo.size() * sizeof(SlotMachineTypeBase)) == 0 )
	{

	}
	else
	{
		memcpy( _SlotMachineBaseInfo , TempData->SlotMachineBaseInfo , sizeof(_SlotMachineBaseInfo) );
		_SlotMachineTypeInfo = TempData->TypeInfo;
		SL_SlotMachineInfo( -1 , _SlotMachineBaseInfo , _SlotMachineTypeInfo );
	}

	delete UserObj;
}

void CNetDC_DepartmentStoreChild::OnLocalSrvConnect( int ZoneID )
{
	SL_SlotMachineInfo( ZoneID , _SlotMachineBaseInfo , _SlotMachineTypeInfo );
}