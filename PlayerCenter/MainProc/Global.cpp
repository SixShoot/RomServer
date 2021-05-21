#include "Global.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
#include "../NetWalker_Member/NetWakerRoleDataCenterInc.h"
#include <intrin.h>
#include "../WebService/Web_Base/Web_BaseClassChild.h"
//----------------------------------------------------------------------------------------------------
#define		_Def_DataBase_ThreadCount_		300

IniFileClass	                                		Global::_Ini;
bool                                            		Global::_IsDelay = true;
bool									        		Global::_IsDestory = false;
PlayerCenterDestroyENUM									Global::_DestoryReason = EM_PlayerCenterDestroy_None;
DBTransferClass*                                		Global::_DBTransfer_Game   = NULL;    //db傳送資料的物件    
DBTransferClass*                                		Global::_DBTransfer_Import   = NULL;    //db傳送資料的物件    
DBTransferClass*                                		Global::_DBTransfer_Account   = NULL;    //db傳送資料的物件    
DBStructTransferClass< PlayerRoleData >*				Global::_RoleDataDB  = NULL;    //db轉結構的物件
DBStructTransferClass< DB_ItemFieldStruct >*    		Global::_RD_ItemDB = NULL;      //db轉結構的物件
DBStructTransferClass< DB_AbilityStruct >*      		Global::_RD_AbilityDB = NULL;   //db轉結構的物件
//DBStructTransferClass< DB_BaseFriendStruct >*			Global::_RD_FriendDB = NULL;	//db轉結構的物件

DBStructTransferClass< int >*					Global::_RD_NormalDB = NULL;	//db轉結構的物件(直接使用SQL命令)一般類型
DBStructTransferClass< int >*					Global::_RD_ImportDB = NULL;	//db轉結構的物件(直接使用SQL命令)一般類型
DBStructTransferClass< int >*					Global::_RD_AccountDB = NULL;	//db轉結構的物件(直接使用SQL命令)一般類型

CreateDBCmdClass<PlayerRoleData>*               Global::_RoleDataSql = NULL;
CreateDBCmdClass<PlayerRoleData>*               Global::_RoleDataAccountSql = NULL;
//CreateDBCmdClass<RoleDataEx>*                   Global::_RoleDataExSql = NULL;
CreateDBCmdClass<DB_ItemFieldStruct>*           Global::_RDItemSql = NULL;
CreateDBCmdClass<DB_AbilityStruct>*             Global::_RDAbilitySql = NULL;
CreateDBCmdClass<DB_BaseFriendStruct>*			Global::_RDFriendSql = NULL;
CreateDBCmdClass<DB_CultivatePetStruct>*		Global::_RDCultivatePetSql = NULL;

