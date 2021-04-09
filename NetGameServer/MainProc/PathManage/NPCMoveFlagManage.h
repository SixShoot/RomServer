#pragma  once
#include "..\Global.h"


class NPCMoveFlagManageClass
{
protected:
	//�Ҧ�NPC�����I���
	static map< int , vector< NPC_MoveBaseStruct >* >	_AllNPCMovePointList;
	static vector< NPCMoveFlagManageClass*>				_ObjList;
	static vector< int >								_UnUsedList;

public:

	static bool Init();
	static bool Release();

	//���o�YNPC���Ҧ����ʸ�T
	static vector< NPC_MoveBaseStruct >* GetNPCMoveList( int NPCDBID );

protected:

	int			_GUID;
	int			_NPCDBID;
	vector< NPC_MoveBaseStruct >* _MoveInfo;

public:
	NPCMoveFlagManageClass();
	~NPCMoveFlagManageClass();

	//�����|����p��
	bool		SetNPCDBID( int NPCDBID );

	vector< NPC_MoveBaseStruct >* MoveInfo() { return _MoveInfo;};
	NPC_MoveBaseStruct* MoveNodeInfo( int Index );
};