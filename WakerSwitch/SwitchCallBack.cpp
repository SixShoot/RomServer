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

    //���o�ʥ]���
    ObjNetGetBuffer( hIncomingData , Size , Data );
    PGSysNULL* PG = ( PGSysNULL *) Data;

    //------------------------------------------------------------------------------


    //�ˬd�O�_���t�Ϋʥ]
    if( PG->SysIdentify == _Sys_Package_Number_ )
    {
        if(_PGInfo.size() <= (UINT32)PG->Command)
        {
            printf("\n(Proxy:%d)_OnProxyPacket : �ʥ]cmd=%d�L�j" , NetID , PG->Command);
            return false;
        }

        //�v���ˬd
        PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );

        //�ʥ]�j�p�ˬd
        if( PGinfo->PGSize <= Size )
        {
            printf("\n(Proxy:%d)_OnProxyPacket : �ʥ]Size=%d�L�j" , NetID , Size );
            return false;
        }

        //�ӷ��ˬd
        if( PGinfo->Type.Proxy == false )
        {
            printf("\n(Proxy:%d)_OnProxyPacket : �ʥ]�ӷ������D (PGinfo->Type.Switch == false ??)" , NetID );
            return false;
        }

        //�I�sCallback �B�z
        _PGCallBack[PG->Command]( this , NetID , PG , Size );
    }

    return true;
}
void	SwitchClass::_OnProxyConnectFailed   ( int NetID )
{

}
//-----------------------------------------------------------------------------------
// ��l�� Client �P Server �� CallBack�禡
//-----------------------------------------------------------------------------------
void	SwitchClass::InitPGCallBack()
{

    int		i;
    PGBaseInfo	PGBase;
    //callback �禡�}�C�X�l��
    for( i = 0 ; i <_Sys_Max_CallbackCount_ ; i++ )
    {
        _PGCallBack[i] = _PGxUnKnoew;
        _PGInfo.push_back(PGBase);
    }

    _PGInfo[ ENUM_SysNet_Proxy_Switch_ConnectRequest ].Type.Proxy = true;
    _PGCallBack[ ENUM_SysNet_Proxy_Switch_ConnectRequest ]         = _PGSxProxyConnectRequest;

}
//----------------------------------------------------------------------------------
//�ˬdproxy ���U�O�_����
//�p�G�S���� �R�����s�u
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