RecycleBin< DBRoleTempDataInfo >*               Global::_ReadRole;
RecycleBin< PlayerRoleData >*                   Global::_RoleBin;
string											Global::_RoleStateStoreProcedure;
bool											Global::_WebServic = false;
//--------------------------------------------------------------------------------------
bool     Global::Init( char* IniFile )
{

	//--------------------------------------------------------------------------------------
	_Ini.SetIniFile( "Global.ini" );
    if( _Ini.SetIniFile( IniFile ) == false )
    {
        MessageBox( NULL , "GSrv Ini File Read Error!!" , "Error" ,MB_OK );
        return false;
    }
	//////////////////////////////////////////////////////////////////////////
	//序號檢查
	{
		//int 	CPUInfo[4];
		//string ProcessKey = _Ini.Str( "ProcessKey" );
		//char* Pwd = DecodePassword( ProcessKey.c_str() , "runewaker" );
		//char	Buf[512];
		//__cpuid( CPUInfo, 1 );
		//sprintf( Buf , "%x.%x.%x.%x", CPUInfo[0],CPUInfo[1]&0xffffff,CPUInfo[2],CPUInfo[3]&0xffffff );
		//
		//if( stricmp( Buf , Pwd ) != 0 )
		//{			
		//	PrintToController(true, Buf );
		//	exit( 1 );
		//}
		
	}

	//////////////////////////////////////////////////////////////////////////
	g_pPrint->SetPrintLV( _Ini.Int( "DebugPrintLv" ) );
	g_pPrint->SetFileOutputLV( _Ini.Int( "DebugOutputLv" ) );
	g_pPrint->SetOutputFileName( _Ini.Str( "DebugOutputFile" ).c_str() );
	g_pPrint->SetOutputFieldName( _Ini.Str( "DebugOutputField" ).c_str() );

	SqlBaseClass::Init( _Ini.Str( "DebugOutputField" ).c_str() , _Ini.Str( "DebugOutputFile" ).c_str() );

	Print( LV5 , "##開啟Server##" , "時間 %s" , TimeStr::Now_Time()  );
	//--------------------------------------------------------------------------------------
	GlobalBase::_Init();
	_ReadRole = NEW( RecycleBin< DBRoleTempDataInfo > );
	_RoleBin = NEW( RecycleBin< PlayerRoleData > );
	//--------------------------------------------------------------------------------------
//	_Schedular = FunctionSchedularClass::New( );
//	Print.Init( "Error.log" );
    //--------------------------------------------------------------------------------------
    //劇情資料初始化

    BaseItemObject::Init( );
	LUA_VMClass::Init();
	

    //--------------------------------------------------------------------------------------
	Print( LV5 , "Global::Init" , "DB Init"  );
    string  DBerver			= _Ini.Str( "Game_DBServer" );
    string  DataBase	    = _Ini.Str( "Game_DataBase" );
    string  Account			= _Ini.Str( "Game_Account" );
    string  Password		= _Ini.Str( "Game_Password" );

	int DBThreadCount =  _Ini.Int( "DBThreadCount" );
	if( DBThreadCount <= 0 )
		DBThreadCount = _Def_DataBase_ThreadCount_;

	char* Pwd = DecodePassword( Password.c_str() , Account.c_str() );


    _DBTransfer_Game     = NEW DBTransferClass( (char*)DBerver.c_str() , (char*)DataBase.c_str() , (char*)Account.c_str() , Pwd , DBThreadCount ) ;

	string	ImportDBServer =  _Ini.Str( "Import_DBServer" );
	string	ImportDataBase =  _Ini.Str( "Import_DataBase" );
	string	ImportAccount  =  _Ini.Str( "Import_Account" );
	string	ImportPassword =  _Ini.Str( "Import_Password" );
	string ImportPwd = DecodePassword( ImportPassword.c_str() , ImportAccount.c_str() );
	_DBTransfer_Import   = NEW DBTransferClass( (char*)ImportDBServer.c_str() , (char*)ImportDataBase.c_str() , (char*)ImportAccount.c_str() , ImportPwd.c_str() , 10 );


	string	AccDBServer =  _Ini.Str( "Acc_DBServer" );
	string	AccDataBase =  _Ini.Str( "Acc_DataBase" );
	string	AccAccount  =  _Ini.Str( "Acc_Account" );
	string	AccPassword =  _Ini.Str( "Acc_Password" );
	string AccPwd = DecodePassword( AccPassword.c_str() , AccAccount.c_str() );
	_DBTransfer_Account = NEW DBTransferClass( (char*)AccDBServer.c_str() , (char*)AccDataBase.c_str() , (char*)AccAccount.c_str() , AccPwd.c_str() , 10 );


    _RoleDataDB     = NEW DBStructTransferClass< PlayerRoleData > ( _DBTransfer_Game , RoleDataEx::ReaderBase() , "RoleData" );
    _RD_ItemDB      = NEW DBStructTransferClass< DB_ItemFieldStruct > ( _DBTransfer_Game , RoleDataEx::ReaderRD_Field()  , "RoleData_Item" );
    _RD_AbilityDB   = NEW DBStructTransferClass< DB_AbilityStruct > ( _DBTransfer_Game , RoleDataEx::ReaderRD_Ability()  , "RoleData_Ability" );
//	_RD_FriendDB    = NEW DBStructTransferClass< DB_BaseFriendStruct > ( _DBTransfer_Game , RoleDataEx::ReaderRD_BaseFriendStruct()  , "RoleData_FriendList" );

	_RD_NormalDB	= NEW DBStructTransferClass< int > ( _DBTransfer_Game , NULL  , "" );							//db轉結構的物件
	_RD_ImportDB	= NEW DBStructTransferClass< int > ( _DBTransfer_Import, NULL  , "" );							//db轉結構的物件
	_RD_AccountDB	= NEW DBStructTransferClass< int > ( _DBTransfer_Account, NULL  , "" );							//db轉結構的物件

    _RoleDataSql    = NEW CreateDBCmdClass<PlayerRoleData>( RoleDataEx::ReaderBase() , "RoleData" );
    _RoleDataAccountSql    = NEW CreateDBCmdClass<PlayerRoleData>( RoleDataEx::ReaderBase_Account() , "RoleData_Account" );
//    _RoleDataExSql   = NEW( CreateDBCmdClass<RoleDataEx>( RoleDataEx::ReaderBaseEx() , "RoleDataEx" ) );
    _RDItemSql      = NEW CreateDBCmdClass<DB_ItemFieldStruct>( RoleDataEx::ReaderRD_Field() , "RoleData_Item" );
    _RDAbilitySql   = NEW CreateDBCmdClass<DB_AbilityStruct>( RoleDataEx::ReaderRD_Ability() , "RoleData_Ability" );
	_RDFriendSql    = NEW CreateDBCmdClass<DB_BaseFriendStruct > ( RoleDataEx::ReaderRD_BaseFriendStruct()  , "RoleData_FriendList" );
	_RDCultivatePetSql  = NEW CreateDBCmdClass<DB_CultivatePetStruct > ( RoleDataEx::ReaderRD_CultivatePetStruct()  , "RoleData_Pet" );
    //--------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------
	Print( LV5 , "Global::Init" , "Load Object"  );
	//讀取物件資料
	string	DataPath  = _Ini.Str( "DataPath" ); 

	string Datalanguage;
	string Stringlanguage;
	// 讀取 Server Data 與 String 的設定
	if( _Ini.Str( "Datalanguage", Datalanguage ) == false )
	{
		Datalanguage = "rw";
	}

	Datalanguage = strlwr( (char*)Datalanguage.c_str() );

	if( _Ini.Str( "Stringlanguage", Stringlanguage ) == false )
	{
		Stringlanguage = "";
	}

	Print( LV5 , "Global::Init" , "InitRuneEngine"  );

	ObjectDataClass::InitRuneEngine( DataPath.c_str() );
	ObjectDataClass::Init( DataPath.c_str() , Datalanguage.c_str() , Stringlanguage.c_str() );
	//--------------------------------------------------------------------------------------
	//網路封包資料初始化
	//--------------------------------------------------------------------------------------
	Print( LV5 , "Global::Init" , "Net Init"  );
	Net_ServerList->Init( NULL , EM_SERVER_TYPE_ROLECENTER , _Ini.Int("PlayerCenterID") );    
	CNetDC_DCBaseChild::Init();
	CNetDC_Login_Child::Init();
	NetSrv_OnlinePlayerChild::Init();
	NetSrv_GMToolsChild::Init();

	_Net->RegGSrvNetID( EM_SERVER_TYPE_ROLECENTER , _Ini.Int("PlayerCenterID") );
//	_Net->ConnectType( EM_GSrvType_Active );
	//--------------------------------------------------------------------------------------
	Web_BaseClassChild::Init();		
	//--------------------------------------------------------------------------------------
    string  IP   = _Ini.Str( "SwitchIP" );
    int     Port = _Ini.Int( "Switch_GSrvPort" );

	//if( _Ini.Int( "IsOnleyWebService" ) == 0 )
	//{
		_Net->Connect( (char*)IP.c_str() , Port , "RoleDataCenter" );
	//}
	//else
	//{
		_WebServic = true;
		Web_BaseClassChild::Host( _Ini.Str( "WebServiceHost" ).c_str() , _Ini.Int( "WebServicePort" ) );
	//}


	RoleDataEx::G_TimeZone = _Ini.Int("TimeZone");
	_RoleStateStoreProcedure = _Ini.Str("RoleStateStoreProcedure");
	//--------------------------------------------------------------------------------------

    return true;
}

