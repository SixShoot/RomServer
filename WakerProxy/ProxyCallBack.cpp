#include "ProxyClass.h"
//---------------------------------------------------------------------------------
void	ProxyClass::_OnSwitchConnected       ( LNETID NetID )
{

    _NetSwitchID = NetID;
    //到Switch 要求登入
    SysNet_Proxy_Switch_ConnectRequest  ConnectRequest;
    SendToSwitch( sizeof( ConnectRequest ) , &ConnectRequest );
}
void	ProxyClass::_OnSwitchDisconnected    ( LNETID NetID )
{
    _NetSwitchID = -1;
}
bool	ProxyClass::_OnSwitchPacket          ( LNETID NetID , HBUFFER hIncomingData )
{

    int		Size;
    void	*Data;

    //取得封包資料
    ObjNetGetBuffer(hIncomingData,Size,Data);
    PGSysNULL* PG = ( PGSysNULL *)Data;

    //------------------------------------------------------------------------------


    //檢查是否為系統封包
    if( PG->SysIdentify == _Sys_Package_Number_ )
    {
        if(_PGInfo.size() <= (UINT32)PG->Command )
        {
            printf("\n(Switch:%d)_OnSwitchPacket : 封包cmd=%d過大" , NetID , PG->Command);
            return false;
        }

        //權限檢查
        PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );

        //封包大小檢查
        if(PGinfo->PGSize <= Size)
        {
            printf("\n(Switch:%d)_OnSwitchPacket : 封包Size=%d過大" , NetID , Size );
            return false;
        }

        //來源檢查
        if( PGinfo->Type.Switch == false )
        {
            printf("\n(Switch:%d)_OnSwitchPacket : 封包來源有問題 (PGinfo->Type.Switch == false ??)" , NetID );
            return false;
        }

        //呼叫Callback 處理
        _PGCallBack[ PG->Command ]( this , NetID , PG , Size );
    }

    return true;
}
void	ProxyClass::_OnSwitchConnectFailed   ( int NetID )
{
    //重新連線
    _Schedular.Push( _ReConnectSwitch , 5000 , this , NULL );

}

//重新連結 Switch Server
int    ProxyClass::_ReConnectSwitch( SchedularInfo* Obj , void* InputClass )
{
    printf( "\n Try to connect to Switch Server ....." );
    ProxyClass* Class = (ProxyClass*)InputClass;
    Class->_NetSwitch->Connect( &Class->_SwitchAddr , Class->_SwitchListener );

    return 0;
}
//-----------------------------------------------------------------------------------
// 初始化 Client 與 Server 的 CallBack函式
//-----------------------------------------------------------------------------------
void	ProxyClass::InitPGCallBack()
{

    int		i;
    PGBaseInfo	PGBase;
    //callback 函式陣列出始化
    for( i = 0 ; i <_Sys_Max_CallbackCount_ ; i++ )
    {
        _PGCallBack[i] = _PGxUnKnoew;
        _PGInfo.push_back(PGBase);
    }

    _PGInfo[ ENUM_SysNet_Switch_Proxy_ConnectResult ].Type.Switch = true;
    _PGCallBack[ ENUM_SysNet_Switch_Proxy_ConnectResult ]         = _PGSxConnectResult;

}
//----------------------------------------------------------------------------------
void     ProxyClass::_PGxUnKnoew( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size)
{
    printf( "\n(Switch:%d)_PGxUnKnoew : Command = %d" , NetID ,  PG->Command );
}
//----------------------------------------------------------------------------------
void    ProxyClass::_PGSxConnectResult( ProxyClass* Obj , int NetID , PGSysNULL* PG,int Size )
{
    printf( "\n(Switch:%d)_PGSxConnectResult" , NetID );
    SysNet_Switch_Proxy_ConnectResult*  ConnectResult = (SysNet_Switch_Proxy_ConnectResult*) PG;

    switch ( ConnectResult->Result )
    {
    case ENUM_Sys_PG_Result_OK :
            Obj->_ProxyID = ConnectResult->ProxyID;
            printf( "\n(Switch:%d)_PGSxConnectResult : Switch Connect Success !!" , NetID );
        break;
    default:
        printf( "\n(Switch:%d)_PGSxConnectResult : Switch Connect Faile !! Result = %d " , NetID , ConnectResult->Result );
        break;
    }
}
//----------------------------------------------------------------------------------
