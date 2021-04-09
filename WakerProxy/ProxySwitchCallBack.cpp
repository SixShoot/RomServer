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

    //���o�ʥ]���
//    ObjNetGetBuffer(hIncomingData,Size,Data);
    PGSysNULL* PG = ( PGSysNULL *)Data;

    //------------------------------------------------------------------------------

    //�ˬd�O�_���t�Ϋʥ]
//    if( PG->SysIdentify == _Sys_Package_Number_ )
    {
        if(_PGInfo.size() <= (UINT32)PG->Command )
        {
            Print( LV2 , "_OnSwitchPacket" , "(Switch:%d)_OnSwitchPacket : �ʥ]cmd=%d�L�j" , NetID , PG->Command);
            return false;
        }

        //�v���ˬd
        PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );

/*        //�ʥ]�j�p�ˬd
        if(PGinfo->PGSize <= Size)
        {
            Print( LV2 , "_OnSwitchPacket" , "(Switch:%d)_OnSwitchPacket : �ʥ]Size=%d�L�j cmd=%d " , NetID , Size , PG->Command );
            return false;
        }*/

        //�ӷ��ˬd
        if( PGinfo->Type.Switch == false )
        {
            Print( LV2 , "_OnSwitchPacket" , "(Switch:%d)_OnSwitchPacket : �ʥ]�ӷ������D (PGinfo->Type.Switch == false ??) cmd=%d " , NetID , PG->Command );
            return false;
        }

        //�I�sCallback �B�z
        _PGCallBack[ PG->Command ]( this , NetID , PG , Size );
    }

    return true;
}
void	ProxyClass::_OnSwitchConnectFailed   ( DWORD NetID )
{
    //���s�s�u
    _Schedular->Push( _ReConnectSwitch , 5000 , this , NULL );

}

//���s�s�� Switch Server
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
//�q����Game Server �n�s��
void    ProxyClass::_PGxGSrvConnectInfo( ProxyClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
    SysNet_Switch_Proxy_GSrvConnect* GSrvConnect = (SysNet_Switch_Proxy_GSrvConnect*)PG; 
    Print( LV1 , "_PGxGSrvConnectInfo" , "(Switch:%d)_PGxGSrvConnectInfo Switch �q������ Server( %d ) �n�s�J" , NetID, GSrvConnect->GSrvID );

    GSrvInfoStruct* GSrvInfo = Obj->_GSrvList.New( GSrvConnect->GSrvID );
    if( GSrvInfo == NULL )
    {
        Print(LV2 , "_PGxGSrvConnectInfo" , "(Switch:%d)_PGxGSrvConnectInfo (GSrvInfo == NULL)��ƲM�������D ??????" , NetID );
        return;
    }
	GSrvInfo->Init();
    GSrvInfo->Stat = EM_NetStat_Login;

	SysNet_Proxy_Switch_GSrvConnectPrepareOK Send;
	Send.GSrvID = GSrvConnect->GSrvID;
	Obj->SendToSwitch( sizeof( Send ) , &Send );
}
//----------------------------------------------------------------------------------
//Switch �n�D Proxy �P�Y GSrv �_�u
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
//�q���� Cli �n�n�J
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
				Print( LV5 , "_PGxCliLoginNotify" , "��CliID�w�s�b CliID(%d) ", CliConnect->CliID );
				TempCliInfo->NetCloseType = EM_ClientNetCloseType_ServerError;
				Obj->_NetBCli->Close( CliNetID );
				return;
			
			}
	}

    CliInfoStruct* CliInfo = Obj->_CliList.New( CliConnect->CliID );
    if( CliInfo == NULL )
    {
        Print( LV2 , "_PGxCliLoginNotify", "(Switch:%d)_PGxCliConnectInfo (CliInfo == NULL)��ƲM�������D ??????" , NetID );
        return;
    }


    CliInfo->Stat = EM_NetStat_Login;
    CliInfo->Account = CliConnect->Account;

	
	SysNet_Proxy_Switch_CliConnectPrepareOK SendPrepareOK;
	SendPrepareOK.CliInfo = CliConnect->CliInfo;
	Obj->SendToSwitch( sizeof( SendPrepareOK ) , &SendPrepareOK );
}
//----------------------------------------------------------------------------------