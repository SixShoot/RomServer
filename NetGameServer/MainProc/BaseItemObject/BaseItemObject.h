#ifndef _BASEITEMOBJECTCLASS_H_2003_09_01_
#define _BASEITEMOBJECTCLASS_H_2003_09_01_
#pragma warning(disable : 4786)
#include <list>
#include <vector>
#include <map>
#include <deque>
#include "RecycleBin/recyclebin.h"
#include "MapAreaManagement/MyMapAreaManagement.h"
#include "RoleData/RoleDataEx.h"
#include "lua\myvm/lua_vmclass.h"
#include "../PathManage/PathManage.h"
#include "functionschedular/functionschedular.h"
#include "PG/NetWakerPGEnum.h"
#include "SimpleSchedular/SimpleSchedular.h"
#include "../AIProc/AIBase.h"
class NPCMoveFlagManageClass;
class BaseItemObject;
#define _Def_Max_Sock_ID_ 0x8000
#define _Def_OnTimeProc_Time_ 1000
#define _Def_Base_ZoneID_ 10000
enum BaseItemObj_ItemTypeEnum
{
    EM_ItemType_Player = 0,
    EM_ItemType_NPC,
    EM_ItemType_Item,
    EM_ItemType_MirrorItem,
    EM_ItemType_MAX = 0xffffffff,
};
typedef bool (*ComObjFunction)(BaseItemObject*, void*);
typedef INT64(*BaseItemObjectFunction)(BaseItemObject*);
struct ClientMiscInfo
{
    int iLanguage;
    ClientMiscInfo()
    {
        iLanguage = 0;
    }
};
class BaseItemObject
{
protected:
    typedef void (*EventFunction)(BaseItemObject*, RoleDataEx*);
    struct BaseItemObjStaticStruct
    {
        RecycleBin<PlayerRoleData> PlayerRoleDataRecycle;
        RecycleBin<RoleData> NPCRoleDataRecycle;
        vector<BaseItemObject*> ZoneObj;
        deque<int> UnUsed;
        deque<int> UnUsedWorldGUID;
        int MaxWorldGUID;
        map<int, BaseItemObject*> MirrorObj;
        set<BaseItemObject*> PlayerObj;
        set<BaseItemObject*> NPCObj;
        set<BaseItemObject*> ItemObj;
        vector<BaseItemObject*> AllPlayer;
        map<string, BaseItemObject*> AccountNameMap;
        map<int, BaseItemObject*> DBIDMap;
        map<int, BaseItemObject*> WorldGUIDMap;
        vector<BaseItemObject*> DelList;
        EventFunction OnCreate;
        EventFunction OnDestroy;
        bool IsRelease;
        SimpleSchedularClass<BaseItemObject> SimpleSchedular;
        set<BaseItemObject*>::iterator SearchIter;
        map<int, BaseItemObject*>::iterator SearchIter_Map;
        BaseItemObjStaticStruct()
            : PlayerRoleDataRecycle(10), NPCRoleDataRecycle(100)
        {
            OnCreate = NULL;
            OnDestroy = NULL;
            MaxWorldGUID = -1;
            IsRelease = false;
        };
    };
    static BaseItemObjStaticStruct* _St;
    static void _NullEventFunction(BaseItemObject*, RoleDataEx*) {};
public:
    static BaseItemObjStaticStruct* St() { return _St; };
    static void Init();
    static void Release();
    static void OnTimeProc();
    static bool CheckMemory();
    static BaseItemObject* GetByOrder_Player(bool IsResetData = false);
    static BaseItemObject* GetByOrder_NPC(bool IsResetData = false);
    static BaseItemObject* GetByOrder_Item(bool IsResetData = false);
    static BaseItemObject* GetByOrder_MirrorObj(bool IsResetData = false);
    static set<BaseItemObject*>* PlayerObjSet() { return &(_St->PlayerObj); }
    static set<BaseItemObject*>* NPCObjSet() { return &(_St->NPCObj); }
    static set<BaseItemObject*>* ItemObjSet() { return &(_St->ItemObj); }
    static map<int, BaseItemObject*>* MirrorObjMap() { return &(_St->MirrorObj); }
    static int GetmaxZoneID();
    static BaseItemObject* GetObj_ByLocalID(int LocalID);
    static BaseItemObject* GetObj(int GUID);
    static BaseItemObject* GetObjBySockID(int ID);
    static BaseItemObject* GetObjByAccountName(string);
    static BaseItemObject* GetObjByPlayerName(string);
    static BaseItemObject* GetObjByName(string);
    static BaseItemObject* GetObjByDBID(int DBID);
    static BaseItemObject* GetObjByWorldGUID(int WorldGUID);
    static BaseItemObject* FindEach(ComObjFunction Func, void* Data);
    static void OnCreateCB(EventFunction Event);
    static void OnDestroyCB(EventFunction Event);
    static void ReportStat();
    static void ReportAllObjInfo();
    static void ReportObjInfo(int GUID) {};
    static void GetUsedWorldItemIDBlock(set<int>& UsedItemIDSet);
    static bool SetMaxWorldGUID(int value);
    static int GetMaxWorldGUID() { return _St->MaxWorldGUID; }
    static int PlayerCount() { return (int)(_St->PlayerObj.size()); }
    static int NPCCount() { return (int)(_St->NPCObj.size()); }
    static int TotalCount() { return int(_St->ZoneObj.size() - _St->UnUsed.size()); }
    static vector<BaseItemObject*>* GetZoneObjList() { return &_St->ZoneObj; };
    static int GetMirrorCount() { return (int)_St->MirrorObj.size(); };
    SimpleSchedularClass<BaseItemObject>& SimpleSchedular() { return _St->SimpleSchedular; };
protected:
    RoleDataEx* _Role;
    LUA_VMClass* _PlotVM;
    PathManageClass _Path;
    BaseItemObj_ItemTypeEnum _Type;
    int _DBID;
    int _WorldGUID;
    int _OrgWorldGUID;
    vector<HSimpleSchedular> _SimpleSchedularRegInfo;
    IAIBaseClass* _AI;
    NPCMoveFlagManageClass* _NPCMoveInfo;
    map<int, ItemFieldStruct> _AccountBag;
    map<int, WebShopBagItemFieldDBStruct> _WebShopBag;
    vector<BattleArraryStruct> _BattleArraryList;
    vector<AttachObjInfoStruct> _AttachList;
    ClientMiscInfo _CliMiscInfo;
public:
    BaseItemObject(BaseItemObj_ItemTypeEnum, int WorldGUID = -1);
    BaseItemObject(int MirrorItemID, int WorldGUID = -1);
    ~BaseItemObject();
    vector<AttachObjInfoStruct>& AttachList() { return _AttachList; };
    bool SetSock(int SockID);
    bool ReleaseSock();
    bool SetDBID(int SockID);
    bool ReleaseDBID();
    bool SetWorldGUID(int WorldGUID);
    int WorldGUID() { return _WorldGUID; };
    void CreateWorldGUID();
    void SetOrgWorldGUID(int WorldGUID);
    int OrgWorldGUID();
    bool ReleaseWorldGUID();
    IAIBaseClass* AI();
    void AI(IAIBaseClass*);
    NPCMoveFlagManageClass* NPCMoveInfo();
    void NPCMoveInfo(int NPCDBID);
    bool SetAccountName(string Name);
    bool ReleaseAccountName();
    RoleDataEx* Role();
    LUA_VMClass* PlotVM();
    void Destroy(const char* szReason);
    PathManageClass* Path() { return &_Path; };
    int SockID();
    int ItemID();
    int GUID();
    bool IsMirror() { return (_Type == EM_ItemType_MirrorItem); }
    INT64 SimpleSchedularReg(BaseItemObjectFunction Func, int Time = 10);
    void ClearSimpleSchedular();
    map<int, ItemFieldStruct>* AccountBag() { return &_AccountBag; };
    map<int, WebShopBagItemFieldDBStruct>* WebShopBag() { return &_WebShopBag; };
    vector<BattleArraryStruct>& BattleArraryList() { return _BattleArraryList; };
    ClientMiscInfo* CliMiscInfo() { return &_CliMiscInfo; }
};
#endif