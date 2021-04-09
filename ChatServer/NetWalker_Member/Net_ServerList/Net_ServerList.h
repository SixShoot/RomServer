//	Server List

//	a. �H�ɫO���̷s�ثe�u�W Server ���

//	�i��i�H��i���a��
//	a.�b�޲z�ηj�M Server ���a��, �ثe�O�ϥ� vector �Ӧs, �i��H��i�H�令�ϥ� multihash_map
//	  ���n��ҹL��i��....�u�ʷj���� Hash ....

#pragma once

//#include "../../mainproc/Global.h"

#include "PG/PG_ServerList.h"

#include <set>

using namespace std;

#define DF_SERVERNAMESIZE		21

class Global;

struct	ServerData
{
	ServerData()
	{
		m_bServerRdy = false;
	};

	EM_SERVER_TYPE		m_ServerType;				// Server����
	DWORD				m_dwServerLocalID;			// �إ� Server �ҫ��w�� Server �N��
	DWORD				m_dwServerID;				// ��ӳ� Server �P �����إ߳s�u��, �Ψӳq�T���N��
	char				m_szServerNamed[ DF_SERVERNAMESIZE ];
	bool				m_bServerRdy;
};

class CNet_ServerList 
{
public:
	//CNet_ServerList( Global* pG , EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
    CNet_ServerList( );
	virtual ~CNet_ServerList(void);
	
	static EM_SERVER_TYPE			m_ServerType;
	static DWORD					m_dwServerLocalID;
	static CNet_ServerList*			m_pServerList;

    //-------------------------------------------------------------------
	bool					m_bRegister;			// �O�_�w�g�M Master ���U ( �q�� Master, �i���w���U�L�F, ���F������`�Ұ����ܼ� )
													// ���ܼƷ|�ǵ� Master �� Server List, �K�Q Master SL �M�w�n���n�q���䥦 SL
	int						m_iSrvID;
//	CNet_ServerList*		m_pThis;
    Global*                 m_pGlobal;

	set< int >				m_setEventRegAccount;
//	static GSrvNetWaker*			m_pNet;



	//bool				_Init_SameLocalID( GSrvNetWaker* pNet, EM_SERVER_TYPE ServerType ) { _Init( pNet, ServerType, DF_REQUEST_LOCALID ); }
	//bool				_Init( GSrvNetWaker* pNet, EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );

//	bool				_Release();

	//map< DWORD , ServerData* > m_mapServerData;

	//static void				_OnLogin ( string Info );						// loging to switch server
	//static void				_OnLogout ();									// logout
	//static void				_OnLogFailed ( SYSLogFailedENUM );

	void				OnEvent_ServerConnect			( int iServerID , string SrvName );
	void				OnEvent_ServerDisconnect		( int iServerID );
	
	void				OnPacket_ResiterServerData		( int iServerID, int iSize, PVOID pData );
	void				OnPacket_TransmitServerData		( int iServerID, int iSize, PVOID pData );
		
		// Callback function for packet PG_ServerList_S2SL_ServerData
	void				OnPacket_RequestServerData		( int iServerID, int iSize, PVOID pData );
	void				OnPacket_RegSrvDataFailed		( int iServerID, int iSize, PVOID pData );

	void				OnPacket_LocalID				( int iServerID, int iSize, PVOID pData );
	void				OnPacket_ServerReady			( int iServerID, int iSize, PVOID pData );

	void				OnPacket_Shutdown				( int iServerID, int iSize, PVOID pData );
	void				OnPacket_RegisterAccountEvent	( int iServerID, int iSize, PVOID pData );
	
		// PG_ServerList_SL2S_RequestServerData

	//static void				_OnCliConnect	( int ID , string Account );	// Clinet connect to servers
	//static void				_OnCliDisconnect( int ID );

	//static void				_PGTest_CliToGSrv	( int Sockid , int Size , void* Data );
	//static void				_PGTest_GSrvToGSrv	( int Sockid , int Size , void* Data );
	//-------------------------------------------------------------------	


public:
    void    Init( Global* pG , EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );

