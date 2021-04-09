#ifndef __PROXYCLASS_H__
#define __PROXYCLASS_H__
/************************************
NetWaker Proxy Class　
Revision By: hsiang
Revised on 2004/8/23 下午 04:31:19
Comments: 
************************************/
/*
#include "ObjNet\ObjNet.h"
#include "ObjNet\lsockutil.h"
#include "ObjNet\lDebug.h"*/
#include "NetBridge/NetBridge.h"

#include "NetWaker\NetWakerPackage.h"
#include "NetWaker\NetWakerDefine.h"
#include "functionschedular\functionschedular.h"
#include "SmallObj/SmallObj.h"
#include "ControllerClient/ControllerClient.h"

class ProxyClass;
using namespace std;
using namespace NetWakerPackage;

#define DF_MAX_PACKETID	10000

//---------------------------------------------------------------------------------
typedef void  (*NetPGCallBack) (ProxyClass* Obj,int	NetID,PGSysNULL* PG,int Size);
//---------------------------------------------------------------------------------
//處理與Switch間通訊傳輸的Class
/*
class LSwitch: public INewConnectionListener, public INetEventListener 
{ 
public:
    ProxyClass	*Parent;
    INetEventListener* LSAPI OnNewConnection( LNETID ){return this;}
    void LSAPI OnConnected      ( LNETID id );
    void LSAPI OnDisconnected   ( LNETID id );
    void LSAPI OnPeerShutdown   ( LNETID id );
    bool LSAPI OnPacket         ( LNETID id, HBUFFER hIncomingData );
    void LSAPI OnConnectFailed  ( int id );
};
*/

class LBSwitch: public CEventObj
{
    ProxyClass	*_Parent;
public:
    LBSwitch( ProxyClass* value ) { _Parent = value; };
    virtual bool		OnRecv			( unsigned long dwNetID, unsigned long dwPackSize, PVOID pPacketData );
    virtual void		OnConnectFailed	( unsigned long dwNetID, unsigned long dwFailedCode );
    virtual void		OnConnect		( unsigned long dwNetID );
    virtual	void		OnDisconnect	( unsigned long dwNetID );
    virtual CEventObj*	OnAccept		( unsigned long dwNetID );
};
//---------------------------------------------------------------------------------
/*
//處理與Game Server間通訊傳輸的Class
class LGSrv: public INewConnectionListener, public INetEventListener 
{ 
public:
    ProxyClass	*Parent;
    INetEventListener* LSAPI OnNewConnection( LNETID ){return this;}
    void LSAPI OnConnected      ( LNETID id );
    void LSAPI OnDisconnected   ( LNETID id );
    void LSAPI OnPeerShutdown   ( LNETID id );
    bool LSAPI OnPacket         ( LNETID id, HBUFFER hIncomingData );
    void LSAPI OnConnectFailed  ( int id );
};
*/
class LBGSrv: public CEventObj
{
    ProxyClass	*_Parent;
public:
    LBGSrv( ProxyClass* value ) { _Parent = value; };
    virtual bool		OnRecv			( unsigned long dwNetID, unsigned long dwPackSize, PVOID pPacketData );
    virtual void		OnConnectFailed	( unsigned long dwNetID, unsigned long dwFailedCode );
    virtual void		OnConnect		( unsigned long dwNetID );
    virtual	void		OnDisconnect	( unsigned long dwNetID );
    virtual CEventObj*	OnAccept		( unsigned long dwNetID );
};
//---------------------------------------------------------------------------------
//處理與 Client 間通訊傳輸的Class
/*
class LCli : public INewConnectionListener, public INetEventListener 
{ 
public:
    ProxyClass	*Parent;
    INetEventListener* LSAPI OnNewConnection( LNETID ){return this;}
    void LSAPI OnConnected      ( LNETID id );
    void LSAPI OnDisconnected   ( LNETID id );
    void LSAPI OnPeerShutdown   ( LNETID id );
    bool LSAPI OnPacket         ( LNETID id, HBUFFER hIncomingData );
    void LSAPI OnConnectFailed  ( int id );
};
*/
class LBCli: public CEventObj
{
    ProxyClass	*_Parent;
public:
    LBCli( ProxyClass* value ) { _Parent = value; };
    virtual bool		OnRecv			( unsigned long dwNetID, unsigned long dwPackSize, PVOID pPacketData );
    virtual void		OnConnectFailed	( unsigned long dwNetID, unsigned long dwFailedCode );
    virtual void		OnConnect		( unsigned long dwNetID );
    virtual	void		OnDisconnect	( unsigned long dwNetID );
    virtual CEventObj*	OnAccept		( unsigned long dwNetID );
};
//---------------------------------------------------------------------------------

