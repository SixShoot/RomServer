#ifndef __PATHMANAGE_2003_11_10__
#define __PATHMANAGE_2003_11_10__
#pragma warning (disable:4786)

#include <vector>
#include <windows.h>
#include "BaseFunction/BaseFunction.h"

//#include "Rune/Rune.h"
//#include "Rune/Fusion/RuFusion_Trekker.h"

#pragma comment (lib, "comctl32")
#pragma comment (lib, "winmm")

#define _Path_Block_Size_		50

//路徑收尋結果
enum	PathSearchResultENUM
{
	ENUM_Path_Search_End		=	0	,		//找到目的 或結束搜尋
	ENUM_Path_Search_Wait		=	1	,		//因系統資源或設定導致暫停
	ENUM_Path_Search_OK			=	2
};

struct POINT3D
{
	float			x , y , z;
	INT64	NodeID;

    bool operator == ( const POINT3D& P ) const
    {
        if(     FloatEQU( x , P.x ) 
            &&  FloatEQU( y , P.y ) 
            &&  FloatEQU( z , P.z ) )
            return true;
        return false;
    };

	void Init()
	{
		memset( this , 0 , sizeof(*this) );
	}
};

class PathManageClass
{	
	//暫時不處理路徑搜尋(用來時間控制)
	static bool				_PathFindPauseFlag;

	//暫時不處理路徑搜尋(GM控制)
	static bool				_GMPathFindPauseFlag;

	static int				_MinCtrlX , _MinCtrlZ;
	static int				_MaxCtrlX , _MaxCtrlZ;

	int						_RoomID;

	//目前位置
    POINT3D                 _Pos;

	//目標位置
    POINT3D                 _TargetPos;

	//最後一次搜尋的目標
    POINT3D                 _LastSearchPos;
		

	//目前重堆疊內取了幾個資料
	short					_UseCount;

	//存放路徑的結構
    std::vector<POINT3D>	_Stack;

	//路徑收尋的步數
	int						_LastSearchStepCount;

	//初始成 
	static bool				_IsReady;

	static bool				_IsUsePathFind;

public:
	//資料出始化
	static bool InitBase( const char* FilePath , const char* File );
	static bool Init( const char* FilePath , const char* File , int MapID , bool IsNeedPathMaker = false );
	static bool Init( const char* FilePath , const char* File , int MinX , int MinZ , int MaxX , int MaxZ , bool IsNeedPathMaker = false );
	static bool Release( );

	//初始化
	PathManageClass();
    ~PathManageClass(){};

	//暫時不處理路徑搜尋
	static	void PathFindPauseFlag( bool F ){ _PathFindPauseFlag = F|_GMPathFindPauseFlag; }
	static	bool PathFindPauseFlag( ){ return _PathFindPauseFlag; }

	static	void GMPathFindPauseFlag( bool F ){ _GMPathFindPauseFlag = F; }
	static	bool GMPathFindPauseFlag( ){ return _GMPathFindPauseFlag; }

	//尋找某一點附近的點
	bool 	Height( float x , float y , float z ,float &Outy, float H );

	//開關路徑收尋
	static void		IsUsePathFind( bool Value );
	static bool		IsUsePathFind( ) { return _IsUsePathFind; };

	
	//公會戰場額外路徑
	static bool		Room_CreatePath( int RoomID );
	static bool		Room_ReleasePath( int RoomID );
	static bool		Room_AddModel( int RoomID , float X , float Y , float Z , float Dir , const char* ModelStr );
	static bool		Room_BuildPath( int RoomID );

	static DWORD WINAPI _BuildPathThreadFuction( void* RoomID );	

	//設定所在房間
	void	RoomID( int Value ) { _RoomID = Value; };

	//如果長距離搜尋不到回傳false	
	//Lock = true 表示一點不差的走道目的點
	//	   = false 表示大約走道目的點
	bool	Target( float X , float Y , float Z , bool IsUsePath = true );
	bool	Target( float TX , float TY , float TZ , float NowX , float NowY , float NowZ );

	//記算有多少個移動點( 撿查是否要移動太久)
	int		CountNode( float TX , float TY , float TZ );

	//取得下一點的位置
	int		NextPoint( float &NX , float &NY , float &NZ , float Leng = 120 );

	//設定目前位置
	float	SetPos( float x , float y , float z );
	void	SetPos_NoCheckHeight( float x , float y , float z );

	//丟一部路徑倒堆疊中
	void	PushPos( float x , float y , float z , INT64 NodeID );

	//檢查堆疊內還有幾步
	int		StackCount( ){ return (int)_Stack.size(); }

	//清除所搜尋的路徑
	void	ClearPath(){ _Stack.clear(); _UseCount = 0; }

	//取得目地點
	bool	GetTargetPoint( float& x , float& y , float& z );

	//取最後一次的搜尋位置
	POINT3D*	LastSearch(){return &_LastSearchPos;};

	//全長度
	float	TotalLength();
	float	TotalLength3D();

	//目前重堆疊內取了幾個資料
	short	UseCount(){ return _UseCount; };

	//路徑收尋的步數
	int		LastSearchStepCount() { return _LastSearchStepCount;	};

	//測障礙
	bool	CheckLine( float x , float y , float z , float x1 , float y1 , float z1 );
	bool	CheckLine_Pure( float x , float y , float z , float x1 , float y1 , float z1 );
	bool	CheckLine_Magic( float x , float y , float z , float x1 , float y1 , float z1 );
	//位置名稱
	static char* MapName( float x , float y , float z );

	//取得主要區域
	static int	MainMapID( float x , float y , float z );
	

	//位置名稱沒有豆號
	static char* MapName1( float x , float y , float z );

	std::vector<POINT3D>*	FindNeighborPoint( float X , float Y , float Z , float Radius, int Divisions );

	//路徑搜尋
	std::vector<POINT3D>*	FindPath( float X , float Y , float Z  );

	int	FindPathNode( float X , float Y , float Z );

	//找重生點
	bool	FindZoneRespawnPoint( float X , float Y , float Z , POINT3D& Point );


	static int MinCtrlX() {return _MinCtrlX;}
	static int MinCtrlZ() {return _MinCtrlZ;}
	static int MaxCtrlX() {return _MaxCtrlX;}
	static int MaxCtrlZ() {return _MaxCtrlZ;}

	static bool IsDeadZone( float X , float Y , float Z);
	static int  GetZoneType(float X , float Y , float Z , float* damageFactorOut);


	static int FindMapID( float X , float Y , float Z );
	static bool CheckMapID( float X , float Y , float Z , int MapID );
};
//--------------------------------------------------
#endif