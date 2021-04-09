#include "ProxyClass.h"
//---------------------------------------------------------------------------------
void	ProxyClass::_OnSwitchConnected       ( LNETID NetID )
{

    _NetSwitchID = NetID;
    //��Switch �n�D�n�J
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

    //���o�ʥ]���
    ObjNetGetBuffer(hIncomingData,Size,Data);
    PGSysNULL* PG = ( PGSysNULL *)Data;

    //------------------------------------------------------------------------------


    //�ˬd�O�_���t�Ϋʥ]
    if( PG->SysIdentify == _Sys_Package_Number_ )
    {
        if(_PGInfo.size() <= (UINT32)PG->Command )
        {
            printf("\n(Switch:%d)_OnSwitchPacket : �ʥ]cmd=%d�L�j" , NetID , PG->Command);
            return false;
        }

        //�v���ˬd
        PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );

        //�ʥ]�j�p�ˬd
        if(PGinfo->PGSize <= Size)
        {
            printf("\n(Switch:%d)_OnSwitchPacket : �ʥ]Size=%d�L�j" , NetID , Size );
            return false;
        }

        //�ӷ��ˬd
        if( PGinfo->Type.Switch == false )
        {
            printf("\n(Switch:%d)_OnSwitchPacket : �ʥ]�ӷ������D (PGinfo->Type.Switch == false ??)" , NetID );
            return false;
        }

        //�I�sCallback �B�z
        _PGCallBack[ PG->Command ]( this , NetID , PG , Size );
    }

    return true;
}
void	ProxyClass::_OnSwitchConnectFailed   ( int NetID )
{
    //���s�s�u
    _Schedular.Push( _ReConnectSwitch , 5000 , this , NULL );

}

//���s�s�� Switch Server
int    ProxyClass::_ReConnectSwitch( SchedularInfo* Obj , void* InputClass )
{
    printf( "\n Try to connect to Switch Server ....." );
    ProxyClass* Class = (ProxyClass*)InputClass;
    Class->_NetSwitch->Connect( &Class->_SwitchAddr , Class->_SwitchListener );

    return 0;
}
//-----------------------------------------------------------------------------------
// ��l�� Client �P Server �� CallBack�禡
//-----------------------------------------------------------------------------------
void	ProxyClass::InitPGCallBack()
{

    int		i;
    PGBaseInfo	PGBase;
    //callback �禡�}�C�X�l��
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
