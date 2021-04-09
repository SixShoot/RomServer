#include "AccountCliClass.h"
//----------------------------------------------------------------------------------------------
/*
void LSAPI LAccount::OnConnected( int id )
{
    _Parent->_OnConnected( id );
}
void LSAPI LAccount::OnDisconnected( int id )
{
    _Parent->_OnDisconnected( id );
}
void LSAPI LAccount::OnPeerShutdown( int id )
{
    _Parent->_OnDisconnected( id );
}
bool LSAPI LAccount::OnPacket( int id, HBUFFER hIncomingData )
{
    return _Parent->_OnPacket( id , hIncomingData );
}
void LSAPI LAccount::OnConnectFailed( int id )
{
    _Parent->_OnConnectFailed( id );
}
*/
bool		LBAccount::OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData )
{
    return _Parent->_OnPacket( dwNetID , dwPackSize , pPacketData );
}
void		LBAccount::OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode )
{
    _Parent->_OnConnectFailed( dwFailedCode );
}
void		LBAccount::OnConnect		( DWORD dwNetID )
{
    _Parent->_OnConnected( dwNetID );
}
void		LBAccount::OnDisconnect	    ( DWORD dwNetID )
{
    _Parent->_OnDisconnected( dwNetID );
}
CEventObj*	LBAccount::OnAccept		    ( DWORD dwNetID )
{
    //_Parent->_OnConnected( dwNetID );
    return this;
}
//-----------------------------------------------------------------------------------
// 初始化 Client 與 Server 的 CallBack函式
//-----------------------------------------------------------------------------------
void	AccountCliClass::InitPGCallBack( )
{
    int		i;
    //callback 函式陣列出始化
    for( i = 0 ; i <EM_AccountNet_Packet_Count ; i++ )
    {
        _PGCallBack[i] = &AccountCliClass::_PGxUnKnow;
    }

    _PGCallBack[ EM_AccountNet_StoC_GSrvLoginResult ]    		= &AccountCliClass::_AccountNet_StoC_GSrvLoginResult;   
    _PGCallBack[ EM_AccountNet_StoC_PlayerLoginResult ]  		= &AccountCliClass::_AccountNet_StoC_PlayerLoginResult; 
    _PGCallBack[ EM_AccountNet_StoC_PlayerLogoutResult ] 		= &AccountCliClass::_AccountNet_StoC_PlayerLogoutResult;
    _PGCallBack[ EM_AccountNet_StoC_CheckAccount ]				= &AccountCliClass::_AccountNet_StoC_CheckAccount;
//    _PGCallBack[ EM_AccountNet_StoC_CheckExchangeItemResult ]   = &AccountCliClass::_AccountNet_StoC_CheckExchangeItemResult;
	_PGCallBack[ EM_AccountNet_StoC_PlayerLoginFailedData ]		= &AccountCliClass::_AccountNet_StoC_PlayerLoginFailedData;
}
//----------------------------------------------------------------------------------------------
void	AccountCliClass::_OnConnected( DWORD NetID )
{
	 Print( LV2 , "_OnConnected" ,  "Account Agent connected" );
    _NetID = NetID;

    AccountNet_CtoS_GSrvLogin Send;
    Send.GameID = _GameID;
    MyStrcpy( Send.Name , _GameName , sizeof(Send.Name ) );

    _BNet->Send( _NetID , sizeof(Send) , &Send );

}
//----------------------------------------------------------------------------------------------
void	AccountCliClass::_OnDisconnected( DWORD id )
{
    Print( LV2 , "_OnDisconnected" , "Account Agent Disconnect ==> 2 sec ReConnect Account Agent" );

	Schedular()->Push( _ReConnectAC , 2000 , this , NULL );
    //_AccountSet.clear();
}
//----------------------------------------------------------------------------------------------
bool	AccountCliClass::_OnPacket( DWORD NetID , DWORD Size , void* Data )
{
//    int		Size;
//    void	*Data;

    //取得封包資料
//    ObjNetGetBuffer( hIncomingData , Size , Data );
    AccountNet_SysNull* PG = ( AccountNet_SysNull *) Data;

    //------------------------------------------------------------------------------

     if(EM_AccountNet_Packet_Count <= (UINT32)PG->Command )
     {
         Print( LV2 , "_OnPacket" ,"AccountCliClass packet cmd=%d out of range " , NetID , PG->Command );
         return false;
     }
     //呼叫Callback 處理
     (this->*_PGCallBack[PG->Command])( NetID , PG , Size );

    //------------------------------------------------------------------------------
    return true;
}
//----------------------------------------------------------------------------------------------
void	AccountCliClass::_OnConnectFailed( int id )
{
    Print( LV2 , "_OnConnectFailed" , "Account Agent connect failed  ==> 2 sec ReConnect Account Agent" );
    Schedular()->Push( _ReConnectAC , 2000 , this , NULL );   
}
//-----------------------------------------------------------------------------------
// 封包Call Back 處理
//-----------------------------------------------------------------------------------
void AccountCliClass::_AccountNet_StoC_GSrvLoginResult      ( int NetID , AccountNet_SysNull* Data , int Size )
{
    AccountNet_StoC_GSrvLoginResult* PG = (AccountNet_StoC_GSrvLoginResult*)Data;
    if( PG->Result == EM_GSrvLoginResult_OK )
    {
        _EventListener->OnLogin( );
    }
    else
    {
		Print( LV5 , "_AccountNet_StoC_GSrvLoginResult" , "Account Login Failed Reslut=%d" , PG->Result );
        _EventListener->OnLoginFailed( PG->Result );
    }
    
}
void AccountCliClass::_AccountNet_StoC_PlayerLoginResult    ( int NetID , AccountNet_SysNull* Data , int Size )
{
    AccountNet_StoC_PlayerLoginResult* PG = (AccountNet_StoC_PlayerLoginResult*)Data;
	Print( LV2 , "_AccountNet_StoC_PlayerLoginResult" , "(AC->Switch)Player login result , Account = %s Result = %d" , PG->Account , PG->Result );
    if( PG->Result == EM_LoginResul_OK )
    {
//        _AccountSet.insert( PG->Account );
        _EventListener->OnPlayerReg( PG->UserID , PG->Account , PG->ChargType , PG->Point , PG->LastLoginIP , PG->LastLoginTime , PG->PlayTimeQuota );
    }
    else
    {
        _EventListener->OnPlayerRegFailed( PG->UserID , PG->Account , PG->Result );
    }
}
void AccountCliClass::_AccountNet_StoC_PlayerLogoutResult   ( int NetID , AccountNet_SysNull* Data , int Size )
{
    AccountNet_StoC_PlayerLogoutResult* PG = (AccountNet_StoC_PlayerLogoutResult*)Data;
	Print( LV2 , "_AccountNet_StoC_PlayerLogoutResult" , "##(AC->Switch)Player logout result , Account = %s Result = %d" , PG->Account , PG->Result );

    if( PG->Result == EM_GSrvLogoutResult_OK )
    {
        _EventListener->OnPlayerLogout( PG->UserID , PG->Account );
    }
    else
    {
        _EventListener->OnPlayerLogoutFaild( PG->UserID , PG->Account , PG->Result );
    }
}
//----------------------------------------------------------------------------------------------
/*
//回傳兌換密碼商品的結果
void AccountCliClass::_AccountNet_StoC_CheckExchangeItemResult	( int NetID , AccountNet_SysNull* Data , int Size )
{
	AccountNet_StoC_CheckExchangeItemResult* PG = (AccountNet_StoC_CheckExchangeItemResult*)Data;
	_EventListener->ItemExchangeResult( PG->GSrvNetID , PG->UserAccount , PG->RoleGUID , PG->Type , PG->Item , PG->Money , PG->Money_Account  );	
}
*/
//----------------------------------------------------------------------------------------------
//檢查此帳號是否還存在此Server
void AccountCliClass::_AccountNet_StoC_CheckAccount( int NetID , AccountNet_SysNull* Data , int Size )
{
    AccountNet_StoC_CheckAccount* PG = (AccountNet_StoC_CheckAccount*)Data;
    
    AccountNet_CtoS_CheckAccountResult Send;
    memcpy( Send.Account , PG->Account , sizeof(Send.Account) );
    Send.Result = _EventListener->CheckAccount( Send.Account );

    _BNet->Send( NetID , sizeof(Send) , &Send  );
}

