#include "ProxyClass.h"

//-----------------------------------------------------------------------------------
void	ProxyClass::_OnCliConnected       ( DWORD NetID )
{
/*	int CliID = _CliNetToIDMap.GetID( NetID );
	CliInfoStruct* CliInfo = _CliList[ CliID ];

	if(     CliInfo == NULL )
	{
		_NetBCli->Close( NetID );
		printf( "\n _OnCliConnected �䤣�즹NetID�����!!****" );
		return;
	}
*/
    //�q�� Cli �p�u 

	int IPNum;
	char IPStr[256];
	_NetBCli->GetConnectInfo( NetID , (DWORD*)&IPNum , NULL );
	ConvertIPtoString( IPNum , IPStr );


	Print( LV2 , "_OnCliConnected" , "_PGxGSrvConnect ID[%d][%s]" , NetID, IPStr );

	if( _PacketDelay == false )
		_NetBCli->SetSocketNoDelay( NetID );

    SysNet_Proxy_Cli_LoginInfoRequest   LoginInfoRequest;
    SendToCli( NetID , sizeof(LoginInfoRequest) , &LoginInfoRequest  );


	//�Ȯ�����
//	_Schedular->Push( _CheckCliConnectAndReg , 2000 , this 
//					, "NetID" , EM_ValueType_int , NetID , NULL );

}
//-----------------------------------------------------------------------------------
//�ˬdGSrv ���U�O�_����
//�p�G�S���� �R�����s�u
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
		Print( LV2 , "_CheckCliConnectAndReg" , "_CheckCliConnectAndReg ���ݮɶ��w�L. NetID(%d) Cli( %d ) �|���V Proxy �{��" , NetID , CliID );
		return 0;
	}

	/*int GSrvID = Class->_CliNetToIDMap.GetID( NetID );
	if( GSrvID == -1 )
	{
		printf( "\n_CheckGSrvConnectAndReg ���ݮɶ��w�L. Srv( %d ) �|���V Proxy �{��" , NetID );
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
	case EM_ClientNetCloseType_ChangeWorld:	//�﨤���@��
		return "EM_ClientNetCloseType_ChangeWorld";
	case EM_ClientNetCloseType_CloseWindow:	//��������
		return "EM_ClientNetCloseType_CloseWindow";
	case EM_ClientNetCloseType_Relogin:	//�h��﨤�e��
		return "EM_ClientNetCloseType_Relogin";
	case EM_ClientNetCloseType_ReturnAccount:	//��j�n�J�ɭ�(�έ��s�n�J)
		return "EM_ClientNetCloseType_ReturnAccount";

	case EM_ClientNetCloseType_ServerClose:				//Server�n�D�_�u(��j�n�J�ɭ�)
		return "EM_ClientNetCloseType_ServerClose";
	case EM_ClientNetCloseType_ServerError:				//Server�n�D�_�u(�s�u�����D)
		return "EM_ClientNetCloseType_ServerError";
	case EM_ClientNetCloseType_ServerClose_ReloginKick:	//Server�n�D�_�u(���Ƶn�J)
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


    //�q���Ҧ����s�u�� GSrv   Cli �_�u
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
            Print( LV2 , "_OnCliDisconnected" , "GSrvInfo ��������Ʀ����D!! CliID ���b GSrvInfo��");
        }
    }

    //�q�� Switch Cli �_�u
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

    //���o�ʥ]���
//    ObjNetGetBuffer( hIncomingData , Size , Data );
    PGSysNULL* PG = ( PGSysNULL *) Data;

    //------------------------------------------------------------------------------


    //�ˬd�O�_���t�Ϋʥ]
//    if( PG->SysIdentify == _Sys_Package_Number_ )
    {
        if(_PGInfo.size() <= (UINT32)PG->Command)
        {
            Print( LV3 , "_OnCliPacket" ,"(Cli:%d)_OnCliPacket : �ʥ]cmd=%d�L�j " , NetID , PG->Command );
            return false;
        }

        //�v���ˬd
        PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );

        //�ʥ]�j�p�ˬd
         if(PGinfo->MinPGSize > Size || PGinfo->MaxPGSize < Size )
        {
            Print( LV3 , "_OnCliPacket" , "(Cli:%d)_OnCliPacket : �ʥ]Size=%d�����D cmd=%d " , NetID , Size , PG->Command );
            return false;
        }

        //�ӷ��ˬd
        if( PGinfo->Type.Client == false )
        {
            Print( LV3 , "_OnCliPacket" , "(Cli:%d)_OnCliPacket : �ʥ]�ӷ������D (PGinfo->Type.Client == false ??) cmd=%d " , NetID , PG->Command );
            return false;
        }

        //�I�sCallback �B�z
        _PGCallBack[PG->Command]( this , NetID , PG , Size );
    }

    return true;
}
void	ProxyClass::_OnCliConnectFailed   ( DWORD NetID )
{

}
//-----------------------------------------------------------------------------------
//client �ݭn�D�_�u
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

//Client  �e�p����T
void	ProxyClass::_PGxGCliConnect( ProxyClass* Obj,int NetID,PGSysNULL* PG,int Size)
{
    int     i;
    Print( LV1 , "_PGxGCliConnect" , "(CliNetID:%d)_PGxCliConnect" , NetID );
    SysNet_Cli_Proxy_LoginInfo* CliConnect = (SysNet_Cli_Proxy_LoginInfo*)PG;

    CliInfoStruct* CliInfo = Obj->_CliList[ CliConnect->CliID ];

    if(     CliInfo == NULL
        ||  CliInfo->Stat != EM_NetStat_Login )
    {
        Print( LV3 , "_PGxGCliConnect" , "( CliInfo->IsReady == false ||  CliInfo->Stat != EM_NetStat_Login ) Cli�n�J���~??????"  );
        return;
    }
    //�q�� Switch Cli �s����?
    SysNet_Proxy_Switch_CliLoginOK   CliConnectOK;
    CliConnectOK.CliID = CliConnect->CliID;

    Print( LV1 , "_PGxGCliConnect" ,"Proxy -> Switch �q�� Switch �ACli �n�J OK " );
    Obj->SendToSwitch( sizeof(CliConnectOK) , &CliConnectOK );

    //�]�wCliNetID �P CliID �����p
    Obj->_CliNetToIDMap.Set( NetID , CliConnect->CliID );
    CliInfo->Stat = EM_NetStat_Connect;
	Obj->_NetBCli->GetConnectInfo( NetID , (DWORD*)&(CliInfo->IPnum) , NULL );

    //---------------------------------------------------------------------------------
    //���M�Ҧ� Active �� GSrv ���� GSrv �P Cli �p�� 

    //�q�� Cli �P GSrv Onconnect �ƥ�
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

	//�q��Client �n�JProxyOK
	{
		SysNet_Proxy_Cli_LoginInfoOK LoginProxyOK;
		Obj->SendToCli( NetID , sizeof(LoginProxyOK) , &LoginProxyOK );
	}
}
//-----------------------------------------------------------------------------------------------------------
//Client �n�D�����e�� GSrv
void    ProxyClass::_PGxDataCliToGSrv( ProxyClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
//    printf( "\n_PGxDataCliToGSrv" );
    SysNet_Cli_Proxy_DataCliToGSrv*  DataCliToGSrv = (SysNet_Cli_Proxy_DataCliToGSrv*)PG;

    //�ˬd�ʥ]�j�p
    //(*�Ȯ�*)
    if( Size != DataCliToGSrv->Size + sizeof( SysNet_Cli_Proxy_DataCliToGSrv) )
    {
        Print( LV2 , "_PGxDataCliToGSrv" , "Client�ݭn�D�ʥ]��e��Ƥj�p�����D" );
        return;
    }

    int     CliID  = Obj->_CliNetToIDMap.GetID( NetID );
    int     GSrvID = DataCliToGSrv->GSrvID;

    if( CliID == -1 )
    {
        Print( LV2 , "_PGxDataCliToGSrv" , "��e��ƿ��~ CliID <=> NetID �����D???" );
        return;
    }

    CliInfoStruct*      CliInfo   = Obj->_CliList[ CliID ];

    if(     CliInfo == NULL
        ||  CliInfo->Stat    != EM_NetStat_Connect )
    {
        return;
    }
    
    //�ˬd��Client�PGSrv�O�_���s�u
    if( CliInfo->GSrvConnect.find( GSrvID ) == CliInfo->GSrvConnect.end() )
    {
        Print( LV2 , "_PGxDataCliToGSrv" , "��Client ID=%d �P GSrv ID=%d�å������p�u?????" , CliID , GSrvID );
        return;
    }


	//  �έp�ʥ]��T
	{
		int iPacketID = *((int*)DataCliToGSrv->Data);

		if( iPacketID > 0 && iPacketID < DF_MAX_PACKETID )
		{
			m_CtoS_PacketID[ iPacketID ]++;
			m_CtoS_Bandwidth[ iPacketID ] += DataCliToGSrv->Size;
		}
	}


    //�����e
    DataCliToGSrv->GSrvID  = CliID;
    DataCliToGSrv->Command = EM_SysNet_Proxy_GSrv_DataCliToGSrv;
    Obj->SendToGSrv_GSrvID( GSrvID , Size , PG );
}
//-----------------------------------------------------------------------------------