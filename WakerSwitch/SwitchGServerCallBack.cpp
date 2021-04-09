#include "SwitchClass.h"
//---------------------------------------------------------------------------------
void	SwitchClass::_OnGSrvConnected       ( int NetID )
{    
//    sockaddr_in pAddr;
//    _NetGSrv->GetPeerAddress( NetID ,&pAddr );
    DWORD IP;
    WORD  Port;
    _NetBGSrv->GetConnectInfo( NetID , &IP , &Port );

    GSrvInfoStruct* Info = _GSrvList.New( NetID );

    if( Info == NULL )
    {
        Print( LV5 , "_OnGSrvConnected" ,  "(GSrv:%d)_OnProxyConnected : _NewProxy false!!" , NetID  );
        return;
    }

    //Info->IPnum    = pAddr.sin_addr.S_un.S_addr;
    Info->IPnum    = IP;
    Info->Stat     = EM_NetStat_Login;

    SysNet_Switch_GSrv_NotifyConnect NotifyConnect;
    NotifyConnect.GSrvID = NetID;
	NotifyConnect.WorldID = SwitchClass::m_WorldID;

    Print( LV2 , "_OnGSrvConnected" , " Switch -> GSrv ,notify GSrv(%d) login to Switch " , NetID );
    SendToGSrv( NetID , sizeof( NotifyConnect ) , &NotifyConnect );
#ifndef _DEBUG
    Schedular()->Push( _CheckGSrvConnectAndReg , 20000 , this 
                    , "GSrvID" , EM_ValueType_int , NetID , NULL );
#endif
}
//---------------------------------------------------------------------------------
void	SwitchClass::_OnGSrvDisconnected    ( int NetID )
{
	if( _ExitFlag == true )
		return;

    SysNet_Switch_Proxy_GSrvDisconnectNotify DisconnectNotify;

    DisconnectNotify.GSrvID = NetID;
    SendToAllProxy( sizeof( DisconnectNotify ) , &DisconnectNotify );

    //通知 Proxy 移除斷線的 GSrv
    Print( LV2 , "_OnGSrvDisconnected" , "notify all Proxy ,erase GSrv(%d)" , NetID );

    //通知所有的 GSrv 此GSrv登出
    SysNet_Switch_GSrv_OnGSrvDisConnect    OnDisConnect;
    OnDisConnect.GSrvID = NetID;
    
    SendToAllGSrv( sizeof(OnDisConnect) , &OnDisConnect );


    if( _GSrvList.Del( NetID ) == false )
    {
        Print( LV5 , "_OnGSrvDisconnected" , "(GSrv:%d)_OnGSrvDisconnected : _DelGSrvInfo false!!" , NetID  );
    }
}
//---------------------------------------------------------------------------------
bool	SwitchClass::_OnGSrvPacket          ( DWORD NetID , DWORD Size , void* Data )
{
//    int		Size;
//    void	*Data;

    //取得封包資料
//    ObjNetGetBuffer( hIncomingData , Size , Data );
    PGSysNULL* PG = ( PGSysNULL *) Data;

    //------------------------------------------------------------------------------
    //檢查是否為系統封包
//    if( PG->SysIdentify == _Sys_Package_Number_ )
    {
        if(_PGInfo.size() <= (UINT32)PG->Command)
        {
            Print( LV2 , "_OnGSrvPacket" , "(GSrv:%d)_OnGSrvPacket : packet cmd=%d out of range" , NetID , PG->Command);
            return false;
        }

        //權限檢查
        PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );

/*        //封包大小檢查
        if( PGinfo->PGSize <= Size )
        {
            Print( LV2 , "_OnGSrvPacket" , "(GSrv:%d)_OnGSrvPacket : packet Size=%d out of range , cmd=%d " , NetID , Size , PG->Command );
            return false;
        }*/

        //來源檢查
        if( PGinfo->Type.GSrv == false )
        {
            Print( LV2 , "_OnGSrvPacket" , "(GSrv:%d)_OnGSrvPacket : the source of packet ,error (PGinfo->Type.GSrv == false ??) cmd=%d " , NetID , PG->Command );
            return false;
        }

        //呼叫Callback 處理
        _PGCallBack[PG->Command]( this , NetID , PG , Size );
    }

    return true;
}
//---------------------------------------------------------------------------------
void	SwitchClass::_OnGSrvConnectFailed   ( int NetID )
{

}
//---------------------------------------------------------------------------------
//檢查GSrv 註冊是否完成
//如果沒完成 刪除此連線
int 	SwitchClass::_CheckGSrvConnectAndReg( SchedularInfo*	Obj , void* InputClass )
{
    SwitchClass*    Class = (SwitchClass*)InputClass;
    int NetID = Obj->GetNumber( "GSrvID" );

    GSrvInfoStruct *Info = Class->_GSrvList[ NetID ];

    if(     Info == NULL
        ||  ( Info->Stat != EM_NetStat_Connect && Info->Stat != EM_NetStat_LoginSrvOK ) )
    {
        Class->_NetBGSrv->Close( NetID );
    }

    return 0;
}
//---------------------------------------------------------------------------------
void	SwitchClass::_PGxGSrvConnect( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size )
{
    Print( LV2 , "_PGxGSrvConnect" , "(GSrv:%d)_PG_GSrvConnect" , NetID );

    SysNet_GSrv_Switch_Connect*  Connect = (SysNet_GSrv_Switch_Connect*) PG;

    GSrvInfoStruct*    GSrvInfo = Obj->_GSrvList[ NetID ];
    if(     GSrvInfo == NULL
        ||  GSrvInfo->Stat != EM_NetStat_Login )
    {
        Print( LV2 , "_PGxGSrvConnect" , "(GSrvID = %d)(GSrvInfo->IsReady == false) or (GSrvInfo->Stat != EM_NetStat_Login)" , NetID );
        return ;
    }

	GSrvInfo->Stat  = EM_NetStat_LoginSrvOK;
    memcpy( GSrvInfo->GSrvName , Connect->GSrvName , sizeof(GSrvInfo->GSrvName) );

    int ProxyCount = Obj->_ProxyList.size( );
    int i;
    ProxyInfoStruct*    Proxy;
    SysNet_Switch_Proxy_GSrvConnect  GSrvConnectNotify;
	GSrvConnectNotify.GSrvID  = NetID;

    SysNet_Switch_GSrv_ProxyAddr     ProxyAddr;
    Obj->_ProxyList.GetData( true );
    for( i = 0 ; i < ProxyCount ; i++ )
    {
        Proxy = Obj->_ProxyList.GetData();
        if(     Proxy == NULL
            ||  Proxy->Stat != EM_NetStat_Connect )
            continue;

        //送資料通知 Proxy Game Server要登入   
        Print( LV2 , "_PGxGSrvConnect" ,  "Switch -> Proxy , notify proxy , Game Server will going to login" );
        Obj->SendToProxy( Proxy->ID , sizeof(GSrvConnectNotify) , &GSrvConnectNotify );
    }

	Print( LV2 , "_PGxGSrvConnectToProxy" , "GSrv login to Switch , Name = %s , ID = %d" , GSrvInfo->GSrvName , GSrvInfo->ID );
    Print( LV2 , "_PGxGSrvConnectToProxy" , "notify all GSrv , new Game Server is login" );
    int GSrvCount = Obj->_GSrvList.size( );
    GSrvInfoStruct*    TempGSrvInfo;

/*
	//
	SysNet_Switch_GSrv_WorldID			PacketWorldID;	
	PacketWorldID.WorldID		= Obj->m_WorldID;
	Obj->SendToGSrv( GSrvInfo->ID, sizeof(SysNet_Switch_GSrv_WorldID), &PacketWorldID );
*/



    SysNet_Switch_GSrv_OnGSrvConnect    OnConnect;
    OnConnect.GSrvID = GSrvInfo->ID;
    memcpy( OnConnect.GSrvName , GSrvInfo->GSrvName , sizeof(GSrvInfo->GSrvName) );
    Obj->SendToAllGSrv( sizeof(OnConnect) , &OnConnect );

    Obj->_GSrvList.GetData( true );
    for( i = 0 ; i < GSrvCount ; i++ )
    {
        TempGSrvInfo = Obj->_GSrvList.GetData( );
        if(     TempGSrvInfo == NULL 
            ||  TempGSrvInfo->Stat    != EM_NetStat_Connect )
            continue;

        if( TempGSrvInfo->ID == GSrvInfo->ID )
            continue;

        OnConnect.GSrvID = TempGSrvInfo->ID;
        memcpy( OnConnect.GSrvName , TempGSrvInfo->GSrvName , sizeof(TempGSrvInfo->GSrvName) );
        Obj->SendToGSrv( GSrvInfo->ID , sizeof(OnConnect) , &OnConnect );

    }
}

