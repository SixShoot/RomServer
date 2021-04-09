#include "PathManage.h"
#include "..\Global.h"
#include <math.h>

#include "Rune Engine/Rune/Rune.h"
#include "Rune Engine/Rune/Rune Engine NULL.h"
#include "Rune Engine/Rune/Fusion/RuFusion_Trekker.h"


//暫時不處理路徑搜尋(用來時間控制)
bool			PathManageClass::_PathFindPauseFlag		= false;
//暫時不處理路徑搜尋(GM控制)
bool			PathManageClass::_GMPathFindPauseFlag	= false;

bool			PathManageClass::_IsReady = false;

bool			PathManageClass::_IsUsePathFind = true;

int				PathManageClass::_MinCtrlX = 0; 
int				PathManageClass::_MinCtrlZ = 0;
int				PathManageClass::_MaxCtrlX = 0;
int				PathManageClass::_MaxCtrlZ = 0;

CRuFusion_Trekker*	_Trekker = NULL;
CRuFusion_Trekker*	_Trekker_Maker = NULL;


enum TrekkerProcStateENUM
{
	EM_TrekkerProcState_Unknow	,
	EM_TrekkerProcState_PathCal	,
	EM_TrekkerProcState_PathOK	,
};

vector< CRuFusion_Trekker* >	_TrekkerList;
vector< TrekkerProcStateENUM >	_TrekkerList_PathState;
MutilThread_CritSect			_PathBuildThread_CritSect;

DWORD WINAPI PathManageClass::_BuildPathThreadFuction( void* _RoomID )
{
	int RoomID = (int)_RoomID;

	if( RoomID >= _TrekkerList.size() )
	{
		return 0;
	}

	CRuFusion_Trekker* RetTrekker = _TrekkerList[ RoomID ];
	if( RetTrekker == NULL )
	{
		return 0;
	}

	
	unsigned BeginTime = timeGetTime();
	_PathBuildThread_CritSect.Enter();
	RetTrekker->UpdateIndependentPathHierarchy();
	_PathBuildThread_CritSect.Leave();

	Print( LV4 , "Room_BuildPath" , "Build Time =%d" , timeGetTime() - BeginTime );

	_TrekkerList_PathState[ RoomID ] = EM_TrekkerProcState_PathOK;



}


CRuFusion_Trekker*	GetTrekker( int RoomID )
{
	if(		(unsigned)RoomID >= _TrekkerList_PathState.size() 
		||	_TrekkerList_PathState[ RoomID ] != EM_TrekkerProcState_PathOK )
		return _Trekker;

	if( RoomID >= _TrekkerList.size() )
		return _Trekker;

	CRuFusion_Trekker* RetTrekker = _TrekkerList[ RoomID ];
	if( RetTrekker == NULL )
		return _Trekker;
	return RetTrekker;
}

bool		PathManageClass::Room_CreatePath( int RoomID )
{
	return true;
	if( RoomID <= 0 || RoomID > 1000 )
		return false;
	
	while( _TrekkerList.size() <= RoomID )
	{
		_TrekkerList.push_back( NULL );
	}
	if( _TrekkerList[ RoomID ] != NULL )
		return false;

	CRuFusion_Trekker* RoomTrekker = ruNEW CRuFusion_Trekker( (CRuOptimalTerrain*)_Trekker_Maker->GetTerrain() );
	RoomTrekker->CreateIndependentPathHierarchy();
	_TrekkerList[ RoomID ] = RoomTrekker;
	return true;
}
bool		PathManageClass::Room_ReleasePath( int RoomID )
{
	return true;
	//_TrekkerList_PathState.[ RoomID ] =  EM_TrekkerProcState_PathCal;
	if( _TrekkerList_PathState.size() > (unsigned)RoomID )
	{
		if( _TrekkerList_PathState[ RoomID ]  == EM_TrekkerProcState_PathCal )
		{
			Print( LV4 , "Room_ReleasePath" , "_TrekkerList_PathState.[ %d ]  == EM_TrekkerProcState_PathCal" , RoomID );
			return false;
		}
	}

	if( unsigned( RoomID ) >= _TrekkerList.size() )
		return false;
	CRuFusion_Trekker* RoomTrekker = _TrekkerList[ RoomID ];

	if( RoomTrekker != NULL )
	{
		delete RoomTrekker;
		_TrekkerList[ RoomID ] = NULL;
	}
	return true;
}
bool		PathManageClass::Room_AddModel( int RoomID , float X , float Y , float Z , float Dir , const char* ModelStr )
{
	if( unsigned( RoomID ) >= _TrekkerList.size() )
		return false;
	CRuFusion_Trekker* RoomTrekker = _TrekkerList[ RoomID ];

	CRuEntity* ModelEntiry = g_ruResourceManager->LoadEntity( ModelStr );
	if( ModelEntiry == NULL )
		return false;

	float Angle = Dir * PI / 180.0f;

	ModelEntiry->Translate(CRuVector3(X,Y,Z));
	RuEntity_Orient( ModelEntiry , CRuVector3(0.0f, 0.0f, -1.0f), CRuVector3(cos(Angle), 0.0f, -sin(Angle)) , 1.0f );

	RoomTrekker->InsertModel( ModelEntiry );
	return true;
}
bool		PathManageClass::Room_BuildPath( int RoomID )
{	
	return false;

	if( unsigned( RoomID ) >= _TrekkerList.size() )
		return false;

	while(_TrekkerList_PathState.size() <= RoomID )
		_TrekkerList_PathState.push_back( EM_TrekkerProcState_Unknow );

	if( _TrekkerList_PathState[ RoomID ] ==  EM_TrekkerProcState_PathCal )
		return false;
	_TrekkerList_PathState[ RoomID ] =  EM_TrekkerProcState_PathCal;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	CreateThread( NULL , 0 , _BuildPathThreadFuction , (LPVOID) RoomID , 0 , NULL );
	return true;

}


