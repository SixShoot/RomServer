#ifndef __NETWAKER_MOVE_H__
#define __NETWAKER_MOVE_H__

#include "../../MainProc/Global.h"
#include "PG/PG_Move.h"

class NetSrv_Move : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Move* This;
    static bool _Init();
    static bool _Release();

    //-------------------------------------------------------------------
    static void _PG_Move_CtoL_PlayerMoveObject	( int Sockid , int Size , void* Data );    
    static void _PG_Move_CtoL_DoAction        	( int Sockid , int Size , void* Data );
    static void _PG_Move_CtoL_JumpBegin       	( int Sockid , int Size , void* Data );
    static void _PG_Move_CtoL_JumpEnd         	( int Sockid , int Size , void* Data );
	static void _PG_Move_CtoL_Ping	          	( int Sockid , int Size , void* Data );

	static void _PG_Move_CtoL_ClientLoadingOK 	( int Sockid , int Size , void* Data );
	static void _PG_Move_CtoL_PingLog		  	( int Sockid , int Size , void* Data );

	static void _PG_Move_CtoL_UnlockRolePosRequest( int Sockid , int Size , void* Data );

	static void _PG_Move_CtoL_AttachObjRequest	( int Sockid , int Size , void* Data );
	static void _PG_Move_CtoL_SetPartition		( int Sockid , int Size , void* Data );
public:    
    //-------------------------------------------------------------------
    // 資料送出(測試)
    //-------------------------------------------------------------------
    //通知玩家某物件移動
    static void Move( int SendToID , RoleDataEx* Item );
