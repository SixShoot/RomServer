#include "NetDC_LotteryChild.h"
// 1�� 30% 2�� 30% 3��30%



vector< LotteryInfoStruct > 	CNetDC_LotteryChild::_LotteryHistory;
LotteryInfoStruct				CNetDC_LotteryChild::_Last5LotteryInfo[5];
int							  	CNetDC_LotteryChild::_LotteryVersion;	//�̷s�@�� �|���}�� 
int							  	CNetDC_LotteryChild::_LotteryNumber[5];	//�̷s�@��
int								CNetDC_LotteryChild::_LotteryPrizeCount[3];
int							  	CNetDC_LotteryChild::_LotteryTotalMoney = 0;	//�ֿn���B
int							  	CNetDC_LotteryChild::_RunLotteryTime;		//�]�ֳz���ɶ�
bool							CNetDC_LotteryChild::_IsInitReady = false;
//--------------------------------------------------------------------------------------
bool    CNetDC_LotteryChild::Init()
{
	srand( (unsigned)_time32( NULL ) );
	

	CNetDC_Lottery::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetDC_LotteryChild );


/*
	_RD_NormalDB->SqlCmd( 0 , _SQLCmdInitProc , _SQLCmdInitProcResult , NULL	, NULL );

	printf( "\n");
	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 1000 );
		_RD_NormalDB->Process( );

		switch( i % 4 )
		{
		case 0:
			printf( "\rLottery Info Loading..... /        " );
			break;
		case 1:
			printf( "\rLottery Info Loading..... |        " );
			break;
		case 2:			
			printf( "\rLottery Info Loading..... \\        " );
			break;
		case 3:
			printf( "\rLottery Info Loading..... -        " );
			break;
		}

		if( _IsInitReady == true )
			break;
	}
	printf( "\n");

	Global::Schedular()->Push( _OnTimeProc_ , 60*1000, NULL , NULL );
	*/
	return true;
}
//--------------------------------------------------------------------------------------
bool    CNetDC_LotteryChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	CNetDC_Lottery::_Release();

	return true;
}
struct TempExchangePrizeStruct
{
	int				SrvSockID;
	int				PlayerDBID;
	ItemFieldStruct Item;
	int				Pos;

	//Return Value
	int				PrizeMoney;
	int				PrizeCount[3];
};

void CNetDC_LotteryChild::RL_ExchangePrize		( int SrvSockID , int PlayerDBID , ItemFieldStruct& Item , int Pos )
{
	//���o�������
	if( _LotteryHistory.size() <= (unsigned) Item.Lottery.Index )
	{
		SL_ExchangePrizeResultFailed( SrvSockID , Item , Pos , PlayerDBID );
		return;
	}

	TempExchangePrizeStruct* Temp = NEW( TempExchangePrizeStruct );

	Temp->SrvSockID		= SrvSockID;
	Temp->PlayerDBID	= PlayerDBID;
	Temp->Item			= Item;
	Temp->Pos			= Pos;

	_RD_NormalDB->SqlCmd( 9 , _SQLCmdLotteryExchangePrize , _SQLCmdLotteryExchangePrizeResult , Temp 	, NULL );
}
//////////////////////////////////////////////////////////////////////////
//�p�⤤���ƶq
int	CNetDC_LotteryChild::_CalLotteryHitBall( LotteryInfoStruct& LotteryPrizeInfo , ItemFieldStruct& Item )
{
	int HitCount = 0;

	for( int i = 0 ; i < Item.Lottery.Count ; i++ )
	{
		for( int j = 0 ; j < 5 ; j++ )
		{
			if( LotteryPrizeInfo.Num[j] == Item.Lottery.SelectID[i] )
			{
				HitCount++;
				break;
			}

		}
	}

	return HitCount;
}

