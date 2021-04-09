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
    static bool     _IsDelay;								// �O�_Server �bDelay���A
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
	static map<int , int>			_ZonePlayerCount;		//�ϰ�H��
	//static LanguageTypeENUM			_LanguageType;
	static CountryTypeENUM			_CountryType;
	static bool						_NewAccountDisableCreateRole;
	static bool						_CheckSecondPassword;
	static bool						_CheckSecondPassword_ExtraCheck_Captcha;	//�G���K�X�ˬd - �B�~�����ҽX�ˬd
	static int						_CaptchaMaxStringLength;					//���ҽX�r�����
	static int						_CaptchaAlphabetCount;						//���ҽX�r��]�t�X�ӭ^��r��, ��l�����Ʀr
	static int						_CaptchaBackGroundImageCount;				//���ҽX�r��I���ϼƶq
	static int						_CaptchaNoiseImageCount;					//���ҽX�r��}�a���ϼƶq
	static int						_EnableGameGuard;

    static void		Destory() { _IsDestroy = true; };
	static bool		Init( char* IniFile );
    static bool		Release( );
    static bool		Process( );
	static GameObjDbStructEx*    GetObjDB( int OrgID );
	//���s�p��u�W�H�����
	static void		RecalculateOnlinePlayer();	
	static int		GetPCenterID( const char* account );

	//Log�x�s
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

// MASTER �Ѽ� DEFINE
#define DF_TIME_WAITLSRVCONNECT		3000
#define DF_TIME_ZONESRV_REGTIMEOUT	9000



#endif //__GLOBAL_H__