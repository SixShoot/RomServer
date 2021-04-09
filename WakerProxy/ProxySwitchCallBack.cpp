#include "ProxyClass.h"
#include "RemotetableInput/RemotableInput.h"

//---------------------------------------------------------------------------------
void	ProxyClass::_OnSwitchConnected       ( DWORD NetID )
{
    _NetSwitchID = NetID;
//#ifndef NOTCONSOLEWIN
	char Buf[256];
	char OrgTitleStr[256];
	GetConsoleTitle( OrgTitleStr ,sizeof( OrgTitleStr ) );
	sprintf(Buf,"[OK]%s", OrgTitleStr );
	SetConsoleTitle(Buf);
//#else
	g_remotableInput.OutputStartupMessage();
//#endif
}
void	ProxyClass::_OnSwitchDisconnected    ( DWORD NetID )
{
    _NetSwitchID = -1;
}
bool	ProxyClass::_OnSwitchPacket          (  DWORD NetID, DWORD Size, void* Data )
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
            Print( LV2 , "_OnSwitchPacket" , "(Switch:%d)_OnSwitchPacket : 封包cmd=%d過大" , NetID , PG->Command);
            return false;
        }

        //權限檢查
        PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );

/*        //封包大小檢查
        if(PGinfo->PGSize <= Size)
        {
            Print( LV2 , "_OnSwitchPacket" , "(Switch:%d)_OnSwitchPacket : 封包Size=%d過大 cmd=%d " , NetID , Size , PG->Command );
            return false;
        }*/

        //來源檢查
        if( PGinfo->Type.Switch == false )
        {
            Print( LV2 , "_OnSwitchPacket" , "(Switch:%d)_OnSwitchPacket : 封包來源有問題 (PGinfo->Type.Switch == false ??) cmd=%d " , NetID , PG->Command );
            return false;
        }

        //呼叫Callback 處理
        _PGCallBack[ PG->Command ]( this , NetID , PG , Size );
    }

    return true;
}
void	ProxyClass::_OnSwitchConnectFailed   ( DWORD NetID )
{
    //重新連線
    _Schedular->Push( _ReConnectSwitch , 5000 , this , NULL );

}

//重新連結 Switch Server
int    ProxyClass::_ReConnectSwitch( SchedularInfo* Obj , void* InputClass )
{
    Print( LV1 , "_ReConnectSwitch" , " Try to connect to Switch Server ....." );
    ProxyClass* Class = (ProxyClass*)InputClass;

//    Class->_NetBSwitch->Connect( &Class->_SwitchAddr , Class->_SwitchBListener );
    Class->_NetBSwitch->Connect( Class->_SwitchIPNum , Class->_SwitchIPPort , Class->_SwitchBListener );

    return 0;
}

