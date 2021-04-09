//********************************************************************************************
//			
//  OverView:	- Gateway Server 主要處理的部分
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
//後門處理 連到其他的服務處理
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
	LBClients*			_CliBListener;	//資料列舉Class
	LBServers*			_SrvBListener;	//資料列舉Class
	IAccountPlugin*		_Account;

	CNetBridge*			_ForwardNet;				            //主要是用連接Switch
	LProxyClient*		_ForwardListener;


	//Client 端樣板檔案資料
	char				_CliTempateFileData[0x10000];
	int					_CliTempateFileSize;
	string				_CliTempateFile;

	int					_CountryFilterType;			// 0 不允許國家　1 允許國家
	int					_MaxPasswordErrorCount;		//密碼最多允許輸入錯誤次數, 超過次數會暫時凍結帳號
	int					_FreezeAccountTime;			//Server凍結帳號的時間長度

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
	//讀取個Server人數
	static bool _SQLCmdRoleCountEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdRoleCountEventResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	
	//讀取ip表資料 與 不可以連線的國家
	static bool _SQLCmdReadIpInfoEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdReadIpInfoEventResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdResetPasswordEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdResetPasswordEventResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );


	void	_LoadIpInfo( );
public:
	
	ServerListClass( IniFileClass* Ini );
	virtual ~ServerListClass( );

	void	Host( char* IP , char* IP_Outside  , int CliPort , int SrvPort );

	//定時處理
	void	Process( );

	//通知各Server 人數
	void	SendWorldRoleCount( int CliNetID , const char* Account_ID );

	//讀取ServerList Client端 設定資料
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
	// 初始化 Client 與 Server 的 CallBack函式
	//-----------------------------------------------------------------------------------
	static  void	InitPGCallBack( );

public:
	int		_ServerListMode;
};

#endif //__ACCOUNTSRVCLASS_H__
