#pragma once

#include "NetDC_Mail.h"
//--------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------
class CNetDC_MailChild : public CNetDC_Mail
{

	static bool _SQLCmdCheckSendMailEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdCheckSendMailResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdSendMailEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdSendMailResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdMailListEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdMailListResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdSendBackEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdSendBackResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdDelMailEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdDelMailResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdReadMailEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdReadMailResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdGetItemEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdGetItemResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdOnTimeProc( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdOnTimeProcResult ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdMailCountEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdMailCountResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool _SQLCmdCheckGetItemEvent( vector<DB_MailStruct>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdCheckGetItemResultEvent ( vector<DB_MailStruct>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );


	static bool _SQLCmdOnTimeProcImportMailEvent( vector<int>& Data , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdOnTimeProcImportMailResultEvent ( vector<int>& Data , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static int	_LastProcTime ;
	static bool _ImportMailProc;

	static int  _OnTimeProc( SchedularInfo* Obj , void* InputClass );
	static int  _OnTimeProc_ImportMail( SchedularInfo* Obj , void* InputClass );
public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

	virtual void RL_SendMail	( int SrvSockID , int DBID , MailBaseInfoStruct& MailBaseInfo , char* Content , int TargetDBID );
	virtual void RL_MailList	( int DBID , int CliSockID , int CliProxyID );
	virtual void RL_SendBack	( int DBID , int MailDBID );
	virtual void RL_DelMail		( int DBID , int MailDBID );
	virtual void RL_ReadMail	( int DBID , int MailDBID );
	virtual void RL_GetItem		( int SrvSockID , int DBID , int MailDBID , int Money , int Money_Account , int Money_Bonus );
	virtual void RL_UnReadMailCountRequest	( int DBID , int CliSockID , int CliProxyID );

	virtual void RL_CheckSendMail( int SrvSockID , int PlayerDBID , char* TargetName , int MapKey  );
	virtual void RL_CheckGetItemRequest( int SrvServerID , int DBID , int MailDBID , int Money , int Money_Account , int Money_Bonus );
//--------------------------------------------------------------------------------------------
	static void SendMail		(  int DBID , MailBaseInfoStruct& MailBaseInfo , char* Content );

	static void	SysSendMoney	( int OrgDBID , const char* ToName , int Money , int Money_Account , int Money_Bonus , const char* Title , const char* Content="" , int ACID = 0 );
	static void	SysSendItem		( int OrgDBID , const char* ToName , ItemFieldStruct& Item , const char* Title , int ACID = 0 );
	static void	SysSendMail		( int OrgDBID , const char* ToName , ItemFieldStruct& Item , const char* Title , int Money , int Money_Account , int ACID = 0 );
};
