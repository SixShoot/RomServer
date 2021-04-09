#include "NPCMoveFlagManage.h"

//所有NPC移動點資料
map< int , vector< NPC_MoveBaseStruct >* >  NPCMoveFlagManageClass::_AllNPCMovePointList;
vector< NPCMoveFlagManageClass*>			NPCMoveFlagManageClass::_ObjList;
vector< int >								NPCMoveFlagManageClass::_UnUsedList;

bool NPCMoveFlagManageClass::Init()
{
	return true;
}
bool NPCMoveFlagManageClass::Release()
{
	map< int , vector< NPC_MoveBaseStruct >* >::iterator Iter;

	for( Iter = _AllNPCMovePointList.begin() ; Iter != _AllNPCMovePointList.end() ; Iter++ )
	{
		if( Iter->second != NULL )
		{
			delete Iter->second;
			Iter->second = NULL;
		}
	}

	for( unsigned int i = 0 ; i < _ObjList.size() ; i++ )
	{
		if( _ObjList[i] != NULL )
		{
			_ObjList[i] = NULL;
			delete _ObjList[i];
		}
	}
	return true;
}

vector< NPC_MoveBaseStruct >* NPCMoveFlagManageClass::GetNPCMoveList( int DBID )
{
	if( DBID < 0 )
		return NULL;

	map< int , vector< NPC_MoveBaseStruct >* >::iterator Iter;
	vector< NPC_MoveBaseStruct >* Data;

	Iter = _AllNPCMovePointList.find( DBID );
	if( Iter == _AllNPCMovePointList.end() )
	{
		Data = NEW(vector< NPC_MoveBaseStruct >);

		_AllNPCMovePointList[ DBID ] = Data;
	}
	else
	{
		Data = Iter->second;
	}

	return Data;
}
//---------------------------------------------------------------------------------------------------------------------
NPCMoveFlagManageClass::NPCMoveFlagManageClass()
{
	if( _UnUsedList.size() == 0 )
	{
		_UnUsedList.push_back( (int)_ObjList.size() );
		_ObjList.push_back( NULL );
	}

	_GUID = _UnUsedList.back();
	_UnUsedList.pop_back();
	_ObjList[ _GUID ] = this;


	_MoveInfo = NEW(vector< NPC_MoveBaseStruct >) ;
	_NPCDBID = -1;
}

NPCMoveFlagManageClass::~NPCMoveFlagManageClass()
{

	_ObjList[ _GUID ] = NULL;
	_UnUsedList.push_back( _GUID );

	if( _NPCDBID == -1 )
	{
		delete _MoveInfo;
	}
}

//讓路徑資料聯結
bool		NPCMoveFlagManageClass::SetNPCDBID( int NPCDBID )
{
	if( _NPCDBID != -1 )
		return false;

	delete _MoveInfo;

	_NPCDBID = NPCDBID;
	_MoveInfo = GetNPCMoveList( NPCDBID );

	return true;
}

NPC_MoveBaseStruct* NPCMoveFlagManageClass::MoveNodeInfo( int Index )
{
	if( (unsigned)Index >= _MoveInfo->size() )
		return NULL;
	return &( (*_MoveInfo)[Index] );
}