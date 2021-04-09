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
//	GM�R�O�޲z���c
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

	//��l��
	GmCommandClass();
	//���U�R�O
	void    RegEvent(ManagementENUM ManageLv, string Name, GmCmdFunction Fun, wstring Help);

	//GM�R�O�B�z
	bool	GMCmdProc(BaseItemObject* OwnerObj, const char* Str, const char* RoleName, ManagementENUM ManageLv, bool checkPassword = false);
	//----------------------------------------------------------------------------------------
	//Help �C�X�Ҧ���GM���O
	bool    GM_Help(BaseItemObject* OwnerObj);
	//�������~
	bool    GM_Give(BaseItemObject* OwnerObj);
	bool    GM_DelBodyItem(BaseItemObject* OwnerObj);
	//����
	bool	GM_GiveMoney(BaseItemObject* OwnerObj);
	bool	GM_GiveMoney_Account(BaseItemObject* OwnerObj);
	bool	GM_GiveMoney_Bonus(BaseItemObject* OwnerObj);
	//�I�s���a
	bool	GM_CallPlayerName(BaseItemObject* OwnerObj);
	//��Y���a
	bool	GM_GotoPlayerName(BaseItemObject* OwnerObj);
	//�䦹�Ϫ��Y����
	bool    GM_GotoID(BaseItemObject* OwnerObj);
	bool	GM_GotoOrgID(BaseItemObject* OwnerObj);

	//�N�ۤv�_�u
	bool	GM_Logout(BaseItemObject* OwnerObj);

	//�j�M����
	bool    GM_SearchObj(BaseItemObject* OwnerObj);
	bool	GM_SearchObjCount(BaseItemObject* OwnerObj);

	//�]�w�ä[���򥻼ƭ�
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

	//�����d��
	bool    GM_CreatePet(BaseItemObject* OwnerObj);
	//�d���R�O
	bool    GM_PetCmd(BaseItemObject* OwnerObj);


	// �]�w Quest �����Υ�����
	bool	GM_QuestFlag(BaseItemObject* OwnerObj);
	//�M���Ҧ� Quest ���
	bool    GM_ClsQuest(BaseItemObject* OwnerObj);

	//����NPC Save & Load
	bool    GM_TestNPCSave(BaseItemObject* OwnerObj);
	bool    GM_TestNPCLoad(BaseItemObject* OwnerObj);

	//����Y�@��
	bool	GM_RunPlot(BaseItemObject* OwnerObj);
	bool    GM_ReloadPlot(BaseItemObject* OwnerObj);
	bool    GM_ReloadPlotEx(BaseItemObject* OwnerObj);
	bool    GM_ReloadObj(BaseItemObject* OwnerObj);
	//���ե�
	bool	GM_Pcall(BaseItemObject* OwnerObj);

	//----------------------------------------------------------------------------------------
	//�I�k
	bool    GM_SpellMagic(BaseItemObject* OwnerObj);

	//�]�w�i���i�H����
	bool    GM_AttackFlag(BaseItemObject* OwnerObj);
	//���Ҧ�NPC�^�쭫���I
	bool    GM_NPCGoHome(BaseItemObject* OwnerObj);
	//�]�wnpc���ʺX��
	bool    GM_NPCMoveFlag(BaseItemObject* OwnerObj);
	//�C�X�Ҧ��u�W���a
	bool	GM_Who(BaseItemObject* OwnerObj);
	//���o���󪩥����
	bool	GM_ObjVersion(BaseItemObject* OwnerObj);

	//�]�w�k�N����
	bool	GM_SetMagicPoint(BaseItemObject* OwnerObj);

	//�]�wKill�@����X��(���ե�)
	bool	GM_SetKillRate(BaseItemObject* OwnerObj);

	//��ﭫ�ͪ��Ҧ�
	bool	GM_SetReviveType(BaseItemObject* OwnerObj);

	//�j�M�A�X���˳�
	bool	GM_ResetEQ(BaseItemObject* OwnerObj);

	//�[�֨t�Φh�֤������ɶ�
	bool	GM_DSysTime(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//�I���ӫ~
	bool	GM_ItemExchange(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//GM����
	bool	GM_GMHide(BaseItemObject* OwnerObj);

	//----------------------------------------------------------------------------------------
	// ����
	//----------------------------------------------------------------------------------------
	//���}��AC
	bool	GM_OpenAC(BaseItemObject* OwnerObj);
	//�����x�s�����I
	bool	GM_TestSaveNPCMove(BaseItemObject* OwnerObj);
	//npc �����I�H�X�l��� 
	bool	GM_ShowNPCMoveFlag(BaseItemObject* OwnerObj);
	//�R���@�t�C���X�l
	bool	GM_DelFlagList(BaseItemObject* OwnerObj);
	//���ն}�Ҵ�������
	bool	GM_ChangeJob(BaseItemObject* OwnerObj);

	//�]�w�s�p���|�ର�������|
	bool	GM_SetGuildReady(BaseItemObject* OwnerObj);

	//Gm Command �}��
	bool	GM_EnabledGMCommand(BaseItemObject* OwnerObj);
	//�ϥθ��|�j�M
	bool	GM_UsePathFind(BaseItemObject* OwnerObj);
	//�]�w����۩w�ٸ�
	bool	GM_SetTitleStr(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//SkillValue�ϥέק缾�a�ޯ�A�ޯ�N�X�Ьd��\n~SkillValue �ޯ�s�� �I��
	bool	GM_SkillValue(BaseItemObject* OwnerObj);
	//SkillFull�����a���ޯ����\n~SkillFull ���a�W��
	bool	GM_SkillFull(BaseItemObject* OwnerObj);
	//CardFull�����a���ޯ����\n~CardFull ���a�W��
	bool	GM_CardFull(BaseItemObject* OwnerObj);

	//Full�i�Ϫ��aHP MP����\n~Full
	bool	GM_Full(BaseItemObject* OwnerObj);
	//CallID(��ϰ�)��Y����I�s�쭱�e\n~CallID �ϰ쪫��ID
	bool	GM_CallID(BaseItemObject* OwnerObj);
	//CallName(��ϰ�)��Y����I�s�쭱�e\n~CallName �ϰ쪫��W��
	bool	GM_CallName(BaseItemObject* OwnerObj);
	//GotoName(��ϰ�)�ǰe��Y����\n~GotoName �ϰ쪫��W��
	bool	GM_GotoName(BaseItemObject* OwnerObj);
	//InfoID�d�߬Y�����T\n~InfoID �ϰ쪫��ID �d�ߧΦ�\n�Ҧp:~InfoID 1 1\n�Φ��N�X\n  0 �W��¾�~\n  1 �������m�O\n  2 �ԲӰ򥻼ƭ�\n  3 ��m\n  4 ��O�@�ӵ��ƭ�\n  5 ���Ŧ�q�g���\n  6 �U���򥻧ޯ�\n  7 �ݭp��X�Ӫ��U���ƭ�)
	bool	GM_InfoID(BaseItemObject* OwnerObj);
	//ResetID�⨭�W���Ҧ��k�N���A����\n~ResetID �ϰ쪫��ID
	bool	GM_ResetMagic(BaseItemObject* OwnerObj);
	//Rename�ק�Y�@�Ө��⪺�W�r\n~Rename ��Ө���W�� ��Ө���W��
	bool	GM_Rename(BaseItemObject* OwnerObj);
	//Kill�����������a��NPC\n~Kill �W��
	bool	GM_KillID(BaseItemObject* OwnerObj);
	//Kick��Y���a��X�C��\n~Kick ���a�W��
	bool	GM_KickID(BaseItemObject* OwnerObj);
	//KickBanID��Y���a��X�C��,�ç�b���ᵲ\n KickBanID �ϰ쪫��ID
	bool	GM_KickBanID(BaseItemObject* OwnerObj);
	//BcastAll���ϼs��\n~BcastAll ���ϼs�������e
	bool	GM_BcastAll(BaseItemObject* OwnerObj);
	//Bcast �ϼs��\n~Bcast �ϼs�������e
	bool	GM_Bcast(BaseItemObject* OwnerObj);
	//DebugPlot�@�������A�]�w�Y�@�Ӫ���]���@���B�J��X���U���O�̡A�̦n�P�ɳ]�w�ؼлP�ۤv�@���`�|�b���a�PNPC���W�ഫ\n~DebugPlot �ϰ쪫��ID �@��ID
	bool	GM_DebugPlot(BaseItemObject* OwnerObj);
	//Faceoff�ܨ����Ǫ�\n�p�ߨ��W�����~�|�ƻs�@�����X��\n�ܨ��ᤣ�ഫ�Ϥ@�w�n�n�X\n~Faceoff �Ǫ�����ID ���᪺�W�r
	bool	GM_FaceOff(BaseItemObject* OwnerObj);
	//GMS.Help	= "�t�ήɶ��]�w\n~SystemTime �ѼƮt �p�ɮt \n~SystemTime 1 1";
	bool	GM_SystemTime(BaseItemObject* OwnerObj);
	//Visibl�]�w�������ζ}���A�������Ϊ��a�d����\n~Hide on\n~Hide off
	bool	GM_Visible(BaseItemObject* OwnerObj);
	//ObjList�C��ϰ쪫����\n~ObjList �}�lID ����ID\n�Ҧp:~ObjList 0 100
	bool	GM_ObjList(BaseItemObject* OwnerObj);
	//OrgObjList�C�� ������
	bool	GM_OrgObjList(BaseItemObject* OwnerObj);
	//NoSpeak�����a�T���X�����A�U�u��N����F\n~NoSpeak ���a�W�� ���
	//SearchOrgObj�M���l����W�����Y�@�Ӧr�ꪺ�R�O�A�ó]�w�����\n~SearchOrgObj �r�� ���O\n�r�� : (*) �N������A�䥦�u�n����W�������禹�r��N��\n���O : ����N������A�䥦�i��(�Z��,����,�D��,�k�N,NPC,�d��)(�H�]�P)(GM,�C,..)\n �Ҧp : ~SearchOrgObj �M �Z���]�M
	bool	GM_SearchOrgObj(BaseItemObject* OwnerObj);
	//BindingItemMagic�]�w�Y�@�Ӫ���ϥΪ��ĪG�A(������쥻�ݭn�i�H�ϥ�)\n~BindingItemMagic ���~ID �k�NID
	bool	GM_BindingItemMagic(BaseItemObject* OwnerObj);
	//MaxOnline�]�w�̰��W�u�H��\n~MaxOnline �̰��W�u�H��
	bool	GM_MaxOnline(BaseItemObject* OwnerObj);
	//GameTime�]�w�C�������ɶ� \n~GameTime �����ɶ�(�p��)
	bool	GM_GameTime(BaseItemObject* OwnerObj);
	//TimeSpeed �[�t�C�� x1 x2..
	bool	GM_TimeSpeed(BaseItemObject* OwnerObj);
	//CallSpell �]�w���ϰ�O�_�i�H�I�I�s�P�񪺪k�N\n��~CallSpell false
	bool	GM_CallSpell(BaseItemObject* OwnerObj);
	//ResitColdown ��k�N��Coldown �ɶ��M��\n��~ResitColdown
	bool	GM_ResetColdown(BaseItemObject* OwnerObj);
	bool	GM_NewResetColdown(BaseItemObject* OwnerObj);
	//SetFightID �]�w�s��Ԫ��էO\n��~SetFightID ����ID �էO \n~SetFightID 100 10
	bool	GM_SetFightID(BaseItemObject* OwnerObj);
	//��ܨ���Ҿ֦����S���~
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

	//�üƲ��ͺX�l
	bool	GM_RandCreateFlag(BaseItemObject* OwnerObj);

	//
	bool	GM_ClearGuildWarTime(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//�]�w�n���olua���~��T
	bool    GM_GetLuaErrorInfo(BaseItemObject* OwnerObj);

	//�إߩҦ����P�� �Ϧ檺NPC 
	bool	GM_CreateImageNPC(BaseItemObject* OwnerObj);

	//�üƸ���Y�Ӫ����m
	bool	GM_TestRandTransport(BaseItemObject* OwnerObj);


	bool	GM_RoleRight(BaseItemObject* OwnerObj);

	bool	GM_KickAll(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	bool	GM_CompareTarget(BaseItemObject* OwnerObj);

	//�R���Ҧ��S���x�s������
	bool	GM_DelUnSaveObject(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//�Ҧ��H��
	bool	GM_ZonePlayerCount(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	bool	GM_SetBTI(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//�}��PK
	bool	GM_PKFlag(BaseItemObject* OwnerObj);
	//�M���Ҧ����ٸ�
	bool	GM_ClearTitle(BaseItemObject* OwnerObj);
	//�M���Ҧ����p��
	bool	GM_ClearHuntingCount(BaseItemObject* OwnerObj);

	//���o�Y������������
	bool	GM_GetObjectInfo(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//���m�C�����
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
	//�]�w�ϰ�L�k�n�J
	bool	GM_StopLogin(BaseItemObject* OwnerObj);

	//�gPing �� Log
	bool	GM_PingLog(BaseItemObject* OwnerObj);
	bool	GM_ProxyPingLog(BaseItemObject* OwnerObj);
	bool	GM_SrvProcTimeLog(BaseItemObject* OwnerObj);
	//�]�wINI
	bool	GM_SetIni(BaseItemObject* OwnerObj);
	bool	GM_RunLottery(BaseItemObject* OwnerObj);
	bool	GM_SetChangeGuild(BaseItemObject* OwnerObj);

	//�]�w�Ȫ������t��
	bool	GM_PlantGrowRate(BaseItemObject* OwnerObj);

	//�]�w���|�ξԳ��ثe���ɶ�
	bool	GM_GuildWarTime(BaseItemObject* OwnerObj);

	//SQL
	bool	GM_SQLCmd(BaseItemObject* OwnerObj);

	//���ռ@���Ȱ�
	bool	GM_TestLuaPause(BaseItemObject* OwnerObj);
	//���ռ@���~��
	bool	GM_TestLuaStart(BaseItemObject* OwnerObj);

	bool	GM_CheckGuildNameRules(BaseItemObject* OwnerObj);

	//�]�w���w�����|�|��
//	bool	GM_SetGuildLeader	( BaseItemObject* OwnerObj );
	bool	GM_DelGuild(BaseItemObject* OwnerObj);

	bool	GM_CheckMem(BaseItemObject* OwnerObj);
	//�M���ƥ��i��
	bool	GM_ClearInstanceInfo(BaseItemObject* OwnerObj);

	//�]�w�W�D�֦���
	bool	GM_SetChannelOwner(BaseItemObject* OwnerObj);
	//----------------------------------------------------------------------------------------
	//PE ���ե�GM�R�O��

	//�M�����wPE���]�w
	bool	GM_PE_ReleasePE(BaseItemObject* OwnerObj);

	//���m�Y�@��PE
	bool	GM_PE_Reset(BaseItemObject* OwnerObj);

	//���o�S�wPE�����A
	bool	GM_PE_GetStatus(BaseItemObject* OwnerObj);

	//���o�S�wPE���]�w
	bool	GM_PE_GetDetail(BaseItemObject* OwnerObj);

	//�j��PE��?
	bool	GM_PE_InstSucc(BaseItemObject* OwnerObj);

	//�j��PE����
	bool	GM_PE_InstFail(BaseItemObject* OwnerObj);

	//�j����D��Y�@��Phase
	bool	GM_PE_JumpToPH(BaseItemObject* OwnerObj);

	//�j��Phase��?
	bool	GM_PE_PH_InstSucc(BaseItemObject* OwnerObj);

	//�j��Phase����
	bool	GM_PE_PH_InstFail(BaseItemObject* OwnerObj);

	//�j��Objective��?
	bool	GM_PE_OB_InstAchieve(BaseItemObject* OwnerObj);

	//�C�X�Ҧ����ܼ�
	bool	GM_PE_ListAllVar(BaseItemObject* OwnerObj);

	//���oPE�άY���ܼƭ� 
	bool	GM_PE_GetVar(BaseItemObject* OwnerObj);

	//�]�wPE�άY���ܼƭ�
	bool	GM_PE_SetVar(BaseItemObject* OwnerObj);

	//�[PE�άY���ܼƭ�
	bool	GM_PE_AddVar(BaseItemObject* OwnerObj);

	//�����w�����a�[��
	bool	GM_PE_GiveScore(BaseItemObject* OwnerObj);

	//���o���w�����a������
	bool	GM_PE_GetScore(BaseItemObject* OwnerObj);

	//��������Ҥ������~���
	bool	GM_CancelAcSellItem(BaseItemObject* OwnerObj);

	// ���o���w����ƭ�
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