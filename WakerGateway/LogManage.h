#pragma  once
//#include "GlobalDef.h"
#include <memory>
#include <map>

using namespace std;

#define _DEF_MAX_PACKET_ID_	5000

struct PacketInfoBase
{
	int PGID;
	int	Size;
	int	Count;
};
class PacketInfoManage
{
public:
	int		ProcCount;
	//¨C¬í¸ê°T
	int		PGCount;	
	int		PGSize;	
	int		PGMaxCount;
	int		PGMaxSize;
	int		PGMinCount;
	int		PGMinSize;
	int		PGTotalCount;
	int		PGTotalSize;

	PacketInfoBase		PGIDInfo[_DEF_MAX_PACKET_ID_];
	PacketInfoBase		PGTop10[10];

		
	PacketInfoManage();
	void	Reset( );
	void	AddPacket( int pgid , int pgsize , int count = 1 );
	void	Process( );
	void	CalPGIDSize( );

};

struct NetStatusStruct
{
	int		MinSrvSecPgCount;
	int		MinSrvSecPgSize;
	int		MaxSrvSecPgCount;
	int		MaxSrvSecPgSize;
	int		AvgSrvSecPgCount;
	int		AvgSrvSecPgSize;

	int		MinSrvSrvSecPgCount;
	int		MinSrvSrvSecPgSize;
	int		MaxSrvSrvSecPgCount;
	int		MaxSrvSrvSecPgSize;
	int		AvgSrvSrvSecPgCount;
	int		AvgSrvSrvSecPgSize;

	int		MinCliSecPgCount;
	int		MinCliSecPgSize;
	int		MaxCliSecPgCount;
	int		MaxCliSecPgSize;
	int		AvgCliSecPgCount;
	int		AvgCliSecPgSize;

	void	Init()
	{
		memset( this , 0 , sizeof(*this));
	}

};