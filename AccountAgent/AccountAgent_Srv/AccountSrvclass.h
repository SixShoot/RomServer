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
#include "AccountPackage.h"
#include "AccountSrvDef.h"
#include "recyclebin/recyclebin.h"
#include "ControllerClient/ControllerClient.h"

#include "../IAccountPlugin/IAccountPlugin.h"
#include "../IAccountPlugin/AccountBaseDef.h"


using namespace std;
using namespace NetWakerAccountPackage;

class AccountClass;
//---------------------------------------------------------------------------------
typedef void  (AccountClass::*NetPGCallBack) ( int NetID , AccountNet_SysNull* PG , int Size );
//---------------------------------------------------------------------------------
//處理與Server間通訊傳輸的Class
class LBServers: public CEventObj
{
    AccountClass	*_Parent;
public:
    LBServers( AccountClass* value ) { _Parent = value; };
    virtual bool		OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData );
    virtual void		OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode );
    virtual void		OnConnect		( DWORD dwNetID );
    virtual	void		OnDisconnect	( DWORD dwNetID );
    virtual CEventObj*	OnAccept		( DWORD dwNetID );
};
//---------------------------------------------------------------------------------
//#define _Sys_Max_CallbackCount_ 256
//---------------------------------------------------------------------------------
class AccountClass : public IAccountPluginNotify
{
	friend	LBServers;	
    //-------------------------------------------------------------------------------------
    CNetBridge*         _BNet;
	LBServers*			_BListener;	//資料列舉Class
	IAccountPlugin*		_AccountPlugin;

	char                _NowTime[32];
	//-------------------------------------------------------------------------------------
	map< string , PlayerAccountInfo* >          _AccountMap;
	vector< GameSrvInfo >                       _GSrvList;
	set< int >                                  _GameIDSet;

	RecycleBin< PlayerAccountInfo >             _PlayerAccountInfoBin;
	RecycleBin< pair< AccountClass* , PlayerAccountInfo*  > >   _PairBin;

	int					_MaxPasswordErrorCount;		//密碼最多允許輸入錯誤次數, 超過次數會暫時凍結帳號
	int					_FreezeAccountTime;			//Server凍結帳號的時間長度

    void	_OnConnected( DWORD NetID );
	void	_OnDisconnected( DWORD NetID );
	bool	_OnPacket( DWORD NetID , DWORD Size , void* Data );
	void	_OnConnectFailed( int NetID );

	//-----------------------------------------------------------------------------------
	// 封包Call Back 處理
	//-----------------------------------------------------------------------------------
    void _AccountNet_CtoS_GSrvLogin(  int NetID , AccountNet_SysNull* PG , int Size );
    void _AccountNet_CtoS_PlayerLogin( int NetID , AccountNet_SysNull* PG , int Size );
    void _AccountNet_CtoS_PlayerLogout(  int NetID , AccountNet_SysNull* PG , int Size );
    void _AccountNet_CtoS_CheckAccountResult(  int NetID , AccountNet_SysNull* PG , int Size );
	void _AccountNet_CtoS_FreezeAccount(  int NetID , AccountNet_SysNull* PG , int Size );

    static NetPGCallBack   _PGCallBack[ EM_AccountNet_Packet_Count ];
    void _PGxUnKnoew( int NetID , AccountNet_SysNull* PG , int Size ){};

   
	//用來都執行序用的
	static  int	_AccountValue( char* Account );
public:
	
	AccountClass( IniFileClass* Ini );
	virtual ~AccountClass( );

	void	Host( char* IP,int Port );

	//定時處理
	void	Process( );

    //檢查所有的登入帳號
    void    CheckAllAccount();

	virtual void AccountLoginResult( void* UserObj , string Account , string LastLoginIP , string LastLoginTime , AccountResultENUM Result );
	virtual void AccountLogoutResult( string Account , AccountResultENUM Result );
	virtual void SetData( string Account , int Type , int Value );
	//-----------------------------------------------------------------------------------
	// 初始化 Client 與 Server 的 CallBack函式
	//-----------------------------------------------------------------------------------
	static  void	InitPGCallBack( );

};

#endif //__ACCOUNTSRVCLASS_H__
