#ifndef		__GMCOMMAND_H__
#define		__GMCOMMAND_H__
#pragma	warning (disable:4786)
#pragma warning(disable : 4996)

#include "BaseItemObject/BaseItemObject.h"
#include <string.h>
using namespace std;
class GmCommandClass;
//----------------------------------------------------------------------------------------
typedef	bool (GmCommandClass::* GmCmdFunction)(BaseItemObject*);
//----------------------------------------------------------------------------------------
//	GM命令管理結構
//----------------------------------------------------------------------------------------
struct GmCmdManageStruct
{
	string				Name;
	wstring				Help;
	ManagementENUM		ManageLv;

	GmCmdFunction	Fun;
	void Init(ManagementENUM _ManageLv, string _Name, GmCmdFunction	_Fun, wstring	_Help)
	{
		ManageLv = _ManageLv;
		Name = _Name;
		Help = _Help;
		Fun = _Fun;
		//_strlwr( (char*)Name.c_str() );
		std::transform(Name.begin(), Name.end(), Name.begin(), ::tolower);
	}
};
//----------------------------------------------------------------------------------------
class GmCommandClass
{
	map< string, GmCmdManageStruct  >   _GMCmd;

	vector< string > _Val;

	int		        _GetNum(int);
	char* _GetStr(int);
	float	        _GetFloat(int);
	int             _Count() { return (int)_Val.size(); };

	string			_CmdResultStr;
public:

	const char* CmdResultStr() { return _CmdResultStr.c_str(); }

	void			Message(RoleDataEx* Owner, const char* Msg);

	//初始化
	GmCommandClass();
	//註冊命令
	void    RegEvent(ManagementENUM ManageLv, string Name, GmCmdFunction Fun, wstring Help);

	//GM命令處理
	bool	GMCmdProc(BaseItemObject* OwnerObj, const char* Str, const char* RoleName, ManagementENUM ManageLv, bool checkPassword = false);
	//----------------------------------------------------------------------------------------
	//Help 列出所有的GM指令
	bool    GM_Help(BaseItemObject* OwnerObj);
	//給予物品
	bool    GM_Give(BaseItemObject* OwnerObj);
	bool    GM_DelBodyItem(BaseItemObject* OwnerObj);
	//給錢
	bool	GM_GiveMoney(BaseItemObject* OwnerObj);
	bool	GM_GiveMoney_Account(BaseItemObject* OwnerObj);
	bool	GM_GiveMoney_Bonus(BaseItemObject* OwnerObj);
	//呼叫玩家
	bool	GM_CallPlayerName(BaseItemObject* OwnerObj);
	//找某玩家
	bool	GM_GotoPlayerName(BaseItemObject* OwnerObj);
	//找此區的某物件
	bool    GM_GotoID(BaseItemObject* OwnerObj);
	bool	GM_GotoOrgID(BaseItemObject* OwnerObj);

	//將自己斷線
	bool	GM_Logout(BaseItemObject* OwnerObj);

	//搜尋物件
	bool    GM_SearchObj(BaseItemObject* OwnerObj);
	bool	GM_SearchObjCount(BaseItemObject* OwnerObj);

	//設定永久的基本數值
	bool	GM_SetValue(BaseItemObject* OwnerObj);
	bool	GM_SetValueID(BaseItemObject* OwnerObj);
	bool	GM_SetTargetValueID(BaseItemObject* OwnerObj);

	bool	GM_ChangeParallelID(BaseItemObject* OwnerObj);
	bool    GM_ChangeZone(BaseItemObject* OwnerObj);
	bool    GM_ChangeWorld(BaseItemObject* OwnerObj);
	bool	GM_ReturnWorld(BaseItemObject* OwnerObj);
	bool    GM_ChangeRoom(BaseItemObject* OwnerObj);
	bool    GM_CreateObj(BaseItemObject* OwnerObj);
	bool    GM_DelObj(BaseItemObject* OwnerObj);
	bool    GM_ModifyObj(BaseItemObject* OwnerObj);
	bool    GM_CreateFlag(BaseItemObject* OwnerObj);
	bool    GM_HideFlag(BaseItemObject* OwnerObj);
	bool    GM_ShowFlag(BaseItemObject* OwnerObj);