//---------------------------------------------------------------------------------
//GSrv 與 Proxy 斷線
void	SwitchClass::_PGxGSrv_GSrvProxyDisconnect( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size )
{
    SysNet_GSrv_Switch_ProxyDisconnect* ProxyDisconnect = (SysNet_GSrv_Switch_ProxyDisconnect*)PG; 

    GSrvInfoStruct* GSrvInfo = Obj->_GSrvList[ NetID ];
    if(     GSrvInfo == NULL
        ||  GSrvInfo->Stat != EM_NetStat_Connect )
    {
        Print( LV5 , "_PGxGSrv_GSrvProxyDisconnect" , " _GetGSrv( %d ) GSrvInfo == NULL or GSrvInfo->Stat != EM_NetStat_Connect" , NetID );
        return;
    }

    Print( LV2 , "_PGxGSrv_GSrvProxyDisconnect" , " notify proxy , to disconnect ProxyID(%d) and GSrvID(%d)" , ProxyDisconnect->ProxyID , NetID );
    SysNet_Switch_Proxy_GSrvDisconnectNotify GSrvDisconnectNotify;
    GSrvDisconnectNotify.GSrvID = NetID;
    Obj->SendToProxy( ProxyDisconnect->ProxyID , sizeof(GSrvDisconnectNotify) , &GSrvDisconnectNotify );

    if( GSrvInfo->ProxyConnect.find( ProxyDisconnect->ProxyID ) == GSrvInfo->ProxyConnect.end() )
    {
        Print( LV2 , "_PGxGSrv_GSrvProxyDisconnect" , "GSrvInfo no register , ProxyID(%d) ????? proxy close?" , ProxyDisconnect->ProxyID  );
    }
    else
    {
        GSrvInfo->ProxyConnect.erase( NetID );
    }
}
//---------------------------------------------------------------------------------
//設定某GSrv　Active or Inactive
void	SwitchClass::_PGxSetGSrvActiveType( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
    Print( LV2 , "_PGxSetGSrvActiveType" , "_PGxSetGSrvActiveType" );
    SysNet_GSrv_Switch_SetActive*   SetActive = (SysNet_GSrv_Switch_SetActive*)PG;

    GSrvInfoStruct* GSrvInfo = Obj->_GSrvList[ NetID ];
    if(     GSrvInfo == NULL         )
    {
        Print( LV5 , "_PGxSetGSrvActiveType" , "_GetGSrv( %d ) GSrvInfo == NULL" , NetID );
        return;
    }
	if( GSrvInfo->Stat != EM_NetStat_Connect )
	{
		Print( LV2 , "_PGxSetGSrvActiveType" , "_GetGSrv( %d ) GSrvInfo->Stat != EM_NetStat_Connect" , NetID );
		GSrvInfo->Type = SetActive->Type;
		return;
	}

    if( GSrvInfo->Type == SetActive->Type )
        return;

    GSrvInfo->Type = SetActive->Type;
    Print( LV2 , "_PGxSetGSrvActiveType" , "notify All Proxy , Game Server mode was change" );
   
    SysNet_Switch_Proxy_SetActive   SendGSrvType;
    SendGSrvType.GSrvID = NetID;
    SendGSrvType.Type   = SetActive->Type;

    Obj->SendToAllProxy( sizeof(SendGSrvType) , &SendGSrvType );
}
//---------------------------------------------------------------------------------
//GSrv 轉送到 GSrv 的封包
void	SwitchClass::_PGxDataGSrvToGSrv( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
//    printf( "\n_PGxDataGSrvToGSrv 轉送封包 " );
    SysNet_GSrv_Switch_DataGSrvToGSrv*  DataGSrvToGSrv = (SysNet_GSrv_Switch_DataGSrvToGSrv*)PG;

    short GSrvID = DataGSrvToGSrv->GSrvID;
    //資料轉送
    DataGSrvToGSrv->GSrvID  = NetID;
    DataGSrvToGSrv->Command = EM_SysNet_Switch_GSrv_DataGSrvToGSrv;

    if( GSrvID == -1 )
    {
        Obj->SendToAllGSrv( Size , PG );
    }
    else
    {
        Obj->SendToGSrv( GSrvID , Size , PG );
    }
}
void	SwitchClass::_PGxDataGSrvToGSrv_Zip( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
	//    printf( "\n_PGxDataGSrvToGSrv 轉送封包 " );
	SysNet_GSrv_Switch_DataGSrvToGSrv_Zip*  DataGSrvToGSrv = (SysNet_GSrv_Switch_DataGSrvToGSrv_Zip*)PG;

	short GSrvID = DataGSrvToGSrv->GSrvID;
	//資料轉送
	DataGSrvToGSrv->GSrvID  = NetID;
	DataGSrvToGSrv->Command = EM_SysNet_Switch_GSrv_DataGSrvToGSrv_Zip;

	if( GSrvID == -1 )
	{
		Obj->SendToAllGSrv( Size , PG );
	}
	else
	{
		Obj->SendToGSrv( GSrvID , Size , PG );
	}
}
//---------------------------------------------------------------------------------
//GSrv 要求 與 Cli聯線
void	SwitchClass::_PGxConnectCliAndGSrv( SwitchClass* Obj , int NetID , PGSysNULL* Data , int Size )
{
	//printf( "\n__PGxConnectCliAndGSrv " );
	SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv*  PG = (SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv*)Data;

	CliInfoStruct*      CliInfo   = Obj->_CliList[ PG->CliID ]; 

	if( CliInfo == NULL  )
	{
		Print( LV5 , "_PGxConnectCliAndGSrv" , " CliID = %d , CliInfo == NULL" , PG->CliID );
		return;
	}

	Print( LV2 , "_PGxConnectCliAndGSrv" , "Account(%s) , Notify Proxy ,let GSrv(%d) and Cli(%d) to be connect" , CliInfo->Account.c_str() , PG->GSrvID , PG->CliID );
	Obj->SendToProxy( CliInfo->ProxyID , sizeof(SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv) , Data );
}

