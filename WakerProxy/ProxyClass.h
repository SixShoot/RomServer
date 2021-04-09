#ifndef __PROXYCLASS_H__
#define __PROXYCLASS_H__
/************************************
NetWaker Proxy Class�@
Revision By: hsiang
Revised on 2004/8/23 �U�� 04:31:19
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
//�B�z�PSwitch���q�T�ǿ骺Class
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
//�B�z�PGame Server���q�T�ǿ骺Class
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
//�B�z�P Client ���q�T�ǿ骺Class
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

    int             _ProxyID;           //proxy ���s��
    //sockaddr_in	    _SwitchAddr;
    int             _SwitchIPNum;
    int             _SwitchIPPort;


    int             _NetSwitchID;

    //IObjNet*	    _NetSwitch;			//�D�n�O�γs��Switch
    CNetBridge*	    _NetBSwitch;			//�D�n�O�γs��Switch
    CNetBridge*	    _NetBGSrv;   		//�D�n�O�γs��Game Server
    CNetBridge*	    _NetBCli;   		    //�D�n�O�γs��Game Server

    LBSwitch*	    _SwitchBListener;    //��ƦC�|Class
    LBGSrv*  	    _GSrvBListener;      //��ƦC�|Class
    LBCli* 		    _CliBListener;       //��ƦC�|Class

    bool            _ExitFlag;          //�����X��
   
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

   
    vector<PGBaseInfo>	        _PGInfo;			//�ʥ]�޲z����T
    NetPGCallBack		        _PGCallBack[ EM_SysNet_Packet_Count ];
    //-----------------------------------------------------------------------------------
    // �ʥ]Call Back �B�z
    //-----------------------------------------------------------------------------------
    static	void _PGxUnKnoew( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);
    //-----------------------------------------------------------------------------------
    //(Switch)
    // �s����Switch
    static	void _PGxConnectNotify( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //�q����Game Server �n�s��
    static	void _PGxGSrvConnectInfo( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //Switch �n�D Proxy �P�Y GSrv �_�u
    static	void _PGxGSrvDisconnectNodify( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //�]�w�YGSrv�@Active or Inactive
    static	void _PGxSetGSrvActiveType( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //�q���� Cli �n�n�J
    static	void _PGxCliLoginNotify( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);
    
    //-----------------------------------------------------------------------------------
    //(GSrv)
    //GSrv �s��
    static	void _PGxGSrvConnect( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //�n�D �Y GSrv �P Cli ���߳s�u
    static	void _PGxConnectGSrvAndCli( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //�n�D �Y GSrv �P Cli �_�u
    static	void _PGxDisconnectGSrvAndCli( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //�n�D �Y Cli �����n�X
    static	void _PGxCliLogout( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //�n�D�����e�� Cli
    static  void _PGxDataGSrvToCli( ProxyClass* Obj , int NetID , PGSysNULL* PG,int Size );

	//�q��Cli���b�����Ƶn�J������
    static  void _NotifyCliIDRelogin( ProxyClass* Obj , int NetID , PGSysNULL* PG,int Size );

	static  void _PGxOnPingLog( ProxyClass* Obj , int NetID , PGSysNULL* PG,int Size );

    //-----------------------------------------------------------------------------------
    //(Cli)
    //Client  �e�p����T
    static	void _PGxGCliConnect( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size);

    //�n�D�����e�� GSrv
    static  void _PGxDataCliToGSrv( ProxyClass* Obj , int NetID , PGSysNULL* PG,int Size );
    //-----------------------------------------------------------------------------------
	//client �ݭn�D�_�u
	static  void _PGxOnNetClose( ProxyClass* Obj , int NetID , PGSysNULL* PG,int Size );
	//-----------------------------------------------------------------------------------

    //���s�s�� Switch Server
    static	int  _ReConnectSwitch( SchedularInfo* Obj , void* InputClass );

    //-----------------------------------------------------------------------------------
    //�w�ɰe�X�Ҧ��s�u��Client�� GSrv
    static	int  _CheckClientConnectData( SchedularInfo* Obj , void* InputClass );

    //�ˬdGSrv ���U�O�_����
    //�p�G�S���� �R�����s�u
    static  int  _CheckGSrvConnectAndReg( SchedularInfo*	Obj , void* InputClass );

	//�ˬdCli ���U�O�_����
	//�p�G�S���� �R�����s�u
	static  int  _CheckCliConnectAndReg( SchedularInfo*	Obj , void* InputClass );
    //-----------------------------------------------------------------------------------

	//�^���ثe�s�u���p
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

    //�w�ɳB�z
    bool	Process( );
	void	CheckSystemStatus();
    //-----------------------------------------------------------------------------------
    bool    SendToSwitch( int Size , void* Data );                  //�e��ƨ� Switch
    bool    SendToGSrv( int NetID , int  Size , void* Data );       //�e��ƨ� GSrv
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
    // ��l�� Client �P Server �� CallBack�禡
    //-----------------------------------------------------------------------------------
    void	InitPGCallBack();

	void	Set_Client_NoRespond( bool bFlag );
	void	Set_Server_NoRespond( bool bFlag );

	CNetBridge*	    NetBSwitch(){ return _NetBSwitch; };	//�D�n�O�γs��Switch
    CNetBridge*	    NetBGSrv(){ return _NetBGSrv; };   		//�D�n�O�γs��Game Server
    CNetBridge*	    NetBCli(){ return _NetBCli; };   		//�D�n�O�γs��Game Server

	static int		m_iProxyDisconnectCount[ EM_ClientNetCloseType_End ];

	static int		m_CtoS_PacketID[ DF_MAX_PACKETID ];
	static int		m_CtoS_Bandwidth[ DF_MAX_PACKETID ];

	static int		m_StoC_PacketID[ DF_MAX_PACKETID ];
	static int		m_StoC_Bandwidth[ DF_MAX_PACKETID ];


};

#endif //__PROXYCLASS_H__