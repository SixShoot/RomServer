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
	EM_GuildHouseObjectState_None		,		//�|����Ϥ�
	EM_GuildHouseObjectState_Loading	,		//���ݸ��J��
	EM_GuildHouseObjectState_OK			,		//���J����
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

	//���o�Y���󪺰Ѧ��I���@�ɮy��
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

	float						_PX , _PY , _PZ , _PDir;	//���a�i�J��m
	CampID_ENUM					_CampID;					//�i�J�̪�CampID -1 ��ܤ��B�z

	map< int , GuildHouseBuildingStruct >		_BuildingMap;
	map< int , GuildHouseItemStruct >			_ItemBox[ _DEF_GUIDHOUSEITEM_MAX_PAGE_ ];
	map< int , GuildHouseFurnitureItemStruct >	_FurnitureItemMap;
	GuildStorehouseRightStruct					_ItemRight[ _DEF_GUIDHOUSEITEM_MAX_PAGE_ ];

	map< int , vector<GuildItemStoreLogStruct> >	_ItemLog[ _DEF_GUIDHOUSEITEM_MAX_PAGE_ ];		// �C�Ѫ�Log���

	vector<int>					_DelBuildingDBID;
	vector<int>					_DelItemDBID;
	vector<int>					_DelFurnitureDBID;

	map< int , int >			_FurnitureDBIDtoObjGUIDMap;


	bool						_BuildingMask[_MAX_MAP_POINT_COUNT_][_MAX_MAP_POINT_COUNT_];

	//��Ķ�Ѧ��I�r��
	MapPointInfoStruct			DecondeMapPointStr( const char * Str );
	//���o�Ѧ��I��m
	bool						MapPointStrPos( GuildBuildingInfoTableStruct* BuildInfo , int ParentBuildingDBID , const char* Str , void* matWorld , bool& IsBuild );
	//�]�wMap Mask
	bool						SetMapMask( GuildBuildingInfoTableStruct* BuildInfo , const char *Point );
	//�M��Map Mask				
	bool						ClearMapMask( GuildBuildingInfoTableStruct* BuildInfo , const char *Point );
	
	//�إߩЫΪ���
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

	//�]�w�Ыΰ򥻸��
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

	//���o�Y�ӫؿv��������
	int							GetMaxBuildLv( const char* TypeStr );

	//����]�w�Y�ӫؿv����m
	bool						ResetBuildingPos( int BuildingDBID );
	//////////////////////////////////////////////////////////////////////////
	//���|�ܮw
	//////////////////////////////////////////////////////////////////////////
	//�]�w���|�ܮw�s���v��
//	bool					SetItemRight( GuildStorehouseRightStruct Right , bool IsLoad );
	bool					SetItemRight( GuildStorehouseRightStruct& Config );
	//���J���~
	bool					AddItem( GuildHouseItemStruct& Item );
	//�R������
	bool					DelItem( int PageID , int Pos );
	//���~�洫
	bool					SwapItem( RoleDataEx* Role , int PageID1 , int Pos1 , int PageID2 , int Pos2 );			//�⪫�~������m
	//����P�Τl�洫���~
	bool					SwapItem( RoleDataEx* Role , ItemFieldStruct& bodyItem , int PageID , int Pos , bool isFromBody , int& GetCount );
	//���o�ܮw�Y��m�����
	GuildHouseItemStruct*	GetItem( int PageID , int Pos );

	GuildStorehouseRightStruct*	GetItemRight( int PageID );
	
	int						GetLootCount( int PageID , int PlayerDBID );

	//////////////////////////////////////////////////////////////////////////
	//���|�í�
	//////////////////////////////////////////////////////////////////////////
	//���J�a�Ѫ��~
	bool					AddFurniture( GuildHouseFurnitureItemStruct& Item );
	//�R���a�Ѫ��~
	bool					DelFurniture( int Pos );
	//�a�Ѧ�m�洫
	bool					SwapFurniture( int Pos1 , int Pos2 );
	//����P�a�Ѧ�m�洫
	bool					SwapFurniture( RoleDataEx* Role , ItemFieldStruct& bodyItem , int Pos , bool isFromBody );
	//���o�íѸ��
	GuildHouseFurnitureItemStruct* GetFurniture( int Pos );
	//�إ߮a�Ѫ���
	//int						CreateBuildingFurniture( int OrgObjID , int FurnitureDBID , House3DLayoutStruct& Layout );
	int						CreateBuildingFurniture( ItemFieldStruct &Item , int FurnitureDBID , House3DLayoutStruct& Layout );
	//���o����P�a�Ѫ�Map
	map< int , int >&		FurnitureDBIDtoObjGUIDMap()	{ return _FurnitureDBIDtoObjGUIDMap; };
	//////////////////////////////////////////////////////////////////////////
	//���|�ܮwLog
	//////////////////////////////////////////////////////////////////////////
	vector<GuildItemStoreLogStruct>* GetItemLog( int PageID , int DayBefore );
	bool					 SetItemLog( GuildItemStoreLogStruct& Log );
	void					 AddLog( int PageID , GuildHouseLootTypeENUM Type , int	PlayerDBID , int ItemOrgID , int ItemCount , int FromPageID = -1 , int ToPageID = -1 );
};