//GSrv 要求 與 Cli斷線
void	SwitchClass::_PGxDisconnectCliAndGSrv( SwitchClass* Obj , int NetID , PGSysNULL* Data , int Size )
{
	Print( LV2 , "_PGxDisconnectCliAndGSrv" , " _PGxDisconnectCliAndGSrv " );
	SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv*  PG = (SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv*)Data;


	CliInfoStruct*      CliInfo   = Obj->_CliList[ PG->CliID ]; 

	if( CliInfo == NULL  )
	{
		Print( LV5 , "_PGxDisconnectCliAndGSrv" , "CliID = %d , CliInfo == NULL" , PG->CliID );
		return;
	}
	Obj->SendToProxy( CliInfo->ProxyID , sizeof(SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv) , Data );
}
//---------------------------------------------------------------------------------
//GSrv 要求 某 GSrv Logout
void	SwitchClass::_PGxSrvLogoutRequest( SwitchClass* Obj , int NetID , PGSysNULL* Data , int Size )
{
	SysNet_GSrv_Switch_SrvLogoutRequest* PG = (SysNet_GSrv_Switch_SrvLogoutRequest*)Data;
	Print( LV2 , "_PGxSrvLogoutRequest" , "GSrvID = %d" , PG->SrvID );
	
	Obj->_NetBGSrv->Close( PG->SrvID );
}
//---------------------------------------------------------------------------------
//GSrv 通知Server的 loading狀況 
void SwitchClass::_PGxServerListInfo( SwitchClass* Obj , int NetID , PGSysNULL* Data , int Size )
{
	if( Obj->_SendServerListInfo == false )
		return;

	SysNet_GSrv_Switch_ServerListInfo* PG = (SysNet_GSrv_Switch_ServerListInfo*)Data;

	Obj->_SrvList.SendServerListInfo( Obj->_AcCli.GameGroupID() , Obj->_AcCli.GameID() , Obj->_WorldName.c_str() , Obj->_ClientHostIP.c_str() , Obj->_ClientPort , PG->MaxPlayerCount , PG->ParallelZoneCount , PG->PlayerCount , Obj->_ServerStatus , PG->TotalPlayerRate );
}
/*
//GSrv 要求兌換物品
void SwitchClass::_PGxExchangeItemRequet( SwitchClass* Obj , int NetID , PGSysNULL* Data , int Size )
{
	SysNet_GSrv_Switch_ExchangeItemRequet* PG = (SysNet_GSrv_Switch_ExchangeItemRequet*)Data;
	Obj->_AcCli.ItemExchangeRequest(  NetID , PG->RoleGUID , PG->UserAccount , PG->UserRoleName , PG->UserIP , PG->ItemKey , PG->ItemSerial );
}
*/

