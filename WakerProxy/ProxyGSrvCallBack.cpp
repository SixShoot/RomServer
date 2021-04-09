#include "ProxyClass.h"
//------------------------------------------------------------------------------
void	ProxyClass::_OnGSrvConnected       ( DWORD NetID )
{
    //要求 GSrv 連結資訊
    SysNet_Proxy_GSrv_ConnectNotify Notify;
    Notify.ProxyID = _ProxyID;

    Print( LV1 , "_OnGSrvConnected" , "Proxy -> GSrv 要求 GSrv 登入 Proxy. 開始等待 2000ms 直到該 Srv 回傳註冊訊息!" );
    SendToGSrv( NetID , sizeof( Notify ) , &Notify );
/*
    _Schedular->Push( _CheckGSrvConnectAndReg , 2000 , this 
        , "NetID" , EM_ValueType_int , NetID , NULL );
		*/
}
//-----------------------------------------------------------------------------------
void	ProxyClass::_OnGSrvDisconnected    ( DWORD NetID )
{
    //通知 Switch  GSrv斷線 
    SysNet_Proxy_Switch_GSrvDisconnect GSrvProxyDisconnect;
    int GSrvID = GSrvProxyDisconnect.GSrvID = _GSrvNetToIDMap.GetID( NetID );

    //簡查_GSrvConnect 此GSrvID 有沒有註冊
    GSrvInfoStruct* GSrvInfo;

    GSrvInfo = _GSrvList[ GSrvID ];

    if(     GSrvProxyDisconnect.GSrvID == -1 
        ||  GSrvInfo == NULL
        ||  GSrvInfo->Stat != EM_NetStat_Connect )
    {
        Print( LV2 , "_OnGSrvDisconnected" , "GSrvID(%d)! GSrvNetID = %d " , GSrvProxyDisconnect.GSrvID , NetID );
        _GSrvNetToIDMap.Cls( NetID , -1 );
        if( GSrvInfo != NULL )
		{
			_GSrvList.Del( GSrvID );
		}
        //    GSrvInfo->Init();
		
        return;
    }

    SysNet_Proxy_Cli_GSrvOnDisConnect GSrvOnDisConnect;
    GSrvOnDisConnect.GSrvID = GSrvID;

    //通知所有有連到此 GSrv的 Cli
    set<int>::iterator Iter;
    int     CliID;
    CliInfoStruct* CliInfo;  
    for( Iter = GSrvInfo->CliConnect.begin() ; Iter != GSrvInfo->CliConnect.end() ; Iter++ )
    {
        CliID = *Iter;
        //清除資料
        CliInfo = _CliList[ CliID ];
        if(     CliInfo == NULL 
            ||  GSrvInfo->Stat != EM_NetStat_Connect )
        {
            Print( LV2 , "_OnGSrvDisconnected" , "此網路Cli(%d)資料有問題 " , CliID );
            continue;
        }

        if( CliInfo->GSrvConnect.erase( GSrvID ) == 0 )
        {
            Print( LV2 , "_OnGSrvDisconnected" , "此網路Cli(%d) 移除 GSrv(%d) 失敗 ，找不到 GSrvID " , CliID , GSrvID );
            continue;
        }
        SendToCli_CliID( CliID , sizeof(GSrvOnDisConnect) , &GSrvOnDisConnect );   
    }
    
    //通知 Switch GSrv斷線
    Print( LV1 , "_OnGSrvDisconnected" , "\n通知 Switch GSrvID = %d GSrvNetID = %d 斷線" , GSrvID , NetID );
    SendToSwitch( sizeof(GSrvProxyDisconnect) , &GSrvProxyDisconnect );

    _GSrvNetToIDMap.Cls( NetID , GSrvID );
    //GSrvInfo->Init();
	_GSrvList.Del( GSrvID );
    
    
}
//-----------------------------------------------------------------------------------
bool	ProxyClass::_OnGSrvPacket          (  DWORD NetID, DWORD Size, void* Data )
{
//    int		Size;
//    void	*Data;

    //取得封包資料
//    ObjNetGetBuffer(hIncomingData,Size,Data);
    PGSysNULL* PG = ( PGSysNULL *)Data;

    //------------------------------------------------------------------------------


    //檢查是否為系統封包
//    if( PG->SysIdentify == _Sys_Package_Number_ )
    {
        if(_PGInfo.size() <= (UINT32)PG->Command )
        {
            Print( LV1 , "_OnGSrvPacket" , "(GSrv:%d)_OnGSrvPacket : 封包cmd=%d過大" , NetID , PG->Command);
            return false;
        }

        //權限檢查
        PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );
/*
        //封包大小檢查
        if(PGinfo->MinPGSize > Size || PGinfo->MaxPGSize < Size )
        {
            Print( LV1 , "_OnGSrvPacket" , "(GSrv:%d)_OnGSrvPacket : 封包Size=%d有問題 cmd=%d" , NetID , Size , PG->Command );
            return false;
        }
*/
        //來源檢查
        if( PGinfo->Type.GSrv == false )
        {
            Print( LV1 , "_OnGSrvPacket" , "(GSrv:%d)_OnGSrvPacket : 封包來源有問題 (PGinfo->Type.GSrv == false ??) cmd=%d" , NetID , PG->Command );
            return false;
        }

        //呼叫Callback 處理
        _PGCallBack[ PG->Command ]( this , NetID , PG , Size );
    }

    return true;

}
void	ProxyClass::_OnGSrvConnectFailed   ( DWORD NetID )
{

}
//------------------------------------------------------------------------------
void	ProxyClass::_PGxOnPingLog( ProxyClass* Obj , int NetID , PGSysNULL* PG,int Size )
{
	SysNet_GSrv_Proxy_PingLog*   PingLogPG = (SysNet_GSrv_Proxy_PingLog*) PG;

	SysNet_Proxy_GSrv_PingLog Send;
	Send.ProxyID = Obj->_ProxyID;
	Send.Time = PingLogPG->Time;
	Obj->SendToGSrv( NetID , sizeof(Send) , &Send );
}

