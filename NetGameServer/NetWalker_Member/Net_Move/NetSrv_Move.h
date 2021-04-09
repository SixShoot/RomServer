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
    // ��ưe�X(����)
    //-------------------------------------------------------------------
    //�q�����a�Y���󲾰�
    static void Move( int SendToID , RoleDataEx* Item );
//	static void Move( int SendToID , int ItemID , float X , float Y , float Z , float Dir , float vX , float vY, float vZ , ClientMoveTypeENUM Type );


	//�q�����a�Y����S����
	static void SpecialMove( int SendToID , int TargetID , SpecialMoveTypeENUM	MoveType , RoleDataEx* Owner , int MagicCollectID );
	static void SpecialMove_BySockID( int SockID , int PoxyID , int TargetID , SpecialMoveTypeENUM	MoveType , RoleDataEx* Owner , int MagicCollectID );

    //�]�w�@���㹳����
    static void AddObject(   int SendToID , int GItemID , int DBID , int WorldGUID , int MasterGUID , CampID_ENUM CampID , int iWorldID , RolePosStruct& Pos , PlayerGraphStruct& Graph );

    //��Y�@�Ӫ��󲾰�
    static void DelObject( int SendToID , int GItemID  , int WorldGUID );

	//�]�w��ָ�T
	static void FlowerPotInfo( int SendToID , int GItemID , ItemFieldStruct& Item );


    //���]��m
    static void SetPos( int SendToID , RoleDataEx* Item );
	static void SetPos_AllCli( RoleDataEx* Item );

    //���ʧ@
    static void DoAction( int SendToID , int GItemID , int ActionID , int TempAction = 0 );

    //�]�w���ʳt��
    static void	SendMoveSpeed( int SendToID , RoleDataEx* Item );

	static void SendMoveInfo( int SendToID , RoleDataEx* Item );
    //�q���Y�@���aModel���Y����
    static void SendModelRate( int SendToID , RoleDataEx* Item );

    //�q���Y���a���˳��㹳���
    static void LookFace( int SendToID , RoleDataEx* Item );

    //�q���Y���a���˳��㹳���
    static void LookEQ( int SendToID , RoleDataEx* Item );

    //�q���Y���a���˳��㹳���
    //static void LookEQ( int SendToID , int ItemID , EQWearPosENUM Pos , int EQ );

    //�q��CliAutoPlot
    static void SendCliAuto( int SendToID , int GItemID , const  char* Plot );

    //�q������ثe���A
    static void SendActionType( int SendToID , int GItemID , RoleActionStruct	Action );

    //Server �q�� �P�򪱮a�Y����n�����V
    static void S_JumpBegin( int SendToID , int GItemID , float X , float Y , float Z , float Dir , float vX , float vY , float vZ );

    //Server �q�� Client���V����
    static void S_JumpEnd  ( int SendToID , int GItemID , float X , float Y , float Z , float Dir);

    //�q���Y����P���a�����Y
    static void S_Relation( int SendID , int TargetID , RoleRelationStruct R );

	//�q��Party�����ۤv����m
	static void S_PartyMemberPos( int SendToDBID , int DBID , int GItemID , float X , float Y , float Z );

	// Server �q�� NPC �P�򪺤H NPC ����ʧ@
	static void S_SetNPCMotion( BaseItemObject* pObj, int iMotionID , int iIdleMotionID );
	static void S_PlayerMotion( int SendToID , int ObjID , int MotionID );


	// Server �q�� NPC �P�򪺤H NPC ���m�ɰʧ@
	static void S_SetNPCIdleMotion( BaseItemObject* pObj, int iMotionID );

	//�^�ePing
	static void S_Ping( int SockID , int RetTime );

	//�q��Server�{�wClient�ݭnLoading ����Client �^��
	static void	S_ClientLoading( int SendID , float CameraX , float CameraY , float CameraZ );

	//�o�e�� Client, �S�w
	static void SendObjMinimapIcon ( int SendID, int iIconID );

	//����S�ĳq��
	static void S_SpecialEfficacy ( int SendID , int GItemID , SpecialEfficacyTypeENUM EffectType );

	//�]�w��V
	static void S_SetDir( int SendID , int GItemID , float Dir );

	//�]�w���⤽�|ID
	static void S_SetGuildID( int SendID , int GItemID , int GuildID );
	//�]�w�����Y��ID
	static void S_SetTitleID( int SendID , int GItemID , bool IsShowTitle , int TitleID , const char* TitleStr );

	static void S_PlayerDropDownDamage( int SendID , int Damage , int Type );

	static void S_ObjGuildBuildingInfo( int SendID , int GItemID , int BuildingDBID , int BuildingParentDBID , int BuildingID , int Dir , const char* Point );

	static void S_ObjGuildFurnitureInfo( int SendID , int GItemID , int FurnitureDBID );

	//�e�y�M�C��
	static void S_HorseColor( int SendID , int GItemID , int Color[4] );

	//�q���ҦA��RoomID
	static void S_RoomID( int SendID , int RoomID );

	//PingLog
	static void S_All_PingLog( int ID , int Time );

	static void S_DeadZone( int SendID , ZoneTypeENUM Type , int DeadCountDown );

	static void S_AttachObj( int SendID , int GItemID , int AttachItemID , int AttachType , const char* ItemPos , const char* AttachPos , int Pos );
	static void S_DetachObj( int SendID , int GItemID );
    //-------------------------------------------------------------------
    //  �ƥ��~�ӹ갵
    //-------------------------------------------------------------------
    virtual	void PlayerMove( BaseItemObject* Sender , int MoveID  
                           , float X , float Y , float Z , float Dir , ClientMoveTypeENUM Type , float vX , float vY , float vZ , int AttachObjID ) = 0;

    virtual void R_DoAction( BaseItemObject* Sender , int GItemID , int ActionID , int TempAction ) = 0;

    virtual void R_JumpBegin( BaseItemObject* Sender , float X , float Y , float Z , float Dir , float vX , float vY , float vZ ) = 0;
    virtual void R_JumpEnd( BaseItemObject* Sender , float X , float Y , float Z , float Dir , int AttackObjID ) = 0;

	//Client �����Ҹ��J�����A�⨤��[�J���ΰϤ�
	virtual void R_ClientLoadingOK( BaseItemObject* Sender ) = 0;

	virtual void R_Ping( BaseItemObject* Sender , int Time ) = 0;
	virtual void R_PingLog( BaseItemObject* Player , int ID , int Time , int ClietNetProcTime ) = 0;  

	//-----------------------------------------------------------------------------------------
	//�Գ��B�~���ʥ]
	//-----------------------------------------------------------------------------------------
	//�s�إߪ����a����
	static void S_BattleGround_AddPlayer( int SendID , RoleDataEx* Role );
	//�R�������a����
	static void S_BattleGround_DelPlayer( int SendID , int GItemID );
	//�s�إߪ����a����
	static void S_AllCli_BattleGround_AddPlayer( RoleDataEx* Role );
	//�R�������a����
	static void S_AllCli_BattleGround_DelPlayer( int GItemID );

	//�w�� ��m����s
	static void S_AllCli_BattleGround_PlayerPos( RoleDataEx* Role );
	//�w�� ��q����s
	static void S_AllCli_BattleGround_PlayerHPMP( RoleDataEx* Role );

	//////////////////////////////////////////////////////////////////////////
	//�d�I�B�z
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
	//�q���Ҧ����a ���ϰ쪱�a���������
	static void SC_BattleGround_PlayerInfo_Zip( int RoomID , vector<BattleGroundPlayerHPMPBaseStruct>& List );
	static void SC_FixPlayerPos( vector<RoleDataEx*>& toList , vector<RoleDataEx*>& moveList );
};

#endif //__NETWAKER_MOVE_H__