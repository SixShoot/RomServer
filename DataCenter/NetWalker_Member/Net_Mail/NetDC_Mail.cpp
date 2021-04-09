#include "PG/PG_Mail.h"
#include "NetDC_Mail.h"
//-------------------------------------------------------------------
CNetDC_Mail*	CNetDC_Mail::This = NULL;
//-------------------------------------------------------------------
bool CNetDC_Mail::_Release()
{

	return false;
}
//-------------------------------------------------------------------
bool CNetDC_Mail::_Init()
{

	_Net->RegOnSrvPacketFunction( EM_PG_Mail_LtoD_SendMail		, _PG_Mail_LtoD_SendMail	);
	_Net->RegOnSrvPacketFunction( EM_PG_Mail_LtoD_MailList		, _PG_Mail_LtoD_MailList	);
	_Net->RegOnSrvPacketFunction( EM_PG_Mail_LtoD_SendBack		, _PG_Mail_LtoD_SendBack	);
	_Net->RegOnSrvPacketFunction( EM_PG_Mail_LtoD_DelMail		, _PG_Mail_LtoD_DelMail		);
	_Net->RegOnSrvPacketFunction( EM_PG_Mail_LtoD_ReadMail		, _PG_Mail_LtoD_ReadMail	);
	_Net->RegOnSrvPacketFunction( EM_PG_Mail_LtoD_GetItem		, _PG_Mail_LtoD_GetItem		);

	_Net->RegOnSrvPacketFunction( EM_PG_Mail_LtoD_UnReadMailCountRequest	, _PG_Mail_LtoD_UnReadMailCountRequest		);
	_Net->RegOnSrvPacketFunction( EM_PG_Mail_LtoD_CheckSendMail				, _PG_Mail_LtoD_CheckSendMail		);
	_Net->RegOnSrvPacketFunction( EM_PG_Mail_LtoD_CheckGetItemRequest		, _PG_Mail_LtoD_CheckGetItemRequest		);

	return true;
}
//-------------------------------------------------------------------
void CNetDC_Mail::_PG_Mail_LtoD_CheckGetItemRequest	( int ServerID , int Size , void* Data )
{
	PG_Mail_LtoD_CheckGetItemRequest* PG = ( PG_Mail_LtoD_CheckGetItemRequest* )Data;
	This->RL_CheckGetItemRequest( ServerID , PG->DBID , PG->MailDBID , PG->Money , PG->Money_Account , PG->Moneu_Bonus );
}

void CNetDC_Mail::_PG_Mail_LtoD_CheckSendMail( int ServerID , int Size , void* Data )
{
	PG_Mail_LtoD_CheckSendMail* PG = ( PG_Mail_LtoD_CheckSendMail* )Data;
	This->RL_CheckSendMail( ServerID , PG->PlayerDBID , (char*)PG->TargetName.Begin() , PG->MapKey );
}