void	ProxyClass::_PGxGSrvConnect( ProxyClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
    Print( LV1 , "_PGxGSrvConnect" , "(GSrv:%d)_PGxGSrvConnect" , NetID );
    SysNet_GSrv_Proxy_Connect*   GSrvConnect = (SysNet_GSrv_Proxy_Connect*) PG;

    GSrvInfoStruct* GSrvInfo = Obj->_GSrvList[ GSrvConnect->GSrvID ];

    if(     GSrvInfo == NULL
        ||  GSrvInfo->Stat != EM_NetStat_Login )
    {
        Print( LV2 , "_PGxGSrvConnect" , "( GSrvInfo->IsReady == false ||  GSrvInfo->Stat != EM_NetStat_Login ) GSrv登入錯誤??????"  );
        return;
    }
    //通知 Switch GSrv 連結成?
    SysNet_Proxy_Switch_GSrvConnectOK   GSrvConnectOK;
    GSrvConnectOK.GSrvID = GSrvConnect->GSrvID;
   
    Print( LV1 , "_PGxGSrvConnect" , "Proxy -> Switch 通知 Switch ，GSrv 登入 OK " );
    Obj->SendToSwitch( sizeof(GSrvConnectOK) , &GSrvConnectOK );

    //設定GSrvNetID 與 GSrvID 的關聯
    Obj->_GSrvNetToIDMap.Set( NetID , GSrvConnect->GSrvID );
    GSrvInfo->Stat = EM_NetStat_Connect;


    memcpy( GSrvInfo->GSrvName , GSrvConnect->GSrvName , sizeof(GSrvConnect->GSrvName) );

}
//檢查GSrv 註冊是否完成
//如果沒完成 刪除此連線
int  ProxyClass::_CheckGSrvConnectAndReg( SchedularInfo*	Obj , void* InputClass )
{
    ProxyClass* Class = ( ProxyClass* )InputClass;
    int NetID = Obj->GetNumber( "NetID" );
    int GSrvID = Class->_GSrvNetToIDMap.GetID( NetID );
    if( GSrvID == -1 )
    {
		Print( LV2 , "_CheckGSrvConnectAndReg" , "等待時間已過. Srv( %d ) 尚未向 Proxy 認證" , NetID );
        Class->_NetBGSrv->Close( NetID );
		return 0;
    }

    return 0;
}
//-----------------------------------------------------------------------------------------------------------
//Client 要求資料轉送到 GSrv
void    ProxyClass::_PGxDataGSrvToCli( ProxyClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
//    printf( "\n_PGxDataGSrvToCli" );
    SysNet_GSrv_Proxy_DataGSrvToCli*  DataGSrvToCli = (SysNet_GSrv_Proxy_DataGSrvToCli*)PG;

    //檢查封包大小
    if( Size != DataGSrvToCli->Size + sizeof( SysNet_GSrv_Proxy_DataGSrvToCli) )
    {
        Print( LV2 , "_PGxDataGSrvToCli" , "\nClient端要求封包轉送資料大小有問題" );
        return;
    }

    int     CliID  = DataGSrvToCli->CliID;
    int     GSrvID = Obj->_GSrvNetToIDMap.GetID( NetID );

	if( CliID == -1 )
	{
		DataGSrvToCli->CliID  = GSrvID;
		DataGSrvToCli->Command = EM_SysNet_Proxy_Cli_DataGSrvToCli;
		Obj->SendToAllCli( Size , PG );
		return;
	}
/*
    if( CliID == -1 )
    {
        Print( LV2 , "_PGxDataGSrvToCli" , "轉送資料錯誤 CliID <=> NetID 有問題???" );
        return;
    }
	*/

    CliInfoStruct*      CliInfo   = Obj->_CliList[ CliID ];

    if(     CliInfo == NULL 
        ||  CliInfo->Stat    != EM_NetStat_Connect       )
    {
        return;
    }

	//  統計封包資訊
	{
		int iPacketID = *((int*)DataGSrvToCli->Data);

		if( iPacketID > 0 && iPacketID < DF_MAX_PACKETID )
		{
			m_StoC_PacketID[ iPacketID ]++;
			m_StoC_Bandwidth[ iPacketID ] += DataGSrvToCli->Size;
		}
	}





    //資料轉送
    DataGSrvToCli->CliID  = GSrvID;
    DataGSrvToCli->Command = EM_SysNet_Proxy_Cli_DataGSrvToCli;
    Obj->SendToCli_CliID( CliID , Size , PG );

}
//-----------------------------------------------------------------------------------
//要求 某 GSrv 與 Cli 成立連線
void    ProxyClass::_PGxConnectGSrvAndCli( ProxyClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
    SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv*    ConnectCliAndGSrv = (SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv*)PG;
    Obj->Connect_GSrv_Cli( ConnectCliAndGSrv->GSrvID , ConnectCliAndGSrv->CliID );
}
//-----------------------------------------------------------------------------------
//要求 某 GSrv 與 Cli 成立連線
void    ProxyClass::_PGxDisconnectGSrvAndCli( ProxyClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
    SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv*    DisconnectCliAndGSrv = (SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv*)PG;
    Obj->Disconnect_GSrv_Cli( DisconnectCliAndGSrv->GSrvID , DisconnectCliAndGSrv->CliID );
}
//-----------------------------------------------------------------------------------
//通知Cli有帳號重複登入此角色
void    ProxyClass::_NotifyCliIDRelogin( ProxyClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
	SysNet_Switch_Proxy_NotifyCliIDRelogin*    NotifyCliIDRelogin = (SysNet_Switch_Proxy_NotifyCliIDRelogin*)PG;
	int CliNetID = Obj->_CliNetToIDMap.GetNetID( NotifyCliIDRelogin->CliID );
	if( CliNetID == -1 )
	{
		Print( LV3 , "_NotifyCliIDRelogin" , "\n NetID = -1  CliID = %d ( net id not find )" , NotifyCliIDRelogin->CliID  );
		//通知 Switch Cli 斷線
		SysNet_Proxy_Switch_CliLogout   CliLogout;
		CliLogout.CliID = NotifyCliIDRelogin->CliID;
		Obj->SendToSwitch( sizeof(CliLogout) , &CliLogout );
		return;
	}
	CliInfoStruct*      CliInfo   = Obj->_CliList[ NotifyCliIDRelogin->CliID ];
	if( CliInfo != NULL )
	{
		CliInfo->NetCloseType = EM_ClientNetCloseType_ServerClose_ReloginKick;
	}

	Obj->_NetBCli->Close( CliNetID );
	/*
	SysNet_Proxy_Cli_NotifyCliIDRelogin Send;
	Send.IPNum =  NotifyCliIDRelogin->IPNum;

    Obj->SendToCli_CliID(  NotifyCliIDRelogin->CliID , sizeof(Send) , &Send );
	*/
}
//-----------------------------------------------------------------------------------
//要求 某 Cli 完全登出
void    ProxyClass::_PGxCliLogout( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size)
{
    SysNet_Proxy_CliLogoutRequest*    CliLogout = (SysNet_Proxy_CliLogoutRequest*)PG;

	Print( LV3 , "_PGxCliLogout" , "(GSrv:%d) Srv Request CliLogout = %s" , NetID, CliLogout->Reason );

	CliInfoStruct*      CliInfo   = Obj->_CliList[ CliLogout->CliID];
	if( CliInfo != NULL )
	{
		CliInfo->NetCloseType = EM_ClientNetCloseType_ServerClose;
	}
	
 
    int CliNetID = Obj->_CliNetToIDMap.GetNetID( CliLogout->CliID );
    Obj->_NetBCli->Close( CliNetID );
}
//-----------------------------------------------------------------------------------
//設定某GSrv　Active or Inactive
void	ProxyClass::_PGxSetGSrvActiveType( ProxyClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
    int     i;
    Print( LV1 , "_PGxSetGSrvActiveType" , "\n(GSrv:%d)_PGxSetGSrvActiveType" , NetID );
    SysNet_Switch_Proxy_SetActive* ActiveType = (SysNet_Switch_Proxy_SetActive*)PG;

    GSrvInfoStruct* GSrvInfo = Obj->_GSrvList[ ActiveType->GSrvID ];
    if(     GSrvInfo == NULL
        ||  GSrvInfo->Stat    != EM_NetStat_Connect )  
    {
        return;
    }

    if( ActiveType->Type == GSrvInfo->Type )
        return;

    GSrvInfo->Type = ActiveType->Type;



    CliInfoStruct* CliInfo;
    if( ActiveType->Type == EM_GSrvType_Active )
    {//讓所有的Client 與此 GSrv 連線
        Obj->_CliList.GetData( true );
        for( i = 0 ; i < Obj->_CliList.size() ; i++ )
        {
            CliInfo = Obj->_CliList.GetData( );
            if(     CliInfo == NULL  
                ||  CliInfo->Stat != EM_NetStat_Connect )
                continue;
            Obj->Connect_GSrv_Cli(  ActiveType->GSrvID , CliInfo->ID );
        }

    }
}
//-----------------------------------------------------------------------------------