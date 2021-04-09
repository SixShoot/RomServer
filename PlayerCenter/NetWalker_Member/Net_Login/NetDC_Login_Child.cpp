#include "ReaderClass/DbSqlExecClass.h"
#include "NetDC_Login_Child.h"	
#include "../Net_ServerList/Net_ServerList_Child.h"
#include "ReaderClass/CreateDBCmdClass.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
#include "../Net_DCBase/NetDC_DCBaseChild.h"
#include "MD5/Mymd5.h"
#include <sstream>
//-------------------------------------------------------------------
bool							CNetDC_Login_Child::_IsInitRoleData_Acccount_OK = false;
map< int , DBRoleTempDataInfo >	CNetDC_Login_Child::_WaitLoadRole;
set< string >					CNetDC_Login_Child::_OnLoadAccountSet;
bool							CNetDC_Login_Child::_CheckZeroSerialCode = false;
//-------------------------------------------------------------------
bool    CNetDC_Login_Child::Init()
{
	CNetDC_Login::_Init();
	if( m_pThis != NULL)
		return false;
	m_pThis = NEW( CNetDC_Login_Child );
	_RD_NormalDB->SqlCmd( 0 ,  _SQLCmdInitProc , _SQLCmdInitProcResult , NULL , NULL );
	printf( "\n");
	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 200 );
		_RD_NormalDB->Process( );
		switch( i % 4 )
		{
		case 0:
			printf( "\rInit RoleData_Account /        " );
			break;
		case 1:
			printf( "\rInit RoleData_Account |        " );
			break;
		case 2:			
			printf( "\rInit RoleData_Account \\        " );
			break;
		case 3:
			printf( "\rInit RoleData_Account -        " );
			break;
		}
		if( _IsInitRoleData_Acccount_OK == true )
			break;
	}
	printf( "\n");
	Global::Schedular()->Push( _OnTimeProc_ , 30*60*1000, NULL , NULL );
	//因為有在thread 內使用
	//所以事先做初始化動作
	RoleDataEx::ReaderRD_ReserveRole();
	_CheckZeroSerialCode = Ini()->Int( "CheckZeroSerialCode" ) != 0;
	return true;
}
//--------------------------------------------------------------------------------------
bool    CNetDC_Login_Child::Release()
{
	if( m_pThis == NULL )
		return false;
	delete m_pThis;
	m_pThis = NULL;
	CNetDC_Login::_Release();
	return true;
}	
//--------------------------------------------------------------------------------------
//初始載入
bool 	CNetDC_Login_Child::_SQLCmdInitProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass		MyDBProc( sqlBase );
	MyDBProc.SqlCmd_WriteOneLine( "UPDATE RoleData_Account SET IsLogin = 0 WHERE IsLogin <> 0" );
	MyDBProc.SqlCmd_WriteOneLine( "UPDATE RoleData SET IsEnterWorld = 0 WHERE IsEnterWorld <>0" );
	return true;
}
void 	CNetDC_Login_Child::_SQLCmdInitProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	_IsInitRoleData_Acccount_OK = true;
}
int CNetDC_Login_Child::_OnTimeProc_( SchedularInfo* Obj , void* InputClass )
{
	char Buf[512];
	sprintf( Buf , "UPDATE RoleData Set IsDelete = 1 WHERE %d > DestroyTime and DestroyTime <> -1 and IsDelete = 0" , TimeStr::Now_Value() );
	_RD_NormalDB->SQLCMDWrite( rand() , NULL , NULL , Buf );
	Global::Schedular()->Push( _OnTimeProc_ , 30*1000*60, NULL , NULL );
	return 0;
}
//--------------------------------------------------------------------------------------
int CNetDC_Login_Child::_LoadRoleDataProc( SchedularInfo* Obj , void* InputClass )
{
	LoadAccount( (DBRoleTempDataInfo*)InputClass  );
	return 0;
}
void CNetDC_Login_Child::LoadAccount( DBRoleTempDataInfo* UserData  )
{
	if( _OnLoadAccountSet.find( UserData->Account ) != _OnLoadAccountSet.end() )
	{
		Print( LV4 , "LoadAccount" , "_OnLoadAccountSet.find('%s' ) != _OnLoadAccountSet.end()" , UserData->Account.c_str()  );
		return;
	}
	if( BaseItemObject::CheckAccountEmpty( UserData->Account ) == false )
	{
		if( BaseItemObject::CheckAccountIsLoading( UserData->Account ) == true )
		{
			Print( LV4 , "LoadAccount" , "BaseItemObject::CheckAccountIsLoading('%s') == true" , UserData->Account.c_str()  );
			return;
		}
		//把所有帳號生存時間設為 0
		vector< BaseItemObject*>* ObjList = BaseItemObject::GetObj_Account( (char*)UserData->Account.c_str() );
		if( ObjList != NULL )
		{
/*			bool IsNeedSave = false;
			//如果有角色需要儲存
			for( int i = 0 ; i < (int)ObjList->size() ; i++ )
			{
				if( (*ObjList)[i]->IsNeedSave()  == true )
					IsNeedSave = true;
			}
			if( IsNeedSave == true	)
			*/
			{
				for( int i = 0 ; i < (int)ObjList->size() ; i++ )
				{
					(*ObjList)[i]->SetLiveTime( 0 );
				}
				Print( LV2 , "LoadAccount" , "_LoadRoleDataProc Account=%s" , UserData->Account.c_str() );
				Global::Schedular()->Push( _LoadRoleDataProc , 1000, UserData , NULL );
				return;
			}
		}
	}
	_OnLoadAccountSet.insert( UserData->Account );
	_RoleDataDB->SqlCmd( rand() , _DBLoadRole , _DBLoadRoleResult , UserData , NULL );
}
void CNetDC_Login_Child::On_LoadAccount( int iServerID, int iClientID, string sAccount )
{
	//檢查帳戶自串是否合法
	if(	CheckSqlStr( sAccount.c_str() ) == false  )
	{
		LoadAccountFailed( iServerID , iClientID , EM_LoadAccountFailed_Err );
		return;
	}
	DBRoleTempDataInfo* UserData = _ReadRole->NewObj();
	if( UserData == NULL )
	{
		LoadAccountFailed( iServerID , iClientID , EM_LoadAccountFailed_Err );
		return;
	}
	UserData->Init();
	UserData->Account    = sAccount;
	UserData->SrvID      = iServerID;
	UserData->LocalObjID = iClientID;
	UserData->IpNum		 = _Net->GetIpNum( iClientID );
	_strlwr( (char*)UserData->Account.c_str() );
	LoadAccount( UserData );
	//LoadAccount( iServerID, iClientID, sAccount  );
}
bool     CNetDC_Login_Child::_DBLoadRole           ( vector<PlayerRoleData>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char	PlotName[32] = {0};
	char	CmdBuf[512];
	bool	IsFindAccountData = false;
	int		LogoutTime = 0;
	DBRoleTempDataInfo*		UserData = (DBRoleTempDataInfo*)UserObj;
	MyDbSqlExecClass		MyDBProc( sqlBase );
	//////////////////////////////////////////////////////////////////////////
	//讀取此帳號要執行的劇情
	{
		vector< string >	SqlCmdList;
		vector< int >		GUIDList;
		int		GUID;
		char	TempSqlCmd[512];
		sprintf( CmdBuf , "SELECT GUID,SqlCmd FROM LoginSqlCommandTable WHERE DestroyTime=0 and AccountID='%s' " , UserData->Account.c_str() );
		MyDBProc.SqlCmd( CmdBuf );
		MyDBProc.Bind( 1, SQL_C_LONG	, SQL_C_DEFAULT			, (LPBYTE)&GUID );
		MyDBProc.Bind( 2, SQL_C_CHAR	, sizeof(TempSqlCmd)	, (LPBYTE)&TempSqlCmd );
		while( MyDBProc.Read() )
		{
			SqlCmdList.push_back( TempSqlCmd );
			GUIDList.push_back( GUID );
		}
		MyDBProc.Close();
		for( int i = 0 ; i < (int)SqlCmdList.size() ; i++ )
		{
			MyDBProc.SqlCmd_WriteOneLine( SqlCmdList[i].c_str() );
			sprintf( CmdBuf , "UPDATE LoginSqlCommandTable SET DestroyTime = getdate() WHERE GUID = %d " , GUIDList[i] );
			MyDBProc.SqlCmd_WriteOneLine( CmdBuf );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//取得符合條件的劇情
	unsigned ConverIPNum =	( UserData->IpNum & 0xff000000 ) /0x1000000 
							+( UserData->IpNum & 0xff0000 )  /0x10000	* 0x100
							+( UserData->IpNum & 0xff00 )	 /0x100		* 0x10000
							+( UserData->IpNum & 0xff )					* 0x1000000;
	float   NowValue = TimeExchangeIntToFloat( TimeStr::Now_Value() + RoleDataEx::G_TimeZone * 60*60 );
	sprintf( CmdBuf , "SELECT PlotName FROM IP_RunPlotTable WHERE ( ( BeginTime < %f or BeginTime = 0 ) and ( EndTime > %f or EndTime = 0) and IpNum_Hight >= %u and IpNum_Low <= %u)" 
		, NowValue , NowValue , ConverIPNum , ConverIPNum );
	MyDBProc.SqlCmd( CmdBuf );
	MyDBProc.Bind( 1, SQL_C_CHAR, sizeof(PlotName) , (LPBYTE)&PlotName );
	MyDBProc.Read();
	MyDBProc.Close();
	//////////////////////////////////////////////////////////////////////////
	// Get secondpassword check history
	int CheckFailedTimes	= 0;
	int	LastCheckTime		= 0;
	sprintf( CmdBuf , "SELECT Result, CheckTime FROM SecondPasswordRecord WHERE AccountID = '%s'",  UserData->Account.c_str() );
	MyDBProc.SqlCmd( CmdBuf );
	MyDBProc.Bind( 1, SQL_C_LONG, sizeof(CheckFailedTimes),		(LPBYTE)&CheckFailedTimes );
	MyDBProc.Bind( 2, SQL_C_LONG, sizeof(LastCheckTime),		(LPBYTE)&LastCheckTime );
	MyDBProc.Read();
	MyDBProc.Close();
	//////////////////////////////////////////////////////////////////////////
	PlayerRoleData Role;	
	Role.Base.Account_ID = UserData->Account.c_str();
	sprintf( CmdBuf , "where Account_ID = '%s'" ,  UserData->Account.c_str() );
	MyDBProc.SqlCmd( _RoleDataAccountSql->GetSelectStr( CmdBuf ).c_str() );
	MyDBProc.Bind( Role , RoleDataEx::ReaderBase_Account() );
	if( MyDBProc.Read() )
		IsFindAccountData = true;
	MyDBProc.Close();
	int& PlayTimeQuota = Role.PlayerBaseData.PlayTimeQuota;
	if( PlayTimeQuota < 60*60*5 )
	{
		PlayTimeQuota += TimeStr::Now_Value() - Role.Base.LogoutTime;
		if( PlayTimeQuota > 60*60*5  )
			PlayTimeQuota = 60*60*5;
	}
	else
	{
		PlayTimeQuota = 60*60*24*30;
	}
	UserData->AccountBaseInfo.ManageLv = Role.PlayerBaseData.ManageLV;
	UserData->AccountBaseInfo.AccountMoney = Role.PlayerBaseData.Body.Money_Account;
	UserData->AccountBaseInfo.BonusMoney = Role.PlayerBaseData.Money_Bonus;
	MyStrcpy( UserData->AccountBaseInfo.Password , Role.PlayerBaseData.Password.Begin() , sizeof(UserData->AccountBaseInfo.Password) );
	UserData->AccountBaseInfo.PlayTimeQuota = PlayTimeQuota;
	UserData->AccountBaseInfo.LockAccountTime = Role.PlayerBaseData.LockAccountTime;
	memcpy( UserData->AccountBaseInfo.LockAccountMoney , Role.PlayerBaseData.LockAccountMoney , sizeof(Role.PlayerBaseData.LockAccountMoney) );
	UserData->AccountBaseInfo.LockAccountMoney_Forever = Role.PlayerBaseData.LockAccountMoney_Forever;
	UserData->AccountBaseInfo.AccountFlag = Role.Base.AccountFlag;
	UserData->AccountBaseInfo.VipMemberType = Role.PlayerBaseData.VipMember.Type;
	UserData->AccountBaseInfo.VipMemberTerm = Role.PlayerBaseData.VipMember.Term;
	UserData->AccountBaseInfo.VipMemberFlag = Role.PlayerBaseData.VipMember._Flag;
	UserData->AccountBaseInfo.UntrustFlag._Flag = Role.PlayerBaseData.VipMember.UntrustFlag._Flag;
	UserData->AccountBaseInfo.CheckFailedTimes	= CheckFailedTimes;
	UserData->AccountBaseInfo.LastCheckTime		= LastCheckTime;
	memcpy( UserData->AccountBaseInfo.CoolClothList , Role.PlayerBaseData.CoolClothList , sizeof( UserData->AccountBaseInfo.CoolClothList ) );
    //////////////////////////////////////////////////////////////////////////
	if( IsFindAccountData == false )
	{
		char Buf[512];
		sprintf( Buf , "EXECUTE CreateAccount '%s'", UserData->Account.c_str() );	
		MyDBProc.SqlCmd_WriteOneLine( Buf );
	}
	else
	{
		//檢查CheckSum
		int AccountSerialCode = 0;
		sprintf( CmdBuf , "SELECT SerialCode FROM RoleData_Account WHERE Account_ID = '%s'",  UserData->Account.c_str() );
		MyDBProc.SqlCmd( CmdBuf );
		MyDBProc.Bind( 1, SQL_C_LONG	, SQL_C_DEFAULT			, (LPBYTE)&AccountSerialCode );
		MyDBProc.Read();
		MyDBProc.Close();
		if( AccountSerialCode != Role.AccountSerialCode() )
		{
			Print( LV4 , "_DBLoadRole" , "account money checksum err , dbSerialCode =%d roleSerialCode=%d Account_ID=%s" , AccountSerialCode , Role.AccountSerialCode() , UserData->Account.c_str() );
			sprintf( CmdBuf , "UPDATE RoleData_Account Set SerialCode =%d where Account_ID = '%s'" 
				, Role.AccountSerialCode() , Role.Base.Account_ID.Begin() );
			MyDBProc.SqlCmd_WriteOneLine( CmdBuf );
		}
	}
	//////////////////////////////////////////////////////////////////////////
#if CHECK_PLAYER_DATA_HASH_CODE
    if (AccountDataHashCodeCheck(UserData->Account.c_str(), Role.PlayerBaseData.Body.Money_Account, sqlBase) == false)
    {
        Print(LV3, "CNetDC_Login_Child", "AccountDataHashCodeCheck(%s) Failed!\n", UserData->Account.c_str());
    }
#endif
    //////////////////////////////////////////////////////////////////////////
	sprintf( CmdBuf , " where Account_id = '%s' and IsDelete = 0 " , UserData->Account.c_str() );
	MyDBProc.SqlCmd( _RoleDataSql->GetSelectStr(CmdBuf).c_str() );
	Role.PlayerBaseData.Bank.Money_Account = 0;
	Role.SelfData.AutoPlot = PlotName;
	MyDBProc.Bind( Role , RoleDataEx::ReaderBase() );
	while( MyDBProc.Read() )
	{			
		RoleDataEx::ReaderBase()->TransferWChartoUTF8( &Role , MyDBProc.wcTempBufList() );
		Data.push_back( Role );
		//Data.back().SetLink();
	}
	MyDBProc.Close();
	char Buf[256];
	for (int i = (int)Data.size() - 1 ; i >= 0 ; --i)
	{
		if (    Data[i].PlayerBaseData.DestroyTime == -1
                || (unsigned int)Data[i].PlayerBaseData.DestroyTime > TimeStr::Now_Value()  )
        {
			continue;
        }
		sprintf( Buf , "Update RoleData Set IsDelete = 1 where DBID = %d and Account_ID = '%s'", Data[i].Base.DBID , UserData->Account.c_str() );
		MyDBProc.SqlCmd_WriteOneLine( Buf );
		Data.erase( Data.begin() + i );
	}
	for (unsigned int i = 0 ; i < Data.size(); ++i)
	{
		Data[i].SetLink();
#if CHECK_PLAYER_DATA_HASH_CODE
        if (PlayerDataHashCodeCheck(UserData->Account.c_str(), Data[i].Base.DBID, Data[i].PlayerBaseData.Body.Money, sqlBase) == false)
        {
            Print(LV3, "CNetDC_Login_Child", "PlayerDataHashCodeCheck(%d) Failed!\n", Data[i].Base.DBID);
        }
#endif
	}
	sprintf( Buf , "UPDATE RoleData_Account SET IsLogin = 1 WHERE Account_ID = '%s'", UserData->Account.c_str() );
	MyDBProc.SqlCmd_WriteOneLine( Buf );
	return true;
}
void     CNetDC_Login_Child::_DBLoadRoleResult     ( vector<PlayerRoleData>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	char Buf[256];
	DBRoleTempDataInfo UserData = *(DBRoleTempDataInfo*)UserObj;
	_OnLoadAccountSet.erase( UserData.Account );
	_ReadRole->DeleteObj( (DBRoleTempDataInfo*)UserObj );
	if( ResultOK == false )
	{
		Print( Def_PrintLV2 ,"_DBReadRoleDBEventCB" , "存取資料庫錯誤!!" );
		return;
	}
	BaseItemObject* Obj;
	PlayerRoleData* Role;
	int i;
	ReplyRoleDataCount( UserData.SrvID , UserData.LocalObjID , (int) Data.size() , UserData.Account.c_str() , UserData.AccountBaseInfo );
	for( i = 0 ; i < (int)Data.size() ; i++ )
	{
		DBRoleTempDataInfo* RoleTempData = _ReadRole->NewObj();
		if( RoleTempData == NULL )
		{
			Print( Def_PrintLV2 , "_DBReadRoleDBEventCB" , " _ReadRole->NewObj() 失敗" );
			break;
		}
		RoleTempData->Init();
		*RoleTempData = UserData;
		Role = &Data[i];        
		Role->SetLink();
		//檢查角色是否存在
		BaseItemObject* CheckObj = BaseItemObject::GetObj_DBID( Role->Base.DBID );
		if( CheckObj == NULL )
		{
			Obj = NEW BaseItemObject( Role );
			RoleTempData->DBID = Role->Base.DBID;
			RoleTempData->Obj = Obj;
			Obj->PushToThread();
			sprintf( Buf , "where DBID = %d " , Role->Base.DBID );
			_RD_ItemDB->Read( _DBReadItemDBEventCB , RoleTempData , Buf );
		}
		else
		{
			if( stricmp( Role->Base.Account_ID.Begin() , CheckObj->Role()->Base.Account_ID.Begin() ) != 0 )
			{
				Print( LV5 , "_DBLoadRoleResult" , "stricmp( Role->Base.Account_ID.Begin() , CheckObj->Role()->Base.Account_ID.Begin() ) " );
			}
			else
			{
				CheckObj->SetLiveTime( 60*60*1000*24 );
				if( CheckObj->IsLoadOK() == true )
				{
					ReplyRoleData( UserData.SrvID, UserData.LocalObjID , Role->Base.DBID , CheckObj->Role() );
				}
				else
				{
					_WaitLoadRole[ Role->Base.DBID ] = UserData;
				}
				_ReadRole->DeleteObj( RoleTempData );
			}
		}
	}
}
//--------------------------------------------------------------------------------------
void    CNetDC_Login_Child::_DBReadItemDBEventCB    ( vector<DB_ItemFieldStruct>& Data , void* UserObj , char *WhereCmd , bool ResultOK )
{
	char Buf[256];
	DBRoleTempDataInfo* UserData = (DBRoleTempDataInfo*)UserObj;
	BaseItemObject* Obj = UserData->Obj;
	Obj->PopFromThread();
	if( Obj == NULL )
	{
		_ReadRole->DeleteObj( UserData );
		return;
	}
	PlayerRoleData* Role = Obj->Role();
	//_ReadRole->DeleteObj( (DBRoleTempDataInfo*)UserObj );
	if( ResultOK == false )
	{
		Print( Def_PrintLV2 , "_DBReadItemDBEventCB" , "存取資料庫錯誤!!" );
		return;
	}    
	//printf( "\n(**) Select DBID = %d Item Size = %d" , Role->Base.DBID , Data.size() );
	Print( Def_PrintLV1 ,"_DBReadItemDBEventCB" , "Select DBID = %d Item Size = %d" , Role->Base.DBID , Data.size()  );
	Role->PlayerBaseData.ItemTemp.Clear();
	for( int i = 0 ; i < (int)Data.size() ; i++ )
	{
		DB_ItemFieldStruct* Temp = &Data[i];
		Temp->Info.Pos = EM_ItemState_NONE;
		switch( Temp->FieldType )
		{
		case EM_ItemFieldType_Body:
			if( Temp->FieldID >= _MAX_BODY_COUNT_ )
				break;
			Role->PlayerBaseData.Body.Item[ Temp->FieldID ] = Temp->Info;
			break;
		case EM_ItemFieldType_Bank:
			if( Temp->FieldID >= _MAX_BANK_COUNT_ )
				break;
			Role->PlayerBaseData.Bank.Item[ Temp->FieldID ] = Temp->Info;
			break;
		case EM_ItemFieldType_EQ:
			if( Temp->FieldID >= EM_EQWearPos_MaxCount )
				break;
			Role->BaseData.EQ.Item[ Temp->FieldID ] = Temp->Info;
			break;
		case EM_ItemFieldType_DestroyItem:
			if( Temp->FieldID >= DEF_MAX_DESTROY_ITEM_LOG_COUNT )
				break;
			Role->PlayerSelfData.DestroyItemLog.ItemList[ Temp->FieldID ] = Temp->Info;
			break;
		case EM_ItemFieldType_TempItem:
			if( Temp->FieldID >= _MAX_TEMP_COUNT_ )
				break;
			if( Temp->Info.IsEmpty() || Temp->FieldID != Role->PlayerBaseData.ItemTemp.Size() )
				continue;
			Role->PlayerBaseData.ItemTemp.Push_Back( Temp->Info );
			break;
		case EM_ItemFieldType_EQBackup:
			{
				unsigned bkID = Temp->FieldID / _DEF_EQ_BACKUP_BASE_;
				unsigned posID = Temp->FieldID % _DEF_EQ_BACKUP_BASE_;
				if(		posID >= EM_EQWearPos_MaxCount 
					||	bkID >= _MAX_BACKUP_EQ_SET_COUNT_ )
					break;
				Role->PlayerBaseData.EQBackup[bkID].Item[ posID ] = Temp->Info;
			}
			break;
		case EM_ItemFieldType_Pet:	//寵物欄位
			{
				int PetPos = Temp->FieldID / 100;
				int FieldID = Temp->FieldID % 100;
				if(		FieldID >= EM_CultivatePetEQType_MAX  
					||	PetPos >= MAX_CultivatePet_Count)
					break;
				Role->PlayerBaseData.Pet.Base[PetPos].EQ[FieldID] = Temp->Info;
			}
			break;
		}
	}
	Obj->PushToThread();
	sprintf( Buf , "where DBID = %d " , Role->Base.DBID );
	_RD_AbilityDB->Read( _DBReadAbilityDBEventCB , UserData , Buf );
}
//--------------------------------------------------------------------------------------
void    CNetDC_Login_Child::_DBReadAbilityDBEventCB    ( vector<DB_AbilityStruct>& Data , void* UserObj , char *WhereCmd , bool ResultOK )
{
	DBRoleTempDataInfo* UserData = (DBRoleTempDataInfo*)UserObj;
	BaseItemObject* Obj = UserData->Obj;
	Obj->PopFromThread();    
	if( Obj == NULL )
	{
		_ReadRole->DeleteObj( UserData );
		return;
	}
	PlayerRoleData* Role = Obj->Role();
	//_ReadRole->DeleteObj( UserData );
	if( ResultOK == false )
	{
		Print( Def_PrintLV2 , "_DBReadAbilityDBEventCB" , "存取資料庫錯誤!!" );
		return;
	}    
	//	printf( "\n(**) Select DBID = %d Ability Size = %d" , Role->Base.DBID , Data.size() );
	Print( Def_PrintLV1 , "_DBReadAbilityDBEventCB" , "Select DBID = %d Ability Size = %d" , Role->Base.DBID , Data.size()  );
	for( int i = 0 ; i < (int)Data.size() ; i++ )
	{
		DB_AbilityStruct* Temp = &Data[i];
		Role->PlayerBaseData.AbilityList[Temp->Job] = Temp->Info;
	}
	//ReplyRoleData( UserData->SrvID, UserData->LocalObjID , Role->Base.DBID , Role );
	Obj->PushToThread();
	//sprintf( Buf , "where DBID = %d " , Role->Base.DBID );
	//_RD_FriendDB->Read( _DBReadFriendDBEventCB , UserData , Buf );
	_RD_NormalDB->SqlCmd( Role->Base.DBID , _DBLoadOtherDB , _DBLoadOtherDBeResult , UserData , NULL  );
}
bool CNetDC_Login_Child::_DBLoadOtherDB( vector<int>& Data, SqlBaseClass* sqlBase, void* UserObj , ArgTransferStruct& Arg )
{
	//////////////////////////////////////////////////////////////////////////
	//讀取好友資料
	//////////////////////////////////////////////////////////////////////////
	char Buf[256];
	DBRoleTempDataInfo* UserData = (DBRoleTempDataInfo*)UserObj;
	BaseItemObject* Obj = UserData->Obj;
	PlayerRoleData*		Role = Obj->Role();
	MyDbSqlExecClass	MyDBProc( sqlBase );
	{
		sprintf( Buf , "where OwnerDBID = %d " , Role->Base.DBID );
		MyDBProc.SqlCmd( _RDFriendSql->GetSelectStr( Buf ).c_str() );
		DB_BaseFriendStruct TempFriendDB;
		MyDBProc.Bind( TempFriendDB , RoleDataEx::ReaderRD_BaseFriendStruct() );
		while( MyDBProc.Read() )
		{
			RoleDataEx::ReaderRD_BaseFriendStruct()->TransferWChartoUTF8( &TempFriendDB , MyDBProc.wcTempBufList() );
			UserData->FriendList.push_back( TempFriendDB );
		}
		MyDBProc.Close();
	}
	//////////////////////////////////////////////////////////////////////////
	//讀取寵物資料
	//////////////////////////////////////////////////////////////////////////
	{
		DB_CultivatePetStruct TempPetDB;
		sprintf( Buf , "where PlayerDBID = %d " , Role->Base.DBID );
		MyDBProc.SqlCmd( _RDCultivatePetSql->GetSelectStr( Buf ).c_str() );
		MyDBProc.Bind( TempPetDB , RoleDataEx::ReaderRD_CultivatePetStruct() );
		while( MyDBProc.Read() )
		{
			RoleDataEx::ReaderRD_CultivatePetStruct()->TransferWChartoUTF8( &TempPetDB , MyDBProc.wcTempBufList() );
			UserData->PetList.push_back( TempPetDB );
		}
		MyDBProc.Close();
	}
	//////////////////////////////////////////////////////////////////////////
	{
		//檢查CheckSum
		int SerialCode1 = 0;
		int SerialCode = 0;
		int SerialCode_All = 0;
		sprintf( Buf , "SELECT SerialCode1 , SerialCode_All , SerialCode  FROM RoleData WHERE DBID = %d",  Role->Base.DBID );
		MyDBProc.SqlCmd( Buf );
		MyDBProc.Bind( 1, SQL_C_LONG	, SQL_C_DEFAULT			, (LPBYTE)&SerialCode1 );
		MyDBProc.Bind( 2, SQL_C_LONG	, SQL_C_DEFAULT			, (LPBYTE)&SerialCode_All );
		MyDBProc.Bind( 3, SQL_C_LONG	, SQL_C_DEFAULT			, (LPBYTE)&SerialCode );
		MyDBProc.Read();
		MyDBProc.Close();
		bool checkSumErr = false;
		int RoleSerialCode = Role->SerialCode();
		int RoleSerialCode1 = Role->SerialCode1();
		int RoleSerialCode_All = Role->SerialCode_All();	
		Role->BaseData.SysFlag.MoneySerialCodeError = false;
		if(		SerialCode1 != RoleSerialCode1  
			||	SerialCode != RoleSerialCode )
		{
			Print( LV4 , "_DBLoadOtherDB" , "money checksum err , dbSerialCode1=%d dbSerialCode1=%d roleSerialCode=%d roleSerialCode1=%d DBID=%d" , SerialCode1 , SerialCode , RoleSerialCode , RoleSerialCode1 , Role->Base.DBID );
			if(	SerialCode1 != RoleSerialCode1 )
			{
				if( SerialCode1 || _CheckZeroSerialCode )
					Role->BaseData.SysFlag.MoneySerialCodeError = true;
			}
		}
		if( SerialCode_All != RoleSerialCode_All )
		{
			Print( LV4 , "_DBLoadOtherDB" , "data checksum err , dbSerialCode=%d roleSerialCode=%d DBID=%d" , SerialCode_All , RoleSerialCode_All , Role->Base.DBID );
			sprintf( Buf , "UPDATE RoleData Set SerialCode_All = %d where DBID = %d" 
				, RoleSerialCode_All , Role->Base.DBID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
	}
	return true;
}
void CNetDC_Login_Child::_DBLoadOtherDBeResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	DBRoleTempDataInfo* UserData = (DBRoleTempDataInfo*)UserObj;
	BaseItemObject* Obj = UserData->Obj;
	Obj->PopFromThread();    
	if( Obj == NULL || ResultOK == false )
	{
		_ReadRole->DeleteObj( UserData );
		return;
	}
	PlayerRoleData* Role = Obj->Role();
	/*if( ResultOK == false )
	{
		Print( Def_PrintLV2 , "_DBReadFriendDBEventCB" , "存取資料庫錯誤!!" );
		return;
	}    */
	Print( Def_PrintLV1 , "_DBReadFriendDBEventCB" , "Select OwnerDBID = %d FriendDB Size = %d" , Role->Base.DBID , UserData->FriendList.size() );
	memset( &(Role->PlayerSelfData.FriendList.AllFriendList) , 0 , sizeof( Role->PlayerSelfData.FriendList.AllFriendList ) );
	//////////////////////////////////////////////////////////////////////////
	for( int i = 0 ; i < (int)UserData->FriendList.size() ; i++ )
	{
		DB_BaseFriendStruct* Temp = &UserData->FriendList[i];
		Temp->Info.IsSave = true;
		switch( Temp->Type )
		{
		case EM_FriendListType_Friend:
			if( (unsigned)Temp->FieldID > _MAX_FRIENDLIST_COUNT_ )
				continue;
			Role->PlayerSelfData.FriendList.FriendList[ Temp->FieldID ] = Temp->Info;
			break;
		case EM_FriendListType_HateFriend:
			if( (unsigned)Temp->FieldID > _MAX_HATE_PLAYER_LIST_COUNT_ )
				continue;
			Role->PlayerSelfData.FriendList.HateFriendList[ Temp->FieldID ] = Temp->Info;
			break;
		case EM_FriendListType_FamilyFriend:
			if( (unsigned)Temp->FieldID > _MAX_FRIENDLIST_FAMILY_COUNT_ )
				continue;
			Role->PlayerSelfData.FriendList.FamilyList[ Temp->FieldID ] = Temp->Info;
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//寵物資料
	//////////////////////////////////////////////////////////////////////////
	for( int i = 0 ; i < (int)UserData->PetList.size() ; i++ )
	{		
		DB_CultivatePetStruct* Temp = &UserData->PetList[i];
		if( Temp->Info.IsEmpty() )
			continue;
		if( (unsigned) Temp->Pos >= MAX_CultivatePet_Count )
		{
			Print( LV3 , "_DBLoadOtherDBeResult" , "Temp->Pos = %d PlayerDBID=%d , pos error" , Temp->Pos , Temp->PlayerDBID );
			continue;
		}
		//Role->PlayerBaseData.Pet.Base[Temp->Pos] = Temp->Info;
		memcpy( Role->PlayerBaseData.Pet.Base[Temp->Pos]._Value , Temp->Info._Value , sizeof(Temp->Info._Value) );
		Role->PlayerBaseData.Pet.Base[Temp->Pos].Name = Temp->Info.Name;
	}
	//////////////////////////////////////////////////////////////////////////
	//Obj->RoleDB()->Copy( Role );
	Obj->SetRoleDBZip( Role );
	Obj->IsLoadOK( true );
	map< int , DBRoleTempDataInfo >::iterator Iter = _WaitLoadRole.find( Role->Base.DBID );
	if( Iter != _WaitLoadRole.end() )
	{
		ReplyRoleData( Iter->second.SrvID, Iter->second.LocalObjID , Role->Base.DBID , Role );
		_WaitLoadRole.erase( Iter );
	}
	else
	{
		ReplyRoleData( UserData->SrvID, UserData->LocalObjID , Role->Base.DBID , Role );
	}
	_ReadRole->DeleteObj( UserData );
}
/*
void CNetDC_Login_Child::_DBReadFriendDBEventCB  ( vector<DB_BaseFriendStruct>& Data , void* UserObj , char *WhereCmd , bool ResultOK )
{
DBRoleTempDataInfo* UserData = (DBRoleTempDataInfo*)UserObj;
BaseItemObject* Obj = UserData->Obj;
Obj->PopFromThread();    
if( Obj == NULL )
{
_ReadRole->DeleteObj( UserData );
return;
}
for( int i = 0 ; i < Data.size() ; i++ )
{
//RoleDataEx::ReaderRD_ACBase()->TransferWChartoUTF8( &TempACData );
RoleDataEx::ReaderRD_BaseFriendStruct()->TransferWChartoUTF8( &Data[i] ); 
}
RoleDataEx* Role = Obj->Role();
_ReadRole->DeleteObj( UserData );
if( ResultOK == false )
{
Print( Def_PrintLV2 , "_DBReadFriendDBEventCB" , "存取資料庫錯誤!!" );
return;
}    
//printf( "\n(**) Select DBID = %d FriendDB Size = %d" , Role->Base.DBID , Data.size() );
Print( Def_PrintLV2 , "_DBReadFriendDBEventCB" , "Select DBID = %d FriendDB Size = %d" , Role->Base.DBID , Data.size() );
memset( &(Role->PlayerSelfData.FriendList.FriendList) , 0 , sizeof( Role->PlayerSelfData.FriendList.FriendList ) );
for( int i = 0 ; i < (int)Data.size() ; i++ )
{
DB_BaseFriendStruct* Temp = &Data[i];
Temp->Info.IsSave = true;
Role->PlayerSelfData.FriendList.FriendList[ Temp->FieldID ] = Temp->Info;
}	
memcpy( Obj->RoleDB() , Role, sizeof(*Role) );
ReplyRoleData( UserData->SrvID, UserData->LocalObjID , Role->Base.DBID , Role );
}
*/
//--------------------------------------------------------------------------------------
void CNetDC_Login_Child::On_CreateRole( int iServerID, int iClientID, string sAccountName, PlayerRoleData* t_pRole )
{
	//檢查角色名是否有問題
	if(		CheckSqlStr( t_pRole->BaseData.RoleName.Begin() ) == false 
		||	CheckSqlStr( sAccountName.c_str() ) == false  )
	{
		CreateRoleResult( iServerID , iClientID , EM_CreateRoleResult_RoleNameExist , t_pRole->BaseData.RoleName.Begin() , t_pRole->Base.DBID );
		return;
	}
	DBRoleTempDataInfo* UserData = _ReadRole->NewObj();
	if( UserData == NULL )
	{
		Print( LV3 , "On_CreateRole" , "UserData == NULL 資料有問題" );
		CreateRoleResult( iServerID , iClientID , EM_CreateRoleResult_SystemError , t_pRole->BaseData.RoleName.Begin() , t_pRole->Base.DBID );
		return;
	}
	UserData->Init();
	PlayerRoleData* Role = _RoleBin->NewObj();
	UserData->Role   = Role;
	if( UserData->Role == NULL )
	{
		_ReadRole->DeleteObj( UserData );
		Print( LV3 , "On_CreateRole" ,"UserData->Role == NULL 資料有問題" );
		CreateRoleResult( iServerID , iClientID , EM_CreateRoleResult_SystemError , t_pRole->BaseData.RoleName.Begin() , t_pRole->Base.DBID );
		return;
	}
	*Role			= *t_pRole;
	Role->Base.CreateTime = TimeStr::Now();
	BaseAbilityStruct*		Ability;
	Ability = Role->TempData.Attr.Ability = &Role->PlayerBaseData.AbilityList[ Role->BaseData.Voc ];
	UserData->Account    		= sAccountName;
	UserData->SrvID      		= iServerID;
	UserData->LocalObjID 		= iClientID;
	UserData->Field      		= Role->Base.CharField;
	UserData->CheckNameResult	= EM_CheckRoleResult_None;
	_RD_ImportDB->SqlCmd( rand() , _DBCheckRoleName , _DBCheckRoleNameResult , &(UserData->CheckNameResult) 
		, "Name" , EM_ValueType_String , t_pRole->BaseData.RoleName.Begin()
		, NULL );
	_RoleDataDB->SqlCmd( 5 , _DBCreateRole , _DBCreateRoleResult , this , "Data" , EM_ValueType_UInt64 , (UINT64)UserData , NULL );
}
bool CNetDC_Login_Child::_DBCreateRole      ( vector<PlayerRoleData>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	int			TimeBegin = timeGetTime();
	//MutilThread_CritSect	Thread_CritSect;
	CNetDC_Login_Child* This		= (CNetDC_Login_Child*)UserObj;
	DBRoleTempDataInfo *UserData = (DBRoleTempDataInfo *)Arg.GetUINT64("Data");
	PlayerRoleData* Role			= UserData->Role;
	char                Buf[8192];
	int                 DBID		= -1;
	bool                Result		= true;
	for( int i = 0 ; i < 1000 ; i++ )
	{
		if( UserData->CheckNameResult != EM_CheckRoleResult_None )
			break;
		Sleep( 1000 );
	}
	if( UserData->CheckNameResult == EM_CheckRoleResult_Failed )
	{
		Arg.PushValue( "Result" ,  IntToStr( EM_CreateRoleResult_RoleNameExist ) );
		return false;
	}
	MyDbSqlExecClass	MyDBProc( sqlBase );
	CharTransferClass	CharTransfer;
	CharTransfer.SetUtf8String( Role->BaseData.RoleName.Begin() );
	string RoleNameBinStr = StringToSqlX( (char*)CharTransfer.GetWCString() , CharTransfer.WCStrLen() * 2 , false );
	////////////////////////////////////////////////////////////////////////////////////
	{
		char	SqlCmd[1024];
		char	DBAccount[64];
		bool	IsReadOK = false;
		sprintf( SqlCmd , "SELECT Account_ID , GiftItemID1,GiftItemID2,GiftItemID3,GiftItemID4,GiftItemID5,GiftItemCount1,GiftItemCount2,GiftItemCount3,GiftItemCount4,GiftItemCount5,Job,Job_Sub,Lv,Lv_Sub FROM ReserveNameTable  WHERE RoleName = CAST( %s AS nvarchar(4000) ) and DestroyTime = 0" , RoleNameBinStr.c_str() );
		MyDBProc.SqlCmd( SqlCmd );
		//MyDBProc.Bind( TempReserve , RoleDataEx::ReaderRD_ReserveRole() );
		MyDBProc.Bind( 1, SQL_C_CHAR	, sizeof(DBAccount)	, (LPBYTE)&DBAccount );
		MyDBProc.Bind( 2, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&UserData->GiftItemID[0] );
		MyDBProc.Bind( 3, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&UserData->GiftItemID[1] );
		MyDBProc.Bind( 4, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&UserData->GiftItemID[2] );
		MyDBProc.Bind( 5, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&UserData->GiftItemID[3] );
		MyDBProc.Bind( 6, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&UserData->GiftItemID[4] );
		MyDBProc.Bind( 7, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&UserData->GiftItemCount[0] );
		MyDBProc.Bind( 8, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&UserData->GiftItemCount[1] );
		MyDBProc.Bind( 9, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&UserData->GiftItemCount[2] );
		MyDBProc.Bind( 10, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&UserData->GiftItemCount[3] );
		MyDBProc.Bind( 11, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&UserData->GiftItemCount[4] );
		MyDBProc.Bind( 12, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&UserData->Job );
		MyDBProc.Bind( 13, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&UserData->Job_Sub );
		MyDBProc.Bind( 14, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&UserData->Lv );
		MyDBProc.Bind( 15, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&UserData->Lv_Sub );
		if( MyDBProc.Read() )
			IsReadOK = true;
		MyDBProc.Close();
		if( IsReadOK == true )
		{
			if( stricmp( UserData->Account.c_str() , DBAccount ) != 0 )
			{
				Arg.PushValue( "Result" ,  IntToStr( EM_CreateRoleResult_RoleNameExist ) );
				return false;
			}
		}
		//sprintf( SqlCmd , "DELETE ReserveNameTable  WHERE RoleName = CAST( %s AS nvarchar(4000) )" , RoleNameBinStr.c_str() );
		sprintf( SqlCmd , "UPDATE ReserveNameTable Set DestroyTime = getdate() WHERE RoleName = CAST( %s AS nvarchar(4000) )" , RoleNameBinStr.c_str() );
		MyDBProc.SqlCmd_WriteOneLine( SqlCmd );
	}
	////////////////////////////////////////////////////////////////////////////////////
	//	g_CritSect()->Enter();
	{
		//		MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );
		//1 搜尋是否有重覆的名稱
		sprintf( Buf , "SELECT DBID FROM RoleData WHERE RoleName = CAST( %s AS nvarchar(4000) ) and IsDelete = 0 " , RoleNameBinStr.c_str() );
		MyDBProc.SqlCmd( Buf );
		MyDBProc.Bind(  1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&DBID  );
		MyDBProc.Read();
		MyDBProc.Close();
		if(  DBID != -1 )
		{
			Arg.PushValue( "Result" ,  IntToStr( EM_CreateRoleResult_RoleNameExist ) );
			Result = false;
		}
		else
		{
			Result = true;
			//2 資料儲存
			MyDBProc.SqlCmd_WriteOneLine( _RoleDataSql->GetInsertStr( Role ).c_str() );
		}
		//		MyDBProc.SqlCmd_WriteOneLine( "COMMIT TRANSACTION" );
	}
	//	g_CritSect()->Leave();
    if( Result == false )
    {
        return false;
    }
    //3 搜尋出DBID
    sprintf( Buf , "SELECT DBID FROM RoleData WHERE Account_ID = '%s' and RoleName = CAST( %s AS nvarchar(4000) ) and IsDelete = 0" 
		, Role->Base.Account_ID.Begin()
		, RoleNameBinStr.c_str() );
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1 , SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&DBID );
	Result = MyDBProc.Read();
	MyDBProc.Close();
    if( DBID == -1 || Result == false )
    {
        Arg.PushValue( "Result" ,  IntToStr( EM_CreateRoleResult_SystemError ) );
        return false;
    }
	//
    Role->Base.DBID = DBID;
	//sprintf( Buf , "EXECUTE CreateItem %d",DBID );	
	//MyDBProc.SqlCmd_WriteOneLine( Buf );
/*
	//建立角色屬性資料
	for( int i = 0 ; i < EM_Max_Vocation ; i++ )
	{
		sprintf( Buf , "Insert RoleData_Ability(DBID,Job) VALUES (%7d,%2d);",DBID , i );	
		MyDBProc.SqlCmd_WriteOneLine( Buf );
	}
*/
	sprintf( Buf , "EXECUTE CreateAbility %d",DBID );	
	MyDBProc.SqlCmd_WriteOneLine( Buf );
//	sprintf( Buf , "INSERT RoleDataEx(DBID) VALUES( %d )",DBID );	
//	MyDBProc.SqlCmd_WriteOneLine( Buf );
	if( Result == true )
	{
		Arg.PushValue( "Result" ,  IntToStr( EM_CreateRoleResult_OK ) );	
		printf( "\n(**) Create角色　Time = %d", timeGetTime() - TimeBegin );
		return true;
	}
	else
	{
		Arg.PushValue( "Result" ,  IntToStr( EM_CreateRoleResult_SystemError ) );
		//刪除資料庫資訊
		sprintf( Buf , "Delete RoleData where DBID = %d" , DBID );	
		MyDBProc.SqlCmd_WriteOneLine( Buf );
		return false;
	}
}
void CNetDC_Login_Child::_DBCreateRoleResult( vector<PlayerRoleData>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
    CNetDC_Login_Child* This = (CNetDC_Login_Child*)UserObj;
	DBRoleTempDataInfo *UserData = (DBRoleTempDataInfo *)Arg.GetUINT64("Data");
    PlayerRoleData*     Role = UserData->Role;
	EM_CreateRoleResult Result = (EM_CreateRoleResult )Arg.GetNumber("Result");
	if( ResultOK == true )
	{
		BaseItemObject* Obj = NEW BaseItemObject( Role );
		if( Obj->Role() == NULL )
			return;
		Obj->IsNeedSave( true );
		Obj->SaveTime( 60*10*1000 );
		Obj->IsLoadOK( true );
		//memset( Obj->RoleDB() , 0 , sizeof( PlayerRoleData ) );
		//Obj->RoleDB()->Init();
		PlayerRoleData  tempRole;
		tempRole.Init();
		Obj->SetRoleDBZip( &tempRole );
		EM_CreateRoleResult Result = (EM_CreateRoleResult)Arg.GetNumber( "Result" );
		CreateRoleResult( UserData->SrvID , UserData->LocalObjID , Result , Role->BaseData.RoleName.Begin(), Role->Base.DBID , UserData->GiftItemID , UserData->GiftItemCount , UserData->Job , UserData->Job_Sub , UserData->Lv , UserData->Lv_Sub );    
	}
	else //???資料庫是否有問題
	{
		CreateRoleResult( UserData->SrvID , UserData->LocalObjID , Result , Role->BaseData.RoleName.Begin(), -1 );
	}
	This->_ReadRole->DeleteObj( UserData );
	This->_RoleBin->DeleteObj( Role );
}
//--------------------------------------------------------------------------------------
void CNetDC_Login_Child::On_DeleteRole( int iServerID, int iClientID, string sAccountName, int iDBID, string sRoleName , bool IsImmediately )
{
    //簡查此角色是否存在記憶體
    BaseItemObject* Obj = BaseItemObject::GetObj_DBID( iDBID );
    if( Obj == NULL )
    {
        Print( Def_PrintLV2 , "On_DeleteRole" , "角色不存在DBCenter !!" );
        DeleteRoleResult( iServerID , iClientID , EM_RoleNotExist , 0 );
        return;
    }
    if( stricmp( Obj->Role()->Base.Account_ID.Begin() , (char*) sAccountName.c_str() ) != 0 )
    {
        Print( Def_PrintLV2 , "On_DeleteRole" , "帳號比對有問題 !!" );
        DeleteRoleResult( iServerID , iClientID , EM_RoleNotExist , 0 );
        return;
    }
	if( IsImmediately == true )
		Obj->Destroy();
    _RoleDataDB->SqlCmd( iServerID , _DBDeleteRole , _DBDeleteRoleResult , this 
                , "ServerID"   , EM_ValueType_Int       , iServerID
                , "ClientID"   , EM_ValueType_Int       , iClientID
                , "Account_ID" , EM_ValueType_String    , sAccountName.c_str()
                , "DBID"       , EM_ValueType_Int       , iDBID 
                , "Field"      , EM_ValueType_Int       , Obj->Role()->Base.CharField
				, "IsImmediately", EM_ValueType_Int     , (int)IsImmediately
                , NULL );
}
bool CNetDC_Login_Child::_DBDeleteRole( vector<PlayerRoleData>& , SqlBaseClass* sqlBase, void* UserObj , ArgTransferStruct& Arg )
{
    int     DBID			= Arg.GetNumber( "DBID" );
    char*   Account_ID		= Arg.GetString( "Account_ID" );
	int		IsImmediately	= Arg.GetNumber( "IsImmediately" );
	MyDbSqlExecClass		MyDBProc( sqlBase );
    char Buf[256];
	if( IsImmediately != 0 )
		sprintf( Buf , "Update RoleData Set IsDelete = 1 where DBID = %d and Account_ID = '%s'", DBID , Account_ID );
	else
		sprintf( Buf , "Update RoleData Set DestroyTime=%d where DBID = %d and Account_ID = '%s'", TimeStr::Now_Value() + _DEL_ROLE_RESERVE_TIME_ , DBID , Account_ID );
	if( MyDBProc.SqlCmd_WriteOneLine( Buf ) == false )
	{
		Arg.PushValue( "Result" ,  IntToStr( EM_RoleNotExist ) );
		return false;
	}
    Arg.PushValue( "Result" ,  IntToStr( EM_DeleteRole_OK ) );
    return true;
}
void CNetDC_Login_Child::_DBDeleteRoleResult( vector<PlayerRoleData>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
    int     ServerID   = Arg.GetNumber( "ServerID" );
    int     ClientID   = Arg.GetNumber( "ClientID" );
    int     Field      = Arg.GetNumber( "Field" );
    int     DBID       = Arg.GetNumber( "DBID" );
    EM_DeleteRoleResult Result = (EM_DeleteRoleResult)Arg.GetNumber( "Result" );
    DeleteRoleResult( ServerID , ClientID , Result , DBID );
/*    BaseItemObject* Obj = BaseItemObject::GetObj_DBID( DBID );
    if( Obj != NULL )
        Obj->Destroy();
*/
}
void CNetDC_Login_Child::On_DelRoleRecoverRequest( int NetID , int ProxyID , int DBID )
{
	//簡查此角色是否存在記憶體
	BaseItemObject* Obj = BaseItemObject::GetObj_DBID( DBID );
	if( Obj == NULL )
	{
		Print( Def_PrintLV2 , "On_DelRoleRecoverRequest" , "角色不存在DBCenter !!" );
		DelRoleRecoverResult( NetID , ProxyID , DBID , false );
		return;
	}
	_RD_NormalDB->SqlCmd( DBID , _DBDelRoleRecover , _DBDelRoleRecoverResult , this 
		, "NetID"		, EM_ValueType_Int       , NetID
		, "ProxyID"		, EM_ValueType_Int       , ProxyID
		, "DBID"		, EM_ValueType_Int       , DBID 
		, NULL );
}
bool     CNetDC_Login_Child::_DBDelRoleRecover           ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	int     DBID       = Arg.GetNumber( "DBID" );
	MyDbSqlExecClass		MyDBProc( sqlBase );
	bool IsDestroy = false;
	bool IsFind = false;
	char CmdBuf[512];
	sprintf( CmdBuf , "Select IsDelete FROM RoleData WHERE DBID = %d ", DBID );
	MyDBProc.SqlCmd( CmdBuf );
	MyDBProc.Bind( 1, SQL_C_BIT, SQL_C_DEFAULT , (LPBYTE)&IsDestroy );
	if( MyDBProc.Read() )	
		IsFind = true;
	MyDBProc.Close();
	if( IsFind == false || IsDestroy == true )
		return false;
	sprintf( CmdBuf , "Update RoleData Set IsDelete = 0 , DestroyTime=-1 where DBID = %d", DBID  );
	if( MyDBProc.SqlCmd_WriteOneLine( CmdBuf ) == false )
		return false;
	return true;
}
void     CNetDC_Login_Child::_DBDelRoleRecoverResult     ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	int     DBID      = Arg.GetNumber( "DBID" );
	int     NetID     = Arg.GetNumber( "NetID" );
	int     ProxyID   = Arg.GetNumber( "ProxyID" );
	DelRoleRecoverResult( NetID , ProxyID , DBID , ResultOK );
}
void	CNetDC_Login_Child::On_SetSecondPassword( char* Account , char* Password )
{
	_RD_NormalDB->SqlCmd( rand() , _DBSetRolePassword , _DBSetRolePasswordResult , this 
		, "Account"		, EM_ValueType_String   , Account
		, "Password"	, EM_ValueType_String   , Password
		, NULL );
}
bool    CNetDC_Login_Child::_DBSetRolePassword          ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char*   Account      = Arg.GetString( "Account" );
	char*   Password     = Arg.GetString( "Password" );
	char CmdBuf[512];
	MyDbSqlExecClass		MyDBProc( sqlBase );
	sprintf( CmdBuf , "EXECUTE CreateAccount '%s'", Account );	
	MyDBProc.SqlCmd_WriteOneLine( CmdBuf );
	sprintf( CmdBuf , "UPDATE RoleData_Account SET Password = '%s' WHERE Account_ID = '%s' ", Password , Account );
	if( MyDBProc.SqlCmd_WriteOneLine( CmdBuf ) == false )
		return false;
	return true;
}
void    CNetDC_Login_Child::_DBSetRolePasswordResult    ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
}
bool     CNetDC_Login_Child::_DBCheckRoleName          ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	CheckRoleResultENUM& Result = *((CheckRoleResultENUM*)UserObj);	
	MyDbSqlExecClass	MyDBProc( sqlBase );
	char	Buf[2048];
	int		Count = 0;
	CharTransferClass	CharTransfer;
	CharTransfer.SetUtf8String( Arg.GetString( "Name" ) );
	string RoleNameBinStr = StringToSqlX( (char*)CharTransfer.GetWCString() , CharTransfer.WCStrLen() * 2 , false );
	//1 搜尋是否 不合理
	sprintf( Buf , "SELECT Count(GUID) FROM IgnoreNameTable  WHERE Name = CAST( %s AS nvarchar(4000) )" , RoleNameBinStr.c_str() );
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind(  1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Count  );
	MyDBProc.Read();
	MyDBProc.Close();
	if( Count != 0 )
	{
		Result = EM_CheckRoleResult_Failed;
		return true;
	}
	Result = EM_CheckRoleResult_OK;
	return true;
}
void     CNetDC_Login_Child::_DBCheckRoleNameResult    ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
}
struct ChangeNameRequestTempStruct
{
	CNetDC_Login_Child*	This;
	int					ServerNetID;
	int					DBID;
	int					CliNetID;
	string				RoleName;
	CheckRoleResultENUM CheckNameResult;
	//----------------------------------
	//回傳
	ChangeNameResultENUM	Result;
};
void	CNetDC_Login_Child::RM_ChangeNameRequest( int ServerNetID , int CliNetID , int DBID , char* RoleName )
{
	if(	CheckSqlStr( RoleName ) == false )
	{
		SM_ChangeNameResult( ServerNetID , CliNetID , DBID , RoleName , EM_ChangeNameResult_NameErr );
		return;
	}
	ChangeNameRequestTempStruct* TempData = NEW(ChangeNameRequestTempStruct);
	TempData->DBID = DBID;
	TempData->ServerNetID = ServerNetID;
	TempData->RoleName = RoleName;
	TempData->This = this;
	TempData->Result = EM_ChangeNameResult_OK;
	TempData->CliNetID = CliNetID;
	TempData->CheckNameResult = EM_CheckRoleResult_None;
	_RD_ImportDB->SqlCmd( rand() , _DBCheckRoleName , _DBCheckRoleNameResult , &(TempData->CheckNameResult) 
		, "Name" , EM_ValueType_String , RoleName
		, NULL );
	_RD_NormalDB->SqlCmd( 6, _DBChangeRoleName , _DBChangeRoleNameResult , TempData 
		, NULL );
}
bool    CNetDC_Login_Child::_DBChangeRoleName          ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	ChangeNameRequestTempStruct* TempData = (ChangeNameRequestTempStruct*)UserObj;
	for( int i = 0 ; i < 1000 ; i++ )
	{
		if( TempData->CheckNameResult != EM_CheckRoleResult_None )
			break;
		Sleep( 1000 );
	}
	if( TempData->CheckNameResult == EM_CheckRoleResult_Failed )
	{
		TempData->Result = EM_ChangeNameResult_NameErr;
		return false;
	}
	int		TimeBegin = timeGetTime();
	char	Buf[512];
	CNetDC_Login_Child* This		= TempData->This;
	MyDbSqlExecClass	MyDBProc( sqlBase );
	bool				Result = true;
	CharTransferClass	CharTransfer;
	CharTransfer.SetUtf8String( TempData->RoleName.c_str() );
	string RoleNameBinStr = StringToSqlX( (char*)CharTransfer.GetWCString() , CharTransfer.WCStrLen() * 2 , false );
	int		DBID = -1;
