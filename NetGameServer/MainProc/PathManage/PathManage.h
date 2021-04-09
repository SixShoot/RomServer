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

//���|���M���G
enum	PathSearchResultENUM
{
	ENUM_Path_Search_End		=	0	,		//���ت� �ε����j�M
	ENUM_Path_Search_Wait		=	1	,		//�]�t�θ귽�γ]�w�ɭP�Ȱ�
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
	//�Ȯɤ��B�z���|�j�M(�ΨӮɶ�����)
	static bool				_PathFindPauseFlag;

	//�Ȯɤ��B�z���|�j�M(GM����)
	static bool				_GMPathFindPauseFlag;

	static int				_MinCtrlX , _MinCtrlZ;
	static int				_MaxCtrlX , _MaxCtrlZ;

	int						_RoomID;

	//�ثe��m
    POINT3D                 _Pos;

	//�ؼЦ�m
    POINT3D                 _TargetPos;

	//�̫�@���j�M���ؼ�
    POINT3D                 _LastSearchPos;
		

	//�ثe�����|�����F�X�Ӹ��
	short					_UseCount;

	//�s����|�����c
    std::vector<POINT3D>	_Stack;

	//���|���M���B��
	int						_LastSearchStepCount;

	//��l�� 
	static bool				_IsReady;

	static bool				_IsUsePathFind;

public:
	//��ƥX�l��
	static bool InitBase( const char* FilePath , const char* File );
	static bool Init( const char* FilePath , const char* File , int MapID , bool IsNeedPathMaker = false );
	static bool Init( const char* FilePath , const char* File , int MinX , int MinZ , int MaxX , int MaxZ , bool IsNeedPathMaker = false );
	static bool Release( );

	//��l��
	PathManageClass();
    ~PathManageClass(){};

	//�Ȯɤ��B�z���|�j�M
	static	void PathFindPauseFlag( bool F ){ _PathFindPauseFlag = F|_GMPathFindPauseFlag; }
	static	bool PathFindPauseFlag( ){ return _PathFindPauseFlag; }

	static	void GMPathFindPauseFlag( bool F ){ _GMPathFindPauseFlag = F; }
	static	bool GMPathFindPauseFlag( ){ return _GMPathFindPauseFlag; }

	//�M��Y�@�I�����I
	bool 	Height( float x , float y , float z ,float &Outy, float H );

	//�}�����|���M
	static void		IsUsePathFind( bool Value );
	static bool		IsUsePathFind( ) { return _IsUsePathFind; };

	
	//���|�Գ��B�~���|
	static bool		Room_CreatePath( int RoomID );
	static bool		Room_ReleasePath( int RoomID );
	static bool		Room_AddModel( int RoomID , float X , float Y , float Z , float Dir , const char* ModelStr );
	static bool		Room_BuildPath( int RoomID );

	static DWORD WINAPI _BuildPathThreadFuction( void* RoomID );	

	//�]�w�Ҧb�ж�
	void	RoomID( int Value ) { _RoomID = Value; };

	//�p�G���Z���j�M����^��false	
	//Lock = true ��ܤ@�I���t�����D�ت��I
	//	   = false ��ܤj�����D�ت��I
	bool	Target( float X , float Y , float Z , bool IsUsePath = true );
	bool	Target( float TX , float TY , float TZ , float NowX , float NowY , float NowZ );

	//�O�⦳�h�֭Ӳ����I( �߬d�O�_�n���ʤӤ[)
	int		CountNode( float TX , float TY , float TZ );

	//���o�U�@�I����m
	int		NextPoint( float &NX , float &NY , float &NZ , float Leng = 120 );

	//�]�w�ثe��m
	float	SetPos( float x , float y , float z );
	void	SetPos_NoCheckHeight( float x , float y , float z );

	//��@�����|�˰��|��
	void	PushPos( float x , float y , float z , INT64 NodeID );

	//�ˬd���|���٦��X�B
	int		StackCount( ){ return (int)_Stack.size(); }

	//�M���ҷj�M�����|
	void	ClearPath(){ _Stack.clear(); _UseCount = 0; }

	//���o�ئa�I
	bool	GetTargetPoint( float& x , float& y , float& z );

	//���̫�@�����j�M��m
	POINT3D*	LastSearch(){return &_LastSearchPos;};

	//������
	float	TotalLength();
	float	TotalLength3D();

	//�ثe�����|�����F�X�Ӹ��
	short	UseCount(){ return _UseCount; };

	//���|���M���B��
	int		LastSearchStepCount() { return _LastSearchStepCount;	};

	//����ê
	bool	CheckLine( float x , float y , float z , float x1 , float y1 , float z1 );
	bool	CheckLine_Pure( float x , float y , float z , float x1 , float y1 , float z1 );
	bool	CheckLine_Magic( float x , float y , float z , float x1 , float y1 , float z1 );
	//��m�W��
	static char* MapName( float x , float y , float z );

	//���o�D�n�ϰ�
	static int	MainMapID( float x , float y , float z );
	

	//��m�W�٨S������
	static char* MapName1( float x , float y , float z );

	std::vector<POINT3D>*	FindNeighborPoint( float X , float Y , float Z , float Radius, int Divisions );

	//���|�j�M
	std::vector<POINT3D>*	FindPath( float X , float Y , float Z  );

	int	FindPathNode( float X , float Y , float Z );

	//�䭫���I
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