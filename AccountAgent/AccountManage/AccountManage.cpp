#pragma once
#include "AccountManage.h"
#include "md5\Mymd5.h"
#include "ReaderClass/DbSqlExecClass.h"
//--------------------------------------------------------------------------------------------------------------
void	AccountManage::PlayerLoginInfoLog( MyDbSqlExecClass& MyDBProc , const char* Account_ID , const char* IP , LoginResultEnum Result, const char* MacAddress )
{
	char Buf[256];
	sprintf( Buf , "INSERT INTO PlayerLoginInfo( Account_ID , IP , LoginResult, MacAddress ) VALUES(CAST( %s AS varchar(64)),'%s',%d, CAST( %s AS varchar(256)) )" 
		, StringToSqlX( Account_ID , 0 , true ).c_str()
		, IP
		, Result
		, StringToSqlX( MacAddress , 0 , true ).c_str() );
	MyDBProc.SqlCmd_WriteOneLine( Buf );
}
//--------------------------------------------------------------------------------------------------------------
AccountManage::AccountManage( IAccountPluginNotify *notify , IniFileClass	*ini ) : IAccountPlugin( notify , ini )
{
	_PlayerLoginInfoClearDay = ini->Int( "PlayerLoginInfoClearDay" );
	if( _PlayerLoginInfoClearDay == 0 )
		_PlayerLoginInfoClearDay = 30;

	_MaxPasswordErrorCount = ini->Int( "MaxPasswordErrorCount" );
	_FreezeAccountTime	= ini->IntDef( "FreezeAccountTime", 3600);

	g_Schedular.Push( _OnTimeClear_PlayerLoginInfo , 1000 , this , NULL );
}
AccountManage::~AccountManage( void )
{
};

struct TempCheckAccountStruct
{
	AccountManage*		This;
	string				Account;
	string				Password;
	string				IPStr;
	int					NetID;
	bool				ResetPassword;
	bool				HashedPassword;

	AccountResultENUM	Result;	
	int					GameID;

	TempCheckAccountStruct()
	{
		This			= NULL;
		Account			= "";
		Password		= "";
		IPStr			= "";
		NetID			= -1;
		ResetPassword	= false;
		HashedPassword	= false;
		Result			= EM_AccountResult_UnKnow;
		GameID			= -1;
	}
};

void AccountManage::CheckAccount( int NetID , string Account , string Password , string IPStr, bool HashedPassword )
{
	TempCheckAccountStruct* UserObj = NEW(TempCheckAccountStruct);
	UserObj->Account		= Account;
	UserObj->Password		= Password;
	UserObj->NetID			= NetID;
	UserObj->This			= this;
	UserObj->IPStr			= IPStr;
	UserObj->HashedPassword = HashedPassword;

	_NormalDB->SqlCmd( _AccountValue( Account ) , _SQLCmdCheckAccountProc , _SQLCmdCheckAccountProcResult , UserObj	, NULL );
}

