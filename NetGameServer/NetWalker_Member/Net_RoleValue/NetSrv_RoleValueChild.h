#pragma once

#include "NetSrv_RoleValue.h"



class NetSrv_RoleValueChild : public NetSrv_RoleValue
{
	static int      _ClientLogoutSchedular( SchedularInfo* Obj , void* InputClass );    //
	static int      _ClientChangeParallelSchedular( SchedularInfo* Obj , void* InputClass );    //
public:
    static bool Init();
    static bool Release();

    //�q���g�򪺤H�ɯ�
    static void NotifyLvUp( RoleDataEx*	Owner );

    //�q���P�򪱮a�Y�@�ӤH���Y�θ�T
//    static void SendRangeTitleInfo( RoleDataEx*	Owner );

    //(���ŤW�ɮ�)�q���P�򪱮a�Y�@�ӤH�����ũδ�¾�~
	static void SendRangeRoleInfo( RoleDataEx*	Owner );

	//�q���P�򪺤H�Y����mode
	static void SendRangeObjMode( RoleDataEx*	Owner );

	//�q���P�򪱮a ���Ϩ���X��
	static void SendRangeSetPlayerAllZoneState( RoleDataEx* Owner );
    //-----------------------------------------------------------------------------------------------------
    virtual void R_RequestFixRoleValue   ( BaseItemObject*	Obj );
    virtual void R_RequestSetPoint       ( BaseItemObject*	Obj , int STR , int STA , int INT , int MND , int AGI );
    virtual void R_SetTitlRequest        ( BaseItemObject*	Obj , int TitleID , bool IsShowTitle );

	virtual void R_CloseChangeJobRequest ( BaseItemObject*	Obj  );
	virtual void R_ChangeJobRequest		 ( BaseItemObject*	Obj , int Job , int Job_Sub );

	virtual void R_ClientLogout			 ( BaseItemObject*	Obj );
	virtual void R_CancelClientLogout	 ( BaseItemObject*  Obj );
	virtual void R_SetPlayerAllZoneState ( BaseItemObject*	Obj , PlayerStateStruct& State );
	virtual void R_SetClientFlag		 ( BaseItemObject*	Obj , StaticFlag<_MAX_CLIENT_FLAG_COUNT_>& ClientFlag );

	virtual void R_ChangeParallel			( BaseItemObject*	Obj , int ParallelID );
	virtual void R_CancelChangeParallel		( BaseItemObject*	Obj );
	virtual void R_ClientComputerInfo		( BaseItemObject*	Obj , ClientComputerInfoStruct& Info );
	virtual void R_SetClientData			( BaseItemObject*	Obj , char* ClientData );
	virtual void R_ClientEnvironmentState	( BaseItemObject*	Obj , ClientStateStruct& State );
};