//----------------------------------------------------------------------------------------------
//帳號登入錯誤時, Account回傳回來的額外資料
void AccountCliClass::_AccountNet_StoC_PlayerLoginFailedData( int NetID , AccountNet_SysNull* Data , int Size )
{
	AccountNet_StoC_PlayerLoginFailedData* PG = (AccountNet_StoC_PlayerLoginFailedData*)Data;
	 _EventListener->OnPlayerRegFailedData( PG->UserID , PG->Account , PG->Result, &(PG->Data), sizeof(PG->Data) );
}

//----------------------------------------------------------------------------------------------
//如果斷線重新聯結Account Agent
int AccountCliClass::_ReConnectAC( SchedularInfo* Obj , void* InputClass )
{
    AccountCliClass* Class = (AccountCliClass*)InputClass;
    Print( LV2 , "_ReConnectAC" , "reconnect Account Agent" );

//    sockaddr_in		addr;
//    MAKE_IP_ADDRESS( (sockaddr_in*)&addr , Class->_IP , Class->_Port );
//    Class->_Net->Connect( &addr , Class->_Listener );	
    int IPNum = ConvertIP( Class->_IP );
    Class->_BNet->Connect( IPNum , Class->_Port , Class->_BListener );	
    return 0;
}
//----------------------------------------------------------------------------------------------
AccountCliClass::AccountCliClass( )
{
	_Schedular  = NEW( FunctionSchedularClass );
    InitPGCallBack( );
    _BListener   = NEW LBAccount( this );
    //_Net        = CreateObjNet( );
    _BNet        = CreateNetBridge( );
    _NetID      = -1;
}
//----------------------------------------------------------------------------------------------
AccountCliClass::~AccountCliClass( )
{
    DeleteNetBridge( _BNet );
    delete _BListener;
    delete _Schedular;
}
//----------------------------------------------------------------------------------------------
void	AccountCliClass::Connect( char* IP , int Port , int GameID , int GameGroupID , char* Name )
{
    _GameID = GameID;
	_GameGroupID = GameGroupID;
    _Port = Port;
    memcpy( _GameName , Name , sizeof(_GameName ) );
    memcpy( _IP , IP , sizeof( _IP ) );

    //sockaddr_in		addr;
    //MAKE_IP_ADDRESS( (sockaddr_in*)&addr , _IP , _Port );
    int IPNum = ConvertIP( IP );
    _BNet->Connect( IPNum , Port , _BListener );	

}
//----------------------------------------------------------------------------------------------
//定時處理
void	AccountCliClass::Process( )
{
    _BNet->Flush();
	Schedular()->Process();
}
//----------------------------------------------------------------------------------------------
void    AccountCliClass::EventListener( AccountCliListener* Event )
{
    _EventListener = Event;
}
//-----------------------------------------------------------------------------------
//AccountCliBase 
//-----------------------------------------------------------------------------------

