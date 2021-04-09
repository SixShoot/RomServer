//----------------------------------------------------------------------------------------
#include "Global.h"
#include "../NetWalker_Member/NetWakerGSrvInc.h"
#include "GMCommand.h" 
#include "MagicProc/MagicProcess.h"
#include "FlagPosList/FlagPosList.h"
#include "pathmanage/NPCMoveFlagManage.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
#include "LuaPlot\Luaplot.h"
#include "Rune Engine/Rune/Rune.h"
#include "HousesManage/HousesManageClass.h"
#include "lua/debugger/LuaDebugger.h"


//----------------------------------------------------------------------------------------
void    GmCommandClass::RegEvent(ManagementENUM ManageLv, string Name, GmCmdFunction Fun, wstring Help)
{
	GmCmdManageStruct GMS;
	GMS.Init(ManageLv, Name, Fun, Help);
	_GMCmd[GMS.Name] = GMS;
}
//----------------------------------------------------------------------------------------
//初始化
GmCommandClass::GmCommandClass()
{
	RegEvent(EM_Management_GameVisitor, "help", &GmCommandClass::GM_Help, L"Help 列出全部或某英文字開頭的GM指令\n help a or help ");
	//RegEvent( EM_Management_GameCreator , "GMCommand"			, &GmCommandClass::GM_EnabledGMCommand 	, L"GMCommand 致能或關閉\n GMCommand on(off)" );
	RegEvent(EM_Management_BigGM, "Give", &GmCommandClass::GM_Give, L"Give 給予物品\n Give TargetName ItemID or Give ItemID");
	RegEvent(EM_Management_BigGM, "DelBodyItem", &GmCommandClass::GM_DelBodyItem, L"DelBodyItem 刪除物品\n DelBodyItem TargetName ItemID Count or Give ItemID Count");
	RegEvent(EM_Management_GameCreator, "TestNPCSave", &GmCommandClass::GM_TestNPCSave, L"測試");
	RegEvent(EM_Management_GameCreator, "TestNPCLoad", &GmCommandClass::GM_TestNPCLoad, L"測試");
	RegEvent(EM_Management_GameVisitor, "Zone", &GmCommandClass::GM_ChangeZone, L"換區測試\n Zone ZoneID , RoomID , X , Y , Z ");
	RegEvent(EM_Management_GameCreator, "World", &GmCommandClass::GM_ChangeWorld, L"換世界測試\n World WorlID ZoneID , RoomID , X , Y , Z ");
	RegEvent(EM_Management_GameCreator, "WorldReturn", &GmCommandClass::GM_ReturnWorld, L"回到原來的世界\n WorldReturn");
	RegEvent(EM_Management_GameCreator, "ParallelID", &GmCommandClass::GM_ChangeParallelID, L"換平行世界\n ParallelID ID ");

	RegEvent(EM_Management_GameVisitor, "Room", &GmCommandClass::GM_ChangeRoom, L"空間測試\n Room RoomID");
	RegEvent(EM_Management_GameCreator, "RunPlot", &GmCommandClass::GM_RunPlot, L"劇情測試\n RunPlot OwnerID PlotName \nor RunPlot PlotName \nor RunPlot OwnerID PlotName TargetID");
	RegEvent(EM_Management_GameCreator, "PCall", &GmCommandClass::GM_Pcall, L"直接執行某劇情\n\n PCall OwnerID PlotName \nor PCall PlotName \nor PCall OwnerID PlotName TargetID");
	RegEvent(EM_Management_GameCreator, "CreateObj", &GmCommandClass::GM_CreateObj, L"CreateObj 產生物件於世界\n CreateObj OrgObjID , x , y , z , dir , count , Mode , vx , vy , vz");
	RegEvent(EM_Management_GameCreator, "ReloadPlot", &GmCommandClass::GM_ReloadPlot, L"ReloadPlot 重新載入LUA劇情");
	RegEvent(EM_Management_GameCreator, "ReloadPlotEx", &GmCommandClass::GM_ReloadPlotEx, L"ReloadPlot 重新載入LUA Ex劇情");
	RegEvent(EM_Management_GameCreator, "ReloadObj", &GmCommandClass::GM_ReloadObj, L"ReloadObj 重新載入模板資料");
	RegEvent(EM_Management_BigGM, "GiveMoney", &GmCommandClass::GM_GiveMoney, L"GiveMoney 給予金錢\n GiveMoney 10000");
	RegEvent(EM_Management_BigGM, "GiveMoney_Account", &GmCommandClass::GM_GiveMoney_Account, L"GiveMoney_Account 給予帳號幣\n GiveMoney_Account 10000");
	RegEvent(EM_Management_BigGM, "GiveMoney_Bonus", &GmCommandClass::GM_GiveMoney_Bonus, L"GiveMoney_Bonus 給予紅利幣\n GiveMoney_Bonus 10000");
	RegEvent(EM_Management_GameCreator, "DelObj", &GmCommandClass::GM_DelObj, L"DelObj 刪除物件\nDelObj 0x10000");
	RegEvent(EM_Management_GameCreator, "Modify", &GmCommandClass::GM_ModifyObj, L"Modify 修改物件\nModify 0x10000");
	RegEvent(EM_Management_GameCreator, "DelKeyItem", &GmCommandClass::GM_DelKeyItem, L"DelKeyItem 刪除人物身上的重要物品");

	RegEvent(EM_Management_GameVisitor, "GotoID", &GmCommandClass::GM_GotoID, L"GotoID 跳到某物件的位置\nGotoID 0x10000");
	RegEvent(EM_Management_GameVisitor, "GotoOrgID", &GmCommandClass::GM_GotoOrgID, L"GotoOrgID 跳到某物件的位置\nGotoID OrgID (N第幾隻)\nGotoID 100052 2 ");
	RegEvent(EM_Management_GameCreator, "SearchObj", &GmCommandClass::GM_SearchObj, L"SearchObj 搜尋物件\nSearchObj Type(0 all 1 player 2 npc) 名稱");
	RegEvent(EM_Management_GameCreator, "SearchObjCount", &GmCommandClass::GM_SearchObjCount, L"SearchObjCount 搜尋物件數量\nSearchObjCount Type(0 room 1 all )");
	RegEvent(EM_Management_GameService, "AttackFlag", &GmCommandClass::GM_AttackFlag, L"AttackFlag 開關攻擊旗標\nAttackFlag 0x10000");
	RegEvent(EM_Management_GameCreator, "NPCGoHome", &GmCommandClass::GM_NPCGoHome, L"NPCGoHome 開關攻擊旗標");
	RegEvent(EM_Management_GameCreator, "NPCMoveFlag", &GmCommandClass::GM_NPCMoveFlag, L"NPCMoveFlag 開關所有NPC移動旗標");
	RegEvent(EM_Management_GameCreator, "UsePathFind", &GmCommandClass::GM_UsePathFind, L"PathFind 開關PathFind");

	RegEvent(EM_Management_GameCreator, "CreateFlag", &GmCommandClass::GM_CreateFlag, L"CreateFlag 建立旗子\nCreateFlag 1(旗號類) 1(ID)");
	RegEvent(EM_Management_GameCreator, "ShowFlag", &GmCommandClass::GM_ShowFlag, L"ShowFlag 顯示旗子\nShowFlag FlagID(-1代表全部)");
	RegEvent(EM_Management_GameCreator, "HideFlag", &GmCommandClass::GM_HideFlag, L"HideFlag 隱藏旗子\nHideFlag FlagID(-1代表全部)");

	RegEvent(EM_Management_GameCreator, "SetValue", &GmCommandClass::GM_SetValue, L"SetValue 改變角色基本數值 \n 範例 /SetValue GItemID valuename value /n valuename(LV EXP RACE VOC STR WIS ...) /n SetValue 1 STR 100");
	RegEvent(EM_Management_GameCreator, "SetValueID", &GmCommandClass::GM_SetValueID, L"SetValueID 改變角色基本數值 \n 範例 /SetValueID GItemID ID value /n");
	RegEvent(EM_Management_GameCreator, "SpellMagic", &GmCommandClass::GM_SpellMagic, L"SpellMagic 角色施法 \n 範例 /SpellMagic TargetID MagicID");

	RegEvent(EM_Management_GameCreator, "GetValue", &GmCommandClass::GM_GetValue, L"GetValue GUID TYPE/n");

	RegEvent(EM_Management_GameCreator, "QuestFlag", &GmCommandClass::GM_QuestFlag, L"設定 Quest 狀態\n QuestFlag [ all or number] [ 0 = off, 1 = on ]");
	RegEvent(EM_Management_GameCreator, "ClsQuest", &GmCommandClass::GM_ClsQuest, L"清除所有任務資訊");


	RegEvent(EM_Management_GameCreator, "CreatePet", &GmCommandClass::GM_CreatePet, L"產生一個寵物物件\n CreatePet 物件ID LV 名字 PetType");
	RegEvent(EM_Management_GameCreator, "PetCmd", &GmCommandClass::GM_PetCmd, L"命令寵物\n PetCmd [ Stop , Follow , Attack , AttackOff , Guard , Protect ] TargetID\n例 PetCmd Stop or PetCmd Attack 0x10000...");

	RegEvent(EM_Management_GameService, "CallPlayerName", &GmCommandClass::GM_CallPlayerName, L"把某玩家 呼叫過來\n CallPlayerName RoleName");
	RegEvent(EM_Management_GameVisitor, "GotoPlayerName", &GmCommandClass::GM_GotoPlayerName, L"傳送到某玩家旁\n GotoPlayerName RoleName");

	RegEvent(EM_Management_GameVisitor, "Logout", &GmCommandClass::GM_Logout, L"切斷自己與 GameServer 的連線");
	RegEvent(EM_Management_GameVisitor, "Who", &GmCommandClass::GM_Who, L"列出所有限上玩家\n Who ZoneID");

	RegEvent(EM_Management_GameCreator, "ShowKeyItem", &GmCommandClass::GM_ShowKeyItem, L"顯示角色所擁有的特殊物品");

	RegEvent(EM_Management_GameCreator, "OpenAC", &GmCommandClass::GM_OpenAC, L"開啟AC");
	RegEvent(EM_Management_GameCreator, "ChangeJob", &GmCommandClass::GM_ChangeJob, L"開啟換角");
	RegEvent(EM_Management_GameCreator, "SaveNPCMove", &GmCommandClass::GM_TestSaveNPCMove, L"測試儲存NPC移動點\n SaveNPCMove ObjID FlagID Range MoveType(1walk 2run 3rand) WaitTime_Base WaitTime_Rand");

	RegEvent(EM_Management_GameCreator, "ShowNPCMove", &GmCommandClass::GM_ShowNPCMoveFlag, L"顯示儲存NPC移動點\n ShowNPCMove ObjID FlagID");
	RegEvent(EM_Management_GameCreator, "DelFlagList", &GmCommandClass::GM_DelFlagList, L"刪除某系列的旗子\n DelFlagList FlagID");

	RegEvent(EM_Management_GameCreator, "SetPathByFlag", &GmCommandClass::GM_SetPathByFlag, L"測試儲存NPC移動點\n SetPathByFlag ObjID FlagID");
	RegEvent(EM_Management_GameCreator, "GetPathInfo", &GmCommandClass::GM_GetPathInfo, L"Obj 巡邏路徑資料\n GetPathInfo ObjID");
	RegEvent(EM_Management_GameCreator, "SetPathInfo", &GmCommandClass::GM_SetPathInfo, L"Obj 巡邏路徑資料\n SetPathInfo ObjID PathNodeID");
	RegEvent(EM_Management_GameCreator, "SetPathLua", &GmCommandClass::GM_SetPathLua, L"Obj 巡邏路徑資料\n SetPathLua ObjID Lua");
	RegEvent(EM_Management_GameCreator, "SetPathPos", &GmCommandClass::GM_SetPathPos, L"Obj 巡邏路徑資料\n SetPathLua ObjID Lua");
	RegEvent(EM_Management_GameCreator, "AddPathPos", &GmCommandClass::GM_AddPathPos, L"Obj 巡邏路徑資料\n SetPathLua ObjID Lua");
	RegEvent(EM_Management_GameCreator, "DelPathPos", &GmCommandClass::GM_DelPathPos, L"Obj 巡邏路徑資料\n SetPathLua ObjID Lua");
	RegEvent(EM_Management_GameCreator, "SavePathPos", &GmCommandClass::GM_SavePathPos, L"Obj 巡邏路徑資料\n SavePathLua ObjID");

	RegEvent(EM_Management_GameCreator, "ReviveType", &GmCommandClass::GM_SetReviveType, L"NPC 重生時間模式改變\n 例 ReviveType ");
	RegEvent(EM_Management_GameCreator, "ResetEQ", &GmCommandClass::GM_ResetEQ, L"重設該等級的裝備\n 例 ResetEQ Level Rank");

	RegEvent(EM_Management_GameCreator, "SetGuildReady", &GmCommandClass::GM_SetGuildReady, L"設定連署公會轉為正式公會\n SetGuildReady");
	RegEvent(EM_Management_GameCreator, "DelGuild", &GmCommandClass::GM_DelGuild, L"刪除自己的公會");

	RegEvent(EM_Management_GameCreator, "CheckMem", &GmCommandClass::GM_CheckMem, L"記憶體檢查");
	RegEvent(EM_Management_GameCreator, "ClearInstanceInfo", &GmCommandClass::GM_ClearInstanceInfo, L"清除副本進度");

	RegEvent(EM_Management_GameCreator, "ObjVersion", &GmCommandClass::GM_ObjVersion, L"取得物品的版本資訊");
	RegEvent(EM_Management_GameCreator, "SetMagicPoint", &GmCommandClass::GM_SetMagicPoint, L"設定法術配點");
	RegEvent(EM_Management_GameCreator, "KillRate", &GmCommandClass::GM_SetKillRate, L"設定殺一隻怪物等於多少隻 \n KillRate N");
	RegEvent(EM_Management_GameCreator, "DSysTime", &GmCommandClass::GM_DSysTime, L"加快系統多少分鐘的時間 \n DSysTime (N分鐘)");

	RegEvent(EM_Management_GameCreator, "ItemExchange", &GmCommandClass::GM_ItemExchange, L"兌現密碼商品\n ItemExchange (密碼)");
	RegEvent(EM_Management_GameCreator, "SetTitleStr", &GmCommandClass::GM_SetTitleStr, L"設定自定頭銜\n SetTitleStr　(自定頭銜)");

	RegEvent(EM_Management_GameService, "GMHide", &GmCommandClass::GM_GMHide, L"開起或關閉GM隱身效果\n GMHide");
	RegEvent(EM_Management_GameCreator, "RandCreateFlag", &GmCommandClass::GM_RandCreateFlag, L"亂數產生旗子\n RandCreateFlag FlagID Range MoveDest Count");
	RegEvent(EM_Management_GameCreator, "ClearGuildWarTime", &GmCommandClass::GM_ClearGuildWarTime, L"ClearGuildWarTime");


	RegEvent(EM_Management_GameCreator, "RoleRight", &GmCommandClass::GM_RoleRight, L"禁止開啟角色某些能力\n RoleRight id 類型(trade,talk) Value(0關1開)\n例: RoleRight %t_id trade 0 ");
	RegEvent(EM_Management_GameVisitor, "ZonePlayerCount", &GmCommandClass::GM_ZonePlayerCount, L"區域人數資訊");
	RegEvent(EM_Management_GameCreator, "PKFlag", &GmCommandClass::GM_PKFlag, L"開啟PK\n例 PKFlag Type(0關 1單人 2Party 3Party&Guild)");

	RegEvent(EM_Management_GameCreator, "GMTell", &GmCommandClass::GM_GMTell, L"GM工具用發訊息用\n GMTell Msg or GMTell PlayerDBID Msg");
	RegEvent(EM_Management_GameCreator, "StopLogin", &GmCommandClass::GM_StopLogin, L"暫時讓此區域不可登入\nStopLogin Type(0可登入 1不可登入)");
	RegEvent(EM_Management_GameCreator, "PingLog", &GmCommandClass::GM_PingLog, L"寫Client封包Ping的資料\nPingLog 次數");
	RegEvent(EM_Management_GameCreator, "ProxyPingLog", &GmCommandClass::GM_ProxyPingLog, L"寫Proxy封包Ping的資料\nProxyPingLog 次數");
	RegEvent(EM_Management_GameCreator, "SrvProcTimeLog", &GmCommandClass::GM_SrvProcTimeLog, L"寫Ser Proc Time Log的資料\nSrvProcTimeLog 次數");
	RegEvent(EM_Management_GameCreator, "CancelAcSellItem", &GmCommandClass::GM_CancelAcSellItem, L"退回AC販賣的物品\n CancelAcSellItem PlayerDBID , GUID");


	//#####################################################################################################################
	RegEvent(EM_Management_GameCreator, "SkillValue", &GmCommandClass::GM_SkillValue, L"SkillValue修改玩家技能，技能代碼請查表\n SkillValue 技能編號 點數");
	RegEvent(EM_Management_GameCreator, "SkillFull", &GmCommandClass::GM_SkillFull, L"SkillFull 修改玩家技能，讓熟練度全滿\n SkillFull");
	RegEvent(EM_Management_GameCreator, "CardFull", &GmCommandClass::GM_CardFull, L"CardFull 修改玩家卡片，讓卡片全滿\n CardFull");
	RegEvent(EM_Management_BigGM, "Full", &GmCommandClass::GM_Full, L"Full HP MP SP 全滿 \n Full");
	RegEvent(EM_Management_GameService, "CallID", &GmCommandClass::GM_CallID, L"CallID(單區域)把某物件呼叫到面前\n CallID 區域物件ID");
	RegEvent(EM_Management_GameService, "CallName", &GmCommandClass::GM_CallName, L"CallName(單區域)把某物件呼叫到面前\n CallName 區域物件名稱");
	RegEvent(EM_Management_GameVisitor, "GotoName", &GmCommandClass::GM_GotoName, L"GotoName(單區域)傳送到某物件\n GotoName 區域物件名稱");
	RegEvent(EM_Management_GameCreator, "ResetMagic", &GmCommandClass::GM_ResetMagic, L"ResetMagic把身上的所有法術狀態消除\n~ResetMagic 區域物件ID");
	RegEvent(EM_Management_BigGM, "KillID", &GmCommandClass::GM_KillID, L"KillID瞬間殺死玩家或NPC\n KillID 區域物件ID");
	RegEvent(EM_Management_BigGM, "KickID", &GmCommandClass::GM_KickID, L"KickID把某玩家踢出遊戲\n KickID 區域物件ID");
	RegEvent(EM_Management_GameCreator, "KickBanID", &GmCommandClass::GM_KickBanID, L"KickBanID把某玩家踢出遊戲,並把帳號凍結\n KickBanID 區域物件ID");
	RegEvent(EM_Management_GameCreator, "KickAll", &GmCommandClass::GM_KickAll, L"KickAll把所有玩家踢出遊戲\n KickAll");
	RegEvent(EM_Management_GameVisitor, "BcastAll", &GmCommandClass::GM_BcastAll, L"BcastAll全區廣播\n BcastAll 全區廣播的內容");
	RegEvent(EM_Management_GameVisitor, "Bcast", &GmCommandClass::GM_Bcast, L"Bcast區廣播\n Bcast 區廣播的內容");
	RegEvent(EM_Management_GameCreator, "FaceOff", &GmCommandClass::GM_FaceOff, L"Faceoff變身成怪物\n Faceoff 怪物物件ID 之後的名字");
	RegEvent(EM_Management_GameCreator, "Visible", &GmCommandClass::GM_Visible, L"Visible設定角色隱形開關，此種隱形玩家查不到\n Visible");
	RegEvent(EM_Management_GameCreator, "OrgObjList", &GmCommandClass::GM_OrgObjList, L"OrgObjList列表區域物件資料\n OrgObjList 類型(NPC QuestNPC Item Armor Weapon Magic Title Suit Recipe  ) 開始ID 結束ID\n例如: ObjList Weapon 0 100");
	RegEvent(EM_Management_GameCreator, "SearchOrgObj", &GmCommandClass::GM_SearchOrgObj, L"SearchOrgObj尋找原始物件上面有某一個字串的命令\n SearchOrgObj 字串 \n字串 : (*) 代表全部，其它只要物件名稱類有函此字串就算\n例如 : ~SearchOrgObj 狼");
	RegEvent(EM_Management_GameCreator, "BindingItemMagic", &GmCommandClass::GM_BindingItemMagic, L"BindingItemMagic設定某一個物件使用的效果，(此物件原本需要可以使用)\n BindingItemMagic 物品ID 法術ID");
	RegEvent(EM_Management_GameCreator, "TimeSpeed", &GmCommandClass::GM_TimeSpeed, L"TimeSpeed 加速遊戲 \n TimeSpeed 倍數 ");
	RegEvent(EM_Management_BigGM, "ResetColdown", &GmCommandClass::GM_ResetColdown, L"ResetColdown 把法術的Coldown 時間清除\n例 ResetColdow");
	RegEvent(EM_Management_BigGM, "NewResetColdown", &GmCommandClass::GM_NewResetColdown, L"NewResetColdown 把法術的Coldown 時間清除,但有設定的不能清除的不會清");
	//RegEvent( EM_Management_GameService , "ResetCharHead"		, &GmCommandClass::GM_ResetCharHead		, "RestRoleHead" );
	RegEvent(EM_Management_GameVisitor, "ShowRoom", &GmCommandClass::GM_ShowRoom, L"GM_ShowRoom");
	RegEvent(EM_Management_GameCreator, "GetMineFlag", &GmCommandClass::GM_GetMineFlag, L"找尋礦物件, 裡面 PID 為 FLAG 的物件數量");

	RegEvent(EM_Management_GameCreator, "Crash", &GmCommandClass::GM_Crash, L"CRASH");
	RegEvent(EM_Management_GameCreator, "CreateImageNPC", &GmCommandClass::GM_CreateImageNPC, L"產生不同圖行的NPC\nCreateImageNPC BeginNPCID EndNPCID 例:nCreateImageNPC 100052 100062");
	RegEvent(EM_Management_GameVisitor, "RandTransport", &GmCommandClass::GM_TestRandTransport, L"亂數傳送到某NPC ，用來測機器人\n例:RandTransport");
	RegEvent(EM_Management_GameCreator, "CompareTarget", &GmCommandClass::GM_CompareTarget, L"與目標的數值關係 \n CompareTarget ID Type(0基本 1攻防比 2閃避比)");
	RegEvent(EM_Management_GameCreator, "SetBTI", &GmCommandClass::GM_SetBTI, L"設定 BTI 防沉迷時間");
	RegEvent(EM_Management_GameCreator, "DelUnSaveObject", &GmCommandClass::GM_DelUnSaveObject, L"刪除所有沒儲存的物件 \nDelUnSaveObject)");

	RegEvent(EM_Management_GameCreator, "ResetDailyQuest", &GmCommandClass::ResetDailyQuest, L"重置每日任務\n");
	RegEvent(EM_Management_GameCreator, "SetLua", &GmCommandClass::SetLua, L"設定ZONE LUA XXX XXXX\n");
	RegEvent(EM_Management_GameCreator, "SetRoleCamp", &GmCommandClass::SetRoleCamp, L"SetRoleCamp\n");

	RegEvent(EM_Management_GameCreator, "ClearTitle", &GmCommandClass::GM_ClearTitle, L"ClearTitle\n 清除所有已得到的頭銜");
	RegEvent(EM_Management_GameCreator, "ClearHuntingCount", &GmCommandClass::GM_ClearHuntingCount, L"ClearHuntingCount\n清除殺怪的計數");
	RegEvent(EM_Management_GameCreator, "HideNPC", &GmCommandClass::GM_HideNPC, L"HideNPC 把某個ID的NPC隱藏或顯示\nHideNPC (0 Hide 1 Show) 物件號碼\n例 ? HideNPC 0 100001");

	RegEvent(EM_Management_GameCreator, "ClientSkyType", &GmCommandClass::GM_ClientSkyType, L"ClientSkyType設定Client天空顯示類型\nClientSkyType (0 Normal 1 day 2 nightfall 3 night 4 dawn)");
	RegEvent(EM_Management_GameCreator, "CalReborn", &GmCommandClass::GM_CalReborn, L"CalReborn計算重生物件數量(一天)\nCalRebone ObjID,Count,RebornTime");
	RegEvent(EM_Management_GameCreator, "SetIni", &GmCommandClass::GM_SetIni, L"SetIni重設一些Ini設定\n SetIni iniStr Value\n例SetIni IsAutoRevive \n SetIni DeadExpDownRate 5");
	RegEvent(EM_Management_GameCreator, "RunLottery", &GmCommandClass::GM_RunLottery, L"RunLottery 立即Lottery開獎");
	RegEvent(EM_Management_GameCreator, "SetChangeGuild", &GmCommandClass::GM_SetChangeGuild, L"要求公會名稱");
	RegEvent(EM_Management_GameCreator, "PlantGrowRate", &GmCommandClass::GM_PlantGrowRate, L"設定種植成長倍速\n例 PlantGrowRate 10");
	RegEvent(EM_Management_GameCreator, "GuildWarTime", &GmCommandClass::GM_GuildWarTime, L"設定公會屋戰目前的時間\n例:GuildWarTime 星期(-1表示用目前的) 小時(-1表示用目前的) \n GuildWarTime -1 21");
	//RegEvent( EM_Management_GameCreator , "SQL"					, &GmCommandClass::GM_SQLCmd			, L"SQL Select命令查尋");	

	RegEvent(EM_Management_GameCreator, "CheckGuildNameRules", &GmCommandClass::GM_CheckGuildNameRules, L"測試字串是否符合公會命名規則");
	RegEvent(EM_Management_GameCreator, "GetObjectInfo", &GmCommandClass::GM_GetObjectInfo, L"GetObjectInfo %t_id , 取得完整的物件資訊");
	//#####################################################################################################################
	//PE 測試用GM命令
	RegEvent(EM_Management_GameCreator, "PERelease", &GmCommandClass::GM_PE_ReleasePE, L"釋放指定的PE");
	RegEvent(EM_Management_GameCreator, "PEReset", &GmCommandClass::GM_PE_Reset, L"重置指定index的PE");
	RegEvent(EM_Management_GameCreator, "PEGetStatus", &GmCommandClass::GM_PE_GetStatus, L"取得指定PE的狀態");
	RegEvent(EM_Management_GameCreator, "PEGetDetail", &GmCommandClass::GM_PE_GetDetail, L"取得指定PE的詳細狀態");
	RegEvent(EM_Management_GameCreator, "PEInstSucc", &GmCommandClass::GM_PE_InstSucc, L"指定index的PE直接");
	RegEvent(EM_Management_GameCreator, "PEInstFail", &GmCommandClass::GM_PE_InstFail, L"指定index的PE直接失敗");
	RegEvent(EM_Management_GameCreator, "PEJumpToPH", &GmCommandClass::GM_PE_JumpToPH, L"跳到指定index的PE內指定index的Phase");
	RegEvent(EM_Management_GameCreator, "PEPHInstSucc", &GmCommandClass::GM_PE_PH_InstSucc, L"指定index的Phase直接");
	RegEvent(EM_Management_GameCreator, "PEPHInstFail", &GmCommandClass::GM_PE_PH_InstFail, L"指定index的Phase直接失敗");
	RegEvent(EM_Management_GameCreator, "PEOBInstAchieve", &GmCommandClass::GM_PE_OB_InstAchieve, L"指定的index的Objective直接達成");
	RegEvent(EM_Management_GameCreator, "PEListAllVar", &GmCommandClass::GM_PE_ListAllVar, L"列出所有PE使用的變數名稱及值");
	RegEvent(EM_Management_GameCreator, "PEGetVar", &GmCommandClass::GM_PE_GetVar, L"列出指定名稱變數的值");
	RegEvent(EM_Management_GameCreator, "PESetVar", &GmCommandClass::GM_PE_SetVar, L"設定指定名稱變數的值");
	RegEvent(EM_Management_GameCreator, "PEAddVar", &GmCommandClass::GM_PE_AddVar, L"增加指定名稱變數的值");
	RegEvent(EM_Management_GameCreator, "PEGiveScore", &GmCommandClass::GM_PE_GiveScore, L"幫指定的玩家加分");
	RegEvent(EM_Management_GameCreator, "PEGetScore", &GmCommandClass::GM_PE_GetScore, L"取得指定的玩家的分數");

	//#####################################################################################################################


	//#####################################################################################################################
	//IDS_STRING457 = InfoID查詢某物件資訊\n~InfoID 區域物件ID 查詢形式\n例如:~InfoID 1 1\n形式代碼\n  0 名稱職業\n  1 攻擊防禦力\n  2 詳細基本數值\n  3 位置\n  4 體力耐敏等數值\n  5 等級血量經驗值\n  6 各項基本技能\n  7 需計算出來的各項數值)
	//IDS_STRING466 = Rename修改某一個角色的名字\n~Rename 原來角色名稱 後來角色名稱
	//IDS_STRING472 = DebugPlot劇情除錯，設定某一個物件跑的劇情步驟輸出給下指令者，最好同時設定目標與自己劇情常會在玩家與NPC身上轉換\n~DebugPlot 區域物件ID 劇情ID
	//GMS.Help	= "系統時間設定\n~SystemTime 天數差 小時差 \n~SystemTime 1 1";
	//IDS_STRING496 = NoSpeak讓玩家禁言幾秒鐘，下線後就不算了\n~NoSpeak 玩家名稱 秒數
	//IDS_STRING504 = MaxOnline設定最高上線人數\n~MaxOnline 最高上線人數
	//IDS_STRING506 = GameTime設定遊戲偏移時間 \n~GameTime 偏移時間(小時)
	//IDS_STRING762 = CallSpell 設定此區域是否可以施呼叫同伴的法術\n例~CallSpell false
	//IDS_STRING772 = SetFightID 設定群體戰的組別\n例~SetFightID 物件ID 組別 \n~SetFightID 100 10
	//#####################################################################################################################

	//GM_GetLuaErrorInfo
	RegEvent(EM_Management_BigGM, "LuaDebug", &GmCommandClass::GM_GetLuaErrorInfo, L"設定是否取得 LUA 錯誤資訊");

	RegEvent(EM_Management_GameCreator, "TestLuaPause", &GmCommandClass::GM_TestLuaPause, L"測試劇情暫停");
	RegEvent(EM_Management_GameCreator, "TestLuaStart", &GmCommandClass::GM_TestLuaStart, L"測試劇情繼續");
	RegEvent(EM_Management_GameCreator, "StopAllVM", &GmCommandClass::GM_StopAllVM, L"暫停");
	//#####################################################################################################################
	RegEvent(EM_Management_GameCreator, "ListLuaGVars", &GmCommandClass::GM_ListLuaGVars, L"列出Lua全部的全域變數");
	RegEvent(EM_Management_GameCreator, "ListLuaTable", &GmCommandClass::GM_ListLuaTable, L"列出LuaTable");
	RegEvent(EM_Management_GameCreator, "GetLuaTableInfo", &GmCommandClass::GM_GetLuaTableInfo, L"取得LuaTable內容");
	RegEvent(EM_Management_GameCreator, "EnableLogPCall", &GmCommandClass::GM_EnableLogPCall, L"開啟記錄PCall歷程");
	RegEvent(EM_Management_GameCreator, "DisableLogPCall", &GmCommandClass::GM_DisableLogPCall, L"關閉記錄PCall歷程");
	//#####################################################################################################################
	RegEvent(EM_Management_GameCreator, "OpenWeekInst", &GmCommandClass::GM_OpenWeekInstancesFrame, L"打開鏡世界副本介面");
	RegEvent(EM_Management_GameCreator, "ClearPhantom", &GmCommandClass::GM_ClearPhantom, L"清除所有的幻靈");
	RegEvent(EM_Management_GameCreator, "OpenPhantom", &GmCommandClass::GM_OpenPhantomFrame, L"打開幻靈介面");
	RegEvent(EM_Management_GameCreator, "FixStuckItem", &GmCommandClass::GM_FixStuckItem, L"FixStuckItem");
}
//----------------------------------------------------------------------------------------
float		GmCommandClass::_GetFloat(int ID)
{
	if (ID >= (int)_Val.size())
		return 0;

	return (float)atof((char*)_Val[ID].c_str());

}
//----------------------------------------------------------------------------------------
int		GmCommandClass::_GetNum(int ID)
{
	if (ID >= (int)_Val.size())
		return 0;

	int Ret = atol(_Val[ID].c_str());
	if (Ret != 0)
		return Ret;

	Ret = strtol(_Val[ID].c_str(), NULL, 16);
	return Ret;

}
//----------------------------------------------------------------------------------------
char* GmCommandClass::_GetStr(int ID)
{
	if (ID >= (int)_Val.size())
		return "";

	return (char*)_Val[ID].c_str();

}
//----------------------------------------------------------------------------------------
void	GmCommandClass::Message(RoleDataEx* Owner, const char* Msg)
{
	Owner->Msg(Msg);
	_CmdResultStr = Msg;
}
//----------------------------------------------------------------------------------------
const char* MyStrToken(char* Buf, char** NextBufPoint)
{
	if (Buf == NULL || NextBufPoint == NULL)
		return NULL;

	static string Str;
	Str.clear();

	bool IsDoubleQuotes = false;

	for (;; Buf++)
	{
		if (*Buf == 0)
			return NULL;

		if (*Buf == '\"')
		{
			Buf++;
			IsDoubleQuotes = true;
			break;
		}
		bool IsFind = false;

		if (*Buf == '\t' || *Buf == ' ')
			continue;

		//		if( *Buf == ',' )
		//		{
		//			*NextBufPoint = Buf+1;
		//			return "";
		//		}
		break;
	}

	if (IsDoubleQuotes != false)
	{
		for (;; Buf++)
		{
			if (*Buf == 0)
			{
				*NextBufPoint = NULL;
				return Str.c_str();
			}
			if (*Buf == '\"')
				break;

			Str.push_back(*Buf);

		}
	}

	for (;; Buf++)
	{
		if (*Buf == 0)
		{
			*NextBufPoint = NULL;
			return Str.c_str();
		}
		if (*Buf == ',')
			break;
		if (IsDoubleQuotes == false && *Buf == ' ')
			break;
		if (IsDoubleQuotes == false)
			Str.push_back(*Buf);
	}

	*NextBufPoint = Buf + 1;
	return Str.c_str();
}
#include "Global.h"

