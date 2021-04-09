#ifndef __ACCOUNT_PACKAGE_DEF__
#define __ACCOUNT_PACKAGE_DEF__
#include "MyTypeDef.h"

namespace   NetWakerAccountPackage
{
    
    //-----------------------------------------------------------------------------------
    /*
    #define UInt8	unsigned char	
    #define UInt16	unsigned short
    #define myUInt32	unsigned long

    #define Int8	char
    #define Int16	short 
    #define Int32	long
    */
    //-----------------------------------------------------------------------------------
	#define _DEF_MAX_ROLETNAME_SIZE_	32
    #define _DEF_MAX_ACCOUNTNAME_SIZE_  64
    #define _DEF_MAX_PASSWORD_SIZE_     256
	#define _DEF_MAX_MAC_ADDRESS_SIZE_  256
    //#define _DEF_MAX_GSRVNAME_SIZE_     32
	#define _DEF_MAX_GSRVNAME_SIZE_     64
    #define _DEF_MAX_IPSTR_SIZE_        64
    #define _DEF_MAX_TIMESTR_SIZE_      28
	#define	_DEF_MAX_ITEM_KEY_SIZE_		32

    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------

    //封包命令列舉
    enum	AccountPacketCommandEnum
    { 
        EM_NOUSED                                  = 0    ,
        EM_AccountNet_CtoS_GSrvLogin                      ,
        EM_AccountNet_StoC_GSrvLoginResult                ,
        EM_AccountNet_CtoS_PlayerLogin                    ,
        EM_AccountNet_StoC_PlayerLoginResult              ,
        EM_AccountNet_CtoS_PlayerLogout                   ,
        EM_AccountNet_StoC_PlayerLogoutResult             ,
        EM_AccountNet_StoC_CheckAccount                   ,
        EM_AccountNet_CtoS_CheckAccountResult             ,
//		EM_AccountNet_CtoS_CheckExchangeItemRequest		  ,
//		EM_AccountNet_StoC_CheckExchangeItemResult		  ,
		
		EM_AccountNet_CtoS_FreezeAccount				  ,
		EM_AccountNet_StoC_PlayerLoginFailedData		  ,

		EM_AccountNet_Packet_Count						  ,
        EM_AccountNet_MAX = 0xffffffff,
    };

    //付費型態
    enum ChargTypeEnum
    {
        EM_ChargType_Trial                  = 1 ,
        EM_ChargType_Hour                       ,
        EM_ChargType_Day                        ,
        EM_ChargType_Month                      ,
        EM_ChargType_Season                     , 

        EM_ChargType_MAX = 0xffffffff,
    };

    //登入結果
    enum LoginResultEnum
    {
        EM_LoginResul_OK              = 0 ,
        EM_LoginResul_Logout             ,
        EM_LoginResul_NotFind            ,		//* 
        EM_LoginResul_LoginTwice         ,
        EM_LoginResul_PasswordError      ,		//*
        EM_LoginResul_LoginByOtherSrv    ,		//Game ID != -1
        EM_LoginResul_SrvDisconnect      ,		//(目前沒用)
        EM_LoginResul_NotEnoughPoint     ,		//(目前沒用)
        EM_LoginResul_NotCorrectChargType,		//不正確的付費狀態(目前沒用)
        EM_LoginResul_Suspended          ,		//警告
        EM_LoginResul_Banned             ,		//被踢
		EM_LoginResul_AccountStrError	 ,		//帳號字串錯誤
		EM_LoginResul_SwitchError		 ,		//Switch 註冊有問題
		EM_LoginResul_FreezeType_Self	 ,
		EM_LoginResul_FreezeType_Official,
		EM_LoginResul_FreezeType_Other3	 ,
		EM_LoginResul_FreezeType_Other4	 ,
		EM_LoginResul_FreezeType_Other5	 ,
		EM_LoginResul_FreezeType_Other6	 ,
		EM_LoginResul_FreezeType_Other7	 ,
		EM_LoginResul_FreezeType_Other8	 ,
		EM_LoginResul_FreezeType_Other9	 ,
		EM_LoginResul_llegalMacAddress	 ,
		EM_LoginResul_MacKick			 ,
		EM_LoginResul_MacBanAccount		 ,
		EM_LoginResul_PasswordErrorFreeze,
		EM_LoginResul_CommLock			 ,		//通訊鎖鎖定中

