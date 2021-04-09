#include "LogManage.h"



PacketInfoManage::PacketInfoManage()
{
	Reset();
}
void	PacketInfoManage::Reset( )
{
	ProcCount = 0;
	PGCount = 0;	
	PGSize = 0;	
	PGMaxCount = 0;
	PGMaxSize = 0;
	PGMinCount  = 100000000;
	PGMinSize	= 100000000;
	PGTotalCount = 0;
	PGTotalSize = 0;

	memset( PGIDInfo , 0 , sizeof(PGIDInfo) );
	memset( PGTop10 , 0 , sizeof(PGTop10) );
	for( int i = 0 ; i < _DEF_MAX_PACKET_ID_ ; i++ )
	{
		PGIDInfo[i].PGID = i;
	}
}
void	PacketInfoManage::AddPacket( int pgid , int pgsize , int count )
{
	PGTotalCount+= count;
	PGTotalSize += pgsize * count;

	PGCount += count;	
	PGSize += pgsize * count;	

	if( (unsigned)pgid >= _DEF_MAX_PACKET_ID_ )
		return;

	PGIDInfo[pgid].Size += pgsize * count;
	PGIDInfo[pgid].Count+= count;
}
void	PacketInfoManage::Process( )
{
	if( PGCount > PGMaxCount )
		PGMaxCount = PGCount;

	if( PGSize > PGMaxSize )
		PGMaxSize = PGSize;

	if( PGCount < PGMinCount )
		PGMinCount = PGCount;

	if( PGSize < PGMinSize )
		PGMinSize = PGSize;

	PGCount = 0;	
	PGSize = 0;	

}

void	PacketInfoManage::CalPGIDSize( )
{
	map< int , PacketInfoBase > pgInfoMap;
	for( int i = 0 ; i < _DEF_MAX_PACKET_ID_ ; i++ )
	{
		PacketInfoBase& info = PGIDInfo[i];
		if( info.Size == 0 )
			continue;

		pgInfoMap[ info.Size*-1 ] = info;
	}

	memset( PGTop10 , 0 , sizeof( PGTop10 ) );
	map< int , PacketInfoBase >::iterator iter = pgInfoMap.begin();
	for( int i = 0 ; i < 10 && iter != pgInfoMap.end() ; iter++ , i++ )
	{
		PGTop10[i] = iter->second;
	}
}

