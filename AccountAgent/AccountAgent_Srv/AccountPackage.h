#ifndef __ACCOUNT_PACKAGE__
#define __ACCOUNT_PACKAGE__
#include "AccountPackageDef.h"
// S = Accent Srv
// C = Game Srv ( 目前指 Switch Srv )

namespace   NetWakerAccountPackage
{
    //--------------------------------------------------------------------------------------    
    //空系統封包
    struct AccountNet_SysNull
    {
        AccountPacketCommandEnum Command;
    };

    //--------------------------------------------------------------------------------------    
    //讓某一個GSrv Group 號註冊
    struct AccountNet_CtoS_GSrvLogin
    {
        AccountPacketCommandEnum Command;

        myUInt32      GameID;                     //那一個遊戲群組
        char        Name[ _DEF_MAX_GSRVNAME_SIZE_ ];

        AccountNet_CtoS_GSrvLogin( )
        {
            Command = EM_AccountNet_CtoS_GSrvLogin;
        }
    };
    //--------------------------------------------------------------------------------------    
    //回傳 GSrv_Login的截果
    struct AccountNet_StoC_GSrvLoginResult
    {
        AccountPacketCommandEnum Command;

        myUInt32              GameID;             //那一個遊戲群組
        GSrvLoginResultEnum Result;

        AccountNet_StoC_GSrvLoginResult( )
        {
            Command = EM_AccountNet_StoC_GSrvLoginResult;
        }
    };
    //--------------------------------------------------------------------------------------    
    //讓某一帳號註冊
    struct AccountNet_CtoS_PlayerLogin
    {
        AccountPacketCommandEnum Command;
        char    Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
        char    Password[ _DEF_MAX_PASSWORD_SIZE_ ];
		char	MacAddress[ _DEF_MAX_MAC_ADDRESS_SIZE_ ];
        myUInt32  IPnum;
        myUInt32  UserID;                             //資料回送時知道是那個帳號要求
		bool	HashedPassword;						  //密碼是否已經經過MD5處理

        AccountNet_CtoS_PlayerLogin( )
        {
            Command = EM_AccountNet_CtoS_PlayerLogin;
        }
    };

    //--------------------------------------------------------------------------------------    
    //註冊結果
    struct AccountNet_StoC_PlayerLoginResult
    {
        AccountPacketCommandEnum Command;
        char            Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
        myUInt32        UserID;                             //回送當初送出的ID
        LoginResultEnum Result;
        ChargTypeEnum   ChargType;
        int             Point;

		//防沉迷
		int				PlayTimeQuota;


		char    LastLoginIP     [ _DEF_MAX_IPSTR_SIZE_ ];
		char    LastLoginTime   [ _DEF_MAX_TIMESTR_SIZE_ ];		

        AccountNet_StoC_PlayerLoginResult( )
        {
            Command = EM_AccountNet_StoC_PlayerLoginResult;
			LastLoginIP[0]=0;
			LastLoginTime[0]=0;
        }
    };
    //--------------------------------------------------------------------------------------    
    //某一帳號登出
    struct AccountNet_CtoS_PlayerLogout
    {
        AccountPacketCommandEnum Command;
        char           Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
        int	           UserID;                             //回送當初送出的ID( -1 表示要求通知此帳號有人重登,但是查不到這個人)

        AccountNet_CtoS_PlayerLogout( )
        {
            Command = EM_AccountNet_CtoS_PlayerLogout;
        }
    };
    //--------------------------------------------------------------------------------------    
    //登出結果
    struct AccountNet_StoC_PlayerLogoutResult
    {
        AccountPacketCommandEnum Command;
        char                Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
        myUInt32              UserID;                         //回送當初送出的ID
        LogoutResultEnum    Result;

        AccountNet_StoC_PlayerLogoutResult( )
        {
            Command = EM_AccountNet_StoC_PlayerLogoutResult;
        }
    };
    //--------------------------------------------------------------------------------------    
    //檢查此Account 是否在 Game 世界中
    struct AccountNet_StoC_CheckAccount
    {
        AccountPacketCommandEnum Command;
        char            Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];

        AccountNet_StoC_CheckAccount( )
        {
            Command = EM_AccountNet_StoC_CheckAccount;
        }
    };
    //--------------------------------------------------------------------------------------    
    //回應結果 檢查此Account 是否在 Game 世界中
    struct AccountNet_CtoS_CheckAccountResult
    {
        AccountPacketCommandEnum Command;
        char            Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
        bool            Result;     //true 表示在 false 表示不在

        AccountNet_CtoS_CheckAccountResult( )
        {
            Command = EM_AccountNet_CtoS_CheckAccountResult;
        }
    };
    //--------------------------------------------------------------------------------------    
	/*
	// 通知使用兌換商品要求
	struct AccountNet_CtoS_CheckExchangeItemRequest
	{
		AccountPacketCommandEnum Command;
		char            UserAccount[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
		char			UserIP[_DEF_MAX_IPSTR_SIZE_];
		char			ItemKey[_DEF_MAX_ITEM_KEY_SIZE_];
		char			UserRoleName[_DEF_MAX_ROLETNAME_SIZE_];
		int				GSrvNetID;
		int				RoleGUID;
		int				WorldID;	
		int				ItemSerial;			//物品序號
	
		AccountNet_CtoS_CheckExchangeItemRequest( )
		{
			Command = EM_AccountNet_CtoS_CheckExchangeItemRequest;
		}
	};
	//--------------------------------------------------------------------------------------    
	// 通知使用兌換商品要求
	struct AccountNet_StoC_CheckExchangeItemResult
	{
		AccountPacketCommandEnum	Command;
		int							GSrvNetID;
		char            			UserAccount[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
		int							RoleGUID;
		CheckExchangeItemTypeENUIM	Type;
		GameItemFieldStruct			Item;
		int							Money;
		int							Money_Account;

		AccountNet_StoC_CheckExchangeItemResult( )
		{
			Command = EM_AccountNet_StoC_CheckExchangeItemResult;
		}
	};
	*/
	//--------------------------------------------------------------------------------------    
	// 要求把某個帳號Ban
	struct AccountNet_CtoS_FreezeAccount
	{
		AccountPacketCommandEnum	Command;		
		char            			UserAccount[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
		int							FreezeType;		//0 無 1 自己 2 官方

		AccountNet_CtoS_FreezeAccount( )
		{
			Command = EM_AccountNet_CtoS_FreezeAccount;
		}
	};
	//--------------------------------------------------------------------------------------    

	//--------------------------------------------------------------------------------------
	//登入失敗, 額外回傳給Client的資料
	union PlayerLoginFailedDataUnion
	{
		struct
		{
			int Count;
			int Max;
			int FreezeTime;
		}PasswordError;

		struct
		{
			int RemainingTime;
		}Freeze;
	};

	struct AccountNet_StoC_PlayerLoginFailedData
	{
		AccountPacketCommandEnum	Command;
		char            Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
		myUInt32        UserID;                             //|^°e·iai°e￥XaoID
		LoginResultEnum Result;
		PlayerLoginFailedDataUnion Data;

		AccountNet_StoC_PlayerLoginFailedData()
		{
			memset(this, 0, sizeof(*this));
			Command = EM_AccountNet_StoC_PlayerLoginFailedData;
		}
	};
	//--------------------------------------------------------------------------------------
}
#endif //__ACCOUNT_PACKAGE__