//----------------------------------------------------------------------------------
void     ProxyClass::_PGxUnKnoew( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size)
{
    Print( LV1 , "_PGxUnKnoew" , "(Switch:%d)_PGxUnKnoew : Command = %d" , NetID ,  PG->Command );
}
//----------------------------------------------------------------------------------
void    ProxyClass::_PGxConnectNotify( ProxyClass* Obj , int NetID , PGSysNULL* PG,int Size )
{
    Print( LV1 , "_PGxConnectNotify" , "(Switch:%d)_PGxConnectNotify" , NetID );
    SysNet_Switch_Proxy_ConnectNotify*  ConnectNotify = (SysNet_Switch_Proxy_ConnectNotify*) PG;
    
    Obj->_ProxyID = ConnectNotify->ProxyID;

    Obj->HostGSrv( (char*)Obj->_GSrvHostIP.c_str()  , Obj->_GSrvHostPort );
    Obj->HostCli ( (char*)Obj->_CliHostIP.c_str()   , Obj->_CliHostPort  );

    SysNet_Proxy_Switch_Connect Connect;
    Connect.GSrvPort = Obj->_GSrvHostPort;
    Connect.CliPort = Obj->_CliHostPort;
	strcpy( Connect.CliIPStr , Obj->_CliHostIP.c_str() );
	strcpy( Connect.GSrvIPStr , Obj->_GSrvHostIP.c_str() );
    Obj->SendToSwitch( sizeof( Connect ) , &Connect );

}
//----------------------------------------------------------------------------------
//通知有Game Server 要連結
void    ProxyClass::_PGxGSrvConnectInfo( ProxyClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
    SysNet_Switch_Proxy_GSrvConnect* GSrvConnect = (SysNet_Switch_Proxy_GSrvConnect*)PG; 
    Print( LV1 , "_PGxGSrvConnectInfo" , "(Switch:%d)_PGxGSrvConnectInfo Switch 通知有部 Server( %d ) 要連入" , NetID, GSrvConnect->GSrvID );

    GSrvInfoStruct* GSrvInfo = Obj->_GSrvList.New( GSrvConnect->GSrvID );
    if( GSrvInfo == NULL )
    {
        Print(LV2 , "_PGxGSrvConnectInfo" , "(Switch:%d)_PGxGSrvConnectInfo (GSrvInfo == NULL)資料清除有問題 ??????" , NetID );
        return;
    }
	GSrvInfo->Init();
    GSrvInfo->Stat = EM_NetStat_Login;

	SysNet_Proxy_Switch_GSrvConnectPrepareOK Send;
	Send.GSrvID = GSrvConnect->GSrvID;
	Obj->SendToSwitch( sizeof( Send ) , &Send );
}
//----------------------------------------------------------------------------------
//Switch 要求 Proxy 與某 GSrv 斷線
void    ProxyClass::_PGxGSrvDisconnectNodify( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size)
{
    SysNet_Switch_Proxy_GSrvDisconnectNotify*   DisconnectNotify = (SysNet_Switch_Proxy_GSrvDisconnectNotify*) PG;
    
    Print( LV2 , "_PGxGSrvDisconnectNodify" , "(Switch:%d)_PGxGSrvDisconnectNodify GSrvID = %d " , NetID ,  DisconnectNotify->GSrvID );
    int GSrvNetID = Obj->_GSrvNetToIDMap.GetNetID( DisconnectNotify->GSrvID );
    if( GSrvNetID == -1 )
    {
        Print( LV2 , "_PGxGSrvDisconnectNodify" , "GSrvID = %d , No connection!!" , DisconnectNotify->GSrvID );
        return;
    }

    Print( LV2 , "_PGxGSrvDisconnectNodify", "Switch request to disconnect with GSrvID = %d NetID = %d " , DisconnectNotify->GSrvID , GSrvNetID );
    Obj->_NetBGSrv->Close( GSrvNetID );
}
//----------------------------------------------------------------------------------
//通知有 Cli 要登入
void	ProxyClass::_PGxCliLoginNotify( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size)
{
    SysNet_Switch_Proxy_CliConnect* CliConnect = (SysNet_Switch_Proxy_CliConnect*)PG; 
    Print( LV1 , "_PGxCliLoginNotify", "(Switch:%d)_PGxCliConnectInfo CliID=%d" , NetID , CliConnect->CliID );

	CliInfoStruct* TempCliInfo = Obj->_CliList[ CliConnect->CliID ];


	if( TempCliInfo != NULL )
	{
			int CliNetID = Obj->_CliNetToIDMap.GetNetID( CliConnect->CliID );
			if( CliNetID == -1 && TempCliInfo->Stat == EM_NetStat_Login )
			{
				Obj->_CliList.Del( CliConnect->CliID );
			}
			else
			{
				Print( LV5 , "_PGxCliLoginNotify" , "此CliID已存在 CliID(%d) ", CliConnect->CliID );
				TempCliInfo->NetCloseType = EM_ClientNetCloseType_ServerError;
				Obj->_NetBCli->Close( CliNetID );
				return;
			
			}
	}

    CliInfoStruct* CliInfo = Obj->_CliList.New( CliConnect->CliID );
    if( CliInfo == NULL )
    {
        Print( LV2 , "_PGxCliLoginNotify", "(Switch:%d)_PGxCliConnectInfo (CliInfo == NULL)資料清除有問題 ??????" , NetID );
        return;
    }


    CliInfo->Stat = EM_NetStat_Login;
    CliInfo->Account = CliConnect->Account;

	
	SysNet_Proxy_Switch_CliConnectPrepareOK SendPrepareOK;
	SendPrepareOK.CliInfo = CliConnect->CliInfo;
	Obj->SendToSwitch( sizeof( SendPrepareOK ) , &SendPrepareOK );
}
//----------------------------------------------------------------------------------