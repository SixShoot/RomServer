#pragma  once
#include "GlobalDef.h"
#include <map>

using namespace std;

//²���u�q�I���˹�
class LineBlockManageClass
{
	map< int , LineSegmentStruct > _LineMap;
public:
	void	Clear();
	void	AddLine( int id , float x1 , float y1 , float x2 , float y2 );
	bool	DelLine( int id );
	bool	CheckCollion( float x1 , float y1 , float x2 , float y2 );
};