bool AccountManage::_SQLCmdCheckAccountProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempCheckAccountStruct* UserData = (TempCheckAccountStruct*)UserObj;
	AccountManage*	This = UserData->This;

	if (This == NULL)
	{
		return false;
	}

	//----------------------------------------
	//�qDBŪ�JAccountData
	char Buf[1024];
	sprintf( Buf ,"WHERE Account_ID = CAST( %s AS varchar(64) )", StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );

	MyDbSqlExecClass MyDBProc( sqlBase );
	g_CritSect()->Enter();
	MyDBProc.SqlCmd( This->_RDPlayerAcccountBaseSql->GetSelectStr( Buf ).c_str() ); 
	g_CritSect()->Leave();

	PlayerAccountDBStruct AccountData;
	vector<PlayerAccountDBStruct> DBData;

	MyDBProc.Bind( AccountData , PlayerAccountDBStruct::Reader() );
	while (MyDBProc.Read())
	{
		DBData.push_back(AccountData);
	}
	MyDBProc.Close();
	//----------------------------------------

	//----------------------------------------
	//�䤣��b��
	if (DBData.empty())
	{
		UserData->Result = EM_AccountResult_NotFind;
		return false;
	}
	//----------------------------------------

	//----------------------------------------
	//�ƼƱb��?
	if (DBData.size() != 1)
	{
		UserData->Result = EM_AccountResult_UnKnow;
		return false;
	}
	//----------------------------------------

	AccountData = DBData.front();

	//----------------------------------------
	int Age = int ( ( TimeExchangeIntToFloat( TimeStr::Now_Value( _TimeZone ) ) - AccountData.Age ) / 365.25f );
	if( Age < 0 )
		Age = 0;

	This->_pEventNotify->SetData( UserData->NetID , EM_SetData_Age , Age );

	int DTime = -1;
	if( AccountData.ValidTime != 0 )
	{
		DTime = TimeExchangeFloatToInt( AccountData.ValidTime ) - (int)TimeStr::Now_Value( _TimeZone );
		if( DTime < 0 )
			DTime = -1;
	}
	This->_pEventNotify->SetData( UserData->NetID , EM_SetData_ValidTime , DTime );

	if( AccountData.VivoxLicenseTime != 0 )
		This->_pEventNotify->SetData( UserData->NetID , EM_SetData_VivoxLicenseTime , TimeExchangeFloatToInt( AccountData.VivoxLicenseTime ) );
	//----------------------------------------

	//----------------------------------------
	//�b���O�_�Q�ᵲ
	if( AccountData.FreezeType != EM_AccountFreezeType_None )
	{
		switch( AccountData.FreezeType )
		{
		case EM_AccountFreezeType_Self:
			UserData->Result = EM_AccountResult_Freeze_Self;
			break;
		case EM_AccountFreezeType_Official:
			UserData->Result = EM_AccountResult_Freeze_Official;
			break;
		default:
			UserData->Result = (AccountResultENUM)(EM_AccountResult_Freeze_Self + AccountData.FreezeType - 1);
			break;
		}

		//���]�w�ᵲ�Ѱ��ɶ�
		if (AccountData.FreezeUntil > 0)
		{
			int FreezeRemaining = 0;

			sprintf( Buf ,"SELECT DATEDIFF(second, getdate(), FreezeUntil) FROM PlayerAccount WHERE Account_ID = CAST( %s AS varchar(64) )", StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );

			MyDBProc.SqlCmd(Buf);
			MyDBProc.Bind(1, SQL_C_LONG, SQL_C_DEFAULT, (LPBYTE)&FreezeRemaining);
			MyDBProc.Read();
			MyDBProc.Close();

			//�w�g�W�L�ᵲ�Ѱ��ɶ�
			if (FreezeRemaining <= 0)
			{
				Print( LV2 , "Login" , "account unfreeze : %s" , UserData->Account.c_str());

				//�۰ʸѰ��b���ᵲ
				sprintf( Buf ,"UPDATE PlayerAccount SET FreezeType = 0, FreezeUntil = 0 WHERE Account_ID = CAST( %s AS varchar(64) )", StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );
				MyDBProc.SqlCmd_WriteOneLine( Buf );
				AccountData.FreezeType = EM_AccountFreezeType_None;

				//------------------------------------------------------------
				//���]�w�ˬd�K�X��J���~����
				if (This->_MaxPasswordErrorCount > 0)
				{
					//�b���ѭ�, ���m�K�X��J���~����
					sprintf( Buf ,"UPDATE PasswordCheckRecord SET LastCheckTime = %d, CheckFailedTimes = 0 WHERE Account_ID = CAST( %s AS varchar(64) )",
						TimeStr::Now_Value(_TimeZone), StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str());
					MyDBProc.SqlCmd_WriteOneLine( Buf );
				}
				//------------------------------------------------------------
			}			
			else
			{
				//�]�w�Ѿl���ᵲ�ɶ�, �^�ǵ�Client
				This->_pEventNotify->SetData( UserData->NetID , EM_SetData_FreezeRemainingTime , FreezeRemaining );
			}
		}

		if (AccountData.FreezeType != EM_AccountFreezeType_None)
		{
			return false;
		}	
	}
	//----------------------------------------

	//----------------------------------------
	//�K�X�ഫ
	MyMD5Class myMd5Class;
	bool WrongPassword = false;

	//�ǹL�Ӫ��K�X�w�g�g�LMD5�B�z
	if (UserData->HashedPassword)
	{		
		if (!AccountData.IsMd5Password)
		{
			//DB�����K�X�O���X, �NDB�����K�X�নMD5���
			myMd5Class.ComputeStringHash( AccountData.Password , NULL );
			WrongPassword = ( _stricmp( UserData->Password.c_str(), myMd5Class.GetMd5Str() ) !=0 );
		}
		else
		{
			WrongPassword = ( _stricmp( UserData->Password.c_str(), AccountData.Password ) !=0 );
		}
	}
	else //�ǹL�Ӫ��K�X�O���X
	{		
		if (!AccountData.IsMd5Password)
		{
			WrongPassword = (strcmp( UserData->Password.c_str(), AccountData.Password ) !=0);
		}
		else
		{
			//�N�ǹL�Ӫ��K�X�নMD5���
			myMd5Class.ComputeStringHash( UserData->Password.c_str() , NULL );
			WrongPassword = (_stricmp( myMd5Class.GetMd5Str(), AccountData.Password ) !=0);
		}
	}

	//�K�X����
	if(WrongPassword)
	{
		UserData->Result = EM_AccountResult_PasswordErr;

		PlayerLoginInfoLog( MyDBProc , UserData->Account.c_str()  , UserData->IPStr.c_str() , EM_LoginResul_PasswordError, "" );

		//------------------------------------------------------------
		//���]�w�ˬd�K�X��J���~����
		if (This->_MaxPasswordErrorCount > 0)
		{
			//---------------------------------
			//�q��Ʈw���o�K�X���~���Ƭ���
			sprintf( Buf ,"SELECT Account_ID, LastCheckTime, CheckFailedTimes FROM PasswordCheckRecord WHERE Account_ID = CAST( %s AS varchar(64) )", StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );

			MyDBProc.SqlCmd( Buf ); 

			PasswordCheckRecordStruct CheckRecord;
			MyDBProc.Bind( 1 , SQL_C_CHAR , sizeof( CheckRecord.Account_ID )	, (LPBYTE)CheckRecord.Account_ID );
			MyDBProc.Bind( 2 , SQL_C_LONG , SQL_DEFAULT							, (LPBYTE)&CheckRecord.LastCheckTime );
			MyDBProc.Bind( 3 , SQL_C_LONG , SQL_DEFAULT							, (LPBYTE)&CheckRecord.CheckFailedTimes );

			if (MyDBProc.Read())
			{
				MyDBProc.Close();
			}
			//��Ʈw�̨S���ӱb�������, �s�W�@����ƨ�PasswordCheckRecord table
			else
			{				
				sprintf( Buf ,"INSERT INTO PasswordCheckRecord (Account_ID) VALUES( CAST( %s AS varchar(64) ))", StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str());
				MyDBProc.SqlCmd_WriteOneLine( Buf );
			}
			//---------------------------------

			unsigned int NowTime	= TimeStr::Now_Value(_TimeZone);
			unsigned int TimeOffset = NowTime - CheckRecord.LastCheckTime;

			//�W����J���~�w�W�L1�p��, �����k0
			if (TimeOffset > 60 *60)
			{
				CheckRecord.CheckFailedTimes = 0;
			}

			//���~����+1
			CheckRecord.CheckFailedTimes++;
			CheckRecord.LastCheckTime = NowTime;

			//�bClientInfo�]�w���~����, �Ǧ^Client��
			This->_pEventNotify->SetData( UserData->NetID , EM_SetData_PasswordErrorCount , CheckRecord.CheckFailedTimes );

			//��s��Ʈw
			sprintf( Buf ,"UPDATE PasswordCheckRecord SET LastCheckTime = %d, CheckFailedTimes = %d WHERE Account_ID = CAST( %s AS varchar(64) )",
				CheckRecord.LastCheckTime, CheckRecord.CheckFailedTimes, StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str());
			MyDBProc.SqlCmd_WriteOneLine( Buf );

			//���~���ƶW�L���\���W������
			if (CheckRecord.CheckFailedTimes >= This->_MaxPasswordErrorCount)
			{
				//�b���Ȯɭᵲ�@�w�ɶ�
				UserData->Result = EM_AccountResult_Freeze_Official;

				//�bClientInfo�]�w�ᵲ�Ѿl�ɶ�, �Ǧ^Client��
				This->_pEventNotify->SetData( UserData->NetID , EM_SetData_FreezeRemainingTime , This->_FreezeAccountTime );

				//��s�ᵲ���A��ᵲ�ɶ���Accout��ƪ�
				sprintf( Buf ,"UPDATE PlayerAccount SET FreezeType = %d, FreezeUntil = DATEADD( second , %d, getdate() ) WHERE Account_ID = CAST( %s AS varchar(64) )",
					EM_AccountFreezeType_Official, This->_FreezeAccountTime, StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str());
				MyDBProc.SqlCmd_WriteOneLine( Buf );

				//�glog
				PlayerLoginInfoLog( MyDBProc , UserData->Account.c_str()  , UserData->IPStr.c_str() , EM_LoginResul_PasswordErrorFreeze, "" );


			}
			//------------------------------------------------------------
		}

		return false;
	}
	//----------------------------------------

	//----------------------------------------
	//�ɶ��ˬd
	if(		AccountData.ValidTime != 0
		&&	(unsigned)TimeExchangeFloatToInt( AccountData.ValidTime ) < TimeStr::Now_Value( _TimeZone ) )
	{
		UserData->Result = EM_AccountResult_ValidTimeErr;
		return false;
	}
	//----------------------------------------

	//----------------------------------------
	if( AccountData.GameID != -1 )
	{
		UserData->GameID = AccountData.GameID;
		UserData->Result = EM_AccountResult_Relogin;
		return false;
	}
	//----------------------------------------

	//----------------------------------------
	if( AccountData.ResetPassword == true )
	{
		UserData->ResetPassword = true;		
		return false;
	}
	//----------------------------------------

	UserData->GameID = AccountData.LastLoginGameID;
	UserData->Result = EM_AccountResult_Success;

	//------------------------------------------------------------
	//���]�w�ˬd�K�X��J���~����
	if (This->_MaxPasswordErrorCount > 0)
	{
		
		sprintf( Buf ,"UPDATE PasswordCheckRecord SET LastCheckTime = %d, CheckFailedTimes = 0 WHERE Account_ID = CAST( %s AS varchar(64) )",
			TimeStr::Now_Value(_TimeZone), StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str());
		MyDBProc.SqlCmd_WriteOneLine( Buf );
	}
	//------------------------------------------------------------

	return true;
}

void AccountManage::_SQLCmdCheckAccountProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempCheckAccountStruct* UserData = (TempCheckAccountStruct*)UserObj;

	if (UserData->This != NULL)
	{
		if (UserData->ResetPassword == true)
		{
			UserData->This->_pEventNotify->ResetPassword( UserData->NetID );
		}
		else
		{
			UserData->This->_pEventNotify->CheckAccountResult( UserData->NetID , UserData->GameID , UserData->Account , UserData->Result);
		}
	}
	
	delete UserData;
}

struct LoginTempStruct
{
	string			Account;
	string			Password;
	string			IPStr;
	string			MacAddress;
	int				GameID;
	void*			UserObj;
	AccountManage*	This;
	unsigned		EnterTime;
	bool			HashedPassword;

	//////////////////////////////////////////////////////////////////////////
	AccountResultENUM  Result;	//�^�ǵ��G
	char	LastIP[32];
	char    LastTime[32];

	LoginTempStruct()
	{
		LastIP[0]	   = 0;
		LastTime[0]	   = 0;  
		HashedPassword = false;
	};
};

void AccountManage::Login( void* UserObj , string Account , string Password , string IPStr , int GameID, string MacAddress, bool HashedPassword )
{
	Print( LV2 , "Login" , " Account=%s IPStr =%s GameID=%d" , Account.c_str() , IPStr.c_str() , GameID );
	
	LoginTempStruct* Temp = NEW(LoginTempStruct);
	
	Temp->Account		= Account;
	Temp->Password		= Password;
	Temp->MacAddress	= MacAddress;
	Temp->IPStr			= IPStr;
	Temp->UserObj		= UserObj;
	Temp->GameID		= GameID;
	Temp->This			= this;
	Temp->EnterTime		= GetTickCount();
	Temp->HashedPassword= HashedPassword;

	_NormalDB->SqlCmd( _AccountValue( Account ) , _SQLCmdLoginProc , _SQLCmdLoginProcResult , Temp	, NULL );
}

