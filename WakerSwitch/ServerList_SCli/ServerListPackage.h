#include "ServerListPackageDef.h"
// S = ServerList
// SC = Game Srv ( 目前指 Switch Srv )
// C = Client

namespace   NetWakerServerListPackage
{
	//--------------------------------------------------------------------------------------    
	//空系統封包
	struct ServerListNet_SysNull
	{
		ServerListPacketCommandEnum Command;
	};
	//-------------------------------------------------------------------------------------
	//SC -> S 註冊目前線上人數
	struct ServerListNet_SCtoS_RegServerInfo
	{
		ServerListPacketCommandEnum Command;
		char	ServerName[ _DEF_MAX_GSRVNAME_SIZE_ ];
		char	IP_DNS[ _DEF_MAX_IPSTR_SIZE_ ];
		int		Port;
		int		MaxPlayerCount;		//最大可上線人數
		int		ParallelZoneCount;
		int		PlayerCount[100];
		int		GameID;
		int		ServerStatus;
		float	TotalPlayerRate;

		ServerListNet_SCtoS_RegServerInfo( )
		{
			Command = EM_ServerListNet_SCtoS_RegServerInfo;
		}
	};
	//-------------------------------------------------------------------------------------
	//C -> S 要求ServerList 資料
	struct ServerListNet_CtoS_ServerListRequest
	{
		ServerListPacketCommandEnum Command;
		char    Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
		char    Password[ _DEF_MAX_PASSWORD_SIZE_ ];
		bool	KickAccount;
		bool	HashedPassword;

		ServerListNet_CtoS_ServerListRequest( )
		{
			Command = EM_ServerListNet_CtoS_ServerListRequest;
		}
	};
	//-------------------------------------------------------------------------------------
	//S -> C ServerList資料
	struct ServerListNet_StoC_ServerListResult
	{
		ServerListPacketCommandEnum Command;

		int			TotalCount;
		int			Index;

		char		ServerName[ _DEF_MAX_GSRVNAME_SIZE_ ];
		char		IP_DNS[ _DEF_MAX_IPSTR_SIZE_ ];
		int			Port;
		int			ParallelZoneCount;			//平行空間數量
		float		PlayerRate[100];			//0(空)~1(滿)
		int			UpdateTime;					//多久前更新的
		int			WorldID;
		int			LastLoginWorldID;			//
		char    	Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
		unsigned	IPNum;						
		char		Country[3];					//國家縮寫

		int			ServerStatus;				//0 為正常, 1 為維修, 2 為隱藏
		int			ServerlistMode;				//0 表正常, 1 表維修
		float		TotalPlayerRate;
		int			Age;
		int			ValidTime;
		int			VivoxLicenseTime;

		ServerListNet_StoC_ServerListResult( )
		{
			Command = EM_ServerListNet_StoC_ServerListResult;
		}
	};
	//-------------------------------------------------------------------------------------
	enum ServerListFailed_ENUM
	{
		EM_ServerListFailed_Unknow			= -1,
		EM_ServerListFailed_RoleInWorld		,		//角色還在遊戲中
		EM_ServerListFailed_Password_Error	,		//帳號或密碼是錯的
		EM_ServerListFailed_NoWorld			,		//沒有一個世界是開的
		EM_ServerListFailed_ValidTimeError	,		//帳號有效時間
		EM_ServerListFailed_IPBlock			,		//此區域IP不能登入
		EM_ServerListFailed_CommLock		,		//通訊鎖鎖定中

		EM_ServerListFailed_Freeze_Self		= 100,	//自己帳號冷凍
		EM_ServerListFailed_Freeze_Official	,		//官方帳號冷凍
		EM_ServerListFailed_Freeze_Other3	,		//帳號冷凍
		EM_ServerListFailed_Freeze_Other4	,		//帳號冷凍
		EM_ServerListFailed_Freeze_Other5	,		//帳號冷凍
		EM_ServerListFailed_Freeze_Other6	,		//帳號冷凍
		EM_ServerListFailed_Freeze_Other7	,		//帳號冷凍
		EM_ServerListFailed_Freeze_Other8	,		//帳號冷凍
		EM_ServerListFailed_Freeze_Other9	,		//帳號冷凍

		EM_ServerListFailed_BetaKeyRequest  = 200,	//k2需要beta 
		EM_ServerListFailed_Gama_Wrong_OTP		,	//橘子錯誤 Wrong_OTP : One time password is not correct
		EM_ServerListFailed_Gama_Wrong_EventOTP	,
		EM_ServerListFailed_VN_Wrong_UserState	,	//越南登入錯誤訊息 帳號沒有認證
		EM_ServerListFailed_Gama_Wrong_Timeout	,
		EM_ServerListFailed_Gama_NoAccount		,	//帳號不存在
		EM_ServerListFailed_KR_AgeRestriction	,	//韓國登入 - 年齡限制

