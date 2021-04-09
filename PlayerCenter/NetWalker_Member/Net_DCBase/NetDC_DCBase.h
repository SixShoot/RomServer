
#pragma once

#include "../../MainProc/Global.h"
#include "../Net_ServerList/Net_ServerList_Child.h"

//class CheckKeyAddressStruct;  // marked by slime - 2011/03/16
// �����D����n�[�o��w�q�b��.
// ���]�禡�����ݭn�Ψ즹���c�w�q,
// �Х[�W #include "filename.h" ����� .cpp �ɮת��̤W��.

class CNetDC_DCBase : public Global
{
protected:
	//-------------------------------------------------------------------
	static bool				_Init();
	static bool				_Release();

    static void _PG_DCBase_XtoRD_SavePlayer                     ( int NetID , int Size , void* Data );
    static void _PG_DCBase_LtoRD_SaveNPCRequest					( int NetID , int Size , void* Data );
    static void _PG_DBBase_XtoRD_SetPlayerLiveTime              ( int NetID , int Size , void* Data );
    static void _PG_DBBase_XtoRD_SetPlayerLiveTime_ByAccount    ( int NetID , int Size , void* Data );
	static void _PG_DBBase_XtoRD_CheckRoleDataSize				( int NetID , int Size , void* Data );

	static void	_OnDataCenterConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	static void	_OnServersConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
//-------------------------------------------------------------------	
public:
	static CNetDC_DCBase*	This;
    //-------------------------------------------------------------------	

    virtual void SavePlayer                 ( int SrvNetID , int SaveTime ,  int DBID , int Start , int Size , void* Data ) = 0;
        static void SavePlayerFailed        ( int SrvNetID , int DBID );

    virtual void SetPlayerLiveTime          ( int SrvNetID ,  int DBID , int LiveTime ) = 0;
        static void SetPlayerLiveTimeFailed ( int SrvNetID , int DBID );

    virtual void SetPlayerLiveTime_ByAccount( int SrvNetID , char* Account, int LiveTime ) = 0;
        static void SetPlayerLiveTimeFailed_ByAccount ( int SrvNetID , char* Account );

	virtual void R_CheckRoleDataSize		( int SrvNetID , int Size ) = 0;
		static void S_CheckRoleDataSizeResult( int SrvNetID , bool Result );

	static void	LogSqlCommand( char* SqlCmd );
	static void	ImportSqlCommand( char* SqlCmd );

	static void SL_CheckRoleLiveTime		( int ZoneID , int DBID  );

	virtual void    OnDataCenterConnect( ) = 0;

	//�PMaster DataCenter Zone ���߳s�u
	virtual void    OnServersConnect( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID ) = 0;
	static void	S_CheckRoleStruct( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	
};
