#include "../NetWakerGSrvInc.h"
#include "NetSrv_GMTools.h"
//-------------------------------------------------------------------
NetSrv_GMTools*    NetSrv_GMTools::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_GMTools::_Init()
{
    Global::Net_ServerList->RegServerLoginEvent( EM_SERVER_TYPE_GMTOOLS  , _EventGMToolsConnect );
    _Net->RegOnSrvPacketFunction( EM_PG_GM_GMtoL_ObjectDataRequest  , _PG_GM_GMtoL_ObjectDataRequest );
    _Net->RegOnSrvPacketFunction( EM_PG_GM_GMtoL_SendMsg            , _PG_GM_GMtoL_SendMsg );
	_Net->RegOnSrvPacketFunction( EM_PG_GM_GMtoL_SendGMCommand      , _PG_GM_GMtoL_SendGMCommand );
	_Net->RegOnSrvPacketFunction( EM_PG_GM_GMtoX_ServerStateRequest , _PG_GM_GMtoX_ServerStateRequest );

//    Print.Init( S_SrvMsgToAllGMTools );
    return true;
}
//-------------------------------------------------------------------
bool NetSrv_GMTools::_Release()
{
    return true;
}
//-------------------------------------------------------------------
void NetSrv_GMTools::_PG_GM_GMtoX_ServerStateRequest	( int sockid , int size , void* data )
{
	PG_GM_GMtoX_ServerStateRequest* pg =(PG_GM_GMtoX_ServerStateRequest*)data;
	This->R_ServerStateRequest( sockid , pg->RequestType );
}
void NetSrv_GMTools::_PG_GM_GMtoL_SendGMCommand		( int sockid , int size , void* data )
{
	PG_GM_GMtoL_SendGMCommand* pg =(PG_GM_GMtoL_SendGMCommand*)data;

	//BaseItemObject*	Obj =	Global::GetObj( pg->GUID );

	This->R_GMCommand( sockid , pg->GUID , pg->Account.Begin() , pg->ManageLv , pg->Content.Begin() );   



}
//-------------------------------------------------------------------
void NetSrv_GMTools::_EventGMToolsConnect		( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID )
{
    int NetID = Global::Net_ServerList->GMID_To_NetID( dwServerLocalID );
    This->R_OnGMToolsConnect( NetID );
}
//-------------------------------------------------------------------
void NetSrv_GMTools::_PG_GM_GMtoL_ObjectDataRequest  ( int sockid , int size , void* data )
{
    PG_GM_GMtoL_ObjectDataRequest* pg =(PG_GM_GMtoL_ObjectDataRequest*)data;

    This->R_ObjectDataRequest( sockid , pg->GUID );       
 
}
void NetSrv_GMTools::_PG_GM_GMtoL_SendMsg            ( int sockid , int size , void* data )
{
    PG_GM_GMtoL_SendMsg* pg =(PG_GM_GMtoL_SendMsg*)data;

    if( pg->GUID == -1 )
    {
        This->R_ToAllPlayerMsg( sockid , pg->Content.Begin() );
    }
    else
    {
        BaseItemObject*	Obj =	Global::GetObj( pg->GUID );
        if( Obj == NULL )
            return ;

        if( strcmp( Obj->Role()->RoleName() , pg->Name.Begin() ) != 0 )
        {
            Print( LV2 , "_PG_GM_GMtoL_SendMsg" , "GM Response Err GUID Name(%s) , Obj Name(%s) Msg=(%s)" , Utf8ToChar( Obj->Role()->RoleName() ).c_str() , Utf8ToChar( pg->Name.Begin() ).c_str() , pg->Content.Begin() );
            return;
        }

        This->R_ToPlayerMsg( sockid , Obj , pg->Name.Begin() , pg->Content.Begin() );   
    }
}
//-------------------------------------------------------------------
void NetSrv_GMTools::S_ZoneInfo( int NetID , int ZoneID , const char* ZoneName , int ObjectCount  , int PlayerCount , int NPCCount )
{
    PG_GM_LToGM_ZoneInfo Send;
    Send.Info.GUID = ZoneID;
    Send.Info.NPCCount = NPCCount;
    Send.Info.ObjectCount = ObjectCount;
    Send.Info.PlayerCount = PlayerCount;
    MyStrcpy( Send.Info.ZoneName , ZoneName , sizeof( Send.Info.ZoneName ) );

//    SendToGMTools( sizeof( Send ) , &Send );
    if( NetID != -1 )
        SendToSrvBySockID( NetID , sizeof( Send ) , &Send );
    else
        SendToAllGMTools( sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
void NetSrv_GMTools::S_ObjInfo( int NetID , RoleDataEx* Role )
{
    if( Role->IsPlayer() )
    {
        PG_GM_LToGM_PlayerInfo Send;

        MyStrcpy( Send.Info.Account , Role->Base.Account_ID.Begin() , sizeof(Send.Info.Account) );
        Send.Info.DBID = Role->DBID();
        Send.Info.GUID = Role->GUID();

		GSrv_CliInfoStruct *CliInfo = _Net->GetCliNetInfo( Role->SockID() );
		if( CliInfo == NULL )
			return;

		unsigned char* IpNum = (unsigned char*)&CliInfo->IPnum;
		sprintf( Send.Info.IP , "%d.%d.%d.%d" , IpNum[0] , IpNum[1], IpNum[2], IpNum[3] );

        Send.Info.Job       = Role->BaseData.Voc;
        Send.Info.Job_Sub   = Role->BaseData.Voc_Sub;
        Send.Info.LV        = Role->TempData.Attr.Level;
        Send.Info.LV_Sub    = Role->TempData.Attr.Level_Sub;
        Send.Info.Race      = Role->BaseData.Race;
        MyStrcpy( Send.Info.RoleName , Role->RoleName() , sizeof(Send.Info.RoleName) );
        Send.Info.ZoneID    = RoleDataEx::G_ZoneID;

        //SendToGMTools( sizeof( Send ) , &Send );
        if( NetID != -1 )
            SendToSrvBySockID( NetID , sizeof( Send ) , &Send );
        else
            SendToAllGMTools( sizeof( Send ) , &Send );
    }
    else if(	Role->IsNPC() 
			||	Role->IsQuestNPC() 
			||	Role->IsMine() )
    {
		if( Global::Ini()->IsSendNpcInfoToGMTools != false )
		{
			PG_GM_LToGM_NPCInfo Send;

			Send.Info.DBID = Role->DBID();
			Send.Info.GUID = Role->GUID();
			Send.Info.LV        = Role->TempData.Attr.Level;
			Send.Info.OrgDBID = Role->BaseData.ItemInfo.OrgObjID;
			MyStrcpy( Send.Info.Name , Role->RoleName() , sizeof(Send.Info.Name) );
			Send.Info.ZoneID    = RoleDataEx::G_ZoneID;

			//SendToGMTools( sizeof( Send ) , &Send );
			if( NetID != -1 )
				SendToSrvBySockID( NetID , sizeof( Send ) , &Send );
			else
				SendToAllGMTools( sizeof( Send ) , &Send );
		}
    }

}

void NetSrv_GMTools::S_DelObjInfo( int NetID , RoleDataEx* Role )
{
    if( Role->IsPlayer() )
    {
        PG_GM_LToGM_DelPlayer Send;
        Send.GUID = Role->GUID();
		Send.DBID = Role->DBID();
        Send.ZoneID = RoleDataEx::G_ZoneID;
        //SendToGMTools( sizeof( Send ) , &Send );
        if( NetID != -1 )
            SendToSrvBySockID( NetID , sizeof( Send ) , &Send );
        else
            SendToAllGMTools( sizeof( Send ) , &Send );
    }
    else if( Role->IsNPC() )
    {
        PG_GM_LToGM_DelNPC  Send;
        Send.GUID = Role->GUID();
        Send.ZoneID = RoleDataEx::G_ZoneID;
        //SendToGMTools( sizeof( Send ) , &Send );
        if( NetID != -1 )
            SendToSrvBySockID( NetID , sizeof( Send ) , &Send );
        else
            SendToAllGMTools( sizeof( Send ) , &Send );
    }    
}

void NetSrv_GMTools::S_ObjectData( int NetID , RoleDataEx* Role )
{
    PG_GM_LtoGM_ObjectData Send;
    Send.Role = *Role;
    //Send.Role.SaveDB_DelTime();
    //SendToGMTools( sizeof( Send ) , &Send );
    if( NetID != -1 )
        SendToSrvBySockID( NetID , sizeof( Send ) , &Send );
    else
        SendToAllGMTools( sizeof( Send ) , &Send );
}

void NetSrv_GMTools::S_PlayerMsg( int NetID , char* Name , char* Msg )
{
    PG_GM_LtoGM_SendMsg Send;
    Send.ContentSize = (int)strlen( Msg );
    MyStrcpy( (char*)Send.Content.Begin() , Msg , sizeof( Send.Content ) );
    MyStrcpy( (char*)Send.Name.Begin() , Name , sizeof( Send.Name ) );
    //SendToGMTools( sizeof( Send ) , &Send );       
    if( NetID != -1 )
        SendToSrvBySockID( NetID , sizeof( Send ) , &Send );
    else
        SendToAllGMTools( sizeof( Send ) , &Send );
}

void NetSrv_GMTools::S_SrvMsgToAllGMTools( int LV , char* Title , char* Context )
{
    PG_GM_LtoGM_SrvSysMsg Send;
    Send.LV = LV;
    Send.Title = Title;
    Send.Msg = Context;
    SendToAllGMTools( sizeof( Send ) , &Send );
}
void NetSrv_GMTools::S_ServerStateResult( int NetID , ServerStateRequestTypeENUM	RequestType , const char* Name , const char* Type , const char* State 
										 , const char* Content1 , const char* Content2
										 , const char* Content3 , const char* Content4
										 , const char* Content5 , const char* Content6
										 , const char* Content7 , const char* Content8 )
{
	PG_GM_XtoGM_ServerStateResult	Send;
	Send.RequestType = RequestType;
	Send.Name = Name;
	Send.Type = Type;
	Send.State = State;
	Send.Content[0] = Content1;
	Send.Content[1] = Content2;
	Send.Content[2] = Content3;
	Send.Content[3] = Content4;
	Send.Content[4] = Content5;
	Send.Content[5] = Content6;
	Send.Content[6] = Content7;
	Send.Content[7] = Content8;
	SendToSrvBySockID( NetID , sizeof( Send ) , &Send );
}