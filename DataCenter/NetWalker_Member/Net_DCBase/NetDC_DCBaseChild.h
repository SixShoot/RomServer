#pragma once

#include "NetDC_DCBase.h"
//--------------------------------------------------------------------------------------------
struct DBNPCSaveInfoStruct
{
    bool    OnProcFlag;
    int     SrvNetID;
    int     LocalObjID;
    
    NPCData NPC;
    DBNPCSaveInfoStruct( )
    {
        OnProcFlag = false;
    }
};


struct GlobalSettingInfo
{

	GlobalServerDBInfoStruct Info;

	union
	{
		int	CheckVersion[10];
		struct  
		{
			int		LastRunTime;		//最後執行的時間
			int		TimeLimit;			//期限
			int		Reserve2;
			int		Reserve3;
			int		Reserve4;
			int		LastRunTimeCheck;	
			int		TimeLimitCheck;		//期限
			int		Reserve7;
			int		Reserve8;
			int		Reserve9;
		};
	};
	
		
	void EncodeCheckVersion()
	{
		const int XorValue = 0x1abcdef1;
		CheckVersion[ 0 ] = CheckVersion[ 0 ] ^ XorValue;
		for( int i = 1 ; i < 10 ; i++ )
		{
			CheckVersion[ i ] = CheckVersion[ i -1 ] ^ CheckVersion[ i ];
		}
	}

	void DecodeCheckVersion()
	{
		const int XorValue = 0x1abcdef1;		
		for( int i = 10 ; i >= 1 ; i-- )
		{
			CheckVersion[ i ] = CheckVersion[ i -1 ] ^ CheckVersion[ i ];
		}
		CheckVersion[ 0 ] = CheckVersion[ 0 ] ^ XorValue;
	}

};

struct ProcessCmdStruct
{
	int		GUID;
	char	SqlCmd[1024];
};

//--------------------------------------------------------------------------------------------
class CNetDC_DCBaseChild : public CNetDC_DCBase
{
	static vector<ZoneConfigBaseStruct> _GlobalSettingList;
	//static GlobalSettingInfo _GlobalSetting;
	//要處理的Sql命令
	static vector< ProcessCmdStruct >	 _SqlCmdList[4];
    //要儲存的NPC List
    static deque< DBNPCSaveInfoStruct >  _SaveNPCList;    
	//所有NPC移動旗子的資訊
	static map< int , vector< DB_NPC_MoveBaseStruct > > _NPCMoveFlagInfo;
	static bool _IsLoadOK_NPCMoveFlagInfo;
	static bool	_IsProcDBSqlCmdOK;
	static bool	_IsGmCommandQuery;
//--------------------------------------------------------------------------------------------

    static void _DBWriteNPCDBEventCB ( NPCData *Data , void* UserObj , char *WhereCmd , bool ResultOK );
    static void _DBDropNPCDBEventCB ( NPCData *Data , void* UserObj , char *Cmd , bool ResultOK );
    
    //static void _DBReadAllNPCDBEventCB  ( vector<NPCData>& Data , void* UserObj , char *WhereCmd , bool ResultOK );
	static bool _SQLCmdLoadAllNPCInfoEvent( vector<NPCData>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadAllNPCInfoResultEvent ( vector<NPCData>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
    static int  _OnTimeProc( SchedularInfo* Obj , void* InputClass );

    static bool _SQLCmdWriteNPCRequestEvent( vector<NPCData>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
    static void _SQLCmdWriteNPCResultEvent ( vector<NPCData>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdLoadAllNPCMoveInfoEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadAllNPCMoveInfoResultEvent ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	
//	static bool _SQLCmdLoadNPCMoveInfoEvent( vector<DB_NPC_MoveBaseStruct>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
//	static void _SQLCmdLoadNPCMoveInfoResultEvent ( vector<DB_NPC_MoveBaseStruct>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdWriteNPCMoveRequestEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdWriteNPCMoveResultEvent ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	
	static bool _SQLCmdLoadInstanceResetTime( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadInstanceResetTime( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdLoadZoneInfoRequestEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadZoneInfoResultEvent ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdLoadGlobalInfoRequestEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadGlobalInfoResultEvent ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdLoadGMCommandEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadGMCommandResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//儲存GM回傳結果
	static bool _SQLCmdSaveGMCommandEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdSaveGMCommandResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//////////////////////////////////////////////////////////////////////////
	//開機存寫資料庫測試
	static bool _SQLCmdDBReadWriteTestEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdDBReadWriteTestResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	//////////////////////////////////////////////////////////////////////////
	//在import db讀取要處理的 db sql 命令
	static bool _SQLCmdLoadSqlCmdEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadSqlCmdResultEvent ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdProcSqlCmdEvent( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdProcSqlCmdResultEvent ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	//////////////////////////////////////////////////////////////////////////

	static void  _SQL_DBWriteEvent( int *Data , void* UserObj , char *WhereCmd , bool ResultOK );	
//	static void  _SQL_Log_DBWriteEvent( int *Data , void* UserObj , char *WhereCmd , bool ResultOK );
public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

//    virtual void SavePlayer                 ( int SrvNetID , int SaveTime , int DBID , int Start , int Size , void* Data );
    virtual void SaveNPCRequest				( int SrvNetID , int LocalObjID , RoleDataEx* NPC );
    virtual void DropNPCRequest             ( int SrvNetID , int LocalObjID , int NPCDBID , bool IsDelFlag , const char* DestroyAccount );
    virtual void LoadAllNPC                 ( int ZoneID );
//    virtual void SetPlayerLiveTime          ( int SrvNetID , int DBID , int LiveTime );
//    virtual void SetPlayerLiveTime_ByAccount( int SrvNetID , char* Account , int LiveTime );

	virtual void LoadAllNPCMoveInfo			( int ZoneID );
	virtual	void SaveNPCMoveInfo			( int TotalCount , int ZoneID , int NPCDBID , int IndexID , NPC_MoveBaseStruct& Base );
	virtual	void R_SqlCommand				( int ThreadID , SqlCommandTypeENUM Type , char* SQLCmd );
//	virtual	void R_LogSqlCommand			( char* SQLCmd );
//	virtual void R_CheckRoleDataSize		( int SrvNetID , int Size );
	virtual void R_ZoneInfoRequest			( int NetID , int ZoneID ); 
	virtual void R_LogData					( int LogType, void* LogData, unsigned long DataSize );

//	virtual void RL_SaveRoleDataNotifyResult	( const char* Account , int PlayerDBID , int ClientID , int MasterSockID , bool Result );

	virtual void RL_GmCommandResult			( int GUID , GmCommandResultENUM Result , char  ResultStr[512] ); 
	

	static void	SaveGMCommandResult			( int GUID , GmCommandResultENUM Result , char ResultStr[512] );

	virtual void OnLogin( );

//	virtual void R_CheckRoleStruct( int Count , CheckKeyAddressStruct Info[1000] );
//--------------------------------------------------------------------------------------------
};
