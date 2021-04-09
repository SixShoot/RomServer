#include "Net_ImportBoardChild.h"	
//-------------------------------------------------------------------
vector< ImportBoardBaseStruct >		Net_ImportBoardChild::_ImportBoardInfo;
int			Net_ImportBoardChild::_LzoSize = 0;
char*		Net_ImportBoardChild::_LzoData = NULL;
float		Net_ImportBoardChild::_LastUpdateTime = 0;
int			Net_ImportBoardChild::_DataCount = 0;
float		Net_ImportBoardChild::_SerialNo = 0;
//-------------------------------------------------------------------

bool    Net_ImportBoardChild::Init()
{
	Net_ImportBoard::_Init();

	if( This != NULL)
		return false;

	This = NEW( Net_ImportBoardChild );

	Global::Schedular()->Push( _OnTimeProc_ , 60*1000, NULL , NULL );

	return true;
}
//--------------------------------------------------------------------------------------
bool    Net_ImportBoardChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	Net_ImportBoard::_Release();
	return true;

}
//--------------------------------------------------------------------------------------
void Net_ImportBoardChild::RL_ImportBoardRequest( int PlayerDBID , float Update )
{
	if( Update == _LastUpdateTime )
	{
		SC_ImportBoardInfo( PlayerDBID , 0 , _LastUpdateTime , 0 , NULL );
		return;
	}

	SC_ImportBoardInfo( PlayerDBID , _DataCount , _LastUpdateTime , _LzoSize , _LzoData );


}
//--------------------------------------------------------------------------------------
int Net_ImportBoardChild::_OnTimeProc_( SchedularInfo* Obj , void* InputClass )
{
	if( Global::_IsDestory == true )
		return 0;

	_RD_NormalDB_Import->SqlCmd( 0 , _SQLCmdOnTimeProc , _SQLCmdOnTimeProcResult , NULL 	, NULL );
	Global::Schedular()->Push( _OnTimeProc_ , 60*1000*5, NULL , NULL );
	return 0;
}

//讀取公告資訊
bool Net_ImportBoardChild::_SQLCmdOnTimeProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{

	MyDbSqlExecClass MyDBProc( sqlBase );
//	MyDBProc.SqlCmd_WriteOneLine( "BEGIN TRANSACTION" );

	//讀取資料庫所有資訊
	CreateDBCmdClass<ImportBoardBaseStruct>* SqlCmdCreator  = NEW CreateDBCmdClass<ImportBoardBaseStruct > ( RoleDataEx::ReaderRD_ImportBoardBase()  , "BoardTable" );
	char	SqlWhereCmd[512];
	float   NowValue = TimeExchangeIntToFloat( TimeStr::Now_Value()  + Global::_TimeZone*60*60 );
	sprintf( SqlWhereCmd , "WHERE ( ( BeginTime < %f or BeginTime = 0 ) and ( EndTime > %f or EndTime = 0) and World%d =  1)  ORDER BY SortNumber" , NowValue , NowValue , Global::GetWorldID() );

	MyDBProc.SqlCmd( SqlCmdCreator->GetSelectStr( SqlWhereCmd ).c_str() );

	ImportBoardBaseStruct TempItem;
	memset( &TempItem , 0 , sizeof( TempItem ) );

	MyDBProc.Bind( TempItem , RoleDataEx::ReaderRD_ImportBoardBase() );
	while( MyDBProc.Read() )
	{
		RoleDataEx::ReaderRD_ImportBoardBase()->TransferWChartoUTF8( &TempItem , MyDBProc.wcTempBufList() );
		_ImportBoardInfo.push_back( TempItem );
	}
	MyDBProc.Close();


//	MyDBProc.SqlCmd_WriteOneLine( "COMMIT TRANSACTION" );

	delete SqlCmdCreator;

	return true;
}
void Net_ImportBoardChild::_SQLCmdOnTimeProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	if( _ImportBoardInfo.size() != 0 )
	{
		ImportBoardBaseStruct* Data = new ImportBoardBaseStruct[ _ImportBoardInfo.size() ];
		bool IsUpdate = false;
		for( unsigned int i = 0 ; i < _ImportBoardInfo.size() ; i++ )
		{
			Data[i] = _ImportBoardInfo[i];
			if( Data[i].UpdateTime > _LastUpdateTime )
			{
				_LastUpdateTime = Data[i].UpdateTime;
				IsUpdate = true;
			}
		}

		if( _LzoData != NULL )
			delete _LzoData;
		_LzoData = NULL;

		MyLzoClass myZip;
		_LzoSize = myZip.Encode_Malloc( (char*)Data 
						, int( _ImportBoardInfo.size() * sizeof( ImportBoardBaseStruct ) ) 
						, &_LzoData );
	

		//如果資料有刪增
		if( (int)_ImportBoardInfo.size() != _DataCount )
			IsUpdate = true;

		if( IsUpdate == true )
		{
			//_SerialNo++;
			SC_All_ImportBoardNewVersion( _LastUpdateTime );
		}

		delete[] Data;
	}
	_DataCount = (int)_ImportBoardInfo.size();
	_ImportBoardInfo.clear();

}

void Net_ImportBoardChild::RL_NewVersionRequest( int SockID , int ProxyID  )
{
	SC_ImportBoardNewVersion( SockID , ProxyID , _LastUpdateTime );
}