bool CNetDC_LotteryChild::_SQLCmdLotteryExchangePrize( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char	Buf[512];
	TempExchangePrizeStruct* UserData = (TempExchangePrizeStruct*)UserObj;

	//�ˬd�o�i�m���I���L���~�F��
	MyDbSqlExecClass	MyDBProc( sqlBase );

	bool	IsFind;

	sprintf( Buf , "SELECT ItemVersion FROM Lottery_ExchangePrize WHERE ItemVersion=%d and ItemCreateTime=%d" , UserData->Item.Serial , UserData->Item.CreateTime );
	MyDBProc.SqlCmd( Buf );

	if( MyDBProc.Read() == false )
		IsFind = false;
	else
		IsFind = true;

	MyDBProc.Close();

	//�o�i�m���I�L��
	if( IsFind == true )
		return false;
	g_CritSect()->Enter();
	if( _LotteryHistory.size() <= (unsigned) UserData->Item.Lottery.Index )
	{
		g_CritSect()->Leave();
		return false;
	}

	LotteryInfoStruct& LotteryPrizeInfo = _LotteryHistory[ UserData->Item.Lottery.Index];

	int HitCount = _CalLotteryHitBall( LotteryPrizeInfo , UserData->Item );

	LotteryInfoTableStruct* LotteryInfoTable = g_ObjectData->LotteryInfoTable( UserData->Item.Lottery.Count );

	if( LotteryInfoTable == NULL )
	{
		g_CritSect()->Leave();
		return false;
	}

	int PrizeCount[3];
	int PrizeMoney = 0;

	//�p�⤤���ƶq �P �p��m��
	for( int i = 0 ; i < 3; i++ )
	{
		PrizeCount[ i ] = LotteryInfoTable->SelectPrize[HitCount][i];
		PrizeMoney = PrizeMoney + LotteryPrizeInfo.PrizeMoney[ i ] * PrizeCount[ i ];		
	}
	g_CritSect()->Leave();
	
//	MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );
	sprintf( Buf , "UPDATE Lottery Set ExchangePrize1Count = ExchangePrize1Count+%d , ExchangePrize2Count = ExchangePrize2Count+%d , ExchangePrize3Count = ExchangePrize3Count+%d  , ExchangeMoney = ExchangeMoney+%d" 
		, PrizeCount[0] ,PrizeCount[1],PrizeCount[2],PrizeMoney );

	MyDBProc.SqlCmd_WriteOneLine( Buf );

	sprintf( Buf , "INSERT Lottery_ExchangePrize( Version, Prize1, Prize2, Prize3, Money, PlayerDBID,ItemCreateTime, ItemVersion, Count, Num20, Num19, Num18, Num17, Num16, Num15, Num14,Num13, Num12, Num11, Num10, Num9, Num8, Num7, Num6, Num5, Num4,Num3, Num2, Num1) Values( %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d )"
		, UserData->Item.Lottery.Index , PrizeCount[0] ,PrizeCount[1],PrizeCount[2],PrizeMoney , UserData->PlayerDBID , UserData->Item.CreateTime , UserData->Item.Serial , UserData->Item.Lottery.Count 
		, UserData->Item.Lottery.SelectID[19] , UserData->Item.Lottery.SelectID[18] , UserData->Item.Lottery.SelectID[17] 
		, UserData->Item.Lottery.SelectID[16] , UserData->Item.Lottery.SelectID[15] , UserData->Item.Lottery.SelectID[14] 
		, UserData->Item.Lottery.SelectID[13] , UserData->Item.Lottery.SelectID[12] , UserData->Item.Lottery.SelectID[11] 
		, UserData->Item.Lottery.SelectID[10] , UserData->Item.Lottery.SelectID[9 ] , UserData->Item.Lottery.SelectID[8 ] 
		, UserData->Item.Lottery.SelectID[7 ] , UserData->Item.Lottery.SelectID[6 ] , UserData->Item.Lottery.SelectID[5 ] 
		, UserData->Item.Lottery.SelectID[4 ] , UserData->Item.Lottery.SelectID[3 ] , UserData->Item.Lottery.SelectID[2 ] 
		, UserData->Item.Lottery.SelectID[1 ] , UserData->Item.Lottery.SelectID[0 ] );
	MyDBProc.SqlCmd_WriteOneLine( Buf ); 

//	MyDBProc.SqlCmd_WriteOneLine( "COMMIT TRANSACTION" );


	UserData->PrizeMoney = PrizeMoney;
	memcpy( UserData->PrizeCount , PrizeCount , sizeof(UserData->PrizeCount) );

	return true;
}
void CNetDC_LotteryChild::_SQLCmdLotteryExchangePrizeResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempExchangePrizeStruct* UserData = (TempExchangePrizeStruct*)UserObj;

	if( ResultOK == true )
	{
		SL_ExchangePrizeResultOK( UserData->SrvSockID , UserData->Item , UserData->Pos , UserData->PlayerDBID , UserData->PrizeCount , UserData->PrizeMoney );
	}
	else
	{
		SL_ExchangePrizeResultFailed( UserData->SrvSockID , UserData->Item , UserData->Pos , UserData->PlayerDBID );
	}

	delete UserData;
}
//////////////////////////////////////////////////////////////////////////
struct TempBuyLotteryStruct
{
	int				SrvSockID;
	int				PlayerDBID;
	int				Money;
	vector<int>		Number;
};

