#pragma once

#include <vector>
#include <string>
using namespace std;
//-------------------------------------------------------------------------------------------------------------
enum    SrvCmpStateENUM
{
    SrvCmpState_New     = 0 ,
    SrvCmpState_Del         ,
    SrvCmpState_Move        ,

    SrvCmpState_MAX = 0xffffffff,
};

struct SrvInfoStruct
{
    int     SrvID;
	int		SubMapID;

    SrvCmpStateENUM     TempCmpState;           //比較資訊暫時的值

    SrvInfoStruct()
    {
        Init();
    }
    void Init()
    {
        SrvID = -1;
    }
};
//-------------------------------------------------------------------------------------------------------------
class SrvPartitionProc
{

    unsigned int    _Width;
    unsigned int    _Height;
	unsigned int	_BlockSize;
	int		_MinX , _MinY;

	std::string	_MapFileName;
	
    vector < SrvInfoStruct* >       _SrvInfoList;
    vector < SrvInfoStruct* >**     _ViewPartition;
    SrvInfoStruct***                _CtrlPartition;

    vector<SrvInfoStruct*>          _RetList ;

public:
    SrvPartitionProc( const char* MapFileName );
    ~SrvPartitionProc( );

	//SubMapID 此區域使用圖檔的區塊
    bool Add( int SrvID , int SubMapID );

    bool Del( int SrvID );

	unsigned int Width( )	{ return _Width;  }
	unsigned int Height( )	{ return _Height; }
	int	MinX() { return _MinX;}
	int	MinY() { return _MinY;}

    SrvInfoStruct*          GetSrv( int SrvID );
    //使用真實的位置
    vector<SrvInfoStruct*>* GetViewSrv( int X , int Y );
    SrvInfoStruct* GetCtrlSrv( int X , int Y );
    vector<SrvInfoStruct*>* CmpBlock( int OrgX , int OrgY , int DesX , int DesY ); 

};
//-------------------------------------------------------------------------------------------------------------
