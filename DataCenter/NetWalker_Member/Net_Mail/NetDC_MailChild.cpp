#include "NetDC_MailChild.h"	
#include "ReaderClass/CreateDBCmdClass.h"
#include "ReaderClass/DbSqlExecClass.h"
//-------------------------------------------------------------------
#define	_MAX_MAIL_COUNT_	300

int		CNetDC_MailChild::_LastProcTime = 0;
bool	CNetDC_MailChild::_ImportMailProc = false;
//-------------------------------------------------------------------
bool    CNetDC_MailChild::Init()
{
	CNetDC_Mail::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetDC_MailChild );


	_LastProcTime = clock();
	Schedular()->Push( _OnTimeProc , 60* 10 * 1000 , This , NULL );  
	Schedular()->Push( _OnTimeProc_ImportMail , 60*1000 , This , NULL );  
	//Schedular()->Push( _OnTimeProc_ImportMail , 10000 , This , NULL );  
	return true;
}
//--------------------------------------------------------------------------------------
bool    CNetDC_MailChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	CNetDC_Mail::_Release();
	return true;

}
//--------------------------------------------------------------------------------------
void CNetDC_MailChild::RL_SendMail	( int SrvSockID , int DBID , MailBaseInfoStruct& MailBaseInfo , char* Content , int TargetDBID )
{
	//要一個 Thread 處理
	DB_MailStruct* MailData = NEW( DB_MailStruct );
	
	MailData->OwnerDBID = -1;
	MailData->MailBase = MailBaseInfo;
	MailData->Content = Content;
	MailData->OrgSendDBID = DBID;

	char* ToRoleName;
	if( MailBaseInfo.IsSendBack == true )
	{
		ToRoleName = (char*)MailBaseInfo.OrgSendName.Begin();
	}
	else
	{
		ToRoleName = (char*)MailBaseInfo.OrgTargetName.Begin();
	}

	//金額有問題
	if( MailData->MailBase.Money < 0 )
	{
		SL_SendMailResult( SrvSockID , DBID 
			, MailBaseInfo.Money , MailBaseInfo.Money_Account , MailBaseInfo.Money_Bonus , MailBaseInfo.IsSendMoney , MailBaseInfo.IsSystem , EM_SendMailResult_MoneyError , ToRoleName );

		return;
	}
	
	if( MailData->MailBase.Money_Account < 0 )
	{
		SL_SendMailResult( SrvSockID , DBID 
			, MailBaseInfo.Money , MailBaseInfo.Money_Account , MailBaseInfo.Money_Bonus , MailBaseInfo.IsSendMoney , MailBaseInfo.IsSystem , EM_SendMailResult_AccountMoneyError , ToRoleName );

		return;
	}
	if( MailData->MailBase.Money_Bonus < 0 )
	{
		SL_SendMailResult( SrvSockID , DBID 
			, MailBaseInfo.Money , MailBaseInfo.Money_Account , MailBaseInfo.Money_Bonus , MailBaseInfo.IsSendMoney , MailBaseInfo.IsSystem , EM_SendMailResult_BonusMoneyError , ToRoleName );

		return;
	}


	if( MailData->MailBase.Money == 0 && MailData->MailBase.Money_Account == 0 )
		MailData->MailBase.IsSendMoney = true;


	_RD_MailDB->SqlCmd( 11 , _SQLCmdSendMailEvent , _SQLCmdSendMailResultEvent , MailData 
				, "SrvSockID"	, EM_ValueType_Int      , SrvSockID
				, "TargetDBID"	, EM_ValueType_Int      , TargetDBID
				, NULL );
}
void CNetDC_MailChild::RL_MailList	( int DBID , int CliSockID , int CliProxyID )
{
	_RD_MailDB->SqlCmd( DBID , _SQLCmdMailListEvent , _SQLCmdMailListResultEvent , NULL 
		, "DBID"		, EM_ValueType_Int      , DBID
		, "CliSockID"   , EM_ValueType_Int      , CliSockID
		, "CliProxyID"	, EM_ValueType_Int		, CliProxyID
		, NULL );
}

