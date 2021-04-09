#include "ServerListSCliClass.h"
//----------------------------------------------------------------------------------------------

bool		LBServerList::OnRecv		( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData )
{
    return _Parent->_OnPacket( dwNetID , dwPackSize , pPacketData );
}
void		LBServerList::OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode )
{
    _Parent->_OnConnectFailed( dwFailedCode );
}
void		LBServerList::OnConnect		( DWORD dwNetID )
{
    _Parent->_OnConnected( dwNetID );
}
void		LBServerList::OnDisconnect	( DWORD dwNetID )
{
    _Parent->_OnDisconnected( dwNetID );
}
CEventObj*	LBServerList::OnAccept		( DWORD dwNetID )
{
    return this;
}
//-----------------------------------------------------------------------------------
// 初始化 Client 與 Server 的 CallBack函式
//-----------------------------------------------------------------------------------
void	ServerListSCliClass::InitPGCallBack( )
{
    int		i;
    //callback 函式陣列出始化
    for( i = 0 ; i <EM_ServerListNet_Packet_Count ; i++ )
    {
        _PGCallBack[i] = &ServerListSCliClass::_PGxUnKnow;
    }
	_PGCallBack[ EM_ServerListNet_StoSC_CliReloginNotify ] = &ServerListSCliClass::_ServerListNet_SCtoS_CliReloginNotify;
	
}
//----------------------------------------------------------------------------------------------
void	ServerListSCliClass::_OnConnected( DWORD NetID )
{
	 Print( LV2 , "_OnConnected" ,  "ServerList connected [%d]", NetID );
	_IsReady = true;
	_NetID = NetID;
}
//----------------------------------------------------------------------------------------------
void	ServerListSCliClass::_OnDisconnected( DWORD id )
{
	_IsReady = false;
    Print( LV2 , "_OnDisconnected" , "ServerList disconnect ==> 2 sec reconnect serverList" );

	Schedular()->Push( _ReConnectServerList , 2000 , this , NULL );
}
//----------------------------------------------------------------------------------------------
bool	ServerListSCliClass::_OnPacket( DWORD NetID , DWORD Size , void* Data )
{
    ServerListNet_SysNull* PG = ( ServerListNet_SysNull *) Data;

     if(EM_ServerListNet_Packet_Count <= (UINT32)PG->Command )
     {
         Print( LV2 , "_OnPacket" ,"ServerListSCliClass  packet cmd=%d out of range" , NetID , PG->Command );
         return false;
     }
     //呼叫Callback 處理
     (this->*_PGCallBack[PG->Command])( NetID , PG , Size );

    //------------------------------------------------------------------------------
    return true;
}
//----------------------------------------------------------------------------------------------
void	ServerListSCliClass::_OnConnectFailed( int id )
{
	_IsReady = false;
    Print( LV2 , "_OnConnectFailed" , "ServerList connect failed ==> 2 sec Reconnect ServerList" );
    Schedular()->Push( _ReConnectServerList , 2000 , this , NULL );   
}
//----------------------------------------------------------------------------------------------
ServerListSCliClass::ServerListSCliClass( )
{
	_Schedular  = NEW FunctionSchedularClass ;
    InitPGCallBack( );
    _BListener  = NEW LBServerList( this );
    _BNet       = CreateNetBridge( );
	_IsReady	= false;
	_NetID		= -1;
}
//----------------------------------------------------------------------------------------------
ServerListSCliClass::~ServerListSCliClass( )
{
    DeleteNetBridge( _BNet );
    delete _BListener;
    delete _Schedular;
}
//----------------------------------------------------------------------------------------------
void	ServerListSCliClass::Connect( char* IP , int Port )
{

    _Port	= Port;
    _IP		= IP;

    int IPNum = ConvertIP( IP );
    _BNet->Connect( IPNum , Port , _BListener );	

}
//----------------------------------------------------------------------------------------------
//定時處理
void	ServerListSCliClass::Process( )
{
    _BNet->Flush();
	Schedular()->Process();
}
//----------------------------------------------------------------------------------------------
FunctionSchedularClass* 	ServerListSCliClass::Schedular()
{
	return _Schedular;
}
//----------------------------------------------------------------------------------------------
//如果斷線重新聯結Account Agent
int ServerListSCliClass::_ReConnectServerList( SchedularInfo* Obj , void* InputClass )
{
	ServerListSCliClass* Class = (ServerListSCliClass*)InputClass;
	Print( LV2 , "_ReConnectServerList" , "Reconnect  ServerList" );
	int IPNum = ConvertIP( Class->_IP.c_str() );
	Class->_BNet->Connect( IPNum , Class->_Port , Class->_BListener );	
	return 0;
}
//----------------------------------------------------------------------------------------------
//把目前的世界狀況送到ServerList
void	ServerListSCliClass::SendServerListInfo( int GameGroupID , int GameID , const char* ServerName , const char* IP_DNS , int Port , int MaxPlayerCount , int ParallelZoneCount , int PlayerCount[100] , int ServerStatus , float TotalPlayerRate )
{
	ServerListNet_SCtoS_RegServerInfo Send;
	MyStrcpy( Send.IP_DNS , IP_DNS , sizeof(Send.IP_DNS) );
	MyStrcpy( Send.ServerName , ServerName , sizeof(Send.ServerName) );
	Send.MaxPlayerCount = MaxPlayerCount;
	Send.ParallelZoneCount = ParallelZoneCount;
	Send.Port = Port;
	Send.GameID = GameID + GameGroupID*0x10000;
	memcpy( Send.PlayerCount , PlayerCount , sizeof( Send.PlayerCount ) );
	Send.ServerStatus = ServerStatus;
	Send.TotalPlayerRate = TotalPlayerRate;

	_BNet->Send( _NetID , sizeof(Send) , &Send );
}
//----------------------------------------------------------------------------------------------
void    ServerListSCliClass::_ServerListNet_SCtoS_CliReloginNotify      ( int NetID , ServerListNet_SysNull* Data , int Size )
{
	ServerListNet_StoSC_CliReloginNotify* PG = (ServerListNet_StoSC_CliReloginNotify*)Data;

	_Listener->ClientRelogin( PG->Account );
}
//----------------------------------------------------------------------------------------------