bool AccountManage::_SQLCmdLoginProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	LoginTempStruct* UserData = (LoginTempStruct*)UserObj;
	AccountManage* This = UserData->This;

	char Buf[512];
	sprintf( Buf ,"WHERE Account_ID = CAST( %s AS varchar(64) )", StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );

	MyDbSqlExecClass MyDBProc( sqlBase );
	g_CritSect()->Enter();
	MyDBProc.SqlCmd( This->_RDPlayerAcccountBaseSql->GetSelectStr( Buf ).c_str() ); 
	g_CritSect()->Leave();

	PlayerAccountDBStruct AccountData;

	MyDBProc.Bind( AccountData , PlayerAccountDBStruct::Reader() );
	if( MyDBProc.Read() == false )
	{
		UserData->Result = EM_AccountResult_NotFind;
		return false;
	}
	MyDBProc.Close();

	//-------------------------------------------------------
	//�ɶ��ˬd
	if(		AccountData.ValidTime != 0
		&&	(unsigned)TimeExchangeFloatToInt( AccountData.ValidTime ) < TimeStr::Now_Value( _TimeZone ) )
	{
		UserData->Result = EM_AccountResult_ValidTimeErr;
		return false;
	}
	//-------------------------------------------------------

	//-------------------------------------------------------
	//Mac address�ˬd
	{
		bool	bFound			= false;
		int		MacBanMethod	= 0;

		sprintf( Buf , "SELECT BanMethod FROM MacLimitTable WHERE MacAddress= CAST( %s AS varchar(256) )" , StringToSqlX( UserData->MacAddress.c_str() , 0 , true ).c_str() );
		MyDBProc.Bind( 1 , SQL_C_LONG , sizeof( MacBanMethod )	, (LPBYTE)&MacBanMethod );
		MyDBProc.SqlCmd( Buf );		

		if( MyDBProc.Read() )
		{
			bFound = true;

			if( MacBanMethod == 0 )	// 0 = refuse login , 1 = ban this account
			{
				UserData->Result = EM_AccountResult_IllegalMacAddress;

				PlayerLoginInfoLog( MyDBProc , UserData->Account.c_str()  , UserData->IPStr.c_str() , EM_LoginResul_MacKick, UserData->MacAddress.c_str() );
			}
			else if( MacBanMethod == 1 ) // ban this account
			{
				UserData->Result = EM_AccountResult_IllegalMacAddress;

				// �]�������\�� mac �s��, �]�s�a�� ban ���ϥθ� mac �� account
				sprintf( Buf , "UPDATE PlayerAccount  SET  AccountState = 5 WHERE Account_ID = CAST( %s AS varchar(64) )", StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );
				MyDBProc.SqlCmd_WriteOneLine( Buf );


				PlayerLoginInfoLog( MyDBProc , UserData->Account.c_str()  , UserData->IPStr.c_str() , EM_LoginResul_MacBanAccount, UserData->MacAddress.c_str() );
			}
		}
		MyDBProc.Close();

		if( bFound == true )
			return false;
	}
	//-------------------------------------------------------

	//-------------------------------------------------------
	//�b�����A�ˬd
	if( AccountData.AccountState == EM_AccountState_Banned )
	{
		UserData->Result = EM_AccountResult_Banned;
		return false;
	}
	//-------------------------------------------------------

	//-------------------------------------------------------
	//�b���ᵲ���A�ˬd
	if( AccountData.FreezeType != EM_AccountFreezeType_None )
	{
		switch( AccountData.FreezeType )
		{
		case EM_AccountFreezeType_Self:
			UserData->Result = EM_AccountResult_Freeze_Self;
			break;
		case EM_AccountFreezeType_Official:
			UserData->Result = EM_AccountResult_Freeze_Official;
			break;
		default:
			UserData->Result = ( AccountResultENUM )( EM_AccountFreezeType_Self + AccountData.FreezeType - 1  );
			break;
		}

		//���]�w�ᵲ�Ѱ��ɶ�
		if (AccountData.FreezeUntil > 0)
		{
			int FreezeRemaining = 0;

			sprintf( Buf ,"SELECT DATEDIFF(second, getdate(), FreezeUntil) FROM PlayerAccount WHERE Account_ID = CAST( %s AS varchar(64) )", StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );

			MyDBProc.SqlCmd(Buf);
			MyDBProc.Bind(1, SQL_C_LONG, SQL_C_DEFAULT, (LPBYTE)&FreezeRemaining);
			MyDBProc.Read();
			MyDBProc.Close();

			//�w�g�W�L�ᵲ�Ѱ��ɶ�
			if (FreezeRemaining <= 0)
			{
				Print( LV2 , "Login" , "account unfreeze : %s" , UserData->Account.c_str());

				//�۰ʸѰ��b���ᵲ
				sprintf( Buf ,"UPDATE PlayerAccount SET FreezeType = 0, FreezeUntil = 0 WHERE Account_ID = CAST( %s AS varchar(64) )", StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );
				MyDBProc.SqlCmd_WriteOneLine( Buf );
				AccountData.FreezeType = EM_AccountFreezeType_None;

				//------------------------------------------------------------
				//���]�w�ˬd�K�X��J���~����
				if (This->_MaxPasswordErrorCount > 0)
				{
					//�b���ѭ�, ���m�K�X��J���~����
					sprintf( Buf ,"UPDATE PasswordCheckRecord SET LastCheckTime = %d, CheckFailedTimes = 0 WHERE Account_ID = CAST( %s AS varchar(64) )",
						TimeStr::Now_Value(_TimeZone), StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str());
					MyDBProc.SqlCmd_WriteOneLine( Buf );
				}
				//------------------------------------------------------------
			}
			else
			{
				//�]�w�Ѿl���ᵲ�ɶ�, �^�ǵ�Client
				This->_pEventNotify->SetData( UserData->Account.c_str() , EM_SetData_FreezeRemainingTime , FreezeRemaining );
			}
		}

		if (AccountData.FreezeType != EM_AccountFreezeType_None)
		{
			return false;
		}	
	}
	//-------------------------------------------------------

	//----------------------------------------
	//�K�X�ഫ
	MyMD5Class myMd5Class;
	bool WrongPassword = false;

	//�ǹL�Ӫ��K�X�w�g�g�LMD5�B�z
	if (UserData->HashedPassword)
	{	
		if (!AccountData.IsMd5Password)
		{
			//DB�����K�X�O���X, �NDB�����K�X�নMD5���
			myMd5Class.ComputeStringHash( AccountData.Password , NULL );
			WrongPassword = (_stricmp( UserData->Password.c_str(), myMd5Class.GetMd5Str() ) !=0 );
		}
		else
		{
			WrongPassword = (_stricmp( UserData->Password.c_str(), AccountData.Password ) !=0 );
		}
	}
	else //�ǹL�Ӫ��K�X�O���X
	{		
		if (!AccountData.IsMd5Password)
		{
			WrongPassword = (strcmp( UserData->Password.c_str(), AccountData.Password ) !=0);
		}
		else
		{
			//�N�ǹL�Ӫ��K�X�নMD5���
			myMd5Class.ComputeStringHash( UserData->Password.c_str() , NULL );
			WrongPassword = (_stricmp( myMd5Class.GetMd5Str(), AccountData.Password ) !=0);
		}
	}

	//�K�X����
	if(WrongPassword)
	{
		UserData->Result = EM_AccountResult_PasswordErr;

		//------------------------------------------------------------
		//���]�w�ˬd�K�X��J���~����
		if (This->_MaxPasswordErrorCount > 0)
		{
			//---------------------------------
			//�q��Ʈw���o�K�X���~���Ƭ���
			sprintf( Buf ,"SELECT Account_ID, LastCheckTime, CheckFailedTimes FROM PasswordCheckRecord WHERE Account_ID = CAST( %s AS varchar(64) )", StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );

			MyDBProc.SqlCmd( Buf ); 

			PasswordCheckRecordStruct CheckRecord;
			MyDBProc.Bind( 1 , SQL_C_CHAR , sizeof( CheckRecord.Account_ID )	, (LPBYTE)CheckRecord.Account_ID );
			MyDBProc.Bind( 2 , SQL_C_LONG , SQL_DEFAULT							, (LPBYTE)&CheckRecord.LastCheckTime );
			MyDBProc.Bind( 3 , SQL_C_LONG , SQL_DEFAULT							, (LPBYTE)&CheckRecord.CheckFailedTimes );

			if (MyDBProc.Read())
			{
				MyDBProc.Close();
			}
			//��Ʈw�̨S���ӱb�������, �s�W�@����ƨ�PasswordCheckRecord table
			else
			{				
				sprintf( Buf ,"INSERT INTO PasswordCheckRecord (Account_ID) VALUES( CAST( %s AS varchar(64) ))", StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str());
				MyDBProc.SqlCmd_WriteOneLine( Buf );
			}
			//---------------------------------

			unsigned int NowTime	= TimeStr::Now_Value(_TimeZone);
			unsigned int TimeOffset = NowTime - CheckRecord.LastCheckTime;

			//�W����J���~�w�W�L1�p��, �����k0
			if (TimeOffset > 60 *60)
			{
				CheckRecord.CheckFailedTimes = 0;
			}

			//���~����+1
			CheckRecord.CheckFailedTimes++;
			CheckRecord.LastCheckTime = NowTime;

			//�bClientInfo�]�w���~����, �Ǧ^Client��
			This->_pEventNotify->SetData( UserData->Account.c_str() , EM_SetData_PasswordErrorCount , CheckRecord.CheckFailedTimes );

			//��s��Ʈw
			sprintf( Buf ,"UPDATE PasswordCheckRecord SET LastCheckTime = %d, CheckFailedTimes = %d WHERE Account_ID = CAST( %s AS varchar(64) )",
				CheckRecord.LastCheckTime, CheckRecord.CheckFailedTimes, StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str());
			MyDBProc.SqlCmd_WriteOneLine( Buf );

			//���~���ƶW�L���\���W������
			if (CheckRecord.CheckFailedTimes >= This->_MaxPasswordErrorCount)
			{
				//�b���Ȯɭᵲ�@�w�ɶ�
				UserData->Result = EM_AccountResult_Freeze_Official;

				//�bClientInfo�]�w�ᵲ�Ѿl�ɶ�, �Ǧ^Client��
				This->_pEventNotify->SetData( UserData->Account.c_str() , EM_SetData_FreezeRemainingTime , This->_FreezeAccountTime );

				//��s�ᵲ���A��ᵲ�ɶ���Accout��ƪ�
				sprintf( Buf ,"UPDATE PlayerAccount SET FreezeType = %d, FreezeUntil = DATEADD( second , %d, getdate() ) WHERE Account_ID = CAST( %s AS varchar(64) )",
					EM_AccountFreezeType_Official, This->_FreezeAccountTime, StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str());
				MyDBProc.SqlCmd_WriteOneLine( Buf );

				//�glog
				PlayerLoginInfoLog( MyDBProc , UserData->Account.c_str()  , UserData->IPStr.c_str() , EM_LoginResul_PasswordErrorFreeze, UserData->MacAddress.c_str() );
			}
			//------------------------------------------------------------
		}

		return false;
	}
	//-------------------------------------------------------

	//-------------------------------------------------------
	//�b���n�J
	{
		//�K�X�۰�MD5�ഫ
		if( AccountData.IsMd5Password == false && AccountData.IsAutoConvertMd5 == true )
		{
			//�ǹL�Ӫ��K�X�w�g�g�LMD5�B�z
			if (UserData->HashedPassword)
			{
				sprintf( Buf , "UPDATE PlayerAccount  SET  IsMd5Password = 1 , Password='%s' WHERE Account_ID = CAST( %s AS varchar(64) )" , UserData->Password.c_str() , StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );
				MyDBProc.SqlCmd_WriteOneLine( Buf );
			}
			else
			{
				//�N�K�X�ഫ��MD5
				myMd5Class.ComputeStringHash( UserData->Password.c_str() , NULL );
				sprintf( Buf , "UPDATE PlayerAccount  SET  IsMd5Password = 1 , Password='%s' WHERE Account_ID = CAST( %s AS varchar(64) )" , myMd5Class.GetMd5Str() , StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );
				MyDBProc.SqlCmd_WriteOneLine( Buf );
			}
		}

		sprintf( Buf , "SELECT Top 1 Time ,IP FROM PlayerLoginInfo WHERE account_id=CAST( %s AS varchar(64) ) order by index_no desc " ,  StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str());

		MyDBProc.Bind( 1 , SQL_C_CHAR , sizeof( UserData->LastTime )	, (LPBYTE)UserData->LastTime );
		MyDBProc.Bind( 2 , SQL_C_CHAR , sizeof( UserData->LastIP )		, (LPBYTE)UserData->LastIP	);
		MyDBProc.SqlCmd( Buf );		
		MyDBProc.Read();
		MyDBProc.Close();
		UserData->Result = EM_AccountResult_Success;

		PlayerLoginInfoLog( MyDBProc , UserData->Account.c_str()  , UserData->IPStr.c_str() , EM_LoginResul_OK, UserData->MacAddress.c_str() );


		sprintf( Buf , "UPDATE PlayerAccount SET GameID=%d , LastLoginGameID=%d WHERE Account_ID = CAST( %s AS varchar(64) )" , UserData->GameID , UserData->GameID , StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );
		MyDBProc.SqlCmd_WriteOneLine( Buf );

		sprintf( Buf , "UPDATE PlayerAccount SET FirstLoginTime= getdate() WHERE Account_ID = CAST( %s AS varchar(64) ) and FirstLoginTime=0" , StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );
		MyDBProc.SqlCmd_WriteOneLine( Buf );

		sprintf( Buf , "UPDATE PlayerAccount SET LastLoginTime= getdate() WHERE Account_ID = CAST( %s AS varchar(64) )" , StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );
		MyDBProc.SqlCmd_WriteOneLine( Buf );

		//------------------------------------------------------------
		//���]�w�ˬd�K�X��J���~����
		if (This->_MaxPasswordErrorCount > 0)
		{
			
			sprintf( Buf ,"UPDATE PasswordCheckRecord SET LastCheckTime = %d, CheckFailedTimes = 0 WHERE Account_ID = CAST( %s AS varchar(64) )",
				TimeStr::Now_Value(_TimeZone), StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str());
			MyDBProc.SqlCmd_WriteOneLine( Buf );
		}
		//------------------------------------------------------------
	}

	return true;
	//-------------------------------------------------------
}

