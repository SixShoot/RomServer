#include "BaseItemObject.h"
#include "../../NetWalker_Member/NetWakerPartInc.h"
#include "../Global.h"
//////////////////////////////////////////////////////////////////////////
vector< ZoneMapNameStruct >				BaseItemObject::_ZoneMapFileName;
map< int , int >						BaseItemObject::_ZoneIDToMapID;
vector< vector<BaseItemObject*> >		BaseItemObject::_AllObj;    //類似_AllObj[][];
vector<SrvPartitionProc*>				BaseItemObject::_AllSrvPart;   //Server註冊的資料
vector<vector<MyMapAreaManagement*>>    BaseItemObject::_AllObjPart;   //管理物件 分割區移動
int										BaseItemObject::_BlockSize;
//////////////////////////////////////////////////////////////////////////
// BlockSize  為分割區
bool    BaseItemObject::Init( )
{
	Release();

	_BlockSize = Global::Ini()->Int( "Block" );
	_ZoneMapFileName.clear();
	_ZoneIDToMapID.clear();
	return true;
}
bool    BaseItemObject::Release( )
{
	for( unsigned int i = 0 ; i < _AllObj.size() ; i++ )
	{
		for( unsigned int j = 0 ; j < _AllObj[i].size() ; j++ )
			delete _AllObj[i][j];
	}
	_AllObj.clear();

	for( unsigned int i = 0 ; i < _AllSrvPart.size() ; i++ )
	{
		delete _AllSrvPart[i];
	}
	_AllSrvPart.clear();

	for( unsigned int i = 0 ; i < _AllObjPart.size() ; i++ )
	{
		for( unsigned int j = 0 ; j < _AllObjPart[i].size() ; j++ )
			delete _AllObjPart[i][j];
	}
	_AllObjPart.clear();

	return true;
}