//GSrv 要求轉送資料到其他世界
void SwitchClass::_PGxDataTransmitToWorldGSrv( SwitchClass* Obj , int NetID , PGSysNULL* Data , int Size  )
{
	SysNet_X_DataTransmitToWorldGSrv* PG = (SysNet_X_DataTransmitToWorldGSrv*)Data;

	switch( PG->FromSrvType )
	{
	case EM_FromServerType_GSrv:
		{
			PG->FromSrvType = EM_FromServerType_Switch;
			PG->FromWorldID = m_WorldID;
			PG->FromNetID	= NetID;
			Obj->_NetGateway->Send( Obj->_GatewayNetID , Size , PG );
		}
		break;
	case EM_FromServerType_Gateway:
		{
			PG->FromSrvType = EM_FromServerType_Switch;
			Obj->_NetBGSrv->Send( PG->ToNetID , Size , PG );
		}
		break;
	default:
		break;
	}
}

//帳號冷凍要求
void SwitchClass::_PGx_FreezeAccount( SwitchClass* Obj , int NetID , PGSysNULL* Data , int Size  )
{
	SysNet_GSrv_Switch_FreezeAccount* PG = (SysNet_GSrv_Switch_FreezeAccount*)Data;
	Obj->_AcCli.FreezeAccount( PG->Account , PG->FreezeType );
}