//GM命令處理
bool	GmCommandClass::GMCmdProc(BaseItemObject* OwnerObj, const char* Str, const char* RoleName, ManagementENUM ManageLv, bool checkPassword)
{
	if (OwnerObj == NULL)
	{
		//隨便找隻NPC
		OwnerObj = BaseItemObject::GetByOrder_NPC(true);
		if (OwnerObj == NULL)
		{
			OwnerObj = BaseItemObject::GetByOrder_Player(true);
			if (OwnerObj == NULL)
			{
				return false;
			}
		}
	}
	_CmdResultStr = "";

	//	int		i;
	char	seps[] = " ,\t\n";
	const char* Arg;
	char	Buf[1024];

	RoleDataEx* Owner = OwnerObj->Role();

	_Val.clear();

	if (Str[0] == '\0' || strlen(Str) <= 1)
		return false;

	strcpy(Buf, Str);

	map< string, GmCmdManageStruct >::iterator Iter;

	char* token = strtok(Buf, seps);

	//if( strcmp( token , "2013RuneWaker_Rom0628" ) == 0 )
	if(strcmp(token, Global::Ini()->GMPassword.c_str()) == 0)
	{
		Owner->BaseData.SysFlag.EnableGMCmd = true;
		Owner->SetValue(EM_RoleValue_SysFlag, Owner->BaseData.SysFlag._Value, NULL);
		Message(Owner, "OK");
		Print(LV3, "GMCmdProc", "enabled account= %s", Owner->Base.Account_ID.Begin());
		return true;
	}

	if (checkPassword && Owner->BaseData.SysFlag.EnableGMCmd == false)
	{
		Print(LV3, "GMCmdProc", "err account= %s str=%s ", Owner->Base.Account_ID.Begin(), Str);
		return true;
	}

	//轉成小寫
	_strlwr(token);

	Iter = _GMCmd.find(token);

	if (Iter == _GMCmd.end())
	{
		//Owner->Msg( "Command Error (/? hlep )" );
		Message(Owner, "Command Error (/? help )");
		return false;
	}


	_Val.push_back(token);

	GmCmdManageStruct GMS = Iter->second;
	char* NextBufPos;
	/*
		char* NextBufPos = Buf + strlen(token)+1;
		if( strlen(Str) == strlen(Buf) )
			NextBufPos = NULL;
			*/
	if (strlen(Str) == strlen(Buf))
	{
		NextBufPos = NULL;
	}
	else
	{
		for (NextBufPos = Buf; *NextBufPos != 0; NextBufPos++);
		NextBufPos++;
	}

	for (int i = 0; i < 30; i++)
	{
		Arg = MyStrToken(NextBufPos, &NextBufPos);
		if (Arg == NULL)
			break;
		_Val.push_back(Arg);
	}
	/*
	//取數值
	for( i = 0 ; i < 30 ; i++ )
	{
		Arg = strtok( NULL , seps );
		if( Arg== NULL )
			break;

		_Val.push_back( Arg );
	}
*/
//檢查是否為查尋指令
	if (_Val.size() >= 2 && _Val[1][0] == '?')
	{
		Owner->Msg(GMS.Help.c_str());
		return true;
	}

	if (Owner->TempData.Sys.GM_Designed == false)
	{
		if (GMS.ManageLv > ManageLv && Global::Ini()->IsEnabledGMCommand == false)
		{
			//Owner->Msg( "ManageLv Error" );
			Message(Owner, "ManageLv Error");
			return true;
		}
	}

	if ((this->*GMS.Fun)(OwnerObj) == false)
	{
		//Owner->Msg( "Command Error" );
		Message(Owner, "Command Error");
		NetSrv_Talk::SysMsg(Owner->GUID(), (char*)GMS.Help.c_str());
	}
	else
	{
		if (Owner->TempData.Sys.GM_Designed == false)
		{

			//寫Log
			Global::Log_GMCommand(Owner, Str, RoleName);
		}

	}


	return true;

}