	//產生寵物
	bool    GM_CreatePet(BaseItemObject* OwnerObj);
	//寵物命令
	bool    GM_PetCmd(BaseItemObject* OwnerObj);


	// 設定 Quest 完成或未完成
	bool	GM_QuestFlag(BaseItemObject* OwnerObj);
	//清除所有 Quest 資料
	bool    GM_ClsQuest(BaseItemObject* OwnerObj);

	//測試NPC Save & Load
	bool    GM_TestNPCSave(BaseItemObject* OwnerObj);
	bool    GM_TestNPCLoad(BaseItemObject* OwnerObj);

	//執行某劇情
	bool	GM_RunPlot(BaseItemObject* OwnerObj);
	bool    GM_ReloadPlot(BaseItemObject* OwnerObj);
	bool    GM_ReloadPlotEx(BaseItemObject* OwnerObj);
	bool    GM_ReloadObj(BaseItemObject* OwnerObj);
	//測試用
	bool	GM_Pcall(BaseItemObject* OwnerObj);

	//----------------------------------------------------------------------------------------
	//施法
	bool    GM_SpellMagic(BaseItemObject* OwnerObj);

	//設定可不可以攻擊
	bool    GM_AttackFlag(BaseItemObject* OwnerObj);
	//讓所有NPC回到重生點
	bool    GM_NPCGoHome(BaseItemObject* OwnerObj);
	//設定npc移動旗標
	bool    GM_NPCMoveFlag(BaseItemObject* OwnerObj);
	//列出所有線上玩家
	bool	GM_Who(BaseItemObject* OwnerObj);
	//取得物件版本資料
	bool	GM_ObjVersion(BaseItemObject* OwnerObj);

	//設定法術等級
	bool	GM_SetMagicPoint(BaseItemObject* OwnerObj);

	//設定Kill一次算幾次(測試用)
	bool	GM_SetKillRate(BaseItemObject* OwnerObj);

	//更改重生的模式
	bool	GM_SetReviveType(BaseItemObject* OwnerObj);

	//搜尋適合的裝備
	bool	GM_ResetEQ(BaseItemObject* OwnerObj);

