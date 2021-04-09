#pragma once

#include "../../MainProc/Global.h"
#include "PG/PG_Mail.h"

class NetSrv_Mail : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_Mail* This;
    static bool _Init();
    static bool _Release();
    //-------------------------------------------------------------------

	static void _PG_Mail_CtoL_SendMail				( int Sockid , int Size , void* Data );
	static void _PG_Mail_CtoL_MailList				( int Sockid , int Size , void* Data );
	static void _PG_Mail_CtoL_SendBack				( int Sockid , int Size , void* Data );
	static void _PG_Mail_CtoL_DelMail				( int Sockid , int Size , void* Data );
	static void _PG_Mail_CtoL_ReadMail				( int Sockid , int Size , void* Data );
	static void _PG_Mail_CtoL_GetItem				( int Sockid , int Size , void* Data );

	static void _PG_Mail_DtoL_SendMailResult		( int Sockid , int Size , void* Data );
	static void _PG_Mail_DtoL_GetItemResult			( int Sockid , int Size , void* Data );

	static void _PG_Mail_CtoL_CloseMail				( int Sockid , int Size , void* Data );
	static void _PG_Mail_DtoL_CheckSendMailResult	( int Sockid , int Size , void* Data );
	static void _PG_Mail_DtoL_CheckGetItemResult	( int Sockid , int Size , void* Data );
	static void _PG_Mail_CtoL_SendMailList			( int Sockid , int Size , void* Data );
public:    

	static void SC_SendMailResult	( int SendID , SendMailResultEnum Result , const char* Name );
	static void SC_GetItemResult	( int SendID , int MailDBID , bool Result );

	static void SC_OpenMail			( int SendID , int NpcGUID );
	static void SC_CloseMail		( int SendID  );
//	static void SC_NewMail			( char* RoleName  );


	static void SD_CheckSendMail	( int PlayerDBID , const char* TargetName , int MapKey );
	static void SD_SendMail			( int DBID , char* Content , MailBaseInfoStruct& MailBaseInfo , int TargetDBID = -1 );
	static void SD_MailList			( int DBID , int CliSockID , int CliProxyID );
	static void SD_SendBack			( int DBID , int MailDBID );
	static void SD_DelMail			( int DBID , int MailDBID );
	static void SD_ReadMail			( int DBID , int MailDBID  );
	static void SD_GetItem			( int DBID , int MailDBID , int Money , int Money_Account , int Money_Bonus );
	static void SD_UnReadMailCountRequest( int DBID , int CliSockID , int CliProxyID );

	static void SD_CheckGetItemRequest( int DBID , int MailDBID , int Money , int Money_Account , int Money_Bonus );


	virtual void RC_SendMail	( BaseItemObject* OwnerObj , MailBaseInfoStruct& MailBaseInfo , int ItemPos[10] , char* Content , char* Password ) = 0;
	virtual void RC_MailList	( BaseItemObject* OwnerObj ) = 0;
	virtual void RC_SendBack	( BaseItemObject* OwnerObj , int MailDBID ) = 0;
	virtual void RC_DelMail		( BaseItemObject* OwnerObj , int MailDBID ) = 0;
	virtual void RC_ReadMail	( BaseItemObject* OwnerObj , int MailDBID ) = 0;
	virtual void RC_GetItem		( BaseItemObject* OwnerObj , int MailDBID , int Money , int Money_Account , int Money_Bonus , const char* Password ) = 0;
	virtual void RC_CloseMail	( BaseItemObject* OwnerObj ) = 0;

	virtual void RD_SendMailResult	( BaseItemObject* OwnerObj , bool IsSendMoney , bool IsSystem 
									, int Money , int Money_Account , int Money_Bonus , SendMailResultEnum Result , const char* TargetName ) = 0;	
	virtual void RD_GetItemResult	( BaseItemObject* OwnerObj , int DBID , int OrgSenderDBID , int MailDBID , int Money , int Money_Account , int Money_Bonus , ItemFieldStruct Item[10] , bool Result  ) = 0;

	virtual void RD_CheckSendMailResult( int PlayerDBID , char* TargetName , int TargetDBID , int MapKey , bool Result , int ErrorCode ) = 0;
	virtual void RD_CheckGetItemResult( int DBID , int TargetDBID , int MailDBID , int Money , int Money_Account , int Money_Bonus , bool Result ) = 0;
	virtual void RC_SendMailList( BaseItemObject* Obj , int TargetCount , StaticString <_MAX_NAMERECORD_SIZE_>	OrgTargetName[20] 
							, const char* Title , MailBackGroundTypeENUM PaperType , const char* Content ) = 0;
};

