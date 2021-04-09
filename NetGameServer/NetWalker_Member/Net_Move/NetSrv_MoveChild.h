#ifndef __NETWAKER_MOVECHILD_H__
#define __NETWAKER_MOVECHILD_H__

#include "NetSrv_Move.h"

class NetSrv_MoveChild : public NetSrv_Move
{
    //�e��̤�۬ݨ쪺���
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
    //  �ƥ��~�ӹ갵
    //-------------------------------------------------------------------
    void PlayerMove( BaseItemObject* Sender , int MoveID  
                    , float X , float Y , float Z , float Dir , ClientMoveTypeENUM Type , float vX , float vY , float vZ , int AttachObjID );

    virtual void R_DoAction( BaseItemObject* Sender , int GItemID , int ActionID , int TempAction  );

    virtual void R_JumpBegin( BaseItemObject* Sender , float X , float Y , float Z , float Dir 
                            , float vX , float vY , float vZ );

    virtual void R_JumpEnd( BaseItemObject* Sender , float X , float Y , float Z , float Dir , int AttackObjID );

	//Client �����Ҹ��J�����A�⨤��[�J���ΰϤ�
	virtual void R_ClientLoadingOK( BaseItemObject* Sender );


	virtual void R_Ping( BaseItemObject* Sender , int Time );

	static void CreatePlayerGraph( RoleDataEx* pRole, PlayerGraphStruct* pGraph );

	virtual void R_PingLog( BaseItemObject* Player , int ID , int Time , int ClietNetProcTime );  

	virtual void R_UnlockRolePosRequest( BaseItemObject* Player , float X , float Y , float Z , bool Cancel );  

	virtual void R_AttachObjRequest	( BaseItemObject* Player , int GItemID , int WagonItemID , AttachObjRequestENUM Type  , const char* ItemPos , const char* WagonPos , int AttachType );

	virtual void R_SetPartition			( BaseItemObject* Player , float X , float Y , float Z );
    //-------------------------------------------------------------------
    //����@��
    static void MoveObj( BaseItemObject* OwnerObj );

	//����@��
	static void MoveObj( RoleDataEx* Owner );

	//����@��
	static void SendRangeMove( RoleDataEx* Owner , ClientMoveTypeENUM Type );


	//����@��
	static void SendRange_SpecialMove( int TargetID , SpecialMoveTypeENUM MoveType , RoleDataEx* Owner , int MagicCollectID );

	//�q���Y�@���aModel���Y����
	static void SendRange_ModelRate( int SendToID  );


    //���]�Y���󪺦�m
    static bool SetObjPos( BaseItemObject* OwnerObj ,  float X , float Y , float Z , float Dir );

    //��Roomid
    static bool ChangeRoom( BaseItemObject* OwnerObj , int RoomId );

    //����Ҧb�϶���s
    static bool	MovePartition( BaseItemObject* OwnerObj );

    //�@�벾��
    static bool NormalMove( BaseItemObject* OwnerObj );

    //�[�J�@�Ӫ���
    static bool	AddToPartition( BaseItemObject* OwnerObj );

    //�R���@�Ӫ���
    static bool	DelFromPartition( BaseItemObject* OwnerObj );
    
    //�q���g�򪺤H�˳Ƹ�T
    static void Send_LookEq( RoleDataEx* Owner );

    //�q���g�򪺤H�˳Ƹ�T
    static void SendRange_ActionType( RoleDataEx* Owner );

    //�]�w���ʳt��
    static void	SendRange_MoveSpeed( RoleDataEx* Owner );
	static void	SendRange_MoveInfo( RoleDataEx* Owner );

    //�e��H�����Y
    static void Send_Relation( RoleDataEx* Owner , RoleDataEx* Target );

    //�e�P�����Y
    static void SendRange_Relation( RoleDataEx* Owner );

	//�q���Ҧ�Party�����ۤv����m
	static void SendAllPartyMemberPos( RoleDataEx* Owner );

	static void _Send_Relation( RoleDataEx* SendObj , RoleDataEx* Target );

	static void SetPosture( BaseItemObject* Sender , int GItemID , int ActionID , int TempAction );

	//(�P��) ����S�ĳq��
	static void SendRangeSpecialEfficacy ( int GItemID , SpecialEfficacyTypeENUM EffectType );

	//�]�w���V
	static void SendRangeSetDir( int GItemID , float Dir );

	//�]�w���|
	static void SendRangeSetGuildID( RoleDataEx* Owner );

	//�]�w���V
	static void SendRangeSetTitleID( RoleDataEx* Owner );

	//�]�w����
	static void SendRangeLookFace( RoleDataEx* Owner );

	//�]�w����
	static void SendRangeHorseColor( RoleDataEx* Owner );

	static void SendRangeAttackObj( RoleDataEx* Owner , int AttachItemID , int AttachType , const char* ItemPos , const char* AttachPos , int Pos );
	static void SendRangeDetachObj( RoleDataEx* Owner );

	static void AttachObjProc( int SendID , BaseItemObject* WagonObj );

	//�o�e�Ҧ����a����m
	static void SendAllPlayerPos( );

	static void SendAllBattlePlayerInfo( );

	//�B�z�ݭn�ץ��y�Ъ����a
	static void FixAllPlayerPosProc	( );
};

#endif //__NETWAKER_MOVECHILD_H__