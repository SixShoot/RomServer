#include "GatewayClass.h"
#include <time.h>

//////////////////////////////////////////////////////////////////////////
bool        LBSwitch::OnRecv		( DWORD NetID, DWORD PackSize, PVOID PacketData )
{
    _Parent->_OnSwitchPacket(NetID , PackSize , PacketData );
    return true;
}
void		LBSwitch::OnConnectFailed( DWORD NetID, DWORD dwFailedCode )
{
    Print( LV1 , "OnConnectFailed" , "(Switch)_OnSwitchConnectFailed Error=%d" , dwFailedCode );
    _Parent->_OnSwitchConnectFailed(dwFailedCode);
}
void		LBSwitch::OnConnect		( DWORD NetID )
{
    Print( LV1 , "OnConnect" , "(Switch:%d)_OnSwitchConnected" , NetID );
    _Parent->_OnSwitchConnected(NetID);
}
void		LBSwitch::OnDisconnect	( DWORD NetID )
{
    Print( LV1 , "OnDisconnect" , "(Switch:%d)_OnSwitchDisconnected" , NetID );
    _Parent->_OnSwitchDisconnected(NetID);
}
CEventObj*	LBSwitch::OnAccept		( DWORD NetID )
{
    Print( LV1 , "OnAccept" , "(Switch:%d)OnSwitchAccept" , NetID );
    //_Parent->_OnSwitchConnected(NetID);
    return this;
}
//////////////////////////////////////////////////////////////////////////
void	GatewayClass::_OnSwitchConnected     ( int NetID )
{
	g_Schedular.Push( _CheckSwitchConnectAndReg , 15000 , this 
		, "NetID" , EM_ValueType_int , NetID , NULL );
}
void	GatewayClass::_OnSwitchDisconnected  ( int NetID )
{
	Print( LV1 , "_OnSwitchDisconnected" , "(NetID:%d)" , NetID );

	int WorldID = _NetIDtoWorldID.GetID( NetID );
	_NetIDtoWorldID.Cls( NetID , WorldID );


	if( _SwitchMap.size() <= (unsigned) WorldID )
	{
		Print( LV5 , "_OnSwitchDisconnected" , "Switch 連線尚未註冊完成就斷線!!!!" );
		return;	
	}

	SwitchInfoStruct& Info = _SwitchMap[ WorldID ];
	//通知所有的其他Switch Server
	if( Info.Stat == EM_NetStat_Connect )
	{
		SysNet_Gateway_Switch_WorldDisconnect Send;
		Send.WorldID = Info.WorldID;
		_NetBSwitch->SendToAll( sizeof( Send ) , &Send );
	}
	
	Info.Init();

}
bool	GatewayClass::_OnSwitchPacket        ( DWORD NetID , DWORD Size , void* Data )
{

	PGSysNULL* PG = ( PGSysNULL *) Data;

	//------------------------------------------------------------------------------
	//檢查是否為系統封包
	{
		//呼叫Callback 處理
		_PGCallBack[PG->Command]( this , NetID , PG , Size );
	}

	return true;
}
void	GatewayClass::_OnSwitchConnectFailed ( int NetID )
{

}
//////////////////////////////////////////////////////////////////////////
void     GatewayClass::_PGxUnKnow( GatewayClass* Obj,int NetID,PGSysNULL* PG,int Size)
{
	Print( LV2 , "_PGxUnKnow" , "(Switch:%d)_PGxUnKnow : Command = %d" , NetID , PG->Command );
}
//-----------------------------------------------------------------------------------
GatewayClass::GatewayClass()
{
	InitPGCallBack();
	_ExitFlag = false;
	_NetBSwitch		 = CreateNetBridge( );		//主要是用來給Server連結用的 
	_SwitchBListener = NEW LBSwitch( this );	//資料列舉Class

	_NetBSwitch->SetOutPutCB( _Output_NetMsg );

	_ResetBaseInfo();
}
//-----------------------------------------------------------------------------------
GatewayClass::~GatewayClass()
{
	DeleteNetBridge( _NetBSwitch );
	delete _SwitchBListener;
}
//-----------------------------------------------------------------------------------
void	GatewayClass::HostSwitch( char* IP,int Port )
{
	int IPNum = ConvertIP( IP );
	_NetBSwitch->Host( IPNum , Port , _SwitchBListener );	
}
//-----------------------------------------------------------------------------------
//定時處理
bool	GatewayClass::Process( )
{
	CNetBridge::WaitAll( 10 );
	PacketLogProc();
	
	Sleep(1);
	return true;
}
//-----------------------------------------------------------------------------------
void GatewayClass::_AddPGTop10Log( PacketInfoManage& info  )
{
	info.CalPGIDSize();

	char buf[2048];
	sprintf_s( buf , "PGTotalSize=%d (PGID-Size-Count)(%d-%d-%d)(%d-%d-%d)(%d-%d-%d)(%d-%d-%d)(%d-%d-%d)(%d-%d-%d)(%d-%d-%d)(%d-%d-%d)(%d-%d-%d)" 
		, info.PGTotalSize
		, info.PGTop10[0].PGID , info.PGTop10[0].Size/1024 , info.PGTop10[0].Count
		, info.PGTop10[1].PGID , info.PGTop10[1].Size/1024 , info.PGTop10[1].Count
		, info.PGTop10[2].PGID , info.PGTop10[2].Size/1024 , info.PGTop10[2].Count
		, info.PGTop10[3].PGID , info.PGTop10[3].Size/1024 , info.PGTop10[3].Count
		, info.PGTop10[4].PGID , info.PGTop10[4].Size/1024 , info.PGTop10[4].Count
		, info.PGTop10[5].PGID , info.PGTop10[5].Size/1024 , info.PGTop10[5].Count
		, info.PGTop10[6].PGID , info.PGTop10[6].Size/1024 , info.PGTop10[6].Count
		, info.PGTop10[7].PGID , info.PGTop10[7].Size/1024 , info.PGTop10[7].Count
		, info.PGTop10[8].PGID , info.PGTop10[8].Size/1024 , info.PGTop10[8].Count
		, info.PGTop10[9].PGID , info.PGTop10[9].Size/1024 , info.PGTop10[9].Count		);
	Print( LV2 , "Top10" , buf );
}
void	GatewayClass::_ResetBaseInfo()
{
	if( _PGInfoManage.ProcCount == 0 )
		_PGInfoManage.ProcCount = 1;

	_BaseInfo.MinSrvSecPgCount	= _PGInfoManage.PGMinCount;
	_BaseInfo.MinSrvSecPgSize	= _PGInfoManage.PGMinSize;
	_BaseInfo.MaxSrvSecPgCount	= _PGInfoManage.PGMaxCount;
	_BaseInfo.MaxSrvSecPgSize	= _PGInfoManage.PGMaxSize;
	_BaseInfo.AvgSrvSecPgCount	= _PGInfoManage.PGTotalCount / _PGInfoManage.ProcCount;
	_BaseInfo.AvgSrvSecPgSize	= _PGInfoManage.PGTotalSize / _PGInfoManage.ProcCount;
}
//-----------------------------------------------------------------------------------
void GatewayClass::PacketLogProc()
{
	static int	procTime = GetTickCount();
	static int  procCount = 0;

	static int  lastProcTime = procTime;
	int	nowTime = GetTickCount();
	int	dTime = GetTickCount() - procTime;

	int dProcTime = nowTime - lastProcTime;
//	if( _MaxProcTime < dProcTime )
//		_MaxProcTime = dProcTime;

	lastProcTime = nowTime;
	if( dTime > 1000 )
	{
		
		procCount++;
		procTime += 1000;
		_PGInfoManage.Process();

		if( procCount > 60 )
		{			
			_ResetBaseInfo();
			_AddPGTop10Log( _PGInfoManage );
			procCount = 0;
//			_MaxProcTime = 0;
			_PGInfoManage.Reset();

		}
	}

}
//-----------------------------------------------------------------------------------
//送資料到 Switch
bool    GatewayClass::SendToSwitch( int NetID , int Size , void* Data )
{
    _NetBSwitch->Send( NetID , Size , Data );

	return true;
}
//-----------------------------------------------------------------------------------
bool    GatewayClass::SendToSwitch_ByWorldID( int WorldID , int Size , void* Data )
{
	if( WorldID == -1 )
	{
		for( unsigned i = 0 ; i < _SwitchMap.size() ; i++ )
		{
			if( _SwitchMap[i].Stat == EM_NetStat_Unknow )
				continue;

			_NetBSwitch->Send( _SwitchMap[i].ID , Size , Data );
		}
		return true;
	}

	if( _SwitchMap.size() <= (unsigned)WorldID )
		return false;

	if( _SwitchMap[WorldID].Stat == EM_NetStat_Unknow )
		return false;

	_NetBSwitch->Send( _SwitchMap[WorldID].ID , Size , Data );
	return true;
}
//-----------------------------------------------------------------------------------
void GatewayClass::_Output_NetMsg( int lv, const char* szOut )
{
	Print( lv , "NetBridge" , szOut );
}
//-----------------------------------------------------------------------------------
//檢查Switch 註冊是否完成
//如果沒完成 刪除此連線
int     GatewayClass::_CheckSwitchConnectAndReg( SchedularInfo*	Obj , void* InputClass )
{
	GatewayClass*    Class = (GatewayClass*)InputClass;
	int NetID = Obj->GetNumber( "NetID" );
	int WorldID = Class->_NetIDtoWorldID.GetID( NetID );

	if( Class->_SwitchMap.size() <= (unsigned) WorldID )
	{
		Class->_NetBSwitch->Close( NetID );
		return 0;
	}

	SwitchInfoStruct& Info = Class->_SwitchMap[ WorldID ];

	if( Info.Stat!= EM_NetStat_Connect )
	{
		Class->_NetBSwitch->Close( NetID );
		return 0;
	}

	return 0;
}