#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_EQ.h"

class NetSrv_EQ : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_EQ* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------

    static void _PG_EQ_CtoL_SetEQ					( int NetID , int Size , void* Data );
	static void _PG_EQ_CtoL_SetShowEQ				( int NetID , int Size , void* Data );
	static void _PG_EQ_CtoL_SwapBackupEQ			( int NetID , int Size , void* Data );
	static void _PG_EQ_CtoL_LearnSuitSkill_Close	( int NetID , int Size , void* Data );
	static void _PG_EQ_CtoL_LearnSuitSkillRequest	( int NetID , int Size , void* Data );
	static void _PG_EQ_CtoL_SetSuitSkillRequest		( int NetID , int Size , void* Data );
	static void _PG_EQ_CtoL_UseSuitSkillRequest		( int NetID , int Size , void* Data );

public:    

    static void S_SetEQ_OK_Body     ( int SendToID , ItemFieldStruct& Item , int ItemPos , ItemFieldStruct& EQ , int EQPos );
    static void S_SetEQ_OK_Bank     ( int SendToID , ItemFieldStruct& Item , int ItemPos , ItemFieldStruct& EQ , int EQPos );
    static void S_SetEQ_OK_EQ       ( int SendToID , ItemFieldStruct& Item , int ItemPos , ItemFieldStruct& EQ , int EQPos );
    static void S_SetEQ_Failed_Body ( int SendToID , ItemFieldStruct& Item , int ItemPos , ItemFieldStruct& EQ , int EQPos );
    static void S_SetEQ_Failed_Bank ( int SendToID , ItemFieldStruct& Item , int ItemPos , ItemFieldStruct& EQ , int EQPos );
    static void S_SetEQ_Failed_EQ   ( int SendToID , ItemFieldStruct& Item , int ItemPos , ItemFieldStruct& EQ , int EQPos );
	static void S_SetEQResult		( int SendToID , EQResult_ENUM Result );
	static void S_EQBroken			( int SendToID , EQWearPosENUM Pos );
	static void S_ShowEQInfo		( int SendToID , int GItemID , ShowEQStruct ShowEQ );
	static void S_SwapBackupEQ		( int SendToId , int BkPosID , bool Result );

    virtual void R_SetEQ_Body	( BaseItemObject* Obj , ItemFieldStruct& Item , int ItemPos , EQWearPosENUM EQPos , bool IsFromEq )  = 0;       
    virtual void R_SetEQ_Bank	( BaseItemObject* Obj , ItemFieldStruct& Item , int ItemPos , EQWearPosENUM EQPos , bool IsFromEq ) = 0;       
    virtual void R_SetEQ_EQ		( BaseItemObject* Obj , ItemFieldStruct& Item , int ItemPos , EQWearPosENUM EQPos ) = 0;       
	virtual void R_SetShowEQ	( BaseItemObject* Obj , ShowEQStruct ShowEQ ) = 0;
	virtual void R_SwapBackupEQ	( BaseItemObject* Obj , int bkPosID ) = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual void R_LearnSuitSkill_Close	( BaseItemObject* Obj ) = 0;
	virtual void R_LearnSuitSkillRequest( BaseItemObject* Obj , int ItemCount , int ItemPos[10] , int LearnSkillID ) = 0;
	virtual void R_SetSuitSkillRequest	( BaseItemObject* Obj , int SkillID , int Pos ) = 0;
	virtual void R_UseSuitSkillRequest	( BaseItemObject* Obj , int SkillID , int TargetID , float PosX , float PosY , float PosZ ) = 0;

	static void S_LearnSuitSkill_Open	( int SendToID , int TargetID );
	static void S_LearnSuitSkillResult	( int SendToID , int LearnSkillID , LearnSuitSkillResultENUM Result );
	static void S_SetSuitSkillResult	( int SendToID , SetSuitSkillResultENUM Result );
	static void S_UseSuitSkillResult	( int SendToID , UseSuitSkillResultENUM Result );
};

