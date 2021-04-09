#include "BaseItemObject.h"
#include "NetWaker/GSrvNetWaker.h"
#include "../../NetWalker_Member/NetWakerGSrvInc.h"
#include "../Global.h"
#include "../pathmanage/NPCMoveFlagManage.h"
BaseItemObject::BaseItemObjStaticStruct* BaseItemObject::_St = NULL;
void BaseItemObject::Init()
{
    _St = NEW(BaseItemObjStaticStruct);
}
void BaseItemObject::Release()
{
    _St->IsRelease = true;
    int i;
    BaseItemObject* Obj;
    for (i = 0; i < GetmaxZoneID(); i++)
    {
        Obj = _St->ZoneObj[i];
        if (Obj == NULL)
            continue;
        delete Obj;
    }
    _St->DelList.clear();
    for (Obj = GetByOrder_MirrorObj(true); Obj != NULL; Obj = GetByOrder_MirrorObj())
    {
        _St->DelList.push_back(Obj);
    }
    for (int i = 0; i < (int)_St->DelList.size(); i++)
    {
        delete _St->DelList[i];
    }
    _St->DelList.clear();
    if (_St != NULL)
        delete _St;
    _St = NULL;
}
void BaseItemObject::OnCreateCB(EventFunction Event)
{
    _St->OnCreate = Event;
}
void BaseItemObject::OnDestroyCB(EventFunction Event)
{
    _St->OnDestroy = Event;
}
bool BaseItemObject::CheckMemory()
{
    return _St->PlayerRoleDataRecycle.CheckAllMem();
}
BaseItemObject* BaseItemObject::GetObj_ByLocalID(int LocalID)
{
    BaseItemObject* Ret;
    if ((unsigned)LocalID >= _St->ZoneObj.size())
        return NULL;
    else
    {
        Ret = _St->ZoneObj[LocalID];
        if (Ret == NULL || Ret->_Role == NULL)
            return NULL;
        return Ret;
    }
}
BaseItemObject* BaseItemObject::GetObj(int ID)
{
    if (ID == -1 || _St == NULL)
        return NULL;
    GItemData IDInfo;
    IDInfo.GUID = ID;
    BaseItemObject* Ret;
    if (IDInfo.Zone == RoleDataEx::G_ZoneID)
    {
        if ((unsigned)IDInfo.ID >= _St->ZoneObj.size())
            return NULL;
        else
        {
            Ret = _St->ZoneObj[IDInfo.ID];
            if (Ret == NULL || Ret->_Role == NULL)
                return NULL;
            return Ret;
        }
    }
    else
    {
        map<int, BaseItemObject*>::iterator Iter;
        Iter = _St->MirrorObj.find(ID);
        if (Iter == _St->MirrorObj.end())
            return NULL;
        if (Iter->second == NULL || Iter->second->_Role == NULL)
            return NULL;
        return Iter->second;
    }
}
BaseItemObject* BaseItemObject::FindEach(ComObjFunction Func, void* Data)
{
    int i;
    for (i = 0; i < GetmaxZoneID(); i++)
    {
        BaseItemObject* Obj = GetObj_ByLocalID(i);
        if (Obj == NULL)
            continue;
        if (Func(Obj, Data))
            return Obj;
    }
    return NULL;
}
void BaseItemObject::OnTimeProc()
{
    _St->SimpleSchedular.Proc();
    static int DClock = 0;
    int i;
    BaseItemObject* Obj;
    RoleDataEx* Owner;
    DClock = RoleDataEx::G_Clock % _DEF_BASE_CLOCL_UNIT_;
    for (i = DClock; i < GetmaxZoneID(); i += _DEF_BASE_CLOCL_UNIT_)
    {
        Obj = _St->ZoneObj[i];
        if (Obj == NULL)
            continue;
        Owner = Obj->Role();
        if (Owner->IsPlayer())
        {
            Global::_Player_OnTimeProc(Obj, Owner);
        }
        else if (Owner->IsNPC())
        {
            Global::_NPC_OnTimeProc(Obj, Owner);
        }
        Global::_Obj_OnTimeProc(Obj, Owner);
        RoleDataEx* Role = Obj->Role();
        if (Role->LiveTime() >= 0)
            continue;
        if (Role->TempData.Sys.OnChangeWorld)
        {
            continue;
        }
        if (Obj->Role()->CheckTreasureDelete() == false)
            continue;
        if (Global::Ini()->IsBattleWorld == false)
        {
            if (Owner->IsPlayer() && RoleDataEx::G_ZoneID == Owner->ZoneID() && Global::Net_ServerList->CheckServerExist(EM_SERVER_TYPE_ROLECENTER) == false)
            {
                Global::DelFromPartition(Owner->GUID());
                continue;
            }
        }
        _St->DelList.push_back(Obj);
    }
    if (DClock == 0)
    {
        for (Obj = GetByOrder_MirrorObj(true); Obj != NULL; Obj = GetByOrder_MirrorObj())
        {
            if (Obj->Role()->LiveTime() >= 0)
                continue;
            _St->DelList.push_back(Obj);
        }
    }
    for (int i = 0; i < (int)_St->DelList.size(); i++)
    {
        delete _St->DelList[i];
    }
    _St->DelList.clear();
    if (RoleDataEx::G_Clock % 18000 == 0)
    {
        _St->PlayerRoleDataRecycle.GarbageCollectionProc();
        _St->NPCRoleDataRecycle.GarbageCollectionProc();
    }
}
BaseItemObject::BaseItemObject(BaseItemObj_ItemTypeEnum Type, int WorldGUID)
{
    _AI = NEW(IAIBaseClass);
    _Type = Type;
    _NPCMoveInfo = NEW(NPCMoveFlagManageClass);
    _PlotVM = NULL;
    GItemData IDInfo;
    IDInfo.Zone = RoleDataEx::G_ZoneID;
    while (_St->UnUsed.size() < 1000)
    {
        if (_St->ZoneObj.size() != 0)
            _St->UnUsed.push_back((int)_St->ZoneObj.size());
        _St->ZoneObj.push_back(NULL);
    }
    if (_Type == EM_ItemType_Player)
    {
        _Role = (RoleDataEx*)_St->PlayerRoleDataRecycle.NewObj();
        if (_Role == NULL)
        {
            Print(Def_PrintLV5, "BaseItemObject", "PlayerRoleDataRecycle.NewObj() == NULL , RoleData Object Recycling has question ?? ");
            _St->DelList.push_back(this);
            return;
        }
        ((PlayerRoleData*)_Role)->Init();
    }
    else
    {
        _Role = (RoleDataEx*)_St->NPCRoleDataRecycle.NewObj();
        if (_Role == NULL)
        {
            Print(Def_PrintLV5, "BaseItemObject", "NPCRoleDataRecycle.NewObj() == NULL , RoleData Object Recycling has question ?? ");
            _St->DelList.push_back(this);
            return;
        }
        _Role->Init();
    }
    IDInfo.ID = _St->UnUsed.front();
    _St->UnUsed.pop_front();
    if (WorldGUID == -1)
    {
        CreateWorldGUID();
    }
    else
    {
        SetWorldGUID(WorldGUID);
    }
    _Role->GUID(IDInfo.GUID);
    _St->ZoneObj[IDInfo.ID] = this;
    _PlotVM = NEW LUA_VMClass(IDInfo.GUID);
    switch (_Type)
    {
    case EM_ItemType_Player:
        _St->PlayerObj.insert(this);
        break;
    case EM_ItemType_NPC:
        _St->NPCObj.insert(this);
        break;
    case EM_ItemType_Item:
        _St->ItemObj.insert(this);
        break;
    default:
        Print(Def_PrintLV4, "BaseItemObject", "Create Object , BaseItemObj_ItemTypeEnum Error");
        Destroy("SYS BaseItemObject Create Error");
        return;
    }
    _Role->OnCreate();
    _St->OnCreate(this, this->Role());
}
BaseItemObject::BaseItemObject(int GUID, int WorldGUID)
{
    _AI = NEW(IAIBaseClass);
    _NPCMoveInfo = NEW(NPCMoveFlagManageClass);
    GItemData IDInfo;
    _Type = EM_ItemType_MirrorItem;
    _Role = NULL;
    _DBID = -1;
    IDInfo.GUID = GUID;
    _PlotVM = NULL;
    if (IDInfo.Zone == RoleDataEx::G_ZoneID)
    {
        Print(Def_PrintLV4, "BaseItemObject", "Create Mirror Object Error!!!!");
        _St->DelList.push_back(this);
        return;
    }
    _Role = (RoleDataEx*)_St->NPCRoleDataRecycle.NewObj();
    if (_Role == NULL)
    {
        Print(Def_PrintLV4, "BaseItemObject", "RoleDataRecycle.NewObj() == NULL , RoleData Object Recycling has question ?? ");
        _St->DelList.push_back(this);
        return;
    }
    _PlotVM = NEW LUA_VMClass(IDInfo.GUID);
    _Role->Init();
    _Role->NpcInitLink();
    SetWorldGUID(WorldGUID);
    _Role->GUID(IDInfo.GUID);
    _St->MirrorObj[IDInfo.GUID] = this;
    _Role->OnCreate();
    _St->OnCreate(this, this->Role());
}
BaseItemObject::~BaseItemObject()
{
    ClearSimpleSchedular();
    GItemData IDInfo;
    if (_St->IsRelease == false)
        _St->OnDestroy(this, this->Role());
    if (_Role != NULL)
    {
        IDInfo.GUID = Role()->GUID();
        if (_Role->IsPlayer() == false)
        {
            Print(Def_PrintLV2, "~BaseItemObject", "Delete NPC  Name=%s DBID =%d GUID=%d OrgID=%d Account=%s ", Global::GetRoleName_ASCII(_Role).c_str(), _Role->DBID(), _Role->GUID(), _Role->BaseData.ItemInfo.OrgObjID, _Role->TempData.Sys.DestroyAccount.Begin());
        }
        else
        {
            if (RoleDataEx::G_ZoneID == _Role->ZoneID())
            {
                Print(Def_PrintLV2, "~BaseItemObject", "Delete Player  Name=%s DBID =%d Account=%s ", Global::GetRoleName_ASCII(_Role).c_str(), _Role->DBID(), _Role->TempData.Sys.DestroyAccount.Begin());
            }
        }
        {
            switch (_Type)
            {
            case EM_ItemType_Player:
                if (_St->IsRelease == false)
                    _Role->OnDestroy();
                if (_St->PlayerObj.erase(this) == 0)
                    Print(Def_PrintLV4, "~BaseItemObject", "Delete Error , EM_ItemType_Player Not Find");
                _St->ZoneObj[IDInfo.ID] = NULL;
                _St->UnUsed.push_back(IDInfo.ID);
                break;
            case EM_ItemType_NPC:
                if (_St->NPCObj.erase(this) == 0)
                    Print(Def_PrintLV4, "~BaseItemObject", "Delete Error , EM_ItemType_NPC Not Find");
                _St->ZoneObj[IDInfo.ID] = NULL;
                _St->UnUsed.push_back(IDInfo.ID);
                break;
            case EM_ItemType_Item:
                if (_St->ItemObj.erase(this) == 0)
                    Print(Def_PrintLV4, "~BaseItemObject", "Delete Error , EM_ItemType_Item Not Find");
                _St->ZoneObj[IDInfo.ID] = NULL;
                _St->UnUsed.push_back(IDInfo.ID);
                break;
            case EM_ItemType_MirrorItem:
                if (_St->MirrorObj.erase(IDInfo.GUID) == 0)
                    Print(Def_PrintLV4, "~BaseItemObject", "Delete Error , EM_ItemType_MirrorItem Not Find");
                break;
            default:
                Print(Def_PrintLV4, "~BaseItemObject", "Delete Error _Type = ???");
                break;
            }
        }
    }
    else
    {
        Print(Def_PrintLV4, "~BaseItemObject", "Role() == NULL");
    }
    if (_PlotVM != NULL)
        delete _PlotVM;
    if (_Role != NULL)
    {
        if (_Type == EM_ItemType_Player)
        {
            _St->PlayerRoleDataRecycle.DeleteObj((PlayerRoleData*)_Role);
            ReleaseAccountName();
        }
        else
        {
            _St->NPCRoleDataRecycle.DeleteObj(_Role);
        }
    }
    ReleaseSock();
    ReleaseDBID();
    ReleaseWorldGUID();
    if (_AI != NULL)
        delete _AI;
    if (_NPCMoveInfo != NULL)
        delete _NPCMoveInfo;
}
bool BaseItemObject::SetSock(int SockID)
{
    if (EM_ItemType_MirrorItem == _Type)
        return false;
    if ((unsigned)SockID > _Def_Max_Sock_ID_)
        return false;
    while ((unsigned)SockID >= _St->AllPlayer.size())
        _St->AllPlayer.push_back(NULL);
    BaseItemObject* Obj = _St->AllPlayer[SockID];
    _St->AllPlayer[SockID] = this;
    _Role->SockID(SockID);
    TRACE1("\nSetSock=%d", SockID);
    return true;
}
bool BaseItemObject::ReleaseSock()
{
    if (EM_ItemType_MirrorItem == _Type)
        return false;
    if ((unsigned)(_Role->SockID()) >= _St->AllPlayer.size())
        return false;
    TRACE1("\nDelSock=%d", _Role->SockID());
    _St->AllPlayer[_Role->SockID()] = NULL;
    _Role->SockID(-1);
    return true;
}
bool BaseItemObject::SetWorldGUID(int WorldGUID)
{
    _WorldGUID = WorldGUID;
    _OrgWorldGUID = WorldGUID;
    Role()->WorldGUID(WorldGUID);
    _St->WorldGUIDMap[WorldGUID] = this;
    return true;
}
bool BaseItemObject::ReleaseWorldGUID()
{
    if (_St->WorldGUIDMap.erase(_WorldGUID) == 0)
        return false;
    return true;
}
bool BaseItemObject::SetDBID(int DBID)
{
    _DBID = DBID;
    _St->DBIDMap[_DBID] = this;
    return true;
}
bool BaseItemObject::ReleaseDBID()
{
    if (_St->DBIDMap.erase(_DBID) == 0)
        return false;
    _DBID = -1;
    return true;
}
RoleDataEx* BaseItemObject::Role()
{
    return _Role;
}
LUA_VMClass* BaseItemObject::PlotVM()
{
    return _PlotVM;
}
void BaseItemObject::Destroy(const char* cszReason)
{
    Global::DelFromPartition(_Role->GUID());
    if (_PlotVM != NULL)
        _PlotVM->EndAllLuaFunc();
    if (IsMirror())
    {
        _Role->LiveTime(0, cszReason);
    }
    else if (_Role->IsPlayer())
    {
        if (_Role->LiveTime() > 2000)
        {
            _Role->LiveTime(2000, cszReason);
        }
    }
    else
    {
        if (_Role->IsDestroy() == false)
        {
            _Role->LiveTime(0, cszReason);
        }
    }
}
BaseItemObject* BaseItemObject::GetByOrder_Player(bool IsResetData)
{
    BaseItemObject* Ret;
    if (IsResetData != false)
        _St->SearchIter = _St->PlayerObj.begin();
    if (_St->SearchIter == _St->PlayerObj.end())
        return NULL;
    Ret = *_St->SearchIter;
    _St->SearchIter++;
    if (Ret->Role() == NULL)
        return GetByOrder_Player();
    return Ret;
}
BaseItemObject* BaseItemObject::GetByOrder_NPC(bool IsResetData)
{
    BaseItemObject* Ret;
    if (IsResetData != false)
        _St->SearchIter = _St->NPCObj.begin();
    if (_St->SearchIter == _St->NPCObj.end())
        return NULL;
    Ret = *_St->SearchIter;
    _St->SearchIter++;
    if (Ret->Role() == NULL)
        return GetByOrder_NPC();
    return Ret;
}
BaseItemObject* BaseItemObject::GetByOrder_Item(bool IsResetData)
{
    BaseItemObject* Ret;
    if (IsResetData != false)
        _St->SearchIter = _St->ItemObj.begin();
    if (_St->SearchIter == _St->ItemObj.end())
        return NULL;
    Ret = *_St->SearchIter;
    _St->SearchIter++;
    if (Ret->Role() == NULL)
        return GetByOrder_Item();
    return Ret;
}
BaseItemObject* BaseItemObject::GetByOrder_MirrorObj(bool IsResetData)
{
    BaseItemObject* Ret;
    if (IsResetData != false)
        _St->SearchIter_Map = _St->MirrorObj.begin();
    if (_St->SearchIter_Map == _St->MirrorObj.end())
        return NULL;
    Ret = _St->SearchIter_Map->second;
    _St->SearchIter_Map++;
    if (Ret->Role() == NULL)
        return GetByOrder_MirrorObj();
    return Ret;
}
int BaseItemObject::GetmaxZoneID()
{
    return (int)_St->ZoneObj.size();
}
int BaseItemObject::SockID()
{
    if (_Role == NULL)
        return -1;
    return _Role->SockID();
}
int BaseItemObject::ItemID()
{
    if (_Role == NULL)
        return -1;
    return _Role->ItemID();
}
int BaseItemObject::GUID()
{
    if (_Role == NULL)
        return -1;
    return _Role->GUID();
}
BaseItemObject* BaseItemObject::GetObjBySockID(int ID)
{
    if (_St->AllPlayer.size() <= unsigned(ID))
        return NULL;
    BaseItemObject* Temp = _St->AllPlayer[ID];
    if (Temp != NULL && _St->AllPlayer[ID]->_Type != EM_ItemType_Player)
        return NULL;
    return _St->AllPlayer[ID];
}
bool BaseItemObject::SetAccountName(string Name)
{
    typedef map<string, BaseItemObject*>::value_type ValType;
    pair<map<string, BaseItemObject*>::iterator, bool> Ret;
    Ret = _St->AccountNameMap.insert(ValType(Name, this));
    return Ret.second;
}
bool BaseItemObject::ReleaseAccountName()
{
    if (_St->AccountNameMap.erase(Role()->Account_ID()) == 0)
        return false;
    return true;
}
BaseItemObject* BaseItemObject::GetObjByAccountName(string Name)
{
    map<string, BaseItemObject*>::iterator Iter;
    Iter = _St->AccountNameMap.find(Name);
    if (Iter == _St->AccountNameMap.end())
        return NULL;
    return Iter->second;
}
BaseItemObject* BaseItemObject::GetObjByPlayerName(string Name)
{
    set<BaseItemObject*>::iterator Iter;
    for (Iter = _St->PlayerObj.begin(); Iter != _St->PlayerObj.end(); Iter++)
    {
        if (stricmp(Name.c_str(), (*Iter)->Role()->RoleName()) == 0)
            return *Iter;
    }
    return NULL;
}
BaseItemObject* BaseItemObject::GetObjByName(string Name)
{
    for (unsigned int i = 0; i < _St->ZoneObj.size(); i++)
    {
        BaseItemObject* Obj = _St->ZoneObj[i];
        if (Obj == NULL)
            continue;
        if (stricmp(Name.c_str(), Global::GetRoleName(Obj->Role())) == 0)
            return Obj;
    }
    return NULL;
}
void BaseItemObject::ReportStat()
{
    Print(LV2, "BaseItemObject::ReportStat", "------------BaseItemObject ���p�C�L-----------------\n");
    Print(LV2, "BaseItemObject::ReportStat", "_UnUsedWorldGUID=%d  _MaxWorldGUID =%d\n", _St->UnUsedWorldGUID.size(), _St->MaxWorldGUID);
    Print(LV2, "BaseItemObject::ReportStat", "_PlayerObj=%d  _NPCObj=%d _ItemObj=%d _MirrorObj=%d \n", _St->PlayerObj.size(), _St->NPCObj.size(), _St->ItemObj.size(), _St->MirrorObj.size());
}
void BaseItemObject::ReportAllObjInfo()
{
    set<BaseItemObject*>::iterator Iter;
    RoleDataEx* Role;
    Print(LV2, "BaseItemObject::ReportAllObjInfo", "------------BaseItemObject PlayerObj ���p�C�L--------------\n");
    for (Iter = _St->PlayerObj.begin(); Iter != _St->PlayerObj.end(); Iter++)
    {
        Role = (*Iter)->Role();
        Print(LV2, "BaseItemObject::ReportAllObjInfo", "GUID = 0x%x WorldGUID = %d Name = %s\n", Role->GUID(), Role->WorldGUID(), Global::GetRoleName_ASCII(Role).c_str());
    }
    Print(LV2, "BaseItemObject::ReportAllObjInfo", "------------BaseItemObject NPCObj ���p�C�L-----------------\n");
    for (Iter = _St->NPCObj.begin(); Iter != _St->NPCObj.end(); Iter++)
    {
        Role = (*Iter)->Role();
        Print(LV2, "BaseItemObject::ReportAllObjInfo", "GUID = 0x%x ItemObj = %d Name = %s\n", Role->GUID(), Role->WorldGUID(), Global::GetRoleName_ASCII(Role).c_str());
    }
    Print(LV2, "BaseItemObject::ReportAllObjInfo", "------------BaseItemObject ItemObj ���p�C�L----------------\n");
    for (Iter = _St->ItemObj.begin(); Iter != _St->ItemObj.end(); Iter++)
    {
        Role = (*Iter)->Role();
        Print(LV2, "BaseItemObject::ReportAllObjInfo", "GUID = 0x%x WorldGUID = %d Name = %s\n", Role->GUID(), Role->WorldGUID(), Global::GetRoleName_ASCII(Role).c_str());
    }
    map<int, BaseItemObject*>::iterator MIter;
    Print(LV2, "BaseItemObject::ReportAllObjInfo", "------------BaseItemObject MirrorObj ���p�C�L----------------\n");
    for (MIter = _St->MirrorObj.begin(); MIter != _St->MirrorObj.end(); MIter++)
    {
        Role = MIter->second->Role();
        Print(LV2, "BaseItemObject::ReportAllObjInfo", "GUID = 0x%x WorldGUID = %d Name = %s\n", Role->GUID(), Role->WorldGUID(), Global::GetRoleName_ASCII(Role).c_str());
    }
}
bool BaseItemObject::SetMaxWorldGUID(int value)
{
    if (_St->MaxWorldGUID != -1)
    {
        Print(LV2, "SetMaxWorldGUID", "Repetition Set MaxWorldGUID!!");
    }
    _St->MaxWorldGUID = value;
    return true;
}
void BaseItemObject::GetUsedWorldItemIDBlock(set<int>& UsedItemIDSet)
{
    int i;
    for (i = 0; i < (int)_St->ZoneObj.size(); i++)
    {
        BaseItemObject* Obj = _St->ZoneObj[i];
        if (Obj == NULL || Obj->Role() == NULL)
            continue;
        UsedItemIDSet.insert(Obj->Role()->WorldGUID() / _Def_WorldItemID_Base_);
    }
    for (i = 0; i < (int)_St->UnUsedWorldGUID.size(); i++)
    {
        UsedItemIDSet.insert(_St->UnUsedWorldGUID[i] / _Def_WorldItemID_Base_);
    }
}
INT64 BaseItemObject::SimpleSchedularReg(BaseItemObjectFunction Func, int Time)
{
    return _St->SimpleSchedular.Register(Time, this, Func);
}
void BaseItemObject::ClearSimpleSchedular()
{
    _St->SimpleSchedular.DeleteAll(this);
}
IAIBaseClass* BaseItemObject::AI()
{
    return _AI;
}
void BaseItemObject::AI(IAIBaseClass* AIinfo)
{
    if (_AI != NULL)
        delete _AI;
    _AI = AIinfo;
}
BaseItemObject* BaseItemObject::GetObjByDBID(int DBID)
{
    map<int, BaseItemObject*>::iterator Iter;
    Iter = _St->DBIDMap.find(DBID);
    if (Iter == _St->DBIDMap.end())
        return NULL;
    if (Iter->second == NULL || Iter->second->_Role == NULL)
        return NULL;
    return Iter->second;
}
BaseItemObject* BaseItemObject::GetObjByWorldGUID(int WorldGUID)
{
    map<int, BaseItemObject*>::iterator Iter;
    Iter = _St->WorldGUIDMap.find(WorldGUID);
    if (Iter == _St->WorldGUIDMap.end())
        return NULL;
    if (Iter->second == NULL || Iter->second->_Role == NULL)
        return NULL;
    return Iter->second;
}
NPCMoveFlagManageClass* BaseItemObject::NPCMoveInfo()
{
    return _NPCMoveInfo;
}
void BaseItemObject::NPCMoveInfo(int NPCDBID)
{
    _NPCMoveInfo->SetNPCDBID(NPCDBID);
}
void BaseItemObject::SetOrgWorldGUID(int WorldGUID)
{
    _OrgWorldGUID = WorldGUID;
}
int BaseItemObject::OrgWorldGUID()
{
    return _OrgWorldGUID;
}
void BaseItemObject::CreateWorldGUID()
{
    {
        if (_St->MaxWorldGUID == -1)
        {
            Print(Def_PrintLV5, "BaseItemObject", "_MaxWorldGUID == -1 Create BaseItemObject But not Obtains WorldGuildID ???");
            _St->MaxWorldGUID = 0;
        }
    }
    while (_St->UnUsedWorldGUID.size() < 1000)
    {
        _St->UnUsedWorldGUID.push_back(_St->MaxWorldGUID);
        _St->MaxWorldGUID++;
    }
    int WorldID = _St->UnUsedWorldGUID.front();
    _St->UnUsedWorldGUID.pop_front();
    SetWorldGUID(WorldID);
}