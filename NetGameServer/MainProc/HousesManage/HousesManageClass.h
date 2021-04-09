#pragma once
//#include "roledata/RoleBaseDef.h"
#include "roledata/RoleDataEx.h"
#include "RecycleBin/recyclebin.h"
#include "ReaderClass/CreateDBCmdClass.h"

enum HouseItemSaveStateENUM
{
	EM_HouseItemSaveState_Empty		,	//�Ū�
	EM_HouseItemSaveState_Normal	,	//�@�몬�A�A��������B�z
	EM_HouseItemSaveState_Update	,	//���n��s
	EM_HouseItemSaveState_Insert	,	//�ݭnInsert
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
	EM_HouseObjectState_None	,		//�|����Ϥ�
	EM_HouseObjectState_Loading	,		//���ݸ��J��
	EM_HouseObjectState_OK		,		//���J����
};

//////////////////////////////////////////////////////////////////////////
//�ЫήĪG
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
	static map< int , HousesManageClass* >				_HouseList_DBID;			//�ЫΦC��
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
	

	static void OnTimeProcAll( bool IsSaveAll = false );	//�w�ɳB�z�Ҧ����Ы�
protected:
	HousesInfoStruct						_HouseBase;
	HouseObjectStateENUM					_State;
	int										_RoomID;			//�B�z���ж�
	int										_SaveTime;
	bool									_IsNeedSave;
	map< int , HouseItemStruct* >			_ItemList_DBID;
	vector< int >							_DelItemDBIDList;
	int										_LiveTime;			//�ͦs�ɶ�
	int										_OwnerID;			//�֦���
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
	
	//�]�w�Ыΰ򥻸��
	bool					SetHouseBase( HouseBaseDBStruct& info );	//�]�w�����JOK���A
	HousesInfoStruct*		GetHouseBase() { return &_HouseBase; };

	FurnitureAbilityStruct&	FurnitureAbility() { return _FurnitureAbility;};

	//��J�s������
	bool					AddItem( HouseItemDBStruct& info );
	//�R������
	bool					DelItem( int itemDBID );
	//���~�洫
	bool					SwapItem( int parentItemDBID1 , int pos1 , int parentItemDBID2 , int pos2 );			//�⪫�~������m
	//����P�Τl�洫���~
	bool					SwapItem( ItemFieldStruct& bodyItem , int parentItemDBID , int pos , bool isFromBody );

	//���X�Y���(�p�G�O�Ū��|����)
	HouseItemStruct*		GetItem( int parentItemDBID , int pos );
	HouseItemStruct*		GetItem( int itemDBID );

	HouseItemStruct**		GetItem_Ref( int parentItemDBID , int pos );

	void					SetOwnerID( int ownerID ) { _OwnerID = ownerID; };


	void					CalOffLineTime( unsigned LogoutTime );

	//�d�ݬO�_���l����
	bool					CheckChildItem( HouseItemStruct* houseItem );

	//�ˬd���O�_�i�H��m
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

	//�p�⯲��
	void					CalRental( bool IsNeedSendtoClient = false );

	//�k����ƳB�z			
	void					ServantProc( );

	//�p��a���O
	void					CalFurnitureAbility();

	//�ˬd�íץ��ƻs���~
	void					CheckAndClearCopyItem( RoleDataEx *Owner );
	void					AddTpExp( int Time );
	//-------------------------------------------------------------------------------------------
	//	�֮�B�z
	//-------------------------------------------------------------------------------------------
	static bool				PlantItemGrowProc( ItemFieldStruct& Item );
	static int				DHealthPoint( int DLv );
	int						PlantItemCount( );

};