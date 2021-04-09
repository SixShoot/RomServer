#pragma once
#include "MyTypeDef.h"

namespace   NetWakerServerListPackage
{
    //-----------------------------------------------------------------------------------
    #define _DEF_MAX_ACCOUNTNAME_SIZE_  64
    #define _DEF_MAX_PASSWORD_SIZE_     256
	#define _DEF_MAX_MAC_ADDRESS_SIZE_  256
    #define _DEF_MAX_IPSTR_SIZE_        64
    #define _DEF_MAX_TIMESTR_SIZE_      28
	//#define _DEF_MAX_GSRVNAME_SIZE_		32
	#define _DEF_MAX_GSRVNAME_SIZE_		64
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
	   //封包命令列舉
	enum	ServerListPacketCommandEnum
	{ 
		EM_NOUSED                                  = 0  ,
		EM_ServerListNet_SCtoS_RegServerInfo			,
		EM_ServerListNet_CtoS_ServerListRequest			,
		EM_ServerListNet_StoC_ServerListResult			,
		EM_ServerListNet_StoC_ServerListFailed			,
		EM_ServerListNet_StoSC_CliReloginNotify			,
		EM_ServerListNet_StoC_GameRoleCount				,
		EM_ServerListNet_CtoS_ForwardTo					,
		EM_ServerListNet_CtoS_SetVivoxLicenseTime		,
		EM_ServerListNet_StoC_ResetPassword				,
		EM_ServerListNet_CtoS_ResetPasswordResult		,
		EM_ServerListNet_StoC_ResetPasswordResult		,
		EM_ServerListNet_StoC_ServerListFailedData		,
		EM_ServerListNet_CtoS_ClientSystemInfo			,

		EM_ServerListNet_Packet_Count					,
		EM_ServerListNet_MAX = 0xffffffff,
	};

	//-----------------------------------------------------------------------------------
	// 事件回呼物件
	//-----------------------------------------------------------------------------------
	class ServerListListener
	{
	public:
		virtual void ClientRelogin( char* Account ) = 0;
	};
}



















