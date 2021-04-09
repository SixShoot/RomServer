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

    CNetBridge*			        _NetBSwitch;		    //�D�n�O�Ψӵ�Switch�s���Ϊ�

    LBSwitch*		            _SwitchBListener;	    //��ƦC�|Class

	PacketInfoManage			_PGInfoManage;
	NetStatusStruct				_BaseInfo;

    void	_OnSwitchConnected     ( int NetID );
    void	_OnSwitchDisconnected  ( int NetID );
    bool	_OnSwitchPacket        ( DWORD NetID , DWORD Size , void* Data );
    void	_OnSwitchConnectFailed ( int NetID );

    bool    _ExitFlag;                              //�����X��

    NetPGCallBack		    _PGCallBack[ EM_SysNet_Packet_Count ];
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // �ʥ]Call Back �B�z
    //-----------------------------------------------------------------------------------
	static  void _SysNet_Switch_Gateway_Register			( GatewayClass* Obj , int NetID , PGSysNULL* PG , int Size );
	static  void _SysNet_X_DataTransmitToWorldGSrv			( GatewayClass* Obj , int NetID , PGSysNULL* PG , int Size );
	static  void _SysNet_X_DataTransmitToWorldGSrv_GSrvID	( GatewayClass* Obj , int NetID , PGSysNULL* PG , int Size );

	//-----------------------------------------------------------------------------------
	static	void _PGxUnKnow( GatewayClass* Obj , int NetID , PGSysNULL* PG , int Size );

	//-----------------------------------------------------------------------------------
	//�ˬdSwitch���U�O�_����
	//�p�G�S���� �R�����s�u
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

    //�w�ɳB�z
    bool	Process( );
	void	PacketLogProc();
    //-----------------------------------------------------------------------------------
    //�e��ƨ� Switch
    bool    SendToSwitch( int NetID , int Size , void* Data );
	bool    SendToSwitch_ByWorldID( int WorldID , int Size , void* Data );

    //-----------------------------------------------------------------------------------
    // ��l�� Client �P Server �� CallBack�禡
    //-----------------------------------------------------------------------------------
    void	InitPGCallBack();


};