	virtual	void	OnServer_RequestServerData			( int iServerID ) =0;
		void			RegisterServerData				( int iServerID, EM_SERVER_TYPE emServerType, int iSrvLocalID, bool bReg );
		
		// �o�e���S�w Server ServerData

	virtual	void	OnServer_Connect					( int iServerID, string SrvName ) =0;
	virtual	void	OnServer_Disconnect					( int iServerID ) = 0;

	virtual	void	OnServer_ResiterServerData			( int iServerID, EM_SERVER_TYPE ServerType, DWORD dwServerLocalID, bool bReg ) =0;
		void			TransmitServerData				( int iServerID, int iTransmitServerID, EM_SERVER_TYPE emServerType, int iSrvLocalID );
		void			RegSrvDataFailed				( int iServerID, EM_REG_SRV_FAILED emRegSrvDataFailed );
		void			SendLocalID						( int iServerID, DWORD dwLocalID );


	virtual	void	OnServer_TransmitServerData			( int iServerID, EM_SERVER_TYPE ServerType, DWORD dwServerLocalID ) =0;

	virtual	void	OnServer_RegSrvDataFailed			( int iServerID, EM_REG_SRV_FAILED emRegSrvDataFailed ) =0;

	virtual	void	OnServer_LocalID					( int iServerID, DWORD dwLocalID ) =0;

	virtual	void	OnServer_ServerReady				( int iServerID, EM_SERVER_TYPE ServerType, DWORD dwServerLocalID ) = 0;
	
	// To Server List Event
	//------------------------------------------------------------------------------------------
	virtual	void	OnServerEvent_Shutdown				( int iServerID, int iCloseTime ) = 0;
	virtual	void	OnServerEvent_ServerReady			( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID ) = 0;
	virtual void	OnServerEvent_ServerLogin			( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID ) = 0;
	virtual void	OnServerEvent_ServerLogOut			( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID ) = 0;

	
	
	virtual	void	Shutdown							( int iSrvID, int iCloseTime );

	virtual	void	RegisterAccountEvent				();
	virtual	void	AccountEnterWorld					( int iClientID, string sAccount );
	virtual	void	AccountLeaveWorld					( int iClientID, string sAccount );


	//virtual void OnLogin			( string sInfo ) = 0;
	//virtual void OnLogout			() = 0;
	//virtual void OnLogFailed		( SYSLogFailedENUM emType ) = 0;

	//virtual void OnServerConnect	( int iSrvID , string sSrvName ) = 0;
	//virtual void OnServerDisconnect	( int iSrvID ) = 0;

	EM_SERVER_TYPE	GetServerType				() { return m_ServerType; }
	int				GetLocalID					() { return m_dwServerLocalID; }
//	bool			IsInit						() { return ( m_pThis != NULL ); }

	

	
	//virtual void OnClientonnect		( int iCliID , string sAccount ) = 0;
	//virtual void OnClientDisconnect	( int iCliID ) = 0;

	//virtual void Test_CliToGSrv( int CliID  , int Data ) = 0;
	//virtual void GetTest_GSrvToGSrv( int CliID  , int Data ) = 0;

	//virtual void GetOtherGSrvMsg( int SrvID  , int Data ) = 0;




	//static bool			SendToServer( EM_SERVER_TYPE ServerType ,int id,int Size,void *Buff);

/*

	static bool			SendToMaster	(int Size,void *Buff);
	static bool			SendToChat		(int Size,void *Buff);
	static bool			SendToDataCenter(int Size,void *Buff);
	static bool			SendToLocal		(int id,int Size,void *Buff);


	static void			SendToAllLocal(int Size,void *Buff);
	static	ServerData* GetServerData (int SockID);
	static	ServerData* GetServerData ( EM_SERVER_TYPE type ,int id);
*/
};
