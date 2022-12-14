#ifndef __SWITCHCLASS_H__
#define __SWITCHCLASS_H__

#include <deque>
#include "NetBridge/NetBridge.h"

//#include "ObjNet\ObjNet.h"
//#include "ObjNet\lsockutil.h"
//#include "ObjNet\lDebug.h"

#include "NetWaker/NetWakerPackage.h"
#include "NetWaker/NetWakerDefine.h"
#include "functionschedular/functionschedular.h"
#include "SmallObj/SmallObj.h"
#include "AccountAgent_Cli/AccountCliClass.h"
#include "ServerList_SCli/ServerListSCliclass.h"
#include "ControllerClient/ControllerClient.h"

class SwitchClass;
using namespace std;
using namespace NetWakerPackage;
//---------------------------------------------------------------------------------
//處理與Proxy間通訊傳輸的Class
class LBProxy: public CEventObj
{
    SwitchClass	*_Parent;
public:
    LBProxy( SwitchClass* value ) { _Parent = value; };
    virtual bool		OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData );
    virtual void		OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode );
    virtual void		OnConnect		( DWORD dwNetID );
    virtual	void		OnDisconnect	( DWORD dwNetID );
    virtual CEventObj*	OnAccept		( DWORD dwNetID );
};
//---------------------------------------------------------------------------------
//處理與Game Server(GSrv)間通訊傳輸的Class
class LBGSrv: public CEventObj
{
    SwitchClass	*_Parent;
public:
    LBGSrv( SwitchClass* value ) { _Parent = value; };
    virtual bool		OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData );
    virtual void		OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode );
    virtual void		OnConnect		( DWORD dwNetID );
    virtual	void		OnDisconnect	( DWORD dwNetID );
    virtual CEventObj*	OnAccept		( DWORD dwNetID );
};
//---------------------------------------------------------------------------------
//處理與 Client 間通訊傳輸的Class
class LBCli: public CEventObj
{
    SwitchClass	*_Parent;
public:
    LBCli( SwitchClass* value ) { _Parent = value; };
    virtual bool		OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData );
    virtual void		OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode );
    virtual void		OnConnect		( DWORD dwNetID );
    virtual	void		OnDisconnect	( DWORD dwNetID );
    virtual CEventObj*	OnAccept		( DWORD dwNetID );
};
//---------------------------------------------------------------------------------
//處理與Gateway之間的傳輸
class LBGateway: public CEventObj
{
	SwitchClass	*_Parent;
public:
	LBGateway( SwitchClass* value ) { _Parent = value; };
	virtual bool		OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData );
	virtual void		OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode );
	virtual void		OnConnect		( DWORD dwNetID );
	virtual	void		OnDisconnect	( DWORD dwNetID );
	virtual CEventObj*	OnAccept		( DWORD dwNetID );
};
//---------------------------------------------------------------------------------
class LAC : public AccountCliListener
{
    SwitchClass	*_Parent;
public:
    LAC( SwitchClass* Value) {_Parent = Value;  }

    virtual void OnLogin				( );
    virtual void OnLoginFailed			( GSrvLoginResultEnum );
    virtual void OnLogout				( );
    virtual void OnPlayerReg			( int UserID , char* Account , ChargTypeEnum , int Point ,  char* LastLoginIP , char* LastLoginTime , int PlayTimeQuota );
    virtual void OnPlayerRegFailed		( int UserID , char* Account , LoginResultEnum );
	virtual void OnPlayerRegFailedData	( int UserID , char* Account , LoginResultEnum, void* Data, unsigned long DataSize );
    virtual void OnPlayerLogout			( int UserID , char* Account );
    virtual void OnPlayerLogoutFaild	( int UserID , char* Account , LogoutResultEnum );
    virtual bool CheckAccount			( char* Account  );
};
//---------------------------------------------------------------------------------
class LSrvList : public ServerListListener
{
	SwitchClass	*_Parent;
public:
	LSrvList( SwitchClass* Value) {_Parent = Value;  }
	virtual void ClientRelogin( char* Account );
};
//---------------------------------------------------------------------------------
struct IPStruct
{
	string	IPStr;
	int		Prot;
};

