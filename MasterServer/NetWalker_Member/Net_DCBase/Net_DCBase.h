
#pragma once

#include "../../MainProc/Global.h"
#include "../Net_ServerList/Net_ServerList_Child.h"
#include "PG/PG_DCBase.h"

class Net_DCBase : public Global
{
protected:
	//-------------------------------------------------------------------
	static Net_DCBase*	This;

	static bool				_Init();
	static bool				_Release();
    
    static void _PG_DBBase_RDtoX_SavePlayerError                ( int NetID , int Size , void* Data );
    static void _PG_DCBase_DtoL_SaveNPCResult	                ( int NetID , int Size , void* Data );
    static void _PG_DBBase_DtoL_DropNPCResult                   ( int NetID , int Size , void* Data );
    static void _PG_DBBase_DtoL_LoadAllNPCResult                ( int NetID , int Size , void* Data );
    static void _PG_DBBase_RDtoX_SetPlayerLiveTimeError         ( int NetID , int Size , void* Data );
    static void _PG_DBBase_RDtoX_SetPlayerLiveTimeError_ByAccount( int NetID , int Size , void* Data );

	static void _PG_DBBase_DtoL_NPCMoveInfoResult				( int NetID , int Size , void* Data );
	static void _PG_DBBase_RDtoX_CheckRoleDataSizeResult		( int NetID , int Size , void* Data );

	static void _PG_DBBase_DtoL_ZoneInfoResult					( int NetID , int Size , void* Data );
	static void _PG_DBBase_DtoL_GlobalZoneInfoResult			( int NetID , int Size , void* Data );

	static void _PG_DBBase_DtoL_SaveRoleDataNotify				( int NetID , int Size , void* Data );

	static void _PG_DBBase_DtoL_GmCommandRequest				( int NetID , int Size , void* Data );

	static void _PG_DBBase_DtoL_CheckRoleLiveTime				( int NetID , int Size , void* Data );
	static void _PG_DBBase_RDtoX_CheckRoleStruct				( int NetID , int Size , void* Data );

	static void _PG_DBBase_DtoL_GlobalZoneInfoList				( int NetID , int Size , void* Data );
	//-------------------------------------------------------------------	

    static void	_OnDataCenterConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	static void	_OnRoleCenterConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	static void	_OnRoleCenterDisconnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
public:
    //-------------------------------------------------------------------	

    static void SavePlayer( int PlayerCenterID , PlayerRoleData* Role , int Start , int Size , int SaveTime = 1000*60*30 );         
        virtual void SavePlayerFailed( int DBID ) = 0;

    static void SaveNPCRequest( RoleDataEx* NPC );
        virtual void SaveNPCResultOK( int LocalObjID , int NPCDBID ) = 0;
        virtual void SaveNPCResultFailed( int LocalObjID ) = 0;

    static void DropNPCRequest( RoleDataEx* NPC );
	static void DropNPCRequest( int NPCDBID , const char* DestroyAccount );
        virtual void DropNPCResultOK( int LocalObjID , int NPCDBID ) = 0;
        virtual void DropNPCResultFailed( int LocalObjID , int NPCDBID ) = 0;

    static void LoadAllNPC( int ZoneID );
        virtual void LoadNPCResult( int TotalCount , int ID , NPCData* NPC ) = 0;
	

    //LiveTime 以千分之一秒為單位
    static void SetPlayerLiveTime( int PlayerCenterID , int DBID , int LiveTime );
        virtual void SetPlayerLiveTimeFailed( int DBID ) = 0;

    //LiveTime 以千分之一秒為單位
    //設定此人的所有角色資料
    static void SetPlayerLiveTime_ByAccount( int PlayerCenterID , char* Account, int LiveTime );
        virtual void SetPlayerLiveTimeFailed_ByAccount( char* Acount ) = 0;

		virtual void    OnDataCenterConnect( ) = 0;
		virtual void    OnRoleCenterConnect( int playerCenterID ) = 0;
		virtual void    OnRoleCenterDisconnect( int playerCenterID ) = 0;
    //-------------------------------------------------------------------	
	//要求載入所有npc移動位置的資訊
	static void LoadAllNPCMoveInfo( int ZoneID );
	//回應npc移動位置的資訊
		virtual void NPCMoveInfoResult( int ZoneID , int NPCDBID , int TotalCount , int IndexID , NPC_MoveBaseStruct& Base ) = 0;
	//要求儲存npc移動位置
	static void SaveNPCMoveInfo(  int ZoneID , int NPCDBID , vector<NPC_MoveBaseStruct>& List );

    //-------------------------------------------------------------------	
	//要求執行 SQL 命令
	static void	SqlCommand_Import( int ThreadID , const char* SqlCmd );
	static void	SqlCommand( int ThreadID , const char* SqlCmd );
	static void	LogSqlCommand( const char* SqlCmd );
	static void	LogSqlCommand( int ThreadID , const char* SqlCmd );

	//-------------------------------------------------------------------	
	//傳送log資料
	static void SD_LogData(int LogType, void* Data, unsigned long DataSize);

	//-------------------------------------------------------------------	
	//檢查角色資料大小是否與datacenter相同
	static void SD_CheckRoleDataSize( int PlayerCenterID );
		virtual void RD_CheckRoleDataSizeResult( bool Result ) = 0;

	static void SD_ZoneInfoRequest( int ZoneID );
		virtual void RD_ZoneInfoResult( LocalServerBaseDBInfoStruct& Info ) = 0;

	virtual void RD_GlobalZoneInfoResult( GlobalServerDBInfoStruct& Info ) = 0;

	virtual void RD_GmCommandRequest( GmCommandStruct&	Info ) = 0;
	static void SD_GmCommandResult( int	GUID , GmCommandResultENUM Result , const char ResultStr[512] );
//	virtual void RD_SaveRoleDataNotify( const char* Account , int PlayerDBID , int ClientID , int MasterSockID ) = 0;
//		static void SD_SaveRoleDataNotifyResult( const char* Account , int PlayerDBID , int ClientID , int MasterSockID , bool Result );

	virtual void RD_CheckRoleLiveTime( int DBID ){};

	virtual void R_CheckRoleStruct( int Count , CheckKeyAddressStruct Info[1000] );
	virtual void R_GlobalZoneInfoList( int Count , ZoneConfigBaseStruct Info[1000] ){}; 
};
