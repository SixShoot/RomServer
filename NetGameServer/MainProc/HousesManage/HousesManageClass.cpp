#include "HousesManageClass.h"
#include "../Global.h"
#include "../../netwalker_member/NetWakerGSrvInc.h"
#include <iostream>
#pragma warning (disable:4018)

#define _DEF_HOUSES_SAVETIME_	(10*1000)
//#define _DEF_HOUSES_LIFETIME_	(60*5*1000)

HousesManageClass*						HousesManageClass::_This = NULL;
map< int , HousesManageClass* >			HousesManageClass::_HouseList_DBID;			//房屋列表
//int										HousesManageClass::_MaxItemDBID = 0;
RecycleBin< HouseItemStruct >			HousesManageClass::_HouseItemRecycle;
//vector< int >							HousesManageClass::_RecycleID;
CreateDBCmdClass<HouseBaseDBStruct>*	HousesManageClass::_RDHouseBaseSql;
CreateDBCmdClass<HouseItemDBStruct>*	HousesManageClass::_RDHouseItemSql;
int										HousesManageClass::_LastProcHouseAddExpTime = 0;
float 									HousesManageClass::G_PlantGrowRate = 1.0f;
//////////////////////////////////////////////////////////////////////////
bool HousesManageClass::Init( )
{
	_RDHouseBaseSql		= NEW CreateDBCmdClass<HouseBaseDBStruct> ( RoleDataEx::ReaderRD_HouseBase()  , "Houses_Base" );
	_RDHouseItemSql		= NEW CreateDBCmdClass<HouseItemDBStruct> ( RoleDataEx::ReaderRD_HouseItem()  , "Houses_Item" );
	return true;
}

bool HousesManageClass::Release( )
{
	delete _RDHouseBaseSql;
	delete _RDHouseItemSql;
	return true;
}

HousesManageClass* HousesManageClass::GetHouseObj_ByRoom( int RoomID )
{
	map< int , HousesManageClass* >::iterator Iter;
	for( Iter = _HouseList_DBID.begin() ; Iter != _HouseList_DBID.end() ; Iter++ )
	{
		if( Iter->second->RoomID() == RoomID )
			return Iter->second;
	}

	return NULL;
}

HousesManageClass* HousesManageClass::GetHouseObj( int houseDBID )
{
	map< int , HousesManageClass* >::iterator Iter;
	Iter = _HouseList_DBID.find( houseDBID );
	if( Iter == _HouseList_DBID.end() )
		return NULL;
	return Iter->second;
}

HousesManageClass*	HousesManageClass::CreateHouse( int houseDBID , int roomID )
{
	HousesManageClass* houseClass = GetHouseObj( houseDBID );
	if( houseClass != NULL )
		return NULL;

	houseClass = NEW(HousesManageClass);
	houseClass->_State = EM_HouseObjectState_Loading;
	houseClass->_HouseBase.Info.HouseDBID = houseDBID;
	houseClass->_RoomID = roomID;	
	
	_HouseList_DBID[ houseDBID ] = houseClass;
	return houseClass;
}
//定時處理所有的房屋(10秒一次)
void HousesManageClass::OnTimeProcAll( bool IsSaveAll )	
{
	static int ProcCount = 0;
	ProcCount++;

	vector< int > roomPlayerCountList;
	vector< HousesManageClass* > delList;

	for( int i = 0 ; i < Global::Ini()->RoomCount ; i++ )
	{
		roomPlayerCountList.push_back( 0 );
	}

	set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	//計算每個房間的人數
	for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
	{
		BaseItemObject* Obj = *PlayerObjIter;
		if( Obj == NULL )
			continue;
		//第0 個房間不處理
		if( Obj->Role()->RoomID() == 0 || Obj->Role()->RoomID() == -1)
			continue;

		if( roomPlayerCountList.size() <= (unsigned)Obj->Role()->RoomID() )
			continue;

		//計算每個房間的人數
		roomPlayerCountList[ Obj->Role()->RoomID() ]++;
	
	}

	map< int , HousesManageClass* >::iterator Iter;

	for( Iter = _HouseList_DBID.begin() ; Iter != _HouseList_DBID.end() ; Iter++ )
	{
		HousesManageClass* houseClass = Iter->second;
/*
		if( houseClass->State() != EM_HouseObjectState_OK )
		{
			if( RoleDataEx::G_SysTime > houseClass->_LiveTime )
				delList.push_back( houseClass );	
			continue;
		}
*/
		//租金計算
		if( houseClass->_IsReady != false )
		{
			if( houseClass->_FurnitureAbility.IsRecalculate != false )
			{
				houseClass->CalFurnitureAbility();
			}

			houseClass->CalRental( true );
			houseClass->ServantProc();

			if( houseClass->_IsNeedSave != false 
				&& (	houseClass->_SaveTime < RoleDataEx::G_SysTime 
					||	houseClass->_LiveTime < RoleDataEx::G_SysTime 
					|| IsSaveAll != false ) )
			{
				houseClass->SaveProc();
				continue;
			}
		}




		int roomID = houseClass->RoomID();
		if( roomPlayerCountList.size() > roomID && roomPlayerCountList[ roomID ] > 0  )
		{
			//houseClass->_LiveTime = RoleDataEx::G_SysTime + _DEF_HOUSES_LIFETIME_;
			houseClass->_LiveTime = RoleDataEx::G_SysTime + Global::Ini()->HouseLifeTime;
			continue;
		}
		if( RoleDataEx::G_SysTime < houseClass->_LiveTime )
			continue;

		delList.push_back( houseClass );	


	}

	for( int i = 0 ; i < delList.size() ; i++ )
	{
		delete delList[i];
	}


	//////////////////////////////////////////////////////////////////////////
	
	int NowTime = TimeStr::Now_Value() / (60*60) ;
	//每小時處理一次
	if( NowTime > _LastProcHouseAddExpTime )
	{
		_LastProcHouseAddExpTime = NowTime;


		for( Iter = _HouseList_DBID.begin() ; Iter != _HouseList_DBID.end() ; Iter++ )
		{
			HousesManageClass* houseClass = Iter->second;

			if( houseClass->_IsReady == false )
				continue;

			houseClass->AddTpExp( 1 );

		}

	}
	//////////////////////////////////////////////////////////////////////////
	if( ProcCount % 6 == 0 )
	{
		for( Iter = _HouseList_DBID.begin() ; Iter != _HouseList_DBID.end() ; Iter++ )
		{
			HousesManageClass* houseClass = Iter->second;
			//租金計算
			if( houseClass->_IsReady != false )
			{
				vector< HouseItemStruct* >& ItemList =houseClass->HouseBase().ItemList[-1];
				for( unsigned i = 0 ; i < ItemList.size() ; i++ )
				{
					if( PlantItemGrowProc( ItemList[i]->Info.Item ) != false )
					{
						if( ItemList[i]->SaveState == EM_HouseItemSaveState_Normal )
							ItemList[i]->SaveState = EM_HouseItemSaveState_Update;
						houseClass->SetSaveInfo();
						NetSrv_HousesChild::SC_FixHouseItemProc( NULL , NULL , ItemList[i]->Info , houseClass->RoomID() );
					}
				}
			}
		}
	}

}
//////////////////////////////////////////////////////////////////////////
HousesManageClass::HousesManageClass()
{
	_OffLineTime = 0;
	_SaveTime = 0;
	_IsNeedSave = false;
	_State = EM_HouseObjectState_None;
	//_LiveTime = RoleDataEx::G_SysTime	 + _DEF_HOUSES_LIFETIME_;
	_LiveTime = RoleDataEx::G_SysTime	 + Global::Ini()->HouseLifeTime;
	_OwnerID = 0 ;
	_IsReady = false;
	_MaxItemDBID = 0;
}
HousesManageClass::~HousesManageClass()
{
	if( _State == EM_HouseObjectState_None )
		return;

	map< int , vector< HouseItemStruct* > >::iterator Iter;
	for( Iter = _HouseBase.ItemList.begin() ; Iter != _HouseBase.ItemList.end() ; Iter++ )
	{
		_DelAllItem( Iter->second );
	}
	_HouseList_DBID.erase(  _HouseBase.Info.HouseDBID );

	//把房間清除
	vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;

	if( (unsigned)_RoomID < RoomList.size() )
	{
		//RoomList[_RoomID].IsActive = false;
		RoomList[_RoomID].Init();
	}
	char Buf[256];
	sprintf_s( Buf , sizeof(Buf) , "UPDATE Houses_Base SET IsLogin = 0 WHERE (HouseDBID = %d)" , _HouseBase.Info.HouseDBID );
	Net_DCBase::SqlCommand( _HouseBase.Info.HouseDBID , Buf );
}

