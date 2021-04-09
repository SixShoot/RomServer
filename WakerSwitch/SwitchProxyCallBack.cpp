#include "SwitchClass.h"
#include <time.h>
//---------------------------------------------------------------------------------
void	SwitchClass::_OnProxyConnected       ( int NetID )
{
//    sockaddr_in pAddr;
//    _NetProxy->GetPeerAddress( NetID ,&pAddr );
    DWORD IP;
    WORD  Port;
    _NetBProxy->GetConnectInfo( NetID , &IP , &Port );

    ProxyInfoStruct* Info = _ProxyList.New( NetID );

    if( Info == NULL )
    {
        Print( LV5 , "_OnProxyConnected" , "(Proxy:%d)_NewProxy false!!" , NetID  );
        return;
    }

//    Info->IPnum    = pAddr.sin_addr.S_un.S_addr;
//    Info->IPnum    = IP;
    Info->Stat     = EM_NetStat_Login;

    SysNet_Switch_Proxy_ConnectNotify ConnectNotify;
    ConnectNotify.ProxyID = NetID;

    Print( LV2 , "_OnProxyConnected" , "Switch -> Proxy , notify Proxy(%d) to login Switch" , NetID );
    SendToProxy( NetID , sizeof(ConnectNotify) , &ConnectNotify );


    Schedular()->Push( _CheckProxyConnectAndReg , 15000 , this 
        , "ProxyID" , EM_ValueType_int , NetID , NULL );

}
//---------------------------------------------------------------------------------
void	SwitchClass::_OnProxyDisconnected    ( int NetID )
{
    Print( LV2 , "_OnProxyDisconnected" , "(Proxy:%d)" , NetID );

    ProxyInfoStruct*        ProxyInfo = _ProxyList[ NetID ];
    CliInfoStruct*          CliInfo;
    set< int >::iterator    ProxyIter;
    set< int >::iterator    CliIter;

    if(     ProxyInfo == NULL
        ||  ProxyInfo->Stat    != EM_NetStat_Connect )
    {
        Print( LV5 , "_OnProxyDisconnected" , "Proxy register not complete " );
        return;
    }

    //³qª¾©Ò¦³ GSrv »P ¦¹ProxyÂ_½u
    Print( LV2 , "_OnProxyDisconnected" , "notify all GSrv , to disconnect with Proxy" );
    SysNet_Switch_GSrv_ProxyDisconnectRequest   ProxyDisconnectRequest;
    ProxyDisconnectRequest.ProxyID = NetID;
    SendToAllGSrv( sizeof(ProxyDisconnectRequest) , &ProxyDisconnectRequest );

    if( _ProxyList.Del( NetID ) == false )
    {
        Print( LV5 , "_OnProxyDisconnected" , "(Proxy:%d)_OnProxyDisconnected : _DelProxyInfo false!!" , NetID  );
        return;
    }
    
    //³qª¾©Ò¦³Áp¨ì¦¹ Proxy ªº Client Â_½u
    //À°Proxy ±aµo
    SysNet_Proxy_GSrv_CliOnDisConnect CliDisconnect;

    for( ProxyIter = ProxyInfo->CliConnect.begin() ; ProxyIter != ProxyInfo->CliConnect.end() ; ProxyIter++ )
    {
        CliInfo = _CliList[ *ProxyIter ];
        if(     CliInfo == NULL
            ||  CliInfo->Stat    != EM_NetStat_Connect )
            continue;
        CliDisconnect.CliID = CliInfo->ID;

        for( CliIter = CliInfo->GSrvConnect.begin() ; CliIter != CliInfo->GSrvConnect.end() ; CliIter++ )
        {
            int GSrvID = *CliIter;
            Print( LV2 , "_OnProxyDisconnected" , "notify GSrvID=%d   CliID=%d to disconnect , Account=%s" , GSrvID , CliInfo->ID , CliInfo->Account.c_str() );
            SendToGSrv( GSrvID , sizeof( CliDisconnect ) , &CliDisconnect );          
        }

        //-------------------------------------------------------------     
		_CliList.Del( *ProxyIter );
        //¦^¦¬©Ò¦³ªºCliID
        _UnUsedClientIDList.push_back( CliInfo->ID );
        _LoginAccount.erase( CliInfo->Account );
		Print( LV2 , "_OnProxyDisconnected" , " notify AC that account logout. Proxy Disconnect , Account = %s " , CliInfo->Account.c_str() );
        _AcCli.PlayerLogout( (char*)CliInfo->Account.c_str() , CliInfo->ID );
        //-------------------------------------------------------------

    }
       
}
//---------------------------------------------------------------------------------
bool	SwitchClass::_OnProxyPacket          ( DWORD NetID , DWORD Size , void* Data )
{

//    int		Size;
//    void	*Data;

    //¨ú±o«Ê¥]¸ê®Æ
//    ObjNetGetBuffer( hIncomingData , Size , Data );
    PGSysNULL* PG = ( PGSysNULL *) Data;

    //------------------------------------------------------------------------------


    //ÀË¬d¬O§_¬°¨t²Î«Ê¥]
//    if( PG->SysIdentify == _Sys_Package_Number_ )
    {
        if(_PGInfo.size() <= (UINT32)PG->Command)
        {
            Print( LV2 , "_OnProxyPacket" , "(Proxy:%d) : packet cmd=%d out of range " , NetID , PG->Command );
            return false;
        }

        //Åv­­ÀË¬d
        PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );

/*        //«Ê¥]¤j¤pÀË¬d
        if( PGinfo->PGSize <= Size )
        {
            Print( LV2 , "_OnProxyPacket" , "(Proxy:%d) : packet Size=%d out of range , cmd=%d " , NetID , Size , PG->Command );
            return false;
        }*/

        //¨Ó·½ÀË¬d
        if( PGinfo->Type.Proxy == false )
        {
            Print( LV2 , "_OnProxyPacket" , "(Proxy:%d) : the source of packet error (PGinfo->Type.Proxy == false ??) cmd=%d " , NetID , PG->Command );
            return false;
        }

        //©I¥sCallback ³B²z
        _PGCallBack[PG->Command]( this , NetID , PG , Size );
    }

    return true;
}
//---------------------------------------------------------------------------------
void	SwitchClass::_OnProxyConnectFailed   ( int NetID )
{

}
//----------------------------------------------------------------------------------
//ÀË¬dproxy µù¥U¬O§_§¹¦¨
//¦pªG¨S§¹¦¨ §R°£¦¹³s½u
int     SwitchClass::_CheckProxyConnectAndReg( SchedularInfo*	Obj , void* InputClass )
{
    SwitchClass*    Class = (SwitchClass*)InputClass;
    int NetID = Obj->GetNumber( "ProxyID" );


    ProxyInfoStruct *Info = Class->_ProxyList[ NetID ];
    
    if(     Info == NULL
        ||  Info->Stat != EM_NetStat_Connect )
    {
        Class->_NetBProxy->Close( NetID );
    }
    return 0;
}
//-----------------------------------------------------------------------------------
void     SwitchClass::_PGxUnKnow( SwitchClass* Obj,int NetID,PGSysNULL* PG,int Size)
{
    Print( LV2 , "_PGxUnKnow" , "(Proxy:%d)_PGxUnKnow : Command = %d" , NetID , PG->Command );
}
//----------------------------------------------------------------------------------