void CNetDC_MailChild::RL_SendBack	( int DBID , int MailDBID )
{
	_RD_MailDB->SqlCmd( DBID , _SQLCmdSendBackEvent , _SQLCmdSendBackResultEvent , NULL 
		, "DBID"		, EM_ValueType_Int      , DBID
		, "MailDBID"    , EM_ValueType_Int      , MailDBID
		, NULL );
}
void CNetDC_MailChild::RL_DelMail	( int DBID , int MailDBID )
{
	_RD_MailDB->SqlCmd( DBID , _SQLCmdDelMailEvent , _SQLCmdDelMailResultEvent , NULL 
		, "DBID"		, EM_ValueType_Int      , DBID
		, "MailDBID"    , EM_ValueType_Int      , MailDBID
		, NULL );
}
void CNetDC_MailChild::RL_ReadMail	( int DBID , int MailDBID  )
{
	_RD_MailDB->SqlCmd( DBID , _SQLCmdReadMailEvent , _SQLCmdReadMailResultEvent , NULL 
		, "DBID"		, EM_ValueType_Int      , DBID
		, "MailDBID"    , EM_ValueType_Int      , MailDBID
		, NULL );
}
void CNetDC_MailChild::RL_GetItem	( int SrvSockID , int DBID , int MailDBID , int Money , int Money_Account , int Money_Bonus )
{
	//_RD_MailDB->SqlCmd_Transcation( DBID , _SQLCmdGetItemEvent , _SQLCmdGetItemResultEvent , NULL 
	_RD_MailDB->SqlCmd( DBID , _SQLCmdGetItemEvent , _SQLCmdGetItemResultEvent , NULL 
		, "SrvSockID"		, EM_ValueType_Int      , SrvSockID
		, "DBID"			, EM_ValueType_Int      , DBID
		, "MailDBID"		, EM_ValueType_Int      , MailDBID
		, "Money"			, EM_ValueType_Int      , Money
		, "Money_Account"	, EM_ValueType_Int      , Money_Account
		, "Money_Bonus"		, EM_ValueType_Int      , Money_Bonus
		, NULL );
}
//--------------------------------------------------------------------------------------
//	寄件處理
//--------------------------------------------------------------------------------------
bool CNetDC_MailChild::_SQLCmdSendMailEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char		Buf[1024];
	bool        Result = true;
	int			_MaxMailDBID = 0;
	int			_MaxMailItemDBID = 0;

	DB_MailStruct* MailData = (DB_MailStruct*)UserObj;

	MyDbSqlExecClass	MyDBProc( sqlBase );

	CharTransferClass	CharTransfer;

	//------------------------------------------------------------------------------------------
	//1 搜尋目標是否存在
	int TargetDBID = -1;// Arg.GetNumber( "TargetDBID" );
	
	if( MailData->MailBase.OrgTargetName.Size() != 0 )
	{
		CharTransfer.SetUtf8String( MailData->MailBase.OrgTargetName.Begin() );
		string RoleNameBinStr = StringToSqlX( (char*)CharTransfer.GetWCString() , CharTransfer.WCStrLen() * 2 , false );

		sprintf( Buf , "SELECT DBID FROM RoleData Where RoleName = CAST( %s AS nvarchar(4000) ) and IsDelete = 0" , RoleNameBinStr.c_str() );

		MyDBProc.SqlCmd( Buf );
		MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TargetDBID );
		MyDBProc.Read( );
		MyDBProc.Close( );

		if( TargetDBID == -1 && MailData->MailBase.IsSystem )
		{
			sprintf( Buf , "SELECT DBID FROM RoleData Where RoleName = CAST( %s AS nvarchar(4000) )" , RoleNameBinStr.c_str() );

			MyDBProc.SqlCmd( Buf );
			MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TargetDBID );
			MyDBProc.Read( );
			MyDBProc.Close( );
		}
	}
	else
	{
		WCHAR wToName[256];
		TargetDBID = Arg.GetNumber( "TargetDBID" );

		sprintf( Buf , "SELECT RoleName FROM RoleData Where DBID = %d" , TargetDBID );

		MyDBProc.SqlCmd( Buf );
		MyDBProc.Bind( 1, SQL_WCHAR , sizeof(wToName)	, (LPBYTE)&wToName );
		if( MyDBProc.Read( ) == false )
			TargetDBID = -1;
		MyDBProc.Close( );

		CharTransfer.SetWCString( wToName );
		MailData->MailBase.OrgTargetName = CharTransfer.GetUtf8String();
	}


	if( TargetDBID == -1 )
	{
		//找不到
		Arg.PushValue( "ResultCode" , EM_SendMailResult_TargetNotFind );
		return false;
	}
	MailData->OwnerDBID = TargetDBID;
	//g_CritSect_Thread()->Enter();
	{
		//MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );
	   
		MyDBProc.SqlCmd( "SELECT MAX(GUID) AS Expr1 FROM Mail_Base " );
		MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&_MaxMailDBID );
		MyDBProc.Read();
		MyDBProc.Close();
		_MaxMailDBID++;

		// 3-2 儲存信件
		MailData->GUID = _MaxMailDBID;
		MailData->MailBase.IsSendBack = false;
		MailData->MailBase.IsRead = false;
		if( MailData->MailBase.IsSendMoney == false && MailData->MailBase.IsSendBack == false )
			MailData->MailBase.LiveTime = 60 * 12;
		else
			MailData->MailBase.LiveTime = 60 * 24 * 30;

		
		MyDBProc.SqlCmd_WriteOneLine( _RDMailBaseSql->GetInsertStr( MailData ).c_str() );
		//存物品
		for( int i = 0 ; i < 10 ; i++ )
		{
			if( MailData->MailBase.Item[i].IsEmpty() )
				continue;
			DB_MailItemStruct MailItem;
			MailItem.MailBaseGUID = MailData->GUID;
			MailItem.Item = MailData->MailBase.Item[i];

			MyDBProc.SqlCmd_WriteOneLine( _RDMailItemSql->GetInsertStr( &MailItem ).c_str() );
		}
	}
	//g_CritSect_Thread()->Leave();

	//MyDBProc.SqlCmd_WriteOneLine( "COMMIT TRANSACTION" );
	return true;
}
void CNetDC_MailChild::_SQLCmdSendMailResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	DB_MailStruct* MailData = (DB_MailStruct*)UserObj;

	int SrvSockID = Arg.GetNumber( "SrvSockID" );

	char* ToRoleName;
	if( MailData->MailBase.IsSendBack == true )
	{
		ToRoleName = (char*)MailData->MailBase.OrgSendName.Begin();
	}
	else
	{
		ToRoleName = (char*)MailData->MailBase.OrgTargetName.Begin();
	}

	if( ResultOK == true )
	{
		SC_NewMail( ToRoleName , MailData->MailBase.IsGift );
	}

	if( MailData->MailBase.IsSystem != true )
	{
		if( ResultOK == true )
		{
			SL_SendMailResult( SrvSockID , MailData->OrgSendDBID 
				, MailData->MailBase.Money , MailData->MailBase.Money_Account , MailData->MailBase.Money_Bonus , MailData->MailBase.IsSendMoney , MailData->MailBase.IsSystem , EM_SendMailResult_OK , ToRoleName );
		}
		else
		{
			SL_SendMailResult( SrvSockID , MailData->OrgSendDBID 
				, MailData->MailBase.Money , MailData->MailBase.Money_Account , MailData->MailBase.Money_Bonus , MailData->MailBase.IsSendMoney , MailData->MailBase.IsSystem , (SendMailResultEnum)Arg.GetNumber("ResultCode") , ToRoleName );
		}
	}
	delete MailData;
}
//--------------------------------------------------------------------------------------
// 要求信件列表
//--------------------------------------------------------------------------------------
bool CNetDC_MailChild::_SQLCmdMailListEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	//MutilThread_CritSect          Thread_CritSect;
	MyDbSqlExecClass	MyDBProc( sqlBase );

	int			DBID = Arg.GetNumber( "DBID" );
	char		Buf[256];

	//--------------------------------------------------------------------------------------
	//1 讀取種共有多少信件
	int		TotalMail = 0;

	sprintf( Buf , "Select COUNT(OwnerDBID) FROM Mail_Base WHERE DestroyTime = 0 AND OwnerDBID = %d" , DBID );

	MyDBProc.SqlCmd( Buf  );
	MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TotalMail );
	if( MyDBProc.Read() == false )
	{
		return false;
	}
	MyDBProc.Close();

	if( TotalMail == 0 )
		return false;

	Arg.PushValue( "TotalMail" , TotalMail );
	//--------------------------------------------------------------------------------------
	{
		//2 讀取前 30 封信
		DB_MailStruct  TempData;

		//上次時間處理到現在有多久
		int DuringTime = (clock() - _LastProcTime) / (1000*60);


		sprintf( Buf , "WHERE DestroyTime = 0 AND OwnerDBID = %d" , DBID );
		//g_CritSect()->Enter();
		MyDBProc.SqlCmd( _RDMailBaseSql->GetSelectStr( Buf , 30 ).c_str() );
		//g_CritSect()->Leave();

		MyDBProc.Bind( TempData , RoleDataEx::ReaderRD_MailBase() );
		while( MyDBProc.Read() )
		{
			RoleDataEx::ReaderRD_MailBase()->TransferWChartoUTF8( &TempData , MyDBProc.wcTempBufList() );
			if( TempData.OwnerDBID == TempData.OrgSendDBID )
				TempData.MailBase.IsSendBack = true;
			else
				TempData.MailBase.IsSendBack = false;

			TempData.MailBase.LiveTime -= DuringTime;
			if( TempData.MailBase.LiveTime < 0 )
				TempData.MailBase.LiveTime = 1;

			Data.push_back( TempData );
		}
		MyDBProc.Close();
	}
	//--------------------------------------------------------------------------------------
	// 3 讀取物品資料

	for( int i = 0 ; i < (int) Data.size() ; i++ )
	{
		DB_MailStruct&  ProcData = Data[ i ]; 
		ProcData.MailBase.initItem();

		sprintf( Buf , " WHERE DestroyTime = 0 AND MailBaseGUID = %d" , ProcData.GUID );
		DB_MailItemStruct MailItem;
		//g_CritSect()->Enter();
			MyDBProc.SqlCmd( _RDMailItemSql->GetSelectStr( Buf ).c_str() );
		//g_CritSect()->Leave();

		MyDBProc.Bind( MailItem , RoleDataEx::ReaderRD_MailItem() );
		for( int j = 0 ; j < 10 ; j++)
		{
			if( MyDBProc.Read() == false )
				break;

			ProcData.MailBase.Item[j] = MailItem.Item;
		}
		MyDBProc.Close();
	}

	return true;
}
void CNetDC_MailChild::_SQLCmdMailListResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	int DBID		= Arg.GetNumber( "DBID" );
	int CliSockID	= Arg.GetNumber( "CliSockID" );
	int CliProxyID	= Arg.GetNumber( "CliProxyID" );
	

	if( ResultOK == false )
	{
		SC_MailListCountResult( CliSockID , CliProxyID , 0 , 0  );
	}
	else
	{
		int TotalMail	= Arg.GetNumber( "TotalMail" );

		SC_MailListCountResult( CliSockID , CliProxyID , TotalMail , (int)Data.size()  );
		for( int i = 0 ; i < (int) Data.size() ; i++ )
		{
			DB_MailStruct&  ProcData = Data[ i ]; 
			if( ProcData.OrgSendDBID == -1 )
			{
				if( ProcData.MailBase.OrgSendName.Size() == 0 )
					ProcData.MailBase.OrgSendName = "[SYS_AC]";//g_ObjectData->GetString( "SYS_AC" );
			}
			SC_MailListResult( CliSockID , CliProxyID , ProcData.GUID , i , ProcData.MailBase , (char*)ProcData.Content.Begin() );
		}
	}
}
//--------------------------------------------------------------------------------------
// 退回信件
//--------------------------------------------------------------------------------------
bool CNetDC_MailChild::_SQLCmdSendBackEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	//MutilThread_CritSect          Thread_CritSect;
	MyDbSqlExecClass	MyDBProc( sqlBase );

	int			DBID = Arg.GetNumber("DBID");
	int			MailDBID = Arg.GetNumber("MailDBID");
	char		Buf[1024];
	
	//1 把信件找出來
	//  簡查擁有者
	//  檢查是否已退過信

	DB_MailStruct SendBackMail;

	sprintf( Buf , "WHERE DestroyTime = 0 AND GUID = %d" , MailDBID );
	//g_CritSect()->Enter();
		MyDBProc.SqlCmd( _RDMailBaseSql->GetSelectStr( Buf ).c_str()  );
	//g_CritSect()->Leave();

	bool IsFind = false;
	MyDBProc.Bind( SendBackMail , RoleDataEx::ReaderRD_MailBase() );
	if( MyDBProc.Read() == true )
	{
		IsFind = true;
		RoleDataEx::ReaderRD_MailBase()->TransferWChartoUTF8( &SendBackMail , MyDBProc.wcTempBufList() );
	}
	MyDBProc.Close();

	if( IsFind == false )
	{
		Arg.PushValue( "ErrorStr" , "讀取Mail 資料錯誤" );
		return false;
	}


	//如果DBID不是擁有者
	if( SendBackMail.OwnerDBID != DBID )
	{
		Arg.PushValue( "ErrorStr" , "要求退件者非信件擁有者" );
		return false;
	}

	if( SendBackMail.MailBase.IsSystem == true )
	{
		Arg.PushValue( "ErrorStr" , "系統信件不可退件" );
		return false;
	}
	
	if( SendBackMail.OrgSendDBID == SendBackMail.OwnerDBID )
	{
		Arg.PushValue( "ErrorStr" , "已退過的信不可再退一次" );
		return false;
	}
	//------------------------------------------------------------------------------------------
	//計算原始寄件者的郵件是否過多
	int	MailCount = 0;
	sprintf( Buf , "SELECT COUNT(GUID) AS Expr1 FROM Mail_Base Where OwnerDBID = %d and DestroyTime = 0" , SendBackMail.OrgSendDBID );

	MyDBProc.SqlCmd( Buf  );
	MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&MailCount );
	if( MyDBProc.Read() == false )
	{
		Arg.PushValue( "ErrorStr" , "取DB信件總數有問題" );
		return false;
	}
	MyDBProc.Close();

	if( MailCount > _MAX_MAIL_COUNT_ )
	{		
		Arg.PushValue( "ErrorStr" , "信件過多,刪除退件" );
		//----------------------------------------------------------------------------------------

//		sprintf( Buf , "DELETE Mail_Base Where GUID = %d" , MailDBID );
		sprintf( Buf , "UPDATE Mail_Base SET DestroyTime = getdate() Where GUID = %d" , MailDBID );

		if( MyDBProc.SqlCmd_WriteOneLine( Buf ) == false )
		{
			Arg.PushValue( "ErrorStr" , "刪除信件有問題" );
			return false;
		}
		//刪除此信件的物品
		sprintf( Buf , "UPDATE Mail_Item SET DestroyTime = getdate() Where MailBaseGUID = %d" , MailDBID );

		if( MyDBProc.SqlCmd_WriteOneLine( Buf ) == false )
		{
			Arg.PushValue( "ErrorStr" , "刪除信件有問題" );
			return false;
		}
	
		//----------------------------------------------------------------------------------------
	}
	else
	{
		if( SendBackMail.MailBase.IsSendMoney == true )
		{
			sprintf( Buf , "UPDATE Mail_Base SET OwnerDBID = %d , LiveTime = 43200  Where GUID = %d" , SendBackMail.OrgSendDBID , MailDBID );
		}
		else
		{
			sprintf( Buf , "UPDATE Mail_Base SET OwnerDBID = %d , LiveTime = 43200 , IsSendMoney = 0 , Money = 0 Where GUID = %d" , SendBackMail.OrgSendDBID , MailDBID );
		}

		if( MyDBProc.SqlCmd_WriteOneLine( Buf ) == false )
		{
			Arg.PushValue( "ErrorStr" , "退信更新資料庫有問題" );
			return false;
		}
	}

	return true;
}
void CNetDC_MailChild::_SQLCmdSendBackResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	//不做回傳處理
}
//--------------------------------------------------------------------------------------
// 刪除信件
//--------------------------------------------------------------------------------------
bool CNetDC_MailChild::_SQLCmdDelMailEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass	MyDBProc( sqlBase );
	int			DBID = Arg.GetNumber("DBID");
	int			MailDBID = Arg.GetNumber("MailDBID");
	char		Buf[1024];


	int			OwnerDBID = -1;
