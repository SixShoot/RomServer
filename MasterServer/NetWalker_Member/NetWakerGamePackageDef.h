#ifndef __NETWAKERPGEM_H__
#define __NETWAKERPGEM_H__

//-----------------------------------------------------------------------------------
#include "../NetWaker/NetWakerDefine.h"
//-----------------------------------------------------------------------------------

enum	GamePGCommandEnum
{
    EM_PG__NULL						= -1       ,
    // 測試資料
    EM_PG_Test_CliToGSrv           = 0         ,
    EM_PG_Test_GSrvToCli                       ,
    EM_PG_Test_GSrvToAllCli                    ,
    EM_PG_Test_GSrvToGSrv                      ,

    // 角色連結 GSrv 的封包
    EM_PG_CliConnect_LtoC_PlayerItemID = 10    ,
    EM_PG_CliConnect_LtoC_ZoneInfo             ,
    EM_PG_CliConnect_LtoC_PlayerData           ,    
    EM_PG_CliConnect_LtoC_PrepareChangeZone    ,
    EM_PG_CliConnect_LtoC_ChangeZoneOK         ,
    EM_PG_CliConnect_CtoL_RequestLogout        ,

    // 動做位置 的封包
    EM_PG_Move_LtoC_AddToPartition				= 20,
    EM_PG_Move_LtoC_DelFromPartition			,
    EM_PG_Move_LtoC_SetPostion					,
    EM_PG_Move_LtoC_ObjectMove					,
    EM_PG_Move_CtoL_PlayerMoveObject			,
    EM_PG_Move_LtoC_DoAction					,
    EM_PG_Move_LtoC_MoveSpeed					,
    EM_PG_Move_LtoC_ModelRate					,


	// Master Server
	// Client 剛連入時, 取出資料用
	EM_PG_Login_M2D_RequestAccountData			= 30,
	EM_PG_Login_D2M_AccountData					,
	EM_PG_Login_M2C_AcountData					,
	
	// Client 得到資料後, 建立或選則角色用
	EM_PG_Character_C2M_SelectCharacter			,
	EM_PG_Character_M2C_ReplySelectCharacter	,

	// SERVER LIST
	EM_PG_SL_SL2S_RequestServerData				= 40,
	EM_PG_SL_SL2S_ServerData					,


    EM_PG_Test_MAX         = 0xffffffff,
};



#endif //__NETWAKERPGEM_H__