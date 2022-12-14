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
	EM_GuildHouseObjectState_None		,		//尚未初使化
	EM_GuildHouseObjectState_Loading	,		//等待載入中
	EM_GuildHouseObjectState_OK			,		//載入完成
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

	//取得某物件的參考點的世界座標
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

	float						_PX , _PY , _PZ , _PDir;	//玩家進入位置
	CampID_ENUM					_CampID;					//進入者的CampID -1 表示不處理

	map< int , GuildHouseBuildingStruct >		_BuildingMap;
	map< int , GuildHouseItemStruct >			_ItemBox[ _DEF_GUIDHOUSEITEM_MAX_PAGE_ ];
	map< int , GuildHouseFurnitureItemStruct >	_FurnitureItemMap;
	GuildStorehouseRightStruct					_ItemRight[ _DEF_GUIDHOUSEITEM_MAX_PAGE_ ];

	map< int , vector<GuildItemStoreLogStruct> >	_ItemLog[ _DEF_GUIDHOUSEITEM_MAX_PAGE_ ];		// 每天的Log資料

	vector<int>					_DelBuildingDBID;
	vector<int>					_DelItemDBID;
	vector<int>					_DelFurnitureDBID;

	map< int , int >			_FurnitureDBIDtoObjGUIDMap;


	bool						_BuildingMask[_MAX_MAP_POINT_COUNT_][_MAX_MAP_POINT_COUNT_];

	//解譯參考點字串
	MapPointInfoStruct			DecondeMapPointStr( const char * Str );
	//取得參考點位置
	bool						MapPointStrPos( GuildBuildingInfoTableStruct* BuildInfo , int ParentBuildingDBID , const char* Str , void* matWorld , bool& IsBuild );
	//設定Map Mask
	bool						SetMapMask( GuildBuildingInfoTableStruct* BuildInfo , const char *Point );
	//清除Map Mask				
	bool						ClearMapMask( GuildBuildingInfoTableStruct* BuildInfo , const char *Point );
	
	//建立房屋物件
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

	//設定房屋基本資料
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

	//取得某個建築物的等級
	int							GetMaxBuildLv( const char* TypeStr );

	//重行設定某個建築的位置
	bool						ResetBuildingPos( int BuildingDBID );
	//////////////////////////////////////////////////////////////////////////
	//公會倉庫
	//////////////////////////////////////////////////////////////////////////
	//設定公會倉庫存取權限
//	bool					SetItemRight( GuildStorehouseRightStruct Right , bool IsLoad );
	bool					SetItemRight( GuildStorehouseRightStruct& Config );
	//載入物品
	bool					AddItem( GuildHouseItemStruct& Item );
	//刪除物件
	bool					DelItem( int PageID , int Pos );
	//物品交換
	bool					SwapItem( RoleDataEx* Role , int PageID1 , int Pos1 , int PageID2 , int Pos2 );			//兩物品互換位置
	//身體與屋子交換物品
	bool					SwapItem( RoleDataEx* Role , ItemFieldStruct& bodyItem , int PageID , int Pos , bool isFromBody , int& GetCount );
	//取得倉庫某位置的資料
	GuildHouseItemStruct*	GetItem( int PageID , int Pos );

	GuildStorehouseRightStruct*	GetItemRight( int PageID );
	
	int						GetLootCount( int PageID , int PlayerDBID );

	//////////////////////////////////////////////////////////////////////////
	//公會傢俱
	//////////////////////////////////////////////////////////////////////////
	//載入家俱物品
	bool					AddFurniture( GuildHouseFurnitureItemStruct& Item );
	//刪除家俱物品
	bool					DelFurniture( int Pos );
	//家俱位置交換
	bool					SwapFurniture( int Pos1 , int Pos2 );
	//身體與家俱位置交換
	bool					SwapFurniture( RoleDataEx* Role , ItemFieldStruct& bodyItem , int Pos , bool isFromBody );
	//取得傢俱資料
	GuildHouseFurnitureItemStruct* GetFurniture( int Pos );
	//建立家俱物件
	//int						CreateBuildingFurniture( int OrgObjID , int FurnitureDBID , House3DLayoutStruct& Layout );
	int						CreateBuildingFurniture( ItemFieldStruct &Item , int FurnitureDBID , House3DLayoutStruct& Layout );
	//取得物件與家俱的Map
	map< int , int >&		FurnitureDBIDtoObjGUIDMap()	{ return _FurnitureDBIDtoObjGUIDMap; };
	//////////////////////////////////////////////////////////////////////////
	//公會倉庫Log
	//////////////////////////////////////////////////////////////////////////
	vector<GuildItemStoreLogStruct>* GetItemLog( int PageID , int DayBefore );
	bool					 SetItemLog( GuildItemStoreLogStruct& Log );
	void					 AddLog( int PageID , GuildHouseLootTypeENUM Type , int	PlayerDBID , int ItemOrgID , int ItemCount , int FromPageID = -1 , int ToPageID = -1 );
};