//	int			ItemGUID = -1;
	int			Money = 0;
	bool		IsSendMoney;
//-----------------------------------------------------------------------------------------------------
	//1 把信件找出來
	//  簡查擁有者

	sprintf( Buf , "Select OwnerDBID , IsSendMoney , Money FROM Mail_Base WHERE GUID = %d" , MailDBID );

	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&OwnerDBID		);
//	MyDBProc.Bind( 2, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&ItemGUID		);
	MyDBProc.Bind( 2, SQL_C_BIT  , SQL_C_DEFAULT , (LPBYTE)&IsSendMoney	);
	MyDBProc.Bind( 3, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&Money			);
	if( MyDBProc.Read() == false )
	{
		Arg.PushValue( "ErrorStr" , "讀取Mail 資料錯誤" );
		return false;
	}
	MyDBProc.Close();

	if( OwnerDBID != DBID )
	{
		Arg.PushValue( "ErrorStr" , "非信件擁有者，不可刪除此信件" );
		return false;
	}

	//如果為貨到付款的不能刪
	if( IsSendMoney == false && Money > 0 )
	{
		Arg.PushValue( "ErrorStr" , "貨到付款，不可刪信" );
		return false;
	}

	//-----------------------------------------------------------------------------------------------------
	/*
	sprintf( Buf , "DELETE Mail_Base Where GUID = %d" , MailDBID );
	
	if( MyDBProc.SqlCmd_WriteOneLine( Buf ) == false )
	{
		Arg.PushValue( "ErrorStr" , "刪除信件有問題" );
		return false;
	}*/
	sprintf( Buf , "UPDATE Mail_Base SET DestroyTime = getdate() Where GUID = %d" , MailDBID );

	if( MyDBProc.SqlCmd_WriteOneLine( Buf ) == false )
	{
		Arg.PushValue( "ErrorStr" , "刪除信件有問題" );
		return false;
	}
	//刪除此信件的物品
	sprintf( Buf , "UPDATE Mail_Item SET DestroyTime = getdate() Where MailBaseGUID = %d" , MailDBID );

	if( MyDBProc.SqlCmd_WriteOneLine( Buf ) == false )
	{
		Arg.PushValue( "ErrorStr" , "刪除信件有問題" );
		return false;
	}


	return true;
}
void CNetDC_MailChild::_SQLCmdDelMailResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	//目前不處理
}
//--------------------------------------------------------------------------------------
//	讀取信件
//--------------------------------------------------------------------------------------
bool CNetDC_MailChild::_SQLCmdReadMailEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	MyDbSqlExecClass	MyDBProc( sqlBase );

	int			DBID = Arg.GetNumber("DBID");
	int			MailDBID = Arg.GetNumber("MailDBID");
	char		Buf[256];

	if( DBID == -1 )
		return false;
	

	//---------------------------------------------------------------------------------------------------
	//讀取信件

	int		OwnerDBID = -1;
	int		LiveTime;

	sprintf( Buf , "SELECT OwnerDBID , LiveTime FROM Mail_Base WHERE DestroyTime = 0 AND GUID = %d" , MailDBID );
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&OwnerDBID	);
	MyDBProc.Bind( 2, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&LiveTime	);
	if( MyDBProc.Read() == false )
	{
		return false;
	}
	MyDBProc.Close();

	//---------------------------------------------------------------------------------------------------
	//擁有者檢查
	if( OwnerDBID != DBID )
	{
		Arg.PushValue( "ErrCode" , "擁有者有問題" );
		return false;
	}
	//---------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------
	if( 60*24*3 < LiveTime )
		LiveTime = 60*24*3;

	//更新信件為已讀取
	sprintf( Buf , "UPDATE Mail_Base SET IsRead = 1 , LiveTime = %d WHERE GUID = %d" , LiveTime , MailDBID);

	if( MyDBProc.SqlCmd_WriteOneLine( Buf ) == false )
	{
		return false;
	}


	//---------------------------------------------------------------------------------------------------
	return true;
}
void CNetDC_MailChild::_SQLCmdReadMailResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{

}
//--------------------------------------------------------------------------------------
//取物品
//--------------------------------------------------------------------------------------
bool CNetDC_MailChild::_SQLCmdGetItemEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	//MutilThread_CritSect    Thread_CritSect;
	MyDbSqlExecClass		MyDBProc( sqlBase );

	int						DBID			= Arg.GetNumber("DBID");
	int						MailDBID		= Arg.GetNumber("MailDBID");
	int						Money			= Arg.GetNumber("Money");
	int						Money_Account	= Arg.GetNumber("Money_Account");
	int						Money_Money		= Arg.GetNumber("Money_Money");
	int						Money_Bonus		= Arg.GetNumber("Money_Bonus");
	DB_MailStruct			TempMail;
	char					Buf[256];

	if( DBID == -1 )
		return false;


	//---------------------------------------------------------------------------------------------------
	//讀取信件
	sprintf( Buf , "WHERE GUID = %d" , MailDBID );
	//g_CritSect()->Enter();
		MyDBProc.SqlCmd( _RDMailBaseSql->GetSelectStr( Buf ).c_str() );
	//g_CritSect()->Leave();

	MyDBProc.Bind( TempMail , RoleDataEx::ReaderRD_MailBase() );
	if( MyDBProc.Read() == false )
	{
		Arg.PushValue( "ErrCode" , "找不到資料" );
		MyDBProc.Close();
		return false;
	}
	RoleDataEx::ReaderRD_MailBase()->TransferWChartoUTF8( &TempMail , MyDBProc.wcTempBufList() );
	MyDBProc.Close();

	//---------------------------------------------------------------------------------------------------
	//擁有者檢查
	if( TempMail.OwnerDBID != DBID )
	{
		Arg.PushValue( "ErrCode" , "擁有者有問題" );
		return false;
	}
	//---------------------------------------------------------------------------------------------------
	
	TempMail.MailBase.initItem();
	
	sprintf( Buf , "WHERE DestroyTime = 0 AND MailBaseGUID = %d " , TempMail.GUID );
	//g_CritSect()->Enter();
	MyDBProc.SqlCmd( _RDMailItemSql->GetSelectStr( Buf ).c_str() );
	//g_CritSect()->Leave();

	DB_MailItemStruct MailItem;

	MyDBProc.Bind( MailItem , RoleDataEx::ReaderRD_MailItem() );
	for( int i = 0 ; i < 10 ; i++ )
	{
		if( MyDBProc.Read() == false )
			break;

		TempMail.MailBase.Item[i] = MailItem.Item;
	}
	MyDBProc.Close();


	//---------------------------------------------------------------------------------------------------
	//金額比較
	if( TempMail.MailBase.IsSendMoney == false && TempMail.MailBase.Money != 0 )
	{
		int	PayMoney =  TempMail.MailBase.Money * -1;

		if( Money != PayMoney )
		{	
			//金錢有問題
			return false;
		}
		Arg.PushValue( "SendBackMoney" , TempMail.MailBase.Money );
	}
	else
	{
		if( Money != TempMail.MailBase.Money )
		{
			return false;
		}
	}

	if( TempMail.MailBase.IsSendMoney == false && TempMail.MailBase.Money_Account != 0 )
	{
		int	PayMoney =  TempMail.MailBase.Money_Account * -1;

		if( Money_Account != PayMoney )
		{	
			//金錢有問題
			return false;
		}
		Arg.PushValue( "SendBackMoney_Account" , TempMail.MailBase.Money_Account );
	}
	else
	{
		if( Money_Account != TempMail.MailBase.Money_Account )
		{
			return false;
		}
	}

	if( TempMail.MailBase.IsSendMoney == false && TempMail.MailBase.Money_Bonus != 0 )
	{
		int	PayMoney =  TempMail.MailBase.Money_Bonus * -1;

		if( Money_Bonus != PayMoney )
		{	
			//金錢有問題
			return false;
		}
		Arg.PushValue( "SendBackMoney_Bonus" , TempMail.MailBase.Money_Bonus );
	}
	else
	{
		if( Money_Bonus != TempMail.MailBase.Money_Bonus )
		{
			return false;
		}
	}
	//---------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------
	//刪除物品
	sprintf( Buf , "UPDATE Mail_Base SET Money = 0 , Money_Bonus = 0 , Money_Account = 0 WHERE GUID = %d" , MailDBID );

	if( MyDBProc.SqlCmd_WriteOneLine( Buf ) == false )
	{
		Arg.PushValue( "ErrorStr" , "刪除信件有問題" );
		return false;
	}

	//刪除此信件的物品
	sprintf( Buf , "UPDATE Mail_Item SET DestroyTime = getdate() Where MailBaseGUID = %d" , MailDBID );

	if( MyDBProc.SqlCmd_WriteOneLine( Buf ) == false )
	{
		Arg.PushValue( "ErrorStr" , "刪除信件有問題" );
		return false;
	}

	/*
	sprintf( Buf , "DELETE Mail_Item Where MailBaseGUID = %d" , MailDBID );
	MyDBProc.SqlCmd_WriteOneLine( Buf );
*/
	//---------------------------------------------------------------------------------------------------
	Data.push_back( TempMail );
	//---------------------------------------------------------------------------------------------------
	return true;
}
void CNetDC_MailChild::_SQLCmdGetItemResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	int			SrvSockID = Arg.GetNumber("SrvSockID");
	int			DBID = Arg.GetNumber("DBID");
	int			MailDBID = Arg.GetNumber("MailDBID");
	int			Money = Arg.GetNumber("Money");
	int			Money_Account = Arg.GetNumber("Money_Account");
	int			Money_Bonus = Arg.GetNumber("Money_Bonus");
	int			SendBackMoney = Arg.GetNumber( "SendBackMoney" );
	int			SendBackMoney_Account = Arg.GetNumber( "SendBackMoney_Account" );
	int			SendBackMoney_Bonus = Arg.GetNumber( "SendBackMoney_Bonus" );

	if( ResultOK == false || Data.size() != 1 )
	{
		ItemFieldStruct Temp[10];
		SL_GetItemResult	( SrvSockID , DBID , -1 , MailDBID , false , Money , Money_Account , Money_Bonus , Temp );
	}
	else
	{
		SL_GetItemResult	( SrvSockID , DBID , Data[0].OrgSendDBID , MailDBID , true , Money , Money_Account , Money_Bonus , Data[0].MailBase.Item );

		MailBaseInfoStruct MailBaseInfo;
		MailBaseInfo.initItem();	
		MailBaseInfo.Money = SendBackMoney;
		MailBaseInfo.Money_Account = SendBackMoney_Account;
		MailBaseInfo.Money_Bonus = SendBackMoney_Bonus;
		MailBaseInfo.IsSendMoney = true;
		MailBaseInfo.IsSystem = true;				//是否為系統信件
		MailBaseInfo.IsRead = false;;
		MailBaseInfo.IsSendBack = false;			//是否為退回信件		
		MailBaseInfo.IsGift = false;

		MailBaseInfo.OrgSendName = Data[0].MailBase.OrgTargetName;
		MailBaseInfo.OrgTargetName = Data[0].MailBase.OrgSendName;

		MailBaseInfo.Title = "[SYS_MAIL_PAY_AND_GET]";//g_ObjectData->GetString( "SYS_MAIL_PAY_AND_GET" );

		char	Buf[256];
		sprintf( Buf , "[SYS_MAIL_COD_MESSAGE][$SETVAR1=%s][$SETVAR2=[%d]]" , MailBaseInfo.OrgSendName.Begin() , Data[0].MailBase.Item[0].OrgObjID );
		if( SendBackMoney != 0 )
		{
			This->RL_SendMail	( SrvSockID , DBID , MailBaseInfo , Buf , -1 );
		}
		
	}
}
//--------------------------------------------------------------------------------------
struct TemnpImportMailStruct
{
	int		GUID;
	char	ToName[32];
	ItemFieldStruct Item;
	int		Money;
	int		Money_Account;
	int		Money_Bonus;
	char	Title[50];
	char	Content[2048];
};
int  CNetDC_MailChild::_OnTimeProc_ImportMail( SchedularInfo* Obj , void* InputClass )
{
	if( Global::_IsDestory == true )
		return 0;

	static vector< TemnpImportMailStruct > MailList;

	if( _ImportMailProc == false  )
	{
		_ImportMailProc = true;
		_RD_NormalDB_Import->SqlCmd( 13 , _SQLCmdOnTimeProcImportMailEvent , _SQLCmdOnTimeProcImportMailResultEvent , &MailList , NULL );
	}

	Schedular()->Push( _OnTimeProc_ImportMail , 1000*60 , This , NULL );  
	return 0;
}

