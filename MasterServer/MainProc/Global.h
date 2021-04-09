#ifndef __GLOBAL_H__
#define __GLOBAL_H__

//#include "../NetWaker/CliNetWaker.h"
#include <Windows.h>
//#include "../NetWalker_Member/NetWakerMasterSrvInc.h"
#include "IniFile/IniFile.h"
//#include "debuglog/errormessage.h"
#include "lua/myvm/lua_vmclass.h"
#include "LuaPlot/LuaPlot.h"

//#include "NetWaker/GSrvNetWaker.h"

#include "RoleData/RoleDataEx.h"
//#include "../Netwalker_Member/ServerList/ServerList_Child.h"

//class CServerList_Child;
#include "RoleData/ObjectDataClass.h"

#include "GlobalBase.h"
#include "ControllerClient/ControllerClient.h"

class Global : public GlobalBase
{
public:
    static IniFileClass	                _Ini;
    static bool     _IsDelay;								// 是否Server 在Delay狀態
    static bool     _IsDestroy;

	static bool						_IsStopLogin;
	static int						_EnableLimit;
	static int						_MaxPlayerCount;
	static int						_MaxLimitPlayerCount;
	static int						_EnableObjWait;
	static int						_MaxObjCount;
	static int						_MaxZonePlayerCount;
	static int						_MaxCreateRoleCount;
	static int						_PlayerCenterCount;
	static int						_PlayerCount[100];
	static map< int , vector<int> > _PlayerZoneCount;
	static map<int , int>			_ZonePlayerCount;		//區域人數
	//static LanguageTypeENUM			_LanguageType;
	static CountryTypeENUM			_CountryType;
	static bool						_NewAccountDisableCreateRole;
	static bool						_CheckSecondPassword;
	static bool						_CheckSecondPassword_ExtraCheck_Captcha;	//二次密碼檢查 - 額外的驗證碼檢查
	static int						_CaptchaMaxStringLength;					//驗證碼字串長度
	static int						_CaptchaAlphabetCount;						//驗證碼字串包含幾個英文字母, 其餘的為數字
	static int						_CaptchaBackGroundImageCount;				//驗證碼字串背景圖數量
	static int						_CaptchaNoiseImageCount;					//驗證碼字串破壞紋圖數量
	static int						_EnableGameGuard;

    static void		Destory() { _IsDestroy = true; };
	static bool		Init( char* IniFile );
    static bool		Release( );
    static bool		Process( );
	static GameObjDbStructEx*    GetObjDB( int OrgID );
	//重新計算線上人物資料
	static void		RecalculateOnlinePlayer();	
	static int		GetPCenterID( const char* account );

	//Log儲存
	static void		Log_ServerStatus( );

	static bool						_IsEnable_APEX;
	static bool						_IsEnable_APEX_KICK;
	static bool						_IsEnable_BTI;

	static char						_szBTI_IP1[100];
	static int						_iBTI_PORT1;

	static char						_szBTI_IP2[100];
	static int						_iBTI_PORT2;

	static int						_IsEnablePlayHistoryLog;

	
	static int						_IsEnable_DisconnectPriority;


	static char						_szVivoxUrl[256];
	static char						_szVivoxAccount[50];
	static char						_szVivoxPassword[50];
	static char						_AgencyKeyword[10][16];

	static int						_IsEnableAgencyCCU;

};

// MASTER 參數 DEFINE
#define DF_TIME_WAITLSRVCONNECT		3000
#define DF_TIME_ZONESRV_REGTIMEOUT	9000



#endif //__GLOBAL_H__