#include "ProxyClass.h"

//-----------------------------------------------------------------------------------
void	ProxyClass::_OnCliConnected       ( DWORD NetID )
{
/*	int CliID = _CliNetToIDMap.GetID( NetID );
	CliInfoStruct* CliInfo = _CliList[ CliID ];

	if(     CliInfo == NULL )
	{
		_NetBCli->Close( NetID );
		printf( "\n _OnCliConnected 找不到此NetID的資料!!****" );
		return;
	}
*/
    //通知 Cli 聯線 

	int IPNum;
	char IPStr[256];
	_NetBCli->GetConnectInfo( NetID , (DWORD*)&IPNum , NULL );
	ConvertIPtoString( IPNum , IPStr );


	Print( LV2 , "_OnCliConnected" , "_PGxGSrvConnect ID[%d][%s]" , NetID, IPStr );

	if( _PacketDelay == false )
		_NetBCli->SetSocketNoDelay( NetID );

    SysNet_Proxy_Cli_LoginInfoRequest   LoginInfoRequest;
    SendToCli( NetID , sizeof(LoginInfoRequest) , &LoginInfoRequest  );


	//暫時關閉
//	_Schedular->Push( _CheckCliConnectAndReg , 2000 , this 
//					, "NetID" , EM_ValueType_int , NetID , NULL );

}
//-----------------------------------------------------------------------------------
//檢查GSrv 註冊是否完成
//如果沒完成 刪除此連線
int  ProxyClass::_CheckCliConnectAndReg( SchedularInfo*	Obj , void* InputClass )
{
	ProxyClass* Class = ( ProxyClass* )InputClass;
	int NetID = Obj->GetNumber( "NetID" );
	int CliID = Class->_CliNetToIDMap.GetID( NetID );
	CliInfoStruct* CliInfo = Class->_CliList[ CliID ];

	if(     CliInfo == NULL
		||  CliInfo->Stat    != EM_NetStat_Connect )
	{
		if( CliInfo )
			CliInfo->NetCloseType = EM_ClientNetCloseType_ServerError;
		Class->_NetBCli->Close( NetID );
		Print( LV2 , "_CheckCliConnectAndReg" , "_CheckCliConnectAndReg 等待時間已過. NetID(%d) Cli( %d ) 尚未向 Proxy 認證" , NetID , CliID );
		return 0;
	}

	/*int GSrvID = Class->_CliNetToIDMap.GetID( NetID );
	if( GSrvID == -1 )
	{
		printf( "\n_CheckGSrvConnectAndReg 等待時間已過. Srv( %d ) 尚未向 Proxy 認證" , NetID );
		Class->_NetBGSrv->Close( NetID );
		return 0;
	}

	*/
	return 0;
}
//-----------------------------------------------------------------------------------
const char* GetClientCloseTypeStr( ClientNetCloseTypeENUM Type )
{
	switch( Type )
	{
	case EM_ClientNetCloseType_None:
		return "EM_ClientNetCloseType_None";
	case EM_ClientNetCloseType_ChangeWorld:	//選角換世界
		return "EM_ClientNetCloseType_ChangeWorld";
	case EM_ClientNetCloseType_CloseWindow:	//關閉視窗
		return "EM_ClientNetCloseType_CloseWindow";
	case EM_ClientNetCloseType_Relogin:	//退到選角畫面
		return "EM_ClientNetCloseType_Relogin";
	case EM_ClientNetCloseType_ReturnAccount:	//返迴登入界面(或重新登入)
		return "EM_ClientNetCloseType_ReturnAccount";

	case EM_ClientNetCloseType_ServerClose:				//Server要求斷線(返迴登入界面)
		return "EM_ClientNetCloseType_ServerClose";
	case EM_ClientNetCloseType_ServerError:				//Server要求斷線(連線有問題)
		return "EM_ClientNetCloseType_ServerError";
	case EM_ClientNetCloseType_ServerClose_ReloginKick:	//Server要求斷線(重複登入)
		return "EM_ClientNetCloseType_ServerClose_ReloginKick";
	default:
		return "Unknow";
	}
};
//-----------------------------------------------------------------------------------
void	ProxyClass::_OnCliDisconnected    ( DWORD NetID )
{

	int IPNum;
	char IPStr[256];
	_NetBCli->GetConnectInfo( NetID , (DWORD*)&IPNum , NULL );
	ConvertIPtoString( IPNum , IPStr );


	Print( LV2 , "_OnCliDisconnected" , "_OnCliDisconnected ID[%d] IP[%s] ", NetID, IPStr );
    int CliID = _CliNetToIDMap.GetID( NetID );
    CliInfoStruct* CliInfo = _CliList[ CliID ];

    if(     CliInfo == NULL
        ||  CliInfo->Stat    != EM_NetStat_Connect )
    {
        Print( LV2 , "_OnCliDisconnected" , "*** Client disconnect but it donest connect to proxy**** IP=%s" , IPStr );
        return;
    }

	Print( LV2 , "_OnCliDisconnected" , "Client CloseType=%s ", GetClientCloseTypeStr( CliInfo->NetCloseType ) );

	if( CliInfo->NetCloseType >= 0 && CliInfo->NetCloseType < EM_ClientNetCloseType_End )
	{
		m_iProxyDisconnectCount[ CliInfo->NetCloseType ] = m_iProxyDisconnectCount[ CliInfo->NetCloseType ] + 1;
	}


    //通知所有有連線的 GSrv   Cli 斷線
    SysNet_Proxy_GSrv_CliOnDisConnect   CliOnDisConnect;
    CliOnDisConnect.CliID = CliID;
        
    set<int>::iterator Iter;

    GSrvInfoStruct*     GSrvInfo;

    for( Iter = CliInfo->GSrvConnect.begin() ; Iter != CliInfo->GSrvConnect.end() ; Iter++ )
    {
        int GSrvID = *Iter;
        SendToGSrv_GSrvID( GSrvID , sizeof(CliOnDisConnect) , &CliOnDisConnect );
        GSrvInfo = _GSrvList[ GSrvID ];
        if( GSrvInfo == NULL )
        {
            Print( LV2 , "_OnCliDisconnected" , "problem in clear GSrvInfo");
            continue;
        }
        if( GSrvInfo->CliConnect.erase( CliID) == 0 )
        {
            Print( LV2 , "_OnCliDisconnected" , "GSrvInfo 內移除資料有問題!! CliID 不在 GSrvInfo內");
        }
    }

    //通知 Switch Cli 斷線
    SysNet_Proxy_Switch_CliLogout   CliLogout;
    CliLogout.CliID = CliID;
    SendToSwitch( sizeof(CliLogout) , &CliLogout );

    _CliList.Del( CliID );
    _CliNetToIDMap.Cls( NetID , CliID );

}
//-----------------------------------------------------------------------------------
bool	ProxyClass::_OnCliPacket          ( DWORD NetID, DWORD Size, void* Data )
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
            Print( LV3 , "_OnCliPacket" ,"(Cli:%d)_OnCliPacket : 封包cmd=%d過大 " , NetID , PG->Command );
            return false;
        }

        //權限檢查
        PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );

        //封包大小檢查
         if(PGinfo->MinPGSize > Size || PGinfo->MaxPGSize < Size )
        {
            Print( LV3 , "_OnCliPacket" , "(Cli:%d)_OnCliPacket : 封包Size=%d有問題 cmd=%d " , NetID , Size , PG->Command );
            return false;
        }

        //來源檢查
        if( PGinfo->Type.Client == false )
        {
            Print( LV3 , "_OnCliPacket" , "(Cli:%d)_OnCliPacket : 封包來源有問題 (PGinfo->Type.Client == false ??) cmd=%d " , NetID , PG->Command );
            return false;
        }

        //呼叫Callback 處理
        _PGCallBack[PG->Command]( this , NetID , PG , Size );
    }

    return true;
}
void	ProxyClass::_OnCliConnectFailed   ( DWORD NetID )
{

}
//-----------------------------------------------------------------------------------
//client 端要求斷線
void	ProxyClass::_PGxOnNetClose( ProxyClass* Obj , int NetID , PGSysNULL* PG,int Size )
{
    SysNet_Cli_Proxy_NetClose* NetClose = (SysNet_Cli_Proxy_NetClose*)PG;

	int CliID = Obj->_CliNetToIDMap.GetID( NetID );
	CliInfoStruct* CliInfo = Obj->_CliList[ CliID ];
	if( CliInfo == NULL )
	{
		Print( LV3 , "_PGxOnNetClose" , "CliInfo == NULL" );
		return;
	}
	CliInfo->NetCloseType = NetClose->NetCloseType;
	Obj->_NetBCli->Close( NetID );
}