//開關路徑收尋
void		PathManageClass::IsUsePathFind( bool Value )
{
	_IsUsePathFind = Value;
}

int		PathManageClass::FindPathNode( float X , float Y , float Z )
{
	CRuVector3 OriginPos( X , Y , Z );
	return GetTrekker(_RoomID)->FindPathNode( OriginPos , 1 );
}

//路徑搜尋
std::vector<POINT3D>* PathManageClass::FindPath( float X , float Y , float Z  )
{
	static std::vector<POINT3D> PointList;

	static CRuArrayList<PTRVALUE>	PathNodeOut;
	static CRuArrayList<CRuVector3> PathList;

	PointList.clear();

	int i;    

	if( X == -1 || Z == -1 )
		return &PointList;

	if( FloatEQU( X , _Pos.x ) && FloatEQU( Z , _Pos.z ) && FloatEQU( Y , _Pos.y ) )
		return &PointList;


	if( Height( X , Y , Z , Y , 100 ) != false )
	{
		CRuVector3 OriginPos( _Pos.x , _Pos.y , _Pos.z );
		CRuVector3 DestPos( X , Y , Z );

		INT64 TargetNodeID = GetTrekker(_RoomID)->FindPathNode( DestPos , 30 );
		if( _Pos.NodeID == 0 )
		{
			_Pos.NodeID = GetTrekker(_RoomID)->FindPathNode( OriginPos , 30 );
		}



		if( GetTrekker(_RoomID)->FindPath( _Pos.NodeID , TargetNodeID , &PathNodeOut ) != false )
		{
			if( GetTrekker(_RoomID)->CalculatePathPoints( OriginPos , DestPos , &PathNodeOut , &PathList ) != false )
			{
				POINT3D P;
				P.x = X; 
				P.y = Y; 
				P.z = Z; 
				P.NodeID = TargetNodeID;
				PointList.push_back(  P );
				for( i = PathList.Count()-2 ; i > 1 ; i-- )
				{        
					P.x = PathList[i].x; 
					P.y = PathList[i].y; 
					P.z = PathList[i].z; 
					P.NodeID = PathNodeOut[i-1];
					//////////////////////////////////////////////////////////////////////////
					//測試code
					if( P.x == 0 && P.y == 0 && P.z == 0 )
					{
						Print( LV1 , "PathManageClass::FindPath" , "Err OrgPos(%f,%f,%f) TargetPos(%f,%f,%f)" , _Pos.x , _Pos.y , _Pos.z , X , Y , Z );						
						continue;
					}
					//如果距離過遠則把此結點刪除
					{
						float dx = P.x - PointList.back().x;
						float dy = P.y - PointList.back().y;
						float dz = P.z - PointList.back().z;
						float Length = dx*dx + dy*dy +dz*dz;
						
						if( Length > 40000 )
						{
							Print( LV1 , "PathManageClass::FindPath" , "Err OrgPos(%f,%f,%f) TargetPos(%f,%f,%f) Length =%d" , _Pos.x , _Pos.y , _Pos.z , X , Y , Z , int( sqrt(Length) ) );
							continue;
						}

					}
					//////////////////////////////////////////////////////////////////////////
					PointList.push_back(  P );
				}
			}
		}
	}	
	
	return &PointList;

}
bool	PathManageClass::Target( float TX , float TY , float TZ , float NowX , float NowY , float NowZ )
{
	float Dx = TX - NowX;
	float Dy = TY - NowY;
	float Dz = TZ - NowZ;
	float Len2 = Dx*Dx + Dy*Dy+Dz*Dz;
	if(		Len2 > 200*200	
		||	Dy > 30 
		||	CheckLine( TX , TY , TZ , NowX , NowY , NowZ ) == false )
		return Target( TX , TY , TZ );
	else
		return Target( TX , TY , TZ , false );
}

