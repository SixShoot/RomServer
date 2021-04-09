#include "SwitchClass.h"
#include "RemotetableInput/RemotableInput.h"

void    SwitchClass::_OnACLogin( )
{
    Print( LV2 , "_OnACLogin" , "login Account Agent success" );
    //註冊所有已登入的帳號
    CliInfoStruct* CliInfo;
    _CliList.GetData( true );
    while( 1 )
    {
        CliInfo = _CliList.GetData( );
        if( CliInfo == NULL )
            break;
//        if(     CliInfo->Stat == EM_NetStat_Connect 
//            ||  CliInfo->Stat == EM_NetStat_ACLoginOK )
        {
			CliInfo->Stat = EM_NetStat_Relogin;
            _AcCli.PlayerReg( (char*)CliInfo->Account.c_str() , (char*)CliInfo->Password.c_str() , CliInfo->IPnum , CliInfo->ID, CliInfo->MacAddress.c_str(), CliInfo->HashedPassword );
        }        
    }

//#ifndef NOTCONSOLEWIN
	char Buf[256];
	char OrgTitleStr[256];
	GetConsoleTitle( OrgTitleStr ,sizeof( OrgTitleStr ) );
	sprintf(Buf,"[OK]%s", OrgTitleStr );
	SetConsoleTitle(Buf);
//#else
	g_remotableInput.OutputStartupMessage();
	
	//SetSwitchStatus( true );

	/*
	if( GetUpdateListMode() == true )
	{
		//printf( "Set Custom State {79CC1671-A46D-478a-9497-4933581B5B03},-1,Enable\n" );
		PrintToController(false,  "Set Custom State {79CC1671-A46D-478a-9497-4933581B5B03},-1,Enable\n" );
	}
	else
	{
		//printf( "Set Custom State {79CC1671-A46D-478a-9497-4933581B5B03},0,Disable\n" );
		PrintToController(false,  "Set Custom State {79CC1671-A46D-478a-9497-4933581B5B03},0,Disable\n" );
	}
	*/
	
//#endif
}
void    SwitchClass::_OnACLoginFailed( GSrvLoginResultEnum Result )
{
    Print( LV2 , "_OnACLoginFailed" , "login Account Agent failed" );
    switch( Result )
    {
    case EM_GSrvLoginResult_RegTwice:
		Print( LV2 , "_OnACLoginFailed" , "login Account Agent failed , relogin" );
        //printf( "\n 重復登入" );
        break;
    case EM_GSrvLoginResult_SrvNotReg:
        //printf( "\n 未註冊" );
		Print( LV2 , "_OnACLoginFailed" , "login Account Agent failed , unregist" );		
        break;
	default:
		Print( LV2 , "_OnACLoginFailed" , "login Account Agent failed , unknow" );
		break;
    }
}
void    SwitchClass::_OnACLogout( )
{
    Print( LV2 , "_OnACLogout" , "login Account Agent " );
}
void    SwitchClass::_OnACPlayerReg(  int UserID , char* Account , ChargTypeEnum Type , int Point , char* LastLoginIP , char* LastLoginTime , int PlayTimeQuota )
{
	Print( LV2 , "_OnACPlayerReg" , "AC->Switch , Player register result OK , UserID=%d Account=%s" , UserID , Account );
    CliInfoStruct*      CliInfo   = _CliList[ UserID ]; 
	SysNet_Switch_Cli_ConnectPorxyNotify	Send;

    if(		CliInfo == NULL 
		||	( CliInfo->Stat != EM_NetStat_Login_CheckAccount &&  CliInfo->Stat != EM_NetStat_Relogin  ) )
    {
		Print( LV5 , "_OnACPlayerReg" , "UserID data error " );
        _AcCli.PlayerLogout( (char*)Account , UserID );		
        return;
    }

	if( CliInfo->Stat == EM_NetStat_Relogin )
	{
		Print( LV2 , "_OnACPlayerReg" , "relogin AC success " );
		return;
	}

	if( stricmp( Account , CliInfo->Account.c_str() ) != 0 )
	{
		Print( LV5 , "_OnACPlayerReg" , " Account != CliInfo->Account.c_str() , Account data error , account=%s , orgAccount=%s" , Account , CliInfo->Account.c_str() );
        //TRACE0( "\n _OnACPlayerReg 回傳Account 與 原本的不符 資料有問題" );
        _AcCli.PlayerLogout( (char*)Account , UserID );		
        return;

	}


    ProxyInfoStruct*    ProxyInfo = _ProxyList[ CliInfo->ProxyID ];
    if(     ProxyInfo == NULL 
        ||  ProxyInfo->Stat != EM_NetStat_Connect )
    {
		Send.Result = EM_NetLogFailed_ConnectFail;
		Print( LV5 , "_OnACPlayerReg" , "ProxyInfo data error!!!!" );
        _AcCli.PlayerLogout( (char*)CliInfo->Account.c_str() , CliInfo->ID );
		SendToCli( CliInfo->SwitchNetID , sizeof(Send) , &Send );
        return;
    }

    if( CliInfo->Stat  == EM_NetStat_Connect )
    {
		// 重新註冊成
        Print( LV2 , "_OnACPlayerReg" , "re-register success" );
        return;
    }

	
    CliInfo->Stat =  EM_NetStat_ACLoginOK;

    //**********************************************************************************
    SysNet_Switch_Proxy_CliConnect  PG_NotifyProxyCliConnect;

	PG_NotifyProxyCliConnect.CliInfo.SwitchNetID =  CliInfo->SwitchNetID;
	PG_NotifyProxyCliConnect.CliInfo.ProxyID = CliInfo->ProxyID;
	strcpy( PG_NotifyProxyCliConnect.CliInfo.ProxyIPStr , ProxyInfo->CliIPStr.c_str() );
	PG_NotifyProxyCliConnect.CliInfo.ProxyPort = ProxyInfo->ClientPort;
	PG_NotifyProxyCliConnect.CliInfo.PlayTimeQuota = PlayTimeQuota;	

	MyStrcpy( PG_NotifyProxyCliConnect.CliInfo.LastLoginIP	, LastLoginIP , sizeof(PG_NotifyProxyCliConnect.CliInfo.LastLoginIP) );
	MyStrcpy( PG_NotifyProxyCliConnect.CliInfo.LastLoginTime , LastLoginTime , sizeof(PG_NotifyProxyCliConnect.CliInfo.LastLoginTime) );

    PG_NotifyProxyCliConnect.CliID = CliInfo->ID;
    //memcpy( PG_NotifyProxyCliConnect.Account , (char*)CliInfo->Account.c_str() , sizeof(PG_NotifyProxyCliConnect.Account) );
	MyStrcpy( PG_NotifyProxyCliConnect.Account , (char*)CliInfo->Account.c_str() , sizeof(PG_NotifyProxyCliConnect.Account) );

    SendToProxy( CliInfo->ProxyID , sizeof(PG_NotifyProxyCliConnect) , &PG_NotifyProxyCliConnect );

}
void    SwitchClass::_OnACPlayerRegFailed(  int UserID , char* Account , LoginResultEnum Result )
{
    Print( LV2 , "_OnACPlayerRegFailed" , "Account Agent register failed , Result=%d " , Result );

	SysNet_Switch_Cli_ConnectPorxyNotify	Send;
	switch( Result )
	{
	case EM_LoginResul_PasswordError:
		Send.Result = EM_NetLogFailed_PasswordError;
		break;
	case EM_LoginResul_NotFind:
		Send.Result = EM_NetLogFailed_NotFindError;
		break;
	case EM_LoginResul_LoginTwice:
		Send.Result = EM_NetLogFailed_LoginTwice;
		break;
	case EM_LoginResul_CommLock:
		Send.Result = EM_NetLogFailed_CommLock;
		break;
	case EM_LoginResul_LoginByOtherSrv:		//Game ID != -1
		Send.Result = EM_NetLogFailed_LoginByOtherSrv;
		break;
	case EM_LoginResul_SrvDisconnect:		//(目前沒用)
		Send.Result = EM_NetLogFailed_SrvDisconnect;
		break;
	case EM_LoginResul_NotEnoughPoint:		//(目前沒用)
		Send.Result = EM_NetLogFailed_NotEnoughPoint;
		break;
	case EM_LoginResul_NotCorrectChargType:	//不正確的付費狀態(目前沒用)
		Send.Result = EM_NetLogFailed_NotCorrectChargType;
		break;
	case EM_LoginResul_Suspended:			//警告
		Send.Result = EM_NetLogFailed_Suspended;
		break;
	case EM_LoginResul_Banned:				//被踢
		Send.Result = EM_NetLogFailed_Banned;
		break;
	case EM_LoginResul_AccountStrError:		//帳號字串錯誤
		Send.Result = EM_NetLogFailed_AccountStrError;
		break;
	case EM_LoginResul_SwitchError:			//Switch 註冊有問題
		Send.Result = EM_NetLogFailed_SwitchError;
		_AcCli.Logout();
		break;
	case EM_LoginResul_FreezeType_Self:
		Send.Result = EM_NetLogFailed_FreezeType_Self;
		break;
	case EM_LoginResul_FreezeType_Official:	
		Send.Result = EM_NetLogFailed_FreezeType_Official;
		break;
	case EM_LoginResul_llegalMacAddress:
		Send.Result = EM_NetLogFailed_llegalMacAddress;
		break;
	case EM_LoginResul_FreezeType_Other3:
	case EM_LoginResul_FreezeType_Other4:
	case EM_LoginResul_FreezeType_Other5:
	case EM_LoginResul_FreezeType_Other6:
	case EM_LoginResul_FreezeType_Other7:
	case EM_LoginResul_FreezeType_Other8:
	case EM_LoginResul_FreezeType_Other9:
		Send.Result = EM_NetLogFailed_FreezeType1_Other;
		break;

	default:
		Send.Result = EM_NetLogFailed_UnknowError;
		break;
	}

	CliInfoStruct*      CliInfo   = _CliList[ UserID ]; 

	if( CliInfo == NULL  )
	{
		//TRACE0( "\n _OnACPlayerRegFailed 回傳的 UserID 資料有問題" );
		Print( LV5 , "_OnACPlayerRegFailed" , "UserID data error" );
		return;
	}

	SendToCli( CliInfo->SwitchNetID , sizeof(Send) , &Send );

	if( CliInfo->TempScID != -1 )
	{
		//TRACE0( "\n _OnACPlayerRegFailed  CliInfo->TempScID != -1 資料有問題" );
		Print( LV5 , "_OnACPlayerRegFailed" , " CliInfo->TempScID != -1 data error" );
		return;
	}

    CliInfo->TempScID = SetCliLogout_2Sec( UserID );    
}

