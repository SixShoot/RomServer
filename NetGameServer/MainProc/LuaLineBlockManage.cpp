#include "basefunction/BaseFunction.h"
#include "LuaLineBlockManage.h"


void	LineBlockManageClass::AddLine( int id , float x1 , float y1 , float x2 , float y2 )
{
	LineSegmentStruct line;
	line.ID = id;
	line.X[0] = x1;
	line.X[1] = x2;
	line.Y[0] = y1;
	line.Y[1] = y2;

	_LineMap[id] = line;
}
bool	LineBlockManageClass::DelLine( int id )
{	
	map< int , LineSegmentStruct >::iterator iter = _LineMap.find( id );
	if( iter == _LineMap.end() )
		return false;

	_LineMap.erase( iter );
	return true;

}
bool	LineBlockManageClass::CheckCollion( float x1 , float y1 , float x2 , float y2 )
{
	map< int , LineSegmentStruct >::iterator iter;
	for( iter = _LineMap.begin() ; iter != _LineMap.end() ; iter++ )
	{
		LineSegmentStruct& l = iter->second;
		if( CheckTwoLineIntersect( l.X[0] , l.Y[0] , l.X[1] , l.Y[1] , x1 , y1 , x2 , y2 ) )
			return true;
	}

	return false;
}

void	LineBlockManageClass::Clear()
{
	_LineMap.clear();
}