//設定房屋基本資料
bool					HousesManageClass::SetHouseBase( HouseBaseDBStruct& info )
{
	if( info.HouseDBID != _HouseBase.Info.HouseDBID )
		return false;

	_HouseBase.Info = info;

	CalRental();
	ServantProc();
	_IsReady = true;

	if( _HouseBase.ItemList.size() != 0 )
		return true;

	 
	//目前不需要

	//建立擺設物品欄位
	for( int i = 0 ; i < _DEF_MAX_HOUSE_FURNITURE ; i++ )
	{
		HouseItemStruct* newItem = _HouseItemRecycle.NewObj();//NEW( HouseItemStruct );
		newItem->Init();
		newItem->Info.Pos = i;
		newItem->Info.ParentItemDBID = -1;
		_HouseBase.ItemList[-1].push_back( newItem );
	}

	for( int i = 0 ; i < MAX_SERVANT_COUNT ; i++ )
	{
		HouseItemStruct* newItem = _HouseItemRecycle.NewObj();//NEW( HouseItemStruct );
		newItem->Init();
		newItem->Info.Pos = i;
		newItem->Info.ParentItemDBID = -2;
		_HouseBase.ItemList[-2].push_back( newItem );
	}

	char Buf[256];
	sprintf_s( Buf , sizeof(Buf) , "UPDATE Houses_Base SET IsLogin = 1 WHERE (HouseDBID = %d)" , info.HouseDBID );
	Net_DCBase::SqlCommand( info.HouseDBID , Buf );

	return true;
}
//放入新的物件
bool					HousesManageClass::AddItem( HouseItemDBStruct& info )
{	
	if( info.ItemDBID != -1 )
	{
		if( _MaxItemDBID <= info.ItemDBID )
			_MaxItemDBID = info.ItemDBID + 1;
	}

	//由位置取出資料
	HouseItemStruct* houseItem = GetItem( info.ParentItemDBID , info.Pos );

	if( houseItem == NULL || info.Item.IsEmpty() != false )
		return false;

	if( houseItem->SaveState != EM_HouseItemSaveState_Empty )
		return false;

	GameObjDbStructEx* ItemDB = Global::GetObjDB( info.Item.OrgObjID );
	if( ItemDB->IsItem() == false && ItemDB->IsRecipe() == false )
		return false;

	houseItem->Info = info;
	houseItem->Info.HouseDBID = _HouseBase.Info.HouseDBID; 

	if( houseItem->Info.ItemDBID != -1 )
	{
		houseItem->SaveState = EM_HouseItemSaveState_Normal;
//		if( _MaxItemDBID <= houseItem->Info.ItemDBID )
//			_MaxItemDBID = houseItem->Info.ItemDBID + 1;

	}
	else
	{
		if( IsLoadOK() == false )
		{
			Print( LV5 , "HousesManageClass::AddItem" , "houseClass->IsLoadOK() == false 房屋還沒載完就要求處理物品資料" );
			return false;
		}

		if( _DelItemDBIDList.size() == 0 )
		{
			/*
			if( _RecycleID.size() != 0 )
			{
				houseItem->Info.ItemDBID = _RecycleID.back();
				_RecycleID.pop_back();
			}
			else
			{
			*/
				houseItem->Info.ItemDBID = _MaxItemDBID++;
				/*
			}
			*/
			houseItem->SaveState = EM_HouseItemSaveState_Insert;
			
		}
		else
		{
			houseItem->Info.ItemDBID = _DelItemDBIDList.back();
			_DelItemDBIDList.pop_back();
			houseItem->SaveState = EM_HouseItemSaveState_Update;
		}
		SetSaveInfo();
	}

	//目前只有最外層可以放寶箱
	if(	( info.ParentItemDBID == -1 || info.ParentItemDBID == -2 ) 
			&& ItemDB->IsItem_Pure()	)
	{
		PlantItemGrowProc( info.Item );
		//家俱數值重新記算
		_FurnitureAbility.IsRecalculate = true;

		for( int i = 0 ; i < ItemDB->Item.iFurnitureStorageSize ; i++ )
		{
			HouseItemStruct* newItem = _HouseItemRecycle.NewObj();//NEW( HouseItemStruct );
			newItem->Init();
			newItem->Info.Pos = i;
			newItem->Info.ParentItemDBID = houseItem->Info.ItemDBID;
			houseItem->ChildList.push_back( newItem );
		}
	}

	info = houseItem->Info;
	_ItemList_DBID[ houseItem->Info.ItemDBID ] = houseItem;

//	_SaveTime	= RoleDataEx::G_Now + _DEF_HOUSES_SAVETIME_;
//	_IsNeedSave	= true;
	return true;
}

