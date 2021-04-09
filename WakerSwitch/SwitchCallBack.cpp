#include "SwitchClass.h"
//---------------------------------------------------------------------------------
void	SwitchClass::_OnProxyConnected       ( LNETID NetID )
{
    sockaddr_in pAddr;
    _NetProxy->GetPeerAddress( NetID ,&pAddr );

    ProxyInfoStruct Info;
    Info.ID     = NetID;
    Info.IPnum  = pAddr.sin_addr.S_un.S_addr;
    Info.Ready  = true;
    Info.Stat   = ENUM_NetStat_Login;

    if( _AddProxyInfo( &Info ) == false )
    {
        printf( "\n(Proxy:%d)_OnProxyConnected : _AddProxyInfo false!!" , NetID  );
        return;
    }

    _Schedular.Push( _CheckProxyConnectAndReg , 2000 , this 
        , "ProxyID" , ENUM_ValueType_int , NetID , NULL );

}
void	SwitchClass::_OnProxyDisconnected    ( LNETID NetID )
{
    if( _DelProxyInfo( NetID ) == false )
    {
        printf( "\n(Proxy:%d)_OnProxyDisconnected : _DelProxyInfo false!!" , NetID  );
    }
        
}
bool	SwitchClass::_OnProxyPacket          ( LNETID NetID , HBUFFER hIncomingData )
{

    int		Size;
    void	*Data;

    //取得封包資料
    ObjNetGetBuffer( hIncomingData , Size , Data );
    PGSysNULL* PG = ( PGSysNULL *) Data;

    //------------------------------------------------------------------------------


    //檢查是否為系統封包
    if( PG->SysIdentify == _Sys_Package_Number_ )
    {
        if(_PGInfo.size() <= (UINT32)PG->Command)
        {
            printf("\n(Proxy:%d)_OnProxyPacket : 封包cmd=%d過大" , NetID , PG->Command);
            return false;
        }

        //權限檢查
        PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );

        //封包大小檢查
        if( PGinfo->PGSize <= Size )
        {
            printf("\n(Proxy:%d)_OnProxyPacket : 封包Size=%d過大" , NetID , Size );
            return false;
        }

        //來源檢查
        if( PGinfo->Type.Proxy == false )
        {
            printf("\n(Proxy:%d)_OnProxyPacket : 封包來源有問題 (PGinfo->Type.Switch == false ??)" , NetID );
            return false;
        }

        //呼叫Callback 處理
        _PGCallBack[PG->Command]( this , NetID , PG , Size );
    }

    return true;
}
void	SwitchClass::_OnProxyConnectFailed   ( int NetID )
{

}
//-----------------------------------------------------------------------------------
// 初始化 Client 與 Server 的 CallBack函式
//-----------------------------------------------------------------------------------
void	SwitchClass::InitPGCallBack()
{

    int		i;
    PGBaseInfo	PGBase;
    //callback 函式陣列出始化
    for( i = 0 ; i <_Sys_Max_CallbackCount_ ; i++ )
    {
        _PGCallBack[i] = _PGxUnKnoew;
        _PGInfo.push_back(PGBase);
    }

    _PGInfo[ ENUM_SysNet_Proxy_Switch_ConnectRequest ].Type.Proxy = true;
    _PGCallBack[ ENUM_SysNet_Proxy_Switch_ConnectRequest ]         = _PGSxProxyConnectRequest;

}
//----------------------------------------------------------------------------------
//檢查proxy 註冊是否完成
//如果沒完成 刪除此連線
int     SwitchClass::_CheckProxyConnectAndReg( SchedularInfo*	Obj , void* InputClass )
{
    SwitchClass*    Class = (SwitchClass*)InputClass;
    int NetID = Obj->GetNumber( "ProxyID" );


    ProxyInfoStruct *Info = Class->_GetProxy( NetID );
    
    if(     Info->Ready == false 
        ||  Info->Stat != ENUM_NetStat_Connect )
    {
        Class->_NetProxy->Close( NetID );
    }
    return 0;
}
//-----------------------------------------------------------------------------------
void     SwitchClass::_PGxUnKnoew( SwitchClass* Obj,int NetID,PGSysNULL* PG,int Size)
{
    printf( "\n(Proxy:%d)_PGxUnKnoew : Command = %d" , NetID , PG->Command );
}
//----------------------------------------------------------------------------------

void    SwitchClass::_PGSxProxyConnectRequest( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size )
{
    printf("\n(Proxy:%d)_PGSxProxyConnectRequest" , NetID );

    SysNet_Proxy_Switch_ConnectRequest*  ConnectRequest = (SysNet_Proxy_Switch_ConnectRequest*) PG;

    SysNet_Switch_Proxy_ConnectResult Result;
    Result.ProxyID = NetID;
    Result.Result  = ENUM_Sys_PG_Result_OK;
    

    ProxyInfoStruct *Info = Obj->_GetProxy( NetID );
    if(     Info->Ready == false 
        ||  Info->Stat != ENUM_NetStat_Login )
    {
        printf( "\n_PGSxProxyConnectRequest : _GetProxy( %d ) == NULL" , NetID );
        return;
    }
    Info->Stat   = ENUM_NetStat_Connect;
 
    Obj->SendToProxy( NetID , sizeof( Result ) , &Result );
}

//----------------------------------------------------------------------------------