void    SwitchClass::_PGxProxyConnect( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size )
{
    Print( LV2 , "_PGxProxyConnect" ,"(Proxy:%d)" , NetID );

    SysNet_Proxy_Switch_Connect*  Connect = (SysNet_Proxy_Switch_Connect*) PG;

    ProxyInfoStruct *ProxyInfo = Obj->_ProxyList[ NetID ];
    if(     ProxyInfo == NULL
        ||  ProxyInfo->Stat != EM_NetStat_Login )
    {
        Print( LV5 , "_PGxProxyConnect" , "_GetProxy( %d ) ProxyInfo == NULL or ProxyInfo->Stat != EM_NetStat_Login" , NetID );
        return;
    }
    ProxyInfo->Stat   = EM_NetStat_Connect;
    ProxyInfo->GSrvPort   = Connect->GSrvPort;
    ProxyInfo->ClientPort = Connect->CliPort;
	ProxyInfo->CliIPStr	  = Connect->CliIPStr;
	ProxyInfo->GSrvIPStr  = Connect->GSrvIPStr;
	//ProxyInfo->IPnum	=  ConvertIP( Connect->CliIPStr );



    //------------------------------------------------------------------------------------
    //³qª¾©Ò¦³ªºGSrvÁpµ²¨ì¦¹ Proxy
    Print( LV2 , "_PGxProxyConnect" , "notify all GSrv connect to new Proxy(%d)" , NetID );

    int GSrvCount = Obj->_GSrvList.size( );
    int i;

    

    GSrvInfoStruct*    GSrvInfo;
    SysNet_Switch_Proxy_GSrvConnect  GSrvConnectNotify;
    SysNet_Switch_GSrv_ProxyAddr     ProxyAddr;

    Obj->_GSrvList.GetData( true );
    for( i = 0 ; i < GSrvCount ; i++ )
    {
        GSrvInfo = Obj->_GSrvList.GetData();
        if(     GSrvInfo == NULL 
            ||  GSrvInfo->Stat == EM_NetStat_Login )
            continue;

        //°e¸ê®Æ³qª¾ Proxy Game Server­nµn¤J
        GSrvConnectNotify.GSrvID  = GSrvInfo->ID;
        Print( LV2 , "_PGxProxyConnect" , "Switch -> Proxy , notify Proxy that Game Server(%d) will be login" , GSrvInfo->ID );
        Obj->SendToProxy( ProxyInfo->ID , sizeof(GSrvConnectNotify) , &GSrvConnectNotify );
    }
}