//如果長距離搜尋不到回傳false	
bool	PathManageClass::Target( float X , float Y , float Z , bool IsUsePath )
{
	if( _IsUsePathFind == false )
		IsUsePath = false;

	if( X == -1 || Z == -1 )
        return false;

	if( IsUsePath != false )
	{
		if( FloatEQU( X , _Pos.x ) && FloatEQU( Z , _Pos.z ) )
			return true;
	}
	else
	{
		if( FloatEQU( X , _Pos.x ) && FloatEQU( Y , _Pos.y ) && FloatEQU( Z , _Pos.z ) )
			return true;

	}


	static int ProcessTimes		= 0;
	static int PathNotFindTimes = 0;
	static int Clock			= clock();
	ProcessTimes++;

	if( clock() - Clock >= 1000  )
	{
		Clock += 1000;
		Print( LV1 , "Target" , " Set Target ProcessTimes=%d PathNotFindTimes=%d" , ProcessTimes , PathNotFindTimes );
		PathNotFindTimes = 0;
		ProcessTimes = 0;
	}

	static CRuArrayList<PTRVALUE>	PathNodeOut;
	static CRuArrayList<CRuVector3> PathList;
	int i;    


//	int	DebugTime = clock();
	_Stack.clear();	

	float Lx, Lz;

	if( IsUsePath )
	{
		//if( Height( X , Y , Z , Y , 100 ) != false )
		{
			CRuVector3 OriginPos( _Pos.x , _Pos.y , _Pos.z );
			CRuVector3 DestPos( X , Y , Z );

			INT64 TargetNodeID = GetTrekker(_RoomID)->FindPathNode( DestPos , 30 );
			if( _Pos.NodeID == 0 )
			{
				_Pos.NodeID = GetTrekker(_RoomID)->FindPathNode( OriginPos , 30 );
			}



			if( GetTrekker(_RoomID)->FindPath( _Pos.NodeID , TargetNodeID , &PathNodeOut ) != false )
			{
				if( GetTrekker(_RoomID)->CalculatePathPoints( OriginPos , DestPos , &PathNodeOut , &PathList ) != false )
				{
					Lx = X ; Lz = Z;
					PushPos( X , Y , Z , TargetNodeID );
					for( i = PathList.Count()-2 ; i > 1 ; i-- )
					{       
						//////////////////////////////////////////////////////////////////////////
						//測試code
						if( PathList[i].x == 0 && PathList[i].y == 0 && PathList[i].z == 0 )
						{
							Print( LV1 , "PathManageClass::Target" , "Err OrgPos(%f,%f,%f) TargetPos(%f,%f,%f)" , _Pos.x , _Pos.y , _Pos.z , X , Y , Z );						
							continue;
						}
						//////////////////////////////////////////////////////////////////////////
						PushPos( PathList[i].x , PathList[i].y , PathList[i].z , PathNodeOut[i-1] );

						Lx = PathList[i].x;
						Lz = PathList[i].z;
					}

					_LastSearchStepCount = (int)_Stack.size();
					_UseCount = 0;
					return true;
				}
			}
		}	
	}

	//找不到路徑
	PathNotFindTimes++;

	int Count = int(max( abs( _Pos.x - X ) , abs( _Pos.z - Z) )/80 );
	if( Count == 0 || Count > 1000 )
		Count = 1;

	float Dx = ( _Pos.x - X ) / (float)Count;
	float Dy = ( _Pos.y - Y ) / (float)Count;
	float Dz = ( _Pos.z - Z ) / (float)Count;

	for( i = 0 ; i < Count ; i++ )
	{        
		float tY = 0;
//		Height( X , Y+10 , Z  ,tY , 200 );
		PushPos( X , Y , Z , 0 );
		X += Dx;
		Y += Dy;
		Z += Dz;		
	}

	return true;
}