bool CNetDC_MailChild::_SQLCmdOnTimeProcImportMailEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	vector< TemnpImportMailStruct >& MailList = *( (vector< TemnpImportMailStruct >*)UserObj );
	
	MyDbSqlExecClass	MyDBProc( sqlBase );
	char				Buf[256];


	TemnpImportMailStruct	TempMail;
	memset( &TempMail , 0 , sizeof(TempMail) );

	wchar_t		wToName[32];
	wchar_t		wTitle[32];
	wchar_t		wContent[ 2048 ];

	//取出所有的信件
	sprintf( Buf , "SELECT Top 2000 GUID ,ToName, OrgObjID, Serial, CreateTime, Count, Durable, Ability, Mode, ExValue, ImageObjectID, Title, [Content], Money, Money_Account,Money_Bonus FROM ImportMail WHERE WorldID=%d and IsSend= 0" , Global::GetWorldID() );
	MyDBProc.SqlCmd( Buf );

	MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempMail.GUID );
//	MyDBProc.Bind( 2, SQL_C_CHAR , sizeof(TempMail.ToName) , (LPBYTE)&TempMail.ToName );
	MyDBProc.Bind( 2, SQL_WCHAR  , sizeof(wToName) , (LPBYTE)&wToName );
	MyDBProc.Bind( 3, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempMail.Item.OrgObjID );
	MyDBProc.Bind( 4, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempMail.Item.Serial );
	MyDBProc.Bind( 5, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempMail.Item.CreateTime );
	MyDBProc.Bind( 6, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempMail.Item.Count );
	MyDBProc.Bind( 7, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempMail.Item.Durable );
	MyDBProc.Bind( 8, SQL_C_BINARY , sizeof(TempMail.Item.Ability) , (LPBYTE)&TempMail.Item.Ability );
	MyDBProc.Bind( 9, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempMail.Item.Mode );
	MyDBProc.Bind( 10, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempMail.Item.ExValue );
	MyDBProc.Bind( 11, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempMail.Item.ImageObjectID );
