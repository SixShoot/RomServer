#include "ProxyClass.h"
//------------------------------------------------------------------------------
void	ProxyClass::_OnGSrvConnected       ( DWORD NetID )
{
    //�n�D GSrv �s����T
    SysNet_Proxy_GSrv_ConnectNotify Notify;
    Notify.ProxyID = _ProxyID;

    Print( LV1 , "_OnGSrvConnected" , "Proxy -> GSrv �n�D GSrv �n�J Proxy. �}�l���� 2000ms ����� Srv �^�ǵ��U�T��!" );
    SendToGSrv( NetID , sizeof( Notify ) , &Notify );
/*
    _Schedular->Push( _CheckGSrvConnectAndReg , 2000 , this 
        , "NetID" , EM_ValueType_int , NetID , NULL );
		*/
}
//-----------------------------------------------------------------------------------
void	ProxyClass::_OnGSrvDisconnected    ( DWORD NetID )
{
    //�q�� Switch  GSrv�_�u 
    SysNet_Proxy_Switch_GSrvDisconnect GSrvProxyDisconnect;
    int GSrvID = GSrvProxyDisconnect.GSrvID = _GSrvNetToIDMap.GetID( NetID );

    //²�d_GSrvConnect ��GSrvID ���S�����U
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

    //�q���Ҧ����s�즹 GSrv�� Cli
    set<int>::iterator Iter;
    int     CliID;
    CliInfoStruct* CliInfo;  
    for( Iter = GSrvInfo->CliConnect.begin() ; Iter != GSrvInfo->CliConnect.end() ; Iter++ )
    {
        CliID = *Iter;
        //�M�����
        CliInfo = _CliList[ CliID ];
        if(     CliInfo == NULL 
            ||  GSrvInfo->Stat != EM_NetStat_Connect )
        {
            Print( LV2 , "_OnGSrvDisconnected" , "������Cli(%d)��Ʀ����D " , CliID );
            continue;
        }

        if( CliInfo->GSrvConnect.erase( GSrvID ) == 0 )
        {
            Print( LV2 , "_OnGSrvDisconnected" , "������Cli(%d) ���� GSrv(%d) ���� �A�䤣�� GSrvID " , CliID , GSrvID );
            continue;
        }
        SendToCli_CliID( CliID , sizeof(GSrvOnDisConnect) , &GSrvOnDisConnect );   
    }
    
    //�q�� Switch GSrv�_�u
    Print( LV1 , "_OnGSrvDisconnected" , "\n�q�� Switch GSrvID = %d GSrvNetID = %d �_�u" , GSrvID , NetID );
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

    //���o�ʥ]���
//    ObjNetGetBuffer(hIncomingData,Size,Data);
    PGSysNULL* PG = ( PGSysNULL *)Data;

    //------------------------------------------------------------------------------


    //�ˬd�O�_���t�Ϋʥ]
//    if( PG->SysIdentify == _Sys_Package_Number_ )
    {
        if(_PGInfo.size() <= (UINT32)PG->Command )
        {
            Print( LV1 , "_OnGSrvPacket" , "(GSrv:%d)_OnGSrvPacket : �ʥ]cmd=%d�L�j" , NetID , PG->Command);
            return false;
        }

        //�v���ˬd
        PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );
/*
        //�ʥ]�j�p�ˬd
        if(PGinfo->MinPGSize > Size || PGinfo->MaxPGSize < Size )
        {
            Print( LV1 , "_OnGSrvPacket" , "(GSrv:%d)_OnGSrvPacket : �ʥ]Size=%d�����D cmd=%d" , NetID , Size , PG->Command );
            return false;
        }
*/
        //�ӷ��ˬd
        if( PGinfo->Type.GSrv == false )
        {
            Print( LV1 , "_OnGSrvPacket" , "(GSrv:%d)_OnGSrvPacket : �ʥ]�ӷ������D (PGinfo->Type.GSrv == false ??) cmd=%d" , NetID , PG->Command );
            return false;
        }

        //�I�sCallback �B�z
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
        Print( LV2 , "_PGxGSrvConnect" , "( GSrvInfo->IsReady == false ||  GSrvInfo->Stat != EM_NetStat_Login ) GSrv�n�J���~??????"  );
        return;
    }
    //�q�� Switch GSrv �s����?
    SysNet_Proxy_Switch_GSrvConnectOK   GSrvConnectOK;
    GSrvConnectOK.GSrvID = GSrvConnect->GSrvID;
   
    Print( LV1 , "_PGxGSrvConnect" , "Proxy -> Switch �q�� Switch �AGSrv �n�J OK " );
    Obj->SendToSwitch( sizeof(GSrvConnectOK) , &GSrvConnectOK );

    //�]�wGSrvNetID �P GSrvID �����p
    Obj->_GSrvNetToIDMap.Set( NetID , GSrvConnect->GSrvID );
    GSrvInfo->Stat = EM_NetStat_Connect;


    memcpy( GSrvInfo->GSrvName , GSrvConnect->GSrvName , sizeof(GSrvConnect->GSrvName) );

}
//�ˬdGSrv ���U�O�_����
//�p�G�S���� �R�����s�u
int  ProxyClass::_CheckGSrvConnectAndReg( SchedularInfo*	Obj , void* InputClass )
{
    ProxyClass* Class = ( ProxyClass* )InputClass;
    int NetID = Obj->GetNumber( "NetID" );
    int GSrvID = Class->_GSrvNetToIDMap.GetID( NetID );
    if( GSrvID == -1 )
    {
		Print( LV2 , "_CheckGSrvConnectAndReg" , "���ݮɶ��w�L. Srv( %d ) �|���V Proxy �{��" , NetID );
        Class->_NetBGSrv->Close( NetID );
		return 0;
    }

    return 0;
}
//-----------------------------------------------------------------------------------------------------------
//Client �n�D�����e�� GSrv
void    ProxyClass::_PGxDataGSrvToCli( ProxyClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
//    printf( "\n_PGxDataGSrvToCli" );
    SysNet_GSrv_Proxy_DataGSrvToCli*  DataGSrvToCli = (SysNet_GSrv_Proxy_DataGSrvToCli*)PG;

    //�ˬd�ʥ]�j�p
    if( Size != DataGSrvToCli->Size + sizeof( SysNet_GSrv_Proxy_DataGSrvToCli) )
    {
        Print( LV2 , "_PGxDataGSrvToCli" , "\nClient�ݭn�D�ʥ]��e��Ƥj�p�����D" );
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
        Print( LV2 , "_PGxDataGSrvToCli" , "��e��ƿ��~ CliID <=> NetID �����D???" );
        return;
    }
	*/

    CliInfoStruct*      CliInfo   = Obj->_CliList[ CliID ];

    if(     CliInfo == NULL 
        ||  CliInfo->Stat    != EM_NetStat_Connect       )
    {
        return;
    }

	//  �έp�ʥ]��T
	{
		int iPacketID = *((int*)DataGSrvToCli->Data);

		if( iPacketID > 0 && iPacketID < DF_MAX_PACKETID )
		{
			m_StoC_PacketID[ iPacketID ]++;
			m_StoC_Bandwidth[ iPacketID ] += DataGSrvToCli->Size;
		}
	}





    //�����e
    DataGSrvToCli->CliID  = GSrvID;
    DataGSrvToCli->Command = EM_SysNet_Proxy_Cli_DataGSrvToCli;
    Obj->SendToCli_CliID( CliID , Size , PG );

}
//-----------------------------------------------------------------------------------
//�n�D �Y GSrv �P Cli ���߳s�u
void    ProxyClass::_PGxConnectGSrvAndCli( ProxyClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
    SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv*    ConnectCliAndGSrv = (SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv*)PG;
    Obj->Connect_GSrv_Cli( ConnectCliAndGSrv->GSrvID , ConnectCliAndGSrv->CliID );
}
//-----------------------------------------------------------------------------------
//�n�D �Y GSrv �P Cli ���߳s�u
void    ProxyClass::_PGxDisconnectGSrvAndCli( ProxyClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
    SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv*    DisconnectCliAndGSrv = (SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv*)PG;
    Obj->Disconnect_GSrv_Cli( DisconnectCliAndGSrv->GSrvID , DisconnectCliAndGSrv->CliID );
}
//-----------------------------------------------------------------------------------
//�q��Cli���b�����Ƶn�J������
void    ProxyClass::_NotifyCliIDRelogin( ProxyClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
	SysNet_Switch_Proxy_NotifyCliIDRelogin*    NotifyCliIDRelogin = (SysNet_Switch_Proxy_NotifyCliIDRelogin*)PG;
	int CliNetID = Obj->_CliNetToIDMap.GetNetID( NotifyCliIDRelogin->CliID );
	if( CliNetID == -1 )
	{
		Print( LV3 , "_NotifyCliIDRelogin" , "\n NetID = -1  CliID = %d ( net id not find )" , NotifyCliIDRelogin->CliID  );
		//�q�� Switch Cli �_�u
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
//�n�D �Y Cli �����n�X
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
//�]�w�YGSrv�@Active or Inactive
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
    {//���Ҧ���Client �P�� GSrv �s�u
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