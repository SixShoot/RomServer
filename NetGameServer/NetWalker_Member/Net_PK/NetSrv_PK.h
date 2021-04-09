#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_PK.h"

class NetSrv_PK : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_PK* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------
	static void _PG_PK_CtoL_PKInviteRequest		( int Sockid , int Size , void* Data );		
	static void _PG_PK_CtoL_PKInviteResult		( int Sockid , int Size , void* Data );		
	static void _PG_PK_CtoL_PKCancel			( int Sockid , int Size , void* Data );			

public:    
	static void SC_PKInviteNotify	( int SendID , int TargetID , int PKScoure );			
	static void SC_PKInviteResult	( int SendID , int TargetID , bool Result );		
	static void SC_PKPrepare		( int SendID , int Player1ID , int Player2ID , int Player1Score , int Player2Score , int X , int Y , int Z , int Range , int Time );
	static void SC_PKBegin			( int SendID , int Time );			
	static void SC_PKEnd			( int SendID , int WinnerID , int LoserID , bool IsEven );	
	static void SC_PKCancel			( int SendID );	

	virtual void R_PKInviteRequest	( BaseItemObject* OwnerObj , int TargetID ) = 0;
	virtual void R_PKInviteResult	( BaseItemObject* OwnerObj , int TargetID , bool Result ) = 0;
	virtual void R_PKCancel			( BaseItemObject* OwnerObj ) = 0;

};

