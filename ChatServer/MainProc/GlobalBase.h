#pragma  once

#include "NetWaker/GSrvNetWaker.h"

#include "debuglog/errormessage.h"

#define SEND_NEW_LOG_CENTER_SERVER 1
#define _Def_BattleGround_WorldID_				30
#define _Def_BattleGround_GuildWar_ZoneID_		402

class CNet_ServerList_Child;

class GlobalBase
{
protected:
	static GSrvNetWaker*              _Net;

    static void    _Init();
    static void    _Release();
public:
    static  GSrvNetWaker* Net(){ return _Net;};
    static  bool 	SendToLocal        	( int ZoneID , int Size , const void* Data );
    static  bool 	SendToMaster       	( int Size , const void* Data );
    static  bool 	SendToChat         	( int Size , const void* Data );
	static  bool 	SendToRoleDBCenter  ( int PlayerCenterID ,int Size , const void* Data );
    static  bool 	SendToDBCenter     	( int Size , const void* Data );

#if SEND_NEW_LOG_CENTER_SERVER
    static  bool    SendToLogCenter     ( int Size , const void* Data );
#endif

    static  bool 	SendToPartition    	( int Size , const void* Data );
    static  bool 	SendToAllGMTools   	( int Size , const void* Data );
	static  bool 	SendToAllLocal		( int Size , const void* Data );
	static  void 	SendToAllServer		( int Size , const void* Data );
    
	
    static  bool 	SendToCli          	( int SockID  , int Size , const void* Data );
	static  bool 	SendToCli_ByProxyID	( int SockID  , int ProxyID , int Size , const void* Data );
    static  void 	SendToAllCli       	( int Size , const void* Data );
	static  void 	SendToAllPlayer    	( int Size , const void* Data );
    static  bool 	SendToSrvBySockID  	( int SockID  , int Size , const void* Data );

    static  int		GetZoneNetID		( int ZoneID );

    static  void    SetPrintLV			( int LV );

	static  bool 	SendToCli_ByDBID	( int DBID , int Size , const void* Data );
	static	char*	GetPlayerName		( int DBID );
	static  FunctionSchedularClass*     Schedular() { return _Net->Schedular(); };


	//���o�Y�ϰ즳�X�ӥ���Ŷ�
	static int		GetParallelCount			( int ZoneID );
	static int		GetParallelMaxID			( int ZoneID );

	//���o���Ī��ϰ�
	static int		GetParallelZoneID			( int ZoneID , int ParallelID );

	//�ˬd�ϰ�O�_���}
	static bool		CheckZoneID					( int ZoneID );

	//��Client �� IP Num
	static unsigned GetClientIpNum				( int SockID );
	static string   GetClientIpNumStr			( int SockID );

	//�j���ʥ]�e�X�h
	static void		SendFlush			()		{ _Net->SendFlush(); }


	static	void	Shutdown_Proxy()			{};			// �����Ҧ��� Proxy
	static	void	Shutdown_Switch()			{};			// �����Ҧ��� Switch

	static int      CheckSystemStatus		( SchedularInfo* Obj , void* InputClass );	
public:
    static CNet_ServerList_Child*				Net_ServerList;

};