//	MyDBProc.Bind( 12, SQL_C_CHAR , sizeof(TempMail.Title) , (LPBYTE)&TempMail.Title );
//	MyDBProc.Bind( 13, SQL_C_CHAR , sizeof(TempMail.Content) , (LPBYTE)&TempMail.Content );
	MyDBProc.Bind( 12, SQL_WCHAR , sizeof(wTitle)	, (LPBYTE)&wTitle );
	MyDBProc.Bind( 13, SQL_WCHAR , sizeof(wContent) , (LPBYTE)&wContent );
	MyDBProc.Bind( 14, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempMail.Money );
	MyDBProc.Bind( 15, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempMail.Money_Account );
	MyDBProc.Bind( 16, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempMail.Money_Bonus );

	while( MyDBProc.Read() )
	{
		CharTransferClass	CharTransfer;

		CharTransfer.SetWCString( wToName );
		MyStrcpy( TempMail.ToName , CharTransfer.GetUtf8String() , sizeof(TempMail.ToName) );

		CharTransfer.SetWCString( wTitle );
		MyStrcpy( TempMail.Title , CharTransfer.GetUtf8String() , sizeof(TempMail.Title) );

		CharTransfer.SetWCString( wContent );
		MyStrcpy( TempMail.Content , CharTransfer.GetUtf8String() , sizeof(TempMail.Content) );

		MailList.push_back( TempMail );
	}
	MyDBProc.Close();

	for( unsigned i = 0 ; i < MailList.size() ; i++ )
	{
		sprintf( Buf , "UPDATE ImportMail SET IsSend = 1 WHERE GUID=%d " , MailList[i].GUID );	
		if( MyDBProc.SqlCmd_WriteOneLine( Buf ) == false )
		{
			MailList.erase( MailList.begin() + i );
			i--;
		}
	}

	return true;
}
void CNetDC_MailChild::_SQLCmdOnTimeProcImportMailResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	static int SerialID = 100000;
	vector< TemnpImportMailStruct >& MailList = *( (vector< TemnpImportMailStruct >*)UserObj );

	_ImportMailProc = false;
	
	MailBaseInfoStruct  MailBaseInfo;
	memset( &MailBaseInfo , 0 , sizeof(MailBaseInfo) );
	MailBaseInfo.IsSendMoney = true;
	MailBaseInfo.LiveTime = 60 * 24 * 30;
	MailBaseInfo.PaperType = EM_MailBackGroundType_System;
	MailBaseInfo.IsSystem = true;

	for( unsigned i = 0 ; i < MailList.size() ; i++ )
	{
		ItemFieldStruct& Item = MailList[i].Item;
		//取出物品資訊
		GameObjDbStructEx* ItemDB = g_ObjectData->GetObj( Item.OrgObjID );
		if( ItemDB != NULL )
		{
			if( Item.CreateTime ==  0 && Item.Serial == 0 )
			{
				Item.CreateTime = RoleDataEx::G_Now;
				Item.Serial = SerialID++;
			}
		}
		
		if( Item.Mode._Mode == -1 && ItemDB != NULL )
		{
			Item.Mode._Mode = 0;
			Item.Mode.Trade = !(ItemDB->Mode.PickupBound);
			if( ItemDB->IsArmor() || ItemDB->IsWeapon() )
				Item.Mode.EQSoulBound = ItemDB->Mode.EQSoulBound;
			else
				Item.Mode.EQSoulBound = false;

			Item.Mode.HideCount = ItemDB->Mode.HideCount;
		}

		if( ItemDB->IsWeapon() || ItemDB->IsArmor() )
		{
			if( Item.ExValue == 0 )
			{
				Item.OrgQuality = 100;		//原始的品質
				Item.Quality = 100;		//品質
				Item.PowerQuality = 10;	//威力品質( 1 為 10%強度)
				Item.Level = 0;	//等級
				Item.RuneVolume = 0;	//符文容量			
			}
			if( ItemDB->Mode.DurableStart0 == true )
				Item.Durable = 0;
			else
				Item.Durable = ItemDB->Item.Durable * Item.Quality;
		}

		MailBaseInfo.Item[0] = MailList[i].Item;
		MailBaseInfo.Money = MailList[i].Money;
		MailBaseInfo.Money_Account = MailList[i].Money_Account;
		MailBaseInfo.Money_Bonus = MailList[i].Money_Bonus;
		MailBaseInfo.Title = MailList[i].Title;
		MailBaseInfo.OrgTargetName = MailList[i].ToName;	
		SendMail( -1 , MailBaseInfo , MailList[i].Content );
	}

	MailList.clear();
}

