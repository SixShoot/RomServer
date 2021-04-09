#pragma once
#include <boost/bind.hpp>
#include "DebugLog/errormessage.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <functional>
#include "NetBridge/NetBridge.h"
#include "smallobj/SmallObj.h"

using namespace std;
class TelnetClass;

class LBTelnet: public CEventObj
{
	TelnetClass	*_Parent;
public:
	LBTelnet( TelnetClass* value ) { _Parent = value; };
	
	virtual bool		OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData );
	virtual void		OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode );
	virtual void		OnConnect		( DWORD dwNetID );
	virtual	void		OnDisconnect	( DWORD dwNetID );
	virtual CEventObj*	OnAccept		( DWORD dwNetID );
};


#define OnConnectFunctionBase			boost::function<void ( int id ) >
#define OnDisconnectFunctionBase		boost::function<void ( int id ) >
#define OnPacketFunctionBase			boost::function<void ( int id , int size , void* data) >


class TelnetClass
{
	friend	LBTelnet;	

	CNetBridge*					_NetTelnet;				            //主要是用連接Switch
	LBTelnet*					_NetListener;

	vector< PacketBuffClass*>	_PacketBuffList;

	vector< OnConnectFunctionBase >		_OnConnectFunc;
	vector< OnDisconnectFunctionBase >	_OnDisconnectFunc;
	vector< OnPacketFunctionBase >		_OnPacketFunc;	

	//-----------------------------------------------------------------------------------
	void	_OnConnected       ( int NetID );
	void	_OnDisconnected    ( int NetID );
	bool	_OnPacket          ( DWORD dwNetID, DWORD Size , PVOID Data );

public:

	TelnetClass( );
	virtual ~TelnetClass();

	void	Host( const char* IP , int Port );

	//定時處理
	bool	Process( );

	//登出
	bool    Close( int NetID );

	//送資料
	void	Send( int NetID , int Size , void* Data );

	bool    RegOnConnectFunction	( OnConnectFunctionBase );
	bool    RegOnDisConnectFunction	( OnDisconnectFunctionBase );
	bool    RegOnPacketFunction		( int PGid , OnPacketFunctionBase );

};
