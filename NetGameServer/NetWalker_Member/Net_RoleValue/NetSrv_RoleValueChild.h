#pragma once

#include "NetSrv_RoleValue.h"



class NetSrv_RoleValueChild : public NetSrv_RoleValue
{
	static int      _ClientLogoutSchedular( SchedularInfo* Obj , void* InputClass );    //
	static int      _ClientChangeParallelSchedular( SchedularInfo* Obj , void* InputClass );    //
public:
    static bool Init();
    static bool Release();

    //通知週圍的人升級
    static void NotifyLvUp( RoleDataEx*	Owner );

    //通知周圍玩家某一個人的頭銜資訊
//    static void SendRangeTitleInfo( RoleDataEx*	Owner );

    //(當等級上升時)通知周圍玩家某一個人的等級或換職業
	static void SendRangeRoleInfo( RoleDataEx*	Owner );

	//通知周圍的人某物件的mode
	static void SendRangeObjMode( RoleDataEx*	Owner );

	//通知周圍玩家 全區角色旗標
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

