#include "Net_TalkChild.h"	
//-------------------------------------------------------------------
bool	Net_TalkChild::IsEnterBroadcastingProc = false;
//-------------------------------------------------------------------

bool    Net_TalkChild::Init()
{
	Net_Talk::_Init();

	if( This != NULL)
		return false;

	This = NEW( Net_TalkChild );

	Schedular()->Push( _OnTimeProc , 1000*60 , This , NULL );	

	return true;
}
//--------------------------------------------------------------------------------------
bool    Net_TalkChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	Net_Talk::_Release();
	return true;

}	
//--------------------------------------------------------------------------------------
struct TempOfflineMessageRequestStruct
{
	int ServerID;
	int PlayerDBID;

	vector<OfflineMessageDBStruct>	MsgList;
};

void Net_TalkChild::RL_OfflineMessageRequest( int ServerID , int PlayerDBID )
{
	TempOfflineMessageRequestStruct* TempData = NEW(TempOfflineMessageRequestStruct);

	TempData->ServerID = ServerID;
	TempData->PlayerDBID = PlayerDBID;

	_RD_NormalDB->SqlCmd( PlayerDBID , _SQLCmdOfflineMsgEvent , _SQLCmdOfflineMsgResultEvent , TempData  , NULL );
}
//--------------------------------------------------------------------------------------
bool Net_TalkChild::_SQLCmdOfflineMsgEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	TempOfflineMessageRequestStruct* TempData = (TempOfflineMessageRequestStruct*)UserObj;

	MyDbSqlExecClass	MyDBProc( sqlBase );
	char	SqlCmd[512];

	sprintf( SqlCmd , "WHERE IsDelete = 0 and ToPlayerDBID =%d" , TempData->PlayerDBID );
	MyDBProc.SqlCmd( _RDOfflineMessageSql->GetSelectStr( SqlCmd  ).c_str() );
	OfflineMessageDBStruct TempOfflineMsg;
	
	MyDBProc.Bind( TempOfflineMsg , RoleDataEx::ReaderRD_OfflineMessage() );
	while( MyDBProc.Read( ) )
	{
		RoleDataEx::ReaderRD_OfflineMessage()->TransferWChartoUTF8( &TempOfflineMsg , MyDBProc.wcTempBufList() );
		TempData->MsgList.push_back( TempOfflineMsg );
	}
	MyDBProc.Close();

	sprintf( SqlCmd , "UPDATE OfflineMessage SET IsDelete = 1 WHERE IsDelete = 0 and ToPlayerDBID =%d" , TempData->PlayerDBID );
	MyDBProc.SqlCmd_WriteOneLine( SqlCmd );

	return true;
}

void Net_TalkChild::_SQLCmdOfflineMsgResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	TempOfflineMessageRequestStruct* TempData = (TempOfflineMessageRequestStruct*)UserObj;

	for( unsigned i = 0 ; i < TempData->MsgList.size() ; i++ )
	{
		OfflineMessageDBStruct& Temp = TempData->MsgList[i];
		switch( Temp.Type )
		{
		case 0: //玩家Msg
			SC_OfflineMessage( Temp.ToPlayerDBID , Temp.FromPlayerDBID , Temp.Time , Temp.FromName.Begin() , Temp.ToName.Begin() , Temp.Content.Begin() );
			break;
		case 1:	//Gm command
			SL_OfflineGMCommand( TempData->ServerID , TempData->PlayerDBID , Temp.ManageLv , Temp.Content.Begin() );
			break;
		}
	}
	delete TempData;
}
//定時處理

int Net_TalkChild::_OnTimeProc( SchedularInfo* Obj , void* InputClass )
{
	if( Global::_IsDestory == true )
		return 0;

	static vector<string>	MessageList;
	if( IsEnterBroadcastingProc == false )
	{
		IsEnterBroadcastingProc = true;
		MessageList.clear();
		_RD_NormalDB_Import->SqlCmd( rand() , _SQLOnTimeBroadCastingProcEvent , _SQLOnTimeBroadCastingProcResultEvent , &MessageList , NULL );		 
	}
	Schedular()->Push( _OnTimeProc , 1000*60 , This , NULL );  
	return 0;
}

bool Net_TalkChild::_SQLOnTimeBroadCastingProcEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	vector<string>& MessageList = *((vector<string>*)UserObj);

	MyDbSqlExecClass	MyDBProc( sqlBase );
	CharTransferClass	CharTransfer;

	vector< int > GuildList;

	char	CmdBuf[4096];
	int		GUID;
	wchar_t	Msg[512];

	sprintf( CmdBuf , "SELECT GUID , Message FROM BroadcastingTable Where  (WorldID = %d) AND (Type=0) AND (NextTime < EndTime) AND (NextTime < GETDATE())" , Global::GetWorldID()  );

	MyDBProc.SqlCmd( CmdBuf );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&GUID );
	MyDBProc.Bind( 2, SQL_WCHAR , sizeof(Msg) , (LPBYTE)&Msg );

	while( MyDBProc.Read( ) )
	{	
		GuildList.push_back( GUID );
		CharTransfer.SetWCString( Msg );
		MessageList.push_back( CharTransfer.GetUtf8String() );
	}

	MyDBProc.Close( );
	for( unsigned i = 0 ; i < GuildList.size() ; i++ )
	{
		sprintf( CmdBuf , "UPDATE BroadcastingTable	SET NextTime = DATEADD(minute, DATEDIFF(minute, NextTime, GETDATE())/ IntervalTime * IntervalTime + IntervalTime, NextTime) WHERE GUID = %d" , GuildList[i] );
		MyDBProc.SqlCmd_WriteOneLine( CmdBuf );
	}
	
	return true;
}
void Net_TalkChild::_SQLOnTimeBroadCastingProcResultEvent( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	vector<string>& MessageList = *((vector<string>*)UserObj);

	for( unsigned i = 0 ; i < MessageList.size() ; i++ )
	{
		SC_ALL_RunningMsg( MessageList[i].c_str() );
	}

	IsEnterBroadcastingProc = false;
}
