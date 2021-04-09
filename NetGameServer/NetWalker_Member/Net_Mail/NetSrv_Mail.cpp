#include "../NetWakerGSrvInc.h"
#include "NetSrv_Mail.h"

//-------------------------------------------------------------------
NetSrv_Mail* NetSrv_Mail::This = NULL;
//-------------------------------------------------------------------
bool NetSrv_Mail::_Init()
{

	//Srv_NetCliReg( PG_Mail_CtoL_SendMail	);
	_Net->RegOnCliPacketFunction(  EM_PG_Mail_CtoL_SendMail		, _PG_Mail_CtoL_SendMail		, PG_Mail_CtoL_SendMail::BasePGSize()		, sizeof( PG_Mail_CtoL_SendMail) );
	_Net->RegOnCliPacketFunction(  EM_PG_Mail_CtoL_SendMailList , _PG_Mail_CtoL_SendMailList	, PG_Mail_CtoL_SendMailList::BasePGSize()	, sizeof( PG_Mail_CtoL_SendMailList) );
	Srv_NetCliReg( PG_Mail_CtoL_MailList	);
	Srv_NetCliReg( PG_Mail_CtoL_SendBack	);
	Srv_NetCliReg( PG_Mail_CtoL_DelMail		);
	Srv_NetCliReg( PG_Mail_CtoL_ReadMail	);
	Srv_NetCliReg( PG_Mail_CtoL_GetItem		);
	Srv_NetCliReg( PG_Mail_CtoL_CloseMail	);

	_Net->RegOnSrvPacketFunction			( EM_PG_Mail_DtoL_SendMailResult		, _PG_Mail_DtoL_SendMailResult		);
	_Net->RegOnSrvPacketFunction			( EM_PG_Mail_DtoL_GetItemResult			, _PG_Mail_DtoL_GetItemResult		);
	_Net->RegOnSrvPacketFunction			( EM_PG_Mail_DtoL_CheckSendMailResult	, _PG_Mail_DtoL_CheckSendMailResult	);
	_Net->RegOnSrvPacketFunction			( EM_PG_Mail_DtoL_CheckGetItemResult	, _PG_Mail_DtoL_CheckGetItemResult	);
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_Mail::_Release()
{
    return true;
}
void NetSrv_Mail::_PG_Mail_CtoL_SendMailList		( int Sockid , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return ;

	PG_Mail_CtoL_SendMailList* PG = (PG_Mail_CtoL_SendMailList*)Data;

	This->RC_SendMailList( Obj , PG->TargetCount , PG->OrgTargetName , PG->Title.Begin() , PG->PaperType , PG->Content.Begin() );
}

void NetSrv_Mail::_PG_Mail_DtoL_CheckGetItemResult	( int Sockid , int Size , void* Data )
{
	PG_Mail_DtoL_CheckGetItemResult* PG = (PG_Mail_DtoL_CheckGetItemResult*)Data;

	This->RD_CheckGetItemResult( PG->DBID , PG->TargetDBID , PG->MailDBID , PG->Money , PG->Money_Account , PG->Money_Bonus , PG->Result );
}

void NetSrv_Mail::_PG_Mail_DtoL_CheckSendMailResult	( int Sockid , int Size , void* Data )
{
	PG_Mail_DtoL_CheckSendMailResult* PG = (PG_Mail_DtoL_CheckSendMailResult*)Data;

	This->RD_CheckSendMailResult( PG->PlayerDBID , (char*)PG->TargetName.Begin() , PG->TargetDBID , PG->MapKey , PG->Result , PG->ErrorCode );
}

void NetSrv_Mail::_PG_Mail_CtoL_CloseMail			( int Sockid , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return ;
//	PG_Mail_CtoL_CloseMail* PG = (PG_Mail_CtoL_CloseMail*)Data;

	This->RC_CloseMail( Obj );
}

void NetSrv_Mail::_PG_Mail_CtoL_SendMail			( int Sockid , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return ;
	PG_Mail_CtoL_SendMail* PG = (PG_Mail_CtoL_SendMail*)Data;

	This->RC_SendMail( Obj , PG->MailBaseInfo , PG->ItemPos , (char*)PG->Content.Begin() , (char*)PG->Password.Begin() );

}
void NetSrv_Mail::_PG_Mail_CtoL_MailList			( int Sockid , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return ;
	PG_Mail_CtoL_MailList* PG = (PG_Mail_CtoL_MailList*)Data;

	This->RC_MailList( Obj );
}
void NetSrv_Mail::_PG_Mail_CtoL_SendBack			( int Sockid , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return ;
	PG_Mail_CtoL_SendBack* PG = (PG_Mail_CtoL_SendBack*)Data;

	This->RC_SendBack( Obj , PG->MailDBID );
}
void NetSrv_Mail::_PG_Mail_CtoL_DelMail			( int Sockid , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return ;
	PG_Mail_CtoL_DelMail* PG = (PG_Mail_CtoL_DelMail*)Data;

	This->RC_DelMail( Obj , PG->MailDBID );
}
void NetSrv_Mail::_PG_Mail_CtoL_ReadMail			( int Sockid , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return ;
	PG_Mail_CtoL_ReadMail* PG = (PG_Mail_CtoL_ReadMail*)Data;

	This->RC_ReadMail( Obj , PG->MailDBID );
}
void NetSrv_Mail::_PG_Mail_CtoL_GetItem			( int Sockid , int Size , void* Data )
{
	BaseItemObject*	Obj =	BaseItemObject::GetObjBySockID( Sockid );
	if( Obj == NULL )
		return ;
	PG_Mail_CtoL_GetItem* PG = (PG_Mail_CtoL_GetItem*)Data;

	This->RC_GetItem( Obj , PG->MailDBID , PG->Money , PG->Money_Account , PG->Money_Bonus , PG->Password.Begin() );
}


void NetSrv_Mail::_PG_Mail_DtoL_SendMailResult		( int Sockid , int Size , void* Data )
{
	PG_Mail_DtoL_SendMailResult* PG = (PG_Mail_DtoL_SendMailResult*)Data;

	BaseItemObject*	Obj =	BaseItemObject::GetObjByDBID( PG->DBID );
	if( Obj == NULL )
		return ;
	
	This->RD_SendMailResult( Obj , PG->IsSendMoney , PG->IsSystem , PG->Money , PG->Money_Account , PG->Money_Bonus , PG->Result , PG->TargetName.Begin() );
}
void NetSrv_Mail::_PG_Mail_DtoL_GetItemResult		( int Sockid , int Size , void* Data )
{
	PG_Mail_DtoL_GetItemResult* PG = (PG_Mail_DtoL_GetItemResult*)Data;

	BaseItemObject*	Obj =	BaseItemObject::GetObjByDBID( PG->DBID );
	if( Obj == NULL )
		return ;

	This->RD_GetItemResult( Obj , PG->DBID , PG->OrgSenderDBID , PG->MailDBID , PG->Money , PG->Money_Account , PG->Money_Bonus , PG->Item , PG->Result );
}
//------------------------------------------------------------------------------------------------------------------------------------

void NetSrv_Mail::SC_SendMailResult	( int SendID , SendMailResultEnum Result , const char* Name )
{
	PG_Mail_LtoC_SendMailResult Send;
	
	Send.Result = Result;
	Send.TargetName = Name;

	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_Mail::SC_GetItemResult	( int SendID , int MailDBID , bool Result )
{
	PG_Mail_LtoC_GetItemResult Send;

	Send.Result = Result;
	Send.MailDBID = MailDBID;

	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_Mail::SD_SendMail		( int DBID , char* Content , MailBaseInfoStruct& MailBaseInfo , int TargetDBID )
{
	PG_Mail_LtoD_SendMail Send;
	Send.DBID = DBID;
	Send.TargetDBID = TargetDBID;
	Send.Content = Content;
	Send.ContentSize = Send.Content.Size()+1;
	Send.MailBaseInfo = MailBaseInfo;
//	Send.RetItemPos = ItemPos;
//	memcpy( Send.RetItemPos , ItemPos , sizeof(Send.RetItemPos) );
	Global::SendToDBCenter( Send.PGSize() , &Send );
}
void NetSrv_Mail::SD_MailList		( int DBID , int CliSockID , int CliProxyID )
{
	PG_Mail_LtoD_MailList Send;
	Send.DBID = DBID;
	Send.CliSockID = CliSockID;
	Send.CliProxyID = CliProxyID;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Mail::SD_SendBack		( int DBID , int MailDBID )
{
	PG_Mail_LtoD_SendBack Send;
	Send.DBID = DBID;
	Send.MailDBID = MailDBID;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Mail::SD_DelMail		( int DBID , int MailDBID )
{
	PG_Mail_LtoD_DelMail Send;
	Send.DBID = DBID;
	Send.MailDBID = MailDBID;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Mail::SD_ReadMail		( int DBID , int MailDBID )
{
	PG_Mail_LtoD_ReadMail Send;
	Send.DBID = DBID;
	Send.MailDBID = MailDBID;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
void NetSrv_Mail::SD_GetItem		( int DBID , int MailDBID , int Money , int Money_Account , int Money_Bonus )
{
	PG_Mail_LtoD_GetItem Send;
	Send.DBID = DBID;
	Send.MailDBID = MailDBID;
	Send.Money = Money;
	Send.Money_Account = Money_Account;
	Send.Money_Bonus = Money_Bonus;

	Global::SendToDBCenter( sizeof( Send ) , &Send );
}

void NetSrv_Mail::SC_OpenMail		( int SendID , int NpcGUID )
{
	PG_Mail_LtoC_OpenMail Send;

	Send.NpcGUID = NpcGUID;

	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}
void NetSrv_Mail::SC_CloseMail		( int SendID  )
{
	PG_Mail_LtoC_CloseMail Send;

	Global::SendToCli_ByGUID( SendID , sizeof( Send ) , &Send );
}

void NetSrv_Mail::SD_UnReadMailCountRequest( int DBID , int CliSockID , int CliProxyID )
{
	PG_Mail_LtoD_UnReadMailCountRequest Send;
	Send.DBID = DBID;
	Send.CliProxyID = CliProxyID;
	Send.CliSockID = CliSockID;
	Global::SendToDBCenter( sizeof( Send ) , &Send );
}
/*
void NetSrv_Mail::SC_NewMail			( char* RoleName )
{
	PG_Mail_LtoC_NewMail Send;
	Global::SendToCli_ByRoleName( RoleName , sizeof( Send ) , &Send );	
}
*/
void NetSrv_Mail::SD_CheckSendMail	( int PlayerDBID , const char* TargetName , int MapKey )
{
	PG_Mail_LtoD_CheckSendMail Send;
	Send.PlayerDBID = PlayerDBID;
	Send.TargetName = TargetName;
	Send.MapKey = MapKey;

	Global::SendToDBCenter( sizeof( Send ) , &Send );	
}

void NetSrv_Mail::SD_CheckGetItemRequest( int DBID , int MailDBID , int Money , int Money_Account , int Money_Bonus )
{
	PG_Mail_LtoD_CheckGetItemRequest Send;
	Send.DBID = DBID;
	Send.MailDBID = MailDBID;
	Send.Money = Money;
	Send.Money_Account = Money_Account;
	Send.Moneu_Bonus = Money_Bonus;
	Global::SendToDBCenter( sizeof( Send ) , &Send );	
}