int		BaseItemObject::AddSrv( int ZoneID , string MapFileName , int SubMapID )
{
	int		Index = -1;
	_strlwr( (char*)MapFileName.c_str() );

	ZoneMapNameStruct ZoneInfo;
	ZoneInfo.FileName = MapFileName;
	//ZoneInfo.ZoneBase = ZoneID / _DEF_ZONE_BASE_COUNT_;
	ZoneInfo.ZoneBase = 0;		//改寫後無用

	int ParalledID = ZoneID / _DEF_ZONE_BASE_COUNT_;

	for( unsigned i = 0 ; i < _ZoneMapFileName.size() ;  i++ )
	{
		if( _ZoneMapFileName[i] == ZoneInfo )
		{
			Index = (unsigned)i;
			break;
		}
	}

	if( Index == -1 )
	{
		Index = (int)_ZoneMapFileName.size();	
		_ZoneMapFileName.push_back( ZoneInfo );

		SrvPartitionProc* TempSrvPart = NEW SrvPartitionProc( MapFileName.c_str());
		_AllSrvPart.push_back( TempSrvPart );
		
		vector<MyMapAreaManagement*> TempVector;
		_AllObjPart.push_back( TempVector );
	}

	SrvPartitionProc* TempSrvPart = _AllSrvPart[ Index ];
	while( (int)_AllObjPart[Index].size() <= ParalledID )
	{
		MyMapAreaManagement* TempObjPart = NEW MyMapAreaManagement( TempSrvPart->Width() , TempSrvPart->Height() , 0xff * 10000 );
		_AllObjPart[Index].push_back( TempObjPart );
	}

	_ZoneIDToMapID[ ZoneID ] = Index;

	_AllSrvPart[ Index ]->Add( ZoneID % _DEF_ZONE_BASE_COUNT_ , SubMapID );

/*
	//收尋所有的可視空間是否有與其他Servert重疊
	//並找出通知新注冊的Server
	
	//1取出所有的可視點
	MyMapAreaManagement* ZoneObjPart = _AllObjPart[Index][ParalledID];
	int	ZoneBase   = ZoneID % _DEF_ZONE_BASE_COUNT_;
	for( unsigned i = 0 ; i < _AllSrvPart[ Index ]->Height() ;i++ )
	{
		for( unsigned j = 0 ; j < _AllSrvPart[ Index ]->Width() ; j++ )
		{
			vector<SrvInfoStruct*>& TempVector = *(_AllSrvPart[ Index ]->GetViewSrv( i , j ));
			bool IsFind = false;
			for( unsigned k = 0 ; k < TempVector.size() ; k++ )
			{
				if( TempVector[i]->SrvID == ZoneBase )
				{
					IsFind = true;
					break;
				}
			}

			if( IsFind == true )
			{
				PlayIDInfo* Temp = ZoneObjPart->Search( j,i );
				if( Temp == NULL )
					continue;

				int OtherPlayerZoneID = Temp->Begin->ID / 0x10000;
				//int OtherZoneID = SrvInfo->SrvID + _DEF_ZONE_BASE_COUNT_

				CNetPart_PartBase::OnAddtoOtherSrv( ZoneID , OtherZoneID, Temp->Begin->ID );
				CNetPart_PartBase::OnAdd( OtherZoneID , Temp->Begin->ID );
			}
		}
	}
*/

	return true;
}
bool	BaseItemObject::DelSrv( int ZoneID )
{
	//刪除所有此Srv的物件
	vector<BaseItemObject*>* ObjList =  BaseItemObject::GetZoneObj( ZoneID );
	BaseItemObject* Obj;
	for( int i = 0 ; i < (int)ObjList->size() ; i++ )
	{
		Obj = (*ObjList)[i];
		if( Obj != NULL )
		{
			delete Obj;
		}
	}
	int MapID = _ZoneIDToMapID[ ZoneID ];

//	return _AllSrvPart[MapID]->Del( ZoneID );
	return true;
}
vector<BaseItemObject*>*    BaseItemObject::GetZoneObj( int ZoneID )
{
	static vector<BaseItemObject*> Ret;
	Ret.clear();

	if( ZoneID >= (int)_AllObj.size() )
		return &Ret;

	Ret.insert( Ret.begin() ,  _AllObj[ ZoneID ].begin() , _AllObj[ ZoneID ].end() );

	return &Ret;

}
BaseItemObject* BaseItemObject::GetObj( int GItemID )
{
	GItemData IDInfo;
	IDInfo.GUID = GItemID;

	return GetObj( IDInfo.Zone , IDInfo.ID );
}
BaseItemObject* BaseItemObject::GetObj( int ZoneID , int ItemID )
{
	if( (int)_AllObj.size() <= ZoneID )
		return NULL;

	vector<BaseItemObject*>* Data = &_AllObj[ ZoneID ];
	if( (int)Data->size() <= ItemID )
		return NULL;

	return (*Data)[ItemID];
}

//////////////////////////////////////////////////////////////////////////
BaseItemObject::BaseItemObject( int ZoneID , int ItemID , int ParallelID , char* RoleName , int X , int Y )
{
	_OrgParallelID = ParallelID;
	_ParallelID = ZoneID / _DEF_ZONE_BASE_COUNT_;
	int MapID = _ZoneIDToMapID[ ZoneID ];
	_SrvPart = _AllSrvPart[ MapID ];
	_ObjPart = _AllObjPart[ MapID ][_ParallelID];
	
	_ItemID.ID      = ItemID;
	_ItemID.Zone    = ZoneID;
	_SecX           = (X - _SrvPart->MinX() )/ _BlockSize;   
	_SecY           = (Y - _SrvPart->MinY() )/ _BlockSize;
	_IsAddSrv       = false;
	_IsAddAllObj    = false;
	_IsDestroy		= false;
	_RoleName		= RoleName;

	if( _Add() == false )
	{
		_IsDestroy = true;
		return;
	}

}
BaseItemObject::~BaseItemObject( )
{
  _Del( );
}

