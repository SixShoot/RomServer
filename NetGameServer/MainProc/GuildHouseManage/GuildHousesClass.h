#pragma once

#include <map>
#include <vector>
#include "roledata/RoleDataEx.h"
#include "ReaderClass/CreateDBCmdClass.h"
//----------------------------------------------------------------------------------------------
#define _MAX_MAP_POINT_COUNT_ 100

struct TempMatrix4x4Struct
{
	float					_11, _12, _13, _14;
	float					_21, _22, _23, _24;
	float					_31, _32, _33, _34;
	float					_41, _42, _43, _44;
	TempMatrix4x4Struct()
	{
		memset( this , 0 , sizeof(*this));
	}
};

enum GuildHouseObjectStateENUM
{
	EM_GuildHouseObjectState_None		,		//﹟ゼㄏて
	EM_GuildHouseObjectState_Loading	,		//单更い
	EM_GuildHouseObjectState_OK			,		//更ЧΘ
};
//----------------------------------------------------------------------------------------------
struct GroudPointStruct
{
	float	X,Y,Z;
	bool	IsBuildOK;
};
//----------------------------------------------------------------------------------------------
struct MapPointInfoStruct
{
	char	PointStr[32];
	union
	{
		int		Value[2];
		struct  
		{
			int X  , Y;
		};
	};

};
//----------------------------------------------------------------------------------------------
struct GuildHouseBuildingStruct
{
	int		ObjGUID;
	TempMatrix4x4Struct	matWorld;
	vector< int > ChildBuildingDBIDList;
	GuildHouseBuildingInfoStruct	Info;
};
//----------------------------------------------------------------------------------------------
class GuildHousesManageClass
{
protected:
	static map< int , GuildHousesManageClass* > _GuildIDMap;
	
public:
	static bool Init();
	static bool Release();

	static void	OnTimeProcAll();
	static void CalRentAll();
	static GuildHousesManageClass* GetHouseObj( int GuildID );
	static GuildHousesManageClass* CreateHouse( int GuildID , int RoomID );
	static GuildHousesManageClass* GetHouseObj_ByRoom( int RoomID );
	static int  HouseCount()				{ return (int)_GuildIDMap.size();};

	static	GroudPointStruct		_Map[ _MAX_MAP_POINT_COUNT_ ][ _MAX_MAP_POINT_COUNT_ ];
	static	CreateDBCmdClass<GuildHouseBuildingInfoStruct>*		_RDGuildHouseBuildingSql;
	static	CreateDBCmdClass<GuildHouseItemStruct>*				_RDGuildHouseItemSql;
	static	CreateDBCmdClass<GuildHouseFurnitureItemStruct>*	_RDGuildHouseFurnitureItemSql;
	static void ClearAll();

	//眔琘ン把σ翴畒夹
	static bool GetObjPoint( int GItemID , const char* Point , float& outX , float& outY , float& outZ , float& outDir );
protected:
	GuildHouseObjectStateENUM	_State;
	int							_RoomID;
	int							_SaveTime;
	bool						_IsNeedSave;
	GuildHousesInfoStruct		_HouseBase;
	int							_MaxBuildingDBID;
	int							_MaxItemDBID;
	int							_MaxFurnitureDBID;
	int							_LiveTime;
	float						_X , _Y , _Z;
	float						_Dir;

	float						_PX , _PY , _PZ , _PDir;	//產秈竚
	CampID_ENUM					_CampID;					//秈CampID -1 ボぃ矪瞶

	map< int , GuildHouseBuildingStruct >		_BuildingMap;
	map< int , GuildHouseItemStruct >			_ItemBox[ _DEF_GUIDHOUSEITEM_MAX_PAGE_ ];
	map< int , GuildHouseFurnitureItemStruct >	_FurnitureItemMap;
	GuildStorehouseRightStruct					_ItemRight[ _DEF_GUIDHOUSEITEM_MAX_PAGE_ ];

	map< int , vector<GuildItemStoreLogStruct> >	_ItemLog[ _DEF_GUIDHOUSEITEM_MAX_PAGE_ ];		// –ぱLog戈

	vector<int>					_DelBuildingDBID;
	vector<int>					_DelItemDBID;
	vector<int>					_DelFurnitureDBID;

	map< int , int >			_FurnitureDBIDtoObjGUIDMap;


	bool						_BuildingMask[_MAX_MAP_POINT_COUNT_][_MAX_MAP_POINT_COUNT_];

	//秆亩把σ翴﹃
	MapPointInfoStruct			DecondeMapPointStr( const char * Str );
	//眔把σ翴竚
	bool						MapPointStrPos( GuildBuildingInfoTableStruct* BuildInfo , int ParentBuildingDBID , const char* Str , void* matWorld , bool& IsBuild );
	//砞﹚Map Mask
	bool						SetMapMask( GuildBuildingInfoTableStruct* BuildInfo , const char *Point );
	//睲埃Map Mask				
	bool						ClearMapMask( GuildBuildingInfoTableStruct* BuildInfo , const char *Point );
	
