#include "../Global.h"
#include "GroupZoneChannel.h"

bool GroupZoneChannel::Init()
{
	//(�Ȯ�)�إ� 100 �Ӱϰ�
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