//	static void Move( int SendToID , int ItemID , float X , float Y , float Z , float Dir , float vX , float vY, float vZ , ClientMoveTypeENUM Type );


	//通知玩家某物件特殊移動
	static void SpecialMove( int SendToID , int TargetID , SpecialMoveTypeENUM	MoveType , RoleDataEx* Owner , int MagicCollectID );
	static void SpecialMove_BySockID( int SockID , int PoxyID , int TargetID , SpecialMoveTypeENUM	MoveType , RoleDataEx* Owner , int MagicCollectID );

    //設定一個顯像物件
    static void AddObject(   int SendToID , int GItemID , int DBID , int WorldGUID , int MasterGUID , CampID_ENUM CampID , int iWorldID , RolePosStruct& Pos , PlayerGraphStruct& Graph );

    //把某一個物件移除
    static void DelObject( int SendToID , int GItemID  , int WorldGUID );

	//設定花盆資訊
	static void FlowerPotInfo( int SendToID , int GItemID , ItemFieldStruct& Item );


    //重設位置
    static void SetPos( int SendToID , RoleDataEx* Item );
	static void SetPos_AllCli( RoleDataEx* Item );

    //做動作
    static void DoAction( int SendToID , int GItemID , int ActionID , int TempAction = 0 );

    //設定移動速度
    static void	SendMoveSpeed( int SendToID , RoleDataEx* Item );

	static void SendMoveInfo( int SendToID , RoleDataEx* Item );
    //通知某一玩家Model的縮放比例
    static void SendModelRate( int SendToID , RoleDataEx* Item );

    //通知某玩家的裝備顯像資料
    static void LookFace( int SendToID , RoleDataEx* Item );

    //通知某玩家的裝備顯像資料
    static void LookEQ( int SendToID , RoleDataEx* Item );

    //通知某玩家的裝備顯像資料
    //static void LookEQ( int SendToID , int ItemID , EQWearPosENUM Pos , int EQ );

    //通知CliAutoPlot
    static void SendCliAuto( int SendToID , int GItemID , const  char* Plot );

    //通知物件目前狀態
    static void SendActionType( int SendToID , int GItemID , RoleActionStruct	Action );

    //Server 通知 周圍玩家某角色要做跳越
    static void S_JumpBegin( int SendToID , int GItemID , float X , float Y , float Z , float Dir , float vX , float vY , float vZ );

    //Server 通知 Client跳越結束
    static void S_JumpEnd  ( int SendToID , int GItemID , float X , float Y , float Z , float Dir);

    //通知某物件與玩家的關係
    static void S_Relation( int SendID , int TargetID , RoleRelationStruct R );

	//通知Party成員自己的位置
	static void S_PartyMemberPos( int SendToDBID , int DBID , int GItemID , float X , float Y , float Z );

	// Server 通知 NPC 周圍的人 NPC 播放動作
	static void S_SetNPCMotion( BaseItemObject* pObj, int iMotionID , int iIdleMotionID );
	static void S_PlayerMotion( int SendToID , int ObjID , int MotionID );


	// Server 通知 NPC 周圍的人 NPC 閒置時動作
	static void S_SetNPCIdleMotion( BaseItemObject* pObj, int iMotionID );

	//回送Ping
	static void S_Ping( int SockID , int RetTime );

	//通知Server認定Client需要Loading 等待Client 回應
	static void	S_ClientLoading( int SendID , float CameraX , float CameraY , float CameraZ );

	//發送給 Client, 特定
	static void SendObjMinimapIcon ( int SendID, int iIconID );

	//角色特效通知
	static void S_SpecialEfficacy ( int SendID , int GItemID , SpecialEfficacyTypeENUM EffectType );

	//設定方向
	static void S_SetDir( int SendID , int GItemID , float Dir );

	//設定角色公會ID
	static void S_SetGuildID( int SendID , int GItemID , int GuildID );
	//設定角色頭銜ID
	static void S_SetTitleID( int SendID , int GItemID , bool IsShowTitle , int TitleID , const char* TitleStr );

	static void S_PlayerDropDownDamage( int SendID , int Damage , int Type );

	static void S_ObjGuildBuildingInfo( int SendID , int GItemID , int BuildingDBID , int BuildingParentDBID , int BuildingID , int Dir , const char* Point );

	static void S_ObjGuildFurnitureInfo( int SendID , int GItemID , int FurnitureDBID );

	//送座騎顏色
	static void S_HorseColor( int SendID , int GItemID , int Color[4] );

	//通知所再的RoomID
	static void S_RoomID( int SendID , int RoomID );

	//PingLog
	static void S_All_PingLog( int ID , int Time );

	static void S_DeadZone( int SendID , ZoneTypeENUM Type , int DeadCountDown );

	static void S_AttachObj( int SendID , int GItemID , int AttachItemID , int AttachType , const char* ItemPos , const char* AttachPos , int Pos );
	static void S_DetachObj( int SendID , int GItemID );
    //-------------------------------------------------------------------
    //  事件繼承實做
    //-------------------------------------------------------------------
    virtual	void PlayerMove( BaseItemObject* Sender , int MoveID  
                           , float X , float Y , float Z , float Dir , ClientMoveTypeENUM Type , float vX , float vY , float vZ , int AttachObjID ) = 0;

    virtual void R_DoAction( BaseItemObject* Sender , int GItemID , int ActionID , int TempAction ) = 0;

    virtual void R_JumpBegin( BaseItemObject* Sender , float X , float Y , float Z , float Dir , float vX , float vY , float vZ ) = 0;
    virtual void R_JumpEnd( BaseItemObject* Sender , float X , float Y , float Z , float Dir , int AttackObjID ) = 0;

	//Client 端環境載入完成，把角色加入分割區內
	virtual void R_ClientLoadingOK( BaseItemObject* Sender ) = 0;

	virtual void R_Ping( BaseItemObject* Sender , int Time ) = 0;
	virtual void R_PingLog( BaseItemObject* Player , int ID , int Time , int ClietNetProcTime ) = 0;  

	//-----------------------------------------------------------------------------------------
	//戰場額外的封包
	//-----------------------------------------------------------------------------------------
	//新建立的玩家角色
	static void S_BattleGround_AddPlayer( int SendID , RoleDataEx* Role );
	//刪除的玩家角色
	static void S_BattleGround_DelPlayer( int SendID , int GItemID );
	//新建立的玩家角色
	static void S_AllCli_BattleGround_AddPlayer( RoleDataEx* Role );
	//刪除的玩家角色
	static void S_AllCli_BattleGround_DelPlayer( int GItemID );

	//定時 位置的更新
	static void S_AllCli_BattleGround_PlayerPos( RoleDataEx* Role );
	//定時 血量的更新
	static void S_AllCli_BattleGround_PlayerHPMP( RoleDataEx* Role );

	//////////////////////////////////////////////////////////////////////////
	//卡點處理
	virtual void R_UnlockRolePosRequest	( BaseItemObject* Player , float X , float Y , float Z , bool Cancel ) = 0;  
	static void S_UnlockRolePosTime		( int GItemID , int Time );
	static void S_UnlockRolePosOK		( int GItemID , bool Result );


	//////////////////////////////////////////////////////////////////////////
	virtual void R_AttachObjRequest		( BaseItemObject* Player , int GItemID , int WagonItemID , AttachObjRequestENUM Type  , const char* ItemPos , const char* WagonPos , int AttachType ) = 0;
	static void SC_AttachObjResult		( int GItemID , AttachObjRequestENUM Type , AttachObjResultENUM Result , int Pos );
	static void SC_ColdownInfo			( int SendToID , int GItemID , MagicColdownStruct& Coldown );

	static void SC_PlayerPos			( vector<PlayerBaseInfoStruct> List );
	//////////////////////////////////////////////////////////////////////////
	virtual void R_SetPartition			( BaseItemObject* Player , float X , float Y , float Z ) = 0;  

	//////////////////////////////////////////////////////////////////////////
	//通知所有玩家 此區域玩家的相關資料
	static void SC_BattleGround_PlayerInfo_Zip( int RoomID , vector<BattleGroundPlayerHPMPBaseStruct>& List );
	static void SC_FixPlayerPos( vector<RoleDataEx*>& toList , vector<RoleDataEx*>& moveList );
};

#endif //__NETWAKER_MOVE_H__