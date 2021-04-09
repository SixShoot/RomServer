#pragma  once
#include "..\Global.h"


class NPCMoveFlagManageClass
{
protected:
	//所有NPC移動點資料
	static map< int , vector< NPC_MoveBaseStruct >* >	_AllNPCMovePointList;
	static vector< NPCMoveFlagManageClass*>				_ObjList;
	static vector< int >								_UnUsedList;

public:

	static bool Init();
	static bool Release();

	//取得某NPC的所有移動資訊
	static vector< NPC_MoveBaseStruct >* GetNPCMoveList( int NPCDBID );

protected:

	int			_GUID;
	int			_NPCDBID;
	vector< NPC_MoveBaseStruct >* _MoveInfo;

public:
	NPCMoveFlagManageClass();
	~NPCMoveFlagManageClass();

	//讓路徑資料聯結
	bool		SetNPCDBID( int NPCDBID );

	vector< NPC_MoveBaseStruct >* MoveInfo() { return _MoveInfo;};
	NPC_MoveBaseStruct* MoveNodeInfo( int Index );
};