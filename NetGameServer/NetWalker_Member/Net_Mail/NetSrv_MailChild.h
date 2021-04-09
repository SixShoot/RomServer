#ifndef __NETWAKER_MailCHILD_H__
#define __NETWAKER_MailCHILD_H__

#include "NetSrv_Mail.h"

struct SendMailTempStruct
{
	bool	IsDestroy;

	int		SenderGUID;
	MailBaseInfoStruct MailBaseInfo;
	int		ItemPos[10];
	string	Content;	

	SendMailTempStruct()
	{
		IsDestroy = true;
	}
};

class NetSrv_MailChild : public NetSrv_Mail
{
	static vector< SendMailTempStruct >  _SendMailWaitVector;
	static vector< int >				 _SemMailUnUseID;

public:
    static bool Init();
    static bool Release();
	
	static void SendSysMail		( const char* SendToName , const char* Title , const char* Content ); 

	virtual void RC_SendMail	( BaseItemObject* OwnerObj , MailBaseInfoStruct& MailBaseInfo , int ItemPos[10] , char* Content , char* Password );
	virtual void RC_MailList	( BaseItemObject* OwnerObj );
	virtual void RC_SendBack	( BaseItemObject* OwnerObj , int MailDBID );
	virtual void RC_DelMail		( BaseItemObject* OwnerObj , int MailDBID );
	virtual void RC_ReadMail	( BaseItemObject* OwnerObj , int MailDBID );
	virtual void RC_GetItem		( BaseItemObject* OwnerObj , int MailDBID , int Money , int Money_Account , int Money_Bonus , const char* Password );
	virtual void RC_CloseMail	( BaseItemObject* OwnerObj );

	virtual void RD_SendMailResult	( BaseItemObject* OwnerObj , bool IsSendMoney , bool IsSystem 
									, int Money , int Money_Account , int Money_Bonus , SendMailResultEnum Result , const char* TargetName );
	virtual void RD_GetItemResult	( BaseItemObject* OwnerObj , int DBID , int OrgSenderDBID , int MailDBID , int Money , int Money_Account , int Money_Bonus , ItemFieldStruct Item[10] , bool Result  );
	virtual void RD_CheckSendMailResult( int PlayerDBID , char* TargetName , int TargetDBID , int MapKey , bool Result , int ErrorCode );
	virtual void RD_CheckGetItemResult( int DBID , int TargetDBID , int MailDBID , int Money , int Money_Account , int Money_Bonus , bool Result );
	virtual void RC_SendMailList( BaseItemObject* Obj , int TargetCount , StaticString <_MAX_NAMERECORD_SIZE_>	OrgTargetName[20] 
								, const char* Title , MailBackGroundTypeENUM PaperType , const char* Content );
};

#endif //__NETWAKER_MailCHILD_H__