void AccountManage::_SQLCmdLoginProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	LoginTempStruct* UserData = (LoginTempStruct*)UserObj;

	Print( LV2 , "_SQLCmdLoginProcResult" , "Login Result Account=%s IPStr=%s GameID=%d Result=%d ProcTime=%d" , UserData->Account.c_str() , UserData->IPStr.c_str() , UserData->GameID , UserData->Result , GetTickCount() - UserData->EnterTime );
	UserData->This->_pEventNotify->AccountLoginResult( UserData->UserObj , UserData->Account , UserData->LastIP , UserData->LastTime , UserData->Result );

	delete UserData;
}


struct LogoutTempStruct
{
	AccountManage*  This;

	string			Account;
	string			IPStr;
	string			MacAddress;
	int				LoginTime;
	unsigned		EnterTime;

	LogoutTempStruct()
	{
		This		= NULL;
		Account		= "";
		IPStr		= "";
		MacAddress	= "";
		LoginTime	= 0;
		EnterTime	= 0;
	}
};
void AccountManage::Logout(  string Account , string IPStr , int LoginTime, string MacAddress )
{
	Print( LV2 , "Logout" , "Account=%s IPStr=%s" , Account.c_str() , IPStr.c_str() );
	LogoutTempStruct* TempData = NEW( LogoutTempStruct );
	
	TempData->Account		= Account;
	TempData->IPStr			= IPStr;
	TempData->MacAddress	= MacAddress;
	TempData->This			= this;
	TempData->LoginTime		= LoginTime;
	TempData->EnterTime		= GetTickCount();

	_NormalDB->SqlCmd( _AccountValue( Account ) , _SQLCmdLogoutProc , _SQLCmdLogoutProcResult , TempData	, NULL );
	
}

