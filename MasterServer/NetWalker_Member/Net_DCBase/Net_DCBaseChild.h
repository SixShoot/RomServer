#pragma once
#include "Net_DCBase.h"

//--------------------------------------------------------------------------------------------
class Net_DCBaseChild : public Net_DCBase
{

	static bool	_IsStart;
public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

    virtual void SavePlayerFailed( int DBID );
    virtual void SaveNPCResultOK( int LocalObjID , int NPCDBID );
    virtual void SaveNPCResultFailed( int LocalObjID );
    virtual void DropNPCResultOK( int LocalObjID , int NPCDBID );
    virtual void DropNPCResultFailed( int LocalObjID , int NPCDBID );
    virtual void LoadNPCResult( int TotalCount , int ID , NPCData* NPC );
    virtual void SetPlayerLiveTimeFailed( int DBID );
    virtual void SetPlayerLiveTimeFailed_ByAccount( char* Acount );
	virtual void NPCMoveInfoResult( int ZoneID , int NPCDBID , int TotalCount , int IndexID , NPC_MoveBaseStruct& Base );
	virtual void RD_CheckRoleDataSizeResult( bool Result );
	virtual void OnDataCenterConnect( ) ;
	virtual void RD_ZoneInfoResult( LocalServerBaseDBInfoStruct& Info );
	virtual void RD_GlobalZoneInfoResult( GlobalServerDBInfoStruct& Info );
	virtual void RD_GmCommandRequest( GmCommandStruct&	Info );
	virtual void OnRoleCenterConnect( int playerCenterID );
	virtual void OnRoleCenterDisconnect( int playerCenterID );
//	virtual void R_CheckRoleStruct( int Count , CheckKeyAddressStruct Info[1000] );
//--------------------------------------------------------------------------------------------
};