//Client  送聯結資訊
void	ProxyClass::_PGxGCliConnect( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size)
{
    int     i;
    Print( LV1 , "_PGxGCliConnect" , "(CliNetID:%d)_PGxCliConnect" , NetID );
    SysNet_Cli_Proxy_LoginInfo* CliConnect = (SysNet_Cli_Proxy_LoginInfo*)PG;

    CliInfoStruct* CliInfo = Obj->_CliList[ CliConnect->CliID ];

    if(     CliInfo == NULL
        ||  CliInfo->Stat != EM_NetStat_Login )
    {
        Print( LV3 , "_PGxGCliConnect" , "( CliInfo->IsReady == false ||  CliInfo->Stat != EM_NetStat_Login ) Cli登入錯誤??????"  );
        return;
    }
    //通知 Switch Cli 連結成?
    SysNet_Proxy_Switch_CliLoginOK   CliConnectOK;
    CliConnectOK.CliID = CliConnect->CliID;

    Print( LV1 , "_PGxGCliConnect" ,"Proxy -> Switch 通知 Switch ，Cli 登入 OK " );
    Obj->SendToSwitch( sizeof(CliConnectOK) , &CliConnectOK );

    //設定CliNetID 與 CliID 的關聯
    Obj->_CliNetToIDMap.Set( NetID , CliConnect->CliID );
    CliInfo->Stat = EM_NetStat_Connect;
	Obj->_NetBCli->GetConnectInfo( NetID , (DWORD*)&(CliInfo->IPnum) , NULL );

    //---------------------------------------------------------------------------------
    //收尋所有 Active 的 GSrv 讓此 GSrv 與 Cli 聯結 

    //通知 Cli 與 GSrv Onconnect 事件
    GSrvInfoStruct* GSrvInfo;
    Obj->_GSrvList.GetData(true);
    for( i = 0 ; i < Obj->_GSrvList.size() ; i++ )
    {
        GSrvInfo = Obj->_GSrvList.GetData();
        if(     GSrvInfo == NULL 
			||  GSrvInfo->Stat != EM_NetStat_Connect 
			||  GSrvInfo->Type == EM_GSrvType_Passive )
            continue;

        Obj->Connect_GSrv_Cli(  GSrvInfo->ID , CliInfo->ID );
    }

	//通知Client 登入ProxyOK
	{
		SysNet_Proxy_Cli_LoginInfoOK LoginProxyOK;
		Obj->SendToCli( NetID , sizeof(LoginProxyOK) , &LoginProxyOK );
	}
}
//-----------------------------------------------------------------------------------------------------------
//Client 要求資料轉送到 GSrv
void    ProxyClass::_PGxDataCliToGSrv( ProxyClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
//    printf( "\n_PGxDataCliToGSrv" );
    SysNet_Cli_Proxy_DataCliToGSrv*  DataCliToGSrv = (SysNet_Cli_Proxy_DataCliToGSrv*)PG;

    //檢查封包大小
    //(*暫時*)
    if( Size != DataCliToGSrv->Size + sizeof( SysNet_Cli_Proxy_DataCliToGSrv) )
    {
        Print( LV2 , "_PGxDataCliToGSrv" , "Client端要求封包轉送資料大小有問題" );
        return;
    }

    int     CliID  = Obj->_CliNetToIDMap.GetID( NetID );
    int     GSrvID = DataCliToGSrv->GSrvID;

    if( CliID == -1 )
    {
        Print( LV2 , "_PGxDataCliToGSrv" , "轉送資料錯誤 CliID <=> NetID 有問題???" );
        return;
    }

    CliInfoStruct*      CliInfo   = Obj->_CliList[ CliID ];

    if(     CliInfo == NULL
        ||  CliInfo->Stat    != EM_NetStat_Connect )
    {
        return;
    }
    
    //檢查此Client與GSrv是否有連線
    if( CliInfo->GSrvConnect.find( GSrvID ) == CliInfo->GSrvConnect.end() )
    {
        Print( LV2 , "_PGxDataCliToGSrv" , "此Client ID=%d 與 GSrv ID=%d並未成立聯線?????" , CliID , GSrvID );
        return;
    }


	//  統計封包資訊
	{
		int iPacketID = *((int*)DataCliToGSrv->Data);

		if( iPacketID > 0 && iPacketID < DF_MAX_PACKETID )
		{
			m_CtoS_PacketID[ iPacketID ]++;
			m_CtoS_Bandwidth[ iPacketID ] += DataCliToGSrv->Size;
		}
	}


    //資料轉送
    DataCliToGSrv->GSrvID  = CliID;
    DataCliToGSrv->Command = EM_SysNet_Proxy_GSrv_DataCliToGSrv;
    Obj->SendToGSrv_GSrvID( GSrvID , Size , PG );
}
//-----------------------------------------------------------------------------------