//--------------------------------------------------------------------------------------
bool     Global::Release( )
{
	SqlBaseClass::Release();
	ObjectDataClass::Release();

	GlobalBase::_Release();
	LUA_VMClass::Release();
	

	CNetDC_DCBaseChild::Release();
	CNetDC_Login_Child::Release();
	NetSrv_OnlinePlayerChild::Release();
	NetSrv_GMToolsChild::Release();

	Web_BaseClassChild::Release();

	BaseItemObject::Release( );
	

    delete _DBTransfer_Game;
	delete _DBTransfer_Import;
	delete _DBTransfer_Account;
    delete _RoleDataDB;
	delete _ReadRole;
	delete _RoleBin;

	delete _RD_ItemDB;     
	delete _RD_AbilityDB;  
//	delete _RD_FriendDB;	

	delete _RD_NormalDB;
	delete _RD_ImportDB;
	delete _RD_AccountDB;

    delete _RDItemSql;
    delete _RDAbilitySql;
	delete _RDFriendSql;
	delete _RDCultivatePetSql;

    delete _RoleDataSql;
	delete _RoleDataAccountSql;
//	delete _RoleDataExSql;
    return true;
}

void	Global::DBProcess()
{
    _RoleDataDB->Process( );
    _RD_ItemDB->Process( );
    _RD_AbilityDB->Process( );
//	_RD_FriendDB->Process( );
	_RD_NormalDB->Process( );
	_RD_ImportDB->Process( );
	_RD_AccountDB->Process( );
	SqlBaseClass::OnTimeProc( );
}

bool     Global::Process( )
{
    static int		ProcTime = timeGetTime();
	static int		NextMemCheckTime = ProcTime + 1000*60*10;
    static int		LastProcDelayTime = 0;
    static int		LastProcDelayRecorder = 0;
	static int		LastAccountLogTime = ProcTime;

	RoleDataEx::OnTimeProcAll( );
    
    LastProcDelayTime = timeGetTime() -  ProcTime;
	_Net->Process( );
    if( LastProcDelayTime > 100 )
    {
        ProcTime += 100;
        LUA_VMClass::Process( );     
		Global::DBProcess();
		
		if( _WebServic )
			Web_BaseClass::Process();
    }

	if( NextMemCheckTime < ProcTime )
	{
		NextMemCheckTime = ProcTime + 1000*60*10;
		MyAllocMemoryCheck();
	}

    if( LastProcDelayTime > 1000)
    {
        if( LastProcDelayRecorder < LastProcDelayTime )
        {
            LastProcDelayRecorder = LastProcDelayTime;
            Print( Def_PrintLV3, "Process" , "Program Delay %d " , LastProcDelayTime );
        }
        Global::_IsDelay = true;
    }
    else
    {
        LastProcDelayRecorder = 0;
        Global::_IsDelay = false;
    }

	if( _IsDestory == true )
	{
		if(	_DBTransfer_Game->TotalRequestCount() == 0 )
			return false;
	}

	if( ProcTime - LastAccountLogTime > 5000 )
	{
		LastAccountLogTime = ProcTime ;
		Print( LV2 , "Process" , "RoleCount = %d" 
			, BaseItemObject::St()->AllPlayerObj_DBID.size() );
	}

	return true;
}