bool        BaseItemObject::_Add( )
{
	int i;

	if(_IsAddAllObj == true )    
		return false;

//	if( (unsigned)_ItemID.Zone > _Def_Max_ZoneID_ )
//		return false;

	if( (unsigned)_ItemID.ID > _Def_Max_ItemID_ )
		return false;

	while( (int)_AllObj.size() <= _ItemID.Zone )
	{
		vector<BaseItemObject*> Temp;
		_AllObj.push_back( Temp );
	}

	while( (int)_AllObj[ _ItemID.Zone ].size() <=  _ItemID.ID )
	{
		_AllObj[ _ItemID.Zone ].push_back( NULL );
	}
	if( _AllObj[ _ItemID.Zone ][ _ItemID.ID ] != NULL )
	{
		Print( Def_PrintLV3 , "_Add" , "ZoneID %d ItemID=%d 已存在" , _ItemID.Zone , _ItemID.ID );        
		return false;
	}
	_AllObj[ _ItemID.Zone ][ _ItemID.ID ] = this;


	_IsAddAllObj   = true;
	if( _ObjPart->Add( _SecX , _SecY , _ItemID.GUID ) == false )
		return false;

	_IsAddSrv = true;
	//---------------------------------------------------------------------    
	//通知所有周圍的 Srv 有物件產生
	vector < SrvInfoStruct* >* SrvInfoList = _SrvPart->GetViewSrv( _SecX , _SecY );
	SrvInfoStruct* SrvInfo;
	if( SrvInfoList == NULL )
	{
		Print( Def_PrintLV3 , "_Add" , "取得 SrvInfoStruct 失敗"  );        
		return false;
	}
	//---------------------------------------------------------------------
	for( i = 0 ; i < (int)SrvInfoList->size() ; i++  )
	{
		SrvInfo = (*SrvInfoList)[i];

		if( SrvInfo->SrvID == _ItemID.Zone % _DEF_ZONE_BASE_COUNT_ )
			continue;
		
		if( GlobalBase::CheckZoneID( SrvInfo->SrvID + _DEF_ZONE_BASE_COUNT_ * _ParallelID ) )
		{
			CNetPart_PartBase::OnAddtoOtherSrv( _ItemID.Zone , SrvInfo->SrvID + _DEF_ZONE_BASE_COUNT_ * _ParallelID , _ItemID.GUID );
			CNetPart_PartBase::OnAdd( SrvInfo->SrvID + _DEF_ZONE_BASE_COUNT_ * _ParallelID  , _ItemID.GUID );
		}
	}
	//---------------------------------------------------------------------
	//檢查是否有換 其他 Srv 的控制
	SrvInfoStruct* CtrlSrvInfo = _SrvPart->GetCtrlSrv( _SecX , _SecY );
	if( CtrlSrvInfo != NULL )
	{
		if( CtrlSrvInfo->SrvID != _ItemID.Zone % _DEF_ZONE_BASE_COUNT_ )
		{
			int ToZoneID = Global::GetParallelZoneID( CtrlSrvInfo->SrvID , _OrgParallelID );
			if( ToZoneID != -1 )
				CNetPart_PartBase::ChangeZone( _ItemID.Zone , ToZoneID , _ItemID.GUID );
		}
	}   
	return true;
}