class ProxyClass
{
public:
	friend	LBSwitch;	
	friend	LBGSrv;	
    friend	LBCli;	

    FunctionSchedularClass*      _Schedular;

    int             _ProxyID;           //proxy 的編號
    //sockaddr_in	    _SwitchAddr;
    int             _SwitchIPNum;
    int             _SwitchIPPort;


    int             _NetSwitchID;

    //IObjNet*	    _NetSwitch;			//主要是用連接Switch
    CNetBridge*	    _NetBSwitch;			//主要是用連接Switch
    CNetBridge*	    _NetBGSrv;   		//主要是用連接Game Server
    CNetBridge*	    _NetBCli;   		    //主要是用連接Game Server

    LBSwitch*	    _SwitchBListener;    //資料列舉Class
    LBGSrv*  	    _GSrvBListener;      //資料列舉Class
    LBCli* 		    _CliBListener;       //資料列舉Class

    bool            _ExitFlag;          //結束旗標
   
    string          _GSrvHostIP;
    int             _GSrvHostPort;

    string          _CliHostIP;
    int             _CliHostPort;

	bool			_PacketDelay;		

    MyVector<GSrvInfoStruct>    _GSrvList;
    Map_NetIDtoIDClass          _GSrvNetToIDMap;

    MyVector<CliInfoStruct>     _CliList;
    Map_NetIDtoIDClass          _CliNetToIDMap;

    void	_OnSwitchConnected     ( DWORD NetID );
    void	_OnSwitchDisconnected  ( DWORD NetID );
    bool	_OnSwitchPacket        ( DWORD NetID, DWORD PackSize, void* PacketData );
    void	_OnSwitchConnectFailed ( DWORD NetID );

    void	_OnGSrvConnected       ( DWORD NetID );
    void	_OnGSrvDisconnected    ( DWORD NetID );
    bool	_OnGSrvPacket          ( DWORD NetID, DWORD PackSize, void* PacketData );
    void	_OnGSrvConnectFailed   ( DWORD NetID );