//----------------------------------------------------------------------------------
//GSrv Ápµ²¨ì Proxy ¦¨¥
void    SwitchClass::_PGxGSrvConnectToProxy( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size )
{
    SysNet_Proxy_Switch_GSrvConnectOK*   GSrvConnectOK = (SysNet_Proxy_Switch_GSrvConnectOK*) PG;

    Print( LV2 , "_PGxGSrvConnectToProxy" , "(Proxy:%d)_PGxGSrvConnectToProxy GSrvID = %d" , NetID , GSrvConnectOK->GSrvID );

    
    ProxyInfoStruct* ProxyInfo = Obj->_ProxyList[ NetID ];
    
    if( ProxyInfo == NULL )
    {
        Print( LV5 , "_PGxGSrvConnectToProxy" ,"Proxy == NULL ??" );
        return ;
    }
    if( ProxyInfo->GSrvConnect.find( GSrvConnectOK->GSrvID ) != ProxyInfo->GSrvConnect.end() )
    {
        Print( LV5 , "_PGxGSrvConnectToProxy" ,"GSrv(%d) ,relogin??" , GSrvConnectOK->GSrvID );
        return;
    }

    Print( LV2 , "_PGxGSrvConnectToProxy" , "Proxy to register Switch GSrv(%d) success " , GSrvConnectOK->GSrvID );
    ProxyInfo->GSrvConnect.insert( GSrvConnectOK->GSrvID );

    //³]©w¦¹ GSrv ¬O¦óºØ¼Ò¦¡ªº GSrv
    GSrvInfoStruct* GSrvInfo = Obj->_GSrvList[ GSrvConnectOK->GSrvID ];
    if(     GSrvInfo == NULL  )
        //||  GSrvInfo->Stat != EM_NetStat_Login )
    {
        Print( LV5 , "_PGxGSrvConnectToProxy" , "_PGxGSrvConnectToProxy : Obj->_GetProxy(%d) GSrvInfo == NULL ,data error!!!!!" , GSrvConnectOK->GSrvID );
        return;
    }

    SysNet_Switch_Proxy_SetActive   SendGSrvType;
    SendGSrvType.GSrvID = GSrvInfo->ID;
    SendGSrvType.Type   = GSrvInfo->Type;

    Obj->SendToProxy( NetID , sizeof(SendGSrvType) , &SendGSrvType );

    
    if( GSrvInfo->Stat == EM_NetStat_Connect )
    {
        Print( LV2 , "_PGxGSrvConnectToProxy" , "aready" );
        return;
    }

    GSrvInfo->Stat = EM_NetStat_Connect;

}
//----------------------------------------------------------------------------------
//GSrv »P Proxy Â_½u
void    SwitchClass::_PGxProxy_GSrvProxyDisconnect( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size )
{ 
    SysNet_Proxy_Switch_GSrvDisconnect* Disconnect = (SysNet_Proxy_Switch_GSrvDisconnect*)PG;

    Print( LV2 , "_PGxGSrvProxyDisconnect" , "(Proxy:%d) GSrvID = %d" , NetID , Disconnect->GSrvID );

    ProxyInfoStruct* Info = Obj->_ProxyList[ NetID ];

    if( Info == NULL )
    {
        Print( LV5 , "_PGxGSrvProxyDisconnect" , "Info == NULL ??" );
        return ;
    }

    if( Info->GSrvConnect.find( Disconnect->GSrvID ) == Info->GSrvConnect.end() )
    {
        Print( LV5 , "_PGxGSrvProxyDisconnect" , "GSrv(%d) not login??" , Disconnect->GSrvID );
        return;
    }

    Print( LV2 , "_PGxGSrvProxyDisconnect" ,  "Proxy notify Switch to erase GSrv(%d) connecting info " , Disconnect->GSrvID );
    Info->GSrvConnect.erase( Disconnect->GSrvID );

}
//----------------------------------------------------------------------------------
//Cli Ápµ²¨ì Proxy ¦¨¥
void    SwitchClass::_PGxCliConnectToProxy( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size )
{

    SysNet_Proxy_Switch_CliLoginOK*   CliConnectOK = (SysNet_Proxy_Switch_CliLoginOK*) PG;

    Print( LV2 , "_PGxCliConnectToProxy" , "(Proxy:%d)_PGxCliConnectToProxy CliID = %d" , NetID , CliConnectOK->CliID );

    ProxyInfoStruct* ProxyInfo = Obj->_ProxyList[ NetID ];
    CliInfoStruct*   CliInfo = Obj->_CliList[ CliConnectOK->CliID ];
    if(     ProxyInfo == NULL  
        ||  ProxyInfo->Stat    != EM_NetStat_Connect 
        ||  CliInfo == NULL         )
    {
        Print( LV5 , "_PGxCliConnectToProxy" , "ProxyInfo == NULL or ProxyInfo->Stat != EM_NetStat_Connect || CliInfo == NULL??" );
        return ;
    }

    if( ProxyInfo->CliConnect.find( CliConnectOK->CliID ) != ProxyInfo->CliConnect.end() )
    {
		Print( LV5 , "_PGxCliConnectToProxy" , "Cli(%d) relogin??" , CliConnectOK->CliID );
        //printf("\nCli(%d) ­«´_µn¤J??" , CliConnectOK->CliID );
         Obj->_NetBCli->Close( CliInfo->SwitchNetID );
        return;
    }

    Print( LV2 , "_PGxCliConnectToProxy" ,  "Proxy to Switch register success,  Cli(%d) " , CliConnectOK->CliID );
    ProxyInfo->CliConnect.insert( CliConnectOK->CliID );

    CliInfo->Stat = EM_NetStat_Connect;

    Obj->_NetBCli->Close( CliInfo->SwitchNetID );

}
//----------------------------------------------------------------------------------
//Cli »P Proxy Â_½u
void    SwitchClass::_PGxProxy_CliProxyDisconnect( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size )
{
    //printf( "\n_PGxProxy_CliProxyDisconnect" );
    SysNet_Proxy_Switch_CliLogout* CliLogout = (SysNet_Proxy_Switch_CliLogout*)PG;
    CliInfoStruct*      CliInfo   = Obj->_CliList[ CliLogout->CliID ];
    ProxyInfoStruct* ProxyInfo    = Obj->_ProxyList[ NetID ];
    if(     CliInfo == NULL
        ||  CliInfo->Stat    != EM_NetStat_Connect ) 
    {
		if( CliInfo != NULL )
		{
			Print( LV5 , "_PGxProxy_CliProxyDisconnect" , "Accoutn = %s login not complete " , CliInfo->Account.c_str() );
			Obj->_LoginAccount.erase( CliInfo->Account );
		}
		else
		{
			Print( LV5 , "_PGxProxy_CliProxyDisconnect" , "login not complete CliInfo == NULL " );
		}
		
        return;
    }

	Print( LV2 , "_PGxProxy_CliProxyDisconnect" , "Account = %s" , CliInfo->Account.c_str() );

    if(     ProxyInfo == NULL
        ||  ProxyInfo->Stat    != EM_NetStat_Connect )
    {
        Print( LV5 , "_PGxProxy_CliProxyDisconnect" , " ProxyInfo == NULL or ProxyInfo->Stat != EM_NetStat_Connect ??" );
        return ;
    }

    if( ProxyInfo->CliConnect.erase( CliLogout->CliID ) == 0 )
    {
        Print( LV5 , "_PGxProxy_CliProxyDisconnect" ,  "ProxyInfo->CliConnect.erase( CliLogout->CliID ) == 0" , CliInfo->Account.c_str() );
        return ;
    }

    int GSrvID;
    //§â©Ò¦³»P¦¹Cli¦³ÃöªºGSrv ²M°£¬ÛÃö¸ê®Æ
    set<int>::iterator Iter;

    for( Iter = CliInfo->GSrvConnect.begin() ; Iter != CliInfo->GSrvConnect.end() ; Iter++ )
    {
        GSrvID = *Iter;
        GSrvInfoStruct *GSrvInfo = Obj->_GSrvList[ GSrvID ];
        if(     GSrvInfo == NULL
            ||  GSrvInfo->Stat    != EM_NetStat_Connect ) 
        {
            Print( LV2 , "_PGxProxy_CliProxyDisconnect" , "GSrvInfo == NULL ||  GSrvInfo->Stat    != EM_NetStat_Connect  " );
            return;
        }

        if( GSrvInfo->CliConnect.find( CliInfo->ID ) == GSrvInfo->CliConnect.end() )
        {
            Print( LV2 , "_PGxProxy_CliProxyDisconnect" , " GSrvInfo->CliConnect.find( CliInfo->ID ) == GSrvInfo->CliConnect.end()" );
            return;
        }

        GSrvInfo->CliConnect.erase( CliInfo->ID );
    }
    Obj->_CliList.Del( CliInfo->ID );
    Obj->_UnUsedClientIDList.push_back( CliInfo->ID );
    Obj->_LoginAccount.erase( CliInfo->Account );
	Print( LV2 , "_PGxProxy_CliProxyDisconnect" , "notify AC account logout . Client and Proxy disconnect , Account= %s " , CliInfo->Account.c_str() );
    Obj->_AcCli.PlayerLogout( (char*)CliInfo->Account.c_str() , CliInfo->ID );
 }
