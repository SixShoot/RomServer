#pragma once
#pragma warning (disable:4786)

#include <set>
#include <vector>
#include <map>
#include <string>

#include "NetBridge/NetBridge.h"

#include "functionschedular/functionschedular.h"

#include "ServerListPackage.h"

using namespace std;
using namespace NetWakerServerListPackage;

class ServerListSCliClass;
//---------------------------------------------------------------------------------
typedef void  (ServerListSCliClass::*SrvList_NetPGCallBack) ( int NetID , ServerListNet_SysNull* PG , int Size );
//---------------------------------------------------------------------------------
//處理與Server間通訊傳輸的Class
class LBServerList: public CEventObj
{
    ServerListSCliClass	*_Parent;
public:
    LBServerList( ServerListSCliClass* value ) { _Parent = value; };
    virtual bool		OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData );
    virtual void		OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode );
    virtual void		OnConnect		( DWORD dwNetID );
    virtual	void		OnDisconnect	( DWORD dwNetID );
    virtual CEventObj*	OnAccept		( DWORD dwNetID );
};
//---------------------------------------------------------------------------------
//#define _Sys_Max_CallbackCount_ 256
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
class ServerListSCliClass  
{
	friend	LBServerList;	

    CNetBridge*			    _BNet;		//主要是用來給Game Server連結用的
	LBServerList*			_BListener;	//資料列舉Class
    
    FunctionSchedularClass* _Schedular;

	bool					_IsReady;

	string					_IP;
	int						_Port;
	int						_NetID;
	ServerListListener*		_Listener;
	//-------------------------------------------------------------------------------------
	void	_OnConnected( DWORD id );
	void	_OnDisconnected( DWORD id );
	bool	_OnPacket( DWORD id, DWORD Size , void* Data );
	void	_OnConnectFailed( int id );
	//-----------------------------------------------------------------------------------
	// 封包Call Back 處理
	//-----------------------------------------------------------------------------------
	void    _ServerListNet_SCtoS_CliReloginNotify      ( int NetID , ServerListNet_SysNull* PG , int Size );
    
    //-----------------------------------------------------------------------------------
    //如果斷線重新聯結Account Agent
    static  int _ReConnectServerList( SchedularInfo* Obj , void* InputClass );

    //-----------------------------------------------------------------------------------
    SrvList_NetPGCallBack   _PGCallBack[ EM_ServerListNet_Packet_Count ];
    void    _PGxUnKnow( int NetID , ServerListNet_SysNull* PG , int Size ){};
public:
	ServerListSCliClass();
	virtual ~ServerListSCliClass();

	void	EventListener( ServerListListener*	Listener ){ _Listener = Listener; };
    //-----------------------------------------------------------------------------------
	void	Connect( char* IP , int Port );

	//把目前的世界狀況送到ServerList
	void	SendServerListInfo( int GameGroup ,	int GameID , const char* ServerName , const char* IP_DNS , int Port , int MaxPlayerCount , int ParallelZoneCount , int PlayerCount[100] , int ServerStatus , float TotalPlayerRate );

	//定時處理
	void	Process();

	FunctionSchedularClass* 	Schedular();
	//-----------------------------------------------------------------------------------
	// 初始化 Client 與 Server 的 CallBack函式
	//-----------------------------------------------------------------------------------
	void	InitPGCallBack();

};

//-----------------------------------------------------------------------------------------------