    void	_OnCliConnected        ( DWORD NetID );
    void	_OnCliDisconnected     ( DWORD NetID );
    bool	_OnCliPacket           ( DWORD NetID, DWORD PackSize, void* PacketData );
    void	_OnCliConnectFailed    ( DWORD NetID );

   
    vector<PGBaseInfo>	        _PGInfo;			//封包管理的資訊
    NetPGCallBack		        _PGCallBack[ EM_SysNet_Packet_Count ];
    //-----------------------------------------------------------------------------------
    // 封包Call Back 處理
    //-----------------------------------------------------------------------------------
    static	void _PGxUnKnoew( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);
    //-----------------------------------------------------------------------------------
    //(Switch)
    // 連結到Switch
    static	void _PGxConnectNotify( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //通知有Game Server 要連結
    static	void _PGxGSrvConnectInfo( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //Switch 要求 Proxy 與某 GSrv 斷線
    static	void _PGxGSrvDisconnectNodify( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //設定某GSrv　Active or Inactive
    static	void _PGxSetGSrvActiveType( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //通知有 Cli 要登入
    static	void _PGxCliLoginNotify( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);
    
    //-----------------------------------------------------------------------------------
    //(GSrv)
    //GSrv 連結
    static	void _PGxGSrvConnect( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //要求 某 GSrv 與 Cli 成立連線
    static	void _PGxConnectGSrvAndCli( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //要求 某 GSrv 與 Cli 斷線
    static	void _PGxDisconnectGSrvAndCli( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //要求 某 Cli 完全登出
    static	void _PGxCliLogout( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //要求資料轉送到 Cli
    static  void _PGxDataGSrvToCli( ProxyClass* Obj , int NetID , PGSysNULL* PG,int Size );

	//通知Cli有帳號重複登入此角色
    static  void _NotifyCliIDRelogin( ProxyClass* Obj , int NetID , PGSysNULL* PG,int Size );

	static  void _PGxOnPingLog( ProxyClass* Obj , int NetID , PGSysNULL* PG,int Size );

    //-----------------------------------------------------------------------------------
    //(Cli)
    //Client  送聯結資訊
    static	void _PGxGCliConnect( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //要求資料轉送到 GSrv
    static  void _PGxDataCliToGSrv( ProxyClass* Obj , int NetID , PGSysNULL* PG,int Size );
    //-----------------------------------------------------------------------------------
	//client 端要求斷線
	static  void _PGxOnNetClose( ProxyClass* Obj , int NetID , PGSysNULL* PG,int Size );
	//-----------------------------------------------------------------------------------

    //重新連結 Switch Server
    static	int  _ReConnectSwitch( SchedularInfo* Obj , void* InputClass );

    //-----------------------------------------------------------------------------------
    //定時送出所有連線的Client給 GSrv
    static	int  _CheckClientConnectData( SchedularInfo* Obj , void* InputClass );

    //檢查GSrv 註冊是否完成
    //如果沒完成 刪除此連線
    static  int  _CheckGSrvConnectAndReg( SchedularInfo*	Obj , void* InputClass );

	//檢查Cli 註冊是否完成
	//如果沒完成 刪除此連線
	static  int  _CheckCliConnectAndReg( SchedularInfo*	Obj , void* InputClass );
    //-----------------------------------------------------------------------------------

	//回報目前連線狀況
	static  int _ReportNetInfoSchedualar( SchedularInfo*	Obj , void* InputClass );
	static  int _ReportPacketInfoSchedualar( SchedularInfo*	Obj , void* InputClass );

	static  void _Output_Server_NetMsg	( int lv, const char* szOut );
	static  void _Output_Client_NetMsg	( int lv, const char* szOut );


public:

    ProxyClass();
    virtual ~ProxyClass();

    void	ConnectSwitch( char* IP,int Port );
    void    SetGSrvHost( char* IP,int Port );
    void    SetCliHost( char* IP,int Port );
    void    HostGSrv( char* IP,int Port );
    void    HostCli( char* IP,int Port );

    //定時處理
    bool	Process( );
	void	CheckSystemStatus();
    //-----------------------------------------------------------------------------------
    bool    SendToSwitch( int Size , void* Data );                  //送資料到 Switch
    bool    SendToGSrv( int NetID , int  Size , void* Data );       //送資料到 GSrv
	bool    SendToCli( int NetID , int  Size , void* Data );        
	void	SendToAllCli( int Size , void* Data );

    bool    SendToGSrv_GSrvID( int GSrvID , int Size , void* Data );
    bool    SendToCli_CliID( int CliID , int Size , void* Data );
    //-----------------------------------------------------------------------------------
    // Connect : Cli and Game Server
    bool    Connect_GSrv_Cli( int GSrvID , int CliID );
    // DisConnect : Cli and Game Server
    bool    Disconnect_GSrv_Cli( int GSrvID , int CliID );
    //-----------------------------------------------------------------------------------
    // 初始化 Client 與 Server 的 CallBack函式
    //-----------------------------------------------------------------------------------
    void	InitPGCallBack();

	void	Set_Client_NoRespond( bool bFlag );
	void	Set_Server_NoRespond( bool bFlag );

	CNetBridge*	    NetBSwitch(){ return _NetBSwitch; };	//主要是用連接Switch
    CNetBridge*	    NetBGSrv(){ return _NetBGSrv; };   		//主要是用連接Game Server
    CNetBridge*	    NetBCli(){ return _NetBCli; };   		//主要是用連接Game Server

	static int		m_iProxyDisconnectCount[ EM_ClientNetCloseType_End ];

	static int		m_CtoS_PacketID[ DF_MAX_PACKETID ];
	static int		m_CtoS_Bandwidth[ DF_MAX_PACKETID ];

	static int		m_StoC_PacketID[ DF_MAX_PACKETID ];
	static int		m_StoC_Bandwidth[ DF_MAX_PACKETID ];


};

#endif //__PROXYCLASS_H__