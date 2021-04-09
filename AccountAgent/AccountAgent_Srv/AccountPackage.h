#ifndef __ACCOUNT_PACKAGE__
#define __ACCOUNT_PACKAGE__
#include "AccountPackageDef.h"
// S = Accent Srv
// C = Game Srv ( �ثe�� Switch Srv )

namespace   NetWakerAccountPackage
{
    //--------------------------------------------------------------------------------------    
    //�Ũt�Ϋʥ]
    struct AccountNet_SysNull
    {
        AccountPacketCommandEnum Command;
    };

    //--------------------------------------------------------------------------------------    
    //���Y�@��GSrv Group �����U
    struct AccountNet_CtoS_GSrvLogin
    {
        AccountPacketCommandEnum Command;

        myUInt32      GameID;                     //���@�ӹC���s��
        char        Name[ _DEF_MAX_GSRVNAME_SIZE_ ];

        AccountNet_CtoS_GSrvLogin( )
        {
            Command = EM_AccountNet_CtoS_GSrvLogin;
        }
    };
    //--------------------------------------------------------------------------------------    
    //�^�� GSrv_Login���I�G
    struct AccountNet_StoC_GSrvLoginResult
    {
        AccountPacketCommandEnum Command;

        myUInt32              GameID;             //���@�ӹC���s��
        GSrvLoginResultEnum Result;

        AccountNet_StoC_GSrvLoginResult( )
        {
            Command = EM_AccountNet_StoC_GSrvLoginResult;
        }
    };
    //--------------------------------------------------------------------------------------    
    //���Y�@�b�����U
    struct AccountNet_CtoS_PlayerLogin
    {
        AccountPacketCommandEnum Command;
        char    Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
        char    Password[ _DEF_MAX_PASSWORD_SIZE_ ];
		char	MacAddress[ _DEF_MAX_MAC_ADDRESS_SIZE_ ];
        myUInt32  IPnum;
        myUInt32  UserID;                             //��Ʀ^�e�ɪ��D�O���ӱb���n�D
		bool	HashedPassword;						  //�K�X�O�_�w�g�g�LMD5�B�z

        AccountNet_CtoS_PlayerLogin( )
        {
            Command = EM_AccountNet_CtoS_PlayerLogin;
        }
    };

    //--------------------------------------------------------------------------------------    
    //���U���G
    struct AccountNet_StoC_PlayerLoginResult
    {
        AccountPacketCommandEnum Command;
        char            Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
        myUInt32        UserID;                             //�^�e���e�X��ID
        LoginResultEnum Result;
        ChargTypeEnum   ChargType;
        int             Point;

		//���I�g
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
    //�Y�@�b���n�X
    struct AccountNet_CtoS_PlayerLogout
    {
        AccountPacketCommandEnum Command;
        char           Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
        int	           UserID;                             //�^�e���e�X��ID( -1 ��ܭn�D�q�����b�����H���n,���O�d����o�ӤH)

        AccountNet_CtoS_PlayerLogout( )
        {
            Command = EM_AccountNet_CtoS_PlayerLogout;
        }
    };
    //--------------------------------------------------------------------------------------    
    //�n�X���G
    struct AccountNet_StoC_PlayerLogoutResult
    {
        AccountPacketCommandEnum Command;
        char                Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
        myUInt32              UserID;                         //�^�e���e�X��ID
        LogoutResultEnum    Result;

        AccountNet_StoC_PlayerLogoutResult( )
        {
            Command = EM_AccountNet_StoC_PlayerLogoutResult;
        }
    };
    //--------------------------------------------------------------------------------------    
    //�ˬd��Account �O�_�b Game �@�ɤ�
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
    //�^�����G �ˬd��Account �O�_�b Game �@�ɤ�
    struct AccountNet_CtoS_CheckAccountResult
    {
        AccountPacketCommandEnum Command;
        char            Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
        bool            Result;     //true ��ܦb false ��ܤ��b

        AccountNet_CtoS_CheckAccountResult( )
        {
            Command = EM_AccountNet_CtoS_CheckAccountResult;
        }
    };
    //--------------------------------------------------------------------------------------    
	/*
	// �q���ϥΧI���ӫ~�n�D
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
		int				ItemSerial;			//���~�Ǹ�
	
		AccountNet_CtoS_CheckExchangeItemRequest( )
		{
			Command = EM_AccountNet_CtoS_CheckExchangeItemRequest;
		}
	};
	//--------------------------------------------------------------------------------------    
	// �q���ϥΧI���ӫ~�n�D
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
	// �n�D��Y�ӱb��Ban
	struct AccountNet_CtoS_FreezeAccount
	{
		AccountPacketCommandEnum	Command;		
		char            			UserAccount[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
		int							FreezeType;		//0 �L 1 �ۤv 2 �x��

		AccountNet_CtoS_FreezeAccount( )
		{
			Command = EM_AccountNet_CtoS_FreezeAccount;
		}
	};
	//--------------------------------------------------------------------------------------    

	//--------------------------------------------------------------------------------------
	//�n�J����, �B�~�^�ǵ�Client�����
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
		myUInt32        UserID;                             //|^�Xe�Piai�Xe�DXaoID
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