//取得下一點的位置
int		PathManageClass::NextPoint(float &NX,float &NY , float &NZ , float Leng )
{
	if( StackCount() == 0 ) 
		return ENUM_Path_Search_End;

	POINT3D Pos = _Stack.back();
	float Dx = Pos.x - _Pos.x;
	float Dz = Pos.z - _Pos.z;

	float NextPointLen = sqrt( Dx*Dx + Dz*Dz );
	int CutCount = int(NextPointLen / Leng) + 1;


	if( CutCount <= 1 )
	{
		_Stack.pop_back();
	}
	else
	{
		Pos.x = Pos.x - Dx  * (CutCount-1) / CutCount;
		Pos.z = Pos.z - Dz  * (CutCount-1) / CutCount;
	}
	
	_UseCount ++;

    _Pos.x = NX = Pos.x;
    _Pos.y = NY = Pos.y;
    _Pos.z = NZ = Pos.z;
	_Pos.NodeID = Pos.NodeID;
	return ENUM_Path_Search_OK;
}

void	PathManageClass::SetPos_NoCheckHeight( float x , float y , float z )
{
	_TargetPos.x = _Pos.x = x;
	_TargetPos.y = _Pos.y = y;			
	_TargetPos.z = _Pos.z = z;	

	_TargetPos.NodeID = _Pos.NodeID = 0;

	ClearPath();
}

//設定目前位置
float	PathManageClass::SetPos( float x , float y , float z )
{
	Height( x , y , z , y , 40 );
	_TargetPos.x = _Pos.x = x;
	_TargetPos.y = _Pos.y = y;			
	_TargetPos.z = _Pos.z = z;	

	_TargetPos.NodeID = _Pos.NodeID = 0;
	//_Trekker->FindPathNode( CRuVector3( x , y , z ) , 30 );


	ClearPath();
	return y;
}

//丟一部路徑倒堆疊中
void	PathManageClass::PushPos( float x, float y , float z , INT64 NodeID )
{
	POINT3D	Value;
	Value.x = x;
	Value.y = y;
	Value.z = z;
	Value.NodeID = NodeID;

	_Stack.push_back( Value );
}

//取得目地點
bool	PathManageClass::GetTargetPoint( float& x , float& y , float& z )
{
	

	x = _TargetPos.x;
	y = _TargetPos.y;
	z = _TargetPos.z;
	if(_Stack.size() == 0 || _Stack.size() > 4 )
	{
		x = _Pos.x;
		y = _Pos.y;
		z = _Pos.z;
	}
	else
	{
		x = _Stack[0].x;
		y = _Stack[0].y;
		z = _Stack[0].z;
	}
	
	return true;
}

float		PathManageClass::TotalLength3D()
{
	int		    i;
	float   	Length = 0;
	float		DX , DY , DZ;


	POINT3D     P1 , P2;

	P1 = _Pos;

	for( i = (int)_Stack.size()-1 ;  i >= 0  ; i-- , P1 = P2 )
	{
		P2 = _Stack[i];
		DX = P1.x - P2.x;
		DY = P1.y - P2.y;
		DZ = P1.z - P2.z;

		float Temp = (float)( DX*DX + DY*DY + DZ*DZ );
		Length += sqrt( Temp );
	}

	return Length;
}

//全長度
float		PathManageClass::TotalLength()
{
	int		    i;
	float   	Length = 0;
	float		DX , DY , DZ;

    //_Pos
    POINT3D     P1 , P2;

    P1 = _Pos;
	
	for( i = (int)_Stack.size()-1 ;  i >= 0  ; i-- , P1 = P2 )
	{
        P2 = _Stack[i];
		DX = P1.x - P2.x;
//		DY = P1.y - P2.y;
		DZ = P1.z - P2.z;
		DY = 0;
        
        float Temp = (float)( DX*DX + DY*DY + DZ*DZ );
		Length += sqrt( Temp );
	}
/*
	if( _Stack.size() != 0 )
	{
		P2 = _Stack[0];
		DX = _Pos.x - P2.x;
//		DY = _Pos.y - P2.y;
		DZ = _Pos.z - P2.z;
		DY = 0;

		float Temp = (float)( DX*DX + DY*DY + DZ*DZ );
		Length += sqrt( Temp );
	}
	*/
	return Length;
}