//---------------------------------------------------------------------------------
class SwitchClass
{

typedef void  (*NetPGCallBack) (SwitchClass* Obj,int	NetID,PGSysNULL* PG,int Size);

	friend	LBProxy;	
    friend	LBGSrv;
    friend	LBCli;
    friend  LAC;
	friend  LSrvList;
	friend	LBGateway;

	string	_WorldName;
	string	_ClientHostIP;
	int		_ClientPort;

	string  _GatewayIP;
	int		_GatewayPort;

	map< string , IPStruct >	_RedirectHost;
	ApnicFileReaderClass		_ApnicFileReader;

    map< string , CliInfoStruct*> _LoginAccount;

    MyVector< ProxyInfoStruct >		_ProxyList;
    MyVector< GSrvInfoStruct >		_GSrvList;
    MyVector< CliInfoStruct >		_CliList;
	set< int >						_Gateway_WorldIDSet;
	vector< Map_NetIDtoIDClass >	_NetIDtoGSrvIDList;
	

    CNetBridge*			        _NetBProxy;			    //主要是用來給Proxy連結用的
    CNetBridge*			        _NetBGSrv;			    //主要是用來給Game Server連結用的
    CNetBridge*			        _NetBCli;			    //主要是用來給Game Server連結用的
	CNetBridge*			        _NetGateway;		    //主要是用來給Game Server連結用的

    LBProxy*		            _ProxyBListener;	    //資料列舉Class
    LBGSrv*			            _GSrvBListener;	        //資料列舉Class
    LBCli*			            _CliBListener;	        //資料列舉Class
	LBGateway*					_GatewayListener;

    deque< int >                _UnUsedClientIDList;
    Map_NetIDtoIDClass          _CliNetToIDMap;


    AccountCliClass             _AcCli;
    LAC*                        _AcListener;

	ServerListSCliClass			_SrvList;
	LSrvList*					_SrvListListener;

	bool						_SendServerListInfo;
	bool						_SwitchReady;
	
	int							_GatewayNetID;


    void    _OnACLogin            ( );
    void    _OnACLoginFailed      ( GSrvLoginResultEnum );
    void    _OnACLogout           ( );
    void    _OnACPlayerReg        ( int UserID , char* Account , ChargTypeEnum , int Point , char* LastLoginIP , char* LastLoginTime , int PlayTimeQuota );
    void    _OnACPlayerRegFailed  ( int UserID , char* Account , LoginResultEnum );
	void	_OnACPlayerRegFailedData(  int UserID , char* Account , LoginResultEnum, void* Data, unsigned long DataSize );
    void    _OnACPlayerLogout     ( int UserID , char* Account );
    void    _OnACPlayerLogoutFaild( int UserID , char* Account , LogoutResultEnum );
    bool    _CheckAccount         ( char* Account );


    void	_OnProxyConnected     ( int NetID );
    void	_OnProxyDisconnected  ( int NetID );
    bool	_OnProxyPacket        ( DWORD NetID , DWORD Size , void* Data );
    void	_OnProxyConnectFailed ( int NetID );

    void	_OnGSrvConnected      ( int NetID );
    void	_OnGSrvDisconnected   ( int NetID );
    bool	_OnGSrvPacket         ( DWORD NetID , DWORD Size , void* Data );
    void	_OnGSrvConnectFailed  ( int NetID );

    void	_OnCliConnected       ( int NetID );
    void	_OnCliDisconnected    ( int NetID );
    bool	_OnCliPacket          ( DWORD NetID , DWORD Size , void* Data );
    void	_OnCliConnectFailed   ( int NetID );