void GetExRoleStateCmd( int Type , char* Buf , RoleDataEx* Role )
{
	if( Type == 0 ) //全身
	{
		Role->InitCal();		
	}
	else
	{
		Role->TempData.Attr.Init_BaseEQValue();
		Role->TempData.Attr.Init_MidValue();

		switch( Type )
		{
		case 1: //空身
			break;
		case 2: //空身+裝備
			Role->ReCalculateEqOnly();
			break;
		case 3: //空身+稱號
			Role->WearObject( Role->BaseData.TitleID );
			break;
		case 4: //空身+怪物卡
			for( int i = 0 ; i < _MAX_CARD_COUNT_ ; i++ )
			{
				if( Role->PlayerBaseData->Card.GetFlag( i ) )
				{	
					GameObjDbStructEx* CardDB = g_ObjectData->GetObj( i + Def_ObjectClass_Card );	
					if( CardDB ==NULL )
						continue;
					Role->WearObject( CardDB->Item.CardAddpower );
				}
			}
			break;
		case 5: //空身+被動技
			for( int i = 0 ; i < Role->PlayerTempData->PassiveSkill.Size() ; i++ )
			{
				Role->WearBuff( Role->PlayerTempData->PassiveSkill[i] , Role->PlayerTempData->PassiveSkillLv[i] );
			}
			break;
		}
		Role->Calculate();
	}

	RoleValueData& Attr = Role->TempData.Attr;

	sprintf( Buf , "Execute %s %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
			, Global::RoleStateStoreProcedure().c_str()
			, Role->DBID() 
			, Type
			, (int)Attr.Fin.STR
			, (int)Attr.Fin.AGI
			, (int)Attr.Fin.STA
			, (int)Attr.Fin.INT
			, (int)Attr.Fin.MND
			, (int)Attr.Fin.MaxHP
			, (int)Attr.Fin.MaxMP
 			, (int)Attr.Fin.Resist.Earth
 			, (int)Attr.Fin.Resist.Water   
 			, (int)Attr.Fin.Resist.Fire
 			, (int)Attr.Fin.Resist.Wind
 			, (int)Attr.Fin.Resist.Light
 			, (int)Attr.Fin.Resist.Darkness
			, (int)Attr.Fin.Main_DMG
			, (int)Attr.Fin.ATK
			, (int)Attr.Fin.CriticalHit
			, (int)Attr.Fin.HitRate
			, (int)Attr.Fin.DEF
			, (int)Attr.Fin.ParryRate
			, (int)Attr.Fin.DodgeRate
			, (int)Attr.Fin.ResistCriticalHit
			, (int)Attr.Fin.MDMG
			, (int)Attr.Fin.MATK
			, (int)Attr.Fin.MagicCriticalHit
			, (int)(Attr.Fin.MDMG * RoleValueData::FixArgTable[1].Dmg_HealArg)
			, (int)Attr.Fin.MagicHitRate
			, (int)Attr.Fin.MDEF
			, (int)Attr.Fin.MagicDodgeRate
			, (int)Attr.Fin.ResistMagicCriticalHit
			, (int)Attr.Fin.Bow_DMG
			, (int)Attr.Fin.ATK
			, (int)Attr.Fin.CriticalHit
			, (int)Attr.Fin.Bow_HitRate			);
}





struct TempSavePlayerStruct
{
	PlayerRoleData Role;
	BaseItemObject* Obj;
};

