#include "../NetWakerGSrvInc.h"
#include "NetSrv_BotChild.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_BotChild::Init()
{
    NetSrv_Bot::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_BotChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_BotChild::Release()
{
    if( This == NULL )
        return false;

    NetSrv_Bot::_Release();

    delete This;
    This = NULL;

    return true;
    
}
//-----------------------------------------------------------------
void	NetSrv_BotChild::R_RequestMovePath( BaseItemObject* Obj , float Range , float X , float  Y , float Z )
{
	RoleDataEx* owner = Obj->Role();

	if( owner->PlayerBaseData->ManageLV < EM_Management_GameVisitor )
		return;

	//Range = Range / 3 + rand() % int( Range * 2/3 );
/*	std::vector<POINT3D>& PointList	= *( Obj->Path()->FindNeighborPoint( X , Y , Z , Range , 10 ) );

	if( PointList.size() == 0 )
	{
		S_MovePoint( Obj->GUID() , 0 , -1 , 0 , 0, 0 );
		return;
	}
	*/
	POINT3D Point;
	Point.x = X - Range + rand() % int( Range  *2 );
	Point.z = Z - Range + rand() % int( Range  *2 );
	Point.y = Y;
	Obj->Path()->Height( Point.x , Point.y , Point.z , Point.y , 100 );


	//POINT3D Point = PointList[ rand() % PointList.size() ];
	/*

	std::vector<POINT3D>& Path = *( Obj->Path()->FindPath( Point.x , Point.y , Point.z ) );

	if( Path.size() <= 1 )
	{
		S_MovePoint( Obj->GUID() , 0 , -1 , 0 , 0, 0 );
		return;
	}

	for( int i = 0 ; i < (int)Path.size() ; i++ )
	{
		S_MovePoint( Obj->GUID() , int( Path.size() ) , i , Path[i].x , Path[i].y , Path[i].z );
	}
	*/
	S_MovePoint( Obj->GUID() , 1 , 0 , Point.x , Point.y , Point.z );
	
}