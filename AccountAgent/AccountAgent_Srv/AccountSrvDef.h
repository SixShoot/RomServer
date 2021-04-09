#ifndef __ACCOUNT_SRV_DEF__
#define __ACCOUNT_SRV_DEF__
#include "AccountPackageDef.h"
#include "ReaderClass/ReaderClass.h"
#include <set>
namespace   NetWakerAccountPackage
{
	/*
    //-------------------------------------------------------------------------------------
    //�s��DB����b����ƪ����c
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

		//���I�g
		int						PlayTimeQuota;		//�C���֭p�ɶ�
		int						LastLoginTime;		//�̫�n�J�ɶ�
		int						LastLogoutTime;		//�̫�n�X�ɶ�
		bool					IsInfant;			//�O�_�����~


        static ReaderClass<PlayerAccountDBStruct>* Reader( );

    };
    //-------------------------------------------------------------------------------------
    //�B�zDB�n�J��ƪ����c
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
    //�t�γB�z�b�����
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

		//���I�g�B�z
		int		LoginTime_Value;		
		int		PlayQuota;

		int		PassErrorCount;		//�K�X���~����
		int		FreezeRemainingTime;//�b���ᵲ�Ѿl�ɶ�

        PlayerAccountInfo( );
        void Init();
    };
    //-------------------------------------------------------------------------------------
    //�C���@��
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