void SwitchClass::_PGx_RegisterGSrvID( SwitchClass* Obj , int NetID , PGSysNULL* Data , int Size  )
{
	SysNet_GSrv_Switch_RegisterGSrvID* PG = (SysNet_GSrv_Switch_RegisterGSrvID*)Data;

	while( Obj->_NetIDtoGSrvIDList.size() <= PG->SrvType )
	{
		Map_NetIDtoIDClass Temp;
		Obj->_NetIDtoGSrvIDList.push_back( Temp );
	}

	Map_NetIDtoIDClass &NetIDtoGSrvID = Obj->_NetIDtoGSrvIDList[ PG->SrvType ];

//	int SrvID = PG->SrvType * 10000 + PG->SrvID;

	NetIDtoGSrvID.Cls( NetID , -1 );
	NetIDtoGSrvID.Cls( -1 , PG->SrvID );
	NetIDtoGSrvID.Set( NetID , PG->SrvID );
}

//GSrv 要求轉送資料到其他世界
void SwitchClass::_PGxDataTransmitToWorldGSrv_GSrvID( SwitchClass* Obj , int NetID , PGSysNULL* Data , int Size  )
{
	SysNet_X_DataTransmitToWorldGSrv_GSrvID* PG = (SysNet_X_DataTransmitToWorldGSrv_GSrvID*)Data;

	switch( PG->FromSrvType )
	{
	case EM_FromServerType_GSrv:
		{			
			PG->FromSrvType = EM_FromServerType_Switch;
			PG->FromWorldID = m_WorldID;
			PG->FromNetID	= NetID;
			Obj->_NetGateway->Send( Obj->_GatewayNetID , Size , PG );
		}
		break;
	case EM_FromServerType_Gateway:
		{
			if( Obj->_NetIDtoGSrvIDList.size() <= (unsigned)PG->ToGSrvType )
				return;

			Map_NetIDtoIDClass &NetIDtoGSrvID = Obj->_NetIDtoGSrvIDList[ PG->ToGSrvType ];

			int SendToNetID = NetIDtoGSrvID.GetNetID( PG->ToGSrvID );
			PG->FromSrvType = EM_FromServerType_Switch;
			Obj->_NetBGSrv->Send( SendToNetID , Size , PG );
		}
		break;
	default:
		break;
	}
}