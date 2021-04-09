#include "SwitchClass.h"

//---------------------------------------------------------------------------------
void	SwitchClass::_OnCliConnected       ( int NetID )
{
    int     i;
    CliInfoStruct*      CliInfo = _NewCli( );
	CliInfo->WaitLoginCount = 120;

    if( CliInfo == NULL )
    {
        //"CliInfo == NULL �n�DClient ��Ƶ��c���� ???"
		Print( LV2 , "_OnCliConnected" , "CliInfo == NULL malloc failed ???" );
        _NetBCli->Close( NetID );
        return;
    }

    if( _ProxyList.size()  == 0 )
    {
        Print( LV2 , "_OnCliConnected" , "no Proxy exist (none of Proxy open )???" );
        _CliList.Del( CliInfo->ID );

		SysNet_Switch_Cli_ConnectPorxyNotify	Send;
		Send.Result = EM_NetLogFailed_ProxyNotExist;
		SendToCli( NetID , sizeof(Send) , &Send );
		
		// �]�w�|����ˬd
		Schedular()->Push( _CheckCliConnectAndReg , 4000 , this , "CliID" , EM_ValueType_int , CliInfo->ID , NULL );
		//_NetBCli->Close( NetID );
        return;
    }

    int MinLoadingProxyID = 0;
    int MinLoadingProxyCliCount = 100000;
    //�j�M�@��Proxy�B�zClient�ݸ�T
    int ProxyCount = _ProxyList.size();
    _ProxyList.GetData( true );
    for( i = 0 ; i <ProxyCount ; i++ )
    {
        ProxyInfoStruct *ProxyInfo = _ProxyList.GetData();

        if(     ProxyInfo == NULL 
            ||  ProxyInfo->Stat != EM_NetStat_Connect )
            continue;

        if( (int)ProxyInfo->CliConnect.size() < MinLoadingProxyCliCount )
        {
            MinLoadingProxyID = ProxyInfo->ID;
            MinLoadingProxyCliCount = (int)ProxyInfo->CliConnect.size();
        }
    }

    CliInfo->ProxyID = MinLoadingProxyID;

//    sockaddr_in pAddr;
//    _NetCli->GetPeerAddress( NetID ,&pAddr );
    DWORD   IP;
    WORD    Port;
    _NetBCli->GetConnectInfo( NetID , &IP , &Port );

    SysNet_Switch_Cli_LoginInfoRequest  CliLoginRequest;
    CliLoginRequest.CliID = CliInfo->ID;

    CliInfo->Stat  = EM_NetStat_Login;
    //CliInfo->IPnum = pAddr.sin_addr.S_un.S_addr;
    CliInfo->IPnum = IP;
    CliInfo->SwitchNetID = NetID;

    if( _CliNetToIDMap.Set( NetID , CliInfo->ID ) == false )
	{
		//ID�����D�Ȯɤ��^��CliInfo->ID
		 Print( LV2 , "_OnCliConnected" , "_CliNetToIDMap.Set( NetID , CliInfo->ID ) == false CliInfo->ID=%d NetID=%d LastErrID=%d" , CliInfo->ID , NetID , _CliNetToIDMap.LastErrID() );
		 _NetBCli->Close( NetID );
		 return;
	};

    SendToCli( NetID , sizeof(CliLoginRequest) , &CliLoginRequest );

#ifndef  _DEBUG
	Schedular()->Push( _CheckCliConnectAndReg , 20000 , this 
		, "CliID" , EM_ValueType_int , CliInfo->ID , NULL );
#endif
}
//---------------------------------------------------------------------------------
void	SwitchClass::_OnCliDisconnected    ( int NetID )
{
    
    CliInfoStruct*      CliInfo   = _CliList[ _CliNetToIDMap.GetID( NetID ) ];

    _CliNetToIDMap.Cls( NetID , -1 );

	if( CliInfo == NULL )
	{
		Print( LV2 , "_OnCliDisconnected" , "CliInfo == NULL error" );
		return;
	}

	//�_�u�Ƶ{�٦b�B�z
	if( CliInfo->TempScID != -1 )	
	{
		Schedular()->Erase( CliInfo->TempScID );
		CliInfo->TempScID = -1;
	}

    if(		CliInfo->Stat != EM_NetStat_Connect 
        &&  CliInfo->Stat != EM_NetStat_Relogin )
    {
		int IPNum;
		char IPStr[256];
		_NetBCli->GetConnectInfo( NetID , (DWORD*)&IPNum , NULL );
		ConvertIPtoString( IPNum , IPStr );

		Print( LV2 , "_OnCliDisconnected" , "account logout , account register uncomplete ,Account = %s IP=%s" , CliInfo->Account.c_str() , IPStr );

        if(		CliInfo->Stat == EM_NetStat_ACLoginOK  )
        {
            //Print( LV2 , "_OnCliDisconnected" , "���U�nAccount Agent ���S�������������U Account = %s" , CliInfo->Account.c_str() );
			Print( LV2 , "_OnCliDisconnected" , "AC register OK , but something register uncomplete. Account=%s , Stat =%d" , CliInfo->Account.c_str() , CliInfo->Stat );
            _AcCli.PlayerLogout( (char*)CliInfo->Account.c_str() , CliInfo->ID );
        }
		else
		{
			int i= 0;
		}

//		 _AcCli.PlayerLogout( (char*)CliInfo->Account.c_str() , CliInfo->ID );
		_CliList.Del( CliInfo->ID );
        _UnUsedClientIDList.push_back( CliInfo->ID );
        _LoginAccount.erase( CliInfo->Account );
    }
	else
	{
		Print( LV2 , "_OnCliDisconnected" , "register OK , connect to Proxy   Account = %s " , CliInfo->Account.c_str() );
	}
}
//---------------------------------------------------------------------------------
bool	SwitchClass::_OnCliPacket          ( DWORD NetID , DWORD Size , void* Data )
{
//    int		Size;
//    void	*Data;

    //���o�ʥ]���
//    ObjNetGetBuffer( hIncomingData , Size , Data );
    PGSysNULL* PG = ( PGSysNULL *) Data;

    //------------------------------------------------------------------------------

    //�ˬd�O�_���t�Ϋʥ]
 //   if( PG->SysIdentify == _Sys_Package_Number_ )
    {
        if(_PGInfo.size() <= (UINT32)PG->Command)
        {
            Print( LV2 , "_OnCliPacket" ,"(Cli:%d)_OnCliPacket : packet cmd=%d out of range" , NetID , PG->Command );
            return false;
        }

        //�v���ˬd
        PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );

        //�ʥ]�j�p�ˬd
        if( PGinfo->MinPGSize > Size || PGinfo->MaxPGSize < Size )
        {
            Print( LV2 , "_OnCliPacket" , "(Cli:%d)_OnCliPacket : packet Size=%d out of range , cmd=%d " , NetID , Size , PG->Command );
            return false;
        }

        //�ӷ��ˬd
        if( PGinfo->Type.Client == false )
        {
            Print( LV2 , "_OnCliPacket" , "(Cli:%d)_OnCliPacket : the source of packet error (PGinfo->Type.Client == false ??) cmd=%d " , NetID , PG->Command );
            return false;
        }

        //�I�sCallback �B�z
        _PGCallBack[PG->Command]( this , NetID , PG , Size );
    }

    return true;
}
//---------------------------------------------------------------------------------
void	SwitchClass::_OnCliConnectFailed   ( int NetID )
{

}
//-----------------------------------------------------------------------------------------------------------
//Client �n�J��T
void    SwitchClass::_PGxClientLoginInfo( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size )
{
    DWORD   IP;
    WORD    Port;

	Obj->_NetBCli->GetConnectInfo( NetID ,&IP , &Port );

    SysNet_Cli_Switch_LoginInfo* LoginInfo = (SysNet_Cli_Switch_LoginInfo*)PG;
    CliInfoStruct*      CliInfo   = Obj->_CliList[ LoginInfo->CliID ];

	if( CliInfo == NULL )
	{
		Print( LV5 , "_PGxClientLoginInfo" , "warning Client CliInfo == NULL , _NetBCli->Close( NetID=%d ) IP=%d" , NetID , IP );
		Obj->_NetBCli->Close( NetID );
		return;
	}
	if( CliInfo->Stat != EM_NetStat_Login )
	{
		Print( LV5 , "_PGxClientLoginInfo" , "(*Client error*) re-send client Login Info!! account=%s" , CliInfo->Account.c_str() );
		Obj->_NetBCli->Close( NetID );
		return;
	}


    if( CliInfo->IPnum != (unsigned int )IP )
    {
		//���p���� IP ���P 
		Print( LV5 , "_PGxClientLoginInfo" , "error , Client IP != CliInfo->IPnum ???" );
		Print( LV5 , "_PGxClientLoginInfo" , "error , _NetBCli->Close , Client IPNum = %x ???" , IP );
        Obj->_NetBCli->Close( NetID );
        return;
    }

	_strlwr( LoginInfo->Account );

	int Strlen = strlen( LoginInfo->Account );
	for( int i = 0 ; i < Strlen ; i++ )
	{
		if( LoginInfo->Account[i] == ' ' )
		{
			LoginInfo->Account[i] = 0;
			break;
		}
	}

	LoginInfo->Account[ _Max_CliAccount_Name_Size_ - 1 ] = 0;
	LoginInfo->Password[ _Max_CliPassword_Name_Size_ - 1 ] = 0;
	LoginInfo->MacAddress[ _Max_CliMac_Address_Size_ - 1 ] = 0;

	CliInfo->Account		= LoginInfo->Account;
	CliInfo->Password		= LoginInfo->Password; 
	CliInfo->MacAddress		= LoginInfo->MacAddress;
	CliInfo->HashedPassword	= LoginInfo->HashedPassword;

    if( Obj->_LoginAccount.find( LoginInfo->Account) != Obj->_LoginAccount.end() )
    {
		CliInfoStruct* LoginPlayer = Obj->_LoginAccount[ LoginInfo->Account ];

		Print( LV5 , "_PGxClientLoginInfo" , "account = %s exist , waitting previously account logout " , LoginInfo->Account );
		CliInfo->WaitLoginCount = 120;
		Obj->Schedular()->Push( _WaitLogin , 3000 , Obj , "CliID" , EM_ValueType_int , LoginInfo->CliID  , NULL );

		SysNet_Switch_Proxy_NotifyCliIDRelogin Send;
		Send.CliID = LoginPlayer->ID;
		Send.IPNum = LoginPlayer->IPnum;
		Obj->SendToProxy( LoginPlayer->ProxyID , sizeof(Send) , &Send );
        return;
    }


	CliInfo->Stat		= EM_NetStat_Login_CheckAccount;
	Obj->_LoginAccount[ LoginInfo->Account ] = CliInfo;

	//**********************************************************************************
	if( SwitchClass::m_NeedPassword )
		Obj->_AcCli.PlayerReg( LoginInfo->Account , LoginInfo->Password , CliInfo->IPnum , CliInfo->ID, CliInfo->MacAddress.c_str(), CliInfo->HashedPassword );
	else
		Obj->_OnACPlayerReg( LoginInfo->CliID , LoginInfo->Account , EM_ChargType_Trial , 0 , "" , "" , 0 );


	Print( LV5 , "_PGxClientLoginInfo" , "message , AC register request , Account = %s CliInfo->IPnum =%d" , LoginInfo->Account , CliInfo->IPnum );

}
//---------------------------------------------------------------------------------
//�ˬdClient ���U�O�_����
//�p�G�S���� �R�����s�u
int 	SwitchClass::_CheckCliConnectAndReg( SchedularInfo*	Obj , void* InputClass )
{
    SwitchClass*    Class = (SwitchClass*)InputClass;
    int CliID = Obj->GetNumber( "CliID" );
	int NetID = Class->_CliNetToIDMap.GetNetID( CliID );

    CliInfoStruct *Info = Class->_CliList[ CliID ];

    if(     Info == NULL
        ||  Info->Stat != EM_NetStat_Connect )
    {
		if( Info != NULL && Info->WaitLoginCount > 0 )
		{
			Class->Schedular()->Push( _CheckCliConnectAndReg , 4000 , Class , "CliID" , EM_ValueType_int , NetID , NULL );
			return 0;
		}
        Class->_NetBCli->Close( NetID );
    }

    return 0;
}
//-----------------------------------------------------------------------------------------------------------
//���ݥL�H�n�X �M��n�J
int 	SwitchClass::_WaitLogin( SchedularInfo*	Obj , void* InputClass )
{
	SwitchClass*    Class = (SwitchClass*)InputClass;
	int CliID = Obj->GetNumber( "CliID" );
	CliInfoStruct*      CliInfo   = Class->_CliList[ CliID ];	
	if( CliInfo == NULL )
		return 0;

	Print( LV2 , "_WaitLogin" , " Account =%s CliInfo->IPnum=%d" , CliInfo->Account.c_str() , CliInfo->IPnum );
	if( Class->_LoginAccount.find( CliInfo->Account) != Class->_LoginAccount.end() )
	{
		CliInfo->WaitLoginCount--;
		if( CliInfo->WaitLoginCount <= 0 )
		{
			Class->_NetBCli->Close( CliInfo->SwitchNetID );			
			return 0;
		}
		Class->Schedular()->Push( _WaitLogin , 3000 , Class , "CliID" , EM_ValueType_int , CliID  , NULL );
		return 0;
	}

	CliInfo->Stat		= EM_NetStat_Login_CheckAccount;
	Class->_LoginAccount[ CliInfo->Account ] = CliInfo;

	//**********************************************************************************
//	Class->_AcCli.PlayerReg( (char*)CliInfo->Account.c_str() , (char*)CliInfo->Password.c_str() , CliInfo->IPnum , CliInfo->ID, CliInfo->MacAddress.c_str() );
	if( SwitchClass::m_NeedPassword )
		Class->_AcCli.PlayerReg( (char*)CliInfo->Account.c_str() , (char*)CliInfo->Password.c_str() , CliInfo->IPnum , CliInfo->ID, CliInfo->MacAddress.c_str(), CliInfo->HashedPassword );
	else
		Class->_OnACPlayerReg(  CliInfo->ID , (char*)CliInfo->Account.c_str() , EM_ChargType_Trial , 0 , "" , "" , 0 );

	Print( LV5 , "_WaitLogin" , "message , AC register request ,  Account = %s CliInfo->IPnum =%d" , CliInfo->Account.c_str() , CliInfo->IPnum );

	return 0;
}