	//加快系統多少分鐘的時間
	bool	GM_DSysTime(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//兌換商品
	bool	GM_ItemExchange(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//GM隱身
	bool	GM_GMHide(BaseItemObject* OwnerObj);

	//----------------------------------------------------------------------------------------
	// 測試
	//----------------------------------------------------------------------------------------
	//測開啟AC
	bool	GM_OpenAC(BaseItemObject* OwnerObj);
	//測試儲存移動點
	bool	GM_TestSaveNPCMove(BaseItemObject* OwnerObj);
	//npc 移動點以旗子顯示 
	bool	GM_ShowNPCMoveFlag(BaseItemObject* OwnerObj);
	//刪除一系列的旗子
	bool	GM_DelFlagList(BaseItemObject* OwnerObj);
	//測試開啟換角介面
	bool	GM_ChangeJob(BaseItemObject* OwnerObj);

	//設定連署公會轉為正式公會
	bool	GM_SetGuildReady(BaseItemObject* OwnerObj);

	//Gm Command 開關
	bool	GM_EnabledGMCommand(BaseItemObject* OwnerObj);
	//使用路徑搜尋
	bool	GM_UsePathFind(BaseItemObject* OwnerObj);
	//設定角色自定稱號
	bool	GM_SetTitleStr(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//SkillValue使用修改玩家技能，技能代碼請查表\n~SkillValue 技能編號 點數
	bool	GM_SkillValue(BaseItemObject* OwnerObj);
	//SkillFull讓玩家的技能全滿\n~SkillFull 玩家名稱
	bool	GM_SkillFull(BaseItemObject* OwnerObj);
	//CardFull讓玩家的技能全滿\n~CardFull 玩家名稱
	bool	GM_CardFull(BaseItemObject* OwnerObj);

	//Full可使玩家HP MP全滿\n~Full
	bool	GM_Full(BaseItemObject* OwnerObj);
	//CallID(單區域)把某物件呼叫到面前\n~CallID 區域物件ID
	bool	GM_CallID(BaseItemObject* OwnerObj);
	//CallName(單區域)把某物件呼叫到面前\n~CallName 區域物件名稱
	bool	GM_CallName(BaseItemObject* OwnerObj);
	//GotoName(單區域)傳送到某物件\n~GotoName 區域物件名稱
	bool	GM_GotoName(BaseItemObject* OwnerObj);
	//InfoID查詢某物件資訊\n~InfoID 區域物件ID 查詢形式\n例如:~InfoID 1 1\n形式代碼\n  0 名稱職業\n  1 攻擊防禦力\n  2 詳細基本數值\n  3 位置\n  4 體力耐敏等數值\n  5 等級血量經驗值\n  6 各項基本技能\n  7 需計算出來的各項數值)
	bool	GM_InfoID(BaseItemObject* OwnerObj);
	//ResetID把身上的所有法術狀態消除\n~ResetID 區域物件ID
	bool	GM_ResetMagic(BaseItemObject* OwnerObj);
	//Rename修改某一個角色的名字\n~Rename 原來角色名稱 後來角色名稱
	bool	GM_Rename(BaseItemObject* OwnerObj);
	//Kill瞬間殺死玩家或NPC\n~Kill 名稱
	bool	GM_KillID(BaseItemObject* OwnerObj);
	//Kick把某玩家踢出遊戲\n~Kick 玩家名稱
	bool	GM_KickID(BaseItemObject* OwnerObj);
	//KickBanID把某玩家踢出遊戲,並把帳號凍結\n KickBanID 區域物件ID
	bool	GM_KickBanID(BaseItemObject* OwnerObj);
	//BcastAll全區廣播\n~BcastAll 全區廣播的內容
	bool	GM_BcastAll(BaseItemObject* OwnerObj);
	//Bcast 區廣播\n~Bcast 區廣播的內容
	bool	GM_Bcast(BaseItemObject* OwnerObj);
	//DebugPlot劇情除錯，設定某一個物件跑的劇情步驟輸出給下指令者，最好同時設定目標與自己劇情常會在玩家與NPC身上轉換\n~DebugPlot 區域物件ID 劇情ID
	bool	GM_DebugPlot(BaseItemObject* OwnerObj);
	//Faceoff變身成怪物\n小心身上的物品會複製一份掉出來\n變身後不能換區一定要登出\n~Faceoff 怪物物件ID 之後的名字
	bool	GM_FaceOff(BaseItemObject* OwnerObj);
	//GMS.Help	= "系統時間設定\n~SystemTime 天數差 小時差 \n~SystemTime 1 1";
	bool	GM_SystemTime(BaseItemObject* OwnerObj);
	//Visibl設定角色隱形開關，此種隱形玩家查不到\n~Hide on\n~Hide off
	bool	GM_Visible(BaseItemObject* OwnerObj);
	//ObjList列表區域物件資料\n~ObjList 開始ID 結束ID\n例如:~ObjList 0 100
	bool	GM_ObjList(BaseItemObject* OwnerObj);
	//OrgObjList列表 物件資料
	bool	GM_OrgObjList(BaseItemObject* OwnerObj);
	//NoSpeak讓玩家禁言幾秒鐘，下線後就不算了\n~NoSpeak 玩家名稱 秒數
	//SearchOrgObj尋找原始物件上面有某一個字串的命令，並設定其種類\n~SearchOrgObj 字串 類別\n字串 : (*) 代表全部，其它只要物件名稱類有函此字串就算\n類別 : 不填代表全部，其它可分(武器,防具,道具,法術,NPC,卡片)(人魔仙)(GM,劍,..)\n 例如 : ~SearchOrgObj 刀 武器魔刀
	bool	GM_SearchOrgObj(BaseItemObject* OwnerObj);
	//BindingItemMagic設定某一個物件使用的效果，(此物件原本需要可以使用)\n~BindingItemMagic 物品ID 法術ID
	bool	GM_BindingItemMagic(BaseItemObject* OwnerObj);
	//MaxOnline設定最高上線人數\n~MaxOnline 最高上線人數
	bool	GM_MaxOnline(BaseItemObject* OwnerObj);
	//GameTime設定遊戲偏移時間 \n~GameTime 偏移時間(小時)
	bool	GM_GameTime(BaseItemObject* OwnerObj);
	//TimeSpeed 加速遊戲 x1 x2..
	bool	GM_TimeSpeed(BaseItemObject* OwnerObj);
	//CallSpell 設定此區域是否可以施呼叫同伴的法術\n例~CallSpell false
	bool	GM_CallSpell(BaseItemObject* OwnerObj);
	//ResitColdown 把法術的Coldown 時間清除\n例~ResitColdown
	bool	GM_ResetColdown(BaseItemObject* OwnerObj);
	bool	GM_NewResetColdown(BaseItemObject* OwnerObj);
	//SetFightID 設定群體戰的組別\n例~SetFightID 物件ID 組別 \n~SetFightID 100 10
	bool	GM_SetFightID(BaseItemObject* OwnerObj);
	//顯示角色所擁有的特殊物品
	bool    GM_ShowKeyItem(BaseItemObject* OwnerObj);
	bool    GM_DelKeyItem(BaseItemObject* OwnerObj);

	bool    GM_GetPathInfo(BaseItemObject* OwnerObj);
	bool    GM_SetPathInfo(BaseItemObject* OwnerObj);
	bool    GM_SetPathLua(BaseItemObject* OwnerObj);
	bool	GM_SetPathByFlag(BaseItemObject* OwnerObj);
	bool	GM_SetPathPos(BaseItemObject* OwnerObj);
	bool	GM_AddPathPos(BaseItemObject* OwnerObj);
	bool	GM_DelPathPos(BaseItemObject* OwnerObj);
	bool	GM_SavePathPos(BaseItemObject* OwnerObj);

	bool	GM_ShowRoom(BaseItemObject* OwnerObj);
	//	bool	GM_ResetCharHead	( BaseItemObject* OwnerObj );
	bool	GM_GetMineFlag(BaseItemObject* OwnerObj);
	bool	GM_Crash(BaseItemObject* OwnerObj);

	//亂數產生旗子
	bool	GM_RandCreateFlag(BaseItemObject* OwnerObj);

	//
	bool	GM_ClearGuildWarTime(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//設定要取得lua錯誤資訊
	bool    GM_GetLuaErrorInfo(BaseItemObject* OwnerObj);

	//建立所有不同的 圖行的NPC 
	bool	GM_CreateImageNPC(BaseItemObject* OwnerObj);

	//亂數跳到某個物件位置
	bool	GM_TestRandTransport(BaseItemObject* OwnerObj);


	bool	GM_RoleRight(BaseItemObject* OwnerObj);

	bool	GM_KickAll(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	bool	GM_CompareTarget(BaseItemObject* OwnerObj);

	//刪除所有沒有儲存的物件
	bool	GM_DelUnSaveObject(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//所有人數
	bool	GM_ZonePlayerCount(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	bool	GM_SetBTI(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//開關PK
	bool	GM_PKFlag(BaseItemObject* OwnerObj);
	//清除所有的稱號
	bool	GM_ClearTitle(BaseItemObject* OwnerObj);
	//清除所有的計數
	bool	GM_ClearHuntingCount(BaseItemObject* OwnerObj);

	//取得某物見的完整資料
	bool	GM_GetObjectInfo(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//重置每日任務
	bool	ResetDailyQuest(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	bool	SetLua(BaseItemObject* OwnerObj);
	bool	SetRoleCamp(BaseItemObject* OwnerObj);
	bool	GM_GMTell(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	bool	GM_HideNPC(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	bool	GM_ClientSkyType(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	bool	GM_CalReborn(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//設定區域無法登入
	bool	GM_StopLogin(BaseItemObject* OwnerObj);

	//寫Ping 的 Log
	bool	GM_PingLog(BaseItemObject* OwnerObj);
	bool	GM_ProxyPingLog(BaseItemObject* OwnerObj);
	bool	GM_SrvProcTimeLog(BaseItemObject* OwnerObj);
	//設定INI
	bool	GM_SetIni(BaseItemObject* OwnerObj);
	bool	GM_RunLottery(BaseItemObject* OwnerObj);
	bool	GM_SetChangeGuild(BaseItemObject* OwnerObj);

	//設定值物成長速度
	bool	GM_PlantGrowRate(BaseItemObject* OwnerObj);

	//設定公會屋戰場目前的時間
	bool	GM_GuildWarTime(BaseItemObject* OwnerObj);

	//SQL
	bool	GM_SQLCmd(BaseItemObject* OwnerObj);

	//測試劇情暫停
	bool	GM_TestLuaPause(BaseItemObject* OwnerObj);
	//測試劇情繼續
	bool	GM_TestLuaStart(BaseItemObject* OwnerObj);

	bool	GM_CheckGuildNameRules(BaseItemObject* OwnerObj);

	//設定指定為公會會長
//	bool	GM_SetGuildLeader	( BaseItemObject* OwnerObj );
	bool	GM_DelGuild(BaseItemObject* OwnerObj);

	bool	GM_CheckMem(BaseItemObject* OwnerObj);
	//清除副本進度
	bool	GM_ClearInstanceInfo(BaseItemObject* OwnerObj);

	//設定頻道擁有者
	bool	GM_SetChannelOwner(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//PE 測試用GM命令區

	//清除指定PE的設定
	bool	GM_PE_ReleasePE(BaseItemObject* OwnerObj);

	//重置某一個PE
	bool	GM_PE_Reset(BaseItemObject* OwnerObj);

	//取得特定PE的狀態
	bool	GM_PE_GetStatus(BaseItemObject* OwnerObj);

	//取得特定PE的設定
	bool	GM_PE_GetDetail(BaseItemObject* OwnerObj);

	//強制PE成?
	bool	GM_PE_InstSucc(BaseItemObject* OwnerObj);

	//強制PE失敗
	bool	GM_PE_InstFail(BaseItemObject* OwnerObj);

	//強制跳躍到某一個Phase
	bool	GM_PE_JumpToPH(BaseItemObject* OwnerObj);

	//強制Phase成?
	bool	GM_PE_PH_InstSucc(BaseItemObject* OwnerObj);

	//強制Phase失敗
	bool	GM_PE_PH_InstFail(BaseItemObject* OwnerObj);

	//強制Objective成?
	bool	GM_PE_OB_InstAchieve(BaseItemObject* OwnerObj);

	//列出所有的變數
	bool	GM_PE_ListAllVar(BaseItemObject* OwnerObj);

	//取得PE用某個變數值 
	bool	GM_PE_GetVar(BaseItemObject* OwnerObj);

	//設定PE用某個變數值
	bool	GM_PE_SetVar(BaseItemObject* OwnerObj);

	//加PE用某個變數值
	bool	GM_PE_AddVar(BaseItemObject* OwnerObj);

	//幫指定的玩家加分
	bool	GM_PE_GiveScore(BaseItemObject* OwnerObj);

	//取得指定的玩家的分數
	bool	GM_PE_GetScore(BaseItemObject* OwnerObj);

	//取消交易所內的物品資料
	bool	GM_CancelAcSellItem(BaseItemObject* OwnerObj);

	// 取得指定角色數值
	bool	GM_GetValue(BaseItemObject* OwnerObj);
	bool	GM_StopAllVM(BaseItemObject* OwnerObj);

	//----------------------------------------------------------------------------------------
	bool	GM_ListLuaGVars(BaseItemObject* OwnerObj);
	bool	GM_ListLuaTable(BaseItemObject* OwnerObj);
	bool	GM_GetLuaTableInfo(BaseItemObject* OwnerObj);
	bool	GM_EnableLogPCall(BaseItemObject* OwnerObj);
	bool	GM_DisableLogPCall(BaseItemObject* OwnerObj);

	//----------------------------------------------------------------------------------------
	bool	GM_ClearPhantom(BaseItemObject* OwnerObj);
	bool	GM_OpenWeekInstancesFrame(BaseItemObject* OwnerObj);
	bool	GM_OpenPhantomFrame(BaseItemObject* OwnerObj);
	bool	GM_FixStuckItem(BaseItemObject* OwnerObj);

};

const char* MyStrToken(char* Buf, char** NextBufPoint);

#endif