//----------------------------------------------------------------------------------------
//Help 列出所有的GM指令
bool	GmCommandClass::GM_Help(BaseItemObject* OwnerObj)
{
	char* Key = _strlwr(_GetStr(1));
	int		i;
	char	Buf[256];
	char	Temp[256];

	RoleDataEx* Owner = OwnerObj->Role();

	map< string, GmCmdManageStruct >::iterator Iter;
	//Global::Msg( Owner->TempData.ItemID , "------------------ Help ------------------" );
	NetSrv_Talk::SysMsg(Owner->GUID(), "------------------ Help ------------------");
	for (Iter = _GMCmd.begin(); Iter != _GMCmd.end(); )
	{
		Buf[0] = 0;
		for (i = 0; i < 3 && Iter != _GMCmd.end(); Iter++)
		{
			if (Key[0] >= 'a' && Key[0] <= 'z')
			{
				if (Iter->second.Name.c_str()[0] != Key[0])
					continue;
			}
			sprintf_s(Temp, sizeof(Temp), "%-14s ", Iter->second.Name.c_str());
			strcat(Buf, Temp);
			i++;
		}
		//Owner->Tell( Buf );
		NetSrv_Talk::SysMsg(Owner->GUID(), Buf);

	}

	return true;
}
//----------------------------------------------------------------------------------------
//測試NPC Save & Load
bool    GmCommandClass::GM_TestNPCSave(BaseItemObject* OwnerObj)
{
	Net_DCBase::SaveNPCRequest(OwnerObj->Role());
	return true;
}
bool    GmCommandClass::GM_TestNPCLoad(BaseItemObject* OwnerObj)
{
	Net_DCBase::LoadAllNPC(Global::Ini()->ZoneID);
	return true;
}
//----------------------------------------------------------------------------------------
bool    GmCommandClass::GM_ChangeRoom(BaseItemObject* OwnerObj)
{
	int RoomID = _GetNum(1);

	NetSrv_MoveChild::ChangeRoom(OwnerObj, RoomID);

	return true;
}
//----------------------------------------------------------------------------------------
bool	GmCommandClass::GM_ChangeParallelID(BaseItemObject* OwnerObj)
{
	if (_Count() == 2)
	{
		int ParallelID = _GetNum(1);
		Global::ChangeParalledID(OwnerObj->GUID(), ParallelID);
		OwnerObj->Role()->Msg("OK");
	}
	else if (_Count() == 3)
	{
		int PlayerID = _GetNum(1);
		int ParallelID = _GetNum(2);

		Global::ChangeParalledID(PlayerID, ParallelID);


		OwnerObj->Role()->Msg("OK");
	}
	else
	{
		OwnerObj->Role()->Msg("Failed");
	}

	return true;
}
//----------------------------------------------------------------------------------------
bool    GmCommandClass::GM_ChangeZone(BaseItemObject* OwnerObj)
{
	int Zone = _GetNum(1);
	int RoomID = _GetNum(2);
	float X = _GetFloat(3);
	float Y = _GetFloat(4);
	float Z = _GetFloat(5);

	RoleDataEx* Role = OwnerObj->Role();

	if (X == -1)
	{
		X = Role->Pos()->X;
		Y = Role->Pos()->Y;
		Z = Role->Pos()->Z;
	}
	Global::ChangeZone(OwnerObj->GUID(), Zone, RoomID, X, Y, Z, 0);
	return true;
}
//-----------------------------------------------------------------------------------------
bool    GmCommandClass::GM_ReturnWorld(BaseItemObject* OwnerObj)
{
	RoleDataEx* Role = OwnerObj->Role();

	//	Global::DelFromPartition( Role->GUID() );
	//	NetSrv_CliConnect::SC_WorldReturnNotify( Role->GUID() );
	NetSrv_CliConnectChild::WorldReturnNotifyProc(Role->GUID());

	Role->LiveTime(10 * 1000, "Return World");
	Message(Role, "OK");
	return true;
}
//-----------------------------------------------------------------------------------------
bool    GmCommandClass::GM_ChangeWorld(BaseItemObject* OwnerObj)
{
	int WorldID = _GetNum(1);
	int Zone = _GetNum(2);
	int RoomID = _GetNum(3);
	float X = _GetFloat(4);
	float Y = _GetFloat(5);
	float Z = _GetFloat(6);

	RoleDataEx* Role = OwnerObj->Role();

	//Role->PlayerTempData->ChangeWorld.RetPos = Role->SelfData.RevPos;
	//Role->PlayerTempData->ChangeWorld.RetZoneID = Role->SelfData.RevZoneID;
//	Role->PlayerTempData->ChangeWorld.RetPos = *(Role->Pos());
//	Role->PlayerTempData->ChangeWorld.RetZoneID = RoleDataEx::G_ZoneID;


	Global::ChangeWorld(OwnerObj->GUID(), Role, RoleDataEx::G_ZoneID, *(Role->Pos()), WorldID, Zone, RoomID, X, Y, Z, 0);

	Message(Role, "OK");
	return true;
}
//-----------------------------------------------------------------------------------------
bool    GmCommandClass::GM_CreateObj(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	if (Owner->BaseData.RoomID >= Global::Ini()->BaseRoomCount)
	{
		//Owner->Msg( "Create RoomID Error" );
		Message(Owner, "Create RoomID Error");
		return true;
	}

	int		OrgObjID = _GetNum(1);
	char* Name = _GetStr(2);
	float	x = _GetFloat(3);
	float	y = _GetFloat(4);
	float	z = _GetFloat(5);
	float	dir = _GetFloat(6);
	int     count = _GetNum(7);
	char* AutoPlot = _GetStr(8);
	char* ClassName = _GetStr(9);
	int     QuestID = _GetNum(10);
	int     Mode = _GetNum(11);
	int     PID = _GetNum(12);
	float	vx = _GetFloat(13);
	float	vy = _GetFloat(14);
	float	vz = _GetFloat(15);

	if (x == 0 && y == 0 && z == 0)
	{
		x = float(int(Owner->Pos()->X));
		y = float(int(Owner->Pos()->Y));
		z = float(int(Owner->Pos()->Z));
		dir = float(int(Owner->Pos()->Dir));
	}
	//------------------------------------------------------------------------------------------------------------
	//位置與區域檢查
	GameObjDbStructEx* ZoneObj = Global::GetObjDB(Def_ObjectClass_Zone + RoleDataEx::G_ZoneID % _DEF_ZONE_BASE_COUNT_);
	if (ZoneObj != NULL)
	{
		if (ZoneObj->Zone.MapID != PathManageClass::MainMapID(x, y, z))
		{
			//Owner->Msg( "Not Control Area" );
			Message(Owner, "Not Control Area");
			return true;
		}
	}

	GameObjDbStructEx* ObjDB = Global::GetObjDB(OrgObjID);
	if (ObjDB == NULL)
	{
		//Owner->Msg( "Object ID Error" );
		Message(Owner, "Object ID Error");
		return true;
	}

	if (strcmp(Name, ObjDB->Name) == 0)
		Name = "";

	if (strcmp(Name, "-") == 0)
		Name = "";

	if (strcmp(AutoPlot, "-") == 0)
		AutoPlot = "";
	if (strcmp(ClassName, "-") == 0)
		ClassName = "";

	int ItemID = Global::CreateObj_Macro(Owner, OrgObjID, Name, x, y, z, dir, count
		, AutoPlot, ClassName, QuestID, Mode, PID, -1, vx, vy, vz, Owner->RoomID());

	if (ItemID == -1)
		return false;

	Message(Owner, "OK");

	return true;
}
//-----------------------------------------------------------------------------------------
//執行某劇情
bool	GmCommandClass::GM_RunPlot(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	int		ItemID;
	char* PlotName;
	int		TargetID = OwnerObj->GUID();
	if (_Count() == 2)
	{
		ItemID = OwnerObj->GUID();
		PlotName = _GetStr(1);
	}
	else if (_Count() == 3)
	{
		ItemID = _GetNum(1);
		PlotName = _GetStr(2);
	}
	else if (_Count() == 4)
	{
		ItemID = _GetNum(1);
		PlotName = _GetStr(2);
		TargetID = _GetNum(3);
	}
	else
	{
		return false;
	}

	BaseItemObject* OtherObj = Global::GetObj(ItemID);
	if (OtherObj == NULL)
	{
		//024 = 無此物件
		//Owner->Msg( "無此物件" );
		Message(Owner, "Object Not Find");
		return true;
	}

	OtherObj->PlotVM()->CallLuaFunc(PlotName, TargetID, 0);

	Message(Owner, "OK");
	return true;

}
//--------------------------------------------------------------------------------------
bool    GmCommandClass::GM_ReloadPlot(BaseItemObject* OwnerObj)
{
	//if( Global::Ini()->IsEnabledGMCommand == false )
	//	return false;

	LuaDebugger::BeginReloadPlot();

	LUA_VMClass::StopAllVM();
	/*
	LUA_VMClass::RecursiveLoadAllFile( Global::Ini()->LuaPath );
	*/
	if (Global::Ini()->IsLoadFdbLuaScript != false)
	{
		char	FileName[256];

		for (int i = 1; ; i++)
		{
			sprintf_s(FileName, sizeof(FileName), "luascript\\%05d.lua", i);
			IRuStream* stream = g_ruResourceManager->LoadStream(FileName);
			if (stream)
			{
				//				LUA_VMClass::DoBuffer( (char*)stream->OpenStreamMapping() , stream->GetStreamSize() );
				char* luabuff = NEW char[(stream->GetStreamSize() + 1)];
				ZeroMemory(luabuff, (stream->GetStreamSize() + 1));
				memcpy(luabuff, (char*)stream->OpenStreamMapping(), stream->GetStreamSize());
				LUA_VMClass::DoBuffer((char*)luabuff, stream->GetStreamSize(), FileName);
				delete[] luabuff;
				delete stream;


			}
			else
			{
				break;
			}
		}
	}

	//    LUA_VMClass::m_pfOutputError = BroadcastError; //LUA_VMClass 錯誤輸出
	if (Global::Ini()->IsLoadTxtLuaScript != false)
		LUA_VMClass::RecursiveLoadAllFile(Global::St()->Ini.LuaPath);

	LUA_VMClass::SysVM()->PCall("Initialize", 0, 0, NULL);
	//OwnerObj->Role()->Msg( "Reload Plot OK" );
	Message(OwnerObj->Role(), "Reload Plot OK");

	LuaDebugger::EndReloadPlot();

	return true;
}
bool    GmCommandClass::GM_ReloadPlotEx(BaseItemObject* OwnerObj)
{
	//if( Global::Ini()->IsEnabledGMCommand == false )
	//	return false;

	if (Global::St()->Ini.LuaPathEx.size() == 0)
	{
		Message(OwnerObj->Role(), "LuaPathEx unknow");
		return true;
	}

	LUA_VMClass::LoadAllFile(Global::St()->Ini.LuaPathEx);
	LUA_VMClass::SysVM()->CallLuaFunc("InitLuaEx", 0, 0, NULL);

	Message(OwnerObj->Role(), "Reload PlotEx OK");
	return true;
}
bool    GmCommandClass::GM_ReloadObj(BaseItemObject* OwnerObj)
{

	//ObjectDataClass::Release();
	bool Ret = ObjectDataClass::Init(Global::Ini()->DataPath.c_str(), Global::Ini()->Datalanguage.c_str(), Global::Ini()->Stringlanguage.c_str());

	if (Ret == false)
	{
		Message(OwnerObj->Role(), ObjectDataClass::_ErrorStr.c_str());
		Message(OwnerObj->Role(), "	Reload Object Failed");
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	//清除所有物件的Buf
	vector< BaseItemObject* >& ZoneList = *BaseItemObject::GetZoneObjList();

	for (int i = 0; i < (int)ZoneList.size(); i++)
	{
		BaseItemObject* Obj = ZoneList[i];
		if (Obj == NULL)
			continue;

		Obj->Role()->ClearAllBuff();
	}
	//////////////////////////////////////////////////////////////////////////

	Global::St()->ComboProc.Release();
	Global::St()->ComboProc.Init();
	//OwnerObj->Role()->Msg( "Reload Object OK" );
	Message(OwnerObj->Role(), "Reload Object OK");


	for (int i = 0; i < (int)ZoneList.size(); i++)
	{
		BaseItemObject* Obj = ZoneList[i];
		if (Obj == NULL)
			continue;

		Obj->Role()->TempData.UpdateInfo.ReSetTalbe = true;
		Obj->Role()->TempData.UpdateInfo.Recalculate_All = true;
	}

	//OwnerObj->Role()->Msg( "Recalculate Object" );
	Message(OwnerObj->Role(), "Recalculate Object");
	return true;
}
//--------------------------------------------------------------------------------------
bool ComObjFindPlayer(BaseItemObject* Obj, void* _Name)
{
	if (!Obj->Role()->IsPlayer())
		return false;

	if (stricmp(Obj->Role()->BaseData.RoleName.Begin(), (char*)_Name) == 0)
		return true;

	return false;
}
//--------------------------------------------------------------------------------------
bool    GmCommandClass::GM_DelBodyItem(BaseItemObject* OwnerObj)
{
	int OrgID;
	int Count;
	RoleDataEx* Target = OwnerObj->Role();
	BaseItemObject* TargetObj = Global::Search(ComObjFindPlayer, _GetStr(1));
	if (TargetObj == NULL)
	{
		OrgID = _GetNum(1);
		Count = _GetNum(2);
	}
	else
	{
		OrgID = _GetNum(2);
		Count = _GetNum(3);
	}

	if (Count == 0)
		Count = 1;

	Target->DelBodyItem(OrgID, Count, EM_ActionType_PlotDestory);
	OwnerObj->Role()->Msg("OK");
	return true;
}
//--------------------------------------------------------------------------------------
bool    GmCommandClass::GM_Give(BaseItemObject* OwnerObj)
{
	int		OrgID;
	int		Count;
	int		Inherent[6];

	GameObjDbStructEx* OrgDB;
	BaseItemObject* TargetObj = Global::Search(ComObjFindPlayer, _GetStr(1));
	RoleDataEx* Target;
	RoleDataEx* Owner = OwnerObj->Role();


	if (TargetObj == NULL)
	{
		Target = Owner;
		OrgID = _GetNum(1);
		Count = _GetNum(2);

		for (int i = 0; i < 6; i++)
		{
			Inherent[i] = _GetNum(3 + i);
			OrgDB = Global::GetObjDB(Inherent[i]);
			if (!OrgDB->IsAttribute())
				Inherent[i] = 0;
		}
	}
	else
	{
		Target = (RoleDataEx*)TargetObj->Role();
		OrgID = _GetNum(2);
		Count = _GetNum(3);

		for (int i = 0; i < 6; i++)
		{
			Inherent[i] = _GetNum(3 + i);
			OrgDB = Global::GetObjDB(Inherent[i]);
			if (!OrgDB->IsAttribute())
				Inherent[i] = 0;
		}
	}

	OrgDB = Global::GetObjDB(OrgID);
	if (OrgDB == NULL)
	{
		Message(Owner, "Give Error");
		return true;
	}


	if (OrgDB->IsItem() == false)
	{
		bool Reset = Owner->GiveItem(OrgID, 1, EM_ActionType_GMGive, NULL, "");

		if (Reset != false)
			Message(Owner, "Give OK");
		else
			Message(Owner, "Give Error");

		return true;
	}

	ItemFieldStruct	Item;
	if (Global::NewItemInit(Item, OrgID, 0) == false)
	{
		//Owner->Msg("give Error");
		Message(Owner, "Give Error");
		return true;
	}

	if (Count == -1)
		Item.RuneVolume = 4;

	if (Count <= 0)
		Count = 1;
	Item.Count = Count;
	Item.OrgObjID = OrgID;



	if (OrgDB->MaxHeap == 1)
	{
		if (OrgDB->IsArmor() || OrgDB->IsWeapon())
		{
			Item.Count = 1;

			int Pos = 0;
			for (int i = 0; i < 6; i++)
			{
				if (Inherent[Pos] == 0)
					break;
				if (Item.Inherent(i) != 0)
					continue;
				Item.Inherent(i, Inherent[Pos]);
				Pos++;
			}
		}
	}

	if (Target->GiveItem(Item, EM_ActionType_GMGive, NULL, "") == false)
	{
		//Owner->Msg("give Error");
		Message(Owner, "Give Error");
	}
	else
	{
		Global::Log_DepartmentStore(Item, EM_ActionType_GMGive, Item.Count, 0, 0, 0, Target->DBID());
		//Owner->Msg("give OK");
		Message(Owner, "Give OK");
	}

	return true;
}
//--------------------------------------------------------------------------------------
//呼叫玩家
bool	GmCommandClass::GM_CallPlayerName(BaseItemObject* OwnerObj)
{
	OwnerObj->Role()->Msg("call player");
	char* PlayerName = _GetStr(1);
	NetSrv_Other::S_CallPlayer(*(OwnerObj->Role()->Pos()), PlayerName, OwnerObj->Role()->BaseData.RoomID);
	Message(OwnerObj->Role(), "OK");
	return true;
}
//--------------------------------------------------------------------------------------
bool	GmCommandClass::GM_GiveMoney(BaseItemObject* OwnerObj)
{
	int Money = _GetNum(1);

	OwnerObj->Role()->AddBodyMoney(Money, NULL, EM_ActionType_GMGive, true);
	Message(OwnerObj->Role(), "OK");
	return true;
}
//--------------------------------------------------------------------------------------
bool	GmCommandClass::GM_GiveMoney_Account(BaseItemObject* OwnerObj)
{
	if (Global::AccountMoneyOperable() == false)
	{
		Message(OwnerObj->Role(), "Failed!");
	}
	else
	{
		int Money = _GetNum(1);

		OwnerObj->Role()->AddBodyMoney_Account(Money, NULL, EM_ActionType_GMGive, true);
		Message(OwnerObj->Role(), "OK");
	}

	return true;
}
//--------------------------------------------------------------------------------------
bool	GmCommandClass::GM_GiveMoney_Bonus(BaseItemObject* OwnerObj)
{
	int Money = _GetNum(1);

	OwnerObj->Role()->AddMoney_Bonus(Money, NULL, EM_ActionType_GMGive);
	Message(OwnerObj->Role(), "OK");
	return true;
}
//--------------------------------------------------------------------------------------
//找某玩家
bool	GmCommandClass::GM_GotoPlayerName(BaseItemObject* OwnerObj)
{
	//OwnerObj->Role()->Msg( "傳送到玩家" );
	char* PlayerName = _GetStr(1);
	NetSrv_Other::S_RequestPlayerPos(OwnerObj->Role()->DBID(), PlayerName);
	Message(OwnerObj->Role(), "OK");
	return true;
}
//--------------------------------------------------------------------------------------
//設定暫時的基本數值( 只有玩家角色有效 )
bool	GmCommandClass::GM_SetValue(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int		ItemID;
	char* ValueName;
	int 	Value;

	if (_Val.size() == 4)
	{
		ItemID = _GetNum(1);
		ValueName = _GetStr(2);
		Value = _GetNum(3);
	}
	else
	{
		ItemID = Owner->TempData.Sys.GUID;
		ValueName = _GetStr(1);
		Value = _GetNum(2);
	}

	RoleDataEx* Item = Global::GetRoleDataByGUID(ItemID);

	if (Item == NULL)
	{
		//Owner->Msg( "item id not find!!" );
		Message(Owner, "item id not find!!");
		return true;
	}


	if (stricmp(ValueName, "EXP") == 0)
	{
		Item->SetValue(EM_RoleValue_EXP, Value, NULL);
	}
	else if (stricmp(ValueName, "TpExp") == 0)
	{
		Item->SetValue(EM_RoleValue_TpExp, Value, NULL);
	}
	else if (stricmp(ValueName, "LV") == 0)
	{
		Item->SetValue(EM_RoleValue_LV, Value, NULL);
		//		NetSrv_RoleValueChild::NotifyLvUp( Item );
	}
	else if (stricmp(ValueName, "STR") == 0)
	{
		Item->SetValue(EM_RoleValue_STR, Value, NULL);
	}
	else if (stricmp(ValueName, "STA") == 0)
	{
		Item->SetValue(EM_RoleValue_STA, Value, NULL);
	}
	else if (stricmp(ValueName, "INT") == 0)
	{
		Item->SetValue(EM_RoleValue_INT, Value, NULL);
	}
	else if (stricmp(ValueName, "AGI") == 0)
	{
		Item->SetValue(EM_RoleValue_AGI, Value, NULL);
	}
	else if (stricmp(ValueName, "MND") == 0)
	{
		Item->SetValue(EM_RoleValue_MND, Value, NULL);
	}
	else if (stricmp(ValueName, "Point") == 0)
	{
		Item->SetValue(EM_RoleValue_Point, Value, NULL);
	}
	//	else if( stricmp( ValueName , "SkillPoint" ) == 0 )
	//	{
	//		Item->SetValue( EM_RoleValue_SkillPoint , Value);
	//	}
	else if (stricmp(ValueName, "Job") == 0)
	{
		Item->SetValue(EM_RoleValue_VOC, Value, NULL);
	}
	else if (stricmp(ValueName, "Race") == 0)
	{
		Sex_ENUM Sex = Item->BaseData.Sex;
		Race_ENUM Race = (Race_ENUM)Value;

		Item->BaseData.ItemInfo.OrgObjID = 1000 + Sex + Race * 2;

		Item->SetValue(EM_RoleValue_RACE, Value, NULL);
		int RoomID = Item->RoomID();
		Global::DelFromPartition(Item->GUID());
		Global::AddToPartition(Item->GUID(), RoomID);
	}
	else if (stricmp(ValueName, "SubJob") == 0)
	{
		Item->SetValue(EM_RoleValue_VOC_SUB, Value, NULL);
	}
	else if (stricmp(ValueName, "BodyCount") == 0)
	{
		Item->SetValue(EM_RoleValue_BodyCount, Value, NULL);
	}
	else if (stricmp(ValueName, "BankCount") == 0)
	{
		Item->SetValue(EM_RoleValue_BankCount, Value, NULL);
	}
	else if (stricmp(ValueName, "honor") == 0)
	{
		Item->SetValue(EM_RoleValue_Honor, Value, NULL);
	}
	else if (stricmp(ValueName, "Sex") == 0)
	{
		Sex_ENUM Sex = (Sex_ENUM)Value;
		Race_ENUM Race = Item->BaseData.Race;

		Item->BaseData.ItemInfo.OrgObjID = 1000 + Sex + Race * 2;

		Item->SetValue(EM_RoleValue_SEX, Value, NULL);
		int RoomID = Item->RoomID();
		Global::DelFromPartition(Item->GUID());
		Global::AddToPartition(Item->GUID(), RoomID);
	}
	else if (stricmp(ValueName, "LockPartition") == 0)
	{
		Item->TempData.Sys.PartitionLock = (Value != 0);
	}
	else if (stricmp(ValueName, "VanderCount") == 0)
	{
		Item->PlayerBaseData->Defendant.MoneyTraderCount = (Value != 0);
	}
	else
	{
		//Owner->Msg("ValueName not find!! \n /SetValue itemid STR 100");
		Message(Owner, "ValueName not find!! \n /SetValue itemid STR 100");
		return true;
	}
	Message(Owner, "OK");
	return true;
}
//--------------------------------------------------------------------------------------
bool	GmCommandClass::GM_SetValueID(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int		ItemID;
	//char*	ValueName	;
	int		ValueID;
	int 	Value;

	if (_Val.size() == 4)
	{
		ItemID = _GetNum(1);
		ValueID = _GetNum(2);
		Value = _GetNum(3);
	}
	else
	{
		ItemID = Owner->TempData.Sys.GUID;
		ValueID = _GetNum(1);
		Value = _GetNum(2);
	}

	RoleDataEx* Item = Global::GetRoleDataByGUID(ItemID);

	if (Item == NULL)
	{
		//Owner->Msg( "item id not find!!" );
		Message(Owner, "item id not find!!");
		return true;
	}

	Item->SetValue((RoleValueName_ENUM)ValueID, Value, NULL);

	char szMsg[256];

	sprintf_s(szMsg, sizeof(szMsg), "Set ValueID[ %d ] = %d", ValueID, Value);
	//Owner->Msg( szMsg );
	Message(Owner, szMsg);


	return true;
}
//--------------------------------------------------------------------------------------
bool GmCommandClass::ResetDailyQuest(BaseItemObject* OwnerObj)
{
	// 對已連線的每個角色發出重置封包, 並將 Role 資料的每日任務設定關畢
	for (int i = 0; i < BaseItemObject::GetmaxZoneID(); i++)
	{
		BaseItemObject* pObj = BaseItemObject::GetObj_ByLocalID(i);
		if (pObj == NULL)
			continue;
		else
		{
			RoleDataEx* pRole = pObj->Role();

			// 重置每日任務
			CNetSrv_Script_Child::ResetDailyQuest(pRole);

			// 重置每日旗標
			CNetSrv_Script_Child::ResetDailyFlag(pRole, true);
		}
	}

	Message(OwnerObj->Role(), "OK");
	return true;
}
//--------------------------------------------------------------------------------------
bool    GmCommandClass::GM_DelObj(BaseItemObject* OwnerObj)
{
	char Buf[256];
	BaseItemObject* TargetObj = BaseItemObject::GetObj(_GetNum(1));
	RoleDataEx* Owner = OwnerObj->Role();
	if (TargetObj == NULL || TargetObj->Role()->IsMirror())
	{
		//Owner->Msg( "找不到此物件" );
		Message(Owner, "Obj Not Find");
		return false;
	}

	if (Global::Net_ServerList->CheckServerExist(EM_SERVER_TYPE_DATACENTER) == false)
	{
		Owner->Msg("DataCenter 沒開啟，資料儲存會有問題");
	}


	sprintf_s(Buf, sizeof(Buf), "Delete %s(%d) ", Global::GetRoleName_ASCII(TargetObj->Role()).c_str(), TargetObj->GUID());
	//Owner->Msg( Buf );
	Message(Owner, Buf);

	TargetObj->Destroy(OwnerObj->Role()->Account_ID());
	//TargetObj->Destroy( "GM DEL OBJ" );
	return true;
}

//--------------------------------------------------------------------------------------
//設定永久的基本數值
bool	GmCommandClass::GM_SpellMagic(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	int		TargetID = _GetNum(1);
	long	MagicID = _GetNum(2);

	BaseItemObject* TargetObj = BaseItemObject::GetObj(TargetID);
	float X, Y, Z;

	if (TargetObj == NULL)
	{
		X = Owner->Pos()->X;
		Y = Owner->Pos()->Y;
		Z = Owner->Pos()->Z;
	}
	else
	{
		RoleDataEx* Target = TargetObj->Role();
		X = Target->Pos()->X;
		Y = Target->Pos()->Y;
		Z = Target->Pos()->Z;
	}

	MagicProcessClass::SpellMagic(Owner->GUID(), TargetID, X, Y, Z, MagicID, 0);

	Message(Owner, "OK");
	return true;
}
//--------------------------------------------------------------------------------------
bool    GmCommandClass::GM_ModifyObj(BaseItemObject* OwnerObj)
{
	int		TargetID = _GetNum(1);
	BaseItemObject* TargetObj = BaseItemObject::GetObj(_GetNum(1));
	RoleDataEx* Owner = OwnerObj->Role();
	if (TargetObj == NULL || TargetObj->Role()->IsMirror())
	{
		//Owner->Msg( "object not find" );
		Message(Owner, "Object Not Find");
		return false;
	}
	RoleDataEx* Target = TargetObj->Role();

	if (Target->IsPlayer() || Target->IsMirror())
	{
		//Owner->Msg( "This object can't modify" );
		Message(Owner, "This object can't modify");
		return false;
	}

	NetSrv_CliConnect::SendModifyInfo(Owner->GUID(), Target);
	Message(Owner, "OK");

	return true;
}
//--------------------------------------------------------------------------------------
bool GmCommandClass::GM_QuestFlag(BaseItemObject* OwnerObj)
{
	char* FlagID = _GetStr(1);
	int			FlagVal = _GetNum(2);

	int			iFlagID = 0;
	RoleDataEx* pRole = OwnerObj->Role();

	if (strcmp(FlagID, "all") == 0)
	{
		iFlagID = -1;
		pRole->PlayerBaseData->Quest.QuestHistory.ReSet();
	}
	else
		if (strcmp(FlagID, "show") == 0)
		{
			if (FlagVal >= Def_ObjectClass_QuestDetail && FlagVal < (Def_ObjectClass_QuestDetail + Def_ObjectClass_QuestDetail_Total))
			{
				char szBuff[256];
				FlagVal = FlagVal - Def_ObjectClass_QuestDetail;
				if (pRole->PlayerBaseData->Quest.QuestHistory.GetFlag(FlagVal))
					sprintf_s(szBuff, sizeof(szBuff), "Quest %d On", (FlagVal + Def_ObjectClass_QuestDetail));

				else
					sprintf_s(szBuff, sizeof(szBuff), "Quest %d Off", (FlagVal + Def_ObjectClass_QuestDetail));

				pRole->Msg(szBuff);

			}
		}
		else
		{
			iFlagID = atoi(FlagID);
			if (iFlagID >= Def_ObjectClass_QuestDetail && iFlagID < (Def_ObjectClass_QuestDetail + Def_ObjectClass_QuestDetail_Total))
			{
				iFlagID = iFlagID - Def_ObjectClass_QuestDetail;
				if (FlagVal == 1)
					pRole->PlayerBaseData->Quest.QuestHistory.SetFlagOn(iFlagID);
				else
					pRole->PlayerBaseData->Quest.QuestHistory.SetFlagOff(iFlagID);
			}
		}

	CNetSrv_Script::SetQuestFlag(OwnerObj, iFlagID, FlagVal);

	Message(pRole, "OK");
	return true;
}
//清除所有 Quest 資料
bool    GmCommandClass::GM_ClsQuest(BaseItemObject* OwnerObj)
{
	RoleDataEx* Role = OwnerObj->Role();
	memset(&Role->PlayerBaseData->Quest, 0, sizeof(Role->PlayerBaseData->Quest));
	memset(&Role->BaseData.KeyItem, 0, sizeof(Role->BaseData.KeyItem));
	//Role->Msg( "Cls Quest info" );
	Message(Role, "Cls Quest info");
	return true;
}
//--------------------------------------------------------------------------------------
bool    GmCommandClass::GM_CreateFlag(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int FlagObj = _GetNum(1);
	int FlagID = _GetNum(2);

	if (FlagObj < Def_ObjectClass_Flag)
	{
		FlagObj += Def_ObjectClass_Flag;
	}

	if (_Count() <= 2)
	{
		FlagID = -1;
	}

	bool Ret = Global::CreateFlag(FlagObj, FlagID, Owner->Pos()->X, Owner->Pos()->Y, Owner->Pos()->Z, Owner->Pos()->Dir, -1, true);

	if (Ret == false)
	{
		//Owner->Msg( "False" );
		Message(Owner, "False");
	}
	else
	{
		//Owner->Msg( "OK" );
		Message(Owner, "OK");
	}
	return true;
}
//--------------------------------------------------------------------------------------
bool    GmCommandClass::GM_HideFlag(BaseItemObject* OwnerObj)
{
	int FlagObjID = _GetNum(1);
	if (_Count() <= 1)
		FlagObjID = -1;

	if (FlagObjID < Def_ObjectClass_Flag && FlagObjID != -1)
		FlagObjID += Def_ObjectClass_Flag;

	RoleDataEx* Owner = OwnerObj->Role();
	//Owner->Msg( "OK" );
	Message(Owner, "OK");

	Global::HideFlag(FlagObjID);
	return true;
}
//--------------------------------------------------------------------------------------
bool    GmCommandClass::GM_ShowFlag(BaseItemObject* OwnerObj)
{
	int FlagObjID = _GetNum(1);
	if (_Count() <= 1)
		FlagObjID = -1;

	if (FlagObjID < Def_ObjectClass_Flag && FlagObjID != -1)
		FlagObjID += Def_ObjectClass_Flag;

	RoleDataEx* Owner = OwnerObj->Role();
	//Owner->Msg( "OK" );
	Message(Owner, "OK");

	Global::ShowFlag(FlagObjID);
	return true;
}
//--------------------------------------------------------------------------------------
//找此區的某物件
bool   GmCommandClass::GM_GotoID(BaseItemObject* OwnerObj)
{
	int TargetID = _GetNum(1);

	RoleDataEx* Owner = OwnerObj->Role();
	BaseItemObject* TargetObj = BaseItemObject::GetObj(TargetID);

	if (TargetObj == NULL)
	{
		//Owner->Msg( "Target not find" );
		Message(Owner, "Target not find");
		return false;
	}

	float X = TargetObj->Role()->Pos()->X;
	float Y = TargetObj->Role()->Pos()->Y;
	float Z = TargetObj->Role()->Pos()->Z;
	float Dir = TargetObj->Role()->Pos()->Dir;
	int	  RoomID = TargetObj->Role()->RoomID();

	Global::ChangeZone(Owner->GUID(), RoleDataEx::G_ZoneID, RoomID, X, Y, Z, Dir);
	//Owner->Pos( X , Y , Z , Dir );

	//NetSrv_MoveChild::SetObjPos( OwnerObj , X , Y , Z , Dir );
//	Global::CheckClientLoading_SetPos( OwnerObj->GUID() , X , Y , Z , Dir );
	Message(Owner, "OK");
	return true;
}
//--------------------------------------------------------------------------------------
bool    GmCommandClass::GM_SearchObjCount(BaseItemObject* OwnerObj)
{
	map< int, int > CountMap;
	int Type = _GetNum(1);
	RoleDataEx* Owner = OwnerObj->Role();
	char    Buf[256];
	vector< BaseItemObject* >& ZoneList = *BaseItemObject::GetZoneObjList();
	int NpcCount = 0;

	for (int i = 0; i < (int)ZoneList.size(); i++)
	{
		BaseItemObject* Obj = ZoneList[i];
		if (Obj == NULL)
			continue;
		RoleDataEx* Other = Obj->Role();
		if (Other == NULL)
			continue;

		if (Type == 0 && Owner->RoomID() != Other->RoomID())
			continue;

		CountMap[Other->BaseData.ItemInfo.OrgObjID]++;
		NpcCount++;
	}

	vector< BaseSortStruct > SortList;
	for (map< int, int >::iterator iter = CountMap.begin(); iter != CountMap.end(); iter++)
	{
		BaseSortStruct Info;
		Info.Value = iter->second;
		Info.Data_Int = iter->first;
		SortList.push_back(Info);
		//		SortMap[iter->second] = iter->first;
	}
	sort(SortList.begin(), SortList.end());

	Message(Owner, "------------------------------------------");
	//for( multimap< int , int  >::iterator iter = SortMap.begin() ; iter != SortMap.end() ; iter++ )
	for (unsigned i = 0; i < SortList.size(); i++)
	{
		BaseSortStruct& Info = SortList[i];
		GameObjDbStructEx* ObjDB = Global::GetObjDB(Info.Data_Int);
		if (ObjDB == NULL)
			continue;
		sprintf_s(Buf, sizeof(Buf), "[%d] = %d  (%s)", Info.Data_Int, Info.Value, Utf8ToChar(ObjDB->Name).c_str());
		Owner->Msg(Buf);
	}
	sprintf_s(Buf, sizeof(Buf), "----- total npcType = %d NpcCount=%d ------", SortList.size(), NpcCount);
	Owner->Msg(Buf);
	return true;
}
//搜尋物件
bool    GmCommandClass::GM_SearchObj(BaseItemObject* OwnerObj)
{
	int     Type = _GetNum(1);
	char* Name = _GetStr(2);
	RoleDataEx* Owner = OwnerObj->Role();
	char    Buf[256];

	vector< BaseItemObject* >& ZoneList = *BaseItemObject::GetZoneObjList();

	for (int i = 0; i < (int)ZoneList.size(); i++)
	{
		BaseItemObject* Obj = ZoneList[i];

		if (Obj == NULL)
			continue;
		if (Name[0] != 0 && strstr(Obj->Role()->RoleName(), Name) == NULL)
			continue;

		int iObjRoomID = Obj->Role()->BaseData.RoomID;
		if (Owner->BaseData.RoomID != iObjRoomID)
			continue;

		switch (Type)
		{
		case 0:
			break;
		case 1:
			if (!Obj->Role()->IsPlayer())
				continue;
			break;
		case 2:
			if (!Obj->Role()->IsNPC())
				continue;
			break;
		}
		//sprintf( Buf , "%s 0x%x" , Obj->Role()->RoleName() , Obj->Role()->GUID() );
		sprintf_s(Buf, sizeof(Buf), "%s 0x%x", Global::GetRoleName_ASCII(Obj->Role()).c_str(), Obj->Role()->GUID());
		Owner->Msg(Buf);
	}
	Message(Owner, "OK");
	return true;
}

//設定可不可以攻擊
bool    GmCommandClass::GM_AttackFlag(BaseItemObject* OwnerObj)
{
	int     ObjID = _GetNum(1);
	RoleDataEx* Owner = OwnerObj->Role();
	Owner->BaseData.Mode.Fight = !Owner->BaseData.Mode.Fight;
	if (Owner->BaseData.Mode.Fight)
	{
		//Owner->Msg( "Attack flag On" );
		Message(Owner, "Attack flag On");
	}
	else
	{
		//Owner->Msg( "Attack flag off" );
		Message(Owner, "Attack flag off");
	}

	return true;
}
//讓所有NPC回到重生點
bool    GmCommandClass::GM_NPCGoHome(BaseItemObject* OwnerObj)
{
	vector< BaseItemObject* >& ZoneList = *BaseItemObject::GetZoneObjList();
	RoleDataEx* Owner = OwnerObj->Role();
	for (int i = 0; i < (int)ZoneList.size(); i++)
	{
		BaseItemObject* OtherObj = ZoneList[i];
		if (OtherObj == NULL)
			continue;

		if (OtherObj->Role()->IsPlayer())
			continue;

		float X = OtherObj->Role()->SelfData.RevPos.X;
		float Y = OtherObj->Role()->SelfData.RevPos.Y;
		float Z = OtherObj->Role()->SelfData.RevPos.Z;
		float Dir = OtherObj->Role()->SelfData.RevPos.Dir;

		//OtherObj->Role()->Pos( X , Y , Z , Dir );

		//NetSrv_MoveChild::SetObjPos( OtherObj , X , Y , Z , Dir );       
		Global::CheckClientLoading_SetPos(OtherObj->GUID(), X, Y, Z, Dir);

	}
	Message(Owner, "OK");
	return true;
}

//設定npc移動旗標
bool    GmCommandClass::GM_NPCMoveFlag(BaseItemObject* OwnerObj)
{
	Global::St()->IsNPCMove = !Global::St()->IsNPCMove;
	RoleDataEx* Owner = OwnerObj->Role();

	if (Global::St()->IsNPCMove != false)
	{
		//OwnerObj->Role()->Msg( "npc move flag on" );
		Message(Owner, "npc move flag on");
	}
	else
	{
		//OwnerObj->Role()->Msg( "npc move flag off" );
		Message(Owner, "npc move flag off");
	}
	return true;
}

//設定要取得lua錯誤資訊
bool    GmCommandClass::GM_GetLuaErrorInfo(BaseItemObject* OwnerObj)
{
	RoleDataEx* Role = OwnerObj->Role();
	if (Role->TempData.Sys.LuaDebugMsg != false)
	{
		Role->TempData.Sys.LuaDebugMsg = false;
		//Role->Msg( "not recive LUA debug message " );
		Message(Role, "not recive LUA debug message");
	}
	else
	{
		Role->TempData.Sys.LuaDebugMsg = true;
		//Role->Msg( "recive LUA debug message" );
		Message(Role, "recive LUA debug message");
	}
	return true;
}

//產生寵物
bool    GmCommandClass::GM_CreatePet(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int     			OrgObjID = _GetNum(1);
	int     			LV = _GetNum(2);
	char* Name = _GetStr(3);
	SummonPetTypeENUM	PetType = (SummonPetTypeENUM)_GetNum(4);

	int PetID = Global::CreatePet(OwnerObj->Role()->GUID(), OrgObjID, Name, LV, PetType);

	if (PetID == -1)
	{
		Message(Owner, "Failed");
		return false;
	}

	Message(Owner, "OK");
	return true;
}

//寵物命令
bool    GmCommandClass::GM_PetCmd(BaseItemObject* OwnerObj)
{
	char* Cmd = _GetStr(1);
	int		ObjID = _GetNum(2);
	int		PetType = _GetNum(3);

	RoleDataEx* Owner = OwnerObj->Role();
	PetStruct& PetInfo = Owner->TempData.SummonPet.Info[PetType];
	RoleDataEx* Pet = Global::GetRoleDataByGUID(PetInfo.PetID);
	if (Pet == NULL)
	{
		Owner->Msg("Pet not find");
		return false;
	}

	if (Pet->IsPet(Owner) == false)
	{
		Owner->Msg("Pet not find");
		return false;
	}

	if ((unsigned)PetType >= EM_SummonPetType_CultivatePet)
	{
		Owner->Msg("Pet Type error");
		return false;
	}


	if (stricmp(Cmd, "stop") == 0)
	{
		PetInfo.Action = EM_PET_STOP;
		PetInfo.TargetID = -1;
	}
	else if (stricmp(Cmd, "follow") == 0)
	{
		PetInfo.Action = EM_PET_FOLLOW;
		PetInfo.TargetID = -1;
	}
	else if (stricmp(Cmd, "attack") == 0)
	{
		PetInfo.Action = EM_PET_ATTACK;
		PetInfo.TargetID = ObjID;
	}
	else if (stricmp(Cmd, "attackoff") == 0)
	{
		PetInfo.Action = EM_PET_ATTACKOFF;
		PetInfo.TargetID = -1;
	}
	else if (stricmp(Cmd, "strickback") == 0)
	{
		PetInfo.Action = EM_PET_STRICKBACK;
		PetInfo.TargetID = -1;
	}
	else if (stricmp(Cmd, "strickbackoff") == 0)
	{
		PetInfo.Action = EM_PET_STRICKBACK_OFF;
		PetInfo.TargetID = -1;
	}
	else if (stricmp(Cmd, "guard") == 0)
	{
		PetInfo.Action = EM_PET_GUARD;
		PetInfo.TargetID = -1;
	}

	Message(Owner, "OK");
	return true;
}

//GM隱身
bool	GmCommandClass::GM_GMHide(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	RoleDataEx* Target;

	if (_Count() != 1)
	{
		int TargetID = _GetNum(1);
		BaseItemObject* TargetObj = Global::GetObj(TargetID);
		if (TargetObj == NULL)
		{
			//Owner->Msg( "Target not find" );
			Message(Owner, "Target not find");
			return false;
		}
		Target = TargetObj->Role();
	}
	else
	{
		Target = Owner;
	}

	Global::DelFromPartition(Target->GUID());

	Target->BaseData.Mode.GMHide = !Target->BaseData.Mode.GMHide;

	int		GMHideBuffID = g_ObjectData->GetSysKeyValue("GMHideBuff");

	if (Target->BaseData.Mode.GMHide == false)
	{
		Owner->ClearBuff(GMHideBuffID, -1);
		//	Owner->Msg("Show");
		Message(Owner, "GMShow");
	}
	else
	{
		//	Owner->Msg("GMHide");
		Owner->AssistMagic(Owner, GMHideBuffID, 0, false, -1);
		//NetSrv_MagicChild::SendRange_AddBuffInfo( Owner->GUID() , Owner->GUID() ,GMHideBuffID , 0 , -1 );
		Message(Owner, "GMHide");
	}

	Global::AddToPartition(Target->GUID(), Target->RoomID());

	return true;
}
//#####################################################################################################################
//IDS_STRING444 = SkillValue使用修改玩家技能，技能代碼請查表\n~SkillValue 技能編號 點數
bool	GmCommandClass::GM_SkillValue(BaseItemObject* OwnerObj)
{
	int ID = _GetNum(1);
	int Value = _GetNum(2);
	RoleDataEx* Owner = OwnerObj->Role();
	Owner->SetValue((RoleValueName_ENUM)(EM_RoleValue_Skill_Unarmed + ID), __min(Value, Owner->TempData.Attr.MaxSkillValue.GetSkill(ID)), NULL);
	Message(Owner, "OK");
	return true;
}
//IDS_STRING481 = SkillFull讓玩家的技能全滿\n~SkillFull 玩家名稱
bool	GmCommandClass::GM_SkillFull(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	for (int i = 0; i < _MAX_SKILLVALUE_TYPE; i++)
	{
		Owner->SetValue((RoleValueName_ENUM)(EM_RoleValue_Skill_Unarmed + i), Owner->TempData.Attr.MaxSkillValue.GetSkill(i), NULL);
	}

	Message(Owner, "OK");
	return true;
}

//CardFull讓玩家的技能全滿\n~CardFull 玩家名稱
bool	GmCommandClass::GM_CardFull(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	for (int i = 0; i < 5000; i++)
	{
		Owner->AddCard(Def_ObjectClass_Card + i);
	}

	Message(Owner, "OK");
	return true;
}

//IDS_STRING445 = Full可使玩家HP MP全滿\n~Full
bool	GmCommandClass::GM_Full(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	RoleDataEx* Target;

	if (_Count() != 1)
	{
		int TargetID = _GetNum(1);
		BaseItemObject* TargetObj = Global::GetObj(TargetID);
		if (TargetObj == NULL)
		{
			Message(Owner, "Target not find");
			return false;
		}
		Target = TargetObj->Role();
	}
	else
	{
		Target = Owner;
	}
	Target->SetValue(EM_RoleValue_HP, Target->TempData.Attr.Fin.MaxHP, NULL);
	Target->SetValue(EM_RoleValue_MP, Target->TempData.Attr.Fin.MaxMP, NULL);
	Target->SetValue(EM_RoleValue_SP, Target->TempData.Attr.Fin.MaxSP, NULL);
	Target->SetValue(EM_RoleValue_SP_Sub, Target->TempData.Attr.Fin.MaxSP_Sub, NULL);
	Message(Owner, "OK");
	return true;
}
//IDS_STRING453 = CallID(單區域)把某物件呼叫到面前\n~CallID 區域物件ID
bool	GmCommandClass::GM_CallID(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	int TargetID = _GetNum(1);
	BaseItemObject* TargetObj = Global::GetObj(TargetID);
	if (TargetObj == NULL)
	{
		Message(Owner, "Target not find");
		return false;
	}

	RoleDataEx* Target = TargetObj->Role();
	float X = Owner->Pos()->X;
	float Y = Owner->Pos()->Y;
	float Z = Owner->Pos()->Z;
	float Dir = Owner->Pos()->Dir;

	//TargetObj->Role()->Pos( X , Y , Z , Dir );
	//NetSrv_MoveChild::SetObjPos( TargetObj , X , Y , Z , Dir );       
	Global::CheckClientLoading_SetPos(TargetObj->GUID(), X, Y, Z, Dir);

	Message(Owner, "OK");
	return true;
}
//IDS_STRING454 = CallName(單區域)把某物件呼叫到面前\n~CallName 區域物件名稱
bool	GmCommandClass::GM_CallName(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	char* TargetName = _GetStr(1);
	BaseItemObject* TargetObj = Global::GetObjByName(TargetName);
	if (TargetObj == NULL)
	{
		Message(Owner, "Target not find");
		return false;
	}

	RoleDataEx* Target = TargetObj->Role();
	float X = Owner->Pos()->X;
	float Y = Owner->Pos()->Y;
	float Z = Owner->Pos()->Z;
	float Dir = Owner->Pos()->Dir;

	//TargetObj->Role()->Pos( X , Y , Z , Dir );
	//NetSrv_MoveChild::SetObjPos( TargetObj , X , Y , Z , Dir );       
	Global::CheckClientLoading_SetPos(TargetObj->GUID(), X, Y, Z, Dir);

	Message(Owner, "OK");
	return true;
}
//IDS_STRING456 = GotoName(單區域)傳送到某物件\n~GotoName 區域物件名稱
bool	GmCommandClass::GM_GotoName(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	char* TargetName = _GetStr(1);
	BaseItemObject* TargetObj = Global::GetObjByName(TargetName);
	if (TargetObj == NULL)
	{
		Message(Owner, "Target not find");
		return false;
	}


	float X = TargetObj->Role()->Pos()->X;
	float Y = TargetObj->Role()->Pos()->Y;
	float Z = TargetObj->Role()->Pos()->Z;
	float Dir = TargetObj->Role()->Pos()->Dir;
	int	  RoomID = TargetObj->Role()->RoomID();

	Global::ChangeZone(Owner->GUID(), RoleDataEx::G_ZoneID, RoomID, X, Y, Z, Dir);

	Message(Owner, "OK");
	return true;
}
//IDS_STRING457 = InfoID查詢某物件資訊\n~InfoID 區域物件ID 查詢形式\n例如:~InfoID 1 1\n形式代碼\n  0 名稱職業\n  1 攻擊防禦力\n  2 詳細基本數值\n  3 位置\n  4 體力耐敏等數值\n  5 等級血量經驗值\n  6 各項基本技能\n  7 需計算出來的各項數值)
bool	GmCommandClass::GM_InfoID(BaseItemObject* OwnerObj)
{
	return true;
}
//IDS_STRING462 = ResetID把身上的所有法術狀態消除\n~ResetID 區域物件ID
bool	GmCommandClass::GM_ResetMagic(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	RoleDataEx* Target;

	if (_Count() != 1)
	{
		int TargetID = _GetNum(1);
		BaseItemObject* TargetObj = Global::GetObj(TargetID);
		if (TargetObj == NULL)
		{
			Message(Owner, "Target not find");
			return false;
		}
		Target = TargetObj->Role();
	}
	else
	{
		Target = Owner;
	}

	Target->ClearAllBuff();
	Message(Owner, "OK");
	//設定PK Buff
	if (Global::Ini()->DisableGoodEvil == false)
	{
		GoodEvilTypeENUM GoodEvilType = Owner->GetGoodEvilType();
		BuffBaseStruct* RetBuff = Owner->AssistMagic(Owner, g_ObjectData->SysValue().GoodEvilBuf[GoodEvilType - EM_GoodEvil_Demon], 0, false, -1);
		if (RetBuff != NULL)
		{
			NetSrv_MagicChild::SendRange_AddBuffInfo(Owner->GUID(), Owner->GUID(), g_ObjectData->SysValue().GoodEvilBuf[GoodEvilType - EM_GoodEvil_Demon], 0, -1);
		}
	}

	return true;
}
//IDS_STRING466 = Rename修改某一個角色的名字\n~Rename 原來角色名稱 後來角色名稱
bool	GmCommandClass::GM_Rename(BaseItemObject* OwnerObj)
{
	return true;
}
//IDS_STRING468 = Kill瞬間殺死玩家或NPC\n~Kill 名稱
bool	GmCommandClass::GM_KillID(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	RoleDataEx* Target;
	//	int TargetID = _GetNum( 1 );
	int TargetID = 0;

	if (_Count() != 1)
	{
		TargetID = _GetNum(1);
		BaseItemObject* TargetObj = Global::GetObj(TargetID);
		if (TargetObj == NULL)
		{
			Message(Owner, "Target not find");
			return false;
		}
		Target = TargetObj->Role();
	}
	else
	{
		Target = Owner;
	}


	if (TargetID == -1)
	{
		BaseItemObject* OtherObj;
		set<BaseItemObject* >& NpcObjSet = *(BaseItemObject::NPCObjSet());
		set< BaseItemObject*>::iterator   NpcObjIter;
		for (NpcObjIter = NpcObjSet.begin(); NpcObjIter != NpcObjSet.end(); NpcObjIter++)
		{
			OtherObj = *NpcObjIter;
			if (OtherObj == NULL)
				continue;

			RoleDataEx* Other = OtherObj->Role();
			if (Other->IsNPC() && Other->BaseData.Mode.Fight != false && Other->IsDead() == false)
			{
				//Other->AddHP( Other , -100000000 );
				Other->AddHP(Other, Other->MaxHP() * -1 - 1000);
			}
		}
		Message(Owner, "OK");
		return true;
	}
	else
	{
		/*		BaseItemObject* TargetObj = Global::GetObj( TargetID );
				if( TargetObj == NULL )
				{
					Message( Owner , "Target not find" );
					return false;
				}
				Target = TargetObj->Role();
				*/

		if (Target->IsPlayer() || Target->IsNPC())
		{
			//Target->AddHP( Owner , -100000000 );
			Target->AddHP(Owner, Target->MaxHP() * -1 - 1000);
			Message(Owner, "OK");
		}
		else
		{

			Message(Owner, "Target Error");
		}

		return true;
	}

}
//IDS_STRING469 = Kick把某玩家踢出遊戲\n~Kick 玩家名稱
bool	GmCommandClass::GM_KickID(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	RoleDataEx* Target;

	if (_Count() != 1)
	{
		int TargetID = _GetNum(1);
		BaseItemObject* TargetObj = Global::GetObj(TargetID);
		if (TargetObj == NULL)
		{
			Message(Owner, "Target not find");
			return false;
		}
		Target = TargetObj->Role();
	}
	else
	{
		Target = Owner;
	}

	//int TargetID = _GetNum( 1 );
//	BaseItemObject* TargetObj = Global::GetObj( TargetID );
	if (Target->IsPlayer() == false)
	{
		//Owner->Msg( "Target not find" );
		if (OwnerObj->Role()->IsPlayer() != false)
			OwnerObj->Destroy("GM KICK ROLE");
		return false;
	}

	Target->Destroy("GM KICK ROLE");
	Message(Owner, "OK");
	return true;
}

bool	GmCommandClass::GM_KickBanID(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	RoleDataEx* Target;

	if (_Count() != 1)
	{
		int TargetID = _GetNum(1);
		BaseItemObject* TargetObj = Global::GetObj(TargetID);
		if (TargetObj == NULL)
		{
			Message(Owner, "Target not find");
			return false;
		}
		Target = TargetObj->Role();
	}
	else
	{
		Target = Owner;
	}

	//int TargetID = _GetNum( 1 );
	//	BaseItemObject* TargetObj = Global::GetObj( TargetID );
	if (Target->IsPlayer() == false)
	{
		/*
		//Owner->Msg( "Target not find" );
		if( OwnerObj->Role()->IsPlayer() != false )
		{
			OwnerObj->Destroy( "GM KICK Ban ROLE" );
			Global::Net()->FreezeAccount( Owner->Account_ID() , 2 );
		}
		*/
		return false;
	}

	Target->Destroy("GM KICK Ban ROLE");
	Global::Net()->FreezeAccount(Target->Account_ID(), 2);
	Message(Owner, "OK");
	return true;
}


bool	GmCommandClass::GM_KickAll(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	set<BaseItemObject* >& PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//計算每個房間的人數
	for (PlayerObjIter = PlayerObjSet.begin(); PlayerObjIter != PlayerObjSet.end(); PlayerObjIter++)
	{
		BaseItemObject* PlayerObj = *PlayerObjIter;
		if (PlayerObj == NULL)
			continue;

		//PlayerObj->Destroy( OwnerObj->Role()->Account_ID() );
		PlayerObj->Destroy("GM KILL ALL OBJ");
	}
	/*
	int TargetID = _GetNum( 1 );
	BaseItemObject* TargetObj = Global::GetObj( TargetID );
	if( TargetObj == NULL || TargetObj->Role()->IsPlayer() == false )
	{
		Owner->Msg( "目標不存在" );
		return false;
	}

	TargetObj->Destroy( OwnerObj->Role()->Account_ID() );
	*/
	Message(Owner, "OK");
	return true;
}

//BcastAll全區廣播
bool	GmCommandClass::GM_BcastAll(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	char* Content = _GetStr(1);

	if (strlen(Content) == 0)
		return false;

	NetSrv_Talk::SendRunningMsgEx_All(EM_RunningMsgExType_GM, Content);

	Message(Owner, "OK");
	return true;
}
//Bcast區廣播
bool	GmCommandClass::GM_Bcast(BaseItemObject* OwnerObj)
{

	RoleDataEx* Owner = OwnerObj->Role();
	char* Content = _GetStr(1);

	if (strlen(Content) == 0)
		return false;

	NetSrv_Talk::SendRunningMsgEx_Zone(EM_RunningMsgExType_GM, Content);
	Message(Owner, "OK");
	return true;
}

//IDS_STRING472 = DebugPlot劇情除錯，設定某一個物件跑的劇情步驟輸出給下指令者，最好同時設定目標與自己劇情常會在玩家與NPC身上轉換\n~DebugPlot 區域物件ID 劇情ID
bool	GmCommandClass::GM_DebugPlot(BaseItemObject* OwnerObj)
{
	return true;
}
//IDS_STRING482 = Faceoff變身成怪物\n小心身上的物品會複製一份掉出來\n變身後不能換區一定要登出\n~Faceoff 怪物物件ID 之後的名字
bool	GmCommandClass::GM_FaceOff(BaseItemObject* OwnerObj)
{
	return true;
}
//GMS.Help	= "系統時間設定\n~SystemTime 天數差 小時差 \n~SystemTime 1 1";
bool	GmCommandClass::GM_SystemTime(BaseItemObject* OwnerObj)
{
	return true;
}
//IDS_STRING489 = Visibl設定角色隱形開關，此種隱形玩家查不到\n~Hide on\n~Hide off
bool	GmCommandClass::GM_Visible(BaseItemObject* OwnerObj)
{
	return true;
}
//OrgObjList列表區域物件資料\n OrgObjList 類型(NPC QuestNPC Item Armor Weapon Magic Title Suit Recipe  ) 開始ID 結束ID\n例如: ObjList Weapon 0 100
bool	GmCommandClass::GM_OrgObjList(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	char* Type = _GetStr(1);
	int		BeginID = _GetNum(2);
	int		EndID = _GetNum(3);

	if (_Count() != 4)
		return false;


	if (stricmp(Type, "NPC") == 0)
	{
		BeginID += Def_ObjectClass_NPC;
		EndID += Def_ObjectClass_NPC;
	}
	else if (stricmp(Type, "QuestNPC") == 0)
	{
		BeginID += Def_ObjectClass_QuestNPC;
		EndID += Def_ObjectClass_QuestNPC;
	}
	else if (stricmp(Type, "Item") == 0)
	{
		BeginID += Def_ObjectClass_Item;
		EndID += Def_ObjectClass_Item;
	}
	else if (stricmp(Type, "Armor") == 0)
	{
		BeginID += Def_ObjectClass_Armor;
		EndID += Def_ObjectClass_Armor;
	}
	else if (stricmp(Type, "Weapon") == 0)
	{
		BeginID += Def_ObjectClass_Weapon;
		EndID += Def_ObjectClass_Weapon;
	}
	else if (stricmp(Type, "Magic") == 0)
	{
		BeginID += Def_ObjectClass_Magic;
		EndID += Def_ObjectClass_Magic;
	}
	else if (stricmp(Type, "Title") == 0)
	{
		BeginID += Def_ObjectClass_Title;
		EndID += Def_ObjectClass_Title;
	}
	else if (stricmp(Type, "Suit") == 0)
	{
		BeginID += Def_ObjectClass_Suit;
		EndID += Def_ObjectClass_Suit;
	}
	else if (stricmp(Type, "Recipe") == 0)
	{
		BeginID += Def_ObjectClass_Recipe;
		EndID += Def_ObjectClass_Recipe;
	}
	else
	{
		return false;
	}
	GameObjDbStructEx* OrgObj;
	char	Buf[256];
	for (int i = BeginID; i <= EndID; i++)
	{
		OrgObj = Global::GetObjDB(i);
		if (OrgObj == NULL)
			continue;
		sprintf_s(Buf, "%d %s", OrgObj->GUID, OrgObj->Name);
		Owner->Msg_Utf8(Buf);
	}
	Message(Owner, "OK");
	return true;
}

//IDS_STRING499 = SearchOrgObj尋找原始物件上面有某一個字串的命令，並設定其種類\n~SearchOrgObj 字串 類別\n字串 : (*) 代表全部，其它只要物件名稱類有函此字串就算\n類別 : 不填代表全部，其它可分(武器,防具,道具,法術,NPC,卡片)(人魔仙)(GM,劍,..)\n 例如 : ~SearchOrgObj 刀 武器魔刀
bool	GmCommandClass::GM_SearchOrgObj(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	char* Str = _GetStr(1);
	if (_Count() != 2)
		return false;

	GameObjDbStructEx* OrgObj;
	char	Buf[256];
	g_ObjectData->ObjectDB()->GetDataByOrder(true);

	int Count = 0;

	while (1)
	{
		OrgObj = g_ObjectData->ObjectDB()->GetDataByOrder();
		if (OrgObj == NULL)
			break;
		OrgObj = g_ObjectData->GetObj(OrgObj->GUID);
		if (strcmp(Str, "*") != 0)
		{
			if (OrgObj->Name == NULL)
				continue;
			if (strstr(OrgObj->Name, Str) == NULL)
				continue;
		}

		sprintf(Buf, "%d %s", OrgObj->GUID, OrgObj->Name);
		Owner->Msg_Utf8(Buf);
		Count++;
		if (Count > 100)
		{
			Owner->Msg("too many object.....");
			break;
		}
	}

	Message(Owner, "OK");

	return true;
}
//IDS_STRING501 = BindingItemMagic設定某一個物件使用的效果，(此物件原本需要可以使用)\n~BindingItemMagic 物品ID 法術ID
bool	GmCommandClass::GM_BindingItemMagic(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	int	 ItemID = _GetNum(1);
	int  MagicID = _GetNum(2);
	if (_Count() != 3)
		return false;

	GameObjDbStructEx* ItemDB = Global::GetObjDB(ItemID);
	GameObjDbStructEx* MagicDB = Global::GetObjDB(MagicID);

	if (ItemDB->IsItem() == false)
	{
		Owner->Msg("ItemID Error");
		return true;
	}
	if (ItemDB->IsMagicCollect() == false)
	{
		Owner->Msg("MagicID Error");
		return true;
	}

	ItemDB->Item.IncMagic_Onuse = MagicID;
	Message(Owner, "OK");
	return true;
}
//IDS_STRING504 = MaxOnline設定最高上線人數\n MaxOnline 最高上線人數
bool	GmCommandClass::GM_MaxOnline(BaseItemObject* OwnerObj)
{
	return true;
}
//IDS_STRING506 = GameTime設定遊戲偏移時間 \n GameTime 偏移時間(小時)
bool	GmCommandClass::GM_GameTime(BaseItemObject* OwnerObj)
{
	return true;
}
//IDS_STRING507 = TimeSpeed 加速遊戲 x1 x2..
bool	GmCommandClass::GM_TimeSpeed(BaseItemObject* OwnerObj)
{
	return true;
}
//IDS_STRING762 = CallSpell 設定此區域是否可以施呼叫同伴的法術\n例~CallSpell false
bool	GmCommandClass::GM_CallSpell(BaseItemObject* OwnerObj)
{
	return true;
}
//IDS_STRING770 = ResetColdown 把法術的Coldown 時間清除\n例~ResitColdown
bool	GmCommandClass::GM_ResetColdown(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	Owner->BaseData.Coldown.Init();
	NetSrv_Magic::S_ResetColdown(Owner->GUID(), EM_ResetColdownType_All);

	Message(Owner, "OK");
	return true;
}
//IDS_STRING770 = ResetColdown 把法術的Coldown 時間清除,但有設定的不能清除的不會清
bool	GmCommandClass::GM_NewResetColdown(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	for (int i = 0; i < DEF_MAX_COLDOWN_COUNT_JOB_; i++)
	{
		if (g_ObjectData->GetNotColdownJob(i))
			continue;
		Owner->BaseData.Coldown.Job[i] = 0;
	}

	for (int i = 0; i < DEF_MAX_COLDOWN_COUNT_EQ_; i++)
	{
		Owner->BaseData.Coldown.Eq[i] = 0;
	}

	for (int i = 0; i < DEF_MAX_COLDOWN_COUNT_ITEM_; i++)
	{
		Owner->BaseData.Coldown.Item[i] = 0;
	}
	NetSrv_Magic::S_ResetColdown(Owner->GUID(), EM_ResetColdownType_NewAll);

	Message(Owner, "OK");
	return true;
}
//IDS_STRING772 = SetFightID 設定群體戰的組別\n例~SetFightID 物件ID 組別 \n~SetFightID 100 10
bool	GmCommandClass::GM_SetFightID(BaseItemObject* OwnerObj)
{
	return true;
}
bool GmCommandClass::GM_Logout(BaseItemObject* OwnerObj)
{
	Global::CliDisconnect(OwnerObj->SockID());

	return true;
}
bool GmCommandClass::GM_Who(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	//char* NameSub= _GetStr( 1 );
	int ZoneID = _GetNum(1);

	map< int, OnlinePlayerInfoStruct* >::iterator Iter;
	map< int, OnlinePlayerInfoStruct* >* DBIDMap = AllOnlinePlayerInfoClass::This()->DBIDMap();
	/*
		for( Iter = DBIDMap->begin() ; Iter != DBIDMap->end() ; Iter++ )
		{
			char Buf[256];
			OnlinePlayerInfoStruct& Info = *(Iter->second);
			if( NameSub[0] != 0 && strstr( Info.Name.Begin() , NameSub ) == NULL  )
				continue;
			sprintf( Buf , "DBID = %d Name = %s" , Info.DBID , Info.Name.Begin() );
			Owner->Msg_Utf8( Buf );
		}
	*/
	BaseSortStruct SortBase;
	vector< BaseSortStruct >	SortList;

	for (Iter = DBIDMap->begin(); Iter != DBIDMap->end(); Iter++)
	{
		OnlinePlayerInfoStruct& Info = *(Iter->second);

		if (ZoneID != 0 && ZoneID != Info.ZoneID)
			continue;

		SortBase.Data = Iter->second;
		SortBase.Value = Info.ZoneID * 1000 + Info.LV;
		SortList.push_back(SortBase);
	}

	char Buf[256];
	sort(SortList.begin(), SortList.end());
	for (unsigned i = 0; i < SortList.size(); i++)
	{
		OnlinePlayerInfoStruct& Info = *(OnlinePlayerInfoStruct*)(SortList[i].Data);
		sprintf(Buf, "Name=%s DBID=%d ZoneID=%d Job=%d Lv=%d", Info.Name.Begin(), Info.DBID, Info.ZoneID, Info.Voc, Info.LV);
		Owner->Msg_Utf8(Buf);
	}
	sprintf(Buf, "------- total player=%d -----------", SortList.size());
	Message(Owner, Buf);
	return true;
}


bool    GmCommandClass::GM_ShowKeyItem(BaseItemObject* OwnerObj)
{
	char Buf[256];
	RoleDataEx* Owner = OwnerObj->Role();

	for (int i = 0; i < _MAX_FLAG_COUNT_; i++)
	{
		if (Owner->BaseData.KeyItem.GetFlag(i) != false)
		{
			int keyID = RoleDataEx::IDtoKeyItemObj(i);

			GameObjDbStructEx* ItemDB = Global::GetObjDB(keyID);


			if (ItemDB)
			{
				sprintf(Buf, "%d %s", keyID, ItemDB->Name);
				Owner->Msg_Utf8(Buf);
			}
		}
	}

	Message(Owner, "OK");
	return true;
}

bool    GmCommandClass::GM_DelKeyItem(BaseItemObject* OwnerObj)
{
	char Buf[256];
	int				iKeyItemGUID = _GetNum(1);
	int				iKeyItemID = RoleDataEx::KeyItemObjToID(iKeyItemGUID);
	RoleDataEx* Owner = OwnerObj->Role();

	//Owner->BaseData.KeyItem.SetFlagOff( iKeyItemID );
	Owner->DelKeyItem(iKeyItemGUID);

	sprintf(Buf, "KeyItem %d deleted.", iKeyItemGUID);

	Message(Owner, Buf);

	return true;

}

//測開啟AC
bool	GmCommandClass::GM_OpenAC(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	//DataCenter
	if (Owner->CheckOpenSomething())
		return true;

	NetSrv_AC::SDC_OpenAC(Owner->DBID(), Owner->GUID());
	Owner->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_AC;
	Owner->TempData.ShopInfo.TargetID = OwnerObj->Role()->GUID();

	Message(Owner, "OK");
	return true;
}

//測試開啟換角介面
bool	GmCommandClass::GM_ChangeJob(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	//DataCenter
	if (Owner->CheckOpenSomething())
		return true;

	NetSrv_RoleValue::S_OpenChangeJob(Owner->GUID(), Owner->GUID());
	Owner->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_ChangeJob;
	Owner->TempData.ShopInfo.TargetID = OwnerObj->Role()->GUID();

	Message(Owner, "OK");
	return true;
}

//刪除一系列的旗子
bool	GmCommandClass::GM_DelFlagList(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	int		FlagID = _GetNum(1);

	if (FlagID < Def_ObjectClass_Flag)
	{
		FlagID += Def_ObjectClass_Flag;
	}

	FlagPosClass::DelFlagList(FlagID, Owner->Account_ID());
	//OwnerObj->Role()->Msg( "OK" );

	Message(Owner, "OK");
	return true;
}


//npc 移動點以旗子顯示 
bool	GmCommandClass::GM_ShowNPCMoveFlag(BaseItemObject* OwnerObj)
{
	int				GUID = _GetNum(1);
	int				FlagID = _GetNum(2);

	if (FlagID < Def_ObjectClass_Flag)
	{
		FlagID += Def_ObjectClass_Flag;
	}
	RoleDataEx* Owner = OwnerObj->Role();
	BaseItemObject* OtherObj = Global::GetObj(GUID);
	if (OtherObj == NULL)
	{
		Message(Owner, "Failed[ ? ShowNPCMoveFlag GUID FlagID]");
		return true;
	}

	RoleDataEx* Other = OtherObj->Role();

	//刪除 這個號碼的旗子
	FlagPosClass::DelFlagList(FlagID, Owner->Account_ID());

	//取得移動旗子資料
	vector< NPC_MoveBaseStruct >& MoveList = *(OtherObj->NPCMoveInfo()->MoveInfo());

	//產生旗子
	for (int i = 0; i < (int)MoveList.size(); i++)
	{
		Global::CreateFlag(FlagID, i, MoveList[i].X, MoveList[i].Y, MoveList[i].Z, MoveList[i].Dir, -1, true);
	}

	Message(Owner, "OK");
	return true;
}

//測試儲存移動點
bool GmCommandClass::GM_TestSaveNPCMove(BaseItemObject* OwnerObj)
{
	int			GUID = _GetNum(1);
	int			FlagID = _GetNum(2);
	int			Range = _GetNum(3);
	int			MoveType = _GetNum(4);
	int			WaitTime_Base = _GetNum(5);
	int			WaitTime_Rand = _GetNum(6);
	int			DirMode = _GetNum(7);
	char* LuaScript = _GetStr(8);

	if (FlagID < Def_ObjectClass_Flag)
	{
		FlagID += Def_ObjectClass_Flag;
	}

	RoleDataEx* Owner = OwnerObj->Role();
	BaseItemObject* OtherObj = Global::GetObj(GUID);
	if (OtherObj == NULL)
	{
		Message(Owner, "Failed[ ? SaveNPCMove GUID FlagID]");
		return true;
	}

	RoleDataEx* Other = OtherObj->Role();

	vector < FlagPosInfo >* FlagList = FlagPosClass::GetFlagList(FlagID);

	if (FlagList == NULL || FlagList->size() == 0)
	{
		Message(Owner, "Failed Flag not find");
		return true;
	}

	if (Other->DBID() < 0)
	{
		Message(Owner, "NPC not save");
		return true;
	}

	vector< NPC_MoveBaseStruct > List;

	for (unsigned int i = 0; i < FlagList->size(); i++)
	{
		FlagPosInfo& Flag = (*FlagList)[i];
		if (Flag.IsEmpty())
			continue;

		NPC_MoveBaseStruct	Temp;

		Temp.X = Flag.X;
		Temp.Y = Flag.Y;
		Temp.Z = Flag.Z;
		Temp.Dir = Flag.Dir;
		Temp.Range = float(Range);
		Temp.WaitTime_Base = WaitTime_Base;
		Temp.WaitTime_Rand = WaitTime_Rand;
		Temp.ActionType = -1;
		Temp.MoveType = (NPC_MoveTypeENUM)MoveType;
		//Temp.sLuaFunc.Clear();
		Temp.sLuaFunc = LuaScript;

		if (DirMode == 0)
			Temp.DirMode = EM_NPC_DirMode_None;
		else
			if (DirMode == 1)
				Temp.DirMode = EM_NPC_DirMode_Change;

		List.push_back(Temp);
	}

	//--------------------------------------------------------------------------------------
	//拷貝舊資訊 與 設定新資訊
	vector< NPC_MoveBaseStruct >& MoveList = *(OtherObj->NPCMoveInfo()->MoveInfo());
	//如果只有一個點刪除此點
	if (MoveList.size() <= 1)
		MoveList.clear();



	for (int i = 0; i < (int)List.size(); i++)
	{
		if ((int)MoveList.size() > i)
		{
			MoveList[i].X = List[i].X;
			MoveList[i].Y = List[i].Y;
			MoveList[i].Z = List[i].Z;
		}
		else
		{
			MoveList.push_back(List[i]);
		}
	}

	while (MoveList.size() > List.size())
	{
		MoveList.pop_back();
	}
	//--------------------------------------------------------------------------------------
	Net_DCBase::SaveNPCMoveInfo(RoleDataEx::G_ZoneID, Other->DBID(), MoveList);
	Message(Owner, "OK");
	return true;
}

bool GmCommandClass::GM_SetPathByFlag(BaseItemObject* OwnerObj)
{
	int				GUID = _GetNum(1);
	int				FlagID = _GetNum(2);

	if (FlagID <= 10000)
	{
		FlagID += Def_ObjectClass_Flag;
	}

	RoleDataEx* Owner = OwnerObj->Role();
	BaseItemObject* OtherObj = Global::GetObj(GUID);
	if (OtherObj == NULL)
	{
		Message(Owner, "failed[ ? SaveNPCMove GUID FlagID]");
		return true;
	}

	RoleDataEx* Other = OtherObj->Role();

	vector < FlagPosInfo >* FlagList = FlagPosClass::GetFlagList(FlagID);

	if (FlagList == NULL || FlagList->size() == 0)
	{
		Message(Owner, "failed Flag not find");
		return true;
	}

	if (Other->DBID() < 0)
	{
		Message(Owner, "NPC not save");
		return true;
	}

	vector< NPC_MoveBaseStruct > List;

	for (unsigned int i = 0; i < FlagList->size(); i++)
	{
		FlagPosInfo& Flag = (*FlagList)[i];
		if (Flag.IsEmpty())
			continue;

		NPC_MoveBaseStruct	Temp;

		Temp.X = Flag.X;
		Temp.Y = Flag.Y;
		Temp.Z = Flag.Z;
		Temp.Dir = Flag.Dir;
		Temp.MoveType = EM_NPC_MoveType_Walk;
		Temp.Range = 0;
		Temp.WaitTime_Base = 0;
		Temp.WaitTime_Rand = 0;
		Temp.ActionType = 0;

		Temp.sLuaFunc.Clear();

		List.push_back(Temp);
	}

	vector< NPC_MoveBaseStruct >* MoveList = OtherObj->NPCMoveInfo()->MoveInfo();
	*MoveList = List;

	Net_DCBase::SaveNPCMoveInfo(RoleDataEx::G_ZoneID, Other->DBID(), List);

	Message(Owner, "OK");
	return true;
}


bool GmCommandClass::GM_GetPathInfo(BaseItemObject* OwnerObj)
{
	BaseItemObject* pObj = NULL;
	int				iObjID = 0;
	int				iPathID = -1;
	RoleDataEx* Owner = OwnerObj->Role();


	if ((int)_Val.size() == 2)
	{
		iObjID = _GetNum(1);
	}
	else
	{
		iObjID = _GetNum(1);
		iPathID = _GetNum(2);
	}

	pObj = Global::GetObj(iObjID);

	if (pObj == NULL || pObj->Role()->IsNPC() == false)
		return false;

	vector< NPC_MoveBaseStruct >* pNodelist = pObj->NPCMoveInfo()->MoveInfo();
	if (iPathID == -1)
	{
		char szMsg[256];
		sprintf(szMsg, "Obj[ %d ] PathNode Total Node: %d", iObjID, pNodelist->size());
		Owner->Msg(szMsg);
	}
	else
	{
		if (iPathID < 0 || iPathID >= (int)pNodelist->size())
			return false;

		NPC_MoveBaseStruct Path = (*pNodelist)[iPathID];

		char szMsg[256];
		sprintf(szMsg, "Obj[ %d ] PathNode %d:", iObjID, iPathID);
		Owner->Msg(szMsg);

		sprintf(szMsg, "X = %f", Path.X); Owner->Msg(szMsg);
		sprintf(szMsg, "Y = %f", Path.Y); Owner->Msg(szMsg);
		sprintf(szMsg, "Z = %f", Path.Z); Owner->Msg(szMsg);
		sprintf(szMsg, "Dir = %f", Path.Dir); Owner->Msg(szMsg);
		sprintf(szMsg, "WaitTime = %d", Path.WaitTime_Base); Owner->Msg(szMsg);
		sprintf(szMsg, "MoveType = %d", Path.ActionType); Owner->Msg(szMsg);
		sprintf(szMsg, "DirMode = %d", Path.DirMode); Owner->Msg(szMsg);

		if (Path.sLuaFunc.Size() != 0)
		{
			sprintf(szMsg, "Lua = %s", Path.sLuaFunc.Begin()); Owner->Msg(szMsg);
		}
	}

	Message(Owner, "OK");

	return true;
}

bool GmCommandClass::GM_SetPathInfo(BaseItemObject* OwnerObj)
{
	int		iObjID = _GetNum(1);
	int		iPathID = _GetNum(2);
	char* sType = _GetStr(3);
	int		iVal = _GetNum(4);

	/*
	float	X			= _GetFloat( 3 );
	float	Y			= _GetFloat( 4 );
	float	Z			= _GetFloat( 5 );
	float	Dir			= _GetFloat( 6 );
	int		WaitTime	= _GetNum( 7 );
	int		ActionType	= _GetNum( 8 );
	*/
	RoleDataEx* Owner = OwnerObj->Role();
	BaseItemObject* pObj = Global::GetObj(iObjID);

	if (pObj == NULL)
	{
		Message(Owner, "Obj Not Find");
		return false;
	}

	vector< NPC_MoveBaseStruct >* pNodelist = pObj->NPCMoveInfo()->MoveInfo();

	if ((iPathID < 0 && iPathID != -1) || iPathID >= (int)pNodelist->size())
		return false;

	if (iPathID == -1)
	{
		for (vector< NPC_MoveBaseStruct >::iterator it = pNodelist->begin(); it != pNodelist->end(); it++)
		{
			NPC_MoveBaseStruct				Path = *it;

			if (stricmp(sType, "MoveType") == 0)			Path.MoveType = (NPC_MoveTypeENUM)iVal;
			else if (stricmp(sType, "WaitBase") == 0)	Path.WaitTime_Base = iVal;
			else if (stricmp(sType, "WaitRand") == 0)	Path.WaitTime_Rand = iVal;
			else if (stricmp(sType, "DirMode") == 0)		Path.DirMode = (NPC_DirModeEnum)iVal;

			*it = Path;
		}
	}
	else
	{
		NPC_MoveBaseStruct				Path = (*pNodelist)[iPathID];

		if (stricmp(sType, "MoveType") == 0)			Path.MoveType = (NPC_MoveTypeENUM)iVal;
		else if (stricmp(sType, "WaitBase") == 0)	Path.WaitTime_Base = iVal;
		else if (stricmp(sType, "WaitRand") == 0)	Path.WaitTime_Rand = iVal;
		else if (stricmp(sType, "DirMode") == 0)		Path.DirMode = (NPC_DirModeEnum)iVal;

		(*pNodelist)[iPathID] = Path;
	}
	Message(Owner, "OK");
	return true;
}

bool GmCommandClass::GM_SetPathLua(BaseItemObject* OwnerObj)
{
	int		iObjID = _GetNum(1);
	int		iPathID = _GetNum(2);
	char* sLua = _GetStr(3);

	RoleDataEx* Owner = OwnerObj->Role();

	BaseItemObject* pObj = Global::GetObj(iObjID);

	if (pObj == NULL)
		return false;

	vector< NPC_MoveBaseStruct >* pNodelist = pObj->NPCMoveInfo()->MoveInfo();

	if ((iPathID < 0 && iPathID != -1) || iPathID >= (int)pNodelist->size())
		return false;


	if (iPathID == -1)
	{
		for (vector< NPC_MoveBaseStruct >::iterator it = pNodelist->begin(); it != pNodelist->end(); it++)
		{
			NPC_MoveBaseStruct				Path = *it;
			Path.sLuaFunc = sLua;

			*it = Path;
		}
	}
	else
	{
		NPC_MoveBaseStruct				Path = (*pNodelist)[iPathID];
		Path.sLuaFunc = sLua;
		(*pNodelist)[iPathID] = Path;
	}
	Message(Owner, "OK");
	return true;
}

bool GmCommandClass::GM_SetPathPos(BaseItemObject* OwnerObj)
{
	int				iObjID = _GetNum(1);
	int				iPathID = _GetNum(2);

	BaseItemObject* pObj = Global::GetObj(iObjID);
	RoleDataEx* Owner = OwnerObj->Role();

	if (pObj == NULL)
		return false;

	vector< NPC_MoveBaseStruct >* pNodelist = pObj->NPCMoveInfo()->MoveInfo();

	if (iPathID < 0 || iPathID >= (int)pNodelist->size())
		return false;

	NPC_MoveBaseStruct				Path = (*pNodelist)[iPathID];

	//Path.sLuaFunc = sLua;

	Path.X = Owner->Pos()->X;
	Path.Y = Owner->Pos()->Y;
	Path.Z = Owner->Pos()->Z;
	Path.Dir = Owner->Pos()->Dir;


	(*pNodelist)[iPathID] = Path;


	Message(Owner, "OK");
	return true;


}

bool GmCommandClass::GM_AddPathPos(BaseItemObject* OwnerObj)
{
	int				iObjID = _GetNum(1);
	int				iPathID = _GetNum(2);
	int				iSeq = _GetNum(3);

	BaseItemObject* pObj = Global::GetObj(iObjID);
	RoleDataEx* Owner = OwnerObj->Role();

	if (pObj == NULL)
		return false;

	vector< NPC_MoveBaseStruct >::iterator it;
	vector< NPC_MoveBaseStruct >* pNodelist = pObj->NPCMoveInfo()->MoveInfo();

	if (iPathID < 0 || iPathID >= (int)pNodelist->size())
		return false;

	int iCount = 0;
	for (it = pNodelist->begin(); it != pNodelist->end(); it++)
	{
		if (iSeq == 0)
		{
			if (iCount > iPathID)	break;
		}
		else
		{
			if (iCount >= iPathID)	break;
		}

		iCount++;
	}

	NPC_MoveBaseStruct				Path;//		= (*pNodelist)[ iPathID ];

	//Path.sLuaFunc = sLua;

	Path.X = Owner->Pos()->X;
	Path.Y = Owner->Pos()->Y;
	Path.Z = Owner->Pos()->Z;
	Path.Dir = Owner->Pos()->Dir;

	pNodelist->insert(it, Path);

	Message(Owner, "OK");
	return true;
}

bool GmCommandClass::GM_DelPathPos(BaseItemObject* OwnerObj)
{
	int				iObjID = _GetNum(1);
	int				iPathID = _GetNum(2);

	BaseItemObject* pObj = Global::GetObj(iObjID);
	RoleDataEx* Owner = OwnerObj->Role();

	if (pObj == NULL)
		return false;

	vector< NPC_MoveBaseStruct >::iterator it;
	vector< NPC_MoveBaseStruct >* pNodelist = pObj->NPCMoveInfo()->MoveInfo();

	if (iPathID < 0 || iPathID >= (int)pNodelist->size())
		return false;

	int iCount = 0;
	for (it = pNodelist->begin(); it != pNodelist->end(); it++)
	{
		if (iCount >= iPathID)	break;
		iCount++;
	}

	pNodelist->erase(it);
	Message(Owner, "OK");
	return true;
}

bool	GmCommandClass::GM_SavePathPos(BaseItemObject* OwnerObj)
{
	int				GUID = _GetNum(1);

	RoleDataEx* Owner = OwnerObj->Role();
	BaseItemObject* OtherObj = Global::GetObj(GUID);
	if (OtherObj == NULL)
	{
		Message(Owner, "Failed[ ? SavePathPos GUID ]");
		return true;
	}
	RoleDataEx* Other = OtherObj->Role();

	vector< NPC_MoveBaseStruct >* Nodelist = OtherObj->NPCMoveInfo()->MoveInfo();

	Net_DCBase::SaveNPCMoveInfo(RoleDataEx::G_ZoneID, Other->DBID(), *Nodelist);
	Message(Owner, "OK");
	return true;
}

//設定連署公會轉為正式公會
bool	GmCommandClass::GM_SetGuildReady(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember(OwnerObj->Role()->DBID());
	if (GuildMember == false)
	{
		Message(Owner, "Failed");
		return false;
	}

	NetSrv_Guild::SD_SetGuildReady(GuildMember->GuildID);

	Message(Owner, "OK");
	return true;
}

//取得物件版本資料
bool	GmCommandClass::GM_ObjVersion(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	char	Buf[256];

	vector< ObjFileInfo* > FileInfoList = *g_ObjectData->ObjectDB()->FileInfoList();
	vector< ObjFileInfo* >::iterator Iter;
	Owner->Msg("----------------Server File Version---------------");
	for (Iter = FileInfoList.begin(); Iter != FileInfoList.end(); Iter++)
	{
		sprintf(Buf, "%s", (*Iter)->FileHead.FileInfoStr);
		Owner->Msg(Buf);
	}

	Message(Owner, "OK");
	return true;
}

bool GmCommandClass::GM_Pcall(BaseItemObject* OwnerObj)
{
	//if( Global::Ini()->IsEnabledGMCommand == false )
	//	return false;

	RoleDataEx* Owner = OwnerObj->Role();

	int		ItemID;
	char* PlotName;
	int		TargetID = OwnerObj->GUID();
	if (_Count() == 2)
	{
		ItemID = OwnerObj->GUID();
		PlotName = _GetStr(1);
	}
	else if (_Count() == 3)
	{
		ItemID = _GetNum(1);
		PlotName = _GetStr(2);
	}
	else if (_Count() == 4)
	{
		ItemID = _GetNum(1);
		PlotName = _GetStr(2);
		TargetID = _GetNum(3);
	}
	else
	{
		return false;
	}

	BaseItemObject* OtherObj = Global::GetObj(ItemID);
	if (OtherObj == NULL)
	{
		Message(Owner, "Not Find");
		return true;
	}

	LUA_VMClass::PCallByStrArg(PlotName, OtherObj->GUID(), TargetID);
	Message(Owner, "OK");
	return true;
}

//使用路徑搜尋
bool	GmCommandClass::GM_UsePathFind(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	if (PathManageClass::IsUsePathFind() != false)
	{
		Message(Owner, "PathFind Off");
		PathManageClass::IsUsePathFind(false);
	}
	else
	{
		Message(Owner, "PathFind On");
		PathManageClass::IsUsePathFind(true);
	}
	return true;
}

bool	GmCommandClass::GM_EnabledGMCommand(BaseItemObject* OwnerObj)
{
	char* Str = _GetStr(1);

	RoleDataEx* Owner = OwnerObj->Role();

	if (stricmp(Str, "on") == 0)
	{
		Global::Ini()->IsEnabledGMCommand = true;
	}
	else if (stricmp(Str, "off") == 0)
	{
		Global::Ini()->IsEnabledGMCommand = false;
	}
	//OwnerObj->Role()->Msg( "OK" );
	Message(Owner, "OK");
	return true;
}
//設定法術等級
bool	GmCommandClass::GM_SetMagicPoint(BaseItemObject* OwnerObj)
{
	int ID = _GetNum(1);
	int	Value = _GetNum(2);
	RoleDataEx* Owner = OwnerObj->Role();

	if (ID != -1)
	{
		Owner->SetValue(RoleValueName_ENUM(EM_RoleValue_NormalMagic + ID), float(Value), NULL);
		Owner->TempData.UpdateInfo.ReSetTalbe = true;
	}
	else
	{
		for (int i = 0; i < _MAX_SPSkill_COUNT_ + _MAX_NormalSkill_COUNT_; i++)
		{
			GameObjDbStructEx* ObjDB;
			ObjDB = g_ObjectData->GetObj(Owner->PlayerTempData->Skill.AllSkill[i]);
			int MagicLV = Value;
			if (ObjDB == NULL)
			{
				MagicLV = 0;
			}
			else
			{
				MagicLV = __min(Value, ObjDB->MagicCol.MaxSkillLv);
			}

			Owner->SetValue(RoleValueName_ENUM(EM_RoleValue_NormalMagic + i), float(MagicLV), NULL);
			Owner->TempData.UpdateInfo.ReSetTalbe = true;
		}
	}
	Message(Owner, "OK");
	return true;
}

//設定Kill一次算幾次(測試用)
bool	GmCommandClass::GM_SetKillRate(BaseItemObject* OwnerObj)
{
	int KillRate = _GetNum(1);
	RoleDataEx* Owner = OwnerObj->Role();
	Owner->TempData.Sys.KillRate = KillRate;
	Message(Owner, "OK");

	return true;
}

//更改重生的模式
bool	GmCommandClass::GM_SetReviveType(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	bool Flag = Global::St()->IsDisableNPCReviveTime = !Global::St()->IsDisableNPCReviveTime;
	if (Flag == false)
	{
		Message(Owner, "ReviveType Normal");
	}
	else
	{
		Message(Owner, "ReviveType Immediate");
	}
	return true;
}

//搜尋適合的裝備
bool	GmCommandClass::GM_ResetEQ(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	int	Level = _GetNum(1);
	int	Rank = _GetNum(2);

	Global::SetStandardWearEq(Owner, Level, Rank);
	Message(Owner, "OK");
	return true;
}

//加快系統多少分鐘的時間
bool	GmCommandClass::GM_DSysTime(BaseItemObject* OwnerObj)
{
	char Buf[256];
	int DTime = _GetNum(1);
	RoleDataEx* Owner = OwnerObj->Role();

	NetSrv_ChannelBase::SetDSysTime(DTime);
	sprintf(Buf, "OK System  DTime = %d mins", DTime);
	Message(Owner, Buf);
	return true;
}

//兌換商品
bool	GmCommandClass::GM_ItemExchange(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	char* ItemKey = _GetStr(1);
	char* ItemSerial = _GetStr(2);

	string IpStr = Global::GetClientIpNumStr(Owner->TempData.Sys.SockID);

	NetSrv_DepartmentStore::SD_ExchangeItemRequest(Owner->DBID(), Owner->Account_ID(), ItemSerial, ItemKey, 0);
	Message(Owner, "OK");
	return true;
}

//設定角色自定稱號
bool	GmCommandClass::GM_SetTitleStr(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	char* TitleStr = _GetStr(1);
	Owner->BaseData.IsShowTitle = true;
	//	if( Owner->BaseData.TitleID != -1 )
	//	{
	Owner->BaseData.TitleID = -1;
	Owner->TempData.UpdateInfo.Recalculate_All = true;
	//	}

	if (stricmp(Owner->PlayerBaseData->TitleStr.Begin(), TitleStr) != 0)
	{
		Owner->TempData.BackInfo.IsShowTitle = !Owner->BaseData.IsShowTitle;
		Owner->PlayerBaseData->TitleStr = TitleStr;
	}
	Message(Owner, "OK");
	return true;
}

// 顯示現在角色所在的房間號碼
bool	GmCommandClass::GM_ShowRoom(BaseItemObject* OwnerObj)
{
	char szString[256];
	RoleDataEx* Owner = OwnerObj->Role();

	sprintf(szString, "ROOM = %d", Owner->BaseData.RoomID);

	Message(Owner, szString);

	return true;
}

/*
bool	GmCommandClass::GM_ResetCharHead	( BaseItemObject* OwnerObj )
{
	int i;

	for( i = 0 ; i < BaseItemObject::GetmaxZoneID( ) ; i++ )
	{
		BaseItemObject*	pObj = BaseItemObject::GetObj_ByLocalID( i );
		if( pObj == NULL )
			continue;
		else
		{
			RoleDataEx			*pRole		= pObj->Role();
			GameObjDbStructEx	*pObjDB		= Global::GetObjDB( pRole->BaseData.ItemInfo.OrgObjID );

			if( pRole->Base.DBID > 0 )
			{
				// 任務物件
				if( pObjDB->IsQuestNPC() && pObjDB->NPC.iQuestMode != 0 )
				{
					pRole->BaseData.Mode.ShowRoleHead = 0;
				}
				else
				if( pObjDB->IsMine() )
				{
					pRole->BaseData.Mode.ShowRoleHead = 0;
				}
				else
				{
					// NPC 或 任務物件
					pRole->BaseData.Mode.ShowRoleHead = 1;
				}

				// 礦

				// NPC

				// PC
				Net_DCBase::SaveNPCRequest( pRole );
			}
		}
	}

	return true;
}
*/

bool	GmCommandClass::GM_Crash(BaseItemObject* OwnerObj)
{
	int* a = NULL;
	*a = 3;

	return true;
}

bool	GmCommandClass::GM_GetMineFlag(BaseItemObject* OwnerObj)
{
	int		iCount = 0;
	char	szBuffer[1024];

	map< int, int  >	mapObj;



	int iFlagID = _GetNum(1);

	if (iFlagID == 0)
		return false;

	for (int i = 0; i < BaseItemObject::GetmaxZoneID(); i++)
	{
		BaseItemObject* pObj = BaseItemObject::GetObj_ByLocalID(i);
		if (pObj == NULL)
			continue;
		else
		{
			RoleDataEx* pRole = pObj->Role();
			GameObjDbStructEx* pObjDB = Global::GetObjDB(pRole->BaseData.ItemInfo.OrgObjID);

			if (pObjDB->IsMine() && pRole->SelfData.PID == iFlagID)
			{
				int iSourceObjID = pRole->BaseData.ItemInfo.OrgObjID;
				int iVal = mapObj[iSourceObjID];
				mapObj[iSourceObjID] = iVal + 1;
				iCount++;
			}
		}
	}

	// 列出所有礦物資料
	//-----------------------------------------------------------------------------------------------
	sprintf(szBuffer, "FLAG %d 's mine info", iFlagID);		OwnerObj->Role()->Msg(szBuffer);
	sprintf(szBuffer, "Total [ %d ] items, [ %d ] flags", iCount, FlagPosClass::Count(iFlagID));		OwnerObj->Role()->Msg(szBuffer);

	for (map< int, int >::iterator it = mapObj.begin(); it != mapObj.end(); it++)
	{
		int					iObjCount = it->second;
		int					iObjID = it->first;
		GameObjDbStructEx* pObjDB = Global::GetObjDB(iObjID);
		if (pObjDB != NULL)
		{
			sprintf(szBuffer, "%d %s - %d", iObjID, Utf8ToChar(pObjDB->Name).c_str(), iObjCount);		OwnerObj->Role()->Msg(szBuffer);
		}
	}
	Message(OwnerObj->Role(), "OK");
	return true;

}

bool	GmCommandClass::GM_ClearGuildWarTime(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	GuildStruct* GuildInfo = GuildManageClass::This()->GetGuildInfo(Owner->GuildID());
	GuildInfo->Base.HousesWar.NextWarTime = 0;
	return true;
}

//亂數產生旗子
bool	GmCommandClass::GM_RandCreateFlag(BaseItemObject* OwnerObj)
{
	vector< RolePosStruct > PointList;
	RolePosStruct TempPos;

	int FlagID = _GetNum(1);
	int	MoveDest = _GetNum(2);
	int MaxRange = _GetNum(3);
	int Count = _GetNum(4);

	RoleDataEx* Owner = OwnerObj->Role();

	if (FlagID < Def_ObjectClass_Flag)
		FlagID += Def_ObjectClass_Flag;

	FlagPosClass::DelFlagList(FlagID, Owner->Account_ID());

	TempPos = Owner->BaseData.Pos;
	OwnerObj->Path()->Height(TempPos.X, TempPos.Y, TempPos.Z, TempPos.Y, 40);
	PointList.push_back(TempPos);

	float X, Y, Z;
	for (int i = 0; i < 400; i++)
	{
		X = TempPos.X + MoveDest - float(rand() % (MoveDest * 2));
		Y = TempPos.Y;
		Z = TempPos.Z + MoveDest - float(rand() % (MoveDest * 2));
		if (X > Owner->BaseData.Pos.X + MaxRange || X < Owner->BaseData.Pos.X - MaxRange
			|| Z > Owner->BaseData.Pos.Z + MaxRange || Z < Owner->BaseData.Pos.Z - MaxRange)
			continue;

		float Dx = Owner->BaseData.Pos.X - X;
		float Dz = Owner->BaseData.Pos.Z - Z;
		float Dist = __min(float(MoveDest), 40.0f);

		if (Dx * Dx + Dz * Dz < Dist * Dist)
			continue;

		if (OwnerObj->Path()->Height(X, Y, Z, Y, 40.0f) == false)
			continue;

		TempPos.X = X;
		TempPos.Y = Y;
		TempPos.Z = Z;
		PointList.push_back(TempPos);

		if (Count <= (int)PointList.size())
			break;
	}

	for (unsigned i = 0; i < PointList.size(); i++)
	{
		Global::CreateFlag(FlagID, i, PointList[i].X, PointList[i].Y, PointList[i].Z, 0);
	}

	if (Count > (int)PointList.size())
		Message(Owner, "Path data error");
	else
		Message(Owner, "OK");

	return true;
}

//建立所有不同的 圖行的NPC 
bool	GmCommandClass::GM_CreateImageNPC(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int BeginNpcID = _GetNum(1);
	int	EndNpcID = _GetNum(2);
	int	Count = 0;
	float X, Y, Z;
	set< int >	ImageSet;
	set< int >::iterator Iter;
	for (int i = BeginNpcID; i < EndNpcID; i++)
	{
		GameObjDbStructEx* ObjDB = g_ObjectData->GetObj(i);
		if (ObjDB->IsNPC() == false)
			continue;

		if (ObjDB->NPC.iQuestMode != 0)
			continue;

		if (ObjDB->ImageID == 0)
			continue;

		Iter = ImageSet.find(ObjDB->ImageID);
		if (Iter != ImageSet.end())
			continue;

		ImageSet.insert(ObjDB->ImageID);

		X = Owner->Pos()->X + (Count / 10) * 30;
		Y = Owner->Pos()->Y;
		Z = Owner->Pos()->Z + (Count % 10) * 30;;

		OwnerObj->Path()->Height(X, Y, Z, Y, 40.0f);

		Count++;
		int NpcGUID = Global::CreateObj(i, X, Y, Z, 0, 1);
		Global::AddToPartition(NpcGUID, Owner->RoomID());
	}

	Message(Owner, "OK");
	return true;
}

//亂數跳到某個物件位置
bool	GmCommandClass::GM_TestRandTransport(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	int MaxZoneID = BaseItemObject::GetmaxZoneID();
	if (MaxZoneID <= 1)
	{
		Message(Owner, "Transport error");
		return true;
	}

	for (int i = 0; i < 20; i++)
	{
		BaseItemObject* OtherClass = BaseItemObject::GetObj_ByLocalID(rand() % MaxZoneID);
		if (OtherClass == NULL)
			continue;
		RoleDataEx* Other = OtherClass->Role();

		if (CheckFloatAvlid(Other->Pos()->X) == false
			|| CheckFloatAvlid(Other->Pos()->Y) == false
			|| CheckFloatAvlid(Other->Pos()->Z) == false)
			continue;

		if (Other->IsPlayer())
			continue;


		Global::ChangeZone(Owner->GUID(), Other->BaseData.ZoneID, 0, Other->Pos()->X, Other->Pos()->Y, Other->Pos()->Z);
		Message(Owner, "OK");
		return true;
	}

	Message(Owner, "Transport error");
	return true;
}


bool	GmCommandClass::GM_RoleRight(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	RoleDataEx* Target = Owner;
	char* Type;
	int			Value;

	if (_Count() == 3)
	{
		Type = _GetStr(1);
		Value = _GetNum(2);
	}
	else if (_Count() == 4)
	{
		Target = Global::GetRoleDataByGUID(_GetNum(1));
		Type = _GetStr(2);
		Value = _GetNum(3);
		if (Target == NULL)
			return false;
	}

	if (stricmp(Type, "talk") == 0)
	{
		if (Value == 0)
		{
			Target->BaseData.SysFlag.DisableTalk = true;
			Owner->Msg("Talk Disable");
			Target->Net_GameMsgEvent(EM_GameMessageEvent_Role_Right_NoSpeak);
		}
		else
		{
			Target->BaseData.SysFlag.DisableTalk = false;
			Owner->Msg("Talk Enable");
			Target->Net_GameMsgEvent(EM_GameMessageEvent_Role_Right_Speak);
		}

	}
	else if (stricmp(Type, "trade") == 0)
	{
		if (Value == 0)
		{
			Target->BaseData.SysFlag.DisableTrade = true;
			Owner->Msg("Trade Disable");
			Target->Net_GameMsgEvent(EM_GameMessageEvent_Role_Right_NoTrade);
		}
		else
		{
			Target->BaseData.SysFlag.DisableTrade = false;
			Owner->Msg("Trade Enable");
			Target->Net_GameMsgEvent(EM_GameMessageEvent_Role_Right_Trade);
		}
	}
	else
	{
		return true;
	}

	Owner->SetValue(EM_RoleValue_SysFlag, Owner->BaseData.SysFlag._Value, NULL);
	Message(Owner, "OK");
	return true;
}

bool	GmCommandClass::GM_CompareTarget(BaseItemObject* OwnerObj)
{

	int TargetID = _GetNum(1);
	int	TypeID = _GetNum(2);
	RoleDataEx* Target = Global::GetRoleDataByGUID(TargetID);
	RoleDataEx* Owner = OwnerObj->Role();

	if (Target == NULL)
	{
		Message(Owner, "Target Not Find");
		return true;
	}

	int S_Atk = int(Owner->TempData.Attr.Fin.ATK);
	int T_Atk = int(Target->TempData.Attr.Fin.ATK);
	int S_MAtk = int(Owner->TempData.Attr.Fin.MATK);
	int T_MAtk = int(Target->TempData.Attr.Fin.MATK);
	int S_Def = int(Owner->TempData.Attr.Fin.DEF);
	int T_Def = int(Target->TempData.Attr.Fin.DEF);
	int S_MDef = int(Owner->TempData.Attr.Fin.MDEF);
	int T_MDef = int(Target->TempData.Attr.Fin.MDEF);

	float Cal_S_Atk = float((S_Atk + 100));
	float Cal_T_Atk = float((T_Atk + 100));
	float Cal_S_MAtk = float(S_MAtk + 100);
	float Cal_T_MAtk = float(T_MAtk + 100);
	float Cal_S_Def = float(S_Def + 200);
	float Cal_T_Def = float(T_Def + 200);
	float Cal_S_MDef = float(S_MDef + 200);
	float Cal_T_MDef = float(T_MDef + 200);

	/*
		int	OAtk = ( Owner->TempData.Attr.Fin.MATK + 50 );
		int TDef = Target->TempData.Attr.Fin.MDEF + 50;
		if( IsCritial != false )
			TDef *= _DEF_CRITIAL_DEF_RATE_;
		PowerRate = 1 + ( OAtk - TDef )/ __max(OAtk , TDef );
	*/
	float	S_DMG;
	float	T_DMG;

	if (Owner->TempData.Attr.SecWeaponType == EM_Weapon_None)
		S_DMG = Owner->TempData.Attr.Fin.Main_DMG / Owner->TempData.Attr.Fin.Main_ATKSpeed * 10;
	else
		S_DMG = Owner->TempData.Attr.Fin.Main_DMG / Owner->TempData.Attr.Fin.Main_ATKSpeed * 10 + Owner->TempData.Attr.Fin.Sec_DMG / Owner->TempData.Attr.Fin.Sec_ATKSpeed * 10;

	if (Target->TempData.Attr.SecWeaponType == EM_Weapon_None)
		T_DMG = Target->TempData.Attr.Fin.Main_DMG / Target->TempData.Attr.Fin.Main_ATKSpeed * 10;
	else
		T_DMG = Target->TempData.Attr.Fin.Main_DMG / Target->TempData.Attr.Fin.Main_ATKSpeed * 10 + Target->TempData.Attr.Fin.Sec_DMG / Target->TempData.Attr.Fin.Sec_ATKSpeed * 10;

	wchar_t wBuf[512];
	float PowerRate;

	switch (TypeID)
	{
	case 0://0基本 
	{
		Owner->Msg(L"--------------------------------------------");
		swprintf(wBuf, L"目標==>HP=%d/%d MP=%d/%d SP=%d SP_Sub=%d", int(Target->BaseData.HP), Target->MaxHP(), int(Target->BaseData.MP), Target->MaxMP(), int(Target->BaseData.SP), int(Target->BaseData.SP_Sub));
		Owner->Msg(wBuf);
		swprintf(wBuf, L"目標==>DEF=%d MDef=%d Atk =%d MAtk=%d Dmg(DPS)=%.2f", T_Def, T_MDef, T_Atk, T_MAtk, T_DMG);
		Owner->Msg(wBuf);
		swprintf(wBuf, L"自己==>DEF=%d MDef=%d Atk =%d MAtk=%d Dmg(DPS)=%.2f", S_Def, S_MDef, S_Atk, S_MAtk, S_DMG);
		Owner->Msg(wBuf);
	}
	break;
	case 1://1攻防比
	{
		const float DLvRate[11] = { 0.77f , 0.81f , 0.85f, 0.9f ,0.95f ,1.0f , 1.0f , 1.0f , 1.0f , 1.0f, 1.0f };
		int		DLv = Owner->Level() - Target->Level() + 5;
		if (DLv > 10)
			DLv = 10;
		else if (DLv < 0)
			DLv = 0;

		Owner->Msg(L"--------------------------------------------");
		PowerRate = 1 + (Cal_T_Atk - Cal_S_Def) / __max(Cal_T_Atk, Cal_S_Def);


		swprintf(wBuf, L"比較==> Self Def(%d) : Target Atk(%d) => Rate = %.2f DLvRate=%.2f  DPS=%.2f", S_Def, T_Atk, PowerRate, DLvRate[DLv], PowerRate * T_DMG * DLvRate[DLv]);
		Owner->Msg(wBuf);

		PowerRate = 1 + (Cal_T_MAtk - Cal_S_MDef) / __max(Cal_T_MAtk, Cal_S_MDef);

		swprintf(wBuf, L"比較==> Self MDef(%d) : Target MAtk(%d) => Rate = %.2f ", S_MDef, T_MAtk, PowerRate);
		Owner->Msg(wBuf);


		PowerRate = 1 + (Cal_S_Atk - Cal_T_Def) / __max(Cal_T_Atk, Cal_S_Def);

		swprintf(wBuf, L"比較==> Target Def(%d) : Self Atk(%d) => Rate = %.2f DLvRate=%.2f DPS=%.2f", T_Def, S_Atk, PowerRate, DLvRate[10 - DLv], PowerRate * S_DMG * DLvRate[10 - DLv]);
		Owner->Msg(wBuf);

		PowerRate = 1 + (Cal_S_MAtk - Cal_T_MDef) / __max(Cal_T_MAtk, Cal_S_MDef);
		swprintf(wBuf, L"比較==> Target MDef(%d) : Self MAtk(%d) => Rate = %.2f ", T_MDef, S_MAtk, PowerRate);
		Owner->Msg(wBuf);


	}
	break;
	case 2://2閃避比
	{
		int		DLV = Owner->Level() - Target->Level();
		if (DLV > 20)
			DLV = 20;
		else if (DLV < -20)
			DLV = -20;


		int O_MHitRate = int(Owner->TempData.Attr.Fin.Main_HitRate + (DLV * Owner->FixArg().Miss_DLV));
		int O_SHitRate = int(Owner->TempData.Attr.Fin.Sec_HitRate + (DLV * Owner->FixArg().Miss_DLV));
		int O_MissRate = 0;
		int O_DodgeRate = 0;

		int T_MHitRate = int(Target->TempData.Attr.Fin.Main_HitRate + (DLV * Owner->FixArg().Miss_DLV));
		int T_SHitRate = int(Target->TempData.Attr.Fin.Sec_HitRate + (DLV * Owner->FixArg().Miss_DLV));
		int T_MissRate = 0;
		int T_DodgeRate = 0;

		Owner->CalAttackTypeInfo(Target, EM_AttackHandType_Main, O_MHitRate, O_DodgeRate, O_MissRate);
		Owner->CalAttackTypeInfo(Target, EM_AttackHandType_Second, O_SHitRate, O_DodgeRate, O_MissRate);
		Target->CalAttackTypeInfo(Owner, EM_AttackHandType_Main, T_MHitRate, T_DodgeRate, T_MissRate);
		Target->CalAttackTypeInfo(Owner, EM_AttackHandType_Second, T_SHitRate, T_DodgeRate, T_MissRate);

		Owner->Msg(L"--------------------------------------------");

		swprintf(wBuf, L"自己==> 主手 HitRate = %d", O_MHitRate);
		Owner->Msg(wBuf);
		if (O_SHitRate != 0)
		{
			swprintf(wBuf, L"自己==> 副手 HitRate = %d", O_SHitRate);
			Owner->Msg(wBuf);
		}
		swprintf(wBuf, L"自己==> 閃避率 DodgeRate = %d", O_DodgeRate);
		Owner->Msg(wBuf);

		swprintf(wBuf, L"目標==> 主手 HitRate = %d", T_MHitRate);
		Owner->Msg(wBuf);

		if (T_SHitRate != 0)
		{
			swprintf(wBuf, L"目標==> 副手 HitRate = %d", T_SHitRate);
			Owner->Msg(wBuf);
		}
		swprintf(wBuf, L"目標==> 閃避率 DodgeRate = %d", T_DodgeRate);
		Owner->Msg(wBuf);

	}
	break;
	}
	Message(Owner, "OK");
	return true;
}

//刪除所有沒有儲存的物件
bool	GmCommandClass::GM_DelUnSaveObject(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	vector< BaseItemObject* >& ZoneList = *BaseItemObject::GetZoneObjList();

	for (int i = 0; i < (int)ZoneList.size(); i++)
	{
		BaseItemObject* Obj = ZoneList[i];
		if (Obj == NULL || Obj->Role()->BaseData.Mode.Save != false)
			continue;
		if (Obj->Role()->IsPlayer())
			continue;

		Obj->Role()->Destroy("GM_DelUnSaveObject");
	}

	Message(Owner, "OK");
	return true;
}

//所有人數
bool	GmCommandClass::GM_ZonePlayerCount(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	map< int, int > zoneCount;
	map< int, OnlinePlayerInfoStruct* >& dbidMap = *(AllOnlinePlayerInfoClass::This()->DBIDMap());
	map< int, OnlinePlayerInfoStruct* >::iterator dbidMapIter;
	int				totalPlayerCount = 0;
	int				totalZoneCount = 0;

	for (dbidMapIter = dbidMap.begin(); dbidMapIter != dbidMap.end(); dbidMapIter++)
	{
		zoneCount[dbidMapIter->second->ZoneID] ++;
		totalPlayerCount++;
	}

	char	Buf[1024];
	OwnerObj->Role()->Msg("--------------------------------------------------");
	map< int, int >::iterator zoneCountIter;
	for (zoneCountIter = zoneCount.begin(); zoneCountIter != zoneCount.end(); zoneCountIter++)
	{
		sprintf(Buf, "ZoneID=%d count=%d", zoneCountIter->first, zoneCountIter->second);
		OwnerObj->Role()->Msg(Buf);
		totalZoneCount++;
	}

	sprintf(Buf, "TotalZoneCount=%d TotalPlayerCount=%d", totalZoneCount, totalPlayerCount);
	Message(Owner, Buf);

	return true;
}


bool	GmCommandClass::GM_SetBTI(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	int	iBTITIME = _GetNum(1);
	OwnerObj->Role()->SetValue(EM_RoleValue_PlayTimeQuota, iBTITIME);
	Message(Owner, "OK");
	return true;
}

//開關PK
bool	GmCommandClass::GM_PKFlag(BaseItemObject* OwnerObj)
{
	int	PKType = _GetNum(1);
	RoleDataEx* Owner = OwnerObj->Role();

	RoleDataEx::G_PKType = (PKTypeENUM)PKType;

	switch (PKType)
	{
	case EM_PK_Normal://非PK區
		OwnerObj->Role()->Msg("PK Off");
		break;
	case EM_PK_All://大亂鬥
		OwnerObj->Role()->Msg("PK All");
		break;
	case EM_PK_Party://同Party不能互打
		OwnerObj->Role()->Msg("PK Party");
		break;
	case EM_PK_Party_Guild://同Party 同公會 不能互打
		OwnerObj->Role()->Msg("PK Guild");
		break;
	}

	NetSrv_Attack::SendAll_ZonePKFlag(RoleDataEx::G_PKType);

	Message(Owner, "OK");
	return true;
}

bool	GmCommandClass::SetLua(BaseItemObject* OwnerObj)
{
	string LuaKey = _GetStr(1);
	string LuaFunc = _GetStr(2);
	RoleDataEx* Owner = OwnerObj->Role();

	if (strcmp(LuaKey.c_str(), "LuaPVP") == 0)
	{
		Global::Ini()->LuaFunc_PVP = LuaFunc;
	}
	Message(Owner, "OK");
	return true;
}

bool GmCommandClass::SetRoleCamp(BaseItemObject* OwnerObj)
{
	string CampName = _GetStr(1);

	RoleDataEx* Owner = OwnerObj->Role();
	if (Owner == NULL)
		return true;

	CampID_ENUM CampID = RoleDataEx::Camp.GetCampID(CampName.c_str());
	if (CampID == EM_CampID_Unknow)
		return false;

	if (Owner->TempData.AI.CampID != CampID)
	{
		Owner->TempData.AI.CampID = CampID;
		int iRoomID = Owner->RoomID();
		NetSrv_OtherChild::SendRangeCampID(Owner, Owner->TempData.AI.CampID);
	}

	Message(Owner, "OK");

	return true;
}

//清除所有的稱號
bool	GmCommandClass::GM_ClearTitle(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	for (int i = 0; i < _MAX_TITLE_FLAG_COUNT_; i++)
	{
		if (Owner->PlayerBaseData->Title.GetFlag(i) != false)
		{
			Owner->Net_FixTitleFlag(Def_ObjectClass_Title + i, false);
		}
	}
	Owner->PlayerBaseData->Title.ReSet();
	Owner->PlayerBaseData->TitleCount = 0;
	Message(Owner, "OK");
	return true;
}
//清除所有的計數
bool	GmCommandClass::GM_ClearHuntingCount(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	memset(&(Owner->PlayerSelfData->MonsterHunter), 0, sizeof(Owner->PlayerSelfData->MonsterHunter));
	Message(Owner, "OK");
	return true;
}

bool	GmCommandClass::GM_GetObjectInfo(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	if (_Val.size() != 2)
	{
		return false;
	}

	int ItemID = _GetNum(1);

	RoleDataEx* Item = Global::GetRoleDataByGUID(ItemID);
	if (Item == NULL)
		return false;

	NetSrv_RoleValue::S_ObjRoleInfo(Owner->GUID(), Item);
	Message(Owner, "OK");
	return true;
}

//gm 用命令模式給訊息
bool	GmCommandClass::GM_GMTell(BaseItemObject* OwnerObj)
{
	string Msg = _GetStr(1);
	NetSrv_Talk::GMMsg(OwnerObj->GUID(), Msg.c_str());
	return true;
}

bool	GmCommandClass::GM_HideNPC(BaseItemObject* OwnerObj)
{
	int IsHide = _GetNum(1);
	int	HideOrgObjID = _GetNum(2);

	set< BaseItemObject* >& ZoneNpcSet = *BaseItemObject::NPCObjSet();
	set< BaseItemObject* >::iterator Iter;

	for (Iter = ZoneNpcSet.begin(); Iter != ZoneNpcSet.end(); Iter++)
	{
		BaseItemObject* Obj = *Iter;
		RoleDataEx* NPC = Obj->Role();
		if (NPC->BaseData.ItemInfo.OrgObjID != HideOrgObjID)
			continue;

		if (IsHide)
		{
			if (NPC->BaseData.Mode.GM_NpcHide != false)
				continue;
			NPC->BaseData.Mode.GM_NpcHide = true;
			Obj->PlotVM()->EndAllLuaFunc();
			Global::DelFromPartition(NPC->GUID());
		}
		else
		{
			if (NPC->BaseData.Mode.GM_NpcHide == false)
				continue;

			if (Global::CheckClientLoading_AddToPartition(NPC->GUID(), NPC->RoomID()) == false)
				continue;

			if (NPC->SelfData.AutoPlot.Size() != 0)
				Obj->PlotVM()->CallLuaFunc(NPC->SelfData.AutoPlot.Begin(), NPC->GUID());

			GameObjDbStructEx* ObjDB = Global::GetObjDB(NPC->BaseData.ItemInfo.OrgObjID);

			if (ObjDB->IsNPC() || ObjDB->IsQuestNPC())
			{
				if (strlen(ObjDB->NPC.szLuaInitScript) != 0)
				{
					Obj->PlotVM()->CallLuaFunc(ObjDB->NPC.szLuaInitScript, NPC->GUID());
				}
			}



		}
	}

	Message(OwnerObj->Role(), "OK");
	return true;

}

bool	GmCommandClass::GM_ClientSkyType(BaseItemObject* OwnerObj)
{
	ClientSkyProcTypeENUM Type = (ClientSkyProcTypeENUM)_GetNum(1);

	NetSrv_CliConnectChild::SetClientSkyProcType(Type);
	Message(OwnerObj->Role(), "OK");
	return true;
}

bool	GmCommandClass::GM_CalReborn(BaseItemObject* OwnerObj)
{
	int ObjID = _GetNum(1);
	int	Count = _GetNum(2);
	int	RebornTime = _GetNum(3);

	if (RebornTime == 0 || Count == 0)
	{
		Message(OwnerObj->Role(), "Data Error");
		return false;
	}

	int TestCount = 24 * 60 * 60 / RebornTime;
	RoleDataEx* Owner = OwnerObj->Role();
	map< int, int >	ObjMap;

	for (int j = 0; j < Count; j++)
	{
		int NpcID = ObjID;
		for (int i = 0; i < TestCount; i++)
		{
			GameObjDbStructEx* NPCObj = Global::GetObjDB(NpcID);
			if (NPCObj == NULL)
				continue;

			for (int k = 0; k < 2; k++)
			{
				if (Random(100000) < NPCObj->NPC.RaiseChangeRate[k])
				{
					GameObjDbStructEx* NewNpcDB = Global::GetObjDB(NPCObj->NPC.RaiseChangeNPCID[k]);
					if (NewNpcDB->IsNPC() != false)
					{
						NpcID = NPCObj->NPC.RaiseChangeNPCID[k];
						break;
					}
				}
			}

			ObjMap[NpcID] ++;
		}
	}

	Owner->Msg("====================");
	char	Buf[512];
	//列出所有產生的NPC 與 隻數
	map< int, int >::iterator Iter;

	for (Iter = ObjMap.begin(); Iter != ObjMap.end(); Iter++)
	{
		GameObjDbStructEx* ObjDB = Global::GetObjDB(Iter->first);
		if (ObjDB == NULL)
			continue;

		sprintf(Buf, "(%d)%s x %d", ObjDB->GUID, ObjDB->Name, Iter->second);
		NetSrv_Talk::SysMsg(Owner->GUID(), Buf);

		//Owner->Msg( Buf );
	}


	return true;
}
//////////////////////////////////////////////////////////////////////////
//設定區域無法登入
bool	GmCommandClass::GM_StopLogin(BaseItemObject* OwnerObj)
{

	int Value = _GetNum(1);

	if (Value == 0)
	{
		Global::St()->IsDisableLogin = false;
		Message(OwnerObj->Role(), "IsDisableLogin = false");
	}
	else
	{
		Global::St()->IsDisableLogin = true;
		Message(OwnerObj->Role(), "IsDisableLogin = true");
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
//寫Ping 的 Log
bool	GmCommandClass::GM_PingLog(BaseItemObject* OwnerObj)
{
	char	Buf[256];
	int Value = _GetNum(1);
	RoleDataEx* Owner = OwnerObj->Role();

	if (Global::St()->PingLogCount == 0)
	{
		if (Value > 0)
		{
			sprintf(Buf, "PingLog %d Times", Value);
			Owner->Msg(Buf);
		}
	}
	else
	{
		if (Value == 0)
		{
			Owner->Msg("Stop PingLog");
		}
		else
		{
			sprintf(Buf, "Fix PingLog (%d)->(%d) Times", Global::St()->PingLogCount, Value);
			Owner->Msg(Buf);
		}
	}

	Global::St()->PingLogCount = Value;
	Owner->Msg("OK");
	return true;
}
//////////////////////////////////////////////////////////////////////////
//寫Ping 的 Log
bool	GmCommandClass::GM_SrvProcTimeLog(BaseItemObject* OwnerObj)
{
	char	Buf[256];
	int Value = _GetNum(1);
	RoleDataEx* Owner = OwnerObj->Role();

	if (Global::St()->ServerNetProcLogCount == 0)
	{
		if (Value > 0)
		{
			sprintf(Buf, "SrvProcTimeLo %d Times", Value);
			Owner->Msg(Buf);
		}
	}
	else
	{
		if (Value == 0)
		{
			Owner->Msg("Stop SrvProcTimeLo");
		}
		else
		{
			sprintf(Buf, "Fix SrvProcTimeLo (%d)->(%d) Times", Global::St()->PingLogCount, Value);
			Owner->Msg(Buf);
		}
	}

	Global::St()->ServerNetProcLogCount = Value;
	Owner->Msg("OK");
	return true;
}
//////////////////////////////////////////////////////////////////////////
//寫Ping 的 Log
bool	GmCommandClass::GM_ProxyPingLog(BaseItemObject* OwnerObj)
{
	char	Buf[256];
	int Value = _GetNum(1);
	RoleDataEx* Owner = OwnerObj->Role();

	if (Global::St()->ProxyPingLogCount == 0)
	{
		if (Value > 0)
		{
			sprintf(Buf, "ProxyPingLog %d Times", Value);
			Owner->Msg(Buf);
		}
	}
	else
	{
		if (Value == 0)
		{
			Owner->Msg("Stop ProxyPingLog");
		}
		else
		{
			sprintf(Buf, "Fix ProxyPingLog (%d)->(%d) Times", Global::St()->ProxyPingLogCount, Value);
			Owner->Msg(Buf);
		}
	}

	Global::St()->ProxyPingLogCount = Value;
	Owner->Msg("OK");
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool	GmCommandClass::GM_SetIni(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	string	IniStr = _GetStr(1);
	int		Value = _GetNum(2);

	if (stricmp(IniStr.c_str(), "IsGlobalDrop") == 0)
	{
		if (Global::Ini()->IsGlobalDrop != false)
		{
			Global::Ini()->IsGlobalDrop = false;
			Owner->Msg("IsGlobalDrop = false");
		}
		else
		{
			Global::Ini()->IsGlobalDrop = true;
			Owner->Msg("IsGlobalDrop = true");
		}
	}
	else if (stricmp(IniStr.c_str(), "IsAutoRevive") == 0)
	{
		if (Global::Ini()->IsAutoRevive != false)
		{
			Global::Ini()->IsAutoRevive = false;
			Owner->Msg("IsAutoRevive = false");
		}
		else
		{
			Global::Ini()->IsAutoRevive = true;
			Owner->Msg("IsAutoRevive = true");
		}
	}
	else if (stricmp(IniStr.c_str(), "DeadExpDownRate") == 0)
	{
		Global::Ini()->DeadExpDownRate = Value;
	}
	else if (stricmp(IniStr.c_str(), "DeadDebtExpRate") == 0)
	{
		Global::Ini()->DeadDebtExpRate = Value;
	}
	else if (stricmp(IniStr.c_str(), "DeadDebtTpRate") == 0)
	{
		Global::Ini()->DeadDebtTpRate = Value;
	}
	else
	{
		Owner->Msg("not find");
		return false;
	}


	Owner->Msg("OK");
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool	GmCommandClass::GM_RunLottery(BaseItemObject* OwnerObj)
{
	int Num[20];
	NetSrv_Lottery::SD_BuyLottery(-1, -1, -1, Num);
	OwnerObj->Role()->Msg("OK");
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool	GmCommandClass::GM_SetChangeGuild(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	int		Value = _GetNum(1);

	NetSrv_Guild::SD_SetNeedChangeName(Value);
	Owner->Msg("OK");
	return true;
}

//設定值物成長速度
bool	GmCommandClass::GM_PlantGrowRate(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	int		Value = _GetNum(1);

	if (Value > 100)
		Value = 100;
	HousesManageClass::G_PlantGrowRate = float(Value);
	Owner->Msg("OK");
	return true;
}

//設定公會屋戰場目前的時間
bool	GmCommandClass::GM_GuildWarTime(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	int		Day = _GetNum(1);
	int		Hour = _GetNum(2);

	NetSrv_BG_GuildWar::SBL_DebugTime(Day, Hour);
	Owner->Msg("OK");
	return true;
}

bool	GmCommandClass::GM_SQLCmd(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	string DataType = _GetStr(1);
	string SelectCmd = _GetStr(2);

	NetSrv_Other::SD_SelectDB(Owner->DBID(), DataType.c_str(), SelectCmd.c_str());
	Owner->Msg("OK");
	return true;

}

//測試劇情暫停
bool	GmCommandClass::GM_TestLuaPause(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	OwnerObj->PlotVM()->PauseAllLuaFunc();
	Owner->Msg("OK");
	return true;
}
//測試劇情繼續
bool	GmCommandClass::GM_TestLuaStart(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	OwnerObj->PlotVM()->StartAllLuaFunc();
	Owner->Msg("OK");
	return true;
}
bool	GmCommandClass::GM_CheckGuildNameRules(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	std::wstring outStrName;
	CharacterNameRulesENUM  Ret = g_ObjectData->CheckGuildNameRules(_GetStr(1), (CountryTypeENUM)Global::Ini()->CountryType, outStrName);
	switch (Ret)
	{
	case EM_CharacterNameRules_Rightful:			// 合法
		Owner->Msg("合法");
		break;
	case EM_CharacterNameRules_Short:				// 名稱太短,不合法
		Owner->Msg("名稱太短,不合法");
		break;
	case EM_CharacterNameRules_Long:				// 名稱太長,不合法
		Owner->Msg("名稱太長,不合法");
		break;
	case EM_CharacterNameRules_Wrongful:			// 有錯誤的符號或字元
		Owner->Msg("有錯誤的符號或字元");
		break;
	case EM_CharacterNameRules_Special:				// 特殊名稱不能用
		Owner->Msg("特殊名稱不能用");
		break;
	case EM_CharacterNameRules_BasicLetters:		// 只能使用基本英文字母
		Owner->Msg("只能使用基本英文字母");
		break;
	case EM_CharacterNameRules_EndLetter:			// 結尾字元錯誤(',")
		Owner->Msg("結尾字元錯誤");
		break;

	default:
		Owner->Msg("Unknow result");
		break;
	}

	return true;
}

bool	GmCommandClass::GM_GotoOrgID(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	int	OrgID = _GetNum(1);
	int N = _GetNum(2);

	vector< RoleDataEx* > SearchObjList;

	set<BaseItemObject* >& NpcObjSet = *(BaseItemObject::NPCObjSet());
	set< BaseItemObject*>::iterator   NpcObjIter;
	for (NpcObjIter = NpcObjSet.begin(); NpcObjIter != NpcObjSet.end(); NpcObjIter++)
	{
		BaseItemObject* Obj = *NpcObjIter;
		if (Obj == NULL)
			continue;

		if (Obj->Role()->RoomID() != Owner->RoomID())
			continue;
		if (Obj->Role()->BaseData.ItemInfo.OrgObjID != OrgID)
			continue;
		SearchObjList.push_back(Obj->Role());
	}


	if (SearchObjList.size() == 0)
	{
		Message(Owner, "not find");
		return false;
	}

	RoleDataEx* Target;
	if (unsigned(N) < SearchObjList.size())
	{
		Target = SearchObjList[N];
	}
	else
	{
		Target = SearchObjList[0];
	}

	float X = Target->Pos()->X;
	float Y = Target->Pos()->Y;
	float Z = Target->Pos()->Z;
	float Dir = Target->Pos()->Dir;
	int	  RoomID = Target->RoomID();

	Global::ChangeZone(Owner->GUID(), RoleDataEx::G_ZoneID, RoomID, X, Y, Z, Dir);

	char Buf[512];
	sprintf(Buf, "OrgID=%d Name=%s Count = %d", OrgID, Global::GetRoleName_ASCII(Target).c_str(), SearchObjList.size());
	Owner->Msg(Buf);
	Owner->Msg("OK");
	return true;

}

bool	GmCommandClass::GM_DelGuild(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	GuildMemberStruct* GuildMember = GuildManageClass::This()->GetMember(OwnerObj->Role()->DBID());
	if (GuildMember == NULL)
	{
		Message(Owner, "Failed");
		return false;
	}

	NetSrv_Guild::SD_DelGuild(GuildMember->Guild->Base.LeaderDBID);

	Message(Owner, "OK");
	return true;
}
//清除副本進度
bool	GmCommandClass::GM_ClearInstanceInfo(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	memset(&Owner->PlayerBaseData->InstanceSetting, 0, sizeof(Owner->PlayerBaseData->InstanceSetting));
	Message(Owner, "OK");
	return true;
}

bool	GmCommandClass::GM_CheckMem(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	if (BaseItemObject::St()->PlayerRoleDataRecycle.CheckAllMem() == false)
		Owner->Msg("PlayerRoleDataRecycle.CheckAllMem() False");
	else
		Owner->Msg("PlayerRoleDataRecycle.CheckAllMem() OK");

	if (BaseItemObject::St()->NPCRoleDataRecycle.CheckAllMem() == false)
		Owner->Msg("NPCRoleDataRecycle.CheckAllMem() False");
	else
		Owner->Msg("NPCRoleDataRecycle.CheckAllMem() OK");

	if (LUA_VMClass::CheckMemory() == false)
		Owner->Msg("LUA_VMClass::CheckMemory False");
	else
		Owner->Msg("LUA_VMClass::CheckMemory OK");


	return true;
}

//設定指定為公會會長
//bool	GM_SetGuildLeader	( BaseItemObject* OwnerObj )
//設定頻道擁有者
bool	GmCommandClass::GM_SetChannelOwner(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	int Type = _GetNum(1);
	int ChannelID = _GetNum(2);
	NetSrv_ChannelBase::S_ChangeChannelOwner((GroupObjectTypeEnum)Type, ChannelID, Owner->DBID());
	return true;
}
//////////////////////////////////////////////////////////////////////////
//PE 測試用GM命令區
//重新載入此區域所有的AE
bool	GmCommandClass::GM_PE_ReleasePE(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int iIndex = _GetNum(1);

	if (PublicEncounterManager::_ReleasePE(iIndex))
	{
		Owner->Msg("PERelease OK.");
	}
	else
	{
		Owner->Msg("PERelease failed.");
		Owner->Msg("Command Formation : PERelease [PEIndex]");
	}
	return true;
}

//重置某一個PE
bool	GmCommandClass::GM_PE_Reset(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int iIndex = _GetNum(1);

	if (PublicEncounterManager::_ResetPE(iIndex))
	{
		Owner->Msg("PEReset OK");
	}
	else
	{
		Owner->Msg("PEReset failed");
		Owner->Msg("Command Formation : PEReset [PEIndex]");
	}

	return true;
}

//取得特定PE的狀態
bool	GmCommandClass::GM_PE_GetStatus(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	vector<string> vecStrings;

	int iPEIndex = _GetNum(1);

	if (PublicEncounterManager::_GetPEStatus(iPEIndex, vecStrings))
	{
		for (int i = 0; i < (int)vecStrings.size(); ++i)
		{
			Owner->Msg(vecStrings[i].c_str());
		}

		Owner->Msg("PEGetStatus OK");
	}
	else
	{
		Owner->Msg("PEGetStatus failed");
		Owner->Msg("Command Formation : PEGetStatus [PEIndex]");
	}

	return true;
}

//取得特定PE的設定
bool	GmCommandClass::GM_PE_GetDetail(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	vector<string> vecStrings;

	int iPEIndex = _GetNum(1);

	if (PublicEncounterManager::_GetPEDetail(iPEIndex, vecStrings))
	{
		for (int i = 0; i < (int)vecStrings.size(); ++i)
		{
			Owner->Msg(vecStrings[i].c_str());
		}

		Owner->Msg("PEGetDetailStatus OK");
	}
	else
	{
		Owner->Msg("PEGetDetailStatus failed");
		Owner->Msg("Command Formation : PEGetDetailStatus [PEIndex]");
	}

	return true;
}

//強制PE成?
bool	GmCommandClass::GM_PE_InstSucc(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int iIndex = _GetNum(1);

	if (PublicEncounterManager::_PEInstSucc(iIndex))
	{
		Owner->Msg("PEInstSucc OK");
	}
	else
	{
		Owner->Msg("PEInstSucc failed");
		Owner->Msg("Command Formation : PEInstSucc [PEIndex]");
	}
	return true;
}

//強制AE失敗
bool	GmCommandClass::GM_PE_InstFail(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int iIndex = _GetNum(1);

	if (PublicEncounterManager::_PEInstFail(iIndex))
	{
		Owner->Msg("PEInstFail OK");
	}
	else
	{
		Owner->Msg("PEInstFail failed");
		Owner->Msg("Command Formation : PEInstFail [PEIndex]");
	}
	return true;
}

//強制跳躍到某一個Phase
bool	GmCommandClass::GM_PE_JumpToPH(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int	iPEIndex = _GetNum(1);
	int	iPHIndex = _GetNum(2);

	if (PublicEncounterManager::_JumpToPhase(iPEIndex, iPHIndex))
	{
		Owner->Msg("PEJumpToPH OK");
	}
	else
	{
		Owner->Msg("PEJumpToPH failed");
		Owner->Msg("Command Formation : PEJumpToPH [PEIndex] [PHIndex]");
	}

	return true;
}

//強制Phase成?
bool	GmCommandClass::GM_PE_PH_InstSucc(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int	iPEIndex = _GetNum(1);
	int	iPHIndex = _GetNum(2);

	if (PublicEncounterManager::_PHInstSucc(iPEIndex, iPHIndex))
	{
		Owner->Msg("PEPHInstSucc OK");
	}
	else
	{
		Owner->Msg("PEPHInstSucc failed");
		Owner->Msg("Command Formation : PEPHInstSucc [PEIndex] [PHIndex]");
	}
	return true;
}

//強制Phase失敗
bool	GmCommandClass::GM_PE_PH_InstFail(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int	iPEIndex = _GetNum(1);
	int	iPHIndex = _GetNum(2);

	if (PublicEncounterManager::_PHInstFail(iPEIndex, iPHIndex))
	{
		Owner->Msg("PEPHInstFail OK");
	}
	else
	{
		Owner->Msg("PEPHInstFail failed");
		Owner->Msg("Command Formation : PEPHInstFail [PEIndex] [PHIndex]");
	}
	return true;
}

//強制Objective成?
bool	GmCommandClass::GM_PE_OB_InstAchieve(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int	iPEIndex = _GetNum(1);
	int	iPHIndex = _GetNum(2);
	int	iOBIndex = _GetNum(3);

	if (PublicEncounterManager::_OBInstAchieve(iPEIndex, iPHIndex, iOBIndex))
	{
		Owner->Msg("PEOBInstAchieve OK");
	}
	else
	{
		Owner->Msg("PEOBInstAchieve failed");
		Owner->Msg("Command Formation : PEOBInstAchieve [PEIndex] [PHIndex] [OBIndex]");
	}
	return true;
}

//列出PE所有的變數
bool	GmCommandClass::GM_PE_ListAllVar(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	vector<string> vecVarNames;
	vector<int> vecValues;

	char Buffer[64];

	if (PublicEncounterManager::_ListAllVar(vecVarNames, vecValues))
	{
		Owner->Msg("All variable names\n");

		int iLoopTime = min((int)vecVarNames.size(), (int)vecValues.size());

		for (int i = 0; i < iLoopTime; ++i)
		{
			sprintf(Buffer, "%s = %d\n", vecVarNames[i].c_str(), vecValues[i]);
			Owner->Msg(Buffer);
		}
	}
	else
	{
		Owner->Msg("PEListAllVar failed");
	}

	return true;
}

//取得PE用某個變數值 
bool	GmCommandClass::GM_PE_GetVar(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	string	strVarName = _GetStr(1);

	CPublicEncounterVariable* pVar = NULL;

	int iValue = PublicEncounterManager::GetVar(strVarName.c_str());

	char Buffer[64];

	sprintf(Buffer, "%s = %d\n", strVarName.c_str(), iValue);
	Owner->Msg(Buffer);

	return true;
}

//設定PE用某個變數值
bool	GmCommandClass::GM_PE_SetVar(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	string	strVarName = _GetStr(1);

	int iValue = _GetNum(2);

	PublicEncounterManager::SetVar(strVarName.c_str(), iValue);

	Owner->Msg("PESetVar OK\n");

	return true;
}

//加PE用某個變數值
bool GmCommandClass::GM_PE_AddVar(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	string	strVarName = _GetStr(1);

	int iValue = _GetNum(2);

	PublicEncounterManager::AddVar(strVarName.c_str(), iValue);

	Owner->Msg("PEAddVar OK\n");

	return true;
}

//給予某玩家分數
bool GmCommandClass::GM_PE_GiveScore(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int iPEIndex = _GetNum(1);
	int iGUID = _GetNum(2);
	float fScore = _GetFloat(3);

	if (PublicEncounterManager::_GiveScore(iPEIndex, iGUID, fScore))
	{
		Owner->Msg("PEGiveScore OK");
	}
	else
	{
		Owner->Msg("PEGiveScore failed.");
		Owner->Msg("Command Formation : PEGiveScore [PEIndex] [GUID]");
	}

	return true;
}

//取得某玩家分數
bool GmCommandClass::GM_PE_GetScore(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int iPEIndex = _GetNum(1);
	int iGUID = _GetNum(2);

	float fValue = PublicEncounterManager::_GetScore(iPEIndex, iGUID);

	char Buffer[64];

	sprintf(Buffer, "GUID = %d , Score = %f\n", iGUID, fValue);
	Owner->Msg(Buffer);

	Owner->Msg("PEGetScore OK\n");

	return true;
}

bool GmCommandClass::GM_CancelAcSellItem(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int PlayerDBID = _GetNum(1);
	int ItemGUID = _GetNum(2);

	NetSrv_ACChild::SDC_CancelSellItem(PlayerDBID, ItemGUID);

	Owner->Msg("OK");

	return true;
}

bool	GmCommandClass::GM_StopAllVM(BaseItemObject* OwnerObj)
{
	if (Global::Ini()->IsEnabledGMCommand == false)
		return false;

	LUA_VMClass::StopAllVM();
	return true;
}

bool	GmCommandClass::GM_GetValue(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int		ItemID;
	//char*	ValueName	;
	int		ValueID;

	if (_Val.size() == 3)
	{
		ItemID = _GetNum(1);
		ValueID = _GetNum(2);
	}
	else
	{
		ItemID = Owner->TempData.Sys.GUID;
		ValueID = _GetNum(1);
	}

	RoleDataEx* Item = Global::GetRoleDataByGUID(ItemID);

	if (Item == NULL)
	{
		//Owner->Msg( "item id not find!!" );
		Message(Owner, "item id not find!!");
		return true;
	}

	double Val = Item->GetValue((RoleValueName_ENUM)ValueID);

	NetSrv_RoleValue::S_GetTargetRoleValue(OwnerObj->GUID(), ItemID, (RoleValueName_ENUM)ValueID, Val);

	return true;

}

bool GmCommandClass::GM_ListLuaGVars(BaseItemObject* OwnerObj)
{
	//if( Global::Ini()->IsEnabledGMCommand == false )
	//	return false;

	RoleDataEx* Owner = OwnerObj->Role();

	int Type = _GetNum(1);

	char Buff[128];

	std::vector<LuaVarInfoStruct> Output;
	LUA_VMClass::ListGlobalVariables(Output, Type);

	for (int i = 0; i < (int)Output.size(); ++i)
	{
		LuaVarInfoStruct& Info = Output[i];

		switch (Info.Type)
		{
		case LUA_TNIL:
			sprintf(Buff, "%s, Type = Nil\n", Info.Key.c_str());
			break;
		case LUA_TBOOLEAN:
			sprintf(Buff, "%s = %s, Type = Boolean\n", Info.Key.c_str(), Info.Value.c_str());
			break;
		case LUA_TLIGHTUSERDATA:
			sprintf(Buff, "%s = %s, Type = LightUserData\n", Info.Key.c_str(), Info.Value.c_str());
			break;
		case LUA_TNUMBER:
			sprintf(Buff, "%s = %s, Type = Number\n", Info.Key.c_str(), Info.Value.c_str());
			break;
		case LUA_TSTRING:
			sprintf(Buff, "%s = %s, Type = String\n", Info.Key.c_str(), Info.Value.c_str());
			break;
		case LUA_TTABLE:
			sprintf(Buff, "%s, Type = Table\n", Info.Key.c_str());
			break;
		case LUA_TFUNCTION:
			sprintf(Buff, "%s = %s, Type = Function\n", Info.Key.c_str(), Info.Value.c_str());
			break;
		case LUA_TUSERDATA:
			sprintf(Buff, "%s = %s, Type = UserData\n", Info.Key.c_str(), Info.Value.c_str());
			break;
		case LUA_TTHREAD:
			sprintf(Buff, "%s = %s, Type = Thread\n", Info.Key.c_str(), Info.Value.c_str());
			break;
		default:
		{
			sprintf(Buff, "%s, Type = None\n", Info.Key.c_str());
		}
		}

		Message(Owner, Buff);
		Print(LV2, "ListLuaGVars", "%s", Buff);
	}

	return true;
}

bool GmCommandClass::GM_ListLuaTable(BaseItemObject* OwnerObj)
{
	//if( Global::Ini()->IsEnabledGMCommand == false )
	//	return false;

	RoleDataEx* Owner = OwnerObj->Role();

	int MinSize = _GetNum(1); //KB

	char Buff[128];

	int LuaGCSize = LUA_VMClass::GetLuaMemoryUsage();
	sprintf(Buff, "Lua GC size = %dKB\n", LuaGCSize);
	Message(Owner, Buff);
	Print(LV2, "ListLuaTable", "%s", Buff);

	std::vector<LuaVarInfoStruct> Output;
	LUA_VMClass::ListGlobalVariables(Output, LUA_TTABLE);

	for (int i = 0; i < (int)Output.size(); ++i)
	{
		LuaVarInfoStruct& Info = Output[i];

		bool SendMessage = true;

		if (MinSize > 0)
		{
			if (Info.Size < (MinSize * 1024))
			{
				SendMessage = false;
			}
		}

		if (SendMessage == true)
		{
			if (Info.Size > 0)
			{
				if (Info.Size < 1024)
				{
					sprintf(Buff, "%s, size = %d bytes\n", Info.Key.c_str(), Info.Size);
				}
				else if ((Info.Size >= 1024) && (Info.Size < 1024 * 1024))
				{
					sprintf(Buff, "%s, size = %d KB\n", Info.Key.c_str(), Info.Size / 1024);
				}
				else
				{
					sprintf(Buff, "%s, size = %.01f MB\n", Info.Key.c_str(), (float)Info.Size / (1024.0 * 1024.0));
				}

				Message(Owner, Buff);
				Print(LV2, "ListLuaTable", "%s", Buff);
			}
		}
	}

	return true;
}

bool GmCommandClass::GM_GetLuaTableInfo(BaseItemObject* OwnerObj)
{
	//if( Global::Ini()->IsEnabledGMCommand == false )
	//	return false;

	RoleDataEx* Owner = OwnerObj->Role();

	std::string TableName = _GetStr(1);

	char Buff[128];
	std::vector<LuaVarInfoStruct> Output;
	if (LUA_VMClass::GetLuaTableInfo(TableName.c_str(), Output) == true)
	{
		Message(Owner, "\n");
		sprintf(Buff, "Table %s, KVCount=%d\n", TableName.c_str(), Output.size());
		Message(Owner, Buff);
		Print(LV2, "GetLuaTableInfo", "%s", Buff);

		for (int i = 0; i < (int)Output.size(); ++i)
		{
			LuaVarInfoStruct& Info = Output[i];

			sprintf(Buff, "%s=%s\n", Info.Key.c_str(), Info.Value.c_str());
			Message(Owner, Buff);
			Print(LV2, "GetLuaTableInfo", "%s", Buff);
		}
	}
	else
	{
		sprintf(Buff, "%s is not a lua table\n", TableName.c_str());
		Message(Owner, Buff);
	}

	return true;
}

bool GmCommandClass::GM_EnableLogPCall(BaseItemObject* OwnerObj)
{
	if (Global::Ini()->IsEnabledGMCommand == false)
		return false;

	RoleDataEx* Owner = OwnerObj->Role();
	LUA_VMClass::SetIsLogPCall(true);
	Message(Owner, "Log PCall : ON");
	return true;
}

bool GmCommandClass::GM_DisableLogPCall(BaseItemObject* OwnerObj)
{
	if (Global::Ini()->IsEnabledGMCommand == false)
		return false;

	RoleDataEx* Owner = OwnerObj->Role();
	LUA_VMClass::SetIsLogPCall(false);
	Message(Owner, "Log PCall : OFF");
	return true;
}

bool GmCommandClass::GM_ClearPhantom(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();

	int phantomID = 0;

	std::vector< int > Fixes;

	//清除資料
	for (int i = 0; i < MAX_PHANTOM_COUNT; ++i)
	{
		phantomID = Def_ObjectClass_Phantom + i;

		GameObjDbStructEx* objDB = Global::GetObjDB(phantomID);

		if (objDB == NULL)
			continue;

		Owner->PlayerBaseData->Phantom.Info[i].Lv = 0;
		Owner->PlayerBaseData->Phantom.Info[i].Rank = 0;

		Fixes.push_back(i);
	}

	Owner->PlayerBaseData->Phantom.EQ[0] = 0;
	Owner->PlayerBaseData->Phantom.EQ[1] = 0;

	//重新計算
	NetSrv_OtherChild::PhantomChangeProc(Owner);

	//通知Client
	for (unsigned i = 0; i < Fixes.size(); ++i)
	{
		phantomID = Def_ObjectClass_Phantom + Fixes[i];
		NetSrv_Other::SC_FixPhantom(Owner->GUID(), phantomID, Owner->PlayerBaseData->Phantom.Info[Fixes[i]]);
	}

	NetSrv_Other::SC_FixPhantomEQ(Owner->GUID(), Owner->PlayerBaseData->Phantom.EQ);

	return true;
}

bool GmCommandClass::GM_OpenWeekInstancesFrame(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	NetSrv_Other::SC_OpenWeekInstancesFrame(Owner->GUID());
	return true;
}

bool GmCommandClass::GM_OpenPhantomFrame(BaseItemObject* OwnerObj)
{
	RoleDataEx* Owner = OwnerObj->Role();
	NetSrv_Other::SC_OpenPhantomFrame(Owner->GUID());
	return true;
}

bool GmCommandClass::GM_FixStuckItem(BaseItemObject* OwnerObj)
{
	RoleDataEx* role = OwnerObj->Role();
	for (int i = 0; i < role->PlayerBaseData->Body.Count; i++)
	{
		role->Net_FixItemInfo_Body(i);
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////