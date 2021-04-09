#ifndef __NETWAKER_CLICONNECTCHILD_H__
#define __NETWAKER_CLICONNECTCHILD_H__

#include "NetSrv_CliConnect.h"

class NetSrv_CliConnectChild : public NetSrv_CliConnect
{
	static ClientSkyProcTypeENUM ClientSkyType;
	static set< int > AllZoneSet;


	int		FindInstanceRoom( RoleDataEx* Player );
public:
    static bool Init();
    static bool Release();

    //-------------------------------------------------------------------
    //  
    //-------------------------------------------------------------------
    static bool ChangeZone( RoleDataEx* Player );

	static void SendLoginWarDeclare( RoleDataEx* Player );

    //-------------------------------------------------------------------
    //  事件繼承實做
    //-------------------------------------------------------------------
	virtual void OnLogin( );
    virtual void CliConnect( int CliID , string Account);
    virtual void CliDisconnect( BaseItemObject* Player );

    //Client 端要求登出由戲 (30 秒倒數)
    virtual void CliLogout( BaseItemObject* Player );

    //有人要求換到此區域
    virtual void OnChangeZone ( PlayerRoleData* Player , int SrcZoneID );

    //要求修改物件
    virtual void OnModify( int CliID , ModifyNPCInfo* Info );

	virtual void OnChangeZoneNodifyResult( int ZoneID , int GItemID , float X , float Y , float Z , ChangeZoneNodifyResult_ENUM Result );

	virtual void RC_GetNPCMoveInfoRequest( BaseItemObject* Player , int NpcGUID );
	virtual void RC_SaveNPCMoveInfo( BaseItemObject* Player , int NpcGUID , int TotalCount , int IndexID , NPC_MoveBaseStruct& Base );


	virtual void RC_GetFlagList		( BaseItemObject* Player );
	virtual void RC_EditFlag		( BaseItemObject* Player, int iFlagGroup, int iFlagID, float X, float Y, float Z, float Dir, int FlagDBID );
	virtual void RC_DelFlag			( BaseItemObject* Player, int iFlagGroup, int iFlagID );
	virtual void RC_DelFlagGroup	( BaseItemObject* Player, int iFlagGroup );

	//////////////////////////////////////////////////////////////////////////
	virtual void RC_ConnectToBattleWorldOK	( BaseItemObject* Player , int CliNetID );

	virtual void RBL_ChangeZoneToOtherWorld( int FromWorldId , int FromNetID , PlayerRoleData& Role , int RoleSize );
	virtual void RBL_ChangeZoneToOtherWorldResult( int FromWorldId , int FromNetID , int RoleDBID , char* IPStr , int Port , ChangeZoneToOtherWorldResultENUM Result );
	virtual void RBL_ConnectCliAndGSrv( int FromWorldId , int FromNetID , int CliNetID  );
	virtual void RBL_RetNomorlWorld( int FromWorldId , int FromNetID , int PlayerDBID , RoleDataEx* RetRole );
	virtual void RBL_BattleSavePlayer( int FromWorldId , int FromNetID , int PlayerDBID , RoleDataEx* RetRole );
	virtual void RC_WorldReturnOK( BaseItemObject* Player );  
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_DelTomb( int TombGUID , int OwnerDBID );
	//////////////////////////////////////////////////////////////////////////
	//設定天空處理方式
	static void SetClientSkyProcType( ClientSkyProcTypeENUM Type );

	//////////////////////////////////////////////////////////////////////////
	//其他world 轉給某個Client 封包轉送
	//////////////////////////////////////////////////////////////////////////
	virtual void RBL_SendToPlayerByDBID( int FromWorldId , int FromNetID , int PlayerDBID , int DataSize , char* Data );
	//////////////////////////////////////////////////////////////////////////
	virtual void OnLocalConnect( int ZoneID );
	virtual void OnLocalDisconnect( int ZoneID );

	//////////////////////////////////////////////////////////////////////////
	//全區域玩家資訊　處理
	static void SendAllPlayerInfoPlayerEnterProc	( BaseItemObject* PlayerObj );


	static void WorldReturnNotifyProc( int GItemID );
	//進入 獨立遊戲戰場 處理
	static void BGIndependenceGameProc( RoleDataEx* Owner );
	static void IndependenceGameReturnProc( RoleDataEx* Role , RoleDataEx* RetRole );
	//////////////////////////////////////////////////////////////////////////
	virtual void RBL_BattleZone( int FromWorldId , int ZoneID , int FromNetID , int PlayerDBID );
	virtual void RL_ReturnWorldRequest( int FromWorldId , int FromNetID , int PlayerDBID , int TestCount );
	virtual void RBL_ReturnWorldResult( int FromWorldId , int PlayerDBID , int TestCount , bool Result );
};

#endif //__NETWAKER_CLICONNECTCHILD_H__