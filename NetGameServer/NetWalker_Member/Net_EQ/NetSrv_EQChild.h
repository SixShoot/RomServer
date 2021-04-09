#pragma once

#include "NetSrv_EQ.h"

class NetSrv_EQChild : public NetSrv_EQ
{
public:
    static bool Init();
    static bool Release();

//	static void SendRangeShowEQInfo( RoleDataEx* Owner );
//    virtual void R_SetEQ( BaseItemObject* Obj , ItemFieldStruct& Item , int ItemPos , EQWearPosENUM EQPos );       
    virtual void R_SetEQ_Body( BaseItemObject* Obj , ItemFieldStruct& Item , int ItemPos , EQWearPosENUM EQPos , bool IsFromEq );
    virtual void R_SetEQ_Bank( BaseItemObject* Obj , ItemFieldStruct& Item , int ItemPos , EQWearPosENUM EQPos , bool IsFromEq );       
    virtual void R_SetEQ_EQ( BaseItemObject* Obj , ItemFieldStruct& Item , int ItemPos , EQWearPosENUM EQPos );       
	virtual void R_SetShowEQ( BaseItemObject* Obj , ShowEQStruct ShowEQ );
	virtual void R_SwapBackupEQ	( BaseItemObject* Obj , int bkPosID );

	//////////////////////////////////////////////////////////////////////////
	//®M¸Ë§Þ¯à
	virtual void R_LearnSuitSkill_Close	( BaseItemObject* Obj );
	virtual void R_LearnSuitSkillRequest( BaseItemObject* Obj , int ItemCount , int ItemPos[10] , int LearnSkillID );
	virtual void R_SetSuitSkillRequest	( BaseItemObject* Obj , int SkillID , int Pos );
	virtual void R_UseSuitSkillRequest	( BaseItemObject* Obj , int SkillID , int TargetID , float PosX , float PosY , float PosZ );
};

