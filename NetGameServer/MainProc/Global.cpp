#include "Global.h"
#include "lua/myvm/lua_vmclass.h"
#include "LuaPlot/LuaPlot.h"
#include "TestDataClass.h"
#include "lua/LuaWrapper/LuaWrap.h"
#include "CutString/CutString.h"

#include "../NetWalker_Member/NetWakerGSrvInc.h"
#include "FlagPosList/FlagPosList.h"
#include "NetUtility.h"
#include "MagicProc/MagicProcess.h"
#include "AIProc/NpcAIBase.h"
#include "PartyInfoList/PartyInfoList.h"
#include "pathmanage//NPCMoveFlagManage.h"
#include "GuildManage/GuildManage.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
#include "HousesManage/HousesManageClass.h"
#include "GuildHouseManage/GuildHousesClass.h"
#include "GuildHouseWarManage/GuildHouseWarManage.h"

#include "Rune Engine/Rune/Rune.h"

#include "StackWalker/StackWalker.h"
//#include "CrashRpt/crashrpt.h" 
#include "FileVersion/FileVersion.h"
#include "cpu_usage/CpuUsage.h"

#include "lua/debugger/LuaDebugger.h"

#pragma		warning (disable:4305)
#include <math.h>
#include <Windows.h>

#ifndef PI
#define PI 3.14159265f
#endif

Global::GlobalStaticStruct* Global::_St = NULL;
CRuMTRandom		Global::m_ruRand;
int				Global::m_iShutdownTimes = 0;
int				Global::m_iSoundID = 0;
//----------------------------------------------------------------------------------------
/*
int		Global::_Dxz[7][7] = 	{	{ 0 ,  0,  0,  0,  0,  0,  0 },
{ 0 , 15,  3,  0, -3,-15,  0 },
{ 0 , -4,  0,  0,  0,  4,  0 },
{ 0 ,-12,  0,  0,  0, 12,  0 },
{ 0 , -4,  0,  0,  0,  4,  0 },
{ 0 , 15,  3,  0, -3,-15,  0 },
{ 0 ,  0,  0,  0,  0,  0,  0 },	};
*/
/*
float	Global::_Dxz[7][7] = 	{	{ 0 ,   0,   0,  0,   0,   0,  0 },
{ 0 , 1.5,  .3,  0, -.3,-1.5,  0 },
{ 0 , -.4,   0,  0,   0,  .4,  0 },
{ 0 ,-1.2,   0,  0,   0, 1.2,  0 },
{ 0 , -.4,   0,  0,   0,  .4,  0 },
{ 0 , 1.5,  .3,  0, -.3,-1.5,  0 },
{ 0 ,  0,    0,  0,   0,   0,  0 },	};*/

//---------------------------------------------------------------------------------------------
//·нµn¤JServerList¦pЄGҐў±СЄєCallBack
void	EventServerRegFailedCB(EM_REG_SRV_FAILED ememRegSrvDataFailed)
{
	Print(LV2, "EventServerRegFailedCB", "Local Server ID = %d , Repetition opens", RoleDataEx::G_ZoneID);
	Sleep(5000);
	exit(1);
}
//--------------------------------------------------------------------------------------
void     Global::InitRoleDataEx()
{
	RoleData::GlobalInit();
	//---------------------------------------------------------------------------------------
	RoleDataEx::G_SysTime_Base = timeGetTime();
	RoleDataEx::G_ZoneID = _St->Ini.ZoneID;
	RoleDataEx::G_PKType = (PKTypeENUM)_St->Ini.PKType;
	//---------------------------------------------------------------------------------------
	//		­PЇаRoleDataExЄ«ҐуЄєcallback
	//---------------------------------------------------------------------------------------
	RoleDataEx::St_GetRoleData = GetRoleDataByGUID;
	RoleDataEx::St_GetObjDB = GetObjDB;
	RoleDataEx::St_GenerateItemVersion = GenerateItemVersion;
	RoleDataEx::St_NewItemInit = NewItemInit;
	RoleDataEx::St_ProcessDead = ProcessDead;
	RoleDataEx::St_SpellMagic = MagicProcessClass::SysSpellMagic;
	RoleDataEx::St_CreateObj = Global::CreateObj;
	RoleDataEx::St_AddToPartition = Global::CheckClientLoading_AddToPartition;
	RoleDataEx::St_DelFromPartition = Global::DelFromPartition;
	RoleDataEx::St_ClearPath = ClearPath;
	RoleDataEx::St_SetPos = SetPos;
	RoleDataEx::St_GetRoleName = GetRoleName;
	RoleDataEx::St_SysKeyValue = SysKeyValue;
	RoleDataEx::St_RuneExchange = ObjectDataClass::RuneExchange;
	RoleDataEx::St_NextRuneLv = ObjectDataClass::NextRuneLv;
	RoleDataEx::St_GetSysKeyValue = ObjectDataClass::St_GetSysKeyValue;
	RoleDataEx::St_GetPCenterID = GetPCenterID;
}
//--------------------------------------------------------------------------------------
void     Global::InitNet()
{
	//--------------------------------------------------------------------------------------
	//єфёф«КҐ]ёк®ЖЄм©l¤Ж
	//--------------------------------------------------------------------------------------	
	NetSrv_CliConnectChild::Init();
	NetSrv_MoveChild::Init();
	NetSrv_ChannelBaseChild::Init();
	NetSrv_OnlinePlayerChild::Init();
	NetSrv_TalkChild::Init();
	Net_DCBaseChild::Init();
	CNetSrv_ZoneStatus_Child::Init();
	CNetSrv_PartBaseChild::Init();
	CNetSrv_PartExchangeDataChild::Init();
	NetSrv_ItemChild::Init();
	NetSrv_EQChild::Init();
	NetSrv_RoleValueChild::Init();
	NetSrv_AttackChild::Init();
	NetSrv_TradeChild::Init();
	CNetSrv_Script_Child::Init();
	NetSrv_MagicChild::Init();
	NetSrv_GMToolsChild::Init();
	NetSrv_ShopChild::Init();
	NetSrv_PartyChild::Init();
	NetSrv_OtherChild::Init();
	NetSrv_FriendListChild::Init();
	NetSrv_MailChild::Init();
	NetSrv_ACChild::Init();
	NetSrv_RecipeChild::Init();
	NetSrv_GuildChild::Init();
	CNetSrv_Gather_Child::Init();
	NetSrv_BotChild::Init();
	NetSrv_BillboardChild::Init();
	NetSrv_PKChild::Init();
	NetSrv_LotteryChild::Init();
	NetSrv_HousesChild::Init();
	NetSrv_DepartmentStoreChild::Init();
	NetSrv_ImportBoardChild::Init();

	NetSrv_TeleportChild::Init();
	NetSrv_TreasureChild::Init();
	NetSrv_GuildHousesChild::Init();
	NetSrv_CultivatePetChild::Init();
	//	NetSrv_GuildHousesWarChild::Init();
	NetSrv_BG_GuildWarChild::Init();
	CNetSrv_BattleGround_Child::Init();
	CNetSrv_FestivalChild::Init();
	NetSrv_InstanceChild::Init();
	CNetSrv_PublicEncounterChild::Init();
	Net_BillingChild::Init();
	Net_LoginChild::Init();

	//InitBattleGround();

	Net_ServerList->Init(NULL, EM_SERVER_TYPE_LOCAL, _St->Ini.ZoneID);
	//--------------------------------------------------------------------------------------
	Net_ServerList->RegSrvDataFailedEvent(EventServerRegFailedCB);


	_Net->RegOnProxyPingLog(_ProxyPingLog);

	_Net->RegGSrvNetID(EM_SERVER_TYPE_LOCAL, _St->Ini.ZoneID);
}
//--------------------------------------------------------------------------------------
void   Global::InitBattleGround()
{
	if (Ini()->BattleGroundType != 0)
	{
		//switch( Ini()->BattleGroundType )
		{
			//case 450:	CNetSrv_450_BattleGround_Child::m_pThis = NEW CNetSrv_450_BattleGround_Child; CNetSrv_450_BattleGround_Child::m_pThis->Init();		break;
			//case 451:	CNetSrv_451_BattleGround_Child::m_pThis = NEW CNetSrv_451_BattleGround_Child; CNetSrv_450_BattleGround_Child::m_pThis->Init();		break;
		}
	}
}
//--------------------------------------------------------------------------------------
void    Global::ReleaseBattleGround()
{
	if (Ini()->BattleGroundType != 0)
	{
		// TODO: ? why empty switch ?
		//switch( Ini()->BattleGroundType )
		//{
		//}
	}
}
//--------------------------------------------------------------------------------------
void     Global::InitBaseItemObject()
{
	BaseItemObject::Init();
	BaseItemObject::OnCreateCB(_OnCreateObj);
	BaseItemObject::OnDestroyCB(_OnDestroyObj);
}
//--------------------------------------------------------------------------------------
bool Global::InitLUA()
{
	//--------------------------------------------------------------------------------------
	//Єм©l¤Ж lua vartual Machine
	//--------------------------------------------------------------------------------------
	if (LUA_VMClass::Init(50000) == false)
	{
		PrintToController(true, "LUA_VMClass::Init( ) Єм©l¤ЖҐў±С!!");
		//MessageBox( NULL , "LUA_VMClass::Init( ) Єм©l¤ЖҐў±С!!" , "Error" ,MB_OK );
		return false;;
	}

	if (G_LuaWrap.Init(LUA_VMClass::L_State()) == false)
	{
		PrintToController(true, "G_LuaWrap.Ini Єм©l¤ЖҐў±С!!");
		//MessageBox( NULL , "G_LuaWrap.Ini Єм©l¤ЖҐў±С!!" , "Error" ,MB_OK );
		return false;;
	}

	if (LuaPlotClass::Init() == false)
	{
		PrintToController(true, "LuaPlotClass::Init( )  Єм©l¤ЖҐў±С!!");
		//MessageBox( NULL , "LuaPlotClass::Init( )  Єм©l¤ЖҐў±С!!" , "Error" ,MB_OK );
		return false;;
	}

	if (Global::Ini()->IsLoadFdbLuaScript == true)
	{
		Print(LV5, "Global::InitLUA", "Global::Ini()->IsLoadFdbLuaScript");
		g_ruResourceManager->LoadPackageFile("fdb\\luascript.fdb");
		Print(LV5, "Global::InitLUA", "g_ruResourceManager->LoadPackageFile() Done");
		char	FileName[256];

		for (int i = 1; ; i++)
		{
			sprintf_s(FileName, sizeof(FileName), "luascript\\%05d.lua", i);
			//Print(LV2, "Global::InitLUA", "g_ruResourceManager->FileExists() %s\n", FileName );	fflush( NULL );

			if (g_ruResourceManager->FileExists(FileName) == FALSE)
			{
				Print(LV5, "Global::InitLUA", "Global::Ini()->IsLoadFdbLuaScript End ID=%d", i);
				break;
			}
			else
			{
				//Print( LV1 , "Global::InitLUA" , "Global::Ini()->IsLoadFdbLuaScript (%s)" , FileName );
				Print(LV2, "Global::InitLUA", "Global::Ini()->IsLoadFdbLuaScript (%s)\n", FileName); fflush(NULL);
				//printf( "Global::InitLUA Global::Ini()->IsLoadFdbLuaScript (%s)\n", FileName );	fflush( NULL );
			}


			IRuStream* stream = g_ruResourceManager->LoadStream(FileName);
			if (stream)
			{
				//Print( LV2 , "Global::InitLUA" , "g_ruResourceManager->LoadStream() = %s Loaded" , FileName ); fflush( NULL );
				//printf( "g_ruResourceManager->LoadStream() %s \n", FileName );	fflush( NULL );
				//lua_pushlstring(L, (const char*)stream->OpenStreamMapping(), stream->GetStreamSize());
				char* luabuff = NEW char[(stream->GetStreamSize() + 1)];
				ZeroMemory(luabuff, (stream->GetStreamSize() + 1));
				memcpy(luabuff, (char*)stream->OpenStreamMapping(), stream->GetStreamSize());
				LUA_VMClass::DoBuffer((char*)luabuff, stream->GetStreamSize(), FileName);
				delete[] luabuff;
				delete stream;
			}
			else
			{
				//Print( LV2 , "Global::InitLUA" , "g_ruResourceManager->LoadStream() = %s NULL\n" , FileName );	fflush( NULL );
				//printf( "g_ruResourceManager->LoadStream() = %s NULL\n", FileName );	fflush( NULL );
			}
		}

		Print(LV5, "Global::InitLUA", "Global::Ini()->IsLoadFdbLuaScript() Done");
	}

	//    LUA_VMClass::m_pfOutputError = BroadcastError; //LUA_VMClass їщ»~їйҐX
	if (Global::Ini()->IsLoadTxtLuaScript == true)
	{
		Print(LV5, "Global::InitLUA", "Global::Ini()->IsLoadTxtLuaScript");
		LUA_VMClass::LoadAllFile(_St->Ini.LuaPath);
	}

	LUA_VMClass::SysVM()->PCall("Initialize", 0, 0, NULL);
	Print(LV5, "Global::InitLUA", "SysVM()->CallLuaFunc(Initialize) done");

	if (_St->Ini.LuaPathEx.size() != 0)
	{
		Print(LV5, "Global::InitLUAEx", "path=%s", _St->Ini.LuaPathEx.c_str());
		LUA_VMClass::LoadAllFile(_St->Ini.LuaPathEx);
		LUA_VMClass::SysVM()->CallLuaFunc("InitLuaEx", 0, 0, NULL);
		Print(LV5, "Global::InitLUAEx", "SysVM()->CallLuaFunc(InitLuaEx) done");
	}

	LUA_VMClass::SetOutputErrorFunc(SendLuaErrorInfo);

	//¤№і\±ѕёьLua Debugger
	if (Global::Ini()->IsAllowLuaDebugger)
	{
		//Єм©l¤Ж LuaDebugger
		LuaDebugger::Init(Global::Ini()->ZoneID, Global::Ini()->LuaDebuggerAgentIP.c_str(), Global::Ini()->LuaDebuggerAgentPort,
			Global::Ini()->IsLoadFdbLuaScript, Global::Ini()->IsLoadTxtLuaScript, Global::Ini()->ResourcePath.c_str(), Global::Ini()->LuaPath.c_str(),
			&LUA_VMClass::GetOwnerID, &Global::GetOrgObjID);

		LUA_VMClass::AllowDebugger();
	}

	return true;
}

bool Global::InitLUA_LoadFile()
{
	if (LUA_VMClass::Init(50000) == false)
	{
		PrintToController(true, "LUA_VMClass::Init( ) Єм©l¤ЖҐў±С!!");
		return false;;
	}

	if (G_LuaWrap.Init(LUA_VMClass::L_State()) == false)
	{
		PrintToController(true, "G_LuaWrap.Ini Єм©l¤ЖҐў±С!!");
		return false;;
	}

	if (LuaPlotClass::Init() == false)
	{
		PrintToController(true, "LuaPlotClass::Init( )  Єм©l¤ЖҐў±С!!");
		return false;;
	}

	//if (Global::Ini()->IsLoadTxtLuaScript == true)
	{
		Print(LV5, "Global::InitLUA_LoadFile", "Global::Ini()->IsLoadTxtLuaScript");
		LUA_VMClass::LoadAllFile("C:\\Runewaker\\Resource\\luascript");
	}
	return true;
}

bool Global::InitLUA_Init()
{
	if (Global::Ini()->IsLoadTxtLuaScript == true)
	{
		Print(LV5, "Global::InitLUA_Init", "Global::Ini()->IsLoadTxtLuaScript");
		LUA_VMClass::LoadAllFile(_St->Ini.LuaPath);
	}

	LUA_VMClass::SysVM()->PCall("Initialize", 0, 0, NULL);
	Print(LV5, "Global::InitLUA_Init", "SysVM()->CallLuaFunc(Initialize) done");

	if (_St->Ini.LuaPathEx.size() != 0)
	{
		Print(LV5, "Global::InitLUA_InitEx", "path=%s", _St->Ini.LuaPathEx.c_str());
		LUA_VMClass::LoadAllFile(_St->Ini.LuaPathEx);
		LUA_VMClass::SysVM()->CallLuaFunc("InitLuaEx", 0, 0, NULL);
		Print(LV5, "Global::InitLUA_InitEx", "SysVM()->CallLuaFunc(InitLUA_InitEx) done");
	}

	LUA_VMClass::SetOutputErrorFunc(SendLuaErrorInfo);

	if (Global::Ini()->IsAllowLuaDebugger)
	{
		LuaDebugger::Init(Global::Ini()->ZoneID, Global::Ini()->LuaDebuggerAgentIP.c_str(), Global::Ini()->LuaDebuggerAgentPort,
			Global::Ini()->IsLoadFdbLuaScript, Global::Ini()->IsLoadTxtLuaScript, Global::Ini()->ResourcePath.c_str(), Global::Ini()->LuaPath.c_str(),
			&LUA_VMClass::GetOwnerID, &Global::GetOrgObjID);

		LUA_VMClass::AllowDebugger();
	}

	return true;
}
//--------------------------------------------------------------------------------------
bool     Global::Init(char* IniFile)
{
	//Print(LV5, "Global::Init", "InitLUA_LoadFile()");
	//InitLUA_LoadFile();
	//і]©w°T®§їйҐX
	g_pPrint->Init(SysPrintOutput);


	if (_St == NULL)
	{
		_St = NEW(GlobalStaticStruct);
	}

	Print(LV5, "Global::Init", "GlobalBase::_Init()");

	int     i;
	GlobalBase::_Init();
	//--------------------------------------------------------------------------------------
	Print(LV5, "Global::Init", "_St->Ini.Init( IniFile )");
	Print(LV5, "Global::Init", "_St->Ini.Init( %s )", IniFile);
	if (_St->Ini.Init(IniFile) == false)
	{
		Print(LV5, "Global::Init", "GSrv Ini File Read Error!!");
		PrintToController(true, "GSrv Ini File Read Error!!");
		//MessageBox( NULL , "GSrv Ini File Read Error!!" , "Error" ,MB_OK );
		return false;
	}

	//і]©w¬O§_ЁПҐОBilling Server
	_St->IsUseBillingServer = (_St->Ini.Int("IsUseBillingServer") > 0);

	char FileName[256];
	sprintf_s(FileName, sizeof(FileName), "%s%d.log", _St->Ini.Str("DebugOutputFile").c_str(), _St->Ini.ZoneID);
	g_pPrint->SetOutputFileName(FileName);
	Print(LV5, "##Open Server##", "Time %s", TimeStr::Now_Time());
	//--------------------------------------------------------------------------------------
	Print(LV5, "Global::Init", "InitNet()");
	InitNet();
	Print(LV5, "Global::Init", "InitRoleDataEx()");
	InitRoleDataEx();
	Print(LV5, "Global::Init", "InitLUA()");
	InitLUA();
	//Print(LV5, "Global::Init", "InitLUA_Init()");
	//InitLUA_Init();
	Print(LV5, "Global::Init", "InitBaseItemObject()");
	InitBaseItemObject();
	Print(LV5, "Global::Init", "FlagPosClass::Init()");
	FlagPosClass::Init();
	Print(LV5, "Global::Init", "MagicProcessClass::Init()");
	MagicProcessClass::Init();
	Print(LV5, "Global::Init", "PartyInfoListClass::Init()");
	PartyInfoListClass::Init();
	Print(LV5, "Global::Init", "NPCMoveFlagManageClass::Init()");
	NPCMoveFlagManageClass::Init();
	Print(LV5, "Global::Init", "GuildManageClass::Init()");
	GuildManageClass::Init();
	Print(LV5, "Global::Init", "HousesManageClass::Init()");
	HousesManageClass::Init();
	if (Global::Ini()->IsGuildHouseZone
		|| Global::Ini()->IsGuildHouseWarZone)
	{
		Print(LV5, "Global::Init", "GuildHousesManageClass::Init()");
		GuildHousesManageClass::Init();
	}

	Print(LV5, "Global::Init", "Step1 OK");
	//°}АзЄм©l¤Ж
	GameObjDbStructEx* ObjDB = Global::GetObjDB(_St->Ini.CampStatuteObj);

	if (ObjDB != NULL)
	{
		RoleDataEx::Camp.Init(ObjDB);
	}
	else
	{
		PrintToController(true, "GSrv Ini File Read Error!! CampSatuteObj not find");
		//MessageBox( NULL , "GSrv Ini File Read Error!! CampSatuteObj not find" , "Error" ,MB_OK );
		return false;
	}

	//--------------------------------------------------------------------------------------
	//ЕЄЁъINIі]©wёк®Ж
	//--------------------------------------------------------------------------------------
	for (i = 0; i < _St->Ini.RoomCount; i++)
	{
		MyMapAreaManagement* NewPartition;
		NewPartition = NEW MyMapAreaManagement(_St->Ini.PartitionWidth
			, _St->Ini.PartitionHeight
			, _St->Ini.PartitionMaxItem);

		_St->Partition.push_back(NewPartition);

		PrivateRoomInfoStruct Temp;
		_St->PrivateRoomInfoList.push_back(Temp);
	}

	_Net->Connect((char*)_St->Ini.SwitchIP.c_str(), _St->Ini.Switch_GServerPort, (char*)_St->Ini.ZoneName.c_str());
	//--------------------------------------------------------------------------------------	

	//--------------------------------------------------------------------------------------
	_St->ComboProc.Init();
	_Net->RegOnClientPacketCrashOutput(ClientPacketCrashOutput);
	_Net->SetEnabledTryException(Ini()->IsEnableTryException);

	// і]©wЁC¤йҐф°И­«ёmЁЖҐу
	//--------------------------------------------------------------------------------------
	__time32_t				ResetTime;
	__time32_t				NowTime;
	struct	tm* gmt;

	_time32(&NowTime);

	gmt = _localtime32(&NowTime);


	NowTime = (int)_mktime32(gmt) + RoleDataEx::G_TimeZone * 60 * 60;

	Print(LV2, "InitSetDialyReset", "SetDialyReset Now GMT H[%d] M[%d] S[%d]", gmt->tm_hour, gmt->tm_min, gmt->tm_sec);


	// 24 ¤p®ЙҐф°И­«ёm
	//­pєв«e¤С­«ёm®Й¶ЎЄє¬нјЖ ( ҐШ«e№wі]¬O¦­¤W 06:00 )
	if (gmt->tm_hour >= DF_DAILY_RESET_TIME)
	{
		gmt->tm_hour = DF_DAILY_RESET_TIME;//+ RoleDataEx::G_TimeZone;
		gmt->tm_sec = 0;
		gmt->tm_min = 0;
		gmt->tm_mday = gmt->tm_mday;
		ResetTime = (int)_mktime32(gmt) + RoleDataEx::G_TimeZone * 60 * 60 + (24 * 60 * 60);
	}
	else
	{
		gmt->tm_hour = DF_DAILY_RESET_TIME; //+ RoleDataEx::G_TimeZone;
		gmt->tm_sec = 0;
		gmt->tm_min = 0;
		ResetTime = (int)_mktime32(gmt) + RoleDataEx::G_TimeZone * 60 * 60;
	}





	int iSec = abs(ResetTime - NowTime);

	Print(LV2, "InitSetDialyReset", "_CheckDailyQuestReset will run after [ %d ]sec", iSec);

	Schedular()->Push(_CheckDailyQuestReset, (iSec * 1000), NULL, NULL);
	//////////////////////////////////////////////////////////////////////////
	//і]©w°ЖҐ»ЄєKey
	Ini()->InstanceSaveID = -1;
	char	KeyStr[128];
	for (int i = 0; i < 20; i++)
	{
		sprintf(KeyStr, "InstanceKey%02d", i);
		int KeyZoneID = g_ObjectData->GetSysKeyValue(KeyStr);
		if (KeyZoneID == RoleDataEx::G_ZoneID)
		{
			Ini()->InstanceSaveID = i;
			break;
		}
	}

	//SpecialCharacterNameFile;
	g_ObjectData->LoadSpecialCharacterName(_St->Ini.Str("SpecialCharacterNameFile").c_str());

	//©w®Й¦C¦LLua°OѕРЕйЁПҐО¶q
	Schedular()->Push(PrintLuaMemoryUsage, _St->Ini.IntDef("PrintLuaMemoryUsageTimeGap", 600) * 1000, NULL, NULL);

	return true;
}
//--------------------------------------------------------------------------------------
void Global::SysPrintOutput(int LV, const char* Title, const char* Context)
{
	/*
	if( LV > 2 )
	{
		NetSrv_GMTools::S_SrvMsgToAllGMTools( LV , (char*)Title , (char*)Context );
	}
	*/

	if (St()->FromNotify != NULL)
	{
		St()->FromNotify->RecvWarningMessage(LV, (char*)Title, (char*)Context);
	}
}
//--------------------------------------------------------------------------------------
//°e°T®§µ№GM Tools
void Global::SendMsgToGMTools(int LV, const char* Title, const char* Context)
{
	NetSrv_GMTools::S_SrvMsgToAllGMTools(LV, (char*)Title, (char*)Context);
}
//--------------------------------------------------------------------------------------
bool     Global::LoadObjDB()
{

	return false;
}
//--------------------------------------------------------------------------------------
bool     Global::Release()
{
	int     i;

	BaseItemObject::Release();
	LuaPlotClass::Release();
	PathManageClass::Release();
	//--------------------------------------------------------------------------------------
	//єфёф«КҐ]ёк®ЖёСєc
	//--------------------------------------------------------------------------------------
	NetSrv_CliConnectChild::Release();
	NetSrv_MoveChild::Release();
	NetSrv_ChannelBaseChild::Release();
	NetSrv_OnlinePlayerChild::Release();
	NetSrv_TalkChild::Release();
	Net_DCBaseChild::Release();
	CNetSrv_ZoneStatus_Child::Release();
	CNetSrv_PartBaseChild::Release();
	CNetSrv_PartExchangeDataChild::Release();
	NetSrv_ItemChild::Release();
	NetSrv_EQChild::Release();
	NetSrv_RoleValueChild::Release();
	NetSrv_AttackChild::Release();
	NetSrv_TradeChild::Release();
	CNetSrv_Script_Child::Release();
	NetSrv_MagicChild::Release();
	NetSrv_GMToolsChild::Release();
	NetSrv_ShopChild::Release();
	NetSrv_OtherChild::Release();
	NetSrv_PartyChild::Release();
	NetSrv_FriendListChild::Release();
	NetSrv_MailChild::Release();
	NetSrv_ACChild::Release();
	NetSrv_RecipeChild::Release();
	NetSrv_GuildChild::Release();
	CNetSrv_Gather_Child::Release();
	NetSrv_BotChild::Release();
	NetSrv_BillboardChild::Release();
	NetSrv_PKChild::Release();
	NetSrv_LotteryChild::Release();
	NetSrv_HousesChild::Release();
	NetSrv_DepartmentStoreChild::Release();
	NetSrv_ImportBoardChild::Release();
	CNetSrv_BattleGround_Child::Release();
	NetSrv_TeleportChild::Release();
	NetSrv_TreasureChild::Release();
	NetSrv_GuildHousesChild::Release();
	NetSrv_CultivatePetChild::Release();
	//	NetSrv_GuildHousesWarChild::Release();
	NetSrv_BG_GuildWarChild::Release();
	NetSrv_InstanceChild::Release();
	CNetSrv_PublicEncounterChild::Release();
	Net_BillingChild::Release();
	Net_LoginChild::Release();
	//--------------------------------------------------------------------------------------
	GlobalBase::_Release();
	//--------------------------------------------------------------------------------------

	if (_St->Partition.size() != 0)
	{
		for (i = 0; i < (int)_St->Partition.size(); i++)
		{
			delete _St->Partition[i];
		}
		_St->Partition.clear();
	}

	LUA_VMClass::Release();
	LuaDebugger::Release();

	if (_St != NULL)
		delete _St;
	//--------------------------------------------------------------------------------------
	ObjectDataClass::Release();
	FlagPosClass::Release();
	PartyInfoListClass::Release();
	NPCMoveFlagManageClass::Release();
	GuildManageClass::Release();
	HousesManageClass::Release();
	GuildHousesManageClass::Release();

	//ReleaseBattleGround();

	return true;
}
//--------------------------------------------------------------------------------------
bool     Global::Process()
{
	static int		ProcTime = MytimeGetTime();
	static int		LastProcDelayTime = 0;
	static int		LastProcDelayRecorder = 0;
	static int		LastNetProcTime = 0;
	static int		MaxNetProcTime = 0;
	static int		MinNetProcTime = 10000;

	static ProcTimeLogClass	OnTimeProcLog;

	int	NowTime = MytimeGetTime();
	LastProcDelayTime = NowTime - ProcTime;

	_Net->Process();

	//////////////////////////////////////////////////////////////////////////
	if (Global::St()->ServerNetProcLogCount > 0)
	{
		int NetProcTime = NowTime - LastNetProcTime;
		if (NetProcTime > MaxNetProcTime)
			MaxNetProcTime = NetProcTime;
		if (NetProcTime < MinNetProcTime)
			MinNetProcTime = NetProcTime;
	}
	//////////////////////////////////////////////////////////////////////////
	LastNetProcTime = NowTime;

	if (LastProcDelayTime > 100)
	{
		//////////////////////////////////////////////////////////////////////////
		// °Oїэ Server іBІz®Й¶ЎЄє Log
		if (Global::St()->ServerNetProcLogCount > 0)
		{
			--(Global::St()->ServerNetProcLogCount);

			Log_LocalServerProcTime(MinNetProcTime, MaxNetProcTime, LastProcDelayTime, RoleDataEx::G_ZoneID, Global::Net()->GetWorldID());

			MaxNetProcTime = 0;
			MinNetProcTime = 10000;
		}
		//////////////////////////////////////////////////////////////////////////
		ProcTime += 100;
		OnTimeProcLog.Enter();
		_OnTimeProc();
		OnTimeProcLog.Leave();


		if (OnTimeProcLog.LogTime() > 5000)
		{
			PrintToFile(Def_PrintLV2, "OnTimeProcLog", " Min =%d Max=%d Avg=%d Count=%d", OnTimeProcLog.Base().MinProcTime, OnTimeProcLog.Base().MaxProcTime, OnTimeProcLog.Base().AvgProcTime(), OnTimeProcLog.Base().ProcCount);
			OnTimeProcLog.Clear();
		}

	}

	if (NowTime - ProcTime > 1000)
	{
		if (LastProcDelayRecorder < LastProcDelayTime)
		{
			LastProcDelayRecorder = LastProcDelayTime;
			Print(Def_PrintLV2, "Process", "Program Delay %d ", LastProcDelayTime);
		}
		if (LastProcDelayTime > 20000)
		{
			ProcTime = NowTime;
		}
		else
			Global::_St->IsDelay = true;
	}
	else
	{
		LastProcDelayRecorder = 0;
		Global::_St->IsDelay = false;
	}

	return !_St->IsDestroy;
}
//--------------------------------------------------------------------------------------
//Ёъ±o¬Y­УIDЄєЄ«Ґуёк®Ж
BaseItemObject* Global::GetObj(int ID)
{
	return BaseItemObject::GetObj(ID);
}
//Ёъ±o¬Y­УSockIDЄєЄ«Ґуёк®Ж
BaseItemObject* Global::GetObjBySockID(int SockID)
{
	return BaseItemObject::GetObjBySockID(SockID);
}

BaseItemObject* Global::GetObjByName(char* Name)
{
	return BaseItemObject::GetObjByName(Name);
}
/*
bool Global::SendToCli   ( int SockID , int Size , void* Data )
{
return Net.SendToCli( SockID , Size , Data );
}
void Global::SendToAllCli( int Size , void* Data )
{
Net.SendToAllCli( Size , Data );
}
bool Global::SendToGSrvBySockID  ( int SockID , int Size , void* Data )
{
return Net.SendToSrv( SockID , Size , Data );
}
*/
bool Global::SendToCli_ByDBID(int DBID, int Size, void* Data)
{
	if (DBID < 0)
		return false;

	OnlinePlayerInfoStruct* Info = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID(DBID);
	if (Info == NULL)
	{
		//¬dёЯ
		BaseItemObject* Obj = BaseItemObject::GetObjByDBID(DBID);
		if (Obj != NULL)
		{
			return _Net->SendToCli(Obj->SockID(), Obj->Role()->ProxyID(), Size, Data);
		}
		return false;
	}
	return _Net->SendToCli(Info->SockID, Info->ProxyID, Size, Data);
}
bool Global::SendToCli_ByGUID(int GUID, int Size, void* Data)
{
	if (GUID < 0)
		return false;

	BaseItemObject* Obj = BaseItemObject::GetObj(GUID);
	if (Obj == NULL)
	{
		OnlinePlayerInfoStruct* Info = AllOnlinePlayerInfoClass::This()->GetInfo_ByGUID(GUID);
		if (Info == NULL)
			return false;
		return _Net->SendToCli(Info->SockID, Info->ProxyID, Size, Data);
	}
	return _Net->SendToCli(Obj->SockID(), Obj->Role()->ProxyID(), Size, Data);
}


bool Global::SendToCli_ByRoleName(const char* RoleName, int Size, void* Data)
{

	OnlinePlayerInfoStruct* Info = AllOnlinePlayerInfoClass::This()->GetInfo_ByName((char*)RoleName);
	if (Info == NULL)
		return false;

	return _Net->SendToCli(Info->SockID, Info->ProxyID, Size, Data);
}

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
bool Global::AddOnLoginPlayer(PlayerRoleData* PlayerData)
{
	RoleDataEx* Role = (RoleDataEx*)PlayerData;

	if (PlayerData->BaseData.__MoneyCheckPoint != _MEMORY_CHECK_VALUE_
		|| PlayerData->PlayerBaseData.__MoneyCheckPoint != _MEMORY_CHECK_VALUE_
		|| PlayerData->SelfData.__MoneyCheckPoint != _MEMORY_CHECK_VALUE_
		|| PlayerData->PlayerSelfData.__MoneyCheckPoint != _MEMORY_CHECK_VALUE_)
	{
		Print(LV5, "AddOnLoginPlayer", "_MEMORY_CHECK_VALUE_ Error, Account = %s, Role = %s ", Role->Account_ID(), Global::GetRoleName_ASCII(Role).c_str());
		return false;
	}

	_strlwr((char*)Role->Account_ID());

	Print(LV2, "AddOnLoginPlayer", "AddOnLoginPlayer Account = %s, Role = %s", Role->Account_ID(), Global::GetRoleName_ASCII(Role).c_str());
	DelOnLoginPlayer(Role->Account_ID());
	/*

	map<string , PlayerRoleData*>::iterator Iter;
	Iter = _St->OnLoginPlayer.find( Role->Account_ID() );

	if( Iter != _St->OnLoginPlayer.end() )
	{
		DelOnLoginPlayer( Role->Account_ID() );
		//return false;
	}
	*/

	PlayerRoleData* PlayerOwner = _St->OnLoginPlayerRecycle.NewObj();
	if (PlayerOwner == NULL)
	{
		Print(Def_PrintLV5, "AddOnLoginPlayer", "OnLoginPlayerRecycle.NewObj() = NULL , NewPlayerData alloc RoleData Error");
		return false;
	}
	//    Owner->Init();
	//memcpy( Owner , Role , sizeof(RoleDataEx) );
	*PlayerOwner = *PlayerData;
	_St->OnLoginPlayer[Role->Account_ID()] = PlayerOwner;


	//і]©w¤@¬q®Й¶Ў«б°µАЛ¬dЄ±®a¬O§_¦іҐї±`µn¤J
	//30¬н«б¦A¦ё°х¦ж
	PlayerOwner->PlayerTempData.LoginTempID =
		Schedular()->Push(_CheckLoginRoleData, _Def_CheckLoginTime_, NULL,
			"account", EM_ValueType_string, Role->Account_ID(), NULL);

	return true;
}

bool Global::DelOnLoginPlayer(string AccountName)
{
	//_strlwr( (char*)AccountName.c_str() );
	std::transform(AccountName.begin(), AccountName.end(), AccountName.begin(), ::tolower);
	map<string, PlayerRoleData*>::iterator Iter;
	Iter = _St->OnLoginPlayer.find(AccountName);

	//§д¤ЈЁмёк®Ж
	if (Iter == _St->OnLoginPlayer.end())
		return false;

	Print(LV1, "DelOnLoginPlayer", "Account = %s", AccountName.c_str());

	Schedular()->Erase(Iter->second->PlayerTempData.LoginTempID);

	_St->OnLoginPlayerRecycle.DeleteObj(Iter->second);

	_St->OnLoginPlayer.erase(Iter);

	//Iter->second.PlayerTempData->LoginTempID;
	return true;
}

PlayerRoleData* Global::GetOnLoginPlayer(string AccountName)
{
	_strlwr((char*)AccountName.c_str());
	Print(LV1, "GetOnLoginPlayer", "Account = %s", AccountName.c_str());


	map<string, PlayerRoleData*>::iterator Iter;
	Iter = _St->OnLoginPlayer.find(AccountName);

	//§д¤ЈЁмёк®Ж
	if (Iter == _St->OnLoginPlayer.end())
		return NULL;

	return (Iter->second);
}
//---------------------------------------------------------------------------
//АЛ¬dЁ¤¦в¬O§_Ґї±`µn¤JЎA¦pЄGЁS¦і§вЁдёк®ЖДА©с
int Global::_CheckLoginRoleData(SchedularInfo* Obj, void* InputClass)
{
	//Ёъ±o±bё№ёк®Ж
	char* Account = Obj->GetString("account");
	if (Account == NULL)
		return 0;

	//§д¤ЈЁмёк®Ж
	if (DelOnLoginPlayer(Account) == true)
	{
		Print(Def_PrintLV3, "_CheckLoginRoleData", "\n(Warning)Account = %s Not Enter World ЎA Account Not Login (Login Info Exist )!!!", Account);
	}

	return 0;
}
//---------------------------------------------------------------------------
//АЛ¬dЁC¤йҐф°И­«ёm®Й¶Ў, ¶}ѕч®Й·|АЛ¬dВч 06:00 ®t¶Z®Й¶Ў±А¤J, °х¦ж¤@¦ё«б¶·­pєвВч¤U¦ё 06:00 ®t¶Z¬нјЖ±А¤J®Й¶Ў
int Global::_CheckDailyQuestReset(SchedularInfo* Obj, void* InputClass)
{

	__time32_t				ResetTime;
	__time32_t				NowTime;
	struct	tm* gmt;

	_time32(&NowTime);
	gmt = _localtime32(&NowTime);
	NowTime = (int)_mktime32(gmt) + RoleDataEx::G_TimeZone * 60 * 60;

	Print(LV2, "_CheckDailyQuestReset", "_CheckDailyQuestReset Now GMT H[%d] M[%d] S[%d]", gmt->tm_hour, gmt->tm_min, gmt->tm_sec);

	//­pєв«e¤С­«ёm®Й¶ЎЄє¬нјЖ ( ҐШ«e№wі]¬O¦­¤W 06:00 )

	if (gmt->tm_hour >= DF_DAILY_RESET_TIME)
	{
		gmt->tm_hour = DF_DAILY_RESET_TIME;
		gmt->tm_sec = 0;
		gmt->tm_min = 0;
		gmt->tm_mday = gmt->tm_mday;
		ResetTime = (int)_mktime32(gmt) + RoleDataEx::G_TimeZone * 60 * 60 + (24 * 60 * 60);
	}
	else
	{
		gmt->tm_hour = DF_DAILY_RESET_TIME;
		gmt->tm_sec = 0;
		gmt->tm_min = 0;
		ResetTime = (int)_mktime32(gmt) + RoleDataEx::G_TimeZone * 60 * 60;
	}

	int iSec = abs(ResetTime - NowTime);
	//Schedular()->Push( _CheckDailyQuestReset , ( iSec * 1000 ) , NULL , NULL );
	Schedular()->Push(_CheckDailyQuestReset, (iSec * 1000), NULL, NULL);

	Print(LV2, "_CheckDailyQuestReset", "_CheckDailyQuestReset will run after [ %d ]sec", iSec);



	Print(LV2, "ResetDaily", "Begin ResetDailyQuest - reset all online player");

	// №п¤wіsЅuЄєЁC­УЁ¤¦вµoҐX­«ёm«КҐ], ЁГ±N Role ёк®ЖЄєЁC¤йҐф°Иі]©wГцІ¦
	for (int i = 0; i < BaseItemObject::GetmaxZoneID(); i++)
	{
		BaseItemObject* pObj = BaseItemObject::GetObj_ByLocalID(i);

		if (pObj == NULL || pObj->Role()->IsPlayer() == false)
			continue;
		else
		{
			RoleDataEx* pRole = pObj->Role();

			// ­«ёmЁC¤йҐф°И
			CNetSrv_Script_Child::ResetDailyQuest(pRole);

			// ­«ёmЁC¤йєXјР
			CNetSrv_Script_Child::ResetDailyFlag(pRole, true);

			NetSrv_DepartmentStoreChild::SlotMachineFreeCountProc(pRole);
		}
	}

	Print(LV2, "ResetDaily", "End ResetDailyQuest");

	return 0;
}


//---------------------------------------------------------------------------
int Global::CalSecX(float X)
{
	int Ret;
	Ret = int((X - _St->Ini.ViewMinX) / _St->Ini.BlockSize);

	if (Ret >= _St->Ini.PartitionWidth)
		return _St->Ini.PartitionWidth - 1;

	if (Ret < 0)
		return 0;

	return Ret;
}
//---------------------------------------------------------------------------
int Global::CalSecZ(float Z)
{
	int Ret;
	Ret = int((Z - _St->Ini.ViewMinZ) / _St->Ini.BlockSize);

	if (Ret >= _St->Ini.PartitionHeight)
		return _St->Ini.PartitionHeight - 1;

	if (Ret < 0)
		return 0;

	return Ret;
}
//---------------------------------------------------------------------------
int Global::ViewBlockSize()
{
	return _St->Ini.BlockSize;
}
//---------------------------------------------------------------------------
MyMapAreaManagement* Global::Partition(int RoomID)
{
	if (_St->Partition.size() <= (unsigned)RoomID)
		return NULL;

	return _St->Partition[RoomID];

}
//---------------------------------------------------------------------------
MyMapAreaManagement* Global::Partition(RoleDataEx* Owner)
{
	if (_St->Partition.size() <= (unsigned)Owner->SecRoomID())
		return NULL;

	return _St->Partition[Owner->SecRoomID()];

}
//---------------------------------------------------------------------------
PlayIDInfo** Global::PartCompSearch(RoleDataEx* Owner, int NewSecX, int NewSecZ, int BlockCount)
{
	MyMapAreaManagement* Part = Partition(Owner);

	if (Part == NULL)
		return NULL;

	return Part->CompSearch(NewSecX, NewSecZ
		, Owner->SecX()
		, Owner->SecZ()
		, BlockCount
		, BlockCount);

}
//---------------------------------------------------------------------------
bool    Global::PartMove(RoleDataEx* Owner, int NewSecX, int NewSecZ)
{
	if (Owner->SecRoomID() == -1)
		return false;

	MyMapAreaManagement* Part = Partition(Owner);

	Part->Move(NewSecX, NewSecZ, Owner->GUID());
	Owner->SecXZ(NewSecX, NewSecZ);
	return true;
}
//---------------------------------------------------------------------------
bool    Global::PartDel(RoleDataEx* Owner)
{

	MyMapAreaManagement* Part = Partition(Owner);
	if (Part == NULL)
		return false;

	Part->Del(Owner->GUID());

	Owner->SecXZ(-1, -1);
	Owner->SecRoomID(-1);
	return true;
}
//---------------------------------------------------------------------------
bool    Global::PartAdd(RoleDataEx* Owner)
{
	if (Owner->SecRoomID() != -1)
		return false;

	MyMapAreaManagement* Part = Partition(Owner->RoomID());
	if (Part == NULL)
	{
		Print(Def_PrintLV3, "PartAdd", "(Warning) Rolde = %s , Add RoomID =%d Error, Partition == NULL", Global::GetRoleName_ASCII(Owner).c_str(), Owner->RoomID());
		Owner->RoomID(0);
		Part = Partition(0);
	}
	int NewSecX = CalSecX(Owner->X());
	int NewSecZ = CalSecZ(Owner->Z());

	Part->Add(NewSecX, NewSecZ, Owner->GUID());
	Owner->SecXZ(NewSecX, NewSecZ);
	Owner->SecRoomID(Owner->RoomID());
	return true;
}
//---------------------------------------------------------------------------
PlayIDInfo** Global::PartSearch(RoleDataEx* Owner, int BlockCount)
{
	static PlayIDInfo* NullRet[1] = { NULL };
	MyMapAreaManagement* Part = Partition(Owner);
	if (Part == NULL)
		return &NullRet[0];

	return Part->Search(Owner->SecX(), Owner->SecZ(), BlockCount, BlockCount);
}
//---------------------------------------------------------------------------
void     Global::_OnCreateObj(BaseItemObject* OwnerObj, RoleDataEx* Owner)
{

}
//---------------------------------------------------------------------------
void     Global::_OnDestroyObj(BaseItemObject* OwnerObj, RoleDataEx* Owner)
{
	NetSrv_MoveChild::DelFromPartition(OwnerObj);

	if (Owner == NULL)
		return;

	if (OwnerObj->IsMirror())
		return;

	//////////////////////////////////////////////////////////////////////////
	//¦pЄG¤Ј¬OѕФіхServer µnїэ±Ж¦Wёк®Ж
	if (Global::Ini()->IsBattleWorld == false
		&& Owner->IsPlayer()
		&& Owner->PlayerBaseData->ManageLV == EM_Management_Nornal)
	{
		//NetSrv_Billboard::SD_New_Register_Update( Owner->DBID() , EM_BillboardSortType_Money				, Owner->PlayerBaseData->Body.Money );
		//NetSrv_Billboard::SD_New_Register_Update( Owner->DBID() , EM_BillboardSortType_Money_Account		, Owner->PlayerBaseData->Body.Money_Account );
		//NetSrv_Billboard::SD_New_Register_Update( Owner->DBID() , EM_BillboardSortType_Money_Bonus			, Owner->PlayerBaseData->Money_Bonus );
		NetSrv_Billboard::SD_New_Register_Update(Owner->DBID(), EM_BillboardSortType_STR, Owner->TempData.Attr.Fin.STR);
		NetSrv_Billboard::SD_New_Register_Update(Owner->DBID(), EM_BillboardSortType_AGI, Owner->TempData.Attr.Fin.AGI);
		NetSrv_Billboard::SD_New_Register_Update(Owner->DBID(), EM_BillboardSortType_STA, Owner->TempData.Attr.Fin.STA);
		NetSrv_Billboard::SD_New_Register_Update(Owner->DBID(), EM_BillboardSortType_INT, Owner->TempData.Attr.Fin.INT);
		NetSrv_Billboard::SD_New_Register_Update(Owner->DBID(), EM_BillboardSortType_MND, Owner->TempData.Attr.Fin.MND);

		NetSrv_Billboard::SD_New_Register_Update(Owner->DBID(), EM_BillboardSortType_ATK, Owner->TempData.Attr.Fin.ATK);
		NetSrv_Billboard::SD_New_Register_Update(Owner->DBID(), EM_BillboardSortType_MATK, Owner->TempData.Attr.Fin.MATK);
		NetSrv_Billboard::SD_New_Register_Update(Owner->DBID(), EM_BillboardSortType_DEF, Owner->TempData.Attr.Fin.DEF);
		NetSrv_Billboard::SD_New_Register_Update(Owner->DBID(), EM_BillboardSortType_MDEF, Owner->TempData.Attr.Fin.MDEF);
		NetSrv_Billboard::SD_New_Register_Update(Owner->DBID(), EM_BillboardSortType_Honor, Owner->PlayerBaseData->Honor);
		NetSrv_Billboard::SD_New_Register_Update(Owner->DBID(), EM_BillboardSortType_TitleCount, Owner->PlayerBaseData->TitleCount);
		NetSrv_Billboard::SD_New_Register_Update(Owner->DBID(), EM_BillboardSortType_QuestCompleteCount, Owner->PlayerBaseData->Quest.HistoryCount());

	}
	//////////////////////////////////////////////////////////////////////////
	if (RoleDataEx::G_ZoneID == Owner->ZoneID())
	{
		NetSrv_DepartmentStoreChild::This->RC_GetSlotMachineItem(OwnerObj);
	}
	//////////////////////////////////////////////////////////////////////////

	if (Owner->PlayerTempData->LuaClock.Type != EM_LuaPlotClockType_Close
		&& RoleDataEx::G_ZoneID == Owner->ZoneID())
	{
		OwnerObj->PlotVM()->PCall(Owner->PlayerTempData->LuaClock.TimeUpPlot.Begin(), Owner->GUID(), Owner->GUID());
	}

	//////////////////////////////////////////////////////////////////////////
	//¦pЄG¬OёьЁг¤W­±©Т¦і¤H¤UёьЁг
	vector< AttachObjInfoStruct >& AttachList = OwnerObj->AttachList();
	for (unsigned i = 0; i < AttachList.size(); i++)
	{
		if (AttachList[i].IsEmpty())
			continue;
		LuaPlotClass::DetachObj(AttachList[i].ItemID);
	}
	LuaPlotClass::DetachObj(Owner->GUID());

	//////////////////////////////////////////////////////////////////////////
	//---------------------------------------------------------------------------
	// ГdЄ«Іѕ°ЈіqЄѕ
	//---------------------------------------------------------------------------
	BaseItemObject* MasterObj = Global::GetObj(Owner->TempData.Sys.OwnerGUID);
	if (MasterObj != NULL && Owner->ZoneID() == RoleDataEx::G_ZoneID)
	{
		//if( MasterObj->Role()->TempData.Pet.PetID == Owner->GUID() )
		if (Owner->TempData.Sys.IsPet)
		{
			SummonPetTypeENUM	PetType = Owner->TempData.SummonPet.Type;
			NetSrv_Magic::S_DeletePet(MasterObj->GUID(), Owner->GUID(), PetType);

			//MasterObj->Role()->TempData.SummonPet.Info[ PetType ].Init();
			RoleDataEx* Master = MasterObj->Role();
			if (PetType == EM_SummonPetType_CultivatePet)
			{
				for (int i = 0; i < MAX_CultivatePet_Count; i++)
				{
					CultivatePetStruct& PetBase = Master->PlayerBaseData->Pet.Base[i];
					if (PetBase.EventType == EM_CultivatePetCommandType_Summon
						&& Master->TempData.SummonPet.Info[EM_SummonPetType_CultivatePet].PetID == Owner->GUID())
					{
						PetBase.EventType = EM_CultivatePetCommandType_None;
						Master->TempData.UpdateInfo.Recalculate_All = true;

						RoleValueName_ENUM ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * i + EM_CultivatePetStructValueType_EventType);
						Master->Net_FixRoleValue(ValueType, PetBase.EventType);
					}
				}
			}

			MasterObj->Role()->GetPetInfo(Owner)->Init();
		}
	}

	// §R°ЈѕФіхЄ±®aёк°T
	if (Owner->IsPlayer())
	{
		if (Owner->ZoneID() == RoleDataEx::G_ZoneID && Ini()->Int("BattleGround") == 1)
		{
			((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->OnEvent_ClientDisconnect(OwnerObj);
		}
	}



	//---------------------------------------------------------------------------
	//§в№УёO§R°Ј
	if (Owner->IsPlayer())
	{
		BaseItemObject* Obj;
		vector< BaseItemObject* >& ZoneObjList = *BaseItemObject::GetZoneObjList();
		for (unsigned i = 0; i < ZoneObjList.size(); i++)
		{
			Obj = ZoneObjList[i];
			if (Obj == NULL)
				continue;

			if (Obj->Role()->TempData.Sys.OwnerDBID != Owner->DBID())
				continue;

			if (Obj->Role()->BaseData.ItemInfo.OrgObjID == g_ObjectData->SysValue().TombObjID)
			{
				Obj->Destroy("Owner Change Zone or Logout");
			}
		}

	}
	//---------------------------------------------------------------------------
	NetSrv_GMTools::S_DelObjInfo(DF_NO_LOCALID, Owner);
	if (Global::St()->FromNotify != NULL)
		Global::St()->FromNotify->OnDelObj(Owner);

	//    if( !OwnerObj->IsMirror() && _Net != NULL ) 
	if (Owner->IsPlayer())
	{

		if (Ini()->IsHouseZone == true)
		{
			if (Owner->PlayerTempData->VisitHouseDBID != Owner->PlayerBaseData->HouseDBID)
			{
				Owner->BaseData.ZoneID = Owner->SelfData.ReturnZoneID;
				Owner->BaseData.RoomID = 0;
				Owner->TempData.Move.CliX = Owner->BaseData.Pos.X = Owner->SelfData.ReturnPos.X;
				Owner->TempData.Move.CliY = Owner->BaseData.Pos.Y = Owner->SelfData.ReturnPos.Y;
				Owner->TempData.Move.CliZ = Owner->BaseData.Pos.Z = Owner->SelfData.ReturnPos.Z;
			}
		}

		if (Global::Ini()->IsBattleWorld)
		{
			if (Global::Ini()->BG_BG_LeaveScript.size())
				LUA_VMClass::PCall(Global::Ini()->BG_BG_LeaveScript.c_str(), Owner->GUID(), Owner->GUID());
			NetSrv_CliConnect::SBL_RetNomorlWorld(Owner->PlayerTempData->ChangeWorld.WorldID, Owner->PlayerTempData->ChangeWorld.WorldNetID, Owner->DBID(), Owner, OwnerObj->OrgWorldGUID());
		}
		if (Global::Ini()->IsZonePartyEnabled)
		{
			NetSrv_Move::S_AllCli_BattleGround_DelPlayer(Owner->GUID());
			NetSrv_PartyChild::This->RC_Zone_KickMember(OwnerObj, Owner->DBID());
		}

		//Net_DCBase::SetPlayerLiveTime( Owner->Base.DBID , 0 );
		//Net_DCBase::SetPlayerLiveTime_ByAccount( (char*)Owner->Account_ID() , 1000 );
		CNetSrv_ZoneStatus::SetRoleLeaveZone(Owner->DBID(), RoleDataEx::G_ZoneID);
		Global::Log_LeaveZone(Owner);

		if (RoleDataEx::G_ZoneID == Owner->ZoneID())
		{
			SlotMachineInfo& slotMachine = Owner->PlayerBaseData->SlotMachine;
			if (slotMachine.ItemID)
			{
				Owner->GiveItem(slotMachine.ItemID, slotMachine.ItemCount, EM_ActionType_SlotMachine, NULL, "");
				slotMachine.ItemID = 0;
				slotMachine.ItemCount = 0;
			}

			CNetSrv_ZoneStatus::SetRoleLeaveWorld(Owner->DBID(), RoleDataEx::G_ZoneID, EM_RoleLeaveWorld_Disconnect);

			Global::Net()->CliLogout(Owner->TempData.Sys.SockID,
				Net_ServerList->m_ServerType,
				Net_ServerList->m_dwServerLocalID,
				(char*)Owner->Account_ID(),
				Owner->TempData.Sys.DestroyAccount.Begin()
			);

		}

		if (RoleDataEx::G_ZoneID == Owner->ZoneID())
		{
			//іqЄѕ©Т¦і¦Ё­ыВчЅu
			PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo(Owner->PartyID());
			if (Party != NULL)
			{
				for (int i = 0; i < (int)Party->Member.size(); i++)
				{
					NetSrv_Party::S_MemberLogout(Party->Member[i].DBID, Owner->DBID());
				}
			}
		}

		if (Ini()->IsGuildHouseZone == true)
		{
			Owner->BaseData.ZoneID = Owner->SelfData.ReturnZoneID;
			Owner->BaseData.RoomID = 0;
			Owner->TempData.Move.CliX = Owner->BaseData.Pos.X = Owner->SelfData.ReturnPos.X;
			Owner->TempData.Move.CliY = Owner->BaseData.Pos.Y = Owner->SelfData.ReturnPos.Y;
			Owner->TempData.Move.CliZ = Owner->BaseData.Pos.Z = Owner->SelfData.ReturnPos.Z;
		}
		Owner->SaveDB_All();
	}
	else if (Owner->IsFlag())
	{
		if (Owner->DBID() != -1)
		{
			Print(LV2, "Net_DCBase::DropNPCRequest", "LiveTime = %d", Owner->SelfData.LiveTime);
			Net_DCBase::DropNPCRequest(Owner);
		}
		FlagPosClass::DelFlag(OwnerObj);
	}
	else if (Owner->Base.DBID != -1)
	{
		Print(LV2, "Net_DCBase::DropNPCRequest", "LiveTime = %d", Owner->SelfData.LiveTime);
		Net_DCBase::DropNPCRequest(Owner);
	}

	if (Owner->IsPlayer() == false && Owner->GuildID() > 0 && Owner->TempData.GuildBuildingDBID > 0)
	{
		GuildHousesManageClass* houseClass = GuildHousesManageClass::GetHouseObj(Owner->GuildID());
		if (houseClass != NULL)
		{
			switch (Owner->TempData.GuildBuildingType)
			{
			case EM_GuildBuildingType_Building:
				//houseClass->DelBuilding( Owner->TempData.GuildBuildingDBID );
				break;
			case EM_GuildBuildingType_Furniture:
				//houseClass->
				break;
			}

		}
	}

	_Net->CliDisconnect(Owner->TempData.Sys.SockID);
}
//---------------------------------------------------------------------------
bool    Global::IsMirror(int GUID)
{
	GItemData IDInfo;
	IDInfo.GUID = GUID;
	return (IDInfo.Zone != RoleDataEx::G_ZoneID);
}
//-----------------------------------------------------------------------------------------
int     Global::CreateObj(int OrgObjID, float x, float y, float z, float Dir, int Count, int WorldGUID)
{
	GameObjDbStructEx* OrgDB;
	BaseItemObject* OwnerObj;
	RoleDataEx* Owner;

	OrgDB = GetObjDB(OrgObjID);
	if (OrgDB == NULL)
	{
		Print(Def_PrintLV1, "CreateObj", "Obj(%d) Not Find", OrgObjID);
		return -1;
	}


	switch (OrgDB->Classification)
	{
	case EM_ObjectClass_QuestNPC:
	{
		OwnerObj = NEW BaseItemObject(EM_ItemType_NPC, WorldGUID);
		Owner = (RoleDataEx*)OwnerObj->Role();
		if (Owner == NULL)
			return -1;

		Owner->SetInitNPC(OrgDB);

		QuestObjRegProc(OwnerObj);

		OwnerObj->AI(NEW NpcAIBaseClass(OwnerObj));
		Owner->TempData.AI.CampID = OrgDB->NPC.CampID;

		break;
	}
	case EM_ObjectClass_NPC:
	{
		OwnerObj = NEW BaseItemObject(EM_ItemType_NPC, WorldGUID);
		Owner = (RoleDataEx*)OwnerObj->Role();
		if (Owner == NULL)
			return -1;

		Owner->SetInitNPC(OrgDB);

		NPCRegProc(OwnerObj);
		OwnerObj->AI(NEW NpcAIBaseClass(OwnerObj));
		Owner->TempData.AI.CampID = OrgDB->NPC.CampID;
	}
	break;
	case EM_ObjectClass_Item:
	{
		OwnerObj = NEW BaseItemObject(EM_ItemType_Item, WorldGUID);
		Owner = (RoleDataEx*)OwnerObj->Role();
		if (Owner == NULL)
			return -1;

		Owner->SetInitItem(OrgDB);
		Owner->TempData.AI.CampID = EM_CampID_Unknow;
		//Owner->LiveTime( 1000 * 3 * 60 , "SYS CreateObjІЈҐНЄ««~" );
		Owner->TempData.AI.CampID = EM_CampID_Visitor;
	}
	break;
	case EM_ObjectClass_Flag:
	{
		OwnerObj = NEW BaseItemObject(EM_ItemType_Item, WorldGUID);
		Owner = (RoleDataEx*)OwnerObj->Role();
		if (Owner == NULL)
			return -1;

		Owner->SetInitItem(OrgDB);
		Owner->BaseData.Mode.Revive = 1;
		Owner->TempData.AI.CampID = EM_CampID_Visitor;
	}
	break;
	case EM_ObjectClass_Mine:
	{
		OwnerObj = NEW BaseItemObject(EM_ItemType_Item, WorldGUID);
		Owner = (RoleDataEx*)OwnerObj->Role();
		if (Owner == NULL)
			return -1;

		// Ґ[¤JЄм©l¶q, ЁГЁМ°Iґо¶q¶ГјЖЅХѕг
		CNetSrv_Gather_Child::SetBaseCount(Owner, int(OrgDB->Mine.BaseCount), OrgDB->Mine.iReduceVal);

		// АЛ¬dёк®Ж®w, ёУДqЄєГю«¬, ¦pЄG¬°«х§№¤Ј·|®шҐўЄєДq, ¤~¦і«мґ_ЄєЇа¤O
		if (OrgDB->Mine.CreateType == 0 && OrgDB->MaxWorldCount != 0)	// ¦іі]©w¤W­­Єє¤~ҐNЄн¦іІЈ¶q
		{
			MineRegProc(OwnerObj);
		}

		Owner->SetInitItem(OrgDB);						// µ№¤©Єм©l­И
		Owner->TempData.AI.CampID = EM_CampID_Visitor;
	}
	break;

	default:
		return -1;
	}
	Owner->BaseData.Mode = OrgDB->GetModeFlag();
	Owner->Base.Type = OrgDB->Classification;

	Owner->BaseData.Mode.Revive = false;

	//------------------------------------------------------------------------------
	//і]°тҐ»ёк®Ж
	//------------------------------------------------------------------------------
	Owner->BaseData.ItemInfo.Init();
	Owner->BaseData.ItemInfo.OrgObjID = OrgObjID;
	Owner->BaseData.ItemInfo.Count = Count;
	Owner->BaseData.ZoneID = RoleDataEx::G_ZoneID;
	Owner->Base.CreateTime = RoleDataEx::G_NowStr;
	Owner->TempData.BoundRadiusX = 0;
	Owner->TempData.BoundRadiusY = 0;

	GameObjDbStructEx* ImageDB = GetObjDB(OrgDB->ImageID);
	if (ImageDB != NULL)
	{
		Owner->TempData.BoundRadiusX = float(ImageDB->Image.BoundRadiusX);
		Owner->TempData.BoundRadiusY = float(ImageDB->Image.BoundRadiusY);
	}
	if (Owner->TempData.BoundRadiusX < 5)
		Owner->TempData.BoundRadiusX = 5;
	if (Owner->TempData.BoundRadiusY < 5)
		Owner->TempData.BoundRadiusY = 10;
	//------------------------------------------------------------------------------
	//----------------------------------------------------------------
	//јИ®Й

	Owner->RoomID(0);
	//----------------------------------------------------------------
	Owner->Pos(x, y, z, Dir);
	Owner->SelfData.RevPos = Owner->BaseData.Pos;
	//Owner->RoleName( OrgDB->Name );
	Owner->RoleName("");
	Owner->InitCal();

	return Owner->GUID();
}
//---------------------------------------------------------------------------
bool	Global::CheckClientLoading_AddToPartition(int GItemID, int RoomID)
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObj(GItemID);

	if (OwnerObj == NULL)
	{
		Print(Def_PrintLV3, "CheckClientLoading_AddToPartition", "Owener(%d)=NULL", GItemID);
		return false;
	}
	RoleDataEx* Owner = OwnerObj->Role();

	float Dx = Owner->Pos()->X - Owner->TempData.BackInfo.LX;
	float Dz = Owner->Pos()->Z - Owner->TempData.BackInfo.LZ;

	Owner->TempData.BackInfo.LX = Owner->Pos()->X;
	Owner->TempData.BackInfo.LZ = Owner->Pos()->Z;

	if (Owner->IsPlayer())
	{
		const float MinNeedLoadingMapRange = 480.0f;

		GameObjDbStructEx* LZoneObj = GetObjDB((Owner->TempData.BackInfo.LZoneID % _DEF_ZONE_BASE_COUNT_) + Def_ObjectClass_Zone);
		GameObjDbStructEx* ZoneObj = GetObjDB((RoleDataEx::G_ZoneID % _DEF_ZONE_BASE_COUNT_) + Def_ObjectClass_Zone);
		//АЛ¬d¬O§_­nµҐ«ЭClient Loading Map ёк®Ж
		if (LZoneObj == NULL
			|| ZoneObj == NULL
			|| stricmp(LZoneObj->Zone.MapFile, ZoneObj->Zone.MapFile) != 0
			|| Dx * Dx + Dz * Dz > MinNeedLoadingMapRange * MinNeedLoadingMapRange)
		{
			NetSrv_Move::S_ClientLoading(Owner->GUID(), Owner->Pos()->X, Owner->Pos()->Y, Owner->Pos()->Z);
			return false;
		}
	}

	return AddToPartition(GItemID, RoomID);
}
//---------------------------------------------------------------------------
bool    Global::AddToPartition(int GItemID, int RoomID)
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObj(GItemID);

	if (OwnerObj == NULL)
	{
		Print(Def_PrintLV3, "AddToPartition", "Owener(%d)=NULL", GItemID);
		return false;
	}

	RoleDataEx* Owner = OwnerObj->Role();
	if (RoomID != -1)
		Owner->RoomID(RoomID);
	else
		RoomID = Owner->RoomID();

	if (Owner->TempData.Sys.OnChangeZone == true)
		return false;

	if (RoomID == -1)
		RoomID = 0;


	if (RoomID < 0 || RoomID >= Global::Ini()->RoomCount)
	{
		Print(Def_PrintLV3, "AddToPartition", "RoomID=%d Error", RoomID);
		return false;
	}

	if (Owner->BaseData.Pos.X == 0 && Owner->BaseData.Pos.Y == 0 && Owner->BaseData.Pos.Z == 0)
	{
		Print(LV3, "Global", "AddToPartition Obj[ %d ] at position zero", GItemID);
	}

	OwnerObj->Path()->RoomID(RoomID);

	bool Ret = NetSrv_MoveChild::AddToPartition(OwnerObj);
	if (Ret == true)
	{
		NetSrv_GMTools::S_ObjInfo(DF_NO_LOCALID, Owner);
		if (Global::St()->FromNotify != NULL)
			Global::St()->FromNotify->OnAddObj(Owner);

		Owner->TempData.Move.IsLockSetPos = true;
		Owner->TempData.Move.LockSetPosX = Owner->X();
		Owner->TempData.Move.LockSetPosZ = Owner->Z();

		Owner->TempData.Move.RetX = Owner->X();
		Owner->TempData.Move.RetY = Owner->Y();
		Owner->TempData.Move.RetZ = Owner->Z();

		Owner->TempData.Move.LOK_CliX = Owner->X();
		Owner->TempData.Move.LOK_CliY = Owner->Y();
		Owner->TempData.Move.LOK_CliZ = Owner->Z();

		if (Owner->BaseData.SysFlag.IsNeedSetPosition == true)
		{
			Owner->BaseData.SysFlag.IsNeedSetPosition = false;
			Global::SetPos(Owner->GUID());
		}
	}

	return Ret;;
}
//---------------------------------------------------------------------------
bool    Global::DelFromPartition(int GItemID)
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObj(GItemID);

	if (OwnerObj == NULL)
	{
		Print(Def_PrintLV3, "DelFromPartition", "Owener(%d)=NULL", GItemID);
		return false;
	}

	if (OwnerObj->Role()->TempData.Sys.SecX == -1 && OwnerObj->Role()->TempData.Sys.SecZ == -1)
		return false;

	return NetSrv_MoveChild::DelFromPartition(OwnerObj);
}
//---------------------------------------------------------------------------
GameObjDbStructEx* Global::GetObjDB(int OrgID)
{
	if (OrgID == 0)
		return NULL;
	return g_ObjectData->GetObj(OrgID);
	//return _St->ObjectDB.GetObject( OrgID );
}
RoleDataEx* Global::GetRoleDataByGUID(int ID)
{
	BaseItemObject* Obj = GetObj(ID);
	if (Obj == NULL)
		return NULL;

	return Obj->Role();
}

RoleDataEx* Global::GetRoleDataByDBID(int DBID)
{
	BaseItemObject* Obj = GetPlayerObj_ByDBID(DBID);
	if (Obj == NULL)
		return NULL;

	return Obj->Role();
}
//---------------------------------------------------------------------------------------------
//©Т¦іЄ±®aЎAЄ««~ЎAЕг№іЄ«ҐуіЈ·|©w®ЙДІµo
void     Global::_Obj_OnTimeProc(BaseItemObject* OwnerObj, RoleDataEx* Owner)
{
	Owner->OnTimeProc();
	NetSrv_MoveChild::SendRange_ActionType(Owner);

	RoleBackupInfo& BackInfo = Owner->TempData.BackInfo;

	//	if( Owner->SecRoomID() != -1 )
	{
		if (BackInfo.Mode._Mode != Owner->BaseData.Mode._Mode)
		{
			NetSrv_RoleValueChild::SendRangeObjMode(Owner);
		}

		//АЛ¬dhp mpЎ@spЎ@јЖ­И¬O§_¦і§у·s
		if (BackInfo.MaxHP != (int)Owner->TempData.Attr.Fin.MaxHP
			|| BackInfo.MaxMP != (int)Owner->TempData.Attr.Fin.MaxMP
			|| BackInfo.MaxSP != (int)Owner->TempData.Attr.Fin.MaxSP
			|| BackInfo.MaxSP_Sub != (int)Owner->TempData.Attr.Fin.MaxSP_Sub)
		{
			NetSrv_AttackChild::SendRangeMaxHPMPInfo(Owner);
			NetSrv_AttackChild::SendPartyMaxHPMaxMP(Owner);
		}

		if (BackInfo.HP != int(Owner->BaseData.HP)
			|| BackInfo.MP != int(Owner->BaseData.MP)
			|| BackInfo.SP != int(Owner->BaseData.SP)
			|| BackInfo.SP_Sub != int(Owner->BaseData.SP_Sub))
		{
			/*
			if( BackInfo.HP != int( Owner->BaseData.HP ) )
				Net_FixRoleValue( EM_RoleValue_HP , Owner->BaseData.HP );
			if( BackInfo.MP != int( Owner->BaseData.MP ) )
				Net_FixRoleValue( EM_RoleValue_MP , Owner->BaseData.MP );
			if( BackInfo.SP != int( Owner->BaseData.SP ) )
				Net_FixRoleValue( EM_RoleValue_SP , Owner->BaseData.SP );
			if( BackInfo.SP_Sub != int( Owner->BaseData.SP_Sub ) )
				Net_FixRoleValue( EM_RoleValue_SP_Sub , Owner->BaseData.SP_Sub );
				*/

			NetSrv_AttackChild::SendRangeHPInfo(Owner);
			NetSrv_AttackChild::SendPartyHPMP(Owner);
		}

	}

	if (int(Owner->TempData.Attr.Fin.MoveSpeed) != BackInfo.MoveSpeed)
	{
		NetSrv_MoveChild::SendRange_MoveSpeed(Owner);
	}

	if (Owner->TempData.BackInfo.JumpRate != Owner->TempData.Attr.Mid.Body[EM_WearEqType_JumpRate]
		|| Owner->TempData.BackInfo.Gravity != Owner->TempData.Attr.Mid.Body[EM_WearEqType_Gravity])
	{
		NetSrv_MoveChild::SendRange_MoveInfo(Owner);
	}

	/*
		//©w®ЙАЛ¬d»PҐАЄ«ҐуЄє¶ZВч
		if( Owner->TempData.Sys.OwnerGUID != -1 && Owner->ClockMod( 60 ) && Owner->BaseData.ItemInfo.OrgObjID != g_ObjectData->SysValue().TombObjID )
		{
			RoleDataEx* Master = Global::GetRoleDataByGUID( Owner->TempData.Sys.OwnerGUID );
			if( Master == NULL || Master->DBID() != Owner->TempData.Sys.OwnerDBID || Master->Length( Owner ) > 800 )
			{
				Owner->Destroy( "_Obj_OnTimeProc" );
			}
		}
		*/

	if (BackInfo.GuildID != Owner->BaseData.GuildID)
	{
		NetSrv_MoveChild::SendRangeSetGuildID(Owner);
		Owner->TempData.UpdateInfo.AllZoneInfoChange = 1;
	}

	if (BackInfo.TitleID != Owner->BaseData.TitleID || BackInfo.IsShowTitle != Owner->BaseData.IsShowTitle)
	{
		NetSrv_MoveChild::SendRangeSetTitleID(Owner);
		Owner->TempData.UpdateInfo.AllZoneInfoChange = 1;
	}

	//¬d·T«лЄн¤є©Т¦іЄ±®aЎA¬O§_¦і«л¤wёg¤Ј¦s¦bЄєЄ«Ґу
	if (Owner->ClockMod(50))
	{
		for (int i = 0; i < Owner->TempData.NPCHate.Hate.Size(); i++)
		{
			NPCHateStruct& Hate = Owner->TempData.NPCHate.Hate[i];
			RoleDataEx* HateObj = Global::GetRoleDataByGUID(Hate.ItemID);
			if (HateObj == NULL || HateObj->IsDead() || HateObj->SecRoomID() == -1 || HateObj->DBID() != Hate.DBID)
			{
				Owner->TempData.NPCHate.Hate.Erase(i);
				i--;
				continue;
			}
		}
	}
}
//---------------------------------------------------------------------------------------------
void Global::ZoneDamageProc(RoleDataEx* Owner)
{
	if (Owner->TempData.ZoneType == EM_ZoneType_NORMAL)
		return;

	if (Owner->IsDead())
	{
		Owner->TempData.ZoneType = EM_ZoneType_NORMAL;
		return;
	}

	if (Owner->TempData.Attr.Effect.ExchangeZoneDamageEvent == false)
	{
		float Damage = Owner->TempData.Attr.Fin.MaxHP * 0.1f * Owner->TempData.ZoneDamgeRate;
		Owner->AddHP(Owner, Damage * -1);
		NetSrv_MoveChild::S_PlayerDropDownDamage(Owner->GUID(), (int)Damage, 0);
	}
	else
	{
		for (int i = 0; i < Owner->BaseData.Buff.Size(); i++)
		{
			BuffBaseStruct& Buff = Owner->BaseData.Buff[i];
			GameObjDbStructEx* Magic = (GameObjDbStructEx*)Buff.Magic;

			if (Magic == NULL)
				continue;

			if (Magic->MagicBase.Effect.ExchangeZoneDamageEvent == false)
				continue;

			if (Magic->MagicBase.SpecialMagicEventType == EM_SpecailMagicEventType_ZoneDamage)
			{
				MagicProcInfo  MagicInfo;
				MagicInfo.Init();
				MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
				MagicInfo.TargetID = Owner->GUID();				//ҐШјР
				MagicInfo.MagicCollectID = Magic->MagicBase.OnSpecialMagicID;			//ЄkіN
				MagicInfo.MagicLv = Buff.Power;
				MagicProcessClass::SysSpellMagic(Owner, &MagicInfo);
			}

		}
	}
}
//---------------------------------------------------------------------------------------------
//Ґu¦іЄ±®a©w®Й·|ДІµo
void     Global::_Player_OnTimeProc(BaseItemObject* OwnerObj, RoleDataEx* Owner)
{
	//////////////////////////////////////////////////////////////////////////
	if (Owner->ClockMod(5))
	{
		Owner->PlayerTempData->ChangeZoneCountDown -= 5;
		if (Owner->PlayerTempData->ChangeZoneCountDown < 0)
			Owner->PlayerTempData->ChangeZoneCountDown = 0;
	}
	//////////////////////////////////////////////////////////////////////////

	if (Owner->TempData.Sys.OnChangeZone == true
		|| Owner->TempData.Sys.OnChangeWorld == true)
		return;

	Owner->OnTimeProc_Player();

	if (Owner->PlayerTempData->LuaClock.Type == EM_LuaPlotClockType_Open)
	{
		bool  IsStop = false;
		LuaPlotClockStruct& LuaClock = Owner->PlayerTempData->LuaClock;
		if (LuaClock.BeginTime > LuaClock.EndTime)
		{
			LuaClock.NowTime--;
			if (LuaClock.NowTime <= LuaClock.EndTime)
				IsStop = true;
		}
		else
		{
			LuaClock.NowTime++;
			if (LuaClock.NowTime >= LuaClock.EndTime)
				IsStop = true;
		}

		if (IsStop == true)
		{
			LuaClock.Type = EM_LuaPlotClockType_Stop;
			LuaClock.NowTime = LuaClock.EndTime;
			OwnerObj->PlotVM()->PCall(LuaClock.TimeUpPlot.Begin(), Owner->GUID(), Owner->GUID());
		}
	}

	if (Owner->IsDead())
	{
		//ѕЯ¬d­«ҐН­ЛјЖ®Й¶Ў¬O§_Ём¤F
		if (Owner->PlayerBaseData->DeadCountDown == 0)
		{
			float DecRate = 0;
			if (Owner->TempData.Attr.Effect.ExpProtect == false)
			{
				DecRate = float((100 - Owner->PlayerBaseData->RaiseInfo.ExpRate) / 100.0f);
			}

			if (Global::St()->Ini.ReviveScript.size() != 0)
			{
				Owner->Dead(false);
				//ІM°ЈBuff
				for (int i = 0; i < Owner->BaseData.Buff.Size(); i++)
				{
					GameObjDbStructEx* Magic = (GameObjDbStructEx*)Owner->BaseData.Buff[i].Magic;
					if (!Magic->IsMagicBase() || Magic->MagicBase.Setting.DeadNotClear == false)
					{
						//Owner->BaseData.Buff.Erase( i );
						Owner->ClearBuffByPos(i);
						i--;
					}
				}
				//				NetSrv_AttackChild::SendRangeResurrection( Owner );


				OwnerObj->PlotVM()->PCall(Global::St()->Ini.ReviveScript, Owner->GUID(), Owner->GUID());

				//Owner->ReCalculateMagicAndEQ( );
				Owner->ReCalculateEQNoBuff();
				Owner->ReCalculateBuff();
				Owner->Calculate();

				//јЖ­И­«і]
				Owner->BaseData.HP = Owner->TempData.Attr.Fin.MaxHP;
				Owner->BaseData.MP = Owner->TempData.Attr.Fin.MaxMP;
				Owner->BaseData.SP = 0;
				Owner->BaseData.SP_Sub = 0;
				if (Owner->ZoneID() == RoleDataEx::G_ZoneID)
					NetSrv_MoveChild::SetPos(Owner->GUID(), Owner);
				NetSrv_AttackChild::SendRangeResurrection(Owner);
				ChangeZone(Owner->GUID(), Owner->ZoneID(), -1, Owner->X(), Owner->Y(), Owner->Z(), float(rand() % 360));
			}
			else if (Global::St()->CampReviveList.size() > (unsigned)Owner->TempData.AI.CampID)
			{
				CampReviveStruct& CampRevive = Global::St()->CampReviveList[Owner->TempData.AI.CampID];
				PlayerResuretion(OwnerObj, CampRevive.ZoneID, CampRevive.X, CampRevive.Y, CampRevive.Z, DecRate);
				OwnerObj->PlotVM()->CallLuaFunc(CampRevive.LuaScript.c_str(), Owner->GUID(), 0);
			}
			else if (Ini()->IsGuildHouseWarZone == true)
			{
				GuildHousesManageClass* guildhouse = GuildHousesManageClass::GetHouseObj(Owner->GuildID());
				if (guildhouse == NULL)
				{
					PlayerResuretion(OwnerObj, Owner->SelfData.RevZoneID, Owner->SelfData.RevPos.X, Owner->SelfData.RevPos.Y, Owner->SelfData.RevPos.Z, 0);
				}
				else
				{
					float X, Y, Z, Dir;
					guildhouse->GetPlayerPostion(X, Y, Z, Dir);
					PlayerResuretion(OwnerObj, RoleDataEx::G_ZoneID, X, Y, Z, 0);
				}
			}
			//Ёъ±o­«ҐН¦мёm
			else if (Ini()->EnabledRevPoint == false)
			{
				POINT3D RevPoint;
				if (OwnerObj->Path()->FindZoneRespawnPoint(Owner->Pos()->X, Owner->Pos()->Y, Owner->Pos()->Z, RevPoint) == true)
					PlayerResuretion(OwnerObj, Owner->G_ZoneID, RevPoint.x, RevPoint.y, RevPoint.z, DecRate);
				else
					PlayerResuretion(OwnerObj, Owner->SelfData.RevZoneID, Owner->SelfData.RevPos.X, Owner->SelfData.RevPos.Y, Owner->SelfData.RevPos.Z, DecRate);
			}
			else
			{
				PlayerResuretion(OwnerObj, Ini()->RevZoneID, float(Ini()->RevX), float(Ini()->RevY), float(Ini()->RevZ), DecRate);
			}
		}
		else
			Owner->PlayerBaseData->DeadCountDown--;

	}


	if (Owner->ClockMod(600))
	{
		Owner->SaveDB_All();
		Net_DCBase::SetPlayerLiveTime(GetPCenterID(Owner->Account_ID()), Owner->Base.DBID, 60 * 60 * 1000);
		//АЛ¬dЁГІM°ЈЅЖ»s
		Owner->CheckAndClearCopyItem();

		PartyBuffProc(Owner);

		TimeFlagProc(Owner);

		for (int i = 0; i < EM_ClientPacketCheckType_Max; i++)
		{
			if (Owner->PlayerTempData->PacketCheckPoint[i] <= 0)
				continue;
			Owner->PlayerTempData->PacketCheckPoint[i] -= 1.0f;
			if (Owner->PlayerTempData->PacketCheckPoint[i] < 0)
				Owner->PlayerTempData->PacketCheckPoint[i] = 0;
		}

		Owner->PlayerBaseData->Defendant.MoneyTraderCount -= (1.0f / 60);
		Owner->PlayerBaseData->Defendant.AutoBotCount -= (1.0f / (60 * 8));

		if (Owner->PlayerBaseData->Defendant.MoneyTraderCount < 0)
			Owner->PlayerBaseData->Defendant.MoneyTraderCount = 0;
		if (Owner->PlayerBaseData->Defendant.AutoBotCount < 0)
			Owner->PlayerBaseData->Defendant.AutoBotCount = 0;
		/*
				//¬dёЯVip·|­ы®Й¶Ў¬O§_¤wЁмґБ
				if(		Owner->PlayerBaseData.VipMember.Term != 0
					&&	Owner->PlayerBaseData.VipMember.Term < RoleDataEx::G_SysTime )
				{
					Owner->PlayerBaseData.VipMember.Term = 0;
					Owner->PlayerBaseData.VipMember.Type = 0;

					LUA_VMClass::PCallByStrArg( "Event_Model_VIP_Timeup()" , Owner->GUID() , Owner->GUID() );
				}
				*/

		Owner->AddValue(EM_RoleValue_TitleSys_Famous, g_ObjectData->SysValue().TitleSys_FamouseDesc_Min * -1);
	}

	if (Ini()->IsClientLog == true && Owner->ClockMod(3000))
	{
		NetSrv_RoleValue::S_ClientComputerInfoRequest(Owner->GUID());
	}

	if (!Owner->IsDead() && Owner->ClockMod(20) && Owner->TempData.Attr.Effect.StopOntimeCure == false)
	{
		if (Owner->ClockMod(120))
			Owner->AddStomachPoint(1);


		//¤K¬ніBІz¤@¦ё і]©w¬O§_БЩ¦b§рА»јТ
		if (Owner->MaxHP() != int(Owner->HP())
			|| Owner->MaxMP() != int(Owner->MP()))
		{
			if (Owner->IsAttackMode())
			{
				AddMP(Owner, Owner, Owner->TempData.Attr.Fin.ReMP / 24);
			}
			else
			{
				if (Owner->IsSit())
				{
					AddHP(Owner, NULL, Owner->TempData.Attr.Fin.ReHP * 0.66f);
					AddMP(Owner, NULL, Owner->TempData.Attr.Fin.ReMP * 0.66f);
				}
				else
				{
					AddHP(Owner, NULL, Owner->TempData.Attr.Fin.ReHP / 3);
					AddMP(Owner, NULL, Owner->TempData.Attr.Fin.ReMP / 3);
				}
			}

		}

		if (Owner->IsAttackMode())
		{

			if (Owner->IsMove())
			{
				AddSP(Owner, NULL, (Owner->TempData.Attr.Arg.ReSPAttackTime_Move * Owner->TempData.Attr.Fin.MainJobSPRate) / 3);
				AddSP_Sub(Owner, NULL, (Owner->TempData.Attr.Arg_Sub.ReSPAttackTime_Move * Owner->TempData.Attr.Fin.SubJobSPRate) / 3);
			}
			else
			{
				AddSP(Owner, NULL, (Owner->TempData.Attr.Arg.ReSPAttackTime_Stop * Owner->TempData.Attr.Fin.MainJobSPRate) / 3);
				AddSP_Sub(Owner, NULL, (Owner->TempData.Attr.Arg_Sub.ReSPAttackTime_Stop * Owner->TempData.Attr.Fin.SubJobSPRate) / 3);
			}
		}
		else
		{
			if (Owner->IsMove())
			{
				//				Owner->IsSit( false );
				AddSP(Owner, NULL, (Owner->TempData.Attr.Arg.ReSPNormal_Move * Owner->TempData.Attr.Fin.MainJobSPRate) / 3);
				AddSP_Sub(Owner, NULL, (Owner->TempData.Attr.Arg_Sub.ReSPNormal_Move * Owner->TempData.Attr.Fin.SubJobSPRate) / 3);
			}
			else
			{
				AddSP(Owner, NULL, (Owner->TempData.Attr.Arg.ReSPNormal_Stop * Owner->TempData.Attr.Fin.MainJobSPRate) / 3);
				AddSP_Sub(Owner, NULL, (Owner->TempData.Attr.Arg_Sub.ReSPNormal_Stop * Owner->TempData.Attr.Fin.SubJobSPRate) / 3);
			}
			Owner->AddValue(EM_RoleValue_SoulPoint, -1, NULL);
		}
	}

	if (Owner->ClockMod(80))
	{
		/*
		//¤K¬ніBІz¤@¦ё і]©w¬O§_БЩ¦b§рА»јТ
		if( Owner->IsAttackMode() && Owner->IsAttack() == false )
		{
			Owner->FixAttackMode();
		}
		*/

		//іqЄѕ¦Ё­ыBuffёк®Ж
		NetSrv_Magic::S_AllMember_PartyMemberBuff(Owner);
	}

	if (Owner->ClockMod(150))
	{
		//15¬ніBІz¤@¦ё Ё­¤W©Т¦іЄ««~¬O§_­n®шҐў
		Owner->CheckItemLiveTime();


		//АЛ¬dЁ¤¦вІѕ°КЄ¬єA»P®Й¶Ў
		if (Owner->TempData.Move.MoveType == EM_ClientMoveType_Jump
			&& RoleDataEx::G_SysTime - Owner->TempData.Move.BeginJumpTime > 30 * 1000
			&& Owner->IsDead() == false)
		{
			Owner->Net_GameMsgEvent(EM_GameMessageEvent_ClientDataError);
			Print(LV3, "Cheat Mode ??", " Jump time =%d RoleDBID=%d", RoleDataEx::G_SysTime - Owner->TempData.Move.BeginJumpTime, Owner->DBID());
			Owner->AddHP(Owner, -1 * Owner->MaxHP() - 1000);
		}
		if (Owner->TempData.Move.MoveType == EM_ClientMoveType_Normal
			&& Owner->TempData.CliState.InWater == false
			&& Owner->TempData.Attr.Effect.Fly == false
			&& Owner->TempData.Attr.Effect.WaterWalk == false)
		{
			if (Ini()->IsCheckHeight && Owner->TempData.Attr.Action.SrvSetPos == false)
			{

				if (OwnerObj->Path()->Height(Owner->Pos()->X, Owner->Pos()->Y, Owner->Pos()->Z, Owner->Pos()->Y, 800) == false)
				{
					Owner->Net_GameMsgEvent(EM_GameMessageEvent_ClientDataError);
					Print(LV3, "Cheat Mode ??", "Height Error RoleDBID=%d X=%f,Y=%f,Z=%f", Owner->DBID(), Owner->Pos()->X, Owner->Pos()->Y, Owner->Pos()->Z);
					Owner->AddHP(Owner, Owner->MaxHP() * -1 - 1000);
				};
			}

		}

	}

	if (Owner->ClockMod(5))
	{
		//ЁC0.5¬нАЛ¬d¤@¦ё
		if (Owner->PlayerTempData->CastingType != EM_CastingType_NULL && Owner->PlayerTempData->CastingType < EM_CastingType_OKRESULT)
		{
			if (unsigned(Owner->PlayerTempData->CastingTime) < RoleDataEx::G_SysTime)
			{
				NetSrv_Other::S_CastingOK(Owner->GUID(), Owner->PlayerTempData->CastingType);
				Owner->PlayerTempData->CastingType = EM_CastingType_OKRESULT;
			}
		}
	}

	if (Owner->ClockMod(10))
	{
		if (Owner->IsAttackMode() && Owner->IsAttack() == false)
		{

			Owner->TempData.PlayerClsHateColdown--;
			if (Owner->TempData.PlayerClsHateColdown == 0)
				Owner->ClearHate();

		}

		_MagicToolsProc(Owner);

		if (Owner->TempData.AI.CampID == EM_CampID_NewHand)
		{
			GoodEvilProc(Owner, 0);
		}


		if (Owner->TempData.DeadZoneCountDown != 0)
		{
			Owner->TempData.DeadZoneCountDown--;
			if (Owner->TempData.DeadZoneCountDown == 0)
			{
				RoleDataEx* DropRole = Owner;
				BaseItemObject* WagonObj = Global::GetObj(Owner->TempData.AttachObjID);
				if (WagonObj != NULL)
				{
					//©Т¦іЁ®¤WЄєЄ«ҐуіЈ¦є
					vector< AttachObjInfoStruct >& AttachList = WagonObj->AttachList();
					for (unsigned i = 0; i < AttachList.size(); i++)
					{
						BaseItemObject* OtherObj = Global::GetObj(AttachList[i].ItemID);
						if (OtherObj == NULL)
							continue;
						RoleDataEx* Other = OtherObj->Role();

						Print(LV3, "DeadZoneCountDown", "DeadZoneCountDown == 0  DBID = %d", Other->DBID());
						Other->AddHP(Other, -1 * Other->MaxHP() - 1000);
					}
					WagonObj->Role()->AddHP(WagonObj->Role(), -1 * WagonObj->Role()->MaxHP() - 1000);

				}
				else
				{
					Print(LV3, "DeadZoneCountDown", "DeadZoneCountDown == 0  DBID = %d", Owner->DBID());
					Owner->AddHP(Owner, -1 * Owner->MaxHP() - 1000);
				}
			}
		}

		ZoneDamageProc(Owner);
		/*
		if(		Owner->TempData.ZoneType != EM_ZoneType_NORMAL
			&& !Owner->IsDead() )
		{
			float Damage = Owner->TempData.Attr.Fin.MaxHP * 0.1f * Owner->TempData.ZoneDamgeRate;
			Owner->AddHP( Owner , Damage * -1 );
			NetSrv_MoveChild::S_PlayerDropDownDamage( Owner->GUID () , (int)Damage , 0 );
		}
		*/

		ProcPetEvent(Owner);

	}

	if (Owner->ClockMod(DF_TIME_PING * 10))
	{
		Owner->TempData.PingCount--;
		if (Owner->TempData.PingCount < 0)
			Owner->TempData.PingCount = 0;
	}
	//////////////////////////////////////////////////////////////////////////

	//ЁC20¤АДБ·|®ш°Ј1ВI­t­И ЁC¤АБй®ш°Ј0.05ВI­t­И
	if (Owner->ClockMod(600) && Global::Ini()->DisableGoodEvil == false && RoleDataEx::G_PKType == EM_PK_Normal)
	{
		if (Owner->PlayerBaseData->GoodEvil < 0)
		{
			Owner->PlayerBaseData->GoodEvilBonus += 0.1;
			GoodEvilProc(Owner, 0.05f);
		}
		else
		{
			Owner->PlayerBaseData->GoodEvilBonus += 0.05;
		}

		if (Owner->PlayerBaseData->GoodEvilBonus > 10)
			Owner->PlayerBaseData->GoodEvilBonus = 10;
	}
	//////////////////////////////////////////////////////////////////////////
/*	if( Global::Ini()->IsZonePartyEnabled == true && Owner->ClockMod( 50 ) == true )
	{
		PlayerBackupInfoStruct& Info = Owner->TempData.BackInfo.ZonePlayerInfo;
		if( abs( Info.LX - Owner->X() ) > 50 || abs( Info.LZ - Owner->Z() ) > 50 )
		{
			Info.LX = Owner->X();
			Info.LZ = Owner->Z();
			NetSrv_Move::S_AllCli_BattleGround_PlayerPos( Owner );
		}

		if( Info.LHPMPSPSum != Owner->HPMPSPSum() )
		{
			Info.LHPMPSPSum = Owner->HPMPSPSum();
			NetSrv_Move::S_AllCli_BattleGround_PlayerHPMP( Owner );
		}
	}
	*/
	//////////////////////////////////////////////////////////////////////////
	if (Global::Ini()->IsBattleWorld == true)
	{
		if (Owner->ClockMod(10 * 5 * 60))
		{
			NetSrv_CliConnect::SBL_BattleSavePlayer(Owner->PlayerTempData->ChangeWorld.WorldID, Owner->PlayerTempData->ChangeWorld.WorldNetID, Owner->DBID(), Owner, OwnerObj->OrgWorldGUID());
		}
	}

}
//---------------------------------------------------------------------------------------------
//Гц«YіBІz
void	Global::PartyBuffProc(RoleDataEx* Owner)
{
	if (Owner->PartyID() == -1)
		return;

	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo(Owner->BaseData.PartyID);
	if (Party == NULL)
	{
		Owner->BaseData.PartyID = -1;
		return;
	}

	BaseFriendStruct* FriendInfo;

	for (unsigned int i = 0; i < Party->Member.size(); i++)
	{
		if (Party->Member[i].DBID == Owner->DBID())
			continue;

		RoleDataEx* M = Global::GetRoleDataByDBID(Party->Member[i].DBID);

		if (M == NULL)
			continue;

		if (Owner->Length(M) > _MAX_PARTY_SHARE_RANGE_)
			continue;

		if (Owner->RoomID() != M->RoomID())
			continue;

		int RetPos = Owner->PlayerSelfData->FriendList.FindFriendInfo(EM_FriendListType_FamilyFriend, Party->Member[i].DBID, &FriendInfo);
		if (RetPos == -1)
			continue;

		//Ёъ±oГц«YЄkіN
		RelationMagicStruct* RelationMagic = g_ObjectData->RelationMagicInfo(FriendInfo->Relation, (int)FriendInfo->Lv);
		if (RelationMagic == NULL || RelationMagic->IsEmpty())
			continue;

		MagicProcInfo  MagicInfo;

		if (M->TempData.Attr.Fin.MaxHP > M->BaseData.HP * 4)
		{
			MagicInfo.Init();
			MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
			MagicInfo.TargetID = M->GUID();							//ҐШјР

			MagicInfo.MagicCollectID = RelationMagic->Quarter_HP_Magic;
			MagicProcessClass::SysSpellMagic(Owner, &MagicInfo);
		}

		MagicInfo.Init();
		MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
		MagicInfo.TargetID = M->GUID();							//ҐШјР

		MagicInfo.MagicCollectID = RelationMagic->NormalMagic;
		MagicProcessClass::SysSpellMagic(Owner, &MagicInfo);

	}
}
//---------------------------------------------------------------------------------------------
void	Global::_MagicToolsProc(RoleDataEx* Owner)
{
	Owner->TempData.MagicToolsColdown--;

	if (Owner->IsDead() == false && Owner->TempData.MagicToolsColdown <= 0)
	{
		//¦pЄG¦іЄkД_ЎA«hіBІzЄkД_Єєј@±Ў		
		for (int i = 0; i < 3; i++)
		{
			GameObjDbStructEx* MagicToolsDB = Global::GetObjDB(Owner->BaseData.EQ.Item[EM_EQWearPos_MagicTool1 + i].OrgObjID);
			ItemFieldStruct* MagicToolItem = Owner->GetEqItem(EM_EQWearPos_MagicTool1 + i);
			if (MagicToolsDB->IsArmor() == false || MagicToolItem->IsEmpty())
			{
				Owner->TempData.MagicToolsColdown = 5;
			}
			else
			{
				Owner->TempData.MagicToolsColdown = MagicToolsDB->Item.MagicToolsColdown;

				vector<MyVMValueStruct> RetList;

				if (strlen(MagicToolsDB->Item.MagicToolsScript) != 0)
				{
					char ScriptBuf[256];
					sprintf_s(ScriptBuf, sizeof(ScriptBuf), "%s(%d)", MagicToolsDB->Item.MagicToolsScript, MagicToolItem->Durable);
					bool Ret = true;
					if (LUA_VMClass::PCallByStrArg(ScriptBuf, Owner->GUID(), Owner->GUID(), &RetList, 1))
					{

						if (RetList.size() == 1)
						{
							MyVMValueStruct& Temp = RetList[0];
							if (Temp.Type == MyVMValueType_Number)
							{
								bool IsNeedFix = false;
								if (Temp.Number_Int == 0)
								{
									Log_ItemDestroy(Owner, EM_ActionType_UseItemDestroy, *MagicToolItem, -1, -1, "");
									MagicToolItem->Init();
									IsNeedFix = true;
								}
								else
								{
									if (MagicToolItem->Durable != Temp.Number_Int)
									{
										IsNeedFix = true;
										MagicToolItem->Durable = Temp.Number_Int;
									}
									else
									{
										//¦pЄGЁS¦іЁПҐОЁм
										Owner->TempData.MagicToolsColdown = MagicToolsDB->Item.MagicToolsTime;
									}
								}
								if (IsNeedFix == true)
								{
									Owner->Net_FixItemInfo_EQ(EM_EQWearPos_MagicTool1 + i);
									break;
								}
							}
						}
					}
				}
			}


		}
	}

}
//Ґu¦іNPC©w®Й·|ДІµo
void     Global::_NPC_OnTimeProc(BaseItemObject* OwnerObj, RoleDataEx* Owner)
{
	Owner->OnTimeProc_NPC();

	if (!Owner->IsDead() && Owner->ClockMod(60) && Owner->TempData.Attr.Effect.StopOntimeCure == false)
	{
		if (Owner->ClockMod(120))
			Owner->AddStomachPoint(1);


		//¤K¬ніBІz¤@¦ё і]©w¬O§_БЩ¦b§рА»јТ
		if (Owner->MaxHP() != int(Owner->HP())
			|| Owner->MaxMP() != int(Owner->MP()))
		{
			if (Owner->OwnerGUID() == -1)
			{
				if (Owner->IsAttackMode()
					|| Owner->IsAttack())
				{
					AddMP(Owner, NULL, (Owner->TempData.Attr.Fin.ReMP));
				}
				else
				{
					AddHP(Owner, NULL, (Owner->TempData.Attr.Fin.MaxHP) / 5);
					AddMP(Owner, NULL, (Owner->TempData.Attr.Fin.MaxMP) / 5);
				}
			}
			else
			{
				if (Owner->IsAttackMode())
				{
					AddMP(Owner, NULL, (Owner->TempData.Attr.Fin.ReMP) / 24);
				}
				else
				{
					AddHP(Owner, NULL, (Owner->TempData.Attr.Fin.ReHP) * 0.66f);
					AddMP(Owner, NULL, (Owner->TempData.Attr.Fin.ReMP) * 0.66f);
				}

			}

		}

		if (Owner->IsAttack())
		{

			if (Owner->IsMove())
			{
				AddSP(Owner, NULL, (Owner->TempData.Attr.Arg.ReSPAttackTime_Move * Owner->TempData.Attr.Fin.MainJobSPRate));
				AddSP_Sub(Owner, NULL, (Owner->TempData.Attr.Arg_Sub.ReSPAttackTime_Move * Owner->TempData.Attr.Fin.SubJobSPRate));
			}
			else
			{
				AddSP(Owner, NULL, (Owner->TempData.Attr.Arg.ReSPAttackTime_Stop * Owner->TempData.Attr.Fin.MainJobSPRate));
				AddSP_Sub(Owner, NULL, (Owner->TempData.Attr.Arg_Sub.ReSPAttackTime_Stop * Owner->TempData.Attr.Fin.SubJobSPRate));
			}
		}
		else
		{
			if (Owner->IsMove())
			{
				//				Owner->IsSit( false );
				AddSP(Owner, NULL, (Owner->TempData.Attr.Arg.ReSPNormal_Move * Owner->TempData.Attr.Fin.MainJobSPRate));
				AddSP_Sub(Owner, NULL, (Owner->TempData.Attr.Arg_Sub.ReSPNormal_Move * Owner->TempData.Attr.Fin.SubJobSPRate));
			}
			else
			{
				AddSP(Owner, NULL, (Owner->TempData.Attr.Arg.ReSPNormal_Stop * Owner->TempData.Attr.Fin.MainJobSPRate));
				AddSP_Sub(Owner, NULL, (Owner->TempData.Attr.Arg_Sub.ReSPNormal_Stop * Owner->TempData.Attr.Fin.SubJobSPRate));
			}
		}
	}

	if (Owner->ClockMod(80))
	{
		/*//¤K¬ніBІz¤@¦ё і]©w¬O§_БЩ¦b§рА»јТ
		if( Owner->IsAttackMode() && Owner->IsAttack() == false )
		{
			Owner->FixAttackMode();
		}
		*/
	}


}
//---------------------------------------------------------------------------------------------
void     Global::_Sys_OnTimeProc()
{
	static unsigned LastProcTime = RoleDataEx::G_Now;

	bool	IsDayChange = false;

	if ((LastProcTime + RoleDataEx::G_TimeZone * 60 * 60) / (60 * 60 * 24) != (RoleDataEx::G_Now + RoleDataEx::G_TimeZone * 60 * 60) / (60 * 60 * 24))
	{
		IsDayChange = true;
	}

	SpaceStruct     Pos;
	BaseItemObject* Obj;
	RoleDataEx* Role;


	//ЁC10 ¬н§у·s°П°мёк°T
	//( Є«Ґу©Т¦b¦мёm »P Є«Ґу­PЇаі]©w )
	if (RoleDataEx::G_Clock % 100 == 0)
	{
		if (Global::Ini()->IsSendAllPlayerPos == true)
		{
			NetSrv_MoveChild::SendAllPlayerPos();
		}

		_St->PosList.clear();
		Pos.Y = 0;
		set<BaseItemObject* >& NpcObjSet = *(BaseItemObject::NPCObjSet());
		set< BaseItemObject*>::iterator   NpcObjIter;
		for (NpcObjIter = NpcObjSet.begin(); NpcObjIter != NpcObjSet.end(); NpcObjIter++)
		{
			Obj = *NpcObjIter;
			if (Obj == NULL)
				continue;

			Role = Obj->Role();
			Pos.RoomID = Role->RoomID();
			Pos.X = Role->TempData.Move.LockTargetPosX;
			Pos.Z = Role->TempData.Move.LockTargetPosZ;
			Pos.Y = 0;
			_St->PosList[Pos] = Role;

		}
	}
	if (RoleDataEx::G_Clock % 50 == 0)
	{
		{
			static int PingLogID = 0;
			if (St()->PingLogCount > 0)
			{
				St()->PingLogCount--;
				PingLogID++;
				NetSrv_Move::S_All_PingLog(PingLogID, timeGetTime());
			}
		}

		{
			if (St()->ProxyPingLogCount > 0)
			{
				St()->ProxyPingLogCount--;
				_Net->ProxyPingLog();
			}
		}
	}

	if (RoleDataEx::G_Clock % 80 == 0)
	{
		FixAllAttackMode();
	}

	//ЁC¤АДБ¦s°П°мЄ¬єAёк®Ж
	if (RoleDataEx::G_Clock % 600 == 0)
	{
		Save_ZoneInfo();
		Log_ZoneMoneyLog();
		NetSrv_InstanceChild::CheckInstanceLiveTime();
	}

	if (IsDayChange)
	{
		int LockAccountTime = RoleDataEx::G_Now / (60 * 60 * 24);
		set<BaseItemObject* >& PlayObjSet = *(BaseItemObject::PlayerObjSet());
		set<BaseItemObject* >::iterator Iter;

		for (Iter = PlayObjSet.begin(); Iter != PlayObjSet.end(); Iter++)
		{
			RoleDataEx* Role = (*Iter)->Role();
			Role->ResetLockAccountMoney();
		}

	}
	/*
		if( Global::Ini()->IsZonePartyEnabled == true && RoleDataEx::G_Clock % 100 == 0  )
		{
			NetSrv_MoveChild::SendAllBattlePlayerInfo( );
		}
		*/
}
//---------------------------------------------------------------------------------------------
void     Global::_OnTimeProc()
{
	RoleDataEx::G_NowStr = TimeStr::Now();
	RoleDataEx::G_Now = TimeStr::Now_Value() + RoleDataEx::SysDTime;
	RoleDataEx::G_Now_Float = TimeExchangeIntToFloat(RoleDataEx::G_Now + RoleDataEx::G_TimeZone * 60 * 60);
	RoleDataEx::G_Day = (RoleDataEx::G_Now + RoleDataEx::G_TimeZone * 60 * 60) / (24 * 60 * 60);
	RoleDataEx::OnTimeProcAll();

	//_St->Schedular->Process( );

	BaseItemObject::OnTimeProc();

	_Sys_OnTimeProc();

	//ґъёХ
	if (RoleDataEx::G_Clock % (10) == 0)
	{
		if (Ini()->IsHouseZone)
		{
			HousesManageClass::OnTimeProcAll();
		}
		NetSrv_MoveChild::FixAllPlayerPosProc();
	}

	if (RoleDataEx::G_Clock % (100) == 0)
	{
		//§у·sServeЄ¬єA
		NetSrv_GMTools::S_ZoneInfo(DF_NO_LOCALID
			, Ini()->ZoneID
			, Ini()->ZoneName.c_str()
			, BaseItemObject::TotalCount()
			, BaseItemObject::PlayerCount()
			, BaseItemObject::NPCCount());

		//їWҐЯZoneАЛ¬dЎA¬O§_¶·­nёСєc
		CheckPrivateZone();

		if (Ini()->IsHouseZone)
		{
			//			HousesManageClass::OnTimeProcAll();
		}
		else if (Ini()->IsGuildHouseZone)
		{
			GuildHousesManageClass::OnTimeProcAll();
		}
		/*
		else if( Ini()->IsGuildHouseWarZone )
		{
			GuildHouseWarManageClass::OnTimeProcAll( );
		}
		*/

		//ЁC¤p®ЙіBІz
		if (RoleDataEx::G_Clock % (10 * 60 * 60) == 0)
		{
			Log_ItemServerLog();
		}
		//6ЁC¤p®ЙіBІz
		if (RoleDataEx::G_Clock % (10 * 60 * 60 * 6) == 0)
		{
			//Log_ItemServerLog();
			RecycleBinMemCheck();
		}

		if (RoleDataEx::G_Clock % (100 * 60) == 0)
		{
			Log_ServerStatus();
		}

	}

	if (RoleDataEx::G_Clock % (10) == 0)
	{
		if (Ini()->IsGuildHouseWarZone)
		{
			GuildHouseWarManageClass::OnTimeProcAll();
		}
	}

	unsigned BeginLuaProcTime = GetTickCount();
	LUA_VMClass::Process();
	unsigned LuaProcTime = GetTickCount() - BeginLuaProcTime;
	if (LuaProcTime > 200)
	{
		Print(LV5, "OnTimeProc", " LuaProcTime=%d", LuaProcTime);
	}

	LuaDebugger::Process();
}
//---------------------------------------------------------------------------------------------
BaseItemObject* Global::Search(ComObjFunction  Func, void* Data)
{
	return BaseItemObject::FindEach(Func, Data);
}
//---------------------------------------------------------------------------------------------
//Є««~§Зё№ІЈҐН
int	Global::GenerateItemVersion()
{
	/*
	GameObjDbStructEx*	OrgDB = GetObjDB( OrgObjID );

	//Ґu¦і¤ЈҐi°пЕ|ЄєЄ««~ҐiҐH¦і°Я¤@§Зё№
	if( OrgDB->MaxHeap != 1 )
		return 0;
		*/

	_St->LocalDBInfo.ItemVersion++;
	int	Version = _St->LocalDBInfo.ItemVersion + _St->Ini.ZoneID * 100000;
	if (_St->LocalDBInfo.ItemVersion > 100000)
		_St->LocalDBInfo.ItemVersion = 1;

	return Version;
}
//---------------------------------------------------------------------------------------------
//ІЈҐН¬YЄ«Ґу ¦h¤Ц­У( ±±ЁоЄ«ҐуІЈҐН¶q )
bool Global::ItemGenerate(int OrgObjID, int Count)
{

	map< int, float >::iterator Iter;

	Iter = _St->ObjectAccumulate.find(OrgObjID);

	if (Iter == _St->ObjectAccumulate.end())
		return false;

	if (Iter->second < Count)
		return false;

	Iter->second -= Count;

	return true;

}
//---------------------------------------------------------------------------------------------
//АЛ¬d¬YЄ«ҐуҐiҐHҐНІЈ¦h¤Ц­У
int	Global::GetItemMaxGenerate(int OrgObjID)
{
	map< int, float >::iterator Iter;

	Iter = _St->ObjectAccumulate.find(OrgObjID);

	if (Iter == _St->ObjectAccumulate.end())
		return 0;

	return int(Iter->second);
}
//----------------------------------------------------------------------------------------
//ЄмЁП¤ЖItem ІЈҐНјЖ¶q
void Global::InitGenerate()
{
	int		i;
	GameObjDbStructEx* OrgDB;

	g_ObjectData->ObjectDB()->GetDataByOrder(true);

	for (i = 0; i < g_ObjectData->ObjectDB()->TotalInfoCount(); i++)
	{
		OrgDB = g_ObjectData->ObjectDB()->GetDataByOrder();
		if (OrgDB == NULL)
			return;

		if (!OrgDB->IsItem())
			continue;
		//ҐШ«eі]©w¬°№п¤j®w¦s¶qЄє¤@Ґb
		_St->ObjectAccumulate[OrgDB->GUID] = OrgDB->MaxWorldCount / 2;
	}
}
//---------------------------------------------------------------------------------------------
//	ҐX©l¤Ж·sІЈҐНҐXЁУЄєЄ««~(®Й¶Ў §Зё№)
//----------------------------------------------------------------------------------------
bool	Global::NewItemInit(ItemFieldStruct& Item, int OrgObjID, int NPCObjID)
{
	GameObjDbStructEx* OrgDB = GetObjDB(OrgObjID);

	if (!OrgDB->IsItem() && !OrgDB->IsRecipe())
		return false;

	Item.Init();
	Item.OrgObjID = OrgObjID;
	Item.ImageObjectID = OrgObjID;
	Item.Serial = GenerateItemVersion();
	Item.CreateTime = RoleDataEx::G_Now;

	Item.Mode.DepartmentStore = OrgDB->Mode.DepartmentStore;

	if (OrgDB->Mode.PickupBound == true)
	{
		Item.Mode.EQSoulBound = 0;
		Item.Mode.Trade = 0;
	}
	else
	{
		Item.Mode.EQSoulBound = OrgDB->Mode.EQSoulBound;
		Item.Mode.Trade = 1;
	}

	if (OrgDB->Mode.HideCount == true && OrgDB->MaxHeap != 1)
		Item.Mode.HideCount = true;
	else
		Item.Mode.HideCount = false;

	//Ґu¦і¤ЈҐi°пЕ|ЄєЄ««~ҐiҐH¦і°Я¤@§Зё№
	if (OrgDB->MaxHeap != 1)
	{
		Item.OrgQuality = Item.Quality = 100;
		Item.PowerQuality = 10;
		Item.Durable = Item.OrgQuality * 100;
		return true;
	}

	Item.Count = 1;
	Item.ExValue = 0;
	Item.MainColor = 0;
	Item.OffColor = 0;


	if (OrgDB->Item.IsFixDurable == true)
	{
		Item.OrgQuality = Item.Quality = 100;
		Item.PowerQuality = 10;
	}
	else
	{
		float  RandValue = float(rand() % 2500);
		Item.OrgQuality = Item.Quality = 120 - int(sqrt(RandValue));

		Item.PowerQuality = 10;
	}

	if (OrgDB->IsWeapon() || OrgDB->IsArmor())
	{
		Item.PowerQuality = 10 + OrgDB->Item.AddPowerQuality;
		Item.Level = __min(OrgDB->Item.RefineLv, 20);
	}



	if (OrgDB->Mode.DurableStart0 == true)
	{
		Item.Durable = 0;
	}
	else
	{
		Item.Durable = Item.Quality * OrgDB->Item.Durable;
	}

	for (int i = 0; i < OrgDB->Item.HoleBase && i < 2; i++)
	{
		if (i == 0)
		{
			if (rand() % 4)
				break;
		}
		else if (rand() % 10)
			break;

		Item.RuneVolume++;
	}


	int TObjID, TCount;
	set< int >	InherentSet;
	int		InerentPos = 0;
	ItemDropStruct* DropItemAbility = OrgDB->Item.DropItemAbility;
	//////////////////////////////////////////////////////////////////////////
	vector<int>		DropAbilityPos;

	for (int i = 0; i < _MAX_DROP_ABILITY_COUNT_; i++)
	{
		if (DropItemAbility[i].Rate != 0)
			DropAbilityPos.push_back(i);
	}

	if (OrgDB->Item.SelectBaseAttrCount < 0)
		OrgDB->Item.SelectBaseAttrCount = 0;

	while ((int)DropAbilityPos.size() > OrgDB->Item.SelectBaseAttrCount)
	{
		DropAbilityPos.erase(DropAbilityPos.begin() + rand() % DropAbilityPos.size());
	}

	int TestInherentSetCount = 0;

	//°тҐ»Їа¤O±јёЁ
	for (int i = 0; i < _MAX_DROP_ABILITY_COUNT_ && InerentPos < OrgDB->Item.MaxAttrCount && i < (int)DropAbilityPos.size(); i++)
	{
		int Pos = DropAbilityPos[i];

		int RandValue = Random(100000);
		if (RandValue >= DropItemAbility[Pos].Rate)
			continue;

		if (RoleDataEx::GetTreasure(DropItemAbility[Pos].OrgID, TObjID, TCount) == false)
		{
			TObjID = DropItemAbility[Pos].OrgID;
			TCount = 1;
		}

		{
			GameObjDbStructEx* AbilityObjDB = GetObjDB(TObjID);
			if (AbilityObjDB->IsAttribute() == false)
				continue;

			if (OrgDB->CheckInherentAbility(AbilityObjDB) == false)
				continue;

			if (InherentSet.insert(TObjID).second == false)
			{
				/*	if( TestInherentSetCount < 40 )
					{
						TestInherentSetCount++;
						i--;
					}*/
				continue;
			}

			Item.Inherent(InerentPos, TObjID);
			InerentPos++;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//АЛ¬d¬O§_­nіBІzNPC±јёЁДЭ©К
	if (OrgDB->Mode.NoNPCAbility == true)
		return true;

	GameObjDbStructEx* NpcObjDB = GetObjDB(NPCObjID);
	if (NpcObjDB->IsNPC() == false)
		return true;

	int Lv = NpcObjDB->NPC.Level + NpcObjDB->NPC.LevelRange / 2 + NpcObjDB->NPC.AddPowerLV;

	DropItemAbility = NpcObjDB->NPC.DropItemAbility;

	DropAbilityPos.clear();
	for (int i = 0; i < _MAX_DROP_ABILITY_COUNT_; i++)
	{
		if (DropItemAbility[i].Rate != 0)
			DropAbilityPos.push_back(i);
	}

	for (int i = 0; i < (int)DropAbilityPos.size(); i++)
	{
		swap(DropAbilityPos[i], DropAbilityPos[rand() % DropAbilityPos.size()]);
	}

	for (int i = 0; i < _MAX_DROPITEM_COUNT_ && i < (int)DropAbilityPos.size(); i++)
	{
		if (InerentPos >= OrgDB->Item.MaxAttrCount)
			break;

		int AbilityPos = DropAbilityPos[i];
		int RandValue = Random(100000);

		if (InerentPos < OrgDB->Item.BaseAttrCount)
			RandValue = 0;

		if (RandValue >= DropItemAbility[AbilityPos].Rate)
			continue;

		for (int j = 0; j < 20; j++)
		{
			if (RoleDataEx::GetTreasure(DropItemAbility[AbilityPos].OrgID, TObjID, TCount) == false)
			{
				TObjID = DropItemAbility[AbilityPos].OrgID;
				TCount = 1;
			}

			GameObjDbStructEx* AbilityObjDB = GetObjDB(TObjID);
			if (AbilityObjDB->IsAttribute() == false || AbilityObjDB->Attirbute.StandardAbility_Lv > Lv)
				continue;

			if (NpcObjDB->NPC.MinAbilityLv > AbilityObjDB->Attirbute.StandardAbility_Lv)
				continue;

			if (OrgDB->CheckInherentAbility(AbilityObjDB) == false)
				continue;

			if (InherentSet.insert(TObjID).second == false)
				continue;

			Item.Inherent(InerentPos, TObjID);
			InerentPos++;
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	return true;
}
//----------------------------------------------------------------------------------------
//	·jґM¶gітЄ±®a
//----------------------------------------------------------------------------------------
void			Global::ResetRange(RoleDataEx* Owner, int Range)
{
	_St->SearchPlayerResult.clear();


	MyMapAreaManagement* Part = Global::Partition(Owner);
	if (Part == NULL)
		return;

	PlayIDInfo** Block = Part->Search(
		Owner->TempData.Sys.SecX
		, Owner->TempData.Sys.SecZ
		, Range
		, Range);

	int			i, j;
	PlayID* TopID;

	for (i = 0; Block[i] != NULL; i++)
	{
		TopID = Block[i]->Begin;

		for (j = 0; TopID != NULL; TopID = TopID->Next)
		{

			BaseItemObject* OtherObj = BaseItemObject::GetObj(TopID->ID);
			if (OtherObj == NULL)
				continue;
			//            if( OtherObj->Role()->IsPlayer() )
			//                continue;

			_St->SearchPlayerResult.push_back(OtherObj);

		}
	}

	if (_St->SearchPlayerResult.size() == 0)
	{
		BaseItemObject* OwnerObj = BaseItemObject::GetObj(Owner->GUID());
		if (OwnerObj != NULL)
			_St->SearchPlayerResult.push_back(OwnerObj);
	}
}

RoleDataEx* Global::GetRangeRole()
{
	/*
	if( _St->SearchPlayerResult.size() == 0 )
	return NULL;

	RoleDataEx* Other = (RoleDataEx*)_St->SearchPlayerResult.back()->Role();
	_St->SearchPlayerResult.pop_back();
	return Other;
	*/
	BaseItemObject* Obj = GetRangeObj();
	if (Obj == NULL)
		return NULL;

	return Obj->Role();
}
BaseItemObject* Global::GetRangeObj()
{
	if (_St->SearchPlayerResult.size() == 0)
		return NULL;

	while (_St->SearchPlayerResult.size())
	{
		BaseItemObject* Other = _St->SearchPlayerResult.back();
		_St->SearchPlayerResult.pop_back();
		return Other;
	}
	return NULL;
}

RoleDataEx* Global::GetRangePlayer()
{

	BaseItemObject* Obj = GetRangePlayerObj();
	if (Obj == NULL)
		return NULL;

	return Obj->Role();
}
BaseItemObject* Global::GetRangePlayerObj()
{
	if (_St->SearchPlayerResult.size() == 0)
		return NULL;

	while (_St->SearchPlayerResult.size())
	{
		BaseItemObject* Other = _St->SearchPlayerResult.back();
		_St->SearchPlayerResult.pop_back();
		if (Other->Role()->IsPlayer())
			return Other;
	}
	return NULL;
}
//----------------------------------------------------------------------------------------
//ёф®|іBІz
//----------------------------------------------------------------------------------------
void	Global::SearchAttackNode(RoleDataEx* Owner, RoleDataEx* Target, float& NX, float& NY, float& NZ)
{
	//	if( Owner->BaseData.Mode.Move == false )
	//		return;

	if (Target->IsPlayer()
		|| Owner->Length(Target) < 40)
	{
		SearchAttackNode(Owner, Target->Pos()->X, Target->Pos()->Y, Target->Pos()->Z, Target->TempData.BoundRadiusY, NX, NY, NZ);
	}
	else
	{
		float x, y, z;
		BaseItemObject* TargetObj = Global::GetObj(Target->GUID());

		if (Target->BaseData.Mode.Move == true
			&& Target->TempData.Attr.Effect.Stop == false)
		{
			TargetObj->Path()->GetTargetPoint(x, y, z);

			x = (x + Target->Pos()->X) / 2;
			z = (z + Target->Pos()->Z) / 2;
		}
		else
		{
			x = Target->Pos()->X;
			y = Target->Pos()->Y;
			z = Target->Pos()->Z;
		}

		SearchAttackNode(Owner, x, y, z, Target->TempData.BoundRadiusY, NX, NY, NZ);
	}

}

void	Global::SearchAttackNode(RoleDataEx* Owner, float TX, float TY, float TZ, float TBoundRadius, float& NX, float& NY, float& NZ)
{
	//	if( Owner->BaseData.Mode.Move == false )
	//		return;

	BaseItemObject* OwnerObj = BaseItemObject::GetObj(Owner->GUID());
	if (OwnerObj == NULL)
		return;

	SpaceStruct		Pos;
	Pos.Y = 0;
	Pos.RoomID = Owner->RoomID();

	map< SpaceStruct, RoleData* >::iterator Iter;
	float		X, Y, Z;

	float		OrgX = Owner->BaseData.Pos.X;
	float		OrgZ = Owner->BaseData.Pos.Z;
	float		OrgY = Owner->BaseData.Pos.Y;

	float		BaseX = TX + _Def_Map_BaseSize_ * 2 - rand() % (_Def_Map_BaseSize_ * 4);
	float		BaseZ = TZ + _Def_Map_BaseSize_ * 2 - rand() % (_Def_Map_BaseSize_ * 4);
	float		BaseY = Owner->Y();
	float		BaseL = 100000000;
	float		L;

	float	Radius = (Owner->TempData.BoundRadiusY + TBoundRadius + 3);

	if (Radius < Owner->TempData.AI.KeepDistance - 20)
		Radius = float(Owner->TempData.AI.KeepDistance) - 20;

	if (Radius > 50
		&& Owner->Length(TX, TY, TZ) < 50)
		Radius = 50;

	int Time = int((PI * Radius) / Owner->TempData.BoundRadiusX);
	if (Time < 8)
		Time = 8;
	if (Time >= 20)
		Time = 20;
	std::vector<POINT3D>& PointList = *(OwnerObj->Path()->FindNeighborPoint(TX, TY, TZ, Radius, Time));

	for (int i = 0; i < (int)PointList.size(); i++)
	{
		X = PointList[i].x;
		Z = PointList[i].z;
		Y = PointList[i].y;

		Pos.X = int(X / _Def_Map_BaseSize_);
		Pos.Z = int(Z / _Def_Map_BaseSize_);

		if (OwnerObj->Path()->CheckLine_Magic(OrgX, OrgY, OrgZ, X, Y, Z) == false)
			continue;

		if (Owner->TempData.Move.LockTargetPosX == Pos.X
			&& Owner->TempData.Move.LockTargetPosZ == Pos.Z)
		{

		}
		else
		{
			Iter = _St->PosList.find(Pos);
			if (Iter != _St->PosList.end())
			{
				if (BaseL >= 10000000)
				{
					BaseL = 10000000 - 1;
					BaseX = X;
					BaseZ = Z;
					BaseY = Y;
				}
				else if (BaseL == 10000000 - 1)
				{
					if (rand() % 2)
					{
						BaseX = X;
						BaseZ = Z;
						BaseY = Y;
					}
				}
				continue;
			}
		}



		L = (X - OrgX) * (X - OrgX) + (Z - OrgZ) * (Z - OrgZ) + (Y - OrgY) * (Y - OrgY);

		if (L < BaseL)
		{
			BaseL = L;
			BaseX = X;
			BaseZ = Z;
			BaseY = Y;
		}
	}


	if (BaseL >= 10000000)
	{
		//		float Angle = float( rand() % 628 )/ 100;
		//		NX = TX + sin( Angle ) * Radius;
		//		NZ = TZ + cos( Angle ) * Radius;
		NX = TX;
		NY = TY;
		NZ = TZ;
	}
	else
	{
		NX = BaseX;
		NZ = BaseZ;
		NY = BaseY;
	}

	if (Owner->TempData.Move.LockTargetPosX != 0
		&& Owner->TempData.Move.LockTargetPosZ != 0)
	{
		Pos.X = Owner->TempData.Move.LockTargetPosX;
		Pos.Z = Owner->TempData.Move.LockTargetPosZ;
		_St->PosList.erase(Pos);
	}

	Pos.X = int(NX / _Def_Map_BaseSize_);
	Pos.Z = int(NZ / _Def_Map_BaseSize_);

	Owner->TempData.Move.LockTargetPosX = Pos.X;
	Owner->TempData.Move.LockTargetPosZ = Pos.Z;

	_St->PosList[Pos] = Owner;
}

//°}°h¦мёm
void	Global::SearchStrikeBackNode(RoleDataEx* Owner, float Tx, float Ty, float Tz, float Range, float& NX, float& NZ)
{
	BaseItemObject* OwnerObj = Global::GetObj(Owner->GUID());
	if (OwnerObj == NULL)
		return;
	//јИ®Й
	float Dx = Owner->Pos()->X - Tx;
	float Dy = Owner->Pos()->Y - Ty;
	float Dz = Owner->Pos()->Z - Tz;
	Dy = 0;

	float Length = sqrt(Dx * Dx + Dy * Dy + Dz * Dz);

	if (Length == 0)
		Length = 0.01f;

	float vX = Dx / Length;
	float vY = Dy / Length;
	float vZ = Dz / Length;


	for (int i = 1; i <= 3; i++)
	{
		NX = Owner->Pos()->X + vX * Range / i;
		NZ = Owner->Pos()->Z + vZ * Range / i;

		if (OwnerObj->Path()->CheckLine(Owner->Pos()->X, Owner->Pos()->Y, Owner->Pos()->Z, NX, Owner->Pos()->Y, NZ) == false)
			continue;

		return;
	}
	NX = Owner->Pos()->X;
	NZ = Owner->Pos()->Z;

}

void    Global::SearchNode(RoleDataEx* Owner, float& NX, float& NZ)
{
	SpaceStruct		Pos;
	map< SpaceStruct, RoleData* >::iterator Iter;
	int		i, j;
	float	X = NX;
	float	Z = NZ;
	float	BaseX = X + _Def_Map_BaseSize_ * 2 - rand() % (_Def_Map_BaseSize_ * 4);
	float	BaseZ = Z + _Def_Map_BaseSize_ * 2 - rand() % (_Def_Map_BaseSize_ * 4);
	float	BaseL = 100000000;
	float	L;

	Pos.RoomID = Owner->BaseData.RoomID;

	for (i = -1; i < 2; i++)
	{
		for (j = -1; j < 2; j++)
		{
			X = NX + i * _Def_Map_BaseSize_;
			Z = NZ + j * _Def_Map_BaseSize_;
			Pos.X = int(X / _Def_Map_BaseSize_);
			Pos.Z = int(Z / _Def_Map_BaseSize_);

			Iter = _St->PosList.find(Pos);
			if (Iter != _St->PosList.end())
				continue;

			L = (X - Owner->BaseData.Pos.X) * (X - Owner->BaseData.Pos.X) + (Z - Owner->BaseData.Pos.Z) * (Z - Owner->BaseData.Pos.Z);

			if (L < BaseL)
			{
				BaseL = L;
				BaseX = X;
				BaseZ = Z;
			}
		}
	}

	NX = BaseX;
	NZ = BaseZ;

	if (Owner->TempData.Move.LockTargetPosX != 0
		&& Owner->TempData.Move.LockTargetPosZ != 0)
	{
		Pos.X = Owner->TempData.Move.LockTargetPosX;
		Pos.Z = Owner->TempData.Move.LockTargetPosZ;
		_St->PosList.erase(Pos);
	}

	Pos.X = int(NX / _Def_Map_BaseSize_);
	Pos.Z = int(NZ / _Def_Map_BaseSize_);

	Owner->TempData.Move.LockTargetPosX = Pos.X;
	Owner->TempData.Move.LockTargetPosZ = Pos.Z;

	_St->PosList[Pos] = Owner;
}

RoleDataEx* Global::SearchAttackModeObj(BaseItemObject* OwnerObj, int Range)
{

	vector<BaseSortStruct>* ObjList = SearchRangeObject(OwnerObj->Role()
		, OwnerObj->Role()->Pos()->X
		, OwnerObj->Role()->Pos()->Y
		, OwnerObj->Role()->Pos()->Z
		, EM_SearchRange_All, Range);

	for (unsigned int i = 0; i < ObjList->size(); i++)
	{
		RoleDataEx* Role = (RoleDataEx*)(*ObjList)[i].Data;
		if (!Role->IsNPC() && !Role->IsPlayer())
			continue;

		if (Role->GUID() == OwnerObj->Role()->GUID())
			continue;

		if (Role->IsAttackMode())
			return Role;
	}
	return NULL;
}

RoleDataEx* Global::SearchEnemy(BaseItemObject* OwnerObj)
{
	static	vector< BaseSortStruct >	EnemyList;
	EnemyList.clear();

	BaseSortStruct  SortTemp;
	//    float             Len;

	RoleDataEx* Owner = (RoleDataEx*)OwnerObj->Role();

	if (Owner->BaseData.Mode.Searchenemy == false)
		return NULL;

	if (Owner->TempData.Attr.Effect.SearchEmenyDisable)
		return NULL;


	NPC_AI_Struct* AI = &Owner->TempData.AI;

	GameObjDbStructEx* OrgDB;
	OrgDB = Global::GetObjDB(Owner->BaseData.ItemInfo.OrgObjID);

	if (!OrgDB->IsNPC())
	{
		Print(LV5, "SearchEnemy", "OrgDB(%d) Not NPC", Owner->BaseData.ItemInfo.OrgObjID);
		return NULL;
	}

	if (OrgDB->NPC.SearchRange == 0)
		return NULL;


	if (OrgDB->NPC.SearchRange + 5 > OrgDB->NPC.FollowRange)
		OrgDB->NPC.SearchRange = OrgDB->NPC.FollowRange - 5;


	int BlockCount = OrgDB->NPC.SearchRange / Global::Ini()->BlockSize + 1;
	int HelpRange = __min(OrgDB->NPC.SearchRange, 150);

	BaseItemObject* TargetObj;
	RoleDataEx* Target;
	PlayIDInfo** Block = Global::PartSearch(Owner, BlockCount);

	int			i, j;
	PlayID* TopID;

	for (i = 0; Block[i] != NULL; i++)
	{
		TopID = Block[i]->Begin;
		for (j = 0; TopID != NULL; TopID = TopID->Next)
		{
			//АЛ¬d¬O§_¬O¦Ы¤v
			if (TopID->ID == Owner->TempData.Sys.GUID)
				continue;

			TargetObj = BaseItemObject::GetObj(TopID->ID);
			if (TargetObj == NULL)
				continue;

			Target = TargetObj->Role();

			if (Target->BaseData.Mode.SearchenemyIgnore == true)
				continue;

			if (Target->TempData.Attr.Effect.BadPhyInvincible == true)
				continue;

			if (Target->BaseData.Mode.Fight == false)
				continue;

			if (RoleDataEx::CheckTime(Target->TempData.Sys.LoginProtect) == false)
				continue;

			float Len = Owner->Length3D(Target);
			if (Len > OrgDB->NPC.SearchRange)
				continue;

			if (Len < Owner->TempData.BoundRadiusY + 55)
				Len = Owner->TempData.BoundRadiusY + 55;

			if (Target->TempData.Attr.Action.Dead == true)
				continue;

			//АЛ¬d¬O§_¬°јД¤H
			if (Owner->CheckEnemy(Target) == false)
			{
				if (Target->IsAttack() == false)
					continue;

				if (Owner->CheckCampHelp(Target) == false)
					continue;

				GameObjDbStructEx* TOrgDB = Global::GetObjDB(Target->BaseData.ItemInfo.OrgObjID);
				// ёsІХАЛ¬d
				if (TOrgDB->IsNPC() == false)
					continue;

				switch (TOrgDB->NPC.LinkType)
				{
				case 0:
					continue;

				case 1:
				{
					if (TOrgDB->NPC.RaceGroupType != OrgDB->NPC.RaceGroupType)
						continue;
				}
				break;
				case 2:
					break;
				}

				BaseItemObject* OtherObj;
				RoleDataEx* Other;
				OtherObj = BaseItemObject::GetObj(Target->AttackTargetID());
				if (OtherObj == NULL)
					continue;

				Other = OtherObj->Role();

				float LenOther = Owner->Length3D(Other);

				if (LenOther > OrgDB->NPC.FollowRange)
					continue;

				LenOther = Target->Length3D(Owner);

				//if( LenOther < Owner->TempData.BoundRadiusY + 55 )
			//		LenOther = Owner->TempData.BoundRadiusY + 55;

				if (Owner->CheckEnemy(Other) == false)
					continue;

				if (Owner->CheckPhyAttack(Other) == false)
					continue;

				//				float Len = Owner->Length3D( Target );
				//				if( Len > OrgDB->NPC.SearchRange )
				//					continue;

								//			   if( Owner->Length( Target ) > 100 )
								//				   	continue;
								//(јИ®Й)АЛ¬d¬O§_¦bµшіҐ¤є

				if (OrgDB->NPC.SearchEnemyAllDir == false
					&& LenOther > 60
					&& !Owner->CheckInSight(Target, HelpRange, OrgDB))
					continue;


				float X = Target->X();
				float Y = Target->Y();
				float Z = Target->Z();
				if (Target->IsPlayer())
				{
					X = Target->TempData.Move.CliX;
					Y = Target->TempData.Move.CliY;
					Z = Target->TempData.Move.CliZ;
				}

				if (OwnerObj->Path()->CheckLine(Owner->Pos()->X, Owner->Pos()->Y, Owner->Pos()->Z, X, Y, Z) == false)
				{
					continue;
				}

				return Other;

			}

			if (Owner->CheckPhyAttack(Target) == false)
				continue;

			//(јИ®Й)АЛ¬d¬O§_¦bµшіҐ¤є
			if (!Owner->CheckInSight(Target, 220, OrgDB))
				continue;

			float X = Target->X();
			float Y = Target->Y();
			float Z = Target->Z();
			if (Target->IsPlayer())
			{
				X = Target->TempData.Move.CliX;
				Y = Target->TempData.Move.CliY;
				Z = Target->TempData.Move.CliZ;
			}

			if (OwnerObj->Path()->CheckLine(Owner->Pos()->X, Owner->Pos()->Y, Owner->Pos()->Z, X, Y, Z) == false)
			{
				continue;
			}

			SortTemp.Value = int(Owner->CalRelation(Target));
			SortTemp.Data = Target;

			EnemyList.push_back(SortTemp);

		}

	}
	if (EnemyList.size() == 0)
		return NULL;

	sort(EnemyList.begin(), EnemyList.end());

	Target = (RoleDataEx*)EnemyList[0].Data;

	return Target;
}

//----------------------------------------------------------------------------------------
//	ҐL¤HјvЕT Ґ[HP ©ОҐ[MP
//----------------------------------------------------------------------------------------
void	Global::AddHP(RoleDataEx* underAtk, RoleDataEx* Atk, float DHP, float HateRate)
{
	underAtk->AddHP(Atk, DHP, HateRate);
}
//----------------------------------------------------------------------------------------
void	Global::AddMP(RoleDataEx* underAtk, RoleDataEx* Atk, float DMP, float HateRate)
{
	underAtk->AddMP(Atk, DMP, HateRate);
}
void	Global::AddSP(RoleDataEx* underAtk, RoleDataEx* Atk, float DSP, float HateRate)
{
	underAtk->AddSP(Atk, DSP, HateRate);
}
void	Global::AddSP_Sub(RoleDataEx* underAtk, RoleDataEx* Atk, float DSP, float HateRate)
{
	underAtk->AddSP_Sub(Atk, DSP, HateRate);
}
void    Global::AddStomachPoint(RoleDataEx* Role, int DStomachPoint)
{
	Role->AddStomachPoint(DStomachPoint);
}
//----------------------------------------------------------------------------------------
void	Global::_TpExpCalculate(RoleDataEx* Owner, float _TpExp, int Exp)
{

	if (Owner->PlayerBaseData->PlayTimeQuota < 0 || Owner->BaseData.SysFlag.IsHonorLeader)
	{
		return;
	}
	else if (Owner->PlayerBaseData->PlayTimeQuota < 2 * 60 * 60)
	{
		_TpExp /= 2;
		Exp /= 2;
	}

	int TpExp = int(_TpExp + Owner->PlayerTempData->TempTpDexp);
	Owner->PlayerTempData->TempTpDexp = _TpExp + Owner->PlayerTempData->TempTpDexp - float(TpExp);

	if (Owner->TempData.Attr.Ability->AddTp >= TpExp)
	{
		Owner->AddValue(EM_RoleValue_AddTP, TpExp * -1);
		TpExp *= 2;
	}
	else if (Owner->TempData.Attr.Ability->AddTp > 0)
	{
		TpExp += Owner->TempData.Attr.Ability->AddTp;
		Owner->AddValue(EM_RoleValue_AddTP, Owner->TempData.Attr.Ability->AddTp * -1);
	}
	else
	{
		Owner->TempData.Attr.Ability->AddTp = 0;
	}

	//////////////////////////////////////////////////////////
	//TP­t¶ЕіBІz
	//////////////////////////////////////////////////////////
	{
		int HelfTP = int(TpExp * 0.7f);
		if (HelfTP > 0 && Owner->TempData.Attr.Ability->DebtTP < 0)
		{
			if (HelfTP > Owner->TempData.Attr.Ability->DebtTP * -1)
				HelfTP = Owner->TempData.Attr.Ability->DebtTP * -1;

			Owner->AddValue(EM_RoleValue_DebtTP, (float)HelfTP);
			Owner->Net_DeltaRoleValue(EM_RoleValue_DebtTP, (float)HelfTP);
			TpExp -= HelfTP;
		}

	}


	Owner->AddValue(EM_RoleValue_TpExp, TpExp);
	Owner->Net_DeltaRoleValue(EM_RoleValue_TpExp, (float)TpExp);


	if (Owner->TempData.Attr.Ability->AddExp >= Exp)
	{
		Owner->AddValue(EM_RoleValue_AddExp, Exp * -1);
		Exp *= 2;
	}
	else if (Owner->TempData.Attr.Ability->AddExp > 0)
	{
		Exp += Owner->TempData.Attr.Ability->AddExp;
		Owner->AddValue(EM_RoleValue_AddExp, Owner->TempData.Attr.Ability->AddExp * -1);
	}
	else
	{
		Owner->TempData.Attr.Ability->AddExp = 0;
	}

	//////////////////////////////////////////////////////////
	//ёgЕз­И­t¶ЕіBІz
	//////////////////////////////////////////////////////////
	{
		int HelfExp = int(Exp * 0.7f);
		if (HelfExp > 0 && Owner->TempData.Attr.Ability->DebtExp < 0)
		{
			if (HelfExp > Owner->TempData.Attr.Ability->DebtExp * -1)
				HelfExp = Owner->TempData.Attr.Ability->DebtExp * -1;

			Owner->AddValue(EM_RoleValue_DebtExp, (float)HelfExp);
			Owner->Net_DeltaRoleValue(EM_RoleValue_DebtExp, (float)HelfExp);
			Exp -= HelfExp;
		}
	}

	Owner->Addexp(Exp);

	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < 3; i++)
	{
		GameObjDbStructEx* MagicToolsDB = Global::GetObjDB(Owner->BaseData.EQ.Item[EM_EQWearPos_MagicTool1 + i].OrgObjID);
		ItemFieldStruct* MagicToolItem = Owner->GetEqItem(EM_EQWearPos_MagicTool1 + i);
		if (MagicToolsDB->IsArmor() == false || MagicToolItem->IsEmpty())
		{
			continue;
		}

		vector<MyVMValueStruct> RetList;

		if (strlen(MagicToolsDB->Item.MagicToolsTpExpScript) != 0)
		{
			char ScriptBuf[256];
			sprintf_s(ScriptBuf, sizeof(ScriptBuf), "%s(%d,%d,%d,%d)", MagicToolsDB->Item.MagicToolsTpExpScript, EM_EQWearPos_MagicTool1 + i, MagicToolItem->Durable, Exp, TpExp);
			bool Ret = true;
			if (LUA_VMClass::PCallByStrArg(ScriptBuf, Owner->GUID(), Owner->GUID(), &RetList, 1))
			{

				if (RetList.size() == 1)
				{
					MyVMValueStruct& Temp = RetList[0];
					if (Temp.Type == MyVMValueType_Number)
					{
						if (Temp.Number_Int > MagicToolItem->Quality * MagicToolsDB->Item.Durable)
							Temp.Number_Int = MagicToolItem->Quality * MagicToolsDB->Item.Durable;
						if (Temp.Number_Int < 0)
							Temp.Number_Int = 0;

						bool IsNeedFix = false;
						if (MagicToolItem->Durable != Temp.Number_Int)
						{
							IsNeedFix = true;
							MagicToolItem->Durable = Temp.Number_Int;
						}

						if (IsNeedFix == true)
							Owner->Net_FixItemInfo_EQ(EM_EQWearPos_MagicTool1 + i);
					}
				}
			}
		}

	}
	//////////////////////////////////////////////////////////////////////////

}

RoleDataEx* Global::SearchMaxHit(RoleDataEx* Dead)
{
	RoleDataEx* MaxHitRole = NULL;

	StaticVector< NPCHateStruct, _MAX_HATELIST_SIZE_ >& Hate = Dead->TempData.NPCHate.Hate;

	int MaxHit = -1;
	int MaxHitID = -1;
	for (int i = 0; i < Hate.Size(); i++)
	{
		if (Hate[i].HitPoint > MaxHit)
		{
			RoleDataEx* TempRole = Global::GetRoleDataByGUID(Hate[i].ItemID);
			if (TempRole == NULL)
				continue;

			if (Dead->Length(TempRole) < 500)
			{
				MaxHit = Hate[i].HitPoint;
				MaxHitID = i;
				MaxHitRole = TempRole;
			}
		}
	}
	return MaxHitRole;
}

void    Global::FightAddexp(RoleDataEx* Dead, RoleDataEx* Kill)
{
	if (Dead->OwnerGUID() > 0)
		return;

	RoleDataEx* Owner = Kill;

	if (Owner == NULL)
		return;

	if (Owner->IsNPC())
	{
		Owner = Global::GetRoleDataByGUID(Owner->OwnerGUID());
		if (Owner == NULL)
			return;
	}

	if (Dead->IsPlayer())
		return;

	if (!Owner->IsPlayer())
		return;

	for (int i = 0; i < Owner->TempData.Sys.KillRate; i++)
	{
		//int DLV = Dead->TempData.Attr.Ability->Level - Kill->TempData.Attr.Ability->Level;
		int DLV = Dead->TempData.Attr.Level - Dead->TempData.NPCHate.MaxHitLv;

		GameObjDbStructEx* OrgDB;
		OrgDB = GetObjDB(Dead->BaseData.ItemInfo.OrgObjID);

		float BaseExpRate = 1.0f;
		float BaseTpRate = 1.0f;


		if (OrgDB->IsNPC())
		{
			BaseExpRate = float(OrgDB->NPC.ExpRate) / 100;
			BaseTpRate = float(OrgDB->NPC.TpRate) / 100;
		}
		int		Lv = Dead->TempData.NPCHate.MaxHitLv;
		//int			Lv = Kill->TempData.Attr.Ability->Level;
		if (unsigned(Lv) >= RoleDataEx::JobExp_LvGrowRateTable.size())
			Lv = (int)RoleDataEx::JobExp_LvGrowRateTable.size() - 1;

		BaseExpRate = Owner->GetBaseDLvExp(DLV) * BaseExpRate * RoleDataEx::JobExp_LvGrowRateTable[Lv];
		BaseTpRate = Owner->GetBaseDLvExp(DLV) * BaseTpRate * RoleDataEx::GlobalSetting.TpRate * RoleDataEx::JobExp_LvGrowRateTable[Lv] / 10;

		if (BaseExpRate == 0 && BaseTpRate == 0)
			break;

		BaseTpRate *= (1 + Dead->TempData.Attr.Mid.Body[EM_WearEqType_NPC_TP_Rate] / 100);
		BaseExpRate *= (1 + Dead->TempData.Attr.Mid.Body[EM_WearEqType_NPC_EXP_Rate] / 100);

		if (Owner->BaseData.PartyID == -1)
		{
			float TpExpRate = Owner->TempData.Attr.Mid.Body[EM_WearEqType_TP_Rate] / 100;
			if (TpExpRate >= 5 || TpExpRate <= -1)
				TpExpRate = 0;

			float TpExp = BaseTpRate * (1 + TpExpRate);

			//АЛ¬d¬O§_¦іҐ[­ј
			float ExpRate = Owner->TempData.Attr.Mid.Body[EM_WearEqType_Exp] / 100;
			if (ExpRate >= 5 || ExpRate <= -1)
				ExpRate = 0;

			int Exp = int(BaseExpRate * (1 + ExpRate));

			_TpExpCalculate(Owner, TpExp, Exp);


		}
		else
		{
			PartyExpCal(Owner, Dead->Level(), BaseExpRate, BaseTpRate);
		}
	}

}
//----------------------------------------------------------------------------------------
#define _MAX_RELATION_LV_	10		//Гц«YіМ°ЄµҐЇЕ
bool	Global::AddRelationExp(RoleDataEx* Owner, RoleDataEx* Target)
{
	BaseFriendStruct* OwnerFriend = NULL;
	BaseFriendStruct* TargetFriend = NULL;

	int OwnerPos = Owner->PlayerSelfData->FriendList.FindFriendInfo(EM_FriendListType_FamilyFriend, Target->DBID(), &OwnerFriend);
	if (OwnerPos == -1)
		return false;

	int TargetPos = Target->PlayerSelfData->FriendList.FindFriendInfo(EM_FriendListType_FamilyFriend, Owner->DBID(), &TargetFriend);
	if (TargetPos == -1)
		return false;

	//	float	ExpTable[ 11 ] = { 100,200,400,800,1600,3200,6400,12800,25600,51200,102400 };
	float	ExpTable[11] = { 300,900,2700,8100,24300,72900,218700,656100,1968300,5904900 };
	//	float	ExpTable[ 11 ] = { 10,10,10,10,10,10,10,10,10,10,10 };
	if (OwnerFriend->Lv >= 0 && OwnerFriend->Lv < _MAX_RELATION_LV_)
	{
		int TempLv = int(OwnerFriend->Lv * 100);
		OwnerFriend->Lv += 1.0f / (ExpTable[ TempLv/100 ] / RoleDataEx::GlobalSetting.RelationExpRate);
		if (TempLv != int(OwnerFriend->Lv * 100))
		{
			if (OwnerFriend->Lv > _MAX_RELATION_LV_)
				OwnerFriend->Lv = _MAX_RELATION_LV_;
			Owner->SetValue((RoleValueName_ENUM)(EM_RoleValue_FamilyFriendListLv_Pos + OwnerPos), OwnerFriend->Lv);
		}
	}
	else
	{
		OwnerFriend->Lv = _MAX_RELATION_LV_;
	}

	if (TargetFriend->Lv >= 0 && TargetFriend->Lv < _MAX_RELATION_LV_)
	{
		int TempLv = int(TargetFriend->Lv * 100);
		TargetFriend->Lv += 1.0f / (ExpTable[ TempLv/100 ] / RoleDataEx::GlobalSetting.RelationExpRate);
		if (TempLv != int(TargetFriend->Lv * 100))
		{
			if (TargetFriend->Lv > _MAX_RELATION_LV_)
				TargetFriend->Lv = _MAX_RELATION_LV_;
			Target->SetValue((RoleValueName_ENUM)(EM_RoleValue_FamilyFriendListLv_Pos + TargetPos), TargetFriend->Lv);
		}
	}
	else
	{
		TargetFriend->Lv = _MAX_RELATION_LV_;
	}

	return true;
}
//----------------------------------------------------------------------------------------
void Global::ProcessDead_Honor(RoleDataEx* pDeader, RoleDataEx* pKiller)
{

	int ResetHonorValue = Global::Ini()->ResetHonorValue;


	if (pDeader->IsPlayer() && pKiller->IsPlayer() && pDeader->CheckEnemy(pKiller) && pDeader != pKiller)	// ¦pЄG¦єЄМ¤Ј¬O±ю¦єЄМ, БЧ§K¦Ы±ю¬~ Honor
	{
		// Ґ­¤Аµ№ЄюЄс©Т¦і¤H
		//_Def_View_Block_Range_
		PlayIDInfo** Block = Global::PartSearch(pDeader, _Def_View_Block_RangeTalk_);

		int				i, j;
		PlayID* TopID;

		vector< RoleDataEx* >	vecHateRole;

		int				iRoomID = pDeader->RoomID();

		for (i = 0; Block[i] != NULL; i++)
		{
			TopID = Block[i]->Begin;
			for (j = 0; TopID != NULL; TopID = TopID->Next)
			{
				BaseItemObject* pObj = Global::GetObj(TopID->ID);
				RoleDataEx* pTarget = pObj->Role();

				if (pTarget->IsPlayer() && pTarget->BaseData.RoomID == iRoomID && pDeader->CheckEnemy(pTarget))
				{

					//SendToCli_ByGUID( TopID->ID , iSize , (PVOID)szBuffer );

					for (int i = 0; i < pDeader->TempData.NPCHate.Hate.Size(); i++)
					{
						int iTargetDBID = pTarget->DBID();
						if (pDeader->TempData.NPCHate.Hate[i].DBID == iTargetDBID)
						{
							vecHateRole.push_back(pTarget);
						}
					}
				}
			}
		}

		// °Oі°¦єЄМіQєaЕAА»±ю®Й¶Ў, ¦^ґ_®eЕA»щ­И
		{
			//			__time32_t				NowTime;
			//			struct	tm*				gmt;

			int		iSec = (int)(_time32(NULL) - pDeader->PlayerBaseData->LastBeHonorKilledTime);
			float	HonorRecover = ((float)iSec) * 0.0000116f;

			pDeader->PlayerBaseData->HonorWorth += HonorRecover;

			if (pDeader->PlayerBaseData->HonorWorth > ResetHonorValue)
			{
				pDeader->PlayerBaseData->HonorWorth = float(ResetHonorValue);
			}

			pDeader->PlayerBaseData->LastBeHonorKilledTime = (int)_time32(NULL);
		}

		// ±N Honor ¤Аµ№ІMіж¤¤Єє¤H
		int		iNumPeople = (int)vecHateRole.size();
		float	fAvgHonor = pDeader->PlayerBaseData->HonorWorth / iNumPeople;

		for (vector< RoleDataEx* >::iterator it = vecHateRole.begin(); it != vecHateRole.end(); it++)
		{
			RoleDataEx* pHateRole = *it;

			float		fHonor = (float)pHateRole->GetValue(EM_RoleValue_Honor);

			if (fHonor < 25000)
			{
				if ((fAvgHonor + fHonor) >= 25000)
					fHonor = 25000;
				else
					fHonor += fAvgHonor;

				pHateRole->SetValue(EM_RoleValue_Honor, fHonor);
			}
		}

		if (fAvgHonor >= 1.0)
		{
			for (vector< RoleDataEx* >::iterator it = vecHateRole.begin(); it != vecHateRole.end(); it++)
			{
				RoleDataEx* pHateRole = *it;
				pHateRole->Net_DeltaRoleValue(EM_RoleValue_Honor, fAvgHonor);
			}
		}

		// ¦©°Ј¦єЄМєaЕA»щ­И 10%
		if (iNumPeople != 0)
		{
			pDeader->PlayerBaseData->HonorWorth = pDeader->PlayerBaseData->HonorWorth * 0.7f;
		}


	}
}
//----------------------------------------------------------------------------------------
bool Global::ProcessDead(RoleDataEx* Dead, RoleDataEx* Kill)
{
	if (Kill == NULL)
		Kill = Dead;

	if (Kill->IsNPC())
	{
		RoleDataEx* KillerOwner = Global::GetRoleDataByGUID(Kill->OwnerGUID());
		if (KillerOwner != NULL)
			Kill = KillerOwner;
	}

	if (Dead->IsNPC() /*&& Kill->IsPlayer()*/)
		Kill = Global::SearchMaxHit(Dead);

	if (Kill == NULL)
		Kill = Dead;
	// 1/1/08 KILL ¬° 0 

	if (Dead->BaseData.HP >= 1
		|| Dead->TempData.Attr.Action.Dead)
		return false;

	BaseItemObject* DeadObj = BaseItemObject::GetObj(Dead->GUID());
	if (DeadObj == NULL)
	{
		Print(LV5, "ProcessDead", "DeadObj(%d) not find", Dead->GUID());
		return false;
	}

	//
	//////////////////////////////////////////////////////////////////////////
	//¦pЄG¬OёьЁг¤W­±©Т¦і¤H¤UёьЁг
	vector< AttachObjInfoStruct >& AttachList = DeadObj->AttachList();
	for (unsigned i = 0; i < AttachList.size(); i++)
	{
		if (AttachList[i].IsEmpty())
			continue;
		LuaPlotClass::DetachObj(AttachList[i].ItemID);
	}
	LuaPlotClass::DetachObj(Dead->GUID());


	if (Dead->TempData.Attr.Action.PK == true && Kill->TempData.Attr.Action.PK == true)
	{
		if (Kill->DBID() == Dead->PlayerTempData->PKInfo.TargetDBID)
		{
			NetSrv_PKChild::SetWinLost(Kill, Dead);
			Kill->PlayerTempData->PKInfo.State = EM_PKState_End;
			Dead->PlayerTempData->PKInfo.State = EM_PKState_End;
			//Dead->BaseData.HP = 1;
			Dead->SetValue(EM_RoleValue_HP, 1, NULL);
			Dead->TempData.BackInfo.HP = 0;
			Dead->StopAttack();
			Kill->StopAttack();
			Dead->TempData.UpdateInfo.IsDualDead = true;
			Dead->TempData.NPCHate.Init();
			Kill->TempData.NPCHate.Init();
			//Dead->ClearAllBuff();
			//ІM°ЈBuff
			for (int i = 0; i < Dead->BaseData.Buff.Size(); i++)
			{
				GameObjDbStructEx* Magic = (GameObjDbStructEx*)Dead->BaseData.Buff[i].Magic;
				if (!Magic->IsMagicBase() || Magic->MagicBase.Setting.DeadNotClear == false)
				{
					if (Magic->MagicBase.Effect.Guilty || Magic->MagicBase.Effect.Hunter)
						continue;
					if (Magic->MagicBase.Setting.Dual_NoClearBuff == true)
						continue;
					//Dead->BaseData.Buff.Erase( i );
					Dead->ClearBuffByPos(i);
					i--;
				}
			}
			for (int i = 0; i < Kill->BaseData.Buff.Size(); i++)
			{
				GameObjDbStructEx* Magic = (GameObjDbStructEx*)Kill->BaseData.Buff[i].Magic;
				if (!Magic->IsMagicBase() || Magic->MagicBase.Setting.DeadNotClear == false)
				{
					if (Magic->MagicBase.Effect.Guilty || Magic->MagicBase.Effect.Hunter)
						continue;

					if (Magic->MagicBase.Setting.Dual_NoClearBuff == true)
						continue;

					//Dead->BaseData.Buff.Erase( i );
					Kill->ClearBuffByPos(i);
					i--;
				}
			}
			//і]©wµLјД 3 ¬н
			GameObjDbStructEx* MagicBase = GetObjDB(RoleDataEx::BaseMagicList[EM_BaseMagic_PlayerUnbeatable]);
			if (MagicBase->IsMagicBase() == true && MagicBase->MagicBase.MagicFunc == EM_MAGICFUN_Assist)
			{
				Dead->AssistMagic(Dead, MagicBase->GUID, 0, false, 5);
				NetSrv_MagicChild::SendRange_AddBuffInfo(Dead->GUID(), Dead->GUID(), MagicBase->GUID, 0, 5);
				//				Dead->TempData.UpdateInfo.Recalculate = true;
				Dead->TempData.Attr.Effect.BadMagicInvincible = true;
				Dead->TempData.Attr.Effect.BadPhyInvincible = true;

				Kill->AssistMagic(Kill, MagicBase->GUID, 0, false, 5);
				NetSrv_MagicChild::SendRange_AddBuffInfo(Kill->GUID(), Kill->GUID(), MagicBase->GUID, 0, 5);
				//				Kill->TempData.Attr.Effect.BadMagicInvincible = true;
				Kill->TempData.Attr.Effect.BadPhyInvincible = true;
				Kill->TempData.UpdateInfo.Recalculate_Buff = true;
			}
			return false;
		}
	}

	if (Dead->TempData.Attr.Effect.Insure == true)
	{
		Dead->InsureProc();
		return false;
	}
	////////////////////////////////////////////////////////////////////////////////////////
	LuaPlotClass::DetachObj(Dead->GUID());
	////////////////////////////////////////////////////////////////////////////////////////
	bool IsDead = true;
	//ЇS®нј@±ЎЁЖҐуіBІz
	if (Dead->CheckDeadPlot())
	{
		vector<MyVMValueStruct> RetList;
		if (LUA_VMClass::PCallByStrArg(Dead->TempData.DeadPlot.Begin(), Dead->GUID(), Kill->GUID(), &RetList, 1) == true)
		{
			if (RetList.size() == 1 && RetList[0].Flag == false)
				IsDead = false;
		}
	}

	if (Dead->IsNPC())
	{
		GameObjDbStructEx* DeadDB = Global::GetObjDB(Dead->BaseData.ItemInfo.OrgObjID);
		if (DeadDB->IsNPC() && strlen(DeadDB->NPC.LuaEvent_OnDead) != 0)
		{
			vector<MyVMValueStruct> RetList;
			if (LUA_VMClass::PCallByStrArg(DeadDB->NPC.LuaEvent_OnDead, Dead->GUID(), Kill->GUID(), &RetList, 1) == true)
			{
				if (RetList.size() == 1 && RetList[0].Flag == false)
					IsDead = false;
			}
		}
	}
	if (Kill->IsNPC())
	{
		GameObjDbStructEx* KillDB = Global::GetObjDB(Kill->BaseData.ItemInfo.OrgObjID);
		if (KillDB->IsNPC() && strlen(KillDB->NPC.LuaEvent_OnKill) != 0)
		{
			vector<MyVMValueStruct> RetList;
			if (LUA_VMClass::PCallByStrArg(KillDB->NPC.LuaEvent_OnKill, Kill->GUID(), Dead->GUID(), &RetList, 1) == true)
			{
				if (RetList.size() == 1 && RetList[0].Flag == false)
					IsDead = false;
			}
		}
	}
	if (IsDead == false)
	{
		if (Dead->BaseData.HP <= 0)
			Dead->SetValue(EM_RoleValue_HP, 1, NULL);
		if (Dead->BaseData.MP <= 0)
			Dead->SetValue(EM_RoleValue_MP, 1, NULL);
		Dead->TempData.BackInfo.HP = 0;
		Dead->TempData.BackInfo.MP = 0;
		Dead->StopAttack();
		Kill->StopAttack();
		Dead->StopMoveNow();
		Kill->StopMoveNow();
		Dead->TempData.NPCHate.Init();
		Kill->TempData.NPCHate.Init();
		return false;

	}
	////////////////////////////////////////////////////////////////////////////////////////


	//µІ§ф©Т¦іј@±Ў
	DeadObj->PlotVM()->EndAllLuaFunc();

	Dead->OnDeadBuffProc(Kill);


	Kill->TempData.UpdateInfo.OnKill = true;

	//ІM°ЈBuff
	for (int i = 0; i < Dead->BaseData.Buff.Size(); i++)
	{
		GameObjDbStructEx* Magic = (GameObjDbStructEx*)Dead->BaseData.Buff[i].Magic;
		if (!Magic->IsMagicBase() || Magic->MagicBase.Setting.DeadNotClear == false)
		{
			Dead->ClearBuffByPos(i);
			i--;
		}
	}
	//Dead->BaseData.Buff.Clear();
	Dead->TempData.UpdateInfo.Recalculate_Buff = true;

	Dead->PlayerTempData->Log.DeadCount++;
	Kill->PlayerTempData->Log.KillCount++;

	//§вГdЄ«§R°Ј
	if (Dead->IsPlayer())
	{
		PlayerDead(Dead, Kill);
		Dead->PlayerTempData->BGInfo.iDeadVal++;
	}
	else
	{
		//§діМ¦XІzЄє±ю¦є©ЗЄє¤H
		//Kill = Global::SearchMaxHit( Dead );
		NPCDead(Dead, Kill);

		if (Kill->IsPlayer() && Kill != Dead)
		{
			//CNetUtility::OnEvent_RoleDead( Kill, Dead );	
		}
	}

	// ¶] Server ©wёqј@±Ў
	if (Kill->IsPlayer() && Dead->IsPlayer() && Global::Ini()->LuaFunc_PVP.size() != 0)
	{
		char	szDeadScript[256];

		int		iZoneID = Kill->BaseData.ZoneID % _DEF_ZONE_BASE_COUNT_;
		int		iParallelZoneID = Kill->PlayerTempData->ParallelZoneID;
		int		iRoomID = Kill->RoomID();

		sprintf_s(szDeadScript, sizeof(szDeadScript), "%s( %d, %d, %d, %d, %d )",
			Global::Ini()->LuaFunc_PVP.c_str(),
			Kill->GUID(),
			Dead->GUID(),
			iZoneID,
			iParallelZoneID,
			iRoomID
		);

		LUA_VMClass::PCallByStrArg(szDeadScript, Kill->GUID(), Dead->GUID(), NULL, NULL);
	}

	// ¬O§_¶}±ТєaЕAЁtІО
	// іQ±ю¦є®Й, ±N¦єЄМЁ­¤W HonorWorth ¤Аµ№©PітЄє Player
	if (Global::Ini()->IsAllowHonorSystem == 1)
	{
		ProcessDead_Honor(Dead, Kill);
	}

	if (Dead->IsDead())
	{
		FightAddexp(Dead, Kill);
	}

	if (Kill->IsPlayer()
		&& Dead->IsPlayer()
		&& Kill != Dead)
	{
		Kill->PlayerTempData->BGInfo.iKillVal++;
	}

	Dead->ClearHate();


	return true;
}
//----------------------------------------------------------------------------------------
bool    Global::PlayerDead(RoleDataEx* Dead, RoleDataEx* Kill)
{

	BaseItemObject* OwnerObj = BaseItemObject::GetObj(Dead->GUID());

	if (OwnerObj == NULL)
	{
		Print(LV5, "PlayerDead", "DeadObj(%d) not find", Dead->GUID());
		return false;
	}
	Dead->Dead(true);

	Dead->Pos()->X = Dead->TempData.Move.Tx;
	Dead->Pos()->Y = Dead->TempData.Move.Ty;
	Dead->Pos()->Z = Dead->TempData.Move.Tz;

	//µІ§ф©Т¦іј@±Ў
	OwnerObj->PlotVM()->EndAllLuaFunc();


	Dead->PlayerBaseData->DeadCountDown = 60 * 60;
	Dead->Log_Dead(Kill);

	//і]©w­«ҐНёк®Ж
	Dead->PlayerBaseData->RaiseInfo.Init();

	DeadOptionMode DeadMode;
	DeadMode.AutoRevive = 1;

	if (Dead->TempData.Attr.Effect.Raise == true)
	{
		DeadMode.RaiseMagic = true;
		Dead->PlayerBaseData->RaiseInfo.Mode.MagicRaiseEnabled = true;
		Dead->PlayerBaseData->RaiseInfo.ExpRate = 0;
		//Dead->PlayerBaseData->RaiseInfo.RevX = Dead->X();
		//Dead->PlayerBaseData->RaiseInfo.RevY = Dead->Y();
		//Dead->PlayerBaseData->RaiseInfo.RevZ = Dead->Z();
		Dead->PlayerBaseData->RaiseInfo.RevX = Dead->TempData.Move.CliX;
		Dead->PlayerBaseData->RaiseInfo.RevY = Dead->TempData.Move.CliY;
		Dead->PlayerBaseData->RaiseInfo.RevZ = Dead->TempData.Move.CliZ;

		if (Global::St()->Ini.ReviveScript.size() != 0)
		{
			Dead->PlayerBaseData->RaiseInfo.Mode.ExpProtect = true;
			Dead->PlayerBaseData->RaiseInfo.Mode.EqProtect = true;
		}
	}

	if (Kill->IsPlayer())
	{
		Dead->PlayerBaseData->RaiseInfo.ExpRate = 100;
		Dead->PlayerBaseData->RaiseInfo.Mode.EqProtect = true;
		Dead->PlayerBaseData->RaiseInfo.Mode.ExpProtect = true;
	}

	if (Dead->TempData.Attr.Effect.ExpProtect == true)
	{
		Dead->PlayerBaseData->RaiseInfo.Mode.ExpProtect = true;
	}
	if (Dead->TempData.Attr.Effect.EqProtect)
		Dead->PlayerBaseData->RaiseInfo.Mode.EqProtect = true;


	if (Global::St()->IsDisableNPCReviveTime == true)
	{
		DeadMode.RaiseMagic = true;
		Dead->PlayerBaseData->RaiseInfo.Mode.MagicRaiseEnabled = true;
		Dead->PlayerBaseData->RaiseInfo.RevX = Dead->X();
		Dead->PlayerBaseData->RaiseInfo.RevY = Dead->Y();
		Dead->PlayerBaseData->RaiseInfo.RevZ = Dead->Z();
	}

	Dead->PlayerBaseData->RaiseInfo.ExpDownRate = Global::Ini()->DeadExpDownRate;
	Dead->PlayerBaseData->RaiseInfo.DebtExpRate = Global::Ini()->DeadDebtExpRate;
	Dead->PlayerBaseData->RaiseInfo.DebtTpRate = Global::Ini()->DeadDebtTpRate;
	//іqЄѕ©PітЄ±®a¦№Є«Ґу¦єҐh
//	NetSrv_AttackChild::SendRangeDead( Dead , Kill->GUID() , Kill->GetOrgOwner()->GUID() , Dead->PlayerBaseData->DeadCountDown , DeadMode.Mode );

	//§R°Ј№УёO
	//if( Dead->PlayerSelfData->Tomb.CreateTime + 24*60*60 > TimeStr::Now_Value() )
	if (Dead->PlayerSelfData->Tomb.CreateTime != 0)
	{
		NetSrv_CliConnect::SL_DelTomb(Dead->PlayerSelfData->Tomb.ZoneID, Dead->PlayerSelfData->Tomb.ItemGUID, Dead->DBID());
		Dead->PlayerSelfData->Tomb.Init();
	}
	//ІM°Ј¶gіт¬IЄkҐШјР¬°¦№¤HЄєЄkіN
	vector<BaseSortStruct>* ObjList = SearchRangeObject(Dead, Dead->Pos()->X, Dead->Pos()->Y, Dead->Pos()->Z, EM_SearchRange_All, 1000);

	for (unsigned int i = 0; i < ObjList->size(); i++)
	{
		RoleDataEx* Role = (RoleDataEx*)(*ObjList)[i].Data;

		if (Role->IsSpell() && Role->TempData.Magic.TargetID == Dead->GUID())
		{
			Role->TempData.Magic.State = EM_MAGIC_PROC_STATE_INTERRUPT;
		}

		//§R°ЈГdЄ«©ОЕ@ѕr
		if (Role->TempData.Sys.OwnerDBID == Dead->DBID())
		{
			if (Role->BaseData.Mode.Save == false)
				Role->Destroy("PlayerDead");
		}

	}

	if (Kill->IsPlayer() && Dead->IsPlayer() && Kill != Dead)
	{
		if (Kill->BaseData.GuildID > 0 && Dead->BaseData.GuildID > 0)
		{
			NetSrv_Guild::SD_DeclareWarScore(Kill->BaseData.GuildID, Dead->BaseData.GuildID, Dead->Level(), 1);
		}
	}

	//////////////////////////////////////////////////////////////////
	//Є±®aіQ±ю­p¦ё
	{
		GameObjDbStructEx* KillObjDB = Global::GetObjDB(Dead->BaseData.ItemInfo.OrgObjID);
		if (KillObjDB == NULL)
			return false;

		if (KillObjDB->IsNPC() == true)
		{
			int KillGroupID = KillObjDB->NPC.KillGroupID;

			if ((unsigned)KillGroupID < _MAX_MONSTERHUNT_COUNT_)
			{
				unsigned short& DeadCount = Kill->PlayerSelfData->MonsterHunter.DeadCount[KillGroupID];
				if (DeadCount < 50000)
				{
					DeadCount++;
					//єЩё№АЛ¬d
					vector<GameObjDbStructEx*>& TitleList = g_ObjectData->KillCountTitleInfo(KillGroupID);

					for (unsigned int i = 0; i < TitleList.size(); i++)
					{
						GameObjDbStructEx* TitleDB = TitleList[i];
						if (TitleDB->Title.KillCount.Type != EM_KillCountType_DeadCount)
							continue;
						if (TitleDB->Title.KillCount.Count <= DeadCount)
						{
							Kill->AddTitle(TitleDB->GUID);
						}
					}
				}
			}
			Dead->PlayerSelfData->MonsterHunter.TotalDeadCount_NPC++;
		}
		else
		{
			if (Dead->IsPlayer() && Kill->IsPlayer() && Kill != Dead)
			{
				Dead->PlayerSelfData->MonsterHunter.TotalDeadCount_Player++;
				Kill->PlayerSelfData->MonsterHunter.TotalKillCount_Player++;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	LUA_VMClass::PCallByStrArg("event_module(1)", Dead->GUID(), Kill->GUID());
	//////////////////////////////////////////////////////////////////
	if (Kill->IsPlayer()
		&& Global::Ini()->DisableGoodEvil == false
		&& RoleDataEx::G_PKType == EM_PK_Normal
		&& Global::Ini()->IsPvE == false
		&& Dead->TempData.Attr.Effect.IgnoreDeadGoodEvil == false)
	{
		//јWҐ[PK¦є¤`Єєёк®Ж
		if (Dead->TempData.Attr.Effect.Guilty == false
			&& Dead->TempData.Attr.Effect.Hunter == false
			&& Dead->GUID() != Kill->GUID())
		{
			if (Dead->PlayerTempData->PKDeadTime.Size() == _MAX_PK_DEAD_SAVE_COUNT_)
				Dead->PlayerTempData->PKDeadTime.Erase(0);
			Dead->PlayerTempData->PKDeadTime.Push_Back(TimeStr::Now_Value());
			Dead->TempData.IsPKDead_NotGuilty = true;
		}

		PlayerDead_CalGoodEvil(Dead, Kill);

		if (Kill->CheckGuildWarDeclare(Dead) == false)
		{
			int		TelportRate[9] = { 100,50,40,30,0,0,0,0,0 };
			GoodEvilTypeENUM DeaderType = Dead->GetGoodEvilType();

			if (RoleDataEx::G_ZoneID != g_ObjectData->SysValue().PK_Revive_ZoneID)
			{
				int ZoneID = GlobalBase::GetParallelZoneID(g_ObjectData->SysValue().PK_Revive_ZoneID, OwnerObj->Role()->PlayerTempData->ParallelZoneID);

				if (rand() % 100 < TelportRate[DeaderType + 4] && ZoneID != -1)
				{
					//іqЄѕ©PітЄ±®a¦№Є«Ґу¦єҐh
					NetSrv_AttackChild::SendRangeDead(Dead, Kill->GUID(), Kill->GetOrgOwner()->GUID(), -1, DeadMode.Mode);

					PlayerResuretion(OwnerObj, g_ObjectData->SysValue().PK_Revive_ZoneID
						, g_ObjectData->SysValue().PK_Revive_X
						, g_ObjectData->SysValue().PK_Revive_Y
						, g_ObjectData->SysValue().PK_Revive_Z
						, 0);
					return true;
				}
			}
		}
	}
	if (Global::Ini()->IsAutoRevive == true)
	{
		Dead->PlayerBaseData->DeadCountDown = 2;
		NetSrv_AttackChild::SendRangeDead(Dead, Kill->GUID(), Kill->GetOrgOwner()->GUID(), -1, DeadMode.Mode);
	}
	else
		NetSrv_AttackChild::SendRangeDead(Dead, Kill->GUID(), Kill->GetOrgOwner()->GUID(), Dead->PlayerBaseData->DeadCountDown, DeadMode.Mode);

	if (Dead->TempData.ZoneType != EM_ZoneType_NORMAL)
	{
		Dead->TempData.ZoneType = EM_ZoneType_NORMAL;
		Dead->TempData.ZoneDamgeRate = 0;
		Dead->TempData.DeadZoneCountDown = 0;
		NetSrv_Move::S_DeadZone(Dead->GUID(), EM_ZoneType_NORMAL, Dead->TempData.DeadZoneCountDown);
	}
	return true;
}
//----------------------------------------------------------------------------------------
bool    Global::NPCDead(RoleDataEx* Dead, RoleDataEx* Kill)
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObj(Dead->GUID());

	if (OwnerObj == NULL)
	{
		Print(LV5, "NPCDead", "DeadObj(%d) not find", Dead->GUID());
		return false;
	}

	GameObjDbStructEx* DeadMagicDB = Global::GetObjDB(Dead->TempData.OnDeadMagic);
	if (DeadMagicDB->IsMagicCollect())
	{
		MagicProcInfo  MagicInfo;
		MagicInfo.Init();
		MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
		MagicInfo.TargetID = Kill->GUID();				//ҐШјР
		MagicInfo.MagicCollectID = DeadMagicDB->GUID;		//ЄkіN
		MagicInfo.MagicLv = 0;
		MagicProcessClass::SysSpellMagic(Dead, &MagicInfo);
	}

	Dead->Dead(true);
	Dead->StopMoveNow();
	OwnerObj->Path()->ClearPath();
	Dead->Log_Dead(Kill);
	//Dead->Pos()->Y = PathFind::GetH
//	OwnerObj->Path()->Height( Dead->Pos()->X , Dead->Pos()->Y , Dead->Pos()->Z , Dead->Pos()->Y , 10.0f );
	//////////////////////////////////////////////////////////////////
	//±ю¦є©ЗЄ«­p¦ё
	if (Kill->IsPlayer())
	{
		GameObjDbStructEx* DeadObj = Global::GetObjDB(Dead->BaseData.ItemInfo.OrgObjID);
		if (DeadObj == NULL)
			return false;
		int KillGroupID = DeadObj->NPC.KillGroupID;

		if ((unsigned)KillGroupID < _MAX_MONSTERHUNT_COUNT_)
		{
			unsigned short& KillCount = Kill->PlayerSelfData->MonsterHunter.KillCount[KillGroupID];
			if (KillCount < 50000)
			{
				KillCount++;
				//єЩё№АЛ¬d
				vector<GameObjDbStructEx*>& TitleList = g_ObjectData->KillCountTitleInfo(KillGroupID);

				for (unsigned int i = 0; i < TitleList.size(); i++)
				{
					GameObjDbStructEx* TitleDB = TitleList[i];
					if (TitleDB->Title.KillCount.Type != EM_KillCountType_KillCount)
						continue;
					if (TitleDB->Title.KillCount.Count <= KillCount)
					{
						Kill->AddTitle(TitleDB->GUID);
					}
				}
			}
		}
		Kill->PlayerSelfData->MonsterHunter.TotalKillCount_NPC++;
	}
	//////////////////////////////////////////////////////////////////


	//ІM°Ј¶gіт¬IЄkҐШјР¬°¦№¤HЄєЄkіN
	vector<BaseSortStruct>* ObjList = SearchRangeObject(Dead, Dead->Pos()->X, Dead->Pos()->Y, Dead->Pos()->Z, EM_SearchRange_All, 1000);

	for (unsigned int i = 0; i < ObjList->size(); i++)
	{
		RoleDataEx* Role = (RoleDataEx*)(*ObjList)[i].Data;

		if (Role->IsSpell() && Role->TempData.Magic.TargetID == Dead->GUID())
		{
			Role->TempData.Magic.State = EM_MAGIC_PROC_STATE_INTERRUPT;
		}
	}

	OwnerObj->Path()->ClearPath();

	//іqЄѕ©PітЄ±®a¦№Є«Ґу¦єҐh
	NetSrv_AttackChild::SendRangeDead(Dead, Kill->GUID(), Kill->GetOrgOwner()->GUID(), Dead->PlayerBaseData->DeadCountDown, 0);

	//µІ§ф©Т¦іј@±Ў
	OwnerObj->PlotVM()->EndAllLuaFunc();

	OwnerObj->PlotVM()->CallLuaFunc("Sys01_NPCDead", Kill->GUID(), 0);

	LUA_VMClass::PCallByStrArg("event_module(4)", Dead->GUID(), Kill->GUID());
	OwnerObj->AI()->DeadInit();
	return true;
}
//----------------------------------------------------------------------------------------
void     Global::DropItem(RoleDataEx* Dead, int DLv, int ItemID, bool IsCreateLightPower, ItemFieldStruct* OutItem)
{
	GameObjDbStructEx* DeadObjDB = Global::GetObjDB(Dead->BaseData.ItemInfo.OrgObjID);

	OutItem->Init();
	int Count = 1;
	int TObjID, TCount;
	if (Dead->GetTreasure(ItemID, TObjID, TCount, DLv) == true)
	{
		ItemID = TObjID;
		Count = TCount;
	}

	GameObjDbStructEx* ItemDB = Global::GetObjDB(ItemID);
	if (ItemDB->IsItem() && ItemDB->Item.ItemType == EM_ItemType_PowerLight)
	{
		if (IsCreateLightPower == true)
		{
			int ObjID = Global::CreateObj(ItemID, Dead->Pos()->X, Dead->Pos()->Y, Dead->Pos()->Z, 0, 1);
			RoleDataEx* ItemRole = Global::GetRoleDataByGUID(ObjID);
			if (ItemRole)
				ItemRole->LiveTime(1000 * 3 * 60, "SYS CreateObjІЈҐНЄ««~");

			Global::AddToPartition(ObjID, Dead->RoomID());
		}
		return;
	}

	ItemFieldStruct& Item = *OutItem;

	if (Global::NewItemInit(Item, ItemID, DeadObjDB->GUID) == false)
		return;

	Item.Count = Count;

}

bool     Global::DeadDropItem(int DeadID, int KillID)
{
	vector< ItemFieldStruct > DropItemList;

	float PlayTimeDropRate = 1.0f;

	RoleDataEx* OrgKiller = Global::GetRoleDataByGUID(KillID);
	OrgKiller = OrgKiller->GetOrgOwner();

	if (OrgKiller != NULL && OrgKiller->IsNPC())
	{
		GameObjDbStructEx* OrgKillDB = Global::GetObjDB(OrgKiller->BaseData.ItemInfo.OrgObjID);
		if (OrgKillDB != NULL)
		{
			if (OrgKillDB->NPC.IgnoreNpcKill == false)
				return false;
		}
	}


	int				i;
	RoleDataEx* Dead = Global::GetRoleDataByGUID(DeadID);
	if (Dead == NULL)
		return false;
	int		KillPartyID = Dead->TempData.NPCHate.BeginAtkPartyID;
	int		KillDBID = Dead->TempData.NPCHate.BeginAtkDBID;

	Dead->TempData.NPCHate.BeginAtkPartyID = 0;
	Dead->TempData.NPCHate.BeginAtkDBID = 0;

	RoleDataEx* Kill = Global::GetRoleDataByDBID(KillDBID);

	if (Kill == NULL)
	{
		Kill = Global::GetRoleDataByGUID(KillID);
		if (Kill == NULL)
			Kill = Dead;
	}
	Kill = Kill->GetOrgOwner();


	int	KillRate = Kill->TempData.Sys.KillRate;

	//АЛ¬d Dead ¬O§_¬°NPC
	GameObjDbStructEx* DeadDB;

	DeadDB = Global::GetObjDB(Dead->BaseData.ItemInfo.OrgObjID);

	if (!DeadDB->IsNPC())
	{
		Print(LV3, "DeadDropItem", "DeadDB(%d) not find", Dead->BaseData.ItemInfo.OrgObjID);
		return false;
	}
	if (DeadDB->NPC.DisableTreasureDrop == true)
	{
		CNetSrv_Script::OnEvent_RoleDeadDropItem(Kill, Dead, NULL, NULL, NULL);
		return false;
	}

	vector< int > PartyMember;
	int	DropMoneyExRate = 0;
	int	DropItemExRate = 0;

	int	MaxOrderValue = clock();
	int MinOrderValue = 0x7fffffff;
	int MinOrderDBID = -1;

	RoleDataEx* MinOrderRole = Kill;


	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo(KillPartyID);
	if (KillPartyID != -1)
	{
		if (Party == NULL)
		{
			//	Kill->BaseData.PartyID = -1;			
		}
		else
		{
			for (unsigned int i = 0; i < Party->Member.size(); i++)
			{
				BaseItemObject* M = Global::GetPlayerObj_ByDBID(Party->Member[i].DBID);

				if (M == NULL)
					continue;

				if (Dead->Length(M->Role()) > _MAX_PARTY_SHARE_RANGE_)
					continue;

				if (Dead->RoomID() != M->Role()->RoomID())
					continue;

				PartyMember.push_back(M->Role()->DBID());

				int TreasureRate = int(M->Role()->TempData.Attr.Mid.Body[EM_WearEqType_Treasure] * RoleDataEx::GlobalSetting.DropTreasureRate);
				int MoneyRate = int(M->Role()->TempData.Attr.Mid.Body[EM_WearEqType_DropMoneyRate] * RoleDataEx::GlobalSetting.MoneyRate);
				if (TreasureRate > DropItemExRate)
					DropItemExRate = TreasureRate;
				if (MoneyRate > DropMoneyExRate)
					DropMoneyExRate = MoneyRate;

				if (MaxOrderValue <= M->Role()->PlayerTempData->PartyLootOrder)
					MaxOrderValue = M->Role()->PlayerTempData->PartyLootOrder + 1;

				if (MinOrderValue > M->Role()->PlayerTempData->PartyLootOrder)
				{
					MinOrderValue = M->Role()->PlayerTempData->PartyLootOrder;
					MinOrderDBID = M->Role()->DBID();
					MinOrderRole = M->Role();
				}

			}
		}
	}
	else
	{

		if (Kill == NULL
			|| (Kill->IsPlayer() == false && Kill->BaseData.Mode.IgnoreNpcKill == false))
			return false;

		PartyMember.push_back(Kill->DBID());

		DropMoneyExRate = int(Kill->TempData.Attr.Mid.Body[EM_WearEqType_DropMoneyRate]);
		DropItemExRate = int(Kill->TempData.Attr.Mid.Body[EM_WearEqType_Treasure]);
	}

	ItemFieldStruct Item;

	//±јїъіBІz
	int DropMoney = 0;
	if (DeadDB->NPC.DropMoney_Base != 0)
	{
		if (DeadDB->NPC.DropMoney_Rand <= 0)
			DeadDB->NPC.DropMoney_Rand = 1;
		DropMoney = DeadDB->NPC.DropMoney_Base + rand() % DeadDB->NPC.DropMoney_Rand;
		DropMoney = int(DropMoney * PlayTimeDropRate * RoleDataEx::GlobalSetting.MoneyRate * (100 + DropMoneyExRate) / 100);

		Item.Init();
		Item.ImageObjectID = Item.OrgObjID = g_ObjectData->SysValue().Money_GoldObjID;
		Item.Count = 1;
		Item.Money = DropMoney * KillRate;
		DropItemList.push_back(Item);
	}

	//­pєв§рА»№L¦№©ЗЄ«ЄєЄє°ЄµҐЇЕ
	int DLv = Dead->Level() - Dead->TempData.NPCHate.MaxHitLv;

	float DLvDropRateDesc = 1.0f;
	if (Global::Ini()->IsDisableDropRateDesc == false)
	{
		if (DLv < -5)
			DLvDropRateDesc = 1 + (DLv + 5) * 0.1f;

		if (DLvDropRateDesc < 0.5)
			DLvDropRateDesc = 0.5;
	}

	//	DLvDropRateDesc = DLvDropRateDesc/100;
	//	int	AllPartyDropID = 0;
	//	int AllPartyDropCount = 0;
	vector< ItemFieldStruct > AllPartyDropItem;

	for (int x = 0; x < KillRate; x++)
	{
		//іBІzЄ««~±јёЁ
		for (i = 0; i < _MAX_DROPITEM_COUNT_; i++)
		{
			if (DeadDB->NPC.DropItem[i].OrgID == 0)
				continue;

			int RandValue = Random(100000);
			if (RandValue >= int(DeadDB->NPC.DropItem[i].Rate * DLvDropRateDesc /* RoleDataEx::GlobalSetting.DropTreasureRate*/ * (100 + DropItemExRate) / 100))
				continue;

			int ItemID = DeadDB->NPC.DropItem[i].OrgID;
			DropItem(Dead, DLv, ItemID, (x == 0), &Item);
			if (Item.IsEmpty())
				continue;

			GameObjDbStructEx* DropItemDB = Global::GetObjDB(Item.OrgObjID);
			if (DropItemDB == NULL)
				continue;

			if (DropItemDB->IsItem() && DropItemDB->Mode.ItemDrop_AllParty == true)
			{
				//				AllPartyDropID.push_back( Item.OrgObjID );
				AllPartyDropItem.push_back(Item);
				continue;
			}

			if (DropItemDB->IsItem() && DropItemDB->Mode.ItemDrop_Depart == true)
			{
				ItemFieldStruct DepartItem = Item;
				DepartItem.Count = 1;

				for (int i = 0; i < Item.Count; i++)
				{
					DropItemList.push_back(DepartItem);
				}
				continue;
			}


			if (DropItemDB->MaxHeap != 1)
			{
				for (int i = 0; i < (int)DropItemList.size(); i++)
				{
					if (DropItemList[i].OrgObjID == Item.OrgObjID)
					{
						if (DropItemDB->MaxHeap >= DropItemList[i].Count + Item.Count)
						{
							DropItemList[i].Count += Item.Count;
							Item.Init();
							break;
						}
					}
				}
				if (Item.IsEmpty() == false)
					DropItemList.push_back(Item);
			}
			else
			{
				DropItemList.push_back(Item);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		{
			//ІЕ¤еЄ««~±јёЁ
			static int	DropRune[EM_MagicResist_MAXCOUNT] = { 200850 , 200851 , 200852 , 200853 , 200854 , 200855 };
			// -5 ЇЕЁм +5
			static int	DropRate[11] = { 1 , 2 , 3 , 4 , 5 , 6  , 10 , 20 , 45 , 70  , 90 };

			int DLv = Dead->Level() - Dead->TempData.NPCHate.MaxHitLv + 5;
			if (DLv >= 11)
				DLv = 10;

			if (DLv >= 0 && unsigned(DeadDB->NPC.ResistType) < EM_MagicResist_MAXCOUNT)
			{
				//іBІzЄ««~±јёЁ
				for (i = 0; i < 3; i++)
				{
					if (rand() % 100 > int(DropRate[DLv] * PlayTimeDropRate))
						continue;

					int ItemID = DropRune[DeadDB->NPC.ResistType];
					DropItem(Dead, DLv, ItemID, (x == 0), &Item);

					if (Item.IsEmpty())
						continue;

					GameObjDbStructEx* DropItemDB = Global::GetObjDB(Item.OrgObjID);
					if (DropItemDB == NULL)
						continue;
					if (DropItemDB->MaxHeap != 1)
					{
						for (int i = 0; i < (int)DropItemList.size(); i++)
						{
							if (DropItemList[i].OrgObjID == Item.OrgObjID)
							{
								if (DropItemDB->MaxHeap >= DropItemList[i].Count + Item.Count)
								{
									DropItemList[i].Count += Item.Count;
									Item.Init();
									break;
								}
							}
						}
						if (Item.IsEmpty() == false)
							DropItemList.push_back(Item);
					}
					else
					{
						DropItemList.push_back(Item);
					}
				}
			}

		}

		//////////////////////////////////////////////////////////////////////////
			//іBІzЄ««~±јёЁ
	//		if( Ini()->IsGlobalDrop == true )
		if (DeadDB->NPC.SpecialDropItemType != EM_SpecialDropItemType_NoGlobalDrop)
		{
			vector<int>& GlobalDropList = g_ObjectData->GlobalDropList();

			//¦@ҐО±јёЁ
			for (int i = 0; i < (int)GlobalDropList.size(); i++)
			{

				int ItemID = GlobalDropList[i];
				DropItem(Dead, DLv, ItemID, (x == 0), &Item);

				if (Item.IsEmpty())
					continue;

				GameObjDbStructEx* DropItemDB = Global::GetObjDB(Item.OrgObjID);
				if (DropItemDB == NULL)
					continue;
				if (DropItemDB->MaxHeap != 1)
				{
					for (int i = 0; i < (int)DropItemList.size(); i++)
					{
						if (DropItemList[i].OrgObjID == Item.OrgObjID)
						{
							if (DropItemDB->MaxHeap >= DropItemList[i].Count + Item.Count)
							{
								DropItemList[i].Count += Item.Count;
								Item.Init();
								break;
							}
						}
					}
					if (Item.IsEmpty() == false)
						DropItemList.push_back(Item);
				}
				else
				{
					DropItemList.push_back(Item);
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//°У«°±јёЁ
		if (Ini()->IsGlobalDrop == true && DeadDB->NPC.SpecialDropItemType == EM_SpecialDropItemType_DepartmentStroe)
		{
			vector<int>& GlobalDropList = g_ObjectData->GlobalDropList_DepartmentStroe();

			//¦@ҐО±јёЁ
			for (int i = 0; i < (int)GlobalDropList.size(); i++)
			{

				int ItemID = GlobalDropList[i];
				DropItem(Dead, DLv, ItemID, (x == 0), &Item);

				if (Item.IsEmpty())
					continue;

				GameObjDbStructEx* DropItemDB = Global::GetObjDB(Item.OrgObjID);
				if (DropItemDB == NULL)
					continue;
				if (DropItemDB->MaxHeap != 1)
				{
					for (int i = 0; i < (int)DropItemList.size(); i++)
					{
						if (DropItemList[i].OrgObjID == Item.OrgObjID)
						{
							if (DropItemDB->MaxHeap >= DropItemList[i].Count + Item.Count)
							{
								DropItemList[i].Count += Item.Count;
								Item.Init();
								break;
							}
						}
					}
					if (Item.IsEmpty() == false)
						DropItemList.push_back(Item);
				}
				else
				{
					DropItemList.push_back(Item);
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//Ґю°м±јёЁіBІz
		for (int x = 0; x < 5; x++)
		{
			int ItemID = RoleDataEx::GlobalSetting.GlobalDropTable[x].TableID;

			if (ItemID == 0)
				continue;

			//ј@±ЎАЛ¬d
			if (CheckLuaScript(Dead->GUID(), Kill->GUID(), RoleDataEx::GlobalSetting.GlobalDropTable[x].LuaCheck.Begin()) == false)
				continue;


			DropItem(Dead, DLv, ItemID, false, &Item);

			if (Item.IsEmpty())
				continue;

			GameObjDbStructEx* DropItemDB = Global::GetObjDB(Item.OrgObjID);
			if (DropItemDB == NULL)
				continue;
			if (DropItemDB->MaxHeap != 1)
			{
				for (int i = 0; i < (int)DropItemList.size(); i++)
				{
					if (DropItemList[i].OrgObjID == Item.OrgObjID)
					{
						if (DropItemDB->MaxHeap >= DropItemList[i].Count + Item.Count)
						{
							DropItemList[i].Count += Item.Count;
							Item.Init();
							break;
						}
					}
				}
				if (Item.IsEmpty() == false)
					DropItemList.push_back(Item);
			}
			else
			{
				DropItemList.push_back(Item);
			}

		}

		//////////////////////////////////////////////////////////////////////////
		//іBІzЄ««~±јёЁ
//		if( Ini()->IsGlobalDrop == true )
		{
			vector<int>* TempGlobalDropList = NULL;

			switch (Dead->BaseData.Sex)
			{
			case EM_Sex_BigMonster:		//єл­^
				TempGlobalDropList = &(g_ObjectData->GlobalDropList_BigMonster());
				break;
			case EM_Sex_King:			//¤э
				TempGlobalDropList = &(g_ObjectData->GlobalDropList_King());
				break;
			case EM_Sex_WorldKing:		//Ґ@¬Й¤э
				TempGlobalDropList = &(g_ObjectData->GlobalDropList_WorldKing());
				break;
			}
			if (TempGlobalDropList != NULL)
			{
				vector<int>& GlobalDropList = *TempGlobalDropList;

				//¦@ҐО±јёЁ
				for (int i = 0; i < (int)GlobalDropList.size(); i++)
				{
					int ItemID = GlobalDropList[i];
					DropItem(Dead, DLv, ItemID, false, &Item);

					if (Item.IsEmpty())
						continue;

					GameObjDbStructEx* DropItemDB = Global::GetObjDB(Item.OrgObjID);
					if (DropItemDB == NULL)
						continue;
					if (DropItemDB->MaxHeap != 1)
					{
						for (int i = 0; i < (int)DropItemList.size(); i++)
						{
							if (DropItemList[i].OrgObjID == Item.OrgObjID)
							{
								if (DropItemDB->MaxHeap >= DropItemList[i].Count + Item.Count)
								{
									DropItemList[i].Count += Item.Count;
									Item.Init();
									break;
								}
							}
						}
						if (Item.IsEmpty() == false)
							DropItemList.push_back(Item);
					}
					else
					{
						DropItemList.push_back(Item);
					}
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	vector<int> OwnerDBIDList;
	vector<bool> Lock;
	if (DropItemList.size() != 0)
	{
		if (Party == NULL
			|| (Party->Info.ItemShareType != EM_PartyItemShare_InOrder
				&& Party->Info.ItemShareType != EM_PartyItemShare_Assign))
		{
			for (int i = 0; i < (int)DropItemList.size(); i++)
			{
				OwnerDBIDList.push_back(-1);
				Lock.push_back(false);
			}
		}
		else
		{

			for (int i = 0; i < (int)DropItemList.size(); i++)
			{
				ItemFieldStruct& Item = DropItemList[i];

				GameObjDbStructEx* DropItemDB = Global::GetObjDB(Item.OrgObjID);
				if (DropItemDB == NULL)
					continue;

				if (Party->Info.RollTypeLV <= DropItemDB->Rare)
				{
					OwnerDBIDList.push_back(-1);
					Lock.push_back(false);
				}
				else
				{
					Lock.push_back(false);
					OwnerDBIDList.push_back(MinOrderDBID);
					MinOrderRole->PlayerTempData->PartyLootOrder = MaxOrderValue;
					KillDBID = MinOrderDBID;
				}
			}

		}
	}
	//////////////////////////////////////////////////////////////////////////
	//Ґю Party ±јёЁ
	Item.Init();
	Item.Count = 1;
	for (unsigned i = 0; i < AllPartyDropItem.size(); i++)
	{
		Item.OrgObjID = Item.ImageObjectID = AllPartyDropItem[i].OrgObjID;
		Item.Count = AllPartyDropItem[i].Count;
		for (unsigned j = 0; j < PartyMember.size(); j++)
		{
			OwnerDBIDList.push_back(PartyMember[j]);
			DropItemList.push_back(Item);
			Lock.push_back(true);
		}

		if (PartyMember.size() == 0)
		{
			OwnerDBIDList.push_back(Kill->DBID());
			DropItemList.push_back(Item);
			Lock.push_back(true);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	if (Dead->OwnerGUID() != -1)
	{
		OwnerDBIDList.clear();
		DropItemList.clear();
		Lock.clear();
	}

	// Ґф°ИЄ««~±јёЁґЎ¤J
	CNetSrv_Script::OnEvent_RoleDeadDropItem(Kill, Dead, &DropItemList, &OwnerDBIDList, &Lock);


	//////////////////////////////////////////////////////////////////////////
	//ІЈҐНД_ЅcЎAі]©wёк®Ж
	if (DropItemList.size() != 0)
		return CreateTreasure(KillDBID, KillPartyID, Dead, DropItemList, OwnerDBIDList, Lock);

	return true;
}
//----------------------------------------------------------------------------------------
void     Global::FixAttackMode(RoleDataEx* Owner)
{
	RoleDataEx* Other;
	bool            Result = false;
	NPCHateStruct* HateTB;
	Global::ResetRange(Owner, 3);
	if (Owner->IsDead())
	{
		Owner->IsAttackMode(false);
		return;
	}

	//¦pЄGҐD¤HЄє§рА»јТ¦Ў¬OҐґ¶}Єє ГdЄ«Єє§рА»јТ¦ЎґN¬OҐґ¶}Єє
	RoleDataEx* PetOwner = Global::GetRoleDataByGUID(Owner->OwnerGUID());
	if (PetOwner != NULL)
	{
		if (PetOwner->IsAttackMode() == true)
		{
			Owner->IsAttackMode(true);
			return;
		}
	}

	while ((Other = Global::GetRangeRole()) != NULL)
	{
		if (Other == Owner)
			continue;
		for (int i = 0; i < Other->TempData.NPCHate.Hate.Size(); i++)
		{
			HateTB = &Other->TempData.NPCHate.Hate[i];
			if (HateTB->ItemID == -1)
				break;
			if (HateTB->ItemID == Owner->GUID())
			{
				Owner->IsAttackMode(true);
				return;
			}
		}
	}

	Owner->IsAttackMode(false);
}
//----------------------------------------------------------------------------------------
vector<RoleDataEx*>* Global::SearchRangeChildObject(RoleDataEx* Owner, int GroupID)
{
	static	vector< RoleDataEx* >	SearchList;
	SearchList.clear();
	int					i;

	if (Owner == NULL)
		return &SearchList;

	float X = Owner->X();
	float Y = Owner->Y();
	float Z = Owner->Z();

	int NewSecX = CalSecX(X);
	int NewSecZ = CalSecZ(Z);

	RoleDataEx* Other;
	PlayIDInfo** Block;
	MyMapAreaManagement* Part = Global::Partition(Owner);
	if (Part == NULL)
		return &SearchList;

	Block = Part->Search(
		NewSecX
		, NewSecZ
		, _Def_View_Block_Range_
		, _Def_View_Block_Range_);


	for (i = 0; Block[i] != NULL; i++)
	{
		PlayID* ItemIDList = Block[i]->Begin;

		for (; ItemIDList != NULL; ItemIDList = ItemIDList->Next)
		{
			Other = Global::GetRoleDataByGUID(ItemIDList->ID);
			if (Other == NULL
				|| Other->TempData.Sys.OwnerGUID != Owner->GUID()
				|| Other->TempData.Sys.OwnerGroupID != GroupID)
				continue;

			SearchList.push_back(Other);
		}
	}

	return &SearchList;
}
//----------------------------------------------------------------------------------------
//	·jґM©PітЄ«Ґу(¦^¶Зёк®Ж·|ЁМ¶ZВч±Ж§З)
//----------------------------------------------------------------------------------------
vector<BaseSortStruct>* Global::SearchRangeObject(RoleDataEx* Owner, float X, float Y, float Z, SearchRangeTypeENUM Type, int Range)
{
	static	vector< BaseSortStruct >	SearchList;
	SearchList.clear();

	RoleDataEx* Other = NULL;


	int					i;
	BaseSortStruct		TempSort;

	if (Owner == NULL)
		return &SearchList;


	int NewSecX = CalSecX(X);
	int NewSecZ = CalSecZ(Z);

	PlayIDInfo** Block;
	MyMapAreaManagement* Part = Global::Partition(Owner);
	if (Part == NULL)
		return &SearchList;

	Block = Part->Search(NewSecX
		, NewSecZ
		, Range / Ini()->BlockSize + 1
		, Range / Ini()->BlockSize + 1);


	switch (Type)
	{
	case EM_SearchRange_All:
	{
		for (i = 0; Block[i] != NULL; i++)
		{
			PlayID* ItemIDList = Block[i]->Begin;

			for (; ItemIDList != NULL; ItemIDList = ItemIDList->Next)
			{
				Other = Global::GetRoleDataByGUID(ItemIDList->ID);
				if (Other == NULL)
					continue;

				if (Other->SecRoomID() == -1)
					continue;

				TempSort.Value = int(Other->Length3D(X, Y, Z) - Other->TempData.BoundRadiusY);

				if (TempSort.Value > Range)
					continue;

				TempSort.Data = Other;


				SearchList.push_back(TempSort);
			}
		}
		break;
	}
	case EM_SearchRange_Member:
	{
		TempSort.Value = 0;
		TempSort.Data = Owner;
		SearchList.push_back(TempSort);

		for (int i = 0; i < EM_SummonPetType_Max; i++)
		{
			RoleDataEx* OwnerPet = Global::GetRoleDataByGUID(Owner->TempData.SummonPet.Info[i].PetID);
			if (OwnerPet != NULL && OwnerPet->SecRoomID() != -1)
			{
				int PetLen = int(OwnerPet->Length3D(X, Y, Z) - OwnerPet->TempData.BoundRadiusY);
				if (PetLen < Range)
				{
					TempSort.Value = PetLen;
					TempSort.Data = OwnerPet;
					SearchList.push_back(TempSort);
				}
			}
		}

		PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo(Owner->BaseData.PartyID);

		if (Party != NULL)
		{

			PartyMemberInfoStruct* POwner = NULL;
			//§дҐX¦Ы¤vЄєid
			for (std::vector< PartyMemberInfoStruct >::iterator it = Party->Member.begin(); it != Party->Member.end(); it++)
			{
				if (it->GItemID == Owner->GUID())
				{
					POwner = &(*it);
					break;
				}
			}

			for (std::vector< PartyMemberInfoStruct >::iterator it = Party->Member.begin(); it != Party->Member.end(); it++)
			{
				RoleDataEx* Member = Global::GetRoleDataByGUID((*it).GItemID);

				if (Member == NULL || Member == Owner)
					continue;

				if (Member->SecRoomID() == -1)
					continue;

				if (POwner == NULL)
					break;

				if (POwner->PartyNo / _MAX_PARTY_COUNT_PARTY_ != (*it).PartyNo / _MAX_PARTY_COUNT_PARTY_)
					continue;

				int Len = int(Member->Length3D(X, Y, Z) - Member->TempData.BoundRadiusY);

				if (Len >= Range)
					continue;

				//¬dГdЄ«
				for (int i = 0; i < EM_SummonPetType_Max; i++)
				{
					RoleDataEx* MemberPet = Global::GetRoleDataByGUID(Member->TempData.SummonPet.Info[i].PetID);
					if (MemberPet != NULL && MemberPet->SecRoomID() != -1)
					{
						int PetLen = int(MemberPet->Length3D(X, Y, Z) - MemberPet->TempData.BoundRadiusY);
						if (PetLen < Range)
						{
							TempSort.Value = PetLen;
							TempSort.Data = MemberPet;
							SearchList.push_back(TempSort);
						}
					}
				}

				TempSort.Value = Len;
				TempSort.Data = Member;
				SearchList.push_back(TempSort);
			}
		}
		break;
	}
	case EM_SearchRange_Friend:
	{
		for (i = 0; Block[i] != NULL; i++)
		{
			PlayID* ItemIDList = Block[i]->Begin;

			for (; ItemIDList != NULL; ItemIDList = ItemIDList->Next)
			{

				Other = Global::GetRoleDataByGUID(ItemIDList->ID);
				if (Other == NULL)
					continue;

				if (Other->SecRoomID() == -1)
					continue;

				if (Other->CheckEnemy(Owner) == true)
					continue;

				//ёьЁг¤ЈіBІz
				if (Other->IsWagon() == true)
					continue;

				TempSort.Value = int(Other->Length3D(X, Y, Z) - Other->TempData.BoundRadiusY);

				if (TempSort.Value > Range)
					continue;

				TempSort.Data = Other;

				SearchList.push_back(TempSort);
			}
		}
		break;
	}
	case EM_SearchRange_Enemy:
	{
		for (i = 0; Block[i] != NULL; i++)
		{
			PlayID* ItemIDList = Block[i]->Begin;

			for (; ItemIDList != NULL; ItemIDList = ItemIDList->Next)
			{
				Other = Global::GetRoleDataByGUID(ItemIDList->ID);
				if (Other == NULL)
					continue;

				if (Other->SecRoomID() == -1)
					continue;

				if (Owner->CheckAttackable(Other) != true)
					continue;

				if (Other->TempData.Attr.Effect.RangeAttackInvincible)
					continue;

				TempSort.Value = int(Other->Length3D(X, Y, Z) - Other->TempData.BoundRadiusY);

				if (TempSort.Value > Range)
					continue;

				TempSort.Data = Other;

				SearchList.push_back(TempSort);
			}
		}
		break;
	}
	case EM_SearchRange_Player:
	{
		for (i = 0; Block[i] != NULL; i++)
		{
			PlayID* ItemIDList = Block[i]->Begin;

			for (; ItemIDList != NULL; ItemIDList = ItemIDList->Next)
			{
				Other = Global::GetRoleDataByGUID(ItemIDList->ID);
				if (Other == NULL)
					continue;

				if (Other->IsPlayer() != true)
					continue;

				if (Other->SecRoomID() == -1)
					continue;

				TempSort.Value = int(Other->Length3D(X, Y, Z) - Other->TempData.BoundRadiusY);

				if (TempSort.Value > Range)
					continue;

				TempSort.Data = Other;

				SearchList.push_back(TempSort);
			}
		}
		break;
	}
	case EM_SearchRange_NPC:
	{
		for (i = 0; Block[i] != NULL; i++)
		{
			PlayID* ItemIDList = Block[i]->Begin;

			for (; ItemIDList != NULL; ItemIDList = ItemIDList->Next)
			{
				Other = Global::GetRoleDataByGUID(ItemIDList->ID);
				if (Other == NULL)
					continue;

				if (Other->IsNPC() != true)
					continue;

				if (Other->SecRoomID() == -1)
					continue;

				TempSort.Value = int(Other->Length3D(X, Y, Z) - Other->TempData.BoundRadiusY);

				if (TempSort.Value > Range)
					continue;

				TempSort.Data = Other;

				SearchList.push_back(TempSort);
			}
		}
		break;
	}
	case EM_SearchRange_Item:
	{
		for (i = 0; Block[i] != NULL; i++)
		{
			PlayID* ItemIDList = Block[i]->Begin;

			for (; ItemIDList != NULL; ItemIDList = ItemIDList->Next)
			{
				Other = Global::GetRoleDataByGUID(ItemIDList->ID);
				if (Other == NULL)
					continue;

				if (Other->IsItem() != true)
					continue;

				if (Other->SecRoomID() == -1)
					continue;

				TempSort.Value = int(Other->Length3D(X, Y, Z));

				if (TempSort.Value > Range)
					continue;

				TempSort.Data = Other;

				SearchList.push_back(TempSort);
			}
		}
		break;
	case EM_SearchRange_Player_Enemy:
	{
		for (i = 0; Block[i] != NULL; i++)
		{
			PlayID* ItemIDList = Block[i]->Begin;

			for (; ItemIDList != NULL; ItemIDList = ItemIDList->Next)
			{
				Other = Global::GetRoleDataByGUID(ItemIDList->ID);
				if (Other == NULL)
					continue;

				if (Other->IsPlayer() != true)
					continue;

				if (Other->SecRoomID() == -1)
					continue;

				if (Other->TempData.Attr.Effect.RangeAttackInvincible)
					continue;

				TempSort.Value = int(Other->Length3D(X, Y, Z) - Other->TempData.BoundRadiusY);

				if (TempSort.Value > Range)
					continue;

				if (Owner->CheckEnemy(Other) == false)
					continue;

				TempSort.Data = Other;

				SearchList.push_back(TempSort);
			}
		}
		break;
	}
	case EM_SearchRange_Player_Friend:
	{
		for (i = 0; Block[i] != NULL; i++)
		{
			PlayID* ItemIDList = Block[i]->Begin;

			for (; ItemIDList != NULL; ItemIDList = ItemIDList->Next)
			{
				Other = Global::GetRoleDataByGUID(ItemIDList->ID);
				if (Other == NULL)
					continue;

				if (Other->IsPlayer() != true)
					continue;

				if (Other->SecRoomID() == -1)
					continue;

				TempSort.Value = int(Other->Length3D(X, Y, Z) - Other->TempData.BoundRadiusY);

				if (TempSort.Value > Range)
					continue;

				if (Owner->CheckEnemy(Other) == true)
					continue;

				TempSort.Data = Other;

				SearchList.push_back(TempSort);
			}
		}
		break;
	}

	}

	}

	sort(SearchList.begin(), SearchList.end());

	return &SearchList;

}

//----------------------------------------------------------------------------------------
//Є««~
//----------------------------------------------------------------------------------------
bool     Global::UseItem(int OwnerID, int TargetID, float TargetX, float TargetY, float TargetZ, int Pos, int Type, ItemFieldStruct& Item)
{
	RoleDataEx* Owner = GetRoleDataByGUID(OwnerID);
	if (Owner == NULL)
		return false;

	if (Owner->TempData.Sys.OnChangeZone == true
		|| Owner->TempData.IsDisabledChangeZone == true)
		return false;

	//АЛ¬dЄ««~¬O§_ҐiҐHЁПҐО
	if (Owner->CheckUseItem(TargetID, TargetX, TargetY, TargetZ, Pos, Type, Item) == false)
		return false;

	GameObjDbStructEx* OrgDB;
	OrgDB = GetObjDB(Item.OrgObjID);

	if (OrgDB->IsRecipe())
	{
		if (OrgDB->Recipe.Request.Skill != EM_SkillValueType_None)
		{
			//int SkillLV = int( Owner->TempData.Attr.Fin.SkillValue.GetSkill( OrgDB->Recipe.Request.Skill ) );
			//if( SkillLV < OrgDB->Recipe.Request.SkillLV )
			//	return false;			
		}

		if (Owner->GiveItem(OrgDB->Recipe.Request.KeyItemID, 1, EM_ActionType_UseItemGive, NULL, "") == false)
			return false;

		NetSrv_Recipe::SC_LearnRecipeResult(Owner->GUID(), Item.OrgObjID, EM_LearnRecipeResult_OK);

		Item.Init();
		NetSrv_Item::FixItemInfo(Owner->GUID(), Item, Pos, Type);

		return true;
	}


	if (!OrgDB->IsItem() || Owner->IsSpell() == true)
		return false;



	Owner->TempData.Magic.Init();
	//і]©wЁПҐОЄ««~ёк°T
	Owner->TempData.Magic.UseItem = Item.OrgObjID;
	Owner->TempData.Magic.UseItemPos = Pos;
	Owner->TempData.Magic.UseItemType = Type;

	if (OrgDB->Item.ItemType == EM_ItemType_SummonHorse)
	{
		memcpy(Owner->TempData.Magic.HorseColor, Item.HorseColor, sizeof(Item.HorseColor));
	}

	if (MagicProcessClass::SpellMagic(OwnerID, TargetID, TargetX, TargetY, TargetZ, OrgDB->Item.IncMagic_Onuse, 0) == true)
	{
		Item.Pos = EM_USE_ITEM;

		//ёк®Ж­ЧҐї
		NetSrv_Item::FixItemInfo(Owner->GUID(), Item, Pos, Type);
	}
	else
	{
		Owner->TempData.Magic.ClsUseItem();
		NetSrv_Item::FixItemInfo(Owner->GUID(), Item, Pos, Type);
		return false;
	}

	return true;
}

int      Global::CreateObj_Macro(RoleDataEx* Owner
	, int OrgObjID, const char* Name
	, float x, float y, float z, float dir, int count
	, const char* AutoPlot, const char* ClassName
	, int QuestID, int Mode, int PID, int DBID, float vX, float vY, float vZ, int RoomID)
{
	if (x == 0 && y == 0 && z == 0 && Owner != NULL)
	{
		x = float(int(Owner->Pos()->X));
		y = float(int(Owner->Pos()->Y));
		z = float(int(Owner->Pos()->Z));
		dir = float(int(Owner->Pos()->Dir));
	}
	if (RoomID == -1)
	{
		if (Owner == NULL)
			RoomID = 0;
		else
			RoomID = Owner->RoomID();
	}

	if (count == 0)
		count = 1;

	//------------------------------------------------------------------------------------------------------------


	int ItemID = Global::CreateObj(OrgObjID, x, y, z, dir, count);

	BaseItemObject* NewClass = BaseItemObject::GetObj(ItemID);

	if (NewClass == NULL)
	{
		char Buf[256];
		sprintf_s(Buf, sizeof(Buf), "Create Error!! OrgObjID = %d", OrgObjID);
		Print(Def_PrintLV3, "CreateObj_Macro", Buf);
		return -1;
	}
	if (DBID != -1)
		NewClass->NPCMoveInfo(DBID);

	RoleDataEx* New = NewClass->Role();

	if (Name[0] != 0)
		New->RoleName((char*)Name);

	New->SelfData.AutoPlot = AutoPlot;

	New->SelfData.NpcQuestID = QuestID;
	New->SelfData.PID = PID;
	New->Base.DBID = DBID;
	New->BaseData.vX = vX;
	New->BaseData.vY = vY;
	New->BaseData.vZ = vZ;
	New->RoomID(RoomID);

	if (ClassName[0] != 0)
		New->SelfData.PlotClassName = ClassName;

	if (Mode != 0)
		New->SetMode(Mode);

	GameObjDbStructEx* NpcObjDB = Global::GetObjDB(OrgObjID);
	if (NpcObjDB->IsNPC())
	{
		New->BaseData.Mode.IsAllZoneVisible = NpcObjDB->NPC.IsAllZoneVisible;
		New->BaseData.Mode.DisableRotate = (NpcObjDB->NPC.AutoRotate == false);
	}


	if (New->BaseData.Mode.Save == true && DBID == -1)
	{
		New->TempData.CreateAccount = Owner->Base.Account_ID;
		if (Net_ServerList->CheckServerExist(EM_SERVER_TYPE_DATACENTER) == false)
		{
			Owner->Msg("DataCenter ЁS¶}±ТЎAёк®ЖАx¦s·|¦і°ЭГD");
			return ItemID;
		}
		if (RoleDataEx::G_ZoneID > 1000)
		{
			Owner->Msg("¤А¬y¤ЈҐiҐHАx¦sЄ«Ґу");
			return ItemID;
		}

		Net_DCBase::SaveNPCRequest(New);
		return ItemID;
	}


	// Є«ҐуЇS®ніBІz
	//----------------------------------------------------------------
	switch (New->Base.Type)
	{
	case EM_ObjectClass_Mine:
	{
		CNetSrv_Gather_Child::ResetMineObj(NewClass);
	} break;
	}

	if (AutoPlot[0] != 0)
	{
		NewClass->PlotVM()->CallLuaFunc(AutoPlot, New->GUID(), 0);
	}

	// јЛЄ©Єм©lј@±Ў
	{
		GameObjDbStructEx* pDBObj = Global::GetObjDB(NewClass->Role()->BaseData.ItemInfo.OrgObjID);

		if (pDBObj && (pDBObj->IsNPC() || pDBObj->IsQuestNPC()))
		{
			if (strlen(pDBObj->NPC.szLuaInitScript) != 0)
			{
				NewClass->PlotVM()->CallLuaFunc(pDBObj->NPC.szLuaInitScript, NewClass->GUID());
			}
		}
	}

	//----------------------------------------------------------------------------------
	// Ег№і
	//----------------------------------------------------------------------------------
	Global::AddToPartition(ItemID, RoomID);

	return ItemID;
}
//----------------------------------------------------------------------------------
bool ComNpcDBIDFunction(BaseItemObject* Obj, void* Data)
{
	int DBID = (int)Data;
	if ((Obj->Role()->IsNPC() || Obj->Role()->IsQuestNPC())
		&& Obj->Role()->Base.DBID == DBID)
		return true;
	return false;
}
BaseItemObject* Global::SearchNpcDBID(int NpcDBID)
{
	BaseItemObject* Obj = Search(ComNpcDBIDFunction, (void*)NpcDBID);
	return Obj;
}
//----------------------------------------------------------------------------------
bool    Global::CreateFlag(int FlagOrgObjID, int FlagID, float x, float y, float z, float dir, int DBID, bool IsSave)
{
	if ((unsigned)(FlagOrgObjID - Def_ObjectClass_Flag) >= 10000)
		return false;

	if (FlagID != -1 && (unsigned)FlagID >= 1000)
		return false;

	GameObjDbStructEx* FlagDB = Global::GetObjDB(FlagOrgObjID);
	if (FlagDB == NULL)
		return false;

	int ItemID = CreateObj(FlagOrgObjID, x, y, z, dir, 1);
	if (ItemID == -1)
		return false;

	BaseItemObject* ItemObj = BaseItemObject::GetObj(ItemID);
	RoleDataEx* Item = ItemObj->Role();



	//Item->SelfData.PID = FlagID;

	FlagPosClass::AddFlag(ItemObj, FlagOrgObjID, FlagID, x, y, z, dir, DBID);

	FlagID = ItemObj->Role()->SelfData.PID;

	char Buf[256];
	sprintf_s(Buf, sizeof(Buf), "%s-%03d", FlagDB->Name, FlagID);
	Item->RoleName(Buf);

	FlagPosInfo* Flag = FlagPosClass::GetFlag(FlagOrgObjID, FlagID);
	if (Flag != NULL && Flag->FlagDBID != -1)
		DBID = Flag->FlagDBID;



	if (DBID != -1)
	{
		IsSave = true;
		Item->BaseData.Mode.Save = true;
		Item->Base.DBID = DBID;
	}
	if (IsSave == true)
	{
		Item->BaseData.Mode.Save = true;

		Net_DCBase::SaveNPCRequest(Item);

		if (DBID == -1)
			return true;
	}

	//----------------------------------------------------------------------------------
	// Ег№і
	//----------------------------------------------------------------------------------
	Global::AddToPartition(ItemID, 0);

	return true;

}
//----------------------------------------------------------------------------------
bool    Global::CreateFlag_ShowFlag(int FlagOrgObjID, int FlagID)
{
	FlagPosInfo* FlagInfo = FlagPosClass::GetFlag(FlagOrgObjID, FlagID);

	//int ItemID = CreateObj( FlagOrgObjID + Def_ObjectClass_Flag , FlagInfo->X , FlagInfo->Y , FlagInfo->Z , FlagInfo->Dir , 1 );
	int ItemID = CreateObj(FlagOrgObjID, FlagInfo->X, FlagInfo->Y, FlagInfo->Z, FlagInfo->Dir, 1);
	if (ItemID == -1)
		return false;

	BaseItemObject* ItemObj = BaseItemObject::GetObj(ItemID);
	RoleDataEx* Item = ItemObj->Role();

	FlagInfo->FlagObj = ItemObj;
	Item->Base.DBID = FlagInfo->FlagDBID;
	Item->SelfData.PID = FlagID;

	char Buf[256];
	sprintf_s(Buf, sizeof(Buf), "%s-%03d", Item->RoleName(), FlagID);
	Item->RoleName(Buf);

	//----------------------------------------------------------------------------------
	// Ег№і
	//----------------------------------------------------------------------------------
	Global::AddToPartition(ItemID, 0);

	return true;
}

void    Global::ShowFlag(int FlagID)
{
	FlagPosClass::Show(FlagID);
}
void    Global::HideFlag(int FlagID)
{
	FlagPosClass::Hide(FlagID);
}

//----------------------------------------------------------------------------------------
//	Є±®aЁtІО¦Ы°Кґ_¬Ў
//----------------------------------------------------------------------------------------
void	Global::PlayerResuretion(BaseItemObject* OwnerObj, int ZoneID, float X, float Y, float Z, float ExpDecRate, bool IsNoBuff)
{
	RoleDataEx* Owner = OwnerObj->Role();

	Owner->Dead(false);
	Owner->PlayerBaseData->DeadCountDown = 0;

	//ІM°ЈBuff
	for (int i = 0; i < Owner->BaseData.Buff.Size(); i++)
	{
		GameObjDbStructEx* Magic = (GameObjDbStructEx*)Owner->BaseData.Buff[i].Magic;
		if (!Magic->IsMagicBase() || Magic->MagicBase.Setting.DeadNotClear == false)
		{
			Owner->ClearBuffByPos(i);
			//Owner->BaseData.Buff.Erase( i );
			i--;
		}
	}

	//іQPK±юЄєBuff
	if (Owner->TempData.IsPKDead_NotGuilty == true)
	{
		Owner->TempData.IsPKDead_NotGuilty = false;
		while (1)
		{
			if (Owner->PlayerTempData->PKDeadTime.Size() == 0)
				break;
			if (Owner->PlayerTempData->PKDeadTime[0] + 1200 > (int)TimeStr::Now_Value())
				break;
			Owner->PlayerTempData->PKDeadTime.Erase(0);
		}
		if (Owner->PlayerTempData->PKDeadTime.Size() > 0)
		{
			if (Global::Ini()->IsDisablePKProtectBuff == false)
			{
				int BuffID = RoleDataEx::BaseMagicList[EM_BaseMagic_PKDead1 + Owner->PlayerTempData->PKDeadTime.Size() - 1];

				//
				GameObjDbStructEx* MagicBasePKProtect = Global::GetObjDB(BuffID);
				if (MagicBasePKProtect != NULL
					&& MagicBasePKProtect->MagicBase.CheckUseLua != NULL
					&& strlen(MagicBasePKProtect->MagicBase.CheckUseLua) != 0)
				{
					LUA_VMClass::PCallByStrArg(MagicBasePKProtect->MagicBase.CheckUseLua, Owner->GUID(), Owner->GUID());
				}

				BuffBaseStruct* Buff = Owner->AssistMagic(Owner, BuffID, 0, false, -1);
				if (Buff != NULL)
					NetSrv_MagicChild::SendRange_AddBuffInfo(Owner->GUID(), Owner->GUID(), BuffID, Buff->Power, Buff->Time);
			}
		}

	}

	GameObjDbStructEx* MagicBase;
	if (Owner->Level() >= 10)
	{
		if (IsNoBuff == false && Owner->PlayerBaseData->RaiseInfo.Mode.ExpProtect == false)
		{
			//Ґ[°I­YBuf
			MagicBase = GetObjDB(RoleDataEx::BaseMagicList[EM_BaseMagic_ReviveWeak]);
			if (MagicBase->IsMagicBase() == true && MagicBase->MagicBase.MagicFunc == EM_MAGICFUN_Assist)
			{
				Owner->AssistMagic(Owner, MagicBase->GUID, 0, false, 60);
				NetSrv_MagicChild::SendRange_AddBuffInfo(Owner->GUID(), Owner->GUID(), MagicBase->GUID, 0, 60);
				Owner->TempData.UpdateInfo.Recalculate_Buff = true;
				NetSrv_Other::SC_PlayerEvent(Owner->GUID(), EM_PlayerEventType_WeakMagic);
			}
		}

		if (Owner->PlayerBaseData->RaiseInfo.Mode.ExpProtect == false)
		{
			int	LvExp = Owner->GetLVExp(Owner->BaseData.Voc);
			//¦©5% ёgЕз­И  
//			int	DecExp  = int( LvExp * ExpDecRate * Ini()->DeadExpDownRate / -100 );
//			int	DebtExp = int( LvExp * ExpDecRate * Ini()->DeadDebtExpRate / -100 );
//			int	DebtTP  = int( LvExp * ExpDecRate * Ini()->DeadDebtTpRate / -1000 );

			int	DecExp = int(LvExp * ExpDecRate * Owner->PlayerBaseData->RaiseInfo.ExpDownRate / -100);
			int	DebtExp = int(LvExp * ExpDecRate * Owner->PlayerBaseData->RaiseInfo.DebtExpRate / -100);
			int	DebtTP = int(LvExp * ExpDecRate * Owner->PlayerBaseData->RaiseInfo.DebtTpRate / -1000);

			Owner->AddValue(EM_RoleValue_EXP, float(DecExp));
			Owner->Net_DeltaRoleValue(EM_RoleValue_EXP, float(DecExp));

			Owner->AddValue(EM_RoleValue_DebtExp, float(DebtExp));
			Owner->Net_DeltaRoleValue(EM_RoleValue_DebtExp, float(DebtExp));

			Owner->AddValue(EM_RoleValue_DebtTP, float(DebtTP));
			Owner->Net_DeltaRoleValue(EM_RoleValue_DebtTP, float(DebtTP));


			if (ExpDecRate == 1.0f
				&& (DecExp != 0 || DebtExp != 0 || DebtTP != 0))
			{
				TombStruct& Tomb = Owner->PlayerSelfData->Tomb;
				Tomb.CreateTime = TimeStr::Now_Value();
				Tomb.DebtExp = DebtExp * 30 / 100;
				Tomb.DebtTp = DebtTP * 30 / 100;
				Tomb.Exp = DecExp * 30 / 100;;

				if (Global::Ini()->CreateTombX == 0 && Global::Ini()->CreateTombY == 0 && Global::Ini()->CreateTombZ == 0)
				{
					/*
					Tomb.X			= Owner->X();
					Tomb.Y			= Owner->Y();
					Tomb.Z			= Owner->Z();

					Tomb.ZoneID		= RoleDataEx::G_ZoneID;
					Tomb.ItemGUID = CreateTomb( Owner , Owner->X() , Owner->Y() , Owner->Z() );
					*/
					Tomb.X = Owner->TempData.Move.CliX;
					Tomb.Y = Owner->TempData.Move.CliY;
					Tomb.Z = Owner->TempData.Move.CliZ;

					Tomb.ZoneID = RoleDataEx::G_ZoneID;
					Tomb.ItemGUID = CreateTomb(Owner, Tomb.X, Tomb.Y, Tomb.Z);
				}
				else
				{
					Tomb.X = float(Global::Ini()->CreateTombX);
					Tomb.Y = float(Global::Ini()->CreateTombY);
					Tomb.Z = float(Global::Ini()->CreateTombZ);
					Tomb.ZoneID = Global::Ini()->CreateTombZoneID;
					if (Tomb.ZoneID == 0)
						Tomb.ZoneID = RoleDataEx::G_ZoneID;

					if (Tomb.ZoneID == RoleDataEx::G_ZoneID)
					{
						Tomb.ItemGUID = CreateTomb(Owner, Owner->X(), Owner->Y(), Owner->Z());
						//	NetSrv_CliConnect::SC_TombInfo( Owner->GUID() , Tomb );
					}
				}
				NetSrv_CliConnect::SC_TombInfo(Owner->GUID(), Tomb);

			}
		}

		if (Owner->PlayerBaseData->RaiseInfo.Mode.EqProtect == false)
		{
			//¦©ёЛіЖ­@¤[«Ч 10 %
			//ёЛіЖ­@¤[­pєв
			for (int i = EM_EQWearPos_Head; i < EM_EQWearPos_MaxCount; i++)
			{
				//				if( i == EM_EQWearPos_Ammo )
				//					continue;

				ItemFieldStruct& EqItem = Owner->BaseData.EQ.Item[i];
				if (EqItem.IsEmpty())
					continue;

				GameObjDbStructEx* Item = GetObjDB(EqItem.OrgObjID);
				if (Item == NULL)
					continue;

				int Durable = EqItem.Quality * Item->Item.Durable / 10;

				Owner->EqDurableProcess((EQWearPosENUM)i, Durable);
			}
		}

	}

	if (IsNoBuff == false)
	{
		//і]©wµLјД
		MagicBase = GetObjDB(RoleDataEx::BaseMagicList[EM_BaseMagic_PlayerUnbeatable]);
		if (MagicBase->IsMagicBase() == true && MagicBase->MagicBase.MagicFunc == EM_MAGICFUN_Assist)
		{
			Owner->AssistMagic(Owner, MagicBase->GUID, 0, false, 20);
			NetSrv_MagicChild::SendRange_AddBuffInfo(Owner->GUID(), Owner->GUID(), MagicBase->GUID, 0, 20);
			Owner->TempData.UpdateInfo.Recalculate_Buff = true;
		}

	}

	//Owner->ReCalculateMagicAndEQ( );
	Owner->ReCalculateEQNoBuff();
	Owner->ReCalculateBuff();
	Owner->Calculate();

	//јЖ­И­«і]
	Owner->BaseData.HP = Owner->TempData.Attr.Fin.MaxHP;
	Owner->BaseData.MP = Owner->TempData.Attr.Fin.MaxMP;
	Owner->BaseData.SP = 0;
	Owner->BaseData.SP_Sub = 0;

	if (Owner->ZoneID() == RoleDataEx::G_ZoneID)
		NetSrv_MoveChild::SetPos(Owner->GUID(), Owner);
	NetSrv_AttackChild::SendRangeResurrection(Owner);

	if (ZoneID != -1)
		ChangeZone(Owner->GUID(), ZoneID, -1, X, Y, Z, float(rand() % 360));
}
//----------------------------------------------------------------------------------------
void     Global::SendToOtherWorld_GSrvID(int WorldID, int GSrvID, int Size, void* Data)
{
	_Net->SendToOtherWorld_GSrvType(WorldID, EM_SERVER_TYPE_LOCAL, GSrvID, Size, Data);
}
//----------------------------------------------------------------------------------------
void     Global::SendToOtherWorld_NetID(int WorldID, int NetID, int Size, void* Data)
{
	_Net->SendToOtherWorld_NetID(WorldID, NetID, Size, Data);
}
//----------------------------------------------------------------------------------------
void   Global::SendToOtherWorld_ServerType(int WorldID, EM_SERVER_TYPE iServerType, int Size, void* Data)
{
	//_Net->SendToOtherWorld_GSrvID( WorldID , GSrvID , Size , Data );
	_Net->SendToOtherWorld_GSrvType(WorldID, iServerType, 0, Size, Data);
}
//----------------------------------------------------------------------------------------
void	 Global::SendToOtherWorldZoneClient_ByDBID(int WorldID, int ZoneID, int ClientDBID, int Size, void* Data)
{
	int iDBID = ClientDBID % _DEF_MAX_DBID_COUNT_;
	NetSrv_CliConnect::SBL_SendToZonePlayerByDBID(WorldID, ZoneID, iDBID, Size, Data);
}
//----------------------------------------------------------------------------------------
void	 Global::SendToOtherWorldSrvClient_ByDBID(int WorldID, int SrvID, int ClientDBID, int Size, void* Data)
{
	int iDBID = ClientDBID % _DEF_MAX_DBID_COUNT_;
	NetSrv_CliConnect::SBL_SendToSrvPlayerByDBID(WorldID, SrvID, iDBID, Size, Data);
}
//----------------------------------------------------------------------------------------
struct TempChangeWorldStruct
{
	int GItemID;
	int	WorldID;
	int ZoneID;
	int RoomID;
	float X;
	float Y;
	float Z;
	float Dir;
};
//----------------------------------------------------------------------------------------
struct TempChangeZoneStruct
{
	int GItemID;
	int ZoneID;
	int RoomID;
	float X;
	float Y;
	float Z;
	float Dir;
};

void	Global::SafeChangeZone(int GItemID, int ZoneID, int RoomID, float X, float Y, float Z, float Dir)
{
	//АЛ¬dҐШјРzone ¦іЁS¦і¶}±Т
	if (CheckZoneID(ZoneID) == false)
	{
		ChangeZone(GItemID, 1, 0, -4070, 245, -8248, 47);
		return;
	}
	ChangeZone(GItemID, ZoneID, RoomID, X, Y, Z, Dir);
}
//----------------------------------------------------------------------------------------
//°eёк°Јїщёк°Tµ№Є±®a
void     Global::SendLuaErrorInfo(int OwnerID, int TargetID, const char* FunName, const char* Msg)
{
	RoleDataEx* Role;
	BaseItemObject* Obj;
	set<BaseItemObject* >& PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//­pєвЁC­У©Р¶ЎЄє¤HјЖ


	//char szFunName[256];
	char szError[1024];
	//LUA_VMClass::GetProcFuncName( szFunName );
	sprintf_s(szError, sizeof(szError), "LuaFunc: %s", FunName);

	for (PlayerObjIter = PlayerObjSet.begin(); PlayerObjIter != PlayerObjSet.end(); PlayerObjIter++)
	{
		Obj = *PlayerObjIter;
		if (Obj == NULL)
			continue;

		Role = Obj->Role();
		if (Role->TempData.Sys.LuaDebugMsg == false)
			continue;
		Role->Msg(szError);
	}


	for (PlayerObjIter = PlayerObjSet.begin(); PlayerObjIter != PlayerObjSet.end(); PlayerObjIter++)
	{
		Obj = *PlayerObjIter;
		if (Obj == NULL)
			continue;

		Role = Obj->Role();
		if (Role->TempData.Sys.LuaDebugMsg == false)
			continue;
		Role->Msg(Msg);
	}



	sprintf_s(szError, sizeof(szError), "LuaFunc: %s - %s", FunName, Msg);
	szError[1023] = 0;
	for (int i = 0; ; i++)
	{
		if (szError[i] == 0)
			break;
		if (szError[i] == '%')
			szError[i] = '#';
	}

	RoleDataEx* pRoleOwner = Global::GetRoleDataByGUID(OwnerID);
	RoleDataEx* pRoleTarget = Global::GetRoleDataByGUID(OwnerID);

	int			OwnerObjID = 0;
	int			TargetObjID = 0;

	if (pRoleOwner != NULL)
		OwnerObjID = pRoleOwner->BaseData.ItemInfo.OrgObjID;

	if (pRoleTarget != NULL)
		TargetObjID = pRoleTarget->BaseData.ItemInfo.OrgObjID;



	Print(LV5, "LUAERROR", "O:%d[%d], T:%d[%d], %s", OwnerID, OwnerObjID, TargetID, TargetObjID, szError);
}
//----------------------------------------------------------------------------------------
int		Global::CreatePet(int OwnerID, int OrgObjID, char* Name, int LV, SummonPetTypeENUM PetType, int MagicBaseID)
{
	RoleDataEx* Owner;
	RoleDataEx* Pet;
	int				PetID;

	if (PetType >= EM_SummonPetType_Max)
		return -1;

	Owner = (RoleDataEx*)Global::GetRoleDataByGUID(OwnerID);
	if (Owner == NULL)
	{
		Print(LV1, "CreatePet", "OwnerID(%d) Not Find!!", OwnerID);
		return -1;
	}

	PetStruct& SummonPet = Owner->TempData.SummonPet.Info[PetType];

	float Angle = float(rand() % 628) / 100;

	float NX = Owner->Pos()->X + sin(Angle) * 15;
	float NZ = Owner->Pos()->Z + cos(Angle) * 15;

	PetID = CreateObj(OrgObjID, NX, Owner->Pos()->Y, NZ, Owner->Pos()->Dir, 1);

	BaseItemObject* PetObj = Global::GetObj(PetID);
	if (PetObj == NULL)
	{
		Print(LV1, "CreatePet", "Pet Object(%d) Create Error!!", OrgObjID);
		return -1;
	}

	//§в¤§«eЄєГdЄ«§R°Ј
	BaseItemObject* OldPetObj = Global::GetObj(SummonPet.PetID);
	if (OldPetObj != NULL && OldPetObj != PetObj && OldPetObj->Role()->TempData.Sys.OwnerDBID == Owner->DBID())
	{
		OldPetObj->Destroy("SYS CreatePet");
		SummonPet.PetID = -1;

		if (PetType == EM_SummonPetType_CultivatePet)
		{
			//§в©Т¦іЄєГdЄ«ёк®Жі]©w­ЧҐї
			for (int i = 0; i < MAX_CultivatePet_Count; i++)
			{
				CultivatePetStruct& PetBase = Owner->PlayerBaseData->Pet.Base[i];
				if (PetBase.EventType == EM_CultivatePetCommandType_Summon)
				{
					PetBase.EventType = EM_CultivatePetCommandType_None;

					RoleValueName_ENUM ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * i + EM_CultivatePetStructValueType_EventType);
					Owner->Net_FixRoleValue(ValueType, PetBase.EventType);
				}
				Owner->TempData.UpdateInfo.Recalculate_All = true;
			}
		}

	}

	Pet = PetObj->Role();

	Pet->TempData.Sys.OwnerGUID = OwnerID;
	Pet->TempData.Sys.OwnerDBID = Owner->DBID();
	Pet->TempData.Sys.IsPet = true;
	Pet->TempData.SummonPet.Type = PetType;
	Pet->TempData.SummonPet.MagicBaseID = MagicBaseID;

	SummonPet.Action = EM_PET_FOLLOW;
	SummonPet.PetID = PetID;
	SummonPet.TargetID = -1;
	SummonPet.MagicID = MagicBaseID;

	GameObjDbStructEx* MagicBaseDB = Global::GetObjDB(MagicBaseID);
	if (MagicBaseDB->IsMagicBase() == true)
	{
		Pet->TempData.Attr.Ability->STR = int(Owner->TempData.Attr.Fin.STR * MagicBaseDB->MagicBase.SummonCreature.OwnerPowerRate);
		Pet->TempData.Attr.Ability->STA = int(Owner->TempData.Attr.Fin.STA * MagicBaseDB->MagicBase.SummonCreature.OwnerPowerRate);
		Pet->TempData.Attr.Ability->INT = int(Owner->TempData.Attr.Fin.INT * MagicBaseDB->MagicBase.SummonCreature.OwnerPowerRate);
		Pet->TempData.Attr.Ability->MND = int(Owner->TempData.Attr.Fin.MND * MagicBaseDB->MagicBase.SummonCreature.OwnerPowerRate);
		Pet->TempData.Attr.Ability->AGI = int(Owner->TempData.Attr.Fin.AGI * MagicBaseDB->MagicBase.SummonCreature.OwnerPowerRate);
	}

	if (LV > 0)
	{
		Pet->SetValue(EM_RoleValue_LV, LV, NULL);
	}
	Pet->InitCal();

	Pet->BaseData.RoleName = Name;
	Pet->BaseData.Mode.Revive = false;
	Pet->BaseData.Mode.Searchenemy = false;
	Pet->BaseData.Mode.IsPet = true;

	if (PetType == EM_SummonPetType_Normal)
	{
		Pet->BaseData.SysFlag.IsContorlPet = true;
	}

	AddToPartition(PetID, Owner->BaseData.RoomID);

	NetSrv_Magic::S_CreatePet(Owner->GUID(), Pet->GUID(), Pet->WorldGUID(), OrgObjID, MagicBaseID, Pet->TempData.SummonPet.Type);

	PetObj->SimpleSchedularReg(PetProc);

	Owner->SetPetSkill();
	return PetID;
}
//----------------------------------------------------------------------------------------
int		Global::CreateGuard(int OwnerID, int OrgObjID, char* Name, int LV, bool CanFight, int GroupID, int MagicBaseID)
{
	RoleDataEx* Owner;
	RoleDataEx* Pet;
	int				PetID;

	Owner = (RoleDataEx*)Global::GetRoleDataByGUID(OwnerID);
	if (Owner == NULL)
	{
		Print(LV1, "CreateGuard", "OwnerID(%d) Not Find!!", OwnerID);
		return -1;
	}

	float Angle = float(rand() % 628) / 100;

	float NX = Owner->Pos()->X + sin(Angle) * 15;
	float NZ = Owner->Pos()->Z + cos(Angle) * 15;

	//----------------------------------------------------------------------------------------
	/*
	//·jґM©Pіт¬O§_¦і¦№GroupЄє¤¬ѕr
	RoleDataEx *TempNPC;

	Global::ResetRange( Owner , _Def_View_Block_Range_ );

	while( (TempNPC = Global::GetRangeRole()) != NULL )
	{
		if( !TempNPC->IsNPC() )
			continue;
		if( TempNPC->SelfData.PID == GroupID && TempNPC->TempData.Sys.OwnerDBID == Owner->DBID() )
			TempNPC->Destroy( "CreateGuard" );
	}
	*/

	set<BaseItemObject* >& NpcObjSet = *(BaseItemObject::NPCObjSet());
	set< BaseItemObject*>::iterator   NpcObjIter;
	for (NpcObjIter = NpcObjSet.begin(); NpcObjIter != NpcObjSet.end(); NpcObjIter++)
	{
		BaseItemObject* Obj = *NpcObjIter;
		if (Obj == NULL)
			continue;

		RoleDataEx* TempNPC = Obj->Role();
		if (TempNPC->SelfData.PID == GroupID && TempNPC->OwnerGUID() == Owner->GUID())
			TempNPC->Destroy("CreateGuard");
	}

	//----------------------------------------------------------------------------------------

	PetID = CreateObj(OrgObjID, NX, Owner->Pos()->Y, NZ, Owner->Pos()->Dir, 1);

	BaseItemObject* PetObj = Global::GetObj(PetID);
	if (PetObj == NULL)
	{
		Print(LV1, "CreateGuard", "Pet Object(%d) Create Error!!", OrgObjID);
		return -1;
	}

	Pet = PetObj->Role();

	Pet->TempData.Sys.OwnerGUID = OwnerID;
	Pet->TempData.Sys.OwnerDBID = Owner->DBID();
	Pet->SelfData.PID = GroupID;
	//Owner->TempData.Pet.Action  = EM_PET_FOLLOW;
	//Owner->TempData.Pet.PetID	= PetID;
	//Owner->TempData.Pet.TargetID= -1;
	GameObjDbStructEx* MagicBaseDB = Global::GetObjDB(MagicBaseID);
	if (MagicBaseDB->IsMagicBase() == true)
	{
		Pet->TempData.Attr.Ability->STR = int(Owner->TempData.Attr.Fin.STR * MagicBaseDB->MagicBase.SummonCreature.OwnerPowerRate);
		Pet->TempData.Attr.Ability->STA = int(Owner->TempData.Attr.Fin.STA * MagicBaseDB->MagicBase.SummonCreature.OwnerPowerRate);
		Pet->TempData.Attr.Ability->INT = int(Owner->TempData.Attr.Fin.INT * MagicBaseDB->MagicBase.SummonCreature.OwnerPowerRate);
		Pet->TempData.Attr.Ability->MND = int(Owner->TempData.Attr.Fin.MND * MagicBaseDB->MagicBase.SummonCreature.OwnerPowerRate);
		Pet->TempData.Attr.Ability->AGI = int(Owner->TempData.Attr.Fin.AGI * MagicBaseDB->MagicBase.SummonCreature.OwnerPowerRate);
	}

	if (LV > 0)
	{
		Pet->SetValue(EM_RoleValue_LV, LV, NULL);
	}

	Pet->InitCal();

	Pet->BaseData.RoleName = Name;
	Pet->BaseData.Mode.Revive = false;
	Pet->BaseData.Mode.Searchenemy = false;
	Pet->BaseData.Mode.Fight = CanFight;
	Pet->BaseData.Mode.Mark = CanFight;

	//SearchNode( Owner , Pet->BaseData.Pos.X , Pet->BaseData.Pos.Z );
	AddToPartition(PetID, Owner->BaseData.RoomID);

	PetObj->SimpleSchedularReg(PetProc);

	return PetID;
}
//----------------------------------------------------------------------------------------
void     Global::PartyExpCal(RoleDataEx* Owner, int DeadLv, float Exp, float Tp)
{
	static float  ExpRate[] = { 1.0f , 1.0f , 1.25f , 1.5f , 1.75f , 2.0f , 2.25f ,
									1.0f , 1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,
									1.0f , 1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,
									1.0f , 1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,
									1.0f , 1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,
									1.0f , 1.0f ,1.0f ,1.0f ,1.0f };

	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo(Owner->BaseData.PartyID);
	if (Party == NULL
		|| Party->Member.size() == 0)
	{
		Owner->BaseData.PartyID = -1;
		return;
	}
	if (Exp == 0)
		return;

	StaticVector< RoleDataEx*, _MAX_PARTY_COUNT_  > Member;
	Member.Push_Back(Owner);

	for (unsigned int i = 0; i < Party->Member.size(); i++)
	{
		BaseItemObject* MObj = BaseItemObject::GetObjByDBID(Party->Member[i].DBID);
		if (MObj == NULL)
			continue;

		RoleDataEx* M = MObj->Role();
		if (M == Owner)
			continue;
		if (Owner->Length(M) > _MAX_PARTY_SHARE_RANGE_)
			continue;

		if (M->IsDead())
			continue;

		if (M->Level() - Owner->Level() > 20)
			continue;

		if (M->RoomID() != Owner->RoomID())
			continue;

		Member.Push_Back(M);
	}

	if (Party->Info.Type == EM_PartyType_Raid)
	{
		Exp = Exp / Member.Size();
		Tp = Tp / Member.Size();
	}
	else
	{
		Exp = Exp * ExpRate[Member.Size()] / Member.Size();
		Tp = Tp * ExpRate[Member.Size()] / Member.Size();
	}

	//Гц«YёgЕз­И­pєв
	for (int i = 0; i < Member.Size(); i++)
	{
		for (int j = i + 1; j < Member.Size(); j++)
		{
			AddRelationExp(Member[i], Member[j]);
		}
	}



	for (int i = 0; i < Member.Size(); i++)
	{
		if (Member[i]->BaseData.SysFlag.IsHonorLeader == true)
			continue;

		int DLV = Member[i]->Level() - DeadLv;
		//АЛ¬d¬O§_¦іҐ[­ј
		float ExpRate = Member[i]->TempData.Attr.Mid.Body[EM_WearEqType_Exp] / 100;
		if (ExpRate >= 5 || ExpRate <= -1)
			ExpRate = 0;

		float TpExpRate = Member[i]->TempData.Attr.Mid.Body[EM_WearEqType_TP_Rate] / 100;
		if (TpExpRate >= 5 || TpExpRate <= -1)
			TpExpRate = 0;

		if (DLV <= 0)
		{
			float TpExp = (Tp * (1 + TpExpRate));
			int TempExp = (int)(Exp * (1 + ExpRate));
			_TpExpCalculate(Member[i], TpExp, TempExp);
		}
		else
		{
			float TpExp = (Tp * (1 - 0.04f * DLV + TpExpRate));

			if (DLV > 20)
				DLV = 20;

			int TempExp = (int)(Exp * (1 - 0.04f * DLV + ExpRate));
			_TpExpCalculate(Member[i], TpExp, TempExp);
		}
	}
	/*
		//////////////////////////////////////////////////////////////////////////
		//Гц«YёgЕз­И­pєв
		float	RelationExpTable[11] = { 100 , 200 , 400 , 800 , 1600 , 3200 , 6400 , 12800 , 25600 , 51200 , 51200 }
		if( Party->Info.Type == EM_PartyType_Party )
		{
			BaseFriendStruct* FriendInfo;

			for( int i = 0 ; i < Member.Size() ; i++ )
			{
				RoleDataEx* M1 = Member[i];
				for( int j = 0 ; j < Member.Size() ; j++ )
				{
					RoleDataEx* M2 = Member[i];
					if( M1 == M2 )
						continue;

					int RetPos = M1->PlayerSelfData->FriendList.FindFriendInfo( EM_FriendListType_FamilyFriend , M2->DBID() , &FriendInfo );
					if( RetPos == -1 )
						continue;

					if( FriendInfo.Lv >= 10 )
					{
						FriendInfo.Lv = 10;
					}

					FriendInfo.Lv =
				}
			}
		}
		*/
		//////////////////////////////////////////////////////////////////////////
}
//----------------------------------------------------------------------------------------
BaseItemObject* Global::GetNPCObj_ByDBID(int NPCDBID)
{
	BaseItemObject* Obj;

	set<BaseItemObject* >& NpcObjSet = *(BaseItemObject::NPCObjSet());
	set< BaseItemObject*>::iterator   NpcObjIter;
	for (NpcObjIter = NpcObjSet.begin(); NpcObjIter != NpcObjSet.end(); NpcObjIter++)
	{
		Obj = *NpcObjIter;
		if (Obj == NULL)
			continue;

		if (Obj->Role()->DBID() == NPCDBID)
			return Obj;
	}

	return NULL;
}
BaseItemObject* Global::GetPlayerObj_ByDBID(int PlayerDBID)
{
	return BaseItemObject::GetObjByDBID(PlayerDBID);
}

int	Global::GetOrgObjID(int ID)
{
	RoleDataEx* pRole = GetRoleDataByGUID(ID);

	if (pRole != NULL)
	{
		return pRole->BaseData.ItemInfo.OrgObjID;
	}
}

//----------------------------------------------------------------------------------------
int		Global::ResetObjProc(SchedularInfo* Obj, void* InputClass)
{
	int					iGUID = (int)InputClass;
	BaseItemObject* pObj = BaseItemObject::GetObj(iGUID);
	GameObjDbStructEx* OrgDB = NULL;

	if (pObj == NULL)
		return 0;

	OrgDB = Global::GetObjDB(pObj->Role()->BaseData.ItemInfo.OrgObjID);
	if (OrgDB == NULL)
		return 0;

	if (OrgDB->IsMine())
	{
		AddToPartition(iGUID, pObj->Role()->BaseData.RoomID);
		Print(LV2, "Gather", "Reset-AddMine[ %d ] RoomID[ %d ], ( %f, %f, %f )", iGUID, pObj->Role()->BaseData.RoomID, pObj->Role()->BaseData.Pos.X, pObj->Role()->BaseData.Pos.Y, pObj->Role()->BaseData.Pos.Z);
	}
	else
		if (!(OrgDB->IsQuestNPC() && OrgDB->NPC.iQuestMode != 0 && OrgDB->NPC.iQuestObjType == 1))
		{
			AddToPartition(iGUID, pObj->Role()->BaseData.RoomID);
		}

	if (pObj->Role()->IsDisableClick() == true)
	{
		pObj->Role()->SetDisableClick(false);
	}

	return 0;
}
//----------------------------------------------------------------------------------------
void Global::ResetObj(int iObjGUID)
{

	BaseItemObject* OwnerObj = BaseItemObject::GetObj(iObjGUID);

	if (OwnerObj == NULL)
	{
		return;
	}

	RoleDataEx* Owner = OwnerObj->Role();
	GameObjDbStructEx* OrgDB = Global::GetObjDB(Owner->BaseData.ItemInfo.OrgObjID);
	if (OrgDB->IsEmpty())
		return;

	OwnerObj->Path()->ClearPath();

	//Owner->InitNPCData();

	Owner->BaseData.Pos = Owner->SelfData.RevPos;
	//­pєв­«ҐН®Й¶Ў

	Owner->InitCal();

	if (!(OrgDB->IsQuestNPC() && OrgDB->NPC.iQuestMode != 0 && OrgDB->NPC.iQuestObjType == 1))
	{
		// ҐэЕэЄ«Ґу±q Server Іѕ°Ј
		DelFromPartition(iObjGUID);
	}


	// ±А¤JЁЖҐуЕэ Obj ­«·sҐ[¤J©О¶}±Т§@ҐО
	int iEventHandle = Schedular()->Push(ResetObjProc, (OrgDB->ReviveTime * 1000), (VOID*)iObjGUID, NULL);

}



/*
//іBІz§рА»¤¤©ЗЄ«±№¶}
void     Global::_ProcFightNPCPos( )
{

	BaseItemObject*	Obj;
	map< int , vector< RoleDataEx* > >	NpcGroup;

	map< int , vector< RoleDataEx* > >::iterator Iter;

	for(    Obj = BaseItemObject::GetByOrder_NPC( true )
		;   Obj != NULL
		;   Obj = BaseItemObject::GetByOrder_NPC( ) )
	{
		if( Obj->Role()->IsAttack() == false )
			continue;

		NpcGroup[ Obj->Role()->TargetID() ].push_back( Obj->Role() );
	}

	float Dx;
	float Dy;
	float Radius;
	for( Iter = NpcGroup.begin() ; Iter != NpcGroup.end() ; Iter++ )
	{
		vector< RoleDataEx*>& TVecotr = Iter->second;

		for( int i = TVecotr.size() - 1 ; i >= 0 ; i-- )
			for( int j = 0 ; j < i ; j++ )
			{
				//­pєвЁв°П¶ф¶ZВч
				Dx = TVector[i]->TempData.Move.Tx - TVector[j]->TempData.Move.Tx;
				Dz = TVector[i]->TempData.Move.Tz - TVector[j]->TempData.Move.Tz;
				Dx = Dx * Dx;
				Dz = Dz * Dz;

				Radius = ( TVector[i]->TempData.BoundRadius + TVector[j]->TempData.BoundRadius ) /2;
				if( Radius * Radius > Dx + Dz )
				{

					break;
				}
			}
	}

}
*/
//----------------------------------------------------------------------------------------
#if 0
void	Global::Log_GMToolsGMCommand(const char* Account, int ManageLv, int ToRoleDBID, const char* Command)
{
	if (Global::Ini()->IsSaveLog == false)
		return;

	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String(Command);
	string CmdBinStr = StringToSqlX((char*)charTransfer.GetWCString(), charTransfer.WCStrLen() * 2, false);

	string AccountX = StringToSqlX(Account, _MAX_ACCOUNT_SIZE_);

	char Buf[2048];
	sprintf_s(Buf, sizeof(Buf), "INSERT INTO GMToolsCommandLog( Account , ZoneID , ManageLv, ToRoleDBID, Command) VALUES (CAST( %s AS varchar(65) ) ,%d, %d, %d, CAST( %s AS nvarchar(4000) ) )"
		, AccountX
		, RoleDataEx::G_ZoneID
		, ManageLv
		, ToRoleDBID
		, CmdBinStr.c_str());


	Net_DCBase::LogSqlCommand(Buf);

}
void	Global::Log_GMCommand(RoleDataEx* Owner, const char* GMCommand, const char* RoleName)
{
	if (Global::Ini()->IsSaveLog == false)
		return;

	if (Owner->IsPlayer() == false)
		return;

	//§вUnicode Utf-8 Ва¬° char*
	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String(GMCommand);
	string CmdBinStr = StringToSqlX((char*)charTransfer.GetWCString(), charTransfer.WCStrLen() * 2, false);

	char Buf[2048];
	sprintf_s(Buf, sizeof(Buf), "INSERT INTO GMCommandLog( PlayerDBID , ZoneID, PosX, PosZ, Command) VALUES ( %d ,%d, %d, %d, CAST( %s AS nvarchar(4000) ) )"
		, Owner->DBID()
		, Owner->G_ZoneID
		, int(Owner->Pos()->X)
		, int(Owner->Pos()->Z)
		, CmdBinStr.c_str());


	Net_DCBase::LogSqlCommand(Buf);

}

void	Global::Log_ItemTrade(int FromPlayerDBID, int FromNpcDBID, int ToPlayerDBID, int X, int Z, ActionTypeENUM Type, ItemFieldStruct& Item, const char* Note)
{
	GameObjDbStructEx* ItemObj = Global::GetObjDB(Item.OrgObjID);
	if (ItemObj == NULL)
		return;

	if (Type == EM_ActionType_Tran
		|| Type == EM_ActionType_RefineLvUp
		|| Type == EM_ActionType_RefineLvDn
		|| Type == EM_ActionType_RefineNone)
	{

	}
	else
	{
		ServerItemLogStruct& Info = _St->SrvItemManage[ItemObj->GUID];
		Info.ItemID = ItemObj->GUID;
		Info.CreateCount += Item.Count;
	}

	if (ItemObj->MaxHeap > 1 && Global::Ini()->IsItemTradeLog_Detail == false)
		return;

	if (Global::Ini()->IsSaveLog == false)
		return;

	if (Global::Ini()->IsItemTradeLog == false)
		return;

	GameObjDbStructEx* ItemDB = Global::GetObjDB(Item.OrgObjID);
	if (ItemDB == NULL)
		return;

	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String(Note);
	string CmdBinStr = StringToSqlX((char*)charTransfer.GetWCString(), charTransfer.WCStrLen() * 2, false);
	string AbilityBinStr = StringToSqlX(Item.Ability, sizeof(Item.Ability), false);

	char Buf[8096];
	sprintf_s(Buf, sizeof(Buf), "Insert ItemTradeLog(TargetPlayerDBID,TargetNpcDBID,PlayerDBID,ActionType,ZoneID,PosX,PosZ,ItemID,ItemSerial,ItemCreateTime,ItemCount,ItemExValue,Note,ItemDurable,ItemMode,ItemImageObjectID,ItemAbility) Values( %d,%d,%d ,%d,%d ,%d,%d,%d,%d,%d,%d,%d,CAST( %s AS nvarchar),%d,%d,%d,cast( %s As binary(32) ))"
		, FromPlayerDBID
		, FromNpcDBID
		, ToPlayerDBID
		, Type
		, RoleDataEx::G_ZoneID
		, X
		, Z
		, Item.OrgObjID
		, Item.Serial
		, Item.CreateTime
		, Item.Count
		, Item.ExValue
		, CmdBinStr.c_str()
		, Item.Durable
		, Item.Mode._Mode
		, Item.ImageObjectID
		, AbilityBinStr.c_str()
	);

	Net_DCBase::LogSqlCommand(Buf);

	//--------------------------------------------------
	//°eLogјЖ­Иёк®ЖЁмDataCenter(for KR) by ®a»Ё 2010/12/30
	LOG_DATA_ITEM_TRADE LogData;

	RoleDataEx* FromRole = GetRoleDataByDBID(FromPlayerDBID);
	RoleDataEx* ToRole = GetRoleDataByDBID(ToPlayerDBID);

	if (FromRole != NULL)
	{
		LogData.From_Account_ID = FromRole->Account_ID();
		LogData.From_Account_ID.SafeStr();
	}

	if (ToRole != NULL)
	{
		LogData.To_Account_ID = ToRole->Account_ID();
		LogData.To_Account_ID.SafeStr();
	}

	LogData.WorldID = Global::Net()->GetWorldID();
	LogData.TargetNPCDBID = FromNpcDBID;
	LogData.TargetPlayerDBID = FromPlayerDBID;
	LogData.PlayerDBID = ToPlayerDBID;
	LogData.ActionType = Type;
	LogData.ZoneID = RoleDataEx::G_ZoneID;
	LogData.PosX = X;
	LogData.PosZ = Z;
	LogData.ItemID = Item.OrgObjID;
	LogData.ItemSerial = Item.Serial;
	LogData.ItemCreateTime = Item.CreateTime;
	LogData.ItemCount = Item.Count;
	LogData.ItemExValue = Item.ExValue;
	LogData.ItemDurable = Item.Durable;
	LogData.ItemMode = Item.Mode._Mode;
	LogData.ItemImageObjectID = Item.ImageObjectID;
	memcpy(LogData.ItemAbility, Item.Ability, sizeof(Item.Ability));

	Net_DCBase::SD_LogData(EM_LogType_ItemTrade, &LogData, sizeof(LogData));
	//--------------------------------------------------	
}

void		Global::Log_ItemServerLog()
{
	if (Global::Ini()->IsServerTotalItemLog == false)
		return;

	map< int, ServerItemLogStruct >::iterator Iter;
	char Buf[2048];

	int	BeginTime = MytimeGetTime();

	for (Iter = _St->SrvItemManage.begin(); Iter != _St->SrvItemManage.end(); Iter++)
	{
		ServerItemLogStruct& Info = Iter->second;

		sprintf_s(Buf, sizeof(Buf), "Insert ServerItemLog( ZoneID , ItemID , CreateCount , DestroyCount ) Values( %d,%d,%d,%d)"
			, RoleDataEx::G_ZoneID, Info.ItemID, Info.CreateCount, Info.DestroyCount);

		Net_DCBase::LogSqlCommand(Buf);
	}

	int ProcTime = MytimeGetTime() - BeginTime;

	Print(LV3, "Log_ItemServerLog", "SrvItemManage Count=%d ProcTime=%d", _St->SrvItemManage.size(), ProcTime);
	_St->SrvItemManage.clear();
}

void	Global::Log_ItemDestroy(RoleDataEx* Role, ActionTypeENUM Type, ItemFieldStruct& Item, int ItemCount, int NPCDBID, const char* Note)
{
	if (ItemCount == -1)
		ItemCount = Item.Count;

	GameObjDbStructEx* ItemObj = Global::GetObjDB(Item.OrgObjID);
	if (ItemObj == NULL)
		return;

	ServerItemLogStruct& Info = _St->SrvItemManage[ItemObj->GUID];
	Info.ItemID = ItemObj->GUID;
	Info.DestroyCount += ItemCount;

	if (Role->IsPlayer() == false)
		return;

	//§в¤ЈҐi°пЕ|ЄєЄ««~°µLog
	if (ItemObj->MaxHeap == 1)
	{
		Role->PlayerSelfData->DestroyItemLog.Push(Item);
	}

	//////////////////////////////////////////////////////////////////////////
	//¦pЄG¬°Є««~ЎAЁГҐB¦і§R°ЈіBµoј@±Ў
	if (ItemObj->IsItem_Pure() && strlen(ItemObj->Item.OnDestroyScript) != 0)
	{
		LUA_VMClass::PCallByStrArg(ItemObj->Item.OnDestroyScript, Role->GUID(), Role->GUID());
	}
	//////////////////////////////////////////////////////////////////////////

	if (Global::Ini()->IsSaveLog == false)
		return;

	if (ItemObj->Mode.DepartmentStore == true)
	{
		Log_DepartmentStore(Item, Type, ItemCount, 0, 0, 0, Role->DBID());
	}

	if (Global::Ini()->IsItemTradeLog == false)
		return;


	if (ItemObj->MaxHeap > 1 && Global::Ini()->IsItemTradeLog_Detail == false)
		return;

	char Buf[8096];

	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String(Note);
	string CmdBinStr = StringToSqlX((char*)charTransfer.GetWCString(), charTransfer.WCStrLen() * 2, false);
	string AbilityBinStr = StringToSqlX(Item.Ability, sizeof(Item.Ability), false);

	sprintf_s(Buf, sizeof(Buf), "Insert ItemTradeLog( PlayerDBID, TargetNpcDBID ,ActionType,ZoneID,PosX,PosZ,ItemID,ItemSerial,ItemCreateTime,ItemExValue,ItemCount,Note,ItemDurable,ItemMode,ItemImageObjectID,ItemAbility) Values( %d,%d,%d,%d ,%d,%d,%d,%d,%d,%d,%d,CAST( %s AS nvarchar),%d,%d,%d,cast( %s As binary(32) ) )"
		, Role->DBID()
		, NPCDBID
		, Type
		, Role->G_ZoneID
		, int(Role->Pos()->X)
		, int(Role->Pos()->Z)
		, Item.OrgObjID
		, Item.Serial
		, Item.CreateTime
		, Item.ExValue
		, ItemCount
		, CmdBinStr.c_str()
		, Item.Durable
		, Item.Mode._Mode
		, Item.ImageObjectID
		, AbilityBinStr.c_str()
	);

	Net_DCBase::LogSqlCommand(Buf);





}

void	Global::Log_Money(int FromPlayerDBID, int FromNpcDBID, RoleDataEx* ToRole, ActionTypeENUM Type, int Money)
{

	if (Global::Ini()->IsSaveLog == false)
		return;

	if (Global::Ini()->IsMoneyLog == false)
		return;

	//int ToPlayerDBID , int X , int Z

	char Buf[2048];

	sprintf_s(Buf, sizeof(Buf), "Insert MoneyLog(TargetNPCDBID,TargetPlayerDBID,PlayerDBID,ActionType,ZoneID,PosX,PosZ,NewMoney,OldMoney ) VALUES( %d,%d,%d,%d,%d,%d,%d,%d,%d)"
		, FromNpcDBID
		, FromPlayerDBID
		, ToRole->DBID()
		, Type
		, RoleDataEx::G_ZoneID
		, int(ToRole->X())
		, int(ToRole->Z())
		, ToRole->BodyMoney()
		, ToRole->BodyMoney() - Money);

	Net_DCBase::LogSqlCommand(Buf);

}

void	Global::Log_Money_Account(int FromPlayerDBID, int FromNpcDBID, RoleDataEx* ToRole, ActionTypeENUM Type, int Money)
{
	if (Global::Ini()->IsSaveLog == false)
		return;

	if (Global::Ini()->IsAccountMoneyLog == false)
		return;

	char Buf[2048];
	sprintf_s(Buf, sizeof(Buf), "Insert AccountMoneyLog(TargetNPCDBID,TargetPlayerDBID,PlayerDBID,ActionType,ZoneID,PosX,PosZ,NewAccountMoney, OldAccountMoney , Lv ) VALUES( %d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
		, FromNpcDBID
		, FromPlayerDBID
		, ToRole->DBID()
		, Type
		, RoleDataEx::G_ZoneID
		, int(ToRole->X())
		, int(ToRole->Z())
		, ToRole->BodyMoney_Account()
		, ToRole->BodyMoney_Account() - Money
		, ToRole->Level());

	Net_DCBase::LogSqlCommand(Buf);

	//--------------------------------------------------
	//°eLogјЖ­Иёк®ЖЁмDataCenter(for KR) by ®a»Ё 2011/01/07
	if (ToRole != NULL)
	{
		LOG_DATA_MONEY_ACCOUNT LogData;

		RoleDataEx* FromRole = GetRoleDataByDBID(FromPlayerDBID);

		if (FromRole != NULL)
		{
			LogData.From_Account_ID = FromRole->Account_ID();
			LogData.From_Account_ID.SafeStr();
		}

		LogData.To_Account_ID = ToRole->Account_ID();
		LogData.To_Account_ID.SafeStr();

		LogData.WorldID = Global::Net()->GetWorldID();
		LogData.TargetNPCDBID = FromNpcDBID;
		LogData.TargetPlayerDBID = FromPlayerDBID;
		LogData.PlayerDBID = ToRole->DBID();
		LogData.ActionType = Type;
		LogData.ZoneID = RoleDataEx::G_ZoneID;
		LogData.PosX = int(ToRole->X());
		LogData.PosZ = int(ToRole->Z());
		LogData.NewAccountMoney = ToRole->BodyMoney_Account();
		LogData.OldAccountMoney = ToRole->BodyMoney_Account() - Money;
		LogData.Lv = ToRole->Level();
		LogData.Money_Lock_Remaining = ToRole->PlayerTempData->LockAccountMoney;
		LogData.Time = TimeStr::Now_Value(RoleDataEx::G_TimeZone);

		if (ToRole->DBID() == RoleDataEx::LastLockMoneySpentPlayerDBID)
		{
			LogData.Cost_Lock = RoleDataEx::LastLockMoneySpent;
		}

		Net_DCBase::SD_LogData(EM_LogType_Money_Account, &LogData, sizeof(LogData));
	}
	//--------------------------------------------------	
}

void	Global::Log_Money_Bonus(int FromPlayerDBID, int FromNpcDBID, RoleDataEx* ToRole, ActionTypeENUM Type, int Money)
{
	if (Global::Ini()->IsSaveLog == false)
		return;

	if (Global::Ini()->IsAccountMoneyLog == false)
		return;

	char Buf[2048];
	sprintf_s(Buf, sizeof(Buf), "Insert BonusMoneyLog(TargetNPCDBID,TargetPlayerDBID,PlayerDBID,ActionType,ZoneID,PosX,PosZ,NewMoney, OldMoney ) VALUES( %d,%d,%d,%d,%d,%d,%d,%d,%d)"
		, FromNpcDBID
		, FromPlayerDBID
		, ToRole->DBID()
		, Type
		, RoleDataEx::G_ZoneID
		, int(ToRole->X())
		, int(ToRole->Z())
		, ToRole->PlayerBaseData->Money_Bonus
		, ToRole->PlayerBaseData->Money_Bonus - Money);

	Net_DCBase::LogSqlCommand(Buf);

}

void	Global::Log_Quest(RoleDataEx* pRole, int iQuestID, int iQuestStatus)
{

	if (Global::Ini()->IsSaveLog == false)
		return;

	if (Global::Ini()->IsQuestLog == false)
		return;

	if (pRole->IsPlayer() == false)
		return;

	GameObjDbStructEx* pQuestObj = Global::GetObjDB(iQuestID);
	if (pQuestObj == NULL)
		return;

	int	iQuestType = 0;

	char Buf[2048];

	if (pQuestObj->QuestDetail.iCheck_Loop == false)
		iQuestType = 0;
	else
		iQuestType = 1;
	/*
		sprintf( Buf , "Insert PlayerLog( PlayerDBID, ZoneID, PosX, PosZ, Job, Lv, QuestID, QuestType, QuestStatus ) Values( %d, %d, %d, %d, %d, %d, %d, %d, %d)"
			, pRole->Base.DBID
			, pRole->G_ZoneID
			, int( pRole->Pos()->X )
			, int( pRole->Pos()->Z )
			, pRole->BaseData.Voc
			, pRole->TempData.Attr.Level
			, iQuestID
			, iQuestType
			, iQuestStatus
			 );
	*/
	sprintf_s(Buf, sizeof(Buf), "Insert QuestLog( PlayerDBID, ZoneID, PosX, PosZ, Job, Lv, QuestID, QuestType, QuestStatus ) Values( %d, %d, %d, %d, %d, %d, %d, %d, %d)"
		, pRole->Base.DBID
		, pRole->G_ZoneID
		, int(pRole->Pos()->X)
		, int(pRole->Pos()->Z)
		, pRole->BaseData.Voc
		, pRole->TempData.Attr.Level
		, iQuestID
		, iQuestType
		, iQuestStatus
	);

	Net_DCBase::LogSqlCommand(Buf);

}

void	Global::Log_PlayerActionLog(RoleDataEx* Role)
{
	if (Global::Ini()->IsSaveLog == false)
		return;

	if (Global::Ini()->IsPlayerActionLog == false)
		return;

	if (Role->IsPlayer() != true)
		return;

	//¤Q¤АДБ¦s¤@¦ё
	//if( Role->BaseData.PlayTime - Role->PlayerTempData->Log.SavePlayTime < 10 )
	//	return;

	char Buf[2048];
	sprintf_s(Buf, sizeof(Buf), "Insert PlayerActionLog(PlayerDBID,Money,Money_Account,ActionMode,DeadCount,KillCount,Exp,Exp_Tp,MainJob,SubJob,Lv,PlayTime_Total,PlayTime_MainJob,Money_Total,Money_Account_Total,X,Z) VALUES(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
		, Role->DBID()
		, Role->PlayerBaseData->Body.Money - Role->PlayerTempData->Log.LastMoney
		, Role->PlayerBaseData->Body.Money_Account - Role->PlayerTempData->Log.LastMoney_Account
		, Role->PlayerTempData->Log.ActionMode._Mode
		, Role->PlayerTempData->Log.DeadCount
		, Role->PlayerTempData->Log.KillCount
		, Role->PlayerTempData->Log.Exp
		, Role->PlayerTempData->Log.Exp_TP
		, Role->BaseData.Voc
		, Role->BaseData.Voc_Sub
		, Role->TempData.Attr.Level
		, Role->BaseData.PlayTime
		, Role->PlayerBaseData->AbilityList[Role->BaseData.Voc].PlayTime
		, Role->PlayerBaseData->Body.Money
		, Role->PlayerBaseData->Body.Money_Account
		, int(Role->X())
		, int(Role->Z())
	);

	Role->PlayerTempData->Log.ActionMode._Mode = 0;
	Role->PlayerTempData->Log.DeadCount = 0;
	Role->PlayerTempData->Log.KillCount = 0;
	Role->PlayerTempData->Log.Exp = 0;
	Role->PlayerTempData->Log.LastMoney = Role->PlayerBaseData->Body.Money;
	Role->PlayerTempData->Log.SavePlayTime = Role->BaseData.PlayTime;
	Role->PlayerTempData->Log.Exp_TP = 0;
	Role->PlayerTempData->Log.LastMoney_Account = Role->PlayerBaseData->Body.Money_Account;

	Net_DCBase::LogSqlCommand(Buf);

}

void	Global::Log_Level(RoleDataEx* Role)
{
	if (Role->IsPlayer() == false)
		return;

	char Buf[2048];

	LUA_VMClass::PCallByStrArg("event_module(0)", Role->GUID(), Role->GUID());


	if (Global::Ini()->IsSaveLog == false)
		return;

	if (Global::Ini()->IsLevelLog == false)
		return;


	sprintf_s(Buf, sizeof(Buf), "Insert LevelLog(PlayerDBID, ZoneID,PosX,PosZ,Job,Lv,PlayTime,JobPlayTime) VALUES(%d,%d,%d,%d,%d,%d,%d,%d)"
		, Role->DBID()
		, Role->G_ZoneID
		, int(Role->Pos()->X)
		, int(Role->Pos()->Z)
		, Role->BaseData.Voc
		, Role->TempData.Attr.Level
		, Role->BaseData.PlayTime
		, Role->TempData.Attr.Ability->PlayTime);

	Net_DCBase::LogSqlCommand(Buf);

}

void	Global::Log_Dead(RoleDataEx* Deader, RoleDataEx* Killer)
{

	if (Global::Ini()->IsSaveLog == false)
		return;

	if (Killer == NULL)
		Deader = Killer;

	char Buf[2048];
	if (Deader->IsPlayer() == true)
	{
		if (Global::Ini()->IsPlayerDeadLog == false)
			return;

		if (Killer->IsPlayer())
		{
			sprintf_s(Buf, sizeof(Buf), "Insert PlayerDead(PlayerDBID, KillerPlayerDBID, KillerNPCOrgObjID, ZoneID, PosX, PosZ, Job,Lv, KillerLv, Job_Sub, KillerJob, KillerJob_Sub, Lv_Sub, KillerLv_Sub) VALUES(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
				, Deader->DBID()
				, Killer->DBID()
				, 0
				, Deader->G_ZoneID
				, int(Deader->Pos()->X)
				, int(Deader->Pos()->Z)
				, Deader->BaseData.Voc
				, Deader->TempData.Attr.Level
				, Killer->TempData.Attr.Level
				, Deader->BaseData.Voc_Sub
				, Killer->BaseData.Voc
				, Killer->BaseData.Voc_Sub
				, Deader->TempData.Attr.Level_Sub
				, Killer->TempData.Attr.Level_Sub);
		}
		else
		{
			sprintf_s(Buf, sizeof(Buf), "Insert PlayerDead(PlayerDBID, KillerPlayerDBID, KillerNPCOrgObjID, ZoneID, PosX, PosZ, Job,Lv, KillerLv, Job_Sub, KillerJob, KillerJob_Sub, Lv_Sub, KillerLv_Sub) VALUES(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
				, Deader->DBID()
				, 0
				, Killer->BaseData.ItemInfo.OrgObjID
				, Deader->G_ZoneID
				, int(Deader->Pos()->X)
				, int(Deader->Pos()->Z)
				, Deader->BaseData.Voc
				, Deader->TempData.Attr.Level
				, Killer->TempData.Attr.Level
				, Deader->BaseData.Voc_Sub
				, Killer->BaseData.Voc
				, Killer->BaseData.Voc_Sub
				, Deader->TempData.Attr.Level_Sub
				, Killer->TempData.Attr.Level_Sub);
		}

	}
	else
	{
		if (Global::Ini()->IsMonsterDeadLog == false)
			return;
		if (Killer->IsPlayer() == false)
			return;

		sprintf_s(Buf, sizeof(Buf), "Insert MonsterDead( OrgObjID,PlayerDBID,ZoneID,PosX,PosZ,KillerJob,Lv,KillerLv) Values(%d,%d,%d,%d,%d,%d,%d,%d)"
			, Deader->BaseData.ItemInfo.OrgObjID
			, Killer->DBID()
			, Deader->G_ZoneID
			, int(Deader->Pos()->X)
			, int(Deader->Pos()->Z)
			, Killer->BaseData.Voc
			, Deader->TempData.Attr.Level
			, Killer->TempData.Attr.Level);

	}

	Net_DCBase::LogSqlCommand(Buf);
}

void		Global::Log_EnterZone(RoleDataEx* Role)
{
	if (Global::Ini()->IsSaveLog == false)
		return;

	char Buf[2048];
	if (Role->IsPlayer() == false)
		return;

	sprintf_s(Buf, sizeof(Buf), "Insert GameLoginLog(ActionType,PlayerDBID , ZoneID, PosX , PosZ , Job , Lv ) Values(%d,%d,%d,%d,%d,%d,%d)"
		, EM_ActionType_EnterWorld
		, Role->DBID()
		, RoleDataEx::G_ZoneID
		, int(Role->Pos()->X)
		, int(Role->Pos()->Z)
		, Role->BaseData.Voc
		, Role->TempData.Attr.Level);

	Net_DCBase::LogSqlCommand(Role->DBID(), Buf);

	sprintf_s(Buf, sizeof(Buf), "Update RoleData Set IsEnterWorld=1,X=%d,Y=%d,Z=%d,ZoneID=%d,RoomID=%d WHERE DBID=%d"
		, int(Role->Pos()->X)
		, int(Role->Pos()->Y)
		, int(Role->Pos()->Z)
		, Role->ZoneID()
		, Role->RoomID()
		, Role->DBID());

	Net_DCBase::SqlCommand(rand(), Buf);

}
void		Global::Log_LeaveZone(RoleDataEx* Role)
{
	if (Role->IsPlayer() == false)
		return;

	char Buf[2048];
	sprintf_s(Buf, sizeof(Buf), "Update RoleData Set IsEnterWorld=0 WHERE DBID=%d"
		, Role->DBID());

	Net_DCBase::SqlCommand(rand(), Buf);

	if (Global::Ini()->IsSaveLog == false)
		return;

	sprintf_s(Buf, sizeof(Buf), "Insert GameLoginLog(ActionType,PlayerDBID , ZoneID, PosX , PosZ , Job , Lv ) Values(%d,%d,%d,%d,%d,%d,%d)"
		, EM_ActionType_LeaveWorld
		, Role->DBID()
		, Role->G_ZoneID
		, int(Role->Pos()->X)
		, int(Role->Pos()->Z)
		, Role->BaseData.Voc
		, Role->TempData.Attr.Level);

	Net_DCBase::LogSqlCommand(Buf);

}

void		Global::Log_AddExp(RoleDataEx* Role, int OldExp)
{
	if (Global::Ini()->IsSaveLog == false)
		return;

	if (Global::Ini()->IsExpLog == false)
		return;

	char Buf[2048];
	if (Role->IsPlayer() == false)
		return;

	sprintf_s(Buf, sizeof(Buf), "Insert ExpLog(ActionType,PlayerDBID , ZoneID, PosX , PosZ , Job , Lv , NewExp , OldExp ) Values(%d,%d,%d,%d,%d,%d,%d,%d,%d)"
		, EM_ActionType_Exp
		, Role->DBID()
		, Role->G_ZoneID
		, int(Role->Pos()->X)
		, int(Role->Pos()->Z)
		, Role->BaseData.Voc
		, Role->TempData.Attr.Level
		, Role->TempData.Attr.Ability->EXP
		, OldExp);

	Net_DCBase::LogSqlCommand(Buf);
}

void		Global::Log_ServerStatus()
{
	if (Global::Ini()->IsSaveLog == false)
		return;

	int RoomCount = 0;
	vector< PrivateRoomInfoStruct >& RoomList = Global::St()->PrivateRoomInfoList;

	for (unsigned int i = Ini()->BaseRoomCount; i < RoomList.size(); i++)
	{
		if (RoomList[i].IsActive == true)
			RoomCount++;
	}
	CCpuUsage cpuUsage;
	char Buf[2048];
	sprintf_s(Buf, sizeof(Buf), "Insert LocalServerStatus( PlayerCount, ObjectCount, ZoneID, RoomCount,CpuUsage,MemUsage ) Values (%d,%d,%d,%d,%f,%d)"
		, BaseItemObject::PlayerCount()
		, BaseItemObject::TotalCount()
		, RoleDataEx::G_ZoneID
		, RoomCount
		, float(GetMemoryUsage(GetCurrentProcessId())) / (1024 * 1024)
		, cpuUsage.GetCpuUsage(GetCurrentProcessId())
	);
	Net_DCBase::LogSqlCommand(Buf);
}

void	Global::Log_DepartmentStore(ItemFieldStruct& Item, ActionTypeENUM Type, int Count, int Cost, int Cost_Bonus, int Cost_Free, int PlayerDBID)
{
	if (Global::Ini()->IsSaveLog == false)
		return;

	//	if( Item.Mode.DepartmentStore == false )
	//		return;
		/*
		char Buf[ 2048];
		sprintf_s( Buf , sizeof(Buf), "Insert DepartmentStoreLog(Type, ItemID, ItemVesion, ItemCreateTime, ItemCount, Cost, Cost_Bonus , Cost_Free , PlayerDBID) Values (%d,%d,%d,%d,%d,%d,%d,%d,%d)"
			, Type
			, Item.OrgObjID
			, Item.Serial
			, Item.CreateTime
			, Count
			, Cost
			, Cost_Bonus
			, Cost_Free
			, PlayerDBID		);
		*/

	int iClientLanguage = EM_LanguageType_TW;
	BaseItemObject* Obj = GetPlayerObj_ByDBID(PlayerDBID);

	if (Obj)
	{
		iClientLanguage = Obj->CliMiscInfo()->iLanguage;
	}

	char Buf[2048];
	sprintf_s(Buf, sizeof(Buf), "Insert DepartmentStoreLog(Type, ItemID, ItemVesion, ItemCreateTime, ItemCount, Cost, Cost_Bonus , Cost_Free , PlayerDBID, Client_Language) Values (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
		, Type
		, Item.OrgObjID
		, Item.Serial
		, Item.CreateTime
		, Count
		, Cost
		, Cost_Bonus
		, Cost_Free
		, PlayerDBID
		, iClientLanguage);

	Net_DCBase::LogSqlCommand(Buf);

	if (Obj == NULL)
		return;
	//--------------------------------------------------
	//°eLogјЖ­Иёк®ЖЁмDataCenter(јИ®Й¬°Бъ°кЄ©Ґ»¦УҐ[) by ®a»Ё 2010/12/15
	RoleDataEx* Owner = Obj->Role();
	GameObjDbStructEx* ObjDB = g_ObjectData->GetObj(Item.OrgObjID);

	if (Owner != NULL && ObjDB != NULL)
	{
		int FreeMoneyID = g_ObjectData->GetSysKeyValue("SYS_PRICETYPE_ICONID_03");

		LOG_DATA_DEPARTMENT_STORE LogData;
		LogData.Account_ID = Owner->Account_ID();
		LogData.Account_ID.SafeStr();

		if (ObjDB->Name != NULL)
		{
			LogData.ItemName = ObjDB->Name;
			LogData.ItemName.SafeStr();
		}

		LogData.Money_Account_Remaining = Owner->GetValue(EM_RoleValue_Money_Account);
		LogData.Money_Bonus_Remaining = Owner->GetValue(EM_RoleValue_BonusMoney);
		LogData.Money_Free_Remaining = Owner->CalBodyItemCount(FreeMoneyID);
		LogData.Money_Lock_Remaining = Owner->PlayerTempData->LockAccountMoney;
		LogData.Type = Type;
		LogData.ItemID = Item.OrgObjID;
		LogData.ItemVesion = Item.Serial;
		LogData.ItemCreateTime = TimeStr::Now_Value(RoleDataEx::G_TimeZone);
		LogData.ItemCount = Count;
		LogData.Cost = Cost;
		LogData.Cost_Bonus = Cost_Bonus;
		LogData.Cost_Free = Cost_Free;
		LogData.WorldID = Global::Net()->GetWorldID();
		LogData.PlayerDBID = PlayerDBID;
		LogData.Client_Language = iClientLanguage;

		//Ёъ±oіМ«б¤@µ§LockMoney®ш¶O¬цїэ
		if (PlayerDBID == RoleDataEx::LastLockMoneySpentPlayerDBID)
		{
			LogData.Cost_Lock = RoleDataEx::LastLockMoneySpent;
		}

		Net_DCBase::SD_LogData(EM_LogType_DepartmentStore, &LogData, sizeof(LogData));
	}
	//--------------------------------------------------
}

void	Global::Log_Design(int Type, char* Content, int PlayerDBID, int GuildID)
{
	if (Global::Ini()->IsSaveLog == false)
		return;

	CharTransferClass	charTransfer;
	charTransfer.SetString(Content);

	string CmdBinStr = StringToSqlX((char*)charTransfer.GetWCString(), charTransfer.WCStrLen() * 2, false);

	char Buf[2048];
	sprintf_s(Buf, sizeof(Buf), "Insert DesignLog(Type,PlayerDBID,GuildID,Content) Values (%d,%d,%d,CAST( %s AS nvarchar(4000)) )"
		, Type
		, PlayerDBID
		, GuildID
		, CmdBinStr.c_str());

	Net_DCBase::LogSqlCommand(Buf);
}

//©w®ЙАЛ¬dїWҐЯ°П°м¬O§_­nёСєc
void	Global::Log_SkillLv(RoleDataEx* Owner, int SkillType, int SkillValue)
{
	if (Global::Ini()->IsSaveLog == false)
		return;

	char Buf[2048];
	sprintf_s(Buf, sizeof(Buf), "Insert SkillLvLog(PlayerDBID,SkillType,SkillValue,PlayTime) Values (%d,%d,%d,%d)"
		, Owner->DBID()
		, SkillType
		, SkillValue
		, Owner->BaseData.PlayTime);

	Net_DCBase::LogSqlCommand(Buf);
}

void	Global::Log_ClientComputerInfo(RoleDataEx* Owner, ClientComputerInfoStruct& Info)
{
	if (Global::Ini()->IsSaveLog == false)
		return;

	if (Global::Ini()->IsClientLog == false)
		return;
	unsigned IPNum = _Net->GetIpNum(Owner->SockID());

	char Buf[2048];
	sprintf_s(Buf, sizeof(Buf), "INSERT INTO ClientStatus(PlayerDBID , IPNum ,CPU, RAM_Total, RAM_Free, DisplayCardVendorID, DisplayCardDeviceID,Ping, FPS, OSID) VALUES(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
		, Owner->DBID(), Info.IPNum, Info.CPU, Info.RAM_Total, Info.RAM_Free, Info.DisplayCardVendorID, Info.DisplayCardDeviceID, Info.Ping, Info.FPS, Info.OSID);

	Net_DCBase::LogSqlCommand(Buf);
}

void	Global::Log_Talk(TalkLogTypeENUM Type, int ChannelID, int FromPlayerDBID, int ToPlayerDBID, const char* Content)
{

	if (Global::Ini()->IsSaveLog == false)
		return;

	if (Global::Ini()->IsTalkLog == false)
		return;

	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String(Content);
	string CmdBinStr = StringToSqlX((char*)charTransfer.GetWCString(), charTransfer.WCStrLen() * 2, false);


	char Buf[4096];
	sprintf_s(Buf, sizeof(Buf), "INSERT INTO TalkLog( Type , ChannelID , FromPlayerDBID , ToPlayerDBID , Content ) VALUES ( %d,%d,%d,%d, CAST( %s AS nvarchar(4000) ) )"
		, Type
		, ChannelID
		, FromPlayerDBID
		, ToPlayerDBID
		, CmdBinStr.c_str());

	Net_DCBase::LogSqlCommand(Buf);

}

void		Global::Log_Warning_MoneyVendor(RoleDataEx* VendorRole, int PlayerDBID, const char* Content)
{
	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String(Content);
	string CmdBinStr = StringToSqlX((char*)charTransfer.GetWCString(), charTransfer.WCStrLen() * 2, false);


	char Buf[4096];
	sprintf_s(Buf, sizeof(Buf), "INSERT INTO Warning_MoneyVendor(MoneyVenderPlayerDBID,PlayerDBID,Zone,Content ) VALUES ( %d,%d,%d, CAST( %s AS nvarchar(4000) ) )"
		, VendorRole->DBID()
		, PlayerDBID
		, RoleDataEx::G_ZoneID
		, CmdBinStr.c_str());

	Net_DCBase::LogSqlCommand(Buf);
}
void		Global::Log_Warning_Bot(RoleDataEx* VendorRole, int PlayerDBID, const char* Content)
{
	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String(Content);
	string CmdBinStr = StringToSqlX((char*)charTransfer.GetWCString(), charTransfer.WCStrLen() * 2, false);


	char Buf[4096];
	sprintf_s(Buf, sizeof(Buf), "INSERT INTO Warning_Bot(BotVenderPlayerDBID,PlayerDBID,Zone,Content,X,Z ) VALUES ( %d,%d,%d, CAST( %s AS nvarchar(4000) ) , %d,%d )"
		, VendorRole->DBID()
		, PlayerDBID
		, RoleDataEx::G_ZoneID
		, CmdBinStr.c_str()
		, int(VendorRole->X())
		, int(VendorRole->Z()));

	Net_DCBase::LogSqlCommand(Buf);
}
void	Global::Log_ZoneMoneyLog()
{
	static int LProcTime = RoleDataEx::G_Now / (60 * 60);
	int NowProcTime = RoleDataEx::G_Now / (60 * 60);

	if (LProcTime == NowProcTime)
		return;
	LProcTime = NowProcTime;

	char Buf[4096];
	sprintf_s(Buf, sizeof(Buf), "INSERT ZoneMoneyLog(ZoneID, MoneyIn, MoneyOut, AccountMoneyIn, AccountMoneyOut)	VALUES(%d,%d,%d,%d,%d)"
		, RoleDataEx::G_ZoneID, RoleDataEx::MoneyIn, RoleDataEx::MoneyOut, RoleDataEx::AccountMoneyIn, RoleDataEx::AccountMoneyOut);
	Net_DCBase::SqlCommand(rand(), Buf);
	RoleDataEx::MoneyIn = 0;
	RoleDataEx::MoneyOut = 0;
	RoleDataEx::AccountMoneyIn = 0;
	RoleDataEx::AccountMoneyOut = 0;
}


void	Global::Log_OfflineMsg(const char* ToPlayerName, const char* FromName, int FromDBID, const char* Msg)
{
	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String(ToPlayerName);
	string ToNameCmdBinStr = StringToSqlX((char*)charTransfer.GetWCString(), charTransfer.WCStrLen() * 2, false);

	charTransfer.SetUtf8String(FromName);
	string FromNameCmdBinStr = StringToSqlX((char*)charTransfer.GetWCString(), charTransfer.WCStrLen() * 2, false);

	charTransfer.SetUtf8String(Msg);
	string MsgCmdBinStr = StringToSqlX((char*)charTransfer.GetWCString(), charTransfer.WCStrLen() * 2, false);

	char Buf[8192];
	sprintf_s(Buf, sizeof(Buf), "BEGIN declare @PlayerName nvarchar( 32 );SET @PlayerName=CAST( %s AS nvarchar(4000));declare @FromPlayerName nvarchar( 32 );SET @FromPlayerName=CAST( %s AS nvarchar(4000));declare @Message nvarchar( 4000 );SET @Message=CAST( %s AS nvarchar(4000));EXECUTE CreateOfflineMessage @PlayerName,%d,@FromPlayerName,@Message; END"
		, ToNameCmdBinStr.c_str()
		, FromNameCmdBinStr.c_str()
		, MsgCmdBinStr.c_str()
		, FromDBID);

	Net_DCBase::SqlCommand(rand(), Buf);

}
#endif

int	Global::Rand(int iMin, int iMax)
{
	static int iLast = 0;
	/*
	if( iMin == iMax )
		return iMax;

	if( iMin > iMax )
	{
		int X = iMin - iMax;
		int number = rand() % X + iMin ;

		return number;
	}
	else
	{
		int X = iMax - iMin;
		int number = rand() % X + iMin ;

		return number;
	}
	*/

	//return (int) ( m_ruRand.GenerateUINT32() % iMax )
	//int iVal = (int) ( m_ruRand.GenerateFloat() * ( iMax - iMin ) ) + iMin;
	if (iMin > iMax)
		return iMin;

	if (iMax == iMin)
		return iMin;

	int iVal = (int)(m_ruRand.GenerateUINT32() % (iMax - iMin) + iMin);

	if (iVal != iLast)
	{
		iLast = iVal;
		return iVal;
	}
	else
	{
		int iVal = (int)(m_ruRand.GenerateUINT32() % (iMax - iMin) + iMin);
		iLast = iVal;

		return iVal;
	}

	//return (int) ( ( (float) rand() / (float) RAND_MAX ) * ( iMax - iMin ) ) + iMin;

	//return 0;
}

//Є««~Ґ[ІЕ¤е
bool		Global::AddRune(ItemFieldStruct* UseItem, GameObjDbStructEx* UseItemOrgDB
	, ItemFieldStruct* TargetItem, GameObjDbStructEx* TargetItemOrgDB)
{
	if (RoleDataEx::CheckAddRune(UseItem, TargetItem) != EM_CheckAddRuneResult_OK)
		return false;


	TargetItem->RuneVolume -= UseItemOrgDB->Item.Volume;

	for (int i = 0; ; i++)
	{
		if (i >= 4)
		{
			return false;
		}

		if (TargetItem->Rune(i) == 0)
		{
			TargetItem->Rune(i, UseItemOrgDB->GUID);
			break;
		}
	}


	//UseItem->Init();

	return true;
}
//--------------------------------------------------------------------------------------------------
//·jґMГdЄ«Єє¬IЄkҐШјР
/*
RoleDataEx*		Global::SearchPetMagicTarget( RoleDataEx* Owner , RoleDataEx* Pet , PetSkillStruct* Skill )
{
	GameObjDbStructEx* MagicOrgDB = Global::GetObjDB( Skill->SkillID );
	if( MagicOrgDB == NULL )
		return NULL;


	if( Skill->Rate < rand()% 100 )
		return NULL;

	if( Pet->CheckSpellMagicCost( Pet , MagicOrgDB , 0  ) == false )
		return NULL;

	switch( Skill->TargetType )
	{
	case EM_PetSpellTarget_Self:
		{
			if( Pet->CheckEffect_SpellMagic( Pet , MagicOrgDB ) == false )
				return NULL;
			else
				return Pet;
		}
		break;
	case EM_PetSpellTarget_Owner:
		{
			if( Pet->CheckEffect_SpellMagic( Owner , MagicOrgDB ) == false )
				return NULL;
			else
				return Owner;
		}
		break;
	case EM_PetSpellTarget_Party:
		{

			if( Owner->PartyID() == -1)
			{
				if( Pet->CheckEffect_SpellMagic( Pet , MagicOrgDB ) == true )
					return Pet;
				if( Pet->CheckEffect_SpellMagic( Owner , MagicOrgDB ) == true )
					return Owner;
				return NULL;
			}

			vector<BaseSortStruct>& RetList = *SearchRangeObject( Pet , Pet->Pos()->X , Pet->Pos()->Y , Pet->Pos()->Z , EM_SearchRange_All , MagicOrgDB->MagicCol.AttackDistance );

			for( int i = 0 ; i < (int)RetList.size() ; i++ )
			{
				RoleDataEx* Target = (RoleDataEx*)RetList[i].Data;
				if( Target->GetOrgOwner()->PartyID() == Owner->PartyID() )
				{
					if( Pet->CheckEffect_SpellMagic( Target , MagicOrgDB ) == true )
						return Target;
				}
			}
		}
		break;
	case EM_PetSpellTarget_Enemy:
		{
			if( Pet->IsAttack() == true )
				return NULL;

			RoleDataEx* Target = Global::GetRoleDataByGUID( Pet->TempData.AI.TargetID );
			if( Target == NULL )
				return NULL;

			if( Pet->CheckEffect_SpellMagic( Target , MagicOrgDB ) == false )
				return NULL;
			else
				return Owner;

		}
		break;
	}

	return NULL;
}
*/
//--------------------------------------------------------------------------------------------------
bool	Global::CheckClientLoading_SetPos(int GItemID, float X, float Y, float Z, float Dir)
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObj(GItemID);

	if (OwnerObj == NULL)
	{
		Print(Def_PrintLV3, "CheckClientLoading_SetPos", "Owener(%d)=NULL", GItemID);
		return false;
	}

	RoleDataEx* Owner = OwnerObj->Role();

	if (Owner->IsPlayer())
	{
		const float MinNeedLoadingMapRange = 1000.0f;
		float Dx = Owner->Pos()->X - X;
		float Dz = Owner->Pos()->Z - Z;

		//АЛ¬d¬O§_­nµҐ«ЭClient Loading Map ёк®Ж
		if (Dx * Dx + Dz * Dz > MinNeedLoadingMapRange * MinNeedLoadingMapRange)
		{
			DelFromPartition(GItemID);
			Owner->Pos(X, Y, Z, Dir);
			Owner->TempData.BackInfo.LX = X;
			Owner->TempData.BackInfo.LZ = Z;

			NetSrv_Move::S_ClientLoading(Owner->GUID(), Owner->Pos()->X, Owner->Pos()->Y, Owner->Pos()->Z);
			return false;
		}

	}

	return NetSrv_MoveChild::SetObjPos(OwnerObj, X, Y, Z, Dir);
}

//-------------------------------------------------------------------------------------------
//їWҐЯ°П°міBІz
//-------------------------------------------------------------------------------------------
//«юЁ©ІД0­У©Р¶ЎЄє©ЗЄ«
bool		Global::CopyRoomMonster(int FromRoomID, int ToRoomID)
{
	if (ToRoomID < Ini()->BaseRoomCount || ToRoomID >= Ini()->RoomCount)
		return false;

	if (FromRoomID < 0 || FromRoomID >= Ini()->BaseRoomCount)
		return false;
	NetSrv_OtherChild::ResetMapMark(ToRoomID);
	NetSrv_OtherChild::ResetRoomValue(ToRoomID);

	DelRoomMonster(ToRoomID);


	GameObjDbStructEx* ZoneObj = Global::GetObjDB(Def_ObjectClass_Zone + RoleDataEx::G_ZoneID % _DEF_ZONE_BASE_COUNT_);
	if (ZoneObj != NULL)
	{
		if (strcmp(ZoneObj->Zone.LuaInitZone, "") != 0)
		{
			// ¶] Zone Єм©lј@±Ў
			char szLuaString[256];
			sprintf_s(szLuaString, sizeof(szLuaString), "%s( %d )", ZoneObj->Zone.LuaInitZone, ToRoomID);
			LUA_VMClass::PCallByStrArg(szLuaString, -1, -1, NULL, NULL);
		}
	}

	vector< RoleDataEx* > CopyRoleList;

	set<BaseItemObject* >& NpcObjSet = *(BaseItemObject::NPCObjSet());
	set< BaseItemObject*>::iterator   NpcObjIter;
	for (NpcObjIter = NpcObjSet.begin(); NpcObjIter != NpcObjSet.end(); NpcObjIter++)
	{
		BaseItemObject* Obj = *NpcObjIter;
		if (Obj == NULL)
			continue;
		RoleDataEx* Other = Obj->Role();
		if (Other->BaseData.Mode.Save == false || Other->IsNPC() == false)
			continue;

		if (Other->BaseData.RoomID != FromRoomID)
			continue;

		CopyRoleList.push_back(Other);
		//CreateRoomObj( RoomID , Other );
	}

	for (unsigned i = 0; i < CopyRoleList.size(); i++)
	{
		CreateRoomObj(ToRoomID, CopyRoleList[i]);
	}



	return true;
}
//§R°Ј¬Y©Р¶ЎЄє©ЗЄ«( ІД0­У©Р¶Ў¤ЈҐi§R°Ј)
bool		Global::DelRoomMonster(int RoomID)
{
	if (RoomID < Ini()->BaseRoomCount || RoomID >= Ini()->RoomCount)
		return false;

	if (RoomID == 0)
		return false;

	//ІM°Ј°П°мЅu¬qёIјІ
	St()->LuaLineBlock[RoomID].Clear();


	set<BaseItemObject* >& NpcObjSet = *(BaseItemObject::NPCObjSet());
	set< BaseItemObject*>::iterator   NpcObjIter;
	for (NpcObjIter = NpcObjSet.begin(); NpcObjIter != NpcObjSet.end(); NpcObjIter++)
	{
		BaseItemObject* Obj = *NpcObjIter;
		if (Obj == NULL)
			continue;
		if (Obj->Role()->RoomID() == RoomID)
			Obj->Destroy("SYS DelRoomMonster");
	}
	return true;
}

int		Global::CreateRoomObj(int RoomID, RoleDataEx* Owner)
{
	if (RoomID == 0 || Owner->RoomID() != 0)
		return -1;
	int ItemID = Global::CreateObj(Owner->BaseData.ItemInfo.OrgObjID, Owner->Pos()->X, Owner->Pos()->Y, Owner->Pos()->Z, Owner->Pos()->Dir, 1);

	BaseItemObject* NewClass = BaseItemObject::GetObj(ItemID);

	if (NewClass == NULL)
	{
		Print(Def_PrintLV3, "CreateRoomObj", "Create Object Error!!");
		return -1;
	}

	NewClass->NPCMoveInfo(Owner->DBID());

	RoleDataEx* New = NewClass->Role();

	New->RoleName((char*)Owner->RoleName());
	New->SelfData.AutoPlot = Owner->SelfData.AutoPlot;

	New->SelfData.NpcQuestID = Owner->SelfData.NpcQuestID;
	New->SelfData.PID = Owner->SelfData.PID;
	New->SelfData.PlotClassName = Owner->SelfData.PlotClassName;

	New->BaseData.Mode._Mode = Owner->BaseData.Mode._Mode;
	New->BaseData.Mode.Save = false;

	// ґъёХ
	if (New->SelfData.AutoPlot.Size() != 0)
	{
		NewClass->PlotVM()->CallLuaFunc(New->SelfData.AutoPlot.Begin(), New->GUID(), 0);
	}

	// јЛЄ©Єм©lј@±Ў
	{
		GameObjDbStructEx* pDBObj = Global::GetObjDB(New->BaseData.ItemInfo.OrgObjID);

		if (pDBObj && (pDBObj->IsNPC() || pDBObj->IsQuestNPC()))
		{
			if (strlen(pDBObj->NPC.szLuaInitScript) != 0)
			{
				NewClass->PlotVM()->CallLuaFunc(pDBObj->NPC.szLuaInitScript, NewClass->GUID());
			}
		}
	}

	//----------------------------------------------------------------------------------
	// Ег№і
	//----------------------------------------------------------------------------------
	Global::AddToPartition(ItemID, RoomID);

	return ItemID;
}

//©w®ЙАЛ¬dїWҐЯ°П°м¬O§_­nёСєc
void	Global::CheckPrivateZone()
{
	if (Ini()->PrivateZoneType == EM_PrivateZoneType_None)
		return;

	vector< int > RoomPlayerCountList;

	for (int i = 0; i < Ini()->RoomCount; i++)
	{
		RoomPlayerCountList.push_back(0);
	}


	set<BaseItemObject* >& PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	BaseItemObject* Obj;
	//­pєвЁC­У©Р¶ЎЄє¤HјЖ
	for (PlayerObjIter = PlayerObjSet.begin(); PlayerObjIter != PlayerObjSet.end(); PlayerObjIter++)
	{
		Obj = *PlayerObjIter;
		if (Obj == NULL)
			continue;

		//ІД0 ­У©Р¶Ў¤ЈіBІz
		if (Obj->Role()->RoomID() == 0 || Obj->Role()->RoomID() == -1)
			continue;

		if (RoomPlayerCountList.size() <= (unsigned)Obj->Role()->RoomID())
			continue;

		//­pєвЁC­У©Р¶ЎЄє¤HјЖ
		RoomPlayerCountList[Obj->Role()->RoomID()]++;

		//------------------------------------------------------
		//АЛ¬d§в¤Ј¦XІzЄєЄ±®a¶ЗЁ«(GM¤ЈіBІz)
		if (Obj->Role()->BaseData.Voc == EM_Vocation_GameMaster)
			continue;

		PrivateRoomInfoStruct& RoomInfo = Global::St()->PrivateRoomInfoList[Obj->Role()->RoomID()];
		if (RoomInfo.IsActive == false
			|| (RoomInfo.LiveTime && (myUInt32)RoomInfo.LiveTime < RoleDataEx::G_Now))
		{
			if (LuaPlotClass::GoReturnPos(Obj->GUID()) == false)
				ChangeZone(Obj->GUID(), Ini()->RevZoneID, 0, (float)Ini()->RevX, (float)Ini()->RevY, (float)Ini()->RevZ, 0);

			continue;
		}

		if (Obj->Role()->IsDead() == true)
			continue;

		switch (Ini()->PrivateZoneType)
		{
		case EM_PrivateZoneType_Private:
			if (RoomInfo.OwnerDBID != Obj->Role()->DBID())
			{
				if (LuaPlotClass::GoReturnPos(Obj->GUID()) == false)
					ChangeZone(Obj->GUID(), Ini()->RevZoneID, 0, (float)Ini()->RevX, (float)Ini()->RevY, (float)Ini()->RevZ, 0);

			}
			break;
		case EM_PrivateZoneType_Party:
			if (RoomInfo.OwnerPartyID != Obj->Role()->PartyID())
			{
				if (LuaPlotClass::GoReturnPos(Obj->GUID()) == false)
					ChangeZone(Obj->GUID(), Ini()->RevZoneID, 0, (float)Ini()->RevX, (float)Ini()->RevY, (float)Ini()->RevZ, 0);

			}
			break;
		case EM_PrivateZoneType_Raid:
			break;
		case EM_PrivateZoneType_Private_Party:
		{
			if (RoomInfo.OwnerPartyID == -1)
			{
				if (RoomInfo.OwnerDBID != Obj->Role()->DBID())
				{
					if (LuaPlotClass::GoReturnPos(Obj->GUID()) == false)
						ChangeZone(Obj->GUID(), Ini()->RevZoneID, 0, (float)Ini()->RevX, (float)Ini()->RevY, (float)Ini()->RevZ, 0);
				}
			}
			else
			{
				if (RoomInfo.OwnerPartyID != Obj->Role()->PartyID())
				{
					if (LuaPlotClass::GoReturnPos(Obj->GUID()) == false)
						ChangeZone(Obj->GUID(), Ini()->RevZoneID, 0, (float)Ini()->RevX, (float)Ini()->RevY, (float)Ini()->RevZ, 0);
				}
			}
		}
		break;
		}
	}

	//§в¶W№Lі]©w®Й¶ЎЄє©Р¶Ў§R°Ј
	for (int i = 1; i < Ini()->RoomCount; i++)
	{
		PrivateRoomInfoStruct& RoomInfo = Global::St()->PrivateRoomInfoList[i];

		if (RoomInfo.LiveTime && (myUInt32)RoomInfo.LiveTime < RoleDataEx::G_Now)
		{
			RoomInfo.Init();
			DelRoomMonster(i);
		}
		else if (RoomPlayerCountList[i] == 0 && RoomInfo.IsActive == true)
		{
			if (RoomInfo.EmptyCount > Ini()->PrivateZoneLiveTime * 6)
			{
				RoomInfo.Init();
				DelRoomMonster(i);
			}
			RoomInfo.EmptyCount++;
		}
		else
		{
			RoomInfo.EmptyCount = 0;
		}
	}
}


int      Global::_CheckChangeZoneNPC(SchedularInfo* Obj, void* InputClass)
{
	int CheckCount = Obj->GetNumber("CheckCount") + 1;

	RoleDataEx* TempRole = (RoleDataEx*)InputClass;
	CreateChangeZoneNPC(TempRole, CheckCount);
	delete TempRole;
	return 0;

}
int		Global::CreateChangeZoneNPC(RoleDataEx* Role, int CheckCount)
{
	//§дҐD¤H
	BaseItemObject* MasterRoleObj = Global::GetPlayerObj_ByDBID(Role->TempData.Sys.OwnerDBID);
	RoleDataEx* MasterRole = NULL;
	if (MasterRoleObj != NULL)
		MasterRole = MasterRoleObj->Role();

	if (Role->IsDead())
		return -1;

	//§д¤ЈЁмҐD¤H
	if (Role->TempData.Sys.OwnerDBID != -1 && MasterRole == NULL)
	{
		if (CheckCount >= 10)
			return -1;

		RoleDataEx* TempRole = NEW(RoleDataEx);
		//*TempRole = *Role;
		TempRole->Copy(Role);
		Schedular()->Push(_CheckChangeZoneNPC, 300, TempRole
			, "CheckCount", EM_ValueType_Int, CheckCount
			, NULL);
		return -2;
	}

	Role->SaveDB_AddTime();

	int NewRoleID = CreateObj(Role->BaseData.ItemInfo.OrgObjID, Role->Pos()->X, Role->Pos()->Y, Role->Pos()->Z, Role->Pos()->Dir, 1, Role->WorldGUID());

	BaseItemObject* NewRoleObj = Global::GetObj(NewRoleID);
	if (NewRoleObj == NULL)
		return -1;

	RoleDataEx* NewRole = NewRoleObj->Role();

	int     GUID = NewRole->GUID();
	int     WorldGUID = NewRole->WorldGUID();
	int     LoginTempID = NewRole->PlayerTempData->LoginTempID;


	Role->TempData.BackInfo.HEvent_Ai = NewRole->TempData.BackInfo.HEvent_Ai;
	Role->TempData.BackInfo.HEvent_Move = NewRole->TempData.BackInfo.HEvent_Move;


	//*NewRole = *Role;
	NewRole->Copy(Role);

	NewRole->GUID(GUID);
	NewRole->WorldGUID(WorldGUID);
	NewRole->PlayerTempData->LoginTempID = LoginTempID;
	NewRole->InitCheckRoleData();
	NewRole->InitCal();


	if (MasterRole && (unsigned)NewRole->TempData.SummonPet.Type <= EM_SummonPetType_Max)
	{
		//PetStruct& SummonPet = *(MasterRole->GetPetInfo( NewRole ));
		PetStruct& SummonPet = MasterRole->TempData.SummonPet.Info[NewRole->TempData.SummonPet.Type];
		NewRole->TempData.Sys.OwnerDBID = MasterRole->DBID();

		if (NewRole->TempData.Sys.IsPet == true)
		{
			BaseItemObject* OldPetObj = Global::GetObj(SummonPet.PetID);
			if (OldPetObj != NULL && OldPetObj->Role()->TempData.Sys.OwnerDBID == MasterRole->DBID())
			{

				//OldPetObj->Destroy( "SYS CreateChangeZoneNPC" );
				//SummonPet.Init();
				//¤w¦і¦№єШГdЄ«
				return -1;
			}

			SummonPet.Action = EM_PET_FOLLOW;
			SummonPet.PetID = NewRoleID;
			SummonPet.TargetID = -1;
			SummonPet.MagicID = NewRole->TempData.SummonPet.MagicBaseID;
			NewRole->TempData.Sys.IsPet = true;
			NewRole->BaseData.Mode.IsPet = true;
			MasterRole->TempData.UpdateInfo.ReSetTalbe = true;

			switch (NewRole->TempData.SummonPet.Type)
			{
			case EM_SummonPetType_Normal:	//¤@ЇлГdЄ«
				NewRole->BaseData.SysFlag.IsContorlPet = true;
				break;
			case EM_SummonPetType_CultivatePet:	//ѕiґЮГdЄ«

				break;
			}
			NewRoleObj->PlotVM()->CallLuaFunc(NewRole->SelfData.AutoPlot.Begin(), MasterRole->GUID(), 0);
			NetSrv_Magic::S_CreatePet(MasterRole->GUID(), NewRole->GUID(), NewRole->WorldGUID(), NewRole->BaseData.ItemInfo.OrgObjID, NewRole->TempData.SummonPet.MagicBaseID, NewRole->TempData.SummonPet.Type);
		}
		NewRole->TempData.Sys.OwnerGUID = MasterRole->GUID();
		NewRole->TempData.Sys.OwnerDBID = MasterRole->DBID();
		NewRole->BaseData.RoomID = MasterRole->RoomID();
		NewRole->Pos()->X = NewRole->TempData.Move.Tx;
		NewRole->Pos()->Y = NewRole->TempData.Move.Ty;
		NewRole->Pos()->Z = NewRole->TempData.Move.Tz;

		//¦pЄG»PҐD¤H¶ZВч¶W№L300ЎA«hЄЅ±µёхЁмҐD¤HЄє¦мёm
		if (MasterRole->Length(NewRole) > 300)
		{
			NewRole->Pos()->X = MasterRole->Pos()->X + rand() % 50 - 25;
			NewRole->Pos()->Y = MasterRole->Pos()->Y;
			NewRole->Pos()->Z = MasterRole->Pos()->Z + rand() % 50 - 25;
			NewRole->Pos()->Dir = MasterRole->Pos()->Dir;

		}
		NewRoleObj->SimpleSchedularReg(PetProc);
	}

	AddToPartition(NewRoleID, NewRole->BaseData.RoomID);

	if (MasterRole)
	{
		NewRole->SetFollow(MasterRole);
	}

	return NewRoleID;
}

void	Global::SaveAllPlayer_byCrash()
{
	set<BaseItemObject* >& PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//­pєвЁC­У©Р¶ЎЄє¤HјЖ
	for (PlayerObjIter = PlayerObjSet.begin(); PlayerObjIter != PlayerObjSet.end(); PlayerObjIter++)
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if (Obj == NULL)
			continue;

		RoleDataEx* Role = Obj->Role();
		Net_DCBase::SavePlayer(GetPCenterID(Role->Account_ID()), (PlayerRoleData*)Role, 0, sizeof(PlayerRoleData), 0);
		Role->Msg("Server Close");
	}
	_Net->SendFlush();
	Sleep(5000);
}


void Global::OnCrashProcessCheck()
{
	char szLuaFunc[256];
	LUA_VMClass::GetProcFuncName(szLuaFunc);
	Print(LV5, "CrashProcess", "Server no respond, running Luafunc= %s, executetime= %d", szLuaFunc, LUA_VMClass::GetProcFuncExecuteTime());
}

void Global::OnCrashProcessExit()
{
	set<BaseItemObject* >& PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//­pєвЁC­У©Р¶ЎЄє¤HјЖ
	for (PlayerObjIter = PlayerObjSet.begin(); PlayerObjIter != PlayerObjSet.end(); PlayerObjIter++)
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if (Obj == NULL)
			continue;

		RoleDataEx* Role = Obj->Role();
		Net_DCBase::SavePlayer(GetPCenterID(Role->Account_ID()), (PlayerRoleData*)Role, 0, sizeof(PlayerRoleData), 0);
		Role->Msg("Server Close");
	}
	_Net->SendFlush();

	Print(LV2, "CrashProcess", "Program No Respond, Save RoleData\n");
}
/*
DWORD WINAPI 	Global::CrashProcess( void* )
{
	RoleDataEx::OnTimeProcAll( );
	int		CheckValue = 0;
	while( 1 )
	{
		Sleep( 10000 );


		if( IsDebuggerPresent() == true )
		{
			continue;
		}



		if( _St->IsDestroy == true )
			return 0;
		//¦pЄG¤@¤АДБіЈЁS§у·s№L
		if( timeGetTime() - RoleDataEx::G_SysTime_Base - RoleDataEx::G_SysTime <= 60*1000  )
		{
			CheckValue = 0;
			continue;
		}

		CheckValue++;
		if( CheckValue < 10 )
		{
			// ­Y¤@¤АДБіЈЁS¤ПАі, «h¦L°TІЯГDҐЬ
			char szLuaFunc[256];

			int iTargetID		= 0;
			int iTargetOrgID	= 0;

			int iOwnerID		= 0;
			int iOwnerOrgID		= 0;

			int iSystime		= 0;

			//iTargetID =

			LUA_VMClass::GetProcFuncName( szLuaFunc );
			Print( LV5, "CrashProcess", "Server no respond, running Luafunc= %s, executetime= %d", szLuaFunc , LUA_VMClass::GetProcFuncExecuteTime() );

			// ¦LҐX CallStack

			StackWalkerChild sw;
			sw.m_callStackStrings.clear();

			sw.ShowCallstack(Global::m_MainThreadHandle);

			Print(LV5, "CrashProcess", "Print Call Stack");
			for (int i = 0 ; i < sw.m_callStackStrings.size(); i++)
			{
				Print(LV5, "CrashProcess", sw.m_callStackStrings[i].c_str());
			}

			Sleep( 1000 );
			continue;
		}

		set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
		set< BaseItemObject*>::iterator   PlayerObjIter;
		//­pєвЁC­У©Р¶ЎЄє¤HјЖ
		for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
		{
			BaseItemObject* Obj = *PlayerObjIter;
			if( Obj == NULL )
				continue;

			RoleDataEx* Role = Obj->Role();
			Net_DCBase::SavePlayer( Role , 0 , sizeof(Role)  , 0 );
			Role->Msg( "Server Close" );
		}
		_Net->SendFlush( );


		Print(LV2, "CrashProcess", "Generate CrashReport\n" );
		ManualGenerateCrachRpt();

		Print(LV2, "CrashProcess", "Program No Respond, Save RoleData\n" );
		return 0;
	}
	return 0;
}
*/

void Global::ClientPacketCrashOutput(int id, int size, PacketBase* data)
{
	Print(LV5, "ClientPacketCrashOutput", "ID=%d Size=%d PackCmd=%d", id, size, data->Command);
	BaseItemObject* OwnerObj = BaseItemObject::GetObjBySockID(id);
	if (OwnerObj == NULL)
	{
		Print(LV5, "ClientPacketCrashOutput", "OwnerObj == NULL ");
		return;
	}
	RoleDataEx* Owner = OwnerObj->Role();

	Print(LV5, "ClientPacketCrashOutput", "Sender Account=%s RoleName=%s", Owner->Base.Account_ID.Begin(), Global::GetRoleName_ASCII(Owner).c_str());
	OwnerObj->Destroy("SYS ClientPacketCrashOutput");

}

//ІM°ЈЁ¤¦вёф®|
void	Global::ClearPath(int GItemID)
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObj(GItemID);
	if (OwnerObj == NULL)
	{
		Print(LV5, "ClientPacketCrashOutput", "OwnerObj == NULL ");
		return;
	}
	OwnerObj->Path()->SetPos(OwnerObj->Role()->X(), OwnerObj->Role()->Y(), OwnerObj->Role()->Z());
	//	OwnerObj->Path()->ClearPath();
}

void	Global::SetPos(int GItemID)
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObj(GItemID);
	if (OwnerObj == NULL)
	{
		Print(LV5, "ClientPacketCrashOutput", "OwnerObj == NULL ");
		return;
	}
	RoleDataEx* Owner = OwnerObj->Role();

	//OwnerObj->Path()->SetPos( Owner->X() , Owner->Y() , Owner->Z() );
	//OwnerObj->Path()->ClearPath();
	NetSrv_MoveChild::SetObjPos(OwnerObj, Owner->X(), Owner->Y(), Owner->Z(), Owner->Dir());
}


void		Global::Save_ZoneInfo()
{

	char Buf[2048];
	sprintf_s(Buf, sizeof(Buf), "UPDATE ZoneInfo SET ItemVersion = %d , InstanceResetTime = CAST( %f AS float ) , InstanceNextResetTime = CAST( %f AS float )  WHERE ZoneID=%d"
		, Global::St()->LocalDBInfo.ItemVersion
		, Global::St()->LocalDBInfo.InstanceResetTime
		, Global::St()->LocalDBInfo.InstanceNextResetTime
		, RoleDataEx::G_ZoneID);


	Net_DCBase::SqlCommand(RoleDataEx::G_ZoneID, Buf);
}

const char* Global::GetRoleName(RoleDataEx* Owner)
{
	if (Owner->IsPlayer())
		return Owner->RoleName();

	if (strlen(Owner->RoleName()) == 0)
	{
		GameObjDbStructEx* ObjDB = Global::GetObjDB(Owner->BaseData.ItemInfo.OrgObjID);
		if (ObjDB == NULL)
			return NULL;
		return ObjDB->Name;
	}

	//§дKey
	const char* KeyStrResult = g_ObjectData->GetString(Owner->RoleName());
	if (strlen(KeyStrResult) == 0)
	{
		return Owner->RoleName();
	}

	return KeyStrResult;
}

const string	Global::GetRoleName_ASCII(RoleDataEx* Owner)
{
	return Utf8ToChar(GetRoleName(Owner));
}

//і]©wјР·ЗёЛіЖ
void		Global::SetStandardWearEq(RoleDataEx* Owner, int Level, int Rank)
{
	int		RankValue[] = { 0 ,2,5,9,12,15,20,24,38,35 };
	GameObjDbStructEx* ObjDB;
	DB_GameObjDbStruct* TempDBObj = NULL;

	g_ObjectData->ObjectDB()->GetDataByOrder(true);

	EQStruct& EQ = Owner->BaseData.EQ;
	EQ.Init();
	int					EQLevel[EM_EQWearPos_MaxCount];
	memset(EQLevel, 0, sizeof(EQLevel));

	if (Level <= 0 || Level > 20)
		Level = 0;

	for (int i = 0; i < g_ObjectData->ObjectDB()->TotalInfoCount(); i++)
	{
		ObjDB = g_ObjectData->ObjectDB()->GetDataByOrder();

		if (ObjDB == NULL)
			continue;

		//		if( ObjDB->Item.StandardType != EM_Yes )
		//			continue;

		if (ObjDB->Rare > Rank || ObjDB->Rare < 0)
			continue;

		int	itemLevel = ObjDB->Limit.Level + RankValue[ObjDB->Rare];


		if (ObjDB->IsWeapon())
		{
			//Owner->CheckSetEQ( EQPos , BodyOrgDB )

			if (ObjDB->Item.WeaponPos == EM_WeaponPos_MainHand
				|| ObjDB->Item.WeaponPos == EM_WeaponPos_SecondHand
				|| ObjDB->Item.WeaponPos == EM_WeaponPos_OneHand
				|| ObjDB->Item.WeaponPos == EM_WeaponPos_TwoHand)
			{
				switch (Owner->BaseData.Voc)
				{
				case EM_Vocation_Warrior:
					if (ObjDB->Item.WeaponType != EM_Weapon_Blade
						&& ObjDB->Item.WeaponType != EM_Weapon_Axe
						&& ObjDB->Item.WeaponType != EM_Weapon_Claymore
						&& ObjDB->Item.WeaponType != EM_Weapon_2H_Hammer)
						continue;
					break;
				case EM_Vocation_Ranger:
					if (ObjDB->Item.WeaponType != EM_Weapon_Blade
						&& ObjDB->Item.WeaponType != EM_Weapon_Dagger)
						continue;
					break;
				case EM_Vocation_Rogue:
					if (ObjDB->Item.WeaponType != EM_Weapon_Dagger)
						continue;
					break;
				case EM_Vocation_Wizard:
					if (ObjDB->Item.WeaponType != EM_Weapon_Staff
						&& ObjDB->Item.WeaponType != EM_Weapon_Wand)
						continue;
					break;
				case EM_Vocation_Priest:
					if (ObjDB->Item.WeaponType != EM_Weapon_Staff
						&& ObjDB->Item.WeaponType != EM_Weapon_Wand)
						continue;
					break;
				case EM_Vocation_Knight:
					if (ObjDB->Item.WeaponType != EM_Weapon_Blade)
						continue;
					break;
				case EM_Vocation_RuneDancer:
					if (ObjDB->Item.WeaponType != EM_Weapon_Staff
						&& ObjDB->Item.WeaponType != EM_Weapon_Wand)
						continue;
					break;
				case EM_Vocation_Druid:
					if (ObjDB->Item.WeaponType != EM_Weapon_Blade)
						continue;
					break;
				case EM_Vocation_Harpsyn:
					break;
				case EM_Vocation_Psyron:
					break;
				case  EM_Vocation_Duelist:
					break;
				}
			}


			if (itemLevel > EQLevel[EM_EQWearPos_MainHand])
			{
				if (Owner->CheckSetEQ(EM_EQWearPos_MainHand, ObjDB, false, 0) == EM_CheckSetEQResult_OK)
				{
					EQLevel[EM_EQWearPos_MainHand] = ObjDB->Limit.Level;
					EQ.MainHand.OrgObjID = ObjDB->GUID;
				}
			}

			if (itemLevel > EQLevel[EM_EQWearPos_SecondHand])
			{
				if (Owner->CheckSetEQ(EM_EQWearPos_SecondHand, ObjDB, false, 0) == EM_CheckSetEQResult_OK)
				{
					EQLevel[EM_EQWearPos_SecondHand] = ObjDB->Limit.Level;
					EQ.SecondHand.OrgObjID = ObjDB->GUID;
				}
			}

			if (itemLevel > EQLevel[EM_EQWearPos_Ammo])
			{
				if (Owner->CheckSetEQ(EM_EQWearPos_Ammo, ObjDB, false, 0) == EM_CheckSetEQResult_OK)
				{
					EQLevel[EM_EQWearPos_Ammo] = ObjDB->Limit.Level;
					EQ.Item[EM_EQWearPos_Ammo].OrgObjID = ObjDB->GUID;
				}
			}
			if (itemLevel > EQLevel[EM_EQWearPos_Bow])
			{
				if (Owner->CheckSetEQ(EM_EQWearPos_Bow, ObjDB, false, 0) == EM_CheckSetEQResult_OK)
				{
					EQLevel[EM_EQWearPos_Bow] = ObjDB->Limit.Level;
					EQ.Item[EM_EQWearPos_Bow].OrgObjID = ObjDB->GUID;
				}
			}
		}
		else if (ObjDB->IsArmor())
		{
			if (ObjDB->Item.ArmorPos == EM_ArmorPos_Necklace
				|| ObjDB->Item.ArmorPos == EM_ArmorPos_Ring
				|| ObjDB->Item.ArmorPos == EM_ArmorPos_Earring)
				continue;

			//int LimitLevel = ObjDB->Limit.Level;
			switch (ObjDB->Item.ArmorType)
			{
			case EM_Armor_HeavyArmed:
				itemLevel += 4;
				break;
			case EM_Armor_LightArmed:
				itemLevel += 3;
				break;
			case EM_Armor_Leather:
				itemLevel += 2;
				break;
			case EM_Armor_Clothes:
				break;
			case EM_Armor_Robe:
				itemLevel += 1;
				break;

			}

			if (itemLevel > EQLevel[EM_EQWearPos_SecondHand])
			{
				if (Owner->CheckSetEQ(EM_EQWearPos_SecondHand, ObjDB, false, 0) == EM_CheckSetEQResult_OK)
				{
					EQLevel[EM_EQWearPos_SecondHand] = itemLevel;
					EQ.SecondHand.OrgObjID = ObjDB->GUID;
				}
			}

			if (itemLevel > EQLevel[EM_EQWearPos_Head])
			{
				if (Owner->CheckSetEQ(EM_EQWearPos_Head, ObjDB, false, 0) == EM_CheckSetEQResult_OK)
				{
					EQLevel[EM_EQWearPos_Head] = itemLevel;
					EQ.Item[EM_EQWearPos_Head].OrgObjID = ObjDB->GUID;
				}
			}
			if (itemLevel > EQLevel[EM_EQWearPos_Gloves])
			{
				if (Owner->CheckSetEQ(EM_EQWearPos_Gloves, ObjDB, false, 0) == EM_CheckSetEQResult_OK)
				{
					EQLevel[EM_EQWearPos_Gloves] = itemLevel;
					EQ.Item[EM_EQWearPos_Gloves].OrgObjID = ObjDB->GUID;
				}
			}
			if (itemLevel > EQLevel[EM_EQWearPos_Shoes])
			{
				if (Owner->CheckSetEQ(EM_EQWearPos_Shoes, ObjDB, false, 0) == EM_CheckSetEQResult_OK)
				{
					EQLevel[EM_EQWearPos_Shoes] = itemLevel;
					EQ.Item[EM_EQWearPos_Shoes].OrgObjID = ObjDB->GUID;
				}
			}
			if (itemLevel > EQLevel[EM_EQWearPos_Clothes])
			{
				if (Owner->CheckSetEQ(EM_EQWearPos_Clothes, ObjDB, false, 0) == EM_CheckSetEQResult_OK)
				{
					EQLevel[EM_EQWearPos_Clothes] = itemLevel;
					EQ.Item[EM_EQWearPos_Clothes].OrgObjID = ObjDB->GUID;
				}
			}
			if (itemLevel > EQLevel[EM_EQWearPos_Pants])
			{
				if (Owner->CheckSetEQ(EM_EQWearPos_Pants, ObjDB, false, 0) == EM_CheckSetEQResult_OK)
				{
					EQLevel[EM_EQWearPos_Pants] = itemLevel;
					EQ.Item[EM_EQWearPos_Pants].OrgObjID = ObjDB->GUID;
				}
			}
			if (itemLevel > EQLevel[EM_EQWearPos_Belt])
			{
				if (Owner->CheckSetEQ(EM_EQWearPos_Belt, ObjDB, false, 0) == EM_CheckSetEQResult_OK)
				{
					EQLevel[EM_EQWearPos_Belt] = itemLevel;
					EQ.Item[EM_EQWearPos_Belt].OrgObjID = ObjDB->GUID;
				}
			}
			if (itemLevel > EQLevel[EM_EQWearPos_Shoulder])
			{
				if (Owner->CheckSetEQ(EM_EQWearPos_Shoulder, ObjDB, false, 0) == EM_CheckSetEQResult_OK)
				{
					EQLevel[EM_EQWearPos_Shoulder] = itemLevel;
					EQ.Item[EM_EQWearPos_Shoulder].OrgObjID = ObjDB->GUID;
				}
			}
		}
	}

	for (int i = 0; i < EM_EQWearPos_MaxCount; i++)
	{
		ObjDB = g_ObjectData->GetObj(EQ.Item[i].OrgObjID);
		if (ObjDB == NULL)
		{
			Owner->Net_FixItemInfo_EQ(i);
			continue;
		}

		EQ.Item[i].CreateTime = RoleDataEx::G_Now;
		EQ.Item[i].Serial = Global::GenerateItemVersion();
		EQ.Item[i].Durable = ObjDB->Item.Durable * 100;
		EQ.Item[i].Level = Level;
		EQ.Item[i].Quality = 100;
		EQ.Item[i].PowerQuality = 10;
		EQ.Item[i].OrgQuality = 100;

		EQ.Item[i].Mode._Mode = 0;
		EQ.Item[i].Count = ObjDB->MaxHeap;
		EQ.Item[i].ImageObjectID = EQ.Item[i].OrgObjID;
		Owner->Net_FixItemInfo_EQ(i);
	}
	Owner->TempData.UpdateInfo.Eq = true;
}

bool		Global::ChangeParalledID(int GItemID, int ParalledID)
{

	if (GItemID != -1)
	{
		RoleDataEx* Role = Global::GetRoleDataByGUID(GItemID);
		if (Role == NULL)
			return false;

		Role->PlayerTempData->ParallelZoneID = ParalledID;
		NetSrv_CliConnect::S_ParalledID(Role->GUID(), Role->PlayerTempData->ParallelZoneID);

		if (Role->RoomID() == 0)
		{
			//int ZoneID = Role->G_ZoneID % _DEF_ZONE_BASE_COUNT_ + _DEF_ZONE_BASE_COUNT_ * ParallelID;
			int ZoneID = GlobalBase::GetParallelZoneID(Role->G_ZoneID, ParalledID);
			if (ZoneID == -1)
			{
				Role->Net_GameMsgEvent(EM_GameMessageEvent_ChanageZoneError_Failed);
				return false;
			}
			if (RoleDataEx::G_ZoneID != ZoneID)
			{
				Role->BaseData.SysFlag.IsNeedSetPosition = true;
				Global::ChangeZone(Role->GUID(), ZoneID, 0, Role->X(), Role->Y(), Role->Z(), 0);
				Role->BaseData.SysFlag.IsNeedSetPosition = false;
			}
			else
			{
				Role->TempData.UpdateInfo.AllZoneInfoChange = true;
			}

		}

	}
	else
	{
		set<BaseItemObject* >::iterator Iter;

		for (Iter = BaseItemObject::PlayerObjSet()->begin(); Iter != BaseItemObject::PlayerObjSet()->end(); Iter++)
		{
			RoleDataEx* Role = (*Iter)->Role();
			Role->PlayerTempData->ParallelZoneID = ParalledID;
			NetSrv_CliConnect::S_ParalledID(Role->GUID(), Role->PlayerTempData->ParallelZoneID);

			if (Role->RoomID() == 0)
			{
				int ZoneID = GlobalBase::GetParallelZoneID(Role->G_ZoneID, ParalledID);
				if (ZoneID == -1)
				{
					Role->Net_GameMsgEvent(EM_GameMessageEvent_ChanageZoneError_Failed);
					continue;
				}
				if (RoleDataEx::G_ZoneID != ZoneID)
				{
					Role->BaseData.SysFlag.IsNeedSetPosition = true;
					Global::ChangeZone(Role->GUID(), ZoneID, 0, Role->TempData.Move.LOK_CliX, Role->TempData.Move.LOK_CliY, Role->TempData.Move.LOK_CliZ, 0);
					Role->BaseData.SysFlag.IsNeedSetPosition = false;
				}
				else
				{
					Role->TempData.UpdateInfo.AllZoneInfoChange = true;
				}
			}
		}



	}
	return true;
}
bool		Global::FindNewHateTarget(RoleDataEx* Owner)
{
	GameObjDbStructEx* OrgDB = Global::GetObjDB(Owner->BaseData.ItemInfo.OrgObjID);
	if (OrgDB->IsNPC())
		return FindNewHateTarget(Owner, (float)(OrgDB->NPC.FollowRange));
	return FindNewHateTarget(Owner, _DEF_NPC_FIGHT_RANGE_);
}
bool		Global::FindNewHateTarget(RoleDataEx* Owner, float Range)
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObj(Owner->GUID());

	if (!Owner->IsNPC())
		return false;

	if (Owner->IsAttack() == false)
		return false;

	if (Owner->IsDead() == true)
		return false;

	BaseItemObject* AttTargetObj = BaseItemObject::GetObj(Owner->TempData.AI.TargetID);
	if (AttTargetObj != NULL
		&& AttTargetObj->Role()->IsDead() == false
		&& AttTargetObj->Role()->TempData.Attr.Effect.LockTarget == true)
		return false;


	RoleDataEx* Target = Owner->SearchHate(Range);
	if (Target == NULL)
	{

		RoleDataEx* OrgOwner = Owner->GetOrgOwner();
		if (OrgOwner->IsPlayer())
		{
			Owner->SetAtkFolOff();
			return false;
		}


		PathProcStruct& Move = Owner->TempData.Move;

		if (Owner->TempData.Attr.Action.NoReturnAttackPoint == false)
		{
			if (Move.RetX != 0 || Move.RetY != 0 || Move.RetZ != 0)
			{
				OwnerObj->Path()->Target(Move.RetX, Move.RetY, Move.RetZ, Owner->X(), Owner->Y(), Owner->Z());

				NpcAIBaseClass* AI = (NpcAIBaseClass*)OwnerObj->AI();//->Type = EM_NPCMoveFlagAIType_WaitMoveEx;
				if (AI)
				{
					AI->GetMoveStatus()->Type = EM_NPCMoveFlagAIType_WaitMoveEx;
					AI->GetMoveStatus()->CountDown = 0;
				}

				if (Owner->TempData.Attr.Effect.StopOntimeCure == false)
				{
					Owner->SetValue(EM_RoleValue_HP, Owner->TempData.Attr.Fin.MaxHP, NULL);
					Owner->SetValue(EM_RoleValue_MP, Owner->TempData.Attr.Fin.MaxMP, NULL);
					//Owner->ClearAllBuff();
					for (int i = Owner->BaseData.Buff.Size() - 1; i >= 0; i--)
					{
						GameObjDbStructEx* MagicBaseDB = (GameObjDbStructEx*)Owner->BaseData.Buff[i].Magic;
						if (MagicBaseDB->IsMagicBase() == true
							&& MagicBaseDB->MagicBase.Setting.NpcEndFight_NotClearBuff == true)
						{
							continue;
						}

						Owner->ClearBuffByPos(i);
					}
				}

				Owner->IsWalk(false);
				int MoveTime = int((OwnerObj->Path()->TotalLength()) / Owner->MoveSpeed() + 1.5);
				if (Owner->BaseData.Mode.Move == true)
					Owner->AssistMagic(Owner, RoleDataEx::BaseMagicList[EM_BaseMagic_Unbeatable], 0, false, MoveTime);

				Owner->TempData.NPCHate.Init();
			}
		}

		Owner->SetAtkFolOff();
		return false;
	}
	Owner->SetAttack(Target);

	return true;
}


int	Global::_CheckZoneShutdown(SchedularInfo* Obj, void* InputClass)   //°±ѕчЁЖҐу
{
	char szMsg[1024];

	if (m_iShutdownTimes > 0)
	{
		if ((m_iShutdownTimes % 60) == 0)
		{
			int iMin = m_iShutdownTimes / 60;
			sprintf_s(szMsg, sizeof(szMsg), "[ZONE_MSGKEY_MIN_SHUTDOWN][$SETVAR1=%d]", iMin);
			NetSrv_Talk::SendRunningMsg_Zone(szMsg);
		}

		if (m_iShutdownTimes == 30)
		{
			sprintf_s(szMsg, sizeof(szMsg), "[ZONE_MSGKEY_SEC_SHUTDOWN][$SETVAR1=%d]", m_iShutdownTimes);
			NetSrv_Talk::SendRunningMsg_Zone(szMsg);
		}

		if (m_iShutdownTimes == 20 || m_iShutdownTimes == 15)
		{
			sprintf_s(szMsg, sizeof(szMsg), "[ZONE_MSGKEY_SEC_SHUTDOWN][$SETVAR1=%d]", m_iShutdownTimes);
			NetSrv_Talk::GMMsg(-1, szMsg);
		}

		if (m_iShutdownTimes <= 10)
		{
			sprintf_s(szMsg, sizeof(szMsg), "[ZONE_MSGKEY_SEC_SHUTDOWN][$SETVAR1=%d]", m_iShutdownTimes);
			NetSrv_Talk::GMMsg(-1, szMsg);
		}
	}

	if (m_iShutdownTimes == 1)
	{
		for (set< BaseItemObject*>::iterator it = BaseItemObject::PlayerObjSet()->begin(); it != BaseItemObject::PlayerObjSet()->end(); it++)
		{
			BaseItemObject* pObj = *it;
			if (pObj == NULL)
				continue;

			Global::Net()->CliLogout(pObj->Role()->TempData.Sys.SockID,
				Net_ServerList->m_ServerType,
				Net_ServerList->m_dwServerLocalID,
				(char*)pObj->Role()->Account_ID(),
				"Server shutdown");
		}
	}

	if (m_iShutdownTimes == -60
		|| (BaseItemObject::PlayerCount() == 0 && m_iShutdownTimes < 0 && m_iShutdownTimes > -1000))
	{
		// µoҐX©RҐO­nЁD DataCenter Аx¦s¤HЄ«.
		//Global::SaveAllPlayer_byCrash();
		HousesManageClass::OnTimeProcAll(true);
		Global::Log_ZoneMoneyLog();
		m_iShutdownTimes = -1000;
	}

	if (m_iShutdownTimes < -1060)
	{
		Global::Destory();
		return 0;
	}

	m_iShutdownTimes--;

	Schedular()->Push(Global::_CheckZoneShutdown, 1000, NULL, NULL);
	return 0;
}


//µҐЇЕ®t
float	Global::DLvRate(int DLv, bool IsPlayer)
{
	if (IsPlayer == true)
		return 1;

	static vector< float > DLvRateTable;

	if (DLvRateTable.size() == 0)
	{
		for (int i = 0; i < 101; i++)
			DLvRateTable.push_back(1.0f);

		for (int i = 49; i >= 0; i--)
		{
			DLvRateTable[i] = DLvRateTable[i + 1] * 0.95f;
		}

		for (int i = 51; i < 101; i++)
		{
			DLvRateTable[i] = DLvRateTable[i - 1] * 1.15f;
			if (DLvRateTable[i] > 2)
				DLvRateTable[i] = 2;
		}
	}

	int Value = DLv + 50;
	if (Value < 0)
		Value = 0;
	if (Value > 100)
		Value = 100;

	if (IsPlayer == true && Value > 50)
		Value = 50;
	else if (IsPlayer == false && Value < 50)
		Value = 50;

	return DLvRateTable[Value];
}
//////////////////////////////////////////////////////////////////////////
//µЅґc­И
void	Global::GoodEvilProc(RoleDataEx* Owner, float Value)
{
	if (Global::Ini()->DisableGoodEvil == true)
		return;
	//	if( Owner->RoomID() != 0 )
	//		return;
	if (RoleDataEx::G_IsPvE == true)
		return;

	if (Value > Owner->PlayerBaseData->GoodEvilBonus)
	{
		Value = Owner->PlayerBaseData->GoodEvilBonus;
		Owner->PlayerBaseData->GoodEvilBonus = 0;
	}
	else if (Value > 0)
	{
		Owner->PlayerBaseData->GoodEvilBonus -= Value;
	}

	GoodEvilTypeENUM OrgGoodEvilType = Owner->GetGoodEvilType();
	float OrgGoodEvilValue = Owner->PlayerBaseData->GoodEvil;
	Owner->PlayerBaseData->GoodEvil += Value;

	if (Owner->PlayerBaseData->GoodEvil > 999)
		Owner->PlayerBaseData->GoodEvil = 999;
	else if (Owner->PlayerBaseData->GoodEvil < -9999)
		Owner->PlayerBaseData->GoodEvil = -9999;

	GoodEvilTypeENUM GoodEvilType = Owner->GetGoodEvilType();
	if (Value != 0)
		Owner->Net_FixRoleValue(EM_RoleValue_GoodEvil, Owner->PlayerBaseData->GoodEvil);

	if (int(OrgGoodEvilValue) != int(Owner->PlayerBaseData->GoodEvil))
	{
		NetSrv_OtherChild::SendRange_GoodEvilValue(Owner);
	}

	if (OrgGoodEvilType != GoodEvilType)
	{
		BuffBaseStruct* RetBuf = NULL;
		RetBuf = Owner->AssistMagic(Owner, g_ObjectData->SysValue().GoodEvilBuf[GoodEvilType - EM_GoodEvil_Demon], 0, false, -1);

		if (RetBuf != NULL)
		{
			NetSrv_MagicChild::SendRange_AddBuffInfo(Owner->GUID(), Owner->GUID(), g_ObjectData->SysValue().GoodEvilBuf[GoodEvilType - EM_GoodEvil_Demon], 0, -1);
		}
	}

	//АЛ¬d°}Аз¬O§_¦і§пЕЬ
	if (Owner->PlayerBaseData->GoodEvil > -30.0f)
	{
		if (Owner->TempData.AI.CampID == EM_CampID_Evil)
		{
			Owner->TempData.AI.CampID = EM_CampID_Good;
			NetSrv_OtherChild::SendRangeCampID(Owner, Owner->TempData.AI.CampID);
		}
		else if (Owner->TempData.AI.CampID == EM_CampID_NewHand)
		{
			if (Owner->Level() >= 15)
			{
				Owner->TempData.AI.CampID = EM_CampID_Good;
				NetSrv_OtherChild::SendRangeCampID(Owner, Owner->TempData.AI.CampID);
			}
		}
	}
	else
	{
		if (Owner->TempData.AI.CampID == EM_CampID_Good || Owner->TempData.AI.CampID == EM_CampID_NewHand)
		{
			Owner->TempData.AI.CampID = EM_CampID_Evil;
			NetSrv_OtherChild::SendRangeCampID(Owner, Owner->TempData.AI.CampID);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//Є±®a¦є¤` PK µЅґc­И ­pєв
void	Global::PlayerDead_CalGoodEvil(RoleDataEx* Deader, RoleDataEx* Killer)
{
	static int		SerialID = 100000;	//«НЕйАЛ¬dДтё№

//	if( Deader->RoomID() != 0 )
//		return;

	if (RoleDataEx::G_IsPvE == true)
		return;


	//////////////////////////////////////////////////////////////////////////
	const float KillerProcValue[9] = { 10,3,2,1,-10,-15,-20,-25,-40 };
	const float DeaderProcValue[9] = { 0 ,-4,-6,-8,-10,0,0,0,0 };

	if (Deader == Killer)
		return;

	GoodEvilTypeENUM DeaderType = Deader->GetGoodEvilType();
	GoodEvilTypeENUM KillerType = Killer->GetGoodEvilType();


	if (Killer->CheckGuildWarDeclare(Deader) == false)
	{
		bool	IsProtect_Killer = false;

		if (Deader->TempData.Attr.Effect.Guilty == true || Deader->TempData.Attr.Effect.Hunter)
		{
			IsProtect_Killer = true;
		}

		if (IsProtect_Killer && DeaderType >= EM_GoodEvil_Normal)
		{

		}
		else if (DeaderType > EM_GoodEvil_Normal && Deader->TempData.Attr.Effect.PKFlag)
		{
			//return;
		}
		else
		{
			//¦pЄG¦єЄМ¬OµЅ¤и 
			if (DeaderType >= EM_GoodEvil_Normal)
			{
				//¦pЄG¦єЄМ¦Ы¤v¶}PK ¤Ј°µҐф¦у ±ю¤HЄМЄєіBІz
				if (Deader->TempData.Attr.Effect.PKFlag == true)
				{

				}
				else
				{
					GoodEvilProc(Killer, KillerProcValue[DeaderType + 4]);
				}

				GoodEvilProc(Deader, DeaderProcValue[DeaderType]);
			}
			else
			{
				if (KillerType >= 0)
				{
					if (Killer->Level() - Deader->Level() < 8)
						GoodEvilProc(Killer, KillerProcValue[DeaderType + 4]);
				}
			}

		}
	}
	//////////////////////////////////////////////////////////////////////////
	if (Killer->Level() - Deader->Level() >= 5)
		return;

	if (Deader->TempData.Sys.OnChangeZone)
		return;

	//¦є¤`ГzёЛіBІz
	GameObjDbStructEx* BoxDB = Global::GetObjDB(g_ObjectData->SysValue().TreasureBaseObjID);

	bool	IsPkProtectItem = false;
	for (int i = 0; i < _MAX_BODY_COUNT_; i++)
	{
		ItemFieldStruct& Item = Deader->PlayerBaseData->Body.Item[i];
		if (Item.OrgObjID == g_ObjectData->SysValue().PK_ProtectEq_ItemID)
		{

			Log_ItemDestroy(Deader, EM_ActionType_PKItemProtect, Item, 1, -1, "");
			if (Item.Count <= 1)
				Item.Init();
			else
				Item.Count--;

			Deader->Net_FixItemInfo_Body(i);

			Deader->Net_GameMsgEvent(EM_GameMessageEvent_PKItemProtect);
			IsPkProtectItem = true;
			break;
		}
	}

	if (BoxDB != NULL && Deader->TempData.Sys.OnChangeZone == false && IsPkProtectItem == false)
	{

		int DropRate[9] = { 80 , 60 , 50 , 40 , 0 , 8 , 7 , 5 ,0 };
		vector<ItemFieldStruct> DropItemList;

		if ((DeaderType == EM_GoodEvil_Demon && KillerType == EM_GoodEvil_Hero)
			|| (DeaderType == EM_GoodEvil_Hero && KillerType == EM_GoodEvil_Demon))
		{
			DropRate[0] = 100;
			DropRate[8] = 100;
		}

		GameObjDbStructEx* ItemObjDB;

		if (rand() % 100 < DropRate[DeaderType + 4])
		{
			set< int >	BodyItemDSet;
			vector< int > ItemPosList;

			for (int i = 0; i <= EM_EQWearPos_SecondHand; i++)
			{
				ItemFieldStruct& Item = Deader->BaseData.EQ.Item[i];
				GameObjDbStructEx* DropItemObjDB = Global::GetObjDB(Item.OrgObjID);
				if (DropItemObjDB == NULL || DropItemObjDB->Mode.PkNoDrop == true)
					continue;
				if (Item.IsEmpty() || Item.Mode.PkProtect)
					continue;

				ItemPosList.push_back(i);
			}

			for (int i = _MAX_BODY_BEGIN_POS_; i < _MAX_BODY_COUNT_; i++)
			{
				ItemFieldStruct& Item = Deader->PlayerBaseData->Body.Item[i];

				GameObjDbStructEx* DropItemObjDB = Global::GetObjDB(Item.OrgObjID);
				if (DropItemObjDB == NULL || DropItemObjDB->Mode.PkNoDrop == true)
					continue;

				if (Item.IsEmpty() || Item.Mode.PkProtect)
					continue;

				ItemObjDB = Global::GetObjDB(Item.OrgObjID);
				if (ItemObjDB == NULL)
					continue;

				if (ItemObjDB->IsItem_Pure() && ItemObjDB->Item.ItemType == EM_ItemType_Plot)
					continue;


				if (BodyItemDSet.insert(Item.OrgObjID).second == false)
					continue;

				ItemPosList.push_back(i + 1000);
			}

			for (int i = 0; i < 2; i++)
			{
				if (ItemPosList.size() == 0)
					break;

				int RandID = rand() % (int)ItemPosList.size();
				int PosID = ItemPosList[RandID];
				ItemPosList.erase(ItemPosList.begin() + RandID);
				if (PosID < 1000)
				{
					ItemFieldStruct& Item = Deader->BaseData.EQ.Item[PosID];
					Log_ItemDestroy(Deader, EM_ActionType_PKDropItem, Item, -1, -1, "");
					DropItemList.push_back(Item);
					NetSrv_Other::SC_PKDeadDropItem(Deader->GUID(), Item);
					Item.Init();
					Deader->Net_FixItemInfo_EQ(PosID);
					Deader->TempData.UpdateInfo.Eq = true;
				}
				else
				{
					ItemFieldStruct& Item = Deader->PlayerBaseData->Body.Item[PosID % 1000];
					Log_ItemDestroy(Deader, EM_ActionType_PKDropItem, Item, -1, -1, "");
					DropItemList.push_back(Item);
					NetSrv_Other::SC_PKDeadDropItem(Deader->GUID(), Item);
					Item.Init();
					Deader->Net_FixItemInfo_Body(PosID % 1000);
					Deader->TempData.UpdateInfo.Body = true;
				}
			}

			if (DropItemList.size() != 0)
			{
				int BoxGUID = Global::CreateObj(BoxDB->GUID, Deader->X(), Deader->Y(), Deader->Z(), Deader->Dir(), 1);
				BaseItemObject* BoxObj = BaseItemObject::GetObj(BoxGUID);

				if (BoxObj == NULL)
				{
					char Buf[256];
					sprintf_s(Buf, sizeof(Buf), "Create Error!! OrgObjID = %d", BoxDB->GUID);
					Print(Def_PrintLV3, "CreateTreasureBox", Buf);
					return;
				}

				RoleDataEx* BoxRoleData = BoxObj->Role();
				ObjectModeStruct& Mode = BoxRoleData->BaseData.Mode;
				Mode.Strikback = false;
				Mode.Move = false;
				Mode.Searchenemy = false;
				Mode.Revive = false;
				Mode.Fight = false;
				Mode.HideName = true;
				Mode.HideMinimap = false;
				Mode.ShowRoleHead = false;
				Mode.Treasure = true;

				BoxRoleData->LiveTime(180 * 1000, "Treasure Obj");

				vector<int> OwnerDBIDList;
				vector<bool> LockList;
				for (int i = 0; i < (int)DropItemList.size(); i++)
				{
					OwnerDBIDList.push_back(-1);
					LockList.push_back(false);
				}


				if (CreateTreasure(Killer->DBID(), Killer->PartyID(), BoxRoleData, DropItemList, OwnerDBIDList, LockList) == true)
				{
					BoxRoleData->DynamicData.TreasureBox->CheckSerialID = SerialID++;
					Global::AddToPartition(BoxGUID, Deader->BaseData.RoomID);
					Schedular()->Push(_CheckTreasureLootRight, 60 * 1000, NULL
						, "GUID", EM_ValueType_Int, BoxGUID
						, "SerialID", EM_ValueType_Int, BoxRoleData->DynamicData.TreasureBox->CheckSerialID
						, NULL);
				}
			}

		}
	}
	//////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
int		Global::_CheckTreasureLootRight(SchedularInfo* Obj, void* InputClass)
{
	int GUID = Obj->GetNumber("GUID");
	int SerialID = Obj->GetNumber("SerialID");

	RoleDataEx* Treasure = Global::GetRoleDataByGUID(GUID);
	if (Treasure == NULL
		|| Treasure->DynamicData.TreasureBox == NULL
		|| Treasure->BaseData.Mode.Treasure == false
		|| Treasure->DynamicData.TreasureBox->CheckSerialID != SerialID)
		return 0;


	TreasureBoxStruct& TreasureBox = *(Treasure->DynamicData.TreasureBox);
	//	if( unsigned( TreasureBox.DeadTime + 60 ) < RoleDataEx::G_Now )
	{
		if (TreasureBox.TreasurePartyID != -1 || TreasureBox.TreasureOwnerDBID != -1)
		{
			TreasureBox.TreasurePartyID = -1;
			TreasureBox.TreasureOwnerDBID = -1;
			NetSrv_TreasureChild::SendRange_TreasureLootInfo(Treasure);
		}
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////
bool	Global::CreateTeleport(float CreateX, float CreateY, float CreateZ, int ToZone, float ToX, float ToY, float ToZ, const char* Name, int Pid)
{

	int BoxGUID = Global::CreateObj(g_ObjectData->SysValue().Teleport_PointObjID, CreateX, CreateY, CreateZ, 0, 1);
	BaseItemObject* BoxObj = BaseItemObject::GetObj(BoxGUID);

	if (BoxObj == NULL)
	{
		char Buf[256];
		sprintf_s(Buf, sizeof(Buf), "Create Error!! OrgObjID = %d", g_ObjectData->SysValue().Teleport_PointObjID);
		Print(Def_PrintLV3, "CreateTeleport", Buf);
		return false;
	}

	RoleDataEx* BoxRoleData = BoxObj->Role();
	ObjectModeStruct& Mode = BoxRoleData->BaseData.Mode;
	Mode.Strikback = false;
	Mode.Move = false;
	Mode.Searchenemy = false;
	Mode.Revive = false;
	Mode.Fight = false;
	Mode.HideName = true;
	Mode.HideMinimap = false;
	Mode.ShowRoleHead = false;

	BoxRoleData->RoleName((char*)Name);
	BoxRoleData->SelfData.PID = Pid;
	BoxRoleData->SelfData.ReturnPos.Dir = 0;
	BoxRoleData->SelfData.ReturnPos.X = ToX;
	BoxRoleData->SelfData.ReturnPos.Y = ToY;
	BoxRoleData->SelfData.ReturnPos.Z = ToZ;
	BoxRoleData->SelfData.ReturnZoneID = ToZone;
	//	BoxRoleData->TempData.TouchPlot = "Sys_TeleportObj";
	//	BoxRoleData->TempData.TouchPlot_Range = 50;
	BoxRoleData->SelfData.AutoPlot = "Sys_TeleportObj_Init";
	BoxObj->PlotVM()->CallLuaFunc(BoxRoleData->SelfData.AutoPlot.Begin(), BoxRoleData->GUID(), 0);

	// јЛЄ©Єм©lј@±Ў
	{
		GameObjDbStructEx* pDBObj = Global::GetObjDB(BoxRoleData->BaseData.ItemInfo.OrgObjID);

		if (pDBObj && (pDBObj->IsNPC() || pDBObj->IsQuestNPC()))
		{
			if (strlen(pDBObj->NPC.szLuaInitScript) != 0)
			{
				BoxObj->PlotVM()->CallLuaFunc(pDBObj->NPC.szLuaInitScript, BoxObj->GUID());
			}
		}
	}

	BoxRoleData->LiveTime(180 * 1000, "Teleport Obj");
	Global::AddToPartition(BoxGUID, 0);

	//¦¬ґM¬O§_¦іЄ«Ґу¤w¦b¦№¦мёm
	vector<BaseSortStruct>* SearchResut = SearchRangeObject(BoxRoleData, CreateX, CreateY, CreateZ, EM_SearchRange_All, 5);
	for (int i = 0; i < (int)SearchResut->size(); i++)
	{
		RoleDataEx* Role = (RoleDataEx*)(*SearchResut)[i].Data;
		if (Role->GUID() == BoxRoleData->GUID())
			continue;
		if (Role->IsPlayer())
			continue;
		if (stricmp(Role->RoleName(), BoxRoleData->RoleName()) == 0)
			Role->Destroy("Teleport Obj Destroy");
	}


	return true;
}

bool Global::CreateTreasure(int OwnerDBID, int OwnerPartyID, RoleDataEx* Deader, vector<ItemFieldStruct>& DropItemList, vector<int>& OwnerDBIDList, vector<bool>& LockPlayer)
{
	RoleDataEx* BoxRoleData = Deader;

	GameObjDbStructEx* DeadObjDB = Global::GetObjDB(Deader->BaseData.ItemInfo.OrgObjID);
	if (DeadObjDB == NULL)
		return false;

	ObjectModeStruct& Mode = BoxRoleData->BaseData.Mode;
	Mode.Treasure = true;

	if (BoxRoleData->DynamicData.TreasureBox == NULL)
	{
		BoxRoleData->DynamicData.TreasureBox = NEW TreasureBoxStruct;
	}

	//Ё­Ейёк®Жі]©w
	TreasureBoxStruct& TreasureBox = *(BoxRoleData->DynamicData.TreasureBox);
	TreasureBox.Count = int(__min(DropItemList.size(), 100));
	for (int i = 0; i < TreasureBox.Count; i++)
	{
		TreasureBox.Item[i] = DropItemList[i];
		TreasureBox.OwnerDBID[i] = OwnerDBIDList[i];
		TreasureBox.Lock[i] = LockPlayer[i];

	}
	if (OwnerPartyID == -1)
	{
		TreasureBox.TreasureOwnerDBID = OwnerDBID;
		TreasureBox.TreasurePartyID = -1;
	}
	else
	{
		TreasureBox.TreasurePartyID = OwnerPartyID;
		TreasureBox.TreasureOwnerDBID = OwnerDBID;
	}

	if (DeadObjDB->ReviveTime != -1)
		//TreasureBox.LiveTime =  RoleDataEx::G_Now +  __min( 60*10 , __max( DeadObjDB->ReviveTime/2 , 90 ) );
		TreasureBox.LiveTime = RoleDataEx::G_Now + RangeValue(90, 600, DeadObjDB->ReviveTime / 2);
	else
		TreasureBox.LiveTime = RoleDataEx::G_Now + 180;

	TreasureBox.DeadTime = RoleDataEx::G_Now;

	NetSrv_TreasureChild::SendRange_TreasureLootInfo(Deader);
	return true;
}
//////////////////////////////////////////////////////////////////////////
SysKeyValueStruct* Global::SysKeyValue()
{
	return &(g_ObjectData->SysValue());
}
//////////////////////////////////////////////////////////////////////////

void		Global::TimeFlagProc(RoleDataEx* Owner)
{
	PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo(Owner->PartyID());
	int* TimeFlag = Owner->PlayerBaseData->TimeFlag;

	//	EM_TimeFlagEffect_HonorParty		= 0 ,	//єaЕAІХ¶¤
	if (Owner->TempData.Attr.AllZoneState.HonorParty == true)
	{
		if (TimeFlag[EM_TimeFlagEffect_HonorParty] < 0)
		{
			Owner->TempData.Attr.AllZoneState.HonorParty = false;
			Owner->TempData.UpdateInfo.AllZoneInfoChange = true;
		}
	}
	else
	{
		if (TimeFlag[EM_TimeFlagEffect_HonorParty] > 0)
		{
			Owner->TempData.Attr.AllZoneState.HonorParty = true;
			Owner->TempData.UpdateInfo.AllZoneInfoChange = true;
		}
	}

	//¬d¬O§_¦№Ё¤¦вҐґ©З¦іExp
	if (Party != NULL
		&& Owner->TempData.Attr.AllZoneState.HonorParty == true
		&& strcmp(Owner->BaseData.RoleName.Begin(), Party->Info.LeaderName.Begin()) == 0)
	{
		Owner->BaseData.SysFlag.IsHonorLeader = true;
	}
	else
	{
		Owner->BaseData.SysFlag.IsHonorLeader = false;
	}


	//		EM_TimeFlagEffect_BankBag25_49			,	//»И¦ж­IҐ] ¶}©с
	///		EM_TimeFlagEffect_BankBag50_74			,	//»И¦ж­IҐ] ¶}©с
	//		EM_TimeFlagEffect_BankBag75_99			,	//»И¦ж­IҐ] ¶}©с
}
//////////////////////////////////////////////////////////////////////////
#if 0
void		Global::Log_Verion()
{
	char	Buf[512];
	char	ExeFileName[MAX_PATH];
	//јg¤JVersionёк®Ж
	GetModuleFileName(NULL, ExeFileName, MAX_PATH);

	CFileVersion Version;
	if (Version.Open(ExeFileName))
	{
		CString ver = Version.GetFixedFileVersion();
		const char* verStr = ver.GetString();

		sprintf_s(Buf, sizeof(Buf), "INSERT INTO ServerStartLog( Version,Server) VALUES ( '%s' , 'Zone%d' )"
			, verStr
			, Ini()->ZoneID);

		Net_DCBase::LogSqlCommand(Buf);
}

}
#endif
//-------------------------------------------------------------------------------------------
//	№УёOіBІz
//-------------------------------------------------------------------------------------------
//¬dґM№УёO¦pЄG¤ЈЁЈ«h­««Ш
void	Global::TombProcess(RoleDataEx* Owner)
{
	//	const int OneDay = (60*24*60 );

	TombStruct& Tomb = Owner->PlayerSelfData->Tomb;
	if (Tomb.ZoneID != RoleDataEx::G_ZoneID % 1000)
		return;
	/*
		int LiveTime = Tomb.CreateTime + OneDay - TimeStr::Now_Value();
		if( LiveTime < 0 )
			return;
	*/

	RoleDataEx* TombRole = Global::GetRoleDataByGUID(Tomb.ItemGUID);
	if (TombRole != NULL
		&& TombRole->BaseData.ItemInfo.OrgObjID == g_ObjectData->SysValue().TombObjID
		&& TombRole->TempData.Sys.OwnerDBID == Owner->DBID())
		return;

	if (Tomb.Exp == 0 && Tomb.DebtTp == 0 && Tomb.DebtExp == 0)
		return;

	Tomb.ItemGUID = CreateTomb(Owner, Tomb.X, Tomb.Y, Tomb.Z);

}
//ІЈҐН№УёO
int Global::CreateTomb(RoleDataEx* Role, float X, float Y, float Z)
{
	//¦pЄG­мҐ»Єє№УёOБЩ¦bЁдҐ¦ЄєServer«h§вВВЄє№УёO§R°Ј
	if (Role->PlayerSelfData->Tomb.CreateTime + 24 * 60 * 60 > (int)TimeStr::Now_Value())
	{
		NetSrv_CliConnect::SL_DelTomb(Role->PlayerSelfData->Tomb.ZoneID, Role->PlayerSelfData->Tomb.ItemGUID, Role->DBID());
		//Role->PlayerSelfData->Tomb.Init();
	}

	int TombID = CreateObj(g_ObjectData->SysValue().TombObjID, X, Y, Z, float(rand() % 360), 1);

	BaseItemObject* TombObj = Global::GetObj(TombID);
	if (TombObj == NULL)
	{
		Print(LV1, "CreateTomb", "Tomb Object Create Error!!");
		return -1;
	}

	RoleDataEx* TombRole = TombObj->Role();

	TombRole->BaseData.RoleName = Role->RoleName();
	TombRole->TempData.Sys.OwnerDBID = Role->DBID();
	TombRole->TempData.Sys.OwnerGUID = Role->GUID();
	TombRole->BaseData.Mode.Strikback = false;
	TombRole->BaseData.Mode.Move = false;
	TombRole->BaseData.Mode.Searchenemy = false;
	TombRole->BaseData.Mode.Revive = false;
	TombRole->BaseData.Mode.Fight = false;
	TombRole->BaseData.Mode.ShowRoleHead = false;
	//	TombRole->BaseData.Mode.Gravity			= false;

	//	TombRole->LiveTime( LiveTime*1000 , "CreateTomb" );

		//і]©wЅdітј@±Ў
	//	TombRole->TempData.RangePlot = "Sys_TouchTomb";
	//	TombRole->TempData.RangePlot_Range = 150;
	TombRole->TempData.TouchPlot = "Sys_TouchTomb";
	TombRole->TempData.TouchPlot_Range = 40;
	TombRole->TempData.CursorType = 13;


	AddToPartition(TombID, Role->RoomID());

	return TombID;
}

int	Global::RoomPlayerCount(int RoomID)
{
	int PlayerCount = 0;
	{
		set<BaseItemObject* >::iterator Iter;
		set<BaseItemObject* >& PlayerObj = *BaseItemObject::PlayerObjSet();
		for (Iter = PlayerObj.begin(); Iter != PlayerObj.end(); Iter++)
		{
			if ((*Iter)->Role()->RoomID() == RoomID
				&& (*Iter)->Role()->BaseData.Voc != EM_Vocation_GameMaster
				&& (*Iter)->Role()->TempData.Attr.Effect.IgnoreInstancePlayer == false)
				PlayerCount++;
		}
	}

	{
		map< string, PlayerRoleData* >::iterator Iter;
		for (Iter = _St->OnLoginPlayer.begin(); Iter != _St->OnLoginPlayer.end(); Iter++)
		{
			if (Iter->second->BaseData.RoomID == RoomID
				&& Iter->second->BaseData.Voc != EM_Vocation_GameMaster
				&& Iter->second->TempData.Attr.Effect.IgnoreInstancePlayer == false)
				PlayerCount++;
		}
	}

	return PlayerCount;
}
#if 0
void		Global::_ProxyPingLog(int ProxyID, int Time)
{
	char	Buf[512];
	sprintf_s(Buf, sizeof(Buf), "Insert ProxyPingLog(Ping,ProxyID) Values (%d,%d)", Time, ProxyID);
	Net_DCBase::LogSqlCommand(Buf);
}
#endif
void Global::GoodEvilAtkInfoProc(RoleDataEx* AtkRole, RoleDataEx* DefRole, int DHP)
{
	if (Global::Ini()->IsPvE == true || Global::Ini()->DisableGoodEvil == true)
		return;

	if (RoleDataEx::IsDisablePVPRule == true
		|| RoleDataEx::IsDisablePVPRule_Temp == true)
		return;

	if (AtkRole == NULL || DefRole == NULL)
		return;

	if (AtkRole->GUID() == DefRole->GUID())
		return;

	if (AtkRole->TempData.Attr.Effect.IgnoreDeadGoodEvil == true
		|| DefRole->TempData.Attr.Effect.IgnoreDeadGoodEvil == true)
		return;

	if (AtkRole->IsPlayer() == false)
	{
		AtkRole = AtkRole->GetOrgOwner();
	}
	if (DefRole->IsPlayer() == false)
	{
		DefRole = DefRole->GetOrgOwner();
	}

	if (AtkRole == NULL || DefRole == NULL)
		return;

	if (AtkRole->IsPlayer() == false || DefRole->IsPlayer() == false)
		return;

	if (AtkRole->TempData.Attr.Action.PK == true && DefRole->TempData.Attr.Action.PK == true)
	{
		if (AtkRole->GUID() == DefRole->PlayerTempData->PKInfo.TargetID)
			return;
	}

	if (DefRole->CheckGuildWarDeclare(AtkRole) == true)
		return;

	if (DHP < 0)
	{
		if (DefRole->TempData.Attr.Effect.Guilty == true)
		{
			BuffBaseStruct* Buff = DefRole->AssistMagic(DefRole, RoleDataEx::BaseMagicList[EM_BaseMagic_Guilty], 0, false, -1);
		}
		else if (DefRole->TempData.Attr.Effect.Hunter == true && AtkRole->TempData.AI.CampID == EM_CampID_Evil)
		{
			BuffBaseStruct* Buff = DefRole->AssistMagic(DefRole, RoleDataEx::BaseMagicList[EM_BaseMagic_PKHunter], 0, false, -1);
		}
		else
		{
			if (DefRole->TempData.AI.CampID == EM_CampID_Evil)
			{
				bool AttRolePkHunter = AtkRole->TempData.Attr.Effect.Hunter;
				BuffBaseStruct* Buff = AtkRole->AssistMagic(AtkRole, RoleDataEx::BaseMagicList[EM_BaseMagic_PKHunter], 0, false, -1);

				if (Buff != NULL)
				{
					if (AttRolePkHunter == true)
					{
					}
					else
						NetSrv_MagicChild::SendRange_AddBuffInfo(AtkRole->GUID(), AtkRole->GUID(), Buff->BuffID, 0, Buff->Time);
				}
			}
			else
			{
				bool AttRoleGuilty = AtkRole->TempData.Attr.Effect.Guilty;
				BuffBaseStruct* Buff = AtkRole->AssistMagic(AtkRole, RoleDataEx::BaseMagicList[EM_BaseMagic_Guilty], 0, false, -1);

				if (Buff != NULL)
				{
					if (AttRoleGuilty == true)
					{

					}
					else
						NetSrv_MagicChild::SendRange_AddBuffInfo(AtkRole->GUID(), AtkRole->GUID(), Buff->BuffID, 0, Buff->Time);
				}
			}
		}
	}
	else
	{

		if (DefRole->TempData.Attr.Effect.Guilty == true)
		{
			bool AttRoleGuilty = AtkRole->TempData.Attr.Effect.Guilty;
			BuffBaseStruct* Buff = AtkRole->AssistMagic(AtkRole, RoleDataEx::BaseMagicList[EM_BaseMagic_Guilty], 0, false, -1);
			if (AttRoleGuilty == false && Buff != NULL)
			{
				NetSrv_MagicChild::SendRange_AddBuffInfo(AtkRole->GUID(), AtkRole->GUID(), Buff->BuffID, 0, Buff->Time);
			}
		}
		else if (DefRole->TempData.Attr.Effect.Hunter == true
			&& AtkRole->TempData.AI.CampID != EM_CampID_Evil
			&& AtkRole->TempData.Attr.Effect.Guilty == false)
		{
			bool AttRoleGuilty = AtkRole->TempData.Attr.Effect.Hunter;
			BuffBaseStruct* Buff = AtkRole->AssistMagic(AtkRole, RoleDataEx::BaseMagicList[EM_BaseMagic_PKHunter], 0, false, -1);
			if (AttRoleGuilty == false && Buff != NULL)
			{
				NetSrv_MagicChild::SendRange_AddBuffInfo(AtkRole->GUID(), AtkRole->GUID(), Buff->BuffID, 0, Buff->Time);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//ГdЄ«©w®ЙіBІz
void Global::ProcPetEvent(RoleDataEx* Owner)
{
	char	Buf[512];
	RoleValueName_ENUM ValueType;
	for (int i = 0; i < MAX_CultivatePet_Count; i++)
	{
		CultivatePetStruct& PetBase = Owner->PlayerBaseData->Pet.Base[i];
		if (PetBase.IsEmpty() == true)
			continue;
		if (PetBase.EventType == EM_CultivatePetCommandType_None)
			continue;

		PetBase.EventColdown--;
		if (PetBase.EventColdown < 0)
		{
			switch (PetBase.EventType)
			{
			case EM_CultivatePetCommandType_OnTimeEvent:	//client єЭЁC10¬ніqЄѕServer ¬ЭЁє°¦ГdЄ«(іBІz»ЎёЬorЇS§OЁЖҐу)
				break;
			case EM_CultivatePetCommandType_Summon:			//ҐlікГdЄ«
				continue;
			case EM_CultivatePetCommandType_Feed:			//Бэ­№
				break;
			case EM_CultivatePetCommandType_Travel:			//®И¦ж
			{
				sprintf_s(Buf, sizeof(Buf), "PetCallBack_Travel_End(%d,%d,%d,%d)", i, PetBase.PetOrgID, PetBase.PetType, PetBase.Property);
				LUA_VMClass::PCallByStrArg(Buf, Owner->GUID(), Owner->GUID());
			}
			break;
			case EM_CultivatePetCommandType_Free:			//©сҐН
				break;
			case EM_CultivatePetCommandType_Train:			//ЇS°V
				break;
				/*
			case EM_CultivatePetCommandType_TrainCheck:		//Ез¦¬
				break;
			case EM_CultivatePetCommandType_ReTrain:		//ЅЖ°V(ЁПҐОЄ««~)
				break;
			case EM_CultivatePetCommandType_TrainAccept:	//Ез¦¬ЅT©w
				break;
				*/
			case EM_CultivatePetCommandType_Return:			//ҐlікҐXЄєГdЄ«¦^¦¬
				break;
				//////////////////////////////////////////////////////////////////////////

			case EM_CultivatePetCommandType_LiftSkill:
			{
				int ProductPos = 0;
				CultivatePetLifeSkillTableStruct* Table = GetVectorPosPoint(g_ObjectData->_CultivatePetLifeSkillTable, PetBase.LifeSkillTablePos);
				if (Table == NULL)
					break;

				if (PetBase.Source.Count < Table->Need.ItemCount)
					break;
				if (Table->Need.ToolID != 0 && PetBase.Tools.Count == 0)
					break;

				int TObjID = Table->Product.ItemID;
				int TCount = 1;

				Owner->GetTreasure(TObjID, TObjID, TCount);


				GameObjDbStructEx* ProductDB = Global::GetObjDB(TObjID);
				if (ProductDB != NULL)
				{
					for (ProductPos = 0; ProductPos < MAX_CultivatePet_Product_Count_; ProductPos++)
					{
						if (PetBase.Product[ProductPos].IsEmpty() == true || PetBase.Product[ProductPos].OrgObjID == TObjID)
							break;
					}

					if (ProductPos >= MAX_CultivatePet_Product_Count_)
						break;

					//if( PetBase.Product[ProductPos].Count == ProductDB->MaxHeap )
					//±NГdЄ«ҐНІЈ¤W­­±jЁоЁм999
					if (PetBase.Product[ProductPos].Count >= 999)
						break;

					PetBase.Product[ProductPos].OrgObjID = TObjID;
					PetBase.Product[ProductPos].Count += TCount;
					PetBase.Product[ProductPos].Mode.Trade = !(ProductDB->Mode.PickupBound);

				}

				PetBase.EventColdown = Table->EventTime;
				PetBase.Source.Count -= Table->Need.ItemCount;
				PetBase.Tools.Count--;

				if (PetBase.Source.Count == 0)
					PetBase.Source.Init();

				if (PetBase.Tools.Count == 0)
					PetBase.Tools.Init();

				//					ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * i + EM_CultivatePetStructValueType_EventColdown);
				//					Owner->Net_FixRoleValue( ValueType , 0 );
				//					Owner->Net_FixRoleValue( ValueType , PetBase.EventColdown );

				NetSrv_CultivatePet::SC_FixItem(Owner->GUID(), i, EM_CultivatePetEQType_Source, PetBase.Source);
				NetSrv_CultivatePet::SC_FixItem(Owner->GUID(), i, EM_CultivatePetEQType_Product1 + ProductPos, PetBase.Product[ProductPos]);
				NetSrv_CultivatePet::SC_FixItem(Owner->GUID(), i, EM_CultivatePetEQType_Tools, PetBase.Tools);

				//////////////////////////////////////////////////////////////////////////
				{
					int Pet_LifeSkill_MaxLv = g_ObjectData->GetSysKeyValue("Pet_LifeSkill_MaxLv");
					if (Pet_LifeSkill_MaxLv == 0)
						Pet_LifeSkill_MaxLv = 45;

					const float Rate[10] = { 1,0.9f,0.8f,0.7f,0.6f,0.5f,0.4f,0.3f,0.2f,0.1f };
					//јфЅm«Ч­pєв
					int EventSkillLv = Table->Need.SkillLv;
					float& PetSkill = PetBase.LifeSkill[Table->SkillType];
					float OrgPetSkill = PetSkill;
					unsigned DSkillLv = int(PetSkill) - EventSkillLv;

					if (OrgPetSkill < Pet_LifeSkill_MaxLv)
					{
						if (DSkillLv >= 10)
							DSkillLv = 9;

						float DSkillExp = 1.0f / RoleDataEx::PetLifeSkillTable[int(PetSkill)] * Rate[DSkillLv];
						PetSkill += DSkillExp;
						if (int(PetSkill * 100) != int(OrgPetSkill * 100))
						{
							ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * i + EM_CultivatePetStructValueType_LifeSkill1 + Table->SkillType);
							Owner->Net_FixRoleValue(ValueType, PetSkill);

						}
					}
				}

				ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * i + EM_CultivatePetStructValueType_EventColdown);
				Owner->Net_FixRoleValue(ValueType, 0);
				//////////////////////////////////////////////////////////////////////////
				//¬d¬Э¬O§_БЩҐiҐHД~Дт°µ¤UҐh
				if (PetBase.Source.Count < Table->Need.ItemCount)
					break;
				if (Table->Need.ToolID != 0 && PetBase.Tools.Count == 0)
					break;
				//////////////////////////////////////////////////////////////////////////
				Owner->Net_FixRoleValue(ValueType, PetBase.EventColdown);
				continue;
			}
			break;
			}

			PetBase.EventType = EM_CultivatePetCommandType_None;
			ValueType = (RoleValueName_ENUM)(EM_RoleValue_CultivatePet_Value + 100 * i + EM_CultivatePetStructValueType_EventType);
			Owner->Net_FixRoleValue(ValueType, PetBase.EventType);

			NetSrv_CultivatePet::SC_PetLifeResult(Owner->GUID(), EM_CultivatePetLifeSkillType_None, i, 0, true);
		}
	}

}
//-------------------------------------------------------------------------------------------

void	Global::ChangeParallel_CloseZone(int ParalledID)
{

	if (St()->WaitServerClose == true)
		return;

	St()->WaitServerClose = true;
	St()->WaitServerCloseCountDown = 30;
	St()->WaitServerCloseChangeParalledID = ParalledID;

	Schedular()->Push(_ChangeParallel_CloseZone_Proc, 0, NULL, NULL);
}

int		Global::_ChangeParallel_CloseZone_Proc(SchedularInfo* Obj, void* InputClass)
{
	char	szMsg[512];
	sprintf_s(szMsg, sizeof(szMsg), g_ObjectData->GetString("MSG_CHANGEPARALLED"), St()->WaitServerCloseCountDown);
	NetSrv_Talk::GMMsg(-1, szMsg);

	switch (St()->WaitServerCloseCountDown)
	{
	case 0:
	{
		//©Т¦іЁ¤¦вҐ[¤@­УµLјД30¬нЄєbuf

		GameObjDbStructEx* MagicBase = GetObjDB(RoleDataEx::BaseMagicList[EM_BaseMagic_PlayerUnbeatable]);

		//©Т¦іЁ¤¦вґ«°П
		set<BaseItemObject* >& PlayerObjSet = *(BaseItemObject::PlayerObjSet());
		set< BaseItemObject*>::iterator   PlayerObjIter;
		//­pєвЁC­У©Р¶ЎЄє¤HјЖ
		for (PlayerObjIter = PlayerObjSet.begin(); PlayerObjIter != PlayerObjSet.end(); PlayerObjIter++)
		{
			BaseItemObject* Obj = *PlayerObjIter;
			if (Obj == NULL)
				continue;

			RoleDataEx* Role = Obj->Role();
			Role->AssistMagic(Role, MagicBase->GUID, 0, false, 30);
			ChangeParalledID(Role->GUID(), St()->WaitServerCloseChangeParalledID);
		}

	}
	break;

	case -10:
	{
		Global::Destory();
		return 0;
	}
	break;
	}
	St()->WaitServerCloseCountDown -= 5;
	Schedular()->Push(_ChangeParallel_CloseZone_Proc, 5000, NULL, NULL);
	return 0;
}
//////////////////////////////////////////////////////////////////////////
//°eЅdіт«КҐ]
//////////////////////////////////////////////////////////////////////////
void Global::SendToCli_ByRoomID(int RoomID, int Size, void* Data)
{
	set<BaseItemObject* >& PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//­pєвЁC­У©Р¶ЎЄє¤HјЖ
	for (PlayerObjIter = PlayerObjSet.begin(); PlayerObjIter != PlayerObjSet.end(); PlayerObjIter++)
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if (Obj == NULL || Obj->Role()->RoomID() != RoomID)
			continue;

		RoleDataEx* Role = Obj->Role();

		SendToCli_ByProxyID(Role->TempData.Sys.SockID, Role->TempData.Sys.ProxyID, Size, Data);
	}

}
void Global::SendToCli_Range(RoleDataEx* Owner, int BlockSize, int Size, void* Data)
{
	// Ґ­¤Аµ№ЄюЄс©Т¦і¤H
	PlayIDInfo** Block = Global::PartSearch(Owner, BlockSize);

	int				i, j;
	PlayID* TopID;

	for (i = 0; Block[i] != NULL; i++)
	{
		TopID = Block[i]->Begin;
		for (j = 0; TopID != NULL; TopID = TopID->Next)
		{
			BaseItemObject* Obj = Global::GetObj(TopID->ID);
			if (Obj == NULL)
				continue;

			RoleDataEx* Other = Obj->Role();

			if (Other->IsPlayer() == false)
				continue;
			SendToCli_ByProxyID(Other->TempData.Sys.SockID, Other->TempData.Sys.ProxyID, Size, Data);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
bool Global::CheckLuaScript(int OwnerID, int TargetID, const char* LuaScript)
{
	//§QҐОј@±ЎАЛ¬d
	vector<MyVMValueStruct> RetList;
	if (strlen(LuaScript) != 0)
	{
		bool Ret = true;
		if (LUA_VMClass::PCallByStrArg(LuaScript, OwnerID, TargetID, &RetList, 1))
		{
			if (RetList.size() != 1)
			{
				Ret = false;
			}
			else
			{
				MyVMValueStruct& Temp = RetList[0];
				if (Temp.Flag != true)
					Ret = false;
			}
		}
		else
			Ret = false;

		return Ret;
	}

	return true;
}

//ЕЄЁъLuaЄє°}¦C
void	Global::ReadLuaArray(int OwnerID, const char* LuaScript, int Size, int RetArray[])
{
	char	Buf[512];
	//§QҐОј@±ЎАЛ¬d
	vector<MyVMValueStruct> RetList;

	for (int i = 0; i < Size; i++)
	{
		sprintf(Buf, "%s(%d)", LuaScript, i);


		if (LUA_VMClass::PCallByStrArg(Buf, OwnerID, OwnerID, &RetList, 1))
		{
			if (RetList.size() != 1)
				return;
			MyVMValueStruct& Temp = RetList[0];
			RetArray[i] = Temp.Number_Int;
		}

	}

}

//Pcall CheckFunction
bool Global::LuaCheckFunction(int OwnerID, int TargetID, const char* LuaScript)
{
	if (LuaScript == NULL || LuaScript[0] == 0)
		return true;

	vector<MyVMValueStruct> RetList;
	if (LUA_VMClass::PCallByStrArg(LuaScript, OwnerID, TargetID, &RetList, 1))
	{
		if (RetList.size() == 1)
		{
			MyVMValueStruct& Temp = RetList[0];
			return Temp.Flag;
		}
	}
	return false;
}
/*
//Ёъ±oЄЕЄє©Р¶Ў
PrivateRoomInfoStruct*	Global::GetEmptyRoomID		( )
{
	bool IsFind = false;
	vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;
	for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
	{
		if( RoomList[i].IsActive == false )
		{
			return &RoomList[i];
		}
	}
	return NULL;
}
*/

//////////////////////////////////////////////////////////////////////////
//јWҐ[Єчїъ(©Т¦іГю«¬)
bool	Global::AddMoneyBase(RoleDataEx* Owner, ActionTypeENUM ActionType, PriceTypeENUM Type, int Money)
{
	//	if( CheckMoneyBase(Type , Money ) == false )
	//		return false;

	switch (Type)
	{
	case EM_ACPriceType_GameMoney:
	{
		return Owner->AddBodyMoney(Money, NULL, ActionType, true);
	}
	break;
	case EM_ACPriceType_AccountMoney:
	{
		if (AccountMoneyOperable() == false)
		{
			return false;
		}
		else
		{
			return Owner->AddBodyMoney_Account(Money, NULL, ActionType, true, true);
		}
	}
	break;
	case EM_ACPriceType_BonusMoney:
	{
		return Owner->AddMoney_Bonus(Money, NULL, ActionType);
	}
	break;
	case  EM_ACPriceType_Honor:
	{
		int orgHonor = Owner->PlayerBaseData->Honor;
		int Honor = int(Owner->PlayerBaseData->Honor + Money);
		Owner->SetValue(EM_RoleValue_Honor, Honor);
		Log_MoneyEx(-1, -1, Owner, ActionType, Type, Owner->PlayerBaseData->Honor, orgHonor);
	}
	break;
	case EM_ACPriceType_GuildWarEnergy:
	{
		GuildHouseWarManageClass* warClass = GuildHouseWarManageClass::GetGuildWar_ByRoomID(Owner->RoomID());
		if (warClass == NULL)
			return false;

		return warClass->WarBase().AddEnergy(Owner->GuildID(), Money);
	}
	break;
	case EM_ACPriceType_DuelistReward:
	{
		int orgValue = Owner->PlayerBaseData->DuelistReward;

		int DuelistReward = int(Owner->PlayerBaseData->DuelistReward + Money);
		Owner->SetValue(EM_RoleValue_DuelistReward, DuelistReward);
		Log_MoneyEx(-1, -1, Owner, ActionType, Type, Owner->PlayerBaseData->DuelistReward, orgValue);
	}
	break;
	case EM_ACPriceType_TrialBadge:		//ёХ·ТАІі№
	{
		int orgValue = Owner->PlayerBaseData->TrialBadgeCoin;
		int TrialBadgeCoin = int(Owner->PlayerBaseData->TrialBadgeCoin + Money);
		Owner->SetValue(EM_RoleValue_TrialBadgeCoin, TrialBadgeCoin);
		Log_MoneyEx(-1, -1, Owner, ActionType, Type, Owner->PlayerBaseData->TrialBadgeCoin, orgValue);
	}
	break;
	case EM_ACPriceType_Relics:			//ҐjҐNїтЄ«
	{
		int orgValue = Owner->PlayerBaseData->RelicsCoin;
		int RelicsCoin = int(Owner->PlayerBaseData->RelicsCoin + Money);
		Owner->SetValue(EM_RoleValue_RelicsCoin, RelicsCoin);
		Log_MoneyEx(-1, -1, Owner, ActionType, Type, Owner->PlayerBaseData->RelicsCoin, orgValue);
	}
	break;
	case EM_ACPriceType_Dreamland:		//№Ъ№ТҐэ»є¦L°O
	{
		int orgValue = Owner->PlayerBaseData->DreamlandCoin;
		int DreamlandCoin = int(Owner->PlayerBaseData->DreamlandCoin + Money);
		Owner->SetValue(EM_RoleValue_DreamlandCoin, DreamlandCoin);
		Log_MoneyEx(-1, -1, Owner, ActionType, Type, Owner->PlayerBaseData->DreamlandCoin, orgValue);
	}
	break;
	case EM_ACPriceType_PhiriusShell:	//ҐІєёҐЕ¶кЁ©
	{
		int orgValue = Owner->PlayerBaseData->PhiriusShellCoin;
		int PhiriusShellCoin = int(Owner->PlayerBaseData->PhiriusShellCoin + Money);
		Owner->SetValue(EM_RoleValue_PhiriusShellCoin, PhiriusShellCoin);
		Log_MoneyEx(-1, -1, Owner, ActionType, Type, Owner->PlayerBaseData->PhiriusShellCoin, orgValue);
	}
	break;
	case EM_ACPriceType_EnergyJustice:	//ҐїёqЇа¶q
	{
		int orgValue = Owner->PlayerBaseData->EnergyJusticeCoin;
		int EnergyJusticeCoin = int(Owner->PlayerBaseData->EnergyJusticeCoin + Money);
		Owner->SetValue(EM_RoleValue_EnergyJusticeCoin, EnergyJusticeCoin);
		Log_MoneyEx(-1, -1, Owner, ActionType, Type, Owner->PlayerBaseData->EnergyJusticeCoin, orgValue);
	}
	break;
	case EM_ACPriceType_ProofLegend:	//¶З»Ў¤§ГТ
	{
		int orgValue = Owner->PlayerBaseData->ProofLegendCoin;
		int ProofLegendCoin = int(Owner->PlayerBaseData->ProofLegendCoin + Money);
		Owner->SetValue(EM_RoleValue_ProofLegendCoin, ProofLegendCoin);
		Log_MoneyEx(-1, -1, Owner, ActionType, Type, Owner->PlayerBaseData->ProofLegendCoin, orgValue);
	}
	break;
	case EM_ACPriceType_MirrorCoin:
	{
		int orgValue = Owner->PlayerBaseData->MirrorCoin;

		int MirrorCoin = int(Owner->PlayerBaseData->MirrorCoin + Money);
		Owner->SetValue(EM_RoleValue_MirrorCoin, MirrorCoin);
		Log_MoneyEx(-1, -1, Owner, ActionType, Type, Owner->PlayerBaseData->DuelistReward, orgValue);
	}
	break;
	default:
	{
		char Buf[256];
		sprintf(Buf, "SYS_PRICETYPE_ICONID_%02d", Type);
		//ЁъҐXЄчїъГю«¬
		int SPMoneyObjID = g_ObjectData->GetSysKeyValue(Buf);
		if (Money < 0)
			return Owner->DelBodyItem(SPMoneyObjID, Money * -1, ActionType);
		else
			return Owner->GiveItem(SPMoneyObjID, Money, ActionType, NULL, "");
	}
	break;
	}

	return false;
}
//°OѕРЕйАЛ¬d
void Global::RecycleBinMemCheck()
{

	if (BaseItemObject::St()->PlayerRoleDataRecycle.CheckAllMem() == false)
		Print(LV5, "RecycleBinMemCheck", "PlayerRoleDataRecycle.CheckAllMem() == false");
	else
		Print(LV3, "RecycleBinMemCheck", "PlayerRoleDataRecycle.CheckAllMem() == true");

	if (BaseItemObject::St()->NPCRoleDataRecycle.CheckAllMem() == false)
		Print(LV5, "RecycleBinMemCheck", "NPCRoleDataRecycle.CheckAllMem() == false");
	else
		Print(LV3, "RecycleBinMemCheck", "NPCRoleDataRecycle.CheckAllMem() == true");

	if (LUA_VMClass::CheckMemory() == false)
		Print(LV5, "RecycleBinMemCheck", "LUA_VMClass::CheckMemory == false");
	else
		Print(LV3, "RecycleBinMemCheck", "LUA_VMClass::CheckMemory == true");


}

//АЛ¬dЄчїъ(©Т¦іГю«¬)
bool	Global::CheckMoneyBase(RoleDataEx* Owner, PriceTypeENUM Type, int Money)
{
	if (Money < 0)
	{
		//Print( LV5 , "NetSrv_ShopChild::R_BuyItemRequest" , "Buy ItemObjID = %d Money=%d" , OrgObjID , Money );
		return false;
	}

	switch (Type)
	{
	case EM_ACPriceType_GameMoney:
	{
		//­pєв»щїъ
		if (Owner->PlayerBaseData->Body.Money < Money)
			return false;
	}
	break;
	case EM_ACPriceType_AccountMoney:
	{
		if (Owner->PlayerBaseData->Body.Money_Account < Money)
			return false;
	}
	break;
	case EM_ACPriceType_BonusMoney:
	{
		if (Owner->PlayerBaseData->Money_Bonus < Money)
			return false;
	}
	break;
	case  EM_ACPriceType_Honor:
	{
		if (Owner->PlayerBaseData->Honor < Money)
			return false;
	}
	break;
	case EM_ACPriceType_GuildWarEnergy:
	{
		GuildHouseWarManageClass* warClass = GuildHouseWarManageClass::GetGuildWar_ByRoomID(Owner->RoomID());
		if (warClass == NULL)
			return false;

		if (warClass->WarBase().GetEnergy(Owner->GuildID()) < Money)
			return false;
		//return warClass->WarBase().AddEnergy( Role->GuildID() , Value );
	}
	break;
	case EM_ACPriceType_DuelistReward:
	{
		if (Owner->PlayerBaseData->DuelistReward < Money)
			return false;
	}
	break;
	case EM_ACPriceType_TrialBadge:		//ёХ·ТАІі№
	{
		if (Owner->PlayerBaseData->TrialBadgeCoin < Money)
			return false;
	}
	break;
	case EM_ACPriceType_Relics:			//ҐjҐNїтЄ«
	{
		if (Owner->PlayerBaseData->RelicsCoin < Money)
			return false;
	}
	break;
	case EM_ACPriceType_Dreamland:		//№Ъ№ТҐэ»є¦L°O
	{
		if (Owner->PlayerBaseData->DreamlandCoin < Money)
			return false;
	}
	break;
	case EM_ACPriceType_PhiriusShell:	//ҐІєёҐЕ¶кЁ©
	{
		if (Owner->PlayerBaseData->PhiriusShellCoin < Money)
			return false;
	}
	break;
	case EM_ACPriceType_EnergyJustice:	//ҐїёqЇа¶q
	{
		if (Owner->PlayerBaseData->EnergyJusticeCoin < Money)
			return false;
	}
	break;
	case EM_ACPriceType_ProofLegend:	//¶З»Ў¤§ГТ
	{
		if (Owner->PlayerBaseData->ProofLegendCoin < Money)
			return false;
	}
	break;
	case EM_ACPriceType_MirrorCoin:
	{
		if (Owner->PlayerBaseData->MirrorCoin < Money)
			return false;
	}
	break;
	default:
	{
		char Buf[256];
		sprintf(Buf, "SYS_PRICETYPE_ICONID_%02d", Type);
		//ЁъҐXЄчїъГю«¬
		int SPMoneyObjID = g_ObjectData->GetSysKeyValue(Buf);

		if (Owner->CalBodyItemCount(SPMoneyObjID) < Money)
			return false;
	}
	break;
	}

	return true;

}
//////////////////////////////////////////////////////////////////////////
/*
void	Global::BillboardRegister( int PlayerDBID , int SortType , int SortValue )
{
	char Buf[2048];
	int RandValue = rand();
	sprintf( Buf , "DELETE BillBoardInfo where Type=%d and PlayerDBID = %d" , SortType , PlayerDBID );
	Net_DCBase::SqlCommand( RandValue , Buf );

	sprintf( Buf , "INSERT BillBoardInfo(Type,PlayerDBID,SortValue) VALUES(%d,%d,%d)" , SortType , PlayerDBID , SortValue );
	Net_DCBase::SqlCommand( RandValue , Buf );
}
*/
//////////////////////////////////////////////////////////////////////////
#if 0
//////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
//©Р«О¬ЫГцlog
//----------------------------------------------------------------------------------------
//void	Global::Log_House( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType)
//{
//	char Buf[ 1024 ];
//	if( ActionType == EM_HouseActionType_IntoHouse || ActionType == EM_HouseActionType_LeaveHouse )
//	{//¶i¤J¤p«О Вч¶}¤p«О
//		sprintf_s( Buf , sizeof(Buf) , "Insert HouseTyLog(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType) Values( %d,%d,%d,%d)"
//			, Player->DBID()
//			, HouseOwnerDBID
//			, HouseDBID
//			, ActionType
//			);	
//		Net_DCBase::LogSqlCommand( Buf );
//	}
//}
void	Global::Log_House(RoleDataEx* Player, int HouseDBID, int HouseOwnerDBID, int ActionType, const char* Msg)
{
	CharTransferClass	charTransfer;
	char Buf[1024];
	charTransfer.SetUtf8String(Msg);
	string MsgCmdBinStr = StringToSqlX((char*)charTransfer.GetWCString(), charTransfer.WCStrLen() * 2, false);


	sprintf_s(Buf, sizeof(Buf), "Insert HouseLog(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,ContentTxt) Values( %d,%d,%d,%d,CAST( %s AS nvarchar(512) ))"
		, Player->DBID()
		, HouseOwnerDBID
		, HouseDBID
		, ActionType
		, MsgCmdBinStr.c_str()
	);
	Net_DCBase::LogSqlCommand(Buf);

	//if( ActionType == EM_HouseActionType_ChangeHouseName)
	//{//­Ч§п¤p«О¦WєЩ
	//	charTransfer.SetUtf8String( Msg );
	//	string MsgCmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );


	//	sprintf_s( Buf , sizeof(Buf) , "Insert HouseType2Log(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,HouseName) Values( %d,%d,%d,%d,CAST( %s AS nvarchar(50) ))"
	//		, Player->DBID()
	//		, HouseOwnerDBID
	//		, HouseDBID
	//		, ActionType
	//		,MsgCmdBinStr.c_str()
	//		);	
	//	Net_DCBase::LogSqlCommand( Buf );
	//}
	//else if( ActionType == EM_HouseActionType_ChangePassWord)
	//{//­Ч§п¤p«О±KЅX
	//	charTransfer.SetUtf8String( Msg );
	//	string MsgCmdBinStr = StringToSqlX( (char*)charTransfer.GetString() , charTransfer.CStrLen() , false );


	//	sprintf_s( Buf , sizeof(Buf) , "Insert HouseType3Log(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,Password) Values( %d,%d,%d,%d,CAST( %s AS varchar(31) ))"
	//		, Player->DBID()
	//		, HouseOwnerDBID
	//		, HouseDBID
	//		, ActionType
	//		,MsgCmdBinStr.c_str()
	//		);	
	//	Net_DCBase::LogSqlCommand( Buf );
	//}
}
//void		Global::Log_House( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number1 , int Number2 , int Number3 , int Number4 )
//{
//	char Buf[ 1024 ];
//	if( ActionType == EM_HouseActionType_ChangeVoc )
//	{//­Ч§пВѕ·~
//		sprintf_s( Buf , sizeof(Buf) , "Insert HouseType4Log(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,BeforeVoc,BeforeSubVoc,AfterVoc,AfterSubVoc) Values( %d,%d,%d,%d,%d,%d,%d,%d)"
//			, Player->DBID()
//			, HouseOwnerDBID
//			, HouseDBID
//			, ActionType
//			, Number1
//			, Number2
//			, Number3
//			, Number4
//			);	
//		Net_DCBase::LogSqlCommand( Buf );
//	}
//}
//void		Global::Log_House( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number1 , int Number2 )
//{
//	char Buf[ 1024 ];
//	if( ActionType == EM_HouseActionType_EnergyIntro	||
//		ActionType == EM_HouseActionType_SpaceIntro		||
//		ActionType == EM_HouseActionType_Servant_Employ
//		)
//	{//Їа¶qёЙҐR //ЄЕ¶ЎВXҐR //¤k№І_¶±ҐО
//		sprintf_s( Buf , sizeof(Buf) , "Insert HouseType5Log(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,Number1,Number2) Values( %d,%d,%d,%d,%d,%d)"
//			, Player->DBID()
//			, HouseOwnerDBID
//			, HouseDBID
//			, ActionType
//			, Number1
//			, Number2
//			);	
//		Net_DCBase::LogSqlCommand( Buf );
//	}
//}
//void		Global::Log_House( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number1 , int Number2 , float Float1 , float Float2 , float Float3 , float Float4 )
//{
//	char Buf[ 1024 ];
//	if( ActionType == EM_HouseActionType_Furniture_Appear || ActionType == EM_HouseActionType_Furniture_Disappear )
//	{//іГ­СВ\і]ЕгҐЬ іГ­СВ\і]БфВГ
//		sprintf_s( Buf , sizeof(Buf) , "Insert HouseType6Log(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,ItemDBID,OrgObjID,X,Y,Z,Dir) Values( %d,%d,%d,%d,%d,%d,%f,%f,%f,%f)"
//			, Player->DBID()
//			, HouseOwnerDBID
//			, HouseDBID
//			, ActionType
//			, Number1
//			, Number2
//			, Float1
//			, Float2
//			, Float3
//			, Float4
//			);	
//		Net_DCBase::LogSqlCommand( Buf );
//	}
//}
//void		Global::Log_House( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number )
//{
//	char Buf[ 1024 ];
//	if( ActionType == EM_HouseActionType_ChangeHouseType	||
//		ActionType == EM_HouseActionType_AddFriend		||
//		ActionType == EM_HouseActionType_DeleteFriend		||
//		ActionType == EM_HouseActionType_Servant_Fire ||
//		( ActionType >= EM_HouseActionType_Servant_Talk && ActionType <= EM_HouseActionType_Servant_CraftCourse )
//		)
//	{//®ж§ЅЕЬ§у //јWҐ[©Р«О¦n¤Н //§R°Ј©Р«О¦n¤Н//¤k№І_¶}°Ј//¤k№І_ҐжЅН//¤k№І_Е]ЄkЅТµ{//¤k№І_§рА»ЅТµ{//¤k№І_¦uЕ@ЅТµ{//¤k№І_Іi¶№ЅТµ{//¤k№І_¤uГАЅТµ{
//		sprintf_s( Buf , sizeof(Buf) , "Insert HouseType7Log(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,Number) Values( %d,%d,%d,%d,%d)"
//			, Player->DBID()
//			, HouseOwnerDBID
//			, HouseDBID
//			, ActionType
//			, Number
//			);	
//		Net_DCBase::LogSqlCommand( Buf );
//	}
//}
//void		Global::Log_House( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , const char* Msg1 , const char* Msg2 , const char* Msg3 )
//{
//	CharTransferClass	charTransfer;
//	char Buf[ 1024 ];
//	if( ActionType == EM_HouseActionType_HouseGreet)
//	{//¦n¤Н°Э­Ф»y
//		charTransfer.SetUtf8String( Msg1 );
//		string strmsg1 = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
//		charTransfer.SetUtf8String( Msg2 );
//		string strmsg2 = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
//		charTransfer.SetUtf8String( Msg3 );
//		string strmsg3 = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
//
//
//		sprintf_s( Buf , sizeof(Buf) , "Insert HouseType8Log(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,Enter,Leave,UseItem) Values( %d,%d,%d,%d,CAST( %s AS nvarchar(256)),CAST( %s AS nvarchar(256)),CAST( %s AS nvarchar(256)))"
//			, Player->DBID()
//			, HouseOwnerDBID
//			, HouseDBID
//			, ActionType
//			,strmsg1.c_str()
//			,strmsg2.c_str()
//			,strmsg3.c_str()
//			);	
//		Net_DCBase::LogSqlCommand( Buf );
//	}
//}
//void		Global::Log_House( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number1 , int Number2 , int Number3 , int Number4 , int Number5 , int Number6 )
//{
//	char Buf[ 1024 ];
//	if( ActionType >= EM_HouseActionType_Item_BagToFurniture && ActionType <= EM_HouseActionType_Item_BankToEq )
//	{//Іѕ°КЄ««~
//		sprintf_s( Buf , sizeof(Buf) , "Insert HouseType9Log(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,ParentItem1DBID,ParentItem1Pos,ParentItem2DBID,ParentItem2Pos,OrgObjID,OrgObjCount) Values( %d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
//			, Player->DBID()
//			, HouseOwnerDBID
//			, HouseDBID
//			, ActionType
//			, Number1
//			, Number2
//			, Number3
//			, Number4
//			, Number5
//			, Number6
//			);	
//		Net_DCBase::LogSqlCommand( Buf );
//	}
//}
#endif
void Global::FixAllAttackMode()
{
	set<RoleDataEx*>	AttackOffSet;
	//ІM°Ј©Т¦іЄ«ҐуЄєBuf
	vector< BaseItemObject* >& zoneList = *BaseItemObject::GetZoneObjList();

	for (int i = 0; i < (int)zoneList.size(); i++)
	{
		BaseItemObject* obj = zoneList[i];
		if (obj == NULL)
			continue;

		RoleDataEx* role = obj->Role();
		if (role->IsAttack() && role->IsNPC())
			continue;
		if (role->TempData.Attr.Action.AttackMode)
		{
			role->TempData.Attr.Action.AttackMode = false;
			AttackOffSet.insert(role);
		}
	}

	for (int i = 0; i < (int)zoneList.size(); i++)
	{
		BaseItemObject* obj = zoneList[i];
		if (obj == NULL)
			continue;

		RoleDataEx* role = obj->Role();

		for (unsigned j = 0; j < (unsigned)role->TempData.NPCHate.Hate.Size(); j++)
		{
			NPCHateStruct& hateBase = role->TempData.NPCHate.Hate[j];
			RoleDataEx* other = Global::GetRoleDataByGUID(hateBase.ItemID);
			if (other == NULL
				|| (other->IsPlayer() && other->DBID() != hateBase.DBID))
			{
				role->TempData.NPCHate.Hate.Erase(j);
				j--;
				continue;
			}
			if (other->Length(role) > 1000)
				continue;
			other->TempData.Attr.Action.AttackMode = true;

			AttackOffSet.erase(other);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		set<RoleDataEx*>::iterator iter;
		for (iter = AttackOffSet.begin(); iter != AttackOffSet.end(); iter++)
		{
			(*iter)->TempData.BackInfo.BuffClearTime.OnStopAttackMode = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
}

int	Global::GetPCenterID(const char* account)
{
	int hashID = GetHashCode(account, (int)strlen(account));
	return hashID % Ini()->PlayerCenterCount;
}

int Global::PrintLuaMemoryUsage(SchedularInfo* Obj, void* InputClass)
{
	int iLuaMem = LUA_VMClass::GetLuaMemoryUsage();
	Print(LV2, "SYSTEM", "Lua Memory[ %d KB][ %d MB]", iLuaMem, iLuaMem / 1024);

	Schedular()->Push(PrintLuaMemoryUsage, _St->Ini.IntDef("PrintLuaMemoryUsageTimeGap", 600) * 1000, NULL, NULL);

	return 0;
}

bool Global::AccountMoneyOperable()
{
	if (_St->IsUseBillingServer == true)
	{
		return Net_Billing::BillingSystemAvaliable();
	}

	return true;
}

bool	Global::AutoUseItem(RoleDataEx* Owner, GameObjDbStructEx* ItemDB)
{
	if (Owner->TempData.Sys.OnChangeZone == true
		|| Owner->TempData.IsDisabledChangeZone == true)
		return false;

	if (!ItemDB->IsItem())
		return false;

	if (ItemDB->Item.UseType != EM_ItemUseType_SrvScript)
		return false;

	LUA_VMClass::PCallByStrArg(ItemDB->Item.SrvScript, Owner->GUID(), Owner->GUID());

	return true;
}
//Ё¤¦вµn¤Jёк®ЖЄм©l¤Ж
void Global::LoginInitProc(RoleDataEx* Owner)
{
	Owner->BaseData.SysFlag.EnableGMCmd = Global::Ini()->DisableGMPassword;

	//§в©Т¦іГdЄ«Ґlґ«ёЛ¤УІM°Ј
	for (int i = 0; i < MAX_CultivatePet_Count; i++)
	{
		CultivatePetStruct& PetBase = Owner->PlayerBaseData->Pet.Base[i];
		if (PetBase.EventType == EM_CultivatePetCommandType_Summon)
		{
			PetBase.EventType = EM_CultivatePetCommandType_None;
		}
	}

	if (Owner->PlayerBaseData->DeadCountDown != 0)
		Owner->IsDead(true);

	//і]©wГdЄ«Ґd¤щёк°T
	for (int i = 0; i < Owner->PlayerBaseData->PetExBag.Size(); i++)
	{
		int cardID = g_ObjectData->GetPetCardID(Owner->PlayerBaseData->PetExBag[i]);
		if (cardID != -1)
			Owner->PlayerBaseData->PetCard.SetFlagOn(cardID);
	}
}
void Global::ChangeZone(int GItemID, int ZoneID, int RoomID, float X, float Y, float Z, float Dir)
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObj(GItemID);
	if (OwnerObj == NULL)
		return;

	RoleDataEx* Role = OwnerObj->Role();

	if (Role->TempData.Sys.OnChangeZone == true)
	{
		if (RoleDataEx::G_SysTime - Role->TempData.Sys.ChangeZoneTime < 60000)
		{
			Print(LV6, "ChangeZone", "OnChangeZone = true DBID=%d", Role->DBID());
			return;
		}
	}

	if (Role->TempData.IsDisabledChangeZone == true)
	{
		Print(LV6, "ChangeZone", "IsDisabledChangeZone = true DBID=%d", Role->DBID());
		return;
	}

	if (Role->TempData.Sys.OnChangeWorld == true)
	{
		Print(LV6, "ChangeZone", "OnChangeWorld = true DBID=%d", Role->DBID());
		return;
	}

	TempChangeZoneStruct* TempData = NEW(TempChangeZoneStruct);
	TempData->GItemID = GItemID;
	TempData->ZoneID = ZoneID;
	TempData->RoomID = RoomID;
	TempData->X = X;
	TempData->Y = Y;
	TempData->Z = Z;
	TempData->Dir = Dir;

	Role->TempData.Sys.OnChangeZone = true;
	Role->TempData.Sys.ChangeZoneTime = RoleDataEx::G_SysTime;

	Print(LV6, "ChangeZone", "DBID=%d Role=%s From(%d,%d,%d,%d) To(%d,%d,%d,%d)"
		, Role->DBID(), Global::GetRoleName_ASCII(Role).c_str()
		, RoleDataEx::G_ZoneID, int(Role->X()), int(Role->Y()), int(Role->Z())
		, ZoneID, int(X), int(Y), int(Z));

	Schedular()->Push(_ChangeZoneDelay, 200, TempData
		, NULL);
}
int Global::_ChangeZoneDelay(SchedularInfo* Obj, void* InputClass)
{
	TempChangeZoneStruct* TempData = (TempChangeZoneStruct*)InputClass;

	RoleDataEx* Role = Global::GetRoleDataByGUID(TempData->GItemID);
	if (Role == NULL)
	{
		Print(LV6, "_ChangeZoneDelay", "Role == NULL");
		return 0;
	}

	if (Role->PlayerTempData->ChangeZoneCountDown > 0)
	{
		Schedular()->Push(_ChangeZoneDelay, 200, TempData, NULL);
		return 0;
	}

	ChangeZoneDirect(TempData->GItemID, TempData->ZoneID, TempData->RoomID, TempData->X, TempData->Y, TempData->Z, TempData->Dir);

	delete TempData;
	return 0;
}
void Global::ChangeZoneDirect(int GItemID, int ZoneID, int RoomID, float X, float Y, float Z, float Dir)
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObj(GItemID);
	if (OwnerObj == NULL)
		return;

	RoleDataEx* Role = OwnerObj->Role();

	if (CheckZoneID(ZoneID) == false)
	{
		OwnerObj->Role()->TempData.Sys.OnChangeZone = false;
		Role->Net_GameMsgEvent(EM_GameMessageEvent_ChanageZoneError_Failed);
		Print(LV6, "ChangeZoneDirect", "CheckZoneID( %d) == false  Role DBID=%d", ZoneID, Role->DBID());
		return;
	}

	// 檢查是否有施法條, 發送中斷施法條給玩家
	{
		if (Role->PlayerTempData->CastData.pCastFunc != NULL)
		{
			((GatherCallBackFunc)Role->PlayerTempData->CastData.pCastFunc)(GItemID, EM_GATHER_FAILED);
			Role->PlayerTempData->CastData.pCastFunc = NULL;
			Global::Schedular()->Erase(Role->PlayerTempData->CastData.iEventHandle);
			Role->PlayerTempData->CastData.iEventHandle = -1;
		}
	}

	//if( Role->ZoneID() == ZoneID )
	if (RoleDataEx::G_ZoneID == ZoneID)
	{
		OwnerObj->Role()->TempData.Sys.OnChangeZone = false;


		if (!DelFromPartition(Role->GUID()))
		{
			//return;
		}
		else if (Role->IsPlayer() && Role->Length(X, Y, Z) >= 100 || (RoomID != -1 && Role->RoomID() != RoomID))
		{
			//把所有寵物都移到自己的位置
			vector< BaseItemObject* >& ObjList = *BaseItemObject::GetZoneObjList();
			for (int i = 0; i < (int)ObjList.size(); i++)
			{
				BaseItemObject* PetObj = ObjList[i];
				if (PetObj == NULL)
					continue;

				if (PetObj->Role()->BaseData.Mode.Move == false)
					continue;

				if (PetObj->Role()->TempData.Sys.OwnerDBID == Role->DBID() && Role->DBID() != -1)
				{
					ChangeZoneDirect(PetObj->GUID(), ZoneID, RoomID, X + rand() % 50 - 25, Y, Z + rand() % 50 - 25, Dir);
				}
			}
		}

		if (RoomID != -1)
		{
			if (RoomID >= Ini()->RoomCount)
				RoomID = 0;

			Role->RoomID(RoomID);
		}

		Role->Pos(X, Y, Z, Dir);
		Role->TempData.Move.LOK_CliX = Role->TempData.Move.CliX = Role->TempData.Move.LCheckLineX = X;
		Role->TempData.Move.LOK_CliY = Role->TempData.Move.CliY = Role->TempData.Move.LCheckLineY = Y;
		Role->TempData.Move.LOK_CliZ = Role->TempData.Move.CliZ = Role->TempData.Move.LCheckLineZ = Z;

		CheckClientLoading_AddToPartition(Role->GUID(), Role->RoomID());
	}
	else
	{

		PlayerRoleData BaseTempPlayer;
		BaseTempPlayer.Copy(Role);

		RoleDataEx* TempPlayer = (RoleDataEx*)&BaseTempPlayer;

		TempPlayer->Pos(X, Y, Z, Dir);
		TempPlayer->RoomID(RoomID);
		TempPlayer->ZoneID(ZoneID);

		if (ZoneID % _DEF_ZONE_BASE_COUNT_ != RoleDataEx::G_ZoneID % _DEF_ZONE_BASE_COUNT_)
			TempPlayer->TempData.Attr.Action.ChangeZone = true;

		NetSrv_CliConnectChild::ChangeZone(TempPlayer);
	}
}
void Global::ChangeWorld(int GItemID, RoleDataEx* pRole, int iReturnZoneID, RolePosStruct Pos, int WorldID, int ZoneID, int RoomID, float X, float Y, float Z, float Dir)
{
	if (pRole->TempData.Sys.OnChangeZone == true)
	{
		Print(LV6, "ChangeWorld", "OnChangeWorld = true DBID=%d", pRole->DBID());
		return;
	}

	if (pRole->TempData.IsDisabledChangeZone == true)
	{
		Print(LV6, "ChangeWorld", "IsDisabledChangeZone = true DBID=%d", pRole->DBID());
		return;
	}

	TempChangeWorldStruct* TempData = NEW(TempChangeWorldStruct);
	TempData->WorldID = WorldID;
	TempData->GItemID = GItemID;
	TempData->ZoneID = ZoneID;
	TempData->RoomID = RoomID;
	TempData->X = X;
	TempData->Y = Y;
	TempData->Z = Z;
	TempData->Dir = Dir;

	pRole->PlayerTempData->ChangeWorld.RetPos = Pos;
	pRole->PlayerTempData->ChangeWorld.RetZoneID = iReturnZoneID;

	pRole->TempData.Sys.OnChangeWorld = true;

	Print(LV6, "ChangeWorld", "DBID=%d Role=%s From(%d,%d,%d,%d) To(%d,%d,%d,%d)"
		, pRole->DBID(), Global::GetRoleName_ASCII(pRole).c_str()
		, RoleDataEx::G_ZoneID, int(pRole->X()), int(pRole->Y()), int(pRole->Z())
		, ZoneID, int(X), int(Y), int(Z));

	Schedular()->Push(_ChangeWorldDelay, 200, TempData, NULL);
}
int Global::_ChangeWorldDelay(SchedularInfo* Obj, void* InputClass)
{
	TempChangeWorldStruct* TempData = (TempChangeWorldStruct*)InputClass;

	RoleDataEx* Role = Global::GetRoleDataByGUID(TempData->GItemID);
	if (Role == NULL)
	{
		Print(LV6, "_ChangeWorldDelay", "Role == NULL");
		return 0;
	}

	ChangeWorldDirect(TempData->GItemID, TempData->WorldID, TempData->ZoneID, TempData->RoomID, TempData->X, TempData->Y, TempData->Z, TempData->Dir);

	delete TempData;
	return 0;
}
void     Global::ChangeWorldDirect(int GItemID, int WorldID, int ZoneID, int RoomID, float X, float Y, float Z, float Dir)
{
	BaseItemObject* OwnerObj = BaseItemObject::GetObj(GItemID);
	if (OwnerObj == NULL)
		return;

	RoleDataEx* Role = OwnerObj->Role();

	//if( OwnerObj->Role()->TempData.Sys.OnChangeZone == true )
	//	return;

	//if( OwnerObj->Role()->TempData.IsDisabledChangeZone == true )
	//	return;


	if (Role->SecRoomID() == -1)
		return;

	PlayerRoleData PlayerRole = *((PlayerRoleData*)Role);

	RoleDataEx* TempPlayer = (RoleDataEx*)&PlayerRole;

	TempPlayer->Pos(X, Y, Z, Dir);
	TempPlayer->RoomID(RoomID);
	TempPlayer->ZoneID(ZoneID);

	TempPlayer->TempData.Attr.Action.ChangeZone = true;

	NetSrv_CliConnectChild::SBL_ChangeZoneToOtherWorld(WorldID, ZoneID, PlayerRole);
}