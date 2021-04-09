#ifndef __ACCOUNT_SRV_DEF__
#define __ACCOUNT_SRV_DEF__
#include "AccountPackageDef.h"
#include "ReaderClass/ReaderClass.h"
#include <set>
namespace   NetWakerAccountPackage
{
	/*
    //-------------------------------------------------------------------------------------
    //存取DB角色帳號資料的結構
    struct PlayerAccountDBStruct
    {
        int                 	GameID;
        char                	Account_ID  [ _DEF_MAX_ACCOUNTNAME_SIZE_ ];    
        char                	Password    [ 33 ];      
		bool					IsMd5Password;
		bool					IsAutoConvertMd5;
        ChargTypeEnum       	ChargType;    
        int                 	Point;
        AccountStateEnum		AccountState;    
		AccountFreezeTypeEnum	FreezeType;
		int						LastLoginGameID;

		//防沉迷
		int						PlayTimeQuota;		//遊戲累計時間
		int						LastLoginTime;		//最後登入時間
		int						LastLogoutTime;		//最後登出時間
		bool					IsInfant;			//是否未成年


        static ReaderClass<PlayerAccountDBStruct>* Reader( );

    };
    //-------------------------------------------------------------------------------------
    //處理DB登入資料的結構
    struct PlayerLoginDBStruct
    {
        char    Account_ID      [ _DEF_MAX_ACCOUNTNAME_SIZE_ ];    
        char    IP[32];
        char    Time[32];   

        LoginResultEnum       LoginResult;

        static ReaderClass<PlayerLoginDBStruct>* Reader( );

    };
	*/
    //-------------------------------------------------------------------------------------
    //系統處理帳號資料
    struct PlayerAccountInfo
    {
        int     GameID;
        int     NetID;
        char    Account_ID      [ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
        char    Password        [ _DEF_MAX_PASSWORD_SIZE_ ];
		char	MacAddress		[ _DEF_MAX_MAC_ADDRESS_SIZE_ ];
        int     Point;

        char    LastLoginIP     [ _DEF_MAX_IPSTR_SIZE_ ];
        char    LastLoginTime   [ _DEF_MAX_TIMESTR_SIZE_ ];

        char    LoginIP     [ _DEF_MAX_IPSTR_SIZE_ ];
        char    LoginTime   [ _DEF_MAX_TIMESTR_SIZE_ ];
       
        ChargTypeEnum       ChargType;
        AccountStateEnum    AccountState;
		AccountFreezeTypeEnum	FreezeType;
    
        int     TempUserID;

		//防沉迷處理
		int		LoginTime_Value;		
		int		PlayQuota;

		int		PassErrorCount;		//密碼錯誤次數
		int		FreezeRemainingTime;//帳號凍結剩餘時間

        PlayerAccountInfo( );
        void Init();
    };
    //-------------------------------------------------------------------------------------
    //遊戲世界
    struct GameSrvInfo
    {
        int     SockID;
        int     GameID;
        char    Name        [ _DEF_MAX_GSRVNAME_SIZE_ ];

        std::set< PlayerAccountInfo* > PlayerInfo;

        GameSrvInfo( );
        void Init( );
        bool IsReady( );
    };
    //-------------------------------------------------------------------------------------
}

#endif // __ACCOUNT_SRV_DEF__










