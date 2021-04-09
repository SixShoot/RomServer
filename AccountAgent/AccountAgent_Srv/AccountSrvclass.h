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
//�B�z�PServer���q�T�ǿ骺Class
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
	LBServers*			_BListener;	//��ƦC�|Class
	IAccountPlugin*		_AccountPlugin;

	char                _NowTime[32];
	//-------------------------------------------------------------------------------------
	map< string , PlayerAccountInfo* >          _AccountMap;
	vector< GameSrvInfo >                       _GSrvList;
	set< int >                                  _GameIDSet;

	RecycleBin< PlayerAccountInfo >             _PlayerAccountInfoBin;
	RecycleBin< pair< AccountClass* , PlayerAccountInfo*  > >   _PairBin;

	int					_MaxPasswordErrorCount;		//�K�X�̦h���\��J���~����, �W�L���Ʒ|�Ȯɭᵲ�b��
	int					_FreezeAccountTime;			//Server�ᵲ�b�����ɶ�����

    void	_OnConnected( DWORD NetID );
	void	_OnDisconnected( DWORD NetID );
	bool	_OnPacket( DWORD NetID , DWORD Size , void* Data );
	void	_OnConnectFailed( int NetID );

	//-----------------------------------------------------------------------------------
	// �ʥ]Call Back �B�z
	//-----------------------------------------------------------------------------------
    void _AccountNet_CtoS_GSrvLogin(  int NetID , AccountNet_SysNull* PG , int Size );
    void _AccountNet_CtoS_PlayerLogin( int NetID , AccountNet_SysNull* PG , int Size );
    void _AccountNet_CtoS_PlayerLogout(  int NetID , AccountNet_SysNull* PG , int Size );
    void _AccountNet_CtoS_CheckAccountResult(  int NetID , AccountNet_SysNull* PG , int Size );
	void _AccountNet_CtoS_FreezeAccount(  int NetID , AccountNet_SysNull* PG , int Size );

    static NetPGCallBack   _PGCallBack[ EM_AccountNet_Packet_Count ];
    void _PGxUnKnoew( int NetID , AccountNet_SysNull* PG , int Size ){};

   
	//�Ψӳ�����ǥΪ�
	static  int	_AccountValue( char* Account );
public:
	
	AccountClass( IniFileClass* Ini );
	virtual ~AccountClass( );

	void	Host( char* IP,int Port );

	//�w�ɳB�z
	void	Process( );

    //�ˬd�Ҧ����n�J�b��
    void    CheckAllAccount();

	virtual void AccountLoginResult( void* UserObj , string Account , string LastLoginIP , string LastLoginTime , AccountResultENUM Result );
	virtual void AccountLogoutResult( string Account , AccountResultENUM Result );
	virtual void SetData( string Account , int Type , int Value );
	//-----------------------------------------------------------------------------------
	// ��l�� Client �P Server �� CallBack�禡
	//-----------------------------------------------------------------------------------
	static  void	InitPGCallBack( );

};

#endif //__ACCOUNTSRVCLASS_H__