int  CNetDC_MailChild::_OnTimeProc( SchedularInfo* Obj , void* InputClass )
{
	if( Global::_IsDestory == true )
		return 0;

	_RD_MailDB->SqlCmd( 12 , _SQLCmdOnTimeProc , _SQLCmdOnTimeProcResult , NULL 
		, NULL );

	Schedular()->Push( _OnTimeProc , 1000*60*60 , This , NULL );  
	return 0;
}


bool CNetDC_MailChild::_SQLCmdOnTimeProc( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	struct	TempMailData
	{
		int		GUID;
		int		OwnerDBID;
		int		OrgSendDBID;
		bool	IsSendMoney;
	};


	MyDbSqlExecClass	MyDBProc( sqlBase );
	char				Buf[256];
	
	int DuringTime = (clock() - _LastProcTime) / (1000*60);
	_LastProcTime = clock();

//	MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );
	//------------------------------------------------------------------------------------------
	//所有信件減時間	
	sprintf( Buf , "UPDATE Mail_Base SET LiveTime = LiveTime - %d WHERE DestroyTime = 0" , DuringTime );
	
	MyDBProc.SqlCmd_WriteOneLine( Buf );

	//----------------------------------------------------------------------------------------
	//刪除所有 自己送出的信 與 系統信
//	sprintf( Buf ,"DELETE Mail_Base WHERE LiveTime < 0 AND ( OrgSendDBID = OwnerDBID OR IsSystem = 1 ) " );
	sprintf( Buf ,"UPDATE Mail_Base SET DestroyTime = getdate() WHERE DestroyTime = 0 AND LiveTime < 0 AND ( OrgSendDBID = OwnerDBID OR IsSystem = 1 ) " );
	MyDBProc.SqlCmd_WriteOneLine( Buf );
	//----------------------------------------------------------------------------------------
	//取出所有的過期的信件
	TempMailData TempData;
	
	vector< TempMailData >	TimeOutMailList;
	sprintf( Buf , "SELECT GUID , OwnerDBID , OrgSendDBID , IsSendMoney FROM Mail_Base WHERE DestroyTime = 0 AND LiveTime < 0" );
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempData.GUID );
	MyDBProc.Bind( 2, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempData.OwnerDBID );
	MyDBProc.Bind( 3, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TempData.OrgSendDBID );
	MyDBProc.Bind( 4, SQL_C_BIT  , SQL_C_DEFAULT , (LPBYTE)&TempData.IsSendMoney );
	while( MyDBProc.Read() )
	{
		TimeOutMailList.push_back( TempData );
	}
	MyDBProc.Close();
	//----------------------------------------------------------------------------------------
	for( int i =0 ; i < (int)TimeOutMailList.size() ; i++ )
	{
		int		OrgSenderMailCount	= 0;
		int		ItemCount			= 0;

		TempMailData& Data = TimeOutMailList[i];
		
		sprintf( Buf , "SELECT COUNT(MailBaseGUID) FROM Mail_Item WHERE DestroyTime = 0 AND MailBaseGUID = %d" , Data.GUID );
		MyDBProc.SqlCmd( Buf );
		MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&ItemCount );
		MyDBProc.Read();
		MyDBProc.Close();
		
		if( ItemCount != 0 )
		{
			sprintf( Buf , "SELECT COUNT(GUID) AS Expr1 FROM Mail_Base WHERE DestroyTime = 0 AND OwnerDBID = %d" , Data.OrgSendDBID );
			MyDBProc.SqlCmd( Buf );
			MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&OrgSenderMailCount );
			MyDBProc.Read();
			MyDBProc.Close();
		}


		if(		OrgSenderMailCount > _MAX_MAIL_COUNT_ 
			||	ItemCount == 0 )
		{	//刪信	
			sprintf( Buf , "UPDATE Mail_Base SET DestroyTime = getdate() Where GUID = %d" , Data.GUID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
			sprintf( Buf , "UPDATE Mail_Item SET DestroyTime = getdate() Where MailBaseGUID = %d" , Data.GUID );
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
		else
		{
			//退信
			if( Data.IsSendMoney == true )
				sprintf( Buf , "UPDATE Mail_Base SET OwnerDBID = %d , LiveTime = 43200  Where GUID = %d" , Data.OrgSendDBID , Data.GUID );
			else
				sprintf( Buf , "UPDATE Mail_Base SET OwnerDBID = %d , LiveTime = 43200 , IsSendMoney = 1 , Money = 0 Where GUID = %d" , Data.OrgSendDBID , Data.GUID );

			MyDBProc.SqlCmd_WriteOneLine( Buf  );
		}
	}
//	MyDBProc.SqlCmd_WriteOneLine( "COMMIT TRANSACTION" );
	//----------------------------------------------------------------------------------------



	//----------------------------------------------------------------------------------------
	return true;
}
void CNetDC_MailChild::_SQLCmdOnTimeProcResult ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	
}

void CNetDC_MailChild::SendMail		(  int DBID , MailBaseInfoStruct& MailBaseInfo , char* Content )
{
	This->RL_SendMail	( -1 , DBID , MailBaseInfo , Content , -1 );
}

