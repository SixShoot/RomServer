#include "../Global.h"
#include "GroupZoneChannel.h"

bool GroupZoneChannel::Init()
{
	//(暫時)建立 100 個區域
	for( int i = 0 ; i < 10000 ; i++ )	
	{
		NEW(  GroupZoneChannel );
	}
	return true;
}

bool GroupZoneChannel::Release()
{
	return false;
}

GroupZoneChannel::GroupZoneChannel():GroupObjectClass( EM_GroupObjectType_SysZoneChannel )
{

}

GroupZoneChannel::~GroupZoneChannel()
{
	
}