void CNetDC_Mail::_PG_Mail_LtoD_UnReadMailCountRequest( int ServerID , int Size , void* Data )
{
	PG_Mail_LtoD_UnReadMailCountRequest* PG = ( PG_Mail_LtoD_UnReadMailCountRequest* )Data;
	This->RL_UnReadMailCountRequest( PG->DBID , PG->CliSockID , PG->CliProxyID );
}
void CNetDC_Mail::_PG_Mail_LtoD_SendMail	( int SrvSockID , int Size , void* Data )	
{
	PG_Mail_LtoD_SendMail* PG = ( PG_Mail_LtoD_SendMail* )Data;
	This->RL_SendMail( SrvSockID , PG->DBID , PG->MailBaseInfo , (char*)PG->Content.Begin() , PG->TargetDBID );
}
void CNetDC_Mail::_PG_Mail_LtoD_MailList	( int SrvSockID , int Size , void* Data )	
{
	PG_Mail_LtoD_MailList* PG = ( PG_Mail_LtoD_MailList* )Data;
	This->RL_MailList( PG->DBID , PG->CliSockID , PG->CliProxyID );
}
void CNetDC_Mail::_PG_Mail_LtoD_SendBack	( int SrvSockID , int Size , void* Data )	
{
	PG_Mail_LtoD_SendBack* PG = ( PG_Mail_LtoD_SendBack* )Data;
	This->RL_SendBack( PG->DBID , PG->MailDBID );
}
void CNetDC_Mail::_PG_Mail_LtoD_DelMail		( int SrvSockID , int Size , void* Data )	
{
	PG_Mail_LtoD_DelMail* PG = ( PG_Mail_LtoD_DelMail* )Data;
	This->RL_DelMail( PG->DBID , PG->MailDBID );
}
void CNetDC_Mail::_PG_Mail_LtoD_ReadMail	( int SrvSockID , int Size , void* Data )	
{
	PG_Mail_LtoD_ReadMail* PG = ( PG_Mail_LtoD_ReadMail* )Data;
	This->RL_ReadMail( PG->DBID , PG->MailDBID  );
}
void CNetDC_Mail::_PG_Mail_LtoD_GetItem		( int SrvSockID , int Size , void* Data )	
{
	PG_Mail_LtoD_GetItem* PG = ( PG_Mail_LtoD_GetItem* )Data;
	This->RL_GetItem( SrvSockID , PG->DBID , PG->MailDBID , PG->Money , PG->Money_Account , PG->Money_Bonus );
}
//-------------------------------------------------------------------
void CNetDC_Mail::SL_SendMailResult	( int SrvSockID , int DBID , int Money , int Money_Account , int Money_Bonus , bool IsSendMoney , bool IsSystem , SendMailResultEnum Result , const char* TargetName )
{
	if( SrvSockID == -1 )
		return;

	PG_Mail_DtoL_SendMailResult Send;

	Send.DBID = DBID;	//要求者的DBID
	Send.Money = Money;		
	Send.Money_Account = Money_Account;		
	Send.Money_Bonus = Money_Bonus;
	Send.IsSendMoney = IsSendMoney;		//true 寄送金錢 false 獲到付款
	Send.IsSystem = IsSystem;			//是否為系統信件
	Send.Result = Result;
	Send.TargetName = TargetName;

	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );

}
//-------------------------------------------------------------------
void CNetDC_Mail::SL_GetItemResult	( int SrvSockID , int DBID , int OrgSenderDBID , int MailDBID , bool Result , int Money , int Money_Account , int Money_Bonus , ItemFieldStruct Item[10] )
{
	PG_Mail_DtoL_GetItemResult Send;

	Send.DBID = DBID;
	Send.OrgSenderDBID = OrgSenderDBID;
	Send.MailDBID = MailDBID;	//信件的DBID (一定要最後一次讀取的信件)
	Send.Result = Result;
	Send.Money = Money;		//金錢 ( + 獲得  - 付出 )
	Send.Money_Account = Money_Account;
	Send.Money_Bonus = Money_Bonus;
	//Send.Item = Item;		//寄送物品
	memcpy( Send.Item , Item , sizeof(Send.Item) );

	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
void CNetDC_Mail::SC_MailListResult	( int CliSockID , int CliProxyID , int MailDBID , int Index , MailBaseInfoStruct&  MailBaseInfo , char* Content )
{
	
	PG_Mail_DtoC_MailListResult Send;

	Send.MailDBID = MailDBID;	//信件的DBID
	Send.Index = Index;	//地幾筆

	Send.MailBaseInfo= MailBaseInfo;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1;

	Global::SendToCli_ByProxyID( CliSockID , CliProxyID , sizeof( Send ) , &Send );
	
}
//-------------------------------------------------------------------
/*
void CNetDC_Mail::SC_ReadMailResult_OK	(  int CliSockID , int CliProxyID , int MailDBID , MailBaseInfoStruct&  MailBaseInfo , char* Content )
{
	PG_Mail_DtoC_ReadMailResult Send;

	Send.MailDBID = MailDBID;	//信件的DBID
	Send.MailBaseInfo = MailBaseInfo;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size() + 1;
	Send.Result = true;

	Global::SendToCli_ByProxyID( CliSockID , CliProxyID , Send.PGSize() , &Send );
}
//-------------------------------------------------------------------
void CNetDC_Mail::SC_ReadMailResult_Failed( int CliSockID , int CliProxyID , int MailDBID  )
{
	PG_Mail_DtoC_ReadMailResult Send;

	Send.MailDBID = MailDBID;	//信件的DBID
	Send.Content = "";
	Send.ContentSize = Send.Content.Size() + 1;
	Send.Result = false;

	Global::SendToCli_ByProxyID( CliSockID , CliProxyID , Send.PGSize() , &Send );
}
*/
//-------------------------------------------------------------------
void CNetDC_Mail::SC_MailListCountResult	( int CliSockID , int CliProxyID , int MaxCount , int Count )
{
	PG_Mail_DtoC_MailListCountResult Send;

	Send.MaxCount = MaxCount;
	Send.Count = Count;

	Global::SendToCli_ByProxyID( CliSockID , CliProxyID , sizeof( Send ) , &Send );
}

void CNetDC_Mail::SC_UnReadMailCount  ( int CliSockID , int CliProxyID , int Count , int GiftCount )
{
	PG_Mail_DtoC_UnReadMailCount Send;
	Send.Count = Count;
	Send.GiftCount = GiftCount;

	Global::SendToCli_ByProxyID( CliSockID , CliProxyID , sizeof( Send ) , &Send );
}

void CNetDC_Mail::SC_NewMail			( char* RoleName , bool IsGift )
{
	PG_Mail_DtoC_NewMail Send;
	Send.IsGift = IsGift;
	Global::SendToCli_ByRoleName( RoleName , sizeof( Send ) , &Send );	
}

void CNetDC_Mail::SL_CheckSendMailResult( int SrvSockID , int PlayerDBID , char* TargetName , int TargetDBID , int MapKey , bool Result , int ErrorCode )
{
	PG_Mail_DtoL_CheckSendMailResult Send;
	Send.PlayerDBID = PlayerDBID;
	Send.TargetDBID = TargetDBID;
	Send.TargetName = TargetName;
	Send.MapKey = MapKey;
	Send.Result = Result;
	Send.ErrorCode = ErrorCode;

	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );	
}
void CNetDC_Mail::SL_CheckGetItemResult( int SrvSockID , int	DBID , int TargetDBID ,	int MailDBID , int Money , int Money_Account , int Money_Bonus , bool	Result )
{
	PG_Mail_DtoL_CheckGetItemResult Send;
	Send.DBID = DBID;			
	Send.TargetDBID = TargetDBID;
	Send.MailDBID = MailDBID;	
	Send.Money = Money;			
	Send.Money_Account = Money_Account;	
	Send.Money_Bonus = Money_Bonus;
	Send.Result = Result;
	Global::SendToSrvBySockID( SrvSockID , sizeof( Send ) , &Send );	
}