bool AccountManage::_SQLCmdLogoutProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	LogoutTempStruct* UserData = (LogoutTempStruct*)UserObj;
	char Buf[512];
	MyDbSqlExecClass MyDBProc( sqlBase );

	float ValidTime = 0;
	//Ū�� ValidTim �ݬO�_�n��s�ɶ�
	sprintf( Buf , "SELECT Cast( ValidTime as Float ) FROM PlayerAccount WHERE Account_ID = CAST( %s AS varchar(64) ) " ,  StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1 , SQL_C_FLOAT , SQL_C_DEFAULT	, (LPBYTE)&ValidTime );
	MyDBProc.Read();
	MyDBProc.Close();

	if( ValidTime == 0 )
	{
		sprintf( Buf , "UPDATE PlayerAccount  SET  GameID = -1 , LastLogoutTime = %d , PlayTime = PlayTime + %d WHERE Account_ID = CAST( %s AS varchar(64) )" 
			, TimeStr::Now_Value() , ( (TimeStr::Now_Value( _TimeZone ) - UserData->LoginTime)/60 ) , StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );
	}
	else
	{
		sprintf( Buf , "UPDATE PlayerAccount  SET  GameID = -1 , LastLogoutTime = %d , PlayTime = PlayTime + %d , ValidTime = DateAdd( mi , 5 , GetDate() ) WHERE Account_ID = CAST( %s AS varchar(64) ) and ValidTime < DateAdd( hh , 1 , GetDate() )" 
			, TimeStr::Now_Value() , ( (TimeStr::Now_Value( _TimeZone ) - UserData->LoginTime)/60 ) , StringToSqlX( UserData->Account.c_str() , 0 , true ).c_str() );
	}
	MyDBProc.SqlCmd_WriteOneLine( Buf );

	PlayerLoginInfoLog( MyDBProc , UserData->Account.c_str()  , UserData->IPStr.c_str() , EM_LoginResul_Logout, UserData->MacAddress.c_str() );

	return true;
}
void AccountManage::_SQLCmdLogoutProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	LogoutTempStruct* UserData = (LogoutTempStruct*)UserObj;
	Print( LV2 , "_SQLCmdLogoutProcResult" , "Account=%s IPStr=%s ProcTime=%d" , UserData->Account.c_str() , UserData->IPStr.c_str() , GetTickCount() - UserData->EnterTime );

	UserData->This->_pEventNotify->AccountLogoutResult( UserData->Account , EM_AccountResult_Success );

	delete UserData;
}