void	SwitchClass::_OnACPlayerRegFailedData(  int UserID , char* Account , LoginResultEnum Result, void* Data, unsigned long DataSize )
{
	SysNet_Switch_Cli_LoginFailedData	Send;
	PlayerLoginFailedDataUnion* pData = (PlayerLoginFailedDataUnion*)Data;

	switch( Result )
	{
	case EM_LoginResul_PasswordError:
		{
			Send.Result							= EM_NetLogFailed_PasswordError;
			Send.Data.PasswordError.Count		= pData->PasswordError.Count;
			Send.Data.PasswordError.Max			= pData->PasswordError.Max;
			Send.Data.PasswordError.FreezeTime	= pData->PasswordError.FreezeTime;
		}		
		break;
	case EM_LoginResul_NotFind:
		Send.Result = EM_NetLogFailed_NotFindError;
		break;
	case EM_LoginResul_LoginTwice:
		Send.Result = EM_NetLogFailed_LoginTwice;
		break;
	case EM_LoginResul_LoginByOtherSrv:		//Game ID != -1
		Send.Result = EM_NetLogFailed_LoginByOtherSrv;
		break;
	case EM_LoginResul_SrvDisconnect:		//(目前沒用)
		Send.Result = EM_NetLogFailed_SrvDisconnect;
		break;
	case EM_LoginResul_NotEnoughPoint:		//(目前沒用)
		Send.Result = EM_NetLogFailed_NotEnoughPoint;
		break;
	case EM_LoginResul_NotCorrectChargType:	//不正確的付費狀態(目前沒用)
		Send.Result = EM_NetLogFailed_NotCorrectChargType;
		break;
	case EM_LoginResul_Suspended:			//警告
		Send.Result = EM_NetLogFailed_Suspended;
		break;
	case EM_LoginResul_Banned:				//被踢
		Send.Result = EM_NetLogFailed_Banned;
		break;
	case EM_LoginResul_AccountStrError:		//帳號字串錯誤
		Send.Result = EM_NetLogFailed_AccountStrError;
		break;
	case EM_LoginResul_SwitchError:			//Switch 註冊有問題
		Send.Result = EM_NetLogFailed_SwitchError;
		_AcCli.Logout();
		break;
	case EM_LoginResul_FreezeType_Self:
		Send.Result = EM_NetLogFailed_FreezeType_Self;
		break;
	case EM_LoginResul_FreezeType_Official:	
		{
			Send.Result = EM_NetLogFailed_FreezeType_Official;
			Send.Data.Freeze.RemainingTime = pData->Freeze.RemainingTime;
		}
		break;
	case EM_LoginResul_llegalMacAddress:
		{
			Send.Result = EM_NetLogFailed_llegalMacAddress;
			Send.Data.Freeze.RemainingTime = pData->Freeze.RemainingTime;
		}		
		break;
	case EM_LoginResul_FreezeType_Other3:
	case EM_LoginResul_FreezeType_Other4:
	case EM_LoginResul_FreezeType_Other5:
	case EM_LoginResul_FreezeType_Other6:
	case EM_LoginResul_FreezeType_Other7:
	case EM_LoginResul_FreezeType_Other8:
	case EM_LoginResul_FreezeType_Other9:
		{
			Send.Result = EM_NetLogFailed_FreezeType1_Other;
			Send.Data.Freeze.RemainingTime = pData->Freeze.RemainingTime;
		}		
		break;

	default:
		Send.Result = EM_NetLogFailed_UnknowError;
		break;
	}

	CliInfoStruct*      CliInfo   = _CliList[ UserID ]; 

	if( CliInfo == NULL  )
	{
		Print( LV5 , "_OnACPlayerRegFailed" , "UserID data error" );
		return;
	}

	SendToCli( CliInfo->SwitchNetID , sizeof(Send) , &Send );
}

