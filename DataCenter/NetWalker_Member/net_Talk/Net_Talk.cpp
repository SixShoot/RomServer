#include "Net_Talk.h"	
Net_Talk*	Net_Talk::This		= NULL;

//-------------------------------------------------------------------
Net_Talk::Net_Talk(void)
{
}
//-------------------------------------------------------------------
Net_Talk::~Net_Talk(void)
{
}
//-------------------------------------------------------------------
bool Net_Talk::_Release()
{

	return true;
}
//-------------------------------------------------------------------
bool Net_Talk::_Init()
{
	_Net->RegOnSrvPacketFunction			( EM_PG_Talk_LtoD_OfflineMessageRequest	, _PG_Talk_LtoD_OfflineMessageRequest		);	
	return true;
}
//-------------------------------------------------------------------
void Net_Talk::_PG_Talk_LtoD_OfflineMessageRequest( int ServerID , int Size , void* Data )
{
	PG_Talk_LtoD_OfflineMessageRequest* pg =(PG_Talk_LtoD_OfflineMessageRequest*)Data;
	This->RL_OfflineMessageRequest( ServerID , pg->PlayerDBID );
}
//-------------------------------------------------------------------
void Net_Talk::GameMsgEvent	( int SockID , int ProxyID , GameMessageEventENUM Event )
{
	PG_GM_LtoC_SendGameMessageEvent Send;
	Send.Event = Event;
	SendToCli_ByProxyID( SockID , ProxyID , sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
void Net_Talk::SC_OfflineMessage	( int PlayerDBID , int FromDBID , float Time , const char* FromName , const char* ToName , const char* Content )
{
	PG_Talk_DtoC_OfflineMessage Send;
	Send.FromDBID = FromDBID;
	Send.Time = Time;
	Send.FromName = FromName;
	Send.ToName = ToName;
	Send.Content = Content;
	Global::SendToCli_ByDBID( PlayerDBID , sizeof(Send) , &Send );
}
void Net_Talk::SL_OfflineGMCommand	( int ServerID , int PlayerDBID , int ManageLv , const char* Content )
{
	PG_Talk_DtoL_OfflineGMCommand Send;
	Send.PlayerDBID = PlayerDBID;
	Send.ManageLv = (ManagementENUM)ManageLv;
	Send.Content = Content;
	
	Global::SendToSrvBySockID( ServerID , sizeof(Send) , &Send );
}

void Net_Talk::SC_ALL_RunningMsg	( const char* Message )
{
	PG_Talk_LtoC_RunningMsg Send;
	Send.Content = Message;
	Send.ContentSize = Send.Content.Size() + 1 ;

	Global::SendToAllPlayer( sizeof( Send ) , &Send );
}

void Net_Talk::SysMsg( int PlayerDBID  , const char* Content )
{
	PG_Talk_LtoC_SystemMsg Send;

	char szBuff[1024];

	sprintf( szBuff, "[DEBUG] %s", Content );
	Send.Content = szBuff;
	Send.ContentSize = Send.Content.Size() + 1 ;

	Global::SendToCli_ByDBID( PlayerDBID , Send.Size() , &Send );
}