void CNetDC_LotteryChild::RL_BuyLottery			( int SrvSockID , int PlayerDBID , int Money , vector<int>& Number )
{
	if( PlayerDBID == -1 && Money == -1 )
	{
		_RunLotteryTime = 0;
		return;
	}

	TempBuyLotteryStruct* Temp = NEW(TempBuyLotteryStruct);

	Temp->SrvSockID = SrvSockID;
	Temp->PlayerDBID = PlayerDBID;
	Temp->Money = Money;
	Temp->Number = Number;
/*
	if( Number.size() == 5 )
	{
		if( Number[0] == 1 
			&& Number[1] == 2 
			&& Number[2] == 3 
			&& Number[3] == 4 
			&& Number[4] == 5 	)
			_RunLotteryTime = 0;
	}
*/
	_RD_NormalDB->SqlCmd( 10 , _SQLCmdLotteryBuy , _SQLCmdLotteryBuyResult , Temp 	, NULL );
}

bool CNetDC_LotteryChild::_SQLCmdLotteryBuy( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char Buf[1024];
	TempBuyLotteryStruct* UserData = (TempBuyLotteryStruct*)UserObj;

	//�p�⤤����
	int HitCount = 0;

	for( int i = 0 ; i < (int)UserData->Number.size() ; i++ )
	{
		for( int j = 0 ; j < 5 ; j++ )
		{
			if( _LotteryNumber[j] == UserData->Number[i] )
			{
				HitCount++;
				break;
			}
		}
	}

	//���o�ֳz��ƪ�
	LotteryInfoTableStruct* LotteryInfoTable = g_ObjectData->LotteryInfoTable( (int)UserData->Number.size() );
	if( LotteryInfoTable == NULL )
		return false;

	if( LotteryInfoTable->CombinationCount * DEF_LOTTERY_COST != UserData->Money )
		return false;


	int PrizeCount[3];
	//�p�⤤���ƶq �P �p��m��
	for( int i = 0 ; i < 3; i++ )
	{
		PrizeCount[ i ] = LotteryInfoTable->SelectPrize[HitCount][i];
	}

	//��s�������
	MyDbSqlExecClass	MyDBProc( sqlBase );

//	MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );
	sprintf( Buf , "UPDATE Lottery Set Prize1Count = Prize1Count+%d , Prize2Count = Prize2Count+%d , Prize3Count = Prize3Count+%d , Money = Money+%d WHERE Version=%d" 
		, PrizeCount[0] ,PrizeCount[1],PrizeCount[2], UserData->Money , _LotteryVersion );
	MyDBProc.SqlCmd_WriteOneLine( Buf );
//	MyDBProc.SqlCmd_WriteOneLine( "COMMIT TRANSACTION" );

	_LotteryTotalMoney += UserData->Money;

	for( int i = 0 ; i < 3 ; i++ )
		_LotteryPrizeCount[i] += PrizeCount[i];

	return true;
}
void CNetDC_LotteryChild::_SQLCmdLotteryBuyResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempBuyLotteryStruct* UserData = (TempBuyLotteryStruct*)UserObj;
	int Number[20];
	for( int i = 0 ; i< (int)UserData->Number.size() ; i++ )
	{
		Number[i] = UserData->Number[i];
	}

	if( ResultOK == true )
	{
		SL_BuyLotteryResult( UserData->SrvSockID , _LotteryVersion , UserData->Money , UserData->PlayerDBID , (int)UserData->Number.size() , Number , true );
	}
	else
	{
		SL_BuyLotteryResult( UserData->SrvSockID , _LotteryVersion , UserData->Money , UserData->PlayerDBID , (int)UserData->Number.size() , Number , false );
	}
	delete UserData;
}
//////////////////////////////////////////////////////////////////////////
void CNetDC_LotteryChild::RL_LotteryInfoRequest	( int SrvSockID , int PlayerDBID )
{
	SC_LotteryInfo( PlayerDBID , _LotteryTotalMoney / 100 * DEF_LOTTERY_PRIZE1_RATE , _LotteryVersion );
}

