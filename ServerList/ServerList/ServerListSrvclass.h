//********************************************************************************************
//			
//  OverView:	- Gateway Server �D�n�B�z������
//  
//  Author: jacklo@softstar.com.tw      JackLo  2003.8.11
//
//********************************************************************* ***********************

#ifndef	__ACCOUNTSRVCLASS_H__
#define __ACCOUNTSRVCLASS_H__
#pragma warning (disable:4786)


#include <vector>
#include <map>
#include <string>

#include "NetBridge/NetBridge.h"
#include "ServerListPackage.h"
#include "ServerListSrvDef.h"
#include "../AccountManage/AccountManage.h"
//#include "../GamaAccountManage/GamaAccountManage.h"
//#include "../K2AccountManage/K2AccountManage.h"
//#include "../VNAccountManage/VNAccountManage.h"
//#include "../KRAccountManage/KRAccountManage.h"
//#include "../AeriaAccountManager/AeriaAccountManager.h"
//#include "../GameWakerAccountManage/GameWakerAccountManage.h"
//#include "../GFAccountManage/GFAccountManage.h"
#include "ControllerClient/ControllerClient.h"

using namespace std;
using namespace NetWakerServerListPackage;

class ServerListClass;
//---------------------------------------------------------------------------------
typedef void  (ServerListClass::*NetPGCallBack) ( int NetID , ServerListNet_SysNull* PG , int Size );
//---------------------------------------------------------------------------------

class LBServers: public CEventObj 
{
    ServerListClass	*_Parent;
public:
    LBServers( ServerListClass* value ) { _Parent = value; };
    virtual bool		OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData );
    virtual void		OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode );
    virtual void		OnConnect		( DWORD dwNetID );
    virtual	void		OnDisconnect	( DWORD dwNetID );
    virtual CEventObj*	OnAccept		( DWORD dwNetID );
};

class LBClients: public CEventObj
{
	ServerListClass	*_Parent;
public:
	LBClients( ServerListClass* value ) { _Parent = value; };
	virtual bool		OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData );
	virtual void		OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode );
	virtual void		OnConnect		( DWORD dwNetID );
	virtual	void		OnDisconnect	( DWORD dwNetID );
	virtual CEventObj*	OnAccept		( DWORD dwNetID );
};
//---------------------------------------------------------------------------------
//����B�z �s���L���A�ȳB�z
class LProxyClient: public CEventObj
{
	ServerListClass	*_Parent;
public:
	LProxyClient( ServerListClass* value ) { _Parent = value; };
	virtual bool		OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData );
	virtual void		OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode );
	virtual void		OnConnect		( DWORD dwNetID );
	virtual	void		OnDisconnect	( DWORD dwNetID );
	virtual CEventObj*	OnAccept		( DWORD dwNetID );
};

//---------------------------------------------------------------------------------
//#define _Sys_Max_CallbackCount_ 256
//---------------------------------------------------------------------------------
class ServerListClass  : public IAccountPluginNotify
{
	friend	LBServers;	
	friend  LBClients;
	friend  LProxyClient;
    //-------------------------------------------------------------------------------------
    CNetBridge*         _CliBNet;
	CNetBridge*         _SrvBNet;
	LBClients*			_CliBListener;	//��ƦC�|Class
	LBServers*			_SrvBListener;	//��ƦC�|Class
	IAccountPlugin*		_Account;

	CNetBridge*			_ForwardNet;				            //�D�n�O�γs��Switch
	LProxyClient*		_ForwardListener;


	//Client �ݼ˪O�ɮ׸��
	char				_CliTempateFileData[0x10000];
	int					_CliTempateFileSize;
	string				_CliTempateFile;

	int					_CountryFilterType;			// 0 �����\��a�@1 ���\��a
	int					_MaxPasswordErrorCount;		//�K�X�̦h���\��J���~����, �W�L���Ʒ|�Ȯɭᵲ�b��
	int					_FreezeAccountTime;			//Server�ᵲ�b�����ɶ�����

	vector< ServerInfoStruct >		_SrvInfoList;
	vector< ClientInfoStruct >		_CliInfoList;

	static NetPGCallBack   _SrvPGCallBack[ EM_ServerListNet_Packet_Count ];
	static NetPGCallBack   _CliPGCallBack[ EM_ServerListNet_Packet_Count ];
	void _PGxUnKnoew( int NetID , ServerListNet_SysNull* PG , int Size );

	ApnicFileReaderClass		_ApnicFileReader;
	
	//-------------------------------------------------------------------------------------
	void	_OnSrvConnected( DWORD NetID );
	void	_OnSrvDisconnected( DWORD NetID );
	bool	_OnSrvPacket( DWORD NetID , DWORD Size , void* Data );
	void	_OnSrvConnectFailed( int NetID );
	//-------------------------------------------------------------------------------------
	void	_OnCliConnected( DWORD NetID );
	void	_OnCliDisconnected( DWORD NetID );
	bool	_OnCliPacket( DWORD NetID , DWORD Size , void* Data );
	void	_OnCliConnectFailed( int NetID );
	//-----------------------------------------------------------------------------------
	void	_OnProxyConnected( DWORD NetID );
	void	_OnProxyDisconnected( DWORD NetID );
	bool	_OnProxyPacket( DWORD NetID , DWORD Size , void* Data );
	void	_OnProxyConnectFailed( int NetID );
	//-----------------------------------------------------------------------------------
	//Ū����Server�H��
	static bool _SQLCmdRoleCountEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdRoleCountEventResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	
	//Ū��ip���� �P ���i�H�s�u����a
	static bool _SQLCmdReadIpInfoEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdReadIpInfoEventResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdResetPasswordEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdResetPasswordEventResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );


	void	_LoadIpInfo( );
public:
	
	ServerListClass( IniFileClass* Ini );
	virtual ~ServerListClass( );

	void	Host( char* IP , char* IP_Outside  , int CliPort , int SrvPort );

	//�w�ɳB�z
	void	Process( );

	//�q���UServer �H��
	void	SendWorldRoleCount( int CliNetID , const char* Account_ID );

	//Ū��ServerList Client�� �]�w���
	void	ReadClientTemplateFile();

	void _ServerListNet_SCtoS_RegServerInfo			(  int NetID , ServerListNet_SysNull* PG , int Size );
	void _ServerListNet_CtoS_ServerListRequest		(  int NetID , ServerListNet_SysNull* PG , int Size );
	void _ServerListNet_CtoS_ForwardTo				(  int NetID , ServerListNet_SysNull* PG , int Size );
	void _ServerListNet_CtoS_SetVivoxLicenseTime	(  int NetID , ServerListNet_SysNull* PG , int Size );
	void _ServerListNet_CtoS_ResetPasswordResult	(  int NetID , ServerListNet_SysNull* PG , int Size );
	void _ServerListNet_CtoS_ClientSystemInfo	(  int NetID , ServerListNet_SysNull* PG , int Size );
	
	//////////////////////////////////////////////////////////////////////////
	//IAccountPluginNotify

	virtual void ResetPassword					( int NetID );
	virtual void CheckAccountResult				( int NetID , int WorldID , string Account , AccountResultENUM Result );
	virtual void AccountLoginResult				( string Account , AccountResultENUM Result );
	virtual void AccountLogoutResult			( string Account , AccountResultENUM Result );
	virtual void SetData						( int NetID , int Type , int Value );
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------------------
	// ��l�� Client �P Server �� CallBack�禡
	//-----------------------------------------------------------------------------------
	static  void	InitPGCallBack( );

public:
	int		_ServerListMode;
};

#endif //__ACCOUNTSRVCLASS_H__