void    SwitchClass::_OnACPlayerLogout( int UserID , char* Account )
{
    Print( LV2 , "_OnACPlayerLogout" , "player logout AC" );
}
void    SwitchClass::_OnACPlayerLogoutFaild( int UserID , char* Account , LogoutResultEnum Result )
{
    Print( LV2 , "_OnACPlayerLogoutFaild" , "player logout AC failed" );
}

bool    SwitchClass::_CheckAccount( char* Account )
{
    if( _LoginAccount.find( Account ) == _LoginAccount.end() )
	{
        //TRACE1( "\n_CheckAccount 帳號檢查有問題 %s 不存在" , Account );
		Print( LV5 , "_CheckAccount" , "acccount( %s) error , not exsit" , Account );
        return false;
    }
    return true;
}

/*
void    SwitchClass::_ItemExchangeResult( int GSrvNetID ,  char* UserAccount , int RoleGUID , CheckExchangeItemTypeENUIM Type 
										 , GameItemFieldStruct& Item , int Money , int Money_Account )
{
	Print( LV1 , "_ItemExchangeResult" , "玩家兌換物品 " );
	SysNet_Switch_GSrv_ExchangeItemResult Send;

//	Send.UserAccount[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
	MyStrcpy( Send.UserAccount , UserAccount , sizeof(Send.UserAccount) );
	Send.RoleGUID		= RoleGUID;
	Send.Type			= Type;
//	Send.ItemObjDBID	= ItemObjDBID;		//物件號碼
//	Send.ItemCount		= ItemCount;
	Send.ItemID			= Item.ItemID;		//物件樣版的ID
	Send.Count			= Item.Count;		//數量
	Send.ExValue		= Item.ExValue;
	Send.Inherent		= Item.Inherent;    //天生附加能力
	Send.Rune[0]       	= Item.Rune[0];     //符石
	Send.Rune[1]       	= Item.Rune[1];     //符石
	Send.Rune[2]       	= Item.Rune[2];		//符石
	Send.Rune[3]       	= Item.Rune[3];     //符石
	Send.MainColor		= Item.MainColor;
	Send.OffColor		= Item.OffColor;
	Send.Mode			= Item.Mode;		//物品狀態
	Send.ImageObjID		= Item.ImageObjID;	//顯像的物件
	Send.Money			= Money;
	Send.Money_Account	= Money_Account;

    SendToGSrv( GSrvNetID , sizeof( Send ) , &Send );
}
*/