void					HousesManageClass::SetSaveInfo( )
{
	if( _IsNeedSave == false )
	{
		_SaveTime = RoleDataEx::G_SysTime + _DEF_HOUSES_SAVETIME_;
		_IsNeedSave = true;
	}
	else if( RoleDataEx::G_SysTime + _DEF_HOUSES_SAVETIME_ > _SaveTime  )
	{
		_SaveTime = _DEF_HOUSES_SAVETIME_ + RoleDataEx::G_SysTime;
	}
}

//刪除物件
bool					HousesManageClass::DelItem( int itemDBID )
{
	HouseItemStruct* houseItem = GetItem( itemDBID );
	if( houseItem == NULL )
		return false;

	if( CheckChildItem( houseItem ) == false )
		return false;

	if( houseItem->Info.ParentItemDBID == -1 )
	{
		//家俱數值重新記算
		_FurnitureAbility.IsRecalculate = true;
	}

	_DelAllItem( houseItem->ChildList );	
	_ItemList_DBID.erase( itemDBID );

	if( houseItem->SaveState != EM_HouseItemSaveState_Insert )
		_DelItemDBIDList.push_back( itemDBID );
/*	else
		_RecycleID.push_back( itemDBID );
		*/

	houseItem->Info.Item.Init();
	houseItem->SaveState = EM_HouseItemSaveState_Empty;

	SetSaveInfo();

	return true;
}
//檢查欄位是否可以放置
bool					HousesManageClass::CheckItemPos( int parentItemDBID , int pos )
{
	if( parentItemDBID == -1 )
	{
		if( pos >= _HouseBase.Info.MaxItemCount )
			return false;
		return true;
	}


	HouseItemStruct* houseParentItem = GetItem( parentItemDBID );
	if( houseParentItem == NULL )
		return false;

	if(  houseParentItem->Info.Pos >= _HouseBase.Info.MaxItemCount )
		return false;
	
	return true;
}
bool					HousesManageClass::SwapItem( int parentItemDBID1 , int pos1 , int parentItemDBID2 , int pos2 )
{
	HouseItemStruct** houseItem1 = GetItem_Ref( parentItemDBID1 , pos1 );
	HouseItemStruct** houseItem2 = GetItem_Ref( parentItemDBID2 , pos2 );

	if( houseItem1 == NULL || houseItem2 == NULL )
		return false;

	if(		( parentItemDBID1 != -1 && parentItemDBID2 == -1 ) 
		||	( parentItemDBID1 == -1 && parentItemDBID2 != -1 ) )
	{
		if( RoleDataEx::CheckMovePlantItem( (*houseItem1)->Info.Item ) == false )
			return false;

		if( RoleDataEx::CheckMovePlantItem( (*houseItem2)->Info.Item ) == false )
			return false;
	}

	//////////////////////////////////////////////////////////////////////////
	//自己不能放入自己
	if( (*houseItem1)->Info.ItemDBID == parentItemDBID2 && parentItemDBID2 != -1 )
		return false;

	if( (*houseItem2)->Info.ItemDBID == parentItemDBID1 && parentItemDBID1 != -1 )
		return false;


	//////////////////////////////////////////////////////////////////////////
	//寶箱有東西不能放入寶鄉	
	if( parentItemDBID1 == -1 && parentItemDBID2 != -1 && CheckChildItem( *houseItem1 ) == false )
	{
		return false;
	}

	if( parentItemDBID1 != -1 && parentItemDBID2 == -1 && CheckChildItem( *houseItem2 ) == false )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	GameObjDbStructEx* Item2DB = g_ObjectData->GetObj( (*houseItem2)->Info.Item.OrgObjID );
	//家俱才可以放到擺設的位置
	if( parentItemDBID1 == -1 )
	{
		if(		Item2DB != NULL 
			&& ( Item2DB->IsItem() == false || Item2DB->Item.ItemType != EM_ItemType_Furniture ) )
			return false;
	}

	GameObjDbStructEx* Item1DB = g_ObjectData->GetObj( (*houseItem1)->Info.Item.OrgObjID );
	if( parentItemDBID2 == -1 )
	{
		
		if(		Item1DB != NULL 
			&& ( Item1DB->IsItem() == false || Item1DB->Item.ItemType != EM_ItemType_Furniture ) )
			return false;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	ItemFieldStruct& Item1 = (*houseItem1)->Info.Item;
	ItemFieldStruct& Item2 = (*houseItem2)->Info.Item;
	//檢查兩個物品是否相同　是否可堆疊
	if(		Item1.OrgObjID == Item2.OrgObjID  
		&&	Item1DB != NULL 
		&&	Item1DB->MaxHeap > 1 )
	{
		if( Item1DB->MaxHeap >=Item1.Count+ Item2.Count )
		{
			Item1.Count += Item2.Count;
			Item2.Init();
			DelItem( (*houseItem2)->Info.ItemDBID );
			if( (*houseItem1)->SaveState == EM_HouseItemSaveState_Normal )
				(*houseItem1)->SaveState = EM_HouseItemSaveState_Update;
		}
		else
		{
			Item2.Count = Item1.Count + Item2.Count - Item1DB->MaxHeap;
			Item1.Count = Item1DB->MaxHeap;

			if( (*houseItem1)->SaveState == EM_HouseItemSaveState_Normal )
				(*houseItem1)->SaveState = EM_HouseItemSaveState_Update;

			if( (*houseItem2)->SaveState == EM_HouseItemSaveState_Normal )
				(*houseItem2)->SaveState = EM_HouseItemSaveState_Update;
		}

	}
	else
	{
		swap( *houseItem1 , *houseItem2 );
		(*houseItem1)->Info.Pos = pos1;
		(*houseItem2)->Info.Pos = pos2;
		(*houseItem1)->Info.ParentItemDBID  = parentItemDBID1;
		(*houseItem2)->Info.ParentItemDBID  = parentItemDBID2;

		if( (*houseItem1)->SaveState == EM_HouseItemSaveState_Normal )
			(*houseItem1)->SaveState = EM_HouseItemSaveState_Update;

		if( (*houseItem2)->SaveState == EM_HouseItemSaveState_Normal )
			(*houseItem2)->SaveState = EM_HouseItemSaveState_Update;
	}

	SetSaveInfo();
	return true;
}
bool					HousesManageClass::SwapItem( ItemFieldStruct& bodyItem , int parentItemDBID , int pos , bool isFromBody )
{
	HouseItemStruct* houseItem = GetItem( parentItemDBID , pos );	
	GameObjDbStructEx* bodyItemDB = Global::GetObjDB( bodyItem.OrgObjID );

	if( bodyItemDB->IsItem() != false && bodyItemDB->Item.ItemType == EM_ItemType_Plot )
		return false;

	if( houseItem == NULL )
		return false;
	
	if( bodyItem.IsEmpty() && houseItem->SaveState == EM_HouseItemSaveState_Empty )
		return false;

	if( parentItemDBID == - 1)
	{		
		
		if( _HouseBase.Info.MaxItemCount <= pos )
		{
			return false;
		}

		_FurnitureAbility.IsRecalculate = true;

		//只能放家具		
		if( bodyItemDB != NULL )
		{
			if( bodyItemDB->IsItem() == false || bodyItemDB->Item.ItemType != EM_ItemType_Furniture || bodyItemDB->Item.Furniture_Type == EM_FurnitureType_GuildOnly )
				return false;
		}

		if( RoleDataEx::CheckMovePlantItem( houseItem->Info.Item ) == false )
			return false;

	}
	else
	{
		HouseItemStruct* houseParentItem = GetItem( parentItemDBID  );	
		GameObjDbStructEx	*BoxDB = Global::GetObjDB( houseParentItem->Info.Item.OrgObjID );
		if( BoxDB->IsItem() == false || BoxDB->Item.iFurnitureStorageSize <= pos )
			return false;
	}

	ItemFieldStruct tempHouseItem;
	
	if( houseItem->SaveState != EM_HouseItemSaveState_Empty )
		tempHouseItem = houseItem->Info.Item;

	if( bodyItem.IsEmpty() )
	{
		if( DelItem( houseItem->Info.ItemDBID ) == false )
			return false;

		bodyItem = tempHouseItem;
	}
	else if( houseItem->SaveState ==  EM_HouseItemSaveState_Empty )
	{
		HouseItemDBStruct newHouseItem;
		newHouseItem.Init();
		newHouseItem.HouseDBID		= _HouseBase.Info.HouseDBID;
		newHouseItem.Item			= bodyItem;
		newHouseItem.Pos			= pos;
		newHouseItem.ParentItemDBID = parentItemDBID;

		AddItem( newHouseItem );
		bodyItem.Init();
	}
	else
	{
		if( bodyItem.OrgObjID == houseItem->Info.Item.OrgObjID && bodyItemDB->MaxHeap > 1 && bodyItem.Mode.HideCount == false )
		{
			int totalCount = bodyItem.Count + houseItem->Info.Item.Count;
			
			if( totalCount > bodyItemDB->MaxHeap )
			{
				if( isFromBody )
				{
					houseItem->Info.Item.Count = bodyItemDB->MaxHeap;
					houseItem->SaveState = EM_HouseItemSaveState_Update;
					bodyItem.Count = totalCount - bodyItemDB->MaxHeap;
				}
				else
				{
					bodyItem.Count = bodyItemDB->MaxHeap;
					houseItem->Info.Item.Count = totalCount - bodyItemDB->MaxHeap;
					houseItem->SaveState = EM_HouseItemSaveState_Update;					
				}
			}
			else
			{
				if( isFromBody )
				{
					houseItem->Info.Item.Count = totalCount;
					houseItem->SaveState = EM_HouseItemSaveState_Update;
					bodyItem.Init();
				}
				else
				{
					bodyItem.Count = totalCount;
					DelItem( houseItem->Info.ItemDBID );
				}
			}
			if( bodyItem.IsEmpty() == false )
				bodyItem.Mode.Trade = !bodyItemDB->Mode.PickupBound;

		}
		else
		{
			if( CheckChildItem( houseItem ) == false )
				return false;

			houseItem->Info.Item = bodyItem;
			houseItem->Info.Layout.Init();
			bodyItem = tempHouseItem;

			houseItem->ChildList.clear();
			for( int i = 0 ; i < bodyItemDB->Item.iFurnitureStorageSize ; i++ )
			{
				HouseItemStruct* newItem = _HouseItemRecycle.NewObj();//NEW( HouseItemStruct );
				newItem->Init();
				newItem->Info.Pos = i;
				newItem->Info.ParentItemDBID = houseItem->Info.ItemDBID;
				houseItem->ChildList.push_back( newItem );
			}

			if( houseItem->SaveState != EM_HouseItemSaveState_Insert )
				houseItem->SaveState = EM_HouseItemSaveState_Update;
		}
	}

	
	SetSaveInfo();
	return true;
}
//取出某欄位(如果是空的會產生)
HouseItemStruct*		HousesManageClass::GetItem( int parentItemDBID , int pos )
{
	vector<HouseItemStruct*>*	itemList = NULL;
	if( parentItemDBID < 0 )
		itemList = &_HouseBase.ItemList[ parentItemDBID ];
	else
	{
		map< int , HouseItemStruct* >::iterator Iter;
		Iter = _ItemList_DBID.find( parentItemDBID );

		if( Iter == _ItemList_DBID.end() )
			return NULL;
		
		itemList = &(Iter->second->ChildList);
	}


	if( (unsigned)pos >= itemList->size() )
			return NULL;

	return (*itemList)[ pos ];
}
HouseItemStruct*		HousesManageClass::GetItem( int itemDBID )
{
	map< int , HouseItemStruct* >::iterator Iter;
	Iter = _ItemList_DBID.find( itemDBID );

	if( Iter == _ItemList_DBID.end() )
		return NULL;

	return  Iter->second;
}

HouseItemStruct**		HousesManageClass::GetItem_Ref( int parentItemDBID , int pos )
{
	vector<HouseItemStruct*>*	itemList = NULL;
	if( parentItemDBID < 0 )
		itemList = &_HouseBase.ItemList[ parentItemDBID ];
	else
	{
		map< int , HouseItemStruct* >::iterator Iter;
		Iter = _ItemList_DBID.find( parentItemDBID );

		if( Iter == _ItemList_DBID.end() )
			return NULL;

		itemList = &(Iter->second->ChildList);
	}


	if( (unsigned)pos >= itemList->size() )
		return NULL;

	return &((*itemList)[ pos ]);
}

void					HousesManageClass::_DelAllItem( vector< HouseItemStruct* >&	itemList )
{
	for( unsigned i = 0 ; i < itemList.size() ; i++ )
	{
		if( itemList[i]->ChildList.size() != 0 )
			_DelAllItem( itemList[i]->ChildList );
		_HouseItemRecycle.DeleteObj( itemList[i] );
	}
	itemList.clear();
}

//查看是否有子物件
bool					HousesManageClass::CheckChildItem( HouseItemStruct* houseItem )
{
	for( unsigned i = 0 ; i < houseItem->ChildList.size() ; i++ )
	{
		if( houseItem->ChildList[i]->SaveState != EM_HouseItemSaveState_Empty )
			return false;
	}

	return true;
}

void					HousesManageClass::SaveHouseBaseProc()
{
	//儲存房屋資料
	char SqlWhereCmd[512];
	sprintf_s( SqlWhereCmd , sizeof(SqlWhereCmd) , "WHERE HouseDBID = %d" , _HouseBase.Info.HouseDBID  );
	Net_DCBase::SqlCommand(  _HouseBase.Info.HouseDBID , _RDHouseBaseSql->GetUpDateStr( &_HouseBase.Info , SqlWhereCmd ).c_str() );	
}


void			HousesManageClass::SaveProc()
{
	if( _IsNeedSave == false )
		return;
	_IsNeedSave = false;

	char Buf[512];
	//刪除所有的不須要的資訊
	for( unsigned int i = 0 ; i < _DelItemDBIDList.size() ; i++ )
	{
		sprintf_s( Buf , sizeof( Buf ) , "DELETE FROM Houses_Item	WHERE (ItemDBID = %d and HouseDBID=%d)" , _DelItemDBIDList[i] , _HouseBase.Info.HouseDBID );
		Net_DCBase::SqlCommand( _DelItemDBIDList[i] , Buf );
	}
	_DelItemDBIDList.clear();

	//資料儲存
	
	map< int , vector< HouseItemStruct* > >::iterator Iter;
	for( Iter = _HouseBase.ItemList.begin() ; Iter != _HouseBase.ItemList.end() ; Iter++ )
	{
		SaveAll( Iter->second );
	}

	SaveHouseBaseProc();
	RoleDataEx *owner = Global::GetRoleDataByGUID( _OwnerID );

	if( owner != NULL && owner->IsPlayer() )
	{
		//要求除存玩家
		//owner->SaveDB_All_Now();
		 owner->Net_SavePlayer( 0 , sizeof( PlayerRoleData ) , 60 * 1000 * 10 );
	}


	//RoleDataEx::G_SysTime < houseClass->_LiveTime
	if( _LiveTime < RoleDataEx::G_SysTime + 1000 * 60 )
		_LiveTime = RoleDataEx::G_SysTime + 1000 * 60;

}

void		HousesManageClass::SaveAll(  vector< HouseItemStruct* >& itemList )
{
	char SqlWhereCmd[512];
	for( unsigned i = 0 ; i < itemList.size() ; i++ )
	{
		if( itemList[i]->ChildList.size() != 0 )
			SaveAll( itemList[i]->ChildList );
		
		HouseItemStruct* item = itemList[i];
		if( item->SaveState == EM_HouseItemSaveState_Update )
		{
			sprintf_s( SqlWhereCmd , sizeof(SqlWhereCmd) , "WHERE ItemDBID = %d and HouseDBID=%d" , item->Info.ItemDBID , _HouseBase.Info.HouseDBID );
			Net_DCBase::SqlCommand(  item->Info.ItemDBID ,  _RDHouseItemSql->GetUpDateStr( &item->Info , SqlWhereCmd ).c_str() );	
		}
		else if( item->SaveState == EM_HouseItemSaveState_Insert )
		{
			Net_DCBase::SqlCommand(  item->Info.ItemDBID , _RDHouseItemSql->GetInsertStr( &item->Info ).c_str() );	
		}
		else
			continue;

		item->SaveState = EM_HouseItemSaveState_Normal;

	}
	//itemList.clear();
}

//女僕資料處理			
void		HousesManageClass::ServantProc( )
{
	const int hourTime = 60*60;
	if( TimeStr::Now_Value() < _HouseBase.Info.LastServantProcTime )
		_HouseBase.Info.LastServantProcTime = TimeStr::Now_Value();

	//時間檢查
	if( TimeStr::Now_Value() < _HouseBase.Info.LastServantProcTime + hourTime )
	{
		return;
	}

	//計算多少小時要加
	int hourCount = (TimeStr::Now_Value() - _HouseBase.Info.LastServantProcTime ) / hourTime;
	_IsNeedSave = true;
	_HouseBase.IsNeedSave = true;
	_HouseBase.Info.LastServantProcTime += hourCount * hourTime;

	//修正疲勞度
	int Tird_Recover = g_ObjectData->GetSysKeyValue( "House_Servant_Tird_Recover" );

	for( int i = 0 ; i < MAX_SERVANT_COUNT ; i++ )
	{
		HouseServantStruct& Info = GetHouseBase()->Info.Servant[ i ];
		if( Info.NPCObjID == 0 )
			continue;

		Info.Tired -= Tird_Recover* hourCount;
		if( Info.Tired <= 0 )
			Info.Tired = 0;

		NetSrv_Houses::SC_FixServantValue( _RoomID , i , EM_HouseServantValueType_Tired , Info.Tired );
	}

}

void		HousesManageClass::CalRental( bool IsNeedSendtoClient )
{
	const int dayTime = 60*60*24;
	//時間檢查
	if( TimeStr::Now_Value() < _HouseBase.Info.LastRantelTime + dayTime )
	{
		return;
	}
	
	//計算多少小時要加
	int dayCount = (TimeStr::Now_Value() - _HouseBase.Info.LastRantelTime ) / dayTime;
	_IsNeedSave = true;
	_HouseBase.IsNeedSave = true;
	_HouseBase.Info.LastRantelTime += dayCount * dayTime;


	//計算每小時的租金
	int RentalEnergy = 0;
	int BoxCount = 0;
	vector<HouseShopInfoStruct>&	houseShopInfo = g_ObjectData->HouseShop();
	for( unsigned i = 0 ; i < houseShopInfo.size() ; i++ )
	{
		if( _HouseBase.Info.ShopFunction.GetFlag( i ) == false )
			continue;
		RentalEnergy += houseShopInfo[i].KeppEnergy;
		BoxCount += houseShopInfo[i].FunitureCount;
	}	

	if( RentalEnergy == 0 )
		return;
	
	if( _HouseBase.Info.EnergyPoint < RentalEnergy*dayCount )
	{
		_HouseBase.Info.EnergyPoint %= RentalEnergy;
		_HouseBase.Info.EnergyPoint = 0;
		_HouseBase.Info.MaxItemCount -= BoxCount;
		_HouseBase.Info.ShopFunction.ReSet();
	}
	else
	{
		_HouseBase.Info.EnergyPoint -= RentalEnergy*dayCount;
	}

	//通知所有的Client 房屋資料改變
	if( IsNeedSendtoClient != false )
	{
		NetSrv_HousesChild::SendAllCli_FixHouseBase( RoomID() , _HouseBase.Info );
	}

	return;
}

//計算家具能力
void	HousesManageClass::CalFurnitureAbility()
{
	_FurnitureAbility.Init();

	vector< HouseItemStruct* >& ItemList = _HouseBase.ItemList[-1];
	for( unsigned i = 0 ; i < ItemList.size() ; i++ )
	{
		HouseItemStruct *Item = ItemList[i];
		if( Item == NULL )
			continue;
		
		GameObjDbStructEx* ItemObj = Global::GetObjDB( Item->Info.Item.OrgObjID );
		if( ItemObj == NULL )
			continue;
		
		if( ItemObj->IsItem() != false && ItemObj->Item.ItemType == EM_ItemType_Furniture )
		{
			_FurnitureAbility.ExpWeighted		+= ItemObj->Item.ExpPoint;
			_FurnitureAbility.TpExpWeighted		+= ItemObj->Item.TpPoint;
			_FurnitureAbility.BlackSmithWeighted+= ItemObj->Item.BlackSmithHQPoint;
			_FurnitureAbility.CarpenterWeighted	+= ItemObj->Item.CarpenterHQPoint;
			_FurnitureAbility.ArmorWeighted		+= ItemObj->Item.MakeArmorHQPoint;
			_FurnitureAbility.TailorWeighted	+= ItemObj->Item.TailorHQPoint;		
			_FurnitureAbility.AlchemyWeighted	+= ItemObj->Item.AlchemyHQPoint;		
			_FurnitureAbility.CookWeighted		+= ItemObj->Item.CookHQPoint;
		}
	}

	//計算比率
	_FurnitureAbility.ExpRate			= g_ObjectData->PowerRate( _FurnitureAbility.ExpWeighted		, EM_PowerRateType_Tpexp );
	_FurnitureAbility.TpExpRate			= g_ObjectData->PowerRate( _FurnitureAbility.TpExpWeighted		, EM_PowerRateType_Tpexp );
	_FurnitureAbility.BlackSmithRate	= g_ObjectData->PowerRate( _FurnitureAbility.BlackSmithWeighted	, EM_PowerRateType_HQ );
	_FurnitureAbility.CarpenterRate		= g_ObjectData->PowerRate( _FurnitureAbility.CarpenterWeighted	, EM_PowerRateType_HQ );
	_FurnitureAbility.ArmorRate			= g_ObjectData->PowerRate( _FurnitureAbility.ArmorWeighted		, EM_PowerRateType_HQ );
	_FurnitureAbility.TailorRate		= g_ObjectData->PowerRate( _FurnitureAbility.TailorWeighted		, EM_PowerRateType_HQ );
	_FurnitureAbility.AlchemyRate		= g_ObjectData->PowerRate( _FurnitureAbility.AlchemyWeighted	, EM_PowerRateType_HQ );
	_FurnitureAbility.CookRate			= g_ObjectData->PowerRate( _FurnitureAbility.CookWeighted		, EM_PowerRateType_HQ );	


	//計算房屋內所有的玩家
	_FurnitureAbility.IsRecalculate = false;

	if( _FurnitureAbility.IsReady == false ) 
	{
		AddTpExp( _OffLineTime );
		_OffLineTime = 0;
	}
	_FurnitureAbility.IsReady = true;
}
//檢查並修正複製物品
void	HousesManageClass::CheckAndClearCopyItem( RoleDataEx *Role )
{
	//收尋是否有複製的物品
	set< ItemFieldStruct > ItemSet;
	pair< set<ItemFieldStruct>::iterator, bool > InsertRet;


	for( int i = 0 ; i < Role->PlayerBaseData->Body.Count ; i++ )
	{
		ItemFieldStruct& Item = Role->PlayerBaseData->Body.Item[i];
		if(	Item.IsEmpty() != false 	)
			continue;

		GameObjDbStructEx* OrgDB = Global::GetObjDB( Item.OrgObjID );

		if( OrgDB == NULL )
		{
			Item.Init();
			Role->Net_FixItemInfo_Body( i );
			continue;
		}

		if( Item.Serial == 0 )
		{
			Item.CreateTime = RoleDataEx::G_Now;
			Item.Serial		= Global::GenerateItemVersion( );
		}

		if( OrgDB->MaxHeap > 1 )
		{
			memset( Item.Ability , 0 , sizeof( Item.Ability ) );
			if( Item.Mode.Trade != !(OrgDB->Mode.PickupBound) )
			{
				Item.Mode.Trade = !(OrgDB->Mode.PickupBound);
				Role->Net_FixItemInfo_Body( i );
			}
		}

		InsertRet = ItemSet.insert( Item );

		if( InsertRet.second == false )
		{
			Role->Log_ItemDestroy( EM_ActionType_DestroyItem_CloneItem , Item , -1 , "" );
			Item.Init();
			Role->Net_FixItemInfo_Body( i );
		}
	}
	//搜尋所有寶箱資料
	map< int , HouseItemStruct* >::iterator Iter;
	vector< int > DelItemDBID;
	
	for( Iter = _ItemList_DBID.begin() ; Iter != _ItemList_DBID.end() ; Iter++ )
	{

		ItemFieldStruct& Item = Iter->second->Info.Item;
		if(	Item.IsEmpty() != false 	)
			continue;

		GameObjDbStructEx* OrgDB = Global::GetObjDB( Item.OrgObjID );

		if( OrgDB == NULL )
		{
			Item.Init();			
			//DelItem( Iter->second->Info.ItemDBID );
			DelItemDBID.push_back( Iter->second->Info.ItemDBID );
			continue;
		}

		if( Item.Serial == 0 )
		{
			Item.CreateTime = RoleDataEx::G_Now;
			Item.Serial		= Global::GenerateItemVersion( );
		}

		if( OrgDB->MaxHeap > 1 )
		{
			memset( Item.Ability , 0 , sizeof( Item.Ability ) );
			Item.Mode.Trade = !(OrgDB->Mode.PickupBound);
		}

		InsertRet = ItemSet.insert( Item );

		if( InsertRet.second == false )
		{	
			Role->Log_ItemDestroy( EM_ActionType_DestroyItem_CloneItem_House  , Item , -1 , "" );
			Item.Init();
			//DelItem( Iter->second->Info.ItemDBID );
			DelItemDBID.push_back( Iter->second->Info.ItemDBID );
		}
	}

	for( unsigned i = 0 ; i < DelItemDBID.size() ; i++ )
	{
		DelItem( DelItemDBID[i] );
	}
}

void	HousesManageClass::CalOffLineTime( unsigned LogoutTime )
{
	_OffLineTime	 = _LastProcHouseAddExpTime - LogoutTime/(60*60);	

	if( _FurnitureAbility.IsReady != false )
	{
		AddTpExp( _OffLineTime );
		_OffLineTime = 0;
	}

}

void	HousesManageClass::AddTpExp( int Time )
{
	if( Time == 0 )
		return;

	RoleDataEx* Role = Global::GetRoleDataByGUID( _OwnerID );
	if( Role == NULL || Role->IsPlayer() == false )
		return;

	int	LevelExp = Role->GetLVExp( Role->BaseData.Voc );

	if(		RoleDataEx::HouseExpArgTable.size() <= (unsigned)Role->Level() )
//		||	Role->PlayerBaseData->MaxLv < Role->Level() )
		return;

	int HouseExpArg = RoleDataEx::HouseExpArgTable[ Role->Level() - 1 ];
	if( HouseExpArg <= 0 )
		return;

	if( Role->TempData.Attr.Ability->AddTp < LevelExp/10 )
	{
		int AddTp  = int( LevelExp * _FurnitureAbility.TpExpRate ) /HouseExpArg / 10 * Time ;

		if( Role->TempData.Attr.Ability->AddTp + AddTp > LevelExp/10 )
		{
			AddTp = LevelExp/10 - Role->TempData.Attr.Ability->AddTp;
		}
		if( AddTp > 0 )
			Role->AddValue( EM_RoleValue_AddTP , AddTp );
	}

	if( Role->TempData.Attr.Ability->AddExp < LevelExp )
	{
		int	AddExp = int( LevelExp * _FurnitureAbility.ExpRate ) / HouseExpArg * Time ; 

		if( Role->TempData.Attr.Ability->AddExp + AddExp > LevelExp )
		{
			AddExp = LevelExp - Role->TempData.Attr.Ability->AddExp;
		}
		if( AddExp > 0 )
			Role->AddValue( EM_RoleValue_AddExp , AddExp );
	}

}

//-------------------------------------------------------------------------------------------
//	盆栽處理
//-------------------------------------------------------------------------------------------
int	HousesManageClass::DHealthPoint( int DLv )
{
	DLv = abs(DLv);
	if( RoleDataEx::PlantEffectPointTable.size() <= DLv )
	{
		return -10000;
	}
	
	return int( RoleDataEx::PlantEffectPointTable[DLv] * 100 );
}
bool HousesManageClass::PlantItemGrowProc( ItemFieldStruct& Item )
{
	
	GameObjDbStructEx* PotObjDB  = Global::GetObjDB( Item.OrgObjID );
	if( PotObjDB->IsItem() == false || PotObjDB->Item.Plant.Pot.Mode == 0 )
		return false;
	if( Item.Plant.SeedID == 0 )
		return false;

	GameObjDbStructEx* SeedObjDB = Global::GetObjDB( Item.Plant.SeedID + Def_ObjectClass_Item );
	if( SeedObjDB->IsItem() == false )
		return false;

	if( Item.Plant.IsLock != false )
		return false;

	if( RoleDataEx::G_Now - Item.Plant.LastProcTime <= int(60*10/G_PlantGrowRate)  )
		return false;

	//肥料處理
	int DecWater	= (int( Item.Plant.DecWaterPoint ) + int( SeedObjDB->Item.Plant.DecWater*100 ) + int( PotObjDB->Item.Plant.DecWater*100 ))*-1 / 6;
	int DecTopdress = (int( Item.Plant.DecTopdressPoint ) + int( SeedObjDB->Item.Plant.DecTopdress*100 ) + int( PotObjDB->Item.Plant.DecTopdress*100 ))*-1 / 6;

	if( DecWater > 0 )
		DecWater = 0;

	if( DecTopdress > 0 )
		DecTopdress = 0;

	//float PotHeathRate = PotObjDB->Item.Plant.Pot.EvnPoint[ SeedObjDB->Item.Plant.Seed.Type ][ 1 ];
	float DecRate = 1;
	switch( SeedObjDB->Item.Plant.Seed.Type )
	{
	case EM_SeedType_Tree:	//木
		{
			DecRate = PotObjDB->Item.Plant.Pot.EvnPoint[0][0];
		}
		break;
	case EM_SeedType_Grass:	//草
		{
			DecRate = PotObjDB->Item.Plant.Pot.EvnPoint[1][0];
		}
		break;
	case EM_SeedType_Mine:	//礦
		{
			DecRate = PotObjDB->Item.Plant.Pot.EvnPoint[2][0];
		}
		break;
	}
	DecWater	= int( DecWater * DecRate );
	DecTopdress = int( DecTopdress * DecRate );

	while( RoleDataEx::G_Now - Item.Plant.LastProcTime > int(60*10/G_PlantGrowRate)  )
	{
		Item.Plant.LastProcTime += int(60*10/G_PlantGrowRate); 

		//計算健康度
		//水肥濃度差
		int DeltaWater	= int( Item.Plant.WaterPoint/100 ) - SeedObjDB->Item.Plant.Seed.BestWater;
		int DeltaTopdree= int( Item.Plant.TopdressPoint/100 ) - SeedObjDB->Item.Plant.Seed.BestTopdress;
		Item.Plant.HealthPoint += int(( DHealthPoint( DeltaWater ) + DHealthPoint(DeltaTopdree) ) );
		
		//水肥濃度處理
		Item.Plant.WaterPoint += DecWater;
		Item.Plant.TopdressPoint += DecTopdress;

		if( Item.Plant.TopdressPoint < 0 )
			Item.Plant.TopdressPoint = 0;

		if( Item.Plant.WaterPoint < 0 )
			Item.Plant.WaterPoint = 0;

		//成長度計算
		Item.Plant.GrowRate += int( SeedObjDB->Item.Plant.Seed.GrowSpeed * 100 / 6  );

		//超過150% 預定一天後死亡
		if( Item.Plant.GrowRate > 15000 )
		{
			Item.Plant.HealthPoint -= ( 10000/(24*6) );
		}

		if( Item.Plant.GrowRate > 20000 )
			Item.Plant.GrowRate = 20000;


		Item.Plant.FeedPoint -= ( g_ObjectData->SysValue().Plant.FeedPointDec *100 / 6 );
		if( Item.Plant.FeedPoint < 0 )
			Item.Plant.FeedPoint = 0;

		if( Item.Plant.HealthPoint <= 0 )
		{
			Item.Plant.HealthPoint = 0;
			Item.Plant.IsLock = true;
			Item.Plant.IsDead = true;
			break;
		}
	}
	return true;
}

bool	HousesManageClass::CheckFriend( int DBID )
{
	for( unsigned i = 0 ; i < _FriendDBIDList.size() ; i++ )
	{
		if( _FriendDBIDList[i] == DBID )
			return true;
	}
	return false;
}

int		HousesManageClass::PlantItemCount( )
{
	int PlantCount = 0;
	vector< HouseItemStruct* >&	ItemList = _HouseBase.ItemList[-1];
	
	for( unsigned int i = 0 ; i < ItemList.size() ; i++ )
	{
		ItemFieldStruct& Item = ItemList[i]->Info.Item;

		GameObjDbStructEx* ItemDB = Global::GetObjDB( Item.OrgObjID );
		if( ItemDB->IsItem() == false )
			continue;

		if( ItemDB->Item.Plant.Pot.Mode == 0 )
			continue;

		if( Item.Plant.SeedID == 0 || Item.Plant.IsLock != false )
			continue;
		PlantCount++;
	}
	return PlantCount;
}