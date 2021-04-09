
#pragma once
#include "PG/PG_Mail.h"
#include "../../MainProc/Global.h"

//送信件的暫時儲存資料
struct DBSendMailTempInfoStruct
{
	int		SrvSockID;
	int		DBID;
	MailBaseInfoStruct		MailBaseInfo;
	StaticString <2048>		Content;
};

class CNetDC_Mail : public Global
{
protected:
	//-------------------------------------------------------------------
	static CNetDC_Mail*	This;

	static bool				_Init();
	static bool				_Release();
	//-------------------------------------------------------------------	

	static void _PG_Mail_LtoD_SendMail	( int ServerID , int Size , void* Data );	
	static void _PG_Mail_LtoD_MailList	( int ServerID , int Size , void* Data );	
	static void _PG_Mail_LtoD_SendBack	( int ServerID , int Size , void* Data );	
	static void _PG_Mail_LtoD_DelMail	( int ServerID , int Size , void* Data );	
	static void _PG_Mail_LtoD_ReadMail	( int ServerID , int Size , void* Data );	
	static void _PG_Mail_LtoD_GetItem	( int ServerID , int Size , void* Data );	

	static void _PG_Mail_LtoD_UnReadMailCountRequest( int ServerID , int Size , void* Data );	

	static void _PG_Mail_LtoD_CheckSendMail			( int ServerID , int Size , void* Data );	
	static void _PG_Mail_LtoD_CheckGetItemRequest	( int ServerID , int Size , void* Data );	
public:
    //-------------------------------------------------------------------	


	static void SL_SendMailResult	( int SrvSockID , int DBID , int Money , int Money_Account , int Money_Bonus
									, bool IsSendMoney , bool IsSystem , SendMailResultEnum Result , const char* TargetName );

	static void SL_GetItemResult	( int SrvSockID , int DBID , int OrgSenderDBID , int MailDBID , bool Result , int Money , int Money_Account , int Money_Bonus , ItemFieldStruct Item[10] );

	static void SL_CheckSendMailResult( int SrvSockID , int PlayerDBID , char* TargetName , int TargetDBID , int MapKey , bool Result , int ErrCode );
	static void SL_CheckGetItemResult(  int SrvSockID , int	DBID , int TargetDBID ,	int MailDBID , int Money , int Money_Account , int Money_Bonus , bool	Result );	

	static void SC_MailListResult	( int CliSockID , int CliProxyID , int MailDBID , int Index 
									, MailBaseInfoStruct&  MailBaseInfo , char* Content  );
	static void SC_MailListCountResult	( int CliSockID , int CliProxyID , int MaxCount , int Count );

	static void SC_ReadMailResult_OK	( int CliSockID , int CliProxyID , int MailDBID , MailBaseInfoStruct&  MailBaseInfo , char* Content );
	static void SC_ReadMailResult_Failed( int CliSockID , int CliProxyID , int MailDBID  );

	static void SC_UnReadMailCount  ( int CliSockID , int CliProxyID , int Count , int GiftCount );
	static void SC_NewMail			( char* RoleName , bool IsGift );

	virtual void RL_SendMail	( int SrvSockID , int DBID , MailBaseInfoStruct& MailBaseInfo , char* Content , int TargetDBID ) = 0;
	virtual void RL_MailList	( int DBID , int CliSockID , int CliProxyID ) = 0;
	virtual void RL_SendBack	( int DBID , int MailDBID ) = 0;
	virtual void RL_DelMail		( int DBID , int MailDBID ) = 0;
	virtual void RL_ReadMail	( int DBID , int MailDBID ) = 0;
	virtual void RL_GetItem		( int SrvSockID , int DBID , int MailDBID , int Money , int Money_Account , int Money_Bonus ) = 0;
	virtual void RL_UnReadMailCountRequest	( int DBID , int CliSockID , int CliProxyID ) = 0;

	virtual void RL_CheckSendMail( int SrvSockID , int PlayerDBID , char* TargetName , int MapKey ) = 0;

	virtual void RL_CheckGetItemRequest( int SrvServerID , int DBID , int MailDBID , int Money , int Money_Account , int Money_Bonus ) = 0;
};