//記算有多少個移動點( 撿查是否要移動太久)
int	PathManageClass::CountNode(  float TX , float TY , float TZ )
{
	static CRuArrayList<PTRVALUE>	PathNodeOut;

	CRuVector3 OriginPos( _Pos.x , _Pos.y , _Pos.z );
	CRuVector3 DestPos( TX , TY , TZ );

	INT64 TargetNodeID = _Trekker->FindPathNode( DestPos , 30 );

	if( _Pos.NodeID == 0 )
	{
		_Pos.NodeID = GetTrekker(_RoomID)->FindPathNode( OriginPos , 30 );
	}


	if( GetTrekker(_RoomID)->FindPath( _Pos.NodeID , TargetNodeID , &PathNodeOut ) == FALSE )
		return 0xffff;

	if( PathNodeOut.Count() == 0 )
		return 0;

	return PathNodeOut.Count();
	
}

bool PathManageClass::InitBase( const char* FilePath , const char* File )
{
	bool	Ret = true;

	Print( LV5 , "PathManageClass::Init" , "RuInitialize_Core()" );
	if( RuInitialize_Core() == FALSE )
	{
		Print( LV2 , "PathManageClass::Init" , "RuInitialize_Core() == FALSE " );
		Ret = false;
	}

	Print( LV5 , "PathManageClass::Init" , "RuInitialize_NULL()" );
	// Initialize NULL engine
	if( RuInitialize_NULL() == FALSE )
	{
		Print( LV2 , "PathManageClass::Init" , "RuInitialize_NULL() == FALSE " );
		Ret = false;
	}

	// Setup resource manager's root path
	g_ruResourceManager->SetRootDirectory( FilePath );

	return Ret;
}

bool PathManageClass::Init( const char* FilePath , const char* File , int MinX , int MinZ , int MaxX , int MaxZ , bool IsNeedPathMaker )
{
	bool	Ret = true;

	g_ruResourceManager->LoadPackageFile("fdb\\model.fdb");
	// Initialize collision system
	_Trekker = ruNEW CRuFusion_Trekker( CombineFilePath( FilePath , File  ) );

	if( Ret == false )
		return false;

	_MinCtrlX = MinX;
	_MinCtrlZ = MinZ;
	_MaxCtrlX = MaxX;
	_MaxCtrlZ = MaxZ;

	Print( LV5 , "PathManageClass::Init" , "preloadRegion" );

	CRuAABB preloadRegion(CRuVector3( float( _MinCtrlX ), -10000.0f, float( _MinCtrlZ ) )
						, CRuVector3( float( _MaxCtrlX ), 10000.0f , float( _MaxCtrlZ ) ));
	_Trekker->PreLoad(preloadRegion);

	_Trekker->SetPathMaximumExpansion(2048);

	Print( LV5 , "PathManageClass::Init" , "Init OK " );

	_IsReady = true;

	if( IsNeedPathMaker != false )
	{
		_Trekker_Maker = ruNEW CRuFusion_Trekker( CombineFilePath( FilePath , File  ) );
		Print( LV5 , "PathManageClass::Init" , "preloadRegion PatherMaker" );
		_Trekker_Maker->PreLoad(preloadRegion);
		_Trekker_Maker->SetPathMaximumExpansion(2048);
		Print( LV5 , "PathManageClass::Init" , "Init OK PatherMaker" );
		_IsReady = true;
	}
	else
	{
		_Trekker_Maker = _Trekker;
	}


	return true;
}

