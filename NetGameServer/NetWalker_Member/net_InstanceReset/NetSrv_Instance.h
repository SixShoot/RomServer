#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_Instance.h"

class NetSrv_Instance : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Instance* This;
    static bool _Init();
    static bool _Release();

	static void	_PG_Instance_DtoL_ResetTime					( int sockid , int size , void* data );
	static void	_PG_Instance_DtoL_PlayInfo					( int sockid , int size , void* data );
	static void	_PG_Instance_CtoL_JoinInstanceResult		( int sockid , int size , void* data );

	static void	_PG_Instance_CtoL_InstanceExtend			( int sockid , int size , void* data );
	static void	_PG_Instance_LtoL_InstanceExtend			( int sockid , int size , void* data );
	static void	_PG_Instance_LtoL_InstanceExtendResult		( int sockid , int size , void* data );

	static void	_PG_Instance_CtoL_InstanceRestTimeRequest	( int sockid , int size , void* data );
	static void	_PG_Instance_LtoL_InstanceRestTimeRequest	( int sockid , int size , void* data );

	static void	_PG_Instance_DtoL_SetWorldVar				( int sockid , int size , void* data );
	static void	_PG_Instance_CtoL_GetWorldVar				( int sockid , int size , void* data );

	
	


	static void	_OnChatConnectEvent    ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	static void	_OnChatDisconnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );

public:    
    
	virtual void RD_ResetTime( WeekDayStruct& ResetDay ) = 0;
	virtual void RD_PlayInfo( int PartyKey , int KeyID , int KeyValue ) = 0;
	virtual void RC_JoinInstanceResult( BaseItemObject* Player , bool Result ) = 0; 

	static void SC_ResetTime( int SendToID , int Time , int InstanceSaveID , int ValueKeyID , bool IsLogin );
	static void SC_JoinInstanceRequest( int SendToID );
	static void SC_AllPlayer_InstanceReset( int InstanceKeyID );
	static void SC_AllMember_InstanceCreate( int PartyID , int Key , int PlayerDBID );

	virtual void OnChatDisconnect( ) = 0;
	virtual void OnChatConnect( ) = 0;

	//副本進度儲存展期處理
	virtual void RC_InstanceExtend( BaseItemObject* Player , int ZoneID , int SavePos , int ExtendTime ) = 0;
		static void SL_InstanceExtend( int ZoneID , int PlayerDBID , int LiveTime , int ExtendTime , int KeyID  , int SavePos );
		virtual void RL_InstanceExtend( int SrvSockID , int PlayerDBID , int ZoneID , int LiveTime , int ExtendTime , int KeyID , int SavePos ) = 0;
			static void SL_InstanceExtendResult( int SrvSockID , int PlayerDBID , int ZoneID , int ExtendTime , int LiveTime , int SavePos , int KeyID ); 
			virtual void RL_InstanceExtendResult( int PlayerDBID , int ZoneID , int ExtendTime , int LiveTime , int SavePos , int KeyID ) = 0; 

	//修正副本進度時間資訊
	static void SC_FixInstanceExtend( int PlayerDBID , int ZoneID , int ExtendTime , int LiveTime , int KeyID );

	//取得副本進度資訊
	virtual void RC_InstanceRestTimeRequest( BaseItemObject* Player  , int ZoneID , int SavePos ) = 0;
		static void SL_InstanceRestTimeRequest( int ZoneID , int PlayerDBID , int KeyID );
		virtual void RL_InstanceRestTimeRequest( int PlayerDBID , int KeyID ) = 0; 
			static void SC_InstanceRestTime( int PlayerDBID , int ZoneID , int KeyID , int LiveTime , int ExtendTime );

	//世界變數
	virtual void RD_SetWorldVar( const char* pszVarName, int iVarValue ) = 0;
	virtual	void RC_GetWorldVar( BaseItemObject* Player, const char* pszVarName ) = 0;

};