void AccountCliClass::Logout( )
{
    _BNet->Close( _NetID );
}
//----------------------------------------------------------------------------------------------
void AccountCliClass::PlayerReg(  char* Account , char* Password , int IPNum, int UserID, const char* pszMacAddress, bool HashedPassword )
{

    if( Account[0] == 0 )
    {
        Print( LV2 , "PlayerReg" , "account error , Account = ''"  );
        _EventListener->OnPlayerRegFailed( UserID , Account , EM_LoginResul_NotFind );
        return;
    }
	
	Print( LV2 , "PlayerReg" , "Account=%s , UserID=%d" , Account , UserID );


    AccountNet_CtoS_PlayerLogin Send;
    MyStrcpy( Send.Account,		Account,		sizeof(Send.Account ) );
    MyStrcpy( Send.Password,	Password,		sizeof(Send.Password ) );
	MyStrcpy( Send.MacAddress,	pszMacAddress,	sizeof(Send.MacAddress ) );

    Send.IPnum = IPNum;
    Send.UserID = UserID;
	Send.HashedPassword = HashedPassword;

    _BNet->Send( _NetID , sizeof(Send) , &Send );
}
//----------------------------------------------------------------------------------------------
void AccountCliClass::PlayerLogout( char* Account , int UserID )
{
	Print( LV2 , "PlayerLogout" , "Account=%s , UserID=%d" , Account , UserID );

    AccountNet_CtoS_PlayerLogout Send;
    Send.UserID = UserID;
    MyStrcpy( Send.Account , Account , sizeof(Send.Account ) );

    _BNet->Send( _NetID , sizeof(Send) , &Send  );
}

void AccountCliClass::FreezeAccount( char* Account , int FreezeType )
{
	AccountNet_CtoS_FreezeAccount Send;
	Send.FreezeType = FreezeType;
	MyStrcpy( Send.UserAccount , Account , sizeof(Send.UserAccount ) );
	_BNet->Send( _NetID , sizeof(Send) , &Send  );
}
//----------------------------------------------------------------------------------------------
FunctionSchedularClass* 	AccountCliClass::Schedular()
{
	return _Schedular;
}
//----------------------------------------------------------------------------------------------
/*
//兌換物品要求
void	AccountCliClass::ItemExchangeRequest( int GSrvNetID , int RoleGUID , char* UserAccount 
											 , char* UserRoleName , char* UserIP , char* ItemKey , int ItemSerial )
{
	AccountNet_CtoS_CheckExchangeItemRequest Send;

	MyStrcpy( Send.UserAccount , UserAccount , sizeof(Send.UserAccount) );
	MyStrcpy( Send.UserIP , UserIP , sizeof(Send.UserIP) );
	MyStrcpy( Send.UserRoleName , UserRoleName , sizeof(Send.UserRoleName) );
	MyStrcpy( Send.ItemKey , ItemKey , sizeof(Send.ItemKey) );

	Send.GSrvNetID = GSrvNetID;
	Send.RoleGUID = RoleGUID;
	Send.WorldID = _GameID;
	Send.ItemSerial = ItemSerial;


	_BNet->Send( _NetID , sizeof(Send) , &Send  );
}
*/
//----------------------------------------------------------------------------------------------
