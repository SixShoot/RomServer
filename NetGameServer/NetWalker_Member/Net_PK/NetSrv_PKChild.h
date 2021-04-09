#pragma  once

#include "NetSrv_PK.h"

class NetSrv_PKChild : public NetSrv_PK
{
	static int  _RolePKBeginState( SchedularInfo* Obj , void* InputClass );		//ÀË¬dPKª¬ºA
	static int  _RolePKPrepareState( SchedularInfo* Obj , void* InputClass );    //ÀË¬dPKª¬ºA

	
public:
    static bool Init();
    static bool Release();

	virtual void R_PKInviteRequest	( BaseItemObject* OwnerObj , int TargetID );
	virtual void R_PKInviteResult	( BaseItemObject* OwnerObj , int TargetID , bool Result );
	virtual void R_PKCancel			( BaseItemObject* OwnerObj );


	static void	SetWinLost( RoleDataEx* Winner , RoleDataEx* Loser );
};

