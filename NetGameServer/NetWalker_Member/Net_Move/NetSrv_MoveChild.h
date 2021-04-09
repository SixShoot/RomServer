#ifndef __NETWAKER_MOVECHILD_H__
#define __NETWAKER_MOVECHILD_H__

#include "NetSrv_Move.h"

class NetSrv_MoveChild : public NetSrv_Move
{
    //送兩者戶相看到的資料
    static void _SendAddPartInfo( BaseItemObject* OwnerObj , BaseItemObject* TargetObj , PlayerGraphStruct*	OwnerGraph );
	static void _SendDelPartInfo( BaseItemObject* OwnerObj , BaseItemObject* TargetObj );

	static void _SendEnterViewInfo( BaseItemObject* AllZoneObj , BaseItemObject* OwnerObj );
	static void _SendLeaveViewInfo( BaseItemObject* AllZoneObj , BaseItemObject* OwnerObj );

	static void _Send_GuildBuildingInfo( RoleDataEx* Owner , RoleDataEx* Target );
	
	static int  _UnlockRolePosSchedular( SchedularInfo* Obj , void* InputClass );

	static void _CheckDeadZone( BaseItemObject* OwnerObj , RoleDataEx* Owner , float X , float Y , float Z );
public:
    static bool Init();
    static bool Release();

    //-------------------------------------------------------------------
    //  事件繼承實做
    //-------------------------------------------------------------------
    void PlayerMove( BaseItemObject* Sender , int MoveID  
                    , float X , float Y , float Z , float Dir , ClientMoveTypeENUM Type , float vX , float vY , float vZ , int AttachObjID );

    virtual void R_DoAction( BaseItemObject* Sender , int GItemID , int ActionID , int TempAction  );

    virtual void R_JumpBegin( BaseItemObject* Sender , float X , float Y , float Z , float Dir 
                            , float vX , float vY , float vZ );

    virtual void R_JumpEnd( BaseItemObject* Sender , float X , float Y , float Z , float Dir , int AttackObjID );

	//Client 端環境載入完成，把角色加入分割區內
	virtual void R_ClientLoadingOK( BaseItemObject* Sender );


	virtual void R_Ping( BaseItemObject* Sender , int Time );

	static void CreatePlayerGraph( RoleDataEx* pRole, PlayerGraphStruct* pGraph );

	virtual void R_PingLog( BaseItemObject* Player , int ID , int Time , int ClietNetProcTime );  

	virtual void R_UnlockRolePosRequest( BaseItemObject* Player , float X , float Y , float Z , bool Cancel );  

	virtual void R_AttachObjRequest	( BaseItemObject* Player , int GItemID , int WagonItemID , AttachObjRequestENUM Type  , const char* ItemPos , const char* WagonPos , int AttachType );

	virtual void R_SetPartition			( BaseItemObject* Player , float X , float Y , float Z );
    //-------------------------------------------------------------------
    //物件一動
    static void MoveObj( BaseItemObject* OwnerObj );

	//物件一動
	static void MoveObj( RoleDataEx* Owner );

	//物件一動
	static void SendRangeMove( RoleDataEx* Owner , ClientMoveTypeENUM Type );


	//物件一動
	static void SendRange_SpecialMove( int TargetID , SpecialMoveTypeENUM MoveType , RoleDataEx* Owner , int MagicCollectID );

	//通知某一玩家Model的縮放比例
	static void SendRange_ModelRate( int SendToID  );


    //重設某物件的位置
    static bool SetObjPos( BaseItemObject* OwnerObj ,  float X , float Y , float Z , float Dir );

    //換Roomid
    static bool ChangeRoom( BaseItemObject* OwnerObj , int RoomId );

    //物件所在區塊更新
    static bool	MovePartition( BaseItemObject* OwnerObj );

    //一般移動
    static bool NormalMove( BaseItemObject* OwnerObj );

    //加入一個物件
    static bool	AddToPartition( BaseItemObject* OwnerObj );

    //刪除一個物件
    static bool	DelFromPartition( BaseItemObject* OwnerObj );
    
    //通知週圍的人裝備資訊
    static void Send_LookEq( RoleDataEx* Owner );

    //通知週圍的人裝備資訊
    static void SendRange_ActionType( RoleDataEx* Owner );

    //設定移動速度
    static void	SendRange_MoveSpeed( RoleDataEx* Owner );
	static void	SendRange_MoveInfo( RoleDataEx* Owner );

    //送兩人的關係
    static void Send_Relation( RoleDataEx* Owner , RoleDataEx* Target );

    //送周圍的關係
    static void SendRange_Relation( RoleDataEx* Owner );

	//通知所有Party成員自己的位置
	static void SendAllPartyMemberPos( RoleDataEx* Owner );

	static void _Send_Relation( RoleDataEx* SendObj , RoleDataEx* Target );

	static void SetPosture( BaseItemObject* Sender , int GItemID , int ActionID , int TempAction );

	//(周圍) 角色特效通知
	static void SendRangeSpecialEfficacy ( int GItemID , SpecialEfficacyTypeENUM EffectType );

	//設定面向
	static void SendRangeSetDir( int GItemID , float Dir );

	//設定公會
	static void SendRangeSetGuildID( RoleDataEx* Owner );

	//設定面向
	static void SendRangeSetTitleID( RoleDataEx* Owner );

	//設定長像
	static void SendRangeLookFace( RoleDataEx* Owner );

	//設定長像
	static void SendRangeHorseColor( RoleDataEx* Owner );

	static void SendRangeAttackObj( RoleDataEx* Owner , int AttachItemID , int AttachType , const char* ItemPos , const char* AttachPos , int Pos );
	static void SendRangeDetachObj( RoleDataEx* Owner );

	static void AttachObjProc( int SendID , BaseItemObject* WagonObj );

	//發送所有玩家的位置
	static void SendAllPlayerPos( );

	static void SendAllBattlePlayerInfo( );

	//處理需要修正座標的玩家
	static void FixAllPlayerPosProc	( );
};

#endif //__NETWAKER_MOVECHILD_H__