void  AccountManage::_DBReadCheckAccountDBEventCB ( vector<PlayerAccountDBStruct>& DBData , void* UserObj , char *WhereCmd , bool ResultOK )
{	
	TempCheckAccountStruct* CheckInfo = (TempCheckAccountStruct*)UserObj;

	string			Account			= CheckInfo->Account;
	string			Password		= CheckInfo->Password;
	string			IPStr			= CheckInfo->IPStr;
	int				NetID			= CheckInfo->NetID;
	AccountManage*	This			= CheckInfo->This;

	bool			bResetPassword	= CheckInfo->ResetPassword;

	delete CheckInfo;
	
	//�䤣��b��
	if( DBData.size() == 0 )
	{
		This->_pEventNotify->CheckAccountResult( NetID , -1 , Account , EM_AccountResult_NotFind  );					   
		return;
	}

	if( DBData.size() != 1 )
	{
		This->_pEventNotify->CheckAccountResult( NetID , -1 , Account , EM_AccountResult_UnKnow  );
		return;
	}


	int Age = int ( ( TimeExchangeIntToFloat( TimeStr::Now_Value( _TimeZone ) ) - DBData.front().Age ) / 365.25f );
	if( Age < 0 )
		Age = 0;

	This->_pEventNotify->SetData( NetID , 0 , Age );

	int DTime = -1;
	if( DBData.front().ValidTime != 0 )
	{
		DTime = TimeExchangeFloatToInt( DBData.front().ValidTime ) - (int)TimeStr::Now_Value( _TimeZone );
		if( DTime < 0 )
			DTime = -1;
	}
	This->_pEventNotify->SetData( NetID , 1 , DTime );

	if( DBData.front().VivoxLicenseTime != 0 )
		This->_pEventNotify->SetData( NetID , 2 , TimeExchangeFloatToInt( DBData.front().VivoxLicenseTime ) );

	if( DBData.front().FreezeType != EM_AccountFreezeType_None )
	{
		switch( DBData.front().FreezeType )
		{
		case EM_AccountFreezeType_Self:
			This->_pEventNotify->CheckAccountResult( NetID , -1 , Account , EM_AccountResult_Freeze_Self  );
			break;
		case EM_AccountFreezeType_Official:
			This->_pEventNotify->CheckAccountResult( NetID , -1 , Account , EM_AccountResult_Freeze_Official  );
			break;
		default:
			This->_pEventNotify->CheckAccountResult( NetID , -1 , Account , (AccountResultENUM)(EM_AccountResult_Freeze_Self+DBData.front().FreezeType-1)  );
			break;
		}
		return;
	}

	MyMD5Class myMd5Class;
	//�K�X�ഫ
	myMd5Class.ComputeStringHash( Password.c_str() , NULL );

	//�K�X����
	if(		( DBData.front().IsMd5Password == true && strcmp( myMd5Class.GetMd5Str() , DBData.front().Password ) !=0 )
		||	( DBData.front().IsMd5Password == false && strcmp( Password.c_str() , DBData.front().Password ) !=0 ) )

	{		
		This->_pEventNotify->CheckAccountResult( NetID , -1 , Account , EM_AccountResult_PasswordErr  );

		char Buf[512];
		sprintf( Buf , "Insert PlayerLoginInfo( Account_ID , IP , LoginResult, MacAddress ) VALUES(CAST( %s AS varchar(64) ) ,'%s',%d, '%s' )" 
			, StringToSqlX( Account.c_str() , 0 , true ).c_str() , IPStr.c_str() , EM_LoginResul_PasswordError, "" );
		This->_NormalDB->SQLCMDWrite(  rand() , NULL , NULL , Buf );
		return;
	}

	//�ɶ��ˬd
	if(		DBData[0].ValidTime != 0
		&&	(unsigned)TimeExchangeFloatToInt( DBData[0].ValidTime ) < TimeStr::Now_Value( _TimeZone ) )
	{
		This->_pEventNotify->CheckAccountResult( NetID , -1 , Account , EM_AccountResult_ValidTimeErr  );
		return;
	}


	if( DBData[0].GameID != -1 )
	{
		This->_pEventNotify->CheckAccountResult( NetID , DBData[0].GameID , Account , EM_AccountResult_Relogin  );
		return;
	}

	if( DBData[0].ResetPassword == true )
	{
		This->_pEventNotify->ResetPassword( NetID );
		return;
	}

	This->_pEventNotify->CheckAccountResult( NetID , DBData[0].LastLoginGameID , Account , EM_AccountResult_Success  );
}
//----------------------------------------------------------------------------------------------
struct PasswordExchangeStruct
{
	char	Account[32];
	char	Password[33];
	char	Md5Password[33];
};
//��s�K�X�B�z
bool AccountManage::_SQLCmdMakeMd5PasswordEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	vector< PasswordExchangeStruct > RoleList;
	char Buf[512];

	MyDbSqlExecClass	MyDBProc( sqlBase );
	//��Ҧ��ݭn�ק諸�K�X��X

	PasswordExchangeStruct Temp;
	MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );		

	sprintf( Buf , "SELECT Account_ID, Password FROM PlayerAccount WHERE (IsMd5Password = 0) AND (IsAutoConvertMd5 = 1) " );

	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind( 1, SQL_C_CHAR, sizeof(Temp.Account) , (LPBYTE)&Temp.Account );
	MyDBProc.Bind( 2, SQL_C_CHAR, sizeof(Temp.Password) , (LPBYTE)&Temp.Password );

	MyMD5Class md5Class;

	while( MyDBProc.Read() )
	{
		//sprintf( Buf , "Rune_%s_Waker" , Temp.Password );
		md5Class.ComputeStringHash( Temp.Password );
		strcpy( Temp.Md5Password , md5Class.GetMd5Str() );
		RoleList.push_back( Temp );
	}

	MyDBProc.Close();

	for( unsigned i = 0 ; i < RoleList.size() ; i++ )
	{
		sprintf( Buf , "UPDATE PlayerAccount  SET  IsMd5Password = 1 , Password= CAST( %s AS varchar(33) ) WHERE Account_ID = CAST( %s AS varchar(64) )" 
			, StringToSqlX( RoleList[i].Md5Password , 0 , true ).c_str()
			, StringToSqlX( RoleList[i].Account , 0 , true ).c_str() );
			//, RoleList[i].Md5Password , RoleList[i].Account );
		MyDBProc.SqlCmd_WriteOneLine( Buf );
		//This->_LoginInfoDB->SQLCMDWrite( rand() , NULL , NULL , Buf );
	}


	MyDBProc.SqlCmd_WriteOneLine( "COMMIT TRANSACTION" );		
	return true;

}
void AccountManage::_SQLCmdMakeMd5PasswordResultEvent ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	AccountManage* This = (AccountManage*)UserObj;
	This->_IsMakePasswordOK = true;
}