bool        BaseItemObject::_Del( )
{
	_AllObj[ _ItemID.Zone ][ _ItemID.ID ] = NULL;

	if(_IsAddAllObj == false )
	{
		return false;
	}

	//---------------------------------------------------------------------    
	//通知所有周圍的 Srv 有物件移除
	int     i;
	vector < SrvInfoStruct* >* SrvInfoList = _SrvPart->GetViewSrv( _SecX , _SecY );
	SrvInfoStruct* SrvInfo;
	if( SrvInfoList == NULL )
	{
		Print( Def_PrintLV3 , "_Del" , "取得 SrvInfoStruct 失敗" );        
		return false;
	}

	if( _IsAddSrv )
	{
		//---------------------------------------------------------------------
		for( i = 0 ; i < (int)SrvInfoList->size() ; i++  )
		{
			SrvInfo = (*SrvInfoList)[i];

			if( SrvInfo->SrvID == _ItemID.Zone )
				continue;

			CNetPart_PartBase::OnDelFromOtherSrv( _ItemID.Zone , SrvInfo->SrvID , _ItemID.GUID );
			CNetPart_PartBase::OnDel( SrvInfo->SrvID , _ItemID.GUID );
		}
		//---------------------------------------------------------------------
		_ObjPart->Del( _ItemID.GUID );
	}
	return true;
}

bool        BaseItemObject::SetPos( int X , int Y )
{
	int NewSecX = ( X - _SrvPart->MinX() ) / _BlockSize;
	int NewSecY = ( Y - _SrvPart->MinY() ) / _BlockSize;

	if( NewSecX == _SecX && NewSecY == _SecY )
		return false;

	//讓處理區塊邊線移動導致的重覆傳輸
	if(     abs( _SecX * _BlockSize + _BlockSize/2 - ( X - _SrvPart->MinX() ) ) <= _BlockSize 
		&&  abs( _SecY * _BlockSize + _BlockSize/2 - ( Y - _SrvPart->MinY() ) ) <= _BlockSize   )
		return false;    

	if( _ObjPart->Move( NewSecX , NewSecY , _ItemID.GUID ) == false )
		return false;


	vector<SrvInfoStruct*>*          CmpBlock = _SrvPart->CmpBlock( _SecX , _SecY ,NewSecX , NewSecY ) ;

	for( int i=0 ; i < (int)CmpBlock->size() ; i++ )
	{
		SrvInfoStruct* SrvInfo = (*CmpBlock)[i];
		if( SrvInfo == NULL )
			continue;

		if( SrvInfo->SrvID == _ItemID.Zone )
			continue;

		int SrvZoneID = SrvInfo->SrvID + _ParallelID * _DEF_ZONE_BASE_COUNT_;

		if( Global::CheckZoneID( SrvZoneID) == false )
			continue;

		switch( SrvInfo->TempCmpState )
		{
		case SrvCmpState_New :			
			CNetPart_PartBase::OnAddtoOtherSrv( _ItemID.Zone , SrvZoneID , _ItemID.GUID );
			CNetPart_PartBase::OnAdd( SrvZoneID , _ItemID.GUID );
			break;
		case SrvCmpState_Del :
			CNetPart_PartBase::OnDelFromOtherSrv( _ItemID.Zone , SrvZoneID , _ItemID.GUID );
			CNetPart_PartBase::OnDel( SrvZoneID , _ItemID.GUID );
			break;
		case SrvCmpState_Move:
			break;
		}
	}


	_SecX = NewSecX;
	_SecY = NewSecY;
	//檢查是否有換 其他 Srv 的控制
	SrvInfoStruct* CtrlSrvInfo = _SrvPart->GetCtrlSrv( _SecX , _SecY );
	if( CtrlSrvInfo != NULL )
	{
		if( CtrlSrvInfo->SrvID != _ItemID.Zone % _DEF_ZONE_BASE_COUNT_ )
		{
			int ToZoneID = Global::GetParallelZoneID( CtrlSrvInfo->SrvID , _OrgParallelID );
			if( ToZoneID != -1 )
				CNetPart_PartBase::ChangeZone( _ItemID.Zone , ToZoneID , _ItemID.GUID );
		}
	}   

	return true;
}
//*注意* 新增移除 Srv 會影響內容
vector < SrvInfoStruct* >*  BaseItemObject::GetViewSrv( )
{
	   return _SrvPart->GetViewSrv( _SecX , _SecY );
}