//-----------------------------------------------------------------------------------------------------------

void    SwitchClass::_SysNet_Proxy_Switch_NotifyCliIDLive( SwitchClass* Obj , int NetID , PGSysNULL* PG , int Size )
{    
    SysNet_Proxy_Switch_NotifyCliIDLive* CliLive = (SysNet_Proxy_Switch_NotifyCliIDLive*)PG;
    CliInfoStruct*      CliInfo   = Obj->_CliList[ CliLive->CliID ];
    ProxyInfoStruct*    ProxyInfo = Obj->_ProxyList[ NetID ];

    if(     CliInfo == NULL
        ||(		CliInfo->Stat    != EM_NetStat_Connect
			&&  CliInfo->Stat    != EM_NetStat_Relogin	) ) 
    {
		if( CliInfo == NULL )
			Print( LV5 , "_SysNet_Proxy_Switch_NotifyCliIDLive" , "_SysNet_Proxy_Switch_NotifyCliIDLive : Cli = %d , CliInfo == NULL" , CliLive->CliID );
		else
			Print( LV5 , "_SysNet_Proxy_Switch_NotifyCliIDLive" , "_SysNet_Proxy_Switch_NotifyCliIDLive : Cli = %d , CliInfo->Stat != EM_NetStat_Connect" , CliLive->CliID );

		//³qª¾Proxy §â¦¹ Client µn¥X

		if( CliInfo != NULL )
		{
			SysNet_Proxy_CliLogoutRequest Send;
			Send.CliID = CliLive->CliID;
			sprintf(Send.Reason, "_SysNet_Proxy_Switch_NotifyCliIDLive : Cli = %d , CliInfo->Stat != EM_NetStat_Connect" , CliLive->CliID );
			Obj->_NetBProxy->Send( CliInfo->ProxyID , sizeof( Send ) , &Send );
		}
        return;
    }

    if(     ProxyInfo == NULL
        ||  ProxyInfo->Stat    != EM_NetStat_Connect )
    {
        Print( LV5 , "_SysNet_Proxy_Switch_NotifyCliIDLive" , "ProxyInfo == NULL ||  ProxyInfo->Stat != EM_NetStat_Connect??" );

		int    CliNetID = Obj->_CliNetToIDMap.GetNetID( CliLive->CliID );
		if( CliNetID != -1 )
		{
			Obj->_NetBCli->Close( CliNetID );
		}
	    return ;
	}
    CliInfo->LastCheckTime = clock();
}
//-----------------------------------------------------------------------------------------------------------
void	SwitchClass::_SysNet_Proxy_Switch_CliConnectPrepareOK( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size )
{
	SysNet_Proxy_Switch_CliConnectPrepareOK* PrepareOK = (SysNet_Proxy_Switch_CliConnectPrepareOK*)PG;

	SysNet_Switch_Cli_ConnectPorxyNotify	Send;
	//³qª¾Client

	//////////////////////////////////////////////////////////////////////////
	//¨ú±oClient IPªº©Ò¦b°ê®a
	int CliID = Obj->_CliNetToIDMap.GetID( PrepareOK->CliInfo.SwitchNetID );
	CliInfoStruct*      CliInfo   = Obj->_CliList[ CliID ];
	if( CliInfo == NULL )
	{
		Print( LV5 , "_SysNet_Proxy_Switch_CliConnectPrepareOK" , "CliInfo == NULL" );
		return;
	}
	ApnicCountryInfoStruct& Info = Obj->_ApnicFileReader.FindIP( CliInfo->IPnum );
	map< string , IPStruct >::iterator Iter = Obj->_RedirectHost.find( Info.Country );
	
	string  ProxyIPStr = PrepareOK->CliInfo.ProxyIPStr;
	int		ProxyPort = PrepareOK->CliInfo.ProxyPort;
	if( Iter != Obj->_RedirectHost.end() )
	{
		ProxyIPStr = Iter->second.IPStr;
		ProxyPort = Iter->second.Prot;
	}

	//////////////////////////////////////////////////////////////////////////

	
	strcpy( Send.LoginInfo , "NULL" );
	Send.ProxyID = PrepareOK->CliInfo.ProxyID;
	//strcpy( Send.ProxyIPStr , PrepareOK->CliInfo.ProxyIPStr );
//	Send.ProxyPort  = PrepareOK->CliInfo.ProxyPort;
	strcpy( Send.ProxyIPStr , ProxyIPStr.c_str() );
	Send.ProxyPort  = ProxyPort;
	Send.PlayTimeQuota = PrepareOK->CliInfo.PlayTimeQuota;

	MyStrcpy( Send.LastLoginIP	, PrepareOK->CliInfo.LastLoginIP , sizeof(Send.LastLoginIP) );
	MyStrcpy( Send.LastLoginTime , PrepareOK->CliInfo.LastLoginTime , sizeof(Send.LastLoginTime) );

	Obj->SendToCli( PrepareOK->CliInfo.SwitchNetID , sizeof(Send) , &Send );
}
//-----------------------------------------------------------------------------------------------------------
void	SwitchClass::_SysNet_Proxy_Switch_GSrvConnectPrepareOK( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size )
{
	
	SysNet_Proxy_Switch_GSrvConnectPrepareOK* PrepareOK = (SysNet_Proxy_Switch_GSrvConnectPrepareOK*)PG;

	Print( LV2 , "_SysNet_Proxy_Switch_GSrvConnectPrepareOK" , "Proxy NetID=%d    GSrvID=%d" , NetID , PrepareOK->GSrvID );

	SysNet_Switch_GSrv_ProxyAddr     ProxyAddr;
	ProxyInfoStruct*    Proxy = Obj->_ProxyList[ NetID ];

	if(     Proxy == NULL
		||  Proxy->Stat != EM_NetStat_Connect )
	{
		Print( LV5 , "_SysNet_Proxy_Switch_GSrvConnectPrepareOK" , "Proxy == NULL ||  Proxy->Stat != EM_NetStat_Connect" );
		return;
	}

	//ProxyAddr.IPNum     = Proxy->IPnum;
	MyStrcpy( ProxyAddr.GSrvIPStr , Proxy->GSrvIPStr.c_str() , sizeof(ProxyAddr.GSrvIPStr) );
	ProxyAddr.Port      = Proxy->GSrvPort;
	ProxyAddr.ProxyID   = Proxy->ID;

	Obj->SendToGSrv( PrepareOK->GSrvID , sizeof( ProxyAddr ) , &ProxyAddr );    
}
//---------------------------------------------------------------------------------