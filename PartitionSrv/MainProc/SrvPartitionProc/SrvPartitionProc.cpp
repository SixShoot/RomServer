#include "srvpartitionproc.h"
#include <algorithm>
#include <functional>      // For greater<int>( )
#include "../Global.h"

#include "Rune Engine/Rune/Rune.h"
#include "Rune Engine/Rune/Rune Engine NULL.h"
#include "Rune Engine/Rune/Fusion/RuFusion_Trekker.h"
//CRuFusion_Trekker*	Trekker = NULL;

SrvPartitionProc::SrvPartitionProc( const char* MapFileName )
{
	unsigned int	i;

	//取得控制範圍
	CRuFusion_Trekker*	Trekker = NULL;

	string  RootPath = Global::Ini()->Str("ResourcePath");
	// Initialize collision system
	Trekker = ruNEW CRuFusion_Trekker( CombineFilePath( RootPath.c_str() , MapFileName ) );

	CRuAABB mapBoundsOut;
	Trekker->GetMapBounds( &mapBoundsOut );

	_MapFileName = MapFileName;
	_BlockSize = 160;

	_MinX	= int( mapBoundsOut.GetMinPoint().m_x /_BlockSize * _BlockSize );
	_MinY   = int( mapBoundsOut.GetMinPoint().m_z /_BlockSize * _BlockSize );
	_Width  = unsigned( mapBoundsOut.GetXSize() / _BlockSize );
	_Height = unsigned( mapBoundsOut.GetZSize() / _BlockSize );

	_ViewPartition = (vector < SrvInfoStruct* >**)new char[ sizeof( char*) * _Width ];

	for( i = 0 ; i < _Width ; i++ )
		_ViewPartition[ i ] = new vector < SrvInfoStruct* >[ _Height ];

	_CtrlPartition = (SrvInfoStruct***)new char[ sizeof( char*) * _Width ];

	for( i = 0 ; i < _Width ; i++ )
	{
		_CtrlPartition[ i ] = (SrvInfoStruct**)new char[ sizeof( char*) * _Height  ];
		memset( _CtrlPartition[ i ] , 0 , sizeof( char*) * _Height );
	} 

	ruSAFE_RELEASE( Trekker );
}
SrvPartitionProc::~SrvPartitionProc( )
{
	unsigned int i;
	for( i = 0 ; i < _Width ; i++ )
		delete [] _ViewPartition[ i ];

	delete _ViewPartition;

	for( i = 0 ; i < _Width ; i++ )
		delete [] _CtrlPartition[ i ];

	delete _CtrlPartition;

	for( i = 0 ; i < (int)_SrvInfoList.size() ; i++ )
		delete _SrvInfoList[i];
}

struct  PointXY
{
	unsigned int X;
	unsigned int Y;
	bool operator < ( const PointXY& P ) const
	{
		if( X < P.X )
			return true;
		if( X > P.X )
			return false;
		if( Y < P.Y )
			return true;
		return false;
	}
};

//SubMapID 此區域使用圖檔的區塊
bool SrvPartitionProc::Add( int SrvID , int SubMapID )
{

	SrvInfoStruct *SrvInfo;
	vector< SrvInfoStruct* >::iterator Iter;

	while( (int)_SrvInfoList.size() <= SrvID )
		_SrvInfoList.push_back( new  SrvInfoStruct);

	SrvInfo = _SrvInfoList[SrvID];
	if( SrvInfo->SrvID != -1 )
		return false;	//此Server已註冊過
	
	SrvInfo->SrvID = SrvID;
	SrvInfo->SubMapID = SubMapID;

	//取得控制範圍
	CRuFusion_Trekker*	Trekker = NULL;

	string  RootPath = Global::Ini()->Str("ResourcePath");
	// Initialize collision system
	//Trekker = ruNEW CRuFusion_Trekker( _MapFileName.c_str() );
	Trekker = ruNEW CRuFusion_Trekker( CombineFilePath( RootPath.c_str() , _MapFileName.c_str() ) );
	CRuArrayList<CRuAABB> controlRectsOut;

	Trekker->FindZoneControlRects( SubMapID , (float)_BlockSize , &controlRectsOut );

	int x,z;
	vector < SrvInfoStruct* >* PartPtr;

	set< PointXY > ViewBlockSet;
	PointXY TempPoint;
	
	for( int i = 0 ; i < controlRectsOut.Count() ; i++ )
	{
		CRuAABB& TempCRuAABB = controlRectsOut[i];
		x = ( int( TempCRuAABB.GetMinPoint().m_x + 1 ) - _MinX )  / int(_BlockSize) ;
		z = ( int( TempCRuAABB.GetMinPoint().m_z + 1 ) - _MinY )  / int(_BlockSize) ;

		if( x >= _Width )
			x = _Width -1;
		else if( x < 0 )
			x = 0;
		if( z >= _Height )
			z = _Height -1;
		else if( z < 0 )
			z = 0;



		SrvInfoStruct** Info = &(_CtrlPartition[x][z]);
		if( *Info != NULL )
		{
			Print(LV2, "SrvPartitionProc", "Add _CtrlPartition x = %d  z = %d Pos(%d,%d)已有資料\n" , x , z 
				, int( TempCRuAABB.GetMinPoint().m_x +1 ) 
				, int( TempCRuAABB.GetMinPoint().m_z +1 ) );
		}
		*Info = SrvInfo;

		for( int a = -5 ; a <= 5 ; a++ )
		{
			for( int b = -5 ; b <= 5 ; b++ )
			{
				TempPoint.X = x + a;
				TempPoint.Y = z + b;

				if(		TempPoint.X >= _Width 
					||	TempPoint.Y >= _Height )
					continue;

				if( ViewBlockSet.find( TempPoint ) != ViewBlockSet.end() )
					continue;

				ViewBlockSet.insert( TempPoint );

				PartPtr = &(_ViewPartition[TempPoint.X][TempPoint.Y]);
				Iter = lower_bound( PartPtr->begin() , PartPtr->end() , SrvInfo );
				PartPtr->insert( Iter , SrvInfo );
			}
		}
	}
	
	ruSAFE_RELEASE( Trekker );
	return true;
}