	void	_OnGatewayConnected    ( int NetID );
	void	_OnGatewayDisconnected ( int NetID );
	bool	_OnGatewayPacket       ( DWORD NetID , DWORD Size , void* Data );
	void	_OnGatewayConnectFailed( int NetID );


	void	_ClientRelogin		( char* Account );

    bool    _ExitFlag;                              //結束旗標

    vector<PGBaseInfo>	    _PGInfo;				//封包管理的資訊
    NetPGCallBack		    _PGCallBack[ EM_SysNet_Packet_Count ];
    //-----------------------------------------------------------------------------------
    // Member Function
    //-----------------------------------------------------------------------------------
    CliInfoStruct*  _NewCli( );
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // 封包Call Back 處理
    //-----------------------------------------------------------------------------------
    static	void _PGxUnKnow( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

    //-----------------------------------------------------------------------------------
    //(Proxy)
    static  void _PGxProxyConnect( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

    //GSrv 聯結到 Proxy 成?
    static  void _PGxGSrvConnectToProxy( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

    //GSrv 與 Proxy 斷線
    static  void _PGxProxy_GSrvProxyDisconnect( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

    //Cli 聯結到 Proxy 成?
    static  void _PGxCliConnectToProxy( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

    //Cli 與 Proxy 斷線
    static  void _PGxProxy_CliProxyDisconnect( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

    //定時通知 Cli 還在 Proxy 內
    static  void _SysNet_Proxy_Switch_NotifyCliIDLive( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

	//確定Proxy等待GSrv聯線
	static  void _SysNet_Proxy_Switch_GSrvConnectPrepareOK( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size );

	//確定Proxy等待Cli聯線
	static  void _SysNet_Proxy_Switch_CliConnectPrepareOK( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size );
    //-----------------------------------------------------------------------------------
    //( GSrv )
    static  void _PGxGSrvConnect( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

    //GSrv 與 Proxy 斷線
    static  void _PGxGSrv_GSrvProxyDisconnect( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

    //設定某GSrv　Active or Inactive
    static  void _PGxSetGSrvActiveType( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

    //GSrv 轉送到 GSrv 的封包
	static  void _PGxDataGSrvToGSrv( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );
	static  void _PGxDataGSrvToGSrv_Zip( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

	//GSrv 要求 與 Cli聯線
	static  void _PGxConnectCliAndGSrv( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

	//GSrv 要求 與 Cli斷線
	static  void _PGxDisconnectCliAndGSrv( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

	//GSrv 要求 某 GSrv Logout
	static  void _PGxSrvLogoutRequest( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

	//GSrv 通知Server的 loading狀況 
	static  void _PGxServerListInfo( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size );

	//GSrv 要求轉送資料到其他世界
	static  void _PGxDataTransmitToWorldGSrv( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size  );
	//GSrv 要求轉送資料到其他世界
	static  void _PGxDataTransmitToWorldGSrv_GSrvID( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size  );
	//註冊GSrv的NetID
	static  void _PGx_RegisterGSrvID( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size  );
	//帳號冷凍要求
	static  void _PGx_FreezeAccount( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size  );
	
	//-----------------------------------------------------------------------------------
    //( Client )
    //Client 登入資訊
    static  void _PGxClientLoginInfo( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size );
	//-----------------------------------------------------------------------------------
	//( Gateway )
	//通知註冊是否成?
	static void _PGxGatewayRegisterResult	( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size );
	//有世界連線
	static void _PGxGatewayWorldConnect		( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size );
	//通知有世界斷線
	static void _PGxGatewayWorldDisconnect	( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size );
    //-----------------------------------------------------------------------------------
    //檢查proxy 註冊是否完成
    //如果沒完成 刪除此連線
	static  int _CheckProxyConnectAndReg( SchedularInfo* Obj , void* InputClass );

    //檢查GSrv 註冊是否完成
    //如果沒完成 刪除此連線
    static  int _CheckGSrvConnectAndReg( SchedularInfo*	Obj , void* InputClass );

    //檢查Client 註冊是否完成
    //如果沒完成 刪除此連線
    static  int _CheckCliConnectAndReg( SchedularInfo*	Obj , void* InputClass );

    //定時檢查 Client 在 Proxy 內的狀況
    static  int _CheckCliConnectInProxy( SchedularInfo*	Obj , void* InputClass );

	//設定一小段時間後Cli斷線
	static  int _CliLogoutSchedualar( SchedularInfo*	Obj , void* InputClass );

	//回報目前連線狀況
	static  int _ReportNetInfoSchedualar( SchedularInfo*	Obj , void* InputClass );

	//等待他人登出 然後登入
	static  int _WaitLogin( SchedularInfo*	Obj , void* InputClass );

	static  void _Output_Server_NetMsg	( int lv, const char* szOut );
	static  void _Output_Client_NetMsg	( int lv, const char* szOut );

	//重新連到Gateway
	static  int _ReconnectGateway( SchedularInfo*	Obj , void* InputClass );

	//-----------------------------------------------------------------------------------
public:

    SwitchClass();
    virtual ~SwitchClass();

    void	HostProxy( char* IP,int Port );
    void	HostGSrv( char* IP,int Port );
    void	HostCli( char* IP,int Port );
	void	ConnectGateway( string IP,int Port );
    void	ConnectAc( char* IP,int Port , int GameID , int GameGroupID , char* GameName );
	void	ConnectServerList( char* IP,int Port );

	//-----------------------------------------------------------------------------------

	CNetBridge*		GetProxy()			{ return _NetBProxy;	}
	CNetBridge*		GetServer()			{ return _NetBGSrv;		}
	CNetBridge*		GetClient()			{ return _NetBCli;		}
	CNetBridge*		GetGateway()		{ return _NetGateway;	}


    //定時處理
    bool	Process( );
	void	CheckSystemStatus();
    //-----------------------------------------------------------------------------------
    //送資料到 Proxy
    bool    SendToProxy( int ProxyID , int Size , void* Data );
    bool    SendToGSrv( int GSrvID , int Size , void* Data );
    void    SendToAllProxy( int Size , void* Data );
    void    SendToAllGSrv( int Size , void* Data );
    bool    SendToCli( int NetID , int Size , void* Data );

    void    CliLogout( int CliID, const char* pszReason );

	int		SetCliLogout_2Sec( int CliID );

	FunctionSchedularClass*     Schedular(){ return _AcCli.Schedular(); };
    //-----------------------------------------------------------------------------------
    // 初始化 Client 與 Server 的 CallBack函式
    //-----------------------------------------------------------------------------------
    void	InitPGCallBack();

	void	SetUpdateListMode( bool bSet )	{ _SendServerListInfo = bSet; }
	bool	GetUpdateListMode()				{ return _SendServerListInfo; } 

	void	SetSwitchStatus( int iMode )		{ _ServerStatus = iMode; }
	int		GetSwitchStatus()					{ return _ServerStatus; }


	void	Set_Client_SendBuffLimit( bool bFlag, int iSize );
	void	Set_Client_NoRespond( bool bFlag );
	void	Set_Server_NoRespond( bool bFlag );

	//////////////////////////////////////////////////////////////////////////
	//重新導向IP
	void	SetRedirectHost	( string Country , string Host , int Port );//{ _RedirectHost[Country] = Host; };
	bool	LoadApnicFile	( string FileName ){ return _ApnicFileReader.LoadFile( FileName.c_str() ); };

	static int		m_WorldID;
	static bool		m_NeedPassword;

	static bool		m_bCheck_Client_SendBuffLimit;
	static bool		m_bCheck_Client_NoRespond;

	static bool		m_bCheck_Server_SendBuffLimit;
	static bool		m_bCheck_Server_NoRespond;

	int		_ServerStatus;			//0 為正常, 1 為維修, 2 為隱藏

};

#endif //__SWITCHCLASS_H__