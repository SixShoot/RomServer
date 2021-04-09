#pragma once

#include <deque>
#include "NetBridge/NetBridge.h"

#include "NetWaker/NetWakerPackage.h"
#include "NetWaker/NetWakerDefine.h"
#include "functionschedular/functionschedular.h"
#include "SmallObj/SmallObj.h"
#include "LogManage.h"
class GatewayClass;
using namespace std;
using namespace NetWakerPackage;
//---------------------------------------------------------------------------------
class LBSwitch: public CEventObj
{
    GatewayClass	*_Parent;
public:
    LBSwitch( GatewayClass* value ) { _Parent = value; };
    virtual bool		OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData );
    virtual void		OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode );
    virtual void		OnConnect		( DWORD dwNetID );
    virtual	void		OnDisconnect	( DWORD dwNetID );
    virtual CEventObj*	OnAccept		( DWORD dwNetID );
};
//---------------------------------------------------------------------------------
struct Gateway_SwitchInfoStruct
{
	int NetID;
	int WorldID;
};
//---------------------------------------------------------------------------------
class GatewayClass
{

typedef void  (*NetPGCallBack) (GatewayClass* Obj,int	NetID,PGSysNULL* PG,int Size);

	friend	LBSwitch;	

	vector< SwitchInfoStruct >	_SwitchMap;				//index = WorldID
	Map_NetIDtoIDClass			_NetIDtoWorldID;
//	map< int , Gateway_SwitchInfoStruct > _SwitchInfoMap;

    CNetBridge*			        _NetBSwitch;		    //主要是用來給Switch連結用的

    LBSwitch*		            _SwitchBListener;	    //資料列舉Class

	PacketInfoManage			_PGInfoManage;
	NetStatusStruct				_BaseInfo;

    void	_OnSwitchConnected     ( int NetID );
    void	_OnSwitchDisconnected  ( int NetID );
    bool	_OnSwitchPacket        ( DWORD NetID , DWORD Size , void* Data );
    void	_OnSwitchConnectFailed ( int NetID );

    bool    _ExitFlag;                              //結束旗標

    NetPGCallBack		    _PGCallBack[ EM_SysNet_Packet_Count ];
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // 封包Call Back 處理
    //-----------------------------------------------------------------------------------
	static  void _SysNet_Switch_Gateway_Register			( GatewayClass* Obj , int NetID , PGSysNULL* PG , int Size );
	static  void _SysNet_X_DataTransmitToWorldGSrv			( GatewayClass* Obj , int NetID , PGSysNULL* PG , int Size );
	static  void _SysNet_X_DataTransmitToWorldGSrv_GSrvID	( GatewayClass* Obj , int NetID , PGSysNULL* PG , int Size );

	//-----------------------------------------------------------------------------------
	static	void _PGxUnKnow( GatewayClass* Obj , int NetID , PGSysNULL* PG , int Size );

	//-----------------------------------------------------------------------------------
	//檢查Switch註冊是否完成
	//如果沒完成 刪除此連線
	static  int _CheckSwitchConnectAndReg( SchedularInfo* Obj , void* InputClass );

	static  void _Output_NetMsg	( int lv, const char* szOut );

	void	_AddPGTop10Log( PacketInfoManage& info  );
	void	_ResetBaseInfo();
public:

    GatewayClass();
    virtual ~GatewayClass();

    void	HostSwitch( char* IP,int Port );

	//-----------------------------------------------------------------------------------

	CNetBridge*		GetSwitch()			{ return _NetBSwitch;	}

    //定時處理
    bool	Process( );
	void	PacketLogProc();
    //-----------------------------------------------------------------------------------
    //送資料到 Switch
    bool    SendToSwitch( int NetID , int Size , void* Data );
	bool    SendToSwitch_ByWorldID( int WorldID , int Size , void* Data );

    //-----------------------------------------------------------------------------------
    // 初始化 Client 與 Server 的 CallBack函式
    //-----------------------------------------------------------------------------------
    void	InitPGCallBack();


};