//	g_CritSect()->Enter();
	{		
//		MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );
		//1 搜尋是否有重覆的名稱
		sprintf( Buf , "SELECT DBID FROM RoleData  WHERE RoleName = CAST( %s AS nvarchar(4000) ) and IsDelete = 0 " , RoleNameBinStr.c_str() );
		MyDBProc.SqlCmd( Buf );
		MyDBProc.Bind(  1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&DBID  );
		MyDBProc.Read();
		MyDBProc.Close();
//		MyDBProc.SqlCmd_WriteOneLine( "COMMIT TRANSACTION" );
	}
//	g_CritSect()->Leave();
	if(  DBID != -1 )
	{						
		TempData->Result = EM_ChangeNameResult_NameUsed;
		return false;
	}
	//更改名字
	sprintf( Buf , "UPDATE RoleData Set RoleName = CAST( %s AS nvarchar(4000) ) WHERE DBID = %d and IsDelete = 0"  , RoleNameBinStr.c_str() , TempData->DBID );
	MyDBProc.SqlCmd_WriteOneLine( Buf );
	return true;
}
void    CNetDC_Login_Child::_DBChangeRoleNameResult    ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	ChangeNameRequestTempStruct* TempData = (ChangeNameRequestTempStruct*)UserObj;
	if( TempData->Result == EM_ChangeNameResult_OK )
	{
		//修正記憶體內資料
		BaseItemObject* Obj = BaseItemObject::GetObj_DBID( TempData->DBID );
		if( Obj != NULL )
		{
			Obj->Role()->BaseData.RoleName = TempData->RoleName.c_str();
			Obj->Role()->BaseData.SysFlag.ChangeNameEnabled = false;
			Obj->IsNeedSave( true );
		}
	}
	TempData->This->SM_ChangeNameResult( TempData->ServerNetID , TempData->CliNetID , TempData->DBID , TempData->RoleName.c_str() , TempData->Result );
	delete TempData;
}
//////////////////////////////////////////////////////////////////////////
struct ReserveRoleTempStruct
{
	int	CliNetID;
	int CliProxyID;
	string Account;
	vector< ReserverRoleStruct > List;
};
void		CNetDC_Login_Child::RM_ReserveRoleRequest( int iServerID , int CliNetID , int CliProxyID , const char* Account_ID )
{
	ReserveRoleTempStruct* TempData = NEW(ReserveRoleTempStruct);
	TempData->CliNetID = CliNetID;
	TempData->CliProxyID = CliProxyID;
	TempData->Account = Account_ID;
	_RD_NormalDB->SqlCmd( rand(), _DBReserveRole , _DBReserveRoleResult , TempData , NULL );
}
bool     CNetDC_Login_Child::_DBReserveRole         ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char	SqlCmd[1024];
	ReserveRoleTempStruct* UserData = (ReserveRoleTempStruct*)UserObj;
	MyDbSqlExecClass	MyDBProc( sqlBase );
    CreateDBCmdClass<ReserverRoleStruct>* _ReserveRoleSql   = NEW CreateDBCmdClass<ReserverRoleStruct>( RoleDataEx::ReaderRD_ReserveRole() , "ReserveNameTable" );
	ReserverRoleStruct TempReserve;	
	sprintf( SqlCmd , "where Account_ID = '%s' and DestroyTime = 0" ,  UserData->Account.c_str() );
	MyDBProc.SqlCmd( _ReserveRoleSql->GetSelectStr( SqlCmd ).c_str() );
	MyDBProc.Bind( TempReserve , RoleDataEx::ReaderRD_ReserveRole() );
	while( MyDBProc.Read() )
	{
		RoleDataEx::ReaderRD_ReserveRole()->TransferWChartoUTF8( &TempReserve , MyDBProc.wcTempBufList() );
		UserData->List.push_back( TempReserve );
	}
	MyDBProc.Close();
	delete _ReserveRoleSql;
	return true;
}
void     CNetDC_Login_Child::_DBReserveRoleResult	( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	ReserveRoleTempStruct* TempData = (ReserveRoleTempStruct*)UserObj;
	for( int i = 0 ; i < TempData->List.size() ; i++ )
	{
		SC_ReserveRole( TempData->CliNetID , TempData->CliProxyID , TempData->List[i] );
	}
	delete TempData;
}
void CNetDC_Login_Child::RM_GetObjCount( int iServerID )
{
	int iObjCount = BaseItemObject::St()->AllPlayerObj_DBID.size();
	SM_GetObjCountResult( iServerID, iObjCount );
}
void CNetDC_Login_Child::RM_SetCheckSecondPasswordResult( int iServerID, int iClientID, const char* szAccountName, int iResult )
{
	_RD_NormalDB->SqlCmd( rand() , _DBSetSPWRecord , _DBSetSPWRecordResult , this 
		, "Account"		, EM_ValueType_String   , szAccountName
		, "Result"		, EM_ValueType_int		, iResult
		, NULL );
}
//-----------------------------------------------------------------------------------------------------------------------
bool CNetDC_Login_Child::_DBSetSPWRecord( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char*   Account      = Arg.GetString( "Account" );
	int		Result		 = Arg.GetNumber( "Result"	);
	char CmdBuf[512];
	MyDbSqlExecClass		MyDBProc( sqlBase );
	sprintf( CmdBuf , "	IF( Select count( AccountID ) From SecondPasswordRecord where  AccountID = '%s' ) = 0 Insert SecondPasswordRecord(AccountID) Values( '%s' )", Account, Account );	
	MyDBProc.SqlCmd_WriteOneLine( CmdBuf , true );
	sprintf( CmdBuf , "UPDATE SecondPasswordRecord SET Result = %d, CheckTime = %d WHERE AccountID = '%s'", Result, TimeStr::Now_Value() , Account );
	if( MyDBProc.SqlCmd_WriteOneLine( CmdBuf ) == false )
		return false;
	return true;
}
//-----------------------------------------------------------------------------------------------------------------------
void CNetDC_Login_Child::_DBSetSPWRecordResult( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
}
//--------------------------------------------------------------------------------------
#if CHECK_PLAYER_DATA_HASH_CODE
void CNetDC_Login_Child::AccountIDHashCode(const char* AccountID, std::string& HashCodeReturn)
{
    std::ostringstream oss;
    oss << AccountID;
    MyMD5Class Md5_Player;
    HashCodeReturn = Md5_Player.ComputeStringHash(oss.str().c_str());
}
bool CNetDC_Login_Child::AccountDataHashCode(const char* AccountID, int AccountMoney, std::string& HashCodeReturn)
{
    char split_token = '&';
    std::ostringstream oss;
    oss << AccountID << split_token;
    oss << AccountMoney << split_token;
    oss << "rw20110311";    // private key
    MyMD5Class Md5;
    HashCodeReturn = Md5.ComputeStringHash(oss.str().c_str());
#if _DEBUG
    OutputDebugString("HashCode> ");
    OutputDebugString(HashCodeReturn.c_str());
    OutputDebugString("\n");
#endif
    return true;
}
bool CNetDC_Login_Child::AccountDataHashCodeSave(const char* AccountID, int AccountMoney, SqlBaseClass* sqlBase)
{
    if (sqlBase == NULL)
        return false;
    // make code
    std::string strHashCode;
    AccountDataHashCode(AccountID, AccountMoney, strHashCode);
    std::string strHashAccount;
    AccountIDHashCode(AccountID, strHashAccount);
    // save to database
    MyDbSqlExecClass MyDBProc(sqlBase);
    char szSqlCmd[128+1];
    _snprintf(szSqlCmd, 128, "EXEC SP_SaveAccountChangeRecord '%s', '%s';",
        strHashAccount.c_str(), strHashCode.c_str());
    return MyDBProc.SqlCmd_WriteOneLine(szSqlCmd);
}
bool CNetDC_Login_Child::AccountDataHashCodeCheck(const char* AccountID, int AccountMoney, SqlBaseClass* sqlBase)
{
    if (sqlBase == NULL)
        return false;
    // make code
    std::string strHashCode;
    AccountDataHashCode(AccountID, AccountMoney, strHashCode);
    std::string strHashAccount;
    AccountIDHashCode(AccountID, strHashAccount);
    // load from database
    MyDbSqlExecClass MyDBProc(sqlBase);
    bool IsFind = false;
    char szSqlCmd[128+1];
    _snprintf(szSqlCmd, 128, "SELECT [password] FROM Account_ChangeRecord WHERE [account_id]='%s';",
        strHashAccount.c_str());
    MyDBProc.SqlCmd(szSqlCmd);
    char szReturnCode[32+1];
    szReturnCode[32] = '\0';
    MyDBProc.Bind(1, SQL_C_CHAR, 32+1, (LPBYTE)&szReturnCode);
    if (MyDBProc.Read())
    {
        IsFind = true;
    }
    MyDBProc.Close();
    if (IsFind)
    {
        // compare hash code
        if (strcmp(strHashCode.c_str(), szReturnCode) == 0)
        {
            return true;
        }
        else
        {
            Print(LV3, "AccountHashCodeCheck", "帳號資料編碼 比對失敗 %s 資料有問題! \n", AccountID);
            return false;
        }
    }
    else
    {
        Print(LV3, "AccountHashCodeCheck", "帳號資料表格 查詢失敗 %s 資料未建立! \n", AccountID);
        return false;
    }
}
void CNetDC_Login_Child::PlayerDBIDHashCode(const char* AccountID, int PlayerDBID, std::string& HashCodeReturn)
{
    std::ostringstream oss;
    oss << AccountID << PlayerDBID;
    MyMD5Class Md5_Player;
    HashCodeReturn = Md5_Player.ComputeStringHash(oss.str().c_str());
}
bool CNetDC_Login_Child::PlayerDataHashCode(const char* AccountID, int PlayerDBID, int PlayerMoney, std::string& HashCodeReturn)
{
    char split_token = '|';
    std::ostringstream oss;
    oss << AccountID << split_token;            // account id
    oss << PlayerDBID << split_token;           // player id
    oss << PlayerMoney << split_token;          // body money
    oss << "rw20110311";                        // private key
    MyMD5Class Md5;
    HashCodeReturn = Md5.ComputeStringHash(oss.str().c_str());
#if _DEBUG
    OutputDebugString("HashCode> ");
    OutputDebugString(HashCodeReturn.c_str());
    OutputDebugString("\n");
#endif
    return true;
}
bool CNetDC_Login_Child::PlayerDataHashCodeSave(const char* AccountID, int PlayerDBID, int PlayerMoney, SqlBaseClass* sqlBase)
{
    if (sqlBase == NULL)
        return false;
    // make code
    std::string strHashCode;
    PlayerDataHashCode(AccountID, PlayerDBID, PlayerMoney, strHashCode);
    std::string strHashPlayer;
    PlayerDBIDHashCode(AccountID, PlayerDBID, strHashPlayer);
    // save to database
    MyDbSqlExecClass MyDBProc(sqlBase);
    char szSqlCmd[128+1];
    _snprintf(szSqlCmd, 128, "EXEC SP_SavePlayerChangeRecord '%s', '%s';",
        strHashPlayer.c_str(), strHashCode.c_str());
    return MyDBProc.SqlCmd_WriteOneLine(szSqlCmd);
}
bool CNetDC_Login_Child::PlayerDataHashCodeCheck(const char* AccountID, int PlayerDBID, int PlayerMoney, SqlBaseClass* sqlBase)
{
    if (sqlBase == NULL)
        return false;
    // make code
    std::string strHashCode;
    PlayerDataHashCode(AccountID, PlayerDBID, PlayerMoney, strHashCode);
    std::string strHashPlayer;
    PlayerDBIDHashCode(AccountID, PlayerDBID, strHashPlayer);
    // load from database
    MyDbSqlExecClass MyDBProc(sqlBase);
    bool IsFind = false;
    char szSqlCmd[128+1];
    _snprintf(szSqlCmd, 128, "SELECT [password] FROM RoleData_ChangeRecord WHERE [player_id]='%s';",
        strHashPlayer.c_str());
    MyDBProc.SqlCmd(szSqlCmd);
    char szReturnCode[32+1];
    szReturnCode[32] = '\0';
    MyDBProc.Bind(1, SQL_C_CHAR, 32+1, (LPBYTE)&szReturnCode);
    if (MyDBProc.Read())
    {
        IsFind = true;
    }
    MyDBProc.Close();
    if (IsFind)
    {
        // compare hash code
        if (strcmp(strHashCode.c_str(), szReturnCode) == 0)
        {
            return true;
        }
        else
        {
            Print(LV3, "PlayerDataHashCodeCheck", "角色資料編碼 比對失敗 %s(%d) 資料有問題! \n", AccountID, PlayerDBID);
            return false;
        }
    }
    else
    {
        Print(LV3, "PlayerDataHashCodeCheck", "角色資料表格 查詢失敗 %s(%d) 資料未建立! \n", AccountID, PlayerDBID);
        return false;
    }
}
#endif
//--------------------------------------------------------------------------------------