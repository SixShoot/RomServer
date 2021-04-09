
#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_DCBase.h"

class CNetDC_DCBase : public Global
{
protected:
	//-------------------------------------------------------------------
	static bool				_Init();
	static bool				_Release();

//    static void _PG_DCBase_XtoD_SavePlayer                      ( int NetID , int Size , void* Data );
    static void _PG_DCBase_LtoD_SaveNPCRequest					( int NetID , int Size , void* Data );
    static void _PG_DBBase_LtoD_DropNPCRequest                  ( int NetID , int Size , void* Data );
    static void _PG_DBBase_LtoD_LoadAllNPC                      ( int NetID , int Size , void* Data );
//    static void _PG_DBBase_XtoD_SetPlayerLiveTime               ( int NetID , int Size , void* Data );
//    static void _PG_DBBase_XtoD_SetPlayerLiveTime_ByAccount     ( int NetID , int Size , void* Data );

	static void _PG_DBBase_LtoD_LoadAllNPCMoveInfo				( int NetID , int Size , void* Data );
	static void _PG_DBBase_LtoD_SaveNPCMoveInfo					( int NetID , int Size , void* Data );

	static void _PG_DBBase_LtoD_SqlCommand						( int NetID , int Size , void* Data );
	static void _PG_DBBase_LtoD_LogSqlCommand					( int NetID , int Size , void* Data );

	static void _PG_DBBase_XtoD_CheckRoleDataSize				( int NetID , int Size , void* Data );
	static void _PG_DBBase_LtoD_ZoneInfoRequest					( int NetID , int Size , void* Data );

	static void _PG_DBBase_LtoD_SaveRoleDataNotifyResult		( int NetID , int Size , void* Data );

	static void _PG_DBBase_LtoD_GmCommandResult					( int NetID , int Size , void* Data );
	static void _PG_DBBase_RDtoX_CheckRoleStruct				( int NetID , int Size , void* Data );
	static void _PG_DBBase_LtoD_LogData							( int NetID , int Size , void* Data );
	static void _OnLoginEvent( string info );
	//-------------------------------------------------------------------	
public:
	static CNetDC_DCBase*	This;
    //-------------------------------------------------------------------	

//    virtual void SavePlayer                 ( int SrvNetID , int SaveTime ,  int DBID , int Start , int Size , void* Data ) = 0;
//        static void SavePlayerFailed        ( int SrvNetID , int DBID );

    virtual void SaveNPCRequest				( int SrvNetID , int LocalObjID , RoleDataEx* NPC ) = 0;
        static void SaveNPCResultOK			( int SrvNetID , int LocalObjID , int NPCDBID );
        static void SaveNPCResultFailed		( int SrvNetID , int LocalObjID );

    virtual void DropNPCRequest             ( int SrvNetID , int LocalObjID , int NPCDBID , bool IsDelFlag , const char* DestroyAccount ) = 0;
        static void DropNPCResultOK         ( int SrvNetID , int LocalObjID , int NPCDBID );
        static void DropNPCResultFailed     ( int SrvNetID , int LocalObjID , int NPCDBID );

    virtual void LoadAllNPC                 ( int ZoneID ) = 0;
        static void LoadNPCResult           ( int ZoneID , int TotalCount , int ID , NPCData* NPC );

//    virtual void SetPlayerLiveTime          ( int SrvNetID ,  int DBID , int LiveTime ) = 0;
//        static void SetPlayerLiveTimeFailed ( int SrvNetID , int DBID );

//    virtual void SetPlayerLiveTime_ByAccount( int SrvNetID , char* Account, int LiveTime ) = 0;
//        static void SetPlayerLiveTimeFailed_ByAccount ( int SrvNetID , char* Account );


	virtual void LoadAllNPCMoveInfo			( int ZoneID ) = 0;
		static void NPCMoveInfoResult		( int ZoneID , int NPCDBID , int TotalCount , int IndexID , NPC_MoveBaseStruct& Base );
	virtual	void SaveNPCMoveInfo			( int TotalCount , int ZoneID , int NPCDBID , int IndexID , NPC_MoveBaseStruct& Base ) = 0;

	virtual	void	R_SqlCommand			( int ThreadID , SqlCommandTypeENUM Type , char* SQLCmd ) = 0;
//	virtual	void	R_LogSqlCommand			( char* SQLCmd ) = 0;

/*	virtual void R_CheckRoleDataSize		( int SrvNetID , int Size ) = 0;
		static void S_CheckRoleDataSizeResult( int SrvNetID , bool Result );
*/
	virtual void R_ZoneInfoRequest			( int NetID , int ZoneID ) = 0; 
		static void S_ZoneInfoResult		( int SrvNetID , LocalServerBaseDBInfoStruct& Info );

	static void S_GlobalZoneInfoResult		( GlobalServerDBInfoStruct& Info );

	static void S_GlobalZoneInfoList		( vector<ZoneConfigBaseStruct>& List );
//	static void SL_SaveRoleDataNotify		( int LocalID , int RoleDBID , char* Account , int ClientID , int MasterSockID );
//		virtual void RL_SaveRoleDataNotifyResult	( const char* Account , int PlayerDBID , int ClientID , int MasterSockID  , bool Result ) = 0;

	static void SL_GmCommandRequest			( int LocalID , GmCommandStruct& Info );
		virtual void RL_GmCommandResult			( int GUID , GmCommandResultENUM Result , char  ResultStr[512] ) = 0; 

	virtual void OnLogin( ) = 0;

	virtual void R_CheckRoleStruct( int Count , CheckKeyAddressStruct Info[1000] );
	virtual void R_LogData(int LogType, void* LogData, unsigned long DataSize) = 0;
};
