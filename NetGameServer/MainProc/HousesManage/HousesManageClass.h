#pragma once
//#include "roledata/RoleBaseDef.h"
#include "roledata/RoleDataEx.h"
#include "RecycleBin/recyclebin.h"
#include "ReaderClass/CreateDBCmdClass.h"

enum HouseItemSaveStateENUM
{
	EM_HouseItemSaveState_Empty		,	//空的
	EM_HouseItemSaveState_Normal	,	//一般狀態，不須任何處理
	EM_HouseItemSaveState_Update	,	//須要更新
	EM_HouseItemSaveState_Insert	,	//需要Insert
};

struct HouseItemStruct
{
	HouseItemDBStruct			Info;
	vector<HouseItemStruct*>	ChildList;
	HouseItemSaveStateENUM		SaveState;

	HouseItemStruct()
	{
		Init();
	}

	void Init()
	{
		Info.Init();
		ChildList.clear();
		SaveState  = EM_HouseItemSaveState_Empty;
	}
};

struct HousesInfoStruct
{
	HouseBaseDBStruct							Info;
	map< int , vector< HouseItemStruct* > >		ItemList;
	bool										IsNeedSave;

	HousesInfoStruct()
	{
		IsNeedSave = false;
		Info.Init();		
	}
};

enum HouseObjectStateENUM
{
	EM_HouseObjectState_None	,		//尚未初使化
	EM_HouseObjectState_Loading	,		//等待載入中
	EM_HouseObjectState_OK		,		//載入完成
};

//////////////////////////////////////////////////////////////////////////
//房屋效果
struct FurnitureAbilityStruct
{
	bool	IsReady;
	bool	IsRecalculate;
	
	int		ExpWeighted;
	int		TpExpWeighted;
	int		BlackSmithWeighted;
	int 	CarpenterWeighted;
	int 	ArmorWeighted;
	int 	TailorWeighted;
	int 	AlchemyWeighted;
	int		CookWeighted;

	float 	ExpRate;
	float 	TpExpRate;
	float 	BlackSmithRate;
	float 	CarpenterRate;
	float 	ArmorRate;
	float 	TailorRate;
	float 	AlchemyRate;
	float 	CookRate;

	FurnitureAbilityStruct()
	{
		Init();
	}

	void Init()
	{
		memset( this , 0 , sizeof(*this) );
	}
};
//////////////////////////////////////////////////////////////////////////

class HousesManageClass
{
protected:
	static HousesManageClass*							_This;
	static map< int , HousesManageClass* >				_HouseList_DBID;			//房屋列表
	static RecycleBin< HouseItemStruct >				_HouseItemRecycle;
	static CreateDBCmdClass<HouseBaseDBStruct>*		   	_RDHouseBaseSql;
	static CreateDBCmdClass<HouseItemDBStruct>*		   	_RDHouseItemSql;
	static int											_LastProcHouseAddExpTime;

public:
	static float  G_PlantGrowRate;
	static bool Init( );
	static bool Release( );
	static HousesManageClass* This(){ return _This; };
	static HousesManageClass* GetHouseObj( int HouseDBID );
	static HousesManageClass* GetHouseObj_ByRoom( int RoomID );
	static HousesManageClass* CreateHouse( int HouseDBID , int RoomID );
	static int  HouseCount()				{ return (int)_HouseList_DBID.size();};
	

	static void OnTimeProcAll( bool IsSaveAll = false );	//定時處理所有的房屋
protected:
	HousesInfoStruct						_HouseBase;
	HouseObjectStateENUM					_State;
	int										_RoomID;			//處理的房間
	int										_SaveTime;
	bool									_IsNeedSave;
	map< int , HouseItemStruct* >			_ItemList_DBID;
	vector< int >							_DelItemDBIDList;
	int										_LiveTime;			//生存時間
	int										_OwnerID;			//擁有者
	bool									_IsReady;
	int										_OffLineTime;
	int										_MaxItemDBID;
	vector< int >							_FriendDBIDList;

	FurnitureAbilityStruct					_FurnitureAbility;

	void									_DelAllItem( vector< HouseItemStruct* >& ItemList );
public:
	HousesManageClass();
	~HousesManageClass();

	vector< int >&			FriendDBIDList(){ return _FriendDBIDList; };
	bool					CheckFriend( int DBID );
	
	//設定房屋基本資料
	bool					SetHouseBase( HouseBaseDBStruct& info );	//設定為載入OK狀態
	HousesInfoStruct*		GetHouseBase() { return &_HouseBase; };

	FurnitureAbilityStruct&	FurnitureAbility() { return _FurnitureAbility;};

	//放入新的物件
	bool					AddItem( HouseItemDBStruct& info );
	//刪除物件
	bool					DelItem( int itemDBID );
	//物品交換
	bool					SwapItem( int parentItemDBID1 , int pos1 , int parentItemDBID2 , int pos2 );			//兩物品互換位置
	//身體與屋子交換物品
	bool					SwapItem( ItemFieldStruct& bodyItem , int parentItemDBID , int pos , bool isFromBody );

	//取出某欄位(如果是空的會產生)
	HouseItemStruct*		GetItem( int parentItemDBID , int pos );
	HouseItemStruct*		GetItem( int itemDBID );

	HouseItemStruct**		GetItem_Ref( int parentItemDBID , int pos );

	void					SetOwnerID( int ownerID ) { _OwnerID = ownerID; };


	void					CalOffLineTime( unsigned LogoutTime );

	//查看是否有子物件
	bool					CheckChildItem( HouseItemStruct* houseItem );

	//檢查欄位是否可以放置
	bool					CheckItemPos( int parentItemDBID , int pos );

	int						RoomID()	{ return _RoomID; };
	HousesInfoStruct&		HouseBase()	{ return _HouseBase; };
	HouseObjectStateENUM	State()		{ return _State; };
	void					LoadOK()	{ _State = EM_HouseObjectState_OK; };
	bool					IsLoadOK()	{ return _State == EM_HouseObjectState_OK;};

	void					SetSaveInfo();

	void					SaveProc();
	void					SaveHouseBaseProc();
	void					SaveAll( vector< HouseItemStruct* >& itemList );

	//計算租金
	void					CalRental( bool IsNeedSendtoClient = false );

	//女僕資料處理			
	void					ServantProc( );

	//計算家具能力
	void					CalFurnitureAbility();

	//檢查並修正複製物品
	void					CheckAndClearCopyItem( RoleDataEx *Owner );
	void					AddTpExp( int Time );
	//-------------------------------------------------------------------------------------------
	//	盆栽處理
	//-------------------------------------------------------------------------------------------
	static bool				PlantItemGrowProc( ItemFieldStruct& Item );
	static int				DHealthPoint( int DLv );
	int						PlantItemCount( );

};