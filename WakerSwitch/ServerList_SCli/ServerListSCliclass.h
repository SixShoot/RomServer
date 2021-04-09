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
//�B�z�PServer���q�T�ǿ骺Class
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

    CNetBridge*			    _BNet;		//�D�n�O�Ψӵ�Game Server�s���Ϊ�
	LBServerList*			_BListener;	//��ƦC�|Class
    
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
	// �ʥ]Call Back �B�z
	//-----------------------------------------------------------------------------------
	void    _ServerListNet_SCtoS_CliReloginNotify      ( int NetID , ServerListNet_SysNull* PG , int Size );
    
    //-----------------------------------------------------------------------------------
    //�p�G�_�u���s�p��Account Agent
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

	//��ثe���@�ɪ��p�e��ServerList
	void	SendServerListInfo( int GameGroup ,	int GameID , const char* ServerName , const char* IP_DNS , int Port , int MaxPlayerCount , int ParallelZoneCount , int PlayerCount[100] , int ServerStatus , float TotalPlayerRate );

	//�w�ɳB�z
	void	Process();

	FunctionSchedularClass* 	Schedular();
	//-----------------------------------------------------------------------------------
	// ��l�� Client �P Server �� CallBack�禡
	//-----------------------------------------------------------------------------------
	void	InitPGCallBack();

};

//-----------------------------------------------------------------------------------------------

