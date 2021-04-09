#pragma once
#include "Net_DCBase.h"

//--------------------------------------------------------------------------------------------
class Net_DCBaseChild : public Net_DCBase
{
	static bool	_IsLoadData;
 
	static int      _CheckLoadNPC		( SchedularInfo* Obj , void* InputClass );   //檢查是否可以載入NPC

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
	virtual void RD_CheckRoleDataSizeResult( bool Result );
	//回應npc移動位置的資訊
	virtual void NPCMoveInfoResult( int ZoneID , int NPCDBID , int TotalCount , int IndexID , NPC_MoveBaseStruct& Base );
	virtual void RD_ZoneInfoResult( LocalServerBaseDBInfoStruct& Info );
	virtual void RD_GlobalZoneInfoResult( GlobalServerDBInfoStruct& Info );
//	virtual void RD_SaveRoleDataNotify( const char* Account , int PlayerDBID , int ClientID , int MasterSockID );
	virtual void RD_GmCommandRequest( GmCommandStruct&	Info );
	virtual void RD_CheckRoleLiveTime( int DBID );
//	virtual void R_CheckRoleStruct( int Count , CheckKeyAddressStruct Info[1000] );
	virtual void R_GlobalZoneInfoList( int Count , ZoneConfigBaseStruct Info[1000] );
//--------------------------------------------------------------------------------------------
    virtual void OnDataCenterConnect( );
	virtual void OnRoleCenterConnect( int playerCenterID );
	virtual void OnRoleCenterDisconnect( int playerCenterID );

};