bool   Global::_SaveRoleSQLCmdRequestEvent ( vector<PlayerRoleData>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempSavePlayerStruct* tempData = (TempSavePlayerStruct*)UserObj;
	//MutilThread_CritSect	Thread_CritSect;
	int			TimeBegin = timeGetTime();
    char        Buf[512];
    int         i;
    bool        Result = true;
	MyDbSqlExecClass	MyDBProc( sqlBase );

    BaseItemObject* Obj = tempData->Obj;

	if( Obj == NULL )
		return false;
	PlayerRoleData     RoleDBTemp;
    PlayerRoleData*    Role = &(tempData->Role);
    PlayerRoleData*    RoleDB = &RoleDBTemp;

    //儲存角色資料
	if( Obj->GetRoleDB( RoleDBTemp ) == false )
		return false;

#if CHECK_PLAYER_DATA_HASH_CODE
    if (CNetDC_Login_Child::AccountDataHashCodeSave(tempData->Role.Base.Account_ID.Begin(), Role->PlayerBaseData.Body.Money_Account, sqlBase) == false)
    {
        Print(LV3, "CNetDC_Login_Child", "AccountDataHashCodeSave(%s) Failed!\n", tempData->Role.Base.Account_ID.Begin());
    }
    if (CNetDC_Login_Child::PlayerDataHashCodeSave(tempData->Role.Base.Account_ID.Begin(), Role->Base.DBID, Role->PlayerBaseData.Body.Money, sqlBase) == false)
    {
        Print(LV3, "CNetDC_Login_Child", "PlayerDataHashCodeSave(%d) Failed!\n", Role->Base.DBID);
    }
#endif

	//g_CritSect()->Enter();
	{
		DB_CultivatePetStruct TempPetInfo;
		DB_ItemFieldStruct  TempItemField;

		sprintf( Buf , "where Account_ID = '%s'" , Role->Base.Account_ID.Begin() );
		MyDBProc.SqlCmd_WriteOneLine( _RoleDataAccountSql->GetUpDateStr( Role , Buf ).c_str() );

		//寫入CheckSum
		{
			sprintf( Buf , "UPDATE RoleData_Account Set SerialCode =%d where Account_ID = '%s'" , Role->AccountSerialCode() , Role->Base.Account_ID.Begin() );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
		

		//////////////////////////////////////////////////////////////////////////
		//RoleBaseEx儲存
		sprintf( Buf , "where DBID = %d" , Role->Base.DBID );
		MyDBProc.SqlCmd_WriteOneLine( _RoleDataSql->GetUpDateStr( Role , Buf ).c_str() );
		//////////////////////////////////////////////////////////////////////////
		//儲存寵物資料
		if( Obj->IsNeedSaveAll() )
		{
			sprintf( Buf , "DELETE RoleData_Pet where PlayerDBID = %d" ,  Role->Base.DBID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}

		for( i = 0 ; i < MAX_CultivatePet_Count ; i++ )
		{
			TempPetInfo.PlayerDBID = Role->Base.DBID;
			TempPetInfo.Pos = i;
			TempPetInfo.Info = Role->PlayerBaseData.Pet.Base[i];

			if( Obj->IsNeedSaveAll()  )
			{	
				if( TempPetInfo.Info.IsEmpty() )
					continue;
				MyDBProc.SqlCmd_WriteOneLine(  _RDCultivatePetSql->GetInsertStr( &TempPetInfo ).c_str() );
			}
			else
			{
				if(		memcmp( &Role->PlayerBaseData.Pet.Base[i]._Value , &RoleDB->PlayerBaseData.Pet.Base[i]._Value , sizeof(Role->PlayerBaseData.Pet.Base[i]._Value) )== 0 
					&&	stricmp( Role->PlayerBaseData.Pet.Base[i].Name.Begin() , RoleDB->PlayerBaseData.Pet.Base[i].Name.Begin()) == 0 ) 
					continue;

				sprintf( Buf , "DELETE RoleData_Pet where PlayerDBID = %d and Pos=%d" ,  Role->Base.DBID , TempPetInfo.Pos );
				MyDBProc.SqlCmd_WriteOneLine( Buf );

				if( TempPetInfo.Info.IsEmpty() == false )
					MyDBProc.SqlCmd_WriteOneLine( _RDCultivatePetSql->GetInsertStr( &TempPetInfo ).c_str() );

			}

		}
		//////////////////////////////////////////////////////////////////////////
		//資料比對看是否有必要儲存		
		if( Obj->IsNeedSaveAll() )
		{
			sprintf( Buf , "DELETE RoleData_Item where FieldType=%d and DBID = %d" , EM_ItemFieldType_Body ,  Role->Base.DBID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
		
		TempItemField.DBID = Role->Base.DBID;
		TempItemField.FieldType = EM_ItemFieldType_Body;
		for( i = 0 ; i < _MAX_BODY_COUNT_ ; i++ )
		{
			if( Role->PlayerBaseData.Body.Item[i].IsEmpty() )
				Role->PlayerBaseData.Body.Item[i].Init();

			TempItemField.FieldID = i;
			TempItemField.Info = Role->PlayerBaseData.Body.Item[i];

			ItemFieldStruct& itemBB = RoleDB->PlayerBaseData.Body.Item[i];

			if( Obj->IsNeedSaveAll()  )
			{
				if( TempItemField.Info.IsEmpty() )
					continue;
				MyDBProc.SqlCmd_WriteOneLine(  _RDItemSql->GetInsertStr( &TempItemField ).c_str() );
			}
			else
			{
				if( memcmp( &TempItemField.Info , &itemBB, sizeof(TempItemField.Info) )== 0 ) 
					continue;

				sprintf( Buf , "DELETE RoleData_Item where FieldType = %d and DBID = %d and FieldID=%d" , EM_ItemFieldType_Body , Role->Base.DBID , TempItemField.FieldID );
				MyDBProc.SqlCmd_WriteOneLine( Buf );

				if( TempItemField.Info.IsEmpty() == false )
					MyDBProc.SqlCmd_WriteOneLine( _RDItemSql->GetInsertStr( &TempItemField ).c_str() );
			}
		}

		if( Obj->IsNeedSaveAll() )
		{
			sprintf( Buf , "DELETE RoleData_Item where FieldType=%d and DBID = %d" , EM_ItemFieldType_Bank ,  Role->Base.DBID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
		TempItemField.FieldType = EM_ItemFieldType_Bank;
		for( i = 0 ; i < _MAX_BANK_COUNT_ ; i++ )
		{
			if( Role->PlayerBaseData.Bank.Item[i].IsEmpty() )
				Role->PlayerBaseData.Bank.Item[i].Init();

			TempItemField.FieldID = i;
			TempItemField.Info = Role->PlayerBaseData.Bank.Item[i];

			ItemFieldStruct& itemBB = RoleDB->PlayerBaseData.Bank.Item[i];

			if( Obj->IsNeedSaveAll()  )
			{
				if( TempItemField.Info.IsEmpty() )
					continue;
				MyDBProc.SqlCmd_WriteOneLine(  _RDItemSql->GetInsertStr( &TempItemField ).c_str() );
			}
			else
			{
				if( memcmp( &TempItemField.Info , &itemBB, sizeof(TempItemField.Info) )== 0 ) 
					continue;

				sprintf( Buf , "DELETE RoleData_Item where FieldType = %d and DBID = %d and FieldID=%d" , EM_ItemFieldType_Bank , Role->Base.DBID , TempItemField.FieldID );
				MyDBProc.SqlCmd_WriteOneLine( Buf );

				if( TempItemField.Info.IsEmpty() == false )
					MyDBProc.SqlCmd_WriteOneLine( _RDItemSql->GetInsertStr( &TempItemField ).c_str() );
			}
		}

		if( Obj->IsNeedSaveAll() )
		{
			sprintf( Buf , "DELETE RoleData_Item where FieldType=%d and DBID = %d" , EM_ItemFieldType_EQ ,  Role->Base.DBID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
		TempItemField.FieldType = EM_ItemFieldType_EQ;
		for( i = 0 ; i < EM_EQWearPos_MaxCount ; i++ )
		{
			if( Role->BaseData.EQ.Item[i].IsEmpty() )
				Role->BaseData.EQ.Item[i].Init();

			TempItemField.FieldID = i;
			TempItemField.Info = Role->BaseData.EQ.Item[i];

			ItemFieldStruct& itemBB = RoleDB->BaseData.EQ.Item[i];

			if( Obj->IsNeedSaveAll()  )
			{
				if( TempItemField.Info.IsEmpty() )
					continue;
				MyDBProc.SqlCmd_WriteOneLine(  _RDItemSql->GetInsertStr( &TempItemField ).c_str() );
			}
			else
			{
				if( memcmp( &TempItemField.Info , &itemBB, sizeof(TempItemField.Info) )== 0 ) 
					continue;

				sprintf( Buf , "DELETE RoleData_Item where FieldType = %d and DBID = %d and FieldID=%d" , EM_ItemFieldType_EQ , Role->Base.DBID , TempItemField.FieldID );
				MyDBProc.SqlCmd_WriteOneLine( Buf );

				if( TempItemField.Info.IsEmpty() == false )
					MyDBProc.SqlCmd_WriteOneLine( _RDItemSql->GetInsertStr( &TempItemField ).c_str() );
			}			
		}

		if( Obj->IsNeedSaveAll() )
		{
			sprintf( Buf , "DELETE RoleData_Item where FieldType=%d and DBID = %d" , EM_ItemFieldType_DestroyItem ,  Role->Base.DBID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
		TempItemField.FieldType = EM_ItemFieldType_DestroyItem;
		for( i = 0 ; i < DEF_MAX_DESTROY_ITEM_LOG_COUNT ; i++ )
		{
			TempItemField.FieldID = i;
			TempItemField.Info = Role->PlayerSelfData.DestroyItemLog.ItemList[i];

			ItemFieldStruct& itemBB = RoleDB->PlayerSelfData.DestroyItemLog.ItemList[i];

			if( Obj->IsNeedSaveAll()  )
			{
				if( TempItemField.Info.IsEmpty() )
					continue;
				MyDBProc.SqlCmd_WriteOneLine(  _RDItemSql->GetInsertStr( &TempItemField ).c_str() );
			}
			else
			{
				if( memcmp( &TempItemField.Info , &itemBB, sizeof(TempItemField.Info) )== 0 ) 
					continue;

				sprintf( Buf , "DELETE RoleData_Item where FieldType = %d and DBID = %d and FieldID=%d" , EM_ItemFieldType_DestroyItem , Role->Base.DBID , TempItemField.FieldID );
				MyDBProc.SqlCmd_WriteOneLine( Buf );

				if( TempItemField.Info.IsEmpty() == false )
					MyDBProc.SqlCmd_WriteOneLine( _RDItemSql->GetInsertStr( &TempItemField ).c_str() );
			}

		}

		sprintf( Buf , "DELETE RoleData_Item where FieldType=%d and DBID = %d" , EM_ItemFieldType_TempItem ,  Role->Base.DBID );
		MyDBProc.SqlCmd_WriteOneLine( Buf );
		TempItemField.FieldType = EM_ItemFieldType_TempItem;

		for( i = 0 ; i < _MAX_TEMP_COUNT_ ; i++ )
		{
			if( i >= Role->PlayerBaseData.ItemTemp.Size() )
			{
				Role->PlayerBaseData.ItemTemp[i].Init();
			}

			TempItemField.FieldID = i;
			TempItemField.Info = Role->PlayerBaseData.ItemTemp[i];

			if( TempItemField.Info.IsEmpty() )
				continue;

			MyDBProc.SqlCmd_WriteOneLine( _RDItemSql->GetInsertStr( &TempItemField ).c_str() );

		}

		if( Obj->IsNeedSaveAll() )
		{
			sprintf( Buf , "DELETE RoleData_Item where FieldType=%d and DBID = %d" , EM_ItemFieldType_EQBackup ,  Role->Base.DBID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
		TempItemField.FieldType = EM_ItemFieldType_EQBackup;
		for( int j = 0 ; j < _MAX_BACKUP_EQ_SET_COUNT_ ; j++ )
		{
			for( i = 0 ; i < EM_EQWearPos_MaxCount ; i++ )
			{
				TempItemField.FieldID = i + j * _DEF_EQ_BACKUP_BASE_;
				TempItemField.Info = Role->PlayerBaseData.EQBackup[j].Item[i];

				ItemFieldStruct& itemBB = RoleDB->PlayerBaseData.EQBackup[j].Item[i];

				if( Obj->IsNeedSaveAll()  )
				{
					if( TempItemField.Info.IsEmpty() )
						continue;
					MyDBProc.SqlCmd_WriteOneLine(  _RDItemSql->GetInsertStr( &TempItemField ).c_str() );
				}
				else
				{
					if( memcmp( &TempItemField.Info , &itemBB, sizeof(TempItemField.Info) )== 0 ) 
						continue;

					sprintf( Buf , "DELETE RoleData_Item where FieldType = %d and DBID = %d and FieldID=%d" , EM_ItemFieldType_EQBackup , Role->Base.DBID , TempItemField.FieldID );
					MyDBProc.SqlCmd_WriteOneLine( Buf );

					if( TempItemField.Info.IsEmpty() == false )
						MyDBProc.SqlCmd_WriteOneLine( _RDItemSql->GetInsertStr( &TempItemField ).c_str() );
				}			
			}
		}
		

		if( Obj->IsNeedSaveAll() )
		{
			sprintf( Buf , "DELETE RoleData_Item where FieldType=%d and DBID = %d" , EM_ItemFieldType_Pet ,  Role->Base.DBID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
		TempItemField.FieldType = EM_ItemFieldType_Pet;

		for( int j = 0 ; j < MAX_CultivatePet_Count ; j++ )
		{			
			for( i = 0 ; i < EM_CultivatePetEQType_MAX ; i++ )
			{
				TempItemField.DBID = Role->Base.DBID;
				TempItemField.FieldID = i + j * 100;
				TempItemField.Info = Role->PlayerBaseData.Pet.Base[j].EQ[i];
				if( Obj->IsNeedSaveAll()  )
				{	
					if( TempItemField.Info.IsEmpty() )
						continue;
					MyDBProc.SqlCmd_WriteOneLine(  _RDItemSql->GetInsertStr( &TempItemField ).c_str() );
				}
				else
				{
					if( memcmp( &Role->PlayerBaseData.Pet.Base[j].EQ[i] , &RoleDB->PlayerBaseData.Pet.Base[j].EQ[i], sizeof(Role->PlayerBaseData.Pet.Base[j].EQ[i]) )== 0 ) 
						continue;

					sprintf( Buf , "DELETE RoleData_Item where FieldType = 7 and DBID = %d and FieldID=%d" ,  Role->Base.DBID , TempItemField.FieldID );
					MyDBProc.SqlCmd_WriteOneLine( Buf );

					if( TempItemField.Info.IsEmpty() == false )
						MyDBProc.SqlCmd_WriteOneLine( _RDItemSql->GetInsertStr( &TempItemField ).c_str() );


				}
			}
		}


		DB_AbilityStruct  TempAbility;
		for( i = 0 ; i < EM_Max_Vocation ; i++ )
		{
			if( Obj->IsNeedSaveAll() == false )
			{
				if( memcmp( &Role->PlayerBaseData.AbilityList[i] , &RoleDB->PlayerBaseData.AbilityList[i] , sizeof(Role->PlayerBaseData.AbilityList[i]) )== 0 ) 
					continue;
			}

			TempAbility.Info = Role->PlayerBaseData.AbilityList[i];
			sprintf( Buf , "where Job = %d and DBID = %d" ,  i , Role->Base.DBID );

			MyDBProc.SqlCmd_WriteOneLine(  _RDAbilitySql->GetUpDateStr( &TempAbility , Buf ).c_str() );
		}

		//好有名單
		DB_BaseFriendStruct  TempFriend;
		FriendListStruct&	FriendList = Role->PlayerSelfData.FriendList;
		FriendListStruct&	FriendListDB = RoleDB->PlayerSelfData.FriendList;
		TempFriend.OwnerDBID = Role->Base.DBID;

		TempFriend.Type = EM_FriendListType_Friend;

		for( int i = 0 ; i < _MAX_FRIENDLIST_COUNT_ ; i++ )
		{
			if( memcmp( &(FriendList.FriendList[i]) , &(FriendListDB.FriendList[i]) , sizeof(FriendList.FriendList[i]) )== 0 ) 
				continue;

			TempFriend.Info = FriendList.FriendList[i];
			TempFriend.FieldID = i;

			sprintf( Buf , "Delete RoleData_FriendList where Type =%d and FieldID = %d and OwnerDBID = %d" , EM_FriendListType_Friend ,  i , Role->Base.DBID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
			if( FriendList.FriendList[i].IsEmpty() == false )
			{
				MyDBProc.SqlCmd_WriteOneLine(  _RDFriendSql->GetInsertStr( &TempFriend ).c_str() );
			}
		}
		//////////////////////////////////////////////////////////////////////////
		
		TempFriend.Type = EM_FriendListType_FamilyFriend;
		for( int i = 0 ; i < _MAX_FRIENDLIST_FAMILY_COUNT_ ; i++ )
		{
			if( memcmp( &(FriendList.FamilyList[i]) , &(FriendListDB.FamilyList[i]) , sizeof(FriendList.FamilyList[i]) )== 0 ) 
				continue;

			TempFriend.Info = FriendList.FamilyList[i];
			TempFriend.FieldID = i;

			sprintf( Buf , "Delete RoleData_FriendList where Type =%d and FieldID = %d and OwnerDBID = %d" , EM_FriendListType_FamilyFriend ,  i , Role->Base.DBID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
			if( FriendList.FamilyList[i].IsEmpty() == false )
			{
				MyDBProc.SqlCmd_WriteOneLine(  _RDFriendSql->GetInsertStr( &TempFriend ).c_str() );
			}
		}
		
		//////////////////////////////////////////////////////////////////////////
		
		TempFriend.Type = EM_FriendListType_HateFriend;

		for( int i = 0 ; i < _MAX_HATE_PLAYER_LIST_COUNT_ ; i++ )
		{
			if( memcmp( &(FriendList.HateFriendList[i]) , &(FriendListDB.HateFriendList[i]) , sizeof(FriendList.HateFriendList[i]) )== 0 ) 
				continue;

			TempFriend.Info = FriendList.HateFriendList[i];
			TempFriend.FieldID = i;

			sprintf( Buf , "Delete RoleData_FriendList where Type =%d and FieldID = %d and OwnerDBID = %d" , EM_FriendListType_HateFriend ,  i , Role->Base.DBID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
			if( FriendList.HateFriendList[i].IsEmpty() == false )
			{
				MyDBProc.SqlCmd_WriteOneLine(  _RDFriendSql->GetInsertStr( &TempFriend ).c_str() );
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//如果為GM則不處理排行榜
		//if( Role->PlayerBaseData.ManageLV >= EM_Management_GameVisitor && Role->TempData.Sys.GM_Designed == false )
		if (Role->PlayerBaseData.ManageLV >= EM_Management_GameVisitor)
		{
			sprintf( Buf , "Update RoleData Set IsGM = 1 where DBID =%d" , Role->Base.DBID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
		//////////////////////////////////////////////////////////////////////////
		Obj->IsNeedSaveAll( false );
		Obj->SetRoleDBZip( Role );
		//printf( "\n----------------儲存SQL_End(%s)--------------" ,  Utf8ToChar( Role->RoleName() ).c_str() );
		//////////////////////////////////////////////////////////////////////////
		//如果有設定角色狀態愈存程序
		if( _RoleStateStoreProcedure.size() != 0 )
		{
			char CmdStr[4096];
			for( int i = 0 ; i < 6 ; i++ )
			{
				GetExRoleStateCmd( i , CmdStr , (RoleDataEx*)Role );
				MyDBProc.SqlCmd_WriteOneLine( CmdStr );
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}
	

	memset( &Role->Base.CreateTime , 0 , sizeof(Role->Base.CreateTime) );
	sprintf( Buf , "SELECT CreateTime FROM RoleData WHERE DBID = %d",  Role->Base.DBID );
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1, SQL_C_CHAR	, sizeof(Role->Base.CreateTime)	, (LPBYTE)Role->Base.CreateTime.Begin() );
	MyDBProc.Read();
	MyDBProc.Close();

//	g_CritSect()->Leave();`
	//寫入角色CheckSum
	{
		sprintf( Buf , "UPDATE RoleData Set SerialCode = %d , SerialCode1 = %d , SerialCode_All = %d where DBID = %d" , Role->SerialCode() , Role->SerialCode1() , Role->SerialCode_All() , Role->Base.DBID );
		MyDBProc.SqlCmd_WriteOneLine( Buf );
	}

	Print(LV2, "Global", "(**) Save角色(%s)　Time = %d", Utf8ToChar( Role->BaseData.RoleName.Begin() ).c_str() ,  timeGetTime() - TimeBegin );
    return true;
}
void   Global::_SaveRoleSQLCmdReusltEventCB ( vector<PlayerRoleData>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempSavePlayerStruct* tempData = (TempSavePlayerStruct*)UserObj;
	BaseItemObject* Obj = tempData->Obj;
	__try
	{
		if( Obj == NULL )
			return;
		
		Obj->IsSave( false );
		Obj->SaveTime( 12 * 30 );
		Obj->PopFromThread();
	}
	__finally
	{
		delete tempData;
	}
}


void		Global::SavePlayerDB( BaseItemObject* Obj )
{
    if( Obj->IsSave() == true )
    {
        return;
    }
    Obj->IsSave( true );

	TempSavePlayerStruct* tempData = NEW TempSavePlayerStruct;
	tempData->Obj = Obj;
	tempData->Role.Copy( Obj->Role() );
	Obj->IsNeedSave( false );

    Obj->PushToThread();
    _RoleDataDB->SqlCmd( Obj->Role()->Base.DBID , _SaveRoleSQLCmdRequestEvent , _SaveRoleSQLCmdReusltEventCB , tempData , NULL  );

}

bool Global::SendToCli_ByRoleName( const char* RoleName , int Size , void* Data )
{

	OnlinePlayerInfoStruct* Info = AllOnlinePlayerInfoClass::This()->GetInfo_ByName( (char*) RoleName );
	if( Info == NULL )
		return false;

	return _Net->SendToCli( Info->SockID , Info->ProxyID , Size , Data );
}

void		Global::UpdateAccountLogout( const char* Account )
{
	char Buf[256];
	sprintf( Buf , "UPDATE RoleData_Account SET IsLogin = 0 , LogoutTime=%d WHERE Account_ID = '%s'", TimeStr::Now_Value(), Account  );
	_RoleDataDB->SqlCmd( -1 , _SQLCmdRequestEvent , _SQLCmdReusltEventCB , NULL , "SqlCmd" , EM_ValueType_String , Buf , NULL );
}

bool   Global::_SQLCmdRequestEvent ( vector<PlayerRoleData>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass		MyDBProc( sqlBase );
	char* SqlCmd = Arg.GetString( "SqlCmd" );
	MyDBProc.SqlCmd_WriteOneLine( SqlCmd );
	return true;
}
void   Global::_SQLCmdReusltEventCB ( vector<PlayerRoleData>& ,void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{

}

struct TempGetRoleCountStruct
{
	string	Account_ID;
	int		RetPlayerCount;
};

void	Global::UpdateRolePlayerCount( const char* Account )
{
	TempGetRoleCountStruct* TempData = NEW TempGetRoleCountStruct;

	TempData->Account_ID = Account;
	TempData->RetPlayerCount = 0;

	_RD_NormalDB->SqlCmd( -1 , _SQLCmdGetRoleCountEvent , _SQLCmdGetRoleCountReusltEventCB , TempData , NULL );

}
bool   Global::_SQLCmdGetRoleCountEvent ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempGetRoleCountStruct* TempData = ( TempGetRoleCountStruct *)UserObj;
	char	SqlCmd[512];
	MyDbSqlExecClass		MyDBProc( sqlBase );
	sprintf( SqlCmd , "Select COUNT( DBID ) FROM RoleData WHERE Account_ID = '%s' and IsDelete = 0" , TempData->Account_ID.c_str() );
	int		RoleCount = 0;
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&TempData->RetPlayerCount );
	MyDBProc.Read();
	MyDBProc.Close();

	return true;
}
void   Global::_SQLCmdGetRoleCountReusltEventCB ( vector<int>& ,void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	_RD_AccountDB->SqlCmd( -1 , _SQLCmdWriteRoleCountEvent , _SQLCmdWriteRoleCountReusltEventCB , UserObj , NULL );
}


bool  Global::_SQLCmdWriteRoleCountEvent ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempGetRoleCountStruct* TempData = ( TempGetRoleCountStruct *)UserObj;

	char	SqlCmd[512];
	int		AccountCount = 0;
	MyDbSqlExecClass		MyDBProc( sqlBase );
	sprintf( SqlCmd , "Select COUNT( Account_ID ) FROM Game_RoleCount WHERE Account_ID = '%s'" , TempData->Account_ID.c_str() );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG	, SQL_C_DEFAULT		, (LPBYTE)&AccountCount );
	MyDBProc.Read();
	MyDBProc.Close();

	if( AccountCount == 0 )
	{
		sprintf_s( SqlCmd , sizeof(SqlCmd) , "INSERT Game_RoleCount( Account_ID,RoleCount%d ) VALUES('%s',%d)" 
			,Global::Net()->GetWorldID() , TempData->Account_ID.c_str() , TempData->RetPlayerCount );
	}
	else
	{
		sprintf_s( SqlCmd , sizeof(SqlCmd) , "UPDATE Game_RoleCount SET RoleCount%d = %d where Account_ID = '%s'"
			,Global::Net()->GetWorldID() , TempData->RetPlayerCount , TempData->Account_ID.c_str());
	}

	MyDBProc.SqlCmd_WriteOneLine( SqlCmd );
	return true;
}
void  Global::_SQLCmdWriteRoleCountReusltEventCB ( vector<int>& ,void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempGetRoleCountStruct* TempData = ( TempGetRoleCountStruct *)UserObj;

	delete TempData;
}