//資料出始化
bool	PathManageClass::Init( const char* FilePath , const char* File , int MapID , bool IsNeedPathMaker )
{
	bool	Ret = true;

	Print( LV5 , "PathManageClass::Init" , "RuInitialize_Core()" );
	if( RuInitialize_Core() == FALSE )
	{
		Print( LV2 , "PathManageClass::Init" , "RuInitialize_Core() == FALSE " );
		Ret = false;
	}

	Print( LV5 , "PathManageClass::Init" , "RuInitialize_NULL()" );
	// Initialize NULL engine
	if( RuInitialize_NULL() == FALSE ) 
	{
		Print( LV2 , "PathManageClass::Init" , "RuInitialize_NULL() == FALSE " );
		Ret = false;
	}


	// Setup resource manager's root path
	g_ruResourceManager->SetRootDirectory( FilePath );

	if( g_ruResourceManager )
	{
		g_ruResourceManager->LoadPackageFile("fdb\\model.fdb");
	}
	Print( LV5 , "PathManageClass::Init" , "ruNew CRuFusion_Trekker" );
	// Initialize collision system
	_Trekker = ruNEW CRuFusion_Trekker( CombineFilePath( FilePath , File  ) );

	if( Ret == false )
		return false;

	CRuAABB controlBoundsOut;
	_Trekker->FindMaximalZoneControlBounds( MapID , &controlBoundsOut );

	_MinCtrlX = int( controlBoundsOut.GetMinPoint().x );
	_MinCtrlZ = int( controlBoundsOut.GetMinPoint().z );
	_MaxCtrlX = int( controlBoundsOut.GetMaxPoint().x );
	_MaxCtrlZ = int( controlBoundsOut.GetMaxPoint().z );

	Print( LV5 , "PathManageClass::Init" , "preloadRegion" );

	CRuAABB preloadRegion(CRuVector3( float( _MinCtrlX ), -10000.0f, float( _MinCtrlZ ) )
						, CRuVector3( float( _MaxCtrlX ), 10000.0f , float( _MaxCtrlZ ) ));
	_Trekker->PreLoad(preloadRegion);

	_Trekker->SetPathMaximumExpansion(2048);

	Print( LV5 , "PathManageClass::Init" , "Init OK" );

	_IsReady = true;

	if( IsNeedPathMaker != false )
	{
		/*
		_Trekker_Maker = ruNEW CRuFusion_Trekker( CombineFilePath( FilePath , File  ) );
		Print( LV5 , "PathManageClass::Init" , "preloadRegion PatherMaker" );
		_Trekker_Maker->PreLoad(preloadRegion);
		_Trekker_Maker->SetPathMaximumExpansion(2048);
		Print( LV5 , "PathManageClass::Init" , "Init OK PatherMaker" );
		*/
		_Trekker_Maker = _Trekker;
		
	}
	else
	{
		_Trekker_Maker = _Trekker;
	}

	return true;
}
bool	PathManageClass::Release( )
{

	if( _IsReady == false ) 
		return false;

	RuShutdown();

//	ruSAFE_RELEASE( _Trekker );

	return true;
}

//尋找某一點附近的點
bool 	PathManageClass::Height( float x , float y , float z ,float &Outy, float H )
{
	//	return true;
	if(	   CheckFloatAvlid(x) == false 
		|| CheckFloatAvlid(y) == false 
		|| CheckFloatAvlid(z) == false )
		return false;

	CRuVector3 Origin;
	Origin.x = x;
	Origin.y = y+5;
	Origin.z = z;	

	if( GetTrekker(_RoomID)->FindRestingHeight( Origin , H , Outy ) == FALSE )
	{
		Outy = y;
		return false;
	}
	
	return true;	
}
PathManageClass::PathManageClass()
{ 
	_LastSearchStepCount = 0; 
	_Pos.Init();
	_TargetPos.Init();
	_LastSearchPos.Init();
	_UseCount = 0;	
	_RoomID = 0;
}

bool	PathManageClass::CheckLine( float x , float y , float z , float x1 , float y1 , float z1 )
{
	if( GetTrekker(_RoomID)->IsPathClear(CRuVector3( x, y+5 , z ), CRuVector3( x1, y1+5, z1 ) ) == FALSE )
	{
		if( GetTrekker(_RoomID)->IsPathClear(CRuVector3( x, y+15 , z ), CRuVector3( x1, y1+15, z1 ) ) == FALSE )
			return false;//測障礙
	}

	return true;
}

bool	PathManageClass::CheckLine_Magic( float x , float y , float z , float x1 , float y1 , float z1 )
{
	if( GetTrekker(_RoomID)->IsPathClear(CRuVector3( x, y+10 , z ), CRuVector3( x1, y1+10, z1 ) , FALSE ) == FALSE )
		return false;//測障礙

	return true;
}


bool	PathManageClass::CheckLine_Pure( float x , float y , float z , float x1 , float y1 , float z1 )
{
	if( GetTrekker(_RoomID)->IsPathClear(CRuVector3( x, y , z ), CRuVector3( x1, y1 , z1 ) ) == FALSE )
		return false;//測障礙

	return true;

}