void AccountManage::AccountServerInit()
{
	_NormalDB->SqlCmd( 0 ,  _SQLCmdMakeMd5PasswordEvent , _SQLCmdMakeMd5PasswordResultEvent , this , NULL );
	printf( "\n");

	_IsMakePasswordOK = false;
	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 200 );
		_NormalDB->Process( );
		switch( i % 4 )
		{
		case 0:
			printf( "\rPassword process /        " );
			break;
		case 1:
			printf( "\rPassword process|        " );
			break;
		case 2:			
			printf( "\rPassword process\\        " );
			break;
		case 3:
			printf( "\rPassword process-        " );
			break;
		}

		if( _IsMakePasswordOK == true )
			break;
	}
	printf( "\n");
}

void AccountManage::LogoutAllAccount()
{
	_NormalDB->SQLCMDWrite( rand() , NULL , NULL , "UPDATE PlayerAccount  SET  GameID = -1 Where ( GameID <> -1 )" );
}

void AccountManage::AccountFreeze( string UserAccount , int FreezeType )
{
	char	Buf[512];
	sprintf( Buf , "UPDATE PlayerAccount  SET  FreezeType = %d Where Account_ID=CAST( %s AS varchar(64) )" , FreezeType , StringToSqlX( UserAccount.c_str() , 0 , true ).c_str() );
	_NormalDB->SQLCMDWrite( rand() , NULL , NULL , Buf );
}

int  AccountManage::_OnTimeClear_PlayerLoginInfo( SchedularInfo* Obj , void* InputClass )
{
	AccountManage* This = (AccountManage*)InputClass;

	char	Buf[512];
	sprintf( Buf , "Delete PlayerLoginInfo Where Time < DateAdd(d,%d,getdate() )" , This->_PlayerLoginInfoClearDay*-1 );
	This->_NormalDB->SQLCMDWrite( 1 , NULL , NULL , Buf );
	g_Schedular.Push( _OnTimeClear_PlayerLoginInfo , 60*60*24*1000 , InputClass , NULL );
	return 0;
}