void	CNetDC_MailChild::SysSendMoney	( int DBID , const char* Name , int Money , int Money_Account , int Money_Bonus, const char* Title , const char* Content , int ACID )
{
	MailBaseInfoStruct MailBaseInfo;

	MailBaseInfo.initItem();
	MailBaseInfo.Money = Money;
	MailBaseInfo.Money_Account = Money_Account;
	MailBaseInfo.Money_Bonus = Money_Bonus;

	MailBaseInfo.IsSendMoney = true;
	MailBaseInfo.IsSystem = true;			//是否為系統信件
	MailBaseInfo.IsRead = false;
	MailBaseInfo.IsSendBack = false;			//是否為退回信件
//	MailBaseInfo.IsFromAC = IsFromAC;
	MailBaseInfo.AcDBID = ACID;
	MailBaseInfo.IsGift = false;

	MailBaseInfo.OrgSendName = "[SYS_AC]";//g_ObjectData->GetString( "SYS_AC" );
	MailBaseInfo.OrgTargetName = Name;

	MailBaseInfo.Title = Title;
	CNetDC_MailChild::SendMail		( DBID , MailBaseInfo , (char*)Content );

}

void	CNetDC_MailChild::SysSendItem ( int DBID , const char* Name , ItemFieldStruct& Item , const char* Title , int ACID )
{
	
	GameObjDbStructEx* ItemDB = g_ObjectData->GetObj( Item.OrgObjID );
	if( ItemDB->IsItem() && ItemDB->Item.ItemType == EM_ItemType_Money )
	{
	//	int	AccountObjID = g_ObjectData->GetSysKeyValue( "Money_AccountObjID" );

		if( Item.OrgObjID == g_ObjectData->SysValue().Money_AccountObjID )
		{
			CNetDC_MailChild::SysSendMoney ( DBID  , Name  , 0 , Item.Money , 0  , Title );
		}
		else if( Item.OrgObjID == g_ObjectData->SysValue().Money_GoldObjID )
		{
			CNetDC_MailChild::SysSendMoney ( DBID  , Name  , Item.Money , 0 , 0 , Title );
		}
		else if( Item.OrgObjID == g_ObjectData->SysValue().Money_BonusObjID )
		{
			CNetDC_MailChild::SysSendMoney ( DBID  , Name  , 0 , 0 , Item.Money , Title );
		}

		return;
	}

	MailBaseInfoStruct MailBaseInfo;
	MailBaseInfo.initItem();

	MailBaseInfo.Item[0] = Item;
	MailBaseInfo.Money = 0;
	MailBaseInfo.Money_Account = 0;
	MailBaseInfo.Money_Bonus = 0;


	MailBaseInfo.IsSendMoney = true;
	MailBaseInfo.IsSystem = true;			//是否為系統信件
	MailBaseInfo.IsRead = false;
	MailBaseInfo.IsSendBack = false;			//是否為退回信件
	MailBaseInfo.AcDBID = ACID;
	MailBaseInfo.IsGift = false;

	MailBaseInfo.OrgSendName = "[SYS_AC]";//g_ObjectData->GetString( "SYS_AC" );
	MailBaseInfo.OrgTargetName = Name;
	MailBaseInfo.PaperType = EM_MailBackGroundType_Normal;

	MailBaseInfo.Title = Title;
	CNetDC_MailChild::SendMail		( DBID , MailBaseInfo , "" );

}

void	CNetDC_MailChild::SysSendMail		( int DBID , const char* Name , ItemFieldStruct& Item , const char* Title , int Money , int Money_Account , int ACID )
{
	MailBaseInfoStruct MailBaseInfo;
	MailBaseInfo.initItem();

	MailBaseInfo.Item[0] = Item;
	MailBaseInfo.Money = Money;
	MailBaseInfo.Money_Account = Money_Account;


	MailBaseInfo.IsSendMoney = true;
	MailBaseInfo.IsSystem = true;			//是否為系統信件
	MailBaseInfo.IsRead = false;
	MailBaseInfo.IsSendBack = false;			//是否為退回信件
	MailBaseInfo.AcDBID = ACID;
	MailBaseInfo.IsGift = false;

	MailBaseInfo.OrgSendName = "[SYS_AC]";//g_ObjectData->GetString( "SYS_AC" );
	MailBaseInfo.OrgTargetName = Name;

	MailBaseInfo.Title = Title;
	CNetDC_MailChild::SendMail		( DBID , MailBaseInfo , "" );

}

void	CNetDC_MailChild::RL_UnReadMailCountRequest	( int DBID , int CliSockID , int CliProxyID )
{
	_RD_MailDB->SqlCmd( DBID , _SQLCmdMailCountEvent , _SQLCmdMailCountResultEvent , NULL 
		, "DBID"		, EM_ValueType_Int      , DBID
		, "CliSockID"   , EM_ValueType_Int      , CliSockID
		, "CliProxyID"	, EM_ValueType_Int		, CliProxyID
		, NULL );
}

bool CNetDC_MailChild::_SQLCmdMailCountEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{

	//MutilThread_CritSect          Thread_CritSect;
	MyDbSqlExecClass	MyDBProc( sqlBase );

	int			DBID = Arg.GetNumber( "DBID" );
	char		Buf[256];

	//--------------------------------------------------------------------------------------
	//1 讀取種共有多少信件
	int		UnReadMail = 0;

	sprintf( Buf , "Select COUNT(OwnerDBID) FROM Mail_Base WHERE DestroyTime = 0 and OwnerDBID = %d and IsRead = 0" , DBID );

	MyDBProc.SqlCmd( Buf  );
	MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&UnReadMail );
	if( MyDBProc.Read() == false )
	{
		return false;
	}
	MyDBProc.Close();

	Arg.PushValue( "UnReadMail" , UnReadMail );

	int		GiftCount = 0;
	sprintf( Buf , "Select COUNT(OwnerDBID) FROM Mail_Base WHERE DestroyTime = 0 and OwnerDBID = %d and IsRead = 0 and IsGift = 1" , DBID );

	MyDBProc.SqlCmd( Buf  );
	MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&GiftCount );
	if( MyDBProc.Read() == false )
	{
		return false;
	}
	MyDBProc.Close();

	Arg.PushValue( "GiftCount" , GiftCount );

	return true;	
}
void CNetDC_MailChild::_SQLCmdMailCountResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	int DBID		= Arg.GetNumber( "DBID" );
	int CliSockID	= Arg.GetNumber( "CliSockID" );
	int CliProxyID	= Arg.GetNumber( "CliProxyID" );
	int UnReadMail	= Arg.GetNumber( "UnReadMail" );
	int	GiftCount	= Arg.GetNumber( "GiftCount" );

	if( ResultOK == true )
	{
		SC_UnReadMailCount( CliSockID , CliProxyID , UnReadMail , GiftCount );
	}

}
//////////////////////////////////////////////////////////////////////////
struct CheckSendMailTempStruct
{
	int SrvSockID;
	int PlayerDBID;
	string TargetName;
	int TargetDBID;
	int	MapKey;

};
void CNetDC_MailChild::RL_CheckSendMail( int SrvSockID , int PlayerDBID , char* TargetName , int MapKey  )
{
	CheckSendMailTempStruct* TempData = NEW(CheckSendMailTempStruct);

	TempData->SrvSockID = SrvSockID;
	TempData->PlayerDBID = PlayerDBID;
	TempData->TargetName = TargetName;
	TempData->MapKey = MapKey;
	TempData->TargetDBID = -1;

	_RD_NormalDB->SqlCmd( PlayerDBID , _SQLCmdCheckSendMailEvent , _SQLCmdCheckSendMailResultEvent , TempData  , NULL );
}