        EM_LoginResult_MAX = 0xffffffff,
    };

    enum AccountStateEnum
    {
        EM_AccountState_OnLogin         = -1,  //還在登入中
        EM_AccountState_OK              = 0 ,
        EM_AccountState_Suspended_LV1       ,
        EM_AccountState_Suspended_LV2       ,
        EM_AccountState_Suspended_LV3       ,
        EM_AccountState_Suspended_LV4       ,
        EM_AccountState_Banned              ,

        EM_AccountState_MAX = 0xffffffff,
    };

	enum AccountFreezeTypeEnum
	{
		EM_AccountFreezeType_None		=0	,
		EM_AccountFreezeType_Self			,
		EM_AccountFreezeType_Official		,		
	};

    //登出結果
    enum LogoutResultEnum
    {
        EM_GSrvLogoutResult_OK         = 0 ,
        EM_GSrvLogoutResult_NoAccount      ,  
        EM_GSrvLogoutResult_GameIDAccountNotMatch,
        EM_GSrvLogoutResult_Error          ,    //????

        EM_GSrvLogoutResult_MAX = 0xffffffff,
    };

    //Srv 登入結果
    enum GSrvLoginResultEnum
    {
        EM_GSrvLoginResult_OK         = 0 ,
        EM_GSrvLoginResult_RegTwice       ,
        EM_GSrvLoginResult_SrvNotReg      ,

        EM_GSrvLoginResult_MAX = 0xffffffff,
    };
	//-----------------------------------------------------------------------------------
	/*
	// 通知使用兌換商品要求
	enum	CheckExchangeItemTypeENUIM
	{
		EM_CheckExchangeItemType_WorldError = -3	,	//此Server不可兌換
		EM_CheckExchangeItemType_ItemUsed	= -2	,	//物品已被使用
		EM_CheckExchangeItemType_Failed		= -1	,	//失敗，無此物件
		EM_CheckExchangeItemType_OK					,
	};
	
	//-----------------------------------------------------------------------------------
	//遊戲物品結構
	struct GameItemFieldStruct
	{
		int				ItemID;					//物件樣版的ID
		int				Count;					//數量
		int				ExValue;
		int     		Inherent;               //天生附加能力
		int     		Rune[4];                //符石
		int				MainColor;
		int				OffColor;
		int				Mode;					//物品狀態
		int				ImageObjID;				//顯像的物件
	};
	*/
    //-----------------------------------------------------------------------------------
    // 事件回呼物件
    //-----------------------------------------------------------------------------------
    class AccountCliListener
    {
    public:
        virtual void OnLogin( ) = 0;
        virtual void OnLoginFailed( GSrvLoginResultEnum ) = 0;
        virtual void OnLogout( ) = 0;
        virtual void OnPlayerReg(  int UserID , char* Account , ChargTypeEnum , int Point , char* LastLoginIP , char* LastLoginTime , int PlayTimeQuota ) = 0;
        virtual void OnPlayerRegFailed(  int UserID , char* Account , LoginResultEnum ) = 0;
		virtual void OnPlayerRegFailedData(  int UserID , char* Account , LoginResultEnum, void* Data, unsigned long DataSize ) = 0;
        virtual void OnPlayerLogout( int UserID , char* Account ) = 0;
        virtual void OnPlayerLogoutFaild( int UserID , char* Account , LogoutResultEnum ) = 0;
        virtual bool CheckAccount( char* Account  ) = 0 ;

		//-----------------------------------------------------------------------------------
/*		virtual void ItemExchangeResult( int GSrvNetID , char* UserAccount , int RoleGUID , CheckExchangeItemTypeENUIM Type ,
										GameItemFieldStruct& Item , int Money , int Money_Account ) = 0;*/
    };

}
#endif //__ACCOUNT_PACKAGE_DEF__
