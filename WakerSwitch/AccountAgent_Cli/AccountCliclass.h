#ifndef	__ACCOUNTCLICLASS_H__
#define __ACCOUNTCLICLASS_H__
#pragma warning (disable:4786)

#include <set>
#include <vector>
#include <map>
#include <string>
/*
#include "ObjNet\ObjNet.h"
#include "ObjNet\lsockutil.h"
#include "ObjNet\lDebug.h"
*/
#include "NetBridge/NetBridge.h"

#include "functionschedular/functionschedular.h"

#include "AccountPackage.h"

using namespace std;
using namespace NetWakerAccountPackage;

class AccountCliClass;
//---------------------------------------------------------------------------------
typedef void  (AccountCliClass::*AC_NetPGCallBack) ( int NetID , AccountNet_SysNull* PG , int Size );
//---------------------------------------------------------------------------------
//�B�z�PServer���q�T�ǿ骺Class
/*
class LAccount: public INewConnectionListener, public INetEventListener 
{ 
    AccountCliClass	*_Parent;
public:
    LAccount( AccountCliClass* value) { _Parent = value; };
	INetEventListener* LSAPI OnNewConnection( int ){return this;}
	void LSAPI OnConnected( int id );
	void LSAPI OnDisconnected( int id );
	void LSAPI OnPeerShutdown( int id );
	bool LSAPI OnPacket( int id, HBUFFER hIncomingData );
	void LSAPI OnConnectFailed( int id );
};*/
class LBAccount: public CEventObj
{
    AccountCliClass	*_Parent;
public:
    LBAccount( AccountCliClass* value ) { _Parent = value; };
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
class AccountCliClass  
{
	friend	LBAccount;	
//	IObjNet*			    _Net;		//�D�n�O�Ψӵ�Game Server�s���Ϊ�
    CNetBridge*			    _BNet;		//�D�n�O�Ψӵ�Game Server�s���Ϊ�
	LBAccount*			    _BListener;	//��ƦC�|Class
    AccountCliListener*     _EventListener;//�ϥΪ̳]�w��
    FunctionSchedularClass* _Schedular;

    char    _IP[32];
    int     _Port;
    int     _GameID;
	int		_GameGroupID;
    char    _GameName[ _DEF_MAX_GSRVNAME_SIZE_ ];
    int     _NetID;
	//-------------------------------------------------------------------------------------
	void	_OnConnected( DWORD id );
	void	_OnDisconnected( DWORD id );
	bool	_OnPacket( DWORD id, DWORD Size , void* Data );
	void	_OnConnectFailed( int id );
	//-----------------------------------------------------------------------------------
	// �ʥ]Call Back �B�z
	//-----------------------------------------------------------------------------------
    void    _AccountNet_StoC_GSrvLoginResult			( int NetID , AccountNet_SysNull* Data , int Size );
    void    _AccountNet_StoC_PlayerLoginResult			( int NetID , AccountNet_SysNull* Data , int Size );
    void    _AccountNet_StoC_PlayerLogoutResult			( int NetID , AccountNet_SysNull* Data , int Size );
    void    _AccountNet_StoC_CheckAccount				( int NetID , AccountNet_SysNull* Data , int Size );
//	void    _AccountNet_StoC_CheckExchangeItemResult	( int NetID , AccountNet_SysNull* Data , int Size );
	void	_AccountNet_StoC_PlayerLoginFailedData		( int NetID , AccountNet_SysNull* Data , int Size );
    //-----------------------------------------------------------------------------------
    //�p�G�_�u���s�p��Account Agent
    static  int _ReConnectAC( SchedularInfo* Obj , void* InputClass );
    //-----------------------------------------------------------------------------------
    AC_NetPGCallBack   _PGCallBack[ EM_AccountNet_Packet_Count ];
    void    _PGxUnKnow( int NetID , AccountNet_SysNull* PG , int Size ){};
public:
	AccountCliClass();
	virtual ~AccountCliClass();
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    void    EventListener( AccountCliListener* );
	void	Connect( char* IP , int Port , int GameID , int GameGroupID , char* Name );

	//�w�ɳB�z
	void	Process();
    void    Logout( );
    void    PlayerReg(  char* Account , char* Password , int IPnum , int UserID, const char* pszMacAddress, bool HashedPassword);
	void    PlayerLogout( char* Account , int UserID );	
	void	FreezeAccount( char* Account , int FreezeType );
	//-----------------------------------------------------------------------------------
	//�I�����~�n�D
//	void	ItemExchangeRequest( int GSrvNetID , int RoleGUID , char* UserAccount , char* UserRoleName , char* UserIP , char* ItemKey , int ItemSerial );


	FunctionSchedularClass* 	Schedular();
	//-----------------------------------------------------------------------------------
	// ��l�� Client �P Server �� CallBack�禡
	//-----------------------------------------------------------------------------------
	void	InitPGCallBack();

	//-----------------------------------------------------------------------------------
	//Ū��
	//-----------------------------------------------------------------------------------
	int		GameID()		{ return _GameID; }
	int		GameGroupID()	{ return _GameGroupID;}
};

//-----------------------------------------------------------------------------------------------
#endif //__ACCOUNTSRVCLASS_H__
