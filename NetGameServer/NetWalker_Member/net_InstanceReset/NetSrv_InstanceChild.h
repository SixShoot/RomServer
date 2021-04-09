#pragma once
#include "NetSrv_Instance.h"



class NetSrv_InstanceChild : public NetSrv_Instance
{
public:
	static map< int , map< int , int > >	_PartyKeyValue;	//PartyKeyValue[PartyKey][KeyID]
	static map< int , bool >				_UpdatePartyLiveTime;
	static map< int , int >					_PartyKeyMap;
	static WeekDayStruct					_ResetDay;
	static unsigned							_InstanceLiveTime;		//Party �i�s���ɶ�
	static unsigned							_NextInstanceLiveTime;	//�U�� �ƥ��s�ɭ��m�I
	static int								_MaxPartyKeyID;

	NetSrv_InstanceChild() {  };
    static bool Init();
    static bool Release();

	virtual void RD_ResetTime( WeekDayStruct& ResetDay );
	virtual void RD_PlayInfo( int PartyKey , int KeyID , int KeyValue );
	virtual void RC_JoinInstanceResult( BaseItemObject* Player , bool Result ); 

	static void SetInstanceLiveTime( unsigned Time );
	static void CheckInstanceLiveTime();

	virtual void OnChatDisconnect( );
	virtual void OnChatConnect( );


	//�ƥ��i���x�s�i���B�z
	virtual void	RC_InstanceExtend( BaseItemObject* Player , int ZoneID , int SavePos , int ExtendTime );
	virtual void	RL_InstanceExtend( int SrvSockID , int PlayerDBID , int ZoneID , int LiveTime , int ExtendTime , int KeyID , int SavePos );
	virtual void	RL_InstanceExtendResult( int PlayerDBID , int ZoneID , int ExtendTime , int LiveTime , int SavePos , int KeyID ); 

	virtual void	RC_InstanceRestTimeRequest( BaseItemObject* Player  , int ZoneID , int SavePos );
	virtual void	RL_InstanceRestTimeRequest( int PlayerDBID , int KeyID ); 

	virtual void	RD_SetWorldVar( const char* pszVarName, int iVarValue );
	virtual void	RC_GetWorldVar( BaseItemObject* Player, const char* pszVarName );

	static	void 	SetWorldVar		( const char* pszVarName, int iVarValue );
	static	int 	GetWorldVar		( const char* pszVarName );

	//----------------------------------------------------------------
	map< string, int >			m_mapVar;

};