//位置名稱
char* PathManageClass::MapName( float x , float y , float z )
{
	return (char*)_Trekker->FindZoneName( CRuVector3( x, y , z )  );
}

char* PathManageClass::MapName1( float x , float y , float z )
{
	char KeyStr[256];
	string Str = _Trekker->FindZoneName( CRuVector3( x, y , z )  );
	replace( Str.begin() , Str.end() , '\'' , ' ' );
	
	sprintf_s( KeyStr , sizeof(KeyStr) , "ZONE_%s" , _strupr( (char*)Str.c_str() ) );

	return (char*)g_ObjectData->GetString( KeyStr );

}

//取得主要區域
int	PathManageClass::MainMapID( float x , float y , float z )
{
	return _Trekker->FindRootZoneID( CRuVector3( x, y , z )  );
}


std::vector<POINT3D>*	PathManageClass::FindNeighborPoint( float X , float Y , float Z , float Radius, int Divisions )
{

	static std::vector<POINT3D> PointList;
	PointList.clear();
/*
	for( int i = 0 ; i < Divisions ; i++ )
	{
		POINT3D Temp;
		Temp.x =  X + sin( 2* PI * i / Divisions ) * Radius;
		Temp.y =  Y;
		Temp.z =  Z + cos( 2* PI * i / Divisions ) * Radius;
		PointList.push_back( Temp );
	}
	*/

	CRuVector3 Origin;
	Origin.x = X;
	Origin.y = Y;
	Origin.z = Z;

	CRuArrayList<CRuVector3> CRuVecorPoint;
	if( Divisions >= 1000 )
		Divisions = 1;

	GetTrekker(_RoomID)->FindNeighborPoints(Origin, Radius, 100 , 100 , Divisions, &CRuVecorPoint );

	for( int i = 0 ; i < CRuVecorPoint.Count() ; i++ )
	{
		POINT3D Temp;
		CRuVector3 &RuPoint = CRuVecorPoint[i];
		Temp.x =  RuPoint.x;
		Temp.y =  RuPoint.y;
		Temp.z =  RuPoint.z;
		PointList.push_back( Temp );
	}
	
	return &PointList;
}

//找重生點
bool	PathManageClass::FindZoneRespawnPoint( float X , float Y , float Z , POINT3D& Point )
{
	CRuVector3 cRuPoint;
	CRuVector3 cRuOutPoint;
	cRuPoint.x = X;
	cRuPoint.y = Y;
	cRuPoint.z = Z;
	
	if( GetTrekker(_RoomID)->FindZoneRespawnPoint( cRuPoint , cRuOutPoint ) == FALSE )
		return false;

	Point.x = cRuOutPoint.x;
	Point.y = cRuOutPoint.y;
	Point.z = cRuOutPoint.z;
	
	return true;
}
bool PathManageClass::IsDeadZone( float X , float Y , float Z)
{
	CRuVector3 cRuPoint;	
	cRuPoint.x = X;
	cRuPoint.y = Y;
	cRuPoint.z = Z;
	return ( TRUE == _Trekker->IsDeadZone( cRuPoint ) );
}

int  PathManageClass::GetZoneType(float X , float Y , float Z , float* damageFactorOut)
{
	CRuVector3 cRuPoint;	
	cRuPoint.x = X;
	cRuPoint.y = Y;
	cRuPoint.z = Z;
	return _Trekker->GetZoneType( cRuPoint , damageFactorOut);
}


int PathManageClass::FindMapID( float X , float Y , float Z )
{
	CRuVector3 cRuPoint;	
	cRuPoint.x = X;
	cRuPoint.y = Y;
	cRuPoint.z = Z;
	
	return _Trekker->FindZoneID( cRuPoint );
}
bool PathManageClass::CheckMapID( float X , float Y , float Z , int MapID )
{
	int findMapID = FindMapID( X , Y , Z );

	CRuArrayList<CRuWorld_ZoneDescriptor *> zoneDescriptorsOut;
	_Trekker->EnumerateSubZones( MapID , &zoneDescriptorsOut);

	for( int i = 0 ; i < zoneDescriptorsOut.Count() ; i++ )	
	{		
		if( zoneDescriptorsOut[i]->GetZoneID() == findMapID )
			return true;
	}
	return false;
}