	//ミ┬ン
	int							CreateBuildingObj( int OrgObjID , int BuildingDBID , float X , float Y , float Z , float Dir , ObjectModeStruct Mode , int BuildSize , bool IsActive );
	void						CalRent( );
public:
	GuildHousesManageClass( );
	~GuildHousesManageClass( );

	void						SetPostion( float X , float Y , float Z , float Dir );
	void						SetPlayerPostion( float X , float Y , float Z , float Dir );
	void						GetPlayerPostion( float& X , float& Y , float& Z , float& Dir );
	void						SetCampID( CampID_ENUM CampID ) { _CampID = CampID; }
	CampID_ENUM					GetCampID( ) { return _CampID; }

	int							RoomID()	{ return _RoomID; }
	GuildHouseObjectStateENUM	State()		{ return _State;};
	void						LoadOK();
	bool						CheckLoadOK() { return (_State == EM_GuildHouseObjectState_OK); };
	void						SaveProc();
	void						NeedSave()	{ _IsNeedSave = true; };	

	//砞﹚┬膀セ戈
	bool						SetHouseBase( GuildHousesInfoStruct& Info );
	GuildHousesInfoStruct*		GetHouseBase() { return &_HouseBase; };

	int							AddBuilding( GuildHouseBuildingInfoStruct& Building );
	bool						DelBuilding( int BuildingDBID );
	int							BuildingUpgrade( int BuildingDBID , int UpgradeBuildingID );
	bool						RebuildAllBuilding( );
	
	GuildHouseBuildingStruct*	GetBuildingInfo( int BuildingDBID );
	GuildHouseBuildingStruct*	GetBuildingInfo( int ParentBuildingDBID , const char* Point );

	map< int , GuildHouseItemStruct >* ItemBox( int PageID );
	map< int , GuildHouseBuildingStruct >&		BuildingMap(){ return _BuildingMap;};	
	map< int , GuildHouseFurnitureItemStruct >&	FurnitureItemMap(){ return 	_FurnitureItemMap; };

	//眔琘縱单
	int							GetMaxBuildLv( const char* TypeStr );

	//︽砞﹚琘縱竚
	bool						ResetBuildingPos( int BuildingDBID );
	//////////////////////////////////////////////////////////////////////////
	//そ穦畐
	//////////////////////////////////////////////////////////////////////////
	//砞﹚そ穦畐舦
//	bool					SetItemRight( GuildStorehouseRightStruct Right , bool IsLoad );
	bool					SetItemRight( GuildStorehouseRightStruct& Config );
	//更珇
	bool					AddItem( GuildHouseItemStruct& Item );
	//埃ン
	bool					DelItem( int PageID , int Pos );
	//珇ユ传
	bool					SwapItem( RoleDataEx* Role , int PageID1 , int Pos1 , int PageID2 , int Pos2 );			//ㄢ珇が传竚
	//ō砰籔ユ传珇
	bool					SwapItem( RoleDataEx* Role , ItemFieldStruct& bodyItem , int PageID , int Pos , bool isFromBody , int& GetCount );
	//眔畐琘竚戈
	GuildHouseItemStruct*	GetItem( int PageID , int Pos );

	GuildStorehouseRightStruct*	GetItemRight( int PageID );
	
	int						GetLootCount( int PageID , int PlayerDBID );

	//////////////////////////////////////////////////////////////////////////
	//そ穦趁
	//////////////////////////////////////////////////////////////////////////
	//更產珇
	bool					AddFurniture( GuildHouseFurnitureItemStruct& Item );
	//埃產珇
	bool					DelFurniture( int Pos );
	//產竚ユ传
	bool					SwapFurniture( int Pos1 , int Pos2 );
	//ō砰籔產竚ユ传
	bool					SwapFurniture( RoleDataEx* Role , ItemFieldStruct& bodyItem , int Pos , bool isFromBody );
	//眔趁戈
	GuildHouseFurnitureItemStruct* GetFurniture( int Pos );
	//ミ產ン
	//int						CreateBuildingFurniture( int OrgObjID , int FurnitureDBID , House3DLayoutStruct& Layout );
	int						CreateBuildingFurniture( ItemFieldStruct &Item , int FurnitureDBID , House3DLayoutStruct& Layout );
	//眔ン籔產Map
	map< int , int >&		FurnitureDBIDtoObjGUIDMap()	{ return _FurnitureDBIDtoObjGUIDMap; };
	//////////////////////////////////////////////////////////////////////////
	//そ穦畐Log
	//////////////////////////////////////////////////////////////////////////
	vector<GuildItemStoreLogStruct>* GetItemLog( int PageID , int DayBefore );
	bool					 SetItemLog( GuildItemStoreLogStruct& Log );
	void					 AddLog( int PageID , GuildHouseLootTypeENUM Type , int	PlayerDBID , int ItemOrgID , int ItemCount , int FromPageID = -1 , int ToPageID = -1 );
};