bool SrvPartitionProc::Del( int SrvID )
{
	SrvInfoStruct *SrvInfo;
	vector< SrvInfoStruct* >::iterator Iter;

	if( (int)_SrvInfoList.size() <= SrvID )
		return false;

	SrvInfo = _SrvInfoList[SrvID];
	if( SrvInfo->SrvID == -1 )
		return false;

	unsigned int x,y;

	for( x = 0 ; x < _Width ; x++ )
	{
		for( y = 0 ; y < _Height ; y++ )
		{
			vector < SrvInfoStruct* >* PartPtr = &(_ViewPartition[x][y]);
			Iter = find( PartPtr->begin() , PartPtr->end() , SrvInfo );
			if( Iter == PartPtr->end() )
				return false;

			PartPtr->erase( Iter );
		}
	}

	//--------------------------------------------------------------------------------------------------------------------
	for( x = 0 ; x < _Width ; x++ )
	{
		for( y = 0 ; y < _Height ; y++ )
		{
			SrvInfoStruct** Info = &(_CtrlPartition[x][y]);
			if( *Info == NULL )
			{
				Print(LV2, "SrvPartitionProc", "Del _CtrlPartition x = %d  y = %d 資料已移除\n", x , y );
				continue;
			}
			if( *Info != SrvInfo )
			{
				Print(LV2, "SrvPartitionProc", "Del _CtrlPartition x = %d  y = %d 資料不同步\n", x , y );
			}
			*Info = NULL;
		}
	}
	//--------------------------------------------------------------------------------------------------------------------

	SrvInfo->Init();
	//--------------------------------------------------------------------------------------------------------------------

	return true;
}

SrvInfoStruct*          SrvPartitionProc::GetSrv( int SrvID )
{
	if( (int)_SrvInfoList.size() <= SrvID )
		return NULL;

	return _SrvInfoList[ SrvID ];
}
//使用真實的位置
vector<SrvInfoStruct*>* SrvPartitionProc::GetViewSrv( int X , int Y )
{

	if( (unsigned)X >= _Width || (unsigned)Y >= _Height )
		return NULL;

	return &(_ViewPartition[X][Y]);
}
SrvInfoStruct* SrvPartitionProc::GetCtrlSrv( int X , int Y )
{
	if( (unsigned)X >= _Width || (unsigned)Y >= _Height )
		return NULL;

	return _CtrlPartition[X][Y];
}
vector<SrvInfoStruct*>* SrvPartitionProc::CmpBlock( int OrgX , int OrgY , int DesX , int DesY )
{
	int     i;
	if( OrgX < 0 )         OrgX = 0;
	if( OrgY < 0 )         OrgY = 0;
	if( OrgX >= (int)_Width )   OrgX = _Width - 1;
	if( OrgY >= (int)_Height )  OrgY = _Height -1;

	if( DesX < 0 )         DesX = 0;
	if( DesY < 0 )         DesY = 0;
	if( DesX >= (int)_Width )   DesX = _Width - 1;
	if( DesY >= (int)_Height )  DesY = _Height -1;

	vector<SrvInfoStruct*>* OrgList = GetViewSrv( OrgX , OrgY );
	vector<SrvInfoStruct*>* DesList = GetViewSrv( DesX , DesY );

	int OrgID = 0;
	int DesID = 0;


	SrvInfoStruct* OrgInfo;
	SrvInfoStruct* DesInfo;
	_RetList.clear();
	while( 1 )
	{
		if( (int)OrgList->size() <= OrgID )
		{
			for( i = DesID ; i < (int)DesList->size() ; i++ )
			{
				DesInfo = (*DesList)[ i ];
				DesInfo->TempCmpState = SrvCmpState_New;
				_RetList.push_back( DesInfo ); 
			}
			break;
		}
		else if( (int)DesList->size() <= DesID )
		{
			for( i = OrgID ; i < (int)OrgList->size() ; i++ )
			{
				OrgInfo = (*OrgList)[ i ];
				OrgInfo->TempCmpState = SrvCmpState_Del;
				_RetList.push_back( OrgInfo ); 
			}
			break;
		}

		OrgInfo = (*OrgList)[OrgID];
		DesInfo = (*DesList)[DesID];
		if( OrgInfo == DesInfo )
		{
			OrgID++;
			DesID++;
			OrgInfo->TempCmpState = SrvCmpState_Move;
			_RetList.push_back( OrgInfo );
		}
		else if( OrgInfo > DesInfo )
		{
			DesID++;
			DesInfo->TempCmpState = SrvCmpState_New;
			_RetList.push_back( DesInfo );            
		}
		else if( OrgInfo < DesInfo )
		{
			OrgID++;
			OrgInfo->TempCmpState = SrvCmpState_Del;
			_RetList.push_back( OrgInfo );            
		}
	}

	return &_RetList;
}