void CNetDC_LotteryChild::RL_LotteryHistoryInfoRequest	( int SrvSockID , int PlayerDBID )
{
	SC_LotteryHistoryInfo( PlayerDBID , _Last5LotteryInfo );
}


int CNetDC_LotteryChild::_OnTimeProc_( SchedularInfo* Obj , void* InputClass )
{
	if( Global::_IsDestory == true )
		return 0;
	if( Global::_DisableLottery == true )
		return 0;
	//
	__time32_t NowTime;
	_time32( &NowTime );

	//�}��
	if( (int)NowTime >= _RunLotteryTime )
	{
		LotteryInfoStruct Info;

		Info.Version = _LotteryVersion;		//�ĴX�� ( -1 �N��S�� )
		Info.TotalMoney = _LotteryTotalMoney;
		Info.RunLotteryTime = (int)NowTime;
		memcpy( Info.Num , _LotteryNumber , sizeof(Info.Num) );
		memcpy( Info.PrizePlayerCount , _LotteryPrizeCount , sizeof(Info.PrizePlayerCount) );

		if( Info.PrizePlayerCount[0] == 0 )
			Info.PrizeMoney[0] = Info.TotalMoney / 100 * DEF_LOTTERY_PRIZE1_RATE;
		else
			Info.PrizeMoney[0] = Info.TotalMoney / Info.PrizePlayerCount[0] /100 * DEF_LOTTERY_PRIZE1_RATE;

		if( Info.PrizePlayerCount[1] == 0 )
			Info.PrizeMoney[1] = Info.TotalMoney / 100 * DEF_LOTTERY_PRIZE2_RATE;
		else
			Info.PrizeMoney[1] = Info.TotalMoney / Info.PrizePlayerCount[1] /100 * DEF_LOTTERY_PRIZE2_RATE;

		if( Info.PrizePlayerCount[2] == 0 )
			Info.PrizeMoney[2] = Info.TotalMoney / 100 * DEF_LOTTERY_PRIZE3_RATE;
		else
			Info.PrizeMoney[2] = Info.TotalMoney / Info.PrizePlayerCount[2] /100 * DEF_LOTTERY_PRIZE3_RATE;

		g_CritSect()->Enter();
		_LotteryHistory.push_back( Info );
		g_CritSect()->Leave();

		for( int i = 3 ; i >= 0 ; i--)
			_Last5LotteryInfo[ i+1 ] = _Last5LotteryInfo[i];
		_Last5LotteryInfo[0] = Info;
		//////////////////////////////////////////////////////////////////////////
	
		_RD_NormalDB->SqlCmd( 0 , _SQLCmdLotteryRun , _SQLCmdLotteryRunResult , NULL 	, NULL );
		//////////////////////////////////////////////////////////////////////////
		_RunLotteryTime = (int)NowTime + 24*60*60*7;
	}
	else
	{
		_RD_NormalDB->SqlCmd( 0 , _SQLCmdLotteryGetTotoalMoney , _SQLCmdLotteryGetTotoalMoneyResult , NULL 	, NULL );
	}

	int DTime = _RunLotteryTime - (int)NowTime;
	
	if( (unsigned)DTime > 20 || DTime < 0)
		Global::Schedular()->Push( _OnTimeProc_ , 20000, NULL , NULL );
	else
		Global::Schedular()->Push( _OnTimeProc_ , DTime* 1000 , NULL , NULL );
	return 0;
}
//////////////////////////////////////////////////////////////////////////
//�}���B�z
bool CNetDC_LotteryChild::_SQLCmdLotteryRun( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char Buf[1024];

	MyDbSqlExecClass	MyDBProc( sqlBase );

//	MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );
	srand( (unsigned)_time32( NULL ) );
	_LotteryVersion++;
	/////////////////////////////////////////////////////////////////////////
	g_CritSect()->Enter();
	LotteryInfoStruct& LotteryInfo = _LotteryHistory.back();
	//���o��Ʈw�����T(�`�m��,�Ĥ@��,�ĤG��,�ĤT��)
	//sprintf( Buf , "SELECT Money , Prize1Count , Prize2Count , Prize3Count FROM Lottery WITH (TABLOCKX) WHERE Version=%d" , LotteryInfo.Version );
	sprintf( Buf , "SELECT Money , Prize1Count , Prize2Count , Prize3Count FROM Lottery WHERE Version=%d" , LotteryInfo.Version );
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind(  1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&LotteryInfo.TotalMoney );
	MyDBProc.Bind(  2, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&LotteryInfo.PrizePlayerCount[0] );
	MyDBProc.Bind(  3, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&LotteryInfo.PrizePlayerCount[1] );
	MyDBProc.Bind(  4, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&LotteryInfo.PrizePlayerCount[2] );
	MyDBProc.Read( );
	MyDBProc.Close( );

	if( LotteryInfo.PrizePlayerCount[0] == 0 )
		LotteryInfo.PrizeMoney[0] = LotteryInfo.TotalMoney / 100 * DEF_LOTTERY_PRIZE1_RATE;
	else
		LotteryInfo.PrizeMoney[0] = LotteryInfo.TotalMoney / LotteryInfo.PrizePlayerCount[0] /100 * DEF_LOTTERY_PRIZE1_RATE;

	if( LotteryInfo.PrizePlayerCount[1] == 0 )
		LotteryInfo.PrizeMoney[1] = LotteryInfo.TotalMoney / 100 * DEF_LOTTERY_PRIZE2_RATE;
	else
		LotteryInfo.PrizeMoney[1] = LotteryInfo.TotalMoney / LotteryInfo.PrizePlayerCount[1] /100 * DEF_LOTTERY_PRIZE2_RATE;

	if( LotteryInfo.PrizePlayerCount[2] == 0 )
		LotteryInfo.PrizeMoney[2] = LotteryInfo.TotalMoney / 100 * DEF_LOTTERY_PRIZE3_RATE;
	else
		LotteryInfo.PrizeMoney[2] = LotteryInfo.TotalMoney / LotteryInfo.PrizePlayerCount[2] /100 * DEF_LOTTERY_PRIZE3_RATE;
	/////////////////////////////////////////////////////////////////////////
	int Number[20];
	for( int i = 0 ; i < 20 ; i++ )
		Number[i] = i+1;

	for( int j = 0 ; j < 50 ; j++ )
		for( int i = 0 ; i < 20 ; i++ )
			swap( Number[ rand()%20 ] , Number[i] );

	for( int i = 0 ; i < 5 ; i++ )
		_LotteryNumber[i] = Number[i];

	if( _LotteryHistory.back().PrizePlayerCount[0] == 0 )
		_LotteryTotalMoney = _LotteryHistory.back().PrizeMoney[0] + _LotteryHistory.back().TotalMoney / 100 * DEF_LOTTERY_BONUS_RATE;
	else
		_LotteryTotalMoney = _LotteryHistory.back().TotalMoney / 100 * DEF_LOTTERY_BONUS_RATE;

	//���o�ثe�ɶ�
	__time32_t NowTime;
	_time32( &NowTime );
	struct tm *gmt;
	gmt = _localtime32( &NowTime );
	//�p��U�����}����
	gmt->tm_hour = 20;
	gmt->tm_sec = 0;
    gmt->tm_min = 0;
	//gmt->tm_mday = gmt->tm_mday + 1;//7 - gmt->tm_wday;
	if( gmt->tm_wday < 3 )
		gmt->tm_mday = gmt->tm_mday - gmt->tm_wday + 3;
	else
		gmt->tm_mday = gmt->tm_mday - gmt->tm_wday + 3 + 7;


	_RunLotteryTime = (int)_mktime32( gmt );

	//�ˬd�O�_�̷s�@���w����LServer�n�J
	int MaxVersion = 0;
	MyDBProc.SqlCmd( "SELECT MAX(Version) FROM Lottery" );
	MyDBProc.Bind(  1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&MaxVersion );	
	MyDBProc.Read();
	MyDBProc.Close();

	if( MaxVersion < _LotteryVersion )
	{
		//��s��Ʈw
		sprintf( Buf , "INSERT Lottery (Version,RunLotteryTime,Num1,Num2,Num3,Num4,Num5,Money,LastMoney) VALUES ( %d , %d,%d,%d,%d,%d,%d,%d,%d)"
			, _LotteryVersion , _RunLotteryTime , _LotteryNumber[0] , _LotteryNumber[1] ,_LotteryNumber[2] ,_LotteryNumber[3] ,_LotteryNumber[4] 
			, _LotteryTotalMoney , _LotteryTotalMoney );

		MyDBProc.SqlCmd_WriteOneLine( Buf );
	}
	else
	{

		//Ū�����v��T
		sprintf( Buf , "SELECT Version , Money , Prize1Count , Prize2Count , Prize3Count , Num1 , Num2 , Num3 , Num4 , Num5 , RunLotteryTime FROM Lottery WHERE Version=%d" , MaxVersion  );
		MyDBProc.SqlCmd( Buf );

		MyDBProc.Bind(  1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&_LotteryVersion);
		MyDBProc.Bind(  2, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&_LotteryTotalMoney );

		for( int i = 0 ; i < 3 ; i++)
			MyDBProc.Bind(  3 + i , SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&_LotteryPrizeCount[i] );
		for( int i = 0 ; i < 5 ; i++)
			MyDBProc.Bind(  6 + i, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&_LotteryNumber[i] );

		MyDBProc.Bind(  11 , SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&_RunLotteryTime );
		MyDBProc.Read();
		MyDBProc.Close();

	}

	memset( _LotteryPrizeCount , 0 , sizeof(_LotteryPrizeCount) );
	
//	MyDBProc.SqlCmd_WriteOneLine( "COMMIT TRANSACTION" );
	g_CritSect()->Leave();
	return true;
}
void CNetDC_LotteryChild::_SQLCmdLotteryRunResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	if( ResultOK == true )
	{
		g_CritSect()->Enter();
		SC_RunLottery( _LotteryHistory.back() );
		g_CritSect()->Leave();
	}
}

