#ifndef __NETWAKER_CLICONNECT_H__
#define __NETWAKER_CLICONNECT_H__

#include "../../MainProc/Global.h"
#include "PG/PG_CliConnect.h"

class NetSrv_CliConnect : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_CliConnect* This;
    static bool _Init();
    static bool _Release();

    static void _OnCliConnect( int ID , string Account );
    static void _OnCliDisconnect( int ID );

    static void _PG_CliConnect_CtoL_RequestLogout			( int Sockid , int Size , void* Data );
    static void _PG_CliConnect_LtoL_ChangeZoneNodify		( int Sockid , int Size , void* Data );
    static void _PG_CliConnect_CtoL_ModifyNPCInfo			( int Sockid , int Size , void* Data );
	static void _PG_CliConnect_LtoL_ChangeZoneNodifyResult 	( int Sockid , int Size , void* Data );

	static void _PG_CliConnect_CtoL_GetNPCMoveInfoRequest	( int Sockid , int Size , void* Data );
	static void _PG_CliConnect_CtoL_SaveNPCMoveInfo			( int Sockid , int Size , void* Data );

	static void _PG_CliConnect_CtoL_GetFlagList				( int Sockid , int Size , void* Data );
	static void _PG_CliConnect_CtoL_EditFlag				( int Sockid , int Size , void* Data );
	static void _PG_CliConnect_CtoL_DelFlag					( int Sockid , int Size , void* Data );
	static void _PG_CliConnect_CtoL_DelFlagGroup			( int Sockid , int Size , void* Data );

	static void _PG_CliConnect_LtoL_DelTomb					( int Sockid , int Size , void* Data );
	//////////////////////////////////////////////////////////////////////////
	static void _PG_CliConnect_CtoL_ConnectToBattleWorldOK	( int Sockid , int Size , void* Data );

	static void _PG_CliConnect_LtoBL_ChangeZoneToOtherWorld			( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_CliConnect_BLtoL_ChangeZoneToOtherWorldResult	( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_CliConnect_LtoBL_ConnectCliAndGSrv				( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_CliConnect_BLtoL_RetNomorlWorld					( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_CliConnect_BLtoBL_SendToPlayerByDBID			( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_CliConnect_BLtoL_BattleSavePlayer				( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_CliConnect_LtoBL_ChangeZoneToOtherWorld_Zip		( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_CliConnect_BLtoL_BattleSavePlayer_Zip			( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_CliConnect_BLtoL_RetNomorlWorld_Zip				( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_CliConnect_CtoL_WorldReturnOK					( int Sockid , int Size , void* Data );

	static void _PG_CliConnect_BLtoL_BattleZone						( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_CliConnect_LtoBL_ReturnWorldRequest				( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_CliConnect_BLtoL_ReturnWorldResult				( int FromWorldId , int FromNetID , int Size , void* Data );

	static void _OnLocalDisconnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );
	static void _OnLocalConnectEvent ( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );

	static void	_OnLogin ( string Info );
public:
    //-------------------------------------------------------------------
    // 資料送出(測試)
    //-------------------------------------------------------------------
    // 傳送區域基本資料
    static void SendZoneInfo( RoleDataEx* Player );

    //送角色區域ID資料
    static void SendPlayerItemID( RoleDataEx* Player );

    //更新某人的角色資料
    static void SendPlayerDataInfo( RoleDataEx* Player );

	//更新某人的角色資料(有壓縮)
	static void SendPlayerDataInfo_ZIP( RoleDataEx* Player );

	//送區域版本資料
	static void SendSysVersionInfo( int SockID , int LearnMagicID , SystemVersionENUM Version , float PK_DotRate );

    //通知角色準備換區
    static void SendPrepareChangeZone( RoleDataEx* Player );

    //通知角色換區OK
    static void SendChangeZoneOK( RoleDataEx* Player );

    //L to L
    //通知換區資訊 
    static bool SendChangeZone( RoleDataEx* Player );

	static void SendChangeZoneNotifyResult( int ToZoneID , int GItemID , float X , float Y , float Z , ChangeZoneNodifyResult_ENUM Result );


    //通知Client 修改物件資訊
    static void SendModifyInfo( int SendID , RoleDataEx* Obj );

	//通知NPC移動資料
	static void SendNPCMoveInfo( int SendID , int NPCGUID ,  vector<NPC_MoveBaseStruct>& List );

	// GM Tool EditFlag

	static void	EditFlagResult		( int CliID, int iResult );
	static void	DelFlagResult		( int CliID, int iResult );
	static void	DelFlagGroupResult	( int CliID, int iResult );

	static void	CliLogoutResult		( int CliID , bool Result );

	static void	S_ParalledID		( int CliID , int ParalledID );
    //-------------------------------------------------------------------
    //  事件繼承實做
    //-------------------------------------------------------------------
    virtual void OnLogin( ) = 0;
    virtual void CliConnect( int CliID , string Account) = 0;
    virtual void CliDisconnect( BaseItemObject* Player ) = 0;

    //Client 端要求登出由戲 (30 秒倒數)
    virtual void CliLogout( BaseItemObject* Player ) = 0;

    //有人要求換到此區域
    virtual void OnChangeZone ( PlayerRoleData* Player , int SrcZoneID ) = 0;

	virtual void OnChangeZoneNodifyResult( int ZoneID , int GItemID , float X , float Y , float Z , ChangeZoneNodifyResult_ENUM Result ) = 0;

   //要求修改物件
    virtual void OnModify( int CliID , ModifyNPCInfo* Info ) = 0;

	virtual void RC_GetNPCMoveInfoRequest( BaseItemObject* Player , int NpcGUID ) = 0;
	virtual void RC_SaveNPCMoveInfo( BaseItemObject* Player , int NpcGUID , int TotalCount , int IndexID , NPC_MoveBaseStruct& Base ) = 0;


	// GM Client 要求取得旗子資料
	virtual void RC_GetFlagList		( BaseItemObject* Player ) = 0;
	virtual void RC_EditFlag		( BaseItemObject* Player, int iFlagGroup, int iFlagID, float X, float Y, float Z, float Dir, int FlagDBID ) = 0;
	virtual void RC_DelFlag			( BaseItemObject* Player, int iFlagGroup, int iFlagID ) = 0;
	virtual void RC_DelFlagGroup	( BaseItemObject* Player, int iFlagGroup ) = 0;

	//////////////////////////////////////////////////////////////////////////
	static void	SBL_ChangeZoneToOtherWorld			( int ToWorldID , int ToGSrvID , PlayerRoleData& Role );
	static void	SBL_ChangeZoneToOtherWorldResult	( int ToWorldID , int ToNetID , int RoleDBID  , char* IPStr , int Port , ChangeZoneToOtherWorldResultENUM Result );
	static void	SBL_ConnectCliAndGSrv				( int ToWorldID , int ToNetID , int CliNetID  );
	static void	SBL_RetNomorlWorld					( int ToWorldID , int ToNetID , int PlayerDBID , RoleDataEx* Role , int OrgWorldGUID );
	static void	SBL_BattleSavePlayer				( int ToWorldID , int ToNetID , int PlayerDBID , RoleDataEx* Role , int OrgWorldGUID );
	static void	SC_ConnectToBattleWorld				( int SendToID , char* IP , int Port );
	static void	SC_WorldReturnNotify				( int SendToID , int ZoneID = -1 );

	virtual void RC_ConnectToBattleWorldOK	( BaseItemObject* Player , int CliNetID ) = 0;
	virtual void RBL_ChangeZoneToOtherWorld( int FromWorldId , int FromNetID , PlayerRoleData& Role , int RoleSie ) = 0;
	virtual void RBL_ChangeZoneToOtherWorldResult( int FromWorldId , int FromNetID , int RoleDBID , char* IPStr , int Port , ChangeZoneToOtherWorldResultENUM Result ) = 0;
	virtual void RBL_ConnectCliAndGSrv( int FromWorldId , int FromNetID , int CliNetID  ) = 0;
	virtual void RBL_RetNomorlWorld( int FromWorldId , int FromNetID , int PlayerDBID , RoleDataEx* Role ) = 0;
	virtual void RBL_BattleSavePlayer( int FromWorldId , int FromNetID , int PlayerDBID , RoleDataEx* Role ) = 0;

	virtual void RC_WorldReturnOK( BaseItemObject* Player ) = 0;  
	//////////////////////////////////////////////////////////////////////////
	//墓碑處理
	//////////////////////////////////////////////////////////////////////////
	virtual void RL_DelTomb( int TombGUID , int OwnerDBID ) = 0;
	static  void SL_DelTomb( int ZoneID , int TombGUID , int OwnerDBID );
	static  void SC_TouchTomb( int SendToID );
	static  void SC_TombInfo( int SendToID , TombStruct& Tomb );

	//////////////////////////////////////////////////////////////////////////
	//天空設定
	//////////////////////////////////////////////////////////////////////////
	static  void SC_SkyProcType( int SendToID , ClientSkyProcTypeENUM Type );

	//////////////////////////////////////////////////////////////////////////
	//其他world 轉給某個Client 封包轉送
	//////////////////////////////////////////////////////////////////////////
	static  void SBL_SendToZonePlayerByDBID( int WorldID, int ZoneID, int ClientDBID, int Size, void* Data );
	static  void SBL_SendToSrvPlayerByDBID( int WorldID, int SrvID, int ClientDBID, int Size, void* Data );

	virtual void RBL_SendToPlayerByDBID( int FromWorldId , int FromNetID , int PlayerDBID , int DataSize , char* Data ) = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual void OnLocalConnect( int ZoneID ) = 0;
	virtual void OnLocalDisconnect( int ZoneID ) = 0;
	static void SC_ZoneOpenState( int SendtoID , int ZoneID , bool IsOpen );
	static void SC_AllCli_ZoneOpenState( int ZoneID , bool IsOpen );
	//////////////////////////////////////////////////////////////////////////
	static void SL_BattleZone( int WorldID, int ZoneID , int PlayerDBID );
		virtual void RBL_BattleZone( int FromWorldId , int ZoneID , int FromNetID , int PlayerDBID ) = 0;
	static void SBL_ReturnWorldRequest( int WorldID, int ZoneID , int PlayerDBID , int TestCount );
		virtual void RL_ReturnWorldRequest( int FromWorldId , int FromNetID , int PlayerDBID , int TestCount ) = 0;
	static void SL_ReturnWorldResult( int WorldID, int NetID , int PlayerDBID , int TestCount , bool Result );
		virtual void RBL_ReturnWorldResult( int FromWorldId , int PlayerDBID , int TestCount , bool Result ) = 0;
};								

#endif //__NETWAKER_CLICONNECT_H__