		EM_ServerListFailed_ConnectFailed	= 300	// 連線失敗, 請檢查網路或防火牆設定
	};
	//S -> C 要求ServerList 資料失敗
	struct ServerListNet_StoC_ServerListFailed
	{
		ServerListPacketCommandEnum Command;
		ServerListFailed_ENUM Result;

		ServerListNet_StoC_ServerListFailed( )
		{
			Command = EM_ServerListNet_StoC_ServerListFailed;
		}
	};
	//-------------------------------------------------------------------------------------
	// S -> SC 當帳號重復登入通知Switch
	struct ServerListNet_StoSC_CliReloginNotify
	{
		ServerListPacketCommandEnum Command;
		char	Account[_DEF_MAX_ACCOUNTNAME_SIZE_];

		ServerListNet_StoSC_CliReloginNotify( )
		{
			Command = EM_ServerListNet_StoSC_CliReloginNotify;
		}
	};
	//-------------------------------------------------------------------------------------
	// S -> C 通知帳號在各Server內的人數
	struct ServerListNet_StoC_GameRoleCount
	{
		ServerListPacketCommandEnum Command;
		char		RoleCount[30];
		int			DataSize;
		char		FileData[0x10000];

		ServerListNet_StoC_GameRoleCount( )
		{
			Command = EM_ServerListNet_StoC_GameRoleCount;
		}
		int Size()
		{
			return sizeof( *this ) - sizeof( FileData ) + DataSize;
		}
	};
   	//-------------------------------------------------------------------------------------
	//要求轉送封包
	struct ServerListNet_CtoS_ForwardTo
	{
		ServerListPacketCommandEnum Command;

		//int		IPNum;
		char	IPStr[ _DEF_MAX_IPSTR_SIZE_ ];
		int		Port;

		ServerListNet_CtoS_ForwardTo( )
		{
			Command = EM_ServerListNet_CtoS_ForwardTo;
		}
	};
	//-------------------------------------------------------------------------------------
	//寫入Vivox 
	struct ServerListNet_CtoS_SetVivoxLicenseTime
	{
		ServerListPacketCommandEnum Command;

		ServerListNet_CtoS_SetVivoxLicenseTime( )
		{
			Command = EM_ServerListNet_CtoS_SetVivoxLicenseTime;
		}
	};	
	//-------------------------------------------------------------------------------------
	// S -> C 要求 Client 重置密碼
	struct ServerListNet_StoC_ResetPassword
	{
		ServerListPacketCommandEnum Command;

		ServerListNet_StoC_ResetPassword( )
		{
			Command = EM_ServerListNet_StoC_ResetPassword;
		}
	};

	//-------------------------------------------------------------------------------------
	// C -> S Client 重置密碼
	struct ServerListNet_CtoS_ResetPasswordResult
	{
		ServerListPacketCommandEnum Command;

		//char    Account[ 64 ];
		char    Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
		char    OldPassword[ _DEF_MAX_PASSWORD_SIZE_ ];
		char    NewPassword[ _DEF_MAX_PASSWORD_SIZE_ ];

		ServerListNet_CtoS_ResetPasswordResult( )
		{
			Command = EM_ServerListNet_CtoS_ResetPasswordResult;
		}
	};

	//-------------------------------------------------------------------------------------
	// S -> C 設置密碼結果
	struct ServerListNet_StoC_ResetPasswordResult
	{
		ServerListPacketCommandEnum Command;

		int Result;	

		ServerListNet_StoC_ResetPasswordResult( )
		{
			Command = EM_ServerListNet_StoC_ResetPasswordResult;
		}
	};

	//-------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------
	//S -> C 要求ServerList 資料失敗 額外資料
	union ServerListFailedDataUnion
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

	struct ServerListNet_StoC_ServerListFailedData
	{
		ServerListPacketCommandEnum Command;
		ServerListFailed_ENUM		Result;
		ServerListFailedDataUnion	Data;

		ServerListNet_StoC_ServerListFailedData( )
		{
			memset(this, 0, sizeof(*this));
			Command = EM_ServerListNet_StoC_ServerListFailedData;
		}
	};
	//-------------------------------------------------------------------------------------
	//C -> S 記錄Client系統資料
	struct ServerListNet_CtoS_ClientSystemInfo
	{
		ServerListPacketCommandEnum Command;
		char    Account[ _DEF_MAX_ACCOUNTNAME_SIZE_ ];
		char		MacAddress[_DEF_MAX_MAC_ADDRESS_SIZE_];
		int			DisplayCardVendorID;
		int			DisplayCardDeviceID;
		char		OSINFO[256];
		ServerListNet_CtoS_ClientSystemInfo( )
		{
			Command = EM_ServerListNet_CtoS_ClientSystemInfo;
		}
	};
}