//////////////////////////////////////////////////////////////////////////
//��l�]�w
bool CNetDC_LotteryChild::_SQLCmdInitProc( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char Buf[1024];
	MyDbSqlExecClass	MyDBProc( sqlBase );

	//Ū�����v��T
	LotteryInfoStruct Temp;
	MyDBProc.SqlCmd( "SELECT Version , Money , Prize1Count , Prize2Count , Prize3Count , Num1 , Num2 , Num3 , Num4 , Num5 , RunLotteryTime FROM Lottery " );

	MyDBProc.Bind(  1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Temp.Version );
	MyDBProc.Bind(  2, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Temp.TotalMoney );

	for( int i = 0 ; i < 3 ; i++)
		MyDBProc.Bind(  3 + i , SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Temp.PrizePlayerCount[i] );
	for( int i = 0 ; i < 5 ; i++)
		MyDBProc.Bind(  6 + i, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Temp.Num[i] );

	MyDBProc.Bind(  11 , SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&Temp.RunLotteryTime );

	while( MyDBProc.Read() )
	{
	//	_LotteryHistory[ Temp.Version ] = Temp;
		//�p��C�Ӽ��������B
		
		if( Temp.PrizePlayerCount[0] == 0 )
			Temp.PrizeMoney[0] = Temp.TotalMoney /100 * DEF_LOTTERY_PRIZE1_RATE;
		else
			Temp.PrizeMoney[0] = Temp.TotalMoney / Temp.PrizePlayerCount[0] /100 * DEF_LOTTERY_PRIZE1_RATE;

		if( Temp.PrizePlayerCount[1] == 0 )
			Temp.PrizeMoney[1] = Temp.TotalMoney /100 * DEF_LOTTERY_PRIZE2_RATE;
		else
			Temp.PrizeMoney[1] = Temp.TotalMoney / Temp.PrizePlayerCount[1] /100 * DEF_LOTTERY_PRIZE2_RATE;

		if( Temp.PrizePlayerCount[2] == 0 )
			Temp.PrizeMoney[2] = Temp.TotalMoney /100 * DEF_LOTTERY_PRIZE3_RATE;
		else
			Temp.PrizeMoney[2] = Temp.TotalMoney / Temp.PrizePlayerCount[2] /100 * DEF_LOTTERY_PRIZE3_RATE;

		//Temp.PrizeMoney[1] = Temp.TotalMoney / Temp.PrizePlayerCount[1] /100 * DEF_LOTTERY_PRIZE2_RATE;
		//Temp.PrizeMoney[2] = Temp.TotalMoney / Temp.PrizePlayerCount[2] /100 * DEF_LOTTERY_PRIZE3_RATE;
		g_CritSect()->Enter();
		_LotteryHistory.push_back(Temp);
		g_CritSect()->Leave();
	};

	MyDBProc.Close();


	g_CritSect()->Enter();
	//�p�G�S����ơA�إߤU�@������T
	if( _LotteryHistory.size() == 0 )
	{
		srand( (unsigned)_time32( NULL ) );

		_LotteryVersion = 0;
		int Number[20];
		for( int i = 0 ; i < 20 ; i++ )
			Number[i] = i+1;
		for( int i = 0 ; i < 20 ; i++ )
			swap( Number[ rand()%20 ] , Number[i] );

		for( int i = 0 ; i < 5 ; i++ )
			_LotteryNumber[i] = Number[i];

		_LotteryTotalMoney = 0;
		//_RunLotterTime = 

		//���o�ثe�ɶ�
		__time32_t NowTime;
		_time32( &NowTime );
		struct tm *gmt;
		gmt = _localtime32( &NowTime );
		//�p��U�����}����

		//gmt->tm_yday = gmt->tm_yday + 7 - gmt->tm_wday;
		//gmt->tm_wday = 0;
		gmt->tm_hour = 20;
		gmt->tm_sec = 0;
        gmt->tm_min = 0;
		//gmt->tm_mday = gmt->tm_mday + 1;//7 - gmt->tm_wday;

		if( gmt->tm_wday < 3 )
			gmt->tm_mday = gmt->tm_mday - gmt->tm_wday + 3;
		else
			gmt->tm_mday = gmt->tm_mday - gmt->tm_wday + 3 + 7;
		

		_RunLotteryTime = (int)_mktime32( gmt );

		//��s��Ʈw
		sprintf( Buf , "INSERT Lottery (Version,RunLotteryTime,Num1,Num2,Num3,Num4,Num5) VALUES ( 0 , %d,%d,%d,%d,%d,%d)"
						, _RunLotteryTime , _LotteryNumber[0] , _LotteryNumber[1] ,_LotteryNumber[2] ,_LotteryNumber[3] ,_LotteryNumber[4] );

		MyDBProc.SqlCmd_WriteOneLine( Buf );

		memset( _LotteryPrizeCount , 0 , sizeof(_LotteryPrizeCount) );
	}
	else
	{
		_LotteryVersion		= _LotteryHistory.back().Version;
		_LotteryTotalMoney	= _LotteryHistory.back().TotalMoney;
		_RunLotteryTime		= _LotteryHistory.back().RunLotteryTime;
		for( int i = 0 ; i < 5 ; i++ )
			_LotteryNumber[i] = _LotteryHistory.back().Num[i];
		for( int i = 0 ; i < 3 ; i++ )
			_LotteryPrizeCount[i] = _LotteryHistory.back().PrizePlayerCount[i];
		_LotteryHistory.pop_back();
	}

	//////////////////////////////////////////////////////////////////////////
	for( int i = 0 ; i < 5 ; i++ )
		_Last5LotteryInfo[i].Version = -1;

	//���X�̫�5��
	for( int i = (int)_LotteryHistory.size() - 1 , j = 0 ; i >= 0 && j < 5 ; i-- , j++ )
	{
		_Last5LotteryInfo[j] = _LotteryHistory[i];
	}
	g_CritSect()->Leave();

	return true;
}
void CNetDC_LotteryChild::_SQLCmdInitProcResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{

	_IsInitReady = true;
}
//////////////////////////////////////////////////////////////////////////
//�w�ɨ��`����
bool CNetDC_LotteryChild::_SQLCmdLotteryGetTotoalMoney( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char Buf[1024];
	MyDbSqlExecClass	MyDBProc( sqlBase );
	int	TotalMoney = 0;
	
	//���o��Ʈw�����T(�`�m��,�Ĥ@��,�ĤG��,�ĤT��)
	sprintf( Buf , "SELECT Money FROM Lottery WHERE Version=%d" , _LotteryVersion );
	MyDBProc.SqlCmd( Buf );
	MyDBProc.Bind(  1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TotalMoney );
	MyDBProc.Read( );
	MyDBProc.Close( );
	Arg.PushValue( "Money" , TotalMoney );
	return true;
}
void CNetDC_LotteryChild::_SQLCmdLotteryGetTotoalMoneyResult ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	int Money = Arg.GetNumber( "Money" );
	if( Money != 0 )
		_LotteryTotalMoney = Money;
}
//////////////////////////////////////////////////////////////////////////