bool CNetDC_MailChild::_SQLCmdCheckSendMailEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	CheckSendMailTempStruct* TempData = (CheckSendMailTempStruct*)UserObj;

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
		Arg.PushValue( "ResultCode" , EM_SendMailResult_TargetNotFind );
		return false;
	}
	TempData->TargetDBID = TargetDBID;
	//------------------------------------------------------------------------------------------
	//2 目標的信件是否過多
	int	TargetMailCount = 0;

	sprintf( Buf , "SELECT COUNT(GUID) AS Expr1 FROM Mail_Base Where DestroyTime = 0 AND OwnerDBID = %d" , TargetDBID );

	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1, SQL_C_LONG , SQL_C_DEFAULT , (LPBYTE)&TargetMailCount );
	if( MyDBProc.Read( ) == false )
	{
		MyDBProc.Close( );
		Arg.PushValue( "ResultCode" , EM_SendMailResult_Unknow );
		return false;
	}
	MyDBProc.Close( );

	if( TargetMailCount > _MAX_MAIL_COUNT_  )
	{
		//找不到
		Arg.PushValue( "ResultCode" , EM_SendMailResult_TargetMailFull );
		return false;
	}
	
	return true;


}
void CNetDC_MailChild::_SQLCmdCheckSendMailResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	CheckSendMailTempStruct* TempData = (CheckSendMailTempStruct*)UserObj;

	SL_CheckSendMailResult( TempData->SrvSockID , TempData->PlayerDBID , (char*)TempData->TargetName.c_str() , TempData->TargetDBID , TempData->MapKey , ResultOK , Arg.GetNumber("ResultCode") );

	delete TempData;
}

void CNetDC_MailChild::RL_CheckGetItemRequest( int SrvServerID , int DBID , int MailDBID , int Money , int Money_Account , int Money_Bonus )
{
	_RD_MailDB->SqlCmd( DBID , _SQLCmdCheckGetItemEvent , _SQLCmdCheckGetItemResultEvent , NULL 
		, "SrvSockID"		, EM_ValueType_Int      , SrvServerID
		, "DBID"			, EM_ValueType_Int      , DBID
		, "MailDBID"		, EM_ValueType_Int      , MailDBID
		, "Money"			, EM_ValueType_Int      , Money
		, "Money_Account"	, EM_ValueType_Int      , Money_Account
		, "Money_Bonus"	, EM_ValueType_Int			, Money_Bonus
		, NULL );

}

bool CNetDC_MailChild::_SQLCmdCheckGetItemEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{

	MyDbSqlExecClass		MyDBProc( sqlBase );

	int						DBID			= Arg.GetNumber("DBID");
	int						MailDBID		= Arg.GetNumber("MailDBID");
	int						Money			= Arg.GetNumber("Money");
	int						Money_Account	= Arg.GetNumber("Money_Account");
	int						Money_Bonus		= Arg.GetNumber("Money_Bonus");

	DB_MailStruct			TempMail;
	char					Buf[256];

	if( DBID == -1 )
		return false;


	//---------------------------------------------------------------------------------------------------
	//讀取信件
	sprintf( Buf , "WHERE GUID = %d AND DestroyTime = 0 " , MailDBID );
	//g_CritSect()->Enter();
	MyDBProc.SqlCmd( _RDMailBaseSql->GetSelectStr( Buf ).c_str() );
	//g_CritSect()->Leave();

	MyDBProc.Bind( TempMail , RoleDataEx::ReaderRD_MailBase() );
	if( MyDBProc.Read() == false )
	{
		Arg.PushValue( "ErrCode" , "找不到資料" );
		MyDBProc.Close();

		return false;
	}
	RoleDataEx::ReaderRD_MailBase()->TransferWChartoUTF8( &TempMail , MyDBProc.wcTempBufList() );
	MyDBProc.Close();

	//---------------------------------------------------------------------------------------------------
	//擁有者檢查
	if( TempMail.OwnerDBID != DBID )
	{
		Arg.PushValue( "ErrCode" , "擁有者有問題" );
		return false;
	}
	//---------------------------------------------------------------------------------------------------
	/*
	TempMail.MailBase.Item.Init();
	//取物品	
	if( TempMail.ItemGUID != -1 )
	{
		sprintf( Buf , "WHERE GUID = %d " , TempMail.ItemGUID );
		g_CritSect()->Enter();
		MyDBProc.SqlCmd( _RDMailItemSql->GetSelectStr( Buf ) );
		g_CritSect()->Leave();

		MyDBProc.Bind( TempMail , RoleDataEx::ReaderRD_MailItem() );
		if( MyDBProc.Read() == false )
		{
			Arg.PushValue( "ErrCode" , "物品取出有問題" );
			return false;
		}
		RoleDataEx::ReaderRD_MailItem()->TransferWChartoUTF8( &TempMail );
		MyDBProc.Close();
	}
*/
	//---------------------------------------------------------------------------------------------------
	//金額比較
	if( TempMail.MailBase.IsSendMoney == false && TempMail.MailBase.Money != 0 )
	{
		int	PayMoney =  TempMail.MailBase.Money * -1;

		if( Money != PayMoney )
		{	
			//金錢有問題
			return false;
		}
		Arg.PushValue( "SendBackMoney" , TempMail.MailBase.Money );
	}
	else
	{
		if( Money != TempMail.MailBase.Money )
		{
			return false;
		}
	}

	if( TempMail.MailBase.IsSendMoney == false && TempMail.MailBase.Money_Account != 0 )
	{
		int	PayMoney =  TempMail.MailBase.Money_Account * -1;

		if( Money_Account != PayMoney )
		{	
			//金錢有問題
			return false;
		}
		Arg.PushValue( "SendBackMoney_Account" , TempMail.MailBase.Money_Account );
	}
	else
	{
		if( Money_Account != TempMail.MailBase.Money_Account )
		{
			return false;
		}
	}

	if( TempMail.MailBase.IsSendMoney == false && TempMail.MailBase.Money_Bonus != 0 )
	{
		int	PayMoney =  TempMail.MailBase.Money_Bonus * -1;

		if( Money_Bonus != PayMoney )
		{	
			//金錢有問題
			return false;
		}
		Arg.PushValue( "SendBackMoney_Bonus" , TempMail.MailBase.Money_Bonus );
	}
	else
	{
		if( Money_Bonus != TempMail.MailBase.Money_Bonus )
		{
			return false;
		}
	}

	//---------------------------------------------------------------------------------------------------
	Data.push_back( TempMail );
	//---------------------------------------------------------------------------------------------------
	return true;
}
void CNetDC_MailChild::_SQLCmdCheckGetItemResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	int			SrvSockID = Arg.GetNumber("SrvSockID");
	int			DBID = Arg.GetNumber("DBID");
	int			MailDBID = Arg.GetNumber("MailDBID");
	int			Money = Arg.GetNumber("Money");
	int			Money_Account = Arg.GetNumber("Money_Account");
	int			Money_Bonus = Arg.GetNumber("Money_Bonus");
	int			SendBackMoney = Arg.GetNumber( "SendBackMoney" );
	int			SendBackMoney_Account = Arg.GetNumber( "SendBackMoney_Account" );

	if( ResultOK == false || Data.size() != 1 )
	{
		SL_CheckGetItemResult( SrvSockID , DBID , -1 , MailDBID , Money , Money_Account , Money_Bonus , false );
	}
	else
	{
		SL_CheckGetItemResult( SrvSockID , DBID , Data[0].OrgSendDBID , MailDBID , Money , Money_Account , Money_Bonus , false );
	}
}